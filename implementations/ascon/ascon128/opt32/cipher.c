#include <stdint.h>
#include <string.h>
#include "api.h"
#include "ascon.h"

/* Define crypto_aead functions required by the cipher */
#define crypto_aead_encrypt cipher_encrypt
#define crypto_aead_decrypt cipher_decrypt

/* Define the required CRYPTO constants if not already defined in api.h */
#ifndef CRYPTO_NSECBYTES
#define CRYPTO_NSECBYTES 0
#endif
#ifndef CRYPTO_NPUBBYTES
#define CRYPTO_NPUBBYTES 16
#endif
#ifndef CRYPTO_ABYTES
#define CRYPTO_ABYTES 16
#endif

/* Round constants for Ascon */
static const uint32_t round_constants[12] = {
    0xf0, 0xe1, 0xd2, 0xc3, 0xb4, 0xa5, 0x96, 0x87, 0x78, 0x69, 0x5a, 0x4b};

/* Rotation macros */
#define ROTR(x, n) (((x) >> (n)) | ((x) << (32 - (n))))
#define ROTL32(x, n) (((x) << (n)) | ((x) >> (32 - (n))))

/* 64-bit rotation for the 32-bit implementation */
#define ROTL64(x, n)                                                         \
    {                                                                        \
        if ((n) == 32)                                                       \
        {                                                                    \
            uint32_t temp = (x)[0];                                          \
            (x)[0] = (x)[1];                                                 \
            (x)[1] = temp;                                                   \
        }                                                                    \
        else if ((n) < 32)                                                   \
        {                                                                    \
            uint32_t temp = ((x)[0] << (n)) | ((x)[1] >> (32 - (n)));        \
            (x)[1] = ((x)[1] << (n)) | ((x)[0] >> (32 - (n)));               \
            (x)[0] = temp;                                                   \
        }                                                                    \
        else                                                                 \
        {                                                                    \
            uint32_t temp = ((x)[0] << ((n) - 32)) | ((x)[1] >> (64 - (n))); \
            (x)[1] = ((x)[1] << ((n) - 32)) | ((x)[0] >> (64 - (n)));        \
            (x)[0] = temp;                                                   \
        }                                                                    \
    }

/* Load a 64-bit value */
static inline void from_bytes(uint32_t *x, const uint8_t *bytes)
{
    x[0] = ((uint32_t)bytes[0] << 24) | ((uint32_t)bytes[1] << 16) |
           ((uint32_t)bytes[2] << 8) | (uint32_t)bytes[3];
    x[1] = ((uint32_t)bytes[4] << 24) | ((uint32_t)bytes[5] << 16) |
           ((uint32_t)bytes[6] << 8) | (uint32_t)bytes[7];
}

/* Store a 64-bit value */
static inline void to_bytes(uint8_t *bytes, const uint32_t *x)
{
    bytes[0] = (x[0] >> 24) & 0xff;
    bytes[1] = (x[0] >> 16) & 0xff;
    bytes[2] = (x[0] >> 8) & 0xff;
    bytes[3] = x[0] & 0xff;
    bytes[4] = (x[1] >> 24) & 0xff;
    bytes[5] = (x[1] >> 16) & 0xff;
    bytes[6] = (x[1] >> 8) & 0xff;
    bytes[7] = x[1] & 0xff;
}

