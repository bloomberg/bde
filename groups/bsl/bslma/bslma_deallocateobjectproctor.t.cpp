// bslma_deallocateobjectproctor.t.cpp                                -*-C++-*-

#include <bslma_deallocateobjectproctor.h>

#include <bslma_allocator.h>
#include <bslma_allocatorutil.h>
#include <bslma_constructionutil.h>
#include <bslma_isstdallocator.h>
#include <bslma_bslallocator.h>
#include <bslma_testallocator.h>

#include <bslmf_nestedtraitdeclaration.h>

#include <bsls_alignmentfromtype.h>
#include <bsls_bsltestutil.h>
#include <bsls_nullptr.h>
#include <bsls_objectbuffer.h>

#include <cstdio>      // `printf`
#include <cstdlib>     // `atoi`

using namespace BloombergLP;

//=============================================================================
//                                 TEST PLAN
//-----------------------------------------------------------------------------
//                                  Overview
//                                  --------
// We are testing a proctor object to ensure that it deallocates memory for the
// managed objects if `release` is not called before the proctor object goes
// out of scope.  We achieve this goal by using a test allocator that tracks
// allocations and deallocations.  After the proctor is destroyed, we verify
// that any memory blocks allocated for the object are deallocated.  The
// proctor is instantiated with `bsl::allocator`, `bslma::TestAllocator *`, and
// a custom pool class pointer to test that it correctly calls the correct
// interface for the memory supplier.
//-----------------------------------------------------------------------------
// [3] DeallocateObjectProctor(const ALLOCATOR&, TYPE *, std::size_t = 1);
// [6] DeallocateObjectProctor(DeallocateObjectProctor&& original);
// [3] ~DeallocateObjectProctor();
// [3] TYPE *ptr() const;
// [4] TYPE *release();
// [5] void reset(TYPE *ptr, std::size_t n = 1);
//-----------------------------------------------------------------------------
// [1] BREATHING TEST
// [2] Helper Class: `my_Class`
// [2] Helper Class: `my_Pool`
// [2] Helper Class: `EnhancedTestAllocator`
// [7] USAGE EXAMPLES
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
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

typedef bslma::AllocatorUtil  AllocUtil;
typedef bslmf::MovableRefUtil MoveUtil;

// Test allocations with these values of `n`:
const std::size_t k_SIZES[]   = { 1, 2, 3, 4, 8, 12, 32, 63, 64 };
const std::size_t k_NUM_SIZES = sizeof(k_SIZES) / sizeof(k_SIZES[0]);

//=============================================================================
//                          HELPER CLASS FOR TESTING
//-----------------------------------------------------------------------------

/// A version of `bslma::TestAllocator` that records the size and alignment
/// of the last four allocations and matches them against their
/// correpsonding deallocations.
class EnhancedTestAllocator : public bslma::TestAllocator {

    enum { k_NUM_CACHE_SLOTS = 4 };

    typedef bslma::TestAllocator Base;

    struct CacheEntry {
        void        *d_ptr;
        std::size_t  d_bytes;
        std::size_t  d_align;

        CacheEntry() : d_ptr(0), d_bytes(0), d_align(0) { }
    };

    CacheEntry  d_cache[k_NUM_CACHE_SLOTS];
    std::size_t d_nextCacheSlot;

  protected:
    // PROTECTED MANIPULATORS
    void* do_allocate(std::size_t bytes,
                      std::size_t alignment) BSLS_KEYWORD_OVERRIDE;
    void do_deallocate(void        *p,
                       std::size_t  bytes,
                       std::size_t  alignment) BSLS_KEYWORD_OVERRIDE;

  public:
    // CREATORS
    explicit
    EnhancedTestAllocator(bslma::Allocator *basicAllocator = 0)
        : Base(basicAllocator), d_nextCacheSlot(0) { }
    explicit
    EnhancedTestAllocator(const char       *name,
                          bslma::Allocator *basicAllocator = 0)
        : Base(name, basicAllocator), d_nextCacheSlot(0) { }
    explicit
    EnhancedTestAllocator(bool              verboseFlag,
                          bslma::Allocator *basicAllocator = 0)
        : Base(verboseFlag, basicAllocator) , d_nextCacheSlot(0) { }
    EnhancedTestAllocator(const char       *name,
                          bool              verboseFlag,
                          bslma::Allocator *basicAllocator = 0)
        : Base(name, verboseFlag, basicAllocator) , d_nextCacheSlot(0) { }
};

void *EnhancedTestAllocator::do_allocate(std::size_t bytes,
                                         std::size_t alignment)
{
    // Call base-class allocate function
    void *ret = Base::do_allocate(bytes, alignment);

    // Store size and alignment in cache
    CacheEntry& cs = d_cache[d_nextCacheSlot];
    d_nextCacheSlot = (d_nextCacheSlot + 1) % k_NUM_CACHE_SLOTS;
    cs.d_ptr = ret;
    cs.d_bytes = bytes;
    cs.d_align = alignment;

    return ret;
}

void EnhancedTestAllocator::do_deallocate(void        *p,
                                          std::size_t  bytes,
                                          std::size_t  alignment)
{
    ASSERT(p);

    for (std::size_t i = 0; i < k_NUM_CACHE_SLOTS; ++i) {
        CacheEntry& cs = d_cache[i];
        if (cs.d_ptr == p) {
            ASSERTV(p, cs.d_align, alignment, cs.d_align == alignment);
            ASSERTV(p, cs.d_bytes, bytes,     cs.d_bytes == bytes);
            cs.d_ptr = 0;
            break;
        }
    }

    // Call base-class deallocate function
    Base::do_deallocate(p, bytes, alignment);
}

