// bslmf_isconvertible.h                                              -*-C++-*-
#ifndef INCLUDED_BSLMF_ISCONVERTIBLE
#define INCLUDED_BSLMF_ISCONVERTIBLE

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a compile-time type conversion checker.
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
// The two meta-functions are functionally equivalent.  The major difference
// between them is that the result for 'bsl::is_convertible' is indicated by
// the class member 'value', while the result for 'bslmf::IsConvertible' is
// indicated by the class member 'VALUE'.
//
// Note that 'bsl::is_convertible' should be preferred over
// 'bslmf::IsConvertible', and in general, should be used by new components.
// Note also that the result is undefined if either parameter is 'void'.  Note
// also that 'bslmf::IsConvertible' can produce compiler errors if the
// conversion is ambiguous.  For example,
//..
//  struct A {};
//  struct B : public A {};
//  struct C : public A {};
//  struct D : public B, public C {};
//
//  static int const C = bslmf::IsConvertible<D*, A*>::value; // ERROR!
//..
//
///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1: Select Function Based on Convertibility
/// - - - - - - - - - - - - - - - - - - - - - - - - -
// This 'bsl::is_convertible' meta-function can be used to select an
// appropriate function (at compile time) based on the convertibility of one
// type to another without causing a compiler error by actually trying the
// conversion.  This implementation technique is especially useful when
// building generic containers that use an allocator protocol to acquire
// resources.  As a design goal, we want to pass the container's allocator to
// contained types if they provide an appropriate constructor.
//
// Suppose we are implementing some container's 'addObj' method that adds a new
// object (in its default state) of the container's template parameter 'TYPE'.
// The method calls an overloaded function 'createObj' to create a new object
// of the parameterized type in its internal array.  The idea is to invoke one
// version of 'createObj' if the type provides a constructor that takes a
// pointer to an allocator as its sole argument, and another version if the
// type provides only a default constructor.
//
// First, we define the allocator to be used:
//..
//  struct MyAllocator {
//      // This is a user-defined allocator.
//
//      void *allocate(size_t sz)
//      {
//          return operator new(sz);
//      }
//
//      void  deallocate(void *address)
//      {
//          operator delete(address);
//      }
//  };
//..
// Then, we define two 'struct's, 'Foo' and 'Bar'.  The constructor of 'Foo'
// takes a 'MyAllocator' object pointer while that of 'Bar' does not:
//..
//  struct Foo {
//      Foo(MyAllocator *) {}
//  };
//
//  struct Bar {
//      Bar() {}
//  };
//..
// Next, we define the first 'createObj' function that takes a
// 'bsl::false_type' as its last argument, whereas the second 'createObj'
// function takes a 'bsl::true_type' object.  The result of the
// 'bsl::is_convertible' meta-function (i.e., its 'Type' member) is used to
// create the last argument passed to 'createObj'.  Neither version of
// 'createObj' makes use of this argument -- it is used only to differentiate
// the argument list so we can overload the function.
//..
//  template<class TYPE>
//  void createObj(TYPE *space, MyAllocator *, bsl::false_type)
//  {
//     // Use the type's default constructor if
//     // 'bsl::is_convertible<MyAllocator*, TYPE>::value == false', i.e.,
//     // there is no conversion from a 'MyAllocator' pointer to a 'TYPE'.
//
//     new (space) TYPE();
//  }
//
//  template<class TYPE>
//  void createObj(TYPE *space, MyAllocator *alloc, bsl::true_type)
//  {
//     // Use the type's constructor that takes a pointer to an allocator if
//     // 'bsl::is_convertible<MyAllocator*, TYPE>::value == true', i.e., there
//     // is a conversion from a 'MyAllocator' pointer to a 'TYPE'.
//
//     new (space) TYPE(alloc);
//  }
//..
// Now, we define our 'MyContainer' type and implement the 'addObj' method:
//..
//  template <class TYPE>
//  class MyContainer {
//      // DATA
//      TYPE *d_array_p;         // underlying array
//
//      MyAllocator *d_alloc_p;  // allocator protocol
//
//      int d_length;            // logical length of array
//
//      // ...
//
//      void resizeInternalArrayIfNeeded() { /* ... */ };
//
//    public:
//      // CREATORS
//      MyContainer(MyAllocator *alloc)
//      : d_alloc_p(alloc)
//      , d_length(0)
//      {
//          d_array_p = (TYPE*) d_alloc_p->allocate(sizeof(TYPE));
//      }
//
//      ~MyContainer()
//      {
//          d_alloc_p->deallocate(d_array_p);
//      }
//
//      // MANIPULATORS
//      void addObj()
//      {
//          resizeInternalArrayIfNeeded();
//          // Work around some Sun's compiler weirdness the code won't compile
//          // with just the typename
//          typedef typename bsl::is_convertible<MyAllocator*, TYPE>::type
//                                                                     isAlloc;
//          createObj(d_array_p + d_length++, d_alloc_p, isAlloc());
//      }
//  };
//..
// Notice that in 'addObj' method we use 'bsl::is_convertible' to get a
// 'bsl::false_type' or 'bsl::true_type', and then call the corresponding
// overloaded 'createObj' method.
//
// Finally, we instantiate 'MyContainer' with both 'Foo' and 'Bar' types, and
// call 'addObj' on both containers:
//..
//  MyAllocator a;
//
//  MyContainer<Foo> fc(&a);
//  fc.addObj();
//
//  MyContainer<Bar> bc(&a);
//  bc.addObj();
//..

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLMF_MATCHANYTYPE
#include <bslmf_matchanytype.h>
#endif

