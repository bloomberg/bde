// bslma_bslallocator.t.cpp                                           -*-C++-*-

#include <bslma_bslallocator.h>

#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>

#include <bslmf_isbitwisemoveable.h>
#include <bslmf_issame.h>
#include <bslmf_istriviallycopyable.h>

#include <bsls_asserttest.h>
#include <bsls_bsltestutil.h>
#include <bsls_objectbuffer.h>

#include <limits>
#include <new>

#include <stdio.h>
#include <stdlib.h>

using namespace BloombergLP;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// A `bsl::allocator` is a reference-semantic type that holds a pointer to a
// `bslma::Allocator` object (its underlying "mechanism").  This pointer can be
// set at construction (and if 0 is passed, then it uses `bslma_default` to
// substitute a pointer to the currently installed default allocator), and it
// can be accessed through the `mechanism` accessor.  It cannot be reset,
// however, since normally an allocator does not change during the lifetime of
// an object.  A `bsl::allocator` is parameterized by the type it allocates,
// and that influences the behavior of several manipulators and accessors,
// mainly depending on the size of that type.  A `bsl::allocator` instantiated
// on a different type can be obtained using the `rebind` nested template.
//
// Our specific concerns are that an allocator constructed with a certain
// underlying mechanism actually uses that mechanism to allocate memory, and
// that its rebound versions do as well.  Another concern is that `max_size()`
// is the maximum possible size for that type (i.e., it is impossible to
// meaningfully pass in a larger size), that `size_type` is unsigned, that
// `difference_type` is signed, and that, generally, all the requirements of
// C++ standard allocators are met in both C++17 (20.5.3.5
// [allocator.requirements]) and C++03 (20.1.5 [lib.allocator.requirements]).
//-----------------------------------------------------------------------------
// Creators
// [ 3] allocator();
// [ 3] allocator(bslma::Allocator *);
// [ 3] allocator(const allocator&);
// [ 3] allocator(const allocator<U>&);
// [ 3] ~allocator();
//
// Modifiers
// [11] allocator& operator=(const allocator& rhs);
// [ 9] pointer allocate(size_type n, const void *hint = 0);
// [ 9] void deallocate(pointer p, size_type n = 1);
// [10] void construct(pointer p, const TYPE& val, ARGS&&... arguments);
// [10] void destroy(pointer p);
//
// Accessors
// [ 4] pointer address(reference x) const;
// [ 4] const_pointer address(const_reference x) const;
// [ 4] bslma::Allocator *mechanism() const;
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
// [ 8] bool operator==(bsl::allocator<T>,  bsl::allocator<T>);
// [ 8] bool operator==(bsl::allocator<T1>,  bsl::allocator<T2>);
// [ 8] bool operator==(bslma::Allocator *, bsl::allocator<T>);
// [ 8] bool operator==(bsl::allocator<T>,  bslma::Allocator*);
// [ 8] bool operator!=(bsl::allocator<T>,  bsl::allocator<T>);
// [ 8] bool operator!=(bsl::allocator<T1>,  bsl::allocator<T2>);
// [ 8] bool operator!=(bslma::Allocator *, bsl::allocator<T>);
// [ 8] bool operator!=(bsl::allocator<T>,  bslma::Allocator*);
//
// Specialized Traits
// [ 6] bsl::allocator_traits<bsl::allocator<E>>
// [ 7] bslma::IsStdAllocator<bsl::allocator<E>>
// [ 2] bslmf::IsBitwiseCopyable<bsl::allocator>
// [ 2] bslmf::IsBitwiseEqualityComparable<bsl::allocator>
// [ 2] bslmf::IsBitwiseMoveable<bsl::allocator>
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [12] USAGE EXAMPLE
//-----------------------------------------------------------------------------

// ============================================================================
//                      STANDARD BDE ASSERT TEST MACROS
// ----------------------------------------------------------------------------
// NOTE: THIS IS A LOW-LEVEL COMPONENT AND MAY NOT USE ANY C++ LIBRARY
// FUNCTIONS, INCLUDING IOSTREAMS.

namespace {

int testStatus = 0;

void aSsErT(bool b, const char *s, int i)
{
    if (b) {
        printf("Error " __FILE__ "(%d): %s    (failed)\n", i, s);
        fflush(stdout);
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}

}  // close unnamed namespace

//=============================================================================
//                       STANDARD BDE TEST DRIVER MACROS
//-----------------------------------------------------------------------------

#define ASSERT       BSLS_BSLTESTUTIL_ASSERT
#define ASSERTV      BSLS_BSLTESTUTIL_ASSERTV

#define Q   BSLS_BSLTESTUTIL_Q   // Quote identifier literally.
#define P   BSLS_BSLTESTUTIL_P   // Print identifier and value.
#define P_  BSLS_BSLTESTUTIL_P_  // P(X) without '\n'.
#define T_  BSLS_BSLTESTUTIL_T_  // Print a tab (w/o newline).
#define L_  BSLS_BSLTESTUTIL_L_  // current Line number

// ============================================================================
//                  NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT_SAFE_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(EXPR)
#define ASSERT_SAFE_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPR)
#define ASSERT_PASS(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS(EXPR)
#define ASSERT_FAIL(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL(EXPR)
#define ASSERT_OPT_PASS(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS(EXPR)
#define ASSERT_OPT_FAIL(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL(EXPR)

#define ASSERT_SAFE_PASS_RAW(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS_RAW(EXPR)
#define ASSERT_SAFE_FAIL_RAW(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL_RAW(EXPR)
#define ASSERT_PASS_RAW(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS_RAW(EXPR)
#define ASSERT_FAIL_RAW(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL_RAW(EXPR)
#define ASSERT_OPT_PASS_RAW(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(EXPR)
#define ASSERT_OPT_FAIL_RAW(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(EXPR)

// ============================================================================
//                           PRINTF FORMAT MACROS
// ----------------------------------------------------------------------------
#define ZU BSLS_BSLTESTUTIL_FORMAT_ZU

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

enum { VERBOSE_ARG_NUM = 2, VERY_VERBOSE_ARG_NUM, VERY_VERY_VERBOSE_ARG_NUM };

//=============================================================================
//                  GLOBAL HELPER CLASSES FOR TESTING
//-----------------------------------------------------------------------------

/// Allocator for testing equality operations.
class EqTestAllocator : public bslma::Allocator {

    // DATA
    int                 d_id;
    mutable const void *d_lastEqArg_p; // Argument from last eq

  protected:
    // PRIVATE ACCESSORS
    bool do_is_equal(const bsl::memory_resource& other) const
                                   BSLS_KEYWORD_NOEXCEPT BSLS_KEYWORD_OVERRIDE;

  public:
    // CREATORS
    explicit EqTestAllocator(int id) : d_id(id) { }

    // MANIPULATORS
    void *allocate(std::size_t bytes) BSLS_KEYWORD_OVERRIDE;
    void  deallocate(void        *p) BSLS_KEYWORD_OVERRIDE;
    void  reset() { d_lastEqArg_p = 0; }

