#include "lcb_crypto.h"
#include "api.h"

// Add prototypes for ascon_encrypt and ascon_decrypt
int ascon_encrypt(unsigned char *c, const unsigned char *m, unsigned long long mlen, const unsigned char *k);
int ascon_decrypt(unsigned char *m, const unsigned char *c, unsigned long long clen, const unsigned char *k);

// Provide implementation information for the LCB framework
cipher_ctx lcb_cipher = {
    "ASCON128",      // Cipher name
    "opt32",         // Implementation type
    CRYPTO_KEYBYTES, // Key size in bytes (16 for ASCON-128)
    ascon_encrypt,   // Encryption function
    ascon_decrypt    // Decryption function
};
