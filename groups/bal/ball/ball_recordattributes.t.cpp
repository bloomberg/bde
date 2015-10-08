// ball_recordattributes.t.cpp                                        -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------


#include <ball_recordattributes.h>

#include <bdlma_bufferedsequentialallocator.h>  // for testing only
#include <bdlt_datetimeutil.h>                     // for testing only
#include <bdlt_epochutil.h>                        // for testing only

#include <bslma_testallocator.h>
#include <bslma_testallocatorexception.h>
#include <bsls_platform.h>
#include <bsls_types.h>

#include <bsl_cstdlib.h>      // atoi()
#include <bsl_cstring.h>      // strlen(), memset(), memcpy(), memcmp()
#ifdef BSLS_PLATFORM_OS_UNIX
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
//    'ball::RecordAttributes' objects behave correctly in normal streaming
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
// 'ball::RecordAttributes' public interface:
// [ 2] ball::RecordAttributes(bslma::Allocator *ba = 0);
// [ 4] ball::RecordAttributes(ts, pid, tid, fn, ln, cat, sev, msg, *ba=0);
// [ 2] ball::RecordAttributes(const ball::RecordAttributes& orig, *ba = 0);
// [ 2] ~ball::RecordAttributes();
// [ 2] ball::RecordAttributes& operator=(const ball::RecordAttributes&);
// [ 2] void setCategory(const char *category);
// [ 2] void setFileName(const char *fileName);
// [ 2] void setLineNumber(int lineNumber);
// [ 2] void setMessage(const char *message);
// [ 2] void setProcessID(int processID);
// [ 2] void setSeverity(int severity);
// [ 2] void setThreadID(bsls::Types::Uint64 threadID);
// [ 2] void setTimestamp(const bdlt::Datetime& timestamp);
// [ 2] const char *category() const;
// [ 2] const char *fileName() const;
// [ 2] int lineNumber() const;
// [ 2] const char *message() const;
// [ 2] bslstl::StringRef messageRef() const;
// [ 2] int processID() const;
// [ 2] int severity() const;
// [ 2] bsls::Types::Uint64 threadID() const;
// [ 2] const bdlt::Datetime& timestamp() const;
// [ 2] void clearMessage();
// [ 2] bdlsb::MemOutStreamBuf& messageStreamBuf();
// [ 3] ostream& print(ostream& os, int level = 0, int spl = 4) const;
//
// [ 2] bool operator==(const Obj& lhs, const Obj& rhs);
// [ 2] bool operator!=(const Obj& lhs, const Obj& rhs);
// [ 2] ostream& operator<<(ostream& os, const ball::RecordAttributes&);
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

typedef ball::RecordAttributes Obj;

struct my_RecordAttributes {
    bdlt::Datetime        timestamp;
    int                  processID;
    bsls::Types::Uint64  threadID;
    const char          *fileName;
    int                  lineNumber;
    const char          *category;
    int                  severity;
    const char          *message;
};

struct my_TestMessage {
    const char *msg;
    int         len1;
    int         len2;
};

my_TestMessage testMsgs[] = {
    {"",              0,  0},
    {"\0",            0,  0},
    {"a",             1,  1},
    {"a\0",           1,  1},
    {"ab",            2,  2},
    {"ab\0",          2,  2},
    {"abc",           3,  3},
    {"abc\0",         3,  3},
    {"abc\0d",        3,  5},
    {"abc\0de",       3,  6},
    {"abc\0def",      3,  7},
    {"abc\0def\0",    3,  7},
    {"abc\0def\0g",   3,  9},
    {"abc\0def\0gh",  3, 10},
    {"abc\0def\0ghi", 3, 11},
};

const int NUM_TEST_MSGS = sizeof(testMsgs) / sizeof(testMsgs[0]);

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

void streamInformationIntoMessageAttribute(ball::RecordAttributes& attributes,
                                           const Information& information)
{
    attributes.clearMessage();
    bsl::ostream os(&attributes.messageStreamBuf());
    os << information;
}
//=============================================================================
//                             USAGE EXAMPLE 1
//-----------------------------------------------------------------------------

void printMessage(ostream& stream, const ball::RecordAttributes& attributes)
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

