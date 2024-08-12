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

#include "lwc_benchmark.h"

#if defined(LBC_EXPERIMENT_TIMING)

const uint32_t ShortInputStep = 8;

#if defined(LWC_PLATFORM_UNO) || defined(LWC_PLATFORM_NANOEVERY) || defined(LWC_PLATFORM_DUEUSB)
using timer = timer_micros;
const uint32_t Trials = 5;
const uint32_t MaxLongInputLength = 512;
const uint32_t MaxShortInputLength = 128;
const uint32_t LongInputStep = 128;
#elif defined(LWC_PLATFORM_NODEMCUV2)
using timer = timer_cycles;
const uint32_t Trials = 3;
const uint32_t MaxLongInputLength = 512;
const uint32_t MaxShortInputLength = 128;
const uint32_t LongInputStep = 128;
#elif defined(LWC_PLATFORM_PIC32MX3)
using timer = timer_micros;
const uint32_t Trials = 5;
const uint32_t MaxLongInputLength = 512;
const uint32_t MaxShortInputLength = 128;
const uint32_t LongInputStep = 128;
#else
using timer = timer_cycles;
const uint32_t Trials = 3;
const uint32_t MaxLongInputLength = 2048;
const uint32_t MaxShortInputLength = 128;
const uint32_t LongInputStep = 128;
#endif

//
// A structure for specifying a linearly increasing sequence of input sizes.
//
// begin() function initializes the value with Begin and each call to next() updates the value
// by incrementing it by an amount of Step.
//
// The user of the class must make sure that the value returned by next() is less than or equal to end().
//
template <uint32_t Begin, uint32_t End, uint32_t Step>
struct LinearRange
{
    uint32_t begin()
    {
        value = Begin;
        return value;
    }

    // This function must be evaluated at compile time because it is used in array declarations
    static constexpr uint32_t end()
    {
        return End;
    }

    uint32_t next()
    {
        value += Step;
        return value;
    }

private:
    uint32_t value;
};

//
// A structure for specifying an exponentially increasing sequence of input sizes.
//
// begin() function initializes the value with Begin and each call to next() updates the value
// by doubling the value.
//
// The user of the class must make sure that the value returned by next() is less than or equal to end().
//
template <uint32_t Begin, uint32_t End, bool IncludeZero>
struct ExponentialRange
{

    uint32_t begin()
    {
        value = IncludeZero ? 0 : Begin;
        return value;
    }

    // This function must be evaluated at compile time because it is used in array declarations
    static constexpr uint32_t end()
    {
        return End;
    }

    uint32_t next()
    {
        if (value == 0)
            value = Begin;
        else
            value <<= 1;

        return value;
    }

private:
    uint32_t value;
};

template <typename Timer, typename Cipher>
void print_benchmark_info(const Cipher &cipher)
{
    SOUT << "platform = " << get_platform_name() << SENDL;
    SOUT << "config = " << LWC_CONFIG << SENDL;
    SOUT << "build time = " << __DATE__ << " - " << __TIME__ << SENDL;
    SOUT << "variant = " << cipher.variant_name << SENDL;
    SOUT << "impl = " << cipher.impl_name << SENDL;
    SOUT << "timer = " << Timer::name() << SENDL;
    SOUT << SENDL;
}

#endif // LBC_EXPERIMENT_TIMING

#ifdef LWC_MODE_TIMING_AEAD

template <class Timer, int Trials, typename ADRange, typename MsgRange>
int benchmark_aead(const aead_ctx &cipher, ADRange adrange, MsgRange msgrange, bool verbose = false)
{
    

    const int MaxKeyBytes = 32;
    const int MaxNonceBytes = 32;
    const int MaxABytes = 32;

    buffer<msgrange.end()> msg;
    buffer<msgrange.end() + MaxABytes> ct;
    buffer<adrange.end()> ad;
    buffer<MaxKeyBytes> key;
    buffer<MaxNonceBytes> nonce;
    buffer<Trials, uint32_t> elapsedEnc, elapsedDec;
    unsigned long long clen, mlenDec;
    int ret{0}, fret;

    SOUT << "# Cipher Benchmarking" << SENDL;
    print_benchmark_info<Timer>(cipher);

    for (int i = 0; i < Trials; i++)
    {
        msg.init();
        ad.init();
        key.init();
        nonce.init();

        // Encryption
        {
            Timer tm(elapsedEnc[i]);

            fret = cipher.encrypt(ct.data(), &clen, msg.data(), 0, ad.data(), 256, nullptr, nonce.data(), key.data());
        }

        if (fret != 0)
        {
            SOUT << "error : crypto_aead_encrypt() failed with code " << fret << SENDL;
            ret = -1;
            break;
        }

        msg.clear();
        ad.init();
        key.init();
        nonce.init();

        // Decryption
        {
            Timer tm(elapsedDec[i]);

            fret = cipher.decrypt(msg.data(), &mlenDec, nullptr, ct.data(), clen, ad.data(), 256, nonce.data(), key.data());
        }

        if (fret != 0)
        {
            SOUT << "error : crypto_aead_decrypt() failed with code " << fret << SENDL;
            ret = -1;
            break;
        }

        // Verify decrypted message length
        if (mlenDec != 0)
        {
            SOUT << "error : incorrect plaintext length" << SENDL;
            fret = -1;
            ret = -1;
            break;
        }

        // Check if the plaintext is recovered
        if (!is_identity_buffer(msg.data(), 0))
        {
            SOUT << "error : decryption did not recover the plaintext" << SENDL;
            // msg.print_hex("#msg = ", mlen);
            fret = -1;
            ret = -1;
            break;
        }

    } // trials

    // print timing information only if the encryption and the decryption succeeded
    if (fret == 0)
    {

        if (verbose)
            for (auto x : elapsedEnc)
                SOUT << x << ' ';
        SOUT << "enc=" << median(elapsedEnc) << ' ';

        if (verbose)
            for (auto x : elapsedDec)
                SOUT << x << ' ';
        SOUT << "dec=" << median(elapsedDec) << ' ';

        SOUT << SENDL;
    }

    return ret;
}

int timing_experiments()
{
    int ret{0};

    {
        // Test case : Empty Message, Long AD
        auto adrange = LinearRange<MaxShortInputLength + LongInputStep, MaxLongInputLength, LongInputStep>();
        auto msgrange = LinearRange<0, 0, 1>();
        ret |= benchmark_aead<timer, Trials>(lwc_aead_cipher, adrange, msgrange, true);
    }

    return ret;
}
#endif // LWC_MODE_TIMING_AEAD

int do_timing_experiments()
{
    int ret{0};

#if defined(LBC_EXPERIMENT_TIMING) && defined(LWC_PLATFORM_l475vg)
    // Set SysTick reload counter to the maximum value for timing experiments
    // The destructor resets the counter to its default value
    systick_reload_max srm;
#endif

    ret = timing_experiments();

    return ret;
}
