// bslma_deallocatebytesproctor.t.cpp                                 -*-C++-*-

#include <bslma_deallocatebytesproctor.h>

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

#include <cstdio>      // 'printf'
#include <cstdlib>     // 'atoi'

using namespace BloombergLP;

//=============================================================================
//                                 TEST PLAN
//-----------------------------------------------------------------------------
//                                  Overview
//                                  --------
// We are testing a proctor object to ensure that it dealloctes memory for the
// managed objects if 'release' is not called before the proctor object goes
// out of scope.  We achieve this goal by using a test allocator that tracks
// allocations and deallocations.  After the proctor is destroyed, we verify
// that any memory blocks allocated for the object are deallocated.  The
// proctor is instantiated with 'bsl::allocator', 'bslma::TestAllocator *', and
// a custom pool class pointer to test that it correctly calls the correct
// interface for the memory supplier.
//-----------------------------------------------------------------------------
// [3] DeallocateBytesProctor(const ALLOCATOR& a, size_t n, size_t align)
// [6] DeallocateBytesProctor(DeallocateBytesProctor&& original);
// [3] ~DeallocateBytesProctor();
// [3] void *ptr() const;
// [4] void *release();
// [5] void reset(void *ptr, std::size_t n, std::size_t alignment);
//-----------------------------------------------------------------------------
// [1] BREATHING TEST
// [2] Helper Class: 'my_Pool'
// [2] Helper Class: 'EnhancedTestAllocator'
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

// Test allocations with these values of 'n':
const std::size_t k_SIZES[]   = { 1, 2, 3, 4, 8, 12, 32, 63, 64 };
const std::size_t k_NUM_SIZES = sizeof(k_SIZES) / sizeof(k_SIZES[0]);

//=============================================================================
//                          HELPER CLASS FOR TESTING
//-----------------------------------------------------------------------------

class EnhancedTestAllocator : public bslma::TestAllocator {
    // A version of 'bslma::TestAllocator' that records the size and alignment
    // of the last four allocations and matches them against their
    // correpsonding deallocations.

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

template <class TYPE>
class FancyPointer;

template <>
class FancyPointer<void>
{
    // Void pointer specialization

    // DATA
    void *m_pointer;

  public:
    // CREATORS
    explicit FancyPointer(void *p = 0) : m_pointer(p) { }
    FancyPointer(bsl::nullptr_t) : m_pointer(0) { }                    // IMPLICIT
    template <class T2>
    FancyPointer(const FancyPointer<T2>& p) : m_pointer(p.get()) { }   // IMPLICIT

    // ACCESSORS
    void *get()      const { return m_pointer; }
    operator bool()  const { return 0 != m_pointer; }
    bool operator!() const { return 0 == m_pointer; }
};

template <class TYPE>
class FancyPointer
{
    // A pointer-like class for testing allocators with non-raw pointers.

    // DATA
    TYPE *m_pointer;

  public:
    // CREATORS
    explicit FancyPointer(TYPE *p = 0) : m_pointer(p) { }
    FancyPointer(bsl::nullptr_t) : m_pointer(0) { }                    // IMPLICIT
    explicit FancyPointer(const FancyPointer<void>& vp)
        : m_pointer(static_cast<TYPE *>(vp.get())) { }

    // ACCESSORS
    TYPE *get()        const { return m_pointer; }
    TYPE *operator->() const { return m_pointer; }
    TYPE& operator*()  const { return *m_pointer; }
    operator bool()  const { return 0 != m_pointer; }
    bool operator!() const { return 0 == m_pointer; }
};

template <class TYPE>
inline bool operator==(FancyPointer<TYPE> a, FancyPointer<TYPE> b)
{
    return a.get() == b.get();
}

template <class TYPE>
inline bool operator!=(FancyPointer<TYPE> a, FancyPointer<TYPE> b)
{
    return a.get() != b.get();
}

template <class TYPE>
class FancyAllocator
{
    // Allocator with non-raw pointer type

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

class my_Pool {
    // This class provides a 'deallocate' method, used to exercise the contract
    // promised by the destructor of the 'bslma::DeallocateBytesProctor'.
    // This object indicates that its 'deallocate' method is called by
    // incrementing the global counter (supplied at construction) that it
    // *holds*.

