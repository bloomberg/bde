// bslstl_array.h                                                     -*-C++-*-
#ifndef INCLUDED_BSLSTL_ARRAY
#define INCLUDED_BSLSTL_ARRAY

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide an STL compliant array.
//
//@CLASSES:
//   bsl::array: an STL compliant array
//
//@CANONICAL_HEADER: bsl_array.h
//
//@SEE_ALSO: bslstl_vector
//
//@DESCRIPTION: This component defines a single class template, 'bsl::array',
// implementing the standard container 'std::array', holding a non-resizable
// array of values of a template parameter type where the size is specified as
// the second template parameter.
//
// An instantiation of 'array' is a value-semantic type whose salient
// attributes are its size and the sequence of values the array contains.  If
// 'array' is instantiated with a value type that is not value-semantic, then
// the array will not retain all of its value-semantic qualities.  In
// particular, if a value type cannot be tested for equality, then an 'array'
// containing objects of that type will fail to compile the equality comparison
// operator.  Similarly, if an 'array' is instantiated with a type that does
// not have a copy-constructor, then the 'array' will not be copyable.
//
// An array meets most the requirements of a container with random access
// iterators in the C++ standard [array].  The 'array' implemented here follows
// the C++11 standard when compiled with a C++11 compiler and follows the C++03
// standard otherwise.

// An array lacks certain requirements of a sequential container.  Array lacks
// 'insert', 'erase', 'emplace', and 'clear', as these functions would require
// modifying the size of the array.

// An array also meets the requirements of an aggregate.  This means that an
// array has: no user-declared constructors, no private or protected non-static
// data members, no base classes, and no virtual functions.  An array can be
// constructed using aggregate initialization.  Refer to the section
// [del.init.aggr] in the C++ standard for more detailed information.
//
///Operations
///----------
// This section describes the run-time complexity of operations on instances of
// 'array':
//..
//  Legend
//  ------
//  'V'                 - (template parameter) 'VALUE_TYPE' of the array
//  'S'                 - (template parameter) 'SIZE' of the array
//  'a', 'b'            - two distinct objects of type 'array<V, S>'
//  'k'                 - non-negative integer
//  'vt1', 'vt2', 'vt3' - objects of type 'VALUE_TYPE'
//
//  |-----------------------------------------+-------------------------------|
//  | Operation                               | Complexity                    |
//  |=========================================+===============================|
//  | array<V> a    (default construction)    | O[1]                          |
//  |-----------------------------------------+-------------------------------|
//  | array<V> a(b) (copy construction)       | O[S]                          |
//  |-----------------------------------------+-------------------------------|
//  | array<V> a = {{vt1, vt2, vt3}}          | O[S]                          |
//  | array<V> a = {vt1, vt2, vt3}            |                               |
//  |               (aggregate initialization)|                               |
//  |-----------------------------------------+-------------------------------|
//  | a.~array<V>() (destruction)             | O[S]                          |
//  |-----------------------------------------+-------------------------------|
//  | a.begin(), a.end(),                     | O[1]                          |
//  | a.cbegin(), a.cend(),                   |                               |
//  | a.rbegin(), a.rend(),                   |                               |
//  | a.crbegin(), a.crend()                  |                               |
//  |-----------------------------------------+-------------------------------|
//  | a.size()                                | O[1]                          |
//  |-----------------------------------------+-------------------------------|
//  | a.max_size()                            | O[1]                          |
//  |-----------------------------------------+-------------------------------|
//  | a.empty()                               | O[1]                          |
//  |-----------------------------------------+-------------------------------|
//  | a[k]                                    | O[1]                          |
//  |-----------------------------------------+-------------------------------|
//  | a.at(k)                                 | O[1]                          |
//  |-----------------------------------------+-------------------------------|
//  | a.front()                               | O[1]                          |
//  |-----------------------------------------+-------------------------------|
//  | a.back()                                | O[1]                          |
//  |-----------------------------------------+-------------------------------|
//  | a.swap(b), swap(a,b)                    | O[S]                          |
//  |-----------------------------------------+-------------------------------|
//  | a = b;           (copy assignment)      | O[S]                          |
//  |-----------------------------------------+-------------------------------|
//  | a == b, a != b                          | O[S]                          |
//  |-----------------------------------------+-------------------------------|
//  | a < b, a <= b, a > b, a >= b            | O[S]                          |
//  |-----------------------------------------+-------------------------------|
//..
//
///Comparing an array of floating point values
///-------------------------------------------
// The comparison operator performs a bit-wise comparison for floating point
// types ('float' and 'double'), which produces results for NaN, +0, and -0
// values that do not meet the guarantees provided by the standard.
// The 'bslmf::IsBitwiseEqualityComparable' trait for 'double' and 'float'
// types returns 'true' which is incorrect because a comparison with a NaN
// value is always 'false', and -0 and +0 are equal.
//..
//    bsl::array<double, 1> a{bsl::numeric_limits<double>::quiet_NaN()};
//    ASSERT(a == a);   // This assertion will *NOT* fail!
//..
// Addressing this issue, i.e., updating 'bslmf::IsBitwiseEqualityComparable'
// to return 'false' for floating point types, could potentially destabilize
// production software so the change (for the moment) has not been made.
//
///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1: Returning an array from a function
///- - - - - - - - - - - - - - - - - - - - - - -
// Suppose we want to define a function that will return an array of 'float's.
// If a raw array were used, the size would need to be tracked separately
// because raw arrays decay to pointers when passed as function arguments, or
// returned by-value.  'bsl::array' does not decay, and so provides a simple
// solution to this problem.
//..
//  typedef bsl::array<float, 3> Point;
//
//  Point createPoint(float f1, float f2, float f3)
//  {
//      bsl::array<float, 3> ret = {f1, f2, f3};
//      return ret;
//  }
//..
// Create a bsl::array object containing three values set to the specified
// 'f1', 'f2', 'f3'.
//..
//  void usageExample()
//  {
//      Point p1 = createPoint(1.0, 1.0, 1.0);
//      Point p2 = createPoint(2.0, 2.0, 2.0);
//      Point p3 = createPoint(3.0, 3.0, 3.0);
//
//      bsl::array<Point, 3> points = {p1, p2, p3};
//
//      for(size_t i = 0; i < points.size(); ++i) {
//          for(size_t j = 0; j < points[i].size(); ++j) {
//              points[i][j] *= 2.0f;
//          }
//      }
//  }
//..
// Use the createPoint function to generate three arrays of floats.  The arrays
// are returned by copy and the 'size()' member function is used to access the
// size of the arrays that could not be done with a raw array.

