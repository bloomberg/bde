// bces_atomicutilimpl_sparc32.h   -*-C++-*-
#ifndef INCLUDED_BCES_ATOMICUTILIMPL_SPARC32
#define INCLUDED_BCES_ATOMICUTILIMPL_SPARC32

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide atomic operations on the 32 bit architectures.
//
//@CLASSES:
//   bces_AtomicUtilImpl<bsls_Platform::CpuSparc_32>: Namespace for atomic
//   operations on 32 bit sparc architectures.
//
//@AUTHOR: Ilougino Rocha (irocha)
//
//@DESCRIPTION: This component provides a full specialization of
// 'bces_AtomicUtilImpl' for 32-bit sparc architectures.  The component is
// not intended to be used directly.  See the 'bces_AtomicUtil' for more on
// using atomic operations.
//
///Compatibility
///-------------
// These atomic operations are designed to be run on a CPU and OS supporting
// the SPARC V8+ architectures or later running in the TSO memory model.
// Earlier architectures and other memory models are *NOT* supported.
// Attempting to use this component on these systems will result in undefined
// runtime behavior.

#ifndef INCLUDED_BCESCM_VERSION
#include <bcescm_version.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSLS_ALIGNMENTUTIL
#include <bsls_alignmentutil.h>
#endif

#ifndef INCLUDED_BSLS_PLATFORM
#include <bsls_platform.h>
#endif

#ifndef INCLUDED_BSLS_PLATFORMUTIL
#include <bsls_platformutil.h>
#endif

#ifdef BSLS_PLATFORM__CPU_SPARC_32

