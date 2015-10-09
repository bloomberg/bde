// bdlt_iso8601utilconfiguration.t.cpp                                -*-C++-*-
#include <bdlt_iso8601utilconfiguration.h>

#include <bslim_testutil.h>
#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>
#include <bsls_asserttest.h>

#include <bsl_cstdlib.h>     // 'atoi'
#include <bsl_cstring.h>     // 'memcmp', 'strcmp'
#include <bsl_iostream.h>
#include <bsl_sstream.h>

using namespace BloombergLP;
using namespace bsl;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// The component under test implements a single, unconstrained (value-semantic)
// attribute class.  The Primary Manipulators and Basic Accessors are
// therefore, respectively, the attribute setters and getters, each of which
// follows our standard unconstrained attribute-type naming conventions:
// 'setAttributeName' and 'attributeName'.
//
// Primary Manipulators:
//: o 'setOmitColonInZoneDesignator'
//: o 'setUseCommaForDecimalSign'
//: o 'setUseZAbbreviationForUtc'
//
// Basic Accessors:
//: o 'omitColonInZoneDesignator'
//: o 'useCommaForDecimalSign'
//: o 'useZAbbreviationForUtc'
//
// We will therefore follow our standard 10-case approach to testing
// value-semantic types, with the default constructor and primary manipulators
// tested fully in case 2.
//
// Certain standard value-semantic-type test cases are omitted:
//: o [10] -- BSLX streaming is not (yet) implemented for this class.
//
// Global Concerns:
//: o The test driver is robust w.r.t. reuse in other, similar components.
//: o ACCESSOR methods are declared 'const'.
//: o CREATOR & MANIPULATOR pointer/reference parameters are declared 'const'.
//: o No memory is ever allocated from the global allocator.
//: o No memory is ever allocated from the default allocator.
//: o Precondition violations are detected in appropriate build modes.
//-----------------------------------------------------------------------------
// CLASS METHODS
// [11] static Config defaultConfiguration();
// [11] static void setDefaultConfiguration(const Config& config);
//
// CREATORS
// [ 2] Iso8601UtilConfiguration();
// [ 7] Iso8601UtilConfiguration(const Config& original);
// [ 2] ~Iso8601UtilConfiguration();
//
// MANIPULATORS
// [ 9] Config& operator=(const Config& rhs);
// [ 2] void setOmitColonInZoneDesignator(bool value);
// [ 2] void setUseCommaForDecimalSign(bool value);
// [ 2] void setUseZAbbreviationForUtc(bool value);
//
// ACCESSORS
// [ 4] bool omitColonInZoneDesignator() const;
// [ 4] bool useCommaForDecimalSign() const;
// [ 4] bool useZAbbreviationForUtc() const;
//
// [ 5] ostream& print(ostream& s, int level = 0, int sPL = 4) const;
//
// FREE OPERATORS
// [ 6] bool operator==(const Config& lhs, const Config& rhs);
// [ 6] bool operator!=(const Config& lhs, const Config& rhs);
// [ 5] ostream& operator<<(ostream& s, const Config& d);
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [12] USAGE EXAMPLE
// [ *] CONCERN: This test driver is reusable w/other, similar components.
// [ *] CONCERN: In no case does memory come from the global allocator.
// [ *] CONCERN: In no case does memory come from the default allocator.
// [ 5] CONCERN: All accessor methods are declared 'const'.
// [10] Reserved for 'bslx' streaming.

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
//                  GLOBALS, TYPEDEFS, CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

typedef bdlt::Iso8601UtilConfiguration Obj;

// ============================================================================
//                             GLOBAL TEST DATA
// ----------------------------------------------------------------------------

// Define DEFAULT DATA used by test cases 4, 6, 7, 9, and 11.

struct DefaultDataRow {
    int  d_line;       // source line number
    bool d_omitColon;  // 'omitColonInZoneDesignator' attribute
    bool d_useComma;   // 'useCommaForDecimalSign'        "
    bool d_useZ;       // 'useZAbbreviationForUtc'        "
};