#include <bslscm_version.h>

#include <bslstl_algorithm.h>
#include <bslstl_iterator.h>
#include <bslstl_stdexceptutil.h>

#include <bslalg_arrayprimitives.h>
#include <bslalg_rangecompare.h>
#include <bslalg_synththreewayutil.h>
#include <bslalg_hasstliterators.h>

#include <bslh_hash.h>

#include <bslma_default.h>

#include <bslmf_assert.h>
#include <bslmf_enableif.h>
#include <bslmf_isnothrowswappable.h>
#include <bslmf_issame.h>
#include <bslmf_movableref.h>
#include <bslmf_removecv.h>

#include <bsls_assert.h>
#include <bsls_compilerfeatures.h>
#include <bsls_keyword.h>
#include <bsls_libraryfeatures.h>
#include <bsls_platform.h>

#include <stddef.h>

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP11_TUPLE)
# include <tuple>  // 'std::tuple_size' etc.
#endif

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY)
#include <utility>     // std::swap (C++11)
#else
#include <algorithm>   // std::swap (C++03)
#endif

#ifndef BDE_DONT_ALLOW_TRANSITIVE_INCLUDES
#include <bsls_nativestd.h>
#endif // BDE_DONT_ALLOW_TRANSITIVE_INCLUDES

#ifndef BDE_DISABLE_CPP17_ABI
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY

#include <array>  // 'std::array'
#include <tuple>  // 'std::get'

namespace bsl {
using std::array;
using std::get;
}  // close namespace bsl

#define BSLSTL_ARRAY_IS_ALIASED
#endif  // BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY
#endif  // BDE_DISABLE_CPP17_ABI


#ifndef BSLSTL_ARRAY_IS_ALIASED

// DEFECT DETECTION MACROS

#if defined(BSLS_COMPILERFEATURES_SUPPORT_CONSTEXPR_CPP14)                    \
 && defined(BSLS_PLATFORM_CMP_GNU) && BSLS_PLATFORM_CMP_VERSION < 60000
// gcc 5.4 (and earlier) does not allow non-'constexpr' code in a relaxed
// 'constexpr' function, such as a 'BSLS_ASSERT' macro.  As use of such code in
// this component is limited to function templates, the effect is to silently
// disable the 'constexpr'-ness of those functions, which parse correctly and
// evaluate as expected at run-time, but fail to compile in a constant
// evaluation context.  In order to support C++14 standard conformance, we
// choose to disable our BDE contract checks on platforms affected by this
// compiler bug.
# define BSLSTL_ARRAY_DISABLE_CONSTEXPR_CONTRACTS       1
#endif

namespace bsl {
                                // ===========
                                // class array
                                // ===========

template <class VALUE_TYPE, size_t SIZE>
struct array {
    // This class template provides a standard conforming implementation of
    // 'std::array'.  'array' is an aggregate wrapper around a raw array,
    // supporting aggregate initialization and an iterator interface as
    // required for a standard container.

// BDE_VERIFY pragma: push
// BDE_VERIFY pragma: -KS02  // Tag implicitly requires private declaration

    // DATA
    VALUE_TYPE d_data[(0 == SIZE) ? 1 : SIZE];

// BDE_VERIFY pragma: pop

    // PUBLIC TYPES
    typedef VALUE_TYPE                             value_type;
    typedef VALUE_TYPE                            *pointer;
    typedef const VALUE_TYPE                      *const_pointer;
    typedef VALUE_TYPE&                            reference;
    typedef const VALUE_TYPE&                      const_reference;
    typedef size_t                                 size_type;
    typedef ptrdiff_t                              difference_type;
    typedef pointer                                iterator;
    typedef const_pointer                          const_iterator;
    typedef bsl::reverse_iterator<iterator>        reverse_iterator;
    typedef bsl::reverse_iterator<const_iterator>  const_reverse_iterator;

    // CREATORS
    //! array() = default;
        // Create an 'array' object.  Every element is default constructed if
        // 'VALUE_TYPE' is default constructible; otherwise, 'array' is not
        // default constructible.
    //! array(const array& original) = default;
        // Create an 'array' object having the same value as the specified
        // 'original' object.  Every element is copy constructed from the
        // corresponding element in the specified 'original' if 'VALUE_TYPE' is
        // copy constructible; otherwise, 'array' is not copy constructible.
        // Only in C++11 and later.
    //! array(array&& original) = default;
        // Create an 'array' object having the same value as the specified
        // 'original' object.  Every element is move constructed from the
        // corresponding element in the specified 'original' if 'VALUE_TYPE' is
        // move constructible; otherwise, 'array' is not move constructible.
    //! ~array() = default;
        // Destroy this object.  Evert element is destroyed if 'VALUE_TYPE' is
        // destructible; otherwise, array is not destructible.

    // MANIPULATORS
    void fill(const VALUE_TYPE& value);
        // Set every element in this array to the specified 'value' using the
        // 'operator=' of 'value_type'.

    void swap(array& rhs) BSLS_KEYWORD_NOEXCEPT_SPECIFICATION(
                                 bsl::is_nothrow_swappable<VALUE_TYPE>::value);
        // Exchange each corresponding element between this array and the
        // specified 'rhs' array by calling 'swap(a,b)' where 'swap' is found
        // by overload resolution including at least the namespaces 'std' and
        // the associated namespaces of 'VALUE_TYPE'.

    BSLS_KEYWORD_CONSTEXPR_CPP14
    iterator begin() BSLS_KEYWORD_NOEXCEPT;
        // Return an iterator providing modifiable access to the first element
        // in this array; return a past-the-end iterator if this array has size
        // 0.

    BSLS_KEYWORD_CONSTEXPR_CPP14
    iterator end() BSLS_KEYWORD_NOEXCEPT;
        // Return a past-the-end iterator providing modifiable access to this
        // array.

