#! /usr/bin/env python3
#
# Copyright 2017-2020 Linaro Limited
# Copyright 2019-2021 Arm Limited
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

import re
import click
import getpass
import hashlib
import imgtool.keys as keys
import sys
from imgtool import image, imgtool_version
from imgtool.version import decode_version
from .keys import (
    RSAUsageError, ECDSAUsageError, Ed25519UsageError, X25519UsageError)

# Import MLDSA if available
try:
    from .keys.mldsa import MLDSAUsageError
    MLDSA_AVAILABLE = True
except ImportError:
    MLDSA_AVAILABLE = False

MIN_PYTHON_VERSION = (3, 6)
if sys.version_info < MIN_PYTHON_VERSION:
    sys.exit("Python %s.%s or newer is required by imgtool."
             % MIN_PYTHON_VERSION)


def gen_rsa2048(keyfile, passwd):
    keys.RSA.generate().export_private(path=keyfile, passwd=passwd)


def gen_rsa3072(keyfile, passwd):
    keys.RSA.generate(key_size=3072).export_private(path=keyfile,
                                                    passwd=passwd)


def gen_ecdsa_p256(keyfile, passwd):
    keys.ECDSA256P1.generate().export_private(keyfile, passwd=passwd)


def gen_ecdsa_p224(keyfile, passwd):
    print("TODO: p-224 not yet implemented")


def gen_ed25519(keyfile, passwd):
    keys.Ed25519.generate().export_private(path=keyfile, passwd=passwd)


def gen_x25519(keyfile, passwd):
    keys.X25519.generate().export_private(path=keyfile, passwd=passwd)


def gen_mldsa87(keyfile, passwd):
    if not MLDSA_AVAILABLE:
        raise click.UsageError("MLDSA support not available. Install dilithium_py package.")
    from imgtool.keys.mldsa import MLDSA
    # Always pass keyfile as private_path to generate
    key = MLDSA.generate(private_path=keyfile, passwd=passwd)
    return key  # Return key object for consistency with other keygen functions


valid_langs = ['c', 'rust']
keygens = {
    'rsa-2048':   gen_rsa2048,
    'rsa-3072':   gen_rsa3072,
    'ecdsa-p256': gen_ecdsa_p256,
    'ecdsa-p224': gen_ecdsa_p224,
    'ed25519':    gen_ed25519,
    'x25519':     gen_x25519,
}

# Add MLDSA to keygens if available
if MLDSA_AVAILABLE:
    keygens['mldsa-87'] = gen_mldsa87


def load_key(keyfile):
    # TODO: better handling of invalid pass-phrase
    key = keys.load(keyfile)
    if key is not None:
        return key
    passwd = getpass.getpass("Enter key passphrase: ").encode('utf-8')
    return keys.load(keyfile, passwd)


def get_password():
    while True:
        passwd = getpass.getpass("Enter key passphrase: ")
        passwd2 = getpass.getpass("Reenter passphrase: ")
        if passwd == passwd2:
            break
        print("Passwords do not match, try again")

    # Password must be bytes, always use UTF-8 for consistent
    # encoding.
    return passwd.encode('utf-8')


@click.option('-p', '--password', is_flag=True,
              help='Prompt for password to protect key')
@click.option('-t', '--type', metavar='type', required=True,
              type=click.Choice(keygens.keys()), prompt=True,
              help='{}'.format('One of: {}'.format(', '.join(keygens.keys()))))
@click.option('-k', '--key', metavar='filename', required=True)
@click.command(help='Generate pub/private keypair')
def keygen(type, key, password):
    password = get_password() if password else None
    keygens[type](key, password)


@click.option('-l', '--lang', metavar='lang', default=valid_langs[0],
              type=click.Choice(valid_langs))
@click.option('-k', '--key', metavar='filename', required=True)
@click.command(help='Dump public key from keypair')
def getpub(key, lang):
    key = load_key(key)
    if key is None:
        print("Invalid passphrase")
    elif lang == 'c':
        key.emit_c_public()
    elif lang == 'rust':
        key.emit_rust_public()
    else:
        raise ValueError("BUG: should never get here!")


@click.option('--minimal', default=False, is_flag=True,
              help='Reduce the size of the dumped private key to include only '
                   'the minimum amount of data required to decrypt. This '
                   'might require changes to the build config. Check the docs!'
              )
