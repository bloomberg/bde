// bslma_allocator.t.cpp                                              -*-C++-*-

#include <bslma_allocator.h>

#include <bsls_alignmentutil.h>
#include <bsls_bsltestutil.h>
#include <bsls_compilerfeatures.h>
#include <bsls_keyword.h>
#include <bsls_platform.h>
#include <bsls_protocoltest.h>

#include <stdio.h>      // `printf`
#include <stdlib.h>     // `atoi`
#include <string.h>

#include <new>

#ifdef BDE_VERIFY
// Suppress some pedantic bde_verify checks in this test driver
#pragma bde_verify -FD01   // Function declaration requires contract
#pragma bde_verify -FD03   // Parameter not documented in function contract
#pragma bde_verify -FABC01 // Function not in alphabetical order
#pragma bde_verify -TP19   // Missing or malformed standard test driver section
#pragma bde_verify -TY02   // Template parameter uses single-letter name
#endif

using namespace BloombergLP;

#ifdef BSLS_PLATFORM_HAS_PRAGMA_GCC_DIAGNOSTIC
#ifdef BSLS_PLATFORM_CMP_CLANG
#pragma GCC diagnostic ignored "-Wunused-private-field"
#endif
#endif

//=============================================================================
//                              TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// We are testing a pure protocol class as well as a set of overloaded
// operators.  We need to verify that (1) a concrete derived class compiles
// and links, (2) that the each of the two concrete template functions,
// `deleteObject` and `deleteObjectRaw` destroys the argument object and calls
// the deallocate method of the supplied allocator, and (3) that the overloaded
// `new` and `delete` operators respectively forward the call to the
// `allocate` and `deallocate` method of the supplied allocator.
//-----------------------------------------------------------------------------
// [ 1] virtual ~Allocator();
// [ 1] virtual void *allocate(size_type) = 0;
// [ 1] virtual void deallocate(void *) = 0;
// [ 3] virtual void* do_allocate(std::size_t, std::size_t);
// [ 3] virtual void do_deallocate(void *p, std::size_t, std::size_t);
// [ 3] virtual bool do_is_equal(const bsl::memory_resource&) const;
// [ 4] template<class TYPE> void deleteObject(const TYPE *);
// [ 5] template<class TYPE> void deleteObjectRaw(const TYPE *);
// [ 4] void deleteObject(bsl::nulptr_t);
// [ 5] void deleteObjectRaw(bsl::nulptr_t);
// [ 6] void *operator new(int size, bslma::Allocator& basicAllocator);
// [ 7] void operator delete(void *address, bslma::Allocator& bA);
#ifndef BDE_OMIT_INTERNAL_DEPRECATED
// [  ] static throwBadAlloc();
#endif
//-----------------------------------------------------------------------------
// [ 1] PROTOCOL TEST - Make sure derived class compiles and links.
// [ 7] EXCEPTION SAFETY - Ensure operator delete is invoked on an exception.
// [ 2] TEST HARNESS  - Make sure test classes and functions work properly.
// [ 8] USAGE EXAMPLE - Make sure usage examples compiles and works properly.
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
        fflush(stdout);

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
//                      CONCRETE DERIVED TYPES
//-----------------------------------------------------------------------------

typedef bsls::AlignmentUtil Align;

typedef Align::MaxAlignedType MaxAlignedType;

const int k_MAX_ALIGNMENT = Align::BSLS_MAX_ALIGNMENT;

/// This class is used with `bsls::ProtocolTest` to test the `bslma::Allocator`
/// protocol.
class AllocatorProtocolTest : public bsls::ProtocolTestImp<bslma::Allocator> {

  protected:
    void* do_allocate(std::size_t, std::size_t) BSLS_KEYWORD_OVERRIDE;
    void do_deallocate(void *, std::size_t, std::size_t) BSLS_KEYWORD_OVERRIDE;
    bool do_is_equal(const bsl::memory_resource&) const
                                   BSLS_KEYWORD_NOEXCEPT BSLS_KEYWORD_OVERRIDE;

  public:
    void *allocate(size_type) BSLS_KEYWORD_OVERRIDE;
    void deallocate(void *) BSLS_KEYWORD_OVERRIDE;
};

void *AllocatorProtocolTest::do_allocate(std::size_t, std::size_t)
{
    static char buf[1];
    return markDoneVal(static_cast<void*>(buf));  // return non-null pointer
}

void AllocatorProtocolTest::do_deallocate(void *, std::size_t, std::size_t)
{
    markDone();
}

bool AllocatorProtocolTest::do_is_equal(const bsl::memory_resource&) const
                                                          BSLS_KEYWORD_NOEXCEPT
{
    return markDone();
}

void *AllocatorProtocolTest::allocate(size_type)
{
    static char buf[1];
    return markDoneVal(static_cast<void*>(buf));  // return non-null pointer
}

void AllocatorProtocolTest::deallocate(void *)
{
    markDone();
}

/// This class is used with `bsls::ProtocolTest` to test the
/// `bslma::Allocator` protocol.  Unlike the `AllocatorProtocolTest`, the
/// non-pure `do_allocate`, `do_deallocate`, and `do_is_equal` virtual
/// functions are not overriden; their default implementations are used
/// instead.
class IndirectProtocolTest : public bsls::ProtocolTestImp<bslma::Allocator> {

  public:
    void *allocate(size_type) BSLS_KEYWORD_OVERRIDE;
    void deallocate(void *) BSLS_KEYWORD_OVERRIDE;
};

void *IndirectProtocolTest::allocate(size_type)
{
    static char buf[1];
    return markDoneVal(static_cast<void*>(buf));  // return non-null pointer
}

void IndirectProtocolTest::deallocate(void *)
{
    markDone();
}

enum AllocOp { e_INIT, e_ALLOC, e_DEALLOC };

/// Instrumented test allocator for single-shot allocations.  This allocator
/// keeps track of the most recent calls to `allocate` or `deallocate`.  The
/// `allocate` method return a pointer whose alignment exactly matches the
/// natural alignment for the requested size.  Only one block may be
/// outstanding at a time; if `allocate` is called twice without an
/// intervening `deallocate`, the result of the previous allocation is
/// discarded and the fixed buffer is reused.  There are no negative
/// consequences to failing to deallocate an allocated block nor to calling
/// `allocate` multiple times without calling `deallocate` in-between, so
/// long as no object is actually constructed in the allocated storage.
class my_Allocator : public bslma::Allocator {

    AllocOp    d_lastOp;    // most recent operation
    size_type  d_lastSize;  // size argument from most recent allocation
    void      *d_lastBlock; // block returned from most recent allocation
    char       d_storage[256 + 32 + 4];

    int        d_allocateCount;    // number of times allocate called
    int        d_deallocateCount;  // number of times deallocate called

  public:
    // CREATORS
    my_Allocator()
        : d_lastOp(e_INIT), d_lastSize(0), d_lastBlock(0)
    , d_allocateCount(0), d_deallocateCount(0) { }

    ~my_Allocator() BSLS_KEYWORD_OVERRIDE { }

    // MANIPULATORS

    /// Return a pointer exactly aligned on the natural alignment of the
    /// specified `s`, and record the inputs and outputs for subsequent
    /// queries.  Note that the returned block might not be large enough to
    /// fit an object of size `s`.
    void *allocate(size_type s) BSLS_KEYWORD_OVERRIDE;

    /// Verify that `p` corresponds to the return value of the most recent
    /// call to `allocate` and record the `deallocation` for subsequent
    /// queries.
    void deallocate(void *p) BSLS_KEYWORD_OVERRIDE;

    // ACCESSORS