    BSLS_KEYWORD_CONSTEXPR_CPP17
    reverse_iterator rbegin() BSLS_KEYWORD_NOEXCEPT;
        // Return a reverse iterator providing modifiable access to the last
        // element in this array; return a past-the-end iterator if this array
        // has size 0.

    BSLS_KEYWORD_CONSTEXPR_CPP17
    reverse_iterator rend() BSLS_KEYWORD_NOEXCEPT;
        // Return the past-the-end reverse iterator providing modifiable access
        // to this array.

    BSLS_KEYWORD_CONSTEXPR_CPP14
    reference operator[](size_type position);
        // Return a reference providing modifiable access to the element at the
        // specified 'position' in this array.  The behavior is undefined
        // unless 'position < size()'.

    BSLS_KEYWORD_CONSTEXPR_CPP14
    reference at(size_type position);
        // Return a reference to the element at the specified 'position' in
        // this array.  Throw an 'out_of_range' exception if
        // 'position >= size()'.

    BSLS_KEYWORD_CONSTEXPR_CPP14
    reference front();
        // Return a reference to the first element in this array.  The behavior
        // is undefined unless 'SIZE > 0'.

    BSLS_KEYWORD_CONSTEXPR_CPP14
    reference back();
        // Return a reference to the last element in this array.  The behavior
        // is undefined unless 'SIZE > 0'.

    BSLS_KEYWORD_CONSTEXPR_CPP14
    pointer data() BSLS_KEYWORD_NOEXCEPT;
        // Return the address of the first element of the underlying raw array.
        // Return a valid 'T*' which cannot be dereferenced if the 'SIZE' is 0.

    //! array& operator=(const array& other);
        // Sets every element in this array to the corresponding element in the
        // specified 'other' if 'VALUE_TYPE' is copy assignable; otherwise,
        // 'array' is not copy assignable.
    //! array& operator=(array&& other);
        // Moves every element in the specified 'other' into the corresponding
        // element in this array in the if 'VALUE_TYPE' is moves assignable;
        // otherwise, 'array' is not move assignable.

    // BDE_VERIFY pragma: -FABC01  // Function not in alphanumeric order
    // ACCESSORS
    BSLS_KEYWORD_CONSTEXPR_CPP14
    const_iterator begin() const BSLS_KEYWORD_NOEXCEPT;
    BSLS_KEYWORD_CONSTEXPR_CPP14
    const_iterator cbegin() const BSLS_KEYWORD_NOEXCEPT;
        // Return an iterator providing non-modifiable access to the first
        // element in this array; return a past-the-end iterator if this array
        // has size 0.

    BSLS_KEYWORD_CONSTEXPR_CPP14
    const_iterator end() const BSLS_KEYWORD_NOEXCEPT;
    BSLS_KEYWORD_CONSTEXPR_CPP14
    const_iterator cend() const BSLS_KEYWORD_NOEXCEPT;
        // Return a past-the-end iterator providing non-modifiable access to
        // this array.

    BSLS_KEYWORD_CONSTEXPR_CPP17
    const_reverse_iterator rbegin() const BSLS_KEYWORD_NOEXCEPT;
    BSLS_KEYWORD_CONSTEXPR_CPP17
    const_reverse_iterator crbegin() const BSLS_KEYWORD_NOEXCEPT;
        // Return a reverse iterator providing non-modifiable access to the
        // last element in this array, and the past-the-end reverse iterator if
        // this array has size 0.

    BSLS_KEYWORD_CONSTEXPR_CPP17
    const_reverse_iterator rend() const BSLS_KEYWORD_NOEXCEPT;
    BSLS_KEYWORD_CONSTEXPR_CPP17
    const_reverse_iterator crend() const BSLS_KEYWORD_NOEXCEPT;
        // Return the past-the-end reverse iterator providing non-modifiable
        // access to this 'array'.

    BSLS_KEYWORD_CONSTEXPR bool empty() const BSLS_KEYWORD_NOEXCEPT;
        // Return 'true' if the array has size 0, and 'false' otherwise.

    BSLS_KEYWORD_CONSTEXPR size_type size() const BSLS_KEYWORD_NOEXCEPT;
    BSLS_KEYWORD_CONSTEXPR size_type max_size() const BSLS_KEYWORD_NOEXCEPT;
        // Return the number of elements in this array.

    BSLS_KEYWORD_CONSTEXPR_CPP14
    const_reference operator[](size_type position) const;
        // Return a reference providing non-modifiable access to the element at
        // the specified 'position' in this array.  The behavior is undefined
        // unless 'position < size()'.

    BSLS_KEYWORD_CONSTEXPR_CPP14
    const_reference at(size_type position) const;
        // Return a reference providing non-modifiable access to the element at
        // the specified 'position' in this array.  Throw an 'out_of_range'
        // exception if 'position >= size()'.

    BSLS_KEYWORD_CONSTEXPR_CPP14 const_reference front() const;
        // Return a reference providing non-modifiable access to the first
        // element in this array.  The behavior is undefined unless 'SIZE > 0'.

    BSLS_KEYWORD_CONSTEXPR_CPP14 const_reference back() const;
        // Return a reference providing non-modifiable access to the last
        // element in this array.  Behavior is undefined unless 'SIZE > 0'.

