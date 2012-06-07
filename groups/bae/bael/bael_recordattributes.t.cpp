// bael_recordattributes.t.cpp         -*-C++-*-

#include <bael_recordattributes.h>

#include <bdema_bufferedsequentialallocator.h>  // for testing only
#include <bdetu_datetime.h>                     // for testing only
#include <bdex_instreamfunctions.h>             // for testing only
#include <bdex_outstreamfunctions.h>            // for testing only
#include <bdex_testinstream.h>                  // for testing only
#include <bdex_testinstreamexception.h>         // for testing only
#include <bdex_testoutstream.h>                 // for testing only

#include <bslma_testallocator.h>                // for testing only
#include <bslma_testallocatorexception.h>       // for testing only
#include <bsls_platform.h>                      // for testing only
#include <bsls_platformutil.h>                  // for testing only

#include <bsl_cstdlib.h>      // atoi()
#include <bsl_cstring.h>      // strlen(), memset(), memcpy(), memcmp()
#ifdef BSLS_PLATFORM__OS_UNIX
#include <unistd.h>     // getpid()
#endif

#include <bsl_new.h>          // placement 'new' syntax
#include <bsl_iostream.h>
#include <bsl_strstream.h>

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// This test plan follows the standard approach for Attribute Classes.
// There are three main tests:
//
// 1. *Basic Value Semantics*
//    This tests all constructors and basic manipulators (setters and getters)
//    to make sure they work as expected.  No constraints or object or class
//    invariants are known, so none are tested.
//
// 2. *Streaming*
//    This tests streamIn/streamOut functionality to ensure that
//    'bael_RecordAttributes' objects behave correctly in normal streaming
//    and in presence of stream exceptions.
//
//    When a streaming exception occurs, we guarantee internal consistency,
//    but not specific value.  The test suite verifies object consistency
//    simply by allowing the object destructor to work correctly.
//
// 3. *Usage Test*
//    This illustrates simple usage examples, taken from the component
//    header file.
//-----------------------------------------------------------------------------
// 'bael_RecordAttributes' public interface:
// [ 3] static int maxSupportedBdexVersion();
// [ 2] bael_RecordAttributes(bslma_Allocator *ba = 0);
// [ 4] bael_RecordAttributes(ts, pid, tid, fn, ln, cat, sev, msg, *ba=0);
// [ 2] bael_RecordAttributes(const bael_RecordAttributes& orig, *ba = 0);
// [ 2] ~bael_RecordAttributes();
// [ 2] bael_RecordAttributes& operator=(const bael_RecordAttributes&);
// [ 2] void setCategory(const char *category);
// [ 2] void setFileName(const char *fileName);
// [ 2] void setLineNumber(int lineNumber);
// [ 2] void setMessage(const char *message);
// [ 2] void setProcessID(int processID);
// [ 2] void setSeverity(int severity);
// [ 2] void setThreadID(bsls_PlatformUtil::Uint64 threadID);
// [ 2] void setTimestamp(const bdet_Datetime& timestamp);
// [ 3] STREAM& bdexStreamIn(STREAM& is, int version);
// [ 2] const char *category() const;
// [ 2] const char *fileName() const;
// [ 2] int lineNumber() const;
// [ 2] const char *message() const;
// [ 2] int processID() const;
// [ 2] int severity() const;
// [ 2] bsls_PlatformUtil::Uint64 threadID() const;
// [ 2] const bdet_Datetime& timestamp() const;
// [ 2] void clearMessage();
// [ 2] bdesb_MemOutStreamBuf& messageStreamBuf();
// [ 3] ostream& print(ostream& os, int level = 0, int spl = 4) const;
// [ 3] STREAM& bdexStreamOut(STREAM& os, int version) const;
//
// [ 2] bool operator==(const Obj& lhs, const Obj& rhs);
// [ 2] bool operator!=(const Obj& lhs, const Obj& rhs);
// [ 2] ostream& operator<<(ostream& os, const bael_RecordAttributes&);
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 5] USAGE EXAMPLE 1
// [ 6] USAGE EXAMPLE 2

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

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

typedef bael_RecordAttributes Obj;

typedef bdex_TestInStream     In;
typedef bdex_TestOutStream    Out;

struct my_RecordAttributes {
    bdet_Datetime              timestamp;
    int                        processID;
    bsls_PlatformUtil::Uint64  threadID;
    const char                *fileName;
    int                        lineNumber;
    const char                *category;
    int                        severity;
    const char                *message;
};

//=============================================================================
//                             USAGE EXAMPLE 2
//-----------------------------------------------------------------------------
class Information
{
  private:
    bsl::string d_heading;
    bsl::string d_contents;

  public:
    Information(const char *heading, const char *contents);
    const bsl::string& heading() const;
    const bsl::string& contents() const;
};

Information::Information(const char *heading, const char *contents)
: d_heading(heading)
, d_contents(contents)
{
}

const bsl::string& Information::heading() const
{
    return d_heading;
}

const bsl::string& Information::contents() const
{
    return d_contents;
}

bsl::ostream& operator<<(bsl::ostream& stream, const Information& information)
{
    stream << information.heading() << endl;
    stream << '\t';
    stream << information.contents() << endl;
    return stream;
}

void streamInformationIntoMessageAttribute(bael_RecordAttributes& attributes,
                                           const Information& information)
{
    attributes.clearMessage();
    bsl::ostream os(&attributes.messageStreamBuf());
    os << information;
}
//=============================================================================
//                             USAGE EXAMPLE 1
//-----------------------------------------------------------------------------

void printMessage(ostream& stream, const bael_RecordAttributes& attributes)
{
using namespace bsl;  // automatically added by script

    stream << "\tTimestamp: " << attributes.timestamp() << endl;
    stream << "\tCategory:  " << attributes.category()  << endl;
    stream << "\tMessage:   " << attributes.message()   << endl;
    stream << endl;
}

//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

void initRecordAttributes(bael_RecordAttributes&     lhs,
                          const my_RecordAttributes& rhs)
{
    lhs.setTimestamp(rhs.timestamp);
    lhs.setProcessID(rhs.processID);
    lhs.setThreadID(rhs.threadID);
    lhs.setFileName(rhs.fileName);
    lhs.setLineNumber(rhs.lineNumber);
    lhs.setCategory(rhs.category);
    lhs.setSeverity(rhs.severity);
    lhs.setMessage(rhs.message);

    ASSERT(0 == strcmp(lhs.category(), rhs.category));
    ASSERT(0 == strcmp(lhs.fileName(), rhs.fileName));
    ASSERT(0 == strcmp(lhs.message(),  rhs.message));
    ASSERT(lhs.lineNumber() == rhs.lineNumber);
    ASSERT(lhs.processID()  == rhs.processID);
    ASSERT(lhs.severity()   == rhs.severity);
    ASSERT(lhs.threadID()   == rhs.threadID);
    ASSERT(lhs.timestamp()  == rhs.timestamp);
}