@click.option('-k', '--key', metavar='filename', required=True)
@click.command(help='Dump private key from keypair')
def getpriv(key, minimal):
    key = load_key(key)
    if key is None:
        print("Invalid passphrase")
    try:
        key.emit_private(minimal)
    except (RSAUsageError, ECDSAUsageError, Ed25519UsageError,
            X25519UsageError) as e:
        raise click.UsageError(e)
    # Handle MLDSA errors if available
    except MLDSAUsageError as e:
        raise click.UsageError(e)


@click.argument('imgfile')
@click.option('-k', '--key', metavar='filename', help='Primary verification key')
@click.option('--second-key', metavar='filename', help='Secondary verification key for hybrid verification')
@click.option('--hybrid-mode', type=click.Choice(['either', 'both']), default='both',
              help='''Hybrid verification mode:
                    'either' - verify with either key, 
                    'both' - require both keys to verify (default)''')
@click.command(help="Check that signed image can be verified by given key(s)")
def verify(key, second_key, hybrid_mode, imgfile):
    key_obj = load_key(key) if key else None
    second_key_obj = load_key(second_key) if second_key else None
    
    # If only second key is provided, swap them for convenience
    if second_key_obj is not None and key_obj is None:
        key_obj = second_key_obj
        second_key_obj = None
    
    # Call verify with the second key parameter if provided
    ret, version, digest = image.Image.verify(imgfile, key_obj, second_key_obj, hybrid_mode) if second_key_obj else image.Image.verify(imgfile, key_obj)
    
    if ret == image.VerifyResult.OK:
        print("Image was correctly validated")
        if second_key_obj:
            print(f"Hybrid verification mode: {hybrid_mode}")
        print("Image version: {}.{}.{}+{}".format(*version))
        print("Image digest: {}".format(digest.hex()))
        return
    elif ret == image.VerifyResult.INVALID_MAGIC:
        print("Invalid image magic; is this an MCUboot image?")
    elif ret == image.VerifyResult.INVALID_TLV_INFO_MAGIC:
        print("Invalid TLV info magic; is this an MCUboot image?")
    elif ret == image.VerifyResult.INVALID_HASH:
        print("Image has an invalid sha256 digest")
    elif ret == image.VerifyResult.INVALID_SIGNATURE:
        print("Invalid signature found")
    else:
        print("Unknown return code: {}".format(ret))
    sys.exit(1)


def validate_version(ctx, param, value):
    try:
        decode_version(value)
        return value
    except ValueError as e:
        raise click.BadParameter("{}".format(e))


def validate_security_counter(ctx, param, value):
    if value is not None:
        if value.lower() == 'auto':
            return 'auto'
        else:
            try:
                return int(value, 0)
            except ValueError:
                raise click.BadParameter(
                    "{} is not a valid integer. Please use code literals "
                    "prefixed with 0b/0B, 0o/0O, or 0x/0X as necessary."
                    .format(value))


def validate_header_size(ctx, param, value):
    min_hdr_size = image.IMAGE_HEADER_SIZE
    if value < min_hdr_size:
        raise click.BadParameter(
            "Minimum value for -H/--header-size is {}".format(min_hdr_size))
    return value


def get_dependencies(ctx, param, value):
    if value is not None:
        versions = []
        images = re.findall(r"\((\d+)", value)
        if len(images) == 0:
            raise click.BadParameter(
                "Image dependency format is invalid: {}".format(value))
        raw_versions = re.findall(r",\s*([0-9.+]+)\)", value)
        if len(images) != len(raw_versions):
            raise click.BadParameter(
                '''There's a mismatch between the number of dependency images
                and versions in: {}'''.format(value))
        for raw_version in raw_versions:
            try:
                versions.append(decode_version(raw_version))
            except ValueError as e:
                raise click.BadParameter("{}".format(e))
        dependencies = dict()
        dependencies[image.DEP_IMAGES_KEY] = images
        dependencies[image.DEP_VERSIONS_KEY] = versions
        return dependencies


class BasedIntParamType(click.ParamType):
    name = 'integer'

    def convert(self, value, param, ctx):
        try:
            return int(value, 0)
        except ValueError:
            self.fail('%s is not a valid integer. Please use code literals '
                      'prefixed with 0b/0B, 0o/0O, or 0x/0X as necessary.'
                      % value, param, ctx)


@click.argument('outfile')
@click.argument('infile')
@click.option('--custom-tlv', required=False, nargs=2, default=[],
              multiple=True, metavar='[tag] [value]',
              help='Custom TLV that will be placed into protected area. '
                   'Add "0x" prefix if the value should be interpreted as an '
                   'integer, otherwise it will be interpreted as a string. '
                   'Specify the option multiple times to add multiple TLVs.')
