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
        if (verbose) cout << "UNIX -- USAGE NOT TESTED\n"
                             "========================\n";
#else
        if (verbose) cout << "TESTING USAGE EXAMPLE\n"
                             "=====================\n";

        // Suppose we have a 'timespec' which is a popular way to store a time
        // quantify on Unix.  It has two fields, 'tv_sec' which indicates
        // seconds, and 'tv_nsec', which indicates nanonseconds.  The size
        // of these fields can vary depending up on the platform for the
        // purposes of our usage example they are both 32 bit signed values.

        timespec tm;
        if (sizeof(tm.tv_sec) != sizeof(int)) {
            cout << "TV_SEC != 32 BITS -- NO TEST\n"
                    "============================\n";
        }
        ASSERT(sizeof(tm.tv_sec) == sizeof(int));   // tm.tv_sec (on this
                                                    // platform) is 'int' or 32
                                                    // bit 'long'
        tm.tv_sec = -1;
        if (tm.tv_sec > 0) {
            cout << "TV_SEC NOT SIGNED -- NO TEST\n"
                    "============================\n";
            break;
        }
        ASSERT(tm.tv_sec < 0);                      // 'tm.tv_sec' is signed on
                                                    // this platform.
        tm.tv_nsec = -1;
        ASSERTV(tm.tv_nsec < 0);                    // 'tm.tv_nsec' is signed
                                                    // on this platform.

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
        // --------------------------------------------------------------------

#ifndef BCES_PLATFORM_POSIX_THREADS
        if (verbose) cout << "NOT UNIX -- NO TESTING OF TIME_T\n";
#else
        if (verbose) cout << "TESTING TIME_T\n"
                             "==============\n";

        bsl::time_t tt;

        tt = -1;
        if (tt < 0 && sizeof(tt) == 4) {
            for (Int64 i = intMin; i <= intMax; i += i16) {
                Obj::toTimeT(&tt, i);

                ASSERT(tt == i);
            }

            for (Int64 i = intMax; i >= intMin; i -= i16) {
                Obj::toTimeT(&tt, i);

                ASSERT(tt == i);
            }

            for (Int64 i = intMax; i < int64Max - i48; i += i48) {
                Obj::toTimeT(&tt, i);

                ASSERT(tt == intMax);
            }

            for (Int64 i = intMin; i > int64Min + i48; i -= i48) {
                Obj::toTimeT(&tt, i);

                ASSERT(tt == intMin);
            }

            Obj::toTimeT(&tt, int64Max);
            ASSERT(tt == intMax);

            Obj::toTimeT(&tt, int64Min);
            ASSERT(tt == intMin);
        }
        else if (tt < 0 && sizeof(tt) == 8) {
            for (Int64 i = int64Min; i < int64Max - i48; i += i48) {
                Obj::toTimeT(&tt, i);

                ASSERT(tt == i);
            }

            for (Int64 i = int64Max; i > int64Min + i48; i -= i48) {
                Obj::toTimeT(&tt, i);

                ASSERT(tt == i);
            }
        }
        else if (tt > 0 && sizeof(tt) == 4) {
            for (Int64 i = 0; i <= uintMax; i += i16) {
                Obj::toTimeT(&tt, i);

                ASSERT(tt == i);
            }

            for (Int64 i = uintMax; i >= 0; i -= i16) {
                Obj::toTimeT(&tt, i);

                ASSERT(tt == i);
            }

            for (Int64 i = uintMax; i < int64Max - i48; i += i48) {
                Obj::toTimeT(&tt, i);

                ASSERT(tt == uintMax);
            }

            for (Int64 i = 0; i > int64Min + i48; i -= i48) {
                Obj::toTimeT(&tt, i);

                ASSERT(tt == 0);
            }

            Obj::toTimeT(&tt, int64Max);
            ASSERT(tt == uintMax);

            Obj::toTimeT(&tt, int64Min);
            ASSERT(tt == 0);
        }
        else if (tt > 0 && sizeof(tt == 8)) {
            for (Int64 i = int64Min; i < int64Max - i48; i += i48) {
                Obj::toTimeT(&tt, i);

                if (i < 0) {
                    ASSERT(0 == tt);
                }
                else {
                    ASSERT(tt == i);
                }
            }

            for (Int64 i = int64Max; i > int64Min + i48; i -= i48) {
                Obj::toTimeT(&tt, i);

                if (i < 0) {
                    ASSERT(0 == tt);
                }
                else {
                    ASSERT(tt == i);
                }
            }
        }
        else {
            ASSERT(0);
        }
#endif
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING TIMESPEC -- UNSIGNED TV_SEC
        // --------------------------------------------------------------------

#ifndef BCES_PLATFORM_POSIX_THREADS
        if (verbose) cout << "TESTING TIMESPEC DISABLED -- NOT UNIX\n"
                             "=====================================\n";
