// bsls_atomicoperations_sparc64_sun_cc.h                             -*-C++-*-
#ifndef INCLUDED_BSLS_ATOMICOPERATIONS_SPARC64_SUN_CC
#define INCLUDED_BSLS_ATOMICOPERATIONS_SPARC64_SUN_CC

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide implementations of atomic operations for Sparc/Sun (64bit).
//
//@CLASSES:
//  bsls::AtomicOperations_SPARC64_SUN_CC: atomics for Sparc64/Sun.
//
//@DESCRIPTION: This component provides classes necessary to implement atomics
// on the Sun Sparc platform in 64bit mode with SunCC compiler.  The classes
// are for private use only.  See 'bsls_atomicoperations' and 'bsls_atomic' for
// the public interface to atomics.

#ifndef INCLUDED_BSLS_ATOMICOPERATIONS_DEFAULT
#include <bsls_atomicoperations_default.h>
#endif

#ifndef INCLUDED_BSLS_ATOMICOPERATIONS_SPARC_SUN_CC_DEFAULT
#include <bsls_atomicoperations_sparc_sun_cc_default.h>
#endif

#ifndef INCLUDED_BSLS_PLATFORM
#include <bsls_platform.h>
#endif

#ifndef INCLUDED_BSLS_TYPES
#include <bsls_types.h>
#endif

#if defined(BSLS_PLATFORM_CPU_SPARC_V9) \
    && (defined(BSLS_PLATFORM_CMP_GNU) || defined(BSLS_PLATFORM_CMP_SUN))

namespace BloombergLP {

extern "C"
{
                         // *** 64 bit operations ***

    // CLASS METHODS
    bsls::Types::Int64 bsls_AtomicOperations_Sparc64_GetInt64(
            const volatile bsls::Types::Int64 *atomicInt);

    void bsls_AtomicOperations_Sparc64_SetInt64(
            volatile bsls::Types::Int64 *atomicInt,
            bsls::Types::Int64 value);

    bsls::Types::Int64 bsls_AtomicOperations_Sparc64_SwapInt64(
            volatile bsls::Types::Int64 *atomicInt,
            bsls::Types::Int64 swapValue);

    bsls::Types::Int64 bsls_AtomicOperations_Sparc64_SwapInt64AcqRel(
            volatile bsls::Types::Int64 *atomicInt,
            bsls::Types::Int64 swapValue);

    bsls::Types::Int64 bsls_AtomicOperations_Sparc64_TestAndSwapInt64(
            volatile bsls::Types::Int64 *atomicInt,
            bsls::Types::Int64 compareValue,
            bsls::Types::Int64 swapValue);

    bsls::Types::Int64 bsls_AtomicOperations_Sparc64_TestAndSwapInt64AcqRel(
            volatile bsls::Types::Int64 *atomicInt,
            bsls::Types::Int64 compareValue,
            bsls::Types::Int64 swapValue);

    bsls::Types::Int64 bsls_AtomicOperations_Sparc64_AddInt64(
            volatile bsls::Types::Int64 *atomicInt,
            bsls::Types::Int64 value);

    bsls::Types::Int64 bsls_AtomicOperations_Sparc64_AddInt64Relaxed(
            volatile bsls::Types::Int64 *atomicInt,
            bsls::Types::Int64 value);
}

namespace bsls {

struct AtomicOperations_SPARC64_SUN_CC;
typedef AtomicOperations_SPARC64_SUN_CC  AtomicOperations_Imp;

          // =========================================================
          // struct Atomic_TypeTraits<AtomicOperations_SPARC64_SUN_CC>
          // =========================================================

template <>
struct Atomic_TypeTraits<AtomicOperations_SPARC64_SUN_CC>
{
    struct Int
    {
#ifdef BSLS_PLATFORM_CMP_GNU
        volatile int d_value __attribute__((__aligned__(sizeof(int))));
#else
#       pragma align 4 (d_value)
        volatile int d_value;
#endif
    };

    struct Int64
    {
#ifdef BSLS_PLATFORM_CMP_GNU
        volatile Types::Int64 d_value
                       __attribute__((__aligned__(sizeof(Types::Int64))));
#else
#       pragma align 8 (d_value)
        volatile Types::Int64 d_value;
#endif
    };

    struct Pointer
    {
#ifdef BSLS_PLATFORM_CMP_GNU
        void const * volatile d_value
                                  __attribute__((__aligned__(sizeof(void *))));
#else
#       pragma align 8 (d_value)
        void const * volatile d_value;
#endif
    };
};

