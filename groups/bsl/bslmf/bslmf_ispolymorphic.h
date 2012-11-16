// bslmf_ispolymorphic.h                                              -*-C++-*-
#ifndef INCLUDED_BSLMF_ISPOLYMORPHIC
#define INCLUDED_BSLMF_ISPOLYMORPHIC

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a compile-time check for polymorphic types.
//
//@CLASSES:
//  bsl::is_polymorphic: standard meta-function for detecting polymorphic types
//  bslmf::IsPolymorphic: meta-function for detecting polymorphic types
//
//@DESCRIPTION: This component defines two meta-functions,
// 'bsl::is_polymorphic' and 'BloombergLP::bslmf::IsPolymorphic', both of which
// may be used to query whether a type is a polymorphic class as defined in the
// C++11 standard [class.virtual].  A class is polymorphic if it has virtual
// functions.  Note that the destructor of such a class should *always* be
// declared 'virtual'.  Therefore, another definition of polymorphic is whether
// a class has a virtual destructor.
//
// 'bsl::is_polymorphic' has the same syntax as the 'is_polymorphic' template
// defined in the C++11 standard [meta.unary.prop], while
// 'bslmf::IsPolymorphic' was devised before 'is_polymorphic' was standardized.
// 'bsl::is_polymorphic' meets the requirements of the C++11 standard with two
// exceptions:
//
//: 1 The compilation will fail if the meta-function is used to evaluate a
//:   'union' type, unless the one of the following complier, which provides an
//:   intrinsic operation to detect this specific trait, are used:
//:     o gcc 4.3 or later
//:     o Visual C++ 2008 or later
//:
//: 2 The meta-function will yield false positives, claiming non-polymorphic
//:   types *are* polymorphic, for types using virtual inheritance.  This case
//:   is known to be handled *correctly* for the following compilers only:
//:     o Compilers with intrinsic support, listed above
//:     o IBM XLC
//
// The two meta-functions are functionally equivalent.  The major difference
// between them is that the result for 'bsl::is_polymorphic' is indicated by
// the class member 'value', while the result for 'bslmf::IsPolymorphic' is
// indicated by the class member 'VALUE'.  'bsl::is_polymorphic' should be
// preferred over 'bslmf::IsPolymorphic', and in general, should be used by new
// components.
//
///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1: Verify Polymorphic Types
///- - - - - - - - - - - - - - - - - -
// Suppose that we want to assert whether a particular type is a polymorphic
// type.
//
// First, we define two types in a non-polymorphic hierarchy, 'MyStruct' and
// 'MyDerivedStruct':
//..
//  struct MyStruct {
//      void nonvirtualMethod();
//  };
//  struct MyDerivedStruct : public MyStruct {
//  };
//..
// Then, we define two types in a polymorphic hierarchy, 'MyClass' and
// 'MyDerivedClass':
//..
//  class MyClass {
//      MyClass();
//      virtual ~MyClass();  // makes 'MyClass' polymorphic
//  };
//
//  class MyDerivedClass : public MyClass {
//      MyDerivedClass();
//      ~MyDerivedClass();
//  };
//..
// Now, assert that the two types in the non-polymorphic hierarchy are not
// polymorphic, and the two types in the polymorphic hierarchy are polymorphic
// using 'bsl::is_polymorphic':
//..
//  assert(false == bsl::is_polymorphic<MyStruct          >::value);
//  assert(false == bsl::is_polymorphic<MyStruct         *>::value);
//  assert(false == bsl::is_polymorphic<MyDerivedStruct&  >::value);
//  assert(false == bsl::is_polymorphic<MyDerivedStruct  *>::value);
//
//  assert(true  == bsl::is_polymorphic<      MyClass    >::value);
//  assert(true  == bsl::is_polymorphic<const MyClass&   >::value);
//  assert(false == bsl::is_polymorphic<      MyClass   *>::value);
//  assert(true  == bsl::is_polymorphic<MyDerivedClass&  >::value);
//  assert(false == bsl::is_polymorphic<MyDerivedClass  *>::value);
//..
// Finally, note that the following class is detected as polymorphic by this
// component, but should really have a virtual destructor ('gcc' issues a
// warning for such infractions):
//..
//  class MyIncorrectPolymorphicClass {
//
//      MyIncorrectPolymorphicClass();
//      ~MyIncorrectPolymorphicClass();
//      virtual void virtualMethod();
//  };
//
//  assert(1 == bslmf::IsPolymorphic<MyIncorrectPolymorphicClass&  >::value);
//  assert(0 == bslmf::IsPolymorphic<MyIncorrectPolymorphicClass  *>::value);
//..

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLMF_ISCLASS
#include <bslmf_isclass.h>
#endif

