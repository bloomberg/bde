// bslalg_rangecompare.h                                              -*-C++-*-
#ifndef INCLUDED_BSLALG_RANGECOMPARE
#define INCLUDED_BSLALG_RANGECOMPARE

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide algorithms to compare iterator-ranges of elements.
//
//@CLASSES:
//  bslalg::RangeCompare: comparison algorithms for iterator ranges
//
//@SEE_ALSO: bslmf_isbitwiseequalitycomparable
//
//@DESCRIPTION: This component provides a utility 'struct',
// 'bslalg::RangeCompare', that defines two overloaded class methods, 'equal'
// and 'lexicographical', for comparing two ranges, each specified by a pair of
// input iterators that are compliant with the C++11 standard [24.2.3].  The
// 'equal' method determines whether two specified ranges compare equal.  The
// 'lexicographical' method determines whether the first range compares
// lexicographically less than, equal to, or greater than the second range.
// Under certain circumstances, 'bslalg::RangeCompare::equal' and
// 'bslalg::RangeCompare::lexicographical' may perform optimized comparisons,
// as described below.
//
// 'bslalg::RangeCompare::equal' may perform a bit-wise comparison of the two
// ranges when the following two criteria are met:
//: o The input iterators are convertible to a pointer type.
//: o The trait 'bslmf::IsBitwiseEqualityComparable' is declared for
//:   the type of the objects in the ranges being compared.
//
// 'bslalg::RangeCompare::lexicographical' may perform a bit-wise comparison of
// the two ranges when the following criterion is met:
//: o The input iterators are convertible to pointers to a wide or unsigned
//    character type.
//
// Note that a class having the 'bslmf::IsBitwiseEqualityComparable'
// trait can be described as bit-wise comparable and should meet the following
// criteria:
//: o The values represented by two objects belonging to the class are the same
//:   if and only if each of the data members in the class has the same value
//:   in both objects.
//: o The class layout includes no padding.
//: o The class has no virtual members.
//
// Note that this component is for use primarily by the 'bslstl' package.
// Other clients should use the STL algorithms (in headers '<bsl_algorithm.h>'
// and '<bsl_memory.h>').
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Defining Equality-Comparison Operators on a Container
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// In this example we will use the 'bslalg::RangeCompare::equal' class method
// to implement the equality-comparison operators for an iterable container
// type residing in the 'bslstl' package, and highlight the circumstances under
// which the optimization provided by the class method may be applied.
//
// Suppose that we have a new iterable container type that will be included in
// the 'bslstl' package, and we wish to define comparison operators for the
// container.  If the container has an iterator that provides access to the
// container's elements in a consistent order, and the elements themselves are
// equality-comparable, we can implement the container's equality-comparison
// operators by pair-wise comparing each of the elements over the entire range
// of elements in both containers.  In such cases the container can use the
// 'bslalg::RangeCompare::equal' class method to equal-compare the container's
// elements, taking advantage of the optimizations the class method provides
// for bit-wise equality-comparable objects.
//
// First, we create an elided definition of a container class, 'MyContainer',
// which provides read-only iterators of the type 'MyContainer::ConstIterator':
//..
//  template <class VALUE_TYPE>
//  class MyContainer {
//      // This class implements a container, semantically similar to
//      // 'std::vector', holding objects of the (template parameter) type
//      // 'VALUE_TYPE'.
//
//    private:
//      // DATA
//      // ...
//
//    public:
//      // PUBLIC TYPES
//      typedef const VALUE_TYPE *ConstIterator;
//          // This 'typedef' provides an alias for the type of iterator
//          // providing non-modifiable access to the elements in the
//          // container.
//
//      // CREATORS
//      explicit MyContainer(bslma::Allocator *basicAllocator = 0);
//          // Create an empty 'MyContainer' object having no capacity.
//          // Optionally specify a 'basicAllocator' used to supply memory.  If
//          // 'basicAllocator' is 0, the currently installed default allocator
//          // is used.
//
//      // ...
//
//      // MANIPULATORS
//      // ...
//
//      void push_back(const VALUE_TYPE& value);
//          // Append the specified 'value' at the past-the-end position in
//          // this container, increasing the container's capacity if needed.
//
//      // ...
//
//      // ACCESSORS
//      ConstIterator begin() const;
//          // Return an iterator providing non-modifiable access to the first
//          // element in this container.
//
//      ConstIterator end() const;
//          // Return an iterator providing non-modifiable access to the
//          // past-the-end element in this container.
//
//      std::size_t size() const;
//          // Return the number of elements in this container.
//
//      // ...
//  };
//..
// Notice that 'ConstIterator' is defined as a pointer type, which is one of
// the criteria required to enable the optimizations provided by the
// 'bslalg::RangeCompare::equal' class method.
//
// Then, we declare the equality-comparison operators for 'MyContainer':
//..
//  template <class VALUE_TYPE>
//  bool operator==(const MyContainer<VALUE_TYPE>& lhs,
//                  const MyContainer<VALUE_TYPE>& rhs);
//      // Return 'true' if the specified 'lhs' and 'rhs' objects have the same
//      // value, and 'false' otherwise.  Two 'MyContainer' objects have the
//      // same value if they have the same length, and each element in 'lhs'
//      // has the same value as the corresponding element in 'rhs'.
//
//  template <class VALUE_TYPE>
//  bool operator!=(const MyContainer<VALUE_TYPE>& lhs,
//                  const MyContainer<VALUE_TYPE>& rhs);
//      // Return 'true' if the specified 'lhs' and 'rhs' objects do not have
//      // the same value, and 'false' otherwise.  Two 'MyContainer' objects do
//      // not have the same value if they do not have the same length, or if
//      // any element in 'lhs' does not have the same value as the
//      // corresponding element in 'rhs'.
//..
// Next, we implement the equality-comparison operators using
// 'bslalg::RangeCompare::equal':
//..
//  template <class VALUE_TYPE>
//  inline
//  bool operator==(const MyContainer<VALUE_TYPE>& lhs,
//                  const MyContainer<VALUE_TYPE>& rhs)
//  {
//      return BloombergLP::bslalg::RangeCompare::equal(lhs.begin(),
//                                                      lhs.end(),
//                                                      lhs.size(),
//                                                      rhs.begin(),
//                                                      rhs.end(),
//                                                      rhs.size());
//  }
//
//  template <class VALUE_TYPE>
//  inline
//  bool operator!=(const MyContainer<VALUE_TYPE>& lhs,
//                  const MyContainer<VALUE_TYPE>& rhs)
//  {
//      return !BloombergLP::bslalg::RangeCompare::equal(lhs.begin(),
//                                                       lhs.end(),
//                                                       lhs.size(),
//                                                       rhs.begin(),
//                                                       rhs.end(),
//                                                       rhs.size());
//  }
//..
// Then, we create the elided definition of a value-semantic class, 'MyString',
// together with its definition of 'operator==':
//..
//  class MyString {
//      // This class provides a simple, elided string class that conforms to
//      // the 'bslma::Allocator' model.
//
//    private:
//      // DATA
//      char             *d_start_p;      // storage for the string
//      std::size_t       d_length;       // length of the string
//      bslma::Allocator *d_allocator_p;  // memory allocator (held, not owned)
//
//      // ...
//
//      // FRIENDS
//      friend bool operator==(const MyString&, const MyString&);
//      // ...
//
//    public:
//      // CREATORS
//      explicit MyString(const char       *string,
//                        bslma::Allocator *basicAllocator = 0);
//          // Create a 'MyString' object initialized to the value of the
//          // specified 'string'.  Optionally specify a 'basicAllocator' used
//          // to supply memory.  If 'basicAllocator' is 0, the currently
//          // installed default allocator is used.
//
//      // ...
//  };
//
//  bool operator==(const MyString& lhs, const MyString& rhs)
//  {
//      return lhs.d_length == rhs.d_length
//          && 0 == std::strncmp(lhs.d_start_p, rhs.d_start_p, lhs.d_length);
//  }
//..
// Notice that 'MyString' is not bit-wise comparable because the address values
// of the 'd_start_p' pointer data members in two 'MyString' objects will be
// different, even if the string values of the two objects are the same.
//
// Next, we create two 'MyContainer<MyString>' objects, and compare them using
// 'operator==':
//..
//  MyContainer<MyString> c1;
//  MyContainer<MyString> c2;
//
//  c1.push_back(MyString("hello"));
//  c1.push_back(MyString("goodbye"));
//
//  c2.push_back(MyString("hello"));
//  c2.push_back(MyString("goodbye"));
//
//  assert(c1 == c2);
//..
// Here, the call to the 'bslalg::RangeCompare::equal' class method in
// 'operator==' will perform an unoptimized pair-wise comparison of the
// elements in 'c1' and 'c2'.
//
// Then, we create the elided definition of another value-semantic class,
// 'MyPoint', together with its definition of 'operator==':
//..
//  class MyPoint {
//      // This class provides a simple, elided point type that is bit-wise
//      // comparable with other objects of the same type.
//
//    private:
//      // DATA
//      int d_x;  // the x-coordinate of the point
//      int d_y;  // the y-coordinate of the point
//
//      // FRIENDS
//      friend bool operator==(const MyPoint&, const MyPoint&);
//      // ...
//
//    public:
//      // TRAITS
//      BSLMF_NESTED_TRAIT_DECLARATION(MyPoint,
//                            BloombergLP::bslmf::IsBitwiseEqualityComparable);
//
//      // CREATORS
//      MyPoint(int x, int y);
//          // Create a 'MyPoint' object whose x- and y-coordinates have the
//          // specified 'x' and 'y' values, respectively.
//
//      // ...
//  };
//
//  bool operator==(const MyPoint& lhs, const MyPoint& rhs)
//  {
//      return lhs.d_x == rhs.d_x && lhs.d_y == rhs.d_y;
//  }
//..
// Notice that the value of a 'MyPoint' object derives from the values of all
// of its data members, and that no padding is required for alignment.
// Furthermore, 'MyPoint' has no virtual methods.  Therefore, 'MyPoint' objects
// are bit-wise comparable, and we can correctly declare the
// 'bslmf::IsBitwiseEqualityComparable' trait for the class, as shown
// above under the public 'TRAITS' section.
//
// Now, we create two 'MyContainer<MyPoint>' objects and compare them using
// 'operator==':
//..
//  MyContainer<MyPoint> c3;
//  MyContainer<MyPoint> c4;
//
//  c3.push_back(MyPoint(1, 2));
//  c3.push_back(MyPoint(3, 4));
//
//  c4.push_back(MyPoint(1, 2));
//  c4.push_back(MyPoint(3, 4));
//
//  assert(c3 == c4);  // potentially optimized
//..
// Here, the call to 'bslalg::RangeCompare::equal' in 'operator==' may take
// advantage of the fact that 'MyPoint' is bit-wise comparable and perform the
// comparison by directly bit-wise comparing the entire range of elements
// contained in the 'MyContainer<MyPoint>' objects.  This comparison can
// provide a significant performance boost over the comparison between two
// 'MyContainer<MyPoint>' objects in which the nested
// 'bslmf::IsBitwiseEqualityComparable' trait is not associated with the
// 'MyPoint' class.
//
// Finally, note that we can instantiate 'MyContainer' with 'int' or any other
// primitive type as the 'VALUE_TYPE' and still benefit from the optimized
// comparison operators, because primitive (i.e.: fundamental, enumerated, and
// pointer) types are inherently bit-wise comparable:
//..
//  MyContainer<int> c5;
//  MyContainer<int> c6;
//
//  c5.push_back(1);
//  c5.push_back(2);
//  c5.push_back(3);
//
//  c6.push_back(1);
//  c6.push_back(2);
//  c6.push_back(3);
//
//  assert(c5 == c6);  // potentially optimized
//..

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLMF_INTEGRALCONSTANT
#include <bslmf_integralconstant.h>
#endif