namespace BloombergLP {

extern "C" {

#if defined(BSLS_PLATFORM__CMP_GNU)

// Disabled because of DRQS 15628785
#if 0

  || (   defined(BSLS_PLATFORM__CMP_SUN) \
      && BSLS_PLATFORM__CMP_VER_MAJOR >= 0x590) \
      && defined(BDE_BUILD_TARGET_OPT)
#endif
// Studio 12+ optimized or gcc
inline
void bces_AtomicUtilImpl_Sparc32SetInt(volatile int  *aInt,
                                       int            val)
{
    asm volatile("st %1, %0\n"
                 "membar #StoreLoad\n"
               : "=m"(*aInt)
               : "r"(val)
               : "memory");
}

inline
int bces_AtomicUtilImpl_Sparc32SwapInt(volatile int  *aInt,
                                       int            val)
{
    // This is for a temp register.  Initialization is un-necessary but
    // Studio 12 emits a warning if not there.

    int dummy = 0;
    int oldVal = *aInt;
    asm volatile("1:\n"
                 "mov %3, %1\n"
                 "cas [%2], %0, %1\n"
                 "cmp %0, %1\n"
                 "bne,a,pn %%icc,1b\n"
                 "mov %1,%0\n"
                 "membar #StoreLoad\n"
               : "+r" (oldVal), "=&r" (dummy)
               : "r"(aInt), "r" (val)
               : "cc", "memory");
    return oldVal;
}

inline
int bces_AtomicUtilImpl_Sparc32GetInt(const volatile int *aInt)
{
    // Initialization is unnecessary; this is a workaround for Studio 12 giving
    // a warning.

    int ret = 0;
    asm volatile ("membar #StoreLoad\n"
                  "ld %1, %0\n"
                  : "=r"(ret)
                  : "m"(*aInt)
                  : "memory");
    return ret;
}

inline
int bces_AtomicUtilImpl_Sparc32AddInt(volatile int *aInt,
                                      int            val)
{
    // This is for a temp register.  Initialization is un-necessary but
    // Studio 12 emits a warning if not there.

    int dummy = 0;
    int oldVal = *aInt;

    asm volatile("1:\n"
                 "add %0, %3, %1\n"
                 "cas [%2], %0, %1\n"
                 "cmp %0, %1\n"
                 "bne,a,pn %%icc,1b\n"
                 "mov %1,%0\n"
                 "add %1, %3, %0\n"
                 "membar #StoreLoad\n"
               : "+r" (oldVal), "=&r" (dummy)
               : "r"(aInt), "r" (val)
               : "cc", "memory");
    return oldVal;
}

inline
int bces_AtomicUtilImpl_Sparc32AddIntRelaxed(volatile int  *aInt,
                                             int            val)
{
    // This is for a temp register.  Initialization is un-necessary but
    // Studio 12 emits a warning if not there.

    int dummy = 0;
    int oldVal = *aInt;

    asm ("1:\n"
         "add %0, %4, %1\n"
         "cas [%3], %0, %1\n"
         "cmp %0, %1\n"
         "bne,a,pn %%icc,1b\n"
         "mov %1,%0\n"
         "add %1, %4, %0\n"
         : "+r" (oldVal), "=&r" (dummy), "+m"(*aInt) //unused, indicates change
         : "r"(aInt), "r" (val)
         : "cc");
    return oldVal;
}

inline
int bces_AtomicUtilImpl_Sparc32TestAndSwap(volatile int *aInt,
                                           int            cmpVal,
                                           int            swapVal)
{
    asm volatile("cas [%1], %2, %0\n"
                 "membar #StoreLoad\n"
                : "+r" (swapVal)
                : "r" (aInt), "r" (cmpVal)
                : "memory");
    return swapVal;
}

#else
void bces_AtomicUtilImpl_Sparc32SetInt(volatile int *aInt, int val);

int bces_AtomicUtilImpl_Sparc32SwapInt(volatile int *aInt, int val);

int bces_AtomicUtilImpl_Sparc32GetInt(const volatile int*);

int bces_AtomicUtilImpl_Sparc32AddIntRelaxed(volatile int *aInt, int val);

int bces_AtomicUtilImpl_Sparc32AddInt(volatile int *aInt, int val);

int bces_AtomicUtilImpl_Sparc32TestAndSwap(volatile int *aInt,
                                           int            cmpVal,
                                           int            swapVal);
#endif

void  bces_AtomicUtilImpl_Sparc32SetInt64(
                                      volatile bsls_PlatformUtil::Int64 *aInt,
                                      bsls_PlatformUtil::Int64      val);

void  bces_AtomicUtilImpl_Sparc32SetInt64Relaxed(
                                      volatile bsls_PlatformUtil::Int64 *aInt,
                                      bsls_PlatformUtil::Int64      val);

bsls_PlatformUtil::Int64  bces_AtomicUtilImpl_Sparc32SwapInt64(
                                      volatile bsls_PlatformUtil::Int64 *aInt,
                                      bsls_PlatformUtil::Int64      val);

bsls_PlatformUtil::Int64 bces_AtomicUtilImpl_Sparc32GetInt64(
                                  const volatile bsls_PlatformUtil::Int64 *);

bsls_PlatformUtil::Int64 bces_AtomicUtilImpl_Sparc32GetInt64Relaxed(
                                  const volatile bsls_PlatformUtil::Int64 *);

bsls_PlatformUtil::Int64 bces_AtomicUtilImpl_Sparc32AddInt64(
                                      volatile bsls_PlatformUtil::Int64 *aInt,
                                      bsls_PlatformUtil::Int64      val);

bsls_PlatformUtil::Int64 bces_AtomicUtilImpl_Sparc32AddInt64Relaxed(
                                      volatile bsls_PlatformUtil::Int64 *aInt,
                                      bsls_PlatformUtil::Int64      val);

bsls_PlatformUtil::Int64 bces_AtomicUtilImpl_Sparc32TestAndSwap64(
                                      volatile bsls_PlatformUtil::Int64 *aInt,
                                      bsls_PlatformUtil::Int64      cmpVal,
                                      bsls_PlatformUtil::Int64      swapVal);

void bces_AtomicUtilImpl_Sparc32SpinLock(volatile int *aSpin);
int  bces_AtomicUtilImpl_Sparc32SpinTryLock(volatile int *aSpin,
                                            int           retries );
void bces_AtomicUtilImpl_Sparc32SpinUnlock(volatile int *aSpin);

}

template <typename PLATFORM> struct bces_AtomicUtilImpl;

template <>
struct bces_AtomicUtilImpl<bsls_Platform::CpuSparc_32> {
    // Namespace for atomic operations on 32-bit Sparc architectures.

    // TYPES
    struct Int {
#ifdef BSLS_PLATFORM__CMP_GNU
        volatile int d_value __attribute__((__aligned__(sizeof(int))));
#else
#ifdef BSLS_PLATFORM__CMP_SUN
        #pragma align 4 (d_value)
#endif
        volatile int d_value;
#endif
    };

    struct Int64 {
#ifdef BSLS_PLATFORM__CMP_GNU
        volatile bsls_PlatformUtil::Int64 d_value
                __attribute__((__aligned__(sizeof(bsls_PlatformUtil::Int64))));
#else
#ifdef BSLS_PLATFORM__CMP_SUN
        #pragma align 8 (d_value)
#endif
        volatile bsls_PlatformUtil::Int64 d_value;
#endif
    };

