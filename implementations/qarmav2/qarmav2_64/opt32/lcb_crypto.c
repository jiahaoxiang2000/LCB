#include "lcb_crypto.h"
#include "api.h"

cipher_ctx lcb_cipher = {
	"qarmav2_64",
	"opt32",
	CRYPTO_KEYBYTES,
	crypto_encrypt,
	crypto_decrypt
};

