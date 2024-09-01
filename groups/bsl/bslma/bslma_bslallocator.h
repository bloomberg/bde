// bslma_bslallocator.h                                               -*-C++-*-
#ifndef INCLUDED_BSLMA_BSLALLOCATOR
#define INCLUDED_BSLMA_BSLALLOCATOR

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide an STL-compatible proxy for `bslma::Allocator` objects.
//
//@CLASSES:
//  bsl::allocator: STL-compatible allocator template
//  bsl::allocator_traits<bsl::allocator>: specialization for `bsl::allocator`
//
//@CANONICAL_HEADER: bsl_memory.h
//
//@SEE_ALSO: bslma_allocator
//
//@DESCRIPTION: This component provides an STL-compatible proxy for any
// allocator class derived from `bslma::Allocator`.  The proxy class,
// `bsl::allocator` is a template that adheres to the allocator requirements
// defined in section 20.5.3.5 [allocator.requirements] of the C++17 standard.
// `bsl::allocator` may be used to instantiate any class template that is
// parameterized by a standard allocator.  The container is expected to
// allocate memory for its own use through the allocator.  The `bsl::allocator`
// object holds a pointer to an object (the allocation *mechanism*) of class
// type derived from `bslma::Allocator`.  Different mechanism types allocate
// memory in different ways or from different pools, so this approach gives the
// programmer run time control over how the container obtains memory.
//
// The `bsl::allocator` template is intended to solve a problem created by the
// C++ standard allocator protocol.  In STL, the allocator type is specified at
// compile time as a container template parameter, so the allocation mechanism
// becomes an explicit part of the resulting container type.  Two containers
// cannot have the same type unless they are instantiated with the same
// allocator type.  The `bsl::allocator` template breaks the connection between
// the *compile-time* allocator type and the *run-time* allocation mechanism.
// The allocation mechanism is chosen at run-time by *initializing* (contrast
// with *instantiating*) the `bsl::allocator` with a pointer to a *mechanism*
// *object* derived from `bslma::Allocator`.  Each class derived from
// `bslma::Allocator` implements a specific allocation mechanism and is thus
// called a *mechanism* *class* within this component.  The `bsl::allocator`
// object forwards calls made through the standard allocator interface to the
// mechanism object with which it was initialized.  In this way, two containers
// instantiated with `bsl::allocator` can use different allocation mechanisms
// even though they have the same compile-time type.  The default mechanism
// object, if none is supplied to the `bsl::allocator` constructor, is
// `bslma::Default::defaultAllocator()`.
//
// A container constructs its elements by calling the `construct` method on its
// allocator.  Importantly, `bsl::allocator` is a *scoped* *allocator* -- when
// its `construct` method is called, the allocator passes itself to the
// constructor of the object being constructed (if that object is allocator
// aware (AA) and uses a compatible allocator type).  Thus, a container
// instantiated with a scoped allocator ensures that its elements use the same
// allocator as the container itself.  The `bsl::allocator::construct` method
// will propagate the allocator not only to element types that use
// `bsl::allocator`, but also to any types that use `bslma::Allocator *` --
// i.e., all type for which the `bslma::UsesBslmaAllocator` trait is true.
//
// A container using `bsl::allocator` should not copy its allocator on
// assignment and thus assignment of `bsl::allocator` objects is almost always
// incorrect.  Its base class, `bsl::polymorphic_allocator`, is not assignable,
// in fact but, for compatibility with some existing code, assignment of
// `bsl::allocator` must compile, but it is a precondition violation if the
// allocators being assigned are not already equal at run time (i.e., when
// assignment is a a no-op).  The assignment operator is deprecated and might
// be removed in the future, once all existing uses have been excised.
//
// Instantiations of `bsl::allocator` have reference semantics.  A
// `bsl::allocator` object does not "own" the `bslma::Allocator` with which it
// is initialized; copying a `bsl::allocator` object does not copy its
// mechanism object and destroying a `bsl::allocator` does not destroy its
// mechanism object.  Two `bsl::allocator` objects compare equal if and only if
// the mechanism objects they refer to compare equal.
//
///Relationship to `bsl::polymorphic_allocator`
///--------------------------------------------
// The `bsl::allocator` class template was the inspiration for the C++17
// `std::pmr::polymorphic_allocator` class template (section 23.12.3,
// [mem.poly.allocator.class] in the C++17 Standard) and `bslma::Allocator` was
// the inspiration for the C++17 `std::pmr::memory_resource` (section 23.12.2,
// mem.res.class] in the C++17 Standard).  For compatibility with the C++17
// standard, `bsl::allocator` is derived from `bsl::polymorphic_allocator`
// which, when using a C++17 library, is identical to
// `std::pmr::polymorphic_allocator`.  Similarly, `bslma::Allocator` is derived
// from `bsl::memory_resource`, which is identical to
// `std::pmr::memory_resource`.  These inheritance relationships ensure that a
// `bsl::allocator` instance can be passed to any type that is instantiated
// with a `std::pmr::polymorphic_allocator`, including `pmr` containers from
// the platform library.  Similarly, a pointer to `bslma::Allocator` is
// implicitly convertible to both `std::pmr::memory_resource *` and
// `std::pmr::polymorphic_allocator`.
//
///C++03 Restrictions on Allocator Usage
///--------------------------------------
// The allocator requirements section of the C++03 standard (section 20.1.5
// [lib.allocator.requirements]) permits containers to assume that two
// allocators of the same type always compare equal, effectively limiting C++03
// to stateless allocators.  This assumption is incorrect for instantiations of
// `bsl::allocator`.  Therefore, for a container (or other facility) to use
// `bsl::allocator`, it must operate correctly in the presence of non-equal
// `bsl::allocator` objects.  In practice, this means that a container cannot
// transfer ownership of allocated memory to another container unless the two
// containers use equal allocators.  Older third-party templates that assume
// stateless allocators might not work correctly when instantiated with
// `bsl::allocator`.
//
///Thread Safety
///-------------
// Because it is immutable, non-assignable, and has reference semantics, a
// single `bsl::allocator` object is safe for concurrent access by multiple
// threads if and only if the `bslma::Allocator` it references is safe for
// concurrent access from multiple threads.  Separate objects of
// `bsl::allocator` type may safely be used in separate threads if and only if
// the `bslma::Allocator` objects they reference are, themselves, safe for
// concurrent access.
//
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
// template <class TYPE, class ALLOC>
// class my_FixedSizeArray {
//     // This class provides an array of (the template parameter) 'TYPE' of
//     // fixed length as determined at construction time, using an instance
//     // of (the template parameter) 'ALLOC' type to supply memory.
//
//     // DATA
//     ALLOC  d_allocator;
//     int    d_length;
//     TYPE  *d_array;
// ```
// Then, we define the public interface:
// ```
//   public:
//     // TYPES
//     typedef ALLOC allocator_type;
//     typedef TYPE  value_type;
//
//     // CREATORS
//     explicit my_FixedSizeArray(int          length,
//                                const ALLOC& allocator = ALLOC());
//         // Create a fixed-size array of the specified 'length', using the
//         // optionally specified 'allocator' to supply memory.  If
//         // 'allocator' is not specified, a default-constructed instance of
//         // the parameterized 'ALLOC' type is used.  All the elements in the
//         // resulting array are default-constructed.
//
//     my_FixedSizeArray(const my_FixedSizeArray& original,
//                       const ALLOC&             allocator = ALLOC());
//         // Create a copy of the specified 'original' fixed-size array,
//         // using the optionally specified 'allocator' to supply memory.  If
//         // 'allocator' is not specified, a default-constructed instance of
//         // the parameterized 'ALLOC' type is used.
//
//     ~my_FixedSizeArray();
//         // Destroy this fixed size array.
//
//     // MANIPULATORS
//     my_FixedSizeArray& operator=(const my_FixedSizeArray& original);
//         // Assign to this array the value of the specified 'original'
//         // array.  Note that the length of this array might change.
//
//     TYPE& operator[](int index) { return d_array[index]; }
//         // Return a reference to the modifiable element at the specified
//         // 'index' position in this fixed size array.  The behavior is
//         // undefined unless 'index' is non-negative and less than
//         // 'length()'.
//
//     // ACCESSORS
//     const TYPE& operator[](int index) const { return d_array[index]; }
//         // Return a reference to the non-modifiable element at the
//         // specified 'index' position in this fixed size array.  The
//         // behavior is undefined unless 'index' is non-negative and less
//         // than 'length()'.
//
//     allocator_type get_allocator() const { return d_allocator; }
//         // Return the allocator used by this fixed size array to supply
//         // memory.
//
//     int length() const { return d_length; }
//         // Return the length specified at construction of this fixed size
//         // array.
// };
//
// // FREE OPERATORS
// template<class TYPE, class ALLOC>
// bool operator==(const my_FixedSizeArray<TYPE, ALLOC>& lhs,
//                 const my_FixedSizeArray<TYPE, ALLOC>& rhs);
//     // Return 'true' if the specified 'lhs' fixed-size array has the same
//     // value as the specified 'rhs' fixed-size array, and 'false'
//     // otherwise.  Two fixed-size arrays have the same value if they have
//     // the same length and if the element at any index in 'lhs' has the
//     // same value as the corresponding element at the same index in 'rhs'.
//
// template<class TYPE, class ALLOC>
// bool operator!=(const my_FixedSizeArray<TYPE, ALLOC>& lhs,
//                 const my_FixedSizeArray<TYPE, ALLOC>& rhs);
//     // Return 'true' if the specified 'lhs' fixed-size array does not have
//     // the same value as the specified 'rhs' fixed-size array, and 'false'
//     // otherwise.  Two fixed-size arrays have the same value if they have
//     // the same length and if the element at any index in 'lhs' has the
//     // same value as the corresponding element at the same index in 'rhs'.
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
// #include <bslma_allocatortraits.h>
//
// // CREATORS
// template<class TYPE, class ALLOC>
// my_FixedSizeArray<TYPE, ALLOC>::my_FixedSizeArray(int          length,
//                                                   const ALLOC& allocator)
//     : d_allocator(allocator), d_length(length)
// {
//     typedef bsl::allocator_traits<ALLOC> Traits;
//
//     d_array = Traits::allocate(d_allocator, d_length);
//
//     // Default construct each element of the array:
//     for (int i = 0; i < d_length; ++i) {
//         Traits::construct(d_allocator, &d_array[i]);
//     }
// }
// ```
// Next, we define the copy constructor, which initializes the allocator member
// but defers the rest of the work to the assignment operator:
// ```
// template<class TYPE, class ALLOC>
// my_FixedSizeArray<TYPE, ALLOC>::my_FixedSizeArray(
//                                         const my_FixedSizeArray& original,
//                                         const ALLOC&             allocator)
//     : d_allocator(allocator), d_length(0), d_array(0)
// {
//     *this = original;
// }
// ```
// Now we define the assignment operator, which allocates the array and copies
// elements from the `rhs` array.  Note, again, that we simplified the code by
// omitting exception-safety constructs.
// ```
// template<class TYPE, class ALLOC>
// my_FixedSizeArray<TYPE, ALLOC>&
// my_FixedSizeArray<TYPE, ALLOC>::operator=(const my_FixedSizeArray& rhs)
// {
//     typedef bsl::allocator_traits<ALLOC> Traits;
//
//     if (this != &rhs) {
//         // Call destructor for each old element
//         for (int i = 0; i < d_length; ++i) {
//             Traits::destroy(d_allocator, &d_array[i]);
//         }
//
//         // Deallocate old storage
//         Traits::deallocate(d_allocator, d_array, d_length);
//
//         // Set length and allocate new array.  Do not assign the allocator!
//         d_length = rhs.d_length;
//         d_array  = Traits::allocate(d_allocator, d_length);
//
//         // Construct each element of the 'lhs' array from the corresponding
//         // 'rhs' element.
//         for (int i = 0; i < d_length; ++i) {
//             Traits::construct(d_allocator, &d_array[i], rhs.d_array[i]);
//         }
//     }
//
//     return *this;                                                 // RETURN
// }
// ```
// Next, we define the destructor, which uses the allocator's `destroy` method
// to destroy each element, then the allocator's `deallocate` method to return
// memory to the allocator:
// ```
// template<class TYPE, class ALLOC>
// my_FixedSizeArray<TYPE, ALLOC>::~my_FixedSizeArray()
// {
//     typedef bsl::allocator_traits<ALLOC> Traits;
//
//     // Call destructor for each element
//     for (int i = 0; i < d_length; ++i) {
//         Traits::destroy(d_allocator, &d_array[i]);
//     }
//
//     // Return memory to allocator.
//     Traits::deallocate(d_allocator, d_array, d_length);
// }
// ```
// The equality and inequality operators simply compare the lengths and element
// values of the two arrays:
// ```
// // FREE OPERATORS
// template<class TYPE, class ALLOC>
// bool operator==(const my_FixedSizeArray<TYPE, ALLOC>& lhs,
//                 const my_FixedSizeArray<TYPE, ALLOC>& rhs)
// {
//     if (lhs.length() != rhs.length()) {
//         return false;                                             // RETURN
//     }
//     for (int i = 0; i < lhs.length(); ++i) {
//         if (lhs[i] != rhs[i]) {
//             return false;                                         // RETURN
//         }
//     }
//     return true;
// }
//
// template<class TYPE, class ALLOC>
// inline
// bool operator!=(const my_FixedSizeArray<TYPE, ALLOC>& lhs,
//                 const my_FixedSizeArray<TYPE, ALLOC>& rhs) {
//     return ! (lhs == rhs);
// }
// ```
// Now we can create array objects with different allocator mechanisms.  First
// we create an array, `a1`, using the default allocator and fill it with the
// values `1 .. 5`:
// ```
// #include <bslma_bslallocator.h>
// #include <bslma_testallocator.h>
//
// int main() {
//
//     my_FixedSizeArray<int, bsl::allocator<int> > a1(5);
//     assert(5                                  == a1.length());
//     assert(bslma::Default::defaultAllocator() == a1.get_allocator());
//
//     for (int i = 0; i < a1.length(); ++i) {
//         a1[i] = i + 1;
//     }
// ```
// Finally, we create a copy of `a1` using a test allocator.  The values of
// `a1` and `a2` are equal, even though they have different allocation
// mechanisms.  We verify that the test allocator was used to allocate the new
// array elements:
// ```
//     bslma::TestAllocator                         testAlloc;
//     my_FixedSizeArray<int, bsl::allocator<int> > a2(a1, &testAlloc);
//     assert(a1 == a2);
//     assert(a1.get_allocator() != a2.get_allocator());
//     assert(&testAlloc         == a2.get_allocator());
//     assert(1 == testAlloc.numBlocksInUse());
// }
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
//
// class MyType {
//
//     bslma::Allocator *d_allocator_p;
//     // etc.
//
//   public:
//     // TRAITS
//     BSLMF_NESTED_TRAIT_DECLARATION(MyType, bslma::UsesBslmaAllocator);
//
//     // CREATORS
//     explicit MyType(bslma::Allocator* basicAlloc = 0)
//       : d_allocator_p(bslma::Default::allocator(basicAlloc)) { /* ... */ }
//     MyType(const MyType&, bslma::Allocator* basicAlloc = 0)
//       : d_allocator_p(bslma::Default::allocator(basicAlloc)) { /* ... */ }
//     // etc.
//
//     // ACCESSORS
//     bslma::Allocator *allocator() const { return d_allocator_p; }
//
//     // etc.
// };
// ```
// Now, we instantiate `my_FixedSizeArray` using `MyType` and verify that,
// when we provide the address of an allocator to the constructor of the
// container, the same address is passed to the constructor of the container's
// elements:
// ```
// #include <bslmf_issame.h>
//
// int main()
// {
//     typedef my_FixedSizeArray<MyType, bsl::allocator<MyType> > ArrayType;
//
//     const int            arrayLen = 7;
//
//     bslma::TestAllocator testAlloc;
//     ArrayType            C1(arrayLen, &testAlloc);
//     assert((bsl::is_same<ArrayType::allocator_type,
//                          bsl::allocator<MyType> >::value));
//     assert(C1.get_allocator()    == bsl::allocator<MyType>(&testAlloc));
//     for (int i = 0; i < arrayLen; ++i) {
//         assert(C1[i].allocator() == &testAlloc);
//     }
// ```
// Next, we copy-construct the container and verify that the copy uses the
// default allocator, not the allocator from the original; moreover, we verify
// that the elements stored in the copy also use the default allocator.
// ```
//     ArrayType C2(C1);
//     assert(C2.get_allocator() != C1.get_allocator());
//     assert(C2.get_allocator() == bsl::allocator<MyType>());
//     for (int i = 0; i < arrayLen; ++i) {
//         assert(C2[i].allocator() != &testAlloc);
//         assert(C2[i].allocator() == bslma::Default::defaultAllocator());
//     }
// ```
// Finally, we create a third array using the test allocator and use assignment
// to give it the same value as the second array.  We then verify that the
// assignment did not modify the allocator of the lhs array and that the
// elements of the resulting copy use the same allocator as the lhs array:
// ```
//     bslma::TestAllocator testAlloc2;
//     ArrayType            C3(1, &testAlloc2);
//     assert(1 == testAlloc2.numBlocksInUse());
//     assert(1 == C3.length());
//     assert(C3.get_allocator() == bsl::allocator<MyType>(&testAlloc2));
//     assert(C3[0].allocator() == &testAlloc2);
//
//     C3 = C2;  // Assignment
//     assert(1        == testAlloc2.numBlocksInUse());
//     assert(arrayLen == C3.length());
//     assert(C3.get_allocator() == bsl::allocator<MyType>(&testAlloc2));
//     for (int i = 0; i < arrayLen; ++i) {
//         assert(C3[i].allocator() == &testAlloc2);
//     }
// }
// ```


