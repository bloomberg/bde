// bslma_autodestructor.t.cpp                                         -*-C++-*-

#include <bslma_autodestructor.h>

#include <bslma_default.h>
#include <bslma_testallocator.h>

#include <bsls_assert.h>
#include <bsls_bsltestutil.h>

#include <stdio.h>      // 'printf'
#include <stdlib.h>     // 'atoi'

#include <new>

using namespace BloombergLP;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// A 'bslma::AutoDestructor' (a kind of "range proctor") is a mechanism (i.e.,
// having state but no value) that is used to conditionally destroy a
// contiguous sequence of objects of parameterized 'TYPE'.  We are mostly
// concerned that this proctor destroys the proper sequence of contiguous
// objects (in unspecified order) according to the specified 'origin' and
// 'length' supplied either at construction or through 'reset' and 'setLength'.
//
// As it happens, there are no basic accessors that can indicate whether the
// origin has been set to the expected value.  Hence, to verify that the
// proctor object is brought to the proper state, we can observe (indirectly)
// that the range of objects proctored by this mechanism is properly destroyed.
// To be able to observe the destruction of proctored objects, we first create
// a test helper class whose destructor produces an observable side-effect when
// invoked.  Objects of this helper class are initialized with the address of a
// unique counter.  As each object is destroyed, its destructor increments the
// counter held by the object, indicating the number of objects being
// destroyed.
//
// With this helper class defined, we can then proceed to address the proctor
// mechanism under test.  We will construct proctors to manage various
// sequences (differing in 'origin' and 'length') of the helper class objects
// in an array.  Rather than using the primary manipulators ('reset' and
// 'setLength'), we will use the "state" constructor to bring the
// 'bslma::AutoDestructor' directly to any desired state.  After each proctor
// is destroyed, we will verify that the corresponding counters of the objects
// managed by the proctor are each incremented exactly once.
//
// Note that the helper objects were constructed on the stack, so an object's
// destructor could be invoked twice -- once when the 'bslma::AutoDestructor'
// goes out of scope, and once when the object itself goes out of scope.
// Although this repeated destruction of helper object is not a problem in
// practice (as the destructor does not release any resources), it is
// technically undefined behavior.  Hence, this test driver employs the
// following technique to avoid the double destruction of helper objects.
//
// Instead of directly creating the helper objects on the stack, we move the
// the data member (a global counter) to a base 'struct', which has nothing but
// the data member itself.  The base 'struct' is effectively a "POD" type.  We
// then construct the base 'struct' on the stack, and use placement 'new' to
// construct the helper object directly on the memory location of the base
// 'struct', which acts as a properly aligned buffer, and the destructor of the
// helper object will be invoked at most once.
//
// Three helper functions are defined to aid testing.  First is a 'load'
// function that constructs helper objects on the provided base 'struct' array.
// Second is an 'areEqual' function that is used to compare two arrays of
// counters.  The third is a 'printArray' function that is used to print an
// array of counters.
//-----------------------------------------------------------------------------
// [4] bslma::AutoDestructor<TYPE>(TYPE *origin, int length = 0);
// [4] ~bslma::AutoDestructor<TYPE>();
// [8] void operator++();
// [8] void operator--();
// [5] void release();
// [6] void reset(TYPE *origin);
// [7] void setLength(int length);
// [7] int length() const;
//-----------------------------------------------------------------------------
// [1] BREATHING TEST
// [2] HELPER CLASS: 'my_Class'
// [3] HELPER FUNCTION: 'areEqual(const int *a1, const int *a2, int ne)'
// [3] HELPER FUNCTION: 'printArray(const int *a1, int ne)'
// [3] HELPER FUNCTION: 'load(my_Struct *b, int *c, int ne)'
// [9] USAGE EXAMPLE

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
//                      CUSTOM TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define PA(X, L) printf( #X " = "); printArray(X, L); printf("\n");
                                              // Print array 'X' of length 'L'
#define PA_(X, L) printf( #X " = "); printArray(X, L); printf(", ");
                                              // PA(X, L) without '\n'

//=============================================================================
//                     GLOBAL VARIABLES FOR TESTING
//-----------------------------------------------------------------------------
static bool globalVeryVeryVerbose = false;

//=============================================================================
//                     HELPER STRUCT/CLASS FOR TESTING
//-----------------------------------------------------------------------------

struct my_Struct {
    // This 'struct' is a POD type that is intended to be inherited by
    // 'my_Class'.

    // DATA
    int *d_counter_p;  // (global) counter to be incremented at destruction
};

//-----------------------------------------------------------------------------

class my_Class : public my_Struct {
    // This object indicates when its destructor is called by incrementing the
    // global counter supplied at construction.

  public:
    // CREATORS
    my_Class(int *counter)
        // Create this object using the address of the specified 'counter' to
        // be held.
    {
        d_counter_p = counter;
        if (globalVeryVeryVerbose) {
            printf("my_Class: "); P_(d_counter_p) P(*d_counter_p);
        }
    }

    ~my_Class()
        // Destroy this object and increment this object's (global) counter.
    {
        ++*d_counter_p;
        if (globalVeryVeryVerbose) {
            printf("~my_Class: "); P_(d_counter_p) P(*d_counter_p);
        }
    }
};

//=============================================================================
//                        STATIC FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

static my_Class *load(my_Struct *buffers, int *counters, int numElements)
    // Construct the specified 'numElements' 'my_Class' objects in the
    // specified 'buffers', supplying the specified 'counters' as respective
    // constructor arguments.  The behavior is undefined unless 'counters' and
    // 'buffers' each refers to an array of at least 'numElements', and
    // '0 <= numElements'.
{
    ASSERT(buffers);
    ASSERT(counters);
    ASSERT(0 <= numElements);

    for (int i = 0; i < numElements; ++i) {
        new(&buffers[i]) my_Class(&counters[i]);
    }

    return static_cast<my_Class *>(buffers);
}

//-----------------------------------------------------------------------------

static bool areEqual(const int *array1, const int *array2, int numElements)
    // Compare the specified initial 'numElements' of the specified integer
    // arrays 'array1' and 'array2'.  Return 'true' if the contents are the
    // same, and 'false' otherwise.  The behavior is undefined unless 'array1'
    // and 'array2' each refers to an 'int' array of at least 'numElements',
    // and '0 <= numElements'.
{
    ASSERT(array1);
    ASSERT(array2);
    ASSERT(0 <= numElements);

    for (int i = 0; i < numElements; ++i) {
        if (array1[i] != array2[i]) {
            return false;                                             // RETURN
        }
    }

    return true;
}

//-----------------------------------------------------------------------------

