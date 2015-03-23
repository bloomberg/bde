// bsls_removereference.t.cpp                                         -*-C++-*-
#include <bsls_removereference.h>
#include <bsls_bsltestutil.h>
#include <bsls_compilerfeatures.h>

#include <sstream>
#include <stdio.h>
#include <stdlib.h>

using namespace BloombergLP;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                                   Overview
//                                   --------
// This component provides a type conversion, i.e., it doesn't really have any
// run-time behavior. The component is tested by defining (and testing) an
// auxiliary class template 'SameType' which is then utilitized to verify that
// the 'RemoveReference' type conversion behaves as expected.
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 2] SAMETYPE
// [ 3] REMOVEREFERENCE
// [ 4] USAGE EXAMPLE

// ============================================================================
//                     STANDARD BSL ASSERT TEST FUNCTION
// ----------------------------------------------------------------------------

namespace {

int testStatus = 0;

void aSsErT(bool condition, const char *message, int line)
{
    if (condition) {
        printf("Error " __FILE__ "(%d): %s    (failed)\n", line, message);

        if (0 <= testStatus && testStatus <= 100) {
            ++testStatus;
        }
    }
}

}  // close unnamed namespace

// ============================================================================
//               STANDARD BSL TEST DRIVER MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT       BSLS_BSLTESTUTIL_ASSERT
#define ASSERTV      BSLS_BSLTESTUTIL_ASSERTV

#define LOOP_ASSERT  BSLS_BSLTESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLS_BSLTESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLS_BSLTESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLS_BSLTESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLS_BSLTESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLS_BSLTESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLS_BSLTESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLS_BSLTESTUTIL_LOOP6_ASSERT

#define Q            BSLS_BSLTESTUTIL_Q   // Quote identifier literally.
#define P            BSLS_BSLTESTUTIL_P   // Print identifier and value.
#define P_           BSLS_BSLTESTUTIL_P_  // P(X) without '\n'.
#define T_           BSLS_BSLTESTUTIL_T_  // Print a tab (w/o newline).
#define L_           BSLS_BSLTESTUTIL_L_  // current Line number

//=============================================================================
//                              USAGE EXAMPLE
//-----------------------------------------------------------------------------

namespace {

template <class TYPE>
typename bsls::RemoveReference<TYPE>::type read(std::istream& in)
    // Read an object of template argument type from the specified stream
    // 'in' and return the read value. To determine if the reading the
    // value was successful check the status of 'in' (i.e., either the
    // conversion to 'bool' or '!in.fail()'.
{
    typedef typename bsls::RemoveReference<TYPE>::type type;
    type value = type();
    in >> value;
    return value;
}

}  // close unnamed namespace

//=============================================================================

namespace {

template <class, class>
struct SameType {
    enum { value = false };
};

template <class TYPE>
struct SameType<TYPE, TYPE> {
    enum { value = true };
};

struct TestType {};

}  // close unnamed namespace

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;

    testStatus = 0;
    switch (test) {
      case 0:
      case 4: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //
        // Concerns:
        //: 1 The usage example provided in the component header file compiles,
        //:   links, and runs as shown.
        //
        // Plan:
        //: 1 Incorporate usage example from header into test driver, remove
        //:   leading comment characters, and replace 'assert' with 'ASSERT'.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) printf("\nUSAGE EXAMPLE"
                            "\n=============\n");

        std::istringstream in("13");
        int                value = read<int&>(in);
        ASSERT(in);
        ASSERT(value == 13);
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // REMOVEREFERENCE
        //
        // Concerns:
        //   Test that the type conversions behave as expected.
        //
        // Plan:
        //   Use all relevant combinations of references and constness for a
        //   type and verify that RemoveReference<TYPE>::type yields a
        //   non-reference type.
        //
        // Testing:
        //    REMOVEREFERENCE
        // --------------------------------------------------------------------

        if (verbose) printf("\nREMOVEREFERENCE"
                            "\n===============\n");

        ASSERT((SameType<bsls::RemoveReference<TestType>::type,
                                                            TestType>::value));
        ASSERT((SameType<bsls::RemoveReference<TestType&>::type,
                                                            TestType>::value));

        ASSERT((SameType<bsls::RemoveReference<const TestType>::type,
                                                      const TestType>::value));
        ASSERT((SameType<bsls::RemoveReference<const TestType&>::type,
                                                      const TestType>::value));

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
        ASSERT((SameType<bsls::RemoveReference<TestType&&>::type,
                                                           TestType >::value));
        ASSERT((SameType<bsls::RemoveReference<const TestType&&>::type,
                                                      const TestType>::value));
