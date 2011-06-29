// bces_atomicutilimpl_amd64.h                                        -*-C++-*-
#ifndef INCLUDED_BCES_ATOMICUTILIMPL_AMD64
#define INCLUDED_BCES_ATOMICUTILIMPL_AMD64

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: provide atomic operations on AMD64 architectures.
//
//@CLASSES:
//   bces_AtomicUtilImpl<bsls_Platform::CpuX86_64>: Namespace for
//   atomic operations on the AMD64 architecture.
//
//@AUTHOR: Nathan Dorfman (ndorfman)
//
//@DESCRIPTION: This component provides a full specialization of
// 'bces_AtomicUtilImpl' for the AMD64 architecture.  This component is
// not intended to be used directly.  See the 'bces_AtomicUtil' for more on
// using atomic operations.

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

#ifdef BSLS_PLATFORM__CPU_X86_64

#if defined(BSLS_PLATFORM__OS_WINDOWS)
#if defined(_MSC_VER) && (_MSC_VER >= 1400)
#ifndef INCLUDED_INTRIN
#include <intrin.h>
#define INCLUDED_INTRIN
#endif
#ifndef INCLUDED_EMMINTRIN
#include <emmintrin.h>
#define INCLUDED_EMMINTRIN
#endif
#else
#error Visual Studio 2005 or above is required on Windows
#endif
#endif

