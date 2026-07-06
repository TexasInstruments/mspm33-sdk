#!/usr/bin/env python3

#   Copyright (C) 2026 Texas Instruments Incorporated - http://www.ti.com/
#
#   Redistribution and use in source and binary forms, with or without
#   modification, are permitted provided that the following conditions
#   are met:
#
#    Redistributions of source code must retain the above copyright
#    notice, this list of conditions and the following disclaimer.
#
#    Redistributions in binary form must reproduce the above copyright
#    notice, this list of conditions and the following disclaimer in the
#    documentation and/or other materials provided with the
#    distribution.
#
#    Neither the name of Texas Instruments Incorporated nor the names of
#    its contributors may be used to endorse or promote products derived
#    from this software without specific prior written permission.
#
#   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
#   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
#   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
#   A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
#   OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
#   SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
#   LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
#   DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
#   THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
#   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
#   OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.


import os
import sys
import hashlib
import argparse

try:
    from cryptography.hazmat.backends import default_backend
    from cryptography.hazmat.primitives import serialization
    from cryptography.hazmat.primitives.asymmetric import ec
    from cryptography.hazmat.primitives.hashes import SHA256
except ImportError:
    print("Error: cryptography package not found.")
    print("To install: pip install cryptography")
    sys.exit(1)

class HSM_ECDSA:
    def __init__(self, key_dir=None):
        self.key_dir = key_dir or os.path.join(os.path.expanduser("~"), ".hsm_ecdsa")
        os.makedirs(self.key_dir, exist_ok=True)
        self.private_key_path = os.path.join(self.key_dir, "hsm_ecdsa_key.prv")
        self.public_key_path = os.path.join(self.key_dir, "hsm_ecdsa_key.pub")
        self.private_key = None
        self.public_key = None
        self.pad_sig = False
        
    def generate_keys(self, force=False, key_id="default"):
        if not force and os.path.exists(self.private_key_path) and os.path.exists(self.public_key_path):
            print(f"Using existing keys from {self.key_dir}")
            return self.load_keys()
            
        print("Generating new ECDSA P-256 keypair...")
        private_key = ec.generate_private_key(
            ec.SECP256R1(),
            default_backend()
        )
        
        # Save private key to PEM file
        pem = private_key.private_bytes(
            encoding=serialization.Encoding.PEM,
            format=serialization.PrivateFormat.PKCS8,
            encryption_algorithm=serialization.NoEncryption()
        )
        with open(self.private_key_path, 'wb') as f:
            f.write(pem)
            
        # Save public key to PEM file
        public_key = private_key.public_key()
        pub_pem = public_key.public_bytes(
            encoding=serialization.Encoding.PEM,
            format=serialization.PublicFormat.SubjectPublicKeyInfo
        )
        with open(self.public_key_path, 'wb') as f:
            f.write(pub_pem)
            
        print(f"Keys generated and saved")
        
        self.private_key = private_key
        self.public_key = public_key
        
        return public_key, private_key
        
    def load_keys(self, key_id="default"):
        if not os.path.exists(self.private_key_path):
            raise FileNotFoundError(f"Private key not found")
            
        # Load private key
        with open(self.private_key_path, 'rb') as f:
            private_key_data = f.read()
            
        self.private_key = serialization.load_pem_private_key(
            private_key_data,
            password=None,
            backend=default_backend()
        )
        
        # Get public key from private key
        self.public_key = self.private_key.public_key()
        
        return self.public_key, self.private_key
    
    def get_public_key(self, key_id="default"):
        if self.public_key is None:
            self.load_keys(key_id)
                
        return self.public_key
    
    def get_public_bytes(self, key_id="default"):
        public_key = self.get_public_key(key_id)
        # Return DER encoded SubjectPublicKeyInfo format (what MCUboot expects)
        return public_key.public_bytes(
            encoding=serialization.Encoding.DER,
            format=serialization.PublicFormat.SubjectPublicKeyInfo
        )
    
    def calculate_key_hash(self, key_id="default"):
        public_key_bytes = self.get_public_bytes(key_id)
        sha = hashlib.sha256()
        sha.update(public_key_bytes)
        return sha.digest()
        
    def sign_data(self, data, key_id="default"):
        if self.private_key is None:
            self.load_keys(key_id)
            
        print(f"Signing data ({len(data)} bytes)")
        signature = self.private_key.sign(
            data=data,
            signature_algorithm=ec.ECDSA(SHA256())
        )
            
        print(f"Signature generated: {len(signature)} bytes")
        
        return signature
        
    def sign_file(self, input_file, key_id="default"):
        with open(input_file, 'rb') as f:
            data = f.read()
            
        return self.sign_data(data, key_id)
    
    def verify_signature(self, signature, data, key_id="default"):
        public_key = self.get_public_key(key_id)
        try:
            # Strip possible padding added during sign
            if len(signature) > 72:
                signature = signature[:72]
            
            # In case of DER signature, it should be shorter than 72 bytes
            # and can be parsed to determine actual length
            if signature[1] + 2 < len(signature):
                signature = signature[:signature[1] + 2]
                
            public_key.verify(
                signature=signature,
                data=data,
                signature_algorithm=ec.ECDSA(SHA256())
            )
            return True
        except Exception as e:
            print(f"Verification error: {str(e)}")
            return False
        
    def verify_file_signature(self, signature, input_file, key_id="default"):
        with open(input_file, 'rb') as f:
            data = f.read()
            
        return self.verify_signature(signature, data, key_id)

    def generate_c_header(self, output_file, array_name="ecdsa_pub_key"):
        """Generate C header file with public key"""
        public_key_bytes = self.get_public_bytes()
        
        with open(output_file, 'w') as f:
            f.write("/* ECDSA P-256 public key */\n")
            f.write("#ifndef ECDSA_PUBLIC_KEY_H\n")
            f.write("#define ECDSA_PUBLIC_KEY_H\n\n")
            f.write("#include <stdint.h>\n\n")
            f.write("/* ECDSA P-256 public key in DER format */\n")
            f.write(f"const unsigned char {array_name}[] = {{\n    ")
            
            # Write key bytes in hex format (16 per line)
            for i, b in enumerate(public_key_bytes):
                f.write(f"0x{b:02x}")
                if i < len(public_key_bytes) - 1:
                    f.write(", ")
                if (i + 1) % 16 == 0 and i < len(public_key_bytes) - 1:
                    f.write("\n    ")
                    
            f.write("\n};\n\n")
            f.write(f"const unsigned int {array_name}_len = {len(public_key_bytes)};\n\n")
            f.write("#endif /* ECDSA_PUBLIC_KEY_H */\n")
            
        return output_file


