// bcemt_default.t.cpp                                                -*-C++-*-
#include <bcemt_default.h>

#include <bsl_cstdlib.h>  // atoi()
#include <bsl_iostream.h>
#include <bsl_limits.h>   // INT_MAX

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              OVERVIEW
//                              --------
// This program tests the functionality of the 'bcemt_Default' class.
//
//-----------------------------------------------------------------------------
// [1] Breathing Test
// [2] recommendedDefaultThreadStackSize
// [3] nativeDefaultThreadStackSize
// [3] setDefaultThreadStackSize
// [3] defaultThreadStackSize
// [3] Usage
// [4] results constant across multiple calls
//=============================================================================
//                    STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------

static int testStatus = 0;

static void aSsErT(int c, const char *s, int i) {
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

//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------

#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", " << flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

typedef bcemt_Default Obj;

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    // int veryVerbose = argc > 3;
    // int veryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:
      case 4: {
        // --------------------------------------------------------------------
        // MULTIPLE CALLS YIELD SAME RESULT
        //
        // Concern:
        //   Some of these calls may cache their results.  Make sure they are
        //   returning the same value when called multiple times.
        //
        // Plan:
        //   Call the routines multiple times and compare values.
        // --------------------------------------------------------------------

        if (verbose) cout << "MULTIPLE CALLS YIELD SAME RESULT\n"
                             "================================\n";

        const int native = Obj::nativeDefaultThreadStackSize();
        ASSERT(native > 0);

        for (int i = 0; i < 10; ++i) {
            ASSERT(native == Obj::nativeDefaultThreadStackSize());
            ASSERT(native == Obj::defaultThreadStackSize());
        }

        const int setSize = native * 2;

        Obj::setDefaultThreadStackSize(setSize);

        for (int i = 0; i < 10; ++i) {
            ASSERT(setSize == Obj::defaultThreadStackSize());
        }

        Obj::setDefaultThreadStackSize(native);

        for (int i = 0; i < 10; ++i) {
            ASSERT(native == Obj::defaultThreadStackSize());
        }
      }  break;
      case 3: {
        // --------------------------------------------------------------------
        // SETTING THE THREAD STACK SIZE / USAGE
        //
        // Concern:
        //   That 'setDefaultThreadStackSize' is able to set the default thread
        //   stack size.
        //
        // Plan:
        //   Call 'setDefaultThreadStackSize' with a value that is not equal to
        //   'nativeDefaultThreadStackSize' and then verify that the result of
        //   'defaultThreadStackSize' has been set to the new value.
        // --------------------------------------------------------------------

        if (verbose) cout << "SETTING THE THREAD STACK SIZE\n"
                             "=============================\n";

        // First, we examine the native thread stack size:

        const int nativeDefault =
                                 bcemt_Default::nativeDefaultThreadStackSize();
        ASSERT(nativeDefault > 0);

        // Then, we verify that when 'defaultThreadStackSize' is called, it
        // returns the native size:

        ASSERT(bcemt_Default::defaultThreadStackSize() == nativeDefault);

        // Next, we define 'newDefaultStackSize' to some size other than the
        // native default size:

        const int newDefaultStackSize = nativeDefault * 2;

        // Now, we set the default size to the new size:

        bcemt_Default::setDefaultThreadStackSize(newDefaultStackSize);

        // Finally, we verify that the default thread stack size has been set
        // to the value we specified:

        ASSERT(bcemt_Default::defaultThreadStackSize() == newDefaultStackSize);
        ASSERT(bcemt_Default::defaultThreadStackSize() != nativeDefault);
      }  break;
      case 2: {
        // --------------------------------------------------------------------
        // RECOMMENDED THREAD STACK SIZE
        //
        // Concern:
        //   That 'recommendedThreadStackSize' returns a reasonable value and
        //   that the stack size can be set to that value on all platforms.
        //
        // Plan:
        //   Call 'recommendedDefaultThreadStackSize' and verify that the value
        //   is non-negative.
        // --------------------------------------------------------------------

        if (verbose) cout << "RECOMMENDED THREAD STACK SIZE\n"
                             "=============================\n";

        const int recommended = Obj::recommendedDefaultThreadStackSize();
        ASSERT(recommended > 0);
        ASSERT(recommended < INT_MAX);

        Obj::setDefaultThreadStackSize(recommended);

        ASSERT(Obj::defaultThreadStackSize() == recommended);
      }  break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //
        // Concern:
        //   That, if 'setDefaultThreadStackSize' hasn't been called, that
        //   'defaultThreadStackSize' equals 'nativeThreadStackSize'.o
        //
        // Plan:
        //   Call 'defaultThreadStackSize' and 'nativeThreadStackSize' without
        //   having called  'setDefaultThreadStackSize', and verify that they
        //   return the same value.
        // --------------------------------------------------------------------

        if (verbose) cout << "BREATHING TEST\n"
                             "==============\n";

        const bsl::size_t defaultSize = Obj::defaultThreadStackSize();
        ASSERT(Obj::nativeDefaultThreadStackSize() == (int) defaultSize);

        ASSERT(defaultSize > 0);
        int maxint = bsl::numeric_limits<int>::max();
        ASSERT(defaultSize <= static_cast<bsl::size_t>(maxint));

        if (verbose) P(defaultSize);
      } break;
      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      }
    }

    return( testStatus );
}

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2002
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
