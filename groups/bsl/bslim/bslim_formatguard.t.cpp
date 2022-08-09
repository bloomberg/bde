// bslim_formatguard.t.cpp                                            -*-C++-*-
#include <bslim_formatguard.h>

#include <bsls_assert.h>
#include <bslim_testutil.h>  // for testing only
#include <bsls_platform.h>

#include <bsl_algorithm.h>
#include <bsl_cstdlib.h>
#include <bsl_cstring.h>
#include <bsl_iomanip.h>
#include <bsl_iostream.h>
#include <bsl_sstream.h>

using namespace BloombergLP;
using bsl::cout;
using bsl::cerr;
using bsl::endl;
using bsl::flush;

//=============================================================================
//                                 TEST PLAN
//-----------------------------------------------------------------------------
//                                 Overview
//                                 --------
// [ 4] FormatGuard(bsl::basic_ostream<unsigned> *);
// [ 3] FormatGuard(bsl::basic_ostream<wchar_t> *);
// [ 2] FormatGuard(bsl::basic_ostream<CHAR_TYPE, CHAR_TRAITS> *);
// [ 2] ~FormatGuard();
//-----------------------------------------------------------------------------
// [ 6] USAGE EXAMPLE
// [ 1] BREATHING TEST

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

#define ASSERT       BSLIM_TESTUTIL_ASSERT
#define ASSERTV      BSLIM_TESTUTIL_ASSERTV

#define Q            BSLIM_TESTUTIL_Q   // Quote identifier literally.
#define P            BSLIM_TESTUTIL_P   // Print identifier and value.
#define P_           BSLIM_TESTUTIL_P_  // P(X) without '\n'.
#define T_           BSLIM_TESTUTIL_T_  // Print a tab (w/o newline).
#define L_           BSLIM_TESTUTIL_L_  // current Line number
                                        //
#define PW(X)                                                                 \
    bsl::wcout << #X " = " << (X) << bsl::endl;
    // Print identifier and its value.

#define PW_(X)                                                                \
    bsl::wcout << #X " = " << (X) << ", " << bsl::flush;
    // 'P(X)' without '\n'

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

typedef bslim::FormatGuard Guard;

//=============================================================================
//                        GLOBAL CLASSES FOR TESTING
//-----------------------------------------------------------------------------

//=============================================================================
//                        GLOBAL FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

namespace {
namespace u {

template <class CHAR_TYPE>
bsl::size_t countChar(const bsl::basic_string<CHAR_TYPE>& str, CHAR_TYPE c)
    // Return the number of times the specified character 'c' occurs in the
    // specified string 'str'.
{
    return bsl::count(str.begin(), str.end(), c);
}

}  // close namespace u
}  // close unnamed namespace