    struct Pointer {
#ifdef BSLS_PLATFORM__CMP_SUN
        #pragma align 4 (d_value)
#endif
        const void * volatile d_value
#ifdef BSLS_PLATFORM__CMP_GNU
                                   __attribute__((__aligned__(sizeof(void *))))
#endif
               ;
    };

    struct SpinLock {
#ifdef BSLS_PLATFORM__CMP_GNU
        volatile int d_lock __attribute__((__aligned__(sizeof(int))));
#else
#ifdef BSLS_PLATFORM__CMP_SUN
        #pragma align 4 (d_lock)
#endif
        volatile int d_lock;
#endif
    };

    static void initInt(
           bces_AtomicUtilImpl<bsls_Platform::CpuSparc_32>::Int *aInt,
           int                                                   initialValue);
        // Initialize the specified 'aInt' to 0. Note that this method
        // must be called before any other operations on 'aInt'.

    static void initInt64(
                bces_AtomicUtilImpl<bsls_Platform::CpuSparc_32>::Int64 *aInt,
                bsls_PlatformUtil::Int64                                 val);
        // Initialize the specified 'aInt' to a 0 value.  Note that
        // this method should before any other operations on 'aInt'.

    static void initPointer(
              bces_AtomicUtilImpl<bsls_Platform::CpuSparc_32>::Pointer *aPtr,
              const void                                       *initialValue);
        // Initialize the specified 'aPtr' to a NULL value.  Note that
        // this method must be called before any other operations on 'aPtr'.

    static void initSpinLock(
           bces_AtomicUtilImpl<bsls_Platform::CpuSparc_32>::SpinLock *aSpin);
        // Initialize the specified 'aSpin' to an unlocked state.
        // Note that method must be called before any other operations
        // on 'aSpin'.

    static void addInt(
                 bces_AtomicUtilImpl<bsls_Platform::CpuSparc_32>::Int *aInt,
                 int val);
        // Atomically add 'val' to the atomic integer specified in 'aInt'.

    static void incrementInt(
                  bces_AtomicUtilImpl<bsls_Platform::CpuSparc_32>::Int *aInt);
        // Atomically increment the atomic integer specified in 'aInt'.

    static void decrementInt(
                  bces_AtomicUtilImpl<bsls_Platform::CpuSparc_32>::Int *aInt);
        // Atomically decrement the atomic integer specified in 'aInt'.

    static int swapInt(
                  bces_AtomicUtilImpl<bsls_Platform::CpuSparc_32>::Int *aInt,
                  int val);
        // Atomically assign the value 'val' to the atomic integer
        // specified in 'aInt' and return its previous value.

    static int testAndSwapInt(
                   bces_AtomicUtilImpl<bsls_Platform::CpuSparc_32>::Int *aInt,
                   int cmpVal, int swapVal);

    static void setInt(
                  bces_AtomicUtilImpl<bsls_Platform::CpuSparc_32>::Int *aInt,
                  int val);
        // Atomically assign the value 'val' to the atomic integer
        // specified in 'aInt'.

    static void setIntRelaxed(
                  bces_AtomicUtilImpl<bsls_Platform::CpuSparc_32>::Int *aInt,
                  int val);
        // Atomically assign the value 'val' to the atomic integer
        // specified in 'aInt', without a memory barrier.

    static int getInt(
            const bces_AtomicUtilImpl<bsls_Platform::CpuSparc_32>::Int& aInt);
        // Atomically get the atomic integer specified in 'aInt'.

    static int getIntRelaxed(
            const bces_AtomicUtilImpl<bsls_Platform::CpuSparc_32>::Int& aInt);
        // Atomically get the atomic integer specified in 'aInt', without a
        // memory barrier.

    static int addIntNv(
                  bces_AtomicUtilImpl<bsls_Platform::CpuSparc_32>::Int *aInt,
                  int val);
        // Atomically add 'val' to the atomic integer specified in 'aInt'
        // and return the resulting value.

    static int addIntNvRelaxed(
                  bces_AtomicUtilImpl<bsls_Platform::CpuSparc_32>::Int *aInt,
                  int val);
        // Atomically add 'val' to the atomic integer specified in 'aInt'
        // and return the resulting value, without a memory barrier.