/// A pointer-like class for testing allocators with non-raw pointers.
template <class TYPE>
class FancyPointer
{

    // DATA
    TYPE *m_pointer;

  public:
    // CREATORS
    explicit FancyPointer(TYPE *p = 0) : m_pointer(p) { }
    FancyPointer(bsl::nullptr_t) : m_pointer(0) { }

    // ACCESSORS
    TYPE *get()        const { return m_pointer; }
    TYPE *operator->() const { return m_pointer; }
    TYPE& operator*()  const { return *m_pointer; }
    operator bool()  const { return 0 != m_pointer; }
    bool operator!() const { return 0 == m_pointer; }

    // HIDDEN FRIENDS
    friend bool operator==(FancyPointer a, FancyPointer b)
        { return a.m_pointer == b.m_pointer; }
    friend bool operator!=(FancyPointer a, FancyPointer b)
        { return a.m_pointer != b.m_pointer; }
};

/// Allocator with non-raw pointer type
template <class TYPE>
class FancyAllocator
{

    // DATA
    bsl::memory_resource *m_resource;

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(FancyAllocator, bslma::IsStdAllocator);

    // TYPES
    typedef TYPE                     value_type;
    typedef FancyPointer<TYPE>       pointer;
    typedef FancyPointer<const TYPE> const_pointer;
    typedef FancyPointer<void>       void_pointer;
    typedef FancyPointer<const void> const_void_pointer;

    // CREATORS
    FancyAllocator(bsl::memory_resource *r) : m_resource(r) { } // IMPLICIT

    template <class OTHER>
    FancyAllocator(const FancyAllocator<OTHER>& other)
        : m_resource(other.resource()) { }

    // MANIPULATORS
    pointer allocate(std::size_t n) {
        void *p = m_resource->allocate(sizeof(TYPE) * n,
                                       bsls::AlignmentFromType<TYPE>::VALUE);
        return pointer(static_cast<TYPE *>(p));
    }

    void deallocate(pointer p, std::size_t n) {
        m_resource->deallocate(p.get(), sizeof(TYPE) * n,
                               bsls::AlignmentFromType<TYPE>::VALUE);
    }

    // ACCESSORS
    bsl::memory_resource *resource() const { return m_resource; }

    // HIDDEN FRIENDS
    friend bool operator==(const FancyAllocator& a, const FancyAllocator& b)
        { return *a.resource() == *b.resource(); }
    friend bool operator!=(const FancyAllocator& a, const FancyAllocator& b)
        { return *a.resource() != *b.resource(); }
};

template <class T1, class T2>
inline
bool operator==(const FancyAllocator<T1>& a, const FancyAllocator<T2>& b)
{
    return *a.resource() == *b.resource();
}

template <class T1, class T2>
inline
bool operator!=(const FancyAllocator<T1>& a, const FancyAllocator<T2>& b)
{
    return *a.resource() != *b.resource();
}

/// Non-empty test class.  Since the proctor under test does not invoke the
/// destructor, it is important that this class has a trivial destructor.
class my_Class {

    // DATA
    int d_data[2];

  public:
    // CREATORS
    explicit my_Class(int i1 = 0, int i2 = 0)
        { d_data[0] = i1; d_data[1] = i2; }

    //! ~my_Class() = default;  // Trivial destructor

    // ACCESSORS
    int operator[](std::size_t i) const { return d_data[i]; }
};

/// This class provides a `deallocate` method, used to exercise the contract
/// promised by the destructor of the `bslma::DeallocateObjectProctor`.
/// This object indicates that its `deallocate` method is called by
/// incrementing the global counter (supplied at construction) that it
/// *holds*.
class my_Pool {

    // DATA
    int *d_counter_p;  // (non-owned) counter incremented on `deallocate`

  public:
    // CREATORS

    /// Create this object holding the specified (global) counter.
    explicit my_Pool(int *counter) : d_counter_p(counter) {}

    // MANIPULATORS

    /// Increment this object's counter.
    void deallocate(void *) { ++*d_counter_p; }
};

//=============================================================================
//                                USAGE EXAMPLES
//-----------------------------------------------------------------------------

#if defined(BDE_BUILD_TARGET_EXC)

//Example 1: Class having an owning pointer
//- - - - - - - - - - - - - - - - - - - - -
// In this example, we create a class, `my_Manager`, having an owning pointer
// to an object of another class, `my_Data`.  Because it owns the `my_Data`
// object, `my_Manager` is responsible for allocating, constructing,
// deallocating, and destroying it.
//
// First, we define the `my_Data` class, which holds an integer value and
// counts how many times its constructor and destructor have been called. Its
// constructor will throw an exception if its integer argument equals the
// number of constructor calls before construction:
// ```
//  #include <bslma_allocatorutil.h>
//  #include <bslma_bslallocator.h>
//  #include <bslma_testallocator.h>

    class my_Data {

        // DATA
        int d_value;

        // CLASS DATA
        static int s_numConstructed;
        static int s_numDestroyed;

      public:
        // CLASS METHODS
        static int numConstructed() { return s_numConstructed; }
        static int numDestroyed()   { return s_numDestroyed;   }

        // CREATORS
        explicit my_Data(int v) : d_value(v)
        {
            if (v == s_numConstructed) throw s_numConstructed;
            ++s_numConstructed;
        }
        my_Data(const my_Data& original);
        ~my_Data() { ++s_numDestroyed; }
    };

    int my_Data::s_numConstructed = 0;
    int my_Data::s_numDestroyed   = 0;