#include <bslscm_version.h>

#include <bslma_allocator.h>
#include <bslma_allocatortraits.h>
#include <bslma_constructionutil.h>
#include <bslma_default.h>
#include <bslma_destructionutil.h>
#include <bslma_isstdallocator.h>
#include <bslma_polymorphicallocator.h>
#include <bslma_usesbslmaallocator.h>

#include <bslmf_isbitwisecopyable.h>
#include <bslmf_isbitwiseequalitycomparable.h>
#include <bslmf_nestedtraitdeclaration.h>

#include <bsls_annotation.h>
#include <bsls_assert.h>
#include <bsls_compilerfeatures.h>
#include <bsls_deprecatefeature.h>
#include <bsls_keyword.h>
#include <bsls_platform.h>
#include <bsls_review.h>
#include <bsls_util.h>     // 'addressof'

#include <cstddef>

#if BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
// Include version that can be compiled with C++03
// Generated on Fri May 13 11:05:19 2022
// Command line: sim_cpp11_features.pl bslma_bslallocator.h
# define COMPILING_BSLMA_BSLALLOCATOR_H
# include <bslma_bslallocator_cpp03.h>
# undef COMPILING_BSLMA_BSLALLOCATOR_H
#else

#define BSLMA_BSLALLOCATOR_DEPRECATE_ASSIGN \
    BSLS_DEPRECATE_FEATURE("bsl", "bsl_allocator_assign", \
                           "Do not assign allocators.")

