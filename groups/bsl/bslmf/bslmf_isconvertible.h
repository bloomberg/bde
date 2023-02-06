// bslmf_isconvertible.h                                              -*-C++-*-
#ifndef INCLUDED_BSLMF_ISCONVERTIBLE
#define INCLUDED_BSLMF_ISCONVERTIBLE

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a compile-time check for type conversion.
//
//@CLASSES:
//  bsl::is_convertible: standard meta-function for type conversion checking
//  bsl::is_convertible_v: the result value of 'bsl::is_convertible'
//  bslmf::IsConvertible: meta-function for type conversion checking
//
//@SEE_ALSO: bslmf_integralconstant
//
//@DESCRIPTION: This component defines two meta-functions,
// 'bsl::is_convertible' and 'BloombergLP::bslmf::IsConvertible' and a template
// variable 'bsl::is_convertible_v', that represents the result value of the
// 'bsl::is_convertible' meta-function.  All these meta-functions may be used
// to check whether an implicit conversion exists from one type to another.
//
// 'bsl::is_convertible' meets the requirements of the 'is_convertible'
// template defined in the C++11 standard [meta.rel], while
// 'bslmf::IsConvertible' was devised before 'is_convertible' was standardized.
//
// The two meta-functions are functionally equivalent except that
// 'bsl::is_convertible' does not allow its template parameter types to be
// incomplete types according to the C++11 standard while
// 'bslmf::IsConvertible' tests conversions involving incomplete types.  The
// other major difference between them is that the result for
// 'bsl::is_convertible' is indicated by the class members 'value' and 'type',
// whereas the result for 'bslmf::IsConvertible' is indicated by the class
// members 'VALUE' and 'Type'.
//
// Note that 'bsl::is_convertible' should be preferred over
// 'bslmf::IsConvertible', and in general, should be used by new components.
// Also note that 'bsl::is_convertible' and 'bslmf::IsConvertible' can produce
// compiler errors if the conversion is ambiguous.  For example:
//..
//  struct A {};
//  struct B : public A {};
//  struct C : public A {};
//  struct D : public B, public C {};
//
//  static int const C = bsl::is_convertible<D*, A*>::value;  // ERROR!
//..
// Also note that the template variable 'is_convertible_v' is defined in the
// C++17 standard as an inline variable.  If the current compiler supports the
// inline variable C++17 compiler feature, 'bsl::is_convertible_v' is defined
// as an 'inline constexpr bool' variable.  Otherwise, if the compiler supports
// the variable templates C++14 compiler feature, 'bsl::is_convertible_v' is
// defined as a non-inline 'constexpr bool' variable.  See
// 'BSLS_COMPILERFEATURES_SUPPORT_INLINE_VARIABLES' and
// 'BSLS_COMPILERFEATURES_SUPPORT_VARIABLE_TEMPLATES' macros in
// bsls_compilerfeatures component for details.
//
///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1: Select Function Based on Type Convertibility
///- - - - - - - - - - - - - - - - - - - - - - - - - - - -
// The 'bsl::is_convertible' meta-function can be used to select an appropriate
// function (at compile time) based on the convertibility of one type to
// another without causing a compiler error by actually trying the conversion.
//
// First, we define two classes, 'Foo' and 'Bar'.  The 'Foo' class has an
// explict constructor from 'int', an implicit conversion operator that returns
// an integer value while the 'Bar' class does neither:
//..
//  class Foo {
//      // DATA
//      int d_value;
//
//    public:
//      // CREATORS
//      explicit Foo(int value) : d_value(value) {}
//
//      // ACCESSORS
//      operator int() const { return d_value; }
//  };
//
//  class Bar {};
//..
// Then, we run:
//..
//  assert(false == (bsl::is_convertible<int, Foo>::value));
//  assert(false == (bsl::is_convertible<int, Bar>::value));
//
//  assert(true  == (bsl::is_convertible<Foo, int>::value));
//  assert(false == (bsl::is_convertible<Bar, int>::value));
//..
// Note that 'int' to 'Foo' is false, even though 'Foo' has a constructor that
// takes an 'int'.  This is because that constructor is explicit, and
// 'is_converitble' ignores explicit constructors.
//
// Next, we go on to demonstrate how this could be used.  Suppose we are
// implementing a 'convertToInt' template method that converts a given object
// of the (template parameter) 't_TYPE' to 'int' type, and returns the integer
// value.  If the given object can not convert to 'int', return 0.  The method
// calls an overloaded function, 'getIntValue', to get the converted integer
// value.  The idea is to invoke one version of 'getIntValue' if the type
// provides a conversion operator that returns an integer value, and another
// version if the type does not provide such an operator.
//
// We define the first 'getIntValue' function that takes a 'bsl::false_type' as
// its last argument, whereas the second 'getIntValue' function takes a
// 'bsl::true_type' object.  The result of the 'bsl::is_convertible'
// meta-function (i.e., its 'type' member) is used to create the last argument
// passed to 'getIntValue'.  Neither version of 'getIntValue' makes use of this
// argument -- it is used only to differentiate the argument list so we can
// overload the function.
//..
//  template <class t_TYPE>
//  inline
//  int getIntValue(t_TYPE *, bsl::false_type)
//  {
//      // Return 0 because the specified 't_TYPE' is not convertible to the
//      // 'int' type.
//
//      return 0;
//  }
//
//  template <class t_TYPE>
//  inline
//  int getIntValue(t_TYPE *object, bsl::true_type)
//  {
//      // Return the integer value converted from the specified 'object' of
//      // the (template parameter) 't_TYPE'.
//
//      return int(*object);
//  }
//..
// Now, we define our 'convertToInt' method:
//..
//  template <class t_TYPE>
//  inline
//  int convertToInt(t_TYPE *object)
//  {
//      typedef typename bsl::is_convertible<t_TYPE,
//                                           int>::type CanConvertToInt;
//      return getIntValue(object, CanConvertToInt());
//  }
//..
// Notice that we use 'bsl::is_convertible' to get a 'bsl::false_type' or
// 'bsl::true_type', and then call the corresponding overloaded 'getIntValue'
// method.
//
// Finally, we call our finished product and observe the return values:
//..
//  Foo foo(99);
//  Bar bar;
//
//  assert(99 == convertToInt(&foo));
//  assert(0  == convertToInt(&bar));
//..

