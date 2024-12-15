#include "lcb_crypto.h"
#include "api.h"


cipher_ctx lcb_cipher = {
	"simon64",
	"opt32",
	CRYPTO_KEYBYTES,
	crypto_encrypt,
	crypto_decrypt
};
