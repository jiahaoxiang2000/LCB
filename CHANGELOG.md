## [0.2.0]

### Added

- Implemented multiple cipher algorithms:
  - AES-128 with CTR mode support
  - TinyJAMBU-128 cipher implementation
  - ASCON128 cipher implementation
  - GIFT-128 cipher implementation
  - WARP128 cipher with bitslicing and ARM assembly support (upgraded from WARP64)
  - LLBC128 cipher implementation
  - LLLWBC cipher implementation
  - Simon cipher implementation
  - QARMAV2-64 implementation (standard and bitslicing versions)
- Enhanced platform support:
  - Added UART support for STM32L475VG with custom TX/RX pin definitions
  - Updated ESP32S3 benchmark data and platform integration
- Documentation improvements:
  - Updated benchmarking platforms and methodology details
  - Updated benchmark results for various cipher implementations
  - Improved README formatting and documentation

### Fixed

- Fixed indentation for experiment execution in setup function

## [0.1.0]

### Added

- Multi-platform support
- ESP32S3 platform benchmark capabilities
- Platform compatibility checks

## [0.0.1]

### Added

- support the `l475vg` `Panduola` board cipher benchmark for code size and timing.
