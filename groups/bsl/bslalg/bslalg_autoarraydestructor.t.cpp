// bslalg_autoarraydestructor.t.cpp                                   -*-C++-*-

#include <bslalg_autoarraydestructor.h>

#include <bslalg_scalarprimitives.h>             // for testing only
#include <bslalg_typetraits.h>                   // for testing only
#include <bslalg_typetraitusesbslmaallocator.h>  // for testing only

#include <bslma_allocator.h>                     // for testing only
#include <bslma_deallocatorproctor.h>            // for testing only
#include <bslma_autodestructor.h>                // for testing only
#include <bslma_default.h>                       // for testing only
#include <bslma_testallocator.h>                 // for testing only
#include <bslma_testallocatorexception.h>        // for testing only
#include <bsls_alignmentutil.h>                  // for testing only
#include <bsls_assert.h>                         // for testing only
#include <bsls_asserttest.h>                     // for testing only
#include <bsls_stopwatch.h>                      // for testing only

#include <cstdio>
#include <cstdlib>     // atoi()
#include <cstring>     // strlen()
#include <ctype.h>     // isalpha()

using namespace BloombergLP;
using namespace std;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// The component to be tested provides a proctor to help with exception-safety
// guarantees.  The test sequence is very simple: we only have to ascertain
// that the index computation is correct and that the proctor does destroy its
// guarded range unless 'release has been called.  We use a test type that
// monitors the number of constructions and destructions, and that allocates in
// order to take advantage of the standard 'bslma' exception test.
//-----------------------------------------------------------------------------
// [ 2] bslalg_AutoArrayDestructor(T *b, T *e);
// [ 2] ~AutoArrayDestructor();
// [ 2] T *moveBegin(ptrdiff_t ne = -1);
// [ 2] T *moveEnd(ptrdiff_t ne = 1);
// [ 3] void release();
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 4] USAGE EXAMPLE

//=============================================================================
//                  STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
// NOTE: THIS IS A LOW-LEVEL COMPONENT AND MAY NOT USE ANY C++ LIBRARY
// FUNCTIONS, INCLUDING IOSTREAMS.
int testStatus = 0;