#include <bslscm_version.h>

#include <bslmf_addconst.h>
#include <bslmf_addlvaluereference.h>
#include <bslmf_assert.h>
#include <bslmf_conditional.h>
#include <bslmf_enableif.h>
#include <bslmf_integralconstant.h>
#include <bslmf_isarray.h>
#include <bslmf_isfunction.h>
#include <bslmf_isfundamental.h>
#include <bslmf_ispointer.h>
#include <bslmf_isvoid.h>
#include <bslmf_matchanytype.h>
#include <bslmf_removecv.h>

#include <bsls_compilerfeatures.h>
#include <bsls_keyword.h>
#include <bsls_platform.h>

#ifdef BSLS_COMPILERFEATURES_SUPPORT_TRAITS_HEADER
# include <type_traits>
#endif // BSLS_COMPILERFEATURES_SUPPORT_TRAITS_HEADER

#ifndef BDE_DONT_ALLOW_TRANSITIVE_INCLUDES
#include <bsls_nativestd.h>
#endif // BDE_DONT_ALLOW_TRANSITIVE_INCLUDES

#if defined(BSLS_COMPILERFEATURES_SUPPORT_TRAITS_HEADER)
# if !defined(BSLS_PLATFORM_CMP_MSVC) || BSLS_PLATFORM_CMP_VERSION > 1900
    // The Microsoft implementation of native traits allows binding of rvalues
    // (including temporaries invented for conversion) to 'const volatile &'
    // references.  Early versions also do not correctly disallow conversion
    // from itself for types that are neither copy- nor move-constructible.
#   define BSLMF_ISCONVERTIBLE_USE_NATIVE_TRAITS
# endif
#endif

namespace bsl {

template <class t_FROM_TYPE, class t_TO_TYPE>
struct is_convertible;

#ifdef BSLS_COMPILERFEATURES_SUPPORT_VARIABLE_TEMPLATES
template <class t_FROM_TYPE, class t_TO_TYPE>
BSLS_KEYWORD_INLINE_VARIABLE constexpr bool is_convertible_v =
                                 is_convertible<t_FROM_TYPE, t_TO_TYPE>::value;
    // This template variable represents the result value of the
    // 'bsl::is_convertible' meta-function.
#endif

}  // close namespace bsl

namespace BloombergLP {
namespace bslmf {

                 // =========================================
                 // private class IsConvertible_CheckComplete
                 // =========================================

template <class t_TYPE,
          bool = bsl::is_function<t_TYPE>::value ||
                 bsl::is_void<t_TYPE>::value>
struct IsConvertible_CheckComplete {
    typedef t_TYPE type;

