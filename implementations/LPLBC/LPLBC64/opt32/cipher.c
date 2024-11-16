#include "api.h"

#include "cipher.h"
#include "LPLBC.h"

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned long u32;

static int C[128] = {27, 91, 51, 112, 93, 53, 1, 97, 30, 113, 92, 0,
                     29, 52, 28, 32, 99, 127, 3, 15, 70, 50, 121, 86,
                     85, 120, 80, 106, 59, 36, 33, 46, 100, 114, 56,
                     103, 96, 81, 49, 4, 16, 117, 118, 7, 19, 71, 39,
                     10, 34, 101, 22, 62, 107, 5, 17, 76, 60, 66, 8,
                     109, 42, 20, 74, 37, 40, 72, 115, 44, 47, 94, 54,
                     11, 13, 57, 104, 26, 111, 90, 125, 68, 83, 78,
                     64, 122, 87, 23, 24, 88, 123, 63, 77, 82, 67, 110,
                     43, 124, 89, 25, 12, 108, 75, 61, 73, 41, 65, 21,
                     38, 9, 18, 6, 116, 48, 95, 55, 45, 102, 35, 58,
                     105, 79, 84, 119, 69, 14, 2, 126, 98, 31};
static int sbox1[16] = {0x0, 0xF, 0xE, 0x5, // 4*4S2盒
                        0xD, 0x3, 0x6, 0xC,
                        0xB, 0x9, 0xA, 0x8,
                        0x7, 0x4, 0x2, 0x1};
static int sbox3[16] = {0x4, 0x5, 0xE, 0x9, // 4*4S盒
                        0xB, 0x8, 0xD, 0xA,
                        0xF, 0xC, 0x3, 0x6,
                        0x1, 0x0, 0x7, 0x2};
static int sbox2[256] = {0x52, 0x5B, 0x72, 0x7B, 0x5D, 0xD4, 0x7F, 0x76, 0x53, 0x5A, 0x73, 0x7A, 0xD5, 0x5C, 0x77, 0x7E, 0x84, 0x0B, 0x66, 0x6B, 0x0D, 0x02, 0x6F, 0x62, 0x03, 0x0C, 0x63, 0x6E, 0x85, 0x0A, 0x67, 0x6A, 0xF0, 0x79, 0xD0, 0x59, 0x7D, 0xF4, 0x5F, 0x56, 0xF1, 0x78, 0xD1, 0x58, 0xF5, 0x7C, 0x57, 0x5E, 0xE4, 0x69, 0x06, 0x09, 0x6D, 0xE0, 0x0F, 0x80, 0xE1, 0x6C, 0x81, 0x0E, 0xE5, 0x68, 0x07, 0x08, 0xA4, 0x29, 0x26, 0x2B, 0x2D, 0xA0, 0x2F, 0x22, 0xA1, 0x2C, 0x23, 0x2E, 0xA5, 0x28, 0x27, 0x2A, 0xB0, 0x39, 0x32, 0x3B, 0x3D, 0xB4, 0x3F, 0x36, 0xB1, 0x38, 0x33, 0x3A, 0xB5, 0x3C, 0x37, 0x3E, 0xC4, 0x4B, 0x46, 0x49, 0x4D, 0x42, 0x4F, 0xC0, 0x43, 0x4C, 0xC1, 0x4E, 0xC5, 0x4A, 0x47, 0x48, 0x12, 0x1B, 0x90, 0x19, 0x1D, 0x94, 0x1F, 0x16, 0x13, 0x1A, 0x91, 0x18, 0x95, 0x1C, 0x17, 0x1E, 0x8C, 0x8B, 0xEE, 0xEB, 0x8D, 0x8A, 0xEF, 0xEA, 0x83, 0x04, 0xE3, 0xE6, 0x05, 0x82, 0xE7, 0xE2, 0xDA, 0xDB, 0xFA, 0xFB, 0xDD, 0xDC, 0xFF, 0xFE, 0xD3, 0xD2, 0xF3, 0xF2, 0x55, 0x54, 0xF7, 0xF6, 0xEC, 0xE9, 0x8E, 0x89, 0xED, 0xE8, 0x8F, 0x88, 0x61, 0x64, 0x01, 0x86, 0x65, 0x60, 0x87, 0x00, 0xF8, 0xF9, 0xD8, 0xD9, 0xFD, 0xFC, 0xDF, 0xDE, 0x71, 0x70, 0x51, 0x50, 0x75, 0x74, 0xD7, 0xD6, 0xAC, 0xA9, 0xAE, 0xAB, 0xAD, 0xA8, 0xAF, 0xAA, 0x21, 0x24, 0xA3, 0xA6, 0x25, 0x20, 0xA7, 0xA2, 0xB8, 0xB9, 0xBA, 0xBB, 0xBD, 0xBC, 0xBF, 0xBE, 0x31, 0x30, 0xB3, 0xB2, 0x35, 0x34, 0xB7, 0xB6, 0xCC, 0xCB, 0xCE, 0xC9, 0xCD, 0xCA, 0xCF, 0xC8, 0xC3, 0x44, 0x41, 0xC6, 0x45, 0xC2, 0xC7, 0x40, 0x9A, 0x9B, 0x98, 0x99, 0x9D, 0x9C, 0x9F, 0x9E, 0x93, 0x92, 0x11, 0x10, 0x15, 0x14, 0x97, 0x96}; // 4*4S1盒
static int P1[16] = {10, 4, 5, 11, 1, 2, 9, 12, 13, 6, 0, 3, 7, 8, 15, 14};                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                               // P1盒
static int P2[16] = {10, 7, 4, 2, 3, 12, 5, 15, 11, 1, 6, 8, 14, 9, 13, 0};                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                               // P2盒