namespace BloombergLP {
namespace bslma {

// FORWARD DECLARATIONS
struct BslAllocator_Voidish;
    // Object type that will be placeholder for 'void'

}  // close package namespace
}  // close enterprise namespace

namespace bsl {

                              // ===============
                              // class allocator
                              // ===============

/// An STL-compatible allocator that forwards allocation calls to an
/// underlying mechanism object of a type derived from `bslma::Allocator`.
/// This class template adheres to the allocator requirements defined in
/// section [allocator.requirements] and implements a superset of the
/// `std::pmr::polymorphic_allocator` class template described in section
/// [mem.poly.allocator.class] of the C++ standard and may be used to
/// instantiate any [container] class template that follows the STL
/// allocator protocol.  The allocation mechanism is chosen at run-time,
/// giving the programmer run-time control over how a container allocates
/// and frees memory.
template <class TYPE = polymorphic_allocator<>::value_type>
class allocator : public polymorphic_allocator<TYPE> {

    // PRIVATE TYPES
    typedef polymorphic_allocator<TYPE> Base;
    typedef allocator_traits<Base>      BaseTraits;

  public:
    // TRAITS
    // Note that 'allocator' is not trivially copyable because its assignment
    // operator is not trivial.
    BSLMF_NESTED_TRAIT_DECLARATION(allocator,
                                   BloombergLP::bslmf::IsBitwiseCopyable);
    BSLMF_NESTED_TRAIT_DECLARATION(allocator,
                              BloombergLP::bslmf::IsBitwiseEqualityComparable);

