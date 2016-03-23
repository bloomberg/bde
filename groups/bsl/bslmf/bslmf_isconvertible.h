// bslmf_isconvertible.h                                              -*-C++-*-
#ifndef INCLUDED_BSLMF_ISCONVERTIBLE
#define INCLUDED_BSLMF_ISCONVERTIBLE

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a compile-time check for type conversion.
//
//@CLASSES:
//  bsl::is_convertible: standard meta-function for type conversion checking
//  bslmf::IsConvertible: meta-function for type conversion checking
//
//@SEE_ALSO: bslmf_integralconstant
//
//@DESCRIPTION: This component defines two meta-functions,
// 'bsl::is_convertible' and 'BloombergLP::bslmf::IsConvertible', both of which
// may be used to check whether a conversion exists from one type to another.
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
// Suppose we are implementing a 'convertToInt' template method that converts a
// given object of the (template parameter) 'TYPE' to 'int' type, and returns
// the integer value.  If the given object can not convert to 'int', return 0.
// The method calls an overloaded function, 'getIntValue', to get the converted
// integer value.  The idea is to invoke one version of 'getIntValue' if the
// type provides a conversion operator that returns an integer value, and
// another version if the type does not provide such an operator.
//
// First, we define two classes, 'Foo' and 'Bar'.  The 'Foo' class has a
// conversion operator that returns an integer value while the 'Bar' class does
// not:
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
// Then, we define the first 'getIntValue' function that takes a
// 'bsl::false_type' as its last argument, whereas the second 'getIntValue'
// function takes a 'bsl::true_type' object.  The result of the
// 'bsl::is_convertible' meta-function (i.e., its 'type' member) is used to
// create the last argument passed to 'getIntValue'.  Neither version of
// 'getIntValue' makes use of this argument -- it is used only to differentiate
// the argument list so we can overload the function.
//..
//  template <class TYPE>
//  inline
//  int getIntValue(TYPE *object, bsl::false_type)
//  {
//      // Return 0 because the specified 'object' of the (template parameter)
//      // 'TYPE' is not convertible to the 'int' type.
//
//      return 0;
//  }
//
//  template <class TYPE>
//  inline
//  int getIntValue(TYPE *object, bsl::true_type)
//  {
//      // Return the integer value converted from the specified 'object' of
//      // the (template parameter) 'TYPE'.
//
//      return int(*object);
//  }
//..
// Now, we define our 'convertToInt' method:
//..
//  template <class TYPE>
//  inline
//  int convertToInt(TYPE *object)
//  {
//      typedef typename bsl::is_convertible<TYPE, int>::type CanConvertToInt;
//      return getIntValue(object, CanConvertToInt());
//  }
//..
// Notice that we use 'bsl::is_convertible' to get a 'bsl::false_type' or
// 'bsl::true_type', and then call the corresponding overloaded 'getIntValue'
// method.
//
// Finally, we call 'convertToInt' with both 'Foo' and 'Bar' classes:
//..
//  Foo foo(99);
//  Bar bar;
//
//  printf("%d\n", convertToInt(&foo));
//  printf("%d\n", convertToInt(&bar));
//..

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLMF_ADDCONST
#include <bslmf_addconst.h>
#endif

#ifndef INCLUDED_BSLMF_ADDLVALUEREFERENCE
#include <bslmf_addlvaluereference.h>
#endif

#ifndef INCLUDED_BSLMF_ASSERT
#include <bslmf_assert.h>
#endif

#ifndef INCLUDED_BSLMF_CONDITIONAL
#include <bslmf_conditional.h>
#endif

#ifndef INCLUDED_BSLMF_ENABLEIF
#include <bslmf_enableif.h>
#endif

#ifndef INCLUDED_BSLMF_INTEGRALCONSTANT
#include <bslmf_integralconstant.h>
#endif

#ifndef INCLUDED_BSLMF_ISARRAY
#include <bslmf_isarray.h>
#endif

#ifndef INCLUDED_BSLMF_ISFUNCTION
#include <bslmf_isfunction.h>
#endif

#ifndef INCLUDED_BSLMF_ISFUNDAMENTAL
#include <bslmf_isfundamental.h>
#endif

#ifndef INCLUDED_BSLMF_ISPOINTER
#include <bslmf_ispointer.h>
#endif

