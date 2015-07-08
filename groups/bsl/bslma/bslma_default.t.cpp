// bslma_default.t.cpp                                                -*-C++-*-

#include <bslma_default.h>

#include <bslma_allocator.h>             // for testing only
#include <bslma_newdeleteallocator.h>

#include <bsls_alignedbuffer.h>          // for testing only
#include <bsls_assert.h>
#include <bsls_asserttest.h>
#include <bsls_bsltestutil.h>

#include <stdio.h>      // 'printf'
#include <stdlib.h>     // 'atoi'
#include <string.h>

#include <new>

using namespace BloombergLP;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// The component under test consists of two independent interfaces: one for the
// default allocator and one for the global allocator.  The methods in the
// interface for the global allocator have no perverse side-effects, so they
// are tested straightforwardly in a single test case (case 9).  On the other
// hand, the subtle side-effects of the 'defaultAllocator' and 'allocator'
// methods that pertain to the default allocator complicate testing of that
// portion of the component's interface.  The aspects of the default allocator
// interface for which these side-effects can be ignored are tested across
// three test cases: case 2 is a "bootstrap" of sorts that tests
// 'setDefaultAllocatorRaw' (primary manipulator) and 'defaultAllocator' (basic
// accessor); case 3 tests 'setDefaultAllocator' and 'lockDefaultAllocator';
// and case 4 tests 'allocator'.  The side-effects of 'defaultAllocator' and
// 'allocator' are then tested in cases specifically targeted at them (cases 5
// and 6 for 'defaultAllocator', and cases 7 and 8 for 'allocator').
//-----------------------------------------------------------------------------
// [ 3] int setDefaultAllocator(*ba);
// [ 2] void setDefaultAllocatorRaw(*ba);
// [ 3] void lockDefaultAllocator();
// [ 2] bslma::Allocator *defaultAllocator();
// [ 4] bslma::Allocator *allocator(*ba = 0);
// [ 9] bslma::Allocator *globalAllocator(*ba = 0);
// [ 9] bslma::Allocator *setGlobalAllocator(*ba);
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 2] BOOTSTRAP TEST
// [10] USAGE EXAMPLE 1
// [11] USAGE EXAMPLE 2
// [12] USAGE EXAMPLE 3

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
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------
typedef bslma::Default Obj;

//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

//=============================================================================
//                  CLASSES FOR TESTING USAGE EXAMPLES
//-----------------------------------------------------------------------------

///Usage
///-----
// The following sequence of usage examples illustrate recommended use of the
// default and global allocators.  The examples employ the following simple
// memory allocator, 'my_CountingAllocator', that counts both the number of
// memory blocks that have been allocated, but not yet deallocated, and the
// cumulative number of blocks ever allocated.  The two values are available
// through the accessors 'numBlocksInUse' and 'numBlocksTotal', respectively.
// For actual allocations and deallocations, 'my_CountingAllocator' uses global
// operators 'new' and 'delete':
//..
    // my_countingallocator.h
//  #include <bslma_allocator.h>

    class my_CountingAllocator : public bslma::Allocator {
        // This concrete allocator maintains (1) a count of the number of
        // blocks allocated that have not yet been deallocated, and (2) a count
        // of the cumulative number of blocks ever allocated.

        // PRIVATE DATA
        int d_numBlocksInUse;  // number of blocks currently allocated
        int d_numBlocksTotal;  // cumulative blocks ever requested

        // NOT IMPLEMENTED
        my_CountingAllocator(const my_CountingAllocator& original);
        my_CountingAllocator& operator=(const my_CountingAllocator& rhs);

      public:
        // CREATORS
        my_CountingAllocator();
            // Create a counting allocator.

        virtual ~my_CountingAllocator();
            // Destroy this counting allocator.

        // MANIPULATORS
        virtual void *allocate(size_type size);
            // Return a newly allocated block of memory of (at least) the
            // specified positive 'size' (bytes).  If 'size' is 0, a null
            // pointer is returned with no effect.  Note that the alignment of
            // the address returned is the maximum alignment for any
            // fundamental type defined for this platform.

        virtual void deallocate(void *address);
            // Return the memory at the specified 'address' back to this
            // allocator.  If 'address' is 0, this function has no effect.  The
            // behavior is undefined if 'address' was not allocated using this
            // allocator, or has already been deallocated.

        // ACCESSORS
        int numBlocksInUse() const;
            // Return the number of blocks currently in use from this counting
            // allocator.

        int numBlocksTotal() const;
            // Return the cumulative number of blocks ever allocated using this
            // counting allocator.  Note that
            // numBlocksTotal() >= numBlocksInUse().
    };

    // CREATORS
    inline
    my_CountingAllocator::my_CountingAllocator()
    : d_numBlocksInUse(0)
    , d_numBlocksTotal(0)
    {
    }

    // ACCESSORS
    inline
    int my_CountingAllocator::numBlocksInUse() const
    {
        return d_numBlocksInUse;
    }

    inline
    int my_CountingAllocator::numBlocksTotal() const
    {
        return d_numBlocksTotal;
    }
