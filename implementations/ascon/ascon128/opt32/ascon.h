#ifndef ASCON_H_
#define ASCON_H_

#include <stdint.h>

/* Ascon parameters */
#define ASCON_128_KEYBYTES 16
#define ASCON_NPUBBYTES 16
#define ASCON_ABYTES 16
#define ASCON_RATE 8

#define ASCON_PA_ROUNDS 12
#define ASCON_PB_ROUNDS 6

typedef struct
{
    uint32_t x0[2];
    uint32_t x1[2];
    uint32_t x2[2];
    uint32_t x3[2];
    uint32_t x4[2];
} ascon_state_t;

/* Optimized implementation functions */
void ascon_initialize(ascon_state_t *state, const uint8_t *key, const uint8_t *nonce);
void ascon_absorb_associated_data(ascon_state_t *state, const uint8_t *ad, uint64_t adlen);
void ascon_absorb_plaintext(ascon_state_t *state, uint8_t *c, const uint8_t *m, uint64_t mlen);
void ascon_squeeze_ciphertext(ascon_state_t *state, uint8_t *c, const uint8_t *m, uint64_t mlen);
void ascon_finalize(ascon_state_t *state, const uint8_t *key);
void ascon_permutation(ascon_state_t *state, int rounds);

#endif /* ASCON_H_ */