def main():
    parser = argparse.ArgumentParser(description="HSM Simulator for ECDSA")
    subparsers = parser.add_subparsers(dest="command", help="Command to execute")
    
    # Sign command
    sign_parser = subparsers.add_parser("sign", help="Sign a binary file")
    sign_parser.add_argument('input_file', help="Binary file to sign")
    sign_parser.add_argument('--key-dir', help="Directory for key storage", default=None)
    sign_parser.add_argument('--output-dir', help="Output directory", default=".")
    sign_parser.add_argument('--output-prefix', help="Prefix for output files", default=None)
    sign_parser.add_argument('--generate-keys', action='store_true', help="Generate new keys")
    sign_parser.add_argument('--show-command', action='store_true', help="Show command for imgtool")
    sign_parser.add_argument('--pad-sig', action='store_true', help="Pad signature to 72 bytes (for MCUboot < v1.5.0)")
    
    # Verify command
    verify_parser = subparsers.add_parser("verify", help="Verify a signature")
    verify_parser.add_argument('input_file', help="Binary file that was signed")
    verify_parser.add_argument('--signature', required=True, help="Signature file")
    verify_parser.add_argument('--key-dir', help="Directory for key storage", default=None)
    
    # Pubkey command
    pubkey_parser = subparsers.add_parser("pubkey", help="Export public key and hash")
    pubkey_parser.add_argument('--key-dir', help="Directory for key storage", default=None)
    pubkey_parser.add_argument('--output-dir', help="Output directory", default=".")
    pubkey_parser.add_argument('--output-prefix', help="Prefix for output files", default="ecdsa")
    pubkey_parser.add_argument('--generate-keys', action='store_true', help="Generate new keys")
    
    args = parser.parse_args()
    
    if not args.command:
        parser.print_help()
        return 1
    
    try:
        # Initialize the HSM simulator
        hsm = HSM_ECDSA(args.key_dir)
        
        # Set padding option if requested
        if args.command == "sign" and args.pad_sig:
            hsm.pad_sig = True
        
        # SIGN command
        if args.command == "sign":
            if not os.path.exists(args.input_file):
                print(f"Error: Input file {args.input_file} not found")
                return 1
                
            os.makedirs(args.output_dir, exist_ok=True)
            
            if args.output_prefix is None:
                base_name = os.path.splitext(os.path.basename(args.input_file))[0]
                args.output_prefix = f"{base_name}_ecdsa"
            elif not args.output_prefix.endswith("_ecdsa"):
                args.output_prefix = f"{args.output_prefix}_ecdsa"
            
            hsm.generate_keys(force=args.generate_keys)
            key_hash = hsm.calculate_key_hash()
            
            output_base = os.path.join(args.output_dir, args.output_prefix)
            signature_path = f"{output_base}_sig.bin"
            public_key_path = f"{output_base}_pub.bin"
            key_hash_path = f"{output_base}_hash.bin"
            c_header_path = f"{output_base}_pub.h"
            
            signature = hsm.sign_file(args.input_file)
            
            with open(signature_path, 'wb') as f:
                f.write(signature)
            print(f"Signature saved to: {signature_path}")
            
            with open(public_key_path, 'wb') as f:
                f.write(hsm.get_public_bytes())
            print(f"Public key saved to: {public_key_path}")
            
            with open(key_hash_path, 'wb') as f:
                f.write(key_hash)
            print(f"Key hash saved to: {key_hash_path}")
            
            # Generate C header file
            hsm.generate_c_header(c_header_path, f"{args.output_prefix}_pub_key")
            print(f"C header file saved to: {c_header_path}")
            
            if hsm.verify_file_signature(signature, args.input_file):
                print("Signature verified successfully")
            else:
                print("Signature verification failed")
                return 1
            
            if args.show_command:
                print("\nExample command for MCUboot's imgtool:")
                print(f"imgtool sign --key-hash {key_hash_path} --external-signature {signature_path} \\\n"
                    f"    --header-size 32 --align 8 --slot-size <SIZE> --version 1.0.0 \\\n"
                    f"    {args.input_file} <output_file>")
            
            return 0
        
        # VERIFY command
        elif args.command == "verify":
            if not os.path.exists(args.input_file):
                print(f"Error: Input file {args.input_file} not found")
                return 1
            
            if not os.path.exists(args.signature):
                print(f"Error: Signature file {args.signature} not found")
                return 1
                
            with open(args.signature, 'rb') as f:
                signature = f.read()
            
            print(f"Verifying signature...")
            if hsm.verify_file_signature(signature, args.input_file):
                print("Signature verified successfully")
                return 0
            else:
                print("Signature verification failed")
                return 1
        
        # PUBKEY command
        elif args.command == "pubkey":
            os.makedirs(args.output_dir, exist_ok=True)
            
            # Ensure output prefix includes ecdsa
            if not args.output_prefix.endswith("_ecdsa"):
                args.output_prefix = f"{args.output_prefix}_ecdsa"
            
            hsm.generate_keys(force=args.generate_keys)
            key_hash = hsm.calculate_key_hash()
            
            output_base = os.path.join(args.output_dir, args.output_prefix)
            public_key_path = f"{output_base}_pub.bin"
            key_hash_path = f"{output_base}_hash.bin"
            c_header_path = f"{output_base}_pub.h"
            
            with open(public_key_path, 'wb') as f:
                f.write(hsm.get_public_bytes())
            print(f"Public key saved to: {public_key_path}")
            
            with open(key_hash_path, 'wb') as f:
                f.write(key_hash)
            print(f"Key hash saved to: {key_hash_path}")
            
            # Generate C header file
            hsm.generate_c_header(c_header_path, f"{args.output_prefix}_pub_key")
            print(f"C header file saved to: {c_header_path}")
            
            return 0
    
    except Exception as e:
        print(f"Error: {str(e)}")
        return 1

if __name__ == "__main__":
    sys.exit(main())