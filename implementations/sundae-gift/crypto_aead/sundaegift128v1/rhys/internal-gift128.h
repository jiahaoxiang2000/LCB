/*
 * Copyright (C) 2020 Southern Storm Software, Pty Ltd.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#ifndef LW_INTERNAL_GIFT128_H
#define LW_INTERNAL_GIFT128_H

/**
 * \file internal-gift128.h
 * \brief GIFT-128 block cipher.
 *
 * There are three versions of GIFT-128 in use within the second round
 * submissions to the NIST lightweight cryptography competition.
 *
 * The most efficient version for 32-bit software implementation is the
 * GIFT-128-b bit-sliced version from GIFT-COFB and SUNDAE-GIFT.
 *
 * The second is the nibble-based version from HYENA.  We implement the
 * HYENA version as a wrapper around the bit-sliced version.
 *
 * The third version is a variant on the HYENA nibble-based version that
 * includes a 4-bit tweak value for domain separation.  It is used by
 * the ESTATE submission to NIST.
 *
 * Technically there is a fourth version of GIFT-128 which is the one that
 * appeared in the original GIFT-128 paper.  It is almost the same as the
 * HYENA version except that the byte ordering is big-endian instead of
 * HYENA's little-endian.  The original version of GIFT-128 doesn't appear
 * in any of the NIST submissions so we don't bother with it in this library.
 *
 * References: https://eprint.iacr.org/2017/622.pdf,
 * https://eprint.iacr.org/2020/412.pdf,
 * https://giftcipher.github.io/gift/
 */

#include <stddef.h>
#include <stdint.h>
#include "internal-gift128-config.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief Size of a GIFT-128 block in bytes.
 */
#define GIFT128_BLOCK_SIZE 16

/**
 * \var GIFT128_ROUND_KEYS
 * \brief Number of round keys for the GIFT-128 key schedule.
 */
#if GIFT128_VARIANT == GIFT128_VARIANT_TINY
#define GIFT128_ROUND_KEYS 4
#elif GIFT128_VARIANT == GIFT128_VARIANT_SMALL
#define GIFT128_ROUND_KEYS 20
#else
#define GIFT128_ROUND_KEYS 80
#endif

/**
 * \brief Structure of the key schedule for GIFT-128 (bit-sliced).
 */
typedef struct
{
    /** Pre-computed round keys for bit-sliced GIFT-128 */
    uint32_t k[GIFT128_ROUND_KEYS];

} gift128b_key_schedule_t;

/**
 * \brief Initializes the key schedule for GIFT-128 (bit-sliced).
 *
 * \param ks Points to the key schedule to initialize.
 * \param key Points to the 16 bytes of the key data.
 */
void gift128b_init(gift128b_key_schedule_t *ks, const unsigned char *key);

/**
 * \brief Encrypts a 128-bit block with GIFT-128 (bit-sliced).
 *
 * \param ks Points to the GIFT-128 key schedule.
 * \param output Output buffer which must be at least 16 bytes in length.
 * \param input Input buffer which must be at least 16 bytes in length.
 *
 * The \a input and \a output buffers can be the same buffer for
 * in-place encryption.
 */
void gift128b_encrypt
    (const gift128b_key_schedule_t *ks, unsigned char *output,
     const unsigned char *input);

/**
 * \brief Encrypts a 128-bit block with GIFT-128 (bit-sliced and pre-loaded).
 *
 * \param ks Points to the GIFT-128 key schedule.
 * \param output Output buffer which must be at least 16 bytes in length.
 * \param input Input buffer which must be at least 16 bytes in length.
 *
 * The \a input and \a output buffers can be the same buffer for
 * in-place encryption.
 *
 * This version assumes that the input has already been pre-loaded from
 * big-endian into host byte order in the supplied word array.  The output
 * is delivered in the same way.
 */
void gift128b_encrypt_preloaded
    (const gift128b_key_schedule_t *ks, uint32_t output[4],
     const uint32_t input[4]);

/**
 * \brief Decrypts a 128-bit block with GIFT-128 (bit-sliced).
 *
 * \param ks Points to the GIFT-128 key schedule.
 * \param output Output buffer which must be at least 16 bytes in length.
 * \param input Input buffer which must be at least 16 bytes in length.
 *
 * The \a input and \a output buffers can be the same buffer for
 * in-place decryption.
 */
void gift128b_decrypt
    (const gift128b_key_schedule_t *ks, unsigned char *output,
     const unsigned char *input);

/**
 * \brief Structure of the key schedule for GIFT-128 (nibble-based).
 */
typedef gift128b_key_schedule_t gift128n_key_schedule_t;

/**
 * \brief Initializes the key schedule for GIFT-128 (nibble-based).
 *
 * \param ks Points to the key schedule to initialize.
 * \param key Points to the 16 bytes of the key data.
 */
