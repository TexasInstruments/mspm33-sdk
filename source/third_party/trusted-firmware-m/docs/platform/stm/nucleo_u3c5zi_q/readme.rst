-------
STM32U3
-------

TF-M is supported on STM32U3xx series

https://www.st.com/en/microcontrollers-microprocessors/stm32u3-series.html

Directory content
^^^^^^^^^^^^^^^^^

- stm/common/stm32u3xx/hal:
   Content from https://github.com/STMicroelectronics/stm32u3xx-hal-driver (HAL version - Tags V1.3.0 )

- stm/common/stm32u3xx/Device:
   Content from https://github.com/STMicroelectronics/cmsis-device-u3

- stm/common/stm32u3xx/secure:
   stm32u3xx Secure porting adaptation from https://github.com/STMicroelectronics/STM32CubeU3.git

- stm/common/stm32u3xx/boards:
   Adaptation and tools specific to stm32 board using stm32u3xx device from https://github.com/STMicroelectronics/STM32CubeU3.git

- stm/common/stm32u3xx/CMSIS_Driver:
   Flash and uart driver for stm32u3xx platform

- stm/common/stm32u3xx/Native_Driver:
   Random generator and tickless implementation

Specific Software Requirements
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

STM32CubeProgrammer is required.(see https://www.st.com/en/development-tools/stm32cubeprog.html)


Limitations to Consider When Using the Platform
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

TF-M Supported without BL1/BL2.

By default, TFM_OTP_DEFAULT_PROVISIONING and DEFAULT_SHARED_DATA switches are enabled in TF-M,
to use dummy data in OTP and SRAM shared_data areas as BL2 not supported.


NUCLEO-U3C5ZI-Q
^^^^^^^^^^^^^^^

STM32 Nucleo-144 development board with STM32U3C5ZIQ.
https://www.st.com/en/evaluation-tools/nucleo-u3c5zi-q.html

Configuration and Build
"""""""""""""""""""""""

GNUARM/ARMCLANG/IARARM compilation is available for this target.
and build the selected configuration as follow.

The build configuration for TF-M is provided to the build system using command
line arguments. Required arguments are noted below.

The following instructions build multi-core TF-M with regression test suites
in Isolation Level 1.

.. code-block:: bash


    cd <TF-M base folder>
    cd <trusted-firmware-m folder>

    git clone https://git.trustedfirmware.org/TF-M/tf-m-tests.git
    git checkout <recommended tf-m-tests commit>

    mkdir build_s && cd build_s

    cmake -S /../tf-m-tests/tests_reg/spe -B . -GNinja -DTFM_PLATFORM=stm/nucleo_u3c5zi_q
         -DTFM_TOOLCHAIN_FILE= /../toolchain_ARMCLANG.cmake
         -DCONFIG_TFM_SOURCE_PATH= /../trusted-firmware-m
         -DTFM_PSA_API=ON -DTFM_ISOLATION_LEVEL=1
         -DTEST_S=ON -DTEST_NS=ON

    cmake --build -C . install -j 8

    cd <trusted-firmware-m folder>
    mkdir build_ns && cd build_ns
    cmake -S /../trusted-firmware-m  /../tf-m-tests/tests_reg -B . -GNinja
         -DCONFIG_SPE_PATH= /../build_s/api_ns -DTFM_TOOLCHAIN_FILE= /../build_s/api_ns/cmake/toolchain_ns_ARMCLANG.cmake

    cmake --build -C . -j 8

The following instructions build multi-core TF-M with PSA API test suite for
the attestation service in Isolation Level 1 on Linux.

.. code-block:: bash


    cd <TF-M base folder>
    cd <trusted-firmware-m folder>

    git clone https://git.trustedfirmware.org/TF-M/tf-m-tests.git
    git checkout <recommended tf-m-tests commit>

    mkdir build_s && cd build_s

    cmake -S /../tf-m-tests/tests_psa_arch/spe -B . -GNinja -DTFM_PLATFORM=stm/nucleo_u3c5zi_q
         -DTFM_TOOLCHAIN_FILE= /../toolchain_ARMCLANG.cmake
         -DCONFIG_TFM_SOURCE_PATH= /../trusted-firmware-m
         -DTFM_PSA_API=ON -DTFM_ISOLATION_LEVEL=1
         -DTEST_PSA_API=INITIAL_ATTESTATION

    cmake --build -C . install -j 8

    cd <trusted-firmware-m folder>
    mkdir build_ns && cd build_ns
    cmake -S /../trusted-firmware-m  /../tf-m-tests/tests_psa_arch -B . -GNinja
         -DCONFIG_SPE_PATH= /../build_s/api_ns -DTFM_TOOLCHAIN_FILE= /../build_s/api_ns/cmake/toolchain_ns_ARMCLANG.cmake

    cmake --build -C . -j 8


Write software on target
^^^^^^^^^^^^^^^^^^^^^^^^
In build folder (api_ns):

  - ``postbuild.sh``: Updates regression.sh and TFM_UPDATE.sh scripts according to flash_layout.h
  - ``regression.sh``: Erases flash and configures option bytes on target
  - ``TFM_UPDATE.sh``: Programs TFM secure and non-secure appli, and provisions target

Connects board to ST-Link USB, then executes the 3 scripts in following order, to program the target:
postbuild.sh, regression.sh, TFM_UPDATE.sh

The virtual com port from STLINK is used for TFM Applications log and serial port configuration should be:

  - Baud rate    = 115200
  - Data         = 8 bits
  - Parity       = none
  - Stop         = 1 bit
  - Flow control = none

In case of target power-off, the TFM_UPDATE.sh script must be run to program bl2_shared_data RAM area again.

-------------

*Copyright (c) 2026, STMicroelectronics. All rights reserved.*
*SPDX-License-Identifier: BSD-3-Clause*
