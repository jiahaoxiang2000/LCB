## Directory structure

The directory structure of the implementations is as follows:

``` plaintext
└── [cipher_name]
    ├── [cipher_variant1]
    │   └── [impl1]
    └── [cipher_variant2]
        ├── [impl1]
        └── [impl2]
```

By this way, we can batch to benchmarking the implementations.


## Changes made to the implementations and the directories

In each implementation folder, a C file that contains a wrapper for the implementation is added to make it compatible with the *benchmarking framework*:

### cipher implementations

A file named `lcb_crypto.c` has been added. This file defines the `cipher_ctx` structure declared in `lcb_crypto.h` on `src/src` folder.

``` c
typedef struct
{
	const char *variant_name;
	const char *impl_name;
	int KeyBytes;
	fn_encrypt encrypt;
	fn_decrypt decrypt;

} cipher_ctx;
```

An implementation must have a `lcb_crypto.c` file that defines a `cipher_ctx` with the name `lcb_cipher`. Here's an example:

``` c
#include "lcb_crypto.h"
#include "api.h"

cipher_ctx lcb_cipher = {
	"warp64_bitslicing",
	"armv7m_asm",
	CRYPTO_KEYBYTES,
	crypto_encrypt,
	crypto_decrypt
};
```



**Note:** The `lcb_crypto.c` make use of the `api.h` file in order to define algorithm parameters such as Key Length, etc. If an implementation did not have this file, it was copied from the *reference implementation* of the variant.


## Other changes

The following files are used as markers for the build script:

 - An empty file named `primary` is added under the primary variant folders.
 - For Assembly implementations, an empty file `lcb_arch_[archname]` is added in order to avoid the compilation of the implementation on incompatible architectures.

## learnings

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
