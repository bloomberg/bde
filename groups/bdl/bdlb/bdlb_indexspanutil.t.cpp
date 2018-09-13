// bdlb_indexspanutil.t.cpp                                           -*-C++-*-
#include <bdlb_indexspanutil.h>

#include <bdlb_indexspan.h>

#include <bslim_testutil.h>

#include <bsls_asserttest.h>
#include <bsls_buildtarget.h>

#include <bsl_cstdlib.h>
#include <bsl_cstring.h>
#include <bsl_iostream.h>
#include <bsl_string.h>    // For the usage example

using namespace BloombergLP;
using namespace bsl;

// ============================================================================
//                             TEST PLAN
// ----------------------------------------------------------------------------
//                              Overview
//                              --------
// This component is a utility operating on 'bldb::IndexSpan' objects.
// ----------------------------------------------------------------------------
// CLASS METHODS
// [2] IndexSpan shrink(original, shrinkBegin, shrinkEnd);
// ----------------------------------------------------------------------------
// [1] BREATHING TEST
// [3] USAGE EXAMPLE

// ============================================================================
//                     STANDARD BDE ASSERT TEST FUNCTION
// ----------------------------------------------------------------------------

namespace {

int testStatus = 0;

void aSsErT(bool condition, const char *message, int line)
{
    if (condition) {
        cout << "Error " __FILE__ "(" << line << "): " << message
             << "    (failed)" << endl;

        if (0 <= testStatus && testStatus <= 100) {
            ++testStatus;
        }
    }
}

}  // close unnamed namespace

// ============================================================================
//               STANDARD BDE TEST DRIVER MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT       BSLIM_TESTUTIL_ASSERT
#define ASSERTV      BSLIM_TESTUTIL_ASSERTV

#define LOOP_ASSERT  BSLIM_TESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLIM_TESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLIM_TESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLIM_TESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLIM_TESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLIM_TESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLIM_TESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLIM_TESTUTIL_LOOP6_ASSERT

#define Q            BSLIM_TESTUTIL_Q   // Quote identifier literally.
#define P            BSLIM_TESTUTIL_P   // Print identifier and value.
#define P_           BSLIM_TESTUTIL_P_  // P(X) without '\n'.
#define T_           BSLIM_TESTUTIL_T_  // Print a tab (w/o newline).
#define L_           BSLIM_TESTUTIL_L_  // current Line number

// ============================================================================
//                  NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT_SAFE_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(EXPR)
#define ASSERT_SAFE_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPR)
#define ASSERT_PASS(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS(EXPR)
#define ASSERT_FAIL(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL(EXPR)
#define ASSERT_OPT_PASS(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS(EXPR)
#define ASSERT_OPT_FAIL(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL(EXPR)

//=============================================================================
//                            TYPE DEFINITIONS
//-----------------------------------------------------------------------------

typedef bdlb::IndexSpanUtil Util;

//=============================================================================
//                              USAGE EXAMPLE
//-----------------------------------------------------------------------------

///Usage
///-----
// This section illustrates intended use of this component.
//
///Example1: Taking a IPv6 address out of a URI
/// - - - - - - - - - - - - - - - - - - - - - -
// Suppose we a class that stores a parsed URL using a string to store the full
// URL and 'IndexSpan' objects to describe the individual parts of the URL and.
// we want to add accessors that handle the case when the host part of the URL
// is an IPv6 address, such as "http://[ff:fe:9]/index.html".  As observed, an
// IPv6 address is indicated by the '[' and ']' characters (the URL is ill
// formed if the closing ']' is not present).  We want to implement two
// methods, one to query if the host part of the URL is IPv6 ('isIPv6Host') and
// another to get the IPv6 address (the part without the square brackets) if
// the host is actually an IPv6 address ('getIPv6Host').
//
// First let us create a 'ParsedUrl' class.  For brevity, the class has only
// those parts that are needed to implement 'isIPv6Host' and 'getIPv6Host'.
//..
    class ParsedUrl {
      private:
        // DATA
        bsl::string     d_url;
        bdlb::IndexSpan d_host;

      public:
        // CREATORS
        ParsedUrl(const bslstl::StringRef& url, bdlb::IndexSpan host)
            // Create a 'ParsedUrl' from the specified 'url', and 'host'.
        : d_url(url)
        , d_host(host)
        {
        }

        // ACCESSORS
        bool isIPv6Host() const;
            // Return 'true' if the host part represents an IPv6 address and
            // 'false' otherwise.

        bslstl::StringRef getIPv6Host() const;
            // Return a string reference to the IPv6 address in the host part
            // of this URL.  The behavior is undefined unless
            // 'isIPv6Host() == true' for this object.
    };
