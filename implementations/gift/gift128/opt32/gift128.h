/*
GIFT-128 implementation
Written by: Siang Meng Sim
Email: crypto.s.m.sim@gmail.com
Date: 08 Feb 2019
*/

#ifndef GIFT128_H
#define GIFT128_H

#include <stdint.h>

void giftb128(uint8_t P[16], const uint8_t K[16], uint8_t C[16]);

#endif /* GIFT128_H */
