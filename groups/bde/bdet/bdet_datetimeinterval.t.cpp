// bdet_datetimeinterval.t.cpp -*-C++-*-

#include <bdet_datetimeinterval.h>

#include <bdex_byteinstream.h>           // for testing only
#include <bdex_byteoutstream.h>          // for testing only
#include <bdex_instreamfunctions.h>      // for testing only
#include <bdex_outstreamfunctions.h>     // for testing only
#include <bdex_testinstream.h>           // for testing only
#include <bdex_testinstreamexception.h>  // for testing only
#include <bdex_testoutstream.h>          // for testing only

#include <bsls_platformutil.h>           // for testing only

#include <bsl_cstdlib.h>     // atoi()
#include <bsl_cstring.h>     // strcmp()
#include <bsl_iostream.h>
#include <bsl_strstream.h>

using namespace BloombergLP;
using namespace bsl;  // automatically added by script


// ============================================================================
//                                   TEST PLAN
// ----------------------------------------------------------------------------
//                                   Overview
//                                   --------
//
// ----------------------------------------------------------------------------
// PRIVATE CREATORS
// [ *] bdet_DatetimeInterval(bsls_PlatformUtil::Int64 milliseconds)
// CREATORS
// [ 2] bdet_DatetimeInterval()
// [11] bdet_DatetimeInterval(int days, int hrs, int mins, int secs, int msecs)
// [ 7] bdet_DatetimeInterval(const bdet_DatetimeInterval& original)
// [ 2] ~bdet_DatetimeInterval()
// MANIPULATORS
// [ 9] bdet_DatetimeInterval& operator=(const bdet_DatetimeInterval& rhs)
// [15] bdet_DatetimeInterval& operator+=(const bdet_DatetimeInterval& rhs)
// [15] bdet_DatetimeInterval& operator-=(const bdet_DatetimeInterval& rhs)
// [ 2] void setInterval(int days, int hrs, int mins, int secs, int msecs)
// [12] void setTotalDays(int days)
// [12] void setTotalHours(int hours)
// [12] void setTotalMinutes(int minutes)
// [12] void setTotalSeconds(int seconds)
// [12] void setTotalMilliseconds(int milliseconds)
// [14] void addInterval(int days, int hrs, int mins, int secs, int msecs)
// [14] void addDays(int days)
// [14] void addHours(int hours)
// [14] void addMinutes(int minutes)
// [14] void addSeconds(int seconds)
// [14] void addMilliseconds(int milliseconds)
// ACCESSORS
// [ 4] int days() const
// [ 4] int hours() const
// [ 4] int minutes() const
// [ 4] int seconds() const
// [ 4] int milliseconds() const
// [ 4] int totalDays() const
// [ 4] int totalHours() const
// [ 4] int totalMinutes() const
// [ 4] int totalSeconds() const
// [ 4] int totalMilliseconds() const
// [ 4] double totalSecondsAsDouble() const
// [10] int maxSupportedBdexVersion() const;
// FREE OPERATORS
// [16] bdet_DtI operator+(const bdet_DtI&, const bdet_DtI&)
// [16] bdet_DtI operator-(const bdet_DtI&, const bdet_DtI&)
// [17] bdet_DatetimeInterval operator-(const bdet_DatetimeInterval& rhs)
// [ 6] bool operator==(const bdet_DtI&, const bdet_DtI&)
// [ 6] bool operator!=(const bdet_DtI&, const bdet_DtI&)
// [13] bool operator< (const bdet_DtI&, const bdet_DtI&)
// [13] bool operator<=(const bdet_DtI&, const bdet_DtI&)
// [13] bool operator> (const bdet_DtI&, const bdet_DtI&)
// [13] bool operator>=(const bdet_DtI&, const bdet_DtI&)
// [ 5] ostream& operator<<(ostream &output, const bdet_DtI&)
// ----------------------------------------------------------------------------
// [ 1] breathing test
// [18] USAGE example
//=============================================================================
//                  STANDARD BDE ASSERT TEST MACROS
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
   if (!(X)) { cout << #I << ": " << I << "\t"                             \
                    << #J << ": " << J << "\t" << #K << ": " << K << "\t"  \
                    << #L << ": " << L << "\t" << #M << ": " << M << "\n"; \
       aSsErT(1, #X, __LINE__); } }

//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", "<< flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number
#define T_()  cout << '\t' << flush;          // Print tab w/o newline
#define NL()  cout << endl;                   // Print newline
#define P64(X) printf(#X " = %lld\n", (X));   // Print 64-bit integer id & val
#define P64_(X) printf(#X " = %lld,  ", (X)); // Print 64-bit integer w/o '\n'

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

typedef bdet_DatetimeInterval    Obj;
typedef bdex_TestInStream        In;
typedef bdex_TestOutStream       Out;
typedef bsls_PlatformUtil::Int64 Int64;

//=============================================================================
//                      HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------
static
Int64 toMsec(int d, int h = 0, int m = 0, int s = 0, int ms = 0)
    // Return the sum of the specified 'd' days, 'h' hours, 'm' minutes, 's'
    // seconds, and 'ms' milliseconds, expressed as total milliseconds.
{
    const Int64 S_TO_MS = 1000;
    const Int64 M_TO_MS = S_TO_MS * 60;
    const Int64 H_TO_MS = M_TO_MS * 60;
    const Int64 D_TO_MS = H_TO_MS * 24;

    Int64 totalMsecs = ms;         // accumulate milliseconds

    totalMsecs += s * S_TO_MS;     // convert and accumulate seconds
    totalMsecs += m * M_TO_MS;     // convert and accumulate minutes
    totalMsecs += h * H_TO_MS;     // convert and accumulate hours
    totalMsecs += d * D_TO_MS;     // convert and accumulate days
    return totalMsecs;             // return the result
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

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:
      case 18: {
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
        {
            bdet_DatetimeInterval ti1;       ASSERT(  0 == ti1.days());
                                             ASSERT(  0 == ti1.hours());
                                             ASSERT(  0 == ti1.minutes());
                                             ASSERT(  0 == ti1.seconds());
                                             ASSERT(  0 == ti1.milliseconds());

            ti1.setTotalDays(-5);
            ti1.addHours(16);                ASSERT( -4 == ti1.days());
                                             ASSERT( -8 == ti1.hours());
                                             ASSERT(  0 == ti1.minutes());
                                             ASSERT(  0 == ti1.seconds());
                                             ASSERT(  0 == ti1.milliseconds());

            bdet_DatetimeInterval ti2(ti1);  ASSERT( -4 == ti2.days());
                                             ASSERT( -8 == ti2.hours());
                                             ASSERT(  0 == ti2.minutes());
                                             ASSERT(  0 == ti2.seconds());
                                             ASSERT(  0 == ti2.milliseconds());

            ti2.addDays(2);
            ti2.addSeconds(4);               ASSERT( -2 == ti2.days());
                                             ASSERT( -7 == ti2.hours());
                                             ASSERT(-59 == ti2.minutes());
                                             ASSERT(-56 == ti2.seconds());
                                             ASSERT(  0 == ti2.milliseconds());
            ASSERT(ti2 > ti1);

            ti1.addInterval(2, 0, 0, 4);
            ASSERT(ti2 == ti1);
            if (verbose) cout << ti2 << endl;
        }

      } break;
      case 17: {
        // --------------------------------------------------------------------
        // TESTING UNARY MINUS OPERATOR:
        //   Unary 'operator-' is a 'friend' of'bdet_DatetimeInterval' and
        //   so is very simple to implement.  Moreover, the fundamental unary
        //   - on Int64 is a ready oracle.  We need only a few vectors to test
        //   that unary 'operator-' performs as advertised.
        //
        //
        // Plan:
        //   In a double loop, generate several lhs values x and several
        //   rhs values y.  Calculate the expected result z of x + y
        //   from the loop variables and confirm that z == x.  Similarly verify
        //   the result of x - y.
        //
        // Testing
        //   bdet_DatetimeInterval operator-(const Obj& rhs)
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Unary 'operator-'"
                          << "\n=========================" << endl;

        {
            if (verbose)
                cout << "\nTesting: Unary 'operator-'" << endl;

            const int START = -1000;
            const int STOP  =  1000;
            const int STEP  =    99;
            for (int rhsMs = START; rhsMs <= STOP; rhsMs += STEP) {
                Obj x;  const Obj &X = x;
                x.setTotalMilliseconds(rhsMs);
                Obj y;  const Obj& Y = y;
                y.setTotalMilliseconds(-rhsMs);

                if (veryVerbose) { T_();  P(rhsMs); }
                if (veryVerbose) { T_();  P_(X);  P_(Y); }

                LOOP_ASSERT(rhsMs, Y == -X);
            }
        }

      } break;
      case 16: {
        // --------------------------------------------------------------------
        // TESTING ARITHMETIC FREE OPERATORS:
        //   The arithmetic free operators are 'friend's of
        //   'bdet_DatetimeInterval' and so are very similar in implementation
        //   and concerns to the member arithmetic assignment operators.  The
        //   test will thus be very similar as well, differing only in the
        //   details of object manipulation, but not at all in concept.
        //
        // Plan:
        //   In a double loop, generate several lhs values x and several
        //   rhs values y.  Calculate the expected result z of x + y
        //   from the loop variables and confirm that z == x.  Similarly verify
        //   the result of x - y.
        //
        // Testing
        //   bdet_DatetimeInterval operator+(const Obj& rhs, const Obj& lhs)
        //   bdet_DatetimeInterval operator-(const Obj& rhs, const Obj& lhs)
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Arithmetic Free Operators"
                          << "\n=================================" << endl;

        Obj x, y, z, exp;  const Obj &X = x, &Y = y, &Z = z, &EXP = exp;

        {
            if (verbose)
                cout << "\nTesting: 'operator+' and 'operator-'" << endl;

            const int START = -1;
            const int STOP  =  1;
            for (int lhsMs = START; lhsMs <= STOP; ++lhsMs)
            for (int rhsMs = START; rhsMs <= STOP; ++rhsMs) {

                x.setTotalMilliseconds(lhsMs);
                y.setTotalMilliseconds(rhsMs);

                if (veryVerbose) { T_();  P_(lhsMs);  P(rhsMs); }
                if (veryVerbose) { T_();  P_(X);  P_(Y); }
                z = x + y;  exp.setTotalMilliseconds(lhsMs + rhsMs);
                if (veryVerbose) { P_(Z);  P_(EXP); }
                LOOP2_ASSERT(lhsMs, rhsMs, EXP == Z);

                z = x - y;  exp.setTotalMilliseconds(lhsMs - rhsMs);
                if (veryVerbose) { P_(Z);  P(EXP); }
                LOOP2_ASSERT(lhsMs, rhsMs, EXP == Z);
            }
        }

      } break;
      case 15: {
        // --------------------------------------------------------------------
        // TESTING ARITHMETIC ASSIGNMENT METHODS:
        //   The arithmetic assignment operators modify the single integer
        //   total-milliseconds data member; no details of the *representation*
        //   of either operand's value are significant.  The only concern is
        //   that the correct operator is applied to the underlying integers.
        //   A small number of simple test vectors is adequate to confirm this.
        //
        // Plan:
        //   In a double loop, generate several initial values x and several
        //   rhs operand values y.  Calculate the expected result z of x += y
        //   from the loop variables and confirm that z == x.  Then verify that
        //   the result of x -= y is the original value of x.
        //
        // Testing
        //   bdet_DtI& operator+=(const bdet_DtI& rhs)
        //   bdet_DtI& operator-=(const bdet_DtI& rhs)
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Arithmetic Assignment Operators"
                          << "\n======================================="
                          << endl;

        Obj x, y, z;  const Obj &X = x, &Y = y, &Z = z;

        {
            if (verbose)
                cout << "\nTesting: 'operator+=' and 'operator-='" << endl;

            const int START = -1;
            const int STOP  =  1;
            for (int initMs = START; initMs <= STOP; ++initMs)
            for (int rhsMs  = START; rhsMs  <= STOP; ++rhsMs) {

                x.setTotalMilliseconds(initMs);
                y.setTotalMilliseconds(rhsMs);

                if (veryVerbose) { T_();  P_(initMs);  P(rhsMs); }
                if (veryVerbose) { T_();  P_(X);  P_(Y); }
                x += y;  z.setTotalMilliseconds(initMs + rhsMs);
                if (veryVerbose) { P_(X);  P_(Z); }
                LOOP2_ASSERT(initMs, rhsMs, Z == X);

                x -= y;  z.setTotalMilliseconds(initMs);
                if (veryVerbose) { P_(X);  P(Z); }
                LOOP2_ASSERT(initMs, rhsMs, Z == X);
            }
        }

      } break;
      case 14: {
        // --------------------------------------------------------------------
        // TESTING 'add' METHODS:
        //   Each 'add' method multiplies it argument(s) by appropriate factors
        //   and adds the product(s) to the underlying total-milliseconds
        //   representation of the object's value.  Our concerns are limited to
        //   correct arithmetic operations, correct numerical constants
        //   (conversion factors), and correct precision-preservation
        //   (conversion to 64-bit representation).  It is sufficient to do the
        //   majority of the testing using an initial time-interval value of 0,
        //   and to confirm with a very few tests that the operation is indeed
        //   an "add" and not a "set".
        //
        // Plan:
        //   First test 'addInterval' explicitly and then use the tested
        //   'addInterval' as an oracle to test the other four methods in
        //   a loop-based test.
        //
        //   Select an arbitrary non-zero initial time-interval value X, and
        //   tabulate a set of input (operand) values and the corresponding
        //   expected object values.  Verify that 'addInterval' produces the
        //   expected result.  Then, in a five-deep nested loop, test
        //   'addInterval' over a wide dynamic range, but on an initial object
        //   value of 0 (so that the tested 'setInterval' may be used to
        //   confirm success).
        //
        //   In a loop, generate arguments for each of the one-argument 'add'
        //   methods and use the (tested) 'addInterval' method as an oracle
        //   (with all zero arguments, except for the one corresponding to the
        //   'add' method under test).
        //
        // Testing
        //   void addInterval(int days, int hrs, int mins, int secs,int msecs);
        //   void addDays(int days);
        //   void addHours(int hours);
        //   void addMinutes(int minutes);
        //   void addSeconds(int seconds);
        //   void addMilliseconds(int milliseconds);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting 'add' Methods"
                          << "\n=====================" << endl;

        const int REF_DAY  = 1;
        const int REF_HR   = 1;
        const int REF_MIN  = 1;
        const int REF_SEC  = 1;
        const int REF_MSEC = 1;

        const Obj REF(REF_DAY, REF_HR, REF_MIN, REF_SEC, REF_MSEC);

        {
            static const struct {
                int d_lineNum;       // source line number
                int d_days;          // days to add
                int d_hours;         // hours to add
                int d_minutes;       // minutes to add
                int d_seconds;       // seconds to add
                int d_msecs;         // milliseconds to add
                int d_expDays;       // expected days value
                int d_expHours;      // expected hours value
                int d_expMinutes;    // expected minutes value
                int d_expSeconds;    // expected seconds value
                int d_expMsecs;      // expected milliseconds value
            } DATA[] = {
                //        - - time added - - -     ---expected values---
                //line #   d   h   m   s   ms     days  h   m   s   ms
                //------   --  --  --  --  ---    ----  --  --  --  ---
                { L_,       0,  0,  0,  0,   0,      1,  1,  1,  1,   1 },

                { L_,       0,  0,  0,  0,   1,      1,  1,  1,  1,   2 },
                { L_,       0,  0,  0,  1,   0,      1,  1,  1,  2,   1 },
                { L_,       0,  0,  1,  0,   0,      1,  1,  2,  1,   1 },
                { L_,       0,  1,  0,  0,   0,      1,  2,  1,  1,   1 },
                { L_,       1,  0,  0,  0,   0,      2,  1,  1,  1,   1 },

                { L_,       0,  0,  0,  0,  -1,      1,  1,  1,  1,   0 },
                { L_,       0,  0,  0, -1,   0,      1,  1,  1,  0,   1 },
                { L_,       0,  0, -1,  0,   0,      1,  1,  0,  1,   1 },
                { L_,       0, -1,  0,  0,   0,      1,  0,  1,  1,   1 },
                { L_,      -1,  0,  0,  0,   0,      0,  1,  1,  1,   1 },

                { L_,       1,  1,  1,  1,   1,      2,  2,  2,  2,   2 },
                { L_,      -1, -1, -1, -1,  -1,      0,  0,  0,  0,   0 },

            };

            if (verbose) cout << "\nTesting: 'addInterval'" << endl;
            if (verbose) cout << "\n\twith non-zero initial value" << endl;
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int LINE  = DATA[i].d_lineNum;
                const int DAYS  = DATA[i].d_days;
                const int HRS   = DATA[i].d_hours;
                const int MINS  = DATA[i].d_minutes;
                const int SECS  = DATA[i].d_seconds;
                const int MSECS = DATA[i].d_msecs;

                const int EXP_DAYS  = DATA[i].d_expDays;
                const int EXP_HRS   = DATA[i].d_expHours;
                const int EXP_MINS  = DATA[i].d_expMinutes;
                const int EXP_SECS  = DATA[i].d_expSeconds;
                const int EXP_MSECS = DATA[i].d_expMsecs;

                Obj x(REF);  const Obj& X = x;
                if (veryVerbose) { T_();  P_(X); }
                x.addInterval(DAYS, HRS, MINS, SECS, MSECS);
                const Obj Y(EXP_DAYS, EXP_HRS, EXP_MINS, EXP_SECS, EXP_MSECS);
                if (veryVerbose) { P_(X);  P(Y); }
                LOOP_ASSERT(LINE, Y == X);
            }
        }

        {
            const int STOP  = 1000 * 1000 * 1000;
            const int START = -STOP;
            const int STEP  = STOP;

            if (verbose) cout << "\n\twith 0 initial value, in a loop" << endl;
            for (int m = START; m <= STOP ; m += STEP)
            for (int l = START; l <= STOP ; l += STEP)
            for (int k = START; k <= STOP ; k += STEP)
            for (int j = START; j <= STOP ; j += STEP)
            for (int i = START; i <= STOP ; i += STEP) {
                Obj x;  const Obj &X = x;
                if (veryVerbose) { T_();  P_(X); }

                x.addInterval(i, j, k, l, m);
                const Obj Y(i, j, k, l, m);
                if (veryVerbose) { P_(X);  P_(Y);  P(i); }

                LOOP5_ASSERT(i, j, k, l, m, Y == X);
            }
        }

        {
            const int STOP  = 1000 * 1000 * 1000;
            const int START = -STOP;
            const int STEP  = STOP / 10;
            int arg;

            if (verbose) cout << "\nTesting 'addDays'" << endl;
            for (arg = START; arg <= STOP ; arg += STEP) {
                Obj x(REF), y(REF);  const Obj &X = x, &Y = y;
                if (veryVerbose) { T_();  P_(X); }

                x.addDays(arg);
                y.addInterval(arg, 0, 0, 0, 0);
                if (veryVerbose) { P_(X);  P_(Y);  P(arg); }

                LOOP_ASSERT(arg, Y == X);
            }

            if (verbose) cout << "\nTesting 'addHours'" << endl;
            for (arg = START; arg <= STOP ; arg += STEP) {
                Obj x(REF), y(REF);  const Obj &X = x, &Y = y;
                if (veryVerbose) { T_();  P_(X); }

                x.addHours(arg);
                y.addInterval(0, arg, 0, 0, 0);
                if (veryVerbose) { P_(X);  P_(Y);  P(arg); }

                LOOP_ASSERT(arg, Y == X);
            }

            if (verbose) cout << "\nTesting: 'addMinutes'" << endl;
            for (arg = START; arg <= STOP ; arg += STEP) {
                Obj x(REF), y(REF);  const Obj &X = x, &Y = y;
                if (veryVerbose) { T_();  P_(X); }

                x.addMinutes(arg);
                y.addInterval(0, 0, arg, 0, 0);
                if (veryVerbose) { P_(X);  P_(Y);  P(arg); }

                LOOP_ASSERT(arg, Y == X);
            }

            if (verbose) cout << "\nTesting: 'addSeconds'" << endl;
            for (arg = START; arg <= STOP ; arg += STEP) {
                Obj x(REF), y(REF);  const Obj &X = x, &Y = y;
                if (veryVerbose) { T_();  P_(X); }

                x.addSeconds(arg);
                y.addInterval(0, 0, 0, arg, 0);
                if (veryVerbose) { P_(X);  P_(Y);  P(arg); }

                LOOP_ASSERT(arg, Y == X);
            }

            if (verbose) cout << "\nTesting: 'addMilliseconds'" << endl;
            for (arg = START; arg <= STOP ; arg += STEP) {
                Obj x(REF), y(REF);  const Obj &X = x, &Y = y;
                if (veryVerbose) { T_();  P_(X); }

                x.addMilliseconds(arg);
                y.addInterval(0, 0, 0, 0, arg);
                if (veryVerbose) { P_(X);  P_(Y);  P(arg); }

                LOOP_ASSERT(arg, Y == X);
            }
        }

      } break;
      case 13: {
        // --------------------------------------------------------------------
        // TESTING RELATIONAL OPERATORS (<, <=, >=, >):
        //   Each operator implements a logical expression on the contained
        //   'bdet_Date' and 'bdet_Time' values, which must be verified.
        //
        // Plan:
        //   Specify an ordered set S of unique object values.  For each (u, v)
        //   in the set S x S, verify the result of u OP v for each OP in
        //   {<, <=, >=, >}.
        //
        // Testing
        //   bool operator< (const bdet_DtI&, const bdet_DtI&)
        //   bool operator<=(const bdet_DtI&, const bdet_DtI&)
        //   bool operator>=(const bdet_DtI&, const bdet_DtI&)
        //   bool operator> (const bdet_DtI&, const bdet_DtI&)
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Relational Operators"
                          << "\n============================" << endl;

        if (verbose) cout <<
            "\nTesting: 'operator<', 'operator<=', 'operator>=', 'operator>'"
                          << endl;
        {
            static const struct {
                int d_days;
                int d_hours;
                int d_minutes;
                int d_seconds;
                int d_msecs;
            } VALUES[] = {
                { -1001,    0,   0,   0,    0 },
                { -1000,  -23, -59, -59, -999 },
                { -1000,  -23, -59, -59, -998 },
                {   -10,   25,  80,  70,   -1 },
                {   -10,   25,  80,  70,    0 },
                {   -10,   25,  80,  70,    1 },
                {    -1,    0,  -1,   0,    0 },
                {    -1,    0,   0,   0,    0 },
                {    -1,    0,   1,   0,    0 },
                {     0,    0,   0,   0,    0 },
                {     1,    0,  -1,   0,    0 },
                {     1,    0,   0,   0,    0 },
                {     1,    0,   1,   0,    0 },
                {    10,   25,  80,  70,   -1 },
                {    10,   25,  80,  70,    0 },
                {    10,   25,  80,  70,    1 },
                {  1000,   23,  59,  59,  998 },
                {  1000,   23,  59,  59,  999 },
                {  1001,    0,   0,   0,    0 },
             };

            const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;

            for (int i = 0; i < NUM_VALUES; ++i) {
                Obj v;  const Obj& V = v;
                v.setInterval(VALUES[i].d_days,    VALUES[i].d_hours,
                              VALUES[i].d_minutes, VALUES[i].d_seconds,
                              VALUES[i].d_msecs);

                for (int j = 0; j < NUM_VALUES; ++j) {
                    Obj u;  const Obj& U = u;
                    u.setInterval(VALUES[j].d_days,    VALUES[j].d_hours,
                                  VALUES[j].d_minutes, VALUES[j].d_seconds,
                                  VALUES[j].d_msecs);
                    if (veryVerbose) { T_();  P_(i);  P_(j);  P_(V);  P(U); }
                    LOOP2_ASSERT(i, j, j <  i == U <  V);
                    LOOP2_ASSERT(i, j, j <= i == U <= V);
                    LOOP2_ASSERT(i, j, j >= i == U >= V);
                    LOOP2_ASSERT(i, j, j >  i == U >  V);
                }
            }
        }

      } break;
      case 12: {
        // --------------------------------------------------------------------
        // TESTING ADDITIONAL SETTING MANIPULATORS:
        //   Each method must compute the underlying total-milliseconds
        //   representation correctly.  The concerns are that the value is
        //   reset (rather than augmented), and that the conversion to 64-bit
        //   integer occurs correctly.
        //
        // Plan:
        //   Create a (single) default object X to be subsequently modified by
        //   each method under test.  In a loop, generate a sequence of test
        //   argument values and use each argument with each 'set' method to
        //   *reset* the value of X.  Verify the value of X using the helper
        //   function 'toMsec' and the accessor 'totalMilliseconds'.
        //
        // Testing:
        //   void setTotalDays(int days)
        //   void setTotalHours(int hours)
        //   void setTotalMinutes(int minutes)
        //   void setTotalSeconds(int seconds)
        //   void setTotalMilliseconds(int milliseconds)
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING 'set' MANIPULATORS:"
                          << "\n===========================" << endl;

        if (verbose) cout << "\nTesting 'setTotalXXX' methods." << endl;
        {
            Obj x;  const Obj& X = x;

            const int STOP  = 1000 * 1000 * 1000;
            const int START = -STOP;
            const int STEP  = STOP / 10;

            for (int arg = START; arg <= STOP ; arg += STEP) {

                x.setTotalDays(arg);
                if (veryVerbose) { T_(); P_(arg); P(X); }
                LOOP_ASSERT(arg, toMsec(arg, 0, 0, 0, 0)
                                 == X.totalMilliseconds());

                x.setTotalHours(arg);
                if (veryVerbose) { T_(); P_(arg); P(X); }
                LOOP_ASSERT(arg, toMsec(0, arg, 0, 0, 0)
                                 == X.totalMilliseconds());

                x.setTotalMinutes(arg);
                if (veryVerbose) { T_(); P_(arg); P(X); }
                LOOP_ASSERT(arg, toMsec(0, 0, arg, 0, 0)
                                 == X.totalMilliseconds());

                x.setTotalSeconds(arg);
                if (veryVerbose) { T_(); P_(arg); P(X); }
                LOOP_ASSERT(arg, toMsec(0, 0, 0, arg, 0)
                                 == X.totalMilliseconds());

                x.setTotalMilliseconds(arg);
                if (veryVerbose) { T_(); P_(arg); P(X); }
                LOOP_ASSERT(arg, toMsec(0, 0, 0, 0, arg)
                                 == X.totalMilliseconds());
            }
        }

      } break;
      case 11: {
        // --------------------------------------------------------------------
        // TESTING INITIALIZATION CONSTRUCTOR:
        //   The constructor must correctly manipulate its three required and
        //   two optional parameters to correctly initialize the total-
        //   milliseconds (64 bit) integer data member.  The key concern is
        //   that the manipulation of 'int' values (which may be 32 bit)
        //   results in the correct 64-bit representation (without truncation).
        //
        // Plan:
        //   Specify a set S of dates as (y, m, d) triplets having widely
        //   varying values.  For each (y, m, d) in S, construct a date object
        //   X and verify that X has the expected value.
        //
        // Testing
        //   bdet_DtI(int days, int hrs, int mins, int secs, int ms);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Initialization Constructors"
                          << "\n===================================" << endl;

        if (verbose)
            cout << "\nTesting: 'bdet_DatetimeInterval(date)'" << endl;
        {
            static const struct {
                int d_days;  int d_hrs;  int d_mins;  int d_secs;  int d_msecs;
            } VALUES[] = {
                { 0,  0,  0,  0,   0  },   { -1,  -1,  -1,  -1,   -1  },
                { 0,  0,  0,  0, 999  },   {  0,   0,   0,   0, -999  },
                { 0,  0,  0, 59,   0  },   {  0,   0,   0, -59,    0  },
                { 0,  0, 59,  0,   0  },   {  0,   0, -59,   0,    0  },
                { 0, 23,  0,  0,   0  },   {  0, -23,   0,   0,    0  },
                { 5,  0,  0,  0,   0  },   { -5,   0,   0,   0,    0  },
                { 5, 23, 22, 21, 209  },   { -5, -23, -59, -59, -999  },
            };

            const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;

            for (int i = 0; i < NUM_VALUES; ++i) {
                const int DAYS  = VALUES[i].d_days;
                const int HRS   = VALUES[i].d_hrs;
                const int MINS  = VALUES[i].d_mins;
                const int SECS  = VALUES[i].d_secs;
                const int MSECS = VALUES[i].d_msecs;

                {
                    Obj x(DAYS, HRS, MINS, SECS, MSECS);  const Obj& X = x;
                    if (veryVerbose) {
                        T_(); P_(X); P_(DAYS); P_(HRS); P_(MINS);
                                     P_(SECS); P(MSECS);
                    }
                    LOOP_ASSERT(i, DAYS  == X.days());
                    LOOP_ASSERT(i, HRS   == X.hours());
                    LOOP_ASSERT(i, MINS  == X.minutes());
                    LOOP_ASSERT(i, SECS  == X.seconds());
                    LOOP_ASSERT(i, MSECS == X.milliseconds());
                    LOOP_ASSERT(i, toMsec(DAYS, HRS, MINS, SECS, MSECS)
                                         == X.totalMilliseconds());
                }

                {
                    Obj x(DAYS, HRS, MINS, SECS);  const Obj& X = x;
                    if (veryVerbose) {
                        T_(); P_(X); P_(DAYS); P_(HRS); P_(MINS); P(SECS);
                    }
                    LOOP_ASSERT(i, DAYS  == X.days());
                    LOOP_ASSERT(i, HRS   == X.hours());
                    LOOP_ASSERT(i, MINS  == X.minutes());
                    LOOP_ASSERT(i, SECS  == X.seconds());
                    LOOP_ASSERT(i, 0     == X.milliseconds());
                    LOOP_ASSERT(i, toMsec(DAYS, HRS, MINS, SECS)
                                         == X.totalMilliseconds());
                }

                {
                    Obj x(DAYS, HRS, MINS);  const Obj& X = x;
                    if (veryVerbose) {
                        T_(); P_(X);  P_(DAYS); P_(HRS); P(MINS); NL();
                    }
                    LOOP_ASSERT(i, DAYS  == X.days());
                    LOOP_ASSERT(i, HRS   == X.hours());
                    LOOP_ASSERT(i, MINS  == X.minutes());
                    LOOP_ASSERT(i, 0     == X.seconds());
                    LOOP_ASSERT(i, 0     == X.milliseconds());
                    LOOP_ASSERT(i, toMsec(DAYS, HRS, MINS)
                                         == X.totalMilliseconds());
                }
            }
        }

      } break;
      case 10: {
        // --------------------------------------------------------------------
        // TESTING 'bdex' STREAMING FUNCTIONALITY:
        //   The 'bdex' streaming concerns for this component are absolutely
        //   standard.  We first probe the member functions 'outStream' and
        //   'inStream' in the manner of a "breathing test" to verify basic
        //   functionality, then we thoroughly test that functionality using
        //   the available bdex stream functions.  We next step through
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
        //   BDEX STREAMING
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Streaming Functionality"
                          << "\n===============================" << endl;

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        // scalar and array object values for various stream tests

        const int DAA = 1,  HRA = 2,  MIA = 3,  SCA = 4,  MSA = 5;   // VA
        const int DAB = 6,  HRB = 7,  MIB = 8,  SCB = 9,  MSB = 0;   // VB
        const int DAC = 10, HRC = 11, MIC = 12, SCC = 13, MSC = 14;  // VC
        const int DAD = 15, HRD = 16, MID = 17, SCD = 18, MSD = 19;  // VD
        const int DAE = 20, HRE = 21, MIE = 22, SCE = 23, MSE = 24;  // VE
        const int DAF = 15, HRF = 26, MIF = 27, SCF = 28, MSF = 29;  // VF

        const Int64 TMSB = toMsec(DAB, HRB, MIB, SCB, MSB);

        const Obj VA(DAA, HRA, MIA, SCA, MSA);
        const Obj VB(DAB, HRB, MIB, SCB, MSB);
        const Obj VC(DAC, HRC, MIC, SCC, MSC);
        const Obj VD(DAD, HRD, MID, SCD, MSD);
        const Obj VE(DAE, HRE, MIE, SCE, MSE);
        const Obj VF(DAF, HRF, MIF, SCF, MSF);

        const int NUM_VALUES = 6;
        const Obj VALUES[NUM_VALUES] = { VA, VB, VC, VD, VE, VF };
        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout <<
            "\nTesting 'streamOut' and (valid) 'streamIn' functionality."
                          << endl;

        const int VERSION = Obj::maxSupportedBdexVersion();

        if (verbose) cout << "\nDirect initial trial of 'streamOut' and"
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

        if (verbose) cout <<
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

        if (verbose) cout << "\tOn corrupted data." << endl;

        const Obj W;               // default value (0)
        const Obj X(VA);           // control value (VA)
        const Obj Y(VB);           // new value (VB)

        if (verbose) cout << "\t\tGood stream (for control)." << endl;
        {
            Out out;
            out.putInt64(TMSB);
            const char *const OD  = out.data();
            const int         LOD = out.length();

            Obj t(X);        ASSERT(W != t);  ASSERT(X == t);  ASSERT(Y != t);
            In in(OD, LOD);  ASSERT(in);
            in.setSuppressVersionCheck(1);
            bdex_InStreamFunctions::streamIn(in, t, VERSION);  ASSERT(in);
                             ASSERT(W != t);  ASSERT(X != t);  ASSERT(Y == t);
        }

        if (verbose) cout << "\t\tBad version." << endl;
        if (verbose) cout << "\t\t\tVersion too small." <<endl;
        {
            const char version = 0; // too small ('version' must be >= 1)

            Out out;
            out.putInt64(TMSB);

            const char *const OD  = out.data();
            const int         LOD = out.length();

            Obj t(X);        ASSERT(W != t);  ASSERT(X == t);  ASSERT(Y != t);
            In in(OD, LOD);  ASSERT(in);
            in.setQuiet(!veryVerbose);
            in.setSuppressVersionCheck(1);
            bdex_InStreamFunctions::streamIn(in, t, version);  ASSERT(!in);
                             ASSERT(W != t);  ASSERT(X == t);  ASSERT(Y != t);
        }

        if (verbose) cout << "\t\t\tVersion too big." << endl;
        {
            const char version = 5; // too large (current versions all = 1)

            Out out;
            out.putInt64(TMSB);

            const char *const OD  = out.data();
            const int         LOD = out.length();

            Obj t(X);        ASSERT(W != t);  ASSERT(X == t);  ASSERT(Y != t);
            In in(OD, LOD);  ASSERT(in);
            in.setQuiet(!veryVerbose);
            in.setSuppressVersionCheck(1);
            bdex_InStreamFunctions::streamIn(in, t, version);  ASSERT(!in);
                             ASSERT(W != t);  ASSERT(X == t);  ASSERT(Y != t);
        }

        if (verbose) cout << "\nTesting 'maxSupportedBdexVersion()'." << endl;
        {
            const Obj X;
            ASSERT(1 == X.maxSupportedBdexVersion());
        }

        if (verbose) cout << "\nWire format direct tests." << endl;
        {
            static const struct {
                int         d_lineNum;      // source line number
                int         d_day;          // specification day
                int         d_hour;         // specification hour
                int         d_minute;       // specification minute
                int         d_second;       // specification second
                int         d_millisecond;  // specification millisecond
                int         d_version;      // version to stream with
                int         d_length;       // expect output length
                const char *d_fmt_p;        // expected output format
            } DATA[] = {
                //line  day hour min sec milli ver len format
                //----  --- ---- --- --- ----- --- --- ---------------------
                { L_,   17,  14, 10,  2,  117,  0,  0, ""                    },
                { L_,   98,  20,  8, 27,  983,  0,  0, ""                    },

                { L_,   17,  14, 10,  2,  117,  1,  8,
                                          "\x00\x00\x00\x00\x5a\x96\x57\x05" },
                { L_,   98,  20,  8, 27,  983,  1,  8,
                                          "\x00\x00\x00\x01\xfd\x01\x9a\x4f" },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int LINE         = DATA[i].d_lineNum;
                const int DAY          = DATA[i].d_day;
                const int HOUR         = DATA[i].d_hour;
                const int MINUTE       = DATA[i].d_minute;
                const int SECOND       = DATA[i].d_second;
                const int MILLISECOND  = DATA[i].d_millisecond;
                const int VERSION      = DATA[i].d_version;
                const int LEN          = DATA[i].d_length;
                const char *const FMT  = DATA[i].d_fmt_p;

                Obj mX(DAY, HOUR, MINUTE, SECOND, MILLISECOND);
                const Obj& X = mX;
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
      } break;
      case 9: {
        // --------------------------------------------------------------------
        // TESTING ASSIGNMENT OPERATOR:
        //   Any value must be assignable to an object having any initial value
        //   without affecting the rhs operand value.  Also, any object must be
        //   assignable to itself.
        //
        // Plan:
        //   Specify a set S of (unique) objects with substantial and varied
        //   differences in value.  Construct and initialize all combinations
        //   (u, v) in the cross product S x S, copy construct a control w from
        //   v, assign v to u, and assert that w == u and w == v.  Then test
        //   aliasing by copy constructing a control w from each u in S,
        //   assigning u to itself, and verifying that w == u.
        //
        // Testing:
        //   bdet_Date& operator=(const bdet_DatetimeInterval& rhs);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Assignment Operator"
                          << "\n===========================" << endl;

        if (verbose) cout << "\nTesting Assignment u = V." << endl;
        {
            static const struct {
                int d_days;
                int d_hours;
                int d_mins;
                int d_secs;
                int d_msecs;
            } VALUES[] = {
                { 0,  0,  0,  0,   0  },   { -1,  -1,  -1,  -1,   -1  },
                { 0,  0,  0,  0, 999  },   {  0,   0,   0,   0, -999  },
                { 0,  0,  0, 59,   0  },   {  0,   0,   0, -59,    0  },
                { 0,  0, 59,  0,   0  },   {  0,   0, -59,   0,    0  },
                { 0, 23,  0,  0,   0  },   {  0, -23,   0,   0,    0  },
                { 5,  0,  0,  0,   0  },   { -5,   0,   0,   0,    0  },
                { 5, 23, 59, 59, 999  },   { -5, -23, -59, -59, -999  },
            };

            const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;
            int i;

            for (i = 0; i < NUM_VALUES; ++i) {
                Obj v;  const Obj& V = v;
                v.setInterval(VALUES[i].d_days, VALUES[i].d_hours,
                              VALUES[i].d_mins, VALUES[i].d_secs,
                              VALUES[i].d_msecs);
                for (int j = 0; j < NUM_VALUES; ++j) {
                    Obj u;  const Obj& U = u;
                    u.setInterval(VALUES[j].d_days, VALUES[j].d_hours,
                                  VALUES[j].d_mins, VALUES[j].d_secs,
                                  VALUES[j].d_msecs);
                    if (veryVerbose) { T_();  P_(V);  P_(U); }
                    Obj w(V);  const Obj &W = w;          // control
                    u = V;
                    if (veryVerbose) P(U);
                    LOOP2_ASSERT(i, j, W == U);
                    LOOP2_ASSERT(i, j, W == V);
                }
            }

            if (verbose) cout << "\nTesting assignment u = u (Aliasing)."
                              << endl;

            for (i = 0; i < NUM_VALUES; ++i) {
                Obj u;  const Obj& U = u;
                u.setInterval(VALUES[i].d_days, VALUES[i].d_hours,
                              VALUES[i].d_mins, VALUES[i].d_secs,
                              VALUES[i].d_msecs);
                Obj w(U);  const Obj &W = w;              // control
                u = u;
                if (veryVerbose) { T_();  P_(U);  P(W); }
                LOOP_ASSERT(i, W == U);
            }
        }

      } break;
      case 8: {
        // --------------------------------------------------------------------
        // TESTING GENERATOR FUNCTION 'g':
        //   Void for 'bdet_datetimeinterval'.
        // --------------------------------------------------------------------

      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING COPY CONSTRUCTOR:
        //   Any value must be able to be copy constructed without affecting
        //   its argument.
        //
        // Plan:
        //   Specify a set S whose elements have substantial and varied
        //   differences in value.  For each element in S, construct and
        //   initialize identically valued objects w and x using tested
        //   methods.  Then copy construct an object y from x, and use the
        //   equality operator to assert that both x and y have the same value
        //   as w.
        //
        // Testing:
        //   bdet_DatetimeInterval(const bdet_DatetimeInterval& original);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Copy Constructor"
                          << "\n========================" << endl;

        if (verbose) cout << "\nTesting copy constructor." << endl;
        {
            static const struct {
                int d_days;
                int d_hours;
                int d_mins;
                int d_secs;
                int d_msecs;
            } VALUES[] = {
                { 0,  0,  0,  0,   0  },   { -1,  -1,  -1,  -1,   -1  },
                { 0,  0,  0,  0, 999  },   {  0,   0,   0,   0, -999  },
                { 0,  0,  0, 59,   0  },   {  0,   0,   0, -59,    0  },
                { 0,  0, 59,  0,   0  },   {  0,   0, -59,   0,    0  },
                { 0, 23,  0,  0,   0  },   {  0, -23,   0,   0,    0  },
                { 5,  0,  0,  0,   0  },   { -5,   0,   0,   0,    0  },
                { 5, 23, 59, 59, 999  },   { -5, -23, -59, -59, -999  },
            };

            const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;

            for (int i = 0; i < NUM_VALUES; ++i) {
                Obj w;  const Obj& W = w;           // control
                w.setInterval(VALUES[i].d_days, VALUES[i].d_hours,
                              VALUES[i].d_mins, VALUES[i].d_secs,
                              VALUES[i].d_msecs);

                Obj x;  const Obj& X = x;
                x.setInterval(VALUES[i].d_days, VALUES[i].d_hours,
                              VALUES[i].d_mins, VALUES[i].d_secs,
                              VALUES[i].d_msecs);

                Obj y(X);  const Obj &Y = y;
                if (veryVerbose) { T_();  P_(W);  P_(X);  P(Y); }
                LOOP_ASSERT(i, X == W);  LOOP_ASSERT(i, Y == W);
            }
        }

      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING EQUALITY OPERATORS:
        //   Any subtle variation in value must be detected by the equality
        //   operators.  The test data have variations in each input parameter,
        //   even though the underlying representation is a single 64-bit
        //   integer.
        //
        // Plan:
        //   Specify a set S of unique object values having various minor or
        //   subtle differences.  Verify the correctness of 'operator==' and
        //   'operator!=' using all elements (u, v) of the cross product
        //    S X S.
        //
        // Testing:
        //   operator==(const bdet_DtI&, const bdet_DtI&);
        //   operator!=(const bdet_DtI&, const bdet_DtI&);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Equality Operators"
                          << "\n==========================" << endl;

        if (verbose) cout <<
            "\nCompare each pair of values (u, v) in S X S." << endl;
        {
            static const struct {
                int d_days;
                int d_hours;
                int d_mins;
                int d_secs;
                int d_msecs;
            } VALUES[] = {
                {     1,   1,   1,   1,    1 },
                {     1,   1,   1,   1,    2 },
                {     1,   1,   1,   2,    1 },
                {     1,   1,   2,   1,    1 },
                {     1,   2,   1,   1,    1 },
                {     2,   1,   1,   1,    1 },

                { -9999, -23, -59, -59, -999 },
                { -9999, -23, -59, -59, -998 },
                { -9999, -23, -59, -58, -999 },
                { -9999, -23, -58, -59, -999 },
                { -9999, -22, -59, -59, -999 },
                { -9998, -23, -59, -59, -999 },
            };

            const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;

            for (int i = 0; i < NUM_VALUES; ++i) {
                Obj u;  const Obj& U = u;
                u.setInterval(VALUES[i].d_days, VALUES[i].d_hours,
                              VALUES[i].d_mins, VALUES[i].d_secs,
                              VALUES[i].d_msecs);
                for (int j = 0; j < NUM_VALUES; ++j) {
                    Obj v;  const Obj& V = v;
                    v.setInterval(VALUES[j].d_days, VALUES[j].d_hours,
                                  VALUES[j].d_mins, VALUES[j].d_secs,
                                  VALUES[j].d_msecs);
                    bool isSame = i == j;
                    if (veryVerbose) { P_(i); P_(j); P_(U); P(V); }
                    LOOP2_ASSERT(i, j,  isSame == (U == V));
                    LOOP2_ASSERT(i, j, !isSame == (U != V));
                    LOOP2_ASSERT(i, j,  isSame == (V == U));
                    LOOP2_ASSERT(i, j, !isSame == (V != U));
                }
            }
        }

      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING OUTPUT (<<) OPERATOR:
        //   The output operator is implemented using the now-fully-tested
        //   basic accessors.  A few test vectors can thoroughly test the
        //   actual output-streaming functionality.
        //
        // Plan:
        //   For each of a small representative set of object values, use
        //   'ostrstream' to write that object's value to a character buffer
        //   and then compare the contents of that buffer with the expected
        //   output format.
        //
        // Testing:
        //   operator<<(ostream&, const bdet_DatetimeInterval&);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Output (<<) Operator"
                          << "\n============================" << endl;

        if (verbose) cout << "\nTesting 'operator<<' (ostream)." << endl;
        {
            static const struct {
                int         d_lineNum;   // source line number
                int         d_days;      // day field value
                int         d_hours;     // hour field value
                int         d_minutes;   // minute field value
                int         d_seconds;   // second field value
                int         d_msecs;     // millisecond field value
                const char *d_fmt_p;     // expected output format
            } DATA[] = {
                //line days  hrs  mins  secs  msecs    output format
                //---- ----  ---  ----  ----  -----   -------------------
                { L_,    0,    0,    0,    0,     0,  "+0_00:00:00.000" },
                { L_,    1,   23,   59,   59,   999,  "+1_23:59:59.999" },
                { L_,   -1,  -23,  -59,  -59,  -999,  "-1_23:59:59.999" },
                { L_,    1,   23,   59,   59,  1000,  "+2_00:00:00.000" },
                { L_,   -1,  -23,  -59,  -59, -1000,  "-2_00:00:00.000" },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            const int SIZE = 1000;        // big enough to hold output string
            const char XX = (char) 0xFF;  // represents unset 'char'
            char mCtrlBuf[SIZE];  memset(mCtrlBuf, XX, SIZE);
            const char *CTRL_BUF = mCtrlBuf; // Used for extra character check.

            for (int di = 0; di < NUM_DATA;  ++di) {
                const int         LINE    = DATA[di].d_lineNum;
                const int         DAYS    = DATA[di].d_days;
                const int         HOURS   = DATA[di].d_hours;
                const int         MINUTES = DATA[di].d_minutes;
                const int         SECONDS = DATA[di].d_seconds;
                const int         MSECS   = DATA[di].d_msecs;
                const char *const FMT     = DATA[di].d_fmt_p;

                char buf[SIZE];
                memcpy(buf, CTRL_BUF, SIZE); // Preset buf to 'unset' values.

                Obj x;  const Obj& X = x;
                x.setInterval(DAYS, HOURS, MINUTES, SECONDS, MSECS);

                if (veryVerbose) cout << "\tEXPECTED FORMAT: " << FMT << endl;
                ostrstream out(buf, SIZE);  out << X << ends;
                if (veryVerbose) cout << "\tACTUAL FORMAT:   " << buf << endl;

                const int SZ = strlen(FMT) + 1;
                LOOP_ASSERT(LINE, SZ < SIZE);  // Check buffer is large enough.
                LOOP_ASSERT(LINE, XX == buf[SIZE - 1]);  // Check for overrun.
                LOOP_ASSERT(LINE,  0 == memcmp(buf, FMT, SZ));
                LOOP_ASSERT(LINE,  0 ==
                                     memcmp(buf + SZ, CTRL_BUF + SZ, SIZE-SZ));
            }
        }

      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING BASIC ACCESSORS:
        //   Each accessor must perform the appropriate arithmetic to convert
        //   the internal milliseconds representation to the five-parameter
        //   (d, h, m, s, ms) canonical representation or else to the rounded
        //   "total" units form.  In the case of 'totalSecondsAsDouble' there
        //   is the further concern that there should be no loss of data for
        //   reasonably large values.
        //
        // Plan:
        //   For each of a sequence of unique object values, verify that each
        //   of the basic accessors returns the expected value.
        //
        // Testing:
        //   int days() const
        //   int hours() const
        //   int minutes() const
        //   int seconds() const
        //   int milliseconds() const
        //   int totalDays() const
        //   int totalHours() const
        //   int totalMinutes() const
        //   int totalSeconds() const
        //   double totalSecondsAsDouble() const
        //   int totalMilliseconds() const
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING BASIC ACCESSORS"
                          << "\n=======================" << endl;

        if (verbose) cout << "\nTesting: Basic Accessors" << endl;
        {
            static const struct {
                int d_days;
                int d_hours;
                int d_minutes;
                int d_seconds;
                int d_msecs;
            } VALUES[] = {
                {  0,     0,  0,  0,   0 },  {     -1,  -1,  -1,  -1,   -1 },
                {  0,     0,  0,  0, 999 },  {      0,   0,   0,   0, -999 },
                {  0,     0,  0, 59,   0 },  {      0,   0,   0, -59,    0 },
                {  0,     0, 59,  0,   0 },  {      0,   0, -59,   0,    0 },
                {  0,    23,  0,  0,   0 },  {      0, -23,   0,   0,    0 },
                { 45,     0,  0,  0,   0 },  {    -45,   0,   0,   0,    0 },
                { 45,    23, 59, 59, 999 },  {    -45, -23, -59, -59, -999 },

                { 45,    23, 22, 21, 206 },  { -24700, -21, -40, -21, -831 },
                { 45,    23, 22, 21, 207 },  {  22840,  18,  25,  59,  676 },
                { 45,    23, 22, 21, 208 },  {  23469,  13,  56,  00,  200 },
                { 10000, 23, 42, 24, 999 },  { -24263, -14, -22, -24, -287 },
                { 24815, 23, 44, 57, 926 },  { -24847, -22, -50, -55, -799 },
           };

            const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;

            for (int i = 0; i < NUM_VALUES; ++i) {
                const int DAYS      = VALUES[i].d_days;
                const int HOURS     = VALUES[i].d_hours;
                const int MINS      = VALUES[i].d_minutes;
                const int SECS      = VALUES[i].d_seconds;
                const int MSECS     = VALUES[i].d_msecs;

                const Int64 TOT_MSECS = toMsec(DAYS, HOURS, MINS, SECS, MSECS);
                const int TOT_SECS  = (int)(TOT_MSECS / 1000);
                const int TOT_MINS  = (int)(TOT_MSECS / (1000 * 60));
                const int TOT_HOURS = (int)(TOT_MSECS / (1000 * 60 * 60));
                const int TOT_DAYS  = (int)(TOT_MSECS / (1000 * 60 * 60 * 24));
                const double DBL_SECS  = (double) TOT_MSECS / (1.0 * 1000);

                Obj x;  const Obj& X = x;
                x.setInterval(DAYS, HOURS, MINS, SECS, MSECS);

                if (veryVerbose) {
                    T_() P_(i)    P(X)
                    T_() P_(DAYS) P_(HOURS) P_(MINS) P_(SECS) P_(MSECS)
                    P(DBL_SECS)
                    T_() T_() printf("debug: "); P_(DBL_SECS * 100000);
                    P_(X.totalSecondsAsDouble()); P_(TOT_MSECS);
                    P((Int64)(DBL_SECS * 1000));
                }

                LOOP_ASSERT(i, DAYS      == X.days());
                LOOP_ASSERT(i, HOURS     == X.hours());
                LOOP_ASSERT(i, MINS      == X.minutes());
                LOOP_ASSERT(i, SECS      == X.seconds());
                LOOP_ASSERT(i, MSECS     == X.milliseconds());

                LOOP_ASSERT(i, TOT_DAYS  == X.totalDays());
                LOOP_ASSERT(i, TOT_HOURS == X.totalHours());
                LOOP_ASSERT(i, TOT_MINS  == X.totalMinutes());
                LOOP_ASSERT(i, TOT_SECS  == X.totalSeconds());
                LOOP_ASSERT(i, TOT_MSECS == X.totalMilliseconds());

#if defined(BSLS_PLATFORM__CPU_X86) && defined(BSLS_PLATFORM__CMP_GNU)
    // This is necessary because on linux, for some inexplainable reason,
    // even 'X.totalSecondsAsDouble() == X.totalSecondsAsDouble()' returns
    // false.
                const double SECONDS = X.totalSecondsAsDouble();
                LOOP_ASSERT(i, DBL_SECS  == SECONDS);

    // This is commented out due to precision problem when casting
    // a double to Int64.  For example:

    // const double DBL_SECS  = (double) TOT_MSECS / (1.0 * 1000);  // 999
    // P((Int64)(((double)TOT_MSECS/(1.0 * 1000)) * 1000));  // prints -999
    // P((Int64)(DBL_SECS * 1000));                          // prints -998

                //LOOP_ASSERT(i, TOT_MSECS == (Int64)(DBL_SECS * 1000));
#else
                LOOP_ASSERT(i, DBL_SECS  == X.totalSecondsAsDouble());
                LOOP_ASSERT(i, TOT_MSECS == (Int64)(DBL_SECS * 1000));
#endif
            }
        }

      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING GENERATOR FUNCTION 'gg':
        //   Void for 'bdet_datetimeinterval'.
        // --------------------------------------------------------------------

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING PRIMARY MANIPULATORS (BOOTSTRAP):
        //   The separate interval fields must be multiplied by the appropriate
        //   factors to convert the five-parameter input representation to the
        //   internal milliseconds representation.  Because the internal
        //   representation stores total milliseconds, it is sufficient to
        //   select test data within the "canonical representation" and to
        //   confirm the total-milliseconds value (since we have inspected the
        //   imp and are concerned only with correct constants and operators).
        //   The key concern is that the manipulation of 'int' values (which
        //   may be 32 bit) results in the correct 64-bit representation
        //   (without truncation).
        //
        // Plan:
        //   First, verify the default constructor by testing the value of the
        //   resulting object.
        //
        //   Next, for a sequence of independent test values, use the default
        //   constructor to create an object and use the primary manipulator
        //   to set its value.  Verify that value using the basic accessors.
        //   Note that the destructor is exercised on each configuration as the
        //   object being tested leaves scope.
        //
        // Testing:
        //   bdet_DatetimeInterval();
        //   ~bdet_DatetimeInterval();
        //   void setInterval(int d, int h = 0, int m = 0, int s = 0, int ms=0)
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Primary Manipulator"
                          << "\n===========================" << endl;

        if (verbose) cout << "\nTesting default constructor." << endl;

        Obj x;  const Obj& X = x;        if (veryVerbose) P(X);

        ASSERT(0 == X.days());     ASSERT(0 == X.hours());
        ASSERT(0 == X.minutes());  ASSERT(0 == X.seconds());
        ASSERT(0 == X.milliseconds());

        if (verbose) cout << "\nTesting 'setInterval'." << endl;
        {
            static const struct {
                int d_days;  int d_hrs;  int d_mins;  int d_secs;  int d_msecs;
            } VALUES[] = {
                { 0,  0,  0,  0,   0  },   { -1,  -1,  -1,  -1,   -1  },
                { 0,  0,  0,  0, 999  },   {  0,   0,   0,   0, -999  },
                { 0,  0,  0, 59,   0  },   {  0,   0,   0, -59,    0  },
                { 0,  0, 59,  0,   0  },   {  0,   0, -59,   0,    0  },
                { 0, 23,  0,  0,   0  },   {  0, -23,   0,   0,    0  },
                { 5,  0,  0,  0,   0  },   { -5,   0,   0,   0,    0  },
                { 5, 23, 22, 21, 209  },   { -5, -23, -59, -59, -999  },
            };

            const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;

            for (int i = 0; i < NUM_VALUES; ++i) {
                const int DAYS  = VALUES[i].d_days;
                const int HRS   = VALUES[i].d_hrs;
                const int MINS  = VALUES[i].d_mins;
                const int SECS  = VALUES[i].d_secs;
                const int MSECS = VALUES[i].d_msecs;

                Obj x;  const Obj& X = x;
                x.setInterval(DAYS, HRS, MINS, SECS, MSECS);
                if (veryVerbose) {
                    T_(); P_(X); P_(DAYS); P_(HRS); P_(MINS);P_(SECS);P(MSECS);
                }
                LOOP_ASSERT(i, DAYS  == X.days());
                LOOP_ASSERT(i, HRS   == X.hours());
                LOOP_ASSERT(i, MINS  == X.minutes());
                LOOP_ASSERT(i, SECS  == X.seconds());
                LOOP_ASSERT(i, MSECS == X.milliseconds());
                LOOP_ASSERT(i, toMsec(DAYS, HRS, MINS, SECS, MSECS)
                                     == X.totalMilliseconds());

                x.setInterval(DAYS, HRS, MINS, SECS);
                if (veryVerbose) {
                    T_();  P_(X);  P_(DAYS);  P_(HRS);  P_(MINS);  P(SECS);
                }
                LOOP_ASSERT(i, DAYS  == X.days());
                LOOP_ASSERT(i, HRS   == X.hours());
                LOOP_ASSERT(i, MINS  == X.minutes());
                LOOP_ASSERT(i, SECS  == X.seconds());
                LOOP_ASSERT(i, 0     == X.milliseconds());
                LOOP_ASSERT(i, toMsec(DAYS, HRS, MINS, SECS)
                                     == X.totalMilliseconds());

                x.setInterval(DAYS, HRS, MINS);
                if (veryVerbose) { T_();  P_(X);  P_(DAYS);  P_(HRS); P(MINS);}
                LOOP_ASSERT(i, DAYS  == X.days());
                LOOP_ASSERT(i, HRS   == X.hours());
                LOOP_ASSERT(i, MINS  == X.minutes());
                LOOP_ASSERT(i, 0     == X.seconds());
                LOOP_ASSERT(i, 0     == X.milliseconds());
                LOOP_ASSERT(i, toMsec(DAYS, HRS, MINS)
                                     == X.totalMilliseconds());

                x.setInterval(DAYS, HRS);
                if (veryVerbose) { T_();  P_(X);  P_(DAYS);  P(HRS); }
                LOOP_ASSERT(i, DAYS  == X.days());
                LOOP_ASSERT(i, HRS   == X.hours());
                LOOP_ASSERT(i, 0     == X.minutes());
                LOOP_ASSERT(i, 0     == X.seconds());
                LOOP_ASSERT(i, 0     == X.milliseconds());
                LOOP_ASSERT(i, toMsec(DAYS, HRS)
                                     == X.totalMilliseconds());

                x.setInterval(DAYS);
                if (veryVerbose) { T_();  P_(X);  P(DAYS);  NL(); }
                LOOP_ASSERT(i, DAYS  == X.days());
                LOOP_ASSERT(i, 0     == X.hours());
                LOOP_ASSERT(i, 0     == X.minutes());
                LOOP_ASSERT(i, 0     == X.seconds());
                LOOP_ASSERT(i, 0     == X.milliseconds());
                LOOP_ASSERT(i, toMsec(DAYS)
                                     == X.totalMilliseconds());
            }
        }

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST:
        //   Exercise a broad cross-section of value-semantic functionality
        //   before beginning testing in earnest.  Probe that functionality
        //   systematically and incrementally to discover basic errors in
        //   isolation.
        //
        // Plan:
        //   Create four test objects by using the default, initializing, and
        //   copy constructors.  Exercise the basic value-semantic methods and
        //   the equality operators using these test objects.  Invoke the
        //   primary manipulator [3, 6], copy constructor [2, 8], and
        //   assignment operator without [9, 10] and with [11] aliasing.  Use
        //   the basic accessors to verify the expected results.  Display
        //   object values frequently in verbose mode.  Note that 'VA', 'VB',
        //   and 'VC' denote unique, but otherwise arbitrary, object values,
        //   while '0' denotes the default object value.
        //
        // 1.  Create a default object x1.          { x1:0 }
        // 2.  Create an object x2 (copy from x1).  { x1:0  x2:0 }
        // 3.  Set x1 to VA.                        { x1:VA x2:0 }
        // 4.  Set x2 to VA.                        { x1:VA x2:VA }
        // 5.  Set x2 to VB.                        { x1:VA x2:VB }
        // 6.  Set x1 to 0.                         { x1:0  x2:VB }
        // 7.  Create an object x3 (with value VC). { x1:0  x2:VB x3:VC }
        // 8.  Create an object x4 (copy from x1).  { x1:0  x2:VB x3:VC x4:0 }
        // 9.  Assign x2 = x1.                      { x1:0  x2:0  x3:VC x4:0 }
        // 10. Assign x2 = x3.                      { x1:0  x2:VC x3:VC x4:0 }
        // 11. Assign x1 = x1 (aliasing).           { x1:0  x2:VC x3:VC x4:0 }
        //
        // Testing:
        //   This Test Case exercises basic value-semantic functionality.
        // --------------------------------------------------------------------
        if (verbose) cout << "\nBREATHING TEST"
                          << "\n==============" << endl;

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        // Values for testing

        const int DAA = 1, HRA = 2, MIA = 3, SCA = 4, MSA = 5;  // VA fields
        const int DAB = 6, HRB = 7, MIB = 8, SCB = 9, MSB = 0;  // VB fields
        const int DAC = 9, HRC = 9, MIC = 9, SCC = 9, MSC = 9;  // VC fields

        //int d, h, m, s, ms;     // reusable variables for accessors call

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 1. Create a default object x1."
                             "\t\t\t{ x1:0 }" << endl;
        Obj mX1;  const Obj& X1 = mX1;
        if (verbose) { T_();  P(X1); }

        if (verbose) cout << "\ta. Check initial state of x1." << endl;
        ASSERT(0 == X1.days());
        ASSERT(0 == X1.hours());
        ASSERT(0 == X1.minutes());
        ASSERT(0 == X1.seconds());
        ASSERT(0 == X1.milliseconds());

        if (verbose) cout <<
            "\tb. Try equality operators: x1 <op> x1." << endl;
        ASSERT(1 == (X1 == X1));        ASSERT(0 == (X1 != X1));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 2. Create an object x2 (copy from x1)."
                             "\t\t{ x1:0  x2:0 }" << endl;
        Obj mX2(X1);  const Obj& X2 = mX2;
        if (verbose) { T_();  P(X2); }

        if (verbose) cout << "\ta. Check the initial state of x2." << endl;
        ASSERT(0 == X2.days());
        ASSERT(0 == X2.hours());
        ASSERT(0 == X2.minutes());
        ASSERT(0 == X2.seconds());
        ASSERT(0 == X2.milliseconds());

        if (verbose) cout <<
            "\tb. Try equality operators: x2 <op> x1, x2." << endl;
        ASSERT(1 == (X2 == X1));        ASSERT(0 == (X2 != X1));
        ASSERT(1 == (X2 == X2));        ASSERT(0 == (X2 != X2));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 3. Set x1 to a new value VA."
                             "\t\t\t{ x1:VA x2:0 }" << endl;
        mX1.setInterval(DAA, HRA, MIA, SCA, MSA);
        if (verbose) { T_();  P(X1); }

        if (verbose) cout << "\ta. Check new state of x1." << endl;
        ASSERT(DAA == X1.days());
        ASSERT(HRA == X1.hours());
        ASSERT(MIA == X1.minutes());
        ASSERT(SCA == X1.seconds());
        ASSERT(MSA == X1.milliseconds());

        if (verbose) cout <<
            "\tb. Try equality operators: x1 <op> x1, x2." << endl;
        ASSERT(1 == (X1 == X1));        ASSERT(0 == (X1 != X1));
        ASSERT(0 == (X1 == X2));        ASSERT(1 == (X1 != X2));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 4. Set x2 to a new value VA."
                             "\t\t\t{ x1:VA x2:VA }" << endl;
        mX2.setTotalDays(DAA);   // Use different manipulators to get to the
        mX2.addHours(HRA);      // same place...
        mX2.addMinutes(MIA);
        mX2.addSeconds(SCA);
        mX2.addMilliseconds(MSA);
        if (verbose) { T_();  P(X2); }

        if (verbose) cout << "\ta. Check initial state of x2." << endl;
        ASSERT(DAA == X2.days());
        ASSERT(HRA == X2.hours());
        ASSERT(MIA == X2.minutes());
        ASSERT(SCA == X2.seconds());
        ASSERT(MSA == X2.milliseconds());

        if (verbose) cout <<
            "\tb. Try equality operators: x2 <op> x1, x2." << endl;
        ASSERT(1 == (X2 == X1));        ASSERT(0 == (X2 != X1));
        ASSERT(1 == (X2 == X2));        ASSERT(0 == (X2 != X2));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 5. Set x2 to VB."
                             "\t\t\t\t{ x1:VA x2:VB }" << endl;
        mX2.setInterval(DAB, HRB, MIB, SCB, MSB);
        if (verbose) { T_();  P(X2); }

        if (verbose) cout << "\ta. Check new state of x2." << endl;
        ASSERT(DAB == X2.days());
        ASSERT(HRB == X2.hours());
        ASSERT(MIB == X2.minutes());
        ASSERT(SCB == X2.seconds());
        ASSERT(MSB == X2.milliseconds());

        if (verbose) cout <<
            "\tb. Try equality operators: x2 <op> x1, x2." << endl;
        ASSERT(0 == (X2 == X1));        ASSERT(1 == (X2 != X1));
        ASSERT(1 == (X2 == X2));        ASSERT(0 == (X2 != X2));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 6. Set x1 to 0."
                             "\t\t\t\t{ x1:0  x2:VB }" << endl;
        mX1.setInterval(0);
        if (verbose) { T_();  P(X1); }

        if (verbose) cout << "\ta. Check new state of x1." << endl;
        ASSERT(0 == X1.days());
        ASSERT(0 == X1.hours());
        ASSERT(0 == X1.minutes());
        ASSERT(0 == X1.seconds());
        ASSERT(0 == X1.milliseconds());

        if (verbose) cout <<
            "\tb. Try equality operators: x1 <op> x1, x2." << endl;
        ASSERT(1 == (X1 == X1));        ASSERT(0 == (X1 != X1));
        ASSERT(0 == (X1 == X2));        ASSERT(1 == (X1 != X2));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 7. Create an object x3 with value VC."
                             "\t\t{ x1:0  x2:VB x3:VC }" << endl;

        Obj mX3(DAC, HRC, MIC, SCC, MSC);  const Obj& X3 = mX3;
        if (verbose) { T_();  P(X3); }

        if (verbose) cout << "\ta. Check new state of x3." << endl;
        ASSERT(DAC == X3.days());
        ASSERT(HRC == X3.hours());
        ASSERT(MIC == X3.minutes());
        ASSERT(SCC == X3.seconds());
        ASSERT(MSC == X3.milliseconds());

        if (verbose) cout <<
            "\tb. Try equality operators: x3 <op> x1, x2, x3." << endl;
        ASSERT(0 == (X3 == X1));        ASSERT(1 == (X3 != X1));
        ASSERT(0 == (X3 == X2));        ASSERT(1 == (X3 != X2));
        ASSERT(1 == (X3 == X3));        ASSERT(0 == (X3 != X3));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 8. Create an object x4 (copy from x1)."
                             "\t\t{ x1:0  x2:VB x3:VC x4:0 }" << endl;

        Obj mX4(X1);  const Obj& X4 = mX4;
        if (verbose) { T_();  P(X4); }

        if (verbose) cout << "\ta. Check initial state of x4." << endl;
        ASSERT(0 == X4.days());
        ASSERT(0 == X4.hours());
        ASSERT(0 == X4.minutes());
        ASSERT(0 == X4.seconds());
        ASSERT(0 == X4.milliseconds());

        if (verbose) cout <<
            "\tb. Try equality operators: x4 <op> x1, x2, x3, x4." << endl;
        ASSERT(1 == (X4 == X1));        ASSERT(0 == (X4 != X1));
        ASSERT(0 == (X4 == X2));        ASSERT(1 == (X4 != X2));
        ASSERT(0 == (X4 == X3));        ASSERT(1 == (X4 != X3));
        ASSERT(1 == (X4 == X4));        ASSERT(0 == (X4 != X4));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 9. Assign x2 = x1."
                             "\t\t\t\t{ x1:0  x2:0  x3:VC x4:0 }" << endl;

        mX2 = X1;
        if (verbose) { T_();  P(X2); }

        if (verbose) cout << "\ta. Check new state of x2." << endl;
        ASSERT(0 == X2.days());
        ASSERT(0 == X2.hours());
        ASSERT(0 == X2.minutes());
        ASSERT(0 == X2.seconds());
        ASSERT(0 == X2.milliseconds());

        if (verbose) cout <<
            "\tb. Try equality operators: x2 <op> x1, x2, x3, x4." << endl;
        ASSERT(1 == (X2 == X1));        ASSERT(0 == (X2 != X1));
        ASSERT(1 == (X2 == X2));        ASSERT(0 == (X2 != X2));
        ASSERT(0 == (X2 == X3));        ASSERT(1 == (X2 != X3));
        ASSERT(1 == (X2 == X4));        ASSERT(0 == (X2 != X4));


        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 10. Assign x2 = x3."
                             "\t\t\t\t{ x1:0  x2:VC x3:VC x4:0 }" << endl;

        mX2 = X3;
        if (verbose) { T_();  P(X2); }

        if (verbose) cout << "\ta. Check new state of x2." << endl;
        ASSERT(DAC == X2.days());
        ASSERT(HRC == X2.hours());
        ASSERT(MIC == X2.minutes());
        ASSERT(SCC == X2.seconds());
        ASSERT(MSC == X2.milliseconds());

        if (verbose) cout <<
            "\tb. Try equality operators: x2 <op> x1, x2, x3, x4." << endl;
        ASSERT(0 == (X2 == X1));        ASSERT(1 == (X2 != X1));
        ASSERT(1 == (X2 == X2));        ASSERT(0 == (X2 != X2));
        ASSERT(1 == (X2 == X3));        ASSERT(0 == (X2 != X3));
        ASSERT(0 == (X2 == X4));        ASSERT(1 == (X2 != X4));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 11. Assign x1 = x1 (aliasing)."
                             "\t\t\t{ x1:0  x2:VC x3:VC x4:0 }" << endl;

        mX1 = X1;
        if (verbose) { T_();  P(X1); }

        if (verbose) cout << "\ta. Check new state of x1." << endl;
        ASSERT(0 == X1.days());
        ASSERT(0 == X1.hours());
        ASSERT(0 == X1.minutes());
        ASSERT(0 == X1.seconds());
        ASSERT(0 == X1.milliseconds());

        if (verbose) cout <<
            "\tb. Try equality operators: x1 <op> x1, x2, x3, x4." << endl;
        ASSERT(1 == (X1 == X1));        ASSERT(0 == (X1 != X1));
        ASSERT(0 == (X1 == X2));        ASSERT(1 == (X1 != X2));
        ASSERT(0 == (X1 == X3));        ASSERT(1 == (X1 != X3));
        ASSERT(1 == (X1 == X4));        ASSERT(0 == (X1 != X4));

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
//      Copyright (C) Bloomberg L.P., 2002
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