    BSLS_KEYWORD_CONSTEXPR_CPP14
    const_pointer data() const BSLS_KEYWORD_NOEXCEPT;
        // Return the address of the first element of the underlying raw array.
        // Return a valid 'T*' which cannot be dereferenced if the 'SIZE' is 0.
};

#ifdef BSLS_COMPILERFEATURES_SUPPORT_CTAD
// CLASS TEMPLATE DEDUCTION GUIDES

template<class    VALUE_TYPE,
         class... OTHERS,
         class = bsl::enable_if_t<(bsl::is_same_v<VALUE_TYPE, OTHERS> && ...)>
         >
array(VALUE_TYPE, OTHERS...) -> array<VALUE_TYPE, 1 + sizeof...(OTHERS)>;
    // Deduce the specified types 'VALUE_TYPE' and 'SIZE' from the
    // corresponding elements in the sequence supplied to the constructor of
    // 'array'. The type of the first element in the sequence is the type of
    // the elements of the array, and the length of the sequence is the size of
    // the array.
#endif

// FREE OPERATORS
template <class VALUE_TYPE, size_t SIZE>
bool operator==(const array<VALUE_TYPE, SIZE>& lhs,
                const array<VALUE_TYPE, SIZE>& rhs);
    // Return 'true' if the specified 'lhs' has the same value as the specified
    // 'rhs'; return false otherwise.  Two arrays have the same value if each
    // element has the same value as the corresponding element in the other
    // array.

#ifndef BSLS_COMPILERFEATURES_SUPPORT_THREE_WAY_COMPARISON

template <class VALUE_TYPE, size_t SIZE>
bool operator!=(const array<VALUE_TYPE, SIZE>& lhs,
                const array<VALUE_TYPE, SIZE>& rhs);
    // Return 'true' if the specified 'lhs' does not have the same value as the
    // specified 'rhs'; return false otherwise.  Two arrays do not have the
    // same value if some element in the ordered sequence of elements of 'lhs'
    // does not have the same value as the corresponding element in the ordered
    // sequence of elements of 'rhs'.

#endif  // BSLS_COMPILERFEATURES_SUPPORT_THREE_WAY_COMPARISON

#ifdef BSLALG_SYNTHTHREEWAYUTIL_AVAILABLE

template <class VALUE_TYPE, size_t SIZE>
BloombergLP::bslalg::SynthThreeWayUtil::Result<VALUE_TYPE> operator<=>(
                                           const array<VALUE_TYPE, SIZE>& lhs,
                                           const array<VALUE_TYPE, SIZE>& rhs);
    // Perform a lexicographic three-way comparison of the specified 'lhs' and
    // the specified 'rhs' arrays by using the comparison operators of
    // 'VALUE_TYPE' on each element; return the result of that comparison.

#else

template <class VALUE_TYPE, size_t SIZE>
bool operator<(const array<VALUE_TYPE, SIZE>& lhs,
               const array<VALUE_TYPE, SIZE>& rhs);
    // Return 'true' if the specified 'lhs' is lexicographically less than the
    // specified 'rhs' by using the comparison operators of 'VALUE_TYPE' on
    // each element; return 'false' otherwise.

template <class VALUE_TYPE, size_t SIZE>
bool operator>(const array<VALUE_TYPE, SIZE>& lhs,
               const array<VALUE_TYPE, SIZE>& rhs);
    // Return 'true' if the specified 'lhs' is lexicographically greater than
    // the specified 'rhs' by using the comparison operators of 'VALUE_TYPE' on
    // each element; return 'false' otherwise.

template <class VALUE_TYPE, size_t SIZE>
bool operator<=(const array<VALUE_TYPE, SIZE>& lhs,
                const array<VALUE_TYPE, SIZE>& rhs);
    // Return 'true' if the specified 'lhs' is lexicographically less than the
    // specified 'rhs' by using the comparison operators of 'VALUE_TYPE' on
    // each element or if 'lhs' and 'rhs' are equal; return 'false' otherwise.

template <class VALUE_TYPE, size_t SIZE>
bool operator>=(const array<VALUE_TYPE, SIZE>& lhs,
                const array<VALUE_TYPE, SIZE>& rhs);
    // Return 'true' if the specified 'lhs' is lexicographically greater than
    // the specified 'rhs' by using the comparison operators of 'VALUE_TYPE' on
    // each element or if 'lhs' and 'rhs' are equal; return 'false' otherwise.

#endif  // BSLALG_SYNTHTHREEWAYUTIL_AVAILABLE

// FREE FUNCTIONS
template <class VALUE_TYPE, size_t SIZE>
void swap(array<VALUE_TYPE, SIZE>& lhs, array<VALUE_TYPE, SIZE>& rhs);
    // Call 'swap' using ADL on each element of the specified 'lhs' with the
    // corresponding element in the specified 'rhs'.

template<size_t INDEX, class TYPE, size_t SIZE>
BSLS_KEYWORD_CONSTEXPR_CPP14
TYPE& get(array<TYPE, SIZE>& a) BSLS_KEYWORD_NOEXCEPT;
    // Return a reference providing modifiable access to the element of the
    // specified 'a', having the ordinal number specified by the (template
    // parameter) 'INDEX'.  This function will not compile unless 'INDEX < N'.

template<size_t INDEX, class TYPE, size_t SIZE>
BSLS_KEYWORD_CONSTEXPR
const TYPE& get(const array<TYPE, SIZE>& a) BSLS_KEYWORD_NOEXCEPT;
    // Return a reference providing non-modifiable access to the element of the
    // specified 'a', having the ordinal number specified by the (template
    // parameter) 'INDEX'.  This function will not compile unless 'INDEX < N'.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
template<size_t INDEX, class TYPE, size_t SIZE>
BSLS_KEYWORD_CONSTEXPR_CPP14
TYPE&& get(array<TYPE, SIZE>&& a) BSLS_KEYWORD_NOEXCEPT;
    // Return an rvalue reference providing modifiable access to the element of
    // the specified 'a', having the ordinal number specified by the (template
    // parameter) 'INDEX'.  This function will not compile unless 'INDEX < N'.

template<size_t INDEX, class TYPE, size_t SIZE>
BSLS_KEYWORD_CONSTEXPR
const TYPE&& get(const array<TYPE, SIZE>&& a) BSLS_KEYWORD_NOEXCEPT;
    // Return an rvalue reference providing non-modifiable access to the
    // element of the specified 'a', having the ordinal number specified by the
    // (template parameter) 'INDEX'.  This function will not compile unless
    // 'INDEX < N'.
#endif  // BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES

// HASH SPECIALIZATIONS
template <class HASH_ALGORITHM, class TYPE, size_t SIZE>
void hashAppend(HASH_ALGORITHM& hashAlgorithm, const array<TYPE, SIZE>& input);
    // Pass the specified 'input' to the specified 'hashAlgorithm'

}  // close namespace bsl

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP11_TUPLE)

namespace std {

#if defined(BSLS_PLATFORM_CMP_CLANG)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmismatched-tags"
    // The native STL library headers for libstdc++ are internally inconsistent
    // so it is not possible for this header to resolve this warning by picking
    // either 'struct' or 'class' for the class introducer.  We do not see this
    // warning directly from the native libraries only because they are tagged
    // as system headers, which implicitly silences all warnings.
#endif

