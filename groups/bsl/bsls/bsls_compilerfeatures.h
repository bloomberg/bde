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

#if defined(BSLS_PLATFORM__CMP_IBM)
#define BSLS_COMPILERFEATURES_SUPPORT_EXTERN_TEMPLATE
#endif

// HP (aCC) also supports 'static_assert' in 0x mode, but that compiler switch
// must be enabled before defining the following for 'BSLS_PLATFORM__CMP_HP'.
#if defined(BSLS_PLATFORM__CMP_MSVC) && BSLS_PLATFORM__CMP_VER_MAJOR >= 1600
#define BSLS_COMPILERFEATURES_SUPPORT_STATIC_ASSERT
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
