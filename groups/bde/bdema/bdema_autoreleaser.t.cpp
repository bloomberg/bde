// bdema_autoreleaser.t.cpp  -*-C++-*-

#include <bdema_autoreleaser.h>

#include <bsl_cstdlib.h>     // atoi()
#include <bsl_cstring.h>     // memcpy()
#include <bsl_iostream.h>

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              OVERVIEW
//
// The goals of this 'bdema_AutoReleaser' test suite are to verify that 1) the
// 'release' method of the managed allocator (pool) is invoked at the proctor's
// destruction; 2) the 'release' method of the managed allocator (pool) is not
// invoked at destruction if the proctor's 'release' method has been invoked;
// and 3) 'reset' correctly resets the managed allocator (pool) and allows
// reuse of the proctor.
//
// To aid testing, a 'TestAllocator' object is provided, which implements a
// 'release' method that sets an internal flag when the method is called.
//
// To achieve goal 1, create a 'bdema_AutoReleaser' proctor initialized with a
// 'TestAllocator'.  Allow the proctor to go out of scope and verify that the
// test allocator indicates its 'release' method has been invoked.  To achieve
// goal 2, repeat the above procedure, but invoke the proctor's 'release'
// method before the proctor goes out of scope.  Verify that the test allocator
// indicates its 'release' method has *not* been invoked.  To achieve goal 3,
// create a proctor initialized with test allocator 'a1'.  Invoke the proctor's
// 'release' method, and then invoke the proctor's 'reset' method with a second
// test allocator 'a2'.  Allow the proctor to go out of scope and verify that
// the 'release' method has been invoked only for 'a2' but not for 'a1'.
//-----------------------------------------------------------------------------
// [2] bdema_AutoReleaser<ALLOCATOR>(originalAllocator);
// [2] ~bdema_AutoReleaser<ALLOCATOR>();
// [2] void release();
// [2] void reset(newAllocator);
//-----------------------------------------------------------------------------
// [3] USAGE EXAMPLE
// [1] Ensure TestAllocator works as expected.
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

//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------

#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", " << flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number

//=============================================================================
//                          HELPER CLASS FOR TESTING
//-----------------------------------------------------------------------------

class TestAllocator {
    int d_isReleaseCalled; // set if 'release' has bee called

  public:
    TestAllocator() : d_isReleaseCalled(0) {}
        // Create this object.

    ~TestAllocator() {}
        // Destroy this object.

    void release() { d_isReleaseCalled = 1; }
        // Set an internal flag to indicate this method has been called.

    bool isReleaseCalled() const { return d_isReleaseCalled; }
        // Return 'true' if 'release' has been called on this object, and
        // 'false' otherwise.
};

//=============================================================================
//                                USAGE EXAMPLE
//-----------------------------------------------------------------------------

class my_BlockList {
    // This class implements a memory manager that allocates and manages a
    // sequence of memory blocks, each potentially of a different size as
    // specified in the 'allocate' method's invocation.  This object's
    // 'release' method deallocates the entire sequence of memory blocks, as
    // does its destructor.

    struct my_Block {
        my_Block *d_next_p;
        void *d_buffer;

        my_Block(int numBytes) { d_buffer = operator new(numBytes); }
        ~my_Block() { operator delete(d_buffer); }
    };

    my_Block        *d_head_p;

  public:
    my_BlockList() : d_head_p(0) {}
    ~my_BlockList();
    void *allocate(int numBytes);
    void release();
};

my_BlockList::~my_BlockList()
{
    release();
}

void *my_BlockList::allocate(int numBytes)
{
    ASSERT(0 <= numBytes);
    if (0 < numBytes) {
        my_Block *newBlock = new my_Block(numBytes);
        newBlock->d_next_p = d_head_p;
        d_head_p = newBlock;
        return newBlock->d_buffer;
    }
    else {
        return 0;
    }
}