#ifndef INCLUDED_BSLMF_INTEGRALCONSTANT
#include <bslmf_integralconstant.h>
#endif

#ifndef INCLUDED_BSLMF_ISFUNDAMENTAL
#include <bslmf_isfundamental.h>
#endif

#ifndef INCLUDED_BSLMF_REMOVECV
#include <bslmf_removecv.h>
#endif

#ifndef INCLUDED_BSLS_PLATFORM
#include <bsls_platform.h>
#endif

namespace BloombergLP {

namespace bslmf {

                         // ==============================
                         // private implementation classes
                         // ==============================

// IMPLEMENTATION NOTE: Why is this so complicated?  The 'IsConvertible'
// meta-function is fairly simple at its core (see 'IsConvertible_Imp', below).
// All that's needed is to have a function that can be called with 'TO_TYPE'
// and test if that function is callable with 'FROM_TYPE'.  The complications
// are as follows:
//
// It is should be possible to determine that some conversions are valid even
// if 'FROM_TYPE' is an incomplete type.  For example, given an incomplete
// type, X, the following can be assumed:
//..
//  X is convertible to X
//  X is convertible to const X
//  const X is convertible to X
//  cvq X& is convertible to cvq X
//  etc.
//..
// It is not acceptable for 'IsConvertible' to fail to compile when
// instantiated on incomplete types, so we make the additional assumption that
// nothing else is convertible to or from X, even though additional information
// may prove that to be incorrect.
//
// In order to accept incomplete types, we must use 'const TO_TYPE&' instead of
// 'TO_TYPE' in our test function parameter.  This causes a separate problem:
// if 'FROM_TYPE' is volatile, then 'const TO_TYPE&' will cause incorrect
// matches.  Thus we suppress the addition of the const reference if
// 'FROM_TYPE' is volatile, at the expense that compilation will fail in the
// very unlikely event that 'FROM_TYPE' is volatile and 'TO_TYPE' is an
// incomplete type.  We must also suppress the addition of 'const' if 'TO_TYPE'
// is already a reference.
//
// Finally, we would like this metafunction to work even if 'FROM_TYPE' and/or
// 'TO_TYPE' are 'void', so we must add specializations for these cases.
//
// In the end, we end up with template for the general case and 11 partial and
// full specializations corresponding to the cross-product of the following
// parameter patterns:
//..
//  FROM_TYPE is volatile, a volatile reference, void, or other
//  TO_TYPE   is a reference, void, or other
//..
// To make things more complicated, the Sun CC 5.2 compiler has serious bugs
// that cause incorrect matching behavior, allowing a function to be called
// with a 'const' argument even if its parameter is a non-const reference.  We
// create the 'IsConvertible_Overload' meta-function to work around this
// problem.
//
// Finally, the 'gcc' compiler generates copious warnings for floating point to
// integral conversions, and so we add convertibility from any non-void
// fundamental type to any other non-void fundamental type as a special case.
// Note that this does *not* remove warnings when a floating point to integral
// conversion is part of a conversion sequence, but it does take care of the
// most common case.  A full solution to this warning problem is likely to be
// much more involved.

struct IsConvertible_Match {
    // Private functions to check for successful match.  Sun CC 5.2 requires
    // that this struct not be nested within 'IsConvertible_Imp'.

