// bslma_memoryresource.t.cpp                                         -*-C++-*-

#include <bslma_memoryresource.h>

#include <bslmf_movableref.h>

#include <bsls_alignmentfromtype.h>
#include <bsls_assert.h>
#include <bsls_asserttest.h>
#include <bsls_bsltestutil.h>
#include <bsls_exceptionutil.h>
#include <bsls_keyword.h>
#include <bsls_protocoltest.h>

#include <cstdio>   // 'printf'
#include <cstdlib>  // 'atoi'
#include <new>      // placement 'new'

#include <stdint.h>  // 'uintptr_t' (not in namespace 'std')

#ifdef BDE_VERIFY
// Suppress some pedantic bde_verify checks in this test driver
#pragma bde_verify -FD01   // Function declaration requires contract
#pragma bde_verify -FD03   // Parameter not documented in function contract
#pragma bde_verify -FABC01 // Function not in alphabetical order
#pragma bde_verify -TP19   // Missing or malformed standard test driver section
#endif

using std::printf;
using namespace BloombergLP;

// ============================================================================
//                             TEST PLAN
// ----------------------------------------------------------------------------
//                                   Overview
//                                   --------
// The 'bsl::memory_resource' class is a pure abstract class.  Its public
// interface comprises three non-virtual functions that simply pass through to
// corresponding private virtual functions.  The test driver employs a modified
// protocol test that accesses the virtual functions through their public
// non-virtual interfaces and a test for each public function testing that it
// correctly forwards its arguments and return value to and from the underlying
// virtual function.
// ----------------------------------------------------------------------------
// [ 2] void *allocate(size_t bytes, size_t alignment);
// [ 3] void deallocate(void *p, size_t bytes, size_t alignment);
// [ 4] bool is_equal(const memory_resource& other) const noexcept;
// ----------------------------------------------------------------------------
// [ 1] PROTOCOL TEST
// [ 5] USAGE EXAMPLES
// ----------------------------------------------------------------------------

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
//                  SEMI-STANDARD NEGATIVE-TESTING MACROS
// ----------------------------------------------------------------------------

#define ASSERT_SAFE_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(EXPR)
#define ASSERT_SAFE_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPR)
#define ASSERT_PASS(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS(EXPR)
#define ASSERT_FAIL(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL(EXPR)
#define ASSERT_OPT_PASS(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS(EXPR)
#define ASSERT_OPT_FAIL(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL(EXPR)

// ============================================================================
//                                VERBOSITY
// ----------------------------------------------------------------------------

int verbose = 0;
int veryVerbose = 0;
int veryVeryVerbose = 0;
int veryVeryVeryVerbose = 0; // For test allocators

// ============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------

class MemoryResourceProtocolTest
    : public bsls::ProtocolTestImp<bsl::memory_resource> {
    // This class is used with 'bsls::ProtocolTest' to test the
    // 'bsl::memory_resource' protocol.

  protected:
    void* do_allocate(std::size_t, std::size_t) BSLS_KEYWORD_OVERRIDE;
    void do_deallocate(void *, std::size_t, std::size_t) BSLS_KEYWORD_OVERRIDE;
    bool do_is_equal(const bsl::memory_resource&) const
                                   BSLS_KEYWORD_NOEXCEPT BSLS_KEYWORD_OVERRIDE;
};

void* MemoryResourceProtocolTest::do_allocate(std::size_t, std::size_t)
{
    static char buf[1];
    return markDoneVal(static_cast<void *>(buf));  // Return non-null ptr
}

void
MemoryResourceProtocolTest::do_deallocate(void *, std::size_t, std::size_t)
{
    markDone();
}

bool MemoryResourceProtocolTest::do_is_equal(const bsl::memory_resource&) const
                                                          BSLS_KEYWORD_NOEXCEPT
{
    return markDone();
}

class PassthroughTest : public bsl::memory_resource {
    // Class to test that pass-through functions correctly pass their arguments
    // through to the private virtual functions.