//=============================================================================
//                              USAGE EXAMPLE
//-----------------------------------------------------------------------------

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int                test = argc > 1 ? bsl::atoi(argv[1]) : 0;
    int             verbose = argc > 2;    (void)             verbose;
    int         veryVerbose = argc > 3;    (void)         veryVerbose;
    int     veryVeryVerbose = argc > 4;    (void)     veryVeryVerbose;
    int veryVeryVeryVerbose = argc > 5;    (void) veryVeryVeryVerbose;

    cout << "TEST " << __FILE__ << " CASE " << test << "\n";

    switch (test) { case 0:  // Zero is always the leading case.
      case 6: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //
        // Concern:
        //: 1 That the usage example compiles and works properly.
        //
        // Plan:
        //: 1 Encode the usage example and run it.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << "USAGE EXAMPLE\n"
                             "=============\n";

//
///Usage
///-----
// In the following example we illustrate the usage of 'FormatGuard'.
//
///Example 1: Saving Stream State to be Restored Later:
///- - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose we want to do some output to a stream for which we must change the
// state of the stream.
//
// First, we declare our stream:
//..
    bsl::ostringstream oss;
//..
// Then, we use a 'FormatGuard' to save the state of 'oss' before we
// reconfigure it, so that when the 'FormatGuard' is destroyed it will resotre
// 'oss' to its original state.
//..
    {
        bslim::FormatGuard guard(&oss);
//..
// Then, we reconfigure out stream and do some output:
//..
        oss << "First line in hex: " << bsl::showbase << bsl::hex << 80 <<
                                                                          endl;
//..
// Next, we leave the block and the destructor of 'guard' will restore 'oss'
// to its original configuration:
//..
    }
//..
// Now, we do another line of output:
//..
    oss << "Second line in decimal: " << 123 << endl;
//..
// Finally, we observe that our guarded output was in hex, as desired, and the
// output afterward was in decimal, as desired:
//..
    ASSERT(oss.str() == "First line in hex: 0x50\n"
                        "Second line in decimal: 123\n");
//..
      } break;
      case 5: {
#if 0
        // The component does not yet support istreams

        // --------------------------------------------------------------------
        // INPUT STREAM
        //
        // Concern:
        //: 1 That the guard can work with a 'basic_istream'.
        //
        // Plan:
        //: 1 Configure a 'basic_istream', save the state, change the state,
        //:   restore the state and observe that it was properly saved.
        //
        // Testing:
        //   FormatGuard(bsl::basic_istream<CHAR_TYPE, CHAR_TRAITS> *);
        // --------------------------------------------------------------------

        bsl::istringstream iss("10 10 10");

        int a, b, c;

        iss >> bsl::hex >> a;
        ASSERT(16 == a);

        {
            Guard guard(&iss);

            iss >> bsl::dec >> b;
            ASSERT(10 == b);
        }

        iss >> c;
        ASSERT(16 == c);
#endif
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // VERY WIDE TYPE
        //
        // Concern:
        //: 1 That the guard can work with a stream whose 'CHAR_TYPE' is some
        //:   type other than 'char' or 'wchar_t'.
        //
        // Plan:
        //: 1 Conduct some experiments on 'basic_ostringstream<unsigned>'.
        //
        // Testing:
        //   FormatGuard(bsl::basic_ostream<unsigned> *);
        // --------------------------------------------------------------------

#if !defined(BSLS_PLATFORM_OS_SOLARIS) || !defined(BSLS_PLATFORM_CMP_SUN)
        if (verbose) cout << "TESTING VERY WIDE TYPE - DISABLED\n"
                             "=================================\n";
#else
        if (verbose) cout << "TESTING VERY WIDE TYPE\n"
                             "======================\n";


        // It turns out that you can't do much to a 'basic_ostringstream<TYPE>'
        // unless the locales are set up just right for 'TYPE', which is not
        // the case for 'basic_ostringstream<unsigned>' except on Solaris with
        // the 'cc' compiler.

        bsl::basic_ostringstream<unsigned> oss;

        const unsigned zero = 0;

        const unsigned fillChar = 1 << 20;
        oss.fill(fillChar);

        if (veryVeryVeryVerbose) P(oss.fill());

        oss.width(5);

        oss << 12;

        unsigned EXP[] = { fillChar, fillChar, fillChar, '1', '2' };
        const bsl::basic_string<unsigned>& out12 = oss.str();

        ASSERTV(out12.length(), out12.length() == 5);
        for (int ii = 0; ii < 5; ++ii) {
            ASSERTV(ii, EXP[ii], out12.c_str()[ii],
                                                EXP[ii] == out12.c_str()[ii]);
        }

        {
            Guard guard(&oss);

            oss.fill('_');

            oss.str(&zero);

            unsigned EXPP[] = { '_', '_', '5', '0' };

            oss.width(4);
            oss << bsl::hex << 80;

            const bsl::basic_string<unsigned>& out50 = oss.str();
            ASSERTV(out50.length(), out50.length() == 4);
            for (int ii = 0; ii < 4; ++ii) {
                ASSERTV(ii, EXPP[ii], out50.c_str()[ii],
                                                EXPP[ii] == out50.c_str()[ii]);
            }
        }

        ASSERT(fillChar == oss.fill());
        ASSERT(bsl::ios_base::dec == (oss.flags() & bsl::ios_base::basefield));
#endif
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // WIDE STRINGSTREAM TEST
        //
        // Concerns:
        //: 1 Ensure the guard will work properly with a wide string stream.
        //
        // Plan:
        //: 1 Create a wide string stream.
        //:
        //: 2 Save its state with a guard.
        //:
        //: 3 Change the stream's state.
        //:
        //: 4 Exit the guarded block.
        //:
        //: 5 Observe that the stream's state is restored.
        //
        // Testing:
        //   FormatGuard(bsl::basic_ostream<wchar_t> *);
        // --------------------------------------------------------------------

        if (verbose) cout << "WIDE STRINGSTREAM TEST\n"
                             "======================\n";
        using bsl::ios_base;

        const double oneSeventh = 1.0 / 7;
        const bsl::size_t npos = bsl::string::npos;

        bsl::basic_ostringstream<wchar_t> owss;

        ASSERT(0 == (owss.flags() & ios_base::floatfield));
        ASSERT(ios_base::dec   == (owss.flags() & ios_base::basefield));
        ASSERT(' ' == owss.fill());

        owss << oneSeventh;
        const bsl::wstring& outputFixedA = owss.str();
        ASSERT(outputFixedA.length() < 10);
        const wchar_t sciChars[] = { 'e', 'E', '+', '-', 0 };
        ASSERT(npos == outputFixedA.find_first_of(sciChars));

        owss.str(L"");

        owss << bsl::setw(3) << 64;
        const wchar_t EXP64[] = { ' ', '6', '4', 0 };
        ASSERT(EXP64 == owss.str());

        {
            Guard guard(&owss);

            owss.str(L"");

            const wchar_t fillChar = 1 << 10;
            owss.fill(fillChar);

            owss << bsl::hex << bsl::setw(3) << bsl::left;
            owss << 64;
            const wchar_t EXP[] = { '4', '0', fillChar, 0 };;
            if (veryVeryVerbose) {
                PW_(owss.str()); PW(EXP);
            }
            ASSERT(owss.str() == EXP);

            owss.str(L"");

            owss << bsl::setprecision(15) << oneSeventh;
            const bsl::wstring& outputFixedB = owss.str();
            ASSERT(15 < outputFixedB.length());
            ASSERT(npos == outputFixedB.find_first_of(sciChars));
            ASSERT(outputFixedA != outputFixedB);

            owss.str(L"");

            owss << bsl::scientific << oneSeventh;
            const bsl::wstring& outputSci = owss.str();
            const wchar_t firstChar = outputSci[0];
            ASSERT(15 < outputSci.length());
            ASSERT(1 == u::countChar(outputSci, wchar_t('.')));
            ASSERT(1 == u::countChar(outputSci, wchar_t('e')));
            ASSERT('1' == firstChar || '0' == firstChar || '.' == firstChar);
            ASSERT((wchar_t('1') == firstChar)
                               ? (1 == u::countChar(outputSci, wchar_t('-')))
                               : (1 == u::countChar(outputSci, wchar_t('+'))));
        }

        ASSERT(ios_base::dec == (owss.flags() & ios_base::basefield));
        ASSERT(' ' == owss.fill());

        owss.str(L"");
        owss << bsl::setw(3) << 64;
        ASSERT(EXP64 == owss.str());

        owss.str(L"");
        owss << oneSeventh;
        const bsl::wstring& outputFixedC = owss.str();
        ASSERT(outputFixedC.length() < 10);
        ASSERT(npos == outputFixedC.find_first_of(sciChars));
        ASSERT(outputFixedA == outputFixedC);
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // NORMAL STREAM TEST
        //
        // Concern:
        //: 1 Test various different forms of stream configuration saved by
        //:   guards.
        //
        // Plan:
        //: 1 Create an 'ostringstream', configure it to a non-default
        //:   state in various ways.
        //:
        //: 2 Do a few outputs with it and save references to 'string's of the
        //:   outputs.
        //:
        //: 3 Iterate through a loop, within the loop:
        //:   o Save the stream state with a guard
        //:
        //:   o change the stream state
        //:
        //:   o do some output relying on the different stream state.
        //:
        //:   o exit the guarded block
        //:
        //:   o repeat outputs identical to the ones done before the loop and
        //:     observe the output is identical.
        //
        // Testing:
        //   FormatGuard(bsl::basic_ostream<CHAR_TYPE, CHAR_TRAITS> *);
        //   ~FormatGuard();
        // --------------------------------------------------------------------

        if (verbose) cout << "NORMAL STREAM TEST\n"
                             "==================\n";

        const bsl::size_t npos = bsl::string::npos;

        const double oneSeventh = 1.0 / 7;

        bsl::ostringstream oss;
        oss.precision(4);
        oss << bsl::left;

        oss.str("");

        oss.fill('_');

        oss.str("");
        oss << bsl::setw(6) << 123;
        const bsl::string& origDec = oss.str();
        if (verbose) P(origDec);
        ASSERT("123___" == origDec);

        oss.str("");
        oss << oneSeventh;
        const bsl::string& origFixed = oss.str();
        if (verbose) P(origFixed);
        const bsl::size_t defaultLen = origFixed.length();
        ASSERT(bsl::strchr("0.", origFixed[0]));
        ASSERT(origFixed.length() < 10);
        ASSERT(npos == origFixed.find_first_of("eE+-"));
        ASSERT(1 == u::countChar(origFixed, '.'));

        oss.str("");
        oss << true;
        const bsl::string& origBool = oss.str();
        if (verbose) P(origBool);
        ASSERT("1" == origBool);

        for (int ii = 0, quit = 0; !quit; ++ii) {
            oss.str("");
            if (veryVerbose) cout << endl;

            {
                Guard guard(&oss);

                switch (ii) {
                  case 0: {
                    // Write out an integral type in a wide field in hex

                    oss.fill('I');
                    oss << bsl::setw(8) << bsl::hex << bsl::showbase <<
                                                             bsl::right << 100;
                    const bsl::string& outputHex = oss.str();
                    if (veryVerbose) P(outputHex);
                    ASSERTV(outputHex, "IIII0x64" == outputHex);
                  } break;
                  case 1: {
                    // Write out an integral type in a wide field in octal

                    oss.fill('*');
                    oss << bsl::oct << bsl::right << bsl::setw(10) <<
                                                           bsl::showbase << 40;
                    const bsl::string& outputOct = oss.str();
                    if (veryVerbose) P(outputOct);
                    ASSERTV(outputOct, "*******050" == outputOct);
                  } break;
                  case 2: {
                    // Write a double out in uppercase scientific

                    oss << bsl::scientific << bsl::uppercase <<
                                           bsl::setprecision(20) << oneSeventh;
                    const bsl::string& outputSci = oss.str();
                    if (veryVerbose) P(outputSci);
                    const char firstChar = outputSci[0];
                    ASSERT(bsl::strchr("01.", firstChar));
                    ASSERT(20 < outputSci.length());
                    ASSERT(1 == u::countChar(outputSci, 'E'));
                    ASSERT(0 == u::countChar(outputSci, 'e'));
                    ASSERT(u::countChar(outputSci, '-') <= 1);
                    ASSERT(u::countChar(outputSci, '+') <= 1);
                    ASSERT(('1' == firstChar) == (1 == u::countChar(
                                                             outputSci, '-')));
                    ASSERT(!!bsl::strchr("0.", firstChar) ==
                                          (1 == u::countChar(outputSci, '+')));
                    ASSERT(1 == u::countChar(outputSci, '.'));
                  } break;
                  case 3: {
                    // Write a double out in lowercase scientific

                    oss << bsl::scientific <<
                                           bsl::setprecision(20) << oneSeventh;
                    const bsl::string& outputSci = oss.str();
                    if (veryVerbose) P(outputSci);
                    const char firstChar = outputSci[0];
                    ASSERT(bsl::strchr("01.", firstChar));
                    ASSERT(20 < outputSci.length());
                    ASSERT(0 == u::countChar(outputSci, 'E'));
                    ASSERT(1 == u::countChar(outputSci, 'e'));
                    ASSERT(u::countChar(outputSci, '-') <= 1);
                    ASSERT(u::countChar(outputSci, '+') <= 1);
                    ASSERT(('1' == firstChar) == (1 == u::countChar(
                                                             outputSci, '-')));
                    ASSERT(!!bsl::strchr("0.", firstChar) ==
                                          (1 == u::countChar(outputSci, '+')));
                    ASSERT(1 == u::countChar(outputSci, '.'));
                  } break;
                  case 4: {
                    // Write a double out in fixed precision

                    oss << bsl::setprecision(20) << oneSeventh;
                    const bsl::string& outputFixed = oss.str();
                    if (veryVerbose) P(outputFixed);
                    const char firstChar = outputFixed[0];
                    ASSERT(bsl::strchr("0.", firstChar));
                    ASSERT(20 < outputFixed.length());
                    ASSERT(npos == outputFixed.find_first_of("eE+-"));
                    ASSERT(1 == u::countChar(outputFixed, '.'));
                  } break;
                  case 5: {
                    // Write 'bool's in alphanumeric form

                    // This test fails when using libc++ because of
                    // https://cplusplus.github.io/LWG/lwg-active.html#2703
                    oss.width(7);
                    oss.fill('%');
                    oss << bsl::right << bsl::boolalpha << false << ' ';
                    oss.width(7);
                    oss << true;
                    const bsl::string& outputBool = oss.str();
                    if (veryVerbose) P(outputBool);
#ifdef BSLS_LIBRARYFEATURES_STDCPP_LLVM
                    ASSERTV(outputBool, "false%%%%%% true" == outputBool);
#else
                    ASSERTV(outputBool, "%%false %%%true" == outputBool);
#endif
                  } break;
                  case 6: {
                    quit = true;
                    continue;
                  } break;
                  default: {
                    BSLS_ASSERT_OPT(0);
                  }
                }
            }

            if (verbose) cout << endl;

            // Write several objects with the original configuration

            oss.str("");
            oss << bsl::setw(6) << 123;
            const bsl::string& postGuardDec = oss.str();
            if (veryVerbose) P(postGuardDec);
            ASSERT(origDec == postGuardDec);
            ASSERT("123___" == postGuardDec);

            oss.str("");
            oss << oneSeventh;
            const bsl::string& postGuardFixed = oss.str();
            if (veryVerbose) P(postGuardFixed);
            ASSERT(postGuardFixed.length() == defaultLen);
            const char firstChar = postGuardFixed[0];
            ASSERT(bsl::strchr("0.", firstChar));
            ASSERT(postGuardFixed.length() < 10);
            ASSERT(npos == postGuardFixed.find_first_of("eE+-"));
            ASSERT(1 == u::countChar(postGuardFixed, '.'));
            ASSERT(origFixed == postGuardFixed);

            oss.str("");
            oss << true;
            const bsl::string& postGuardBool = oss.str();
            if (veryVerbose) P(postGuardBool);
            ASSERT("1" == postGuardBool);
            ASSERT(origBool == postGuardBool);
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //
        // Concerns:
        //: 1 The class is sufficiently functional to support comprehensive
        //:   testing.
        //
        // Plan:
        //: 1 Use the guard on an output stream and an input stream and observe
        //:   its effects.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) cout << "BREATHING TEST\n"
                             "==============\n";

        using bsl::ios_base;

        bsl::ostringstream out;

        ASSERT(0 == (out.flags() & ios_base::floatfield));
        ASSERT(ios_base::dec   == (out.flags() & ios_base::basefield));
        ASSERT(' ' == out.fill());

        {
            Guard guard(&out);

            out << bsl::fixed;

            ASSERT(ios_base::fixed == (out.flags() & ios_base::floatfield));

            out << bsl::hex;

            ASSERT(ios_base::hex == (out.flags() & ios_base::basefield));

            out.fill('_');

            ASSERT(out.fill() == '_');

            out.width(4);

            out << 12;

            ASSERT("___c" == out.str());

            out.str("");

            out.precision(20);

            out << (1.0 / 7);

            bsl::size_t len = out.str().length();

            ASSERT(20 < len);

            if (verbose) {
                P_(len);    P(out.str());
            }
        }

        out.str("");
        out << (1.0 / 7);

        bsl::size_t len = out.str().length();

        ASSERTV(len, len < 10);

        if (verbose) {
            P_(len);    P(out.str());
        }

        ASSERT(0 == (out.flags() & ios_base::floatfield));
        ASSERT(ios_base::dec   == (out.flags() & ios_base::basefield));
        ASSERT(' ' == out.fill());

        bsl::istringstream in;

        ASSERT(0 == (in.flags() & ios_base::floatfield));
        ASSERT(ios_base::dec   == (in.flags() & ios_base::basefield));
      } break;
      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND.\n";
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        cerr << "Error, non-zero test status = " << testStatus << ".\n";
    }
    return testStatus;
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
