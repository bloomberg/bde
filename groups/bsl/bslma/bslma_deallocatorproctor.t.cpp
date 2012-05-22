// bslma_deallocatorproctor.t.cpp                                     -*-C++-*-

#include <bslma_deallocatorproctor.h>
#include <bslma_allocator.h>      // for testing only
#include <bslma_default.h>        // for testing only
#include <bslma_testallocator.h>  // for testing only

#include <cstdlib>   // atoi()
#include <iostream>

using namespace BloombergLP;
using namespace std;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// We are testing a proctor object to ensure that it deallocates the correct
// memory address with the allocator it holds.  We achieve this goal by
// utilizing the 'TestAllocator' allocator, whose 'deallocate' method is
// instrumented to record the most recent memory address used to invoke the
// method.  We initialize the 'bslma::DeallocatorProctor' proctor object
// with this allocator and verify that when the proctor object is destroyed the
// expected memory address is recorded in the allocator.  Since 'TestAllocator'
// is not derived from 'bslma::Allocator' and does not implement an 'allocate'
// method, we ensure that this proctor works with any 'ALLOCATOR' object that
// supports the required 'deallocate' method.  We also need to verify that when
// the 'release' method is invoked on the proctor object the proctor does not
// deallocate its managed memory.  We achieve this goal by instrumenting the
// allocator object with a flag that indicates whether its 'deallocate' method
// has ever been called, and ensure that this flag is in the proper state when
// the proctor's 'release' method is called.
//-----------------------------------------------------------------------------
// [3] bslma::DeallocatorProctor<ALLOCATOR>(memory, allocator);
// [3] ~bslma::DeallocatorProctor<ALLOCATOR>();
// [4] void release();
// [5] void reset(memory);
//-----------------------------------------------------------------------------
// [1] BREATHING TEST
// [2] HELPER CLASS: 'TestAllocator'
// [3] CONCERN: the (non-virtual) 'deallocate' method for pools is also invoked
// [6] USAGE EXAMPLE
//=============================================================================

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
#define T_ cout << "\t" << flush;             // Print tab w/o newline

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

static int verbose;
static int veryVerbose;
static int veryVeryVerbose;

//=============================================================================
//                          HELPER CLASS FOR TESTING
//-----------------------------------------------------------------------------

namespace {

class TestAllocator {

    // DATA
    bool  d_isDeallocateCalled;    // set if 'deallocate' has bee called
    void *d_lastDeallocateAddress; // last memory address deallocated

  public:
    // CREATORS
    TestAllocator();
        // Create this 'TestAllocator'.

    ~TestAllocator();
        // Destroy this 'TestAllocator'.

    // MANIPULATORS
    void deallocate(void *address);
        // Record the specified 'address' and set an internal flag to indicate
        // this method has been called.

    // ACCESSORS
    void *lastDeallocateAddress() const;
        // Return the last memory address that 'deallocate' was invoked on, or
        // return null if 'deallocate' has never been called on this
        // 'TestAllocator'.

    bool isDeallocateCalled() const;
        // Return 'true' if 'deallocate' has been called on this object, and
        // 'false' otherwise.
};

// CREATORS
TestAllocator::TestAllocator()
: d_isDeallocateCalled(0)
, d_lastDeallocateAddress(0)
{
}

TestAllocator::~TestAllocator()
{
}

// MANIPULATORS
void TestAllocator::deallocate(void *address)
{
    d_lastDeallocateAddress = address;
    d_isDeallocateCalled    = true;
}

// ACCESSORS
void *TestAllocator::lastDeallocateAddress() const
{
    return d_lastDeallocateAddress;
}

bool TestAllocator::isDeallocateCalled() const
{
    return d_isDeallocateCalled;
}

}  // close unnamed namespace