void initRecordAttributes(ball::RecordAttributes&     lhs,
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
    ASSERT(0 == strcmp(OBJ.message(),  ORA.message));               \
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

    bslma::TestAllocator testAllocator(veryVeryVerbose);

    switch (test) { case 0:  // Zero is always the leading case.
      case 5: {
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
        ball::RecordAttributes attr;
        streamInformationIntoMessageAttribute(attr, info);
        if (verbose) {
            cout << attr.message();
        }

      } break;

      case 4: {
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

            ball::RecordAttributes attributes;
            bdlt::Datetime now = bdlt::EpochUtil::convertFromTimeT(time(0));
            attributes.setTimestamp(now);
            #ifdef BSLS_PLATFORM_OS_UNIX
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

      case 3: {
        // --------------------------------------------------------------------
        // Initialization Constructor Test
        //
        // Testing:
        //   ball::RecordAttributes(ts, pid, tid, fn, ln, cat, sev, msg, *ba=0);
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
        const bdlt::Datetime DEFAULT_TIMESTAMP;

        const char         *OTHER_CATEGORY   = "category";
        const char         *OTHER_FILENAME   = "filename.cpp";
        const int           OTHER_LINENUMBER = L_;
        const char         *OTHER_MESSAGE    = "message";
        const int           OTHER_PROCESSID  = 13248;
        const int           OTHER_SEVERITY   = 128;
        const int           OTHER_THREADID   = 19;
        bdlt::Datetime      OTHER_TIMESTAMP  =
                            bdlt::EpochUtil::convertFromTimeT(time(0));  // now

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
            bdlma::BufferedSequentialAllocator a(memory, sizeof memory);

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
      case 2: {
        // --------------------------------------------------------------------
        // Basic Attribute Test
        //
        // Testing:
        //   ball::RecordAttributes(bslma::Allocator *ba = 0);
        //   ball::RecordAttributes(const ball::RecordAttributes& orig, *ba = 0);
        //   ~ball::RecordAttributes();
        //   ball::RecordAttributes& operator=(const ball::RecordAttributes&);
        //   void setCategory(const char *category);
        //   void setFileName(const char *fileName);
        //   void setLineNumber(int lineNumber);
        //   void setMessage(const char *message);
        //   void setProcessID(int processID);
        //   void setSeverity(int severity);
        //   void setThreadID(bsls::Types::Uint64 threadID);
        //   void setTimestamp(const bdlt::Datetime& timestamp);
        //   const char *category() const;
        //   const char *fileName() const;
        //   int lineNumber() const;
        //   const char *message() const;
        //   bslstl::StringRef messageRef() const;
        //   int processID() const;
        //   int severity() const;
        //   bsls::Types::Uint64 threadID() const;
        //   const bdlt::Datetime& timestamp() const;
        //   void clearMessage();
        //   bdlsb::MemOutStreamBuf& messageStreamBuf();
        //   ostream& print(ostream& os, int level = 0, int spl = 4) const;
        //   bool operator==(const Obj& lhs, const Obj& rhs);
        //   bool operator!=(const Obj& lhs, const Obj& rhs);
        //   ostream& operator<<(ostream& os, const ball::RecordAttributes&);
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
        const bdlt::Datetime DEFAULT_TIMESTAMP;

        const char         *OTHER_CATEGORY   = "category";
        const char         *OTHER_FILENAME   = "filename.cpp";
        const int           OTHER_LINENUMBER = L_;
        const char         *OTHER_MESSAGE    = "message";
        const int           OTHER_PROCESSID  = 13248;
        const int           OTHER_SEVERITY   = 128;
        const int           OTHER_THREADID   = 19;
        bdlt::Datetime      OTHER_TIMESTAMP  =
                            bdlt::EpochUtil::convertFromTimeT(time(0));  // now

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
             cout << "\tTesting 'setMessage' and 'message'" << endl;
        }
        {
            for (int i = 0;i < NUM_TEST_MSGS; ++i) {
                Obj mA;
                ASSERT(0 == strcmp("", mA.message()));
                mA.setMessage(testMsgs[i].msg);
                ASSERT(0 == strcmp(testMsgs[i].msg, mA.message()));
            }
        }

        if (veryVerbose) {
             cout << "\tTesting 'messageRef'" << endl;
        }
        {
            bslstl::StringRef emptyMsgRef("", 0);
            for (int i = 0;i < NUM_TEST_MSGS; ++i) {
                Obj mA;
                bslstl::StringRef testMsgRef(testMsgs[i].msg,
                                             testMsgs[i].len2);
                ASSERT(emptyMsgRef == mA.messageRef());
                mA.messageStreamBuf().pubseekpos(0);
                mA.messageStreamBuf().sputn(testMsgs[i].msg, testMsgs[i].len2);

                bslstl::StringRef message = mA.messageRef();

                if (veryVeryVerbose) {
                    P_(testMsgRef); P_(testMsgRef.length()); P_(message);
                    P(message.length());
                }

                ASSERT(testMsgRef == message);
            }
        }

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
            mX.setFileName("ball_recordattributes.t.cpp");
            mX.setLineNumber(1066);
            mX.setMessage("message");
            mX.setProcessID(74372);
            mX.setSeverity(128);
            mX.setThreadID(19);

            o << X << ends;
            bsl::string s = "[ 01JAN0001_24:00:00.000 "
                            "74372 "
                            "19 "
                            "ball_recordattributes.t.cpp "
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
                                                "ball_recordattributes.t.cpp "
                                                "1066 "
                                                "category "
                                                "128 "
                                                "test-message "
                                                "]"
                },

                { L_,    0,          0,        "["                           NL
                                               "01JAN0001_24:00:00.000"      NL
                                               "74372"                       NL
                                               "19"                          NL
                                               "ball_recordattributes.t.cpp" NL
                                               "1066"                        NL
                                               "category"                    NL
                                               "128"                         NL
                                               "test-message"                NL
                                               "]"                           NL
                },

                { L_,    0,          2,      "["                             NL
                                             "  01JAN0001_24:00:00.000"      NL
                                             "  74372"                       NL
                                             "  19"                          NL
                                             "  ball_recordattributes.t.cpp" NL
                                             "  1066"                        NL
                                             "  category"                    NL
                                             "  128"                         NL
                                             "  test-message"                NL
                                             "]"                             NL
                },

                { L_,    1,          1,      " ["                            NL
                                             "  01JAN0001_24:00:00.000"      NL
                                             "  74372"                       NL
                                             "  19"                          NL
                                             "  ball_recordattributes.t.cpp" NL
                                             "  1066"                        NL
                                             "  category"                    NL
                                             "  128"                         NL
                                             "  test-message"                NL
                                             " ]"                            NL
                },

                { L_,    1,          2,    "  ["                             NL
                                           "    01JAN0001_24:00:00.000"      NL
                                           "    74372"                       NL
                                           "    19"                          NL
                                           "    ball_recordattributes.t.cpp" NL
                                           "    1066"                        NL
                                           "    category"                    NL
                                           "    128"                         NL
                                           "    test-message"                NL
                                           "  ]"                             NL
                },

                { L_,   -1,          2,    "["                               NL
                                           "    01JAN0001_24:00:00.000"      NL
                                           "    74372"                       NL
                                           "    19"                          NL
                                           "    ball_recordattributes.t.cpp" NL
                                           "    1066"                        NL
                                           "    category"                    NL
                                           "    128"                         NL
                                           "    test-message"                NL
                                           "  ]"                             NL
                },

                { L_,   -2,          1,     "["                              NL
                                            "   01JAN0001_24:00:00.000"      NL
                                            "   74372"                       NL
                                            "   19"                          NL
                                            "   ball_recordattributes.t.cpp" NL
                                            "   1066"                        NL
                                            "   category"                    NL
                                            "   128"                         NL
                                            "   test-message"                NL
                                            "  ]"                            NL
                },

                { L_,    2,          1,    "  ["                             NL
                                           "   01JAN0001_24:00:00.000"       NL
                                           "   74372"                        NL
                                           "   19"                           NL
                                           "   ball_recordattributes.t.cpp"  NL
                                           "   1066"                         NL
                                           "   category"                     NL
                                           "   128"                          NL
                                           "   test-message"                 NL
                                           "  ]"                             NL
                },

                { L_,    1,          3,  "   ["                              NL
                                         "      01JAN0001_24:00:00.000"      NL
                                         "      74372"                       NL
                                         "      19"                          NL
                                         "      ball_recordattributes.t.cpp" NL
                                         "      1066"                        NL
                                         "      category"                    NL
                                         "      128"                         NL
                                         "      test-message"                NL
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
            mX.setFileName("ball_recordattributes.t.cpp");
            mX.setLineNumber(1066);
            mX.messageStreamBuf().pubseekpos(0);
            mX.messageStreamBuf().sputn("test-\0mess\0age", 14);
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

                const int SZ = strlen(FMT) + 3;  // Count in the two '\0'.
                const int REST = SIZE - SZ;

                LOOP_ASSERT(ti, SZ < SIZE);  // Check buffer is large enough.
                LOOP_ASSERT(ti, Z1 == buf1[SIZE - 1]);  // Check for overrun.
                LOOP_ASSERT(ti, Z2 == buf2[SIZE - 1]);  // Check for overrun.

                // Verify 'buf1' and 'buf2' have same conetents as 'FMT', if
                // ignore the '\0's in the middle of 'buf1' and 'buf2'.

                const char *p1 = FMT;
                const char *p2 = buf1;
                int tj, tk;
                for (tj = 0, tk = 0;tj < strlen(FMT); ++tj, ++tk) {
                    if (0 == p2[tk]) {  // Skip '\0' in the middle.
                        ++tk;
                    }
                    LOOP2_ASSERT(tj, tk, p1[tj] == p2[tk]);
                }
                LOOP_ASSERT(tj, 0 == p1[tj]);
                LOOP_ASSERT(tk, 0 == p2[tk]);

                p1 = FMT;
                p2 = buf2;
                for (tj = 0, tk = 0;tj < strlen(FMT); ++tj, ++tk) {
                    if (0 == p2[tk]) {  // Skip '\0' in the middle.
                        ++tk;
                    }
                    LOOP2_ASSERT(tj, tk, p1[tj] == p2[tk]);
                }
                LOOP_ASSERT(tj, 0 == p1[tj]);
                LOOP_ASSERT(tk, 0 == p2[tk]);

                // Verify the rest parts of 'buf1' and 'buf2' are untouched.

                LOOP_ASSERT(ti,  0 == memcmp(buf1 + SZ, CTRL_BUF1 + SZ, REST));
                LOOP_ASSERT(ti,  0 == memcmp(buf2 + SZ, CTRL_BUF2 + SZ, REST));
            }
        }

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (veryVerbose) cout << "Test that the internal memory management "
                                 "system is hooked up properly." << endl;
        {
            char memory[2048];
            bdlma::BufferedSequentialAllocator a(memory, sizeof memory);

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

        bdlt::Datetime TIMESTAMP =
                            bdlt::EpochUtil::convertFromTimeT(time(0));  // now

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
