#include "lcb_crypto.h"
#include "api.h"


aead_ctx lwc_aead_cipher = {
	"warp64v1",
	"armv7m-c",
	CRYPTO_KEYBYTES,
	crypto_encrypt,
	crypto_decrypt
};

