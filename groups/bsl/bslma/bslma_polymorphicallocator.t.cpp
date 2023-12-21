// bslma_polymorphicallocator.t.cpp                                   -*-C++-*-

#include <bslma_polymorphicallocator.h>

#include <bslma_default.h>
#include <bslma_isstdallocator.h>
#include <bslma_testallocator.h>

#include <bslmf_isbitwisemoveable.h>
#include <bslmf_istriviallycopyable.h>
#include <bslmf_movableref.h>

#include <bsls_asserttest.h>
#include <bsls_bsltestutil.h>

#include <bsls_assert.h>
#include <bsls_keyword.h>
#include <bsls_objectbuffer.h>

#include <cstdio>   // 'printf'
#include <cstdlib>  // 'atoi'

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
// A 'bsl::polymorphic_allocator' is a reference-semantic type that holds a
// pointer to a 'bsl::memory_resource' object (its underlying "resource").
// This pointer can be set at construction, and it can be accessed through the
// 'resource' accessor.  It cannot be reset, however, since normally an
// allocator does not change during the lifetime of an object.  A
// 'bsl::polymorphic_allocator' is parameterized by the type it allocates, and
// that influences the behavior of several manipulators and accessors, mainly
// depending on the size of that type.  A 'bsl::polymorphic_allocator'
// instantiated on a different type can be obtained using the 'rebind' nested
// template.
//
// Our specific concerns are that an allocator constructed with a certain
// underlying resource actually uses that resource to allocate memory, and
// that its rebound versions do as well.
//
// Another concern is standards conformance.  In a C++17 or later build, the
// platform library's 'std::pmr::polymorphic_allocator' template is used, so
// conformance to the C++17 standard (C++17 section 20.5.3.5
// [allocator.requirements]) is assured.  On pre C++17 platforms, we must
// ensure that, in addition to the C++17 requirements, all the requirements of
// C++03 allocators (section 20.1.5 [lib.allocator.requirements]) are met, even
// though many of those requirements were removed or relaxed in
// C++11. Specifically, we must ensure that 'max_size()' is the maximum
// possible size for that type (i.e., it is impossible to meaningfully pass in
// a larger size), that 'size_type' is unsigned, that 'difference_type' is
// signed, etc..
//-----------------------------------------------------------------------------
// Creators
// [ 3] polymorphic_allocator();
// [ 3] polymorphic_allocator('bsl::memory_resource *);
// [ 3] polymorphic_allocator(const polymorphic_allocator&);
// [ 3] polymorphic_allocator(const polymorphic_allocator<U>&);
// [ 3] ~polymorphic_allocator();
//
// Modifiers
// [ 9] pointer allocate(size_type n, const void *hint = 0);
// [ 9] void deallocate(pointer p, size_type n = 1);
// [10] void construct(pointer p, const TYPE& val, ARGS&&... arguments);
// [10] void destroy(pointer p);
//
// Accessors
// [ 4] pointer address(reference x) const;
// [ 4] const_pointer address(const_reference x) const;
// [ 4] 'bsl::memory_resource *resource() const;
// [ 4] size_type max_size() const;
//
// Nested types
// [ 5] size_type
// [ 5] difference_type
// [ 5] pointer;
// [ 5] const_pointer;
// [ 5] reference;
// [ 5] const_reference;
// [ 5] value_type;
// [ 5] template rebind<U>::other
//
// Free functions (operators)
// [ 8] operator==(polymorphic_allocator<T>,  polymorphic_allocator<T>);
// [ 8] operator==(polymorphic_allocator<T1>, polymorphic_allocator<T2>);
// [ 8] operator==(memory_resource *,         polymorphic_allocator<T>);
// [ 8] operator==(polymorphic_allocator<T>,  memory_resource*);
// [ 8] operator!=(polymorphic_allocator<T>,  polymorphic_allocator<T>);
// [ 8] operator!=(polymorphic_allocator<T1>, polymorphic_allocator<T2>);
// [ 8] operator!=(memory_resource *,         polymorphic_allocator<T>);
// [ 8] operator!=(polymorphic_allocator<T>,  memory_resource*);
//
// Specialized Traits
// [ 6] bsl::allocator_traits<bsl::polymorphic_allocator<E>>
// [ 2] bsl::is_trivially_copyable<bsl::polymorphic_allocator>
// [ 7] bslma::IsStdAllocator<bsl::polymorphic_allocator<E>>
// [ 2] bslmf::IsBitwiseEqualityComparable<bsl::polymorphic_allocator>
// [ 2] bslmf::IsBitwiseMoveable<bsl::polymorphic_allocator>
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [11] USAGE EXAMPLE
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
//                           PRINTF FORMAT MACROS
// ----------------------------------------------------------------------------
#define ZU BSLS_BSLTESTUTIL_FORMAT_ZU

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

class EqTestResource : public bsl::memory_resource {
    // Memory resource for testing equality operations.

    // DATA
    int                                 d_id;
    mutable const bsl::memory_resource *d_lastEqArg_p; // Argument from last eq

    // PRIVATE MANIPULATORS
    void *do_allocate(std::size_t bytes,
                      std::size_t alignment) BSLS_KEYWORD_OVERRIDE;
    void do_deallocate(void        *p,
                       std::size_t  bytes,
                       std::size_t  alignment) BSLS_KEYWORD_OVERRIDE;

    // PRIVATE ACCESSORS
    bool do_is_equal(const bsl::memory_resource& other) const
                                   BSLS_KEYWORD_NOEXCEPT BSLS_KEYWORD_OVERRIDE;

  public:
    // CREATORS
    explicit EqTestResource(int id) : d_id(id) { }

    // MANIPULATORS
    void reset() { d_lastEqArg_p = 0; }

    // ACCESSORS
    int id() const { return d_id; }
    const bsl::memory_resource *lastEqArg() const { return d_lastEqArg_p; }
};

void *EqTestResource::do_allocate(std::size_t, std::size_t)
{
    return 0;
}

void EqTestResource::do_deallocate(void *, std::size_t, std::size_t)
{
}

bool EqTestResource::do_is_equal(const bsl::memory_resource& other) const
                                                         BSLS_KEYWORD_NOEXCEPT
{
    d_lastEqArg_p = &other;

    const EqTestResource *pOther =
        dynamic_cast<const EqTestResource *>(&other);
    if (pOther) {
        return d_id == pOther->d_id;                                  // RETURN
    }

    return false;
}

struct NonAllocator {
    // An empty "allocator" class
};

