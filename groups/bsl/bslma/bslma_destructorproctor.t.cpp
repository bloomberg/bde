// bslma_destructorproctor.t.cpp                                      -*-C++-*-

#include <bslma_destructorproctor.h>

#include <bslma_deallocatorproctor.h>  // for testing only
#include <bslma_default.h>             // for testing only
#include <bslma_testallocator.h>       // for testing only

#include <bsls_bsltestutil.h>

#include <stdio.h>      // 'printf'
#include <stdlib.h>     // 'atoi'

#include <new>
#include <string>  // For breathing test only - TBD rewrite without

using namespace BloombergLP;

//=============================================================================
//                                 TEST PLAN
//-----------------------------------------------------------------------------
//                                  Overview
//                                  --------
// We are testing a proctor object to ensure that it destroys the managed
// objects if release is not called before the proctor object goes out of
// scope.  We achieve this goal by creating objects of a user-defined type that
// are each initialized with the address of a unique counter, using a
// 'bslma::TestAllocator'.  As each object is destroyed, its destructor
// increments the counter held by the object, indicating the number of times
// the object's destructor is called.  After the proctor is destroyed, we
// verify that the corresponding counters of the object managed by the proctor
// are modified.
//-----------------------------------------------------------------------------
// [3] bslma::DestructorProctor<ALLOCATOR>(object);
// [3] ~bslma::DestructorProctor<ALLOCATOR>();
// [4] void release();
// [5] void reset(object);
//-----------------------------------------------------------------------------
// [1] Breathing Test
// [2] Helper Class: 'my_Class'
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

class my_Class {
    // This object indicates that its destructor is called by incrementing the
    // global counter (supplied at construction) that it holds.

    // DATA
    int *d_counter_p;  // (global) counter to be incremented at destruction

  public:
    // CREATORS
    my_Class(int *counter) : d_counter_p(counter) {}
        // Create this object using the address of the specified 'counter' to
        // be held.

    ~my_Class() { ++*d_counter_p; }
        // Destroy this object and increment this object's (global) counter.
};


//=============================================================================
//                                USAGE EXAMPLE
//-----------------------------------------------------------------------------
// The 'bslma::DestructorProctor' is normally used to manage objects that are
// constructed sequentially in a block of provided memory.  This is often the
// case when memory management and primitive helpers are implemented in
// different components.  An example would be the construction of a pair object
// within another container with the help of a scalar primitive helper (see
// bslalg_scalarprimitives).  After the first object is constructed in the
// provided memory, it should be protected in case the constructor of the
// second object throws.  The following example illustrates a typical use of
// the 'bslma::DestructorProctor'.
//
// First, suppose we have a pair class:
//..
// my_pair.h
// ...

template <class TYPE1, class TYPE2>
class my_Pair {
    // This class provides a pair container to pair two different objects,
    // one of parameterized 'TYPE1', and the other of parameterized
    // 'TYPE2'.

  public:
    // PUBLIC TYPES
    typedef TYPE1 firstType;
    typedef TYPE2 secondType;

    // PUBLIC DATA
    TYPE1            first;          // first object
    TYPE2            second;         // second object

    // DATA
    bslma::Allocator *d_allocator_p;  // allocator (held, not owned)

    // Declare trait 'my_PairTrait'.
    // ...

  public:
    // CREATORS
    my_Pair(const TYPE1&      iFirst,
            const TYPE2&      iSecond,
            bslma::Allocator *basic_Allocator = 0)
        // Create a 'my_Pair' object that holds a copy of the specified
        // 'iFirst' and 'iSecond'.  Optionally specify 'basicAllocator' to
        // supply memory.  If 'basicAllocator' is zero,  the global default
        // allocator will be used to supply memory.
    : first(iFirst)
    , second(iSecond)
    , d_allocator_p(bslma::Default::allocator(basic_Allocator))
    {
    }

    // ...

};
//..
// Note that parts of the implementation, including the 'my_PairTrait'
// declaration, are elided.  The 'my_PairTrait' will be used by the primitive
// helper to customize implementations for objects that are pairs.
//
// We now implement the primitive helper:
//..
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//           Additional Functionality Needed to Complete Usage Test Case
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// !!!Warning: Modified usage example in order to test it
template <class TYPE>
struct my_HasPairTrait : bsl::false_type { };
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

