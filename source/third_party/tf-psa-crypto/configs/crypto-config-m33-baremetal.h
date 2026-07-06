/**
 * \file crypto-config-m33-baremetal.h
 *
 * \brief Crypto configuration for bare-metal Cortex-M33 (ARMv8-M)
 *
 * This configuration is suitable for embedded systems without an OS.
 * It disables features that require OS support like filesystem I/O,
 * time functions, and built-in entropy sources.
 */
/*
 *  Copyright The Mbed TLS Contributors
 *  SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
 */

#ifndef PSA_CRYPTO_CONFIG_H
#define PSA_CRYPTO_CONFIG_H

#define TF_PSA_CRYPTO_CONFIG_VERSION 0x01000000

/**
 * \name SECTION: Cryptographic mechanism selection (PSA API)
 * \{
 */

/* Symmetric ciphers */
#define PSA_WANT_ALG_CBC_NO_PADDING             1
#define PSA_WANT_ALG_CBC_PKCS7                  1
#define PSA_WANT_ALG_CCM                        1
#define PSA_WANT_ALG_CCM_STAR_NO_TAG            1
#define PSA_WANT_ALG_CMAC                       1
#define PSA_WANT_ALG_CFB                        1
#define PSA_WANT_ALG_CTR                        1
#define PSA_WANT_ALG_ECB_NO_PADDING             1
#define PSA_WANT_ALG_GCM                        1
#define PSA_WANT_ALG_OFB                        1
#define PSA_WANT_ALG_STREAM_CIPHER              1

/* ChaCha20-Poly1305 - comment out if not needed to save code size */
#define PSA_WANT_ALG_CHACHA20_POLY1305          1

/* Hash algorithms */
#define PSA_WANT_ALG_SHA_224                    1
#define PSA_WANT_ALG_SHA_256                    1
#define PSA_WANT_ALG_SHA_384                    1
#define PSA_WANT_ALG_SHA_512                    1
/* Uncomment if needed - increases code size */
//#define PSA_WANT_ALG_SHA_1                      1
//#define PSA_WANT_ALG_MD5                        1
//#define PSA_WANT_ALG_RIPEMD160                  1
//#define PSA_WANT_ALG_SHA3_224                   1
//#define PSA_WANT_ALG_SHA3_256                   1
//#define PSA_WANT_ALG_SHA3_384                   1
//#define PSA_WANT_ALG_SHA3_512                   1

/* MAC and KDF */
#define PSA_WANT_ALG_HMAC                       1
#define PSA_WANT_ALG_HKDF                       1
#define PSA_WANT_ALG_HKDF_EXTRACT               1
#define PSA_WANT_ALG_HKDF_EXPAND                1
#define PSA_WANT_ALG_TLS12_PRF                  1
#define PSA_WANT_ALG_TLS12_PSK_TO_MS            1
//#define PSA_WANT_ALG_PBKDF2_HMAC                1
//#define PSA_WANT_ALG_PBKDF2_AES_CMAC_PRF_128    1

/* Asymmetric - ECC */
#define PSA_WANT_ALG_ECDH                       1
#define PSA_WANT_ALG_ECDSA                      1
#define PSA_WANT_ALG_DETERMINISTIC_ECDSA        1
//#define PSA_WANT_ALG_JPAKE                      1

/* ECC curves - enable only what you need to save code size */
#define PSA_WANT_ECC_SECP_R1_256                1
//#define PSA_WANT_ECC_SECP_R1_384                1
//#define PSA_WANT_ECC_SECP_R1_521                1
//#define PSA_WANT_ECC_SECP_K1_256                1
//#define PSA_WANT_ECC_BRAINPOOL_P_R1_256         1
//#define PSA_WANT_ECC_BRAINPOOL_P_R1_384         1
//#define PSA_WANT_ECC_BRAINPOOL_P_R1_512         1
//#define PSA_WANT_ECC_MONTGOMERY_255             1
//#define PSA_WANT_ECC_MONTGOMERY_448             1

