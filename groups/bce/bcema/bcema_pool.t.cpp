// bcema_pool.t.cpp                                                   -*-C++-*-

#include <bcema_pool.h>

#include <bcemt_barrier.h>
#include <bcemt_thread.h>
#include <bcemt_threadgroup.h>
#include <bcemt_qlock.h>
#include <bdef_bind.h>
#include <bdema_infrequentdeleteblocklist.h>
#include <bslma_testallocator.h>          // for testing only
#include <bslma_testallocatorexception.h> // for testing only
#include <bsls_alignmentutil.h>
#include <bsls_platform.h>

#include <bsl_cmath.h>       // log()
#include <bsl_cstdlib.h>     // atoi()
#include <bsl_vector.h>
#include <bsl_iostream.h>
#include <bsl_cstring.h>     // memcpy()

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

//=============================================================================
//                                  TEST PLAN
//-----------------------------------------------------------------------------
//                                  Overview
//                                  --------
// The goals of this 'bcema_Pool' test suite are to verify that 1) the
// 'allocate' method distributes memory of the correct object size; 2) the pool
// replenishes correctly according to the 'numObjects' parameter; 3) the
// 'deallocate' method returns the memory to the pool; and 4) the 'release'
// method and the destructor releases all memory allocated through the pool.
//
// To achieve goal 1, initialize pools of varying object sizes.  Invoke
// 'allocate' repeatedly and verify that the difference between the returned
// memory addresses of two consecutive requests is equal to the specified
// object size for the current pool.  To achieve goal 2, initialize a pool with
// a test allocator and varying 'numObjects'.  Invoke 'allocate' repeatedly and
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
// [ 5] bcema_Pool(objectSize, basicAllocator);
// [ 2] bcema_Pool(int, strategy, int, allocator) : BLOCK SIZE
// [ 3] bcema_Pool(int, strategy, int, allocator) : CONSTANT GROWTH STRATEGY
// [ 4] bcema_Pool(int, strategy, int, allocator) : GEOMETRIC GROWTH STRATEGY
// [11] bcema_Pool(int, strategy, allocator);
// [12] bcema_Pool(int, int, allocator);
// [ 7] ~bcema_Pool();
// [ 2] void *allocate();
// [ 6] void deallocate(address);
// [10] void deleteObject(const TYPE *object);
// [10] void deleteObjectRaw(const TYPE *object);
// [ 7] void release();
// [ 8] void reserveCapacity(int numObjects);
// [ 9] template<typename TYPE> void deleteObject(TYPE *object)
//-----------------------------------------------------------------------------
// [16] USAGE EXAMPLE
// [15] OLD USAGE EXAMPLE
// [14] PERFORMANCE TEST
// [13] CONCURRENCY TEST
// [ 1] int blockSize(numBytes);
// [ 1] int poolObjectSize(size);
//=============================================================================
//                    STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------

static int testStatus = 0;

static bcemt_QLock coutMutex;

static void rEpOrT(const char *s, int i) {
    cout << "Error " << __FILE__ << "(" << i << "): " << s
         << "    (failed)" << endl;
    if (testStatus >= 0 && testStatus <= 100) ++testStatus;
}

static void aSsErT(int c, const char *s, int i) {
    if (c) {
        bcemt_QLockGuard guard(&coutMutex);
        rEpOrT(s, i);
    }
}
# define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

//=============================================================================
//                  STANDARD BDE LOOP-ASSERT TEST MACROS
//-----------------------------------------------------------------------------

