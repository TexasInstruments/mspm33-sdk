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
"""
MLDSA key management
"""
import hashlib
import os.path
import sys
import base64

# Define the correct key sizes
PUBLIC_KEY_SIZE = 2592
SECRET_KEY_SIZE = 4896

# Import the ML-DSA implementation
try:
    from dilithium_py.ml_dsa import ML_DSA_87
    # Add the key size attributes to ML_DSA_87 if they don't exist
    if not hasattr(ML_DSA_87, 'PUBLIC_KEY_SIZE'):
        ML_DSA_87.PUBLIC_KEY_SIZE = PUBLIC_KEY_SIZE
    if not hasattr(ML_DSA_87, 'SECRET_KEY_SIZE'):
        ML_DSA_87.SECRET_KEY_SIZE = SECRET_KEY_SIZE
except ImportError:
    print("Warning: dilithium_py package not found. MLDSA support disabled.")
    print("To enable MLDSA, install the dilithium_py package.")

class MLDSAUsageError(Exception):
    pass

class MLDSA:
    """
    MLDSA signing and verification
    """
    def __init__(self, key, public_key=None):
        """
        Construct a MLDSA signing/verification context with the given key
        
        Args:
            key (bytes): The MLDSA private key
            public_key (bytes, optional): The corresponding public key
        """
        self.key = key
        self._public_key = public_key

    @staticmethod
    def generate(private_path=None, passwd=None):
        """
        Generate a new MLDSA keypair
        
        Args:
            private_path (str, optional): Path to save private key file
            passwd (bytes, optional): Password to protect the key
            
        Returns:
            MLDSA: A new MLDSA instance with the generated keys
        """
        public_key, secret_key = ML_DSA_87.keygen()
        mldsa_obj = MLDSA(secret_key, public_key)
        
        # If a path is provided, export both keys
        if private_path:
            # Export the private key
            mldsa_obj.export_private(private_path, passwd)
            
            # Also export the public key as C header
            base_name = os.path.splitext(private_path)[0]
            public_c_path = f"{base_name}_pub.h"
            public_bin_path = f"{base_name}_pub.bin"
            
            # Export public key to C header file
            with open(public_c_path, 'w') as f:
                f.write("/* MLDSA public key: */\n")
                f.write("const unsigned char mldsa_pub_key[] = {")
                for i, b in enumerate(public_key):
                    if i % 8 == 0:
                        f.write("\n    ")
                    else:
                        f.write(" ")
                    f.write("0x{:02x},".format(b))
                f.write("\n};\n")
                f.write("const unsigned int mldsa_pub_key_len = {};\n".format(len(public_key)))
            
            # Export public key to binary file
            with open(public_bin_path, 'wb') as f:
                f.write(public_key)
            
            print(f"Private key saved to: {private_path}")
            print(f"Public key C header saved to: {public_c_path}")
            print(f"Public key binary saved to: {public_bin_path}")
                
        return mldsa_obj

    def _get_secret(self):
        """Get the secret key as bytes"""
        return self.key

    def get_public_bytes(self):
        """
        Get the public key
        
        Returns:
            bytes: The MLDSA public key
            
        Raises:
            MLDSAUsageError: If public key is not available
        """
        if self._public_key is None:
            raise MLDSAUsageError("Public key not available - it must be provided separately for MLDSA")
        return self._public_key
        
    def export_public_bin(self, path):
        """
        Export the public key to a binary file
        
        Args:
            path (str): Path to save the public key in binary format
        """
        public_key = self.get_public_bytes()
        with open(path, 'wb') as f:
            f.write(public_key)

    def export_private(self, path, passwd=None):
        """Export the private key to a file"""
        with open(path, 'wb') as f:
            f.write(self.key)

    def sign(self, payload):
        """Sign a payload using MLDSA"""
        return ML_DSA_87.sign(self.key, payload)

    def verify(self, signature, payload):
        """Verify an MLDSA signature"""
        result = ML_DSA_87.verify(self.key, payload, signature)
        if not result:
            # Raise InvalidSignature exception to match other cryptographic implementations
            from cryptography.exceptions import InvalidSignature
            raise InvalidSignature("MLDSA signature verification failed")
        return result

    def sig_tlv(self):
        """Return the TLV type that should be used when creating a signature"""
        return "MLDSA87"

    def emit_c_public(self):
        """Generate a C header file containing the public key"""
        public = self.get_public_bytes()
        print("/* MLDSA public key: */")
        print("const unsigned char mldsa_pub_key[] = {", end='')
        for i, b in enumerate(public):
            if i % 8 == 0:
                print("\n    ", end='')
            else:
                print(" ", end='')
            print("0x{:02x},".format(b), end='')
        print("\n};")
        print("const unsigned int mldsa_pub_key_len = {};".format(len(public)))

    def emit_rust_public(self):
        """Generate Rust source containing the public key"""
        public = self.get_public_bytes()
        print("// MLDSA public key:")
        print("static MLDSA_PUB_KEY: &'static [u8] = &[", end='')
        for i, b in enumerate(public):
            if i % 8 == 0:
                print("\n    ", end='')
            else:
                print(" ", end='')
            print("0x{:02x},".format(b), end='')
        print("\n];")

    def emit_private(self, minimal=False):
        """Generate a C header file containing the private key"""
        if minimal:
            print("MLDSA minimal private key mode not implemented")
            return
        secret = self._get_secret()
        print("/* MLDSA private key: */")
        print("const unsigned char mldsa_priv_key[] = {", end='')
        for i, b in enumerate(secret):
            if i % 8 == 0:
                print("\n    ", end='')
            else:
                print(" ", end='')
            print("0x{:02x},".format(b), end='')
        print("\n};")
        print("const unsigned int mldsa_priv_key_len = {};".format(len(secret)))

