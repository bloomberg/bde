// bces_atomicutilimpl_sparc32.cpp -*-C++-*-
#include <bces_atomicutilimpl_sparc32.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bces_atomicutilimpl_sparc32_cpp,"$Id$ $CSID$")

#ifdef BSLS_PLATFORM__CPU_SPARC_32

namespace BloombergLP {

#if !defined(__GNUC__) || (__GNUC__ < 4)
    // gcc-4.x fails to compile this function if it is static
    static
#endif
void bces_AtomicUtilImpl_Sparc32Common()
{
#if !defined(BSLS_PLATFORM__CMP_GNU)
    // void bces_AtomicUtilImpl_Sparc32SetInt(volatile int*, int);
    asm(".global bces_AtomicUtilImpl_Sparc32SetInt\n"
        ".type bces_AtomicUtilImpl_Sparc32SetInt,#function\n"
        "bces_AtomicUtilImpl_Sparc32SetInt:\n"
        "st %o1, [%o0]\n"
        "retl\n"
        "membar #StoreLoad");

    // long bces_AtomicUtilImpl_Sparc32SwapInt(volatile int*, int);
    asm(".global bces_AtomicUtilImpl_Sparc32SwapInt\n"
        ".type bces_AtomicUtilImpl_Sparc32SwapInt,#function\n"
        "bces_AtomicUtilImpl_Sparc32SwapInt:\n"
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

    // int bces_AtomicUtilImpl_Sparc32GetInt(volatile int*);
    asm(".global bces_AtomicUtilImpl_Sparc32GetInt\n"
        ".type bces_AtomicUtilImpl_Sparc32GetInt,#function\n"
        "bces_AtomicUtilImpl_Sparc32GetInt:\n"
        "membar #StoreLoad\n"
        "retl\n"
        "ld [%o0], %o0");

    // int bces_AtomicUtilImpl_Sparc32AddInt(volatile int*, int);
    asm(".global bces_AtomicUtilImpl_Sparc32AddInt\n"
        ".type bces_AtomicUtilImpl_Sparc32AddInt,#function\n"
        "bces_AtomicUtilImpl_Sparc32AddInt:\n"
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

    // int bces_AtomicUtilImpl_Sparc32AddIntRelaxed(volatile int*, int);
    asm(".global bces_AtomicUtilImpl_Sparc32AddIntRelaxed\n"
        ".type bces_AtomicUtilImpl_Sparc32AddIntRelaxed,#function\n"
        "bces_AtomicUtilImpl_Sparc32AddIntRelaxed:\n"
        "ld [%o0], %o2\n"
        "1:\n"
        "add %o2, %o1, %o3\n"
        "cas [%o0], %o2, %o3\n"
        "cmp %o2, %o3\n"
        "bne,a,pn %icc, 1b\n"
        "mov %o3, %o2\n"
        "retl\n"
        "add %o2, %o1, %o0");

    // int bces_AtomicUtilImpl_Sparc32TestAndSwap(volatile int*, int, int);
    asm(".global bces_AtomicUtilImpl_Sparc32TestAndSwap\n"
        ".type bces_AtomicUtilImpl_Sparc32TestAndSwap,#function\n"
        "bces_AtomicUtilImpl_Sparc32TestAndSwap:\n"
        "cas [%o0], %o1, %o2\n"
        "mov %o2, %o0\n"
        "retl\n"
        "membar #StoreLoad");
#endif

    // void bces_AtomicUtilImpl_Sparc32SpinLock(volatile char *)
    asm(".global bces_AtomicUtilImpl_Sparc32SpinLock\n"
        ".type bces_AtomicUtilImpl_Sparc32SpinLock,#function\n"
        "bces_AtomicUtilImpl_Sparc32SpinLock:\n"
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

    // int bces_AtomicUtilImpl_Sparc32SpinTryLock(volatile char *,
    //                                            int retries )
    asm(".global bces_AtomicUtilImpl_Sparc32SpinTryLock\n"
        ".type bces_AtomicUtilImpl_Sparc32SpinTryLock,#function\n"
        "bces_AtomicUtilImpl_Sparc32SpinTryLock:\n"
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

    // void bces_AtomicUtilImpl_Sparc32SpinUnlock(volatile char *)
    asm(".global bces_AtomicUtilImpl_Sparc32SpinUnlock\n"
        ".type bces_AtomicUtilImpl_Sparc32SpinUnlock,#function\n"
        "bces_AtomicUtilImpl_Sparc32SpinUnlock:\n"
        "retl\n"
        "stub %g0, [%o0]");

    // These 64-bit ops rely on the V8+ ABI.  Our sparc processors support it
    // (plink is using this by default).  We're getting our arguments just like
    // we would do for sparcv8 (i.e 32-bit), but we're allowed to use 64-bit
    // ops and 64-bit registers in the functions.  Therefore, we'll need to
    // convert any 64-bit argument, that is something that comes in a 32-bit
    // register pairs, in a 64-bit value contained in one register.  If
    // we're returning a 64-bit value, we'll have to split it between
    // %o0 (for the high 32 bits) and %o1 (low 32-bit).

    // void bces_AtomicUtilImpl_Sparc32SetInt64(volatile long long *atomic,
    //                                          long long           value);
    // atomic is in %o0, high 32bits of value are in %o1, low 32 bits in %o2
    asm(".global bces_AtomicUtilImpl_Sparc32SetInt64\n"
        ".type bces_AtomicUtilImpl_Sparc32SetInt64,#function\n"
        "bces_AtomicUtilImpl_Sparc32SetInt64:\n"
        "sllx %o1, 32, %o1\n"  // %o1 has the high 32-bit of value, this
                               // instruction moves these 32 bits to the
                               // high bits of %o1.
        "srl  %o2, 0, %o2\n"   // This clears the high bits of %o2
                               // The low-32 bits contain the low 32 bits
                               // of value
        "add %o1, %o2, %o1\n"  // merge %o1 and %o2, we now have value
                               // as a 64-bit value in %o1
        "stx %o1, [%o0]\n"
        "retl\n"
        "membar #StoreLoad");

    // void bces_AtomicUtilImpl_Sparc32SetInt64Relaxed(
    //                                          volatile long long *atomic,
    //                                          long long           value);
    // atomic is in %o0, high 32bits of value are in %o1, low 32 bits in %o2
    asm(".global bces_AtomicUtilImpl_Sparc32SetInt64Relaxed\n"
        ".type bces_AtomicUtilImpl_Sparc32SetInt64Relaxed,#function\n"
        "bces_AtomicUtilImpl_Sparc32SetInt64Relaxed:\n"
        "sllx %o1, 32, %o1\n"  // %o1 has the high 32-bit of value, this
                               // instruction moves these 32 bits to the
                               // high bits of %o1.
        "srl  %o2, 0, %o2\n"   // This clears the high bits of %o2
                               // The low-32 bits contain the low 32 bits
                               // of value
        "add %o1, %o2, %o1\n"  // merge %o1 and %o2, we now have value
                               // as a 64-bit value in %o1
        "retl\n"
        "stx %o1, [%o0]");

    // long bces_AtomicUtilImpl_Sparc32SwapInt64(volatile long long*,
    //                                           long long value);
    asm(".global bces_AtomicUtilImpl_Sparc32SwapInt64\n"
        ".type bces_AtomicUtilImpl_Sparc32SwapInt64,#function\n"
        "bces_AtomicUtilImpl_Sparc32SwapInt64:\n"
        "sllx %o1, 32, %o1\n"
        "srl  %o2, 0, %o2\n"
        "add %o1, %o2, %o1\n" // %o1 has value as a 64-bit number
                              // see SetInt64 for the details
        "ldx [%o0], %o2\n"
        "1:\n"
        "mov %o1, %o3\n"
        "casx [%o0], %o2, %o3\n"
        "cmp %o2, %o3\n"
        "bne,a,pn %xcc,1b\n"
        "mov %o3,%o2\n"
        "srl %o3, 0, %o1\n"     // We need to do the opposite operation as
                                // above.  %o0 must get the high 32-bit of
                                // %03 as a 32-bit reg, and %o1 the low
                                // 32-bit of %o3.
                                // This instruction simply copies the low
                                // 32-bit of %o3 and clears the high 32-bit
                                // ones

        "srlx %o3, 32, %o0\n"  // This shifts %o3 so %o0 gets the high
                                // 32-bit of %o3 in its low 32-bit.
        "retl\n"
        "membar #StoreLoad");

    // int bces_AtomicUtilImpl_Sparc32GetInt64Relaxed(volatile long long*);
    asm(".global bces_AtomicUtilImpl_Sparc32GetInt64Relaxed\n"
        ".type bces_AtomicUtilImpl_Sparc32GetInt64Relaxed,#function\n"
        "bces_AtomicUtilImpl_Sparc32GetInt64Relaxed:\n"
        "ldx [%o0], %o2\n"
        "srl %o2, 0, %o1\n"     // %o0 must get the high 32-bit of
                                // %02 as a 32-bit reg, and %o1 the low
                                // 32-bit of %o2.
                                // This instruction simply copies the low
                                // 32-bit of %o2 and clears the high 32-bit
                                // ones
        "retl\n"
        "srlx %o2, 32, %o0");  // This shifts %o2 so %o0 gets the high
                                // 32-bit of %o2 in its low 32-bit.

    // int bces_AtomicUtilImpl_Sparc32GetInt64(volatile long long*);
    asm(".global bces_AtomicUtilImpl_Sparc32GetInt64\n"
        ".type bces_AtomicUtilImpl_Sparc32GetInt64,#function\n"
        "bces_AtomicUtilImpl_Sparc32GetInt64:\n"
        "membar #StoreLoad\n"
        "ldx [%o0], %o2\n"
        "srl %o2, 0, %o1\n"     // %o0 must get the high 32-bit of
                                // %02 as a 32-bit reg, and %o1 the low
                                // 32-bit of %o2.
                                // This instruction simply copies the low
                                // 32-bit of %o2 and clears the high 32-bit
                                // ones
        "retl\n"
        "srlx %o2, 32, %o0");  // This shifts %o2 so %o0 gets the high
                                // 32-bit of %o2 in its low 32-bit.

    // int bces_AtomicUtilImpl_Sparc32AddInt64(volatile long long*, long long);
    asm(".global bces_AtomicUtilImpl_Sparc32AddInt64\n"
        ".type bces_AtomicUtilImpl_Sparc32AddInt64,#function\n"
        "bces_AtomicUtilImpl_Sparc32AddInt64:\n"
        "sllx %o1, 32, %o1\n"  // convert the long long, see explanation
        "srl %o2, 0, %o2\n"    // above
        "add %o1, %o2, %o1\n"  // the long long is now in %o1
        "ldx [%o0], %o2\n"
        "1:\n"
        "add %o2, %o1, %o3\n"
        "casx [%o0], %o2, %o3\n"
        "cmp %o2, %o3\n"
        "bne,a,pn %xcc, 1b\n"
        "mov %o3, %o2\n"
        "add %o2, %o1, %o0\n"
        "srl %o0, 0, %o1\n"    // convert the 64-bit value in %o0 into
        "srlx %o0, 32, %o0\n"  // a 32-bit %o0/%01 pair
        "retl\n"
        "membar #StoreLoad");

    // int bces_AtomicUtilImpl_Sparc32AddInt64Relaxed(volatile long long*,
    //                                                long long);
    asm(".global bces_AtomicUtilImpl_Sparc32AddInt64Relaxed\n"
        ".type bces_AtomicUtilImpl_Sparc32AddInt64Relaxed,#function\n"
        "bces_AtomicUtilImpl_Sparc32AddInt64Relaxed:\n"
        "sllx %o1, 32, %o1\n"  // convert the long long, see explanation
        "srl %o2, 0, %o2\n"    // above
        "add %o1, %o2, %o1\n"  // the long long is now in %o1
        "ldx [%o0], %o2\n"
        "1:\n"
        "add %o2, %o1, %o3\n"
        "casx [%o0], %o2, %o3\n"
        "cmp %o2, %o3\n"
        "bne,a,pn %xcc, 1b\n"
        "mov %o3, %o2\n"
        "add %o2, %o1, %o0\n"
        "srl %o0, 0, %o1\n"    // convert the 64-bit value in %o0 into
        "retl\n"               // ...
        "srlx %o0, 32, %o0");  // a 32-bit %o0/%01 pair

    // int bces_AtomicUtilImpl_Sparc32TestAndSwap64(volatile long long*,
    //                                              long long,
    //                                              long long);
    asm(".global bces_AtomicUtilImpl_Sparc32TestAndSwap64\n"
        ".type bces_AtomicUtilImpl_Sparc32TestAndSwap64,#function\n"
        "bces_AtomicUtilImpl_Sparc32TestAndSwap64:\n"
        "sllx %o1, 32, %o1\n"          // convert 1st long long
        "srl %o2, 0, %o2\n"
        "add %o1, %o2, %o1\n"          // 1st ll arg is in %o1
        "sllx %o3, 32, %o2\n"          // convert 2nd long long
        "srl %o4, 0, %o3\n"
        "add %o3, %o2, %o2\n"          // 2nd ll arg is in %o2
        "casx [%o0], %o1, %o2\n"
        "srlx %o2, 32, %o0\n"          // fix return value
        "srl %o2, 0, %o1\n"
        "retl\n"
        "membar #StoreLoad");
}

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2009
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