    enum { k_CHECK_COMPLETE = sizeof(t_TYPE) };
};

template <class t_TYPE>
struct IsConvertible_CheckComplete<t_TYPE&, false>
: IsConvertible_CheckComplete<t_TYPE> {
    typedef t_TYPE&  type;
};

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
template <class t_TYPE>
struct IsConvertible_CheckComplete<t_TYPE&&, false>
: IsConvertible_CheckComplete<t_TYPE> {
    typedef t_TYPE && type;
};
#endif

template <class t_TYPE>
struct IsConvertible_CheckComplete<t_TYPE, true> {
    typedef t_TYPE type;
};

#if !defined(BSLS_PLATFORM_CMP_IBM)  // IBM rejects this valid specialization
template <class t_TYPE>
struct IsConvertible_CheckComplete<t_TYPE[], false> {
    typedef t_TYPE type[];
};
#endif

}  // close package namespace
}  // close enterprise namespace

#ifdef BSLMF_ISCONVERTIBLE_USE_NATIVE_TRAITS

namespace bsl {

template <class t_FROM_TYPE, class t_TO_TYPE>
struct is_convertible
: ::bsl::integral_constant<
      bool,
      ::std::is_convertible<
          typename BloombergLP::bslmf::IsConvertible_CheckComplete<
              t_FROM_TYPE>::type,
          typename BloombergLP::bslmf::IsConvertible_CheckComplete<
              t_TO_TYPE>::type>::value> {
};

}  // close namespace bsl
#else

namespace BloombergLP {
namespace bslmf {

                         // ==========================
                         // struct IsConvertible_Match
                         // ==========================

struct IsConvertible_Match {
    // This 'struct' provides functions to check for successful conversion
    // matches.  Sun CC 5.2 requires that this 'struct' not be nested within
    // 'IsConvertible_Imp'.

    typedef struct { char a;    } yes_type;
    typedef struct { char a[2]; } no_type;

    static yes_type match(IsConvertible_Match&);
        // Return 'yes_type' if called on 'IsConvertible_Match' type.

#if !defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
    template <class t_TYPE>
    static no_type match(const t_TYPE&);
        // Return 'yes_type' if the (template parameter) 't_TYPE' is
        // 'IsConvertible_Match', and 'no_type' otherwise.

    template <class t_TYPE>
    static no_type match(const volatile t_TYPE&);
        // Return 'yes_type' if the (template parameter) 't_TYPE' is
        // 'IsConvertible_Match' and 'no_type' otherwise.

    template <class t_TYPE>
    static typename
    bsl::enable_if<bsl::is_function<t_TYPE>::value, no_type>::type
    match(t_TYPE&);
        // Return 'yes_type' if the (template parameter) 't_TYPE' is
        // 'IsConvertible_Match' and 'no_type' otherwise.
#else
    template <class t_TYPE>
    static no_type match(t_TYPE&&);
        // Return 'yes_type' if the (template parameter) 't_TYPE' is
        // 'IsConvertible_Match', and 'no_type' otherwise.
#endif
};

                         // ========================
                         // struct IsConvertible_Imp
                         // ========================

template <class t_FROM_TYPE,
          class t_TO_TYPE
#if defined(BSLS_PLATFORM_CMP_GNU) || defined(BSLS_PLATFORM_CMP_CLANG)
          ,
          int IS_FROM_FUNDAMENTAL = bsl::is_fundamental<t_FROM_TYPE>::value,
          int IS_TO_FUNDAMENTAL   = bsl::is_fundamental<t_TO_TYPE>::value
#endif
          >
struct IsConvertible_Imp {
    // This 'struct' template implements the meta-function to determine type
    // conversion between the (template parameter) 't_FROM_TYPE' and the
    // (template parameter) 't_TO_TYPE' where the conversion to the 't_TO_TYPE'
    // is not necessarily the same as conversion to 'const t_TO_TYPE&'.
    //
    // Note that significant documentation about the details of this
    // implementation can be found in 'bslmf_isconvertible.cpp'.

  private:
    struct Test
    {
        // A unique (empty) type returned by the comma operator.

        IsConvertible_Match& operator, (t_TO_TYPE) const;
            // Return a reference to type 'IsConvertible_Match'.
    };

  public:

#ifdef BSLS_PLATFORM_CMP_MSVC
#   pragma warning(push)
#   pragma warning(disable: 4244)  // loss of precision warning ignored
#endif
    enum {

