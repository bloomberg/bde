// bsls_atomicoperations_powerpc_aix_xlc_default.cpp                  -*-C++-*-

#include <bsls_atomicoperations_powerpc_aix_xlc_default.h>

#include <bsls_ident.h>
BSLS_IDENT("$Id$ $CSID$")

#include <bsls_types.h>

#if defined(BSLS_PLATFORM_CPU_POWERPC) && defined(BSLS_PLATFORM_CMP_IBM)

namespace BloombergLP {

namespace bsls {

// For reference on atomic operations on the PowerPC platform see:
// http://www.rdrop.com/users/paulmck/scalability/paper/N2745r.2011.03.04a.html
// http://www.rdrop.com/users/paulmck/scalability/paper/
//                                                    N2745rP5.2010.02.19a.html

int AtomicOperations_Powerpc_GetInt(const volatile int *atomicInt)
{
    int result;

    asm volatile (
        "       sync                            \n\t"

        "1:     lwarx %[res], %[zero], %[obj]   \n\t"
        "       cmpw %[res], %[res]             \n\t" // create data
                                                      // dependency for
                                                      // load/load ordering
        "       bne- 1b                         \n\t" // never taken
        "       isync                           \n\t"
                : [res]  "=b" (result)
                : [obj]  "b"  (atomicInt),
                  [zero] "i"  (0)
                : "cr0", "ctr");

    return result;
}

int AtomicOperations_Powerpc_GetIntAcquire(const volatile int *atomicInt)
{
    int result;

    asm volatile (
        "1:     lwz %[res], %[obj]              \n\t"
        "       cmpw %[res], %[res]             \n\t" // create data
                                                      // dependency for
                                                      // load/load ordering
        "       bne- 1b                         \n\t" // never taken
        "       isync                           \n\t"
                : [res]  "=b" (result)
                : [obj]  "m"  (*atomicInt),
                  [zero] "i"  (0)
                : "cr0", "ctr");

    return result;
}

void AtomicOperations_Powerpc_SetInt(volatile int *atomicInt, int value)
{
    asm volatile (
        "       sync                    \n\t"
        "       stw %[val], %[obj]      \n\t"
                : [obj] "=m" (*atomicInt)
                : [val]  "b"  (value));
}

void AtomicOperations_Powerpc_SetIntRelease(volatile int *atomicInt,
                                            int value)
{
    asm volatile (
        "       lwsync                  \n\t"
        "       stw %[val], %[obj]      \n\t"
                : [obj] "=m" (*atomicInt)
                : [val]  "b"  (value));
}

int AtomicOperations_Powerpc_SwapInt(volatile int *atomicInt,
                                     int swapValue)
{
    int result;

    asm volatile (
        "       sync                                \n\t"

        "1:     lwarx %[res], %[zero], %[obj]       \n\t"
        "       stwcx. %[val], %[zero], %[obj]      \n\t"
        "       bne- 1b                             \n\t"

        "       isync                               \n\t"
                : [res]  "=&b" (result)
                : [obj]  "b"   (atomicInt),
                  [val]  "b"   (swapValue),
                  [zero] "i"   (0)
                : "cr0", "ctr");

    return result;
}

int AtomicOperations_Powerpc_SwapIntAcqRel(volatile int *atomicInt,
                                           int swapValue)
{
    int result;

    asm volatile (
        "       lwsync                              \n\t"

        "1:     lwarx %[res], %[zero], %[obj]       \n\t"
        "       stwcx. %[val], %[zero], %[obj]      \n\t"
        "       bne- 1b                             \n\t"

        "       isync                               \n\t"
                : [res]  "=&b" (result)
                : [obj]  "b"   (atomicInt),
                  [val]  "b"   (swapValue),
                  [zero] "i"   (0)
                : "cr0", "ctr");

    return result;
}

int AtomicOperations_Powerpc_TestAndSwapInt(volatile int *atomicInt,
                                            int compareValue,
                                            int swapValue)
{
    int result;

    asm volatile (
        "       sync                            \n\t"

        "1:     lwarx %[res], %[zero], %[obj]   \n\t"  // load and reserve
        "       cmpw %[cmp], %[res]             \n\t"  // compare values
        "       bne- 2f                         \n\t"
        "       stwcx. %[val], %[zero], %[obj]  \n\t"  // store new value
        "       bne- 1b                         \n\t"

        "       isync                           \n\t"
        "2:                                     \n\t"
                : [res]  "=&b" (result)
                : [obj]  "b"   (atomicInt),
                  [cmp]  "b"   (compareValue),
                  [val]  "b"   (swapValue),
                  [zero] "i"   (0)
                : "cr0", "ctr");

    return result;
}

int AtomicOperations_Powerpc_TestAndSwapIntAcqRel(volatile int *atomicInt,
                                                  int compareValue,
                                                  int swapValue)
{
    int result;

    asm volatile (
        "       lwsync                          \n\t"

        "1:     lwarx %[res], %[zero], %[obj]   \n\t"  // load and reserve
        "       cmpw %[cmp], %[res]             \n\t"  // compare values
        "       bne- 2f                         \n\t"
        "       stwcx. %[val], %[zero], %[obj]  \n\t"  // store new value
        "       bne- 1b                         \n\t"

        "       isync                           \n\t"
        "2:                                     \n\t"
                : [res]  "=&b" (result)
                : [obj]  "b"   (atomicInt),
                  [cmp]  "b"   (compareValue),
                  [val]  "b"   (swapValue),
                  [zero] "i"   (0)
                : "cr0", "ctr");

    return result;
}

int AtomicOperations_Powerpc_AddInt(volatile int *atomicInt, int value)
{
    int result;

    asm volatile (
        "       sync                            \n\t"

        "1:     lwarx %[res], %[zero], %[obj]   \n\t"  // load and reserve
        "       add %[res], %[val], %[res]      \n\t"  // add the operand
        "       stwcx. %[res], %[zero], %[obj]  \n\t"  // store old value
                                                       // if still reserved
        "       bne- 1b                         \n\t"
        "       isync                           \n\t"
                : [res]  "=&b" (result)
                : [obj]  "b"   (atomicInt),
                  [val]  "b"   (value),
                  [zero] "i"   (0)
                : "cr0", "ctr");

    return result;
}

int AtomicOperations_Powerpc_AddIntAcqRel(volatile int *atomicInt,
                                          int value)
{
    int result;

    asm volatile (
        "       lwsync                          \n\t"

        "1:     lwarx %[res], %[zero], %[obj]   \n\t"  // load and reserve
        "       add %[res], %[val], %[res]      \n\t"  // add the operand
        "       stwcx. %[res], %[zero], %[obj]  \n\t"  // store old value
                                                       // if still reserved
        "       bne- 1b                         \n\t"
        "       isync                           \n\t"
                : [res]  "=&b" (result)
                : [obj]  "b"   (atomicInt),
                  [val]  "b"   (value),
                  [zero] "i"   (0)
                : "cr0", "ctr");

    return result;
}

}  // close package namespace

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