#ifndef INCLUDED_BSLMF_ISBITWISEEQUALITYCOMPARABLE
#include <bslmf_isbitwiseequalitycomparable.h>
#endif

#ifndef INCLUDED_BSLMF_ISCONVERTIBLE
#include <bslmf_isconvertible.h>
#endif

#ifndef INCLUDED_BSLMF_MATCHANYTYPE
#include <bslmf_matchanytype.h>
#endif

#ifndef INCLUDED_BSLMF_METAINT
#include <bslmf_metaint.h>
#endif

#ifndef INCLUDED_CLIMITS
#include <climits>
#define INCLUDED_CLIMITS
#endif

#ifndef INCLUDED_CSTDDEF
#include <cstddef>
#define INCLUDED_CSTDDEF
#endif

#ifndef INCLUDED_CSTRING
#include <cstring>
#define INCLUDED_CSTRING
#endif

#ifndef INCLUDED_CWCHAR
#include <cwchar>
#define INCLUDED_CWCHAR
#endif

namespace BloombergLP {

namespace bslalg {

                          // ==================
                          // class RangeCompare
                          // ==================

struct RangeCompare {
    // This utility 'struct' provides two static class methods, 'equal' and
    // 'lexicographical', for comparing two ranges of values.  'equal' returns
    // 'true' if each element in one range has the same value as the
    // corresponding element in the other range, and 'false' otherwise.
    // 'lexicographical' returns 0 if the two ranges are equal, a positive
    // value if the first range is greater than the second, and a negative
    // value if the second range is greater than the first.  A range is
    // specified by a pair of beginning and ending iterators, with an optional
    // length parameter.  Additionally, an overload is provided for the 'equal'
    // class method that allows the end iterator for one range to be omitted.
    //
    // 'equal' requires that the elements in the ranges can be compared with
    // 'operator=='.
    //
    // 'lexicographical' requires that the elements in the ranges can be
    // compared with 'operator<'.

