#include "api.h"
#include "cipher.h"
#include "gift128.h"
#include <string.h>

// Block size is 16 bytes (128 bits)
#define BLOCK_SIZE 16

// Implementation of ascon_encrypt using GIFT-128
int cipher_encrypt(unsigned char *c, const unsigned char *m, unsigned long long mlen, const unsigned char *k)
{
    unsigned char buffer[BLOCK_SIZE];
    unsigned char block_out[BLOCK_SIZE];
    // Default nonce (all zeros)
    unsigned char nonce[8] = {0};

    // Use counter mode for encryption
    uint64_t counter = 0;
    uint64_t blocks = (mlen + BLOCK_SIZE - 1) / BLOCK_SIZE; // Ceiling division

    for (uint64_t i = 0; i < blocks; i++)
    {
        // Prepare the counter block: nonce (8 bytes) + counter (8 bytes)
        memcpy(buffer, nonce, 8);

        // Encode counter in big-endian format
        buffer[8] = (counter >> 56) & 0xFF;
        buffer[9] = (counter >> 48) & 0xFF;
        buffer[10] = (counter >> 40) & 0xFF;
        buffer[11] = (counter >> 32) & 0xFF;
        buffer[12] = (counter >> 24) & 0xFF;
        buffer[13] = (counter >> 16) & 0xFF;
        buffer[14] = (counter >> 8) & 0xFF;
        buffer[15] = counter & 0xFF;

        // Encrypt the counter block
        giftb128(buffer, k, block_out);

        // XOR with plaintext
        uint64_t block_size = (i == blocks - 1 && mlen % BLOCK_SIZE != 0) ? mlen % BLOCK_SIZE : BLOCK_SIZE;
        for (uint64_t j = 0; j < block_size; j++)
        {
            c[i * BLOCK_SIZE + j] = m[i * BLOCK_SIZE + j] ^ block_out[j];
        }

        // Increment counter
        counter++;
    }

    return 0;
}

// Implementation of cipher_decrypt using GIFT-128
int cipher_decrypt(unsigned char *m, const unsigned char *c, unsigned long long clen, const unsigned char *k)
{
    // not used in this implementation
    return 0;
}