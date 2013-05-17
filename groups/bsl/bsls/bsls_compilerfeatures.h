// bsls_compilerfeatures.h                                            -*-C++-*-
#ifndef INCLUDED_BSLS_COMPILERFEATURES
#define INCLUDED_BSLS_COMPILERFEATURES

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide macros to identify compiler support for C++11 features.
//
//@CLASSES:
//
//@MACROS
//  BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES: flag for alias templates
//  BSLS_COMPILERFEATURES_SUPPORT_DECLTYPE: flag for 'decltype'
//  BSLS_COMPILERFEATURES_SUPPORT_EXTERN_TEMPLATE: flag for 'extern template'
//  BSLS_COMPILERFEATURES_SUPPORT_INCLUDE_NEXT: flag for 'include_next'
//  BSLS_COMPILERFEATURES_SUPPORT_NULLPTR: flag for 'nullptr'
//  BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES: flag for rvalue references
//  BSLS_COMPILERFEATURES_SUPPORT_STATIC_ASSERT: flag for 'static_assert'
//  BSLS_COMPILERFEATURES_SUPPORT_VARIADIC_TEMPLATES: flag for variadic params.
//
//@SEE_ALSO: bsls_platform
//
//@DESCRIPTION: This component provides a suite of preprocessor macros to
// identify compiler-specific support of language features that may not be
// available on all compilers in use across an organization.  For example, as
// new C++11 language facilities become more broadly available, such as
// 'extern template', macros will be defined here to indicate whether the
// current platform/compiler combination supports the corresponding language
// features.  Note that in some cases, support for a given feature may need to
// be explicitly enabled by using an appropriate compiler command-line option.
//
///Macro Summary
///-------------
// The following are the macros provided by this component.  Note that they are
// not defined for all platform/compiler combinations.
//
//: 'BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES'
//:    This macro is defined if alias templates are supported by the current
//:    compiler settings for this platform.
//:
//: 'BSLS_COMPILERFEATURES_SUPPORT_DECLTYPE'
//:    This macro is defined if 'decltype' is supported by the current compiler
//:    settings for this platform.
//:
//: 'BSLS_COMPILERFEATURES_SUPPORT_EXTERN_TEMPLATE'
//:     This macro is defined if 'extern template' is supported by the current
//:     compiler settings for this platform.
//:
//: 'BSLS_COMPILERFEATURES_SUPPORT_INCLUDE_NEXT'
//:    This macro is defined if 'include_next' is supported by the current
//:    compiler settings for this platform.
//:
//: 'BSLS_COMPILERFEATURES_SUPPORT_NULLPTR'
//:    This macro is defined if 'nullptr' is supported by the current compiler
//:    settings for this platform.
//:
//: 'BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES'
//:    This macro is defined if rvalue references are supported by the current
//:    compiler settings for this platform.
//:
//: 'BSLS_COMPILERFEATURES_SUPPORT_STATIC_ASSERT'
//:    This macro is defined if 'static_assert' is supported by the current
//:    compiler settings for this platform.
//:
//: 'BSLS_COMPILERFEATURES_SUPPORT_VARIADIC_TEMPLATES'
//:    This macro is defined if variadic template parameters are supported by
//:    the current compiler settings for this platform.
//
///Usage
///-----
// The following code snippets illustrate use of this component.
//
// Suppose that we wish to "preinstantiate" 'bsl::basic_string' for a given
// character type, say, 'char', on platforms that support 'extern template'.
// To accomplish this, we would do the following in the '.h' and '.cpp' files
// of the 'bslstl_string' component:
//..
//  // bslstl_string.h
//  // ...
//  #ifdef BSLS_COMPILERFEATURES_SUPPORT_EXTERN_TEMPLATE
//  extern template class bsl::String_Imp<char, bsl::string::size_type>;
//  extern template class bsl::basic_string<char>;
//  #endif
//  // ...
//
//  // bslstl_string.cpp
//  // ...
//  #ifdef BSLS_COMPILERFEATURES_SUPPORT_EXTERN_TEMPLATE
//  template class bsl::String_Imp<char, bsl::string::size_type>;
//  template class bsl::basic_string<char>;
//  #endif
//  // ...
//..

