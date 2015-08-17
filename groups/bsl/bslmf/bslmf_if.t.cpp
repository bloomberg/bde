// bslmf_if.t.cpp                                                     -*-C++-*-

#include <bslmf_if.h>

#include <bslmf_nil.h>  // for testing only

#include <stdlib.h>
#include <iostream>

using namespace BloombergLP;
using namespace std;

//=============================================================================
//                                TEST PLAN
//-----------------------------------------------------------------------------
//                                Overview
//                                --------
// The component under test contains the simple meta-function 'bslmf::If'.  The
// meta-function is tested by enumeration of all combinations of (1) 'true' and
// 'false' conditions, and (2) defaulting of zero, one, or two of the type
// arguments to 'bslmf::Nil'.
//-----------------------------------------------------------------------------
// [ 1] bslmf::If<CONDITION, IF_TRUE_TYPE, IF_FALSE_TYPE>

//=============================================================================
//                  STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
static int testStatus = 0;

static void aSsErT(int c, const char *s, int i) {
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}
#define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }
//-----------------------------------------------------------------------------
#define LOOP_ASSERT(I,X) { \
    if (!(X)) { cout << #I << ": " << I << "\n"; aSsErT(1, #X, __LINE__);}}

#define LOOP2_ASSERT(I,J,X) { \
    if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " \
        << J << "\n"; aSsErT(1, #X, __LINE__); } }
//=============================================================================
#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", " << flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number
#define T_() cout << '\t' << flush;           // Print tab w/o linefeed.

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

namespace {

    // 'SizeOf' is a meta-function that returns the non-zero size of its type
    // argument, or 0 for 'bslmf::Nil'.

    template <class T>
    struct SizeOf {
        static const size_t VALUE = sizeof(T);
    };

    template <>
    struct SizeOf<bslmf::Nil> {
        static const size_t VALUE = 0;
    };

#define SIZEOF(TYPE) SizeOf<TYPE>::VALUE

    // The compile-time evaluations depend on the following types being of
    // different sizes.

    typedef char   TrueType;   // sizeof(TrueType) == 1
    typedef double FalseType;  // sizeof(FalseType) > 1

    const size_t True  = SIZEOF(TrueType);
    const size_t False = SIZEOF(FalseType);
    const size_t Nil   = 0;

    typedef bslmf::If<true, TrueType, FalseType>::Type CT0;
    const size_t C0 = SIZEOF(CT0);                        // True

    typedef bslmf::If<false, TrueType, FalseType>::Type CT1;
    const size_t C1 = SIZEOF(CT1);                        // False

    typedef bslmf::If<true>::Type CT2;
    const size_t C2 = SIZEOF(CT2);                        // Nil

    typedef bslmf::If<false>::Type CT3;
    const size_t C3 = SIZEOF(CT3);                        // Nil

    typedef bslmf::If<true, TrueType>::Type CT4;
    const size_t C4 = SIZEOF(CT4);                        // True

    typedef bslmf::If<false, TrueType>::Type CT5;
    const size_t C5 = SIZEOF(CT5);                        // Nil

    typedef bslmf::If<true, TrueType, FalseType>::Type CT10;
    const size_t C10 = SIZEOF(CT10);                      // True

    typedef bslmf::If<false, TrueType, FalseType>::Type CT11;
    const size_t C11 = SIZEOF(CT11);                      // False

    typedef bslmf::If<true>::Type CT12;
    const size_t C12 = SIZEOF(CT12);                      // Nil

    typedef bslmf::If<false>::Type CT13;
    const size_t C13 = SIZEOF(CT13);                      // Nil

    typedef bslmf::If<true, TrueType>::Type CT14;
    const size_t C14 = SIZEOF(CT14);                      // True

    typedef bslmf::If<false, TrueType>::Type CT15;
    const size_t C15 = SIZEOF(CT15);                      // Nil

}  // close unnamed namespace

//=============================================================================
//                GLOBAL TYPES AND FUNCTIONS FOR USAGE EXAMPLE
//-----------------------------------------------------------------------------

///Usage
//------
// The following snippets of code illustrate basic use of the 'bslmf::If'
// meta-function.  The examples make use of the following declarations to
// identify the type that is selected by a given constant integral expression:
//..
    enum TypeCode { T_UNKNOWN = 0, T_CHAR = 1, T_INT = 2, T_NIL = 3 };

    TypeCode whatType(char)       { return T_CHAR; }
    TypeCode whatType(int)        { return T_INT; }
    TypeCode whatType(bslmf::Nil) { return T_NIL; }
    TypeCode whatType(...)        { return T_UNKNOWN; }
//..

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    // int veryVerbose = argc > 3;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:  // Zero is always the leading case.
      case 2: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //   This case verifies that the usage example works as advertised.
        //
        // Concerns:
        //   That the usage example compiles, links, and runs as expected.
        //
        // Plan:
        //   Replicate the usage example from the component header here.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "USAGE EXAMPLE" << endl
                                  << "=============" << endl;

// In the following example, the meta-function condition (the first argument to
// 'bslmf::If') evaluates to true (non-zero).  Thus, 'bslmf::If<...>::Type' is
// a synonym for 'int'; i.e., it "evaluates" (at compile time) to 'int':
//..
     typedef int  T1;  ASSERT(1 <  sizeof(T1));
     typedef char T2;  ASSERT(1 == sizeof(T2));

     typedef bslmf::If<(sizeof(T1) > sizeof(T2)), T1, T2>::Type LargerType;
     ASSERT(T_INT == whatType(LargerType()));
//..
// In the next example, the condition argument evaluates to false (zero).  In
// this case, 'bslmf::If<...>::Type' evaluates to 'bslmf::Nil' since the third
// template argument (the "else" type) is not explicitly specified:
//..
     typedef bslmf::If<(sizeof(T2) > 1), int>::Type Type2;
     ASSERT(T_NIL == whatType(Type2()));
//..

      } break;

      case 1: {
        // --------------------------------------------------------------------
        // TESTING BSLMF_IF
        //   Test the 'bslmf::If' meta-function.
        //
        // Concerns:
        //   - That 'true' and 'false' conditions select the first and second
        //     type arguments, respectively.
        //   - That selected types that are not explicitly specified resolve to
        //     the default 'bslmf::Nil' type.
        //
        // Plan:
        //   - Provide distinct type arguments with both 'true' and 'false'
        //     conditions and verify that the correct type is selected.
        //   - Repeat the tests such that:
        //      1. The second type parameter (only) defaults to 'bslmf::Nil'.
        //      2. Both type parameters default to 'bslmf::Nil'.
        //
        // Testing:
        //   bslmf::If<CONDITION, IF_TRUE_TYPE, IF_FALSE_TYPE>
        // --------------------------------------------------------------------

        if (verbose) cout << "bslmf::If" << endl
                          << "=========" << endl;

        ASSERT(1 == True);
        ASSERT(1 <  False);

        ASSERT(True  == C0);
        ASSERT(False == C1);
        ASSERT(Nil   == C2);
        ASSERT(Nil   == C3);
        ASSERT(True  == C4);
        ASSERT(Nil   == C5);

        ASSERT(C10   == C0);
        ASSERT(C11   == C1);
        ASSERT(C12   == C2);
        ASSERT(C13   == C3);
        ASSERT(C14   == C4);
        ASSERT(C15   == C5);

      } break;
      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        cerr << "Error, non-zero test status = " << testStatus << "." << endl;
    }
    return testStatus;
}

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
