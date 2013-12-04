// bcec_atomicringbufferindexmanager.t.cpp       -*-C++-*-

#include <bcec_atomicringbufferindexmanager.h>

#include <bcema_testallocator.h>
#include <bslma_defaultallocatorguard.h>

#include <bsl_iostream.h>

using namespace BloombergLP;
using namespace bsl;
// ============================================================================
//                             TEST PLAN
// ----------------------------------------------------------------------------
//                             Overview
//                             --------
// ----------------------------------------------------------------------------
// PUBLIC CONSTANTS
// [  ] e_MAX_CAPACITY 
// CREATORS
// [  ] explicit
// [  ] bcec_AtomicRingBufferIndexManager(unsigned int, bslma::Allocator *);
// [  ] ~bcec_AtomicRingBufferIndexManager();
// MANIPULATORS
// [  ] int acquirePushIndex(unsigned int *, unsigned int *);
// [  ] void releasePushIndex(unsigned int , unsigned int );
// [  ] int acquirePopIndex(unsigned int *, unsigned int *);
// [  ] void releasePopIndex(unsigned int , unsigned int );
// [  ] void incrementPopIndexFrom(unsigned int );
// [  ] void disable();
// [  ] void enable();
// ACCESSORS
// [  ] bool isEnabled() const;        
// [  ] unsigned int length() const;
// [  ] unsigned int capacity() const; 
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [12] USAGE EXAMPLE

//=============================================================================
//                      STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
static int testStatus = 0;

namespace {

void aSsErT(int c, const char *s, int i)
{
    if (c) {
        bsl::cout << "Error " << __FILE__ << "(" << i << "): " << s
                  << "    (failed)" << bsl::endl;
        if (0 <= testStatus && testStatus <= 100) ++testStatus;
    }
}

}  // close anonymous namespace

#define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

//=============================================================================
//                  STANDARD BDE LOOP-ASSERT TEST MACROS
//-----------------------------------------------------------------------------
#define LOOP_ASSERT(I,X) { \
    if (!(X)) { bsl::cout << #I << ": " << I << "\n"; \
                aSsErT(1, #X, __LINE__); }}

#define LOOP2_ASSERT(I,J,X) { \
    if (!(X)) { bsl::cout << #I << ": " << I << "\t"  \
                          << #J << ": " << J << "\n"; \
                aSsErT(1, #X, __LINE__); } }

#define LOOP3_ASSERT(I,J,K,X) { \
   if (!(X)) { bsl::cout << #I << ": " << I << "\t" \
                         << #J << ": " << J << "\t" \
                         << #K << ": " << K << "\n";\
               aSsErT(1, #X, __LINE__); } }

#define LOOP0_ASSERT ASSERT
#define LOOP1_ASSERT LOOP_ASSERT

//=============================================================================
//                  STANDARD BDE VARIADIC ASSERT TEST MACROS
//-----------------------------------------------------------------------------

#define NUM_ARGS_IMPL(X5, X4, X3, X2, X1, X0, N, ...)   N
#define NUM_ARGS(...) NUM_ARGS_IMPL(__VA_ARGS__, 5, 4, 3, 2, 1, 0, "")

#define LOOPN_ASSERT_IMPL(N, ...) LOOP ## N ## _ASSERT(__VA_ARGS__)
#define LOOPN_ASSERT(N, ...)      LOOPN_ASSERT_IMPL(N, __VA_ARGS__)

#define ASSERTV(...) LOOPN_ASSERT(NUM_ARGS(__VA_ARGS__), __VA_ARGS__)

//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define P(X) bsl::cout << #X " = " << (X) << bsl::endl;
                                              // Print identifier and value.
#define Q(X) bsl::cout << "<| " #X " |>" << bsl::endl;
                                              // Quote identifier literally.
#define P_(X) bsl::cout << #X " = " << (X) << ", " << bsl::flush;
                                              // P(X) without '\n'
#define L_ __LINE__                           // current Line number
#define NL "\n"
//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

static int verbose = 0;
static int veryVerbose = 0;
static int veryVeryVerbose = 0;
static int veryVeryVeryVerbose = 0;

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
        //   This case exercises (but does not fully test) basic functionality.
        //
        // Concerns:
        //: 1 The class is sufficiently functional to enable comprehensive
        //:   testing in subsequent test cases.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BREATHING TEST" << endl
                          << "==============" << endl;

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