        value = (sizeof(IsConvertible_Match::yes_type) ==
                 sizeof(IsConvertible_Match::match(
                                       (Test(), TypeRep<t_FROM_TYPE>::rep()))))
        // Return the convertibility between 't_FROM_TYPE' and 't_TO_TYPE'.
        // This is set by invoking the 'operator,' method having 'Test&' on
        // the left and 't_FROM_TYPE' on the right.  The 'value' is 'true' if
        // 't_FROM_TYPE' is convertible to 't_TO_TYPE', and 'false' otherwise.
    };

#ifdef BSLS_PLATFORM_CMP_MSVC
#   pragma warning(pop)
#endif

    typedef bsl::integral_constant<bool, value> type;
        // This 'typedef' returns 'bsl::true_type' if 't_FROM_TYPE' is
        // convertible to 't_TO_TYPE', and 'bsl::false_type' otherwise.
};

#if 0 // defined(BSLS_PLATFORM_CMP_GNU) || defined(BSLS_PLATFORM_CMP_CLANG)
// The following template partial specializations produce the same results as
// the unspecialized template would, but avoid generating conversion warnings
// on the affected compilers.  However, there is one known bug in this block of
// code (reported by the test driver, not yet tracked down) and experimentally
// removing it seems to clear the bug, without raising the feared warnings on
// supported gcc platforms - although it still raises a few warnings with gcc
// 4.3.5.

#define BSLMF_ISCONVERTIBLE_SAMETYPEVALUE(t_VALUE,                            \
                                          t_FROM,                             \
                                          t_TO,                               \
                                          t_FROM_FUND,                        \
                                          t_TO_FUND)                          \
    template <class t_TYPE>                                                   \
    struct IsConvertible_Imp<t_FROM, t_TO, t_FROM_FUND, t_TO_FUND>            \
    : bsl::integral_constant<bool, VALUE> {                                   \
    };
    // This partial specialization of 'bslmf::IsConvertible_Imp' derives from
    // 'bsl::integral_constant' having the specified macro argument 'VALUE'.
    // The specified macro arguments 't_FROM' and 't_TO' are cv-qualified type
    // expressions constructed out of the (template parameter) 't_TYPE'.

#define BSLMF_ISCONVERTIBLE_VALUE(t_VALUE,                                    \
                                  t_FROM,                                     \
                                  t_TO,                                       \
                                  t_FROM_FUND,                                \
                                  t_TO_FUND)                                  \
    template <class t_FROM_TYPE, class t_TO_TYPE>                             \
    struct IsConvertible_Imp<t_FROM, t_TO, t_FROM_FUND, t_TO_FUND>            \
    : bsl::integral_constant<bool, t_VALUE> {                                 \
    };
    // This partial specialization of 'bslmf::IsConvertible_Imp' derives from
    // 'bsl::integral_constant' having the specified macro argument 't_VALUE'.
    // The specified macro arguments 't_FROM' and 't_TO' are cv-qualified type
    // expressions constructed out of 't_FROM_TYPE' and 't_TO_TYPE',
    // respectively.

#define BSLMF_ISCONVERTIBLE_FORWARD(t_FROM, t_TO, t_FROM_FUND, t_TO_FUND)     \
    template <class t_FROM_TYPE, class t_TO_TYPE>                             \
    struct IsConvertible_Imp<t_FROM, t_TO, t_FROM_FUND, t_TO_FUND>            \
    : IsConvertible_Imp<t_FROM, t_TO, 0, 0> {                                 \
    };
    // This partial specialization of 'bslmf::IsConvertible_Imp' applies the
    // general mechanism for non-fundamental types.  The specified macro
    // arguments 't_FROM' and 't_TO' are cv-qualified type expressions
    // constructed out of 't_FROM_TYPE' and 't_TO_TYPE', respectively.

BSLMF_ISCONVERTIBLE_SAMETYPEVALUE(0,
                                  const volatile t_TYPE,
                                  const t_TYPE&,
                                  1,
                                  1)
BSLMF_ISCONVERTIBLE_SAMETYPEVALUE(0,
                                  volatile t_TYPE,
                                  const t_TYPE&,
                                  1,
                                  1)
    // These two partial specializations are instantiated when a (possibly
    // 'const'-qualified) 'volatile' fundamental type is tested for
    // convertibility to its 'const' reference type.  The conversion shall
    // fail.

BSLMF_ISCONVERTIBLE_VALUE(1,
                          const volatile t_FROM_TYPE,
                          const t_TO_TYPE&,
                          1,
                          1)
BSLMF_ISCONVERTIBLE_VALUE(1,
                          volatile t_FROM_TYPE,
                          const t_TO_TYPE&,
                          1,
                          1)
    // These two partial specializations are instantiated when a (possibly
    // 'const'-qualified) 'volatile' type is tested for convertibility to the
    // 'const' reference type of another fundamental type.  These partial
    // specializations will be picked up if the previous two fail to match.
    // The conversion shall succeed.

