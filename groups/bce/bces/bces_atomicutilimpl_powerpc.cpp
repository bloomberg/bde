// bces_atomicutilimpl_powerpc.cpp                                    -*-C++-*-

#include <bdes_ident.h>
BDES_IDENT_RCSID(bces_atomicutilimpl_powerpc_cpp,"$Id$ $CSID$")

#include <bsls_platformutil.h>
#include <bslmf_assert.h>

namespace BloombergLP {

#ifdef BSLS_PLATFORM__CPU_POWERPC

/// IMPLEMENTATION NOTES
///---------------------
//
///64-bit Integer Arguments
/// - - - - - - - - - - - -
// In 32bit mode on PowerPC, 64bit arguments are passed in two consecutive
// registers.  Functions implemented in xlC inline assembly that receive 64bit
// arguments need to load both parts of a 64bit value from two separate
// registers into a single register.  But it's not possible to accomplish
// directly using the xlC inline assembly because it requires mapping a single
// 64bit argument to a pair of registers and xlC doesn't support it.  To work
// around this limitation, the following trick is used.  A function gets
// declared with a prototype having one 64bit parameter, but implemented with a
// prototype having two 32bit parameters.  The function declaration and the
// implementation have different parameter types, and in order for them to link
// correctly, the parameter type information needs to be erased.  This is
// accomplished with the 'EXTERN_C_32' macro, which in 32bit mode declares the
// function as 'extern "C"' and removes the types of parameters from its
// prototype.
//
// For example, see functions:
//   bces_AtomicUtilImpl_PowerpcSetInt64
//   bces_AtomicUtilImpl_PowerpcAddInt64
//
///Use of 'sync' Vs. 'isync'
///- - - - - - - - - - - - -
// Use of PowerPC synchronization instructions to implement memory consistency
// guarantees: the PowerPC architecture manual may imply that the lightweight
// 'isync' instruction may be sufficient to achieve the sequential memory
// consistency guarantee instead of a heavyweight 'sync' instruction.  However
// there are subtle issues and bugs in the architecture that effectively
// require the use of 'sync'.  See the following two documents for the
// blueprint implementations of the C++ memory consistency guarantees on the
// PowerPC platform:
//
// http://www.rdrop.com/users/paulmck/scalability/paper/N2745r.2011.03.04a.html
// http://www.rdrop.com/users/paulmck/scalability/paper/
//                                                    N2745rP5.2010.02.19a.html

