// bslma_rawdeleterproctor.t.cpp                                      -*-C++-*-

#include <bslma_rawdeleterproctor.h>

#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bslma_testallocator.h>

#include <bsls_bsltestutil.h>

#include <stdio.h>      // 'printf'
#include <stdlib.h>     // 'atoi'

#include <string>   // breathing test, should be replaceable

using namespace BloombergLP;

//=============================================================================
//                                 TEST PLAN
//-----------------------------------------------------------------------------
//                                  Overview
//                                  --------
// We are testing a proctor object to ensure that it destroys the managed
// objects and deallocates memory used if release is not called before the
// proctor object goes out of scope.  We achieve this goal by creating objects
// of a user-defined type that are each initialized with the address of a
// unique counter, using a 'bslma::TestAllocator'.  As each object is
// destroyed, its destructor increments the counter held by the object,
// indicating the number of times the object's destructor is called.  After
// the proctor is destroyed, we verify that the corresponding counters of the
// object managed by the proctor are modified, and all allocated memory are
// deallocated.
//-----------------------------------------------------------------------------
// [3] bslma::RawDeleterProctor<TYPE, ALLOCATOR>(obj, allocator);
// [3] ~bslma::RawDeleterProctor<TYPE, ALLOCATOR>();
// [4] void release();
// [5] void reset(obj);
//-----------------------------------------------------------------------------
// [1] Breathing Test
// [2] Helper Class: 'my_Class'
// [2] Helper Class: 'my_Pool'
// [3] Concern: the (non-virtual) 'deallocate' method for pools is also invoked
// [6] Usage Example
//=============================================================================

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

//=============================================================================
//                          HELPER CLASS FOR TESTING
//-----------------------------------------------------------------------------

class my_Pool {
    // This class provides a 'deallocate' method, used to exercise the
    // contract promised by the destructor of the 'bslma::RawDeleterGuard'.
    // This object indicates that its 'deallocate' method is called by
    // incrementing the global counter (supplied at construction) that it
    // *holds*.

    // DATA
    int *d_counter_p;  // counter to be incremented when 'deallocate' is called

  public:
    // CREATORS
    explicit my_Pool(int *counter) : d_counter_p(counter) {}
        // Create this object holding the specified (global) counter.

    // MANIPULATORS
    void deallocate(void *) { ++*d_counter_p; }
        // Increment this object's counter.
};

class my_Class {
    // This object indicates that its destructor is called by incrementing the
    // global counter (supplied at construction) that it holds.

    // DATA
    int *d_counter_p;  // (global) counter to be incremented at destruction

  public:
    // CREATORS
    explicit my_Class(int *counter) : d_counter_p(counter) {}
        // Create this object using the address of the specified 'counter' to
        // be held.

    ~my_Class() { ++*d_counter_p; }
        // Destroy this object and increment this object's (global) counter.
};

// Testing Single Inheritance
class myParent {
    int x;
  public:
    myParent()          {}
    virtual ~myParent() {}
};

class myChild : public myParent {
    my_Class c;
  public:
    explicit myChild(int *counter = 0) : c(counter) {}
    ~myChild() {}
};

// Testing Multiple Inheritance
class myVirtualBase {
    int x;
  public:
    myVirtualBase()          { }
    virtual ~myVirtualBase() { }
};

class myLeftBase : virtual public myVirtualBase {
    int x;
  public:
    myLeftBase()             { }
    virtual ~myLeftBase()    { }
};

class myRightBase : virtual public myVirtualBase {
    int x;
  public:
    myRightBase()            { }
    virtual ~myRightBase()   { }
};

class myMostDerived : public myLeftBase, public myRightBase {
    my_Class c;
  public:
    explicit myMostDerived(int *counter = 0) : c(counter) {}
    ~myMostDerived() {}
};

