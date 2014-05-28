// bsls_byteorderutil_impl.cpp                                        -*-C++-*-
#include <bsls_byteorderutil_impl.h>

#include <bsls_ident.h>
BSLS_IDENT("$Id$ $CSID$")

namespace BloombergLP {

#if defined(BSLS_PLATFORM_CPU_SPARC) && !defined(BSLS_PLATFORM_CMP_GNU)
void bsls_byteOrderUtil_Impl_common()
    // This function is never called, it is just a common area in which the
    // following assembler is to reside.
{
// Commented out since the generic implementation is probably faster than the
// function call for 16 and 32 bits.
//..
//  asm(".global bsls_byteOrderUtil_Impl_sparc_CC_swap_p16\n"
//      ".type   bsls_byteOrderUtil_Impl_sparc_CC_swap_p16,#function\n"
//      "bsls_byteOrderUtil_Impl_sparc_CC_swap_p16:\n"
//      "retl\n"
//      "lduha [%o0] 0x88, %o0\n");
//
//  asm(".global bsls_byteOrderUtil_Impl_sparc_CC_swap_p32\n"
//      ".type   bsls_byteOrderUtil_Impl_sparc_CC_swap_p32,#function\n"
//      "bsls_byteOrderUtil_Impl_sparc_CC_swap_p32:\n"
//      "retl\n"
//      "lduwa [%o0] 0x88, %o0\n");
//..

#ifdef BSLS_PLATFORM_CPU_32_BIT
    asm(".global bsls_byteOrderUtil_Impl_sparc_CC_swap_p64\n"
        ".type   bsls_byteOrderUtil_Impl_sparc_CC_swap_p64,#function\n"
        "bsls_byteOrderUtil_Impl_sparc_CC_swap_p64:\n"
        "ldxa [%o0] 0x88, %o2\n" // load into %o2
        "srl %o2, 0, %o1\n"      // %o1 = %o2 & UINT_MAX
        "retl\n"
        "srlx %o2, 32, %o0\n");  // %o0 = %o2 >> 32;

#else  // BSLS_PLATFORM_CPU_32_BIT else

    asm(".global bsls_byteOrderUtil_Impl_sparc_CC_swap_p64\n"
        ".type   bsls_byteOrderUtil_Impl_sparc_CC_swap_p64,#function\n"
        "bsls_byteOrderUtil_Impl_sparc_CC_swap_p64:\n"
        "retl\n"
        "ldxa [%o0] 0x88, %o0\n");

#endif  // BSLS_PLATFORM_CPU_32_BIT else
}

#endif  // BSLS_PLATFORM_CPU_SPARC

}  // close enterprise namespace

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2014
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ----------------------------------
