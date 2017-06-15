// bsls_atomicoperations_powerpc_aix_xlc_default.h                    -*-C++-*-
#ifndef INCLUDED_BSLS_ATOMICOPERATIONS_POWERPC_AIX_XLC_DEFAULT
#define INCLUDED_BSLS_ATOMICOPERATIONS_POWERPC_AIX_XLC_DEFAULT

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide default base implementations of atomics for PowerPC/AIX.
//
//@CLASSES:
//  bsls::AtomicOperations_POWERPC_AIX_XLC_Default32: default base for 32bit.
//  bsls::AtomicOperations_POWERPC_AIX_XLC_Default64: default base for 64bit.
//
//@DESCRIPTION: This component provides default base classes necessary to
// implement atomics on the AIX PowerPC platform with the xlC compiler.  The
// classes are for private use only.  See 'bsls_atomicoperations' and
// 'bsls_atomic' for the public interface to atomics.

#ifndef INCLUDED_BSLS_ATOMICOPERATIONS_DEFAULT
#include <bsls_atomicoperations_default.h>
#endif

#ifndef INCLUDED_BSLS_PLATFORM
#include <bsls_platform.h>
#endif

#ifndef INCLUDED_BSLS_TYPES
#include <bsls_types.h>
#endif

#if defined(BSLS_PLATFORM_CPU_POWERPC) && defined(BSLS_PLATFORM_CMP_IBM)

#include <builtins.h>

namespace BloombergLP {

namespace bsls {

    // *** functions implemented in assembly for both 32bit and 64bit ***

int AtomicOperations_Powerpc_GetInt(const volatile int *atomicInt);
int AtomicOperations_Powerpc_GetIntAcquire(const volatile int *atomicInt);
void AtomicOperations_Powerpc_SetInt(volatile int *atomicInt, int value);
void AtomicOperations_Powerpc_SetIntRelease(volatile int *atomicInt,
                                            int value);
int AtomicOperations_Powerpc_SwapInt(volatile int *atomicInt,
                                     int swapValue);
int AtomicOperations_Powerpc_SwapIntAcqRel(volatile int *atomicInt,
                                           int swapValue);
int AtomicOperations_Powerpc_TestAndSwapInt(volatile int *atomicInt,
                                            int compareValue,
                                            int swapValue);
int AtomicOperations_Powerpc_TestAndSwapIntAcqRel(volatile int *atomicInt,
                                                  int compareValue,
                                                  int swapValue);
int AtomicOperations_Powerpc_AddInt(volatile int *atomicInt, int value);
int AtomicOperations_Powerpc_AddIntAcqRel(volatile int *atomicInt,
                                          int value);

             // ==================================================
             // struct AtomicOperations_POWERPC_AIX_XLC_DefaultInt
             // ==================================================

template <typename IMP>
struct AtomicOperations_POWERPC_AIX_XLC_DefaultInt
    : AtomicOperations_DefaultInt<IMP>
{
    typedef Atomic_TypeTraits<IMP> AtomicTypes;

        // *** atomic functions for int ***

    static int getInt(typename AtomicTypes::Int const *atomicInt);

    static int getIntAcquire(typename AtomicTypes::Int const *atomicInt);

    static void setInt(typename AtomicTypes::Int *atomicInt, int value);

    static void setIntRelease(typename AtomicTypes::Int *atomicInt, int value);

    static int swapInt(typename AtomicTypes::Int *atomicInt, int swapValue);

    static int swapIntAcqRel(typename AtomicTypes::Int *atomicInt, int swapValue);

    static int testAndSwapInt(typename AtomicTypes::Int *atomicInt,
                              int compareValue,
                              int swapValue);

    static int testAndSwapIntAcqRel(typename AtomicTypes::Int *atomicInt,
                                    int compareValue,
                                    int swapValue);

    static int addIntNv(typename AtomicTypes::Int *atomicInt, int value);

    static int addIntNvRelaxed(typename AtomicTypes::Int *atomicInt, int value);

    static int addIntNvAcqRel(typename AtomicTypes::Int *atomicInt, int value);
};

              // =================================================
              // struct AtomicOperations_POWERPC_AIX_XLC_Default32
              // =================================================

template <typename IMP>
struct AtomicOperations_POWERPC_AIX_XLC_Default32
    : AtomicOperations_POWERPC_AIX_XLC_DefaultInt<IMP>
    , AtomicOperations_DefaultInt64<IMP>
    , AtomicOperations_DefaultUint<IMP>
    , AtomicOperations_DefaultUint64<IMP>
    , AtomicOperations_DefaultPointer32<IMP>
{};

