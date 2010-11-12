// bces_atomicutilimpl_intel_pentium.h                                -*-C++-*-
#ifndef INCLUDED_BCES_ATOMICUTILIMPL_INTEL_PENTIUM
#define INCLUDED_BCES_ATOMICUTILIMPL_INTEL_PENTIUM

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: provide atomic operations on Intel Pentium architectures.
//
//@CLASSES:
//   bces_AtomicUtilImpl<bsls_Platform::CpuX86>: Namespace for
//   atomic operations on the Intel Pentium architecture.
//
//@AUTHOR: Ilougino Rocha (irocha)
//
//@DESCRIPTION: This component provides a full specialization of
// 'bces_AtomicUtilImpl' for the intel pentium architecture.  This component is
// not intended to be used directly.  See the 'bces_AtomicUtil' for more on
// using atomic operations.
//

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

#ifdef BSLS_PLATFORM__CPU_X86

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
#pragma warning(push)
#pragma warning(disable:4312)
#pragma warning(disable:4311)
#else
#error Visual Studio 2005 or above is required on Windows
#endif
#endif

namespace BloombergLP {

#if defined(__GNUC__)

// NOTE: GCC uses AT&T assembly syntax, so for all instructions,
// SOURCE comes before DESTINATION.  mov X, Y copies X to Y.

inline
void bces_AtomicUtilImpl_IntelPentiumSetInt(volatile int  *aInt,
                                            int           val)
{
#ifdef __SSE2__
    __asm__ __volatile__ (
                          "movl %1,%0\n\t"
                          "mfence"
                          : "=m"(*aInt)
                          : "r"(val)
                          : "memory");
#else
    __asm__ __volatile__ (
                          "movl %1,%0\n\t"
                          "lock addl $0,0(%%esp)\n\t"
                          : "=m"(*aInt)
                          : "r"(val)
                          : "memory", "cc");
#endif
}

inline
int bces_AtomicUtilImpl_IntelPentiumSwapInt(volatile int *aInt,
                                            int           val)
{
   __asm__ __volatile__ (
                          "lock xchgl %1,%0\n\t"
                          : "=m"(*aInt), "=r"(val)
                          : "1"(val),
                            "m"(*aInt)
                          : "memory");
    return val;
}

inline
int bces_AtomicUtilImpl_IntelPentiumGetInt(volatile const int *aInt)
{
   int ret;
#ifdef __SSE2__
    __asm__ __volatile__ ("mfence\n\t"
                          "movl %1,%0"
                          : "=r"(ret)
                          : "m"(*aInt)
                          : "memory");
#else
    __asm__ __volatile__ ("lock addl $0,0(%%esp)\n\t"
                          "movl %1,%0"
                          : "=r"(ret)
                          : "m"(*aInt)
                          : "memory", "cc");
#endif
    return ret;
}

inline
int bces_AtomicUtilImpl_IntelPentiumAddInt(volatile int *aInt, int val)
{
   const int oldVal = val;
    __asm__ __volatile__ (
                          "lock xaddl %0, %1\n\t"
                          : "=r"(val), "+m" (*aInt)
                          : "0"(val)
                          : "memory",  "cc");

   return val + oldVal;
}

inline
int bces_AtomicUtilImpl_IntelPentiumAddIntRelaxed(volatile int *aInt, int val)
{
   const int oldVal = val;
    __asm__ ("lock xaddl %0, %1\n\t"
             : "=r"(val), "+m" (*aInt)
             : "0"(val)
             : "cc");

   return val + oldVal;
}

inline
int bces_AtomicUtilImpl_IntelPentiumTestAndSwap(volatile int *aInt,
                                                 int           cmpVal,
                                                 int           swapVal)
{
    __asm__ __volatile__ (
                          "lock cmpxchgl %1, %2\n\t"
                          : "+a" (cmpVal)
                          : "r" (swapVal), "m" (*aInt)
                          : "memory", "cc");
   return cmpVal;

}

inline
bsls_PlatformUtil::Int64
bces_AtomicUtilImpl_IntelPentiumSwapInt64(
                                     volatile bsls_PlatformUtil::Int64 *aInt,
                                     bsls_PlatformUtil::Int64           val)
{
    register bsls_PlatformUtil::Int64 result;
    __asm__ __volatile__ (
#ifdef __PIC__
                          "pushl %%ebx      \n\t"
                          "movl  %2, %%ebx  \n\t"
#endif
                          "\n1:\t"
                          "lock cmpxchg8b %1\n\t"
                          "jnz 1b           \n\t"
#ifdef __PIC__
                          "popl %%ebx      \n\t"
#endif
                          : "=A"(result),
                            "+m"(*aInt)
#ifdef __PIC__
                          : "g"((unsigned int)val),
#else
                          : "b"((unsigned int)val),
#endif
                            "c"((int)(val>>32LL)),
                            "A"(*aInt)
                          : "memory", "cc");
    return result;
}

#ifndef __MMX__
inline
void  bces_AtomicUtilImpl_IntelPentiumSetInt64(
                                      volatile bsls_PlatformUtil::Int64 *aInt,
                                      bsls_PlatformUtil::Int64           val)
{
    bces_AtomicUtilImpl_IntelPentiumSwapInt64(aInt, val);
}

inline
void  bces_AtomicUtilImpl_IntelPentiumSetInt64Relaxed(
                                      volatile bsls_PlatformUtil::Int64 *aInt,
                                      bsls_PlatformUtil::Int64           val)
{
    bces_AtomicUtilImpl_IntelPentiumSetInt64(aInt, val);
}

inline
bsls_PlatformUtil::Int64 bces_AtomicUtilImpl_IntelPentiumGetInt64(
                              volatile const bsls_PlatformUtil::Int64 *aInt)
{
    register bsls_PlatformUtil::Int64 result;
    __asm__ __volatile__ (
#ifdef __PIC__
                          "pushl %%ebx      \n\t"
#endif
                          "movl %%ebx, %%eax\n\t"
                          "movl %%ecx, %%edx\n\t"
#if __GNUC__ != 3
                          "lock cmpxchg8b %1\n\t"
#else
                          // gcc 3.4 seems to think that it can take edx as %1.

                          "lock cmpxchg8b (%1)\n\t"
#endif
#ifdef __PIC__
                          "popl %%ebx      \n\t"
#endif
                          : "=&A"(result)
                          :
#if __GNUC__ != 3
                            "m"(*aInt),
#else
                            "S"(aInt),
#endif
                            "0"(0)
                          :
#ifndef __PIC__
                            "ebx",
#endif
                            "ecx", "cc", "memory");
    return result;
}

inline
bsls_PlatformUtil::Int64 bces_AtomicUtilImpl_IntelPentiumGetInt64Relaxed(
                              volatile const bsls_PlatformUtil::Int64 *aInt)
{
    return bces_AtomicUtilImpl_IntelPentiumGetInt64(aInt);
}

#elif defined(__SSE__)
inline
bsls_PlatformUtil::Int64 bces_AtomicUtilImpl_IntelPentiumGetInt64(
                              volatile const bsls_PlatformUtil::Int64 *aInt)
{
    register bsls_PlatformUtil::Int64 result;
    __asm__ __volatile__ (
                          "lock addl $0,0(%%esp)\n\t"  // barrier
                          "movq %1,%0\n\t"
                          : "=x"(result)
                          : "m"(*aInt)
                          : "cc", "memory");
    return result;
}

inline
void  bces_AtomicUtilImpl_IntelPentiumSetInt64(
                                      volatile bsls_PlatformUtil::Int64 *aInt,
                                      bsls_PlatformUtil::Int64           val)
{
    __asm__ __volatile__ (
                          "movq %1,%0\n\t"
                          "lock addl $0,0(%%esp)\n\t"  // barrier
                          : "=m"(*aInt)
                          : "x"(val)
                          : "cc", "memory");
}
inline
bsls_PlatformUtil::Int64 bces_AtomicUtilImpl_IntelPentiumGetInt64Relaxed(
                              volatile const bsls_PlatformUtil::Int64 *aInt)
{
    register bsls_PlatformUtil::Int64 result;
    __asm__ __volatile__ (
                          "movq %1,%0\n\t"
                          : "=x"(result)
                          : "m"(*aInt)
                          : );
    return result;
}

inline
void  bces_AtomicUtilImpl_IntelPentiumSetInt64Relaxed(
                                      volatile bsls_PlatformUtil::Int64 *aInt,
                                      bsls_PlatformUtil::Int64           val)
{
    __asm__ __volatile__ (
                          "movq %1,%0\n\t"
                          : "=m"(*aInt)
                          : "x"(val)
                          : );
}
#else

// In the cpp, can't be inlined because they use MMX instructions.

bsls_PlatformUtil::Int64 bces_AtomicUtilImpl_IntelPentiumGetInt64(
                              volatile const bsls_PlatformUtil::Int64 *aInt);

bsls_PlatformUtil::Int64 bces_AtomicUtilImpl_IntelPentiumGetInt64Relaxed(
                              volatile const bsls_PlatformUtil::Int64 *aInt);

void  bces_AtomicUtilImpl_IntelPentiumSetInt64(
                                      volatile bsls_PlatformUtil::Int64 *aInt,
                                      bsls_PlatformUtil::Int64           val);

void  bces_AtomicUtilImpl_IntelPentiumSetInt64Relaxed(
                                      volatile bsls_PlatformUtil::Int64 *aInt,
                                      bsls_PlatformUtil::Int64           val);
#endif

inline
bsls_PlatformUtil::Int64
bces_AtomicUtilImpl_IntelPentiumAddInt64(
                                    volatile bsls_PlatformUtil::Int64 *aInt,
                                    bsls_PlatformUtil::Int64           val)
{
    register bsls_PlatformUtil::Int64 result;
    __asm__ __volatile__ (
#ifdef __PIC__
                          "pushl %%ebx      \n\t"
#endif
                          "\n1:\t"
                          "movl %%eax, %%ebx\n\t"
                          "movl %%edx, %%ecx\n\t"
                          "addl (%2), %%ebx\n\t"
                          "adcl 4(%2), %%ecx\n\t"
                          "lock cmpxchg8b (%1)\n\t"
                          "jnz 1b\n\t"
                          "movl %%ebx, %%eax\n\t"
                          "movl %%ecx, %%edx\n\t"
#ifdef __PIC__
                          "popl %%ebx      \n\t"
#endif
                          : "=&A"(result),
                            "+D"(aInt) // should be m but this breaks gcc 4
                          : "S"(&val),  // should be "m" but this breaks gcc
                                        // 3.4.2
                            "0" (*aInt)
                          :
#ifndef __PIC__
                            "ebx",
#endif
                            "ecx","cc","memory");
   return result;
}

inline
bsls_PlatformUtil::Int64 bces_AtomicUtilImpl_IntelPentiumTestAndSwap64(
                                 volatile bsls_PlatformUtil::Int64 *aInt,
                                 bsls_PlatformUtil::Int64           cmpVal,
                                 bsls_PlatformUtil::Int64           swapVal)
{
    register bsls_PlatformUtil::Int64 result;
    __asm__ __volatile__ (
#ifdef __PIC__
                          "pushl %%ebx      \n\t"
                          "movl  %2, %%ebx \n\t"
#endif
                          "lock cmpxchg8b %1\n\t"
#ifdef __PIC__
                          "popl %%ebx      \n\t"
#endif
                          : "=A"(result),
                            "+m"(*aInt)
                          :
#ifdef __PIC__
                            "g"((unsigned int)swapVal),
#else
                            "b"((unsigned int)swapVal),
#endif
                            "c"((int)(swapVal>>32)),
                            "0"(cmpVal)
                          : "cc", "memory");
   return result;
}

inline
void bces_AtomicUtilImpl_IntelPentiumSpinLock(volatile int *aSpin)
{
    register int result;
    __asm__ __volatile__("1:\n\t"
                        "xorb %%al, %%al\n\t"
                        "lock cmpxchgb %%ah, %0; \n\t"
                        "jnz 1b\n\t"
                       : "+m"(*(volatile char*)aSpin), "=a" (result)
                       : "1" (0xff00)
                       : "memory", "cc");
}

inline
int bces_AtomicUtilImpl_IntelPentiumSpinTryLock(volatile int *aSpin,
                                                int            retries)
{
    register int result;
    __asm__ __volatile__ (
                          "\n1:\t"
                          "xorb %%al,%%al\n\t"
                          "lock cmpxchgb %%ah,%1\n\t"
                          "loopnzl 1b\n\t"
                          "xorb %%ah,%%ah\n\t"
                          : "=a"(result), "+m" (*(volatile char*)aSpin),
                            "+c"(retries)
                          : "0" (0xff00)
                          : "memory", "cc");
   return result;
}

inline
void bces_AtomicUtilImpl_IntelPentiumSpinUnlock(volatile int *aSpin)
{
    register char result;
    __asm__ __volatile__ (
                          "lock xchgb %b1,%0\n\t"
                          : "+m" (*(volatile char*)aSpin), "=q" (result)
                          : "1" (0)
                          :  "memory");
}

#elif defined(BSLS_PLATFORM__OS_SOLARIS)

extern "C" {

int bces_AtomicUtilImpl_IntelPentiumSwapInt(volatile int *aInt,
                                             int           val);

int bces_AtomicUtilImpl_IntelPentiumAddInt(volatile int *aInt,
                                            int           val);

int bces_AtomicUtilImpl_IntelPentiumTestAndSwap(volatile int *aInt,
                                                 int           cmpVal,
                                                 int           swapVal);

int bces_AtomicUtilImpl_IntelPentiumGetInt(volatile const int *aInt);

bsls_PlatformUtil::Int64 bces_AtomicUtilImpl_IntelPentiumSwapInt64(
                                  volatile bsls_PlatformUtil::Int64 *aInt,
                                  bsls_PlatformUtil::Int64           val);

bsls_PlatformUtil::Int64 bces_AtomicUtilImpl_IntelPentiumGetInt64(
                                  volatile const bsls_PlatformUtil::Int64*);

bsls_PlatformUtil::Int64 bces_AtomicUtilImpl_IntelPentiumAddInt64(
                                  volatile bsls_PlatformUtil::Int64 *aInt,
                                  bsls_PlatformUtil::Int64           val);

bsls_PlatformUtil::Int64 bces_AtomicUtilImpl_IntelPentiumTestAndSwap64(
                                  volatile bsls_PlatformUtil::Int64 *aInt,
                                  bsls_PlatformUtil::Int64           cmpVal,
                                  bsls_PlatformUtil::Int64           swapVal);

void bces_AtomicUtilImpl_IntelPentiumSpinLock(volatile int *aSpin);
int  bces_AtomicUtilImpl_IntelPentiumSpinTryLock(
                                 volatile char *aSpin, int retries);
void bces_AtomicUtilImpl_IntelPentiumSpinUnlock(
                                              volatile char *aSpin);
}

#elif defined(BSLS_PLATFORM__OS_WINDOWS)

// using compiler intrinsics on Windows

__forceinline void bces_AtomicUtilImpl_IntelPentiumSetInt(
        volatile int *aInt,
        int val)
{
    // volatile memory access is fully serialized by VS2005+
    *aInt = val;
    __asm lock add dword ptr [esp], 0;
}

__forceinline int bces_AtomicUtilImpl_IntelPentiumGetInt(
        volatile const int *aInt)
{
    __asm lock add dword ptr [esp], 0;
    // volatile memory access is fully serialized by VS2005+
    return( *aInt);
}

inline int bces_AtomicUtilImpl_IntelPentiumAddInt(
        volatile int *aInt,
        int val)
{
    return _InterlockedExchangeAdd(reinterpret_cast<volatile long*>(aInt), val)
            + val;
}

inline int bces_AtomicUtilImpl_IntelPentiumAddIntRelaxed(
        volatile int *aInt,
        int val)
{
    // no way to make this Relaxed, it seems...same implementation as
    // PentiumAddInt
    return _InterlockedExchangeAdd(reinterpret_cast<volatile long*>(aInt), val)
            + val;
}

inline int bces_AtomicUtilImpl_IntelPentiumSwapInt(volatile int *aInt,
                                            int           val)
{
    return _InterlockedExchange(reinterpret_cast<volatile long*>(aInt), val);
}

inline int bces_AtomicUtilImpl_IntelPentiumTestAndSwap(
        volatile int *aInt,
        int           cmpVal,
        int           swapVal)
{
    return _InterlockedCompareExchange(reinterpret_cast<volatile long*>(aInt),
                                       swapVal, cmpVal);
}

inline bsls_PlatformUtil::Int64 bces_AtomicUtilImpl_IntelPentiumSwapInt64(
    volatile bsls_PlatformUtil::Int64 *aInt,
    bsls_PlatformUtil::Int64           val)
{
    bsls_PlatformUtil::Int64 old = *aInt;
    bsls_PlatformUtil::Int64 tmp;
    while(old != (tmp = _InterlockedCompareExchange64(aInt, val, old))) {
        old = tmp;
        _mm_pause();
    }
    return old;
}

inline void bces_AtomicUtilImpl_IntelPentiumSetInt64(
    volatile bsls_PlatformUtil::Int64 *aInt,
    bsls_PlatformUtil::Int64           val)
{
    bces_AtomicUtilImpl_IntelPentiumSwapInt64(aInt, val);
}

inline bsls_PlatformUtil::Int64 bces_AtomicUtilImpl_IntelPentiumGetInt64(
        volatile const bsls_PlatformUtil::Int64*aInt)
{
    return _InterlockedCompareExchange64(
                         const_cast<volatile bsls_PlatformUtil::Int64*>(aInt),
                         0, 0);
}

inline bsls_PlatformUtil::Int64 bces_AtomicUtilImpl_IntelPentiumAddInt64(
        volatile bsls_PlatformUtil::Int64 *aInt,
        bsls_PlatformUtil::Int64           val )
{
    bsls_PlatformUtil::Int64 old = *aInt;
    bsls_PlatformUtil::Int64 tmp;
    while(old != (tmp = _InterlockedCompareExchange64(aInt, old + val, old))) {
        old = tmp;
        _mm_pause();
    }
    return old + val;
}

inline bsls_PlatformUtil::Int64 bces_AtomicUtilImpl_IntelPentiumTestAndSwap64(
        volatile bsls_PlatformUtil::Int64 *aInt,
        bsls_PlatformUtil::Int64           cmpVal,
        bsls_PlatformUtil::Int64           swapVal)
{
    return _InterlockedCompareExchange64(aInt, swapVal, cmpVal);
}

inline void bces_AtomicUtilImpl_IntelPentiumSpinLock(
        volatile int *aSpin)
{
    do {
        if (*aSpin) { _mm_pause(); continue; }
    } while(_InterlockedCompareExchange(
                               reinterpret_cast<volatile long*>(aSpin), 1, 0));
}

inline int  bces_AtomicUtilImpl_IntelPentiumSpinTryLock(
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
    } while(_InterlockedCompareExchange(
                reinterpret_cast<volatile long*>(aSpin), 1, 0));
    return 0;
}

