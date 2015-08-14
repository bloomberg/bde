// bslma_newdeleteallocator.t.cpp                                     -*-C++-*-

#include <bslma_newdeleteallocator.h>

#include <bslma_allocator.h>    // for testing only

#include <bsls_bsltestutil.h>
#include <bsls_platform.h>

#include <stdio.h>      // 'printf'
#include <stdlib.h>     // 'atoi'
#include <string.h>

#ifdef BDE_BUILD_TARGET_EXC
#include <new>  // 'std::bad_alloc' for standard exception specification
#endif

#ifdef BSLS_PLATFORM_CMP_MSVC       // Microsoft Compiler
#ifdef _MSC_EXTENSIONS               // Microsoft Extensions Enabled
#include <new>                       // if so, need to include new as well
#endif
#endif

using namespace BloombergLP;

// ============================================================================
//                     STANDARD BSL ASSERT TEST FUNCTION
// ----------------------------------------------------------------------------

namespace {

int testStatus = 0;

void aSsErT(bool condition, const char *message, int line)
{
    if (condition) {
        printf("Error " __FILE__ "(%d): %s    (failed)\n", line, message);

        if (0 <= testStatus && testStatus <= 100) {
            ++testStatus;
        }
    }
}

}  // close unnamed namespace

// ============================================================================
//               STANDARD BSL TEST DRIVER MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT       BSLS_BSLTESTUTIL_ASSERT
#define ASSERTV      BSLS_BSLTESTUTIL_ASSERTV

#define LOOP_ASSERT  BSLS_BSLTESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLS_BSLTESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLS_BSLTESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLS_BSLTESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLS_BSLTESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLS_BSLTESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLS_BSLTESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLS_BSLTESTUTIL_LOOP6_ASSERT

#define Q            BSLS_BSLTESTUTIL_Q   // Quote identifier literally.
#define P            BSLS_BSLTESTUTIL_P   // Print identifier and value.
#define P_           BSLS_BSLTESTUTIL_P_  // P(X) without '\n'.
#define T_           BSLS_BSLTESTUTIL_T_  // Print a tab (w/o newline).
#define L_           BSLS_BSLTESTUTIL_L_  // current Line number

// ============================================================================
//                  NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT_SAFE_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(EXPR)
#define ASSERT_SAFE_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPR)
#define ASSERT_PASS(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS(EXPR)
#define ASSERT_FAIL(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL(EXPR)
#define ASSERT_OPT_PASS(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS(EXPR)
#define ASSERT_OPT_FAIL(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL(EXPR)

//=============================================================================
//                              TEST PLAN
//-----------------------------------------------------------------------------
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
//-----------------------------------------------------------------------------
// [ 2] static bslma::NewDeleteAllocator& singleton();
// [ 2] static bslma::Allocator *allocator(bslma::Allocator *basicAllocator);
// [ 1] bslma::NewDeleteAllocator();
// [ 1] ~bslma::NewDeleteAllocator();
// [ 1] void *allocate(int size);
// [ 1] void deallocate(void *address);
//-----------------------------------------------------------------------------
// [ 1] Make sure that global operators new and delete are called.
// [ 2] Make sure that the lifetime of the singleton is sufficient.
// [ 2] Make sure that memory is not leaked.
// [ 3] USAGE EXAMPLE
//=============================================================================