void gift128n_init(gift128n_key_schedule_t *ks, const unsigned char *key);

/**
 * \brief Encrypts a 128-bit block with GIFT-128 (nibble-based).
 *
 * \param ks Points to the GIFT-128 key schedule.
 * \param output Output buffer which must be at least 16 bytes in length.
 * \param input Input buffer which must be at least 16 bytes in length.
 *
 * The \a input and \a output buffers can be the same buffer for
 * in-place encryption.
 */
void gift128n_encrypt
    (const gift128n_key_schedule_t *ks, unsigned char *output,
     const unsigned char *input);

/**
 * \brief Decrypts a 128-bit block with GIFT-128 (nibble-based).
 *
 * \param ks Points to the GIFT-128 key schedule.
 * \param output Output buffer which must be at least 16 bytes in length.
 * \param input Input buffer which must be at least 16 bytes in length.
 *
 * The \a input and \a output buffers can be the same buffer for
 * in-place decryption.
 */
void gift128n_decrypt
    (const gift128n_key_schedule_t *ks, unsigned char *output,
     const unsigned char *input);

/* 4-bit tweak values expanded to 32-bit for TweGIFT-128 */
#define GIFT128T_TWEAK_0    0x00000000      /**< TweGIFT-128 tweak value 0 */
#define GIFT128T_TWEAK_1    0xe1e1e1e1      /**< TweGIFT-128 tweak value 1 */
#define GIFT128T_TWEAK_2    0xd2d2d2d2      /**< TweGIFT-128 tweak value 2 */
#define GIFT128T_TWEAK_3    0x33333333      /**< TweGIFT-128 tweak value 3 */
#define GIFT128T_TWEAK_4    0xb4b4b4b4      /**< TweGIFT-128 tweak value 4 */
#define GIFT128T_TWEAK_5    0x55555555      /**< TweGIFT-128 tweak value 5 */
#define GIFT128T_TWEAK_6    0x66666666      /**< TweGIFT-128 tweak value 6 */
#define GIFT128T_TWEAK_7    0x87878787      /**< TweGIFT-128 tweak value 7 */
#define GIFT128T_TWEAK_8    0x78787878      /**< TweGIFT-128 tweak value 8 */
#define GIFT128T_TWEAK_9    0x99999999      /**< TweGIFT-128 tweak value 9 */
#define GIFT128T_TWEAK_10   0xaaaaaaaa      /**< TweGIFT-128 tweak value 10 */
#define GIFT128T_TWEAK_11   0x4b4b4b4b      /**< TweGIFT-128 tweak value 11 */
#define GIFT128T_TWEAK_12   0xcccccccc      /**< TweGIFT-128 tweak value 12 */
#define GIFT128T_TWEAK_13   0x2d2d2d2d      /**< TweGIFT-128 tweak value 13 */
#define GIFT128T_TWEAK_14   0x1e1e1e1e      /**< TweGIFT-128 tweak value 14 */
#define GIFT128T_TWEAK_15   0xffffffff      /**< TweGIFT-128 tweak value 15 */

/**
 * \brief Encrypts a 128-bit block with TweGIFT-128 (tweakable variant).
 *
 * \param ks Points to the GIFT-128 key schedule.
 * \param output Output buffer which must be at least 16 bytes in length.
 * \param input Input buffer which must be at least 16 bytes in length.
 * \param tweak 4-bit tweak value expanded to 32-bit.
 *
 * The \a input and \a output buffers can be the same buffer for
 * in-place encryption.
 *
 * This variant of GIFT-128 is used by the ESTATE submission to the
 * NIST Lightweight Cryptography Competition.  A 4-bit tweak is added to
 * some of the rounds to provide domain separation.  If the tweak is
 * zero, then this function is identical to gift128n_encrypt().
 */
void gift128t_encrypt
    (const gift128n_key_schedule_t *ks, unsigned char *output,
     const unsigned char *input, uint32_t tweak);

/**
 * \brief Decrypts a 128-bit block with TweGIFT-128 (tweakable variant).
 *
 * \param ks Points to the GIFT-128 key schedule.
 * \param output Output buffer which must be at least 16 bytes in length.
 * \param input Input buffer which must be at least 16 bytes in length.
 * \param tweak 4-bit tweak value expanded to 32-bit.
 *
 * The \a input and \a output buffers can be the same buffer for
 * in-place encryption.
 *
 * This variant of GIFT-128 is used by the ESTATE submission to the
 * NIST Lightweight Cryptography Competition.  A 4-bit tweak is added to
 * some of the rounds to provide domain separation.  If the tweak is
 * zero, then this function is identical to gift128n_encrypt().
 */
void gift128t_decrypt
    (const gift128n_key_schedule_t *ks, unsigned char *output,
     const unsigned char *input, uint32_t tweak);

#ifdef __cplusplus
}
#endif

#endif
