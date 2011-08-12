// bsls_atomicoperations_ia64_hp_acc.h                                -*-C++-*-
#ifndef INCLUDED_BSLS_ATOMICOPERATIONS_IA64_HP_ACC
#define INCLUDED_BSLS_ATOMICOPERATIONS_IA64_HP_ACC

//@PURPOSE: Provide implentations of atomic operations for HPUX/IA64.
//
//@CLASSES:
//  bsls_AtomicOperations_IA64_HP_ACC: implementation of atomics for HPUX/IA64.
//
//@AUTHOR: Alexei Zakharov (azakhar1)
//
//@DESCRIPTION: This component provides classes necessary to implement atomics
// on the HPUX/IA64 platform in 32 and 64bit modes.  The classes are for
// private use only.  See 'bsls_atomicoperations' and 'bsls_atomic' for the
// public inteface to atomics.

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

#ifndef INCLUDED_BSLS_TYPES
#include <bsls_types.h>
#endif

#ifndef INCLUDED_BSLS_ATOMICOPERATIONS_DEFAULT
#include <bsls_atomicoperations_default.h>
#endif

#if defined(BSLS_PLATFORM__CPU_IA64) && defined(BSLS_PLATFORM__OS_HPUX)

#include <machine/sys/inline.h>

// For reference implementation of atomics on HP-UX/IA64 see:
// http://h21007.www2.hp.com/portal/download/files/unprot/itanium/spinlocks.pdf
//
// Notes:
// * const_cast is used where necessary to drop the volatile qualifier to avoid
//   generating unnecessary ld.acq.
// * casts to (unsigned) in assembler intrinsics is used to avoid signed
//   int->int64 promotion.

namespace BloombergLP {

struct bsls_AtomicOperations_IA64_HP_ACC;
typedef bsls_AtomicOperations_IA64_HP_ACC  bsls_AtomicOperations_Imp;

      // ================================================================
      // struct bsls_Atomic_TypeTraits<bsls_AtomicOperations_IA64_HP_ACC>
      // ================================================================

template<>
struct bsls_Atomic_TypeTraits<bsls_AtomicOperations_IA64_HP_ACC>
{
    struct Int
    {
        volatile int d_value __attribute__((__aligned__(sizeof(int))));
    };

    struct Int64
    {
        volatile bsls_Types::Int64 d_value
                       __attribute__((__aligned__(sizeof(bsls_Types::Int64))));
    };

    struct Pointer
    {
        void const * volatile d_value
                                  __attribute__((__aligned__(sizeof(void *))));
    };
};

#if defined(BSLS_PLATFORM__CPU_64_BIT)

template <typename IMP>
struct bsls_AtomicOperations_IA64_HP_ACC_Default
    : bsls_AtomicOperations_Default64<IMP>
{};

#else

template <typename IMP>
struct bsls_AtomicOperations_IA64_HP_ACC_Default
    : bsls_AtomicOperations_Default32<IMP>
{};

#endif

                  // ========================================
                  // struct bsls_AtomicOperations_IA64_HP_ACC
                  // ========================================

struct bsls_AtomicOperations_IA64_HP_ACC
    : bsls_AtomicOperations_IA64_HP_ACC_Default<
                                             bsls_AtomicOperations_IA64_HP_ACC>
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
    typedef bsls_Atomic_TypeTraits<bsls_AtomicOperations_IA64_HP_ACC> Types;

        // *** atomic functions for int ***

    static int getInt(const Types::Int *atomicInt);

    static void setInt(Types::Int *atomicInt, int value);

    static void setIntRelease(Types::Int *atomicInt, int value);

    static int swapInt(Types::Int *atomicInt, int swapValue);

    static int testAndSwapInt(Types::Int *atomicInt,
                              int         compareValue,
                              int         swapValue);

    static int testAndSwapIntAcqRel(Types::Int *atomicInt,
                                    int         compareValue,
                                    int         swapValue);

    static int addIntNv(Types::Int *atomicInt, int value);

    static int addIntNvAcqRel(Types::Int *atomicInt, int value);

        // *** atomic functions for Int64 ***

    static bsls_Types::Int64 getInt64(const Types::Int64 *atomicInt);

    static void setInt64(Types::Int64 *atomicInt, bsls_Types::Int64 value);

    static void setInt64Release(Types::Int64      *atomicInt,
                                bsls_Types::Int64  value);

    static bsls_Types::Int64 swapInt64(Types::Int64       *atomicInt,
                                       bsls_Types::Int64   swapValue);

    static bsls_Types::Int64 testAndSwapInt64(Types::Int64      *atomicInt,
                                              bsls_Types::Int64  compareValue,
                                              bsls_Types::Int64  swapValue);

    static bsls_Types::Int64 testAndSwapInt64AcqRel(
                                              Types::Int64      *atomicInt,
                                              bsls_Types::Int64  compareValue,
                                              bsls_Types::Int64  swapValue);

    static bsls_Types::Int64 addInt64Nv(Types::Int64      *atomicInt,
                                        bsls_Types::Int64  value);

