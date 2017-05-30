// bsls_atomicoperations_powerpc_all_gcc.h                            -*-C++-*-
#ifndef INCLUDED_BSLS_ATOMICOPERATIONS_POWERPC_ALL_GCC
#define INCLUDED_BSLS_ATOMICOPERATIONS_POWERPC_ALL_GCC

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide implementations of atomic operations for gcc on PowerPC
//
//@CLASSES:
//  bsls::AtomicOperations_POWERPC_ALL_GCC: atomics for gcc on PowerPC
//
//@DESCRIPTION: This component provides classes necessary to implement atomics
// on the PowerPC platform in 32bit/64bit mode with the GCC compiler.  The
// classes are for private use only.  See 'bsls_atomicoperations' and
// 'bsls_atomic' for the public interface to atomics.
//
// 'bsls_atomicoperations_all_all_gccintrinsics.h' is used for gcc 4.7+ and
// provides C++11 atomics.  The implementation herein is intended for earlier
// versions of gcc and will work correctly for 64-bit programs, as well as for
// 32-bit programs on AIX 6 or better, where AIX kernel saves and restores
// 64-bit registers across context switches and interrupts, even in 32-bit
// programs.  According to IBM developers, this guarantee is not provided by
// the 32-bit ABI when running Linux kernel on POWER hardware, and therefore
// 64-bit atomic operations using gcc __sync_* intrinsics might not operate
// properly in some circumstances on Linux on POWER.  If running on Linux on
// POWER, it is highly recommended that gcc 4.7+ be used, such as the IBM
// Advanced Toolchain (AT) which currently provides gcc 4.8.3 for POWER, or the
// RedHat Developer Toolset (DTS) for POWER.
//
// IMPLEMENTATION NOTE: there are likely excess explicit barriers since gcc
// __sync_* intrinsics may provide their own barriers

#ifndef INCLUDED_BSLS_ATOMICOPERATIONS_DEFAULT
#include <bsls_atomicoperations_default.h>
#endif

#ifndef INCLUDED_BSLS_PLATFORM
#include <bsls_platform.h>
#endif

#ifndef INCLUDED_BSLS_TYPES
#include <bsls_types.h>
#endif

#if defined(BSLS_PLATFORM_CPU_POWERPC) && defined(BSLS_PLATFORM_CMP_GNU)

namespace BloombergLP {

namespace bsls {

struct AtomicOperations_POWERPC_ALL_GCC;
typedef AtomicOperations_POWERPC_ALL_GCC  AtomicOperations_Imp;

         // ==========================================================
         // struct Atomic_TypeTraits<AtomicOperations_POWERPC_ALL_GCC>
         // ==========================================================

template <>
struct Atomic_TypeTraits<AtomicOperations_POWERPC_ALL_GCC>
{
    struct __attribute__((__aligned__(sizeof(int)))) Int
    {
          int d_value;
    };

    struct __attribute__((__aligned__(sizeof(Types::Int64)))) Int64
    {
          Types::Int64 d_value;
    };

    struct __attribute__((__aligned__(sizeof(unsigned int)))) Uint
    {
          unsigned int d_value;
    };

    struct __attribute__((__aligned__(sizeof(Types::Uint64)))) Uint64
    {
          Types::Uint64 d_value;
    };

    struct __attribute__((__aligned__(sizeof(void *)))) Pointer
    {
          void * d_value;
    };
};

