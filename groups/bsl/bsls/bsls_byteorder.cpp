// bsls_byteorder.cpp                                                 -*-C++-*-
#include <bsls_byteorder.h>

#include <bsls_ident.h>
BSLS_IDENT("$Id$ $CSID$")

namespace BloombergLP {

#if defined(BSLS_PLATFORM__CPU_SPARC) && !defined(BSLS_PLATFORM__CMP_GNU)
void bsls_ByteOrder__Util_common()
{
// Commented out since the generic implementation is probably faster than the
// function call for 16 and 32 bits.
//..
//  asm(".global bsls_ByteOrder__Util_sparc_swap_16\n"
//      ".type bsls_ByteOrder__Util_sparc_swap_16,#function\n"
//      "bsls_ByteOrder__Util_sparc_swap_16:\n"
//      "retl\n"
//      "lduha [%o0] 0x88, %o0\n");
//
//  asm(".global bsls_ByteOrder__Util_sparc_swap_32\n"
//      ".type bsls_ByteOrder__Util_sparc_swap_32,#function\n"
//      "bsls_ByteOrder__Util_sparc_swap_32:\n"
//      "retl\n"
//      "lduwa [%o0] 0x88, %o0\n");
//..

#ifdef BSLS_PLATFORM__CPU_32_BIT
    asm(".global bsls_ByteOrder__Util_sparc_swap_64\n"
        ".type bsls_ByteOrder__Util_sparc_swap_64,#function\n"
        "bsls_ByteOrder__Util_sparc_swap_64:\n"
        "ldxa [%o0] 0x88, %o2\n" // load into %o2
        "srl %o2, 0, %o1\n"      // %o1 = %o2 & UINT_MAX
        "retl\n"
        "srlx %o2, 32, %o0\n");  // %o0 = %o2 >> 32;

#else  // BSLS_PLATFORM__CPU_64_BIT

    asm(".global bsls_ByteOrder__Util_sparc_swap_64\n"
        ".type bsls_ByteOrder__Util_sparc_swap_64,#function\n"
        "bsls_ByteOrder__Util_sparc_swap_64:\n"
        "retl\n"
        "ldxa [%o0] 0x88, %o0\n");

#endif  // BSLS_PLATFORM__CPU_32_BIT
}

#endif  // BSLS_PLATFORM__CPU_SPARC

}  // close enterprise namespace

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2009
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
