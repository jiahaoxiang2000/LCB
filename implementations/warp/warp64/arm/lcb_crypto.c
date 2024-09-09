#include "lcb_crypto.h"
#include "api.h"


cipher_ctx lcb_cipher = {
	"warp64",
	"arm",
	CRYPTO_KEYBYTES,
	crypto_encrypt,
	crypto_decrypt
};

