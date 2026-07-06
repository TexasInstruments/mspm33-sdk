/**
 * \file config-m33-baremetal.h
 *
 * \brief Mbed TLS configuration for Cortex-M33 bare-metal (ARMv8-M)
 *
 * Enables TLS 1.2, X.509 certificate handling, and the PSA crypto API for
 * bare-metal embedded systems without an OS.
 *
 * In Mbed TLS 4.x, all crypto algorithm and primitive selection
 * (AES, SHA, ECC curves, BigNum, OID, etc.) is handled exclusively by
 * TF-PSA-Crypto.  Those macros (MBEDTLS_BIGNUM_C, MBEDTLS_ECP_C,
 * MBEDTLS_ECDSA_C, MBEDTLS_OID_C, MBEDTLS_ECP_DP_*, etc.) are internal
 * to TF-PSA-Crypto and MUST NOT be set in this file.
 *
 * PSA algorithm selection is controlled separately via TF_PSA_CRYPTO_CONFIG_FILE
 * which points to:
 *   tf-psa-crypto/configs/crypto-config-m33-baremetal.h
 *
 * This file controls only:
 *   - TLS protocol configuration
 *   - X.509 certificate handling
 *   - PSA key storage / entropy overrides
 *   - Bare-metal platform constraints (no FS, no OS time, no BSD sockets)
 *   - Software crypto tuning flags still owned by the mbedtls layer
 *
 * See README.txt for usage instructions.
 */
/*
 *  Copyright The Mbed TLS Contributors
 *  SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
 */

#ifndef MBEDTLS_CONFIG_M33_BAREMETAL_H
#define MBEDTLS_CONFIG_M33_BAREMETAL_H

/* ===========================================================================
 * SECTION: TLS Protocol Layer
 * =========================================================================*/

/** TLS core state machine - required for any TLS operation. */
#define MBEDTLS_SSL_TLS_C

/** TLS client role - required for outbound connections (e.g., IoT to cloud). */
#define MBEDTLS_SSL_CLI_C

/** TLS server role - required for inbound connections. */
#define MBEDTLS_SSL_SRV_C

/** TLS 1.2 protocol support. */
#define MBEDTLS_SSL_PROTO_TLS1_2

/**
 * Key exchange method for TLS 1.2.
 *
 * MBEDTLS_KEY_EXCHANGE_*_ENABLED must be defined when MBEDTLS_SSL_PROTO_TLS1_2
 * is enabled.  ECDHE-ECDSA provides forward secrecy (ephemeral ECDH) with
 * ECDSA authentication.  The underlying ECC, SHA, and AES primitives are
 * provided by TF-PSA-Crypto via the PSA crypto config.
 *
 * Uncomment additional ciphersuites as needed:
 */
#define MBEDTLS_KEY_EXCHANGE_ECDHE_ECDSA_ENABLED
/* #define MBEDTLS_KEY_EXCHANGE_ECDHE_RSA_ENABLED */
/* #define MBEDTLS_KEY_EXCHANGE_PSK_ENABLED        */

/**
 * Limit TLS record fragment size to reduce RAM usage.
 * Default is 16384 bytes - far too large for M33 bare-metal.
 * 4096 is a reasonable compromise for embedded TLS clients.
 * Requires the peer to support the max_fragment_length extension (RFC 6066).
 */
#define MBEDTLS_SSL_IN_CONTENT_LEN      4096
#define MBEDTLS_SSL_OUT_CONTENT_LEN     4096

/** TLS extensions for modern interoperability. */
#define MBEDTLS_SSL_EXTENDED_MASTER_SECRET
#define MBEDTLS_SSL_MAX_FRAGMENT_LENGTH
#define MBEDTLS_SSL_SERVER_NAME_INDICATION

/**
 * TLS session tickets (RFC 5077) for stateless session resumption.
 * Requires AES for ticket encryption (provided via PSA).
 * Comment out to save code size if session resumption is not needed.
 */
#define MBEDTLS_SSL_SESSION_TICKETS

/* ===========================================================================
 * SECTION: X.509 Certificate Handling
 * =========================================================================*/

/** Core X.509 parsing infrastructure. */
#define MBEDTLS_X509_USE_C

/** Parse X.509 certificates (DER format; PEM disabled below). */
#define MBEDTLS_X509_CRT_PARSE_C

/** Parse PKCS#10 certificate signing requests. */
#define MBEDTLS_X509_CSR_PARSE_C

