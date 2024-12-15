#include "api.h"
#include "cipher.h"
#include <stdint.h>
#include <stdbool.h>

// Convert an 8-bit boolean array to a byte
void bit_to_byte(const bool *bits, uint8_t *byte) {
    *byte = 0;
    for (int i = 0; i < 8; i++) {
        *byte |= (bits[i] ? 1 : 0) << (7 - i);
    }
}

// Convert a byte to an 8-bit boolean array
void byte_to_bit(uint8_t byte, bool *bits) {
    for (int i = 0; i < 8; i++) {
        bits[i] = (byte >> (7 - i)) & 1;
    }
}

uint8_t cks[21][4] = {
        {0x13, 0x19, 0x8a, 0x2e},
        {0x03, 0x70, 0x73, 0x44},
        {0xa4, 0x09, 0x38, 0x22},
        {0x29, 0x9f, 0x31, 0xd0},
        {0x08, 0x2e, 0xfa, 0x98},
        {0xec, 0x4e, 0x6c, 0x89},
        {0x45, 0x28, 0x21, 0xe6},
        {0x38, 0xd0, 0x13, 0x77},
        {0xbe, 0x54, 0x66, 0xcf},
        {0x34, 0xe9, 0x0c, 0x6c},
        {0xc9, 0x7c, 0x50, 0xdd},
        {0xf4, 0x45, 0x25, 0xdb},
        {0xbe, 0x54, 0x66, 0xcf},
        {0xf8, 0x7c, 0x3a, 0xc0},
        {0x45, 0x28, 0x21, 0xe6},
        {0x2c, 0xe2, 0x45, 0x3e},
        {0x08, 0x2e, 0xfa, 0x98},
        {0xe9, 0x33, 0x18, 0x67},
        {0xa4, 0x09, 0x38, 0x22},
        {0xc3, 0xdc, 0x5a, 0xf3},
        {0x13, 0x19, 0x8a, 0x2e},
};

uint8_t sbox[16] = {0x0B, 0x0F, 0x03, 0x02, 0x0A, 0x0C, 0x09, 0x01, 0x06, 0x07, 0x08, 0x00, 0x0E, 0x05, 0x0D, 0x04};

void lllwbc_key_schedule(bool *key, bool (*kws)[64], bool (*krs)[32]) {
    // K = kw || ke

    // kw1 = kw
    for (int i = 0; i < 64; ++i) {
        kws[0][i] = key[i];
    }
    // kw2 = (kw >>> 1) ^ (kw >> 63)
    kws[1][0] = key[63];
    for (int i = 1; i < 63; ++i) {
        kws[1][i] = key[i - 1];
    }
    kws[1][63] = key[62] ^ key[0];

    // ke = K_1 || K_2
    // r = 1,3,...21 k^{r-1} = K_1 ^ ck^{r}
    // r = 2,4,...20 k^{r-1} = K_2 ^ ck^{r}
    bool temp[8] = {0};
    for (int i = 0; i < 10; ++i) {
        // 1,3,...19
        // r = i * 2
        for (int j = 0; j < 4; ++j) {
            // index = j * 8 + k
            byte_to_bit(cks[i * 2][j], temp);
            for (int k = 0; k < 8; ++k) {
                krs[i * 2][j * 8 + k] = key[64 + j * 8 + k] ^ temp[k];
            }
        }

        // 2,3,....20
        // r = i * 2 + 1
        for (int j = 0; j < 4; ++j) {
            // index = j * 8 + k
            byte_to_bit(cks[i * 2 + 1][j], temp);
            for (int k = 0; k < 8; ++k) {
                krs[i * 2 + 1][j * 8 + k] = key[96 + j * 8 + k] ^ temp[k];
            }
        }
    }

    // r = 21
    for (int j = 0; j < 4; ++j) {
        // index = j * 8 + k
        byte_to_bit(cks[20][j], temp);
        for (int k = 0; k < 8; ++k) {
            krs[20][j * 8 + k] = key[64 + j * 8 + k] ^ temp[k];
        }
    }
}

static inline uint8_t mul2(uint8_t a) {
    return (a & 0x08) ? ((a << 1) & 0x0f ^ 0x03) : (a << 1);
}

static inline uint8_t mul3(uint8_t a) {
    return mul2(a) ^ a;
}

void lllwbc_f(bool *state8) {
    bool u0b[8] = {0};
    bool u1b[8] = {0};
    uint8_t u0 = 0;
    uint8_t u1 = 0;
    uint8_t tu0 = 0;
    uint8_t tu1 = 0;

    for (int i = 4; i < 8; ++i) {
        u0b[i] = state8[i - 4];
        u1b[i] = state8[i];
    }

    bit_to_byte(u0b, &u0);
    bit_to_byte(u1b, &u1);

    u0 = sbox[u0];
    u1 = sbox[u1];

    tu0 = mul2(u0) ^ mul3(u1);
    tu1 = u0 ^ u1;

    u0 = tu0;
    u1 = tu1;

    u0 = sbox[u0];
    u1 = sbox[u1];

    u0 = u0 << 4 | u1;

    byte_to_bit(u0, state8);
}

