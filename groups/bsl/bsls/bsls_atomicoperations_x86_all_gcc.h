// bsls_atomicoperations_x86_all_gcc.h                                -*-C++-*-
#ifndef INCLUDED_BSLS_ATOMICOPERATIONS_X86_ALL_GCC
#define INCLUDED_BSLS_ATOMICOPERATIONS_X86_ALL_GCC

//@PURPOSE: Provide implentations of atomic operations for X86/GCC.
//
//@CLASSES:
//  bsls_AtomicOperations_X86_ALL_GCC: implementation of atomics for X86/GCC.
//
//@AUTHOR: Alexei Zakharov (azakhar1)
//
//@DESCRIPTION: This component provides classes necessary to implement atomics
// on the Linux X86 platform with GCC.  The classes are for private use only.
// See 'bsls_atomicoperations' and 'bsls_atomic' for the public inteface to
// atomics.

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

#ifndef INCLUDED_BSLS_TYPES
#include <bsls_types.h>
#endif

#ifndef INCLUDED_BSLS_ATOMICOPERATIONS_DEFAULT
#include <bsls_atomicoperations_default.h>
#endif

#if defined(BSLS_PLATFORM__CPU_X86) && defined(BSLS_PLATFORM__CMP_GNU)

namespace BloombergLP {

struct bsls_AtomicOperations_X86_ALL_GCC;
typedef bsls_AtomicOperations_X86_ALL_GCC  bsls_AtomicOperations_Imp;

      // ================================================================
      // struct bsls_Atomic_TypeTraits<bsls_AtomicOperations_X86_ALL_GCC>
      // ================================================================

template <>
struct bsls_Atomic_TypeTraits<bsls_AtomicOperations_X86_ALL_GCC>
{
    struct Int
    {
        volatile int d_value __attribute__((__aligned__(sizeof(int))));
    };

    struct Int64
    {
        volatile bsls_Types::Int64 d_value
                       __attribute__((__aligned__(sizeof(bsls_Types::Int64))));
    };

    struct Pointer
    {
        void const * volatile d_value
                                  __attribute__((__aligned__(sizeof(void *))));
    };
};

                  // ========================================
                  // struct bsls_AtomicOperations_X86_ALL_GCC
                  // ========================================

struct bsls_AtomicOperations_X86_ALL_GCC
    : bsls_AtomicOperations_Default32<bsls_AtomicOperations_X86_ALL_GCC>
{
    typedef bsls_Atomic_TypeTraits<bsls_AtomicOperations_X86_ALL_GCC> Types;

        // *** atomic functions for int ***

    static int getInt(const Types::Int *atomicInt);

    static int getIntAcquire(const Types::Int *atomicInt);

    static void setInt(Types::Int *atomicInt, int value);

    static void setIntRelease(Types::Int *atomicInt, int value);

    static int swapInt(Types::Int *atomicInt, int swapValue);

    static int testAndSwapInt(Types::Int *atomicInt,
                              int compareValue,
                              int swapValue);

    static int addIntNv(Types::Int *atomicInt, int value);

        // *** atomic functions for Int64 ***

    static bsls_Types::Int64 getInt64(const Types::Int64 *atomicInt);

    static void setInt64(Types::Int64 *atomicInt, bsls_Types::Int64 value);

    static bsls_Types::Int64 swapInt64(Types::Int64       *atomicInt,
                                       bsls_Types::Int64   swapValue);

    static bsls_Types::Int64 testAndSwapInt64(
                                            Types::Int64        *atomicInt,
                                            bsls_Types::Int64    compareValue,
                                            bsls_Types::Int64    swapValue);

    static bsls_Types::Int64 addInt64Nv(Types::Int64      *atomicInt,
                                        bsls_Types::Int64  value);
};

// ===========================================================================
//                        INLINE FUNCTION DEFINITIONS
// ===========================================================================