        // *** both 32 and 64 bit implementations ***

extern "C"
int bces_AtomicUtilImpl_PowerpcGetInt(const volatile int *value)
{
    int result;

    // __volatile__ to avoid compiler optimizations
    __asm__ __volatile__ (
             "sync                                \n\t"

// This should be just a regular load but a bug in Power5/Power5+ forces us to
// use a lwarx.  See
// http://www.rdrop.com/users/paulmck/scalability/paper/N2745r.2011.03.04a.html
// http://www.rdrop.com/users/paulmck/scalability/paper/
//                                                    N2745rP5.2010.02.19a.html

         "1:  lwarx %[result], %[zero], %[value]  \n\t"
             "cmpw %[result], %[result]           \n\t" // create data
                                                        // dependency for
                                                        // load/load ordering
             "bne- 1b                             \n\t" // never taken
             "isync                               \n\t"
           : [result] "=b" (result)
           : [value]  "b"  (value),
             [zero]   "i"  (0)
           : "cr0", "ctr");

    return result;
}

extern "C"
void bces_AtomicUtilImpl_PowerpcSetInt(volatile int *aInt, int val)
{
    // __volatile__ to avoid compiler optimizations
    __asm__ __volatile__ (
             "sync                  \n\t"
             "stw %[val], %[aInt]   \n\t"
           : [aInt] "=m" (*aInt)
           : [val]  "b"  (val));
}

extern "C"
int bces_AtomicUtilImpl_PowerpcAddInt(volatile int *aInt, int val)
{
    int result;

    // __volatile__ to avoid compiler optimizations
    __asm__ __volatile__ (
             "sync                                  \n\t"

         "1:  lwarx %[result], %[zero], %[aInt]     \n\t"  // load and reserve
             "add %[result], %[val], %[result]      \n\t"  // add the operand
             "stwcx. %[result], %[zero], %[aInt]    \n\t"  // store old value
                                                           // if still reserved
             "bne- 1b                               \n\t"
             "isync                                 \n\t"
           : [result] "=&b" (result)
           : [aInt]   "b"   (aInt),
             [val]    "b"   (val),
             [zero]   "i"   (0)
           : "cr0", "ctr");

    return result;
}

extern "C"
int bces_AtomicUtilImpl_PowerpcSwapInt(volatile int *aInt,
                                       int val)
{
    int result;

    // __volatile__ to avoid compiler optimizations
    __asm__ __volatile__(
            "sync                               \n\t"

        "1:  lwarx %[result], %[zero], %[aInt]  \n\t"
            "stwcx. %[val], %[zero], %[aInt]    \n\t"
            "bne- 1b                            \n\t"
            "isync                              \n\t"
          : [result] "=&b" (result)
          : [aInt]   "b"   (aInt),
            [val]    "b"   (val),
            [zero]   "i"   (0)
          : "cr0", "ctr");

    return result;
}

extern "C"
int bces_AtomicUtilImpl_PowerpcTestAndSwap(volatile int *aInt,
                                           int           cmpVal,
                                           int           swapVal)
{
    int result;

    // __volatile__ to avoid compiler optimizations
    __asm__ __volatile__ (
             "sync                                \n\t"

         "1:  lwarx %[result], %[zero], %[aInt]   \n\t"  // load and reserve
             "cmpw %[cmpVal], %[result]           \n\t"  // compare the values
             "bne- 2f                             \n\t"
             "stwcx. %[swapVal], %[zero], %[aInt] \n\t"  // store the new value
             "bne- 1b                             \n\t"
             "isync                               \n\t"
         "2:                                      \n\t"
           : [result]  "=&b" (result)
           : [aInt]    "b"   (aInt),
             [cmpVal]  "b"   (cmpVal),
             [swapVal] "b"   (swapVal),
             [zero]    "i"   (0)
           : "cr0", "ctr");

    return result;
}

extern "C"
void bces_AtomicUtilImpl_PowerpcSpinLock(volatile int *aSpin)
{
    int temp;

    // __volatile__ to avoid compiler optimizations
    __asm__ __volatile__ (
         "1:  lwarx %[temp], %[zero], %[aSpin]   \n\t"   // load and reserve
             "cmpwi %[temp], 0                   \n\t"   // compare the values
             "bne- 1b                            \n\t"
             "stwcx. %[aSpin], %[zero], %[aSpin] \n\t"   // store the new value
             "bne- 1b                            \n\t"   // lost reservation?
             "isync                              \n\t"
           : [temp]  "=&b" (temp)
           : [aSpin] "b"   (aSpin),
             [zero]  "i"   (0)
           : "cr0", "ctr");
}

extern "C"
int  bces_AtomicUtilImpl_PowerpcSpinTryLock(volatile int *aSpin,
                                            int           retries)
{
    int result;

    // __volatile__ to avoid compiler optimizations
    __asm__ __volatile__ (
         "1:  lwarx %[result], %[zero], %[aSpin]  \n\t"  // load and reserve
             "cmpwi %[result], 0                  \n\t"  // compare the values
             "bne- 2f                             \n\t"

             "stwcx. %[aSpin], %[zero], %[aSpin]  \n\t"  // store the new value
             "bne- 2f                             \n\t"  // lost reservation?

             "isync                               \n\t"
             "li %[result], 0                     \n\t"  // return success
             "b 3f                                \n\t"

         "2:  addi %[retries], %[retries], -1     \n\t"  // --retries
             "cmpwi %[retries], 0                 \n\t"  // if (retries > 0)
             "bne- 1b                             \n\t"  //     retry lock
             "li %[result], 255                   \n\t"  // return error
         "3:                                      \n\t"
           : [result]  "=&b" (result)
           : [aSpin]   "b"   (aSpin),
             [retries] "b"   (retries),
             [zero]    "i"   (0)
           : "cr0", "ctr");

    return result;
}

extern "C"
void bces_AtomicUtilImpl_PowerpcSpinUnlock(volatile int *aSpin)
{
    int temp;

    // __volatile__ to avoid compiler optimizations
    __asm__ __volatile__ (
             "lwsync                    \n\t"
             "li %[temp], 0             \n\t"
             "stw %[temp], %[aSpin]     \n\t"
           : [temp]  "=&b" (temp),
             [aSpin] "=m"  (*aSpin));
}

#if !defined(BSLS_PLATFORM__CPU_64_BIT)

