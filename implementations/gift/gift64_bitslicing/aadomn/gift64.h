#ifndef GIFT64_H_
#define GIFT64_H_

#define KEY_SIZE 16
#define GIFT64_BLOCK_SIZE 8

typedef unsigned char u8;
typedef unsigned int u32;

extern void gift64_rearrange_key(u32 rkey[56], const u8 key[16]);
extern void giftb64_keyschedule(u32 rkey[56]);
extern void gift64_encrypt_block(u8 out_block[16], const u32 rkey[56], const u8 in_block0[8], const u8 *in_block1);
// extern void giftb64_encrypt_block(u8 out_block[16], const u32 rkey[56], const u8 in_block0[8], const u8 *in_block1);

int cipher_encrypt(unsigned char *c, const unsigned char *m, unsigned long long mlen, const unsigned char *k);

int cipher_decrypt(unsigned char *m, const unsigned char *c, unsigned long long clen, const unsigned char *k);
#endif // GIFT64_H_