// my_primitives.h
// ...

struct my_Primitives {
    // This struct provides a namespace for pure procedure primitive
    // functions used to construct, destroy, insert, append and remove
    // objects.

  private:
    // PRIVATE TYPES
    enum { PAIR_TRAIT = 1, NIL_TRAIT = 0 };

  public:
    // CLASS METHODS
    template <class TYPE>
    static void copyConstruct(TYPE             *address,
                              const TYPE&       original,
                              bslma::Allocator *basicAllocator);
        // Copy construct the specified 'original' into the specified
        // 'address' using the specified 'basicAllocator' (if the
        // copy constructor of 'TYPE' takes an allocator).

    template <class TYPE>
    static void copyConstruct(TYPE                       *address,
                              const TYPE&                 original,
                              bslma::Allocator           *basicAllocator,
                              bsl::integral_constant<bool, PAIR_TRAIT> *);
        // Copy construct the specified 'original' into the specified
        // 'address' using the specified 'basicAllocator' (if the
        // copy constructor of 'TYPE' takes an allocator).  Note that
        // the last parameter is only used for overload resolution.

    template <class TYPE>
    static void copyConstruct(TYPE                      *address,
                              const TYPE&                original,
                              bslma::Allocator          *basicAllocator,
                              bsl::integral_constant<bool, NIL_TRAIT> *);
        // Copy construct the specified 'original' into the specified
        // 'address' using the specified 'basicAllocator' (if the
        // copy constructor of 'TYPE' takes an allocator).  Note that
        // the last parameter is only used for overload resolution.
};

template <class TYPE>
inline
void my_Primitives::copyConstruct(TYPE             *address,
                                  const TYPE&       original,
                                  bslma::Allocator *basicAllocator)
{
    copyConstruct(address,
                  original,
                  basicAllocator,
                  (typename my_HasPairTrait<TYPE>::type *)0);
}

template <class TYPE>
inline
void my_Primitives::copyConstruct(TYPE                       *address,
                                  const TYPE&                 original,
                                  bslma::Allocator           *basicAllocator,
                                  bsl::integral_constant<bool, PAIR_TRAIT> *)
{
    copyConstruct(&address->first, original.first, basicAllocator);

    //**************************************************
    // Note the use of the destructor proctor (below). *
    //**************************************************

    bslma::DestructorProctor<typename TYPE::firstType> proctor(
                                                              &address->first);

    copyConstruct(&address->second, original.second, basicAllocator);

    //********************************************************
    // Note that the destructor proctor is released (below). *
    //********************************************************

    proctor.release();
}

template <class TYPE>
inline
void my_Primitives::copyConstruct(TYPE                      *address,
                                  const TYPE&                original,
                                  bslma::Allocator          *basicAllocator,
                                  bsl::integral_constant<bool, NIL_TRAIT> *)
{
    new(address)TYPE(original, basicAllocator);
}
//..
// In the above implementation, if the copy construction of the second object
// in the pair throws, all memory (and any other resources) acquired as a
// result of copying the (not-yet-managed) object would be leaked.  Using the
// 'bslma::DestructorProctor' prevents the leaks by invoking the destructor of
// the proctored object automatically should the proctor go out of scope
// before the 'release' method of the proctor is called (such as when the
// function exits prematurely due to an exception).
//
// Note that the 'copyConstruct' method assumes the copy constructor of
// 'TYPE::firstType' and 'TYPE::secondType' takes an allocator as a second
// argument.  In production code, a constructor proxy that checks the traits
// of 'TYPE::firstType' and 'TYPE::secondType' (to determine whether they uses
// 'bslma::Allocator') should be used (see bslalg_constructorproxy).

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//           Additional Functionality Needed to Complete Usage Test Case

class my_AllocatingClass {
    // This object indicates that its destructor is called by incrementing the
    // counter it *holds* (provided at construction) in the destructor.

