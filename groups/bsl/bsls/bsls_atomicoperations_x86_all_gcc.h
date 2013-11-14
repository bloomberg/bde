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
    Types::Int64 value = atomicInt->d_value;
    return __sync_val_compare_and_swap(
                const_cast<Types::Int64 * volatile>(&atomicInt->d_value),
                value,
                value);
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
    Types::Int64 oldValue;

    do
    {
        oldValue = atomicInt->d_value;
    } while (__sync_val_compare_and_swap(&atomicInt->d_value,
                                         oldValue,
                                         swapValue)
             != oldValue);

    return oldValue;
}

inline
Types::Int64 AtomicOperations_X86_ALL_GCC::
    testAndSwapInt64(AtomicTypes::Int64 *atomicInt,
                     Types::Int64 compareValue,
                     Types::Int64 swapValue)
{
    return __sync_val_compare_and_swap(&atomicInt->d_value,
                                       compareValue,
                                       swapValue);
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
// Copyright (C) 2013 Bloomberg L.P.
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