__forceinline void bces_AtomicUtilImpl_IntelPentiumSpinUnlock(
        volatile int *aSpin)
{
    // volatile memory access is fully serialized by VS2005+
    *aSpin = 0;
    __asm lock add dword ptr [esp], 0;
}

#endif

template <class PLATFORM> struct bces_AtomicUtilImpl;

template <>
struct bces_AtomicUtilImpl<bsls_Platform::CpuX86> {
    // Namespace for atomic operations on the Intel Pentium architecture.

    // TYPES
    struct SpinLock {
#if defined(BSLS_PLATFORM__CMP_GNU)
        volatile int d_spin __attribute__((__aligned__(sizeof(int))));
#elif defined(BSLS_PLATFORM__CMP_MSVC)
        __declspec(align(4)) volatile int d_spin;
#else
        volatile int d_spin;
#endif
    };

    struct Int {
#if defined(BSLS_PLATFORM__CMP_GNU)
        volatile int d_value __attribute__((__aligned__(sizeof(int))));
#elif defined(BSLS_PLATFORM__CMP_MSVC)
        __declspec(align(4)) volatile int d_value;
#else
        volatile int d_value;
#endif
    };

    struct Int64 {
#if defined(BSLS_PLATFORM__CMP_GNU)
        volatile bsls_PlatformUtil::Int64 d_value
                __attribute__((__aligned__(sizeof(bsls_PlatformUtil::Int64))));
#elif defined(BSLS_PLATFORM__CMP_MSVC)
        __declspec(align(8)) volatile bsls_PlatformUtil::Int64 d_value;
#else
        volatile bsls_PlatformUtil::Int64 d_value;
#endif
    };

