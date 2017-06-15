// bsls_atomicoperations_ia64_hp_acc.h                                -*-C++-*-
#ifndef INCLUDED_BSLS_ATOMICOPERATIONS_IA64_HP_ACC
#define INCLUDED_BSLS_ATOMICOPERATIONS_IA64_HP_ACC

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide implementations of atomic operations for HPUX/IA64.
//
//@CLASSES:
//  bsls::AtomicOperations_IA64_HP_ACC: atomics implementation for HPUX/IA64.
//
//@DESCRIPTION: This component provides classes necessary to implement atomics
// on the HPUX/IA64 platform in 32 and 64bit modes.  The classes are for
// private use only.  See 'bsls_atomicoperations' and 'bsls_atomic' for the
// public interface to atomics.

#ifndef INCLUDED_BSLS_ATOMICOPERATIONS_DEFAULT
#include <bsls_atomicoperations_default.h>
#endif

#ifndef INCLUDED_BSLS_PLATFORM
#include <bsls_platform.h>
#endif

#ifndef INCLUDED_BSLS_TYPES
#include <bsls_types.h>
#endif

#if defined(BSLS_PLATFORM_CPU_IA64) && defined(BSLS_PLATFORM_OS_HPUX)

#include <machine/sys/inline.h>

// For reference implementation of atomics on HP-UX/IA64 see:
// http://h21007.www2.hp.com/portal/download/files/unprot/itanium/spinlocks.pdf
// and the Itanium Architecture Developer's Manual:
// http://www.intel.com/content/www/us/en/processors/itanium/
//                   itanium-architecture-vol-1-2-3-4-reference-set-manual.html
//
// Notes:
// * const_cast is used where necessary to drop the volatile qualifier to avoid
//   generating unnecessary ld.acq.
// * casts to (unsigned) in assembler intrinsics is used to avoid signed
//   int->int64 promotion.

namespace BloombergLP {

namespace bsls {

struct AtomicOperations_IA64_HP_ACC;
typedef AtomicOperations_IA64_HP_ACC  AtomicOperations_Imp;

           // ======================================================
           // struct Atomic_TypeTraits<AtomicOperations_IA64_HP_ACC>
           // ======================================================

template<>
struct Atomic_TypeTraits<AtomicOperations_IA64_HP_ACC>
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

#if defined(BSLS_PLATFORM_CPU_64_BIT)

template <typename IMP>
struct AtomicOperations_IA64_HP_ACC_Default
    : AtomicOperations_Default64<IMP>
{};

#else

template <typename IMP>
struct AtomicOperations_IA64_HP_ACC_Default
    : AtomicOperations_Default32<IMP>
{};

#endif

                     // ===================================
                     // struct AtomicOperations_IA64_HP_ACC
                     // ===================================

struct AtomicOperations_IA64_HP_ACC
    : AtomicOperations_IA64_HP_ACC_Default<AtomicOperations_IA64_HP_ACC>
{
  private:
    // release semantics
    static const _Asm_fence d_upfence   = _Asm_fence( _UP_MEM_FENCE
                                                    | _UP_ALU_FENCE
                                                    | _UP_CALL_FENCE
                                                    | _UP_BR_FENCE);

    // acquire semantics
    static const _Asm_fence d_downfence = _Asm_fence( _DOWN_MEM_FENCE
                                                    | _DOWN_ALU_FENCE
                                                    | _DOWN_CALL_FENCE
                                                    | _DOWN_BR_FENCE);

    static const _Asm_fence d_fullfence = _Asm_fence(d_upfence | d_downfence);

  public:
    typedef Atomic_TypeTraits<AtomicOperations_IA64_HP_ACC> AtomicTypes;

        // *** atomic functions for int ***

    static int getInt(const AtomicTypes::Int *atomicInt);

    static int getIntAcquire(const AtomicTypes::Int *atomicInt);

    static void setInt(AtomicTypes::Int *atomicInt, int value);

    static void setIntRelease(AtomicTypes::Int *atomicInt, int value);

    static int swapInt(AtomicTypes::Int *atomicInt, int swapValue);

    static int testAndSwapInt(AtomicTypes::Int *atomicInt,
                              int compareValue,
                              int swapValue);

    static int testAndSwapIntAcqRel(AtomicTypes::Int *atomicInt,
                                    int compareValue,
                                    int swapValue);

    static int addIntNv(AtomicTypes::Int *atomicInt, int value);

    static int addIntNvAcqRel(AtomicTypes::Int *atomicInt, int value);

        // *** atomic functions for Int64 ***

    static Types::Int64 getInt64(const AtomicTypes::Int64 *atomicInt);

    static Types::Int64 getInt64Acquire(const AtomicTypes::Int64 *atomicInt);

    static void setInt64(AtomicTypes::Int64 *atomicInt, Types::Int64 value);

    static void setInt64Release(AtomicTypes::Int64 *atomicInt,
                                Types::Int64 value);

    static Types::Int64 swapInt64(AtomicTypes::Int64 *atomicInt,
                                  Types::Int64  swapValue);

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
};

// ===========================================================================
//                        INLINE FUNCTION DEFINITIONS
// ===========================================================================

