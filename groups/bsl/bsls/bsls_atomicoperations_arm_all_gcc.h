// bsls_atomicoperations_arm_all_gcc.h                                -*-C++-*-
#ifndef INCLUDED_BSLS_ATOMICOPERATIONS_ARM_ALL_GCC
#define INCLUDED_BSLS_ATOMICOPERATIONS_ARM_ALL_GCC

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide implementations of atomic operations for ARM/GCC.
//
//@CLASSES:
//  bsls::AtomicOperations_ARM_ALL_GCC: implementation of atomics for ARM/GCC.
//
//@DESCRIPTION: This component provides classes necessary to implement atomics
// on the Linux ARM platform with GCC.  The classes are for private use only.
// See 'bsls_atomicoperations' and 'bsls_atomic' for the public interface to
// atomics.
//
// IMPLEMENTATION NOTES: Wherever possible we use atomic intrinsics that both
// GCC 4.6+ and Clang 3.2+ support.  Otherwise the inline assembly code is
// used that can be compiled both by GCC and Clang.  This puts some
// restrictions on the assembly code because Clang doesn't support register
// pairs that represent a 64bit value.  So instead of 'ldrexd %1, %H1, [%3]'
// the exact registers have to be specified, as in 'ldrexd r2, r3, [%3]'.  Note
// also that operations like 'ldrexd' and 'stdrexd' are supported only starting
// from 'armv6zk', so not every armv6 platform will work.
//
// For more details on ARM atomic pritives see the Linux kernel source code
// (arch/arm/include/asm/atomic.h) and the "C/C++11 mappings to processors"
// (http://www.cl.cam.ac.uk/~pes20/cpp/cpp0xmappings.html), and the
// "ARM1156T2F-S Technical Reference Manual"
// (http://infocenter.arm.com/help/index.jsp?
// topic=/com.arm.doc.ddi0290g/Babebdcb.html), which contains documentation on
// the p15 coprocessor opcodes used in this component).

#ifndef INCLUDED_BSLS_ATOMICOPERATIONS_DEFAULT
#include <bsls_atomicoperations_default.h>
#endif

#ifndef INCLUDED_BSLS_PLATFORM
#include <bsls_platform.h>
#endif

#ifndef INCLUDED_BSLS_TYPES
#include <bsls_types.h>
#endif

#if defined(BSLS_PLATFORM_CPU_ARM) \
    && (defined(BSLS_PLATFORM_CMP_GNU) || defined(BSLS_PLATFORM_CMP_CLANG))

namespace BloombergLP {

namespace bsls {

struct AtomicOperations_ARM_ALL_GCC;
typedef AtomicOperations_ARM_ALL_GCC  AtomicOperations_Imp;

