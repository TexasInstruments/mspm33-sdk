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
    from dilithium_py.ml_dsa import ML_DSA_87
    PUBLIC_KEY_SIZE = 2592
    SECRET_KEY_SIZE = 4896
except ImportError:
    print("Error: dilithium_py package not found.")
    print("To install: pip install dilithium_py")
    sys.exit(1)

class HSM_MLDSA:
    def __init__(self, key_dir=None):
        self.key_dir = key_dir or os.path.join(os.path.expanduser("~"), ".hsm_mldsa")
        os.makedirs(self.key_dir, exist_ok=True)
        self.private_key_path = os.path.join(self.key_dir, "hsm_mldsa_key.prv")
        self.public_key_path = os.path.join(self.key_dir, "hsm_mldsa_key.pub")
        self.private_key = None
        self.public_key = None
        
    def generate_keys(self, force=False, key_id="default"):
        if not force and os.path.exists(self.private_key_path) and os.path.exists(self.public_key_path):
            print(f"Using existing keys from {self.key_dir}")
            return self.load_keys()
            
        print("Generating new MLDSA keypair...")
        public_key, private_key = ML_DSA_87.keygen()
        
        with open(self.private_key_path, 'wb') as f:
            f.write(private_key)
        with open(self.public_key_path, 'wb') as f:
            f.write(public_key)
            
        print(f"Keys generated and saved")
        
        self.private_key = private_key
        self.public_key = public_key
        
        return public_key, private_key
        
    def load_keys(self, key_id="default"):
        if not os.path.exists(self.private_key_path) or not os.path.exists(self.public_key_path):
            raise FileNotFoundError(f"Keys not found")
            
        with open(self.private_key_path, 'rb') as f:
            private_key = f.read()
        with open(self.public_key_path, 'rb') as f:
            public_key = f.read()
            
        if len(private_key) != SECRET_KEY_SIZE:
            raise ValueError(f"Invalid private key size: {len(private_key)}")
        if len(public_key) != PUBLIC_KEY_SIZE:
            raise ValueError(f"Invalid public key size: {len(public_key)}")
            
        self.private_key = private_key
        self.public_key = public_key
        
        return public_key, private_key
    
    def get_public_key(self, key_id="default"):
        if self.public_key is None:
            if os.path.exists(self.public_key_path):
                with open(self.public_key_path, 'rb') as f:
                    self.public_key = f.read()
            else:
                raise ValueError("Public key not available")
                
        return self.public_key
    
    def calculate_key_hash(self, key_id="default"):
        public_key = self.get_public_key(key_id)
        sha = hashlib.sha256()
        sha.update(public_key)
        return sha.digest()
        
    def sign_data(self, data, key_id="default"):
        if self.private_key is None:
            self.load_keys(key_id)
            
        print(f"Signing data ({len(data)} bytes)")
        signature = ML_DSA_87.sign(self.private_key, data)
        print(f"Signature generated: {len(signature)} bytes")
        
        return signature
        
    def sign_file(self, input_file, key_id="default"):
        with open(input_file, 'rb') as f:
            data = f.read()
            
        return self.sign_data(data, key_id)
    
    def verify_signature(self, signature, data, key_id="default"):
        public_key = self.get_public_key(key_id)
        result = ML_DSA_87.verify(public_key, data, signature)
        return result
        
    def verify_file_signature(self, signature, input_file, key_id="default"):
        with open(input_file, 'rb') as f:
            data = f.read()
            
        return self.verify_signature(signature, data, key_id)

    def generate_c_header(self, output_file, array_name="mldsa_pub_key"):
        """Generate C header file with public key"""
        public_key = self.get_public_key()
        
        with open(output_file, 'w') as f:
            f.write("/* MLDSA public key */\n")
            f.write("#ifndef MLDSA_PUBLIC_KEY_H\n")
            f.write("#define MLDSA_PUBLIC_KEY_H\n\n")
            f.write("#include <stdint.h>\n\n")
            f.write("/* MLDSA-87 public key */\n")
            f.write(f"const unsigned char {array_name}[] = {{\n    ")
            
            # Write key bytes in hex format (16 per line)
            for i, b in enumerate(public_key):
                f.write(f"0x{b:02x}")
                if i < len(public_key) - 1:
                    f.write(", ")
                if (i + 1) % 16 == 0 and i < len(public_key) - 1:
                    f.write("\n    ")
                    
            f.write("\n};\n\n")
            f.write(f"const unsigned int {array_name}_len = {len(public_key)};\n\n")
            f.write("#endif /* MLDSA_PUBLIC_KEY_H */\n")
            
        return output_file


def main():
    parser = argparse.ArgumentParser(description="HSM Simulator for MLDSA")
    subparsers = parser.add_subparsers(dest="command", help="Command to execute")
    
    # Sign command
    sign_parser = subparsers.add_parser("sign", help="Sign a binary file")
    sign_parser.add_argument('input_file', help="Binary file to sign")
    sign_parser.add_argument('--key-dir', help="Directory for key storage", default=None)
    sign_parser.add_argument('--output-dir', help="Output directory", default=".")
    sign_parser.add_argument('--output-prefix', help="Prefix for output files", default=None)
    sign_parser.add_argument('--generate-keys', action='store_true', help="Generate new keys")
    sign_parser.add_argument('--show-command', action='store_true', help="Show command for imgtool")
    
    # Verify command
    verify_parser = subparsers.add_parser("verify", help="Verify a signature")
    verify_parser.add_argument('input_file', help="Binary file that was signed")
    verify_parser.add_argument('--signature', required=True, help="Signature file")
    verify_parser.add_argument('--key-dir', help="Directory for key storage", default=None)
    
    # Pubkey command
    pubkey_parser = subparsers.add_parser("pubkey", help="Export public key and hash")
    pubkey_parser.add_argument('--key-dir', help="Directory for key storage", default=None)
    pubkey_parser.add_argument('--output-dir', help="Output directory", default=".")
    pubkey_parser.add_argument('--output-prefix', help="Prefix for output files", default="mldsa")
    pubkey_parser.add_argument('--generate-keys', action='store_true', help="Generate new keys")
    
    args = parser.parse_args()
    
    if not args.command:
        parser.print_help()
        return 1
    
    try:
        # Initialize the HSM simulator
        hsm = HSM_MLDSA(args.key_dir)
        
        # SIGN command
        if args.command == "sign":
            if not os.path.exists(args.input_file):
                print(f"Error: Input file {args.input_file} not found")
                return 1
                
            os.makedirs(args.output_dir, exist_ok=True)
            
            if args.output_prefix is None:
                args.output_prefix = os.path.splitext(os.path.basename(args.input_file))[0]
            
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
                f.write(hsm.get_public_key())
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
            
            hsm.generate_keys(force=args.generate_keys)
            key_hash = hsm.calculate_key_hash()
            
            output_base = os.path.join(args.output_dir, args.output_prefix)
            public_key_path = f"{output_base}_pub.bin"
            key_hash_path = f"{output_base}_hash.bin"
            c_header_path = f"{output_base}_pub.h"
            
            with open(public_key_path, 'wb') as f:
                f.write(hsm.get_public_key())
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