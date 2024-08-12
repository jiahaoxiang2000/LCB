//
// NIST-developed software is provided by NIST as a public service.
// You may use, copy and distribute copies of the software in any medium,
// provided that you keep intact this entire notice. You may improve, 
// modify and create derivative works of the software or any portion of
// the software, and you may copy and distribute such modifications or
// works. Modified works should carry a notice stating that you changed
// the software and should note the date and nature of any such change.
// Please explicitly acknowledge the National Institute of Standards and 
// Technology as the source of the software.
//
// NIST-developed software is expressly provided "AS IS." NIST MAKES NO 
// WARRANTY OF ANY KIND, EXPRESS, IMPLIED, IN FACT OR ARISING BY OPERATION
// OF LAW, INCLUDING, WITHOUT LIMITATION, THE IMPLIED WARRANTY OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE, NON-INFRINGEMENT AND DATA ACCURACY. NIST
// NEITHER REPRESENTS NOR WARRANTS THAT THE OPERATION OF THE SOFTWARE WILL BE 
// UNINTERRUPTED OR ERROR-FREE, OR THAT ANY DEFECTS WILL BE CORRECTED. NIST 
// DOES NOT WARRANT OR MAKE ANY REPRESENTATIONS REGARDING THE USE OF THE SOFTWARE
// OR THE RESULTS THEREOF, INCLUDING BUT NOT LIMITED TO THE CORRECTNESS, ACCURACY,
// RELIABILITY, OR USEFULNESS OF THE SOFTWARE.
//
// You are solely responsible for determining the appropriateness of using and 
// distributing the software and you assume all risks associated with its use, 
// including but not limited to the risks and costs of program errors, compliance 
// with applicable laws, damage to or loss of data, programs or equipment, and 
// the unavailability or interruption of operation. This software is not intended
// to be used in any situation where a failure could cause risk of injury or 
// damage to property. The software developed by NIST employees is not subject to
// copyright protection within the United States.
//

#pragma once

#include "lwc_mode.h"

// Ensure that an operating mode is defined
#if !defined(LWC_MODE_GENKAT_AEAD) && !defined(LWC_MODE_GENKAT_HASH) && !defined(LWC_MODE_GENKAT_COMBINED) && \
    !defined(LWC_MODE_TIMING_AEAD) && !defined(LWC_MODE_TIMING_HASH) && \
    !defined(LWC_MODE_USE_AEAD_ENCRYPT) && !defined(LWC_MODE_USE_AEAD_DECRYPT) && !defined(LWC_MODE_USE_AEAD_BOTH) && !defined(LWC_MODE_USE_HASH) && !defined(LWC_MODE_USE_COMBINED_AEAD_ENCRYPT) && !defined(LWC_MODE_USE_COMBINED_AEAD_DECRYPT) && !defined(LWC_MODE_USE_COMBINED_AEAD_BOTH)

#error No mode is defined in lwc_mode.h

#endif

// Define experiment type based on the mode
#if defined(LWC_MODE_GENKAT_AEAD) || defined(LWC_MODE_GENKAT_HASH) || defined(LWC_MODE_GENKAT_COMBINED)
    #define LWC_EXPERIMENT_GENKAT
#elif defined(LWC_MODE_TIMING_AEAD) || defined(LWC_MODE_TIMING_HASH)
    #define LBC_EXPERIMENT_TIMING
#elif defined(LWC_MODE_USE_AEAD_ENCRYPT) || defined(LWC_MODE_USE_AEAD_DECRYPT) || defined(LWC_MODE_USE_AEAD_BOTH) || defined(LWC_MODE_USE_HASH) || defined(LWC_MODE_USE_COMBINED_AEAD_ENCRYPT) || defined(LWC_MODE_USE_COMBINED_AEAD_DECRYPT) || defined(LWC_MODE_USE_COMBINED_AEAD_BOTH)
    #define LWC_EXPERIMENT_SIZE
#endif


#include "lwc_constraints.h"

// If the implementation does not have message length constraints, set the default value to 1
#ifndef LWC_MLEN_STEP
#define LWC_MLEN_STEP 1
#endif

// If the implementation does not have associated data length constraints, set the default value to 1
#ifndef LWC_ALEN_STEP
#define LWC_ALEN_STEP 1
#endif


#include "lwc_crypto_aead.h"
#include "lwc_crypto_hash.h"
#include "experiments.h"
#include "timers.h"
#include "utils.h"
