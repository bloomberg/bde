// bsls_atomicoperations_sparc_sun_cc_default.t.cpp                   -*-C++-*-

#include <bsls_atomicoperations_sparc_sun_cc_default.h>

#include <bsls_types.h>

#include <cstdlib>

using namespace BloombergLP;
using namespace std;

#if (defined(BSLS_PLATFORM_CPU_SPARC_V9)                                     \
                                     || defined(BSLS_PLATFORM_CPU_SPARC_32)) \
    && (defined(BSLS_PLATFORM_CMP_GNU) || defined(BSLS_PLATFORM_CMP_SUN))

typedef bsls::Types::UintPtr UintPtr;

UintPtr getInt = (UintPtr) &bsls_AtomicOperations_Sparc_GetInt;
UintPtr setInt = (UintPtr) &bsls_AtomicOperations_Sparc_SetInt;
UintPtr SwapInt = (UintPtr) &bsls_AtomicOperations_Sparc_SwapInt;
UintPtr SwapIntAcqRel = (UintPtr) &bsls_AtomicOperations_Sparc_SwapIntAcqRel;
UintPtr TestAndSwapInt = (UintPtr) &bsls_AtomicOperations_Sparc_TestAndSwapInt;
UintPtr TestAndSwapIntAcqRel =
                   (UintPtr) &bsls_AtomicOperations_Sparc_TestAndSwapIntAcqRel;
UintPtr AddInt = (UintPtr) &bsls_AtomicOperations_Sparc_AddInt;

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