    // TYPES
    typedef std::size_t size_type;
        // 'size_type' is an alias for an unsigned value representing the size
        // of an object or the number of elements in a range.

    // CLASS METHODS
    template <class INPUT_ITER>
    static bool equal(INPUT_ITER start1,
                      INPUT_ITER end1,
                      INPUT_ITER start2);
        // Compare each element in the range beginning at the specified
        // 'start1' position and ending immediately before the specified 'end1'
        // position to the corresponding element in the range of the same
        // length beginning at the specified 'start2' position, as if using
        // 'operator==' element-by-element.  Return 'true' if every pair of
        // corresponding elements compares equal, and 'false' otherwise.  Note
        // that this implementation uses 'operator==' to perform the
        // comparisons, or bit-wise comparison if the value type has the
        // bit-wise equality-comparable trait.

    template <class INPUT_ITER>
    static bool equal(INPUT_ITER start1,
                      INPUT_ITER end1,
                      INPUT_ITER start2,
                      INPUT_ITER end2);
    template <class INPUT_ITER>
    static bool equal(INPUT_ITER start1, INPUT_ITER end1, size_type length1,
                      INPUT_ITER start2, INPUT_ITER end2, size_type length2);
        // Compare each element in the range beginning at the specified
        // 'start1' position and ending immediately before the specified 'end1'
        // position, to the corresponding element in the range beginning at the
        // specified 'start2' position and ending immediately before the
        // specified 'end2' position, as if by using 'operator=='
        // element-by-element.  Optionally specify the length of each range,
        // 'length1' and 'length2'.  Return 'true' if the ranges have the same
        // length and every element in the first range compares equal with the
        // corresponding element in the second, and 'false' otherwise.  The
        // behavior is undefined unless 'length1' is either unspecified or
        // equals the length of the range '[start1, end1)', and 'length2' is
        // either unspecified or equals the length of the range
        // '[start2, end2)'.  Note that this implementation uses 'operator=='
        // to perform the comparisons, or bit-wise comparison if the value type
        // has the bit-wise equality-comparable trait.  Also note that
        // providing lengths may reduce the runtime cost of this operation.