static void CharToBit(u8 input[], int output[], int n)
{
    for (int j = 0; j < n; j++)
    {
        for (int i = 7; i >= 0; i--)
        {
            output[j * 8 + (7 - i)] = (input[j] >> i) & 1;
        }
    }
}
u32 array_to_u32(int array[])
{
    u32 result = 0;

    for (int i = 0; i < 32; i++)
    {
        result = (result << 1) | array[i];
    }

    return result;
}

void u32_to_array(u32 number, int array[32])
{
    for (int i = 31; i >= 0; i--)
    {
        array[31 - i] = (number >> i) & 1;
    }
}
void u16_to_binary_array(u16 u16_data, int binary_array[16])
{
    for (int i = 15; i >= 0; i--)
    {
        binary_array[15 - i] = (u16_data >> i) & 1;
    }
}
u8 rotate_right(u8 value, int shift)
{
    return (value >> shift) | (value << (8 - shift));
}
u16 rotate_right_u16(u16 value, int shift)
{
    return (value >> shift) | (value << (16 - shift));
}

u16 rotate_left_u16(u16 value, int shift)
{
    return (value << shift) | (value >> (16 - shift));
}

u16 binary_array_to_u16(int binary_array[16])
{
    u16 u16_data = 0;
    for (int i = 0; i < 16; i++)
    {
        u16_data = (u16_data << 1) | binary_array[i];
    }
    return u16_data;
}
void numToBinaryArray(u16 num, int binary[])
{
    int i;
    for (i = 15; i >= 0; i--)
    {
        binary[i] = num & 1; // 获取最低位的二进制数字
        num >>= 1;           // 右移一位，获取一个二进制数字
    }
}
u16 binaryArrayToNum(int binary[])
{
    u16 num = 0;
    int i;
    for (i = 0; i < 16; i++)
    {
        num <<= 1;        // 左移一位，为下一个二进制位腾出空间
        num |= binary[i]; // 设置当前二进制位的值
    }
    return num;
}

