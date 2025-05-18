/*
AES-128 Implementation
Based on FIPS 197
*/

#ifndef AES128_H
#define AES128_H

#include <stdint.h>

// AES-128 encryption function (single block)
void aes128_encrypt(const uint8_t plaintext[16], const uint8_t key[16], uint8_t ciphertext[16]);

// AES-128 decryption function (single block)
void aes128_decrypt(const uint8_t ciphertext[16], const uint8_t key[16], uint8_t plaintext[16]);

#endif /* AES128_H */