// ball_predicateset.t.cpp                                            -*-C++-*-
#include <ball_predicateset.h>

#include <bslmf_issame.h>

#include <bsl_cstdlib.h>
#include <bsl_iostream.h>
#include <bsl_ostream.h>

using namespace bsl;
using bsl::cout;
using bsl::cerr;
using bsl::endl;

// ============================================================================
//                             TEST PLAN
// ----------------------------------------------------------------------------
//                             Overview
//                             --------
// The component under test consists of one or more simple 'typedef's.
//
// We make sure that each 'typedef' in the component is to the correct
// corresponding type, and that that type is complete (e.g., not simply a
// forward declaration) by constructing an object of the 'typedef' type.
//
// ----------------------------------------------------------------------------
// [ 1] TESTING TYPEDEFS

//=============================================================================
//                      STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
static int testStatus = 0;

static void aSsErT(int c, const char *s, int i)
{
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (0 <= testStatus && testStatus <= 100) ++testStatus;
    }
}

#define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

// ============================================================================
//                              MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    bool verbose = argc > 2;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:  // Zero is always the leading case.
      case 1: {
        // --------------------------------------------------------------------
        // TESTING TYPEDEF
        //
        // Concerns:
        //: 1 Each 'typedef' in the component aliases the correct underlying
        //:   type.
        //:
        //: 2 Each 'typedef' in the component can be constructed (the 'typedef'
        //:   is not to a forward-declared type).
        //
        // Plan:
        //: 1 For each 'typedef'
        //:   1 Use 'bsl::is_same' to make sure the alias is correct.
        //:
        //:   2 Construct an object of the 'typedef' to make sure the
        //:     underlying type is complete.
        //
        // Testing:
        //   TESTING TYPEDEFS
        // --------------------------------------------------------------------

        if (verbose)
            cout << "\nTESTING TYPEDEFS"
                 << "\n================" << endl;

        // typedef ball::ManagedAttributeSet ball::PredicateSet;
        {
            ASSERT(true ==
                   (bsl::is_same<BloombergLP::ball::ManagedAttributeSet,
                                 BloombergLP::ball::PredicateSet>::value));

            BloombergLP::ball::PredicateSet();
        }

      } break;
      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      }
    }

    return testStatus;
}

// ----------------------------------------------------------------------------
// Copyright 2020 Bloomberg Finance L.P.
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