#ifndef INCLUDED_BSLS_PLATFORM
#include <bsls_platform.h>
#endif

             // BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES

// GCC 4.7 has support with '-std=c++0x'.
#if defined(BSLS_PLATFORM_CMP_GNU) && BSLS_PLATFORM_CMP_VER_MAJOR >= 40700\
 && defined(__GXX_EXPERIMENTAL_CXX0X__)
#define BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES
#endif

             // BSLS_COMPILERFEATURES_SUPPORT_DECLTYPE

// Microsoft VC2010 always supports the feature (it cannot be disabled).
#if defined(BSLS_PLATFORM_CMP_MSVC) && BSLS_PLATFORM_CMP_VER_MAJOR >= 1600
#define BSLS_COMPILERFEATURES_SUPPORT_DECLTYPE

// GCC 4.3 has support with '-std=c++0x'.
#elif defined(BSLS_PLATFORM_CMP_GNU) && BSLS_PLATFORM_CMP_VER_MAJOR >= 40300\
   && defined(__GXX_EXPERIMENTAL_CXX0X__)
#define BSLS_COMPILERFEATURES_SUPPORT_DECLTYPE
#endif

             // BSLS_COMPILERFEATURES_SUPPORT_EXTERN_TEMPLATE

// IBM xlC compiler has support
#if defined(BSLS_PLATFORM_CMP_IBM)
#define BSLS_COMPILERFEATURES_SUPPORT_EXTERN_TEMPLATE

// GCC 4.7 has support with '-std=c++0x'.
#elif defined(BSLS_PLATFORM_CMP_GNU) && BSLS_PLATFORM_CMP_VER_MAJOR >= 40700\
   && defined(__GXX_EXPERIMENTAL_CXX0X__)
#define BSLS_COMPILERFEATURES_SUPPORT_EXTERN_TEMPLATE
#endif

             // BSLS_COMPILERFEATURES_SUPPORT_INCLUDE_NEXT

// GCC and CLANG compilers have support
#if defined(BSLS_PLATFORM_CMP_GNU) || defined(BSLS_PLATFORM_CMP_CLANG)
#define BSLS_COMPILERFEATURES_SUPPORT_INCLUDE_NEXT
#endif

             // BSLS_COMPILERFEATURES_SUPPORT_NULLPTR

// Microsoft VC2010 always supports the feature (it cannot be disabled).
#if defined(BSLS_PLATFORM_CMP_MSVC) && BSLS_PLATFORM_CMP_VER_MAJOR >= 1600
#define BSLS_COMPILERFEATURES_SUPPORT_NULLPTR

// GCC 4.6 has support with '-std=c++0x'.
#elif defined(BSLS_PLATFORM_CMP_GNU) && BSLS_PLATFORM_CMP_VER_MAJOR >= 40600\
   && defined(__GXX_EXPERIMENTAL_CXX0X__)
#define BSLS_COMPILERFEATURES_SUPPORT_NULLPTR
#endif

             // BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES

// Microsoft VC2010 always supports the feature (it cannot be disabled).
#if defined(BSLS_PLATFORM_CMP_MSVC) && BSLS_PLATFORM_CMP_VER_MAJOR >= 1600
#   define BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES

// GCC 4.5 has support with '-std=c++0x' (earlier versions are broken)
#elif defined(BSLS_PLATFORM_CMP_GNU) && BSLS_PLATFORM_CMP_VER_MAJOR >= 40500\
   && defined(__GXX_EXPERIMENTAL_CXX0X__)
#define BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
#endif

             // BSLS_COMPILERFEATURES_SUPPORT_STATIC_ASSERT

