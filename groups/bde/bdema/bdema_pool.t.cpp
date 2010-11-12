// bdema_pool.t.cpp                                                   -*-C++-*-

#include <bdema_pool.h>

#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>
#include <bslma_testallocatorexception.h>
#include <bsls_alignmentfromtype.h>
#include <bsls_alignmentutil.h>
#include <bsls_blockgrowth.h>

#include <bsl_cstdlib.h>     // atoi()
#include <bsl_cstring.h>     // memcpy()
#include <bsl_iostream.h>
#include <bsl_ostream.h>
#include <bsl_vector.h>

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

//=============================================================================
//                                  TEST PLAN
//-----------------------------------------------------------------------------
//                                  Overview
//                                  --------
// The goals of this 'bdema_Pool' test suite are to verify that 1) the
// 'allocate' method distributes memory of the correct block size; 2) the pool
// replenishes correctly according to the 'numBlocks' parameter; 3) the
// 'deallocate' method returns the memory to the pool; and 4) the 'release'
// method and the destructor releases all memory allocated through the pool.
//
// To achieve goal 1, initialize pools of varying block sizes.  Invoke
// 'allocate' repeatedly and verify that the difference between the returned
// memory addresses of two consecutive requests is equal to the specified
// block size for the current pool.  To achieve goal 2, initialize a pool with
// a test allocator and varying 'numBlocks'.  Invoke 'allocate' repeatedly and
// verify that the pool requests memory blocks of the expected sizes from the
// allocator.  To achieve goal 3, allocate multiple memory from the pool and
// store the returned addresses in an array.  Deallocate the memory in reverse
// order, then allocate memory again and verify that the allocated memory are
// in the same order as those stored in the array.  Note that this test depends
// on the implementation detail of 'deallocate', in which a deallocated memory
// is placed at the beginning of the free memory list.  To achieve goal 4,
// initialize two pools, each supplied with its own test allocator.  Invoke
// 'allocate' repeatedly.  Invoke 'release' on one pool, and allow the other
// pool to go out of scope.  Verify that both test allocators indicate all
// memory is released.
//-----------------------------------------------------------------------------
// [ 5] bdema_Pool(blockSize, basicAllocator);
// [ 2] bdema_Pool(blockSize, numBlocks, basicAllocator);
// [ 7] ~bdema_Pool();
// [ 2] void *allocate();
// [ 6] void deallocate(address);
// [ 7] void release();
// [ 8] void *operator new(size, pool);
// [ 9] void *operator new(size, pool);
// [ 9] void operator delete(address, pool);
// [10] template<typename TYPE> void deleteObject(TYPE *object);
// [11] template<typename TYPE> void deleteObjectRaw(TYPE *object);
// [12] void reserveCapacity(numBlocks);
//-----------------------------------------------------------------------------
// [13] USAGE EXAMPLE
// [ 2] 'allocate' returns memory of the correct block size.
// [ 3] Pool replenishes memory of the correct size for positive 'numBlocks'.
// [ 4] Pool replenishes memory of the correct size for negative 'numBlocks'.
// [ 5] Pool replenishes memory of the correct size for default 'numBlocks'.
// [ 1] int blockSize(numBytes);
// [ 1] int poolBlockSize(size);
// [ 1] int growNumBlocks(numBlocks, growFactor, maxNumBlocks);
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
#define T_ cout << "\t" << flush;             // Print a tab (w/o newline)
//                               GLOBAL TYPEDEF
//-----------------------------------------------------------------------------

typedef bdema_Pool Obj;

// This type is copied from the 'bdema_infrequentdeleteblocklist.h' for testing
// purposes.

struct InfrequentDeleteBlock {
    InfrequentDeleteBlock              *d_next_p;
    bsls_AlignmentUtil::MaxAlignedType  d_memory;  // force alignment
};

// This type is copied from 'bdema_pool.cpp' to determine the internal limits
// of 'bdema_Pool'.
enum {
    INITIAL_NUM_BLOCKS = 1,
    INITIAL_CHUNK_SIZE = 1,
    GROW_FACTOR        = 2,
    MAX_NUM_BLOCKS     = 32,
    MAX_CHUNK_SIZE     = 32
};

//=============================================================================
//                      FILE-STATIC FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

static int blockSize(int numBytes)
    // Return the adjusted block size based on the specified 'numBytes' using
    // the calculation performed by the
    // 'bdema_InfrequentDeleteBlockList::allocate' method.
{
    ASSERT(0 <= numBytes);

    if (numBytes) {
        numBytes += sizeof(InfrequentDeleteBlock) - 1;
        numBytes &= ~(bsls_AlignmentUtil::BSLS_MAX_ALIGNMENT - 1);
    }

    return numBytes;
}

inline
int myAbs(int n)
    // Return the absolute value of the specified 'n'.  The behavior is
    // undefined unless INT_MIN + 1 < n < INT_MAX.
{
    return n >= 0 ? n : -n;
}

inline
int roundUp(int x, int y)
    // Round up the specified 'x' to the nearest multiples of the specified
    // 'y'.  The behavior is undefined unless 0 <= x and 0 < y;
{
    ASSERT(0 <= x);
    ASSERT(0 < y);
    return (x + y - 1) / y * y;
}

inline
int poolBlockSize(int size)
    // Return the actual block size used by the pool when given the specified
    // 'size'.
{
    if (size < sizeof(void *))
        return sizeof(void *);
    else
        return roundUp(size, bsls_AlignmentFromType<void *>::VALUE);
}

inline
int growNumBlocks(int numBlocks, int growFactor, int maxNumBlocks)
    // Simulate the pool's growing behavior using the specified 'numBlocks',
    // 'growFactor' and 'maxNumBlocks' and return the new value for
    // 'numBlocks'.  The behavior is undefined unless 0 > numBlocks,
    // 0 < growFactor, and 0 > maxNumBlocks.
{
    ASSERT(0 != numBlocks);
    ASSERT(0 < growFactor);
    ASSERT(0 < maxNumBlocks);

    if (numBlocks < maxNumBlocks) {
        numBlocks *= 2;
        if (numBlocks > maxNumBlocks) {
            numBlocks = maxNumBlocks;
        }
    }
    return numBlocks;
}