// 密钥扩展
static void E(int temp1[64], int tempout[64])
{
    int temp1_sboxin1 = 0;
    int temp1_sboxin2 = 0;
    int temp1_sboxout = 0;
    int R1 = 59, R2 = 77;
    int temp1_sboxout1, temp1_sboxout2, temp1_sboxout_R;
    for (int i = 0; i < 64; i++)
    {
        tempout[i] = temp1[i];
    }
    for (int i = 0; i < 2; i++)
    {
        temp1_sboxin1 = 0;
        temp1_sboxin2 = 0;
        for (int j = 0; j < 4; j++)
        {
            temp1_sboxin1 = temp1[i * 32 + j] << (3 - j) | temp1_sboxin1;
            temp1_sboxin2 = temp1[i * 32 + 4 + j] << (3 - j) | temp1_sboxin2;
        }
        temp1_sboxout1 = sbox1[temp1_sboxin1];
        temp1_sboxout2 = sbox1[temp1_sboxin2];
        for (int j = 0; j < 4; j++)
        {
            tempout[i * 32 + j] = ((temp1_sboxout1 >> (3 - j)) & 1);
            tempout[i * 32 + 4 + j] = ((temp1_sboxout2 >> (3 - j)) & 1);
        }
    }
    for (int i = 0; i < 2; i++)
    {
        temp1_sboxout = 0;
        for (int j = 0; j < 8; j++)
        {
            temp1_sboxout = tempout[i * 32 + 16 + j] << (7 - j) | temp1_sboxout;
        }
        if (i == 0)
        {
            temp1_sboxout_R = temp1_sboxout ^ R1;
        }
        else
        {
            temp1_sboxout_R = temp1_sboxout ^ R2;
        }
        for (int j = 0; j < 8; j++)
        {
            tempout[i * 32 + 16 + j] = ((temp1_sboxout_R >> (7 - j)) & 1);
        }
    }
}
static void SubKey(u8 K[16], u16 subkey[18][2])
{
    int temp1[64] = {0};
    int tempout1[64] = {0};
    int temp2[64] = {0};
    int tempout2[64] = {0};
    int Ka[128] = {0};
    int Kb[128] = {0};
    CharToBit(K, Kb, 16);
    int K1[32] = {0};
    int K2[32] = {0};
    int K3[32] = {0};
    int K4[32] = {0};
    int K5[64] = {0};
    int K6[64] = {0};
    int RK[128] = {0};
    int subkey_temp[32] = {0};
    int temp1_sboxin = 0;
    int temp1_sboxout, temp1_sboxout1, temp1_sboxout2;
    int temp2_sboxin;
    int temp2_sboxout;
    int R1 = 59, R2 = 77;
    for (int Nr = 0; Nr < 18; Nr++)
    {
        for (int i = 0; i < 128; i++)
        {
            Ka[C[i]] = Kb[i];
        }
        for (int i = 0; i < 32; i++)
        {
            K1[i] = Ka[i];
            K2[i] = Ka[i + 32];
            K3[i] = Ka[i + 64];
            K4[i] = Ka[i + 96];
        }
        if (Nr % 3 == 0)
        {
            for (int i = 0; i < 32; i++)
            {
                temp1[i] = K1[i];
                temp1[i + 32] = K2[i];
            }
            E(temp1, tempout1);
            for (int j = 0; j < 64; j++)
            {
                K5[j] = tempout1[(j + 21) % 64];
            }
            for (int i = 0; i < 32; i++)
            {
                temp2[i] = K3[i];
                temp2[i + 32] = K4[i];
            }
            E(temp2, tempout2);
            for (int j = 0; j < 64; j++)
            {
                K6[j] = tempout2[(j + 13) % 64];
            }
            for (int i = 0; i < 64; i++)
            {
                RK[i] = K5[i];
                RK[i + 64] = K6[i];
            }
        }
        if (Nr % 3 == 1)
        {
            for (int i = 0; i < 32; i++)
            {
                temp1[i] = K1[i];
                temp1[i + 32] = K3[i];
            }
            E(temp1, tempout1);
            for (int j = 0; j < 64; j++)
            {
                K5[j] = tempout1[(j + 7) % 64];
            }
            for (int i = 0; i < 32; i++)
            {
                temp2[i] = K2[i];
                temp2[i + 32] = K4[i];
            }
            E(temp2, tempout2);
            for (int j = 0; j < 64; j++)
            {
                K6[j] = tempout2[(j + 35) % 64];
            }
            for (int i = 0; i < 64; i++)
            {
                RK[i] = K5[i];
                RK[i + 64] = K6[i];
            }
        }
        if (Nr % 3 == 2)
        {
            for (int i = 0; i < 32; i++)
            {
                temp1[i] = K1[i];
                temp1[i + 32] = K4[i];
            }
            E(temp1, tempout1);
            for (int j = 0; j < 64; j++)
            {
                K5[j] = tempout1[(j + 63) % 64];
            }
            for (int i = 0; i < 32; i++)
            {
                temp2[i] = K2[i];
                temp2[i + 32] = K3[i];
            }
            E(temp2, tempout2);
            for (int j = 0; j < 64; j++)
            {
                K6[j] = tempout2[(j + 49) % 64];
            }
            for (int i = 0; i < 64; i++)
            {
                RK[i] = K5[i];
                RK[i + 64] = K6[i];
            }
        }
        for (int j = 0; j < 32; j++)
        {
            subkey_temp[j] = RK[j];
        }
        for (int j = 0; j < 2; j++)
        {
            subkey[Nr][j] = 0xffff & (subkey_temp[j * 16] << 15 | subkey_temp[j * 16 + 1] << 14 | subkey_temp[j * 16 + 2] << 13 | subkey_temp[j * 16 + 3] << 12 | subkey_temp[j * 16 + 4] << 11 | subkey_temp[j * 16 + 5] << 10 | subkey_temp[j * 16 + 6] << 9 | subkey_temp[j * 16 + 7] << 8 | subkey_temp[j * 16 + 8] << 7 | subkey_temp[j * 16 + 9] << 6 | subkey_temp[j * 16 + 10] << 5 | subkey_temp[j * 16 + 11] << 4 | subkey_temp[j * 16 + 12] << 3 | subkey_temp[j * 16 + 13] << 2 | subkey_temp[j * 16 + 14] << 1 | subkey_temp[j * 16 + 15]);
        }
        for (int i = 0; i < 128; i++)
        {
            Kb[i] = RK[i];
        }
    }
}

