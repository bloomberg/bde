// bsls_compilerfeatures.h                                            -*-C++-*-
#ifndef INCLUDED_BSLS_COMPILERFEATURES
#define INCLUDED_BSLS_COMPILERFEATURES

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide macros to identify compiler support for C++0x features.
//
//@CLASSES:
//
//@SEE_ALSO: bsls_platform
//
//@AUTHOR: Alisdair Meredith (ameredith1)
//
//@DESCRIPTION: This component provides a suite of preprocessor macros to
// identify compiler-specific support of language features that may not be
// available on all compilers in use across an organization.  For example, as
// new C++0x language facilities become more broadly available, such as
// 'extern template', macros will be defined here to indicate whether the
// current platform/compiler combination supports the corresponding language
// features.  Note that in some cases, support for a given feature may need to
// be explicitly enabled by using an appropriate compiler command-line option.
//
///Macro Summary
///-------------
// The following are the macros provided by this component.  Note that they are
// not defined for all platform/compiler combinations.
//..
//  BSLS_COMPILERFEATURES_SUPPORT_EXTERN_TEMPLATE
//      This macro is defined if 'extern template' is supported by the current
//      compiler settings for this platform.
//
//  BSLS_COMPILERFEATURES_SUPPORT_STATIC_ASSERT
//      This macro is defined if 'static_assert' is supported by the current
//      compiler settings for this platform.
//..
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
#if defined(BSLS_PLATFORM__CMP_GNU) && BSLS_PLATFORM__CMP_VER_MAJOR >= 40700\
   && defined(__GXX_EXPERIMENTAL_CXX0X__)
#   define BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES
#endif

             // BSLS_COMPILERFEATURES_SUPPORT_EXTERN_TEMPLATE

#if defined(BSLS_PLATFORM__CMP_IBM)
#   define BSLS_COMPILERFEATURES_SUPPORT_EXTERN_TEMPLATE
#endif

             // BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES

// Microsoft VC2010 always supports the feature (it cannot be disabled).
#if defined(BSLS_PLATFORM__CMP_MSVC) && BSLS_PLATFORM__CMP_VER_MAJOR >= 1600
#   define BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES

// GCC 4.5 has support with '-std=c++0x' (earlier versions are broken)
#elif defined(BSLS_PLATFORM__CMP_GNU) && BSLS_PLATFORM__CMP_VER_MAJOR >= 40500\
   && defined(__GXX_EXPERIMENTAL_CXX0X__)
#   define BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
#endif

             // BSLS_COMPILERFEATURES_SUPPORT_STATIC_ASSERT

// Microsoft VC2010 always supports the feature (it cannot be disabled).
#if defined(BSLS_PLATFORM__CMP_MSVC) && BSLS_PLATFORM__CMP_VER_MAJOR >= 1600
#   define BSLS_COMPILERFEATURES_SUPPORT_STATIC_ASSERT

// IBM XL C++ 11 has support in '0x' mode, or if explicitly activated.  Note
// that '__IBMCPP_STATIC_ASSERT' was not available until xlC 11, so no version
// check is needed.
#elif defined(BSLS_PLATFORM__CMP_IBM) && defined(__IBMCPP_STATIC_ASSERT)
#   define BSLS_COMPILERFEATURES_SUPPORT_STATIC_ASSERT

// HP-UX aCC 6.25 has support with '-Ax'.
#elif defined(BSLS_PLATFORM__CMP_HP) && BSLS_PLATFORM__CMP_VER_MAJOR >= 62500 \
   && defined(_HP_CXX0x_SOURCE)
#   define BSLS_COMPILERFEATURES_SUPPORT_STATIC_ASSERT

// GCC 4.3 has support with '-std=c++0x'.
#elif defined(BSLS_PLATFORM__CMP_GNU) && BSLS_PLATFORM__CMP_VER_MAJOR >= 40300\
   && defined(__GXX_EXPERIMENTAL_CXX0X__)
#   define BSLS_COMPILERFEATURES_SUPPORT_STATIC_ASSERT
#endif

             // BSLS_COMPILERFEATURES_SUPPORT_VARIADIC_TEMPLATES

// IBM XL C++ 11 has support in '0x' mode, or if explicitly activated.  Note
// that '__IBMCPP_VARIADIC_TEMPLATES' was not available until xlC 11, so no
// version check is needed.
#if defined(BSLS_PLATFORM__CMP_IBM) && defined(__IBMCPP_VARIADIC_TEMPLATES)
#   define BSLS_COMPILERFEATURES_SUPPORT_VARIADIC_TEMPLATES

// GCC 4.3 has support with '-std=c++0x'.
#elif defined(BSLS_PLATFORM__CMP_GNU) && BSLS_PLATFORM__CMP_VER_MAJOR >= 40300\
   && defined(__GXX_EXPERIMENTAL_CXX0X__)
#   define BSLS_COMPILERFEATURES_SUPPORT_VARIADIC_TEMPLATES
#endif

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2010
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