                     // -----------------------------------
                     // struct AtomicOperations_IA64_HP_ACC
                     // -----------------------------------

inline
int AtomicOperations_IA64_HP_ACC::
    swapInt(AtomicTypes::Int *atomicInt, int swapValue)
{
    _Asm_mf();  // need mf because xchg has acquire semantics only
    return int(_Asm_xchg(_SZ_W,
                         atomicInt,
                         (unsigned) swapValue,
                         _LDHINT_NONE,
                         d_fullfence));
}

inline
int AtomicOperations_IA64_HP_ACC::
    testAndSwapInt(AtomicTypes::Int *atomicInt,
                   int compareValue,
                   int swapValue)
{
    _Asm_mov_to_ar(_AREG_CCV, (unsigned) compareValue);
    _Asm_mf();
    return int(_Asm_cmpxchg(_SZ_W,
                            _SEM_REL,
                            atomicInt,
                            (unsigned) swapValue,
                            _LDHINT_NONE,
                            d_fullfence));
}

inline
int AtomicOperations_IA64_HP_ACC::
    testAndSwapIntAcqRel(AtomicTypes::Int *atomicInt,
                         int compareValue,
                         int swapValue)
{
    _Asm_mov_to_ar(_AREG_CCV, (unsigned) compareValue);
    return int(_Asm_cmpxchg(_SZ_W,
                            _SEM_REL,
                            atomicInt,
                            (unsigned) swapValue,
                            _LDHINT_NONE,
                            d_upfence));
}

inline
int AtomicOperations_IA64_HP_ACC::
    addIntNv(AtomicTypes::Int *atomicInt, int value)
{
    int actual = const_cast<int &>(atomicInt->d_value);
    int expected;

    do
    {
        expected = actual;
        actual = testAndSwapInt(atomicInt, expected, expected + value);
    } while (actual != expected);

    return expected + value;
}

inline
int AtomicOperations_IA64_HP_ACC::
    addIntNvAcqRel(AtomicTypes::Int *atomicInt, int value)
{
    int actual = const_cast<int &>(atomicInt->d_value);
    int expected;

    do
    {
        expected = actual;
        actual = testAndSwapIntAcqRel(atomicInt,
                                      expected,
                                      expected + value);
    } while (actual != expected);

    return expected + value;
}

inline
Types::Int64 AtomicOperations_IA64_HP_ACC::
    swapInt64(AtomicTypes::Int64 *atomicInt,
              Types::Int64 swapValue)
{
    _Asm_mf();  // need mf because xchg has acquire semantics only
    return _Asm_xchg(_SZ_D,
                     atomicInt,
                     swapValue,
                     _LDHINT_NONE,
                     d_fullfence);
}

inline
Types::Int64 AtomicOperations_IA64_HP_ACC::
    testAndSwapInt64(AtomicTypes::Int64 *atomicInt,
                     Types::Int64 compareValue,
                     Types::Int64 swapValue)
{
    _Asm_mov_to_ar(_AREG_CCV, compareValue);
    _Asm_mf();
    return _Asm_cmpxchg(_SZ_D,
                        _SEM_REL,
                        atomicInt,
                        swapValue,
                        _LDHINT_NONE,
                        d_fullfence);
}

inline
Types::Int64 AtomicOperations_IA64_HP_ACC::
    testAndSwapInt64AcqRel(AtomicTypes::Int64 *atomicInt,
                           Types::Int64 compareValue,
                           Types::Int64 swapValue)
{
    _Asm_mov_to_ar(_AREG_CCV, compareValue);
    return _Asm_cmpxchg(_SZ_D,
                        _SEM_REL,
                        atomicInt,
                        swapValue,
                        _LDHINT_NONE,
                        d_upfence);
}

inline
Types::Int64 AtomicOperations_IA64_HP_ACC::
    addInt64Nv(AtomicTypes::Int64 *atomicInt,
               Types::Int64 value)
{
    Types::Int64 actual = const_cast<Types::Int64 &>(atomicInt->d_value);
    Types::Int64 expected;

    do
    {
        expected = actual;
        actual = testAndSwapInt64(atomicInt, expected, expected + value);
    } while (actual != expected);

    return expected + value;
}

inline
Types::Int64 AtomicOperations_IA64_HP_ACC::
    addInt64NvAcqRel(AtomicTypes::Int64 *atomicInt,
                     Types::Int64 value)
{
    Types::Int64 actual = const_cast<Types::Int64 &>(atomicInt->d_value);
    Types::Int64 expected;

    do
    {
        expected = actual;
        actual = testAndSwapInt64AcqRel(atomicInt,
                                        expected,
                                        expected + value);
    } while (actual != expected);

    return expected + value;
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