BSLMF_ISCONVERTIBLE_VALUE(1, const t_FROM_TYPE, const t_TO_TYPE&, 1, 1)
BSLMF_ISCONVERTIBLE_VALUE(1,       t_FROM_TYPE, const t_TO_TYPE&, 1, 1)
    // These two partial specializations are instantiated when a (possibly
    // 'const'-qualified) fundamental type is tested for convertibility to the
    // 'const' reference type of another fundamental type.  These partial
    // specializations will be picked up if the previous two fail to match.
    // The conversion shall succeed.

BSLMF_ISCONVERTIBLE_FORWARD(const volatile t_FROM_TYPE,
                            const volatile t_TO_TYPE&, 1, 1)
BSLMF_ISCONVERTIBLE_FORWARD(      volatile t_FROM_TYPE,
                            const volatile t_TO_TYPE&, 1, 1)
BSLMF_ISCONVERTIBLE_FORWARD(const          t_FROM_TYPE,
                            const volatile t_TO_TYPE&, 1, 1)
BSLMF_ISCONVERTIBLE_FORWARD(               t_FROM_TYPE,
                            const volatile t_TO_TYPE&, 1, 1)
    // These four partial specializations are instantiated when a (possibly
    // cv-qualified) fundamental type is tested for convertibility to the
    // 'const volatile' reference type of another fundamental type.

BSLMF_ISCONVERTIBLE_FORWARD(const volatile t_FROM_TYPE,
                                  volatile t_TO_TYPE&, 1, 1)
BSLMF_ISCONVERTIBLE_FORWARD(      volatile t_FROM_TYPE,
                                  volatile t_TO_TYPE&, 1, 1)
BSLMF_ISCONVERTIBLE_FORWARD(const          t_FROM_TYPE,
                                  volatile t_TO_TYPE&, 1, 1)
BSLMF_ISCONVERTIBLE_FORWARD(               t_FROM_TYPE,
                                  volatile t_TO_TYPE&, 1, 1)
    // These four partial specializations are instantiated when a (possibly
    // cv-qualified) fundamental type is tested for convertibility to the
    // 'volatile' reference type of another fundamental type.

BSLMF_ISCONVERTIBLE_FORWARD(const volatile t_FROM_TYPE, t_TO_TYPE&, 1, 1)
BSLMF_ISCONVERTIBLE_FORWARD(      volatile t_FROM_TYPE, t_TO_TYPE&, 1, 1)
BSLMF_ISCONVERTIBLE_FORWARD(const          t_FROM_TYPE, t_TO_TYPE&, 1, 1)
BSLMF_ISCONVERTIBLE_FORWARD(               t_FROM_TYPE, t_TO_TYPE&, 1, 1)
    // These four partial specializations are instantiated when a (possibly
    // cv-qualified) fundamental type is tested for convertibility to the
    // non-cv-qualified reference type of another fundamental type.

template <class t_FROM_TYPE, class t_TO_TYPE>
struct IsConvertible_Imp<const t_FROM_TYPE, t_TO_TYPE, 1, 1>
    : IsConvertible_Imp<const t_FROM_TYPE, double, 0, 0>::type {
    // This partial specialization is instantiated when the 'const' (template
    // parameter) fundamental 't_FROM_TYPE' is tested for convertibility to
    // another (template parameter) fundamental 't_TO_TYPE'.  This partial
    // specialization derives from
    // 'IsConvertible_Imp<const t_FROM_TYPE, double, 0, 0>' to avoid any
    // compilation warnings in case the 't_TO_TYPE' is an integral type and
    // 't_FROM_TYPE' is a floating-point type.
};

template <class t_FROM_TYPE, class t_TO_TYPE>
struct IsConvertible_Imp<t_FROM_TYPE, t_TO_TYPE, 1, 1>
    : IsConvertible_Imp<t_FROM_TYPE, double, 0, 0>::type {
    // This partial specialization is instantiated when the (template
    // parameter) fundamental 't_FROM_TYPE' is tested for convertibility to
    // another (template parameter) fundamental 't_TO_TYPE'.  This partial
    // specialization derives from
    // 'IsConvertible_Imp<t_FROM_TYPE, double, 0, 0>' to avoid any compilation
    // warnings in case that the 't_FROM_TYPE' is a floating-point type and
    // 't_TO_TYPE' is an integral type.
};

