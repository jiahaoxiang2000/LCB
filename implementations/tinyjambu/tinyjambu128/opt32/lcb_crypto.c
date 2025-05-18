#include "lcb_crypto.h"
#include "api.h"

// Add prototypes for ascon_encrypt and ascon_decrypt
int cipher_encrypt(unsigned char *c, const unsigned char *m, unsigned long long mlen, const unsigned char *k);
int cipher_decrypt(unsigned char *m, const unsigned char *c, unsigned long long clen, const unsigned char *k);

cipher_ctx lcb_cipher = {
    "tinyjambu128",
    "opt32",
    CRYPTO_KEYBYTES,
    cipher_encrypt,
    cipher_decrypt};