@click.option('-R', '--erased-val', type=click.Choice(['0', '0xff']),
              required=False,
              help='The value that is read back from erased flash.')
@click.option('-x', '--hex-addr', type=BasedIntParamType(), required=False,
              help='Adjust address in hex output file.')
@click.option('-L', '--load-addr', type=BasedIntParamType(), required=False,
              help='Load address for image when it should run from RAM.')
@click.option('-F', '--rom-fixed', type=BasedIntParamType(), required=False,
              help='Set flash address the image is built for.')
@click.option('--save-enctlv', default=False, is_flag=True,
              help='When upgrading, save encrypted key TLVs instead of plain '
                   'keys. Enable when BOOT_SWAP_SAVE_ENCTLV config option '
                   'was set.')
@click.option('-E', '--encrypt', metavar='filename',
              help='Encrypt image using the provided public key. '
                   '(Not supported in direct-xip or ram-load mode.)')
@click.option('--encrypt-keylen', default='128',
              type=click.Choice(['128','256']),
              help='When encrypting the image using AES, select a 128 bit or '
                   '256 bit key len.')
@click.option('-e', '--endian', type=click.Choice(['little', 'big']),
              default='little', help="Select little or big endian")
@click.option('--overwrite-only', default=False, is_flag=True,
              help='Use overwrite-only instead of swap upgrades')
@click.option('--boot-record', metavar='sw_type', help='Create CBOR encoded '
              'boot record TLV. The sw_type represents the role of the '
              'software component (e.g. CoFM for coprocessor firmware). '
              '[max. 12 characters]')
@click.option('-M', '--max-sectors', type=int,
              help='When padding allow for this amount of sectors (defaults '
                   'to 128)')
@click.option('--confirm', default=False, is_flag=True,
              help='When padding the image, mark it as confirmed (implies '
                   '--pad)')
@click.option('--pad', default=False, is_flag=True,
              help='Pad image to --slot-size bytes, adding trailer magic')
@click.option('-S', '--slot-size', type=BasedIntParamType(), required=True,
              help='Size of the slot. If the slots have different sizes, use '
              'the size of the secondary slot.')
@click.option('--pad-header', default=False, is_flag=True,
              help='Add --header-size zeroed bytes at the beginning of the '
                   'image')
@click.option('-H', '--header-size', callback=validate_header_size,
              type=BasedIntParamType(), required=True)
@click.option('--pad-sig', default=False, is_flag=True,
              help='Add 0-2 bytes of padding to ECDSA signature '
                   '(for mcuboot <1.5)')
@click.option('-d', '--dependencies', callback=get_dependencies,
              required=False, help='''Add dependence on another image, format:
              "(<image_ID>,<image_version>), ... "''')
@click.option('-s', '--security-counter', callback=validate_security_counter,
              help='Specify the value of security counter. Use the `auto` '
              'keyword to automatically generate it from the image version.')
@click.option('-v', '--version', callback=validate_version,  required=True)
@click.option('--align', type=click.Choice(['1', '2', '4', '8']),
              required=True)
@click.option('--public-key-format', type=click.Choice(['hash', 'full']),
              default='hash', help='In what format to add the public key to '
              'the image manifest: full key or hash of the key.')
@click.option('-k', '--key', metavar='filename')
@click.option('--key-hash', metavar='filename',
              help='Key hash file (alternative to specifying a key)')
@click.option('--external-signature', metavar='filename',
              help='Use signature from external file (e.g. from HSM) instead of generating one')
@click.option('--second-key', metavar='filename', 
              help='Secondary signing key for hybrid signing')
@click.option('--second-key-hash', metavar='filename',
              help='Secondary key hash file')
@click.option('--second-external-signature', metavar='filename',
              help='Use secondary signature from external file for hybrid signing')
@click.option('--hybrid-signing-mode', type=click.Choice(['either', 'both']), default='both',
              help='''Verification mode for hybrid signing:
                    'either' - either key must verify,
                    'both' - both keys must verify (default)''')
@click.command(help='''Create a signed or unsigned image\n
               INFILE and OUTFILE are parsed as Intel HEX if the params have
               .hex extension, otherwise binary format is used''')