/* Ascon permutation round function */
static inline void ascon_round(ascon_state_t *s, uint32_t round_const)
{
    uint32_t t0[2], t1[2], t2[2], t3[2], t4[2];

    /* Addition of round constant */
    s->x2[0] ^= round_const;

    /* Substitution layer (5-bit S-box) */
    /* Step 1: x0 ← x0 ⊕ x4,   x2 ← x2 ⊕ x1,   t0 ← x0 & x4,   t1 ← x2 & x1 */
    s->x0[0] ^= s->x4[0];
    s->x0[1] ^= s->x4[1];
    s->x2[0] ^= s->x1[0];
    s->x2[1] ^= s->x1[1];
    t0[0] = s->x0[0] & s->x4[0];
    t0[1] = s->x0[1] & s->x4[1];
    t1[0] = s->x2[0] & s->x1[0];
    t1[1] = s->x2[1] & s->x1[1];

    /* Step 2: x4 ← x4 ⊕ x3,   x1 ← x1 ⊕ x0,   t2 ← x4 & x3,   t3 ← x1 & x0 */
    s->x4[0] ^= s->x3[0];
    s->x4[1] ^= s->x3[1];
    s->x1[0] ^= s->x0[0];
    s->x1[1] ^= s->x0[1];
    t2[0] = s->x4[0] & s->x3[0];
    t2[1] = s->x4[1] & s->x3[1];
    t3[0] = s->x1[0] & s->x0[0];
    t3[1] = s->x1[1] & s->x0[1];

    /* Step 3: x3 ← x3 ⊕ t0,   x0 ← x0 ⊕ t1,   t4 ← x3 & x0 */
    s->x3[0] ^= t0[0];
    s->x3[1] ^= t0[1];
    s->x0[0] ^= t1[0];
    s->x0[1] ^= t1[1];
    t4[0] = s->x3[0] & s->x0[0];
    t4[1] = s->x3[1] & s->x0[1];

    /* Step 4: x2 ← x2 ⊕ t2,   x4 ← x4 ⊕ t3 */
    s->x2[0] ^= t2[0];
    s->x2[1] ^= t2[1];
    s->x4[0] ^= t3[0];
    s->x4[1] ^= t3[1];

    /* Step 5: x1 ← x1 ⊕ t4,   t0 ← x1 & x2 */
    s->x1[0] ^= t4[0];
    s->x1[1] ^= t4[1];
    t0[0] = s->x1[0] & s->x2[0];
    t0[1] = s->x1[1] & s->x2[1];

    /* Step 6: x2 ← x2 ⊕ x3,   x3 ← x3 ⊕ x4,   x4 ← x4 ⊕ t0 */
    s->x2[0] ^= s->x3[0];
    s->x2[1] ^= s->x3[1];
    s->x3[0] ^= s->x4[0];
    s->x3[1] ^= s->x4[1];
    s->x4[0] ^= t0[0];
    s->x4[1] ^= t0[1];

    /* Step 7: x1 ← ~x1,       x0 ← ~x0,        x2 ← x2 ⊕ (x0 & x1) */
    s->x1[0] = ~s->x1[0];
    s->x1[1] = ~s->x1[1];
    s->x0[0] = ~s->x0[0];
    s->x0[1] = ~s->x0[1];
    s->x2[0] ^= (s->x0[0] & s->x1[0]);
    s->x2[1] ^= (s->x0[1] & s->x1[1]);

    /* Linear diffusion layer */
    /* x0 ← x0 ⊕ (x0 ≫ 19) ⊕ (x0 ≫ 28) */
    t0[0] = s->x0[0];
    t0[1] = s->x0[1];
    ROTL64(t0, 19);
    t1[0] = s->x0[0];
    t1[1] = s->x0[1];
    ROTL64(t1, 28);
    s->x0[0] ^= t0[0] ^ t1[0];
    s->x0[1] ^= t0[1] ^ t1[1];

    /* x1 ← x1 ⊕ (x1 ≫ 61) ⊕ (x1 ≫ 39) */
    t0[0] = s->x1[0];
    t0[1] = s->x1[1];
    ROTL64(t0, 61);
    t1[0] = s->x1[0];
    t1[1] = s->x1[1];
    ROTL64(t1, 39);
    s->x1[0] ^= t0[0] ^ t1[0];
    s->x1[1] ^= t0[1] ^ t1[1];

    /* x2 ← x2 ⊕ (x2 ≫ 1) ⊕ (x2 ≫ 6) */
    t0[0] = s->x2[0];
    t0[1] = s->x2[1];
    ROTL64(t0, 1);
    t1[0] = s->x2[0];
    t1[1] = s->x2[1];
    ROTL64(t1, 6);
    s->x2[0] ^= t0[0] ^ t1[0];
    s->x2[1] ^= t0[1] ^ t1[1];

    /* x3 ← x3 ⊕ (x3 ≫ 10) ⊕ (x3 ≫ 17) */
    t0[0] = s->x3[0];
    t0[1] = s->x3[1];
    ROTL64(t0, 10);
    t1[0] = s->x3[0];
    t1[1] = s->x3[1];
    ROTL64(t1, 17);
    s->x3[0] ^= t0[0] ^ t1[0];
    s->x3[1] ^= t0[1] ^ t1[1];

    /* x4 ← x4 ⊕ (x4 ≫ 7) ⊕ (x4 ≫ 41) */
    t0[0] = s->x4[0];
    t0[1] = s->x4[1];
    ROTL64(t0, 7);
    t1[0] = s->x4[0];
    t1[1] = s->x4[1];
    ROTL64(t1, 41);
    s->x4[0] ^= t0[0] ^ t1[0];
    s->x4[1] ^= t0[1] ^ t1[1];
}

