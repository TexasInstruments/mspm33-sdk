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

# this is a wrapper function to allow additional parsing and arguments such
# as a custom TLV with a hash to be added dynamically based on a key being 
# provided

import subprocess
import click
import json
import platform

@click.command()
@click.option('-s','--sdk', help='Path to the SDK to find additional scripts and tools as needed')
@click.option('-t','--toolchain',help='Path to the toolchain')
@click.option('-n','--name',help="name of project")
@click.option('-c','--color',help="color of the LED")
def make_binary(sdk,toolchain,name,color):
    # Select the proper default python executable depending on OS
    python_exe = "python3"
    if "Window" in platform.system():
        python_exe = "python"

    print("sdk:", sdk)
    print("toolchain:", toolchain)
    print("projectName:", name)

    f = open('../signingArgs.json')

    config = json.load(f)

    f.close()
    # config = {
    #     "version" : "1.0.0",
    #     "addKey" : True
    #     "privateKeyPath" : "/source/third_party/mcuboot/image_signTEST.pem"
    # }
    subprocess.run([toolchain+'/bin/tiarmobjcopy',name+'.out',
                '--output-target','binary','sample_image-unsigned.bin'],
                capture_output=True)


    if(type(config["version"]) is not list):
        versionList = [config["version"]]
    else:
        versionList = config["version"]

    retVal = config.get("slotSize")
    if(retVal is not None): 
        slotSize = retVal
    else:
        slotSize = 0x2800

    retVal = config.get("bankSize")
    if(retVal is not None): 
        bankSize = retVal
    else:
        bankSize = 0x20000
    
    retVal = config.get("offset")
    if(retVal is not None): 
        offset = retVal
    else:
        offset = 0x4800

    # iterate over all versions provided
    for ver in versionList:

        versionStr = ver.replace('.','_')

        imgtool_optional_args = ['--header-size','0x100','--align','4','--slot-size', hex(slotSize),
                '--pad','--version',ver,'--pad-header','--overwrite-only','--key',
                sdk+config["privateKeyPath"]]

        if "addKey" in config: 
            print("adding a key")
            imgtool_optional_args.extend(["--custom-tlv", "0xCC", "0xb93c72b74bc8487d29827005f80d635918f91bc22b14d7ed05714d58f96702dd"])
            keyFile = open("keyFile.bin", 'wb')
            keyArray = [0x603deb10, 0x15ca71be, 0x2b73aef0, 0x857d7781,
                        0x1f352c07, 0x3b6108d7, 0x2d9810a3, 0x0914dff4]
            keyString = ''.join(format(x, '08x') for x in keyArray)
            keyFile.write(bytearray.fromhex(keyString))
            keyFile.close()

        if "removeKey" in config:
            print("removing a key")
            imgtool_optional_args.extend(["--custom-tlv", "0xCD", "0x555555555555555555555555555555"])

        outFilename = 'sample_image_signed_' + hex(offset) + '_v'+versionStr+'_'+color

        imgtool_args = [python_exe,sdk+'/source/third_party/mcuboot/scripts/imgtool.py',
                    'sign'] + imgtool_optional_args + ['sample_image-unsigned.bin',
                        outFilename+'.bin']

        print(imgtool_args)
        proc = subprocess.run(imgtool_args, capture_output=True)
        print(proc)

        txtConversion_args = [python_exe,sdk+'/source/third_party/mcuboot/scripts/bin-to-ti-txt.py',
                            '-f',outFilename+'.bin','-o',outFilename+'.txt','--encrypt','-c',
                            outFilename+'_encrypted.txt','-s',hex(bankSize+offset)]

        proc = subprocess.run(txtConversion_args, capture_output=True)
        print(proc)

if __name__ == "__main__":
    make_binary()