    struct Pointer {
#if defined(BSLS_PLATFORM__CMP_GNU)
        const void * volatile d_value
                                  __attribute__((__aligned__(sizeof(void *))));
#elif defined(BSLS_PLATFORM__CMP_MSVC)
        __declspec(align(4)) const void * volatile d_value;
#else
        const void * volatile d_value;
#endif
    };

    static void initInt(
          bces_AtomicUtilImpl<bsls_Platform::CpuX86>::Int *aInt,
          int initialValue);
        // Initialize the specified atomic integer 'aInt', and set its value to
        // the specified 'initialValue'.  Note that this method must be called
        // before any other operations on 'aInt'.

    static void initInt64(
        bces_AtomicUtilImpl<bsls_Platform::CpuX86>::Int64 *aInt,
        bsls_PlatformUtil::Int64  initialValue);
        // Initialize the specified atomic integer 'aInt', and set its value to
        // the specified 'initialValue'.  Note that this method must be called
        // before any other operations on 'aInt'.

    static void initPointer(
        bces_AtomicUtilImpl<bsls_Platform::CpuX86>::Pointer *aPtr,
        const void *initialValue);
        // Initialize the specified atomic pointer 'aPointer', and set its
        // value to the specified 'initialValue'.  Note that this function must
        // be called before any other operations on 'aPointer'.