//=============================================================================
//                                USAGE EXAMPLE
//-----------------------------------------------------------------------------
// The most common and proper use of 'bslma::NewDeleteAllocator' is both
// *indirect* and *by* *default* (see 'bslma_default').  For example,
// consider (along with its destructor) the default and copy constructors for,
// say, a simple container, such as 'my_ShortArray', each of which take as
// its final optional argument the address of a 'bdema_Allocator' protocol:
//..
//  // my_shortarray.h:
//  // ...
//  namespace bslma { class Allocator; }

    class my_ShortArray {
        short            *d_array_p;     // dynamically-allocated array
        int               d_capacity;    // physical capacity (in elements)
        int               d_length;      // logical length (in elements)
        bslma::Allocator *d_allocator_p; // memory allocator (not owned)

      public:
        my_ShortArray(bslma::Allocator *basicAllocator = 0);
            // Create an empty 'my_shortArray'.  Optionally specify a
            // 'basicAllocator' used to supply memory.  If 'basicAllocator'
            // is 0, the currently installed default allocator is used.

        my_ShortArray(const my_ShortArray&  other,
                      bslma::Allocator     *basicAllocator = 0);
            // Create a 'bslma::ShortArray' having the same value as the
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
// 'bslma::NewDeleteAllocator', but which could be configured to return some
// other allocator, say a *test* allocator (see 'bslma_testallocator'):
//..
//  // my_default.h:
//  // ...
//  namespace bslma { class Allocator; }

    struct my_Default {
        // This class maintains a process-wide 'bslma_allocator' object
        // to be used when an allocator is needed, and not suppled explicitly.
        // By default, the currently installed default allocator is the unique
        // 'bslma::NewDeleteAllocator' object returned by the 'static' method,
        // 'bslma::NewDeleteAllocator::singleton()'.  Note that the default
        // allocator will exist longer than any possibility of its use.

        static bslma::Allocator *allocator(bslma::Allocator *basicAllocator);
            // Return the address of the specified modifiable
            // 'basicAllocator' or, if 'basicAllocator' is 0, an instance of
            // the currently installed default 'bslma_allocator' object, which
            // will exist longer than any possibility of its use.  Note
            // that this function can safely be called concurrently (from
            // multiple threads).

        static bslma::Allocator *replace(bslma::Allocator *basicAllocator);
            // Replace the address of the currently installed allocator with
            // that of the specified modifiable 'basicAllocator' (or if 0,
            // with the "factory" default, 'bslma::NewDeleteAllocator'), and
            // return the address of the previous allocator.  The behavior is
            // undefined unless 'basicAllocator' will exist longer than any
            // possibility of its use.  Note that this function is *not* *at*
            // *all* thread safe, and should *never* be called when multiple
            // threads are active.
    };

    // my_default.cpp:
    // ...

//  #include <my_default.h>

    static bslma::Allocator *s_default_p = 0; // load-time initialized

    bslma::Allocator *my_Default::allocator(bslma::Allocator* /*basicAllocator*/)
    {
        return bslma::NewDeleteAllocator::allocator(s_default_p);
    }

    bslma::Allocator *my_Default::replace(bslma::Allocator *basicAllocator)
    {
        bslma::Allocator *tmp =
                             bslma::NewDeleteAllocator::allocator(s_default_p);
        s_default_p = bslma::NewDeleteAllocator::allocator(basicAllocator);
        return tmp;
    }
//..
// Notice that the only part of the 'bslma::NewDeleteAllocator' class we used
// directly was its static 'allocator' method, which -- in addition to safely
// constructing the singleton 'bslma::NewDeleteAllocator' object on first
// access -- also automatically replaces a 0 address value with that of
// singleton 'bslma::NewDeleteAllocator' object.  From now on, we will never
// again need to invoke the 'bslma_newdeleteallocator' component's interface
// directly, but instead use it through 'my_Default' (see bslma::Default' for
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
                              // contention for the 'bslma::NewDeleteAllocator'

        GROW_FACTOR = 2       // typical value for geometric growth
    };

    // ...

    my_ShortArray::my_ShortArray(bslma::Allocator *basicAllocator)
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
                                 bslma::Allocator     *basicAllocator)
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
// object of type 'bslma::NewDeleteAllocator', defined in this component.  Note
// that since 'INITIAL_CAPACITY' is 0, a default constructed object that is
// created using a 'bslma::NewDeleteAllocator' will *not* invoke the 'operator
// new' function, which on some platforms may needlessly acquire a lock,
// causing unnecessary overhead (the same potential overhead is avoided for
// 'operator delete' whenever a 0 'd_array_p' value is deallocated in the
// destructor) and 'd_allocator_p refers to a 'bslma::NewDeleteAllocator'.
// Note also that, for the copy constructor, the currently installed default
// allocator, and not the 'other' array's allocator is used whenever
// 'basicAllocator' is 0 or not explicitly supplied.
//
// Finally note that this entire component is *not* intended for direct use by
// typical clients: See 'bslma_default' for more information or proper usage.

