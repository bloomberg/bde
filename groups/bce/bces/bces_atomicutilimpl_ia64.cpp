// bces_atomicutilimpl_ia64.cpp -*-C++-*-
#include <bces_atomicutilimpl_ia64.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bces_atomicutilimpl_ia64_cpp,"$Id$ $CSID$")

namespace BloombergLP {

#ifdef BSLS_PLATFORM__CPU_IA64

#if defined(__GNUC__)

#error "not implemented"

#endif


#if defined(BSLS_PLATFORM__OS_WINDOWS)
    // TBD: Implementation for Windows in Itanium



                 // --------------------------
                 // END WINDOWS IMPLEMENTATION
                 // --------------------------
#endif

#if defined(BSLS_PLATFORM__OS_HPUX)
#include <machine/sys/inline.h>

    // the "UP" fences prevent scheduling upstream instructions of that type
    // after the fence instruction; the "DOWN" fences prevent scheduling
    // downstream instructions before the fence.  We will generally use
    // full fences for memory, call, branch instructions.

#define IA64_UPFENCE \
  ((_Asm_fence)(_UP_MEM_FENCE | _UP_ALU_FENCE | _UP_CALL_FENCE | _UP_BR_FENCE))

#define IA64_DOWNFENCE \
  ((_Asm_fence)(_DOWN_MEM_FENCE | _DOWN_ALU_FENCE | \
                _DOWN_CALL_FENCE | _DOWN_BR_FENCE))

#define IA64_FULLFENCE ((_Asm_fence)(IA64_UPFENCE | IA64_DOWNFENCE))

int  bces_AtomicUtilImpl_IA64SetInt(volatile int  *aInt,
                                    int            val)
{
    bsls_PlatformUtil::Uint64 prev
        = _Asm_xchg(_SZ_W,       // word = 4 bytes
                    aInt,        // destination
                    (bsls_PlatformUtil::Uint64)val, // value
                    _LDHINT_NONE, // no pre-fetch hint
                    IA64_FULLFENCE); // memory barrier
    return (int ) prev;
}

int  bces_AtomicUtilImpl_IA64GetInt(volatile const int  *aInt)
{
    _Asm_sched_fence(IA64_DOWNFENCE);
    int value = *aInt;
    _Asm_sched_fence(IA64_UPFENCE);
    return value;
}

int  bces_AtomicUtilImpl_IA64TestAndSwap(volatile int  *aInt,
                                         int            cmpVal,
                                         int            swapVal)
{
    unsigned val = (unsigned) cmpVal;
    // We must cast it to unsigned and then cast the unsigned to unsigned
    // int64.  Otherwise, this function won't work for negative values.
    _Asm_mov_to_ar(_AREG_CCV, val); // old value
    bsls_PlatformUtil::Uint64 u
        = _Asm_cmpxchg(_SZ_W,              // word
                       _SEM_REL,           // release
                       aInt,               // destination
                       swapVal,            // new val
                       _LDHINT_NONE,       // pre-fetch hint
                       IA64_FULLFENCE);    // memory barrier
    return (int ) u;
}

int  bces_AtomicUtilImpl_IA64AddInt(volatile int  *aInt,
                                    int            val)
{
    int prev = *aInt;
    int prevCopy;
    int newVal;
    do {
        prevCopy = prev;
        newVal = prev + val;
        prev = bces_AtomicUtilImpl_IA64TestAndSwap(aInt, prev, newVal);
    } while (prev != prevCopy);

    return newVal;

}


bsls_PlatformUtil::Int64
bces_AtomicUtilImpl_IA64SetInt64(volatile bsls_PlatformUtil::Int64 *aInt,
                                 bsls_PlatformUtil::Int64           val)
{

    bsls_PlatformUtil::Int64 prev
        = _Asm_xchg(_SZ_D,           // word = 4 bytes
                    aInt,            // destination
                    val,             // new value
                    _LDHINT_NONE,
                    IA64_FULLFENCE);   // pre-fetch hint
    return prev;
}

bsls_PlatformUtil::Int64
bces_AtomicUtilImpl_IA64GetInt64(volatile const bsls_PlatformUtil::Int64 *aInt)
{
    _Asm_sched_fence(IA64_DOWNFENCE);
    bsls_PlatformUtil::Int64 value = *aInt;
    _Asm_sched_fence(IA64_UPFENCE);
    return *aInt;
}

bsls_PlatformUtil::Int64
bces_AtomicUtilImpl_IA64TestAndSwap64(
                                    volatile bsls_PlatformUtil::Int64 *aInt,
                                    bsls_PlatformUtil::Int64           cmpVal,
                                    bsls_PlatformUtil::Int64           swapVal)
{
    _Asm_mov_to_ar(_AREG_CCV, cmpVal);     // old value
    bsls_PlatformUtil::Int64 prev
        = _Asm_cmpxchg(_SZ_D,              // word
                       _SEM_REL,           // release
                       aInt,               // destination
                       swapVal,            // new val
                       _LDHINT_NONE,       // pre-fetch hint
                       IA64_FULLFENCE);    // memory barrier

    return prev;
}

bsls_PlatformUtil::Int64
bces_AtomicUtilImpl_IA64AddInt64(volatile bsls_PlatformUtil::Int64 *aInt,
                                 bsls_PlatformUtil::Int64           val)
{
    bsls_PlatformUtil::Int64 prev = *aInt;
    bsls_PlatformUtil::Int64 prevCopy;
    bsls_PlatformUtil::Int64 newVal;
    do {
        prevCopy = prev;
        newVal = prev + val;
        prev =
            bces_AtomicUtilImpl_IA64TestAndSwap64(
                aInt, prev, newVal);
    } while (prev != prevCopy);
    return newVal;

}


enum { UNLOCKED = 0, LOCKED = 1 };

void  bces_AtomicUtilImpl_IA64SpinLock(volatile int *aSpin)
{
    // Note: 'xchg' always has 'acquire' (DOWNSTREAM) barrier semantics
    do {
        if (*aSpin == UNLOCKED) {
            int prev = _Asm_xchg(_SZ_W, aSpin, (int) LOCKED, _LDHINT_NONE);
            if (prev == UNLOCKED)
            {
                return;
            }
        }
    } while(1);
}


int  bces_AtomicUtilImpl_IA64SpinTryLock(volatile int  *aSpin,
                                         int            retries)
{
    // Note: 'xchg' always has 'acquire' (DOWNSTREAM) barrier semantics
    do {
        if (*aSpin == UNLOCKED) {
            int prev = _Asm_xchg(_SZ_W, aSpin, (int) LOCKED, _LDHINT_NONE);
            if (prev == UNLOCKED)
            {
                return 0;
            }
        }
    } while(retries--);
    return -1;
}

void bces_AtomicUtilImpl_IA64SpinUnlock(volatile int *aSpin)
{
    // NOTE: st_volatile always has 'release' (UPSTREAM) barrier semantics
    // Logic: *aSpin = UNLOCKED
    _Asm_st_volatile(_SZ_W, _STHINT_NONE, aSpin, 0);
}

                 // ---------------
                 // END HP-UX LOGIC
                 // ---------------

#endif

#endif



}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2007
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