    static void initSpinLock(
      bces_AtomicUtilImpl<bsls_Platform::CpuX86>::SpinLock *aSpin);
        // Initialize the specified spin lock 'aSpin' and set it to an an
        // unlocked state.  Note that method must be called before any other
        // operations on 'aSpin'.

    static void addInt(
        bces_AtomicUtilImpl<bsls_Platform::CpuX86>::Int *aInt,
        int val);
        // Atomically add to the specified 'aInt' the specified 'value'.  Note
        // that the behavior is undefined if 'aInt' is 0.

    static void incrementInt(
         bces_AtomicUtilImpl<bsls_Platform::CpuX86>::Int *aInt);
        // Atomically increment the value of the specified 'aInt' by 1.  Note
        // that the behavior is undefined if 'aInt' is 0.

    static void decrementInt(
         bces_AtomicUtilImpl<bsls_Platform::CpuX86>::Int *aInt);
        // Atomically decrement the value of the specified 'aInt' by 1.  Note
        // that the behavior is undefined if 'aInt' is 0.

    static int swapInt(
         bces_AtomicUtilImpl<bsls_Platform::CpuX86>::Int *aInt,
         int val);
        // Atomically set the value of the specified 'aInt' to the specified
        // 'value' and return its previous value.  Note that the behavior is
        // undefined if 'aInt' is 0.

    static int testAndSwapInt(
          bces_AtomicUtilImpl<bsls_Platform::CpuX86>::Int *aInt,
          int cmpVal, int swapVal);
        // Conditionally set the value of the specified 'aInt' to the specified
        // 'swapVal' and return its previous value.  The value of 'aInt' is
        // compared to the specified 'cmpVal'; if they are equal, then
        // 'swapVal' will be assigned to 'aInt', otherwise the value of 'aInt'
        // is left unchanged.  The whole operation is performed atomically.
        // Note that the behavior is undefined if 'aInt' is 0.

    static void setInt(
          bces_AtomicUtilImpl<bsls_Platform::CpuX86>::Int *aInt,
          int val);
        // Atomically set the value of the specified 'aInt' to the specified
        // 'value'.  Note that the behavior is undefined if 'aInt' is 0.

    static void setIntRelaxed(
          bces_AtomicUtilImpl<bsls_Platform::CpuX86>::Int *aInt,
          int val);
        // Atomically set the value of the specified 'aInt' to the specified
        // 'value' without additional ordering constraints.  The behavior is
        // undefined if 'aInt' is 0.

    static int getInt(
       const bces_AtomicUtilImpl<bsls_Platform::CpuX86>::Int& aInt);
        // Atomically retrieve the value of the specified 'aInt'.