    // ACCESSORS
    int id() const { return d_id; }
    const void *lastEqArg() const { return d_lastEqArg_p; }
};

void *EqTestAllocator::allocate(std::size_t)
{
    return 0;
}

void EqTestAllocator::deallocate(void *)
{
}

bool EqTestAllocator::do_is_equal(const bsl::memory_resource& other) const
                                                          BSLS_KEYWORD_NOEXCEPT
{
    d_lastEqArg_p = &other;

    const EqTestAllocator *pOther =
        dynamic_cast<const EqTestAllocator *>(&other);
    if (pOther) {
        return d_id == pOther->d_id;                                  // RETURN
    }

    return false;
}

/// An empty "allocator" class
struct NonAllocator {
};

/// An allocator-aware test type that tracks its construction and
/// destruction.
template <class ALLOCATOR>
class TestType {

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
    TestType(bsl::allocator_arg_t, const allocator_type &alloc,
             long a01 = 0, long a02 = 0, long a03 = 0, long a04 = 0,
             long a05 = 0, long a06 = 0, long a07 = 0, long a08 = 0,
             long a09 = 0, long a10 = 0, long a11 = 0, long a12 = 0,
             long a13 = 0, long a14 = 0);

    TestType(const TestType& original) : d_value(original.d_value) { }
    TestType(bsl::allocator_arg_t, const allocator_type &alloc,
             const TestType& original)
        : d_allocator(alloc), d_value(original.d_value) { }

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
TestType<ALLOCATOR>::TestType(bsl::allocator_arg_t,
                              const allocator_type &alloc,
                              long a01, long a02, long a03, long a04,
                              long a05, long a06, long a07, long a08,
                              long a09, long a10, long a11, long a12,
                              long a13, long a14)
    : d_allocator(alloc)
    , d_value(a01 + a02 + a03 + a04 + a05 + a06 + a07 + a08 + a09 + a10 +
              a11 + a12 + a13 + a14)
{
}

/// A non-trivial-sized object.
struct MyObject
{

    // DATA
    int  d_i;
    char d_s[10];
};

/// Class that overrides `operator&`, for testing the `address` method of
/// `bsl::allocator`.
struct HasOperatorAmpersand {

    HasOperatorAmpersand *operator&() { return 0;    }
    HasOperatorAmpersand *address()   { return this; }

    const HasOperatorAmpersand *operator&() const { return 0;    }
    const HasOperatorAmpersand *address()   const { return this; }
};

//=============================================================================
//                            USAGE EXAMPLE
//-----------------------------------------------------------------------------

///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: A fixed size array
///- - - - - - - - - - - - - - -
// We first show how to define a container type parameterized with an STL-style
// allocator template parameter.  To avoid issues concerning reallocation,
// dynamic growth, etc., we choose an array whose size is fixed at
// construction.  Our array will accept any STL-compatible allocator; we do not
// assume as scoped allocator, which would dictate that we pass the allocator
// through to the parameterized `T` contained type (see the `bslma_allocator`
// component and `bslma_constructionutil` package).
//
// We begin by defining member variables to hold the allocator, length, and
// allocated array:
// ```

    /// This class provides an array of (the template parameter) `TYPE` of
    /// fixed length as determined at construction time, using an instance
    /// of (the template parameter) `ALLOC` type to supply memory.
    template <class TYPE, class ALLOC>
    class my_FixedSizeArray {

        // DATA
        ALLOC  d_allocator;
        int    d_length;
        TYPE  *d_array;
// ```
// Then, we define the public interface:
// ```
      public:
        // TYPES
        typedef ALLOC allocator_type;
        typedef TYPE  value_type;

        // CREATORS

        /// Create a fixed-size array of the specified `length`, using the
        /// optionally specified `allocator` to supply memory.  If
        /// `allocator` is not specified, a default-constructed instance of
        /// the parameterized `ALLOC` type is used.  All the elements in the
        /// resulting array are default-constructed.
        explicit my_FixedSizeArray(int          length,
                                   const ALLOC& allocator = ALLOC());

        /// Create a copy of the specified `original` fixed-size array,
        /// using the optionally specified `allocator` to supply memory.  If
        /// `allocator` is not specified, a default-constructed instance of
        /// the parameterized `ALLOC` type is used.
        my_FixedSizeArray(const my_FixedSizeArray& original,
                          const ALLOC&             allocator = ALLOC());

        /// Destroy this fixed size array.
        ~my_FixedSizeArray();

        // MANIPULATORS

        /// Assign to this array the value of the specified `original`
        /// array.  Note that the length of this array might change.
        my_FixedSizeArray& operator=(const my_FixedSizeArray& original);

        /// Return a reference to the modifiable element at the specified
        /// `index` position in this fixed size array.  The behavior is
        /// undefined unless `index` is non-negative and less than
        /// `length()`.
        TYPE& operator[](int index) { return d_array[index]; }

        // ACCESSORS

        /// Return a reference to the modifiable element at the specified
        /// `index` position in this fixed size array.  The behavior is
        /// undefined unless `index` is non-negative and less than
        /// `length()`.
        const TYPE& operator[](int index) const { return d_array[index]; }

        /// Return the allocator used by this fixed size array to supply
        /// memory.
        allocator_type get_allocator() const { return d_allocator; }

        /// Return the length specified at construction of this fixed size
        /// array.
        int length() const { return d_length; }
    };

    // FREE OPERATORS

    /// Return `true` if the specified `lhs` fixed-size array has the same
    /// value as the specified `rhs` fixed-size array, and `false`
    /// otherwise.  Two fixed-size arrays have the same value if they have
    /// the same length and if the element at any index in `lhs` has the
    /// same value as the corresponding element at the same index in `rhs`.
    template<class TYPE, class ALLOC>
    bool operator==(const my_FixedSizeArray<TYPE, ALLOC>& lhs,
                    const my_FixedSizeArray<TYPE, ALLOC>& rhs);

    /// Return `true` if the specified `lhs` fixed-size array does not have
    /// the same value as the specified `rhs` fixed-size array, and `false`
    /// otherwise.  Two fixed-size arrays have the same value if they have
    /// the same length and if the element at any index in `lhs` has the
    /// same value as the corresponding element at the same index in `rhs`.
    template<class TYPE, class ALLOC>
    bool operator!=(const my_FixedSizeArray<TYPE, ALLOC>& lhs,
                    const my_FixedSizeArray<TYPE, ALLOC>& rhs);
// ```
// Next, we define the first constructor, which uses the allocator's `allocate`
// memory to obtain memory, then uses its `construct` method to construct each
// element.  To provide a uniform and future-proof interface, the standard way
// to call `allocate` and `construct` is indrectly though
// `bsl::allocator_traits`.  If `ALLOC` is a `bsl::allocator` object, then the
// `construct` method will attempt to pass the allocator to the constructed
// elements.  Note that exception safety has been sacrificed for simplicity of
// presentation; a production version of `my_FixedSizeArray` would need to
// unwind any constructed elements and the allocation if an exception were
// thrown.
// ```
//  #include <bslma_allocatortraits.h>