/* Ascon permutation */
void ascon_permutation(ascon_state_t *state, int rounds)
{
    int start_round = 12 - rounds;
    for (int i = start_round; i < 12; i++)
    {
        ascon_round(state, round_constants[i]);
    }
}

/* Initialize Ascon state with key and nonce */
void ascon_initialize(ascon_state_t *state, const uint8_t *key, const uint8_t *nonce)
{
    /* Set initial state */
    state->x0[0] = 0x80400c06;
    state->x0[1] = 0x0;               /* IV || K1 */
    from_bytes(state->x1, key);       /* Key K0 || K1 */
    from_bytes(state->x2, key + 8);   /* Key K2 || K3 */
    from_bytes(state->x3, nonce);     /* Nonce N0 || N1 */
    from_bytes(state->x4, nonce + 8); /* Nonce N2 || N3 */

    /* Apply pa permutation rounds */
    ascon_permutation(state, ASCON_PA_ROUNDS);

    /* XOR key to the state */
    state->x3[0] ^= ((uint32_t *)key)[0];
    state->x3[1] ^= ((uint32_t *)key)[1];
    state->x4[0] ^= ((uint32_t *)key)[2];
    state->x4[1] ^= ((uint32_t *)key)[3];
}

/* Absorb associated data */
void ascon_absorb_associated_data(ascon_state_t *state, const uint8_t *ad, uint64_t adlen)
{
    uint32_t block[2];

    /* Process associated data blocks */
    while (adlen >= ASCON_RATE)
    {
        from_bytes(block, ad);
        state->x0[0] ^= block[0];
        state->x0[1] ^= block[1];
        ascon_permutation(state, ASCON_PB_ROUNDS);
        ad += ASCON_RATE;
        adlen -= ASCON_RATE;
    }

    /* Process last block */
    if (adlen > 0)
    {
        uint8_t last_block[ASCON_RATE] = {0};
        memcpy(last_block, ad, adlen);
        last_block[adlen] = 0x80; /* Padding */
        from_bytes(block, last_block);
        state->x0[0] ^= block[0];
        state->x0[1] ^= block[1];
    }
    else
    {
        /* Empty block with padding */
        state->x0[0] ^= 0x80000000;
    }

    /* Domain separation */
    state->x4[0] ^= 0x1;

    /* Apply pb permutation rounds */
    ascon_permutation(state, ASCON_PB_ROUNDS);
}

/* Process plaintext blocks */
void ascon_absorb_plaintext(ascon_state_t *state, uint8_t *c, const uint8_t *m, uint64_t mlen)
{
    uint32_t block[2];

    /* Process plaintext blocks */
    while (mlen >= ASCON_RATE)
    {
        from_bytes(block, m);
        state->x0[0] ^= block[0];
        state->x0[1] ^= block[1];
        to_bytes(c, state->x0);
        ascon_permutation(state, ASCON_PB_ROUNDS);
        m += ASCON_RATE;
        c += ASCON_RATE;
        mlen -= ASCON_RATE;
    }

    /* Process last block */
    if (mlen > 0)
    {
        uint8_t last_block[ASCON_RATE] = {0};
        memcpy(last_block, m, mlen);
        last_block[mlen] = 0x80; /* Padding */
        from_bytes(block, last_block);
        state->x0[0] ^= block[0];
        state->x0[1] ^= block[1];
        to_bytes(last_block, state->x0);
        memcpy(c, last_block, mlen);
    }
}

/* Process ciphertext blocks */
void ascon_absorb_ciphertext(ascon_state_t *state, uint8_t *m, const uint8_t *c, uint64_t clen)
{
    uint32_t block[2];
    uint8_t buffer[ASCON_RATE];

    /* Process ciphertext blocks */
    while (clen >= ASCON_RATE)
    {
        from_bytes(block, c);
        to_bytes(buffer, state->x0);

        for (int i = 0; i < ASCON_RATE; i++)
        {
            m[i] = buffer[i] ^ c[i];
        }

        from_bytes(state->x0, c);
        ascon_permutation(state, ASCON_PB_ROUNDS);
        m += ASCON_RATE;
        c += ASCON_RATE;
        clen -= ASCON_RATE;
    }

    /* Process last block */
    if (clen > 0)
    {
        to_bytes(buffer, state->x0);
        for (int i = 0; i < clen; i++)
        {
            m[i] = buffer[i] ^ c[i];
        }

        /* Apply padding to plaintext */
        buffer[clen] = buffer[clen] ^ 0x80;
        from_bytes(block, buffer);
        state->x0[0] = block[0];
        state->x0[1] = block[1];
    }
    else
    {
        /* Empty block with padding */
        state->x0[0] ^= 0x80000000;
    }
}

