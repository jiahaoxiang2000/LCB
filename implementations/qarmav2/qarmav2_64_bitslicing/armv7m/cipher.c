#include "api.h"

#include "cipher.h"
#include "qarmav2.h"

int crypto_encrypt(unsigned char *c,
                   const unsigned char *m, unsigned long long mlen,
                   const unsigned char *k)
{

  uint32_t r0, r1, r2, r3;
  r0 = 0xffff;
  r1 = 0xff00ff;
  r2 = 0xf0f0f0f;
  r3 = 0x33333333;
  qarmav2_bitslicing();
  return 0;
}

int crypto_decrypt(unsigned char *m,
		const unsigned char *c, unsigned long long clen,
		const unsigned char *k)
{

  return 0;
}