    typedef struct { char a;    } yes_type;
    typedef struct { char a[2]; } no_type;

    static yes_type match(IsConvertible_Match&);
    template <typename T>
    static no_type match(const T&);
    template <typename T>
    static no_type match(const volatile T&);
        // Return 'yes_type' if called on an argument of type
        // 'IsConvertible_Match' and 'no_type' otherwise.
};

template <typename FROM_TYPE, typename TO_TYPE
#if defined(BSLS_PLATFORM_CMP_GNU)
         , int IS_FROM_FUNDAMENTAL = IsFundamental<FROM_TYPE>::value
         , int IS_TO_FUNDAMENTAL   = IsFundamental<TO_TYPE>::value
#endif
         >
struct IsConvertible_Imp {
    // Private class.

    // General instance: Implements 'IsConvertible<FROM_TYPE, TO_TYPE>' where
    // conversion to 'TO_TYPE' is not necessarily the same as conversion to
    // 'const TO_TYPE&'.  Will not compile if 'TO_TYPE' is an incomplete type
    // (although it may be a pointer or reference to incomplete type).

  private:
    struct Test
    {
        // A unique (empty) type returned by the comma operator.

        IsConvertible_Match& operator, (TO_TYPE) const;
            // Return a reference to type 'IsConvertible_Match' if called with
            // an argument convertible to 'TO_TYPE'; otherwise the default
            // comma operator will simply return its second argument (of type
            // 'TO_TYPE').
    };

  public:

#ifdef BSLS_PLATFORM_CMP_MSVC
#   pragma warning(push)
#   pragma warning(disable: 4244) //loss of precision warning ignored
#endif
    enum {
        // Invoke the comma operator with 'Test&' on the left and 'FROM_TYPE'
        // on the right.  If 'FROM_TYPE' is convertible to 'TO_TYPE', the comma
        // will return 'IsConvertible_Match' and cause a match, otherwise it
        // will return 'FROM_TYPE', which does not match.
        value = (sizeof(IsConvertible_Match::yes_type) ==
                 sizeof(IsConvertible_Match::match(
                           (TypeRep<Test>::rep(), TypeRep<FROM_TYPE>::rep()))))
            // 'value' will be true if 'FROM_TYPE' is convertible to 'TO_TYPE'.
    };
#ifdef BSLS_PLATFORM_CMP_MSVC
#   pragma warning(pop)
#endif