template <class ALLOCATOR>
class TestType {
    // An allocator-aware test type that tracks its construction and
    // destruction.

    static TestType *s_lastDestroyed;  // Most recently destroyed object

    ALLOCATOR        d_allocator;
    long             d_value;          // sum of constructor arguments

  public:
    BSLMF_NESTED_TRAIT_DECLARATION(TestType, bslmf::UsesAllocatorArgT);

    typedef ALLOCATOR allocator_type;

    static TestType *lastDestroyed() { return s_lastDestroyed; }

    explicit TestType(long a01 = 0, long a02 = 0, long a03 = 0, long a04 = 0,
                      long a05 = 0, long a06 = 0, long a07 = 0, long a08 = 0,
                      long a09 = 0, long a10 = 0, long a11 = 0, long a12 = 0,
                      long a13 = 0, long a14 = 0);
    TestType(bsl::allocator_arg_t, const ALLOCATOR &alloc = ALLOCATOR(),
             long a01 = 0, long a02 = 0, long a03 = 0, long a04 = 0,
             long a05 = 0, long a06 = 0, long a07 = 0, long a08 = 0,
             long a09 = 0, long a10 = 0, long a11 = 0, long a12 = 0,
             long a13 = 0, long a14 = 0);

    ~TestType() { s_lastDestroyed = this; }

    allocator_type get_allocator() const { return d_allocator; }
    long value() const { return d_value; }
};

template <class ALLOCATOR>
TestType<ALLOCATOR> *TestType<ALLOCATOR>::s_lastDestroyed = 0;

template <class ALLOCATOR>
TestType<ALLOCATOR>::TestType(long a01, long a02, long a03, long a04,
                              long a05, long a06, long a07, long a08,
                              long a09, long a10, long a11, long a12,
                              long a13, long a14)
    : d_value(a01 + a02 + a03 + a04 + a05 + a06 + a07 + a08 + a09 + a10 +
              a11 + a12 + a13 + a14)
{
}

template <class ALLOCATOR>
TestType<ALLOCATOR>::TestType(bsl::allocator_arg_t, const ALLOCATOR &alloc,
                              long a01, long a02, long a03, long a04,
                              long a05, long a06, long a07, long a08,
                              long a09, long a10, long a11, long a12,
                              long a13, long a14)
    : d_allocator(alloc)
    , d_value(a01 + a02 + a03 + a04 + a05 + a06 + a07 + a08 + a09 + a10 +
              a11 + a12 + a13 + a14)
{
}

struct MyObject
{
    // A non-trivial-sized object.

    // DATA
    int  d_i;
    char d_s[10];
};

struct HasOperatorAmpersand {
    // Class that overrides 'operator&', for testing the 'address' method of
    // 'bsl::allocator'.

    HasOperatorAmpersand *operator&() { return 0;    }
    HasOperatorAmpersand *address()   { return this; }

    const HasOperatorAmpersand *operator&() const { return 0;    }
    const HasOperatorAmpersand *address()   const { return this; }
};

struct ConvertibleToPolyAlloc {
    // Type that is convertible to 'polymorphic_allocator' but does not inherit
    // from it, for testing 'operator=='.

    operator bsl::polymorphic_allocator<double>() const
        { return bsl::polymorphic_allocator<double>(); }
};

// ============================================================================
//                             USAGE EXAMPLES
// ----------------------------------------------------------------------------

///Usage
///-----
//
///Example 1: A class that allocates memory
///- - - - - - - - - - - - - - - - - - - -
// In this example, we define a class template, 'Holder<TYPE>', that holds a
// single instance of 'TYPE' on the heap.  'Holder' is designed such that its
// memory use can be customized by supplying an appropriate allocator.  A
// holder object can be empty and it can be move-constructed even if
// 'TYPE' is not movable.  In addition, the footprint of a 'Holder' object is
// the same (typically the size of 2 pointers), regardless of the size of
// 'TYPE'.
//
// First, we create a 'CountingResource' class, derived from
// 'bsl::memory_resource', that keeps track of the number of blocks of memory
// that were allocated from the resource but not yet returned to the resource;
// see usage example 1 in 'bslma_memoryresource'.
//..
//  #include <bslmf_movableref.h>
//  #include <bsls_assert.h>
//  #include <bsls_keyword.h>
//  #include <stdint.h>  // 'uintptr_t'

    class CountingResource : public bsl::memory_resource {

        // DATA
        int d_blocksOutstanding;

        CountingResource(const CountingResource&) BSLS_KEYWORD_DELETED;
        CountingResource& operator=(const CountingResource&)
                                                          BSLS_KEYWORD_DELETED;

      private:
        // PRIVATE MANIPULATORS
        void *do_allocate(std::size_t bytes,
                          std::size_t alignment) BSLS_KEYWORD_OVERRIDE;
        void do_deallocate(void        *p,
                           std::size_t  bytes,
                           std::size_t  alignment) BSLS_KEYWORD_OVERRIDE;

        // PRIVATE ACCESSORS
        bool do_is_equal(const bsl::memory_resource& other) const
                                   BSLS_KEYWORD_NOEXCEPT BSLS_KEYWORD_OVERRIDE;

      public:
        // CREATORS
        CountingResource() : d_blocksOutstanding(0) { }
        ~CountingResource() BSLS_KEYWORD_OVERRIDE;

        // ACCESSORS
        int blocksOutstanding() const { return d_blocksOutstanding; }
    };

    CountingResource::~CountingResource()
    {
        BSLS_ASSERT(0 == d_blocksOutstanding);
    }

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

    void CountingResource::do_deallocate(void *p, std::size_t, std::size_t)
    {
        ::operator delete(p);
        --d_blocksOutstanding;
    }

    bool CountingResource::do_is_equal(const bsl::memory_resource& other) const
                                                          BSLS_KEYWORD_NOEXCEPT
    {
        return this == &other;
    }