//=============================================================================
//                                USAGE EXAMPLE
//-----------------------------------------------------------------------------
// 'bslma::RawDeleterProctor' is normally used to achieve *exception* *safety*
// in an *exception* *neutral* way by managing objects that are created
// temporarily on the heap, but not yet committed to a container object's
// management.  This (somewhat contrived) example illustrates the use of a
// 'bslma::RawDeleterProctor' to manage a dynamically-allocated object,
// deleting the object automatically should an exception occur.
//
// Suppose we have a simple linked list class that manages objects of
// parameterized 'TYPE', but which are (for the purpose of this example)
// allocated separately from the links that hold them (thereby requiring two
// separate allocations for each 'append' operation):
//..
// my_list.h
// ...

template <class TYPE>
class my_List {
    // This class is a container that uses a linked list data structure to
    // manage objects of parameterized 'TYPE'.

    // PRIVATE TYPES
    struct Link {
        TYPE *d_object_p;  // object held by the link
        Link *d_next_p;    // next link
    };

    // DATA
    Link             *d_head_p;       // head of list
    Link             *d_tail_p;       // tail of list
    int               d_length;       // number of objects
    bslma::Allocator *d_allocator_p;  // allocator (held, not owned)

  public:
    // CREATORS
    my_List(bslma::Allocator *basicAllocator = 0);
        // Create a 'my_List' object having an initial length of 0.
        // Optionally specify a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator
        // is used.

    // ...

    ~my_List();
        // Destroy this 'my_List' object and all elements currently stored.

    // MANIPULATORS
    // ...

    void append(const TYPE& object);
        // Append (a copy of) the specified 'object' of parameterized
        // 'TYPE' to (the end of) this list.

    // ...
};
//..
// Note that the rest of the 'my_List' interface (above) and implementation
// (below) are omitted as the portion shown is sufficient to demonstrate the
// use of 'bslma::RawDeleterProctor'.
//..
// CREATORS
template <class TYPE>
inline
my_List<TYPE>::my_List(bslma::Allocator *basicAllocator)
: d_head_p(0)
, d_tail_p(0)
, d_length(0)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

template <class TYPE>
my_List<TYPE>::~my_List()
{
    while (d_head_p) {
        Link *tmp = d_head_p;
        d_head_p  = d_head_p->d_next_p;
        d_allocator_p->deleteObject(tmp->d_object_p);
        d_allocator_p->deallocate(tmp);
    }
}

// MANIPULATORS
template <class TYPE>
void my_List<TYPE>::append(const TYPE& object)
{
    // !!!Warning: Modified since 'my_Class' does not take an allocator.
    TYPE *tmp = (TYPE *)new(*d_allocator_p) TYPE(object);
                                                        // possibly throw

    //************************************************************
    // Note the use of the raw deleter proctor on 'tmp' (below). *
    //************************************************************

    bslma::RawDeleterProctor<TYPE, bslma::Allocator> proctor(tmp,
                                                             d_allocator_p);

    if (!d_head_p) {
        d_head_p           = new(*d_allocator_p) Link;  // possibly throw
        d_tail_p           = d_head_p;
    }
    else {
        d_tail_p->d_next_p = new(*d_allocator_p) Link;  // possibly throw
        d_tail_p           = d_tail_p->d_next_p;
    }
    d_tail_p->d_object_p   = tmp;
    d_tail_p->d_next_p     = 0;

    //*********************************************************
    // Note that the raw deleter proctor is released (below). *
    //*********************************************************

    proctor.release();
}
//..
// The 'append' method defined above potentially throws in three places.  If
// the memory allocator held in 'd_allocator_p' were to throw while attempting
// to create the object of parameterized 'TYPE', no memory would be leaked.
// But without subsequent use of the 'bslma::RawDeleterProctor', if the
// allocator subsequently throws while creating the link, all memory (and any
// other resources) acquired as a result of copying the (not-yet-managed)
// object would be leaked.  Using the 'bslma::RawDeleterProctor' prevents the
// leaks by deleting the proctored object automatically should the proctor go
// out of scope before the 'release' method of the proctor is called (such as
// when the function exits prematurely due to an exception).
//
// Note that the 'append' method assumes the copy constructor of 'TYPE' takes
// an allocator as a second argument.  In production code, a constructor proxy
// that checks the traits of 'TYPE' (to determine whether 'TYPE' uses
// 'bslma::Allocator') should be used (see bslalg_constructorproxy).

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

    (void)veryVerbose;           // suppress unused variable warning
    (void)veryVeryVerbose;       // suppress unused variable warning

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:  // Zero is always the leading case.
      case 6: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE TEST
        //
        // Concerns:
        //   The usage example provided in the component header file must
        //   compile, link, and run on all platforms as shown.
        //
        // Plan:
        //   Run the usage example and exercise the creators and manipulators
        //   of 'my_List' using a 'bslma::TestAllocator' to verify that memory
        //   is allocated and deallocated properly.
        //
        // Testing:
        //   Usage example
        // --------------------------------------------------------------------

        if (verbose) printf("\nUSAGE EXAMPLE"
                            "\n=============\n");