namespace BloombergLP {

#if defined(BSLS_PLATFORM__CMP_GNU)

inline
void bces_AtomicUtilImpl_Amd64SetInt(volatile int *aInt, int val)
{
    __asm__ __volatile__ (
                          "movl %1,%0\n\t"
                          "mfence\n\t"
                          : "=m"(*aInt)
                          : "r" (val)
                          : "memory" );
}

int bces_AtomicUtilImpl_Amd64SwapInt(volatile int *aInt,
                                     int           val);

inline
int bces_AtomicUtilImpl_Amd64GetInt(volatile const int *aInt)
{
    int ret;
    __asm__ __volatile__ ("mfence\n\t"
                          "movl %1,%0"
                          : "=r"(ret)
                          : "m"(*aInt)
                          : "memory");
    return ret;
}

inline
int bces_AtomicUtilImpl_Amd64AddInt(volatile int *aInt, int val)
{
   int oldVal = val;
    __asm__ __volatile__ (
                          "lock xaddl %0, %1\n\t"
                          : "=r"(val), "+m"(*aInt)
                          : "0"(val)
                          : "memory", "cc");

   return val + oldVal;
}

inline
int bces_AtomicUtilImpl_Amd64AddIntRelaxed(volatile int *aInt, int val)
{
   int oldVal = val;
    __asm__  ("lock xaddl %0, %1\n\t"
              : "=r"(val), "+m"(*aInt)
              : "0"(val)
              : "cc");

   return val + oldVal;
}

inline
int bces_AtomicUtilImpl_Amd64TestAndSwap(volatile int *aInt,
                                         int           cmpVal,
                                         int           swapVal)
{
    __asm__ __volatile__ (
                          "lock cmpxchgl %2, %1\n\t"
                          : "+a" (cmpVal), "+m" (*aInt)
                          : "r" (swapVal)
                          : "memory", "cc");
   return cmpVal;

}

inline
void  bces_AtomicUtilImpl_Amd64SetInt64(
                                       volatile bsls_PlatformUtil::Int64 *aInt,
                                       bsls_PlatformUtil::Int64 val)
{
    __asm__ __volatile__ (
                          "movq %1,%0\n\t"
                          "mfence\n\t"
                          : "=m"(*aInt)
                          : "r" (val)
                          : "memory" );
}

inline
bsls_PlatformUtil::Int64  bces_AtomicUtilImpl_Amd64SwapInt64(
                                      volatile bsls_PlatformUtil::Int64 *aInt,
                                      bsls_PlatformUtil::Int64 val)
{
    __asm__ __volatile__ (
        "lock xchgq %0,%1\n\t"
        : "+r"(val), "=m"(*aInt)
        : "m"(*aInt)
        : "memory");
    return val;
}

inline
bsls_PlatformUtil::Int64 bces_AtomicUtilImpl_Amd64GetInt64(
                                 volatile const bsls_PlatformUtil::Int64 *aInt)
{
    bsls_PlatformUtil::Int64 ret;
    __asm__ __volatile__ ("mfence\n\t"
                          "movq %1,%0"
                          : "=r"(ret)
                          : "m"(*aInt)
                          : "memory");
    return ret;
}

inline
bsls_PlatformUtil::Int64 bces_AtomicUtilImpl_Amd64AddInt64(
                                       volatile bsls_PlatformUtil::Int64 *aInt,
                                       bsls_PlatformUtil::Int64 val )
{
    bsls_PlatformUtil::Int64 oldVal = val;
    __asm__ __volatile__ (
                          "lock xaddq %0, %1\n\t"
                          : "+r"(val), "+m"(*aInt)
                          :
                          : "memory", "cc");

    return val + oldVal;
}

inline
bsls_PlatformUtil::Int64 bces_AtomicUtilImpl_Amd64AddInt64Relaxed(
                                       volatile bsls_PlatformUtil::Int64 *aInt,
                                       bsls_PlatformUtil::Int64 val )
{
    bsls_PlatformUtil::Int64 oldVal = val;
    __asm__ ("lock xaddq %0, %1\n\t"
             : "+r"(val), "+m"(*aInt)
             :
             : "cc");

    return val + oldVal;
}

inline bsls_PlatformUtil::Int64 bces_AtomicUtilImpl_Amd64TestAndSwap64(
                                    volatile bsls_PlatformUtil::Int64 *aInt,
                                    bsls_PlatformUtil::Int64           cmpVal,
                                    bsls_PlatformUtil::Int64           swapVal)
{
    __asm__ __volatile__ (
                          "lock cmpxchgq %2, %1\n\t"
                          : "+a" (cmpVal), "+m" (*aInt)
                          : "r" (swapVal)
                          : "memory", "cc");

   return cmpVal;
}

inline
void bces_AtomicUtilImpl_Amd64SpinLock(volatile char *aSpin)
{
    register int result;
    register int cmpValue;
    __asm__ __volatile__ ("1:\n\t"
                        "xorb %%al, %%al\n\t"
                        "lock cmpxchgb %b2, %0; \n\t"
                        "jnz 1b\n\t"
                       : "+m"(*aSpin), "=a" (result), "=q" (cmpValue)
                       : "1" (0), "2" (0xff)
                       : "memory", "cc");
}

inline
int bces_AtomicUtilImpl_Amd64SpinTryLock(volatile char *aSpin, int retries)
{
    register int result;
    register int cmpValue;
    register bsls_PlatformUtil::Int64 numRetries = retries;
    __asm__ __volatile__ (
                          "\n1:\t"
                          "xorb %%al,%%al\n\t"
                          "lock cmpxchgb %b2,%0\n\t"
                          "loopnzq 1b\n\t"
                          : "+m" (*aSpin), "=a" (result), "=q" (cmpValue),
                            "+c"(numRetries)
                          : "1" (0), "2"(0xff)
                          : "memory", "cc");
   return result;
}

inline
void bces_AtomicUtilImpl_Amd64SpinUnlock(volatile char *aSpin)
{
    register char result;
    __asm__ __volatile__ (
                          "lock xchgb %b1,%0\n\t"
                          : "+m" (*aSpin), "=q" (result)
                          : "1" (0)
                          : "memory");
}

#elif defined(BSLS_PLATFORM__CMP_SUN)

extern "C" {

int bces_AtomicUtilImpl_Amd64SetInt(volatile int *aInt,
                                     int           val);
int bces_AtomicUtilImpl_Amd64SwapInt(volatile int *aInt,
                                     int           val);

int bces_AtomicUtilImpl_Amd64AddInt(volatile int *aInt,
                                     int           val);

int bces_AtomicUtilImpl_Amd64TestAndSwap(volatile int *aInt,
                                          int           cmpVal,
                                          int           swapVal);

bsls_PlatformUtil::Int64 bces_AtomicUtilImpl_Amd64SetInt64(
                                    volatile bsls_PlatformUtil::Int64 *aInt,
                                    bsls_PlatformUtil::Int64           val);

bsls_PlatformUtil::Int64 bces_AtomicUtilImpl_Amd64SwapInt64(
                                    volatile bsls_PlatformUtil::Int64 *aInt,
                                    bsls_PlatformUtil::Int64           val);

bsls_PlatformUtil::Int64 bces_AtomicUtilImpl_Amd64GetInt64(
                                    volatile const bsls_PlatformUtil::Int64*);

bsls_PlatformUtil::Int64 bces_AtomicUtilImpl_Amd64AddInt64(
                                    volatile bsls_PlatformUtil::Int64 *aInt,
                                    bsls_PlatformUtil::Int64           val);

bsls_PlatformUtil::Int64 bces_AtomicUtilImpl_Amd64TestAndSwap64(
                                   volatile bsls_PlatformUtil::Int64 *aInt,
                                   bsls_PlatformUtil::Int64           cmpVal,
                                   bsls_PlatformUtil::Int64           swapVal);

void bces_AtomicUtilImpl_Amd64SpinLock(volatile char *aSpin);
int  bces_AtomicUtilImpl_Amd64SpinTryLock(
                                 volatile char *aSpin, int retries);
void bces_AtomicUtilImpl_Amd64SpinUnlock(
                                        volatile char *aSpin);
}

#elif defined(BSLS_PLATFORM__OS_WINDOWS) \
   && defined(_MSC_VER) \
   && (_MSC_VER >= 1300)

// using compiler intrinsics on Windows

inline void bces_AtomicUtilImpl_Amd64SetInt(
        volatile int *aInt,
        int val)
{
    // MS Studio 2005 and later do not reorder operations on volatiles.

    *aInt = val;
    _mm_mfence();
}

inline int bces_AtomicUtilImpl_Amd64GetInt(
        volatile const int *aInt)
{
    // MS Studio 2005 and later do not reorder operations on volatiles.

    _mm_mfence();
    return( *aInt);
}

inline int bces_AtomicUtilImpl_Amd64AddInt(
        volatile int *aInt,
        int val)
{
    return _InterlockedExchangeAdd(reinterpret_cast<volatile long*>(aInt), val)
                                   + val;
}

inline int bces_AtomicUtilImpl_Amd64AddIntRelaxed(
        volatile int *aInt,
        int val)
{
    return _InterlockedExchangeAdd(reinterpret_cast<volatile long*>(aInt), val)
                                   + val;
}

inline int bces_AtomicUtilImpl_Amd64SwapInt(volatile int *aInt,
                                            int           val)
{
    return _InterlockedExchange(reinterpret_cast<volatile long*>(aInt), val);
}

inline int bces_AtomicUtilImpl_Amd64TestAndSwap(
        volatile int *aInt,
        int           cmpVal,
        int           swapVal)
{
    return _InterlockedCompareExchange(
                                        reinterpret_cast<volatile long*>(aInt),
                                        swapVal, cmpVal);
}

inline void bces_AtomicUtilImpl_Amd64SetInt64(
    volatile bsls_PlatformUtil::Int64 *aInt,
    bsls_PlatformUtil::Int64           val)
{
    // MS Studio 2005 and later do not reorder operations on volatiles.

    *aInt = val;
    _mm_mfence();
}

inline bsls_PlatformUtil::Int64 bces_AtomicUtilImpl_Amd64SwapInt64(
    volatile bsls_PlatformUtil::Int64 *aInt,
    bsls_PlatformUtil::Int64           val)
{
    return _InterlockedExchange64(aInt, val);
}

inline bsls_PlatformUtil::Int64 bces_AtomicUtilImpl_Amd64GetInt64(
        volatile const bsls_PlatformUtil::Int64 *aInt)
{
    // MS Studio 2005 and later do not reorder operations on volatiles.

    _mm_mfence();
    return *aInt;
}

inline bsls_PlatformUtil::Int64 bces_AtomicUtilImpl_Amd64AddInt64(
        volatile bsls_PlatformUtil::Int64 *aInt,
        bsls_PlatformUtil::Int64           val )
{
    return _InterlockedExchangeAdd64(aInt, val) + val;
}

inline bsls_PlatformUtil::Int64 bces_AtomicUtilImpl_Amd64AddInt64Relaxed(
        volatile bsls_PlatformUtil::Int64 *aInt,
        bsls_PlatformUtil::Int64           val )
{
    return _InterlockedExchangeAdd64(aInt, val) + val;
}

inline bsls_PlatformUtil::Int64 bces_AtomicUtilImpl_Amd64TestAndSwap64(
        volatile bsls_PlatformUtil::Int64 *aInt,
        bsls_PlatformUtil::Int64           cmpVal,
        bsls_PlatformUtil::Int64           swapVal)
{
    return _InterlockedCompareExchange64(aInt, swapVal, cmpVal);
}

inline void bces_AtomicUtilImpl_Amd64SpinLock(
        volatile int *aSpin)
{
    do {
        if (*aSpin) { _mm_pause(); continue; }
    } while(_InterlockedExchange(reinterpret_cast<volatile long*>(aSpin), 1));
}

inline int  bces_AtomicUtilImpl_Amd64SpinTryLock(
        volatile int *aSpin,
        int            retries)
{
    do {
        if (*aSpin) {
            if(!retries--) {
                return 1;
            }
            _mm_pause();
            continue;
        }
    } while(_InterlockedExchange(reinterpret_cast<volatile long*>(aSpin),1));
    return 0;
}

inline void bces_AtomicUtilImpl_Amd64SpinUnlock(
        volatile int *aSpin)
{
    *aSpin = 0;
    _mm_mfence();
}

#else
#error not implemented for this platform
#endif

template <class PLATFORM> struct bces_AtomicUtilImpl;

template <>
struct bces_AtomicUtilImpl<bsls_Platform::CpuX86_64> {
    // Namespace for atomic operations on the AMD64 architecture.

