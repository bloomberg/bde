// bsls_atomicoperations_sparc32_sun_cc.t.cpp                         -*-C++-*-

#include <bsls_atomicoperations_sparc32_sun_cc.h>

#include <cstdlib>

using namespace BloombergLP;
using namespace std;

// Make sure routines are at least defined somewhere.

#if defined(BSLS_PLATFORM_CPU_SPARC_32) \
    && (defined(BSLS_PLATFORM_CMP_GNU) || defined(BSLS_PLATFORM_CMP_SUN))

typedef bsls::Types::Int64 (*GetInt64Ptr)(
                                 const volatile bsls::Types::Int64 *atomicInt);
GetInt64Ptr getInt64        = &bsls_AtomicOperations_Sparc32_GetInt64;
GetInt64Ptr getInt64Relaxed = &bsls_AtomicOperations_Sparc32_GetInt64Relaxed;

typedef void (*SetInt64Ptr)(
             volatile bsls::Types::Int64 *atomicInt, bsls::Types::Int64 value);
SetInt64Ptr setInt64        = &bsls_AtomicOperations_Sparc32_SetInt64;
SetInt64Ptr setInt64Relaxed = bsls_AtomicOperations_Sparc32_SetInt64Relaxed;

typedef bsls::Types::Int64 (*SwapInt64Ptr)(
            volatile bsls::Types::Int64 *atomicInt,
            bsls::Types::Int64 swapValue);

SwapInt64Ptr swapInt64       = &bsls_AtomicOperations_Sparc32_SwapInt64;
SwapInt64Ptr swapInt64AcqRel = &bsls_AtomicOperations_Sparc32_SwapInt64AcqRel;

typedef bsls::Types::Int64 (*TestAndSwapInt64)(
            volatile bsls::Types::Int64 *atomicInt,
            bsls::Types::Int64 compareValue,
            bsls::Types::Int64 swapValue);

TestAndSwapInt64 testAndSwapInt64 =
                         &bsls_AtomicOperations_Sparc32_TestAndSwapInt64;
TestAndSwapInt64 testAndSwapInt64AcqRel =
                         &bsls_AtomicOperations_Sparc32_TestAndSwapInt64AcqRel;

typedef bsls::Types::Int64 (*AddInt64)(
            volatile bsls::Types::Int64 *atomicInt, bsls::Types::Int64 value);

AddInt64 addInt64        = &bsls_AtomicOperations_Sparc32_AddInt64;
AddInt64 addInt64Relaxed = &bsls_AtomicOperations_Sparc32_AddInt64Relaxed;

#endif

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;

    switch (test) { case 0:
        return 0;                                                     // RETURN
      default:
        return -1;                                                    // RETURN
    }
}

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