template <class t_FROM_TYPE, class t_TO_TYPE>
struct IsConvertible_Imp<t_FROM_TYPE, t_TO_TYPE, 0, 1>
    : IsConvertible_Imp<t_FROM_TYPE, double, 0, 0>::type {
    // This partial specialization is instantiated when the (template
    // parameter) 't_FROM_TYPE' is a non-fundamental type, and the (template
    // parameter) 't_TO_TYPE' is a non-'void' fundamental type.  This partial
    // specialization derives from
    // 'IsConvertible_Imp<t_FROM_TYPE, double, 0, 0>' to avoid any compilation
    // warnings in case that the 't_FROM_TYPE' is a floating-point type and the
    // 't_TO_TYPE' is an integral type.
};

template <class t_FROM_TYPE, class t_TO_TYPE>
struct IsConvertible_Imp<t_FROM_TYPE, t_TO_TYPE, 1, 0>
    : IsConvertible_Imp<int, t_TO_TYPE, 0, 0>::type {
    // This partial specialization is instantiated when the (template
    // parameter) 't_FROM_TYPE' is a non-'void' fundamental type, and the
    // (template parameter) 't_TO_TYPE' is a non-fundamental type.  This
    // partial specialization derives from
    // 'IsConvertible_Imp<int, t_TO_TYPE, 0, 0>' to avoid any compilation
    // warnings in case that the 't_FROM_TYPE' is a floating-point type and the
    // 't_TO_TYPE' is an integral type.
};

#undef BSLMF_ISCONVERTIBLE_SAMETYPEVALUE
#undef BSLMF_ISCONVERTIBLE_VALUE
#undef BSLMF_ISCONVERTIBLE_FORWARD

#endif

template <class t_TO_TYPE>
struct IsConvertible_LazyTrait
: bsl::add_lvalue_reference<typename bsl::add_const<
      typename bsl::remove_cv<t_TO_TYPE>::type>::type> {
};

template <class t_FROM_TYPE, class t_TO_TYPE>
struct IsConvertible_IsNeverConvertible
: bsl::integral_constant<bool,
                         bsl::is_void<t_FROM_TYPE>::value ||
                             bsl::is_array<t_TO_TYPE>::value ||
                             bsl::is_function<t_TO_TYPE>::value> {
};

template <class t_FROM_TYPE, class t_TO_TYPE>
struct IsConvertible_FilterNeverConvertible
: bsl::conditional<
      IsConvertible_IsNeverConvertible<t_FROM_TYPE, t_TO_TYPE>::value,
      bsl::false_type,
      IsConvertible_Imp<t_FROM_TYPE,
                        typename IsConvertible_LazyTrait<t_TO_TYPE>::type> >::
      type {
};

template <class t_FROM_TYPE, class t_TO_TYPE>
struct IsConvertible_Conditional
: bsl::conditional<
      bsl::is_void<t_TO_TYPE>::value,
      typename bsl::is_void<t_FROM_TYPE>::type,
      IsConvertible_FilterNeverConvertible<
          typename IsConvertible_CheckComplete<t_FROM_TYPE>::type,
          typename IsConvertible_CheckComplete<t_TO_TYPE>::type> >::type {
};

}  // close package namespace
}  // close enterprise namespace

namespace bsl {

