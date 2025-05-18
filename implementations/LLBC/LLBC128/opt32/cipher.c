#include "api.h"
#include "cipher.h"
#include "LLBC.h"

// Example: block size 16 bytes (128 bits)
#define BLOCK_SIZE 16

static u32 KeyAll_1[r][4] = {0};

void Subcell(u32 Plaintext[4])
{
    u32 Plaintext_sbox_temp1[2] = {Plaintext[0], Plaintext[1]};
    u32 Plaintext_sbox_temp2[2] = {Plaintext[0], Plaintext[1]};
    // Rotate
    ROL64(&Plaintext_sbox_temp1[0], 32);
    ROL64(&Plaintext_sbox_temp2[0], 16);
    // S-box 1
    for (uint32_t i = 0; i < 2; i++)
    {
        Plaintext_sbox_temp1[i] = sbox_1[(Plaintext_sbox_temp1[i] >> 28) & 0x0F] << 28 |
                                  sbox_1[(Plaintext_sbox_temp1[i] >> 24) & 0x0F] << 24 |
                                  sbox_1[(Plaintext_sbox_temp1[i] >> 20) & 0x0F] << 20 |
                                  sbox_1[(Plaintext_sbox_temp1[i] >> 16) & 0x0F] << 16 |
                                  sbox_1[(Plaintext_sbox_temp1[i] >> 12) & 0x0F] << 12 |
                                  sbox_1[(Plaintext_sbox_temp1[i] >> 8) & 0x0F] << 8 |
                                  sbox_1[(Plaintext_sbox_temp1[i] >> 4) & 0x0F] << 4 |
                                  sbox_1[Plaintext_sbox_temp1[i] & 0x0F];
    }
    // S-box 2
    for (uint32_t i = 0; i < 2; i++)
    {
        Plaintext_sbox_temp2[i] = sbox_2[(Plaintext_sbox_temp2[i] >> 28) & 0x0F] << 28 |
                                  sbox_2[(Plaintext_sbox_temp2[i] >> 24) & 0x0F] << 24 |
                                  sbox_2[(Plaintext_sbox_temp2[i] >> 20) & 0x0F] << 20 |
                                  sbox_2[(Plaintext_sbox_temp2[i] >> 16) & 0x0F] << 16 |
                                  sbox_2[(Plaintext_sbox_temp2[i] >> 12) & 0x0F] << 12 |
                                  sbox_2[(Plaintext_sbox_temp2[i] >> 8) & 0x0F] << 8 |
                                  sbox_2[(Plaintext_sbox_temp2[i] >> 4) & 0x0F] << 4 |
                                  sbox_2[Plaintext_sbox_temp2[i] & 0x0F];
    }
    Plaintext[0] = Plaintext_sbox_temp1[0];
    Plaintext[1] = Plaintext_sbox_temp1[1];
    Plaintext[2] = Plaintext_sbox_temp2[0];
    Plaintext[3] = Plaintext_sbox_temp2[1];
}

void Crypt_Encryption(u32 Plaintext[4], u32 Key[4], int CryptRound)
{
    u32 Plaintext_left64_temp[2], Plaintext_right64_temp[2], Plaintext_temp1[2], Plaintext_temp2[2], Plaintext_temp3[2], Plaintext_temp4[2], Plaintext_temp5[2], Plaintext_right64_start[2];
    for (int j = 0; j < CryptRound; j++)
    {
        for (int i = 0; i < 4; i++)
            Key[i] = KeyAll_1[j][i];
        for (int i = 0; i < 2; i++)
            Plaintext_left64_temp[i] = Plaintext[i];
        Plaintext_left64_temp[0] ^= (Key[0] ^ RC_Encryption);
        Plaintext_left64_temp[1] ^= Key[1];
        for (int i = 0; i < 2; i++)
            Plaintext_right64_start[i] = Plaintext[i + 2];
        Subcell(Plaintext);
        for (int i = 0; i < 2; i++)
            Plaintext_right64_temp[i] = Plaintext[i + 2];
        for (int i = 0; i < 2; i++)
            Plaintext_temp4[i] = Plaintext_right64_temp[i];
        for (int i = 0; i < 2; i++)
            Plaintext_temp1[i] = Plaintext[i];
        for (int i = 0; i < 2; i++)
            Plaintext_right64_temp[i] ^= Plaintext_temp1[i];
        for (int i = 0; i < 2; i++)
            Plaintext_temp2[i] = Plaintext_right64_temp[i];
        ROL64(&Plaintext_right64_temp[0], 32);
        for (int i = 0; i < 2; i++)
            Plaintext_temp1[i] ^= Plaintext_right64_temp[i];
        for (int i = 0; i < 2; i++)
            Plaintext_temp3[i] = Plaintext_temp1[i];
        ROL64(&Plaintext_temp1[0], 16);
        for (int i = 0; i < 2; i++)
            Plaintext_temp2[i] ^= Plaintext_temp1[i];
        for (int i = 0; i < 2; i++)
            Plaintext_right64_temp[i] = Plaintext_temp2[i];
        ROL64(&Plaintext_temp2[0], 32);
        for (int i = 0; i < 2; i++)
            Plaintext_temp3[i] ^= Plaintext_temp2[i];
        for (int i = 0; i < 2; i++)
            Plaintext_temp2[i] = Plaintext_temp3[i];
        ROL64(&Plaintext_temp3[0], 16);
        for (int i = 0; i < 2; i++)
            Plaintext_right64_temp[i] ^= Plaintext_temp3[i];
        for (int i = 0; i < 2; i++)
            Plaintext_temp5[i] = Plaintext_temp2[i] ^ Plaintext_right64_temp[i];
        Plaintext_temp5[0] ^= Key[2];
        Plaintext_temp5[1] ^= Key[3];
        Plaintext[0] = Plaintext_temp5[0] ^ Plaintext_right64_start[0];
        Plaintext[1] = Plaintext_temp5[1] ^ Plaintext_right64_start[1];
        Plaintext[2] = Plaintext_left64_temp[0];
        Plaintext[3] = Plaintext_left64_temp[1];
    }
}