                   // =======================================
                   // struct AtomicOperations_POWERPC_ALL_GCC
                   // =======================================

struct AtomicOperations_POWERPC_ALL_GCC
  #ifdef BSLS_PLATFORM_CPU_64_BIT
    : AtomicOperations_Default64<AtomicOperations_POWERPC_ALL_GCC>
  #else
    : AtomicOperations_Default32<AtomicOperations_POWERPC_ALL_GCC>
  #endif
{
    typedef Atomic_TypeTraits<AtomicOperations_POWERPC_ALL_GCC> AtomicTypes;

    typedef char AtomicInt_SizeCheck[sizeof(int) == 4 ? 1 : -1];
        // compile-time assert

        // *** atomic functions for int ***

    static void initInt(AtomicTypes::Int *atomicInt, int value);

    static int getInt(const AtomicTypes::Int *atomicInt);

    static int getIntAcquire(const AtomicTypes::Int *atomicInt);

    static int getIntRelaxed(const AtomicTypes::Int *atomicInt);

    static void setInt(AtomicTypes::Int *atomicInt, int value);

    static void setIntRelease(AtomicTypes::Int *atomicInt, int value);

    static void setIntRelaxed(AtomicTypes::Int *atomicInt, int value);

    static int swapInt(AtomicTypes::Int *atomicInt, int swapValue);

    static int swapIntAcqRel(AtomicTypes::Int *atomicInt, int swapValue);

    static int testAndSwapInt(AtomicTypes::Int *atomicInt,
                              int compareValue,
                              int swapValue);

    static int testAndSwapIntAcqRel(AtomicTypes::Int *atomicInt,
                                    int compareValue,
                                    int swapValue);

    static int addIntNv(AtomicTypes::Int *atomicInt, int value);

    static int addIntNvAcqRel(AtomicTypes::Int *atomicInt, int value);

    static int addIntNvRelaxed(AtomicTypes::Int *atomicInt, int value);

        // *** atomic functions for Int64 ***

    static void initInt64(AtomicTypes::Int64 *atomicInt, Types::Int64 value);

    static Types::Int64 getInt64(const AtomicTypes::Int64 *atomicInt);

    static Types::Int64 getInt64Acquire(const AtomicTypes::Int64 *atomicInt);

    static Types::Int64 getInt64Relaxed(const AtomicTypes::Int64 *atomicInt);

    static void setInt64(AtomicTypes::Int64 *atomicInt, Types::Int64 value);

    static void setInt64Release(AtomicTypes::Int64 *atomicInt,
                                Types::Int64 value);

    static void setInt64Relaxed(AtomicTypes::Int64 *atomicInt,
                                Types::Int64 value);

    static Types::Int64 swapInt64(AtomicTypes::Int64  *atomicInt,
                                  Types::Int64 swapValue);

    static Types::Int64 swapInt64AcqRel(AtomicTypes::Int64  *atomicInt,
                                        Types::Int64 swapValue);

    static Types::Int64 testAndSwapInt64(AtomicTypes::Int64 *atomicInt,
                                         Types::Int64 compareValue,
                                         Types::Int64 swapValue);

    static Types::Int64 testAndSwapInt64AcqRel(AtomicTypes::Int64 *atomicInt,
                                               Types::Int64 compareValue,
                                               Types::Int64 swapValue);

    static Types::Int64 addInt64Nv(AtomicTypes::Int64 *atomicInt,
                                   Types::Int64 value);

    static Types::Int64 addInt64NvAcqRel(AtomicTypes::Int64 *atomicInt,
                                         Types::Int64 value);

    static Types::Int64 addInt64NvRelaxed(AtomicTypes::Int64 *atomicInt,
                                          Types::Int64 value);
};

// ===========================================================================
//                        INLINE FUNCTION DEFINITIONS
// ===========================================================================

