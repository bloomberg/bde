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
//  BSLS_COMPILERFEATURES_SUPPORT_VARIADIC_TEMPLATES: flag for variadic params
//  BSLS_COMPILERFEATURES_SUPPORT_ALIGNAS: flag for 'alignas'.
//  BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT: flag for 'noexcept' keyword
//  BSLS_COMPILERFEATURES_SUPPORT_TRAITS_HEADER: Has <type_traits> header
//  BSLS_COMPILERFEATURES_FORWARD_REF(T): argument of type 'T' to be forwarded
//  BSLS_COMPILERFEATURES_FORWARD(T,V): Forward argument 'V' of type 'T'
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
//: 'BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES':
//:     This macro is defined if alias templates are supported by the current
//:     compiler settings for this platform.
//:
//: 'BSLS_COMPILERFEATURES_SUPPORT_ATTRIBUTE_NORETURN':
//:     This macro is defined if the '[[noreturn]]' attribute is supported by
//:     the current compiler settings for this platform.
//:
//: 'BSLS_COMPILERFEATURES_SUPPORT_CONSTEXPR':
//:     This macro is defined if 'constexpr' is supported by the current
//:     compiler settings for this platform.
//:
//: 'BSLS_COMPILERFEATURES_SUPPORT_DECLTYPE':
//:     This macro is defined if 'decltype' is supported by the current
//:     compiler settings for this platform.
//:
//: 'BSLS_COMPILERFEATURES_SUPPORT_DEFAULTED_FUNCTIONS':
//:     This macro is defined if defaulted functions are supported by the
//:     current compiler settings for this platform.
//
//: 'BSLS_COMPILERFEATURES_SUPPORT_DELETED_FUNCTIONS':
//:     This macro is defined if deleted functions are supported by the
//:     current compiler settings for this platform.
//:
//: 'BSLS_COMPILERFEATURES_SUPPORT_EXTERN_TEMPLATE':
//:     This macro is defined if 'extern template' is supported by the current
//:     compiler settings for this platform.
//:
//: 'BSLS_COMPILERFEATURES_SUPPORT_FINAL':
//:     This macro is defined if 'final' is supported for classes and member
//:     functions by the current compiler settings for this platform.
//:
//: 'BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS':
//:     This macro is defined if generalized initializers are supported by the
//:     current compiler settings for this platform.
//:
//: 'BSLS_COMPILERFEATURES_SUPPORT_INCLUDE_NEXT':
//:     This macro is defined if 'include_next' is supported by the current
//:     compiler settings for this platform.
//:
//: 'BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT':
//:     This macro is defined if 'noexcept' is supported by the current
//:     compiler settings for this platform.
//:
//: 'BSLS_COMPILERFEATURES_SUPPORT_NULLPTR':
//:    This macro is defined if 'nullptr' is supported by the current compiler
//:    settings for this platform.
//:
//: 'BSLS_COMPILERFEATURES_SUPPORT_OPERATOR_EXPLICIT'
//:     This macro is defined if the 'explicit' keyword applied to conversion
//:     operators is supported by the current compiler settings for this
//:     platform.
//:
//: 'BSLS_COMPILERFEATURES_SUPPORT_OVERRIDE'
//:     This macro is defined if the 'override' keyword is supported by the
//:     current compiler settings for this platform.
//:
//: 'BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES'
//:     This macro is defined if rvalue references are supported by the current
//:     compiler settings for this platform.
//:
//: 'BSLS_COMPILERFEATURES_SUPPORT_STATIC_ASSERT'
//:     This macro is defined if 'static_assert' is supported by the current
//:     compiler settings for this platform.
//:
//: 'BSLS_COMPILERFEATURES_SUPPORT_VARIADIC_TEMPLATES'
//:     This macro is defined if variadic template parameters are supported by
//:     the current compiler settings for this platform.
//:
//: 'BSLS_COMPILERFEATURES_SUPPORT_ALIGNAS'
//:     This macro is defined if 'alignas' alignment specifier is supported by
//:     the current compiler settings for this platform.
//:
//: 'BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT'
//:     This macro is defined if the 'noexcept' keyword is supported by the
//:     current compiler settings for this platform, both for designating a
//:     function as not throwing and for testing if an expression may throw.
//:
//: 'BSLS_COMPILERFEATURES_SUPPORT_TRAITS_HEADER'
//:     This macro is defined if the <type_traits> header is provided by the
//:     current compiler settings for this platform and supports traits like
//:     'is_trivially_copyable' and 'is_nothrow_destructible'
//
///Usage
///-----
// The following code snippets illustrate use of this component.
//
///Example 1: Using 'BSLS_COMPILERFEATURES_SUPPORT_EXTERN_TEMPLATE'
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
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
//
///Feature Support in Compilers
///----------------------------
//
///'BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES'
///- - - - - - - - - - - - - - - - - - - - - - - -
// This macro is defined if the compiler supports syntax to introduce a
// 'typedef'-name using alias-declaration syntax, declaring a name for a family
// of types.
//
//: o Compiler support:
//:   o gcc 4.7
//:   o clang 3.0
//:   o MSVC 2013
//:   o Oracle CC 12.4
//
///'BSLS_COMPILERFEATURES_SUPPORT_ATTRIBUTE_NORETURN'
/// - - - - - - - - - - - - - - - - - - - - - - - - -
// This macro is defined if the compiler supports '[[noreturn]]' C++11
// attribute syntax.  MSVC supports the attribute with alternative syntax
// __declspec(noreturn), and earlier versions of gcc and clang support the
// alternative syntax '__attribute__((noreturn))'.
//
//: o Compiler support:
//:   o gcc 4.8
//:   o clang 3.3
//:   o xlC 12.1
//:   o Oracle CC 12.4
//
///'BSLS_COMPILERFEATURES_SUPPORT_CONSTEXPR'
///- - - - - - - - - - - - - - - - - - - - -
// This macro is defined in the compilier supports the 'constexpr' reserved
// keyword.
//
//: o Compiler support:
//:   o gcc 4.7
//:   o clang 3.1
//:   o xlC 13
//:   o Oracle CC 12.4
//
///'BSLS_COMPILERFEATURES_SUPPORT_DECLTYPE'
/// - - - - - - - - - - - - - - - - - - - -
// This macro is defined if the compiler supports the 'decltype' reserved word.
//
//: o Compiler support:
//:   o gcc 4.3
//:   o clang 3.3
//:   o MSVC 2010
//:   o xlC 11.1
//:   o Oracle CC 12.4
//
///'BSLS_COMPILERFEATURES_SUPPORT_DEFAULTED_FUNCTIONS'
///- - - - - - - - - - - - - - - - - - - - - - - - - -
// This macro is defined if the compiler supports syntax to introduce defaulted
// functions.
//
//: o Compiler support:
//:   o gcc 4.4
//:   o clang 2.9
//:   o MSVC 2013
//:   o xlC 13.1
//:   o Oracle CC 12.4
//
///'BSLS_COMPILERFEATURES_SUPPORT_DELETED_FUNCTIONS'
///- - - - - - - - - - - - - - - - - - - - - - - - -
// This macro is defined if the compiler supports syntax to introduce deleted
// functions.
//
//: o Compiler support:
//:   o gcc 4.4
//:   o clang 2.9
//:   o MSVC 2013
//:   o xlC 13.1
//:   o Oracle CC 12.4
//
///'BSLS_COMPILERFEATURES_SUPPORT_EXTERN_TEMPLATE'
///- - - - - - - - - - - - - - - - - - - - - - - -
// This macro is defined if the compiler supports allowing supression of
// implicit instantiation of templates by prefixing an explicit instantiation
// directive with the 'extern' keyword.
//
//: o Compiler support:
//:   o gcc 3.3
//:   o clang (any)
//:   o MSVC 2010
//:   o xlC 11.1
//:   o Oracle CC 12.4
//
///'BSLS_COMPILERFEATURES_SUPPORT_FINAL'
///- - - - - - - - - - - - - - - - - - -
// This macro is defined if the compiler supports both finalizing a class using
// the 'final' keyword after the class name as well as preventing further
// derived classes from overriding a 'virtual' function by using 'final' after
// its signature.
//
//: o Compiler support:
//:   o gcc 4.7
//:   o clang 3.0
//:   o MSVC 2012
//:   o xlC 11.1
//:   o Oracle CC 12.4
//
///'BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS'
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// This macro is defined if the compiler supports generalized initializers.
//
//: o Compiler support:
//:   o gcc 4.4
//:   o clang 3.1
//:   o MSVC 2013
//:   o xlC not supported
//:   o Oracle CC 13
//
///'BSLS_COMPILERFEATURES_SUPPORT_INCLUDE_NEXT'
/// - - - - - - - - - - - - - - - - - - - - - -
// This macro is defined if the compiler supports #include_next semantics
// expected by the deprecated and internal include code wrapped by
// BSL_OVERRIDES_STD macro.
//
//: o Compiler support:
//:   o gcc (any)
//:   o clang (any)
//:   o xlC 8
//:   o Oracle CC 12.4
//
///'BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT'
/// - - - - - - - - - - - - - - - - - - - -
// This macro is defined if the compiler supports the 'noexcept' reserved
// keyword.
//
//: o Compiler support:
//:   o gcc 4.6
//:   o clang 3.0
//
///'BSLS_COMPILERFEATURES_SUPPORT_NULLPTR'
///- - - - - - - - - - - - - - - - - - - -
// This macro is defined if the compiler supports the 'nullptr' reserved word.
//
//: o Compiler support:
//:   o gcc 4.6
//:   o clang 3.0
//:   o MSVC 2010
//:   o xlC 13.1
//:   o Oracle CC 12.4
//
///'BSLS_COMPILERFEATURES_SUPPORT_OPERATOR_EXPLICIT'
///- - - - - - - - - - - - - - - - - - - - - - - - -
// This macro is defined if the compiler supports use of the 'explicit' keyword
// to indicate that a conversion operator only available for explicit
// conversions.
//
//: o Compiler support:
//:   o gcc 4.5
//:   o clang 3.0
//:   o MSVC 2013
//:   o xlC 11.1
//:   o Oracle CC 12.4
//
///'BSLS_COMPILERFEATURES_SUPPORT_OVERRIDE'
/// - - - - - - - - - - - - - - - - - - - -
// This macro is defined if the compiler supports use of the 'override' keyword
// to indicate that a member function is meant to override a 'virtual'
// function (and cause a failure if that is not the case).
//
//: o Compiler support:
//:   o gcc 4.7
//:   o clang 3.0
//:   o MSVC 2012
//:   o xlC 11.1
//:   o Oracle CC 12.4
//
///'BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES'
///- - - - - - - - - - - - - - - - - - - - - - - - -
// This macro is defined if the compiler supports rvalue references and move
// semantics compatible with the final C++11 specification.  (Semantics have
// changed since early draft proposals.)
//
//: o Compiler support:
//:   o gcc 4.5 (rvalue references v2.1; original draft support in gcc 4.3 is
//:     not correct with respect to final spec (v3.0))
//:   o clang 2.9
//:   o MSVC 2010
//:   o xlC 12.1
//:   o Oracle CC 12.4
//
///'BSLS_COMPILERFEATURES_SUPPORT_STATIC_ASSERT'
///- - - - - - - - - - - - - - - - - - - - - - -
// This macro is defined if the compiler supports the 'static_assert' reserved
// word.
//
//: o Compiler support:
//:   o gcc 4.3
//:   o clang 2.9
//:   o MSVC 2010
//:   o xlC 11.1
//:   o Oracle CC 12.4
//
///'BSLS_COMPILERFEATURES_SUPPORT_VARIADIC_TEMPLATES'
/// - - - - - - - - - - - - - - - - - - - - - - - - -
// This macro is defined if the compiler supports the ability to express a
// template that accepts an arbitrary number of parameters in a type-safe
// manner.
//
//: o Compiler support:
//:   o gcc 4.3
//:   o clang 2.9
//:   o MSVC 2015
//:   o xlC 11.1
//:   o Oracle CC 12.4
//
// Note that bugs in MSVC 2013 support for variadic templates preclude
// enabling the feature for BSL.
//
///'BSLS_COMPILERFEATURES_SUPPORT_ALIGNAS'
///- - - - - - - - - - - - - - - - - - - -
// This macro is defined if the compiler supports the 'alignas' alignment
// specifier.
//
//: o Compiler support:
//:   o gcc 4.8
//:   o clang 3.0
//
// This feature is not yet supported in Visual Studio, xlc, CC.