                  // ----------------------------------------
                  // struct bsls_AtomicOperations_X86_ALL_GCC
                  // ----------------------------------------

inline
int bsls_AtomicOperations_X86_ALL_GCC::
    getInt(const Types::Int *atomicInt)
{
    int result;

#ifdef __SSE2__
    asm volatile (
        "       mfence                  \n\t"
        "       movl %[obj], %[res]     \n\t"

                : [res] "=r" (result)
                : [obj] "m"  (*atomicInt)
                : "memory");
#else
    asm volatile (
        "       lock addl $0, 0(%%esp)  \n\t"
        "       movl %[obj], %[res]     \n\t"

                : [res] "=r" (result)
                : [obj] "m"  (*atomicInt)
                : "memory", "cc");
#endif

    return result;
}

inline
int bsls_AtomicOperations_X86_ALL_GCC::
    getIntAcquire(const Types::Int *atomicInt)
{
    int result;

    asm volatile (
        "       movl %[obj], %[res]     \n\t"

                : [res] "=r" (result)
                : [obj] "m"  (*atomicInt)
                : "memory");

    return result;
}

inline
void bsls_AtomicOperations_X86_ALL_GCC::
    setInt(Types::Int *atomicInt, int value)
{
#ifdef __SSE2__
    asm volatile (
        "       movl %[val], %[obj]     \n\t"
        "       mfence                  \n\t"

                : [obj] "=m" (*atomicInt)
                : [val] "r"  (value)
                : "memory");
#else
    asm volatile (
        "       movl %[val], %[obj]     \n\t"
        "       lock addl $0, 0(%%esp)  \n\t"

                : [obj] "=m" (*atomicInt)
                : [val] "r"  (value)
                : "memory", "cc");
#endif
}

inline
void bsls_AtomicOperations_X86_ALL_GCC::
    setIntRelease(Types::Int *atomicInt, int value)
{
    asm volatile (
        "       movl %[val], %[obj]     \n\t"

                : [obj] "=m" (*atomicInt)
                : [val] "r"  (value)
                : "memory");
}

inline
int bsls_AtomicOperations_X86_ALL_GCC::
    swapInt(Types::Int *atomicInt, int swapValue)
{
    asm volatile (
        "       lock xchgl %[val], %[obj]   \n\t"

                : [obj] "=m" (*atomicInt),
                  [val] "=r" (swapValue)
                : "1" (swapValue), "m" (*atomicInt)
                : "memory");

    return swapValue;
}

inline
int bsls_AtomicOperations_X86_ALL_GCC::
    testAndSwapInt(Types::Int *atomicInt, int compareValue, int swapValue)
{
    asm volatile (
        "       lock cmpxchgl %[val], %[obj]    \n\t"

                : [cmp] "+a" (compareValue)
                : [val] "r"  (swapValue),
                  [obj] "m"  (*atomicInt)
                : "memory", "cc");

    return compareValue;
}

inline
int bsls_AtomicOperations_X86_ALL_GCC::
    addIntNv(Types::Int *atomicInt, int value)
{
#if BSLS_PLATFORM__CMP_VER_MAJOR >= 40100 // gcc >= 4.1
    return __sync_add_and_fetch(&atomicInt->d_value, value);
#else
    const int orig = value;

    asm volatile (
        "       lock xaddl %[val], %[obj]   \n\t"

                : [val] "=r" (value),
                  [obj] "+m" (*atomicInt)
                : "0" (value)
                : "memory", "cc");

    return orig + value;
#endif
}

inline
bsls_Types::Int64 bsls_AtomicOperations_X86_ALL_GCC::
    getInt64(const Types::Int64 *atomicInt)
{
    bsls_Types::Int64 result;

    asm volatile (
#ifdef __PIC__
        "       pushl %%ebx                 \n\t"
#endif
        "       movl %%ebx, %%eax           \n\t"
        "       movl %%ecx, %%edx           \n\t"
#if __GNUC__ != 3
        "       lock cmpxchg8b %[obj]       \n\t"
#else
        // gcc 3.4 seems to think that it can take edx as %1.
        "       lock cmpxchg8b (%[obj])     \n\t"
#endif
#ifdef __PIC__
        "       popl %%ebx                  \n\t"
#endif
                : [res] "=&A" (result)
                :
#if __GNUC__ != 3
                  [obj] "m" (*atomicInt),
#else
                  [obj] "S" (atomicInt),
#endif
                  "0" (0)
                :
#ifndef __PIC__
                  "ebx",
#endif
                  "ecx", "cc", "memory");
    return result;
}

inline
void bsls_AtomicOperations_X86_ALL_GCC::
    setInt64(Types::Int64 *atomicInt, bsls_Types::Int64 value)
{
    swapInt64(atomicInt, value);
}

inline
bsls_Types::Int64 bsls_AtomicOperations_X86_ALL_GCC::
    swapInt64(Types::Int64       *atomicInt,
              bsls_Types::Int64   swapValue)
{
    bsls_Types::Int64 result;

    asm volatile (
#ifdef __PIC__
        "       pushl %%ebx             \n\t"
        "       movl  %[val], %%ebx     \n\t"
#endif
        "1:                             \n\t"
        "       lock cmpxchg8b %[obj]   \n\t"
        "       jnz 1b                  \n\t"
#ifdef __PIC__
        "       popl %%ebx              \n\t"
#endif
                : [res] "=A" (result),
                  [obj] "+m" (*atomicInt)
                :
#ifdef __PIC__
                  [val] "g" ((unsigned int) swapValue),
#else
                  [val] "b" ((unsigned int) swapValue),
#endif
                  "c" ((int) (swapValue >> 32)),
                  "A" (*atomicInt)
                : "memory", "cc");

    return result;
}

inline
bsls_Types::Int64 bsls_AtomicOperations_X86_ALL_GCC::
    testAndSwapInt64(Types::Int64        *atomicInt,
                     bsls_Types::Int64    compareValue,
                     bsls_Types::Int64    swapValue)
{
    asm volatile (
#ifdef __PIC__
        "       pushl   %%ebx               \n\t"
        "       movl    %[val], %%ebx       \n\t"
#endif
        "       lock cmpxchg8b %[obj]       \n\t"
#ifdef __PIC__
        "       popl    %%ebx               \n\t"
#endif
                : [cmp] "=A" (compareValue),
                  [obj] "+m" (*atomicInt)
                :
#ifdef __PIC__
                  [val] "g" ((unsigned int) swapValue),
#else
                  [val] "b" ((unsigned int) swapValue),
#endif
                  "c" ((int) (swapValue >> 32)),
                  "0" (compareValue)
                : "memory", "cc");

    return compareValue;
}

inline
bsls_Types::Int64 bsls_AtomicOperations_X86_ALL_GCC::
    addInt64Nv(Types::Int64      *atomicInt,
               bsls_Types::Int64  value)
{
#if BSLS_PLATFORM__CMP_VER_MAJOR >= 40100 // gcc >= 4.1
    return __sync_add_and_fetch(&atomicInt->d_value, value);
#else
    bsls_Types::Int64 result;

    asm volatile (
#   ifdef __PIC__
        "       pushl %%ebx             \n\t"
#   endif
        "1:                             \n\t"
        "       movl %%eax, %%ebx       \n\t"
        "       movl %%edx, %%ecx       \n\t"
        "       addl (%[val]), %%ebx    \n\t"
        "       adcl 4(%[val]), %%ecx   \n\t"
        "       lock cmpxchg8b (%[obj]) \n\t"
        "       jnz 1b                  \n\t"
        "       movl %%ebx, %%eax       \n\t"
        "       movl %%ecx, %%edx       \n\t"
#   ifdef __PIC__
        "       popl %%ebx              \n\t"
#   endif
                : [res] "=&A" (result),
                  [obj] "+D"  (atomicInt)// should be m, but it breaks gcc 4
                : [val] "S"   (&value),  // should be m, but it breaks gcc 3
                  "0" (*atomicInt)
                :
#   ifndef __PIC__
                  "ebx",
#   endif
                  "ecx","cc","memory");

   return result;
#endif
}

}  // close enterprise namespace

#endif  // defined(BSLS_PLATFORM__CPU_X86) && defined(BSLS_PLATFORM__CMP_GNU)

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2011
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
