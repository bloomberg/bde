// bcec_atomicringbufferindexmanager.t.cpp       -*-C++-*-

#include <bcec_atomicringbufferindexmanager.h>

#include <bcema_testallocator.h>
#include <bslma_defaultallocatorguard.h>

#include <bsl_iostream.h>

using namespace BloombergLP;
using namespace bsl;

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

static int verbose;
static int veryVerbose;
static int veryVeryVerbose;

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

//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", "<< flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number
#define T_()  cout << '\t' << flush;          // Print tab w/o newline
#define V(X) { if (verbose) P(X) }            // Print in verbose mode

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

typedef bcec_AtomicRingBufferIndexManager Obj;

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    verbose = argc > 2;
    veryVerbose = argc > 3;
    veryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;;

    switch (test) { case 0:  // Zero is always the leading case.
      case 2: {
        // --------------------------------------------------------------------
        // ALLOCATOR TEST
        //
        // Test proper usage of the allocator
        // --------------------------------------------------------------------

        bcema_TestAllocator ta(veryVeryVerbose);
        bcema_TestAllocator da(veryVeryVerbose);
        bslma::DefaultAllocatorGuard defaultAllocGuard(&da);
        {        
            Obj mX(1, &ta);
            ASSERT(0 == da.numBytesInUse());
        }     
        ASSERT(0 == da.numBytesInUse());
        ASSERT(0 < ta.numAllocations());
        ASSERT(0 == ta.numBytesInUse());
        
      } break;

      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        // --------------------------------------------------------------------

        bcema_TestAllocator ta(veryVeryVerbose);
        {
            Obj mX(1, &ta);
            ASSERT(mX.isEnabled());
            ASSERT(0 == mX.length());
            ASSERT(1 == mX.capacity());

            bsl::size_t generation, index;
            ASSERT(0 == mX.acquirePushIndex(&generation, &index));
            mX.releasePushIndex(generation, index);
            ASSERT(1 == mX.length());
            ASSERT(0 == generation);
            ASSERT(0 == index);
            ASSERT(0 != mX.acquirePushIndex(&generation, &index));
            ASSERT(1 == mX.length());
            
            generation = index = 1;
            ASSERT(0 == mX.acquirePopIndex(&generation, &index));            
            mX.releasePopIndex(generation, index);
            ASSERT(0 == mX.length());
            ASSERT(0 == generation);
            ASSERT(0 == index);
            ASSERT(0 != mX.acquirePopIndex(&generation, &index));            
            ASSERT(0 == mX.length());
            ASSERT(1 == generation);
            ASSERT(0 == index);

            mX.disable();
            ASSERT(!mX.isEnabled());
            ASSERT(0 != mX.acquirePushIndex(&generation, &index));
            ASSERT(0 == mX.length());

            generation = index = 1;
            mX.enable();
            ASSERT(mX.isEnabled());
            ASSERT(0 == mX.acquirePushIndex(&generation, &index));
            mX.releasePushIndex(generation, index);
            ASSERT(1 == mX.length());
            ASSERT(1 == generation);
            ASSERT(0 == index);            
        }
        ASSERT(0 < ta.numAllocations());
        ASSERT(0 == ta.numBytesInUse());

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

