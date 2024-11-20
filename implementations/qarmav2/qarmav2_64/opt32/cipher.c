#include "api.h"

#include "cipher.h"
#include "qarmav2.h"
#include "stdint.h"
#include <stdio.h>

static uint32_t a;

static uint8_t tau[16] = {0, 11, 6, 13, 10, 1, 12, 7, 5, 14, 3, 8, 15, 4, 9, 2};
static uint8_t tau_f[16] = {1, 10, 14, 6, 2, 9, 13, 5, 0, 8, 12, 4, 3, 11, 15, 7};

static uint8_t tau_inverse[16] = {0, 5, 15, 10, 13, 8, 2, 7, 11, 14, 4, 1, 6, 3, 9, 12};
static uint8_t tau_f_inverse[16] = {8, 0, 4, 12, 11, 7, 3, 15, 9, 5, 1, 13, 10, 6, 2, 14};

/// @brief permutation the state by p, the result is stored in state
/// @param state the state to be permuted
/// @param p the permutation array
/// @attention have pass test
void permutation(uint8_t state[16], uint8_t p[16])
{
    uint8_t temp[16];
    for (int i = 0; i < 16; i++)
    {
        temp[i] = state[p[i]];
    }
    for (int i = 0; i < 16; i++)
    {
        state[i] = temp[i];
    }
}

#define rotate_left(x, n) ((((x) << (n)) | ((x) >> (4 - (n)))) & 0x0F)

void mix_column(uint8_t state[16])
{
    uint8_t temp[16];
    uint8_t index;

    for (int i = 0; i < 4; i++)
    {
        for (size_t j = 0; j < 4; j++)
        {
            index = i + 4 * j;
            temp[index] = rotate_left(state[(index + 4) % 16], 1) ^ rotate_left(state[(index + 8) % 16], 2) ^ rotate_left(state[(index + 12) % 16], 3);
        }
    }

    for (int i = 0; i < 16; i++)
    {
        state[i] = temp[i];
    }
}

static uint8_t S[16] = {4, 7, 9, 11, 12, 6, 14, 15, 0, 5, 1, 13, 8, 3, 2, 10};
static uint8_t S_inverse[16] = {8, 10, 14, 13, 0, 9, 5, 1, 12, 2, 15, 3, 4, 11, 6, 7};

void s_box(uint8_t state[16], uint8_t s[16])
{
    for (int i = 0; i < 16; i++)
    {
        state[i] = s[state[i]];
    }
}
// pass
uint64_t o_func(uint64_t w, uint8_t b)
{
    uint64_t t1, t2;
    t1 = (w >> 1) | (w << (b - 1)) & ((1 << b) - 1);
    t2 = w >> (b - 1);
    return t1 ^ t2;
}
// pass
uint64_t o_func_inverse(uint64_t w, uint8_t b)
{
    uint64_t t1, t2;
    t1 = (w << 1) & ((1 << b) - 1) | (w >> (b - 1));
    t2 = w >> (b - 3) & (2);
    return t1 ^ t2;
}

uint64_t psi_func(uint64_t input)
{
    uint64_t spill, tmp;
    spill = input >> 51;
    tmp = ((input << 13) & (0xffffffffffffffff ^ (spill << 50) ^ (spill << 33) ^ (spill << 19) ^ spill));
    spill = tmp >> 54;
    tmp = ((tmp << 10) & (0xffffffffffffffff) ^ (spill << 50) ^ (spill << 33) ^ (spill << 19) ^ spill);
    return tmp;
}

// pass
void int_to_4bit_list(uint64_t input, uint8_t output[16])
{
    for (int i = 0; i < 16; i++)
    {
        output[i] = (input >> (4 * (15 - i))) & 0x0F;
    }
}

// pass
void bit_list_to_int(uint8_t input[16], uint64_t* output)
{
    *output = 0;
    for (int i = 0; i < 16; i++)
    {
        *output |= (uint64_t)input[i] << (4 * (15 - i));
    }
}