//..
// Now we define our actual 'Holder' template with with data members to hold
// the memory allocator and a pointer to the contained object:
//..
    template <class TYPE>
    class Holder {
        bsl::polymorphic_allocator<TYPE>  d_allocator;
        TYPE                             *d_data_p;
//..
// Next, we declare the constructors.  Following the pattern for
// allocator-aware types used in BDE, the public interface contains an
// 'allocator_type' typedef that can be passed to each constructor.:
//..
      public:
        // TYPES
        typedef bsl::polymorphic_allocator<TYPE> allocator_type;

        // CREATORS
        explicit Holder(const allocator_type& allocator = allocator_type());
        explicit Holder(const TYPE&           value,
                        const allocator_type& allocator = allocator_type());
        Holder(const Holder&         other,
               const allocator_type& allocator = allocator_type());
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
// 'bsl::polymorphic_allocator' is implicitly convertible from
// 'bsl::memory_resource *':
//..
    void usageExample1()
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
      case 11: {
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

      } break;
      case 10: {
        // --------------------------------------------------------------------
        // 'construct' AND 'destroy'
        //
        // Concerns:
        //: 1 Using 'construct' to construct an object with 0 to 14 arguments
        //:   passes those arguments to the object's constructor.
        //: 2 If the object is allocator-aware and uses a compatible allocator,
        //:   the allocator is passed to the object's constructor.
        //: 3 Destroy invokes the object's destructor.
        //
        // Plan:
        //: 1 Define a test type whose constructor takes 0 to 14 arguments.
        //:   Construct an object of that type using the 'construct' method and
        //:   verify that the arguments were passed to its constructor.  (C-1)
        //: 2 Define an allocator-aware test type whose constructor takes 0 to
        //:   14 arguments.  Construct an object of that type using the
        //:   'construct' method and verify that the arguments were passed to
        //:   its constructor.
        //: 3 Destroy the objects from steps 1 and 2 using the 'destroy'
        //:   method.  Verify that the objects' destructors were invoked.
        //
        // Testing:
        //
        // --------------------------------------------------------------------

        if (verbose) printf("\n'construct' AND 'destroy'"
                            "\n=========================\n");

        typedef TestType<NonAllocator>                     TT;
        typedef TestType<bsl::polymorphic_allocator<int> > AATT;

        bslma::TestAllocator ta("case 10", veryVeryVeryVerbose);
        bsl::polymorphic_allocator<> alloc(&ta);

        bsls::ObjectBuffer<TT>       buf1;
        bsls::ObjectBuffer<AATT>     buf2;

#define TEST(EXP, ...) do {                                                   \
        alloc.construct(&buf1.object(), __VA_ARGS__);                         \
        ASSERTV(EXP, buf1.object().value(), EXP == buf1.object().value());    \
        alloc.destroy(&buf1.object());                                        \
        ASSERTV(TT::lastDestroyed() == buf1.address());                       \
        alloc.construct(&buf2.object(), __VA_ARGS__);                         \
        ASSERT(alloc == buf2.object().get_allocator());                       \
        ASSERTV(EXP, buf2.object().value(), EXP == buf1.object().value());    \
        alloc.destroy(&buf2.object());                                        \
        ASSERTV(AATT::lastDestroyed() == buf2.address());                     \
    } while (false);

        // Test with no constructor arguments
        {
            alloc.construct(&buf1.object());
            ASSERTV(buf1.object().value(), 0 == buf1.object().value());
            alloc.destroy(&buf1.object());
            ASSERTV(TT::lastDestroyed() == buf1.address());

            alloc.construct(&buf2.object());
            ASSERT(alloc == buf2.object().get_allocator());
            ASSERTV(0, buf2.object().value(), 0 == buf1.object().value());
            alloc.destroy(&buf2.object());
            ASSERTV(AATT::lastDestroyed() == buf2.address());
        }

        //    Exp
        //   Value   Ctor Arguments
        //   ------  --------------
        TEST(0x0001, 0x0001);
        TEST(0x0003, 0x0001, 0x0002);
        TEST(0x0007, 0x0001, 0x0002, 0x0004);
        TEST(0x000f, 0x0001, 0x0002, 0x0004, 0x0008);
        TEST(0x001f, 0x0001, 0x0002, 0x0004, 0x0008, 0x0010);
        TEST(0x003f, 0x0001, 0x0002, 0x0004, 0x0008, 0x0010, 0x0020);
        TEST(0x007f, 0x0001, 0x0002, 0x0004, 0x0008, 0x0010, 0x0020, 0x0040);
        TEST(0x00ff, 0x0001, 0x0002, 0x0004, 0x0008, 0x0010, 0x0020, 0x0040,
             0x0080);
        TEST(0x01ff, 0x0001, 0x0002, 0x0004, 0x0008, 0x0010, 0x0020, 0x0040,
             0x0080, 0x0100);
        TEST(0x03ff, 0x0001, 0x0002, 0x0004, 0x0008, 0x0010, 0x0020, 0x0040,
             0x0080, 0x0100, 0x0200);
        TEST(0x07ff, 0x0001, 0x0002, 0x0004, 0x0008, 0x0010, 0x0020, 0x0040,
             0x0080, 0x0100, 0x0200, 0x0400);
        TEST(0x0fff, 0x0001, 0x0002, 0x0004, 0x0008, 0x0010, 0x0020, 0x0040,
             0x0080, 0x0100, 0x0200, 0x0400, 0x0800);
        TEST(0x1fff, 0x0001, 0x0002, 0x0004, 0x0008, 0x0010, 0x0020, 0x0040,
             0x0080, 0x0100, 0x0200, 0x0400, 0x0800, 0x1000);
        TEST(0x3fff, 0x0001, 0x0002, 0x0004, 0x0008, 0x0010, 0x0020, 0x0040,
             0x0080, 0x0100, 0x0200, 0x0400, 0x0800, 0x1000, 0x2000);
#undef TEST

      } break;
      case 9: {
        // --------------------------------------------------------------------
        // 'allocate' AND 'deallocate'
        //
        // Concerns:
        //: 1 For 'bsl::allocator<T>', calling 'allocate' with argument 'n'
        //:   will allocate 'n * sizeof(T)' bytes from the resource object.
        //: 2 A 'bsl::allocator<T2>' copied from a 'bsl::allocator<T1>' will
        //:   allocate blocks from the same resource, even though the blocks
        //:   have different sizes.
        //: 3 An optional 'hint' pointer can be passed to 'allocate' but will
        //:   have no effect.
        //: 4 Deallocating a block from a 'bsl::allocator' returns it to the
        //:   underlying resource object.
        //
        // Plan:
        //: 1 Create a 'bsl::allocator<int>' using a 'bslma::TestAllocator' and
        //:   call 'allocate' with different values for 'n', with and without a
        //:   hint pointer.  Verify that the blocks allocated from the test
        //:   allocator have size 'n * sizeof(int)'.  (C-1, C-3)
        //: 2 Initialize a 'bsl::allocator<char>' from the allocator in the
        //:   previous step.  Allocate several blocks from the new allocator
        //:   and verify that the blocks are correctly allocated from the same
        //:   test allocator as was used in the previous step.
        //: 3 Deallocate each of the blocks allocated in steps 1 and 2.  Verify
        //:   that the memory is returned to the test allocator.
        //
        // Testing:
        //    pointer allocate(size_type n, const void *hint = 0);
        //    void deallocate(TYPE *p, std::size_t n = 1);
        // --------------------------------------------------------------------

        if (verbose) printf("\n'allocate' AND 'deallocate'"
                            "\n===========================\n");

        bslma::TestAllocator ta("test 9", veryVeryVeryVerbose);

        // Step 1
        typedef bsl::polymorphic_allocator<int> IAlloc;
        typedef bsl::allocator_traits<IAlloc>   ITraits;
        IAlloc  ia(&ta);

        int    *p1 = ia.allocate(1);
        ASSERT(1               == ta.numBlocksInUse());
        ASSERT(1 * sizeof(int) == ta.numBytesInUse());

        int    *p2 = ITraits::allocate(ia, 2, p1);  // Use hint
        ASSERT(2               == ta.numBlocksInUse());
        ASSERT(3 * sizeof(int) == ta.numBytesInUse());

        // Step 2
        typedef bsl::polymorphic_allocator<char> CAlloc;
        typedef bsl::allocator_traits<CAlloc>    CTraits;
        CAlloc ca(ia);
        ASSERT(&ta == ca.resource());

        char *p3 = ca.allocate(1);
        ASSERT(3                                  == ta.numBlocksInUse());
        ASSERT(1 * sizeof(char) + 3 * sizeof(int) == ta.numBytesInUse());

        char *p4 = CTraits::allocate(ca, 3, p3);  // Use hint
        ASSERT(4                                  == ta.numBlocksInUse());
        ASSERT(4 * sizeof(char) + 3 * sizeof(int) == ta.numBytesInUse());

        // Step 3
        ia.deallocate(p1, 1);
        ASSERT(3                                  == ta.numBlocksInUse());
        ASSERT(4 * sizeof(char) + 2 * sizeof(int) == ta.numBytesInUse());
        ca.deallocate(p4, 3);
        ASSERT(2                                  == ta.numBlocksInUse());
        ASSERT(1 * sizeof(char) + 2 * sizeof(int) == ta.numBytesInUse());
        ca.deallocate(p3, 1);
        ASSERT(1               == ta.numBlocksInUse());
        ASSERT(2 * sizeof(int) == ta.numBytesInUse());
        ia.deallocate(p2, 2);
        ASSERT(0 == ta.numBlocksInUse());
        ASSERT(0 == ta.numBytesInUse());

      } break;
      case 8: {
        // --------------------------------------------------------------------
        // 'operator==' AND 'operator!='
        //
        // Concerns:
        //: 1 Two 'bsl::polymorphic_allocator' objects that contain the same
        //:   'bsl::memory_resource' pointer compare equal using 'operator=='.
        //:   The 'bsl::memory_resource::is_equal' member function is not
        //:   invoked.
        //: 2 Two 'bsl::polymorphic_allocator' objects that contain different
        //:   'bsl::memory_resource' pointers compare equal if the 'is_equal'
        //:   member function of 'memory_resource' returns 'true'.
        //: 3 Two 'bsl::polymorphic_allocator' objects that contain different
        //:   'bsl::memory_resource' pointers do not compare equal if the
        //:   'is_equal' member function of 'memory_resource' returns 'false'.
        //: 4 Concerns 1-3 apply regardless of whether the two
        //:   'bsl::polymorphic_allocator' objects have the same 'value_type'.
        //: 5 A 'bsl::polymorphic_allocator' can be compared for equality
        //:   against a pointer to 'bsl::memory_resource'; the result is the
        //:   same as comparing it against a 'bsl::polymorphic_allocator' that
        //:   was initialized with the 'bsl::memory_resource' address.
        //: 6 A 'bsl::polymorphic_allocator' can be compared for equality
        //:   against an object convertible via a conversion operator to the
        //:   same 'bsl::polymorphic_allocator' instantiation.
        //: 7 In all cases, 'operator!=' returns the inverse of 'operator=='.
        //: 8 Every comparison is commutative; the return value is the same
        //:   regardless of which operand is on the left.
        //
        // Plan:
        //: 1 Define a class derived from 'bsl::memory_resource' having a
        //:   constructor-supplied integer ID.  The 'do_is_equal' method for
        //:   this class will return true *iff* the IDs compare equal.  In
        //:   addition, the 'do_is_equal' method will remember the argument
        //:   argument for its most resent invocation.  For this test, the
        //:   'do_allocate' and 'do_deallocate' methods are not called and can
        //:   simply return null.  Create three instances of this class, two
        //:   having the same ID and a third having a different ID.
        //: 2 Perform the remaining steps in a loop, each iteration using a
        //:   different combination of two of resources from step 1, 'r1' and
        //:   'r2' (where 'r1' and 'r2' might be the same object).  Create two
        //:   objects of 'bsl::polymorphic_allocator', 'pa1' and 'pa2',
        //:   initialized from the addresses of 'r1' and 'r2', respecively.
        //: 3 Verify that, when 'r1' and 'r2' are the same object, that 'pa1 ==
        //:   pa2' is true and that no calls are made to 'do_is_equal'.  As a
        //:   special case, test self-equality is always true and that no calls
        //:   are made to 'do_is_equal' when comparing an object to itself.
        //:   (C-1)
        //: 4 Verify that, when 'r1' and 'r2' are different objects with the
        //:   same ID, that 'pa1 == pa2' is true and that one call is made to
        //:   'r1.do_is_equal(&r2)'.  (C-2)
        //: 5 Verify that, when 'r1' and 'r2' have different IDs, that 'pa1 ==
        //:   Pa2' is false and that one call is made to
        //:   'r1.do_is_equal(&r2)'.  (C-3)
        //: 6 Ensure that 'pa1' and 'pa2' are have different value type
        //:   arguments in step 2.  The self-equality test in step 3 ensures
        //:   that 'operator==' and 'operator!=' work with the same value type
        //:   whereas the remaining tests ensure that they work with different
        //:   value types.  (C-4)
        //: 7 Verify that 'pa1 == &r2' and that '&r1 == pa2' have the same
        //:   result as 'pa1 == pa2', including whether 'do_is_equal' is
        //:   called.  (C-5)
        //: 8 Create a class containing a conversion operator to
        //:   'polymorphic_allocator<double>'.  Verify that a
        //:   'polymorphic_allocator<double>' can be compared for equality
        //:   with an object of that class.  (C-6)
        //: 9 In each of the steps testing '==', verify that '!=' returns the
        //:   inverse.  (C-7)
        //: 10 For each step involving '==' and '!=', repeat the step with the
        //:   arguments reversed.  (C-8)
        //
        // Testing:
        //    operator==(polymorphic_allocator<T>,  polymorphic_allocator<T>);
        //    operator==(polymorphic_allocator<T1>, polymorphic_allocator<T2>);
        //    operator==(memory_resource *,         polymorphic_allocator<T>);
        //    operator==(polymorphic_allocator<T>,  memory_resource*);
        //    operator!=(polymorphic_allocator<T>,  polymorphic_allocator<T>);
        //    operator!=(polymorphic_allocator<T1>, polymorphic_allocator<T2>);
        //    operator!=(memory_resource *,         polymorphic_allocator<T>);
        //    operator!=(polymorphic_allocator<T>,  memory_resource*);
        // --------------------------------------------------------------------

        if (verbose) printf("\n'operator==' AND 'operator!='"
                            "\n=============================\n");

        // Create resources
        EqTestResource etrA(0);
        EqTestResource etrB(0);
        EqTestResource etrC(1);

        // Loop over combinations of resources
        EqTestResource *const resources[] = { &etrA, &etrB, &etrC };
        const int numResources = sizeof(resources) / sizeof(resources[0]);

        for (int i = 0; i < numResources; ++i) {
            for (int j = i; j < numResources; ++j) {  // 'j' can equal 'i'
                EqTestResource& r1(*resources[i]);
                EqTestResource& r2(*resources[j]);
                r1.reset();
                r2.reset();

                // 'polymorphic_allocator's with different value types
                bsl::polymorphic_allocator<char> pa1(&r1);
                bsl::polymorphic_allocator<int>  pa2(&r2);

                // self-equality test
                ASSERTV(i, j,    pa1 == pa1);
                ASSERTV(i, j, ! (pa1 != pa1));
                ASSERTV(i, j, 0 == r1.lastEqArg());
                ASSERTV(i, j, 0 == r2.lastEqArg());
                r1.reset();

                // Determine expected results
                const bool expEq                         = r1.id() == r2.id();
                const bool expNe                         = r1.id() != r2.id();
                const bsl::memory_resource *expLastEqArg =
                    (&r1 != &r2) ? &r2 : 0;

                // Compare 'polymorphic_allocator' objects
                ASSERTV(i, j, expEq        == (pa1 == pa2));
                ASSERTV(i, j, expLastEqArg == r1.lastEqArg());
                ASSERTV(i, j, 0            == r2.lastEqArg());
                r1.reset();
                ASSERTV(i, j, expNe        == (pa1 != pa2));
                ASSERTV(i, j, expLastEqArg == r1.lastEqArg());
                ASSERTV(i, j, 0            == r2.lastEqArg());
                r1.reset();

                // Compare 'polymorphic_allocator' against 'memory_resource *'
                ASSERTV(i, j, expEq        == (pa1 == &r2));
                ASSERTV(i, j, expLastEqArg == r1.lastEqArg());
                ASSERTV(i, j, 0            == r2.lastEqArg());
                r1.reset();
                ASSERTV(i, j, expEq        == (&r1 == pa2));
                ASSERTV(i, j, expLastEqArg == r1.lastEqArg());
                ASSERTV(i, j, 0            == r2.lastEqArg());
                r1.reset();
                ASSERTV(i, j, expNe        == (pa1 != &r2));
                ASSERTV(i, j, expLastEqArg == r1.lastEqArg());
                ASSERTV(i, j, 0            == r2.lastEqArg());
                r1.reset();
                ASSERTV(i, j, expNe        == (&r1 != pa2));
                ASSERTV(i, j, expLastEqArg == r1.lastEqArg());
                ASSERTV(i, j, 0            == r2.lastEqArg());
                r1.reset();

                // Reverse arguments for all of the above tests
                expLastEqArg = (&r2 != &r1) ? &r1 : 0;
                ASSERTV(i, j, expEq        == (pa2 == pa1));
                ASSERTV(i, j, 0            == r1.lastEqArg());
                ASSERTV(i, j, expLastEqArg == r2.lastEqArg());
                r2.reset();
                ASSERTV(i, j, expNe        == (pa2 != pa1));
                ASSERTV(i, j, 0            == r1.lastEqArg());
                ASSERTV(i, j, expLastEqArg == r2.lastEqArg());
                r2.reset();
                ASSERTV(i, j, expEq        == (&r2 == pa1));
                ASSERTV(i, j, 0            == r1.lastEqArg());
                ASSERTV(i, j, expLastEqArg == r2.lastEqArg());
                r2.reset();
                ASSERTV(i, j, expEq        == (pa2 == &r1));
                ASSERTV(i, j, 0            == r1.lastEqArg());
                ASSERTV(i, j, expLastEqArg == r2.lastEqArg());
                r2.reset();
                ASSERTV(i, j, expNe        == (&r2 != pa1));
                ASSERTV(i, j, 0            == r1.lastEqArg());
                ASSERTV(i, j, expLastEqArg == r2.lastEqArg());
                r2.reset();
                ASSERTV(i, j, expNe        == (pa2 != &r1));
                ASSERTV(i, j, 0            == r1.lastEqArg());
                ASSERTV(i, j, expLastEqArg == r2.lastEqArg());
                r2.reset();
            }
        }

        // The MSVC workaround for 'type_identity' prevents comparison against
        // a type convertible to 'polymorphic_allocator' via a conversion
        // operator.  As this is a rare-to-nonexistant use case, we won't
        // bother to fix that for now.
#if ! defined(BSLS_PLATFORM_CMP_MSVC)
        {
            bsl::polymorphic_allocator<double> pa;
            ConvertibleToPolyAlloc             ca;
            ASSERT(pa == ca);
            ASSERT(ca == pa);
        }
#endif

      } break;
      case 7: {
        // --------------------------------------------------------------------
        // 'bslma::IsStdAllocator'
        //
        // Concerns:
        //: 1 The trait 'bslma::IsStdAllocator<bsl::polymorphic_allocator<E>>'
        //:   inherits from 'bsl::true_type' for any complete type 'E'.
        //
        // Plan:
        //: 1 For various complete types, 'E', create an instance of
        //:   'bslma::IsStdAllocator<bsl::polymorphic_allocator<E>>' and verify
        //:   that the address of that instance is convertible to
        //:   'bsl::true_type *'. (C-1)
        //
        // Testing:
        //   bslma::IsStdAllocator<bsl::polymorphic_allocator<E>>
        // --------------------------------------------------------------------

        if (verbose) printf("\nUSAGE EXAMPLE"
                            "\n=============\n");

        bslma::IsStdAllocator<bsl::polymorphic_allocator<int>   > xI;
        bslma::IsStdAllocator<bsl::polymorphic_allocator<float> > xF;
        bslma::IsStdAllocator<bsl::polymorphic_allocator<long>  > xL;

        ASSERT( xI.value);
        ASSERT( xF.value);
        ASSERT( xL.value);

        // Pointer conversion will fail unless type on right is derived from
        // type on left.
        bsl::true_type  *pI = &xI;
        bsl::true_type  *pF = &xF;
        bsl::true_type  *pL = &xL;

        (void) pI;
        (void) pF;
        (void) pL;

      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING ALLOCATOR_TRAITS
        //
        // Concerns:
        //: 1 that the specialization of 'bsl::allocator_traits' for
        //    'bsl::polymorphic_allocator' produces the correct member
        //    typedefs.
        //
        // Plan:
        //: 1 The testing is straightforward and follows the concerns.
        //
        // Testing:
        //   bsl::allocator_traits<bsl::polymorphic_allocator<E>>
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING ALLOCATOR_TRAITS"
                            "\n========================\n");

        typedef bsl::polymorphic_allocator<int>   AI;
        typedef bsl::polymorphic_allocator<float> AF;
        typedef bsl::polymorphic_allocator<long>  AL;

        bslma::TestAllocator ta;
        AI a_def;  // Default
        AI ai(&ta);
        AF af(&ta);
        AL al(&ta);

        typedef bsl::allocator_traits<AI> TI;
        typedef bsl::allocator_traits<AF> TF;
        typedef bsl::allocator_traits<AL> TL;


        if (verbose) printf("\tTesting 'allocator_type'.\n");
        {
            ASSERT((bsl::is_same<TI::allocator_type, AI>::value));
            ASSERT((bsl::is_same<TF::allocator_type, AF>::value));
        }


        if (verbose) printf("\tTesting 'value_type'.\n");
        {
            ASSERT((bsl::is_same<TI::value_type, int>::value));
            ASSERT((bsl::is_same<TI::value_type, AI::value_type>::value));

            ASSERT((bsl::is_same<TF::value_type, float>::value));
            ASSERT((bsl::is_same<TF::value_type, AF::value_type>::value));

            ASSERT((bsl::is_same<TL::value_type, long>::value));
            ASSERT((bsl::is_same<TL::value_type, AL::value_type>::value));
        }