#endif // BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
        } break;
      case 2: {
        // --------------------------------------------------------------------
        // SAMETYPE
        //
        // Concerns:
        //   Test that the auxiliary type SameType detects identical and
        //   non-identical types correctly.
        //
        // Plan:
        //   Use all relevant combinations of references and constness for a
        //   type and verify that SameType<TYPE1, TYPE2>::type yield 'true'
        //   when 'TYPE1' and 'TYPE2' are the same.
        //
        // Testing:
        //   SAMETYPE
        // --------------------------------------------------------------------

        if (verbose) printf("\nSAMETYPE"
                            "\n========\n");

        ASSERT((SameType<TestType, TestType>::value));
        ASSERT((!SameType<TestType, TestType&>::value));
        ASSERT((!SameType<TestType, const TestType>::value));
        ASSERT((!SameType<TestType, const TestType&>::value));

        ASSERT((!SameType<TestType&, TestType>::value));
        ASSERT((SameType<TestType&, TestType&>::value));
        ASSERT((!SameType<TestType&, const TestType>::value));
        ASSERT((!SameType<TestType&, const TestType&>::value));

        ASSERT((!SameType<const TestType, TestType>::value));
        ASSERT((!SameType<const TestType, TestType&>::value));
        ASSERT((SameType<const TestType, const TestType>::value));
        ASSERT((!SameType<const TestType, const TestType&>::value));

        ASSERT((!SameType<const TestType&, TestType>::value));
        ASSERT((!SameType<const TestType&, TestType&>::value));
        ASSERT((!SameType<const TestType&, const TestType>::value));
        ASSERT((SameType<const TestType&, const TestType&>::value));

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
        ASSERT((!SameType<TestType&&, TestType>::value));
        ASSERT((!SameType<TestType&&, TestType&>::value));
        ASSERT((!SameType<TestType&&, const TestType>::value));
        ASSERT((!SameType<TestType&&, const TestType&>::value));

        ASSERT((!SameType<const TestType&&, TestType>::value));
        ASSERT((!SameType<const TestType&&, TestType&>::value));
        ASSERT((!SameType<const TestType&&, const TestType>::value));
        ASSERT((!SameType<const TestType&&, const TestType&>::value));

        ASSERT((!SameType<TestType, TestType&&>::value));
        ASSERT((!SameType<TestType, const TestType&&>::value));

        ASSERT((!SameType<TestType&, TestType&&>::value));
        ASSERT((!SameType<TestType&, const TestType&&>::value));

        ASSERT((!SameType<const TestType, TestType&&>::value));
        ASSERT((!SameType<const TestType, const TestType&&>::value));

        ASSERT((!SameType<const TestType&, TestType&&>::value));
        ASSERT((!SameType<const TestType&, const TestType&&>::value));

        ASSERT((SameType<TestType&&, TestType&&>::value));
        ASSERT((!SameType<TestType&&, const TestType&&>::value));

        ASSERT((!SameType<const TestType&&, TestType&&>::value));
        ASSERT((SameType<const TestType&&, const TestType&&>::value));
#endif // BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //
        // Concerns:
        //: 1 Verify that 'RemoveReference<TYPE>' has a nested 'type' member
        //:   for all variations of specifying a reference.
        //
        // Plan:
        //: 1 Use 'typedef's for all variations of reference types.
        //
        // Testing:
        //     BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) printf("\nBREATHING TEST"
                            "\n==============\n");

        typedef  bsls::RemoveReference<int>::type type0;
        typedef  bsls::RemoveReference<int&>::type type1;
        typedef  bsls::RemoveReference<const int>::type type2;
        typedef  bsls::RemoveReference<const int&>::type type3;
#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
        typedef  bsls::RemoveReference<int&&>::type type4;
        typedef  bsls::RemoveReference<const int&&>::type type5;
#endif // BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
      } break;
      default: {
        fprintf(stderr, "WARNING: CASE `%d' NOT FOUND.\n", test);
        testStatus = -1;
      } break;
    }

    return testStatus;
}

// ----------------------------------------------------------------------------
// Copyright 2015 Bloomberg Finance L.P.
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
