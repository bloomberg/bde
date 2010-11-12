// bdecs_finiteautomatonutil.t.cpp     -*-C++-*-

#include <bdecs_finiteautomatonutil.h>
#include <bdecs_finiteautomaton.h>

#include <bdes_platformutil.h>                  // for testing only
#include <bdema_bufferallocator.h>              // for testing only
#include <bdema_testallocator.h>                // for testing only
#include <bdema_testallocatorexception.h>       // for testing only
#include <bdex_testoutstream.h>                 // for testing only
#include <bdex_testinstream.h>                  // for testing only
#include <bdex_testinstreamexception.h>         // for testing only

#include <cstdlib>     // atoi()
#include <cstring>     // strlen(), memset(), memcpy(), memcmp()
#include <new>         // placement syntax
#include <iostream>
#include <strstream>

using namespace BloombergLP;
using namespace std;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// The component under test consists of a suite of static member functions
// (pure procedures) that perform basic concatenation operations on finite
// automata.
//-----------------------------------------------------------------------------
// [ 2] void repeat(bdecs_FiniteAutomaton *fa, int numMatch);
// [ 3] void repeat(bdecs_FiniteAutomaton *fa, int min, int max);
// [ 4] void repeatMin(bdecs_FiniteAutomaton *fa, int numMatch);
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 5] USAGE EXAMPLE

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
//                  STANDARD BDEMA EXCEPTION TEST MACROS
//-----------------------------------------------------------------------------

#ifdef BDE_BUILD_TARGET_EXC
#define BEGIN_BDEMA_EXCEPTION_TEST {                                       \
    {                                                                      \
        static int firstTime = 1;                                          \
        if (veryVerbose && firstTime) cout <<                              \
            "### BDEMA EXCEPTION TEST -- (ENABLED) --" << endl;            \
        firstTime = 0;                                                     \
    }                                                                      \
    if (veryVeryVerbose) cout <<                                           \
        "### Begin bdema exception test." << endl;                         \
    int bdemaExceptionCounter = 0;                                         \
    static int bdemaExceptionLimit = 100;                                  \
    testAllocator.setAllocationLimit(bdemaExceptionCounter);               \
    do {                                                                   \
        try {

#define END_BDEMA_EXCEPTION_TEST                                           \
        } catch (bdema_TestAllocatorException& e) {                        \
            if (veryVerbose && bdemaExceptionLimit || veryVeryVerbose) {   \
                --bdemaExceptionLimit;                                     \
                cout << "(*** " <<                                         \
                bdemaExceptionCounter << ')';                              \
                if (veryVeryVerbose) { cout << " BEDMA_EXCEPTION: "        \
                    << "alloc limit = " << bdemaExceptionCounter << ", "   \
                    << "last alloc size = " << e.numBytes();               \
                }                                                          \
                else if (0 == bdemaExceptionLimit) {                       \
                     cout << " [ Note: 'bdemaExceptionLimit' reached. ]";  \
                }                                                          \
                cout << endl;                                              \
            }                                                              \
            testAllocator.setAllocationLimit(++bdemaExceptionCounter);     \
            continue;                                                      \
        }                                                                  \
        testAllocator.setAllocationLimit(-1);                              \
        break;                                                             \
    } while (1);                                                           \
    if (veryVeryVerbose) cout <<                                           \
        "### End bdema exception test." << endl;                           \
}
#else
#define BEGIN_BDEMA_EXCEPTION_TEST                                         \
{                                                                          \
    static int firstTime = 1;                                              \
    if (verbose && firstTime) { cout <<                                    \
        "### BDEMA EXCEPTION TEST -- (NOT ENABLED) --" << endl;            \
        firstTime = 0;                                                     \
    }                                                                      \
}
#define END_BDEMA_EXCEPTION_TEST
#endif


//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", "<< flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

typedef bdecs_FiniteAutomaton Obj;
typedef bdecs_FiniteAutomatonAccumulator ObjAcc;

//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

int accepts(const Obj& fa, const char *s)
{
    ObjAcc faa(fa);

    while (*s) {
        faa.transition(*s);
        if (faa.isError()) { return 0; }
        ++s;
    }

    return faa.isAccepting();
}

