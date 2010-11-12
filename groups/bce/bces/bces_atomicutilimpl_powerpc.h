// bces_atomicutilimpl_powerpc.h                                      -*-C++-*-
#ifndef INCLUDED_BCES_ATOMICUTILIMPL_POWERPC
#define INCLUDED_BCES_ATOMICUTILIMPL_POWERPC

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide atomic operations on the POWERPC platform.
//
//@CLASSES:
//   bces_AtomicUtilImpl<bsls_Platform::CpuPowerpc>: namespace for atomic
//   operations on 32-/64-bit powerpc architectures
//
//@AUTHOR: Ilougino Rocha (irocha)
//
//@DESCRIPTION: This component provides a full specialization of
// 'bces_AtomicUtilImpl' for 32-/64-bit powerpc architectures.  The component
// is not intended to be used directly.  See the 'bces_atomicutil' for more on
// using atomic operations.
//
///Compatibility
///-------------
// These atomic operations are designed to be run in 32-bit mode on a 64-bit
// cpu and 64-bit kernel.  32-bit kernels and 32-bit cpu are *NOT* supported
// and will result in undefined runtime behavior.

#ifndef INCLUDED_BCESCM_VERSION
#include <bcescm_version.h>
#endif

#ifndef INCLUDED_BSLS_ALIGNMENTUTIL
#include <bsls_alignmentutil.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSLS_PLATFORM
#include <bsls_platform.h>
#endif

#ifndef INCLUDED_BSLS_PLATFORMUTIL
#include <bsls_platformutil.h>
#endif

#ifdef BSLS_PLATFORM__CPU_POWERPC
#ifdef BSLS_PLATFORM__CMP_IBM
// builtins.h is xlc-specific.
#ifndef INCLUDED_BUILTINS
#include <builtins.h>
#define INCLUDED_BUILTINS
#endif
#else
// DRQS 16384546 - NOT IMPLEMENTED - these are present to allow g++
// compilation.  This will compile but not link for g++ on AIX, allowing
// cscompile checks to succeed.
int __fetch_and_add(volatile int *, int);
long __fetch_and_addlp(volatile long *, long);
#endif

namespace BloombergLP {

extern "C" {

#ifdef BSLS_PLATFORM__CPU_64_BIT
    int bces_AtomicUtilImpl_PowerpcSwapInt(volatile int *aInt, int val);
#else
    bsls_PlatformUtil::Int64 bces_AtomicUtilImpl_PowerpcGetInt64Relaxed(
                                    const volatile bsls_PlatformUtil::Int64 *);
    void  bces_AtomicUtilImpl_PowerpcSetInt64Relaxed(
                                       volatile bsls_PlatformUtil::Int64 *aInt,
                                       bsls_PlatformUtil::Int64           val);

    bsls_PlatformUtil::Int64 bces_AtomicUtilImpl_PowerpcAddInt64Relaxed(
                                       volatile bsls_PlatformUtil::Int64 *aInt,
                                       bsls_PlatformUtil::Int64           val);

    bsls_PlatformUtil::Int64 bces_AtomicUtilImpl_PowerpcGetInt64(
                                    const volatile bsls_PlatformUtil::Int64 *);
#endif

    int bces_AtomicUtilImpl_PowerpcAddInt(volatile int *aInt, int val);

    int bces_AtomicUtilImpl_PowerpcTestAndSwap(volatile int *aInt,
                                               int           cmpVal,
                                               int           swapVal);

    bsls_PlatformUtil::Int64  bces_AtomicUtilImpl_PowerpcSwapInt64(
                                       volatile bsls_PlatformUtil::Int64 *aInt,
                                       bsls_PlatformUtil::Int64           val);

    bsls_PlatformUtil::Int64 bces_AtomicUtilImpl_PowerpcAddInt64(
                                       volatile bsls_PlatformUtil::Int64 *aInt,
                                       bsls_PlatformUtil::Int64           val);

    bsls_PlatformUtil::Int64 bces_AtomicUtilImpl_PowerpcTestAndSwap64(
                                   volatile bsls_PlatformUtil::Int64 *aInt,
                                   bsls_PlatformUtil::Int64           cmpVal,
                                   bsls_PlatformUtil::Int64           swapVal);

    void bces_AtomicUtilImpl_PowerpcSpinLock(volatile int *aSpin);
    int  bces_AtomicUtilImpl_PowerpcSpinTryLock(volatile int *aSpin,
                                                int           retries);
    void bces_AtomicUtilImpl_PowerpcSpinUnlock(volatile int *aSpin);
    void bces_AtomicUtilImpl_PowerpcSetInt64(
                                       volatile bsls_PlatformUtil::Int64 *aInt,
                                       bsls_PlatformUtil::Int64           val);
}

#if BSLS_PLATFORM__CMP_VER_MAJOR >= 0xa00
    // xlc10: can't inline anything because of DRQS 16073004

