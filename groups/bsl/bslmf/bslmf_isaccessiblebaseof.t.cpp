// bslmf_isaccessiblebaseof.t.cpp                                     -*-C++-*-

#include <bslmf_isaccessiblebaseof.h>

#include <bsls_bsltestutil.h>
#include <bsls_platform.h>

#include <stdio.h>   // 'printf'
#include <stdlib.h>  // 'atoi'

// ============================================================================
//                                 TEST PLAN
// ----------------------------------------------------------------------------
//                                 Overview
//                                 --------
// The component under test defines meta-function 'bslmf::IsAccessibleBaseOf'
// that determines whether one class is an accessible (observable) base class
// of another.  Tests need to ensure that all pairs that have the base and
// derived relationship return 'true', and all others return 'false'.
// Interesting cases include transitive base and derived relationships,
// multiple inheritance, 'private' and 'protected' inheritance, and virtual
// inheritance.
// ----------------------------------------------------------------------------
// PUBLIC CLASS DATA
// [ 2] BloombergLP::bslmf::IsAccessibleBaseOf::value
// ----------------------------------------------------------------------------
// [ 1] USAGE EXAMPLE

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

// ============================================================================
//                     NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT_SAFE_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(EXPR)
#define ASSERT_SAFE_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPR)

// ============================================================================
//                    GLOBAL CLASS DEFINITIONS FOR TESTING
// ----------------------------------------------------------------------------
using namespace BloombergLP;

class Base {
};

class Derived : public Base {
};

#ifndef BSLS_PLATFORM_CMP_IBM
// There is a bug in the XLC compiler which causes these tests to fail.  The
// problem impacts bslmf_isconvertible, so if a workaround is applied, it would
// appear in that component instead of this one.
class PrivatelyDerived : private Base {
  public:
    bool IsBaseTest()
    {
        Base *base = this;
        (void)base;
        return bslmf::IsAccessibleBaseOf<Base, PrivatelyDerived>::value;
    }
    static bool IsBaseStaticTest()
    {
        PrivatelyDerived  pd;
        PrivatelyDerived *pd_ptr   = &pd;
        Base             *base_ptr = pd_ptr;
        (void)base_ptr;
        return bslmf::IsAccessibleBaseOf<Base, PrivatelyDerived>::value;
    }
};

class ProtectedlyDerived : protected Base {
  public:
    bool IsBaseTest()
    {
        Base *base = this;
        (void)base;
        return bslmf::IsAccessibleBaseOf<Base, ProtectedlyDerived>::value;
    }
    static bool IsBaseStaticTest()
    {
        ProtectedlyDerived  pd;
        ProtectedlyDerived *pd_ptr   = &pd;
        Base               *base_ptr = pd_ptr;
        (void)base_ptr;
        return bslmf::IsAccessibleBaseOf<Base, ProtectedlyDerived>::value;
    }
};

class DerivedOfProtectedlyDerived : private ProtectedlyDerived {
  public:
    bool IsBaseTest()
    {
        Base *base = this;
        (void)base;
        return bslmf::IsAccessibleBaseOf<Base, ProtectedlyDerived>::value;
    }
    static bool IsBaseStaticTest()
    {
        DerivedOfProtectedlyDerived  pd;
        DerivedOfProtectedlyDerived *pd_ptr   = &pd;
        Base                        *base_ptr = pd_ptr;
        (void)base_ptr;
        return bslmf::IsAccessibleBaseOf<Base,
                                         DerivedOfProtectedlyDerived>::value;
    }
};
#endif  // BSLS_PLATFORM_CMP_IBM

class TransitivelyDerived : public Derived {
};

class OtherBase {
};

class MultipleDerived : public Base, public OtherBase {
};

class VirtualDiamondLeft : virtual public Base {
};

class VirtualDiamondRight : virtual public Base {
};

class VirtualDiamondTop : public VirtualDiamondLeft,
                          public VirtualDiamondRight {
};

class AmbiguousLeft : public Base {
};

class AmbiguousRight : public Base {
};

class AmbiguousTop : public AmbiguousLeft, public AmbiguousRight {
};

class Incomplete;

// ============================================================================
//                              USAGE EXAMPLES
// ----------------------------------------------------------------------------