                            // ====================
                            // struct tuple_element
                            // ====================

template<size_t INDEX, class TYPE, size_t SIZE>
struct tuple_element<INDEX, bsl::array<TYPE, SIZE> >
{
    // This partial specialization of 'tuple_element' provides compile-time
    // access to the type of the array's elements.

    // STATIC CHECKS
    BSLMF_ASSERT(INDEX < SIZE);

    // TYPES
    typedef TYPE type;
};

                             // =================
                             // struct tuple_size
                             // =================

template<class TYPE, size_t SIZE>
struct tuple_size<bsl::array<TYPE, SIZE> > : integral_constant<size_t, SIZE>
{
    // This meta-function provides a compile-time way to obtain the number of
    // elements in an array.
};

#if defined(BSLS_PLATFORM_CMP_CLANG)
#pragma clang diagnostic pop
#endif

}  // close namespace std

#endif  // BSLS_LIBRARYFEATURES_HAS_CPP11_TUPLE
#endif  // !BSLSTL_ARRAY_IS_ALIASED

#ifdef BSLSTL_ARRAY_IS_ALIASED
namespace BloombergLP {
namespace bslh {

// HASH SPECIALIZATIONS
template <class HASH_ALGORITHM, class TYPE, size_t SIZE>
void hashAppend(HASH_ALGORITHM&               hashAlgorithm,
                const std::array<TYPE, SIZE>& input);
    // Pass the specified 'input' to the specified 'hashAlgorithm' hashing
    // algorithm of the (template parameter) type 'HASH_ALGORITHM'.  Note that
    // this function violates the BDE coding standard, adding a function for a
    // namespace for a different package, and none of the function parameters
    // are from this package either.  This is necessary in order to provide an
    // implementation of 'bslh::hashAppend' for the (native) standard library
    // 'array' type as we are not allowed to add overloads directly into
    // namespace 'std', and this component essentially provides the interface
    // between 'bsl' and 'std' array types.

}  // close namespace bslh
}  // close enterprise namespace

#endif  // BSLSTL_ARRAY_IS_ALIASED

// ============================================================================
//                  TEMPLATE AND INLINE FUNCTION DEFINITIONS
// ============================================================================

#ifndef BSLSTL_ARRAY_IS_ALIASED

