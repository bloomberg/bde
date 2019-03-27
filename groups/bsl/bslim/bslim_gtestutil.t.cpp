// bslim_gtestutil.t.cpp                                              -*-C++-*-
#include <bslim_gtestutil.h>

#include <bslim_testutil.h>

#include <bslma_testallocator.h>

#include <bsl_cstdio.h>
#include <bsl_cstdlib.h>
#include <bsl_cstring.h>
#include <bsl_iostream.h>
#include <bsl_limits.h>
#include <bsl_sstream.h>
#include <bsl_string.h>

// Do not 'using' any namespaces.  We want to verify that everything works from
// the global namespace.

using bsl::cout;
using bsl::cerr;
using bsl::flush;
using bsl::endl;

// ============================================================================
//                             TEST PLAN
// ----------------------------------------------------------------------------
//                             Overview
//                             --------
// The component under test consists of a series of static member functions
// that provide facilities for debugging BDE with gtest.
// ----------------------------------------------------------------------------
// [ 2] void PrintTo(const bslstl::StringRef& value, ostream *stream);
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 3] USAGE EXAMPLE
// ----------------------------------------------------------------------------

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

#define Q            BSLIM_TESTUTIL_Q     // Quote identifier literally.
#define P            BSLIM_TESTUTIL_P     // Print identifier and value.
#define P_           BSLIM_TESTUTIL_P_    // P(X) without '\n'.
#define T_           BSLIM_TESTUTIL_T_    // Print a tab (w/o newline).
#define L_           BSLIM_TESTUTIL_L_    // current Line number

// ============================================================================
//            GLOBAL TYPES, CONSTANTS, AND VARIABLES FOR TESTING
// ----------------------------------------------------------------------------

// ============================================================================
//            SIMULATE COMPETING 'PrintTo' DECLARATIONS IN GTEST
// ----------------------------------------------------------------------------

template <typename T>
void PrintTo(const T& , ::native_std::ostream* )
{
    ASSERT(0);
}

void PrintTo(unsigned char, ::native_std::ostream* )
{
    ASSERT(0);
}

void PrintTo(signed char , ::native_std::ostream* )
{
    ASSERT(0);
}

inline
void PrintTo(char , ::native_std::ostream* )
{
    ASSERT(0);
}

inline
void PrintTo(bool , ::native_std::ostream* )
{
    ASSERT(0);
}

void PrintTo(wchar_t , ::native_std::ostream* )
{
    ASSERT(0);
}

void PrintTo(const char* , ::native_std::ostream* )
{
    ASSERT(0);
}

inline
void PrintTo(char* , ::native_std::ostream* )
{
    ASSERT(0);
}

inline
void PrintTo(const signed char* , ::native_std::ostream* )
{
    ASSERT(0);
}

inline
void PrintTo(signed char* , ::native_std::ostream* )
{
    ASSERT(0);
}

inline
void PrintTo(const unsigned char* , ::native_std::ostream* )
{
    ASSERT(0);
}

inline
void PrintTo(unsigned char* , ::native_std::ostream* )
{
    ASSERT(0);
}

void PrintTo(const wchar_t* , ::native_std::ostream* )
{
    ASSERT(0);
}

inline
void PrintTo(wchar_t* , ::native_std::ostream* )
{
    ASSERT(0);
}

class string;
inline
void PrintTo(const ::string& , ::native_std::ostream* )
{
    ASSERT(0);
}

inline
void PrintTo(const ::native_std::string& , ::native_std::ostream* )
{
    ASSERT(0);
}

class wstring;
inline
void PrintTo(const ::wstring& , ::native_std::ostream* )
{
    ASSERT(0);
}

inline
void PrintTo(const ::native_std::wstring& , ::native_std::ostream* )
{
    ASSERT(0);
}