#define EXPLICIT_CONSTRUCTOR(OBJ, ORA, ALLOC)                       \
    Obj OBJ(ORA.timestamp,                                          \
            ORA.processID,                                          \
            ORA.threadID,                                           \
            ORA.fileName,                                           \
            ORA.lineNumber,                                         \
            ORA.category,                                           \
            ORA.severity,                                           \
            ORA.message,                                            \
            ALLOC);                                                 \
    ASSERT(0 == strcmp(OBJ.category(), ORA.category));              \
    ASSERT(0 == strcmp(OBJ.fileName(), ORA.fileName));              \
    ASSERT(0 == strcmp(OBJ.message(),  ORA.message));        \
    ASSERT(OBJ.lineNumber() == ORA.lineNumber);                     \
    ASSERT(OBJ.processID()  == ORA.processID);                      \
    ASSERT(OBJ.severity()   == ORA.severity);                       \
    ASSERT(OBJ.threadID()   == ORA.threadID);                       \
    ASSERT(OBJ.timestamp()  == ORA.timestamp);

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;
    int veryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    bslma_TestAllocator testAllocator(veryVeryVerbose);

    switch (test) { case 0:  // Zero is always the leading case.
      case 6: {
        // --------------------------------------------------------------------
        // TESTING USAGE EXAMPLE 2
        //
        // Concerns:
        //   The usage example-2 provided in the component header file must
        //   compile, link, and run on all platforms as shown.
        //
        // Plan:
        //   Incorporate usage example-2 from header into driver, remove
        //   leading comment characters, and replace 'assert' with 'ASSERT'.
        //
        // Testing:
        //   USAGE EXAMPLE 2
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "Testing Usage Example-2" << endl
                                  << "=======================" << endl;

        Information info("MY-HEADING", "MY-CONTENTS");
        bael_RecordAttributes attr;
        streamInformationIntoMessageAttribute(attr, info);
        if (verbose) {
            cout << attr.message();
        }

      } break;

      case 5: {
        // --------------------------------------------------------------------
        // TESTING USAGE EXAMPLE 1
        //
        // Concerns:
        //   The usage example-1 provided in the component header file must
        //   compile, link, and run on all platforms as shown.
        //
        // Plan:
        //   Incorporate usage example-1 from header into driver, remove
        //   leading comment characters, and replace 'assert' with 'ASSERT'.
        //
        // Testing:
        //   USAGE EXAMPLE 1
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "Testing Usage Example-1" << endl
                                  << "=======================" << endl;

        {
            char buf[2048];
            ostrstream out(buf, sizeof(buf));

            enum Severity { INFO, WARN, BUY, SELL };
            const char *Category[] = { "Bonds", "Equities", "Futures" };

            bael_RecordAttributes attributes;
            bdet_Datetime now;
            bdetu_Datetime::convertFromTimeT(&now, time(0));
            attributes.setTimestamp(now);
            #ifdef BSLS_PLATFORM__OS_UNIX
            attributes.setProcessID(getpid());
            #endif
            attributes.setThreadID(-1);        // pthread_self()
            attributes.setFileName(__FILE__);
            attributes.setLineNumber(__LINE__);
            attributes.setCategory(Category[2]);
            attributes.setSeverity(WARN);
            attributes.setMessage(
                                 "sugar up (locust infestations on the rise)");

            if (veryVerbose) { cout << attributes << endl; }

            printMessage(out, attributes);
            if (veryVerbose) { out << ends; cout << buf << endl; }
        }
      } break;

      case 4: {
        // --------------------------------------------------------------------
        // Initialization Constructor Test
        //
        // Testing:
        //   bael_RecordAttributes(ts, pid, tid, fn, ln, cat, sev, msg, *ba=0);
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "Initialization Constructor Test" << endl
                                  << "===============================" << endl;

        const char         *DEFAULT_CATEGORY   = "";
        const char         *DEFAULT_FILENAME   = "";
        const int           DEFAULT_LINENUMBER = 0;
        const char         *DEFAULT_MESSAGE    = "";
        const int           DEFAULT_PROCESSID  = 0;
        const int           DEFAULT_SEVERITY   = 0;
        const int           DEFAULT_THREADID   = 0;
        const bdet_Datetime DEFAULT_TIMESTAMP;

        const char         *OTHER_CATEGORY   = "category";
        const char         *OTHER_FILENAME   = "filename.cpp";
        const int           OTHER_LINENUMBER = L_;
        const char         *OTHER_MESSAGE    = "message";
        const int           OTHER_PROCESSID  = 13248;
        const int           OTHER_SEVERITY   = 128;
        const int           OTHER_THREADID   = 19;
        bdet_Datetime       OTHER_TIMESTAMP;
        bdetu_Datetime::convertFromTimeT(&OTHER_TIMESTAMP, time(0));  // now

        {
            Obj mX;  const Obj& X = mX;

            Obj mY(DEFAULT_TIMESTAMP,
                   DEFAULT_PROCESSID,
                   DEFAULT_THREADID,
                   DEFAULT_FILENAME,
                   DEFAULT_LINENUMBER,
                   DEFAULT_CATEGORY,
                   DEFAULT_SEVERITY,
                   DEFAULT_MESSAGE);
            const Obj& Y = mY;
            if (veryVeryVerbose) { T_(); P(X);  T_(); P(Y); }

            ASSERT(1 == (X == X));          ASSERT(0 == (X != X));
            ASSERT(1 == (X == Y));          ASSERT(0 == (X != Y));
        }

        {
            Obj mX;  const Obj& X = mX;
            mX.setCategory(OTHER_CATEGORY);
            mX.setFileName(OTHER_FILENAME);
            mX.setLineNumber(OTHER_LINENUMBER);
            mX.setMessage(OTHER_MESSAGE);
            mX.setProcessID(OTHER_PROCESSID);
            mX.setSeverity(OTHER_SEVERITY);
            mX.setThreadID(OTHER_THREADID);
            mX.setTimestamp(OTHER_TIMESTAMP);

            Obj mY(OTHER_TIMESTAMP,
                   OTHER_PROCESSID,
                   OTHER_THREADID,
                   OTHER_FILENAME,
                   OTHER_LINENUMBER,
                   OTHER_CATEGORY,
                   OTHER_SEVERITY,
                   OTHER_MESSAGE);
            const Obj& Y = mY;
            if (veryVeryVerbose) { T_(); P(X);  T_(); P(Y); }

            ASSERT(1 == (X == X));          ASSERT(0 == (X != X));
            ASSERT(1 == (X == Y));          ASSERT(0 == (X != Y));
        }

        if (veryVerbose) cout << "Test that the internal memory management "
                                 "system is hooked up properly." << endl;
        {
            char memory[2048];
            bdema_BufferedSequentialAllocator a(memory, sizeof memory);

            new(a.allocate(sizeof(Obj)))
              Obj(DEFAULT_TIMESTAMP,
                  DEFAULT_PROCESSID,
                  DEFAULT_THREADID,
                  "a_string_long_enough_to_trigger_allocation",
                  DEFAULT_LINENUMBER,
                  "a_string_long_enough_to_trigger_allocation",
                  DEFAULT_SEVERITY,
                  "a_string_long_enough_to_trigger_allocation",
                  &a);

            // No destructor is called; will produce memory leak in purify
            // if internal allocators are not hooked up properly.
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING 'bdex' STREAMING FUNCTIONALITY
        //
        // Concerns:
        //   The 'bdex' streaming concerns for this component are absolutely
        //   standard.  We first probe the member functions 'outStream' and
        //   'inStream' in the manner of a "breathing test" to verify basic
        //   functionality, then we thoroughly test that functionality using
        //   the available bdex stream functions, which forward
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
        // Testing:
        //   static int maxSupportedBdexVersion() const;
        //   STREAM& bdexStreamIn(STREAM& is, int version);
        //   STREAM& bdexStreamOut(STREAM& os, int) const;
        //
        //   Note: '^' indicates a private method which is tested indirectly.
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Streaming Functionality"
                          << "\n===============================" << endl;

        bdet_Datetime now;
        bdetu_Datetime::convertFromTimeT(&now, time(0));
        const char *CATEGORY[] = { "Bonds", "Equities", "Futures" };
        enum SEVERITY { INFO = 0, WARN = 1, BUY = 2, SELL = 3 };
        my_RecordAttributes RA[] = {
            { now,     0,     -1, __FILE__, L_, CATEGORY[0], INFO,
              "Bond market is closed."                                       },
            { now, 32767,      0, __FILE__, L_, CATEGORY[2], INFO,
              "Futures market is closed."                                    },
            { now, 32768,      0, __FILE__, L_, CATEGORY[1], WARN,
              "Current Equity not available."                                },
            { now, 54321,     -1, __FILE__, L_, CATEGORY[2], SELL,
              "Pork Belly Futures are through the roof."                     },
            { now, 12345,  32767, __FILE__, L_, CATEGORY[0], BUY,
              "Buy U.S. Savings Bonds!"                                      },
            { now, 32768, -32768, __FILE__, L_, CATEGORY[1], WARN,
              "Current Equity not listed."                                   },
        };
        const int NUM_VALUES = sizeof RA / sizeof *RA;

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        // six object values for use in tests below

        Obj va, vb, vc;
        initRecordAttributes(va, RA[0]);
        initRecordAttributes(vb, RA[1]);
        initRecordAttributes(vc, RA[2]);

        EXPLICIT_CONSTRUCTOR(vd, RA[3], &testAllocator);
        EXPLICIT_CONSTRUCTOR(ve, RA[4], &testAllocator);
        EXPLICIT_CONSTRUCTOR(vf, RA[5], &testAllocator);

        const Obj VA(va);
        const Obj VB(vb);
        const Obj VC(vc);
        const Obj VD(vd);
        const Obj VE(ve);
        const Obj VF(vf);

        const Obj VALUES[NUM_VALUES] = { VA, VB, VC, VD, VE, VF };
        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (veryVerbose) cout << "\nTesting 'maxSupportedBdexVersion' method."
                              << endl;
        {
            const Obj X;
            ASSERT(1 == X.maxSupportedBdexVersion());
        }
        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (veryVerbose) cout <<
            "\nTesting 'streamOut' and (valid) 'streamIn' functionality."
            << endl;
        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        const int VERSION = Obj::maxSupportedBdexVersion();
        if (veryVerbose) cout << "\nDirect initial trial of 'streamOut(s)' and"
                                 " 'streamIn(s)'." << endl;
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
        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (veryVerbose)
            cout << "\nDirect initial trial of 'bdexStreamOut(s, version)'"
                    " and 'streamIn(s, version)'." << endl;
        {
            const Obj X(VC);
            Out out;

            X.bdexStreamOut(out, VERSION);

            const char *const OD  = out.data();
            const int         LOD = out.length();
            In in(OD, LOD);
            ASSERT(in);                                 ASSERT(!in.isEmpty());

            Obj t(VA);  const Obj& T = t;               ASSERT(X != T);
            in.setSuppressVersionCheck(1);
                                                       // don't require version
            t.bdexStreamIn(in, VERSION);                ASSERT(X == T);
            ASSERT(in);                                 ASSERT(in.isEmpty());
        }
        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (veryVerbose) cout <<
            "\nThorough test of stream operators ('<<' and '>>')." << endl;
        {
            for (int i = 0; i < NUM_VALUES; ++i) {
                const Obj X(VALUES[i]);
                if (veryVeryVerbose) { T_(); P(X); }
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
                  Obj t;
                  BEGIN_BDEX_EXCEPTION_TEST {
                    // create locally; destructor will verify consistency
                    Obj t_obj(VALUES[j]);
                    in.reset();
                    bdex_InStreamFunctions::streamIn(in, t_obj, VERSION);
                    t = t_obj;
                    LOOP2_ASSERT(i, j, X == t);
                  } END_BDEX_EXCEPTION_TEST

                  LOOP2_ASSERT(i, j, X == t);
                  LOOP2_ASSERT(i, j, in);  LOOP2_ASSERT(i, j, in.isEmpty());
                }
            }
        }
        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (veryVerbose) cout <<
            "\nTesting streamIn functionality via operator ('>>')." << endl;

        // - - - - - -
        if (veryVerbose) cout << "\tOn empty and invalid streams." << endl;
        {
            Out out;
            const char *const  OD = out.data();
            const int         LOD = out.length();
            ASSERT(0 == LOD);

            for (int i = 0; i < NUM_VALUES; ++i) {
              In in(OD, LOD);
              in.setSuppressVersionCheck(1);
              In &testInStream = in;
              LOOP_ASSERT(i, in);
              LOOP_ASSERT(i, in.isEmpty());

              // Ensure that reading from an empty or invalid input stream
              // leaves the stream invalid and the target object unchanged.

              const Obj X(VALUES[i]);
              Obj t(X);
              LOOP_ASSERT(i, X == t);

              BEGIN_BDEX_EXCEPTION_TEST {
                in.reset();

                if (veryVeryVerbose) cout << "empty stream; " << flush;
                bdex_InStreamFunctions::streamIn(in, t, VERSION);
                LOOP_ASSERT(i, !in);
                LOOP_ASSERT(i, X == t);

                if (veryVeryVerbose) cout << "invalid stream; " << flush;
                bdex_InStreamFunctions::streamIn(in, t, VERSION);
                LOOP_ASSERT(i, !in);
                LOOP_ASSERT(i, X == t);
                if (veryVeryVerbose) cout << "done" << endl;

              } END_BDEX_EXCEPTION_TEST
            }
        }

        // ***
        // - - - - - -
        if (veryVerbose) cout << "\tOn incomplete (but otherwise valid) data."
                              << endl;
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
              In in(OD, i);
              in.setSuppressVersionCheck(1);
              In &testInStream = in;
              BEGIN_BDEX_EXCEPTION_TEST {
                in.reset();
                LOOP_ASSERT(i, in);
                LOOP_ASSERT(i, !i == in.isEmpty());
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

        // - - - - - -
        if (veryVerbose) cout << "\tOn corrupted data." << endl;

        const Obj W;               // default value (0)
        const Obj X(VA);           // control value (VA)
        const Obj Y(VB);           // new value (VB)

        // *** commented
        // - - - - - -
        if (veryVerbose) cout << "\t\tGood stream (for control)." << endl;
        {
            // white-box implementation of bael_recordattribute::streamOut
            bsl::string fn(Y.fileName());
            bsl::string cat(Y.category());
            bsl::string msg(Y.message());

            Out out;
            now.bdexStreamOut(out, VERSION);
            out.putInt32(Y.processID());
            out.putInt32(Y.threadID());
            //            fn.bdexStreamOut(out, 1);
            out.putString(fn);
            out.putInt32(Y.lineNumber());
            //            cat.bdexStreamOut(out, 1);
            out.putString(cat);
            out.putInt32(Y.severity());
            //            msg.bdexStreamOut(out, 1);

            int len = msg.length();
            const char *data = msg.c_str();

            out.putInt32(len);
            for (int i=0; i < len; ++i) {
                out.putInt8(data[i]);
            }

            const char *const OD  = out.data();
            const int         LOD = out.length();
            In in(OD, LOD);  ASSERT(in);
            in.setSuppressVersionCheck(1);
            Obj t(X);        ASSERT(W != t);  ASSERT(X == t);  ASSERT(Y != t);
            bdex_InStreamFunctions::streamIn(in, t, VERSION);
            ASSERT(in);
                             ASSERT(W != t);  ASSERT(X != t);  ASSERT(Y == t);
        }

        // - - - - - -
        if (veryVerbose) cout << "\t\tBad version." << endl;
        if (veryVerbose) cout << "\t\t\tVersion too small." << endl;

        {
            const int version = 0; // too small ('version' must be >= 1)
            bsl::string fn(Y.fileName());
            bsl::string cat(Y.category());
            bsl::string msg(Y.message());

            Out out;
            now.bdexStreamOut(out, VERSION);
            out.putInt32(Y.processID());
            out.putInt32(Y.threadID());
            //            fn.bdexStreamOut(out, 1);
            out.putString(fn);
            out.putInt32(Y.lineNumber());
            //            cat.bdexStreamOut(out, 1);
            out.putString(cat);
            out.putInt32(Y.severity());
            //            msg.bdexStreamOut(out, 1);

            int len = msg.length();
            const char *data = msg.c_str();

            out.putInt32(len);
            for (int i=0; i < len; ++i) {
                out.putInt8(data[i]);
            }

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

        if (veryVerbose) cout << "\t\t\tVersion too large (5)." << endl;
        {
            const char version = 5; // too large (currently 1)
            bsl::string fn(Y.fileName());
            bsl::string cat(Y.category());
            bsl::string msg(Y.message());

            Out out;

            now.bdexStreamOut(out, 1);
            out.putInt32(Y.processID());
            out.putInt32(Y.threadID());
            //            fn.bdexStreamOut(out, 1);
            out.putString(fn);
            out.putInt32(Y.lineNumber());
            //            cat.bdexStreamOut(out, 1);
            out.putString(cat);
            out.putInt32(Y.severity());
            //            msg.bdexStreamOut(out, 1);

            int len = msg.length();
            const char *data = msg.c_str();

            out.putInt32(len);
            for (int i=0; i < len; ++i) {
                out.putInt8(data[i]);
            }

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
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // Basic Attribute Test
        //
        // Testing:
        //   bael_RecordAttributes(bslma_Allocator *ba = 0);
        //   bael_RecordAttributes(const bael_RecordAttributes& orig, *ba = 0);
        //   ~bael_RecordAttributes();
        //   bael_RecordAttributes& operator=(const bael_RecordAttributes&);
        //   void setCategory(const char *category);
        //   void setFileName(const char *fileName);
        //   void setLineNumber(int lineNumber);
        //   void setMessage(const char *message);
        //   void setProcessID(int processID);
        //   void setSeverity(int severity);
        //   void setThreadID(bsls_PlatformUtil::Uint64 threadID);
        //   void setTimestamp(const bdet_Datetime& timestamp);
        //   const char *category() const;
        //   const char *fileName() const;
        //   int lineNumber() const;
        //   const char *message() const;
        //   int processID() const;
        //   int severity() const;
        //   bsls_PlatformUtil::Uint64 threadID() const;
        //   const bdet_Datetime& timestamp() const;
        //   void clearMessage();
        //   bdesb_MemOutStreamBuf& messageStreamBuf();
        //   ostream& print(ostream& os, int level = 0, int spl = 4) const;
        //   bool operator==(const Obj& lhs, const Obj& rhs);
        //   bool operator!=(const Obj& lhs, const Obj& rhs);
        //   ostream& operator<<(ostream& os, const bael_RecordAttributes&);
        //
        //   Note: '^' indicates a private method which is tested indirectly.
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "Basic Attribute Test" << endl
                                  << "====================" << endl;

        const char         *DEFAULT_CATEGORY   = "";
        const char         *DEFAULT_FILENAME   = "";
        const int           DEFAULT_LINENUMBER = 0;
        const char         *DEFAULT_MESSAGE    = "";
        const int           DEFAULT_PROCESSID  = 0;
        const int           DEFAULT_SEVERITY   = 0;
        const int           DEFAULT_THREADID   = 0;
        const bdet_Datetime DEFAULT_TIMESTAMP;

        const char         *OTHER_CATEGORY   = "category";
        const char         *OTHER_FILENAME   = "filename.cpp";
        const int           OTHER_LINENUMBER = L_;
        const char         *OTHER_MESSAGE    = "message";
        const int           OTHER_PROCESSID  = 13248;
        const int           OTHER_SEVERITY   = 128;
        const int           OTHER_THREADID   = 19;
        bdet_Datetime       OTHER_TIMESTAMP;
        bdetu_Datetime::convertFromTimeT(&OTHER_TIMESTAMP, time(0));  // now

        Obj mX;  const Obj& X = mX;
        Obj mY;  const Obj& Y = mY;
        Obj mZ;  const Obj& Z = mZ;  // Z is the control
        if (veryVeryVerbose) { P_(X); P_(Y); P(Z); }

        if (veryVerbose) cout << "\n Check default ctor." << endl;

        ASSERT(0 == strcmp(DEFAULT_CATEGORY, X.category()));
        ASSERT(0 == strcmp(DEFAULT_FILENAME, X.fileName()));
        ASSERT(0 == strcmp(DEFAULT_MESSAGE,  X.message()));
        ASSERT(DEFAULT_LINENUMBER == X.lineNumber());
        ASSERT(DEFAULT_PROCESSID  == X.processID());
        ASSERT(DEFAULT_SEVERITY   == X.severity());
        ASSERT(DEFAULT_THREADID   == X.threadID());
        ASSERT(DEFAULT_TIMESTAMP  == X.timestamp());
        ASSERT(1 == (X == X));          ASSERT(0 == (X != X));
        ASSERT(1 == (X == Z));          ASSERT(0 == (X != Z));
        ASSERT(1 == (Z == Y));          ASSERT(0 == (Z != Y));
        ASSERT(1 == (Y == Z));          ASSERT(0 == (Y != Z));

        if (veryVerbose)
            cout << "\t    Setting default values explicitly." << endl;

        mX.setCategory(DEFAULT_CATEGORY);
        mX.setFileName(DEFAULT_FILENAME);
        mX.setLineNumber(DEFAULT_LINENUMBER);
        mX.setMessage(DEFAULT_MESSAGE);
        mX.setProcessID(DEFAULT_PROCESSID);
        mX.setSeverity(DEFAULT_SEVERITY);
        mX.setThreadID(DEFAULT_THREADID);
        mX.setTimestamp(DEFAULT_TIMESTAMP);
        ASSERT(1 == (X == X));          ASSERT(0 == (X != X));
        ASSERT(1 == (X == Z));          ASSERT(0 == (X != Z));
        ASSERT(1 == (Z == Y));          ASSERT(0 == (Z != Y));
        ASSERT(1 == (Y == Z));          ASSERT(0 == (Y != Z));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (veryVerbose) {
             cout << "\tTesting set/get methods." << endl
                  << "\t  Change attribute 0."    << endl;
        }

        mX.setCategory(OTHER_CATEGORY);
        ASSERT(0 == strcmp(OTHER_CATEGORY,   X.category()));
        ASSERT(0 == strcmp(DEFAULT_FILENAME, X.fileName()));
        ASSERT(0 == strcmp(DEFAULT_MESSAGE,  X.message()));
        ASSERT(DEFAULT_LINENUMBER == X.lineNumber());
        ASSERT(DEFAULT_PROCESSID  == X.processID());
        ASSERT(DEFAULT_SEVERITY   == X.severity());
        ASSERT(DEFAULT_THREADID   == X.threadID());
        ASSERT(DEFAULT_TIMESTAMP  == X.timestamp());
        ASSERT(1 == (X == X));          ASSERT(0 == (X != X));
        ASSERT(0 == (X == Z));          ASSERT(1 == (X != Z));
        ASSERT(0 == (Z == X));          ASSERT(1 == (Z != X));
        ASSERT(1 == (Y == Z));          ASSERT(0 == (Y != Z));
        {
            Obj C(X);
            ASSERT(C == X == 1);        ASSERT(C != X == 0);
        }

        mY = X;
        ASSERT(1 == (Y == Y));          ASSERT(0 == (Y != Y));
        ASSERT(1 == (Y == X));          ASSERT(0 == (Y != X));
        ASSERT(0 == (Y == Z));          ASSERT(1 == (Y != Z));

        mX.setCategory(DEFAULT_CATEGORY);
        ASSERT(1 == (X == X));          ASSERT(0 == (X != X));
        ASSERT(1 == (X == Z));          ASSERT(0 == (X != Z));
        ASSERT(0 == (Y == Z));          ASSERT(1 == (Y != Z));

        mX = mY = Z;
        ASSERT(1 == (X == X));          ASSERT(0 == (X != X));
        ASSERT(1 == (X == Z));          ASSERT(0 == (X != Z));
        ASSERT(1 == (Y == Z));          ASSERT(0 == (Y != Z));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (veryVerbose) {
             cout << "\tTesting 'clearMessage'" << endl;
        }
        {
            Obj mA;
            char testMessage[] = "test-message";
            ASSERT(0 == strcmp("", mA.message()));
            mA.setMessage(testMessage);
            ASSERT(0 == strcmp(testMessage, mA.message()));
            mA.clearMessage();
            ASSERT(0 == strcmp("", mA.message()));
        }
        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (veryVerbose) {
             cout << "\tTesting 'messageStreamBuf'" << endl;
        }
        {
            Obj mA;
            ASSERT(0 == strcmp("", mA.message()));

            char str1[] = "str1";
            char str2[] = "str2";
            char str3[] = "str3";

            ostream os(&mA.messageStreamBuf());

            os << str1;
            ASSERT(0 == strcmp("str1", mA.message()));

            os << str2;
            ASSERT(0 == strcmp("str1str2", mA.message()));

            os << str3;
            ASSERT(0 == strcmp("str1str2str3", mA.message()));

            mA.clearMessage();

            os << str1;
            ASSERT(0 == strcmp("str1", mA.message()));

            os << str2;
            ASSERT(0 == strcmp("str1str2", mA.message()));

            os << str3;
            ASSERT(0 == strcmp("str1str2str3", mA.message()));
        }
        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (veryVerbose) cout << "\t  Change attribute 1." << endl;

        mX.setFileName(OTHER_FILENAME);
        ASSERT(0 == strcmp(DEFAULT_CATEGORY, X.category()));
        ASSERT(0 == strcmp(OTHER_FILENAME,   X.fileName()));
        ASSERT(0 == strcmp(DEFAULT_MESSAGE,  X.message()));
        ASSERT(DEFAULT_LINENUMBER == X.lineNumber());
        ASSERT(DEFAULT_PROCESSID  == X.processID());
        ASSERT(DEFAULT_SEVERITY   == X.severity());
        ASSERT(DEFAULT_THREADID   == X.threadID());
        ASSERT(DEFAULT_TIMESTAMP  == X.timestamp());
        ASSERT(1 == (X == X));          ASSERT(0 == (X != X));
        ASSERT(0 == (X == Z));          ASSERT(1 == (X != Z));
        ASSERT(0 == (Z == X));          ASSERT(1 == (Z != X));
        ASSERT(1 == (Y == Z));          ASSERT(0 == (Y != Z));
        {
            Obj C(X);
            ASSERT(C == X == 1);        ASSERT(C != X == 0);
        }

        mY = X;
        ASSERT(1 == (Y == Y));          ASSERT(0 == (Y != Y));
        ASSERT(1 == (Y == X));          ASSERT(0 == (Y != X));
        ASSERT(0 == (Y == Z));          ASSERT(1 == (Y != Z));

        mX.setFileName(DEFAULT_FILENAME);
        ASSERT(1 == (X == X));          ASSERT(0 == (X != X));
        ASSERT(1 == (X == Z));          ASSERT(0 == (X != Z));
        ASSERT(0 == (Y == Z));          ASSERT(1 == (Y != Z));

        mX = mY = Z;
        ASSERT(1 == (X == X));          ASSERT(0 == (X != X));
        ASSERT(1 == (X == Z));          ASSERT(0 == (X != Z));
        ASSERT(1 == (Y == Z));          ASSERT(0 == (Y != Z));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (veryVerbose) cout << "\t  Change attribute 2." << endl;

        mX.setMessage(OTHER_MESSAGE);
        ASSERT(0 == strcmp(DEFAULT_CATEGORY, X.category()));
        ASSERT(0 == strcmp(DEFAULT_FILENAME, X.fileName()));
        ASSERT(0 == strcmp(OTHER_MESSAGE,    X.message()));
        ASSERT(DEFAULT_LINENUMBER == X.lineNumber());
        ASSERT(DEFAULT_PROCESSID  == X.processID());
        ASSERT(DEFAULT_SEVERITY   == X.severity());
        ASSERT(DEFAULT_THREADID   == X.threadID());
        ASSERT(DEFAULT_TIMESTAMP  == X.timestamp());
        ASSERT(1 == (X == X));          ASSERT(0 == (X != X));
        ASSERT(0 == (X == Z));          ASSERT(1 == (X != Z));
        ASSERT(0 == (Z == X));          ASSERT(1 == (Z != X));
        ASSERT(1 == (Y == Z));          ASSERT(0 == (Y != Z));
        {
            Obj C(X);
            ASSERT(C == X == 1);        ASSERT(C != X == 0);
        }

        mY = X;
        ASSERT(1 == (Y == Y));          ASSERT(0 == (Y != Y));
        ASSERT(1 == (Y == X));          ASSERT(0 == (Y != X));
        ASSERT(0 == (Y == Z));          ASSERT(1 == (Y != Z));

        mX.setMessage(DEFAULT_MESSAGE);
        ASSERT(1 == (X == X));          ASSERT(0 == (X != X));
        ASSERT(1 == (X == Z));          ASSERT(0 == (X != Z));
        ASSERT(0 == (Y == Z));          ASSERT(1 == (Y != Z));

        mX = mY = Z;
        ASSERT(1 == (X == X));          ASSERT(0 == (X != X));
        ASSERT(1 == (X == Z));          ASSERT(0 == (X != Z));
        ASSERT(1 == (Y == Z));          ASSERT(0 == (Y != Z));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (veryVerbose) cout << "\t  Change attribute 3." << endl;

        mX.setLineNumber(OTHER_LINENUMBER);
        ASSERT(0 == strcmp(DEFAULT_CATEGORY, X.category()));
        ASSERT(0 == strcmp(DEFAULT_FILENAME, X.fileName()));
        ASSERT(0 == strcmp(DEFAULT_MESSAGE,  X.message()));
        ASSERT(OTHER_LINENUMBER  == X.lineNumber());
        ASSERT(DEFAULT_PROCESSID == X.processID());
        ASSERT(DEFAULT_SEVERITY  == X.severity());
        ASSERT(DEFAULT_THREADID  == X.threadID());
        ASSERT(DEFAULT_TIMESTAMP == X.timestamp());
        ASSERT(1 == (X == X));          ASSERT(0 == (X != X));
        ASSERT(0 == (X == Z));          ASSERT(1 == (X != Z));
        ASSERT(0 == (Z == X));          ASSERT(1 == (Z != X));
        ASSERT(1 == (Y == Z));          ASSERT(0 == (Y != Z));
        {
            Obj C(X);
            ASSERT(C == X == 1);        ASSERT(C != X == 0);
        }

        mY = X;
        ASSERT(1 == (Y == Y));          ASSERT(0 == (Y != Y));
        ASSERT(1 == (Y == X));          ASSERT(0 == (Y != X));
        ASSERT(0 == (Y == Z));          ASSERT(1 == (Y != Z));

        mX.setLineNumber(DEFAULT_LINENUMBER);
        ASSERT(1 == (X == X));          ASSERT(0 == (X != X));
        ASSERT(1 == (X == Z));          ASSERT(0 == (X != Z));
        ASSERT(0 == (Y == Z));          ASSERT(1 == (Y != Z));

        mX = mY = Z;
        ASSERT(1 == (X == X));          ASSERT(0 == (X != X));
        ASSERT(1 == (X == Z));          ASSERT(0 == (X != Z));
        ASSERT(1 == (Y == Z));          ASSERT(0 == (Y != Z));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (veryVerbose) cout << "\t  Change attribute 4." << endl;

        mX.setProcessID(OTHER_PROCESSID);
        ASSERT(0 == strcmp(DEFAULT_CATEGORY, X.category()));
        ASSERT(0 == strcmp(DEFAULT_FILENAME, X.fileName()));
        ASSERT(0 == strcmp(DEFAULT_MESSAGE,  X.message()));
        ASSERT(DEFAULT_LINENUMBER == X.lineNumber());
        ASSERT(OTHER_PROCESSID    == X.processID());
        ASSERT(DEFAULT_SEVERITY   == X.severity());
        ASSERT(DEFAULT_THREADID   == X.threadID());
        ASSERT(DEFAULT_TIMESTAMP  == X.timestamp());
        ASSERT(1 == (X == X));          ASSERT(0 == (X != X));
        ASSERT(0 == (X == Z));          ASSERT(1 == (X != Z));
        ASSERT(0 == (Z == X));          ASSERT(1 == (Z != X));
        ASSERT(1 == (Y == Z));          ASSERT(0 == (Y != Z));
        {
            Obj C(X);
            ASSERT(C == X == 1);        ASSERT(C != X == 0);
        }

        mY = X;
        ASSERT(1 == (Y == Y));          ASSERT(0 == (Y != Y));
        ASSERT(1 == (Y == X));          ASSERT(0 == (Y != X));
        ASSERT(0 == (Y == Z));          ASSERT(1 == (Y != Z));

        mX.setProcessID(DEFAULT_PROCESSID);
        ASSERT(1 == (X == X));          ASSERT(0 == (X != X));
        ASSERT(1 == (X == Z));          ASSERT(0 == (X != Z));
        ASSERT(0 == (Y == Z));          ASSERT(1 == (Y != Z));

        mX = mY = Z;
        ASSERT(1 == (X == X));          ASSERT(0 == (X != X));
        ASSERT(1 == (X == Z));          ASSERT(0 == (X != Z));
        ASSERT(1 == (Y == Z));          ASSERT(0 == (Y != Z));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (veryVerbose) cout << "\t  Change attribute 5." << endl;

        mX.setSeverity(OTHER_SEVERITY);
        ASSERT(0 == strcmp(DEFAULT_CATEGORY, X.category()));
        ASSERT(0 == strcmp(DEFAULT_FILENAME, X.fileName()));
        ASSERT(0 == strcmp(DEFAULT_MESSAGE,  X.message()));
        ASSERT(DEFAULT_LINENUMBER == X.lineNumber());
        ASSERT(DEFAULT_PROCESSID  == X.processID());
        ASSERT(OTHER_SEVERITY     == X.severity());
        ASSERT(DEFAULT_THREADID   == X.threadID());
        ASSERT(DEFAULT_TIMESTAMP  == X.timestamp());
        ASSERT(1 == (X == X));          ASSERT(0 == (X != X));
        ASSERT(0 == (X == Z));          ASSERT(1 == (X != Z));
        ASSERT(0 == (Z == X));          ASSERT(1 == (Z != X));
        ASSERT(1 == (Y == Z));          ASSERT(0 == (Y != Z));
        {
            Obj C(X);
            ASSERT(C == X == 1);        ASSERT(C != X == 0);
        }

        mY = X;
        ASSERT(1 == (Y == Y));          ASSERT(0 == (Y != Y));
        ASSERT(1 == (Y == X));          ASSERT(0 == (Y != X));
        ASSERT(0 == (Y == Z));          ASSERT(1 == (Y != Z));

        mX.setSeverity(DEFAULT_SEVERITY);
        ASSERT(1 == (X == X));          ASSERT(0 == (X != X));
        ASSERT(1 == (X == Z));          ASSERT(0 == (X != Z));
        ASSERT(0 == (Y == Z));          ASSERT(1 == (Y != Z));

        mX = mY = Z;
        ASSERT(1 == (X == X));          ASSERT(0 == (X != X));
        ASSERT(1 == (X == Z));          ASSERT(0 == (X != Z));
        ASSERT(1 == (Y == Z));          ASSERT(0 == (Y != Z));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (veryVerbose) cout << "\t  Change attribute 6." << endl;

        mX.setThreadID(OTHER_THREADID);
        ASSERT(0 == strcmp(DEFAULT_CATEGORY, X.category()));
        ASSERT(0 == strcmp(DEFAULT_FILENAME, X.fileName()));
        ASSERT(0 == strcmp(DEFAULT_MESSAGE,  X.message()));
        ASSERT(DEFAULT_LINENUMBER == X.lineNumber());
        ASSERT(DEFAULT_PROCESSID  == X.processID());
        ASSERT(DEFAULT_SEVERITY   == X.severity());
        ASSERT(OTHER_THREADID     == X.threadID());
        ASSERT(DEFAULT_TIMESTAMP  == X.timestamp());
        ASSERT(1 == (X == X));          ASSERT(0 == (X != X));
        ASSERT(0 == (X == Z));          ASSERT(1 == (X != Z));
        ASSERT(0 == (Z == X));          ASSERT(1 == (Z != X));
        ASSERT(1 == (Y == Z));          ASSERT(0 == (Y != Z));
        {
            Obj C(X);
            ASSERT(C == X == 1);        ASSERT(C != X == 0);
        }

        mY = X;
        ASSERT(1 == (Y == Y));          ASSERT(0 == (Y != Y));
        ASSERT(1 == (Y == X));          ASSERT(0 == (Y != X));
        ASSERT(0 == (Y == Z));          ASSERT(1 == (Y != Z));

        mX.setThreadID(DEFAULT_THREADID);
        ASSERT(1 == (X == X));          ASSERT(0 == (X != X));
        ASSERT(1 == (X == Z));          ASSERT(0 == (X != Z));
        ASSERT(0 == (Y == Z));          ASSERT(1 == (Y != Z));

        mX = mY = Z;
        ASSERT(1 == (X == X));          ASSERT(0 == (X != X));
        ASSERT(1 == (X == Z));          ASSERT(0 == (X != Z));
        ASSERT(1 == (Y == Z));          ASSERT(0 == (Y != Z));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (veryVerbose) cout << "\t  Change attribute 7." << endl;

        mX.setTimestamp(OTHER_TIMESTAMP);
        ASSERT(0 == strcmp(DEFAULT_CATEGORY, X.category()));
        ASSERT(0 == strcmp(DEFAULT_FILENAME, X.fileName()));
        ASSERT(0 == strcmp(DEFAULT_MESSAGE,  X.message()));
        ASSERT(DEFAULT_LINENUMBER == X.lineNumber());
        ASSERT(DEFAULT_PROCESSID  == X.processID());
        ASSERT(DEFAULT_SEVERITY   == X.severity());
        ASSERT(DEFAULT_THREADID   == X.threadID());
        ASSERT(OTHER_TIMESTAMP    == X.timestamp());
        ASSERT(1 == (X == X));          ASSERT(0 == (X != X));
        ASSERT(0 == (X == Z));          ASSERT(1 == (X != Z));
        ASSERT(0 == (Z == X));          ASSERT(1 == (Z != X));
        ASSERT(1 == (Y == Z));          ASSERT(0 == (Y != Z));
        {
            Obj C(X);
            ASSERT(C == X == 1);        ASSERT(C != X == 0);
        }

        mY = X;
        ASSERT(1 == (Y == Y));          ASSERT(0 == (Y != Y));
        ASSERT(1 == (Y == X));          ASSERT(0 == (Y != X));
        ASSERT(0 == (Y == Z));          ASSERT(1 == (Y != Z));

        mX.setTimestamp(DEFAULT_TIMESTAMP);
        ASSERT(1 == (X == X));          ASSERT(0 == (X != X));
        ASSERT(1 == (X == Z));          ASSERT(0 == (X != Z));
        ASSERT(0 == (Y == Z));          ASSERT(1 == (Y != Z));

        mX = mY = Z;
        ASSERT(1 == (X == X));          ASSERT(0 == (X != X));
        ASSERT(1 == (X == Z));          ASSERT(0 == (X != Z));
        ASSERT(1 == (Y == Z));          ASSERT(0 == (Y != Z));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (veryVerbose) cout << "Testing output operator (<<)." << endl;
        {
            char buf[1024];
            ostrstream o(buf, sizeof buf);

            Obj mX;  const Obj& X = mX;
            mX.setCategory("category");
            mX.setFileName("bael_recordattributes.t.cpp");
            mX.setLineNumber(1066);
            mX.setMessage("message");
            mX.setProcessID(74372);
            mX.setSeverity(128);
            mX.setThreadID(19);

            o << X << ends;
            bsl::string s = "[ 01JAN0001_24:00:00.000 "
                            "74372 "
                            "19 "
                            "bael_recordattributes.t.cpp "
                            "1066 "
                            "category "
                            "128 "
                            "message "
                            "]";
            if (veryVeryVerbose) cout << "X buf:\n"    << buf << endl;
            if (veryVeryVerbose) cout << "Expected:\n" << s   << endl;
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
                { L_,    0,         -1,         "[ 01JAN0001_24:00:00.000 "
                                                "74372 "
                                                "19 "
                                                "bael_recordattributes.t.cpp "
                                                "1066 "
                                                "category "
                                                "128 "
                                                "message "
                                                "]"
                },

                { L_,    0,          0,        "["                           NL
                                               "01JAN0001_24:00:00.000"      NL
                                               "74372"                       NL
                                               "19"                          NL
                                               "bael_recordattributes.t.cpp" NL
                                               "1066"                        NL
                                               "category"                    NL
                                               "128"                         NL
                                               "message"                     NL
                                               "]"                           NL
                },

                { L_,    0,          2,      "["                             NL
                                             "  01JAN0001_24:00:00.000"      NL
                                             "  74372"                       NL
                                             "  19"                          NL
                                             "  bael_recordattributes.t.cpp" NL
                                             "  1066"                        NL
                                             "  category"                    NL
                                             "  128"                         NL
                                             "  message"                     NL
                                             "]"                             NL
                },

                { L_,    1,          1,      " ["                            NL
                                             "  01JAN0001_24:00:00.000"      NL
                                             "  74372"                       NL
                                             "  19"                          NL
                                             "  bael_recordattributes.t.cpp" NL
                                             "  1066"                        NL
                                             "  category"                    NL
                                             "  128"                         NL
                                             "  message"                     NL
                                             " ]"                            NL
                },

                { L_,    1,          2,    "  ["                             NL
                                           "    01JAN0001_24:00:00.000"      NL
                                           "    74372"                       NL
                                           "    19"                          NL
                                           "    bael_recordattributes.t.cpp" NL
                                           "    1066"                        NL
                                           "    category"                    NL
                                           "    128"                         NL
                                           "    message"                     NL
                                           "  ]"                             NL
                },

                { L_,   -1,          2,    "["                               NL
                                           "    01JAN0001_24:00:00.000"      NL
                                           "    74372"                       NL
                                           "    19"                          NL
                                           "    bael_recordattributes.t.cpp" NL
                                           "    1066"                        NL
                                           "    category"                    NL
                                           "    128"                         NL
                                           "    message"                     NL
                                           "  ]"                             NL
                },

                { L_,   -2,          1,     "["                              NL
                                            "   01JAN0001_24:00:00.000"      NL
                                            "   74372"                       NL
                                            "   19"                          NL
                                            "   bael_recordattributes.t.cpp" NL
                                            "   1066"                        NL
                                            "   category"                    NL
                                            "   128"                         NL
                                            "   message"                     NL
                                            "  ]"                            NL
                },

                { L_,    2,          1,    "  ["                             NL
                                           "   01JAN0001_24:00:00.000"       NL
                                           "   74372"                        NL
                                           "   19"                           NL
                                           "   bael_recordattributes.t.cpp"  NL
                                           "   1066"                         NL
                                           "   category"                     NL
                                           "   128"                          NL
                                           "   message"                      NL
                                           "  ]"                             NL
                },

                { L_,    1,          3,  "   ["                              NL
                                         "      01JAN0001_24:00:00.000"      NL
                                         "      74372"                       NL
                                         "      19"                          NL
                                         "      bael_recordattributes.t.cpp" NL
                                         "      1066"                        NL
                                         "      category"                    NL
                                         "      128"                         NL
                                         "      message"                     NL
                                         "   ]"                              NL
                },
            };
#undef NL
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            const int SIZE = 1024; // Must be big enough to hold output string.
            const char Z1  = (char) 0xFF;  // Value 1 used for an unset char.
            const char Z2  = 0x00; // Value 2 used to represent an unset char.

            char mCtrlBuf1[SIZE];  memset(mCtrlBuf1, Z1, SIZE);
            char mCtrlBuf2[SIZE];  memset(mCtrlBuf2, Z2, SIZE);
            const char *CTRL_BUF1 = mCtrlBuf1;
            const char *CTRL_BUF2 = mCtrlBuf2;

            Obj mX;  const Obj& X = mX;
            mX.setCategory("category");
            mX.setFileName("bael_recordattributes.t.cpp");
            mX.setLineNumber(1066);
            mX.setMessage("message");
            mX.setProcessID(74372);
            mX.setSeverity(128);
            mX.setThreadID(19);

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
                if (veryVerbose) cout << "ACTUAL FORMAT:  "<< endl<<buf1<<endl;

                const int SZ = strlen(FMT) + 1;
                const int REST = SIZE - SZ;
                LOOP_ASSERT(ti, SZ < SIZE);  // Check buffer is large enough.
                LOOP_ASSERT(ti, Z1 == buf1[SIZE - 1]);  // Check for overrun.
                LOOP_ASSERT(ti, Z2 == buf2[SIZE - 1]);  // Check for overrun.
                LOOP_ASSERT(ti,  0 == strcmp(buf1, FMT));
                LOOP_ASSERT(ti,  0 == strcmp(buf2, FMT));
                LOOP_ASSERT(ti,  0 == memcmp(buf1 + SZ, CTRL_BUF1 + SZ, REST));
                LOOP_ASSERT(ti,  0 == memcmp(buf2 + SZ, CTRL_BUF2 + SZ, REST));
            }
        }

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (veryVerbose) cout << "Test that the internal memory management "
                                 "system is hooked up properly." << endl;
        {
            char memory[2048];
            bdema_BufferedSequentialAllocator a(memory, sizeof memory);

            Obj *doNotDeleteMe = new(a.allocate(sizeof(Obj))) Obj(&a);
            Obj& mX = *doNotDeleteMe;  const Obj& X = mX;
            mX.setCategory("a_string_long_enough_to_trigger_allocation");
            mX.setFileName("a_string_long_enough_to_trigger_allocation");
            mX.setMessage("a_string_long_enough_to_trigger_allocation");

            new(a.allocate(sizeof(Obj))) Obj(X, &a);

            // No destructor is called; will produce memory leak in purify
            // if internal allocators are not hooked up properly.
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //
        // Concerns:
        //   We want to exercise basic value-semantic functionality.  In
        //   particular, we want to demonstrate a base-line level of correct
        //   operation.  In addition, we would like to exercise objects with
        //   potentially different internal organizations representing the
        //   same value.
        //
        // Plan:
        //   Create four objects using both the default and copy constructors.
        //   Exercise these objects using primary manipulators, basic
        //   accessors, equality operators, and the assignment operator.
        //   Invoke the primary manipulator [1&7], copy constructor [2&8],
        //   and assignment operator [10&11].  Try aliasing with assignment
        //   for a non-empty instance [11] and allow the result to
        //   leave scope, enabling the destructor to assert internal object
        //   invariants.  Display object values frequently in verbose mode.
        //
        //    1. Create an object x1 (default ctor).
        //    2. Create a second object x2 (copy from x1).
        //    3. Set processID and threadID for x1 and x2.
        //    4. Set lineNumber and severity for x1.
        //    5. Set lineNumber and severity for x2. (different)
        //    6. Set timeStamp for x1 and x2.
        //    7. Create a third object x3 (explicit ctor).
        //    8. Create a fourth object x4 (copy of x3).
        //    9. Set fileName, category, and message for x3.
        //   10. Assign x2 = x1, x3 = x4
        //   11. Assign x4 = x4 (aliasing).
        //
        // Testing:
        //   This "test" *exercises* basic functionality, but *tests* nothing.
        //
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "BREATHING TEST" << endl
                                  << "==============" << endl;

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (veryVerbose) cout << "\n 1. Create an object x1 (default ctor)"
                              << endl;

        Obj mX1(&testAllocator);                const Obj& X1 = mX1;
        if (veryVeryVerbose) { T_(); P(X1); }

        if (veryVerbose) cout << "\ta. Try equality operators -- x1 <op> x1."
                              << endl;

        ASSERT(1 == (X1 == X1));                ASSERT(0 == (X1 != X1));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (veryVerbose)
            cout << "\n 2. Create a second object x2 (copy from x1)" << endl;

        Obj mX2(X1, &testAllocator);            const Obj& X2 = mX2;
        if (veryVeryVerbose) { T_(); P(X2); }

        if (veryVerbose) {
            cout << "\ta. Try equality operators -- x2 <op> x1, x2."
                 << endl;
        }

        ASSERT(1 == (X2 == X1));                ASSERT(0 == (X2 != X1));
        ASSERT(1 == (X2 == X2));                ASSERT(0 == (X2 != X2));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (veryVerbose) cout << "\n 3. Set processID and threadID." << endl;

        mX1.setProcessID(12345);                mX2.setProcessID(12345);
        mX1.setThreadID(678);                   mX2.setThreadID(678);
        if (veryVeryVerbose) { T_(); P(X1); }
        if (veryVeryVerbose) { T_(); P(X2); }

        if (veryVerbose) {
            cout << "\ta. Try equality operators -- x2 <op> x1, x2."
                 << endl;
        }

        ASSERT(1 == (X2 == X1));                ASSERT(0 == (X2 != X1));
        ASSERT(1 == (X2 == X2));                ASSERT(0 == (X2 != X2));
        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (veryVerbose) cout << "\n 4. Set lineNumber and severity for x1."
                              << endl;

        mX1.setLineNumber(L_);
        mX1.setSeverity(1);
        if (veryVeryVerbose) { T_(); P(X1); }

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (veryVerbose) cout << "\n 5. Set lineNumber and severity for x2."
                              << endl;

        mX2.setLineNumber(L_);
        mX2.setSeverity(1);
        if (veryVeryVerbose) { T_(); P(X2); }

        if (veryVerbose) cout << "\ta. Try equality operators -- x2 <op> x1"
                                 " (should be different)" << endl;

        ASSERT(0 == (X2 == X1));                ASSERT(1 == (X2 != X1));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (veryVerbose) cout << "\n 6. Set timeStamp for x1 and x2." << endl;

        bdet_Datetime TIMESTAMP;
        bdetu_Datetime::convertFromTimeT(&TIMESTAMP, time(0));  // now

        mX1.setTimestamp(TIMESTAMP);
        mX2.setTimestamp(TIMESTAMP);
        if (veryVeryVerbose) { T_(); P(X1); }
        if (veryVeryVerbose) { T_(); P(X2); }

        if (veryVerbose) cout << "\ta. Try equality operators -- x2 <op> x1"
                                 " (should be different)" << endl;

        ASSERT(0 == (X2 == X1));                ASSERT(1 == (X2 != X1));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (veryVerbose)
            cout << "\n 7. Create a third object x3 (explicit ctor)." << endl;

        const char   *CATEGORY   = "Bonds";
        const char   *FILENAME   = "bondmarket.cpp";
        const int     LINENUMBER = L_;
        const char   *MESSAGE    = "Bond market is closed.";
        const int     PROCESSID  = 234178;
        const int     SEVERITY   = 128;
        const int     THREADID   = 15;

        Obj mX3(TIMESTAMP,
                PROCESSID,
                THREADID,
                FILENAME,
                LINENUMBER,
                CATEGORY,
                SEVERITY,
                MESSAGE,
                &testAllocator);
        const Obj& X3 = mX3;
        if (veryVeryVerbose) { T_(); P(X3); }

        if (veryVerbose) cout << "\ta. Validate getter functions." << endl;

        ASSERT(0 == strcmp(X3.category(), CATEGORY));
        ASSERT(0 == strcmp(X3.fileName(), FILENAME));
        ASSERT(0 == strcmp(X3.message(),  MESSAGE));
        ASSERT(X3.lineNumber() == LINENUMBER);
        ASSERT(X3.processID()  == PROCESSID);
        ASSERT(X3.threadID()   == THREADID);
        ASSERT(X3.timestamp()  == TIMESTAMP);
        ASSERT(X3.severity()   == SEVERITY);

        if (veryVerbose) cout << "\tb. Try equality operators -- x3 <op> x3."
                              << endl;

        ASSERT(1 == (X3 == X3));                ASSERT(0 == (X3 != X3));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (veryVerbose)
            cout << "\n 8. Create a fourth object x4 (copy of x3)." << endl;

        Obj mX4(X3, &testAllocator);            const Obj& X4 = mX4;
        if (veryVeryVerbose) { T_(); P(X4); }

        if (veryVerbose) {
            cout << "\ta. Try equality operators -- x4 <op> x4, x3."
                 << endl;
        }

        ASSERT(1 == (X4 == X3));                ASSERT(0 == (X4 != X3));
        ASSERT(1 == (X4 == X4));                ASSERT(0 == (X4 != X4));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (veryVerbose)
            cout << "\n 9. Set fileName, category, message for x3." << endl;

        const char *CAT = "Testing";
        const char *FN  = "Myfile.cpp";
        const char *MSG = "This message validates the set function.";
        mX3.setCategory(CAT);
        mX3.setFileName(FN);
        mX3.setMessage(MSG);
        if (veryVeryVerbose) { T_(); P(X3); }

        if (veryVerbose) cout << "\ta. Validate setters with getters" << endl;

        ASSERT(0 == strcmp(X3.category(), CAT));
        ASSERT(0 == strcmp(X3.fileName(), FN));
        ASSERT(0 == strcmp(X3.message(),  MSG));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (veryVerbose) cout << "\n 10. Assign x2 = x1, x3 = x4." << endl;

        mX2 = X1;
        if (veryVeryVerbose) { T_(); P(X2); }
        mX3 = X4;
        if (veryVeryVerbose) { T_(); P(X3); }

        if (veryVerbose)
            cout << "\ta. Verify equality -- x2 <op> x1, x3 <op> x4." << endl;

        ASSERT(1 == (X2 == X1));                ASSERT(0 == (X2 != X1));
        ASSERT(1 == (X3 == X4));                ASSERT(0 == (X3 != X4));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (veryVerbose) cout << "\n 11. Assign x4 = x4." << endl;

        mX4 = X4;
        if (veryVeryVerbose) { T_(); P(X4); }

        if (veryVerbose) cout << "\ta. Verify identify -- x4 <op> x3. "
                                 "(x4 == x3 from previous step)" << endl;

        ASSERT(1 == (X4 == X3));                ASSERT(0 == (X4 != X3));

        if (veryVerbose) cout << "\tb. Verify equality -- x4 <op> x4." << endl;

        ASSERT(1 == (X4 == X4));                ASSERT(0 == (X4 != X4));

        if (veryVerbose)
            cout << "Test complete - deallocating memory." << endl;

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