// Microsoft VC2010 always supports the feature (it cannot be disabled).
#if defined(BSLS_PLATFORM_CMP_MSVC) && BSLS_PLATFORM_CMP_VER_MAJOR >= 1600
#define BSLS_COMPILERFEATURES_SUPPORT_STATIC_ASSERT

// IBM XL C++ 11 has support in '0x' mode, or if explicitly activated.  Note
// that '__IBMCPP_STATIC_ASSERT' was not available until xlC 11, so no version
// check is needed.
#elif defined(BSLS_PLATFORM_CMP_IBM) && defined(__IBMCPP_STATIC_ASSERT)
#define BSLS_COMPILERFEATURES_SUPPORT_STATIC_ASSERT

// HP-UX aCC 6.25 has support with '-Ax'.
#elif defined(BSLS_PLATFORM_CMP_HP) && BSLS_PLATFORM_CMP_VER_MAJOR >= 62500 \
   && defined(_HP_CXX0x_SOURCE)
#define BSLS_COMPILERFEATURES_SUPPORT_STATIC_ASSERT

// GCC 4.3 has support with '-std=c++0x'.
#elif defined(BSLS_PLATFORM_CMP_GNU) && BSLS_PLATFORM_CMP_VER_MAJOR >= 40300\
   && defined(__GXX_EXPERIMENTAL_CXX0X__)
#define BSLS_COMPILERFEATURES_SUPPORT_STATIC_ASSERT
#endif

             // BSLS_COMPILERFEATURES_SUPPORT_VARIADIC_TEMPLATES

// IBM XL C++ 11 has support in '0x' mode, or if explicitly activated.  Note
// that '__IBMCPP_VARIADIC_TEMPLATES' was not available until xlC 11, so no
// version check is needed.
#if defined(BSLS_PLATFORM_CMP_IBM) && defined(__IBMCPP_VARIADIC_TEMPLATES)
#define BSLS_COMPILERFEATURES_SUPPORT_VARIADIC_TEMPLATES

// GCC 4.3 has support with '-std=c++0x'.
#elif defined(BSLS_PLATFORM_CMP_GNU) && BSLS_PLATFORM_CMP_VER_MAJOR >= 40300\
   && defined(__GXX_EXPERIMENTAL_CXX0X__)
#define BSLS_COMPILERFEATURES_SUPPORT_VARIADIC_TEMPLATES
#endif

    //  *** Simulate various C++11 features ***

#ifndef BSLS_COMPILERFEATURES_SUPPORT_VARIADIC_TEMPLATES
#   define BSLS_COMPILERFEATURES_SIMULATE_VARIADIC_TEMPLATES 1

#   ifndef BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
#       define BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES 1
#   endif
#endif

#ifndef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
#   define BSLS_COMPILERFEATURES_SIMULATE_FORWARD_WORKAROUND 1

#   ifndef BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
#       define BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES 1
#   endif
#endif

#ifdef BSLS_COMPILERFEATURES_SIMULATE_FORWARD_WORKAROUND
// Use a work-around for the absence of perfect-forwarding.

#   define BSLS_COMPILERFEATURES_FORWARD_REF(T) const T&
    // On compilers that support C++11 perfect forwarding, replace with 'T&&',
    // i.e., an argument that can be perfect-forwarded; otherwise, replace
    // with 'const T&', the classic way to accept arguments of unknown
    // rvalue/lvalue-ness.

#   define BSLS_COMPILERFEATURES_FORWARD(T,V) (V)
    // On compilers that support C++11 perfect forwarding, replace with
    // 'bsl::forward<T>(V)', i.e., use perfect-forwarding; otherwise, replace
    // with '(V)', the classic way to forward arguments safely.

#else
// Use real perfect-forwarding; no workaround needed.
#   define BSLS_COMPILERFEATURES_FORWARD_REF(T) T&&
#   define BSLS_COMPILERFEATURES_FORWARD(T,V)   native_std::forward<T>(V)
#endif

#endif

// ----------------------------------------------------------------------------
// Copyright (C) 2013 Bloomberg L.P.
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
