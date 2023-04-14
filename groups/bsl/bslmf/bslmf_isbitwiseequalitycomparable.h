// bslmf_isbitwiseequalitycomparable.h                                -*-C++-*-
#ifndef INCLUDED_BSLMF_ISBITWISEEQUALITYCOMPARABLE
#define INCLUDED_BSLMF_ISBITWISEEQUALITYCOMPARABLE

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a type trait for bitwise equality.
//
//@CLASSES:
//  bslmf::IsBitwiseEqualityComparable: trait metafunction
//
//@DESCRIPTION: This component provides a single trait metafunction,
// 'bslmf::IsBitwiseEqualityComparable', which allows generic code to determine
// whether objects of the specified 't_TYPE' can be compared using 'memcmp'.
// Such types are said to be bitwise EqualityComparable.  Observe that this
// trait may be 'true' only for object types, even though, for example,
// reference types may be guaranteed identical bit representations when they
// refer to the same object, just as the corresponding pointer type would be
// bitwise EqualityComparable.
//
// For a type to be bitwise EqalityComparable, each bit of its object
// representation must be significant in the value representation, and distinct
// sequences of bits represent different values, i.e., this trait is an
// assertion that the specified 't_TYPE' has unique representations for each
// possible value, and no padding bits.  This property is deemed to hold for
// 'bool' and enumerations where, in practice, the compiler will enforce a
// value representation over all the seemingly unused bits.  For a C++17 tool
// chain, this trait should be equivalent to the
// 'std::has_unique_object_representation' trait.
//
// Note that as arrays are not allowed to introduce padding, arrays of a
// bitwise EqualityComparable 't_TYPE' are also bitwise EqualityComparable,
// even though they do not provide an overloaded 'operator=='.  While
// transforming comparisons of a single object using this trait into calls to
// 'memcmp' is unlikely to be profitable, transforming comparisons of a whole
// array into a single 'memcmp' call is more likely to be beneficial.
//
///Usage
///-----
//
///Example 1: Using the trait to optimize range comparison
///- - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose we want to compare two sequences of the same object type to
// determine whether or not they hold the same values.  The simplest solution
// would be to iterate over both sequences, comparing each member, and return
// 'false' as soon as any pair of elements do not compare equal; if we walk all
// the way to the end of both sequences, then they hold the same values.  If we
// want to perform this comparison most efficiently though, we would rather not
// invoke 'operator==' on each member, and instead defer to the 'memcmp'
// function in the standard library that is highly optimized (often to take
// advantage of platform-specific instructions) for comparing ranges of raw
// memory.  We can switch to this other technique only if we know that the
// value representations of a type are unique, rely on all of the bits in their
// representation, and do not have strange values like 'NaN' that self-compare
// as 'false'.  This property is denoted by the 'IsBitwiseEqualityComparable'
// trait.
//
// First, we create a simple 'struct' that contains a 'char' and a 'short' as
// its two data members, and supported comparison with 'operator=='.  Note that
// there will be a byte of padding between the 'char' and the 'short' members
// to ensure proper alignment.  We insert telemetry to count the number of
// times 'operator==' is called:
//..
//  namespace BloombergLP {
//
//  struct SimpleType {
//      // This 'struct' holds two data members with a byte of padding, and can
//      // be compared using the overloaded 'operator=='.
//
//      char  d_dataC;
//      short d_dataS;
//
//      static int s_comparisons;
//
//      friend bool operator==(const SimpleType& a, const SimpleType& b)
//          // Return 'true' if the specified 'a' has the same value as the
//          // specified 'b'.  Two 'SimpleType' objects have the same value if
//          // their corresponding 'd_dataC' and 'd_dataS' members have the
//          // same value.  The static data member 's_comparisons' is
//          // incremented by one each time this function is called.
//      {
//          ++s_comparisons;
//          return a.d_dataC == b.d_dataC
//              && a.d_dataS == b.d_dataS;
//      }
//
//      friend bool operator!=(const SimpleType& a, const SimpleType& b)
//          // Return 'true' if the specified 'a' does not have the same value
//          // as the specified 'b'.  Two 'SimpleType' objects do not have the
//          // same value if their corresponding 'd_dataC' and 'd_dataS'
//          // members do not have the same value.  The static data member
//          // 's_comparisons' is incremented by one each time this function is
//          // called.
//      {
//          ++s_comparisons;
//          return a.d_dataC != b.d_dataC
//              || a.d_dataS != b.d_dataS;
//      }
//  };
//
//  int SimpleType::s_comparisons = 0;
//..
// Then, we create another 'struct' that wraps a single 'int' as its only data
// member, and supports comparison with 'operator==', inserting telemetry to
// count the number of times 'operator==' is called:
//..
//  struct SecondType {
//      // This 'struct' holds a single 'int' member, 'd_data', and can be
//      // compared using the overloaded 'operator=='.
//..
// We associate the bitwise EqualityComparable trait with 'SecondType' using
// the BDE nested trait declaration facility:
//..
//      BSLMF_NESTED_TRAIT_DECLARATION(SecondType,
//                                     bslmf::IsBitwiseEqualityComparable);
//
//      int d_data;
//
//      static int s_comparisons;
//
//      friend bool operator==(const SecondType& a, const SecondType& b)
//          // Return 'true' if the specified 'a' has the same value as the
//          // specified 'b'.  Two 'SecondType' objects have the same value if
//          // their corresponding 'd_data' elements have the same value.  The
//          // static data member 's_comparisons' is incremented by one each
//          // time this function is called.
//      {
//          ++s_comparisons;
//          return a.d_data == b.d_data;
//      }
//
//      friend bool operator!=(const SecondType& a, const SecondType& b)
//          // Return 'true' if the specified 'a' does not have the same value
//          // as the specified 'b'.  Two 'SecondType' objects do not have the
//          // same value if their corresponding 'd_data' elements do not have
//          // the same value.  The static data member 's_comparisons' is
//          // incremented by one each time this function is called.
//      {
//          ++s_comparisons;
//          return a.d_data != b.d_data;
//      }
//  };
//
//  int SecondType::s_comparisons = 0;
//..
// Next, we create another 'struct' that wraps a single 'int' as its only data
// member, and supports comparison with 'operator==', inserting telemetry to
// count the number of times 'operator==' is called:
//..
//  struct ThirdType {
//      // This 'struct' holds a single 'int' member, 'd_data', and can be
//      // compared using the overloaded 'operator=='.
//
//      int d_data;
//
//      static int s_comparisons;
//
//      friend bool operator==(const ThirdType& a, const ThirdType& b)
//          // Return 'true' if the specified 'a' has the same value as the
//          // specified 'b'.  Two 'SecondType' objects have the same value if
//          // their corresponding 'd_data' elements have the same value.  The
//          // static data member 's_comparisons' is incremented by one each
//          // time this function is called.
//      {
//          ++s_comparisons;
//          return a.d_data == b.d_data;
//      }
//
//      friend bool operator!=(const ThirdType& a, const ThirdType& b)
//          // Return 'true' if the specified 'a' does not have the same value
//          // as the specified 'b'.  Two 'ThirdType' objects do not have the
//          // same value if their corresponding 'd_data' elements do not have
//          // the same value.  The static data member 's_comparisons' is
//          // incremented by one each time this function is called.
//      {
//          ++s_comparisons;
//          return a.d_data != b.d_data;
//      }
//  };
//
//  int ThirdType::s_comparisons = 0;
//..
// We associate the bitwise EqualityComparable trait with 'ThirdType' by
// explicitly specializing the trait:
//..
//  namespace bslmf {
//      template <>
//      struct IsBitwiseEqualityComparable<ThirdType> : bsl::true_type {};
//  }  // close namespace bslmf
//..
// Now, we write a function template to compare two arrays of the same type:
//..
//  template <class t_TYPE>
//  bool rangeCompare(const t_TYPE *start, size_t length, const t_TYPE *other)
//  {
//..
// If we detect the bitwise EqualityComparable trait, we rely on the optimized
// 'memcmp' function:
//..
//      if (bslmf::IsBitwiseEqualityComparable<t_TYPE>::value) {
//          return 0 == memcmp(start,
//                             other,
//                             length * sizeof(t_TYPE));              // RETURN
//      }
//..
// Otherwise we iterate over the range directly until we find a pair of
// elements that do not have the same value, and return 'true' if we reach the
// end of the range.
//..
//      if (0 != length)  {
//          while (*start++ == *other++) {
//              if (!--length) {
//                  return true;                                      // RETURN
//              }
//          }
//      }
//      return false;
//  }
//..
// Finally, we write a test to confirm that two arrays containing different
// values do not compare as equal (using our array comparison function), and
// that an array compares equal to itself, as it does comprise elements all
// having the same value.  By inspecting the static data members provided for
// telemetry, we can confirm that 'operator==' is called only for 'SimpleType'
// as the other two 'struct's dispatch to 'memcmp' instead:
//..
//  int usageExample1()
//  {
//..
// We confirm the initial state of the telemetry:
//..
//      assert(0 == SimpleType::s_comparisons);
//      assert(0 == SecondType::s_comparisons);
//      assert(0 == ThirdType ::s_comparisons);
//..
// Then we create zero-initialized arrays for each of the types to be tested,
// and a second array for each type with a set of values distinct from all
// zeroes:
//..
//      const SimpleType simpleZeroes[10] = { };
//      const SecondType secondZeroes[10] = { };
//      const ThirdType  thirdZeroes [10] = { };
//
//      const SimpleType simpleValues[10] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
//      const SecondType secondValues[10] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
//      const ThirdType  thirdValues [10] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
//..
// Next we confirm that the two arrays (of each type) do not compare equal, and
// inspect the telemetry to confirm that the comparison operator was called for
// only the 'SimpleType' without the bitwise EqualityComparable trait:
//..
//      assert(!rangeCompare(simpleZeroes, 10u, simpleValues) );
//      assert(!rangeCompare(secondZeroes, 10u, secondValues) );
//      assert(!rangeCompare(thirdZeroes,  10u, thirdValues)  );
//
//      assert(0 <  SimpleType::s_comparisons);
//      assert(0 == SecondType::s_comparisons);
//      assert(0 == ThirdType ::s_comparisons);
//
//..
// Then we reset the telemetry and confirm that an array of each type compares
// equal to itself, and inspect the telemetry to confirm that the comparison
// operator was called for only the 'SimpleType' without the bitwise
// EqualityComparable trait:
//..
//      SimpleType::s_comparisons = 0;
//
//      assert( rangeCompare(simpleValues, 10u, simpleValues) );
//      assert( rangeCompare(secondValues, 10u, secondValues) );
//      assert( rangeCompare(thirdZeroes,  10u, thirdZeroes)  );
//
//      assert(0 <  SimpleType::s_comparisons);
//      assert(0 == SecondType::s_comparisons);
//      assert(0 == ThirdType ::s_comparisons);
//
//      return 0;
//  }
//
//  }  // close enterprise namespace
//..
//
///Example 2: Associating a Trait with a Class Template
/// - - - - - - - - - - - - - - - - - - - - - - - - - -
// In this example, we associate a trait not with a class, but with a class
// *template*.  We create a "control" template that is not bitwise
// EqualityComparable, and two class templates, each of which uses a different
// mechanisms for being associated with the 'IsBitwiseEqualityComparable'
// trait.  First, we define a class template that is not bitwise
// EqualityComparable, 'NotComparable':
//..
//  namespace BloombergLP {
//
//  template <class t_TYPE>
//  struct NotComparable
//  {
//      t_TYPE d_value;
//  };
//..
// Then, we define the class template 'PotentiallyComparable1', which uses
// partial template specialization to associate the
// 'IsBitwiseEqualityComparable' trait with each instantiation on a 't_TYPE'
// that is itself bitwise EqualityComparable:
//..
//  template <class t_TYPE>
//  struct PotentiallyComparable1
//  {
//      t_TYPE d_value;
//  };
//
//  namespace bslmf {
//      template <class t_TYPE>
//      struct IsBitwiseEqualityComparable<PotentiallyComparable1<t_TYPE> >
//          : IsBitwiseEqualityComparable<t_TYPE>::type {
//      };
//  }  // close namespace bslmf
//..
// Next, we define the class template'PotentiallyComparable2', which uses the
// 'BSLMF_NESTED_TRAIT_DECLARATION' macro to associate the
// 'IsBitwiseEqualityComparable' trait with each instantiation on a 't_TYPE'
// that is itself bitwise EqualityComparable:
//..
//  template <class t_TYPE>
//  struct PotentiallyComparable2
//  {
//      t_TYPE d_value;
//
//      BSLMF_NESTED_TRAIT_DECLARATION_IF(
//                          PotentiallyComparable2,
//                          bslmf::IsBitwiseEqualityComparable,
//                          bslmf::IsBitwiseEqualityComparable<t_TYPE>::value);
//  };
//..
// Finally, we check that the traits are correctly associated by instantiating
// each template with types that are bitwise EqualityComparable and with types
// that are not not bitwise EqualityComparable, verifying the value of
// 'IsBitwiseEqualityComparable<T>::value' in each case:
//..
//  int usageExample2()
//  {
//      using namespace bslmf;
//
//      assert(!IsBitwiseEqualityComparable<NotComparable<int> >::value);
//      assert(!IsBitwiseEqualityComparable<
//                            NotComparable<NotComparable<int> > >::value);
//
//      assert( IsBitwiseEqualityComparable<
//                                 PotentiallyComparable1<int> >::value);
//      assert(!IsBitwiseEqualityComparable<
//                   PotentiallyComparable1<NotComparable<int> > >::value);
//
//      assert( IsBitwiseEqualityComparable<
//                                 PotentiallyComparable2<int> >::value);
//      assert(!IsBitwiseEqualityComparable<
//                   PotentiallyComparable2<NotComparable<int> > >::value);
//
//      return 0;
//  }
//
//  }  // close enterprise namespace
//..

