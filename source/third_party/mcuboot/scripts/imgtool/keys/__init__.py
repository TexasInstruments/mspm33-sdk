# Copyright 2017 Linaro Limited
# Copyright (c) 2025, Texas Instruments Incorporated
#
# SPDX-License-Identifier: Apache-2.0
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

"""
Cryptographic key management for imgtool.
"""

import os.path
from cryptography.hazmat.backends import default_backend
from cryptography.hazmat.primitives import serialization
from cryptography.hazmat.primitives.asymmetric.rsa import (
    RSAPrivateKey, RSAPublicKey)
from cryptography.hazmat.primitives.asymmetric.ec import (
    EllipticCurvePrivateKey, EllipticCurvePublicKey)
from cryptography.hazmat.primitives.asymmetric.ed25519 import (
    Ed25519PrivateKey, Ed25519PublicKey)
from cryptography.hazmat.primitives.asymmetric.x25519 import (
    X25519PrivateKey, X25519PublicKey)

from .rsa import RSA, RSAPublic, RSAUsageError, RSA_KEY_SIZES
from .ecdsa import ECDSA256P1, ECDSA256P1Public, ECDSAUsageError
from .ed25519 import Ed25519, Ed25519Public, Ed25519UsageError
from .x25519 import X25519, X25519Public, X25519UsageError

# Check for MLDSA support
try:
    from .mldsa import MLDSA, MLDSAPublic, MLDSAUsageError, load as mldsa_load
    MLDSA_AVAILABLE = True
except ImportError:
    MLDSA_AVAILABLE = False


class PasswordRequired(Exception):
    """Raised to indicate that the key is password protected, but a
    password was not specified."""
    pass


def load(path, passwd=None):
    """Try loading a key from the given path.  Returns None if the password wasn't specified."""
    
    # First, try to load as an MLDSA key if supported
    if MLDSA_AVAILABLE:
        try:
            # Check if file might be MLDSA key (simple size check)
            file_size = os.path.getsize(path)
            from .mldsa import ML_DSA_87
            if file_size in (ML_DSA_87.SECRET_KEY_SIZE, ML_DSA_87.PUBLIC_KEY_SIZE):
                return mldsa_load(path, passwd)
        except (ImportError, OSError, AttributeError):
            pass
        except Exception as e:
            if "MLDSA" in str(e):
                raise e
    
    # If not an MLDSA key or MLDSA not available, try standard formats
    try:
        with open(path, 'rb') as f:
            raw_pem = f.read()
    except:
        raise Exception(f"Unable to read key file: {path}")
        
    try:
        pk = serialization.load_pem_private_key(
                raw_pem,
                password=passwd,
                backend=default_backend())
    # Unfortunately, the crypto library raises unhelpful exceptions,
    # so we have to look at the text.
    except TypeError as e:
        msg = str(e)
        if "private key is encrypted" in msg:
            return None
        raise e
    except ValueError:
        # This seems to happen if the key is a public key, let's try
        # loading it as a public key.
        try:
            pk = serialization.load_pem_public_key(
                    raw_pem,
                    backend=default_backend())
        except ValueError:
            # If still failing, and MLDSA is available, try as MLDSA key again
            # without size restriction check
            if MLDSA_AVAILABLE:
                try:
                    return mldsa_load(path, passwd)
                except Exception:
                    pass
            raise Exception(f"Unable to parse key file: {path}")

    if isinstance(pk, RSAPrivateKey):
        if pk.key_size not in RSA_KEY_SIZES:
            raise Exception("Unsupported RSA key size: " + str(pk.key_size))
        return RSA(pk)
    elif isinstance(pk, RSAPublicKey):
        if pk.key_size not in RSA_KEY_SIZES:
            raise Exception("Unsupported RSA key size: " + str(pk.key_size))
        return RSAPublic(pk)
    elif isinstance(pk, EllipticCurvePrivateKey):
        if pk.curve.name != 'secp256r1':
            raise Exception("Unsupported EC curve: " + pk.curve.name)
        if pk.key_size != 256:
            raise Exception("Unsupported EC size: " + str(pk.key_size))
        return ECDSA256P1(pk)
    elif isinstance(pk, EllipticCurvePublicKey):
        if pk.curve.name != 'secp256r1':
            raise Exception("Unsupported EC curve: " + pk.curve.name)
        if pk.key_size != 256:
            raise Exception("Unsupported EC size: " + str(pk.key_size))
        return ECDSA256P1Public(pk)
    elif isinstance(pk, Ed25519PrivateKey):
        return Ed25519(pk)
    elif isinstance(pk, Ed25519PublicKey):
        return Ed25519Public(pk)
    elif isinstance(pk, X25519PrivateKey):
        return X25519(pk)
    elif isinstance(pk, X25519PublicKey):
        return X25519Public(pk)
    else:
        raise Exception("Unknown key type: " + str(type(pk)))