//=============================================================================
//                                USAGE EXAMPLE
//-----------------------------------------------------------------------------
// The 'bslma::DeallocatorProctor' is normally used to achieve *exception*
// *safety* in an *exception *neutral* way by managing memory in a sequence of
// continuous memory allocations.  Since each memory allocation may
// potentially throw an exception, an instance of this proctor can be used to
// (temporarily) manage newly allocated memory while attempting to allocate
// additional memory.  Should an exception occur in subsequent memory
// allocation, the proctor's destructor deallocates its managed memory,
// preventing a memory leak.
//
// This example illustrate a typical use of 'bslma::DeallocatorProctor'.
// Suppose we have an array class that stores an "in-place" representation of
// objects of parameterized 'TYPE':
//..
// my_array.h
// ...

template <class TYPE>
class my_Array {
    // This class implements an "in-place" array of objects of
    // parameterized 'TYPE' stored contiguously in memory.

    // DATA
    TYPE             *d_array_p;      // dynamically allocated array
    int               d_length;       // logical length of this array
    int               d_size;         // physical capacity of this array
    bslma::Allocator *d_allocator_p;  // allocator (held, not owned)

  public:
    // CREATORS
    my_Array(bslma::Allocator *basicAllocator = 0);
        // Create a 'my_Array' object.  Optionally specify a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is
        // 0, the currently installed default allocator is used.

    // ...

    ~my_Array();
        // Destroy this 'my_Array' object and all elements currently
        // stored.

    // MANIPULATORS
    // ...

    void append(const TYPE& object);
        // Append (a copy of) the specified 'object' of parameterized
        // 'TYPE' to (the end of) this array.

    // ...
};
//..
// Note that the rest of the 'my_Array' interface (above) and implementation
// (below) is omitted as the portion shown is sufficient to demonstrate the use
// of 'bslma::DeallocatorProctor'.
//..
// CREATORS
template <class TYPE>
inline
my_Array<TYPE>::my_Array(bslma::Allocator *basicAllocator)
: d_length(0)
, d_size(1)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    d_array_p = (TYPE *)d_allocator_p->allocate(sizeof(TYPE));
}

template <class TYPE>
my_Array<TYPE>::~my_Array()
{
    for (int i = 0; i < d_length; ++i) {
        d_array_p[i].~TYPE();
    }
    d_allocator_p->deallocate(d_array_p);
}
//..
// In order to implement the 'append' function, we first have to introduce an
// 'my_AutoDestructor' 'class', which automatically destroy a sequence of
// managed objects upon destruction.  See 'bslma::AutoDestructor' for a similar
// component with full documentation:
//..
// my_autodestructor.h
// ...

template <class TYPE>
class my_AutoDestructor {
    // This class implements a range proctor that, unless its 'release'
    // method has previously been invoked, automatically invokes the
    // destructor of each of sequence of objects it manages.

    // DATA
    TYPE * d_origin_p;
    int    d_length;

  public:
    // CREATORS
    my_AutoDestructor(TYPE *origin, int length)
        // Create an 'my_AutoDestructor' to manage a contiguous sequence of
        // objects.
    : d_origin_p(origin)
    , d_length(length)
    {
    }

    ~my_AutoDestructor()
        // Destroy this 'my_AutoDestructor' and, unless its 'release'
        // method has previously been invoked, destroy the sequence of
        // objects it manages by invoking the destructor of each of the
        // (managed) objects.
    {
        if (d_length) {
            for (; d_length > 0; --d_length, ++d_origin_p) {
                d_origin_p->~TYPE();
            }
        }
    }

    // MANIPULATORS
    my_AutoDestructor<TYPE>& operator++()
        // Increase by one the length of the sequence of objects managed by
        // this range proctor.
    {
        ++d_length;
        return *this;
    }

    void release()
        // Release from management the sequence of objects currently
        // managed by this range proctor.
    {
        d_length = 0;
    }
};
//..
// We can now continue with our implementation of the 'my_Array' class:
//..
// my_array.h
// ...