#else
        if (verbose) cout << "TESTING TIMESPEC -- UNSIGNED TV_SEC\n"
                             "===================================\n";

        timespec tm;

        tm.tv_sec = -1;
        if (tm.tv_sec < 0) {
            if (verbose) cout << "TV_SEC IS SIGNED -- NO TESTING\n";

            break;
        }

        int ns;

        if (sizeof(tm.tv_sec) == 4) {
            ns = 0;

            for (Int64 i = 0; i <= uintMax; i += i16, ++ns) {
                bdet_TimeInterval ti(i, ns * sign(i));

                Obj::toTimeSpec(&tm, ti);

                ASSERT(tm.tv_sec == i);
                ASSERT(tm.tv_nsec == ns * sign(i));
            }
            ns = 0;
            for (Int64 i = uintMax; i >= 0; i -= i16, ++ns) {
                bdet_TimeInterval ti(i, ns * sign(i));

                Obj::toTimeSpec(&tm, ti);

                ASSERT(tm.tv_sec == i);
                ASSERT(tm.tv_nsec == ns * sign(i));
            }
            ns = 0;
            for (Int64 i = uintMax; i < int64Max - i48; i += i48, ++ns) {
                bdet_TimeInterval ti(i, ns * sign(i));

                Obj::toTimeSpec(&tm, ti);

                ASSERT(tm.tv_sec == uintMax);
                ASSERT(tm.tv_nsec == ns * sign(i));
            }
            ns = 0;
            for (Int64 i = 0; i > int64Min + i48; i -= i48, ++ns) {
                bdet_TimeInterval ti(i, ns * sign(i));

                Obj::toTimeSpec(&tm, ti);

                ASSERT(tm.tv_sec == 0);
                ASSERT(tm.tv_nsec == 0);
            }

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

            ns = 0;
            for (Int64 i = int64Min; i < int64Max - i48; i += i48, ++ns) {
                bdet_TimeInterval ti(i, ns * sign(i));

                Obj::toTimeSpec(&tm, ti);

                ASSERT(tm.tv_sec == i);
                ASSERT(tm.tv_nsec == ns * sign(i));
            }

            ns = 0;
            for (Int64 i = int64Max; i > int64Min + i48; i -= i48, ++ns) {
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
        }
#endif
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING TIMESPEC -- SIGNED TIME_T
        // --------------------------------------------------------------------

#ifndef BCES_PLATFORM_POSIX_THREADS
        if (verbose) cout << "TESTING TIMESPEC DISABLED -- NOT UNIX\n"
                             "=====================================\n";
#else
        if (verbose) cout << "TESTING TIMESPEC -- SIGNED TV_SEC\n"
                             "=================================\n";

        enum { BILLION = 1000 * 1000 * 1000 };

        timespec tm;

        tm.tv_sec = -1;
        if (tm.tv_sec > 0) {
            if (verbose) cout << "TV_SEC IS UNSIGNED -- NO TESTING\n";

            break;
        }

        int ns;

        if (sizeof(tm.tv_sec) == 4) {
            ns = 0;
            for (Int64 i = intMin; i <= intMax; i += i16, ++ns) {
                bdet_TimeInterval ti(i, ns * sign(i));

                Obj::toTimeSpec(&tm, ti);

                ASSERT(tm.tv_sec == i);
                ASSERT(tm.tv_nsec == ns * sign(i));
            }
            ns = 0;
            for (Int64 i = intMax; i >= intMin; i -= i16, ++ns) {
                bdet_TimeInterval ti(i, ns * sign(i));

                Obj::toTimeSpec(&tm, ti);

                ASSERTV(tm.tv_sec, i,   tm.tv_sec  ==  i);
                ASSERTV(tm.tv_nsec, ns, tm.tv_nsec == ns * sign(i));
            }
            ns = 100;
            for (Int64 i = intMax; i < int64Max - i48; i += i48, ++ns) {
                bdet_TimeInterval ti(i, ns * sign(i));

                Obj::toTimeSpec(&tm, ti);

                ASSERT(tm.tv_sec == intMax);
                ASSERT(tm.tv_nsec == (i == intMax ? ns : BILLION - 1));
            }
            ns = 0;
            for (Int64 i = intMin; i > int64Min + i48; i -= i48, ++ns) {
                bdet_TimeInterval ti(i, ns * sign(i));

                Obj::toTimeSpec(&tm, ti);

                ASSERT(tm.tv_sec == intMin);
                ASSERT(tm.tv_nsec == (i == intMin ? ns * sign(i)
                                                  : -(BILLION - 1)));
            }

            {
                bdet_TimeInterval ti;

                ti.setInterval(int64Max, 310);
                Obj::toTimeSpec(&tm, ti);

                ASSERT(tm.tv_sec  == intMax);
                ASSERT(tm.tv_nsec == BILLION - 1);

                ti.setInterval(int64Min, -237);
                Obj::toTimeSpec(&tm, ti);

                ASSERT(tm.tv_sec  == intMin);
                ASSERT(tm.tv_nsec == -(BILLION - 1));
            }
        }
        else {
            ASSERT(sizeof(tm.tv_sec) == 8);

            ns = 0;
            for (Int64 i = int64Min; i < int64Max - i48; i += i48, ++ns) {
                bdet_TimeInterval ti(i, ns * sign(i));

                Obj::toTimeSpec(&tm, ti);

                ASSERT(tm.tv_sec  == i);
                ASSERT(tm.tv_nsec == ns * sign(i));
            }

            ns = 0;
            for (Int64 i = int64Max; i > int64Min + i48; i -= i48, ++ns) {
                bdet_TimeInterval ti(i, ns * sign(i));

                Obj::toTimeSpec(&tm, ti);

                ASSERT(tm.tv_sec == i);
                ASSERT(tm.tv_nsec == ns * sign(i));
            }
        }
#endif
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //
        // Plan:
        //
        // Testing:
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