    // PUBLIC TYPES
    typedef TYPE                                    value_type;
    typedef value_type&                             reference;
    typedef value_type const&                       const_reference;
    typedef typename BaseTraits::size_type          size_type;
    typedef typename BaseTraits::difference_type    difference_type;
    typedef typename BaseTraits::pointer            pointer;
    typedef typename BaseTraits::const_pointer      const_pointer;
    typedef typename BaseTraits::void_pointer       void_pointer;
    typedef typename BaseTraits::const_void_pointer const_void_pointer;

    /// This nested `struct` template, parameterized by `ANY_TYPE`, provides
    /// a namespace for an `other` type alias, which is this template
    /// instantiated with `ANY_TYPE` instead of `TYPE`.  Note that this
    /// allocator type is convertible to and from `other` for any type,
    /// including `void`.
    template <class ANY_TYPE>
    struct rebind {

        typedef allocator<ANY_TYPE> other;
    };

    // CREATORS

    /// Create an allocator that will forward allocation calls to the
    /// object pointed to by `bslma::Default::defaultAllocator()`.
    /// Postcondition:
    /// ```
    /// this->mechanism() == bslma::Default::defaultAllocator()
    /// ```
    allocator();

    /// Convert a `bslma::Allocator` pointer to an `allocator` object that
    /// forwards allocation calls to the object pointed to by the specified
    /// `mechanism`.  If `mechanism` is 0, then the currently installed
    /// default allocator is used instead.  Postcondition:
    /// ```
    /// this->mechanism() == bslma::Default::allocator(mechanism)
    /// ```
    allocator(BloombergLP::bslma::Allocator *mechanism);            // IMPLICIT

#ifdef BSLS_COMPILERFEATURES_SUPPORT_DEFAULTED_FUNCTIONS
    allocator(const allocator& original) BSLS_KEYWORD_NOEXCEPT = default;
#else
    allocator(const allocator& original) BSLS_KEYWORD_NOEXCEPT;
#endif
    /// Create an allocator sharing the same mechanism object as the
    /// specified `original`.  The newly constructed allocator will compare
    /// equal to `original`, even though they may be instantiated on
    /// different types.  Postconditions:
    /// ```
    /// *this == original
    /// this->mechanism() == original.mechanism()
    /// ```
    template <class ANY_TYPE>
    allocator(const allocator<ANY_TYPE>& original) BSLS_KEYWORD_NOEXCEPT;