namespace bsl {
                                // -----------
                                // class array
                                // -----------

// MANIPULATORS

// suppress comparison of 'unsigned' expression is always false warnings
#ifdef BSLS_PLATFORM_HAS_PRAGMA_GCC_DIAGNOSTIC
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wtype-limits"
#endif

template <class VALUE_TYPE, size_t SIZE>
void array<VALUE_TYPE, SIZE>::fill(const VALUE_TYPE& value)
{
    for (size_t i = 0; i < SIZE; ++i) {
        d_data[i] = value;
    }
}

template <class VALUE_TYPE, size_t SIZE>
void array<VALUE_TYPE, SIZE>::swap(array<VALUE_TYPE, SIZE>& rhs)
    BSLS_KEYWORD_NOEXCEPT_SPECIFICATION(
        bsl::is_nothrow_swappable<VALUE_TYPE>::value)
{
    for (size_t i = 0; i < SIZE; ++i) {
        using std::swap;
        swap(d_data[i], rhs.d_data[i]);
    }
}

// suppress comparison of 'unsigned' expression is always false warnings
#ifdef BSLS_PLATFORM_HAS_PRAGMA_GCC_DIAGNOSTIC
#pragma GCC diagnostic pop
#endif

// ACCESSORS
template <class VALUE_TYPE, size_t SIZE>
BSLS_KEYWORD_CONSTEXPR_CPP14
typename array<VALUE_TYPE, SIZE>::iterator
array<VALUE_TYPE, SIZE>::begin() BSLS_KEYWORD_NOEXCEPT
{
    return d_data;
}

template <class VALUE_TYPE, size_t SIZE>
BSLS_KEYWORD_CONSTEXPR_CPP14
typename array<VALUE_TYPE, SIZE>::const_iterator
array<VALUE_TYPE, SIZE>::begin() const BSLS_KEYWORD_NOEXCEPT
{
    return d_data;
}

template <class VALUE_TYPE, size_t SIZE>
BSLS_KEYWORD_CONSTEXPR_CPP14
typename array<VALUE_TYPE, SIZE>::iterator
array<VALUE_TYPE, SIZE>::end() BSLS_KEYWORD_NOEXCEPT
{
    return d_data + SIZE;
}

template <class VALUE_TYPE, size_t SIZE>
BSLS_KEYWORD_CONSTEXPR_CPP14
typename array<VALUE_TYPE, SIZE>::const_iterator
array<VALUE_TYPE, SIZE>::end() const BSLS_KEYWORD_NOEXCEPT
{
    return d_data + SIZE;
}

template <class VALUE_TYPE, size_t SIZE>
BSLS_KEYWORD_CONSTEXPR_CPP17
typename array<VALUE_TYPE, SIZE>::reverse_iterator
array<VALUE_TYPE, SIZE>::rbegin() BSLS_KEYWORD_NOEXCEPT
{
    return array<VALUE_TYPE, SIZE>::reverse_iterator(d_data + SIZE);
}

template <class VALUE_TYPE, size_t SIZE>
BSLS_KEYWORD_CONSTEXPR_CPP17
typename array<VALUE_TYPE, SIZE>::const_reverse_iterator
array<VALUE_TYPE, SIZE>::rbegin() const BSLS_KEYWORD_NOEXCEPT
{
    return array<VALUE_TYPE, SIZE>::const_reverse_iterator(d_data + SIZE);
}

template <class VALUE_TYPE, size_t SIZE>
BSLS_KEYWORD_CONSTEXPR_CPP17
typename array<VALUE_TYPE, SIZE>::reverse_iterator
array<VALUE_TYPE, SIZE>::rend() BSLS_KEYWORD_NOEXCEPT
{
    return array<VALUE_TYPE, SIZE>::reverse_iterator(d_data);
}

template <class VALUE_TYPE, size_t SIZE>
BSLS_KEYWORD_CONSTEXPR_CPP17
typename array<VALUE_TYPE, SIZE>::const_reverse_iterator
array<VALUE_TYPE, SIZE>::rend() const BSLS_KEYWORD_NOEXCEPT
{
    return array<VALUE_TYPE, SIZE>::const_reverse_iterator(d_data);
}

template <class VALUE_TYPE, size_t SIZE>
BSLS_KEYWORD_CONSTEXPR_CPP14
typename array<VALUE_TYPE, SIZE>::const_iterator
array<VALUE_TYPE, SIZE>::cbegin() const BSLS_KEYWORD_NOEXCEPT
{
    return d_data;
}

template <class VALUE_TYPE, size_t SIZE>
BSLS_KEYWORD_CONSTEXPR_CPP14
typename array<VALUE_TYPE, SIZE>::const_iterator
array<VALUE_TYPE, SIZE>::cend() const BSLS_KEYWORD_NOEXCEPT
{
    return d_data + SIZE;
}

template <class VALUE_TYPE, size_t SIZE>
BSLS_KEYWORD_CONSTEXPR_CPP17
typename array<VALUE_TYPE, SIZE>::const_reverse_iterator
array<VALUE_TYPE, SIZE>::crbegin() const BSLS_KEYWORD_NOEXCEPT
{
    return array<VALUE_TYPE, SIZE>::const_reverse_iterator(d_data + SIZE);
}

template <class VALUE_TYPE, size_t SIZE>
BSLS_KEYWORD_CONSTEXPR_CPP17
typename array<VALUE_TYPE, SIZE>::const_reverse_iterator
array<VALUE_TYPE, SIZE>::crend() const BSLS_KEYWORD_NOEXCEPT
{
    return array<VALUE_TYPE, SIZE>::const_reverse_iterator(d_data);
}

template <class VALUE_TYPE, size_t SIZE>
BSLS_KEYWORD_CONSTEXPR
bool array<VALUE_TYPE, SIZE>::empty() const BSLS_KEYWORD_NOEXCEPT
{
    return SIZE == 0;
}

template <class VALUE_TYPE, size_t SIZE>
BSLS_KEYWORD_CONSTEXPR
size_t array<VALUE_TYPE, SIZE>::size() const BSLS_KEYWORD_NOEXCEPT
{
    return SIZE;
}

template <class VALUE_TYPE, size_t SIZE>
BSLS_KEYWORD_CONSTEXPR
size_t array<VALUE_TYPE, SIZE>::max_size() const BSLS_KEYWORD_NOEXCEPT
{
    return SIZE;
}

template <class VALUE_TYPE, size_t SIZE>
BSLS_KEYWORD_CONSTEXPR_CPP14
typename array<VALUE_TYPE, SIZE>::reference
array<VALUE_TYPE, SIZE>::operator[](size_type position)
{
    BSLS_ASSERT(position < SIZE);
    return d_data[position];
}

template <class VALUE_TYPE, size_t SIZE>
BSLS_KEYWORD_CONSTEXPR_CPP14
typename array<VALUE_TYPE, SIZE>::const_reference
array<VALUE_TYPE, SIZE>::operator[](size_type position) const
{
#if !defined(BSLSTL_ARRAY_DISABLE_CONSTEXPR_CONTRACTS)
    BSLS_ASSERT(position < SIZE);
#endif

    return d_data[position];
}

template <class VALUE_TYPE, size_t SIZE>
BSLS_KEYWORD_CONSTEXPR_CPP14
typename array<VALUE_TYPE, SIZE>::reference array<VALUE_TYPE, SIZE>::at(
                                                            size_type position)
{
    if (position >= SIZE) {
        BloombergLP::bslstl::StdExceptUtil::throwOutOfRange(
            "array<...>::at(position): invalid position");
    }
    return d_data[position];
}

template <class VALUE_TYPE, size_t SIZE>
BSLS_KEYWORD_CONSTEXPR_CPP14
typename array<VALUE_TYPE, SIZE>::const_reference
array<VALUE_TYPE, SIZE>::at(size_type position) const
{
    if (position >= SIZE) {
        BloombergLP::bslstl::StdExceptUtil::throwOutOfRange(
            "array<...>::at(position): invalid position");
    }
    return d_data[position];
}

template <class VALUE_TYPE, size_t SIZE>
typename array<VALUE_TYPE, SIZE>::reference
BSLS_KEYWORD_CONSTEXPR_CPP14
array<VALUE_TYPE, SIZE>::front()
{
    BSLS_ASSERT(SIZE > 0);
    return d_data[0];
}

template <class VALUE_TYPE, size_t SIZE>
BSLS_KEYWORD_CONSTEXPR_CPP14
typename array<VALUE_TYPE, SIZE>::const_reference
array<VALUE_TYPE, SIZE>::front() const
{
#if !defined(BSLSTL_ARRAY_DISABLE_CONSTEXPR_CONTRACTS)
    BSLS_ASSERT(SIZE > 0);
#endif

    return d_data[0];
}

template <class VALUE_TYPE, size_t SIZE>
typename array<VALUE_TYPE, SIZE>::reference
BSLS_KEYWORD_CONSTEXPR_CPP14
array<VALUE_TYPE, SIZE>::back()
{
    BSLS_ASSERT(SIZE > 0);
    return d_data[SIZE - 1];
}

template <class VALUE_TYPE, size_t SIZE>
BSLS_KEYWORD_CONSTEXPR_CPP14
typename array<VALUE_TYPE, SIZE>::const_reference
array<VALUE_TYPE, SIZE>::back() const
{
#if !defined(BSLSTL_ARRAY_DISABLE_CONSTEXPR_CONTRACTS)
    BSLS_ASSERT(SIZE > 0);
#endif

    return d_data[SIZE - 1];
}

template <class VALUE_TYPE, size_t SIZE>
BSLS_KEYWORD_CONSTEXPR_CPP14
typename array<VALUE_TYPE, SIZE>::iterator
array<VALUE_TYPE, SIZE>::data() BSLS_KEYWORD_NOEXCEPT
{
    return d_data;
}

template <class VALUE_TYPE, size_t SIZE>
BSLS_KEYWORD_CONSTEXPR_CPP14
typename array<VALUE_TYPE, SIZE>::const_iterator
array<VALUE_TYPE, SIZE>::data() const BSLS_KEYWORD_NOEXCEPT
{
    return d_data;
}

// HASH SPECIALIZATIONS

// suppress comparison of 'unsigned' expression is always false warnings
#ifdef BSLS_PLATFORM_HAS_PRAGMA_GCC_DIAGNOSTIC
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wtype-limits"
#endif

template <class HASH_ALGORITHM, class TYPE, size_t SIZE>
void hashAppend(HASH_ALGORITHM& hashAlgorithm, const array<TYPE, SIZE>& input)
{
    using ::BloombergLP::bslh::hashAppend;

    hashAppend(hashAlgorithm, SIZE);
    for (size_t i = 0; i < SIZE; ++i)
    {
        hashAppend(hashAlgorithm, input[i]);
    }
}

// suppress comparison of 'unsigned' expression is always false warnings
#ifdef BSLS_PLATFORM_HAS_PRAGMA_GCC_DIAGNOSTIC
#pragma GCC diagnostic pop
#endif
}  // close namespace bsl