#include <bslscm_version.h>

#include <bslmf_detectnestedtrait.h>
#include <bslmf_integralconstant.h>
#include <bslmf_isconst.h>
#include <bslmf_voidtype.h>

#include <bsls_platform.h>

#include <stddef.h>

#ifndef BDE_DONT_ALLOW_TRANSITIVE_INCLUDES
#include <bslmf_isenum.h>
#endif


namespace BloombergLP {
namespace bslmf {

template <class t_TYPE>
struct IsBitwiseEqualityComparable;

#if defined(BSLS_PLATFORM_CMP_IBM)
                     // =========================================
                     // struct IsBitwiseEqualityComparable_Scalar
                     // =========================================

template <class t_TYPE, class = void>
struct IsBitwiseEqualityComparable_Imp2 : bsl::false_type {
};
template <class t_TYPE>
struct IsBitwiseEqualityComparable_Imp2<t_TYPE, BSLMF_VOIDTYPE(t_TYPE[])>
: bsl::true_type {
    // This implementation-detail trait determines whether 't_TYPE' is a scalar
    // type (an arithmetic type, enumeration, pointer, or pointer-to-member).
    // This implementation takes advantage of a previous layer of filtering
    // handling all class-types, so any remaining types that are valid as array
    // elements must be scalar types, i.e., the 'BSLMF_VOIDTYPE' test will
    // filter function types and reference types.
};

