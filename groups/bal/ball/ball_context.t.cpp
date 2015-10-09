// ball_context.t.cpp                                                 -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------


#include <ball_context.h>

#include <bsls_platform.h>                      // for testing only

#include <bslma_testallocator.h>                // for testing only
#include <bslma_testallocatorexception.h>       // for testing only

#include <bdls_testutil.h>

#include <bsl_climits.h>      // INT_MAX
#include <bsl_cstdlib.h>      // atoi()
#include <bsl_cstring.h>      // strlen(), memset(), memcpy(), memcmp()

#include <bsl_new.h>          // placement 'new' syntax
#include <bsl_iostream.h>
#include <bsl_string.h>
#include <bsl_strstream.h>
#include <bsl_vector.h>

#ifdef BSLS_PLATFORM_OS_UNIX
#include <unistd.h>     // getpid()
#endif

#ifdef BSLS_PLATFORM_OS_WINDOWS
// Undefine some awkwardly named Windows macros that interfere with this cpp
// file, but only after the last #include.
# undef ERROR
#endif

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
// 'ball::Context' public interface:
// [ 2] static bool isValid(cause, index, length);
// [ 1] ball::Context(bslma::Allocator *ba = 0);
// [ 2] ball::Context(cause, index, length, *ba = 0);
// [ 1] ball::Context(const ball::Context& original, *ba = 0);
// [ 1] ~ball::Context();
// [ 1] ball::Context& operator=(const ball::Context& rhs);
// [ 2] int setAttributes(cause, index, length);
// [ 1] void setAttributesRaw(cause, index, length);
// [ 1] void setRecordIndexRaw(int index);
// [ 1] ball::Transmission::Cause transmissionCause() const;
// [ 1] int recordIndex() const;
// [ 1] int sequenceLength() const;
// [ 1] ostream& print(ostream& os, int level = 0, int spl = 4) const;
// [ 1] bool operator==(const ball::Context& lhs, const ball::Context& rhs);
// [ 1] bool operator!=(const ball::Context& lhs, const ball::Context& rhs);
// [ 1] ostream& operator<<(ostream&, const ball::Context&);
//-----------------------------------------------------------------------------
// [ 3] USAGE EXAMPLE
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
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

typedef ball::Context       Obj;

