// bsls_atomicoperations_sparc32_sun_cc.cpp                           -*-C++-*-

#include <bsls_atomicoperations_sparc32_sun_cc.h>

#include <bsls_ident.h>
BSLS_IDENT("$Id$ $CSID$")

#include <bsls_annotation.h>

#if defined(BSLS_PLATFORM_CPU_SPARC_32) \
    && (defined(BSLS_PLATFORM_CMP_GNU) || defined(BSLS_PLATFORM_CMP_SUN))

namespace BloombergLP {

// These function implementations rely on the V8+ ABI.  Our sparc processors
// supports it.  32-bit arguments are put into registers: %o0, %o1, etc.
// 64-bit arguments are put into register pairs: (%o0, %o1), (%o2, %o3), etc.
// We are allows to use 64-bit registers inside the functions.  But we'll need
// to convert 64-bit arguments from register pairs to single registers.
// Correspondingly, the 32-bit return value should be put into %o0, and the
// 64-bit return value - into (%o0, %o1) register pair.

static
void bsls_AtomicOperations_Sparc32_AssemblyContainer() BSLS_ANNOTATION_USED;
    // Never called, just holds assembly code.

void bsls_AtomicOperations_Sparc32_AssemblyContainer()
{
    // *** 64 bit operations ***

    // Int64 bsls_AtomicOperations_Sparc32_GetInt64(const volatile Int64*);

    asm(".global bsls_AtomicOperations_Sparc32_GetInt64\n"
        ".type bsls_AtomicOperations_Sparc32_GetInt64,#function\n"
        "bsls_AtomicOperations_Sparc32_GetInt64:\n"
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

    // Int64 bsls_AtomicOperations_Sparc32_GetInt64Relaxed(
    //                                                  const volatile Int64*);

    asm(".global bsls_AtomicOperations_Sparc32_GetInt64Relaxed\n"
        ".type bsls_AtomicOperations_Sparc32_GetInt64Relaxed,#function\n"
        "bsls_AtomicOperations_Sparc32_GetInt64Relaxed:\n"
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

    // void bsls_AtomicOperations_Sparc32_SetInt64(volatile Int64 *, Int64);

    asm(".global bsls_AtomicOperations_Sparc32_SetInt64\n"
        ".type bsls_AtomicOperations_Sparc32_SetInt64,#function\n"
        "bsls_AtomicOperations_Sparc32_SetInt64:\n"
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

    // void bsls_AtomicOperations_Sparc32_SetInt64Relaxed(
    //                                                volatile Int64 *, Int64);

    asm(".global bsls_AtomicOperations_Sparc32_SetInt64Relaxed\n"
        ".type bsls_AtomicOperations_Sparc32_SetInt64Relaxed,#function\n"
        "bsls_AtomicOperations_Sparc32_SetInt64Relaxed:\n"
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

    // Int64 bsls_AtomicOperations_Sparc32_SwapInt64(volatile Int64 *, Int64);

    asm(".global bsls_AtomicOperations_Sparc32_SwapInt64\n"
        ".type bsls_AtomicOperations_Sparc32_SwapInt64,#function\n"
        "bsls_AtomicOperations_Sparc32_SwapInt64:\n"
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

        "srlx %o3, 32, %o0\n"   // This shifts %o3 so %o0 gets the high
                                // 32-bit of %o3 in its low 32-bit.
        "retl\n"
        "membar #StoreLoad");

    // Int64 bsls_AtomicOperations_Sparc32_SwapInt64AcqRel(
    //                                                volatile Int64 *, Int64);

    asm(".global bsls_AtomicOperations_Sparc32_SwapInt64AcqRel\n"
        ".type bsls_AtomicOperations_Sparc32_SwapInt64AcqRel,#function\n"
        "bsls_AtomicOperations_Sparc32_SwapInt64AcqRel:\n"
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
        "retl\n"
        "srlx %o3, 32, %o0");   // This shifts %o3 so %o0 gets the high
                                // 32-bit of %o3 in its low 32-bit.

    // Int64 bsls_AtomicOperations_Sparc32_TestAndSwapInt64(
    //                                         volatile Int64 *, Int64, Int64);

    asm(".global bsls_AtomicOperations_Sparc32_TestAndSwapInt64\n"
        ".type bsls_AtomicOperations_Sparc32_TestAndSwapInt64,#function\n"
        "bsls_AtomicOperations_Sparc32_TestAndSwapInt64:\n"
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

    // Int64 bsls_AtomicOperations_Sparc32_TestAndSwapInt64AcqRel(
    //                                         volatile Int64 *, Int64, Int64);

    asm(".global bsls_AtomicOperations_Sparc32_TestAndSwapInt64AcqRel\n"
       ".type bsls_AtomicOperations_Sparc32_TestAndSwapInt64AcqRel,#function\n"
        "bsls_AtomicOperations_Sparc32_TestAndSwapInt64AcqRel:\n"
        "sllx %o1, 32, %o1\n"          // convert 1st long long
        "srl %o2, 0, %o2\n"
        "add %o1, %o2, %o1\n"          // 1st ll arg is in %o1
        "sllx %o3, 32, %o2\n"          // convert 2nd long long
        "srl %o4, 0, %o3\n"
        "add %o3, %o2, %o2\n"          // 2nd ll arg is in %o2
        "casx [%o0], %o1, %o2\n"
        "srlx %o2, 32, %o0\n"          // fix return value
        "retl\n"
        "srl %o2, 0, %o1");

    // Int64 bsls_AtomicOperations_Sparc32_AddInt64(volatile Int64 *, Int64);

    asm(".global bsls_AtomicOperations_Sparc32_AddInt64\n"
        ".type bsls_AtomicOperations_Sparc32_AddInt64,#function\n"
        "bsls_AtomicOperations_Sparc32_AddInt64:\n"
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

    // Int64 bsls_AtomicOperations_Sparc32_AddInt64Relaxed(
    //                                                volatile Int64 *, Int64);

    asm(".global bsls_AtomicOperations_Sparc32_AddInt64Relaxed\n"
        ".type bsls_AtomicOperations_Sparc32_AddInt64Relaxed,#function\n"
        "bsls_AtomicOperations_Sparc32_AddInt64Relaxed:\n"
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