void Crypt_Decryption(u32 Plaintext[4], u32 Key[4], int CryptRound)
{
    // Not implemented in main.cpp
}

void Key_Schedule(u32 Key[4], int CryptRound)
{
    u32 Key_temp0, Key_temp1, Key_temp2, Key_temp3;
    uint16_t a, b;
    uint16_t Key_temp0_16, Key_temp1_16, Key_temp2_16, Key_temp3_16;
    for (int c = 0; c < CryptRound; c++)
    {
        Key_temp0 = Key[0];
        Key_temp1 = Key[1];
        Key_temp2 = Key[2];
        Key_temp3 = Key[3];
        Key_temp0_16 = (Key_temp2 & 0xFFFF0000) >> 16;
        Key_temp1_16 = Key_temp2 & 0x0000FFFF;
        Key_temp2_16 = (Key_temp3 & 0xFFFF0000) >> 16;
        Key_temp3_16 = Key_temp3 & 0x0000FFFF;
        for (int i = 0; i < 2; i++)
        {
            Key_temp0_16 = ROL16(Key_temp0_16, 3);
            a = Key_temp0_16;
            Key_temp0_16 ^= Key_temp2_16;
            Key_temp2_16 = a;
            Key_temp1_16 = ROL16(Key_temp1_16, 1);
            b = Key_temp1_16;
            Key_temp1_16 ^= Key_temp3_16;
            Key_temp3_16 = b;
        }
        Key[2] = (Key_temp0_16 << 16) + Key_temp1_16;
        Key[3] = (Key_temp2_16 << 16) + Key_temp3_16;
        Key[0] ^= Key[2];
        Key[1] ^= Key[3];
        Key[1] ^= RC_Encryption;
        Key[2] ^= RC_KS;
        for (int i = 0; i < 4; i++)
            KeyAll_1[c][i] = Key[i];
    }
}

// Simple ECB mode for demonstration (replace with real mode as needed)
int cipher_encrypt(
    unsigned char *c,
    const unsigned char *m, unsigned long long mlen,
    const unsigned char *k)
{
    u32 key[4];
    for (int i = 0; i < 4; ++i)
        key[i] = ((u32 *)k)[i];
    for (unsigned long long i = 0; i < mlen; i += BLOCK_SIZE)
    {
        u32 block[4] = {0};
        for (int j = 0; j < 4; ++j)
            block[j] = ((u32 *)(m + i))[j];
        Crypt_Encryption(block, key, r);
        for (int j = 0; j < 4; ++j)
            ((u32 *)(c + i))[j] = block[j];
    }
    return 0;
}

int cipher_decrypt(
    unsigned char *m,
    const unsigned char *c, unsigned long long clen,
    const unsigned char *k)
{
    u32 key[4];
    for (int i = 0; i < 4; ++i)
        key[i] = ((u32 *)k)[i];
    for (unsigned long long i = 0; i < clen; i += BLOCK_SIZE)
    {
        u32 block[4] = {0};
        for (int j = 0; j < 4; ++j)
            block[j] = ((u32 *)(c + i))[j];
        Crypt_Decryption(block, key, r);
        for (int j = 0; j < 4; ++j)
            ((u32 *)(m + i))[j] = block[j];
    }
    return 0;
}
