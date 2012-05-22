// bslma_autorawdeleter.t.cpp                                         -*-C++-*-

#include <bslma_autorawdeleter.h>
#include <bslma_allocator.h>   // for testing only

#include <cstdlib>     // atoi()
#include <cstring>     // memcpy()
#include <iostream>

using namespace BloombergLP;
using namespace std;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                             Overview
//                             --------
// We are testing a proctor object to ensure that: 1) it destroys the proper
// sequence of objects (in unspecified order) and, 2) it deallocates the
// correct memory address associated with each object using the allocator it
// holds.
//
// We achieve the first goal by creating objects of a user-defined type that
// are each initialized with the address of a unique counter.  As each object
// is destroyed, its destructor increments the counter held by the object,
// indicating the number of times the object's destructor is called.  We create
// proctors to manage varying sequences (differing in origin and length) of
// such user-defined-type objects in an array.  After each proctor is
// destroyed, we verify that the corresponding counters of the objects managed
// by the proctor are modified.
//
// We achieve the second goal by using the 'TestAllocator' allocator, whose
// 'deallocate' method is instrumented to record all memory addresses with
// which the method is invoked.  We then initialize the proctor object with
// this allocator and verify that when the proctor is destroyed the expected
// memory addresses are recorded in the allocator.  Note that since
// 'TestAllocator' is not derived from 'bslma::Allocator' and does not implement
// an 'allocate' method, we ensure that this proctor works with any 'ALLOCATOR'
// object that supports the required 'deallocate' method.
//-----------------------------------------------------------------------------
// [3] bslma::AutoRawDeleter<TYPE, ALLOCATOR>(origin, allocator, length = 0);
// [3] ~bslma::AutoRawDeleter<TYPE, ALLOCATOR>();
// [3] void operator++();
// [3] void operator--();
// [3] void reset();
// [3] void setLength();
// [3] void length();
// [3] void release();
// [4] explicit test of some or all proctored elements being NULL
//-----------------------------------------------------------------------------
// [1] Ensure helper classes 'my_Class' and 'TestAllocator' work
// [2] Ensure helper functions 'areEqual' and 'countNonZero' work
// [5] USAGE EXAMPLE
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

//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------

#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", " << flush; // P(X) without '\n'
#define PA(X, L) cout << #X " = "; printArray(X, L); cout << endl;
                                              // Print array 'X' of length 'L'
#define PA_(X, L) cout << #X " = "; printArray(X, L); cout << ", " << flush;
                                              // PA(X, L) without '\n'
#define L_ __LINE__                           // current Line number

//=============================================================================
//                    FILE-STATIC FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

static int minInt(int x, int y)
{
    return x < y ? x : y;
}

static bool areEqual(const int *array1, const int *array2, int numElements)
    // Compare the specified initial 'numElements' of the specified integer
    // arrays 'array1' and 'array2'.  Return 'true' if the contents are equal,
    // and 'false' otherwise.  The behavior is undefined unless
    // '0 <= numElements'.
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

static void printArray(const int *array, int numElements)
    // Write the specified initial 'numElements' of the specified 'array' in a
    // single-line format.  The behavior is undefined unless 0 <= numElements.
{
    ASSERT(array);
    ASSERT(0 <= numElements);
    cout << "[ ";
    for (int i = 0; i < numElements; ++i) {
        cout << array[i] << ' ';
    }
    cout << ']';
}

static int countNonZero(const int *array, int numElements)
    // Return the number of non-zero values in the specified initial
    // 'numElements' of the specified 'array'.  The behavior is undefined
    // unless 0 <= numElements.
{
    ASSERT(array);
    ASSERT(0 <= numElements);

    int count = 0;
    for (int i = 0; i < numElements; ++i) {
        count += !!array[i];
    }
    return count;
}

//=============================================================================
//                         HELPER CLASSES FOR TESTING
//-----------------------------------------------------------------------------

class my_Class {
    // This instrumented test object increments its held counter (provided at
    // construction) upon destruction, thus indicating that its destructor has
    // been called.
  private:
    int *d_counter_p; // Counter to be incremented at destruction

  public:
    // CREATORS
    my_Class(int *counter) : d_counter_p(counter) {}
        // Create this object and initialize it with the address of the
        // specified 'counter' to be held.

    ~my_Class() { if (d_counter_p) ++*d_counter_p; }
        // Destroy this object.  Also increment this object's counter if it is
        // not 'null'.
};

class TestAllocator {
    // This allocator does *not* actually allocate/deallocate memory.  It
    // records (in sequence) the memory addresses with which its 'deallocate'
    // method is invoked.  Note that this allocator can only record up to
    // 'MAX_DEALLOCATE_CNT' memory addresses.

    enum { MAX_DEALLOCATE_CNT = 1000 };
        // Maximum number of memory addresses recorded by this allocator.

    void *d_deallocatedMemory[MAX_DEALLOCATE_CNT]; // Deallocated memory addr.
    int   d_numDeallocated; // Number of memory addresses deallocated so far

  public:
    // CREATORS
    TestAllocator();
    ~TestAllocator() {}

    // MANIPULATORS
    void deallocate(void *address);
        // Record the specified 'address' in this allocator.  The order of
        // addresses stored is undefined.

    // ACCESSORS
    void *deallocatedMemory(int index) const;
        // Return the memory address stored at the specified 'index' in the
        // internal array.  The behavior is undefined unless
        // index < MAX_DEALLOCATE_CNT.

    bool isMemoryDeallocated(const void *memory) const;
        // Return 'true' if the specified 'memory' has been recorded by this
        // allocator, and 'false' otherwise.

    int numDeallocated() const { return d_numDeallocated; }
        // Return the number of memory addresses deallocated by this allocator.
};

TestAllocator::TestAllocator()
: d_numDeallocated(0)
{
    for (int i = 0; i < TestAllocator::MAX_DEALLOCATE_CNT; ++i) {
        d_deallocatedMemory[i] = 0;
    }
}