    typedef bsl::integral_constant<bool, value> type;
        // 'bsl::true_type' if 'FROM_TYPE' is convertible to 'TO_TYPE', else
        // 'bsl::false_type'.
};

#if defined(BSLS_PLATFORM_CMP_GNU)
// Partial specializations when 'TO_TYPE' is a fundamental type.  The sole
// purpose of these specializations is to remove gcc warnings when converting
// from 'FLOAT' to 'INT' as above, where 'INT' and 'FLOAT' represent an integer
// type and a floating point type, respectively.  Because of the large number
// of specializations, and to make the intent of the code clearer, we will use
// three macros:

#define BSLMF_ISCONVERTIBLE_SAMETYPEVALUE(VALUE, FROM, TO, FROM_FUND, TO_FUND)\
template <typename TYPE>                                                      \
struct IsConvertible_Imp<FROM, TO, FROM_FUND, TO_FUND>                        \
    : bsl::integral_constant<bool, VALUE> {};
    // Define a partial specialization of 'bslmf::IsConvertible_Imp' in terms
    // of a single template parameter 'TYPE', defined as
    // 'bsl::integral_constant<bool, VALUE>' for the specified macro argument
    // 'VALUE'.  The specified macro arguments 'FROM' and 'TO' are cv-qualified
    // type expressions constructed out of 'TYPE'.

#define BSLMF_ISCONVERTIBLE_VALUE(VALUE, FROM, TO, FROM_FUND, TO_FUND)        \
template <typename FROM_TYPE, typename TO_TYPE>                               \
struct IsConvertible_Imp<FROM, TO, FROM_FUND, TO_FUND>                        \
    : bsl::integral_constant<bool, VALUE> {};
    // Define a partial specialization of 'bslmf::IsConvertible_Imp' in terms
    // of two template parameters 'FROM_TYPE' and 'TO_TYPE', defined as
    // 'bsl::integral_constant<bool, VALUE>' for the specified macro argument
    // 'VALUE'.  The specified macro arguments 'FROM' and 'TO' are cv-qualified
    // type expression constructed out of 'FROM_TYPE' and 'TO_TYPE',
    // respectively.

#define BSLMF_ISCONVERTIBLE_FORWARD(FROM, TO, FROM_FUND, TO_FUND)             \
template <typename FROM_TYPE, typename TO_TYPE>                               \
struct IsConvertible_Imp<FROM, TO, FROM_FUND, TO_FUND>                        \
    : IsConvertible_Imp<FROM, TO, 0, 0> {};
    // Define a partial specialization of 'bslmf::IsConvertible_Imp' in terms
    // of two template parameters 'FROM_TYPE' and 'TO_TYPE', that simply
    // applies the general mechanism for non-fundamental types.  The specified
    // macro arguments 'FROM' and 'TO' are cv-qualified type expressions
    // constructed out of 'FROM_TYPE' and 'TO_TYPE', respectively.

// The logic we follow is to rely on the above implementation in cases that we
// know never produce a warning.  There are two cases we must guard against:
//
// (1) When 'TO_TYPE' is fundamental (but not void), and the conversion from
// 'FROM_TYPE' to 'TO_TYPE' would succeed: there is a risk that 'FROM_TYPE' is
// a floating point type, or convertible to a floating point type, and
// 'TO_TYPE' is integral.  But then the conversion would also succeed by
// replacing 'TO_TYPE' by 'double', which produces no warning.  This is
// *almost* an equivalence (i.e., the conversion fails if it fails when
// replacing 'TO_TYPE' by 'double'), with the only exception being when both
// types are fundamental, and the conversion is from 'volatile FROM_TYPE' to
// 'const TO_TYPE&' where both 'FROM_TYPE' and 'TO_TYPE' are cv-unqualified.
// The latter conversion should always fail if 'FROM_TYPE' is the same as
// 'TO_TYPE', and always succeed if 'FROM_TYPE' is also a fundamental type
// (note that testing so would trigger the warning).
//
// (2) When 'TO_TYPE' is not fundamental: there is a risk that 'FROM_TYPE' is a
// floating point type, and 'TO_TYPE' has an implicit constructor from an
// integral type.  But then the conversion would also succeed by replacing
// 'FROM_TYPE' by 'int', which produces no warning.  In that case, there is a
// strict equivalence between the two definitions, since the 'volatile' to
// 'const&' conversion is only a problem when both types are fundamental.

// SECTION 1: BOTH TYPES ARE FUNDAMENTAL
// -------------------------------------
// The following six partial specializations will match if TO_TYPE is exactly a
// 'const&'.

BSLMF_ISCONVERTIBLE_SAMETYPEVALUE(0, const volatile TYPE, const TYPE&, 1, 1)
BSLMF_ISCONVERTIBLE_SAMETYPEVALUE(0, volatile TYPE, const TYPE&, 1, 1)
    // When both cv-unqualified types are fundamental, conversion should fail
    // if they are the same.

BSLMF_ISCONVERTIBLE_VALUE(1, const volatile FROM_TYPE, const TO_TYPE&, 1, 1)
BSLMF_ISCONVERTIBLE_VALUE(1, volatile FROM_TYPE, const TO_TYPE&, 1, 1)
    // When both cv-unqualified types are fundamental, conversion should
    // succeed if they differ.  These specializations will be picked up if the
    // previous one fail to match (i.e., if the cv-unqualified types differ).

BSLMF_ISCONVERTIBLE_VALUE(1, const FROM_TYPE, const TO_TYPE&, 1, 1)
BSLMF_ISCONVERTIBLE_VALUE(1, FROM_TYPE, const TO_TYPE&, 1, 1)
    // When both cv-unqualified types are fundamental, conversion to a const&
    // should always succeed except in cases above.  These specializations will
    // be picked up if the previous one fails to match (i.e., if the
    // 'FROM_TYPE' is not volatile).

// The next eight partial specializations will match if 'TO_TYPE' is a
// 'volatile' reference (const or not).  In that case, we can just pass through
// to the usual implementation since it is never going to make a temporary,
// hence no implicit conversions generating warnings.

// There are eight of them to avoid triggering ambiguities in the template
// instantiation process, when We systematically define any specialization for
// 'FROM_TYPE' with the four cv-qualifiers combinations.

