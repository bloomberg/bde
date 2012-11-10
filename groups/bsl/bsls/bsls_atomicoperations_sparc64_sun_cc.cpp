// bsls_atomicoperations_sparc64_sun_cc.cpp                           -*-C++-*-

#include <bsls_atomicoperations_sparc64_sun_cc.h>

#include <bsls_ident.h>
BSLS_IDENT("$Id$ $CSID$")

#if defined(BSLS_PLATFORM_CPU_SPARC_V9) \
    && (defined(BSLS_PLATFORM_CMP_GNU) || defined(BSLS_PLATFORM_CMP_SUN))

namespace BloombergLP {

static
void bsls_AtomicOperations_Sparc64_AssemblyContainer()
{
        // *** 64 bit operations ***

    // Int64 bsls_AtomicOperations_Sparc64_GetInt64(const volatile Int64*);
    asm(".global bsls_AtomicOperations_Sparc64_GetInt64\n"
        ".type bsls_AtomicOperations_Sparc64_GetInt64,#function\n"
        "bsls_AtomicOperations_Sparc64_GetInt64:\n"
        "membar #StoreLoad\n"
        "retl\n"
        "ldx [%o0], %o0");

    // void bsls_AtomicOperations_Sparc64_SetInt64(volatile Int64 *, Int64);
    asm(".global bsls_AtomicOperations_Sparc64_SetInt64\n"
        ".type bsls_AtomicOperations_Sparc64_SetInt64,#function\n"
        "bsls_AtomicOperations_Sparc64_SetInt64:\n"
        "stx %o1, [%o0]\n"
        "retl\n"
        "membar #StoreLoad");

    // Int64 bsls_AtomicOperations_Sparc64_SwapInt64(volatile Int64 *, Int64);
    asm(".global bsls_AtomicOperations_Sparc64_SwapInt64\n"
        ".type bsls_AtomicOperations_Sparc64_SwapInt64,#function\n"
        "bsls_AtomicOperations_Sparc64_SwapInt64:\n"
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

    // Int64 bsls_AtomicOperations_Sparc64_SwapInt64AcqRel(
    //                                                volatile Int64 *, Int64);
    asm(".global bsls_AtomicOperations_Sparc64_SwapInt64AcqRel\n"
        ".type bsls_AtomicOperations_Sparc64_SwapInt64AcqRel,#function\n"
        "bsls_AtomicOperations_Sparc64_SwapInt64AcqRel:\n"
        "ldx [%o0], %o2\n"
        "1:\n"
        "mov %o1, %o3\n"
        "casx [%o0], %o2, %o3\n"
        "cmp %o2, %o3\n"
        "bne,a,pn %xcc,1b\n"
        "mov %o3,%o2\n"
        "retl\n"
        "mov %o3, %o0");

    // Int64 bsls_AtomicOperations_Sparc64_TestAndSwapInt64(
    //                                         volatile Int64 *, Int64, Int64);
    asm(".global bsls_AtomicOperations_Sparc64_TestAndSwapInt64\n"
        ".type bsls_AtomicOperations_Sparc64_TestAndSwapInt64,#function\n"
        "bsls_AtomicOperations_Sparc64_TestAndSwapInt64:\n"
        "casx [%o0], %o1, %o2\n"
        "mov %o2, %o0\n"
        "retl\n"
        "membar #StoreLoad");

    // Int64 bsls_AtomicOperations_Sparc64_TestAndSwapInt64AcqRel(
    //                                         volatile Int64 *, Int64, Int64);
    asm(".global bsls_AtomicOperations_Sparc64_TestAndSwapInt64AcqRel\n"
       ".type bsls_AtomicOperations_Sparc64_TestAndSwapInt64AcqRel,#function\n"
        "bsls_AtomicOperations_Sparc64_TestAndSwapInt64AcqRel:\n"
        "casx [%o0], %o1, %o2\n"
        "retl\n"
        "mov %o2, %o0");

    // Int64 bsls_AtomicOperations_Sparc64_AddInt64(volatile Int64 *, Int64);
    asm(".global bsls_AtomicOperations_Sparc64_AddInt64\n"
        ".type bsls_AtomicOperations_Sparc64_AddInt64,#function\n"
        "bsls_AtomicOperations_Sparc64_AddInt64:\n"
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

    // Int64 bsls_AtomicOperations_Sparc64_AddInt64Relaxed(
    //                                                volatile Int64 *, Int64);
    asm(".global bsls_AtomicOperations_Sparc64_AddInt64Relaxed\n"
        ".type bsls_AtomicOperations_Sparc64_AddInt64Relaxed,#function\n"
        "bsls_AtomicOperations_Sparc64_AddInt64Relaxed:\n"
        "ldx [%o0], %o2\n"
        "1:\n"
        "add %o2, %o1, %o3\n"
        "casx [%o0], %o2, %o3\n"
        "cmp %o2, %o3\n"
        "bne,a,pn %xcc, 1b\n"
        "mov %o3, %o2\n"
        "retl\n"
        "add %o2, %o1, %o0");
}

}  // close enterprise namespace

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