static void printArray(const int *array, int numElements)
    // Write to 'stdout' the specified 'numElements' of the specified 'array'
    // in a single-line format.  The behavior is undefined unless 'array'
    // refers to an 'int' array of at least 'numElements', and
    // '0 <= numElements'.
{
    ASSERT(array);
    ASSERT(0 <= numElements);

    printf("[ ");
    for (int i = 0; i < numElements; ++i) {
        printf("%d ", array[i]);
    }
    printf("]");
}

//=============================================================================
//                                USAGE EXAMPLE
//-----------------------------------------------------------------------------
// 'bslma::AutoDestructor' is normally used to achieve *exception* *safety*
// in an *exception* *neutral* way by automatically destroying
// (otherwise-unmanaged) orphaned objects for an "in-place" array should an
// exception occur.  The following example illustrates the insertion operation
// for a generic array.  Assume that the array initially contains the following
// five elements:
//..
//     0     1     2     3     4
//   _____ _____ _____ _____ _____ __
//  | "A" | "B" | "C" | "D" | "E" |
//  `=====^=====^=====^=====^=====^==
//..
// To insert an element "F" at index position 2, the existing elements at index
// positions 2 - 4 (i.e., "C", "D", and "E") are first shifted right to create
// an empty spot at the specified insert destination (we assume here and below
// that the array has sufficient capacity).  The elements have to be shifted
// one by one by invoking the copy constructor (immediately followed by
// destroying the original elements).  However, should any of the copy
// construction operations throw, all allocated resources from every previous
// copy construction would be leaked.  Using the 'bslma::AutoDestructor'
// prevents the leak by invoking the destructor of each of the previously
// copied elements should the proctor go out of scope before the 'release'
// method of the proctor is called (such as when the function exits prematurely
// due to an exception):
//..
//     0     1     2     3     4     5     6
//   _____ _____ _____ _____ _____ _____ _____
//  | "A" | "B" | "C" | "D" | "E" |xxxxx|xxxxx|
//  `=====^=====^=====^=====^=====^=====^====='
//  my_Array                            ^----- bslma::AutoDestructor
//  (length = 5)                           (origin = 6, length = 0)
//
//   Note: "xxxxx" denotes uninitialized memory.
//..
// As each of the elements at index positions beyond the insertion position is
// shifted up by one index position, the proctor (i.e., the proctor's length)
// is *decremented*, thereby *extending* by one the sequence of elements it
// manages *below* its origin (note that when the proctor's length is non-
// positive, the element at the origin is *not* managed).  At the same time,
// the array's length is *decremented* to ensure that each array element is
// always being managed (during an allocation attempt) either by the proctor or
// the array itself, but not both:
//..
//     0     1     2     3     4     5     6
//   _____ _____ _____ _____ _____ _____ _____
//  | "A" | "B" | "C" | "D" |xxxxx| "E" |xxxxx|
//  `=====^=====^=====^=====^=====^=====^====='
//  my_Array                      ^----------- bslma::AutoDestructor
//  (length = 4)                          (origin = 6, length = -1)
//
//   Note: Configuration after shifting up one element.
//..
// When all elements are shifted, the 'bslma::AutoDestructor' will protect the
// entire range of shifted objects:
//..
//     0     1     2     3     4     5     6
//   _____ _____ _____ _____ _____ _____ _____
//  | "A" | "B" |xxxxx| "C" | "D" | "E" |xxxxx|
//  `=====^=====^=====^=====^=====^=====^====='
//  my_Array          ^----------------------- bslma::AutoDestructor
//  (length = 2)                           (origin = 6, length = -3)
//
//    Note: Configuration after shifting up three elements.
//..
// Next, a new copy of element "F" must be created.  If, during creation,
// an allocation fails and an exception is thrown, the array (now of length 2)
// is in a valid state, while the proctor is responsible for destroying the
// orphaned elements at index positions 3 - 5.  If no exception is thrown, the
// proctor's 'release()' method is called, releasing its control over the
// temporarily-managed contents:
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
            // Create a 'my_Array' object having an initial length and capacity
            // of 0.  Optionally specify a 'basicAllocator' used to supply
            // memory.  If 'basicAllocator' is 0, the currently installed
            // default allocator is used.

        my_Array(int initialCapacity, bslma::Allocator *basicAllocator = 0);
            // Create a 'my_Array' object having an initial length of 0 and
            // the specified 'initialCapacity'.  Optionally specify a
            // 'basicAllocator' used to supply memory.  If 'basicAllocator' is
            // 0, the currently installed default allocator is used.

        // ...

        ~my_Array();
            // Destroy this 'my_Array' object and all elements currently
            // stored.

        // MANIPULATORS
        // ...

        void insert(int dstIndex, const TYPE& object);
            // Insert (a copy of) the specified 'object' of parameterized
            // 'TYPE' at the specified 'dstIndex' position of this array.  All
            // values with initial indices at or above 'dstIndex' are shifted
            // up by one index position.  The behavior is undefined unless
            // '0 <= dstIndex' and 'dstIndex' is less than the number of items
            // in this array.

        // ...
    };