    //! ~allocator() = default;
        // Destroy this object.  Note that this destructor does not delete the
        // object pointed to by 'mechanism()'.

    // MANIPULATORS
    BSLMA_BSLALLOCATOR_DEPRECATE_ASSIGN
    /// **DEPRECATED** `bsl::allocator` should not be assigned.  Modify this
    /// allocator to use the same mechanism as the specified `rhs` allocator
    /// and return a modifiable reference to this object.  Note that
    /// `bsl::allocator` objects should never be assigned at runtime, but,
    /// in the absence of `if constexpr`, such assignments can sometimes be
    /// found legitimately in dead branches (branches that are never taken
    /// at runtime) within function templates; ideally, such code would be
    /// replaced by more sophisticated metaprogramming that avoided calls to
    /// this operator entirely.  Invoking this assignment will result in a
    /// review error unless `rhs == *this`, i.e., when the assignment would
    /// be a no-op.  In the future, the review error may be replaced with an
    /// a hard assertion failure.
    allocator& operator=(const allocator& rhs);

    /// Return a block of memory having sufficient size and alignment to
    /// hold the specified `n` objects of `value_type`, allocated from the
    /// memory resource held by this allocator.  Optionally specify a
    /// `hint`, which is ignored by this allocator type but theoretically
    /// used by other allocators as an aid for optimizing locality.
    BSLS_ANNOTATION_NODISCARD
    pointer allocate(size_type n, const void *hint = 0);

    /// Deallocate a block of memory at the specified `p` address by
    /// returning it to the memory resource held by this allocator.
    /// Optionally specify the number of objects, `n`, to deallocate.  The
    /// behavior is undefined unless `p` is the address of a block
    /// previously allocated by a call to `allocate` with the same `n` from
    /// a copy of this allocator having the same `value_type` and not yet
    /// deallocated.
    void deallocate(TYPE *p, std::size_t n = 1);

    /// Create a default-constructed object of (template parameter)
    /// `ELEMENT_TYPE` at the specified `address`.  If `ELEMENT_TYPE`
    /// supports `bslma`-style allocation, this allocator passes itself to
    /// the extended default constructor.  If the constructor throws, the
    /// memory at `address` is left in an unspecified state.  The behavior
    /// is undefined unless `address` refers to a block of sufficient size
    /// and properly aligned for objects of `ELEMENT_TYPE`.
    template <class ELEMENT_TYPE>
    void construct(ELEMENT_TYPE *address);

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES // $var-args=14

    /// Create an object of (template parameter) `ELEMENT_TYPE` at the
    /// specified `address`, constructed by forwarding the specified
    /// `argument1` and the (variable number of) additional specified
    /// `arguments` to the corresponding constructor of `ELEMENT_TYPE`.  If
    /// `ELEMENT_TYPE` supports `bslma`-style allocation, this allocator
    /// passes itself to the constructor.  If
    /// the constructor throws, the memory at `address` is left in an
    /// unspecified state.  Note that, in C++03, perfect forwarding is
    /// limited such that any lvalue reference in the `arguments` parameter
    /// pack is const-qualified when forwarded to the `ELEMENT_TYPE`
    /// constructor; only `argument1` can be forwarded as an unqualified
    /// lvalue.  The behavior is undefined unless `address` refers to a
    /// block of sufficient size and properly aligned for objects of
    /// `ELEMENT_TYPE`.
    template <class ELEMENT_TYPE, class ARG1, class... ARGS>
    void construct(ELEMENT_TYPE            *address,
                   ARG1&                    argument1,
                   ARGS&&...                arguments);
    template <class ELEMENT_TYPE, class ARG1, class... ARGS>
    void construct(ELEMENT_TYPE                            *address,
                   BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  argument1,
                   ARGS&&...                                arguments);
#endif

