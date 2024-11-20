#include "api.h"

#include "cipher.h"
#include "qarmav2.h"
#include "stdint.h"
#include <stdio.h>

#include <stdio.h>

#include <stdint.h>

#include <stdint.h>

#define KS_SIZE 56
#define RS_SIZE 4
#define SS_SIZE 20


// Key schedule array
uint32_t ks[KS_SIZE] = {
    0xffff, 0xff00ff, 0xf0f0f0f, 0x33333333,
    0xc0f0c0f0, 0xcfcfcfcf, 0xcccccccc, 0x0,
    0xcfcfc3f0, 0xf300003c, 0xcfcccc03, 0xf3cf00f,
    0xccfffc, 0x30cfcc30, 0xfcf0cfcc, 0xf0cff33,
    0xfcffc303, 0xfff0c30c, 0xf3cccfc0, 0xf3f3333,
    0xcffcc330, 0xccffcff0, 0x3c00fff, 0xccffccf3,
    0xc3c3c3c3, 0xcccccccc, 0xffff, 0xff00fc,
    0xf0f0f0f0, 0xcccccccf, 0xfffc0000, 0xfc03fc03,
    0x3cfc0c3f, 0xcf00fc3c, 0x3300f0f0, 0xf03c33c0,
    0xc33ccfc, 0xc3000c0f, 0x33ffc33f, 0xcfcf3f00,
    0xfc33330, 0xf00c3fff, 0x300fc33, 0x33cf0000,
    0x30f3fc3, 0xc3c0f3fc, 0xfccfffff, 0xcfccfc3c,
    0xfc0c0000, 0xff030c30, 0xc030ccff, 0x3cc3ff33,
    0xf0cc3ffc, 0xc3f000cc, 0xf03ccc0, 0xf3c33f00
};

// Message state
const uint32_t ms = 0xc0030000;

// Scratch space
uint32_t ss[SS_SIZE] = {0};


// Replicate PPO macro functionality
static inline uint32_t ppo(uint32_t src, uint32_t rotate_amount, uint32_t mask_value) {
    uint32_t temp = (src >> rotate_amount) | (src << (32 - rotate_amount)); // ROR
    return temp & mask_value;
}

// Implementation of PPO_TUA permutation
uint32_t ppo_tua(uint32_t src) {
    uint32_t dest = 0;
    
    dest |= src & ms;
    dest |= ppo(src, 6, 0xc300);    // [8, 11]
    dest |= ppo(src, 8, 0x300000);  // [5]
    dest |= ppo(src, 10, 0xc);      // [14]
    dest |= ppo(src, 12, 0x33000000); // [1, 3]
    dest |= ppo(src, 14, 0xc00);    // [10]
    dest |= ppo(src, 18, 0x30);     // [13]
    dest |= ppo(src, 20, 0xcc0000); // [4, 6]
    dest |= ppo(src, 22, 0x3000);   // [9]
    dest |= ppo(src, 24, 0xc000000); // [2]
    dest |= ppo(src, 26, 0xc3);     // [12, 15]
    
    return dest;
}

// Implementation of PPO_TUA_INV permutation
uint32_t ppo_tua_inv(uint32_t src) {
    uint32_t dest = 0;
    
    dest |= src & ms;
    
    // Special case for rotation 6
    uint32_t temp = (src >> 6) | (src << (32 - 6));
    dest |= temp & 0xc000003;
    
    dest |= ppo(src, 8, 0xc0000);   // [6]
    dest |= ppo(src, 10, 0xc);      // [14]
    dest |= ppo(src, 12, 0xcc0);    // [10, 12]
    dest |= ppo(src, 14, 0xc00000); // [4]
    dest |= ppo(src, 18, 0x3000000); // [3]
    dest |= ppo(src, 20, 0x330);    // [11, 13]
    dest |= ppo(src, 22, 0x3000);   // [9]
    dest |= ppo(src, 24, 0x30000000); // [1]
    dest |= ppo(src, 26, 0x30c000); // [5, 8]
    
    return dest;
}


// Forward TUA permutation
uint32_t ppo_tua_f(uint32_t src) {
    uint32_t dest = 0;
    
    dest |= ppo(src, 2, 0x3000);      // [9]
    dest |= ppo(src, 4, 0xc30030);    // [4, 7, 13]
    dest |= ppo(src, 8, 0xc000000);   // [2]
    dest |= ppo(src, 14, 0x30000300); // [1, 11]
    dest |= ppo(src, 16, 0xc003);     // [8, 15]
    dest |= ppo(src, 18, 0xc00c0);    // [6, 12]
    dest |= ppo(src, 24, 0x300000);   // [5]
    dest |= ppo(src, 26, 0x3000000);  // [3]
    dest |= ppo(src, 28, 0xc00);      // [10]
    dest |= ppo(src, 30, 0xc000000c); // [0, 14]
    
    return dest;
}