        // *** 32 bit implementations ***

namespace {

struct Int64_Words
    // Provide access to the individual high and low words of the 'Int64' value
    // to allow inline assembly mapping of PowerPC registers directly to these
    // words in 32 bit mode.  Note that the layout of 'Int64_Words' assumes a
    // big-endian architecture.
{
    int hi;
    int lo;

    const bsls_PlatformUtil::Int64& value() const
    {
        return reinterpret_cast<const bsls_PlatformUtil::Int64&>(*this);
    }
};

BSLMF_ASSERT(sizeof(Int64_Words) == sizeof(bsls_PlatformUtil::Int64));

}

extern "C"
bsls_PlatformUtil::Int64
    bces_AtomicUtilImpl_PowerpcGetInt64(
                                  const volatile bsls_PlatformUtil::Int64 *val)
{
    Int64_Words result;

    // __volatile__ to avoid compiler optimizations
    __asm__ __volatile__(
            "sync                       \n\t"

        "1:  ld %[lo], %[val]           \n\t"   // load double word (atomic)
            "cmpd %[lo], %[lo]          \n\t"   // create data dependency for
                                                // load/load ordering
            "bne- 1b                    \n\t"   // never taken
            "srdi %[hi], %[lo], 32      \n\t"   // move high 32-bit of %[lo]
                                                // into %[hi] low 32-bit
                                                // NOTE: we're leaving the high
                                                // 32-bits of %[lo] still set,
                                                // but the caller will not see
                                                // them
            "isync              \n\t"
          : [lo]  "=b" (result.lo),
            [hi]  "=b" (result.hi)
          : [val] "m"  (*val)
          : "cr0", "ctr");

    return result.value();
}

extern "C"
bsls_PlatformUtil::Int64
    bces_AtomicUtilImpl_PowerpcGetInt64Relaxed(
                                  const volatile bsls_PlatformUtil::Int64 *val)
{
    Int64_Words result;

    // __volatile__ to avoid compiler optimizations
    __asm__ __volatile__(
            "ld %[lo], %[val]       \n\t"   // load double word (atomic)
            "srdi %[hi], %[lo], 32  \n\t"   // move high 32-bit of %[lo] into
                                            // %[hi] low 32-bit
                                            // NOTE: we're leaving the high
                                            // 32-bits of %[lo] still set, but
                                            // the caller will not see them
          : [lo]  "=b" (result.lo),
            [hi]  "=b" (result.hi)
          : [val] "m"  (*val));

    return result.value();
}

extern "C"
void bces_AtomicUtilImpl_PowerpcSetInt64(
                                      volatile bsls_PlatformUtil::Int64 *aInt,
                                      // bsls_PlatformUtil::Int64        val
                                      int                                valHi,
                                      int                                valLo)
{
    // Receiving a single 'Int64' 'val' argument as a 'valHi'/'valLo' pair of
    // arguments in order to be able to map the 'val' argument to a register
    // pair in the inline assembly code below.  The PowerPC ABI guarantees that
    // the registers used for 'val' and 'valHi'/'valLo' arguments are the same.

    // __volatile__ to avoid compiler optimizations
    __asm__ __volatile__ (
             "rldimi %[valLo], %[valHi], 32, 64 \n\t"   // %[valLo] has now
                                                        // 'val' as a full
                                                        // 64-bit value
             "sync                              \n\t"
             "std %[valLo], %[aInt]             \n\t"   // store value in
                                                        // memory (atomic)
           : [aInt]  "=m" (*aInt)
           : [valHi] "b"  (valHi),
             [valLo] "b"  (valLo));
}

extern "C"
void bces_AtomicUtilImpl_PowerpcSetInt64Relaxed(
                                      volatile bsls_PlatformUtil::Int64 *aInt,
                                      // bsls_PlatformUtil::Int64        val
                                      int                                valHi,
                                      int                                valLo)
{
    // __volatile__ to avoid compiler optimizations
    __asm__ __volatile__ (
             "rldimi %[valLo], %[valHi], 32, 64 \n\t" // %[valLo] has now 'val'
                                                      // as a full 64-bit value
             "std %[valLo], %[aInt]            \n\t"  // store atomically
           : [aInt]  "=m" (*aInt)
           : [valHi] "b"  (valHi),
             [valLo] "b"  (valLo));
}

extern "C"
bsls_PlatformUtil::Int64
    bces_AtomicUtilImpl_PowerpcAddInt64(
                                      volatile bsls_PlatformUtil::Int64 *aInt,
                                      // bsls_PlatformUtil::Int64        val
                                      int                                valHi,
                                      int                                valLo)
{
    Int64_Words result;

    // __volatile__ to avoid compiler optimizations
    __asm__ __volatile__ (
             "rldimi %[valLo], %[valHi], 32, 64 \n\t" // %[valLo] has now 'val'
                                                      // as a full 64-bit value
             "sync                              \n\t"

         "1:  ldarx %[lo], %[zero], %[aInt]     \n\t" // load and reserve
             "add %[lo], %[lo], %[valLo]        \n\t" // add the operand
             "stdcx. %[lo], %[zero], %[aInt]    \n\t" // store old value if
                                                      // still reserved
             "bne- 1b                           \n\t"
             "srdi %[hi], %[lo], 32             \n\t" // move high 32-bit of
                                                      // %[lo] into %[hi] low
                                                      // 32-bit
                                                      // NOTE: we're leaving
                                                      // the high 32-bits of
                                                      // %[lo] still set, but
                                                      // the caller will not
                                                      // see them
             "isync                             \n\t"
           : [lo]    "=&b" (result.lo),
             [hi]    "=&b" (result.hi)
           : [aInt]  "b"   (aInt),
             [valHi] "b"   (valHi),
             [valLo] "b"   (valLo),
             [zero]  "i"   (0)
           : "cr0", "ctr");

    return result.value();
}

extern "C"
bsls_PlatformUtil::Int64
    bces_AtomicUtilImpl_PowerpcAddInt64Relaxed(
                                      volatile bsls_PlatformUtil::Int64 *aInt,
                                      // bsls_PlatformUtil::Int64        val
                                      int                                valHi,
                                      int                                valLo)
{
    Int64_Words result;

    // __volatile__ to avoid compiler optimizations
    __asm__ __volatile__ (
             "rldimi %[valLo], %[valHi], 32, 64 \n\t" // %[valLo] has now 'val'
                                                      // as a full 64-bit value
         "1:  ldarx %[lo], %[zero], %[aInt]     \n\t" // load and reserve
             "add %[lo], %[lo], %[valLo]        \n\t" // add the operand
             "stdcx. %[lo], %[zero], %[aInt]    \n\t" // store old value if
                                                      // still reserved
             "bne- 1b                           \n\t"
             "srdi %[hi], %[lo], 32             \n\t" // move high 32-bit of
                                                      // %[lo] into %[hi] low
                                                      // 32-bit
                                                      // NOTE: we're leaving
                                                      // the high 32-bits of
                                                      // %[lo] still set, but
                                                      // the caller will not
                                                      // see them
           : [lo]    "=&b" (result.lo),
             [hi]    "=&b" (result.hi)
           : [aInt]  "b"   (aInt),
             [valHi] "b"   (valHi),
             [valLo] "b"   (valLo),
             [zero]  "i"   (0)
           : "cr0", "ctr");

    return result.value();
}

extern "C"
bsls_PlatformUtil::Int64
    bces_AtomicUtilImpl_PowerpcSwapInt64(
                                      volatile bsls_PlatformUtil::Int64 *aInt,
                                      // bsls_PlatformUtil::Int64        val
                                      int                                valHi,
                                      int                                valLo)
{
    Int64_Words result;

    // __volatile__ to avoid compiler optimizations
    __asm__ __volatile__ (
             "rldimi %[valLo], %[valHi], 32, 64 \n\t" // %[valLo] has now 'val'
                                                      // as a full 64-bit value
             "sync                              \n\t"

         "1:  ldarx %[lo], %[zero], %[aInt]     \n\t" // load and reserve
             "stdcx. %[valLo], %[zero], %[aInt] \n\t" // store old value if
                                                      // still reserved
             "bne- 1b                           \n\t"

             "srdi %[hi], %[lo], 32             \n\t" // move high 32-bit of
                                                      // %[lo] into %[hi] low
                                                      // 32-bit
                                                      // NOTE: we're leaving
                                                      // the high 32-bits of
                                                      // %[lo] still set, but
                                                      // the caller will not
                                                      // see them
             "isync                             \n\t"
           : [lo]    "=&b" (result.lo),
             [hi]    "=&b" (result.hi)
           : [aInt]  "b"   (aInt),
             [valHi] "b"   (valHi),
             [valLo] "b"   (valLo),
             [zero]  "i"   (0)
           : "cr0", "ctr");

    return result.value();
}

extern "C"
bsls_PlatformUtil::Int64
    bces_AtomicUtilImpl_PowerpcTestAndSwap64(
                                  volatile bsls_PlatformUtil::Int64 *aInt,
                                  // bsls_PlatformUtil::Int64        cmpVal
                                  int                                cmpValHi,
                                  int                                cmpValLo,
                                  // bsls_PlatformUtil::Int64        swapVal
                                  int                                swapValHi,
                                  int                                swapValLo)
{
    Int64_Words result;

    // __volatile__ to avoid compiler optimizations
    __asm__ __volatile__ (
             "rldimi %[cmpValLo], %[cmpValHi], 32, 64   \n\t"
                                                          // %[cmpValLo] has
                                                          // now 'cmpVal' as a
                                                          // full 64-bit value
             "rldimi %[swapValLo], %[swapValHi], 32, 64 \n\t"
                                                          // %[swapValLo] has
                                                          // now 'swapVal' as a
                                                          // full 64-bit value
             "sync                                  \n\t"

         "1:  ldarx %[lo], %[zero], %[aInt]         \n\t" // load and reserve
             "cmpd %[cmpValLo], %[lo]               \n\t" // compare values
             "bne- 2f                               \n\t"

             "stdcx. %[swapValLo], %[zero], %[aInt] \n\t" // store new value
             "bne- 1b                               \n\t"
             "isync                                 \n\t"

         "2:  srdi %[hi], %[lo], 32                 \n\t" // move high 32-bit
                                                          // of %[lo] into
                                                          // %[hi] low 32-bit
                                                          // NOTE: we're
                                                          // leaving the high
                                                          // 32-bits of %[lo]
                                                          // still set, but the
                                                          // caller will not
                                                          // see them
           : [lo]        "=&b" (result.lo),
             [hi]        "=&b" (result.hi)
           : [aInt]      "b"   (aInt),
             [cmpValHi]  "b"   (cmpValHi),
             [cmpValLo]  "b"   (cmpValLo),
             [swapValHi] "b"   (swapValHi),
             [swapValLo] "b"   (swapValLo),
             [zero]      "i"   (0)
           : "cr0", "ctr");

    return result.value();
}

#else