//=============================================================================
//                                USAGE EXAMPLE
//-----------------------------------------------------------------------------
// A 'bdema_Pool' can be used by node-based containers (such as lists, trees,
// and hash tables that hold multiple elements of uniform size) for efficient
// memory allocation of new elements.  The following container class,
// 'my_PooledArray', stores templatized values "out-of-place" as nodes in a
// 'vector' of pointers.  Since the size of each node is fixed and known *a
// priori*, the class uses a 'bdema_Pool' to allocate memory for the nodes to
// improve memory allocation efficiency:
//..
//  // my_poolarray.h
//
    template <class T>
    class my_PooledArray {
        // This class implements a container that stores 'double' values
        // out-of-place.

        // DATA
        bsl::vector<T *> d_array_p;  // array of pooled elements
        bdema_Pool       d_pool;     // memory manager for array elements

      public:
        // CREATORS
        my_PooledArray(bslma_Allocator *basicAllocator = 0);
            // Create a pooled array that stores the parameterized values
            // "out-of-place".  Optionally specify a 'basicAllocator' used to
            // supply memory.  If 'basicAllocator' is 0, the currently
            // installed default allocator is used.

        ~my_PooledArray();
            // Destroy this array and all elements held by it.

        // MANIPULATORS
        void append(const T &value);
            // Append the specified 'value' to this array.

        void removeAll();
            // Remove all elements from this array.

        // ACCESSORS
        int length() const;
            // Return the number of elements in this array.

        const T& operator[](int index) const;
            // Return a reference to the non-modifiable value at the specified
            // 'index' in this array.  The behavior is undefined unless
            // '0 <= index < length()'.
    };
//..
// In the 'removeAll' method, all elements are deallocated by invoking the
// pool's 'release' method.  This technique implies significant performance
// gain when the array contains many elements:
//..
    // MANIPULATORS
    template <class T>
    inline
    void my_PooledArray<T>::removeAll()
    {
        d_array_p.clear();
        d_pool.release();
    }

    // ACCESSORS
    template <class T>
    inline
    int my_PooledArray<T>::length() const
    {
        return d_array_p.size();
    }

    template <class T>
    inline
    const T& my_PooledArray<T>::operator[](int index) const
    {
        ASSERT(0 <= index);
        ASSERT(index < length());

        return *d_array_p[index];
    }
//..
// Note that the growth strategy and maximum chunk size of the pool is left as
// the default value:
//..
//  // my_poolarray.cpp
//  #include <my_poolarray.h>

    // CREATORS
    template <class T>
    my_PooledArray<T>::my_PooledArray(bslma_Allocator *basicAllocator)
    : d_array_p(basicAllocator)
    , d_pool(sizeof(T), basicAllocator)
    {
    }
//..
// Since all memory is managed by 'd_pool', we do not have to explicitly invoke
// 'deleteObject' to reclaim outstanding memory.  The destructor of the pool
// will automatically deallocate all array elements:
//..
    template <class T>
    my_PooledArray<T>::~my_PooledArray()
    {
        // Elements are automatically deallocated when 'd_pool' is destroyed.
    }
//..
// Note that the overloaded "placement" 'new' is used to allocate new nodes:
//..
    template <class T>
    void my_PooledArray<T>::append(const T& value)
    {
        T *tmp = new (d_pool) T(value);
        d_array_p.push_back(tmp);
    }
//..


//=============================================================================
//           Additional Functionality Needed to Complete Usage Test Case
//-----------------------------------------------------------------------------

template <class T>
ostream& operator<<(ostream& stream, const my_PooledArray<T>& array)
{
    stream << "[ ";
    for (int i = 0; i < array.length(); ++i) {
        stream << array[i] << " ";
    }
    return stream << ']' << flush;
}

//-----------------------------------------------------------------------------
//                      OVERLOADED OPERATOR NEW USAGE EXAMPLE
//-----------------------------------------------------------------------------

class my_Type {
    char *d_stuff_p;
    bslma_Allocator *d_allocator_p;

  public:
    my_Type(int size, bslma_Allocator *basicAllocator)
    : d_allocator_p(basicAllocator)
    {
        d_stuff_p = (char *) d_allocator_p->allocate(size);
    }

    ~my_Type()
    {
        d_allocator_p->deallocate(d_stuff_p);
    }
};

my_Type *newMyType(bdema_Pool *pool, bslma_Allocator *basicAllocator) {
    return new (*pool) my_Type(5, basicAllocator);
}

void deleteMyType(bdema_Pool *pool, my_Type *t) {
    t->~my_Type();
    pool->deallocate(t);
}

//-----------------------------------------------------------------------------
// HELPER CLASS FOR TESTING EXCEPTION SAFETY OF OVERLOADED OPERATOR NEW
//-----------------------------------------------------------------------------

class my_ClassThatMayThrowFromConstructor {
    char d_c;
  public:
    my_ClassThatMayThrowFromConstructor()
    {
#ifdef BDE_BUILD_TARGET_EXC
        throw int(13);
#endif
    }

    ~my_ClassThatMayThrowFromConstructor()
    {
    }
};

//=============================================================================
// CONCRETE OBJECTS FOR TESTING 'operator new' and 'deleteObject'
//-----------------------------------------------------------------------------
static int my_ClassCode = 0;

class my_Class1 {
  public:
    my_Class1() { my_ClassCode = 1; }
    ~my_Class1() { my_ClassCode = 2; }
};

class my_Class2 {
  public:
    my_Class2() { my_ClassCode = 3; }
    ~my_Class2() { my_ClassCode = 4; }
};

// The "dreaded diamond".

static int virtualBaseObjectCount = 0;
static int leftBaseObjectCount    = 0;
static int rightBaseObjectCount   = 0;
static int mostDerivedObjectCount = 0;

class my_VirtualBase {
    int x;
public:
    my_VirtualBase()          { virtualBaseObjectCount = 1; }
    virtual ~my_VirtualBase() { virtualBaseObjectCount = 0; }
};

class my_LeftBase : virtual public my_VirtualBase {
    int x;
public:
    my_LeftBase()             { leftBaseObjectCount = 1; }
    virtual ~my_LeftBase()    { leftBaseObjectCount = 0; }
};

class my_RightBase : virtual public my_VirtualBase {
    int x;
public:
    my_RightBase()            { rightBaseObjectCount = 1; }
    virtual ~my_RightBase()   { rightBaseObjectCount = 0; }
};