//..
// The 'virtual' methods of 'my_CountingAllocator' are defined in the component
// '.cpp' file:
//..
    // my_countingallocator.cpp
//  #include <my_countingallocator.h>

    // CREATORS
    my_CountingAllocator::~my_CountingAllocator()
    {
    }

    // MANIPULATORS
    void *my_CountingAllocator::allocate(size_type size)
    {
        ++d_numBlocksInUse;
        ++d_numBlocksTotal;
        return ::operator new(size);
    }

    void my_CountingAllocator::deallocate(void *address)
    {
        --d_numBlocksInUse;
        ::operator delete(address);
    }
//..
///Usage 1 -- Basic Default Allocator Use
/// - - - - - - - - - - - - - - - - - - -
// This usage example illustrates the basics of class design that relate to
// proper use of the default allocator, and introduces the standard pattern to
// apply when setting (and *locking*) the default allocator.  First we define a
// trivial class, 'my_Id', that uses an allocator.  'my_Id' simply encapsulates
// a C-style (null-terminated) id string that is accessible through the 'id'
// method.  Note that each constructor is declared to take an *optional*
// 'bslma::Allocator *' as its last argument.  Also note that the expression:
//..
//  bslma::Default::allocator(basicAllocator)
//..
// is used in applicable member initializers to propagate each constructor's
// allocator argument to the data members that require it (in this case, the
// object allocator that is held by each 'my_Id' instance).  If
// 'basicAllocator' is 0, the object is created using the default allocator.
// Otherwise, the explicitly supplied allocator is used:
//..
    // my_id.h
//  #include <bslma_allocator.h>
//  #include <bslma_default.h>

    class my_Id {
        // This is a trivial class solely intended to illustrate proper use
        // of the default allocator.

        // PRIVATE DATA
        char             *d_buffer_p;     // allocated (*owned*)
        bslma::Allocator *d_allocator_p;  // allocator (held, not owned)

        // NOT IMPLEMENTED (in order to reduce example size)
        my_Id& operator=(const my_Id& rhs);

      public:
        // CREATORS
        explicit my_Id(const char *id, bslma::Allocator *basicAllocator = 0);
            // Create an Id object having the specified 'id'.  Optionally
            // specify a 'basicAllocator' used to supply memory.  If
            // 'basicAllocator' is 0, the currently installed default allocator
            // is used.

        my_Id(const my_Id& original, bslma::Allocator *basicAllocator = 0);
            // Create an Id object initialized to the value of the specified
            // 'original' Id object.  Optionally specify a 'basicAllocator'
            // used to supply memory.  If 'basicAllocator' is 0, the currently
            // installed default allocator is used.

        ~my_Id();
            // Destroy this Id object.

        // ACCESSORS
        const char *id() const;
            // Return the id of this Id object.
    };

    // CREATORS
    inline
    my_Id::my_Id(const char *id, bslma::Allocator *basicAllocator)
    : d_allocator_p(bslma::Default::allocator(basicAllocator))
    {
        d_buffer_p = (char *)d_allocator_p->allocate(strlen(id) + 1);
        strcpy(d_buffer_p, id);
    }

    inline
    my_Id::my_Id(const my_Id& original, bslma::Allocator *basicAllocator)
    : d_allocator_p(bslma::Default::allocator(basicAllocator))
    {
        const char *id = original.id();
        d_buffer_p = (char *)d_allocator_p->allocate(strlen(id) + 1);
        strcpy(d_buffer_p, id);
    }

    inline
    my_Id::~my_Id()
    {
        d_allocator_p->deallocate(d_buffer_p);
    }

    // ACCESSORS
    inline
    const char *my_Id::id() const
    {
        return d_buffer_p;
    }
//..

///Usage 2 -- Detecting Allocator Propagation Bugs
///- - - - - - - - - - - - - - - - - - - - - - - -
// This example demonstrates how the default allocator is used to detect a very
// common programming error pertaining to allocator usage.  First we define the
// trivial (but buggy) 'my_IdPair' class:
//..
    // my_idpair.h
//  #include <my_id.h>
//  #include <bslma_default.h>

    class my_IdPair {
        // This is a trivial class solely intended to help illustrate a common
        // programming error.  This class has two instances of 'my_Id', only
        // one of which has the allocator correctly passed to it in the
        // definition of the constructor.

        // PRIVATE DATA
        my_Id d_id;     // primary id (allocating)
        my_Id d_alias;  // alias (allocating)

        // NOT IMPLEMENTED (in order to reduce example size)
        my_IdPair(const my_IdPair& original);
        my_IdPair& operator=(const my_IdPair& rhs);

      public:
        // CREATORS
        my_IdPair(const char       *id,
                  const char       *alias,
                  bslma::Allocator *basicAllocator = 0);
            // Create an Id pair having the specified 'id' and 'alias' ids.
            // Optionally specify a 'basicAllocator' used to supply memory.  If
            // 'basicAllocator' is 0, the currently installed default allocator
            // is used.

        ~my_IdPair();
            // Destroy this Id pair.

        // ACCESSORS
        const char *id() const;
            // Return the primary id of this Id pair.

        const char *alias() const;
            // Return the alias of this Id pair.
    };

    // CREATORS
    inline
    my_IdPair::my_IdPair(const char       *id,
                         const char       *alias,
                         bslma::Allocator *basicAllocator)
    : d_id(id, bslma::Default::allocator(basicAllocator))
    , d_alias(alias)  // drat! (forgot to pass along 'basicAllocator')
    {
    }

    inline
    my_IdPair::~my_IdPair()
    {
    }

    // ACCESSORS
    inline
    const char *my_IdPair::id() const
    {
        return d_id.id();
    }

    inline
    const char *my_IdPair::alias() const
    {
        return d_alias.id();
    }
