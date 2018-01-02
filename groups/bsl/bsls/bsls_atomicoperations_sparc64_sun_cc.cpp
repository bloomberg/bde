// bsls_atomicoperations_sparc64_sun_cc.cpp                           -*-C++-*-

#include <bsls_atomicoperations_sparc64_sun_cc.h>

#include <bsls_ident.h>
BSLS_IDENT("$Id$ $CSID$")

#include <bsls_annotation.h>

#if defined(BSLS_PLATFORM_CPU_SPARC_V9) \
    && (defined(BSLS_PLATFORM_CMP_GNU) || defined(BSLS_PLATFORM_CMP_SUN))

namespace BloombergLP {

static
void bsls_AtomicOperations_Sparc64_AssemblyContainer() BSLS_ANNOTATION_USED;
    // Never called, just holds asembly code.

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
