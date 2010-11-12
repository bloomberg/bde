// bteso_flag.t.cpp          -*-C++-*-

#include <bteso_flag.h>

#include <bsls_platformutil.h>           // for testing only
#include <bdex_instreamfunctions.h>      // for testing only
#include <bdex_outstreamfunctions.h>     // for testing only
#include <bdex_testinstream.h>           // for testing only
#include <bdex_testoutstream.h>          // for testing only
#include <bdex_testinstreamexception.h>  // for testing only

#include <bsl_cstdlib.h>                       // atoi()
#include <bsl_cstring.h>                       // strcmp(), memcmp() memcpy()
#include <bsl_iostream.h>

#if defined(BDES_PLATFORMUTIL__NO_LONG_HEADER_NAMES)
#include <strstrea.h>
#else
#include <bsl_strstream.h>
#endif

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
//
// [ 5] STREAMING TEST: enum Flag
// [ 6] STREAMING TEST: enum BlockingMode
// [ 7] STREAMING TEST: enum ShutdownType
// [ 8] STREAMING TEST: enum IOWaitType

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

typedef bdex_TestInStream  In;
typedef bdex_TestOutStream Out;

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
class StreamingTest
    // This template describes a functor which automates the standard
    // STREAMING TEST for enumerated types within a class scope,
    // including the input and output stream operators, 'operator>>'
    // and 'operator<<'.  The enumerated type 'ENUM' has 'd_numEnums'
    // elements, and is described by the generating function 'd_generator'.
    // Both the number of elements and the generating function are passed to
    // the functor constructor.
{
    int          d_verbose;
    int          d_veryVerbose;
    int          d_veryVeryVerbose;
    int          d_numEnums;
    Generator    d_generator;
    int          d_version;

public:
    // CONSTRUCTORS
    StreamingTest(int          verbose,
                  int          veryVerbose,
                  int          veryVeryVerbose,
                  int          numEnums,
                  Generator    generator);
    virtual ~StreamingTest();

    // MANIPULATORS
    void operator()() const;

private:
    StreamingTest(const StreamingTest&);               // not implemented
    StreamingTest& operator=(const StreamingTest&);    // not implemented
};

template <typename ENUM>
StreamingTest<ENUM>::StreamingTest(
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
    if (verbose) cout << endl << "Testing Streaming Operators" << endl
                              << "===========================" << endl;
}

template <typename ENUM>
StreamingTest<ENUM>::~StreamingTest()
{
}