    template <class INPUT_ITER>
    static int lexicographical(INPUT_ITER start1,
                               INPUT_ITER end1,
                               INPUT_ITER start2,
                               INPUT_ITER end2);
    template <class INPUT_ITER>
    static int lexicographical(INPUT_ITER start1,
                               INPUT_ITER end1,
                               size_type  length1,
                               INPUT_ITER start2,
                               INPUT_ITER end2,
                               size_type  length2);
        // Compare each element in the range beginning at the specified
        // 'start1' position and ending immediately before the specified 'end1'
        // position, to the corresponding element in the range beginning at the
        // specified 'start2' position and ending immediately before the
        // specified 'end2' position.  Optionally specify the length of each
        // range, 'length1' and 'length2'.  Return a negative value if the
        // first range compares lexicographically less than the second range, 0
        // if they are the same length and compare lexicographically equal, and
        // a positive value if the first range compares lexicographically
        // greater than the second range.  The behavior is undefined unless
        // 'length1' is either unspecified or equals the length of the range
        // '[start1, end1)', and 'length2' is either unspecified or equals the
        // length of the range '[start2, end2)'.  Note that this implementation
        // uses 'std::memcmp' for unsigned character comparisons,
        // 'std::wmemcmp' for wide character comparisons, and 'operator<' for
        // all other types.
};

                       // =======================
                       // struct RangeCompare_Imp
                       // =======================

struct RangeCompare_Imp {
    // This utility 'struct' provides the implementations for
    // 'bslalg::RangeCompare'.  Multiple implementations are provided for each
    // method in 'bslalg::RangeCompare', and the most efficient version is
    // found by disambiguating based on the iterator type, the value type, or
    // the presence of nested traits.

