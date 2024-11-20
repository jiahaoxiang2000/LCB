# LCB

This repository hosts the benchmarking framework used to evaluate the software performance of Lightweight block cipher Project on microcontrollers.

 - `src`: benchmarking framework source code and the build scripts used to perform the experiments
 - `implementations`: implementations of the cipher
 - `benchmarks`: benchmark results


## Implementations

More information on implementations included in this effort can be found [here](implementations/). Please note that this repository only includes implementations that have successfully passed known answer tests. 

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

## CHNAGELOG

- [x] remove the KAT function in the benchmarking framework
- [x] let warp implement to different floder
- [x] add the more platform to the benchmarking framework
- [x] refloct the build.sh by the python script
- [] the print use the log framework
- [x] add the data collection script