    static bsls_Types::Int64 addInt64NvAcqRel(Types::Int64      *atomicInt,
                                              bsls_Types::Int64  value);
};

// ===========================================================================
//                        INLINE FUNCTION DEFINITIONS
// ===========================================================================

                  // ----------------------------------------
                  // struct bsls_AtomicOperations_IA64_HP_ACC
                  // ----------------------------------------

inline
int bsls_AtomicOperations_IA64_HP_ACC::
    getInt(const Types::Int *atomicInt)
{
    return int(_Asm_ld_volatile(_SZ_W, _LDHINT_NONE, &atomicInt->d_value));
}

inline
void bsls_AtomicOperations_IA64_HP_ACC::
    setInt(Types::Int *atomicInt, int value)
{
    _Asm_st_volatile(_SZ_W,
                     _STHINT_NONE,
                     &atomicInt->d_value,
                     (unsigned) value);
    _Asm_mf();
}

inline
void bsls_AtomicOperations_IA64_HP_ACC::
    setIntRelease(Types::Int *atomicInt, int value)
{
    _Asm_st_volatile(_SZ_W,
                     _STHINT_NONE,
                     &atomicInt->d_value,
                     (unsigned) value);
}

inline
int bsls_AtomicOperations_IA64_HP_ACC::
    swapInt(Types::Int *atomicInt, int swapValue)
{
    return int(_Asm_xchg(_SZ_W,
                         atomicInt,
                         (unsigned) swapValue,
                         _LDHINT_NONE,
                         d_fullfence));
}

inline
int bsls_AtomicOperations_IA64_HP_ACC::
    testAndSwapInt(Types::Int *atomicInt, int compareValue, int swapValue)
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
int bsls_AtomicOperations_IA64_HP_ACC::
    testAndSwapIntAcqRel(Types::Int *atomicInt,
                         int         compareValue,
                         int         swapValue)
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
int bsls_AtomicOperations_IA64_HP_ACC::
    addIntNv(Types::Int *atomicInt, int value)
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
int bsls_AtomicOperations_IA64_HP_ACC::
    addIntNvAcqRel(Types::Int *atomicInt, int value)
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
bsls_Types::Int64 bsls_AtomicOperations_IA64_HP_ACC::
    getInt64(const Types::Int64 *atomicInt)
{
    return _Asm_ld_volatile(_SZ_D, _LDHINT_NONE, &atomicInt->d_value);
}

inline
void bsls_AtomicOperations_IA64_HP_ACC::
    setInt64(Types::Int64 *atomicInt, bsls_Types::Int64 value)
{
    _Asm_st_volatile(_SZ_D,
                     _STHINT_NONE,
                     &atomicInt->d_value,
                     value);
    _Asm_mf();
}

inline
void bsls_AtomicOperations_IA64_HP_ACC::
    setInt64Release(Types::Int64      *atomicInt,
                    bsls_Types::Int64  value)
{
    _Asm_st_volatile(_SZ_D,
                     _STHINT_NONE,
                     &atomicInt->d_value,
                     value);
}

inline
bsls_Types::Int64 bsls_AtomicOperations_IA64_HP_ACC::
    swapInt64(Types::Int64       *atomicInt,
              bsls_Types::Int64   swapValue)
{
    return _Asm_xchg(_SZ_D,
                     atomicInt,
                     swapValue,
                     _LDHINT_NONE,
                     d_fullfence);
}

inline
bsls_Types::Int64 bsls_AtomicOperations_IA64_HP_ACC::
    testAndSwapInt64(Types::Int64      *atomicInt,
                     bsls_Types::Int64  compareValue,
                     bsls_Types::Int64  swapValue)
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
bsls_Types::Int64 bsls_AtomicOperations_IA64_HP_ACC::
    testAndSwapInt64AcqRel(Types::Int64      *atomicInt,
                           bsls_Types::Int64  compareValue,
                           bsls_Types::Int64  swapValue)
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
bsls_Types::Int64 bsls_AtomicOperations_IA64_HP_ACC::
    addInt64Nv(Types::Int64      *atomicInt,
               bsls_Types::Int64  value)
{
    bsls_Types::Int64 actual =
                           const_cast<bsls_Types::Int64 &>(atomicInt->d_value);
    bsls_Types::Int64 expected;

    do
    {
        expected = actual;
        actual = testAndSwapInt64(atomicInt, expected, expected + value);
    } while (actual != expected);

    return expected + value;
}

inline
bsls_Types::Int64 bsls_AtomicOperations_IA64_HP_ACC::
    addInt64NvAcqRel(Types::Int64      *atomicInt,
                     bsls_Types::Int64  value)
{
    bsls_Types::Int64 actual =
                           const_cast<bsls_Types::Int64 &>(atomicInt->d_value);
    bsls_Types::Int64 expected;

    do
    {
        expected = actual;
        actual = testAndSwapInt64AcqRel(atomicInt,
                                        expected,
                                        expected + value);
    } while (actual != expected);

    return expected + value;
}

}  // close enterprise namespace

#endif

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2011
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