                     // ======================================
                     // struct IsBitwiseEqualityComparable_Imp
                     // ======================================

template <class t_TYPE, class = void>
struct IsBitwiseEqualityComparable_Imp
: IsBitwiseEqualityComparable_Imp2<t_TYPE>::type {
};
template <class t_TYPE>
struct IsBitwiseEqualityComparable_Imp<t_TYPE, BSLMF_VOIDTYPE(int t_TYPE::*)>
: DetectNestedTrait<t_TYPE, IsBitwiseEqualityComparable>::type {
    // This trait 'struct' derives from 'bsl::true_type' if (the template
    // paramter) 't_TYPE' is a scalar type or a class with a nested trait
    // declaration for the 'bslmf::IsBitwiseEqualityComparable' trait, and from
    // 'bsl::false_type' otherwise.  Note that this template handles only the
    // class-type cases, delegating the final filtering for scalar types to a
    // further trait template.
};
#else
                     // ======================================
                     // struct IsBitwiseEqualityComparable_Imp
                     // ======================================

template <class t_TYPE, class = void>
struct IsBitwiseEqualityComparable_Imp : bsl::is_const<const t_TYPE>::type {
};
template <class t_TYPE>
struct IsBitwiseEqualityComparable_Imp<t_TYPE, BSLMF_VOIDTYPE(int t_TYPE::*)>
: DetectNestedTrait<t_TYPE, IsBitwiseEqualityComparable>::type {
    // This trait 'struct' derives from 'bsl::true_type' if (the template
    // paramter) 't_TYPE' is a scalar type or a class with a nested trait
    // declaration for the 'bslmf::IsBitwiseEqualityComparable' trait, and from
    // 'bsl::false_type' otherwise.  Note that this implementation relies on
    // the fact that reference and function types cannot be cv-qualified, and
    // any other non-class type (that is not handled by specializations of the
    // primary template) will be a scalar type.
};
#endif