    // DATA
    int *d_counter_p;  // (non-owned) counter incremented on 'deallocate'

  public:
    // CREATORS
    explicit my_Pool(int *counter) : d_counter_p(counter) {}
        // Create this object holding the specified (global) counter.

    // MANIPULATORS
    void deallocate(void *) { ++*d_counter_p; }
        // Increment this object's counter.
};

//=============================================================================
//                                USAGE EXAMPLES
//-----------------------------------------------------------------------------

#if defined(BDE_BUILD_TARGET_EXC)

//Example 1: Class having an owning pointer
//- - - - - - - - - - - - - - - - - - - - -
// In this example, we create a class, 'my_Manager', having an owning pointer
// to an object of another class, 'my_Data'.  Because it owns the 'my_Data'
// object, 'my_Manager' is responsible for allocating, constructing,
// deallocating, and destroying it.
//
// First, we define the 'my_Data' class, which holds an integer value and
// counts how many times its constructor and destructor have been called. Its
// constructor will throw an exception if its integer argument equals the
// number of constructor calls before construction:
//..
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
//..
// Next, we define 'my_Manager' as an allocator-aware class holding a pointer
// to 'my_Data' and maintaining its own count of constructor invocations:
//..
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
//..
// Next, we define the constructor for 'my_Manager', which begins by allocating
// a 'my_Data' object:
//..
    my_Manager::my_Manager(int v, const allocator_type& allocator)
        : d_allocator(allocator), d_data_p(0)
    {
        d_data_p = static_cast<my_Data*>(
            bslma::AllocatorUtil::allocateBytes(allocator, sizeof(my_Data)));
//..
// Then, the 'my_Manager' constructor constructs the 'my_Data' object in the
// allocated memory.  However, as the constructor might throw, it first
// protects the data object with a 'bslma::DeallocateBytesProctor':
//..
        bslma::DeallocateBytesProctor<allocator_type>
                               proctor(d_allocator, d_data_p, sizeof(my_Data));
        bslma::ConstructionUtil::construct(d_data_p, d_allocator, v);
//..
// Then, once the 'construct' operation completes successfully, we can release
// the data object from the proctor.  Only then do we increment the
// construction count, as the constructor is now complete:
//..
        proctor.release();
        ++s_numConstructed;
    }
//..
// Next, we define the 'my_Manager' destructor, which destroys and deallocates
// its data object.  Note that the arguments to 'deallocateBytes' is identical
// to the constructor arguments to the 'DeallocateBytesProctor', above:
//..
    my_Manager::~my_Manager()
    {
        d_data_p->~my_Data();
        bslma::AllocatorUtil::deallocateBytes(d_allocator, d_data_p,
                                              sizeof(my_Data));
    }
//..
// Now, we use a 'bslma::TestAllocator' to verify that, under normal (non
// exceptional) circumstances, constructing a 'my_Manager' object will result
// in one block of memory being allocated and one invocation of the 'my_Data'
// constructor:
//..
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
//..
// Finally, when the 'my_Data' constructor does throw, a block is allocated but
// we verify that the 'my_Manager' constructor did not complete and that the
// block is automatically deallocated, resulting in no leaks:
//..
        try {
            my_Manager obj2(1, &ta);  // Will throw an exception
            ASSERT(false && "Can't get here");
        }
        catch (int e) {
            ASSERT(1 == e);
            ASSERT(2 == ta.numBlocksTotal());  // A 2nd block was allocated...
            ASSERT(0 == ta.numBlocksInUse());  // ...but was then deallocated
            ASSERT(1 == my_Manager::numConstructed());
        }
        ASSERT(1 == my_Manager::numConstructed());
    }
//..
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
        //: 1 That the usage examples shown in the component-level
        //:   documentation compile and run as described.
        //
        // Plan:
        //: 1 Copy the usage examples from the component header, changing
        //    'assert' to 'ASSERT' and execute them.
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
        //: 1 Move-constructing one 'bslma::DeallocateBytesProctor' from
        //:   another results in the new proctor engaging the same object with
        //:   the same allocator as the original.
        //: 2 After the move, the original object is disengaged, but retains
        //:   its allocator.
        //: 3 Move-constructing a disengaged proctor yields a second disengaged
        //:   proctor using the same allocator.
        //: 4 The above concerns apply for all allocator/pool categories
        //:   supported by this component.
        //
        // Plan:
        //: 1 Construct a 'bslma::DeallocateBytesProctor' managing a class
        //:   object.  Copy-construct a second
        //:   'bslma::DeallocateBytesProctor'.  Verify that 'ptr()' returns
        //:   the original class object and that no deallocations occured.
        //:   Verify that when the second proctor goes out of scope, the
        //:   original object is deallocated.  (C-1)
        //: 2 Verify that the original (first) proctor from step 1 is
        //:   disengaged ('ptr()' returns null).  Reset it to manage a new
        //:   object.  When the first proctor goes out of scope, verify that it
        //:   deallocates the new object.  (C-2)
        //: 3 Create a third, disengaged 'bslma::DeallocateBytesProctor', then
        //:   move-construct it, creating a fourth proctor.  Verify that the
        //:   third and fourth proctors are both disengaged.  Reset the fourth
        //:   proctor to manage a new object.  When the fourth proctor goes out
        //:   of scope, verify that it deallocates the new object.  (C-3)
        //: 4 Repeat the preceding steps using 'bsl::allocator',
        //:   'bslma::Allocator *', and 'my_Pool *' for the allocator type.
        //
        // Testing:
        //     DeallocateBytesProctor(DeallocateBytesProctor&& original);
        // --------------------------------------------------------------------