    // When 'TO_TYPE' is 'const volatile&'.
BSLMF_ISCONVERTIBLE_FORWARD(const volatile FROM_TYPE,
                            const volatile TO_TYPE&, 1, 1)
BSLMF_ISCONVERTIBLE_FORWARD(volatile FROM_TYPE, const volatile TO_TYPE&, 1, 1)
BSLMF_ISCONVERTIBLE_FORWARD(const FROM_TYPE, const volatile TO_TYPE&, 1, 1)
BSLMF_ISCONVERTIBLE_FORWARD(FROM_TYPE, const volatile TO_TYPE&, 1, 1)

    // When 'TO_TYPE' is 'const volatile&'.
BSLMF_ISCONVERTIBLE_FORWARD(const volatile FROM_TYPE, volatile TO_TYPE&, 1, 1)
BSLMF_ISCONVERTIBLE_FORWARD(volatile FROM_TYPE, volatile TO_TYPE&, 1, 1)
BSLMF_ISCONVERTIBLE_FORWARD(const FROM_TYPE, volatile TO_TYPE&, 1, 1)
BSLMF_ISCONVERTIBLE_FORWARD(FROM_TYPE, volatile TO_TYPE&, 1, 1)

// The next four partial specializations will match if 'TO_TYPE' is a
// non-cv-qualified reference to a fundamental type, since all matches to
// 'const volatile TO_TYPE&', 'const TO_TYPE&', and 'volatile TO_TYPE&' have
// been described above.

BSLMF_ISCONVERTIBLE_FORWARD(const volatile FROM_TYPE, TO_TYPE&, 1, 1)
BSLMF_ISCONVERTIBLE_FORWARD(volatile FROM_TYPE, TO_TYPE&, 1, 1)
BSLMF_ISCONVERTIBLE_FORWARD(const FROM_TYPE, TO_TYPE&, 1, 1)
BSLMF_ISCONVERTIBLE_FORWARD(FROM_TYPE, TO_TYPE&, 1, 1)

// The last two partial specializations will be picked up by the compiler when
// 'TO_TYPE' is a non-reference fundamental type.  They do FORWARD with a
// slight twist: Casting to a double instead of an int to avoid the warning in
// case the 'TO_TYPE' is an integral type and the 'FROM_TYPE' is a floating
// point type.

template <typename FROM_TYPE, typename TO_TYPE>
struct IsConvertible_Imp<const FROM_TYPE, TO_TYPE, 1, 1>
    : IsConvertible_Imp<const FROM_TYPE, double, 0, 0>::type
{
};

template <typename FROM_TYPE, typename TO_TYPE>
struct IsConvertible_Imp<FROM_TYPE, TO_TYPE, 1, 1>
    : IsConvertible_Imp<FROM_TYPE, double, 0, 0>::type
{
};

// SECTION 2: ONLY THE 'TO_TYPE' IS FUNDAMENTAL
// --------------------------------------------
// This partial specialization will be picked up by the compiler when 'TO_TYPE'
// is a fundamental (non-void) type, but 'FROM_TYPE' is not.  It does FORWARD
// with a slight twist: Casting to a double to avoid the warning in case the
// 'TO_TYPE' is an integral type and the 'FROM_TYPE' is a floating point type.

template <typename FROM_TYPE, typename TO_TYPE>
struct IsConvertible_Imp<FROM_TYPE, TO_TYPE, 0, 1>
    : IsConvertible_Imp<FROM_TYPE, double, 0, 0>::type
{
};

// SECTION 3: ONLY THE 'FROM_TYPE' IS FUNDAMENTAL
// ----------------------------------------------
// This partial specialization will be picked up by the compiler when
// 'FROM_TYPE' is a fundamental (non-void) type, but 'TO_TYPE' is not.  It does
// FORWARD with a slight twist: Casting from an int to avoid the warning in
// case the 'TO_TYPE' is an integral type and the 'FROM_TYPE' is a floating
// point type.

template <typename FROM_TYPE, typename TO_TYPE>
struct IsConvertible_Imp<FROM_TYPE, TO_TYPE, 1, 0>
    : IsConvertible_Imp<int, TO_TYPE, 0, 0>::type
{
};

#undef BSLMF_ISCONVERTIBLE_SAMETYPEVALUE
#undef BSLMF_ISCONVERTIBLE_VALUE
#undef BSLMF_ISCONVERTIBLE_FORWARD
#endif

}  // close package namespace

}  // close enterprise namespace

