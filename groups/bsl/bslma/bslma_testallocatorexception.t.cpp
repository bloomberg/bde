// bslma_testallocatorexception.t.cpp                                 -*-C++-*-

#include <bslma_testallocatorexception.h>

#include <bslma_allocator.h>

#include <cstring>     // memset()
#include <cstdlib>     // atoi()
#include <iostream>

using namespace BloombergLP;
using namespace std;

//=============================================================================
//                                  TEST PLAN
//-----------------------------------------------------------------------------
//                                  Overview
//
// We are testing a simple exception object that contains attributes
// initialized by its constructor.  We exercise both the constructor and the
// accessors for the attributes by creating objects initialized with varying
// values, and ensure that the accessors return the expected values.
//-----------------------------------------------------------------------------
// [1] bslma::Testallocatorexception(int numBytes);
// [1] ~bslma::Testallocatorexception();
// [1] int numBytes() const;
//-----------------------------------------------------------------------------
// [2] USAGE TEST - Make sure main usage example compiles and works.
//=============================================================================
//                    STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------

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
    if (!(X)) { cout << #I << ": " << I << "\n"; aSsErT(1, #X, __LINE__);}}

#define LOOP2_ASSERT(I,J,X) { \
    if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " \
              << J << "\n"; aSsErT(1, #X, __LINE__); } }

//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------

#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define Q_(X) cout << "<| " #X " |>" << flush;  // Q(X) without '\n'
#define P_(X) cout << #X " = " << (X) << ", " << flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number

//=============================================================================
//                                USAGE EXAMPLE
//-----------------------------------------------------------------------------
// my_allocator.h

class my_Allocator : public bslma::Allocator {
    int d_allocationLimit;
    // ...

  private:
    // NOT IMPLEMENTED
    my_Allocator(const my_Allocator&);
    my_Allocator& operator=(const my_Allocator&);

  public:
    // CREATORS
    my_Allocator() : d_allocationLimit(-1) {}
    ~my_Allocator() {}
    void *allocate(size_type size);
    void deallocate(void *address) { free(address); }
    void setAllocationLimit(int limit) { d_allocationLimit = limit; }
    int allocationLimit() const { return d_allocationLimit; }
    // ...
};

// my_allocator.cpp
//  #include <my_allocator.h>

void *my_Allocator::allocate(size_type size)
{
#ifdef BDE_BUILD_TARGET_EXC
    if (0 <= d_allocationLimit) {
        --d_allocationLimit;
        if (0 > d_allocationLimit) {
            throw bslma::TestAllocatorException(size);
        }
    }
#endif
    return (void *)malloc(size);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// my_shortarray.h

class my_ShortArray {
    short *d_array_p; // dynamically-allocated array of short integers
    int d_size;       // physical size of the 'd_array_p' array (elements)
    int d_length;     // logical length of the 'd_array_p' array (elements)
    bslma::Allocator *d_allocator_p; // holds (but does not own) allocator

  private:
    void increaseSize(); // Increase the capacity by at least one element.

  public:
    // CREATORS
    my_ShortArray(bslma::Allocator *basicAllocator = 0);
        // Create a empty array.  Optionally specify a 'basicAllocator'
        // used to supply memory.  If 'basicAllocator' is 0, global
        // operators 'new' and 'delete' are used.
     // ...

    ~my_ShortArray();
    void append(int value);
    const short& operator[](int index) const { return d_array_p[index]; }
    int length() const { return d_length; }
    operator const short *() const { return d_array_p; }
};

enum { INITIAL_SIZE = 1, GROW_FACTOR = 2 };

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// my_shortarray.cpp

my_ShortArray::my_ShortArray(bslma::Allocator *basicAllocator)
: d_size(INITIAL_SIZE)
, d_length(0)
, d_allocator_p(basicAllocator)
{
    int sz = d_size * sizeof *d_array_p;
    if (basicAllocator) {
        d_array_p = (short *) d_allocator_p->allocate(sz);
    }
    else {
        d_array_p = (short *) operator new(sz);
    }
}

my_ShortArray::~my_ShortArray()
{
    // CLASS INVARIANTS
    ASSERT(d_array_p);
    ASSERT(0 <= d_size);
    ASSERT(0 <= d_length); ASSERT(d_length <= d_size);

    if (d_allocator_p) {
        d_allocator_p->deallocate(d_array_p);
    }
    else {
        operator delete(d_array_p);
    }
}

inline void my_ShortArray::append(int value)
{
    if (d_length >= d_size) {
        increaseSize();
    }
    d_array_p[d_length++] = value;
}

inline static
void reallocate(short            **array, 
                int                newSize, 
                int                length,
                bslma::Allocator  *basicAllocator)
    // Reallocate memory in the specified 'array' to the specified 'newSize'
    // using the specified 'basicAllocator', or, if 'basicAllocator' is 0,
    // global operators 'new' and 'delete'.  The specified 'length' number of
    // leading elements are preserved.  Since the class invariant requires
    // that the physical capacity of the container may grow but never shrink;
    // the behavior is undefined unless length <= newSize.
{
    ASSERT(array);
    ASSERT(1 <= newSize);
    ASSERT(0 <= length);
    ASSERT(length <= newSize);          // enforce class invariant

    short *tmp = *array;                // support exception neutrality
    int sz = newSize * sizeof **array;
    if (basicAllocator) {
        *array = (short *) basicAllocator->allocate(sz);
    }
    else {
        *array = (short *) operator new(sz);
    }

    // COMMIT POINT

    memcpy(*array, tmp, length * sizeof **array);

    if (basicAllocator) {
        basicAllocator->deallocate(tmp);
    }
    else {
        operator delete(tmp);
    }
}

void my_ShortArray::increaseSize()
{
     int proposedNewSize = d_size * GROW_FACTOR;      // reallocate can throw
     ASSERT(proposedNewSize > d_length);
     reallocate(&d_array_p, proposedNewSize, d_length, d_allocator_p);
     d_size = proposedNewSize;                        // we're committed
}

ostream& operator<<(ostream& stream, const my_ShortArray& array)
{
    stream << '[';
    const int len = array.length();
    for (int i = 0; i < len; ++i) {
        stream << ' ' << array[i];
    }
    return stream << " ]" << flush;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// my_shortarray.t.cpp

#ifdef BDE_BUILD_TARGET_EXC
#define BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN {                          \
    {                                                                       \
        static int firstTime = 1;                                           \
        if (veryVerbose && firstTime) cout <<                               \
            "### BSLMA EXCEPTION TEST -- (ENABLED) --" << endl;             \
        firstTime = 0;                                                      \
    }                                                                       \
    if (veryVeryVerbose) cout <<                                            \
        "### Begin bslma exception test." << endl;                          \
    int bslmaExceptionCounter = 0;                                          \
    static int bslmaExceptionLimit = 100;                                   \
    testAllocator.setAllocationLimit(bslmaExceptionCounter);                \
    do {                                                                    \
        try {

#define BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END                              \
        } catch (bslma::TestAllocatorException& e) {                        \
            if ((veryVerbose && bslmaExceptionLimit) || veryVeryVerbose) {  \
                --bslmaExceptionLimit;                                      \
                cout << "(*** " << bslmaExceptionCounter << ')';            \
                if (veryVeryVerbose) { cout << " BEDMA_EXCEPTION: "         \
                    << "alloc limit = " << bslmaExceptionCounter << ", "    \
                    << "last alloc size = " << e.numBytes();                \
                }                                                           \
                else if (0 == bslmaExceptionLimit) {                        \
                    cout << " [ Note: 'bslmaExceptionLimit' reached. ]";    \
                }                                                           \
                cout << endl;                                               \
            }                                                               \
            testAllocator.setAllocationLimit(++bslmaExceptionCounter);      \
            continue;                                                       \
        }                                                                   \
        testAllocator.setAllocationLimit(-1);                               \
        break;                                                              \
    } while (1);                                                            \
    if (veryVeryVerbose) cout <<                                            \
        "### End bslma exception test." << endl;                            \
}
#else
#define BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN                            \
{                                                                           \
    static int firstTime = 1;                                               \
    if (verbose && firstTime) { cout <<                                     \
        "### BSLMA EXCEPTION TEST -- (NOT ENABLED) --" << endl;             \
        firstTime = 0;                                                      \
    }                                                                       \
}
#define BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
#endif

typedef short Element;
const Element VALUES[] = { 1, 2, 3, 4, -5 };
const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;

const Element &V0 = VALUES[0], &VA = V0,
              &V1 = VALUES[1], &VB = V1,
              &V2 = VALUES[2], &VC = V2,
              &V3 = VALUES[3], &VD = V3,
              &V4 = VALUES[4], &VE = V4;

static bool areEqual(const short *array1, const short *array2, int numElement)
    // Return 'true' if the specified initial 'numElement' in the specified
    // 'array1' and 'array2' have the same values, and 'false' otherwise.
{
    for (int i = 0; i < numElement; ++i) {
        if (array1[i] != array2[i]) return false;
    }
    return true;
}

//=============================================================================
//                                MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[]) {
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;
    int veryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    my_Allocator testAllocator;

    switch (test) { case 0:
      case 2: {
        // --------------------------------------------------------------------
        // USAGE TEST
        //   Verify that the usage example is free of syntax errors and works
        //   works as advertised.
        //
        // Testing:
        //   Make sure main usage example compiles and works.
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "USAGE TEST" << endl
                                  << "==========" << endl;

        struct {
            int d_line;
            int d_numElem;
            short d_exp[NUM_VALUES];
        } DATA[] = {
            { L_, 0, { 0 } },
            { L_, 1, { V0 } },
            { L_, 5, { V0, V1, V2, V3, V4 } }
        };

        const int NUM_TEST = sizeof DATA / sizeof *DATA;

        for (int ti = 0; ti < NUM_TEST; ++ti) {
            const int    LINE     = DATA[ti].d_line;
            const int    NUM_ELEM = DATA[ti].d_numElem;
            const short *EXP      = DATA[ti].d_exp;

            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN {
                my_ShortArray mA(&testAllocator);  const my_ShortArray& A = mA;
                for (int ei = 0; ei < NUM_ELEM; ++ei) {
                    mA.append(VALUES[ei]);
                }
                if (veryVerbose) { P_(ti); P_(NUM_ELEM); P(A); }
                LOOP2_ASSERT(LINE, ti, areEqual(EXP, A, NUM_ELEM));
            } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BASIC TEST
        //   Create 'bslma::TestAllocatorException' objects with varying
        //   initial value.  Verify that each object contains the expected
        //   value using basic accessor 'numBytes'.
        //
        // Testing:
        //   bslma::Testallocatorexception(int numBytes);
        //   ~bslma::Testallocatorexception();
        //   int numBytes() const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "BASIC TEST" << endl
                                  << "==========" << endl;

        if (verbose) cout << "Testing ctor and accessor." << endl;
        {
            const int NUM_BYTES[] = { 0, 1, 5, 100, -1, -100 };
            const int NUM_TEST = sizeof NUM_BYTES / sizeof *NUM_BYTES;
            for (int i = 0; i < NUM_TEST; ++i) {
                const bslma::TestAllocatorException X(NUM_BYTES[i]);
                LOOP_ASSERT(i, NUM_BYTES[i] == X.numBytes());
            }
        }
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

// ----------------------------------------------------------------------------
// Copyright (C) 2012 Bloomberg L.P.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
// ----------------------------- END-OF-FILE ----------------------------------