    /// Call the `TYPE` destructor for the object pointed to by the
    /// specified `address`.  Do not directly deallocate any memory.
    template <class ELEMENT_TYPE>
    void destroy(ELEMENT_TYPE *address);

    // ACCESSORS

    /// Return the address of the object referred to by the specified `x`
    /// reference, even if the (template parameter) `TYPE` overloads the
    /// unary `operator&`.
    pointer       address(reference x) const;
    const_pointer address(const_reference x) const;

    /// Return the maximum number of elements of (template parameter) `TYPE`
    /// that can be allocated using this allocator.  Note that there is no
    /// guarantee that attempts at allocating fewer elements than the value
    /// returned by `max_size` will not throw.
    BSLS_KEYWORD_CONSTEXPR
    size_type max_size() const;

    /// Return a pointer to the mechanism object to which this proxy
    /// forwards allocation and deallocation calls.
    BloombergLP::bslma::Allocator *mechanism() const;

    /// Return a default-constructed allocator.
    allocator select_on_container_copy_construction() const;
};

                           // =====================
                           // class allocator<void>
                           // =====================

/// Specialization of `allocator<T>` where `T` is `void`.  Does not contain
/// members that are unrepresentable for `void`.  Note that this
/// specialization may be removed in the future.  Use `allocator<>` or
/// `allocator<char>` instead.
template <>
class allocator<void>
    : public allocator<BloombergLP::bslma::BslAllocator_Voidish>
{

    // PRIVATE TYPES
    typedef allocator<BloombergLP::bslma::BslAllocator_Voidish> Base;

    // NOT DEFINED
    void allocate();     // Hide name inherited from base class
    void deallocate();   // Hide name inherited from base class
    void construct();    // Hide name inherited from base class
    void destroy();      // Hide name inherited from base class
    void max_size(int);  // Hide name inherited from base class

  public:
    // TRAITS
    // Note that 'allocator' is not trivially copyable because its assignment
    // operator is not trivial.
    BSLMF_NESTED_TRAIT_DECLARATION(allocator,
                                   BloombergLP::bslmf::IsBitwiseCopyable);
    BSLMF_NESTED_TRAIT_DECLARATION(allocator,
                              BloombergLP::bslmf::IsBitwiseEqualityComparable);

    // PUBLIC TYPES
    typedef void           *pointer;
    typedef const void     *const_pointer;
    typedef void           *void_pointer;
    typedef const void     *const_void_pointer;
    typedef void            value_type;

    // CREATORS

    /// Create a proxy object that will forward allocation calls to the
    /// object pointed to by `bslma::Default::defaultAllocator()`.
    /// Postcondition:
    /// ```
    /// this->mechanism() == bslma::Default::defaultAllocator();
    /// ```
    allocator();

    /// Convert a `bslma::Allocator` pointer to an `allocator` object that
    /// forwards allocation calls to the object pointed to by the specified
    /// `mechanism`.  If `mechanism` is 0, then the currently installed
    /// default allocator is used instead.  Postcondition:
    /// ```
    /// this->mechanism() == bslma::Default::allocator(mechanism);
    /// ```
    allocator(BloombergLP::bslma::Allocator *mechanism);            // IMPLICIT

#ifdef BSLS_COMPILERFEATURES_SUPPORT_DEFAULTED_FUNCTIONS
    allocator(const allocator& original) BSLS_KEYWORD_NOEXCEPT = default;
#else
    allocator(const allocator& original) BSLS_KEYWORD_NOEXCEPT;
#endif
    /// Create a proxy object sharing the same mechanism object as the
    /// specified `original`.  The newly constructed allocator will compare
    /// equal to `original`, even though they may be instantiated on
    /// different types.  Postcondition:
    /// ```
    /// this->mechanism() == original.mechanism();
    /// ```
    template <class ANY_TYPE>
    allocator(const allocator<ANY_TYPE>& original) BSLS_KEYWORD_NOEXCEPT;

    //! ~allocator();
        // Destroy this object.  Note that this does not delete the object
        // pointed to by 'mechanism()'.  Also note that this method's
        // definition is compiler generated.

    // MANIPULATORS
    //! allocator& operator=(const allocator& rhs) = default;

    // ACCESSORS

    /// Return a default-constructed allocator.
    allocator select_on_container_copy_construction() const;
};

                  // ========================================
                  // class allocator_traits<allocator<TYPE> >
                  // ========================================

/// This `struct` template provides a specialization of the
/// `allocator_traits` class template for `bsl::allocator`.  This
/// specialization is not strictly necessary, but its presence speeds up
/// compliation by bypassing a significant amount of metaprogramming.
template <class TYPE>
struct allocator_traits<allocator<TYPE> > {

    // PUBLIC TYPES
    typedef allocator<TYPE>                             allocator_type;
    typedef TYPE                                        value_type;

    typedef typename allocator_type::pointer            pointer;
    typedef typename allocator_type::const_pointer      const_pointer;
    typedef typename allocator_type::void_pointer       void_pointer;
    typedef typename allocator_type::const_void_pointer const_void_pointer;
    typedef typename allocator_type::difference_type    difference_type;
    typedef typename allocator_type::size_type          size_type;

#ifdef BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES
    template <class ELEMENT_TYPE>
    using rebind_alloc = allocator<ELEMENT_TYPE>;