//..
// The definition of the 'my_IdPair' constructor above intentionally includes a
// common programming error: the allocator in use by the object is not passed
// to *all* data members that require it.  We will see shortly how this error
// is detected at runtime using the default allocator.

///Usage 3 -- Basic Global Allocator Use
///- - - - - - - - - - - - - - - - - - -
// Next we define a simple singleton class, 'my_Singleton', that defaults to
// using the global allocator if one is not explicitly specified when the
// singleton object is initialized.  Toward that end, note that in contrast to
// 'my_Id', the constructor for 'my_Singleton' uses:
//..
//  bslma::Default::globalAllocator(basicAllocator)
//..
// in its member initializer:
//..
    // my_singleton.h
    class my_Singleton {
        // This is a trivial singleton class solely intended to illustrate use
        // of the global allocator.

        // CLASS DATA
        static my_Singleton *s_singleton_p;  // pointer to singleton object

        // PRIVATE DATA
        my_Id d_id;  // allocating

        // NOT IMPLEMENTED
        my_Singleton(const my_Singleton&  original,
                     bslma::Allocator    *basicAllocator = 0);
        my_Singleton& operator=(const my_Singleton& rhs);

      private:
        // PRIVATE CREATORS
        explicit my_Singleton(const char       *id,
                              bslma::Allocator *basicAllocator = 0);
            // Create a singleton having the specified 'id'.  Optionally
            // specify a 'basicAllocator' used to supply memory.  If
            // 'basicAllocator' is 0, the currently installed global allocator
            // is used.

        ~my_Singleton();
            // Destroy this singleton.

      public:
        // CLASS METHODS
        static void initSingleton(const char       *id,
                                  bslma::Allocator *basicAllocator = 0);
            // Initialize the singleton with the specified 'id'.  Optionally
            // specify a 'basicAllocator' used to supply memory.  If
            // 'basicAllocator' is 0, the currently installed global allocator
            // is used.

        static const my_Singleton& singleton();
            // Return a reference to the non-modifiable singleton of this
            // class.  The behavior is undefined unless the singleton has been
            // initialized.

        // ACCESSORS
        const char *id() const;
            // Return the id of this singleton.
    };

    // CLASS METHODS
    inline
    const my_Singleton& my_Singleton::singleton()
    {
        return *s_singleton_p;
    }

    // CREATORS
    inline
    my_Singleton::my_Singleton(const char *id,
                               bslma::Allocator *basicAllocator)
    : d_id(id, bslma::Default::globalAllocator(basicAllocator))
    {
    }

    inline
    my_Singleton::~my_Singleton()
    {
    }

    // ACCESSORS
    inline
    const char *my_Singleton::id() const
    {
        return d_id.id();
    }
//..
// The following completes the definition of 'my_Singleton' in the component
// '.cpp' file:
//..
    // my_singleton.cpp
//  #include <my_singleton.h>
//  #include <bsls_alignedbuffer.h>

    my_Singleton *my_Singleton::s_singleton_p;

    // CLASS METHODS
    void my_Singleton::initSingleton(const char       *id,
                                     bslma::Allocator *basicAllocator)
    {
        static bsls::AlignedBuffer<sizeof(my_Singleton)> singleton;
        s_singleton_p = new (singleton.buffer()) my_Singleton(id,
                                                              basicAllocator);
    }
//..

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int                 test = argc > 1 ? atoi(argv[1]) : 0;
    bool             verbose = argc > 2;
    bool         veryVerbose = argc > 3;
    bool     veryVeryVerbose = argc > 4;
    bool veryVeryVeryVerbose = argc > 5;

    (void)veryVeryVerbose;       // suppress unused variable warning
    (void)veryVeryVeryVerbose;   // suppress unused variable warning

    bslma::NewDeleteAllocator *NDA = &bslma::NewDeleteAllocator::singleton();

    my_CountingAllocator mU;  bslma::Allocator *U = &mU;
    my_CountingAllocator mV;  bslma::Allocator *V = &mV;

    ASSERT(NDA != U);  ASSERT(NDA != V);  ASSERT(U != V);

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:  // Zero is always the leading case.
      case 12: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE 3
        //
        // Concerns:
        //   Usage example #3 provided in the component header file must
        //   compile, link, and run on all platforms as shown.
        //
        // Plan:
        //   Incorporate usage example #3 from header into driver, remove
        //   leading comment characters, and replace 'assert' with 'ASSERT'.
        //
        // Testing:
        //   USAGE EXAMPLE 3
        // --------------------------------------------------------------------

        if (verbose) printf("\nUSAGE EXAMPLE 3"
                            "\n===============\n");