        if (verbose) printf("\nMOVE CONSTRUCTOR TEST"
                            "\n=====================\n");

        bslma::TestAllocator ta;

        if (veryVerbose) printf("Test with bsl::allocator\n");
        {
            typedef bsl::allocator<>                     Alloc;
            typedef bslma::DeallocateBytesProctor<Alloc> Obj;

            Alloc alloc(&ta);
            void *p1 = AllocUtil::allocateBytes(alloc, 1);
            void *p2 = 0, *p3 = 0;
            Obj proctorA(alloc, p1, 1);  // Engaged
            Obj proctorC(alloc, 0,  0);  // Disenaged
            {
                // Move engaged proctor
                Obj proctorB(MoveUtil::move(proctorA));
                ASSERT(p1 == proctorB.ptr());
                ASSERT(0  == proctorA.ptr());

                p2 = AllocUtil::allocateBytes(alloc, 2, 2);
                proctorA.reset(p2, 2, 2);  // Re-engage with same allocator
                ASSERT(3 == ta.numBytesInUse());

                // Move disengaged proctor
                Obj proctorD(MoveUtil::move(proctorC));
                ASSERT(0 == proctorC.ptr());
                ASSERT(0 == proctorD.ptr());

                p3 = AllocUtil::allocateBytes(alloc, 4, 4);
                proctorD.reset(p3, 4, 4);  // Engage with same allocator
                ASSERT(7 == ta.numBytesInUse());
            }
            ASSERT(p2 == proctorA.ptr());
            ASSERT(2  == ta.numBytesInUse());
        }
        ASSERT(0 == ta.numBytesInUse());

