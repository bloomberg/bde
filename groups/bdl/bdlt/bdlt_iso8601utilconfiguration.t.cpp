// bdlt_iso8601utilconfiguration.t.cpp                                -*-C++-*-
#include <bdlt_iso8601utilconfiguration.h>

#include <bdls_testutil.h>

#include <bsls_asserttest.h>

#include <bsl_cctype.h>      // 'isdigit'
#include <bsl_cstdlib.h>
#include <bsl_cstring.h>
#include <bsl_iostream.h>
#include <bsl_sstream.h>
#include <bsl_string.h>

using namespace BloombergLP;
using namespace bsl;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// TBD
//
// Global Concerns:
//: o No memory is ever allocated from the global allocator.
//: o Precondition violations are detected in appropriate build modes.
//-----------------------------------------------------------------------------
// CLASS METHODS
// [ 1] static Config defaultConfiguration();
// [ 1] static void setDefaultConfiguration(const Config& config);
//
// CREATORS
// [ 1] Iso8601UtilConfiguration();
// [ 1] Iso8601UtilConfiguration(const Config& original);
// [ 1] ~Iso8601UtilConfiguration();
//
// MANIPULATORS
// [ 1] Config& operator=(const Config& rhs);
// [ 1] void setOmitColonInZoneDesignator(bool value);
// [ 1] void setUseCommaForDecimalSign(bool value);
// [ 1] void setUseZAbbreviationForUtc(bool value);
//
// ACCESSORS
// [ 1] bool omitColonInZoneDesignator() const;
// [ 1] bool useCommaForDecimalSign() const;
// [ 1] bool useZAbbreviationForUtc() const;
//
// [ 5] ostream& print(ostream& s, int level = 0, int sPL = 4) const;
//
// FREE OPERATORS
// [ 1] bool operator==(const Config& lhs, const Config& rhs);
// [ 1] bool operator!=(const Config& lhs, const Config& rhs);
// [ 5] operator<<(ostream& s, const Config& d);
//-----------------------------------------------------------------------------
// [ 2] USAGE EXAMPLE
//-----------------------------------------------------------------------------

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

#define ASSERT       BDLS_TESTUTIL_ASSERT
#define ASSERTV      BDLS_TESTUTIL_ASSERTV

#define LOOP_ASSERT  BDLS_TESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BDLS_TESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BDLS_TESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BDLS_TESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BDLS_TESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BDLS_TESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BDLS_TESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BDLS_TESTUTIL_LOOP6_ASSERT

#define Q            BDLS_TESTUTIL_Q   // Quote identifier literally.
#define P            BDLS_TESTUTIL_P   // Print identifier and value.
#define P_           BDLS_TESTUTIL_P_  // P(X) without '\n'.
#define T_           BDLS_TESTUTIL_T_  // Print a tab (w/o newline).
#define L_           BDLS_TESTUTIL_L_  // current Line number

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
//                  GLOBALS, TYPEDEFS, CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

typedef bdlt::Iso8601UtilConfiguration Obj;

// ============================================================================
//                             GLOBAL TEST DATA
// ----------------------------------------------------------------------------

struct DefaultCnfgDataRow {
    int  d_line;       // source line number
    bool d_omitColon;  // 'omitColonInZoneDesignator' attribute
    bool d_useComma;   // 'useCommaForDecimalSign'        "
    bool d_useZ;       // 'useZAbbreviationForUtc'        "
};

static
const DefaultCnfgDataRow DEFAULT_CNFG_DATA[] =
{
    //LINE   omit ':'   use ','   use 'Z'
    //----   --------   -------   -------
    { L_,      false,    false,    false  },
    { L_,      false,    false,     true  },
    { L_,      false,     true,    false  },
    { L_,      false,     true,     true  },
    { L_,       true,    false,    false  },
    { L_,       true,    false,     true  },
    { L_,       true,     true,    false  },
    { L_,       true,     true,     true  },
};
const int NUM_DEFAULT_CNFG_DATA =
        static_cast<int>(sizeof DEFAULT_CNFG_DATA / sizeof *DEFAULT_CNFG_DATA);