    // CREATORS
    template<class TYPE, class ALLOC>
    my_FixedSizeArray<TYPE, ALLOC>::my_FixedSizeArray(int          length,
                                                      const ALLOC& allocator)
        : d_allocator(allocator), d_length(length)
    {
        typedef bsl::allocator_traits<ALLOC> Traits;

        d_array = Traits::allocate(d_allocator, d_length);

        // Default construct each element of the array:
        for (int i = 0; i < d_length; ++i) {
            Traits::construct(d_allocator, &d_array[i], TYPE());
        }
    }
// ```
// Next, we define the copy constructor, which initializes the allocator member
// but defers the rest of the work to the assignment operator:
// ```
    template<class TYPE, class ALLOC>
    my_FixedSizeArray<TYPE, ALLOC>::my_FixedSizeArray(
                                            const my_FixedSizeArray& original,
                                            const ALLOC&             allocator)
        : d_allocator(allocator), d_length(0), d_array(0)
    {
        *this = original;
    }
// ```
// Now we define the assignment operator, which allocates the array and copies
// elements from the `rhs` array.  Note, again, that we simplified the code by
// omitting exception-safety constructs.
// ```
    template<class TYPE, class ALLOC>
    my_FixedSizeArray<TYPE, ALLOC>&
    my_FixedSizeArray<TYPE, ALLOC>::operator=(const my_FixedSizeArray& rhs)
    {
        typedef bsl::allocator_traits<ALLOC> Traits;

        if (this != &rhs) {
            // Call destructor for each old element
            for (int i = 0; i < d_length; ++i) {
                Traits::destroy(d_allocator, &d_array[i]);
            }

            // Deallocate old storage
            if (d_array) {
                Traits::deallocate(d_allocator, d_array, d_length);
            }

            // Set length and allocate new array.  Do not assign the allocator!
            d_length = rhs.d_length;
            d_array  = Traits::allocate(d_allocator, d_length);

            // Construct each element of the `lhs` array from the corresponding
            // `rhs` element.
            for (int i = 0; i < d_length; ++i) {
                Traits::construct(d_allocator, &d_array[i], rhs.d_array[i]);
            }
        }

        return *this;                                                 // RETURN
    }
// ```
// Next, we define the destructor, which uses the allocator's `destroy` method
// to destroy each element, then the allocator's `deallocate` method to return
// memory to the allocator:
// ```
    template<class TYPE, class ALLOC>
    my_FixedSizeArray<TYPE, ALLOC>::~my_FixedSizeArray()
    {
        typedef bsl::allocator_traits<ALLOC> Traits;

        // Call destructor for each element
        for (int i = 0; i < d_length; ++i) {
            Traits::destroy(d_allocator, &d_array[i]);
        }

        // Return memory to allocator.
        Traits::deallocate(d_allocator, d_array, d_length);
    }
// ```
// The equality and inequality operators simply compare the lengths and element
// values of the two arrays:
// ```
    // FREE OPERATORS
    template<class TYPE, class ALLOC>
    bool operator==(const my_FixedSizeArray<TYPE, ALLOC>& lhs,
                    const my_FixedSizeArray<TYPE, ALLOC>& rhs)
    {
        if (lhs.length() != rhs.length()) {
            return false;                                             // RETURN
        }
        for (int i = 0; i < lhs.length(); ++i) {
            if (lhs[i] != rhs[i]) {
                return false;                                         // RETURN
            }
        }
        return true;
    }

    template<class TYPE, class ALLOC>
    inline
    bool operator!=(const my_FixedSizeArray<TYPE, ALLOC>& lhs,
                    const my_FixedSizeArray<TYPE, ALLOC>& rhs) {
        return ! (lhs == rhs);
    }
// ```
// Now we can create array objects with different allocator mechanisms.  First
// we create an array, `a1`, using the default allocator and fill it with the
// values `1 .. 5`:
// ```
//  #include <bslma_bslallocator.h>
//  #include <bslma_testallocator.h>

    void usageExample1() {

        my_FixedSizeArray<int, bsl::allocator<int> > a1(5);
        ASSERT(5                                  == a1.length());
        ASSERT(bslma::Default::defaultAllocator() == a1.get_allocator());

        for (int i = 0; i < a1.length(); ++i) {
            a1[i] = i + 1;
        }
// ```
// Finally, we create a copy of `a1` using a test allocator.  The values of
// `a1` and `a2` are equal, even though they have different allocation
// mechanisms.  We verify that the test allocator was used to allocate the new
// array elements:
// ```
        bslma::TestAllocator                         testAlloc;
        my_FixedSizeArray<int, bsl::allocator<int> > a2(a1, &testAlloc);
        ASSERT(a1 == a2);
        ASSERT(a1.get_allocator() != a2.get_allocator());
        ASSERT(&testAlloc         == a2.get_allocator());
        ASSERT(1 == testAlloc.numBlocksInUse());
    }
// ```
//
///Example 2: Propagation of the Allocator to Elements
///- - - - - - - - - - - - - - - - - - - - - - - - - -
// In this example, we use the `FixedSizeArray` template defined in Example 1
// and demonstrate how `bsl::allocator` propagates itself to the elements it
// constructs, such that the container and its elements all use the same
// allocator.
//
// First, we create a representative element class, `MyType`, that allocates
// memory using the `bslma::Allocator` protocol:
// ```
// #include <bslma_allocator.h>
// #include <bslma_default.h>
// #include <bslma_usesbslmaallocator.h>

    class MyType {

        bslma::Allocator *d_allocator_p;
        // etc.

      public:
        // TRAITS
        BSLMF_NESTED_TRAIT_DECLARATION(MyType, bslma::UsesBslmaAllocator);

        // CREATORS
        explicit MyType(bslma::Allocator* basicAlloc = 0)
          : d_allocator_p(bslma::Default::allocator(basicAlloc)) { /* ... */ }
        MyType(const MyType&, bslma::Allocator* basicAlloc = 0)
          : d_allocator_p(bslma::Default::allocator(basicAlloc)) { /* ... */ }
        // etc.

        // ACCESSORS
        bslma::Allocator *allocator() const { return d_allocator_p; }

        // etc.
    };
// ```
// Now, we instantiate `my_FixedSizeArray` using `MyType` and verify that,
// when we provide the address of an allocator to the constructor of the
// container, the same address is passed to the constructor of the container's
// elements:
// ```
// #include <bslmf_issame.h>

    void usageExample2()
    {
        typedef my_FixedSizeArray<MyType, bsl::allocator<MyType> > ArrayType;

        const int            arrayLen = 7;

        bslma::TestAllocator testAlloc;
        ArrayType            C1(arrayLen, &testAlloc);
        ASSERT((bsl::is_same<ArrayType::allocator_type,
                             bsl::allocator<MyType> >::value));
        ASSERT(C1.get_allocator()    == bsl::allocator<MyType>(&testAlloc));
        for (int i = 0; i < arrayLen; ++i) {
            ASSERT(C1[i].allocator() == &testAlloc);
        }
// ```
// Next, we copy-construct the container and verify that the copy uses the
// default allocator, not the allocator from the original; moreover, we verify
// that the elements stored in the copy also use the default allocator.
// ```
        ArrayType C2(C1);
        ASSERT(C2.get_allocator() != C1.get_allocator());
        ASSERT(C2.get_allocator() == bsl::allocator<MyType>());
        for (int i = 0; i < arrayLen; ++i) {
            ASSERT(C2[i].allocator() != &testAlloc);
            ASSERT(C2[i].allocator() == bslma::Default::defaultAllocator());
        }
// ```
// Finally, we create a third array using the test allocator and use assignment
// to give it the same value as the second array.  We then verify that the
// assignment did not modify the allocator of the lhs array and that the
// elements of the resulting copy use the same allocator as the lhs array:
// ```
        bslma::TestAllocator testAlloc2;
        ArrayType            C3(1, &testAlloc2);
        ASSERT(1 == testAlloc2.numBlocksInUse());
        ASSERT(1 == C3.length());
        ASSERT(C3.get_allocator() == bsl::allocator<MyType>(&testAlloc2));
        ASSERT(C3[0].allocator() == &testAlloc2);

        C3 = C2;  // Assignment
        ASSERT(1        == testAlloc2.numBlocksInUse());
        ASSERT(arrayLen == C3.length());
        ASSERT(C3.get_allocator() == bsl::allocator<MyType>(&testAlloc2));
        for (int i = 0; i < arrayLen; ++i) {
            ASSERT(C3[i].allocator() == &testAlloc2);
        }
    }
// ```

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int                 test = argc > 1 ? atoi(argv[1]) : 0;
    bool             verbose = argc > 2;
    bool         veryVerbose = argc > 3;
    bool     veryVeryVerbose = argc > 4;
    bool veryVeryVeryVerbose = argc > 5;