void my_BlockList::release()
{
    while (d_head_p) {
        my_Block *p = d_head_p;
        d_head_p = d_head_p->d_next_p;
        delete(p);
    }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// my_strpool.h

class my_StrPool {
    // This class implements a fast memory manager that allocates and manages
    // *unaligned* memory of varying sizes.  This memory manager internally
    // requests relatively large memory blocks, and distributes memory
    // piecemeal from each memory block on demand.  The 'release' method
    // releases all memory managed by this memory manager, as does the
    // destructor.  Note, however, that no facility is provided for
    // deallocating individually allocated blocks of memory.

    int          d_blockSize;
    char        *d_block_p;
    int          d_cursor;
    my_BlockList d_blockList;

  private:
    void *allocateBlock(int numBytes);

  private: // not implemented
    my_StrPool(const my_StrPool&);
    my_StrPool& operator=(const my_StrPool&);

  public:
    my_StrPool();
    ~my_StrPool();
    void *allocate(int numBytes);
    void release();
};

inline
void *my_StrPool::allocate(int numBytes)
{
    if (numBytes <= 0) {
        return 0;
    }
    if (d_block_p && numBytes + d_cursor <= d_blockSize) {
        char *p = d_block_p + d_cursor;
        d_cursor += numBytes;
        return p;
    }
    else {
        return allocateBlock(numBytes);
    }
}

inline
void my_StrPool::release()
{
    d_blockList.release();
    d_block_p = 0;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// my_strpool.cpp

enum {
    INITIAL_SIZE = 128,
    GROW_FACTOR  = 2,
    THRESHOLD    = 128
};

void *my_StrPool::allocateBlock(int numBytes)
{
    ASSERT(0 < numBytes);
    if (THRESHOLD < numBytes) {
        return d_blockList.allocate(numBytes);
    }
    else {
        if (d_block_p) {
            d_blockSize *= GROW_FACTOR;
        }
        d_block_p = (char *) d_blockList.allocate(d_blockSize);
        d_cursor = numBytes;
        return d_block_p;
    }
}

my_StrPool::my_StrPool()
: d_blockSize(INITIAL_SIZE)
, d_block_p(0)
, d_blockList()
{
}

my_StrPool::~my_StrPool()
{
    ASSERT(INITIAL_SIZE <= d_blockSize);
    ASSERT(d_block_p || 0 <= d_cursor && d_cursor <= d_blockSize);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// my_fastcstrarray.h

class my_FastCstrArray {
    // This class implements an array of C string elements.  Each C string is
    // allocated using the 'my_StrPool' member for fast memory allocation and
    // deallocation.

    char         **d_array_p; // dynamically allocated array
    int            d_size;    // physical capacity of this array
    int            d_length;  // logical length of this array
    my_StrPool     d_strPool; // memory manager to supply memory

  private:
    void increaseSize();

  public:
    my_FastCstrArray();
    ~my_FastCstrArray();

    void append(const char *item);
    my_FastCstrArray& operator=(const my_FastCstrArray& original);
    const char *operator[](int ind) const { return d_array_p[ind];}
    int length() const { return d_length; }
};

// FREE OPERATORS
ostream& operator<<(ostream& stream, const my_FastCstrArray& array);

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// my_fastcstrarray.cpp

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
void reallocate(char ***array, int *size,
                int newSize, int length)
    // Reallocate memory in the specified 'array' and update the
    // specified size to the specified 'newSize'.  The specified 'length'
    // number of leading elements are preserved.  If 'new' should throw
    // an exception, this function has no effect.  The behavior is
    // undefined unless 1 <= newSize, 0 <= length, and newSize <= length.
{
    ASSERT(array);
    ASSERT(*array);
    ASSERT(size);
    ASSERT(1 <= newSize);
    ASSERT(0 <= length);
    ASSERT(length <= *size);    // sanity check
    ASSERT(length <= newSize);  // ensure class invariant

    char **tmp = *array;

    *array = (char **) operator new(newSize * sizeof **array);
    // COMMIT
    memcpy(*array, tmp, length * sizeof **array);
    *size = newSize;
    operator delete(tmp);
}

void my_FastCstrArray::increaseSize()
{
    reallocate(&d_array_p, &d_size, nextSize(d_size), d_length);
}

my_FastCstrArray::my_FastCstrArray()
: d_size(MY_INITIAL_SIZE)
, d_length(0)
{
    d_array_p = (char **) operator new(d_size * sizeof *d_array_p);
}

my_FastCstrArray::~my_FastCstrArray()
{
    ASSERT(1 <= d_size);
    ASSERT(0 <= d_length);
    ASSERT(d_length <= d_size);
    operator delete(d_array_p);
}

void my_FastCstrArray::append(const char *item)
{
    if (d_length >= d_size) {
        this->increaseSize();
    }
    int sSize = strlen(item) + 1;
    char *elem = (char *) d_strPool.allocate(sSize);
    memcpy(elem, item, sSize * sizeof *item);
    d_array_p[d_length++] = elem;
}

my_FastCstrArray& my_FastCstrArray::operator=(const my_FastCstrArray& original)
{
    if (&original != this) {
        d_strPool.release();
        d_length = 0;
        if (original.d_length > d_size) {
            char **tmp = d_array_p;
            d_array_p =
                (char **) operator new(original.d_length * sizeof *d_array_p);
            d_size = original.d_length;
            operator delete(tmp);
        }
        bdema_AutoReleaser<my_StrPool> autoReleaser(&d_strPool);
        for (int i = 0; i < original.d_length; ++i) {
            int size = strlen(original.d_array_p[i]) + 1;
            d_array_p[i] = (char *) d_strPool.allocate(size);
            memcpy(d_array_p[i], original.d_array_p[i], size);
        }
        d_length = original.d_length;
        autoReleaser.release();
    }
    return *this;
}

ostream& operator<<(ostream& stream, const my_FastCstrArray& array)
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

int main(int argc, char *argv[]) {

    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:
      case 3: {
        // --------------------------------------------------------------------
        // USAGE TEST
        //   Create a 'my_FastCstrArray' and append varying strings to it.
        //   Verify that the strings has been correctly appended using
        //   'operator[]'.  Create a second 'my_FastCstrArray' and assign the
        //   first array to it.  Verify that the two arrays have the same
        //   contents by comparing the respective elements in each array.
        //
        // Testing:
        //   USAGE TEST - Make sure main usage example compiles and works.
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "USAGE TEST" << endl
                                  << "==========" << endl;

        if (verbose) cout << "Testing 'my_FastCstrArray'." << endl;
        {
            const char *DATA[] = { "A", "B", "C", "D", "E" };
            const int NUM_ELEM = sizeof DATA / sizeof *DATA;

            my_FastCstrArray mX;    const my_FastCstrArray& X = mX;
            for (int i = 0; i < NUM_ELEM; ++i) {
                mX.append(DATA[i]);
                for (int j = 0; j < i; ++j) {
                    LOOP2_ASSERT(i, j, 0 == strcmp(DATA[j], X[j]));
                }
            }
            ASSERT(NUM_ELEM == X.length());
            if (veryVerbose) { cout << '\t'; P(X); }

            my_FastCstrArray mY;    const my_FastCstrArray& Y = mY;
            mY = X;
            if (veryVerbose) { cout << '\t'; P(Y); }
            ASSERT(X.length() == Y.length());
            for (int k = 0; k < X.length(); ++k) {
                LOOP_ASSERT(k, 0 == strcmp(X[k], Y[k]));
            }
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // BASIC TEST
        //   To test the destructor, create a 'bdema_AutoReleaser' proctor
        //   initialized with a 'TestAllocator'.  Allow the proctor to go out
        //   of scope and verify that the test allocator indicates its
        //   'release' method has been invoked.  To test 'release', repeat the
        //   above procedure, but invoke the proctor's 'release' method before
        //   the proctor goes out of scope.  Verify that the test allocator
        //   indicates its 'release' method has *not* been invoked.  To test
        //   'reset', Create a proctor initialized with test allocator 'a1'.
        //   Invoke the proctor's 'release' method, and then invoke the
        //   proctor's 'reset' method with a second test allocator 'a2'.  Allow
        //   the proctor to go out of scope and verify that the 'release'
        //   method has been invoked only for 'a2' but not for 'a1'.
        //
        // Testing:
        //   bdema_AutoReleaser<ALLOCATOR>(originalAllocator);
        //   ~bdema_AutoReleaser<ALLOCATOR>();
        //   void release();
        //   void reset(newAllocator);
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "BASIC TEST" << endl
                                  << "==========" << endl;

        if (verbose) cout << "Testing constructor." << endl;
        {
            TestAllocator a;                   const TestAllocator &A = a;
            {
                const bdema_AutoReleaser<TestAllocator> X(&a);
            }
            ASSERT(A.isReleaseCalled());
        }

        if (verbose) cout << "Testing 'release'." << endl;
        {
            TestAllocator a;                   const TestAllocator &A = a;
            {
                bdema_AutoReleaser<TestAllocator> x(&a);
                x.release();
            }
            ASSERT(!A.isReleaseCalled());
        }

        if (verbose) cout << "Testing 'reset'." << endl;
        {
            TestAllocator a1;                   const TestAllocator &A1 = a1;
            TestAllocator a2;                   const TestAllocator &A2 = a2;
            {
                bdema_AutoReleaser<TestAllocator> x(&a1);
                x.release();
                x.reset(&a2);
            }
            ASSERT(!A1.isReleaseCalled());
            ASSERT(A2.isReleaseCalled());
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // HELPER CLASS TEST
        //   Create a 'TestAllocator' object and call its 'release' method.
        //   Verify that 'isReleaseCalled' indicates the proper state of the
        //   'TestAllocator' object before and after 'release' is called.
        //
        // Testing:
        //   TestAllocator();
        //   ~TestAllocator();
        //   void release();
        //   bool isReleaseCalled();
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "HELPER CLASS TEST" << endl
                                  << "=================" << endl;

        if (verbose) cout << "Testing 'TestAllocator'." << endl;

        TestAllocator mX;    const TestAllocator &X = mX;
        ASSERT(!X.isReleaseCalled());
        mX.release();
        ASSERT(X.isReleaseCalled());
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
