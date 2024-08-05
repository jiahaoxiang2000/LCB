### Benchmark Results

This page contains the benchmark results grouped with respect to the microcontroller. Each result file contains code size and timing measurements for all the implementations that have been benchmarked. Descriptions of the column names in the `csv` files are as follows:

 - TODO some metrics

### Notes

 - An implementation compiled with a certain flag does not appear in the result file if any of the followings occur:
    * Compilation failure
    * Build failure due to large program size
 - If the build succeeds then there will be an entry for that implementation and compilation flag in the result file. However, for some of the implementations the timing measurements could not be obtained for some/all inputs. These entries are marked with a `-` symbol. This can be due to the following reasons:
    * Decryption function returning an error code
    * Decryption function returning an incorrect message length
    * Decryption function not recovering the message
    * Program crash
 - In the case of a decryption failure, the timing measurement is not recorded but the experiment proceeds with the next input. On the contrary, when the program crashes, the measurements for the remaining inputs cannot be performed.