class my_MostDerived : public my_LeftBase, public my_RightBase {
    int x;
public:
    my_MostDerived()          { mostDerivedObjectCount = 1; }
    ~my_MostDerived()         { mostDerivedObjectCount = 0; }
};

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
      case 13: {
        // --------------------------------------------------------------------
        // TESTING blockSize
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "Testing 'blockSize'" << endl
                                  << "===================" << endl;

        bslma_TestAllocator a(veryVeryVerbose);
        Obj mX(1, &a);
        ASSERT(1 == mX.blockSize());
        char *c = new (mX) char;
        mX.deleteObject(c);

      } break;
      case 12: {
        // --------------------------------------------------------------------
        // TESTING USAGE EXAMPLE
        //   The usage example provided in the component header file must
        //   compile, link, and run on all platforms as shown.
        //
        // Plan:
        //   Create a 'my_PooledArray' object and append varying values to it.
        //   Verify that the values are correctly appended using 'operator[]'.
        //   Invoke 'removeAll' and verify that the array length becomes 0.
        //
        // Testing:
        //   USAGE TEST - Make sure main usage example compiles and works.
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "USAGE TEST" << endl
                                  << "==========" << endl;

        if (verbose) cout << "\nTesting 'my_PooledArray'." << endl;
        {
            const double DATA[] = { 0.0, 1.2, 2.3, 3.4, 4.5, 5.6, 6.7 };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            bslma_TestAllocator a(veryVeryVerbose);
            my_PooledArray<double> array(&a);

            for (int i = 0; i < NUM_DATA; ++i) {
                const double VALUE = DATA[i];
                array.append(VALUE);
                LOOP_ASSERT(i, i + 1 == array.length());
                LOOP_ASSERT(i, VALUE == array[i]);
            }
            if (veryVerbose) { cout << '\t' << array << endl; }
            array.removeAll();
            ASSERT(0 == array.length());
        }

        if (verbose) cout << "\nUsage test for 'new' operator." << endl;
        {
            bslma_TestAllocator a(veryVeryVerbose);
            const int BLOCK_SIZE = sizeof(my_Type);
            const int CHUNK_SIZE = 10;
            Obj mX(BLOCK_SIZE,
                   bsls_BlockGrowth::BSLS_GEOMETRIC,
                   CHUNK_SIZE,
                   &a);

            my_Type *t = newMyType(&mX, &a);
            deleteMyType(&mX, t);
        }

      } break;
      case 11: {
        // --------------------------------------------------------------------
        // RESERVE CAPACITY TEST
        //   We want to make sure that when capacity in a pool has been
        //   reserved, calls to allocate do not result in memory allocations.
        //   Also, we want to make sure that 'reserveCapacity' behaves
        //   correctly when 'numBlocks' passed to it is 0, less than current
        //   capacity, the same as current capacity, and larger than current
        //   capacity.
        //
        // Plan:
        //    Reserve various capacities as described above and check the
        //    number of memory allocation.
        //
        // Testing:
        //   void reserveCapacity(numBlocks);
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "RESERVECAPACITY TEST" << endl
                                  << "====================" << endl;

        if (verbose) cout << "\nTesting 'reserveCapacity'." << endl;

        bslma_TestAllocator a(veryVeryVerbose);
         const bslma_TestAllocator& A = a;
        {
            const int BLOCK_SIZE = 5;
            const int CHUNK_SIZE = 10;
            Obj mX(BLOCK_SIZE,
                   bsls_BlockGrowth::BSLS_CONSTANT,
                   CHUNK_SIZE,
                   &a);

            for (int i = 0; i < CHUNK_SIZE / 2; ++i) {
                mX.allocate();
            }

            int numAllocation = A.numAllocations();
            mX.reserveCapacity(CHUNK_SIZE / 2);
            ASSERT(A.numAllocations() == numAllocation);

            for (int ii = 0; ii < CHUNK_SIZE / 2; ++ii) {
                mX.allocate();
            }

            mX.reserveCapacity(0);
            ASSERT(A.numAllocations() == numAllocation);

            mX.reserveCapacity(CHUNK_SIZE);
            ++numAllocation;
            ASSERT(A.numAllocations() == numAllocation);

            for (int j = 0; j < CHUNK_SIZE; ++j) {
                mX.allocate();
            }
            ASSERT(A.numAllocations() == numAllocation);
        }
        ASSERT(0 == A.numBytesInUse());
      } break;
      case 10: {
        // --------------------------------------------------------------------
        // MEMBER TEMPLATE METHOD 'deleteObjectRaw' TEST:
        //   We want to make sure that when 'deleteObjectRaw' is used both
        //   destructor and 'deallocate' are invoked.
        //
        // Plan:
        //   Using a pool and placement new operator construct objects of
        //   two different classes.  Invoke 'deleteObjectRaw' to delete
        //   constructed objects and check that both destructor and
        //   'deallocate' have been called.
        //
        // Testing:
        //   template<typename TYPE> void deleteObjectRaw(TYPE *object)
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "'deleteObjectRaw' TEST" << endl
                                  << "======================" << endl;

        if (verbose) cout << "\nTesting 'deleteObjectRaw':" << endl;
        {
            bslma_TestAllocator a(veryVeryVerbose);
            const bslma_TestAllocator& A = a;

            const int BLOCK_SIZE = sizeof(my_Class1);
            ASSERT(sizeof(my_Class2) == BLOCK_SIZE);
            const int CHUNK_SIZE = 1;
            Obj mX(BLOCK_SIZE,
                   bsls_BlockGrowth::BSLS_CONSTANT,
                   CHUNK_SIZE,
                   &a);

            if (verbose) cout << "\twith a my_Class1 object" << endl;

            my_ClassCode=0;

            my_Class1 *pC1 = (my_Class1 *) mX.allocate();
            new(pC1) my_Class1;
            if (verbose) { T_  T_  P(my_ClassCode); }
            ASSERT(1 == my_ClassCode);
            ASSERT(A.numAllocations() == 1);

            mX.deleteObjectRaw(pC1);
            if (verbose) { T_  T_  P(my_ClassCode); }
            ASSERT(2 == my_ClassCode);
            ASSERT(A.numAllocations() == 1);
            mX.allocate();
            ASSERT(A.numAllocations() == 1);
                  // By observing that the number of allocations
                  // stays at one we confirm that the memory obtained
                  // from the pool has been returned by 'deleteObj'.
                  // Had it not been returned, the call to allocate would
                  // have required another allocation from the allocator.

            if (verbose) cout << "\twith a my_Class2 object" << endl;

            my_Class2 *pC2 = (my_Class2 *) mX.allocate();
            new(pC2) my_Class2;
            if (verbose) { T_  T_  P(my_ClassCode); }
            ASSERT(3 == my_ClassCode);
            ASSERT(A.numAllocations() == 2);

            mX.deleteObjectRaw(pC2);
            if (verbose) { T_  T_  P(my_ClassCode); }
            ASSERT(4 == my_ClassCode);
            ASSERT(A.numAllocations() == 2);
            mX.allocate();
            ASSERT(A.numAllocations() == 2);
        }

        if (verbose) cout <<"\nTesting 'deleteObjectRaw' on polymorphic types:"
                          << endl;
        {
            bslma_TestAllocator a(veryVeryVerbose);
            const bslma_TestAllocator& A = a;

            const int BLOCK_SIZE = sizeof(my_MostDerived);
            ASSERT(sizeof(my_MostDerived) == BLOCK_SIZE);
            const int CHUNK_SIZE = 1;
            Obj mX(BLOCK_SIZE,
                   bsls_BlockGrowth::BSLS_CONSTANT,
                   CHUNK_SIZE,
                   &a);

            if (verbose) cout << "\tdeleteObjectRaw(my_MostDerived*)" << endl;

            my_MostDerived *pMost = (my_MostDerived *) mX.allocate();
            const my_MostDerived *pMostCONST = pMost;

            ASSERT(0 == mostDerivedObjectCount);
            ASSERT(0 == rightBaseObjectCount);
            ASSERT(0 == leftBaseObjectCount);
            ASSERT(0 == virtualBaseObjectCount);
            new(pMost) my_MostDerived;
            ASSERT(1 == mostDerivedObjectCount);
            ASSERT(1 == rightBaseObjectCount);
            ASSERT(1 == leftBaseObjectCount);
            ASSERT(1 == virtualBaseObjectCount);

            mX.deleteObjectRaw(pMostCONST);
            ASSERT(0 == mostDerivedObjectCount);
            ASSERT(0 == rightBaseObjectCount);
            ASSERT(0 == leftBaseObjectCount);
            ASSERT(0 == virtualBaseObjectCount);

            if (verbose) cout << "\tWith a null pointer" << endl;

            pMost = 0;
            mX.deleteObjectRaw(pMost);
            ASSERT(0 == mostDerivedObjectCount);
            ASSERT(0 == rightBaseObjectCount);
            ASSERT(0 == leftBaseObjectCount);
            ASSERT(0 == virtualBaseObjectCount);
        }
      } break;
      case 9: {
        // --------------------------------------------------------------------
        // MEMBER TEMPLATE METHOD 'deleteObject' TEST:
        //   We want to make sure that when 'deleteObj' is used both
        //   destructor and 'deallocate' are invoked.
        //
        // Plan:
        //   Using a pool and placement new operator construct objects of
        //   two different classes.  Invoke 'deleteObj' to delete constructed
        //   objects and check that both destructor and 'deallocate' have been
        //   called.
        //
        // Testing:
        //   template<typename TYPE> void deleteObject(TYPE *object)
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "'deleteObject' TEST" << endl
                                  << "===================" << endl;

        if (verbose) cout << "\nTesting 'deleteObject':" << endl;
        {
            bslma_TestAllocator a(veryVeryVerbose);
            const bslma_TestAllocator& A = a;

            const int BLOCK_SIZE = sizeof(my_Class1);
            ASSERT(sizeof(my_Class2) == BLOCK_SIZE);
            const int CHUNK_SIZE = 1;
            Obj mX(BLOCK_SIZE,
                   bsls_BlockGrowth::BSLS_CONSTANT,
                   CHUNK_SIZE,
                   &a);

            if (verbose) cout << "\twith a my_Class1 object" << endl;

            my_ClassCode=0;

            my_Class1 *pC1 = (my_Class1 *) mX.allocate();
            new(pC1) my_Class1;
            if (verbose) { T_  T_  P(my_ClassCode); }
            ASSERT(1 == my_ClassCode);
            ASSERT(A.numAllocations() == 1);

            mX.deleteObject(pC1);
            if (verbose) { T_  T_  P(my_ClassCode); }
            ASSERT(2 == my_ClassCode);
            ASSERT(A.numAllocations() == 1);
            mX.allocate();
            ASSERT(A.numAllocations() == 1);
                  // By observing that the number of allocations
                  // stays at one we confirm that the memory obtained
                  // from the pool has been returned by 'deleteObj'.
                  // Had it not been returned, the call to allocate would
                  // have required another allocation from the allocator.

            if (verbose) cout << "\twith a my_Class2 object" << endl;

            my_Class2 *pC2 = (my_Class2 *) mX.allocate();
            new(pC2) my_Class2;
            if (verbose) { T_  T_  P(my_ClassCode); }
            ASSERT(3 == my_ClassCode);
            ASSERT(A.numAllocations() == 2);

            mX.deleteObject(pC2);
            if (verbose) { T_  T_  P(my_ClassCode); }
            ASSERT(4 == my_ClassCode);
            ASSERT(A.numAllocations() == 2);
            mX.allocate();
            ASSERT(A.numAllocations() == 2);
        }

        if (verbose) cout << "\nTesting 'deleteObject' on polymorphic types:"
                          << endl;
        {
            bslma_TestAllocator a(veryVeryVerbose);
            const bslma_TestAllocator& A = a;

            const int BLOCK_SIZE = sizeof(my_MostDerived);
            ASSERT(sizeof(my_MostDerived) == BLOCK_SIZE);
            const int CHUNK_SIZE = 1;
            Obj mX(BLOCK_SIZE,
                   bsls_BlockGrowth::BSLS_CONSTANT,
                   CHUNK_SIZE,
                   &a);

            if (verbose) cout << "\tdeleteObject(my_MostDerived*)" << endl;

            my_MostDerived *pMost = (my_MostDerived *) mX.allocate();
            const my_MostDerived *pMostCONST = pMost;

            ASSERT(0 == mostDerivedObjectCount);
            ASSERT(0 == rightBaseObjectCount);
            ASSERT(0 == leftBaseObjectCount);
            ASSERT(0 == virtualBaseObjectCount);
            new(pMost) my_MostDerived;
            ASSERT(1 == mostDerivedObjectCount);
            ASSERT(1 == rightBaseObjectCount);
            ASSERT(1 == leftBaseObjectCount);
            ASSERT(1 == virtualBaseObjectCount);

            mX.deleteObject(pMostCONST);
            ASSERT(0 == mostDerivedObjectCount);
            ASSERT(0 == rightBaseObjectCount);
            ASSERT(0 == leftBaseObjectCount);
            ASSERT(0 == virtualBaseObjectCount);

            if (verbose) cout << "\tdeleteObject(my_LeftBase*)" << endl;

            pMost = (my_MostDerived *) mX.allocate();

            new(pMost) my_MostDerived;
            const my_LeftBase *pLeftCONST = pMost;
            ASSERT(1 == mostDerivedObjectCount);
            ASSERT(1 == rightBaseObjectCount);
            ASSERT(1 == leftBaseObjectCount);
            ASSERT(1 == virtualBaseObjectCount);

            mX.deleteObject(pLeftCONST);
            ASSERT(0 == mostDerivedObjectCount);
            ASSERT(0 == rightBaseObjectCount);
            ASSERT(0 == leftBaseObjectCount);
            ASSERT(0 == virtualBaseObjectCount);

            if (verbose) cout << "\tdeleteObject(my_RightBase*)" << endl;

            pMost = (my_MostDerived *) mX.allocate();

            new(pMost) my_MostDerived;
            const my_RightBase *pRightCONST = pMost;
            ASSERT(1 == mostDerivedObjectCount);
            ASSERT(1 == rightBaseObjectCount);
            ASSERT(1 == leftBaseObjectCount);
            ASSERT(1 == virtualBaseObjectCount);

            mX.deleteObject(pRightCONST);
            ASSERT(0 == mostDerivedObjectCount);
            ASSERT(0 == rightBaseObjectCount);
            ASSERT(0 == leftBaseObjectCount);
            ASSERT(0 == virtualBaseObjectCount);

            if (verbose) cout << "\tdeleteObject(my_VirtualBase*)" << endl;

            pMost = (my_MostDerived *) mX.allocate();

            new(pMost) my_MostDerived;
            const my_VirtualBase *pVirtualCONST = pMost;
            ASSERT(1 == mostDerivedObjectCount);
            ASSERT(1 == rightBaseObjectCount);
            ASSERT(1 == leftBaseObjectCount);
            ASSERT(1 == virtualBaseObjectCount);

            mX.deleteObject(pVirtualCONST);
            ASSERT(0 == mostDerivedObjectCount);
            ASSERT(0 == rightBaseObjectCount);
            ASSERT(0 == leftBaseObjectCount);
            ASSERT(0 == virtualBaseObjectCount);

            if (verbose) cout << "\tWith a null pointer" << endl;

            pMost = 0;
            mX.deleteObject(pMost);
            ASSERT(0 == mostDerivedObjectCount);
            ASSERT(0 == rightBaseObjectCount);
            ASSERT(0 == leftBaseObjectCount);
            ASSERT(0 == virtualBaseObjectCount);
        }

      } break;
      case 8: {
        // --------------------------------------------------------------------
        // EXCEPTION SAFETY OF OPERATOR NEW TEST:
        //   We want to make sure that when the overloaded operator new
        //   is invoked and the constructor of the new object throws an
        //   exception, the overloaded delete operator is invoked
        //   automatically to deallocate the object.
        //
        // Plan:
        //   Invoke the 'operator new' for a class which throws exception from
        //   the constructor.  Catch the exception and verify that deallocation
        //   was performed automatically.
        //
        // Testing:
        //   void *operator new(int size, bdema_Pool& pool);
        //   void operator delete(void *address, bdema_Pool& pool);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "EXCEPTION SAFETY OF OPERATOR NEW TEST" << endl
                          << "=====================================" << endl;

        bslma_TestAllocator a(veryVeryVerbose);
        const bslma_TestAllocator& A = a;
        {
            const int BLOCK_SIZE =
                                   sizeof(my_ClassThatMayThrowFromConstructor);
            const int CHUNK_SIZE = 1;
            Obj mX(BLOCK_SIZE,
                   bsls_BlockGrowth::BSLS_CONSTANT,
                   CHUNK_SIZE,
                   &a);

            my_ClassThatMayThrowFromConstructor *p=0;
#ifdef BDE_BUILD_TARGET_EXC
            try
#endif
            {
                p = new (mX) my_ClassThatMayThrowFromConstructor;
                p->~my_ClassThatMayThrowFromConstructor();
                mX.deallocate(p);
                p = 0;
            }
#ifdef BDE_BUILD_TARGET_EXC
            catch(int n)
            {
                if (verbose) cout << "\nCaught exception." << endl;
                ASSERT(13 == n);
            }
#endif
            ASSERT(!p);
            mX.allocate();
            ASSERT(A.numAllocations() == 1);
                  // By observing that the number of allocations
                  // stays at one we confirm that the memory obtained
                  // from the pool by the operator 'new' has been returned
                  // to the pool on exception.
                  // Had it not been returned, the call to allocate would
                  // have required another allocation from the allocator.
        }

      } break;
      case 7: {
        // --------------------------------------------------------------------
        // OPERATOR TEST:
        //   We want to make sure that the correct underlying method is
        //   called based on the type of the overloaded 'new' operator.
        //
        // Plan:
        //   Invoke 'operator new' for various types.  Verify that
        //   correct constructors are called.
        //
        // Testing:
        //   void *operator new(int size, bdema_Pool& pool);
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "OPERATOR TEST" << endl
                                  << "=============" << endl;

        bslma_TestAllocator a(veryVeryVerbose);

        const int BLOCK_SIZE = sizeof(my_Class1);
        ASSERT(sizeof(my_Class2)==BLOCK_SIZE);
        const int CHUNK_SIZE = 10;
        Obj mX(BLOCK_SIZE, bsls_BlockGrowth::BSLS_CONSTANT, CHUNK_SIZE, &a);

        my_ClassCode=0;
        my_Class1 *c1 = new (mX) my_Class1;
        ASSERT(c1);  ASSERT(1 == my_ClassCode);
        my_Class2 *c2 = new (mX) my_Class2;
        ASSERT(c2);  ASSERT(3 == my_ClassCode);

      } break;
      case 6: {
        // --------------------------------------------------------------------
        // RELEASE TEST
        //   We want to make sure that both destructor and 'release' free all
        //   the memory used by pool.
        //
        // Plan:
        //   Initialize two pools with varying block sizes and 'numBlocks',
        //   and supply each with its own test allocator.  Invoke 'allocate'
        //   repeatedly.  Invoke 'release' on one pool, and allow the other
        //   pool to go out of scope.  Verify that both allocators indicate all
        //   memory has been released by the pools.
        //
        // Testing:
        //   ~bdema_pool();
        //   void release();
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "RELEASE TEST" << endl
                                  << "============" << endl;

        if (verbose) cout << "\nTesting 'release' and destructor." << endl;

        bsls_BlockGrowth::Strategy GEO = bsls_BlockGrowth::BSLS_GEOMETRIC;
        bsls_BlockGrowth::Strategy CON = bsls_BlockGrowth::BSLS_CONSTANT;

        struct {
            int                        d_line;
            int                        d_blockSize;
            int                        d_numBlocks;
            bsls_BlockGrowth::Strategy d_strategy;
        } DATA[] = {
            //line    block
            //no.     size      max chunk size    growth strategy
            //----    ------    ----------------  ---------------
            { L_,       1,                    5,            CON },
            { L_,       5,                   10,            CON },
            { L_,      12,                    1,            GEO },
            { L_,      24,                    5,            GEO },
            { L_,      32,       MAX_CHUNK_SIZE,            GEO }
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        const int NUM_REQUESTS = 100;
        bslma_TestAllocator taX(veryVeryVerbose);
        const bslma_TestAllocator& TAX = taX;
        bslma_TestAllocator taY(veryVeryVerbose);
        const bslma_TestAllocator& TAY = taY;

        for (int di = 0; di < NUM_DATA; ++di) {
            const int                        LINE       = DATA[di].d_line;
            const int                        BLOCK_SIZE = DATA[di].d_blockSize;
            const int                        CHUNK_SIZE = DATA[di].d_numBlocks;
            const bsls_BlockGrowth::Strategy STRATEGY   = DATA[di].d_strategy;
            {
                Obj mX(BLOCK_SIZE,
                       bsls_BlockGrowth::BSLS_GEOMETRIC,
                       CHUNK_SIZE,
                       &taX);
                Obj mY(BLOCK_SIZE,
                       bsls_BlockGrowth::BSLS_GEOMETRIC,
                       CHUNK_SIZE,
                       &taY);

                for (int ai = 0; ai < NUM_REQUESTS; ++ai) {
                    mX.allocate();
                    mY.allocate();
                }

                if (veryVerbose) { T_ P_(TAX.numBytesInUse()); }
                mX.release();
                if (veryVerbose) { T_ P(TAX.numBytesInUse()); }

                if (veryVerbose) { T_ P_(TAY.numBytesInUse()); }
                // Let 'mY' go out of scope.
            }
            if (veryVerbose) { T_ P(TAY.numBytesInUse()); }

            LOOP2_ASSERT(LINE, di, 0 == TAX.numBytesInUse());
            LOOP2_ASSERT(LINE, di, 0 == TAY.numBytesInUse());
        }
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // DEALLOCATE TEST
        //
        // Concerns:
        //   We want to make sure that memory returned to a pool by
        //   'deallocate' is 'pooled' for future allocations.
        //
        // Plan:
        //   Initialize a pool with varying block sizes and 'numBlocks'.
        //   Invoke 'allocate' repeatedly and store the returned memory address
        //   in an array.  Then deallocate the allocated memory address in
        //   reverse order.  Finally, allocate memory again and verify that the
        //   returned memory addresses are in the same order as those stored in
        //   the array.  Also verify that no additional memory request to the
        //   allocator occurs.
        //
        // Testing:
        //   void deallocate(address);
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "DEALLOCATE TEST" << endl
                                  << "===============" << endl;

        if (verbose) cout << "\nTesting 'deallocate'." << endl;

        bsls_BlockGrowth::Strategy GEO = bsls_BlockGrowth::BSLS_GEOMETRIC;
        bsls_BlockGrowth::Strategy CON = bsls_BlockGrowth::BSLS_CONSTANT;

        struct {
            int                        d_line;
            int                        d_blockSize;
            int                        d_numBlocks;
            bsls_BlockGrowth::Strategy d_strategy;
        } DATA[] = {
            //line    block
            //no.     size      max chunk size    growth strategy
            //----    ------    ----------------  ---------------
            { L_,       1,                    5,            CON },
            { L_,       5,                   10,            CON },
            { L_,      12,                    1,            GEO },
            { L_,      24,                    5,            GEO },
            { L_,      32,       MAX_CHUNK_SIZE,            GEO }
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        const int NUM_REQUESTS = 100;
        void *p[NUM_REQUESTS];
        bslma_TestAllocator ta(veryVeryVerbose);
        const bslma_TestAllocator& TA = ta;

        for (int di = 0; di < NUM_DATA; ++di) {
            const int                        LINE       = DATA[di].d_line;
            const int                        BLOCK_SIZE = DATA[di].d_blockSize;
            const int                        CHUNK_SIZE = DATA[di].d_numBlocks;
            const bsls_BlockGrowth::Strategy STRATEGY   = DATA[di].d_strategy;
            Obj mX(BLOCK_SIZE, STRATEGY, CHUNK_SIZE, &ta);

            for (int ai = 0; ai < NUM_REQUESTS; ++ai) {
                p[ai] = mX.allocate();
            }

            int numAllocation = TA.numAllocations();

            for (int dd = NUM_REQUESTS - 1; dd >= 0; --dd) {
                mX.deallocate(p[dd]);
            }

            if (veryVerbose) { T_ P_(CHUNK_SIZE); P(numAllocation); }

            // Ensure memory was deallocated in expected sequence
            for (int aj = 0; aj < NUM_REQUESTS; ++aj) {
                LOOP3_ASSERT(LINE, di, aj, p[aj] == mX.allocate());
            }

            // Ensure no additional memory request to the allocator occurred
            LOOP2_ASSERT(LINE, di, TA.numAllocations() == numAllocation);
        }
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // DEFAULT PARAMETERS TEST
        //
        // Concerns:
        //    1. That the pool replenishes with the proper growth strategy when
        //       'growthStrategy' is not supplied.
        //
        //    2. That the pool replenishes with the proper maximum blocks per
        //       chunk when 'maxBlocksPerChunk' is not supplied.
        //
        //    3. That the new delete allocator is to supply memory when
        //       'basicAllocator' is not supplied.
        //
        // Plan:
        //   Initialize a pool with a chosen block size, default 'numBlocks'
        //   and a test allocator.  Initialize a second pool as a reference
        //   with the same block size, INITIAL_NUM_BLOCKS for 'numBlocks'
        //   and a second test allocator.  Invoke 'allocate' repeatedly on both
        //   pools so that the pools deplete and replenish until the pools stop
        //   growing in size.  Verify that for each replenishment the allocator
        //   for the pool under test contains the same number of memory
        //   requests and the same request size as the allocator for the
        //   reference pool.
        //
        // Testing:
        //   bdema_Pool(bs, gs = GEO, mbpc = 32, basicAllocator = 0);
        //   void *allocate();
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "DEFAULT PARAMETERS TEST" << endl
                                  << "=======================" << endl;

        const int BLOCK_SIZE = 4;

//        if (verbose) cout << "\nTesting default growth strategy." << endl;
//        {
//            bslma_TestAllocator taX(veryVeryVerbose);
//            const bslma_TestAllocator& TAX = taX;
//            Obj mX(BLOCK_SIZE, MAX_CHUNK_SIZE, &taX);
//
//            bslma_TestAllocator taY(veryVeryVerbose);
//            const bslma_TestAllocator& TAY = taY;
//            Obj mY(BLOCK_SIZE,
//                   bsls_BlockGrowth::BSLS_GEOMETRIC,
//                   MAX_CHUNK_SIZE,
//                   &taY);
//
//            int exceedMaxNumBlocksCount = 0;
//            int chunkSize = INITIAL_NUM_BLOCKS;
//            while (1) {
//                // Allocate until current pool is deplete.
//                for (int oi = 0; oi < myAbs(chunkSize); ++oi) {
//                    mX.allocate();
//                    mY.allocate();
//                }
//
//                int numAllocation = TAX.numAllocations();
//                int numBytes = TAX.lastAllocatedNumBytes();
//                if (veryVerbose) { T_ P_(numAllocation); T_ P(numBytes); }
//                LOOP_ASSERT(chunkSize, TAY.numAllocations() == numAllocation);
//                LOOP_ASSERT(chunkSize, TAY.lastAllocatedNumBytes()
//                                                                  == numBytes);
//
//                int newChunkSize = growNumBlocks(chunkSize,
//                                                 GROW_FACTOR,
//                                                 MAX_CHUNK_SIZE);
//                if (newChunkSize == chunkSize) ++exceedMaxNumBlocksCount;
//                if (2 < exceedMaxNumBlocksCount) break;
//                chunkSize = newChunkSize;
//            }
//        }

        if (verbose) cout << "\nTesting default max blocks per chunk." << endl;
        {
            bslma_TestAllocator taX(veryVeryVerbose);
            const bslma_TestAllocator& TAX = taX;
            Obj mX(BLOCK_SIZE, bsls_BlockGrowth::BSLS_GEOMETRIC, &taX);

            bslma_TestAllocator taY(veryVeryVerbose);
            const bslma_TestAllocator& TAY = taY;
            Obj mY(BLOCK_SIZE,
                   bsls_BlockGrowth::BSLS_GEOMETRIC,
                   MAX_CHUNK_SIZE,
                   &taY);

            int exceedMaxNumBlocksCount = 0;
            int chunkSize = INITIAL_NUM_BLOCKS;
            while (1) {
                // Allocate until current pool is deplete.
                for (int oi = 0; oi < myAbs(chunkSize); ++oi) {
                    mX.allocate();
                    mY.allocate();
                }

                int numAllocation = TAX.numAllocations();
                int numBytes = TAX.lastAllocatedNumBytes();
                if (veryVerbose) { T_ P_(numAllocation); T_ P(numBytes); }
                LOOP_ASSERT(chunkSize, TAY.numAllocations() == numAllocation);
                LOOP_ASSERT(chunkSize, TAY.lastAllocatedNumBytes()
                                                                  == numBytes);

                int newChunkSize = growNumBlocks(chunkSize,
                                                 GROW_FACTOR,
                                                 MAX_CHUNK_SIZE);
                if (newChunkSize == chunkSize) ++exceedMaxNumBlocksCount;
                if (2 < exceedMaxNumBlocksCount) break;
                chunkSize = newChunkSize;
            }
        }

        if (verbose) cout << "\nTesting default allocator." << endl;
        {
            // Make sure memory is allocated from the default allocator
            bslma_TestAllocator ta;
            bslma_DefaultAllocatorGuard dag(&ta);
            ASSERT(0 == ta.numBytesInUse());

            bdema_Pool p(BLOCK_SIZE);
            ASSERT(0 == ta.numBytesInUse());

            p.allocate();
            ASSERT(0 != ta.numBytesInUse());
        }

      } break;
      case 3: {
        // --------------------------------------------------------------------
        // 'growthStrategy' TEST
        //
        // Concerns:
        //   1. That when constant growth is used, the pool replenishes with
        //      the specified 'maxBlocksPerChunk'.
        //
        //   2. That when geometric growth is used, the pool replenishes with
        //      geometrically increasing chunk size up to the specified
        //      'maxBlocksPerChunk'.
        //
        // Plan:
        //   Using the table driven technique, specify the block size, growth
        //   strategy and maxBlocksPerChunk.  Create a pool with the parameters
        //   from the table, along with a test allocator.  Invoke 'allocate'
        //   repeatedly so that the pool depletes and replenishes.  Verify that
        //   for each replenishment the pool requests memory of the expected
        //   size.
        //
        // Testing:
        //   bdema_Pool(bs, gs, mbps, basicAllocator);
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "'growthStrategy' TEST" << endl
                                  << "=====================" << endl;

        if (verbose) cout << "\nTesting constructor and 'allocate' w/ varying "
                             "positive 'numBlocks'." << endl;

        typedef bsls_BlockGrowth::Strategy St;
        St GEO = bsls_BlockGrowth::BSLS_GEOMETRIC;
        St CON = bsls_BlockGrowth::BSLS_CONSTANT;

        static const struct {
            int d_line;
            int d_blockSize;
            St  d_strategy;
            int d_maxBlocksPerChunk;
        } DATA[] = {
            // LINE     BLOCKSIZE    STRATEGY     MAXBLOCKS
            // ----     ---------    --------     ---------
            // Constant growth
            {  L_,              1,       CON,           16 },
            {  L_,              1,       CON,           30 },
            {  L_,              1,       CON,           32 },
            {  L_,              1,       CON,           48 },
            {  L_,              1,       CON,           64 },

            {  L_,              2,       CON,           16 },
            {  L_,              2,       CON,           30 },
            {  L_,              2,       CON,           32 },
            {  L_,              2,       CON,           48 },
            {  L_,              2,       CON,           64 },

            {  L_,              4,       CON,           16 },
            {  L_,              4,       CON,           30 },
            {  L_,              4,       CON,           32 },
            {  L_,              4,       CON,           48 },
            {  L_,              4,       CON,           64 },

            // Geometric growth
            {  L_,              1,       GEO,           16 },
            {  L_,              1,       GEO,           31 },
            {  L_,              1,       GEO,           32 },
            {  L_,              1,       GEO,           33 },
            {  L_,              1,       GEO,           48 },

            {  L_,              2,       GEO,           16 },
            {  L_,              2,       GEO,           31 },
            {  L_,              2,       GEO,           32 },
            {  L_,              2,       GEO,           33 },
            {  L_,              2,       GEO,           48 },

            {  L_,              4,       GEO,           16 },
            {  L_,              4,       GEO,           31 },
            {  L_,              4,       GEO,           32 },
            {  L_,              4,       GEO,           33 },
            {  L_,              4,       GEO,           48 }
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int LINE       = DATA[ti].d_line;
            const int BLOCK_SIZE = DATA[ti].d_blockSize;
            const St  STRATEGY   = DATA[ti].d_strategy;
            const int MAXBLOCKS  = DATA[ti].d_maxBlocksPerChunk;

            if (veryVerbose) {
                T_ P_(LINE) P_(BLOCK_SIZE)
                if (STRATEGY == GEO) {
                    cout << "STRATEGY = GEOMETRIC_GROWTH, ";
                }
                else {
                    cout << "STRATEGY = CONSTANT_GROWTH, ";
                }
                P(MAXBLOCKS)
            }

            const int POOL_BLOCK_SIZE = poolBlockSize(BLOCK_SIZE);

            bslma_TestAllocator testAllocator(veryVeryVerbose);
            const bslma_TestAllocator& TA = testAllocator;

            BEGIN_BSLMA_EXCEPTION_TEST {
                Obj mX(BLOCK_SIZE, STRATEGY, MAXBLOCKS, &testAllocator);

                int numAllocations = 0;

                // If geometric, grow till constant size first.
                if (GEO == STRATEGY) {
                    int ri = INITIAL_CHUNK_SIZE;

                    while (ri < MAXBLOCKS) {
                        numAllocations = TA.numAllocations();

                        //  Use all memory blocks from current chunk.
                        char *lastAddr = 0;
                        for (int j = 0; j < ri; ++j) {
                            char *addr = (char *)mX.allocate();
                            if (j) {
                                int EXP  = poolBlockSize(BLOCK_SIZE);
                                int size = addr - lastAddr;
                                LOOP2_ASSERT(EXP, size, EXP == size);
                            }
                            lastAddr = addr;
                        }

                        LOOP_ASSERT(ri, TA.numAllocations()
                                                        == numAllocations + 1);
                        ri <<= 1;
                    }

                }

                // Test for constant growth.

                const int NUM_REPLENISH = 3;
                for (int ri = 0; ri < NUM_REPLENISH; ++ri) {
                    numAllocations = TA.numAllocations();

                    // Use all memory blocks from current chunk.
                    char *lastAddr = 0;
                    for (int j = 0; j < MAXBLOCKS; ++j) {
                        char *addr = (char *)mX.allocate();
                        if (j) {
                            int EXP  = poolBlockSize(BLOCK_SIZE);
                            int size = addr - lastAddr;
                            LOOP2_ASSERT(EXP, size, EXP == size);
                        }
                        lastAddr = addr;
                    }

                    LOOP_ASSERT(ri, TA.numAllocations()
                                                    == numAllocations + 1);
                }
            } END_BSLMA_EXCEPTION_TEST
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // CONSTRUCTOR AND ALLOCATE TEST
        //
        // Concerns:
        //   That 'blockSize' constructor will create a 'bdema_Pool' that
        //   return memory blocks having the specified 'blockSize'.
        //
        // Plan:
        //   Initialize a pool with a positive 'numBlocks' and varying block
        //   sizes.  Invoke 'allocate' repeatedly and verify that the
        //   difference between the memory addresses of two consecutive
        //   requests is equal to the expected block size.
        //
        // Testing:
        //   bdema_Pool(bs, gs, mbpc, basicAllocator);
        //   void *allocate();
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "CONSTRUCTOR AND 'allocate' TEST" << endl
                                  << "===============================" << endl;

        if (verbose) cout << "\nTesting 'blockSize' constructor and 'allocate' "
                             "with varying block sizes." << endl;

        const int DATA[] = { 1, 2, 5, 6, 12, 24, 32 };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;
        const int NUM_BLOCKS = 5;
        bslma_TestAllocator testAllocator(veryVeryVerbose);

        for (int di = 0; di < NUM_DATA; ++di) {
            BEGIN_BSLMA_EXCEPTION_TEST {
                const int BLOCK_SIZE = DATA[di];
                Obj mX(BLOCK_SIZE,
                       bsls_BlockGrowth::BSLS_CONSTANT,
                       NUM_BLOCKS,
                       &testAllocator);

                char *lastP = 0;
                for (int oi = 0; oi < NUM_BLOCKS; ++oi) {
                    char *p = (char *) mX.allocate();
                    if (oi) {
                        ASSERT(lastP);

                        int       size = p - lastP;
                        const int EXP  = poolBlockSize(BLOCK_SIZE);

                        if (veryVerbose) { T_ P_(size) T_ P(EXP) }
                        LOOP2_ASSERT(di, oi, EXP == size);
                    }
                    lastP = p;
                }
            } END_BSLMA_EXCEPTION_TEST
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // FILE-STATIC FUNCTION TEST
        //   Make sure that the static functions 'blockSize', 'poolBlockSize'
        //   and 'growNumBlocks' implemented consistently with the
        //   implementation of the pool itself.
        //
        // Plan:
        //   To test 'blockSize', create a 'bdema_InfrequentDeleteBlockList'
        //   object initialized with a test allocator.  Invoke both the
        //   'blockSize' function and the
        //   'bdema_InfrequentDeleteBlockList::allocate' method with varying
        //   memory sizes, and verify that the sizes returned by 'blockSize'
        //   are equal to the sizes recorded by the allocator.
        //
        //   To test 'poolBlockSize', invoke the function with varying sizes,
        //   and verify that the returned value is equal to the difference
        //   between the returned memory addresses of two consecutive requests
        //   (i.e., the size of each returned memory) to a pool initialized
        //   with the current size.
        //
        //   To test 'growNumBlocks', iterate over a set of table-based test
        //   vectors.  Invoke 'growNumBlocks' with the parameters specified in
        //   each test vector and verify that the function returns the expected
        //   value.
        //
        // Testing:
        //   int blockSize(numBytes);
        //   int poolBlockSize(size);
        //   int growNumBlocks(numBlocks, growFactor, maxNumBlocks);
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "FILE-STATIC FUNCTION TEST" << endl
                                  << "=========================" << endl;

        if (verbose) cout << "\nTesting 'blockSize'." << endl;
        {
            const int DATA[] = { 0, 1, 5, 12, 24, 64, 1000 };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            bslma_TestAllocator a(veryVeryVerbose);
            bdema_InfrequentDeleteBlockList bl(&a);
            for (int i = 0; i < NUM_DATA; ++i) {
                const int SIZE = DATA[i];
                int blkSize = blockSize(SIZE);
                bl.allocate(SIZE);

                const int EXP = a.lastAllocatedNumBytes();

                if (veryVerbose) {T_ P_(SIZE); P_(blkSize); P(EXP);}
                LOOP_ASSERT(i, EXP == blkSize);
            }
        }

        if (verbose) cout << "\nTesting 'poolBlockSize'." << endl;
        {
            const int DATA[] = { 1, 2, 5, 6, 12, 24, 32 };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int di = 0; di < NUM_DATA; ++di) {
                const int SIZE = DATA[di];
                Obj mX(SIZE, bsls_BlockGrowth::BSLS_CONSTANT, 2);
                char *p = (char *) mX.allocate();
                char *q = (char *) mX.allocate();
                int EXP = q - p;
                int blockSize = poolBlockSize(SIZE);
                if (veryVerbose) { T_ P_(blockSize); P(EXP); }
                LOOP_ASSERT(di, EXP == blockSize);
            }
        }
#if 0
        if (verbose) cout << "\nTesting 'growNumBlocks'." << endl;
        {
            struct {
                int d_line;
                int d_numBlocks;
                int d_growFactor;
                int d_maxNumBlocks;
                int d_exp;
            } DATA[] = {
          //----^
          //line    Num.       Grow      Maximum         Expected
          //no.     Blocks     Factor    Num. Blocks     Value
          //----    -------    ------    ------------    --------
          { L_,       -1,         2,         -1,             1    },
          { L_,       -1,         2,         -2,            -2    },
          { L_,       -1,         2,         -3,            -2    },
          { L_,       -1,         4,         -3,            -4    },
          { L_,       -1,         4,         -4,            -4    },
          { L_,       -1,         4,         -5,            -4    },

          { L_,       -5,         2,         -4,             5    },
          { L_,       -5,         2,         -5,             5    },
          { L_,       -5,         2,         -9,           -10    },
          { L_,       -5,         2,        -10,           -10    },
          { L_,       -5,         2,        -11,           -10    },
          { L_,       -5,         4,        -19,           -20    },
          { L_,       -5,         4,        -20,           -20    },
          { L_,       -5,         4,        -21,           -20    },
          //----v
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;
            for (int di = 0; di < NUM_DATA; ++di) {
                const int LINE = DATA[di].d_line;
                const int NUM_BLOCKS = DATA[di].d_numBlocks;
                const int GROW_FACTOR = DATA[di].d_growFactor;
                const int MAX_NUM_BLOCKS = DATA[di].d_maxNumBlocks;
                const int EXP = DATA[di].d_exp;
                int numBlocks = growNumBlocks(NUM_BLOCKS,
                                              GROW_FACTOR,
                                              MAX_NUM_BLOCKS);
                if (veryVerbose) { T_ P_(numBlocks); T_ P(EXP); }
                LOOP2_ASSERT(LINE, di, EXP == numBlocks);
            }
        }
#endif
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
//      Copyright (C) Bloomberg L.P., 2004
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