const ball::Transmission::Cause CAUSE[] = {
    ball::Transmission::e_PASSTHROUGH,
    ball::Transmission::e_TRIGGER,
    ball::Transmission::e_TRIGGER_ALL,
    ball::Transmission::e_MANUAL_PUBLISH,
    ball::Transmission::e_MANUAL_PUBLISH_ALL
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
    void publish(const bsl::string& message, const ball::Context& context);

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
void my_Logger::publish(const bsl::string&   message,
                        const ball::Context& context)
{
    switch (context.transmissionCause()) {
      case ball::Transmission::e_PASSTHROUGH: {
        d_os << "Single Pass-through Message: ";
      } break;
      case ball::Transmission::e_TRIGGER_ALL: {
        d_os << "Remotely ";               // no 'break'; concatenated output
      } break;
      case ball::Transmission::e_TRIGGER: {
        d_os << "Triggered Publication Sequence: Message "
             << context.recordIndex() + 1  // Account for zero-based index.
             << " of " << context.sequenceLength() << ": ";
      } break;
      case ball::Transmission::e_MANUAL_PUBLISH: {
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
        ball::Context context(ball::Transmission::e_PASSTHROUGH, 0, 1);
        publish(message, context);
      } break;
      case ERROR: {
        int index  = 0;
        int length = (int)archive.size();
        ball::Context context(ball::Transmission::e_TRIGGER, index, length);
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

    bslma::TestAllocator testAllocator(veryVeryVerbose);

    switch (test) { case 0:  // Zero is always the leading case.
      case 3: {
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

        char buf[1024];  memset(buf, 0xff, sizeof buf);  // Scribble on buffer
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

      case 2: {
        // --------------------------------------------------------------------
        // Constraints Test:
        //
        // Testing:
        //   static bool isValid(cause, index, length);
        //   int setAttributes(cause, index, length);
        //   ball::Context(cause, index, length, *ba = 0);
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "Constraints Test" << endl
                                  << "================" << endl;

        ASSERT(NUM_INDEX == NUM_LENGTH);

        if (veryVerbose) cout << "\n\tPASSTHROUGH." << endl;
        if (veryVerbose) cout << "\t\tValid attributes." << endl;
        {
            ASSERT(1 == Obj::isValid(CAUSE[0], INDEX[0], LENGTH[0]));

            Obj mX(CAUSE[0], INDEX[0], LENGTH[0]);  const Obj& X = mX;
            if (veryVeryVerbose) { T_; T_; P(X); }
            ASSERT( CAUSE[0] == X.transmissionCause());
            ASSERT( INDEX[0] == X.recordIndex());
            ASSERT(LENGTH[0] == X.sequenceLength());

            Obj mY;  const Obj& Y = mY;
            mY.setAttributesRaw(CAUSE[1], INDEX[1], LENGTH[1]);
            ASSERT( CAUSE[0] != Y.transmissionCause());
            ASSERT( INDEX[0] != Y.recordIndex());
            ASSERT(LENGTH[0] != Y.sequenceLength());
            ASSERT(0 == mY.setAttributes(CAUSE[0], INDEX[0], LENGTH[0]));
            if (veryVeryVerbose) { T_; T_; P(Y); }
            ASSERT( CAUSE[0] == Y.transmissionCause());
            ASSERT( INDEX[0] == Y.recordIndex());
            ASSERT(LENGTH[0] == Y.sequenceLength());
        }
        if (veryVerbose) cout << "\t\tInvalid attributes." << endl;
        {
            const ball::Transmission::Cause C = CAUSE[0];
            for (int tlen = 0; tlen < NUM_LENGTH; ++tlen) {
                const int L = LENGTH[tlen];
                for (int tind = 0; tind < NUM_INDEX; ++tind) {
                    const int I = INDEX[tind];
                    // Lone valid combination.
                    if (0 == tlen && 0 == tind) continue;
                    if (veryVeryVerbose) { T_; T_; P_(C); P_(I); P(L); }
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
                const ball::Transmission::Cause C = CAUSE[tc];
                for (int tlen = 0; tlen < NUM_LENGTH; ++tlen) {
                    const int L = LENGTH[tlen];
                    for (int tind = 0; tind <= tlen; ++tind) {
                        const int I = INDEX[tind];
                        if (veryVeryVerbose) {
                            T_; T_; P_(C); P_(I); P(L);
                        }
                        ASSERT(1 == Obj::isValid(C, I, L));

                        Obj mX(C, I, L);  const Obj& X = mX;
                        if (veryVeryVerbose) { T_; T_; P(X); }
                        ASSERT(C == X.transmissionCause());
                        ASSERT(I == X.recordIndex());
                        ASSERT(L == X.sequenceLength());

                        Obj mY;  const Obj& Y = mY;
                        mY.setAttributesRaw(CAUSE[0], INDEX[0], LENGTH[0]);
                        ASSERT( CAUSE[0] == Y.transmissionCause());
                        ASSERT( INDEX[0] == Y.recordIndex());
                        ASSERT(LENGTH[0] == Y.sequenceLength());
                        ASSERT(0 == mY.setAttributes(C, I, L));
                        if (veryVeryVerbose) { T_; T_; P(Y); }
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
                const ball::Transmission::Cause C = CAUSE[tc];
                for (int tlen = 0; tlen < NUM_LENGTH; ++tlen) {
                    const int L = LENGTH[tlen];
                    for (int tind = tlen + 1; tind < NUM_INDEX; ++tind) {
                        const int I = INDEX[tind];
                        if (veryVeryVerbose) {
                            T_; T_; P_(C); P_(I); P(L);
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
      case 1: {
        // --------------------------------------------------------------------
        // Basic Attribute Test:
        //
        // Testing:
        //   ball::Context(bslma::Allocator *ba = 0);
        //   ball::Context(const ball::Context& original, *ba = 0);
        //   ~ball::Context();
        //   ball::Context& operator=(const ball::Context& rhs);
        //   void setAttributesRaw(cause, index, length);
        //   void setRecordIndexRaw(int index);
        //   ball::Transmission::Cause transmissionCause() const;
        //   int recordIndex() const;
        //   int sequenceLength() const;
        //   ostream& print(ostream& os, int level = 0, int spl = 4) const;
        //   bool operator==(const Context& lhs, const Context& rhs);
        //   bool operator!=(const Context& lhs, const Context& rhs);
        //   ostream& operator<<(ostream& os, const ball::Context&);
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