              // =================================================
              // struct AtomicOperations_POWERPC_AIX_XLC_Default64
              // =================================================

template <typename IMP>
struct AtomicOperations_POWERPC_AIX_XLC_Default64
    : AtomicOperations_POWERPC_AIX_XLC_DefaultInt<IMP>
    , AtomicOperations_DefaultInt64<IMP>
    , AtomicOperations_DefaultUint<IMP>
    , AtomicOperations_DefaultUint64<IMP>
    , AtomicOperations_DefaultPointer64<IMP>
{};

// ===========================================================================
//                        INLINE FUNCTION DEFINITIONS
// ===========================================================================

             // --------------------------------------------------
             // struct AtomicOperations_POWERPC_AIX_XLC_DefaultInt
             // --------------------------------------------------

template <typename IMP>
inline
int AtomicOperations_POWERPC_AIX_XLC_DefaultInt<IMP>::
    getInt(typename AtomicTypes::Int const *atomicInt)
{
    return AtomicOperations_Powerpc_GetInt(&atomicInt->d_value);
}

template <typename IMP>
inline
int AtomicOperations_POWERPC_AIX_XLC_DefaultInt<IMP>::
    getIntAcquire(typename AtomicTypes::Int const *atomicInt)
{
    return AtomicOperations_Powerpc_GetIntAcquire(&atomicInt->d_value);
}

template <typename IMP>
inline
void AtomicOperations_POWERPC_AIX_XLC_DefaultInt<IMP>::
    setInt(typename AtomicTypes::Int *atomicInt, int value)
{
    AtomicOperations_Powerpc_SetInt(&atomicInt->d_value, value);
}

template <typename IMP>
inline
void AtomicOperations_POWERPC_AIX_XLC_DefaultInt<IMP>::
    setIntRelease(typename AtomicTypes::Int *atomicInt, int value)
{
    AtomicOperations_Powerpc_SetIntRelease(&atomicInt->d_value, value);
}

template <typename IMP>
inline
int AtomicOperations_POWERPC_AIX_XLC_DefaultInt<IMP>::
    swapInt(typename AtomicTypes::Int *atomicInt, int swapValue)
{
    return AtomicOperations_Powerpc_SwapInt(&atomicInt->d_value,
                                            swapValue);
}

template <typename IMP>
inline
int AtomicOperations_POWERPC_AIX_XLC_DefaultInt<IMP>::
    swapIntAcqRel(typename AtomicTypes::Int *atomicInt, int swapValue)
{
    return AtomicOperations_Powerpc_SwapIntAcqRel(&atomicInt->d_value,
                                                  swapValue);
}

template <typename IMP>
inline
int AtomicOperations_POWERPC_AIX_XLC_DefaultInt<IMP>::
    testAndSwapInt(typename AtomicTypes::Int *atomicInt,
                   int compareValue,
                   int swapValue)
{
    return AtomicOperations_Powerpc_TestAndSwapInt(&atomicInt->d_value,
                                                   compareValue,
                                                   swapValue);
}

template <typename IMP>
inline
int AtomicOperations_POWERPC_AIX_XLC_DefaultInt<IMP>::
    testAndSwapIntAcqRel(typename AtomicTypes::Int *atomicInt,
                         int compareValue,
                         int swapValue)
{
    return AtomicOperations_Powerpc_TestAndSwapIntAcqRel(
                                                    &atomicInt->d_value,
                                                    compareValue,
                                                    swapValue);
}

template <typename IMP>
inline
int AtomicOperations_POWERPC_AIX_XLC_DefaultInt<IMP>::
    addIntNv(typename AtomicTypes::Int *atomicInt, int value)
{
    return AtomicOperations_Powerpc_AddInt(&atomicInt->d_value, value);
}

template <typename IMP>
inline
int AtomicOperations_POWERPC_AIX_XLC_DefaultInt<IMP>::
    addIntNvRelaxed(typename AtomicTypes::Int *atomicInt, int value)
{
    return __fetch_and_add(&atomicInt->d_value, value) + value;
}

template <typename IMP>
inline
int AtomicOperations_POWERPC_AIX_XLC_DefaultInt<IMP>::
    addIntNvAcqRel(typename AtomicTypes::Int *atomicInt, int value)
{
    return AtomicOperations_Powerpc_AddIntAcqRel(&atomicInt->d_value,
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
