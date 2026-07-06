/* Copyright (C) 2013 Henry S. Warren Jr. You are free to use, copy,
 * and distribute any of the code on this web site, whether modified
 * by you or not.
 */

#include "efi_soft_crc.h"

/* The standard polynomial, in reverse */
#define POLYNOMIAL 0xEDB88320

/* Algorithmic approach to CRC calculation: avoids lookup tables, slow. Byte
 * reversal is avoided by shifting the crc register right instead of left and
 * by using a reversed 32-bit word to represent the polynomial.
 *
 * Derived from work by Henry S. Warren Jr.
 */
uint32_t efi_soft_crc32_update(uint32_t old_crc32, const uint8_t *buf, size_t len)
{
    register uint32_t crc32 = ~old_crc32;
    uint32_t mask;

    for ( ; len; --len, ++buf)
    {
        crc32 ^= *buf;
        for (size_t i = 0; i < 8; ++i) {
            mask = -(crc32 & 1);
            crc32 = (crc32 >> 1) ^ (POLYNOMIAL & mask);
        }
    }

    return ~crc32;
}