#ifndef INCLUDED_BSLMF_INTEGRALCONSTANT
#include <bslmf_integralconstant.h>
#endif

#ifndef INCLUDED_BSLMF_REMOVECV
#include <bslmf_removecv.h>
#endif

#ifndef INCLUDED_BSLMF_REMOVEREFERENCE
#include <bslmf_removereference.h>
#endif

#ifndef INCLUDED_BSLS_EXCEPTIONUTIL
#include <bsls_exceptionutil.h>
#endif

#if (defined(BSLS_PLATFORM_CMP_GNU) && BSLS_PLATFORM_CMP_VER_MAJOR >= 40300)\
 || (defined(BSLS_PLATFORM_CMP_MSVC) && BSLS_PLATFORM_CMP_VER_MAJOR >= 1500)
#define BSLMF_ISPOLYMORPHIC_HAS_INTRINSIC
#endif

namespace BloombergLP {

namespace bslmf {

                       // ========================
                       // struct IsPolymorphic_Imp
                       // ========================

#if defined(BSLMF_ISPOLYMORPHIC_HAS_INTRINSIC)

// Use type traits intrinsics, where available, to give the correct answer for
// the tricky cases where existing ABIs prevent programatic detection.
template <typename TYPE>
struct IsPolymorphic_Imp {
    enum { Value = __is_polymorphic(TYPE) };
};

#else

template <typename TYPE, bool IS_CLASS = bsl::is_class<TYPE>::value>
struct IsPolymorphic_Imp {
    // This 'struct' template provides a meta-function to determine whether the
    // (template parameter) 'TYPE' is a (non-cv-qualified) polymorphic type.
    // This generic default template defines a static data member, 'Value',
    // that is set to 'false'.  A template specialization is provided (below)
    // that has 'Value' set to 'true'.

    enum { Value = false };
};

template <typename TYPE>
struct IsPolymorphic_Imp<TYPE, true> {
     // This partial specialization of 'IsPolymorphic_Imp',for when the
     // (template parameter) 'TYPE' is a (non-cv-qualified) polymorphic type,
     // provides a static data member, 'Value', that is set to 'true'.

    struct IsPoly : public TYPE {
        IsPoly();
        virtual ~IsPoly() BSLS_NOTHROW_SPEC;

        char dummy[256];
    };

    struct MaybePoly : public TYPE {
        MaybePoly();
        ~MaybePoly() BSLS_NOTHROW_SPEC;
        char dummy[256];
    };

    enum { Value = (sizeof(IsPoly) == sizeof(MaybePoly)) };
};

#endif

}  // close package namespace
}  // close enterprise namespace

namespace bsl {

template <typename TYPE>
struct is_polymorphic
    : integral_constant<bool,
                        BloombergLP::bslmf::IsPolymorphic_Imp<
                             typename remove_cv<TYPE>::type>
                        ::Value> {
    // This 'struct' template implements the 'is_polymorphic' meta-function
    // defined in the C++11 standard [meta.unary.prop] to determine if the
    // (template parameter) 'TYPE' is a (possiblly cv-qualified) polymorphic
    // type.  This 'struct' derives from 'bsl::true_type' if the 'TYPE' is a
    // polymorphic type, and 'bsl::false_type' otherwise.
};

}  // close namespace bsl

namespace BloombergLP {

namespace bslmf {

                         // ====================
                         // struct IsPolymorphic
                         // ====================

template <typename TYPE>
struct IsPolymorphic : bsl::is_polymorphic<TYPE>::type {
    // This 'struct' template implements a meta-function to determine if the
    // (template parameter) 'TYPE' is a (possiblly cv-qualified) polymorphic
    // type.  This 'struct' derives from 'bsl::true_type' if the 'TYPE' is a
    // polymorphic type, and 'bsl::false_type' otherwise.
    //
    // Note that although this 'struct' is functionally equivalent to
    // 'bsl::is_polymorphic', the use of 'bsl::is_polymorphic' should be
    // preferred.
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