def sign(key, key_hash, second_key, second_key_hash, external_signature, second_external_signature, 
         hybrid_signing_mode, public_key_format, align, version, pad_sig, header_size,
         pad_header, slot_size, pad, confirm, max_sectors, overwrite_only,
         endian, encrypt_keylen, encrypt, dependencies, load_addr, hex_addr, erased_val, save_enctlv, 
         security_counter, boot_record, custom_tlv, rom_fixed, infile, outfile):
    if confirm:
        # Confirmed but non-padded images don't make much sense, because
        # otherwise there's no trailer area for writing the confirmed status.
        pad = True
    img = image.Image(version=decode_version(version), header_size=header_size,
                     pad_header=pad_header, pad=pad, confirm=confirm,
                     align=int(align), slot_size=slot_size,
                     max_sectors=max_sectors, overwrite_only=overwrite_only,
                     endian=endian, load_addr=load_addr, rom_fixed=rom_fixed,
                     erased_val=erased_val, save_enctlv=save_enctlv,
                     security_counter=security_counter)
    img.load(infile)
    
    # Input validation for key combinations
    if key and key_hash:
        raise click.UsageError("Cannot specify both --key and --key-hash")
    if second_key and second_key_hash:
        raise click.UsageError("Cannot specify both --second-key and --second-key-hash")
    
    # Check external signature requirements
    if external_signature and not (key or key_hash):
        raise click.UsageError("--external-signature requires --key or --key-hash to be specified")
    if second_external_signature and not (second_key or second_key_hash):
        raise click.UsageError("--second-external-signature requires --second-key or --second-key-hash to be specified")
    
    # Load key hashes if provided
    primary_key_hash = None
    secondary_key_hash = None
    
    if key_hash:
        try:
            with open(key_hash, 'rb') as f:
                primary_key_hash = f.read()
            if len(primary_key_hash) != 32:  # SHA-256 hash is 32 bytes
                raise click.UsageError("Key hash must be 32 bytes")
            print(f"Loaded key hash from {key_hash}")
        except (IOError, OSError) as e:
            raise click.UsageError(f"Failed to read key hash: {e}")
    
    if second_key_hash:
        try:
            with open(second_key_hash, 'rb') as f:
                secondary_key_hash = f.read()
            if len(secondary_key_hash) != 32:  # SHA-256 hash is 32 bytes
                raise click.UsageError("Second key hash must be 32 bytes")
            print(f"Loaded secondary key hash from {second_key_hash}")
        except (IOError, OSError) as e:
            raise click.UsageError(f"Failed to read secondary key hash: {e}")
    
    # Load external signatures if provided
    external_sig = None
    second_external_sig = None
    
    if external_signature:
        try:
            with open(external_signature, 'rb') as f:
                external_sig = f.read()
            print(f"Loaded external signature from {external_signature}")
        except (IOError, OSError) as e:
            raise click.UsageError(f"Failed to read external signature: {e}")
    
    if second_external_signature:
        try:
            with open(second_external_signature, 'rb') as f:
                second_external_sig = f.read()
            print(f"Loaded secondary external signature from {second_external_signature}")
        except (IOError, OSError) as e:
            raise click.UsageError(f"Failed to read secondary external signature: {e}")
    
    # Load primary key
    key_obj = load_key(key) if key else None
    
    # Load second key if provided
    second_key_obj = load_key(second_key) if second_key else None
    
    # If second key is provided but first isn't, and no key hash, swap them
    if second_key_obj is not None and key_obj is None and not primary_key_hash:
        key_obj = second_key_obj
        second_key_obj = None
        
    # Load encryption key
    enckey = load_key(encrypt) if encrypt else None
    
    if enckey and key_obj:
        if ((isinstance(key_obj, keys.ECDSA256P1) and
             not isinstance(enckey, keys.ECDSA256P1Public))
                or (isinstance(key_obj, keys.RSA) and
                    not isinstance(enckey, keys.RSAPublic))):
            # FIXME
            raise click.UsageError("Signing and encryption must use the same "
                                  "type of key")
    
    # Apply padding to primary signature if requested
    if pad_sig and hasattr(key_obj, 'pad_sig'):
        key_obj.pad_sig = True
    
    # Apply padding to secondary signature if requested
    if pad_sig and second_key_obj is not None and hasattr(second_key_obj, 'pad_sig'):
        second_key_obj.pad_sig = True
        
    # Get list of custom protected TLVs from the command-line
    custom_tlvs = {}
    for tlv in custom_tlv:
        tag = int(tlv[0], 0)
        if tag in custom_tlvs:
            raise click.UsageError('Custom TLV %s already exists.' % hex(tag))
        if tag in image.TLV_VALUES.values():
            raise click.UsageError(
                'Custom TLV %s conflicts with predefined TLV.' % hex(tag))

        value = tlv[1]
        if value.startswith('0x'):
            if len(value[2:]) % 2:
                raise click.UsageError('Custom TLV length is odd.')
            custom_tlvs[tag] = bytes.fromhex(value[2:])
        else:
            custom_tlvs[tag] = value.encode('utf-8')
    
    # Print info about hybrid signing if enabled
    if second_key_obj is not None or secondary_key_hash is not None:
        print(f"Using hybrid signing with {hybrid_signing_mode} verification policy")
        if second_key:
            print(f"  - Secondary key: {second_key}")
        if second_key_hash:
            print(f"  - Using secondary key hash from file")
        print(f"  - Note: Verification policy must be configured in the bootloader")
    
    # Call create() with all the parameters
    img.create(key_obj, public_key_format, enckey, dependencies, boot_record,
              custom_tlvs, int(encrypt_keylen), second_key_obj, 
              external_sig=external_sig, second_external_sig=second_external_sig,
              key_hash=primary_key_hash, second_key_hash=secondary_key_hash)
    img.save(outfile, hex_addr)