#ifndef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
        // Only the C++03 version of 'polymorphic_allocator' has these types:

        if (verbose) printf("\tTesting 'pointer'.\n");
        {
            ASSERT((bsl::is_same<TI::pointer, AI::pointer>::value));
            ASSERT((bsl::is_same<TF::pointer, AF::pointer>::value));
        }

        if (verbose) printf("\tTesting 'const_pointer'.\n");
        {
            ASSERT((bsl::is_same<TI::const_pointer,
                                 AI::const_pointer>::value));
            ASSERT((bsl::is_same<TF::const_pointer,
                                 AF::const_pointer>::value));
        }

        if (verbose) printf("\tTesting 'void_pointer'.\n");
        {
            ASSERT((bsl::is_same<TI::void_pointer, void *>::value));
            ASSERT((bsl::is_same<TF::void_pointer, void *>::value));
        }

        if (verbose) printf("\tTesting 'const_void_pointer'.\n");
        {
            ASSERT((bsl::is_same<TI::const_void_pointer, const void*>::value));
        }

        if (verbose) printf("\tTesting 'difference_type'.\n");
        {
            ASSERT((bsl::is_same<TI::difference_type,
                                 AI::difference_type>::value));
            ASSERT((bsl::is_same<TF::difference_type,
                                 AF::difference_type>::value));
        }

        if (verbose) printf("\tTesting 'size_type'.\n");
        {
            ASSERT((bsl::is_same<TI::size_type, AI::size_type>::value));
            ASSERT((bsl::is_same<TF::size_type, AF::size_type>::value));
        }

        if (verbose) printf("\tTesting 'rebind_alloc'.\n");
        {
#ifdef BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES
            ASSERT((bsl::is_same<TI::rebind_alloc<long>,
                                 AI::rebind<long>::other>::value));
            ASSERT((bsl::is_same<TF::rebind_alloc<long>,
                                 AF::rebind<long>::other>::value));
#else
            ASSERT((bsl::is_convertible<TI::rebind_alloc<long>,
                                        AI::rebind<long>::other>::value));
            ASSERT((bsl::is_convertible<TF::rebind_alloc<long>,
                                        AF::rebind<long>::other>::value));
#endif
        }

        if (verbose) printf("\tTesting 'max_size()'.\n");
        {
            ASSERT(TI::max_size(ai) == ai.max_size());
            ASSERT(TF::max_size(ai) == af.max_size());
            ASSERT(TL::max_size(ai) == al.max_size());
        }