#ifndef INCLUDED_BSLS_PLATFORM
#include <bsls_platform.h>
#endif

#ifndef INCLUDED_BSLS_MACROREPEAT
#include <bsls_macrorepeat.h>
#endif

// gcc
// https://wiki.apache.org/stdcxx/C%2B%2B0xCompilerSupport
#if defined(BSLS_PLATFORM_CMP_GNU)
#define BSLS_COMPILERFEATURES_SUPPORT_INCLUDE_NEXT
#endif
#if BSLS_PLATFORM_CMP_VERSION >= 30300
#define BSLS_COMPILERFEATURES_SUPPORT_EXTERN_TEMPLATE
#endif
// gcc -std=c++11 or -std=c++0x or -std=gnu++11 or -std=gnu++0x
#if defined(BSLS_PLATFORM_CMP_GNU) && defined(__GXX_EXPERIMENTAL_CXX0X__)
#if BSLS_PLATFORM_CMP_VERSION >= 40300
#define BSLS_COMPILERFEATURES_SUPPORT_DECLTYPE
#define BSLS_COMPILERFEATURES_SUPPORT_STATIC_ASSERT
#define BSLS_COMPILERFEATURES_SUPPORT_VARIADIC_TEMPLATES
#endif
#if BSLS_PLATFORM_CMP_VERSION >= 40400
#define BSLS_COMPILERFEATURES_SUPPORT_DEFAULTED_FUNCTIONS
#define BSLS_COMPILERFEATURES_SUPPORT_DELETED_FUNCTIONS
#define BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS
#endif
#if BSLS_PLATFORM_CMP_VERSION >= 40500
#define BSLS_COMPILERFEATURES_SUPPORT_OPERATOR_EXPLICIT
#define BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
// r-value references v1.0 in gcc 4.3, v2.1 in gcc 4.5, v3.0 in gcc 4.6
#endif
#if BSLS_PLATFORM_CMP_VERSION >= 40600
#define BSLS_COMPILERFEATURES_SUPPORT_NULLPTR
#define BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT
#endif
#if BSLS_PLATFORM_CMP_VERSION >= 40700
#define BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES
#define BSLS_COMPILERFEATURES_SUPPORT_CONSTEXPR
#define BSLS_COMPILERFEATURES_SUPPORT_FINAL
#define BSLS_COMPILERFEATURES_SUPPORT_OVERRIDE
#endif
#if BSLS_PLATFORM_CMP_VERSION >= 40800
#define BSLS_COMPILERFEATURES_SUPPORT_ATTRIBUTE_NORETURN
// gcc supports __attribute__((noreturn)) in earlier versions
#define BSLS_COMPILERFEATURES_SUPPORT_ALIGNAS
#endif
#if BSLS_PLATFORM_CMP_VERSION >= 50000
#define BSLS_COMPILERFEATURES_SUPPORT_TRAITS_HEADER
#endif
#endif

