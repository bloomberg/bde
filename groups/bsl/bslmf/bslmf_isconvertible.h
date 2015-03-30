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

#ifndef INCLUDED_BSLMF_INTEGRALCONSTANT
#include <bslmf_integralconstant.h>
#endif

#ifndef INCLUDED_BSLMF_ISFUNDAMENTAL
#include <bslmf_isfundamental.h>
#endif

#ifndef INCLUDED_BSLMF_MATCHANYTYPE
#include <bslmf_matchanytype.h>
#endif

#ifndef INCLUDED_BSLMF_REMOVECV
#include <bslmf_removecv.h>
#endif

#ifndef INCLUDED_BSLS_PLATFORM
#include <bsls_platform.h>
#endif

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

    template <class TYPE>
    static no_type match(const TYPE&);
        // Return 'yes_type' if the (template parameter) 'TYPE' is
        // 'IsConvertible_Match', and 'no_type' otherwise.

    template <class TYPE>
    static no_type match(const volatile TYPE&);
        // Return 'yes_type' if the (template parameter) 'TYPE' is
        // 'IsConvertible_Match' and 'no_type' otherwise.
};

                         // ========================
                         // struct IsConvertible_Imp
                         // ========================

template <class FROM_TYPE, class TO_TYPE
#if defined(BSLS_PLATFORM_CMP_GNU) || defined(BSLS_PLATFORM_CMP_CLANG)
         , int IS_FROM_FUNDAMENTAL = IsFundamental<FROM_TYPE>::value
         , int IS_TO_FUNDAMENTAL   = IsFundamental<TO_TYPE>::value
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
                           (TypeRep<Test>::rep(), TypeRep<FROM_TYPE>::rep()))))
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

#if defined(BSLS_PLATFORM_CMP_GNU) || defined(BSLS_PLATFORM_CMP_CLANG)

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
    // specialization derives from 'IsConvertible_Imp<const FROM_TYPE, double,
    // 0, 0>' to avoid any compilation warnings in case the 'TO_TYPE' is an
    // integral type and 'FROM_TYPE' is a floating-point type.
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
    // to avoid any compilation warnings in case that the 'FROM_TYPE' is
    // a floating-point type and the 'TO_TYPE' is an integral type.
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

}  // close package namespace

}  // close enterprise namespace

namespace bsl {

                         // =====================
                         // struct is_convertible
                         // =====================

template <class FROM_TYPE, class TO_TYPE>
struct is_convertible
    : BloombergLP::bslmf::IsConvertible_Imp<
                              FROM_TYPE,
                              typename remove_cv<TO_TYPE>::type const&>::type {
    // This 'struct' template implements the 'is_convertible' meta-function
    // defined in the C++11 standard [meta.rel] to determine if the (template
    // parameter) 'FROM_TYPE' is convertible to the (template parameter)
    // 'TO_TYPE'.  This 'struct' derives from 'bsl::true_type' if the
    // 'FROM_TYPE' is convertible to 'TO_TYPE', and from 'bsl::false_type'
    // otherwise.  Note that both 'FROM_TYPE' and 'TO_TYPE' should be complete
    // types, arrays of unknown bound, or (possibly cv-qualified) 'void' types.
};

template <class FROM_TYPE, class TO_TYPE>
struct is_convertible<FROM_TYPE, TO_TYPE&>
    : BloombergLP::bslmf::IsConvertible_Imp<FROM_TYPE, TO_TYPE&>::type {
    // This partial specialization is instantiated for the case where the
    // (template parameter) 'TO_TYPE' is a reference type.
};

template <class FROM_TYPE>
struct is_convertible<FROM_TYPE, void> : false_type {
    // This partial specialization deriving from 'bsl::false_type' is
    // instantiated for the case where the (template parameter) 'FROM_TYPE' is
    // a non-'void' type and is being converted to the 'void' type.
};

template <class FROM_TYPE, class TO_TYPE>
struct is_convertible<volatile FROM_TYPE, TO_TYPE>
   : BloombergLP::bslmf::IsConvertible_Imp<volatile FROM_TYPE, TO_TYPE>::type {
    // This partial specialization is instantiated for the case where the
    // (template parameter) 'FROM_TYPE' is a 'volatile' type.
};

template <class FROM_TYPE, class TO_TYPE>
struct is_convertible<volatile FROM_TYPE, TO_TYPE&>
  : BloombergLP::bslmf::IsConvertible_Imp<volatile FROM_TYPE, TO_TYPE&>::type {
    // This partial specialization is instantiated for the case where the
    // (template parameter) 'FROM_TYPE' is a 'volatile' type and the (template
    // parameter) 'TO_TYPE' is a reference type.
};

template <class FROM_TYPE>
struct is_convertible<volatile FROM_TYPE, void> : false_type {
    // This partial specialization deriving from 'bsl::false_type' is
    // instantiated for the case where the (template parameter) 'FROM_TYPE' is
    // a 'volatile' type and is being converted to the 'void' type.
};

template <class FROM_TYPE, class TO_TYPE>
struct is_convertible<volatile FROM_TYPE&, TO_TYPE>
  : BloombergLP::bslmf::IsConvertible_Imp<volatile FROM_TYPE&, TO_TYPE>::type {
    // This partial specialization is instantiated for the case where the
    // (template parameter) 'FROM_TYPE' is a reference to a 'volatile' type.
};

template <class FROM_TYPE, class TO_TYPE>
struct is_convertible<volatile FROM_TYPE&, TO_TYPE&>
 : BloombergLP::bslmf::IsConvertible_Imp<volatile FROM_TYPE&, TO_TYPE&>::type {
    // This partial specialization is instantiated for the case where the
    // (template parameter) 'FROM_TYPE' is a reference to a 'volatile' type and
    // the (template parameter) 'TO_TYPE' is a reference type.
};

template <class FROM_TYPE>
struct is_convertible<volatile FROM_TYPE&, void> : false_type {
    // This partial specialization deriving from 'bsl::false_type' is
    // instantiated for the case where the (template parameter) 'FROM_TYPE' is
    // a reference to a 'volatile' type and is being converted to the 'void'
    // type.
};

template <class TO_TYPE>
struct is_convertible<void, TO_TYPE> : false_type {
    // This partial specialization deriving from 'bsl::false_type' is
    // instantiated for the case where the 'void' type is being converted to
    // the non-'void' (template parameter) 'TO_TYPE'.
};

template <class TO_TYPE>
struct is_convertible<void, TO_TYPE&> : false_type {
    // This partial specialization deriving from 'bsl::false_type' is
    // instantiated for the case where the 'void' type is being converted to
    // the (template parameter) 'TO_TYPE', which is a reference type.
};

template <>
struct is_convertible<void, void> : true_type {
    // This partial specialization deriving from 'bsl::true_type' is
    // instantiated for the case where the 'void' type is being converted to
    // the 'void' type.
};

}  // close namespace bsl

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