                     // ==================================
                     // struct IsBitwiseEqualityComparable
                     // ==================================

template <class t_TYPE>
struct IsBitwiseEqualityComparable
: IsBitwiseEqualityComparable_Imp<t_TYPE>::type {
    // This trait 'struct' is a metafunction that determines whether the
    // specified parameter 't_TYPE' is bitwise EqualityComparable.  If
    // 'IsBitwiseEqualityComparable<t_TYPE>' is derived from 'true_type' then
    // 't_TYPE' is bitwise EqualityComparable.  Otherwise, bitwise equality
    // comparability cannot be inferred for 't_TYPE'.  This trait can be
    // associated with a bitwise EqualityComparable user-defined class by
    // specializing this class or by using the 'BSLMF_NESTED_TRAIT_DECLARATION'
    // macro.
};

template <class t_TYPE>
struct IsBitwiseEqualityComparable<const t_TYPE>
: IsBitwiseEqualityComparable<t_TYPE>::type {
};
template <class t_TYPE>
struct IsBitwiseEqualityComparable<volatile t_TYPE>
: IsBitwiseEqualityComparable<t_TYPE>::type {
};
template <class t_TYPE>
struct IsBitwiseEqualityComparable<const volatile t_TYPE>
: IsBitwiseEqualityComparable<t_TYPE>::type {
};
    // Partial specializations for cv-qualified types channel to a single
    // instantiation of the implementation type.  Note that we cannot derive
    // through the 'Imp' type directly as we would not correctly handle
    // cv-qualified types that have been explicitly specialized by our users.

template <class t_TYPE, size_t t_LEN>
struct IsBitwiseEqualityComparable<t_TYPE[t_LEN]>
: IsBitwiseEqualityComparable<t_TYPE>::type {
};
template <class t_TYPE, size_t t_LEN>
struct IsBitwiseEqualityComparable<const t_TYPE[t_LEN]>
: IsBitwiseEqualityComparable<t_TYPE>::type {
};
template <class t_TYPE, size_t t_LEN>
struct IsBitwiseEqualityComparable<volatile t_TYPE[t_LEN]>
: IsBitwiseEqualityComparable<t_TYPE>::type {
};
template <class t_TYPE, size_t t_LEN>
struct IsBitwiseEqualityComparable<const volatile t_TYPE[t_LEN]>
: IsBitwiseEqualityComparable<t_TYPE>::type {
};

template <class t_TYPE>
struct IsBitwiseEqualityComparable<t_TYPE[]>
: IsBitwiseEqualityComparable<t_TYPE>::type {
};
template <class t_TYPE>
struct IsBitwiseEqualityComparable<const t_TYPE[]>
: IsBitwiseEqualityComparable<t_TYPE>::type {
};
template <class t_TYPE>
struct IsBitwiseEqualityComparable<volatile t_TYPE[]>
: IsBitwiseEqualityComparable<t_TYPE>::type {
};
template <class t_TYPE>
struct IsBitwiseEqualityComparable<const volatile t_TYPE[]>
: IsBitwiseEqualityComparable<t_TYPE>::type {
};
    // Partial specializations for array types, as arrays have a contiguity
    // guarantee (that can inferred from pointer arithmetic rules) so this if a
    // type is bitwise equality comparable, we can be sure there is no padding
    // introduced by an array, so arrays of this type should have the same
    // property.

template <>
struct IsBitwiseEqualityComparable<void> : bsl::false_type {};
    // Explicit specialization to confirm that 'void' types are never bitwise
    // EqualityComparable.  Note that cv-'void' types are covered by the
    // partial specialization for any cv-qualified type.

template <>
struct IsBitwiseEqualityComparable<float> : bsl::true_type {};
template <>
struct IsBitwiseEqualityComparable<double> : bsl::true_type {};
template <>
struct IsBitwiseEqualityComparable<long double> : bsl::true_type {};
    // Revert of {DRQS 143286899}. Once clients are fixed, change to false.
    // Explicit specialization to confirm that floating point types are not
    // bitwise EqualityComparable, as they typically have specific problematic
    // values: NaNs do not compare equal with themselves, and there may be
    // multiple representations for zero (with negative zero).

}  // close package namespace
}  // close enterprise namespace

#endif // ! defined(INCLUDED_BSLMF_ISBITWISEEQUALITYCOMPARABLE)

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