// clang
// http://clang.llvm.org/cxx_status.html
// http://clang.llvm.org/docs/LanguageExtensions.html
#if defined(BSLS_PLATFORM_CMP_CLANG)
// clang supported
#define BSLS_COMPILERFEATURES_SUPPORT_INCLUDE_NEXT
#define BSLS_COMPILERFEATURES_SUPPORT_EXTERN_TEMPLATE
// clang -std=c++11 or -std=c++0x or -std=gnu++11 or -std=gnu++0x
// clang 2.9
#if __has_feature(cxx_rvalue_references)
#define BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
#endif
#if __has_feature(cxx_defaulted_functions)
#define BSLS_COMPILERFEATURES_SUPPORT_DEFAULTED_FUNCTIONS
#endif
#if __has_feature(cxx_deleted_functions)
#define BSLS_COMPILERFEATURES_SUPPORT_DELETED_FUNCTIONS
#endif
#if __has_feature(cxx_static_assert)
#define BSLS_COMPILERFEATURES_SUPPORT_STATIC_ASSERT
#endif
#if __has_feature(cxx_variadic_templates)
#define BSLS_COMPILERFEATURES_SUPPORT_VARIADIC_TEMPLATES
#endif
// clang 3.0
#if __has_feature(cxx_alias_templates)
#define BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES
#endif
#if __has_feature(cxx_noexcept)
#define BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT
#endif
// clang 3.1
#if __has_feature(cxx_constexpr)
#define BSLS_COMPILERFEATURES_SUPPORT_CONSTEXPR
#endif
#if __has_feature(cxx_explicit_conversions)
#define BSLS_COMPILERFEATURES_SUPPORT_OPERATOR_EXPLICIT
#endif
#if __has_feature(cxx_override_control)
#define BSLS_COMPILERFEATURES_SUPPORT_FINAL
#define BSLS_COMPILERFEATURES_SUPPORT_OVERRIDE
#endif
#if __has_feature(cxx_nullptr)
#define BSLS_COMPILERFEATURES_SUPPORT_NULLPTR
#endif
// clang 3.1
#if __has_feature(cxx_generalized_initializers)
#define BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS
#endif
#if __has_feature(cxx_alignas)
#define BSLS_COMPILERFEATURES_SUPPORT_ALIGNAS
#endif
// clang 3.3
#if __has_feature(cxx_decltype)
#define BSLS_COMPILERFEATURES_SUPPORT_DECLTYPE
#endif
#if __has_feature(cxx_attributes)
#define BSLS_COMPILERFEATURES_SUPPORT_ATTRIBUTE_NORETURN
// clang supports __attribute__((noreturn)) in earlier versions
#endif
#endif


