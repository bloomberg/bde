// bael_context.t.cpp         -*-C++-*-

#include <bael_context.h>

#include <bsls_platform.h>                      // for testing only
#include <bsls_platformutil.h>                  // for testing only

#include <bslma_testallocator.h>                // for testing only
#include <bslma_testallocatorexception.h>       // for testing only

#include <bdex_instreamfunctions.h>             // for testing only
#include <bdex_outstreamfunctions.h>            // for testing only
#include <bdex_byteinstream.h>                  // for testing only
#include <bdex_byteoutstream.h>                 // for testing only
#include <bdex_testinstream.h>                  // for testing only
#include <bdex_testinstreamexception.h>         // for testing only
#include <bdex_testoutstream.h>                 // for testing only

#include <bsl_climits.h>      // INT_MAX
#include <bsl_cstdlib.h>      // atoi()
#include <bsl_cstring.h>      // strlen(), memset(), memcpy(), memcmp()
#ifdef BSLS_PLATFORM_OS_UNIX
#include <unistd.h>     // getpid()
#endif

#include <bsl_new.h>          // placement 'new' syntax
#include <bsl_iostream.h>
#include <bsl_string.h>
#include <bsl_strstream.h>
#include <bsl_vector.h>

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// This test plan follows the standard approach for components implementing
// the attributes pattern.
//
// Note that places where test drivers in this family are likely to require
// adjustment are indicated by the tag: "ADJ".
//-----------------------------------------------------------------------------
// 'bael_Context' public interface:
// [ 3] static bool isValid(cause, index, length);
// [ 2] static int maxSupportedBdexVersion();
// [ 1] bael_Context(bslma_Allocator *ba = 0);
// [ 3] bael_Context(cause, index, length, *ba = 0);
// [ 1] bael_Context(const bael_Context& original, *ba = 0);
// [ 1] ~bael_Context();
// [ 1] bael_Context& operator=(const bael_Context& rhs);
// [ 3] int setAttributes(cause, index, length);
// [ 1] void setAttributesRaw(cause, index, length);
// [ 1] void setRecordIndexRaw(int index);
// [ 2] bdex_InStream& streamIn(bdex_InStream&);
// [ 2] bdex_InStream& streamIn(bdex_InStream&, int version);
// [ 1] bael_Transmission::Cause transmissionCause() const;
// [ 1] int recordIndex() const;
// [ 1] int sequenceLength() const;
// [ 1] ostream& print(ostream& os, int level = 0, int spl = 4) const;
// [ 2] bdex_OutStream& bdexStreamOut(bdex_OutStream&, int version) const;
//
// [ 1] bool operator==(const bael_Context& lhs, const bael_Context& rhs);
// [ 1] bool operator!=(const bael_Context& lhs, const bael_Context& rhs);
// [ 1] ostream& operator<<(ostream&, const bael_Context&);
//-----------------------------------------------------------------------------
// [ 4] USAGE EXAMPLE
//-----------------------------------------------------------------------------

//=============================================================================
//                      STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
static int testStatus = 0;

void aSsErT(int c, const char *s, int i)
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

