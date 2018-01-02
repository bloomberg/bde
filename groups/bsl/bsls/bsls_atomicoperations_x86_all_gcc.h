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

#if defined(BSLS_PLATFORM_CPU_X86) \
    && (defined(BSLS_PLATFORM_CMP_GNU) || defined(BSLS_PLATFORM_CMP_CLANG))

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

    struct Uint
    {
        volatile unsigned int d_value
                            __attribute__((__aligned__(sizeof(unsigned int))));
    };

    struct Uint64
    {
        volatile Types::Uint64 d_value
                       __attribute__((__aligned__(sizeof(Types::Uint64))));
    };

    struct Pointer
    {
        void * volatile d_value __attribute__((__aligned__(sizeof(void *))));
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
    return __sync_val_compare_and_swap(&atomicInt->d_value,
                                       compareValue,
                                       swapValue);
}

inline
int AtomicOperations_X86_ALL_GCC::
    addIntNv(AtomicTypes::Int *atomicInt, int value)
{
    return __sync_add_and_fetch(&atomicInt->d_value, value);
}

inline
Types::Int64 AtomicOperations_X86_ALL_GCC::
    getInt64(const AtomicTypes::Int64 *atomicInt)
{
#if BSLS_PLATFORM_CMP_VER_MAJOR >= 40300 // gcc >= 4.3
    Types::Int64 value = atomicInt->d_value;
    return __sync_val_compare_and_swap(
                const_cast<Types::Int64 * volatile>(&atomicInt->d_value),
                value,
                value);
#else
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

#if defined(BSLS_PLATFORM_CMP_CLANG) && defined(__PIC__)
                  "ebx",    // Clang wants to reuse 'ebx' even in PIC mode
                            // and generates invalid code.
                            // Mark 'ebx' as clobbered to prevent that.
#endif
                  "ecx", "cc", "memory");
    return result;
#endif

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
#if BSLS_PLATFORM_CMP_VER_MAJOR >= 40300 // gcc >= 4.3
    Types::Int64 oldValue;

    do
    {
        oldValue = atomicInt->d_value;
    } while (__sync_val_compare_and_swap(&atomicInt->d_value,
                                         oldValue,
                                         swapValue)
             != oldValue);

    return oldValue;
#else
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
#endif
}

inline
Types::Int64 AtomicOperations_X86_ALL_GCC::
    testAndSwapInt64(AtomicTypes::Int64 *atomicInt,
                     Types::Int64 compareValue,
                     Types::Int64 swapValue)
{
#if BSLS_PLATFORM_CMP_VER_MAJOR >= 40300 // gcc >= 4.3
    return __sync_val_compare_and_swap(&atomicInt->d_value,
                                       compareValue,
                                       swapValue);
#else
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
                :
#if defined(BSLS_PLATFORM_CMP_CLANG) && defined(__PIC__)
                  "ebx",    // Clang wants to reuse 'ebx' even in PIC mode
                            // and generates invalid code.
                            // Mark 'ebx' as clobbered to prevent that.
#endif
                  "memory", "cc");

    return compareValue;
#endif
}

inline
Types::Int64 AtomicOperations_X86_ALL_GCC::
    addInt64Nv(AtomicTypes::Int64 *atomicInt,
               Types::Int64 value)
{
    return __sync_add_and_fetch(&atomicInt->d_value, value);
}

}  // close package namespace

}  // close enterprise namespace

#endif  // defined(BSLS_PLATFORM_CPU_X86) && (CMP_GNU || CMP_CLANG)

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