namespace {
    void aSsErT(int c, const char *s, int i) {
    if (c) {
        printf("Error " __FILE__ "(%d): %s    (failed)\n", i, s);
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}
}

# define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }
//=============================================================================
//                  STANDARD BDE LOOP-ASSERT TEST MACROS
//-----------------------------------------------------------------------------
// NOTE: This implementation of LOOP_ASSERT macros must use printf since
//       cout uses new and be called during exception testing.
#define LOOP_ASSERT(I,X) { \
    if (!(X)) { printf("%s: %d\n", #I, I); aSsErT(1, #X, __LINE__); } }

#define LOOP2_ASSERT(I,J,X) { \
    if (!(X)) { printf("%s: %d\t%s: %d\n", #I, I, #J, J); \
                aSsErT(1, #X, __LINE__); } }

#define LOOP3_ASSERT(I,J,K,X) { \
    if (!(X)) { printf("%s: %d\t%s: %c\t%s: %c\n", #I, I, #J, J, #K, K); \
                aSsErT(1, #X, __LINE__); } }

#define LOOP4_ASSERT(I,J,K,L,X) { \
    if (!(X)) { printf("%s: %d\t%s: %d\t%s: %d\t%s: %d\n", \
                #I, I, #J, J, #K, K, #L, L); aSsErT(1, #X, __LINE__); } }

//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
// #define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) printf("<| " #X " |>\n");  // Quote identifier literally.
//#define P_(X) cout << #X " = " << (X) << ", " << flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number
#define T_ printf("\t");             // Print a tab (w/o newline)

//=============================================================================
//                  SEMI-STANDARD NEGATIVE-TESTING MACROS
//-----------------------------------------------------------------------------
#define ASSERT_SAFE_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(EXPR)
#define ASSERT_SAFE_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPR)
#define ASSERT_PASS(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS(EXPR)
#define ASSERT_FAIL(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL(EXPR)
#define ASSERT_OPT_PASS(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS(EXPR)
#define ASSERT_OPT_FAIL(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL(EXPR)

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS/TYPES FOR TESTING
//-----------------------------------------------------------------------------

// TYPES
class TestType;

typedef TestType                      T;    // uses 'bslma' allocators

// STATIC DATA
static int verbose, veryVerbose, veryVeryVerbose;

const int MAX_ALIGN = bsls_AlignmentUtil::BSLS_MAX_ALIGNMENT;

static int numDefaultCtorCalls = 0;
static int numCharCtorCalls    = 0;
static int numCopyCtorCalls    = 0;
static int numAssignmentCalls  = 0;
static int numDestructorCalls  = 0;

bslma_TestAllocator *Z;  // initialized at the start of main()

                               // ==============
                               // class TestType
                               // ==============

class TestType {
    // This test type contains a 'char' in some allocated storage.  It counts
    // the number of default and copy constructions, assignments, and
    // destructions.  It has no traits other than using a 'bslma' allocator.
    // It could have the bit-wise moveable traits but we defer that trait to
    // the 'MoveableTestType'.

    char            *d_data_p;
    bslma_Allocator *d_allocator_p;

  public:
    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS(TestType,
                                 bslalg_TypeTraitUsesBslmaAllocator);

    // CREATORS
    TestType(bslma_Allocator *ba = 0)
    : d_data_p(0)
    , d_allocator_p(bslma_Default::allocator(ba))
    {
        ++numDefaultCtorCalls;
        d_data_p  = (char *)d_allocator_p->allocate(sizeof(char));
        *d_data_p = '?';
    }

    TestType(char c, bslma_Allocator *ba = 0)
    : d_data_p(0)
    , d_allocator_p(bslma_Default::allocator(ba))
    {
        ++numCharCtorCalls;
        d_data_p  = (char *)d_allocator_p->allocate(sizeof(char));
        *d_data_p = c;
    }

    TestType(const TestType& original, bslma_Allocator *ba = 0)
    : d_data_p(0)
    , d_allocator_p(bslma_Default::allocator(ba))
    {
        ++numCopyCtorCalls;
        if (&original != this) {
        d_data_p  = (char *)d_allocator_p->allocate(sizeof(char));
        *d_data_p = *original.d_data_p;
    }
    }

    ~TestType() {
        ++numDestructorCalls;
        *d_data_p = '_';
        d_allocator_p->deallocate(d_data_p);
        d_data_p = 0;
    }

    // MANIPULATORS
    TestType& operator=(const TestType& rhs)
    {
        ++numAssignmentCalls;
        if (&rhs != this) {
        char *newData = (char *)d_allocator_p->allocate(sizeof(char));
        *d_data_p = '_';
        d_allocator_p->deallocate(d_data_p);
        d_data_p  = newData;
        *d_data_p = *rhs.d_data_p;
        }
        return *this;
    }

    void setDatum(char c) {
        *d_data_p = c;
    }

    // ACCESSORS
    char datum() const {
        return *d_data_p;
    }

    void print() const
    {
        if (d_data_p) {
        ASSERT(isalpha(*d_data_p));
            printf("%c (int: %d)\n", *d_data_p, (int)*d_data_p);
        } else {
            printf("VOID\n");
        }
    }
};

bool operator==(const TestType& lhs, const TestType& rhs)
{
    ASSERT(isalpha(lhs.datum()));
    ASSERT(isalpha(rhs.datum()));

    return lhs.datum() == rhs.datum();
}

//=============================================================================
//                            USAGE EXAMPLE
//-----------------------------------------------------------------------------

// my_string.h

                              // ===============
                              // class my_String
                              // ===============

class my_String {

    // DATA
    char *d_string_p;
    int   d_length;
    int   d_size;

  public:
    // CREATORS
    my_String(const char *string);
    my_String(const my_String& original);
    ~my_String();
    // ...

    /// ACCESSORS
    inline int length() const { return d_length; }
    inline operator const char *() const { return d_string_p; }
    // ...
};

// FREE OPERATORS
inline bool operator==(const my_String& lhs, const char *rhs)
{
    return strcmp(lhs, rhs) == 0;
}

// ...

// my_string.cpp

                              // ===============
                              // class my_String
                              // ===============

// CREATORS
my_String::my_String(const char *string)
: d_length(strlen(string))
{
    ASSERT(string);
    d_size = d_length + 1;
    d_string_p = (char *) operator new(d_size);
    memcpy(d_string_p, string, d_size);
}

my_String::my_String(const my_String& original)
: d_length(original.d_length)
, d_size(original.d_length + 1)
{
    d_string_p = (char *) operator new(d_size);
    memcpy(d_string_p, original.d_string_p, d_size);
}

my_String::~my_String()
{
    ASSERT(d_string_p);
    delete d_string_p;
}

// ...

// my_array.h

                               // ==============
                               // class my_Array
                               // ==============

template <class TYPE>
class my_Array {
    // This extremely simple 'vector'-like class is merely to demonstrate that
    // the usage example works properly.  For better testing, it uses a
    // test allocator.

    // DATA
    TYPE            *d_array_p; // dynamically allocated array
    int              d_size;    // physical capacity of this array
    int              d_length;  // logical length of this array
    bslma_Allocator *d_alloc_p;

  private:
    // PRIVATE TYPES
    enum {
        INITIAL_SIZE = 1, // initial physical capacity
        GROW_FACTOR = 2   // multiplicative factor by which to grow
                          // 'd_size'
    };

    // CLASS METHODS
    static int nextSize(int size, int newSize);
    static void reallocate(TYPE            **array,
                           int              *size,
                           int               newSize,
                           int               length,
                           bslma_Allocator  *allocator);

    // PRIVATE MANIPULATORS
    void increaseSize(int numElements);

  public:
    // CREATORS
    my_Array(bslma_Allocator *allocator);
    ~my_Array();

    // MANIPULATORS
    void insert(int dstIndex, const TYPE& item, int numItems);

    // ACCESSORS
    inline int length() const { return d_length; }
    inline const TYPE& operator[](int index) const { return d_array_p[index]; }
};

                               // --------------
                               // class my_Array
                               // --------------

// CLASS METHODS
template <class TYPE> inline
int my_Array<TYPE>::nextSize(int size, int newSize) {
    while (size < newSize) {
        size *= GROW_FACTOR;
    }
    return size;
}

template <class TYPE> inline
void my_Array<TYPE>::reallocate(TYPE            **array,
                                int              *size,
                                int               newSize,
                                int               length,
                                bslma_Allocator  *allocator)
    // Reallocate memory in the specified 'array' and update the
    // specified size to the specified 'newSize'.  The specified 'length'
    // number of leading elements are preserved.  If 'allocator' should throw
    // an exception, this function has no effect.  The behavior is
    // undefined unless 1 <= newSize, 0 <= length, and newSize <= length.
    // Note that an "auto deallocator" is needed here to ensure that
    // memory allocated for the new array is deallocated when an exception
    // occurs.
{
    ASSERT(0 < size);
    ASSERT(1 <= newSize);
    ASSERT(0 <= length);
    ASSERT(length <= *size);    // sanity check
    ASSERT(length <= newSize);  // ensure class invariant

    TYPE *newArray = (TYPE *) allocator->allocate(newSize * sizeof **array);

    // 'autoDealloc' and 'autoDtor' are destroyed in reverse order

    bslma_DeallocatorProctor<bslma_Allocator> autoDealloc(newArray, allocator);
    bslma_AutoDestructor<TYPE>                autoDtor(newArray, 0);
    int i;
    for (i = 0; i < length; ++i, ++autoDtor) {
        new(&newArray[i]) TYPE((*array)[i]);
    }
    autoDtor.release();
    autoDealloc.release();

    for (i = 0; i < length; ++i) {
        (*array)[i].~TYPE();
    }
    allocator->deallocate((void *) *array);
    *array = newArray;
    *size  = newSize;
}

// PRIVATE MANIPULATORS
template <class TYPE> inline
void my_Array<TYPE>::increaseSize(int numElements)
{
    reallocate(&d_array_p,
               &d_size,
               nextSize(d_size, d_size + numElements),
               d_length,
               d_alloc_p);
}

// CREATORS
template <class TYPE> inline
my_Array<TYPE>::my_Array(bslma_Allocator *allocator)
: d_size(INITIAL_SIZE)
, d_length(0)
, d_alloc_p(allocator)
{
    d_array_p = (TYPE *) d_alloc_p->allocate(d_size * sizeof *d_array_p);
}

template <class TYPE> inline
my_Array<TYPE>::~my_Array()
{
    for (int i = 0; i < d_length; ++i) {
        d_array_p[i].~TYPE();
    }
    d_alloc_p->deallocate((void *) d_array_p); // delete all allocated memory
}

// MANIPULATORS
// as part of the usage example (below)

///Usage
///-----
// The usage example is nearly identical to that of 'bslma_autodestructor', so
// we will only quote and adapt a small portion of that usage example.
// Namely, we will focus on an array that supports arbitrary user-defined
// types, and suppose that we want to implement insertion of an arbitrary
// number of elements at some (intermediate) position in the array, taking care
// that if an element copy constructor or assignment operator throws, the whole
// array is left in a valid (but unspecified) state.
//
// Consider the implementation of the 'insert' method for a templatized array
// below.  The proctor's *origin* is set (at construction) to refer to the
// 'numItems' position past 'array[length]'.  Initially, the proctor manages no
// objects (i.e., its end is the same as its beginning).
//..
//     0     1     2     3     4     5     6     7
//   _____ _____ _____ _____ _____ _____ _____ _____
//  | "A" | "B" | "C" | "D" | "E" |xxxxx|xxxxx|xxxxx|
//  `=====^=====^=====^=====^=====^=====^=====^====='
//  my_Array                                  ^----- bslalg_AutoArrayDestructor
//  (length = 5)
//
//              Figure: Use of proctor for my_Array::insert
//..
// As each of the elements at index positions beyond the insertion position is
// shifted up by two index positions, the proctor's begin address is
// *decremented*.  At the same time, the array's length is *decremented* to
// ensure that each array element is always being managed (during an allocation
// attempt) either by the proctor or the array itself, but not both.
//..
//     0     1     2     3     4     5     6     7
//   _____ _____ _____ _____ _____ _____ _____ _____
//  | "A" | "B" | "C" | "D" |xxxxx|xxxxx| "E" |xxxxx|
//  `=====^=====^=====^=====^=====^=====^=====^====='
//  my_Array                            ^     ^ bslalg_AutoArrayDestructor::end
//  (length = 4)                        `---- bslalg_AutoArrayDestructor::begin
//
//              Figure: Configuration after shifting up one element
//..
// After the required number of elements have been shifted, the hole is filled
// (backwards) by copies of the element to be inserted.  The code for the
// templatized 'insert' method is as follows:
//..
    // Assume no aliasing.
    template <class TYPE> inline
    void my_Array<TYPE>::insert(int dstIndex, const TYPE& item, int numItems)
    {
        if (d_length >= d_size) {
            this->increaseSize(numItems);
        }

        int   origLen = d_length;
        TYPE *src     = &d_array_p[d_length];
        TYPE *dest    = &d_array_p[d_length + numItems];
        bslalg_AutoArrayDestructor<TYPE> autoDtor(dest, dest);

        for (int i = d_length; i > dstIndex; --i, --d_length) {
            dest = autoDtor.moveBegin(-1);  // decrement destination
            new(dest) TYPE(*(--src));       // copy to new index
            src->~TYPE();                   // destroy original
        }
        for (int i = numItems; i > 0; --i) {
            dest = autoDtor.moveBegin(-1);  // decrement destination
            new(dest) TYPE(item);           // copy new value into hole
        }
        autoDtor.release();
        d_length = origLen + numItems;
    }
//..
// Note that in the 'insert' example above, we illustrate exception
// neutrality, but not alias safety (i.e., in the case when 'item' is a
// reference into the portion of the array at 'dstIndex' or beyond).

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;

    verbose = argc > 2;
    veryVerbose = argc > 3;
    veryVeryVerbose = argc > 4;

    setbuf(stdout, NULL);    // Use unbuffered output

    printf("TEST " __FILE__ " CASE %d\n", test);

    bslma_TestAllocator testAllocator(veryVeryVerbose);
    Z = &testAllocator;

    switch (test) { case 0:  // Zero is always the leading case.
      case 4: {
        // --------------------------------------------------------------------
        // TESTING USAGE EXAMPLE
        //
        // Concerns: That the usage example compiles and runs as expected.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING USAGE."
                            "\n==============\n");

        if (verbose)
            printf("Testing 'my_Array::insert' and 'my_Array::remove'.\n");

        BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator)
        {
            const char *DATA[] = { "A", "B", "C", "D", "E" };
            const int NUM_ELEM = sizeof DATA / sizeof *DATA;

            my_Array<my_String> mX(Z);    const my_Array<my_String>& X = mX;
            for (int i = 0; i < NUM_ELEM; ++i) {
                my_String s(DATA[i]);
                mX.insert(i, s, 1);
            }

            if (verbose)
                printf("\tInsert two strings \"F\" at index position 2.\n");

            my_String item("F");
            mX.insert(2, item, 2);

            ASSERT(NUM_ELEM + 2 == X.length());
            ASSERT(X[0] == DATA[0]);
            ASSERT(X[1] == DATA[1]);
            ASSERT(X[2] == item);
            ASSERT(X[2] == item);
            for (int i = 4; i < X.length(); ++i) {
                LOOP_ASSERT(i, X[i] == DATA[i - 2]);
            }
        }
        BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING 'release'
        //
        // Concerns:  That the guard does not free guarded memory if 'release'
        //    has been called.
        //
        // Plan:
        //
        // Testing:
        //   void release();
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING 'release'."
                            "\n==================\n");

        const int MAX_SIZE = 16;
        static union {
            char                               d_raw[MAX_SIZE * sizeof(T)];
            bsls_AlignmentUtil::MaxAlignedType d_align;
        } u;
        T *buf = (T*)&u.d_raw[0];

        if (verbose) printf("\tWith release.\n");
        {
            char c = 'a';

            for (int i = 0; i < MAX_SIZE; ++i, ++c) {
                new (&buf[i]) T(c, Z);
                if (veryVerbose) { buf[i].print(); }
            }

            bslalg_AutoArrayDestructor<T> mG(&buf[0], &buf[0] + MAX_SIZE);
            mG.release();
        }
        ASSERT(0 <  testAllocator.numBytesInUse());
        ASSERT(0 == testAllocator.numMismatches());

        if (verbose) printf("\tWithout release.\n");
        {
            bslalg_AutoArrayDestructor<T> mG(&buf[0], &buf[0] + MAX_SIZE);
        }
        ASSERT(0 == testAllocator.numBytesInUse());
        ASSERT(0 == testAllocator.numMismatches());

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING 'class bslalg_AutoArrayDestructor'
        //
        // Concerns:  That the guard frees guarded memory properly upon
        //   exceptions.
        //
        // Plan: After asserting that the interface behaves as intended and
        //   that the guard destruction indeed frees the memory, we set up an
        //   exception test that ensures that the guard indeed correctly guards
        //   a varying portion of an array.
        //
        // Testing:
        //   bslalg_AutoArrayDestructor(T *b, T *e);
        //   ~AutoArrayDestructor();
        //   T *moveBegin(ptrdiff_t offset = -1);
        //   T *moveEnd(ptrdiff_t offset = 1);
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING 'bslalg_AutoArrayDestructor'."
                            "\n=====================================\n");

        const int MAX_SIZE = 16;
        static union {
            char                               d_raw[MAX_SIZE * sizeof(T)];
            bsls_AlignmentUtil::MaxAlignedType d_align;
        } u;
        T *buf = (T*)&u.d_raw[0];

        if (verbose)
            printf("\tSimple interface tests (from breathing test).\n");
        {
            char c = 'a';

            for (int i = 0; i < MAX_SIZE; ++i, ++c) {
                new (&buf[i]) T(c, Z);
                if (veryVerbose) { buf[i].print(); }
            }

            bslalg_AutoArrayDestructor<T> mG(&buf[0], &buf[0]);

            ASSERT(&buf[5] == mG.moveEnd(5));
            ASSERT(&buf[3] == mG.moveBegin(3));

            ASSERT(&buf[2] == mG.moveBegin(-1));
            ASSERT(&buf[4] == mG.moveEnd(-1));

            ASSERT(&buf[0]        == mG.moveBegin(-2));
            ASSERT(&buf[MAX_SIZE] == mG.moveEnd(MAX_SIZE - 4));
        }
        ASSERT(0 == testAllocator.numBytesInUse());
        ASSERT(0 == testAllocator.numMismatches());

        if (verbose)
            printf("\tException test.\n");
        {
            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator)
            {
                bslalg_AutoArrayDestructor<T> mG(&buf[0], &buf[0]);
                const bslalg_AutoArrayDestructor<T>& G = mG;

                char c = 'a';
                for (int i = 0; i < MAX_SIZE; ++i, ++c) {
                    new (&buf[i]) T(c, Z);

                    ASSERT(&buf[i + 1] == mG.moveEnd(1));
                    if (veryVerbose) { buf[i].print(); }
                }
            }
            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
        }
        ASSERT(0 == testAllocator.numBytesInUse());
        ASSERT(0 == testAllocator.numMismatches());

        if(verbose) printf("\nNegative testing constructors\n");
        {
            bsls_AssertFailureHandlerGuard g(bsls_AssertTest::failTestDriver);

            int * null = 0;
            ASSERT_SAFE_PASS(bslalg_AutoArrayDestructor<int>(0, 0));
            int simpleArray[] = { 0, 1, 2, 3, 4 };
            int * begin = simpleArray;
            int * end = begin;
            ASSERT_SAFE_FAIL(bslalg_AutoArrayDestructor<int>(0, begin));
            ASSERT_SAFE_FAIL(bslalg_AutoArrayDestructor<int>(begin, 0));
            ASSERT_SAFE_PASS(bslalg_AutoArrayDestructor<int>(begin, begin));

            ++begin; ++begin;  // Advance begin by two to form an invalid range
            ++end;
            ASSERT_SAFE_FAIL(bslalg_AutoArrayDestructor<int>(begin, end));
            ++end;
            ASSERT(begin == end);
            ASSERT_SAFE_PASS(bslalg_AutoArrayDestructor<int>(begin, end));
            ++end;
            ASSERT_SAFE_PASS(bslalg_AutoArrayDestructor<int>(begin, end));
        }

        if(verbose) printf("\nNegative testing 'moveBegin' and 'moveEnd'\n");
        {
            bsls_AssertFailureHandlerGuard g(bsls_AssertTest::failTestDriver);

            int * null = 0;
            bslalg_AutoArrayDestructor<int> emptyGuard(0, 0);
            ASSERT_SAFE_PASS(emptyGuard.moveBegin(0));
            ASSERT_SAFE_PASS(emptyGuard.moveEnd(0));
            ASSERT_SAFE_FAIL(emptyGuard.moveBegin());
            ASSERT_SAFE_FAIL(emptyGuard.moveEnd());

            int simpleArray[] = { 0, 1, 2, 3, 4 };
            int * begin = &simpleArray[2];
            bslalg_AutoArrayDestructor<int> intGuard(begin, begin);
            ASSERT_SAFE_PASS(intGuard.moveBegin(0));
            ASSERT_SAFE_PASS(intGuard.moveEnd(0));

            ASSERT_SAFE_FAIL(intGuard.moveBegin(1));
            ASSERT_SAFE_FAIL(intGuard.moveEnd(-1));

            ASSERT_SAFE_PASS(intGuard.moveBegin());
            ASSERT_SAFE_PASS(intGuard.moveEnd(-1));

            ASSERT_SAFE_PASS(intGuard.moveEnd());
            ASSERT_SAFE_PASS(intGuard.moveBegin(1));
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        //   This test exercises the component but tests nothing.
        // --------------------------------------------------------------------

        if (verbose) printf("\nBREATHING TEST"
                            "\n==============\n");

        if (verbose) printf("\nclass bslalg_AutoArrayDestructor"
                            "\n--------------------------------\n");
        {
            const int MAX_SIZE = 16;
            static union {
                char                               d_raw[MAX_SIZE * sizeof(T)];
                bsls_AlignmentUtil::MaxAlignedType d_align;
            } u;
            T *buf = (T*)&u.d_raw[0];

            char c = 'a';
            for (int i = 0; i < MAX_SIZE; ++i, ++c) {
                new (&buf[i]) T(c, Z);
                if (veryVerbose) { buf[i].print(); }
            }

            bslalg_AutoArrayDestructor<T> mG(&buf[0], &buf[0]);

            ASSERT(&buf[5] == mG.moveEnd(5));
            ASSERT(&buf[3] == mG.moveBegin(3));

            ASSERT(&buf[2] == mG.moveBegin(-1));
            ASSERT(&buf[6] == mG.moveEnd(1));

            ASSERT(&buf[0]        == mG.moveBegin(-2));
            ASSERT(&buf[MAX_SIZE] == mG.moveEnd(MAX_SIZE - 6));
        }  // deallocates buf
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
//      Copyright (C) Bloomberg L.P., 2008
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