    // DATA
    int                    d_buffer[4];         // buffer to allocate from
    std::size_t            d_lastBytesArg;      // most recent 'bytes' argument
    std::size_t            d_lastAlignmentArg;  // most recent 'alignment' arg
    void                  *d_lastBlock_p;       // most recent memory block
    mutable const memory_resource *d_lastOther_p; // most recent 'other' addr

    // PRIVATE MANIPULATORS
    void* do_allocate(size_t bytes, size_t alignment) BSLS_KEYWORD_OVERRIDE;
    void do_deallocate(void* p, size_t bytes, size_t alignment)
                                                         BSLS_KEYWORD_OVERRIDE;
    bool do_is_equal(const memory_resource& other) const
                                   BSLS_KEYWORD_NOEXCEPT BSLS_KEYWORD_OVERRIDE;

  public:
    // CREATORS
    PassthroughTest()
        : d_lastBytesArg(0)
        , d_lastAlignmentArg(0)
        , d_lastBlock_p(0)
        , d_lastOther_p(0) { }

    // ACCESSORS
    std::size_t  lastBytesArg()        const { return d_lastBytesArg;     }
    std::size_t  lastAlignmentArg()    const { return d_lastAlignmentArg; }
    void        *lastBlock()           const { return d_lastBlock_p;      }
    const memory_resource& lastOther() const { return *d_lastOther_p;     }
};

void* PassthroughTest::do_allocate(size_t bytes, size_t alignment)
{
    d_lastBytesArg     = bytes;
    d_lastAlignmentArg = alignment;
    d_lastBlock_p      = &d_buffer[bytes % 4];
    return d_lastBlock_p;
}

void PassthroughTest::do_deallocate(void* p, size_t bytes, size_t alignment)
{
    d_lastBytesArg     = bytes;
    d_lastAlignmentArg = alignment;
    d_lastBlock_p      = p;
}

bool PassthroughTest::do_is_equal(const memory_resource& other) const
                                                          BSLS_KEYWORD_NOEXCEPT
{
    d_lastOther_p = &other;
    return this == &other;
}

// ============================================================================
//                             USAGE EXAMPLES
// ----------------------------------------------------------------------------

///Usage
///-----
// The 'bsl::memory_resource' protocol provided in this component defines a
// bilateral contract between suppliers and consumers of raw memory.  The
// following subsections illustrate (1) implementation of a concrete resource
// drived from the abstract 'bsl::memory_resource' base class and (2) use of a
// 'bsl::memory_resource'.
//
///Example 1: a counting memory resource
///- - - - - - - - - - - - - - - - - - -
// In this example, we derive a concrete 'CountingResource' class from
// 'bsl::memory_resource', overriding and providing concrete implementations
// for all of the virtual functions declared in the base class.  This resource
// keeps track of the number of blocks of memory that were allocated from the
// resource but not yet returned to the resource.
//
// First, we define the 'CountingResource' class with a single private data
// member to keep track of the number of blocks outstanding.  We don't want
// this type to be copyable, so we disable copying here, too.
//..
//  #include <bslmf_movableref.h>
//  #include <bsls_assert.h>
//  #include <bsls_keyword.h>
//  #include <bsls_exceptionutil.h>
//  #include <stdint.h>  // 'uintptr_t'

    class CountingResource : public bsl::memory_resource {

        // DATA
        int d_blocksOutstanding;

        CountingResource(const CountingResource&) BSLS_KEYWORD_DELETED;
        CountingResource& operator=(const CountingResource&)
                                                          BSLS_KEYWORD_DELETED;
//..
// Next, we declare the protected virtual functions that override the
// base-class virtual functions:
//..
      protected:
        // PROTECTED MANIPULATORS
        void* do_allocate(std::size_t bytes,
                          std::size_t alignment) BSLS_KEYWORD_OVERRIDE;
        void do_deallocate(void* p, std::size_t bytes,
                           std::size_t alignment) BSLS_KEYWORD_OVERRIDE;

        // PROTECTED ACCESSORS
        bool do_is_equal(const bsl::memory_resource& other) const
                                   BSLS_KEYWORD_NOEXCEPT BSLS_KEYWORD_OVERRIDE;
//..
// Now we can declare the public interface, comprising the default constructor,
// the destructor, and an accessor to return the current block count; all other
// public members are inherited from the base class:
//..
      public:
        // CREATORS
        CountingResource() : d_blocksOutstanding(0) { }
        ~CountingResource() BSLS_KEYWORD_OVERRIDE;

        // ACCESSORS
        int blocksOutstanding() const { return d_blocksOutstanding; }
    };