/**
 * X.509 certificate and CSR creation (for devices that generate certs).
 * Comment out if the device only verifies certificates (saves code size).
 */
/* #define MBEDTLS_X509_CREATE_C  */
/* #define MBEDTLS_X509_CRT_WRITE_C */
/* #define MBEDTLS_X509_CSR_WRITE_C */

/* ===========================================================================
 * SECTION: PSA Crypto Runtime Configuration
 * =========================================================================*/

/**
 * Disable persistent key storage - requires a filesystem not available on
 * bare-metal.  All keys are volatile (lost on reset).
 */
/* #define MBEDTLS_PSA_CRYPTO_STORAGE_C */
/* #define MBEDTLS_PSA_ITS_FILE_C        */

/**
 * Disable built-in entropy collection (/dev/urandom - not on bare-metal).
 * The application MUST implement a hardware TRNG entropy source.
 * Typically wired to the MSP hardware RNG peripheral.
 */
/* #define MBEDTLS_PSA_BUILTIN_GET_ENTROPY */
#define MBEDTLS_PSA_DRIVER_GET_ENTROPY

/**
 * Use a static (pre-allocated) pool for PSA key slots instead of malloc.
 * Gives deterministic memory usage - important on constrained M33.
 * The slot count is configured in TF-PSA-Crypto (psa/crypto_config.h).
 */
#define MBEDTLS_PSA_STATIC_KEY_SLOTS

/* ===========================================================================
 * SECTION: Bare-metal Platform Constraints
 * =========================================================================*/

/**
 * Disable filesystem I/O - not available on bare-metal.
 * Keys and certificates must be loaded as DER (binary) byte arrays in flash.
 * Convert PEM to DER:
 *   openssl ec   -in key.pem  -out key.der  -outform DER
 *   openssl x509 -in cert.pem -out cert.der -outform DER
 */
/* #define MBEDTLS_FS_IO */

/**
 * Disable OS time functions - not available without RTC / RTOS.
 * Certificate validity (not_before / not_after) cannot be checked unless
 * the application provides a custom time callback.
 */
/* #define MBEDTLS_HAVE_TIME      */
/* #define MBEDTLS_HAVE_TIME_DATE */

/**
 * Disable built-in TCP/UDP networking - application provides the transport.
 * Pass read/write callbacks via mbedtls_ssl_set_bio().
 */
/* #define MBEDTLS_NET_C */

/* ===========================================================================
 * SECTION: PEM / Base64
 *
 * Disabled to save code size - use DER (binary) format on embedded targets.
 * =========================================================================*/
/* #define MBEDTLS_PEM_PARSE_C */
/* #define MBEDTLS_PEM_WRITE_C  */
/* #define MBEDTLS_BASE64_C     */

/* ===========================================================================
 * SECTION: Software Crypto Tuning (M33 specific)
 *
 * The following flags tune software implementations that are still owned by
 * the mbedtls layer (AES table layout, SHA variant selection, assembly).
 * They are valid in Mbed TLS 4.x and do NOT overlap with TF-PSA-Crypto
 * internal macros.
 * =========================================================================*/

/**
 * Enable inline assembly (Thumb-2 on Cortex-M33).
 * Used in bn_mul.h and other arithmetic primitives for performance.
 */
#define MBEDTLS_HAVE_ASM

/**
 * AES: store S-box lookup tables in ROM (flash) instead of RAM.
 * Saves ~6 KB of RAM at the cost of slightly slower AES if flash access is slow.
 */
#define MBEDTLS_AES_ROM_TABLES

/**
 * AES: use 1/4 of the tables (computed on-the-fly) to reduce ROM by ~6 KB.
 * Slight performance penalty - acceptable on M33 for the ROM savings.
 */
#define MBEDTLS_AES_FEWER_TABLES

/**
 * SHA-256: use a smaller, slightly slower implementation.
 * Approximately 30% performance reduction with significant code size saving.
 */
#define MBEDTLS_SHA256_SMALLER

/**
 * SHA-512: use a smaller implementation for embedded targets.
 */
#define MBEDTLS_SHA512_SMALLER

/* ===========================================================================
 * SECTION: Error Reporting
 * =========================================================================*/

/**
 * Human-readable error code strings (adds ~5 KB code, useful for debugging).
 * Comment out in production to save code size.
 */
#define MBEDTLS_ERROR_C

/**
 * Debug output - large code and RAM increase.
 * Uncomment only during development.
 */
/* #define MBEDTLS_DEBUG_C */

#endif /* MBEDTLS_CONFIG_M33_BAREMETAL_H */