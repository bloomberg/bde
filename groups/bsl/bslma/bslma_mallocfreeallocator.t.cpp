// bslma_mallocfreeallocator.t.cpp  -*-C++-*-

#include <bslma_mallocfreeallocator.h>

#include <bslma_allocator.h>       // for testing only
#include <iostream>
#include <cstdio>       // std::printf

using namespace BloombergLP;
using namespace std;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// We are testing an allocator that uses 'std::malloc' and 'std::free' instead
// of global 'new' and 'delete'.  Since there is no way of intercepting the
// 'std::malloc' and 'std::free' methods, we indirectly check for proper source
// of memory allocation by overriding global operator 'new' and 'delete', and
// check whether they are invoked.  If no calls to global 'new' and 'delete'
// are made, we assume 'std::malloc' and 'std::free' are used.
//-----------------------------------------------------------------------------
// [ 1] bslma_MallocFreeAllocator()
// [ 1] ~bslma_MallocFreeAllocator()
// [ 1] void *allocate(size_type size)
// [ 1] void deallocate((void *address)
// [ 2] void *allocate(size_type size)   // allocate 0
// [ 2] void deallocate((void *address)  // deallocate 0
// [ 3] static bslma_MallocFreeAllocator& singleton()
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 4] USAGE EXAMPLE
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
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------

#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", " << flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number
#define T_ cout << "\t" << flush;             // Print tab w/o newline

//=============================================================================
//                          GLOBALS FOR TESTING
//-----------------------------------------------------------------------------

static  int  globalVeryVeryVerbose            = 0;

static  int  globalNewCalledCount             = 0;
static  int  globalNewCalledCountIsEnabled    = 0;
static  int  globalNewCalledLastArg           = 0;

static  int  globalDeleteCalledCount          = 0;
static  int  globalDeleteCalledCountIsEnabled = 0;
static void *globalDeleteCalledLastArg        = 0;

//==========================================================================
//                 REDEFINED GLOBAL OPERATOR NEW AND DELETE
//--------------------------------------------------------------------------

#ifdef BDE_BUILD_TARGET_EXC
void *operator new(size_t size) throw(std::bad_alloc)
#else
void *operator new(size_t size)
#endif
    // Trace use of global operator new.  Note that we must use printf
    // to avoid recursion.
{
    void *addr = malloc(size);

    if (globalNewCalledCountIsEnabled) {
        ++globalNewCalledCount;
        globalNewCalledLastArg = size;
    }

    if (globalVeryVeryVerbose) {
        printf("global new called -");
        if (!globalNewCalledCountIsEnabled) {
            printf(" not");
        }
        printf(" enabled - count = %d: %p\n", globalNewCalledCount, addr);
    }

    return addr;
}

#ifdef BDE_BUILD_TARGET_EXC
void operator delete(void *address) throw()
#else
void operator delete(void *address)
#endif
    // Trace use of global operator delete.
{
    if (globalDeleteCalledCountIsEnabled) {
        ++globalDeleteCalledCount;
        globalDeleteCalledLastArg = address;
    }

    if (globalVeryVeryVerbose) {
        printf("global delete called -");
        if (!globalDeleteCalledCountIsEnabled) {
            printf(" not");
        }
        printf(" enabled - count = %d: freeing %p\n",
                globalDeleteCalledCount, address);
    }

    free(address);
}

