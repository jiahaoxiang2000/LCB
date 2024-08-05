Those repositories is perfect way to learn software implementations the lightweight cryptography:

 - https://github.com/ascon/ascon-c
 - https://github.com/aadomn/gift
 - https://github.com/TimBeyne/Elephant
 - https://github.com/romulusae/romulus
 - https://lab.las3.de/gitlab/lwc/candidates
 - https://github.com/rweather/lwc-finalists
 - https://github.com/nerilex/arm-crypto-lib	
 - https://github.com/cryptolu/FELICS
 - https://github.com/ARMmbed/mbedtls
 - https://nacl.cr.yp.to/

## Directory structure

The directory structure of the implementations is as follows:

```
- [cipher_name]
	- [cipher_variant1]
		- [impl1]
		- [impl2]
	- [cipher_variant2]
		- [impl1]
		- [impl2]
```


## Changes made to the implementations and the directories

In each implementation folder, a C file that contains a wrapper for the implementation is added to make it compatible with the *benchmarking framework*:

### cipher implementations

A file named `cipher.c` has been aded. This file defines the `cipher_ctx` structure declared in `cipher.h`:

``` c
typedef struct {

	const char* variant_name;
	const char* impl_name;
	int KeyBytes;
	int NonceBytes;
	int ABytes;
	fn_aead_encrypt encrypt;
	fn_aead_decrypt decrypt;

} cipher_ctx;
```

An AEAD implementation must have a `cipher.c` file that defines a `cipher_ctx` with the name `lcb_cipher`. Here's an example:

``` c
#include "lwc_crypto_aead.h"
#include "api.h"

cipher_ctx lcb_cipher = {
	"aes-gcm",
	"mbedtls",
	CRYPTO_KEYBYTES,
	CRYPTO_NPUBBYTES,
	CRYPTO_ABYTES,
	crypto_aead_encrypt,
	crypto_aead_decrypt
};
```



**Note:** The `cipher.c` make use of the `api.h` file in order to define algorithm parameters such as Key Length, etc. If an implementation did not have this file, it was copied from the *reference implementation* of the variant.


## Other changes

The following files are used as markers for the build script:

 - An empty file named `primary` is added under the primary variant folders.
 - For Assembly implementations, an empty file `lcb_[archname]` is added in order to avoid the compilation of the implementation on incompatible architectures.


For implementations that require an input sizes to be a multiple of *k > 1* bytes, a file named `lcb_constraints.h` is added. If this file does not exist in the implementation directory then it is assumed that the implementation handles inputs of all sizes. As an example, if an AEAD implementation requires the plaintext length to be a multiple of 4 bytes, then the `lcb_constraints.h` file must have the following content:
 ``` c
 #define LCB_MLEN_STEP 4
 ```
The `LCB_MLEN_STEP` definition can be used to specify input size constraints for both the *plaintext length*. Similarly, `LCB_MLEN_STEP` can be used to specify input constraints for the associated data length.