#endif // ! BSLS_LIBRARYFEATURES_HAS_CPP17_PMR

        if (verbose) printf("\tTesting 'rebind_traits'.\n");
        {
#ifdef BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES
            ASSERT((bsl::is_same<TI::rebind_traits<long>,TL>::value));
            ASSERT((bsl::is_same<TF::rebind_traits<long>,TL>::value));
#else
            ASSERT((bsl::is_convertible<TI::rebind_traits<long>,TL>::value));
            ASSERT((bsl::is_same<TI::rebind_traits<long>::allocator_type,
                                 AL>::value));

            ASSERT((bsl::is_convertible<TF::rebind_traits<long>,TL>::value));
            ASSERT((bsl::is_same<TF::rebind_traits<long>::allocator_type,
                                 AL>::value));
#endif
        }

        if (verbose)
            printf("\tTesting 'select_on_container_copy_construction()'.\n");
        {
            ASSERT(a_def == TI::select_on_container_copy_construction(ai));
            ASSERT(a_def == TF::select_on_container_copy_construction(af));
            ASSERT(a_def == TL::select_on_container_copy_construction(al));
        }

        if (verbose) printf("\tTesting boolean traits.\n");
        {
            ASSERT(! TI::is_always_equal::value);
            ASSERT(! TF::is_always_equal::value);
            ASSERT(! TL::is_always_equal::value);

            ASSERT(! TI::propagate_on_container_copy_assignment::value);
            ASSERT(! TF::propagate_on_container_copy_assignment::value);
            ASSERT(! TL::propagate_on_container_copy_assignment::value);

            ASSERT(! TI::propagate_on_container_move_assignment::value);
            ASSERT(! TF::propagate_on_container_move_assignment::value);
            ASSERT(! TL::propagate_on_container_move_assignment::value);

            ASSERT(! TI::propagate_on_container_swap::value);
            ASSERT(! TF::propagate_on_container_swap::value);
            ASSERT(! TL::propagate_on_container_swap::value);
        }
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING NESTED TYPES
        //
        // Concerns:
        //: 1 that 'size_type' is unsigned while 'difference_type' is signed.
        //: 2 that size_type and difference_type are the right size (i.e., they
        //:   can represent any difference of pointers in the memory model)
        //: 3 that all other types exist and are as specified by the C++
        //:   standard
        //: 4 that if Y is X::rebind<U>::other, then Y::rebind<T>::other is
        //:    the same type as X
        //
        // Plan:
        //: 1 The testing is straightforward and follows the concerns.
        //
        // Testing:
        //   size_type
        //   difference_type
        //   pointer;
        //   const_pointer;
        //   reference;
        //   const_reference;
        //   value_type;
        //   template rebind<U>::other
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING NESTED TYPES"
                            "\n====================\n");

        typedef bsl::polymorphic_allocator<int>   AI;
        typedef bsl::polymorphic_allocator<float> AF;