    static int getIntRelaxed(
       const bces_AtomicUtilImpl<bsls_Platform::CpuX86>::Int& aInt);
        // Atomically retrieve the value of the specified 'aInt' without
        // additional ordering constraints.

    static int addIntNv(
             bces_AtomicUtilImpl<bsls_Platform::CpuX86>::Int *aInt,
             int val);
        // Atomically add to the specified 'aInt' the specified 'value' and
        // return the resulting value.  The behavior is undefined if 'aInt'
        // is 0.

    static int addIntNvRelaxed(
             bces_AtomicUtilImpl<bsls_Platform::CpuX86>::Int *aInt,
             int val);
        // Atomically add to the specified 'aInt' the specified 'value' and
        // return the resulting value, without additional ordering constraints.
        // The behavior is undefined if 'aInt' is 0.

    static int incrementIntNv(
             bces_AtomicUtilImpl<bsls_Platform::CpuX86>::Int *aInt);
        // Atomically increment the specified 'aInt' by 1 and return the
        // resulting value.  The behavior is undefined if 'aInt' is 0.

    static int decrementIntNv(
             bces_AtomicUtilImpl<bsls_Platform::CpuX86>::Int *aInt);
        // Atomically decrement the specified 'aInt' by 1 and return the
        // resulting value.  Note that the behavior is undefined if 'aInt' is
        // 0.

    static void addInt64(
         bces_AtomicUtilImpl<bsls_Platform::CpuX86>::Int64 *aInt,
         bsls_PlatformUtil::Int64 val);
        // Atomically add to the specified 'aInt' the specified 'value'.  Note
        // that the behavior is undefined if 'aInt' is 0.

    static void incrementInt64(
         bces_AtomicUtilImpl<bsls_Platform::CpuX86>::Int64 *aInt);
        // Atomically increment the value of the specified 'aInt' by 1.  Note
        // that the behavior is undefined if 'aInt' is 0.

    static void decrementInt64(
         bces_AtomicUtilImpl<bsls_Platform::CpuX86>::Int64 *aInt);
        // Atomically decrement the value of the specified 'aInt' by 1.  Note
        // that the behavior is undefined if 'aInt' is 0.

    static bsls_PlatformUtil::Int64 swapInt64(
         bces_AtomicUtilImpl<bsls_Platform::CpuX86>::Int64 *aInt,
         bsls_PlatformUtil::Int64 val);
        // Atomically set the value of the specified 'aInt' to the specified
        // 'value' and return its previous value.  Note that the behavior is
        // undefined if 'aInt' is 0.

    static bsls_PlatformUtil::Int64 testAndSwapInt64(
         bces_AtomicUtilImpl<bsls_Platform::CpuX86>::Int64 *aInt,
         bsls_PlatformUtil::Int64  cmpVal,
         bsls_PlatformUtil::Int64  swapVal);
        // Conditionally set the value of the specified 'aInt' to the specified
        // 'swapVal' and return its previous value.  The value of 'aInt' is
        // compared to the specified 'cmpVal'; if they are equal, then
        // 'swapVal' will be assigned to 'aInt' otherwise, the value of 'aInt'
        // is left unchanged.  The whole operation is performed atomically.
        // Note that the behavior is undefined if 'aInt' is 0.

    static bsls_PlatformUtil::Int64 addInt64Nv(
         bces_AtomicUtilImpl<bsls_Platform::CpuX86>::Int64 *aInt,
         bsls_PlatformUtil::Int64 val);
        // Atomically add to the specified 'aInt' the specified 'value' and
        // return the resulting value.  Note that the behavior is undefined if
        // 'aInt' is 0.

    static bsls_PlatformUtil::Int64 addInt64NvRelaxed(
         bces_AtomicUtilImpl<bsls_Platform::CpuX86>::Int64 *aInt,
         bsls_PlatformUtil::Int64 val);
        // Atomically add to the specified 'aInt' the specified 'value' and
        // return the resulting value.  The behavior is undefined if 'aInt' is
        // 0.  Note that this method is identical to 'addInt64Nv' for this
        // platform.

    static bsls_PlatformUtil::Int64 incrementInt64Nv(
         bces_AtomicUtilImpl<bsls_Platform::CpuX86>::Int64 *aInt);
        // Atomically increment the specified 'aInt' by 1 and return the
        // resulting value.  The behavior is undefined if 'aInt' is 0.

    static bsls_PlatformUtil::Int64 decrementInt64Nv(
         bces_AtomicUtilImpl<bsls_Platform::CpuX86>::Int64 *aInt);
        // Atomically decrement the specified 'aInt' by 1 and return the
        // resulting value.  Note that the behavior is undefined if 'aInt' is
        // 0.

    static void setInt64(
         bces_AtomicUtilImpl<bsls_Platform::CpuX86>::Int64 *aInt,
         bsls_PlatformUtil::Int64 val);
        // Atomically set the value of the specified 'aInt' to the specified
        // 'value'.  The behavior is undefined if 'aInt' is 0.

    static void setInt64Relaxed(
         bces_AtomicUtilImpl<bsls_Platform::CpuX86>::Int64 *aInt,
         bsls_PlatformUtil::Int64 val);
        // Atomically set the value of the specified 'aInt' to the specified
        // 'value'.  Note that this method is identical to 'setInt64' for this
        // platform.

    static bsls_PlatformUtil::Int64 getInt64(
               const bces_AtomicUtilImpl<bsls_Platform::CpuX86>::Int64& aInt);
        // Atomically retrieve the value of the specified 'aInt'.

    static bsls_PlatformUtil::Int64 getInt64Relaxed(
               const bces_AtomicUtilImpl<bsls_Platform::CpuX86>::Int64& aInt);
        // Atomically retrieve the value of the specified 'aInt'.  Note that
        // this method is identical to 'getInt64' for this platform.

    static void *getPtr(
          const bces_AtomicUtilImpl<bsls_Platform::CpuX86>::Pointer& aPointer);
        // Atomically retrieve the value of the specified 'aPointer'.

    static void *getPtrRelaxed(
          const bces_AtomicUtilImpl<bsls_Platform::CpuX86>::Pointer& aPointer);
        // Atomically retrieve the value of the specified 'aPointer' without
        // additional ordering constraints.