void TestAllocator::deallocate(void *address)
{
    ASSERT(TestAllocator::MAX_DEALLOCATE_CNT > d_numDeallocated);
    d_deallocatedMemory[d_numDeallocated++] = address;
}

bool TestAllocator::isMemoryDeallocated(const void *memory) const
{
    for (int i = 0; i < d_numDeallocated; ++i) {
        if (memory == d_deallocatedMemory[i]) return true;
    }
    return false;
}

//=============================================================================
//                                USAGE EXAMPLE
//-----------------------------------------------------------------------------

// my_autodeallocator.h

template <class ALLOCATOR>
class my_AutoDeallocator {
    // This proctor object automatically deallocates its managed memory
    // (provided at construction) if its 'release' method is not explicitly
    // invoked.

  private:
    void      *d_memory_p;
    ALLOCATOR *d_allocator_p;

  private:
    my_AutoDeallocator(const my_AutoDeallocator<ALLOCATOR>&);
    my_AutoDeallocator<ALLOCATOR>& operator=(
                                         const my_AutoDeallocator<ALLOCATOR>&);

  public:
    // CREATORS
    my_AutoDeallocator(void *memory, ALLOCATOR *originalAllocator)
    : d_memory_p(memory)
    , d_allocator_p(originalAllocator)
    {
    }
    ~my_AutoDeallocator()
    {
        if (d_memory_p) d_allocator_p->deallocate(d_memory_p);
    }

    // MANIPULATORS
    void release()                { d_memory_p = 0;       }
    void reset(void *address) { d_memory_p = address; }
};

// my_array.h

template <class TYPE>
class my_Array2 {
    TYPE            **d_array_p;     // dynamically allocated array
    int               d_size;        // physical capacity of this array
    int               d_length;      // logical length of this array
    bslma::Allocator *d_allocator_p; // holds (but does not own) allocator

  private:

    enum {
        INITIAL_SIZE = 1, // initial physical capacity
        GROW_FACTOR = 2   // multiplicative factor by which to grow
                          // 'd_size'
    };

    static int nextSize(int size);

    static int calculateSufficientSize(int minLength, int size);

    static void reallocate(TYPE             ***array,
                           int                *size,
                           int                 newSize,
                           int                 length,
                           bslma::Allocator   *basicAllocator);

    void increaseSize();

  public:
    my_Array2(bslma::Allocator *basicAllocator = 0);
    ~my_Array2();

    void append(const TYPE& item);
    void insert(int dstIndext, const my_Array2<TYPE>& srcArray);

    int length() const                      { return d_length; }
    const TYPE& operator[](int index) const { return *d_array_p[index];}
};

// FREE OPERATORS
template <class TYPE> inline
ostream& operator<<(ostream& stream, const my_Array2<TYPE>& array);

template <class TYPE> inline
int my_Array2<TYPE>::nextSize(int size)
{
    return size * GROW_FACTOR;
}

template <class TYPE> inline
int my_Array2<TYPE>::calculateSufficientSize(int minLength, int size)
    // Geometrically grow the specified current 'size' value while it is less
    // than the specified 'minLength' value.  Return the new size value.  The
    // behavior is undefined unless 1 <= size and 0 <= minLength.  Note that
    // if minLength <= size then 'size' is returned.
{
    while (size < minLength) {
        size = nextSize(size);
    };
    return size;
}

template <class TYPE> inline
void my_Array2<TYPE>::reallocate(TYPE             ***array,
                                 int                *size,
                                 int                 newSize,
                                 int                 length,
                                 bslma::Allocator   *basicAllocator)
    // Reallocate memory in the specified 'array' using the specified
    // 'basicAllocator' and update the specified size to the specified
    // 'newSize'.  The specified 'length' number of leading elements are
    // preserved.  If 'new' should throw an exception, this function has no
    // effect.  The behavior is undefined unless 1 <= newSize, 0 <= length,
    // and newSize <= length.
{
    TYPE **tmp = *array;
    *array = (TYPE **) basicAllocator->allocate(newSize * sizeof **array);
    memcpy(*array, tmp, length * sizeof **array);
    basicAllocator->deallocate(tmp);
    *size = newSize;
}

template <class TYPE> inline
void my_Array2<TYPE>::increaseSize()
{
    my_Array2<TYPE>::reallocate(&d_array_p, &d_size, nextSize(d_size),
                                d_length, d_allocator_p);
}

// CREATORS
template <class TYPE> inline
my_Array2<TYPE>::my_Array2(bslma::Allocator *basicAllocator)
: d_size(INITIAL_SIZE)
, d_length(0)
, d_allocator_p(basicAllocator)
{
    d_array_p = (TYPE **) d_allocator_p->allocate(d_size * sizeof *d_array_p);
}

template <class TYPE> inline
my_Array2<TYPE>::~my_Array2()
{
    for (int i = 0; i < d_length; ++i) {
        d_array_p[i]->~TYPE();
        d_allocator_p->deallocate(d_array_p[i]);
    }
    d_allocator_p->deallocate(d_array_p);
}

template <class TYPE> inline
void my_Array2<TYPE>::append(const TYPE& item)
{
    if (d_length >= d_size) {
        this->increaseSize();
    }
    TYPE *elem = (TYPE *) d_allocator_p->allocate(sizeof *elem);
    my_AutoDeallocator<bslma::Allocator> autoDeallocator(elem, d_allocator_p);
    new(elem) TYPE(item, d_allocator_p);
    autoDeallocator.release();
    d_array_p[d_length++] = elem;
}

