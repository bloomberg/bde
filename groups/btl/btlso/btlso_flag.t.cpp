// btlso_flag.t.cpp          -*-C++-*-

#include <btlso_flag.h>

#include <bsl_cstdlib.h>                       // atoi()
#include <bsl_cstring.h>                       // strcmp(), memcmp() memcpy()
#include <bsl_iostream.h>
#include <bsl_strstream.h>

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// Standard enumeration test plan.  The VALUE and STREAMING tests for
// bteso_Flag enumerated types are each implemented by a templated functor to
// guarantee uniformity and ease maintenance.
//-----------------------------------------------------------------------------
// [ 1] VALUE TEST: enum Flag
// [ 2] VALUE TEST: enum BlockingMode
// [ 3] VALUE TEST: enum ShutdownType
// [ 4] VALUE TEST: enum IOWaitType

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
            return n;
        }
        return 1 << n;
    }
};


//-----------------------------------------------------------------------------
template <typename ENUM>
class ValueTest
    // This template describes a functor which automates the standard
    // VALUE TEST for enumerated types within a class scope,
    // including the output stream operator, 'operator<<'.
{
    int          d_verbose;
    int          d_veryVerbose;
    int          d_veryVeryVerbose;
    int          d_numEnums;
    Generator    d_generator;

public:
    struct Data {
        ENUM          d_enum;       // enumerated value
        const char   *d_ascii_p;    // string representation
    };

    // CONSTRUCTORS
    ValueTest(int          verbose,
              int          veryVerbose,
              int          veryVeryVerbose,
              int          numEnums,
              Generator    generator);
    virtual ~ValueTest();

    // MANIPULATORS
    void operator()(const Data *DATA) const;

private:
    ValueTest(const ValueTest&);               // not implemented
    ValueTest& operator=(const ValueTest&);    // not implemented
};

template <typename ENUM>
ValueTest<ENUM>::ValueTest(
    int          verbose,
    int          veryVerbose,
    int          veryVeryVerbose,
    int          numEnums,
    Generator    generator)
: d_verbose(verbose)
, d_veryVerbose(veryVerbose)
, d_veryVeryVerbose(veryVeryVerbose)
, d_numEnums(numEnums)
, d_generator(generator)
{
    ASSERT(d_numEnums);
    ASSERT(d_generator);

    if (d_verbose)
        cout << endl << "VALUE TEST" << endl
                     << "==========" << endl;
}

template <typename ENUM>
ValueTest<ENUM>::~ValueTest()
{
}

