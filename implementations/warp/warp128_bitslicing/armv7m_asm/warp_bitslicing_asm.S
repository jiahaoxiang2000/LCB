.syntax unified
.thumb

.global warp_rs
.section .data
.align 4
.type warp_rs, %object
warp_rs:    .word 0xffff00, 0xf0ff0f0, 0x3333cccc, 0x5555aaaa @ test vector
@ warp_rs:    .word 0xff00ff, 0xf0f0f0f, 0x33333333, 0x55555555 @ test for compose
@ rs: .word 0xffff, 0xff00ff, 0xf0f0f0f, 0x33333333
@ rs: .word 0xfffff0cf, 0xfc3ff33f, 0xcccc3ff, 0x3fcc030c
ks:         .word 0x5555, 0x10550055, 0x5050505, 0x11111111, 0x45550000, 0x45005500, 0x50505050, 0x44444444, 0x10005555, 0x10550055, 0x5050505, 0x51111111, 0x45550000, 0x45005500, 0x10505050, 0x4444444, 0x10005555, 0x50550055, 0x45050505, 0x51111111, 0x5550000, 0x5005500, 0x10505050, 0x4444444, 0x50005555, 0x40550055, 0x45050505, 0x51111111, 0x15550000, 0x5005500, 0x10505050, 0x4444444, 0x50005555, 0x40550055, 0x45050505, 0x11111111, 0x15550000, 0x5005500, 0x50505050, 0x4444444, 0x50005555, 0x550055, 0x45050505, 0x11111111, 0x55550000, 0x5005500, 0x50505050, 0x4444444, 0x50005555, 0x10550055, 0x45050505, 0x11111111, 0x45550000, 0x15005500, 0x50505050, 0x4444444, 0x40005555, 0x550055, 0x45050505, 0x51111111, 0x55550000, 0x5005500, 0x10505050, 0x44444444, 0x50005555, 0x50550055, 0x5050505, 0x11111111, 0x5550000, 0x55005500, 0x50505050, 0x4444444, 0x5555, 0x10550055, 0x45050505, 0x51111111, 0x45550000, 0x5005500, 0x10505050, 0x44444444, 0x50005555, 0x50550055, 0x5050505, 0x51111111, 0x5550000, 0x55005500, 0x10505050, 0x4444444, 0x5555, 0x50550055, 0x45050505, 0x51111111, 0x5550000, 0x5005500, 0x10505050, 0x44444444, 0x50005555, 0x40550055, 0x5050505, 0x51111111, 0x15550000, 0x45005500, 0x10505050, 0x4444444, 0x10005555, 0x40550055, 0x45050505, 0x11111111, 0x15550000, 0x15005500, 0x50505050, 0x4444444, 0x40005555, 0x10550055, 0x45050505, 0x11111111, 0x45550000, 0x15005500, 0x50505050, 0x44444444, 0x40005555, 0x550055, 0x5050505, 0x51111111, 0x55550000, 0x45005500, 0x10505050, 0x44444444, 0x10005555, 0x50550055, 0x5050505, 0x11111111, 0x5550000, 0x45005500, 0x50505050, 0x4444444, 0x10005555, 0x550055, 0x45050505, 0x51111111, 0x55550000, 0x15005500, 0x10505050, 0x4444444, 0x40005555, 0x40550055, 0x45050505, 0x11111111, 0x15550000, 0x5005500, 0x50505050, 0x44444444, 0x50005555, 0x550055, 0x5050505, 0x11111111, 0x55550000, 0x45005500, 0x50505050, 0x4444444,0x10005555,0x10550055,0x45050505,0x11111111
ss:         .space 20*4

.global warp_bitslicing
.type warp_bitslicing, %function

