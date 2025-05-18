#include "api.h"
#include "tinyjambu.h"

/*
     TinyJAMBU-128: 128-bit key, 96-bit IV
     Optimized implementation for 32-bit platforms
     The state consists of four 32-bit registers
     state[3] || state[2] || state[1] || state[0]

     Adapted from implementation by: Hongjun Wu
*/

/*optimized state update function*/
void state_update(unsigned int *state, const unsigned char *key, unsigned int number_of_steps)
{
    unsigned int i;
    unsigned int t1, t2, t3, t4;

    // in each iteration, we compute 128 rounds of the state update function.
    for (i = 0; i < number_of_steps; i = i + 128)
    {
        t1 = (state[1] >> 15) | (state[2] << 17); // 47 = 1*32+15
        t2 = (state[2] >> 6) | (state[3] << 26);  // 47 + 23 = 70 = 2*32 + 6
        t3 = (state[2] >> 21) | (state[3] << 11); // 47 + 23 + 15 = 85 = 2*32 + 21
        t4 = (state[2] >> 27) | (state[3] << 5);  // 47 + 23 + 15 + 6 = 91 = 2*32 + 27
        state[0] ^= t1 ^ (~(t2 & t3)) ^ t4 ^ ((unsigned int *)key)[0];

        t1 = (state[2] >> 15) | (state[3] << 17);
        t2 = (state[3] >> 6) | (state[0] << 26);
        t3 = (state[3] >> 21) | (state[0] << 11);
        t4 = (state[3] >> 27) | (state[0] << 5);
        state[1] ^= t1 ^ (~(t2 & t3)) ^ t4 ^ ((unsigned int *)key)[1];

        t1 = (state[3] >> 15) | (state[0] << 17);
        t2 = (state[0] >> 6) | (state[1] << 26);
        t3 = (state[0] >> 21) | (state[1] << 11);
        t4 = (state[0] >> 27) | (state[1] << 5);
        state[2] ^= t1 ^ (~(t2 & t3)) ^ t4 ^ ((unsigned int *)key)[2];

        t1 = (state[0] >> 15) | (state[1] << 17);
        t2 = (state[1] >> 6) | (state[2] << 26);
        t3 = (state[1] >> 21) | (state[2] << 11);
        t4 = (state[1] >> 27) | (state[2] << 5);
        state[3] ^= t1 ^ (~(t2 & t3)) ^ t4 ^ ((unsigned int *)key)[3];
    }
}

// The initialization
/* The input to initialization is the 128-bit key; 96-bit IV;*/
void initialization(const unsigned char *key, const unsigned char *iv, unsigned int *state)
{
    int i;

    // initialize the state as 0
    for (i = 0; i < 4; i++)
        state[i] = 0;

    // update the state with the key
    state_update(state, key, NROUND2);

    // introduce IV into the state
    for (i = 0; i < 3; i++)
    {
        state[1] ^= FrameBitsIV;
        state_update(state, key, NROUND1);
        state[3] ^= ((unsigned int *)iv)[i];
    }
}

// process the associated data
void process_ad(const unsigned char *k, const unsigned char *ad, unsigned long long adlen, unsigned int *state)
{
    unsigned long long i;
    unsigned int j;

    for (i = 0; i < (adlen >> 2); i++)
    {
        state[1] ^= FrameBitsAD;
        state_update(state, k, NROUND1);
        state[3] ^= ((unsigned int *)ad)[i];
    }

    // if adlen is not a multiple of 4, we process the remaining bytes
    if ((adlen & 3) > 0)
    {
        state[1] ^= FrameBitsAD;
        state_update(state, k, NROUND1);
        for (j = 0; j < (adlen & 3); j++)
            ((unsigned char *)state)[12 + j] ^= ad[(i << 2) + j];
        state[1] ^= adlen & 3;
    }
}

// Encrypt a message using TinyJAMBU
int cipher_encrypt(unsigned char *c, const unsigned char *m, unsigned long long mlen, const unsigned char *k)
{
    // Default nonce (all zeros)
    static const unsigned char default_nonce[12] = {0};

    unsigned long long i;
    unsigned int j;
    unsigned char mac[8];
    unsigned int state[4];

    // initialization stage
    initialization(k, default_nonce, state);

    // process the associated data (none in this case)
    //  We're not using AD in the cipher-only mode

    // process the plaintext
    for (i = 0; i < (mlen >> 2); i++)
    {
        state[1] ^= FrameBitsPC;
        state_update(state, k, NROUND2);
        state[3] ^= ((unsigned int *)m)[i];
        ((unsigned int *)c)[i] = state[2] ^ ((unsigned int *)m)[i];
    }

    // if mlen is not a multiple of 4, we process the remaining bytes
    if ((mlen & 3) > 0)
    {
        state[1] ^= FrameBitsPC;
        state_update(state, k, NROUND2);
        for (j = 0; j < (mlen & 3); j++)
        {
            ((unsigned char *)state)[12 + j] ^= m[(i << 2) + j];
            c[(i << 2) + j] = ((unsigned char *)state)[8 + j] ^ m[(i << 2) + j];
        }
        state[1] ^= mlen & 3;
    }

    return 0;
}

// Decrypt a message using TinyJAMBU
int cipher_decrypt(unsigned char *m, const unsigned char *c, unsigned long long clen, const unsigned char *k)
{
    // Default nonce (all zeros)
    static const unsigned char default_nonce[12] = {0};

    unsigned long long i;
    unsigned int j;
    unsigned int state[4];

    // initialization stage
    initialization(k, default_nonce, state);

    // process the associated data (none in this case)
    //  We're not using AD in the cipher-only mode

    // process the ciphertext
    for (i = 0; i < (clen >> 2); i++)
    {
        state[1] ^= FrameBitsPC;
        state_update(state, k, NROUND2);
        ((unsigned int *)m)[i] = state[2] ^ ((unsigned int *)c)[i];
        state[3] ^= ((unsigned int *)m)[i];
    }

    // if clen is not a multiple of 4, we process the remaining bytes
    if ((clen & 3) > 0)
    {
        state[1] ^= FrameBitsPC;
        state_update(state, k, NROUND2);
        for (j = 0; j < (clen & 3); j++)
        {
            m[(i << 2) + j] = c[(i << 2) + j] ^ ((unsigned char *)state)[8 + j];
            ((unsigned char *)state)[12 + j] ^= m[(i << 2) + j];
        }
        state[1] ^= clen & 3;
    }

    return 0;
}
