// bslma_defaultallocatorguard.t.cpp                  -*-C++-*-

#include <bslma_defaultallocatorguard.h>

#include <bslma_allocator.h>               // for testing only
#include <bslma_default.h>                 // for testing only
#include <bslma_newdeleteallocator.h>      // for testing only
#include <bslma_testallocator.h>           // for testing only

#include <cstdio>
#include <cstdlib>

using namespace BloombergLP;
using namespace std;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// The component under test "guards" the process-wide default allocator, which
// is to say that an instance of this object copies the default allocator to
// a data member and installs a new default (from the constructor argument) on
// construction, and restores the original default allocator on destruction.
//
// The concerns and testing in a single-threaded environment are
// straightforward.
//-----------------------------------------------------------------------------
// CREATORS
// [ 1] bslma_DefaultAllocatorGuard(bslma_Allocator *temporary);
// [ 1] ~bslma_DefaultAllocatorGuard();
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 2] USAGE EXAMPLE
//=============================================================================

//-----------------------------------------------------------------------------
//                  STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
static int testStatus = 0;

static void aSsErT(int c, const char *s, int i) {
    if (c) {
        printf("Error " __FILE__ "(%d): %s    (failed)\n", i, s);
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}

# define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }
//-----------------------------------------------------------------------------

//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define Q(X) printf("<| " #X " |>\n");  // Quote identifier literally.
#define L_ __LINE__                     // current Line number
#define T_ printf("\t");                // Print a tab (w/o newline)

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------
typedef bslma_DefaultAllocatorGuard Obj;

int globalVerbose         = 0;
int globalVeryVerbose     = 0;
int globalBeryVeryVerbose = 0;

//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

//=============================================================================
//                  CLASSES FOR TESTING USAGE EXAMPLES
//-----------------------------------------------------------------------------

class my_CountingAllocator : public bslma_Allocator
{
    int d_blocksOutstanding;
  public:
    my_CountingAllocator();
    ~my_CountingAllocator();

    virtual void *allocate(size_type size);
    virtual void deallocate(void *address);

    int blocksOutstanding() const { return d_blocksOutstanding; }
};

inline
my_CountingAllocator::my_CountingAllocator()
: d_blocksOutstanding(0)
{
}

inline
my_CountingAllocator::~my_CountingAllocator()
{
    if (0 < d_blocksOutstanding && globalVerbose) {
        printf("***ERROR: Memory Leak***\n"
               "%d block(s) leaked.\n"
               "Program aborting.\n", d_blocksOutstanding);
        ASSERT(0 || globalVerbose);
    }
}

void *my_CountingAllocator::allocate(size_type size)
{
    ++d_blocksOutstanding;
    return operator new(size);
}

void my_CountingAllocator::deallocate(void *address)
{
    --d_blocksOutstanding;
    operator delete(address);
}

class my_Container
{
    bslma_Allocator *d_allocator;
    int             *d_intArray;
  public:
    my_Container(bslma_Allocator* alloc = 0);

    // Containers don't typically have a function to return their
    // allocators, but this is useful for exposition:
    bslma_Allocator *allocator() const { return d_allocator; }
};

// Constructor
my_Container::my_Container(bslma_Allocator* alloc)
: d_allocator(bslma_Default::allocator(alloc))
{
    d_intArray = (int*) d_allocator->allocate(10 * sizeof(int));
}

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    // int veryVerbose = argc > 3;
    int veryVeryVerbose = argc > 4;

    globalVerbose = verbose;

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:  // Zero is always the leading case.
      case 2: {
        // --------------------------------------------------------------------
        // TESTING USAGE EXAMPLE
        //   The usage example provided in the component header file must
        //   compile, link, and run on all platforms as shown.
        //
        // Plan:
        //   Incorporate usage example from header into driver, remove
        //   leading comment characters, and replace 'assert' with
        //   'ASSERT'.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) printf("\nUSAGE EXAMPLE"
                            "\n=============\n");

        {
            bslma_NewDeleteAllocator *na
                = &bslma_NewDeleteAllocator::singleton();
            ASSERT(na == bslma_Default::defaultAllocator());

            {
                my_CountingAllocator testAllocator;
                Obj guard(&testAllocator);
                ASSERT(&testAllocator == bslma_Default::defaultAllocator());

                // Create and test the object under test, which will use the
                // test allocator *by* *default*.

                // . . .

                void *arena = bslma_Default::defaultAllocator()->allocate(10);
                ASSERT(arena);
                // Oops -- this will leak.

                ASSERT(0 != testAllocator.blocksOutstanding());
            }
            ASSERT(na == bslma_Default::defaultAllocator());
        }

      } break;

      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //
        // Concerns:
        //   1. The default allocator is bslma_NewDeleteAllocator
        //   2. The allocator function returns default allocator if passed-in
        //      allocator is null.
        //   3. The allocator function.
        //
        // Plan:
        //
        // Testing:
        //    bslma_DefaultAllocatorGuard(bslma_Allocator *temporary);
        //    ~bslma_DefaultAllocatorGuard();
        // --------------------------------------------------------------------

        if (verbose) printf("\nBREATHING TEST"
                            "\n==============\n");

        {
            my_CountingAllocator defaultAllocator;

            bslma_Default::setDefaultAllocator(&defaultAllocator);
            ASSERT(&defaultAllocator == bslma_Default::defaultAllocator());

            {
                bslma_TestAllocator testAllocator(veryVeryVerbose);
                Obj guard(&testAllocator);
                ASSERT(&testAllocator == bslma_Default::defaultAllocator());
            }
            ASSERT(&defaultAllocator == bslma_Default::defaultAllocator());
        }
      } break;

      default: {
        fprintf(stderr, "WARNING: CASE `%d' NOT FOUND.\n", test);
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        fprintf(stderr, "Error, non-zero test status = %d.\n", testStatus);
    }

    return testStatus;
}

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2004
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
