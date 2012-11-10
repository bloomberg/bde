// bsls_byteorder.cpp                                                 -*-C++-*-
#include <bsls_byteorder.h>

#include <bsls_ident.h>
BSLS_IDENT("$Id$ $CSID$")

namespace BloombergLP {

#if defined(BSLS_PLATFORM_CPU_SPARC) && !defined(BSLS_PLATFORM_CMP_GNU)
void bsls_ByteOrder_Util_common()
{
// Commented out since the generic implementation is probably faster than the
// function call for 16 and 32 bits.
//..
//  asm(".global bsls_ByteOrder_Util_sparc_swap_16\n"
//      ".type bsls_ByteOrder_Util_sparc_swap_16,#function\n"
//      "bsls_ByteOrder_Util_sparc_swap_16:\n"
//      "retl\n"
//      "lduha [%o0] 0x88, %o0\n");
//
//  asm(".global bsls_ByteOrder_Util_sparc_swap_32\n"
//      ".type bsls_ByteOrder_Util_sparc_swap_32,#function\n"
//      "bsls_ByteOrder_Util_sparc_swap_32:\n"
//      "retl\n"
//      "lduwa [%o0] 0x88, %o0\n");
//..

#ifdef BSLS_PLATFORM_CPU_32_BIT
    asm(".global bsls_ByteOrder_Util_sparc_swap_64\n"
        ".type bsls_ByteOrder_Util_sparc_swap_64,#function\n"
        "bsls_ByteOrder_Util_sparc_swap_64:\n"
        "ldxa [%o0] 0x88, %o2\n" // load into %o2
        "srl %o2, 0, %o1\n"      // %o1 = %o2 & UINT_MAX
        "retl\n"
        "srlx %o2, 32, %o0\n");  // %o0 = %o2 >> 32;

#else  // BSLS_PLATFORM_CPU_64_BIT

    asm(".global bsls_ByteOrder_Util_sparc_swap_64\n"
        ".type bsls_ByteOrder_Util_sparc_swap_64,#function\n"
        "bsls_ByteOrder_Util_sparc_swap_64:\n"
        "retl\n"
        "ldxa [%o0] 0x88, %o0\n");

#endif  // BSLS_PLATFORM_CPU_32_BIT
}

#endif  // BSLS_PLATFORM_CPU_SPARC

}  // close enterprise namespace

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
