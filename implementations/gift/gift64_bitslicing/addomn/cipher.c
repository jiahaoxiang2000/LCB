#include "api.h"
#include "gift64.h"
#include <string.h>

// Block size is 8 bytes (64 bits)
#define BLOCK_SIZE 8

// Implementation of cipher_encrypt using GIFT-64
int cipher_encrypt(unsigned char *c, const unsigned char *m, unsigned long long mlen, const unsigned char *k)
{
    unsigned char buffer[BLOCK_SIZE];
    unsigned char block_out[BLOCK_SIZE];
    u32 rkey[GIFT64_KEY_SCHEDULE_WORDS];
    
    // Generate round keys
    gift64_keyschedule(k, rkey);
    
    // Use ECB mode for simplicity (in practice, use a proper mode)
    unsigned long long blocks = (mlen + BLOCK_SIZE - 1) / BLOCK_SIZE;
    
    for (unsigned long long i = 0; i < blocks; i++) {
        // Prepare input block
        unsigned long long block_size = (i == blocks - 1 && mlen % BLOCK_SIZE != 0) ? mlen % BLOCK_SIZE : BLOCK_SIZE;
        memset(buffer, 0, BLOCK_SIZE);
        memcpy(buffer, m + i * BLOCK_SIZE, block_size);
        
        // Encrypt block
        gift64_encrypt_block(block_out, rkey, buffer);
        
        // Copy output
        memcpy(c + i * BLOCK_SIZE, block_out, block_size);
    }
    
    return 0;
}

// Implementation of cipher_decrypt using GIFT-64
int cipher_decrypt(unsigned char *m, const unsigned char *c, unsigned long long clen, const unsigned char *k)
{
    unsigned char buffer[BLOCK_SIZE];
    unsigned char block_out[BLOCK_SIZE];
    u32 rkey[GIFT64_KEY_SCHEDULE_WORDS];
    
    // Generate round keys
    gift64_keyschedule(k, rkey);
    
    // Use ECB mode for simplicity
    unsigned long long blocks = (clen + BLOCK_SIZE - 1) / BLOCK_SIZE;
    
    for (unsigned long long i = 0; i < blocks; i++) {
        // Prepare input block
        unsigned long long block_size = (i == blocks - 1 && clen % BLOCK_SIZE != 0) ? clen % BLOCK_SIZE : BLOCK_SIZE;
        memset(buffer, 0, BLOCK_SIZE);
        memcpy(buffer, c + i * BLOCK_SIZE, block_size);
        
        // Decrypt block
        gift64_decrypt_block(block_out, rkey, buffer);
        
        // Copy output
        memcpy(m + i * BLOCK_SIZE, block_out, block_size);
    }
    
    return 0;
}