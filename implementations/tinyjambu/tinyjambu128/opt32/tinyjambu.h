#ifndef TINYJAMBU_H
#define TINYJAMBU_H

#include <stdint.h>

// TinyJAMBU frame bits
#define FrameBitsIV 0x10
#define FrameBitsAD 0x30
#define FrameBitsPC 0x50 // Framebits for plaintext/ciphertext
#define FrameBitsFinalization 0x70

// TinyJAMBU round constants
#define NROUND1 128 * 5
#define NROUND2 128 * 8

// Function prototypes
void state_update(unsigned int *state, const unsigned char *key, unsigned int number_of_steps);
void initialization(const unsigned char *key, const unsigned char *iv, unsigned int *state);
void process_ad(const unsigned char *k, const unsigned char *ad, unsigned long long adlen, unsigned int *state);

// Cipher encrypt and decrypt functions
int cipher_encrypt(unsigned char *c, const unsigned char *m, unsigned long long mlen, const unsigned char *k);
int cipher_decrypt(unsigned char *m, const unsigned char *c, unsigned long long clen, const unsigned char *k);

#endif /* TINYJAMBU_H */
