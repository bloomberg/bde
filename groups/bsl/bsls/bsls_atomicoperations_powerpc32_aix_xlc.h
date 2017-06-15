// bsls_atomicoperations_powerpc32_aix_xlc.h                          -*-C++-*-
#ifndef INCLUDED_BSLS_ATOMICOPERATIONS_POWERPC32_AIX_XLC
#define INCLUDED_BSLS_ATOMICOPERATIONS_POWERPC32_AIX_XLC

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide implementations of atomic operations for PowerPC/AIX-32bit.
//
//@CLASSES:
//  bsls::AtomicOperations_POWERPC32_AIX_XLC: atomics for PPC32/AIX.
//
//@DESCRIPTION: This component provides classes necessary to implement atomics
// on the AIX PowerPC platform in 32bit mode with the xlC compiler.  The
// classes are for private use only.  See 'bsls_atomicoperations' and
// 'bsls_atomic' for the public interface to atomics.

#ifndef INCLUDED_BSLS_ATOMICOPERATIONS_POWERPC_AIX_XLC_DEFAULT
#include <bsls_atomicoperations_powerpc_aix_xlc_default.h>
#endif

#ifndef INCLUDED_BSLS_PLATFORM
#include <bsls_platform.h>
#endif

#ifndef INCLUDED_BSLS_TYPES
#include <bsls_types.h>
#endif

#if defined(BSLS_PLATFORM_CPU_POWERPC) && defined(BSLS_PLATFORM_CMP_IBM) \
    && defined(BSLS_PLATFORM_CPU_32_BIT)

namespace BloombergLP {

namespace bsls {

    // *** functions implemented in assembly ***

Types::Int64 AtomicOperations_Powerpc32_GetInt64(
                                  const volatile Types::Int64 *atomicInt);
Types::Int64 AtomicOperations_Powerpc32_GetInt64Relaxed(
                                  const volatile Types::Int64 *atomicInt);
Types::Int64 AtomicOperations_Powerpc32_GetInt64Acquire(
                                  const volatile Types::Int64 *atomicInt);

}  // close package namespace

    // *** extern "C" functions due to prototype hacks ***

extern "C"
void bsls_AtomicOperations_Powerpc32_SetInt64(
                                        volatile bsls::Types::Int64 *atomicInt,
                                        bsls::Types::Int64 value);

extern "C"
void bsls_AtomicOperations_Powerpc32_SetInt64Relaxed(
                                        volatile bsls::Types::Int64 *atomicInt,
                                        bsls::Types::Int64 value);

extern "C"
void bsls_AtomicOperations_Powerpc32_SetInt64Release(
                                        volatile bsls::Types::Int64 *atomicInt,
                                        bsls::Types::Int64 value);

extern "C"
bsls::Types::Int64 bsls_AtomicOperations_Powerpc32_SwapInt64(
                                        volatile bsls::Types::Int64 *atomicInt,
                                        bsls::Types::Int64 value);

extern "C"
bsls::Types::Int64 bsls_AtomicOperations_Powerpc32_SwapInt64AcqRel(
                                        volatile bsls::Types::Int64 *atomicInt,
                                        bsls::Types::Int64 value);

extern "C"
bsls::Types::Int64 bsls_AtomicOperations_Powerpc32_testAndSwapInt64(
                                     volatile bsls::Types::Int64 *atomicInt,
                                     bsls::Types::Int64 compareValue,
                                     bsls::Types::Int64 swapValue);

extern "C"
bsls::Types::Int64 bsls_AtomicOperations_Powerpc32_testAndSwapInt64AcqRel(
                                     volatile bsls::Types::Int64 *atomicInt,
                                     bsls::Types::Int64 compareValue,
                                     bsls::Types::Int64 swapValue);

extern "C"
bsls::Types::Int64 bsls_AtomicOperations_Powerpc32_AddInt64(
                                        volatile bsls::Types::Int64 *atomicInt,
                                        bsls::Types::Int64 value);

extern "C"
bsls::Types::Int64 bsls_AtomicOperations_Powerpc32_AddInt64Relaxed(
                                        volatile bsls::Types::Int64 *atomicInt,
                                        bsls::Types::Int64 value);

extern "C"
bsls::Types::Int64 bsls_AtomicOperations_Powerpc32_AddInt64AcqRel(
                                        volatile bsls::Types::Int64 *atomicInt,
                                        bsls::Types::Int64 value);

namespace bsls {

struct AtomicOperations_POWERPC32_AIX_XLC;
typedef AtomicOperations_POWERPC32_AIX_XLC  AtomicOperations_Imp;

        // ============================================================
        // struct Atomic_TypeTraits<AtomicOperations_POWERPC32_AIX_XLC>
        // ============================================================

template <>
struct Atomic_TypeTraits<AtomicOperations_POWERPC32_AIX_XLC>
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

