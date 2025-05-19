### Benchmark Results

This page contains the benchmark results grouped with respect to the microcontroller. Each result file contains code size and timing measurements for all the implementations that have been benchmarked. Descriptions of the column names in the `csv` files are as follows:

- `variant`: The implementation variant for the cipher on different architectures
- `impl`: The implementation name for the cipher on different platforms
- `config`: The compile flag used for the implementation
- `enc`: The time taken to encrypt the message in cycles
- `flash`: The code size of the implementation in bytes

### Notes

- An implementation compiled with a certain flag does not appear in the result file if any of the followings occur:
  - Compilation failure
  - Build failure due to large program size
- If the build succeeds then there will be an entry for that implementation and compilation flag in the result file. However, for some of the implementations the timing measurements could not be obtained for some/all inputs. These entries are marked with a `-` symbol. This can be due to the following reasons:
  - Decryption function returning an error code
  - Decryption function returning an incorrect message length
  - Decryption function not recovering the message
  - Program crash
- In the case of a decryption failure, the timing measurement is not recorded but the experiment proceeds with the next input. On the contrary, when the program crashes, the measurements for the remaining inputs cannot be performed.
- The `config` (Ox compile level) does not affect the `enc` (encryption cycles)` in these results for the ESP32-S3 platform; the timing measurements remain the same across different optimization levels on this platform.

### Environment Introduction

The benchmarking experiments were conducted on 32-bit microcontroller platforms, specifically the STM32L475VET6 and ESP32-S3. The evaluation of LLBC implementations follows a methodology analogous to the FELICS benchmarking framework, focusing on the encryption of 128-bit data blocks in ECB mode. All cipher implementations are developed in assembly language and compiled using platform-specific toolchains supported by PlatformIO. The reported performance metrics comprise code size (ROM) and execution time (measured in clock cycles).