// In the following, the default and global allocators are set to distinct
// instances of 'my_CountingAllocator'.  Note that the default allocator is set
// and locked identically to what was done in the previous two usage examples:
//..
    static my_CountingAllocator defaultCountingAllocator;

    int status = bslma::Default::setDefaultAllocator(&defaultCountingAllocator);
    ASSERT(0 == status);
    bslma::Default::lockDefaultAllocator();
    ASSERT(bslma::Default::defaultAllocator() == &defaultCountingAllocator);

    static my_CountingAllocator globalCountingAllocator;

    bslma::Default::setGlobalAllocator(&globalCountingAllocator);
    ASSERT(bslma::Default::globalAllocator() == &globalCountingAllocator);
//..
// Finally, we initialize the singleton object.  We explicitly specify the
// desired allocator in the call to 'initSingleton' to make our intentions as
// clear as possible.  Of course, because of the way the 'my_Singleton'
// constructor was written, the result would have been the same if no allocator
// had been specified.  As in previous examples, the states of the default and
// global allocators are asserted before and after initializing the singleton:
//..
    ASSERT(0 == defaultCountingAllocator.numBlocksInUse());
    ASSERT(0 == defaultCountingAllocator.numBlocksTotal());
    ASSERT(0 == globalCountingAllocator.numBlocksInUse());
    ASSERT(0 == globalCountingAllocator.numBlocksTotal());

    my_Singleton::initSingleton("S", bslma::Default::globalAllocator());

    ASSERT(0 == defaultCountingAllocator.numBlocksInUse());
    ASSERT(0 == defaultCountingAllocator.numBlocksTotal());
    ASSERT(1 == globalCountingAllocator.numBlocksInUse());
    ASSERT(1 == globalCountingAllocator.numBlocksTotal());
//..

      } break;
      case 11: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE 2
        //
        // Concerns:
        //   Usage example #2 provided in the component header file must
        //   compile, link, and run on all platforms as shown.
        //
        // Plan:
        //   Incorporate usage example #2 from header into driver, remove
        //   leading comment characters, and replace 'assert' with 'ASSERT'.
        //
        // Testing:
        //   USAGE EXAMPLE 2
        // --------------------------------------------------------------------

        if (verbose) printf("\nUSAGE EXAMPLE 2"
                            "\n===============\n");

//
// Next, the default allocator is set and locked identically to what was done
// in usage example 1:
//..
    static my_CountingAllocator defaultCountingAllocator;

    int status = bslma::Default::setDefaultAllocator(&defaultCountingAllocator);
    ASSERT(0 == status);
    bslma::Default::lockDefaultAllocator();
    ASSERT(bslma::Default::defaultAllocator() == &defaultCountingAllocator);
//..
// Now we instantiate an instance of 'my_IdPair' without explicitly specifying
// an allocator.  As a result, the object uses the default allocator.  The
// assertions verify the expected changes in the state of the default
// allocator:
//..
    ASSERT(0 == defaultCountingAllocator.numBlocksInUse());
    ASSERT(0 == defaultCountingAllocator.numBlocksTotal());
    {
        my_IdPair idPair("A", "B");
        ASSERT(2 == defaultCountingAllocator.numBlocksInUse());
        ASSERT(2 == defaultCountingAllocator.numBlocksTotal());
    }
    ASSERT(0 == defaultCountingAllocator.numBlocksInUse());
    ASSERT(2 == defaultCountingAllocator.numBlocksTotal());
//..
// Next we instantiate a second instance of 'my_IdPair', this time supplying it
// with an instance of the counting allocator that is distinct from the default
// allocator.  The assertions in the following code fragment that are commented
// out indicate the *expected* states of the allocators (i.e., in a bug-free
// implementation of 'my_IdPair') after the object has been constructed and
// again after it has been destroyed.  However, due to the (intentional) bug in
// the constructor, the uncommented assertions reveal the *true* state of
// affairs:
//..
    my_CountingAllocator objectCountingAllocator;
    ASSERT(0 == objectCountingAllocator.numBlocksInUse());
    ASSERT(0 == objectCountingAllocator.numBlocksTotal());
    {
        my_IdPair idPair("X", "Y", &objectCountingAllocator);
        // ASSERT(2 == objectCountingAllocator.numBlocksInUse());
        // ASSERT(2 == objectCountingAllocator.numBlocksTotal());
        // ASSERT(0 == defaultCountingAllocator.numBlocksInUse());
        // ASSERT(2 == defaultCountingAllocator.numBlocksTotal());
        ASSERT(1 == objectCountingAllocator.numBlocksInUse());
        ASSERT(1 == objectCountingAllocator.numBlocksTotal());
        ASSERT(1 == defaultCountingAllocator.numBlocksInUse());
        ASSERT(3 == defaultCountingAllocator.numBlocksTotal());
    }
    // ASSERT(0 == objectCountingAllocator.numBlocksInUse());
    // ASSERT(2 == objectCountingAllocator.numBlocksTotal());
    // ASSERT(0 == defaultCountingAllocator.numBlocksInUse());
    // ASSERT(2 == defaultCountingAllocator.numBlocksTotal());
    ASSERT(0 == objectCountingAllocator.numBlocksInUse());
    ASSERT(1 == objectCountingAllocator.numBlocksTotal());
    ASSERT(0 == defaultCountingAllocator.numBlocksInUse());
    ASSERT(3 == defaultCountingAllocator.numBlocksTotal());