#ifndef INCLUDED_BSLMF_ISVOID
#include <bslmf_isvoid.h>
#endif

#ifndef INCLUDED_BSLMF_MATCHANYTYPE
#include <bslmf_matchanytype.h>
#endif

#ifndef INCLUDED_BSLMF_REMOVECV
#include <bslmf_removecv.h>
#endif

#ifndef INCLUDED_BSLS_COMPILERFEATURES
#include <bsls_compilerfeatures.h>
#endif

#ifndef INCLUDED_BSLS_PLATFORM
#include <bsls_platform.h>
#endif

#ifdef BSLS_COMPILERFEATURES_SUPPORT_TRAITS_HEADER
#ifndef INCLUDED_BSLS_NATIVESTD
#include <bsls_nativestd.h>
#endif

#ifndef INCLUDED_TYPE_TRAITS
# define BSLMF_INCLUDE_ONLY_NATIVE_TRAITS
# include <type_traits>
#endif

#endif // BSLS_COMPILERFEATURES_SUPPORT_TRAITS_HEADER

#if defined(BSLS_COMPILERFEATURES_SUPPORT_TRAITS_HEADER)
# if !defined(BSLS_PLATFORM_CMP_MSVC) || BSLS_PLATFORM_CMP_VERSION > 1900
    // The Microsft implementation of native traits allows binding of rvalues
    // (including temporaries invented for conversion) to 'const volatile &'
    // references.  Early versions also do not correctly disallow conversion
    // from itself for types that are neither copy- nor move-constructible.
#   define BSLMF_ISCONVERTIBLE_USE_NATIVE_TRAITS
# endif
#endif

namespace BloombergLP {
namespace bslmf {

                 // =========================================
                 // private class IsConvertible_CheckComplete
                 // =========================================

template <class TYPE,
          bool = bsl::is_function<TYPE>::value || bsl::is_void<TYPE>::value>
struct IsConvertible_CheckComplete {
    typedef TYPE type;

    BSLMF_ASSERT((sizeof(TYPE) > 0));
};

template <class TYPE>
struct IsConvertible_CheckComplete<TYPE &, false>
     : IsConvertible_CheckComplete<TYPE > {
    typedef TYPE & type;
};

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
template <class TYPE>
struct IsConvertible_CheckComplete<TYPE &&, false>
     : IsConvertible_CheckComplete<TYPE > {
    typedef TYPE && type;
};
#endif

template <class TYPE>
struct IsConvertible_CheckComplete<TYPE, true> {
    typedef TYPE type;
};

#if !defined(BSLS_PLATFORM_CMP_IBM)  // IBM rejects this valid specialization
template <class TYPE>
struct IsConvertible_CheckComplete<TYPE[], false> {
    typedef TYPE type[];
};
#endif

}  // close package namespace
}  // close enterprise namespace

#ifdef BSLMF_ISCONVERTIBLE_USE_NATIVE_TRAITS

namespace bsl {

template <class FROM_TYPE, class TO_TYPE>
struct is_convertible
    : ::bsl::integral_constant<bool,
                               ::native_std::is_convertible<
     typename BloombergLP::bslmf::IsConvertible_CheckComplete<FROM_TYPE>::type,
     typename BloombergLP::bslmf::IsConvertible_CheckComplete<  TO_TYPE>::type
                                                           >::value> {
};

}
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
    template <class TYPE>
    static no_type match(const TYPE&);
        // Return 'yes_type' if the (template parameter) 'TYPE' is
        // 'IsConvertible_Match', and 'no_type' otherwise.

    template <class TYPE>
    static no_type match(const volatile TYPE&);
        // Return 'yes_type' if the (template parameter) 'TYPE' is
        // 'IsConvertible_Match' and 'no_type' otherwise.

    template <class TYPE>
    static
    typename bsl::enable_if<bsl::is_function<TYPE>::value, no_type>::type
        match(TYPE&);
        // Return 'yes_type' if the (template parameter) 'TYPE' is
        // 'IsConvertible_Match' and 'no_type' otherwise.
#else
    template <class TYPE>
    static no_type match(TYPE&&);
        // Return 'yes_type' if the (template parameter) 'TYPE' is
        // 'IsConvertible_Match', and 'no_type' otherwise.