    // CLASS METHODS
    template <class VALUE_TYPE>
    static bool equal(const VALUE_TYPE  *start1,
                      const VALUE_TYPE  *end1,
                      const VALUE_TYPE  *start2,
                      const VALUE_TYPE  *end2,
                      const VALUE_TYPE&,
                      bsl::true_type);
    template <class INPUT_ITER, class VALUE_TYPE>
    static bool equal(INPUT_ITER         start1,
                      INPUT_ITER         end1,
                      INPUT_ITER         start2,
                      INPUT_ITER         end2,
                      const VALUE_TYPE&,
                      bsl::false_type);
    template <class INPUT_ITER, class VALUE_TYPE>
    static bool equal(INPUT_ITER         start1,
                      INPUT_ITER         end1,
                      INPUT_ITER         start2,
                      INPUT_ITER         end2,
                      const VALUE_TYPE&);
        // Compare the range beginning at the specified 'start1' position and
        // ending immediately before the specified 'end1' position with the
        // range beginning at the specified 'start2' position and ending
        // immediately before the specified 'end2' position, as if using
        // 'operator==' element-by-element.  The unnamed 'VALUE_TYPE' argument
        // is for automatic type deduction, and is ignored.  The fifth argument
        // is for overloading resolution, and is also ignored.

    template <class INPUT_ITER, class VALUE_TYPE>
    static bool equal(INPUT_ITER        start1,
                      INPUT_ITER        end1,
                      INPUT_ITER        start2,
                      const VALUE_TYPE&,
                      bsl::true_type);
    template <class INPUT_ITER, class VALUE_TYPE>
    static bool equal(INPUT_ITER        start1,
                      INPUT_ITER        end1,
                      INPUT_ITER        start2,
                      const VALUE_TYPE&,
                      bsl::false_type);
    template <class INPUT_ITER, class VALUE_TYPE>
    static bool equal(INPUT_ITER        start1,
                      INPUT_ITER        end1,
                      INPUT_ITER        start2,
                      const VALUE_TYPE&);
        // Compare the range beginning at the specified 'start1' position and
        // ending immediately before the specified 'end1' position with the
        // range beginning at the specified 'start2' position of the same
        // length (namely, 'end1 - start1'), as if using 'operator=='
        // element-by-element.  The unnamed 'VALUE_TYPE' argument is for
        // automatic type deduction, and is ignored.  The fifth argument is for
        // overloading resolution, and is also ignored.

    template <class VALUE_TYPE>
    static bool equalBitwiseEqualityComparable(const VALUE_TYPE  *start1,
                                               const VALUE_TYPE  *end1,
                                               const VALUE_TYPE  *start2,
                                               bsl::true_type);
        // Compare the range beginning at the specified 'start1' position and
        // ending immediately before the specified 'end1' position with the
        // range beginning at the specified 'start2' position of the same
        // length (namely, 'end1 - start1'), using bit-wise comparison across
        // the entire ranges.  The last argument is for removing overload
        // ambiguities, and is not used.  Return 'true' if the ranges are
        // bit-wise equal, and 'false' otherwise.

    template <class INPUT_ITER>
    static bool equalBitwiseEqualityComparable(INPUT_ITER        start1,
                                               INPUT_ITER        end1,
                                               INPUT_ITER        start2,
                                               bsl::false_type);
        // Compare the range beginning at the specified 'start1' position and
        // ending immediately before the specified 'end1' position with the
        // range beginning at the specified 'start2' position of the same
        // length (namely, 'end1 - start1'), using 'operator=='
        // element-by-element.  The last argument is for removing overload
        // ambiguities, and is not used.  Return 'true' if each element in the
        // first range is equal to the corresponding element in the second
        // range, and 'false' otherwise.

    template <class VALUE_TYPE>
    static int lexicographical(const VALUE_TYPE  *start1,
                               const VALUE_TYPE  *end1,
                               const VALUE_TYPE  *start2,
                               const VALUE_TYPE  *end2,
                               const VALUE_TYPE&,
                               bsl::true_type);
        // Compare the range beginning at the specified 'start1' position and
        // ending immediately before the specified 'end1' position with the
        // range beginning at the specified 'start2' position and ending
        // immediately before the specified 'end2' position.  The last two
        // arguments are for removing overload ambiguities and are not used.
        // Return a negative value if the
        // first range compares lexicographically less than the second range, 0
        // if they are the same length and compare lexicographically equal, and
        // a positive value if the first range compares lexicographically
        // greater than the second range.