    (void) veryVerbose;
    (void) veryVeryVerbose;

    bslma::TestAllocator globalAllocator("global", veryVeryVeryVerbose);
    bslma::Default::setGlobalAllocator(&globalAllocator);

    bslma::TestAllocator defaultAllocator("default", veryVeryVeryVerbose);
    ASSERT(0 == bslma::Default::setDefaultAllocator(&defaultAllocator));

    setbuf(stdout, NULL);    // Use unbuffered output

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:  // Zero is always the leading case.
      case 12: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //
        // Concerns:
        // 1. The usage example must compile end execute without errors.
        //
        // Plan:
        // 1. Copy-paste the usage example and replace `assert` by `ASSERT`.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) printf("\nUSAGE EXAMPLE"
                            "\n=============\n");

        bslma::TestAllocator ta("default for usage", veryVeryVeryVerbose);
        bslma::DefaultAllocatorGuard allocGuard(&ta);

        usageExample1();
        usageExample2();

      } break;
      case 11: {
        // --------------------------------------------------------------------
        // ASSIGNMENT
        //
        // Concerns:
        // 1. Assigning a `bsl::allocator` to one having the same mechanism has
        //    no effect.
        // 2. Self assignment has no effect.
        // 3. (Negative test) Assigning allocators having different mechanisms
        //    is disallowed at run time via an assertion failure.
        //
        // Plan:
        // 1. Assign allocators having the same mechanism and verify that
        //    nothing has changed.
        // 2. Assign an allocator to itself and verify that nothing has
        //    changed.
        // 3. (Build-mode dependent) Verify that trying to modify an allocator
        //    by assigning to it from one having a different mechanism triggers
        //    an assertion failure.
        //
        // Testing:
        //    allocator& operator=(const allocator& rhs);
        // --------------------------------------------------------------------

        if (verbose) printf("\nASSIGNMENT"
                            "\n==========\n");

        bslma::TestAllocator ta1, ta2;

        const bsl::allocator<> A1A(&ta1);
        bsl::allocator<>       a1b(&ta1); const bsl::allocator<>& A1B = a1b;
        bsl::allocator<>       a2(&ta2);

        a1b = A1A;  // No-op assignment
        ASSERT(&ta1 == A1A.mechanism());
        ASSERT(&ta1 == A1B.mechanism());

        a1b = A1B;  // Self assignment
        ASSERT(&ta1 == A1B.mechanism());

        // Negative test. Assertion should fire
        {
            bsls::AssertTestHandlerGuard g;
            ASSERT_OPT_FAIL(a2 = A1A);
        }

      } break;
      case 10: {
        // --------------------------------------------------------------------
        // `construct` AND `destroy`
        //
        // Concerns:
        // 1. Using `construct` to construct an object with 0 to 14 arguments
        //    passes those arguments to the object's constructor.
        // 2. If the object is allocator-aware and uses a compatible allocator,
        //    the allocator is passed to the object's constructor.
        // 3. Destroy invokes the object's destructor.
        //
        // Plan:
        // 1. Define a test type whose constructor takes 0 to 14 arguments.
        //    Construct an object of that type using the `construct` method and
        //    verify that the arguments were passed to its constructor.  (C-1)
        // 2. Define an allocator-aware test type whose constructor takes 0 to
        //   14. arguments.  Construct an object of that type using the
        //    `construct` method and verify that the arguments were passed to
        //    its constructor.
        // 3. Destroy the objects from steps 1 and 2 using the `destroy`
        //    method.  Verify that the objects' destructors were invoked.
        //
        // Testing:
        //
        // --------------------------------------------------------------------

        if (verbose) printf("\n'construct' AND `destroy`"
                            "\n=========================\n");

        typedef TestType<NonAllocator>         TT;
        typedef TestType<bsl::allocator<int> > AATT;

        bslma::TestAllocator ta("case 10", veryVeryVeryVerbose);
        bsl::allocator<>     alloc(&ta);

        bsls::ObjectBuffer<TT>   buf1;
        bsls::ObjectBuffer<AATT> buf2;

#define TEST(EXP, ...) do {                                                   \
        alloc.construct(&buf1.object(), __VA_ARGS__);                         \
        ASSERTV(EXP, buf1.object().value(), EXP == buf1.object().value());    \
        alloc.destroy(&buf1.object());                                        \
        ASSERTV(TT::lastDestroyed() == buf1.address());                       \
        alloc.construct(&buf2.object(), __VA_ARGS__);                         \
        ASSERT(alloc == buf2.object().get_allocator());                       \
        ASSERTV(EXP, buf2.object().value(), EXP == buf2.object().value());    \
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
            ASSERTV(0, buf2.object().value(), 0 == buf2.object().value());
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
        // `allocate` AND `deallocate`
        //
        // Concerns:
        // 1. For `bsl::allocator<T>`, calling `allocate` with argument `n`
        //    will allocate `n * sizeof(T)` bytes from the mechanism object.
        // 2. A `bsl::allocator<T2>` copied from a `bsl::allocator<T1>` will
        //    allocate blocks from the same mechanism, even though the blocks
        //    have different sizes.
        // 3. An optional `hint` pointer can be passed to `allocate` but will
        //    have no effect.
        // 4. Deallocating a block from a `bsl::allocator` returns it to the
        //    underlying mechanism object.
        //
        // Plan:
        // 1. Create a `bsl::allocator<int>` using a `bslma::TestAllocator` and
        //    call `allocate` with different values for `n`, with and without a
        //    hint pointer.  Verify that the blocks allocated from the test
        //    allocator have size `n * sizeof(int)`.  (C-1, C-3)
        // 2. Initialize a `bsl::allocator<char>` from the allocator in the
        //    previous step.  Allocate several blocks from the new allocator
        //    and verify that the blocks are correctly allocated from the same
        //    test allocator as was used in the previous step.
        // 3. Deallocate each of the blocks allocated in steps 1 and 2.  Verify
        //    that the memory is returned to the test allocator.
        //
        // Testing:
        //    pointer allocate(size_type n, const void *hint = 0);
        //    void deallocate(TYPE *p, std::size_t n = 1);
        // --------------------------------------------------------------------

        if (verbose) printf("\n'allocate' AND `deallocate`"
                            "\n===========================\n");

        bslma::TestAllocator ta("test 9", veryVeryVeryVerbose);

        // Step 1
        bsl::allocator<int> ia(&ta);

        int *p1 = ia.allocate(1);
        ASSERT(1               == ta.numBlocksInUse());
        ASSERT(1 * sizeof(int) == ta.numBytesInUse());

        int *p2 = ia.allocate(2, p1);  // Use hint
        ASSERT(2               == ta.numBlocksInUse());
        ASSERT(3 * sizeof(int) == ta.numBytesInUse());

        // Step 2
        bsl::allocator<char> ca(ia);
        ASSERT(&ta == ca.mechanism());

        char *p3 = ca.allocate(1);
        ASSERT(3                                  == ta.numBlocksInUse());
        ASSERT(1 * sizeof(char) + 3 * sizeof(int) == ta.numBytesInUse());

        char *p4 = ca.allocate(3, p3);  // Use hint
        ASSERT(4                                  == ta.numBlocksInUse());
        ASSERT(4 * sizeof(char) + 3 * sizeof(int) == ta.numBytesInUse());

        // Step 3
        ia.deallocate(p1);
        ASSERT(3                                  == ta.numBlocksInUse());
        ASSERT(4 * sizeof(char) + 2 * sizeof(int) == ta.numBytesInUse());
        ca.deallocate(p4);
        ASSERT(2                                  == ta.numBlocksInUse());
        ASSERT(1 * sizeof(char) + 2 * sizeof(int) == ta.numBytesInUse());
        ca.deallocate(p3);
        ASSERT(1               == ta.numBlocksInUse());
        ASSERT(2 * sizeof(int) == ta.numBytesInUse());
        ia.deallocate(p2);
        ASSERT(0 == ta.numBlocksInUse());
        ASSERT(0 == ta.numBytesInUse());

      } break;
      case 8: {
        // --------------------------------------------------------------------
        // `operator==` AND `operator!=`
        //
        // Concerns:
        // 1. Two `bsl::allocator` objects that contain the same
        //    `bslma::Allocator` pointer compare equal using `operator==`.
        //    The `bslma::Allocator::is_equal` member function is not
        //    invoked.
        // 2. Two `bsl::allocator` objects that contain different
        //    `bslma::Allocator` pointers compare equal if the `is_equal`
        //    member function of `bslma::Allocator` returns `true`.
        // 3. Two `bsl::allocator` objects that contain different
        //    `bslma::Allocator` pointers do not compare equal if the
        //    `is_equal` member function of `bslma::Allocator` returns `false`.
        // 4. Concerns 1-3 apply regardless of whether the two `bsl::allocator`
        //    objects have the same `value_type`.
        // 5. A `bsl::allocator` can be compared for equality
        //    against a pointer to `bslma::Allocator`; the result is the
        //    same as comparing it against a `bsl::allocator` that
        //    was initialized with the `bslma::Allocator` address.
        // 6. In all cases, `operator!=` returns the inverse of `operator==`.
        // 7. Every comparison is commutative; the return value is the same
        //    regardless of which operand is on the left.
        //
        // Plan:
        // 1. Define a class derived from `bslma::Allocator` having a
        //    constructor-supplied integer ID.  The `do_is_equal` method for
        //    this class will return true *iff* the IDs compare equal.  In
        //    addition, the `do_is_equal` method will remember the argument
        //    argument for its most resent invocation.  For this test, the
        //    `do_allocate` and `do_deallocate` methods are not called and can
        //    simply return null.  Create three instances of this class, two
        //    having the same ID and a third having a different ID.
        // 2. Perform the remaining steps in a loop, each iteration using a
        //    different combination of two of resources from step 1, `r1` and
        //    `r2` (where `r1` and `r2` might be the same object).  Create two
        //    objects of `bsl::allocator`, `pa1` and `pa2`,
        //    initialized from the addresses of `r1` and `r2`, respecively.
        // 3. Verify that, when `r1` and `r2` are the same object, that 'pa1 ==
        //    pa2' is true and that no calls are made to `do_is_equal`.  As a
        //    special case, test self-equality is always true and that no calls
        //    are made to `do_is_equal` when comparing an object to itself.
        //    (C-1)
        // 4. Verify that, when `r1` and `r2` are different objects with the
        //    same ID, that `pa1 == pa2` is true and that one call is made to
        //    `r1.do_is_equal(&r2)`.  (C-2)
        // 5. Verify that, when `r1` and `r2` have different IDs, that 'pa1 ==
        //    pa2' is false and that one call is made to
        //    `r1.do_is_equal(&r2)`.  (C-3)
        // 6. Ensure that `pa1` and `pa2` are have different value type
        //    arguments in step 2.  The self-equality test in step 3 ensures
        //    that `operator==` and `operator!=` work with the same value type
        //    whereas the remaining tests ensure that they work with different
        //    value types.  (C-4)
        // 7. Verify that `pa1 == &r2` and that `&r1 == pa2` have the same
        //    result as `pa1 == pa2`, including whether or not `do_is_equal` is
        //    called.  (C-5)
        // 8. In each of the steps testing `==`, verify that `!=` returns the
        //    inverse.  (C-6)
        // 9. For each step involving `==` and `!=`, repeat the step with the
        //    arguments reversed.  (C-7)
        //
        // Testing:
        //    bool operator==(bsl::allocator<T>,  bsl::allocator<T>);
        //    bool operator==(bsl::allocator<T1>, bsl::allocator<T2>);
        //    bool operator==(bslma::Allocator *, bsl::allocator<T>);
        //    bool operator==(bsl::allocator<T>,  bslma::Allocator*);
        //    bool operator!=(bsl::allocator<T>,  bsl::allocator<T>);
        //    bool operator!=(bsl::allocator<T1>, bsl::allocator<T2>);
        //    bool operator!=(bslma::Allocator *, bsl::allocator<T>);
        //    bool operator!=(bsl::allocator<T>,  bslma::Allocator*);
        // --------------------------------------------------------------------

        if (verbose) printf("\n'operator==` AND `operator!='"
                            "\n=============================\n");

        // Create resources
        EqTestAllocator etrA(0);
        EqTestAllocator etrB(0);
        EqTestAllocator etrC(1);

        // Loop over combinations of mechanisms
        EqTestAllocator *const mechanisms[] = { &etrA, &etrB, &etrC };
        const int numMechanisms = sizeof(mechanisms) / sizeof(mechanisms[0]);

        for (int i = 0; i < numMechanisms; ++i) {
            for (int j = i; j < numMechanisms; ++j) {  // `j` can equal `i`
                EqTestAllocator& r1(*mechanisms[i]);
                EqTestAllocator& r2(*mechanisms[j]);
                r1.reset();
                r2.reset();

                // `allocator`s with different value types
                bsl::allocator<char> pa1(&r1);
                bsl::allocator<int>  pa2(&r2);

                // self-equality test
                ASSERTV(i, j,    pa1 == pa1);
                ASSERTV(i, j, ! (pa1 != pa1));
                ASSERTV(i, j, 0 == r1.lastEqArg());
                ASSERTV(i, j, 0 == r2.lastEqArg());
                r1.reset();

                // Determine expected results
                const bool  expEq        = r1.id() == r2.id();
                const bool  expNe        = r1.id() != r2.id();
                const void *expLastEqArg = (&r1 != &r2) ? &r2 : 0;

                // Compare `allocator` objects
                ASSERTV(i, j, expEq        == (pa1 == pa2));
                ASSERTV(i, j, expLastEqArg == r1.lastEqArg());
                ASSERTV(i, j, 0            == r2.lastEqArg());
                r1.reset();
                ASSERTV(i, j, expNe        == (pa1 != pa2));
                ASSERTV(i, j, expLastEqArg == r1.lastEqArg());
                ASSERTV(i, j, 0            == r2.lastEqArg());
                r1.reset();

                // Compare `allocator` against `bslma::Allocator *`
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

      } break;
      case 7: {
        // --------------------------------------------------------------------
        // `bslma::IsStdAllocator`
        //
        // Concerns:
        // 1. The trait `bslma::IsStdAllocator<bsl::allocator<E>>` inherits
        //    from `bsl::true_type` for any complete type `E`.
        // 2. The trait `bslma::IsStdAllocator<bsl::allocator<void>>` inherits
        //    from `bsl::false_type`.
        //
        // Plan:
        // 1. For various complete types, `E`, create an instance of
        //    `bslma::IsStdAllocator<bsl::allocator<E>>` and verify that the
        //    address of that instance is convertible to `bsl::true_type *`.
        //    (C-1)
        // 2. Create an instance of
        //    `bslma::IsStdAllocator<bsl::allocator<void>>` and verify that the
        //    address of that instance is convertible to `bsl::false_type *`.
        //    (C-2)
        //
        // Testing:
        //   bslma::IsStdAllocator<bsl::allocator<E>>
        // --------------------------------------------------------------------

        if (verbose) printf("\nUSAGE EXAMPLE"
                            "\n=============\n");

        bslma::IsStdAllocator<bsl::allocator<int>   > xI;
        bslma::IsStdAllocator<bsl::allocator<float> > xF;
        bslma::IsStdAllocator<bsl::allocator<long>  > xL;
        bslma::IsStdAllocator<bsl::allocator<void>  > xV;

        ASSERT( xI.value);
        ASSERT( xF.value);
        ASSERT( xL.value);
        ASSERT(!xV.value);

        // Pointer conversion will fail unless type on right is derived from
        // type on left.
        bsl::true_type  *pI = &xI;
        bsl::true_type  *pF = &xF;
        bsl::true_type  *pL = &xL;
        bsl::false_type *pV = &xV;

        (void) pI;
        (void) pF;
        (void) pL;
        (void) pV;

      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING ALLOCATOR_TRAITS
        //
        // Concerns:
        // 1. that the specialization of `bsl::allocator_traits` for
        //    `bsl::allocator` produces the correct member typedefs.
        //
        // Plan:
        // 1. The testing is straightforward and follows the concerns.
        //
        // Testing:
        //   bsl::allocator_traits<bsl::allocator<E>>
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING ALLOCATOR_TRAITS"
                            "\n========================\n");

        typedef bsl::allocator<int>   AI;
        typedef bsl::allocator<float> AF;
        typedef bsl::allocator<void>  AV;
        typedef bsl::allocator<long>  AL;

        bslma::TestAllocator ta;
        AI a_def;  // Default
        AI ai(&ta);
        AF af(&ta);
        AV av(&ta);
        AL al(&ta);

        typedef bsl::allocator_traits<AI> TI;
        typedef bsl::allocator_traits<AF> TF;
        typedef bsl::allocator_traits<AV> TV;
        typedef bsl::allocator_traits<AL> TL;


        if (verbose) printf("\tTesting `allocator_type`.\n");
        {
            ASSERT((bsl::is_same<TI::allocator_type, AI>::value));
            ASSERT((bsl::is_same<TF::allocator_type, AF>::value));
            ASSERT((bsl::is_same<TV::allocator_type, AV>::value));
        }


        if (verbose) printf("\tTesting `value_type`.\n");
        {
            ASSERT((bsl::is_same<TI::value_type, int>::value));
            ASSERT((bsl::is_same<TI::value_type, AI::value_type>::value));

            ASSERT((bsl::is_same<TF::value_type, float>::value));
            ASSERT((bsl::is_same<TF::value_type, AF::value_type>::value));

            ASSERT((bsl::is_same<TV::value_type, void>::value));
            ASSERT((bsl::is_same<TV::value_type, AV::value_type>::value));

            ASSERT((bsl::is_same<TL::value_type, long>::value));
            ASSERT((bsl::is_same<TL::value_type, AL::value_type>::value));
        }

        if (verbose) printf("\tTesting `pointer`.\n");
        {
            ASSERT((bsl::is_same<TI::pointer, AI::pointer>::value));
            ASSERT((bsl::is_same<TF::pointer, AF::pointer>::value));
            ASSERT((bsl::is_same<TV::pointer, AV::pointer>::value));
        }

        if (verbose) printf("\tTesting `const_pointer`.\n");
        {
            ASSERT((bsl::is_same<TI::const_pointer,
                                 AI::const_pointer>::value));
            ASSERT((bsl::is_same<TF::const_pointer,
                                 AF::const_pointer>::value));
            ASSERT((bsl::is_same<TV::const_pointer,
                                 AV::const_pointer>::value));
        }

        if (verbose) printf("\tTesting `void_pointer`.\n");
        {
            ASSERT((bsl::is_same<TI::void_pointer, AV::pointer>::value));
            ASSERT((bsl::is_same<TF::void_pointer, AV::pointer>::value));
            ASSERT((bsl::is_same<TV::void_pointer, AV::pointer>::value));
        }

        if (verbose) printf("\tTesting `const_void_pointer`.\n");
        {
            ASSERT((bsl::is_same<TI::const_void_pointer,
                                 AV::const_pointer>::value));
            ASSERT((bsl::is_same<TF::const_void_pointer,
                                 AV::const_pointer>::value));
            ASSERT((bsl::is_same<TV::const_void_pointer,
                                 AV::const_pointer>::value));
        }

        if (verbose) printf("\tTesting `difference_type`.\n");
        {
            ASSERT((bsl::is_same<TI::difference_type,
                                 AI::difference_type>::value));
            ASSERT((bsl::is_same<TF::difference_type,
                                 AF::difference_type>::value));
            ASSERT((bsl::is_same<TV::difference_type,
                                 AV::difference_type>::value));
        }

        if (verbose) printf("\tTesting `size_type`.\n");
        {
            ASSERT((bsl::is_same<TI::size_type, AI::size_type>::value));
            ASSERT((bsl::is_same<TF::size_type, AF::size_type>::value));
            ASSERT((bsl::is_same<TV::size_type, AV::size_type>::value));
        }

        if (verbose) printf("\tTesting `rebind_alloc`.\n");
        {
#ifdef BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES
            ASSERT((bsl::is_same<TI::rebind_alloc<long>,
                                 AI::rebind<long>::other>::value));
            ASSERT((bsl::is_same<TF::rebind_alloc<long>,
                                 AF::rebind<long>::other>::value));
            ASSERT((bsl::is_same<TV::rebind_alloc<long>,
                                 AV::rebind<long>::other>::value));
#else
            ASSERT((bsl::is_convertible<TI::rebind_alloc<long>,
                                        AI::rebind<long>::other>::value));
            ASSERT((bsl::is_convertible<TF::rebind_alloc<long>,
                                        AF::rebind<long>::other>::value));
            ASSERT((bsl::is_convertible<TV::rebind_alloc<long>,
                                        AV::rebind<long>::other>::value));
#endif
        }

        if (verbose) printf("\tTesting `rebind_traits`.\n");
        {
#ifdef BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES
            ASSERT((bsl::is_same<TI::rebind_traits<long>,TL>::value));
            ASSERT((bsl::is_same<TF::rebind_traits<long>,TL>::value));
            ASSERT((bsl::is_same<TV::rebind_traits<long>,TL>::value));
#else
            ASSERT((bsl::is_convertible<TI::rebind_traits<long>,TL>::value));
            ASSERT((bsl::is_same<TI::rebind_traits<long>::allocator_type,
                                 AL>::value));

            ASSERT((bsl::is_convertible<TF::rebind_traits<long>,TL>::value));
            ASSERT((bsl::is_same<TF::rebind_traits<long>::allocator_type,
                                 AL>::value));

            ASSERT((bsl::is_convertible<TV::rebind_traits<long>,TL>::value));
            ASSERT((bsl::is_same<TV::rebind_traits<long>::allocator_type,
                                 AL>::value));
#endif
        }

        if (verbose) printf("\tTesting `max_size()`.\n");
        {
            ASSERT(TI::max_size(ai) == ai.max_size());
            ASSERT(TF::max_size(ai) == af.max_size());
            ASSERT(TL::max_size(ai) == al.max_size());
        }

        if (verbose)
            printf("\tTesting `select_on_container_copy_construction()`.\n");
        {
            ASSERT(a_def == TI::select_on_container_copy_construction(ai));
            ASSERT(a_def == TF::select_on_container_copy_construction(af));
            ASSERT(a_def == TL::select_on_container_copy_construction(al));
            ASSERT(a_def == TV::select_on_container_copy_construction(av));
        }

        if (verbose) printf("\tTesting boolean traits.\n");
        {
            ASSERT(! TI::is_always_equal::value);
            ASSERT(! TF::is_always_equal::value);
            ASSERT(! TL::is_always_equal::value);
            ASSERT(! TV::is_always_equal::value);

            ASSERT(! TI::propagate_on_container_copy_assignment::value);
            ASSERT(! TF::propagate_on_container_copy_assignment::value);
            ASSERT(! TL::propagate_on_container_copy_assignment::value);
            ASSERT(! TV::propagate_on_container_copy_assignment::value);

            ASSERT(! TI::propagate_on_container_move_assignment::value);
            ASSERT(! TF::propagate_on_container_move_assignment::value);
            ASSERT(! TL::propagate_on_container_move_assignment::value);
            ASSERT(! TV::propagate_on_container_move_assignment::value);

            ASSERT(! TI::propagate_on_container_swap::value);
            ASSERT(! TF::propagate_on_container_swap::value);
            ASSERT(! TL::propagate_on_container_swap::value);
            ASSERT(! TV::propagate_on_container_swap::value);
        }
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING NESTED TYPES
        //
        // Concerns:
        // 1. that `size_type` is unsigned while `difference_type` is signed.
        // 2. that size_type and difference_type are the right size (i.e., they
        //    can represent any difference of pointers in the memory model)
        // 3. that all other types exist and are as specified by the C++
        //    standard
        // 4. that if Y is X::rebind<U>::other, then Y::rebind<T>::other is
        //     the same type as X
        //
        // Plan:
        // 1. The testing is straightforward and follows the concerns.
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

        typedef bsl::allocator<int>   AI;
        typedef bsl::allocator<float> AF;
        typedef bsl::allocator<void>  AV;

        if (verbose) printf("\tTesting `size_type`.\n");
        {
            ASSERT(sizeof(AI::size_type) == sizeof(int*));
            ASSERT(sizeof(AV::size_type) == sizeof(void*));

            ASSERT(0 < ~(AI::size_type)0);
            ASSERT(0 < ~(AV::size_type)0);
        }

        if (verbose) printf("\tTesting `difference_type`.\n");
        {
            ASSERT(sizeof(AI::difference_type) == sizeof(int*));
            ASSERT(sizeof(AV::difference_type) == sizeof(void*));

            ASSERT(0 > ~(AI::difference_type)0);
            ASSERT(0 > ~(AV::difference_type)0);
        }

        if (verbose) printf("\tTesting `pointer`.\n");
        {
            ASSERT((bsl::is_same<AI::pointer, int*>::value));
            ASSERT((bsl::is_same<AF::pointer, float*>::value));
            ASSERT((bsl::is_same<AV::pointer, void*>::value));
        }

        if (verbose) printf("\tTesting `const_pointer`.\n");
        {
            ASSERT((bsl::is_same<AI::const_pointer, const int*>::value));
            ASSERT((bsl::is_same<AF::const_pointer, const float*>::value));
            ASSERT((bsl::is_same<AV::const_pointer, const void*>::value));
        }

        if (verbose) printf("\tTesting `reference`.\n");
        {
            ASSERT((bsl::is_same<AI::reference, int&>::value));
            ASSERT((bsl::is_same<AF::reference, float&>::value));
        }

        if (verbose) printf("\tTesting `const_reference`.\n");
        {
            ASSERT((bsl::is_same<AI::const_reference, const int&>::value));
            ASSERT((bsl::is_same<AF::const_reference, const float&>::value));
        }

        if (verbose) printf("\tTesting `value_type`.\n");
        {
            ASSERT((bsl::is_same<AI::value_type, int>::value));
            ASSERT((bsl::is_same<AF::value_type, float>::value));
            ASSERT((bsl::is_same<AV::value_type, void>::value));
        }

        if (verbose) printf("\tTesting `rebind`.\n");
        {
            ASSERT((bsl::is_same<AI::rebind<int  >::other, AI>::value));
            ASSERT((bsl::is_same<AI::rebind<float>::other, AF>::value));
            ASSERT((bsl::is_same<AI::rebind<void >::other, AV>::value));
            ASSERT((bsl::is_same<AF::rebind<int  >::other, AI>::value));
            ASSERT((bsl::is_same<AF::rebind<float>::other, AF>::value));
            ASSERT((bsl::is_same<AF::rebind<void >::other, AV>::value));
            ASSERT((bsl::is_same<AV::rebind<int  >::other, AI>::value));
            ASSERT((bsl::is_same<AV::rebind<float>::other, AF>::value));
            ASSERT((bsl::is_same<AV::rebind<void >::other, AV>::value));
        }

      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING ACCESSORS
        //
        // Concerns:
        // 1. that the correct `bslma::Allocator*` is returned by `mechanism`.
        // 2. that the result of `max_size` fits and represents the maximum
        //    possible number of bytes in a `bslma::Allocator::size_type`.
        // 3. that `address` returns the address of its argument, ignoring any
        //    `operator&` member of the argument type.
        //
        // Plan:
        // 1. The concerns are straightforward to test.
        //
        // Testing:
        //   pointer address(reference x) const;
        //   const_pointer address(const_reference x) const;
        //   bslma::Allocator *mechanism() const;
        //   size_type max_size() const;
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING ACCESSORS"
                            "\n=================\n");

        bslma::Allocator *dflt = bslma::Default::allocator();

        if (verbose) printf("\tTesting `address()`.\n");
        {
            bsl::allocator<int> ai;
            int                 i  = 0; const int& I = i;
            int                *pi = ai.address(i);
            const int          *pI = ai.address(I);
            ASSERT(&i == pi);
            ASSERT(&I == pI);

            bsl::allocator<HasOperatorAmpersand> ah;
            HasOperatorAmpersand                 mHoa;
            const HasOperatorAmpersand&          HOA   = mHoa;

            // `HasOperatorAmpersand::operator&` returns a null pointer whereas
            // `HasOperatorAmpersand::address()` returns the real address.
            ASSERT(0 == &mHoa);
            ASSERT(0 == &HOA);
            ASSERT(0 != mHoa.address());
            ASSERT(0 != HOA.address());

            // `bsl::allocator::address(x)` returns the real address of `x`,
            // ignoring the presence of `x.operator&()`.
            HasOperatorAmpersand       *ph = ah.address(mHoa);
            const HasOperatorAmpersand *pH = ah.address(HOA);
            ASSERT(mHoa.address() == ph);
            ASSERT(HOA.address()  == pH);
        }

        if (verbose) printf("\tTesting `mechanism()`.\n");
        {
            bslma::TestAllocator ta(veryVeryVeryVerbose);

            bsl::allocator<int>  ai1;       ASSERT(dflt == ai1.mechanism());
            bsl::allocator<int>  ai2(&ta);  ASSERT(&ta  == ai2.mechanism());
            bsl::allocator<int>  ai4(0);    ASSERT(dflt == ai4.mechanism());

            bsl::allocator<void> av1;       ASSERT(dflt == av1.mechanism());
            bsl::allocator<void> av2(&ta);  ASSERT(&ta  == av2.mechanism());
            bsl::allocator<void> av4(0);    ASSERT(dflt == av4.mechanism());

            bsl::allocator<int>  ai5(av2);  ASSERT(&ta  == ai5.mechanism());
            bsl::allocator<void> av5(ai2);  ASSERT(&ta  == av5.mechanism());
        }

        if (verbose) printf("\tTesting `max_size()`.\n");
        {
            typedef bslma::Allocator::size_type bsize;

            bsl::allocator<> charAlloc;
            bsize cas = charAlloc.max_size();

            // verify that max_size() is the largest positive integer of type
            // size_type
            ASSERTV(cas, cas > 0);
            ASSERTV(cas, cas == std::numeric_limits<bsize>::max());

            if (verbose) {
                printf("cas = " ZU "\n", cas);
            }

            bsl::allocator<MyObject> objAlloc;

            // Detect problem with MSVC in 64-bit mode, which can't do 64-bit
            // int arithmetic correctly for enums.
            ASSERT(objAlloc.max_size() < charAlloc.max_size());

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

      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING CONSTRUCTORS AND DESTRUCTOR
        //
        // Concerns:
        // 1. that an allocator can be constructed from the various
        //    constructors and that it uses the correct mechanism object.
        // 2. that an allocator can be constructed from an allocator to a
        //    different type
        // 3. that the destructor does not destroy its mechanism
        //
        // Plan:
        // 1. We construct a number of allocators from various mechanisms, and
        //    test that they do compare equal to the selected mechanism.  Copy
        //    constructed allocators have to compare equal to their original
        //    values.  (C-1, C-2)
        // 2. Construct a `bsl::allocator` from a test allocator.  Allocate a
        //    byte from the test allocator, then let the `bsl::allocator`
        //    object go out of scope.  Verify that the test allocator did not
        //    detect a leak (i.e., its destructor was not called with
        //    outstanding memory blocks.  (C-3)
        //
        // Testing:
        //   allocator();
        //   allocator(bslma::Allocator *);
        //   allocator(const allocator&);
        //   allocator(const allocator<U>&);
        //   ~allocator()
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING CONSTRUCTORS AND DESTRUCTOR"
                            "\n===================================\n");

        bslma::Allocator *dflt = bslma::Default::allocator();

        bslma::TestAllocator ta("test case 3", veryVeryVeryVerbose);

        bsl::allocator<int>  ai1;        ASSERT(dflt == ai1);
        bsl::allocator<int>  ai2(&ta);   ASSERT(&ta  == ai2);
        bsl::allocator<int>  ai3(ai2);   ASSERT(&ta  == ai3);
        bsl::allocator<int>  ai4(0);     ASSERT(dflt == ai4);

        bsl::allocator<void> av1;        ASSERT(dflt == av1);
        bsl::allocator<void> av2(&ta);   ASSERT(&ta  == av2);
        bsl::allocator<void> av3(av2);   ASSERT(&ta  == av3);
        bsl::allocator<void> av4(0);     ASSERT(dflt == av4);

        bsl::allocator<int>  ai5(av2);   ASSERT(av2  == ai5);
        bsl::allocator<void> av5(ai2);   ASSERT(ai2  == av5);

        // Test that destructor does not destroy the mechanism object
        void *block;
        {
            bsl::allocator<int> ai(&ta);
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
        // 1. That an allocator has the proper traits defined.
        //
        // Plan:
        // 1. Since it does not matter what type `bsl::allocator` is
        //    instantiated with, use `int` and test for each expected trait.
        //    Note that `void` also needs to be tested since it is a
        //    specialization.
        //
        // Testing:
        //   bslmf::IsBitwiseEqualityComparable<sl::allocator>
        //   bslmf::IsBitwiseCopyable<bsl::allocator>
        //   bslmf::IsBitwiseMoveable<bsl::allocator>
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING TRAITS"
                            "\n==============\n");

        typedef bsl::allocator<int>  IntAlloc;
        typedef bsl::allocator<void> VoidAlloc;

        ASSERT((bslmf::IsBitwiseCopyable<IntAlloc>::value));
        ASSERT((bslmf::IsBitwiseMoveable<IntAlloc>::value));
        ASSERT((bslmf::IsBitwiseEqualityComparable<IntAlloc>::value));
        (void) bslmf::IsTriviallyCopyableCheck<IntAlloc>::value;

        ASSERT((bslmf::IsBitwiseCopyable<VoidAlloc>::value));
        ASSERT((bslmf::IsBitwiseMoveable<VoidAlloc>::value));
        ASSERT((bslmf::IsBitwiseEqualityComparable<VoidAlloc>::value));
        (void) bslmf::IsTriviallyCopyableCheck<VoidAlloc>::value;

      } break;

      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //    This test case exercises the component but *tests* nothing.
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) printf("\nBREATHING TEST"
                            "\n==============\n");

        bslma::TestAllocator ta("breathing test A", veryVeryVeryVerbose);
        bslma::TestAllocator tb("breathing test B", veryVeryVeryVerbose);

        my_FixedSizeArray<int, bsl::allocator<int> > a1(5, &ta);

        ASSERT(5 == a1.length());
