// bael_loggermanagerdefaults.t.cpp  -*-C++-*-

#include <bael_loggermanagerdefaults.h>
#include <bdex_testoutstream.h>                 // for testing only
#include <bdex_testinstream.h>                  // for testing only
#include <bdex_testinstreamexception.h>         // for testing only
#include <bdex_instreamfunctions.h>             // for testing only
#include <bdex_outstreamfunctions.h>            // for testing only
#include <bdex_byteoutstream.h>                 // for testing only
#include <bdex_byteinstream.h>                  // for testing only

#include <bsl_string.h>
#include <bsl_cstring.h>     // strlen()
#include <bsl_cstdlib.h>     // atoi()
#include <bsl_iostream.h>
#include <bsl_strstream.h>

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

//=============================================================================
//                                 TEST PLAN
//-----------------------------------------------------------------------------
//                                 Overview
//                                 --------
// This test plan follows the standard approach for components implementing a
// constrained-attribute type.
//
// In the test plan documentation below, 'bael_LMD' is occasionally used for
// 'bael_LoggerManagerDefaults' for brevity.
//
//-----------------------------------------------------------------------------
// [ 3] static int maxSupportedBdexVersion();
// [ 1] bael_LoggerManagerDefaults();
// [ 1] bael_LoggerManagerDefaults(const bael_LoggerManagerDefaults&);
// [ 1] ~bael_LoggerManagerDefaults();
// [ 1] bael_LMD& operator=(const bael_LoggerManagerDefaults& rhs);
// [ 2] int setDefaultRecordBufferSizeIfValid(int numBytes);
// [ 2] int setDefaultLoggerBufferSizeIfValid(int numBytes);
// [ 2] int setDefaultThresholdLevelsIfValid(int, int, int, int);
// [ 3] bdex_InStream& streamIn(bdex_InStream& stream, int version);
// [ 1] int defaultRecordBufferSize() const;
// [ 1] int defaultLoggerBufferSize() const;
// [ 1] int defaultRecordLevel() const;
// [ 1] int defaultPassLevel() const;
// [ 1] int defaultTriggerLevel() const;
// [ 1] int defaultTriggerAllLevel() const;
// [ 3] bdex_OutStream& streamOut(bdex_OutStream& stream, int version) const;
//
// [ 1] bool operator==(const bael_LMD& lhs, const bael_LMD& rhs);
// [ 1] bool operator!=(const bael_LMD& lhs, const bael_LMD& rhs);
// [ 1] bsl::ostream& operator<<(bsl::ostream&, const bael_LMD& rhs);
//-----------------------------------------------------------------------------
// [ 4] USAGE EXAMPLE
//-----------------------------------------------------------------------------

//=============================================================================
//                      STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
static int testStatus = 0;

static void aSsErT(int c, const char *s, int i)
{
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (0 <= testStatus && testStatus <= 100) ++testStatus;
    }
}

#define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

