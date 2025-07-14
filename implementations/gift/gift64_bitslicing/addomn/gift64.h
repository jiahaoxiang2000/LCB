#ifndef GIFT64_H_
#define GIFT64_H_

#include <stdint.h>

#define GIFT64_KEY_SIZE    16
#define GIFT64_BLOCK_SIZE  8
#define GIFT64_KEY_SCHEDULE_WORDS  4

typedef uint8_t u8;
typedef uint32_t u32;

// Function prototypes
extern void gift64_keyschedule(const u8* key, u32* rkey);
extern void gift64_encrypt_block(u8* out_block, const u32* rkey, const u8* in_block);
extern void gift64_decrypt_block(u8* out_block, const u32* rkey, const u8* in_block);

#endif  // GIFT64_H_