void F1(u16 in, int p_out[16])
{
    u16 s_temp;
    u16 p_out_u16;
    u16 M, M_temp;
    u8 M0, M1;
    u8 M0_1, M1_1;
    int in_array[16] = {0};
    int s_out[16] = {0};
    u16_to_binary_array(in, in_array);
    for (int i = 0; i < 16; i++)
    {
        p_out[P1[i]] = in_array[i];
    }
    p_out_u16 = binary_array_to_u16(p_out);
    s_temp = sbox1[((p_out_u16 & 0xf000) >> 12)] << 12 | sbox1[((p_out_u16 & 0x0f00) >> 8)] << 8 | sbox1[((p_out_u16 & 0x00f0) >> 4)] << 4 | sbox1[(p_out_u16 & 0x000f)];
    for (int i = 0; i < 16; i++)
    {
        s_out[i] = (s_temp >> (15 - i)) & 1;
    }
    M = binary_array_to_u16(s_out);
    M0 = M >> 8 & 0xff;
    M1 = M & 0xff;
    M0_1 = M0 ^ rotate_right(M0, 2) ^ rotate_right(M0, 6);
    M1_1 = rotate_right(M1, 3) ^ rotate_right(M1, 4) ^ rotate_right(M1, 7);
    M_temp = (M0_1 << 8) & 0xff00 | M1_1 & 0x00ff;
    u16_to_binary_array(M_temp, p_out);
}
void F2(u16 in1, u16 in2, int p_out[16])
{
    u16 s_temp;
    u16 M, M_temp;
    u8 M0, M1;
    u8 M0_1, M1_1;
    u16 s_input;
    int s_out[16] = {0};
    int p_out_temp[16] = {0};
    u16 in;
    in = in1 ^ in2;
    s_temp = sbox2[((in & 0xff00) >> 8)] << 8 | sbox2[(in & 0x00ff)];
    for (int i = 0; i < 16; i++)
    {
        s_out[i] = (s_temp >> (15 - i)) & 1;
    }
    for (int i = 0; i < 16; i++)
    {
        p_out[P2[i]] = s_out[i];
    }
}
void F3(u16 in, int p_out[16])
{
    u16 s_temp;
    u16 p_out_u16;
    u16 M, M_temp;
    u8 M0, M1;
    u8 M0_1, M1_1;
    int in_array[16] = {0};
    int s_out[16] = {0};
    u16_to_binary_array(in, in_array);
    M = in;
    M0 = M >> 8 & 0xff;
    M1 = M & 0xff;
    M0_1 = M0 ^ rotate_right(M0, 2) ^ rotate_right(M0, 6);
    M1_1 = rotate_right(M1, 3) ^ rotate_right(M1, 4) ^ rotate_right(M1, 7);
    M_temp = (M0_1 << 8) & 0xff00 | M1_1 & 0x00ff;
    s_temp = sbox1[((M_temp & 0xf000) >> 12)] << 12 | sbox1[((M_temp & 0x0f00) >> 8)] << 8 | sbox1[((M_temp & 0x00f0) >> 4)] << 4 | sbox1[(M_temp & 0x000f)];
    for (int i = 0; i < 16; i++)
    {
        s_out[i] = (s_temp >> (15 - i)) & 1;
    }
    for (int i = 0; i < 16; i++)
    {
        p_out[P1[i]] = s_out[i];
    }
}

