// bsls_atomicoperations_sparc_sun_cc_default.cpp                     -*-C++-*-

#include <bsls_atomicoperations_sparc_sun_cc_default.h>

#include <bsls_ident.h>
BSLS_IDENT("$Id$ $CSID$")

#include <bsls_annotation.h>

#if (defined(BSLS_PLATFORM_CPU_SPARC_V9)                                     \
                                     || defined(BSLS_PLATFORM_CPU_SPARC_32)) \
    && (defined(BSLS_PLATFORM_CMP_GNU) || defined(BSLS_PLATFORM_CMP_SUN))

namespace BloombergLP {

// SPARC implements the Total Store Order (TSO) memory model in which
// #LoadLoad, #LoadStore and #StoreStore barriers are no-op.  Consequently the
// relaxed operations already ordered the way that satisfies the
// acquire/release semantics.  See http://g.oswego.edu/dl/jmm/cookbook.html and
// the SPARC V9 manual for details.

static
void bsls_AtomicOperations_Sparc_AssemblyContainer() BSLS_ANNOTATION_USED;
    // Never called, just holds aseembly code.

void bsls_AtomicOperations_Sparc_AssemblyContainer()
{
    // int bsls_AtomicOperations_Sparc_GetInt(const volatile int*);

    asm(".global bsls_AtomicOperations_Sparc_GetInt\n"
        ".type bsls_AtomicOperations_Sparc_GetInt,#function\n"
        "bsls_AtomicOperations_Sparc_GetInt:\n"
        "membar #StoreLoad\n"
        "retl\n"
        "ld [%o0], %o0");

    // void bsls_AtomicOperations_Sparc_SetInt(volatile int*, int);

    asm(".global bsls_AtomicOperations_Sparc_SetInt\n"
        ".type bsls_AtomicOperations_Sparc_SetInt,#function\n"
        "bsls_AtomicOperations_Sparc_SetInt:\n"
        "st %o1, [%o0]\n"
        "retl\n"
        "membar #StoreLoad");

    // int bsls_AtomicOperations_Sparc_SwapInt(volatile int*, int);

    asm(".global bsls_AtomicOperations_Sparc_SwapInt\n"
        ".type bsls_AtomicOperations_Sparc_SwapInt,#function\n"
        "bsls_AtomicOperations_Sparc_SwapInt:\n"
        "ld [%o0], %o2\n"
        "1:\n"
        "mov %o1, %o3\n"
        "cas [%o0], %o2, %o3\n"
        "cmp %o2, %o3\n"
        "bne,a,pn %icc,1b\n"
        "mov %o3, %o2\n"
        "mov %o3, %o0\n"
        "retl\n"
        "membar #StoreLoad");

    // int bsls_AtomicOperations_Sparc_SwapIntAcqRel(volatile int*, int);

    asm(".global bsls_AtomicOperations_Sparc_SwapIntAcqRel\n"
        ".type bsls_AtomicOperations_Sparc_SwapIntAcqRel,#function\n"
        "bsls_AtomicOperations_Sparc_SwapIntAcqRel:\n"
        "ld [%o0], %o2\n"
        "1:\n"
        "mov %o1, %o3\n"
        "cas [%o0], %o2, %o3\n"
        "cmp %o2, %o3\n"
        "bne,a,pn %icc,1b\n"
        "mov %o3, %o2\n"
        "retl\n"
        "mov %o3, %o0");

    // int bsls_AtomicOperations_Sparc_TestAndSwapInt(volatile int*, int, int);

    asm(".global bsls_AtomicOperations_Sparc_TestAndSwapInt\n"
        ".type bsls_AtomicOperations_Sparc_TestAndSwapInt,#function\n"
        "bsls_AtomicOperations_Sparc_TestAndSwapInt:\n"
        "cas [%o0], %o1, %o2\n"
        "mov %o2, %o0\n"
        "retl\n"
        "membar #StoreLoad");

    // int bsls_AtomicOperations_Sparc_TestAndSwapIntAcqRel(
    //                                                volatile int*, int, int);

    asm(".global bsls_AtomicOperations_Sparc_TestAndSwapIntAcqRel\n"
        ".type bsls_AtomicOperations_Sparc_TestAndSwapIntAcqRel,#function\n"
        "bsls_AtomicOperations_Sparc_TestAndSwapIntAcqRel:\n"
        "cas [%o0], %o1, %o2\n"
        "retl\n"
        "mov %o2, %o0");

    // int bsls_AtomicOperations_Sparc_AddInt(volatile int*, int);

    asm(".global bsls_AtomicOperations_Sparc_AddInt\n"
        ".type bsls_AtomicOperations_Sparc_AddInt,#function\n"
        "bsls_AtomicOperations_Sparc_AddInt:\n"
        "ld [%o0], %o2\n"
        "1:\n"
        "add %o2, %o1, %o3\n"
        "cas [%o0], %o2, %o3\n"     // %o3 gets old [%o0]
        "cmp %o2, %o3\n"            // old [%o0] is what's expected?
        "bne,a,pn %icc, 1b\n"       // retry if not
        "mov %o3, %o2\n"            // executed with the branch
        "add %o2, %o1, %o0\n"       // return the new value
        "retl\n"
        "membar #StoreLoad");

    // int bsls_AtomicOperations_Sparc_AddIntRelaxed(volatile int*, int);

    asm(".global bsls_AtomicOperations_Sparc_AddIntRelaxed\n"
        ".type bsls_AtomicOperations_Sparc_AddIntRelaxed,#function\n"
        "bsls_AtomicOperations_Sparc_AddIntRelaxed:\n"
        "ld [%o0], %o2\n"
        "1:\n"
        "add %o2, %o1, %o3\n"
        "cas [%o0], %o2, %o3\n"     // %o3 gets old [%o0]
        "cmp %o2, %o3\n"            // old [%o0] is what's expected?
        "bne,a,pn %icc, 1b\n"       // retry if not
        "mov %o3, %o2\n"            // executed with the branch
        "retl\n"
        "add %o2, %o1, %o0\n");     // return the new value
}

}  // close enterprise namespace

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
