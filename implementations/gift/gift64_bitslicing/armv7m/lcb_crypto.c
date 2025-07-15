#include "lcb_crypto.h"
#include "api.h"
#include "gift64.h"

cipher_ctx lcb_cipher = {
    "gift64_bitslicing",
    "armv7m",
    CRYPTO_KEYBYTES,
    cipher_encrypt,
    cipher_decrypt};