//=============================================================================
//                               USAGE EXAMPLE
//-----------------------------------------------------------------------------
// This component is intended to be used when the use of 'new' and 'delete' are
// not desirable, such as the case of 'bslma_TestAllocator'.  Instead of using
// 'bslma_Default' which uses the 'bslma_NewDeleteAllocator', this component
// can be used to bypass the use of 'new' and 'delete'.
//
// The following example demonstrates the use of this component for a user
// defined allocator instead of using the default allocator:
//..
//  // my_allocator.h
//  // ...
//
    class my_Allocator : public bslma_Allocator {
//      // This class provides a mechanism for allocation and deallocation.
//
//      // DATA
        bslma_Allocator *d_allocator_p;  // allocator (held, not owned)
//
      public:
//      // CREATORS
        my_Allocator(bslma_Allocator *basicAllocator = 0);
//          // Create a 'my_Allcoator'.  Optionally specify 'basicAllocator' to
//          // supply memory.  If 'basicAllocator' is 0, the
//          // 'bslma_MallocFreeAllocator' will be used.
//
        ~my_Allocator();
//          // Destroy this allocator.  Note that the behavior of destroying an
//          // allocator while memory is allocated from it is not specified.
//          // (Unless you *know* that it is valid to do so, don't!)
//
//      // MANIPULATORS
        void *allocate(size_type size);
//          // Return a newly allocated block of memory of (at least) the
//          // specified positive 'size' (bytes).  If 'size' is 0, a null
//          // pointer is returned with no effect.  The behavior is undefined
//          // if size < 0.  Note that the alignment of the address returned is
//          // the maximum alignment for any fundamental type defined for this
//          // platform.
//
        void deallocate(void *address);
//          // Return the memory at the specified 'address' back to this
//          // allocator.  If 'address' is 0, this function has no effect.  The
//          // behavior is undefined if 'address' was not allocated using this
//          // allocator, or has already been deallocated.
    };
//..
// The constructor is implemented using 'bslma_MallocFreeAllocator'.
//..
//  // my_allocator.cpp
//  // ...
//
//  // CREATORS
    my_Allocator::my_Allocator(bslma_Allocator *basicAllocator)
    : d_allocator_p(basicAllocator
                    ? basicAllocator 
                    : &bslma_MallocFreeAllocator::singleton())
    {
    }
//
//  // ...
//..
// When the 'basicAllocator' is not specified, the 'bslma_MallocFreeAllocator'
// will be used - which then calls 'std::malloc' and 'std::free' for allocating
// and deallocating memory.

//=============================================================================
//           Additional Functionality Needed to Complete Usage Test Case
//-----------------------------------------------------------------------------

my_Allocator::~my_Allocator()
{
}

// MANIPULATORS
void *my_Allocator::allocate(size_type size)
{
    return d_allocator_p->allocate(size);
}

void my_Allocator::deallocate(void *address)
{
    d_allocator_p->deallocate(address);
}

