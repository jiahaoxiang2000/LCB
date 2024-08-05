# LCB

This repository hosts the benchmarking framework used to evaluate the software performance of Lightweight block cipher Project on microcontrollers.

 - `src`: benchmarking framework source code and the build scripts used to perform the experiments
 - `implementations`: implementations of the cipher
 - `benchmarks`: benchmark results


# Implementations

More information on implementations included in this effort can be found [here](implementations/). Please note that this repository only includes implementations that have successfully passed known answer tests. 

# Refer benchmarking framework

- [NIST LWC - 2022](https://github.com/usnistgov/Lightweight-Cryptography-Benchmarking)
  - more modern by use the platformio, little configure for more platforms.
- [Fair Evaluation of Lightweight Cryptographic Systems (FELICS) - 2019](https://github.com/cryptolu/FELICS)
  - use the shell script to implement the benchmarking framework, hard to debug
- [lwc-finalists - 2022](https://github.com/rweather/lwc-finalists)
  - use the makefile to do, flexible, but need some pre-configuration