template <class TYPE> inline
void my_Array2<TYPE>::insert(int dstIndex, const my_Array2<TYPE>& srcArray)
{
    int srcLength  = srcArray.d_length;
    int newLength  = d_length + srcLength;
    int numShifted = d_length - dstIndex;

    if (newLength > d_size) {  // Need to resize.
        int newSize = calculateSufficientSize(newLength, d_size);
        TYPE **tmp = d_array_p;
        d_array_p =
            (TYPE **) d_allocator_p->allocate(newSize * sizeof *d_array_p);
        d_size = newSize;

        memcpy(d_array_p, tmp, dstIndex * sizeof *d_array_p);
        memcpy(d_array_p + dstIndex + srcLength,
               tmp + dstIndex, numShifted * sizeof *d_array_p);
        d_allocator_p->deallocate(tmp);
    }
    else {
        memmove(d_array_p + dstIndex + srcLength,
                d_array_p + dstIndex, numShifted * sizeof *d_array_p);
    }

    // Shorten 'd_length' and use auto deleter to proctor tail elements.
    d_length = dstIndex;
    bslma::AutoRawDeleter<TYPE, bslma::Allocator>
                     tailDeleter(d_array_p + dstIndex + srcLength,
                                 d_allocator_p, numShifted);

    // Used to temporarily proctor each new element's memory.
    my_AutoDeallocator<bslma::Allocator>
                     elementDeallocator(0, d_allocator_p);

    if (this == &srcArray) { // self-alias
        // Copy elements up to (but not including) insertion position
        for (int i = 0; i < minInt(dstIndex, srcLength); ++i, ++d_length) {
            d_array_p[dstIndex + i] =
                          (TYPE *) d_allocator_p->allocate(sizeof **d_array_p);
            elementDeallocator.reset(d_array_p[dstIndex + i]);
            new(d_array_p[dstIndex + i])
                                   TYPE(*srcArray.d_array_p[i], d_allocator_p);
            elementDeallocator.release();
        }
        // Copy elements at and beyond insertion position
        for (int j = dstIndex; j < srcLength; ++j, ++d_length) {
            d_array_p[dstIndex + j] =
                          (TYPE *) d_allocator_p->allocate(sizeof **d_array_p);

            elementDeallocator.reset(d_array_p[dstIndex + j]);
            new(d_array_p[dstIndex + j])
                       TYPE(*srcArray.d_array_p[j + srcLength], d_allocator_p);
            elementDeallocator.release();
        }
    } else {
        for (int i = 0; i < srcLength; ++i, ++d_length) {
            d_array_p[dstIndex + i] =
                (TYPE *) d_allocator_p->allocate(sizeof **d_array_p);

            elementDeallocator.reset(d_array_p[dstIndex + i]);
            new(d_array_p[dstIndex + i])
                                   TYPE(*srcArray.d_array_p[i], d_allocator_p);
            elementDeallocator.release();
        }
    }

    tailDeleter.release();
    d_length = newLength;
}