        if (veryVerbose) printf("Test with bslma::TestAllocator*\n");
        {
            typedef bslma::TestAllocator                        *Alloc;
            typedef bslma::DeallocateBytesProctor<Alloc>  Obj;

            Alloc alloc(&ta);
            void *p1 = AllocUtil::allocateBytes(alloc, 1);
            void *p2 = 0, *p3 = 0;
            Obj proctorA(alloc, p1, 1);  // Engaged
            Obj proctorC(alloc, 0,  0);  // Disenaged
            {
                // Move engaged proctor
                Obj proctorB(MoveUtil::move(proctorA));
                ASSERT(p1 == proctorB.ptr());
                ASSERT(0  == proctorA.ptr());

                p2 = AllocUtil::allocateBytes(alloc, 2, 2);
                proctorA.reset(p2, 2, 2);  // Re-engage with same allocator
                ASSERT(3 == ta.numBytesInUse());

                // Move disengaged proctor
                Obj proctorD(MoveUtil::move(proctorC));
                ASSERT(0 == proctorC.ptr());
                ASSERT(0 == proctorD.ptr());

                p3 = AllocUtil::allocateBytes(alloc, 4);
                proctorD.reset(p3, 4);  // Engage with same allocator
                ASSERT(7 == ta.numBytesInUse());
            }
            ASSERT(p2 == proctorA.ptr());
            ASSERT(2  == ta.numBytesInUse());
        }
        ASSERT(0 == ta.numBytesInUse());