// Inverse TUA permutation
uint32_t ppo_tua_f_inv(uint32_t src) {
    uint32_t dest = 0;
    
    dest |= ppo(src, 2, 0x30000003);  // [1, 15]
    dest |= ppo(src, 4, 0xc0);        // [12]
    dest |= ppo(src, 6, 0xc0000);     // [6]
    dest |= ppo(src, 8, 0x3000);      // [9]
    dest |= ppo(src, 14, 0x3000030);  // [3, 13]
    dest |= ppo(src, 16, 0xc0030000); // [0, 7]
    dest |= ppo(src, 18, 0xc00c00);   // [4, 10]
    dest |= ppo(src, 24, 0xc);        // [14]
    dest |= ppo(src, 28, 0xc300300);  // [2, 5, 11]
    dest |= ppo(src, 30, 0xc000);     // [8]
    
    return dest;
}

// Apply permutation to array of 4 32-bit words
void permutation(uint32_t *base, uint32_t (*ppo_func)(uint32_t)) {
    for (int i = 0; i < 4; i++) {
        base[i] = ppo_func(base[i]);
    }
}


// Diffusion matrix operation
void diffusion_matrix(uint32_t *base) {
    uint32_t r1 = base[0];
    uint32_t r2 = base[1];
    uint32_t r3 = base[2];
    uint32_t r4 = base[3];
    
    uint32_t r5 = (r2 >> 24) | (r2 << 8);  // ROR #24
    r5 ^= (r3 >> 16) | (r3 << 16);         // ROR #16
    r5 ^= (r4 >> 8) | (r4 << 24);          // ROR #8
    
    uint32_t r6 = (r3 >> 24) | (r3 << 8);
    r6 ^= (r4 >> 16) | (r4 << 16);
    r6 ^= (r1 >> 8) | (r1 << 24);
    
    uint32_t r7 = (r4 >> 24) | (r4 << 8);
    r7 ^= (r1 >> 16) | (r1 << 16);
    r7 ^= (r2 >> 8) | (r2 << 24);
    
    uint32_t r8 = (r1 >> 24) | (r1 << 8);
    r8 ^= (r2 >> 16) | (r2 << 16);
    r8 ^= (r3 >> 8) | (r3 << 24);
    
    base[0] = r5;
    base[1] = r6;
    base[2] = r7;
    base[3] = r8;
}

// S-box operation
void sbox(uint32_t *rs, uint32_t *ss) {
    uint32_t r0 = rs[0];
    uint32_t r1 = rs[1];
    uint32_t r2 = rs[2];
    uint32_t r3 = rs[3];
    
    // T[0] = R2 ^ R3
    ss[0] = r2 ^ r3;
    
    // T[1] = T[0] ^ 0xFFFFFFFF
    ss[1] = ~ss[0];
    
    // T[2] = T[1] | R0
    ss[2] = ss[1] | r0;
    
    // T[3] = T[2] ^ R3
    ss[3] = ss[2] ^ r3;
    
    // T[4] = T[3] | R1
    ss[4] = ss[3] | r1;
    
    // T[5] = R0 ^ T[4]
    ss[5] = r0 ^ ss[4];
    
    // T[6] = R1 ^ T[0]
    ss[6] = r1 ^ ss[0];
    
    // T[7] = T[1] ^ T[3]
    ss[7] = ss[1] ^ ss[3];
    
    // T[8] = R2 | T[7]
    ss[8] = r2 | ss[7];
    
    // T[9] = T[8] ^ T[3]
    ss[9] = ss[8] ^ ss[3];
    
    // T[10] = T[6] | T[8]
    ss[10] = ss[6] | ss[8];
    
    // T[11] = T[10] | R1
    ss[11] = ss[10] | r1;
    
    // T[12] = T[11] & T[9]
    ss[12] = ss[11] & ss[9];
    
    // T[13] = R1 ^ T[10]
    ss[13] = r1 ^ ss[10];
    
    // T[14] = T[1] ^ T[0]
    ss[14] = ss[1] ^ ss[0];
    
    // T[15] = T[0] ^ T[5]
    ss[15] = ss[0] ^ ss[5];
    
    // T[16] = T[13] & T[15]
    ss[16] = ss[13] & ss[15];
    
    // T[17] = R3 & T[15]
    ss[17] = r3 & ss[15];
    
    // T[18] = T[16] ^ T[8]
    ss[18] = ss[16] ^ ss[8];
    
    // T[19] = T[13] | T[17]
    ss[19] = ss[13] | ss[17];
    
    // Store results back
    rs[0] = ss[12]; // T[12]
    rs[1] = ss[5];  // T[5]
    rs[2] = ss[18]; // T[18]
    rs[3] = ss[19]; // T[19]
}

