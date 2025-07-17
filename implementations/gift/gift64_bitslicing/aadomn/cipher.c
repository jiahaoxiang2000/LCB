#include "gift64.h"
#include "api.h"
#include <string.h>

static u32 rkey[56];

int cipher_encrypt(unsigned char *c, const unsigned char *m, unsigned long long mlen, const unsigned char *k)
{

    // // Convert key to u32 array and rearrange
    // here we to test the CTR MODE, not to consider the key schedule.
    // gift64_rearrange_key(rkey, k);
    // giftb64_keyschedule(rkey);

    // For single block encryption, we need to pad to 16 bytes for the parallel implementation
    u8 padded_in[16];
    u8 padded_out[16];

    memcpy(padded_in, m, 8);
    memset(padded_in + 8, 0, 8);

    // Use giftb64_encrypt_block for bitslicing
    gift64_encrypt_block(padded_out, rkey, padded_in, padded_in + 8);

    memcpy(c, padded_out, 8);

    return 0;
}

int cipher_decrypt(unsigned char *m, const unsigned char *c, unsigned long long clen, const unsigned char *k)
{
    // Note: This implementation only provides encryption
    // Decryption would need additional implementation
    return -1;
}