    int bces_AtomicUtilImpl_PowerpcGetInt(const volatile int *);
    int bces_AtomicUtilImpl_PowerpcSwapInt(volatile int *aInt, int val);

#if defined(BSLS_PLATFORM__CPU_64_BIT)
    bsls_PlatformUtil::Int64 bces_AtomicUtilImpl_PowerpcGetInt64(
                                    const volatile bsls_PlatformUtil::Int64 *);
void bces_AtomicUtilImpl_PowerpcSetInt64(
                                       volatile bsls_PlatformUtil::Int64 *aInt,
                                       bsls_PlatformUtil::Int64           val);
#endif

#else
    // xlc 8

    inline
    int bces_AtomicUtilImpl_PowerpcGetInt(const volatile int *value)
    {
        int result;
        __asm__ __volatile__ (
                 "sync\n\t"
                 "1:\n\t"
                 "lwarx %0, %2, %1\n\t" // see note in .cpp
                 "cmpw %0, %0\n" // create data dependency for load/load order
                 "bne- 1b\n\t"   // never taken
                 "isync\n\t"
               : "=r"(result)
               : "r"(value), "i"(0)
               : "cr0", "ctr");
        return result;
    }

#if !defined(BSLS_PLATFORM__CPU_64_BIT)
    inline
    int bces_AtomicUtilImpl_PowerpcSwapInt(volatile int *aInt, int val)
    {
        int result;
        __asm__ __volatile__(
                 "lwsync\n\t"
                 "1:\n\t"
                 "lwarx %0, %3, %1\n\t"
                 "stwcx. %2, %3, %1\n\t"
                 "bne- 1b\n\t"
                 "isync"
              : "=&r"(result)
              : "r"(aInt), "r"(val), "i"(0)
              : "cr0", "ctr");
        return result;
    }

#else
    inline
    bsls_PlatformUtil::Int64 bces_AtomicUtilImpl_PowerpcGetInt64(
                                const volatile bsls_PlatformUtil::Int64 *value)
    {
        bsls_PlatformUtil::Int64 result;
        __asm__ __volatile__(
                 "sync\n\t"
                 "1:\n\t"
                 "ldarx %0, %2, %1\n\t" // see note in .cpp
                 "cmpd %0, %0\n\t" // create fake dependency for ordering
                 "bne- 1b\n\t"     // never taken
                 "isync"
               : "=r"(result)
               : "r"(value), "i"(0)
               : "cr0", "ctr");
        return result;
    }

#endif

#endif

inline
int bces_AtomicUtilImpl_PowerpcAddIntRelaxed(volatile int *aInt, int val)
{
    return __fetch_and_add(aInt, val) + val;
}

void bces_AtomicUtilImpl_PowerpcSetInt(volatile int *aInt, int val);

#ifdef BSLS_PLATFORM__CPU_64_BIT

inline
bsls_PlatformUtil::Int64
bces_AtomicUtilImpl_PowerpcAddInt64Relaxed(
                                       volatile bsls_PlatformUtil::Int64 *aInt,
                                       bsls_PlatformUtil::Int64           val)
{
    return __fetch_and_addlp((volatile long *)aInt, (long)val) + val;
}

#else

bsls_PlatformUtil::Int64
bces_AtomicUtilImpl_PowerpcAddInt64Relaxed(
                                       volatile bsls_PlatformUtil::Int64 *aInt,
                                       bsls_PlatformUtil::Int64           val);
#endif

template <typename PLATFORM> struct bces_AtomicUtilImpl;

template <>
struct bces_AtomicUtilImpl<bsls_Platform::CpuPowerpc> {
    // Namespace for atomic operations on the POWERPC architecture.

    // TYPES

    struct Int {
        volatile int d_value __attribute__((__aligned__(sizeof(int))));
    };

    struct Int64 {
        volatile bsls_PlatformUtil::Int64 d_value
              __attribute__((__aligned__(sizeof(bsls_PlatformUtil::Int64))));
    };

    struct Pointer {
        const void *volatile d_value
              __attribute__((__aligned__(sizeof(void *))));
    };

    struct SpinLock {
        volatile int d_lock;
    };

    static void initInt(
            bces_AtomicUtilImpl<bsls_Platform::CpuPowerpc>::Int *aInt,
            int                                                  initialValue);
        // Initialize the specified 'aInt' and set its value to the specified
        // 'initialValue'.  Note that this method must be called before any
        // other operations on 'aInt'.