#endif
};

                         // ========================
                         // struct IsConvertible_Imp
                         // ========================

template <class FROM_TYPE, class TO_TYPE
#if defined(BSLS_PLATFORM_CMP_GNU) || defined(BSLS_PLATFORM_CMP_CLANG)
         , int IS_FROM_FUNDAMENTAL = bsl::is_fundamental<FROM_TYPE>::value
         , int IS_TO_FUNDAMENTAL   = bsl::is_fundamental<TO_TYPE>::value
#endif
         >
struct IsConvertible_Imp {
    // This 'struct' template implements the meta-function to determine type
    // conversion between the (template parameter) 'FROM_TYPE' and the
    // (template parameter) 'TO_TYPE' where the conversion to the 'TO_TYPE' is
    // not necessarily the same as conversion to 'const TO_TYPE&'.
    //
    // Note that significant documentation about the details of this
    // implementation can be found in 'bslmf_isconvertible.cpp'.

  private:
    struct Test
    {
        // A unique (empty) type returned by the comma operator.

        IsConvertible_Match& operator, (TO_TYPE) const;
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
                                         (Test(), TypeRep<FROM_TYPE>::rep()))))
            // Return the convertibility between 'FROM_TYPE' and 'TO_TYPE'.
            // This is set by invoking the 'operator,' method having 'Test&' on
            // the left and 'FROM_TYPE' on the right.  The 'value' is 'true' if
            // 'FROM_TYPE' is convertible to 'TO_TYPE', and 'false' otherwise.
    };

#ifdef BSLS_PLATFORM_CMP_MSVC
#   pragma warning(pop)
#endif

    typedef bsl::integral_constant<bool, value> type;
        // This 'typedef' returns 'bsl::true_type' if 'FROM_TYPE' is
        // convertible to 'TO_TYPE', and 'bsl::false_type' otherwise.
};

#if 0 // defined(BSLS_PLATFORM_CMP_GNU) || defined(BSLS_PLATFORM_CMP_CLANG)
// The following template partial specializations produce the same results as
// the unspecialized template would, but avoid generating conversion warnings
// on the affected compilers.  However, there is one known bug in this block of
// code (reported by the test driver, not yet tracked down) and experimentally
// removing it seems to clear the bug, without raising the feared warnings on
// supported gcc platforms - although it still raises a few warnings with gcc
// 4.3.5.

#define BSLMF_ISCONVERTIBLE_SAMETYPEVALUE(VALUE, FROM, TO, FROM_FUND, TO_FUND)\
template <class TYPE>                                                         \
struct IsConvertible_Imp<FROM, TO, FROM_FUND, TO_FUND>                        \
     : bsl::integral_constant<bool, VALUE> {};
    // This partial specialization of 'bslmf::IsConvertible_Imp' derives from
    // 'bsl::integral_constant' having the specified macro argument 'VALUE'.
    // The specified macro arguments 'FROM' and 'TO' are cv-qualified type
    // expressions constructed out of the (template parameter) 'TYPE'.

#define BSLMF_ISCONVERTIBLE_VALUE(VALUE, FROM, TO, FROM_FUND, TO_FUND)        \
template <class FROM_TYPE, class TO_TYPE>                                     \
struct IsConvertible_Imp<FROM, TO, FROM_FUND, TO_FUND>                        \
     : bsl::integral_constant<bool, VALUE> {};
    // This partial specialization of 'bslmf::IsConvertible_Imp' derives from
    // 'bsl::integral_constant' having the specified macro argument 'VALUE'.
    // The specified macro arguments 'FROM' and 'TO' are cv-qualified type
    // expressions constructed out of 'FROM_TYPE' and 'TO_TYPE', respectively.

#define BSLMF_ISCONVERTIBLE_FORWARD(FROM, TO, FROM_FUND, TO_FUND)             \
template <class FROM_TYPE, class TO_TYPE>                                     \
struct IsConvertible_Imp<FROM, TO, FROM_FUND, TO_FUND>                        \
     : IsConvertible_Imp<FROM, TO, 0, 0> {};
    // This partial specialization of 'bslmf::IsConvertible_Imp' applies the
    // general mechanism for non-fundamental types.  The specified macro
    // arguments 'FROM' and 'TO' are cv-qualified type expressions constructed
    // out of 'FROM_TYPE' and 'TO_TYPE', respectively.