///Usage
///-----
// In this section we show intended use of this component.
//
namespace Example1 {
///Example 1: Base And Derived Classes
///- - - - - - - - - - - - - - - - - -
// Define two classes, one inheriting from the other.
//..
struct Base {
};
struct Derived : Base {
};
//..
// Evaluate 'bslmf::IsAccessibleBaseOf::value'.
//..
void example1()
{
    ASSERT((true == bslmf::IsAccessibleBaseOf<Base, Derived>::value));
    ASSERT((true == bslmf::IsAccessibleBaseOf<Base, Base>::value));
    ASSERT((true == bslmf::IsAccessibleBaseOf<Derived, Derived>::value));
    ASSERT((false == bslmf::IsAccessibleBaseOf<Derived, Base>::value));
}
//..
}  // close namespace Example1

namespace Example2 {
///Example 2: Unrelated Classes
/// - - - - - - - - - - - - - -
// Define two classes, one inheriting privately from the other.
//..
class Unrelated {
};

class Unrelated2 {
};
//..
// Evaluate 'bslmf::IsAccessibleBaseOf::value'.  Note that 'Derived' is not
// *observably* derived from 'Base', so std::is_base_of would evaluate 'true',
// but 'bslmf::IsAccessibleBaseOf' evaluates 'false'.
//..
void example2()
{
    ASSERT((false == bslmf::IsAccessibleBaseOf<Unrelated, Unrelated2>::value));
}
//..
}  // close namespace Example2