    static void initInt64(
                   bces_AtomicUtilImpl<bsls_Platform::CpuPowerpc>::Int64 *aInt,
                   bsls_PlatformUtil::Int64                               val);
        // Initialize the specified 'aInt' and set its value to the specified
        // 'initialValue'.  Note that this method must be called before any
        // other operations on 'aInt'.

    static void initPointer(
        bces_AtomicUtilImpl<bsls_Platform::CpuPowerpc>::Pointer *aPtr,
        const void                                              *initialValue);
        // Initialize the specified 'aPointer' and set its value to the
        // specified 'initialValue'.  Note that this function must be called
        // before any other operations on 'aPointer'.

    static void initSpinLock(
              bces_AtomicUtilImpl<bsls_Platform::CpuPowerpc>::SpinLock *aSpin);
        // Initialize the specified 'aSpin' and set it to an an unlocked state.
        // Note that method must be called before any other operations on
        // 'aSpin'.

    static void addInt(
                     bces_AtomicUtilImpl<bsls_Platform::CpuPowerpc>::Int *aInt,
                     int                                                  val);
        // Atomically add to the specified 'aInt' the specified 'value'.  Note
        // that the behavior is undefined if 'aInt' is 0.

    static void incrementInt(
                    bces_AtomicUtilImpl<bsls_Platform::CpuPowerpc>::Int *aInt);
        // Atomically increment the value of the specified 'aInt' by 1.  Note
        // that the behavior is undefined if 'aInt' is 0.

    static void decrementInt(
                    bces_AtomicUtilImpl<bsls_Platform::CpuPowerpc>::Int *aInt);
        // Atomically decrement the value of the specified 'aInt' by 1.  Note
        // that the behavior is undefined if 'aInt' is 0.

    static int swapInt(
                     bces_AtomicUtilImpl<bsls_Platform::CpuPowerpc>::Int *aInt,
                     int                                                  val);
        // Atomically set the value of the specified 'aInt' to the specified
        // 'value' and return its previous value.  Note that the behavior is
        // undefined if 'aInt' is 0.

    static int testAndSwapInt(
                 bces_AtomicUtilImpl<bsls_Platform::CpuPowerpc>::Int *aInt,
                 int                                                  cmpVal,
                 int                                                  swapVal);
        // Conditionally set the value of the specified 'aInt' to the specified
        // 'swapVal' and return its previous value.  The value of 'aInt' is
        // compared to the specified 'cmpVal'; if they are equal, then
        // 'swapVal' will be assigned to 'aInt', otherwise the value of 'aInt'
        // is left unchanged.  The whole operation is performed atomically.
        // Note that the behavior is undefined if 'aInt' is 0.

    static void setInt(
                     bces_AtomicUtilImpl<bsls_Platform::CpuPowerpc>::Int *aInt,
                     int                                                  val);
        // Atomically set the value of the specified 'aInt' to the specified
        // 'value'.  Note that the behavior is undefined if 'aInt' is 0.

    static void setIntRelaxed(
                     bces_AtomicUtilImpl<bsls_Platform::CpuPowerpc>::Int *aInt,
                     int                                                  val);
        // Atomically set the value of the specified 'aInt' to the specified
        // 'value', without a memory barrier.  Note that the behavior is
        // undefined if 'aInt' is 0.

    static int getInt(
              const bces_AtomicUtilImpl<bsls_Platform::CpuPowerpc>::Int& aInt);
        // Atomically retrieve the value of the specified 'aInt'.

    static int getIntRelaxed(
              const bces_AtomicUtilImpl<bsls_Platform::CpuPowerpc>::Int& aInt);
        // Atomically retrieve the value of the specified 'aInt', without a
        // memory barrier.

    static int addIntNv(
                     bces_AtomicUtilImpl<bsls_Platform::CpuPowerpc>::Int *aInt,
                     int                                                  val);
        // Atomically add to the specified 'aInt' the specified 'value' and
        // return the resulting value.  Note that the behavior is undefined if
        // 'aInt' is 0.

    static int addIntNvRelaxed(
                     bces_AtomicUtilImpl<bsls_Platform::CpuPowerpc>::Int *aInt,
                     int                                                  val);
        // Atomically add to the specified 'aInt' the specified 'value' and
        // return the resulting value, without a memory barrier.  The behavior
        // is undefined if 'aInt' is 0.

    static int incrementIntNv(
                    bces_AtomicUtilImpl<bsls_Platform::CpuPowerpc>::Int *aInt);
        // Atomically increment the specified 'aInt' by 1 and return the
        // resulting value.  Note that the behavior is undefined if 'aInt' is
        // 0.

    static int decrementIntNv(
                    bces_AtomicUtilImpl<bsls_Platform::CpuPowerpc>::Int *aInt);
        // Atomically decrement the specified 'aInt' by 1 and return the
        // resulting value.  Note that the behavior is undefined if 'aInt' is
        // 0.