/* Asymmetric - RSA (comment out to save significant code size) */
//#define PSA_WANT_ALG_RSA_OAEP                   1
//#define PSA_WANT_ALG_RSA_PKCS1V15_CRYPT         1
//#define PSA_WANT_ALG_RSA_PKCS1V15_SIGN          1
//#define PSA_WANT_ALG_RSA_PSS                    1

/* Asymmetric - FFDH (comment out to save code size) */
//#define PSA_WANT_ALG_FFDH                       1
//#define PSA_WANT_DH_RFC7919_2048                1
//#define PSA_WANT_DH_RFC7919_3072                1
//#define PSA_WANT_DH_RFC7919_4096                1

/* Key types */
#define PSA_WANT_KEY_TYPE_DERIVE                1
#define PSA_WANT_KEY_TYPE_PASSWORD              1
#define PSA_WANT_KEY_TYPE_HMAC                  1
#define PSA_WANT_KEY_TYPE_AES                   1
#define PSA_WANT_KEY_TYPE_CHACHA20              1
#define PSA_WANT_KEY_TYPE_RAW_DATA              1
#define PSA_WANT_KEY_TYPE_ECC_PUBLIC_KEY        1
#define PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_BASIC    1
#define PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_IMPORT   1
#define PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_EXPORT   1
#define PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_GENERATE 1
//#define PSA_WANT_KEY_TYPE_RSA_PUBLIC_KEY        1
//#define PSA_WANT_KEY_TYPE_RSA_KEY_PAIR_BASIC    1
//#define PSA_WANT_KEY_TYPE_RSA_KEY_PAIR_IMPORT   1
//#define PSA_WANT_KEY_TYPE_RSA_KEY_PAIR_EXPORT   1
//#define PSA_WANT_KEY_TYPE_DH_PUBLIC_KEY         1

/* Comment out to save code size */
//#define PSA_WANT_KEY_TYPE_ARIA                  1
//#define PSA_WANT_KEY_TYPE_CAMELLIA              1

/** \} name SECTION: Cryptographic mechanism selection (PSA API) */

/**
 * \name SECTION: Platform abstraction layer
 *
 * Configuration for bare-metal embedded systems
 * \{
 */

/* Disable filesystem I/O - not available on bare-metal */
//#define MBEDTLS_FS_IO

/* Disable time functions - not available on bare-metal without RTC */
//#define MBEDTLS_HAVE_TIME
//#define MBEDTLS_HAVE_TIME_DATE

/* Enable platform abstraction layer */
#define MBEDTLS_PLATFORM_C

/* Use custom memory allocator if needed */
//#define MBEDTLS_PLATFORM_MEMORY
//#define MBEDTLS_MEMORY_BUFFER_ALLOC_C

/* Disable standard functions - provide your own or use macros */
//#define MBEDTLS_PLATFORM_NO_STD_FUNCTIONS

/* Custom platform functions - uncomment and implement as needed */
//#define MBEDTLS_PLATFORM_EXIT_ALT
//#define MBEDTLS_PLATFORM_FPRINTF_ALT
//#define MBEDTLS_PLATFORM_PRINTF_ALT
//#define MBEDTLS_PLATFORM_SNPRINTF_ALT

/** \} name SECTION: Platform abstraction layer */

/**
 * \name SECTION: General configuration options
 * \{
 */

/* Enable self-test functions for validation */
//#define MBEDTLS_SELF_TEST

/* Version information */
#define TF_PSA_CRYPTO_VERSION

/** \} name SECTION: General configuration options */

/**
 * \name SECTION: Cryptographic mechanism selection (extended API)
 * \{
 */

/* Disable LMS - large code size */
//#define MBEDTLS_LMS_C

/* Enable MD wrapper if needed */
#define MBEDTLS_MD_C