    static void setPtr(
     bces_AtomicUtilImpl<bsls_Platform::CpuX86>::Pointer *aPointer,
     const void *val);
        // Atomically set the value of the specified 'aPointer' to the
        // specified 'value'.  The behavior is undefined if 'aPointer' is 0.

    static void setPtrRelaxed(
               bces_AtomicUtilImpl<bsls_Platform::CpuX86>::Pointer *aPointer,
               const void *val);
        // Atomically set the value of the specified 'aPointer' to the
        // specified 'value' without additional ordering constraints.  The
        // behavior is undefined if 'aPointer' is 0.

    static void *swapPtr(
     bces_AtomicUtilImpl<bsls_Platform::CpuX86>::Pointer *aPointer,
     const void *val);
        // Atomically set the value of the specified 'aPointer' to the
        // specified 'value' and return its previous value.  Note that the
        // behavior is undefined if 'aPointer' is 0.

    static void *testAndSwapPtr(
     bces_AtomicUtilImpl<bsls_Platform::CpuX86>::Pointer *aPointer,
     const void *cmpVal,
     const void *swapVal);
        // Conditionally set the value of the specified 'aPointer' to the
        // specified 'swapVal' and return its previous value.  The value of
        // 'aPointer' is compared to the specified 'cmpVal'; if they are equal,
        // then 'swapVal' will be assigned to 'aPointer' otherwise, the value
        // of 'aPointer' is left unchanged.  The whole operation is performed
        // atomically.  Note that the behavior is undefined if 'aPointer' is 0.

    static void spinLock(
    bces_AtomicUtilImpl<bsls_Platform::CpuX86>::SpinLock *aSpin);
        // Lock the specified 'aSpin'.  Note that the behavior is undefined if
        // 'aSpin' is 0.

    static int spinTryLock(
    bces_AtomicUtilImpl<bsls_Platform::CpuX86>::SpinLock *aSpin,
    int retries);
        // Attempt to lock the specified 'aSpin' up to 'retries' times.  Return
        // 0 on success, non-zero otherwise.  Note that the behavior is
        // undefined if 'aSpin' is 0 or 0 <= retries.

    static void spinUnlock(
    bces_AtomicUtilImpl<bsls_Platform::CpuX86>::SpinLock *aSpin);
        // Unlock the specified 'aSpin' which was previously locked by a
        // successful call to 'spinLock' or 'spinTryLock'.  Note that the
        // behavior is undefined if 'aSpin' is 0.
};

inline
void bces_AtomicUtilImpl<bsls_Platform::CpuX86>::initInt(
              bces_AtomicUtilImpl<bsls_Platform::CpuX86>::Int *aInt,
              int initialValue)
{
    BSLS_ASSERT_OPT(bsls_AlignmentUtil::is4ByteAligned(aInt));
    aInt->d_value = initialValue;
}

inline
void bces_AtomicUtilImpl<bsls_Platform::CpuX86>::initInt64(
            bces_AtomicUtilImpl<bsls_Platform::CpuX86>::Int64 *aInt,
            bsls_PlatformUtil::Int64  initialValue)
{
    BSLS_ASSERT_OPT(bsls_AlignmentUtil::is8ByteAligned(aInt));
    aInt->d_value = initialValue;
}

inline
void bces_AtomicUtilImpl<bsls_Platform::CpuX86>::initPointer(
          bces_AtomicUtilImpl<bsls_Platform::CpuX86>::Pointer *aPtr,
          const void *initialValue )
{
    BSLS_ASSERT_OPT(bsls_AlignmentUtil::is4ByteAligned(aPtr));
    aPtr->d_value = initialValue;
}

inline
void bces_AtomicUtilImpl<bsls_Platform::CpuX86>::initSpinLock(
       bces_AtomicUtilImpl<bsls_Platform::CpuX86>::SpinLock *aSpin)
{
    BSLS_ASSERT_OPT(bsls_AlignmentUtil::is4ByteAligned(aSpin));
    aSpin->d_spin = 0;
}

inline
void bces_AtomicUtilImpl<bsls_Platform::CpuX86>::addInt(
    bces_AtomicUtilImpl<bsls_Platform::CpuX86>::Int *aInt, int val)
{
    bces_AtomicUtilImpl_IntelPentiumAddInt(&aInt->d_value, val);
}

inline
void bces_AtomicUtilImpl<bsls_Platform::CpuX86>::incrementInt(
              bces_AtomicUtilImpl<bsls_Platform::CpuX86>::Int *aInt)
{
    bces_AtomicUtilImpl_IntelPentiumAddInt(&aInt->d_value, 1);
}

inline
void bces_AtomicUtilImpl<bsls_Platform::CpuX86>::decrementInt(
              bces_AtomicUtilImpl<bsls_Platform::CpuX86>::Int *aInt)
{
    bces_AtomicUtilImpl_IntelPentiumAddInt(&aInt->d_value, -1);
}

inline
int bces_AtomicUtilImpl<bsls_Platform::CpuX86>::swapInt(
              bces_AtomicUtilImpl<bsls_Platform::CpuX86>::Int *aInt,
              int val)
{
    return bces_AtomicUtilImpl_IntelPentiumSwapInt(&aInt->d_value, val);
}

inline
int bces_AtomicUtilImpl<bsls_Platform::CpuX86>::testAndSwapInt(
              bces_AtomicUtilImpl<bsls_Platform::CpuX86>::Int *aInt,
              int cmpVal, int swapVal)
{
    return bces_AtomicUtilImpl_IntelPentiumTestAndSwap(&aInt->d_value,cmpVal,
                                                       swapVal);
}

inline
void bces_AtomicUtilImpl<bsls_Platform::CpuX86>::setInt(
             bces_AtomicUtilImpl<bsls_Platform::CpuX86>::Int *aInt,
             int val)
{
#if defined(__GNUC__)
    bces_AtomicUtilImpl_IntelPentiumSetInt(&aInt->d_value, val);
#else
    bces_AtomicUtilImpl_IntelPentiumSwapInt(&aInt->d_value, val);
#endif
}

inline
void bces_AtomicUtilImpl<bsls_Platform::CpuX86>::setIntRelaxed(
             bces_AtomicUtilImpl<bsls_Platform::CpuX86>::Int *aInt,
             int val)
{
    aInt->d_value = val;
}