    template <class ELEMENT_TYPE>
    using rebind_traits = allocator_traits<allocator<ELEMENT_TYPE> >;
#else
    template <class ELEMENT_TYPE>
    struct rebind_alloc : allocator<ELEMENT_TYPE> {
        rebind_alloc()
        : allocator<ELEMENT_TYPE>()
        {
        }

        template <typename ARG>
        rebind_alloc(const ARG& allocatorArg)
            // Convert from anything that can be used to cosntruct the base
            // type.  This might be better if SFINAE-ed out using
            // 'is_convertible', but stressing older compilers more seems
            // unwise.
        : allocator<ELEMENT_TYPE>(allocatorArg)
        {
        }
    };

    template <class ELEMENT_TYPE>
    struct rebind_traits : allocator_traits<allocator<ELEMENT_TYPE> > {
    };
#endif

    static pointer allocate(allocator_type& m, size_type n)
    {
        return m.allocate(n);
    }

    static pointer allocate(allocator_type&    m,
                            size_type          n,
                            const_void_pointer /* hint */)
    {
        return m.allocate(n);
    }

    static void deallocate(allocator_type& m, pointer p, size_type n)
    {
        m.deallocate(p, n);
    }

    template <class TYPE2>
    static void construct(allocator_type&  m,
                          TYPE2           *p)
    {
        m.construct(p);
    }

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES // $var-args=14
    template <class TYPE2, class ARG1, class... ARGS>
    static void construct(allocator_type&  m,
                          TYPE2           *p,
                          ARG1&            argument1,
                          ARGS&&...        arguments)
    {
        m.construct(p,
                    argument1,
                    BSLS_COMPILERFEATURES_FORWARD(ARGS, arguments)...);
    }

    template <class TYPE2, class ARG1, class... ARGS>
    static void construct(allocator_type&                          m,
                          TYPE2                                   *p,
                          BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  argument1,
                          ARGS&&...                                arguments)
    {
        m.construct(p,
                    BSLS_COMPILERFEATURES_FORWARD(ARG1, argument1),
                    BSLS_COMPILERFEATURES_FORWARD(ARGS, arguments)...);
    }
#endif

    template <class ELEMENT_TYPE>
    static void destroy(allocator_type& m, ELEMENT_TYPE *p)
    {
        m.destroy(p);
    }

    BSLS_KEYWORD_CONSTEXPR
    static size_type max_size(const allocator_type& m)
    {
        return m.max_size();
    }

    // Allocator propagation traits
    static
    allocator_type select_on_container_copy_construction(const allocator_type&)
    {
        return allocator_type();
    }

    typedef false_type is_always_equal;

    typedef false_type propagate_on_container_copy_assignment;

    typedef false_type propagate_on_container_move_assignment;

    typedef false_type propagate_on_container_swap;
};


// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================

                              // ---------------
                              // class allocator
                              // ---------------

// CREATORS
template <class TYPE>
inline
allocator<TYPE>::allocator()
: Base(BloombergLP::bslma::Default::defaultAllocator())
{
}

template <class TYPE>
inline
allocator<TYPE>::allocator(BloombergLP::bslma::Allocator *mechanism)
: Base(BloombergLP::bslma::Default::allocator(mechanism))
{
}

#ifndef BSLS_COMPILERFEATURES_SUPPORT_DEFAULTED_FUNCTIONS
// In C++11 and later, this copy constructor is defaulted.
template <class TYPE>
inline
allocator<TYPE>::allocator(const allocator& original) BSLS_KEYWORD_NOEXCEPT
: Base(original)
{
}
#endif

template <class TYPE>
template <class ANY_TYPE>
inline
allocator<TYPE>::allocator(const allocator<ANY_TYPE>& original)
                                                          BSLS_KEYWORD_NOEXCEPT
: Base(original)
{
}

// MANIPULATORS
template <class TYPE>
inline
allocator<TYPE>&
allocator<TYPE>::operator=(const allocator& rhs)
{
    BSLS_REVIEW_OPT(rhs == *this &&
                    "'bsl::allocator' objects cannot be assigned");

    if (this != &rhs) {
        // As the base class does not support assignment, the only way to
        // change the mechanism is to destroy and re-create this object
        this->~allocator();
        return *::new(this) allocator(rhs);
    }

    return *this;
}

template <class TYPE>
inline
typename allocator<TYPE>::pointer
allocator<TYPE>::allocate(size_type n, const void * /* hint */)
{
    return Base::allocate(n);
}

template <class TYPE>
inline
void allocator<TYPE>::deallocate(TYPE *p, std::size_t n)
{
    Base::deallocate(p, n);
}

template <class TYPE>
template <class ELEMENT_TYPE>
inline
void allocator<TYPE>::construct(ELEMENT_TYPE *address)
{
    BloombergLP::bslma::ConstructionUtil::construct(address, *this);
}

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
template <class TYPE>
template <class ELEMENT_TYPE, class ARG1, class... ARGS>
inline
void allocator<TYPE>::construct(ELEMENT_TYPE *address,
                                ARG1&         argument1,
                                ARGS&&...     arguments)
{
    BloombergLP::bslma::ConstructionUtil::construct(
        address,
        *this,
        argument1,
        BSLS_COMPILERFEATURES_FORWARD(ARGS, arguments)...);
}

template <class TYPE>
template <class ELEMENT_TYPE, class ARG1, class... ARGS>
inline
void allocator<TYPE>::construct(
                            ELEMENT_TYPE                            *address,
                            BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  argument1,
                            ARGS&&...                                arguments)
{
    BloombergLP::bslma::ConstructionUtil::construct(
        address,
        *this,
        BSLS_COMPILERFEATURES_FORWARD(ARG1, argument1),
        BSLS_COMPILERFEATURES_FORWARD(ARGS, arguments)...);
}
#endif

template <class TYPE>
template <class ELEMENT_TYPE>
inline
void allocator<TYPE>::destroy(ELEMENT_TYPE *address)
{
    BloombergLP::bslma::DestructionUtil::destroy(address);
}


// ACCESSORS
template <class TYPE>
inline
typename allocator<TYPE>::pointer
allocator<TYPE>::address(reference x) const
{
    return BloombergLP::bsls::Util::addressOf(x);
}

template <class TYPE>
inline
typename allocator<TYPE>::const_pointer
allocator<TYPE>::address(const_reference x) const
{
    return BloombergLP::bsls::Util::addressOf(x);
}

template <class TYPE>
BSLS_KEYWORD_CONSTEXPR inline
typename allocator<TYPE>::size_type allocator<TYPE>::max_size() const
{
    // Return the largest value, 'v', such that 'v * sizeof(T)' fits in a
    // 'size_type'.

    BSLS_KEYWORD_CONSTEXPR
        const size_type MAX_NUM_BYTES    = ~size_type(0);
    BSLS_KEYWORD_CONSTEXPR
        const size_type MAX_NUM_ELEMENTS = MAX_NUM_BYTES / sizeof(TYPE);

    return MAX_NUM_ELEMENTS;
}

template <class TYPE>
inline
BloombergLP::bslma::Allocator *allocator<TYPE>::mechanism() const
{
    return static_cast<BloombergLP::bslma::Allocator *>(this->resource());
}

template <class TYPE>
inline
allocator<TYPE> allocator<TYPE>::select_on_container_copy_construction() const
{
    return allocator();
}