namespace bsl {

template <typename FROM_TYPE, typename TO_TYPE>
struct is_convertible
    : BloombergLP::bslmf::IsConvertible_Imp<
                                FROM_TYPE,
                                typename remove_cv<TO_TYPE>::type const&>::type
    // Implement a meta function which computes -- at compile time -- whether
    // 'FROM_TYPE' is convertible to 'TO_TYPE'.  Note that if 'TO_TYPE' is not
    // a reference type, then the cv-qualification of 'TO_TYPE' is ignored.

    // We pass in a const& because the 'TO_TYPE' may not have a
    // copy-constructor but the 'FROM_TYPE' could have a conversion operator to
    // a const& (not a value), and we still want the conversion to succeed.
    // Also, if the TO_TYPE is incomplete, we don't want to require a
    // copy-constructor.
{
};

template <typename FROM_TYPE, typename TO_TYPE>
struct is_convertible<FROM_TYPE, TO_TYPE&>
    : BloombergLP::bslmf::IsConvertible_Imp<FROM_TYPE, TO_TYPE&>::type
    // Partial specialization of 'IsConvertible<FROM_TYPE, TO_TYPE>' for the
    // case where 'TO_TYPE' is a reference.
{
};

template <typename FROM_TYPE>
struct is_convertible<FROM_TYPE, void> : false_type
    // Partial specialization of 'IsConvertible<FROM_TYPE, TO_TYPE>' for the
    // case where 'TO_TYPE' is 'void'.  Evaluates to false.
{
};

template <typename FROM_TYPE, typename TO_TYPE>
struct is_convertible<volatile FROM_TYPE, TO_TYPE>
    : BloombergLP::bslmf::IsConvertible_Imp<volatile FROM_TYPE, TO_TYPE>::type
    // Partial specialization of 'IsConvertible<FROM_TYPE, TO_TYPE>' for for
    // the case where 'FROM_TYPE' is volatile.
{
};

template <typename FROM_TYPE, typename TO_TYPE>
struct is_convertible<volatile FROM_TYPE, TO_TYPE&>
    : BloombergLP::bslmf::IsConvertible_Imp<volatile FROM_TYPE, TO_TYPE&>::type
    // Partial specialization of 'IsConvertible<FROM_TYPE, TO_TYPE>' for for
    // the case where 'FROM_TYPE' is volatile and 'TO_TYPE' is a reference.
{
};

template <typename FROM_TYPE>
struct is_convertible<volatile FROM_TYPE, void> : false_type
    // Partial specialization of 'IsConvertible<FROM_TYPE, TO_TYPE>' for for
    // the case where 'FROM_TYPE' is volatile and 'TO_TYPE' is void.  Evaluates
    // to false.  In that case, disable the check for fundamentals.
{
};

template <typename FROM_TYPE, typename TO_TYPE>
struct is_convertible<volatile FROM_TYPE&, TO_TYPE>
    : BloombergLP::bslmf::IsConvertible_Imp<volatile FROM_TYPE&, TO_TYPE>::type
    // Partial specialization of 'IsConvertible<FROM_TYPE, TO_TYPE>' for for
    // the case where 'FROM_TYPE' is a reference to 'volatile'.
{
};

template <typename FROM_TYPE, typename TO_TYPE>
struct is_convertible<volatile FROM_TYPE&, TO_TYPE&>
   : BloombergLP::bslmf::IsConvertible_Imp<volatile FROM_TYPE&, TO_TYPE&>::type
    // Partial specialization of 'IsConvertible<FROM_TYPE, TO_TYPE>' for for
    // the case where 'FROM_TYPE' is a reference to 'volatile' and 'TO_TYPE' is
    // a reference.
{
};

template <typename FROM_TYPE>
struct is_convertible<volatile FROM_TYPE&, void> : false_type
    // Partial specialization of 'IsConvertible<FROM_TYPE, TO_TYPE>' for for
    // the case where 'FROM_TYPE' is a reference to 'volatile' and 'TO_TYPE' is
    // void.  Evaluates to false.
{
};

template <typename TO_TYPE>
struct is_convertible<void, TO_TYPE> : false_type
    // Partial specialization of 'IsConvertible<FROM_TYPE, TO_TYPE>' for for
    // the case where 'FROM_TYPE' is void.  Evaluates to false.
{
};

template <typename TO_TYPE>
struct is_convertible<void, TO_TYPE&> : false_type
    // Partial specialization of 'IsConvertible<FROM_TYPE, TO_TYPE>' for for
    // the case where 'FROM_TYPE' is void and 'TO_TYPE' is a reference.
    // Evaluates to false.
{
};

template <>
struct is_convertible<void, void> : true_type
    // Specialization of 'IsConvertible<FROM_TYPE, TO_TYPE>' for for the case
    // where 'FROM_TYPE' and 'TO_TYPE' are both 'void'.  Evaluates to true.
{
};

}  // close namespace bsl

namespace BloombergLP {

namespace bslmf {

                         // ====================
                         // struct IsConvertible
                         // ====================

template <typename FROM_TYPE, typename TO_TYPE>
struct IsConvertible : bsl::is_convertible<FROM_TYPE, TO_TYPE>::type
{
};

}  // close package namespace


}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright (C) 2012 Bloomberg L.P.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
// ----------------------------- END-OF-FILE ----------------------------------