    // DATA
    int              *d_counter_p;   // counter to increment at destruction
    void             *d_memory_p;    // dummy piece of memory
    bslma::Allocator *d_allocator_p; // allocator (held, not owned)

  public:
    // CREATORS
    my_AllocatingClass(int              *counter,
                       bslma::Allocator *basicAllocator = 0)
        // Create a 'my_AllocatingClass' using the (global) 'counter'.
        // Optionally specified 'basicAllocator'.  If 'basicAllocator' is zero,
        // the global default allocator will be used to supply memory.  Create
        // this object and optionally specify 'allocator' used for 'counter' to
        // be held.
    : d_counter_p(counter)
    , d_allocator_p(bslma::Default::allocator(basicAllocator))
    {
        d_memory_p = d_allocator_p->allocate(1);
    }

    my_AllocatingClass(const my_AllocatingClass&  object,
                       bslma::Allocator          *basicAllocator = 0)
        // Create a 'my_AllocatingClass' that as the same counter and allocator
        // as the specified 'object'.
    : d_counter_p(object.d_counter_p)
    , d_allocator_p(bslma::Default::allocator(basicAllocator))
    {
        d_memory_p = d_allocator_p->allocate(1);
    }

    ~my_AllocatingClass()
        // Destroy this object.  Also deallocate memory allocated at
        // construction and increment this object's counter if it is
        // not 'null'.
    {
        d_allocator_p->deallocate(d_memory_p);
        ++*d_counter_p;
    }
};