    template <class INPUT_ITER, class VALUE_TYPE>
    static int lexicographical(INPUT_ITER        start1,
                               INPUT_ITER        end1,
                               INPUT_ITER        start2,
                               INPUT_ITER        end2,
                               const VALUE_TYPE&,
                               bsl::false_type);
        // Compare each element in the range beginning at the specified
        // 'start1' position and ending immediately before the specified 'end1'
        // position with the corresponding element in the range beginning at
        // the specified 'start2' position and ending immediately before the
        // specified 'end2' position using 'operator<'.  The last two arguments
        // are for removing overload ambiguities and are not used.  Return a
        // negative value if the first range compares lexicographically less
        // than the second range, 0 if they are the same length and compare
        // lexicographically equal, and a positive value if the first range
        // compares lexicographically greater than the second range.

    template <class INPUT_ITER, class VALUE_TYPE>
    static int lexicographical(INPUT_ITER        start1,
                               INPUT_ITER        end1,
                               INPUT_ITER        start2,
                               INPUT_ITER        end2,
                               const VALUE_TYPE&);
        // Compare the range beginning at the specified 'start1' position and
        // ending immediately before the specified 'end1' position with the
        // range beginning at the specified 'start2' position and ending
        // immediately before the specified 'end2' position.  The type of the
        // last argument is considered in determining what optimizations, if
        // any, can be applied to the comparison.  The last argument is not
        // used in any other way.  Return a negative value if the
        // first range compares lexicographically less than the second range, 0
        // if they are the same length and compare lexicographically equal, and
        // a positive value if the first range compares lexicographically
        // greater than the second range.

    static int lexicographical(const char *start1,
                               const char *end1,
                               const char *start2);
        // Compare the range beginning at the specified 'start1' position and
        // ending immediately before the specified 'end1' position with the
        // range beginning at the specified 'start2' position of the same
        // length (namely, 'end1 - start1'), using a bit-wise comparison
        // across the entire range, if 'const char' is unsigned, and using
        // 'operator<' otherwise.  Return a negative value if the
        // first range compares lexicographically less than the second range, 0
        // if they are the same length and compare lexicographically equal, and
        // a positive value if the first range compares lexicographically
        // greater than the second range.

    static int lexicographical(const unsigned char *start1,
                               const unsigned char *end1,
                               const unsigned char *start2);
    static int lexicographical(const wchar_t *start1,
                               const wchar_t *end1,
                               const wchar_t *start2);
    template <class INPUT_ITER>
    static int lexicographical(INPUT_ITER           start1,
                               INPUT_ITER           end1,
                               INPUT_ITER           start2,
                               bslmf::MatchAnyType);
    template <class INPUT_ITER>
    static int lexicographical(INPUT_ITER     start1,
                               INPUT_ITER     end1,
                               INPUT_ITER     start2);
        // Compare each element in the range beginning at the specified
        // 'start1' position and ending immediately before the specified 'end1'
        // position with the corresponding element in the range of the same
        // length beginning at the specified 'start2' position.  Return a
        // negative value if the first range compares lexicographically less
        // than the second range, 0 if they are the same length and compare
        // lexicographically equal, and a positive value if the first range
        // compares lexicographically greater than the second range.
};

// ============================================================================
//                  INLINE AND TEMPLATE FUNCTION DEFINITIONS
// ============================================================================

                         // -------------------
                         // struct RangeCompare
                         // -------------------

// CLASS METHODS
template <class INPUT_ITER>
inline
bool RangeCompare::equal(INPUT_ITER start1,
                         INPUT_ITER end1,
                         INPUT_ITER start2)
{
    if (start1 == end1) {
        return true;                                                  // RETURN
    }
    return RangeCompare_Imp::equal(start1, end1, start2, *start1);
}

template <class INPUT_ITER>
inline
bool RangeCompare::equal(INPUT_ITER start1,
                         INPUT_ITER end1,
                         INPUT_ITER start2,
                         INPUT_ITER end2)
{
    if (start1 == end1) {
        return start2 == end2;                                        // RETURN
    }
    return RangeCompare_Imp::equal(start1, end1, start2, end2, *start1);
}

template <class INPUT_ITER>
inline
bool RangeCompare::equal(INPUT_ITER start1,
                         INPUT_ITER end1,
                         size_type  length1,
                         INPUT_ITER start2,
                         INPUT_ITER,
                         size_type  length2)
{
    if (length1 != length2) {
        return false;                                                 // RETURN
    }
    if (start1 == end1) {
        return true;                                                  // RETURN
    }
    return RangeCompare_Imp::equal(start1, end1, start2, *start1);
}

template <class INPUT_ITER>
int RangeCompare::lexicographical(INPUT_ITER start1,
                                  INPUT_ITER end1,
                                  INPUT_ITER start2,
                                  INPUT_ITER end2)
{
    if (start1 == end1) {
        return start2 != end2 ? -1 : 0;                               // RETURN
    }
    return RangeCompare_Imp::lexicographical(start1,
                                             end1,
                                             start2,
                                             end2,
                                             *start1);
}

template <class INPUT_ITER>
int RangeCompare::lexicographical(INPUT_ITER start1,
                                  INPUT_ITER end1,
                                  size_type  length1,
                                  INPUT_ITER start2,
                                  INPUT_ITER end2,
                                  size_type  length2)
{
    const int result = length2 < length1
                       ? - RangeCompare_Imp::lexicographical(start2,
                                                             end2,
                                                             start1)
                       :   RangeCompare_Imp::lexicographical(start1,
                                                             end1,
                                                             start2);

    if (result < 0) {
        return -1;                                                    // RETURN
    }
    if (0 < result) {
        return 1;                                                     // RETURN
    }
    if (length1 < length2) {
        return -1;                                                    // RETURN
    }
    if (length2 < length1) {
        return 1;                                                     // RETURN
    }
    return 0;
}