// MSVC
// http://msdn.microsoft.com/en-us/library/hh567368.aspx
// http://blogs.msdn.com/b/vcblog/archive/2014/06/11/c-11-14-feature-tables-for-visual-studio-14-ctp1.aspx
// MSVC enables C++11 features automatically in versions that provide the
// feature.  Features can not be disabled.
//
//: * extern template is not supported. It is documented as being
//:   "supported" but behaves in a non-conforming manner.
#if defined(BSLS_PLATFORM_CMP_MSVC)
#if BSLS_PLATFORM_CMP_VERSION >= 1600  // Microsoft Visual Studio 2010
#define BSLS_COMPILERFEATURES_SUPPORT_DECLTYPE
#define BSLS_COMPILERFEATURES_SUPPORT_NULLPTR
#define BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
#define BSLS_COMPILERFEATURES_SUPPORT_STATIC_ASSERT
#endif
#if BSLS_PLATFORM_CMP_VERSION >= 1800  // Microsoft Visual Studio 2013
#define BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES
#define BSLS_COMPILERFEATURES_SUPPORT_FINAL
#define BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS
#define BSLS_COMPILERFEATURES_SUPPORT_OPERATOR_EXPLICIT
#define BSLS_COMPILERFEATURES_SUPPORT_OVERRIDE
#define BSLS_COMPILERFEATURES_SUPPORT_DEFAULTED_FUNCTIONS
#define BSLS_COMPILERFEATURES_SUPPORT_DELETED_FUNCTIONS
#endif
#if BSLS_PLATFORM_CMP_VERSION >= 1900  // Microsoft Visual Studio 2015
// Note that while MSVC 2013 supports variadic templates in principle, there
// are sufficient problems with the implementation that we defer support until
// the 2015 compiler where those issues are ironed out.
#define BSLS_COMPILERFEATURES_SUPPORT_VARIADIC_TEMPLATES
#endif
// MSVC has __declspec(noreturn)
// (see bsls_bslexceptionutil.h bslstl_stdexceptutil.h)
//#define BSLS_COMPILERFEATURES_SUPPORT_ATTRIBUTE_NORETURN
// (not yet supported in MSVC)
//#define BSLS_COMPILERFEATURES_SUPPORT_INCLUDE_NEXT
#endif