                  // =========================================
                  // struct AtomicOperations_POWERPC32_AIX_XLC
                  // =========================================

struct AtomicOperations_POWERPC32_AIX_XLC
    : AtomicOperations_POWERPC_AIX_XLC_Default32<
                                       AtomicOperations_POWERPC32_AIX_XLC>
{
    typedef Atomic_TypeTraits<AtomicOperations_POWERPC32_AIX_XLC>
        AtomicTypes;

        //  *** atomic functions for Int64 ***

    static Types::Int64 getInt64(const AtomicTypes::Int64 *atomicInt);

    static Types::Int64 getInt64Relaxed(const AtomicTypes::Int64 *atomicInt);

    static Types::Int64 getInt64Acquire(const AtomicTypes::Int64 *atomicInt);

    static void setInt64(AtomicTypes::Int64 *atomicInt, Types::Int64 value);

    static void setInt64Relaxed(AtomicTypes::Int64 *atomicInt,
                                Types::Int64 value);

    static void setInt64Release(AtomicTypes::Int64 *atomicInt,
                                Types::Int64 value);

    static Types::Int64 swapInt64(AtomicTypes::Int64  *atomicInt,
                                  Types::Int64 swapValue);

    static Types::Int64 swapInt64AcqRel(AtomicTypes::Int64  *atomicInt,
                                        Types::Int64 swapValue);

    static Types::Int64 testAndSwapInt64(AtomicTypes::Int64   *atomicInt,
                                         Types::Int64 compareValue,
                                         Types::Int64 swapValue);

    static Types::Int64 testAndSwapInt64AcqRel(AtomicTypes::Int64   *atomicInt,
                                               Types::Int64 compareValue,
                                               Types::Int64 swapValue);

    static Types::Int64 addInt64Nv(AtomicTypes::Int64 *atomicInt,
                                   Types::Int64 value);

    static Types::Int64 addInt64NvRelaxed(AtomicTypes::Int64  *atomicInt,
                                          Types::Int64 value);

    static Types::Int64 addInt64NvAcqRel(AtomicTypes::Int64 *atomicInt,
                                         Types::Int64 value);
};

// ===========================================================================
//                        INLINE FUNCTION DEFINITIONS
// ===========================================================================

                  // -----------------------------------------
                  // struct AtomicOperations_POWERPC32_AIX_XLC
                  // -----------------------------------------

inline
Types::Int64 AtomicOperations_POWERPC32_AIX_XLC::
    getInt64(const AtomicTypes::Int64 *atomicInt)
{
    return AtomicOperations_Powerpc32_GetInt64(&atomicInt->d_value);
}

inline
Types::Int64 AtomicOperations_POWERPC32_AIX_XLC::
    getInt64Relaxed(const AtomicTypes::Int64 *atomicInt)
{
    return AtomicOperations_Powerpc32_GetInt64Relaxed(&atomicInt->d_value);
}

inline
Types::Int64 AtomicOperations_POWERPC32_AIX_XLC::
    getInt64Acquire(const AtomicTypes::Int64 *atomicInt)
{
    return AtomicOperations_Powerpc32_GetInt64Acquire(&atomicInt->d_value);
}

inline
void AtomicOperations_POWERPC32_AIX_XLC::
    setInt64(AtomicTypes::Int64 *atomicInt, Types::Int64 value)
{
    bsls_AtomicOperations_Powerpc32_SetInt64(&atomicInt->d_value, value);
}

inline
void AtomicOperations_POWERPC32_AIX_XLC::
    setInt64Relaxed(AtomicTypes::Int64 *atomicInt, Types::Int64 value)
{
    bsls_AtomicOperations_Powerpc32_SetInt64Relaxed(&atomicInt->d_value,
                                                    value);
}

inline
void AtomicOperations_POWERPC32_AIX_XLC::
    setInt64Release(AtomicTypes::Int64 *atomicInt, Types::Int64 value)
{
    bsls_AtomicOperations_Powerpc32_SetInt64Release(&atomicInt->d_value,
                                                    value);
}

inline
Types::Int64 AtomicOperations_POWERPC32_AIX_XLC::
    swapInt64(AtomicTypes::Int64  *atomicInt,
              Types::Int64 swapValue)
{
    return bsls_AtomicOperations_Powerpc32_SwapInt64(&atomicInt->d_value,
                                                     swapValue);
}

inline
Types::Int64 AtomicOperations_POWERPC32_AIX_XLC::
    swapInt64AcqRel(AtomicTypes::Int64  *atomicInt,
                    Types::Int64 swapValue)
{
    return bsls_AtomicOperations_Powerpc32_SwapInt64AcqRel(&atomicInt->d_value,
                                                           swapValue);
}

inline
Types::Int64 AtomicOperations_POWERPC32_AIX_XLC::
    testAndSwapInt64(AtomicTypes::Int64   *atomicInt,
                     Types::Int64 compareValue,
                     Types::Int64 swapValue)
{
    return bsls_AtomicOperations_Powerpc32_testAndSwapInt64(
                                                           &atomicInt->d_value,
                                                           compareValue,
                                                           swapValue);
}

inline
Types::Int64 AtomicOperations_POWERPC32_AIX_XLC::
    testAndSwapInt64AcqRel(AtomicTypes::Int64   *atomicInt,
                           Types::Int64 compareValue,
                           Types::Int64 swapValue)
{
    return bsls_AtomicOperations_Powerpc32_testAndSwapInt64AcqRel(
                                                        &atomicInt->d_value,
                                                        compareValue,
                                                        swapValue);
}

inline
Types::Int64 AtomicOperations_POWERPC32_AIX_XLC::
    addInt64Nv(AtomicTypes::Int64 *atomicInt,
               Types::Int64 value)
{
    return bsls_AtomicOperations_Powerpc32_AddInt64(&atomicInt->d_value,
                                                    value);
}

inline
Types::Int64 AtomicOperations_POWERPC32_AIX_XLC::
    addInt64NvRelaxed(AtomicTypes::Int64  *atomicInt,
                      Types::Int64 value)
{
    return bsls_AtomicOperations_Powerpc32_AddInt64Relaxed(&atomicInt->d_value,
                                                           value);
}

inline
Types::Int64 AtomicOperations_POWERPC32_AIX_XLC::
    addInt64NvAcqRel(AtomicTypes::Int64 *atomicInt,
                     Types::Int64 value)
{
    return bsls_AtomicOperations_Powerpc32_AddInt64AcqRel(&atomicInt->d_value,
                                                          value);
}

}  // close package namespace

}  // close enterprise namespace

#endif

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
