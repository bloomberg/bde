// btemt_queryprocessorconfiguration.t.cpp  -*-C++-*-

#include <btemt_queryprocessorconfiguration.h>

#include <bdet_timeinterval.h>
#include <bdex_testoutstream.h>                 // for testing only
#include <bdex_testinstream.h>                  // for testing only
#include <bdex_testinstreamexception.h>         // for testing only
#include <bdex_byteoutstream.h>                 // for testing only
#include <bdex_byteinstream.h>                  // for testing only

#include <bsl_cstring.h>     // strlen()
#include <bsl_cstdlib.h>     // atoi()
#include <bsl_iostream.h>
#include <bsl_string.h>
#include <bsl_strstream.h>

using namespace BloombergLP;
using namespace bsl;  // automatically added by script


//=============================================================================
//                                 TEST PLAN
//-----------------------------------------------------------------------------
//                                 Overview
//                                 --------
// This test plan follows the standard approach for components implementing
//
//-----------------------------------------------------------------------------
// [ 3] static int maxSupportedBdexVersion();
// [ 1] btemt_QueryProcessorConfiguration();
// [ 1] btemt_QueryProcessorConfiguration(const config&);
// [ 1] ~btemt_QueryProcessorConfiguration();
// [ 1] btemt_QueryProcessorConfiguration&
// [ 1] operator=(const btemt_QueryProcessorConfiguration& rhs);
// [ 2] int setIncomingMessageSize(int min, int typical, int max);
// [ 2] int setOutgoingMessageSize(int min, int typical, int max);
// [ 2] int setMaxConnections(int maxConnections);
// [ 2] int setMaxIoThreads(int maxIoThreads);
// [ 2] int setMaxWriteCache(int numBytes);
// [ 2] int setMetricsInterval(double metricsInterval);
// [ 2] int setReadTimeout(double readTimeout);
// [ 3] bdex_InStream& streamIn(bdex_InStream& stream);
// [ 3] bdex_InStream& streamIn(bdex_InStream& stream, int version);
// [ 1] int incomingMessageSize() const;
// [ 1] int minOutgoingMessageSize() const;
// [ 1] int maxConnections() const;
// [ 1] int maxIoThreads() const;
// [ 1] int maxWriteCache() const;
// [ 1] double metricsInterval() const;
// [ 1] double readTimeout() const;
// [ 3] bdex_OutStream& streamOut(bdex_OutStream& stream) const;
// [ 3] bdex_OutStream& streamOut(bdex_OutStream& stream, int version) const;
//
// [ 1] bool operator==(const btemt_QueryProcessorConfiguration& lhs, ...
// [ 1] bool operator!=(const btemt_QueryProcessorConfiguration& lhs, ...
// [ 1] bsl::ostream& operator<<(bsl::ostream&, const btemt_ChannelPoolConf...
// [ 3] bdex_InStream& operator>>(bdex_InStream&, btemt_ChannelPoolConfig...
// [ 3] bdex_OutStream& operator<<(bdex_OutStream&, const double&);
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

typedef btemt_QueryProcessorConfiguration Obj;
typedef double TI;
typedef bdex_TestInStream  In;
typedef bdex_TestOutStream Out;

const int NUM_VALUES = 7;

// The 0th element in each array contains the value set by the default
// constructor for the corresponding attribute.

const TI T00 = 30.0;
const TI T10 = 60.0;
const TI T11 = 61.1;
const TI T20 = 120.0;
const TI T21 = 121.0;
const TI T30 = 333.0;
const TI T31 = 333.3;
const TI T40 = 444.0;
const TI T41 = 444.4;
const TI T50 = 555.0;
const TI T51 = 555.5;
const TI T60 = 678.0;
const TI T61 = 689.6;

const int MAXCONNECTIONS[NUM_VALUES]   = { 1,   10,  20,  300, 400, 500, 600 };
const int MAXNUMTHREADS[NUM_VALUES]    = { 1,   11,  21,  301, 401, 501, 601 };
const int MAXWRITECACHE[NUM_VALUES]    = { 256, 512, 600, 700, 800, 900, 999 };
const TI  READTIMEOUT[NUM_VALUES]      = { T00, T10, T20, T30, T40, T50, T60 };
const TI  METRICSINTERVAL[NUM_VALUES]  = { T00, T11, T21, T31, T41, T51, T61 };
const int MINMESSAGESIZEOUT[NUM_VALUES]= { 1,   12,  22,  302, 402, 502, 602 };
const int MINMESSAGESIZEIN[NUM_VALUES] = { 1,   15,  25,  305, 405, 505, 605 };

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
      case 1: {
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

        btemt_QueryProcessorConfiguration cpc;
        ASSERT(0 == cpc.setIncomingMessageSize(4));
        ASSERT(4 == cpc.incomingMessageSize());

        ASSERT(0 == cpc.setOutgoingMessageSize(6));
        ASSERT(6 == cpc.outgoingMessageSize());

        ASSERT(0 == cpc.setMaxConnections(100));
        ASSERT(100 == cpc.maxConnections());

        ASSERT(0 == cpc.setMaxIoThreads(4));
        ASSERT(4 == cpc.maxIoThreads());

        ASSERT(0 == cpc.setMaxWriteCache(1024));
        ASSERT(1024 == cpc.maxWriteCache());

        ASSERT(0 == cpc.setReadTimeout(3.5));
        ASSERT(3.5 == cpc.readTimeout());

        ASSERT(0 == cpc.setMetricsInterval(5.25));
        ASSERT(5.25 == cpc.metricsInterval());

        ASSERT(0 != cpc.setIncomingMessageSize(-256));
        ASSERT(4 == cpc.incomingMessageSize());

        ASSERT(0 == cpc.setProcessingThreads(3, 5));
        ASSERT(3 == cpc.minProcessingThreads());
        ASSERT(5 == cpc.maxProcessingThreads());

        ASSERT(0 == cpc.setIdleTimeout(3.6));
        ASSERT(3.6 == cpc.idleTimeout());

        char buf[10000];
        {
            ostrstream o(buf, sizeof buf);
            o << cpc;
            if (verbose) cout << "X1 buf:\n" << buf << endl;
            bsl::string s =
                "[" NL
                "\tmaxConnections         : 100" NL
                "\tmaxIoThreads           : 4" NL
                "\tmaxWriteCache          : 1024" NL
                "\treadTimeout            : 3.5" NL
                "\tmetricsInterval        : 5.25" NL
                "\toutgoingMessageSize    : 6" NL
                "\tincomingMessageSize    : 4" NL
                "\tminProcThreads         : 3" NL
                "\tmaxProcThreads         : 5" NL
                "\tidleTimeout            : 3.6" NL
                "]" NL
                ;
            if (verbose) P(s);
            ASSERT(buf == s);
        }
      } break;