// ============================================================================
//                               MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int                 test = (argc > 1) ? bsl::atoi(argv[1]) : 1;
    bool             verbose = (argc > 2); (void)verbose;
    bool         veryVerbose = (argc > 3); (void)veryVerbose;
    bool     veryVeryVerbose = (argc > 4); (void)veryVeryVerbose;
    bool veryVeryVeryVerbose = (argc > 5); (void)veryVeryVeryVerbose;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    BloombergLP::bslma::TestAllocator ta("test", veryVeryVeryVerbose);

    switch (test) { case 0:
      case 3: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //
        // Concerns:
        //: 1 Demonstrate the usage of this component.
        //
        // Plan:
        //: 1 Using 'PrintTo' to output a string.
        // --------------------------------------------------------------------

        if (verbose) cout << "USAGE EXAMPLE\n"
                             "=============\n";

///Usage
///-----
// Suppose we have a string 'str' that we want to output:
//..
    bsl::string str =
                   "No matter where you go, There you are! -- Buckaroo Banzai";
//..
// Call 'PrintTo', passing the string and a pointer to a 'bsl::ostream':
//..
    PrintTo(str, &cout);
    cout << endl;
//..
// Which results in the string being streamed to standard output, surrounded by
// double quotes:
//..
//  "No matter where you go, There you are! -- Buckaroo Banzai"
//..
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING 'PrintTo' FOR 'StringRef'
        //
        // Concerns:
        //: 1 The 'PrintTo' correctly writes an empty 'StringRef' object's
        //:   value to the stream.
        //:
        //: 2 The 'PrintTo' correctly writes 'StringRef' object's value
        //:   containing embedded null character to the stream.
        //:
        //: 3 The 'PrintTo' correctly writes any non-empty 'StringRef' object's
        //:   value to the stream.
        //
        // Plan:
        //: 1 Create several 'StringRef' objects, having different values.
        //:   Print them to the stream using 'PrintTo' function and verify the
        //:   result.  (C-1..3)
        //
        // Testing:
        //   void PrintTo(const bslstl::StringRef& value, ostream *stream);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING 'PrintTo' FOR 'StringRef'"
                          << "\n=================================\n";

        typedef BloombergLP::bslstl::StringRef StringRef;

        const char        *EMPTY_STRING = "";
        const char        *LONG_STRING  = "123456789012345678901234567890"
                                          "123456789012345678901234567890"
                                          "123456789012345678901234567890";

        // Non empty string: "12345\0abcde".
        // Note the null value character in the middle.

        const char         NON_EMPTY_STRING[] =
                                 {48, 49, 50, 51, 52, 0, 97, 98, 99, 100, 101};
        const bsl::size_t  NON_EMPTY_STRING_LENGTH =
                          sizeof(NON_EMPTY_STRING) / sizeof(*NON_EMPTY_STRING);
        const char        *NON_EMPTY_STRING_END =
                                    NON_EMPTY_STRING + NON_EMPTY_STRING_LENGTH;

        if (veryVerbose) cout << "Testing default-constructed 'StringRef'."
                              << endl;
        {
            StringRef        mDES;
            const StringRef& DES = mDES;

            bsl::ostringstream ossDES;
            const bsl::string  EXPECTED_DES("\"\"");

            bsl::PrintTo(DES, &ossDES);

            ASSERTV(ossDES.good());
            ASSERTV(EXPECTED_DES, ossDES.str(),
                    EXPECTED_DES == ossDES.str());
        }

        if (veryVerbose) cout << "Testing empty 'StringRef'." << endl;
        {
            StringRef        mES(EMPTY_STRING);
            const StringRef& ES = mES;

            bsl::ostringstream ossES;
            const bsl::string  EXPECTED_ES("\"\"");

            bsl::PrintTo(ES, &ossES);

            ASSERTV(ossES.good());
            ASSERTV(EXPECTED_ES, ossES.str(), EXPECTED_ES == ossES.str());
        }

        if (veryVerbose) cout << "Testing non-empty 'StringRef'." << endl;
        {
            for (const char *begin = NON_EMPTY_STRING;
                 begin != NON_EMPTY_STRING_END;
                 ++begin) {
                for (const char *end = begin;
                     end != NON_EMPTY_STRING_END + 1;
                     ++end) {
                    StringRef        mNES(begin, end);
                    const StringRef& NES = mNES;

                    bsl::ostringstream ossNES;
                    const bsl::string  EXPECTED_NES = bsl::string("\"")
                                                    + bsl::string(begin, end)
                                                    + bsl::string("\"");
                    bsl::PrintTo(NES, &ossNES);

                    ASSERTV(ossNES.good());
                    ASSERTV(EXPECTED_NES, ossNES.str(),
                            EXPECTED_NES == ossNES.str());
                }
            }
        }

        if (veryVerbose) cout << "Testing long 'StringRef'." << endl;
        {
            StringRef        mLS(LONG_STRING);
            const StringRef& LS = mLS;

            bsl::ostringstream ossLS;
            const bsl::string  EXPECTED_LS = bsl::string("\"")
                                           + bsl::string(LONG_STRING)
                                           + bsl::string("\"");

            bsl::PrintTo(LS, &ossLS);

            ASSERTV(ossLS.good());
            ASSERTV(EXPECTED_LS, ossLS.str(), EXPECTED_LS == ossLS.str());
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //   This case exercises (but does not fully test) basic functionality.
        //
        // Concerns:
        //: 1 That the function can stream a string.
        //:
        //: 2 If the stream is invalid, the function will not modify the
        //:   stream.
        //
        // Plan:
        //: 1 Stream a couple of strings to an 'ostringtream' and observe the
        //:   result.
        //:
        //: 2 Set the state of the 'ostringstream' to invalid, call the
        //:   function again, and observe that the stream has not been
        //:   modified.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) cout << "\nBREATHING TEST"
                             "\n==============\n";

        bsl::ostringstream oss(&ta);
        bsl::string s("hello, world.\n", &ta);

        PrintTo(s, &oss);
        ASSERT(oss.good());
        bsl::string expected = '"' + s + '"';
        ASSERTV(oss.str(), expected, oss.str() == expected);

        oss.str("");
        s.assign("The rain in Spain falls mainly in the plain.\n");
        PrintTo(s, &oss);
        ASSERT(oss.good());
        expected = '"' + s + '"';
        ASSERTV(oss.str(), expected, oss.str() == expected);

        oss.str("");
        oss << "woof";
        ASSERTV(oss.str(), oss.str() == "woof");
        ASSERT(oss.good());
        oss.setstate(bsl::ios_base::failbit);
        ASSERT(!oss.good());
        PrintTo(s, &oss);
        ASSERTV(oss.str(), oss.str() == "woof");
        ASSERT(!oss.good());

        oss.clear();
        oss.str("");
        bsl::wstring ws(&ta);
        ws.push_back(wchar_t(0x80));
        ws.push_back(wchar_t(0xabcd));
        ws.push_back('m');
        ws.push_back('e');
        ws.push_back('o');
        ws.push_back('w');
        ws.push_back(wchar_t(0xf1d9));
        ws.push_back(wchar_t(7));        // bell (non-printable ascii)

        PrintTo(ws, &oss);

        ASSERTV(sizeof(wchar_t), 2 == sizeof(wchar_t) || 4 == sizeof(wchar_t));

        const char *exp = 2 == sizeof(wchar_t)
                      ? "\"\\x0080\\xabcdmeow\\xf1d9\\x0007\""
                      : "\"\\x00000080\\x0000abcdmeow\\x0000f1d9\\x00000007\"";
        ASSERTV(oss.str(), exp, oss.str() == exp);

        if (verbose) { P_(sizeof(wchar_t));    P(oss.str()); }
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
