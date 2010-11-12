// bces_atomicutilimpl_sparcv9.cpp -*-C++-*-
#include <bces_atomicutilimpl_sparcv9.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bces_atomicutilimpl_sparcv9_cpp,"$Id$ $CSID$")

#ifdef BSLS_PLATFORM__CPU_SPARC_V9

namespace BloombergLP {

#if !defined(__GNUC__) || (__GNUC__ < 4)
    // gcc-4.x fails to compile this function if it is static
    static
#endif
void bces_AtomicUtilImpl_SparcV9Common()
{
    // void bces_AtomicUtilImpl_SparcV9SetInt( volatile int*, int );
    asm(".global bces_AtomicUtilImpl_SparcV9SetInt\n"
        ".type bces_AtomicUtilImpl_SparcV9SetInt,#function\n"
        "bces_AtomicUtilImpl_SparcV9SetInt:\n"
        "st %o1, [%o0]\n"
        "retl\n"
        "membar #StoreLoad\n");

    // long bces_AtomicUtilImpl_SparcV9SwapInt( volatile int*, int );
    asm(".global bces_AtomicUtilImpl_SparcV9SwapInt\n"
        ".type bces_AtomicUtilImpl_SparcV9SwapInt,#function\n"
        "bces_AtomicUtilImpl_SparcV9SwapInt:\n"
        "ld [%o0], %o2\n"
        "1:\n"
        "mov %o1, %o3\n"
        "cas [%o0], %o2, %o3\n"
        "cmp %o2, %o3\n"
        "bne,a,pn %icc,1b\n"
        "mov %o3,%o2\n"
        "mov %o3, %o0\n"
        "retl\n"
        "membar #StoreLoad");

    // void bces_AtomicUtilImpl_SparcV9GetInt(volatile int*);
    asm(".global bces_AtomicUtilImpl_SparcV9GetInt\n"
        ".type bces_AtomicUtilImpl_SparcV9GetInt,#function\n"
        "bces_AtomicUtilImpl_SparcV9GetInt:\n"
        "membar #StoreLoad\n"
        "retl\n"
        "ld [%o0], %o0");

    // int bces_AtomicUtilImpl_SparcV9AddInt(volatile int*, int);
    asm(".global bces_AtomicUtilImpl_SparcV9AddInt");
    asm(".type bces_AtomicUtilImpl_SparcV9AddInt,#function\n"
        "bces_AtomicUtilImpl_SparcV9AddInt:\n"
        "ld [%o0], %o2\n"
        "1:\n"
        "add %o2, %o1, %o3\n"
        "cas [%o0], %o2, %o3\n"
        "cmp %o2, %o3\n"
        "bne,a,pn %icc, 1b\n"
        "mov %o3, %o2\n"
        "add %o2, %o1, %o0\n"
        "retl\n"
        "membar #StoreLoad");


    // int bces_AtomicUtilImpl_SparcV9AddIntRelaxed(volatile int*, int);
    asm(".global bces_AtomicUtilImpl_SparcV9AddIntRelaxed\n"
        ".type bces_AtomicUtilImpl_SparcV9AddIntRelaxed,#function\n"
        "bces_AtomicUtilImpl_SparcV9AddIntRelaxed:\n"
        "ld [%o0], %o2\n"
        "1:\n"
        "add %o2, %o1, %o3\n"
        "cas [%o0], %o2, %o3\n"
        "cmp %o2, %o3\n"
        "bne,a,pn %icc, 1b\n"
        "mov %o3, %o2\n"
        "retl\n"
        "add %o2, %o1, %o0");

    // int bces_AtomicUtilImpl_SparcV9TestAndSwap(volatile int*, int, int);
    asm(".global bces_AtomicUtilImpl_SparcV9TestAndSwap\n"
        ".type bces_AtomicUtilImpl_SparcV9TestAndSwap,#function\n"
        "bces_AtomicUtilImpl_SparcV9TestAndSwap:\n"
        "cas [%o0], %o1, %o2\n"
        "mov %o2, %o0\n"
        "retl\n"
        "membar #StoreLoad");

    // void bces_AtomicUtilImpl_SparcV9SpinLock( volatile char * )
    asm(".global bces_AtomicUtilImpl_SparcV9SpinLock\n"
        ".type bces_AtomicUtilImpl_SparcV9SpinLock,#function\n"
        "bces_AtomicUtilImpl_SparcV9SpinLock:\n"
        "mov %o0, %o1\n"
        "0:\n"
        "ldstub [%o1], %o0 \n"
        "brz,a,pn %o0,2f\n"
        "nop\n"
        "1:\n"
        "ldub [%o1], %o0\n"
        "brnz,a,pn %o0, 1b\n"
        "nop\n"
        "ba,a 0b\n"
        "2:\n"
        "retl\n"
        "membar #StoreLoad");

    // int bces_AtomicUtilImpl_SparcV9SpinTryLock( volatile char *,
    //                                                   int retries )
    asm(".global bces_AtomicUtilImpl_SparcV9SpinTryLock\n"
        ".type bces_AtomicUtilImpl_SparcV9SpinTryLock,#function\n"
        "bces_AtomicUtilImpl_SparcV9SpinTryLock:\n"
        "mov %o0, %o2\n"
        "0:\n"
        "ldstub [%o2], %o0 \n"
        "brz,a,pn %o0,2f\n"
        "nop\n"
        "1:\n"
        "subcc %o1,1,%o1\n"
        "bneg,a 2f\n"
        "nop\n"
        "ldub [%o2], %o0\n"
        "brnz,a %o0,1b\n"
        "nop\n"
        "ba,a 0b\n"
        "2:\n"
        "retl\n"
        "membar #StoreLoad");

    // void bces_AtomicUtilImpl_SparcV9SpinUnlock(volatile char *)
    asm(".global bces_AtomicUtilImpl_SparcV9SpinUnlock\n"
        ".type bces_AtomicUtilImpl_SparcV9SpinUnlock,#function\n"
        "bces_AtomicUtilImpl_SparcV9SpinUnlock:\n"
        "retl\n"
        "stub %g0, [%o0]\n");

}

#ifdef BSLS_PLATFORM__CPU_64_BIT

static void bces_AtomicUtilImpl_SparcV964BitMode()
{
    // int bces_AtomicUtilImpl_SparcV9SetInt64(volatile long long*, long long);
    asm(".global bces_AtomicUtilImpl_SparcV9SetInt64\n"
        ".type bces_AtomicUtilImpl_SparcV9SetInt64,#function\n"
        "bces_AtomicUtilImpl_SparcV9SetInt64:\n"
        "stx %o1, [%o0]\n"
        "retl\n"
        "membar #StoreLoad");

    // long bces_AtomicUtilImpl_SparcV9SwapInt64( volatile int*, int );
    asm(".global bces_AtomicUtilImpl_SparcV9SwapInt64\n"
        ".type bces_AtomicUtilImpl_SparcV9SwapInt64,#function\n"
        "bces_AtomicUtilImpl_SparcV9SwapInt64:\n"
        "ldx [%o0], %o2\n"
        "1:\n"
        "mov %o1, %o3\n"
        "casx [%o0], %o2, %o3\n"
        "cmp %o2, %o3\n"
        "bne,a,pn %xcc,1b\n"
        "mov %o3,%o2\n"
        "mov %o3, %o0\n"
        "retl\n"
        "membar #StoreLoad");

    // long long bces_AtomicUtilImpl_SparcV9GetInt64(volatile long long*);
    asm(".global bces_AtomicUtilImpl_SparcV9GetInt64\n"
        ".type bces_AtomicUtilImpl_SparcV9GetInt64,#function\n"
        "bces_AtomicUtilImpl_SparcV9GetInt64:\n"
        "membar #StoreLoad\n"
        "retl\n"
        "ldx [%o0], %o0");

    // int bces_AtomicUtilImpl_SparcV9AddInt64(volatile long long*, long long);
    asm(".global bces_AtomicUtilImpl_SparcV9AddInt64\n"
        ".type bces_AtomicUtilImpl_SparcV9AddInt64,#function\n"
        "bces_AtomicUtilImpl_SparcV9AddInt64:\n"
        "ldx [%o0], %o2\n"
        "1:\n"
        "add %o2, %o1, %o3\n"
        "casx [%o0], %o2, %o3\n"
        "cmp %o2, %o3\n"
        "bne,a,pn %xcc, 1b\n"
        "mov %o3, %o2\n"
        "add %o2, %o1, %o0\n"
        "retl\n"
        "membar #StoreLoad");

    // int bces_AtomicUtilImpl_SparcV9AddInt64Relaxed(volatile long long*,
    //                                                long long);
    asm(".global bces_AtomicUtilImpl_SparcV9AddInt64Relaxed\n"
        ".type bces_AtomicUtilImpl_SparcV9AddInt64Relaxed,#function\n"
        "bces_AtomicUtilImpl_SparcV9AddInt64Relaxed:\n"
        "ldx [%o0], %o2\n"
        "1:\n"
        "add %o2, %o1, %o3\n"
        "casx [%o0], %o2, %o3\n"
        "cmp %o2, %o3\n"
        "bne,a,pn %xcc, 1b\n"
        "mov %o3, %o2\n"
        "retl\n"
        "add %o2, %o1, %o0");

    // int bces_AtomicUtilImpl_SparcV9TestAndSwap64( volatile long long* ,
    //                                               long long, long long );
    asm(".global bces_AtomicUtilImpl_SparcV9TestAndSwap64\n"
        ".type bces_AtomicUtilImpl_SparcV9TestAndSwap64,#function\n"
        "bces_AtomicUtilImpl_SparcV9TestAndSwap64:\n"
        "casx [%o0], %o1, %o2\n"
        "mov %o2, %o0\n"
        "retl\n"
        "membar #StoreLoad");
}

#else

#error "32-bit mode not supported for this CPU" // not used

#endif

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2007
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