           // ======================================================
           // struct Atomic_TypeTraits<AtomicOperations_ARM_ALL_GCC>
           // ======================================================

template <>
struct Atomic_TypeTraits<AtomicOperations_ARM_ALL_GCC>
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
                     // struct AtomicOperations_ARM_ALL_GCC
                     // ===================================

struct AtomicOperations_ARM_ALL_GCC
    : AtomicOperations_Default32<AtomicOperations_ARM_ALL_GCC>
{
private:
    // PRIVATE TYPES
    struct Int64_Words {
        int w1;
        int w2;
    };

public:
    typedef Atomic_TypeTraits<AtomicOperations_ARM_ALL_GCC> AtomicTypes;

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
                     // struct AtomicOperations_ARM_ALL_GCC
                     // -----------------------------------

#if BSLS_PLATFORM_CMP_VERSION < 40700
#   if defined(BSLS_PLATFORM_CPU_ARM_V7)

#       define BSLS_ATOMICOPERATIONS_BARRIER()          \
            asm volatile ("dmb" ::: "memory")           \

#       define BSLS_ATOMICOPERATIONS_INSTR_BARRIER()    \
            asm volatile ("isb")                        \

#   else

#       define BSLS_ATOMICOPERATIONS_BARRIER()                      \
            do {                                                    \
                int temp_reg = 0;                                   \
                asm volatile (                                      \
                        "mcr p15, 0, %[temp_reg], c7, c10, 5 \n\t"  \
                        :                                           \
                        : [temp_reg] "r" (temp_reg)                 \
                        : "memory");                                \
            } while (0)                                             \

#       define BSLS_ATOMICOPERATIONS_INSTR_BARRIER()                \
            do {                                                    \
                int temp_reg = 0;                                   \
                asm volatile (                                      \
                        "mcr p15, 0, %[temp_reg], c7, c5, 4 \n\t"   \
                        :                                           \
                        : [temp_reg] "r" (temp_reg));               \
            } while (0)                                             \

#   endif
#else
#   define BSLS_ATOMICOPERATIONS_USE_CPP11_INTRINSICS
#endif

inline
int AtomicOperations_ARM_ALL_GCC::
    getInt(const AtomicTypes::Int *atomicInt)
{
#ifdef BSLS_ATOMICOPERATIONS_USE_CPP11_INTRINSICS
    return __atomic_load_n(&atomicInt->d_value, __ATOMIC_SEQ_CST);
#else
    int result = atomicInt->d_value;
    BSLS_ATOMICOPERATIONS_BARRIER();

    return result;

#endif
}

inline
int AtomicOperations_ARM_ALL_GCC::
    getIntAcquire(const AtomicTypes::Int *atomicInt)
{
#ifdef BSLS_ATOMICOPERATIONS_USE_CPP11_INTRINSICS
    return __atomic_load_n(&atomicInt->d_value, __ATOMIC_ACQUIRE);
#else
    return getInt(atomicInt);
#endif
}

inline
void AtomicOperations_ARM_ALL_GCC::
    setInt(AtomicTypes::Int *atomicInt, int value)
{
#ifdef BSLS_ATOMICOPERATIONS_USE_CPP11_INTRINSICS
    __atomic_store_n(&atomicInt->d_value, value, __ATOMIC_SEQ_CST);
#else
    BSLS_ATOMICOPERATIONS_BARRIER();
    atomicInt->d_value = value;
    BSLS_ATOMICOPERATIONS_BARRIER();
#endif
}

inline
void AtomicOperations_ARM_ALL_GCC::
    setIntRelease(AtomicTypes::Int *atomicInt, int value)
{
#ifdef BSLS_ATOMICOPERATIONS_USE_CPP11_INTRINSICS
    __atomic_store_n(&atomicInt->d_value, value, __ATOMIC_RELEASE);
#else
    BSLS_ATOMICOPERATIONS_BARRIER();
    setInt(atomicInt, value);
#endif
}

inline
int AtomicOperations_ARM_ALL_GCC::
    swapInt(AtomicTypes::Int *atomicInt, int swapValue)
{
#ifdef BSLS_ATOMICOPERATIONS_USE_CPP11_INTRINSICS
    return __atomic_exchange_n(&atomicInt->d_value,
                               swapValue,
                               __ATOMIC_SEQ_CST);
#else
    int oldValue;
    int tmp;

    BSLS_ATOMICOPERATIONS_BARRIER();

    asm volatile (
        "1:     ldrex   %[old], [%3]            \n\t"
        "       strex   %[tmp], %[val], [%3]    \n\t"
        "       teq     %[tmp], #0              \n\t"
        "       bne     1b                      \n\t"

                : [old] "=&r" (oldValue),
                  [tmp] "=&r" (tmp),
                        "+Qo" (*atomicInt)
                :       "r"   (atomicInt),
                  [val] "r"   (swapValue)
                : "cc", "memory");

    BSLS_ATOMICOPERATIONS_INSTR_BARRIER();

    return oldValue;
#endif
}

inline
int AtomicOperations_ARM_ALL_GCC::
    testAndSwapInt(AtomicTypes::Int *atomicInt,
                   int compareValue,
                   int swapValue)
{
#ifdef BSLS_ATOMICOPERATIONS_USE_CPP11_INTRINSICS
    __atomic_compare_exchange_n(&atomicInt->d_value, &compareValue, swapValue,
                                false, __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST);
    return compareValue;
#else
    return __sync_val_compare_and_swap(&atomicInt->d_value,
                                       compareValue,
                                       swapValue);
#endif
}

inline
int AtomicOperations_ARM_ALL_GCC::
    addIntNv(AtomicTypes::Int *atomicInt, int value)
{
#ifdef BSLS_ATOMICOPERATIONS_USE_CPP11_INTRINSICS
    return __atomic_add_fetch(&atomicInt->d_value, value, __ATOMIC_SEQ_CST);
#else
    return __sync_add_and_fetch(&atomicInt->d_value, value);
#endif
}

inline
Types::Int64 AtomicOperations_ARM_ALL_GCC::
    getInt64(const AtomicTypes::Int64 *atomicInt)
{
#ifdef BSLS_ATOMICOPERATIONS_USE_CPP11_INTRINSICS
    return __atomic_load_n(&atomicInt->d_value, __ATOMIC_SEQ_CST);
#else
    Types::Int64 result;

    asm volatile (
        "       ldrexd  r2, r3, [%2]       \n\t"
        "       mov     %[res1], r2        \n\t"
        "       mov     %[res2], r3        \n\t"

                : [res1] "=&r" (((Int64_Words &) result).w1),
                  [res2] "=&r" (((Int64_Words &) result).w2)
                :        "r"   (atomicInt),
                         "Qo"  (*atomicInt)
                : "r2", "r3");

    BSLS_ATOMICOPERATIONS_BARRIER();

    return result;
#endif
}

inline
void AtomicOperations_ARM_ALL_GCC::
    setInt64(AtomicTypes::Int64 *atomicInt, Types::Int64 value)
{
#ifdef BSLS_ATOMICOPERATIONS_USE_CPP11_INTRINSICS
    return __atomic_store_n(&atomicInt->d_value, value, __ATOMIC_SEQ_CST);
#else
    swapInt64(atomicInt, value);
#endif
}

inline
Types::Int64 AtomicOperations_ARM_ALL_GCC::
    swapInt64(AtomicTypes::Int64 *atomicInt,
              Types::Int64 swapValue)
{
#ifdef BSLS_ATOMICOPERATIONS_USE_CPP11_INTRINSICS
    return __atomic_exchange_n(&atomicInt->d_value,
                               swapValue,
                               __ATOMIC_SEQ_CST);
#else
    Types::Int64 oldValue;
    int rc;

    BSLS_ATOMICOPERATIONS_BARRIER();

    asm volatile (
        "1:     ldrexd  r2, r3, [%4]           \n\t"
        "       mov     %[old1], r2            \n\t"
        "       mov     %[old2], r3            \n\t"
        "       mov     r2, %[val1]            \n\t"
        "       mov     r3, %[val2]            \n\t"
        "       strexd  %[rc], r2, r3, [%4]    \n\t"
        "       teq     %[rc], #0              \n\t"
        "       bne     1b                     \n\t"

                : [rc]   "=&r" (rc),
                  [old1] "=&r" (((Int64_Words &) oldValue).w1),
                  [old2] "=&r" (((Int64_Words &) oldValue).w2),
                         "+Qo" (*atomicInt)
                :        "r"   (atomicInt),
                  [val1] "r"   (((Int64_Words &) swapValue).w1),
                  [val2] "r"   (((Int64_Words &) swapValue).w2)
                : "r2", "r3", "cc", "memory");

    BSLS_ATOMICOPERATIONS_INSTR_BARRIER();

    return oldValue;
#endif
}

inline
Types::Int64 AtomicOperations_ARM_ALL_GCC::
    testAndSwapInt64(AtomicTypes::Int64 *atomicInt,
                     Types::Int64 compareValue,
                     Types::Int64 swapValue)
{
#ifdef BSLS_ATOMICOPERATIONS_USE_CPP11_INTRINSICS
    __atomic_compare_exchange_n(&atomicInt->d_value,
                                &compareValue,
                                swapValue,
                                false,
                                __ATOMIC_SEQ_CST,
                                __ATOMIC_SEQ_CST);
    return compareValue;
#else
    Types::Int64 oldValue;
    int rc;

    BSLS_ATOMICOPERATIONS_BARRIER();

    asm volatile (
        "1:     ldrexd  r2, r3, [%4]           \n\t"
        "       mov     %[old1], r2            \n\t"
        "       mov     %[old2], r3            \n\t"
        "       mov     r2, %[val1]            \n\t"
        "       mov     r3, %[val2]            \n\t"
        "       mov     %[rc], #0              \n\t"
        "       teq     %[old1], %[cmp1]       \n\t"
        "       itt     eq                     \n\t"
        "       teqeq   %[old2], %[cmp2]       \n\t"
        "       strexdeq %[rc], r2, r3, [%4]   \n\t"
        "       teq     %[rc], #0              \n\t"
        "       bne     1b                     \n\t"

                : [rc]   "=&r" (rc),
                  [old1] "=&r" (((Int64_Words &) oldValue).w1),
                  [old2] "=&r" (((Int64_Words &) oldValue).w2),
                         "+Qo" (*atomicInt)
                :        "r"   (atomicInt),
                  [cmp1] "r"   (((Int64_Words &) compareValue).w1),
                  [cmp2] "r"   (((Int64_Words &) compareValue).w2),
                  [val1] "r"   (((Int64_Words &) swapValue).w1),
                  [val2] "r"   (((Int64_Words &) swapValue).w2)
                : "r2", "r3", "cc", "memory");

    BSLS_ATOMICOPERATIONS_INSTR_BARRIER();

    return oldValue;
#endif
}

inline
Types::Int64 AtomicOperations_ARM_ALL_GCC::
    addInt64Nv(AtomicTypes::Int64 *atomicInt,
               Types::Int64 value)
{
#ifdef BSLS_ATOMICOPERATIONS_USE_CPP11_INTRINSICS
    return __atomic_add_fetch(&atomicInt->d_value, value, __ATOMIC_SEQ_CST);
#else
    Types::Int64 old;
    Types::Int64 newVal;
    Types::Int64 prev = atomicInt->d_value;

    do {
        old = prev;
        newVal = old + value;
    } while (old != (prev = testAndSwapInt64(atomicInt, old, newVal)));

    return newVal;
#endif
}

}  // close package namespace

}  // close enterprise namespace

#endif  // defined(BSLS_PLATFORM_CPU_ARM) && (CMP_GNU || CMP_CLANG)

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
