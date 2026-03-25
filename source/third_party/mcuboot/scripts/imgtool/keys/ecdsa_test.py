# SPDX-License-Identifier: Apache-2.0

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
Tests for ECDSA keys
"""


import io
import os.path
import sys
import tempfile
import unittest

from cryptography.exceptions import InvalidSignature
from cryptography.hazmat.primitives.asymmetric import ec
from cryptography.hazmat.primitives.hashes import SHA256

sys.path.insert(0, os.path.abspath(os.path.join(os.path.dirname(__file__), '../..')))

from imgtool.keys import load, ECDSA256P1, ECDSAUsageError

class EcKeyGeneration(unittest.TestCase):

    def setUp(self):
        self.test_dir = tempfile.TemporaryDirectory()

    def tname(self, base):
        return os.path.join(self.test_dir.name, base)

    def tearDown(self):
        self.test_dir.cleanup()

    def test_keygen(self):
        name1 = self.tname("keygen.pem")
        k = ECDSA256P1.generate()
        k.export_private(name1, b'secret')

        self.assertIsNone(load(name1))

        k2 = load(name1, b'secret')

        pubname = self.tname('keygen-pub.pem')
        k2.export_public(pubname)
        pk2 = load(pubname)

        # We should be able to export the public key from the loaded
        # public key, but not the private key.
        pk2.export_public(self.tname('keygen-pub2.pem'))
        self.assertRaises(ECDSAUsageError,
                pk2.export_private, self.tname('keygen-priv2.pem'))

    def test_emit(self):
        """Basic sanity check on the code emitters."""
        k = ECDSA256P1.generate()

        ccode = io.StringIO()
        k.emit_c_public(ccode)
        self.assertIn("ecdsa_pub_key", ccode.getvalue())
        self.assertIn("ecdsa_pub_key_len", ccode.getvalue())

        rustcode = io.StringIO()
        k.emit_rust_public(rustcode)
        self.assertIn("ECDSA_PUB_KEY", rustcode.getvalue())

    def test_emit_pub(self):
        """Basic sanity check on the code emitters."""
        pubname = self.tname("public.pem")
        k = ECDSA256P1.generate()
        k.export_public(pubname)

        k2 = load(pubname)

        ccode = io.StringIO()
        k2.emit_c_public(ccode)
        self.assertIn("ecdsa_pub_key", ccode.getvalue())
        self.assertIn("ecdsa_pub_key_len", ccode.getvalue())

        rustcode = io.StringIO()
        k2.emit_rust_public(rustcode)
        self.assertIn("ECDSA_PUB_KEY", rustcode.getvalue())

    def test_sig(self):
        k = ECDSA256P1.generate()
        buf = b'This is the message'
        sig = k.raw_sign(buf)

        # The code doesn't have any verification, so verify this
        # manually.
        k.key.public_key().verify(
                signature=sig,
                data=buf,
                signature_algorithm=ec.ECDSA(SHA256()))

        # Modify the message to make sure the signature fails.
        self.assertRaises(InvalidSignature,
                k.key.public_key().verify,
                signature=sig,
                data=b'This is thE message',
                signature_algorithm=ec.ECDSA(SHA256()))

if __name__ == '__main__':
    unittest.main()