    /// Return the number of times that `allocate` was called.
    int allocateCount() const { return d_allocateCount; }

    /// Return the number of times that `deallocate` was called.
    int deallocateCount() const { return d_deallocateCount; }

    /// Return last argument value for `allocate`.
    size_type lastSize() const { return d_lastSize; }

    /// Return the address of the most recently allocated block.
    void *lastBlock() const { return d_lastBlock; }

    /// Return a descriptive code for the last `allocate` or `deallocate` call.
    AllocOp lastOp() const { return d_lastOp; }
};

// MANIPULATORS
void *my_Allocator::allocate(size_type s)
{
    d_lastOp    = e_ALLOC;
    d_lastSize  = s;

    if (0 == s) {
        d_lastBlock = 0;
    }
    else {
        // Allocate a correctly aligned block from `d_storage`.
        int   alignment = Align::calculateAlignmentFromSize(s);
        int   offset = Align::calculateAlignmentOffset(d_storage, alignment);
        char *p      = static_cast<char *>(d_storage) + offset;

        // Make sure `p` is exactly aligned, not overaligned.
        if (Align::isAligned(p, alignment << 1)) {
            p += alignment;
        }

        d_lastBlock = p;
    }

    ++d_allocateCount;

    return d_lastBlock;
}

void my_Allocator::deallocate(void *p)
{
    ASSERT(e_ALLOC == d_lastOp);
    ASSERT(p       == d_lastBlock);
    d_lastOp    = e_DEALLOC;
    d_lastBlock = p;
    ++d_deallocateCount;
}

/// Test class used to verify examples.
class CountingNewDeleteAlloc : public bslma::Allocator {

    int d_count;

    enum { MAGIC   = 0xDEADBEEF,
           DELETED = 0xBADF000D };

  public:
    CountingNewDeleteAlloc(): d_count(0) { }
    ~CountingNewDeleteAlloc() BSLS_KEYWORD_OVERRIDE { }

    void *allocate(size_type size) BSLS_KEYWORD_OVERRIDE {
        ++d_count;

        if (0 == size) {
            return 0;
        }

        unsigned *p = (unsigned *) operator new(size + k_MAX_ALIGNMENT);
        *p = MAGIC;

        return (char *) p + k_MAX_ALIGNMENT;
    }

    void deallocate(void *address) BSLS_KEYWORD_OVERRIDE {
        ++d_count;

        if (0 == address) {
            return;
        }

        unsigned *p = (unsigned *) ((MaxAlignedType *) address - 1);
        ASSERT(MAGIC == *p);
        *p = DELETED;

        operator delete(p);
    }

    int getCount() const            { return d_count; }
};

//=============================================================================
//                   CONCRETE OBJECTS FOR TESTING `deleteObject`
//-----------------------------------------------------------------------------
static int globalObjectStatus = 0;    // global flag set by test-object d'tors
static int class3ObjectCount = 0;     // Count set by my_Class3 c'tor/d'tor

class my_Class1 {
  public:
    my_Class1() { globalObjectStatus = 1; }
    ~my_Class1() { globalObjectStatus = 0; }
};

class my_Class2 {
  public:
    my_Class2() { globalObjectStatus = 1; }
    virtual ~my_Class2() { globalObjectStatus = 0; }
};

class my_Class3Base {
  public:
    my_Class3Base() { }
    virtual ~my_Class3Base() = 0;
};

class my_Class3 : public my_Class3Base {
  public:
    my_Class3() { ++class3ObjectCount; }
    ~my_Class3() BSLS_KEYWORD_OVERRIDE;
};

my_Class3Base::~my_Class3Base() { }
my_Class3::~my_Class3() { --class3ObjectCount; }

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
    my_LeftBase()                        { leftBaseObjectCount = 1; }
    ~my_LeftBase() BSLS_KEYWORD_OVERRIDE { leftBaseObjectCount = 0; }
};

class my_RightBase : virtual public my_VirtualBase {
    int x;
  public:
    my_RightBase()                        { rightBaseObjectCount = 1; }
    ~my_RightBase() BSLS_KEYWORD_OVERRIDE { rightBaseObjectCount = 0; }
};

class my_MostDerived : public my_LeftBase, public my_RightBase {
    int x;
  public:
    my_MostDerived()                        { mostDerivedObjectCount = 1; }
    ~my_MostDerived() BSLS_KEYWORD_OVERRIDE { mostDerivedObjectCount = 0; }
};

//=============================================================================
//                              USAGE EXAMPLE
//-----------------------------------------------------------------------------

///Example 1: Derived Concrete Allocator
/// - - - - - - - - - - - - - - - - - -
// In order for the `bslma::Allocator` interface to be useful, we must supply a
// concrete allocator that implements it.  In this example we demonstrate how
// to adapt `operator new` and `operator delete` to this protocol base class.
//
// First, in a component `.h` file, we define a class, derived from
// `bslma::Allocator`, that provides concrete implementations of the `virtual`
// `allocate` and `deallocate` methods:
// ```
    // my_newdeleteallocator.h
    // ...

//  #include <bslma_allocator.h>
//  #include <bsls_keyword.h>
//  #include <new>

    /// This class is a sample concrete implementation of the
    /// `bslma::Allocator` protocol that provides direct access to the
    /// system-supplied (native) global operators `new` and `delete`.
    class my_NewDeleteAllocator : public bslma::Allocator {

        // NOT IMPLEMENTED
        my_NewDeleteAllocator(const my_NewDeleteAllocator&);
        my_NewDeleteAllocator& operator=(const my_NewDeleteAllocator&);

      public:
        // CLASS METHODS

        /// Return the address of a singleton object of
        /// `my_NewDeleteAllocator`. Since `my_NewDeleteAllocator` has no
        /// state, there is never a need for more than one.
        static my_NewDeleteAllocator *singleton();

        // CREATORS

        /// Create an allocator that wraps the global (native) operators
        /// `new` and `delete` to supply memory.  Note that all objects of
        /// this class share the same underlying resource.
        my_NewDeleteAllocator() { }

        /// Destroy this allocator object.  Note that destroying this
        /// allocator has no effect on any outstanding allocated memory.
        ~my_NewDeleteAllocator() BSLS_KEYWORD_OVERRIDE;

        // MANIPULATORS

        /// Return a newly allocated block of memory of (at least) the
        /// specified positive `size` (in bytes).  If `size` is 0, a null
        /// pointer is returned with no other effect.  If this allocator
        /// cannot return the requested number of bytes, then it will throw
        /// a `std::bad_alloc` exception in an exception-enabled build, or
        /// else will abort the program in a non-exception build.  The
        /// behavior is undefined unless `0 <= size`.  Note that the
        /// alignment of the address returned is the maximum alignment for
        /// any type defined on this platform.  Also note that global
        /// `operator new` is *not* called when `size` is 0 (in order to
        /// avoid having to acquire a lock, and potential contention in
        /// multi-threaded programs).
        void *allocate(size_type size) BSLS_KEYWORD_OVERRIDE;

        /// Return the memory block at the specified `address` back to this
        /// allocator.  If `address` is 0, this function has no effect.  The
        /// behavior is undefined unless `address` was allocated using this
        /// allocator object and has not already been deallocated.  Note
        /// that global `operator delete` is *not* called when `address` is
        /// 0 (in order to avoid having to acquire a lock, and potential
        /// contention in multi-treaded programs).
        void deallocate(void *address) BSLS_KEYWORD_OVERRIDE;
    };
// ```
// Next, in the component `.cpp` file, we define the `singleton()` method,
// which provides the typical way of obtaining an instance of this allocator:
// ```
    // my_newdeleteallocator.cpp