    // TYPES
    struct SpinLock {
#if defined(BSLS_PLATFORM__OS_WINDOWS)
        volatile int d_spin;
#else
        volatile char d_spin;
#endif
    };

    struct Int {
        volatile int d_value
#ifdef BSLS_PLATFORM__CMP_GNU
             __attribute__((__aligned__(sizeof(int))))
#endif
            ;
    };

    struct Int64 {
        volatile bsls_PlatformUtil::Int64 d_value
#ifdef BSLS_PLATFORM__CMP_GNU
             __attribute__((__aligned__(sizeof(bsls_PlatformUtil::Int64))))
#endif
            ;
    };

    struct Pointer {
        const void * volatile d_value
#ifdef BSLS_PLATFORM__CMP_GNU
             __attribute__((__aligned__(sizeof(void*))))
#endif
            ;
    };

    static void initInt(
          bces_AtomicUtilImpl<bsls_Platform::CpuX86_64>::Int *aInt,
          int initialValue);
        // Initialize the specified atomic integer 'aInt', and set its value to
        // the specified 'initialValue'.  Note that this method must be called
        // before any other operations on 'aInt'.

    static void initInt64(
        bces_AtomicUtilImpl<bsls_Platform::CpuX86_64>::Int64 *aInt,
        bsls_PlatformUtil::Int64  initialValue);
        // Initialize the specified atomic integer 'aInt', and set its value to
        // the specified 'initialValue'.  Note that this method must be called
        // before any other operations on 'aInt'.