BSLMF_ISCONVERTIBLE_SAMETYPEVALUE(0, const volatile TYPE, const TYPE&, 1, 1)
BSLMF_ISCONVERTIBLE_SAMETYPEVALUE(0,       volatile TYPE, const TYPE&, 1, 1)
    // These two partial specializations are instantiated when a (possibly
    // 'const'-qualified) 'volatile' fundamental type is tested for
    // convertibility to its 'const' reference type.  The conversion shall
    // fail.

BSLMF_ISCONVERTIBLE_VALUE(1, const volatile FROM_TYPE, const TO_TYPE&, 1, 1)
BSLMF_ISCONVERTIBLE_VALUE(1,       volatile FROM_TYPE, const TO_TYPE&, 1, 1)
    // These two partial specializations are instantiated when a (possibly
    // 'const'-qualified) 'volatile' type is tested for convertibility to the
    // 'const' reference type of another fundamental type.  These partial
    // specializations will be picked up if the previous two fail to match.
    // The conversion shall succeed.

BSLMF_ISCONVERTIBLE_VALUE(1, const FROM_TYPE, const TO_TYPE&, 1, 1)
BSLMF_ISCONVERTIBLE_VALUE(1,       FROM_TYPE, const TO_TYPE&, 1, 1)
    // These two partial specializations are instantiated when a (possibly
    // 'const'-qualified) fundamental type is tested for convertibility to the
    // 'const' reference type of another fundamental type.  These partial
    // specializations will be picked up if the previous two fail to match.
    // The conversion shall succeed.

BSLMF_ISCONVERTIBLE_FORWARD(const volatile FROM_TYPE,
                            const volatile TO_TYPE&, 1, 1)
BSLMF_ISCONVERTIBLE_FORWARD(      volatile FROM_TYPE,
                            const volatile TO_TYPE&, 1, 1)
BSLMF_ISCONVERTIBLE_FORWARD(const          FROM_TYPE,
                            const volatile TO_TYPE&, 1, 1)
BSLMF_ISCONVERTIBLE_FORWARD(               FROM_TYPE,
                            const volatile TO_TYPE&, 1, 1)
    // These four partial specializations are instantiated when a (possibly
    // cv-qualified) fundamental type is tested for convertibility to the
    // 'const volatile' reference type of another fundamental type.

BSLMF_ISCONVERTIBLE_FORWARD(const volatile FROM_TYPE,
                                  volatile TO_TYPE&, 1, 1)
BSLMF_ISCONVERTIBLE_FORWARD(      volatile FROM_TYPE,
                                  volatile TO_TYPE&, 1, 1)
BSLMF_ISCONVERTIBLE_FORWARD(const          FROM_TYPE,
                                  volatile TO_TYPE&, 1, 1)
BSLMF_ISCONVERTIBLE_FORWARD(               FROM_TYPE,
                                  volatile TO_TYPE&, 1, 1)
    // These four partial specializations are instantiated when a (possibly
    // cv-qualified) fundamental type is tested for convertibility to the
    // 'volatile' reference type of another fundamental type.

BSLMF_ISCONVERTIBLE_FORWARD(const volatile FROM_TYPE, TO_TYPE&, 1, 1)
BSLMF_ISCONVERTIBLE_FORWARD(      volatile FROM_TYPE, TO_TYPE&, 1, 1)
BSLMF_ISCONVERTIBLE_FORWARD(const          FROM_TYPE, TO_TYPE&, 1, 1)
BSLMF_ISCONVERTIBLE_FORWARD(               FROM_TYPE, TO_TYPE&, 1, 1)
    // These four partial specializations are instantiated when a (possibly
    // cv-qualified) fundamental type is tested for convertibility to the
    // non-cv-qualified reference type of another fundamental type.

template <class FROM_TYPE, class TO_TYPE>
struct IsConvertible_Imp<const FROM_TYPE, TO_TYPE, 1, 1>
    : IsConvertible_Imp<const FROM_TYPE, double, 0, 0>::type {
    // This partial specialization is instantiated when the 'const' (template
    // parameter) fundamental 'FROM_TYPE' is tested for convertibility to
    // another (template parameter) fundamental 'TO_TYPE'.  This partial
    // specialization derives from
    // 'IsConvertible_Imp<const FROM_TYPE, double, 0, 0>' to avoid any
    // compilation warnings in case the 'TO_TYPE' is an integral type and
    // 'FROM_TYPE' is a floating-point type.
};