static uint64_t P = 0x0000000000000000;
static uint64_t K0 = 0x0123456789ABCDEF, K1 = 0xFEDCBA9876543210;
static uint64_t T0 = 0x7E5C3A18F6D4B290, T1 = 0x1EB852FC9630DA74;
static uint64_t W0, W1;
static uint8_t r = 5;


int crypto_encrypt(unsigned char *c,
                   const unsigned char *m, unsigned long long mlen,
                   const unsigned char *k)
{
  uint8_t temp0[16];

    W0 = o_func(o_func(K0, 64), 64);
    W1 = o_func_inverse(o_func_inverse(K1, 64), 64);

    int_to_4bit_list(T0, temp0);
    for (size_t i = 0; i < r - 1; i++)
    {
        permutation(temp0, tau_f);
    }
    bit_list_to_int(temp0, &T0);

    uint64_t t0 = T0, t1 = T1, k0 = K0, k1 = K1;
    uint8_t state_l[16];
    uint64_t state;
    uint64_t cs[6] = {0, 0, 0x243f6a8885a308d3, 0x3b92eeee967a1ffd, 0x5370afe1b69dc900, 0x134c3e4b47a1b8f1};
    uint64_t alpha = 0x13198A2E03707344;
    uint64_t beta = 0x249ea1b3c5118ce3;

    // Your code Start

    int_to_4bit_list(P ^ k0, state_l);
    s_box(state_l, S);

    // forward function
    for (size_t i = 1; i < r + 1; i++)
    {
        bit_list_to_int(state_l, &state);
        
        if (i % 2 == 1)
        {
            state = state ^ k1 ^ t1;
        }
        else
        {
            state = state ^ k0 ^ t0;
        }
         
        state = state ^ cs[i];
        
        int_to_4bit_list(state, state_l);
        permutation(state_l, tau);
        mix_column(state_l);
        s_box(state_l, S);

        if (i % 2 == 1)
        {
            int_to_4bit_list(t1, temp0);
            permutation(temp0, tau_f);
            bit_list_to_int(temp0, &t1);
        }
        else
        {
            int_to_4bit_list(t0, temp0);
            permutation(temp0, tau_f_inverse);
            bit_list_to_int(temp0, &t0);
        }
    }

    // middle part
    k0 = o_func(k0, 64) ^ alpha;
    k1 = o_func_inverse(k1, 64) ^ beta;

    permutation(state_l, tau);
    bit_list_to_int(state_l, &state);
    state = state ^ W0;
    int_to_4bit_list(state, state_l);
    mix_column(state_l);
    bit_list_to_int(state_l, &state);
    state = state ^ W1;
    int_to_4bit_list(state, state_l);
    permutation(state_l, tau_inverse);

    // backward part
    for (size_t i = r; i > 0; i--)
    {
        s_box(state_l, S_inverse);
        mix_column(state_l);
        permutation(state_l, tau_inverse);

        bit_list_to_int(state_l, &state);
        state = state ^ cs[i];
        if ((i + 1) % 2)
        {
            state = state ^ k1 ^ t1;
        }
        else
        {
            state = state ^ k0 ^ t0;
        }
        int_to_4bit_list(state, state_l);

        if (i > 1 && (i % 2 == 0))
        {
            int_to_4bit_list(t1, temp0);
            permutation(temp0, tau_f);
            bit_list_to_int(temp0, &t1);
        }
        else
        {
            int_to_4bit_list(t0, temp0);
            permutation(temp0, tau_f_inverse);
            bit_list_to_int(temp0, &t0);
        }
    }

    s_box(state_l, S_inverse);
    bit_list_to_int(state_l, &state);
    state = state ^ k1;


  return 0;
}

int crypto_decrypt(unsigned char *m,
		const unsigned char *c, unsigned long long clen,
		const unsigned char *k)
{

  
  return 0;
}