    static void initPointer(
        bces_AtomicUtilImpl<bsls_Platform::CpuX86_64>::Pointer *aPtr,
        const void *initialValue);
        // Initialize the specified atomic pointer 'aPointer', and set its
        // value to the specified 'initialValue'.  Note that this function must
        // be called before any other operations on 'aPointer'.

    static void initSpinLock(
      bces_AtomicUtilImpl<bsls_Platform::CpuX86_64>::SpinLock *aSpin);
        // Initialize the specified spin lock 'aSpin' and set it to an an
        // unlocked state.  Note that method must be called before any other
        // operations on 'aSpin'.

    static void addInt(
        bces_AtomicUtilImpl<bsls_Platform::CpuX86_64>::Int *aInt,
        int val);
        // Atomically add to the specified 'aInt' the specified 'value'.  Note
        // that the behavior is undefined if 'aInt' is 0.

    static void incrementInt(
         bces_AtomicUtilImpl<bsls_Platform::CpuX86_64>::Int *aInt);
        // Atomically increment the value of the specified 'aInt' by 1.  Note
        // that the behavior is undefined if 'aInt' is 0.

    static void decrementInt(
         bces_AtomicUtilImpl<bsls_Platform::CpuX86_64>::Int *aInt);
        // Atomically decrement the value of the specified 'aInt' by 1.  Note
        // that the behavior is undefined if 'aInt' is 0.

    static int swapInt(
         bces_AtomicUtilImpl<bsls_Platform::CpuX86_64>::Int *aInt,
         int val);
        // Atomically set the value of the specified 'aInt' to the specified
        // 'value' and return its previous value.  Note that the behavior is
        // undefined if 'aInt' is 0.

    static int testAndSwapInt(
          bces_AtomicUtilImpl<bsls_Platform::CpuX86_64>::Int *aInt,
          int cmpVal, int swapVal);
        // Conditionally set the value of the specified 'aInt' to the specified
        // 'swapVal' and return its previous value.  The value of 'aInt' is
        // compared to the specified 'cmpVal'; if they are equal, then
        // 'swapVal' will be assigned to 'aInt', otherwise the value of 'aInt'
        // is left unchanged.  The whole operation is performed atomically.
        // Note that the behavior is undefined if 'aInt' is 0.

    static void setInt(
          bces_AtomicUtilImpl<bsls_Platform::CpuX86_64>::Int *aInt,
          int val);
        // Atomically set the value of the specified 'aInt' to the specified
        // 'value'.  Note that the behavior is undefined if 'aInt' is 0.

    static void setIntRelaxed(
          bces_AtomicUtilImpl<bsls_Platform::CpuX86_64>::Int *aInt,
          int val);
        // Atomically set the value of the specified 'aInt' to the specified
        // 'value' without additional ordering constraints.  The behavior is
        // undefined if 'aInt' is 0.

    static int getInt(
       const bces_AtomicUtilImpl<bsls_Platform::CpuX86_64>::Int& aInt);
        // Atomically retrieve the value of the specified 'aInt'.

    static int getIntRelaxed(
       const bces_AtomicUtilImpl<bsls_Platform::CpuX86_64>::Int& aInt);
        // Atomically retrieve the value of the specified 'aInt' without
        // additional ordering constraints.

    static int addIntNv(
             bces_AtomicUtilImpl<bsls_Platform::CpuX86_64>::Int *aInt,
             int val);
        // Atomically add to the specified 'aInt' the specified 'value' and
        // return the resulting value.  Note that the behavior is undefined if
        // 'aInt' is 0.

    static int addIntNvRelaxed(
             bces_AtomicUtilImpl<bsls_Platform::CpuX86_64>::Int *aInt,
             int val);
        // Atomically add to the specified 'aInt' the specified 'value' and
        // return the resulting value, without additional ordering constraints.
        // The behavior is undefined if 'aInt' is 0.

    static int incrementIntNv(
             bces_AtomicUtilImpl<bsls_Platform::CpuX86_64>::Int *aInt);
        // Atomically increment the specified 'aInt' by 1 and return the
        // resulting value.  The behavior is undefined if 'aInt' is 0.

    static int decrementIntNv(
             bces_AtomicUtilImpl<bsls_Platform::CpuX86_64>::Int *aInt);
        // Atomically decrement the specified 'aInt' by 1 and return the
        // resulting value.  Note that the behavior is undefined if 'aInt' is
        // 0.

    static void addInt64(
         bces_AtomicUtilImpl<bsls_Platform::CpuX86_64>::Int64 *aInt,
         bsls_PlatformUtil::Int64 val);
        // Atomically add to the specified 'aInt' the specified 'value'.  Note
        // that the behavior is undefined if 'aInt' is 0.

    static void incrementInt64(
         bces_AtomicUtilImpl<bsls_Platform::CpuX86_64>::Int64 *aInt);
        // Atomically increment the value of the specified 'aInt' by 1.  Note
        // that the behavior is undefined if 'aInt' is 0.

    static void decrementInt64(
         bces_AtomicUtilImpl<bsls_Platform::CpuX86_64>::Int64 *aInt);
        // Atomically decrement the value of the specified 'aInt' by 1.  Note
        // that the behavior is undefined if 'aInt' is 0.