template <class FROM_TYPE, class TO_TYPE>
struct IsConvertible_Imp<FROM_TYPE, TO_TYPE, 1, 1>
    : IsConvertible_Imp<FROM_TYPE, double, 0, 0>::type {
    // This partial specialization is instantiated when the (template
    // parameter) fundamental 'FROM_TYPE' is tested for convertibility to
    // another (template parameter) fundamental 'TO_TYPE'.  This partial
    // specialization derives from 'IsConvertible_Imp<FROM_TYPE, double, 0, 0>'
    // to avoid any compilation warnings in case that the 'FROM_TYPE' is a
    // floating-point type and 'TO_TYPE' is an integral type.
};

template <class FROM_TYPE, class TO_TYPE>
struct IsConvertible_Imp<FROM_TYPE, TO_TYPE, 0, 1>
    : IsConvertible_Imp<FROM_TYPE, double, 0, 0>::type {
    // This partial specialization is instantiated when the (template
    // parameter) 'FROM_TYPE' is a non-fundamental type, and the (template
    // parameter) 'TO_TYPE' is a non-'void' fundamental type.  This partial
    // specialization derives from 'IsConvertible_Imp<FROM_TYPE, double, 0, 0>'
    // to avoid any compilation warnings in case that the 'FROM_TYPE' is a
    // floating-point type and the 'TO_TYPE' is an integral type.
};

template <class FROM_TYPE, class TO_TYPE>
struct IsConvertible_Imp<FROM_TYPE, TO_TYPE, 1, 0>
    : IsConvertible_Imp<int, TO_TYPE, 0, 0>::type {
    // This partial specialization is instantiated when the (template
    // parameter) 'FROM_TYPE' is a non-'void' fundamental type, and the
    // (template parameter) 'TO_TYPE' is a non-fundamental type.  This partial
    // specialization derives from 'IsConvertible_Imp<int, TO_TYPE, 0, 0>' to
    // avoid any compilation warnings in case that the 'FROM_TYPE' is a
    // floating-point type and the 'TO_TYPE' is an integral type.
};

#undef BSLMF_ISCONVERTIBLE_SAMETYPEVALUE
#undef BSLMF_ISCONVERTIBLE_VALUE
#undef BSLMF_ISCONVERTIBLE_FORWARD

#endif


template <class TO_TYPE>
struct IsConvertible_LazyTrait : bsl::add_lvalue_reference<
                                     typename bsl::add_const<
                                       typename bsl::remove_cv<TO_TYPE>::type
                                                                      >::type
                                                                      > {
};


template <class FROM_TYPE, class TO_TYPE>
struct IsConvertible_IsNeverConvertible
     : bsl::integral_constant<bool, bsl::is_void<FROM_TYPE>::value
                                    || bsl::is_array<TO_TYPE>::value
                                    || bsl::is_function<TO_TYPE>::value> {};

template <class FROM_TYPE, class TO_TYPE>
struct IsConvertible_FilterNeverConvertible
     : bsl::conditional<
                   IsConvertible_IsNeverConvertible<FROM_TYPE, TO_TYPE>::value,
                   bsl::false_type,
                   IsConvertible_Imp<
                                FROM_TYPE,
                                typename IsConvertible_LazyTrait<TO_TYPE>::type
                                    >
                       >::type {
};


template <class FROM_TYPE, class TO_TYPE>
struct IsConvertible_Conditional : bsl::conditional<
     bsl::is_void<TO_TYPE>::value,
     typename bsl::is_void<FROM_TYPE>::type,
     IsConvertible_FilterNeverConvertible<
                        typename IsConvertible_CheckComplete<FROM_TYPE>::type,
                        typename IsConvertible_CheckComplete<  TO_TYPE>::type>
                                                                      >::type {
};

}  // close package namespace
}  // close enterprise namespace

namespace bsl {

