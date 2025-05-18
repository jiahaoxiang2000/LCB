#ifndef LLBC_h
#define LLBC_h

#include <stdio.h>
#include <stdint.h>

/* r为 轮数 */
#define r 15
#define u8 unsigned char
#define u32 uint32_t
#define ROL16(x,a) (((x)<<(a))|((x)>>(16-(a))))
#define ROL64(x,a) {\
    if (a == 32){\
        u32 ROL64_tmp = (x)[0];\
        (x)[0] = (x)[1];\
        (x)[1] = ROL64_tmp;\
    }\
    else {\
    u32 ROL64_tmp = (x)[0] << (a) | (x)[1] >> (32-a);\
    (x)[1] = (x)[1] << (a) | (x)[0] >> (32 - a);\
    (x)[0] = ROL64_tmp;}\
} //循环左移
#define ROR64(x,a) {\
    if (a == 32){\
        u32 ROR64_tmp = (x)[0];\
        (x)[0] = (x)[1];\
        (x)[1] = ROR64_tmp;\
    }\
    else {\
    u32 ROR64_tmp = (x)[0] >> (a) | (x)[1] << (32-a);\
    (x)[1] = (x)[1] >> (a) | (x)[0] << (32 - a);\
    (x)[0] = ROR64_tmp;}\
}//循环右移

// S-box definitions
static const int sbox_1[16] = {0x4, 0x7, 0x5, 0x1, 0xc, 0xb, 0xd, 0x8, 0xe, 0xf, 0x6, 0x3, 0xa, 0x9, 0x2, 0x0};
static const int sbox_2[16] = {0xe, 0xc, 0x8, 0x4, 0xb, 0xd, 0x1, 0x5, 0xf, 0x3, 0xa, 0x2, 0x9, 0x7, 0x0, 0x6};

// Round constants
static const u32 RC_Encryption = 0x243F6A88;
static const u32 RC_KS = 0x85A308D3;

// Function declarations
void Subcell(u32 Plaintext[4]);
void Crypt_Encryption(u32 Plaintext[4], u32 Key[4], int CryptRound);
void Crypt_Decryption(u32 Ciphertext[4], u32 Key[4], int CryptRound);
void Key_Schedule(u32 Key[4], int CryptRound);

#endif // LLBC_h
