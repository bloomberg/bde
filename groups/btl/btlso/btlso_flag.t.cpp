// btlso_flag.t.cpp                                                   -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------


#include <btlso_flag.h>

#include <bsl_cstdlib.h>                       // atoi()
#include <bsl_cstring.h>                       // strcmp(), memcmp() memcpy()
#include <bsl_iostream.h>
#include <bsl_sstream.h>

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// Standard enumeration test plan.  The VALUE and STREAMING tests for
// btlso::Flag enumerated types are each implemented by a templated functor to
// guarantee uniformity and ease maintenance.
//-----------------------------------------------------------------------------
// [ 1] VALUE TEST: enum Flag
// [ 2] VALUE TEST: enum BlockingMode
// [ 3] VALUE TEST: enum ShutdownType
// [ 4] VALUE TEST: enum IOWaitType
// ----------------------------------------------------------------------------
// [ 5] USAGE EXAMPLE

//=============================================================================
//                  STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------

static int testStatus = 0;

namespace {

void aSsErT(int c, const char *s, int i) {
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}

}  // close unnamed namespace

#define ASSERT(X) { ::aSsErT(!(X), #X, __LINE__); }
//-----------------------------------------------------------------------------
#define LOOP_ASSERT(I,X) { \
    if (!(X)) { cout << #I << ": " << I << "\n"; ::aSsErT(1, #X, __LINE__);}}

#define LOOP2_ASSERT(I,J,X) { \
    if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " \
              << J << "\n"; ::aSsErT(1, #X, __LINE__); } }

//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", "<< flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number
#define T_()  cout << "\t" << flush;          // Print tab w/o newline

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

typedef int (*Generator)(int);

//=============================================================================
//                  HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

struct Util {

    static inline int identity(int n)
        // Return the value n
    {
        return n;
    }

    static inline int twoToTheN(int n)
        // Return the value of 2 raised to the power of the specified 'n' if
        // n >= 0, and return the (negative) value of 'n' otherwise.
    {
        if (n < 0)
        {
            return n;                                                 // RETURN
        }
        return 1 << n;
    }
};

//-----------------------------------------------------------------------------

template <class ENUM>
class ValueTest
    // This template describes a functor which automates the standard VALUE
    // TEST for enumerated types within a class scope, including the output
    // stream operator, 'operator<<'.
{
    int       d_verbose;
    int       d_veryVerbose;
    int       d_veryVeryVerbose;
    int       d_numEnums;
    Generator d_generator;

    ValueTest(const ValueTest&);               // not implemented
    ValueTest& operator=(const ValueTest&);    // not implemented

  public:
    struct Data {
        ENUM        d_enum;       // enumerated value
        const char *d_ascii_p;    // string representation
    };

    // CONSTRUCTORS
    ValueTest(int       verbose,
              int       veryVerbose,
              int       veryVeryVerbose,
              int       numEnums,
              Generator generator);
    virtual ~ValueTest();

    // MANIPULATORS
    void operator()(const Data *DATA) const;
};

template <class ENUM>
ValueTest<ENUM>::ValueTest(int       verbose,
                           int       veryVerbose,
                           int       veryVeryVerbose,
                           int       numEnums,
                           Generator generator)
: d_verbose(verbose)
, d_veryVerbose(veryVerbose)
, d_veryVeryVerbose(veryVeryVerbose)
, d_numEnums(numEnums)
, d_generator(generator)
{
    ASSERT(d_numEnums);
    ASSERT(d_generator);

    if (d_verbose) cout << endl << "VALUE TEST" << endl
                                << "==========" << endl;
}

template <class ENUM>
ValueTest<ENUM>::~ValueTest()
{
}

template <class ENUM>
void ValueTest<ENUM>::operator()(const Data *DATA) const
{
    // --------------------------------------------------------------------
    // VALUE TEST:
    // The enumerators in this component are consecutive integers in the
    // sequence generated by the function 'd_generator'.  Verify that the
    // 'toAscii' function produces strings that are identical to their
    // respective enumerator symbols.  Verify that the output operator produces
    // the same respective string values that would be produced by 'toAscii'.
    // Also verify the ascii representation and 'ostream' output for invalid
    // enumerator values.
    //
    // Testing:
    //   enum Enum { ... };
    //   static const char *toAscii(btlso::Flag::ENUM value);
    //   operator<<(ostream&, btlso::Flag::ENUM rhs)
    // --------------------------------------------------------------------

    ASSERT(DATA);
    ASSERT(d_numEnums);

    const char *const UNKNOWN_FMT = "(* UNKNOWN *)";

    if (d_verbose) cout << "\nVerify enum values are sequential." << endl;

    for (int i = 0; i < d_numEnums; ++i) {
        LOOP_ASSERT(i, DATA[i].d_enum == d_generator(i));
    }

    if (d_verbose) cout << "\nVerify the toAscii function." << endl;

    for (int i = -1; i < d_numEnums + 1; ++i) {    // also check UNKNOWN_FMT
        const char *const FMT = 0 <= i && i < d_numEnums
                              ? DATA[i].d_ascii_p : UNKNOWN_FMT;

        if (d_veryVerbose) cout << "EXPECTED FORMAT: " << FMT << endl;

        const char *const ACT = btlso::Flag::toAscii(
                                            static_cast<ENUM>(d_generator(i)));

        if (d_veryVerbose) cout << "  ACTUAL FORMAT: " << ACT << endl;

        LOOP_ASSERT(i, 0 == strcmp(FMT, ACT));

        for (int j = 0; j < i; ++j) {  // make sure ALL strings are unique
            LOOP2_ASSERT(i, j, 0 != strcmp(DATA[j].d_ascii_p, FMT));
        }
    }

    if (d_verbose) cout << "\nVerify the output (<<) operator." << endl;

    for (int i = -1; i < d_numEnums + 1; ++i) {    // also check UNKNOWN_FMT
        const char *const FMT = 0 <= i && i < d_numEnums
                                ? DATA[i].d_ascii_p : UNKNOWN_FMT;

        if (d_veryVerbose) cout << "EXPECTED FORMAT: " << FMT << endl;

        ostringstream out;

        ASSERT(&out == &(out << static_cast<ENUM>(d_generator(i))));

        if (d_veryVerbose) cout << "  ACTUAL FORMAT: " << out.str() << endl;

        ASSERT(FMT == out.str());
    }
}

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;
    int veryVeryVerbose = argc > 4;

#define VERBOSITY    verbose, veryVerbose, veryVeryVerbose

    cout << "TEST " << __FILE__ << " CASE " << test << endl;;

    switch (test) { case 0:
      case 5: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //
        // Concerns:
        //: 1 The usage example provided in the component header file must
        //:   compile, link, and run as shown.
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

        if (verbose) cout << endl << "USAGE EXAMPLE" << endl
                                  << "=============" << endl;

///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Basic Syntax
///- - - - - - - - - - - -
// The following snippets of code provide a simple illustration of using
// one of the enumerations defined in this component, 'btlso::BlockingMode'.
//
// First, we create a variable 'value' of type 'btlso::Flag::BlockingMode' and
// initialize it with the enumerator value 'btlso::Flag::e_NONBLOCKING_MODE':
//..
    btlso::Flag::BlockingMode value = btlso::Flag::e_NONBLOCKING_MODE;
//..
// Now, we store the address of its ASCII representation in a pointer variable,
// 'asciiValue', of type 'const char *':
//..
    const char *asciiValue = btlso::Flag::toAscii(value);
    ASSERT(0 == bsl::strcmp(asciiValue, "NONBLOCKING_MODE"));
//..
// Finally, we print 'value' to 'bsl::cout'.
//..
if (veryVerbose)
     bsl::cout << value << bsl::endl;
//..
// This statement produces the following output on 'stdout':
//..
//  NONBLOCKING_MODE
//..
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // VALUE TEST: 'btlso::Flag::IOWaitType' enumerators
        // --------------------------------------------------------------------

        ValueTest<btlso::Flag::IOWaitType>::Data DATA[] = {
            // Enumerated Value                 String Representation
            // --------------------------       --------------------------
            { btlso::Flag::e_IO_READ,              "IO_READ" },
            { btlso::Flag::e_IO_WRITE,             "IO_WRITE" },
            { btlso::Flag::e_IO_RW,                "IO_RW" },
        };

        const int DATA_LENGTH = sizeof DATA / sizeof *DATA;
        ValueTest<btlso::Flag::IOWaitType> valueTest(VERBOSITY,
                                                     DATA_LENGTH,
                                                     Util::identity);
        valueTest(DATA);
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // VALUE TEST: 'btlso::Flag::ShutdownType' enumerators
        // --------------------------------------------------------------------

        ValueTest<btlso::Flag::ShutdownType>::Data DATA[] = {
            // Enumerated Value                 String Representation
            // --------------------------       --------------------------
            { btlso::Flag::e_SHUTDOWN_RECEIVE,     "SHUTDOWN_RECEIVE" },
            { btlso::Flag::e_SHUTDOWN_SEND,        "SHUTDOWN_SEND" },
            { btlso::Flag::e_SHUTDOWN_BOTH,        "SHUTDOWN_BOTH" },
        };

        const int DATA_LENGTH = sizeof DATA / sizeof *DATA;
        ValueTest<btlso::Flag::ShutdownType> valueTest(VERBOSITY,
                                                       DATA_LENGTH,
                                                       Util::identity);

        valueTest(DATA);
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // VALUE TEST: 'btlso::Flag::BlockingMode' enumerators
        // --------------------------------------------------------------------

        ValueTest<btlso::Flag::BlockingMode>::Data DATA[] = {
            // Enumerated Value                 String Representation
            // --------------------------       --------------------------
            { btlso::Flag::e_BLOCKING_MODE,        "BLOCKING_MODE" },
            { btlso::Flag::e_NONBLOCKING_MODE,     "NONBLOCKING_MODE" },
        };

        const int DATA_LENGTH = sizeof DATA / sizeof *DATA;
        ValueTest<btlso::Flag::BlockingMode> valueTest(VERBOSITY,
                                                       DATA_LENGTH,
                                                       Util::identity);

        valueTest(DATA);
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // VALUE TEST: 'btlso::Flag::Flag' enumerators
        // --------------------------------------------------------------------

        ValueTest<btlso::Flag::FlagType>::Data DATA[] = {
            // Enumerated Value                 String Representation
            // --------------------------       --------------------------
            { btlso::Flag::k_ASYNC_INTERRUPT,   "ASYNC_INTERRUPT" },
        };

        const int DATA_LENGTH = sizeof DATA / sizeof *DATA;
        ValueTest<btlso::Flag::FlagType> valueTest(VERBOSITY,
                                                   DATA_LENGTH,
                                                   Util::twoToTheN);

        if (verbose) cout << "\nVerify table length is correct." << endl;

        ASSERT(DATA_LENGTH == btlso::Flag::k_NFLAGS);
        valueTest(DATA);

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

#undef VERBOSITY
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