    static bsls_PlatformUtil::Int64 swapInt64(
         bces_AtomicUtilImpl<bsls_Platform::CpuX86_64>::Int64 *aInt,
         bsls_PlatformUtil::Int64 val);
        // Atomically set the value of the specified 'aInt' to the specified
        // 'value' and return its previous value.  Note that the behavior is
        // undefined if 'aInt' is 0.

    static bsls_PlatformUtil::Int64 testAndSwapInt64(
         bces_AtomicUtilImpl<bsls_Platform::CpuX86_64>::Int64 *aInt,
         bsls_PlatformUtil::Int64  cmpVal,
         bsls_PlatformUtil::Int64 swapVal);
        // Conditionally set the value of the specified 'aInt' to the specified
        // 'swapVal' and return its previous value.  The value of 'aInt' is
        // compared to the specified 'cmpVal'; if they are equal, then
        // 'swapVal' will be assigned to 'aInt' otherwise, the value of 'aInt'
        // is left unchanged.  The whole operation is performed atomically.
        // Note that the behavior is undefined if 'aInt' is 0.

    static bsls_PlatformUtil::Int64 addInt64Nv(
         bces_AtomicUtilImpl<bsls_Platform::CpuX86_64>::Int64 *aInt,
         bsls_PlatformUtil::Int64 val);
        // Atomically add to the specified 'aInt' the specified 'value' and
        // return the resulting value.  Note that the behavior is undefined if
        // 'aInt' is 0.

    static bsls_PlatformUtil::Int64 addInt64NvRelaxed(
         bces_AtomicUtilImpl<bsls_Platform::CpuX86_64>::Int64 *aInt,
         bsls_PlatformUtil::Int64 val);
        // Atomically add to the specified 'aInt' the specified 'value' and
        // return the resulting value, without additional ordering constraints.
        // The behavior is undefined if 'aInt' is 0.

    static bsls_PlatformUtil::Int64 incrementInt64Nv(
         bces_AtomicUtilImpl<bsls_Platform::CpuX86_64>::Int64 *aInt);
        // Atomically increment the specified 'aInt' by 1 and return the
        // resulting value.  The behavior is undefined if 'aInt' is 0.

    static bsls_PlatformUtil::Int64 decrementInt64Nv(
         bces_AtomicUtilImpl<bsls_Platform::CpuX86_64>::Int64 *aInt);
        // Atomically decrement the specified 'aInt' by 1 and return the
        // resulting value.  Note that the behavior is undefined if 'aInt' is
        // 0.

    static void setInt64(
         bces_AtomicUtilImpl<bsls_Platform::CpuX86_64>::Int64 *aInt,
         bsls_PlatformUtil::Int64 val);
        // Atomically set the value of the specified 'aInt' to the specified
        // 'value'.  The behavior is undefined if 'aInt' is 0.

    static void setInt64Relaxed(
         bces_AtomicUtilImpl<bsls_Platform::CpuX86_64>::Int64 *aInt,
         bsls_PlatformUtil::Int64 val);
        // Atomically set the value of the specified 'aInt' to the specified
        // 'value' without additional ordering constraints.  The behavior is
        // undefined if 'aInt' is 0.

    static bsls_PlatformUtil::Int64 getInt64(
     const bces_AtomicUtilImpl<bsls_Platform::CpuX86_64>::Int64& aInt);
        // Atomically retrieve the value of the specified 'aInt'.

    static bsls_PlatformUtil::Int64 getInt64Relaxed(
     const bces_AtomicUtilImpl<bsls_Platform::CpuX86_64>::Int64& aInt);
        // Atomically retrieve the value of the specified 'aInt' without
        // additional ordering constraints.

    static void *getPtr(
       const bces_AtomicUtilImpl<bsls_Platform::CpuX86_64>::Pointer& aPointer);
        // Atomically retrieve the value of the specified 'aPointer'.

    static void *getPtrRelaxed(
       const bces_AtomicUtilImpl<bsls_Platform::CpuX86_64>::Pointer& aPointer);
        // Atomically retrieve the value of the specified 'aPointer' without
        // additional ordering constraints.

    static void setPtr(
            bces_AtomicUtilImpl<bsls_Platform::CpuX86_64>::Pointer *aPointer,
            const void *val);
        // Atomically set the value of the specified 'aPointer' to the
        // specified 'value'.  The behavior is undefined if 'aPointer' is 0.

    static void setPtrRelaxed(
            bces_AtomicUtilImpl<bsls_Platform::CpuX86_64>::Pointer *aPointer,
            const void *val);
        // Atomically set the value of the specified 'aPointer' to the
        // specified 'value' without additional ordering constraints.  The
        // behavior is undefined if 'aPointer' is 0.

    static void *swapPtr(
     bces_AtomicUtilImpl<bsls_Platform::CpuX86_64>::Pointer *aPointer,
     const void *val);
        // Atomically set the value of the specified 'aPointer' to the
        // specified 'value' and return its previous value.  Note that the
        // behavior is undefined if 'aPointer' is 0.

