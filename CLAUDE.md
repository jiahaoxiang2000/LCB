# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

LCB (Lightweight Cryptographic Benchmarks) is a benchmarking framework for evaluating lightweight block cipher implementations on microcontrollers. This is based on the NIST lightweight cryptography benchmarking framework and uses PlatformIO for cross-platform builds.

## Code Architecture

### Core Components

- **`src/`** - Main benchmarking framework
  - `arduino_main.cpp` - Arduino-compatible main entry point
  - `experiments.cpp/.h` - Experiment orchestration
  - `lcb_crypto.h` - Common crypto interface
  - `lwc_benchmark.h` - Main framework header
  - `platformio.ini` - PlatformIO configuration

### Implementation Structure

- **`implementations/`** - Cipher implementations organized as:
  ```
  cipher_name/
    ├── variant1/
    │   └── impl1/ (e.g., opt32, opt64, armv7m_asm)
    └── variant2/
        ├── impl1/
        └── impl2/
  ```

### Key Files in Each Implementation

- `lcb_crypto.c` - Implementation adapter for benchmarking framework
- `api.h` - Standard API definitions (key sizes, etc.)
- `cipher.c` - Main cipher implementation
- `lcb_arch_*` - Architecture markers for compatibility
- `primary` - Marker for primary variants

## Common Development Workflows

### Adding New Cipher Implementation

1. Create directory structure: `implementations/cipher_name/variant/impl/`
2. Add required files: `lcb_crypto.c`, `api.h`, `cipher.c`
3. Add architecture markers if needed: `lcb_arch_armv7m`, etc.
4. Mark primary variant with `primary` file
5. To ensure it builds correctly

```bash
cd src/
python build.py -t l475vg -s cipher_name -v variant -e "size" -w
```

### Platform-Specific Development

- **STM32L475**: Use `armv7m` assembly optimizations
- **ESP32-S3**: Use `esp32s3` specific optimizations
- Check `platformio.ini` for build flags and configurations

## Key Constraints

- Implementations must provide `lcb_crypto.c` with `lcb_cipher` context
- Architecture compatibility checked via `lcb_arch_*` files