                         // ==============================
                         // struct is_convertible_dispatch
                         // ==============================

template <class FROM_TYPE, class TO_TYPE>
struct is_convertible_dispatch
    : BloombergLP::bslmf::IsConvertible_Conditional<FROM_TYPE, TO_TYPE>::type {
    // This 'struct' template implements the 'is_convertible_dispatch'
    // meta-function defined in the C++11 standard [meta.rel] to determine if
    // the (template parameter) 'FROM_TYPE' is convertible to the (template
    // parameter) 'TO_TYPE'.  This 'struct' derives from 'bsl::true_type' if
    // the 'FROM_TYPE' is convertible to 'TO_TYPE', and from 'bsl::false_type'
    // otherwise.  Note that both 'FROM_TYPE' and 'TO_TYPE' should be complete
    // types, arrays of unknown bound, or (possibly cv-qualified) 'void' types.
};

template <class TYPE>
struct is_convertible_dispatch<TYPE, TYPE&>
     : integral_constant< bool
                        , is_reference<TYPE>::value
                          || is_function<TYPE>::value
                          || is_const<TYPE>::value> {
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

template <class TYPE>
struct is_convertible_dispatch<TYPE, const TYPE&>
    : true_type {
};

template <class TYPE>
struct is_convertible_dispatch<TYPE, volatile TYPE&>
    : false_type {
};

template <class TYPE>
struct is_convertible_dispatch<TYPE, const volatile TYPE&>
    : false_type {
};

template <class TYPE>
struct is_convertible_dispatch<const TYPE, TYPE&>
    : false_type {
};

template <class TYPE>
struct is_convertible_dispatch<const TYPE, const TYPE&>
    : true_type {
};

template <class TYPE>
struct is_convertible_dispatch<const TYPE, volatile TYPE&>
    : false_type {
};

template <class TYPE>
struct is_convertible_dispatch<const TYPE, const volatile TYPE&>
    : false_type {
};

template <class TYPE>
struct is_convertible_dispatch<volatile TYPE, TYPE&>
    : false_type {
};

template <class TYPE>
struct is_convertible_dispatch<volatile TYPE, const TYPE&>
    : false_type {
};

template <class TYPE>
struct is_convertible_dispatch<volatile TYPE, volatile TYPE&>
    : false_type {
};

template <class TYPE>
struct is_convertible_dispatch<volatile TYPE, const volatile TYPE&>
    : false_type {
};

template <class TYPE>
struct is_convertible_dispatch<const volatile TYPE, TYPE&>
    : false_type {
};

template <class TYPE>
struct is_convertible_dispatch<const volatile TYPE, const TYPE&>
    : false_type {
};

template <class TYPE>
struct is_convertible_dispatch<const volatile TYPE, volatile TYPE&>
    : false_type {
};

template <class TYPE>
struct is_convertible_dispatch<const volatile TYPE, const volatile TYPE&>
    : false_type {
};

// The next group of partial specializations deal with various cases of
// converting to an lvalue-reference, which we make explicitly conform to the
// C++11 idiom of converting from an rvalue (which may be an lvalue-reference).

template <class TYPE>
struct is_convertible_dispatch<TYPE&, TYPE&> : true_type {
};

template <class TYPE>
struct is_convertible_dispatch<TYPE&, const TYPE&> : true_type {
};

template <class TYPE>
struct is_convertible_dispatch<TYPE&, volatile TYPE&> : true_type {
};

template <class TYPE>
struct is_convertible_dispatch<TYPE&, const volatile TYPE&> : true_type {
};

template <class TYPE>
struct is_convertible_dispatch<const TYPE&, TYPE&> : false_type {
};

template <class TYPE>
struct is_convertible_dispatch<const TYPE&, const TYPE&> : true_type {
};

template <class TYPE>
struct is_convertible_dispatch<const TYPE&, volatile TYPE&> : false_type {
};

template <class TYPE>
struct is_convertible_dispatch<const TYPE&, const volatile TYPE&> : true_type {
};

template <class TYPE>
struct is_convertible_dispatch<volatile TYPE&, TYPE&> : false_type {
};

template <class TYPE>
struct is_convertible_dispatch<volatile TYPE&, const TYPE&> : false_type {
};

template <class TYPE>
struct is_convertible_dispatch<volatile TYPE&, volatile TYPE&> : true_type {
};

template <class TYPE>
struct is_convertible_dispatch<volatile TYPE&, const volatile TYPE&>
    : true_type {
};

template <class TYPE>
struct is_convertible_dispatch<const volatile TYPE&, TYPE&> : false_type {
};

template <class TYPE>
struct is_convertible_dispatch<const volatile TYPE&, const TYPE&>
    : false_type {
};

template <class TYPE>
struct is_convertible_dispatch<const volatile TYPE&, volatile TYPE&>
    : false_type {
};

template <class TYPE>
struct is_convertible_dispatch<const volatile TYPE&, const volatile TYPE&>
    : true_type {
};

template <class TYPE>
struct is_convertible_dispatch<volatile TYPE, TYPE>
     : BloombergLP::bslmf::IsConvertible_Conditional<TYPE, TYPE>::type {
    // Correct handling of non-fundamental volatile conversions to self.  Note
    // that this is not trivially true, but tests that 'TYPE' is copy (or move)
    // constructible.
};

template <class FROM_TYPE, class TO_TYPE>
struct is_convertible_dispatch<FROM_TYPE, volatile TO_TYPE&>
    : false_type {
};

template <class FROM_TYPE, class TO_TYPE>
struct is_convertible_dispatch<FROM_TYPE, const volatile TO_TYPE&>
    : false_type {
};

template <class FROM_TYPE, class TO_TYPE>
struct is_convertible_dispatch<volatile FROM_TYPE&, volatile TO_TYPE&>
    : is_convertible_dispatch<FROM_TYPE*, TO_TYPE*>::type {
};

template <class FROM_TYPE, class TO_TYPE>
struct is_convertible_dispatch<volatile FROM_TYPE&, const volatile TO_TYPE&>
     : is_convertible_dispatch<FROM_TYPE, const volatile TO_TYPE&>::type {
};

                         // =====================
                         // struct is_convertible
                         // =====================

template <class FROM_TYPE>
struct EffectiveFromType : conditional<
              is_fundamental<FROM_TYPE>::value || is_pointer<FROM_TYPE>::value,
              typename remove_cv<FROM_TYPE>::type,
              FROM_TYPE> {
};

template <class FROM_TYPE, class TO_TYPE>
struct is_convertible_dispatch<volatile FROM_TYPE&, TO_TYPE>
     : bsl::conditional<
              bsl::is_fundamental<FROM_TYPE>::value,
              typename bsl::is_convertible_dispatch<FROM_TYPE, TO_TYPE>::type,
              typename BloombergLP::bslmf::IsConvertible_Conditional<
                                                            volatile FROM_TYPE,
                                                            TO_TYPE>::type
                       >::type {
};


template <class FROM_TYPE, class TO_TYPE>
struct is_convertible
    :  is_convertible_dispatch< typename EffectiveFromType<FROM_TYPE>::type
                              , TO_TYPE
                              >::type {
    // This 'struct' template implements the 'is_convertible_dispatch'
    // meta-function defined in the C++11 standard [meta.rel] to determine if
    // the (template parameter) 'FROM_TYPE' is convertible to the (template
    // parameter) 'TO_TYPE'.  This 'struct' derives from 'bsl::true_type' if
    // the 'FROM_TYPE' is convertible to 'TO_TYPE', and from 'bsl::false_type'
    // otherwise.  Note that both 'FROM_TYPE' and 'TO_TYPE' should be complete
    // types, arrays of unknown bound, or (possibly cv-qualified) 'void' types.
};

}  // close namespace bsl
#endif

namespace BloombergLP {
namespace bslmf {

                         // ====================
                         // struct IsConvertible
                         // ====================

template <class FROM_TYPE, class TO_TYPE>
struct IsConvertible : bsl::is_convertible<FROM_TYPE, TO_TYPE>::type {
    // This 'struct' template implements a meta-function to determine if the
    // (template parameter) 'FROM_TYPE' is convertible to the (template
    // parameter) 'TO_TYPE'.  This 'struct' derives from 'bsl::true_type' if
    // the 'FROM_TYPE' is convertible to 'TO_TYPE', and from 'bsl::false_type'
    // otherwise.  Note that both 'FROM_TYPE' and 'TO_TYPE' should be complete
    // types, arrays of unknown bound, or (possibly cv-qualified) 'void' types.
};

}  // close package namespace


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