#ifndef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
        // Most of these types exist only in the C++03 implementation

        if (verbose) printf("\tTesting 'size_type'.\n");
        {
            ASSERT(sizeof(AI::size_type) == sizeof(int*));

            ASSERT(0 < ~(AI::size_type)0);
        }

        if (verbose) printf("\tTesting 'difference_type'.\n");
        {
            ASSERT(sizeof(AI::difference_type) == sizeof(int*));

            ASSERT(0 > ~(AI::difference_type)0);
        }

        if (verbose) printf("\tTesting 'pointer'.\n");
        {
            ASSERT((bsl::is_same<AI::pointer, int*>::value));
            ASSERT((bsl::is_same<AF::pointer, float*>::value));
        }

        if (verbose) printf("\tTesting 'const_pointer'.\n");
        {
            ASSERT((bsl::is_same<AI::const_pointer, const int*>::value));
            ASSERT((bsl::is_same<AF::const_pointer, const float*>::value));
        }

        if (verbose) printf("\tTesting 'reference'.\n");
        {
            ASSERT((bsl::is_same<AI::reference, int&>::value));
            ASSERT((bsl::is_same<AF::reference, float&>::value));
        }

        if (verbose) printf("\tTesting 'const_reference'.\n");
        {
            ASSERT((bsl::is_same<AI::const_reference, const int&>::value));
            ASSERT((bsl::is_same<AF::const_reference, const float&>::value));
        }

        if (verbose) printf("\tTesting 'rebind'.\n");
        {
            ASSERT((bsl::is_same<AI::rebind<int  >::other, AI>::value));
            ASSERT((bsl::is_same<AI::rebind<float>::other, AF>::value));
            ASSERT((bsl::is_same<AF::rebind<int  >::other, AI>::value));
            ASSERT((bsl::is_same<AF::rebind<float>::other, AF>::value));
        }