// IBM Visual Age xlC 11.1 and better include increasing support for C++11
// http://www-01.ibm.com/support/knowledgecenter/SSGH3R_13.1.0/com.ibm.xlcpp131.aix.doc/compiler_ref/macros_lang_levels.html
// http://www-01.ibm.com/support/knowledgecenter/SSGH3R_13.1.0/com.ibm.xlcpp131.aix.doc/compiler_ref/opt_langlvl.html
// xlC_r -qlanglvl=extended    (include next, extern template, noreturn)
// xlC_r -qlanglvl=extended0x  (additional features)
#if defined(BSLS_PLATFORM_CMP_IBM)
#if defined(__IBMCPP_CONSTEXPR)
#define BSLS_COMPILERFEATURES_SUPPORT_CONSTEXPR
#endif
#if defined(__IBMCPP_DECLTYPE)
#define BSLS_COMPILERFEATURES_SUPPORT_DECLTYPE
#endif
#if defined(__IBMCPP_DEFAULTED_AND_DELETED_FUNCTIONS)
#define BSLS_COMPILEFEATURES_SUPPORT_DEFAULTED_FUNCTIONS
#define BSLS_COMPILERFEATURES_SUPPORT_DELETED_FUNCTIONS
#endif
#if defined(__IBMCPP_EXPLICIT)
#define BSLS_COMPILERFEATURES_SUPPORT_OPERATOR_EXPLICIT
#endif
#if defined(__IBMCPP_EXTERN_TEMPLATE)
#define BSLS_COMPILERFEATURES_SUPPORT_EXTERN_TEMPLATE
#endif
#if defined(__IBM_INCLUDE_NEXT)
#define BSLS_COMPILERFEATURES_SUPPORT_INCLUDE_NEXT
#endif
#if defined(__IBMCPP_NULLPTR)
#define BSLS_COMPILERFEATURES_SUPPORT_NULLPTR
#endif
#if defined(__IBMCPP_OVERRIDE)
#define BSLS_COMPILERFEATURES_SUPPORT_FINAL
#define BSLS_COMPILERFEATURES_SUPPORT_OVERRIDE
#endif
#if defined(__IBMCPP_RVALUE_REFERENCES)
#define BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
#endif
#if defined(__IBMCPP_STATIC_ASSERT)
#define BSLS_COMPILERFEATURES_SUPPORT_STATIC_ASSERT
#endif
#if defined(__IBMCPP_VARIADIC_TEMPLATES)
#define BSLS_COMPILERFEATURES_SUPPORT_VARIADIC_TEMPLATES
#endif
#if defined(__IBMC_NORETURN)
#define BSLS_COMPILERFEATURES_SUPPORT_ATTRIBUTE_NORETURN
#endif
// (not yet supported in xlC)
//#define BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES
//#define BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT
#endif