static
const DefaultDataRow DEFAULT_DATA[] =
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
const int NUM_DEFAULT_DATA =
                  static_cast<int>(sizeof DEFAULT_DATA / sizeof *DEFAULT_DATA);

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

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    // CONCERN: This test driver is reusable w/other, similar components.

    // CONCERN: In no case does memory come from the global allocator.

    bslma::TestAllocator globalAllocator("global", veryVeryVeryVerbose);
    bslma::Default::setGlobalAllocator(&globalAllocator);

    // CONCERN: In no case does memory come from the default allocator.

    bslma::TestAllocator defaultAllocator("default", veryVeryVeryVerbose);
    bslma::DefaultAllocatorGuard defaultAllocatorGuard(&defaultAllocator);

    switch (test) { case 0:  // Zero is always the leading case.
      case 12: {
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
      case 11: {
        // --------------------------------------------------------------------
        // CLASS METHODS
        //   Ensure that the two class methods correctly set and retrieve the
        //   default configuration.
        //
        // Concerns:
        //: 1 The initial value of the default configuration is the same as
        //:   that of a default constructed 'bdlt::Iso8601UtilConfiguration'
        //:   object.
        //:
        //: 2 'defaultConfiguration' returns the current value of the default
        //:   configuration.
        //:
        //: 3 'setDefaultConfiguration' sets the value of the default
        //:   configuration to that of its argument.
        //:
        //: 4 The value of the object passed to 'setDefaultConfiguration' is
        //:   not modified.
        //
        // Plan:
        //: 1 Prior to invoking 'setDefaultConfiguration', verify that the
        //:   value initially returned by 'defaultConfiguration' has the
        //:   default-constructed value.  (C-1)
        //:
        //: 2 Using the table-driven technique, specify a set of distinct
        //:   object values (one per row) in terms of their attribute values.
        //:
        //: 3 For each row 'R' in the table of P-2:  (C-2..4)
        //:
        //:   1 Use the default constructor and primary manipulators to create
        //:     a 'const' object 'Z' having the value from 'R', and use the
        //:     copy constructor to create a second 'const' object 'ZZ' from
        //:     'Z'.
        //:
        //:   2 Invoke 'setDefaultConfiguration' on 'Z'.
        //:
        //:   3 Use the equality-comparison operator to verify that: (C-2..4)
        //:
        //:     1 The default configuration, fetched using
        //:       'defaultConfiguration', now has the same value as that of
        //:       'Z'.  (C-2..3)
        //:
        //:     2 'Z' still has the same value as that of 'ZZ'.  (C-4)
        //
        // Testing:
        //   static Config defaultConfiguration();
        //   static void setDefaultConfiguration(const Config& config);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "CLASS METHODS" << endl
                          << "=============" << endl;

        ASSERT(Obj() == Obj::defaultConfiguration());

        const int              NUM_DATA        = NUM_DEFAULT_DATA;
        const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int  LINE      = DATA[ti].d_line;
            const bool OMITCOLON = DATA[ti].d_omitColon;
            const bool USECOMMA  = DATA[ti].d_useComma;
            const bool USEZ      = DATA[ti].d_useZ;

            if (veryVerbose) { T_ P_(LINE) P_(OMITCOLON) P_(USECOMMA) P(USEZ) }

            Obj mZ;  const Obj& Z = mZ;

            mZ.setOmitColonInZoneDesignator(OMITCOLON);
            mZ.setUseCommaForDecimalSign(USECOMMA);
            mZ.setUseZAbbreviationForUtc(USEZ);

            const Obj ZZ(Z);

            Obj::setDefaultConfiguration(Z);

            ASSERTV(LINE,  Z == Obj::defaultConfiguration());
            ASSERTV(LINE, ZZ == Z);
        }

      } break;
      case 10: {
        // --------------------------------------------------------------------
        // BSLX STREAMING
        //   Ensure that we can serialize the value of any object of the class,
        //   and then deserialize that value back into any object of the class.
        //
        // Concerns:
        //   N/A
        //
        // Plan:
        //   N/A
        //
        // Testing:
        //   Reserved for 'bslx' streaming.
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BSLX STREAMING" << endl
                          << "==============" << endl;

        if (verbose) cout << "Not yet implemented." << endl;

      } break;
      case 9: {
        // --------------------------------------------------------------------
        // COPY-ASSIGNMENT OPERATOR
        //   Ensure that we can assign the value of any object of the class to
        //   any object of the class, such that the two objects subsequently
        //   have the same value.
        //
        // Concerns:
        //: 1 The assignment operator can change the value of any modifiable
        //:   target object to that of any source object.
        //:
        //: 2 The signature and return type are standard.
        //:
        //: 3 The reference returned is to the target object (i.e., '*this').
        //:
        //: 4 The value of the source object is not modified.
        //:
        //: 5 Assigning an object to itself behaves as expected (alias-safety).
        //
        // Plan:
        //: 1 Use the address of 'operator=' to initialize a member-function
        //:   pointer having the appropriate signature and return type for the
        //:   copy-assignment operator defined in this component.  (C-2)
        //:
        //: 2 Using the table-driven technique, specify a set of distinct
        //:   object values (one per row) in terms of their attribute values.
        //:
        //: 3 For each row 'R1' in the table of P-2:  (C-1, 3..4)
        //:
        //:   1 Use the default constructor and primary manipulators to create
        //:     a 'const' object 'Z' having the value from 'R1', and use the
        //:     copy constructor to create a second 'const' object 'ZZ' from
        //:     'Z'.
        //:
        //:   2 For each row 'R2' in the table of P-2:  (C-1, 3..4)
        //:
        //:     1 Use the default constructor and primary manipulators to
        //:       create a modifiable 'Obj', 'mX', having the value from 'R2'.
        //:
        //:     2 Assign 'mX' from 'Z'.
        //:
        //:     3 Verify that the address of the return value is the same as
        //:       that of 'mX'.  (C-3)
        //:
        //:     4 Use the equality-comparison operator to verify that: (C-1, 4)
        //:
        //:       1 The target object, 'mX', now has the same value as that of
        //:         'Z'.  (C-1)
        //:
        //:       2 'Z' still has the same value as that of 'ZZ'.  (C-4)
        //:
        //: 4 Repeat steps similar to those described in P-3 except that, this
        //:   time, there is no inner loop (as in P-3.2); instead, the source
        //:   object, 'Z', is a reference to the target object, 'mX', and both
        //:   'mX' and 'ZZ' are created to have the value from 'R1'.  For each
        //:   'R1' in the table of P-2:  (C-5)
        //:
        //:   1 Use the default constructor and primary manipulators to create
        //:     a modifiable 'Obj', 'mX', having the value from 'R1', and use
        //:     the copy constructor to create a 'const' object 'ZZ' from 'mX'.
        //:
        //:   2 Let 'Z' be a reference providing only 'const' access to 'mX'.
        //:
        //:   3 Assign 'mX' from 'Z'.
        //:
        //:   4 Verify that the address of the return value is the same as that
        //:     of 'mX'.
        //:
        //:   5 Use the equality-comparison operator to verify that the
        //:     target object, 'Z' ('mX'), still has the same value as that of
        //:     'ZZ'.  (C-5)
        //
        // Testing:
        //   Config& operator=(const Config& rhs);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "COPY-ASSIGNMENT OPERATOR" << endl
                          << "========================" << endl;

        if (verbose) cout <<
                 "\nAssign the address of the operator to a variable." << endl;
        {
            typedef Obj& (Obj::*operatorPtr)(const Obj&);

            // Verify that the signature and return type are standard.

            operatorPtr operatorAssignment = &Obj::operator=;

            (void)operatorAssignment;  // quash potential compiler warning
        }

        const int              NUM_DATA        = NUM_DEFAULT_DATA;
        const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int  ILINE      = DATA[ti].d_line;
            const bool IOMITCOLON = DATA[ti].d_omitColon;
            const bool IUSECOMMA  = DATA[ti].d_useComma;
            const bool IUSEZ      = DATA[ti].d_useZ;

            if (veryVerbose) {
                T_ P_(ILINE) P_(IOMITCOLON) P_(IUSECOMMA) P(IUSEZ)
            }

            Obj mZ;  const Obj& Z = mZ;

            mZ.setOmitColonInZoneDesignator(IOMITCOLON);
            mZ.setUseCommaForDecimalSign(IUSECOMMA);
            mZ.setUseZAbbreviationForUtc(IUSEZ);

            const Obj ZZ(Z);

            if (veryVerbose) { T_ P_(ILINE) P(Z) }

            // Ensure the first row of the table contains the
            // default-constructed value.

            static bool firstFlag = true;
            if (firstFlag) {
                ASSERTV(ILINE, Obj(), Z, Obj() == Z);
                firstFlag = false;
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

                mX.setOmitColonInZoneDesignator(JOMITCOLON);
                mX.setUseCommaForDecimalSign(JUSECOMMA);
                mX.setUseZAbbreviationForUtc(JUSEZ);

                if (veryVerbose) { T_ T_ P_(JLINE) P(X) }

                ASSERTV(ILINE, JLINE, Z, X, (Z == X) == (ILINE == JLINE));

                Obj *mR = &(mX = Z);

                ASSERTV(ILINE, JLINE,  Z,   X,  Z == X);
                ASSERTV(ILINE, JLINE, mR, &mX, mR == &mX);
                ASSERTV(ILINE, JLINE, ZZ,   Z, ZZ == Z);
            }

            // self-assignment

            {
                Obj mX;  const Obj& X = mX;

                mX.setOmitColonInZoneDesignator(IOMITCOLON);
                mX.setUseCommaForDecimalSign(IUSECOMMA);
                mX.setUseZAbbreviationForUtc(IUSEZ);

                const Obj ZZ(X);

                const Obj& Z = mX;

                ASSERTV(ILINE, ZZ,   Z, ZZ == Z);

                Obj *mR = &(mX = Z);

                ASSERTV(ILINE, mR, &mX, mR == &mX);
                ASSERTV(ILINE, ZZ,   Z, ZZ == Z);
            }
        }

      } break;
      case 8: {
        // --------------------------------------------------------------------
        // SWAP MEMBER AND FREE FUNCTIONS
        //   Ensure that, when member and free 'swap' are implemented, we can
        //   exchange the values of any two objects.
        //
        // Concerns:
        //   N/A
        //
        // Plan:
        //   N/A
        //
        // Testing:
        //  Reserved for 'swap' testing.
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "SWAP MEMBER AND FREE FUNCTIONS" << endl
                          << "==============================" << endl;

        if (verbose) {
            cout << "Not implemented for 'bdlt::Iso8601UtilConfiguration'."
                 << endl;
        }

      } break;
      case 7: {
        // --------------------------------------------------------------------
        // COPY CONSTRUCTOR
        //   Ensure that we can create a distinct object of the class from any
        //   other one, such that the two objects have the same value.
        //
        // Concerns:
        //: 1 The copy constructor creates an object having the same value as
        //:   that of the supplied original object.
        //:
        //: 2 The original object is passed as a reference providing
        //:   non-modifiable access to that object.
        //:
        //: 3 The value of the original object is unchanged.
        //
        // Plan:
        //: 1 Using the table-driven technique, specify a set of distinct
        //:   object values (one per row) in terms of their attribute values.
        //:
        //: 2 For each row 'R' in the table of P-1:  (C-1..3)
        //:
        //:   1 Use the default constructor and primary manipulators to create
        //:     two 'const' objects, 'Z' and 'ZZ', both having the value from
        //:     'R'.
        //:
        //:   2 Use the copy constructor to create an object 'X' from 'Z'.
        //:     (C-2)
        //:
        //:   3 Use the equality-comparison operator to verify that:  (C-1, 3)
        //:
        //:     1 'X' has the same value as that of 'Z'.  (C-1)
        //:
        //:     2 'Z' still has the same value as that of 'ZZ'.  (C-3)
        //
        // Testing:
        //   Iso8601UtilConfiguration(const Config& original);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "COPY CONSTRUCTOR" << endl
                          << "================" << endl;

        if (verbose) cout << "\nUse a table of distinct object values."
                          << endl;

        const int              NUM_DATA        = NUM_DEFAULT_DATA;
        const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int  LINE      = DATA[ti].d_line;
            const bool OMITCOLON = DATA[ti].d_omitColon;
            const bool USECOMMA  = DATA[ti].d_useComma;
            const bool USEZ      = DATA[ti].d_useZ;

            if (veryVerbose) { T_ P_(LINE) P_(OMITCOLON) P_(USECOMMA) P(USEZ) }

            Obj mZ;  const Obj& Z = mZ;

            mZ.setOmitColonInZoneDesignator(OMITCOLON);
            mZ.setUseCommaForDecimalSign(USECOMMA);
            mZ.setUseZAbbreviationForUtc(USEZ);

            Obj mZZ;  const Obj& ZZ = mZZ;

            mZZ.setOmitColonInZoneDesignator(OMITCOLON);
            mZZ.setUseCommaForDecimalSign(USECOMMA);
            mZZ.setUseZAbbreviationForUtc(USEZ);

            if (veryVerbose) { T_ T_ P_(Z) P(ZZ) }

            const Obj X(Z);

            if (veryVerbose) { T_ T_ T_ P(X) }

            // Ensure the first row of the table contains the
            // default-constructed value.

            static bool firstFlag = true;
            if (firstFlag) {
                ASSERTV(LINE, Obj(), X, Obj() == X)
                firstFlag = false;
            }

            // Verify the value of the object.

            ASSERTV(LINE, Z,  X,  Z == X);

            // Verify that the value of 'Z' has not changed.

            ASSERTV(LINE, ZZ, Z, ZZ == Z);
        }

      } break;
      case 6: {
        // --------------------------------------------------------------------
        // EQUALITY-COMPARISON OPERATORS
        //   Ensure that '==' and '!=' are the operational definition of value.
        //
        // Concerns:
        //: 1 Two objects, 'X' and 'Y', compare equal if and only if each of
        //:   their corresponding salient attributes respectively compares
        //:   equal.
        //:
        //: 2 All salient attributes participate in the comparison.
        //:
        //: 3 'true  == (X == X)'  (i.e., identity)
        //:
        //: 4 'false == (X != X)'  (i.e., identity)
        //:
        //: 5 'X == Y' if and only if 'Y == X'  (i.e., commutativity)
        //:
        //: 6 'X != Y' if and only if 'Y != X'  (i.e., commutativity)
        //:
        //: 7 'X != Y' if and only if '!(X == Y)'
        //:
        //: 8 Comparison is symmetric with respect to user-defined conversion
        //:   (i.e., both comparison operators are free functions).
        //:
        //: 9 Non-modifiable objects can be compared (i.e., objects or
        //:   references providing only non-modifiable access).
        //:
        //:10 The equality-comparison operators' signatures and return types
        //:   are standard.
        //
        // Plan:
        //: 1 Use the respective addresses of 'operator==' and 'operator!=' to
        //:   initialize function pointers having the appropriate signatures
        //:   and return types for the two homogeneous, free equality-
        //:   comparison operators defined in this component.  (C-8..10)
        //:
        //: 2 Using the table-driven technique, specify a set of distinct
        //:   object values (one per row) in terms of their attribute values.
        //:
        //: 3 For each row 'R1' in the table of P-2:  (C-1..7)
        //:
        //:   1 Use the default constructor and primary manipulators to create
        //:     a 'const' object, 'W', having the value from 'R1'.
        //:
        //:   2 Using 'W', verify the reflexive (anti-reflexive) property of
        //:     equality (inequality) in the presence of aliasing.  (C-3..4)
        //:
        //:   3 For each row 'R2' in the table of P-2:  (C-1..2, 5..7)
        //:
        //:     1 Record, in 'EXP', whether or not distinct objects set to
        //:       values from 'R1' and 'R2', respectively, are expected to have
        //:       the same value.
        //:
        //:     2 Use the default constructor and primary manipulators to
        //:       create a 'const' object, 'X', having the value from 'R1', and
        //:       a second 'const' object, 'Y', having the value from 'R2'.
        //:
        //:     3 Using 'X' and 'Y', verify the commutativity property and
        //:       expected return value for both '==' and '!='.  (C-1..2, 5..7)
        //
        // Testing:
        //   bool operator==(const Config& lhs, const Config& rhs);
        //   bool operator!=(const Config& lhs, const Config& rhs);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "EQUALITY-COMPARISON OPERATORS" << endl
                          << "=============================" << endl;

        if (verbose) cout <<
                "\nAssign the address of each operator to a variable." << endl;
        {
            typedef bool (*operatorPtr)(const Obj&, const Obj&);

            // Verify that the signatures and return types are standard.

            operatorPtr operatorEq = bdlt::operator==;
            operatorPtr operatorNe = bdlt::operator!=;

            (void)operatorEq;  // quash potential compiler warnings
            (void)operatorNe;
        }

        const int              NUM_DATA        = NUM_DEFAULT_DATA;
        const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int  ILINE      = DATA[ti].d_line;
            const bool IOMITCOLON = DATA[ti].d_omitColon;
            const bool IUSECOMMA  = DATA[ti].d_useComma;
            const bool IUSEZ      = DATA[ti].d_useZ;

            if (veryVerbose) {
                T_ P_(ILINE) P_(IOMITCOLON) P_(IUSECOMMA) P(IUSEZ)
            }

            // Ensure an object compares correctly with itself (alias test).
            {
                Obj mW;  const Obj& W = mW;

                mW.setOmitColonInZoneDesignator(IOMITCOLON);
                mW.setUseCommaForDecimalSign(IUSECOMMA);
                mW.setUseZAbbreviationForUtc(IUSEZ);

                ASSERTV(ILINE, W,   W == W);
                ASSERTV(ILINE, W, !(W != W));

                // Ensure the first row of the table contains the
                // default-constructed value.

                static bool firstFlag = true;
                if (firstFlag) {
                    ASSERTV(ILINE, Obj(), W, Obj() == W)
                    firstFlag = false;
                }
            }

            Obj mX;  const Obj& X = mX;

            mX.setOmitColonInZoneDesignator(IOMITCOLON);
            mX.setUseCommaForDecimalSign(IUSECOMMA);
            mX.setUseZAbbreviationForUtc(IUSEZ);

            for (int tj = 0; tj < NUM_DATA; ++tj) {
                const int  JLINE      = DATA[tj].d_line;
                const bool JOMITCOLON = DATA[tj].d_omitColon;
                const bool JUSECOMMA  = DATA[tj].d_useComma;
                const bool JUSEZ      = DATA[tj].d_useZ;

                if (veryVerbose) {
                    T_ T_ P_(JLINE) P_(JOMITCOLON) P_(JUSECOMMA) P(JUSEZ)
                }

                const bool EXP = ti == tj;  // expected for equality comparison

                Obj mY;  const Obj& Y = mY;

                mY.setOmitColonInZoneDesignator(JOMITCOLON);
                mY.setUseCommaForDecimalSign(JUSECOMMA);
                mY.setUseZAbbreviationForUtc(JUSEZ);

                if (veryVerbose) { T_ T_ T_ P_(EXP) P_(X) P(Y) }

                // Verify expected against basic accessor comparison.

                if (EXP) {
                    ASSERTV(ILINE, JLINE, X, Y,
                 X.omitColonInZoneDesignator() == Y.omitColonInZoneDesignator()
              && X.useCommaForDecimalSign()    == Y.useCommaForDecimalSign()
              && X.useZAbbreviationForUtc()    == Y.useZAbbreviationForUtc());
                }
                else {
                    ASSERTV(ILINE, JLINE, X, Y,
                 X.omitColonInZoneDesignator() != Y.omitColonInZoneDesignator()
              || X.useCommaForDecimalSign()    != Y.useCommaForDecimalSign()
              || X.useZAbbreviationForUtc()    != Y.useZAbbreviationForUtc());
                }

                // Verify value and commutativity.

                ASSERTV(ILINE, JLINE, X, Y,  EXP == (X == Y));
                ASSERTV(ILINE, JLINE, Y, X,  EXP == (Y == X));

                ASSERTV(ILINE, JLINE, X, Y, !EXP == (X != Y));
                ASSERTV(ILINE, JLINE, Y, X, !EXP == (Y != X));
            }
        }

      } break;
      case 5: {
        // --------------------------------------------------------------------
        // PRINT AND OUTPUT OPERATOR
        //   Ensure that the value of the object can be formatted appropriately
        //   on an 'ostream' in some standard, human-readable form.
        //
        // Concerns:
        //: 1 The 'print' method writes the value to the specified 'ostream'.
        //:
        //: 2 The 'print' method writes the value in the intended format.
        //:
        //: 3 The output using 's << obj' is the same as 'obj.print(s, 0, -1)',
        //:   but with each "attributeName = " elided.
        //:
        //: 4 The 'print' method signature and return type are standard.
        //:
        //: 5 The 'print' method returns the supplied 'ostream'.
        //:
        //: 6 The optional 'level' and 'spacesPerLevel' parameters have the
        //:   correct default values.
        //:
        //: 7 The output 'operator<<' signature and return type are standard.
        //:
        //: 8 The output 'operator<<' returns the supplied 'ostream'.
        //
        // Plan:
        //: 1 Use the addresses of the 'print' member function and 'operator<<'
        //:   free function defined in this component to initialize,
        //:   respectively, member-function and free-function pointers having
        //:   the appropriate signatures and return types.  (C-4, 7)
        //:
        //: 2 Using the table-driven technique:  (C-1..3, 5..6, 8)
        //:
        //:   1 Define fourteen carefully selected combinations of (two) object
        //:     values ('A' and 'B'), having distinct values for each
        //:     corresponding salient attribute, and various values for the
        //:     two formatting parameters, along with the expected output.
        //:
        //:     ( 'value' x  'level'   x 'spacesPerLevel' ):
        //:     1 { A   } x {  0     } x {  0, 1, -1, -8 } --> 3 expected o/ps
        //:     2 { A   } x {  3, -3 } x {  0, 2, -2, -8 } --> 6 expected o/ps
        //:     3 { B   } x {  2     } x {  3            } --> 1 expected o/p
        //:     4 { A B } x { -8     } x { -8            } --> 2 expected o/ps
        //:     5 { A B } x { -9     } x { -9            } --> 2 expected o/ps
        //:
        //:   2 For each row in the table defined in P-2.1:  (C-1..3, 5..6, 8)
        //:
        //:     1 Using a 'const' 'Obj', supply each object value and pair of
        //:       formatting parameters to 'print', omitting the 'level' or
        //:       'spacesPerLevel' parameter if the value of that argument is
        //:       '-8'.  If the parameters are, arbitrarily, (-9, -9), then
        //:       invoke the 'operator<<' instead.
        //:
        //:     2 Use a standard 'ostringstream' to capture the actual output.
        //:
        //:     3 Verify the address of what is returned is that of the
        //:       supplied stream.  (C-5, 8)
        //:
        //:     4 Compare the contents captured in P-2.2.2 with what is
        //:       expected.  (C-1..3, 6)
        //
        // Testing:
        //   ostream& print(ostream& s, int level = 0, int sPL = 4) const;
        //   ostream& operator<<(ostream& s, const Config& d);
        //   CONCERN: All accessor methods are declared 'const'.
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "PRINT AND OUTPUT OPERATOR" << endl
                          << "=========================" << endl;

        if (verbose) cout << "\nAssign the addresses of 'print' and "
                             "the output 'operator<<' to variables." << endl;
        {
            typedef ostream& (Obj::*funcPtr)(ostream&, int, int) const;
            typedef ostream& (*operatorPtr)(ostream&, const Obj&);

            // Verify that the signatures and return types are standard.

            funcPtr     printMember = &Obj::print;
            operatorPtr operatorOp  = bdlt::operator<<;

            (void)printMember;  // quash potential compiler warnings
            (void)operatorOp;
        }

        if (verbose) cout <<
             "\nCreate a table of distinct value/format combinations." << endl;

        static const struct {
            int         d_line;           // source line number
            int         d_level;
            int         d_spacesPerLevel;

            bool        d_omitColonInZoneDesignator;
            bool        d_useCommaForDecimalSign;
            bool        d_useZAbbreviationForUtc;

            const char *d_expected_p;
        } DATA[] = {

#define NL "\n"
#define SP " "

        // ------------------------------------------------------------------
        // P-2.1.1: { A } x { 0 }     x { 0, 1, -1, -8 } -->  4 expected o/ps
        // ------------------------------------------------------------------

        //LINE L SPL  OMIT   SIGN   USEZ   EXP
        //---- - ---  ----   ----   ----   ---

        { L_,  0,  0, true,  true,  true,
                                          "["                                NL
                                          "omitColonInZoneDesignator = true" NL
                                          "useCommaForDecimalSign = true"    NL
                                          "useZAbbreviationForUtc = true"    NL
                                          "]"                                NL
                                                                             },

        { L_,  0,  1, true,  true,  true,
                                         "["                                 NL
                                         " omitColonInZoneDesignator = true" NL
                                         " useCommaForDecimalSign = true"    NL
                                         " useZAbbreviationForUtc = true"    NL
                                         "]"                                 NL
                                                                             },

        { L_,  0, -1, true,  true,  true, "["                                SP
                                          "omitColonInZoneDesignator = true" SP
                                          "useCommaForDecimalSign = true"    SP
                                          "useZAbbreviationForUtc = true"    SP
                                          "]"
                                                                             },

        { L_,  0, -8, true,  true,  true,
                                      "["                                    NL
                                      "    omitColonInZoneDesignator = true" NL
                                      "    useCommaForDecimalSign = true"    NL
                                      "    useZAbbreviationForUtc = true"    NL
                                      "]"                                    NL
                                                                             },

        // ------------------------------------------------------------------
        // P-2.1.2: { A } x { 3, -3 } x { 0, 2, -2, -8 } -->  6 expected o/ps
        // ------------------------------------------------------------------

        //LINE L SPL  OMIT   SIGN   USEZ   EXP
        //---- - ---  ----   ----   ----   ---

        { L_,  3,  0, true,  true,  true, "["                                NL
                                          "omitColonInZoneDesignator = true" NL
                                          "useCommaForDecimalSign = true"    NL
                                          "useZAbbreviationForUtc = true"    NL
                                          "]"                                NL
                                                                             },

        { L_,  3,  2, true,  true,  true,
                                  "      ["                                  NL
                                  "        omitColonInZoneDesignator = true" NL
                                  "        useCommaForDecimalSign = true"    NL
                                  "        useZAbbreviationForUtc = true"    NL
                                  "      ]"                                  NL
                                                                             },

        { L_,  3, -2, true,  true,  true, "      ["                          SP
                                          "omitColonInZoneDesignator = true" SP
                                          "useCommaForDecimalSign = true"    SP
                                          "useZAbbreviationForUtc = true"    SP
                                          "]"
                                                                             },

        { L_,  3, -8, true,  true,  true,
                          "            ["                                    NL
                          "                omitColonInZoneDesignator = true" NL
                          "                useCommaForDecimalSign = true"    NL
                          "                useZAbbreviationForUtc = true"    NL
                          "            ]"                                    NL
                                                                             },

        { L_, -3,  0, true,  true,  true, "["                                NL
                                          "omitColonInZoneDesignator = true" NL
                                          "useCommaForDecimalSign = true"    NL
                                          "useZAbbreviationForUtc = true"    NL
                                          "]"                                NL
                                                                             },

        { L_, -3,  2, true,  true,  true,
                                  "["                                        NL
                                  "        omitColonInZoneDesignator = true" NL
                                  "        useCommaForDecimalSign = true"    NL
                                  "        useZAbbreviationForUtc = true"    NL
                                  "      ]"                                  NL
                                                                             },

        { L_, -3, -2, true,  true,  true,
                                        "["                                  SP
                                        "omitColonInZoneDesignator = true"   SP
                                        "useCommaForDecimalSign = true"      SP
                                        "useZAbbreviationForUtc = true"      SP
                                        "]"
                                                                             },
        { L_, -3, -8, true,  true,  true,
                          "["                                                NL
                          "                omitColonInZoneDesignator = true" NL
                          "                useCommaForDecimalSign = true"    NL
                          "                useZAbbreviationForUtc = true"    NL
                          "            ]"                                    NL
                                                                             },

        // -----------------------------------------------------------------
        // P-2.1.3: { B } x { 2 }     x { 3 }            -->  1 expected o/p
        // -----------------------------------------------------------------

        //LINE L SPL  OMIT   SIGN   USEZ   EXP
        //---- - ---  ----   ----   ----   ---

        { L_,  2,  3, false, false, false,
                                "      ["                                    NL
                                "         omitColonInZoneDesignator = false" NL
                                "         useCommaForDecimalSign = false"    NL
                                "         useZAbbreviationForUtc = false"    NL
                                "      ]"                                    NL
                                                                             },

        // -----------------------------------------------------------------
        // P-2.1.4: { A B } x { -8 }   x { -8 }         -->  2 expected o/ps
        // -----------------------------------------------------------------

        //LINE L SPL  OMIT   SIGN   USEZ   EXP
        //---- - ---  ----   ----   ----   ---

        { L_, -8, -8, true,  true,  true,
                                      "["                                    NL
                                      "    omitColonInZoneDesignator = true" NL
                                      "    useCommaForDecimalSign = true"    NL
                                      "    useZAbbreviationForUtc = true"    NL
                                      "]"                                    NL
                                                                             },

        { L_, -8, -8, false, false, false,
                                     "["                                     NL
                                     "    omitColonInZoneDesignator = false" NL
                                     "    useCommaForDecimalSign = false"    NL
                                     "    useZAbbreviationForUtc = false"    NL
                                     "]"                                     NL
                                                                             },
        // -----------------------------------------------------------------
        // P-2.1.5: { A B } x { -9 }   x { -9 }         -->  2 expected o/ps
        // -----------------------------------------------------------------

        //LINE L SPL  OMIT   SIGN   USEZ   EXP
        //---- - ---  ----   ----   ----   ---

        { L_, -9, -9, true,  true,  true,  "[ true true true ]"              },

        { L_, -9, -9, false, false, false, "[ false false false ]"           },

#undef NL
#undef SP

        };
        const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

        if (verbose) cout << "\nTesting with various print specifications."
                          << endl;
        {
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int         LINE = DATA[ti].d_line;
                const int         L    = DATA[ti].d_level;
                const int         SPL  = DATA[ti].d_spacesPerLevel;
                const bool        OMIT = DATA[ti].d_omitColonInZoneDesignator;
                const bool        SIGN = DATA[ti].d_useCommaForDecimalSign;
                const bool        USEZ = DATA[ti].d_useZAbbreviationForUtc;
                const char *const EXP  = DATA[ti].d_expected_p;

                if (veryVerbose) { T_ P_(L) P_(SPL) P_(OMIT) P_(SIGN) P(USEZ) }

                if (veryVeryVerbose) { T_ T_ Q(EXPECTED) cout << EXP; }

                Obj mX;  const Obj& X = mX;
                mX.setOmitColonInZoneDesignator(OMIT);
                mX.setUseCommaForDecimalSign(SIGN);
                mX.setUseZAbbreviationForUtc(USEZ);

                bslma::TestAllocator oa("object", veryVeryVeryVerbose);

                ostringstream os(&oa);

                // Verify supplied stream is returned by reference.

                if (-9 == L && -9 == SPL) {
                    ASSERTV(LINE, &os == &(os << X));

                    if (veryVeryVerbose) { T_ T_ Q(operator<<) }
                }
                else {
                    ASSERTV(LINE, -8 == SPL || -8 != L);

                    if (-8 != SPL) {
                        ASSERTV(LINE, &os == &X.print(os, L, SPL));
                    }
                    else if (-8 != L) {
                        ASSERTV(LINE, &os == &X.print(os, L));
                    }
                    else {
                        ASSERTV(LINE, &os == &X.print(os));
                    }

                    if (veryVeryVerbose) { T_ T_ Q(print) }
                }

                {
                    bslma::TestAllocator da("default", veryVeryVeryVerbose);
                    bslma::DefaultAllocatorGuard dag(&da);

                    // Verify output is formatted as expected.

                    if (veryVeryVerbose) { P(os.str()) }

                    ASSERTV(LINE, EXP, os.str(), EXP == os.str());
                }
            }
        }

      } break;
      case 4: {
        // --------------------------------------------------------------------
        // BASIC ACCESSORS
        //   Ensure each basic accessor properly interprets object state.
        //
        // Concerns:
        //: 1 Each of the three basic accessors returns the value of the
        //:   corresponding attribute of the object.
        //:
        //: 2 Each basic accessor method is declared 'const'.
        //
        // Plan:
        //: 1 Using the table-driven technique, specify a set of distinct
        //:   object values (one per row) in terms of their attribute values.
        //:
        //: 2 For each row 'R' in the table of P-1:  (C-1..2)
        //:
        //:   1 Use the default constructor and primary manipulators to create
        //:     a 'const' object, 'X', having the value from 'R'.
        //:
        //:   2 Verify that each basic accessor, invoked on 'X', returns the
        //:     expected value.  (C-1..2)
        //
        // Testing:
        //   bool omitColonInZoneDesignator() const;
        //   bool useCommaForDecimalSign() const;
        //   bool useZAbbreviationForUtc() const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BASIC ACCESSORS" << endl
                          << "===============" << endl;

        if (verbose) cout <<
                "\nVerify all basic accessors report expected values." << endl;

        if (verbose) cout << "\nUse a table of distinct object values."
                          << endl;

        const int              NUM_DATA        = NUM_DEFAULT_DATA;
        const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int  LINE      = DATA[ti].d_line;
            const bool OMITCOLON = DATA[ti].d_omitColon;
            const bool USECOMMA  = DATA[ti].d_useComma;
            const bool USEZ      = DATA[ti].d_useZ;

            if (veryVerbose) {
                T_ P_(LINE) P_(OMITCOLON) P_(USECOMMA) P(USEZ)
            }

            Obj mX;  const Obj& X = mX;

            mX.setOmitColonInZoneDesignator(OMITCOLON);
            mX.setUseCommaForDecimalSign(USECOMMA);
            mX.setUseZAbbreviationForUtc(USEZ);

            ASSERTV(LINE, OMITCOLON == X.omitColonInZoneDesignator());
            ASSERTV(LINE, USECOMMA  == X.useCommaForDecimalSign());
            ASSERTV(LINE, USEZ      == X.useZAbbreviationForUtc());
        }

      } break;
      case 3: {
        // --------------------------------------------------------------------
        // GENERATOR FUNCTION 'gg'
        //   There is no 'gg' function for this component.
        //
        // Concerns:
        //   N/A
        //
        // Plan:
        //   N/A
        //
        // Testing:
        //   Reserved for 'gg' generator function.
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "GENERATOR FUNCTION 'gg'" << endl
                          << "=======================" << endl;

        if (verbose) {
            cout << "No 'gg' function for 'bdlt::Iso8601UtilConfiguration'."
                 << endl;
        }

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // DEFAULT CTOR, PRIMARY MANIPULATORS, & DTOR
        //   Ensure that we can use the default constructor to create an
        //   object (having the default-constructed value), use the primary
        //   manipulators to put that object into any state relevant for
        //   thorough testing, and use the destructor to destroy it safely.
        //
        // Concerns:
        //: 1 An object created with the default constructor has the
        //:   contractually specified default value.
        //:
        //: 2 Each attribute is modifiable independently.
        //:
        //: 3 Each attribute can be set to represent any value.
        //:
        //: 4 Any argument can be 'const'.
        //
        // Plan:
        //: 1 Create two sets of attribute values for the object: ('D') values
        //:   corresponding to the default-constructed object and ('A') values
        //:   distinct from those corresponding to the default-constructed
        //:   object.
        //:
        //: 2 Use the default constructor to create an object 'X'.
        //:
        //: 3 Use the individual (as yet unproven) salient attribute accessors
        //:   to verify the default-constructed value.  (C-1)
        //:
        //: 4 For each attribute 'i', in turn, create a local block.  Then
        //:   inside the block, using brute force, set that attribute's value,
        //:   passing a 'const' argument representing each of the two test
        //:   values, in turn (see P-1), first to 'Ai', then back to 'Di'.
        //:   After each transition, use the (as yet unproven) basic accessors
        //:   to verify that only the intended attribute value changed.
        //:   (C-3..4)
        //:
        //: 5 Corroborate that attributes are modifiable independently by first
        //:   setting all of the attributes to their 'A' values.  Then
        //:   incrementally set each attribute to it's corresponding 'D' value
        //:   and verify after each manipulation that only that attribute's
        //:   value changed.  (C-2)
        //:
        // Testing:
        //   Iso8601UtilConfiguration();
        //   ~Iso8601UtilConfiguration();
        //   void setOmitColonInZoneDesignator(bool value);
        //   void setUseCommaForDecimalSign(bool value);
        //   void setUseZAbbreviationForUtc(bool value);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                       << "DEFAULT CTOR, PRIMARY MANIPULATORS, & DTOR" << endl
                       << "==========================================" << endl;

        if (verbose) cout << "\nEstablish suitable attribute values." << endl;

        // 'D' values: These are the default-constructed values.

        const bool D1 = false;  // 'omitColonInZoneDesignator'
        const bool D2 = false;  // 'useCommaForDecimalSign'
        const bool D3 = false;  // 'useZAbbreviationForUtc'

        // 'A' values.

        const bool A1 = true;   // 'omitColonInZoneDesignator'
        const bool A2 = true;   // 'useCommaForDecimalSign'
        const bool A3 = true;   // 'useZAbbreviationForUtc'

        {
            Obj mX;  const Obj& X = mX;

            // -------------------------------------
            // Verify the object's attribute values.
            // -------------------------------------

            ASSERTV(D1, X.omitColonInZoneDesignator(),
                    D1 == X.omitColonInZoneDesignator());
            ASSERTV(D2, X.useCommaForDecimalSign(),
                    D2 == X.useCommaForDecimalSign());
            ASSERTV(D3, X.useZAbbreviationForUtc(),
                    D3 == X.useZAbbreviationForUtc());

            // -----------------------------------------------------
            // Verify that each attribute is independently settable.
            // -----------------------------------------------------

            // 'omitColonInZoneDesignator'
            {
                mX.setOmitColonInZoneDesignator(A1);
                ASSERT(A1 == X.omitColonInZoneDesignator());
                ASSERT(D2 == X.useCommaForDecimalSign());
                ASSERT(D3 == X.useZAbbreviationForUtc());

                mX.setOmitColonInZoneDesignator(D1);
                ASSERT(D1 == X.omitColonInZoneDesignator());
                ASSERT(D2 == X.useCommaForDecimalSign());
                ASSERT(D3 == X.useZAbbreviationForUtc());
            }

            // 'useCommaForDecimalSign'
            {
                mX.setUseCommaForDecimalSign(A2);
                ASSERT(D1 == X.omitColonInZoneDesignator());
                ASSERT(A2 == X.useCommaForDecimalSign());
                ASSERT(D3 == X.useZAbbreviationForUtc());

                mX.setUseCommaForDecimalSign(D2);
                ASSERT(D1 == X.omitColonInZoneDesignator());
                ASSERT(D2 == X.useCommaForDecimalSign());
                ASSERT(D3 == X.useZAbbreviationForUtc());
            }

            // 'useZAbbreviationForUtc'
            {
                mX.setUseZAbbreviationForUtc(A3);
                ASSERT(D1 == X.omitColonInZoneDesignator());
                ASSERT(D2 == X.useCommaForDecimalSign());
                ASSERT(A3 == X.useZAbbreviationForUtc());

                mX.setUseZAbbreviationForUtc(D3);
                ASSERT(D1 == X.omitColonInZoneDesignator());
                ASSERT(D2 == X.useCommaForDecimalSign());
                ASSERT(D3 == X.useZAbbreviationForUtc());
            }

            // Corroborate attribute independence.
            {
                // Set all attributes to their 'A' values.

                mX.setOmitColonInZoneDesignator(A1);
                mX.setUseCommaForDecimalSign(A2);
                mX.setUseZAbbreviationForUtc(A3);

                ASSERT(A1 == X.omitColonInZoneDesignator());
                ASSERT(A2 == X.useCommaForDecimalSign());
                ASSERT(A3 == X.useZAbbreviationForUtc());

                // Set all attributes to their 'D' values.

                mX.setOmitColonInZoneDesignator(D1);
                ASSERT(D1 == X.omitColonInZoneDesignator());
                ASSERT(A2 == X.useCommaForDecimalSign());
                ASSERT(A3 == X.useZAbbreviationForUtc());

                mX.setUseCommaForDecimalSign(D2);
                ASSERT(D1 == X.omitColonInZoneDesignator());
                ASSERT(D2 == X.useCommaForDecimalSign());
                ASSERT(A3 == X.useZAbbreviationForUtc());

                mX.setUseZAbbreviationForUtc(D3);
                ASSERT(D1 == X.omitColonInZoneDesignator());
                ASSERT(D2 == X.useCommaForDecimalSign());
                ASSERT(D3 == X.useZAbbreviationForUtc());
            }

            // Let the object go out of scope.
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
        //: 1 Create an object 'w' (default ctor).       { w:D             }
        //: 2 Create an object 'x' (copy from 'w').      { w:D x:D         }
        //: 3 Set 'x' to 'A' (value distinct from 'D').  { w:D x:A         }
        //: 4 Create an object 'y' (init. to 'A').       { w:D x:A y:A     }
        //: 5 Create an object 'z' (copy from 'y').      { w:D x:A y:A z:A }
        //: 6 Set 'z' to 'D' (the default value).        { w:D x:A y:A z:D }
        //: 7 Assign 'w' from 'x'.                       { w:A x:A y:A z:D }
        //: 8 Assign 'w' from 'z'.                       { w:D x:A y:A z:D }
        //: 9 Assign 'x' from 'x' (aliasing).            { w:D x:A y:A z:D }
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BREATHING TEST" << endl
                          << "==============" << endl;

        // Attribute 1 Values: 'omitColonInZoneDesignator'

        const bool D1 = false;  // default value
        const bool A1 = true;

        // Attribute 2 Values: 'useCommaForDecimalSign'

        const bool D2 = false;  // default value
        const bool A2 = true;

        // Attribute 3 Values: 'useZAbbreviationForUtc'

        const bool D3 = false;  // default value
        const bool A3 = true;

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\n 1. Create an object 'w' (default ctor)."
                             "\t\t{ w:D             }" << endl;

        Obj mW;  const Obj& W = mW;

        if (veryVerbose) cout << "\ta. Check initial value of 'w'." << endl;
        if (veryVeryVerbose) { T_ T_ P(W) }

        ASSERT(D1 == W.omitColonInZoneDesignator());
        ASSERT(D2 == W.useCommaForDecimalSign());
        ASSERT(D3 == W.useZAbbreviationForUtc());

        if (veryVerbose) cout <<
                  "\tb. Try equality operators: 'w' <op> 'w'." << endl;

        ASSERT(1 == (W == W));        ASSERT(0 == (W != W));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\n 2. Create an object 'x' (copy from 'w')."
                             "\t\t{ w:D x:D         }" << endl;

        Obj mX(W);  const Obj& X = mX;

        if (veryVerbose) cout << "\ta. Check initial value of 'x'." << endl;
        if (veryVeryVerbose) { T_ T_ P(X) }

        ASSERT(D1 == X.omitColonInZoneDesignator());
        ASSERT(D2 == X.useCommaForDecimalSign());
        ASSERT(D3 == X.useZAbbreviationForUtc());

        if (veryVerbose) cout <<
                   "\tb. Try equality operators: 'x' <op> 'w', 'x'." << endl;

        ASSERT(1 == (X == W));        ASSERT(0 == (X != W));
        ASSERT(1 == (X == X));        ASSERT(0 == (X != X));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\n 3. Set 'x' to 'A' (value distinct from 'D')."
                             "\t\t{ w:D x:A         }" << endl;

        mX.setOmitColonInZoneDesignator(A1);
        mX.setUseCommaForDecimalSign(A2);
        mX.setUseZAbbreviationForUtc(A3);

        if (veryVerbose) cout << "\ta. Check new value of 'x'." << endl;
        if (veryVeryVerbose) { T_ T_ P(X) }

        ASSERT(A1 == X.omitColonInZoneDesignator());
        ASSERT(A2 == X.useCommaForDecimalSign());
        ASSERT(A3 == X.useZAbbreviationForUtc());

        if (veryVerbose) cout <<
             "\tb. Try equality operators: 'x' <op> 'w', 'x'." << endl;

        ASSERT(0 == (X == W));        ASSERT(1 == (X != W));
        ASSERT(1 == (X == X));        ASSERT(0 == (X != X));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\n 4. Create an object 'y' (init. to 'A')."
                             "\t\t{ w:D x:A y:A     }" << endl;

        Obj mY;  const Obj& Y = mY;
        mY.setOmitColonInZoneDesignator(A1);
        mY.setUseCommaForDecimalSign(A2);
        mY.setUseZAbbreviationForUtc(A3);

        if (veryVerbose) cout << "\ta. Check initial value of 'y'." << endl;
        if (veryVeryVerbose) { T_ T_ P(Y) }

        ASSERT(A1 == Y.omitColonInZoneDesignator());
        ASSERT(A2 == Y.useCommaForDecimalSign());
        ASSERT(A3 == Y.useZAbbreviationForUtc());

        if (veryVerbose) cout <<
             "\tb. Try equality operators: 'y' <op> 'w', 'x', 'y'" << endl;

        ASSERT(0 == (Y == W));        ASSERT(1 == (Y != W));
        ASSERT(1 == (Y == X));        ASSERT(0 == (Y != X));
        ASSERT(1 == (Y == Y));        ASSERT(0 == (Y != Y));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\n 5. Create an object 'z' (copy from 'y')."
                             "\t\t{ w:D x:A y:A z:A }" << endl;

        Obj mZ(Y);  const Obj& Z = mZ;

        if (veryVerbose) cout << "\ta. Check initial value of 'z'." << endl;
        if (veryVeryVerbose) { T_ T_ P(Z) }

        ASSERT(A1 == Z.omitColonInZoneDesignator());
        ASSERT(A2 == Z.useCommaForDecimalSign());
        ASSERT(A3 == Z.useZAbbreviationForUtc());

        if (veryVerbose) cout <<
           "\tb. Try equality operators: 'z' <op> 'w', 'x', 'y', 'z'." << endl;

        ASSERT(0 == (Z == W));        ASSERT(1 == (Z != W));
        ASSERT(1 == (Z == X));        ASSERT(0 == (Z != X));
        ASSERT(1 == (Z == Y));        ASSERT(0 == (Z != Y));
        ASSERT(1 == (Z == Z));        ASSERT(0 == (Z != Z));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\n 6. Set 'z' to 'D' (the default value)."
                             "\t\t\t{ w:D x:A y:A z:D }" << endl;

        mZ.setOmitColonInZoneDesignator(D1);
        mZ.setUseCommaForDecimalSign(D2);
        mZ.setUseZAbbreviationForUtc(D3);

        if (veryVerbose) cout << "\ta. Check new value of 'z'." << endl;
        if (veryVeryVerbose) { T_ T_ P(Z) }

        ASSERT(D1 == Z.omitColonInZoneDesignator());
        ASSERT(D2 == Z.useCommaForDecimalSign());
        ASSERT(D3 == Z.useZAbbreviationForUtc());

        if (veryVerbose) cout <<
           "\tb. Try equality operators: 'z' <op> 'w', 'x', 'y', 'z'." << endl;

        ASSERT(1 == (Z == W));        ASSERT(0 == (Z != W));
        ASSERT(0 == (Z == X));        ASSERT(1 == (Z != X));
        ASSERT(0 == (Z == Y));        ASSERT(1 == (Z != Y));
        ASSERT(1 == (Z == Z));        ASSERT(0 == (Z != Z));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\n 7. Assign 'w' from 'x'."
                             "\t\t\t\t{ w:A x:A y:A z:D }" << endl;
        mW = X;

        if (veryVerbose) cout << "\ta. Check new value of 'w'." << endl;
        if (veryVeryVerbose) { T_ T_ P(W) }

        ASSERT(A1 == W.omitColonInZoneDesignator());
        ASSERT(A2 == W.useCommaForDecimalSign());
        ASSERT(A3 == W.useZAbbreviationForUtc());

        if (veryVerbose) cout <<
           "\tb. Try equality operators: 'w' <op> 'w', 'x', 'y', 'z'." << endl;

        ASSERT(1 == (W == W));        ASSERT(0 == (W != W));
        ASSERT(1 == (W == X));        ASSERT(0 == (W != X));
        ASSERT(1 == (W == Y));        ASSERT(0 == (W != Y));
        ASSERT(0 == (W == Z));        ASSERT(1 == (W != Z));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\n 8. Assign 'w' from 'z'."
                             "\t\t\t\t{ w:D x:A y:A z:D }" << endl;
        mW = Z;

        if (veryVerbose) cout << "\ta. Check new value of 'w'." << endl;
        if (veryVeryVerbose) { T_ T_ P(W) }

        ASSERT(D1 == W.omitColonInZoneDesignator());
        ASSERT(D2 == W.useCommaForDecimalSign());
        ASSERT(D3 == W.useZAbbreviationForUtc());

        if (veryVerbose) cout <<
           "\tb. Try equality operators: 'x' <op> 'w', 'x', 'y', 'z'." << endl;

        ASSERT(1 == (W == W));        ASSERT(0 == (W != W));
        ASSERT(0 == (W == X));        ASSERT(1 == (W != X));
        ASSERT(0 == (W == Y));        ASSERT(1 == (W != Y));
        ASSERT(1 == (W == Z));        ASSERT(0 == (W != Z));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\n 9. Assign 'x' from 'x' (aliasing)."
                             "\t\t\t{ w:D x:A y:A z:D }" << endl;
        mX = X;

        if (veryVerbose) cout << "\ta. Check (same) value of 'x'." << endl;
        if (veryVeryVerbose) { T_ T_ P(X) }

        ASSERT(A1 == X.omitColonInZoneDesignator());
        ASSERT(A2 == X.useCommaForDecimalSign());
        ASSERT(A3 == X.useZAbbreviationForUtc());

        if (veryVerbose) cout <<
           "\tb. Try equality operators: 'x' <op> 'w', 'x', 'y', 'z'." << endl;

        ASSERT(0 == (X == W));        ASSERT(1 == (X != W));
        ASSERT(1 == (X == X));        ASSERT(0 == (X != X));
        ASSERT(1 == (X == Y));        ASSERT(0 == (X != Y));
        ASSERT(0 == (X == Z));        ASSERT(1 == (X != Z));

      } break;
      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      }
    }

    // CONCERN: In no case does memory come from the global allocator.

    ASSERTV(globalAllocator.numBlocksTotal(),
            0 == globalAllocator.numBlocksTotal());

    // CONCERN: In no case does memory come from the default allocator.

    ASSERTV(defaultAllocator.numBlocksTotal(),
            0 == defaultAllocator.numBlocksTotal());

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