//..
// Note that, although not necessary in the case of the simple 'my_IdPair'
// class, the default allocator can be used (and typically *should* be used)
// within the body of a constructor, or any other member function, to allocate
// dynamic memory that is *temporarily* needed by the method (and, hence, not
// owned by the object after the method has returned).  Thus, the invariant
// that must hold immediately after a method of an object returns is that the
// value returned by 'defaultCountingAllocator.numBlocksInUse()' must be
// *identical* to what it was immediately prior to calling the method.  Of
// course, note that the above invariant pertains to cases in *single*-threaded
// programs where the object allocator in use by the instance is *distinct*
// from the default allocator.  Also note that the value returned by
// 'defaultCountingAllocator.numBlocksTotal()' *can* differ across function
// invocations (i.e., even with correct code).

      } break;
      case 10: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE 1
        //
        // Concerns:
        //   Usage example #1 provided in the component header file must
        //   compile, link, and run on all platforms as shown.
        //
        // Plan:
        //   Incorporate usage example #1 from header into driver, remove
        //   leading comment characters, and replace 'assert' with 'ASSERT'.
        //
        // Testing:
        //   USAGE EXAMPLE 1
        // --------------------------------------------------------------------

        if (verbose) printf("\nUSAGE EXAMPLE 1"
                            "\n===============\n");

// Next we set the default allocator to an instance of our counting allocator.
// Note that immediately after successfully setting it, we lock the default
// allocator, so that subsequent calls to 'bslma::Default::setDefaultAllocator'
// fail.  (The default allocator can still be modified by calling
// 'bslma::Default::setDefaultAllocatorRaw', but calling that function in
// production code is anti-social.  Our usage examples expressly do *not* call
// that method.)  With the possible exception of test drivers, the default
// allocator should be set and locked early in 'main' before threads are
// started and before objects are initialized:
//..
    static my_CountingAllocator defaultCountingAllocator;

    int status = bslma::Default::setDefaultAllocator(&defaultCountingAllocator);
    ASSERT(0 == status);
    bslma::Default::lockDefaultAllocator();  // subsequent calls to "set" fail
    ASSERT(bslma::Default::defaultAllocator() == &defaultCountingAllocator);

    status = bslma::Default::setDefaultAllocator(
                                      &bslma::NewDeleteAllocator::singleton());
    ASSERT(0 != status);
    ASSERT(bslma::Default::defaultAllocator() == &defaultCountingAllocator);
//..
// In the following, we instantiate two instances of 'my_Id'.  The first
// instance, 'idA', is not supplied with an allocator, so it uses the default
// allocator.  The second instance, 'idB', is supplied with an instance of
// 'my_CountingAllocator'.  The assertions track the states of the two
// allocators at each point in the code fragment.  In particular, note that the
// state of the default allocator does not change during the lifetime of 'idB':
//..
    ASSERT(0 == defaultCountingAllocator.numBlocksInUse());
    ASSERT(0 == defaultCountingAllocator.numBlocksTotal());
    {
        my_Id id("A");
        ASSERT(1 == defaultCountingAllocator.numBlocksInUse());
        ASSERT(1 == defaultCountingAllocator.numBlocksTotal());
    }
    ASSERT(0 == defaultCountingAllocator.numBlocksInUse());
    ASSERT(1 == defaultCountingAllocator.numBlocksTotal());

    my_CountingAllocator objectCountingAllocator;
    ASSERT(0 == objectCountingAllocator.numBlocksInUse());
    ASSERT(0 == objectCountingAllocator.numBlocksTotal());
    {
        my_Id idB("B", &objectCountingAllocator);
        ASSERT(1 == objectCountingAllocator.numBlocksInUse());
        ASSERT(1 == objectCountingAllocator.numBlocksTotal());
        ASSERT(0 == defaultCountingAllocator.numBlocksInUse());
        ASSERT(1 == defaultCountingAllocator.numBlocksTotal());
    }
    ASSERT(0 == objectCountingAllocator.numBlocksInUse());
    ASSERT(1 == objectCountingAllocator.numBlocksTotal());
    ASSERT(0 == defaultCountingAllocator.numBlocksInUse());
    ASSERT(1 == defaultCountingAllocator.numBlocksTotal());