void lllwbc_p(bool *state64) {
    // y -> z
    //z0 = y6, z1 = y11, z2 = y0, z3 = y12, z4 = y10, z5 = y7, z6 = y13, z7 = y1,
    //z8 = y3, z9 = y15, z10 = y4, z11 = y9, z12 = y2, z13 = y14, z14 = y5, z15 = y8

    // (0,1,.....15) -> (6,11,0,12,10,7,13,1,3,15,4,9,2,14,5,8)
    uint8_t index[16] = {6, 11, 0, 12, 10, 7, 13, 1, 3, 15, 4, 9, 2, 14, 5, 8};
    bool temp[64] = {0};
    for (int i = 0; i < 16; ++i) {
        for (int j = 0; j < 4; ++j) {
            temp[i * 4 + j] = state64[index[i] * 4 + j];
        }
    }
    for (int i = 0; i < 64; ++i) {
        state64[i] = temp[i];
    }
}

void lllwbc_p_inverse(bool *state64) {
    // z -> y
    //y0 = z2, y1 = z7, y2 = z12, y3 = z8, y4 = z10, y5 = z14, y6 = z0, y7 = z5,
    //y8 = z15, y9 = z11, y10 = z4, y11 = z1, y12 = z3, y13 = z6, y14 = z13, y15 = z9.

    // (0,1,.....15) -> (2,7,12,8,10,14,0,5,15,11,4,1,3,6,13,9)
    uint8_t index[16] = {2, 7, 12, 8, 10, 14, 0, 5, 15, 11, 4, 1, 3, 6, 13, 9};
    bool temp[64] = {0};
    for (int i = 0; i < 16; ++i) {
        for (int j = 0; j < 4; ++j) {
            temp[i * 4 + j] = state64[index[i] * 4 + j];
        }
    }
    for (int i = 0; i < 64; ++i) {
        state64[i] = temp[i];
    }
}

int crypto_encrypt(unsigned char *c,
                   const unsigned char *m, unsigned long long mlen,
                   const unsigned char *k)
{
    bool state[64];
    bool kws[2][64];
    bool krs[21][32];
    bool key[128];
    bool plain_text[64];
    bool cipher_text[64];

    // translate the m array bytes to the plain_text bit array
    for (uint16_t i = 0; i < 8; i++)
    {
        byte_to_bit(m[i], &plain_text[i * 8]);
    }
    

    for (int i = 0; i < 64; ++i) {
        state[i] = plain_text[i];
    }

    lllwbc_key_schedule(key, kws, krs);

    // s ^ kw0
    for (int i = 0; i < 64; ++i) {
        state[i] = state[i] ^ kws[0][i];
    }

    bool tb8[8] = {0};

    // 1-10 round
    for (int i = 0; i < 10; ++i) {
        // i = round number
        // j = byte level index
        // k = bit level index
        // 4 * 16 = 64
        for (int j = 0; j < 4; ++j) {

            // use 0-7 of 0-15
            for (int k = 0; k < 8; ++k) {
                tb8[k] = state[j * 16 + k];
            }
            // tb8 ^ kr
            for (int k = 0; k < 8; ++k) {
                tb8[k] = tb8[k] ^ krs[i][j * 8 + k];
            }
            // f
            lllwbc_f(tb8);

            // use 7-15 of 0-15
            for (int k = 0; k < 8; ++k) {
                state[j * 16 + 8 + k] ^= tb8[k];
            }

        }

        // p permutation
        lllwbc_p(state);

    }

    // 11-20 round
    for (int i = 10; i < 20; ++i) {
        // i = round number
        // j = byte level index
        // k = bit level index
        // 4 * 16 = 64
        for (int j = 0; j < 4; ++j) {

            // use 0-7 of 0-15
            for (int k = 0; k < 8; ++k) {
                tb8[k] = state[j * 16 + k];
            }
            // tb8 ^ kr
            for (int k = 0; k < 8; ++k) {
                tb8[k] = tb8[k] ^ krs[i][j * 8 + k];
            }
            // f
            lllwbc_f(tb8);

            // use 7-15 of 0-15
            for (int k = 0; k < 8; ++k) {
                state[j * 16 + 8 + k] ^= tb8[k];
            }

        }

        // p permutation
        lllwbc_p_inverse(state);

    }

    // 21 round
    for (int j = 0; j < 4; ++j) {

        // use 0-7 of 0-15
        for (int k = 0; k < 8; ++k) {
            tb8[k] = state[j * 16 + k];
        }
        // tb8 ^ kr
        for (int k = 0; k < 8; ++k) {
            tb8[k] = tb8[k] ^ krs[20][j * 8 + k];
        }
        // f
        lllwbc_f(tb8);

        // use 7-15 of 0-15
        for (int k = 0; k < 8; ++k) {
            state[j * 16 + 8 + k] ^= tb8[k];
        }

    }

    // s ^ kw1
    for (int i = 0; i < 64; ++i) {
        state[i] = state[i] ^ kws[1][i];
    }

    // output
    for (int i = 0; i < 64; ++i) {
        cipher_text[i] = state[i];
    }

    // translate the cipher_text bit array to the c array bytes
    for (uint16_t i = 0; i < 8; i++)
    {
        bit_to_byte(&cipher_text[i * 8], &c[i]);
    }
  return 0;
}

int crypto_decrypt(unsigned char *m,
		const unsigned char *c, unsigned long long clen,
		const unsigned char *k)
{

  
  return 0;
}