//  #include <my_newdeleteallocator.h>

    // CLASS METHODS
    my_NewDeleteAllocator *my_NewDeleteAllocator::singleton()
    {
        static my_NewDeleteAllocator obj;
        return &obj;
    }
// ```
// Next, we implement the `bslma::Allocator` protocol by defining (also in the
// component `.cpp` file) the virtual methods:
// ```
    // CREATORS
    my_NewDeleteAllocator::~my_NewDeleteAllocator()
    {
    }

    // MANIPULATORS
    void *my_NewDeleteAllocator::allocate(size_type size)
    {
        return 0 == size ? 0 : ::operator new(size);
    }

    void my_NewDeleteAllocator::deallocate(void *address)
    {
        // While the C++ standard guarantees that calling delete(0) is safe
        // (3.7.3.2 paragraph 3), some libc implementations take out a lock to
        // deal with the free(0) case, so this check can improve efficiency of
        // threaded programs.

        if (address) {
            ::operator delete(address);
        }
    }
// ```
// Now we can use `my_NewDeleteAllocator` to allocate and deallocate storage
// for (in this case, `int`) objects:
// ```
    void usageExample1()
    {
        typedef int T;  // Can be any type

        my_NewDeleteAllocator myA;
        T *p = static_cast<T *>(myA.allocate(sizeof(T)));
        new (p) T(5);    // Construct object at `p`.
        ASSERT(5 == *p);
        p->~T();         // not needed for `int`, but important for class types
        myA.deallocate(p);
// ```
// Finally, we repeat the previous example using the `singleton` object instead
// of constructing a new `my_NewDeleteAllocator` and using the `operator new`
// and `deleteObject` interface instead of raw `allocate`-construct and
// destroy-`deallocate`.  Note that these interfaces can be mixed and matched
// (e.g., `singleton` can be used with `allocate`):
// ```
        p = new (*my_NewDeleteAllocator::singleton()) T(6);
        ASSERT(6 == *p);
        my_NewDeleteAllocator::singleton()->deleteObject(p);
    }
// ```
//
///Example 2: Container Objects
/// - - - - - - - - - - - - - -
// Allocators are often supplied to objects requiring dynamically-allocated
// memory at construction.  For example, consider the following
// `my_DoubleStack` class, which uses a `bslma::Allocator` to allocate memory.
//
// First, we define the class interface, which is a minimal subset of a typical
// container interface:
// ```
    // my_doublestack.h
    // ...

//  #include <bslma_allocator.h>
//  #include <my_NewDeleteAllocator.h>

    /// dynamically growing stack of `double` values
    class my_DoubleStack {
        enum { k_INITIAL_SIZE = 1, k_GROWTH_FACTOR = 2 };

        int               d_capacity;    // physical capacity of this stack (in
                                         // elements)
        int               d_size;        // number of available stack elements
                                         // currently in use
        double           *d_stack_p;     // dynamically allocated array of
                                         // `d_capacity` elements
        bslma::Allocator *d_allocator_p; // holds (but doesn't own) allocator

      private:
        /// Increase the capacity by `k_GROWTH_FACTOR`.
        void increaseSize();

      public:
        // CREATORS
        explicit my_DoubleStack(bslma::Allocator *basicAllocator = 0);
        my_DoubleStack(const my_DoubleStack&  other,
                       bslma::Allocator      *basicAllocator = 0);
        ~my_DoubleStack();

        // MANIPULATORS
        my_DoubleStack& operator=(const my_DoubleStack& rhs);
        void pop() { --d_size; }
        void push(double value);

        // ACCESSORS
        double operator[](int i) const { return d_stack_p[i]; }
        bslma::Allocator *allocator() const { return d_allocator_p; }
        int capacity() const { return d_capacity; }
        bool isEmpty() const { return 0 == d_size; }
        int size() const { return d_size; }
        double top() const { return d_stack_p[d_size - 1]; }
    };
// ```
// Next, we define the constructor, which takes an optional `basicAllocator`
// supplied only at construction.  (We avoid use of the name `allocator` so as
// not to conflict with the STL use of the word, which differs slightly.)  If
// non-zero, the stack holds a pointer to this allocator, but does not own it.
// If no allocator is supplied, the implementation itself must either
// conditionally invoke global `new` and `delete` explicitly whenever dynamic
// memory must be managed (BAD IDEA) or (GOOD IDEA) install a default allocator
// that adapts use of these global operators to the `bslma_allocator` interface
// (see `bslma_default`).  The constructor uses the selected allocator to
// allocate memory via the `allocate` method.
// ```
    // my_doublestack.cpp
//  #include <my_doublestack.h>
//  #include <bslma_allocator.h>
//  #include <bslma_default.h>    // for selecting a default allocator

    // CREATORS
    my_DoubleStack::my_DoubleStack(bslma::Allocator *basicAllocator)
    : d_capacity(k_INITIAL_SIZE)
    , d_size(0)
    , d_allocator_p(basicAllocator ?
                    basicAllocator : my_NewDeleteAllocator::singleton())
        // The above initialization expression is roughly equivalent to
        // `bslma::Default::allocator(basicAllocator)`
    {
        ASSERT(d_allocator_p);
        d_stack_p = (double *)
            d_allocator_p->allocate(d_capacity * sizeof *d_stack_p);
    }
// ```
// Next, we define a destructor that frees the memory held by the container
// using the allocator's `deallocate` method:
// ```
    my_DoubleStack::~my_DoubleStack()
    {
        // CLASS INVARIANTS
        ASSERT(d_allocator_p);
        ASSERT(d_stack_p);
        ASSERT(0 <= d_size);
        ASSERT(0 <= d_capacity);
        ASSERT(d_size <= d_capacity);

        d_allocator_p->deallocate(d_stack_p);
    }
// ```
// Next, we define a `reallocation` function that expands a dynamic array of
// `double`s.  Even in this simplified implementation, all use of the allocator
// protocol is relegated to the `.cpp` file:
// ```
    /// Reallocate memory in the specified `array` to the specified `newSize`
    /// using the specified `basicAllocator`.  The specified `length` number of
    /// leading elements are preserved.  Since the class invariant requires
    /// that the physical capacity of the container may grow but never shrink;
    /// the behavior is undefined unless `length <= newSize`.
    static inline
    void reallocate(double           **array,
                    int                newSize,
                    int                length,
                    bslma::Allocator  *basicAllocator)
    {
        ASSERT(array);
        ASSERT(1 <= newSize);
        ASSERT(0 <= length);
        ASSERT(basicAllocator);
        ASSERT(length <= newSize);        // enforce class invariant

        double *tmp = *array;             // support exception neutrality
        *array = (double *) basicAllocator->allocate(newSize * sizeof **array);

        // COMMIT POINT

        memcpy(*array, tmp, length * sizeof **array);
        basicAllocator->deallocate(tmp);
    }
// ```
// Next, we define the private `increaseSize` method to allocate more space for
// container elements as needed:
// ```
    void my_DoubleStack::increaseSize()
    {
        int proposedNewSize = d_capacity * k_GROWTH_FACTOR;
        ASSERT(proposedNewSize > d_size);

        // Reallocate might throw.
        reallocate(&d_stack_p, proposedNewSize, d_size, d_allocator_p);

        // Commit change only after `reallocate` succeeds.
        d_capacity = proposedNewSize;
    }
// ```
// Now we have what we need to implement the `push` method:
// ```
    void my_DoubleStack::push(double value)
    {
        if (d_size >= d_capacity) {
            increaseSize();
        }
        d_stack_p[d_size++] = value;
    }
