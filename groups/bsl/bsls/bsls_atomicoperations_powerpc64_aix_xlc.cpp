// bsls_atomicoperations_powerpc64_aix_xlc.cpp                        -*-C++-*-

#include <bsls_atomicoperations_powerpc64_aix_xlc.h>

#include <bsls_ident.h>
BSLS_IDENT("$Id$ $CSID$")

#include <bsls_types.h>

#if defined(BSLS_PLATFORM_CPU_POWERPC) && defined(BSLS_PLATFORM_CMP_IBM) \
    && defined(BSLS_PLATFORM_CPU_64_BIT)

namespace BloombergLP {

namespace bsls {

// For reference on atomic operations on the PowerPC platform see:
// http://www.rdrop.com/users/paulmck/scalability/paper/N2745r.2011.03.04a.html
// http://www.rdrop.com/users/paulmck/scalability/paper/
//                                                    N2745rP5.2010.02.19a.html

Types::Int64 AtomicOperations_Powerpc64_GetInt64(
                                  const volatile Types::Int64 *atomicInt)
{
    Types::Int64 result;

    asm volatile (
        "       sync                                \n\t"

        "1:     ldarx %[res], %[zero], %[obj]       \n\t"
        "       cmpd %[res], %[res]                 \n\t" // create data
                                                          // dependency for
                                                          // load/load ordering
        "       bne- 1b                             \n\t" // never taken
        "       isync                               \n\t"
                : [res]  "=b" (result)
                : [obj]  "b"  (atomicInt),
                  [zero] "i"  (0)
                : "cr0", "ctr");

    return result;
}

Types::Int64 AtomicOperations_Powerpc64_GetInt64Acquire(
                                  const volatile Types::Int64 *atomicInt)
{
    Types::Int64 result;

    asm volatile (
        "1:     ld %[res], %[obj]                   \n\t"
        "       cmpd %[res], %[res]                 \n\t" // create data
                                                          // dependency for
                                                          // load/load ordering
        "       bne- 1b                             \n\t" // never taken
        "       isync                               \n\t"
                : [res]  "=b" (result)
                : [obj]  "m"  (*atomicInt),
                  [zero] "i"  (0)
                : "cr0", "ctr");

    return result;
}

void AtomicOperations_Powerpc64_SetInt64(volatile Types::Int64 *atomicInt,
                                         Types::Int64           value)
{
    asm volatile (
        "       sync                        \n\t"
        "       std %[val], %[obj]          \n\t"
                : [obj] "=m" (*atomicInt)
                : [val] "b"  (value));
}

void AtomicOperations_Powerpc64_SetInt64Release(
                                        volatile Types::Int64 *atomicInt,
                                        Types::Int64           value)
{
    asm volatile (
        "       lwsync                      \n\t"
        "       std %[val], %[obj]          \n\t"
                : [obj] "=m" (*atomicInt)
                : [val] "b"  (value));
}

Types::Int64 AtomicOperations_Powerpc64_SwapInt64(
                                        volatile Types::Int64 *atomicInt,
                                        Types::Int64           value)
{
    Types::Int64 result;

    asm volatile (
        "       sync                                \n\t"

        "1:     ldarx %[res], %[zero], %[obj]       \n\t"
        "       stdcx. %[val], %[zero], %[obj]      \n\t"
        "       bne- 1b                             \n\t"

        "       isync                               \n\t"
                : [res]  "=&b" (result)
                : [obj]  "b"   (atomicInt),
                  [val]  "b"   (value),
                  [zero] "i"   (0)
                : "cr0", "ctr");

    return result;
}

Types::Int64 AtomicOperations_Powerpc64_SwapInt64AcqRel(
                                        volatile Types::Int64 *atomicInt,
                                        Types::Int64           value)
{
    Types::Int64 result;

    asm volatile (
        "       lwsync                              \n\t"

        "1:     ldarx %[res], %[zero], %[obj]       \n\t"
        "       stdcx. %[val], %[zero], %[obj]      \n\t"
        "       bne- 1b                             \n\t"

        "       isync                               \n\t"
                : [res]  "=&b" (result)
                : [obj]  "b"   (atomicInt),
                  [val]  "b"   (value),
                  [zero] "i"   (0)
                : "cr0", "ctr");

    return result;
}

Types::Int64 AtomicOperations_Powerpc64_testAndSwapInt64(
                                    volatile Types::Int64 *atomicInt,
                                    Types::Int64           compareValue,
                                    Types::Int64           swapValue)
{
    Types::Int64 result;

    asm volatile (
        "       sync                            \n\t"

        "1:     ldarx %[res], %[zero], %[obj]   \n\t"  // load and reserve
        "       cmpd %[cmp], %[res]             \n\t"  // compare values
        "       bne- 2f                         \n\t"

        "       stdcx. %[val], %[zero], %[obj]  \n\t"  // store the new value
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

Types::Int64 AtomicOperations_Powerpc64_testAndSwapInt64AcqRel(
                                    volatile Types::Int64 *atomicInt,
                                    Types::Int64           compareValue,
                                    Types::Int64           swapValue)
{
    Types::Int64 result;

    asm volatile (
        "       lwsync                          \n\t"

        "1:     ldarx %[res], %[zero], %[obj]   \n\t"  // load and reserve
        "       cmpd %[cmp], %[res]             \n\t"  // compare values
        "       bne- 2f                         \n\t"

        "       stdcx. %[val], %[zero], %[obj]  \n\t"  // store the new value
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

Types::Int64 AtomicOperations_Powerpc64_AddInt64(
                                        volatile Types::Int64 *atomicInt,
                                        Types::Int64           value)
{
    Types::Int64 result;

    asm volatile (
        "       sync                            \n\t"

        "1:     ldarx %[res], %[zero], %[obj]   \n\t" // load and reserve
        "       add %[res], %[res], %[val]      \n\t" // add the operand
        "       stdcx. %[res], %[zero], %[obj]  \n\t" // store old value if
                                                      // still reserved

        "       bne- 1b                         \n\t"

        "       isync                           \n\t"
                : [res]  "=&b" (result)
                : [obj]  "b"   (atomicInt),
                  [val]  "b"   (value),
                  [zero] "i"   (0)
                : "cr0", "ctr");

    return result;
}

Types::Int64 AtomicOperations_Powerpc64_AddInt64AcqRel(
                                        volatile Types::Int64 *atomicInt,
                                        Types::Int64           value)
{
    Types::Int64 result;

    asm volatile (
        "       lwsync                          \n\t"

        "1:     ldarx %[res], %[zero], %[obj]   \n\t" // load and reserve
        "       add %[res], %[res], %[val]      \n\t" // add the operand
        "       stdcx. %[res], %[zero], %[obj]  \n\t" // store old value if
                                                      // still reserved

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