        // *** 64 bit implementations ***

extern "C"
bsls_PlatformUtil::Int64 bces_AtomicUtilImpl_PowerpcGetInt64(
                                const volatile bsls_PlatformUtil::Int64 *value)
{
    bsls_PlatformUtil::Int64 result;

    // __volatile__ to avoid compiler optimizations
    __asm__ __volatile__(
             "sync                                  \n\t"

// This should be just a regular load but a bug in Power5/Power5+ forces us to
// use a ldarx.  see
// http://www.rdrop.com/users/paulmck/scalability/paper/N2745r.2011.03.04a.html
// http://www.rdrop.com/users/paulmck/scalability/paper/
//                                                    N2745rP5.2010.02.19a.html

         "1:  ldarx %[result], %[zero], %[value]    \n\t"
             "cmpd %[result], %[result]             \n\t" // create data
                                                          // dependency for
                                                          // load/load ordering
             "bne- 1b                               \n\t" // never taken
             "isync                                 \n\t"
           : [result] "=b" (result)
           : [value]  "b"  (value),
             [zero]   "i"  (0)
           : "cr0", "ctr");

    return result;
}

extern "C"
void bces_AtomicUtilImpl_PowerpcSetInt64(
                                       volatile bsls_PlatformUtil::Int64 *aInt,
                                       bsls_PlatformUtil::Int64           val)
{
    // __volatile__ to avoid compiler optimizations
    __asm__ __volatile__ (
             "sync                  \n\t"
             "std %[val], %[aInt]   \n\t"
           : [aInt] "=m" (*aInt)
           : [val]  "b"  (val));
}

extern "C"
bsls_PlatformUtil::Int64
    bces_AtomicUtilImpl_PowerpcAddInt64(
                                      volatile bsls_PlatformUtil::Int64 *aInt,
                                      bsls_PlatformUtil::Int64           val)
{
    bsls_PlatformUtil::Int64 result;

    // __volatile__ to avoid compiler optimizations
    __asm__ __volatile__ (
             "sync                                \n\t"

         "1:  ldarx %[result], %[zero], %[aInt]   \n\t" // load and reserve
             "add %[result], %[result], %[val]    \n\t" // add the operand
             "stdcx. %[result], %[zero], %[aInt]  \n\t" // store old value if
                                                        // still reserved

             "bne- 1b                             \n\t"

             "isync                               \n\t"
           : [result] "=&b" (result)
           : [aInt]   "b"   (aInt),
             [val]    "b"   (val),
             [zero]   "i"   (0)
           : "cr0", "ctr");

    return result;
}

extern "C"
bsls_PlatformUtil::Int64
    bces_AtomicUtilImpl_PowerpcSwapInt64(
                                      volatile bsls_PlatformUtil::Int64 *aInt,
                                      bsls_PlatformUtil::Int64           val)
{
    bsls_PlatformUtil::Int64 result;

    // __volatile__ to avoid compiler optimizations
    __asm__ __volatile__(
            "sync                               \n\t"

        "1:  ldarx %[result], %[zero], %[aInt]  \n\t"
            "stdcx. %[val], %[zero], %[aInt]    \n\t"
            "bne- 1b                            \n\t"

            "isync                              \n\t"
          : [result] "=&b" (result)
          : [aInt]   "b"   (aInt),
            [val]    "b"   (val),
            [zero]   "i"   (0)
          : "cr0", "ctr");

    return result;
}

extern "C"
bsls_PlatformUtil::Int64
    bces_AtomicUtilImpl_PowerpcTestAndSwap64(
                                  volatile bsls_PlatformUtil::Int64 *aInt,
                                  bsls_PlatformUtil::Int64           cmpVal,
                                  bsls_PlatformUtil::Int64           swapVal)
{
    bsls_PlatformUtil::Int64 result;

    // __volatile__ to avoid compiler optimizations
    __asm__ __volatile__ (
             "sync                                \n\t"

         "1:  ldarx %[result], %[zero], %[aInt]   \n\t"  // load and reserve
             "cmpd %[cmpVal], %[result]           \n\t"  // compare values
             "bne- 2f                             \n\t"

             "stdcx. %[swapVal], %[zero], %[aInt] \n\t"  // store the new value
             "bne- 1b                             \n\t"
             "isync                               \n\t"
         "2:                                      \n\t"
           : [result]  "=&b" (result)
           : [aInt]    "b"   (aInt),
             [cmpVal]  "b"   (cmpVal),
             [swapVal] "b"   (swapVal),
             [zero]    "i"   (0)
           : "cr0", "ctr");

    return result;
}

#endif  // BSLS_PLATFORM__CPU_64_BIT

#endif  // BSLS_PLATFORM__CPU_POWERPC

}  // close enterprise namespace

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2011
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
