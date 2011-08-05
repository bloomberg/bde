// bdema_heapbypassallocator.t.cpp                                    -*-C++-*-
#include <bdema_heapbypassallocator.h>

#include <bsl_iostream.h>

#include <bsl_cstdlib.h>    // atoi

using namespace BloombergLP;
using bsl::cin;
using bsl::cout;
using bsl::cerr;
using bsl::endl;

//=============================================================================
//                                 TEST PLAN
//-----------------------------------------------------------------------------

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
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " \
                    << J << "\t" \
                    << #K << ": " << K <<  "\n"; aSsErT(1, #X, __LINE__); } }

//=============================================================================
//                       SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------

#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", "<< flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number
#define T_()  cout << "\t" << flush;          // Print tab w/o newline

//=============================================================================
//                      GLOBAL HELPER #DEFINES FOR TESTING
//-----------------------------------------------------------------------------

//=============================================================================
//                  GLOBAL HELPER TYPES & CLASSES FOR TESTING
//-----------------------------------------------------------------------------

//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

//=============================================================================
//                             USAGE EXAMPLES
//-----------------------------------------------------------------------------

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? bsl::atoi(argv[1]) : 0;
    int verbose = argc > 2;
    //  int veryVerbose = argc > 3;
    //  int veryVeryVerbose = argc > 4;

    switch (test) { case 0:
      case 2: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << "bdema_HeapBypassAllocator usage example\n"
                             "=======================================\n";

        // Here we allocate some memory using a heap bypass allocator, then
        // write to that memory, then read from it and verify the values
        // written are preserved.

        enum {
            LENGTH = 10 * 1000,
            NUM_SEGMENTS = 60
        };

        bdema_HeapBypassAllocator hbpa;

        // First we allocate some segments

        char *segments[NUM_SEGMENTS];
        for (int i = 0; i < NUM_SEGMENTS; ++i) {
            segments[i] = static_cast<char *>(hbpa.allocate(LENGTH));
            BSLS_ASSERT(segments[i]);
        }

        // Next we write to the segments.

        char c = 'a';
        for (int i = 0; i < NUM_SEGMENTS; ++i) {
            char *segment = segments[i];
            for (int j = 0; j < LENGTH; ++j) {
                c = (c + 1) & 0x7f;
                segment[j] = c;
            }
        }

        // Finally, we read from the segments and verify the written data is
        // still there.

        c = 'a';
        for (int i = 0; i < NUM_SEGMENTS; ++i) {
            char *segment = segments[i];
            for (int j = 0; j < LENGTH; ++j) {
                c = (c + 1) & 0x7f;
                BSLS_ASSERT(segment[j] == c);
            }
        }

        // Memory is released upon destruction of object 'hbpa' when it goes
        // out of scope.
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // bdema_HeapBypassAllocator BREATHING TEST
        //
        // Concerns:
        //   Exercise heap bypass allocator basic functionality.
        //
        // Plan:
        //   Create a heap bypass allocator, do some allocations, then destroy
        //   the object.
        // --------------------------------------------------------------------

        if (verbose) cout << "bdema_HeapBypassAllocator breathing test\n"
                             "========================================\n";

        enum {
            LENGTH = 10 * 1000,
            NUM_SEGMENTS = 20
        };

        bdema_HeapBypassAllocator hbpa;

        char *segments[NUM_SEGMENTS];
        for (int i = 0; i < NUM_SEGMENTS; ++i) {
            segments[i] = static_cast<char *>(hbpa.allocate(LENGTH));
            char *pcB = segments[i];

            const char C = 'a' + i;
            for (char *pcE = pcB + LENGTH, *pc = pcB; pc < pcE; ++pc) {
                *pc = C;
            }
        }

        for (int i = 0; i < NUM_SEGMENTS; ++i) {
            const char *pcB = segments[i];

            const char C = 'a' + i;
            for (const char *pcE = pcB + LENGTH, *pc = pcB; pc < pcE; ++pc) {
                ASSERT(C == *pc);
            }
        }
      } break;
      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        cerr << "Error, non-zero test status = " << testStatus << "."
             << endl;
    }
    return testStatus;
}

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2010
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