//=============================================================================
//                  STANDARD BDE LOOP-ASSERT TEST MACROS
//-----------------------------------------------------------------------------
#define LOOP_ASSERT(I,X) { \
   if (!(X)) { cout << #I << ": " << I << "\n"; aSsErT(1, #X, __LINE__); }}

#define LOOP2_ASSERT(I,J,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " \
              << J << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP3_ASSERT(I,J,K,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" \
              << #K << ": " << K << "\n"; aSsErT(1, #X, __LINE__); } }

//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", "<< flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number
#define NL "\n"

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

typedef bael_LoggerManagerDefaults Obj;
typedef bdex_TestInStream          In;
typedef bdex_TestOutStream         Out;

const int NUM_VALUES = 7;

// The 0th element in each array contains the value set by the default
// constructor for the corresponding attribute.

// NOTE: In this test driver only, these values must be written as explicit
// literals to provide an independent representation.  These values MUST NOT
// be copied to other test drivers.  Use the 'Obj::defaultDefault...' class
// methods in other test drivers.

const int RECORD_BUFFER_SIZE[NUM_VALUES] = { 32768, 10, 20, 30, 40, 50, 60 };
const int LOGGER_BUFFER_SIZE[NUM_VALUES] = {  8192, 11, 21, 31, 41, 51, 61 };
const int RECORD_LEVEL[NUM_VALUES]       = {     0, 12, 22, 32, 42, 52, 62 };
const int PASS_LEVEL[NUM_VALUES]         = {    64, 13, 23, 33, 43, 53, 63 };
const int TRIGGER_LEVEL[NUM_VALUES]      = {     0, 14, 24, 34, 44, 54, 64 };
const int TRIGGER_ALL_LEVEL[NUM_VALUES]  = {     0, 15, 25, 35, 45, 55, 65 };

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;
    int veryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;;

    switch (test) { case 0:  // Zero is always the leading case.
      case 4: {
        // --------------------------------------------------------------------
        // TESTING USAGE EXAMPLE
        //   The usage example provided in the component header file must
        //   compile, link, and run on all platforms as shown.
        //
        // Plan:
        //   Incorporate usage example from header into driver, remove leading
        //   comment characters, and replace 'assert' with 'ASSERT'.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Usage Example"
                          << "\n=====================" << endl;

        bael_LoggerManagerDefaults lmd;

        ASSERT(    0 == lmd.setDefaultRecordBufferSizeIfValid(32768));
        ASSERT(32768 == lmd.defaultRecordBufferSize());

        ASSERT(   0 == lmd.setDefaultLoggerBufferSizeIfValid(2048));
        ASSERT(2048 == lmd.defaultLoggerBufferSize());

        ASSERT(  0 == lmd.setDefaultThresholdLevelsIfValid(192, 64, 48, 32));
        ASSERT(192 == lmd.defaultRecordLevel());
        ASSERT( 64 == lmd.defaultPassLevel());
        ASSERT( 48 == lmd.defaultTriggerLevel());
        ASSERT( 32 == lmd.defaultTriggerAllLevel());
//..
// The configuration object is now validly configured with our choice of
// parameters.  If, however, we attempt to set an invalid configuration, the
// "set" method will fail (with a non-zero return status), and the
// configuration will be left unchanged.
//..
        ASSERT(  0 != lmd.setDefaultThresholdLevelsIfValid(256, 90, 60, 30));
        ASSERT(192 == lmd.defaultRecordLevel());
        ASSERT( 64 == lmd.defaultPassLevel());
        ASSERT( 48 == lmd.defaultTriggerLevel());
        ASSERT( 32 == lmd.defaultTriggerAllLevel());
//..
// Finally, we can print the configuration value to 'stdout'.
//..
        if (verbose) {
            cout << "Executing: bsl::cout << lmd << bsl::endl;" << endl;
            bsl::cout << lmd << bsl::endl;
        }

//..
// This produces the following (multi-line) output:
//..
// [
//     recordBufferSize : 32768
//     loggerBufferSize : 2048
//     recordLevel      : 192
//     passLevel        : 64
//     triggerLevel     : 48
//     triggerAllLevel  : 32
// ]
        // Programmatically verify the output format
        char buf[10000];
        {
            ostrstream o(buf, sizeof buf);
            o << lmd << ends;
            bsl::string s =
                "[" NL
                "    recordBufferSize : 32768" NL
                "    loggerBufferSize : 2048"  NL
                "    recordLevel      : 192"   NL
                "    passLevel        : 64"    NL
                "    triggerLevel     : 48"    NL
                "    triggerAllLevel  : 32"    NL
                "]" NL
                ;
            ASSERT(buf == s);
        }

      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING 'bdex' STREAMING FUNCTIONALITY:
        //
        // Concerns:
        //   The 'bdex' streaming concerns for this component are standard.  We
        //   first test the class method 'maxSupportedBdexVersion' and then
        //   use that method to probe the member functions 'bdexStreamOut' and
        //   'bdexStreamIn' in the manner of a "breathing test" to verify basic
        //   functionality.  We then thoroughly test streaming functionality
        //   relying on the available bdex stream functions provided by the
        //   concrete streams, which forward appropriate calls to the member
        //   functions of this component.  We next step through the sequence
        //   of possible stream states (valid, empty, invalid, incomplete, and
        //   corrupted), appropriately selecting data sets as described below.
        //   In all cases, exception neutrality is confirmed using the
        //   specially instrumented 'bdex_TestInStream' and a pair of standard
        //   macros, 'BEGIN_BDEX_EXCEPTION_TEST' and
        //   'END_BDEX_EXCEPTION_TEST', which configure the
        //   'bdex_TestInStream' object appropriately in a loop.
        //
        // Plan:
        //   PRELIMINARY MEMBER FUNCTION TEST
        //     First test 'maxSupportedBdexVersion' explicitly, and then
        //     perform a trivial direct test of the 'outStream' and 'inStream'
        //     methods (the rest of the testing will use the stream operators).
        //
        //   VALID STREAMS
        //     For the set S of globally-defined test values, use all
        //     combinations (u, v) in the cross product S X S, stream the
        //     value of v into (a temporary copy of) u and assert u == v.
        //
        //   EMPTY AND INVALID STREAMS
        //     For each u in S, create a copy and attempt to stream into it
        //     from an empty and then invalid stream.  Verify after each try
        //     that the object is unchanged and that the stream is invalid.
        //
        //   INCOMPLETE (BUT OTHERWISE VALID) DATA
        //     Write 3 distinct objects to an output stream buffer of total
        //     length N.  For each partial stream length from 0 to N - 1,
        //     construct an input stream and attempt to read into objects
        //     initialized with distinct values.  Verify values of objects
        //     that are either successfully modified or left entirely
        //     unmodified, and that the stream became invalid immediately after
        //     the first incomplete read.  Finally ensure that each object
        //     streamed into is in some valid state by assigning it a distinct
        //     new value and testing for equality.
        //
        //   CORRUPTED DATA
        //     Use the underlying stream package to simulate an instance of a
        //     typical valid (control) stream and verify that it can be
        //     streamed in successfully.  Then for each data field in the
        //     stream (beginning with the version number), provide one or more
        //     similar tests with that data field corrupted.  After each test,
        //     verify that the object is in some valid state after streaming,
        //     and that the input stream has gone invalid.
        //
        //   Finally, tests of the explicit wire format will be performed.
        //
        // Testing:
        //   int maxSupportedBdexVersion() const;
        //   bdex_InStream& streamIn(bdex_InStream& stream);
        //   bdex_InStream& streamIn(bdex_InStream& stream, int version);
        //   bdex_OutStream& streamOut(bdex_OutStream& stream) const;
        //   bdex_OutStream& streamOut(bdex_OutStream& stream, int) const;
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Streaming Functionality"
                          << "\n===============================" << endl;

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        // scalar and array object values for various stream tests
        Obj va, vb, vc, vd, ve, vf;
        vb.setDefaultRecordBufferSizeIfValid(RECORD_BUFFER_SIZE[1]);
        vb.setDefaultLoggerBufferSizeIfValid(LOGGER_BUFFER_SIZE[1]);
        vb.setDefaultThresholdLevelsIfValid(RECORD_LEVEL[1],  PASS_LEVEL[1],
                                       TRIGGER_LEVEL[1], TRIGGER_ALL_LEVEL[1]);
        vc.setDefaultRecordBufferSizeIfValid(RECORD_BUFFER_SIZE[2]);
        vc.setDefaultLoggerBufferSizeIfValid(LOGGER_BUFFER_SIZE[2]);
        vc.setDefaultThresholdLevelsIfValid(RECORD_LEVEL[2],  PASS_LEVEL[2],
                                     TRIGGER_LEVEL[2], TRIGGER_ALL_LEVEL[2]);
        vd.setDefaultRecordBufferSizeIfValid(RECORD_BUFFER_SIZE[3]);
        vd.setDefaultLoggerBufferSizeIfValid(LOGGER_BUFFER_SIZE[3]);
        vd.setDefaultThresholdLevelsIfValid(RECORD_LEVEL[3],  PASS_LEVEL[3],
                                     TRIGGER_LEVEL[3], TRIGGER_ALL_LEVEL[3]);
        ve.setDefaultRecordBufferSizeIfValid(RECORD_BUFFER_SIZE[1]);
        ve.setDefaultLoggerBufferSizeIfValid(LOGGER_BUFFER_SIZE[1]);
        ve.setDefaultThresholdLevelsIfValid(RECORD_LEVEL[4],  PASS_LEVEL[4],
                                     TRIGGER_LEVEL[4], TRIGGER_ALL_LEVEL[4]);
        vf.setDefaultRecordBufferSizeIfValid(RECORD_BUFFER_SIZE[1]);
        vf.setDefaultLoggerBufferSizeIfValid(LOGGER_BUFFER_SIZE[1]);
        vf.setDefaultThresholdLevelsIfValid(RECORD_LEVEL[5],  PASS_LEVEL[5],
                                     TRIGGER_LEVEL[5], TRIGGER_ALL_LEVEL[5]);

        const Obj VA;
        const Obj VB(vb);
        const Obj VC(vc);
        const Obj VD(vd);
        const Obj VE(ve);
        const Obj VF(vf);

        const int NUM_VALUES = 6;
        const Obj VALUES[NUM_VALUES] = { VA, VB, VC, VD, VE, VF };
        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout <<
            "\nTesting 'streamOut' and (valid) 'streamIn' functionality."
                          << endl;

        if (verbose) cout << "\nTesting 'maxSupportedBdexVersion' method."
                          << endl;
        {
            if (verbose) cout << "\tusing object syntax:" << endl;
            const Obj X;
            ASSERT(1 == X.maxSupportedBdexVersion());
            if (verbose) cout << "\tusing class method syntax:" << endl;
            ASSERT(1 == Obj::maxSupportedBdexVersion());
        }

        const int VERSION = Obj::maxSupportedBdexVersion();
        if (verbose) cout << "\nDirect initial trial of 'streamOut' and"
                             " (valid) 'streamIn' functionality." << endl;
        {
            const Obj X(VC);
            Out       out;

            X.bdexStreamOut(out, VERSION);

            const char *const OD  = out.data();
            const int         LOD = out.length();
            In                in(OD, LOD);              ASSERT(in);
                                                        ASSERT(!in.isEmpty());
            Obj               t(VA);                    ASSERT(X != t);

            in.setSuppressVersionCheck(1);  // needed for direct method test
            t.bdexStreamIn(in, VERSION);                ASSERT(X == t);
            ASSERT(in);                                 ASSERT(in.isEmpty());
        }

        if (verbose) cout <<
            "\nThorough test using stream operators ('<<' and '>>')." << endl;
        {
            for (int i = 0; i < NUM_VALUES; ++i) {
                const Obj X(VALUES[i]);
                Out out;
                bdex_OutStreamFunctions::streamOut(out, X, VERSION);
                const char *const OD  = out.data();
                const int         LOD = out.length();

                // Verify that each new value overwrites every old value
                // and that the input stream is emptied, but remains valid.

                for (int j = 0; j < NUM_VALUES; ++j) {
                    In in(OD, LOD);  In &testInStream = in;
                    in.setSuppressVersionCheck(1);
                    LOOP2_ASSERT(i, j, in);  LOOP2_ASSERT(i, j, !in.isEmpty());

                    Obj t(VALUES[j]);
                  BEGIN_BDEX_EXCEPTION_TEST { in.reset();
                    LOOP2_ASSERT(i, j, X == t == (i == j));
                    bdex_InStreamFunctions::streamIn(in, t, VERSION);
                  } END_BDEX_EXCEPTION_TEST
                    LOOP2_ASSERT(i, j, X == t);
                    LOOP2_ASSERT(i, j, in);  LOOP2_ASSERT(i, j, in.isEmpty());
                }
            }
        }

        if (verbose) cout <<
            "\nTesting streamIn functionality via operator ('>>')." << endl;

        if (verbose) cout << "\tOn empty and invalid streams." << endl;
        {
            Out out;
            const char *const  OD = out.data();
            const int         LOD = out.length();
            ASSERT(0 == LOD);

            for (int i = 0; i < NUM_VALUES; ++i) {
                In in(OD, LOD);  In &testInStream = in;
                in.setSuppressVersionCheck(1);
                LOOP_ASSERT(i, in);           LOOP_ASSERT(i, in.isEmpty());

                // Ensure that reading from an empty or invalid input stream
                // leaves the stream invalid and the target object unchanged.

                const Obj X(VALUES[i]);  Obj t(X);  LOOP_ASSERT(i, X == t);
              BEGIN_BDEX_EXCEPTION_TEST { in.reset();
                bdex_InStreamFunctions::streamIn(in, t, VERSION);
                LOOP_ASSERT(i, !in);     LOOP_ASSERT(i, X == t);
                bdex_InStreamFunctions::streamIn(in, t, VERSION);
                LOOP_ASSERT(i, !in);     LOOP_ASSERT(i, X == t);
              } END_BDEX_EXCEPTION_TEST
            }
        }

        if (verbose) cout <<
            "\tOn incomplete (but otherwise valid) data." << endl;
        {
            const Obj W1 = VA, X1 = VB, Y1 = VC;
            const Obj W2 = VB, X2 = VC, Y2 = VD;
            const Obj W3 = VC, X3 = VD, Y3 = VE;

            // Note: This object does not produce a special "error" value on
            //       stream corruption, and so this test block is modified from
            //       the "canonical form".
            //const Obj E;  // error value for invalidated stream

            Out out;
            bdex_OutStreamFunctions::streamOut(out, X1, VERSION);
            const int LOD1 = out.length();
            bdex_OutStreamFunctions::streamOut(out, X2, VERSION);
            const int LOD2 = out.length();
            bdex_OutStreamFunctions::streamOut(out, X3, VERSION);
            const int LOD  = out.length();
            const char *const OD = out.data();

            for (int i = 0; i < LOD; ++i) {
                In in(OD, i);  In &testInStream = in;
                in.setSuppressVersionCheck(1);
              BEGIN_BDEX_EXCEPTION_TEST { in.reset();
                LOOP_ASSERT(i, in); LOOP_ASSERT(i, !i == in.isEmpty());
                Obj t1(W1), t2(W2), t3(W3);

                if (i < LOD1) {
                    bdex_InStreamFunctions::streamIn(in, t1, VERSION);
                    LOOP_ASSERT(i, !in);
                                                     LOOP_ASSERT(i, W1 == t1);
                                     //if (0 == i) { LOOP_ASSERT(i, W1 == t1);}
                                     //if (2 <= i) { LOOP_ASSERT(i, E  == t1);}
                    bdex_InStreamFunctions::streamIn(in, t2, VERSION);
                    LOOP_ASSERT(i, !in);  LOOP_ASSERT(i, W2 == t2);
                    bdex_InStreamFunctions::streamIn(in, t3, VERSION);
                    LOOP_ASSERT(i, !in);  LOOP_ASSERT(i, W3 == t3);
                }
                else if (i < LOD2) {
                    bdex_InStreamFunctions::streamIn(in, t1, VERSION);
                    LOOP_ASSERT(i,  in);  LOOP_ASSERT(i, X1 == t1);
                    bdex_InStreamFunctions::streamIn(in, t2, VERSION);
                    LOOP_ASSERT(i, !in);
                                                     LOOP_ASSERT(i, W2 == t2);
                               //if (LOD1 == i)    { LOOP_ASSERT(i, W2 == t2);}
                               //if (LOD1 + 1 < i) { LOOP_ASSERT(i, E  == t2);}
                    bdex_InStreamFunctions::streamIn(in, t3, VERSION);
                    LOOP_ASSERT(i, !in);  LOOP_ASSERT(i, W3 == t3);
                }
                else {
                    bdex_InStreamFunctions::streamIn(in, t1, VERSION);
                    LOOP_ASSERT(i,  in);  LOOP_ASSERT(i, X1 == t1);
                    bdex_InStreamFunctions::streamIn(in, t2, VERSION);
                    LOOP_ASSERT(i,  in);  LOOP_ASSERT(i, X2 == t2);
                    bdex_InStreamFunctions::streamIn(in, t3, VERSION);
                    LOOP_ASSERT(i, !in);
                                                     LOOP_ASSERT(i, W3 == t3);
                               //if (LOD2 == i)    { LOOP_ASSERT(i, W3 == t3);}
                               //if (LOD2 + 1 < i) { LOOP_ASSERT(i, E  == t3);}
                }

                                LOOP_ASSERT(i, Y1 != t1);
                t1 = Y1;        LOOP_ASSERT(i, Y1 == t1);

                                LOOP_ASSERT(i, Y2 != t2);
                t2 = Y2;        LOOP_ASSERT(i, Y2 == t2);

                                LOOP_ASSERT(i, Y3 != t3);
                t3 = Y3;        LOOP_ASSERT(i, Y3 == t3);
              } END_BDEX_EXCEPTION_TEST
            }
        }

        if (verbose) cout << "\tOn corrupted data." << endl;

        const Obj W;                // default value
        const Obj X(VB);            // original (control) value
        const Obj Y(VC);            // new (streamed-out) value

        const int RECBUF_SIZE = 20;    // internal representation of Obj Y
        const int MSGBUF_SIZE = 21;
        const int REC_LEV     = 22;
        const int PAS_LEV     = 23;
        const int TRG_LEV     = 24;
        const int TRA_LEV     = 25;

        if (verbose) cout << "\t\tGood stream (for control)." << endl;
        {
            Out out;
            out.putInt32(RECBUF_SIZE);            // Stream out "new" value.
            out.putInt32(MSGBUF_SIZE);
            out.putInt32(REC_LEV);
            out.putInt32(PAS_LEV);
            out.putInt32(TRG_LEV);
            out.putInt32(TRA_LEV);
            const char *const OD  = out.data();
            const int         LOD = out.length();

            Obj t(X);        ASSERT(W != t);  ASSERT(X == t);  ASSERT(Y != t);
            In in(OD, LOD);  ASSERT(in);
            in.setSuppressVersionCheck(1);
            bdex_InStreamFunctions::streamIn(in, t, VERSION);
            ASSERT(in);
                             ASSERT(W != t);  ASSERT(X != t);  ASSERT(Y == t);
        }

        if (verbose) cout << "\t\tBad version." << endl;
        {
            const int version = 0; // too small ('version' must be >= 1)

            Out out;
            out.putInt32(RECBUF_SIZE);            // Stream out "new" value.
            out.putInt32(MSGBUF_SIZE);
            out.putInt32(REC_LEV);
            out.putInt32(PAS_LEV);
            out.putInt32(TRG_LEV);
            out.putInt32(TRA_LEV);

            const char *const OD  = out.data();
            const int         LOD = out.length();

            Obj t(X);        ASSERT(W != t);  ASSERT(X == t);  ASSERT(Y != t);
            In in(OD, LOD);  ASSERT(in);
            in.setSuppressVersionCheck(1);
            in.setQuiet(!veryVerbose);
            bdex_InStreamFunctions::streamIn(in, t, version);
            ASSERT(!in);
                             ASSERT(W != t);  ASSERT(X == t);  ASSERT(Y != t);
        }
        {
            const int version = 5 ; // too large (current Date version is 1)

            Out out;
            out.putInt32(RECBUF_SIZE);            // Stream out "new" value.
            out.putInt32(MSGBUF_SIZE);
            out.putInt32(REC_LEV);
            out.putInt32(PAS_LEV);
            out.putInt32(TRG_LEV);
            out.putInt32(TRA_LEV);

            const char *const OD  = out.data();
            const int         LOD = out.length();

            Obj t(X);        ASSERT(W != t);  ASSERT(X == t);  ASSERT(Y != t);
            In in(OD, LOD);  ASSERT(in);
            in.setSuppressVersionCheck(1);
            in.setQuiet(!veryVerbose);
            bdex_InStreamFunctions::streamIn(in, t, version);
            ASSERT(!in);
                             ASSERT(W != t);  ASSERT(X == t);  ASSERT(Y != t);
        }
        if (verbose) cout << "\t\tGood version but bad date." << endl;
        {
            Out out;
            out.putInt32(0);             // Stream out invalid date value.
            out.putInt32(MSGBUF_SIZE);
            out.putInt32(REC_LEV);
            out.putInt32(PAS_LEV);
            out.putInt32(TRG_LEV);
            out.putInt32(TRA_LEV);
            const char *const OD  = out.data();
            const int         LOD = out.length();

            Obj t(X);        ASSERT(W != t);  ASSERT(X == t);  ASSERT(Y != t);
            In in(OD, LOD);  ASSERT(in);
            in.setSuppressVersionCheck(1);
            bdex_InStreamFunctions::streamIn(in, t, VERSION);
            ASSERT(!in);
        }
        if (verbose) cout << "\t\tGood version but bad date." << endl;
        {
            Out out;
            out.putInt32(RECBUF_SIZE);   // Stream out invalid date value.
            out.putInt32(0);             // <== BAD
            out.putInt32(REC_LEV);
            out.putInt32(PAS_LEV);
            out.putInt32(TRG_LEV);
            out.putInt32(TRA_LEV);
            const char *const OD  = out.data();
            const int         LOD = out.length();

            Obj t(X);        ASSERT(W != t);  ASSERT(X == t);  ASSERT(Y != t);
            In in(OD, LOD);  ASSERT(in);
            in.setSuppressVersionCheck(1);
            bdex_InStreamFunctions::streamIn(in, t, VERSION);
            ASSERT(!in);
        }
        if (verbose) cout << "\t\tGood version but bad date." << endl;
        {
            Out out;
            out.putInt32(RECBUF_SIZE);    // Stream out invalid date value.
            out.putInt32(MSGBUF_SIZE);
            out.putInt32(256);            // <== BAD
            out.putInt32(PAS_LEV);
            out.putInt32(TRG_LEV);
            out.putInt32(TRA_LEV);
            const char *const OD  = out.data();
            const int         LOD = out.length();

            Obj t(X);        ASSERT(W != t);  ASSERT(X == t);  ASSERT(Y != t);
            In in(OD, LOD);  ASSERT(in);
            in.setSuppressVersionCheck(1);
            bdex_InStreamFunctions::streamIn(in, t, VERSION);
            ASSERT(!in);
        }
#if 0
        if (verbose) cout << "\nWire format direct tests." << endl;
        {
            static const struct {
                int         d_lineNum;         // source line number
                int         d_bufferSize;      // specification buffer size
                int         d_recordLevel;     // specification record level
                int         d_passLevel;       // specification pass level
                int         d_triggerLevel;    // specified trigger level
                int         d_triggerAllLevel; // specified trigAll level
                int         d_version;         // version to stream with
                int         d_length;          // expect output length
                const char *d_fmt_p;           // expected output format
            } DATA[] = {
    //----------^
    //line  bufSz rec  pass  trg  trAll  ver  len  format
    //----  ----  ---  ----  ---  -----  ---  ---  ------------------
    { L_,   1024,  10,   20,  30,    40,   0,   0,  ""                    },
    { L_,   2048,  80,   70,  60,    50,   5,   0,  ""                    },

    { L_,   1024, 128,   96,  64,    32,   1,  20,  "\x00\x00\x08\x00"
                                                    "\x00\x00\x00\x80"
                                                    "\x00\x00\x00\x60"
                                                    "\x00\x00\x00\x40"
                                                    "\x00\x00\x00\x20"    },
    { L_,   2048, 240,  192, 128,    64,   1,  20,  "\x00\x00\x0f\x00"
                                                    "\x00\x00\x00\xf0"
                                                    "\x00\x00\x00\xc0"
                                                    "\x00\x00\x00\x80"
                                                    "\x00\x00\x00\x40"    },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int LINE         = DATA[i].d_lineNum;
                const int BUF_SZ       = DATA[i].d_bufferSize;
                const int REC          = DATA[i].d_recordLevel;
                const int PASS         = DATA[i].d_passLevel;
                const int TRIG         = DATA[i].d_triggerLevel;
                const int TR_ALL       = DATA[i].d_triggerAllLevel;
                const int VERSION      = DATA[i].d_version;
                const int LEN          = DATA[i].d_length;
                const char *const FMT  = DATA[i].d_fmt_p;

                Obj mX;  const Obj& X = mX;
                bdex_ByteOutStream out;  X.bdexStreamOut(out, VERSION);

                LOOP_ASSERT(LINE, LEN == out.length());
                LOOP_ASSERT(LINE, 0 == memcmp(out.data(),
                                              FMT,
                                              LEN));

                if (verbose && memcmp(out.data(), FMT, LEN)) {
                    const char *hex = "0123456789abcdef";
                    P_(LINE);
                    for (int j = 0; j < out.length(); ++j) {
                        cout << "\\x"
                             << hex[(unsigned char)*
                                          (out.data() + j) >> 4]
                             << hex[(unsigned char)*
                                       (out.data() + j) & 0x0f];
                    }
                    cout << endl;
                }

                Obj mY;  const Obj& Y = mY;
                if (LEN) { // version is supported
                    bdex_ByteInStream in(out.data(),
                                         out.length());
                    mY.bdexStreamIn(in, VERSION);
                }
                else { // version is not supported
                    mY = X;
                    bdex_ByteInStream in;
                    mY.bdexStreamIn(in, VERSION);
                    LOOP_ASSERT(LINE, !in);
                }
                LOOP_ASSERT(LINE, X == Y);
            }
        }
#endif
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // CONSTRAINTS TEST
        //
        // Concerns:
        //
        //
        //
        // Plan:
        //
        //
        //
        // Testing:
        //    bool isValidDefaultRecordBufferSize(int numBytes);
        //    bool isValidDefaultLoggerBufferSize(int numBytes);
        //    bool areValidDefaultThresholdLevels(int, int, int, int);
        //    int setDefaultRecordBufferSizeIfValid(int numBytes);
        //    int setDefaultLoggerBufferSizeIfValid(int numBytes);
        //    int setDefaultThresholdLevels(int, int, int, int);
        //    int defaultRecordBufferSize();
        //    int defaultLoggerBufferSize();
        //    int defaultRecordLevel();
        //    int defaultPassLevel();
        //    int defaultTriggerLevel();
        //    int defaultTriggerAllLevel();
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Constraints Test" << endl
                          << "================" << endl;

        Obj mX1; const Obj& X1 = mX1;

        if (verbose)
            cout << "\t Check defaultRecordBufferSize contraint. " << endl;
        {
            ASSERT(0 == Obj::isValidDefaultRecordBufferSize(0));
            ASSERT(1 == Obj::isValidDefaultRecordBufferSize(1));
            ASSERT(1 == Obj::isValidDefaultRecordBufferSize(2));

            ASSERT(0 != mX1.setDefaultRecordBufferSizeIfValid(0));
            ASSERT(RECORD_BUFFER_SIZE[0] == X1.defaultRecordBufferSize());
            ASSERT(0 == mX1.setDefaultRecordBufferSizeIfValid(1));
            ASSERT(1 == X1.defaultRecordBufferSize());
            ASSERT(0 == mX1.setDefaultRecordBufferSizeIfValid(2));
            ASSERT(2 == X1.defaultRecordBufferSize());
        }
        if (verbose)
            cout << "\t Check defaultLoggerBufferSize contraint. " << endl;
        {
            ASSERT(0 == Obj::isValidDefaultLoggerBufferSize(0));
            ASSERT(1 == Obj::isValidDefaultLoggerBufferSize(1));
            ASSERT(1 == Obj::isValidDefaultLoggerBufferSize(2));

            ASSERT(0 != mX1.setDefaultLoggerBufferSizeIfValid(0));
            ASSERT(LOGGER_BUFFER_SIZE[0] == X1.defaultLoggerBufferSize());
            ASSERT(0 == mX1.setDefaultLoggerBufferSizeIfValid(1));
            ASSERT(1 == X1.defaultLoggerBufferSize());
            ASSERT(0 == mX1.setDefaultLoggerBufferSizeIfValid(2));
            ASSERT(2 == X1.defaultLoggerBufferSize());
        }
        if (verbose)
            cout << "\t Check defaultThresholdLevels contraint. " << endl;
        {
            ASSERT(0 == Obj::areValidDefaultThresholdLevels(-1, 2, 3, 4));
            ASSERT(0 == Obj::areValidDefaultThresholdLevels(1, -1, 3, 4));
            ASSERT(0 == Obj::areValidDefaultThresholdLevels(1, 2, -1, 4));
            ASSERT(0 == Obj::areValidDefaultThresholdLevels(1, 2, 3, -1));
            ASSERT(0 == Obj::areValidDefaultThresholdLevels(256, 2, 3, 4));
            ASSERT(0 == Obj::areValidDefaultThresholdLevels(1, 256, 3, 4));
            ASSERT(0 == Obj::areValidDefaultThresholdLevels(1, 2, 256, 4));
            ASSERT(0 == Obj::areValidDefaultThresholdLevels(1, 2, 3, 256));
            ASSERT(1 == Obj::areValidDefaultThresholdLevels(1, 2, 3, 4));

            ASSERT(0 != mX1.setDefaultThresholdLevelsIfValid(-1, 2, 3, 4));
            ASSERT(     RECORD_LEVEL[0] == X1.defaultRecordLevel());
            ASSERT(       PASS_LEVEL[0] == X1.defaultPassLevel());
            ASSERT(    TRIGGER_LEVEL[0] == X1.defaultTriggerLevel());
            ASSERT(TRIGGER_ALL_LEVEL[0] == X1.defaultTriggerAllLevel());

            ASSERT(0 != mX1.setDefaultThresholdLevelsIfValid(1, -1, 3, 4));
            ASSERT(     RECORD_LEVEL[0] == X1.defaultRecordLevel());
            ASSERT(       PASS_LEVEL[0] == X1.defaultPassLevel());
            ASSERT(    TRIGGER_LEVEL[0] == X1.defaultTriggerLevel());
            ASSERT(TRIGGER_ALL_LEVEL[0] == X1.defaultTriggerAllLevel());

            ASSERT(0 != mX1.setDefaultThresholdLevelsIfValid(1, 2, -1, 4));
            ASSERT(     RECORD_LEVEL[0] == X1.defaultRecordLevel());
            ASSERT(       PASS_LEVEL[0] == X1.defaultPassLevel());
            ASSERT(    TRIGGER_LEVEL[0] == X1.defaultTriggerLevel());
            ASSERT(TRIGGER_ALL_LEVEL[0] == X1.defaultTriggerAllLevel());

            ASSERT(0 != mX1.setDefaultThresholdLevelsIfValid(1, 2, 3, -1));
            ASSERT(     RECORD_LEVEL[0] == X1.defaultRecordLevel());
            ASSERT(       PASS_LEVEL[0] == X1.defaultPassLevel());
            ASSERT(    TRIGGER_LEVEL[0] == X1.defaultTriggerLevel());
            ASSERT(TRIGGER_ALL_LEVEL[0] == X1.defaultTriggerAllLevel());

            ASSERT(0 != mX1.setDefaultThresholdLevelsIfValid(256, 2, 3, 4));
            ASSERT(     RECORD_LEVEL[0] == X1.defaultRecordLevel());
            ASSERT(       PASS_LEVEL[0] == X1.defaultPassLevel());
            ASSERT(    TRIGGER_LEVEL[0] == X1.defaultTriggerLevel());
            ASSERT(TRIGGER_ALL_LEVEL[0] == X1.defaultTriggerAllLevel());

            ASSERT(0 != mX1.setDefaultThresholdLevelsIfValid(1, 256, 3, 4));
            ASSERT(     RECORD_LEVEL[0] == X1.defaultRecordLevel());
            ASSERT(       PASS_LEVEL[0] == X1.defaultPassLevel());
            ASSERT(    TRIGGER_LEVEL[0] == X1.defaultTriggerLevel());
            ASSERT(TRIGGER_ALL_LEVEL[0] == X1.defaultTriggerAllLevel());

            ASSERT(0 != mX1.setDefaultThresholdLevelsIfValid(1, 2, 256, 4));
            ASSERT(     RECORD_LEVEL[0] == X1.defaultRecordLevel());
            ASSERT(       PASS_LEVEL[0] == X1.defaultPassLevel());
            ASSERT(    TRIGGER_LEVEL[0] == X1.defaultTriggerLevel());
            ASSERT(TRIGGER_ALL_LEVEL[0] == X1.defaultTriggerAllLevel());

            ASSERT(0 != mX1.setDefaultThresholdLevelsIfValid(1, 2, 3, 256));
            ASSERT(     RECORD_LEVEL[0] == X1.defaultRecordLevel());
            ASSERT(       PASS_LEVEL[0] == X1.defaultPassLevel());
            ASSERT(    TRIGGER_LEVEL[0] == X1.defaultTriggerLevel());
            ASSERT(TRIGGER_ALL_LEVEL[0] == X1.defaultTriggerAllLevel());

            ASSERT(0 == mX1.setDefaultThresholdLevelsIfValid(1, 2, 3, 4));
            ASSERT(1 == X1.defaultRecordLevel());
            ASSERT(2 == X1.defaultPassLevel());
            ASSERT(3 == X1.defaultTriggerLevel());
            ASSERT(4 == X1.defaultTriggerAllLevel());
        }

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // Basic Attribute Test:
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Basic Attribute Test" << endl
                          << "=====================" << endl;

        Obj mX1;  const Obj& X1 = mX1;
        Obj mY1;  const Obj& Y1 = mY1;
        Obj mZ1;  const Obj& Z1 = mZ1; // Z1 is the control
        if (verbose) { cout << "X1 = ";  X1.print(cout, -1, 4); }

        if (verbose) cout << "\nCheck imp.-defined default values " << endl;

        ASSERT(RECORD_BUFFER_SIZE[0] == Obj::defaultDefaultRecordBufferSize());
        ASSERT(LOGGER_BUFFER_SIZE[0] == Obj::defaultDefaultLoggerBufferSize());
        ASSERT(      RECORD_LEVEL[0] == Obj::defaultDefaultRecordLevel());
        ASSERT(        PASS_LEVEL[0] == Obj::defaultDefaultPassLevel());
        ASSERT(     TRIGGER_LEVEL[0] == Obj::defaultDefaultTriggerLevel());
        ASSERT( TRIGGER_ALL_LEVEL[0] == Obj::defaultDefaultTriggerAllLevel());

        if (verbose) cout << "\nCheck default ctor. " << endl;

        ASSERT(RECORD_BUFFER_SIZE[0] == X1.defaultRecordBufferSize());
        ASSERT(LOGGER_BUFFER_SIZE[0] == X1.defaultLoggerBufferSize());
        ASSERT(      RECORD_LEVEL[0] == X1.defaultRecordLevel());
        ASSERT(        PASS_LEVEL[0] == X1.defaultPassLevel());
        ASSERT(     TRIGGER_LEVEL[0] == X1.defaultTriggerLevel());
        ASSERT( TRIGGER_ALL_LEVEL[0] == X1.defaultTriggerAllLevel());

        ASSERT(1 == (X1 == X1));          ASSERT(0 == (X1 != X1));
        ASSERT(1 == (X1 == Z1));          ASSERT(0 == (X1 != Z1));
        ASSERT(1 == (Z1 == Y1));          ASSERT(0 == (Z1 != Y1));
        ASSERT(1 == (Y1 == Z1));          ASSERT(0 == (Y1 != Z1));

        if (verbose) cout << "\tSetting default values explicitly." <<endl;

        ASSERT(0 == mX1.setDefaultRecordBufferSizeIfValid(
                                                       RECORD_BUFFER_SIZE[0]));
        ASSERT(0 == mX1.setDefaultLoggerBufferSizeIfValid(
                                                       LOGGER_BUFFER_SIZE[0]));
        ASSERT(0 == mX1.setDefaultThresholdLevelsIfValid(
                                                        RECORD_LEVEL[0],
                                                        PASS_LEVEL[0],
                                                        TRIGGER_LEVEL[0],
                                                        TRIGGER_ALL_LEVEL[0]));

        ASSERT(1 == (X1 == X1));          ASSERT(0 == (X1 != X1));
        ASSERT(1 == (X1 == Z1));          ASSERT(0 == (X1 != Z1));
        ASSERT(1 == (Z1 == Y1));          ASSERT(0 == (Z1 != Y1));
        ASSERT(1 == (Y1 == Z1));          ASSERT(0 == (Y1 != Z1));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\tTesting set/get methods."
                          << "\n\t\tChange attribute 0." << endl;

        ASSERT(0 == mX1.setDefaultRecordBufferSizeIfValid(
                                                       RECORD_BUFFER_SIZE[1]));

        ASSERT(RECORD_BUFFER_SIZE[1] == X1.defaultRecordBufferSize());
        ASSERT(LOGGER_BUFFER_SIZE[0] == X1.defaultLoggerBufferSize());
        ASSERT(      RECORD_LEVEL[0] == X1.defaultRecordLevel());
        ASSERT(        PASS_LEVEL[0] == X1.defaultPassLevel());
        ASSERT(     TRIGGER_LEVEL[0] == X1.defaultTriggerLevel());
        ASSERT( TRIGGER_ALL_LEVEL[0] == X1.defaultTriggerAllLevel());

        ASSERT(1 == (X1 == X1));          ASSERT(0 == (X1 != X1));
        ASSERT(0 == (X1 == Z1));          ASSERT(1 == (X1 != Z1));
        ASSERT(0 == (Z1 == X1));          ASSERT(1 == (Z1 != X1));
        ASSERT(1 == (Y1 == Z1));          ASSERT(0 == (Y1 != Z1));
        {
            Obj C(X1);
            ASSERT(1 == (C == X1));       ASSERT(0 == (C != X1));
        }

        mY1 = X1;
        ASSERT(1 == (Y1 == Y1));          ASSERT(0 == (Y1 != Y1));
        ASSERT(1 == (Y1 == X1));          ASSERT(0 == (Y1 != X1));
        ASSERT(0 == (Y1 == Z1));          ASSERT(1 == (Y1 != Z1));

        ASSERT(0 == mX1.setDefaultRecordBufferSizeIfValid(
                                                       RECORD_BUFFER_SIZE[0]));
        ASSERT(1 == (X1 == X1));          ASSERT(0 == (X1 != X1));
        ASSERT(1 == (X1 == Z1));          ASSERT(0 == (X1 != Z1));
        ASSERT(0 == (Y1 == Z1));          ASSERT(1 == (Y1 != Z1));

        mX1 = mY1 = Z1;
        ASSERT(1 == (X1 == X1));          ASSERT(0 == (X1 != X1));
        ASSERT(1 == (X1 == Z1));          ASSERT(0 == (X1 != Z1));
        ASSERT(1 == (Y1 == Z1));          ASSERT(0 == (Y1 != Z1));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\t\tChange attribute 1." << endl;

        ASSERT(0 == mX1.setDefaultLoggerBufferSizeIfValid(
                                                       LOGGER_BUFFER_SIZE[1]));

        ASSERT(RECORD_BUFFER_SIZE[0] == X1.defaultRecordBufferSize());
        ASSERT(LOGGER_BUFFER_SIZE[1] == X1.defaultLoggerBufferSize());
        ASSERT(      RECORD_LEVEL[0] == X1.defaultRecordLevel());
        ASSERT(        PASS_LEVEL[0] == X1.defaultPassLevel());
        ASSERT(     TRIGGER_LEVEL[0] == X1.defaultTriggerLevel());
        ASSERT( TRIGGER_ALL_LEVEL[0] == X1.defaultTriggerAllLevel());

        ASSERT(1 == (X1 == X1));          ASSERT(0 == (X1 != X1));
        ASSERT(0 == (X1 == Z1));          ASSERT(1 == (X1 != Z1));
        ASSERT(0 == (Z1 == X1));          ASSERT(1 == (Z1 != X1));
        ASSERT(1 == (Y1 == Z1));          ASSERT(0 == (Y1 != Z1));
        {
            Obj C(X1);
            ASSERT(1 == (C == X1));       ASSERT(0 == (C != X1));
        }

        mY1 = X1;
        ASSERT(1 == (Y1 == Y1));          ASSERT(0 == (Y1 != Y1));
        ASSERT(1 == (Y1 == X1));          ASSERT(0 == (Y1 != X1));
        ASSERT(0 == (Y1 == Z1));          ASSERT(1 == (Y1 != Z1));

        ASSERT(0 == mX1.setDefaultLoggerBufferSizeIfValid(
                                                       LOGGER_BUFFER_SIZE[0]));
        ASSERT(1 == (X1 == X1));          ASSERT(0 == (X1 != X1));
        ASSERT(1 == (X1 == Z1));          ASSERT(0 == (X1 != Z1));
        ASSERT(0 == (Y1 == Z1));          ASSERT(1 == (Y1 != Z1));

        mX1 = mY1 = Z1;
        ASSERT(1 == (X1 == X1));          ASSERT(0 == (X1 != X1));
        ASSERT(1 == (X1 == Z1));          ASSERT(0 == (X1 != Z1));
        ASSERT(1 == (Y1 == Z1));          ASSERT(0 == (Y1 != Z1));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\t\tChange attribute 2." << endl;

        ASSERT(0 == mX1.setDefaultThresholdLevelsIfValid(RECORD_LEVEL[1],
                                                  PASS_LEVEL[1],
                                                  TRIGGER_LEVEL[1],
                                                  TRIGGER_ALL_LEVEL[1]));

        ASSERT(LOGGER_BUFFER_SIZE[0] == X1.defaultLoggerBufferSize());
        ASSERT(      RECORD_LEVEL[1] == X1.defaultRecordLevel());
        ASSERT(        PASS_LEVEL[1] == X1.defaultPassLevel());
        ASSERT(     TRIGGER_LEVEL[1] == X1.defaultTriggerLevel());
        ASSERT( TRIGGER_ALL_LEVEL[1] == X1.defaultTriggerAllLevel());

        ASSERT(1 == (X1 == X1));          ASSERT(0 == (X1 != X1));
        ASSERT(0 == (X1 == Z1));          ASSERT(1 == (X1 != Z1));
        ASSERT(0 == (Z1 == X1));          ASSERT(1 == (Z1 != X1));
        ASSERT(1 == (Y1 == Z1));          ASSERT(0 == (Y1 != Z1));
        {
            Obj C(X1);
            ASSERT(1 == (C == X1));       ASSERT(0 == (C != X1));
        }

        mY1 = X1;
        ASSERT(1 == (Y1 == Y1));          ASSERT(0 == (Y1 != Y1));
        ASSERT(1 == (Y1 == X1));          ASSERT(0 == (Y1 != X1));
        ASSERT(0 == (Y1 == Z1));          ASSERT(1 == (Y1 != Z1));

        ASSERT(0 == mX1.setDefaultThresholdLevelsIfValid(
                                                        RECORD_LEVEL[0],
                                                        PASS_LEVEL[0],
                                                        TRIGGER_LEVEL[0],
                                                        TRIGGER_ALL_LEVEL[0]));
        ASSERT(1 == (X1 == X1));          ASSERT(0 == (X1 != X1));
        ASSERT(1 == (X1 == Z1));          ASSERT(0 == (X1 != Z1));
        ASSERT(0 == (Y1 == Z1));          ASSERT(1 == (Y1 != Z1));

        mX1 = mY1 = Z1;
        ASSERT(1 == (X1 == X1));          ASSERT(0 == (X1 != X1));
        ASSERT(1 == (X1 == Z1));          ASSERT(0 == (X1 != Z1));
        ASSERT(1 == (Y1 == Z1));          ASSERT(0 == (Y1 != Z1));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "Testing output operator<<." << endl;

        ASSERT(0 == mY1.setDefaultRecordBufferSizeIfValid(
                                                       RECORD_BUFFER_SIZE[1]));
        ASSERT(0 == mY1.setDefaultLoggerBufferSizeIfValid(
                                                       LOGGER_BUFFER_SIZE[1]));
        ASSERT(0 == mY1.setDefaultThresholdLevelsIfValid(
                                                        RECORD_LEVEL[1],
                                                        PASS_LEVEL[1],
                                                        TRIGGER_LEVEL[1],
                                                        TRIGGER_ALL_LEVEL[1]));

        char buf[10000];

        if (verbose) cout << "\tOn a default object:" << endl;
        {
            ostrstream o(buf, sizeof buf);
            o << X1 << ends;
            bsl::string s =
                "[" NL
                "    recordBufferSize : 32768" NL
                "    loggerBufferSize : 8192"  NL
                "    recordLevel      : 0"     NL
                "    passLevel        : 64"    NL
                "    triggerLevel     : 0"     NL
                "    triggerAllLevel  : 0"     NL
                "]" NL
                ;
            ASSERT(buf == s);
            if (veryVerbose) cout << "buf:\n" << buf << "\ns:\n" << s << endl;
        }

        if (verbose) cout << "\tOn a non-default object:" << endl;
        {
            ostrstream o(buf, sizeof buf);
            o << Y1 << ends;
            bsl::string s =
                "[" NL
                "    recordBufferSize : 10" NL
                "    loggerBufferSize : 11" NL
                "    recordLevel      : 12" NL
                "    passLevel        : 13" NL
                "    triggerLevel     : 14" NL
                "    triggerAllLevel  : 15" NL
                "]" NL
                ;
            ASSERT(buf == s);
            if (veryVerbose) cout << "buf:\n" << buf << "\ns:\n" << s << endl;
        }

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "Testing 'bdex' streaming." << endl;

        if (verbose) cout << "\n\tStream 'Y1' into 'mX1'." << endl;

        const int VERSION = Obj::maxSupportedBdexVersion();

        Out out;
        bdex_OutStreamFunctions::streamOut(out, Y1, VERSION);

        if (veryVerbose)  { cout << "\tY1 = ";  Y1.print(cout, -3, 4); }

        ASSERT(X1 != Y1);

        In in(out.data(), out.length());
        in.setSuppressVersionCheck(1);

        if (veryVerbose) cout << "\tBefore streaming in 'mX1':\n\tX1 = ";
        if (veryVerbose)  X1.print(cout, -3, 4);
        bdex_InStreamFunctions::streamIn(in, mX1, VERSION);
        if (veryVerbose) cout << "\tAfter streaming in 'mX1':\n\tX1 = ";
        if (veryVerbose) X1.print(cout, -3, 4);

        ASSERT(X1 == Y1);

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

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2004
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