// Inverse S-box operation
void sbox_inv(uint32_t *rs, uint32_t *ss) {
    uint32_t r0 = rs[0];
    uint32_t r1 = rs[1];
    uint32_t r2 = rs[2];
    uint32_t r3 = rs[3];
    
    // T[0] = R2 & R1
    ss[0] = r2 & r1;
    
    // T[1] = R0 ^ R1
    ss[1] = r0 ^ r1;
    
    // T[2] = R2 | R0
    ss[2] = r2 | r0;
    
    // T[3] = R0 ^ T[0]
    ss[3] = r0 ^ ss[0];
    
    // T[4] = R1 | R2
    ss[4] = r1 | r2;
    
    // T[5] = R3 & T[3]
    ss[5] = r3 & ss[3];
    
    // T[6] = R1 ^ 0xFFFFFFFF
    ss[6] = ~r1;
    
    // T[7] = T[5] | R1
    ss[7] = ss[5] | r1;
    
    // T[8] = T[1] | R3
    ss[8] = ss[1] | r3;
    
    // T[9] = R3 | R2
    ss[9] = r3 | r2;
    
    // T[10] = T[9] & T[4]
    ss[10] = ss[9] & ss[4];
    
    // T[11] = T[6] ^ T[7]
    ss[11] = ss[6] ^ ss[7];
    
    // T[12] = T[4] ^ T[8]
    ss[12] = ss[4] ^ ss[8];
    
    // T[13] = T[2] ^ T[5]
    ss[13] = ss[2] ^ ss[5];
    
    // T[14] = T[12] | R0
    ss[14] = ss[12] | r0;
    
    // T[15] = T[14] & T[9]
    ss[15] = ss[14] & ss[9];
    
    // T[16] = T[11] ^ T[0]
    ss[16] = ss[11] ^ ss[0];
    
    // T[17] = T[10] & T[8]
    ss[17] = ss[10] & ss[8];
    
    // T[18] = T[6] | T[17]
    ss[18] = ss[6] | ss[17];
    
    // T[19] = T[16] & T[18]
    ss[19] = ss[16] & ss[18];
    
    // Store results
    rs[0] = ss[19]; // T[19]
    rs[1] = ss[13]; // T[13]
    rs[2] = ss[15]; // T[15]
    rs[3] = ss[17]; // T[17]
}

// XOR operation on 4 words
void xor_bitslicing(uint32_t *dest, uint32_t *src) {
    for (int i = 0; i < 4; i++) {
        dest[i] ^= src[i];
    }
}

// Round function
void round_function(uint32_t *rs, uint32_t *ks, uint32_t *ss) {
    ks += 4; // Advance key schedule pointer by 16 bytes
    xor_bitslicing(rs, ks);
    for (size_t i = 0; i < 4; i++)
    {
        rs[i] = ppo_tua_f(rs[i]);
    }
    diffusion_matrix(rs);  // Previously defined
    sbox(rs, ss);  // Previously defined
}
#include <stdint.h>

// Inverse round function
void round_inv(uint32_t *rs, uint32_t *ks, uint32_t *ss) {
    sbox_inv(rs, ss);
    diffusion_matrix(rs);
     for (size_t i = 0; i < 4; i++)
    {
        rs[i] = ppo_tua_inv(rs[i]);
    }
    ks += 4; // Advance key schedule pointer by 16 bytes
    xor_bitslicing(rs, ks);
}



int crypto_encrypt(unsigned char *c,
                   const unsigned char *m, unsigned long long mlen,
                   const unsigned char *k)
{
    uint32_t rs[4]={0};
    for (size_t i = 0; i < 4; i++)
    {
        rs[i] = m[i];
    }
    uint32_t *ks = ks;
    // Initial transformation
    xor_bitslicing(rs, ks);
    sbox(rs, ss);

    for (int i = 0; i < 25; i++) {
        round_function(rs, ks, ss);
    }

    // Middle part
    for (size_t i = 0; i < 4; i++)
    {
        rs[i] = ppo_tua(rs[i]);
    }
    ks += 4;
    xor_bitslicing(rs, ks);
    diffusion_matrix(rs);
    ks += 4;
    xor_bitslicing(rs, ks);
    for (size_t i = 0; i < 4; i++)
    {
        rs[i] = ppo_tua_f(rs[i]);
    }
    
    for (int i = 0; i < 25; i++) {
        round_inv(rs, ks, ss);
    }

    // Final transformation
    sbox_inv(rs, ss);
    ks += 4;
    xor_bitslicing(rs, ks);
    for (size_t i = 0; i < 4; i++)
    {
        c[i] = rs[i];
    }
    
  return 0;
}

int crypto_decrypt(unsigned char *m,
		const unsigned char *c, unsigned long long clen,
		const unsigned char *k)
{

  
  return 0;
}


