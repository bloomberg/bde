// bcemt_saturatedtimeconversion.t.cpp                                -*-C++-*-

#include <bcemt_saturatedtimeconversion.h>

#include <bsls_bsltestutil.h>

#include <bsl_cstdlib.h>
#include <bsl_iostream.h>
#include <bsl_limits.h>

using namespace BloombergLP;
using bsl::cout;
using bsl::cerr;
using bsl::endl;
using bsl::flush;

//=============================================================================
//                                  TEST PLAN
//-----------------------------------------------------------------------------
//                                  Overview
//                                  --------
//=============================================================================
//                    STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------

static int testStatus = 0;

static void aSsErT(int c, const char *s, int i)
{
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}
# define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

//=============================================================================
//                  STANDARD BDE LOOP-ASSERT TEST MACROS
//-----------------------------------------------------------------------------

#define LOOP_ASSERT(I,X) { \
    if (!(X)) { cout << #I << ": " << I << "\n"; aSsErT(1, #X, __LINE__);}}

#define LOOP2_ASSERT(I,J,X) { \
    if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " \
              << J << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP3_ASSERT(I,J,K,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" \
              << #K << ": " << K << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP4_ASSERT(I,J,K,L,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" \
              << #K << ": " << K << "\t" << #L << ": " << L <<  "\n";     \
              aSsErT(1, #X, __LINE__); } }

#define LOOP5_ASSERT(I,J,K,L,M,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" \
              << #K << ": " << K << "\t" << #L << ": " << L << "\t" <<    \
              #M << ": " << M <<  "\n";                                   \
              aSsErT(1, #X, __LINE__); } }

#define LOOP0_ASSERT    ASSERT
#define LOOP1_ASSERT    LOOP_ASSERT

//=============================================================================
//           BSLS_BSLTESTUTIL -- LIKE MACROS
// The 'BSLS_BSLTESTUTIL_EXPAND' macro is required to workaround a
// pre-processor issue on windows that prevents __VA_ARGS__ to be expanded in
// the definition of 'BSLS_BSLTESTUTIL_NUM_ARGS'
//-----------------------------------------------------------------------------

#define EXPAND(X)                                            \
    X

#define NUM_ARGS_IMPL(X5, X4, X3, X2, X1, X0, N, ...)        \
    N

#define NUM_ARGS(...)                                        \
    EXPAND(NUM_ARGS_IMPL(__VA_ARGS__, 5, 4, 3, 2, 1, 0, ""))

#define LOOPN_ASSERT_IMPL(N, ...)                            \
    EXPAND(LOOP ## N ## _ASSERT(__VA_ARGS__))

#define LOOPN_ASSERT(N, ...)                                 \
    LOOPN_ASSERT_IMPL(N, __VA_ARGS__)

#define ASSERTV(...)                                         \
    LOOPN_ASSERT(NUM_ARGS(__VA_ARGS__), __VA_ARGS__)

//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------

#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", " << flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number
#define T_ cout << "\t" << flush;             // Print a tab (w/o newline)

//=============================================================================
//                               GLOBAL TYPEDEF
//-----------------------------------------------------------------------------

typedef bcemt_SaturatedTimeConversion Obj;
typedef bsls::Types::Int64            Int64;
typedef bsls::Types::Uint64           Uint64;

//=============================================================================
//                               GLOBAL CONSTANTS
//-----------------------------------------------------------------------------

const Int64 intMax      = bsl::numeric_limits<int>::max();
const Int64 intMin      = bsl::numeric_limits<int>::min();
const Int64 int64Max    = bsl::numeric_limits<Int64>::max();
const Int64 int64Min    = bsl::numeric_limits<Int64>::min();
const Int64 uintMax     = bsl::numeric_limits<unsigned>::max();
const Int64 i16         = 1 << 16;
const Int64 i32         = ((Int64) 1 << 32);
const Int64 i48         = ((Int64) 1 << 48);

//=============================================================================
//                              STATIC FUNCTIONS
//-----------------------------------------------------------------------------

static int sign(Int64 value)
{
    return value < 0 ? -1 : 1;
}

//=============================================================================
//                                MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? bsl::atoi(argv[1]) : 0;
    int verbose = argc > 2;
    // int veryVerbose = argc > 3;
    // int veryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:
      case 6: {
        // --------------------------------------------------------------------
        // USAGE
        // --------------------------------------------------------------------

#if !defined(BCES_PLATFORM_POSIX_THREADS)
        if (verbose) cout << "NOT UNIX -- USAGE NOT TESTED\n"
                             "========================\n";
#else
        // Suppose we have a 'timespec' which is a popular way to store a time
        // quantify on Unix.  It has two fields, 'tv_sec' which indicates
        // seconds, and 'tv_nsec', which indicates nanonseconds.  The size
        // of these fields can vary depending up on the platform for the
        // purposes of our usage example they are both 32 bit signed values.

        timespec tm;
        if (sizeof(tm.tv_sec) != sizeof(int)) {
            cout << "USAGE: TV_SEC != 32 BITS -- NO TEST\n"
                    "===================================\n";
            break;
        }
        ASSERT(sizeof(tm.tv_sec) == sizeof(int));   // tm.tv_sec (on this
                                                    // platform) is 'int' or 32
                                                    // bit 'long'
        tm.tv_sec = -1;
        if (tm.tv_sec > 0) {
            cout << "USAGE: TV_SEC NOT SIGNED -- NO TEST\n"
                    "===================================\n";
            break;
        }
        ASSERT(tm.tv_sec < 0);                      // 'tm.tv_sec' is signed on
                                                    // this platform.
        tm.tv_nsec = -1;
        ASSERTV(tm.tv_nsec < 0);                    // 'tm.tv_nsec' is signed
                                                    // on this platform.

        if (verbose) cout << "TESTING USAGE EXAMPLE\n"
                             "=====================\n";

        // First, we take a 'bdet_TimeInterval' representing a time that can
        // be represented in a 'timespec'.  Our 'toTimeSpec' will convert one
        // to the other without modification.

        bdet_TimeInterval ti(12345678, 987654321);

        ASSERT(ti.seconds()     ==  12345678);
        ASSERT(ti.nanoseconds() == 987654321);

        bcemt_SaturatedTimeConversion::toTimeSpec(&tm, ti);

        ASSERT((bsls::Types::Int64) tm.tv_sec == ti.seconds());
        ASSERT(tm.tv_nsec                     == ti.nanoseconds());

        // Then, we set our time interval to a value to high to be represented
        // by the 'timespec':

        bsls::Types::Int64 usageMaxInt = bsl::numeric_limits<int>::max();
        ti.setInterval(usageMaxInt + 100000, 500 * 1000 * 1000);

        ASSERT(ti.seconds()     == usageMaxInt + 100000);
        ASSERT(ti.nanoseconds() == 500 * 1000 * 1000);

        // Next, we use 'toTimeSpec' to assign its value to 'tm':

        bcemt_SaturatedTimeConversion::toTimeSpec(&tm, ti);

        // Then, we observe that 'tm' has been 'saturated' -- it has been set
        // to the highest value that a 'timespec' is capable of representing:

        ASSERT(tm.tv_sec  == usageMaxInt);
        ASSERT(tm.tv_nsec == 999999999);

        // Now, we set our time interval to a value too low to be represented
        // by a 'timespec':

        bsls::Types::Int64 usageMinInt = bsl::numeric_limits<int>::min();
        ti.setInterval(usageMinInt - 100000, -500 * 1000 * 1000);

        ASSERT(ti.seconds()     == usageMinInt - 100000);
        ASSERT(ti.nanoseconds() == -500 * 1000 * 1000);

        // Finally, we do the conversion and observe that 'tm' has been
        // saturated and now is set to the lowest value that it can represent:

        bcemt_SaturatedTimeConversion::toTimeSpec(&tm, ti);

        ASSERT(tm.tv_sec  == usageMinInt);
        ASSERT(tm.tv_nsec == -999999999);
#endif
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING DWORD
        //
        // Concerns:
        //   That 'toMilliSec' copies and saturates properly.  Note that
        //   'DWORD' is a typedef to 'unsigned long' on Windows, and 'long'
        //   on Windows is always 4 bytes.
        //
        // Plan:
        //: o Calculate 'maxSec' and 'maxNSec', the 'seconds' and 'nanoSeconds'
        //:   fields that a 'bdet_TimeInterval' will have and map to the exact
        //:   highest value of DWORD (== uintMax) when properly converted
        //:   by 'toMilliSec'.
        //: o Do trials with 'seconds() == maxSex', and 'nanoseconds()' varying
        //:   between the max possible value and 0 by increments of a million,
        //:   observing that for nanonseconds greater than 'maxNSec', the
        //:   result is saturate, and below that it is not.
        //: o Vary input with 'seconds' from 'maxSec - 1000' to 'maxSec + 1000'
        //:   with 'nanoseconds == 0', verifying that for values below
        //:   'maxSec' no saturation occurs, and above 'maxSec' saturation
        //:   does occur.
        //: o Vary input with 'seconds' from 'maxSec - 1000' to 'maxSec + 1000'
        //:   with 'nanoseconds == maxNSec + 1000000', verifying that for
        //:   values below 'maxSec' no saturation occurs, and '>= maxSec'
        //:   saturation does occur.
        //: o Vary the seconds from 0 to 'maxSec + 2000000', incrementing
        //:   nanoseconds gradually, observing that saturation occurs at
        //:   exactly the right times.
        //: o Set nanoseconds to 'maxNSec' and vary seconds from 'maxSec' to
        //:   lower values down to 0, observing that no saturation occurs.
        //: o Set nanoseconds to a negative value and vary seconds from 0 down
        //:   to 'int64Min' by increments of '(1 << 48)', observing that
        //:   saturatioon always occurs.
        //: o Set nanoseconds to 'maxNSec' and slowly increment it while
        //:   setting seconds to 'maxSec' and incrementing it by '(1 << 48)'
        //:   and observe that saturation always occurs.
        //: o Set seconds to 0 and vary nanoseconds over the range
        //:   '( -BILLION, BILLION )' by increments of a million, observing
        //:   that negative values are saturated and positive values are not.
        //: o Set seconds to the max and min possible values, varying
        //:   nanoseconds over the full possible range by increments of a
        //:   million, and observe that saturation always properly occurs.
        // --------------------------------------------------------------------

#ifndef BCES_PLATFORM_WIN32_THREADS
        if (verbose) cout << "NOT WINDOWS -- DWORD TEST NOT RUN\n"
                             "=================================\n";
#else
        if (verbose) cout << "DWORD TEST\n"
                             "==========\n";

        enum { MILLION = 1000 * 1000,
               BILLION = 1000 * MILLION };

        DWORD d;

        const Int64 maxSec  = uintMax / 1000;
        const int   maxNSec = (uintMax % 1000) * MILLION;

        ASSERT(uintMax == maxSec * 1000 + maxNSec / MILLION);

        for (int ns = BILLION - 1; ns >= 0; ns -= MILLION) {
            bdet_TimeInterval ti(maxSec, ns);
            Obj::toMillisec(&d, ti);

            Int64 expected = ns > maxNSec ? uintMax
                                          : 1000 * maxSec + ns / MILLION;
            ASSERTV(uintMax, maxNSec, ns, d, expected, (Int64) d == expected);
        }

        for (Int64 i = maxSec - 1000; i < maxSec + 1000; ++i) {
            bdet_TimeInterval ti(i, 0);
            Obj::toMillisec(&d, ti);

            ASSERT((Int64) d == (i > maxSec ? uintMax : i * 1000));
        }

        Int64 nsDiv = maxNSec / MILLION + 1;
        ASSERT(nsDiv < 1000);
        for (Int64 i = maxSec - 1000; i < maxSec + 1000; ++i) {
            bdet_TimeInterval ti(i, maxNSec + MILLION);
            Obj::toMillisec(&d, ti);

            ASSERT((Int64) d == (i >= maxSec ? uintMax : i * 1000 + nsDiv));
        }

        Int64 stopAt = maxSec + 2 * MILLION;
        int ns = 0;
        for (Int64 i = 0; i < stopAt; i += 1000, ++ns) {
            bdet_TimeInterval ti(i, ns);
            Obj::toMillisec(&d, ti);

            ASSERT((Int64) d == (i < maxSec || i == maxSec && ns <= maxNSec
                               ? i * 1000 + ns / MILLION
                               : uintMax));
        }

        ns = maxNSec;
        nsDiv = ns / MILLION;
        for (Int64 i = maxSec; i >= 0; i -= 1000) {
            bdet_TimeInterval ti(i, ns);
            Obj::toMillisec(&d, ti);

            ASSERT(d == i * 1000 + nsDiv);
        }

        ns = -MILLION;
        for (Int64 i = 0; i > -i48; i -= i32, --ns) {
            bdet_TimeInterval ti(i, ns);
            Obj::toMillisec(&d, ti);

            ASSERT(d == 0);
        }

        ns = maxNSec;
        for (Int64 i = maxSec; i < -i48; i += i32, ++ns) {
            bdet_TimeInterval ti(i, ns);
            Obj::toMillisec(&d, ti);

            ASSERT(d == uintMax);
        }

        for (ns = -BILLION + MILLION; ns < BILLION; ns += MILLION / 4) {
            bdet_TimeInterval ti(0, ns);
            ASSERT(ti.nanoseconds() == ns);

            Obj::toMillisec(&d, ti);

            if (ns <= 0) {
                ASSERT(0 == d);
            }
            else {
                ASSERT(ns / MILLION >= 0);
                ASSERT((DWORD) (ns / MILLION) == d);
            }
        }

        ns = 0;
        for (ns = 0; ns < BILLION; ns += MILLION) {
            bdet_TimeInterval ti(int64Max, ns);
            Obj::toMillisec(&d, ti);

            ASSERT(d == uintMax);
        }

        ns = 0;
        for (ns = 0; ns > -BILLION; ns -= MILLION) {
            bdet_TimeInterval ti(int64Min, ns);
            Obj::toMillisec(&d, ti);

            ASSERT(d == 0);
        }
#endif
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING TIME_T
        //
        // Concerns:
        //: 1 That 'toTimeT' assigns 'Int64's to 'time_t's properly, exactly
        //:   copying values whenever possible, and correctly saturating
        //:   otherwise.
        //: 2 Note that the exact type of 'time_t' is not clearly specified
        //:   and may vary with the platform.  It is also unclear whether it
        //:   is a 4 or 8 bit quantify, so we have to set up separate tests
        //:   for every case
        //
        // Plan:
        //: 1 32-bit signed
        //:   o Iterate over the range of values that can be exactly assigned,
        //:     incrementing in 64K increments, being sure to include the min
        //:     and max possible values.
        //:   o iterate from the max exactly assignable value to the max of
        //:     of the input range, in iterations of (1 << 48), observing
        //:     saturation.
        //:   o iterate from the min exactly assignable value to the min of
        //:     of the input range, in iterations of (1 << 48), observing
        //:     saturation.
        //:   o test the exact min and max values of the input range.
        //: 2 64-bit signed
        //:   o iterate from the min of the input range to the max of the
        //:     input range in increments of (1 << 48), observing that values
        //:     are assigned without modification.
        //:   o iterate from the max of the input range to the min of te
        //:     input range in increments of (1 << 48), observing that values
        //:     are assigned without modification.
        //: 3 32-bit unsigned
        //:   o Iterate over the range of values that can be exactly assigned,
        //:     incrementing in 64K increments, being sure to include the min
        //:     and max possible values.
        //:   o iterate from the max value that can be accurately copied to
        //:     the max of the input range in (1 << 48) increments, observing
        //:     saturation.
        //:   o iterate from 0 to the min of the input range in (1 << 48)
        //:     increments, observing saturation.
        //:   o test the exact min and max of the input range, observing
        //:     saturation.
        //: 4 64-bit unsigned
        //:   o traverse the input range in (1 << 48) increments, taking care
        //:     to include both the absolute min and max values, observing
        //:     that non-negative values are copied without modification,
        //:     and negative values are satuated as 0.
        // --------------------------------------------------------------------

#ifndef BCES_PLATFORM_POSIX_THREADS
        if (verbose) cout << "NOT UNIX -- NO TESTING OF TIME_T\n";
#else
        if (verbose) cout << "TESTING TIME_T\n"
                             "==============\n";

        bsl::time_t tt;
        int ct;

        tt = -1;

        if (verbose) {
            P_((tt < 0));    P(sizeof(bsl::time_t));
        }

        if (tt < 0 && sizeof(tt) == 4) {
            ct = 0;
            for (Int64 i = intMin; i <= intMax; i += i16, ++ct) {
                Obj::toTimeT(&tt, i);

                ASSERT((Int64) tt == i);
            }
            ASSERT(ct > 65000);
            ct = 0;
            for (Int64 i = intMax; i >= intMin; i -= i16, ++ct) {
                Obj::toTimeT(&tt, i);

                ASSERT((Int64) tt == i);
            }
            ASSERT(ct > 65000);
            ct = 0;
            for (Int64 i = intMax; i < int64Max - i48; i += i48, ++ct) {
                Obj::toTimeT(&tt, i);

                ASSERT((Int64) tt == intMax);
            }
            ASSERT(ct > 32000);
            ct = 0;
            for (Int64 i = intMin; i > int64Min + i48; i -= i48, ++ct) {
                Obj::toTimeT(&tt, i);

                ASSERT((Int64) tt == intMin);
            }
            ASSERT(ct > 32000);

            Obj::toTimeT(&tt, int64Max);
            ASSERT((Int64) tt == intMax);

            Obj::toTimeT(&tt, int64Min);
            ASSERT((Int64) tt == intMin);
        }
        else if (tt < 0 && sizeof(tt) == 8) {
            ct = 0;
            for (Int64 i = int64Min; i < int64Max - i48; i += i48, ++ct) {
                Obj::toTimeT(&tt, i);

                ASSERT((Int64) tt == i);
            }
            ASSERT(ct > 65000);
            ct = 0;
            for (Int64 i = int64Max; i > int64Min + i48; i -= i48, ++ct) {
                Obj::toTimeT(&tt, i);

                ASSERT((Int64) tt == i);
            }
            ASSERT(ct > 65000);
        }
        else if (tt > 0 && sizeof(tt) == 4) {
            ct = 0;
            for (Int64 i = 0; i <= uintMax; i += i16, ++ct) {
                Obj::toTimeT(&tt, i);

                ASSERT(i >= 0 && (Uint64) tt == (Uint64) i);
            }
            ASSERT(ct > 65000);
            ct = 0;
            for (Int64 i = uintMax; i >= 0; i -= i16, ++ct) {
                Obj::toTimeT(&tt, i);

                ASSERT(i >= 0 && (Uint64) tt == (Uint64) i);
            }
            ASSERT(ct > 65000);
            ct = 0;
            for (Int64 i = uintMax; i < int64Max - i48; i += i48, ++ct) {
                Obj::toTimeT(&tt, i);

                ASSERT((Uint64) tt == (Uint64) uintMax);
            }
            ASSERT(ct > 32000);
            ct = 0;
            for (Int64 i = 0; i > int64Min + i48; i -= i48, ++ct) {
                Obj::toTimeT(&tt, i);

                ASSERT(tt == 0);
            }
            ASSERT(ct > 32000);

            Obj::toTimeT(&tt, int64Max);
            ASSERT((Uint64) tt == (Uint64) uintMax);

            Obj::toTimeT(&tt, int64Min);
            ASSERT(tt == 0);
        }
        else if (tt > 0 && sizeof(tt) == 8) {
            ct = 0;
            for (Int64 i = int64Min; i < int64Max - i48; i += i48, ++ct) {
                Obj::toTimeT(&tt, i);

                if (i < 0) {
                    ASSERT(0 == tt);
                }
                else {
                    ASSERT((Uint64) tt == (Uint64) i);
                }
            }
            ASSERT(ct > 65000);
            ct = 0;
            for (Int64 i = int64Max; i > int64Min + i48; i -= i48, ++ct) {
                Obj::toTimeT(&tt, i);

                if (i < 0) {
                    ASSERT(0 == tt);
                }
                else {
                    ASSERT((Uint64) tt == (Uint64) i);
                }
            }
            ASSERT(ct > 65000);
        }
        else {
            ASSERT(0);
        }
#endif
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING TIMESPEC -- UNSIGNED TV_SEC
        //
        // Concerns:
        //: 1 That 'toTimeSpec' will properly assign a value from a
        //:   'bdet_TimeInterval' to a 'timespec', and that if values can be,
        //:   they are assigned without modification, and if not, that proper
        //:   saturation occurs.
        //: 2 In 2038, a signed, 32-bit 'tv_sec' will no longer be able to
        //:   represent the current time.  While it is anticipated that all
        //:   platforms will evolve to a 64-bit signed 'tv_sec', it is not
        //:   impossible that someone, somewhere may decide to extend the life
        //:   of 'timespec' by 68 years by simply making 'tv_sec' 32-bit
        //:   unsigned.
        //: 3 Note this code will not be tested until someone creates a
        //:   'timespec' with an unsigned 'tv_sec'.
        //
        // Plan:
        //: 1 sizeof(tv_sec) == 4
        //:   o Iterate testing for values of 'TimeInterval.seconds()' from 0
        //:     to 'uintMax' at intervals of 64K, making sure to include the
        //:     exact values of 0 and 'uintMax', varying the nanosecond field
        //:     slightly but keeping it in normal range.
        //:   o Test with values above and below the range which can be
        //:     represented with a 'timespec', and observe that both fields of
        //:     the 'timespec' are at the appropriate extreme.
        //:   o Test with 'timeInterval.second()' at its exact minimum and
        //:     maximum values, and verify the results are as they should be.
        //: 2 sizeof(tv_sec) == 8
        //:   o Iterate from the minimum to the maximum values of
        //:     'timeInteval.seconds()', verifying that 'tv_sec' is always
        //:     exactly equal to the '.seconds()' value.
        // --------------------------------------------------------------------

#ifndef BCES_PLATFORM_POSIX_THREADS
        if (verbose) cout << "NOT UNIX -- TESTING TIMESPEC DISABLED\n"
                             "=====================================\n";
#else
        enum { MILLION = 1000 * 1000,
               BILLION = 1000 * MILLION };

        timespec tm;

        tm.tv_sec = -1;
        if (tm.tv_sec < 0) {
            if (verbose) cout << "TV_SEC IS SIGNED -- NO UNSIGNED TESTING\n"
                                 "=======================================\n";

            break;
        }

        if (verbose) cout << "TESTING TIMESPEC -- UNSIGNED TV_SEC\n"
                             "===================================\n";

        int ns, ct;

        if (verbose) P(sizeof(tm.tv_sec));

        if (sizeof(tm.tv_sec) == 4) {
            ns = 500 * MILLION, ct = 0;
            for (Int64 i = 0; i <= uintMax; i += i16, ++ns, ++ct) {
                bdet_TimeInterval ti(i, ns * sign(i));

                Obj::toTimeSpec(&tm, ti);

                ASSERT(tm.tv_sec == i);
                ASSERT(tm.tv_nsec == ns * sign(i));
            }
            ASSERT(ct > 65000);
            ns = 500 * MILLION, ct = 0;
            for (Int64 i = uintMax; i >= 0; i -= i16, ++ns, ++ct) {
                bdet_TimeInterval ti(i, ns * sign(i));

                Obj::toTimeSpec(&tm, ti);

                ASSERT(tm.tv_sec == i);
                ASSERT(tm.tv_nsec == ns * sign(i));
            }
            ASSERT(ct > 65000);
            ns = 500 * MILLION, ct = 0;
            for (Int64 i = uintMax; i < int64Max - i48; i += i48, ++ns, ++ct) {
                bdet_TimeInterval ti(i, ns * sign(i));

                Obj::toTimeSpec(&tm, ti);

                ASSERT(tm.tv_sec == uintMax);
                ASSERT(tm.tv_nsec == ns * sign(i));
            }
            ASSERT(ct > 32000);
            ns = 500 * MILLION, ct = 0;
            for (Int64 i = 0; i > int64Min + i48; i -= i48, ++ns, ++ct) {
                bdet_TimeInterval ti(i, ns * sign(i));

                Obj::toTimeSpec(&tm, ti);

                ASSERT(tm.tv_sec == 0);
                ASSERT(tm.tv_nsec == 0);
            }
            ASSERT(ct > 32000);

            {
                bdet_TimeInterval ti;

                ti.setInterval(int64Max, 310);
                Obj::toTimeSpec(&tm, ti);

                ASSERT(tm.tv_sec  == uintMax);
                ASSERT(tm.tv_nsec == 310);

                ti.setInterval(int64Min, -237);
                Obj::toTimeSpec(&tm, ti);

                ASSERT(tm.tv_sec  == 0);
                ASSERT(tm.tv_nsec == 0);
            }
        }
        else {
            ASSERT(sizeof(tm.tv_sec) == 8);

            ns = 500 * MILLION, ct = 0;
            for (Int64 i = int64Min; i < int64Max - i48; i += i48, ++ns, ++ct){
                bdet_TimeInterval ti(i, ns * sign(i));

                Obj::toTimeSpec(&tm, ti);

                ASSERT(tm.tv_sec == i);
                ASSERT(tm.tv_nsec == ns * sign(i));
            }
            ASSERT(ct > 65000);
            ns = 500 * MILLION, ct = 0;
            for (Int64 i = int64Max; i > int64Min + i48; i -= i48, ++ns, ++ct){
                bdet_TimeInterval ti(i, ns * sign(i));

                Obj::toTimeSpec(&tm, ti);

                if (i >= 0) {
                    ASSERT(tm.tv_sec == i);
                    ASSERT(tm.tv_nsec == ns);
                }
                else {
                    ASSERT(tm.tv_sec  == 0);
                    ASSERT(tm.tv_nsec == 0);
                }
            }
            ASSERT(ct > 65000);

            {
                bdet_TimeInterval ti(int64Max, ns);

                Obj::toTimeSpec(&tm, ti);

                ASSERT((Uint64) tm.tv_sec == (Uint64) int64Max);

                ASSERT(tm.tv_nsec == ns);
            }

            {
                bdet_TimeInterval ti(int64Min, -ns);

                Obj::toTimeSpec(&tm, ti);

                ASSERT(tm.tv_sec  == 0);
                ASSERT(tm.tv_nsec == 0);
            }
        }
#endif
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING TIMESPEC -- SIGNED TV_SEC
        //
        // Concerns:
        //: o That 'toTimeSpec' will properly assign a value from a
        //:   'bdet_TimeInterval' to a 'timespec', and that if values can be,
        //:    they are assigned without modification, and if not, that proper
        //:    saturation occurs.
        //
        // Plan:
        //: 1 sizeof(tv_sec) == 4
        //:   o Iterate testing for values of 'TimeInterval.seconds()' from
        //:     'intMin' to 'intMax' at intervals of 64K, making sure to
        //:     include the exact values of 'intMin' and 'intMax', varying the
        //:     nanosecond field slightly but keeping it in normal range.
        //:   o Test with values above and below the range which can be
        //:     represented with a 'timespec', and observe that both fields of
        //:     the 'timespec' are at the appropriate extreme.
        //:   o Test with 'timeInterval.second()' at its exact minimum and
        //:     maximum values, and verify the results are as they should be.
        //: 1 sizeof(tv_sec) == 8
        //:   o Iterate from the minimum to the maximum values of
        //:     'timeInteval.seconds()', verifying that 'tv_sec' is always
        //:     exactly equal to the '.seconds()' value.
        // --------------------------------------------------------------------

#ifndef BCES_PLATFORM_POSIX_THREADS
        if (verbose) cout << "NOT UNIX -- TESTING TIMESPEC DISABLED\n"
                             "=====================================\n";
#else
        enum { MILLION = 1000 * 1000,
               BILLION = MILLION * 1000 };

        timespec tm;

        tm.tv_sec = -1;
        if (tm.tv_sec > 0) {
            if (verbose) cout << "TV_SEC IS UNSIGNED -- NO SIGNED TESTING\n"
                                 "=======================================\n";

            break;
        }

        if (verbose) cout << "TESTING TIMESPEC -- SIGNED TV_SEC\n"
                             "=================================\n";

        if (verbose) P(sizeof(tm.tv_sec));

        int ns, ct;

        if (sizeof(tm.tv_sec) == 4) {
            ns = 500 * MILLION, ct = 0;
            for (Int64 i = intMin; i <= intMax; i += i16, ++ns, ++ct) {
                bdet_TimeInterval ti(i, ns * sign(i));

                Obj::toTimeSpec(&tm, ti);

                ASSERT(tm.tv_sec == i);
                ASSERT(tm.tv_nsec == ns * sign(i));

                ti.setInterval(i, 0);
                Obj::toTimeSpec(&tm, ti);

                ASSERT(tm.tv_sec == i);
                ASSERT(tm.tv_nsec == 0);
            }
            ASSERT(ct > 65000);
            ns = 500 * MILLION, ct = 0;
            for (Int64 i = intMax; i >= intMin; i -= i16, ++ns, ++ct) {
                bdet_TimeInterval ti(i, ns * sign(i));

                Obj::toTimeSpec(&tm, ti);

                ASSERTV(tm.tv_sec, i,   tm.tv_sec  ==  i);
                ASSERTV(tm.tv_nsec, ns, tm.tv_nsec == ns * sign(i));

                ti.setInterval(i, 0);
                Obj::toTimeSpec(&tm, ti);

                ASSERT(tm.tv_sec == i);
                ASSERT(tm.tv_nsec == 0);
            }
            ASSERT(ct > 65000);
            ns = 0, ct = 0;
            for (Int64 i = intMax; i < int64Max - i48; i += i48, ++ns, ++ct) {
                bdet_TimeInterval ti(i, ns * sign(i));

                Obj::toTimeSpec(&tm, ti);

                ASSERT(tm.tv_sec == intMax);
                ASSERT(tm.tv_nsec == (i == intMax ? ns : BILLION - 1));

                ti.setInterval(i, 0);
                Obj::toTimeSpec(&tm, ti);

                ASSERT(tm.tv_sec == intMax);
                ASSERT(tm.tv_nsec == (i == intMax ? 0 : BILLION - 1));
            }
            ASSERT(ct > 32000);
            ns = 500 * MILLION, ct = 0;
            for (Int64 i = intMin; i > int64Min + i48; i -= i48, ++ns, ++ct) {
                bdet_TimeInterval ti(i, ns * sign(i));

                Obj::toTimeSpec(&tm, ti);

                ASSERT(tm.tv_sec == intMin);
                ASSERT(tm.tv_nsec == (i == intMin ? ns * sign(i)
                                                  : -(BILLION - 1)));

                ti.setInterval(i, 0);
                Obj::toTimeSpec(&tm, ti);

                ASSERT(tm.tv_sec == intMin);
                ASSERT(tm.tv_nsec == (i == intMin ? 0
                                                  : -(BILLION - 1)));
            }
            ASSERT(ct > 32000);

            {
                bdet_TimeInterval ti;

                ns = 500 * MILLION;;
                ti.setInterval(int64Max, ns);
                Obj::toTimeSpec(&tm, ti);

                ASSERT(tm.tv_sec  == intMax);
                ASSERT(tm.tv_nsec == BILLION - 1);

                ti.setInterval(int64Min, -ns);
                Obj::toTimeSpec(&tm, ti);

                ASSERT(tm.tv_sec  == intMin);
                ASSERT(tm.tv_nsec == -(BILLION - 1));
            }
        }
        else {
            ASSERTV(sizeof(tm.tv_sec), sizeof(tm.tv_sec) == 8);

            ns = 500 * MILLION, ct = 0;
            for (Int64 i = int64Min; i < int64Max - i48; i += i48, ++ns, ++ct){
                bdet_TimeInterval ti(i, ns * sign(i));

                Obj::toTimeSpec(&tm, ti);

                ASSERT(tm.tv_sec  == i);
                ASSERT(tm.tv_nsec == ns * sign(i));
            }
            ASSERT(ct > 65000);
            ns = 500 * MILLION, ct = 0;
            for (Int64 i = int64Max; i > int64Min + i48; i -= i48, ++ns, ++ct){
                bdet_TimeInterval ti(i, ns * sign(i));

                Obj::toTimeSpec(&tm, ti);

                ASSERT(tm.tv_sec == i);
                ASSERT(tm.tv_nsec == ns * sign(i));
            }
            ASSERT(ct > 65000);

            {
                bdet_TimeInterval ti(int64Max, ns);

                Obj::toTimeSpec(&tm, ti);

                ASSERT(tm.tv_sec  == int64Max);
                ASSERT(tm.tv_nsec == ns);
            }

            {
                bdet_TimeInterval ti(int64Min, -ns);

                Obj::toTimeSpec(&tm, ti);

                ASSERT(tm.tv_sec  == int64Min);
                ASSERT(tm.tv_nsec == -ns);
            }
        }
#endif
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //
        // Concerns:
        //
        // Plan:
        // --------------------------------------------------------------------

        if (verbose) cout << "BREATHING TEST\n"
                             "==============\n";

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
//      Copyright (C) Bloomberg L.P., 2013
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