inline
int  bces_AtomicUtilImpl<bsls_Platform::CpuX86>::getInt(
        const bces_AtomicUtilImpl<bsls_Platform::CpuX86>::Int& aInt)
{
    return bces_AtomicUtilImpl_IntelPentiumGetInt(&aInt.d_value);
}

inline
int  bces_AtomicUtilImpl<bsls_Platform::CpuX86>::getIntRelaxed(
        const bces_AtomicUtilImpl<bsls_Platform::CpuX86>::Int& aInt)
{
    return aInt.d_value;
}

inline
int bces_AtomicUtilImpl<bsls_Platform::CpuX86>::addIntNv(
    bces_AtomicUtilImpl<bsls_Platform::CpuX86>::Int *aInt, int val)
{
    return bces_AtomicUtilImpl_IntelPentiumAddInt(&aInt->d_value, val);
}

inline
int bces_AtomicUtilImpl<bsls_Platform::CpuX86>::addIntNvRelaxed(
    bces_AtomicUtilImpl<bsls_Platform::CpuX86>::Int *aInt, int val)
{
    return bces_AtomicUtilImpl_IntelPentiumAddIntRelaxed(&aInt->d_value, val);
}

inline
int bces_AtomicUtilImpl<bsls_Platform::CpuX86>::incrementIntNv(
             bces_AtomicUtilImpl<bsls_Platform::CpuX86>::Int *aInt)
{
    return bces_AtomicUtilImpl_IntelPentiumAddInt(&aInt->d_value, 1);
}

inline
int bces_AtomicUtilImpl<bsls_Platform::CpuX86>::decrementIntNv(
              bces_AtomicUtilImpl<bsls_Platform::CpuX86>::Int *aInt)
{
    return bces_AtomicUtilImpl_IntelPentiumAddInt(&aInt->d_value, -1);
}

inline
void bces_AtomicUtilImpl<bsls_Platform::CpuX86>::addInt64(
            bces_AtomicUtilImpl<bsls_Platform::CpuX86>::Int64 *aInt,
            bsls_PlatformUtil::Int64 val)
{
    bces_AtomicUtilImpl_IntelPentiumAddInt64(&aInt->d_value, val);
}

inline
void bces_AtomicUtilImpl<bsls_Platform::CpuX86>::incrementInt64(
            bces_AtomicUtilImpl<bsls_Platform::CpuX86>::Int64 *aInt)
{
    bces_AtomicUtilImpl_IntelPentiumAddInt64(&aInt->d_value, 1);
}

inline
void bces_AtomicUtilImpl<bsls_Platform::CpuX86>::decrementInt64(
            bces_AtomicUtilImpl<bsls_Platform::CpuX86>::Int64 *aInt)
{
    bces_AtomicUtilImpl_IntelPentiumAddInt64(&aInt->d_value, -1);
}

inline
bsls_PlatformUtil::Int64
bces_AtomicUtilImpl<bsls_Platform::CpuX86>::swapInt64(
            bces_AtomicUtilImpl<bsls_Platform::CpuX86>::Int64 *aInt,
            bsls_PlatformUtil::Int64 val)
{
    return bces_AtomicUtilImpl_IntelPentiumSwapInt64(&aInt->d_value, val);
}

inline
bsls_PlatformUtil::Int64
bces_AtomicUtilImpl<bsls_Platform::CpuX86>::testAndSwapInt64(
                                                                  Int64 *aInt,
                                              bsls_PlatformUtil::Int64 cmpVal,
                                              bsls_PlatformUtil::Int64 swapVal)
{
    return bces_AtomicUtilImpl_IntelPentiumTestAndSwap64(&aInt->d_value,cmpVal
                                                         ,swapVal);
}

inline
bsls_PlatformUtil::Int64
  bces_AtomicUtilImpl<bsls_Platform::CpuX86>::addInt64Nv(
         bces_AtomicUtilImpl<bsls_Platform::CpuX86>::Int64 *aInt,
         bsls_PlatformUtil::Int64 val)
{
    return bces_AtomicUtilImpl_IntelPentiumAddInt64(&aInt->d_value, val);
}

inline
bsls_PlatformUtil::Int64
  bces_AtomicUtilImpl<bsls_Platform::CpuX86>::addInt64NvRelaxed(
         bces_AtomicUtilImpl<bsls_Platform::CpuX86>::Int64 *aInt,
         bsls_PlatformUtil::Int64 val)
{
    return bces_AtomicUtilImpl_IntelPentiumAddInt64(&aInt->d_value, val);
}

inline
bsls_PlatformUtil::Int64
bces_AtomicUtilImpl<bsls_Platform::CpuX86>::incrementInt64Nv(
            bces_AtomicUtilImpl<bsls_Platform::CpuX86>::Int64 *aInt)
{
    return bces_AtomicUtilImpl_IntelPentiumAddInt64(&aInt->d_value, 1);
}

inline
bsls_PlatformUtil::Int64
bces_AtomicUtilImpl<bsls_Platform::CpuX86>::decrementInt64Nv(
            bces_AtomicUtilImpl<bsls_Platform::CpuX86>::Int64 *aInt)
{
    return bces_AtomicUtilImpl_IntelPentiumAddInt64(&aInt->d_value, -1);
}

inline
void bces_AtomicUtilImpl<bsls_Platform::CpuX86>::setInt64(
            bces_AtomicUtilImpl<bsls_Platform::CpuX86>::Int64 *aInt,
            bsls_PlatformUtil::Int64 val)
{
#if defined(__GNUC__)
    bces_AtomicUtilImpl_IntelPentiumSetInt64(&aInt->d_value, val);
#else
    bces_AtomicUtilImpl_IntelPentiumSwapInt64(&aInt->d_value, val);
#endif
}

inline
void bces_AtomicUtilImpl<bsls_Platform::CpuX86>::setInt64Relaxed(
            bces_AtomicUtilImpl<bsls_Platform::CpuX86>::Int64 *aInt,
            bsls_PlatformUtil::Int64 val)
{
#if defined(__GNUC__)
    bces_AtomicUtilImpl_IntelPentiumSetInt64Relaxed(&aInt->d_value, val);
#else
    bces_AtomicUtilImpl_IntelPentiumSwapInt64(&aInt->d_value, val);
#endif
}