// MANIPULATORS
template <class TYPE>
void my_Array<TYPE>::append(const TYPE &object)
{
    if (d_length == d_size) {
        TYPE *newArray = (TYPE *)d_allocator_p->allocate(
                                 d_size * 2 * sizeof(TYPE));  // possibly throw

        //*****************************************************************
        // Note the use of the deallocator proctor on 'newArray' (below). *
        //*****************************************************************

        bslma::DeallocatorProctor<bslma::Allocator> proctor(newArray,
                                                            d_allocator_p);

        // Note use of 'my_AutoDestructor' here to protect the copy
        // construction of 'TYPE' objects.
        my_AutoDestructor<TYPE> destructor(newArray, 0);

        for (int i = 0; i < d_length; ++i) {
            new(&newArray[i]) TYPE(d_array_p[i], d_allocator_p);
                                                         // possibly throw
            d_array_p[i].~TYPE();
            ++destructor;
        }

        destructor.release();

        //*********************************************************
        // Note that the deallocator proctor is released (below). *
        //*********************************************************

        proctor.release();

        d_allocator_p->deallocate(d_array_p);
        d_array_p = newArray;
        d_size   *= 2;
    }

    new(&d_array_p[d_length]) TYPE(object, d_allocator_p);
    ++d_length;
}
//..
// Both the use of 'bslma::DeallocatorProctor' and 'my_AutoDestructor' are
// necessary to implement exception safety.
//
// The 'append' method defined above potentially throws in two places.  If the
// memory allocator held in 'd_allocator_p' where to throw while attempting to
// allocate the new array of parameterized 'TYPE', no memory would be leaked.
// But without subsequent use of the 'bslma::DeallocatorProctor', if the
// allocator subsequently throws while copy constructing the objects from the
// old array to the new array, the newly allocated memory block would be
// leaked.  Using the 'bslma::DeallocatorProctor' prevents the leak by
// deallocating the proctored memory automatically should the proctor go out
// of scope before the 'release' method of the proctor is called (such as when
// the function exits prematurely due to an exception).
//
// Similarly, any resources acquired as a result of copy constructing the
// objects from the old array to the new array would be leaked if the
// constructor of 'TYPE' throws.  Using the 'my_AutoDestructor' prevents the
// leak by invoking the destructor of the proctored (and newly created) objects
// in the new array should the 'my_AutoDestructor' goes out of scope before the
// 'release' method of the proctor is called.
//
// Note that the 'append' method assumes the copy constructor of 'TYPE' takes
// an allocator as a second argument.  In reality, a constructor proxy that
// checks the traits of 'TYPE' (to see whether 'TYPE' uses 'bslma::Allocator')
// should be used (see 'bslalg::ConstructorProxy').

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//           Additional Functionality Needed to Complete Usage Test Case

class my_AllocatingClass {
    // This object allocates memory at construction to assist in forcing
    // exceptions due to allocations for the usage example test.

    // DATA
    void             *d_memory_p;     // memory allocated
    bslma::Allocator *d_allocator_p;  // allocator (held, not owned)

  public:
    // CREATORS
    my_AllocatingClass(bslma::Allocator *basicAllocator = 0)
        // Create a 'my_AllocatingClass', optionally specified
        // 'basicAllocator'.  If 'basicAllocator' is zero, the global default
        // allocator will be used to supply memory.
    : d_allocator_p(bslma::Default::allocator(basicAllocator))
    {
        d_memory_p = d_allocator_p->allocate(1);
    }

    my_AllocatingClass(const my_AllocatingClass&  object,
                       bslma::Allocator          *basicAllocator = 0)
        // Create a 'my_AllocatingClass' that as the same counter and allocator
        // as the specified 'object'.
    : d_allocator_p(bslma::Default::allocator(basicAllocator))
    {
        d_memory_p = d_allocator_p->allocate(1);
    }