//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

static
Obj& gg(Obj *object,
           bool    omitColonInZoneDesignatorFlag,
           bool    useCommaForDecimalSignFlag,
           bool    useZAbbreviationForUtcFlag)
    // Return, by reference, the specified '*object' with its value adjusted
    // according to the specified 'omitColonInZoneDesignatorFlag',
    // 'useCommaForDecimalSignFlag', and 'useZAbbreviationForUtcFlag'.
{
    object->setOmitColonInZoneDesignator(omitColonInZoneDesignatorFlag);
    object->setUseCommaForDecimalSign(useCommaForDecimalSignFlag);
    object->setUseZAbbreviationForUtc(useZAbbreviationForUtcFlag);

    return *object;
}

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    const int                 test = argc > 1 ? atoi(argv[1]) : 0;
    const bool             verbose = argc > 2;
    const bool         veryVerbose = argc > 3;
    const bool     veryVeryVerbose = argc > 4;
    const bool veryVeryVeryVerbose = argc > 5;

    (void)veryVeryVerbose;  // eliminate unused variable warning
    (void)veryVeryVeryVerbose;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:  // Zero is always the leading case.
      case 2: {
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

        if (verbose) cout << endl
                          << "USAGE EXAMPLE" << endl
                          << "=============" << endl;

///Usage
///-----
// This section illustrates intended use of this component.
//
{  // START EXAMPLE 1
///Example 1: Configuring ISO 8601 String Generation
///- - - - - - - - - - - - - - - - - - - - - - - - -
// This example demonstrates creation of a 'bdlt::Iso8601UtilConfiguration'
// object that may be used to influence the format of the output produced by a
// hypothetical utility, 'my::Iso8601Util', that generates and parses ISO 8601
// strings for 'bdlt' vocabulary types (see 'bdlt_iso8601util', which provides
// just such functionality).  In particular, suppose that given a sample
// 'bdlt::TimeTz' object:
//..
//  const bdlt::TimeTz timeTz(bdlt::Time(8, 59, 59, 123), 240);
//..
// 'my::Iso8601Util' produces, by default, the following (valid) ISO 8601
// string:
//..
//  08:59:59.123+04:00
//..
// However, we would like to produce the following (also valid) ISO 8601 string
// instead:
//..
//  08:59:59,123+0400
//..
// 'bdlt::Iso8601UtilConfiguration' can be used to obtain the desired result
// assuming that 'my::Iso8601Util' uses 'bdlt::Iso8601UtilConfiguration' to
// affect the format of generated strings in this fashion (e.g., again see
// 'bdlt_iso8601util').
//
// First, we construct a 'bdlt::Iso8601UtilConfiguration' object that has the
// default value:
//..
    bdlt::Iso8601UtilConfiguration configuration;
    ASSERT(!configuration.omitColonInZoneDesignator());
    ASSERT(!configuration.useCommaForDecimalSign());
    ASSERT(!configuration.useZAbbreviationForUtc());
//..
// Then, we modify 'configuration' to indicate that we want to use ',' as the
// decimal sign (in fractional seconds):
//..
    configuration.setUseCommaForDecimalSign(true);
    ASSERT(!configuration.omitColonInZoneDesignator());
    ASSERT( configuration.useCommaForDecimalSign());
    ASSERT(!configuration.useZAbbreviationForUtc());
//..
// Finally, we modify 'configuration' to indicate that we want to omit the ':'
// in zone designators:
//..
    configuration.setOmitColonInZoneDesignator(true);
    ASSERT( configuration.omitColonInZoneDesignator());
    ASSERT( configuration.useCommaForDecimalSign());
    ASSERT(!configuration.useZAbbreviationForUtc());
//..
// Our 'configuration' object can now be supplied to 'my::Iso8601Util' to
// produce the desired result.
//
}  // END EXAMPLE 1
{  // START EXAMPLE 2
///Example 2: Setting the Process-Wide Default Configuration
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// This example demonstrates how to establish the process-wide default
// configuration.
//
// First, we retrieve the default configuration in effect at process start-up
// and note that it has the default-constructed value:
//..
    bdlt::Iso8601UtilConfiguration configuration =
                        bdlt::Iso8601UtilConfiguration::defaultConfiguration();
    ASSERT(bdlt::Iso8601UtilConfiguration() == configuration);
    ASSERT(!configuration.omitColonInZoneDesignator());
    ASSERT(!configuration.useCommaForDecimalSign());
    ASSERT(!configuration.useZAbbreviationForUtc());
//..
// Next, we modify 'configuration' to indicate that we want to output 'Z' when
// the zone designator is UTC (i.e., instead of '+00:00'):
//..
    configuration.setUseZAbbreviationForUtc(true);
    ASSERT(!configuration.omitColonInZoneDesignator());
    ASSERT(!configuration.useCommaForDecimalSign());
    ASSERT( configuration.useZAbbreviationForUtc());
//..
// Now, we set the default configuration to the value of our 'configuration'
// object:
//..
    bdlt::Iso8601UtilConfiguration::setDefaultConfiguration(configuration);
//..
// Finally, we verify that the default configuration was updated as expected:
//..
    const bdlt::Iso8601UtilConfiguration newConfiguration =
                        bdlt::Iso8601UtilConfiguration::defaultConfiguration();
    ASSERT(!newConfiguration.omitColonInZoneDesignator());
    ASSERT(!newConfiguration.useCommaForDecimalSign());
    ASSERT( newConfiguration.useZAbbreviationForUtc());
//..
// Note that the expected usage is that the process-wide configuration will be
// established *once*, early in 'main', and not changed throughout the lifetime
// of a process.
}  // END EXAMPLE 2

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // TESTING 'Iso8601UtilConfiguration'
        //
        // Concerns:
        //: 1 Our concerns include those that apply to unconstrained attribute
        //:   types, with the simplification that the class under test lacks a
        //:   'print' method and free 'operator<<'.
        //:
        //: 2 The two class methods that manage the process-wide configuration
        //:   work correctly.
        //
        // Plan:
        //: 1 Follow the standard regimen for testing an unconstrained
        //:   attribute type.  (C-1)
        //:
        //: 2 Exercise the class methods on all possible configurations and
        //:   verify that the behavior is as expected.  (C-2)
        //
        // Testing:
        //   static Config defaultConfiguration();
        //   static void setDefaultConfiguration(const Config& config);
        //   Iso8601UtilConfiguration();
        //   Iso8601UtilConfiguration(const Config& original);
        //   ~Iso8601UtilConfiguration();
        //   Config& operator=(const Config& rhs);
        //   void setOmitColonInZoneDesignator(bool value);
        //   void setUseCommaForDecimalSign(bool value);
        //   void setUseZAbbreviationForUtc(bool value);
        //   bool omitColonInZoneDesignator() const;
        //   bool useCommaForDecimalSign() const;
        //   bool useZAbbreviationForUtc() const;
        //   bool operator==(const Config& lhs, const Config& rhs);
        //   bool operator!=(const Config& lhs, const Config& rhs);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING 'Iso8601UtilConfiguration'" << endl
                          << "==================================" << endl;

        const int                  NUM_DATA        = NUM_DEFAULT_CNFG_DATA;
        const DefaultCnfgDataRow (&DATA)[NUM_DATA] = DEFAULT_CNFG_DATA;

        // Testing:
        //   Iso8601UtilConfiguration();
        //   ~Iso8601UtilConfiguration();
        {
            const Obj X;

            ASSERT(!X.omitColonInZoneDesignator());
            ASSERT(!X.useCommaForDecimalSign());
            ASSERT(!X.useZAbbreviationForUtc());
        }

        // Testing:
        //   void setOmitColonInZoneDesignator(bool value);
        //   void setUseCommaForDecimalSign(bool value);
        //   void setUseZAbbreviationForUtc(bool value);
        //   bool omitColonInZoneDesignator() const;
        //   bool useCommaForDecimalSign() const;
        //   bool useZAbbreviationForUtc() const;
        {
            {
                Obj mX;  const Obj& X = mX;

                mX.setOmitColonInZoneDesignator(true);

                ASSERT( X.omitColonInZoneDesignator());
                ASSERT(!X.useCommaForDecimalSign());
                ASSERT(!X.useZAbbreviationForUtc());
            }

            {
                Obj mX;  const Obj& X = mX;

                mX.setUseCommaForDecimalSign(true);

                ASSERT(!X.omitColonInZoneDesignator());
                ASSERT( X.useCommaForDecimalSign());
                ASSERT(!X.useZAbbreviationForUtc());
            }

            {
                Obj mX;  const Obj& X = mX;

                mX.setUseZAbbreviationForUtc(true);

                ASSERT(!X.omitColonInZoneDesignator());
                ASSERT(!X.useCommaForDecimalSign());
                ASSERT( X.useZAbbreviationForUtc());
            }

            Obj mX;  const Obj& X = mX;

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int  LINE      = DATA[ti].d_line;
                const bool OMITCOLON = DATA[ti].d_omitColon;
                const bool USECOMMA  = DATA[ti].d_useComma;
                const bool USEZ      = DATA[ti].d_useZ;

                if (veryVerbose) {
                    T_ P_(LINE) P_(OMITCOLON) P_(USECOMMA) P(USEZ)
                }

                mX.setOmitColonInZoneDesignator(OMITCOLON);
                mX.setUseCommaForDecimalSign(USECOMMA);
                mX.setUseZAbbreviationForUtc(USEZ);

                ASSERTV(LINE, OMITCOLON == X.omitColonInZoneDesignator());
                ASSERTV(LINE, USECOMMA  == X.useCommaForDecimalSign());
                ASSERTV(LINE, USEZ      == X.useZAbbreviationForUtc());
            }
        }

        // Testing:
        //   bool operator==(const Obj& lhs, const Obj& rhs);
        //   bool operator!=(const Obj& lhs, const Obj& rhs);
        {
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int  ILINE      = DATA[ti].d_line;
                const bool IOMITCOLON = DATA[ti].d_omitColon;
                const bool IUSECOMMA  = DATA[ti].d_useComma;
                const bool IUSEZ      = DATA[ti].d_useZ;

                if (veryVerbose) {
                    T_ P_(ILINE) P_(IOMITCOLON) P_(IUSECOMMA) P(IUSEZ)
                }

                for (int tj = 0; tj < NUM_DATA; ++tj) {
                    const int  JLINE      = DATA[tj].d_line;
                    const bool JOMITCOLON = DATA[tj].d_omitColon;
                    const bool JUSECOMMA  = DATA[tj].d_useComma;
                    const bool JUSEZ      = DATA[tj].d_useZ;

                    if (veryVerbose) {
                        T_ P_(JLINE) P_(JOMITCOLON) P_(JUSECOMMA) P(JUSEZ)
                    }

                    Obj mX;  const Obj& X = mX;
                    gg(&mX, IOMITCOLON, IUSECOMMA, IUSEZ);

                    Obj mY;  const Obj& Y = mY;
                    gg(&mY, JOMITCOLON, JUSECOMMA, JUSEZ);

                    const bool EXP = ti == tj;  // expected for '==' comparison

                    ASSERTV(ILINE, JLINE,  EXP == (X == Y));
                    ASSERTV(ILINE, JLINE,  EXP == (Y == X));

                    ASSERTV(ILINE, JLINE, !EXP == (X != Y));
                    ASSERTV(ILINE, JLINE, !EXP == (Y != X));
                }
            }
        }

        // Testing:
        //   Iso8601UtilConfiguration(const Obj& original);
        {
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int  LINE      = DATA[ti].d_line;
                const bool OMITCOLON = DATA[ti].d_omitColon;
                const bool USECOMMA  = DATA[ti].d_useComma;
                const bool USEZ      = DATA[ti].d_useZ;

                if (veryVerbose) {
                    T_ P_(LINE) P_(OMITCOLON) P_(USECOMMA) P(USEZ)
                }

                Obj mX;  const Obj& X = mX;
                gg(&mX, OMITCOLON, USECOMMA, USEZ);

                const Obj Y(X);

                ASSERTV(LINE, X == Y);
            }
        }

        // Testing:
        //   Obj& operator=(const Obj& rhs);
        {
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int  ILINE      = DATA[ti].d_line;
                const bool IOMITCOLON = DATA[ti].d_omitColon;
                const bool IUSECOMMA  = DATA[ti].d_useComma;
                const bool IUSEZ      = DATA[ti].d_useZ;

                if (veryVerbose) {
                    T_ P_(ILINE) P_(IOMITCOLON) P_(IUSECOMMA) P(IUSEZ)
                }

                Obj mZ;  const Obj& Z = mZ;
                gg(&mZ, IOMITCOLON, IUSECOMMA, IUSEZ);

                for (int tj = 0; tj < NUM_DATA; ++tj) {
                    const int  JLINE      = DATA[tj].d_line;
                    const bool JOMITCOLON = DATA[tj].d_omitColon;
                    const bool JUSECOMMA  = DATA[tj].d_useComma;
                    const bool JUSEZ      = DATA[tj].d_useZ;

                    if (veryVerbose) {
                        T_ P_(JLINE) P_(JOMITCOLON) P_(JUSECOMMA) P(JUSEZ)
                    }

                    Obj mX;  const Obj& X = mX;
                    gg(&mX, JOMITCOLON, JUSECOMMA, JUSEZ);

                    ASSERTV(ILINE, JLINE, (Z == X) == (ILINE == JLINE));

                    Obj *mR = &(mX = Z);

                    ASSERTV(ILINE, JLINE,  Z == X);
                    ASSERTV(ILINE, JLINE, mR == &mX);
                }

                // self-assignment

                {
                    Obj mX;
                    gg(&mX, IOMITCOLON, IUSECOMMA, IUSEZ);

                    Obj mZZ;  const Obj& ZZ = mZZ;
                    gg(&mZZ, IOMITCOLON, IUSECOMMA, IUSEZ);

                    const Obj& Z = mX;

                    ASSERTV(ILINE, ZZ == Z);

                    Obj *mR = &(mX = Z);

                    ASSERTV(ILINE, mR == &mX);
                    ASSERTV(ILINE, ZZ == Z);
                }
            }
        }

        // Testing:
        //   static void setDefaultConfiguration(const Obj& config);
        //   static Obj defaultConfiguration();
        {
            ASSERT(Obj() == Obj::defaultConfiguration());

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int  LINE      = DATA[ti].d_line;
                const bool OMITCOLON = DATA[ti].d_omitColon;
                const bool USECOMMA  = DATA[ti].d_useComma;
                const bool USEZ      = DATA[ti].d_useZ;

                if (veryVerbose) {
                    T_ P_(LINE) P_(OMITCOLON) P_(USECOMMA) P(USEZ)
                }

                Obj mX;  const Obj& X = mX;
                gg(&mX, OMITCOLON, USECOMMA, USEZ);

                {
                    Obj mY;  const Obj& Y = mY;
                    gg(&mY, OMITCOLON, USECOMMA, USEZ);

                    Obj::setDefaultConfiguration(Y);
                }

                ASSERTV(LINE, X == Obj::defaultConfiguration());
            }
        }

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