    static void addInt64(
                   bces_AtomicUtilImpl<bsls_Platform::CpuPowerpc>::Int64 *aInt,
                   bsls_PlatformUtil::Int64                               val);
        // Atomically add to the specified 'aInt' the specified 'value'.  Note
        // that the behavior is undefined if 'aInt' is 0.

    static void incrementInt64(
                  bces_AtomicUtilImpl<bsls_Platform::CpuPowerpc>::Int64 *aInt);
        // Atomically increment the value of the specified 'aInt' by 1.  Note
        // that the behavior is undefined if 'aInt' is 0.

    static void decrementInt64(
                  bces_AtomicUtilImpl<bsls_Platform::CpuPowerpc>::Int64 *aInt);
        // Atomically decrement the value of the specified 'aInt' by 1.  Note
        // that the behavior is undefined if 'aInt' is 0.

    static bsls_PlatformUtil::Int64 swapInt64(
                   bces_AtomicUtilImpl<bsls_Platform::CpuPowerpc>::Int64 *aInt,
                   bsls_PlatformUtil::Int64                               val);
        // Atomically set the value of the specified 'aInt' to the specified
        // 'value' and return its previous value.  Note that the behavior is
        // undefined if 'aInt' is 0.

    static bsls_PlatformUtil::Int64 testAndSwapInt64(
                bces_AtomicUtilImpl<bsls_Platform::CpuPowerpc>::Int64 *aInt,
                bsls_PlatformUtil::Int64                              cmpVal,
                bsls_PlatformUtil::Int64                              swapVal);
        // Conditionally set the value of the specified 'aInt' to the specified
        // 'swapVal' and return its previous value.  The value of 'aInt' is
        // compared to the specified 'cmpVal'; if they are equal, then
        // 'swapVal' will be assigned to 'aInt' otherwise, the value of 'aInt'
        // is left unchanged.  The whole operation is performed atomically.
        // Note that the behavior is undefined if 'aInt' is 0.

    static bsls_PlatformUtil::Int64 addInt64Nv(
                   bces_AtomicUtilImpl<bsls_Platform::CpuPowerpc>::Int64 *aInt,
                   bsls_PlatformUtil::Int64                               val);
        // Atomically add to the specified 'aInt' the specified 'value' and
        // return the resulting value.  Note that the behavior is undefined if
        // 'aInt' is 0.

    static bsls_PlatformUtil::Int64 addInt64NvRelaxed(
                   bces_AtomicUtilImpl<bsls_Platform::CpuPowerpc>::Int64 *aInt,
                   bsls_PlatformUtil::Int64                               val);
        // Atomically add to the specified 'aInt' the specified 'value' and
        // return the resulting value, without a memory barrier.  The behavior
        // is undefined if 'aInt' is 0.

    static bsls_PlatformUtil::Int64 incrementInt64Nv(
                  bces_AtomicUtilImpl<bsls_Platform::CpuPowerpc>::Int64 *aInt);
        // Atomically increment the specified 'aInt' by 1 and return the
        // resulting value.  The behavior is undefined if 'aInt' is 0.

    static bsls_PlatformUtil::Int64 decrementInt64Nv(
                  bces_AtomicUtilImpl<bsls_Platform::CpuPowerpc>::Int64 *aInt);
        // Atomically decrement the specified 'aInt' by 1 and return the
        // resulting value.  The behavior is undefined if 'aInt' is 0.

    static void setInt64(
                   bces_AtomicUtilImpl<bsls_Platform::CpuPowerpc>::Int64 *aInt,
                   bsls_PlatformUtil::Int64                               val);
        // Atomically set the value of the specified 'aInt' to the specified
        // 'value'.  The behavior is undefined if 'aInt' is 0.

    static void setInt64Relaxed(
                   bces_AtomicUtilImpl<bsls_Platform::CpuPowerpc>::Int64 *aInt,
                   bsls_PlatformUtil::Int64                               val);
        // Atomically set the value of the specified 'aInt' to the specified
        // 'value' without a memory barrier.  The behavior is undefined if
        // 'aInt' is 0.

    static bsls_PlatformUtil::Int64 getInt64(
            const bces_AtomicUtilImpl<bsls_Platform::CpuPowerpc>::Int64& aInt);
        // Atomically retrieve the value of the specified 'aInt'.

    static bsls_PlatformUtil::Int64 getInt64Relaxed(
            const bces_AtomicUtilImpl<bsls_Platform::CpuPowerpc>::Int64& aInt);
        // Atomically retrieve the value of the specified 'aInt' without
        // a memory barrier.

