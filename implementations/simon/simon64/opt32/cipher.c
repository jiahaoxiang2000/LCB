#include "api.h"
#include "cipher.h"
#include <stdint.h>

#define ROTATE_RIGHT_32(x, n) (((x) >> (n)) | ((x) << (32 - (n))))
#define ROTATE_LEFT_32(x, n) (((x) << (n)) | ((x) >> (32 - (n))))


typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;

static u8 z[62] =
{1,1,0,1,1,0,1,1,1,0,1,0,1,1,0,0,0,1,1,0,0,1,0,1,1,1,1,0,0,0,0,0,0,1,0,0,1,0,0,0,1,0,1,0,0,1,1,1,0,0,1,1,0,1,0,0,0,0,1,1,1,1};

void KeyExpansion ( u32 k[] )
{
    u8 i;
    u32 tmp;
    for ( i=4 ; i<44 ; i++ )
    {
        tmp = ROTATE_RIGHT_32(k[i-1],3);
        tmp = tmp ^ k[i-3];
        tmp = tmp ^ ROTATE_RIGHT_32(tmp,1);
        k[i] = ~k[i-4] ^ tmp ^ z[i-4] ^ 3;
    }
}

void Encrypt ( u32 text[], u32 crypt[], u32 key[] )
{
    u8 i;
    u32 tmp;
    crypt[0] = text[0];
    crypt[1] = text[1];

    for ( i=0 ; i<44 ; i++ )
    {
        tmp = crypt[0];
        crypt[0] = crypt[1] ^ ((ROTATE_LEFT_32(crypt[0],1)) & (ROTATE_LEFT_32(crypt[0],8))) ^ (ROTATE_LEFT_32(crypt[0],2)) ^ key[i];
        crypt[1] = tmp;
    }
}

void Decrypt ( u32 text[], u32 crypt[], u32 key[] )
{
    u8 i;
    u32 tmp;
    crypt[0] = text[0];
    crypt[1] = text[1];

    for ( i=0 ; i<44 ; i++ )
    {
        tmp = crypt[1];
        crypt[1] = crypt[0] ^ ((ROTATE_LEFT_32(crypt[1],1)) & (ROTATE_LEFT_32(crypt[1],8))) ^ (ROTATE_LEFT_32(crypt[1],2)) ^ key[43-i];
        crypt[0] = tmp;
    }
}

int crypto_encrypt(unsigned char *c,
                   const unsigned char *m, unsigned long long mlen,
                   const unsigned char *k)
{
    u32 text[2];
    // copy c to text
    text[0] = (u32)c[0] | (u32)c[1] << 8 | (u32)c[2] << 16 | (u32)c[3] << 24;
    text[1] = (u32)c[4] | (u32)c[5] << 8 | (u32)c[6] << 16 | (u32)c[7] << 24;

    u32 crypt[2] = {0};
    u32 key[44];
    key[3] = 0x1b1a1918;
    key[2] = 0x13121110;
    key[1] = 0x0b0a0908;
    key[0] = 0x03020100;
    KeyExpansion ( key );

    KeyExpansion ( key );
    Encrypt ( text, crypt, key );

    // copy crypt to c
    c[0] = crypt[0] & 0xff;
    c[1] = (crypt[0] >> 8) & 0xff;
    c[2] = (crypt[0] >> 16) & 0xff;
    c[3] = (crypt[0] >> 24) & 0xff;
    c[4] = crypt[1] & 0xff;
    c[5] = (crypt[1] >> 8) & 0xff;
    c[6] = (crypt[1] >> 16) & 0xff;
    c[7] = (crypt[1] >> 24) & 0xff;
  return 0;
}

int crypto_decrypt(unsigned char *m,
		const unsigned char *c, unsigned long long clen,
		const unsigned char *k)
{

  
  return 0;
}