                         // ==============================
                         // struct is_convertible_dispatch
                         // ==============================

template <class t_FROM_TYPE, class t_TO_TYPE>
struct is_convertible_dispatch
: BloombergLP::bslmf::IsConvertible_Conditional<t_FROM_TYPE, t_TO_TYPE>::type {
    // This 'struct' template implements the 'is_convertible_dispatch'
    // meta-function defined in the C++11 standard [meta.rel] to determine if
    // the (template parameter) 't_FROM_TYPE' is convertible to the (template
    // parameter) 't_TO_TYPE'.  This 'struct' derives from 'bsl::true_type' if
    // the 't_FROM_TYPE' is convertible to 't_TO_TYPE', and from
    // 'bsl::false_type' otherwise.  Note that both 't_FROM_TYPE' and
    // 't_TO_TYPE' should be complete types, arrays of unknown bound, or
    // (possibly cv-qualified) 'void' types.
};

template <class t_TYPE>
struct is_convertible_dispatch<t_TYPE, t_TYPE&>
: integral_constant<bool,
                    is_reference<t_TYPE>::value ||
                        is_function<t_TYPE>::value ||
                        is_const<t_TYPE>::value> {
    // This set of rules corresponds with the reference binding rules in c++11,
    // where the specification of 'is_convertible_dispatch' relies on
    // rvalue-references.  We must supply these specializations directly in
    // order to support C++03 compilers that do not have a good substitute for
    // rvalue-references, as using 'const &' instead produces subtly different
    // results in some cases.
};

// Some compilers need explicit guidance on a few of the reference-binding
// conversions.  All compilers get most of these correct, but once a few
// specializations are added, the full set is required to avoid ambiguities.

template <class t_TYPE>
struct is_convertible_dispatch<t_TYPE, const t_TYPE&> : true_type {
};

template <class t_TYPE>
struct is_convertible_dispatch<t_TYPE, volatile t_TYPE&> : false_type {
};

template <class t_TYPE>
struct is_convertible_dispatch<t_TYPE, const volatile t_TYPE&> : false_type {
};

template <class t_TYPE>
struct is_convertible_dispatch<const t_TYPE, t_TYPE&> : false_type {
};

template <class t_TYPE>
struct is_convertible_dispatch<const t_TYPE, const t_TYPE&> : true_type {
};

template <class t_TYPE>
struct is_convertible_dispatch<const t_TYPE, volatile t_TYPE&> : false_type {
};

template <class t_TYPE>
struct is_convertible_dispatch<const t_TYPE, const volatile t_TYPE&>
: false_type {
};

template <class t_TYPE>
struct is_convertible_dispatch<volatile t_TYPE, t_TYPE&> : false_type {
};

template <class t_TYPE>
struct is_convertible_dispatch<volatile t_TYPE, const t_TYPE&> : false_type {
};

template <class t_TYPE>
struct is_convertible_dispatch<volatile t_TYPE, volatile t_TYPE&>
: false_type {
};

template <class t_TYPE>
struct is_convertible_dispatch<volatile t_TYPE, const volatile t_TYPE&>
: false_type {
};

template <class t_TYPE>
struct is_convertible_dispatch<const volatile t_TYPE, t_TYPE&> : false_type {
};

template <class t_TYPE>
struct is_convertible_dispatch<const volatile t_TYPE, const t_TYPE&>
: false_type {
};

template <class t_TYPE>
struct is_convertible_dispatch<const volatile t_TYPE, volatile t_TYPE&>
: false_type {
};

template <class t_TYPE>
struct is_convertible_dispatch<const volatile t_TYPE, const volatile t_TYPE&>
: false_type {
};

// The next group of partial specializations deal with various cases of
// converting to an lvalue-reference, which we make explicitly conform to the
// C++11 idiom of converting from an rvalue (which may be an lvalue-reference).

template <class t_TYPE>
struct is_convertible_dispatch<t_TYPE&, t_TYPE&> : true_type {
};

template <class t_TYPE>
struct is_convertible_dispatch<t_TYPE&, const t_TYPE&> : true_type {
};

template <class t_TYPE>
struct is_convertible_dispatch<t_TYPE&, volatile t_TYPE&> : true_type {
};

template <class t_TYPE>
struct is_convertible_dispatch<t_TYPE&, const volatile t_TYPE&> : true_type {
};

template <class t_TYPE>
struct is_convertible_dispatch<const t_TYPE&, t_TYPE&> : false_type {
};

template <class t_TYPE>
struct is_convertible_dispatch<const t_TYPE&, const t_TYPE&> : true_type {
};

template <class t_TYPE>
struct is_convertible_dispatch<const t_TYPE&, volatile t_TYPE&> : false_type {
};

template <class t_TYPE>
struct is_convertible_dispatch<const t_TYPE&, const volatile t_TYPE&>
: true_type {
};

template <class t_TYPE>
struct is_convertible_dispatch<volatile t_TYPE&, t_TYPE&> : false_type {
};

template <class t_TYPE>
struct is_convertible_dispatch<volatile t_TYPE&, const t_TYPE&> : false_type {
};

template <class t_TYPE>
struct is_convertible_dispatch<volatile t_TYPE&, volatile t_TYPE&>
: true_type {
};

template <class t_TYPE>
struct is_convertible_dispatch<volatile t_TYPE&, const volatile t_TYPE&>
: true_type {
};

template <class t_TYPE>
struct is_convertible_dispatch<const volatile t_TYPE&, t_TYPE&> : false_type {
};

template <class t_TYPE>
struct is_convertible_dispatch<const volatile t_TYPE&, const t_TYPE&>
: false_type {
};

template <class t_TYPE>
struct is_convertible_dispatch<const volatile t_TYPE&, volatile t_TYPE&>
: false_type {
};

template <class t_TYPE>
struct is_convertible_dispatch<const volatile t_TYPE&, const volatile t_TYPE&>
: true_type {
};

template <class t_TYPE>
struct is_convertible_dispatch<volatile t_TYPE, t_TYPE>
: BloombergLP::bslmf::IsConvertible_Conditional<t_TYPE, t_TYPE>::type {
    // Correct handling of non-fundamental volatile conversions to self.  Note
    // that this is not trivially true, but tests that 't_TYPE' is copy (or
    // move) constructible.
};

template <class t_FROM_TYPE, class t_TO_TYPE>
struct is_convertible_dispatch<t_FROM_TYPE, volatile t_TO_TYPE&> : false_type {
};

template <class t_FROM_TYPE, class t_TO_TYPE>
struct is_convertible_dispatch<t_FROM_TYPE, const volatile t_TO_TYPE&>
: false_type {
};

template <class t_FROM_TYPE, class t_TO_TYPE>
struct is_convertible_dispatch<volatile t_FROM_TYPE&, volatile t_TO_TYPE&>
: is_convertible_dispatch<t_FROM_TYPE *, t_TO_TYPE *>::type {
};

template <class t_FROM_TYPE, class t_TO_TYPE>
struct is_convertible_dispatch<volatile t_FROM_TYPE&,
                               const volatile t_TO_TYPE&>
: is_convertible_dispatch<t_FROM_TYPE, const volatile t_TO_TYPE&>::type {
};

