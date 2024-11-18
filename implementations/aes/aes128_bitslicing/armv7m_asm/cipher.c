#include "api.h"

#include "cipher.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

typedef struct param {
    uint32_t ctr;
    uint8_t nonce[12];
    uint8_t rk[2*11*16];
} param;

extern void AES_128_keyschedule(const uint8_t *, uint8_t *);
extern void AES_128_encrypt_ctr(param const *, const uint8_t *, uint8_t *, uint32_t);
#define AES_128_decrypt_ctr AES_128_encrypt_ctr


int crypto_encrypt(unsigned char *c,
                   const unsigned char *m, unsigned long long mlen,
                   const unsigned char *k)
{
    const uint32_t LEN = 32;
    const uint32_t LEN_ROUNDED = ((LEN+31)/32)*32;

    uint8_t key[16];
    uint8_t in[LEN];
    uint8_t out[LEN_ROUNDED];

    unsigned int i, j;
    char buffer[36];
    param p;

   AES_128_encrypt_ctr(&p, in, out, LEN);

  return 0;
}

int crypto_decrypt(unsigned char *m,
		const unsigned char *c, unsigned long long clen,
		const unsigned char *k)
{

  return 0;
}