template <typename ENUM>
void ValueTest<ENUM>::operator()(const Data *DATA) const
{
    // --------------------------------------------------------------------
    // VALUE TEST:
    //   The enumerators in this component are consecutive integers in the
    //   sequence generated by the function 'd_generator'.
    //   Verify that
    //   the 'toAscii' function produces strings that are identical to
    //   their respective enumerator symbols.  Verify that the output
    //   operator produces the same respective string values that would
    //   be produced by 'toAscii'.  Also verify the ascii representation
    //   and 'ostream' output for invalid enumerator values.
    //
    // Testing:
    //   enum Enum { ... };
    //   static const char *toAscii(bteso_Flag::ENUM value);
    //   operator<<(ostream&, bteso_Flag::ENUM rhs)
    // --------------------------------------------------------------------

    ASSERT(DATA);
    ASSERT(d_numEnums);

    const char *const UNKNOWN_FMT = "(* UNKNOWN *)";

    if (d_verbose)
        cout << "\nVerify enumerator values are sequential." << endl;

    for (int i = 0; i < d_numEnums; ++i) {
        LOOP_ASSERT(i, DATA[i].d_enum == d_generator(i));
    }

    if (d_verbose)
        cout << "\nVerify the toAscii function." << endl;

    for (int i = -1; i < d_numEnums + 1; ++i)    // also check UNKNOWN_FMT
    {
        const char *const FMT = 0 <= i && i < d_numEnums
                              ? DATA[i].d_ascii_p : UNKNOWN_FMT;
        if (d_veryVerbose)
            cout << "EXPECTED FORMAT: " << FMT << endl;
        const char *const ACT =
            bteso_Flag::toAscii(static_cast<ENUM>(d_generator(i)));
        if (d_veryVerbose)
            cout << "  ACTUAL FORMAT: " << ACT << endl;

        LOOP_ASSERT(i, 0 == strcmp(FMT, ACT));
        for (int j = 0; j < i; ++j) {  // make sure ALL strings are unique
            LOOP2_ASSERT(i, j, 0 != strcmp(DATA[j].d_ascii_p, FMT));
        }
    }

    if (d_verbose)
        cout << "\nVerify the output (<<) operator." << endl;

    const int SIZE = 64;              // big enough to hold output string
    const char XX = (char) 0xff;      // used to represent an unset char
    char mCtrlBuf[SIZE];
    const char *CTRL_BUF = mCtrlBuf;  // used to check for extra characters

    memset(mCtrlBuf, XX, SIZE);
    for (int i = -1; i < d_numEnums + 1; ++i)    // also check UNKNOWN_FMT
    {
        char buf[SIZE];
        memcpy(buf, CTRL_BUF, SIZE);  // preset buf to 'unset' char values

        const char *const FMT = 0 <= i && i < d_numEnums
                                ? DATA[i].d_ascii_p : UNKNOWN_FMT;
        if (d_veryVerbose)
            cout << "EXPECTED FORMAT: " << FMT << endl;
        ostrstream out(buf, sizeof buf);
        out << static_cast<ENUM>(d_generator(i)) << ends;
        if (d_veryVerbose)
            cout << "  ACTUAL FORMAT: " << buf << endl;

        const int SZ = strlen(FMT) + 1;
        LOOP_ASSERT(i, SZ < SIZE);            // check buffer is large enough
        LOOP_ASSERT(i, XX == buf[SIZE - 1]);  // check for overrun
        LOOP_ASSERT(i, 0 == memcmp(buf, FMT, SZ));
        LOOP_ASSERT(i, 0 == memcmp(buf + SZ, CTRL_BUF + SZ, SIZE - SZ));
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
      case 4: {
        // --------------------------------------------------------------------
        // VALUE TEST:
        // --------------------------------------------------------------------

        ValueTest<bteso_Flag::IOWaitType>::Data DATA[] = {
            // Enumerated Value                 String Representation
            // --------------------------       --------------------------
            { bteso_Flag::e_IO_READ,              "IO_READ" },
            { bteso_Flag::e_IO_WRITE,             "IO_WRITE" },
            { bteso_Flag::e_IO_RW,                "IO_RW" },
        };

        const int DATA_LENGTH = sizeof DATA / sizeof *DATA;
        ValueTest<bteso_Flag::IOWaitType> valueTest(VERBOSITY,
                                                    DATA_LENGTH,
                                                    Util::identity);
        valueTest(DATA);
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // VALUE TEST:
        // --------------------------------------------------------------------

        ValueTest<bteso_Flag::ShutdownType>::Data DATA[] = {
            // Enumerated Value                 String Representation
            // --------------------------       --------------------------
            { bteso_Flag::e_SHUTDOWN_RECEIVE,     "SHUTDOWN_RECEIVE" },
            { bteso_Flag::e_SHUTDOWN_SEND,        "SHUTDOWN_SEND" },
            { bteso_Flag::e_SHUTDOWN_BOTH,        "SHUTDOWN_BOTH" },
        };

        const int DATA_LENGTH = sizeof DATA / sizeof *DATA;
        ValueTest<bteso_Flag::ShutdownType> valueTest(VERBOSITY,
                                                      DATA_LENGTH,
                                                      Util::identity);
        valueTest(DATA);
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // VALUE TEST:
        // --------------------------------------------------------------------

        ValueTest<bteso_Flag::BlockingMode>::Data DATA[] = {
            // Enumerated Value                 String Representation
            // --------------------------       --------------------------
            { bteso_Flag::e_BLOCKING_MODE,        "BLOCKING_MODE" },
            { bteso_Flag::e_NONBLOCKING_MODE,     "NONBLOCKING_MODE" },
        };

        const int DATA_LENGTH = sizeof DATA / sizeof *DATA;
        ValueTest<bteso_Flag::BlockingMode> valueTest(VERBOSITY,
                                                      DATA_LENGTH,
                                                      Util::identity);
        valueTest(DATA);
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // VALUE TEST:
        // --------------------------------------------------------------------

        ValueTest<bteso_Flag::Flag>::Data DATA[] = {
            // Enumerated Value                 String Representation
            // --------------------------       --------------------------
            { bteso_Flag::k_ASYNC_INTERRUPT,      "ASYNC_INTERRUPT" },
        };

        const int DATA_LENGTH = sizeof DATA / sizeof *DATA;
        ValueTest<bteso_Flag::Flag> valueTest(VERBOSITY,
                                              DATA_LENGTH,
                                              Util::twoToTheN);
        if (verbose)
            cout << "\nVerify table length is correct." << endl;
        ASSERT(DATA_LENGTH == bteso_Flag::k_NFLAGS);
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

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2004
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
