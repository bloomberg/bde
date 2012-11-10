// bsls_atomicoperations_x86_all_gcc.h                                -*-C++-*-
#ifndef INCLUDED_BSLS_ATOMICOPERATIONS_X86_ALL_GCC
#define INCLUDED_BSLS_ATOMICOPERATIONS_X86_ALL_GCC

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide implementations of atomic operations for X86/GCC.
//
//@CLASSES:
//  bsls::AtomicOperations_X86_ALL_GCC: implementation of atomics for X86/GCC.
//
//@DESCRIPTION: This component provides classes necessary to implement atomics
// on the Linux X86 platform with GCC.  The classes are for private use only.
// See 'bsls_atomicoperations' and 'bsls_atomic' for the public interface to
// atomics.

#ifndef INCLUDED_BSLS_ATOMICOPERATIONS_DEFAULT
#include <bsls_atomicoperations_default.h>
#endif

#ifndef INCLUDED_BSLS_PLATFORM
#include <bsls_platform.h>
#endif

#ifndef INCLUDED_BSLS_TYPES
#include <bsls_types.h>
#endif

#if defined(BSLS_PLATFORM_CPU_X86) && defined(BSLS_PLATFORM_CMP_GNU)

namespace BloombergLP {

namespace bsls {

struct AtomicOperations_X86_ALL_GCC;
typedef AtomicOperations_X86_ALL_GCC  AtomicOperations_Imp;

           // ======================================================
           // struct Atomic_TypeTraits<AtomicOperations_X86_ALL_GCC>
           // ======================================================

template <>
struct Atomic_TypeTraits<AtomicOperations_X86_ALL_GCC>
{
    struct Int
    {
        volatile int d_value __attribute__((__aligned__(sizeof(int))));
    };

    struct Int64
    {
        volatile Types::Int64 d_value
                       __attribute__((__aligned__(sizeof(Types::Int64))));
    };

    struct Pointer
    {
        void const * volatile d_value
                                  __attribute__((__aligned__(sizeof(void *))));
    };
};

                     // ===================================
                     // struct AtomicOperations_X86_ALL_GCC
                     // ===================================

struct AtomicOperations_X86_ALL_GCC
    : AtomicOperations_Default32<AtomicOperations_X86_ALL_GCC>
{
    typedef Atomic_TypeTraits<AtomicOperations_X86_ALL_GCC> AtomicTypes;

        // *** atomic functions for int ***

    static int getInt(const AtomicTypes::Int *atomicInt);

    static int getIntAcquire(const AtomicTypes::Int *atomicInt);

    static void setInt(AtomicTypes::Int *atomicInt, int value);

    static void setIntRelease(AtomicTypes::Int *atomicInt, int value);

    static int swapInt(AtomicTypes::Int *atomicInt, int swapValue);

    static int testAndSwapInt(AtomicTypes::Int *atomicInt,
                              int compareValue,
                              int swapValue);

    static int addIntNv(AtomicTypes::Int *atomicInt, int value);

        // *** atomic functions for Int64 ***

    static Types::Int64 getInt64(const AtomicTypes::Int64 *atomicInt);

    static void setInt64(AtomicTypes::Int64 *atomicInt, Types::Int64 value);

    static Types::Int64 swapInt64(AtomicTypes::Int64  *atomicInt,
                                  Types::Int64 swapValue);

    static Types::Int64 testAndSwapInt64(AtomicTypes::Int64 *atomicInt,
                                         Types::Int64 compareValue,
                                         Types::Int64 swapValue);

    static Types::Int64 addInt64Nv(AtomicTypes::Int64 *atomicInt,
                                   Types::Int64 value);
};

// ===========================================================================
//                        INLINE FUNCTION DEFINITIONS
// ===========================================================================

                     // -----------------------------------
                     // struct AtomicOperations_X86_ALL_GCC
                     // -----------------------------------

inline
int AtomicOperations_X86_ALL_GCC::
    getInt(const AtomicTypes::Int *atomicInt)
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
int AtomicOperations_X86_ALL_GCC::
    getIntAcquire(const AtomicTypes::Int *atomicInt)
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
void AtomicOperations_X86_ALL_GCC::
    setInt(AtomicTypes::Int *atomicInt, int value)
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
void AtomicOperations_X86_ALL_GCC::
    setIntRelease(AtomicTypes::Int *atomicInt, int value)
{
    asm volatile (
        "       movl %[val], %[obj]     \n\t"

                : [obj] "=m" (*atomicInt)
                : [val] "r"  (value)
                : "memory");
}

inline
int AtomicOperations_X86_ALL_GCC::
    swapInt(AtomicTypes::Int *atomicInt, int swapValue)
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
int AtomicOperations_X86_ALL_GCC::
    testAndSwapInt(AtomicTypes::Int *atomicInt,
                   int compareValue,
                   int swapValue)
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
int AtomicOperations_X86_ALL_GCC::
    addIntNv(AtomicTypes::Int *atomicInt, int value)
{
#if BSLS_PLATFORM_CMP_VER_MAJOR >= 40100 // gcc >= 4.1
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
Types::Int64 AtomicOperations_X86_ALL_GCC::
    getInt64(const AtomicTypes::Int64 *atomicInt)
{
    Types::Int64 result;

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
void AtomicOperations_X86_ALL_GCC::
    setInt64(AtomicTypes::Int64 *atomicInt, Types::Int64 value)
{
    swapInt64(atomicInt, value);
}

inline
Types::Int64 AtomicOperations_X86_ALL_GCC::
    swapInt64(AtomicTypes::Int64 *atomicInt,
              Types::Int64 swapValue)
{
    Types::Int64 result;

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
                :
#if defined(BSLS_PLATFORM_CMP_CLANG) && defined(__PIC__)
                  "ebx",    // Clang wants to reuse 'ebx' even in PIC mode
                            // and generates invalid code.
                            // Mark 'ebx' as clobbered to prevent that.
#endif
                  "memory", "cc");

    return result;
}

inline
Types::Int64 AtomicOperations_X86_ALL_GCC::
    testAndSwapInt64(AtomicTypes::Int64 *atomicInt,
                     Types::Int64 compareValue,
                     Types::Int64 swapValue)
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
Types::Int64 AtomicOperations_X86_ALL_GCC::
    addInt64Nv(AtomicTypes::Int64 *atomicInt,
               Types::Int64 value)
{
#if BSLS_PLATFORM_CMP_VER_MAJOR >= 40100 // gcc >= 4.1
    return __sync_add_and_fetch(&atomicInt->d_value, value);
#else
    Types::Int64 result;

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

}  // close package namespace

}  // close enterprise namespace

#endif  // defined(BSLS_PLATFORM_CPU_X86) && defined(BSLS_PLATFORM_CMP_GNU)

#endif

// ----------------------------------------------------------------------------
// Copyright (C) 2012 Bloomberg L.P.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
// ----------------------------- END-OF-FILE ----------------------------------