// Oracle Solaris Studio 12.4 claims C++11 support except for C++11
// concurrency and atomic operations, and for user-defined literals
// http://docs.oracle.com/cd/E37069_01/html/E37071/gncix.html#scrolltoc
// No C++11 features are available by default. To use any C++11 features,
// you must use the new -std=c++11 option with the CC compiler.
// (__cplusplus >= 201103L when Oracle Solaris Studio CC -std=c++11 is invoked)
// CC -std=c++11
#if defined(BSLS_PLATFORM_CMP_SUN) && BSLS_PLATFORM_CMP_VERSION >= 0x5130 \
 && __cplusplus >= 201103L
#define BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES
#define BSLS_COMPILERFEATURES_SUPPORT_CONSTEXPR
#define BSLS_COMPILERFEATURES_SUPPORT_DECLTYPE
#define BSLS_COMPILERFEATURES_SUPPORT_DEFAULTED_FUNCTIONS
#define BSLS_COMPILERFEATURES_SUPPORT_DELETED_FUNCTIONS
#define BSLS_COMPILERFEATURES_SUPPORT_EXTERN_TEMPLATE
#define BSLS_COMPILERFEATURES_SUPPORT_FINAL
#define BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS
#define BSLS_COMPILERFEATURES_SUPPORT_INCLUDE_NEXT
#define BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT
#define BSLS_COMPILERFEATURES_SUPPORT_NULLPTR
#define BSLS_COMPILERFEATURES_SUPPORT_OPERATOR_EXPLICIT
#define BSLS_COMPILERFEATURES_SUPPORT_OVERRIDE
#define BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
#define BSLS_COMPILERFEATURES_SUPPORT_STATIC_ASSERT
#define BSLS_COMPILERFEATURES_SUPPORT_VARIADIC_TEMPLATES
#define BSLS_COMPILERFEATURES_SUPPORT_ATTRIBUTE_NORETURN
#endif