//..
// Next, we implement 'isIPv6Host'.
//..
    bool ParsedUrl::isIPv6Host() const
    {
        return !d_host.isEmpty() && '[' == d_url[d_host.position()];
    }
//..
//  Then, to make the accessors simple (and readable), we implement a helper
//  function that creates a 'StringRef' from a 'StringRef' and an 'IndexSpan'.
//  (Don't do this in real code, use 'IndexSpanStringUtil::bind' that is
//  levelized above this component - so we cannot use it here.)
//..
    bslstl::StringRef bindSpan(const bslstl::StringRef& full,
                               const bdlb::IndexSpan&   part)
        // Return a string reference to the substring of the specified 'full'
        // thing defined by the specified 'part'.
    {
        BSLS_ASSERT(part.position() <= full.length());
        BSLS_ASSERT(part.position() + part.length() <= full.length());

        return bslstl::StringRef(full.data() + part.position(), part.length());
    }
//..
// Next, we implement 'getIPv6Host' using 'bdlb::IndexSpanUtil::shrink'.
//..
    bslstl::StringRef ParsedUrl::getIPv6Host() const
    {
        BSLS_ASSERT(isIPv6Host());

        return bindSpan(d_url, bdlb::IndexSpanUtil::shrink(d_host, 1, 1));
    }