                           // ---------------------
                           // class allocator<void>
                           // ---------------------

// CREATORS
inline
allocator<void>::allocator()
{
}

inline
allocator<void>::allocator(BloombergLP::bslma::Allocator *mechanism)
: Base(mechanism)
{
}

#ifndef BSLS_COMPILERFEATURES_SUPPORT_DEFAULTED_FUNCTIONS
// In C++11 and later, this copy constructor is defaulted.
inline
allocator<void>::allocator(const allocator& original) BSLS_KEYWORD_NOEXCEPT
: Base(original)
{
}
#endif

template <class ANY_TYPE>
inline
allocator<void>::allocator(const allocator<ANY_TYPE>& original)
                                                          BSLS_KEYWORD_NOEXCEPT
: Base(original)
{
}

inline
allocator<void> allocator<void>::select_on_container_copy_construction() const
{
    return allocator<void>();
}

}  // close namespace bsl

#if BSLS_PLATFORM_CMP_MSVC
// As of MSVC 19.30.30709 (2022), the following workaround is still needed.
// When a fix is released, the above '#if' condition should be updated to apply
// only to versions before the fixed one.

// These equality and inequality operators should be unnecessary because they
// automatically fall back on 'polymoprhic_allocator'.  However an odd bug in
// MSVC causes it, in the presence of '<bslma_convertibleallocator.h>' to
// include the 'bslma::ConvertibleAllocator' "hidden friend" equality operators
// in the the lookup set even if neither argument is 'ConvertibleAllocator',
// thus making 'operator==' ambiguous when comparing 'bsl::allocator' to
// 'bslma::Allocator *'.  Strangely 'using namespace bslma' suppresses this
// bug, but it is not practical to require clients to do that.  The operators
// below quash this ambiguity.  Although harmless for other platforms, they are
// compiled only for affected versions of MSVC and are considered an
// implementation detail (not part of the interface for 'bsl::allocator').

template <class TYPE>
inline
bool operator==(const bsl::allocator<TYPE>&    a,
                BloombergLP::bslma::Allocator *b)
{
    return a.resource() == b || a.resource()->is_equal(*b);
}

template <class TYPE>
inline
bool operator==(BloombergLP::bslma::Allocator *a,
                const bsl::allocator<TYPE>&    b)
{
    return a == b.resource() || a->is_equal(*b.resource());
}

template <class TYPE>
inline
bool operator!=(const bsl::allocator<TYPE>&    a,
                BloombergLP::bslma::Allocator *b)
{
    return ! (a.resource() == b || a.resource()->is_equal(*b));
}

template <class TYPE>
inline
bool operator!=(BloombergLP::bslma::Allocator *a,
                const bsl::allocator<TYPE>&    b)
{
    return ! (a == b.resource() || a->is_equal(*b.resource()));
}
#endif

// ============================================================================
//                                TYPE TRAITS
// ============================================================================

namespace BloombergLP {
namespace bslma {

/// An allocator is not *itself* an allocator-aware type, even though it is
/// convertible from `bsl::Allocator *`.
template <class TYPE>
struct UsesBslmaAllocator< ::bsl::allocator<TYPE> > : bsl::false_type {
};

/// A `bsl::allocator` inherits its `allocate` method from a base class,
/// which causes `IsStdAllocator` to fail the auto-detected it.
template <class TYPE>
struct IsStdAllocator<bsl::allocator<TYPE> > : bsl::true_type {
};

/// `bsl::allocator<void>` is not an allocator type, even though all other
/// instantiations are allocator types.
template <>
struct IsStdAllocator<bsl::allocator<void> > : bsl::false_type {
};

}  // close namespace bslma
}  // close enterprise namespace

#endif // End C++11 code

#endif

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