class MLDSAPublic:
    """
    Verify using MLDSA
    """
    def __init__(self, key):
        """
        Construct a verifier with the given public key
        
        Args:
            key (bytes): The MLDSA public key
        """
        self.key = key

    def get_public_bytes(self):
        """
        Get the public key bytes
        Returns:
            bytes: The MLDSA public key
        """
        return self.key

    def verify(self, signature, payload):
        """Verify an MLDSA signature"""
        result = ML_DSA_87.verify(self.key, payload, signature)
        if not result:
            # Raise InvalidSignature exception to match other cryptographic implementations
            from cryptography.exceptions import InvalidSignature
            raise InvalidSignature("MLDSA signature verification failed")
        return result

    def sig_tlv(self):
        """Return the TLV type that should be used when verifying a signature"""
        return "MLDSA87"

    @staticmethod
    def load_from_file(path):
        """Load the public key from a file"""
        with open(path, 'rb') as f:
            # Use the PUBLIC_KEY_SIZE constant defined at the top
            data = f.read(PUBLIC_KEY_SIZE)
            if len(data) < PUBLIC_KEY_SIZE:
                raise MLDSAUsageError("Public key too short")
            return MLDSAPublic(data)

def load(keyfile, passwd=None):
    """Load a key from a file"""
    with open(keyfile, 'rb') as f:
        key_bytes = f.read()
    
    # Use the constants defined at the top
    if len(key_bytes) == SECRET_KEY_SIZE:
        # This is a private key
        # Try to load the associated public key
        base_name = os.path.splitext(keyfile)[0]
        public_key_path = f"{base_name}_pub.bin"
        
        # Check if the public key file exists
        public_key = None
        if os.path.exists(public_key_path):
            with open(public_key_path, 'rb') as f:
                public_key = f.read()
            print(f"Loaded associated public key from: {public_key_path}")
        else:
            print(f"Warning: Public key file not found at {public_key_path}")
            print(f"Some operations may fail without the public key")
            
        return MLDSA(key_bytes, public_key)
    elif len(key_bytes) == PUBLIC_KEY_SIZE:
        # This is a public key
        return MLDSAPublic(key_bytes)
    else:
        raise MLDSAUsageError(
            f"Key file has invalid size (got {len(key_bytes)} bytes, "
            f"expected {SECRET_KEY_SIZE} or {PUBLIC_KEY_SIZE})"
        )