    static int incrementIntNv(
                  bces_AtomicUtilImpl<bsls_Platform::CpuSparc_32>::Int *aInt);
        // Atomically increment the atomic integer specified in 'aInt'
        // and return the resulting value.

    static int decrementIntNv(
                  bces_AtomicUtilImpl<bsls_Platform::CpuSparc_32>::Int *aInt);
        // Atomically decrement the atomic integer specified in 'aInt'
        // and return the resulting value.

    static void addInt64(
                 bces_AtomicUtilImpl<bsls_Platform::CpuSparc_32>::Int64 *aInt,
                 bsls_PlatformUtil::Int64 val);
        // Atomically add 'val' to the 64 bit atomic integer specified in
        // 'aInt'.

    static void incrementInt64(
                bces_AtomicUtilImpl<bsls_Platform::CpuSparc_32>::Int64 *aInt);
        // Atomically increment the 64 bit atomic integer specified in 'aInt'.

    static void decrementInt64(
                bces_AtomicUtilImpl<bsls_Platform::CpuSparc_32>::Int64 *aInt);
        // Atomically decrement the 64 bit atomic integer specified in 'aInt'.

    static bsls_PlatformUtil::Int64 swapInt64(
                bces_AtomicUtilImpl<bsls_Platform::CpuSparc_32>::Int64 *aInt,
                bsls_PlatformUtil::Int64 val);
        // Atomically assign 'val' to the 64 bit atomic integer
        // specified in 'aInt'.

    static bsls_PlatformUtil::Int64 testAndSwapInt64(
                bces_AtomicUtilImpl<bsls_Platform::CpuSparc_32>::Int64 *aInt,
                                             bsls_PlatformUtil::Int64  cmpVal,
                                             bsls_PlatformUtil::Int64 swapVal);
        // Atomically decrement the 64 bit atomic integer specified in 'aInt'.

    static bsls_PlatformUtil::Int64 addInt64Nv(
                bces_AtomicUtilImpl<bsls_Platform::CpuSparc_32>::Int64 *aInt,
                                               bsls_PlatformUtil::Int64 val);
        // Atomically add 'val' to the 64 bit atomic integer specified in
        // 'aInt' and return the resulting value.

    static bsls_PlatformUtil::Int64 addInt64NvRelaxed(
                bces_AtomicUtilImpl<bsls_Platform::CpuSparc_32>::Int64 *aInt,
                                               bsls_PlatformUtil::Int64 val);
        // Atomically add 'val' to the 64 bit atomic integer specified in
        // 'aInt' and return the resulting value, without a memory barrier.

    static bsls_PlatformUtil::Int64 incrementInt64Nv(
                bces_AtomicUtilImpl<bsls_Platform::CpuSparc_32>::Int64 *aInt);
        // Atomically increment the 64 bit atomic integer specified in 'aInt'.
        // and return the resulting value.

    static bsls_PlatformUtil::Int64 decrementInt64Nv(
                bces_AtomicUtilImpl<bsls_Platform::CpuSparc_32>::Int64 *aInt);
        // Atomically decrement the 64 bit atomic integer specified in 'aInt'.
        // and return the resulting value.

    static void setInt64(
                bces_AtomicUtilImpl<bsls_Platform::CpuSparc_32>::Int64 *aInt,
                bsls_PlatformUtil::Int64 val);
        // Atomically assign 'val' to the 64 bit atomic integer
        // specified in 'aInt'.

    static void setInt64Relaxed(
                bces_AtomicUtilImpl<bsls_Platform::CpuSparc_32>::Int64 *aInt,
                bsls_PlatformUtil::Int64 val);
        // Atomically assign 'val' to the 64 bit atomic integer specified in
        // 'aInt', without a memory barrier.

    static bsls_PlatformUtil::Int64 getInt64(
          const bces_AtomicUtilImpl<bsls_Platform::CpuSparc_32>::Int64& aInt);
        // Atomically retrieve the value of the specified 'aInt'.

    static bsls_PlatformUtil::Int64 getInt64Relaxed(
          const bces_AtomicUtilImpl<bsls_Platform::CpuSparc_32>::Int64& aInt);
        // Atomically retrieve the value of the specified 'aInt' without a
        // memory barrier.

    static void *getPtr(
     const bces_AtomicUtilImpl<bsls_Platform::CpuSparc_32>::Pointer& aPointer);
        // Atomically retrieve the value of the specified 'aPointer'.