/* Finalize and generate tag */
void ascon_finalize(ascon_state_t *state, const uint8_t *key)
{
    /* XOR key to the state */
    state->x1[0] ^= ((uint32_t *)key)[0];
    state->x1[1] ^= ((uint32_t *)key)[1];
    state->x2[0] ^= ((uint32_t *)key)[2];
    state->x2[1] ^= ((uint32_t *)key)[3];

    /* Apply pa permutation rounds */
    ascon_permutation(state, ASCON_PA_ROUNDS);

    /* XOR key to the state to get the tag */
    state->x3[0] ^= ((uint32_t *)key)[0];
    state->x3[1] ^= ((uint32_t *)key)[1];
    state->x4[0] ^= ((uint32_t *)key)[2];
    state->x4[1] ^= ((uint32_t *)key)[3];
}

/* Ascon AEAD encryption */
int cipher_encrypt(const unsigned char *key, const unsigned char *nonce,
                   const unsigned char *ad, unsigned long long adlen,
                   const unsigned char *m, unsigned long long mlen,
                   unsigned char *c, unsigned long long *clen)
{
    ascon_state_t state;

    /* Set ciphertext length (plaintext length + tag length) */
    *clen = mlen + ASCON_ABYTES;

    /* Initialize Ascon state */
    ascon_initialize(&state, key, nonce);

    /* Process associated data */
    ascon_absorb_associated_data(&state, ad, adlen);

    /* Process plaintext */
    ascon_absorb_plaintext(&state, c, m, mlen);

    /* Finalize and generate tag */
    ascon_finalize(&state, key);

    /* Append tag to ciphertext */
    to_bytes(c + mlen, state.x3);
    to_bytes(c + mlen + 8, state.x4);

    return 0;
}

/* Ascon AEAD decryption */
int cipher_decrypt(const unsigned char *key, const unsigned char *nonce,
                   const unsigned char *ad, unsigned long long adlen,
                   const unsigned char *c, unsigned long long clen,
                   unsigned char *m, unsigned long long *mlen)
{
    ascon_state_t state;
    uint8_t tag[ASCON_ABYTES];

    /* Check minimum ciphertext length (at least a tag) */
    if (clen < ASCON_ABYTES)
        return -1;

    /* Set plaintext length (ciphertext length - tag length) */
    *mlen = clen - ASCON_ABYTES;

    /* Initialize Ascon state */
    ascon_initialize(&state, key, nonce);

    /* Process associated data */
    ascon_absorb_associated_data(&state, ad, adlen);

    /* Process ciphertext */
    ascon_absorb_ciphertext(&state, m, c, *mlen);

    /* Finalize and generate tag */
    ascon_finalize(&state, key);

    /* Extract expected tag from state */
    to_bytes(tag, state.x3);
    to_bytes(tag + 8, state.x4);

    /* Verify tag (constant-time comparison) */
    unsigned char result = 0;
    for (unsigned i = 0; i < ASCON_ABYTES; ++i)
    {
        result |= (tag[i] ^ c[*mlen + i]);
    }

    /* Return 0 if tag is valid, -1 otherwise */
    return (result == 0) ? 0 : -1;
}

/* Ascon AEAD encryption wrapper for the LCB framework */
int ascon_encrypt(unsigned char *c, const unsigned char *m, unsigned long long mlen, const unsigned char *k)
{
    // Use default values for nonce (all zeros) and associated data (NULL)
    unsigned char nonce[CRYPTO_NPUBBYTES] = {0};
    unsigned long long clen;

    // Call with default values for nonce and associated data
    return cipher_encrypt(k, nonce, NULL, 0, m, mlen, c, &clen);
}

/* Ascon AEAD decryption wrapper for the LCB framework */
int ascon_decrypt(unsigned char *m, const unsigned char *c, unsigned long long clen, const unsigned char *k)
{
    // Use default values for nonce (all zeros) and associated data (NULL)
    unsigned char nonce[CRYPTO_NPUBBYTES] = {0};
    unsigned long long mlen;

    // Call with default values for nonce and associated data
    return cipher_decrypt(k, nonce, NULL, 0, c, clen, m, &mlen);
}