#define LOOP_ASSERT(I,X) { \
    if (!(X)) { bcemt_QLockGuard guard(&coutMutex);        \
                cout << #I << ": " << I << "\n";           \
                rEpOrT(#X, __LINE__); }}

#define LOOP2_ASSERT(I,J,X) { \
    if (!(X)) { bcemt_QLockGuard guard(&coutMutex);                          \
                cout << #I << ": " << I << "\t" << #J << ": " << J << "\n";  \
                rEpOrT(#X, __LINE__); }}

#define LOOP3_ASSERT(I,J,K,X) { \
   if (!(X)) { bcemt_QLockGuard guard(&coutMutex);                           \
               cout << #I << ": " << I << "\t" << #J << ": " << J << "\t"    \
                    << #K << ": " << K << "\n";                              \
               rEpOrT(#X, __LINE__); }}

#define LOOP4_ASSERT(I,J,K,L,X) { \
   if (!(X)) { bcemt_QLockGuard guard(&coutMutex);                           \
               cout << #I << ": " << I << "\t" << #J << ": " << J << "\t"    \
                    << #K << ": " << K << "\t" << #L << ": " << L << "\n";   \
               rEpOrT(#X, __LINE__); }}

//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------

#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", " << flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number
#define TAB cout << '\t';
#define T_()  cout << "\t" << flush;          // Print a tab (w/o newline)

//=============================================================================
//                   GLOBAL TYPEDEFS, CONSTANTS, AND VARIABLES
//-----------------------------------------------------------------------------

typedef bcema_Pool Obj;

static int verbose;
static int veryVerbose;
static int veryVeryVerbose;

// This type is copied from the 'bdema_infrequentdeleteblocklist.h' for testing
// purposes.

struct InfrequentDeleteBlock {
    InfrequentDeleteBlock          *d_next_p;
    bsls_AlignmentUtil::MaxAlignedType  d_memory;  // force alignment
};

// This type is copied from 'bcema_pool.cpp' to determine the internal limits
// of 'bcema_Pool'.
enum {
    INITIAL_CHUNK_SIZE   =  1,
    GROW_FACTOR          =  2,
    MAX_BLOCKS_PER_CHUNK =  32
};

int numLeftChildren   = 0;
int numMiddleChildren = 0;
int numRightChildren  = 0;
int numMostDerived    = 0;

struct LeftChild {
    int d_li;
    LeftChild()           { ++numLeftChildren; }
    virtual ~LeftChild()  { --numLeftChildren; }
};

struct MiddleChild {    // non-polymorphic middle child
    int d_mi;
    MiddleChild()         { ++numMiddleChildren; }
    ~MiddleChild()        { --numMiddleChildren; }
};

struct RightChild {
    int d_ri;
    RightChild()          { ++numRightChildren; }
    virtual ~RightChild() { --numRightChildren; }
};

struct MostDerived : LeftChild, MiddleChild, RightChild {
    int d_md;
    MostDerived()         { ++numMostDerived; }
    ~MostDerived()        { --numMostDerived; }
};

//=============================================================================
//                      FILE-STATIC FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

struct LLink {
    // Note that this type is copied from 'bcema_pool.h'.

    union {
        bces_AtomicUtil::Int               d_refCount;
        bsls_AlignmentUtil::MaxAlignedType d_dummy;
    };
    LLink *d_next_p;
};

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

inline static int roundUp(int x, int y)
    // Round up the specified 'x' to the nearest multiples of the specified
    // 'y'.  The behavior is undefined unless 0 <= x and 0 < y;
{
    ASSERT(0 <= x);
    ASSERT(0 < y);
    return (x + y - 1) / y * y;
}

inline static int poolObjectSize(int size)
    // Return the actual object size used by the pool when given the specified
    // 'size'.
{
    const int HEADER_SIZE = offsetof(LLink, d_next_p);
    return roundUp(size + HEADER_SIZE < (int)sizeof(LLink)
                   ? sizeof(LLink) : size + HEADER_SIZE,
                   bsls_AlignmentUtil::BSLS_MAX_ALIGNMENT);
}

static void scribble(char *address, int size)
    // Assign a non-zero value to each of the specified 'size' bytes starting
    // at the specified 'address'.
{
    memset(address, 0xff, size);
}

void stretch(Obj *object, int numElements)
   // Using only primary manipulators, extend the capacity of the specified
   // 'object' to (at least) the specified 'numElements'.  The behavior is
   // undefined unless 0 <= numElements and 0 <= objSize.
{
    ASSERT(object);
    ASSERT(0 <= numElements);

    for (int i = 0; i < numElements; ++i) {
        object->allocate();
    }
}

void stretchRemoveAll(Obj *object, int numElements)
   // Using only primary manipulators, extend the capacity of the specified
   // 'object' to (at least) the specified 'numElements', then remove all
   // elements leaving 'object' empty.  The behavior is undefined unless 0 <=
   // numElements and 0 <= objSize.
{
    ASSERT(object);
    ASSERT(0 <= numElements);

    stretch(object, numElements);
    object->release();
}

//=============================================================================
//                               USAGE EXAMPLE
//-----------------------------------------------------------------------------

///Usage
///-----
// A 'bcema_Pool' can be used by node-based containers (such as lists, trees,
// and hash tables that hold multiple elements of uniform size) for efficient
// memory allocation of new elements.  The following container class,
// 'my_PooledArray', stores templatized values "out-of-place" as nodes in a
// 'vector' of pointers.  Since the size of each node is fixed and known *a
// priori*, the class uses a 'bcema_Pool' to allocate memory for the nodes to
// improve memory allocation efficiency:
//..
    // my_poolarray.h
//
    template <class T>
    class my_PooledArray {
        // This class implements a container that stores 'double' values
        // out-of-place.
//
        // DATA
        bsl::vector<T *> d_array_p;  // array of pooled elements
        bcema_Pool       d_pool;     // memory manager for array elements
//
      public:
        // CREATORS
        my_PooledArray(bslma_Allocator *basicAllocator = 0);
            // Create a pooled array that stores the parameterized values
            // "out-of-place".  Optionally specify a 'basicAllocator' used to
            // supply memory.  If 'basicAllocator' is 0, the currently
            // installed default allocator is used.
//
        ~my_PooledArray();
            // Destroy this array and all elements held by it.
//
        // MANIPULATORS
        void append(const T &value);
            // Append the specified 'value' to this array.
//
        void removeAll();
            // Remove all elements from this array.
//
        // ACCESSORS
        int length() const;
            // Return the number of elements in this array.
//
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
//
    // ACCESSORS
    template <class T>
    inline
    int my_PooledArray<T>::length() const
    {
        return d_array_p.size();
    }
//
    template <class T>
    inline
    const T& my_PooledArray<T>::operator[](int index) const
    {
        ASSERT(0 <= index);
        ASSERT(index < length());
//
        return *d_array_p[index];
    }
//..
// Note that the growth strategy and maximum chunk size of the pool is left as
// the default value:
//..
    // my_poolarray.cpp
// #include <my_poolarray.h>
//
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
//                               OLD  USAGE EXAMPLE
//-----------------------------------------------------------------------------
// my_doublearray2.h

class my_DoubleArray2 {
    double         **d_array_p;     // dynamically allocated array
    int              d_size;        // physical capacity of this array
    int              d_length;      // logical length of this array
    bcema_Pool       d_pool;        // memory manager for array elements
    bslma_Allocator *d_allocator_p; // holds (but does not own) allocator

  private:
    void increaseSize();

  public:
    my_DoubleArray2(bslma_Allocator *basicAllocator);
    ~my_DoubleArray2();

    void append(double item);
    void removeAll();

    int length() const                        { return d_length; }
    const double& operator[](int index) const { return *d_array_p[index];}
};

inline
void my_DoubleArray2::removeAll()
{
    d_pool.release();
    d_length = 0;
}

ostream& operator<<(ostream& stream, const my_DoubleArray2& array);

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// my_doublearray2.cpp

enum {
    MY_INITIAL_SIZE = 1, // initial physical capacity
    MY_GROW_FACTOR = 2   // multiplicative factor by which to grow 'd_size'
};

inline
static int nextSize(int size)
{
    return size * MY_GROW_FACTOR;
}

inline
static void reallocate(double        ***array,
                       int             *size,
                       int              newSize,
                       int              length,
                       bslma_Allocator *basicAllocator)
    // Reallocate memory in the specified 'array' using the specified
    // 'basicAllocator' and update the specified size to the specified
    // 'newSize'.  The specified 'length' number of leading elements are
    // preserved.  If 'new' should throw an exception, this function has no
    // effect.  The behavior is undefined unless 1 <= newSize, 0 <= length,
    // and newSize <= length.
{
    ASSERT(array);
    ASSERT(*array);
    ASSERT(size);
    ASSERT(1 <= newSize);
    ASSERT(0 <= length);
    ASSERT(basicAllocator);
    ASSERT(length <= *size);    // sanity check
    ASSERT(length <= newSize);  // ensure class invariant

    double **tmp = *array;
    *array = (double **) basicAllocator->allocate(newSize * sizeof **array);
    memcpy(*array, tmp, length * sizeof **array);
    basicAllocator->deallocate(tmp);
    *size = newSize;
}

void my_DoubleArray2::increaseSize()
{
    reallocate(&d_array_p, &d_size, nextSize(d_size),
               d_length, d_allocator_p);
}

// CREATORS
my_DoubleArray2::my_DoubleArray2(bslma_Allocator *basicAllocator)
: d_size(MY_INITIAL_SIZE)
, d_length(0)
, d_pool(sizeof(double), basicAllocator)
, d_allocator_p(basicAllocator)
{
    ASSERT(d_allocator_p);
    d_array_p =
        (double **) d_allocator_p->allocate(d_size * sizeof *d_array_p);
}

my_DoubleArray2::~my_DoubleArray2()
{
    ASSERT(d_array_p);
    ASSERT(1 <= d_size);
    ASSERT(0 <= d_length);
    ASSERT(d_allocator_p);
    ASSERT(d_length <= d_size);

    // Elements are automatically deallocated when 'd_pool' is destroyed.
    d_allocator_p->deallocate(d_array_p);
}

void my_DoubleArray2::append(double item)
{
    if (d_length >= d_size) {
        increaseSize();
    }
    d_array_p[d_length++] = new(d_pool.allocate()) double(item);
}

ostream& operator<<(ostream& stream, const my_DoubleArray2& array)
{
    stream << "[ ";
    for (int i = 0; i < array.length(); ++i) {
        stream << array[i] << " ";
    }
    return stream << ']' << flush;
}

//=============================================================================
//                CONCRETE OBJECTS FOR TESTING 'deleteObject'
//-----------------------------------------------------------------------------

static int my_ClassCode = 0;

class my_Class1 {
  public:
    my_Class1()  { my_ClassCode = 1; }
    ~my_Class1() { my_ClassCode = 2; }
};

class my_Class2 {
  public:
    my_Class2()  { my_ClassCode = 3; }
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
//                      HELPER FUNCTION FOR CONCURRENCY TEST
//-----------------------------------------------------------------------------

enum {
    OBJECT_SIZE = 56,
    NUM_INTS = OBJECT_SIZE / sizeof(int),
    NUM_OBJECTS = 10000,
    NUM_THREADS = 4
};

bcemt_Barrier barrier(NUM_THREADS);
extern "C"
void *workerThread(void *arg) {
    Obj *mX = (Obj *) arg;
    ASSERT(OBJECT_SIZE == mX->blockSize());

    barrier.wait();
    for (int i = 0; i < NUM_OBJECTS; ++i) {
        volatile int *buffer = (int*)mX->allocate();
        if (veryVeryVerbose) {
            printf("Thread %d: Allocated %p\n", bcemt_ThreadUtil::self(),
                   (void *)buffer);
        }
        LOOP_ASSERT(i, (void*)buffer != (void*)0xAB);
        LOOP_ASSERT(i, buffer);
        *buffer = 0xAB;
        LOOP_ASSERT(i, (void*)buffer != (void*)0xAB);
        mX->deallocate((void*)buffer);
        LOOP_ASSERT(i, (void*)buffer != (void*)0xAB);
    }
    return arg;
}

//=============================================================================
//                     BENCHMARKS
//-----------------------------------------------------------------------------
namespace bench {

struct Item {
    int  d_threadId;
};

struct Control {
    bcemt_Barrier         *d_barrier;
    bcema_Pool            *d_pool;
    int                    d_iterations;
    int                    d_numObjects;
};

void bench(Control *control)
{
    int threadId = bcemt_ThreadUtil::selfIdAsInt();

    bcema_Pool *pool = control->d_pool;
    int numObjects = control->d_numObjects;

    bsl::vector<Item *> objects(numObjects, (Item *)0);

    control->d_barrier->wait();

    for (int i=0; i<control->d_iterations; i++) {
        for(int j=0; j<numObjects; j++) {
            for(int t=0; t<=j; t++) {
                Item *item = (Item *)pool->allocate();
                ASSERT(item);
                item->d_threadId = threadId;
                objects[t] = item;
            }
            for(int t=0; t<=j; t++) {
                Item *item = objects[t];
                ASSERT(item->d_threadId == threadId);
                pool->deallocate(item);
            }
        }
    }
}

void runtest(int numIterations, int numObjects, int numThreads)
{
    bcema_Pool pool(sizeof(Item), numThreads * numObjects);

    bcemt_Barrier barrier(numThreads);

    Control control;

    control.d_barrier = &barrier;
    control.d_pool = &pool;
    control.d_iterations = numIterations;
    control.d_numObjects = numObjects;

    bcemt_ThreadGroup tg;
    tg.addThreads(bdef_BindUtil::bind(&bench,&control), numThreads);

    tg.joinAll();
}
}

//=============================================================================
//                                MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[]) {

    int test = argc > 1 ? atoi(argv[1]) : 0;
    verbose = argc > 2;
    veryVerbose = argc > 3;
    veryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:
      case 16: {
        // --------------------------------------------------------------------
        // USAGE TEST
        //
        //   Test the usage example.  Create a 'my_PooledArray<double>'
        //   object and append varying values to it.  Verify that the values
        //   are correctly appended using 'operator[]'.  Invoke 'removeAll'
        //   and verify that the array length becomes 0.
        //
        // Testing:
        //   USAGE TEST - Make sure main usage example compiles and works.
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "USAGE TEST" << endl
                                  << "==========" << endl;

        if (verbose) cout << "\nTesting 'my_PooledArray<double>'." << endl;

        const double DATA[] = { 0.0, 1.2, 2.3, 3.4, 4.5, 5.6, 6.7 };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        bslma_TestAllocator a;
        my_PooledArray<double> array(&a);

        for (int i = 0; i < NUM_DATA; ++i) {
            const double VALUE = DATA[i];
            array.append(VALUE);
            LOOP_ASSERT(i, i + 1 == array.length());
            LOOP_ASSERT(i, VALUE == array[i]);
        }
        array.removeAll();
        ASSERT(0 == array.length());
      } break;
      case 15: {
        // --------------------------------------------------------------------
        // ORIGINAL USAGE TEST
        //
        //   Test the old (removed) usage example.  Create a 'my_DoubleArray2'
        //   object and append varying values to it.  Verify that the values
        //   are correctly appended using 'operator[]'.  Invoke 'removeAll'
        //   and verify that the array length becomes 0.
        //
        // Testing:
        //   USAGE TEST - Make sure main usage example compiles and works.
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "USAGE TEST" << endl
                                  << "==========" << endl;

        if (verbose) cout << "\nTesting 'my_DoubleArray2'." << endl;

        const double DATA[] = { 0.0, 1.2, 2.3, 3.4, 4.5, 5.6, 6.7 };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        bslma_TestAllocator a;
        my_DoubleArray2 array(&a);

        for (int i = 0; i < NUM_DATA; ++i) {
            const double VALUE = DATA[i];
            array.append(VALUE);
            LOOP_ASSERT(i, i + 1 == array.length());
            LOOP_ASSERT(i, VALUE == array[i]);
        }
        if (veryVerbose) { cout << '\t' << array << endl; }
        array.removeAll();
        ASSERT(0 == array.length());
      } break;
      case 14: {
        // ---------------------------------------------------------
        // BENCHMARK
        // ---------------------------------------------------------
        if (verbose) cout << endl
                          << "Benchmark" << endl
                          << "========================" << endl;
        enum {
            NUM_THREADS = 4,
            NUM_ITERATIONS = 500,
            NUM_OBJECTS = 50
        };

        int numIterations = NUM_ITERATIONS;
        int numObjects = NUM_OBJECTS;

        for(int numThreads=1; numThreads<=NUM_THREADS; numThreads++) {

            bench::runtest(numIterations, numObjects, numThreads);
        }

      } break;
      case 13: {
        // --------------------------------------------------------------------
        // CONCURRENCY TEST
        //
        // Testing:
        //     Thread-safety of allocate/deallocate methods.
        // --------------------------------------------------------------------
        bcemt_ThreadUtil::Handle threads[NUM_THREADS];
        Obj mX(OBJECT_SIZE);
        for (int i = 0; i < NUM_THREADS; ++i) {
            int rc =
                bcemt_ThreadUtil::create(&threads[i], workerThread, &mX);
            LOOP_ASSERT(i, 0 == rc);
        }
        for (int i = 0; i < NUM_THREADS; ++i) {
            int rc =
                bcemt_ThreadUtil::join(threads[i]);
            LOOP_ASSERT(i, 0 == rc);
        }
      } break;
      case 12: {
        // --------------------------------------------------------------------
        // TESTING  Alternative (deprecated) constructor
        //
        // Concerns:
        //   That the alternative 'bcema_Pool'  constructor uses the correct
        //   default argument values for the unspecified parameters.
        //
        // Plan:
        //   Create one pool using the alternative constructor, and one pool
        //   using the primary constructor with the correct default argument
        //   values, and verify they behave the same.
        //
        // Testing:
        //   bcema_Pool(int, int,  bslma_Allocator);
        // --------------------------------------------------------------------

        if (verbose) cout << endl << " bcema_Pool(int, int, ...)" << endl
                                  << "===============================" << endl;

        struct {
            int  d_line;
            int  d_objectSize;
            int  d_maxBlocksPerChunk;
            bool d_geometric;
        } DATA[] = {
            //line    object                          geometric
            //no.     size      maxBlocksPerChunk            growth
            //----    ------    --------------------  ------
            { L_,       1,                         5, false },
            { L_,       5,                        10, false },
            { L_,      12,                         1, false },
            { L_,      24,                         5, false },
            { L_,      32,      MAX_BLOCKS_PER_CHUNK, false },
            { L_,       1,                         5,  true },
            { L_,       5,                        10,  true },
            { L_,      12,                         1,  true },
            { L_,      24,                         5,  true },
            { L_,      32,      MAX_BLOCKS_PER_CHUNK,  true }
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        const int NUM_REQUESTS = 100;
        bslma_TestAllocator taX;    const bslma_TestAllocator& TAX   = taX;
        bslma_TestAllocator taExp;  const bslma_TestAllocator& TAEXP = taExp;

        for (int di = 0; di < NUM_DATA; ++di) {
            bsls_BlockGrowth::Strategy strategy =
                DATA[di].d_geometric
                  ? bsls_BlockGrowth::BSLS_GEOMETRIC
                  : bsls_BlockGrowth::BSLS_CONSTANT;

            const int LINE = DATA[di].d_line;
            const int OBJECT_SIZE      = DATA[di].d_objectSize;
            const int BLOCKS_PER_CHUNK = DATA[di].d_maxBlocksPerChunk;
            const int NUM_OBJECTS      =
                                 (strategy == bsls_BlockGrowth::BSLS_CONSTANT)
                                 ? BLOCKS_PER_CHUNK
                                 : -BLOCKS_PER_CHUNK;
            {
                Obj mX(OBJECT_SIZE, NUM_OBJECTS, &taX);
                Obj mExp(OBJECT_SIZE, strategy, BLOCKS_PER_CHUNK, &taExp);

                for (int ai = 0; ai < NUM_REQUESTS; ++ai) {
                    mX.allocate();
                    mExp.allocate();
                }

                int numAllocations = TAX.numAllocations();
                int numBytes       = TAX.lastAllocatedNumBytes();
                if (veryVerbose) { TAB; P_(numAllocations); TAB; P(numBytes); }
                LOOP2_ASSERT(numAllocations,
                             TAEXP.numAllocations(),
                             TAEXP.numAllocations() == numAllocations);
                LOOP2_ASSERT(numBytes,
                             TAEXP.lastAllocatedNumBytes(),
                             TAEXP.lastAllocatedNumBytes() == numBytes);

            }
        }
      } break;
      case 11: {
        // --------------------------------------------------------------------
        // TESTING  Alternative constructor
        //
        // Concerns:
        //   That the alternative 'bcema_Pool'  constructor uses the correct
        //   default argument values for the unspecified parameters.
        //
        // Plan:
        //   Create one pool using the alternative constructor, and one pool
        //   using the primary constructor with the correct default argument
        //   values, and verify they behave the same.
        //
        // Testing:
        //   bcema_Pool(int, bsls_BlockGrowth::Strategy, bslma_Allocator);
        // --------------------------------------------------------------------

        if (verbose) cout << endl << " bcema_Pool(int, Strategy, ...)" << endl
                                  << "===============================" << endl;

        struct {
            int  d_line;
            int  d_objectSize;
            bool d_geometric;
        } DATA[] = {
            //line    object    geometric
            //no.     size      growth
            //----    ------    ------
            { L_,       1,      false },
            { L_,       5,      false },
            { L_,      12,      false },
            { L_,      24,      false },
            { L_,      32,      false },
            { L_,       1,       true },
            { L_,       5,       true },
            { L_,      12,       true },
            { L_,      24,       true },
            { L_,      32,       true }
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        const int NUM_REQUESTS = 100;
        bslma_TestAllocator taX;    const bslma_TestAllocator& TAX   = taX;
        bslma_TestAllocator taExp;  const bslma_TestAllocator& TAEXP = taExp;

        for (int di = 0; di < NUM_DATA; ++di) {
            const int LINE = DATA[di].d_line;
            const int OBJECT_SIZE = DATA[di].d_objectSize;
            bsls_BlockGrowth::Strategy strategy =
                DATA[di].d_geometric
                  ? bsls_BlockGrowth::BSLS_GEOMETRIC
                  : bsls_BlockGrowth::BSLS_CONSTANT;
            {

                Obj mX(OBJECT_SIZE,   strategy, &taX);
                Obj mExp(OBJECT_SIZE, strategy, MAX_BLOCKS_PER_CHUNK, &taExp);

                for (int ai = 0; ai < NUM_REQUESTS; ++ai) {
                    mX.allocate();
                    mExp.allocate();
                }

                int numAllocations = TAX.numAllocations();
                int numBytes       = TAX.lastAllocatedNumBytes();
                if (veryVerbose) { TAB; P_(numAllocations); TAB; P(numBytes); }
                LOOP2_ASSERT(numAllocations,
                             TAEXP.numAllocations(),
                             TAEXP.numAllocations() == numAllocations);
                LOOP2_ASSERT(numBytes,
                             TAEXP.lastAllocatedNumBytes(),
                             TAEXP.lastAllocatedNumBytes() == numBytes);

            }
        }

      } break;
      case 10: {
        // --------------------------------------------------------------------
        // TEST: 'deleteObject' AND 'deleteObjectRaw'
        //
        // Concerns:
        //   That 'deleteObject' and 'deleteObjectRaw' properly destroy and
        //   deallocate managed objects.
        //
        // Plan:
        //   Iterate where at the beginning of the loop, we create an object
        //   of type 'mostDerived' that multiply inherits from two types with
        //   virtual destructors.  Then in the middle of the loop we switch
        //   into several ways of destroying and deallocating the object with
        //   various forms of 'deleteObjectRaw' and 'deleteObject', after
        //   which we verify that the destructors have been run.  Each
        //   iteration we verify that the memory we got was the same as for
        //   the previous iteration, which shows that memory is being
        //   deallocated and recovered by the pool.
        //
        // Testing:
        //   deleteObjectRaw()
        //   deleteObject()
        // --------------------------------------------------------------------

        bslma_TestAllocator alloc, *Z = &alloc;

        bool finished = false;
        const MostDerived *repeater = 0;    // verify we're re-using the memory
                                            // each time
        Obj pool(sizeof(MostDerived), bsls_BlockGrowth::BSLS_CONSTANT, 10, Z);
        for (int di = 0; !finished; ++di) {
            MostDerived *pMD = (MostDerived *) pool.allocate();
            const MostDerived *pMDC = pMD;

            if (!repeater) {
                repeater = pMDC;
            }
            else {
                // this verifies that we are freeing the memory each iteration
                // because we get the same pointer every time we allocate, and
                // we allocate one extra time at the end
                LOOP_ASSERT(di, repeater == pMDC);
            }
            new (pMD) MostDerived();

            ASSERT(1 == numLeftChildren);
            ASSERT(1 == numMiddleChildren);
            ASSERT(1 == numRightChildren);
            ASSERT(1 == numMostDerived);

            switch (di) {
              case 0: {
                pool.deleteObjectRaw(pMDC);
              } break;
              case 1: {
                const LeftChild *pLCC = pMDC;
                ASSERT((void*) pLCC == (void*) pMDC);
                pool.deleteObjectRaw(pLCC);
              } break;
              case 2: {
                pool.deleteObject(pMDC);
              } break;
              case 3: {
                const LeftChild *pLCC = pMDC;
                ASSERT((void*) pLCC == (void*) pMDC);
                pool.deleteObject(pLCC);
              } break;
              case 4: {
                const RightChild *pRCC = pMDC;
                ASSERT((void*) pRCC != (void*) pMDC);
                pool.deleteObject(pRCC);
              } break;
              case 5: {
                pool.deleteObjectRaw(pMDC);    // 2nd time we do this

                finished = true;
              } break;
              default: {
                ASSERT(0);
              }
            }

            LOOP_ASSERT(di, 0 == numLeftChildren);
            LOOP_ASSERT(di, 0 == numMiddleChildren);
            LOOP_ASSERT(di, 0 == numRightChildren);
            LOOP_ASSERT(di, 0 == numMostDerived);
        }
      } break;
      case 9: {
        // --------------------------------------------------------------------
        // TEST: 'deleteObject'
        //   We want to make sure that when 'deleteObject' is used both
        //   destructor and 'deallocate' are invoked.
        //
        // Plan:
        //   Using a pool and placement new operator construct objects of
        //   two different classes.  Invoke 'deleteObject' to delete
        //   constructed objects and check that both destructor and
        //   'deallocate' have been called.
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

            const int OBJECT_SIZE = sizeof(my_Class1);
            ASSERT(sizeof(my_Class2) == OBJECT_SIZE);
            const int NUM_OBJECTS = 1;
            Obj mX(OBJECT_SIZE,
                   bsls_BlockGrowth::BSLS_CONSTANT,
                   NUM_OBJECTS,
                   &a);

            if (verbose) cout << "\twith a my_Class1 object" << endl;

            my_ClassCode=0;

            my_Class1 *pC1 = (my_Class1 *) mX.allocate();
            new(pC1) my_Class1;
            if (verbose) { T_();  T_();  P(my_ClassCode); }
            ASSERT(1 == my_ClassCode);
            ASSERT(A.numAllocations() == 1);

            mX.deleteObject(pC1);
            if (verbose) { T_();  T_();  P(my_ClassCode); }
            ASSERT(2 == my_ClassCode);
            ASSERT(A.numAllocations() == 1);
            mX.allocate();
            ASSERT(A.numAllocations() == 1);
                  // By observing that the number of allocations
                  // stays at one we confirm that the memory obtained
                  // from the pool has been returned by 'deleteObject'.
                  // Had it not been returned, the call to allocate would
                  // have required another allocation from the allocator.

            if (verbose) cout << "\twith a my_Class2 object" << endl;

            my_Class2 *pC2 = (my_Class2 *) mX.allocate();
            new(pC2) my_Class2;
            if (verbose) { T_();  T_();  P(my_ClassCode); }
            ASSERT(3 == my_ClassCode);
            ASSERT(A.numAllocations() == 2);

            mX.deleteObject(pC2);
            if (verbose) { T_();  T_();  P(my_ClassCode); }
            ASSERT(4 == my_ClassCode);
            ASSERT(A.numAllocations() == 2);
            mX.allocate();
            ASSERT(A.numAllocations() == 2);
        }

        if (verbose) cout << "\nTesting 'deleteObject' on polymorphic types:"
                          << endl;
        {
            bslma_TestAllocator a(veryVeryVerbose);

            const int OBJECT_SIZE = sizeof(my_MostDerived);
            ASSERT(sizeof(my_MostDerived) == OBJECT_SIZE);
            const int NUM_OBJECTS = 1;
            Obj mX(OBJECT_SIZE,
                   bsls_BlockGrowth::BSLS_CONSTANT,
                   NUM_OBJECTS,
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
        // TEST: reserviceCapacity
        //
        // Testing:
        //   void reserveCapacity(numObjects);
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "RESERVECAPACITY TEST" << endl
                                  << "====================" << endl;

        if (verbose) cout << "\nTesting 'reserveCapacity'." << endl;

        const int RESERVED[] = {
            0, 1, 2, 3, 4, 5, 15, 16, 17
        };
        const int NUM_RESERVED = sizeof RESERVED / sizeof *RESERVED;

        const int EXTEND[] = {
            0, 1, 4, 5, 7, 17, 23, 100
        };
        const int NUM_EXTEND = sizeof EXTEND / sizeof *EXTEND;

        bsls_BlockGrowth::Strategy STRATEGIES[] = {
            bsls_BlockGrowth::BSLS_CONSTANT,
            bsls_BlockGrowth::BSLS_GEOMETRIC
        };
        const int NUM_STRATEGIES = sizeof STRATEGIES / sizeof *STRATEGIES;

        static const int BLOCK_SIZES[] = {
            bsls_AlignmentUtil::BSLS_MAX_ALIGNMENT,
            bsls_AlignmentUtil::BSLS_MAX_ALIGNMENT * 2,
            bsls_AlignmentUtil::BSLS_MAX_ALIGNMENT * 3,
            bsls_AlignmentUtil::BSLS_MAX_ALIGNMENT * 4,
        };
        const int NUM_BLOCK_SIZES = sizeof BLOCK_SIZES / sizeof *BLOCK_SIZES;

        bslma_TestAllocator a;    const bslma_TestAllocator& A = a;
        for (int si = 0; si < NUM_STRATEGIES; ++si) {
            for (int bsi = 0; bsi < NUM_BLOCK_SIZES; ++bsi) {
                for (int ri = 0; ri < NUM_RESERVED; ++ri) {
                    for (int ei = 0; ei < NUM_EXTEND; ++ei) {
                        const int BLOCK_SIZE = BLOCK_SIZES[bsi];
                        const int NUM_BLOCKS = RESERVED[ri];
                        const int EXTEND_SZ  = EXTEND[ei];
                        const bsls_BlockGrowth::Strategy STRATEGY =
                                                                STRATEGIES[si];

                        // Add 'EXTEND' elements to mX, and add 'EXTEND'
                        // elements to mY and then remove those elements.
                        Obj mX(BLOCK_SIZE, STRATEGY, &a);
                        Obj mY(BLOCK_SIZE, STRATEGY, &a);

                        stretch(&mX, EXTEND_SZ);
                        stretchRemoveAll(&mX, EXTEND_SZ);

                        mX.reserveCapacity(NUM_BLOCKS);
                        mY.reserveCapacity(NUM_BLOCKS);
                        const int ALLOC_BLOCKS = A.numBlocksTotal();
                        const int ALLOC_BYTES  = A.numBytesInUse();

                        for (int i = 0; i < NUM_BLOCKS; ++i) {
                            mX.allocate();
                            mY.allocate();
                        }
                        LOOP4_ASSERT(si, bsi, ri, ei,
                                     ALLOC_BLOCKS == A.numBlocksTotal());
                        LOOP4_ASSERT(si, bsi, ri, ei,
                                 ALLOC_BYTES  == A.numBytesInUse());
                    }
                }
            }
        }
        ASSERT(0 == A.numBytesInUse());

      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TEST: release
        //   Initialize two pools with varying object sizes and 'numObjects',
        //   and supply each with its own test allocator.  Invoke 'allocate'
        //   repeatedly.  Invoke 'release' on one pool, and allow the other
        //   pool to go out of scope.  Verify that both allocators indicate all
        //   memory has been released by the pools.
        //
        // Testing:
        //   void release();
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "RELEASE TEST" << endl
                                  << "============" << endl;

        if (verbose) cout << "\nTesting 'release' and destructor." << endl;

        struct {
            int  d_line;
            int  d_objectSize;
            int  d_numObjects;
            bool d_geometric;
        } DATA[] = {
            //line    object                          geometric
            //no.     size      numObjects            growth
            //----    ------    --------------------  ------
            { L_,       1,                         5, false },
            { L_,       5,                        10, false },
            { L_,      12,                         1, false },
            { L_,      24,                         5, false },
            { L_,      32,      MAX_BLOCKS_PER_CHUNK, false },
            { L_,       1,                         5,  true },
            { L_,       5,                        10,  true },
            { L_,      12,                         1,  true },
            { L_,      24,                         5,  true },
            { L_,      32,      MAX_BLOCKS_PER_CHUNK,  true }
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        const int NUM_REQUESTS = 100;
        bslma_TestAllocator taX;    const bslma_TestAllocator& TAX = taX;
        bslma_TestAllocator taY;    const bslma_TestAllocator& TAY = taY;

        for (int di = 0; di < NUM_DATA; ++di) {
            const int LINE = DATA[di].d_line;
            const int OBJECT_SIZE = DATA[di].d_objectSize;
            const int NUM_OBJECTS = DATA[di].d_numObjects;
            bsls_BlockGrowth::Strategy strategy =
                DATA[di].d_geometric
                  ? bsls_BlockGrowth::BSLS_GEOMETRIC
                  : bsls_BlockGrowth::BSLS_CONSTANT;
            {

                Obj mX(OBJECT_SIZE, strategy, NUM_OBJECTS, &taX);
                Obj mY(OBJECT_SIZE, strategy, NUM_OBJECTS, &taY);

                for (int ai = 0; ai < NUM_REQUESTS; ++ai) {
                    mX.allocate();
                    mY.allocate();
                }

                if (veryVerbose) { TAB; P_(TAX.numBytesInUse()); }
                mX.release();
                if (veryVerbose) { TAB; P(TAX.numBytesInUse()); }

                if (veryVerbose) { TAB; P_(TAY.numBytesInUse()); }
                // Let 'mY' go out of scope.
            }
            if (veryVerbose) { TAB; P(TAY.numBytesInUse()); }

            LOOP2_ASSERT(LINE, di, 0 == TAX.numBytesInUse());
            LOOP2_ASSERT(LINE, di, 0 == TAY.numBytesInUse());
        }
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TEST: deallocate
        //   Initialize a pool with varying object sizes and 'numObjects'.
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

        struct {
            int  d_line;
            int  d_objectSize;
            int  d_numObjects;
            bool d_geometric;
        } DATA[] = {
            //line    object                          geometric
            //no.     size      numObjects            growth
            //----    ------    --------------------  ------
            { L_,       1,                         5, false },
            { L_,       5,                        10, false },
            { L_,      12,                         1, false },
            { L_,      24,                         5, false },
            { L_,      32,      MAX_BLOCKS_PER_CHUNK, false },
            { L_,       1,                         5,  true },
            { L_,       5,                        10,  true },
            { L_,      12,                         1,  true },
            { L_,      24,                         5,  true },
            { L_,      32,      MAX_BLOCKS_PER_CHUNK,  true }
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        const int NUM_REQUESTS = 100;
        void *p[NUM_REQUESTS];
        bslma_TestAllocator ta;    const bslma_TestAllocator& TA = ta;

        for (int di = 0; di < NUM_DATA; ++di) {
            const int LINE = DATA[di].d_line;
            const int OBJECT_SIZE = DATA[di].d_objectSize;
            const int NUM_OBJECTS = DATA[di].d_numObjects;

            bsls_BlockGrowth::Strategy strategy =
                DATA[di].d_geometric
                  ? bsls_BlockGrowth::BSLS_GEOMETRIC
                  : bsls_BlockGrowth::BSLS_CONSTANT;

            Obj mX(OBJECT_SIZE, strategy, NUM_OBJECTS, &ta);

            for (int ai = 0; ai < NUM_REQUESTS; ++ai) {
                p[ai] = mX.allocate();
            }

            int numAllocations = TA.numAllocations();

            for (int dd = NUM_REQUESTS - 1; dd >= 0; --dd) {
                mX.deallocate(p[dd]);
            }

            if (veryVerbose) { TAB; P_(NUM_OBJECTS); P(numAllocations); }

            // Ensure memory was deallocated in expected sequence
            for (int aj = 0; aj < NUM_REQUESTS; ++aj) {
                LOOP3_ASSERT(LINE, di, aj, p[aj] == mX.allocate());
            }

            // Ensure no additional memory request to the allocator occurred
            LOOP2_ASSERT(LINE, di, TA.numAllocations() == numAllocations);
        }
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TEST: bcema_Pool(objectSize, basicAllocator);
        //
        // Plan:
        //   Initialize a pool with a chosen object size, default
        //   'maxBlocksPerChunk' and a test allocator.  Initialize a second
        //   pool as a reference with the same object size,
        //   MAX_BLOCKS_PER_CHUNK for 'numObjects' and a second test
        //   allocator.  Invoke 'allocate' repeatedly on both pools so that
        //   the pools deplete and replenish until the pools stop growing in
        //   size.  Verify that for each replenishment the allocator for the
        //   pool under test contains the same number of memory requests and
        //   the same request size as the allocator for the reference pool.
        //
        // Testing:
        //   bcema_Pool(objectSize, basicAllocator);
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "DEFAULT NUMOBJECTS TEST" << endl
                                  << "=======================" << endl;

        if (verbose) cout << "\nTesting constructor and 'allocate' w/ default "
                             "'numObjects'." << endl;

        const int OBJECT_SIZE = 4;

        bslma_TestAllocator taX;    const bslma_TestAllocator& TAX = taX;
        Obj mX(OBJECT_SIZE, &taX);  ASSERT(OBJECT_SIZE == mX.blockSize());

        bslma_TestAllocator taexp;    const bslma_TestAllocator& TAEXP = taexp;
        Obj mExp(OBJECT_SIZE,
                 bsls_BlockGrowth::BSLS_GEOMETRIC,
                 MAX_BLOCKS_PER_CHUNK,
                 &taexp);
        ASSERT(OBJECT_SIZE == mExp.blockSize());

        // Number of iterations is number of chunk allocations before
        // the max chunk size is reached, that is,
        // logBase2(CURRENT_MAX_BLOCKS_PER_CHUNK), plus an arbitrary
        // fudge factor.
        const int NUM_ITERATIONS = 4 +
                              (int)(bsl::log((double)MAX_BLOCKS_PER_CHUNK) /
                                    bsl::log(2.0));

        int blocksPerChunk = INITIAL_CHUNK_SIZE;
        for (int i = 0; i < NUM_ITERATIONS; ++i) {
            // Allocate until current pool is depleted.
            for (int j = 0; j < blocksPerChunk; ++j) {
                mX.allocate();
                mExp.allocate();
            }

            int numAllocations = TAX.numAllocations();
            int numBytes       = TAX.lastAllocatedNumBytes();
            if (veryVerbose) { TAB; P_(numAllocations); TAB; P(numBytes); }
            LOOP3_ASSERT(blocksPerChunk,
                        numAllocations,
                        TAEXP.numAllocations(),
                        TAEXP.numAllocations() == numAllocations);
            LOOP3_ASSERT(blocksPerChunk,
                        numBytes,
                        TAEXP.lastAllocatedNumBytes(),
                        TAEXP.lastAllocatedNumBytes() == numBytes);

            blocksPerChunk = blocksPerChunk * 2 <= MAX_BLOCKS_PER_CHUNK
                             ? blocksPerChunk *2
                             : MAX_BLOCKS_PER_CHUNK;
        }
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // GEOMETRIC GROWTH TEST
        //
        // Testing:
        //   bcema_Pool(objectSize,
        //              bsls_BlockGrowth::BSLS_GEOMETRIC,
        //              numObjects,
        //              basicAllocator);
        //   void *allocate();
        //
        //   Ensure pool replenishes the correct size of memory with negative
        //   'numObjects'.
        // --------------------------------------------------------------------

        const int DATA[] = {
            1,
            5,
            MAX_BLOCKS_PER_CHUNK / GROW_FACTOR - 1,
            MAX_BLOCKS_PER_CHUNK / GROW_FACTOR,
            MAX_BLOCKS_PER_CHUNK / GROW_FACTOR + 1,
            MAX_BLOCKS_PER_CHUNK - 1,
            MAX_BLOCKS_PER_CHUNK,
            MAX_BLOCKS_PER_CHUNK + 1
        };

        const int NUM_DATA         = sizeof DATA / sizeof *DATA;
        const int OBJECT_SIZE      = 8;
        const int POOL_OBJECT_SIZE = poolObjectSize(OBJECT_SIZE);

        for (int di = 0; di < NUM_DATA; ++di) {
            bslma_TestAllocator ta;    const bslma_TestAllocator& TA = ta;
            bslma_TestAllocator& testAllocator = ta;

            const int CURRENT_MAX_BLOCKS_PER_CHUNK = DATA[di];
            if (veryVerbose) cout << "\t[Starting 'numObjects' : "
                                  << CURRENT_MAX_BLOCKS_PER_CHUNK
                                  << "]" << endl;

            BEGIN_BSLMA_EXCEPTION_TEST {
                Obj mX(OBJECT_SIZE,
                       bsls_BlockGrowth::BSLS_GEOMETRIC,
                       CURRENT_MAX_BLOCKS_PER_CHUNK,
                       &ta);

                LOOP_ASSERT(di, OBJECT_SIZE == mX.blockSize());

                int numAllocations = TA.numAllocations();
                int blocksPerChunk = INITIAL_CHUNK_SIZE;

                // Number of iterations is number of chunk allocations before
                // the max chunk size is reached, that is,
                // logBase2(CURRENT_MAX_BLOCKS_PER_CHUNK), plus an arbitrary
                // fudge factor.
                const int NUM_ITERATIONS = 4 +
                    (int)(bsl::log((double)CURRENT_MAX_BLOCKS_PER_CHUNK) /
                          bsl::log(2.0));

                for (int i = 0; i < NUM_ITERATIONS; ++i) {
                    for (int j = 0; j < blocksPerChunk; ++j) {
                        mX.allocate();
                    }
                    ++numAllocations;
                    ASSERT(numAllocations == TA.numAllocations());
                    const int EXP_SIZE =
                                 blockSize(POOL_OBJECT_SIZE * blocksPerChunk);
                    LOOP3_ASSERT(blocksPerChunk,
                                 EXP_SIZE,
                                 TA.lastAllocatedNumBytes(),
                                 EXP_SIZE == TA.lastAllocatedNumBytes());
                    blocksPerChunk =
                          blocksPerChunk * 2 <= CURRENT_MAX_BLOCKS_PER_CHUNK
                        ? blocksPerChunk *2
                        : CURRENT_MAX_BLOCKS_PER_CHUNK;
                }

            } END_BSLMA_EXCEPTION_TEST
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // CONSTANT GROWTH TEST
        //   Initialize a pool with a chosen object size, varying positive
        //   (non-zero) 'numObjects' and a test allocator.  Invoke 'allocate'
        //   repeatedly so that the pool depletes and replenishes.  Verify that
        //   for each replenishment the pool requests memory of the expected
        //   size from the allocator and that no additional memory requests
        //   occurs between replenishments.
        //
        // Testing:
        //   bcema_Pool(objectSize,
        //              bsls_BlockGrowth::BSLS_CONSTANT,
        //              numObjects,
        //              basicAllocator);
        //   void *allocate();
        //
        //   Ensure pool replenishes the correct size of memory with positive
        //   'numObjects'.
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "CONSTANT GROWTH TEST" << endl
                                  << "========================" << endl;

        if (verbose) cout << "\nTesting constructor and 'allocate' w/ varying "
                             "positive 'numObjects'." << endl;

        const int DATA[] = { 1, 2, 10, MAX_BLOCKS_PER_CHUNK };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;
        const int OBJECT_SIZE = 4;
        const int POOL_OBJECT_SIZE = poolObjectSize(OBJECT_SIZE);
        const int NUM_REPLENISH = 3;

        bslma_TestAllocator ta;    const bslma_TestAllocator& TA = ta;
        bslma_TestAllocator& testAllocator = ta;

        for (int di = 0; di < NUM_DATA; ++di) {
            BEGIN_BSLMA_EXCEPTION_TEST {
                const int NUM_OBJECTS = DATA[di];
                Obj mX(OBJECT_SIZE,
                       bsls_BlockGrowth::BSLS_CONSTANT,
                       NUM_OBJECTS,
                       &ta);
                LOOP_ASSERT(di, OBJECT_SIZE == mX.blockSize());

                for (int ri = 0; ri < NUM_REPLENISH; ++ri) {
                    int numAllocations = TA.numAllocations();

                    // Allocate until current pool is deplete.
                    for (int oi = 0; oi < NUM_OBJECTS; ++oi) {
                        mX.allocate();
                    }

                    const int EXP = blockSize(POOL_OBJECT_SIZE * NUM_OBJECTS);
                    if (veryVerbose) { TAB; P_(numAllocations); TAB; P(EXP); }

                    LOOP2_ASSERT(di, ri,
                                 TA.numAllocations() == numAllocations + 1);
                    LOOP2_ASSERT(di, ri, TA.lastAllocatedNumBytes() == EXP);
                }
            } END_BSLMA_EXCEPTION_TEST
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // BLOCK SIZE TEST
        //   Initialize a pool with a positive 'numObjects' and varying object
        //   sizes.  Invoke 'allocate' repeatedly and verify that the
        //   difference between the memory addresses of two consecutive
        //   requests is equal to the expected object size.
        //
        // Testing:
        //   bcema_Pool(blockSize,
        //              bsls_BlockGrowth::BSLS_CONSTANT,
        //              numObjects,
        //              basicAllocator);
        //   void *allocate();
        //
        //   Ensure 'allocate' returns memory of the correct object size.
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "BLOCK SIZE TEST" << endl
                                  << "===============" << endl;

        if (verbose) cout << "\nTesting constructor and 'allocate' w/ varying "
                             "object sizes." << endl;

        const int DATA[] = { 1, 2, 5, 6, 12, 24, 32 };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;
        const int NUM_OBJECTS = 3;
        bslma_TestAllocator testAllocator;

        for (int di = 0; di < NUM_DATA; ++di) {
            BEGIN_BSLMA_EXCEPTION_TEST {
                const int OBJECT_SIZE = DATA[di];
                Obj mX(OBJECT_SIZE,
                       bsls_BlockGrowth::BSLS_CONSTANT,
                       NUM_OBJECTS,
                       &testAllocator);
                LOOP_ASSERT(di, OBJECT_SIZE == mX.blockSize());
                char *lastP = 0;
                for (int oi = 0; oi < NUM_OBJECTS; ++oi) {
                    char *p = (char *) mX.allocate();
                    scribble(p, OBJECT_SIZE);
                    if (oi) {
                        int size = p - lastP;
                        const int EXP = poolObjectSize(OBJECT_SIZE);
                        if (veryVerbose) { TAB; P_(size); TAB; P(EXP); }
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
        //   To test 'blockSize', create a 'bdema_BlockList' object initialized
        //   with a test allocator.  Invoke both the 'blockSize' function and
        //   the 'bdema_BlockList::allocate' method with varying memory sizes,
        //   and verify that the sizes returned by 'blockSize' are equal to the
        //   sizes recorded by the allocator.
        //
        //   To test 'poolObjectSize', invoke the function with varying sizes,
        //   and verify that the returned value is equal to the difference
        //   between the returned memory addresses of two consecutive requests
        //   (i.e., the size of each returned memory) to a pool initialized
        //   with the current size.
        //
        // Testing:
        //   int blockSize(numBytes);
        //   int poolObjectSize(size);
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "FILE-STATIC FUNCTION TEST" << endl
                                  << "=========================" << endl;

        if (verbose) cout << "\nTesting 'blockSize'." << endl;
        {
            const int DATA[] = { 0, 1, 5, 12, 24, 64, 1000 };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            bslma_TestAllocator a;
            bdema_InfrequentDeleteBlockList bl(&a);
            for (int i = 0; i < NUM_DATA; ++i) {
                const int SIZE = DATA[i];
                int blkSize = blockSize(SIZE);
                bl.allocate(SIZE);

                const int EXP = a.lastAllocatedNumBytes();

                if (veryVerbose) {TAB; P_(SIZE); P_(blkSize); P(EXP);}
                LOOP_ASSERT(i, EXP == blkSize);
            }
        }

        if (verbose) cout << "\nTesting 'poolObjectSize'." << endl;
        {
            const int DATA[] = { 1, 2, 5, 6, 12, 24, 32 };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int di = 0; di < NUM_DATA; ++di) {
                const int SIZE = DATA[di];
                Obj mX(SIZE, bsls_BlockGrowth::BSLS_CONSTANT, 2);
                LOOP_ASSERT(di, SIZE == mX.blockSize());
                char *p = (char *) mX.allocate();
                char *q = (char *) mX.allocate();

                int EXP = q - p;

                int objectSize = poolObjectSize(SIZE);
                if (veryVerbose) { TAB; P_(SIZE); P_(objectSize); P(EXP); }
                LOOP3_ASSERT(di, EXP, objectSize, EXP == objectSize);
            }
        }
      } break;
      case -1: {
        // --------------------------------------------------------------------
        // MEMORY EXHAUSTION TEST
        //
        // Concern: When a sufficiently huge number of allocation requests for
        // tiny blocks is made, an exception is thrown.
        //
        // Plan: Attempt to allocate an infinite number of int-sized objects.
        // Assert that an exception is thrown and caught eventually.
        // --------------------------------------------------------------------

        Obj mX(sizeof(int));

        bool caught = false;
        try {
            while (1) {
                mX.allocate();
            }
        }
        catch (bsl::bad_alloc) {
            caught = true;
        }
        ASSERT(caught);
      } break;
      case -2: {
        // ---------------------------------------------------------
        // BENCHMARK
        // ---------------------------------------------------------
        if (verbose) cout << endl
                          << "Benchmark" << endl
                          << "========================" << endl;
        enum {
            NUM_THREADS = 4,
            NUM_ITERATIONS = 50,
            NUM_OBJECTS = 10
        };

        int numThreads = argc > 2 ? atoi(argv[2]) : NUM_THREADS;
        int numIterations = argc > 3 ? atoi(argv[3]) : NUM_ITERATIONS;
        int numObjects = argc > 4 ? atoi(argv[4]) : NUM_OBJECTS;

        if (verbose) cout << endl
                          << "NUM THREADS: " << numThreads << endl
                          << "NUM ITERATIONS: " << numIterations << endl
                          << "POOL SIZE: " << numObjects * numThreads << endl;

        bench::runtest(numIterations, numObjects, numThreads);

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