//        ASSERT(bslma::Default::defaultAllocator() == a1.allocator());
        ASSERT(&ta == a1.get_allocator());

        for (int i = 0; i < a1.length(); ++i) {
            a1[i] = i + 1;
        }

        bslma::TestAllocator testAlloc;
        my_FixedSizeArray<int, bsl::allocator<int> > a2(a1, &testAlloc);

        ASSERT(a1 == a2);
        ASSERT(a1.get_allocator() != a2.get_allocator());
        ASSERT(&testAlloc == a2.get_allocator());
        ASSERT(1 == testAlloc.numBlocksInUse());

        bsl::allocator<int> aiA(&ta);
        bsl::allocator<int> aiB(&tb);

        // Test that this will compile:

        bsl::allocator<void> voidAlloc(&testAlloc);

      } break;
      default: {
        fprintf(stderr, "WARNING: CASE `%d' NOT FOUND.\n", test);
        testStatus = -1;
      }
    }

    // CONCERN: In no case does memory come from the default allocator.
    ASSERTV(defaultAllocator.numBlocksTotal(),
            0 == defaultAllocator.numBlocksTotal());

    // CONCERN: In no case does memory come from the global allocator.
    ASSERTV(globalAllocator.numBlocksTotal(),
            0 == globalAllocator.numBlocksTotal());

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