int acceptsRepeated(const Obj& fa, const char *s, int n)
{
    ObjAcc faa(fa);
    const char *p = s;

    while (n-- > 0) {
        s = p;
        while (*s) {
            faa.transition(*s);
            if (faa.isError()) { return 0; }
            ++s;
        }
    }

    return faa.isAccepting();
}

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
    // int veryVerbose = argc > 3;
    int veryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;;

    bdema_TestAllocator testAllocator(veryVeryVerbose);

    switch (test) { case 0:  // Zero is always the leading case.
      case 5: {
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

        if (verbose) cout << endl
                          << "Testing Usage Example" << endl
                          << "=====================" << endl;

        {
            bdecs_FiniteAutomaton fa;
            fa.appendSet("xyz", 3);

            bdecs_FiniteAutomatonUtil::repeat(&fa, 5, 10);

            bdecs_FiniteAutomatonAccumulator faa(fa);
            int i = 0;

            do {
                faa.transition((i % 3) + 'x');    // 'x', 'y', or 'z'
                ASSERT(0 == faa.isAccepting());
            } while (++i < 4);

            do {
                faa.transition((i % 3) + 'x');
                ASSERT(1 == faa.isAccepting());
            } while (++i < 10);

            do {
                faa.transition((i % 3) + 'x');
                ASSERT(0 == faa.isAccepting());
            } while (++i < 25);
        }
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING REPEATMIN
        // Concerns:
        //
        // Plan: BUG
        //
        // Testing:
        //   void repeatMin(bdecs_FiniteAutomaton *fa, int numMatch);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                 << "Testing the 'repeatMin' method (thoroughly)."
                 << endl;

        static const struct {
            int         d_lineNum;  // source line number
            const char *d_str;      // source string
            int         d_min;      // min no. of repetitions
            int         d_max;      // max no. of repetitions
        } DATA[] = {
            //line   str      min     max
            //----   ------   ---     ---
            { L_,    "x",       4,    6   },
            { L_,    "xy",      2,    4   },
            { L_,    "yz",      2,    4   },
            { L_,    "zxy",     2,    4   }
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        {
            Obj fa(&testAllocator);
            fa.appendSet("xyz", 3);
            bdecs_FiniteAutomatonUtil::repeatMin(&fa, 4);

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int   LINE = DATA[ti].d_lineNum;
                const char *S    = DATA[ti].d_str;
                const int   MIN  = DATA[ti].d_min;
                const int   MAX  = DATA[ti].d_max;
                int j;
                for (j = 0; j < MIN; ++j) {
                    LOOP_ASSERT(LINE, 0 == acceptsRepeated(fa, S, j));
                }
                for (j = MIN; j <= MAX; ++j) {
                    LOOP_ASSERT(LINE, 1 == acceptsRepeated(fa, S, j));
                }
            }
        }

        {
            Obj fa(&testAllocator);
            fa.appendSet("xyz", 3);
            bdecs_FiniteAutomatonUtil::repeatMin(&fa, 0);

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int   LINE = DATA[ti].d_lineNum;
                const char *S    = DATA[ti].d_str;
                const int   MAX  = DATA[ti].d_max;
                int j;
                for (j = 0; j <= MAX; ++j) {
                    LOOP_ASSERT(LINE, 1 == acceptsRepeated(fa, S, j));
                }
            }
        }

        {
            Obj fa(&testAllocator);
            fa.appendSequence("xyz", 3);
            bdecs_FiniteAutomatonUtil::repeatMin(&fa, 1);

            ASSERT(0 == accepts(fa, ""));

            for (int j = 1; j <= 3; ++j) {
                LOOP_ASSERT(j, 1 == acceptsRepeated(fa, "xyz", j));
                LOOP_ASSERT(j, 0 == acceptsRepeated(fa, "xy", j));
                LOOP_ASSERT(j, 0 == acceptsRepeated(fa, "xyzx", j));
            }
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING REPEAT (3 ARGUMENT)
        // Concerns:
        //
        // Plan: BUG
        //
        // Testing:
        //   void repeat(bdecs_FiniteAutomaton *fa, int min, int max);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                 << "Testing the 3-argument 'repeat' method (thoroughly)."
                 << endl;

        {
            Obj fa(&testAllocator);
            fa.appendSet("xyz", 3);
            bdecs_FiniteAutomatonUtil::repeat(&fa, 0, 0);

            ASSERT(1 == accepts(fa, ""));
            ASSERT(0 == accepts(fa, "x"));
            ASSERT(0 == accepts(fa, "y"));
            ASSERT(0 == accepts(fa, "z"));
        }

        {
            Obj fa(&testAllocator);
            fa.appendSet("xyz", 3);
            bdecs_FiniteAutomatonUtil::repeat(&fa, 0, 1);

            ASSERT(1 == accepts(fa, ""));
            ASSERT(1 == accepts(fa, "x"));
            ASSERT(1 == accepts(fa, "y"));
            ASSERT(1 == accepts(fa, "z"));
            ASSERT(0 == accepts(fa, "xy"));
            ASSERT(0 == accepts(fa, "yz"));
            ASSERT(0 == accepts(fa, "zxy"));
        }

        {
            Obj fa(&testAllocator);
            fa.appendSet("xyz", 3);
            bdecs_FiniteAutomatonUtil::repeat(&fa, 1, 1);

            ASSERT(0 == accepts(fa, ""));
            ASSERT(1 == accepts(fa, "x"));
            ASSERT(1 == accepts(fa, "y"));
            ASSERT(1 == accepts(fa, "z"));
            ASSERT(0 == accepts(fa, "xy"));
            ASSERT(0 == accepts(fa, "yz"));
            ASSERT(0 == accepts(fa, "zxy"));
        }

        {
            Obj fa(&testAllocator);
            fa.appendSet("xyz", 3);
            bdecs_FiniteAutomatonUtil::repeat(&fa, 1, 2);

            ASSERT(0 == accepts(fa, ""));
            ASSERT(1 == accepts(fa, "x"));
            ASSERT(1 == accepts(fa, "y"));
            ASSERT(1 == accepts(fa, "z"));
            ASSERT(1 == accepts(fa, "xy"));
            ASSERT(1 == accepts(fa, "xz"));
            ASSERT(1 == accepts(fa, "yx"));
            ASSERT(1 == accepts(fa, "yz"));
            ASSERT(1 == accepts(fa, "zx"));
            ASSERT(1 == accepts(fa, "zy"));
            ASSERT(0 == accepts(fa, "zxy"));
            ASSERT(0 == accepts(fa, "xyzxy"));
        }

        static const struct {
            int         d_lineNum;  // source line number
            const char *d_str;      // source string
            int         d_min;      // min no. of repetitions
            int         d_max;      // max no. of repetitions
        } DATA[] = {
            //line   str      min     max
            //----   ------   ---     ---
            { L_,    "x",       2,    6   },
            { L_,    "xy",      1,    3   },
            { L_,    "yz",      1,    3   },
            { L_,    "zxy",     1,    2   }
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        {
            Obj fa(&testAllocator);
            fa.appendSet("xyz", 3);
            bdecs_FiniteAutomatonUtil::repeat(&fa, 2, 6);

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int   LINE = DATA[ti].d_lineNum;
                const char *S    = DATA[ti].d_str;
                const int   MIN  = DATA[ti].d_min;
                const int   MAX  = DATA[ti].d_max;
                int j;
                for (j = 0; j < MIN; ++j) {
                    LOOP_ASSERT(LINE, 0 == acceptsRepeated(fa, S, j));
                }
                for (j = MIN; j <= MAX; ++j) {
                    LOOP_ASSERT(LINE, 1 == acceptsRepeated(fa, S, j));
                }
                for (j = MAX+1; j <= MAX+3; ++j) {
                    LOOP_ASSERT(LINE, 0 == acceptsRepeated(fa, S, j));
                }
            }
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING REPEAT (2 ARGUMENT)
        // Concerns:
        //
        // Plan: BUG
        //
        // Testing:
        //   void repeat(bdecs_FiniteAutomaton *fa, int numMatch);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                 << "Testing the 2-argument 'repeat' method (thoroughly)."
                 << endl;

        {
            Obj fa(&testAllocator);
            fa.appendSet("xyz", 3);
            bdecs_FiniteAutomatonUtil::repeat(&fa, 0);

            ASSERT(1 == accepts(fa, ""));
            ASSERT(0 == accepts(fa, "x"));
            ASSERT(0 == accepts(fa, "y"));
            ASSERT(0 == accepts(fa, "z"));
        }

        {
            Obj fa(&testAllocator);
            fa.appendSet("xyz", 3);
            bdecs_FiniteAutomatonUtil::repeat(&fa, 1);

            ASSERT(0 == accepts(fa, ""));
            ASSERT(1 == accepts(fa, "x"));
            ASSERT(1 == accepts(fa, "y"));
            ASSERT(1 == accepts(fa, "z"));
            ASSERT(0 == accepts(fa, "xy"));
            ASSERT(0 == accepts(fa, "yz"));
            ASSERT(0 == accepts(fa, "zxy"));
        }

        {
            Obj fa(&testAllocator);
            fa.appendSet("xyz", 3);
            bdecs_FiniteAutomatonUtil::repeat(&fa, 3);

            ASSERT(0 == accepts(fa, ""));
            ASSERT(0 == accepts(fa, "x"));
            ASSERT(0 == accepts(fa, "y"));
            ASSERT(0 == accepts(fa, "z"));
            ASSERT(0 == accepts(fa, "xy"));
            ASSERT(0 == accepts(fa, "xz"));
            ASSERT(0 == accepts(fa, "yx"));
            ASSERT(0 == accepts(fa, "yz"));
            ASSERT(0 == accepts(fa, "zx"));
            ASSERT(0 == accepts(fa, "zy"));
            ASSERT(1 == accepts(fa, "xyz"));
            ASSERT(1 == accepts(fa, "xzy"));
            ASSERT(1 == accepts(fa, "yxz"));
            ASSERT(1 == accepts(fa, "yzx"));
            ASSERT(1 == accepts(fa, "zxy"));
            ASSERT(1 == accepts(fa, "xxx"));
            ASSERT(1 == accepts(fa, "yyy"));
            ASSERT(1 == accepts(fa, "zzz"));
            ASSERT(0 == accepts(fa, "zzyx"));
            ASSERT(0 == accepts(fa, "yzxx"));
            ASSERT(0 == accepts(fa, "xzyz"));
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST:
        //   Create 'bdecs_FiniteAutomaton' objects and exercise these
        //   objects using the 'bdecs_FiniteAutomatonUtil' functions.
        //
        // Testing:
        //   Exercise of basic functionality.
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "BREATHING TEST" << endl
                                  << "==============" << endl;

        if (verbose) cout << endl << "Testing repeat(fa, numMatch)" << endl;
        {
            Obj fa(&testAllocator);
            fa.appendSequence("abc", 3);

            bdecs_FiniteAutomatonUtil::repeat(&fa, 3);

            ObjAcc faa(fa);

            ASSERT(0 == faa.isAccepting());

            int i;
            for (i = 0; i < 2; ++i) {
                faa.transition('a');
                ASSERT(0 == faa.isAccepting());
                faa.transition('b');
                ASSERT(0 == faa.isAccepting());
                faa.transition('c');
                ASSERT(0 == faa.isAccepting());
            }

            faa.transition('a');
            ASSERT(0 == faa.isAccepting());
            faa.transition('b');
            ASSERT(0 == faa.isAccepting());
            faa.transition('c');
            ASSERT(1 == faa.isAccepting());

            for (i = 0; i < 4; ++i) {
                faa.transition('a');
                ASSERT(0 == faa.isAccepting());
                faa.transition('b');
                ASSERT(0 == faa.isAccepting());
                faa.transition('c');
                ASSERT(0 == faa.isAccepting());
            }
        }

        if (verbose) cout << endl
                          << "Testing repeat(fa, minNumMatch, maxNumMatch)"
                          << endl;
        {
            Obj fa(&testAllocator);
            fa.appendSequence("abc", 3);

            bdecs_FiniteAutomatonUtil::repeat(&fa, 3, 7);

            ObjAcc faa(fa);

            ASSERT(0 == faa.isAccepting());

            int i;
            for (i = 0; i < 2; ++i) {
                faa.transition('a');
                ASSERT(0 == faa.isAccepting());
                faa.transition('b');
                ASSERT(0 == faa.isAccepting());
                faa.transition('c');
                ASSERT(0 == faa.isAccepting());
            }

            for (i = 0; i < 5; ++i) {
                faa.transition('a');
                ASSERT(0 == faa.isAccepting());
                faa.transition('b');
                ASSERT(0 == faa.isAccepting());
                faa.transition('c');
                ASSERT(1 == faa.isAccepting());
            }

            for (i = 0; i < 8; ++i) {
                faa.transition('a');
                ASSERT(0 == faa.isAccepting());
                faa.transition('b');
                ASSERT(0 == faa.isAccepting());
                faa.transition('c');
                ASSERT(0 == faa.isAccepting());
            }
        }

        if (verbose) cout << endl << "Testing repeatMin(fa, numMatch)" << endl;
        {
            Obj fa(&testAllocator);
            fa.appendSequence("abc", 3);

            bdecs_FiniteAutomatonUtil::repeatMin(&fa, 3);

            ObjAcc faa(fa);

            ASSERT(0 == faa.isAccepting());

            int i;
            for (i = 0; i < 2; ++i) {
                faa.transition('a');
                ASSERT(0 == faa.isAccepting());
                faa.transition('b');
                ASSERT(0 == faa.isAccepting());
                faa.transition('c');
                ASSERT(0 == faa.isAccepting());
            }

            for (i = 0; i < 10; ++i) {
                faa.transition('a');
                ASSERT(0 == faa.isAccepting());
                faa.transition('b');
                ASSERT(0 == faa.isAccepting());
                faa.transition('c');
                ASSERT(1 == faa.isAccepting());
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
//      Copyright (C) Bloomberg L.P., 2002
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