                     // ---------------------------------------
                     // struct AtomicOperations_POWERPC_ALL_GCC
                     // ---------------------------------------

inline
void AtomicOperations_POWERPC_ALL_GCC::
    initInt(AtomicTypes::Int *atomicInt, int value)
{
    __asm__ __volatile__ ("stw%U0%X0 %1,%0"
                         :"=m"(atomicInt->d_value)
                         :"r"(value));
}

inline
int AtomicOperations_POWERPC_ALL_GCC::
    getInt(const AtomicTypes::Int *atomicInt)
{
    int rv;
    __asm__ __volatile__ ("sync":::"memory");
    __asm__ __volatile__ ("lwz%U1%X1 %0,%1"
                         :"=r"(rv)
                         :"m"(atomicInt->d_value));
    __asm__ __volatile__ ("lwsync":::"memory");
    return rv;
}

inline
int AtomicOperations_POWERPC_ALL_GCC::
    getIntAcquire(const AtomicTypes::Int *atomicInt)
{
    int rv;
    __asm__ __volatile__ ("lwz%U1%X1 %0,%1"
                         :"=r"(rv)
                         :"m"(atomicInt->d_value));
    __asm__ __volatile__ ("lwsync":::"memory");
    return rv;
}

inline
int AtomicOperations_POWERPC_ALL_GCC::
    getIntRelaxed(const AtomicTypes::Int *atomicInt)
{
    int rv;
    __asm__ __volatile__ ("lwz%U1%X1 %0,%1"
                         :"=r"(rv)
                         :"m"(atomicInt->d_value));
    return rv;
}

inline
void AtomicOperations_POWERPC_ALL_GCC::
    setInt(AtomicTypes::Int *atomicInt, int value)
{
    __asm__ __volatile__ ("sync":::"memory");
    __asm__ __volatile__ ("stw%U0%X0 %1,%0"
                         :"=m"(atomicInt->d_value)
                         :"r"(value));
}

inline
void AtomicOperations_POWERPC_ALL_GCC::
    setIntRelease(AtomicTypes::Int *atomicInt, int value)
{
    __asm__ __volatile__ ("lwsync":::"memory");
    __asm__ __volatile__ ("stw%U0%X0 %1,%0"
                         :"=m"(atomicInt->d_value)
                         :"r"(value));
}

inline
void AtomicOperations_POWERPC_ALL_GCC::
    setIntRelaxed(AtomicTypes::Int *atomicInt, int value)
{
    __asm__ __volatile__ ("stw%U0%X0 %1,%0"
                         :"=m"(atomicInt->d_value)
                         :"r"(value));
}

inline
int AtomicOperations_POWERPC_ALL_GCC::
    swapInt(AtomicTypes::Int *atomicInt, int swapValue)
{
    __asm__ __volatile__ ("sync":::"memory");
    return __sync_lock_test_and_set(&atomicInt->d_value, swapValue);
}

inline
int AtomicOperations_POWERPC_ALL_GCC::
    swapIntAcqRel(AtomicTypes::Int *atomicInt, int swapValue)
{
    __asm__ __volatile__ ("lwsync":::"memory");
    return __sync_lock_test_and_set(&atomicInt->d_value, swapValue);
}

inline
int AtomicOperations_POWERPC_ALL_GCC::
    testAndSwapInt(AtomicTypes::Int *atomicInt,
                   int compareValue, int swapValue)
{
    __asm__ __volatile__ ("sync":::"memory");
    return __sync_val_compare_and_swap(&atomicInt->d_value,
                                       compareValue, swapValue);
}

inline
int AtomicOperations_POWERPC_ALL_GCC::
    testAndSwapIntAcqRel(AtomicTypes::Int *atomicInt,
                         int compareValue, int swapValue)
{
    __asm__ __volatile__ ("lwsync":::"memory");
    return __sync_val_compare_and_swap(&atomicInt->d_value,
                                       compareValue, swapValue);
}

inline
int AtomicOperations_POWERPC_ALL_GCC::
    addIntNv(AtomicTypes::Int *atomicInt, int value)
{
    int rv;
    __asm__ __volatile__ ("sync":::"memory");
    rv = __sync_add_and_fetch(&atomicInt->d_value, value);
    __asm__ __volatile__ ("lwsync":::"memory");
    return rv;
}

inline
int AtomicOperations_POWERPC_ALL_GCC::
    addIntNvAcqRel(AtomicTypes::Int *atomicInt, int value)
{
    int rv;
    __asm__ __volatile__ ("lwsync":::"memory");
    rv = __sync_add_and_fetch(&atomicInt->d_value, value);
    __asm__ __volatile__ ("lwsync":::"memory");
    return rv;
}

inline
int AtomicOperations_POWERPC_ALL_GCC::
    addIntNvRelaxed(AtomicTypes::Int *atomicInt, int value)
{
    return __sync_add_and_fetch(&atomicInt->d_value, value);
}

inline
void AtomicOperations_POWERPC_ALL_GCC::
    initInt64(AtomicTypes::Int64 *atomicInt, Types::Int64 value)
{
    __asm__ __volatile__ ("std%U0%X0 %1,%0"
                         :"=m"(atomicInt->d_value)
                         :"r"(value));
}

inline
Types::Int64 AtomicOperations_POWERPC_ALL_GCC::
    getInt64(const AtomicTypes::Int64 *atomicInt)
{
    Types::Int64 rv;
    __asm__ __volatile__ ("sync":::"memory");
    __asm__ __volatile__ ("ld%U1%X1 %0,%1"
                         :"=r"(rv)
                         :"m"(atomicInt->d_value));
    __asm__ __volatile__ ("lwsync":::"memory");
    return rv;
}

inline
Types::Int64 AtomicOperations_POWERPC_ALL_GCC::
    getInt64Acquire(const AtomicTypes::Int64 *atomicInt)
{
    Types::Int64 rv;
    __asm__ __volatile__ ("ld%U1%X1 %0,%1"
                         :"=r"(rv)
                         :"m"(atomicInt->d_value));
    __asm__ __volatile__ ("lwsync":::"memory");
    return rv;
}

inline
Types::Int64 AtomicOperations_POWERPC_ALL_GCC::
    getInt64Relaxed(const AtomicTypes::Int64 *atomicInt)
{
    Types::Int64 rv;
    __asm__ __volatile__ ("ld%U1%X1 %0,%1"
                         :"=r"(rv)
                         :"m"(atomicInt->d_value));
    return rv;
}

inline
void AtomicOperations_POWERPC_ALL_GCC::
    setInt64(AtomicTypes::Int64 *atomicInt, Types::Int64 value)
{
    __asm__ __volatile__ ("sync":::"memory");
    __asm__ __volatile__ ("std%U0%X0 %1,%0"
                         :"=m"(atomicInt->d_value)
                         :"r"(value));
}

inline
void AtomicOperations_POWERPC_ALL_GCC::
    setInt64Release(AtomicTypes::Int64 *atomicInt, Types::Int64 value)
{
    __asm__ __volatile__ ("lwsync":::"memory");
    __asm__ __volatile__ ("std%U0%X0 %1,%0"
                         :"=m"(atomicInt->d_value)
                         :"r"(value));
}

inline
void AtomicOperations_POWERPC_ALL_GCC::
    setInt64Relaxed(AtomicTypes::Int64 *atomicInt, Types::Int64 value)
{
    __asm__ __volatile__ ("std%U0%X0 %1,%0"
                         :"=m"(atomicInt->d_value)
                         :"r"(value));
}

inline
Types::Int64 AtomicOperations_POWERPC_ALL_GCC::
    swapInt64(AtomicTypes::Int64 *atomicInt, Types::Int64 swapValue)
{
    __asm__ __volatile__ ("sync":::"memory");
    return __sync_lock_test_and_set(&atomicInt->d_value, swapValue);
}

inline
Types::Int64 AtomicOperations_POWERPC_ALL_GCC::
    swapInt64AcqRel(AtomicTypes::Int64 *atomicInt, Types::Int64 swapValue)
{
    __asm__ __volatile__ ("lwsync":::"memory");
    return __sync_lock_test_and_set(&atomicInt->d_value, swapValue);
}

inline
Types::Int64 AtomicOperations_POWERPC_ALL_GCC::
    testAndSwapInt64(AtomicTypes::Int64 *atomicInt,
                     Types::Int64 compareValue, Types::Int64 swapValue)
{
    __asm__ __volatile__ ("sync":::"memory");
    return __sync_val_compare_and_swap(&atomicInt->d_value,
                                       compareValue, swapValue);
}

inline
Types::Int64 AtomicOperations_POWERPC_ALL_GCC::
    testAndSwapInt64AcqRel(AtomicTypes::Int64 *atomicInt,
                           Types::Int64 compareValue, Types::Int64 swapValue)
{
    __asm__ __volatile__ ("lwsync":::"memory");
    return __sync_val_compare_and_swap(&atomicInt->d_value,
                                       compareValue, swapValue);
}

inline
Types::Int64 AtomicOperations_POWERPC_ALL_GCC::
    addInt64Nv(AtomicTypes::Int64 *atomicInt, Types::Int64 value)
{
    Types::Int64 rv;
    __asm__ __volatile__ ("sync":::"memory");
    rv = __sync_add_and_fetch(&atomicInt->d_value, value);
    __asm__ __volatile__ ("lwsync":::"memory");
    return rv;
}

inline
Types::Int64 AtomicOperations_POWERPC_ALL_GCC::
    addInt64NvAcqRel(AtomicTypes::Int64 *atomicInt, Types::Int64 value)
{
    Types::Int64 rv;
    __asm__ __volatile__ ("lwsync":::"memory");
    rv = __sync_add_and_fetch(&atomicInt->d_value, value);
    __asm__ __volatile__ ("lwsync":::"memory");
    return rv;
}

inline
Types::Int64 AtomicOperations_POWERPC_ALL_GCC::
    addInt64NvRelaxed(AtomicTypes::Int64 *atomicInt, Types::Int64 value)
{
    return __sync_add_and_fetch(&atomicInt->d_value, value);
}

}  // close package namespace

}  // close enterprise namespace

#endif  // BSLS_PLATFORM_CPU_POWERPC && BSLS_PLATFORM_CMP_GNU

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