//..
// Note that the rest of the 'my_Array' interface (above) and implementation
// (below) is omitted as the portion shown is sufficient to demonstrate the use
// of 'bslma::AutoDestructor'.
//..
    // CREATORS
    template <class TYPE>
    inline
    my_Array<TYPE>::my_Array(bslma::Allocator *basicAllocator)
    : d_array_p(0)
    , d_length(0)
    , d_size(0)
    , d_allocator_p(bslma::Default::allocator(basicAllocator))
    {
    }

    template <class TYPE>
    inline
    my_Array<TYPE>::my_Array(int               initialCapacity,
                             bslma::Allocator *basicAllocator)
    : d_length(0)
    , d_size(initialCapacity)
    , d_allocator_p(bslma::Default::allocator(basicAllocator))
    {
        d_array_p = (TYPE *)d_allocator_p->allocate(
                                               sizeof(TYPE) * initialCapacity);
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
// The elided implementation of the following 'insert' function (which shows
// code for the case above, i.e., there is sufficient capacity) is sufficient
// to illustrate the use of 'bslma::AutoDestructor':
//..
    // MANIPULATORS
    template <class TYPE>
    void my_Array<TYPE>::insert(int dstIndex, const TYPE& object)
    {
        BSLS_ASSERT(0 <= dstIndex);
        BSLS_ASSERT(dstIndex <= d_length);

        if (d_size == d_length) {  // resize needed
            // ...
        }

        const TYPE *tmp = &object;

        if ((d_array_p + dstIndex <= &object)
         && (&object < d_array_p + d_length)) {  // self-aliasing
            tmp = &object + 1;
        }

        //***************************************************************
        // Note the use of the auto destructor on 'd_array_p' (below).  *
        //***************************************************************

        bslma::AutoDestructor<TYPE> autoDtor(&d_array_p[d_length + 1], 0);
        int origLen = d_length;
        for (int i = d_length - 1; i >= dstIndex; --i, --autoDtor,
                                                                  --d_length) {
            new(&d_array_p[i + 1]) TYPE(d_array_p[i], d_allocator_p);
                                                           // copy to new index
            d_array_p[i].~TYPE();                          // destroy original
        }

        new(&d_array_p[dstIndex]) TYPE(*tmp, d_allocator_p);

        //******************************************************
        // Note that the auto destructor is released (below).  *
        //******************************************************

        autoDtor.release();

        d_length = origLen + 1;
    }
//..
// Note that the 'insert' method assumes the copy constructor of 'TYPE' takes
// an allocator as a second argument.  In production code, a constructor proxy
// that checks the traits of 'TYPE' (to determine whether 'TYPE' indeed uses
// 'bslma::Allocator') should be used (see 'bslalg_constructorproxy').

//=============================================================================
//           Additional Functionality Needed to Complete Usage Test Case
//-----------------------------------------------------------------------------

class my_AllocatingClass {
    // This object indicates that its destructor is called by incrementing the
    // global counter supplied at construction.  This object also allocates
    // memory at construction, which is necessary in order to trigger a memory
    // allocation exception when invoking the 'insert' function of the
    // 'my_Array' class holding 'my_AllocatingClass' objects.

    // DATA
    my_Class          d_my_Class;     // keeps track of counter
    void             *d_memory_p;     // memory allocated
    bslma::Allocator *d_allocator_p;  // allocator (held, not owned)

  public:
    // CREATORS
    my_AllocatingClass(int *counter, bslma::Allocator *basicAllocator)
        // Create a 'my_AllocatingClass' using the specified (global) 'counter'
        // and 'basicAllocator'.
    : d_my_Class(counter)
    , d_allocator_p(basicAllocator)
    {
        d_memory_p = d_allocator_p->allocate(1);
    }

    my_AllocatingClass(const my_AllocatingClass&  original,
                       bslma::Allocator          *basicAllocator)
        // Create a 'my_AllocatingClass' using the same (global) counter
        // as the specified 'original' and the specified 'basicAllocator'.
    : d_my_Class(original.d_my_Class)
    , d_allocator_p(basicAllocator)
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
    int                 test = argc > 1 ? atoi(argv[1]) : 0;
    bool             verbose = argc > 2;
    bool         veryVerbose = argc > 3;
    bool     veryVeryVerbose = argc > 4;
    bool veryVeryVeryVerbose = argc > 5;

    globalVeryVeryVerbose = veryVeryVerbose;

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:
      case 9: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE TEST
        //
        // Concerns:
        //   The usage example provided in the component header file must
        //   compile, link, and run on all platforms as shown.
        //
        // Plan:
        //   Run the usage example and exercise the creators and manipulators
        //   of 'my_Array' using a 'my_AllocatingClass' and
        //   'bslma::TestAllocator'.  (Display individual memory allocations
        //   and deallocations only in 'veryVeryVeryVerbose' mode.)
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) printf("\nUSAGE EXAMPLE"
                            "\n=============\n");

        bslma::TestAllocator ta(veryVeryVeryVerbose);
        const bslma::TestAllocator& TA = ta;

        enum { NUM_INIT = 5, INIT_SIZE = 8 };
        int counter = 0;

        if (verbose) printf("\nTesting usage example without exception.\n");
        {
            my_Array<my_AllocatingClass> array(INIT_SIZE, &ta);
            const bsls::Types::Int64 INITBYTES = TA.numBytesInUse();

            // First initialize the array to be the same as the usage example.

            for (int i = 0; i < NUM_INIT; ++i) {
                LOOP_ASSERT(i, INITBYTES + i == TA.numBytesInUse());
                array.insert(i, my_AllocatingClass(&counter, &ta));
            }

            if (veryVerbose) {
                T_ P_(counter) P(TA.numBytesInUse())
            }
            ASSERT(INITBYTES + NUM_INIT     == TA.numBytesInUse());

            array.insert(2, my_AllocatingClass(&counter, &ta));

            if (veryVerbose) {
                T_ P_(counter) P(TA.numBytesInUse())
            }
            ASSERT(INITBYTES + NUM_INIT + 1 == TA.numBytesInUse());
        }

        // Verify that all objects are deleted and memory deallocated.
        // Counter should be 15 because:
        // 6 (temporaries) + 6 (inserted into array) + 3 (shifting in array)

        if (veryVerbose) {
            T_ P_(counter) P(TA.numBytesInUse())
        }
        ASSERT(15 == counter);
        ASSERT(0  == TA.numBytesInUse());

        counter = 0;  // reset counter

#ifdef BDE_BUILD_TARGET_EXC
        if (verbose) printf("\nTesting usage example with exception.\n");
        {
            my_Array<my_AllocatingClass> array(INIT_SIZE, &ta);
            const bsls::Types::Int64 INITBYTES = TA.numBytesInUse();

            // First initialize the array to be the same as the usage example.

            for (int i = 0; i < NUM_INIT; ++i) {
                LOOP_ASSERT(i, INITBYTES + i == TA.numBytesInUse());
                array.insert(i, my_AllocatingClass(&counter, &ta));
            }
            ASSERT(INITBYTES + NUM_INIT == TA.numBytesInUse());

            // Ensures insert throws during the shift of internal objects.
            // 1 allocation for constructing temporary, 2 allocations for
            // moving the first 2 objects.  Throws on the 3rd move.

            ta.setAllocationLimit(3);

            try
            {
                array.insert(2, my_AllocatingClass(&counter, &ta));

                ASSERT(0);  // should not get here
            }
            catch (...) {

                // Counter should be 11 because:
                // 6 (temporaries) + 2 (originals of shifted elements)
                // + 2 (guarded by 'bslma::AutoDestructor'
                // + 1 (constructed 'my_Class' object inside
                //      'my_AllocatingClass', before the constructor throws)

                ASSERT(11 == counter);

                // 3 elements left in the array, so 'INITBYTES + 3'.

                if (veryVerbose) {
                    T_ P_(counter) P(TA.numBytesInUse());
                }
                ASSERT(INITBYTES + 3 == TA.numBytesInUse());
            }
        }

        // Verify that all objects are deleted and memory deallocated.

        if (veryVerbose) {
            T_ P_(counter) P(TA.numBytesInUse());
        }
        ASSERT(14 == counter);
        ASSERT(0  == TA.numBytesInUse());
#endif

      } break;
      case 8: {
        // --------------------------------------------------------------------
        // 'operator++' AND 'operator--' TEST
        //
        // Concerns:
        //   That both 'operator++' and 'operator--' respectively increment and
        //   decrement the (signed) length of the array of objects protected by
        //   the range proctor.
        //
        // Plan:
        //   Since the 'length' method is thoroughly tested in case 7, we can
        //   simply verify that 'operator++' and 'operator--' properly set
        //   the length by checking the values returned by 'length()' for
        //   positive, negative, and zero values of length using the
        //   brute-force technique.
        //
        // Testing:
        //   void operator++();
        //   void operator--();
        // --------------------------------------------------------------------

        if (verbose) printf("\n'operator++' AND 'operator--' TEST"
                            "\n==================================\n");

        enum { NUM_BLOCKS = 10 };
        int array[NUM_BLOCKS];  // dummy pointer passed to the proctor
        int expLen = 0;

        if (verbose) printf("\nStart at length 0.\n");

        bslma::AutoDestructor<int> proctor(array, 0);

        if (verbose) printf("\nIncrementing the length by 2.\n");

        ++proctor;
        ++expLen;
        if (veryVerbose) { T_ P_(expLen); P(proctor.length()); }
        LOOP2_ASSERT(expLen, proctor.length(), expLen == proctor.length());

        ++proctor;
        ++expLen;
        if (veryVerbose) { T_ P_(expLen); P(proctor.length()); }
        LOOP2_ASSERT(expLen, proctor.length(), expLen == proctor.length());

        if (verbose) printf("\nDecrementing the length by 4.\n");

        --proctor;
        --expLen;
        if (veryVerbose) { T_ P_(expLen); P(proctor.length()); }
        LOOP2_ASSERT(expLen, proctor.length(), expLen == proctor.length());

        --proctor;
        --expLen;
        if (veryVerbose) { T_ P_(expLen); P(proctor.length()); }
        LOOP2_ASSERT(expLen, proctor.length(), expLen == proctor.length());

        --proctor;
        --expLen;
        if (veryVerbose) { T_ P_(expLen); P(proctor.length()); }
        LOOP2_ASSERT(expLen, proctor.length(), expLen == proctor.length());

        --proctor;
        --expLen;
        if (veryVerbose) { T_ P_(expLen); P(proctor.length()); }
        LOOP2_ASSERT(expLen, proctor.length(), expLen == proctor.length());

        if (verbose) printf("\nIncrementing the length by 2.\n");

        ++proctor;
        ++expLen;
        if (veryVerbose) { T_ P_(expLen); P(proctor.length()); }
        LOOP2_ASSERT(expLen, proctor.length(), expLen == proctor.length());

        ++proctor;
        ++expLen;
        if (veryVerbose) { T_ P_(expLen); P(proctor.length()); }
        LOOP2_ASSERT(expLen, proctor.length(), expLen == proctor.length());

      } break;
      case 7: {
        // --------------------------------------------------------------------
        // 'length' AND 'setLength' TEST
        //
        // Concerns:
        //   1) That the 'length' method properly returns the number of
        //      elements currently protected by a 'const'
        //      'bslma::AutoDestructor'.
        //   2) That 'setLength' properly sets the length of the sequence of
        //      elements protected by the 'bslma::AutoDestructor'.
        //
        // Plan:
        //   Both concerns will be addressed using the table-driven technique.
        //   The origin offset and length are chosen to test all boundary
        //   conditions.
        //
        //   For concern 1, initialize a 'const' 'bslma::AutoDestructor' with
        //   *both* the offset and length specified from a table.  Verify that
        //   'length' returns the expected length.
        //
        //   For concern 2, construct 'bslma::AutoDestructor' to proctor
        //   'my_Class' object arrays (each initialized with a unique global
        //   counter) using the offset from the same test vector.  Once
        //   constructed, change the length by invoking the 'setLength' method.
        //   Verify using 'length' that 'setLength' properly sets the length.
        //   As the 'bslma::AutoDestructor' goes out of scope, verify that the
        //   values of the global counters match the specified expected values
        //   from the table.
        //
        // Testing:
        //   void setLength(int length);
        //   int length() const;
        // --------------------------------------------------------------------

        if (verbose) printf("\n'length' AND 'setLength' TEST"
                            "\n=============================\n");

        if (verbose) printf(
                           "\nTesting the 'length' and 'setLength' method.\n");

        enum { NUM_ELEM = 5 };

        static const struct {
            int d_line;                 // line number
            int d_originOffset;         // origin of proctor
            int d_proctorLength;        // initial length of proctor
            int d_expValues[NUM_ELEM];  // expected counter values
        } DATA[] = {
            // LINE     OFFSET    LENGTH       EXPECTED
            // ----     ------    -------      -----------------

            // Positive Length.
             { L_,        0,         1,        { 1, 0, 0, 0, 0 } },
             { L_,        0,         5,        { 1, 1, 1, 1, 1 } },
             { L_,        1,         1,        { 0, 1, 0, 0, 0 } },
             { L_,        1,         4,        { 0, 1, 1, 1, 1 } },
             { L_,        2,         1,        { 0, 0, 1, 0, 0 } },
             { L_,        2,         3,        { 0, 0, 1, 1, 1 } },
             { L_,        3,         1,        { 0, 0, 0, 1, 0 } },
             { L_,        3,         2,        { 0, 0, 0, 1, 1 } },
             { L_,        4,         1,        { 0, 0, 0, 0, 1 } },

            // Negative Length.
             { L_,        5,        -1,        { 0, 0, 0, 0, 1 } },
             { L_,        5,        -5,        { 1, 1, 1, 1, 1 } },
             { L_,        4,        -1,        { 0, 0, 0, 1, 0 } },
             { L_,        4,        -4,        { 1, 1, 1, 1, 0 } },
             { L_,        3,        -1,        { 0, 0, 1, 0, 0 } },
             { L_,        3,        -3,        { 1, 1, 1, 0, 0 } },
             { L_,        2,        -1,        { 0, 1, 0, 0, 0 } },
             { L_,        2,        -2,        { 1, 1, 0, 0, 0 } },
             { L_,        1,        -1,        { 1, 0, 0, 0, 0 } },

            // 0 Length.
             { L_,        0,         0,        { 0, 0, 0, 0, 0 } },
             { L_,        1,         0,        { 0, 0, 0, 0, 0 } },
             { L_,        2,         0,        { 0, 0, 0, 0, 0 } },
             { L_,        3,         0,        { 0, 0, 0, 0, 0 } },
             { L_,        4,         0,        { 0, 0, 0, 0, 0 } },
             { L_,        5,         0,        { 0, 0, 0, 0, 0 } },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        static const int EMPTY[NUM_ELEM] = { 0 };

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int  LINE     = DATA[ti].d_line;
            const int  OFFSET   = DATA[ti].d_originOffset;
            const int  LENGTH   = DATA[ti].d_proctorLength;
            const int *EXPECTED = DATA[ti].d_expValues;

            if (veryVerbose) {
                T_ P_(LINE) P_(OFFSET) P_(LENGTH) PA(EXPECTED, NUM_ELEM)
            }

            if (veryVeryVerbose) { T_ T_ printf("'length':\n"); }
            {
                int counters[NUM_ELEM] = { 0 };  // initialize to 0

                my_Struct buffer[NUM_ELEM];
                my_Class *classArray = load(buffer, counters, NUM_ELEM);

                {
                    const bslma::AutoDestructor<my_Class> PROCTOR(
                                                           classArray + OFFSET,
                                                           LENGTH);
                    if (veryVeryVerbose) { T_ T_ T_ P(PROCTOR.length()); }
                    LOOP_ASSERT(LINE, LENGTH == PROCTOR.length());
                }
            }

            if (veryVeryVerbose) { T_ T_ printf("'setLength':\n"); }
            {
                int counters[NUM_ELEM] = { 0 };  // initialize to 0

                my_Struct buffer[NUM_ELEM];
                my_Class *classArray = load(buffer, counters, NUM_ELEM);

                if (veryVeryVerbose) { T_ T_ T_ PA(counters, NUM_ELEM); }
                LOOP_ASSERT(LINE, areEqual(EMPTY, counters, NUM_ELEM));
                {
                    bslma::AutoDestructor<my_Class> proctor(
                                                           classArray + OFFSET,
                                                           0);
                    if (veryVeryVerbose) { T_ T_ T_ PA(counters, NUM_ELEM); }
                    LOOP_ASSERT(LINE, areEqual(EMPTY, counters, NUM_ELEM));
                    LOOP_ASSERT(LINE, 0 == proctor.length());

                    proctor.setLength(LENGTH);
                    if (veryVeryVerbose) { T_ T_ T_ PA(counters, NUM_ELEM); }
                    LOOP_ASSERT(LINE, areEqual(EMPTY, counters, NUM_ELEM));
                    LOOP_ASSERT(LINE, LENGTH == proctor.length());
                }
                if (veryVeryVerbose) { T_ T_ T_ PA(counters, NUM_ELEM); }
                LOOP_ASSERT(LINE, areEqual(EXPECTED, counters, NUM_ELEM));
            }
        }

      } break;
      case 6: {
        // --------------------------------------------------------------------
        // 'reset' TEST
        //
        // Concerns:
        //   1) That when the 'reset' method is called, the range proctor
        //      object properly manages a different sequence of objects of the
        //      same length.
        //   2) That 'reset' can be called on a range proctor currently not
        //      managing any objects.
        //
        // Plan:
        //   For concern 1, create three arrays of 'my_Class' objects of equal
        //   length.  Next, initialize two 'bslma::AutoDestructor' proctors,
        //   one with the first array and one with the second.  Invoke 'reset'
        //   on the second proctor with the third array before both proctors go
        //   out of scope.  Once the proctors go out of scope, verify that only
        //   the first and third array's objects are destroyed.
        //
        //   For concern 2, create a range proctor using a null pointer and 0
        //   length.  Invoke 'reset' on the range proctor to manage a sequence
        //   of objects.  Once the proctor goes out of scope, verify that the
        //   objects are destroyed.
        //
        // Testing:
        //   void reset(TYPE *origin);
        // --------------------------------------------------------------------

        if (verbose) printf("\n'reset' TEST"
                            "\n============\n");

        if (verbose) printf("\nTesting 'reset' method.\n");

        if (veryVerbose) printf("\t'reset' on initialized proctor.\n");

        enum { NUM_ELEM = 5 };
        static const int EMPTY[NUM_ELEM] = { 0, 0, 0, 0, 0 };
        static const int FULL[NUM_ELEM]  = { 1, 1, 1, 1, 1 };
        int              cA[NUM_ELEM]    = { 0, 0, 0, 0, 0 };
        int              cB[NUM_ELEM]    = { 0, 0, 0, 0, 0 };
        int              cC[NUM_ELEM]    = { 0, 0, 0, 0, 0 };

        my_Struct bufferA[NUM_ELEM];
        my_Class *classArrayA = load(bufferA, cA, NUM_ELEM);

        my_Struct bufferB[NUM_ELEM];
        my_Class *classArrayB = load(bufferB, cB, NUM_ELEM);

        my_Struct bufferC[NUM_ELEM];
        my_Class *classArrayC = load(bufferC, cC, NUM_ELEM);

        if (veryVerbose) {
            T_ T_ PA_(cA, NUM_ELEM) PA_(cB, NUM_ELEM) PA(cC, NUM_ELEM)
        }
        ASSERT(areEqual(cA, EMPTY, NUM_ELEM));
        ASSERT(areEqual(cB, EMPTY, NUM_ELEM));
        ASSERT(areEqual(cC, EMPTY, NUM_ELEM));
        {
            bslma::AutoDestructor<my_Class> proctorA(classArrayA, NUM_ELEM);
            bslma::AutoDestructor<my_Class> proctorB(classArrayB, NUM_ELEM);

            if (veryVerbose) {
                T_ T_ PA_(cA, NUM_ELEM) PA_(cB, NUM_ELEM) PA(cC, NUM_ELEM)
            }
            ASSERT(areEqual(cA, EMPTY, NUM_ELEM));
            ASSERT(areEqual(cB, EMPTY, NUM_ELEM));
            ASSERT(areEqual(cC, EMPTY, NUM_ELEM));

            proctorB.reset(classArrayC);

            if (veryVerbose) {
                T_ T_ PA_(cA, NUM_ELEM) PA_(cB, NUM_ELEM) PA(cC, NUM_ELEM)
            }
            ASSERT(areEqual(cA, EMPTY, NUM_ELEM));
            ASSERT(areEqual(cB, EMPTY, NUM_ELEM));
            ASSERT(areEqual(cC, EMPTY, NUM_ELEM));
        }
        if (veryVerbose) {
            T_ T_ PA_(cA, NUM_ELEM) PA_(cB, NUM_ELEM) PA(cC, NUM_ELEM)
        }
        ASSERT(areEqual(cA, FULL,  NUM_ELEM));
        ASSERT(areEqual(cB, EMPTY, NUM_ELEM));
        ASSERT(areEqual(cC, FULL,  NUM_ELEM));

        if (veryVerbose) printf("\t'reset' on uninitialized proctor.\n");

        int        c[NUM_ELEM] = { 0, 0, 0, 0, 0 };
        my_Struct  buffer[NUM_ELEM];
        my_Class  *classArray = load(buffer, c, NUM_ELEM);

        if (veryVerbose) { T_ T_ PA(c, NUM_ELEM) }
        ASSERT(areEqual(c, EMPTY, NUM_ELEM));
        {
            bslma::AutoDestructor<my_Class> proctor(0);
            proctor.reset(classArray);

            if (veryVerbose) { T_ T_ PA(c, NUM_ELEM) }
            ASSERT(areEqual(c, EMPTY, NUM_ELEM));

            proctor.setLength(NUM_ELEM);

            if (veryVerbose) { T_ T_ PA(c, NUM_ELEM) }
            ASSERT(areEqual(c, EMPTY, NUM_ELEM));
        }
        if (veryVerbose) { T_ T_ PA(c, NUM_ELEM) }
        ASSERT(areEqual(c, FULL, NUM_ELEM));

      } break;
      case 5: {
        // --------------------------------------------------------------------
        // 'release' TEST
        //
        // Concerns:
        //   That when the 'release' method is called, the range proctor object
        //   properly releases from management the sequence of objects
        //   currently managed by the proctor.
        //
        // Plan:
        //   Create two arrays of 'my_Class' objects of equal length, one to be
        //   used as a control.  Next initialize two 'bslma::AutoDestructor'
        //   proctors, one with the first array and the other with the second.
        //   Invoke 'release' on the second proctor before it goes out of
        //   scope.  Verify that only the objects managed by the first proctor
        //   are destroyed.
        //
        // Testing:
        //   void release();
        // --------------------------------------------------------------------

        if (verbose) printf("\n'release' TEST"
                            "\n==============\n");

        if (verbose) printf("\nTesting the 'release' method.\n");

        enum { NUM_ELEM = 5 };

        static const int EMPTY[NUM_ELEM] = { 0, 0, 0, 0, 0 };
        static const int FULL[NUM_ELEM]  = { 1, 1, 1, 1, 1 };
        int              cA[NUM_ELEM]    = { 0, 0, 0, 0, 0 };
        int              cB[NUM_ELEM]    = { 0, 0, 0, 0, 0 };

        my_Struct bufferA[NUM_ELEM];
        my_Class *classArrayA = load(bufferA, cA, NUM_ELEM);

        my_Struct bufferB[NUM_ELEM];
        my_Class *classArrayB = load(bufferB, cB, NUM_ELEM);

        if (veryVerbose) {
            T_ PA_(cA, NUM_ELEM) PA(cB, NUM_ELEM)
        }
        {
            bslma::AutoDestructor<my_Class> proctorA(classArrayA, NUM_ELEM);
            bslma::AutoDestructor<my_Class> proctorB(classArrayB, NUM_ELEM);

            if (veryVerbose) {
                T_ PA_(cA, NUM_ELEM) PA(cB, NUM_ELEM)
            }
            ASSERT(areEqual(cA, EMPTY, NUM_ELEM));
            ASSERT(areEqual(cB, EMPTY, NUM_ELEM));

            proctorB.release();

            if (veryVerbose) {
                T_ PA_(cA, NUM_ELEM) PA(cB, NUM_ELEM)
            }
            ASSERT(areEqual(cA, EMPTY, NUM_ELEM));
            ASSERT(areEqual(cB, EMPTY, NUM_ELEM));
        }
        if (veryVerbose) {
            T_ PA_(cA, NUM_ELEM) PA(cB, NUM_ELEM)
        }
        ASSERT(areEqual(cA, FULL,  NUM_ELEM));
        ASSERT(areEqual(cB, EMPTY, NUM_ELEM));

      } break;
      case 4: {
        // --------------------------------------------------------------------
        // CTOR / DTOR TEST
        //
        // Concerns:
        //: 1 That the 'bslma::AutoDestructor' guards a sequence of objects of
        //:   the provided length (positive).
        //: 2 That the 'bslma::AutoDestructor' guards a sequence of objects of
        //:   the provided length (negative).
        //: 3 That the 'bslma::AutoDestructor' guards nothing when supplied a
        //:   zero length and non-zero origin at construction.
        //: 4 That we can construct a 'bslma::AutoDestructor' with zero length
        //:   and a null origin.
        //: 5 That length is default constructed to 0.
        //
        // Plan:
        //   Concerns 1 - 3 will be addressed using the table-driven technique.
        //   'bslma::AutoDestructor' will be constructed to proctor 'my_Class'
        //   object arrays (each initialized with a unique global counter)
        //   using the offset and length specified from the table.  As the
        //   'bslma::AutoDestructor' goes out of scope, verify that the values
        //   of the global counters match the specified expected values from
        //   the table (i.e., only the counters of the proctored objects are
        //   incremented).  The origin and length are chosen to test all
        //   boundary conditions.
        //
        //   Concern 4 will be tested separately afterwards by constructing a
        //   'bslma::AutoDestructor' with zero length and a null pointer.
        //
        //   Concern 5 will also be tested separately by constructing a
        //   'bslma::AutoDestructor' with an array of 'my_Class' objects
        //   without specifying the length.  Verify that the counters used to
        //   initialize the objects are not incremented.
        //
        // Testing:
        //   bslma::AutoDestructor<TYPE>(TYPE *origin, int length = 0);
        //   ~bslma::AutoDestructor<TYPE>();
        // --------------------------------------------------------------------

        if (verbose) printf("\nCTOR / DOTR TEST"
                            "\n================\n");

        if (verbose) printf("\nTesting CTOR and DTOR.\n");

        enum { NUM_ELEM = 5 };

        static const struct {
            int d_line;                 // line number
            int d_originOffset;         // origin of proctor
            int d_proctorLength;        // initial length of proctor
            int d_expValues[NUM_ELEM];  // expected counter values
        } DATA[] = {
            // LINE   OFFSET   LENGTH    EXPECTED
            // ----   ------   -------   -----------------

            // Concern 1: guarding positive length.
             { L_,       0,      1,      { 1, 0, 0, 0, 0 } },
             { L_,       0,      5,      { 1, 1, 1, 1, 1 } },
             { L_,       1,      1,      { 0, 1, 0, 0, 0 } },
             { L_,       1,      4,      { 0, 1, 1, 1, 1 } },
             { L_,       2,      1,      { 0, 0, 1, 0, 0 } },
             { L_,       2,      3,      { 0, 0, 1, 1, 1 } },
             { L_,       3,      1,      { 0, 0, 0, 1, 0 } },
             { L_,       3,      2,      { 0, 0, 0, 1, 1 } },
             { L_,       4,      1,      { 0, 0, 0, 0, 1 } },

            // Concern 2: guarding negative length.
             { L_,       5,     -1,      { 0, 0, 0, 0, 1 } },
             { L_,       5,     -5,      { 1, 1, 1, 1, 1 } },
             { L_,       4,     -1,      { 0, 0, 0, 1, 0 } },
             { L_,       4,     -4,      { 1, 1, 1, 1, 0 } },
             { L_,       3,     -1,      { 0, 0, 1, 0, 0 } },
             { L_,       3,     -3,      { 1, 1, 1, 0, 0 } },
             { L_,       2,     -1,      { 0, 1, 0, 0, 0 } },
             { L_,       2,     -2,      { 1, 1, 0, 0, 0 } },
             { L_,       1,     -1,      { 1, 0, 0, 0, 0 } },

            // Concern 3: guarding 0 length.
             { L_,       0,      0,      { 0, 0, 0, 0, 0 } },
             { L_,       1,      0,      { 0, 0, 0, 0, 0 } },
             { L_,       2,      0,      { 0, 0, 0, 0, 0 } },
             { L_,       3,      0,      { 0, 0, 0, 0, 0 } },
             { L_,       4,      0,      { 0, 0, 0, 0, 0 } },
             { L_,       5,      0,      { 0, 0, 0, 0, 0 } },

            // Note that concern 4 and 5 are tested separately.
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        static const int EMPTY[NUM_ELEM] = { 0 };

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int  LINE     = DATA[ti].d_line;
            const int  OFFSET   = DATA[ti].d_originOffset;
            const int  LENGTH   = DATA[ti].d_proctorLength;
            const int *EXPECTED = DATA[ti].d_expValues;

            if (veryVerbose) {
                T_ P_(LINE) P_(OFFSET) P_(LENGTH) PA(EXPECTED, NUM_ELEM)
            }

            int counters[NUM_ELEM] = { 0 };  // initialize to 0

            my_Struct buffer[NUM_ELEM];
            my_Class *classArray = load(buffer, counters, NUM_ELEM);

            if (veryVeryVerbose) { T_ T_ PA(counters, NUM_ELEM) }
            LOOP_ASSERT(LINE, areEqual(EMPTY, counters, NUM_ELEM));
            {
                bslma::AutoDestructor<my_Class> proctor(classArray + OFFSET,
                                                        LENGTH);

                if (veryVeryVerbose) { T_ T_ PA(counters, NUM_ELEM) }
                LOOP_ASSERT(LINE, areEqual(EMPTY, counters, NUM_ELEM));
            }
            if (veryVeryVerbose) { T_ T_ PA(counters, NUM_ELEM) }
            LOOP_ASSERT(LINE, areEqual(EXPECTED, counters, NUM_ELEM));
        }

        if (verbose) printf("\nTesting Construction with null pointer.\n");
        {
            bslma::AutoDestructor<my_Class> destructor(0, 0);
        }

        if (verbose) printf(
                          "\nTesting length defaults to 0 at construction.\n");

        int counters[NUM_ELEM] = { 0 };

        my_Struct buffer[NUM_ELEM];
        my_Class *classArray = load(buffer, counters, NUM_ELEM);

        if (veryVeryVerbose) { T_ T_ PA(counters, NUM_ELEM) }
        ASSERT(areEqual(EMPTY, counters, NUM_ELEM));
        {
            bslma::AutoDestructor<my_Class> destructor(classArray);
            if (veryVeryVerbose) { T_ T_ PA(counters, NUM_ELEM) }
            ASSERT(areEqual(EMPTY, counters, NUM_ELEM));
        }
        if (veryVeryVerbose) { T_ T_ PA(counters, NUM_ELEM) }
        ASSERT(areEqual(EMPTY, counters, NUM_ELEM));

      } break;
      case 3: {
        // --------------------------------------------------------------------
        // HELPER FUNCTIONS TEST
        //
        // Concerns:
        //   1) 'areEqual' satisfies the following concerns:
        //      a) That the helper function properly returns whether the first
        //         'numElements' in two integer arrays are equal.
        //      b) That two empty arrays are equal.
        //      c) That similar but different arrays (differing by one element)
        //         are not equal.
        //   2) 'printArray' properly prints the value of each counter in the
        //      provided array of counters.
        //   3) 'load' properly constructs 'my_Class' objects in the provided
        //      buffer.
        //
        // Plan:
        //   For concern 1, use the table-driven technique to compare similar
        //   integer arrays of varying length, and verify true is returned only
        //   if all the respective element values are the same.
        //
        //   For concern 2, print an array of counter using 'printArray' (in
        //   'veryVerbose' mode) and verify visually.
        //
        //   For concern 3, using the loop-based technique, create an array of
        //   'my_Struct' objects on the stack, then use 'load' to construct an
        //   array of 'my_Class' objects with an array of counters.  Let the
        //   array of 'my_struct' objects go out of scope, and use 'areEqual'
        //   to verify that each respective counter used to construct the
        //   'my_Class' objects is incremented.
        //
        // Testing:
        //   HELPER FUNCTION: 'areEqual(const int *a1, const int *a2, int ne)'
        //   HELPER FUNCTION: 'printArray(const int *a1, int ne)'
        //   HELPER FUNCTION: 'load(my_Struct *b, int *c, int ne)'
        // --------------------------------------------------------------------

        if (verbose) printf("\nHELPER FUNCTIONS TEST"
                            "\n=====================\n");

        enum { NUM_TEST = 5 };

        if (verbose) printf("\nTesting 'areEqual'.\n");
        {
            enum { SZ = 5 };
            const static struct {
                int d_line;        // line number
                int d_array1[SZ];  // first array argument
                int d_array2[SZ];  // second array argument
                int d_length;      // number of elements to compare
                int d_exp;         // expected result
            } DATA[] = {
       // LINE   ARRAY1                   ARRAY2               LENGTH    RESULT
       // ----   ----------------         ----------------     ------    ------

       // Testing length (equal)
        { L_,    { 1, 2, 3, 4, 5 },       { 1, 7, 8, 9, 0 },     1,        1 },
        { L_,    { 1, 2, 3, 4, 5 },       { 1, 2, 8, 9, 0 },     2,        1 },
        { L_,    { 1, 2, 3, 4, 5 },       { 1, 2, 3, 9, 0 },     3,        1 },
        { L_,    { 1, 2, 3, 4, 5 },       { 1, 2, 3, 4, 0 },     4,        1 },
        { L_,    { 1, 2, 3, 4, 5 },       { 1, 2, 3, 4, 5 },     5,        1 },

       // Testing length (differ in last element)
        { L_,    { 1, 2, 3, 4, 5 },       { 6, 7, 8, 9, 0 },     1,        0 },
        { L_,    { 1, 2, 3, 4, 5 },       { 1, 7, 8, 9, 0 },     2,        0 },
        { L_,    { 1, 2, 3, 4, 5 },       { 1, 2, 8, 9, 0 },     3,        0 },
        { L_,    { 1, 2, 3, 4, 5 },       { 1, 2, 3, 9, 0 },     4,        0 },
        { L_,    { 1, 2, 3, 4, 5 },       { 1, 2, 3, 4, 0 },     5,        0 },

       // Testing length (differ in first element)
        { L_,    { 1, 2, 3, 4, 5 },       { 6, 2, 8, 9, 0 },     2,        0 },
        { L_,    { 1, 2, 3, 4, 5 },       { 6, 2, 3, 9, 0 },     3,        0 },
        { L_,    { 1, 2, 3, 4, 5 },       { 6, 2, 3, 4, 0 },     4,        0 },
        { L_,    { 1, 2, 3, 4, 5 },       { 6, 2, 3, 4, 5 },     5,        0 },

       // Testing empty array
        { L_,    { 1, 2, 3, 4, 5 },       { 6, 7, 8, 9, 0 },     0,        1 },

       // Testing similar arrays
        { L_,    { 1, 0, 0, 0, 0 },       { 0, 0, 0, 0, 0 },     5,        0 },
        { L_,    { 0, 1, 0, 0, 0 },       { 0, 0, 0, 0, 0 },     5,        0 },
        { L_,    { 0, 0, 1, 0, 0 },       { 0, 0, 0, 0, 0 },     5,        0 },
        { L_,    { 0, 0, 0, 1, 0 },       { 0, 0, 0, 0, 0 },     5,        0 },
        { L_,    { 0, 0, 0, 0, 1 },       { 0, 0, 0, 0, 0 },     5,        0 },
        { L_,    { 0, 0, 0, 0, 0 },       { 1, 0, 0, 0, 0 },     5,        0 },
        { L_,    { 0, 0, 0, 0, 0 },       { 0, 1, 0, 0, 0 },     5,        0 },
        { L_,    { 0, 0, 0, 0, 0 },       { 0, 0, 1, 0, 0 },     5,        0 },
        { L_,    { 0, 0, 0, 0, 0 },       { 0, 0, 0, 1, 0 },     5,        0 },
        { L_,    { 0, 0, 0, 0, 0 },       { 0, 0, 0, 0, 1 },     5,        0 },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int  LINE   = DATA[ti].d_line;
                const int *ARRAY1 = DATA[ti].d_array1;
                const int *ARRAY2 = DATA[ti].d_array2;
                const int  LENGTH = DATA[ti].d_length;
                const int  RESULT = DATA[ti].d_exp;

                if (veryVerbose) {
                    T_ P_(LINE); P_(LENGTH); P(RESULT)
                    T_ PA_(ARRAY1, SZ); PA(ARRAY2, SZ);
                }
                LOOP_ASSERT(LINE, RESULT == areEqual(ARRAY1, ARRAY2, LENGTH));
            }
        }

        if (verbose) printf("\nTesting 'printArray'.\n");
        {
            int counters[NUM_TEST] = { 0, 1, 2, 3, 4 };
            if (veryVerbose) {
                printf("\tShould be printing: [ 0 1 2 3 4 ]\n");
                printf("\tIs printing       : "); PA(counters, NUM_TEST)
            }
        }


        if (verbose) printf("\nTesting 'load'.\n");

        {
            int counters[NUM_TEST] = { 0 };
            int expected[NUM_TEST] = { 0 };
            my_Struct buffer[NUM_TEST];
            my_Class *array = load(buffer, counters, NUM_TEST);

            if (veryVerbose) {
                T_ PA_(counters, NUM_TEST) PA(expected, NUM_TEST)
            }
            ASSERT(areEqual(counters, expected, NUM_TEST));

            for (int i = 0; i < NUM_TEST; ++i) {
                array[i].~my_Class();
                ++expected[i];

                if (veryVerbose) {
                    T_ PA_(counters, NUM_TEST) PA(expected, NUM_TEST)
                }
                LOOP_ASSERT(i, areEqual(counters, expected, NUM_TEST));
            }
        }

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // HELPER CLASS TEST
        //
        // Concerns:
        //   The helper class 'my_Class' properly increments its counter at
        //   destruction.
        //
        // Plan:
        //   Construct 'my_Class' objects passing the address of a counter
        //   variable initialized to 0.  Verify that the counter is incremented
        //   after each 'my_Class' object is destroyed.
        //
        // Testing:
        //   HELPER CLASS: 'my_Class'
        // --------------------------------------------------------------------

        if (verbose) printf("\nHELPER CLASS TEST"
                            "\n=================\n");

        if (verbose) printf("\nTesting 'my_Class' ctor and dtor.\n");

        {
            enum { NUM_TEST = 5 };
            int counter = 0;
            for (int i = 0; i < NUM_TEST; ++i) {
                if (veryVerbose) {
                    T_ P_(i) P(counter)
                }

                LOOP_ASSERT(i, i == counter);
                {
                    my_Class X(&counter);
                    LOOP_ASSERT(i, i == counter);
                }
                LOOP_ASSERT(i, i + 1 == counter);
            }
        }

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //
        // Concerns:
        //   1) A 'bslma::AutoDestructor' can be created and destroyed
        //      gracefully.
        //   2) The objects managed by 'bslma::AutoDestructor' are destroyed
        //      when the range proctor goes out of scope.
        //
        // Plan:
        //   Create an array of 'my_Class' objects initialized with the same
        //   counter.  Proctor the array of objects with a
        //   'bslma::AutoDestructor'.  Verify that when the range proctor goes
        //   out of scope, the counter is incremented.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) printf("\nBREATHING TEST"
                            "\n==============\n");

        if (verbose) printf("\nProctor 3 'my_Class' objects.\n");

        enum { NUM_TEST = 3 };
        static const int EMPTY[NUM_TEST]    = { 0, 0, 0 };
        static const int FULL[NUM_TEST]     = { 1, 1, 1 };
        int              counters[NUM_TEST] = { 0, 0, 0 };

        my_Struct buffer[NUM_TEST];
        my_Class *classArray = load(buffer, counters, NUM_TEST);

        if (veryVerbose) { T_ PA(counters, NUM_TEST) }
        ASSERT(areEqual(EMPTY, counters, NUM_TEST));
        {
            bslma::AutoDestructor<my_Class> proctor(classArray, NUM_TEST);

            if (veryVerbose) { T_ PA(counters, NUM_TEST) }
            ASSERT(areEqual(EMPTY, counters, NUM_TEST));
        }
        if (veryVerbose) { T_ PA(counters, NUM_TEST) }
        ASSERT(areEqual(FULL, counters, NUM_TEST));

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