// Feature interactions
#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) &&  \
   !defined(BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES)
#   undef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
    // BDE move-emulation for C++03 requires support for alias templates in
    // order to provide a transparent upgrade to the C++11 syntax that also
    // supports implicit-move from rvalues.
#endif
    //  *** Simulate various C++11 features ***

#ifndef BSLS_COMPILERFEATURES_SUPPORT_VARIADIC_TEMPLATES
#   define BSLS_COMPILERFEATURES_SIMULATE_VARIADIC_TEMPLATES 1

#   ifndef BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
#       define BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES 1
#   endif

#   if BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
namespace BloombergLP {
namespace bsls {

    enum CompilerFeaturesNilT { COMPILERFEATURESNILV = 0x7fff6f76 };

#   define BSLS_COMPILERFEATURES_NILT BloombergLP::bsls::CompilerFeaturesNilT
#   define BSLS_COMPILERFEATURES_NILV BloombergLP::bsls::CompilerFeaturesNilV

#   define BSLS_COMPILERFEATURES_NILTR(n) BSLS_COMPILERFEATURES_NILT,
#   define BSLS_COMPILERFEATURES_FILLT(n)  \
     BSLS_MACROREPEAT(n,BSLS_COMPILERFEATURES_NILTR) BSLS_COMPILERFEATURES_NILT

#   define BSLS_COMPILERFEATURES_NILVR(n) BSLS_COMPILERFEATURES_NILV,
#   define BSLS_COMPILERFEATURES_FILLV(n)  \
     BSLS_MACROREPEAT(n,BSLS_COMPILERFEATURES_NILVR) BSLS_COMPILERFEATURES_NILV

} // close package namespace
} // close enterprise namespace

#   endif // BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
#endif // ! BSLS_COMPILERFEATURES_SUPPORT_VARIADIC_TEMPLATES

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
// Use real perfect-forwarding; no workaround needed.  Note that the explicit
// 'BloombergLP' qualification is needed for 'bsls::Util' when it is used from
// within namespace 'bsl', the BDE standard library implementation.
#   define BSLS_COMPILERFEATURES_FORWARD_REF(T) T&&
#   define BSLS_COMPILERFEATURES_FORWARD(T,V)       \
        ::BloombergLP::bsls::Util::forward<T>(V)
#endif

#endif

// ----------------------------------------------------------------------------
// Copyright 2013-2015 Bloomberg Finance L.P.
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
