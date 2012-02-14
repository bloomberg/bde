// bdema_strallocator.t.cpp  -*-C++-*-

#include <bdema_strallocator.h>
#include <bslma_allocator.h>                  // for testing only
#include <bslma_default.h>                    // for testing only
#include <bslma_deleterhelper.h>              // for testing only
#include <bslma_testallocator.h>              // for testing only

#include <bsl_cstdlib.h>     // atoi()
#include <bsl_cstring.h>     // memcpy()
#include <bsl_iostream.h>

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

//=============================================================================
//                                  TEST PLAN
//-----------------------------------------------------------------------------
//                                  Overview
//                                  --------
// The goals of this test suite are to verify that 1) 'bdema_StrAllocator'
// correctly proxies memory requests (except deallocation) to its
// 'bdema_StrPool' member; and 2) 'deallocate' method does *not* deallocate any
// memory.
//
// To achieve goal 1, create a string allocator and a string pool, and supply
// each with its own instance of test allocator.  Request memory of varying
// sizes from both the string allocator and the string pool.  Verify that the
// test allocators contains the same number of bytes in use and the same total
// number of bytes requested.
//
// To achieve goal 2, create a string allocator supplied with a test allocator.
// Request memory of varying sizes and then deallocate each memory.  Verify
// that the number of bytes in use indicated by the test allocator does not
// decrease after each 'deallocate' method invocation.
//-----------------------------------------------------------------------------
// [1] bdema_StrAllocator(basicAllocator);
// [1] ~bdema_StrAllocator();
// [1] void *allocate(numBytes);
// [2] void deallocate(address);
// [1] void release();
// [3] void reserveCapacity(numBytes);
//-----------------------------------------------------------------------------
// [4] USAGE EXAMPLE
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

//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------

#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", " << flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number
#define TAB cout << '\t';

//=============================================================================
//                                USAGE EXAMPLE
//-----------------------------------------------------------------------------
// my_string.h

class my_String {
    // This is a simple implementation of a string object.

    char            *d_string_p;
    int              d_length;
    int              d_size;
    bslma_Allocator *d_allocator_p;

  public:
    my_String(const char *string, bslma_Allocator *basicAllocator = 0);
    my_String(const my_String& original, bslma_Allocator *basicAllocator = 0);
    ~my_String();

    int length() const            { return d_length;   }
    operator const char *() const { return d_string_p; }
};