inline
bsls_PlatformUtil::Int64
bces_AtomicUtilImpl<bsls_Platform::CpuX86>::getInt64(
      const bces_AtomicUtilImpl<bsls_Platform::CpuX86>::Int64& aInt)
{

    return bces_AtomicUtilImpl_IntelPentiumGetInt64(&aInt.d_value);
}

inline
bsls_PlatformUtil::Int64
bces_AtomicUtilImpl<bsls_Platform::CpuX86>::getInt64Relaxed(
      const bces_AtomicUtilImpl<bsls_Platform::CpuX86>::Int64& aInt)
{
#if defined(__GNUC__)
    return bces_AtomicUtilImpl_IntelPentiumGetInt64Relaxed(&aInt.d_value);
#else
    return bces_AtomicUtilImpl_IntelPentiumGetInt64(&aInt.d_value);
#endif
}

inline
void *bces_AtomicUtilImpl<bsls_Platform::CpuX86>::getPtr(
const bces_AtomicUtilImpl<bsls_Platform::CpuX86>::Pointer& aPointer)
{
#ifdef BSLS_PLATFORM__CPU_64_BIT
    return (void*)bces_AtomicUtilImpl_IntelPentiumGetInt64(
                        (volatile bsls_PlatformUtil::Int64*)&aPointer.d_value);
#else
    return (void*)bces_AtomicUtilImpl_IntelPentiumGetInt(
                                        (volatile int*)&aPointer.d_value);
#endif
}

inline
void *bces_AtomicUtilImpl<bsls_Platform::CpuX86>::getPtrRelaxed(
         const bces_AtomicUtilImpl<bsls_Platform::CpuX86>::Pointer& aPointer)
{
    // This ugly cast is necessary because otherwise the Windows optimizer
    // will eliminate the load operation.
    volatile bces_AtomicUtilImpl<bsls_Platform::CpuX86>::Pointer *ptr =
        const_cast<volatile bces_AtomicUtilImpl<bsls_Platform::CpuX86>::
                                                        Pointer *>(&aPointer);
    return (void*)ptr->d_value;
}

inline
void bces_AtomicUtilImpl<bsls_Platform::CpuX86>::setPtr(
    bces_AtomicUtilImpl<bsls_Platform::CpuX86>::Pointer *aPointer,
    const void *val)
{
#ifdef BSLS_PLATFORM__CPU_64_BIT
#if defined(__GNUC__)
    bces_AtomicUtilImpl_IntelPentiumSetInt64(
                        (volatile bsls_PlatformUtil::Int64*)aPointer->d_value,
                        (bsls_PlatformUtil::Int64)val);
#else
    bces_AtomicUtilImpl_IntelPentiumSwapInt64(
                        (volatile bsls_PlatformUtil::Int64*)&aPointer->d_value,
                        (bsls_PlatformUtil::Int64)val);
#endif
#else
#if defined(__GNUC__)
    bces_AtomicUtilImpl_IntelPentiumSetInt((volatile int*)&aPointer->d_value,
                                           (int)val);
#else
    bces_AtomicUtilImpl_IntelPentiumSwapInt((volatile int*)&aPointer->d_value,
                                            (int)val);
#endif
#endif
}

inline
void bces_AtomicUtilImpl<bsls_Platform::CpuX86>::setPtrRelaxed(
             bces_AtomicUtilImpl<bsls_Platform::CpuX86>::Pointer *aPointer,
             const void *val)
{
    aPointer->d_value = val;
}

inline
void *bces_AtomicUtilImpl<bsls_Platform::CpuX86>::swapPtr(
  bces_AtomicUtilImpl<bsls_Platform::CpuX86>::Pointer *aPointer,
  const void *val)
{
#if defined(BSLS_PLATFORM__CPU_64_BIT)
    return (void*)bces_AtomicUtilImpl_IntelPentiumSwapInt64(
                       (volatile bsls_PlatformUtil::Int64*)&aPointer->d_value,
                       (bsls_PlatformUtil::Int64)val);
#else
    return (void*)bces_AtomicUtilImpl_IntelPentiumSwapInt(
                       (volatile int*)&aPointer->d_value,
                       (int)val);
#endif
}

inline
void *bces_AtomicUtilImpl<bsls_Platform::CpuX86>::testAndSwapPtr(
   bces_AtomicUtilImpl<bsls_Platform::CpuX86>::Pointer *aPointer,
   const void *cmpVal, const void *swapVal)
{
#ifdef BSLS_PLATFORM__CPU_64_BIT
    return (void*)bces_AtomicUtilImpl_IntelPentiumTestAndSwap64(
                      (volatile bsls_PlatformUtil::Int64 *)&aPointer->d_value,
                      (bsls_PlatformUtil::Int64)cmpVal,
                      (bsls_PlatformUtil::Int64)swapVal);
#else
    return (void*)bces_AtomicUtilImpl_IntelPentiumTestAndSwap(
                      (volatile int*)&aPointer->d_value,
                      (int)cmpVal,
                      (int)swapVal);
#endif
}

inline
void bces_AtomicUtilImpl<bsls_Platform::CpuX86>::spinLock(
        bces_AtomicUtilImpl<bsls_Platform::CpuX86>::SpinLock *aSpin)
{
    bces_AtomicUtilImpl_IntelPentiumSpinLock(&aSpin->d_spin);
}

inline
int bces_AtomicUtilImpl<bsls_Platform::CpuX86>::spinTryLock(
                 bces_AtomicUtilImpl<bsls_Platform::CpuX86>::SpinLock *aSpin,
                 int                                                   retries)
{
    return bces_AtomicUtilImpl_IntelPentiumSpinTryLock(&aSpin->d_spin,retries);
}

inline
void bces_AtomicUtilImpl<bsls_Platform::CpuX86>::spinUnlock(
        bces_AtomicUtilImpl<bsls_Platform::CpuX86>::SpinLock *aSpin)
{
     bces_AtomicUtilImpl_IntelPentiumSpinUnlock(&aSpin->d_spin);
}

}  // close namespace BloombergLP

#if defined(_MSC_VER) && (_MSC_VER >= 1400)
#pragma warning(pop)
#endif
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