                   // ======================================
                   // struct AtomicOperations_SPARC64_SUN_CC
                   // ======================================

struct AtomicOperations_SPARC64_SUN_CC
    : AtomicOperations_SPARC_SUN_CC_Default64<AtomicOperations_SPARC64_SUN_CC>
{
    typedef Atomic_TypeTraits<AtomicOperations_SPARC64_SUN_CC> AtomicTypes;

                    // *** atomic functions for Int64 ***

    // CLASS METHODS
    static Types::Int64 getInt64(const AtomicTypes::Int64 *atomicInt);

    static Types::Int64 getInt64Acquire(const AtomicTypes::Int64 *atomicInt);

    static void setInt64(AtomicTypes::Int64 *atomicInt, Types::Int64 value);

    static void setInt64Release(AtomicTypes::Int64 *atomicInt,
                                Types::Int64 value);

    static Types::Int64 swapInt64(AtomicTypes::Int64  *atomicInt,
                                  Types::Int64   swapValue);

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

    static Types::Int64 addInt64NvRelaxed(AtomicTypes::Int64 *atomicInt,
                                          Types::Int64 value);

    static Types::Int64 addInt64NvAcqRel(AtomicTypes::Int64 *atomicInt,
                                         Types::Int64 value);
};

// ===========================================================================
//                        INLINE FUNCTION DEFINITIONS
// ===========================================================================

                   // --------------------------------------
                   // struct AtomicOperations_SPARC64_SUN_CC
                   // --------------------------------------

inline
Types::Int64 AtomicOperations_SPARC64_SUN_CC::
    getInt64(const AtomicTypes::Int64 *atomicInt)
{
    return bsls_AtomicOperations_Sparc64_GetInt64(&atomicInt->d_value);
}

inline
Types::Int64 AtomicOperations_SPARC64_SUN_CC::
    getInt64Acquire(const AtomicTypes::Int64 *atomicInt)
{
    return getInt64Relaxed(atomicInt);
}

inline
void AtomicOperations_SPARC64_SUN_CC::
    setInt64(AtomicTypes::Int64 *atomicInt, Types::Int64 value)
{
    bsls_AtomicOperations_Sparc64_SetInt64(&atomicInt->d_value, value);
}

inline
void AtomicOperations_SPARC64_SUN_CC::
    setInt64Release(AtomicTypes::Int64 *atomicInt, Types::Int64 value)
{
    setInt64Relaxed(atomicInt, value);
}

inline
Types::Int64 AtomicOperations_SPARC64_SUN_CC::
    swapInt64(AtomicTypes::Int64 *atomicInt,
              Types::Int64 swapValue)
{
    return bsls_AtomicOperations_Sparc64_SwapInt64(&atomicInt->d_value,
                                                   swapValue);
}

inline
Types::Int64 AtomicOperations_SPARC64_SUN_CC::
    swapInt64AcqRel(AtomicTypes::Int64 *atomicInt,
                    Types::Int64 swapValue)
{
    return bsls_AtomicOperations_Sparc64_SwapInt64AcqRel(&atomicInt->d_value,
                                                         swapValue);
}

inline
Types::Int64 AtomicOperations_SPARC64_SUN_CC::
    testAndSwapInt64(AtomicTypes::Int64 *atomicInt,
                     Types::Int64 compareValue,
                     Types::Int64 swapValue)
{
    return bsls_AtomicOperations_Sparc64_TestAndSwapInt64(&atomicInt->d_value,
                                                          compareValue,
                                                          swapValue);
}

inline
Types::Int64 AtomicOperations_SPARC64_SUN_CC::
    testAndSwapInt64AcqRel(AtomicTypes::Int64 *atomicInt,
                           Types::Int64 compareValue,
                           Types::Int64 swapValue)
{
    return bsls_AtomicOperations_Sparc64_TestAndSwapInt64AcqRel(
                                                    &atomicInt->d_value,
                                                    compareValue,
                                                    swapValue);
}

inline
Types::Int64 AtomicOperations_SPARC64_SUN_CC::
    addInt64Nv(AtomicTypes::Int64 *atomicInt,
               Types::Int64 value)
{
    return bsls_AtomicOperations_Sparc64_AddInt64(&atomicInt->d_value, value);
}

inline
Types::Int64 AtomicOperations_SPARC64_SUN_CC::
    addInt64NvRelaxed(AtomicTypes::Int64 *atomicInt,
                      Types::Int64 value)
{
    return bsls_AtomicOperations_Sparc64_AddInt64Relaxed(&atomicInt->d_value,
                                                         value);
}

inline
Types::Int64 AtomicOperations_SPARC64_SUN_CC::
    addInt64NvAcqRel(AtomicTypes::Int64 *atomicInt,
                     Types::Int64 value)
{
    return addInt64NvRelaxed(atomicInt, value);
}

}  // close package namespace

}  // close enterprise namespace

#endif  // SPARC_V9 && (GNU || SUN)

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
