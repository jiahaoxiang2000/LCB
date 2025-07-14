# LCB

This repository hosts the benchmarking framework used to evaluate the software performance of Lightweight block cipher Project on microcontrollers.

- `src`: benchmarking framework source code and the build scripts used to perform the experiments
- `implementations`: implementations of the cipher
- `benchmarks`: benchmark results

## Implementations

More information on implementations included in this effort can be found [here](implementations/). Please note that this repository only includes implementations that have successfully passed known answer tests.

The benchmarking framework currently supports the following lightweight block ciphers:

| Cipher        | Block Size | Key Size | Variants                | Implementations       | Architecture Support |
| ------------- | ---------- | -------- | ----------------------- | --------------------- | -------------------- |
| **AES**       | 128-bit    | 128-bit  | `aes128`                | `opt32`, `opt32-f`    | Generic              |
| **AES**       | 128-bit    | 128-bit  | `aes128_bitslicing`     | `Ko-`, `armv7m_asm`   | ARMv7-M              |
| **ASCON**     | 64-bit     | 128-bit  | `ascon128`              | `opt32`               | Generic              |
| **GIFT**      | 128-bit    | 128-bit  | `gift128`               | `opt32`               | Generic              |
| **GIFT**      | 64-bit     | 128-bit  | `gift64_bitslicing`     | `aadomn`              | ARMv7-M              |
| **LLBC**      | 128-bit    | 128-bit  | `LLBC128`               | `opt32`               | Generic              |
| **LPLBC**     | 64-bit     | 128-bit  | `LPLBC64`               | `opt32`               | Generic              |
| **LLLWBC**    | 64-bit     | 128-bit  | `lllwbc64`              | `opt32`               | Generic              |
| **QARMAv2**   | 64-bit     | 128-bit  | `qarmav2_64`            | `opt32`               | Generic              |
| **QARMAv2**   | 64-bit     | 128-bit  | `qarmav2_64_bitslicing` | `armv7m`              | ARMv7-M              |
| **Simon**     | 64-bit     | 128-bit  | `simon64`               | `opt32`               | Generic              |
| **TinyJAMBU** | 64-bit     | 128-bit  | `tinyjambu128`          | `opt32`               | Generic              |
| **WARP**      | 128-bit    | 128-bit  | `warp128`               | `opt32`               | Generic              |
| **WARP**      | 128-bit    | 128-bit  | `warp128_bitslicing`    | `opt32`, `armv7m_asm` | Generic, ARMv7-M     |

**Implementation Types:**

- `opt32`: Optimized for 32-bit platforms
- `opt32-f`: Optimized for 32-bit platforms with specific features
- `armv7m_asm`: Hand-optimized ARMv7-M assembly implementation
- `aadomn`: Alexandre Adomnicai's bitsliced implementation
- `Ko-`: Ko's bitsliced implementation

## Supported Platforms

- [x] [Panduola board](https://www.alientek.com/Product_Details/77.html) for STM32L475vet6, is compatible with the STM32L4 series of microcontrollers.
- [x] [ESP32S3](https://www.espressif.com/en/products/socs/esp32s3) for ESP32-S3, is compatible with the ESP32 series of microcontrollers.

## Refer benchmarking framework

- [NIST LWC - 2022](https://github.com/usnistgov/Lightweight-Cryptography-Benchmarking)
  - more modern by use the platformio, little configure for more platforms.
- [Fair Evaluation of Lightweight Cryptographic Systems (FELICS) - 2019](https://github.com/cryptolu/FELICS)
  - use the shell script to implement the benchmarking framework, hard to debug
- [lwc-finalists - 2022](https://github.com/rweather/lwc-finalists)
  - use the makefile to do, flexible, but need some pre-configuration