//=============================================================================
//                                MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    static int test = argc > 1 ? atoi(argv[1]) : 0;
    static int verbose = argc > 2;
    static int veryVerbose = argc > 3;
    static int veryVeryVerbose = globalVeryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:
      case 4: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //
        // Concerns:
        //   The usage example provided in the component header file must
        //   compile, link, and run on all platforms as shown.
        //
        // Plan:
        //   Allocate memory using the default constructed 'my_Allocator', then
        //   ensure that global 'new' and 'delete' are not invoked.  Also
        //   construct 'my_Allocator' using a 'bslma_newDeleteAllocator', then
        //   ensure that global 'new' and 'delete' are invoked.
        //
        // Testing:
        //   Usage example
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "USAGE EXAMPLE" << endl
                                  << "=============" << endl;

        if (verbose) cout << "\nCreating my_Allocator" << endl;

        my_Allocator alloc;

        if (verbose) cout << "\nTesting allocate" << endl;

        ASSERT(0 == globalNewCalledCount);
        ASSERT(0 == globalNewCalledLastArg);
        globalNewCalledCountIsEnabled = 1;
        void *addr1 = alloc.allocate(5);
        globalNewCalledCountIsEnabled = 0;

        ASSERT(0 != addr1);
        ASSERT(0 == globalNewCalledCount);
        ASSERT(0 == globalDeleteCalledCount);
        ASSERT(0 == globalNewCalledLastArg);

        ASSERT(0 == globalNewCalledCount);
        ASSERT(0 == globalNewCalledLastArg);
        globalNewCalledCountIsEnabled = 1;
        void *addr2 = alloc.allocate(10);
        globalNewCalledCountIsEnabled = 0;

        ASSERT(0 != addr1);
        ASSERT(0 == globalNewCalledCount);
        ASSERT(0 == globalDeleteCalledCount);
        ASSERT(0 == globalNewCalledLastArg);

        if (verbose) cout << "\nTesting deallocate" << endl;

        ASSERT(0 == globalDeleteCalledCount);
        ASSERT(0 == globalDeleteCalledLastArg);

        globalDeleteCalledCountIsEnabled = 1;
        alloc.deallocate(addr1);
        globalDeleteCalledCountIsEnabled = 0;
        ASSERT(0 == globalNewCalledCount);
        ASSERT(0 == globalDeleteCalledCount);
        ASSERT(0 == globalDeleteCalledLastArg);

        globalDeleteCalledCountIsEnabled = 1;
        alloc.deallocate(addr2);
        globalDeleteCalledCountIsEnabled = 0;
        ASSERT(0 == globalNewCalledCount);
        ASSERT(0 == globalDeleteCalledCount);
        ASSERT(0 == globalDeleteCalledLastArg);

      } break;
      case 3: {
        // --------------------------------------------------------------------
        // SINGLETON TEST
        //
        // Concerns:
        //   1. That the 'singleton' method returns a valid
        //      'bslma_MallocFreeAllocator' that can be used to allocate and
        //      deallocate memory using 'malloc' and 'free'.
        //
        //   2. Multiple invocations of 'singleton' return the same instance
        //      of 'bslma_MallocFreeAllocator'.
        //
        // Plan:
        //   Perform the same breathing test on the 'bslma_MallocFreeAllocator'
        //   returned by the 'singleton' method.  Also store the address of the
        //   instance returned by the 'singleton' method.
        //
        // Testing:
        //   static bslma_MallocFreeAllocator& singleton()
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "SINGLETON TEST" << endl
                                  << "==============" << endl;

        if (verbose) cout << "\nTesting 'singleton'" << endl;

        bslma_MallocFreeAllocator *alloc =
                                       &bslma_MallocFreeAllocator::singleton();
        bslma_MallocFreeAllocator *alloc2 =
                                       &bslma_MallocFreeAllocator::singleton();

        ASSERT(alloc == alloc2);

        if (veryVerbose) cout << "\tTesting allocate" << endl;

        ASSERT(0 == globalNewCalledCount);
        ASSERT(0 == globalNewCalledLastArg);
        globalNewCalledCountIsEnabled = 1;
        void *addr1 = alloc->allocate(5);
        globalNewCalledCountIsEnabled = 0;

        ASSERT(0 != addr1);
        ASSERT(0 == globalNewCalledCount);
        ASSERT(0 == globalDeleteCalledCount);
        ASSERT(0 == globalNewCalledLastArg);

        ASSERT(0 == globalNewCalledCount);
        ASSERT(0 == globalNewCalledLastArg);
        globalNewCalledCountIsEnabled = 1;
        void *addr2 = alloc->allocate(10);
        globalNewCalledCountIsEnabled = 0;

        ASSERT(0 != addr1);
        ASSERT(0 == globalNewCalledCount);
        ASSERT(0 == globalDeleteCalledCount);
        ASSERT(0 == globalNewCalledLastArg);

        if (veryVerbose) cout << "\tTesting deallocate" << endl;

        ASSERT(0 == globalDeleteCalledCount);
        ASSERT(0 == globalDeleteCalledLastArg);

        globalDeleteCalledCountIsEnabled = 1;
        alloc->deallocate(addr1);
        globalDeleteCalledCountIsEnabled = 0;
        ASSERT(0 == globalNewCalledCount);
        ASSERT(0 == globalDeleteCalledCount);
        ASSERT(0 == globalDeleteCalledLastArg);

        globalDeleteCalledCountIsEnabled = 1;
        alloc->deallocate(addr2);
        globalDeleteCalledCountIsEnabled = 0;
        ASSERT(0 == globalNewCalledCount);
        ASSERT(0 == globalDeleteCalledCount);
        ASSERT(0 == globalDeleteCalledLastArg);
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // ALLOCATE / DEALLOCATE NULL TEST
        //
        // Concerns:
        //   That invoking 'allocate' with 0 size and 'deallocate' on 0 address
        //   succeeds.
        //
        // Plan:
        //   Create a 'bslma_MallocFreeAllocator' on the stack.  Then invoke
        //   'allocate' with 0 size and 'deallocate' on 0 address.
        //
        // Testing:
        //   void *allocate(size_type size)   // allocate 0
        //   void deallocate((void *address)  // deallocate 0
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "ALLOCATE / DEALLCOATE NULL TEST" << endl
                                  << "===============================" << endl;

        if (verbose) cout << "\nCreating allocator" << endl;

        bslma_MallocFreeAllocator alloc;

        if (verbose) cout << "\nTesting allocate 0" << endl;

        void *address = alloc.allocate(0);
        ASSERT(0 == address);

        if (verbose) cout << "\nTesting deallocate 0" << endl;

        alloc.deallocate(address);
        ASSERT(0 == address);

      } break;
      case 1: {
        // -----------------------------------------------------------------
        // BREATHING TEST:
        //
        // Concerns:
        //   That 'new' and 'delete' are not invoked by the
        //   'bslma_MallocFreeAllocator'.
        //
        // Plan:
        //   Create a malloc-free allocator on the program stack and verify
        //   that the global 'new' and 'delete' are not called per method
        //   invocation, and with the appropriate arguments.
        //
        // Testing:
        //    bslma_MallocFreeAllocator();
        //    ~bslma_MallocFreeAllocator();
        //    void *allocate(int size);
        //    void deallocate(void *address);
        // -----------------------------------------------------------------

        if (verbose) cout << endl << "BREATHING TEST" << endl
                                  << "==============" << endl;

        if (verbose) cout << "\nCreating allocator" << endl;

        bslma_MallocFreeAllocator alloc;

        if (verbose) cout << "\nTesting allocate" << endl;

        ASSERT(0 == globalNewCalledCount);
        ASSERT(0 == globalNewCalledLastArg);
        globalNewCalledCountIsEnabled = 1;
        void *addr1 = alloc.allocate(5);
        globalNewCalledCountIsEnabled = 0;

        ASSERT(0 != addr1);
        ASSERT(0 == globalNewCalledCount);
        ASSERT(0 == globalDeleteCalledCount);
        ASSERT(0 == globalNewCalledLastArg);

        ASSERT(0 == globalNewCalledCount);
        ASSERT(0 == globalNewCalledLastArg);
        globalNewCalledCountIsEnabled = 1;
        void *addr2 = alloc.allocate(10);
        globalNewCalledCountIsEnabled = 0;

        ASSERT(0 != addr1);
        ASSERT(0 == globalNewCalledCount);
        ASSERT(0 == globalDeleteCalledCount);
        ASSERT(0 == globalNewCalledLastArg);

        if (verbose) cout << "\nTesting deallocate" << endl;

        ASSERT(0 == globalDeleteCalledCount);
        ASSERT(0 == globalDeleteCalledLastArg);

        globalDeleteCalledCountIsEnabled = 1;
        alloc.deallocate(addr1);
        globalDeleteCalledCountIsEnabled = 0;
        ASSERT(0 == globalNewCalledCount);
        ASSERT(0 == globalDeleteCalledCount);
        ASSERT(0 == globalDeleteCalledLastArg);

        globalDeleteCalledCountIsEnabled = 1;
        alloc.deallocate(addr2);
        globalDeleteCalledCountIsEnabled = 0;
        ASSERT(0 == globalNewCalledCount);
        ASSERT(0 == globalDeleteCalledCount);
        ASSERT(0 == globalDeleteCalledLastArg);

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
//      Copyright (C) Bloomberg L.P., 2009
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
