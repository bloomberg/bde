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
//@AUTHOR: Alexei Zakharov (azakhar1)
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

#if defined(BSLS_PLATFORM_CPU_X86_64) && defined(BSLS_PLATFORM_CMP_GNU)

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
        void const * volatile d_value
                                  __attribute__((__aligned__(sizeof(void *))));
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

    static Types::Int64 getInt64Acquire(const AtomicTypes::Int64 *atomicInt);

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

// ===========================================================================
//                        INLINE FUNCTION DEFINITIONS
// ===========================================================================

                     // -----------------------------------
                     // struct AtomicOperations_X64_ALL_GCC
                     // -----------------------------------

inline
int AtomicOperations_X64_ALL_GCC::
    getInt(const AtomicTypes::Int *atomicInt)
{
    int ret;

    asm volatile (
        "       mfence                  \n\t"
        "       movl %[obj], %[ret]     \n\t"

                : [ret] "=r" (ret)
                : [obj] "m"  (*atomicInt)
                : "memory");

    return ret;
}

inline
int AtomicOperations_X64_ALL_GCC::
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
    asm volatile (
        "       lock cmpxchgl %[val], %[obj]    \n\t"

                : [cmp] "+a" (compareValue)
                : [val] "r"  (swapValue),
                  [obj] "m"  (*atomicInt)
                : "memory", "cc");

    return compareValue;
}

inline
int AtomicOperations_X64_ALL_GCC::
    addIntNv(AtomicTypes::Int *atomicInt, int value)
{
#if BSLS_PLATFORM_CMP_VER_MAJOR >= 40100 // gcc >= 4.1
    return __sync_add_and_fetch(&atomicInt->d_value, value);
#else
    int orig = value;

    asm volatile (
        "       lock xaddl %[val], %[obj]   \n\t"

                : [val] "=r" (value),
                  [obj] "=m" (*atomicInt)
                : "0" (value), "m" (*atomicInt)
                : "memory", "cc");

    return orig + value;
#endif
}

inline
Types::Int64 AtomicOperations_X64_ALL_GCC::
    getInt64(const AtomicTypes::Int64 *atomicInt)
{
    Types::Int64 result;

    asm volatile (
        "       mfence                      \n\t"
        "       movq %[obj], %[res]         \n\t"

                : [res] "=r" (result)
                : [obj] "m"  (*atomicInt)
                : "memory");

    return result;
}

inline
Types::Int64 AtomicOperations_X64_ALL_GCC::
    getInt64Acquire(const AtomicTypes::Int64 *atomicInt)
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
    asm volatile (
        "       lock cmpxchgq %[val], %[obj]    \n\t"

                : [cmp] "+a" (compareValue),
                  [obj] "+m" (*atomicInt)
                : [val] "r"  (swapValue)
                : "memory", "cc");

    return compareValue;
}

inline
Types::Int64 AtomicOperations_X64_ALL_GCC::
    addInt64Nv(AtomicTypes::Int64 *atomicInt,
               Types::Int64 value)
{
#if BSLS_PLATFORM_CMP_VER_MAJOR >= 40100 // gcc >= 4.1
    return __sync_add_and_fetch(&atomicInt->d_value, value);
#else
    Types::Int64 operand = value;

    asm volatile (
        "       lock xaddq %[val], %[obj]   \n\t"

                : [val] "+r" (value),
                  [obj] "+m" (*atomicInt)
                :
                : "memory", "cc");

    return operand + value;
#endif
}

}  // close package namespace

}  // close enterprise namespace

#endif // defined(BSLS_PLATFORM_CPU_X86_64) && defined(BSLS_PLATFORM_CMP_GNU)

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2011
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