//..

      } break;
      case 9: {
        // --------------------------------------------------------------------
        // TESTING GLOBAL ALLOCATOR
        //
        // Concerns:
        //   1) Initially, the global allocator is the address of the
        //      'bslma::NewDeleteAllocator' singleton.
        //   2) 'globalAllocator', called with no argument, always returns the
        //      global allocator that is in effect at the point of call.
        //   3) 'globalAllocator', called with a non-zero argument, always
        //      returns that argument and has no effect on the global
        //      allocator.
        //   4) 'setGlobalAllocator' unconditionally sets the global allocator
        //      and returns the global allocator that was in effect prior to
        //      the call.
        //   5) 'setGlobalAllocator', called with a 0 argument, resets the
        //      global allocator to the address of the
        //      'bslma::NewDeleteAllocator' singleton.
        //
        // Plan:
        //   Call 'globalAllocator', without an argument, in the first
        //   substantive line of the test case to verify that the global
        //   allocator is initially the address of the
        //   'bslma::NewDeleteAllocator' singleton.  Subsequently, call
        //   'globalAllocator', with and without an argument, and
        //   'setGlobalAllocator' with various values and assert that the value
        //   returned by each method is as expected.  Finally test that calling
        //   'setGlobalAllocator' with a 0 argument resets the global allocator
        //   to its initial setting.
        //
        // Testing:
        //   bslma::Allocator *globalAllocator(*ba = 0);
        //   bslma::Allocator *setGlobalAllocator(*ba);
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING GLOBAL ALLOCATOR"
                            "\n========================\n");

        ASSERT(NDA == Obj::globalAllocator());
        ASSERT(NDA == Obj::globalAllocator(NDA));
        ASSERT(  U == Obj::globalAllocator(U));
        ASSERT(NDA == Obj::globalAllocator());

        ASSERT(NDA == Obj::setGlobalAllocator(NDA));
        ASSERT(NDA == Obj::globalAllocator());
        ASSERT(  U == Obj::globalAllocator(U));
        ASSERT(NDA == Obj::globalAllocator(NDA));
        ASSERT(NDA == Obj::globalAllocator());

        ASSERT(NDA == Obj::setGlobalAllocator(U));
        ASSERT(  U == Obj::globalAllocator());
        ASSERT(  V == Obj::globalAllocator(V));
        ASSERT(NDA == Obj::globalAllocator(NDA));
        ASSERT(  U == Obj::globalAllocator());

        ASSERT(  U == Obj::setGlobalAllocator(V));
        ASSERT(  V == Obj::globalAllocator());
        ASSERT(  U == Obj::globalAllocator(U));
        ASSERT(NDA == Obj::globalAllocator(NDA));
        ASSERT(  V == Obj::globalAllocator());

        ASSERT(  V == Obj::setGlobalAllocator(0));
        ASSERT(NDA == Obj::globalAllocator());
        ASSERT(  U == Obj::globalAllocator(U));
        ASSERT(NDA == Obj::globalAllocator(NDA));
        ASSERT(NDA == Obj::globalAllocator());

      } break;
      case 8: {
        // --------------------------------------------------------------------
        // TESTING 'allocator' SIDE-EFFECTS
        //
        // Concerns:
        //   1) Calling 'allocator' with a non-zero argument does *not* lock
        //      the default allocator.
        //   2) 'allocator', called with no argument, has the side-effect of
        //      locking the default allocator, if not already locked, to that
        //      which is in effect at the point of call.  In particular, the
        //      default allocator can be locked in this manner to something
        //      other than its initial setting.
        //
        // Plan:
        //   Call 'allocator' with a non-zero argument, then call
        //   'setDefaultAllocator' to set the default allocator to a value
        //   distinct from its initial setting.  Next call 'allocator' without
        //   an argument to lock the default allocator to this new
        //   (non-initial) setting.  Verify that the default allocator is
        //   locked by calling 'setDefaultAllocator' and asserting that the
        //   call fails.
        //
        // Testing:
        //   bslma::Allocator *allocator(*ba = 0);
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING 'allocator' Side-Effects"
                            "\n================================\n");

        ASSERT(V == Obj::allocator(V));
        ASSERT(0 == Obj::setDefaultAllocator(U));
        ASSERT(U == Obj::allocator());
        ASSERT(0 != Obj::setDefaultAllocator(V));
        ASSERT(U == Obj::allocator());

      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING 'allocator' SIDE-EFFECTS
        //
        // Concerns:
        //   'allocator', called with no argument, has the side-effect of
        //   locking the default allocator, if not already locked, to that
        //   which is in effect at the point of call.  In particular, the
        //   default allocator can be locked in this manner to its initial
        //   setting.
        //
        // Plan:
        //   Call 'allocator' without an argument in the first substantive line
        //   of the test case to lock the default allocator to its initial
        //   setting, namely the 'bslma::NewDeleteAllocator' singleton.  Verify
        //   that the default allocator is locked by calling
        //   'setDefaultAllocator' and asserting that the call fails.
        //
        // Testing:
        //   bslma::Allocator *allocator(*ba = 0);
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING 'allocator' Side-Effects"
                            "\n================================\n");

        ASSERT(NDA == Obj::allocator());
        ASSERT(  0 != Obj::setDefaultAllocator(U));
        ASSERT(NDA == Obj::allocator());

      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING 'defaultAllocator' SIDE-EFFECTS
        //
        // Concerns:
        //   'defaultAllocator' has the side-effect of locking the default
        //   allocator, if not already locked, to that which is in effect at
        //   the point of call.  In particular, the default allocator can be
        //   locked in this manner to something other than its initial setting.
        //
        // Plan:
        //   Call 'setDefaultAllocator' to set the default allocator to a value
        //   distinct from its initial setting, then call 'defaultAllocator' to
        //   lock the default allocator to this new (non-initial) setting.
        //   Verify that the default allocator is locked by calling
        //   'setDefaultAllocator' and asserting that the call fails.
        //
        // Testing:
        //   bslma::Allocator *defaultAllocator();
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING 'defaultAllocator' Side-Effects"
                            "\n=======================================\n");

        ASSERT(0 == Obj::setDefaultAllocator(U));
        ASSERT(U == Obj::defaultAllocator());
        ASSERT(0 != Obj::setDefaultAllocator(V));
        ASSERT(U == Obj::defaultAllocator());

      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING 'defaultAllocator' SIDE-EFFECTS
        //
        // Concerns:
        //   'defaultAllocator' has the side-effect of locking the default
        //   allocator, if not already locked, to that which is in effect at
        //   the point of call.  In particular, the default allocator can be
        //   locked in this manner to its initial setting.
        //
        // Plan:
        //   Call 'defaultAllocator' in the first substantive line of the test
        //   case to lock the default allocator to its initial setting, namely
        //   the 'bslma::NewDeleteAllocator' singleton.  Verify that the
        //   default allocator is locked by calling 'setDefaultAllocator' and
        //   asserting that the call fails.
        //
        // Testing:
        //   bslma::Allocator *defaultAllocator();
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING 'defaultAllocator' Side-Effects"
                            "\n=======================================\n");

        ASSERT(NDA == Obj::defaultAllocator());
        ASSERT(  0 != Obj::setDefaultAllocator(U));
        ASSERT(NDA == Obj::defaultAllocator());

      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING 'allocator' METHOD
        //
        // Concerns:
        //   1) 'allocator', called with no argument, always returns the
        //      default allocator that is in effect at the point of call.
        //   2) 'allocator', called with a non-zero argument, always returns
        //      that argument and has no effect on the default allocator.
        //
        // Plan:
        //   Call 'allocator', without an argument, in the first substantive
        //   line of the test case to verify that the default allocator is
        //   initially the address of the 'bslma::NewDeleteAllocator'
        //   singleton.  Subsequently, use 'setDefaultAllocatorRaw' to set the
        //   default allocator to various values and verify that 'allocator',
        //   when called without an argument, returns the expected result.
        //   Regularly call 'allocator' *with* an argument throughout the test
        //   case, and verify that the argument is returned and that the
        //   setting of the default allocator is not affected.  Note that
        //   side-effects of 'allocator' are ignored in this test case.
        //
        // Testing:
        //   bslma::Allocator *allocator(*ba = 0);     (ignore side-effects)
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING 'allocator'"
                            "\n===================\n");

        if (veryVerbose) printf("\tIgnoring 'allocator' side-effects.\n");

        ASSERT(NDA == Obj::allocator());
        ASSERT(  U == Obj::allocator(U));
        ASSERT(NDA == Obj::allocator(NDA));
        ASSERT(NDA == Obj::allocator());

        Obj::setDefaultAllocatorRaw(U);
        ASSERT(  U == Obj::allocator());
        ASSERT(  V == Obj::allocator(V));
        ASSERT(NDA == Obj::allocator(NDA));
        ASSERT(  U == Obj::allocator());

        Obj::setDefaultAllocatorRaw(V);
        ASSERT(  V == Obj::allocator());
        ASSERT(  U == Obj::allocator(U));
        ASSERT(NDA == Obj::allocator(NDA));
        ASSERT(  V == Obj::allocator());

        Obj::setDefaultAllocatorRaw(NDA);
        ASSERT(NDA == Obj::allocator());
        ASSERT(  U == Obj::allocator(U));
        ASSERT(NDA == Obj::allocator(NDA));
        ASSERT(NDA == Obj::allocator());

      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING 'setDefaultAllocator' AND 'lockDefaultAllocator' METHODS
        //
        // Concerns:
        //   1) 'setDefaultAllocator' succeeds until the default allocator is
        //      locked.
        //   2) 'lockDefaultAllocator' locks the default allocator such that
        //      all subsequent calls to 'setDefaultAllocator' fail.
        //   3) Calling 'lockDefaultAllocator' after the default allocator is
        //      locked has no effect.
        //   4) The default allocator may be set by 'setDefaultAllocatorRaw'
        //      even if it is locked.
        //
        // Plan:
        //   Call 'setDefaultAllocator' in each of the first two substantive
        //   lines of the test case and verify that they both succeed (but
        //   do *not* call 'defaultAllocator' until later in the test as that
        //   has the side-effect of locking the default allocator).  Then call
        //   'lockDefaultAllocator' and verify that a subsequent call to
        //   'setDefaultAllocator' fails, and also verify that
        //   'defaultAllocator' returns the value that was passed to the most
        //   recent *successful* call to 'setDefaultAllocator'.  Include
        //   individual tests to verify that a subsequent call to
        //   'lockDefaultAllocator' has no effect and that
        //   'setDefaultAllocatorRaw' may be used to set the default allocator
        //   even after it has been locked.
        //
        // Testing:
        //   int setDefaultAllocator(*ba);
        //   void lockDefaultAllocator();
        // --------------------------------------------------------------------

        if (verbose)
            printf("\nTESTING 'setDefaultAllocator', 'lockDefaultAllocator'"
                   "\n====================================================="
                   "\n");

        ASSERT(0 == Obj::setDefaultAllocator(U));
        ASSERT(0 == Obj::setDefaultAllocator(V));
        Obj::lockDefaultAllocator();
        ASSERT(0 != Obj::setDefaultAllocator(U));
        ASSERT(V == Obj::defaultAllocator());

        Obj::lockDefaultAllocator();
        ASSERT(0 != Obj::setDefaultAllocator(U));
        ASSERT(V == Obj::defaultAllocator());

        Obj::setDefaultAllocatorRaw(U);
        ASSERT(U == Obj::defaultAllocator());
        ASSERT(0 != Obj::setDefaultAllocator(V));

        if (verbose) printf("\nNegative testing\n");

        bsls::AssertTestHandlerGuard guard;

        ASSERT_FAIL(Obj::setDefaultAllocator(0));
        ASSERT_PASS(Obj::setDefaultAllocator(U));

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // BOOTSTRAP TEST
        //
        // Concerns:
        //   1) Initially, the default allocator is the address of the
        //      'bslma::NewDeleteAllocator' singleton.
        //   2) 'defaultAllocator' always returns the default allocator that is
        //      in effect at the point of call.
        //   3) 'setDefaultAllocatorRaw' unconditionally sets the default
        //      allocator.
        //
        // Plan:
        //   Call 'defaultAllocator' in the first substantive line of the test
        //   case to verify that the default allocator is initially the address
        //   of the 'bslma::NewDeleteAllocator' singleton.  Subsequently, use
        //   'setDefaultAllocatorRaw' to set the default allocator to various
        //   values and verify that 'defaultAllocator' returns the expected
        //   result.  Note that side-effects of 'defaultAllocator' are ignored
        //   in this test case.
        //
        // Testing:
        //   void setDefaultAllocatorRaw(*ba);
        //   bslma::Allocator *defaultAllocator();   (ignore side-effects)
        // --------------------------------------------------------------------

        if (verbose) printf("\nBOOTSTRAP TEST"
                            "\n==============\n");

        if (veryVerbose) {
            printf("\tTesting 'setDefaultAllocatorRaw', "
                   "'defaultAllocator'.\n");
            printf("\tIgnoring 'defaultAllocator' side-effects.\n");
        }

        ASSERT(NDA == Obj::defaultAllocator());

        Obj::setDefaultAllocatorRaw(U);
        ASSERT(  U == Obj::defaultAllocator());

        Obj::setDefaultAllocatorRaw(V);
        ASSERT(  V == Obj::defaultAllocator());

        Obj::setDefaultAllocatorRaw(NDA);
        ASSERT(NDA == Obj::defaultAllocator());

        if (verbose) printf("\nNegative testing\n");

        bsls::AssertTestHandlerGuard guard;

        ASSERT_FAIL(Obj::setDefaultAllocatorRaw(0));
        ASSERT_PASS(Obj::setDefaultAllocatorRaw(U));

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //
        // Concerns:
        //   The primary concern is that basic functionality is correct such
        //   that the component may be tested systematically in subsequent test
        //   cases.
        //
        // Plan:
        //   Breath all methods, except for 'lockDefaultAllocator', and assert
        //   the expected results.  Note that it is useless to test the lock
        //   method in this breathing test given the side-effects of
        //   'allocator', when called with no argument, and 'defaultAllocator'.
        //
        // Testing:
        //   This test exercises basic functionality.
        // --------------------------------------------------------------------

        if (verbose) printf("\nBREATHING TEST"
                            "\n==============\n");

        if (veryVerbose) printf("\tBreathing default allocator.\n");

        ASSERT(NDA == Obj::defaultAllocator());  // locks default
        ASSERT(NDA == Obj::allocator());
        ASSERT(  V == Obj::allocator(V));

        ASSERT(  0 != Obj::setDefaultAllocator(U));
        ASSERT(NDA == Obj::defaultAllocator());
        ASSERT(NDA == Obj::allocator());
        ASSERT(  V == Obj::allocator(V));

        Obj::setDefaultAllocatorRaw(U);
        ASSERT(  U == Obj::defaultAllocator());
        ASSERT(  U == Obj::allocator());
        ASSERT(  V == Obj::allocator(V));

        if (veryVerbose) printf("\tBreathing global allocator.\n");

        ASSERT(NDA == Obj::globalAllocator());
        ASSERT(  V == Obj::globalAllocator(V));

        ASSERT(NDA == Obj::setGlobalAllocator(U));
        ASSERT(  U == Obj::globalAllocator());
        ASSERT(  V == Obj::globalAllocator(V));

        ASSERT(  U == Obj::setGlobalAllocator(0));
        ASSERT(NDA == Obj::globalAllocator());
        ASSERT(  V == Obj::globalAllocator(V));

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
