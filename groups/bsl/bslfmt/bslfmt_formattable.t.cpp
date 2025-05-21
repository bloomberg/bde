// bslfmt_formattable.t.cpp                                           -*-C++-*-
#include <bslfmt_formattable.h>

#include <bslfmt_format.h>

#include <bsls_bsltestutil.h>

#include <bslstl_ostringstream.h>
#include <bslstl_string.h>

using namespace BloombergLP;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// Tested is the "trait" `bsl::formattable`
//-----------------------------------------------------------------------------
// [  ] TBD
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 2] USAGE EXAMPLE
//-----------------------------------------------------------------------------

// ============================================================================
//                     STANDARD BSL ASSERT TEST FUNCTION
// ----------------------------------------------------------------------------

namespace {

int testStatus = 0;

void aSsErT(bool condition, const char *message, int line)
{
    if (condition) {
        printf("Error " __FILE__ "(%d): %s    (failed)\n", line, message);
        fflush(stdout);

        if (0 <= testStatus && testStatus <= 100) {
            ++testStatus;
        }
    }
}

}  // close unnamed namespace

// ============================================================================
//               STANDARD BSL TEST DRIVER MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT BSLS_BSLTESTUTIL_ASSERT
#define ASSERTV BSLS_BSLTESTUTIL_ASSERTV

#define LOOP_ASSERT BSLS_BSLTESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLS_BSLTESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLS_BSLTESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLS_BSLTESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLS_BSLTESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLS_BSLTESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLS_BSLTESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLS_BSLTESTUTIL_LOOP6_ASSERT

#define Q BSLS_BSLTESTUTIL_Q    // Quote identifier literally.
#define P BSLS_BSLTESTUTIL_P    // Print identifier and value.
#define P_ BSLS_BSLTESTUTIL_P_  // P(X) without '\n'.
#define T_ BSLS_BSLTESTUTIL_T_  // Print a tab (w/o newline).
#define L_ BSLS_BSLTESTUTIL_L_  // current Line number

#define RUN_EACH_TYPE BSLTF_TEMPLATETESTFACILITY_RUN_EACH_TYPE

// ============================================================================
//                  NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT_SAFE_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(EXPR)
#define ASSERT_SAFE_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPR)
#define ASSERT_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_PASS(EXPR)
#define ASSERT_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_FAIL(EXPR)
#define ASSERT_OPT_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_OPT_PASS(EXPR)
#define ASSERT_OPT_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_OPT_FAIL(EXPR)

#define ASSERT_SAFE_PASS_RAW(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS_RAW(EXPR)
#define ASSERT_SAFE_FAIL_RAW(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL_RAW(EXPR)
#define ASSERT_PASS_RAW(EXPR) BSLS_ASSERTTEST_ASSERT_PASS_RAW(EXPR)
#define ASSERT_FAIL_RAW(EXPR) BSLS_ASSERTTEST_ASSERT_FAIL_RAW(EXPR)
#define ASSERT_OPT_PASS_RAW(EXPR) BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(EXPR)
#define ASSERT_OPT_FAIL_RAW(EXPR) BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(EXPR)

//=============================================================================
//                               TEST TYPES
//-----------------------------------------------------------------------------

struct NotFormattable {};

//=============================================================================
//                              USAGE EXAMPLE
//-----------------------------------------------------------------------------

///Example: Verify the Presence of a bsl::formatter
/// - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose we want to verify in code that a given type may be used with
// `bsl::format` and fall back to using standard streaming if not.
//
// First we create a type that supports streaming but not formatting with
// `bsl::format`:
//
//```
    struct Streamable {};
    std::ostream& operator<<(std::ostream& os, const Streamable&) {
        return os << "Streamable";
    }
//```
//
// Then we demonstrate that this type is not formattable, but an `int` is.
// Since the concept may not exists (older compilers/standards) we need to
// protect the code with the preprocessor:
//```
//#ifdef BSL_FORMATTABLE_DEFINED
//  assert(false == (bsl::formattable<Streamable, char>));
//  assert(true  == (bsl::formattable<int,        char>));
//#endif
//```
//
// Next we create a generic function to convert a value to string,
// center-aligned, and we provide two implementations, the first one for types
// that are formattable in case the concept exists, and just the stream-based
// variation if it does not.
//
//```
    template <class t_TYPE>
    bsl::string
    centeredIn(const t_TYPE& obj, size_t width)
#ifdef BSL_FORMATTABLE_DEFINED
    requires (!bsl::formattable<t_TYPE, char>)
#endif
   {
       bsl::ostringstream os;
       os << obj;
       bsl::string s = os.str();

       width = bsl::max(width, s.length());

       const size_t allPadding = width - s.length();
       s.insert(s.begin(), allPadding / 2, ' ');
       s.append(allPadding - allPadding / 2, ' ');

       return s;
   }