template <class TYPE> inline
ostream& operator<<(ostream& stream, const my_Array2<TYPE>& array)
{
    stream << "[ ";
    for (int i = 0; i < array.length(); ++i) {
        stream << array[i] << " ";
    }
    return stream << ']' << flush;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//           Additional Functionality Needed to Complete Usage Test Case

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// my_mallocfreeallocator.h

class my_MallocFreeAllocator : public bslma::Allocator {
    // This allocator object allocates memory using the global 'malloc'
    // function and deallocates the memory using the global 'free' function.

  private: // not implemented.
    my_MallocFreeAllocator(const my_MallocFreeAllocator&);
    my_MallocFreeAllocator& operator=(const my_MallocFreeAllocator&);

  public:
    // CREATORS
    my_MallocFreeAllocator() {}
    ~my_MallocFreeAllocator() {}
    void *allocate(size_type size) { return (void *) malloc(size); }
    inline void deallocate(void *address) { free(address); }
};

// my_string.h

class my_String {
    // This is a simple implementation of a string object.

    char *d_string_p;
    int d_length;
    int d_size;
    bslma::Allocator *d_allocator_p;

  public:
    my_String(const char *string, bslma::Allocator *basicAllocator);
    my_String(const my_String& original, bslma::Allocator *basicAllocator);
    ~my_String();

    inline int length() const { return d_length; }
    inline operator const char *() const { return d_string_p; }
};

// FREE OPERATORS
inline bool operator==(const my_String& lhs, const char *rhs)
{ return strcmp(lhs, rhs) == 0; }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// my_string.cpp

my_String::my_String(const char *string, bslma::Allocator *basicAllocator)
: d_length(strlen(string))
, d_allocator_p(basicAllocator)
{
    ASSERT(string);
    ASSERT(basicAllocator);
    d_size = d_length + 1;
    d_string_p = (char *) d_allocator_p->allocate(d_size);
    memcpy(d_string_p, string, d_size);
}

my_String::my_String(const my_String&  original,
                     bslma::Allocator *basicAllocator)
: d_length(original.d_length)
, d_size(original.d_length + 1)
, d_allocator_p(basicAllocator)
{
    ASSERT(basicAllocator);
    d_string_p = (char *) d_allocator_p->allocate(d_size);
    memcpy(d_string_p, original.d_string_p, d_size);
}

my_String::~my_String()
{
    ASSERT(d_string_p);
    d_allocator_p->deallocate(d_string_p);
}

//=============================================================================
//                                MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:
      case 5: {
        // --------------------------------------------------------------------
        // USAGE TEST
        //   Create a 'my_Array2' array parameterized with 'my_String' and
        //   initialized with a 'my_MallocFreeAllocator'.  Invoke its 'append'
        //   method with varying values and verify that the resulting array
        //   contains the expected contents.  Create a second 'my_Array2' array
        //   and append varying values to it.  Invoke its 'insert' method with
        //   the first array as the source array.  Verify that the resulting
        //   array contains the expected contents.  Create a third 'my_Array2'
        //   array and append varying values to it.  Invoke its 'insert' method
        //   with itself as the source array.  Verify that the resulting array
        //   contains the expected contents.
        //   Note it is not necessary to independently test all the types used
        //   in this example because the purpose of this test is to verify that
        //   the usage example works, not the correctness of this component.
        //
        // Concerns:
        //   USAGE TEST - Make sure main usage example compiles and works.
        //
        // Plan:
        //   Compile and run the usage test.
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "USAGE TEST" << endl
                                  << "==========" << endl;

        if (verbose) cout << "\nTesting 'my_Array2::append'." << endl;

        const char *DATA[] = { "A", "B", "C", "D", "E" };
        const int NUM_ELEM = sizeof DATA / sizeof *DATA;
        int i;

        my_MallocFreeAllocator a;
        my_Array2<my_String> mX(&a);       const my_Array2<my_String>& X = mX;
        for (i = 0; i < NUM_ELEM; ++i) {
            my_String s(DATA[i], &a);
            mX.append(s);
        }
        if (veryVerbose) { cout << '\t'; P(X); }

        ASSERT(NUM_ELEM == X.length());
        for (i = 0; i < NUM_ELEM; ++i) {
            LOOP_ASSERT(i, X[i] == DATA[i]);
        }

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\nTesting 'my_Array2::insert'." << endl;

        const char *EXP[] = { "A", "B", "A", "B", "C",
                              "D", "E", "C", "D", "E" };
        const int NUM_EXP = sizeof EXP / sizeof *EXP;

        my_Array2<my_String> mY(&a);       const my_Array2<my_String>& Y = mY;
        for (i = 0; i < NUM_ELEM; ++i) {
            my_String s(DATA[i], &a);
            mY.append(s);
        }
        mY.insert(2, mX);    // test insert here
        ASSERT(NUM_EXP == Y.length());
        for (i = 0; i < NUM_EXP; ++i) {
            LOOP_ASSERT(i, Y[i] == EXP[i]);
        }
        if (veryVerbose) { cout << '\t'; P(Y); }

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\nTesting 'my_Array2::insert' self." << endl;
        my_Array2<my_String> mZ(&a);       const my_Array2<my_String>& Z = mZ;
        for (i = 0; i < NUM_ELEM; ++i) {
            my_String s(DATA[i], &a);
            mZ.append(s);
        }
        mZ.insert(2, mZ);    // test insert here
        ASSERT(NUM_EXP == Z.length());
        for (i = 0; i < NUM_EXP; ++i) {
            LOOP_ASSERT(i, Z[i] == EXP[i]);
        }
        if (veryVerbose) { cout << '\t'; P(Z); }
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // EXPLICIT NULL-ELEMENT TEST
        //
        // Concerns:
        //   All of the functionality of this component has already been tested
        //   for the case where all proctored elements address valid objects.
        //   We are now concerned only that everything continues to work as
        //   expected when one or more proctored elements are null pointers.
        //
        // Plan:
        //   We will use the tabulated test vectors from case 3 modified to set
        //   one or more proctored elements to null values.
        //
        //   In particular, we construct an array of "null masks" to "mask out"
        //   corresponding 'my_Class' objects in the proctored array.  The
        //   (non-'release') test block from case 3 is repeated, but with
        //   selected elements of the proctored 'myClassArray' set to 0 as per
        //   the null mask.  The expected ('EXP') values for the integer
        //   counters (which are incremented by 'my_Class' on destruction) are
        //   logically ANDed with the null mask to ensure the proper results in
        //   the 'ASSERT' statements.
        //
        // Testing:
        //   behavior when one or more proctored elements are null
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "NULL-ELEMENT TEST" << endl
                                  << "=================" << endl;

        const int NUM_ELEM = 5;

        struct {
            int d_line;                  // line number
            int d_origin;                // origin of proctor
            int d_proctorLength;         // initial length of proctor
            int d_incDec;                // increment/decrement amount
            int d_expCounters[NUM_ELEM]; // expected array of counters
        } DATA[] = {
            // line     origin    proctor      inc/      expected
            // no.      index     length       dec       counters
            // ----     ------    -------      ----      -----------------
            { L_,        0,         0,         0,       { 0, 0, 0, 0, 0 } },
            { L_,        0,         0,         1,       { 1, 0, 0, 0, 0 } },
            { L_,        0,         0,         3,       { 1, 1, 1, 0, 0 } },
            { L_,        0,         0,         5,       { 1, 1, 1, 1, 1 } },

            { L_,        0,         1,         0,       { 1, 0, 0, 0, 0 } },
            { L_,        0,         1,         1,       { 1, 1, 0, 0, 0 } },
            { L_,        0,         1,         3,       { 1, 1, 1, 1, 0 } },
            { L_,        0,         1,         4,       { 1, 1, 1, 1, 1 } },
            { L_,        0,         1,        -1,       { 0, 0, 0, 0, 0 } },

            { L_,        0,         5,         0,       { 1, 1, 1, 1, 1 } },
            { L_,        0,         5,        -1,       { 1, 1, 1, 1, 0 } },
            { L_,        0,         5,        -3,       { 1, 1, 0, 0, 0 } },
            { L_,        0,         5,        -5,       { 0, 0, 0, 0, 0 } },

            { L_,        1,         0,         0,       { 0, 0, 0, 0, 0 } },
            { L_,        1,         0,         1,       { 0, 1, 0, 0, 0 } },
            { L_,        1,         0,         3,       { 0, 1, 1, 1, 0 } },
            { L_,        1,         0,         4,       { 0, 1, 1, 1, 1 } },
            { L_,        1,         0,        -1,       { 1, 0, 0, 0, 0 } },

            { L_,        1,         1,         0,       { 0, 1, 0, 0, 0 } },
            { L_,        1,         1,         1,       { 0, 1, 1, 0, 0 } },
            { L_,        1,         1,         3,       { 0, 1, 1, 1, 1 } },
            { L_,        1,         1,        -1,       { 0, 0, 0, 0, 0 } },
            { L_,        1,         1,        -2,       { 1, 0, 0, 0, 0 } },

            { L_,        1,         4,         0,       { 0, 1, 1, 1, 1 } },
            { L_,        1,         4,        -1,       { 0, 1, 1, 1, 0 } },
            { L_,        1,         4,        -4,       { 0, 0, 0, 0, 0 } },
            { L_,        1,         4,        -5,       { 1, 0, 0, 0, 0 } },

            { L_,        5,         0,         0,       { 0, 0, 0, 0, 0 } },
            { L_,        5,         0,        -1,       { 0, 0, 0, 0, 1 } },
            { L_,        5,         0,        -3,       { 0, 0, 1, 1, 1 } },
            { L_,        5,         0,        -5,       { 1, 1, 1, 1, 1 } },

            { L_,        5,        -1,         0,       { 0, 0, 0, 0, 1 } },
            { L_,        5,        -1,        -1,       { 0, 0, 0, 1, 1 } },
            { L_,        5,        -1,        -4,       { 1, 1, 1, 1, 1 } },
            { L_,        5,        -1,         1,       { 0, 0, 0, 0, 0 } },

            { L_,        5,        -5,         0,       { 1, 1, 1, 1, 1 } },
            { L_,        5,        -5,         1,       { 0, 1, 1, 1, 1 } },
            { L_,        5,        -5,         3,       { 0, 0, 0, 1, 1 } },
            { L_,        5,        -5,         5,       { 0, 0, 0, 0, 0 } },
        };

        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        if (verbose) cout << "\nTesting behavior with null elements" << endl;

        int nullMask[][NUM_ELEM] = {
            // In each array, the value 0 indicates that the corresponding
            // 'myClassArray' element should be NULL, while the value 1
            // indicates that that element should address a properly
            // constructed object.  A small number of test vectors is
            // sufficient, and the test data selection is a bit arbitrary.
            { 0, 1, 1, 1, 1 },
            { 1, 0, 1, 1, 1 },
            { 1, 1, 0, 1, 1 },
            { 1, 1, 1, 0, 1 },
            { 1, 1, 1, 1, 0 },
            { 0, 1, 1, 1, 0 },
            { 0, 1, 0, 1, 0 },
            { 0, 0, 1, 0, 0 },
            { 0, 0, 0, 0, 0 },
        };

        const int NUM_NULLMASKS = sizeof nullMask / sizeof nullMask[0];

        for (int nm = 0; nm < NUM_NULLMASKS; ++nm) {
            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE   = DATA[i].d_line;
                const int   ORIGIN = DATA[i].d_origin;
                const int   PLEN   = DATA[i].d_proctorLength;
                const int   INCDEC = DATA[i].d_incDec;
                const int  *EXP    = DATA[i].d_expCounters;

                int nmEXP[NUM_ELEM];
                for (int j = 0; j < NUM_ELEM; ++j) {
                    nmEXP[j] = EXP[j] && nullMask[nm][j];
                }

                int counters[NUM_ELEM] = { 0 };          // initialized to 0's
                my_Class *myClassArray[NUM_ELEM] = { 0 };// initialized to 0's
                for (int j = 0; j < NUM_ELEM; ++j) {
                    if (nullMask[nm][j]) {
                        myClassArray[j] = (my_Class *)
                                                    new my_Class(&counters[j]);
                    }
                    LOOP2_ASSERT(nm,j, !!myClassArray[j] == !!nullMask[nm][j]);
                }

                TestAllocator a;
                {
                    typedef bslma::AutoRawDeleter<my_Class, TestAllocator> T;
                    T mX(myClassArray + ORIGIN, &a, PLEN);
                    if (0 < INCDEC) {       // increment
                        for (int ii = 0; ii < INCDEC; ++ii, ++mX);
                    }
                    else if (0 > INCDEC) {  // decrement
                        for (int di = 0; di > INCDEC; --di, --mX);
                    }
                }

                if (veryVerbose) {
                    cout << '\t'; P_(nm); P_(i); PA(counters, NUM_ELEM);
                }
                LOOP3_ASSERT(LINE, nm, i, areEqual(nmEXP, counters,NUM_ELEM));

                const int NUM_DEALLOCATED = countNonZero(nmEXP, NUM_ELEM);
                LOOP3_ASSERT(LINE, nm,i,NUM_DEALLOCATED == a.numDeallocated());

                for (int j = 0; j < NUM_ELEM; ++j) {
                    const void *MEM = myClassArray[j];
                    if (nmEXP[j]) {
                        LOOP4_ASSERT(LINE, nm, i,j,a.isMemoryDeallocated(MEM));
                    }
                    else if (nullMask[nm][j]) {
                        LOOP4_ASSERT(LINE, nm,i,j, 0 != MEM);
                        LOOP4_ASSERT(LINE, nm,i,j,!a.isMemoryDeallocated(MEM));
                    }
                    else {
                        LOOP4_ASSERT(LINE, nm,i,j, 0 == MEM);
                    }
                }

                for (int j = 0; j < NUM_ELEM; ++j) {
                    if (nullMask[nm][j]) {
                        delete myClassArray[j]; // clean up
                    }
                    else {
                        LOOP_ASSERT(LINE, !myClassArray[j]);
                    }
                }
            }
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // CTOR/OPERATORS TEST
        // Concerns:
        //   That the c'tor and d'tor, operators ++ and --, and the 'release'
        //   method work.
        //
        // Plan:
        //   Iterate over a set of table-generated test vectors and perform
        //   independent tests.  For each test, create an array of 'my_Class'
        //   objects and a corresponding array of counters.  Initialize each
        //   element in the array of 'my_Class' objects with the element in the
        //   array of counters at the respective index position.  Create a
        //   'bslma::AutoRawDeleter' proctor initialized with 'd_origin' and
        //   'd_proctorLength' as specified in the test vector to manage a
        //   sequence of 'my_Class' objects.  Also initialize the proctor with
        //   a 'TestAllocator' object to trace memory deallocation.  Increment
        //   or decrement the proctor according to 'd_incDec' in the test
        //   vector.  Destroy the the proctor and verify that the array of
        //   counters contains the expected contents, and that the memory
        //   addresses held by the deleted (by the proctor) 'my_Class' objects
        //   are deallocated and recorded in the allocator.
        //
        // Testing:
        //   bslma::AutoRawDeleter<TYPE, ALLOCATOR>(origin, allocator,
        //                                                         length = 0);
        //   ~bslma::AutoRawDeleter<TYPE, ALLOCATOR>();
        //   void operator++();
        //   void operator--();
        //   void release();
        //   void reset();
        //   void setLength();
        //   int length();
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "CTOR/OPERATORS TEST" << endl
                                  << "===================" << endl;

        const int NUM_ELEM = 5;

        struct {
            int d_line;                  // line number
            int d_origin;                // origin of proctor
            int d_proctorLength;         // initial length of proctor
            int d_incDec;                // increment/decrement amount
            int d_expCounters[NUM_ELEM]; // expected array of counters
        } DATA[] = {
            // line     origin    proctor      inc/      expected
            // no.      index     length       dec       counters
            // ----     ------    -------      ----      -----------------
            { L_,        0,         0,         0,       { 0, 0, 0, 0, 0 } },
            { L_,        0,         0,         1,       { 1, 0, 0, 0, 0 } },
            { L_,        0,         0,         3,       { 1, 1, 1, 0, 0 } },
            { L_,        0,         0,         5,       { 1, 1, 1, 1, 1 } },

            { L_,        0,         1,         0,       { 1, 0, 0, 0, 0 } },
            { L_,        0,         1,         1,       { 1, 1, 0, 0, 0 } },
            { L_,        0,         1,         3,       { 1, 1, 1, 1, 0 } },
            { L_,        0,         1,         4,       { 1, 1, 1, 1, 1 } },
            { L_,        0,         1,        -1,       { 0, 0, 0, 0, 0 } },

            { L_,        0,         5,         0,       { 1, 1, 1, 1, 1 } },
            { L_,        0,         5,        -1,       { 1, 1, 1, 1, 0 } },
            { L_,        0,         5,        -3,       { 1, 1, 0, 0, 0 } },
            { L_,        0,         5,        -5,       { 0, 0, 0, 0, 0 } },

            { L_,        1,         0,         0,       { 0, 0, 0, 0, 0 } },
            { L_,        1,         0,         1,       { 0, 1, 0, 0, 0 } },
            { L_,        1,         0,         3,       { 0, 1, 1, 1, 0 } },
            { L_,        1,         0,         4,       { 0, 1, 1, 1, 1 } },
            { L_,        1,         0,        -1,       { 1, 0, 0, 0, 0 } },

            { L_,        1,         1,         0,       { 0, 1, 0, 0, 0 } },
            { L_,        1,         1,         1,       { 0, 1, 1, 0, 0 } },
            { L_,        1,         1,         3,       { 0, 1, 1, 1, 1 } },
            { L_,        1,         1,        -1,       { 0, 0, 0, 0, 0 } },
            { L_,        1,         1,        -2,       { 1, 0, 0, 0, 0 } },

            { L_,        1,         4,         0,       { 0, 1, 1, 1, 1 } },
            { L_,        1,         4,        -1,       { 0, 1, 1, 1, 0 } },
            { L_,        1,         4,        -4,       { 0, 0, 0, 0, 0 } },
            { L_,        1,         4,        -5,       { 1, 0, 0, 0, 0 } },

            { L_,        5,         0,         0,       { 0, 0, 0, 0, 0 } },
            { L_,        5,         0,        -1,       { 0, 0, 0, 0, 1 } },
            { L_,        5,         0,        -3,       { 0, 0, 1, 1, 1 } },
            { L_,        5,         0,        -5,       { 1, 1, 1, 1, 1 } },

            { L_,        5,        -1,         0,       { 0, 0, 0, 0, 1 } },
            { L_,        5,        -1,        -1,       { 0, 0, 0, 1, 1 } },
            { L_,        5,        -1,        -4,       { 1, 1, 1, 1, 1 } },
            { L_,        5,        -1,         1,       { 0, 0, 0, 0, 0 } },

            { L_,        5,        -5,         0,       { 1, 1, 1, 1, 1 } },
            { L_,        5,        -5,         1,       { 0, 1, 1, 1, 1 } },
            { L_,        5,        -5,         3,       { 0, 0, 0, 1, 1 } },
            { L_,        5,        -5,         5,       { 0, 0, 0, 0, 0 } },
        };

        const int SIZE = sizeof DATA / sizeof *DATA;

        if (verbose)
            cout << "\nTesting ctor, 'operator++' and 'operator--'." << endl;

        for (int i = 0; i < SIZE; ++i) {
            const int   LINE   = DATA[i].d_line;
            const int   ORIGIN = DATA[i].d_origin;
            const int   PLEN   = DATA[i].d_proctorLength;
            const int   INCDEC = DATA[i].d_incDec;
            const int  *EXP    = DATA[i].d_expCounters;
            int j;

            int counters[NUM_ELEM] = { 0 };     // initialized to 0's
            my_Class *myClassArray[NUM_ELEM];
            for (j = 0; j < NUM_ELEM; ++j) {
                myClassArray[j] = (my_Class *) new my_Class(&counters[j]);
            }
            TestAllocator a;
            {
                typedef bslma::AutoRawDeleter<my_Class, TestAllocator> T;
                T mX(myClassArray + ORIGIN, &a, PLEN);
                if (0 < INCDEC) {       // increment
                    for (int ii = 0; ii < INCDEC; ++ii, ++mX);
                }
                else if (0 > INCDEC) {  // decrement
                    for (int di = 0; di > INCDEC; --di, --mX);
                }
            }

            if (veryVerbose) {
                cout << '\t'; P_(i); PA(counters, NUM_ELEM);
            }
            LOOP2_ASSERT(LINE, i, areEqual(EXP, counters, NUM_ELEM));

            const int NUM_DEALLOCATED = countNonZero(EXP, NUM_ELEM);
            LOOP2_ASSERT(LINE, i, NUM_DEALLOCATED == a.numDeallocated());

            for (j = 0; j < NUM_ELEM; ++j) {
                const void *MEM = myClassArray[j];
                if (EXP[j]) {
                    LOOP3_ASSERT(LINE, i, j, a.isMemoryDeallocated(MEM));
                } else {
                    LOOP3_ASSERT(LINE, i, j, !a.isMemoryDeallocated(MEM));
                }
            }

            for (j = 0; j < NUM_ELEM; ++j) {
                delete myClassArray[j]; // clean up
            }
        }

        if (verbose) cout << "\nTesting 'reset', 'setLength', and 'length'\n";

        for (int i = 0; i < SIZE; ++i) {
            const int   LINE   = DATA[i].d_line;
            const int   ORIGIN = DATA[i].d_origin;
            const int   PLEN   = DATA[i].d_proctorLength;
            const int   INCDEC = DATA[i].d_incDec;
            const int  *EXP    = DATA[i].d_expCounters;

            const int   LENGTH = PLEN + INCDEC;

            int counters[NUM_ELEM] = { 0 };     // initialized to 0's
            my_Class *myClassArray[NUM_ELEM];
            for (int j = 0; j < NUM_ELEM; ++j) {
                myClassArray[j] = (my_Class *) new my_Class(&counters[j]);
            }
            TestAllocator a;
            {
                typedef bslma::AutoRawDeleter<my_Class, TestAllocator> T;
                T mX(myClassArray + 100, &a, 12);    // configured for disaster
                mX.reset(myClassArray + ORIGIN);
                mX.setLength(LENGTH);
                LOOP_ASSERT(LINE, LENGTH == mX.length());
            }

            if (veryVerbose) {
                cout << '\t'; P_(i); PA(counters, NUM_ELEM);
            }
            LOOP2_ASSERT(LINE, i, areEqual(EXP, counters, NUM_ELEM));

            const int NUM_DEALLOCATED = countNonZero(EXP, NUM_ELEM);
            LOOP2_ASSERT(LINE, i, NUM_DEALLOCATED == a.numDeallocated());

            for (int j = 0; j < NUM_ELEM; ++j) {
                const void *MEM = myClassArray[j];
                if (EXP[j]) {
                    LOOP3_ASSERT(LINE, i, j, a.isMemoryDeallocated(MEM));
                } else {
                    LOOP3_ASSERT(LINE, i, j, !a.isMemoryDeallocated(MEM));
                }
            }

            for (int j = 0; j < NUM_ELEM; ++j) {
                delete myClassArray[j]; // clean up
            }
        }

        if (verbose) cout << "\nTesting 'release'." << endl;

        for (int i = 0; i < SIZE; ++i) {
            const int   LINE   = DATA[i].d_line;
            const int   ORIGIN = DATA[i].d_origin;
            const int   PLEN   = DATA[i].d_proctorLength;
            const int   INCDEC = DATA[i].d_incDec;
            const static int EXP[NUM_ELEM] = { 0 };    // initialized to 0's
            int j;

            int counters[NUM_ELEM] = { 0 };            // initialized to 0's
            my_Class *myClassArray[NUM_ELEM];
            for (j = 0; j < NUM_ELEM; ++j) {
                myClassArray[j] = (my_Class *) new my_Class(&counters[j]);
            }
            TestAllocator a;
            {
                typedef bslma::AutoRawDeleter<my_Class, TestAllocator> T;
                T mX(myClassArray + ORIGIN, &a, PLEN);
                if (0 < INCDEC) {       // increment
                    for (int ii = 0; ii < INCDEC; ++ii, ++mX);
                }
                else if (0 > INCDEC) {  // decrement
                    for (int di = 0; di > INCDEC; --di, --mX);
                }
                mX.release();
            }

            if (veryVerbose) {
                cout << '\t'; P_(i); PA(counters, NUM_ELEM);
            }
            LOOP2_ASSERT(LINE, i, areEqual(EXP, counters, NUM_ELEM));
            LOOP2_ASSERT(LINE, i, 0 == a.numDeallocated());

            for (j = 0; j < NUM_ELEM; ++j) {
                delete myClassArray[j]; // clean up
            }
        }

        if (verbose) cout << "\nC'tor with 'length' defaulting to 0\n";

        for (int i = 0; i < SIZE; ++i) {
            const int   LINE   = DATA[i].d_line;
            const int   ORIGIN = DATA[i].d_origin;
            const int   PLEN   = DATA[i].d_proctorLength;
            const int   INCDEC = DATA[i].d_incDec;
            const static int EXP[NUM_ELEM] = { 0 };    // initialized to 0's
            int j;

            int counters[NUM_ELEM] = { 0 };            // initialized to 0's
            my_Class *myClassArray[NUM_ELEM];
            for (j = 0; j < NUM_ELEM; ++j) {
                myClassArray[j] = (my_Class *) new my_Class(&counters[j]);
            }
            TestAllocator a;
            {
                typedef bslma::AutoRawDeleter<my_Class, TestAllocator> T;
                T mX(myClassArray + ORIGIN, &a);
                LOOP_ASSERT(LINE, 0 == mX.length());
            }

            if (veryVerbose) {
                cout << '\t'; P_(i); PA(counters, NUM_ELEM);
            }
            LOOP2_ASSERT(LINE, i, areEqual(EXP, counters, NUM_ELEM));
            LOOP2_ASSERT(LINE, i, 0 == a.numDeallocated());

            for (j = 0; j < NUM_ELEM; ++j) {
                delete myClassArray[j]; // clean up
            }
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // FILE-STATIC FUNCTIONS TEST
        //
        // Concerns:
        //   That file-static functions 'areEqual' and 'countNonZero' work.
        //
        // Plan:
        //   For each static function test, iterate over a set of tabulated
        //   test vectors and verify that the function produces the expected
        //   results with the input values specified in the test vectors.
        //
        // Testing:
        //   bool areEqual(array1, array2, len);
        //   int countNonZero(array, len);
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "FILE-STATIC FUNCTIONS TEST" << endl
                                  << "==========================" << endl;

        if (verbose) cout << "\nTesting 'areEqual'." << endl;
        {
            const int SZ = 5;
            struct {
                int d_line;       // line number
                int d_array1[SZ]; // first array argument
                int d_array2[SZ]; // second array argument
                int d_length;     // number of elements to compare
                int d_exp;        // expected result
            } DATA[] = {
      // line   array1               array2               length   result
      // ----   ------------------   ------------------   ------   ------
       { L_,    { 0, 0, 0, 0, 0 },   { 0, 0, 0, 0, 0 },     5,       1  },
       { L_,    { 0, 0, 0, 0, 0 },   { 1, 0, 0, 0, 0 },     5,       0  },
       { L_,    { 0, 0, 0, 0, 0 },   { 1, 2, 3, 4, 5 },     5,       0  },
       { L_,    { 1, 2, 3, 4, 5 },   { 1, 2, 3, 4, 5 },     5,       1  },
       { L_,    { 1, 2, 3, 4, 5 },   { 1, 2, 3, 4, 0 },     5,       0  },
       { L_,    {-1, 2,-3, 4,-5 },   {-1, 2,-3, 4,-5 },     5,       1  },
       { L_,    { 1, 2, 3, 4, 5 },   { 6, 7, 8, 9, 0 },     0,       1  },
       { L_,    { 1, 2, 3, 4, 5 },   { 1, 2, 3, 9, 0 },     3,       1  }
            };

            const int NUM_TEST = sizeof DATA / sizeof *DATA;
            for (int i = 0; i < NUM_TEST; ++i) {
                const int  LINE = DATA[i].d_line;
                const int *A1   = DATA[i].d_array1;
                const int *A2   = DATA[i].d_array2;
                const int  LEN  = DATA[i].d_length;
                const int  EXP  = DATA[i].d_exp;
                if (veryVerbose) {
                    cout << '\t'; P_(i); P_(LEN); PA_(A1, SZ); PA(A2, SZ);
                }
                LOOP2_ASSERT(LINE, i, EXP == areEqual(A1, A2, LEN));
            }
        }

        if (verbose) cout << "\nTesting 'countNonZero'." << endl;
        {
            const int SZ = 100;   // maximum number of elements in an array
            struct {
                int d_line;       // line number
                int d_array[SZ];  // array to count
                int d_length;     // number of elements to count
                int d_exp;        // expected result
            } DATA[] = {
                // line   array1              length   result
                // ----   -----------------   ------   ------
                { L_,    { 0, 0, 0, 0, 0 },     5,       0  },
                { L_,    { 1, 0, 0, 0, 0 },     5,       1  },
                { L_,    { 0, 0, 2, 0, 0 },     5,       1  },
                { L_,    { 0, 0, 0, 0, 3 },     5,       1  },
                { L_,    { 1, 2, 3, 4, 5 },     5,       5  },
                { L_,    {-1, 2,-3, 4,-5 },     5,       5  },
                { L_,    { 1, 2, 3, 4, 5 },     0,       0  },
                { L_,    { 1, 2, 3, 4, 5 },     3,       3  }
            };

            const int NUM_TEST = sizeof DATA / sizeof *DATA;
            for (int i = 0; i < NUM_TEST; ++i) {
                const int  LINE = DATA[i].d_line;
                const int *A    = DATA[i].d_array;
                const int  LEN  = DATA[i].d_length;
                const int  EXP  = DATA[i].d_exp;
                if (veryVerbose) {
                    cout << '\t'; P_(i); P_(LEN); PA(A, LEN);
                }
                LOOP2_ASSERT(LINE, i, EXP == countNonZero(A, LEN));
            }
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // HELPER CLASSES TEST
        //
        // Concerns:
        //   That testing helper classes 'my_Class' and 'TestAllocator' work.
        //
        // Plan:
        //   For testing 'my_Class', create 'my_Class' objects initialized with
        //   a *counter* variable.  Verify that the counter is incremented
        //   after each object is destroyed.
        //   For testing 'TestAllocator', create a 'TestAllocator' object and
        //   call its 'deallocate' method with varying memory addresses.
        //   Verify that 'numDeallocated' method returns the expected number of
        //   times that 'deallocate' has been invoked, and that
        //   'isMemoryDeallocated' method properly indicates the status of
        //   whether a memory address has been deallocated and recorded.
        //
        // Testing:
        //   my_Class::(counter);
        //   my_Class::~my_Class();
        //   TestAllocator::TestAllocator();
        //   TestAllocator::~TestAllocator();
        //   void TestAllocator::deallocate(memory);
        //   int TestAllocator::numDeallocated() const;
        //   int TestAllocator::isMemoryDeallocated(memory) const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "HELPER CLASSES TEST" << endl
                                  << "===================" << endl;

        if (verbose) cout << "\nTesting 'my_Class'." << endl;

        if (verbose) cout << "\tTesting default ctor and dtor." << endl;
        {
            int counter = 0;
            const int NUM_TEST = 5;
            for (int i = 0; i < NUM_TEST; ++i) {
                LOOP_ASSERT(i, counter == i);
                my_Class X(&counter);
            }
            ASSERT(NUM_TEST == counter);
        }

        if (verbose) cout << "\nTesting 'TestAllocator'." << endl;

        const int DATA[] = { 1, 10, 100, -1, -100 };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        TestAllocator mX;    const TestAllocator &X = mX;
        ASSERT(0 == X.numDeallocated());
        for (int di = 0; di < NUM_DATA; ++di) {
            const int NUM_DEALLOCATED = di + 1;
            void *mem = (void *) DATA[di];
            mX.deallocate(mem);

            if (veryVerbose) { cout << '\t'; P(mem); }
            LOOP_ASSERT(di, NUM_DEALLOCATED == X.numDeallocated());
            for (int j = 0; j < NUM_DEALLOCATED; ++j) {
                const void *MEM = (void *) DATA[j];
                LOOP2_ASSERT(di, j, X.isMemoryDeallocated(MEM));
            }
            for (int k = NUM_DEALLOCATED; k < NUM_DATA; ++k) {
                const void *MEM = (void *) DATA[k];
                LOOP2_ASSERT(di, k, !X.isMemoryDeallocated(MEM));
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

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2009
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