// TBD
#if 0
      case 3: {
        // --------------------------------------------------------------------
        // TESTING 'bdex' STREAMING FUNCTIONALITY:
        //   The 'bdex' streaming concerns for this component are absolutely
        //   standard.  We first probe the member functions 'outStream' and
        //   'inStream' in the manner of a "breathing test" to verify basic
        //   functionality, then we thoroughly test that functionality using
        //   the overloaded '<<' and '>>' free operators, which forward
        //   appropriate calls to the member functions.  We next step through
        //   the sequence of possible stream states (valid, empty, invalid,
        //   incomplete, and corrupted), appropriately selecting data sets as
        //   described below.  In all cases, exception neutrality is confirmed
        //   using the specially instrumented 'bdex_TestInStream' and a pair of
        //   standard macros, 'BEGIN_BDEX_EXCEPTION_TEST' and
        //   'END_BDEX_EXCEPTION_TEST', which configure the
        //   'bdex_TestInStream' object appropriately in a loop.
        //
        // Plan:
        //   PRELIMINARY MEMBER FUNCTION TEST
        //     First perform a trivial direct test of the 'outStream' and
        //     'inStream' methods (the rest of the testing will use the stream
        //     operators).
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
        //   operator>>(bdex_InStream&, btemt_QueryProcessorConfiguration&);
        //   operator<<(bdex_OutStream&, const config&);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Streaming Functionality"
                          << "\n===============================" << endl;

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        // object values for various stream tests

        Obj va, vb, vc, vd, ve, vf;
        ASSERT(0 == va.setIncomingMessageSize(MINMESSAGESIZEIN[1],
                                                TYPMESSAGESIZEIN[1],
                                                MAXMESSAGESIZEIN[1]));
        ASSERT(0 == va.setOutgoingMessageSize(MINMESSAGESIZEOUT[1],
                                                TYPMESSAGESIZEOUT[1],
                                                MAXMESSAGESIZEOUT[1]));
        ASSERT(0 == va.setMaxConnections(MAXCONNECTIONS[1]));
        ASSERT(0 == va.setMaxIoThreads(MAXNUMTHREADS[1]));
        ASSERT(0 == va.setMaxWriteCache(MAXWRITECACHE[1]));
        ASSERT(0 == va.setMetricsInterval(METRICSINTERVAL[1]));
        ASSERT(0 == va.setReadTimeout(READTIMEOUT[1]));
        ASSERT(0 == va.setWorkloadThreshold(WORKLOADTHRESHOLD[1]));

        ASSERT(0 == vb.setIncomingMessageSize(MINMESSAGESIZEIN[2],
                                                TYPMESSAGESIZEIN[2],
                                                MAXMESSAGESIZEIN[2]));
        ASSERT(0 == vb.setOutgoingMessageSize(MINMESSAGESIZEOUT[2],
                                                TYPMESSAGESIZEOUT[2],
                                                MAXMESSAGESIZEOUT[2]));
        ASSERT(0 == vb.setMaxConnections(MAXCONNECTIONS[2]));
        ASSERT(0 == vb.setMaxIoThreads(MAXNUMTHREADS[2]));
        ASSERT(0 == vb.setMaxWriteCache(MAXWRITECACHE[2]));
        ASSERT(0 == vb.setMetricsInterval(METRICSINTERVAL[2]));
        ASSERT(0 == vb.setReadTimeout(READTIMEOUT[2]));
        ASSERT(0 == vb.setWorkloadThreshold(WORKLOADTHRESHOLD[2]));

        ASSERT(0 == vc.setIncomingMessageSize(MINMESSAGESIZEIN[3],
                                                TYPMESSAGESIZEIN[3],
                                                MAXMESSAGESIZEIN[3]));
        ASSERT(0 == vc.setOutgoingMessageSize(MINMESSAGESIZEOUT[3],
                                                TYPMESSAGESIZEOUT[3],
                                                MAXMESSAGESIZEOUT[3]));
        ASSERT(0 == vc.setMaxConnections(MAXCONNECTIONS[3]));
        ASSERT(0 == vc.setMaxIoThreads(MAXNUMTHREADS[3]));
        ASSERT(0 == vc.setMaxWriteCache(MAXWRITECACHE[3]));
        ASSERT(0 == vc.setMetricsInterval(METRICSINTERVAL[3]));
        ASSERT(0 == vc.setReadTimeout(READTIMEOUT[3]));
        ASSERT(0 == vc.setWorkloadThreshold(WORKLOADTHRESHOLD[3]));

        ASSERT(0 == vd.setIncomingMessageSize(MINMESSAGESIZEIN[4],
                                                TYPMESSAGESIZEIN[4],
                                                MAXMESSAGESIZEIN[4]));
        ASSERT(0 == vd.setOutgoingMessageSize(MINMESSAGESIZEOUT[4],
                                                TYPMESSAGESIZEOUT[4],
                                                MAXMESSAGESIZEOUT[4]));
        ASSERT(0 == vd.setMaxConnections(MAXCONNECTIONS[4]));
        ASSERT(0 == vd.setMaxIoThreads(MAXNUMTHREADS[4]));
        ASSERT(0 == vd.setMaxWriteCache(MAXWRITECACHE[4]));
        ASSERT(0 == vd.setMetricsInterval(METRICSINTERVAL[4]));
        ASSERT(0 == vd.setReadTimeout(READTIMEOUT[4]));
        ASSERT(0 == vd.setWorkloadThreshold(WORKLOADTHRESHOLD[4]));

        ASSERT(0 == ve.setIncomingMessageSize(MINMESSAGESIZEIN[5],
                                                TYPMESSAGESIZEIN[5],
                                                MAXMESSAGESIZEIN[5]));
        ASSERT(0 == ve.setOutgoingMessageSize(MINMESSAGESIZEOUT[5],
                                                TYPMESSAGESIZEOUT[5],
                                                MAXMESSAGESIZEOUT[5]));
        ASSERT(0 == ve.setMaxConnections(MAXCONNECTIONS[5]));
        ASSERT(0 == ve.setMaxIoThreads(MAXNUMTHREADS[5]));
        ASSERT(0 == ve.setMaxWriteCache(MAXWRITECACHE[5]));
        ASSERT(0 == ve.setMetricsInterval(METRICSINTERVAL[5]));
        ASSERT(0 == ve.setReadTimeout(READTIMEOUT[5]));
        ASSERT(0 == ve.setWorkloadThreshold(WORKLOADTHRESHOLD[5]));

        ASSERT(0 == vf.setIncomingMessageSize(MINMESSAGESIZEIN[6],
                                                TYPMESSAGESIZEIN[6],
                                                MAXMESSAGESIZEIN[6]));
        ASSERT(0 == vf.setOutgoingMessageSize(MINMESSAGESIZEOUT[6],
                                                TYPMESSAGESIZEOUT[6],
                                                MAXMESSAGESIZEOUT[6]));
        ASSERT(0 == vf.setMaxConnections(MAXCONNECTIONS[6]));
        ASSERT(0 == vf.setMaxIoThreads(MAXNUMTHREADS[6]));
        ASSERT(0 == vf.setMaxWriteCache(MAXWRITECACHE[6]));
        ASSERT(0 == vf.setMetricsInterval(METRICSINTERVAL[6]));
        ASSERT(0 == vf.setReadTimeout(READTIMEOUT[6]));
        ASSERT(0 == vf.setWorkloadThreshold(WORKLOADTHRESHOLD[6]));

        const Obj VA(va);
        const Obj VB(vb);
        const Obj VC(vc);
        const Obj VD(vd);
        const Obj VE(ve);
        const Obj VF(vf);

        const Obj VALUES[NUM_VALUES] = { VA, VB, VC, VD, VE, VF };
        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout <<
            "\nTesting 'streamOut' and (valid) 'streamIn' functionality."
                          << endl;

        if (verbose) cout << "\nDirect initial trial of 'streamOut' and"
                             " (valid) 'streamIn' functionality." << endl;
        {
            const Obj X(VC);
            Out out;
            out << X;

            const char *const OD  = out.data();
            const int         LOD = out.length();
            In in(OD, LOD);
            ASSERT(in);                                 ASSERT(!in.isEmpty());

            Obj t(VA);  const Obj& T = t;               ASSERT(X != T);
            in >> t;                                    ASSERT(X == T);
            ASSERT(in);                                 ASSERT(in.isEmpty());
        }

        if (verbose) cout <<
            "\nThorough test of stream operators ('<<' and '>>')." << endl;
        {
            for (int i = 0; i < NUM_VALUES; ++i) {
                const Obj X(VALUES[i]);
                Out out;
                out << X;
                const char *const OD  = out.data();
                const int         LOD = out.length();

                // Verify that each new value overwrites every old value
                // and that the input stream is emptied, but remains valid.

                for (int j = 0; j < NUM_VALUES; ++j) {
                    In in(OD, LOD);  In &testInStream = in;
                    LOOP2_ASSERT(i, j, in);  LOOP2_ASSERT(i, j, !in.isEmpty());

                    Obj t(VALUES[j]);
                  BEGIN_BDEX_EXCEPTION_TEST { in.reset();
                    LOOP2_ASSERT(i, j, X == t == (i == j));
                    in >> t;
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
                LOOP_ASSERT(i, in);           LOOP_ASSERT(i, in.isEmpty());

                // Ensure that reading from an empty or invalid input stream
                // leaves the stream invalid and the target object unchanged.

                const Obj X(VALUES[i]);  Obj t(X);  LOOP_ASSERT(i, X == t);
              BEGIN_BDEX_EXCEPTION_TEST { in.reset();
                in >> t;   LOOP_ASSERT(i, !in);     LOOP_ASSERT(i, X == t);
                in >> t;   LOOP_ASSERT(i, !in);     LOOP_ASSERT(i, X == t);
              } END_BDEX_EXCEPTION_TEST
            }
        }

        if (verbose) cout <<
            "\tOn incomplete (but otherwise valid) data." << endl;
        {
            const Obj W1 = VA, X1 = VB, Y1 = VC;
            const Obj W2 = VB, X2 = VC, Y2 = VD;
            const Obj W3 = VC, X3 = VD, Y3 = VE;

            Out out;
            out << X1;  const int LOD1 = out.length();
            out << X2;  const int LOD2 = out.length();
            out << X3;  const int LOD  = out.length();
            const char *const OD = out.data();

            for (int i = 0; i < LOD; ++i) {
                In in(OD, i);  In &testInStream = in;
              BEGIN_BDEX_EXCEPTION_TEST { in.reset();
                LOOP_ASSERT(i, in); LOOP_ASSERT(i, !i == in.isEmpty());
                Obj t1(W1), t2(W2), t3(W3);

                if (i < LOD1) {
                    in >> t1;  LOOP_ASSERT(i, !in);
                                         if (0 == i) LOOP_ASSERT(i, W1 == t1);
                    in >> t2;  LOOP_ASSERT(i, !in);  LOOP_ASSERT(i, W2 == t2);
                    in >> t3;  LOOP_ASSERT(i, !in);  LOOP_ASSERT(i, W3 == t3);
                }
                else if (i < LOD2) {
                    in >> t1;  LOOP_ASSERT(i,  in);  LOOP_ASSERT(i, X1 == t1);
                    in >> t2;  LOOP_ASSERT(i, !in);
                                      if (LOD1 == i) LOOP_ASSERT(i, W2 == t2);
                    in >> t3;  LOOP_ASSERT(i, !in);  LOOP_ASSERT(i, W3 == t3);
                }
                else {
                    in >> t1;  LOOP_ASSERT(i,  in);  LOOP_ASSERT(i, X1 == t1);
                    in >> t2;  LOOP_ASSERT(i,  in);  LOOP_ASSERT(i, X2 == t2);
                    in >> t3;  LOOP_ASSERT(i, !in);
                                      if (LOD2 == i) LOOP_ASSERT(i, W3 == t3);
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

        const Obj W;               // default value (0)
        const Obj X(VA);           // control value (VA)
        const Obj Y(VB);           // new value (VB)

        if (verbose) cout << "\t\tGood stream (for control)." << endl;
        {
            const char version = 1;

            Out out;
            out.putVersion(version);

            out.putInt32(MAXCONNECTIONS[2]);
            out.putInt32(MAXNUMTHREADS[2]);
            out.putInt32(MAXWRITECACHE[2]);
            out.putFloat64(READTIMEOUT[2]);
            out.putFloat64(METRICSINTERVAL[2]);
            out.putInt32(MINMESSAGESIZEOUT[2]);
            out.putInt32(TYPMESSAGESIZEOUT[2]);
            out.putInt32(MAXMESSAGESIZEOUT[2]);
            out.putInt32(MINMESSAGESIZEIN[2]);
            out.putInt32(TYPMESSAGESIZEIN[2]);
            out.putInt32(MAXMESSAGESIZEIN[2]);
            out.putInt32(WORKLOADTHRESHOLD[2]);

            const char *const OD  = out.data();
            const int         LOD = out.length();

            Obj t(X);        ASSERT(W != t);  ASSERT(X == t);  ASSERT(Y != t);
            In in(OD, LOD);  ASSERT(in);
            in >> t;         ASSERT(in);
                             ASSERT(W != t);  ASSERT(X != t);  ASSERT(Y == t);
        }

        if (verbose) cout << "\t\tBad version." << endl;
        if (verbose) cout << "\t\t\tVersion too small." <<endl;
        {
            const char badVersion = 0; // too small ('version' must be >= 1)

            Out out;
            out.putVersion(badVersion);

            out.putInt32(MAXCONNECTIONS[2]);
            out.putInt32(MAXNUMTHREADS[2]);
            out.putInt32(MAXWRITECACHE[2]);
            out.putFloat64(READTIMEOUT[2]);
            out.putFloat64(METRICSINTERVAL[2]);
            out.putInt32(MINMESSAGESIZEOUT[2]);
            out.putInt32(TYPMESSAGESIZEOUT[2]);
            out.putInt32(MAXMESSAGESIZEOUT[2]);
            out.putInt32(MINMESSAGESIZEIN[2]);
            out.putInt32(TYPMESSAGESIZEIN[2]);
            out.putInt32(MAXMESSAGESIZEIN[2]);
            out.putInt32(WORKLOADTHRESHOLD[2]);

            const char *const OD  = out.data();
            const int         LOD = out.length();

            Obj t(X);        ASSERT(W != t);  ASSERT(X == t);  ASSERT(Y != t);
            In in(OD, LOD);  ASSERT(in);
            in.setQuiet(!veryVerbose);
            in >> t;         ASSERT(!in);
                             ASSERT(W != t);  ASSERT(X == t);  ASSERT(Y != t);
        }

        if (verbose) cout << "\t\t\tVersion too big." << endl;
        {
            const char badVersion = 5; // too large (current versions all = 1)

            Out out;
            out.putVersion(badVersion);

            out.putInt32(MAXCONNECTIONS[2]);
            out.putInt32(MAXNUMTHREADS[2]);
            out.putInt32(MAXWRITECACHE[2]);
            out.putFloat64(READTIMEOUT[2]);
            out.putFloat64(METRICSINTERVAL[2]);
            out.putInt32(MINMESSAGESIZEOUT[2]);
            out.putInt32(TYPMESSAGESIZEOUT[2]);
            out.putInt32(MAXMESSAGESIZEOUT[2]);
            out.putInt32(MINMESSAGESIZEIN[2]);
            out.putInt32(TYPMESSAGESIZEIN[2]);
            out.putInt32(MAXMESSAGESIZEIN[2]);
            out.putInt32(WORKLOADTHRESHOLD[2]);

            const char *const OD  = out.data();
            const int         LOD = out.length();

            Obj t(X);        ASSERT(W != t);  ASSERT(X == t);  ASSERT(Y != t);
            In in(OD, LOD);  ASSERT(in);
            in.setQuiet(!veryVerbose);
            in >> t;         ASSERT(!in);
                             ASSERT(W != t);  ASSERT(X == t);  ASSERT(Y != t);
        }

        if (verbose) cout << "\t\tBad value." << endl;
        if (verbose) cout << "\t\t\tmaxConnections neg." <<endl;
        {
            const char version = 1;

            Out out;
            out.putVersion(version);

            out.putInt32(-MAXCONNECTIONS[2]);
            out.putInt32(MAXNUMTHREADS[2]);
            out.putInt32(MAXWRITECACHE[2]);
            out.putFloat64(READTIMEOUT[2]);
            out.putFloat64(METRICSINTERVAL[2]);
            out.putInt32(MINMESSAGESIZEOUT[2]);
            out.putInt32(TYPMESSAGESIZEOUT[2]);
            out.putInt32(MAXMESSAGESIZEOUT[2]);
            out.putInt32(MINMESSAGESIZEIN[2]);
            out.putInt32(TYPMESSAGESIZEIN[2]);
            out.putInt32(MAXMESSAGESIZEIN[2]);
            out.putInt32(WORKLOADTHRESHOLD[2]);

            const char *const OD  = out.data();
            const int         LOD = out.length();

            Obj t(X);        ASSERT(W != t);  ASSERT(X == t);  ASSERT(Y != t);
            In in(OD, LOD);  ASSERT(in);
            in.setQuiet(!veryVerbose);
            in >> t;         ASSERT(!in);
                             ASSERT(W != t);  ASSERT(X == t);  ASSERT(Y != t);
        }
        if (verbose) cout << "\t\t\tmaxIoThreads neg." <<endl;
        {
            const char version = 1;

            Out out;
            out.putVersion(version);

            out.putInt32(MAXCONNECTIONS[2]);
            out.putInt32(-MAXNUMTHREADS[2]);
            out.putInt32(MAXWRITECACHE[2]);
            out.putFloat64(READTIMEOUT[2]);
            out.putFloat64(METRICSINTERVAL[2]);
            out.putInt32(MINMESSAGESIZEOUT[2]);
            out.putInt32(TYPMESSAGESIZEOUT[2]);
            out.putInt32(MAXMESSAGESIZEOUT[2]);
            out.putInt32(MINMESSAGESIZEIN[2]);
            out.putInt32(TYPMESSAGESIZEIN[2]);
            out.putInt32(MAXMESSAGESIZEIN[2]);
            out.putInt32(WORKLOADTHRESHOLD[2]);

            const char *const OD  = out.data();
            const int         LOD = out.length();

            Obj t(X);        ASSERT(W != t);  ASSERT(X == t);  ASSERT(Y != t);
            In in(OD, LOD);  ASSERT(in);
            in.setQuiet(!veryVerbose);
            in >> t;         ASSERT(!in);
                             ASSERT(W != t);  ASSERT(X == t);  ASSERT(Y != t);
        }
        if (verbose) cout << "\t\t\tmaxWriteCache neg." <<endl;
        {
            const char version = 1;

            Out out;
            out.putVersion(version);

            out.putInt32(MAXCONNECTIONS[2]);
            out.putInt32(MAXNUMTHREADS[2]);
            out.putInt32(-MAXWRITECACHE[2]);
            out.putFloat64(READTIMEOUT[2]);
            out.putFloat64(METRICSINTERVAL[2]);
            out.putInt32(MINMESSAGESIZEOUT[2]);
            out.putInt32(TYPMESSAGESIZEOUT[2]);
            out.putInt32(MAXMESSAGESIZEOUT[2]);
            out.putInt32(MINMESSAGESIZEIN[2]);
            out.putInt32(TYPMESSAGESIZEIN[2]);
            out.putInt32(MAXMESSAGESIZEIN[2]);
            out.putInt32(WORKLOADTHRESHOLD[2]);

            const char *const OD  = out.data();
            const int         LOD = out.length();

            Obj t(X);        ASSERT(W != t);  ASSERT(X == t);  ASSERT(Y != t);
            In in(OD, LOD);  ASSERT(in);
            in.setQuiet(!veryVerbose);
            in >> t;         ASSERT(!in);
                             ASSERT(W != t);  ASSERT(X == t);  ASSERT(Y != t);
        }
        if (verbose) cout << "\t\t\treadTimeOut neg." <<endl;
        {
            const char version = 1;

            Out out;
            out.putVersion(version);

            out.putInt32(MAXCONNECTIONS[2]);
            out.putInt32(MAXNUMTHREADS[2]);
            out.putInt32(MAXWRITECACHE[2]);
            out.putFloat64(-1.1);
            out.putFloat64(METRICSINTERVAL[2]);
            out.putInt32(MINMESSAGESIZEOUT[2]);
            out.putInt32(TYPMESSAGESIZEOUT[2]);
            out.putInt32(MAXMESSAGESIZEOUT[2]);
            out.putInt32(MINMESSAGESIZEIN[2]);
            out.putInt32(TYPMESSAGESIZEIN[2]);
            out.putInt32(MAXMESSAGESIZEIN[2]);
            out.putInt32(WORKLOADTHRESHOLD[2]);

            const char *const OD  = out.data();
            const int         LOD = out.length();

            Obj t(X);        ASSERT(W != t);  ASSERT(X == t);  ASSERT(Y != t);
            In in(OD, LOD);  ASSERT(in);
            in.setQuiet(!veryVerbose);
            in >> t;         ASSERT(!in);
                             ASSERT(W != t);  ASSERT(X == t);  ASSERT(Y != t);
        }
        if (verbose) cout << "\t\t\tminOutgoingMessageSize neg." <<endl;
        {
            const char version = 1;

            Out out;
            out.putVersion(version);

            out.putInt32(MAXCONNECTIONS[2]);
            out.putInt32(MAXNUMTHREADS[2]);
            out.putInt32(MAXWRITECACHE[2]);
            out.putFloat64(READTIMEOUT[2]);
            out.putFloat64(METRICSINTERVAL[2]);
            out.putInt32(-MINMESSAGESIZEOUT[2]);
            out.putInt32(TYPMESSAGESIZEOUT[2]);
            out.putInt32(MAXMESSAGESIZEOUT[2]);
            out.putInt32(MINMESSAGESIZEIN[2]);
            out.putInt32(TYPMESSAGESIZEIN[2]);
            out.putInt32(MAXMESSAGESIZEIN[2]);
            out.putInt32(WORKLOADTHRESHOLD[2]);

            const char *const OD  = out.data();
            const int         LOD = out.length();

            Obj t(X);        ASSERT(W != t);  ASSERT(X == t);  ASSERT(Y != t);
            In in(OD, LOD);  ASSERT(in);
            in.setQuiet(!veryVerbose);
            in >> t;         ASSERT(!in);
                             ASSERT(W != t);  ASSERT(X == t);  ASSERT(Y != t);
        }
        if (verbose) cout << "\t\t\t"
                 "typOutgoingMessageSize < minOutgoingMessageSize." << endl;
        {
            const char version = 1;

            Out out;
            out.putVersion(version);

            out.putInt32(MAXCONNECTIONS[2]);
            out.putInt32(MAXNUMTHREADS[2]);
            out.putInt32(MAXWRITECACHE[2]);
            out.putFloat64(READTIMEOUT[2]);
            out.putFloat64(METRICSINTERVAL[2]);
            out.putInt32(MINMESSAGESIZEOUT[2]);
            out.putInt32(MINMESSAGESIZEOUT[2] - 1);
            out.putInt32(MAXMESSAGESIZEOUT[2]);
            out.putInt32(MINMESSAGESIZEIN[2]);
            out.putInt32(TYPMESSAGESIZEIN[2]);
            out.putInt32(MAXMESSAGESIZEIN[2]);
            out.putInt32(WORKLOADTHRESHOLD[2]);

            const char *const OD  = out.data();
            const int         LOD = out.length();

            Obj t(X);        ASSERT(W != t);  ASSERT(X == t);  ASSERT(Y != t);
            In in(OD, LOD);  ASSERT(in);
            in.setQuiet(!veryVerbose);
            in >> t;         ASSERT(!in);
                             ASSERT(W != t);  ASSERT(X == t);  ASSERT(Y != t);
        }
        if (verbose) cout << "\t\t\t"
                 "maxOutgoingMessageSize < typOutgoingMessageSize." << endl;
        {
            const char version = 1;

            Out out;
            out.putVersion(version);

            out.putInt32(MAXCONNECTIONS[2]);
            out.putInt32(MAXNUMTHREADS[2]);
            out.putInt32(MAXWRITECACHE[2]);
            out.putFloat64(READTIMEOUT[2]);
            out.putFloat64(METRICSINTERVAL[2]);
            out.putInt32(MINMESSAGESIZEOUT[2]);
            out.putInt32(TYPMESSAGESIZEOUT[2]);
            out.putInt32(TYPMESSAGESIZEOUT[2] - 1);
            out.putInt32(MINMESSAGESIZEIN[2]);
            out.putInt32(TYPMESSAGESIZEIN[2]);
            out.putInt32(MAXMESSAGESIZEIN[2]);
            out.putInt32(WORKLOADTHRESHOLD[2]);

            const char *const OD  = out.data();
            const int         LOD = out.length();

            Obj t(X);        ASSERT(W != t);  ASSERT(X == t);  ASSERT(Y != t);
            In in(OD, LOD);  ASSERT(in);
            in.setQuiet(!veryVerbose);
            in >> t;         ASSERT(!in);
                             ASSERT(W != t);  ASSERT(X == t);  ASSERT(Y != t);
        }
        if (verbose) cout << "\t\t\tincomingMessageSize neg." <<endl;
        {
            const char version = 1;

            Out out;
            out.putVersion(version);

            out.putInt32(MAXCONNECTIONS[2]);
            out.putInt32(MAXNUMTHREADS[2]);
            out.putInt32(MAXWRITECACHE[2]);
            out.putFloat64(READTIMEOUT[2]);
            out.putFloat64(METRICSINTERVAL[2]);
            out.putInt32(MINMESSAGESIZEOUT[2]);
            out.putInt32(TYPMESSAGESIZEOUT[2]);
            out.putInt32(MAXMESSAGESIZEOUT[2]);
            out.putInt32(-MINMESSAGESIZEIN[2]);
            out.putInt32(TYPMESSAGESIZEIN[2]);
            out.putInt32(MAXMESSAGESIZEIN[2]);
            out.putInt32(WORKLOADTHRESHOLD[2]);

            const char *const OD  = out.data();
            const int         LOD = out.length();

            Obj t(X);        ASSERT(W != t);  ASSERT(X == t);  ASSERT(Y != t);
            In in(OD, LOD);  ASSERT(in);
            in.setQuiet(!veryVerbose);
            in >> t;         ASSERT(!in);
                             ASSERT(W != t);  ASSERT(X == t);  ASSERT(Y != t);
        }
        if (verbose) cout << "\t\t\t"
                 "typIncomingMessageSize < incomingMessageSize." << endl;
        {
            const char version = 1;

            Out out;
            out.putVersion(version);

            out.putInt32(MAXCONNECTIONS[2]);
            out.putInt32(MAXNUMTHREADS[2]);
            out.putInt32(MAXWRITECACHE[2]);
            out.putFloat64(READTIMEOUT[2]);
            out.putFloat64(METRICSINTERVAL[2]);
            out.putInt32(MINMESSAGESIZEOUT[2]);
            out.putInt32(TYPMESSAGESIZEOUT[2]);
            out.putInt32(MAXMESSAGESIZEOUT[2]);
            out.putInt32(MINMESSAGESIZEIN[2]);
            out.putInt32(MINMESSAGESIZEIN[2] - 1);
            out.putInt32(MAXMESSAGESIZEIN[2]);
            out.putInt32(WORKLOADTHRESHOLD[2]);

            const char *const OD  = out.data();
            const int         LOD = out.length();

            Obj t(X);        ASSERT(W != t);  ASSERT(X == t);  ASSERT(Y != t);
            In in(OD, LOD);  ASSERT(in);
            in.setQuiet(!veryVerbose);
            in >> t;         ASSERT(!in);
                             ASSERT(W != t);  ASSERT(X == t);  ASSERT(Y != t);
        }
        if (verbose) cout << "\t\t\t"
                 "maxIncomingMessageSize < typIncomingMessageSize." << endl;
        {
            const char version = 1;

            Out out;
            out.putVersion(version);

            out.putInt32(MAXCONNECTIONS[2]);
            out.putInt32(MAXNUMTHREADS[2]);
            out.putInt32(MAXWRITECACHE[2]);
            out.putFloat64(READTIMEOUT[2]);
            out.putFloat64(METRICSINTERVAL[2]);
            out.putInt32(MINMESSAGESIZEOUT[2]);
            out.putInt32(TYPMESSAGESIZEOUT[2]);
            out.putInt32(MAXMESSAGESIZEOUT[2]);
            out.putInt32(MINMESSAGESIZEIN[2]);
            out.putInt32(TYPMESSAGESIZEIN[2]);
            out.putInt32(TYPMESSAGESIZEIN[2] - 1);
            out.putInt32(WORKLOADTHRESHOLD[2]);

            const char *const OD  = out.data();
            const int         LOD = out.length();

            Obj t(X);        ASSERT(W != t);  ASSERT(X == t);  ASSERT(Y != t);
            In in(OD, LOD);  ASSERT(in);
            in.setQuiet(!veryVerbose);
            in >> t;         ASSERT(!in);
                             ASSERT(W != t);  ASSERT(X == t);  ASSERT(Y != t);
        }
        if (verbose) cout << "\t\t\tworkloadThreshold neg." <<endl;
        {
            const char version = 1;

            Out out;
            out.putVersion(version);

            out.putInt32(MAXCONNECTIONS[2]);
            out.putInt32(MAXNUMTHREADS[2]);
            out.putInt32(MAXWRITECACHE[2]);
            out.putFloat64(READTIMEOUT[2]);
            out.putFloat64(METRICSINTERVAL[2]);
            out.putInt32(MINMESSAGESIZEOUT[2]);
            out.putInt32(TYPMESSAGESIZEOUT[2]);
            out.putInt32(MAXMESSAGESIZEOUT[2]);
            out.putInt32(MINMESSAGESIZEIN[2]);
            out.putInt32(TYPMESSAGESIZEIN[2]);
            out.putInt32(MAXMESSAGESIZEIN[2]);
            out.putInt32(-WORKLOADTHRESHOLD[2]);

            const char *const OD  = out.data();
            const int         LOD = out.length();

            Obj t(X);        ASSERT(W != t);  ASSERT(X == t);  ASSERT(Y != t);
            In in(OD, LOD);  ASSERT(in);
            in.setQuiet(!veryVerbose);
            in >> t;         ASSERT(!in);
                             ASSERT(W != t);  ASSERT(X == t);  ASSERT(Y != t);
        }

        if (verbose) cout << "\nTesting 'maxSupportedBdexVersion()'." << endl;
        {
            const Obj X;
            ASSERT(1 == X.maxSupportedBdexVersion());
        }

// TBD
#if 0
        if (verbose) cout << "\nWire format direct tests." << endl;
        {
            static const struct {
                int         d_lineNum;     // source line number
                int         d_second;      // specification second
                int         d_nanosecond;  // specification nanosecond
                int         d_version;     // version to stream with
                int         d_length;      // expect output length
                const char *d_fmt_p;       // expected output format
            } DATA[] = {
                //line  sec  nano  ver  len  format
                //----  ---  ----  ---  ---  -------------------------------
                { L_,   14,   10,   0,   0,  ""                              },
                { L_,   20,    8,   0,   0,  ""                              },

                { L_,   14,   10,   1,  12,
                          "\x00\x00\x00\x00\x00\x00\x00\x0e\x00\x00\x00\x0a" },
                { L_,   20,    8,   1,  12,
                          "\x00\x00\x00\x00\x00\x00\x00\x14\x00\x00\x00\x08" },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int LINE        = DATA[i].d_lineNum;
                const int SECOND      = DATA[i].d_second;
                const int NANOSECOND  = DATA[i].d_nanosecond;
                const int VERSION     = DATA[i].d_version;
                const int LEN         = DATA[i].d_length;
                const char *const FMT = DATA[i].d_fmt_p;

                Obj mX(SECOND, NANOSECOND);  const Obj& X = mX;
                bdex_ByteOutStream out;  X.streamOut(out, VERSION);

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
                    mY.streamIn(in, VERSION);
                }
                else { // version is not supported
                    mY = X;
                    bdex_ByteInStream in;
                    mY.streamIn(in, VERSION);
                    LOOP_ASSERT(LINE, !in);
                }
                LOOP_ASSERT(LINE, X == Y);
            }
        }
#endif
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // Constraints Test:
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Constraints Test" << endl
                          << "================" << endl;

        Obj mX1; const Obj& X1 = mX1;

        if (verbose) cout << "\t Check maxConnections contraint. " << endl;
        {
            ASSERT(0 != mX1.setMaxConnections(-1));
            ASSERT(MAXCONNECTIONS[0] == X1.maxConnections());
            ASSERT(0 == mX1.setMaxConnections(0));
            ASSERT(0 == X1.maxConnections());
            ASSERT(0 == mX1.setMaxConnections(1));
            ASSERT(1 == X1.maxConnections());
            ASSERT(0 == mX1.setMaxConnections(2));
            ASSERT(2 == X1.maxConnections());
        }
        if (verbose) cout << "\t Check maxIoThreads contraint. " << endl;
        {
            ASSERT(0 != mX1.setMaxIoThreads(-1));
            ASSERT(MAXNUMTHREADS[0] == mX1.maxIoThreads());
            ASSERT(0 == mX1.setMaxIoThreads(0));
            ASSERT(0 == X1.maxIoThreads());
            ASSERT(0 == mX1.setMaxIoThreads(1));
            ASSERT(1 == X1.maxIoThreads());
        }
        if (verbose) cout << "\t Check maxWriteCache contraint. " << endl;
        {
            ASSERT(0 != mX1.setMaxWriteCache(-1));
            ASSERT(MAXWRITECACHE[0] == mX1.maxWriteCache());
            ASSERT(0 == mX1.setMaxWriteCache(0));
            ASSERT(0 == X1.maxWriteCache());
            ASSERT(0 == mX1.setMaxWriteCache(1));
            ASSERT(1 == X1.maxWriteCache());
        }
        if (verbose) cout << "\t Check readTimeOut contraint. " << endl;
        {
            ASSERT(0 != mX1.setReadTimeout(-1.1));
            ASSERT(READTIMEOUT[0] == X1.readTimeout());
            ASSERT(0 == mX1.setReadTimeout(0.0));
            ASSERT(0.0 == X1.readTimeout());
            ASSERT(0 == mX1.setReadTimeout(0.1));
            ASSERT(0.1 == X1.readTimeout());
        }
        if (verbose) cout << "\t Check metricsInterval contraint. " << endl;
        {
            ASSERT(0 != mX1.setMetricsInterval(-1.1));
            ASSERT(METRICSINTERVAL[0] == X1.metricsInterval());
            ASSERT(0 == mX1.setMetricsInterval(0.0));
            ASSERT(0.0 == X1.metricsInterval());
            ASSERT(0 == mX1.setMetricsInterval(0.1));
            ASSERT(0.1 == X1.metricsInterval());
        }
        if (verbose) cout << "\t Check workloadThreshold contraint. " << endl;
        {
            ASSERT(0 != mX1.setWorkloadThreshold(-1));
            ASSERT(1 == X1.workloadThreshold());
            ASSERT(0 == mX1.setWorkloadThreshold(0));
            ASSERT(0 == X1.workloadThreshold());
            ASSERT(0 == mX1.setWorkloadThreshold(1));
            ASSERT(1 == X1.workloadThreshold());
        }
        if (verbose) cout << "\t Check messageSizeIn contraint. " << endl;
        {
            ASSERT(0 != mX1.setIncomingMessageSize(-1,  1,  1));
            ASSERT(0 != mX1.setIncomingMessageSize( 1, -1,  1));
            ASSERT(0 != mX1.setIncomingMessageSize( 1,  1, -1));
            ASSERT(0 != mX1.setIncomingMessageSize(-1, -1,  0));
            ASSERT(0 != mX1.setIncomingMessageSize(-1, -1, -1));

            ASSERT(0 != mX1.setIncomingMessageSize(1, 0, 1));
            ASSERT(0 != mX1.setIncomingMessageSize(1, 1, 0));
            ASSERT(0 != mX1.setIncomingMessageSize(0, 2, 1));

            ASSERT(1 == X1.incomingMessageSize());
            ASSERT(1 == X1.typicalIncomingMessageSize());
            ASSERT(1 == X1.maxIncomingMessageSize());

            ASSERT(0 == mX1.setIncomingMessageSize(0, 0, 0));
            ASSERT(0 == X1.incomingMessageSize());
            ASSERT(0 == X1.typicalIncomingMessageSize());
            ASSERT(0 == X1.maxIncomingMessageSize());

            ASSERT(0 == mX1.setIncomingMessageSize(1, 1, 1));
            ASSERT(1 == X1.incomingMessageSize());
            ASSERT(1 == X1.typicalIncomingMessageSize());
            ASSERT(1 == X1.maxIncomingMessageSize());

            ASSERT(0 == mX1.setIncomingMessageSize(0, 1, 1));
            ASSERT(0 == X1.incomingMessageSize());
            ASSERT(1 == X1.typicalIncomingMessageSize());
            ASSERT(1 == X1.maxIncomingMessageSize());

            ASSERT(0 == mX1.setIncomingMessageSize(0, 1, 2));
            ASSERT(0 == X1.incomingMessageSize());
            ASSERT(1 == X1.typicalIncomingMessageSize());
            ASSERT(2 == X1.maxIncomingMessageSize());
        }
        if (verbose) cout << "\t Check messageSizeOut contraint. " << endl;
        {
            ASSERT(0 != mX1.setOutgoingMessageSize(-1,  1,  1));
            ASSERT(0 != mX1.setOutgoingMessageSize( 1, -1,  1));
            ASSERT(0 != mX1.setOutgoingMessageSize( 1,  1, -1));
            ASSERT(0 != mX1.setOutgoingMessageSize(-1, -1,  0));
            ASSERT(0 != mX1.setOutgoingMessageSize(-1, -1, -1));

            ASSERT(0 != mX1.setOutgoingMessageSize(1, 0, 1));
            ASSERT(0 != mX1.setOutgoingMessageSize(1, 1, 0));
            ASSERT(0 != mX1.setOutgoingMessageSize(0, 2, 1));

            ASSERT(1 == X1.minOutgoingMessageSize());
            ASSERT(1 == X1.typicalOutgoingMessageSize());
            ASSERT(1 == X1.maxOutgoingMessageSize());

            ASSERT(0 == mX1.setOutgoingMessageSize(0, 0, 0));
            ASSERT(0 == X1.minOutgoingMessageSize());
            ASSERT(0 == X1.typicalOutgoingMessageSize());
            ASSERT(0 == X1.maxOutgoingMessageSize());

            ASSERT(0 == mX1.setOutgoingMessageSize(1, 1, 1));
            ASSERT(1 == X1.minOutgoingMessageSize());
            ASSERT(1 == X1.typicalOutgoingMessageSize());
            ASSERT(1 == X1.maxOutgoingMessageSize());

            ASSERT(0 == mX1.setOutgoingMessageSize(0, 1, 1));
            ASSERT(0 == X1.minOutgoingMessageSize());
            ASSERT(1 == X1.typicalOutgoingMessageSize());
            ASSERT(1 == X1.maxOutgoingMessageSize());

            ASSERT(0 == mX1.setOutgoingMessageSize(0, 1, 2));
            ASSERT(0 == X1.minOutgoingMessageSize());
            ASSERT(1 == X1.typicalOutgoingMessageSize());
            ASSERT(2 == X1.maxOutgoingMessageSize());
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // Basic Attribute Test:
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Basic Attribute Test" << endl
                          << "=====================" << endl;

        Obj mX1, mY1; const Obj& X1 = mX1; const Obj& Y1 = mY1;
        Obj mZ1; const Obj& Z1 = mZ1; // Z1 is the control
        if (verbose) { cout << '\t';  P(X1); }

        if (verbose) cout << "\n Check default ctor. " << endl;

        ASSERT( MINMESSAGESIZEIN[0] == X1.incomingMessageSize());
        ASSERT( TYPMESSAGESIZEIN[0] == X1.typicalIncomingMessageSize());
        ASSERT( MAXMESSAGESIZEIN[0] == X1.maxIncomingMessageSize());
        ASSERT(MINMESSAGESIZEOUT[0] == X1.minOutgoingMessageSize());
        ASSERT(TYPMESSAGESIZEOUT[0] == X1.typicalOutgoingMessageSize());
        ASSERT(MAXMESSAGESIZEOUT[0] == X1.maxOutgoingMessageSize());
        ASSERT(   MAXCONNECTIONS[0] == X1.maxConnections());
        ASSERT(       MAXNUMTHREADS[0] == X1.maxIoThreads());
        ASSERT(    MAXWRITECACHE[0] == X1.maxWriteCache());
        ASSERT(  METRICSINTERVAL[0] == X1.metricsInterval());
        ASSERT(      READTIMEOUT[0] == X1.readTimeout());
        ASSERT(WORKLOADTHRESHOLD[0] == X1.workloadThreshold());
        ASSERT(1 == (X1 == X1));          ASSERT(0 == (X1 != X1));
        ASSERT(1 == (X1 == Z1));          ASSERT(0 == (X1 != Z1));
        ASSERT(1 == (Z1 == Y1));          ASSERT(0 == (Z1 != Y1));
        ASSERT(1 == (Y1 == Z1));          ASSERT(0 == (Y1 != Z1));

        if (verbose) cout << "\t    Setting default values explicitly." <<endl;

        ASSERT(0 == mX1.setIncomingMessageSize(MINMESSAGESIZEIN[0],
                                                TYPMESSAGESIZEIN[0],
                                                MAXMESSAGESIZEIN[0]));
        ASSERT(0 == mX1.setOutgoingMessageSize(MINMESSAGESIZEOUT[0],
                                                TYPMESSAGESIZEOUT[0],
                                                MAXMESSAGESIZEOUT[0]));
        ASSERT(0 == mX1.setMaxConnections(MAXCONNECTIONS[0]));
        ASSERT(0 == mX1.setMaxIoThreads(MAXNUMTHREADS[0]));
        ASSERT(0 == mX1.setMaxWriteCache(MAXWRITECACHE[0]));
        ASSERT(0 == mX1.setMetricsInterval(METRICSINTERVAL[0]));
        ASSERT(0 == mX1.setReadTimeout(READTIMEOUT[0]));
        ASSERT(0 == mX1.setWorkloadThreshold(WORKLOADTHRESHOLD[0]));
        ASSERT(1 == (X1 == X1));          ASSERT(0 == (X1 != X1));
        ASSERT(1 == (X1 == Z1));          ASSERT(0 == (X1 != Z1));
        ASSERT(1 == (Z1 == Y1));          ASSERT(0 == (Z1 != Y1));
        ASSERT(1 == (Y1 == Z1));          ASSERT(0 == (Y1 != Z1));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\tTesting set/get methods."
                          << "\n\t  Change attribute 0." << endl;

        ASSERT(0 == mX1.setIncomingMessageSize(MINMESSAGESIZEIN[1],
                                                TYPMESSAGESIZEIN[1],
                                                MAXMESSAGESIZEIN[1]));
        ASSERT( MINMESSAGESIZEIN[1] == X1.incomingMessageSize());
        ASSERT( TYPMESSAGESIZEIN[1] == X1.typicalIncomingMessageSize());
        ASSERT( MAXMESSAGESIZEIN[1] == X1.maxIncomingMessageSize());
        ASSERT(MINMESSAGESIZEOUT[0] == X1.minOutgoingMessageSize());
        ASSERT(TYPMESSAGESIZEOUT[0] == X1.typicalOutgoingMessageSize());
        ASSERT(MAXMESSAGESIZEOUT[0] == X1.maxOutgoingMessageSize());
        ASSERT(   MAXCONNECTIONS[0] == X1.maxConnections());
        ASSERT(       MAXNUMTHREADS[0] == X1.maxIoThreads());
        ASSERT(    MAXWRITECACHE[0] == X1.maxWriteCache());
        ASSERT(  METRICSINTERVAL[0] == X1.metricsInterval());
        ASSERT(      READTIMEOUT[0] == X1.readTimeout());
        ASSERT(WORKLOADTHRESHOLD[0] == X1.workloadThreshold());
        ASSERT(1 == (X1 == X1));          ASSERT(0 == (X1 != X1));
        ASSERT(0 == (X1 == Z1));          ASSERT(1 == (X1 != Z1));
        ASSERT(0 == (Z1 == X1));          ASSERT(1 == (Z1 != X1));
        ASSERT(1 == (Y1 == Z1));          ASSERT(0 == (Y1 != Z1));
        {
            Obj C(X1);
            ASSERT(C == X1 == 1);          ASSERT(C != X1 == 0);
        }

        mY1 = X1;
        ASSERT(1 == (Y1 == Y1));          ASSERT(0 == (Y1 != Y1));
        ASSERT(1 == (Y1 == X1));          ASSERT(0 == (Y1 != X1));
        ASSERT(0 == (Y1 == Z1));          ASSERT(1 == (Y1 != Z1));

        ASSERT(0 == mX1.setIncomingMessageSize(MINMESSAGESIZEIN[0],
                                                TYPMESSAGESIZEIN[0],
                                                MAXMESSAGESIZEIN[0]));
        ASSERT(1 == (X1 == X1));          ASSERT(0 == (X1 != X1));
        ASSERT(1 == (X1 == Z1));          ASSERT(0 == (X1 != Z1));
        ASSERT(0 == (Y1 == Z1));          ASSERT(1 == (Y1 != Z1));

        mX1 = mY1 = Z1;
        ASSERT(1 == (X1 == X1));          ASSERT(0 == (X1 != X1));
        ASSERT(1 == (X1 == Z1));          ASSERT(0 == (X1 != Z1));
        ASSERT(1 == (Y1 == Z1));          ASSERT(0 == (Y1 != Z1));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\t Change attribute 1." << endl;

        ASSERT(0 == mX1.setOutgoingMessageSize(MINMESSAGESIZEOUT[1],
                                                TYPMESSAGESIZEOUT[1],
                                                MAXMESSAGESIZEOUT[1]));
        ASSERT( MINMESSAGESIZEIN[0] == X1.incomingMessageSize());
        ASSERT( TYPMESSAGESIZEIN[0] == X1.typicalIncomingMessageSize());
        ASSERT( MAXMESSAGESIZEIN[0] == X1.maxIncomingMessageSize());
        ASSERT(MINMESSAGESIZEOUT[1] == X1.minOutgoingMessageSize());
        ASSERT(TYPMESSAGESIZEOUT[1] == X1.typicalOutgoingMessageSize());
        ASSERT(MAXMESSAGESIZEOUT[1] == X1.maxOutgoingMessageSize());
        ASSERT(   MAXCONNECTIONS[0] == X1.maxConnections());
        ASSERT(       MAXNUMTHREADS[0] == X1.maxIoThreads());
        ASSERT(    MAXWRITECACHE[0] == X1.maxWriteCache());
        ASSERT(  METRICSINTERVAL[0] == X1.metricsInterval());
        ASSERT(      READTIMEOUT[0] == X1.readTimeout());
        ASSERT(WORKLOADTHRESHOLD[0] == X1.workloadThreshold());
        ASSERT(1 == (X1 == X1));          ASSERT(0 == (X1 != X1));
        ASSERT(0 == (X1 == Z1));          ASSERT(1 == (X1 != Z1));
        ASSERT(0 == (Z1 == X1));          ASSERT(1 == (Z1 != X1));
        ASSERT(1 == (Y1 == Z1));          ASSERT(0 == (Y1 != Z1));
        {
            Obj C(X1);
            ASSERT(C == X1 == 1);          ASSERT(C != X1 == 0);
        }

        mY1 = X1;
        ASSERT(1 == (Y1 == Y1));          ASSERT(0 == (Y1 != Y1));
        ASSERT(1 == (Y1 == X1));          ASSERT(0 == (Y1 != X1));
        ASSERT(0 == (Y1 == Z1));          ASSERT(1 == (Y1 != Z1));

        ASSERT(0 == mX1.setOutgoingMessageSize(MINMESSAGESIZEOUT[0],
                                                TYPMESSAGESIZEOUT[0],
                                                MAXMESSAGESIZEOUT[0]));
        ASSERT(1 == (X1 == X1));          ASSERT(0 == (X1 != X1));
        ASSERT(1 == (X1 == Z1));          ASSERT(0 == (X1 != Z1));
        ASSERT(0 == (Y1 == Z1));          ASSERT(1 == (Y1 != Z1));

        mX1 = mY1 = Z1;
        ASSERT(1 == (X1 == X1));          ASSERT(0 == (X1 != X1));
        ASSERT(1 == (X1 == Z1));          ASSERT(0 == (X1 != Z1));
        ASSERT(1 == (Y1 == Z1));          ASSERT(0 == (Y1 != Z1));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\t Change attribute 2." << endl;

        ASSERT(0 == mX1.setMaxConnections(MAXCONNECTIONS[1]));
        ASSERT( MINMESSAGESIZEIN[0] == X1.incomingMessageSize());
        ASSERT( TYPMESSAGESIZEIN[0] == X1.typicalIncomingMessageSize());
        ASSERT( MAXMESSAGESIZEIN[0] == X1.maxIncomingMessageSize());
        ASSERT(MINMESSAGESIZEOUT[0] == X1.minOutgoingMessageSize());
        ASSERT(TYPMESSAGESIZEOUT[0] == X1.typicalOutgoingMessageSize());
        ASSERT(MAXMESSAGESIZEOUT[0] == X1.maxOutgoingMessageSize());
        ASSERT(   MAXCONNECTIONS[1] == X1.maxConnections());
        ASSERT(       MAXNUMTHREADS[0] == X1.maxIoThreads());
        ASSERT(    MAXWRITECACHE[0] == X1.maxWriteCache());
        ASSERT(  METRICSINTERVAL[0] == X1.metricsInterval());
        ASSERT(      READTIMEOUT[0] == X1.readTimeout());
        ASSERT(WORKLOADTHRESHOLD[0] == X1.workloadThreshold());
        ASSERT(1 == (X1 == X1));          ASSERT(0 == (X1 != X1));
        ASSERT(0 == (X1 == Z1));          ASSERT(1 == (X1 != Z1));
        ASSERT(0 == (Z1 == X1));          ASSERT(1 == (Z1 != X1));
        ASSERT(1 == (Y1 == Z1));          ASSERT(0 == (Y1 != Z1));
        {
            Obj C(X1);
            ASSERT(C == X1 == 1);          ASSERT(C != X1 == 0);
        }

        mY1 = X1;
        ASSERT(1 == (Y1 == Y1));          ASSERT(0 == (Y1 != Y1));
        ASSERT(1 == (Y1 == X1));          ASSERT(0 == (Y1 != X1));
        ASSERT(0 == (Y1 == Z1));          ASSERT(1 == (Y1 != Z1));

        ASSERT(0 == mX1.setMaxConnections(MAXCONNECTIONS[0]));
        ASSERT(1 == (X1 == X1));          ASSERT(0 == (X1 != X1));
        ASSERT(1 == (X1 == Z1));          ASSERT(0 == (X1 != Z1));
        ASSERT(0 == (Y1 == Z1));          ASSERT(1 == (Y1 != Z1));

        mX1 = mY1 = Z1;
        ASSERT(1 == (X1 == X1));          ASSERT(0 == (X1 != X1));
        ASSERT(1 == (X1 == Z1));          ASSERT(0 == (X1 != Z1));
        ASSERT(1 == (Y1 == Z1));          ASSERT(0 == (Y1 != Z1));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\t Change attribute 3." << endl;

        ASSERT(0 == mX1.setMaxIoThreads(MAXNUMTHREADS[1]));
        ASSERT( MINMESSAGESIZEIN[0] == X1.incomingMessageSize());
        ASSERT( TYPMESSAGESIZEIN[0] == X1.typicalIncomingMessageSize());
        ASSERT( MAXMESSAGESIZEIN[0] == X1.maxIncomingMessageSize());
        ASSERT(MINMESSAGESIZEOUT[0] == X1.minOutgoingMessageSize());
        ASSERT(TYPMESSAGESIZEOUT[0] == X1.typicalOutgoingMessageSize());
        ASSERT(MAXMESSAGESIZEOUT[0] == X1.maxOutgoingMessageSize());
        ASSERT(   MAXCONNECTIONS[0] == X1.maxConnections());
        ASSERT(       MAXNUMTHREADS[1] == X1.maxIoThreads());
        ASSERT(    MAXWRITECACHE[0] == X1.maxWriteCache());
        ASSERT(  METRICSINTERVAL[0] == X1.metricsInterval());
        ASSERT(      READTIMEOUT[0] == X1.readTimeout());
        ASSERT(WORKLOADTHRESHOLD[0] == X1.workloadThreshold());
        ASSERT(1 == (X1 == X1));          ASSERT(0 == (X1 != X1));
        ASSERT(0 == (X1 == Z1));          ASSERT(1 == (X1 != Z1));
        ASSERT(0 == (Z1 == X1));          ASSERT(1 == (Z1 != X1));
        ASSERT(1 == (Y1 == Z1));          ASSERT(0 == (Y1 != Z1));
        {
            Obj C(X1);
            ASSERT(C == X1 == 1);          ASSERT(C != X1 == 0);
        }

        mY1 = X1;
        ASSERT(1 == (Y1 == Y1));          ASSERT(0 == (Y1 != Y1));
        ASSERT(1 == (Y1 == X1));          ASSERT(0 == (Y1 != X1));
        ASSERT(0 == (Y1 == Z1));          ASSERT(1 == (Y1 != Z1));

        ASSERT(0 == mX1.setMaxIoThreads(MAXNUMTHREADS[0]));
        ASSERT(1 == (X1 == X1));          ASSERT(0 == (X1 != X1));
        ASSERT(1 == (X1 == Z1));          ASSERT(0 == (X1 != Z1));
        ASSERT(0 == (Y1 == Z1));          ASSERT(1 == (Y1 != Z1));

        mX1 = mY1 = Z1;
        ASSERT(1 == (X1 == X1));          ASSERT(0 == (X1 != X1));
        ASSERT(1 == (X1 == Z1));          ASSERT(0 == (X1 != Z1));
        ASSERT(1 == (Y1 == Z1));          ASSERT(0 == (Y1 != Z1));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\t Change attribute 4." << endl;

        ASSERT(0 == mX1.setMaxWriteCache(MAXWRITECACHE[1]));
        ASSERT( MINMESSAGESIZEIN[0] == X1.incomingMessageSize());
        ASSERT( TYPMESSAGESIZEIN[0] == X1.typicalIncomingMessageSize());
        ASSERT( MAXMESSAGESIZEIN[0] == X1.maxIncomingMessageSize());
        ASSERT(MINMESSAGESIZEOUT[0] == X1.minOutgoingMessageSize());
        ASSERT(TYPMESSAGESIZEOUT[0] == X1.typicalOutgoingMessageSize());
        ASSERT(MAXMESSAGESIZEOUT[0] == X1.maxOutgoingMessageSize());
        ASSERT(   MAXCONNECTIONS[0] == X1.maxConnections());
        ASSERT(       MAXNUMTHREADS[0] == X1.maxIoThreads());
        ASSERT(    MAXWRITECACHE[1] == X1.maxWriteCache());
        ASSERT(  METRICSINTERVAL[0] == X1.metricsInterval());
        ASSERT(      READTIMEOUT[0] == X1.readTimeout());
        ASSERT(WORKLOADTHRESHOLD[0] == X1.workloadThreshold());
        ASSERT(1 == (X1 == X1));          ASSERT(0 == (X1 != X1));
        ASSERT(0 == (X1 == Z1));          ASSERT(1 == (X1 != Z1));
        ASSERT(0 == (Z1 == X1));          ASSERT(1 == (Z1 != X1));
        ASSERT(1 == (Y1 == Z1));          ASSERT(0 == (Y1 != Z1));
        {
            Obj C(X1);
            ASSERT(C == X1 == 1);          ASSERT(C != X1 == 0);
        }

        mY1 = X1;
        ASSERT(1 == (Y1 == Y1));          ASSERT(0 == (Y1 != Y1));
        ASSERT(1 == (Y1 == X1));          ASSERT(0 == (Y1 != X1));
        ASSERT(0 == (Y1 == Z1));          ASSERT(1 == (Y1 != Z1));

        ASSERT(0 == mX1.setMaxWriteCache(MAXWRITECACHE[0]));
        ASSERT(1 == (X1 == X1));          ASSERT(0 == (X1 != X1));
        ASSERT(1 == (X1 == Z1));          ASSERT(0 == (X1 != Z1));
        ASSERT(0 == (Y1 == Z1));          ASSERT(1 == (Y1 != Z1));

        mX1 = mY1 = Z1;
        ASSERT(1 == (X1 == X1));          ASSERT(0 == (X1 != X1));
        ASSERT(1 == (X1 == Z1));          ASSERT(0 == (X1 != Z1));
        ASSERT(1 == (Y1 == Z1));          ASSERT(0 == (Y1 != Z1));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\t Change attribute 5." << endl;

        ASSERT(0 == mX1.setMetricsInterval(METRICSINTERVAL[1]));
        ASSERT( MINMESSAGESIZEIN[0] == X1.incomingMessageSize());
        ASSERT( TYPMESSAGESIZEIN[0] == X1.typicalIncomingMessageSize());
        ASSERT( MAXMESSAGESIZEIN[0] == X1.maxIncomingMessageSize());
        ASSERT(MINMESSAGESIZEOUT[0] == X1.minOutgoingMessageSize());
        ASSERT(TYPMESSAGESIZEOUT[0] == X1.typicalOutgoingMessageSize());
        ASSERT(MAXMESSAGESIZEOUT[0] == X1.maxOutgoingMessageSize());
        ASSERT(   MAXCONNECTIONS[0] == X1.maxConnections());
        ASSERT(       MAXNUMTHREADS[0] == X1.maxIoThreads());
        ASSERT(    MAXWRITECACHE[0] == X1.maxWriteCache());
        ASSERT(  METRICSINTERVAL[1] == X1.metricsInterval());
        ASSERT(      READTIMEOUT[0] == X1.readTimeout());
        ASSERT(WORKLOADTHRESHOLD[0] == X1.workloadThreshold());
        ASSERT(1 == (X1 == X1));          ASSERT(0 == (X1 != X1));
        ASSERT(0 == (X1 == Z1));          ASSERT(1 == (X1 != Z1));
        ASSERT(0 == (Z1 == X1));          ASSERT(1 == (Z1 != X1));
        ASSERT(1 == (Y1 == Z1));          ASSERT(0 == (Y1 != Z1));
        {
            Obj C(X1);
            ASSERT(C == X1 == 1);          ASSERT(C != X1 == 0);
        }

        mY1 = X1;
        ASSERT(1 == (Y1 == Y1));          ASSERT(0 == (Y1 != Y1));
        ASSERT(1 == (Y1 == X1));          ASSERT(0 == (Y1 != X1));
        ASSERT(0 == (Y1 == Z1));          ASSERT(1 == (Y1 != Z1));

        ASSERT(0 == mX1.setMetricsInterval(METRICSINTERVAL[0]));
        ASSERT(1 == (X1 == X1));          ASSERT(0 == (X1 != X1));
        ASSERT(1 == (X1 == Z1));          ASSERT(0 == (X1 != Z1));
        ASSERT(0 == (Y1 == Z1));          ASSERT(1 == (Y1 != Z1));

        mX1 = mY1 = Z1;
        ASSERT(1 == (X1 == X1));          ASSERT(0 == (X1 != X1));
        ASSERT(1 == (X1 == Z1));          ASSERT(0 == (X1 != Z1));
        ASSERT(1 == (Y1 == Z1));          ASSERT(0 == (Y1 != Z1));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\t Change attribute 6." << endl;

        ASSERT(0 == mX1.setReadTimeout(READTIMEOUT[1]));
        ASSERT( MINMESSAGESIZEIN[0] == X1.incomingMessageSize());
        ASSERT( TYPMESSAGESIZEIN[0] == X1.typicalIncomingMessageSize());
        ASSERT( MAXMESSAGESIZEIN[0] == X1.maxIncomingMessageSize());
        ASSERT(MINMESSAGESIZEOUT[0] == X1.minOutgoingMessageSize());
        ASSERT(TYPMESSAGESIZEOUT[0] == X1.typicalOutgoingMessageSize());
        ASSERT(MAXMESSAGESIZEOUT[0] == X1.maxOutgoingMessageSize());
        ASSERT(   MAXCONNECTIONS[0] == X1.maxConnections());
        ASSERT(       MAXNUMTHREADS[0] == X1.maxIoThreads());
        ASSERT(    MAXWRITECACHE[0] == X1.maxWriteCache());
        ASSERT(  METRICSINTERVAL[0] == X1.metricsInterval());
        ASSERT(      READTIMEOUT[1] == X1.readTimeout());
        ASSERT(WORKLOADTHRESHOLD[0] == X1.workloadThreshold());
        ASSERT(1 == (X1 == X1));          ASSERT(0 == (X1 != X1));
        ASSERT(0 == (X1 == Z1));          ASSERT(1 == (X1 != Z1));
        ASSERT(0 == (Z1 == X1));          ASSERT(1 == (Z1 != X1));
        ASSERT(1 == (Y1 == Z1));          ASSERT(0 == (Y1 != Z1));
        {
            Obj C(X1);
            ASSERT(C == X1 == 1);          ASSERT(C != X1 == 0);
        }

        mY1 = X1;
        ASSERT(1 == (Y1 == Y1));          ASSERT(0 == (Y1 != Y1));
        ASSERT(1 == (Y1 == X1));          ASSERT(0 == (Y1 != X1));
        ASSERT(0 == (Y1 == Z1));          ASSERT(1 == (Y1 != Z1));

        ASSERT(0 == mX1.setReadTimeout(READTIMEOUT[0]));
        ASSERT(1 == (X1 == X1));          ASSERT(0 == (X1 != X1));
        ASSERT(1 == (X1 == Z1));          ASSERT(0 == (X1 != Z1));
        ASSERT(0 == (Y1 == Z1));          ASSERT(1 == (Y1 != Z1));

        mX1 = mY1 = Z1;
        ASSERT(1 == (X1 == X1));          ASSERT(0 == (X1 != X1));
        ASSERT(1 == (X1 == Z1));          ASSERT(0 == (X1 != Z1));
        ASSERT(1 == (Y1 == Z1));          ASSERT(0 == (Y1 != Z1));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\t Change attribute 7." << endl;

        ASSERT(0 == mX1.setWorkloadThreshold(WORKLOADTHRESHOLD[1]));
        ASSERT( MINMESSAGESIZEIN[0] == X1.incomingMessageSize());
        ASSERT( TYPMESSAGESIZEIN[0] == X1.typicalIncomingMessageSize());
        ASSERT( MAXMESSAGESIZEIN[0] == X1.maxIncomingMessageSize());
        ASSERT(MINMESSAGESIZEOUT[0] == X1.minOutgoingMessageSize());
        ASSERT(TYPMESSAGESIZEOUT[0] == X1.typicalOutgoingMessageSize());
        ASSERT(MAXMESSAGESIZEOUT[0] == X1.maxOutgoingMessageSize());
        ASSERT(   MAXCONNECTIONS[0] == X1.maxConnections());
        ASSERT(       MAXNUMTHREADS[0] == X1.maxIoThreads());
        ASSERT(    MAXWRITECACHE[0] == X1.maxWriteCache());
        ASSERT(  METRICSINTERVAL[0] == X1.metricsInterval());
        ASSERT(      READTIMEOUT[0] == X1.readTimeout());
        ASSERT(WORKLOADTHRESHOLD[1] == X1.workloadThreshold());
        ASSERT(1 == (X1 == X1));          ASSERT(0 == (X1 != X1));
        ASSERT(0 == (X1 == Z1));          ASSERT(1 == (X1 != Z1));
        ASSERT(0 == (Z1 == X1));          ASSERT(1 == (Z1 != X1));
        ASSERT(1 == (Y1 == Z1));          ASSERT(0 == (Y1 != Z1));
        {
            Obj C(X1);
            ASSERT(C == X1 == 1);          ASSERT(C != X1 == 0);
        }

        mY1 = X1;
        ASSERT(1 == (Y1 == Y1));          ASSERT(0 == (Y1 != Y1));
        ASSERT(1 == (Y1 == X1));          ASSERT(0 == (Y1 != X1));
        ASSERT(0 == (Y1 == Z1));          ASSERT(1 == (Y1 != Z1));

        ASSERT(0 == mX1.setWorkloadThreshold(WORKLOADTHRESHOLD[0]));
        ASSERT(1 == (X1 == X1));          ASSERT(0 == (X1 != X1));
        ASSERT(1 == (X1 == Z1));          ASSERT(0 == (X1 != Z1));
        ASSERT(0 == (Y1 == Z1));          ASSERT(1 == (Y1 != Z1));

        mX1 = mY1 = Z1;
        ASSERT(1 == (X1 == X1));          ASSERT(0 == (X1 != X1));
        ASSERT(1 == (X1 == Z1));          ASSERT(0 == (X1 != Z1));
        ASSERT(1 == (Y1 == Z1));          ASSERT(0 == (Y1 != Z1));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "Testing output operator (<<)." << endl;

        ASSERT(0 == mY1.setIncomingMessageSize(MAXMESSAGESIZEIN[1]));
        ASSERT(0 == mY1.setOutgoingMessageSize(MAXMESSAGESIZEOUT[1]));
        ASSERT(0 == mY1.setMaxConnections(MAXCONNECTIONS[1]));
        ASSERT(0 == mY1.setMaxIoThreads(MAXNUMTHREADS[1]));
        ASSERT(0 == mY1.setMaxWriteCache(MAXWRITECACHE[1]));
        ASSERT(0 == mY1.setMetricsInterval(METRICSINTERVAL[1]));
        ASSERT(0 == mY1.setReadTimeout(READTIMEOUT[1]));
        ASSERT(0 == mY1.setWorkloadThreshold(WORKLOADTHRESHOLD[1]));
        ASSERT(mX1 != mY1);

        char buf[10000];
        {
            ostrstream o(buf, sizeof buf);
            o << X1 << ends;
            if (verbose) cout << "X1 buf:\n" << buf << endl;
            bsl::string s =
                "[" NL
                "\tmaxConnections         : 1" NL
                "\tmaxIoThreads             : 1" NL
                "\tmaxWriteCache          : 256" NL
                "\treadTimeout            : 30" NL
                "\tmetricsInterval        : 30" NL
                "\tminOutgoingMessageSize : 1" NL
                "\ttypOutgoingMessageSize : 1" NL
                "\tmaxOutgoingMessageSize : 1" NL
                "\tincomingMessageSize : 1" NL
                "\ttypIncomingMessageSize : 1" NL
                "\tmaxIncomingMessageSize : 1" NL
                "\tworkLoadthreshold      : 1" NL
                "]" NL
                ;
            ASSERT(buf == s);
        }
        {
            ostrstream o(buf, sizeof buf);
            o << Y1 << ends;
            if (verbose) cout << "Y1 buf:\n" << buf << endl;
            bsl::string s =
                "[" NL
                "\tmaxConnections         : 10" NL
                "\tmaxIoThreads           : 11" NL
                "\tmaxWriteCache          : 512" NL
                "\treadTimeout            : 60" NL
                "\tmetricsInterval        : 61.1" NL
                "\toutgoingMessageSize    : 12" NL
                "\tincomingMessageSize    : 15" NL

                "]" NL
                ;
            ASSERT(buf == s);
        }
      } break;
#endif
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
//      Copyright (C) Bloomberg L.P., 2002
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