                         // =====================
                         // struct is_convertible
                         // =====================

template <class t_FROM_TYPE>
struct EffectiveFromType : conditional<is_fundamental<t_FROM_TYPE>::value ||
                                           is_pointer<t_FROM_TYPE>::value,
                                       typename remove_cv<t_FROM_TYPE>::type,
                                       t_FROM_TYPE> {
};

template <class t_FROM_TYPE, class t_TO_TYPE>
struct is_convertible_dispatch<volatile t_FROM_TYPE&, t_TO_TYPE>
: bsl::conditional<
      bsl::is_fundamental<t_FROM_TYPE>::value,
      typename bsl::is_convertible_dispatch<t_FROM_TYPE, t_TO_TYPE>::type,
      typename BloombergLP::bslmf::IsConvertible_Conditional<
          volatile t_FROM_TYPE,
          t_TO_TYPE>::type>::type {
};

template <class t_FROM_TYPE, class t_TO_TYPE>
struct is_convertible
: is_convertible_dispatch<typename EffectiveFromType<t_FROM_TYPE>::type,
                          t_TO_TYPE>::type {
    // This 'struct' template implements the 'is_convertible_dispatch'
    // meta-function defined in the C++11 standard [meta.rel] to determine if
    // the (template parameter) 't_FROM_TYPE' is convertible to the (template
    // parameter) 't_TO_TYPE'.  This 'struct' derives from 'bsl::true_type' if
    // the 't_FROM_TYPE' is convertible to 't_TO_TYPE', and from
    // 'bsl::false_type' otherwise.  Note that both 't_FROM_TYPE' and
    // 't_TO_TYPE' should be complete types, arrays of unknown bound, or
    // (possibly cv-qualified) 'void' types.
};

}  // close namespace bsl
#endif

namespace BloombergLP {
namespace bslmf {

                         // ====================
                         // struct IsConvertible
                         // ====================

template <class t_FROM_TYPE, class t_TO_TYPE>
struct IsConvertible : bsl::is_convertible<t_FROM_TYPE, t_TO_TYPE>::type {
    // This 'struct' template implements a meta-function to determine if the
    // (template parameter) 't_FROM_TYPE' is convertible to the (template
    // parameter) 't_TO_TYPE'.  This 'struct' derives from 'bsl::true_type' if
    // the 't_FROM_TYPE' is convertible to 't_TO_TYPE', and from
    // 'bsl::false_type' otherwise.  Note that both 't_FROM_TYPE' and
    // 't_TO_TYPE' should be complete types, arrays of unknown bound, or
    // (possibly cv-qualified) 'void' types.
};

}  // close package namespace

#ifndef BDE_OPENSOURCE_PUBLICATION  // BACKWARD_COMPATIBILITY
// ============================================================================
//                           BACKWARD COMPATIBILITY
// ============================================================================

#ifdef bslmf_IsConvertible
#undef bslmf_IsConvertible
#endif
#define bslmf_IsConvertible bslmf::IsConvertible
    // This alias is defined for backward compatibility.
#endif  // BDE_OPENSOURCE_PUBLICATION -- BACKWARD_COMPATIBILITY

}  // close enterprise namespace

#endif // ! defined(INCLUDED_BSLMF_ISCONVERTIBLE)

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