//..

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int  test            = argc > 1 ? bsl::atoi(argv[1]) : 0;
    bool verbose         = argc > 2;
    bool veryVerbose     = argc > 3;
    bool veryVeryVerbose = argc > 4; (void)veryVeryVerbose;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;;

    switch (test)  { case 0:
      case 3: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //   Extracted from component header file.
        //
        // Concerns:
        //: 1 The usage example provided in the component header file compiles,
        //:   links, and runs as shown.
        //
        // Plan:
        //: 1 Incorporate usage example from header into test driver, replace
        //:   leading comment characters with spaces, replace 'assert' with
        //:   'ASSERT', and insert 'if (veryVerbose)' before all output
        //:   operations.  (C-1)
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------
        if (verbose) cout << "\nUSAGE EXAMPLE"
                             "\n=============\n";

// See the rest of the code just before the 'main' function.
//
// Finally, we verify the two methods with URLs.
//..
    ParsedUrl pu1("https://host/path/", bdlb::IndexSpan(8, 4));
    ASSERT(false == pu1.isIPv6Host());

    ParsedUrl pu2("https://[12:3:fe:9]/path/", bdlb::IndexSpan(8, 11));
    ASSERT(true == pu2.isIPv6Host());
    ASSERT("12:3:fe:9" == pu2.getIPv6Host());
//..
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING SHRINK
        //
        // Concerns:
        //: 1 Shrinking from the beginning increases position and decreases
        //:   length.
        //:
        //: 2 Shrinking from the end decreases length only.
        //:
        //: 2 Shrinking beyond the size 'BSLS_ASSERT's.
        //
        // Plan:
        //: 1 Table based testing.
        //
        // Testing:
        //   IndexSpan shrink(original, shrinkBegin, shrinkEnd);
        // --------------------------------------------------------------------
        if (verbose) cout << "\nTESTING SHRINK"
                             "\n==============\n";

        static struct TestData {
            long long   d_line;
            bsl::size_t d_pos;
            bsl::size_t d_len;
            bsl::size_t d_beginShrink;
            bsl::size_t d_endShrink;
            char        d_bad;  // 'X' if the shrink is too big
            bsl::size_t d_expectedPos;
            bsl::size_t d_expectedLen;
        } k_DATA[] = {
            //    pos len beg end bad  pos len
            //    --- --- --- --- ---- --- ---
            { L_,  0,  0,  0,  0, ' ',  0,  0 },
            { L_,  0,  0,  1,  0, 'X',  0,  0 },
            { L_,  0,  0,  0,  1, 'X',  0,  0 },
            { L_,  0,  0,  1,  1, 'X',  0,  0 },
            { L_,  0,  1,  1,  0, ' ',  1,  0 },
            { L_,  0,  1,  0,  1, ' ',  0,  0 },
            { L_,  0,  2,  1,  0, ' ',  1,  1 },
            { L_,  0,  2,  0,  1, ' ',  0,  1 },
            { L_,  1,  1,  1,  0, ' ',  2,  0 },
            { L_,  1,  1,  0,  1, ' ',  1,  0 },
        };

        static const bsl::size_t k_NUM_TESTS = sizeof k_DATA / sizeof *k_DATA;

        for (bsl::size_t i = 0; i < k_NUM_TESTS; ++i) {
            const TestData&   k_TEST = k_DATA[i];
            const long long   k_LINE         = k_TEST.d_line;
            const bsl::size_t k_POS          = k_TEST.d_pos;
            const bsl::size_t k_LEN          = k_TEST.d_len;
            const bsl::size_t k_BEGIN_SHRINK = k_TEST.d_beginShrink;
            const bsl::size_t k_END_SHRINK   = k_TEST.d_endShrink;
            const char        k_BAD          = k_TEST.d_bad;
            const bsl::size_t k_EXPECTED_POS = k_TEST.d_expectedPos;
            const bsl::size_t k_EXPECTED_LEN = k_TEST.d_expectedLen;

            if (veryVerbose) {
                P_(k_LINE) P_(k_POS) P_(k_LEN)
                    P_(k_BEGIN_SHRINK) P(k_END_SHRINK)
            }

            if ('X' != k_BAD) {
                const bdlb::IndexSpan X(k_POS, k_LEN);
                const bdlb::IndexSpan R = Util::shrink(X,
                                                       k_BEGIN_SHRINK,
                                                       k_END_SHRINK);
                ASSERTV(k_LINE, R.position(), k_EXPECTED_POS,
                        k_EXPECTED_POS == R.position());
                ASSERTV(k_LINE, R.length(),   k_EXPECTED_LEN,
                        k_EXPECTED_LEN == R.length());
            }
            else {
#ifdef BDE_BUILD_TARGET_EXC
                bsls::AssertTestHandlerGuard g; (void)g;

                const bdlb::IndexSpan X(k_POS, k_LEN);

                ASSERT_FAIL(Util::shrink(X, k_BEGIN_SHRINK, k_END_SHRINK));
#endif
            }
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //   This case exercises (but does not fully test) basic functionality.
        //
        // Concerns:
        //: 1 The class is sufficiently functional to enable comprehensive
        //:   testing in subsequent test cases.
        //
        // Plan:
        //: 1 Call the utility functions to verify their existence and basics.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------
        if (verbose) cout << "\nBREATHING TEST"
                             "\n==============\n";

        const bdlb::IndexSpan span(1, 2);
        ASSERT(bdlb::IndexSpan(2, 0) == Util::shrink(span, 1, 1));

      } break;
      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      } break;
    }
    if (testStatus > 0) {
        cerr << "Error, non-zero test status = " << testStatus << "." << endl;
    }
    return testStatus;
}

// ----------------------------------------------------------------------------
// Copyright 2018 Bloomberg Finance L.P.
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