    static void *getPtrRelaxed(
     const bces_AtomicUtilImpl<bsls_Platform::CpuSparc_32>::Pointer& aPointer);
        // Atomically retrieve the value of the specified 'aPointer' without
        // a memory barrier.

    static void setPtr(
           bces_AtomicUtilImpl<bsls_Platform::CpuSparc_32>::Pointer *aPointer,
           const void                                               *val);
        // Atomically set the value of the specified 'aPointer' to 'val'.

    static void setPtrRelaxed(
           bces_AtomicUtilImpl<bsls_Platform::CpuSparc_32>::Pointer *aPointer,
           const void                                               *val);
        // Atomically set the value of the specified 'aPointer' to 'val'
        // without a memory barrier.

    static void *swapPtr(
           bces_AtomicUtilImpl<bsls_Platform::CpuSparc_32>::Pointer *aPointer,
           const volatile void                                      *val);
        // Atomically set the value of the specified 'aPointer' to 'val' and
        // return its previous value.

    static void *testAndSwapPtr(
           bces_AtomicUtilImpl<bsls_Platform::CpuSparc_32>::Pointer *aPointer,
           const volatile void                                      *cmpVal,
           const volatile void                                      *swapVal);

    static void spinLock(
           bces_AtomicUtilImpl<bsls_Platform::CpuSparc_32>::SpinLock *aSpin);
        // Attempt to lock the specified 'aSpin'.  Return 0 on success,
        // non-zero otherwise.

    static int spinTryLock(
           bces_AtomicUtilImpl<bsls_Platform::CpuSparc_32>::SpinLock *aSpin,
           int retries  );
        // Attempt to lock the specified 'aSpin' up to 'retries' times.
        // Return 0 on success, non-zero otherwise.