int crypto_encrypt(unsigned char *c,
                   const unsigned char *m, unsigned long long mlen,
                   const unsigned char *k)
{
    
    u8 K[16];
    u16 p[4];
    u16 cipher[4];
    // transform the input to the internal format
    // m is 8-bit array, need m to the u16 p[4]
    for (int i = 0; i < 4; i++)
    {
        p[i] = m[i] << 8;
        p[i] = m[i + 1];
    }
    // k is 128-bit, k to the u8 K[16]
    for (int i = 0; i < 16; i++)
    {
        K[i] = k[i];
    }

    u16 subkey[18][2] = {0};
    u8 temp[8] = {0};
    u16 p_F[4] = {0};
    u16 p_1[4] = {0};
    int F_out1[16] = {0};
    int F_out2[16] = {0};
    int P_F2_output[16] = {0};
    int P_F3_output[16] = {0};
    u16 Tin;
    int p_F_array[16] = {0};
    u16 p_temp[4] = {0};
    u16 w1;
    int round_num = 18;
    SubKey(K, subkey);
    for (int i = 0; i < round_num; i++)
    {
        u16 p_subadd[2] = {0};
        u16 p_l[2] = {0};
        if (i % 2 == 0)
        {
            p_subadd[0] = p[0] ^ subkey[i][0];
            F1(p_subadd[0], p_F_array);
            p_F[0] = binary_array_to_u16(p_F_array);
            F2(rotate_left_u16(p_F[0], 5), p[3], P_F2_output);
            p_F[1] = binary_array_to_u16(P_F2_output);
            p_F[2] = binary_array_to_u16(P_F2_output);
            p_F[2] = p_F[2] << 9;
            F3(p[3], P_F3_output);
            p_F[3] = binary_array_to_u16(P_F3_output);
            p_subadd[1] = p_F[3] ^ subkey[i][1];
            p_temp[3] = p_F[2] ^ p[2];
            p_temp[1] = p_F[0];
            p_temp[2] = p_subadd[1];
            p_temp[0] = p_F[1] ^ p[1];
            for (int i = 0; i < 4; i++)
            {
                p[i] = p_temp[i];
            }
        }
        else
        {
            p_subadd[0] = p[0] ^ subkey[i][0];
            F1(p_subadd[0], p_F_array);
            p_F[0] = binary_array_to_u16(p_F_array);
            F2(p_F[0], rotate_left_u16(p[3], 5), P_F2_output);
            p_F[1] = binary_array_to_u16(P_F2_output);
            p_F[2] = binary_array_to_u16(P_F2_output);
            p_F[1] = p_F[1] << 9;
            F3(p[3], P_F3_output);
            p_F[3] = binary_array_to_u16(P_F3_output);
            p_subadd[1] = p_F[3] ^ subkey[i][1];
            p_temp[1] = p_F[2] ^ p[2];
            p_temp[3] = p_F[0];
            p_temp[0] = p_subadd[1];
            p_temp[2] = p_F[1] ^ p[1];
            for (int i = 0; i < 4; i++)
            {
                p[i] = p_temp[i];
            }
        }
    }
    for (int i = 0; i < 4; i++)
    {
        c[i] = (p[i] & 0xff00 >> 8);
        c[i + 1] = p[i] & 0x00ff;
    }
    return 0;
}

int crypto_decrypt(unsigned char *m,
                   const unsigned char *c, unsigned long long clen,
                   const unsigned char *k)
{

    return 0;
}