//-----------------------------------------------------------------------------
//                      REDEFINED GLOBAL OPERATOR NEW
//-----------------------------------------------------------------------------

static bool globalVeryVerbose = false;

// In optimize mode, the HPUX compiler fails to take into account that
// '*.allocate' can possibly call 'new' and '*.deallocate' can call 'delete'
// and fails to save relevant statics to RAM.  Declare these to be 'volatile'
// to ensure the compiler saves their values to RAM.

static volatile int    globalNewCalledCount = 0;
static volatile int    globalNewCalledCountIsEnabled = 0;
static volatile size_t globalNewCalledLastArg = 0;

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
    int                 test = argc > 1 ? atoi(argv[1]) : 0;
    bool             verbose = argc > 2;
    bool         veryVerbose = argc > 3;
    bool     veryVeryVerbose = argc > 4;
    bool veryVeryVeryVerbose = argc > 5;

    globalVeryVerbose = veryVerbose;

    (void)veryVeryVerbose;       // suppress unused variable warning
    (void)veryVeryVeryVerbose;   // suppress unused variable warning

    printf("TEST " __FILE__ " CASE %d\n", test);

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
        //   of 'my_Queue' using a 'bslma::TestAllocator' to verify that memory
        //   is allocated and deallocated properly.
        //
        // Testing:
        //   Usage example
        // -----------------------------------------------------------------

        if (verbose) printf("\nUSAGE EXAMPLE"
                            "\n=============\n");

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
        //   static bslma::NewDeleteAllocator& singleton();
        //   static bslma::Allocator *allocator(bslma::Allocator *basicAlloc);
        //
        //   Make sure that the lifetime of the singleton is sufficient.
        //   Make sure that memory is not leaked.
        // -----------------------------------------------------------------

        if (verbose) printf("\nSINGLETON TEST"
                            "\n==============\n");

        if (veryVerbose) printf("\nTesting 'singleton'.\n");
        {
            ASSERT(0 == globalNewCalledCount);
            ASSERT(0 == globalDeleteCalledCount);

            globalNewCalledCountIsEnabled = 1;
            globalDeleteCalledCountIsEnabled = 1;

            bslma::NewDeleteAllocator *p =
                                &bslma::NewDeleteAllocator::singleton();
            bslma::NewDeleteAllocator *q =
                                &bslma::NewDeleteAllocator::singleton();

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

        if (veryVerbose) printf("\nTesting 'allocator'.\n");
        {
            bslma::Allocator *p = (bslma::Allocator *) 0xDeadBeef;
            bslma::Allocator *q = &bslma::NewDeleteAllocator::singleton();

            bslma::Allocator *r =  bslma::NewDeleteAllocator::allocator(p);
            ASSERT(r == p);

            bslma::Allocator *s =  bslma::NewDeleteAllocator::allocator(0);
            ASSERT(q == s);

            bslma::Allocator *t =  bslma::NewDeleteAllocator::allocator(q);
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
        //    bslma::NewDeleteAllocator();
        //    ~bslma::NewDeleteAllocator();
        //    void *allocate(int size);
        //    void deallocate(void *address);
        //
        //    Make sure that global operators new and delete are called.
        // -----------------------------------------------------------------

        if (verbose) printf("\nBASIC TEST"
                            "\n==========\n");

        if (veryVerbose) printf("\nCreate an allocator\n");

        bslma::NewDeleteAllocator a;

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
        fprintf(stderr, "WARNING: CASE `%d' NOT FOUND.\n", test);
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        fprintf(stderr, "Error, non-zero test status = %d.\n", testStatus);
    }

    return testStatus;
}

// ----------------------------------------------------------------------------
// Copyright 2013 Bloomberg Finance L.P.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// ----------------------------- END-OF-FILE ----------------------------------