    ~my_AllocatingClass()
        // Destroy this 'my_AllocatingClass'.
    {
        d_allocator_p->deallocate(d_memory_p);
        d_memory_p = 0;
    }
};

//=============================================================================
//                                MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    verbose = argc > 2;
    veryVerbose = argc > 3;
    veryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:
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
        //   of 'my_Array' using a 'bslma::TestAllocator' to verify that
        //   memory is allocated and deallocated properly.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << "USAGE EXAMPLE TEST" << endl
                          << "==================" << endl;

        bslma::TestAllocator allocator(veryVeryVerbose);
        const bslma::TestAllocator &Z = allocator;

        ASSERT(0 == Z.numBytesInUse());
        {
            my_Array<my_AllocatingClass> array(&allocator);

            array.append(my_AllocatingClass(&allocator));
            array.append(my_AllocatingClass(&allocator));

            int numBytes = Z.numBytesInUse();  const int& NUMBYTES = numBytes;

            // Force the reallocation in the array to fail.
            allocator.setAllocationLimit(1);
            ASSERT(NUMBYTES == Z.numBytesInUse());

            try {
                array.append(my_AllocatingClass(&allocator));

                ASSERT(0);  // should not get here
            }
            catch (...) {
                // Make sure no memory is leaked.
                ASSERT(NUMBYTES == Z.numBytesInUse());
            }
            ASSERT(NUMBYTES == Z.numBytesInUse());
        }
        ASSERT(0 == Z.numBytesInUse());

      } break;
      case 5: {
        // --------------------------------------------------------------------
        // 'reset' TEST
        //
        // Concerns:
        //   Verify that when the 'reset' method is called, the proctor object
        //   properly manages a different block of memory.
        //
        // Plans:
        //   Allocate a memory block using a 'bslma::TestAllocator'.  Next,
        //   allocate another memory block with different size using the same
        //   test allocator.  Finally, initialize a 'bslma::DeallocatorProctor'
        //   with the first memory block and the test allocator.  Call 'reset'
        //   on the proctor with the second memory block before the proctor
        //   goes out of scope.  Once the proctor goes out of scope, verify
        //   that only the second memory block is deallocated (by checking the
        //   expected number of bytes in use).
        //
        // Testing:
        //   void reset();
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "'reset' TEST" << endl
                                  << "============" << endl;

        if (verbose) cout << "\nTesting the 'reset' method." << endl;

        bslma::TestAllocator allocator(veryVeryVerbose);
        const bslma::TestAllocator& Z = allocator;
        ASSERT(0 == Z.numBytesInUse());

        void *memory1 = allocator.allocate(1);
        void *memory2 = allocator.allocate(2);
        ASSERT(3 == Z.numBytesInUse());

        {
            bslma::DeallocatorProctor<bslma::Allocator> proctor(memory1,
                                                                &allocator);
            ASSERT(3 == Z.numBytesInUse());

            proctor.reset(memory2);
            ASSERT(3 == Z.numBytesInUse());
        }
        ASSERT(1 == Z.numBytesInUse());

        allocator.deallocate(memory1);
        ASSERT(0 == Z.numBytesInUse());

      } break;
      case 4: {
        // --------------------------------------------------------------------
        // 'release' TEST
        //
        // Concerns:
        //   Verify that when the 'release' method is called, the proctor
        //   object properly releases from management the block of memory
        //   currently managed by this proctor.
        //
        // Plan:
        //   Initialize a 'bslma::DeallocatorProctor' with static memory address
        //   and a 'TestAllocator'.  Call 'release' on the proctor before it
        //   goes out of scope.  Verify that 'deallocate' method of the
        //   'TestAllocator' is not called.
        //
        // Testing:
        //   void release();
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "'release' TEST" << endl
                                  << "==============" << endl;

        if (verbose) cout << "\nTesting the 'release' method." << endl;

        TestAllocator t;    const TestAllocator& T = t;

        int x;

        ASSERT(false == T.isDeallocateCalled());
        ASSERT(0     == T.lastDeallocateAddress());

        {
            bslma::DeallocatorProctor<TestAllocator> proctor(&x, &t);
            ASSERT(false == T.isDeallocateCalled());
            ASSERT(0     == T.lastDeallocateAddress());

            proctor.release();
            ASSERT(false == T.isDeallocateCalled());
            ASSERT(0     == T.lastDeallocateAddress());
        }

        ASSERT(false == T.isDeallocateCalled());
        ASSERT(0     == T.lastDeallocateAddress());

      } break;
      case 3: {
        // --------------------------------------------------------------------
        // CTOR / DTOR TEST
        //
        // Concerns:
        //   1) Verify that the proctor object properly invokes the
        //      'deallocate' method of its held allocator (or pool) when the
        //      proctor goes out of scope.
        //
        //   2) Verify that when an allocator (or pool) not inherited from
        //      'bslma::Allocator' is supplied to the
        //      'bslma::DeallocatorProctor', the deallocate method of the
        //      allocator (or pool) supplied is still invoked.
        //
        //   3) Verify that the 'deallocate' method is not invoked when the
        //      'bslma::DeallocatorProctor' is initialized with 'null' memory
        //      address.
        //
        // Plan:
        //   For concern 1, create 'bslma::DeallocatorProctor' proctor objects
        //   holding 'bslma::TestAllocator' objects and varying memory
        //   addresses allocated by the same test allocator.  Verify that when
        //   the proctor object goes out of scope the allocator object contains
        //   the expected memory address.
        //
        //   For concern 2, create a 'TestAllocator', which does not inherit
        //   from 'bslma::Allocator'.  Since the 'TestAllocator' does not
        //   actually allocate memory, use pass static memory address to the
        //   'bslma::DeallocatorProctor'.
        //
        //   For concern 3, initialize the 'bslma::DeallocatorProctor' with a
        //   'null' memory address and the 'TestAllocator'.  Verify that the
        //   'deallocate' method is not invoked.
        //
        // Testing:
        //   bslma::DeallocatorProctor(memory, allocator);
        //   ~bslma::DeallocatorProctor();
        //   CONCERN: the (non-virtual) 'deallocate' method for pools is also
        //            invoked
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "CTOR / DTOR TEST" << endl
                                  << "================" << endl;

        if (verbose) cout << "\nTesting with bslma_TestAllocator." << endl;

        {

        bslma::TestAllocator allocator;
        const bslma::TestAllocator& Z = allocator;
        ASSERT(0 == Z.numBytesInUse());

        if (verbose) cout << "\tTesting constructor." << endl;

        enum { NUM_TEST = 5 };
        for (int i = 0; i < NUM_TEST; ++i) {
            LOOP_ASSERT(i, 0 == Z.numBytesInUse());

            void *memory = allocator.allocate(i);
            LOOP_ASSERT(i, i == Z.numBytesInUse());

            {
                bslma::DeallocatorProctor<bslma::Allocator> proctor(
                                                                   memory,
                                                                   &allocator);

                LOOP_ASSERT(i, i == Z.numBytesInUse());
            }
            LOOP_ASSERT(i, 0 == Z.numBytesInUse());
        }

        if (verbose) cout << "\tTesting constructor with null memory address."
                          << endl;

        {
            ASSERT(0 == Z.numBytesInUse());
            bslma::DeallocatorProctor<bslma::Allocator> proctor(0, &allocator);
            ASSERT(0 == Z.numBytesInUse());
        }
        ASSERT(0 == Z.numBytesInUse());

        }

        if (verbose) cout << "\nTesting with TestAllocator." << endl;

        {

        const void *DATA[] = {(void *) 1, (void *) 2, (void *) 3 };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        if (verbose) cout << "\tTesting constructor" << endl;

        for (int i = 0; i < NUM_DATA; ++i) {
            TestAllocator t;
            const TestAllocator& T = t;
            void *memory       = (void *) DATA[i];
            const void *MEMORY = (void *) memory;

            LOOP_ASSERT(i, false == T.isDeallocateCalled());
            LOOP_ASSERT(i, 0     == T.lastDeallocateAddress());
            {
                bslma::DeallocatorProctor<TestAllocator> proctor(memory, &t);
                LOOP_ASSERT(i, false == T.isDeallocateCalled());
                LOOP_ASSERT(i, 0     == T.lastDeallocateAddress());
            }
            LOOP_ASSERT(i, MEMORY == T.lastDeallocateAddress());
            LOOP_ASSERT(i, true   == T.isDeallocateCalled());
        }

        if (verbose) cout << "\tTesting constructor with null memory address."
                          << endl;

        TestAllocator t;
        const TestAllocator& T = t;
        {
            ASSERT(false == T.isDeallocateCalled());
            ASSERT(0     == T.lastDeallocateAddress());
            bslma::DeallocatorProctor<TestAllocator> proctor(0, &t);
            ASSERT(false == T.isDeallocateCalled());
            ASSERT(0     == T.lastDeallocateAddress());
        }
        ASSERT(false == T.isDeallocateCalled());
        ASSERT(0     == T.lastDeallocateAddress());

        }

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // HELPER CLASS TEST
        //
        // Concerns:
        //   1) 'lastDeallocatedAddress' method properly returns the last
        //      address deallocated.
        //
        //   2) 'isDeallocateCalled()' returns whether the 'deallocate' method
        //      of the 'TestAllocator' had been called.
        //
        // Plan:
        //   Create a 'TestAllocator' object and call its 'deallocate' method
        //   with varying memory address.  Verify that 'lastDeallocateAddress'
        //   returns the expected memory addresses.  Also verify that
        //   'isDeallocateCalled' indicates the proper state of the
        //   'TestAllocator' object before and after 'deallocate' is called.
        //
        // Testing:
        //   HELPER CLASS: 'TestAllocator'
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "HELPER CLASS TEST" << endl
                                  << "=================" << endl;

        if (verbose) cout << "\nTesting 'TestAllocator'." << endl;

        const void *DATA[] = {(void *) 0, (void *) 1, (void *) 2, (void *) 3 };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        TestAllocator mX;    const TestAllocator &X = mX;
        ASSERT(0 == X.lastDeallocateAddress());
        ASSERT(!X.isDeallocateCalled());
        for (int di = 0; di < NUM_DATA; ++di) {
            void *addr = (void *) DATA[di];    const void *ADDR = addr;
            mX.deallocate(addr);
            if (veryVerbose) { T_ P_(di) P(ADDR) }
            LOOP_ASSERT(di, ADDR == X.lastDeallocateAddress());
            LOOP_ASSERT(di, X.isDeallocateCalled());
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //
        // Concerns:
        //   1) The 'bslma::DeallocatorProctor' can be constructed and
        //      destructed gracefully.
        //   2) The allocator's 'deallocate' method is invoked.
        //
        // Plan:
        //   Allocate a block of memory with a 'bslma::TestAllocator' and
        //   guard it with 'bslma::DeallocatorProctor' to show that the
        //   'deallocate' method is called (by verifying all memory is returned
        //   to the 'bslma::TestAllocator').
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "BREATHING TEST" << endl
                                  << "==============" << endl;

        bslma::TestAllocator allocator(veryVeryVerbose);
        const bslma::TestAllocator& Z = allocator;
        ASSERT(0 == Z.numBytesInUse());

        void *memory = allocator.allocate(1);

        ASSERT(1 == Z.numBytesInUse());
        {
            bslma::DeallocatorProctor<bslma::Allocator> proctor(memory,
                                                                &allocator);
            ASSERT(1 == Z.numBytesInUse());
        }
        ASSERT(0 == Z.numBytesInUse());


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
