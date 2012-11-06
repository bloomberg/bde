// baejsn_printutil.t.cpp                                             -*-C++-*-
#include <baejsn_printutil.h>

#include <bsl_sstream.h>
#include <bsl_string.h>

#include <bdet_time.h>
#include <bdet_timetz.h>
#include <bdet_date.h>
#include <bdet_datetz.h>
#include <bdet_datetime.h>
#include <bdet_datetimetz.h>

#include <bsl_iostream.h>

using namespace BloombergLP;
using bsl::cout;
using bsl::cerr;
using bsl::endl;

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

#define LOOP0_ASSERT ASSERT

// ============================================================================
//                  STANDARD BDE LOOP-ASSERT TEST MACROS
// ----------------------------------------------------------------------------

#define LOOP_ASSERT(I,X) {                                                    \
    if (!(X)) { cout << #I << ": " << I << "\n"; aSsErT(1, #X, __LINE__);}}

#define LOOP1_ASSERT LOOP_ASSERT

#define LOOP2_ASSERT(I,J,X) {                                                 \
    if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": "                 \
              << J << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP3_ASSERT(I,J,K,X) {                                               \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t"     \
              << #K << ": " << K << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP4_ASSERT(I,J,K,L,X) {                                             \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" <<  \
       #K << ": " << K << "\t" << #L << ": " << L << "\n";                    \
       aSsErT(1, #X, __LINE__); } }

#define LOOP5_ASSERT(I,J,K,L,M,X) {                                           \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" <<  \
       #K << ": " << K << "\t" << #L << ": " << L << "\t" <<                  \
       #M << ": " << M << "\n";                                               \
       aSsErT(1, #X, __LINE__); } }

// ============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
// ----------------------------------------------------------------------------

#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", " << flush; // 'P(X)' without '\n'
#define T_ cout << "\t" << flush;             // Print tab w/o newline.
#define L_ __LINE__                           // current Line number

// The 'BSLS_BSLTESTUTIL_EXPAND' macro is required to workaround a
// pre-proccessor issue on windows that prevents __VA_ARGS__ to be expanded in
// the definition of 'BSLS_BSLTESTUTIL_NUM_ARGS'
#define EXPAND(X)                                            \
    X

#define NUM_ARGS_IMPL(X5, X4, X3, X2, X1, X0, N, ...)        \
    N

#define NUM_ARGS(...)                                        \
    EXPAND(NUM_ARGS_IMPL( __VA_ARGS__, 5, 4, 3, 2, 1, 0, ""))

#define LOOPN_ASSERT_IMPL(N, ...)                            \
    EXPAND(LOOP ## N ## _ASSERT(__VA_ARGS__))

#define LOOPN_ASSERT(N, ...)                                 \
    LOOPN_ASSERT_IMPL(N, __VA_ARGS__)

#define ASSERTV(...)                                         \
    LOOPN_ASSERT(NUM_ARGS(__VA_ARGS__), __VA_ARGS__)

// ============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------

typedef baejsn_PrintUtil Obj;

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    bool verbose = argc > 2;
    //veryVerbose = argc > 3;
    bool veryVeryVerbose = argc > 4;
    //veryVeryVeryVerbose = argc > 5;
    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:
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
        //: 1 Call 'printValue' on each value type.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------
        bsl::ostringstream oss;

        if (verbose) cout << "Test boolean" << endl;
        {
            Obj::printValue(oss, true);
            ASSERTV("true" == oss.str());
            oss.str("");

            Obj::printValue(oss, false);
            ASSERTV("false" == oss.str());
            oss.str("");
        }

        if (verbose) cout << "Test unsigned integers" << endl;
        {
            Obj::printValue(oss, (unsigned char) 1);
            ASSERTV("1" == oss.str());
            oss.str("");

            Obj::printValue(oss, (unsigned short) 2);
            ASSERTV("2" == oss.str());
            oss.str("");

            Obj::printValue(oss, (unsigned int) 3);
            ASSERTV("3" == oss.str());
            oss.str("");

            Obj::printValue(oss, (bsls::Types::Uint64) 4);
            ASSERTV("4" == oss.str());
            oss.str("");
        }

        if (verbose) cout << "Test signed integers" << endl;
        {
            Obj::printValue(oss, (short) -2);
            ASSERTV("-2" == oss.str());
            oss.str("");

            Obj::printValue(oss, (int) -3);
            ASSERTV("-3" == oss.str());
            oss.str("");

            Obj::printValue(oss, (bsls::Types::Int64) -4);
            ASSERTV("-4" == oss.str());
            oss.str("");
        }

        if (verbose) cout << "Test decimal" << endl;
        {
            Obj::printValue(oss, 3.14159f);
            ASSERTV("3.14159" == oss.str());
            oss.str("");

            Obj::printValue(oss, 3.1415926535);
            ASSERTV("3.1415926535" == oss.str());
            oss.str("");
        }

        if (verbose) cout << "Test string" << endl;
        {
            Obj::printValue(oss, "Hello");
            ASSERTV("\"Hello\"" == oss.str());
            oss.str("");

            Obj::printValue(oss, bsl::string("World"));
            ASSERTV("\"World\"" == oss.str());
            oss.str("");

            Obj::printValue(oss, '!');
            ASSERTV("\"!\"" == oss.str());
            oss.str("");
        }

        if (verbose) cout << "Test date/time" << endl;
        {
            const int YEAR        = 9999;
            const int MONTH       = 12;
            const int DAY         = 31;
            const int HOUR        = 23;
            const int MINUTE      = 59;
            const int SECOND      = 59;
            const int MILLISECOND = 999;
            const int OFFSET      = -720;

            bdet_Date theDate(YEAR, MONTH, DAY);
            bdet_Time theTime(HOUR, MINUTE, SECOND, MILLISECOND);
            bdet_Datetime theDatetime(YEAR, MONTH, DAY,
                                      HOUR, MINUTE, SECOND, MILLISECOND);

            bdet_DateTz     theDateTz(theDate, OFFSET);
            bdet_TimeTz     theTimeTz(theTime, OFFSET);
            bdet_DatetimeTz theDatetimeTz(theDatetime, OFFSET);

            Obj::printValue(oss, theDate);
            ASSERTV(oss.str(), "\"9999-12-31\"" == oss.str());
            oss.str("");

            Obj::printValue(oss, theDateTz);
            ASSERTV(oss.str(),"\"9999-12-31-12:00\"" == oss.str());
            oss.str("");

            Obj::printValue(oss, theTime);
            ASSERTV(oss.str(),"\"23:59:59.999\"" == oss.str());
            oss.str("");

            Obj::printValue(oss, theTimeTz);
            ASSERTV(oss.str(),"\"23:59:59.999-12:00\"" == oss.str());
            oss.str("");

            Obj::printValue(oss, theDatetime);
            ASSERTV(oss.str(),"\"9999-12-31T23:59:59.999\"" == oss.str());
            oss.str("");

            Obj::printValue(oss, theDatetimeTz);
            ASSERTV(oss.str(),"\"9999-12-31T23:59:59.999-12:00\"" == oss.str());
            oss.str("");

            Obj::printValue(oss, theDatetimeTz);
            ASSERTV(oss.str(),"\"9999-12-31T23:59:59.999-12:00\"" == oss.str());
            oss.str("");
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
    return -1;
}
