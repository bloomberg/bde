// bsls_atomicoperations_powerpc32_aix_xlc.cpp                        -*-C++-*-

//#include <bsls_atomicoperations_powerpc32_aix_xlc.h>
// This component deliberately does not include its header, as it relies on a
// quirk of the PowerPC architecture making header and cpp incompatible at
// compile-time, but perfect partners at link-time.  The issue is that the
// "extern "C" functions required by the atomics protocols for 64-bit integer
// values must be interpreted by the function implementation as being passed
// two 32bit integer values.  Following the "C" protocols, having bound the
// name and calling the function with the right convention as found by the
// header file, the linker will pick up the function with a slightly
// different signature from the .cpp file, and the inline ASM will decode
// the passed arguments correctly.

#include <bsls_ident.h>
BSLS_IDENT("$Id$ $CSID$")

#include <bsls_platform.h>
#include <bsls_types.h>

#if defined(BSLS_PLATFORM_CPU_POWERPC) && defined(BSLS_PLATFORM_CMP_IBM) \
    && defined(BSLS_PLATFORM_CPU_32_BIT)

namespace BloombergLP {

namespace {

struct Int64_Words
    // Provide access to the individual high and low words of the 'Int64' value
    // to allow inline assembly mapping of PowerPC registers directly to these
    // words in 32 bit mode.  Note that the layout of 'Int64_Words' assumes a
    // big-endian architecture.
{
    int hi;
    int lo;

    const bsls::Types::Int64& value() const
    {
        return reinterpret_cast<const bsls::Types::Int64&>(*this);
    }
};

// static assert, can't use bslmf here
typedef char Int64Size_StaticAssert[
                            sizeof(Int64_Words) == sizeof(bsls::Types::Int64)
                            ? 1 : -1];

}

namespace bsls {

Types::Int64 AtomicOperations_Powerpc32_GetInt64(
                                  const volatile Types::Int64 *atomicInt)
{
    Int64_Words result;

    asm volatile (
        "       sync                    \n\t"

        "1:     ld %[lo], %[obj]        \n\t"   // load double word (atomic)
        "       cmpd %[lo], %[lo]       \n\t"   // create data dependency
                                                // for load/load ordering
        "       bne- 1b                 \n\t"   // never taken
        "       srdi %[hi], %[lo], 32   \n\t"   // move high 32-bit of %[lo]
                                                // into %[hi] low 32-bit
                                                // NOTE: we're leaving the high
                                                // 32-bits of %[lo] still set,
                                                // but the caller will not see
                                                // them
        "       isync                   \n\t"
                : [lo]  "=b" (result.lo),
                  [hi]  "=b" (result.hi)
                : [obj] "m"  (*atomicInt)
                : "cr0", "ctr");

    return result.value();
}

Types::Int64 AtomicOperations_Powerpc32_GetInt64Relaxed(
                                  const volatile Types::Int64 *atomicInt)
{
    Int64_Words result;

    asm volatile (
        "       ld %[lo], %[obj]        \n\t"   // load double word (atomic)
        "       srdi %[hi], %[lo], 32   \n\t"   // move high 32-bit of %[lo]
                                                // into %[hi] low 32-bit
                                                // NOTE: we're leaving the high
                                                // 32-bits of %[lo] still set,
                                                // but the caller will not see
                                                // them
                : [lo]  "=b" (result.lo),
                  [hi]  "=b" (result.hi)
                : [obj] "m"  (*atomicInt));

    return result.value();
}

Types::Int64 AtomicOperations_Powerpc32_GetInt64Acquire(
                                  const volatile Types::Int64 *atomicInt)
{
    Int64_Words result;

    asm volatile (
        "1:     ld %[lo], %[obj]        \n\t"   // load double word (atomic)
        "       cmpd %[lo], %[lo]       \n\t"   // create data dependency
                                                // for load/load ordering
        "       bne- 1b                 \n\t"   // never taken
        "       srdi %[hi], %[lo], 32   \n\t"   // move high 32-bit of %[lo]
                                                // into %[hi] low 32-bit
                                                // NOTE: we're leaving the high
                                                // 32-bits of %[lo] still set,
                                                // but the caller will not see
                                                // them
        "       isync                   \n\t"
                : [lo]  "=b" (result.lo),
                  [hi]  "=b" (result.hi)
                : [obj] "m"  (*atomicInt)
                : "cr0", "ctr");

    return result.value();
}

}  // close package namespace

extern "C"
void bsls_AtomicOperations_Powerpc32_SetInt64(
                                        volatile bsls::Types::Int64 *atomicInt,
                                        // bsls::Types::Int64        value
                                        int                          valueHi,
                                        int                          valueLo)
{
    // Receiving a single 'Int64' 'val' argument as a 'valHi'/'valLo' pair of
    // arguments in order to be able to map the 'val' argument to a register
    // pair in the inline assembly code below.  The PowerPC ABI guarantees that
    // the registers used for 'val' and 'valHi'/'valLo' arguments are the same.

    asm volatile (
        "       rldimi %[valLo], %[valHi], 32, 64   \n\t"   // %[valLo] has now
                                                            // 'val' as a full
                                                            // 64-bit value
        "       sync                                \n\t"
        "       std %[valLo], %[obj]                \n\t"   // store value in
                                                            // memory (atomic)
                : [obj]  "=m" (*atomicInt)
                : [valHi] "b"  (valueHi),
                  [valLo] "b"  (valueLo));
}

extern "C"
void bsls_AtomicOperations_Powerpc32_SetInt64Relaxed(
                                        volatile bsls::Types::Int64 *atomicInt,
                                        // bsls::Types::Int64        value
                                        int                          valueHi,
                                        int                          valueLo)
{
    asm volatile (
        "       rldimi %[valLo], %[valHi], 32, 64   \n\t"   // %[valLo] has now
                                                            // 'val' as a full
                                                            // 64-bit value
        "       std %[valLo], %[obj]                \n\t"   // store value in
                                                            // memory (atomic)
                : [obj]  "=m" (*atomicInt)
                : [valHi] "b"  (valueHi),
                  [valLo] "b"  (valueLo));
}

extern "C"
void bsls_AtomicOperations_Powerpc32_SetInt64Release(
                                        volatile bsls::Types::Int64 *atomicInt,
                                        // bsls::Types::Int64        value
                                        int                          valueHi,
                                        int                          valueLo)
{
    // Receiving a single 'Int64' 'val' argument as a 'valHi'/'valLo' pair of
    // arguments in order to be able to map the 'val' argument to a register
    // pair in the inline assembly code below.  The PowerPC ABI guarantees that
    // the registers used for 'val' and 'valHi'/'valLo' arguments are the same.

    asm volatile (
        "       rldimi %[valLo], %[valHi], 32, 64   \n\t"   // %[valLo] has now
                                                            // 'val' as a full
                                                            // 64-bit value
        "       lwsync                              \n\t"
        "       std %[valLo], %[obj]                \n\t"   // store value in
                                                            // memory (atomic)
                : [obj]  "=m" (*atomicInt)
                : [valHi] "b"  (valueHi),
                  [valLo] "b"  (valueLo));
}

extern "C"
bsls::Types::Int64 bsls_AtomicOperations_Powerpc32_SwapInt64(
                                        volatile bsls::Types::Int64 *atomicInt,
                                        // bsls::Types::Int64        value
                                        int                          valueHi,
                                        int                          valueLo)
{
    Int64_Words result;

    asm volatile (
        "       rldimi %[valLo], %[valHi], 32, 64   \n\t" // %[valLo] has now
                                                          // 'val' as a full
                                                          // 64-bit value
        "       sync                                \n\t"

        "1:     ldarx %[lo], %[zero], %[obj]        \n\t" // load and reserve
        "       stdcx. %[valLo], %[zero], %[obj]    \n\t" // store old value if
                                                          // still reserved
        "       bne- 1b                             \n\t"

        "       srdi %[hi], %[lo], 32               \n\t" // move high 32-bit
                                                          // of %[lo] into
                                                          // %[hi] low 32-bit
                                                          // NOTE: we're
                                                          // leaving the high
                                                          // 32-bits of %[lo]
                                                          // still set, but the
                                                          // caller will not
                                                          // see them
        "       isync                               \n\t"
                : [lo]    "=&b" (result.lo),
                  [hi]    "=&b" (result.hi)
                : [obj]  "b"    (atomicInt),
                  [valHi] "b"   (valueHi),
                  [valLo] "b"   (valueLo),
                  [zero]  "i"   (0)
                : "cr0", "ctr");

    return result.value();
}

extern "C"
bsls::Types::Int64 bsls_AtomicOperations_Powerpc32_SwapInt64AcqRel(
                                        volatile bsls::Types::Int64 *atomicInt,
                                        // bsls::Types::Int64        value
                                        int                          valueHi,
                                        int                          valueLo)
{
    Int64_Words result;

    asm volatile (
        "       rldimi %[valLo], %[valHi], 32, 64   \n\t" // %[valLo] has now
                                                          // 'val' as a full
                                                          // 64-bit value
        "       lwsync                              \n\t"

        "1:     ldarx %[lo], %[zero], %[obj]        \n\t" // load and reserve
        "       stdcx. %[valLo], %[zero], %[obj]    \n\t" // store old value if
                                                          // still reserved
        "       bne- 1b                             \n\t"

        "       srdi %[hi], %[lo], 32               \n\t" // move high 32-bit
                                                          // of %[lo] into
                                                          // %[hi] low 32-bit
                                                          // NOTE: we're
                                                          // leaving the high
                                                          // 32-bits of %[lo]
                                                          // still set, but the
                                                          // caller will not
                                                          // see them
        "       isync                               \n\t"
                : [lo]    "=&b" (result.lo),
                  [hi]    "=&b" (result.hi)
                : [obj]  "b"    (atomicInt),
                  [valHi] "b"   (valueHi),
                  [valLo] "b"   (valueLo),
                  [zero]  "i"   (0)
                : "cr0", "ctr");

    return result.value();
}

extern "C"
bsls::Types::Int64 bsls_AtomicOperations_Powerpc32_testAndSwapInt64(
                                      volatile bsls::Types::Int64 *atomicInt,
                                      // bsls::Types::Int64        compareValue
                                      int                          compareHi,
                                      int                          compareLo,
                                      // bsls::Types::Int64        swapValue
                                      int                          swapHi,
                                      int                          swapLo)
{
    Int64_Words result;

    asm volatile (
        "       rldimi %[cmpLo], %[cmpHi], 32, 64   \n\t" // %[cmpLo] now has a
                                                          // full 64-bit value
        "       rldimi %[swapLo], %[swapHi], 32, 64 \n\t" // %[swapLo] now has
                                                          // a full 64-bit
                                                          // value
        "       sync                                \n\t"

        "1:     ldarx %[lo], %[zero], %[obj]        \n\t" // load and reserve
        "       cmpd %[cmpLo], %[lo]                \n\t" // compare values
        "       bne- 2f                             \n\t"

        "       stdcx. %[swapLo], %[zero], %[obj]   \n\t" // store new value
        "       bne- 1b                             \n\t"

        "       isync                               \n\t"

        "2:     srdi %[hi], %[lo], 32               \n\t" // move high 32-bit
                                                          // of %[lo] into
                                                          // %[hi] low 32-bit
                                                          // NOTE: we're
                                                          // leaving the high
                                                          // 32-bits of %[lo]
                                                          // still set, but the
                                                          // caller will not
                                                          // see them
           : [lo]     "=&b" (result.lo),
             [hi]     "=&b" (result.hi)
           : [obj]    "b"   (atomicInt),
             [cmpHi]  "b"   (compareHi),
             [cmpLo]  "b"   (compareLo),
             [swapHi] "b"   (swapHi),
             [swapLo] "b"   (swapLo),
             [zero]   "i"   (0)
           : "cr0", "ctr");

    return result.value();
}

extern "C"
bsls::Types::Int64 bsls_AtomicOperations_Powerpc32_testAndSwapInt64AcqRel(
                                      volatile bsls::Types::Int64 *atomicInt,
                                      // bsls::Types::Int64        compareValue
                                      int                          compareHi,
                                      int                          compareLo,
                                      // bsls::Types::Int64        swapValue
                                      int                          swapHi,
                                      int                          swapLo)
{
    Int64_Words result;

    asm volatile (
        "       rldimi %[cmpLo], %[cmpHi], 32, 64   \n\t" // %[cmpLo] now has a
                                                          // full 64-bit value
        "       rldimi %[swapLo], %[swapHi], 32, 64 \n\t" // %[swapLo] now has
                                                          // a full 64-bit
                                                          // value
        "       lwsync                              \n\t"

        "1:     ldarx %[lo], %[zero], %[obj]        \n\t" // load and reserve
        "       cmpd %[cmpLo], %[lo]                \n\t" // compare values
        "       bne- 2f                             \n\t"

        "       stdcx. %[swapLo], %[zero], %[obj]   \n\t" // store new value
        "       bne- 1b                             \n\t"

        "       isync                               \n\t"

        "2:     srdi %[hi], %[lo], 32               \n\t" // move high 32-bit
                                                          // of %[lo] into
                                                          // %[hi] low 32-bit
                                                          // NOTE: we're
                                                          // leaving the high
                                                          // 32-bits of %[lo]
                                                          // still set, but the
                                                          // caller will not
                                                          // see them
           : [lo]     "=&b" (result.lo),
             [hi]     "=&b" (result.hi)
           : [obj]    "b"   (atomicInt),
             [cmpHi]  "b"   (compareHi),
             [cmpLo]  "b"   (compareLo),
             [swapHi] "b"   (swapHi),
             [swapLo] "b"   (swapLo),
             [zero]   "i"   (0)
           : "cr0", "ctr");

    return result.value();
}

extern "C"
bsls::Types::Int64 bsls_AtomicOperations_Powerpc32_AddInt64(
                                        volatile bsls::Types::Int64 *atomicInt,
                                        // bsls::Types::Int64        value
                                        int                          valueHi,
                                        int                          valueLo)
{
    Int64_Words result;

    asm volatile (
        "       rldimi %[valLo], %[valHi], 32, 64   \n\t" // %[valLo] now has a
                                                          // full 64-bit value
        "       sync                                \n\t"

        "1:     ldarx %[lo], %[zero], %[obj]        \n\t" // load and reserve
        "       add %[lo], %[lo], %[valLo]          \n\t" // add the operand
        "       stdcx. %[lo], %[zero], %[obj]       \n\t" // store old value if
                                                          // still reserved
        "       bne- 1b                             \n\t"
        "       srdi %[hi], %[lo], 32               \n\t" // move high 32-bit
                                                          // of %[lo] into
                                                          // %[hi] low 32-bit
                                                          // NOTE: we're
                                                          // leaving the high
                                                          // 32-bits of %[lo]
                                                          // still set, but the
                                                          // caller will not
                                                          // see them
        "       isync                               \n\t"
                : [lo]    "=&b" (result.lo),
                  [hi]    "=&b" (result.hi)
                : [obj]   "b"   (atomicInt),
                  [valHi] "b"   (valueHi),
                  [valLo] "b"   (valueLo),
                  [zero]  "i"   (0)
           : "cr0", "ctr");

    return result.value();
}

extern "C"
bsls::Types::Int64 bsls_AtomicOperations_Powerpc32_AddInt64Relaxed(
                                        volatile bsls::Types::Int64 *atomicInt,
                                        // bsls::Types::Int64        value
                                        int                          valueHi,
                                        int                          valueLo)
{
    Int64_Words result;

    asm volatile (
        "       rldimi %[valLo], %[valHi], 32, 64   \n\t" // %[valLo] now has a
                                                          // full 64-bit value
        "1:     ldarx %[lo], %[zero], %[obj]        \n\t" // load and reserve
        "       add %[lo], %[lo], %[valLo]          \n\t" // add the operand
        "       stdcx. %[lo], %[zero], %[obj]       \n\t" // store old value if
                                                          // still reserved
        "       bne- 1b                             \n\t"
        "       srdi %[hi], %[lo], 32               \n\t" // move high 32-bit
                                                          // of %[lo] into
                                                          // %[hi] low 32-bit
                                                          // NOTE: we're
                                                          // leaving the high
                                                          // 32-bits of %[lo]
                                                          // still set, but the
                                                          // caller will not
                                                          // see them
                : [lo]    "=&b" (result.lo),
                  [hi]    "=&b" (result.hi)
                : [obj]   "b"   (atomicInt),
                  [valHi] "b"   (valueHi),
                  [valLo] "b"   (valueLo),
                  [zero]  "i"   (0)
           : "cr0", "ctr");

    return result.value();
}

extern "C"
bsls::Types::Int64 bsls_AtomicOperations_Powerpc32_AddInt64AcqRel(
                                        volatile bsls::Types::Int64 *atomicInt,
                                        // bsls::Types::Int64        value
                                        int                          valueHi,
                                        int                          valueLo)
{
    Int64_Words result;

    asm volatile (
        "       rldimi %[valLo], %[valHi], 32, 64   \n\t" // %[valLo] now has a
                                                          // full 64-bit value
        "       lwsync                              \n\t"

        "1:     ldarx %[lo], %[zero], %[obj]        \n\t" // load and reserve
        "       add %[lo], %[lo], %[valLo]          \n\t" // add the operand
        "       stdcx. %[lo], %[zero], %[obj]       \n\t" // store old value if
                                                          // still reserved
        "       bne- 1b                             \n\t"
        "       srdi %[hi], %[lo], 32               \n\t" // move high 32-bit
                                                          // of %[lo] into
                                                          // %[hi] low 32-bit
                                                          // NOTE: we're
                                                          // leaving the high
                                                          // 32-bits of %[lo]
                                                          // still set, but the
                                                          // caller will not
                                                          // see them
        "       isync                               \n\t"
                : [lo]    "=&b" (result.lo),
                  [hi]    "=&b" (result.hi)
                : [obj]   "b"   (atomicInt),
                  [valHi] "b"   (valueHi),
                  [valLo] "b"   (valueLo),
                  [zero]  "i"   (0)
           : "cr0", "ctr");

    return result.value();
}

}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2013 Bloomberg Finance L.P.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// ----------------------------- END-OF-FILE ----------------------------------