#if !defined(BDE_BUILD_TARGET_EXC)
        if (verbose) printf("Test not run without exception support.\n");
#else
        bslma::TestAllocator z(veryVeryVeryVerbose);
        const bslma::TestAllocator &Z = z;
        int counter = 0;

        {
            bsls::Types::Int64 numBytesUsed = 0;

            // Initialize the list with some objects
            my_List<my_Class> list(&z);
            list.append(my_Class(&counter));
            list.append(my_Class(&counter));
            list.append(my_Class(&counter));

            // Verify that the destructor is called for each of the
            // temporaries.
            ASSERT(3 == counter);

            try {

                numBytesUsed = Z.numBytesInUse();

                // Make sure the allocation for the 'link' fails.
                z.setAllocationLimit(1);

                list.append(my_Class(&counter));

                ASSERT(0);  // should never get here
            }
            catch (...) {

                // Verify that the destructor of 'my_Class' is called for both
                // the temporary and the one created within the 'list' object.
                ASSERT(5 == counter);

                // Verify that the memory allocated for the construction of
                // 'my_Class' is properly deallocated.
                ASSERT(numBytesUsed == Z.numBytesInUse());
            }
        }
        // Verify that the destructor for the other 3 objects used to
        // initialize the list is called.
        ASSERT(8 == counter);

        // Verify all allocated memory are deallocated.
        ASSERT(0 == Z.numBytesInUse());