//```
//
// Then, if the concept is present, we define the format-based overload:
//
//```
#ifdef BSL_FORMATTABLE_DEFINED
    template <class t_TYPE>
    bsl::string
    centeredIn(const t_TYPE& obj, size_t width)
    requires (bsl::formattable<t_TYPE, char>)
    {
        return bsl::format("{:^{}}", obj, width);
    }
#endif
//```
//
// Finally, we can call the `centeredIn` function and let concepts select the
// right variation (if concepts are available):
//
//```
//  bsl::string s = centeredIn(Streamable(), 14);
//  assert(s == "  Streamable  ");
//
//  s = centeredIn(42, 8);
//  assert(s == "   42   ");
//```

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char **argv)
{
    const int  test        = argc > 1 ? atoi(argv[1]) : 0;
    const bool verbose     = argc > 2;
    const bool veryVerbose = argc > 3;
    (void) veryVerbose;

    printf("TEST %s CASE %d \n", __FILE__, test);

    switch (test) {  case 0:
      case 2: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //
        // Concern:
        //: 1 Demonstrate the functioning of this component.
        //
        // Plan:
        //: 1 Use test contexts to format a single string.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) puts("\nUSAGE EXAMPLE"
                          "\n=============");

///Example: Verify the Presence of a bsl::formatter
/// - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose we want to verify in code that a given type may be used with
// `bsl::format` and fall back to using standard streaming if not.
//
// First we create a type that supports streaming but not formatting with
// `bsl::format`:
//
//```
//  struct Streamable {};
//  std::ostream& operator<<(std::ostream& os, const Streamable&) {
//      return os << "Streamable";
//  }
//```
//
// Then we demonstrate that this type is not formattable, but an `int` is.
// Since the concept may not exists (older compilers/standards) we need to
// protect the code with the preprocessor:
//```
#ifdef BSL_FORMATTABLE_DEFINED
        ASSERT(false == (bsl::formattable<Streamable, char>));
        ASSERT(true  == (bsl::formattable<int,        char>));
#endif
//```
//
// Next we create a generic function to convert a value to string,
// center-aligned, and we provide two implementations, the first one for types
// that are formattable in case the concept exists, and just the stream-based
// variation if it does not.
//
//```
//  template <class t_TYPE>
//  bsl::string
//  centeredIn(const t_TYPE& obj, size_t width)
//#ifdef BSL_FORMATTABLE_DEFINED
//  requires (!bsl::formattable<t_TYPE, char>)
//#endif
// {
//     bsl::ostringstream os;
//     os << obj;
//     bsl::string s = os.str();
//
//     width = bsl::max(width, s.length());
//
//     const size_t allPadding = width - s.length();
//     s.insert(s.begin(), allPadding / 2, ' ');
//     s.append(allPadding - allPadding / 2, ' ');
//
//     return s;
// }
//```
//
// Then, if the concept is present, we define the format-based overload:
//
//```
//#ifdef BSL_FORMATTABLE_DEFINED
//  template <class t_TYPE>
//  bsl::string
//  centeredIn(const t_TYPE& obj, size_t width)
//  requires (bsl::formattable<t_TYPE, char>)
//  {
//      return bsl::format("{:^{}}", obj, width);
//  }
//#endif
//```
//
// Finally, we can call the `centeredIn` function and let concepts select the
// right variation (if concepts are available):
//
//```
        bsl::string s = centeredIn(Streamable(), 14);
        ASSERT(s == "  Streamable  ");

        s = centeredIn(42, 8);
        ASSERT(s == "   42   ");
//```
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //
        // Concerns:
        //: 1 That basic functionality appears to work as advertised before
        //:   before beginning testing in earnest:
        //:   - default and copy constructors
        //:   - assignment operator
        //:   - primary manipulators, basic accessors
        //:   - 'operator==', 'operator!='
        //
        // Plan:
        //: 1 Test all public methods mentioned in concerns.  (C-1)
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------
        if (verbose) puts("\nBREATHING TEST"
                          "\n==============");

#ifdef BSL_FORMATTABLE_DEFINED
        ASSERT((bsl::formattable<NotFormattable, char> == false));

        ASSERT((bsl::formattable<int, char> == true));
#else
        ASSERT(true);  // Avoid unused warning
#endif  // BSL_FORMATTABLE_DEFINED
      } break;
      default: {
        printf("WARNING: CASE `%d' NOT FOUND.\n", test);
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        printf("Error, non-zero test status = %d .\n", testStatus);
    }
    return testStatus;
}

// ----------------------------------------------------------------------------
// Copyright 2025 Bloomberg Finance L.P.
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