// FREE OPERATORS
template <class VALUE_TYPE, size_t SIZE>
bool bsl::operator==(const array<VALUE_TYPE, SIZE>& lhs,
                     const array<VALUE_TYPE, SIZE>& rhs)
{
    return BloombergLP::bslalg::RangeCompare::equal(lhs.begin(),
                                                    lhs.end(),
                                                    lhs.size(),
                                                    rhs.begin(),
                                                    rhs.end(),
                                                    rhs.size());
}

#ifndef BSLS_COMPILERFEATURES_SUPPORT_THREE_WAY_COMPARISON

template <class VALUE_TYPE, size_t SIZE>
bool bsl::operator!=(const array<VALUE_TYPE, SIZE>& lhs,
                     const array<VALUE_TYPE, SIZE>& rhs)
{
    return !(lhs == rhs);
}

#endif  // BSLS_COMPILERFEATURES_SUPPORT_THREE_WAY_COMPARISON

#ifdef BSLALG_SYNTHTHREEWAYUTIL_AVAILABLE

template <class VALUE_TYPE, size_t SIZE>
BloombergLP::bslalg::SynthThreeWayUtil::Result<VALUE_TYPE> bsl::operator<=>(
                                            const array<VALUE_TYPE, SIZE>& lhs,
                                            const array<VALUE_TYPE, SIZE>& rhs)
{
    return bsl::lexicographical_compare_three_way(
                              lhs.begin(),
                              lhs.end(),
                              rhs.begin(),
                              rhs.end(),
                              BloombergLP::bslalg::SynthThreeWayUtil::compare);
}

#else

template <class VALUE_TYPE, size_t SIZE>
bool bsl::operator<(const array<VALUE_TYPE, SIZE>& lhs,
                    const array<VALUE_TYPE, SIZE>& rhs)
{
    return 0 > BloombergLP::bslalg::RangeCompare::lexicographical(lhs.begin(),
                                                                  lhs.end(),
                                                                  lhs.size(),
                                                                  rhs.begin(),
                                                                  rhs.end(),
                                                                  rhs.size());
}

template <class VALUE_TYPE, size_t SIZE>
bool bsl::operator>(const array<VALUE_TYPE, SIZE>& lhs,
                    const array<VALUE_TYPE, SIZE>& rhs)
{
    return rhs < lhs;
}

template <class VALUE_TYPE, size_t SIZE>
bool bsl::operator<=(const array<VALUE_TYPE, SIZE>& lhs,
                     const array<VALUE_TYPE, SIZE>& rhs)
{
    return !(rhs < lhs);
}

template <class VALUE_TYPE, size_t SIZE>
bool bsl::operator>=(const array<VALUE_TYPE, SIZE>& lhs,
                     const array<VALUE_TYPE, SIZE>& rhs)
{
    return !(lhs < rhs);
}

#endif  // BSLALG_SYNTHTHREEWAYUTIL_AVAILABLE

// FREE FUNCTIONS
template <class VALUE_TYPE, size_t SIZE>
void bsl::swap(array<VALUE_TYPE, SIZE>& lhs, array<VALUE_TYPE, SIZE>& rhs)
{
    lhs.swap(rhs);
}

template<size_t INDEX, class TYPE, size_t SIZE>
BSLS_KEYWORD_CONSTEXPR_CPP14
TYPE& bsl::get(array<TYPE, SIZE>& a) BSLS_KEYWORD_NOEXCEPT
{
    return a.d_data[INDEX];
}

