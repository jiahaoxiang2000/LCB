#include "api.h"
#include "cipher.h"
#include "aes128.h"
#include <string.h>

#define BLOCK_SIZE 16

// AES-128 CTR mode encryption (nonce is all zeros for now)
int cipher_encrypt(unsigned char *c, const unsigned char *m, unsigned long long mlen, const unsigned char *k)
{
    unsigned char buffer[BLOCK_SIZE];
    unsigned char block_out[BLOCK_SIZE];
    unsigned char nonce[8] = {0};
    uint64_t counter = 0;
    uint64_t blocks = (mlen + BLOCK_SIZE - 1) / BLOCK_SIZE;

    for (uint64_t i = 0; i < blocks; i++)
    {
        memcpy(buffer, nonce, 8);
        buffer[8] = (counter >> 56) & 0xFF;
        buffer[9] = (counter >> 48) & 0xFF;
        buffer[10] = (counter >> 40) & 0xFF;
        buffer[11] = (counter >> 32) & 0xFF;
        buffer[12] = (counter >> 24) & 0xFF;
        buffer[13] = (counter >> 16) & 0xFF;
        buffer[14] = (counter >> 8) & 0xFF;
        buffer[15] = counter & 0xFF;

        aes128_encrypt(buffer, k, block_out);

        uint64_t block_size = (i == blocks - 1 && mlen % BLOCK_SIZE != 0) ? mlen % BLOCK_SIZE : BLOCK_SIZE;
        for (uint64_t j = 0; j < block_size; j++)
        {
            c[i * BLOCK_SIZE + j] = m[i * BLOCK_SIZE + j] ^ block_out[j];
        }
        counter++;
    }
    return 0;
}

// Decryption is identical to encryption in CTR mode
int cipher_decrypt(unsigned char *m, const unsigned char *c, unsigned long long clen, const unsigned char *k)
{
    return cipher_encrypt(m, c, clen, k);
}
