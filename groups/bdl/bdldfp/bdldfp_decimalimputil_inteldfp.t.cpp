// bdldfp_decimalimputil_inteldfp.t.cpp                               -*-C++-*-
#include <bdldfp_decimalimputil_inteldfp.h>

#include <bsls_ident.h>
BSLS_IDENT("$Id$")

#include <bdls_testutil.h>
#include <bsls_assert.h>
#include <bsl_iostream.h>
#include <bsl_cstdlib.h>

// ============================================================================
//                                 TEST PLAN
// ----------------------------------------------------------------------------
//                                  Overview
//                                  --------
// The Intel DFP implementation provides a BID representation of DFP.  The code
// is written in C, but due to compiler-portability concerns, we have to
// compile it with a C++ compiler.  This test driver is not blank, to address
// some C/C++ compatibility concerns.
//
// Global Concerns:
//
//:  1 The IntelDFP library defines some manifest constants that might cause
//:    linker failures.
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

using namespace BloombergLP;
using bsl::cout;
using bsl::cerr;
using bsl::flush;
using bsl::endl;
using bsl::atoi;

//=============================================================================
//                    STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------

namespace {

int testStatus = 0;

void aSsErT(bool b, const char *s, int i)
{
    if (b) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (0 <= testStatus && testStatus <= 100) ++testStatus;
    }
}

}  // close unnamed namespace

//=============================================================================
//                       STANDARD BDE TEST DRIVER MACROS
//-----------------------------------------------------------------------------

#define ASSERT       BDLS_TESTUTIL_ASSERT

extern "C" {
extern const unsigned long long int  __four_over_pi[];
}

#define DEFINES
#include "library_float128_dpml_four_over_pi.cpp"
typedef struct { float a, b; double c; } SQRT_COEF_STRUCT;
extern "C" const SQRT_COEF_STRUCT __dpml_bid_sqrt_t_table[];
#undef DEFINES

int main()
{
    {
        const SQRT_COEF_STRUCT *p = &__dpml_bid_sqrt_t_table[0];
        ASSERT(p);
    }

    {
        const DIGIT_TYPE *p = &FOUR_OVER_PI_TABLE_NAME[0];
        ASSERT(p);
    }

    return -1;
}

// ----------------------------------------------------------------------------
// Copyright 2014 Bloomberg Finance L.P.
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