template<size_t INDEX, class TYPE, size_t SIZE>
BSLS_KEYWORD_CONSTEXPR
const TYPE& bsl::get(const array<TYPE, SIZE>& a) BSLS_KEYWORD_NOEXCEPT
{
    return a.d_data[INDEX];
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
template<size_t INDEX, class TYPE, size_t SIZE>
BSLS_KEYWORD_CONSTEXPR_CPP14
TYPE&& bsl::get(array<TYPE, SIZE>&& a) BSLS_KEYWORD_NOEXCEPT
{
    return BloombergLP::bslmf::MovableRefUtil::move(a.d_data[INDEX]);
}

template<size_t INDEX, class TYPE, size_t SIZE>
BSLS_KEYWORD_CONSTEXPR
const TYPE&& bsl::get(const array<TYPE, SIZE>&& a) BSLS_KEYWORD_NOEXCEPT
{
    return BloombergLP::bslmf::MovableRefUtil::move(a.d_data[INDEX]);
}
#endif  // BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES


#endif  // !BSLSTL_ARRAY_IS_ALIASED

#ifdef BSLSTL_ARRAY_IS_ALIASED
namespace BloombergLP {
namespace bslh {

// HASH SPECIALIZATIONS
template <class HASH_ALGORITHM, class TYPE, size_t SIZE>
void hashAppend(HASH_ALGORITHM&               hashAlgorithm,
                const std::array<TYPE, SIZE>& input)
{
    using ::BloombergLP::bslh::hashAppend;

    hashAppend(hashAlgorithm, SIZE);
    if BSLS_KEYWORD_CONSTEXPR_CPP17 (SIZE > 0) {
        for (size_t i = 0; i < SIZE; ++i) {
            hashAppend(hashAlgorithm, input[i]);
        }
    }
}

}  // close namespace bslh
}  // close enterprise namespace

#endif  // BSLSTL_ARRAY_IS_ALIASED

#if defined(BSLSTL_ARRAY_IS_ALIASED) \
 && defined(BSLS_LIBRARYFEATURES_HAS_CPP20_BASELINE_LIBRARY)
namespace bsl {
using std::to_array;
}  // close namespace bsl
#else
namespace bsl {

// FREE FUNCTIONS
template< class TYPE, std::size_t SIZE >
BSLS_KEYWORD_CONSTEXPR_CPP14
array<typename remove_cv<TYPE>::type, SIZE> to_array( TYPE (&src)[SIZE] );
    // Creates an 'array' from the specified 'src' one-dimensional built-in
    // array by copying the corresponding elements.  The template parameter
    // 'TYPE' shall not itself be a built-in array.  Note that 'TYPE' must
    // be 'CopyConstructible' and, in C++ versions prior to C++14, must also
    // be 'DefaultConstructible'.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
template< class TYPE, std::size_t SIZE >
BSLS_KEYWORD_CONSTEXPR_CPP14
array<typename remove_cv<TYPE>::type, SIZE> to_array( TYPE (&&src)[SIZE] );
    // Creates an 'array' from the specified 'src' one-dimensional built-in
    // array by moving the corresponding elements.  The template parameter
    // 'TYPE' shall not itself be a built-in array.  Note that 'TYPE' must
    // be 'MoveConstructible' and, in C++ versions prior to C++14, must also
    // be 'DefaultConstructible'.

#endif  // BSLSTL_ARRAY_IS_ALIASED &&
        // BSLS_LIBRARYFEATURES_HAS_CPP20_BASELINE_LIBRARY

}  // close namespace bsl

// FREE FUNCTIONS

#if defined(BSLS_COMPILERFEATURES_SUPPORT_VARIADIC_TEMPLATES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_VARIABLE_TEMPLATES)
namespace BloombergLP {
namespace bslstl_to_array_impl {

template <class TYPE, std::size_t SIZE, std::size_t... INDICES>
inline
BSLS_KEYWORD_CONSTEXPR_CPP14 bsl::array<bsl::remove_cv_t<TYPE>, SIZE>
to_array_lvalue_builder(TYPE (&src)[SIZE], std::index_sequence<INDICES...>)
    // This implementation detail function copy constructs a 'bsl::array' from
    // the specified 'src' argument.
{
    return {{src[INDICES]...}};
}

template <class TYPE, std::size_t SIZE, std::size_t... INDICES>
inline
BSLS_KEYWORD_CONSTEXPR_CPP14 bsl::array<bsl::remove_cv_t<TYPE>, SIZE>
to_array_rvalue_builder(TYPE(&&src)[SIZE], std::index_sequence<INDICES...>)
    // This implementation detail function move constructs a 'bsl::array' from
    // the specified 'src' argument.
{
    return {{std::move(src[INDICES])...}};
}

}  // close namespace bslstl_to_array_impl
}  // close enterprise namespace

template <class TYPE, std::size_t SIZE>
inline
BSLS_KEYWORD_CONSTEXPR_CPP14
bsl::array<typename bsl::remove_cv<TYPE>::type, SIZE>
bsl::to_array(TYPE (&src)[SIZE])
{
    BSLMF_ASSERT(!bsl::is_array<TYPE>::value);
    BSLMF_ASSERT(bsl::is_copy_constructible<TYPE>::value);

    return BloombergLP::bslstl_to_array_impl::to_array_lvalue_builder(
                                             src,
                                             std::make_index_sequence<SIZE>());
}

template <class TYPE, std::size_t SIZE>
inline
BSLS_KEYWORD_CONSTEXPR_CPP14
bsl::array<typename bsl::remove_cv<TYPE>::type, SIZE>
bsl::to_array(TYPE (&&src)[SIZE])
{
    BSLMF_ASSERT(!bsl::is_array<TYPE>::value);
    BSLMF_ASSERT(std::is_move_constructible<TYPE>::value);

    return BloombergLP::bslstl_to_array_impl::to_array_rvalue_builder(
                                             std::move(src),
                                             std::make_index_sequence<SIZE>());
}

#else // ! ..._SUPPORT_{VARIADIC,VARIABLE}_TEMPLATES

template <class TYPE, std::size_t SIZE>
BSLS_KEYWORD_CONSTEXPR_CPP14
bsl::array<typename bsl::remove_cv<TYPE>::type, SIZE>
bsl::to_array(TYPE (&src)[SIZE])
{
    BSLMF_ASSERT(!bsl::is_array<TYPE>::value);
    BSLMF_ASSERT(bsl::is_copy_constructible<TYPE>::value);

    array<typename remove_cv<TYPE>::type, SIZE> result;

    for (std::size_t i = 0; i < SIZE; ++i) {
        result[i] = src[i];
    }

    return result;
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
template <class TYPE, std::size_t SIZE>
BSLS_KEYWORD_CONSTEXPR_CPP14
bsl::array<typename bsl::remove_cv<TYPE>::type, SIZE>
bsl::to_array(TYPE(&&src)[SIZE])
{
    BSLMF_ASSERT(!bsl::is_array<TYPE>::value);
    BSLMF_ASSERT(std::is_move_constructible<TYPE>::value);

    array<typename remove_cv<TYPE>::type, SIZE> result;

    for (std::size_t i = 0; i < SIZE; ++i) {
        result[i] = std::move(src[i]);
    }

    return result;
}
#endif  // BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES


#endif  // BSLS_COMPILERFEATURES_SUPPORT_VARIADIC_TEMPLATES
        // && BSLS_COMPILERFEATURES_SUPPORT_VARIABLE_TEMPLATES


#endif
// ============================================================================
//                                TYPE TRAITS
// ============================================================================

namespace BloombergLP {

namespace bslalg {

template <class TYPE, size_t SIZE>
struct HasStlIterators<bsl::array<TYPE, SIZE> >
    : bsl::true_type
{};

}  // close namespace bslalg

}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2019 Bloomberg Finance L.P.
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