// ============================================================================
//                               MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    using namespace BloombergLP;

    const int test = argc > 1 ? atoi(argv[1]) : 0;

    const bool verbose             = argc > 2;
    const bool veryVerbose         = argc > 3;
    const bool veryVeryVerbose     = argc > 4;
    const bool veryVeryVeryVerbose = argc > 5;

    (void)verbose;              // suppress warning
    (void)veryVerbose;          // suppress warning
    (void)veryVeryVerbose;      // suppress warning
    (void)veryVeryVeryVerbose;  // suppress warning

    setbuf(stdout, NULL);  // Use unbuffered output

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) {
      case 0:  // Zero is always the leading case.
      case 2: {
        // --------------------------------------------------------------------
        // CLASS TRAITS
        //   Ensure that trait 'IsAccessibleBaseOf::value' correctly identifies
        //   the base-derived relationship between classes which inherit
        //   publicly and not ambiguously.
        //
        // Concerns:
        //: 1 The 'value' trait is 'true' for base-derived related classes that
        //:   inherit publicly, including transitive relationships, multiple
        //:   inheritance relationships, and virtual relationships.
        //:
        //: 2 The 'value' trait is 'false' for classes having ambiguous
        //:   inheritance, private or protected inheritance, or no
        //:   relationship.
        //
        // Plan:
        //: 1 Assert of the expected value of 'IsAccessibleBaseOf::value' in
        //:   all relevant configurations of types.
        //:
        //: 2 Assert expected value of 'IsAccessibleBaseOf::value' as returned
        //:   from a member function which exposes protected or private
        //:   relationships between classes.
        //:
        //: 3 Assert expected value of 'IsAccessibleBaseOf::value' as returned
        //:   from a static class function which exposes protected or private
        //:   relationships between classes.
        //
        // Testing:
        //    'value'
        // --------------------------------------------------------------------
        if (verbose)
            printf("CLASS TRAITS\n"
                   "============\n");
        ASSERT((true == bslmf::IsAccessibleBaseOf<Base, Derived>::value));
        ASSERT((true ==
                bslmf::IsAccessibleBaseOf<Base, TransitivelyDerived>::value));
        ASSERT((true ==
                bslmf::IsAccessibleBaseOf<Base, MultipleDerived>::value));
        ASSERT((true ==
                bslmf::IsAccessibleBaseOf<OtherBase, MultipleDerived>::value));
        ASSERT((true ==
                bslmf::IsAccessibleBaseOf<Base, VirtualDiamondLeft>::value));
        ASSERT((true ==
                bslmf::IsAccessibleBaseOf<Base, VirtualDiamondRight>::value));
        ASSERT((true ==
                bslmf::IsAccessibleBaseOf<Base, VirtualDiamondTop>::value));
        ASSERT((true ==
                bslmf::IsAccessibleBaseOf<Base, AmbiguousLeft>::value));
        ASSERT((true ==
                bslmf::IsAccessibleBaseOf<Base, AmbiguousRight>::value));
#ifndef BSLS_PLATFORM_CMP_IBM
        // There is a bug in the XLC compiler which causes these tests to fail.
        // The problem impacts bslmf_isconvertible, so if a workaround is
        // applied, it would appear in that component instead of this one.
        ASSERT((false ==
                bslmf::IsAccessibleBaseOf<Base, PrivatelyDerived>::value));
        ASSERT((false ==
                bslmf::IsAccessibleBaseOf<Base, ProtectedlyDerived>::value));
        ASSERT(
              (false ==
               bslmf::IsAccessibleBaseOf<Base,
                                         DerivedOfProtectedlyDerived>::value));
        ASSERT((false ==
                bslmf::IsAccessibleBaseOf<Base, AmbiguousTop>::value));
#endif  // BSLS_PLATFORM_CMP_IBM
        ASSERT((false == bslmf::IsAccessibleBaseOf<Base, OtherBase>::value));

        ASSERT((false == bslmf::IsAccessibleBaseOf<Derived, Base>::value));

        ASSERT((false == bslmf::IsAccessibleBaseOf<int, Base>::value));
        ASSERT((false == bslmf::IsAccessibleBaseOf<int, Derived>::value));
        ASSERT((false == bslmf::IsAccessibleBaseOf<int, int>::value));
        ASSERT((false == bslmf::IsAccessibleBaseOf<int, Incomplete>::value));
        ASSERT((false == bslmf::IsAccessibleBaseOf<Base, int>::value));
        ASSERT((false == bslmf::IsAccessibleBaseOf<Derived, int>::value));
        ASSERT((false == bslmf::IsAccessibleBaseOf<int, int>::value));
        ASSERT((false == bslmf::IsAccessibleBaseOf<Incomplete, int>::value));

        ASSERT((false == bslmf::IsAccessibleBaseOf<void, Base>::value));
        ASSERT((false == bslmf::IsAccessibleBaseOf<void, Derived>::value));
        ASSERT((false == bslmf::IsAccessibleBaseOf<void, void>::value));
        ASSERT((false == bslmf::IsAccessibleBaseOf<void, Incomplete>::value));
        ASSERT((false == bslmf::IsAccessibleBaseOf<Base, void>::value));
        ASSERT((false == bslmf::IsAccessibleBaseOf<Derived, void>::value));
        ASSERT((false == bslmf::IsAccessibleBaseOf<void, void>::value));
        ASSERT((false == bslmf::IsAccessibleBaseOf<Incomplete, void>::value));

#ifndef BSLS_PLATFORM_CMP_IBM
        // There is a bug in the XLC compiler which causes these tests to fail.
        // The problem impacts bslmf_isconvertible, so if a workaround is
        // applied, it would appear in that component instead of this one.
        ASSERT(false == PrivatelyDerived{}.IsBaseTest());
        ASSERT(false == ProtectedlyDerived{}.IsBaseTest());
        ASSERT(false == DerivedOfProtectedlyDerived{}.IsBaseTest());

        ASSERT(false == PrivatelyDerived::IsBaseStaticTest());
        ASSERT(false == ProtectedlyDerived::IsBaseStaticTest());
        ASSERT(false == DerivedOfProtectedlyDerived::IsBaseStaticTest());
#endif  // BSLS_PLATFORM_CMP_IBM
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //   Extracted from component header file.
        //
        // Concerns:
        //: 1 The usage example provided in the component header file compiles,
        //:   links, and runs as shown.
        //
        // Plan:
        //: 1 Incorporate usage example from header into test driver, remove
        //:   leading comment characters, and replace 'assert' with 'ASSERT'.
        //:   (C-1)
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose)
            printf("USAGE EXAMPLE\n"
                   "=============\n");
        Example1::example1();
        Example2::example2();
      } break;
      default: {
        fprintf(stderr, "WARNING: CASE `%d' NOT FOUND.\n", test);
        testStatus = -1;
      } break;
    }

    if (testStatus > 0)
        fprintf(stderr, "Error, non-zero test status = %d.\n", testStatus);

    return testStatus;
}

// ----------------------------------------------------------------------------
// Copyright 2022 Bloomberg Finance L.P.
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
