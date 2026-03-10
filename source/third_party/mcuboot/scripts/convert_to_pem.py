#!/usr/bin/env python3

import sys
from cryptography.hazmat.primitives import serialization
from cryptography.hazmat.primitives.asymmetric import ec
from cryptography.hazmat.backends import default_backend

def convert_binary_to_pem(input_file, output_file):
    # Read binary public key
    with open(input_file, 'rb') as f:
        binary_data = f.read()
    
    try:
        # Try to load as SubjectPublicKeyInfo (likely for ECDSA)
        public_key = serialization.load_der_public_key(binary_data, backend=default_backend())
        
        # Write PEM format
        pem_data = public_key.public_bytes(
            encoding=serialization.Encoding.PEM,
            format=serialization.PublicFormat.SubjectPublicKeyInfo
        )
        
        with open(output_file, 'wb') as f:
            f.write(pem_data)
        
        print(f"Successfully converted {input_file} to PEM format: {output_file}")
    
    except Exception as e:
        print(f"Error converting {input_file}: {str(e)}")
        print("The key might not be in DER SubjectPublicKeyInfo format.")
        # Try saving as raw binary for other key types (like MLDSA)
        with open(output_file, 'wb') as f:
            f.write(binary_data)
        print(f"Copied binary file to {output_file} without conversion")

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage: python convert_key.py input_binary.bin output.pem")
        sys.exit(1)
    
    convert_binary_to_pem(sys.argv[1], sys.argv[2])