#include "lcb_crypto.h"
#include "api.h"
#include "cipher.h"

cipher_ctx lcb_cipher = {
    "aes128",
    "opt32",
    CRYPTO_KEYBYTES,
    cipher_encrypt,
    cipher_decrypt};