/* Disable NIST key wrapping to save code size */
//#define MBEDTLS_NIST_KW_C

/* Enable PK layer for key handling */
#define MBEDTLS_PK_C
#define MBEDTLS_PK_PARSE_C
#define MBEDTLS_PK_WRITE_C
//#define MBEDTLS_PK_PARSE_EC_EXTENDED
//#define MBEDTLS_PK_PARSE_EC_COMPRESSED

/* Disable PKCS5 to save code size */
//#define MBEDTLS_PKCS5_C

/** \} name SECTION: Cryptographic mechanism selection (extended API) */

/**
 * \name SECTION: Data format support
 * \{
 */

#define MBEDTLS_ASN1_PARSE_C
#define MBEDTLS_ASN1_WRITE_C
#define MBEDTLS_BASE64_C

/* Disable PEM support to save code size on embedded */
//#define MBEDTLS_PEM_PARSE_C
//#define MBEDTLS_PEM_WRITE_C

/** \} name SECTION: Data format support */

/**
 * \name SECTION: PSA core
 * \{
 */

/* Enable CTR_DRBG for random number generation */
#define MBEDTLS_CTR_DRBG_C

/* Enable HMAC_DRBG as alternative */
#define MBEDTLS_HMAC_DRBG_C

/* Enable PSA Crypto API */
#define MBEDTLS_PSA_CRYPTO_C

/* IMPORTANT: Disable built-in entropy - not available on bare-metal */
/* You must provide your own entropy source via MBEDTLS_PSA_DRIVER_GET_ENTROPY */
//#define MBEDTLS_PSA_BUILTIN_GET_ENTROPY

/* Enable custom entropy driver - YOU MUST IMPLEMENT mbedtls_platform_get_entropy() */
#define MBEDTLS_PSA_DRIVER_GET_ENTROPY

/* Alternative: Use external RNG if your platform has a hardware RNG */
/* If enabled, implement mbedtls_psa_external_get_random() */
//#define MBEDTLS_PSA_CRYPTO_EXTERNAL_RNG

/* Disable persistent key storage - requires filesystem */
//#define MBEDTLS_PSA_CRYPTO_STORAGE_C
//#define MBEDTLS_PSA_ITS_FILE_C

/* Use static key slots to avoid dynamic memory allocation */
#define MBEDTLS_PSA_STATIC_KEY_SLOTS
//#define MBEDTLS_PSA_KEY_STORE_DYNAMIC

/* Set key slot count for static allocation */
#define MBEDTLS_PSA_KEY_SLOT_COUNT 16

/** \} name SECTION: PSA core */

/**
 * \name SECTION: Builtin drivers
 * \{
 */

/* Disable x86-specific optimizations */
//#define MBEDTLS_AESNI_C

/* Disable ARM Crypto Extensions - M33 doesn't have them */
//#define MBEDTLS_AESCE_C

/* Use ROM tables to save RAM */
#define MBEDTLS_AES_ROM_TABLES

/* Use fewer tables to save ROM */
#define MBEDTLS_AES_FEWER_TABLES

/* Use smaller SHA-256 implementation */
#define MBEDTLS_SHA256_SMALLER

/* Use smaller SHA-512 implementation */
#define MBEDTLS_SHA512_SMALLER

/* Enable assembly optimizations */
#define MBEDTLS_HAVE_ASM

/* ECP optimizations */
#define MBEDTLS_ECP_NIST_OPTIM

/* Reduce ECP memory usage */
#define MBEDTLS_ECP_WINDOW_SIZE            2
#define MBEDTLS_ECP_FIXED_POINT_OPTIM      0

/* Reduce MPI memory usage */
#define MBEDTLS_MPI_MAX_SIZE              256
#define MBEDTLS_MPI_WINDOW_SIZE            1

/** \} name SECTION: Builtin drivers */

#endif /* PSA_CRYPTO_CONFIG_H */