#endif // ! BSLS_LIBRARYFEATURES_HAS_CPP17_PMR

        if (verbose) printf("\tTesting 'value_type'.\n");
        {
            ASSERT((bsl::is_same<AI::value_type, int>::value));
            ASSERT((bsl::is_same<AF::value_type, float>::value));
        }

      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING ACCESSORS
        //
        // Concerns:
        //: 1 that the correct 'bsl::memory_resource*' is returned by the
        //:   'resource' accessor.
        //: 2 that the result of 'max_size' fits and represents the maximum
        //:   possible number of bytes in a 'size_type'.
        //: 3 that 'address' returns the address of its argument, ignoring any
        //:   'operator&' member of the argument type.
        //
        // Plan:
        //: 1 The concerns are straightforward to test.
        //
        // Testing:
        //   pointer address(reference x) const;
        //   const_pointer address(const_reference x) const;
        //   bsl::memory_resource *resource() const;
        //   size_type max_size() const;
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING ACCESSORS"
                            "\n=================\n");

        bsl::memory_resource *dflt = bslma::Default::allocator();

        if (verbose) printf("\tTesting 'resource()'.\n");
        {
            bslma::TestAllocator ta(veryVeryVeryVerbose);

            bsl::polymorphic_allocator<int>  ai1;
            ASSERT(dflt == ai1.resource());
            bsl::polymorphic_allocator<int>  ai2(&ta);
            ASSERT(&ta  == ai2.resource());
        }

#ifndef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR

        if (verbose) printf("\tTesting 'address()'.\n");
        {
            bsl::polymorphic_allocator<int> ai;
            int                             i  = 0; const int& I = i;
            int                            *pi = ai.address(i);
            const int                      *pI = ai.address(I);
            ASSERT(&i == pi);
            ASSERT(&I == pI);

            bsl::polymorphic_allocator<HasOperatorAmpersand> ah;
            HasOperatorAmpersand                             mHoa;
            const HasOperatorAmpersand&                      HOA   = mHoa;

            // 'HasOperatorAmpersand::operator&' returns a null pointer whereas
            // 'HasOperatorAmpersand::address()' returns the real address.
            ASSERT(0 == &mHoa);
            ASSERT(0 == &HOA);
            ASSERT(0 != mHoa.address());
            ASSERT(0 != HOA.address());

            // 'bsl::polymorphic_allocator::address(x)' returns the real
            // address of 'x', ignoring the presence of 'x.operator&()'.
            HasOperatorAmpersand       *ph = ah.address(mHoa);
            const HasOperatorAmpersand *pH = ah.address(HOA);
            ASSERT(mHoa.address() == ph);
            ASSERT(HOA.address()  == pH);
        }

        if (verbose) printf("\tTesting 'max_size()'.\n");
        {
            typedef bsl::polymorphic_allocator<>::size_type bsize;

            bsl::polymorphic_allocator<> byteAlloc;
            bsize cas = byteAlloc.max_size();

            // verify that max_size() is the largest positive integer of type
            // size_type
            ASSERTV(cas, cas > 0);
            ASSERTV(cas, cas == std::numeric_limits<bsize>::max());

            if (verbose) {
                printf("cas = " ZU "\n", cas);
            }

            bsl::polymorphic_allocator<MyObject> objAlloc;

            // Detect problem with MSVC in 64-bit mode, which can't do 64-bit
            // int arithmetic correctly for enums.
            ASSERT(objAlloc.max_size() < byteAlloc.max_size());

            bsize oas = objAlloc.max_size();
            bsize oass = oas * sizeof(MyObject);
            bsize oassplus = oass + sizeof(MyObject);
            ASSERTV(oas, oas > 0);
            ASSERTV(oass, oass < cas);
            ASSERTV(oass, oass > oas);         // no overflow
            ASSERTV(oassplus, oassplus < oas); // overflow

            if (verbose) {
                printf("\tAs unsigned long: oas = " ZU ", oass = " ZU ", "
                       "oassplus = " ZU ".\n", oas, oass, oassplus);
            }
        }