// FREE OPERATORS
inline bool operator==(const my_String& lhs, const char *rhs)
{
    return strcmp(lhs, rhs) == 0;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// my_string.cpp

my_String::my_String(const char *string, bslma_Allocator *basicAllocator)
: d_length(strlen(string))
, d_allocator_p(basicAllocator)
{
    ASSERT(string);
    ASSERT(d_allocator_p);
    d_size = d_length + 1;
    d_string_p = (char *) d_allocator_p->allocate(d_size);
    memcpy(d_string_p, string, d_size);
}

my_String::my_String(const my_String& original,
                     bslma_Allocator *basicAllocator)
: d_length(original.d_length)
, d_size(original.d_length + 1)
, d_allocator_p(bslma_Default::allocator(basicAllocator))
{
    ASSERT(d_allocator_p);
    d_string_p = (char *) d_allocator_p->allocate(d_size);
    memcpy(d_string_p, original.d_string_p, d_size);
}

my_String::~my_String()
{
    ASSERT(d_string_p);
    d_allocator_p->deallocate(d_string_p);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// my_strarray.h

class my_StrArray {
    my_String        *d_array_p;        // dynamically allocated array
    int               d_size;           // physical capacity of this array
    int               d_length;         // logical length of this array
    bslma_Allocator  *d_allocator_p;    // supply non-string memory
    bslma_Allocator  *d_strAllocator_p; // supply memory for strings

  private: // not implemented.
    my_StrArray(const my_StrArray& original);

  private:
    void increaseSize();

  public:
    enum Hint { NO_HINT, INFREQUENT_DELETE_HINT };
    my_StrArray(Hint             allocationHint = NO_HINT,
                bslma_Allocator *basicAllocator = 0);
    ~my_StrArray();

    my_StrArray& operator=(const my_StrArray& rhs);
    void append(const char *item);
    const my_String& operator[](int ind) const { return d_array_p[ind]; }
    int length() const                         { return d_length;       }
};

ostream& operator<<(ostream& stream, const my_StrArray& array);

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// my_strarray.cpp

enum {
    MY_INITIAL_SIZE = 1, // initial physical capacity (number of elements)
    MY_GROW_FACTOR = 2   // multiplicative factor by which to grow 'd_size'
};

inline static
int nextSize(int size)
    // Return the specified 'size' multiplied by 'MY_GROW_FACTOR'.
{
    return size * MY_GROW_FACTOR;
}

inline static
void reallocate(my_String       **array,
                int              *size,
                int               newSize,
                int               length,
                bslma_Allocator  *basicAllocator)
    // Reallocate memory in the specified 'array' using the specified
    // 'basicAllocator' and update the specified size to the specified
    // 'newSize'.  The specified 'length' number of leading elements are
    // preserved.  If 'new' should throw an exception, this function has no
    // effect.  The behavior is undefined unless 1 <= newSize, 0 <= length,
    // and newSize <= length.
{
    ASSERT(array);
    ASSERT(size);
    ASSERT(1 <= newSize);
    ASSERT(0 <= length);
    ASSERT(basicAllocator);
    ASSERT(length <= *size);    // sanity check
    ASSERT(length <= newSize);  // ensure class invariant

    my_String *tmp = *array;
    *array = (my_String *) basicAllocator->allocate(newSize * sizeof **array);
    // COMMIT
    memcpy(*array, tmp, length * sizeof **array);
    *size = newSize;

    basicAllocator->deallocate(tmp);
}

void my_StrArray::increaseSize()
{
    reallocate(&d_array_p, &d_size, nextSize(d_size),
               d_length, d_allocator_p);
}

my_StrArray::my_StrArray(my_StrArray::Hint  allocationHint,
                         bslma_Allocator   *basicAllocator)
: d_size(MY_INITIAL_SIZE)
, d_length(0)
, d_allocator_p(bslma_Default::allocator(basicAllocator))
{
    ASSERT(d_allocator_p);

    if (INFREQUENT_DELETE_HINT == allocationHint) {
        d_strAllocator_p =
            new(d_allocator_p->allocate(sizeof(bdema_StrAllocator)))
                bdema_StrAllocator(d_allocator_p);
    }
    else {
        d_strAllocator_p = d_allocator_p;
    }

    d_array_p =
        (my_String *) d_allocator_p->allocate(d_size * sizeof *d_array_p);
}

my_StrArray::~my_StrArray()
{
    ASSERT(1 <= d_size);
    ASSERT(0 <= d_length);
    ASSERT(d_length <= d_size);
    ASSERT(d_allocator_p);
    ASSERT(d_strAllocator_p);

    // If the allocator for string elements is different from the basic
    // allocator, it is a specialized string allocator.  Memory for all string
    // elements will be automatically deallocated when the string allocator is
    // destroyed.
    if (d_strAllocator_p == d_allocator_p) {
        for (int i = 0; i < d_length; ++i) {
            d_array_p[i].~my_String();
        }
    }
    else {
        bslma_DeleterHelper::deleteObject(d_strAllocator_p, d_allocator_p);
    }
    d_allocator_p->deallocate(d_array_p);
}

my_StrArray& my_StrArray::operator=(const my_StrArray& rhs)
{
    if (&rhs != this) {
        if (d_strAllocator_p == d_allocator_p) {
            // Strings using basic allocator.  Destroy each string.
            for (int i = 0; i < d_length; ++i) {
                d_array_p[i].~my_String();
            }
        }
        else {
            // Strings using string allocator.  Release all string memory.
            ((bslma_ManagedAllocator *) d_strAllocator_p)->release();
        }
        d_length = 0;

        if (rhs.d_length > d_size) {
            reallocate(&d_array_p, &d_size, rhs.d_length, d_length,
                       d_allocator_p);
        }

        for (d_length = 0; d_length < rhs.d_length; ++d_length) {
            new(d_array_p + d_length)
                my_String(rhs.d_array_p[d_length], d_strAllocator_p);
        }
    }
    return *this;
}

void my_StrArray::append(const char *item)
{
    if (d_length >= d_size) {
        this->increaseSize();
    }
    new(d_array_p + d_length) my_String(item, d_strAllocator_p);
    ++d_length;
}

ostream& operator<<(ostream& stream, const my_StrArray& array)
{
    stream << "[ ";
    for (int i = 0; i < array.length(); ++i) {
        stream << '"' << array[i] << "\" ";
    }
    return stream << ']' << flush;
}

//=============================================================================
//                                MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;
    int veryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:
      case 4: {
        // --------------------------------------------------------------------
        // USAGE TEST
        //   Create two 'my_StrArray' objects with 'INFREQUENT_DELETE_HINT'
        //   flag.  Append different strings to both arrays and verify the
        //   contents of the arrays using 'operator[]'.  Assign one array to
        //   another and verify the contents of the assigned array.  Allow both
        //   arrays to leave scope and verify that all memory is deallocated.
        //
        // Testing:
        //   Ensure usage example compiles and works.
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "USAGE TEST" << endl
                                  << "==========" << endl;

        if (verbose) cout << "Testing 'my_StrArray'." << endl;

        const char *DATA[] = { "A", "B", "C", "D", "E" };
        const int NUM_ELEM = sizeof DATA / sizeof *DATA;

        bslma_TestAllocator ta(veryVeryVerbose);
        const bslma_TestAllocator& TA = ta;
        bslma_Allocator *const ALLOCATOR[] = { &ta, 0 };
        const int NUM_ALLOCATOR = sizeof ALLOCATOR / sizeof *ALLOCATOR;

        for (int ai = 0; ai < NUM_ALLOCATOR; ++ai) {
            bslma_Allocator *a = ALLOCATOR[ai];

            const my_StrArray::Hint hint = my_StrArray::INFREQUENT_DELETE_HINT;
            my_StrArray mX(hint, a);    const my_StrArray& X = mX;
            my_StrArray mY(hint, a);    const my_StrArray& Y = mY;

            if (verbose) cout << "\t[Append data to 'X']" << endl;
            for (int xi = 0; xi < NUM_ELEM; ++xi) {
                mX.append(DATA[xi]);
                for (int j = 0; j < xi; ++j) {
                    LOOP3_ASSERT(ai, xi, j, 0 == strcmp(DATA[j], X[j]));
                }
            }
            if (veryVerbose) { TAB; P(X); TAB; P(Y); }

            if (verbose) cout << "\n\t[Append data to 'Y']" << endl;
            for (int yi = NUM_ELEM - 1; yi >= 3; --yi) {
                mY.append(DATA[yi]);
                for (int j = 0; j < NUM_ELEM - yi; ++j) {
            //------^
            LOOP3_ASSERT(ai, yi, j, 0 == strcmp(DATA[NUM_ELEM - j - 1], Y[j]));
            //------v
                }
            }
            if (veryVerbose) { TAB; P(X); TAB; P(Y); }

            if (verbose) cout << "\n\t[Assign 'X' to 'Y']" << endl;
            mY = X;
            if (veryVerbose) { TAB; P(X); TAB; P(Y); }

            for (int xj = 0; xj < NUM_ELEM; ++xj) {
                LOOP2_ASSERT(ai, xj, X[xj] == Y[xj]);
            }
        }
        ASSERT(0 == TA.numBytesInUse());
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // RESERVECAPACITY TEST
        //   Create a string allocator and a string pool, and initialize each
        //   with its own instance of test allocator.  Reserve capacity and
        //   request memory of varying sizes from both the string allocator and
        //   the string pool.  Verify that both test allocators contain the
        //   same number of bytes in use and the same total number of bytes
        //   requested.
        //
        // Testing:
        //   void reserveCapacity(numBytes);
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "RESERVECAPACITY TEST" << endl
                                  << "====================" << endl;

        if (verbose) cout << "Testing 'reserveCapacity'." << endl;

        const int DATA[] = { 0, 5, 12, 24, 32, 64, 256, 1000 };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        const int RES_DATA[] = { 0, 2, 4, 8, 16, 32, 64, 128, 256, 512 };
        const int NUM_RES_DATA = sizeof RES_DATA / sizeof *RES_DATA;

        bslma_TestAllocator strAllocatorTA(veryVeryVerbose);
        bslma_TestAllocator strPoolTA(veryVeryVerbose);

        for (int j = 0; j < NUM_RES_DATA; ++j) {
            bdema_StrAllocator sa(&strAllocatorTA);
            bdema_StrPool sp(&strPoolTA);

            sa.reserveCapacity(RES_DATA[j]);
            sp.reserveCapacity(RES_DATA[j]);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int SIZE = DATA[i];
                sa.allocate(SIZE);
                sp.allocate(SIZE);
                LOOP_ASSERT(i, strAllocatorTA.numBytesInUse()
                                                 == strPoolTA.numBytesInUse());
            }

            sa.release();
            sp.release();
            ASSERT(0 == strAllocatorTA.numBytesInUse());
            ASSERT(0 == strPoolTA.numBytesInUse());
            ASSERT(strAllocatorTA.numBytesTotal()
                                                 == strPoolTA.numBytesTotal());
        }

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // DEALLOCATE TEST
        //   Create a string allocator initialized with a test allocator.
        //   Request memory of varying sizes and then deallocate each memory.
        //   Verify that the number of bytes in use indicated by the test
        //   allocator does not decrease after each 'deallocate' method
        //   invocation.
        //
        // Testing:
        //   void deallocate(address);
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "DEALLOCATE TEST" << endl
                                  << "===============" << endl;

        if (verbose) cout << "Testing 'deallocate'." << endl;

        const int DATA[] = { 0, 5, 12, 24, 32, 64, 256, 1000 };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        bslma_TestAllocator ta(veryVeryVerbose);
        bdema_StrAllocator sa(&ta);

        int lastNumBytesInUse = ta.numBytesInUse();

        for (int i = 0; i < NUM_DATA; ++i) {
            const int SIZE = DATA[i];
            void *p = sa.allocate(SIZE);
            const int numBytesInUse = ta.numBytesInUse();
            sa.deallocate(p);
            LOOP_ASSERT(i, numBytesInUse == ta.numBytesInUse());
            LOOP_ASSERT(i, lastNumBytesInUse <= ta.numBytesInUse());
            lastNumBytesInUse = ta.numBytesInUse();
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BASIC TEST
        //   Create a string allocator and a string pool, and initialize each
        //   with its own instance of test allocator.  Request memory of
        //   varying sizes from both the string allocator and the string pool.
        //   Verify that both test allocators contain the same number of bytes
        //   in use and the same total number of bytes requested.
        //
        // Testing:
        //   bdema_StrAllocator(basicAllocator);
        //   ~bdema_StrAllocator();
        //   void *allocate(numBytes);
        //   void release();
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "BASIC TEST" << endl
                                  << "==========" << endl;

        if (verbose) cout << "Testing 'allocate', 'deallocate' and 'release'."
                          << endl;

        const int DATA[] = { 0, 5, 12, 24, 32, 64, 256, 1000 };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        bslma_TestAllocator strAllocatorTA(veryVeryVerbose);
        bslma_TestAllocator strPoolTA(veryVeryVerbose);

        bdema_StrAllocator sa(&strAllocatorTA);
        bdema_StrPool sp(&strPoolTA);

        for (int i = 0; i < NUM_DATA; ++i) {
            const int SIZE = DATA[i];
            sa.allocate(SIZE);
            sp.allocate(SIZE);
            LOOP_ASSERT(i, strAllocatorTA.numBytesInUse()
                           == strPoolTA.numBytesInUse());
        }

        sa.release();
        sp.release();
        ASSERT(0 == strAllocatorTA.numBytesInUse());
        ASSERT(0 == strPoolTA.numBytesInUse());
        ASSERT(strAllocatorTA.numBytesTotal() == strPoolTA.numBytesTotal());

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
