## Example Summary

This example demonstrates how to use the SHA-512 hash function from the
MSP Crypto Library. SHA-512 is a member of the SHA-2 family of hash functions
defined in FIPS 180-4, producing a 512-bit (64-byte) message digest.

## Peripherals & Pin Assignments

N/A

## BoosterPacks, Board Resources & Jumper Settings

N/A

## Example Usage

Compile, load, and run the example. The example computes the SHA-512 hash
of a test message using a NIST CAVP test vector (128-bit message length).

The computed hash is compared against the expected hash value. If they match,
the program enters an infinite loop at the pass condition. If the hash
computation fails or the hashes don't match, the program enters an infinite
loop at the error condition.

Use a debugger to verify the program reaches the pass condition loop.