    static void *testAndSwapPtr(
     bces_AtomicUtilImpl<bsls_Platform::CpuX86_64>::Pointer *aPointer,
     const void *cmpVal,
     const void *swapVal);
        // Conditionally set the value of the specified 'aPointer' to the
        // specified 'swapVal' and return its previous value.  The value of
        // 'aPointer' is compared to the specified 'cmpVal'; if they are equal,
        // then 'swapVal' will be assigned to 'aPointer' otherwise, the value
        // of 'aPointer' is left unchanged.  The whole operation is performed
        // atomically.  Note that the behavior is undefined if 'aPointer' is 0.

    static void spinLock(
    bces_AtomicUtilImpl<bsls_Platform::CpuX86_64>::SpinLock *aSpin);
        // Lock the specified 'aSpin'.  Note that the behavior is undefined if
        // 'aSpin' is 0.

    static int spinTryLock(
    bces_AtomicUtilImpl<bsls_Platform::CpuX86_64>::SpinLock *aSpin,
    int retries);
        // Attempt to lock the specified 'aSpin' up to 'retries' times.  Return
        // 0 on success, non-zero otherwise.  Note that the behavior is
        // undefined if 'aSpin' is 0 or 0 <= retries.

    static void spinUnlock(
    bces_AtomicUtilImpl<bsls_Platform::CpuX86_64>::SpinLock *aSpin);
        // Unlock the specified 'aSpin' which was previously locked by a
        // successful call to 'spinLock' or 'spinTryLock'.  Note that the
        // behavior is undefined if 'aSpin' is 0.
};

inline
void bces_AtomicUtilImpl<bsls_Platform::CpuX86_64>::initInt(
              bces_AtomicUtilImpl<bsls_Platform::CpuX86_64>::Int *aInt,
              int initialValue)
{
    BSLS_ASSERT_OPT(bsls_AlignmentUtil::is4ByteAligned(aInt));
    aInt->d_value = initialValue;
}

inline
void bces_AtomicUtilImpl<bsls_Platform::CpuX86_64>::initInt64(
            bces_AtomicUtilImpl<bsls_Platform::CpuX86_64>::Int64 *aInt,
            bsls_PlatformUtil::Int64  initialValue)
{
    BSLS_ASSERT_OPT(bsls_AlignmentUtil::is8ByteAligned(aInt));
    aInt->d_value = initialValue;
}

inline
void bces_AtomicUtilImpl<bsls_Platform::CpuX86_64>::initPointer(
          bces_AtomicUtilImpl<bsls_Platform::CpuX86_64>::Pointer *aPtr,
          const void *initialValue )
{
    BSLS_ASSERT_OPT(bsls_AlignmentUtil::is8ByteAligned(aPtr));
    aPtr->d_value = initialValue;
}

inline
void bces_AtomicUtilImpl<bsls_Platform::CpuX86_64>::initSpinLock(
       bces_AtomicUtilImpl<bsls_Platform::CpuX86_64>::SpinLock *aSpin)
{
#if defined(BSLS_PLATFORM__OS_WINDOWS)
    BSLS_ASSERT_OPT(bsls_AlignmentUtil::is4ByteAligned(aSpin));
#endif
    // otherwise the spinlock is a byte
    aSpin->d_spin = 0;
}

inline
void bces_AtomicUtilImpl<bsls_Platform::CpuX86_64>::addInt(
    bces_AtomicUtilImpl<bsls_Platform::CpuX86_64>::Int *aInt, int val)
{
    bces_AtomicUtilImpl_Amd64AddInt(&aInt->d_value, val);
}

inline
void bces_AtomicUtilImpl<bsls_Platform::CpuX86_64>::incrementInt(
              bces_AtomicUtilImpl<bsls_Platform::CpuX86_64>::Int *aInt)
{
    bces_AtomicUtilImpl_Amd64AddInt(&aInt->d_value, 1);
}

inline
void bces_AtomicUtilImpl<bsls_Platform::CpuX86_64>::decrementInt(
              bces_AtomicUtilImpl<bsls_Platform::CpuX86_64>::Int *aInt)
{
    bces_AtomicUtilImpl_Amd64AddInt(&aInt->d_value, -1);
}

inline
int bces_AtomicUtilImpl<bsls_Platform::CpuX86_64>::swapInt(
              bces_AtomicUtilImpl<bsls_Platform::CpuX86_64>::Int *aInt,
              int val)
{
    return bces_AtomicUtilImpl_Amd64SwapInt(&aInt->d_value, val);
}

inline
int bces_AtomicUtilImpl<bsls_Platform::CpuX86_64>::testAndSwapInt(
              bces_AtomicUtilImpl<bsls_Platform::CpuX86_64>::Int *aInt,
              int cmpVal, int swapVal)
{
    return bces_AtomicUtilImpl_Amd64TestAndSwap(&aInt->d_value,cmpVal,
                                                       swapVal);
}

inline
void bces_AtomicUtilImpl<bsls_Platform::CpuX86_64>::setIntRelaxed(
             bces_AtomicUtilImpl<bsls_Platform::CpuX86_64>::Int *aInt,
             int val)
{
    aInt->d_value = val;
}

inline
void bces_AtomicUtilImpl<bsls_Platform::CpuX86_64>::setInt(
             bces_AtomicUtilImpl<bsls_Platform::CpuX86_64>::Int *aInt,
             int val)
{
    bces_AtomicUtilImpl_Amd64SetInt(&aInt->d_value, val);
}

inline
int  bces_AtomicUtilImpl<bsls_Platform::CpuX86_64>::getInt(
        const bces_AtomicUtilImpl<bsls_Platform::CpuX86_64>::Int& aInt)
{
    return bces_AtomicUtilImpl_Amd64GetInt(&aInt.d_value);
}

inline
int  bces_AtomicUtilImpl<bsls_Platform::CpuX86_64>::getIntRelaxed(
        const bces_AtomicUtilImpl<bsls_Platform::CpuX86_64>::Int& aInt)
{
    return aInt.d_value;
}

inline
int bces_AtomicUtilImpl<bsls_Platform::CpuX86_64>::addIntNv(
    bces_AtomicUtilImpl<bsls_Platform::CpuX86_64>::Int *aInt, int val)
{
    return bces_AtomicUtilImpl_Amd64AddInt(&aInt->d_value, val);
}

inline
int bces_AtomicUtilImpl<bsls_Platform::CpuX86_64>::addIntNvRelaxed(
             bces_AtomicUtilImpl<bsls_Platform::CpuX86_64>::Int *aInt, int val)
{
    return bces_AtomicUtilImpl_Amd64AddIntRelaxed(&aInt->d_value, val);
}

inline
int bces_AtomicUtilImpl<bsls_Platform::CpuX86_64>::incrementIntNv(
             bces_AtomicUtilImpl<bsls_Platform::CpuX86_64>::Int *aInt)
{
    return bces_AtomicUtilImpl_Amd64AddInt(&aInt->d_value, 1);
}

inline
int bces_AtomicUtilImpl<bsls_Platform::CpuX86_64>::decrementIntNv(
              bces_AtomicUtilImpl<bsls_Platform::CpuX86_64>::Int *aInt)
{
    return bces_AtomicUtilImpl_Amd64AddInt(&aInt->d_value, -1);
}

inline
void bces_AtomicUtilImpl<bsls_Platform::CpuX86_64>::addInt64(
            bces_AtomicUtilImpl<bsls_Platform::CpuX86_64>::Int64 *aInt,
            bsls_PlatformUtil::Int64 val)
{
    bces_AtomicUtilImpl_Amd64AddInt64(&aInt->d_value, val);
}

inline
void bces_AtomicUtilImpl<bsls_Platform::CpuX86_64>::incrementInt64(
            bces_AtomicUtilImpl<bsls_Platform::CpuX86_64>::Int64 *aInt)
{
    bces_AtomicUtilImpl_Amd64AddInt64(&aInt->d_value, 1);
}

inline
void bces_AtomicUtilImpl<bsls_Platform::CpuX86_64>::decrementInt64(
            bces_AtomicUtilImpl<bsls_Platform::CpuX86_64>::Int64 *aInt)
{
    bces_AtomicUtilImpl_Amd64AddInt64(&aInt->d_value, -1);
}

inline
bsls_PlatformUtil::Int64
bces_AtomicUtilImpl<bsls_Platform::CpuX86_64>::swapInt64(
            bces_AtomicUtilImpl<bsls_Platform::CpuX86_64>::Int64 *aInt,
            bsls_PlatformUtil::Int64 val)
{
    return bces_AtomicUtilImpl_Amd64SwapInt64(&aInt->d_value, val);
}

inline
bsls_PlatformUtil::Int64
bces_AtomicUtilImpl<bsls_Platform::CpuX86_64>::testAndSwapInt64(
                                                                  Int64 *aInt,
                                              bsls_PlatformUtil::Int64 cmpVal,
                                              bsls_PlatformUtil::Int64 swapVal)
{
    return bces_AtomicUtilImpl_Amd64TestAndSwap64(&aInt->d_value,
                                                  cmpVal,
                                                  swapVal);
}

inline
bsls_PlatformUtil::Int64
bces_AtomicUtilImpl<bsls_Platform::CpuX86_64>::addInt64Nv(
        bces_AtomicUtilImpl<bsls_Platform::CpuX86_64>::Int64 *aInt,
        bsls_PlatformUtil::Int64 val)
{
    return bces_AtomicUtilImpl_Amd64AddInt64(&aInt->d_value, val);
}

inline
bsls_PlatformUtil::Int64
bces_AtomicUtilImpl<bsls_Platform::CpuX86_64>::addInt64NvRelaxed(
        bces_AtomicUtilImpl<bsls_Platform::CpuX86_64>::Int64 *aInt,
        bsls_PlatformUtil::Int64 val)
{
    return bces_AtomicUtilImpl_Amd64AddInt64Relaxed(&aInt->d_value, val);
}

inline
bsls_PlatformUtil::Int64
bces_AtomicUtilImpl<bsls_Platform::CpuX86_64>::incrementInt64Nv(
            bces_AtomicUtilImpl<bsls_Platform::CpuX86_64>::Int64 *aInt)
{
    return bces_AtomicUtilImpl_Amd64AddInt64(&aInt->d_value, 1);
}

inline
bsls_PlatformUtil::Int64
bces_AtomicUtilImpl<bsls_Platform::CpuX86_64>::decrementInt64Nv(
            bces_AtomicUtilImpl<bsls_Platform::CpuX86_64>::Int64 *aInt)
{
    return bces_AtomicUtilImpl_Amd64AddInt64(&aInt->d_value, -1);
}

inline
void bces_AtomicUtilImpl<bsls_Platform::CpuX86_64>::setInt64(
            bces_AtomicUtilImpl<bsls_Platform::CpuX86_64>::Int64 *aInt,
            bsls_PlatformUtil::Int64 val)
{
    bces_AtomicUtilImpl_Amd64SetInt64(&aInt->d_value, val);
}

inline
void bces_AtomicUtilImpl<bsls_Platform::CpuX86_64>::setInt64Relaxed(
            bces_AtomicUtilImpl<bsls_Platform::CpuX86_64>::Int64 *aInt,
            bsls_PlatformUtil::Int64 val)
{
    aInt->d_value = val;
}

inline
bsls_PlatformUtil::Int64
bces_AtomicUtilImpl<bsls_Platform::CpuX86_64>::getInt64(
      const bces_AtomicUtilImpl<bsls_Platform::CpuX86_64>::Int64& aInt)
{
    return bces_AtomicUtilImpl_Amd64GetInt64(&aInt.d_value);
}

inline
bsls_PlatformUtil::Int64
bces_AtomicUtilImpl<bsls_Platform::CpuX86_64>::getInt64Relaxed(
      const bces_AtomicUtilImpl<bsls_Platform::CpuX86_64>::Int64& aInt)
{
    return aInt.d_value;
}

inline
void *bces_AtomicUtilImpl<bsls_Platform::CpuX86_64>::getPtr(
const bces_AtomicUtilImpl<bsls_Platform::CpuX86_64>::Pointer& aPointer)
{
    return (void*)bces_AtomicUtilImpl_Amd64GetInt64(
                       (volatile bsls_PlatformUtil::Int64*)&aPointer.d_value);
}

inline
void *bces_AtomicUtilImpl<bsls_Platform::CpuX86_64>::getPtrRelaxed(
const bces_AtomicUtilImpl<bsls_Platform::CpuX86_64>::Pointer& aPointer)
{
    // This ugly cast is necessary because otherwise the Windows optimizer
    // will eliminate the load operation.
    volatile bces_AtomicUtilImpl<bsls_Platform::CpuX86_64>::Pointer *ptr =
        const_cast<volatile bces_AtomicUtilImpl<bsls_Platform::CpuX86_64>::
                                                        Pointer *>(&aPointer);
    return (void*)ptr->d_value;
}

inline
void bces_AtomicUtilImpl<bsls_Platform::CpuX86_64>::setPtr(
    bces_AtomicUtilImpl<bsls_Platform::CpuX86_64>::Pointer *aPointer,
    const void *val)
{
    bces_AtomicUtilImpl_Amd64SetInt64(
                                (volatile bsls_PlatformUtil::Int64*)aPointer,
                                (bsls_PlatformUtil::Int64)val);
}

inline
void bces_AtomicUtilImpl<bsls_Platform::CpuX86_64>::setPtrRelaxed(
             bces_AtomicUtilImpl<bsls_Platform::CpuX86_64>::Pointer *aPointer,
             const void *val)
{
    aPointer->d_value = val;
}

inline
void *bces_AtomicUtilImpl<bsls_Platform::CpuX86_64>::swapPtr(
  bces_AtomicUtilImpl<bsls_Platform::CpuX86_64>::Pointer *aPointer,
  const void *val)
{
    return (void*)bces_AtomicUtilImpl_Amd64SwapInt64(
                       (volatile bsls_PlatformUtil::Int64*)&aPointer->d_value,
                       (bsls_PlatformUtil::Int64)val);
}

inline
void *bces_AtomicUtilImpl<bsls_Platform::CpuX86_64>::testAndSwapPtr(
   bces_AtomicUtilImpl<bsls_Platform::CpuX86_64>::Pointer *aPointer,
   const void *cmpVal, const void *swapVal)
{
    return (void*)bces_AtomicUtilImpl_Amd64TestAndSwap64(
                     (volatile bsls_PlatformUtil::Int64 *)&aPointer->d_value,
                     (bsls_PlatformUtil::Int64)cmpVal,
                     (bsls_PlatformUtil::Int64)swapVal);
}

inline
void bces_AtomicUtilImpl<bsls_Platform::CpuX86_64>::spinLock(
        bces_AtomicUtilImpl<bsls_Platform::CpuX86_64>::SpinLock *aSpin)
{
    bces_AtomicUtilImpl_Amd64SpinLock(&aSpin->d_spin);
}

inline
int bces_AtomicUtilImpl<bsls_Platform::CpuX86_64>::spinTryLock(
      bces_AtomicUtilImpl<bsls_Platform::CpuX86_64>::SpinLock *aSpin,
      int                                                              retries)
{
    return bces_AtomicUtilImpl_Amd64SpinTryLock(&aSpin->d_spin,retries);
}

inline
void bces_AtomicUtilImpl<bsls_Platform::CpuX86_64>::spinUnlock(
        bces_AtomicUtilImpl<bsls_Platform::CpuX86_64>::SpinLock *aSpin)
{
     bces_AtomicUtilImpl_Amd64SpinUnlock(&aSpin->d_spin);
}

}  // close namespace BloombergLP

#endif

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2006
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
