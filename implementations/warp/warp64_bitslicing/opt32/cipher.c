#include "api.h"

#include "cipher.h"
#include "warp_bitslicing.h"



void sbox(uint32_t *r0, uint32_t *r1, uint32_t *r2, uint32_t *r3)
{
  uint32_t T[20] = {0};

  // T_{0} =  X_{0} & X_{1}
  T[0] = *r0 & *r1;
  // T_{1} =  X_{0} & X_{2}
  T[1] = *r0 & *r2;
  // T_{2} =  X_{2} | T_{0}
  T[2] = *r2 | T[0];
  // T_{3} =  X_{0} | X_{3}
  T[3] = *r0 | *r3;
  // T_{4} =  X_{3} | X_{1}
  T[4] = *r3 | *r1;
  // T_{5} =  T_{2} & T_{3}
  T[5] = T[2] & T[3];
  // T_{6} =  T_{2} ^ T_{0}
  T[6] = T[2] ^ T[0];
  // T_{7} =  T_{4} & X_{0}
  T[7] = T[4] & *r0;
  // T_{8} =  X_{1} | T_{7}
  T[8] = *r1 | T[7];
  // T_{9} =  T_{7} ^ X_{1}
  T[9] = T[7] ^ *r1;
  // T_{10} = ~T_{0}
  T[10] = ~T[0];
  // T_{11} =  T_{4} ^ T_{0}
  T[11] = T[4] ^ T[0];
  // T_{12} =  T_{7} & X_{3}
  T[12] = T[7] & *r3;
  // T_{13} =  T_{9} & T_{3}
  T[13] = T[9] & T[3];
  // T_{14} =  T_{6} | T_{13}
  T[14] = T[6] | T[13];
  // T_{15} =  T_{8} & X_{2}
  T[15] = T[8] & *r2;
  // T_{16} = ~T_{5}
  T[16] = ~T[5];
  // T_{17} = ~T_{3}
  T[17] = ~T[3];
  // T_{18} =  T_{11} ^ T_{12}
  T[18] = T[11] ^ T[12];
  // T_{19} =  T_{15} | T_{17}
  T[19] = T[15] | T[17];
  // Y_{0} = T_{16}
  *r0 = T[16];
  // Y_{1} = T_{19}
  *r1 = T[19];
  // Y_{2} = T_{18}
  *r2 = T[18];
  // Y_{3} = T_{14}
  *r3 = T[14];
}

// Function to perform right cyclic shift
uint32_t right_cyclic_shift(uint32_t value, uint32_t shift)
{
  // Ensure the shift is within the range of 0 to 31
  shift &= 31;
  return (value >> shift) | (value << (32 - shift));
}

// Function to perform permutation in bitslicing
void permutation_bitslicing(uint32_t *r0, uint32_t *r1, uint32_t *r2, uint32_t *r3, uint32_t shift[])
{

  uint32_t R0, R1, R2, R3;
  R0 = 0;
  R1 = 0;
  R2 = 0;
  R3 = 0;

  for (int i = 0; i < 32; i++)
  {
    R0 |= right_cyclic_shift(*r0, shift[i]) & (1 << ((31 - i)));
    R1 |= right_cyclic_shift(*r1, shift[i]) & (1 << ((31 - i)));
    R2 |= right_cyclic_shift(*r2, shift[i]) & (1 << ((31 - i)));
    R3 |= right_cyclic_shift(*r3, shift[i]) & (1 << ((31 - i)));
  }
  *r0 = R0;
  *r1 = R1;
  *r2 = R2;
  *r3 = R3;
}

uint32_t ks[] = {0x5555, 0x10550055, 0x5050505, 0x11111111, 0x45550000, 0x45005500, 0x50505050, 0x44444444, 0x10005555, 0x10550055, 0x5050505, 0x51111111, 0x45550000, 0x45005500, 0x10505050, 0x4444444, 0x10005555, 0x50550055, 0x45050505, 0x51111111, 0x5550000, 0x5005500, 0x10505050, 0x4444444, 0x50005555, 0x40550055, 0x45050505, 0x51111111, 0x15550000, 0x5005500, 0x10505050, 0x4444444, 0x50005555, 0x40550055, 0x45050505, 0x11111111, 0x15550000, 0x5005500, 0x50505050, 0x4444444, 0x50005555, 0x550055, 0x45050505, 0x11111111, 0x55550000, 0x5005500, 0x50505050, 0x4444444, 0x50005555, 0x10550055, 0x45050505, 0x11111111, 0x45550000, 0x15005500, 0x50505050, 0x4444444, 0x40005555, 0x550055, 0x45050505, 0x51111111, 0x55550000, 0x5005500, 0x10505050, 0x44444444, 0x50005555, 0x50550055, 0x5050505, 0x11111111, 0x5550000, 0x55005500, 0x50505050, 0x4444444, 0x5555, 0x10550055, 0x45050505, 0x51111111, 0x45550000, 0x5005500, 0x10505050, 0x44444444, 0x50005555, 0x50550055, 0x5050505, 0x51111111, 0x5550000, 0x55005500, 0x10505050, 0x4444444, 0x5555, 0x50550055, 0x45050505, 0x51111111, 0x5550000, 0x5005500, 0x10505050, 0x44444444, 0x50005555, 0x40550055, 0x5050505, 0x51111111, 0x15550000, 0x45005500, 0x10505050, 0x4444444, 0x10005555, 0x40550055, 0x45050505, 0x11111111, 0x15550000, 0x15005500, 0x50505050, 0x4444444, 0x40005555, 0x10550055, 0x45050505, 0x11111111, 0x45550000, 0x15005500, 0x50505050, 0x44444444, 0x40005555, 0x550055, 0x5050505, 0x51111111, 0x55550000, 0x45005500, 0x10505050, 0x44444444, 0x10005555, 0x50550055, 0x5050505, 0x11111111, 0x5550000, 0x45005500, 0x50505050, 0x4444444, 0x10005555, 0x550055, 0x45050505, 0x51111111, 0x55550000, 0x15005500, 0x10505050, 0x4444444, 0x40005555, 0x40550055, 0x45050505, 0x11111111, 0x15550000, 0x5005500, 0x50505050, 0x44444444, 0x50005555, 0x550055, 0x5050505, 0x11111111, 0x55550000, 0x45005500, 0x50505050, 0x4444444, 0x10005555, 0x10550055, 0x45050505, 0x11111111};



