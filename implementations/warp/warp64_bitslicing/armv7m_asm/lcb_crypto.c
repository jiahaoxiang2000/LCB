#include "lcb_crypto.h"
#include "api.h"


cipher_ctx lcb_cipher = {
	"warp64_bitslicing",
	"armv7m_asm",
	CRYPTO_KEYBYTES,
	crypto_encrypt,
	crypto_decrypt
};