@ the r11 is the pointer to the rs, r12 is the pointer to the ss, r0-3 are the output registers
.macro SBOX r11, r12, r0, r1, r2, r3, r4, r5
    LDR \r0, [\r11]
    LDR \r1, [\r11, #4]
    LDR \r2, [\r11, #8]
    LDR \r3, [\r11, #12]
    @ T[0] = R0 & R1
    AND \r4, \r0, \r1
    STR \r4, [\r12]
    @ T[1] = R0 & R2
    AND \r4, \r0, \r2
    STR \r4, [\r12, #4]
    @ T[2] = R2 | T[0]
    LDR \r4, [\r12]
    ORR \r4, \r2, \r4
    STR \r4, [\r12, #8]
    @ T[3] = R0 | R3
    ORR \r4, \r0, \r3
    STR \r4, [\r12, #12]
    @ T[4] = R3 | R1
    ORR \r4, \r3, \r1
    STR \r4, [\r12, #16]
    @ T[5] = T[2] & T[3]
    LDR \r4, [\r12, #8]
    LDR \r5, [\r12, #12]
    AND \r4, \r4, \r5
    STR \r4, [\r12, #20]
    @ T[6] = T[2] ^ T[0]
    LDR \r4, [\r12, #8]
    LDR \r5, [\r12]
    EOR \r4, \r4, \r5
    STR \r4, [\r12, #24]
    @ T[7] = T[4] & R0
    LDR \r4, [\r12, #16]
    AND \r4, \r4, \r0
    STR \r4, [\r12, #28]
    @ T[8] = R1 | T[7]
    LDR \r4, [\r12, #28]
    ORR \r4, \r1, \r4
    STR \r4, [\r12, #32]
    @ T[9] = T[7] ^ R1
    LDR \r4, [\r12, #28]
    EOR \r4, \r4, \r1
    STR \r4, [\r12, #36]
    @ T[10] = ~T[0]
    LDR \r4, [\r12]
    MVN \r4, \r4
    STR \r4, [\r12, #40]
    @ T[11] = T[4] ^ T[0]
    LDR \r4, [\r12, #16]
    LDR \r5, [\r12]
    EOR \r4, \r4, \r5
    STR \r4, [\r12, #44]
    @ T[12] = T[7] & R3
    LDR \r4, [\r12, #28]
    AND \r4, \r4, \r3
    STR \r4, [\r12, #48]
    @ T[13] = T[9] & T[3]
    LDR \r4, [\r12, #36]
    LDR \r5, [\r12, #12]
    AND \r4, \r4, \r5
    STR \r4, [\r12, #52]
    @ T[14] = T[6] | T[13]
    LDR \r4, [\r12, #24]
    LDR \r5, [\r12, #52]
    ORR \r4, \r4, \r5
    STR \r4, [\r12, #56]
    @ T[15] = T[8] & R2
    LDR \r4, [\r12, #32]
    AND \r4, \r4, \r2
    STR \r4, [\r12, #60]
    @ T[16] = ~T[5]
    LDR \r4, [\r12, #20]
    MVN \r4, \r4
    STR \r4, [\r12, #64]
    @ T[17] = ~T[3]
    LDR \r4, [\r12, #12]
    MVN \r4, \r4
    STR \r4, [\r12, #68]
    @ T[18] = T[11] ^ T[12]
    LDR \r4, [\r12, #44]
    LDR \r5, [\r12, #48]
    EOR \r4, \r4, \r5
    STR \r4, [\r12, #72]
    @ T[19] = T[15] | T[17]
    LDR \r4, [\r12, #60]
    LDR \r5, [\r12, #68]
    ORR \r4, \r4, \r5
    STR \r4, [\r12, #76]
    @ r0 = T[16]
    LDR \r0, [\r12, #64]
    @ r1 = T[19]
    LDR \r1, [\r12, #76]
    @ r2 = T[18]
    LDR \r2, [\r12, #72]
    @ r3 = T[14]
    LDR \r3, [\r12, #56]
.endm

.macro PPO, dest_r, src_r, t1, t2, rotate_amount, mask_value
    MOV \t1, \src_r, ROR#\rotate_amount
    LDR \t2, =\mask_value
    AND \t1, \t1, \t2
    ORR \dest_r, \dest_r, \t1
.endm

.macro PPO_PI dest_r, src_r, t1, t2
    MOV \dest_r, #0x0
    PPO \dest_r, \src_r, \t1, \t2, 1, 0x800080 @ (1, [8, 24], '0x800080')
    PPO \dest_r, \src_r, \t1, \t2, 5, 0x2000200 @ (5, [6, 22], '0x2000200')
    PPO \dest_r, \src_r, \t1, \t2, 7, 0x80008 @ (7, [12, 28], '0x80008')
    PPO \dest_r, \src_r, \t1, \t2, 9, 0x1000100 @ (9, [7, 23], '0x1000100')
    PPO \dest_r, \src_r, \t1, \t2, 11, 0x20002  @ (11, [14, 30], '0x20002')
    PPO \dest_r, \src_r, \t1, \t2, 13, 0x400040 @ (13, [9, 25], '0x400040')
    PPO \dest_r, \src_r, \t1, \t2, 15, 0x4000400 @ (15, [5, 21], '0x4000400')
    PPO \dest_r, \src_r, \t1, \t2, 19, 0x100010 @ (19, [11, 27], '0x100010')
    PPO \dest_r, \src_r, \t1, \t2, 21, 0x80008000 @ (21, [0, 16], '0x80008000')
    PPO \dest_r, \src_r, \t1, \t2, 23, 0x8000800 @ (23, [4, 20], '0x8000800')
    PPO \dest_r, \src_r, \t1, \t2, 25, 0x30003000 @ (25, [2, 3, 18, 19], '0x30003000')
    PPO \dest_r, \src_r, \t1, \t2, 27, 0x240024 @ (27, [10, 13, 26, 29], '0x240024')
    PPO \dest_r, \src_r, \t1, \t2, 29, 0x40004000 @ (29, [1, 17], '0x40004000')
    PPO \dest_r, \src_r, \t1, \t2, 31, 0x10001 @ (31, [15, 31], '0x10001')
.endm

.macro PERMUTATION base_r, t1, t2, t3, t4, PPO_F
    LDR \t2, [\base_r]// Load the first item of rs into r1
    \PPO_F \t1, \t2, \t3, \t4
    STR \t1, [\base_r]// Store the value of r1 back into the first item of rs

    LDR \t2, [\base_r, #4]// Load the second item of rs into r1
    \PPO_F \t1, \t2, \t3, \t4
    STR \t1, [\base_r, #4]// Store the value of r1 back into the second item of rs

    LDR \t2, [\base_r, #8]// Load the third item of rs into r1
    \PPO_F \t1, \t2, \t3, \t4
    STR \t1, [\base_r, #8]// Store the value of r1 back into the third item of rs

    LDR \t2, [\base_r, #12]// Load the fourth item of rs into r1
    \PPO_F \t1, \t2, \t3, \t4
    STR \t1, [\base_r, #12]// Store the value of r1 back into the fourth item of rs
.endm

.macro F_ROUND base, ss, src0, src1, src2, src3, t0, t1
    SBOX \base, \ss, \src0, \src1, \src2, \src3,\t0, \t1
    MOV \src0, \src0, ROR#1
    AND \src0, \src0, #0x55555555
    LDR \t0, [\base]
    EOR \src0, \src0, \t0
    STR \src0, [\base]
    MOV \src1, \src1, ROR#1
    AND \src1, \src1, #0x55555555
    LDR \t0, [\base, #4]
    EOR \src1, \src1, \t0
    STR \src1, [\base, #4]
    MOV \src2, \src2, ROR#1
    AND \src2, \src2, #0x55555555
    LDR \t0, [\base, #8]
    EOR \src2, \src2, \t0
    STR \src2, [\base, #8]
    MOV \src3, \src3, ROR#1
    AND \src3, \src3, #0x55555555
    LDR \t0, [\base, #12]
    EOR \src3, \src3, \t0
    STR \src3, [\base, #12]
.endm

.macro XOR_BITSLICING r12, r11, t0, t1
    LDR \t0, [\r12]
    LDR \t1, [\r11]
    EOR \t0, \t0, \t1
    STR \t0, [\r12]
    LDR \t0, [\r12, #4]
    LDR \t1, [\r11, #4]
    EOR \t0, \t0, \t1
    STR \t0, [\r12, #4]
    LDR \t0, [\r12, #8]
    LDR \t1, [\r11, #8]
    EOR \t0, \t0, \t1
    STR \t0, [\r12, #8]
    LDR \t0, [\r12, #12]
    LDR \t1, [\r11, #12]
    EOR \t0, \t0, \t1
    STR \t0, [\r12, #12]
.endm

.macro ROUND r12, r11, r10, r0, r1, r2, r3, r4, r5
    F_ROUND \r12, \r10, \r0, \r1, \r2, \r3, \r4, \r5
    XOR_BITSLICING \r12, \r11, \r0, \r1
    ADD \r11, \r11, #16
    PERMUTATION \r12, \r0, \r1, \r2, \r3, PPO_PI
.endm

.text
warp_bitslicing:
    PUSH {r0-r12, r14}
    MOVW r12, #:lower16:warp_rs
    MOVT r12, #:upper16:warp_rs

    MOVW r11, #:lower16:ks
    MOVT r11, #:upper16:ks

    MOVW r10, #:lower16:ss
    MOVT r10, #:upper16:ss

    MOV r6, #40 // Initialize counter with 40
loop_start:
    ROUND r12, r11, r10, r0, r1, r2, r3, r4, r5 // Execute ROUND
    SUBS r6, r6, #1     // Decrement counter
    BNE loop_start      // If counter is not zero, branch back to loop_start
    
    @ last round not permutation
    F_ROUND r12, r10, r0, r1, r2, r3, r4, r5
    XOR_BITSLICING r12, r11, r0, r1
    
    POP {r0-r12, r14}
    BX lr