// ```
// Next, we define `my_Manager` as an allocator-aware class holding a pointer
// to `my_Data` and maintaining its own count of constructor invocations:
// ```
    class my_Manager {

        // DATA
        bsl::allocator<my_Data>  d_allocator;
        my_Data                 *d_data_p;

        // CLASS DATA
        static int s_numConstructed;

      public:
        // TYPES
        typedef bsl::allocator<> allocator_type;

        // CLASS METHODS
        static int numConstructed() { return s_numConstructed; }

        // CREATORS
        explicit my_Manager(int                   v,
                          const allocator_type& allocator = allocator_type());
        my_Manager(const my_Manager& original);
        ~my_Manager();

        // ...
    };

    int my_Manager::s_numConstructed = 0;
// ```
// Next, we define the constructor for `my_Manager`, which begins by allocating
// a `my_Data` object:
// ```
    my_Manager::my_Manager(int v, const allocator_type& allocator)
        : d_allocator(allocator), d_data_p(0)
    {
        d_data_p = bslma::AllocatorUtil::allocateObject<my_Data>(allocator);
// ```
// Then, the `my_Manager` constructor constructs the `my_Data` object in the
// allocated memory.  However, as the constructor might throw it first protects
// the data object with a `bslma::DeallocateObjectProctor`:
// ```
        bslma::DeallocateObjectProctor<allocator_type, my_Data>
                                                proctor(d_allocator, d_data_p);
        bslma::ConstructionUtil::construct(d_data_p, d_allocator, v);
// ```
// Then, once the `construct` operation completes successfully, we can release
// the data object from the proctor.  Only then do we increment the
// construction count:
// ```
        proctor.release();
        ++s_numConstructed;
    }
// ```
// Next, we define the `my_Manager` destructor, which destroys and deallocates
// its data object:
// ```
    my_Manager::~my_Manager()
    {
        d_data_p->~my_Data();
        bslma::AllocatorUtil::deallocateObject(d_allocator, d_data_p);
    }
// ```
// Now, we use a `bslma::TestAllocator` to verify that, under normal (non
// exceptional) circumstances, constructing a `my_Manager` object will result
// in one block of memory being allocated and one invocation of the `my_Data`
// constructor:
// ```
    void usageExample1()
    {
        bslma::TestAllocator ta;

        {
            my_Manager obj1(7, &ta);
            ASSERT(1 == ta.numBlocksInUse());
            ASSERT(1 == ta.numBlocksTotal());
            ASSERT(1 == my_Manager::numConstructed());
        }
        ASSERT(0 == ta.numBlocksInUse());
        ASSERT(1 == ta.numBlocksTotal());
        ASSERT(1 == my_Manager::numConstructed());
// ```
// Finally, when the `my_Data` constructor does throw, a block is allocated but
// we verify that the `my_Manager` constructor did not complete and that the
// block was deallocated, resulting in no leaks:
// ```
        try {
            my_Manager obj2(1, &ta);
            ASSERT(false && "Can't get here");
        }
        catch (int e) {
            ASSERT(1 == e);
            ASSERT(0 == ta.numBlocksInUse());
            ASSERT(2 == ta.numBlocksTotal());
            ASSERT(1 == my_Manager::numConstructed());
        }
        ASSERT(1 == my_Manager::numConstructed());
    }
// ```
#endif // defined(BDE_BUILD_TARGET_EXC)

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

    (void)veryVerbose;           // suppress unused variable warning
    (void)veryVeryVerbose;       // suppress unused variable warning

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:  // Zero is always the leading case.
      case 7: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLES
        //
        // Concerns:
        // 1. That the usage examples shown in the component-level
        //    documentation compile and run as described.
        //
        // Plan:
        // 1. Copy the usage examples from the component header, changing
        //    `assert` to `ASSERT` and execute them.
        //
        // Testing:
        //     USAGE EXAMPLES
        // --------------------------------------------------------------------

        if (verbose) printf("\nUSAGE EXAMPLES"
                            "\n==============\n");

#if defined(BDE_BUILD_TARGET_EXC)
        usageExample1();
#else
        if (veryVerbose) printf("Test not run without exception support.\n");
