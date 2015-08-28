// bdlma_autoreleaser.t.cpp                                           -*-C++-*-
#include <bdlma_autoreleaser.h>

#include <bslim_testutil.h>

#include <bslma_default.h>

#include <bsl_cstdlib.h>
#include <bsl_cstring.h>
#include <bsl_iostream.h>

using namespace BloombergLP;
using namespace bsl;

// ============================================================================
//                             TEST PLAN
// ----------------------------------------------------------------------------
//                              Overview
//                              --------
// We are testing a proctor object to ensure that, when the proctor object goes
// out of scope, it invokes the 'release' method of the managed allocator or
// pool (if any).  We use a local 'TestAllocator' that has a 'release' method
// instrumented to set an internal flag when it has been called; the flag is
// accessible via a 'wasReleaseCalled' method.  We initialize the
// 'bdlma::AutoReleaser' proctor object with this allocator and verify that
// after the proctor object is destroyed, 'wasReleaseCalled' returns the
// expected result.
// ----------------------------------------------------------------------------
// [ 2] bdlma::AutoReleaser<ALLOCATOR>(ALLOCATOR *originalAllocator);
// [ 2] ~bdlma::AutoReleaser<ALLOCATOR>();
// [ 2] void release();
// [ 2] void reset(ALLOCATOR *newAllocator);
// ----------------------------------------------------------------------------
// [ 3] USAGE EXAMPLE
// [ 1] Ensure local helper class TestAllocator works as expected.

// ============================================================================
//                    STANDARD BDE ASSERT TEST MACRO
// ----------------------------------------------------------------------------

namespace {

int testStatus = 0;

void aSsErT(int c, const char *s, int i)
{
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (0 <= testStatus && testStatus <= 100) ++testStatus;
    }
}

}  // close unnamed namespace

// ============================================================================
//                       STANDARD BDE TEST DRIVER MACROS
// ----------------------------------------------------------------------------

#define ASSERT       BSLIM_TESTUTIL_ASSERT
#define LOOP_ASSERT  BSLIM_TESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLIM_TESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLIM_TESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLIM_TESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLIM_TESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLIM_TESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLIM_TESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLIM_TESTUTIL_LOOP6_ASSERT
#define ASSERTV      BSLIM_TESTUTIL_ASSERTV

#define Q            BSLIM_TESTUTIL_Q   // Quote identifier literally.
#define P            BSLIM_TESTUTIL_P   // Print identifier and value.
#define P_           BSLIM_TESTUTIL_P_  // P(X) without '\n'.
#define T_           BSLIM_TESTUTIL_T_  // Print a tab (w/o newline).
#define L_           BSLIM_TESTUTIL_L_  // current Line number

// ============================================================================
//                  NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT_SAFE_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(EXPR)
#define ASSERT_SAFE_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPR)
#define ASSERT_PASS(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS(EXPR)
#define ASSERT_FAIL(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL(EXPR)
#define ASSERT_OPT_PASS(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS(EXPR)
#define ASSERT_OPT_FAIL(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL(EXPR)

// ============================================================================
//                               TEST APPARATUS
// ----------------------------------------------------------------------------

class TestAllocator {
    // This test class maintains state indicating whether or not the 'release'
    // method has been called.

    // DATA
    bool d_wasReleaseCalled;  // 'true' if 'release' has been called

  public:
    TestAllocator() : d_wasReleaseCalled(false) {}
        // Create a test allocator object.

    ~TestAllocator() {}
        // Destroy this object.

    void release() { d_wasReleaseCalled = true; }
        // Set an internal flag to indicate that this method has been called.

    bool wasReleaseCalled() const { return d_wasReleaseCalled; }
        // Return 'true' if 'release' has been called on this object, and
        // 'false' otherwise.
};

// ============================================================================
//                                USAGE EXAMPLE
// ----------------------------------------------------------------------------

class my_BlockList {
    // This class implements a memory manager that allocates and manages a
    // sequence of memory blocks, each potentially of a different size as
    // specified in the 'allocate' method's invocation.  The 'release' method
    // deallocates the entire sequence of memory blocks, as does the
    // destructor.