// ```
// Now, to test our stack class, we first verify that its constructor captures
// the allocator correctly; if supplied an allocator pointer, it holds on to
// that pointer, otherwise it uses `my_NewDeleteAllocator::singleton()`:
// ```
    void usageExample2()
    {
        my_NewDeleteAllocator myA;

        my_DoubleStack ds1(&myA); // Supply an allocator.
        ASSERT(ds1.allocator() == &myA);

        my_DoubleStack ds2;       // Do not supply an allocator.
        ASSERT(ds2.allocator() == my_NewDeleteAllocator::singleton());
// ```
// Finally, we exercise and verify the behavior of the manipulators and
// accessors:
// ```
        ASSERT(ds2.isEmpty());
        ASSERT(1 == ds2.capacity());
        ds2.push(1.25);
        ds2.push(1.5);
        ds2.push(1.75);

        ASSERT(! ds2.isEmpty());
        ASSERT(4    == ds2.capacity());
        ASSERT(3    == ds2.size());
        ASSERT(1.75 == ds2.top());
        ASSERT(1.25 == ds2[0]);
        ASSERT(1.5  == ds2[1]);
        ASSERT(1.75 == ds2[2]);

        ds2.pop();
        ASSERT(4   == ds2.capacity());
        ASSERT(2   == ds2.size());
        ASSERT(1.5 == ds2.top());
    }
// ```

//-----------------------------------------------------------------------------
//                      OVERLOADED OPERATOR NEW USAGE EXAMPLE
//-----------------------------------------------------------------------------

class my_Type {
    char             *d_stuff_p;
    bslma::Allocator *d_allocator_p;

  public:
    my_Type(int size, bslma::Allocator *basicAllocator)
    : d_allocator_p(basicAllocator)
    {
        d_stuff_p = (char *) d_allocator_p->allocate(size);
    }

    ~my_Type()
    {
        d_allocator_p->deallocate(d_stuff_p);
    }
};