#define LOOP4_ASSERT(I,J,K,L,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" << \
       #K << ": " << K << "\t" << #L << ": " << L << "\n"; \
       aSsErT(1, #X, __LINE__); } }

#define LOOP5_ASSERT(I,J,K,L,M,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" << \
       #K << ": " << K << "\t" << #L << ": " << L << "\t" << \
       #M << ": " << M << "\n"; \
       aSsErT(1, #X, __LINE__); } }

#define LOOP6_ASSERT(I,J,K,L,M,N,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" << \
       #K << ": " << K << "\t" << #L << ": " << L << "\t" << \
       #M << ": " << M << "\t" << #N << ": " << N << "\n"; \
       aSsErT(1, #X, __LINE__); } }

//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", "<< flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number
#define T_()  cout << "\t" << flush;          // Print tab w/o newline
#define NL "\n"

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

typedef bael_Context       Obj;
typedef bdex_TestInStream  In;
typedef bdex_TestOutStream Out;

const bael_Transmission::Cause CAUSE[] = {
    bael_Transmission::BAEL_PASSTHROUGH,
    bael_Transmission::BAEL_TRIGGER,
    bael_Transmission::BAEL_TRIGGER_ALL,
    bael_Transmission::BAEL_MANUAL_PUBLISH,
    bael_Transmission::BAEL_MANUAL_PUBLISH_ALL
};

const int INDEX[]  = { 0, 1, 8, 98,  99, 9998, INT_MAX-1 };
const int LENGTH[] = { 1, 2, 9, 99, 100, 9999, INT_MAX };

const int NUM_CAUSE  = sizeof CAUSE / sizeof *CAUSE;
const int NUM_INDEX  = sizeof INDEX / sizeof *INDEX;
const int NUM_LENGTH = sizeof LENGTH / sizeof *LENGTH;

//=============================================================================
//                             USAGE EXAMPLE
//-----------------------------------------------------------------------------

// my_logger.h

class my_Logger {

    bsl::vector<bsl::string> archive;  // log message archive
    ostream& d_os;

    // NOT IMPLEMENTED
    my_Logger(const my_Logger&);
    my_Logger& operator=(const my_Logger&);

    // PRIVATE MANIPULATORS
    void publish(const bsl::string& message, const bael_Context& context);

  public:
    // TYPES
    enum Severity { ERROR = 0, WARN = 1, TRACE = 2 };

    // CREATORS
    my_Logger(ostream& stream);
    ~my_Logger();

    // MANIPULATORS
    void logMessage(const bsl::string& message, Severity severity);
};

// my_Logger.cpp

// PRIVATE MANIPULATORS
void my_Logger::publish(const bsl::string&  message,
                        const bael_Context& context)
{
    switch (context.transmissionCause()) {
      case bael_Transmission::BAEL_PASSTHROUGH: {
        d_os << "Single Pass-through Message: ";
      } break;
      case bael_Transmission::BAEL_TRIGGER_ALL: {
        d_os << "Remotely ";               // no 'break'; concatenated output
      } break;
      case bael_Transmission::BAEL_TRIGGER: {
        d_os << "Triggered Publication Sequence: Message "
             << context.recordIndex() + 1  // Account for zero-based index.
             << " of " << context.sequenceLength() << ": ";
      } break;
      case bael_Transmission::BAEL_MANUAL_PUBLISH: {
        d_os << "Manually triggered Message: ";
      } break;
      default: {
        d_os << "***ERROR*** Unsupported Message Cause: ";
      } break;
    }
    d_os << message << endl;
}

// CREATORS
my_Logger::my_Logger(ostream& stream) : d_os(stream) { }
my_Logger::~my_Logger() { }

// MANIPULATORS
void my_Logger::logMessage(const bsl::string& message, Severity severity)
{
    archive.push_back(message);
    switch (severity) {
      case TRACE: {
        // Do nothing beyond archiving the message.
      } break;
      case WARN: {
        bael_Context context(bael_Transmission::BAEL_PASSTHROUGH, 0, 1);
        publish(message, context);
      } break;
      case ERROR: {
        int index  = 0;
        int length = (int)archive.size();
        bael_Context context(bael_Transmission::BAEL_TRIGGER, index, length);
        while (length--) {
            publish(archive[length], context);
            context.setRecordIndexRaw(++index);
        }
        archive.clear();  // flush archive
      } break;
    }
}

//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

//=============================================================================
//              GENERATOR FUNCTIONS 'g' AND 'gg' FOR TESTING
//-----------------------------------------------------------------------------

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

    bslma_TestAllocator testAllocator(veryVeryVerbose);

    switch (test) { case 0:  // Zero is always the leading case.
      case 4: {
        // --------------------------------------------------------------------
        // TESTING USAGE EXAMPLE
        //
        // Concerns:
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

        if (verbose) cout << endl << "Testing Usage Example" << endl
                                  << "=====================" << endl;

        char buf[1024];  memset(buf, 0xff, sizeof buf);  // Scribble on buf.
        ostrstream out(buf, sizeof buf);

        my_Logger   logger(out);
        bsl::string message;

        message = "TRACE 1";  logger.logMessage(message, my_Logger::TRACE);
        message = "TRACE 2";  logger.logMessage(message, my_Logger::TRACE);
        message = "WARNING";  logger.logMessage(message, my_Logger::WARN);
        message = "TRACE 3";  logger.logMessage(message, my_Logger::TRACE);
        message = "TROUBLE!"; logger.logMessage(message, my_Logger::ERROR);

        if (veryVerbose) { out << ends; cout << buf << endl; }

      } break;

      case 3: {
        // --------------------------------------------------------------------
        // Constraints Test:
        //
        // Testing:
        //   static bool isValid(cause, index, length);
        //   int setAttributes(cause, index, length);
        //   bael_Context(cause, index, length, *ba = 0);
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "Constraints Test" << endl
                                  << "================" << endl;

        ASSERT(NUM_INDEX == NUM_LENGTH);

        if (veryVerbose) cout << "\n\tPASSTHROUGH." << endl;
        if (veryVerbose) cout << "\t\tValid attributes." << endl;
        {
            ASSERT(1 == Obj::isValid(CAUSE[0], INDEX[0], LENGTH[0]));

            Obj mX(CAUSE[0], INDEX[0], LENGTH[0]);  const Obj& X = mX;
            if (veryVeryVerbose) { T_(); T_(); P(X); }
            ASSERT( CAUSE[0] == X.transmissionCause());
            ASSERT( INDEX[0] == X.recordIndex());
            ASSERT(LENGTH[0] == X.sequenceLength());

            Obj mY;  const Obj& Y = mY;
            mY.setAttributesRaw(CAUSE[1], INDEX[1], LENGTH[1]);
            ASSERT( CAUSE[0] != Y.transmissionCause());
            ASSERT( INDEX[0] != Y.recordIndex());
            ASSERT(LENGTH[0] != Y.sequenceLength());
            ASSERT(0 == mY.setAttributes(CAUSE[0], INDEX[0], LENGTH[0]));
            if (veryVeryVerbose) { T_(); T_(); P(Y); }
            ASSERT( CAUSE[0] == Y.transmissionCause());
            ASSERT( INDEX[0] == Y.recordIndex());
            ASSERT(LENGTH[0] == Y.sequenceLength());
        }
        if (veryVerbose) cout << "\t\tInvalid attributes." << endl;
        {
            const bael_Transmission::Cause C = CAUSE[0];
            for (int tlen = 0; tlen < NUM_LENGTH; ++tlen) {
                const int L = LENGTH[tlen];
                for (int tind = 0; tind < NUM_INDEX; ++tind) {
                    const int I = INDEX[tind];
                    // Lone valid combination.
                    if (0 == tlen && 0 == tind) continue;
                    if (veryVeryVerbose) { T_(); T_(); P_(C); P_(I); P(L); }
                    ASSERT(0 == Obj::isValid(C, I, L));

                    Obj mX;  const Obj& X = mX;
                    mX.setAttributesRaw(CAUSE[1], INDEX[1], LENGTH[1]);
                    ASSERT( CAUSE[1] == X.transmissionCause());
                    ASSERT( INDEX[1] == X.recordIndex());
                    ASSERT(LENGTH[1] == X.sequenceLength());
                    ASSERT(0 != mX.setAttributes(C, I, L));
                    ASSERT( CAUSE[1] == X.transmissionCause());
                    ASSERT( INDEX[1] == X.recordIndex());
                    ASSERT(LENGTH[1] == X.sequenceLength());
                }
            }
        }

        if (veryVerbose)
            cout << "\n\tTRIGGER[_ALL] & MANUAL_PUBLISH[_ALL]." << endl;
        if (veryVerbose) cout << "\t\tValid attributes." << endl;
        {
            for (int tc = 1; tc < NUM_CAUSE; ++tc) {  // skip PASSTHROUGH
                const bael_Transmission::Cause C = CAUSE[tc];
                for (int tlen = 0; tlen < NUM_LENGTH; ++tlen) {
                    const int L = LENGTH[tlen];
                    for (int tind = 0; tind <= tlen; ++tind) {
                        const int I = INDEX[tind];
                        if (veryVeryVerbose) {
                            T_(); T_(); P_(C); P_(I); P(L);
                        }
                        ASSERT(1 == Obj::isValid(C, I, L));

                        Obj mX(C, I, L);  const Obj& X = mX;
                        if (veryVeryVerbose) { T_(); T_(); P(X); }
                        ASSERT(C == X.transmissionCause());
                        ASSERT(I == X.recordIndex());
                        ASSERT(L == X.sequenceLength());

                        Obj mY;  const Obj& Y = mY;
                        mY.setAttributesRaw(CAUSE[0], INDEX[0], LENGTH[0]);
                        ASSERT( CAUSE[0] == Y.transmissionCause());
                        ASSERT( INDEX[0] == Y.recordIndex());
                        ASSERT(LENGTH[0] == Y.sequenceLength());
                        ASSERT(0 == mY.setAttributes(C, I, L));
                        if (veryVeryVerbose) { T_(); T_(); P(Y); }
                        ASSERT(C == Y.transmissionCause());
                        ASSERT(I == Y.recordIndex());
                        ASSERT(L == Y.sequenceLength());
                    }
                }
            }
        }
        if (veryVerbose) cout << "\t\tInvalid attributes." << endl;
        {
            for (int tc = 1; tc < NUM_CAUSE; ++tc) {  // skip PASSTHROUGH
                const bael_Transmission::Cause C = CAUSE[tc];
                for (int tlen = 0; tlen < NUM_LENGTH; ++tlen) {
                    const int L = LENGTH[tlen];
                    for (int tind = tlen + 1; tind < NUM_INDEX; ++tind) {
                        const int I = INDEX[tind];
                        if (veryVeryVerbose) {
                            T_(); T_(); P_(C); P_(I); P(L);
                        }
                        ASSERT(0 == Obj::isValid(C, I, L));

                        Obj mY;  const Obj& Y = mY;
                        mY.setAttributesRaw(CAUSE[0], INDEX[0], LENGTH[0]);
                        ASSERT( CAUSE[0] == Y.transmissionCause());
                        ASSERT( INDEX[0] == Y.recordIndex());
                        ASSERT(LENGTH[0] == Y.sequenceLength());
                        ASSERT(0 != mY.setAttributes(C, I, L));
                        ASSERT( CAUSE[0] == Y.transmissionCause());
                        ASSERT( INDEX[0] == Y.recordIndex());
                        ASSERT(LENGTH[0] == Y.sequenceLength());
                    }
                }
            }
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING 'bdex' STREAMING FUNCTIONALITY:
        //
        //   The 'bdex' streaming concerns for this component are absolutely
        //   standard.  We first probe the member functions 'outStream' and
        //   'inStream' in the manner of a "breathing test" to verify basic
        //   functionality, then we thoroughly test that functionality using
        //   the available bdex stream operators, which forward
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
        //   static int maxSupportedBdexVersion() const;
        //   bdex_InStream& streamIn(bdex_InStream&);
        //   bdex_InStream& streamIn(bdex_InStream&, int version);
        //   bdex_OutStream& bdexStreamOut(bdex_OutStream&, int version) const;
        //
        //   Note: '^' indicates a private method which is tested indirectly.
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Streaming Functionality"
                          << "\n===============================" << endl;

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        // object values for various stream tests

        Obj va, vb, vc, vd, ve, vf;
        ASSERT(0 == va.setAttributes(CAUSE[0], INDEX[0], LENGTH[0]));
        ASSERT(0 == vb.setAttributes(CAUSE[1], INDEX[1], LENGTH[1]));
        ASSERT(0 == vc.setAttributes(CAUSE[2], INDEX[2], LENGTH[2]));
        ASSERT(0 == vd.setAttributes(CAUSE[1], INDEX[3], LENGTH[3]));
        ASSERT(0 == ve.setAttributes(CAUSE[1], INDEX[4], LENGTH[4]));
        ASSERT(0 == vf.setAttributes(CAUSE[2], INDEX[5], LENGTH[5]));

        const Obj VA(va);
        const Obj VB(vb);
        const Obj VC(vc);
        const Obj VD(vd);
        const Obj VE(ve);
        const Obj VF(vf);

        const Obj VALUES[] = {VA, VB, VC, VD, VE, VF};
        const int NUM_VALUES = 6;
        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        const int VERSION = Obj::maxSupportedBdexVersion();
        if (veryVerbose) cout <<
            "\nTesting 'streamOut' and (valid) 'streamIn' functionality."
                              << endl;

        if (veryVerbose) cout << "\nDirect initial trial of 'streamOut' and"
                             " (valid) 'streamIn' functionality." << endl;
        {
            const Obj X(VC);
            Out out;
            bdex_OutStreamFunctions::streamOut(out, X, VERSION);

            const char *const OD  = out.data();
            const int         LOD = out.length();
            In in(OD, LOD);
            in.setSuppressVersionCheck(1);
            ASSERT(in);                                 ASSERT(!in.isEmpty());

            Obj t(VA);  const Obj& T = t;               ASSERT(X != T);
            bdex_InStreamFunctions::streamIn(in, t, VERSION);
            ASSERT(X == T);
            ASSERT(in);                                 ASSERT(in.isEmpty());
        }

        if (veryVerbose) cout <<
            "\nTesting 'streamOut' and (valid) 'streamIn' functionality."
                              << endl;

        if (veryVerbose) cout << "\nDirect initial trial of 'streamOut' and"
                             " (valid) 'streamIn' functionality." << endl;
        {
            const Obj X(VC);
            Out out;
            bdex_OutStreamFunctions::streamOut(out, X, VERSION);

            const char *const OD  = out.data();
            const int         LOD = out.length();
            In in(OD, LOD);
            in.setSuppressVersionCheck(1);
            ASSERT(in);                                 ASSERT(!in.isEmpty());

            Obj t(VA);  const Obj& T = t;               ASSERT(X != T);
            bdex_InStreamFunctions::streamIn(in, t, VERSION);
            ASSERT(X == T);
            ASSERT(in);                                 ASSERT(in.isEmpty());
        }
        if (veryVerbose) cout <<
            "\nThorough test of stream operators ('<<' and '>>')." << endl;
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
                    BEGIN_BDEX_EXCEPTION_TEST {
                      in.reset();
                      LOOP2_ASSERT(i, j, X == t == (i == j));
                      bdex_InStreamFunctions::streamIn(in, t, VERSION);
                    } END_BDEX_EXCEPTION_TEST
                    LOOP2_ASSERT(i, j, X == t);
                    LOOP2_ASSERT(i, j, in);  LOOP2_ASSERT(i, j, in.isEmpty());
                }
            }
        }

        if (veryVerbose) cout <<
            "\nTesting streamIn functionality via operator ('>>')." << endl;

        if (veryVerbose) cout << "\tOn empty and invalid streams." << endl;
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
              BEGIN_BDEX_EXCEPTION_TEST {
                in.reset();
                bdex_InStreamFunctions::streamIn(in, t, VERSION);
                LOOP_ASSERT(i, !in);     LOOP_ASSERT(i, X == t);
                bdex_InStreamFunctions::streamIn(in, t, VERSION);
                LOOP_ASSERT(i, !in);     LOOP_ASSERT(i, X == t);
              } END_BDEX_EXCEPTION_TEST
            }
        }

        if (veryVerbose) cout <<
            "\tOn incomplete (but otherwise valid) data." << endl;
        {
            const Obj W1 = VA, X1 = VB, Y1 = VC;
            const Obj W2 = VB, X2 = VC, Y2 = VD;
            const Obj W3 = VC, X3 = VD, Y3 = VE;

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
              BEGIN_BDEX_EXCEPTION_TEST {
                in.reset();
                LOOP_ASSERT(i, in); LOOP_ASSERT(i, !i == in.isEmpty());
                Obj t1(W1), t2(W2), t3(W3);

                if (i < LOD1) {
                    bdex_InStreamFunctions::streamIn(in, t1, VERSION);
                    LOOP_ASSERT(i, !in);
                                         if (0 == i) LOOP_ASSERT(i, W1 == t1);
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
                                      if (LOD1 == i) LOOP_ASSERT(i, W2 == t2);
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
        if (veryVerbose) cout <<
            "\tOn incomplete (but otherwise valid) data." << endl;
        {
            const Obj W1 = VA, X1 = VB, Y1 = VC;
            const Obj W2 = VB, X2 = VC, Y2 = VD;
            const Obj W3 = VC, X3 = VD, Y3 = VE;

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
              BEGIN_BDEX_EXCEPTION_TEST {
                in.reset();
                LOOP_ASSERT(i, in); LOOP_ASSERT(i, !i == in.isEmpty());
                Obj t1(W1), t2(W2), t3(W3);

                if (i < LOD1) {
                    bdex_InStreamFunctions::streamIn(in, t1, VERSION);
                    LOOP_ASSERT(i, !in);
                                         if (0 == i) LOOP_ASSERT(i, W1 == t1);
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
                                      if (LOD1 == i) LOOP_ASSERT(i, W2 == t2);
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

        if (veryVerbose) cout << "\tOn corrupted data." << endl;

        const Obj W;               // default value (0)
        const Obj X(VA);           // control value (VA)
        const Obj Y(VB);           // new value (VB)

        if (veryVerbose) cout << "\t\tGood stream (for control)." << endl;
        {
            Out out;

            bael_Transmission::bdexStreamOut(out, CAUSE[1], 1);
            out.putInt32(INDEX[1]);
            out.putInt32(LENGTH[1]);

            const char *const OD  = out.data();
            const int         LOD = out.length();

            Obj t(X);        ASSERT(W == t);  ASSERT(X == t);  ASSERT(Y != t);
            In in(OD, LOD);  ASSERT(in);
            in.setSuppressVersionCheck(1);
            bdex_InStreamFunctions::streamIn(in, t, VERSION);
            ASSERT(in);
                             ASSERT(W != t);  ASSERT(X != t);  ASSERT(Y == t);
        }

        if (veryVerbose) cout << "\t\tBad version." << endl;
        if (veryVerbose) cout << "\t\t\tVersion too small." <<endl;
        {
            const char version = 0; // too small ('version' must be >= 1)

            Out out;

            bael_Transmission::bdexStreamOut(out, CAUSE[2], 1);
            out.putInt32(INDEX[2]);
            out.putInt32(LENGTH[2]);

            const char *const OD  = out.data();
            const int         LOD = out.length();

            Obj t(X);        ASSERT(W == t);  ASSERT(X == t);  ASSERT(Y != t);
            In in(OD, LOD);  ASSERT(in);
            in.setSuppressVersionCheck(1);
            in.setQuiet(!veryVerbose);
            bdex_InStreamFunctions::streamIn(in, t, version);
            ASSERT(!in);
                             ASSERT(W == t);  ASSERT(X == t);  ASSERT(Y != t);
        }

        if (veryVerbose) cout << "\t\t\tVersion too big." << endl;
        {
            const char version = 5; // too large (current versions all = 1)

            Out out;

            bdex_OutStreamFunctions::streamOut(out, CAUSE[2], 1);
            out.putInt32(INDEX[2]);
            out.putInt32(LENGTH[2]);

            const char *const OD  = out.data();
            const int         LOD = out.length();

            Obj t(X);        ASSERT(W == t);  ASSERT(X == t);  ASSERT(Y != t);
            In in(OD, LOD);  ASSERT(in);
            in.setSuppressVersionCheck(1);
            in.setQuiet(!veryVerbose);
            bdex_InStreamFunctions::streamIn(in, t, version);
            ASSERT(!in);
                             ASSERT(W == t);  ASSERT(X == t);  ASSERT(Y != t);
        }

        if (veryVerbose) cout << "\t\tBad value." << endl;
        if (veryVerbose) cout << "\t\t\tindex neg." <<endl;
        {
            Out out;

            bael_Transmission::bdexStreamOut(out, CAUSE[1], 1);
            out.putInt32(-INDEX[1]);
            out.putInt32(LENGTH[1]);

            const char *const OD  = out.data();
            const int         LOD = out.length();

            Obj t(X);        ASSERT(W == t);  ASSERT(X == t);  ASSERT(Y != t);
            In in(OD, LOD);  ASSERT(in);
            in.setSuppressVersionCheck(1);
            in.setQuiet(!veryVerbose);
            bdex_InStreamFunctions::streamIn(in, t, VERSION);
            ASSERT(!in);
                             ASSERT(W == t);  ASSERT(X == t);  ASSERT(Y != t);
        }
        if (veryVerbose) cout << "\t\t\tsequenceLength neg." <<endl;
        {
            Out out;

            bael_Transmission::bdexStreamOut(out, CAUSE[1], 1);
            out.putInt32(INDEX[1]);
            out.putInt32(-LENGTH[1]);

            const char *const OD  = out.data();
            const int         LOD = out.length();

            Obj t(X);        ASSERT(W == t);  ASSERT(X == t);  ASSERT(Y != t);
            In in(OD, LOD);  ASSERT(in);
            in.setSuppressVersionCheck(1);
            in.setQuiet(!veryVerbose);
            bdex_InStreamFunctions::streamIn(in, t, VERSION);
            ASSERT(!in);
                             ASSERT(W == t);  ASSERT(X == t);  ASSERT(Y != t);
        }
        if (veryVerbose) cout << "\t\t\tsequenceLength 0." <<endl;
        {
            Out out;

            bael_Transmission::bdexStreamOut(out, CAUSE[1], 1);
            out.putInt32(INDEX[1]);
            out.putInt32(0);

            const char *const OD  = out.data();
            const int         LOD = out.length();

            Obj t(X);        ASSERT(W == t);  ASSERT(X == t);  ASSERT(Y != t);
            In in(OD, LOD);  ASSERT(in);
            in.setSuppressVersionCheck(1);
            in.setQuiet(!veryVerbose);
            bdex_InStreamFunctions::streamIn(in, t, VERSION);
            ASSERT(!in);
                             ASSERT(W == t);  ASSERT(X == t);  ASSERT(Y != t);
        }
        if (veryVerbose) cout << "\nTesting 'maxSupportedBdexVersion()'."
                              << endl;
        {
            const Obj X;
            ASSERT(1 == X.maxSupportedBdexVersion());
        }

        if (veryVerbose) cout << "\nWire format direct tests." << endl;
        {
            static const struct {
                int         d_lineNum;    // source line number
                int         d_cause;      // cause of publish
                int         d_index;      // record index
                int         d_seqLength;  // total record number in seq.
                int         d_version;    // version to stream with
                int         d_length;     // expect output length
                const char *d_fmt_p;      // expected output format
            } DATA[] = {
                //line  cau  indx  seq  ver  len  format
                //----  ---  ----  ---  ---  ---  --------------------------
                { L_,    0,   10,   0,   0,   0,  ""                         },
                { L_,    1,    8,   0,   0,   0,  ""                         },

                { L_,    2,    0,   1,   1,   9,
                                      "\x02\x00\x00\x00\x00\x00\x00\x00\x01" },
                { L_,    1,    8,   9,   1,   9,
                                      "\x01\x00\x00\x00\x08\x00\x00\x00\x09" },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int         LINE    = DATA[i].d_lineNum;
                const int         CAUSE   = DATA[i].d_cause;
                const int         INDEX   = DATA[i].d_index;
                const int         SEQLEN  = DATA[i].d_seqLength;
                const int         VERSION = DATA[i].d_version;
                const int         LEN     = DATA[i].d_length;
                const char *const FMT     = DATA[i].d_fmt_p;

                Obj mX((bael_Transmission::Cause)CAUSE, INDEX, SEQLEN);
                const Obj& X = mX;
                bdex_ByteOutStream out;  X.bdexStreamOut(out, VERSION);

                LOOP_ASSERT(LINE, LEN == out.length());
                LOOP_ASSERT(LINE, 0 == memcmp(out.data(),
                                              FMT,
                                              LEN));

                if (veryVeryVerbose && memcmp(out.data(),
                                              FMT,
                                              LEN)) {
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
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // Basic Attribute Test:
        //
        // Testing:
        //   bael_Context(bslma_Allocator *ba = 0);
        //   bael_Context(const bael_Context& original, *ba = 0);
        //   ~bael_Context();
        //   bael_Context& operator=(const bael_Context& rhs);
        //   void setAttributesRaw(cause, index, length);
        //   void setRecordIndexRaw(int index);
        //   bael_Transmission::Cause transmissionCause() const;
        //   int recordIndex() const;
        //   int sequenceLength() const;
        //   ostream& print(ostream& os, int level = 0, int spl = 4) const;
        //   bool operator==(const bael_Context& lhs, const bael_Context& rhs);
        //   bool operator!=(const bael_Context& lhs, const bael_Context& rhs);
        //   ostream& operator<<(ostream& os, const bael_Context&);
        //
        //   Note: '^' indicates a private method which is tested indirectly.
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "Basic Attribute Test" << endl
                                  << "====================" << endl;

        Obj mX1, mY1; const Obj& X1 = mX1; const Obj& Y1 = mY1;
        Obj mZ1; const Obj& Z1 = mZ1; // Z1 is the control
        if (veryVeryVerbose) { P(X1); }

        if (veryVerbose) cout << "\n Check default ctor. " << endl;

        ASSERT( CAUSE[0] == X1.transmissionCause());
        ASSERT( INDEX[0] == X1.recordIndex());
        ASSERT(LENGTH[0] == X1.sequenceLength());
        ASSERT(1 == (X1 == X1));          ASSERT(0 == (X1 != X1));
        ASSERT(1 == (X1 == Z1));          ASSERT(0 == (X1 != Z1));
        ASSERT(1 == (Z1 == Y1));          ASSERT(0 == (Z1 != Y1));
        ASSERT(1 == (Y1 == Z1));          ASSERT(0 == (Y1 != Z1));

        if (veryVerbose)
            cout << "\t    Setting default values explicitly." <<endl;

        mX1.setAttributesRaw(CAUSE[0], INDEX[0], LENGTH[0]);
        ASSERT(1 == (X1 == X1));          ASSERT(0 == (X1 != X1));
        ASSERT(1 == (X1 == Z1));          ASSERT(0 == (X1 != Z1));
        ASSERT(1 == (Z1 == Y1));          ASSERT(0 == (Z1 != Y1));
        ASSERT(1 == (Y1 == Z1));          ASSERT(0 == (Y1 != Z1));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (veryVerbose) cout << "\tTesting set/get methods."
                              << "\n\t  Change attribute 0." << endl;

        mX1.setAttributesRaw(CAUSE[1], INDEX[0], LENGTH[0]);
        ASSERT( CAUSE[1] == X1.transmissionCause());
        ASSERT( INDEX[0] == X1.recordIndex());
        ASSERT(LENGTH[0] == X1.sequenceLength());
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

        mX1.setAttributesRaw(CAUSE[0], INDEX[0], LENGTH[0]);
        ASSERT(1 == (X1 == X1));          ASSERT(0 == (X1 != X1));
        ASSERT(1 == (X1 == Z1));          ASSERT(0 == (X1 != Z1));
        ASSERT(0 == (Y1 == Z1));          ASSERT(1 == (Y1 != Z1));

        mX1 = mY1 = Z1;
        ASSERT(1 == (X1 == X1));          ASSERT(0 == (X1 != X1));
        ASSERT(1 == (X1 == Z1));          ASSERT(0 == (X1 != Z1));
        ASSERT(1 == (Y1 == Z1));          ASSERT(0 == (Y1 != Z1));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (veryVerbose) cout << "\t Change attribute 1." << endl;

        mX1.setRecordIndexRaw(INDEX[1]);
        ASSERT( CAUSE[0] == X1.transmissionCause());
        ASSERT( INDEX[1] == X1.recordIndex());
        ASSERT(LENGTH[0] == X1.sequenceLength());
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

        mX1.setRecordIndexRaw(INDEX[0]);
        ASSERT( CAUSE[0] == X1.transmissionCause());
        ASSERT( INDEX[0] == X1.recordIndex());
        ASSERT(LENGTH[0] == X1.sequenceLength());
        ASSERT(1 == (X1 == X1));          ASSERT(0 == (X1 != X1));
        ASSERT(1 == (X1 == Z1));          ASSERT(0 == (X1 != Z1));
        ASSERT(0 == (Y1 == Z1));          ASSERT(1 == (Y1 != Z1));

        mX1 = mY1 = Z1;
        ASSERT(1 == (X1 == X1));          ASSERT(0 == (X1 != X1));
        ASSERT(1 == (X1 == Z1));          ASSERT(0 == (X1 != Z1));
        ASSERT(1 == (Y1 == Z1));          ASSERT(0 == (Y1 != Z1));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (veryVerbose) cout << "\t Change attribute 2." << endl;

        mX1.setAttributesRaw(CAUSE[1], INDEX[1], LENGTH[1]);
        ASSERT( CAUSE[1] == X1.transmissionCause());
        ASSERT( INDEX[1] == X1.recordIndex());
        ASSERT(LENGTH[1] == X1.sequenceLength());
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

        mX1.setAttributesRaw(CAUSE[0], INDEX[0], LENGTH[0]);
        ASSERT( CAUSE[0] == X1.transmissionCause());
        ASSERT( INDEX[0] == X1.recordIndex());
        ASSERT(LENGTH[0] == X1.sequenceLength());
        ASSERT(1 == (X1 == X1));          ASSERT(0 == (X1 != X1));
        ASSERT(1 == (X1 == Z1));          ASSERT(0 == (X1 != Z1));
        ASSERT(0 == (Y1 == Z1));          ASSERT(1 == (Y1 != Z1));

        mX1 = mY1 = Z1;
        ASSERT(1 == (X1 == X1));          ASSERT(0 == (X1 != X1));
        ASSERT(1 == (X1 == Z1));          ASSERT(0 == (X1 != Z1));
        ASSERT(1 == (Y1 == Z1));          ASSERT(0 == (Y1 != Z1));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (veryVerbose) cout << "Testing output operator (<<)." << endl;

        mY1.setAttributesRaw(CAUSE[1], INDEX[2], LENGTH[2]);
        ASSERT(mX1 != mY1);

        char buf[1024];
        {
            ostrstream o(buf, sizeof buf);
            o << X1 << ends;
            if (veryVeryVerbose) cout << "X1 buf:\n" << buf << endl;
            bsl::string s = "[ PASSTHROUGH 0 1 ]";
            if (veryVeryVerbose) cout << "Expected:\n" << s << endl;
            ASSERT(buf == s);
        }
        {
            ostrstream o(buf, sizeof buf);
            o << Y1 << ends;
            if (veryVeryVerbose) cout << "Y1 buf:\n" << buf << endl;
            bsl::string s = "[ TRIGGER 8 9 ]";
            if (veryVeryVerbose) cout << "Expected:\n" << s << endl;
            ASSERT(buf == s);
        }

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (veryVerbose) cout << "Testing 'print'." << endl;

        {
#define NL "\n"
            static const struct {
                int         d_lineNum;  // source line number
                int         d_indent;   // indentation level
                int         d_spaces;   // spaces per indentation level
                const char *d_fmt_p;    // expected output format
            } DATA[] = {
                //line  indent +/-  spaces/Tab  format                // ADJUST
                //----  ----------  ----------  --------------
                { L_,    0,         -1,         "[ PASSTHROUGH 0 1 ]"        },

                { L_,    0,          0,         "["                 NL
                                                "PASSTHROUGH"       NL
                                                "0"                 NL
                                                "1"                 NL
                                                "]"                 NL       },

                { L_,    0,          2,         "["                 NL
                                                "  PASSTHROUGH"     NL
                                                "  0"               NL
                                                "  1"               NL
                                                "]"                 NL       },

                { L_,    1,          1,         " ["                NL
                                                "  PASSTHROUGH"     NL
                                                "  0"               NL
                                                "  1"               NL
                                                " ]"                NL       },

                { L_,    1,          2,         "  ["               NL
                                                "    PASSTHROUGH"   NL
                                                "    0"             NL
                                                "    1"             NL
                                                "  ]"               NL       },

                { L_,   -1,          2,         "["                 NL
                                                "    PASSTHROUGH"   NL
                                                "    0"             NL
                                                "    1"             NL
                                                "  ]"               NL       },

                { L_,   -2,          1,         "["                 NL
                                                "   PASSTHROUGH"    NL
                                                "   0"              NL
                                                "   1"              NL
                                                "  ]"               NL       },

                { L_,    2,          1,         "  ["               NL
                                                "   PASSTHROUGH"    NL
                                                "   0"              NL
                                                "   1"              NL
                                                "  ]"               NL       },

                { L_,    1,          3,         "   ["              NL
                                                "      PASSTHROUGH" NL
                                                "      0"           NL
                                                "      1"           NL
                                                "   ]"              NL       },
            };
#undef NL
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            const int SIZE = 128;  // Must be big enough to hold output string.
            const char Z1  = (char) 0xFF;  // Value 1 used for an unset char.
            const char Z2  = 0x00; // Value 2 used to represent an unset char.

            char mCtrlBuf1[SIZE];  memset(mCtrlBuf1, Z1, SIZE);
            char mCtrlBuf2[SIZE];  memset(mCtrlBuf2, Z2, SIZE);
            const char *CTRL_BUF1 = mCtrlBuf1;
            const char *CTRL_BUF2 = mCtrlBuf2;

            Obj mX;  const Obj& X = mX;  // [ PASSTHROUGH 0 1 ]
            mX.setAttributesRaw(CAUSE[0], INDEX[0], LENGTH[0]);

            for (int ti = 0; ti < NUM_DATA;  ++ti) {
//                const int         LINE = DATA[ti].d_lineNum; // unused
                const int         IND  = DATA[ti].d_indent;
                const int         SPL  = DATA[ti].d_spaces;
                const char *const FMT  = DATA[ti].d_fmt_p;

                char buf1[SIZE], buf2[SIZE];
                memcpy(buf1, CTRL_BUF1, SIZE); // Preset buf1 to Z1 values.
                memcpy(buf2, CTRL_BUF2, SIZE); // Preset buf2 to Z2 values.

                if (veryVerbose) cout << "EXPECTED FORMAT:" << endl<<FMT<<endl;
                ostrstream out1(buf1, SIZE);  X.print(out1, IND, SPL) << ends;
                ostrstream out2(buf2, SIZE);  X.print(out2, IND, SPL) << ends;
                if (veryVerbose) cout << "ACTUAL FORMAT:" << endl<<buf1<<endl;

                const int SZ = strlen(FMT) + 1;
                const int REST = SIZE - SZ;
                LOOP_ASSERT(ti, SZ < SIZE);  // Check buffer is large enough.
                LOOP_ASSERT(ti, Z1 == buf1[SIZE - 1]);  // Check for overrun.
                LOOP_ASSERT(ti, Z2 == buf2[SIZE - 1]);  // Check for overrun.
                LOOP_ASSERT(ti, 0 == strcmp(buf1, FMT));
                LOOP_ASSERT(ti, 0 == strcmp(buf2, FMT));
                LOOP_ASSERT(ti, 0 == memcmp(buf1 + SZ, CTRL_BUF1 + SZ, REST));
                LOOP_ASSERT(ti, 0 == memcmp(buf2 + SZ, CTRL_BUF2 + SZ, REST));
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

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2004
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