                       // -----------------------
                       // struct RangeCompare_Imp
                       // -----------------------

// CLASS METHODS

                          // *** equal overloads: ***

template <class VALUE_TYPE>
inline
bool RangeCompare_Imp::equal(const VALUE_TYPE  *start1,
                             const VALUE_TYPE  *end1,
                             const VALUE_TYPE  *start2,
                             const VALUE_TYPE  *end2,
                             const VALUE_TYPE&,
                             bsl::true_type)
{
    return RangeCompare::equal(start1,
                               end1,
                               end1 - start1,
                               start2,
                               end2,
                               end2 - start2);
}

template <class INPUT_ITER, class VALUE_TYPE>
bool RangeCompare_Imp::equal(INPUT_ITER        start1,
                             INPUT_ITER        end1,
                             INPUT_ITER        start2,
                             INPUT_ITER        end2,
                             const VALUE_TYPE&,
                             bsl::false_type)
{
    for ( ; start1 != end1 && start2 != end2; ++start1, ++start2) {
        if (!(*start1 == *start2)) {
            return false;                                             // RETURN
        }
    }
    return start1 == end1 && start2 == end2;
}

template <class INPUT_ITER, class VALUE_TYPE>
bool RangeCompare_Imp::equal(INPUT_ITER        start1,
                             INPUT_ITER        end1,
                             INPUT_ITER        start2,
                             INPUT_ITER        end2,
                             const VALUE_TYPE& value)
{
    typedef typename bsl::is_convertible<INPUT_ITER, const VALUE_TYPE *>::Type
                                                      CanUseLengthOptimization;

    return equal(start1,
                 end1,
                 start2,
                 end2,
                 value,
                 CanUseLengthOptimization());
}

template <class INPUT_ITER, class VALUE_TYPE>
inline
bool RangeCompare_Imp::equal(INPUT_ITER        start1,
                             INPUT_ITER        end1,
                             INPUT_ITER        start2,
                             const VALUE_TYPE&,
                             bsl::true_type)
{
    // Note: We are forced to call a different function to resolve whether
    // 'INPUT_ITER' is convertible to 'const TARGET_TYPE *' or not, otherwise
    // we would be introducing ambiguities (the additional parameter
    // 'CanUseBitwiseCopyOptimization' is necessary to remove further
    // ambiguities on SunPro).

    typedef typename bsl::is_convertible<INPUT_ITER, const VALUE_TYPE *>::Type
                                              CanUseBitwiseCompareOptimization;

    return equalBitwiseEqualityComparable(start1,
                                          end1,
                                          start2,
                                          CanUseBitwiseCompareOptimization());
}

template <class INPUT_ITER, class VALUE_TYPE>
bool RangeCompare_Imp::equal(INPUT_ITER        start1,
                             INPUT_ITER        end1,
                             INPUT_ITER        start2,
                             const VALUE_TYPE&,
                             bsl::false_type)
{
    for ( ; start1 != end1; ++start1, ++start2) {
        if (!(*start1 == *start2)) {
            return false;                                             // RETURN
        }
    }
    return true;
}

template <class INPUT_ITER, class VALUE_TYPE>
inline
bool RangeCompare_Imp::equal(INPUT_ITER        start1,
                             INPUT_ITER        end1,
                             INPUT_ITER        start2,
                             const VALUE_TYPE& value)
{
    typedef typename
        bslmf::IsBitwiseEqualityComparable<VALUE_TYPE>::type Trait;
    return equal(start1, end1, start2, value, Trait());
}