    struct my_Block {
        my_Block *d_next_p;
        void     *d_buffer;

        my_Block(int numBytes) { d_buffer = operator new(numBytes); }
        ~my_Block()            { operator delete(d_buffer); }
    };

    my_Block     *d_head_p;

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
        return newBlock->d_buffer;                                    // RETURN
    }
    else {
        return 0;                                                     // RETURN
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
    // releases all memory managed by the memory manager, as does the
    // destructor.  Note, however, that no facility is provided for
    // deallocating individually allocated blocks of memory.

    int           d_blockSize;
    char         *d_block_p;
    int           d_cursor;
    my_BlockList  d_blockList;

  private:
    void *allocateBlock(int numBytes);

  private:
    // NOT IMPLEMENTED
    my_StrPool(const my_StrPool&);
    my_StrPool& operator=(const my_StrPool&);

  public:
    my_StrPool();
    ~my_StrPool();
    void *allocate(int numBytes);
    void release();
};

void *my_StrPool::allocate(int numBytes)
{
    if (numBytes <= 0) {
        return 0;                                                     // RETURN
    }
    if (d_block_p && numBytes + d_cursor <= d_blockSize) {
        char *p = d_block_p + d_cursor;
        d_cursor += numBytes;
        return p;                                                     // RETURN
    }
    else {
        return allocateBlock(numBytes);                               // RETURN
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
    k_INITIAL_SIZE = 128,
    k_GROW_FACTOR  =   2,
    k_THRESHOLD    = 128
};

void *my_StrPool::allocateBlock(int numBytes)
{
    ASSERT(0 < numBytes);

    if (k_THRESHOLD < numBytes) {
        return d_blockList.allocate(numBytes);                        // RETURN
    }
    else {
        if (d_block_p) {
            d_blockSize *= k_GROW_FACTOR;
        }
        d_block_p = (char *)d_blockList.allocate(d_blockSize);
        d_cursor = numBytes;
        return d_block_p;                                             // RETURN
    }
}

my_StrPool::my_StrPool()
: d_blockSize(k_INITIAL_SIZE)
, d_block_p(0)
, d_blockList()
{
}

my_StrPool::~my_StrPool()
{
    ASSERT(k_INITIAL_SIZE <= d_blockSize);
    ASSERT(d_block_p || (0 <= d_cursor && d_cursor <= d_blockSize));
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// my_fastcstrarray.h

///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Using a 'bdlma::AutoReleaser' to Preserve Exception Neutrality
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// A 'bdlma::AutoReleaser' proctor is often used to preserve exception
// neutrality for containers that allocate their elements using a managed
// allocator or pool.  For operations that may potentially throw an exception,
// a proctor can be used to (temporarily) manage the container's allocator or
// pool and its associated memory.  If an exception is thrown, the proctor's
// destructor invokes the 'release' method of its held allocator or pool,
// deallocating memory for all of the container's elements, thereby preventing
// a memory leak and restoring the container to the empty state.
//
// In this example, we illustrate use of a 'bdlma::AutoReleaser' proctor within
// the 'operator=' method of 'my_FastStrArray', a class that implements an
// array of C string elements.  Note that a 'my_FastStrArray' object allocates
// memory for its C string elements using a string pool, 'my_StrPool', the
// definition of which is elided.
//
// First, we define the interface of our 'my_FastStrArray' class:
//..
    class my_FastCstrArray {
        // This class implements an array of C string elements.  Each C string
        // is allocated using the 'my_StrPool' member for fast memory
        // allocation and deallocation.

        // DATA
        char             **d_array_p;      // dynamically allocated array
        int                d_capacity;     // physical capacity of this array
        int                d_length;       // logical length of this array
        my_StrPool         d_strPool;      // memory manager to supply memory
        bslma::Allocator  *d_allocator_p;  // held, not owned

      private:
        // PRIVATE MANIPULATORS
        void increaseSize();

        // Not implemented:
        my_FastCstrArray(const my_FastCstrArray&);

      public:
        // CREATORS
        my_FastCstrArray(bslma::Allocator *basicAllocator = 0);
        ~my_FastCstrArray();

        // MANIPULATORS
        my_FastCstrArray& operator=(const my_FastCstrArray& rhs);
        void append(const char *item);

        // ACCESSORS
        const char *operator[](int index) const { return d_array_p[index]; }
        int length() const { return d_length; }
    };

    // FREE OPERATORS
    ostream& operator<<(ostream& stream, const my_FastCstrArray& array);

//..
// Then, we implement the methods:
//..
    enum {
        k_MY_INITIAL_SIZE = 1, // initial physical capacity
        k_MY_GROW_FACTOR  = 2  // factor by which to grow 'd_capacity'
    };

    static inline
    int nextSize(int size)
        // Return the specified 'size' multiplied by 'k_MY_GROW_FACTOR'.
    {
        return size * k_MY_GROW_FACTOR;
    }

    static inline
    void reallocate(char             ***array,
                    int                *size,
                    int                 newSize,
                    int                 length,
                    bslma::Allocator   *allocator)
        // Reallocate memory in the specified 'array' and update the specified
        // 'size' to the specified 'newSize', using the specified 'allocator'
        // to supply memory.  The specified 'length' number of leading elements
        // are preserved.  If 'allocate' should throw an exception, this
        // function has no effect.  The behavior is undefined unless
        // '1 <= newSize', '0 <= length', and 'length <= newSize'.
    {
        ASSERT(array);
        ASSERT(*array);
        ASSERT(size);
        ASSERT(1 <= newSize);
        ASSERT(0 <= length);
        ASSERT(length <= *size);    // sanity check
        ASSERT(length <= newSize);  // ensure class invariant

        char **tmp = *array;

        *array = (char **)allocator->allocate(newSize * sizeof **array);

        // commit
        bsl::memcpy(*array, tmp, length * sizeof **array);
        *size = newSize;
        allocator->deallocate(tmp);
    }

    void my_FastCstrArray::increaseSize()
    {
        reallocate(&d_array_p,
                   &d_capacity,
                   nextSize(d_capacity),
                   d_length,
                   d_allocator_p);
    }

    // CREATORS
    my_FastCstrArray::my_FastCstrArray(bslma::Allocator *basicAllocator)
    : d_capacity(k_MY_INITIAL_SIZE)
    , d_length(0)
    , d_allocator_p(bslma::Default::allocator(basicAllocator))
    {
        d_array_p = (char **)d_allocator_p->allocate(
                                               d_capacity * sizeof *d_array_p);
    }

    my_FastCstrArray::~my_FastCstrArray()
    {
        ASSERT(1        <= d_capacity);
        ASSERT(0        <= d_length);
        ASSERT(d_length <= d_capacity);

        d_allocator_p->deallocate(d_array_p);
    }
//..
// Now, we implement 'my_FastCstrArray::operator=' using a
// 'bdlma::AutoReleaser' proctor to preserve exception neutrality:
//..
    // MANIPULATORS
    my_FastCstrArray&
    my_FastCstrArray::operator=(const my_FastCstrArray& rhs)
    {
        if (&rhs != this) {
            d_strPool.release();
            d_length = 0;

            if (rhs.d_length > d_capacity) {
                char **tmp = d_array_p;
                d_array_p = (char **)d_allocator_p->allocate(
                                             rhs.d_length * sizeof *d_array_p);
                d_capacity = rhs.d_length;
                d_allocator_p->deallocate(tmp);
            }

            bdlma::AutoReleaser<my_StrPool> autoReleaser(&d_strPool);

            for (int i = 0; i < rhs.d_length; ++i) {
                const int size =
                           static_cast<int>(bsl::strlen(rhs.d_array_p[i])) + 1;
                d_array_p[i] = (char *)d_strPool.allocate(size);
                bsl::memcpy(d_array_p[i], rhs.d_array_p[i], size);
            }

            d_length = rhs.d_length;
            autoReleaser.release();
        }

        return *this;
    }
//..
// Note that a 'bdlma::AutoReleaser' proctor is used to manage the array's C
// string memory pool while allocating memory for the individual elements.  If
// an exception is thrown during the 'for' loop, the proctor's destructor
// releases memory for all elements allocated through the pool, thus ensuring
// that no memory is leaked.
//
// Finally, we complete the implementation:
//..
    void my_FastCstrArray::append(const char *item)
    {
        if (d_length >= d_capacity) {
            this->increaseSize();
        }
        const int sSize = static_cast<int>(bsl::strlen(item)) + 1;
        char *elem = (char *)d_strPool.allocate(sSize);
        bsl::memcpy(elem, item, sSize * sizeof *item);
        d_array_p[d_length] = elem;
        ++d_length;
    }

    // FREE OPERATORS
    ostream& operator<<(ostream& stream, const my_FastCstrArray& array)
    {
        stream << "[ ";
        for (int i = 0; i < array.length(); ++i) {
            stream << '"' << array[i] << "\" ";
        }
        return stream << ']' << flush;
    }
//..

// ============================================================================
//                                MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:
      case 3: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //   Extracted from component header file.
        //
        // Concerns:
        //: 1 The usage example provided in the component header file compiles,
        //:   links, and runs as shown.
        //
        // Plan:
        //: 1 Incorporate usage example from header into test driver, remove
        //:   leading comment characters, and replace 'assert' with 'ASSERT'.
        //:   (C-1)
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "USAGE EXAMPLE" << endl
                          << "=============" << endl;

        if (verbose) cout << "Testing 'my_FastCstrArray'." << endl;
        {
            const char *DATA[] = { "A", "B", "C", "D", "E" };
            const int NUM_ELEM = sizeof DATA / sizeof *DATA;

            my_FastCstrArray mX;  const my_FastCstrArray& X = mX;
            for (int i = 0; i < NUM_ELEM; ++i) {
                mX.append(DATA[i]);
                for (int j = 0; j < i; ++j) {
                    LOOP2_ASSERT(i, j, 0 == strcmp(DATA[j], X[j]));
                }
            }
            ASSERT(NUM_ELEM == X.length());
            if (veryVerbose) { cout << '\t'; P(X); }

            my_FastCstrArray mY;  const my_FastCstrArray& Y = mY;
            mY = X;
            if (veryVerbose) { cout << '\t'; P(Y); }
            ASSERT(X.length() == Y.length());
            for (int k = 0; k < NUM_ELEM; ++k) {
                LOOP_ASSERT(k, 0 == strcmp(X[k], Y[k]));
            }
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // BASIC TEST
        //   Ensure that 'bdlma::AutoReleaser' works as expected.
        //
        // Concerns:
        //: 1 A proctor invokes the 'release' method on its managed allocator
        //:   or pool upon destruction.
        //:
        //: 2 A proctor does *not* invoke the 'release' method on an
        //:   allocator or pool that has been released from management prior
        //:   to destruction.
        //:
        //: 3 A proctor may be reset to manage a different allocator or pool
        //:   from that with which it was constructed by first explicitly
        //:   calling 'release', then calling 'reset'.
        //:
        //: 4 A proctor may be reset to manage a different allocator or pool
        //:   from that with which it was constructed *without* explicitly
        //:   calling 'release' prior to a call to 'reset'.
        //
        // Plan:
        //: 1 Create a 'bdlma::AutoReleaser' proctor initialized with a
        //:   'TestAllocator', 'a'.
        //:
        //: 2 Allow the proctor created in P-1 to go out of scope and verify
        //:   that 'a' indicates its 'release' method has been invoked.  (C-1)
        //:
        //: 3 Repeat P-1..2, but invoke the proctor's 'release' method before
        //:   the proctor goes out of scope.  Verify that 'a' indicates its
        //:   'release' method has *not* been invoked.  (C-2)
        //:
        //: 4 Create a proctor initialized with test allocator 'a1'.
        //:
        //: 5 Invoke the 'release' method on the proctor created in P-4, and
        //:   then invoke the proctor's 'reset' method with a second test
        //:   allocator 'a2'.
        //:
        //: 6 Allow the proctor to go out of scope and verify that the
        //:   'release' method has been invoked for 'a2', but *not* for 'a1'.
        //:   (C-3)
        //:
        //: 7 Repeat P-4..6, but without invoking the 'release' method prior to
        //:   calling 'reset'.  Again verify that the 'release' method has been
        //:   invoked for 'a2', but not for 'a1', when the proctor goes out of
        //:   scope.  (C-4)
        //
        // Testing:
        //   bdlma::AutoReleaser<ALLOCATOR>(ALLOCATOR *originalAllocator);
        //   ~bdlma::AutoReleaser<ALLOCATOR>();
        //   void release();
        //   void reset(ALLOCATOR *newAllocator);
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "BASIC TEST" << endl
                                  << "==========" << endl;

        if (verbose) cout << "Testing constructor." << endl;
        {
            // C-1

            TestAllocator a;  const TestAllocator &A = a;
            {
                const bdlma::AutoReleaser<TestAllocator> X(&a);
            }
            ASSERT(A.wasReleaseCalled());
        }

        if (verbose) cout << "Testing 'release'." << endl;
        {
            // C-2

            TestAllocator a;  const TestAllocator &A = a;
            {
                bdlma::AutoReleaser<TestAllocator> x(&a);
                x.release();
            }
            ASSERT(!A.wasReleaseCalled());
        }

        if (verbose) cout << "Testing 'reset'." << endl;
        {
            // C-3

            TestAllocator a1;  const TestAllocator &A1 = a1;
            TestAllocator a2;  const TestAllocator &A2 = a2;
            {
                bdlma::AutoReleaser<TestAllocator> x(&a1);
                x.release();
                x.reset(&a2);
            }
            ASSERT(!A1.wasReleaseCalled());
            ASSERT( A2.wasReleaseCalled());
        }

        {
            // C-4

            TestAllocator a1;  const TestAllocator &A1 = a1;
            TestAllocator a2;  const TestAllocator &A2 = a2;
            {
                bdlma::AutoReleaser<TestAllocator> x(&a1);
                x.reset(&a2);
            }
            ASSERT(!A1.wasReleaseCalled());
            ASSERT( A2.wasReleaseCalled());
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // HELPER CLASS TEST
        //   Ensure that the 'TestAllocator' works as expected.
        //
        // Concerns:
        //: 1 The 'wasReleaseCalled' method indicates the proper state of the
        //:  'TestAllocator' object *before* 'release' is called.
        //:
        //: 2 The 'wasReleaseCalled' method indicates the proper state of the
        //:  'TestAllocator' object *after* 'release' is called.
        //:
        //: 3 The 'wasReleaseCalled' method indicates the proper state of the
        //:  'TestAllocator' object after 'release' is called a *second* time.
        //
        // Plan:
        //: 1 Create a 'TestAllocator' object and verify that
        //:   'wasReleaseCalled' returns the expected result ('false').  (C-1)
        //:
        //: 2 Invoke 'release' on the object created in P-1 and verify that
        //:   'wasReleaseCalled' returns the expected result ('true').  (C-2)
        //:
        //: 3 Invoke 'release' on the object a second time and verify that
        //:   'wasReleaseCalled' still returns the expected result ('true').
        //:   (C-3)
        //
        // Testing:
        //   TestAllocator();
        //   ~TestAllocator();
        //   void release();
        //   bool wasReleaseCalled() const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "HELPER CLASS TEST" << endl
                                  << "=================" << endl;

        if (verbose) cout << "Testing 'TestAllocator'." << endl;

        // C-1

        TestAllocator mX;  const TestAllocator &X = mX;
                       ASSERT(!X.wasReleaseCalled());

        // C-2

        mX.release();  ASSERT( X.wasReleaseCalled());

        // C-3

        mX.release();  ASSERT( X.wasReleaseCalled());

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
// Copyright 2015 Bloomberg Finance L.P.
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