#endif
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // MOVE CONSTRUCTOR TEST
        //
        // Concerns:
        // 1. Move-constructing one `bslma::DeallocateObjectProctor` from
        //    another results in the new proctor engaging the same object with
        //    the same allocator as the original.
        // 2. After the move, the original object is disengaged, but retains
        //    its allocator.
        // 3. Move-constructing a disengaged proctor yields a second disengaged
        //    proctor using the same allocator.
        // 4. The above concerns apply for all allocator/pool categories
        //    supported by this component.
        //
        // Plan:
        // 1. Construct a `bslma::DeallocateObjectProctor` managing a class
        //    object.  Copy-construct a second
        //    `bslma::DeallocateObjectProctor`.  Verify that `ptr()` returns
        //    the original class object and that no deallocations occured.
        //    Verify that when the second proctor goes out of scope, the
        //    original object is deallocated.  (C-1)
        // 2. Verify that the original (first) proctor from step 1 is
        //    disengaged (`ptr()` returns null).  Reset it to manage a new
        //    object.  When the first proctor goes out of scope, verify that it
        //    deallocates the new object.  (C-2)
        // 3. Create a third, disengaged `bslma::DeallocateObjectProctor`, then
        //    move-construct it, creating a fourth proctor.  Verify that the
        //    third and fourth proctors are both disengaged.  Reset the fourth
        //    proctor to manage a new object.  When the fourth proctor goes out
        //    of scope, verify that it deallocates the new object.  (C-3)
        // 4. Repeat the preceding steps using `bsl::allocator`,
        //    `bslma::Allocator *`, and `my_Pool *` for the allocator type.
        //
        // Testing:
        //     DeallocateObjectProctor(DeallocateObjectProctor&& original);
        // --------------------------------------------------------------------

        if (verbose) printf("\nMOVE CONSTRUCTOR TEST"
                            "\n=====================\n");

        bslma::TestAllocator ta;

        if (veryVerbose) printf("Test with bsl::allocator\n");
        {
            typedef bsl::allocator<>                                Alloc;
            typedef bslma::DeallocateObjectProctor<Alloc, my_Class> Obj;

            Alloc alloc(&ta);
            my_Class *p1 = AllocUtil::allocateObject<my_Class>(alloc);
            my_Class *p2 = 0, *p3 = 0;
            Obj proctorA(alloc, p1);  // Engaged
            Obj proctorC(alloc, 0);   // Disenaged
            {
                // Move engaged proctor
                Obj proctorB(MoveUtil::move(proctorA));
                ASSERT(p1 == proctorB.ptr());
                ASSERT(0  == proctorA.ptr());

                p2 = AllocUtil::allocateObject<my_Class>(alloc);
                proctorA.reset(p2);  // Re-engage with same allocator
                ASSERT(sizeof(my_Class) * 2 == ta.numBytesInUse());

                // Move disengaged proctor
                Obj proctorD(MoveUtil::move(proctorC));
                ASSERT(0 == proctorC.ptr());
                ASSERT(0 == proctorD.ptr());

                p3 = AllocUtil::allocateObject<my_Class>(alloc);
                proctorD.reset(p3);  // Engage with same allocator
                ASSERT(sizeof(my_Class) * 3 == ta.numBytesInUse());
            }
            ASSERT(p2               == proctorA.ptr());
            ASSERT(sizeof(my_Class) == ta.numBytesInUse());
        }
        ASSERT(0 == ta.numBytesInUse());

        if (veryVerbose) printf("Test with bslma::TestAllocator*\n");
        {
            typedef bslma::TestAllocator                            *Alloc;
            typedef bslma::DeallocateObjectProctor<Alloc, my_Class>  Obj;

            Alloc alloc(&ta);
            my_Class *p1 = AllocUtil::allocateObject<my_Class>(alloc);
            my_Class *p2 = 0, *p3 = 0;
            Obj proctorA(alloc, p1);  // Engaged
            Obj proctorC(alloc, 0);   // Disenaged
            {
                // Move engaged proctor
                Obj proctorB(MoveUtil::move(proctorA));
                ASSERT(p1 == proctorB.ptr());
                ASSERT(0  == proctorA.ptr());

                p2 = AllocUtil::allocateObject<my_Class>(alloc);
                proctorA.reset(p2);  // Re-engage with same allocator
                ASSERT(sizeof(my_Class) * 2 == ta.numBytesInUse());

                // Move disengaged proctor
                Obj proctorD(MoveUtil::move(proctorC));
                ASSERT(0 == proctorC.ptr());
                ASSERT(0 == proctorD.ptr());

                p3 = AllocUtil::allocateObject<my_Class>(alloc);
                proctorD.reset(p3);  // Engage with same allocator
                ASSERT(sizeof(my_Class) * 3 == ta.numBytesInUse());
            }
            ASSERT(p2               == proctorA.ptr());
            ASSERT(sizeof(my_Class) == ta.numBytesInUse());
        }
        ASSERT(0 == ta.numBytesInUse());

        if (veryVerbose) printf("Test with my_Pool\n");
        int deallocCounter = 0;
        bsls::ObjectBuffer<my_Class> ob1, ob2, ob3;
        {
            typedef my_Pool                                         *Alloc;
            typedef bslma::DeallocateObjectProctor<Alloc, my_Class>  Obj;

            my_Pool pool(&deallocCounter);
            Alloc  alloc(&pool);

            my_Class *p1 = &ob1.object();
            my_Class *p2 = 0, *p3 = 0;
            Obj proctorA(alloc, p1);  // Engaged
            Obj proctorC(alloc, 0);   // Disenaged
            {
                // Move engaged proctor
                Obj proctorB(MoveUtil::move(proctorA));
                ASSERT(p1 == proctorB.ptr());
                ASSERT(0  == proctorA.ptr());
                ASSERT(0  == deallocCounter);

                p2 = &ob2.object();
                proctorA.reset(p2);  // Re-engage with same allocator
                ASSERT(0  == deallocCounter);

                // Move disengaged proctor
                Obj proctorD(MoveUtil::move(proctorC));
                ASSERT(0 == proctorC.ptr());
                ASSERT(0 == proctorD.ptr());
                ASSERT(0  == deallocCounter);

                p3 = &ob3.object();
                proctorD.reset(p3);  // Engage with same allocator
                ASSERT(0  == deallocCounter);
            }
            ASSERT(p2 == proctorA.ptr());
            ASSERT(2  == deallocCounter);
        }
        ASSERT(3  == deallocCounter);

      } break;
      case 5: {
        // --------------------------------------------------------------------
        // `reset` TEST
        //
        // Concerns:
        // 1. When the `reset` method is called, the proctor object properly
        //    manages a different object.
        //
        // Plan:
        // 1. Allocate `my_Class` object using a test allocator.  Allocate a
        //    separate array of two `my_Class` objects from the same allocator.
        //    Finally initialize a `bslma::DeallocateObjectProctor` object with
        //    the first block.
        // 2. Call `reset` on the proctor before it goes out of scope, passing
        //    it the second block.  Verify that the `ptr` method returns the
        //    second pointer.  (C-1)
        // 3. Once the proctor goes out of scope, verify that the memory
        //    allocated for two `my_Class` objects was deallocated.  Deallocate
        //    the first object and verify that the test allocator did not
        //    complain of a double deletion, thereby verifying that the proctor
        //    deallocated the second block, as expected.  (C-1)
        //
        // Testing:
        //   void reset(TYPE *ptr, std::size_t n = 1);
        // --------------------------------------------------------------------

        if (verbose) printf("\n'reset' TEST"
                            "\n============\n");

        EnhancedTestAllocator       z(veryVeryVeryVerbose);
        const bslma::TestAllocator& Z = z;

        my_Class *pC1, *pC2;
        {
            pC1 = AllocUtil::allocateObject<my_Class>(&z, 1);
            ASSERT(sizeof(my_Class) == Z.numBytesInUse());
            pC2 = AllocUtil::allocateObject<my_Class>(&z, 2);
            ASSERT(3 * sizeof(my_Class) == Z.numBytesInUse());

            bslma::DeallocateObjectProctor<bslma::Allocator*, my_Class>
                proctor(&z, pC1);
            ASSERT(proctor.ptr()        == pC1);
            ASSERT(3 * sizeof(my_Class) == Z.numBytesInUse());

            proctor.reset(pC2, 2);
            ASSERT(proctor.ptr()        == pC2);
            ASSERT(3 * sizeof(my_Class) == Z.numBytesInUse());
        }
        ASSERT(sizeof(my_Class) == Z.numBytesInUse());
        ASSERT(0                == Z.numMismatches());

        AllocUtil::deallocateObject(&z, pC1);
        ASSERT(0 == Z.numBytesInUse());
        ASSERT(0 == Z.numMismatches());

      } break;
      case 4: {
        // --------------------------------------------------------------------
        // `release` TEST
        //
        // Concerns:
        // 1. Calling `release` on a proctor returns a pointer to the managed
        //    object.
        // 2. After calling `release`, `ptr()` returns a null pointer.
        //    `release` is idempotent; calling `release` on a proctor that has
        //    already been released returns a null pointer.
        // 3. Calling `release` on a proctor does not deallocate the managed
        //    object.
        // 4. After calling `release` on a proctor, its destructor becomes a
        //    no-op - no objects are deallocated.
        //
        // Plan:
        // 1. Create `my_Class` objects using `bslma::TestAllocator`.  Next,
        //    initialize a `bslma::DeallocateObjectProctor` object with the
        //    corresponding `my_Class` object and `bslma::TestAllocator`.
        // 2. Call `release` on the proctor before it goes out of scope.
        //    Verify that the return value is the pointer that was used on
        //    construction.  Verify that `ptr()` returns null.  Verify that the
        //    memory allocated by the test allocator is not deallocated.  (C-1,
        //    C-2)
        // 3. Call `release` on the proctor again.  Verify that the return
        //    value is null.  (C-3)
        // 4. Allow the proctor to go out of scope.  Verify that the memory
        //    allocated by the test allocator is not deallocated.  (C-4)
        //
        // Testing:
        //   TYPE *release();
        // --------------------------------------------------------------------

        if (verbose) printf("\n'release' TEST"
                            "\n==============\n");

        bslma::TestAllocator z(veryVeryVeryVerbose);
        const bslma::TestAllocator& Z = z;

        if (veryVerbose) std::printf("Test with `bslma::Allocator *`\n");
        {
            my_Class *pC;
            {
                pC = AllocUtil::allocateObject<my_Class>(&z);
                ASSERT(sizeof(my_Class) == Z.numBytesInUse());

                bslma::DeallocateObjectProctor<bslma::Allocator*, my_Class>
                    proctor(&z, pC);
                ASSERT(pC               == proctor.ptr());
                ASSERT(sizeof(my_Class) == Z.numBytesInUse());

                my_Class *ret = proctor.release();
                ASSERT(pC               == ret);
                ASSERT(0                == proctor.ptr());
                ASSERT(sizeof(my_Class) == Z.numBytesInUse());

                ret = proctor.release();  // idempotent
                ASSERT(0                == ret);
                ASSERT(0                == proctor.ptr());
                ASSERT(sizeof(my_Class) == Z.numBytesInUse());
            }
            ASSERT(sizeof(my_Class) == Z.numBytesInUse());

            AllocUtil::deallocateObject(&z, pC);
            ASSERT(0 == Z.numBytesInUse());
        }

        if (veryVerbose) std::printf("Test with `FancyAllocator`\n");
        {
            FancyAllocator<int>    alloc(&z);
            FancyPointer<my_Class> pC;
            {
                pC = AllocUtil::allocateObject<my_Class>(alloc);
                ASSERT(sizeof(my_Class) == Z.numBytesInUse());

                bslma::DeallocateObjectProctor<FancyAllocator<my_Class> >
                    proctor(alloc, pC);
                ASSERT(pC               == proctor.ptr());
                ASSERT(sizeof(my_Class) == Z.numBytesInUse());

                FancyPointer<my_Class> ret = proctor.release();
                ASSERT(pC               == ret);
                ASSERT(bsl::nullptr_t() == proctor.ptr());
                ASSERT(sizeof(my_Class) == Z.numBytesInUse());

                ret = proctor.release();  // idempotent
                ASSERT(! ret);
                ASSERT(! proctor.ptr());
                ASSERT(sizeof(my_Class) == Z.numBytesInUse());
            }
            ASSERT(sizeof(my_Class) == Z.numBytesInUse());

            AllocUtil::deallocateObject(alloc, pC);
            ASSERT(0 == Z.numBytesInUse());
        }

      } break;
      case 3: {
        // --------------------------------------------------------------------
        // CONSTRUCTOR / DESTRUCTOR TEST
        //
        // Concerns:
        // 1. The proctor constructor takes, as arguments, an allocator or
        //    pool, a pointer to an object, and an optional number of objects.
        //    The `ptr()` accessor returns the object pointer passed to the
        //    constructor.
        // 2. On destruction, the protected object is deallocated.  The number
        //    of objects returned to the allocator matches the number provided
        //    on construction (1 by default).
        // 3. The above concerns apply to a variety of allocator-like types.
        //    If the allocator, `a`, is an STL-style allocator, the destructor
        //    will call `a.deallocate(p, n)`.  If it is a pointer to a class
        //    derived from `bslma::Allocator` or a pointer to any class having
        //    a `deallocate(p)` member (e.g., a pool), then the destructor will
        //    call `a->deallocate(p)`.
        // 4. If the object pointer is null, however, the destructor has no
        //    effect.
        //
        // Plan:
        // 1. Create an `EnhancedTestAllocator` that matches the size of
        //    deallocations to the size of allocations.
        // 2. Create a `bsl::allocator` object that gets memory from an
        //    `EnhancedTestAllocator`.  Allocate and construct a `my_Class`
        //    object using that allocator.  Create a proctor using that
        //    allocator and the allocated `my_Class` object.  (C-1)
        // 3. When the proctor is destroyed, verify that it deallocates the
        //    correct number of managed `my_Class` objects, using the
        //    allocator's deallocation instrumentation.  (C-2)
        // 4. Repeat steps 1 and 2 with an `EnhancedTestAllocator` pointer for
        //    the allocator.  Repeat steps 1 and 2 with a `my_Pool` object
        //    except, since `my_Pool` doesn't have an `allocate` method, create
        //    the object in a `bsls::ObjectBuffer<my_Class>` instead.  (C-3)
        // 5. Repeat step 2 for each allocator type, passing a null pointer as
        //    the second argument to the proctor constructor.  Verify that
        //    nothing is deallocated by that proctor.  (C-4)
        //
        // Testing:
        //   DeallocateObjectProctor(const ALLOCATOR&, TYPE *, std::size_t = 1)
        //   ~DeallocateObjectProctor();
        //   TYPE *ptr() const;
        // --------------------------------------------------------------------

        if (verbose) printf("\nCONSTRUCTOR / DESTRUCTOR TEST"
                            "\n=============================\n");

        if (veryVerbose) printf("\nTesting with bsl::allocator\n");
        {
            EnhancedTestAllocator        z(veryVeryVeryVerbose);
            const EnhancedTestAllocator& Z = z;
            bsl::allocator<>             alloc(&z);

            if (veryVerbose) printf("\tWith `n` defaulted to 1\n");
            {
                my_Class *pC = AllocUtil::allocateObject<my_Class>(alloc);
                ASSERT(sizeof(my_Class) == Z.numBytesInUse());

                // Form 1: explicitly specify `TYPE`, which might be different
                // from `ALLOCATOR::value_type`.
                bslma::DeallocateObjectProctor<bsl::allocator<>, my_Class>
                                                            proctor(alloc, pC);
                const
                    bslma::DeallocateObjectProctor<bsl::allocator<>, my_Class>&
                                                             PROCTOR = proctor;
                ASSERT(PROCTOR.ptr()    == pC);
                ASSERT(sizeof(my_Class) == Z.numBytesInUse());
            }
            ASSERT(0 == Z.numBytesInUse());

            if (veryVerbose) printf("\tWith `n` explicitly specified\n");
            for (std::size_t i = 0; i < k_NUM_SIZES; ++i) {
                const std::size_t n = k_SIZES[i];
                my_Class *pC = AllocUtil::allocateObject<my_Class>(alloc, n);
                ASSERT(sizeof(my_Class) * n == (unsigned) Z.numBytesInUse());

                // Form 2: deduce `TYPE` from `ALLOCATOR::value_type`.
                bslma::DeallocateObjectProctor<bsl::allocator<my_Class> >
                                                         proctor(alloc, pC, n);
                const
                    bslma::DeallocateObjectProctor<bsl::allocator<my_Class> >&
                                                             PROCTOR = proctor;
                ASSERT(PROCTOR.ptr()        == pC);
                ASSERT(sizeof(my_Class) * n == (unsigned) Z.numBytesInUse());
            }
            ASSERT(0 == Z.numBytesInUse());

            if (veryVerbose) printf("\tWith null `p`\n");
            {
                bslma::DeallocateObjectProctor<bsl::allocator<my_Class> >
                                                                proctor(&z, 0);
                const
                    bslma::DeallocateObjectProctor<bsl::allocator<my_Class> >&
                                                             PROCTOR = proctor;
                ASSERT(0 == PROCTOR.ptr());
                ASSERT(0 == Z.numBytesInUse());
            }
            ASSERT(0 == Z.numBytesInUse());
        }

        if (veryVerbose) printf("\nTesting with EnhancedTestAllocator *\n");
        {
            EnhancedTestAllocator        z(veryVeryVeryVerbose);
            const EnhancedTestAllocator& Z       = z;

            if (veryVerbose) printf("\tWith `n` defaulted to 1\n");
            {
                my_Class *pC = AllocUtil::allocateObject<my_Class>(&z);
                ASSERT(sizeof(my_Class) == (unsigned) Z.numBytesInUse());

                bslma::DeallocateObjectProctor<bslma::Allocator*, my_Class>
                                                               proctor(&z, pC);
                const bslma::DeallocateObjectProctor<bslma::Allocator*,
                                                     my_Class>&
                                                             PROCTOR = proctor;
                ASSERT(PROCTOR.ptr()    == pC);
                ASSERT(sizeof(my_Class) == (unsigned) Z.numBytesInUse());
            }
            ASSERT(0 == Z.numBytesInUse());

            if (veryVerbose) printf("\tWith `n` explicitly specified\n");
            for (std::size_t i = 0; i < k_NUM_SIZES; ++i) {
                const std::size_t n = k_SIZES[i];
                my_Class *pC = AllocUtil::allocateObject<my_Class>(&z, n);
                ASSERT(sizeof(my_Class) * n == (unsigned) Z.numBytesInUse());

                bslma::DeallocateObjectProctor<bslma::Allocator*, my_Class>
                                                               proctor(&z, pC);
                const bslma::DeallocateObjectProctor<bslma::Allocator*,
                                                     my_Class>&
                                                             PROCTOR = proctor;
                ASSERT(PROCTOR.ptr()        == pC);
                ASSERT(sizeof(my_Class) * n == (unsigned) Z.numBytesInUse());
            }
            ASSERT(0 == Z.numBytesInUse());

            if (veryVerbose) printf("\tWith null `p`\n");
            {
                bslma::DeallocateObjectProctor<bslma::Allocator*, my_Class>
                                                                proctor(&z, 0);
                const bslma::DeallocateObjectProctor<bslma::Allocator*,
                                                     my_Class>&
                                                             PROCTOR = proctor;
                ASSERT(0 == PROCTOR.ptr());
                ASSERT(0 == Z.numBytesInUse());
            }
            ASSERT(0 == Z.numBytesInUse());
       }

        if (veryVerbose) printf("\nTesting with FancyAllocator\n");
        {
            EnhancedTestAllocator        z(veryVeryVeryVerbose);
            const EnhancedTestAllocator& Z = z;
            FancyAllocator<char>         alloc(&z);

            if (veryVerbose) printf("\tWith `n` defaulted to 1\n");
            {
                FancyPointer<my_Class> pC =
                    AllocUtil::allocateObject<my_Class>(alloc);
                ASSERT(sizeof(my_Class) == Z.numBytesInUse());

                // Form 1: explicitly specify `TYPE`, which might be different
                // from `ALLOCATOR::value_type`.
                bslma::DeallocateObjectProctor<FancyAllocator<char>, my_Class>
                                                            proctor(alloc, pC);
                const bslma::DeallocateObjectProctor<FancyAllocator<char>,
                                                     my_Class>&
                                                             PROCTOR = proctor;
                ASSERT(PROCTOR.ptr()    == pC);
                ASSERT(sizeof(my_Class) == Z.numBytesInUse());
            }
            ASSERT(0 == Z.numBytesInUse());

            if (veryVerbose) printf("\tWith `n` explicitly specified\n");
            for (std::size_t i = 0; i < k_NUM_SIZES; ++i) {
                const std::size_t n = k_SIZES[i];
                FancyPointer<my_Class> pC =
                    AllocUtil::allocateObject<my_Class>(alloc, n);
                ASSERT(sizeof(my_Class) * n == (unsigned) Z.numBytesInUse());

                // Form 2: deduce `TYPE` from `ALLOCATOR::value_type`.
                bslma::DeallocateObjectProctor<FancyAllocator<my_Class> >
                                                         proctor(alloc, pC, n);
                const
                    bslma::DeallocateObjectProctor<FancyAllocator<my_Class> >&
                                                             PROCTOR = proctor;
                ASSERT(PROCTOR.ptr()        == pC);
                ASSERT(sizeof(my_Class) * n == (unsigned) Z.numBytesInUse());
            }
            ASSERT(0 == Z.numBytesInUse());

            if (veryVerbose) printf("\tWith null `p`\n");
            {
                bslma::DeallocateObjectProctor<FancyAllocator<my_Class> >
                                                 proctor(&z, bsl::nullptr_t());
                const
                    bslma::DeallocateObjectProctor<FancyAllocator<my_Class> >&
                                                             PROCTOR = proctor;
                ASSERT(bsl::nullptr_t() == PROCTOR.ptr());
                ASSERT(0 == Z.numBytesInUse());
            }
            ASSERT(0 == Z.numBytesInUse());
        }

        if (veryVerbose) printf("\nTesting with my_Pool\n");
        {
            int     deallocCounter = 0;
            my_Pool pool(&deallocCounter);

            if (veryVerbose) printf("\tWith `n` defaulted to 1\n");
            {
                bsls::ObjectBuffer<my_Class> buf;
                my_Class *pC = &buf.object();
                ASSERT(0 == deallocCounter);

                bslma::DeallocateObjectProctor<my_Pool*, my_Class>
                                                            proctor(&pool, pC);
                const bslma::DeallocateObjectProctor<my_Pool*, my_Class>&
                                                             PROCTOR = proctor;
                ASSERT(PROCTOR.ptr() == pC);
                ASSERT(0             == deallocCounter);
            }
            ASSERT(1 == deallocCounter);

            if (veryVerbose) printf("\tWith null `p`\n");
            {
                bslma::DeallocateObjectProctor<my_Pool*, my_Class>
                                                             proctor(&pool, 0);
                const bslma::DeallocateObjectProctor<my_Pool*, my_Class>&
                                                             PROCTOR = proctor;
                ASSERT(0 == PROCTOR.ptr());
                ASSERT(1 == deallocCounter);
            }
            ASSERT(1 == deallocCounter);
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // HELPER CLASS TEST
        //
        // Concerns:
        // 1. The helper class `my_Class` properly increments its counter upon
        //    destruction.
        // 2. The helper class `my_Pool` properly increments its counter when
        //    its `deallocate` method is called.
        // 3. The `EnhancedTestAllocator` correctly tracks allocations and
        //    dealllocations of different sizes and capacities.
        //
        // Plan:
        // 1. Create a `my_Class` object, passing to the constructor the
        //    address of a counter.  Verify that the counter is incremented on
        //    destruction.  Repeat several times.  (C-1)
        // 2. Create a `my_Pool` object, passing to the constructor the address
        //    of a counter.  Invoke the `deallocate` method several times and
        //    verify that the counter is incremented each time.  (C-2)
        // 3. Create an `EnahcnedTestAllocator` and use its address to
        //    initialize a `bsl::allocator<int>`.  Verify that matching
        //    allocations and deallocations cancel out.  (C-3)
        //
        // Testing:
        //   Helper Class: `my_Class`
        //   Helper Class: `my_Pool`
        //   Helper Class: `EnhancedTestAllocator`
        // --------------------------------------------------------------------

        if (verbose) printf("\nHELPER CLASS TEST"
                            "\n=================\n");

        if (veryVerbose) printf("\nTesting `my_Class`.\n");

        if (veryVerbose) printf("\tTesting default ctor and dtor.\n");
        {
            const int NUM_TEST = 5;
            for (int i = 0; i < NUM_TEST; ++i) {
                my_Class mx(i);
                ASSERT(i == mx[0]);
                ASSERT(0 == mx[1]);
            }
        }

        if (veryVerbose) printf("\nTesting `my_Pool`.\n");

        if (veryVerbose) printf("\tTesting default ctor and `deallocate`.\n");

        {
            int counter = 0;
            const int NUM_TEST = 5;
            my_Pool mx(&counter);

            for (int i = 0; i < NUM_TEST; ++i) {
                ASSERTV(i, i == counter);
                mx.deallocate(0);
                ASSERTV(i, i + 1 == counter);
            }
            ASSERT(NUM_TEST == counter);
        }

        if (veryVerbose) printf("\nTesting `EnhancedTestAllocator`.\n");
        {
            EnhancedTestAllocator    eta;
            bsl::allocator<my_Class> alloc(&eta);

            my_Class *p1 = alloc.allocate(1);
            ASSERT(sizeof(my_Class) == eta.numBytesInUse());
            ASSERT(1                == eta.numBlocksInUse());

            my_Class *p2 = alloc.allocate(2);
            ASSERT(3 * sizeof(my_Class) == eta.numBytesInUse());
            ASSERT(2                    == eta.numBlocksInUse());

            alloc.deallocate(p1, 1);
            ASSERT(2 * sizeof(my_Class) == eta.numBytesInUse());
            ASSERT(1                    == eta.numBlocksInUse());

            alloc.deallocate(p2, 2);
            ASSERT(0 == eta.numBytesInUse());
            ASSERT(0 == eta.numBlocksInUse());
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //   This case exercises (but does not fully test) basic functionality.
        //
        // Concerns:
        // 1. The class is sufficiently functional to enable comprehensive
        //    testing in subsequent test cases.
        //
        // Plan:
        // 1. Execute each method to verify functionality for simple cases.
        //
        // Testing:
        //      BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) printf("\nBREATHING TEST"
                            "\n==============\n");

        bslma::TestAllocator        z(veryVeryVeryVerbose);
        const bslma::TestAllocator& Z       = z;

        if (veryVerbose) printf("\tTesting with `my_Class` object\n");
        {
            my_Class *p = AllocUtil::allocateObject<my_Class>(&z);
            ASSERT(sizeof(my_Class) == Z.numBytesInUse());

            bslma::DeallocateObjectProctor<bslma::Allocator*, my_Class>
                proctor(&z, p);
            ASSERT(proctor.ptr() == p);
        }
        ASSERT(0 == Z.numBytesInUse());

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
// Copyright 2023 Bloomberg Finance L.P.
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
