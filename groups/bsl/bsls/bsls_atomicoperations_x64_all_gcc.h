// bsls_atomicoperations_x64_all_gcc.h                                -*-C++-*-
#ifndef INCLUDED_BSLS_ATOMICOPERATIONS_X64_ALL_GCC
#define INCLUDED_BSLS_ATOMICOPERATIONS_X64_ALL_GCC

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide implementations of atomic operations for X86_64/GCC.
//
//@CLASSES:
// bsls::AtomicOperations_X64_ALL_GCC: implementation of atomics for X86_64/GCC
//
//@DESCRIPTION: This component provides classes necessary to implement atomics
// on the Linux X86_64 platform with GCC.  The classes are for private use
// only.  See 'bsls_atomicoperations' and 'bsls_atomic' for the public
// interface to atomics.
//
///Notes
///-----
//: o GCC atomic intrinsics are used where possible instead of the assembly
//:   code,
//: o "+m" constraint may cause an 'inconsistent operand constraint' error in
//:   GCC 3.x in optimized builds, therefore sometimes a combination of output
//:   "=m" and input "m" constraints is used.

#ifndef INCLUDED_BSLS_ATOMICOPERATIONS_DEFAULT
#include <bsls_atomicoperations_default.h>
#endif

#ifndef INCLUDED_BSLS_PLATFORM
#include <bsls_platform.h>
#endif

#ifndef INCLUDED_BSLS_TYPES
#include <bsls_types.h>
#endif

#if defined(BSLS_PLATFORM_CPU_X86_64) \
    && (defined(BSLS_PLATFORM_CMP_GNU) || defined(BSLS_PLATFORM_CMP_CLANG))

namespace BloombergLP {

namespace bsls {

struct AtomicOperations_X64_ALL_GCC;
typedef AtomicOperations_X64_ALL_GCC  AtomicOperations_Imp;

               // ===============================================
               // Atomic_TypeTraits<AtomicOperations_X64_ALL_GCC>
               // ===============================================

template <>
struct Atomic_TypeTraits<AtomicOperations_X64_ALL_GCC>
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
                     // struct AtomicOperations_X64_ALL_GCC
                     // ===================================

struct AtomicOperations_X64_ALL_GCC
    : AtomicOperations_Default64<AtomicOperations_X64_ALL_GCC>
{
    typedef Atomic_TypeTraits<AtomicOperations_X64_ALL_GCC> AtomicTypes;

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

    static void setInt64Release(AtomicTypes::Int64 *atomicInt,
                                Types::Int64 value);

    static Types::Int64 swapInt64(AtomicTypes::Int64 *atomicInt,
                                  Types::Int64 swapValue);

    static Types::Int64 testAndSwapInt64(AtomicTypes::Int64 *atomicInt,
                                         Types::Int64 compareValue,
                                         Types::Int64 swapValue);

    static Types::Int64 addInt64Nv(AtomicTypes::Int64 *atomicInt,
                                   Types::Int64 value);
};

// ============================================================================
//                        INLINE FUNCTION DEFINITIONS
// ============================================================================

                     // -----------------------------------
                     // struct AtomicOperations_X64_ALL_GCC
                     // -----------------------------------

inline
int AtomicOperations_X64_ALL_GCC::
    getInt(const AtomicTypes::Int *atomicInt)
{
    int ret;

    asm volatile (
        "       movl %[obj], %[ret]     \n\t"

                : [ret] "=r" (ret)
                : [obj] "m"  (*atomicInt)
                : "memory");

    return ret;
}

inline
void AtomicOperations_X64_ALL_GCC::
    setInt(AtomicTypes::Int *atomicInt, int value)
{
    asm volatile (
        "       movl %[val], %[obj]     \n\t"
        "       mfence                  \n\t"

                : [obj] "=m" (*atomicInt)
                : [val] "r"  (value)
                : "memory");
}

inline
void AtomicOperations_X64_ALL_GCC::
    setIntRelease(AtomicTypes::Int *atomicInt, int value)
{
    asm volatile (
        "       movl %[val], %[obj]     \n\t"

                : [obj] "=m" (*atomicInt)
                : [val] "r"  (value)
                : "memory");
}

inline
int AtomicOperations_X64_ALL_GCC::
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
int AtomicOperations_X64_ALL_GCC::
    testAndSwapInt(AtomicTypes::Int *atomicInt,
                   int compareValue,
                   int swapValue)
{
    return __sync_val_compare_and_swap(&atomicInt->d_value,
                                       compareValue,
                                       swapValue);
}

inline
int AtomicOperations_X64_ALL_GCC::
    addIntNv(AtomicTypes::Int *atomicInt, int value)
{
    return __sync_add_and_fetch(&atomicInt->d_value, value);
}

inline
Types::Int64 AtomicOperations_X64_ALL_GCC::
    getInt64(const AtomicTypes::Int64 *atomicInt)
{
    Types::Int64 result;

    asm volatile (
        "       movq %[obj], %[res]         \n\t"

                : [res] "=r" (result)
                : [obj] "m"  (*atomicInt)
                : "memory");

    return result;
}

inline
void AtomicOperations_X64_ALL_GCC::
    setInt64(AtomicTypes::Int64 *atomicInt, Types::Int64 value)
{
    asm volatile (
        "       movq %[val], %[obj]         \n\t"
        "       mfence                      \n\t"

                : [obj] "=m" (*atomicInt)
                : [val] "r"  (value)
                : "memory");
}

inline
void AtomicOperations_X64_ALL_GCC::
    setInt64Release(AtomicTypes::Int64 *atomicInt,
                    Types::Int64 value)
{
    asm volatile (
        "       movq %[val], %[obj]         \n\t"

                : [obj] "=m" (*atomicInt)
                : [val] "r"  (value)
                : "memory");
}

inline
Types::Int64 AtomicOperations_X64_ALL_GCC::
    swapInt64(AtomicTypes::Int64 *atomicInt,
              Types::Int64 swapValue)
{
    asm volatile (
        "       lock xchgq %[val], %[obj]   \n\t"

                : [val] "+r" (swapValue),
                  [obj] "=m" (*atomicInt)
                : "m" (*atomicInt)
                : "memory");

    return swapValue;
}

inline
Types::Int64 AtomicOperations_X64_ALL_GCC::
    testAndSwapInt64(AtomicTypes::Int64 *atomicInt,
                     Types::Int64 compareValue,
                     Types::Int64 swapValue)
{
    return __sync_val_compare_and_swap(&atomicInt->d_value,
                                       compareValue,
                                       swapValue);
}

inline
Types::Int64 AtomicOperations_X64_ALL_GCC::
    addInt64Nv(AtomicTypes::Int64 *atomicInt,
               Types::Int64 value)
{
    return __sync_add_and_fetch(&atomicInt->d_value, value);
}

}  // close package namespace

}  // close enterprise namespace

#endif // defined(BSLS_PLATFORM_CPU_X86_64) && (CMP_GNU || CMP_CLANG)

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