//..
// Next, we implement the 'do_allocate' method to allocate memory using
// 'operator new', then increment the block counter.  We cannot, in C++11,
// force 'operator new' to return memory that is more than maximally aligned,
// so we throw an exception if the specified 'alignment' is not met; other
// resources can use the 'alignment' argument more productively.
//..
    void *CountingResource::do_allocate(std::size_t bytes,
                                        std::size_t alignment)
    {
        void *ret = ::operator new(bytes);
        if (uintptr_t(ret) & (alignment - 1)) {
            ::operator delete(ret);
            BSLS_THROW(this);  // Alignment failed
        }
        ++d_blocksOutstanding;
        return ret;
    }
//..
// Next, we implement 'do_deallocate', which returns the memory referenced by
// 'p' to the heap and decrements the block counter.  The 'bytes' and
// 'alignment' arguments are ignored:
//..
    void CountingResource::do_deallocate(void* p, std::size_t, std::size_t)
    {
        ::operator delete(p);
        --d_blocksOutstanding;
    }
//..
// Next, we implement 'do_is_equal', which determines if the specified 'other'
// resource is equal to this one.  For this and most other resource types,
// 'do_is_equal' returns 'true' if and only if the two resources are the same
// object:
//..
    bool CountingResource::do_is_equal(const bsl::memory_resource& other) const
                                                          BSLS_KEYWORD_NOEXCEPT
    {
        return this == &other;
    }
//..
// Next, we implement the destructor, which simply asserts that the block count
// is zero upon destruction:
//..
    CountingResource::~CountingResource()
    {
        BSLS_ASSERT(0 == d_blocksOutstanding);
    }
//..
// Finally, we construct an object of 'CountingResource' and verify that
// allocation, deallocation, and equality testing work as expected.
//..
    void usageExample1()
    {
        CountingResource obj;
        ASSERT(0 == obj.blocksOutstanding());

        void *p = obj.allocate(16, 4);
        ASSERT(p);
        ASSERT(0 == (uintptr_t(p) & 3));
        ASSERT(1 == obj.blocksOutstanding());

        obj.deallocate(p, 16, 4);
        ASSERT(0 == obj.blocksOutstanding());

        CountingResource obj2;
        ASSERT(obj == obj);
        ASSERT(obj != obj2);
    }
//..
//
///Example 2: A class that allocates memory
///- - - - - - - - - - - - - - - - - - - -
// In this example, we define a class template, 'Holder<TYPE>', that holds a
// single instance of 'TYPE' on the heap.  'Holder' is designed such that its
// memory use can be customized by supplying an appropriate memory resource.  A
// holder object can be empty and it can be move-constructed even if 'TYPE' is
// not movable.  In addition, the footprint of a 'Holder' object is the same
// (typically the size of 2 pointers), regardless of the size of 'TYPE'.
//
// First, we define a simple class template modeled after the C++17 standard
// library 'std::pmr::polymorphic_allocator' template, which is a thin wrapper
// around a 'memory_resource' pointer.  By wrapping the pointer in a class, we
// avoid some the problems of raw pointers such as accidental use of a null
// pointer:
//..
//  #include <bsls_alignmentfromtype.h>

    template <class TYPE>
    class PolymorphicAllocator {

        // DATA
        bsl::memory_resource *d_resource_p;

      public:
        // CREATORS
        PolymorphicAllocator(bsl::memory_resource *r);              // IMPLICIT

        // MANIPULATORS
        TYPE *allocate(std::size_t n);
        void deallocate(TYPE *p, size_t n);

        // ACCESSORS
        bsl::memory_resource *resource() const { return d_resource_p; }
    };