int crypto_encrypt(unsigned char *c,
                   const unsigned char *m, unsigned long long mlen,
                   const unsigned char *k)
{
    uint32_t r0, r1, r2, r3;
    // bit covert, the c is one byte to 8-bit data, need to store to r0, r1, r2, r3 uint32_t data, that easy connect to bitslicing c[0-3] = r0, c[4-7] = r1, c[8-11] = r2, c[12-15] = r3
    r0 = (uint32_t)m[0] | ((uint32_t)m[1] << 8) | ((uint32_t)m[2] << 16) | ((uint32_t)m[3] << 24);
    r1 = (uint32_t)m[4] | ((uint32_t)m[5] << 8) | ((uint32_t)m[6] << 16) | ((uint32_t)m[7] << 24);
    r2 = (uint32_t)m[8] | ((uint32_t)m[9] << 8) | ((uint32_t)m[10] << 16) | ((uint32_t)m[11] << 24);
    r3 = (uint32_t)m[12] | ((uint32_t)m[13] << 8) | ((uint32_t)m[14] << 16) | ((uint32_t)m[15] << 24);
    
    uint32_t p_shift[] = {21, 29, 25, 25, 23, 15, 5, 9, 1, 13, 27, 19, 7, 27, 11, 31, 21, 29, 25, 25, 23, 15, 5, 9, 1, 13, 27, 19, 7, 27, 11, 31};


    uint32_t t0, t1, t2, t3;

    // Your code Start
    for (size_t r = 0; r < 40; r++)
    {
        t0 = r0 & 0xAAAAAAAA;
        t1 = r1 & 0xAAAAAAAA;
        t2 = r2 & 0xAAAAAAAA;
        t3 = r3 & 0xAAAAAAAA;
        sbox(&t0, &t1, &t2, &t3);
        t0 = (t0 >> 1) & 0x55555555;
        t1 = (t1 >> 1) & 0x55555555;
        t2 = (t2 >> 1) & 0x55555555;
        t3 = (t3 >> 1) & 0x55555555;
        r0 ^= t0 ^ ks[r * 4 + 0];
        r1 ^= t1 ^ ks[r * 4 + 1];
        r2 ^= t2 ^ ks[r * 4 + 2];
        r3 ^= t3 ^ ks[r * 4 + 3];
        permutation_bitslicing(&r0, &r1, &r2, &r3, p_shift);
    }
    t0 = r0 & 0xAAAAAAAA;
    t1 = r1 & 0xAAAAAAAA;
    t2 = r2 & 0xAAAAAAAA;
    t3 = r3 & 0xAAAAAAAA;

    sbox(&t0, &t1, &t2, &t3);
    t0 = (t0 >> 1) & 0x55555555;
    t1 = (t1 >> 1) & 0x55555555;
    t2 = (t2 >> 1) & 0x55555555;
    t3 = (t3 >> 1) & 0x55555555;
    r0 ^= t0 ^ ks[40 * 4 + 0];
    r1 ^= t1 ^ ks[40 * 4 + 1];
    r2 ^= t2 ^ ks[40 * 4 + 2];
    r3 ^= t3 ^ ks[40 * 4 + 3];

    // like the start, the output need 32-bit data to 8-bit data, so need to bit covert
    for (size_t i = 0; i < 4; i++)
    {
        c[i] = (uint8_t)r0;
        c[i + 4] = (uint8_t)r1;
        c[i + 8] = (uint8_t)r2;
        c[i + 12] = (uint8_t)r3;
        r0 >>= 8;
        r1 >>= 8;
        r2 >>= 8;
        r3 >>= 8;
    }

  return 0;
}

int crypto_decrypt(unsigned char *m,
		const unsigned char *c, unsigned long long clen,
		const unsigned char *k)
{

  
  return 0;
}