    static void *getPtr(
      const bces_AtomicUtilImpl<bsls_Platform::CpuPowerpc>::Pointer& aPointer);
        // Atomically retrieve the value of the specified 'aPointer'.

    static void *getPtrRelaxed(
      const bces_AtomicUtilImpl<bsls_Platform::CpuPowerpc>::Pointer& aPointer);
        // Atomically retrieve the value of the specified 'aPointer' without
        // a memory barrier.

    static void setPtr(
             bces_AtomicUtilImpl<bsls_Platform::CpuPowerpc>::Pointer *aPointer,
             const void                                              *val);
        // Atomically set the value of the specified 'aPointer' to the
        // specified 'value'.  Note that the behavior is undefined if
        // 'aPointer' is 0.

    static void setPtrRelaxed(
             bces_AtomicUtilImpl<bsls_Platform::CpuPowerpc>::Pointer *aPointer,
             const void                                              *val);
        // Atomically set the value of the specified 'aPointer' to the
        // specified 'value' without a memory barrier.

    static void *swapPtr(
             bces_AtomicUtilImpl<bsls_Platform::CpuPowerpc>::Pointer *aPointer,
             const void                                              *val);
        // Atomically set the value of the specified 'aPointer' to the
        // specified 'value' and return its previous value.  Note that the
        // behavior is undefined if 'aPointer' is 0.

    static void *testAndSwapPtr(
             bces_AtomicUtilImpl<bsls_Platform::CpuPowerpc>::Pointer *aPointer,
             const void                                              *cmpVal,
             const void                                              *swapVal);
        // Conditionally set the value of the specified 'aPointer' to the
        // specified 'swapVal' and return its previous value.  The value of
        // 'aPointer' is compared to the specified 'cmpVal'; if they are equal,
        // then 'swapVal' will be assigned to 'aPointer' otherwise, the value
        // of 'aPointer' is left unchanged.  The whole operation is performed
        // atomically.  Note that the behavior is undefined if 'aPointer' is 0.

    static void spinLock(
              bces_AtomicUtilImpl<bsls_Platform::CpuPowerpc>::SpinLock *aSpin);
        // Lock the specified 'aSpin'.  Note that the behavior is undefined if
        // 'aSpin' is 0.

    static int spinTryLock(
            bces_AtomicUtilImpl<bsls_Platform::CpuPowerpc>::SpinLock *aSpin,
            int                                                       retries);
        // Attempt to lock the specified 'aSpin' up to 'retries' times.  Return
        // 0 on success, non-zero otherwise.  Note that the behavior is
        // undefined if 'aSpin' is 0 or 0 <= retries.