//..
// Next, we implement the constructor for 'PolymorphicAllocator', which stores
// the pointer argument and defensively checks that it is not null:
//..
    template <class TYPE>
    PolymorphicAllocator<TYPE>::PolymorphicAllocator(bsl::memory_resource *r)
        : d_resource_p(r)
    {
        BSLS_ASSERT(0 != r);
    }
//..
// Next, we implement the allocation and deallocation functions by forwarding
// to the corresponding function of the memory resource.  Note that the size
// and alignment of 'TYPE' are used to compute the appropriate number of bytes
// and alignment to request from the memory resource:
//..
    template <class TYPE>
    TYPE *PolymorphicAllocator<TYPE>::allocate(std::size_t n)
    {
        void *p = d_resource_p->allocate(n * sizeof(TYPE),
                                         bsls::AlignmentFromType<TYPE>::VALUE);
        return static_cast<TYPE *>(p);
    }

    template <class TYPE>
    void PolymorphicAllocator<TYPE>::deallocate(TYPE *p, std::size_t n)
    {
        d_resource_p->deallocate(p, n * sizeof(TYPE),
                                 bsls::AlignmentFromType<TYPE>::VALUE);
    }
//..
// Now we define our actual 'Holder' template with with data members to hold
// the memory allocator and a pointer to the contained object:
//..
    template <class TYPE>
    class Holder {
        PolymorphicAllocator<TYPE>  d_allocator;
        TYPE                       *d_data_p;
//..
// Next, we declare the constructors.  Following the pattern for
// allocator-aware types used in BDE, the public interface contains an
// 'allocator_type' typedef that can be passed to each constructor.
// Typically, the allocator constructor argument would be optional, but,
// because our 'PolymorphicAllocator' has no default constructor (unlike the
// 'std::pmr::polymorphic_allocator'), the allocator is *required* for all
// constructors except the move constructor:
//..
      public:
        // TYPES
        typedef PolymorphicAllocator<TYPE> allocator_type;

        // CREATORS
        explicit Holder(const allocator_type& allocator);
        Holder(const TYPE& value, const allocator_type& allocator);
        Holder(const Holder& other, const allocator_type& allocator);
        Holder(bslmf::MovableRef<Holder> other);                    // IMPLICIT
        Holder(bslmf::MovableRef<Holder> other,
               const allocator_type&     allocator);
        ~Holder();
//..
// Next, we declare the manipulators and accessors, allowing a 'Holder' to be
// assigned and giving a client access to its value and allocator:
//..
        // MANIPULATORS
        Holder& operator=(const Holder& rhs);
        Holder& operator=(bslmf::MovableRef<Holder> rhs);
        TYPE& value() { return *d_data_p; }

        // ACCESSORS
        bool isEmpty() const { return 0 == d_data_p; }
        const TYPE& value() const { return *d_data_p; }
        allocator_type get_allocator() const { return d_allocator; }
    };
//..
// Next, we'll implement the first constructor, which creates an empty object;
// its only job is to store the allocator:
//..
    template <class TYPE>
    Holder<TYPE>::Holder(const allocator_type& allocator)
        : d_allocator(allocator)
        , d_data_p(0)
    {
    }
//..
// Next, we'll implement the second constructor, which allocates memory and
// constructs an object in it.  The 'try'/'catch' block is needed to free the
// memory in case the constructor for 'TYPE' throws and exception.  An
// alternative implementation would use an RAII object to automatically free
// the memory in the case of an exception (see 'bslma_deallocatorproctor'):
//..
    template <class TYPE>
    Holder<TYPE>::Holder(const TYPE& value, const allocator_type& allocator)
        : d_allocator(allocator)
        , d_data_p(0)
    {
        d_data_p = d_allocator.allocate(1);
        BSLS_TRY {
            ::new(d_data_p) TYPE(value);
        }
        BSLS_CATCH(...) {
            d_allocator.deallocate(d_data_p, 1);
            BSLS_RETHROW;
        }
    }
//..
// Next, we'll implement a destructor that deletes the value object and
// deallocates the allocated memory:
//..
    template <class TYPE>
    Holder<TYPE>::~Holder()
    {
        if (! isEmpty()) {
            d_data_p->~TYPE();                    // Destroy object.
            d_allocator.deallocate(d_data_p, 1);  // Deallocate memory.
        }
    }