template <>
struct my_HasPairTrait<my_Pair<my_AllocatingClass, my_AllocatingClass> >
    : bsl::true_type { };

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
        //   of 'my_Pair' and 'my_Primitives' using a 'bslma::TestAllocator' to
        //   verify that memory is allocated and deallocated properly.
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
//      const bslma::TestAllocator &Z = z;

        int counter1 = 0;
        int counter2 = 0;

        ASSERT(counter1 == 0);
        ASSERT(counter2 == 0);
        ASSERT(z.numBytesInUse() == 0);

        typedef my_Pair<my_AllocatingClass, my_AllocatingClass> PairType;

        if (verbose) printf("\nTesting without exception\n");
        {
            PairType *memory = (PairType *)z.allocate(sizeof(PairType));
            const bsls::Types::Int64& NUM_BYTES = z.numBytesInUse();
            {
                PairType mp(my_AllocatingClass(&counter1),
                            my_AllocatingClass(&counter2),
                            &z);
                ASSERT(1         == counter1);
                ASSERT(1         == counter2);

                const PairType& P = mp;

                my_Primitives::copyConstruct(memory, P, &z);
                ASSERT(1         == counter1);
                ASSERT(1         == counter2);
            }
            if (veryVerbose) { T_ P_(counter1); P(counter2); }
            ASSERT(2             == counter1);
            ASSERT(2             == counter2);
            // + 2 for the constructed pair.
            ASSERT(NUM_BYTES + 2 == z.numBytesInUse());

            z.deleteObjectRaw(memory);
            ASSERT(3             == counter1);
            ASSERT(3             == counter2);
            ASSERT(0             == z.numBytesInUse());
        }
        ASSERT(3  == counter1);
        ASSERT(3  == counter2);
        ASSERT(0  == z.numBytesInUse());

        counter1 = 0;  // reset counter to 0
        counter2 = 0;  // reset counter to 0

        if (verbose) printf("\nTesting with exception\n");
        {
            PairType *memory = (PairType *)z.allocate(sizeof(PairType));
            const bsls::Types::Int64& NUM_BYTES = z.numBytesInUse();
            {
                PairType mp(my_AllocatingClass(&counter1),
                            my_AllocatingClass(&counter2),
                            &z);

                const PairType& P = mp;

                // Makes the second allocation fail.
                z.setAllocationLimit(1);

                try {
                    my_Primitives::copyConstruct(memory, P, &z);

                    ASSERT(0);  // should never get here
                }
                catch(...) {
                    // 'first' is copied, then destroyed.
                    ASSERT(2         == counter1);
                    ASSERT(1         == counter2);
                    ASSERT(NUM_BYTES == z.numBytesInUse());
                }
            }
            ASSERT(3         == counter1);
            ASSERT(2         == counter2);
            ASSERT(NUM_BYTES == z.numBytesInUse());

            z.deallocate(memory);
            ASSERT(3         == counter1);
            ASSERT(2         == counter2);
            ASSERT(0         == z.numBytesInUse());
        }
        ASSERT(3 == counter1);
        ASSERT(2 == counter2);
        ASSERT(0 == z.numBytesInUse());
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
        //   initialize a 'bslma::DestructorProctor' object with the first
        //   object.  Call 'reset' on the proctor with the second object before
        //   it goes out of scope.  Once the proctor goes out of scope, verify
        //   that only the second counter is incremented, and no memory is
        //   deallocated.
        //
        // Testing:
        //   void reset();
        // --------------------------------------------------------------------

        if (verbose) printf("\n'reset' TEST"
                            "\n============\n");

        bslma::TestAllocator z(veryVeryVeryVerbose);
        const bslma::TestAllocator& Z = z;

        int counter1 = 0;  const int& COUNTER1 = counter1;
        int counter2 = 0;  const int& COUNTER2 = counter2;
        if (veryVerbose) {
            printf("Testing the 'reset' method\n");
        }
        my_Class *pC1;
        my_Class *pC2;
        {
            pC1 = new(z) my_Class(&counter1);
            pC2 = new(z) my_Class(&counter2);
            ASSERT(0                    == COUNTER1);
            ASSERT(0                    == COUNTER2);
            ASSERT(2 * sizeof(my_Class) == Z.numBytesInUse());

            bslma::DestructorProctor<my_Class> proctor(pC1);
            ASSERT(0                    == COUNTER1);
            ASSERT(0                    == COUNTER2);
            ASSERT(2 * sizeof(my_Class) == Z.numBytesInUse());

            proctor.reset(pC2);
            ASSERT(0                    == COUNTER1);
            ASSERT(0                    == COUNTER2);
            ASSERT(2 * sizeof(my_Class) == Z.numBytesInUse());
        }
        ASSERT(0                    == COUNTER1);
        ASSERT(1                    == COUNTER2);
        ASSERT(2 * sizeof(my_Class) == Z.numBytesInUse());

        z.deleteObject(pC1);
        z.deallocate(pC2);

        ASSERT(1 == COUNTER1);
        ASSERT(1 == COUNTER2);
        ASSERT(0 == Z.numBytesInUse());

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
        //   'bslma::DestructorProctor' object with the corresponding
        //   'my_Class' object.  Call 'release' on the proctor before it goes
        //   out of scope.  Verify that the counter is not incremented, and
        //   the memory allocated by the test allocator is not deallocated.
        //
        // Testing:
        //   void release();
        // --------------------------------------------------------------------

        if (verbose) printf("\n'release' TEST"
                            "\n==============\n");

        bslma::TestAllocator z(veryVeryVeryVerbose);
        const bslma::TestAllocator& Z = z;

        int counter = 0;  const int& COUNTER = counter;
        if (veryVerbose) {
            printf("Testing the 'release' method\n");
        }
        my_Class *pC;
        {
            pC = new(z) my_Class(&counter);
            ASSERT(0                == COUNTER);
            ASSERT(sizeof(my_Class) == Z.numBytesInUse());

            bslma::DestructorProctor<my_Class> proctor(pC);
            ASSERT(0                == COUNTER);
            ASSERT(sizeof(my_Class) == Z.numBytesInUse());

            proctor.release();
            ASSERT(0                == COUNTER);
            ASSERT(sizeof(my_Class) == Z.numBytesInUse());
        }
        ASSERT(0                == COUNTER);
        ASSERT(sizeof(my_Class) == Z.numBytesInUse());

        z.deleteObjectRaw(pC);
        ASSERT(1 == COUNTER);
        ASSERT(0 == Z.numBytesInUse());

      }break;
      case 3: {
        // --------------------------------------------------------------------
        // CTOR / DTOR TEST
        //
        // Concerns:
        //   1) Verify that when the 'bslma::DestructorProctor' properly
        //      invokes the destructor of the proctored object when it goes
        //      out of scope.
        //   2) Verify that the 'bslma::DestructorProctor' can be constructed
        //      with a null pointer.
        //
        // Plan:
        //   For concern 1, create 'my_Class' objects using
        //   'bslma::TestAllocator' and initialize it with a counter.  Next
        //   initialize a 'bslma::DestructorProctor' object with the
        //   corresponding 'my_Class' object.  Verify that when the proctor
        //   goes out of scope, the counter is incremented, but memory is not
        //   deallocated.
        //
        //   For concern 2, initialize a 'bslma::DestructorProctor' with a null
        //   pointer and let it go out of scope.
        //
        // Testing:
        //   bslma::DestructorProctor<TYPE>(object);
        //   ~bslma::DestructorProctor<TYPE>();
        // --------------------------------------------------------------------

        if (verbose) printf("\nCTOR / DOTR TEST"
                            "\n================\n");

        if (verbose) printf("\nTesting CTOR and DTOR\n");

        bslma::TestAllocator z(veryVeryVeryVerbose);
        const bslma::TestAllocator& Z = z;

        int counter = 0;  const int& COUNTER = counter;
        my_Class *pMC;
        ASSERT(0 == Z.numBytesInUse());
        {
            pMC = new(z) my_Class(&counter);
            ASSERT(0           == COUNTER);
            ASSERT(sizeof *pMC == Z.numBytesInUse());

            bslma::DestructorProctor<my_Class> proctor(pMC);
            ASSERT(0           == COUNTER);
            ASSERT(sizeof *pMC == Z.numBytesInUse());
        }
        ASSERT(1           == COUNTER);
        ASSERT(sizeof *pMC == Z.numBytesInUse());

        z.deallocate(pMC);
        ASSERT(0 == Z.numBytesInUse());

        if (verbose) printf("\nTesting CTOR with null pointer\n");

        {
            bslma::DestructorProctor<my_Class> proctor((my_Class *)0);
        }

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // HELPER CLASS TEST
        //
        // Concerns:
        //   1) The helper class 'my_Class' properly increments its counter at
        //      destruction.
        //
        // Plan:
        //   Create 'my_Class' objects and assign to each object a counter
        //   variable initialized to 0.  Verify that the counter is incremented
        //   after each 'my_Class' object is destroyed.
        //
        // Testing:
        //   Helper Class: 'my_Class'
        // --------------------------------------------------------------------

        if (verbose) printf("\nHELPER CLASS TEST"
                            "\n=================\n");

        if (verbose) printf("\nTesting 'my_Class'\n");

        if (verbose) printf("\tTesting default ctor and dtor\n");
        {
            enum { NUM_TEST = 5 };
            int counter = 0;    const int& COUNTER = counter;
            for (int i = 0; i < NUM_TEST; ++i) {
                if (veryVerbose) {
                    T_ P_(i) P(COUNTER)
                }
                LOOP_ASSERT(i, COUNTER == i);
                my_Class mx(&counter);
            }
            ASSERT(NUM_TEST == COUNTER);
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //
        // Concerns:
        //   1) The 'bslma::DestructorProctor' can be constructed and
        //      destructed gracefully.
        //
        // Plan:
        //   Allocate a block of memory with a 'bslma::TestAllocator' and
        //   construct a string object on it.  Guard the array with
        //   'bslma::DestructorProctor'.
        //
        // Testing:
        //   Breathing Test
        // --------------------------------------------------------------------

        if (verbose) printf("\nBREATHING TEST"
                            "\n==============\n");

        using std::string;

        bslma::TestAllocator allocator(veryVeryVeryVerbose);
        const bslma::TestAllocator& Z = allocator;

        enum { NUM_TEST = 10 };

        void *memory = allocator.allocate(sizeof(string) * NUM_TEST);

        new(memory)string();

        {
            bslma::DestructorProctor<string> proctor((string *)memory);
        }

        allocator.deallocate(memory);

        ASSERT(0 == Z.numBytesInUse());

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