        if (veryVerbose) printf("Test with my_Pool\n");
        int deallocCounter = 0;
        char ob1, ob2, ob3;
        {
            typedef my_Pool                                         *Alloc;
            typedef bslma::DeallocateBytesProctor<Alloc>  Obj;

            my_Pool pool(&deallocCounter);
            Alloc  alloc(&pool);

            void *p1 = &ob1;
            void *p2 = 0, *p3 = 0;
            Obj proctorA(alloc, p1, 1);  // Engaged
            Obj proctorC(alloc, 0,  0);  // Disenaged
            {
                // Move engaged proctor
                Obj proctorB(MoveUtil::move(proctorA));
                ASSERT(p1 == proctorB.ptr());
                ASSERT(0  == proctorA.ptr());
                ASSERT(0  == deallocCounter);

                p2 = &ob2;
                proctorA.reset(p2, 1);  // Re-engage with same allocator
                ASSERT(0  == deallocCounter);

                // Move disengaged proctor
                Obj proctorD(MoveUtil::move(proctorC));
                ASSERT(0 == proctorC.ptr());
                ASSERT(0 == proctorD.ptr());
                ASSERT(0  == deallocCounter);

                p3 = &ob3;
                proctorD.reset(p3, 1);  // Engage with same allocator
                ASSERT(0  == deallocCounter);
            }
            ASSERT(p2 == proctorA.ptr());
            ASSERT(2  == deallocCounter);
        }
        ASSERT(3 == deallocCounter);

      } break;
      case 5: {
        // --------------------------------------------------------------------
        // 'reset' TEST
        //
        // Concerns:
        //: 1 When the 'reset' method is called, the proctor object properly
        //:   manages a different block of memory.
        //: 2 The 'reset' method does not deallocate the previously managed
        //:   block of memory.
        //
        // Plan:
        //: 1 Allocate two blocks of memory from a test allocator, the
        //:   first being 1-byte in size and the second being 2-bytes in size.
        //:   Initialize a 'bslma::DeallocateBytesProctor' object with the
        //:   first block.
        //: 2 Call 'reset' on the proctor before it goes out of scope, passing
        //:   it the second block of memory.  Verify that the 'ptr' method
        //:   returns the second pointer and that no memory was deallocated.
        //:   (C-1, C-2)
        //: 3 Once the proctor goes out of scope, verify that two bytes were
        //:   deallocated.  Deallocate the first block and verify that the test
        //:   allocator did not complain of a double deletion, thereby
        //:   verifying that the proctor deallocated the second block, as
        //:   expected, and did not touch the first block.  (C-1)
        //
        // Testing:
        //   void reset(void *ptr, std::size_t n, std::size_t alignment);
        // --------------------------------------------------------------------

        if (verbose) printf("\n'reset' TEST"
                            "\n============\n");

        EnhancedTestAllocator        z(veryVeryVeryVerbose);
        const EnhancedTestAllocator& Z = z;

        void *pC1, *pC2;
        {
            pC1 = AllocUtil::allocateBytes(&z, 1);
            ASSERT(1 == Z.numBytesInUse());
            pC2 = AllocUtil::allocateBytes(&z, 2, 2);
            ASSERT(3 == Z.numBytesInUse());

            bslma::DeallocateBytesProctor<bslma::Allocator*>
                                                           proctor(&z, pC1, 1);
            ASSERT(proctor.ptr()        == pC1);
            ASSERT(3 == Z.numBytesInUse());

            proctor.reset(pC2, 2, 2);
            ASSERT(proctor.ptr()        == pC2);
            ASSERT(3 == Z.numBytesInUse());
        }
        ASSERT(1 == Z.numBytesInUse());
        ASSERT(0 == Z.numMismatches());

        AllocUtil::deallocateBytes(&z, pC1, 1);
        ASSERT(0 == Z.numBytesInUse());
        ASSERT(0 == Z.numMismatches());

      } break;
      case 4: {
        // --------------------------------------------------------------------
        // 'release' TEST
        //
        // Concerns:
        //: 1 Calling 'release' on a proctor returns a pointer to the managed
        //:   object.
        //: 2 After calling 'release', 'ptr()' returns a null pointer.
        //:   'release' is idempotent; calling 'release' on a proctor that has
        //:   already been released returns a null pointer.
        //: 3 Calling 'release' on a proctor does not deallocate the managed
        //:   object.
        //: 4 After calling 'release' on a proctor, its destructor becomes a
        //:   no-op - no objects are deallocated.
        //
        // Plan:
        //: 1 Allocate a block of memory using 'bslma::TestAllocator'.  Next,
        //:   initialize a 'bslma::DeallocateBytesProctor' object with the test
        //:   allocator and corresponding block pointer.
        //: 2 Call 'release' on the proctor before it goes out of scope.
        //:   Verify that the return value is the pointer that was used on
        //:   construction.  Verify that 'ptr()' returns null.  Verify that the
        //:   memory allocated from the test allocator was not deallocated.
        //:   (C-1, C-2)
        //: 3 Call 'release' on the proctor again.  Verify that the return
        //:   value is null.  (C-3)
        //: 4 Allow the proctor to go out of scope.  Verify that the memory
        //:   allocated by the test allocator is not deallocated.  (C-4)
        //
        // Testing:
        //   void *release();
        // --------------------------------------------------------------------

        if (verbose) printf("\n'release' TEST"
                            "\n==============\n");

        bslma::TestAllocator        z(veryVeryVeryVerbose);
        const bslma::TestAllocator& Z = z;

        if (veryVerbose) std::printf("Test with 'bslma::Allocator *'\n");
        {
            void *pC;
            {
                pC = AllocUtil::allocateBytes(&z, 92);
                ASSERT(92 == Z.numBytesInUse());

                bslma::DeallocateBytesProctor<bslma::Allocator*>
                    proctor(&z, pC, 92);
                ASSERT(pC == proctor.ptr());
                ASSERT(92 == Z.numBytesInUse());

                void *ret = proctor.release();
                ASSERT(pC == ret);
                ASSERT(0  == proctor.ptr());
                ASSERT(92 == Z.numBytesInUse());

                ret = proctor.release();  // idempotent
                ASSERT(0  == ret);
                ASSERT(0  == proctor.ptr());
                ASSERT(92 == Z.numBytesInUse());
            }
            ASSERT(92 == Z.numBytesInUse());

            AllocUtil::deallocateBytes(&z, pC, 92);
            ASSERT(0 == Z.numBytesInUse());
        }

        if (veryVerbose) std::printf("Test with 'FancyAllocator'\n");
        {
            FancyAllocator<int> alloc(&z);
            FancyPointer<void>  pC;
            {
                pC = AllocUtil::allocateBytes(alloc, 92, 4);
                ASSERT(92 == Z.numBytesInUse());

                bslma::DeallocateBytesProctor<FancyAllocator<char> >
                    proctor(alloc, pC, 92);
                ASSERT(pC == proctor.ptr());
                ASSERT(92 == Z.numBytesInUse());

                FancyPointer<void> ret = proctor.release();
                ASSERT(pC == ret);
                ASSERT(! proctor.ptr());
                ASSERT(92 == Z.numBytesInUse());

                ret = proctor.release();  // idempotent
                ASSERT(! ret);
                ASSERT(! proctor.ptr());
                ASSERT(92 == Z.numBytesInUse());
            }
            ASSERT(92 == Z.numBytesInUse());

            AllocUtil::deallocateBytes(alloc, pC, 92);
            ASSERT(0 == Z.numBytesInUse());
        }

      } break;
      case 3: {
        // --------------------------------------------------------------------
        // CONSTRUCTOR / DESTRUCTOR TEST
        //
        // Concerns:
        //: 1 The proctor constructor takes, as arguments, an allocator or
        //:   pool, a pointer to an allocated block of memory, the size of the
        //:   block in bytes, and an optional alignment.  The 'ptr()' accessor
        //:   returns the memory pointer passed to the constructor.
        //: 2 On destruction, the managed object is deallocated.  The number
        //:   of bytes returned to the allocator matches the number provided
        //:   on construction and the alignment passed to the deallocate
        //:   function matches the alignment provided at construction.
        //: 3 The above concerns apply to a variety of allocator-like types.
        //:   If the allocator is an STL-style allocator, the destructor will
        //:   call 'a.deallocate(p, n)', where 'a' is the allocator rebound to
        //:   a type having the proper alignment, 'p' is a pointer-like object
        //:   appropriate to the allocator type, and 'n' is a number computed
        //:   so that the correct number of bytes are returned.  If the
        //:   allocator 'a_p' is a pointer to a class derived from
        //:   'bslma::Allocator' or a pointer to any class having a
        //:   'deallocate(p)' member (e.g., a pool), then the destructor will
        //:   call 'a_p->deallocate(p)'.
        //: 4 If the object pointer is null, however, the destructor has no
        //:   effect.
        //
        // Plan:
        //: 1 Create an 'EnhancedTestAllocator' that matches the size and
        //:   alignments of deallocations to the size and alignments of
        //:   allocations.
        //: 2 Create a 'bsl::allocator' object that gets memory from an
        //:   'EnhancedTestAllocator'.  Allocate memory using that allocator.
        //:   Create a proctor using that allocator and the allocated memory
        //:   pointer.  (C-1)
        //: 3 When the proctor is destroyed, verify that it deallocates the
        //:   correct number of bytes, using the allocator's deallocation
        //:   instrumentation.  (C-2)
        //: 4 Repeat steps 1 and 2 with an 'EnhancedTestAllocator' pointer for
        //:   the allocator.  Repeat steps 1 and 2 with a 'my_Pool' object
        //:   except, since 'my_Pool' doesn't have an 'allocate' method, use
        //:   the address of an 'int' on the stack, instead.  (C-3)
        //: 5 Repeat step 2 for each allocator type, passing a null pointer as
        //:   the second argument to the proctor constructor.  Verify that
        //:   nothing is deallocated by that proctor.  (C-4)
        //
        // Testing:
        //   DeallocateBytesProctor(const ALLOCATOR& a, size_t n, size_t align)
        //   ~DeallocateBytesProctor();
        //   void *ptr() const;
        // --------------------------------------------------------------------

        if (verbose) printf("\nCONSTRUCTOR / DESTRUCTOR TEST"
                            "\n=============================\n");

        if (veryVerbose) printf("\nTesting with bsl::allocator\n");
        {
            EnhancedTestAllocator        z(veryVeryVeryVerbose);
            const EnhancedTestAllocator& Z = z;
            bsl::allocator<>             alloc(&z);

            if (veryVerbose)
                printf("\tWith various 'n' and 'alignment' values\n");
            for (std::size_t i = 0; i < k_NUM_SIZES; ++i) {
                const std::size_t n     = k_SIZES[i];
                const std::size_t align = 1U << (i % 3);
                void *pC = AllocUtil::allocateBytes(alloc, n, align);
                ASSERT(n == (unsigned) Z.numBytesInUse());

                bslma::DeallocateBytesProctor<bsl::allocator<> >
                                                  proctor(alloc, pC, n, align);
                const
                    bslma::DeallocateBytesProctor<bsl::allocator<> >&
                                                             PROCTOR = proctor;
                ASSERT(PROCTOR.ptr() == pC);
                ASSERT(n             == (unsigned) Z.numBytesInUse());
            }
            ASSERT(0 == Z.numBytesInUse());

            if (veryVerbose) printf("\tWith null 'p'\n");
            {
                bslma::DeallocateBytesProctor<bsl::allocator<> >
                                                            proctor(&z, 0, 99);
                const bslma::DeallocateBytesProctor<bsl::allocator<> >&
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
            FancyAllocator<int>          alloc(&z);

            if (veryVerbose)
                printf("\tWith various 'n' and 'alignment' values\n");
            for (std::size_t i = 0; i < k_NUM_SIZES; ++i) {
                const std::size_t n     = k_SIZES[i];
                const std::size_t align = 1U << (i % 3);
                const std::size_t exp   = (n + align - 1) & ~(align - 1);
                FancyPointer<void> pC = AllocUtil::allocateBytes(alloc, n, align);
                ASSERTV(exp, Z.numBytesInUse(),
                        exp == (unsigned) Z.numBytesInUse());

                bslma::DeallocateBytesProctor<FancyAllocator<int> >
                                                  proctor(alloc, pC, n, align);
                const
                    bslma::DeallocateBytesProctor<FancyAllocator<int> >&
                                                             PROCTOR = proctor;
                ASSERT(PROCTOR.ptr() == pC);
                ASSERTV(exp, Z.numBytesInUse(),
                        exp == (unsigned) Z.numBytesInUse());
            }
            ASSERT(0 == Z.numBytesInUse());

            if (veryVerbose) printf("\tWith null 'p'\n");
            {
                bslma::DeallocateBytesProctor<FancyAllocator<int> >
                                                proctor(&z, bsl::nullptr_t(), 99);
                const bslma::DeallocateBytesProctor<FancyAllocator<int> >&
                                                             PROCTOR = proctor;
                ASSERT(! PROCTOR.ptr());
                ASSERT(0 == Z.numBytesInUse());
            }
            ASSERT(0 == Z.numBytesInUse());
        }

        if (veryVerbose) printf("\nTesting with EnhancedTestAllocator *\n");
        {
            EnhancedTestAllocator        z(veryVeryVeryVerbose);
            const EnhancedTestAllocator& Z       = z;

            if (veryVerbose)
                printf("\tWith various 'n' and 'alignment' values\n");
            for (std::size_t i = 0; i < k_NUM_SIZES; ++i) {
                const std::size_t n     = k_SIZES[i];
                const std::size_t align = 1U << (i % 3);
                void *pC = AllocUtil::allocateBytes(&z, n, align);
                ASSERT(n == (unsigned) Z.numBytesInUse());

                bslma::DeallocateBytesProctor<bslma::Allocator*>
                                                     proctor(&z, pC, n, align);
                const bslma::DeallocateBytesProctor<bslma::Allocator*>&
                                                             PROCTOR = proctor;
                ASSERT(PROCTOR.ptr() == pC);
                ASSERT(n             == (unsigned) Z.numBytesInUse());
            }
            ASSERT(0 == Z.numBytesInUse());

            if (veryVerbose) printf("\tWith null 'p'\n");
            {
                bslma::DeallocateBytesProctor<bslma::Allocator*>
                                                             proctor(&z, 0, 0);
                const bslma::DeallocateBytesProctor<bslma::Allocator*>&
                                                             PROCTOR = proctor;
                ASSERT(0 == PROCTOR.ptr());
                ASSERT(0 == Z.numBytesInUse());
            }
            ASSERT(0 == Z.numBytesInUse());
       }

        if (veryVerbose) printf("\nTesting with my_Pool\n");
        {
            int     deallocCounter = 0;
            my_Pool pool(&deallocCounter);

            if (veryVerbose) printf("\tWith 'n' set to 'sizeof(int)'\n");
            {
                int   buf;
                void *pC = &buf;
                ASSERT(0 == deallocCounter);

                bslma::DeallocateBytesProctor<my_Pool*> proctor(&pool, pC,
                                                                sizeof(int));
                const bslma::DeallocateBytesProctor<my_Pool*>&
                                                             PROCTOR = proctor;
                ASSERT(PROCTOR.ptr() == pC);
                ASSERT(0             == deallocCounter);
            }
            ASSERT(1 == deallocCounter);

            if (veryVerbose) printf("\tWith null 'p'\n");
            {
                bslma::DeallocateBytesProctor<my_Pool*> proctor(&pool, 0,
                                                                sizeof(int));
                const bslma::DeallocateBytesProctor<my_Pool*>&
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
        //: 1 The helper class 'my_Pool' properly increments its counter when
        //:   its 'deallocate' method is called.
        //: 2 The 'EnhancedTestAllocator' correctly tracks allocations and
        //:   dealllocations of different sizes and capacities.
        //
        // Plan:
        //: 1 Create a 'my_Pool' object, passing to the constructor the address
        //:   of a counter.  Invoke the 'deallocate' method several times and
        //:   verify that the counter is incremented each time.  (C-1)
        //: 2 Create an 'EnahcnedTestAllocator' and use its address to
        //:   initialize a 'bsl::allocator<int>'.  Verify that matching
        //:   allocations and deallocations cancel out.  (C-2)
        //
        // Testing:
        //   Helper Class: 'my_Pool'
        //   Helper Class: 'EnhancedTestAllocator'
        // --------------------------------------------------------------------

        if (verbose) printf("\nHELPER CLASS TEST"
                            "\n=================\n");

        if (veryVerbose) printf("\nTesting 'my_Pool'.\n");

        if (veryVerbose) printf("\tTesting default ctor and 'deallocate'.\n");

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

        if (veryVerbose) printf("\nTesting 'EnhancedTestAllocator'.\n");
        {
            EnhancedTestAllocator eta;
            bsl::allocator<int>   alloc(&eta);

            int *p1 = alloc.allocate(1);
            ASSERT(sizeof(int) == eta.numBytesInUse());
            ASSERT(1           == eta.numBlocksInUse());

            int *p2 = alloc.allocate(2);
            ASSERT(3 * sizeof(int) == eta.numBytesInUse());
            ASSERT(2               == eta.numBlocksInUse());

            alloc.deallocate(p1, 1);
            ASSERT(2 * sizeof(int) == eta.numBytesInUse());
            ASSERT(1               == eta.numBlocksInUse());

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
        //: 1 The class is sufficiently functional to enable comprehensive
        //:   testing in subsequent test cases.
        //
        // Plan:
        //: 1 Execute each method to verify functionality for simple cases.
        //
        // Testing:
        //      BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) printf("\nBREATHING TEST"
                            "\n==============\n");

        bslma::TestAllocator        z(veryVeryVeryVerbose);
        const bslma::TestAllocator& Z = z;

        {
            void *p = AllocUtil::allocateBytes(&z, 10);
            ASSERT(10 == Z.numBytesInUse());

            bslma::DeallocateBytesProctor<bslma::Allocator*> proctor(&z,p,10);
            ASSERT(10 == Z.numBytesInUse());
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