//..
// Finally, we've implemented enough of 'Holder' to demonstrate its use.
// Below, we pass the 'CountingResource' from Example 1 to the constructors
// several 'Holder' objects.  Each non-empty 'Holder' allocates one block of
// memory, which is reflected in the outstanding block count.  Note that the
// address of the resource can be passed directly to the constructors because
// 'PolymorphicAllocator' is implicitly convertible from 'bsl::memory_resource
// *':
//..
    void usageExample2()
    {
        CountingResource rsrc;

        {
            Holder<int> h1(&rsrc);   // Empty resource
            ASSERT(h1.isEmpty());
            ASSERT(0 == rsrc.blocksOutstanding());

            Holder<int> h2(2, &rsrc);
            ASSERT(! h2.isEmpty());
            ASSERT(1 == rsrc.blocksOutstanding());

            Holder<double> h3(3.0, &rsrc);
            ASSERT(! h3.isEmpty());
            ASSERT(2 == rsrc.blocksOutstanding());
        }

        ASSERT(0 == rsrc.blocksOutstanding());  // Destructors freed memory
    }
//..

}  // close unnamed namespace

// ============================================================================
//                              MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? std::atoi(argv[1]) : 0;
    verbose = argc > 2;
    veryVerbose = argc > 3;
    veryVeryVerbose = argc > 4;
    veryVeryVeryVerbose = argc > 5;

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:  // Zero is always the leading case.
      case 5: {
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

        usageExample1();
        usageExample2();

      } break;

      case 4: {
        // --------------------------------------------------------------------
        // TESTING 'is_equal'
        //
        // Concerns:
        //: 1 A call to 'is_equal' passes its argument to the
        //:   'do_is_equal' virtual function and returns the result.
        //
        // Plan:
        //: 1 Using a test resource that tracks the argument values and return
        //:   values of virtual member function calls, invoke 'is_equal' and
        //:   verify that the argument to 'do_is_equal' matches the
        //:   argument passed to 'is_equal'.  Also verify that the return value
        //:   of 'is_equal' is 'true' when comparing a test resource to itself
        //:   and 'false' otherwise.  (C-1)
        //
        // Testing:
        //    bool is_equal(const memory_resource& other) const noexcept;
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING 'is_equal'"
                            "\n==================\n");

        PassthroughTest  obj, other;

        bool result = obj.is_equal(other);
        ASSERT(false == result);
        ASSERT(&obj.lastOther() == &other);

        result = obj.is_equal(obj);
        ASSERT(true == result);
        ASSERT(&obj.lastOther() == &obj);

      } break;

      case 3: {
        // --------------------------------------------------------------------
        // TESTING 'deallocate'
        //
        // Concerns:
        //: 1 A call to 'deallocate' passes its arguments to the
        //:   'do_deallocate' virtual function.
        //: 2 If the 'alignment' argument is omitted, the the maximum platform
        //:   alignment is used.
        //
        // Plan:
        //: 1 Using a test resource that tracks the argument values and return
        //:   values of virtual member function calls, invoke 'deallocate' and
        //:   verify that the three arguments to 'do_deallocate' match the
        //:   three arguments passed to 'deallocate'.  (C-1)
        //: 2 Repeat step 1 but omit the 'alignment' argument to 'deallocate'.
        //:   Verify that the alignment passed to 'do_deallocate' is the
        //:   maximum alignment for the platform.  (C-2)
        //
        // Testing:
        //    void deallocate(void *p, size_t bytes, size_t alignment);
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING 'deallocate'"
                            "\n====================\n");

        PassthroughTest  obj;
        int              block[2];
        void            *p = block;

        obj.deallocate(p, 8, 2);
        ASSERT(obj.lastBytesArg()     == 8);
        ASSERT(obj.lastAlignmentArg() == 2);
        ASSERT(obj.lastBlock()        == p);

        p = block + 1;
        obj.deallocate(p, 4);
        ASSERT(obj.lastBytesArg()     == 4);
        ASSERT(obj.lastAlignmentArg() ==
                                      bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT);
        ASSERT(obj.lastBlock()        == p);

      } break;

      case 2: {
        // --------------------------------------------------------------------
        // TESTING 'allocate'
        //
        // Concerns:
        //: 1 A call to 'allocate' passes its arguments to the 'do_allocate'
        //:   virtual function and returns its result.
        //: 2 If the 'alignment' argument is omitted, the the maximum platform
        //:   alignment is used.
        //
        // Plan:
        //: 1 Using a test resource that tracks the argument values and return
        //:   values of virtual member function calls, invoke 'allocate' and
        //:   verify that the two arguments to 'do_allocate' match the
        //:   two arguments passed to 'allocate' and that the return value of
        //:   'allocate' matches the return value from 'do_allocate'.  (C-1)
        //: 2 Repeat step 1 but omit the 'alignment' argument to 'allocate'.
        //:   Verify that the alignment passed to 'do_allocate' is the maximum
        //:   alignment for the platform.  (C-2)
        //
        // Testing:
        //    void *allocate(size_t bytes, size_t alignment);
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING 'allocate'"
                            "\n==================\n");

        PassthroughTest  obj;

        void            *p = obj.allocate(8, 2);
        ASSERT(obj.lastBytesArg()     == 8);
        ASSERT(obj.lastAlignmentArg() == 2);
        ASSERT(obj.lastBlock()        == p);

        p = obj.allocate(4);
        ASSERT(obj.lastBytesArg()     == 4);
        ASSERT(obj.lastAlignmentArg() ==
                                      bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT);
        ASSERT(obj.lastBlock()        == p);

      } break;

      case 1: {
        // --------------------------------------------------------------------
        // PROTOCOL TEST:
        //   Ensure this class is a properly defined protocol.
        //
        // Concerns:
        //: 1 The protocol is abstract: no objects of it can be created.
        //: 2 The protocol has no data members.
        //: 3 The protocol has a virtual destructor.
        //: 4 All methods of the protocol are pure virtual.
        //: 5 All methods of the protocol are available through publicly
        //:   accessible pass-through functions.
        //
        // Plan:
        //: 1 Define a concrete derived implementation of
        //:   'bsl::memory_resource', 'MemoryResourceProtocolTest'.
        //: 2 Create an object of the 'bsls::ProtocolTest' class template
        //:   parameterized by 'MemoryResourceProtocolTest', and use it to
        //:   verify that:
        //:
        //:   1 The protocol is abstract. (C-1)
        //:   2 The protocol has no data members. (C-2)
        //:   3 The protocol has a virtual destructor. (C-3)
        //:
        //: 3 Use the 'BSLS_PROTOCOLTEST_ASSERT' macro to verify that
        //:   non-creator methods of the protocol are:
        //:
        //:   1 virtual, (C-4)
        //:   2 publicly accessible through pass-through functions. (C-5)
        //
        // Testing:
        //      ~memory_resource();
        //      void* do_allocate(std::size_t, std::size_t);
        //      void do_deallocate(void *p, std::size_t, std::size_t);
        //      bool do_is_equal(const bsl::memory_resource&) const;
        // --------------------------------------------------------------------

        if (verbose) printf("\nPROTOCOL TEST"
                            "\n=============\n");

        bsls::ProtocolTest<MemoryResourceProtocolTest> testObj(veryVerbose);

        ASSERT(testObj.testAbstract());
        ASSERT(testObj.testNoDataMembers());
        ASSERT(testObj.testVirtualDestructor());

        // Create a reference to 'memory_resource' to test protocol.
        const bsl::memory_resource& other = MemoryResourceProtocolTest();
        void *p = 0;

        // Note that we are testing the public pass-through functions as a
        // proxy for the private virtual functions.
        BSLS_PROTOCOLTEST_ASSERT(testObj, allocate(2, 1));
        BSLS_PROTOCOLTEST_ASSERT(testObj, deallocate(p, 2, 1));
        BSLS_PROTOCOLTEST_ASSERT(testObj, is_equal(other));
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
// Copyright 2022 Bloomberg Finance L.P.
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