#endif
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // 'reset' TEST
        //
        // Concerns:
        //   Verify that when the 'reset' method is called, the proctor
        //   object properly manages a different object.
        //
        // Plan:
        //   Create a 'my_Class' object using a 'bslma::TestAllocator' and
        //   initialize it with a counter.  Next create another 'my_Class'
        //   object and initialize it with a different counter.  Finally
        //   initialize a 'bslma::RawDeleterProctor' object with the first
        //   object and 'bslma::TestAllocator'.  Call 'reset' on the proctor
        //   with the second object before it goes out of scope.  Once the
        //   proctor goes out of scope, verify that only the second counter is
        //   incremented, and only the memory allocated by the second test
        //   allocator is deallocated.
        //
        // Testing:
        //   void reset(obj);
        // --------------------------------------------------------------------

        if (verbose) printf("\n'reset' TEST"
                            "\n============\n");

        bslma::TestAllocator z(veryVeryVeryVerbose);
        const bslma::TestAllocator& Z = z;

        int counter1 = 0;
        int counter2 = 0;
        if (verbose) printf("\nTesting the 'reset' method\n");

        my_Class *pC1;
        {
            pC1 = new(z) my_Class(&counter1);
            my_Class *pC2 = new(z) my_Class(&counter2);
            ASSERT(0                    == counter1);
            ASSERT(0                    == counter2);
            ASSERT(2 * sizeof(my_Class) == Z.numBytesInUse());

            bslma::RawDeleterProctor<my_Class, bslma::Allocator> proctor(pC1,
                                                                         &z);
            ASSERT(0                    == counter1);
            ASSERT(0                    == counter2);
            ASSERT(2 * sizeof(my_Class) == Z.numBytesInUse());

            proctor.reset(pC2);
            ASSERT(0                    == counter1);
            ASSERT(0                    == counter2);
            ASSERT(2 * sizeof(my_Class) == Z.numBytesInUse());
        }
        ASSERT(0                == counter1);
        ASSERT(1                == counter2);
        ASSERT(sizeof(my_Class) == Z.numBytesInUse());

        z.deleteObject(pC1);
        ASSERT(1                 == counter1);
        ASSERT(1                 == counter2);
        ASSERT(Z.numBytesInUse() == 0);

      }break;
      case 4: {
        // --------------------------------------------------------------------
        // 'release' TEST
        //
        // Concerns:
        //   Verify that when the 'release' method is called, the proctor
        //   object properly releases from management the object currently
        //   managed by this proctor.
        //
        // Plan:
        //   Create 'my_Class' objects using 'bslma::TestAllocator' and
        //   initialize it with a counter.  Next initialize a
        //   'bslma::RawDeleterProctor' object with the corresponding
        //   'my_Class' object and 'bslma::TestAllocator'.  Call 'release' on
        //   the proctor before it goes out of scope.  Verify that the counter
        //   is not incremented, and the memory allocated by the test
        //   allocator is not deallocated.
        //
        // Testing:
        //   void release();
        // --------------------------------------------------------------------

        if (verbose) printf("\n'release' TEST"
                            "\n==============\n");

        bslma::TestAllocator z(veryVeryVeryVerbose);
        const bslma::TestAllocator& Z = z;

        int counter = 0;
        if (verbose) printf("\nTesting the 'release' method\n");
        my_Class *pC;
        {
            pC = new(z) my_Class(&counter);
            ASSERT(0                == counter);
            ASSERT(sizeof(my_Class) == Z.numBytesInUse());

            bslma::RawDeleterProctor<my_Class, bslma::Allocator>
                                                               proctor(pC, &z);
            ASSERT(0                == counter);
            ASSERT(sizeof(my_Class) == Z.numBytesInUse());

            proctor.release();
            ASSERT(0                == counter);
            ASSERT(sizeof(my_Class) == Z.numBytesInUse());
        }
        ASSERT(0                == counter);
        ASSERT(sizeof(my_Class) == Z.numBytesInUse());

        z.deleteObject(pC);
        ASSERT(1 == counter);
        ASSERT(0 == Z.numBytesInUse());

      }break;
      case 3: {
        // --------------------------------------------------------------------
        // CTOR / DTOR TEST
        //
        // Concerns:
        //   1) Verify that the proctor object properly deletes different types
        //   of managed objects on destruction using the allocator supplied at
        //   construction.  The concerns are:
        //
        //       a) Single inheritance object with derived class pointer
        //       b) Single inheritance object with base class pointer
        //       c) Multiple inheritance object with derived class pointer
        //
        //   2) When an allocator (or pool) not inherited from
        //   'bslma::Allocator' is supplied to the 'bslma::RawDeleterProctor',
        //   the destructor of the managed object and 'deallocate' method of
        //   the allocator (or pool) supplied is still invoked.
        //
        // Plan:
        //   Create a 'myChild' object, which increments a counter every time
        //   it is destructed, using a 'bslma::TestAllocator' which keeps track
        //   of how many bytes are allocated.  Next create a
        //   'bslma::RawDeleterGuard' object to guard the created 'myChild'
        //   object.  When the guard object goes out of scope, verify that both
        //   the counter is incremented and memory is deallocated.  Next,
        //   repeat by creating 'myParent' object to address concern 1b.  Next,
        //   repeat by creating 'myMostDerived' object to address concern 1c.
        //
        //   Finally, repeat all three steps above using 'my_Pool' as the
        //   parameterized 'ALLCOATOR' type, which does not inherit from
        //   'bslma::Allocator'.
        //
        // Testing:
        //   bslma::RawDeleterProctor<TYPE, ALLOCATOR>(obj, allocator);
        //   ~bslma::RawDeleterProctor<TYPE, ALLOCATOR>();
        //   Concern: the (non-virtual) 'deallocate' method for pools is also
        //            invoked
        // --------------------------------------------------------------------

        if (verbose) printf("\nCTOR / DTOR TEST"
                            "\n================\n");

        bslma::TestAllocator z(veryVeryVeryVerbose);
        const bslma::TestAllocator& Z = z;
        int counter = 0;

        if (verbose) printf("\nTesting with bslma::TestAllocator\n");
        {

            if (verbose) printf("\tTesting single inheritance with derived "
                                 "class pointer\n");
            {
                myChild *pC = new(z) myChild(&counter);
                ASSERT(0               == counter);
                ASSERT(sizeof(myChild) == Z.numBytesInUse());

                bslma::RawDeleterProctor<myChild, bslma::Allocator>
                                                               proctor(pC, &z);
                ASSERT(0               == counter);
                ASSERT(sizeof(myChild) == Z.numBytesInUse());
            }
            ASSERT(1 == counter);
            ASSERT(0 == Z.numBytesInUse());

            counter = 0;  // reset counter

            if (verbose) printf("\tTesting single inheritance with base "
                                 "class pointer\n");
            {
                myParent *pP = new(z) myChild(&counter);
                ASSERT(0               == counter);
                ASSERT(sizeof(myChild) == Z.numBytesInUse());

                bslma::RawDeleterProctor<myParent, bslma::Allocator>
                                                               proctor(pP, &z);
                ASSERT(0               == counter);
                ASSERT(sizeof(myChild) == Z.numBytesInUse());
            }
            ASSERT(1 == counter);
            ASSERT(0 == Z.numBytesInUse());

            counter = 0;  // reset counter

            if (verbose) printf("\tTesting multiple inheritance with derived "
                                 "class pointer\n");
            {
                myMostDerived *pMD = new(z) myMostDerived(&counter);
                ASSERT(0                     == counter);
                ASSERT(sizeof(myMostDerived) == Z.numBytesInUse());

                bslma::RawDeleterProctor<myMostDerived, bslma::Allocator>
                                                              proctor(pMD, &z);
                ASSERT(0                     == counter);
                ASSERT(sizeof(myMostDerived) == Z.numBytesInUse());
            }
            ASSERT(1 == counter);
            ASSERT(0 == Z.numBytesInUse());

            counter = 0;  // reset counter
        }

        if (verbose) printf("\nTesting with my_Pool\n");
        {
            int allocCounter = 0;
            my_Pool myAlloc(&allocCounter);

            if (verbose) printf("\tTesting single inheritance with derived "
                                 "class pointer\n");
            myChild *pC = 0;
            {
                pC = new(z) myChild(&counter);
                ASSERT(0               == counter);
                ASSERT(0               == allocCounter);
                ASSERT(sizeof(myChild) == Z.numBytesInUse());

                bslma::RawDeleterProctor<myChild, my_Pool> proctor(pC,
                                                                   &myAlloc);
                ASSERT(0               == counter);
                ASSERT(0               == allocCounter);
                ASSERT(sizeof(myChild) == Z.numBytesInUse());
            }
            ASSERT(1               == counter);
            ASSERT(1               == allocCounter);
            ASSERT(sizeof(myChild) == Z.numBytesInUse());

            z.deallocate(pC);

            ASSERT(0 == Z.numBytesInUse());

            counter      = 0;  // reset counter
            allocCounter = 0;  // reset counter

            if (verbose) printf("\tTesting single inheritance with base "
                                 "class pointer\n");
            myParent *pP = 0;
            {
                pP = new(z) myChild(&counter);
                ASSERT(0               == counter);
                ASSERT(0               == allocCounter);
                ASSERT(sizeof(myChild) == Z.numBytesInUse());

                bslma::RawDeleterProctor<myParent, my_Pool> proctor(pP,
                                                                    &myAlloc);
                ASSERT(0               == counter);
                ASSERT(0               == allocCounter);
                ASSERT(sizeof(myChild) == Z.numBytesInUse());
            }
            ASSERT(1               == counter);
            ASSERT(1               == allocCounter);
            ASSERT(sizeof(myChild) == Z.numBytesInUse());

            z.deallocate(pP);

            ASSERT(0 == Z.numBytesInUse());

            counter      = 0;  // reset counter
            allocCounter = 0;  // reset counter

            if (verbose) printf("\tTesting multiple inheritance with derived "
                                 "class pointer\n");
            myMostDerived *pMD;
            {
                pMD = new(z) myMostDerived(&counter);
                ASSERT(0                     == counter);
                ASSERT(0                     == allocCounter);
                ASSERT(sizeof(myMostDerived) == Z.numBytesInUse());

                bslma::RawDeleterProctor<myMostDerived, my_Pool>
                                                        proctor(pMD, &myAlloc);
                ASSERT(0                     == counter);
                ASSERT(0                     == allocCounter);
                ASSERT(sizeof(myMostDerived) == Z.numBytesInUse());
            }
            ASSERT(1                     == counter);
            ASSERT(1                     == allocCounter);
            ASSERT(Z.numBytesInUse() == sizeof(myMostDerived));

            z.deallocate(pMD);

            ASSERT(0 == Z.numBytesInUse());
        }
      }break;
      case 2: {
        // --------------------------------------------------------------------
        // HELPER CLASS TEST
        //
        // Concerns:
        //   1) The helper class 'my_Class' properly increments its counter at
        //      destruction.
        //   2) The helper class 'my_Pool' properly increments its counter when
        //      its 'deallocate' method is called.
        //
        // Plan:
        //   Create 'my_Class' objects and assign to each object a counter
        //   variable initialized to 0.  Verify that the counter is incremented
        //   after each 'my_Class' object is destroyed.
        //
        //   Next create 'my_Pool' objects and assign to each object a counter
        //   variable initialized to 0.  Invoke the 'deallocate' method and
        //   verify that the counter is incremented.
        //
        // Testing:
        //   Helper Class: 'my_Class'
        //   Helper Class: 'my_Pool'
        // --------------------------------------------------------------------

        if (verbose) printf("\nHELPER CLASS TEST"
                            "\n=================\n");

        if (verbose) printf("\nTesting 'my_Class'.\n");

        if (verbose) printf("\tTesting default ctor and dtor.\n");
        {
            int counter = 0;
            const int NUM_TEST = 5;
            for (int i = 0; i < NUM_TEST; ++i) {
                LOOP_ASSERT(i, counter == i);
                my_Class mx(&counter);
            }
            ASSERT(NUM_TEST == counter);
        }

        if (verbose) printf("\nTesting 'my_Pool'.\n");

        if (verbose) printf("\tTesting default ctor and 'deallocate'.\n");

        {
            int counter = 0;
            const int NUM_TEST = 5;
            for (int i = 0; i < NUM_TEST; ++i) {
                LOOP_ASSERT(i, i == counter);
                my_Pool mx(&counter);
                mx.deallocate(0);
                LOOP_ASSERT(i, i + 1 == counter);
            }
            ASSERT(NUM_TEST == counter);
        }
      }break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //
        // Concerns:
        //   1) The 'bslma::RawDeleterProctor' can be constructed and
        //      destructed gracefully.
        //   1) The allocator's 'deallocate' method is invoked.
        //   2) The (managed) object's destructor is invoked.
        //
        // Plan:
        //   Allocate a 'std::string' with a 'bslma::TestAllocator' and guard
        //   it with 'bslma::RawDeleterProctor' to show that both the
        //   destructor and 'deallocate' is called (by verifying all memory is
        //   returned to the 'bslma::TestAllocator').
        //
        // Testing:
        //   Breathing Test
        // --------------------------------------------------------------------

        if (verbose) printf("\nBREATHING TEST"
                            "\n==============\n");

        using std::string;

        bslma::TestAllocator allocator(veryVeryVeryVerbose);
        const bslma::TestAllocator& Z = allocator;

        if (verbose) printf("\tTesting with 'string' object\n");
        {
            string *s = (string *)new(allocator)string();
            ASSERT(0 < Z.numBytesInUse());
            *s = "Hello World!";
            bslma::RawDeleterProctor<string, bslma::Allocator> guard(
                                                                   s,
                                                                   &allocator);
        }
        ASSERT(0 == Z.numBytesInUse())

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