#endif // ! BSLS_LIBRARYFEATURES_HAS_CPP17_PMR

      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING CONSTRUCTORS AND DESTRUCTOR
        //
        // Concerns:
        //: 1 That a default-constructed allocator uses 'NewDeleteAllocator' if
        //:   constructed before any call to a method of 'bslma::Default' and
        //:   uses the default bslma allocator if constructed after such a
        //:   call.
        //: 2 that an allocator can be constructed from the various
        //:   constructors and that it uses the correct resource object.
        //: 3 that an allocator can be constructed from an allocator to a
        //:   different type.
        //: 4 that the destructor does not destroy its resource
        //
        // Plan:
        //: 1 Default construct a 'bsl::polymorphic_allocator' and verify that
        //:   its resource pointer points to the
        //:   'bslma::NewDeleteAllocator::singleton()'.  Call
        //:   'bslma::Default::setDefaultAllocator()' then construct another
        //:   'bsl::polymorphic_allocator'.  Verify that the second allocator's
        //:   resource points to the new default.
        //: 2 We construct a number of allocators from various resources, and
        //:   test that they do compare equal to the selected resource.  Copy
        //:   constructed allocators have to compare equal to their original
        //:   values.  (C-2, C-3)
        //: 3 Construct a 'bsl::polymorphic_allocator' from a test allocator.
        //:   Allocate a byte from the test allocator, then let the
        //:   'bsl::polymorphic_allocator' object go out of scope.  Verify that
        //:   the test allocator did not detect a leak (i.e., its destructor
        //:   was not called with outstanding memory blocks.  (C-4)
        //
        // Testing:
        //   polymorphic_allocator();
        //   polymorphic_allocator(bsl::memory_resource *);
        //   polymorphic_allocator(const polymorphic_allocator&);
        //   polymorphic_allocator(const polymorphic_allocator<U>&);
        //   ~polymorphic_allocator();
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING CONSTRUCTORS AND DESTRUCTOR"
                            "\n===================================\n");

        bslma::Allocator *ndRsrc = &bslma::NewDeleteAllocator::singleton();
        bsl::polymorphic_allocator<int>  ai1a;       ASSERT(ndRsrc == ai1a);

        bslma::TestAllocator dflt("Default allocator");
        bslma::Default::setDefaultAllocatorRaw(&dflt);
        bsl::polymorphic_allocator<int>  ai1b;       ASSERT(&dflt == ai1b);
        bslma::Default::setDefaultAllocatorRaw(ndRsrc);

        bslma::TestAllocator ta("test case 3", veryVeryVeryVerbose);

        bsl::polymorphic_allocator<int>  ai2(&ta);   ASSERT(&ta  == ai2);
        bsl::polymorphic_allocator<int>  ai3(ai2);   ASSERT(&ta  == ai3);

        // Test that destructor does not destroy the resource object
        void *block;
        {
            bsl::polymorphic_allocator<int> ai(&ta);
            block = ta.allocate(1);
            ASSERT(1 == ta.numBlocksInUse());
        } // ai goes out of scope here
        ASSERT(1 == ta.numBlocksInUse());  // ta was not destroyed
        ta.deallocate(block);

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING TRAITS
        //
        // Concerns:
        //: 1 That an allocator has the proper traits defined.
        //
        // Plan:
        //: 1 Since it does not matter what type 'bsl::polymorphic_allocator'
        //:   is instantiated with, use 'int' and test for each expected trait.
        //
        // Testing:
        //   bsl::is_trivially_copyable<bsl::polymorphic_allocator>
        //   bslmf::IsBitwiseEqualityComparable<sl::allocator>
        //   bslmf::IsBitwiseMoveable<bsl::polymorphic_allocator>
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING TRAITS"
                            "\n==============\n");

        typedef bsl::polymorphic_allocator<int> Alloc;

        ASSERT((bslmf::IsBitwiseMoveable<Alloc>::value));
        ASSERT((bsl::is_trivially_copyable<Alloc>::value));
        ASSERT((bslmf::IsBitwiseEqualityComparable<Alloc>::value));

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

        typedef bsl::polymorphic_allocator<char> Obj;

        // Exercise 'allocate' and 'deallocate'
        bslma::TestAllocator ta;
        Obj                  daAlloc(bslma::Default::defaultAllocator());
        Obj                  taAlloc(&ta);
        ASSERT(daAlloc == Obj());
        ASSERT(&ta == taAlloc.resource());
        ASSERT(taAlloc != daAlloc);

        char *p = taAlloc.allocate(3);
        ASSERT(p);
        ASSERT(3 == ta.numBytesInUse());
        ASSERT(p == ta.lastAllocatedAddress());

        taAlloc.deallocate(p, 3);
        ASSERT(0 == ta.numBytesInUse());
        ASSERT(p == ta.lastDeallocatedAddress());

        // Check that 'allocator_traits' instantiates correctly
        typedef bsl::allocator_traits<Obj> Traits;
        ASSERT(! Traits::is_always_equal::value);
        ASSERT(! Traits::propagate_on_container_swap::value);
        ASSERT(daAlloc ==
               Traits::select_on_container_copy_construction(taAlloc));

        // Check that 'IsStdAllocator' produces the correct result.
        ASSERT(bslma::IsStdAllocator<Obj>::value);

        // Check default template parameter type
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
        typedef std::byte ExpDefaultType;
#else
        typedef unsigned char ExpDefaultType;
#endif
        ASSERT((bsl::is_same<ExpDefaultType,
                bsl::polymorphic_allocator<>::value_type>::value));

        ASSERT(bslmf::IsTriviallyCopyableCheck<Obj>::value);

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
