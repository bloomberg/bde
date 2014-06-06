// bsls_byteorderutil_impl.cpp                                        -*-C++-*-
#include <bsls_byteorderutil_impl.h>

#include <bsls_bsltestutil.h>    // TESTING ONLY

#include <bsls_ident.h>
BSLS_IDENT("$Id$ $CSID$")

// ============================================================================
// IMPLEMENTATION NOTES
//
// Note that we declare all the possible 'custom_*' functions in the namespace
// 'struct', but only at most half of them (and possibly none of them) will
// actually be defined.  All the ones that will be defined will be defined
// inline in the .h file.  Never will a 'p' version (a function that takes 'x'
// as a ptr) and a 'non-p' (a function that takes 'x' by value') be defined for
// the same word width at the same time.  It was felt that to add the myriad
// and byzantine #ifdef's needed to only declare the functions that will later
// be defined in the .h file would unacceptably obscure the readability of the
// declarations.
// ============================================================================

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