my_Type *newMyType(bslma::Allocator *basicAllocator) {
    return new (*basicAllocator) my_Type(5, basicAllocator);
}
void deleteMyType(bslma::Allocator *basicAllocator, my_Type *t) {
    t->~my_Type();
    basicAllocator->deallocate(t);
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
//                                 MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int                 test = argc > 1 ? atoi(argv[1]) : 0;
    bool             verbose = argc > 2;
    bool         veryVerbose = argc > 3;
    bool     veryVeryVerbose = argc > 4;
    bool veryVeryVeryVerbose = argc > 5;

    (void)veryVerbose;           // suppress unused variable warning
    (void)veryVeryVerbose;       // suppress unused variable warning
    (void)veryVeryVeryVerbose;   // suppress unused variable warning

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:
      case 8: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //   The usage example provided in the component header file must
        //   compile, link, and run on all platforms as shown.
        //
        // Plan:
        //   Incorporate usage example from header into driver, remove leading
        //   comment characters, and replace `assert` with `ASSERT`.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) printf("\nUSAGE EXAMPLE"
                            "\n=============\n");

        usageExample1();
        usageExample2();

      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TEST `operator delete` AND EXCEPTION SAFETY
        //   We want to make sure that when the overloaded `operator new`
        //   is invoked and the constructor of the new object throws an
        //   exception, the overloaded `operator delete` is invoked
        //   automatically to deallocate the object.
        //
        // Plan:
        //   Invoke `operator new` for a class that throws an exception from
        //   the chosen constructor.  Catch the exception and verify that
        //   deallocation was performed automatically.
        //
        // Testing:
        //   void operator delete(void *address, bslma::Allocator& bA);
        //   CONCERN: EXCEPTION SAFETY
        // --------------------------------------------------------------------

        if (verbose) printf("\n`operator delete` AND EXCEPTION SAFETY"
                            "\n======================================\n");
        {
            my_Allocator                         myA;
            bslma::Allocator&                    a   = myA;
            my_ClassThatMayThrowFromConstructor *p   = 0;

#ifdef BDE_BUILD_TARGET_EXC
            try
#endif
            {
                p = new (a) my_ClassThatMayThrowFromConstructor;
                p->~my_ClassThatMayThrowFromConstructor();
                a.deallocate(p);
                p = 0;
            }
#ifdef BDE_BUILD_TARGET_EXC
            catch(int n)
            {
                if (veryVerbose) printf("\nCaught exception.\n");
                ASSERT(13 == n);
            }
#endif
            ASSERT(!p);  // `operator new` never returned, hence never set `p`.
            ASSERT(e_DEALLOC == myA.lastOp());
            ASSERT(1 == myA.allocateCount());
            ASSERT(1 == myA.deallocateCount());
        }

      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TEST `operator new`
        //   We want to make sure that the correct underlying method is
        //   called based on the type of the overloaded `new` operator.
        //
        // Plan:
        //   Invoke `operator new` for types of various size.  Verify that
        //   correct parameters are passed to allocate method.
        //
        // Testing:
        //   void *operator new(int size, bslma::Allocator& basicAllocator);
        // --------------------------------------------------------------------

        if (verbose) printf("\nTEST `operator new`"
                            "\n===================\n");

        my_Allocator      myA;
        bslma::Allocator& a   = myA;

        {
            void *p;

            p = new(a) char;
            ASSERT(myA.lastOp()    == e_ALLOC);
            ASSERT(myA.lastSize()  == sizeof(char));
            ASSERT(myA.lastBlock() == p);

            p = new(a) short;
            ASSERT(myA.lastOp()    == e_ALLOC);
            ASSERT(myA.lastSize()  == sizeof(short));
            ASSERT(myA.lastBlock() == p);

            p = new(a) int;
            ASSERT(myA.lastOp()    == e_ALLOC);
            ASSERT(myA.lastSize()  == sizeof(int));
            ASSERT(myA.lastBlock() == p);

            p = new(a) double;
            ASSERT(myA.lastOp()    == e_ALLOC);
            ASSERT(myA.lastSize()  == sizeof(double));
            ASSERT(myA.lastBlock() == p);

            struct X15 { char d_z[15]; };        ASSERT(15 == sizeof(X15));
            p = new(a) X15;
            ASSERT(myA.lastOp()    == e_ALLOC);
            ASSERT(myA.lastSize()  == sizeof(X15));
            ASSERT(myA.lastBlock() == p);
        }

      } break;
      case 5: {
        // --------------------------------------------------------------------
        // MEMBER TEMPLATE METHOD `deleteObjectRaw` TEST
        //   We want to make sure that when `deleteObjRaw` is used both
        //   destructor and `deallocate` are invoked.
        //
        // Plan:
        //   Using an allocator and placement new operator construct objects of
        //   two different classes.  Invoke `deleteObjectRaw` to delete
        //   constructed objects and check that both destructor and
        //   `deallocate` have been called.  Repeat tests with a derived-class
        //   object with a virtual destructor.  Test with null pointer.
        //
        // Testing:
        //   template<class TYPE> void deleteObjectRaw(const TYPE *);
        //   void deleteObjectRaw(bsl::nulptr_t);
        // --------------------------------------------------------------------

        if (verbose) printf(
                          "\nMEMBER TEMPLATE METHOD `deleteObjectRaw` TEST"
                          "\n=============================================\n");

        if (veryVerbose) printf("\nTesting `deleteObjectRaw`:\n");
        {
            CountingNewDeleteAlloc myA;  bslma::Allocator& a = myA;

            if (veryVerbose) printf("\twith a my_Class1 object\n");

            if (veryVerbose) { T_;  T_;  P(globalObjectStatus); }
            ASSERT(0 == globalObjectStatus);

            ASSERT(0 == myA.getCount());
            my_Class1 *pC1 = (my_Class1 *) a.allocate(sizeof(my_Class1));
            const my_Class1 *pC1CONST = pC1;
            ASSERT(1 == myA.getCount());

            new(pC1) my_Class1;
            if (veryVerbose) { T_;  T_;  P(globalObjectStatus); }
            ASSERT(1 == globalObjectStatus);

            ASSERT(1 == myA.getCount());
            a.deleteObjectRaw(pC1CONST);
            if (veryVerbose) { T_;  T_;  P(globalObjectStatus); }
            ASSERT(0 == globalObjectStatus);
            ASSERT(2 == myA.getCount());

            if (veryVerbose) printf("\twith a my_Class2 object\n");

            if (veryVerbose) { T_;  T_;  P(globalObjectStatus); }
            ASSERT(0 == globalObjectStatus);

            ASSERT(2 == myA.getCount());
            my_Class2 *pC2 = (my_Class2 *) a.allocate(sizeof(my_Class2));
            const my_Class2 *pC2CONST = pC2;
            ASSERT(3 == myA.getCount());

            new(pC2) my_Class2;
            if (veryVerbose) { T_;  T_;  P(globalObjectStatus); }
            ASSERT(1 == globalObjectStatus);

            ASSERT(3 == myA.getCount());
            a.deleteObjectRaw(pC2CONST);
            if (veryVerbose) { T_;  T_;  P(globalObjectStatus); }
            ASSERT(0 == globalObjectStatus);
            ASSERT(4 == myA.getCount());

            if (veryVerbose) printf("\tWith a polymorphic object\n");

            ASSERT(0 == class3ObjectCount);
            my_Class3 *pC3 = (my_Class3 *) a.allocate(sizeof(my_Class3));
            const my_Class3Base *pC3bCONST = pC3;
            (void) pC3bCONST;
            ASSERT(0 == class3ObjectCount);
            ASSERT(0 == globalObjectStatus);
            ASSERT(5 == myA.getCount());

            new(pC3) my_Class3;
            if (veryVerbose) { T_;  T_;  P(class3ObjectCount); }
            ASSERT(1 == class3ObjectCount);
            ASSERT(0 == globalObjectStatus);

            ASSERT(5 == myA.getCount());
            a.deleteObjectRaw(pC3);
            if (veryVerbose) { T_;  T_;  P(class3ObjectCount); }
            ASSERT(0 == class3ObjectCount);
            ASSERT(0 == globalObjectStatus);
            ASSERT(6 == myA.getCount());

            if (veryVerbose) printf("\tWith a null my_Class3 pointer\n");

            pC3 = 0;
            a.deleteObjectRaw(pC3);
            if (veryVerbose) { T_;  T_;  P(class3ObjectCount); }
            ASSERT(0 == class3ObjectCount);
            ASSERT(0 == globalObjectStatus);
            ASSERT(6 == myA.getCount());

            if (veryVerbose) printf("\tWith a null pointer literal\n");

            a.deleteObjectRaw(0);
            ASSERT(6 == myA.getCount());

#if defined(BSLS_COMPILERFEATURES_SUPPORT_NULLPTR)
            a.deleteObjectRaw(nullptr);
            ASSERT(6 == myA.getCount());
#endif // BSLS_COMPILERFEATURES_SUPPORT_NULLPTR

        }
        {
            CountingNewDeleteAlloc myA;  bslma::Allocator& a = myA;

            if (veryVerbose) printf("\tdeleteObjectRaw(my_MostDerived*)\n");

            ASSERT(0 == myA.getCount());
            my_MostDerived *pMost =
                         (my_MostDerived *) a.allocate(sizeof(my_MostDerived));
            const my_MostDerived *pMostCONST = pMost;
            ASSERT(1 == myA.getCount());

            ASSERT(0 == mostDerivedObjectCount);
            ASSERT(0 == rightBaseObjectCount);
            ASSERT(0 == leftBaseObjectCount);
            ASSERT(0 == virtualBaseObjectCount);
            new(pMost) my_MostDerived;
            ASSERT(1 == mostDerivedObjectCount);
            ASSERT(1 == rightBaseObjectCount);
            ASSERT(1 == leftBaseObjectCount);
            ASSERT(1 == virtualBaseObjectCount);

            a.deleteObjectRaw(pMostCONST);
            ASSERT(2 == myA.getCount());
            ASSERT(0 == mostDerivedObjectCount);
            ASSERT(0 == rightBaseObjectCount);
            ASSERT(0 == leftBaseObjectCount);
            ASSERT(0 == virtualBaseObjectCount);
        }

      } break;

      case 4: {
        // --------------------------------------------------------------------
        // MEMBER TEMPLATE METHOD `deleteObject` TEST
        //
        // Concerns:
        // 1. `deleteObject` can be called with a null pointer, having no
        //    effect.
        // 2. `deleteObject` can be called with a null pointer literal, having
        //    no effect.
        // 3. `deleteObject`, when passed a pointer to an object allocated by
        //    a given allocator, runs the destructor for the referenced object,
        //    and then calls `deallocate` for the footprint of that object.
        // 4. `deleteObject`, when passed a pointer to a derived object (where
        //    the base type has a virtual destructor) allocated by a given
        //    allocator, runs the derived destructor for the referenced object,
        //    and then calls `deallocate` for the footprint of that whole
        //    object, even when the base class is not the left-most base of the
        //    derived type.
        //
        // Plan:
        //   Using an allocator and placement new operator construct objects of
        //   two different classes.  Invoke `deleteObject` to delete
        //   constructed objects and check that both destructor and
        //   `deallocate` have been called.  Repeat tests with a derived-class
        //   object with a virtual destructor.  Test with null pointer.
        //
        // Testing:
        //   template<class TYPE> void deleteObject(const TYPE *);
        //   void deleteObject(bsl::nulptr_t);
        // --------------------------------------------------------------------

        if (verbose) printf("\nMEMBER TEMPLATE METHOD `deleteObject` TEST"
                            "\n==========================================\n");

        if (veryVerbose) printf("\nTesting `deleteObject`:\n");
        {
            CountingNewDeleteAlloc myA;  bslma::Allocator& a = myA;

            if (veryVerbose) printf("\twith a my_Class1 object\n");

            if (veryVerbose) { T_;  T_;  P(globalObjectStatus); }
            ASSERT(0 == globalObjectStatus);

            ASSERT(0 == myA.getCount());
            my_Class1 *pC1 = (my_Class1 *) a.allocate(sizeof(my_Class1));
            const my_Class1 *pC1CONST = pC1;
            ASSERT(1 == myA.getCount());

            new(pC1) my_Class1;
            if (veryVerbose) { T_;  T_;  P(globalObjectStatus); }
            ASSERT(1 == globalObjectStatus);

            ASSERT(1 == myA.getCount());
            a.deleteObject(pC1CONST);
            if (veryVerbose) { T_;  T_;  P(globalObjectStatus); }
            ASSERT(0 == globalObjectStatus);
            ASSERT(2 == myA.getCount());

            if (veryVerbose) printf("\twith a my_Class2 object\n");

            if (veryVerbose) { T_;  T_;  P(globalObjectStatus); }
            ASSERT(0 == globalObjectStatus);

            ASSERT(2 == myA.getCount());
            my_Class2 *pC2 = (my_Class2 *) a.allocate(sizeof(my_Class2));
            const my_Class2 *pC2CONST = pC2;
            ASSERT(3 == myA.getCount());

            new(pC2) my_Class2;
            if (veryVerbose) { T_;  T_;  P(globalObjectStatus); }
            ASSERT(1 == globalObjectStatus);

            ASSERT(3 == myA.getCount());
            a.deleteObject(pC2CONST);
            if (veryVerbose) { T_;  T_;  P(globalObjectStatus); }
            ASSERT(0 == globalObjectStatus);
            ASSERT(4 == myA.getCount());

            if (veryVerbose) printf("\tWith a my_Class3Base object\n");

            ASSERT(0 == class3ObjectCount);
            my_Class3 *pC3 = (my_Class3 *) a.allocate(sizeof(my_Class3));
            const my_Class3Base *pC3bCONST = pC3;
            ASSERT(0 == class3ObjectCount);
            ASSERT(0 == globalObjectStatus);
            ASSERT(5 == myA.getCount());

            new(pC3) my_Class3;
            if (veryVerbose) { T_;  T_;  P(class3ObjectCount); }
            ASSERT(1 == class3ObjectCount);
            ASSERT(0 == globalObjectStatus);

            ASSERT(5 == myA.getCount());
            a.deleteObject(pC3bCONST);
            if (veryVerbose) { T_;  T_;  P(class3ObjectCount); }
            ASSERT(0 == class3ObjectCount);
            ASSERT(0 == globalObjectStatus);
            ASSERT(6 == myA.getCount());

            if (veryVerbose) printf("\tWith a null my_Class3 pointer\n");

            pC3 = 0;
            a.deleteObject(pC3);
            if (veryVerbose) { T_;  T_;  P(class3ObjectCount); }
            ASSERT(0 == class3ObjectCount);
            ASSERT(0 == globalObjectStatus);
            ASSERT(6 == myA.getCount());

            if (veryVerbose) printf("\tWith a null pointer literal\n");

            a.deleteObject(0);
            ASSERT(6 == myA.getCount());

#if defined(BSLS_COMPILERFEATURES_SUPPORT_NULLPTR)
            a.deleteObject(nullptr);
            ASSERT(6 == myA.getCount());
#endif // BSLS_COMPILERFEATURES_SUPPORT_NULLPTR
        }
        {
            CountingNewDeleteAlloc myA;  bslma::Allocator& a = myA;

            if (veryVerbose) printf("\tdeleteObject(my_MostDerived*)\n");

            ASSERT(0 == myA.getCount());
            my_MostDerived *pMost =
                         (my_MostDerived *) a.allocate(sizeof(my_MostDerived));
            const my_MostDerived *pMostCONST = pMost;
            ASSERT(1 == myA.getCount());

            ASSERT(0 == mostDerivedObjectCount);
            ASSERT(0 == rightBaseObjectCount);
            ASSERT(0 == leftBaseObjectCount);
            ASSERT(0 == virtualBaseObjectCount);
            new(pMost) my_MostDerived;
            ASSERT(1 == mostDerivedObjectCount);
            ASSERT(1 == rightBaseObjectCount);
            ASSERT(1 == leftBaseObjectCount);
            ASSERT(1 == virtualBaseObjectCount);

            a.deleteObject(pMostCONST);
            ASSERT(2 == myA.getCount());
            ASSERT(0 == mostDerivedObjectCount);
            ASSERT(0 == rightBaseObjectCount);
            ASSERT(0 == leftBaseObjectCount);
            ASSERT(0 == virtualBaseObjectCount);

            if (veryVerbose) printf("\tdeleteObject(my_LeftBase*)\n");

            pMost = (my_MostDerived *) a.allocate(sizeof(my_MostDerived));
            ASSERT(3 == myA.getCount());

            new(pMost) my_MostDerived;
            const my_LeftBase *pLeftCONST = pMost;
            ASSERT(1 == mostDerivedObjectCount);
            ASSERT(1 == rightBaseObjectCount);
            ASSERT(1 == leftBaseObjectCount);
            ASSERT(1 == virtualBaseObjectCount);

            a.deleteObject(pLeftCONST);
            ASSERT(4 == myA.getCount());
            ASSERT(0 == mostDerivedObjectCount);
            ASSERT(0 == rightBaseObjectCount);
            ASSERT(0 == leftBaseObjectCount);
            ASSERT(0 == virtualBaseObjectCount);

            if (veryVerbose) printf("\tdeleteObject(my_RightBase*)\n");

            pMost = (my_MostDerived *) a.allocate(sizeof(my_MostDerived));
            ASSERT(5 == myA.getCount());

            new(pMost) my_MostDerived;
            const my_RightBase *pRightCONST = pMost;
            ASSERT(1 == mostDerivedObjectCount);
            ASSERT(1 == rightBaseObjectCount);
            ASSERT(1 == leftBaseObjectCount);
            ASSERT(1 == virtualBaseObjectCount);

            ASSERT(static_cast<const void *>(pRightCONST) !=
                   static_cast<const void *>(pMost));

            a.deleteObject(pRightCONST);
            ASSERT(6 == myA.getCount());
            ASSERT(0 == mostDerivedObjectCount);
            ASSERT(0 == rightBaseObjectCount);
            ASSERT(0 == leftBaseObjectCount);
            ASSERT(0 == virtualBaseObjectCount);

            if (veryVerbose) printf("\tdeleteObject(my_VirtualBase*)\n");

            pMost = (my_MostDerived *) a.allocate(sizeof(my_MostDerived));
            ASSERT(7 == myA.getCount());

            new(pMost) my_MostDerived;
            const my_VirtualBase *pVirtualCONST = pMost;
            ASSERT(1 == mostDerivedObjectCount);
            ASSERT(1 == rightBaseObjectCount);
            ASSERT(1 == leftBaseObjectCount);
            ASSERT(1 == virtualBaseObjectCount);

            a.deleteObject(pVirtualCONST);
            ASSERT(8 == myA.getCount());
            ASSERT(0 == mostDerivedObjectCount);
            ASSERT(0 == rightBaseObjectCount);
            ASSERT(0 == leftBaseObjectCount);
            ASSERT(0 == virtualBaseObjectCount);
        }

      } break;
      case 3: {
        // --------------------------------------------------------------------
        // `memory_resource` METHODS
        //   `bslma::Allocator` is unlike other protocol classes in that it
        //   contains a small amount of implementation, not just an interface.
        //   The `do_allocate` and `do_deallocate` virtual methods that it
        //   inherits from `memory_resource` have default implementations that
        //   adapt and forward calls to the traditional BDE allocator
        //   interface.  The implementation of these adaptor methods are tested
        //   in this test case.
        //
        // Concerns:
        // 1. A class, `my_Allocator`, derived from `bslma::Allocator`, which
        //    overrides `allocate(size_type)` and `deallocate(p)` but does not
        //    override `do_allocate`, `do_deallocate`, or `do_is_equal` is a
        //    concrete class.
        // 2. An object, `alloc` of this type can be used through the
        //    `bslma::Allocator` interface (`alloc.allocate(bytes)` and
        //    `alloc.deallocate(p)`.
        // 3. Upcasting `alloc` to a reference, `mr` to `bsl::memory_resource`,
        //    allows it to be used through the `memory_resource` interface
        //    (`mr.allocate(bytes, align)` and `mr.deallocate(p, bytes,
        //    align)`).
        // 4. When `bytes == 0`, `mr.allocate(bytes, align)` invokes
        //    `alloc.allocate(0)` then returns a non-null pointer distinct from
        //    any potential object pointer.  The `align` argument is ignored
        //    and same pointer is returned each time `mr.allocate` is called.
        // 5. The pointer returned from `mr.allocate(bytes, align)` refers
        //    to an address having at least the requested alignment.
        //    Specifically, for `natAlign` being the natural alignment for an
        //    object of `bytes` size:
        //
        //    1. When `align <= natAlign`, `mr.allocate(bytes, align)` is
        //       identical to `alloc.allocate(bytes)`.
        //    2. When `natAlign < align && align <= k_MAX_ALIGNMENT`,
        //       `mr.allocate(bytes, align)` returns `alloc.allocate(bytes2)`,
        //       where `bytes2 > bytes`.
        //    3. When `natAlign > k_MAX_ALIGNMENT`, `mr_allocate(bytes, align)`
        //       `mr.allocate(bytes, align)` invokes `alloc.allocate(bytes2)`,
        //       where `bytes2 > bytes` and modifies the return pointer such
        //       that it points within the same block, but does not point to
        //       the start of of the block.
        //
        // 6. Given `p = mr.allocate(bytes, align), `mr.deallocate(p,
        //    bytes, align)` invokes `alloc.deallocate(p2)`, where `p2` is
        //    the address originally returned by `alloc.allocate`, regardless
        //    of the value of `align`.
        // 7. Given two `bslma::Allocator` references, `a1` and `a2`, refering
        //    to `my_Allocator` objects, `a1.is_equal(a2)` returns `true` if
        //    `a1` and `a2` refer to the same object and `false` otherwise.
        //
        // Plan:
        // 1. Define a class, `my_Allocator`, derived from `bslma::Allocator`,
        //    that overrides the `allocate` and `deallocate` virtual functions
        //    but not the `do_allocate`, `do_deallocate`, or `do_is_equal`
        //    virtual functions.  The `allocate` function always returns a
        //    block exactly aligned for the natural alignment of the supplied
        //    size argument, up to a maximum alignment of `k_MAX_ALIGNMENT`.
        // 2. Create an object, `alloc` of type `my_Allocator`.  (C-1)
        // 3. Create a reference, `mr`, of type `bsl::memory_resource&`, to
        //    `alloc`.  Loop over sizes from 0 to 2048 bytes and alignments
        //    from 1 to 256 (powers of 2 only).  For each combination:
        //
        //    1. Verify that `alloc.allocate(bytes)` and `alloc.deallocate(p)`
        //       are callable and produce expected results.  (C-2)
        //    2. Verify that `mr.allocate(bytes, align)` and `mr.deallocate(p,
        //       bytes, align)` are callable and produce expected results.
        //       (C-3)
        //    3. Verify that calling `mr.allocate(bytes, align)` always returns
        //       a correctly aligned result from the expected corresponding
        //       call to `allocate` through the `bslma::Allocator` interface.
        //       (C-4, C-5)
        //    4. Verify that calling `mr.deallocate(p, bytes, align)` always
        //       returns invokes the expected corresponding invocation of
        //       `deallocate` through the `bslma::Allocator` interface.  (C-6)
        //
        // 4. Create a second `my_Allocator` object, `alloc2`.  Verify that
        //    `alloc.is_equal(alloc)` returns `true` and that
        //    `alloc.is_equal(alloc2)` returns `false`.  (C-7)
        //
        // Testing:
        //      void* do_allocate(std::size_t, std::size_t);
        //      void do_deallocate(void *p, std::size_t, std::size_t);
        //      bool do_is_equal(const bsl::memory_resource&) const;
        // --------------------------------------------------------------------

        if (verbose) printf("\n`memory_resource` METHODS"
                            "\n=========================\n");

        my_Allocator alloc;     const my_Allocator& ALLOC  = alloc;
        my_Allocator alloc2;    const my_Allocator& ALLOC2 = alloc2;

        bsl::memory_resource& mr = alloc;  const bsl::memory_resource& MR = mr;

        // Always expect the same pointer when allocating 0 bytes through the
        // `memory_resouce` interface.
        void *const zeroAlloc_p = mr.allocate(0, 1);
        ASSERTV(zeroAlloc_p, 0 != zeroAlloc_p);

        for (std::size_t bytes = 0; bytes <= 2048; ++bytes) {

            std::size_t natAlign = bytes > 0 ?
                (std::size_t) Align::calculateAlignmentFromSize(bytes) : 0;

            {
                // Use `bslma::Allocator` interface.
                void *p = alloc.allocate(bytes);
                ASSERTV(bytes, natAlign, p, ALLOC.lastOp() == e_ALLOC);
                ASSERTV(bytes, natAlign, p,
                        Align::pointerAlignment(p) == natAlign);
                ASSERTV(bytes, natAlign, p, ALLOC.lastBlock() == p);

                alloc.deallocate(p);
                ASSERTV(bytes, natAlign, p, ALLOC.lastOp() == e_DEALLOC);
                ASSERTV(bytes, natAlign, p, ALLOC.lastBlock() == p);
            }

            for (std::size_t align = 1; align <= 256; align <<= 1) {

                // Use `bsl::memory_resource` interface
                void *p = mr.allocate(bytes, align);
                ASSERTV(bytes, align, p, ALLOC.lastOp() == e_ALLOC);

                // Actual allocated block; contains returned block (`p`) but
                // might be bigger than block `p`.
                void *block = ALLOC.lastBlock();

                if (0 == bytes) {
                    // Special case for 0-sized allocation
                    ASSERTV(bytes, align, p, zeroAlloc_p      == p);
                    ASSERTV(bytes, align, p, block            == 0);
                    ASSERTV(bytes, align, p, ALLOC.lastSize() == 0);
                }
                else if (align > k_MAX_ALIGNMENT) {
                    // Over-aligned allocation:
                    ASSERTV(bytes, align, p, Align::isAligned(p, align));

                    // Verify that `p` is within `block`.
                    ASSERTV(bytes, align, p, block, (char*)block < (char*)p);
                    ASSERTV(bytes, align, p, block,
                            (char*)p-(char*)block + bytes <= ALLOC.lastSize());
                }
                else if (align > natAlign) {
                    // allocation aligned to more than natural alignment
                    ASSERTV(bytes, align, p, Align::isAligned(p, align));
                    ASSERTV(bytes, align, p, block            == p);
                    ASSERTV(bytes, align, p, ALLOC.lastSize() >  bytes);
                }
                else {
                    // allocation aligned to natural alignment or less
                    ASSERTV(bytes, align, p, Align::isAligned(p, align));
                    ASSERTV(bytes, align, p, block            == p);
                    ASSERTV(bytes, align, p, ALLOC.lastSize() == bytes);
                }

                mr.deallocate(p, bytes, align);
                ASSERTV(p, bytes, align, ALLOC.lastOp()    == e_DEALLOC);
                ASSERTV(p, bytes, align, ALLOC.lastBlock() == block);
            }

            ASSERT(  MR.is_equal(ALLOC));
            ASSERT(! MR.is_equal(ALLOC2));
            ASSERT(  ALLOC2.is_equal(ALLOC2));
            ASSERT(! ALLOC2.is_equal(MR));
        }

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TEST HARNESS
        //   Test the utilities used to test this component.
        //
        // Concerns:
        // 1. The `my_Allocator::allocate` method records most recent
        //    allocation operation, the returned block, and the block`s size,
        //    and makes them available via the `lastOp`, lastBlock`, and
        //    `lastSize` accessors.
        // 2. The `my_Allocator::allocate` method returns a block exactly
        //    aligned to the natural alignment for its size argument.
        // 3. The `my_Allocator::deallocate` method records the most recent
        //    operation as being a deallocation.
        //
        // Plan:
        // 2. For a list of sizes from 1 to `2 * k_MAX_ALIGNEMNT`, verify that
        //    the `allocate` method returns an address that is correctly
        //    aligned and is not overaligned, and that `lastOp, `lastSize`, and
        //    `lastBlock` return the correct values.  (C-1, C-2)
        // 3. For each block allocated in step 2, verify that `deallocate` can
        //    be invoked and that `lastOp` subsequently reflects the
        //    deallocation.  (C-3)
        //
        // Testing:
        //     TEST HARNESS
        // --------------------------------------------------------------------

        if (verbose) printf("\nTEST HARNESS"
                            "\n============\n");

        if (veryVerbose) printf("\tTesting `my_Allocator`\n");
        {
            my_Allocator mA;  const my_Allocator& A = mA;

            for (int size = 1; size <= 2 * k_MAX_ALIGNMENT; ++size) {

                void *p = mA.allocate(size);
                ASSERT(A.lastOp()          == e_ALLOC);
                ASSERT((int) A.lastSize()  == size);
                ASSERT(A.lastBlock()       == p);
                ASSERT(A.allocateCount()   == size);
                ASSERT(A.deallocateCount() == size - 1);

                std::size_t expAlign = Align::calculateAlignmentFromSize(size);
                ASSERT(Align::pointerAlignment(p) == expAlign);

                mA.deallocate(p);
                ASSERT(A.lastOp()          == e_DEALLOC);
                ASSERT(A.lastBlock()       == p);
                ASSERT(A.allocateCount()   == size);
                ASSERT(A.deallocateCount() == size);
            }

            // special case for zero-sized allocations
            ASSERT(mA.allocate(0) == 0);
            ASSERT(A.lastOp()     == e_ALLOC);
            ASSERT(A.lastBlock()  == 0);

            mA.deallocate(0);
            ASSERT(A.lastOp()     == e_DEALLOC);
            ASSERT(A.lastBlock()  == 0);
        }

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // PROTOCOL TEST:
        //   Ensure this class is a properly defined protocol.
        //
        // Concerns:
        // 1. The protocol is abstract: no objects of it can be created.
        // 2. The protocol has no data members.
        // 3. The protocol has a virtual destructor.
        // 4. All methods of the `bslma::Allocator` protocol are publicly
        //    accessible virtual functions.
        // 5. All virtual methods inherited from `bsl::memory_resource` are
        //    available through public pass-through functions
        // 6. The methods inherited from `bsl::memory_resource` are not pure
        //    virtual in the `bslma::Allocator` derived class. If they are not
        //    overriden, `do_allocate` and `do_deallocate` call the (overriden)
        //    `allocate` and `deallocate` methods, respectively.
        //
        // Plan:
        // 1. Define a concrete derived implementation of the protocol,
        //    `AllocatorProtocolTest`, that overrides all of the virtual
        //    methods and records when they are called.
        // 2. Create an object of the `bsls::ProtocolTest` class template
        //    parameterized by `AllocatorProtocolTest`, and use it to verify
        //    that:
        //
        //    1. The protocol is abstract. (C-1)
        //    2. The protocol has no data members. (C-2)
        //    3. The protocol has a virtual destructor. (C-3)
        //
        // 3. Use the `BSLS_PROTOCOLTEST_ASSERT` macro to verify that
        //    non-creator methods of the `bslma::Allocator` protocol are
        //    virtual and publicly available.  (C-4)
        // 4. Use the `BSLS_PROTOCOLTEST_ASSERT` macro to verify that
        //    non-creator methods inherited from `bsl::memory_resource` are
        //    virtual and available through public pass-through functions.
        //    (C-5)
        // 5. Define a concrete derivded implementation of the protocol,
        //    `IndirectProtocolTest`, that overrides the new (pure) virtual
        //    methods introduced by `bslma::Allocator` but not the (non-pure)
        //    virtual methods inherited from `bsl::memory_resource`.  Repeat
        //    steps 2-4 with this `IndirectProtocolTest`.
        //
        // Testing:
        //      ~Allocator();
        //      void *allocate(size_type);
        //      void deallocate(void *);
        // --------------------------------------------------------------------

        if (verbose) printf("\nPROTOCOL TEST"
                            "\n=============\n");

        bsls::ProtocolTest<AllocatorProtocolTest> testObj(veryVerbose);

        ASSERT(testObj.testAbstract());
        ASSERT(testObj.testNoDataMembers());
        ASSERT(testObj.testVirtualDestructor());

        // Create a reference to `bslma::Allocator` to test protocol.
        const bslma::Allocator& other = AllocatorProtocolTest();
        void *p = 0;

        // Test `bslma::Allocator` protocol.  Note that the base-class
        // `allocate` and `deallocate` non-virtual functions are hidden.
        BSLS_PROTOCOLTEST_RV_ASSERT(testObj, allocate(2), p);
        BSLS_PROTOCOLTEST_ASSERT(testObj, deallocate(p));

        // Test `bsl::memory_resource` base-class protocol via pass-through
        // functions.  Note that the base-class `allocate` and `deallocate` are
        // hidden within `bslma::Allocator` and must be qualified in the method
        // call.
        typedef bsl::memory_resource Base;
        BSLS_PROTOCOLTEST_RV_ASSERT(testObj, Base::allocate(2, 1), p);
        BSLS_PROTOCOLTEST_ASSERT(testObj, Base::deallocate(p, 2, 1));
        BSLS_PROTOCOLTEST_ASSERT(testObj, is_equal(other));

        bsls::ProtocolTest<IndirectProtocolTest> testIndirect(veryVerbose);

        ASSERT(testIndirect.testAbstract());
        ASSERT(testIndirect.testNoDataMembers());
        ASSERT(testIndirect.testVirtualDestructor());

        // Test `bslma::Allocator` protocol.  Note that the base-class
        // `allocate` and `deallocate` non-virtual functions are hidden.
        BSLS_PROTOCOLTEST_ASSERT(testIndirect, allocate(2));
        BSLS_PROTOCOLTEST_ASSERT(testIndirect, deallocate(p));

        // Test `bsl::memory_resource` base-class protocol via pass-through
        // functions.  Note that the base-class `allocate` and `deallocate` are
        // hidden within `bslma::Allocator` and must be qualified in the method
        // call.
        typedef bsl::memory_resource Base;
        BSLS_PROTOCOLTEST_RV_ASSERT(testIndirect, Base::allocate(2, 1), p);
        BSLS_PROTOCOLTEST_ASSERT(testIndirect, Base::deallocate(p, 2, 1));
        // The default implementation of `do_is_equal` cannot be tested because
        // it does call a virtual function that we can intercept.

        // Test zero-byte allocations.
        // TBD: This is a very basic and undocumented test.  A more
        // sophisticated test will be added as part of the fix for DRQS
        // 176364960.
        CountingNewDeleteAlloc myA;

        // Zero-byte allocation through `bslma::Allocator` interface.
        void *p1 = myA.allocate(0);
        ASSERT(0 == p1);
        ASSERT(1 == myA.getCount());    // Increments even for zero bytes

        myA.deallocate(p1);             // Can deallocate null pointer
        ASSERT(2 == myA.getCount());    // Increments even for zero bytes

        // Zero-byte allocation through `bsl::memory_resource` interface.
        bsl::memory_resource& myR = myA;
        void *p2 = myR.allocate(0);
        ASSERT(0 != p2);                // Non-zero return
        ASSERT(3 == myA.getCount());    // Increments even for zero bytes

        ASSERT(myR.allocate(0) == p2);  // Returns same pointer every time
        ASSERT(4 == myA.getCount());

        myR.deallocate(p2, 0);          // Can deallocate zero-size block
        ASSERT(5 == myA.getCount());    // Increments even for zero bytes

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
