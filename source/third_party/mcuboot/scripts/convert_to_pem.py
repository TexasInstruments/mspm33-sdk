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