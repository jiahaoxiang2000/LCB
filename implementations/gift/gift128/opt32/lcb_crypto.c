#include "lcb_crypto.h"
#include "api.h"

// Add prototypes for cipher_encrypt and cipher_decrypt if not already declared
int cipher_encrypt(const unsigned char *key, const unsigned char *nonce, const unsigned char *in, unsigned char *out, unsigned long long inlen);
int cipher_decrypt(const unsigned char *key, const unsigned char *nonce, const unsigned char *in, unsigned char *out, unsigned long long inlen);

cipher_ctx lcb_cipher = {
    "gift128",
    "opt32",
    CRYPTO_KEYBYTES,
    cipher_encrypt,
    cipher_decrypt};
