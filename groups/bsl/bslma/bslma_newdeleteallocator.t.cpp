// bslma_newdeleteallocator.t.cpp                                     -*-C++-*-

#include <bslma_newdeleteallocator.h>

#include <bslma_allocator.h>    // for testing only

#include <bsls_platform.h>

#include <cstdio>      // printf()
#include <cstdlib>     // atoi()
#include <cstring>     // memcpy()
#include <iostream>

#ifdef BSLS_PLATFORM__CMP_MSVC       // Microsoft Compiler
#ifdef _MSC_EXTENSIONS               // Microsoft Extensions Enabled
#include <new>                       // if so, need to include new as well
#endif
#endif

using namespace BloombergLP;
using namespace std;

//==========================================================================
//                    STANDARD BDE ASSERT TEST MACRO
//--------------------------------------------------------------------------

static int testStatus = 0;
static void aSsErT(int c, const char *s, int i)
{
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
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
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------

#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", "<< flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number
#define T_()  cout << '\t' << flush;          // Print tab w/o newline
#define PP(X) (cout << #X " = " << (X) << endl, 0) // Print name and
                                                   // value, then return false.

//==========================================================================
//                              TEST PLAN
//--------------------------------------------------------------------------
//                              Overview
//                              --------
// We are testing a concrete implementation of a protocol.  The component also
// provides access to a singleton instance.  This singleton must be available
// from the moment it is first needed until after its last use, yet we want to
// suppress any reports of potential memory leaks resulting from analysis tools
// such as purify.  We also need to verify that instances of this concrete
// class call global operators 'new' and 'delete'.  We can do that by
// redefining these global operators and instrumenting them to be sure that
// these operators are in fact called.
//--------------------------------------------------------------------------
// [ 2] static bslma_NewDeleteAllocator& singleton();
// [ 2] static bslma_Allocator *allocator(bslma_Allocator *basicAllocator);
// [ 1] bslma_NewDeleteAllocator();
// [ 1] ~bslma_NewDeleteAllocator();
// [ 1] void *allocate(int size);
// [ 1] void deallocate(void *address);
//--------------------------------------------------------------------------
// [ 1] Make sure that global operators new and delete are called.
// [ 2] Make sure that the lifetime of the singleton is sufficient.
// [ 2] Make sure that memory is not leaked.
// [ 3] USAGE EXAMPLE
//==========================================================================

//=============================================================================
//                                USAGE EXAMPLE
//-----------------------------------------------------------------------------
// The most common and proper use of 'bslma_NewDeleteAllocator' is both
// *indirect* and *by* *default* (see 'bslma_default').  For example,
// consider (along with its destructor) the default and copy constructors for,
// say, a simple container, such as 'my_ShortArray', each of which take as
// its final optional argument the address of a 'bdema_Allocator' protocol:
//..
//  // my_shortarray.h:
//  // ...
//  class bslma_Allocator;

    class my_ShortArray {
        short           *d_array_p;     // dynamically-allocated array
        int              d_capacity;    // physical capacity (in elements)
        int              d_length;      // logical length (in elements)
        bslma_Allocator *d_allocator_p; // memory allocator (not owned)

      public:
        my_ShortArray(bslma_Allocator *basicAllocator = 0);
            // Create an empty 'my_shortArray'.  Optionally specify a
            // 'basicAllocator' used to supply memory.  If 'basicAllocator'
            // is 0, the currently installed default allocator is used.

        my_ShortArray(const my_ShortArray&  other,
                      bslma_Allocator      *basicAllocator = 0);
            // Create a 'bslma_ShortArray' having the same value as the
            // specified 'other' array.  Optionally specify a 'basicAllocator'
            // used to supply memory.  If 'basicAllocator' is 0, the currently
            // installed default allocator is used.

        ~my_ShortArray();
            // Destroy this object.

       // ...
    };

    // ...
//..
// In order to satisfy this contract, we will need a globally accessible
// utility (see 'bslma_default'), which by default returns the singleton
// 'bslma_NewDeleteAllocator', but which could be configured to return some
// other allocator, say a *test* allocator (see 'bslma_testallocator'):
//..
//  // my_default.h:
//  // ...
//  class bslma_Allocator;

    struct my_Default {
        // This class maintains a process-wide 'bslma_allocator' object
        // to be used when an allocator is needed, and not suppled explicitly.
        // By default, the currently installed default allocator is the unique
        // 'bslma_NewDeleteAllocator' object returned by the 'static' method,
        // 'bslma_NewDeleteAllocator::singleton()'.  Note that the default
        // allocator will exist longer than any possibility of its use.

        static bslma_Allocator *allocator(bslma_Allocator *basicAllocator);
            // Return the address of the specified modifiable
            // 'basicAllocator' or, if 'basicAllocator' is 0, an instance of
            // the currently installed default 'bslma_allocator' object, which
            // will exist longer than any possibility of its use.  Note
            // that this function can safely be called concurrently (from
            // multiple threads).

        static bslma_Allocator *replace(bslma_Allocator *basicAllocator);
            // Replace the address of the currently installed allocator with
            // that of the specified modifiable 'basicAllocator' (or if 0,
            // with the "factory" default, 'bslma_NewDeleteAllocator'), and
            // return the address of the previous allocator.  The behavior is
            // undefined unless 'basicAllocator' will exist longer than any
            // possibility of its use.  Note that this function is *not* *at*
            // *all* thread safe, and should *never* be called when multiple
            // threads are active.
    };

    // my_default.cpp:
    // ...

//  #include <my_default.h>

    static bslma_Allocator *s_default_p = 0; // load-time initialized

    bslma_Allocator *my_Default::allocator(bslma_Allocator *basicAllocator)
    {
        return bslma_NewDeleteAllocator::allocator(s_default_p);
    }

    bslma_Allocator *my_Default::replace(bslma_Allocator *basicAllocator)
    {
        bslma_Allocator *tmp =
                              bslma_NewDeleteAllocator::allocator(s_default_p);
        s_default_p = bslma_NewDeleteAllocator::allocator(basicAllocator);
        return tmp;
    }
//..
// Notice that the only part of the 'bslma_NewDeleteAllocator' class we used
// directly was its static 'allocator' method, which -- in addition to safely
// constructing the singleton 'bslma_NewDeleteAllocator' object on first
// access -- also automatically replaces a 0 address value with that of
// singleton 'bslma_NewDeleteAllocator' object.  From now on, we will never
// again need to invoke the 'bslma_newdeleteallocator' component's interface
// directly, but instead use it through 'my_Default' (see bslma_Default' for
// what is actually used in practice).
//
// Turning back to our 'my_shortarray' example, let's now implement the two
// constructors using the 'bslma_newdeleteallocator' component indirectly
// via the 'my_default' component:
//..
//  // my_shortarray.cpp:
//  #include <my_shortarray.h>
//  #include <my_default.h>
//  #include <bsls_assert.h>

    // ...

    enum {
        INITIAL_CAPACITY = 0, // recommended to avoid unnecessary allocations
                              // possibly resulting in locking and extra thread
                              // contention for the 'bslma_NewDeleteAllocator'

        GROW_FACTOR = 2       // typical value for geometric growth
    };

    // ...

    my_ShortArray::my_ShortArray(bslma_Allocator *basicAllocator)
    : d_capacity(INITIAL_CAPACITY)
    , d_length(0)
    , d_allocator_p(my_Default::allocator(basicAllocator))
    {
        ASSERT(d_allocator_p);
        d_array_p = (short *)  // no thread contention if 'd_capacity' is 0
                    d_allocator_p->allocate(d_capacity * sizeof *d_array_p);
        ASSERT(0 == d_array_p);
    }

    my_ShortArray::my_ShortArray(const my_ShortArray&  other,
                                 bslma_Allocator      *basicAllocator)
    : d_capacity(other.d_capacity)
    , d_length(other.d_length)
    , d_allocator_p(my_Default::allocator(basicAllocator))
    {
        ASSERT(d_allocator_p);
        d_array_p = (short *)
                    d_allocator_p->allocate(d_capacity * sizeof *d_array_p);
        ASSERT(!d_capacity == !d_array_p);
        memcpy(d_array_p, other.d_array_p, d_length * sizeof *d_array_p);
    }

    my_ShortArray::~my_ShortArray()
    {
        d_allocator_p->deallocate(d_array_p); // no locking if 'd_array_p' is 0
    }

   // ...
//..
// When the default constructor is called, the default capacity and length are
// recorded in data members via the initialization list.  The static function
// 'allocator' (provided in 'my_Default') is used to assign the value of the
// 'basicAllocator' address passed in, or if that is 0, the address of the
// currently installed default allocator, which by default is the singleton
// object of type 'bslma_NewDeleteAllocator', defined in this component.  Note
// that since 'INITIAL_CAPACITY' is 0, a default constructed object that is
// created using a 'bslma_NewDeleteAllocator' will *not* invoke the 'operator
// new' function, which on some platforms may needlessly acquire a lock,
// causing unnecessary overhead (the same potential overhead is avoided for
// 'operator delete' whenever a 0 'd_array_p' value is deallocated in the
// destructor) and 'd_allocator_p refers to a 'bslma_NewDeleteAllocator'.
// Note also that, for the copy constructor, the currently installed default
// allocator, and not the 'other' array's allocator is used whenever
// 'basicAllocator' is 0 or not explicitly supplied.
//
// Finally note that this entire component is *not* intended for direct use by
// typical clients: See 'bslma_default' for more information or proper usage.

//--------------------------------------------------------------------------
//                      REDEFINED GLOBAL OPERATOR NEW
//--------------------------------------------------------------------------

static int globalVeryVerbose = 0;

// In optimize mode, the HPUX compiler fails to take into account that
// '*.allocate' can possibly call 'new' and '*.deallocate' can call 'delete'
// and fails to save relevant statics to RAM.  Declare these to be 'volatile'
// to ensure the compiler saves their values to RAM.

static volatile int   globalNewCalledCount = 0;
static volatile int   globalNewCalledCountIsEnabled = 0;
static volatile int   globalNewCalledLastArg = 0;

static volatile int   globalDeleteCalledCount = 0;
static volatile int   globalDeleteCalledCountIsEnabled = 0;
static volatile void *globalDeleteCalledLastArg = 0;

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

    if (globalVeryVerbose) {
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

    if (globalVeryVerbose) {
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
//                                MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    static int test = argc > 1 ? atoi(argv[1]) : 0;
    static int verbose = argc > 2;
    static int veryVerbose = globalVeryVerbose = argc > 3;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:
      case 3: {
        // -----------------------------------------------------------------
        // USAGE EXAMPLE
        //
        // Concerns:
        //   The usage example provided in the component header file must
        //   compile, link, and run on all platforms as shown.
        //
        // Plan:
        //   Run the usage example and exercise the creators and manipulators
        //   of 'my_Queue' using a 'bslma_TestAllocator' to verify that memory
        //   is allocated and deallocated properly.
        //
        // Testing:
        //   Usage example
        // -----------------------------------------------------------------

        if (verbose) cout << endl << "USAGE TEST" << endl
                                  << "==========" << endl;

        ASSERT(0 == globalNewCalledCount);
        globalNewCalledCountIsEnabled = 1;

        my_ShortArray array;

        globalNewCalledCountIsEnabled = 0;

        // Verify that a default constructed 'my_ShortArray' does not trigger
        // a call to operator 'new' (as specified by the component level doc).
        ASSERT(0 == globalNewCalledCount);

      } break;
      case 2: {
        // -----------------------------------------------------------------
        // SINGLETON TEST:
        //   We need to make sure that we get a valid singleton and that
        //   it remains valid as long as we need it.  The way we'll try do
        //   this is to allocate memory in the destructor of a static object
        //   that is defined prior the first use of the allocator.
        //
        // Testing:
        //   static bslma_NewDeleteAllocator& singleton();
        //   static bslma_Allocator *allocator(bslma_Allocator *basicAlloc);
        //
        //   Make sure that the lifetime of the singleton is sufficient.
        //   Make sure that memory is not leaked.
        // -----------------------------------------------------------------

        if (verbose) cout << endl << "SINGLETON TEST" << endl
                                  << "==============" << endl;

        if (veryVerbose) cout << "\nTesting 'singleton'." << endl;
        {
            ASSERT(0 == globalNewCalledCount);
            ASSERT(0 == globalDeleteCalledCount);

            globalNewCalledCountIsEnabled = 1;
            globalDeleteCalledCountIsEnabled = 1;

            bslma_NewDeleteAllocator *p =
                                &bslma_NewDeleteAllocator::singleton();
            bslma_NewDeleteAllocator *q =
                                &bslma_NewDeleteAllocator::singleton();

            globalNewCalledCountIsEnabled = 0;
            globalDeleteCalledCountIsEnabled = 0;

            ASSERT(0 == globalNewCalledCount);
            ASSERT(0 == globalDeleteCalledCount);

            ASSERT(p == q);

            globalNewCalledCountIsEnabled = 1;
            void *addr1 = p->allocate(15);
            globalNewCalledCountIsEnabled = 0;
            ASSERT(1 == globalNewCalledCount);
            ASSERT(0 == globalDeleteCalledCount);
            ASSERT(15 == globalNewCalledLastArg);

            ASSERT(0 == globalDeleteCalledCount);
            ASSERT(0 == globalDeleteCalledLastArg);

            globalDeleteCalledCountIsEnabled = 1;
            q->deallocate(addr1);
            globalDeleteCalledCountIsEnabled = 0;
            ASSERT(1 == globalNewCalledCount);
            ASSERT(1 == globalDeleteCalledCount);
            ASSERT(addr1 == globalDeleteCalledLastArg);
        }

        if (veryVerbose) cout << "\nTesting 'allocator'." << endl;
        {
            bslma_Allocator *p = (bslma_Allocator *) 0xDeadBeef;
            bslma_Allocator *q = &bslma_NewDeleteAllocator::singleton();

            bslma_Allocator *r = bslma_NewDeleteAllocator::allocator(p);
            ASSERT(r == p);

            bslma_Allocator *s = bslma_NewDeleteAllocator::allocator(0);
            ASSERT(q == s);

            bslma_Allocator *t = bslma_NewDeleteAllocator::allocator(q);
            ASSERT(q == t);
        }

      } break;
      case 1: {
        // -----------------------------------------------------------------
        // BASIC TEST:
        //   Create a new-delete allocator on the program stack and verify that
        //   'new' and 'delete' are each called exactly once per method
        //   invocation, and with the appropriate arguments.
        //
        // Testing:
        //    bslma_NewDeleteAllocator();
        //    ~bslma_NewDeleteAllocator();
        //    void *allocate(int size);
        //    void deallocate(void *address);
        //
        //    Make sure that global operators new and delete are called.
        // -----------------------------------------------------------------

        if (verbose) cout << endl << "BASIC TEST" << endl
                                  << "==========" << endl;

        if (veryVerbose) cout << "\nCreate an allocator" << endl;

        bslma_NewDeleteAllocator a;

        ASSERT(0 == globalNewCalledCount);
        ASSERT(0 == globalNewCalledLastArg);

        globalNewCalledCountIsEnabled = 1;
        void *addr1 = a.allocate(5);
        globalNewCalledCountIsEnabled = 0;
        ASSERT(1 == globalNewCalledCount);
        ASSERT(0 == globalDeleteCalledCount);
        ASSERT(5 == globalNewCalledLastArg);

        globalNewCalledCountIsEnabled = 1;
        void *addr2 = a.allocate(10);
        globalNewCalledCountIsEnabled = 0;
        ASSERT(2 == globalNewCalledCount);
        ASSERT(0 == globalDeleteCalledCount);
        ASSERT(10 == globalNewCalledLastArg);

        ASSERT(0 == globalDeleteCalledCount);
        ASSERT(0 == globalDeleteCalledLastArg);

        globalDeleteCalledCountIsEnabled = 1;
        a.deallocate(addr1);
        globalDeleteCalledCountIsEnabled = 0;
        ASSERT(2 == globalNewCalledCount);
        ASSERT(1 == globalDeleteCalledCount);
        ASSERT(addr1 == globalDeleteCalledLastArg);

        globalDeleteCalledCountIsEnabled = 1;
        a.deallocate(addr2);
        globalDeleteCalledCountIsEnabled = 0;
        ASSERT(2 == globalNewCalledCount);
        ASSERT(2 == globalDeleteCalledCount);
        ASSERT(addr2 == globalDeleteCalledLastArg);

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