             // *** equalBitwiseEqualityComparable overloads: ***

template <class VALUE_TYPE>
inline
bool RangeCompare_Imp::equalBitwiseEqualityComparable(
                                                     const VALUE_TYPE  *start1,
                                                     const VALUE_TYPE  *end1,
                                                     const VALUE_TYPE  *start2,
                                                     bsl::true_type)
{
    std::size_t numBytes = reinterpret_cast<const char *>(end1)
                         - reinterpret_cast<const char *>(start1);

    return 0 == std::memcmp(reinterpret_cast<const void *>(start1),
                            reinterpret_cast<const void *>(start2),
                            numBytes);
}

template <class INPUT_ITER>
inline
bool RangeCompare_Imp::equalBitwiseEqualityComparable(INPUT_ITER        start1,
                                                      INPUT_ITER        end1,
                                                      INPUT_ITER        start2,
                                                      bsl::false_type)
{
    // We can't be as optimized as above.

    return equal(start1, end1, start2, *start1, bsl::false_type());
}

                     // *** lexicographical overloads: ***

template <class VALUE_TYPE>
inline
int RangeCompare_Imp::lexicographical(const VALUE_TYPE  *start1,
                                      const VALUE_TYPE  *end1,
                                      const VALUE_TYPE  *start2,
                                      const VALUE_TYPE  *end2,
                                      const VALUE_TYPE&,
                                      bsl::true_type)
{
    // In this case, we can compute the length directly, and avoid the overhead
    // of the two comparisons in the loop condition (one is enough).

    return RangeCompare::lexicographical(start1,
                                         end1,
                                         end1 - start1,
                                         start2,
                                         end2,
                                         end2 - start2);
}

template <class INPUT_ITER, class VALUE_TYPE>
int RangeCompare_Imp::lexicographical(INPUT_ITER        start1,
                                      INPUT_ITER        end1,
                                      INPUT_ITER        start2,
                                      INPUT_ITER        end2,
                                      const VALUE_TYPE&,
                                      const bsl::false_type)
{
    for ( ; start1 != end1 && start2 != end2; ++start1, ++start2) {
        if (*start1 < *start2) {
            return -1;                                                // RETURN
        }
        else if (*start2 < *start1) {
            return 1;                                                 // RETURN
        }
    }
    if (start1 != end1) {
        return 1;                                                     // RETURN
    }
    if (start2 != end2) {
        return -1;                                                    // RETURN
    }
    return 0;
}

template <class INPUT_ITER, class VALUE_TYPE>
inline
int RangeCompare_Imp::lexicographical(INPUT_ITER        start1,
                                      INPUT_ITER        end1,
                                      INPUT_ITER        start2,
                                      INPUT_ITER        end2,
                                      const VALUE_TYPE& value)
{
    typedef typename bsl::is_convertible<INPUT_ITER, const VALUE_TYPE *>::Type
                                                      CanUseLengthOptimization;

    return lexicographical(start1, end1, start2, end2, value,
                           CanUseLengthOptimization());
}

inline
int RangeCompare_Imp::lexicographical(const unsigned char *start1,
                                      const unsigned char *end1,
                                      const unsigned char *start2)
{
    return std::memcmp(start1, start2, end1 - start1);
}

inline
int RangeCompare_Imp::lexicographical(const char *start1,
                                      const char *end1,
                                      const char *start2)
{
#if CHAR_MAX == SCHAR_MAX
    return std::memcmp(start1, start2, (end1 - start1));
#else
    return lexicographical<const char *>(start1, end1, start2, 0);
#endif
}

inline
int RangeCompare_Imp::lexicographical(const wchar_t *start1,
                                      const wchar_t *end1,
                                      const wchar_t *start2)
{
    return std::wmemcmp(start1, start2, end1 - start1);
}

template <class INPUT_ITER>
int RangeCompare_Imp::lexicographical(INPUT_ITER start1,
                                      INPUT_ITER end1,
                                      INPUT_ITER start2,
                                      bslmf::MatchAnyType)
{
    for ( ; start1 != end1; ++start1, ++start2) {
        if (*start1 < *start2) {
            return -1;                                                // RETURN
        }
        else if (*start2 < *start1) {
            return 1;                                                 // RETURN
        }
    }
    return 0;
}

template <class INPUT_ITER>
inline
int RangeCompare_Imp::lexicographical(INPUT_ITER start1,
                                      INPUT_ITER end1,
                                      INPUT_ITER start2)
{
    if (start1 != end1) {
        return lexicographical(start1, end1, start2, *start1);        // RETURN
    }
    return 0;
}

}  // close package namespace


}  // close enterprise namespace

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