    static void spinUnlock(
           bces_AtomicUtilImpl<bsls_Platform::CpuSparc_32>::SpinLock *aSpin);
        // Unlock the specified 'aSpin' which was previously locked by
        // a successful call to 'spinLock' or 'spinTryLock'.   Return 0
        // on success, non-zero otherwise.
};

inline
void bces_AtomicUtilImpl<bsls_Platform::CpuSparc_32>::initInt(
            bces_AtomicUtilImpl<bsls_Platform::CpuSparc_32>::Int *aInt,
            int                                                   initialValue)
{
    // NOT NEEDED FOR THE NEWER (v8plus and later) ARCHITECTURES
    // aInt->d_lock = 0;
    BSLS_ASSERT_OPT(bsls_AlignmentUtil::is4ByteAligned(aInt));
    aInt->d_value = initialValue;
}

inline
void bces_AtomicUtilImpl<bsls_Platform::CpuSparc_32>::initInt64(
          bces_AtomicUtilImpl<bsls_Platform::CpuSparc_32>::Int64 *aInt,
          bsls_PlatformUtil::Int64                                initialValue)
{
    BSLS_ASSERT_OPT(bsls_AlignmentUtil::is8ByteAligned(aInt));
    aInt->d_value = initialValue;
}

inline
void bces_AtomicUtilImpl<bsls_Platform::CpuSparc_32>::initPointer(
        bces_AtomicUtilImpl<bsls_Platform::CpuSparc_32>::Pointer *aPtr,
        const void                                               *initialValue)
{
    // NOT NEEDED FOR THE NEWER (v8plus and later) ARCHITECTURES
    // aPtr->d_lock = 0;
    BSLS_ASSERT_OPT(bsls_AlignmentUtil::is4ByteAligned(aPtr));
    aPtr->d_value = initialValue;
}

inline
void bces_AtomicUtilImpl<bsls_Platform::CpuSparc_32>::initSpinLock(
              bces_AtomicUtilImpl<bsls_Platform::CpuSparc_32>::SpinLock *aSpin)
{
    BSLS_ASSERT_OPT(bsls_AlignmentUtil::is4ByteAligned(aSpin));
    aSpin->d_lock = 0;
}

inline
void bces_AtomicUtilImpl<bsls_Platform::CpuSparc_32>::addInt(
          bces_AtomicUtilImpl<bsls_Platform::CpuSparc_32>::Int *aInt,
          int val)
{
    bces_AtomicUtilImpl_Sparc32AddInt(&aInt->d_value, val);
}

inline
void bces_AtomicUtilImpl<bsls_Platform::CpuSparc_32>::incrementInt(
                    bces_AtomicUtilImpl<bsls_Platform::CpuSparc_32>::Int *aInt)
{
    bces_AtomicUtilImpl_Sparc32AddInt(&aInt->d_value, 1);
}

inline
void bces_AtomicUtilImpl<bsls_Platform::CpuSparc_32>::decrementInt(
                    bces_AtomicUtilImpl<bsls_Platform::CpuSparc_32>::Int *aInt)
{
    bces_AtomicUtilImpl_Sparc32AddInt(&aInt->d_value, -1);
}

inline
int bces_AtomicUtilImpl<bsls_Platform::CpuSparc_32>::swapInt(
                   bces_AtomicUtilImpl<bsls_Platform::CpuSparc_32>::Int *aInt,
                   int                                                   val)
{
    return bces_AtomicUtilImpl_Sparc32SwapInt(&aInt->d_value, val);
}

inline
int bces_AtomicUtilImpl<bsls_Platform::CpuSparc_32>::testAndSwapInt(
                 bces_AtomicUtilImpl<bsls_Platform::CpuSparc_32>::Int *aInt,
                 int                                                   cmpVal,
                 int                                                   swapVal)
{
    return bces_AtomicUtilImpl_Sparc32TestAndSwap(&aInt->d_value,
                                                  cmpVal,
                                                  swapVal);
}

inline
void bces_AtomicUtilImpl<bsls_Platform::CpuSparc_32>::setInt(
                   bces_AtomicUtilImpl<bsls_Platform::CpuSparc_32>::Int *aInt,
                   int                                                   val)
{
    bces_AtomicUtilImpl_Sparc32SetInt((int *)aInt, val);
}

inline
void bces_AtomicUtilImpl<bsls_Platform::CpuSparc_32>::setIntRelaxed(
                   bces_AtomicUtilImpl<bsls_Platform::CpuSparc_32>::Int *aInt,
                   int                                                   val)
{
    aInt->d_value = val;
}

inline
int  bces_AtomicUtilImpl<bsls_Platform::CpuSparc_32>::getInt(
              const bces_AtomicUtilImpl<bsls_Platform::CpuSparc_32>::Int& aInt)
{
    return bces_AtomicUtilImpl_Sparc32GetInt(&aInt.d_value);
}

inline
int  bces_AtomicUtilImpl<bsls_Platform::CpuSparc_32>::getIntRelaxed(
              const bces_AtomicUtilImpl<bsls_Platform::CpuSparc_32>::Int& aInt)
{
    return aInt.d_value;
}

inline
int bces_AtomicUtilImpl<bsls_Platform::CpuSparc_32>::addIntNv(
                   bces_AtomicUtilImpl<bsls_Platform::CpuSparc_32>::Int *aInt,
                   int                                                   val)
{
    return bces_AtomicUtilImpl_Sparc32AddInt(&aInt->d_value, val);
}

inline
int bces_AtomicUtilImpl<bsls_Platform::CpuSparc_32>::addIntNvRelaxed(
                   bces_AtomicUtilImpl<bsls_Platform::CpuSparc_32>::Int *aInt,
                   int                                                   val)
{
    return bces_AtomicUtilImpl_Sparc32AddIntRelaxed(&aInt->d_value, val);
}

inline
int bces_AtomicUtilImpl<bsls_Platform::CpuSparc_32>::incrementIntNv(
                    bces_AtomicUtilImpl<bsls_Platform::CpuSparc_32>::Int *aInt)
{
    return bces_AtomicUtilImpl_Sparc32AddInt(&aInt->d_value, 1);
}

inline
int bces_AtomicUtilImpl<bsls_Platform::CpuSparc_32>::decrementIntNv(
                    bces_AtomicUtilImpl<bsls_Platform::CpuSparc_32>::Int *aInt)
{
    return bces_AtomicUtilImpl_Sparc32AddInt(&aInt->d_value, -1);
}

inline
void bces_AtomicUtilImpl<bsls_Platform::CpuSparc_32>::addInt64(
                  bces_AtomicUtilImpl<bsls_Platform::CpuSparc_32>::Int64 *aInt,
                  bsls_PlatformUtil::Int64                                val)
{
    bces_AtomicUtilImpl_Sparc32AddInt64(&aInt->d_value, val);
}

inline
void bces_AtomicUtilImpl<bsls_Platform::CpuSparc_32>::incrementInt64(
                  bces_AtomicUtilImpl<bsls_Platform::CpuSparc_32>::Int64 *aInt)
{
    bces_AtomicUtilImpl_Sparc32AddInt64(&aInt->d_value, 1);
}

inline
void bces_AtomicUtilImpl<bsls_Platform::CpuSparc_32>::decrementInt64(
                  bces_AtomicUtilImpl<bsls_Platform::CpuSparc_32>::Int64 *aInt)
{
    bces_AtomicUtilImpl_Sparc32AddInt64(&aInt->d_value, -1);
}

inline
bsls_PlatformUtil::Int64
bces_AtomicUtilImpl<bsls_Platform::CpuSparc_32>::swapInt64(
                  bces_AtomicUtilImpl<bsls_Platform::CpuSparc_32>::Int64 *aInt,
                  bsls_PlatformUtil::Int64                                val)
{
    return bces_AtomicUtilImpl_Sparc32SwapInt64(&aInt->d_value, val);
}

inline
bsls_PlatformUtil::Int64
bces_AtomicUtilImpl<bsls_Platform::CpuSparc_32>::testAndSwapInt64(
                                             Int64                   *aInt,
                                             bsls_PlatformUtil::Int64  cmpVal,
                                             bsls_PlatformUtil::Int64  swapVal)
{
    return bces_AtomicUtilImpl_Sparc32TestAndSwap64(&aInt->d_value,
                                                    cmpVal,swapVal);
}

inline
bsls_PlatformUtil::Int64
  bces_AtomicUtilImpl<bsls_Platform::CpuSparc_32>::addInt64Nv(
                  bces_AtomicUtilImpl<bsls_Platform::CpuSparc_32>::Int64 *aInt,
                  bsls_PlatformUtil::Int64                                val)
{
    return bces_AtomicUtilImpl_Sparc32AddInt64(&aInt->d_value, val);
}

inline
bsls_PlatformUtil::Int64
  bces_AtomicUtilImpl<bsls_Platform::CpuSparc_32>::addInt64NvRelaxed(
                  bces_AtomicUtilImpl<bsls_Platform::CpuSparc_32>::Int64 *aInt,
                  bsls_PlatformUtil::Int64                                val)
{
    return bces_AtomicUtilImpl_Sparc32AddInt64Relaxed(&aInt->d_value, val);
}

inline
bsls_PlatformUtil::Int64
bces_AtomicUtilImpl<bsls_Platform::CpuSparc_32>::incrementInt64Nv(
                 bces_AtomicUtilImpl<bsls_Platform::CpuSparc_32>::Int64 *aInt)
{
    return bces_AtomicUtilImpl_Sparc32AddInt64(&aInt->d_value, 1);
}

inline
bsls_PlatformUtil::Int64
   bces_AtomicUtilImpl<bsls_Platform::CpuSparc_32>::decrementInt64Nv(
                 bces_AtomicUtilImpl<bsls_Platform::CpuSparc_32>::Int64 *aInt)
{
    return bces_AtomicUtilImpl_Sparc32AddInt64(&aInt->d_value, -1);
}

inline
void bces_AtomicUtilImpl<bsls_Platform::CpuSparc_32>::setInt64(
                  bces_AtomicUtilImpl<bsls_Platform::CpuSparc_32>::Int64 *aInt,
                  bsls_PlatformUtil::Int64                                val)
{
    bces_AtomicUtilImpl_Sparc32SetInt64(&aInt->d_value, val);
}

inline
void bces_AtomicUtilImpl<bsls_Platform::CpuSparc_32>::setInt64Relaxed(
                  bces_AtomicUtilImpl<bsls_Platform::CpuSparc_32>::Int64 *aInt,
                  bsls_PlatformUtil::Int64                                val)
{
    bces_AtomicUtilImpl_Sparc32SetInt64Relaxed(&aInt->d_value, val);
}

inline
bsls_PlatformUtil::Int64
bces_AtomicUtilImpl<bsls_Platform::CpuSparc_32>::getInt64(
            const bces_AtomicUtilImpl<bsls_Platform::CpuSparc_32>::Int64& aInt)
{
    return bces_AtomicUtilImpl_Sparc32GetInt64(&aInt.d_value);
}

inline
bsls_PlatformUtil::Int64
bces_AtomicUtilImpl<bsls_Platform::CpuSparc_32>::getInt64Relaxed(
            const bces_AtomicUtilImpl<bsls_Platform::CpuSparc_32>::Int64& aInt)
{
    return bces_AtomicUtilImpl_Sparc32GetInt64Relaxed(&aInt.d_value);
}

inline
void *bces_AtomicUtilImpl<bsls_Platform::CpuSparc_32>::getPtr(
      const bces_AtomicUtilImpl<bsls_Platform::CpuSparc_32>::Pointer& aPointer)
{
#if defined(BSLS_PLATFORM__CPU_32_BIT)
    return (void *) bces_AtomicUtilImpl_Sparc32GetInt(
                                      (const volatile int*)&aPointer.d_value);
#else
    return (void *) bces_AtomicUtilImpl_Sparc32GetInt64(
                  (const volatile bsls_PlatformUtil::Int64*)&aPointer.d_value);
#endif
}

inline
void *bces_AtomicUtilImpl<bsls_Platform::CpuSparc_32>::getPtrRelaxed(
                                                      const Pointer& aPointer)
{
    return const_cast<void*> (const_cast<volatile Pointer*>(&aPointer)->
                                                                      d_value);
}

inline
void bces_AtomicUtilImpl<bsls_Platform::CpuSparc_32>::setPtr(
            bces_AtomicUtilImpl<bsls_Platform::CpuSparc_32>::Pointer *aPointer,
            const void                                               *val)
{
#if defined(BSLS_PLATFORM__CPU_32_BIT)
    bces_AtomicUtilImpl_Sparc32SetInt((int*)&aPointer->d_value, (int)val);
#else
    bces_AtomicUtilImpl_Sparc32SetInt64(
                                 (bsls_PlatformUtil::Int64*)&aPointer->d_value,
                                 (bsls_PlatformUtil::Int64)val);
#endif
}

inline
void bces_AtomicUtilImpl<bsls_Platform::CpuSparc_32>::setPtrRelaxed(
            bces_AtomicUtilImpl<bsls_Platform::CpuSparc_32>::Pointer *aPointer,
            const void                                               *val)
{
    aPointer->d_value = val;
}

inline
void *bces_AtomicUtilImpl<bsls_Platform::CpuSparc_32>::swapPtr(
            bces_AtomicUtilImpl<bsls_Platform::CpuSparc_32>::Pointer *aPointer,
            const volatile void                                      *val)
{
#if defined(BSLS_PLATFORM__CPU_32_BIT)
    return (void*)bces_AtomicUtilImpl_Sparc32SwapInt(
                                             (volatile int*)&aPointer->d_value,
                                             (int)val);
#else
    return (void*)bces_AtomicUtilImpl_Sparc32SwapInt64(
                        (volatile bsls_PlatformUtil::Int64*)&aPointer->d_value,
                        (bsls_PlatformUtil::Int64)val);
#endif
}

inline
void *bces_AtomicUtilImpl<bsls_Platform::CpuSparc_32>::testAndSwapPtr(
            bces_AtomicUtilImpl<bsls_Platform::CpuSparc_32>::Pointer *aPointer,
            const volatile void                                      *cmpVal,
            const volatile void                                      *swapVal)
{
#if defined(BSLS_PLATFORM__CPU_32_BIT)
    return (void*)bces_AtomicUtilImpl_Sparc32TestAndSwap(
                                             (volatile int*)&aPointer->d_value,
                                             (int)cmpVal,
                                             (int)swapVal);
#else
    return (void*)bces_AtomicUtilImpl_Sparc32TestAndSwap64(
                        (volatile bsls_PlatformUtil::Int64*)&aPointer->d_value,
                        (bsls_PlatformUtil::Int64)cmpVal,
                        (bsls_PlatformUtil::Int64)swapVal);
#endif
}

inline
void bces_AtomicUtilImpl<bsls_Platform::CpuSparc_32>::spinLock(
              bces_AtomicUtilImpl<bsls_Platform::CpuSparc_32>::SpinLock *aSpin)
{
    bces_AtomicUtilImpl_Sparc32SpinLock(&aSpin->d_lock);
}

inline
int bces_AtomicUtilImpl<bsls_Platform::CpuSparc_32>::spinTryLock(
            bces_AtomicUtilImpl<bsls_Platform::CpuSparc_32>::SpinLock *aSpin,
            int                                                        retries)
{
    return bces_AtomicUtilImpl_Sparc32SpinTryLock(&aSpin->d_lock,retries);
}

inline
void bces_AtomicUtilImpl<bsls_Platform::CpuSparc_32>::spinUnlock(
              bces_AtomicUtilImpl<bsls_Platform::CpuSparc_32>::SpinLock *aSpin)
{
     bces_AtomicUtilImpl_Sparc32SpinUnlock(&aSpin->d_lock);
}

}  // close namespace BloombergLP

#endif

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2002
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