    static void spinUnlock(
           bces_AtomicUtilImpl<bsls_Platform::CpuPowerpc>::SpinLock *aSpin);
        // Unlock the specified 'aSpin' which was previously locked by a
        // successful call to 'spinLock' or 'spinTryLock'.  Note that the
        // behavior is undefined if 'aSpin' is 0.
};

inline
void bces_AtomicUtilImpl<bsls_Platform::CpuPowerpc>::initInt(
             bces_AtomicUtilImpl<bsls_Platform::CpuPowerpc>::Int *aInt,
             int                                                  initialValue)
{
    BSLS_ASSERT_OPT(bsls_AlignmentUtil::is4ByteAligned(aInt));
    aInt->d_value = initialValue;
}

inline
void bces_AtomicUtilImpl<bsls_Platform::CpuPowerpc>::initInt64(
           bces_AtomicUtilImpl<bsls_Platform::CpuPowerpc>::Int64 *aInt,
           bsls_PlatformUtil::Int64                               initialValue)
{
    BSLS_ASSERT_OPT(bsls_AlignmentUtil::is8ByteAligned(aInt));
    aInt->d_value = initialValue;
}

inline
void bces_AtomicUtilImpl<bsls_Platform::CpuPowerpc>::initPointer(
         bces_AtomicUtilImpl<bsls_Platform::CpuPowerpc>::Pointer *aPtr,
         const void                                              *initialValue)
{
    BSLS_ASSERT_OPT(
      0 == bsls_AlignmentUtil::calculateAlignmentOffset(aPtr, sizeof(void *)));
    aPtr->d_value = initialValue;
}

inline
void bces_AtomicUtilImpl<bsls_Platform::CpuPowerpc>::initSpinLock(
               bces_AtomicUtilImpl<bsls_Platform::CpuPowerpc>::SpinLock *aSpin)
{
    BSLS_ASSERT_OPT(bsls_AlignmentUtil::is4ByteAligned(aSpin));
    aSpin->d_lock = 0;
}

inline
void bces_AtomicUtilImpl<bsls_Platform::CpuPowerpc>::addInt(
                     bces_AtomicUtilImpl<bsls_Platform::CpuPowerpc>::Int *aInt,
                     int                                                  val)
{
    bces_AtomicUtilImpl_PowerpcAddInt(&aInt->d_value, val);
}

inline
void bces_AtomicUtilImpl<bsls_Platform::CpuPowerpc>::incrementInt(
                     bces_AtomicUtilImpl<bsls_Platform::CpuPowerpc>::Int *aInt)
{
    bces_AtomicUtilImpl_PowerpcAddInt(&aInt->d_value, 1);
}

inline
void bces_AtomicUtilImpl<bsls_Platform::CpuPowerpc>::decrementInt(
                     bces_AtomicUtilImpl<bsls_Platform::CpuPowerpc>::Int *aInt)
{
    bces_AtomicUtilImpl_PowerpcAddInt(&aInt->d_value, -1);
}

inline
int bces_AtomicUtilImpl<bsls_Platform::CpuPowerpc>::swapInt(
                     bces_AtomicUtilImpl<bsls_Platform::CpuPowerpc>::Int *aInt,
                     int                                                  val)
{
    return bces_AtomicUtilImpl_PowerpcSwapInt(&aInt->d_value, val);
}

inline
int bces_AtomicUtilImpl<bsls_Platform::CpuPowerpc>::testAndSwapInt(
                  bces_AtomicUtilImpl<bsls_Platform::CpuPowerpc>::Int *aInt,
                  int                                                  cmpVal,
                  int                                                  swapVal)
{
    return bces_AtomicUtilImpl_PowerpcTestAndSwap(&aInt->d_value,
                                                  cmpVal,
                                                  swapVal);
}

inline
int bces_AtomicUtilImpl<bsls_Platform::CpuPowerpc>::getInt(
               const bces_AtomicUtilImpl<bsls_Platform::CpuPowerpc>::Int& aInt)
{
    return bces_AtomicUtilImpl_PowerpcGetInt(&aInt.d_value);
}

inline
void bces_AtomicUtilImpl<bsls_Platform::CpuPowerpc>::setInt(
                     bces_AtomicUtilImpl<bsls_Platform::CpuPowerpc>::Int *aInt,
                     int                                                  val)
{
    bces_AtomicUtilImpl_PowerpcSetInt(&aInt->d_value, val);
}

inline
int bces_AtomicUtilImpl<bsls_Platform::CpuPowerpc>::getIntRelaxed(
               const bces_AtomicUtilImpl<bsls_Platform::CpuPowerpc>::Int& aInt)
{
    return aInt.d_value;
}

inline
void bces_AtomicUtilImpl<bsls_Platform::CpuPowerpc>::setIntRelaxed(
                     bces_AtomicUtilImpl<bsls_Platform::CpuPowerpc>::Int *aInt,
                     int                                                  val)
{
    aInt->d_value = val;
}

inline
int bces_AtomicUtilImpl<bsls_Platform::CpuPowerpc>::addIntNv(
                     bces_AtomicUtilImpl<bsls_Platform::CpuPowerpc>::Int *aInt,
                     int                                                  val)
{
    return bces_AtomicUtilImpl_PowerpcAddInt(&aInt->d_value, val);
}

inline
int bces_AtomicUtilImpl<bsls_Platform::CpuPowerpc>::addIntNvRelaxed(
                     bces_AtomicUtilImpl<bsls_Platform::CpuPowerpc>::Int *aInt,
                     int                                                  val)
{
    return bces_AtomicUtilImpl_PowerpcAddIntRelaxed(&aInt->d_value, val);
}

inline
int bces_AtomicUtilImpl<bsls_Platform::CpuPowerpc>::incrementIntNv(
                     bces_AtomicUtilImpl<bsls_Platform::CpuPowerpc>::Int *aInt)
{
    return bces_AtomicUtilImpl_PowerpcAddInt(&aInt->d_value, 1);
}

inline
int bces_AtomicUtilImpl<bsls_Platform::CpuPowerpc>::decrementIntNv(
                     bces_AtomicUtilImpl<bsls_Platform::CpuPowerpc>::Int *aInt)
{
    return bces_AtomicUtilImpl_PowerpcAddInt(&aInt->d_value, -1);
}

inline
void bces_AtomicUtilImpl<bsls_Platform::CpuPowerpc>::addInt64(
                   bces_AtomicUtilImpl<bsls_Platform::CpuPowerpc>::Int64 *aInt,
                   bsls_PlatformUtil::Int64                               val)
{
    bces_AtomicUtilImpl_PowerpcAddInt64(&aInt->d_value, val);
}

inline
void bces_AtomicUtilImpl<bsls_Platform::CpuPowerpc>::incrementInt64(
                   bces_AtomicUtilImpl<bsls_Platform::CpuPowerpc>::Int64 *aInt)
{
    bces_AtomicUtilImpl_PowerpcAddInt64(&aInt->d_value, 1);
}

inline
void bces_AtomicUtilImpl<bsls_Platform::CpuPowerpc>::decrementInt64(
                   bces_AtomicUtilImpl<bsls_Platform::CpuPowerpc>::Int64 *aInt)
{
    bces_AtomicUtilImpl_PowerpcAddInt64(&aInt->d_value, -1);
}

inline
bsls_PlatformUtil::Int64
bces_AtomicUtilImpl<bsls_Platform::CpuPowerpc>::swapInt64(
                   bces_AtomicUtilImpl<bsls_Platform::CpuPowerpc>::Int64 *aInt,
                   bsls_PlatformUtil::Int64                               val)
{
    return bces_AtomicUtilImpl_PowerpcSwapInt64(&aInt->d_value, val);
}

inline
bsls_PlatformUtil::Int64
bces_AtomicUtilImpl<bsls_Platform::CpuPowerpc>::testAndSwapInt64(
                                             Int64                    *aInt,
                                             bsls_PlatformUtil::Int64  cmpVal,
                                             bsls_PlatformUtil::Int64  swapVal)
{
    return bces_AtomicUtilImpl_PowerpcTestAndSwap64(&aInt->d_value,
                                                    cmpVal,
                                                    swapVal);
}

inline
bsls_PlatformUtil::Int64
bces_AtomicUtilImpl<bsls_Platform::CpuPowerpc>::addInt64Nv(
                   bces_AtomicUtilImpl<bsls_Platform::CpuPowerpc>::Int64 *aInt,
                   bsls_PlatformUtil::Int64                               val)
{
    return bces_AtomicUtilImpl_PowerpcAddInt64(&aInt->d_value, val);
}

inline
bsls_PlatformUtil::Int64
bces_AtomicUtilImpl<bsls_Platform::CpuPowerpc>::addInt64NvRelaxed(
                   bces_AtomicUtilImpl<bsls_Platform::CpuPowerpc>::Int64 *aInt,
                   bsls_PlatformUtil::Int64                               val)
{
    return bces_AtomicUtilImpl_PowerpcAddInt64Relaxed(&aInt->d_value, val);
}

inline
bsls_PlatformUtil::Int64
bces_AtomicUtilImpl<bsls_Platform::CpuPowerpc>::incrementInt64Nv(
                   bces_AtomicUtilImpl<bsls_Platform::CpuPowerpc>::Int64 *aInt)
{
    return bces_AtomicUtilImpl_PowerpcAddInt64(&aInt->d_value, 1);
}

inline
bsls_PlatformUtil::Int64
bces_AtomicUtilImpl<bsls_Platform::CpuPowerpc>::decrementInt64Nv(
                   bces_AtomicUtilImpl<bsls_Platform::CpuPowerpc>::Int64 *aInt)
{
    return bces_AtomicUtilImpl_PowerpcAddInt64(&aInt->d_value, -1);
}

inline
void bces_AtomicUtilImpl<bsls_Platform::CpuPowerpc>::setInt64(
                   bces_AtomicUtilImpl<bsls_Platform::CpuPowerpc>::Int64 *aInt,
                   bsls_PlatformUtil::Int64                               val)
{
    bces_AtomicUtilImpl_PowerpcSetInt64(&aInt->d_value, val);
}

inline
void bces_AtomicUtilImpl<bsls_Platform::CpuPowerpc>::setInt64Relaxed(
                   bces_AtomicUtilImpl<bsls_Platform::CpuPowerpc>::Int64 *aInt,
                   bsls_PlatformUtil::Int64                               val)
{
#ifdef BSLS_PLATFORM__CPU_64_BIT
    aInt->d_value = val;
#else
    bces_AtomicUtilImpl_PowerpcSetInt64Relaxed(&aInt->d_value, val);
#endif
}

inline
bsls_PlatformUtil::Int64
bces_AtomicUtilImpl<bsls_Platform::CpuPowerpc>::getInt64(
             const bces_AtomicUtilImpl<bsls_Platform::CpuPowerpc>::Int64& aInt)
{
    return bces_AtomicUtilImpl_PowerpcGetInt64(&aInt.d_value);
}

inline
void bces_AtomicUtilImpl<bsls_Platform::CpuPowerpc>::setPtr(
             bces_AtomicUtilImpl<bsls_Platform::CpuPowerpc>::Pointer *aPointer,
             const void                                               *val)
{
#ifdef BSLS_PLATFORM__CPU_64_BIT
    bces_AtomicUtilImpl_PowerpcSetInt64(
                                (bsls_PlatformUtil::Int64 *)&aPointer->d_value,
                                (bsls_PlatformUtil::Int64)val);
#else
    bces_AtomicUtilImpl_PowerpcSetInt((int *)&aPointer->d_value, (int)val);
#endif
}

inline
void bces_AtomicUtilImpl<bsls_Platform::CpuPowerpc>::setPtrRelaxed(
             bces_AtomicUtilImpl<bsls_Platform::CpuPowerpc>::Pointer *aPointer,
             const void                                              *val)
{
    aPointer->d_value = val;
}

inline
void *bces_AtomicUtilImpl<bsls_Platform::CpuPowerpc>::swapPtr(
             bces_AtomicUtilImpl<bsls_Platform::CpuPowerpc>::Pointer *aPointer,
             const void                                              *val)
{
#ifdef BSLS_PLATFORM__CPU_64_BIT
    return (void *)bces_AtomicUtilImpl_PowerpcSwapInt64(
                       (volatile bsls_PlatformUtil::Int64 *)&aPointer->d_value,
                       (bsls_PlatformUtil::Int64)val);
#else
    return (void *)bces_AtomicUtilImpl_PowerpcSwapInt(
                                            (volatile int *)&aPointer->d_value,
                                            (int)val);
#endif
}

inline
void *bces_AtomicUtilImpl<bsls_Platform::CpuPowerpc>::testAndSwapPtr(
             bces_AtomicUtilImpl<bsls_Platform::CpuPowerpc>::Pointer *aPointer,
             const void                                              *cmpVal,
             const void                                              *swapVal)
{
#ifdef BSLS_PLATFORM__CPU_64_BIT
    return (void *)bces_AtomicUtilImpl_PowerpcTestAndSwap64(
                       (volatile bsls_PlatformUtil::Int64 *)&aPointer->d_value,
                       (bsls_PlatformUtil::Int64)cmpVal,
                       (bsls_PlatformUtil::Int64)swapVal);
#else
    return (void *)bces_AtomicUtilImpl_PowerpcTestAndSwap(
                                            (volatile int *)&aPointer->d_value,
                                            (int)cmpVal,
                                            (int)swapVal);
#endif
}

inline
bsls_PlatformUtil::Int64
bces_AtomicUtilImpl<bsls_Platform::CpuPowerpc>::getInt64Relaxed(
         const bces_AtomicUtilImpl<bsls_Platform::CpuPowerpc>::Int64& aPointer)
{
#ifdef BSLS_PLATFORM__CPU_64_BIT
    return aPointer.d_value;
#else
    return bces_AtomicUtilImpl_PowerpcGetInt64Relaxed(&aPointer.d_value);
#endif
}

inline
void *bces_AtomicUtilImpl<bsls_Platform::CpuPowerpc>::getPtr(
       const bces_AtomicUtilImpl<bsls_Platform::CpuPowerpc>::Pointer& aPointer)
{
#ifdef BSLS_PLATFORM__CPU_64_BIT
    return (void *)bces_AtomicUtilImpl_PowerpcGetInt64
                      ((volatile bsls_PlatformUtil::Int64 *)&aPointer.d_value);
#else
    return (void *)bces_AtomicUtilImpl_PowerpcGetInt(
                                            (volatile int *)&aPointer.d_value);
#endif
}

inline
void *bces_AtomicUtilImpl<bsls_Platform::CpuPowerpc>::getPtrRelaxed(
       const bces_AtomicUtilImpl<bsls_Platform::CpuPowerpc>::Pointer& aPointer)
{
    return const_cast<void *>(aPointer.d_value);
}

inline
void bces_AtomicUtilImpl<bsls_Platform::CpuPowerpc>::spinLock(
               bces_AtomicUtilImpl<bsls_Platform::CpuPowerpc>::SpinLock *aSpin)
{
    bces_AtomicUtilImpl_PowerpcSpinLock(&aSpin->d_lock);
}

inline
int bces_AtomicUtilImpl<bsls_Platform::CpuPowerpc>::spinTryLock(
             bces_AtomicUtilImpl<bsls_Platform::CpuPowerpc>::SpinLock *aSpin,
             int                                                       retries)
{
    return bces_AtomicUtilImpl_PowerpcSpinTryLock(&aSpin->d_lock, retries);
}

inline
void bces_AtomicUtilImpl<bsls_Platform::CpuPowerpc>::spinUnlock(
               bces_AtomicUtilImpl<bsls_Platform::CpuPowerpc>::SpinLock *aSpin)
{
     bces_AtomicUtilImpl_PowerpcSpinUnlock(&aSpin->d_lock);
}

}  // close namespace BloombergLP

#endif

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2004
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