@click.argument('outfile')
@click.argument('infile')
@click.option('-v', '--version', callback=validate_version, required=True)
@click.option('-H', '--header-size', callback=validate_header_size,
              type=BasedIntParamType(), required=True)
@click.option('-S', '--slot-size', type=BasedIntParamType(), required=True)
@click.option('--align', type=click.Choice(['1', '2', '4', '8']),
              default='1')
@click.option('--pad-header', default=False, is_flag=True,
              help='Add --header-size zeroed bytes at the beginning of the image')
@click.option('-e', '--endian', type=click.Choice(['little', 'big']),
              default='little', help="Select little or big endian")
@click.option('-L', '--load-addr', type=BasedIntParamType(), required=False,
              help='Load address for image when it should run from RAM.')
@click.option('-F', '--rom-fixed', type=BasedIntParamType(), required=False,
              help='Set flash address the image is built for.')
@click.command(help='Prepare image and generate hash for external signing')
def prepare_for_hsm(infile, outfile, version, header_size, slot_size, align, pad_header, endian, load_addr, rom_fixed):
    img = image.Image(version=decode_version(version),
                     header_size=header_size,
                     pad_header=pad_header,
                     align=int(align),
                     slot_size=slot_size,
                     endian=endian,
                     load_addr=load_addr,
                     rom_fixed=rom_fixed)
    
    img.load(infile)
    img.add_header(None, 0)
    
    # Calculate hash
    sha = hashlib.sha256()
    sha.update(img.payload)
    digest = sha.digest()
    
    # Save image with header
    img.save(outfile)
    print(f"Image with header saved to {outfile}")
    
    # Generate hash filename based on outfile
    # Remove .bin extension if present, then add _hash.bin
    base_name = outfile
    if base_name.endswith('.bin'):
        base_name = base_name[:-4]  # Remove .bin extension
    hash_path = f"{base_name}_hash.bin"
    
    # Save hash to file
    with open(hash_path, 'wb') as f:
        f.write(digest)
    print(f"Hash written to {hash_path}")
    print(f"Hash: {digest.hex()}")

class AliasesGroup(click.Group):

    _aliases = {
        "create": "sign",
    }

    def list_commands(self, ctx):
        cmds = [k for k in self.commands]
        aliases = [k for k in self._aliases]
        return sorted(cmds + aliases)

    def get_command(self, ctx, cmd_name):
        rv = click.Group.get_command(self, ctx, cmd_name)
        if rv is not None:
            return rv
        if cmd_name in self._aliases:
            return click.Group.get_command(self, ctx, self._aliases[cmd_name])
        return None


@click.command(help='Print imgtool version information')
def version():
    print(imgtool_version)


@click.command(cls=AliasesGroup,
               context_settings=dict(help_option_names=['-h', '--help']))
def imgtool():
    pass


imgtool.add_command(keygen)
imgtool.add_command(getpub)
imgtool.add_command(getpriv)
imgtool.add_command(verify)
imgtool.add_command(sign)
imgtool.add_command(version)
imgtool.add_command(prepare_for_hsm)


if __name__ == '__main__':
    imgtool()