template <typename ENUM>
void StreamingTest<ENUM>::operator()() const
{
    // --------------------------------------------------------------------
    // STREAMING TEST:
    //   The 'bdex' streaming concerns for this 'enum' component are
    //   standard.  We thoroughly test "normal" functionality using the
    //   available bdex stream functions.  We next step through
    //   the sequence of possible "abnormal" stream states (empty, invalid,
    //   incomplete, and corrupted), appropriately selecting data sets as
    //   described below.  In all cases, exception neutrality is confirmed
    //   using the specially instrumented 'bdex_TestInStream' and a pair of
    //   standard macros, 'BEGIN_BDEX_EXCEPTION_TEST' and
    //   'END_BDEX_EXCEPTION_TEST', which configure the
    //   'bdex_TestInStream' object appropriately in a loop.
    //
    // Plan:
    //   Let L represent the number of valid enumerator values.
    //   Let S represent the set of consecutive integers { 0 .. L - 1 }
    //   Let T represent the set of consecutive integers { -1 .. L }
    //
    //   VALID STREAMS
    //     Verify that each valid enumerator value in S can be written to
    //     and successfully read from a valid 'bdex' data stream into an
    //     instance of the enumeration with any initial value in T leaving
    //     the stream in a valid state.
    //
    //   EMPTY AND INVALID STREAMS
    //     For each valid and invalid initial enumerator value in T,
    //     create an instance of the enumeration and attempt to stream
    //     into it from an empty and then invalid stream.  Verify that the
    //     instance has its initial value, and that the stream is invalid.
    //
    //   INCOMPLETE (BUT OTHERWISE VALID) DATA
    //     Write 3 identical valid enumerator values to an output stream
    //     buffer, which will then be of total length N.  For each partial
    //     stream length from 0 to N - 1, construct an input stream and
    //     attempt to read into enumerator instances initialized with 3
    //     other identical values.  Verify values of instances that are
    //     successfully modified, partially modified (and therefore reset
    //     to the default value), or left entirely unmodified.  Also verify
    //     that the stream becomes invalid immediately after the first
    //     incomplete read.
    //
    //   CORRUPTED DATA
    //     Use the underlying stream package to simulate an instance of a
    //     typical valid (control) stream and verify that it can be
    //     streamed in successfully.  Then for each of the two data fields
    //     in the stream (beginning with the version number), provide two
    //     similar tests with the data field corrupted ("too small" and
    //     "too large").  After each test, verify the instance has the
    //     default value, and that the input stream has gone invalid.
    //
    // Testing:
    //   bdexStreamIn(bdex_InStream&, bteso_Flag::ENUM& rhs);
    //   bdexStreamOut(bdex_OutStream&, bteso_Flag::ENUM rhs);
    // --------------------------------------------------------------------

    // The following declarations are needed to make the BDEX_EXCEPTION
    // macros work.

#if !defined(BDE_BUILD_TARGET_EXC)
    int verbose = d_verbose;
#endif
    int veryVerbose = d_veryVerbose;
    int veryVeryVerbose = d_veryVeryVerbose;

    const int VERSION = bteso_Flag::maxSupportedBdexVersion();
    if (d_verbose)
        cout << "\nTesting ('<<') and ('>>') on valid streams and data."
             << endl;
    if (d_verbose)
        cout << "\tFor normal (correct) conditions." << endl;
    {
        for (int i = 0; i < d_numEnums; ++i) {
            const ENUM X = static_cast<ENUM>(d_generator(i));
            if (d_veryVerbose) { P_(i);  P_(d_generator(i));  P(X); }
            Out out;
            bdex_OutStreamFunctions::streamOut(out, X, VERSION);
            const char *const OD  = out.data();
            const int         LOD = out.length();

            // Verify that each new value overwrites every old value
            // and that the input stream is emptied, but remains valid.

            for (int j = 0; j < d_numEnums; ++j) {
                In in(OD, LOD);  In &testInStream = in;
                in.setSuppressVersionCheck(1);
                LOOP2_ASSERT(i, j, in);  LOOP2_ASSERT(i, j, !in.isEmpty());

                ENUM t = static_cast<ENUM>(d_generator(j));
              BEGIN_BDEX_EXCEPTION_TEST { in.reset();
                LOOP2_ASSERT(i, j, X == t == (i == j));
                bdex_InStreamFunctions::streamIn(in, t, VERSION);
              } END_BDEX_EXCEPTION_TEST
                LOOP2_ASSERT(i, j, X == t);
                LOOP2_ASSERT(i, j, in);  LOOP2_ASSERT(i, j, in.isEmpty());
            }
        }
    }

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

    if (d_verbose)
        cout << "\tOn empty and invalid streams." << endl;
    {
        Out out;
        const char *const  OD = out.data();
        const int         LOD = out.length();
        ASSERT(0 == LOD);

        for (int i = -1; i <= d_numEnums; ++i) {
            In in(OD, LOD);      In &testInStream = in;
            in.setSuppressVersionCheck(1);
            LOOP_ASSERT(i, in);  LOOP_ASSERT(i, in.isEmpty());

            // Ensure that reading from an empty or invalid input stream
            // leaves the stream invalid and the target object unchanged if
            // it was initially valid.

            const ENUM X = static_cast<ENUM>(d_generator(i));  ENUM t(X);
            LOOP_ASSERT(i, X == t);
          BEGIN_BDEX_EXCEPTION_TEST { in.reset();
            bdex_InStreamFunctions::streamIn(in, t, VERSION);
            LOOP_ASSERT(i, !in);
            LOOP_ASSERT(i, X == t ||
                                  ((i == -1 || i == d_numEnums) && 0 == t));
            bdex_InStreamFunctions::streamIn(in, t, VERSION);
            LOOP_ASSERT(i, !in);
            LOOP_ASSERT(i, X == t ||
                                  ((i == -1 || i == d_numEnums) && 0 == t));
          } END_BDEX_EXCEPTION_TEST
        }
    }

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

    if (d_verbose)
        cout << "\tOn incomplete (but otherwise valid) data." << endl;
    {
        const ENUM W1 = static_cast<ENUM>(2), X1 = static_cast<ENUM>(1);
        const ENUM W2 = static_cast<ENUM>(1), X2 = static_cast<ENUM>(2);

        Out out;
        bdex_OutStreamFunctions::streamOut(out, X1, VERSION);
        const int LOD1 = out.length();
        bdex_OutStreamFunctions::streamOut(out, X2, VERSION);
        const int LOD  = out.length();
        const char *const     OD   = out.data();

        for (int i = 0; i < LOD; ++i) {
            In in(OD, i);  In &testInStream = in;
            in.setSuppressVersionCheck(1);
          BEGIN_BDEX_EXCEPTION_TEST { in.reset();
            LOOP_ASSERT(i, in); LOOP_ASSERT(i, !i == in.isEmpty());
            ENUM t1(W1), t2(W2);

            if (i < LOD1) {
                bdex_InStreamFunctions::streamIn(in, t1, VERSION);
                LOOP_ASSERT(i, !in);
                                     if (0 == i) LOOP_ASSERT(i, W1 == t1);
                bdex_InStreamFunctions::streamIn(in, t2, VERSION);
                LOOP_ASSERT(i, !in);  LOOP_ASSERT(i, W2 == t2);
            }
            else {
                bdex_InStreamFunctions::streamIn(in, t1, VERSION);
                LOOP_ASSERT(i,  in);  LOOP_ASSERT(i, X1 == t1);
                bdex_InStreamFunctions::streamIn(in, t2, VERSION);
                LOOP_ASSERT(i, !in);
                                  if (LOD1 == i) LOOP_ASSERT(i, W2 == t2);
            }
          } END_BDEX_EXCEPTION_TEST
        }
    }

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

    if (d_verbose)
        cout << "\tOn corrupted data." << endl;

    const ENUM W = static_cast<ENUM>(1), X = static_cast<ENUM>(0);
    ASSERT(d_generator(d_numEnums) > X);
    // If only two enumerators, use Y = X = 1 and remove "ASSERT(Y != t)"s.

    // -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -

    if (d_verbose)
        cout << "\t\tGood stream (for control)." << endl;

    {
        const char enumerator = static_cast<char>(W);

        Out out;
        bdex_OutStreamFunctions::streamOut(out, enumerator, VERSION);
        const char *const OD  = out.data();
        const int         LOD = out.length();

        ENUM t(X);          ASSERT(W != t); ASSERT(X == t);
        In in(OD, LOD);     ASSERT(in);
        in.setSuppressVersionCheck(1);
        bdex_InStreamFunctions::streamIn(in, t, VERSION);
        ASSERT(in);
                            ASSERT(W == t); ASSERT(X != t);
     }

    // -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -

    if (d_verbose)
        cout << "\t\tBad version number." << endl;

    {
        const int version    = 0;          // BAD: too small
        const char enumerator = static_cast<char>(W);

        Out out;
        bdex_OutStreamFunctions::streamOut(out, enumerator, VERSION);
        const char *const OD  = out.data();
        const int         LOD = out.length();

        ENUM t(X);          ASSERT(W != t); ASSERT(X == t);
        In in(OD, LOD);     ASSERT(in);
        in.setQuiet(!d_veryVerbose);
        in.setSuppressVersionCheck(1);
        bdex_InStreamFunctions::streamIn(in, t, version);
        ASSERT(!in);
                            ASSERT(W != t); ASSERT(X == t);
    }
    {
        const int version    = 5;           // BAD: too large
        const char enumerator = static_cast<char>(W);

        Out out;
        bdex_OutStreamFunctions::streamOut(out, enumerator, VERSION);
        const char *const OD  = out.data();
        const int         LOD = out.length();

        ENUM t(X);          ASSERT(W != t); ASSERT(X == t);
        In in(OD, LOD);     ASSERT(in);
        in.setQuiet(!d_veryVerbose);
        in.setSuppressVersionCheck(1);
        bdex_InStreamFunctions::streamIn(in, t, version);
        ASSERT(!in);
                            ASSERT(W != t); ASSERT(X == t);
    }

    // -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -

    if (d_verbose)
        cout << "\t\tBad enumerator value." << endl;

    {
        const char enumerator = static_cast<char>(-1);     // BAD: too small

        Out out;
        bdex_OutStreamFunctions::streamOut(out, enumerator, VERSION);
        const char *const OD =  out.data();
        const int         LOD = out.length();

        ENUM t(X);          ASSERT(W != t); ASSERT(X == t);
        In in(OD, LOD);     ASSERT(in);
        in.setSuppressVersionCheck(1);
        bdex_InStreamFunctions::streamIn(in, t, VERSION);
        ASSERT(!in);
                            ASSERT(W != t); ASSERT(X == t);
    }
    {
        const char enumerator = static_cast<char>(d_generator(d_numEnums));
                                                                // BAD: too big

        Out out;
        bdex_OutStreamFunctions::streamOut(out, enumerator, VERSION);
        const char *const OD  = out.data();
        const int         LOD = out.length();

        ENUM t(X);          ASSERT(W != t); ASSERT(X == t);
        In in(OD, LOD);     ASSERT(in);
        in.setSuppressVersionCheck(1);
        bdex_InStreamFunctions::streamIn(in, t, VERSION);
        ASSERT(!in);
                            ASSERT(W != t); ASSERT(X == t);
    }
}

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
      case 8: {
        // --------------------------------------------------------------------
        // STREAMING TEST:
        // --------------------------------------------------------------------

        const int NUM_ENUMS = 3;
        StreamingTest<bteso_Flag::IOWaitType> streamingTest(VERBOSITY,
                                                            NUM_ENUMS,
                                                            Util::identity);
        if (verbose) cout << "gen = Util::identity" << endl;
        streamingTest();
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // STREAMING TEST:
        // --------------------------------------------------------------------

        const int NUM_ENUMS = 3;
        StreamingTest<bteso_Flag::ShutdownType> streamingTest(VERBOSITY,
                                                              NUM_ENUMS,
                                                              Util::identity);
        if (verbose) cout << "gen = Util::identity" << endl;
        streamingTest();
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // STREAMING TEST:
        // --------------------------------------------------------------------

        const int NUM_ENUMS = 2;
        StreamingTest<bteso_Flag::BlockingMode> streamingTest(VERBOSITY,
                                                              NUM_ENUMS,
                                                              Util::identity);
        if (verbose) cout << "gen = Util::identity" << endl;
        streamingTest();
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // STREAMING TEST:
        // --------------------------------------------------------------------

        const int NUM_ENUMS = bteso_Flag::BTESO_NFLAGS;
        StreamingTest<bteso_Flag::Flag> streamingTest(VERBOSITY,
                                                      NUM_ENUMS,
                                                      Util::twoToTheN);
        if (verbose) cout << "gen = Util::twoToTheN" << endl;
        streamingTest();
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // VALUE TEST:
        // --------------------------------------------------------------------

        ValueTest<bteso_Flag::IOWaitType>::Data DATA[] = {
            // Enumerated Value                 String Representation
            // --------------------------       --------------------------
            { bteso_Flag::BTESO_IO_READ,              "IO_READ" },
            { bteso_Flag::BTESO_IO_WRITE,             "IO_WRITE" },
            { bteso_Flag::BTESO_IO_RW,                "IO_RW" },
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
            { bteso_Flag::BTESO_SHUTDOWN_RECEIVE,     "SHUTDOWN_RECEIVE" },
            { bteso_Flag::BTESO_SHUTDOWN_SEND,        "SHUTDOWN_SEND" },
            { bteso_Flag::BTESO_SHUTDOWN_BOTH,        "SHUTDOWN_BOTH" },
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
            { bteso_Flag::BTESO_BLOCKING_MODE,        "BLOCKING_MODE" },
            { bteso_Flag::BTESO_NONBLOCKING_MODE,     "NONBLOCKING_MODE" },
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
            { bteso_Flag::BTESO_ASYNC_INTERRUPT,      "ASYNC_INTERRUPT" },
        };

        const int DATA_LENGTH = sizeof DATA / sizeof *DATA;
        ValueTest<bteso_Flag::Flag> valueTest(VERBOSITY,
                                              DATA_LENGTH,
                                              Util::twoToTheN);
        if (verbose)
            cout << "\nVerify table length is correct." << endl;
        ASSERT(DATA_LENGTH == bteso_Flag::BTESO_NFLAGS);
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
