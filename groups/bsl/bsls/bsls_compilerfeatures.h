// bsls_compilerfeatures.h                                            -*-C++-*-
#ifndef INCLUDED_BSLS_COMPILERFEATURES
#define INCLUDED_BSLS_COMPILERFEATURES

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide macros to identify compiler support for C++11 features.
//
//@CLASSES:
//
//@MACROS
//  BSLS_COMPILERFEATURES_CPLUSPLUS: Portable version of '__cplusplus'
//  BSLS_COMPILERFEATURES_INITIALIZER_LIST_LEAKS_ON_EXCEPTIONS: compiler bug
//  BSLS_COMPILERFEATURES_PP_LINE_IS_ON_FIRST: __LINE__ value for multi-line
//  BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES: flag for alias templates
//  BSLS_COMPILERFEATURES_SUPPORT_ALIGNAS: flag for 'alignas'
//  BSLS_COMPILERFEATURES_SUPPORT_ATTRIBUTE_NORETURN: '[[noreturn]]' attribute
//  BSLS_COMPILERFEATURES_SUPPORT_ATTRIBUTE_NODISCARD: '[[nodiscard]]'
//  BSLS_COMPILERFEATURES_SUPPORT_ATTRIBUTE_FALLTHROUGH: '[[fallthrough]]'
//  BSLS_COMPILERFEATURES_SUPPORT_ATTRIBUTE_MAYBE_UNUSED: '[[maybe_unused]]'
//  BSLS_COMPILERFEATURES_SUPPORT_CONSTEXPR: 'constexpr' specifier
//  BSLS_COMPILERFEATURES_SUPPORT_CONSTEXPR_CPP14: C++14 'constexpr' spec.
//  BSLS_COMPILERFEATURES_SUPPORT_CONSTEXPR_CPP17: C++17 'constexpr' spec.
//  BSLS_COMPILERFEATURES_SUPPORT_DECLTYPE: flag for 'decltype'
//  BSLS_COMPILERFEATURES_SUPPORT_DEFAULT_TEMPLATE_ARGS: for function templates
//  BSLS_COMPILERFEATURES_SUPPORT_DEFAULTED_FUNCTIONS: explicit '= default'
//  BSLS_COMPILERFEATURES_SUPPORT_DELETED_FUNCTIONS: explicit '= deleted'
//  BSLS_COMPILERFEATURES_SUPPORT_ENUM_CLASS: flag for 'enum class'
//  BSLS_COMPILERFEATURES_SUPPORT_EXTERN_TEMPLATE: flag for 'extern template'
//  BSLS_COMPILERFEATURES_SUPPORT_FINAL: flag for 'final' keyword
//  BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS: init-list syntax
//  BSLS_COMPILERFEATURES_SUPPORT_HAS_INCLUDE: flag for '__has_include'
//  BSLS_COMPILERFEATURES_SUPPORT_INCLUDE_NEXT: flag for 'include_next'
//  BSLS_COMPILERFEATURES_SUPPORT_INLINE_NAMESPACE: flag for 'inline namespace'
//  BSLS_COMPILERFEATURES_SUPPORT_INLINE_VARIABLES: flag for 'inline' variables
//  BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT: 'noexcept' operator
//  BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT_TYPES: func-type includes 'noexcept'
//  BSLS_COMPILERFEATURES_SUPPORT_NULLPTR: flag for 'nullptr'
//  BSLS_COMPILERFEATURES_SUPPORT_OPERATOR_EXPLICIT: 'explicit' operator
//  BSLS_COMPILERFEATURES_SUPPORT_OVERRIDE: 'override' keyword
//  BSLS_COMPILERFEATURES_SUPPORT_RAW_STRINGS: C++11-style R"tag(string)tag"
//  BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS: ref-qualified member function
//  BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES: flag for rvalue references
//  BSLS_COMPILERFEATURES_SUPPORT_STATIC_ASSERT: flag for 'static_assert'
//  BSLS_COMPILERFEATURES_SUPPORT_THROW_SPECIFICATIONS: C++98 exception specs.
//  BSLS_COMPILERFEATURES_SUPPORT_TRAITS_HEADER: has <type_traits> header
//  BSLS_COMPILERFEATURES_SUPPORT_UNICODE_CHAR_TYPES: flag for 'char(16|32)_t'
//  BSLS_COMPILERFEATURES_SUPPORT_VARIABLE_TEMPLATES: variable templates flag
//  BSLS_COMPILERFEATURES_SUPPORT_VARIADIC_TEMPLATES: flag for variadic params
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
//: 'BSLS_COMPILERFEATURES_CPLUSPLUS':
//:     This macro provides a portable way to determine the version of the C++
//:     standard mode that is being used.  In general, this has the same value
//:     as the stanard '__cplusplus' macro, but on some compilers with some
//:     settings the standard macro does not get assigned the correct value.
//:     The values generally set (as defined in the C++ standard) are the year
//:     and month when the standard was completed, and the value of this macro
//:     should be compared with the appropriate constants -- '199711L',
//:     '201103L', '201402L', '201703L', etc.
//:
//: 'BSLS_COMPILERFEATURES_INITIALIZER_LIST_LEAKS_ON_EXCEPTIONS':
//:     The 'BSLS_COMPILERFEATURES_INITIALIZER_LIST_LEAKS_ON_EXCEPTIONS' macro
//:     is defined for implementations that leak temporary objects constructed
//:     trying to initialize a 'std::initializer_list' object in the event that
//:     one of the elements of the list throws from its constructor.  This is
//:     known to affect gcc as recently as the 8.x series, and the Sun CC
//:     compiler in C++11 mode. This would often reveal itself as a spurious
//:     memory leak in exception-safety tests for 'initializer_list'
//:     constructors, so rises to the level of a generally supported
//:     defect-detection macro.
//:
//: 'BSLS_COMPILERFEATURES_PP_LINE_IS_ON_FIRST':
//:     The 'BSLS_COMPILERFEATURES_PP_LINE_IS_ON_FIRST' macro is defined for
//:     implementations that implement WG14 N2322
//:     (http://www.open-std.org/jtc1/sc22/wg14/www/docs/n2322.htm)
//:     "Recommended practice" on the substitution value for the '__LINE__'
//:     (predefined) preprocessor macro when expanding a macro that uses
//:     '__LINE__' in its body, and that macro invocation spans multiple source
//:     lines (logical or physical).  When macro is defined '__LINE__' is
//:     substituted with the line number of the first character of the macro
//:     name of the (multi-line) macro invocation.  When this macro is not
//:     defined '__LINE__' is replaced (as traditional on older C++ compilers)
//:     by the line number of the last character of the  macro invocation that
//:     expands to a use of '__LINE__'.  See also
//:     {Example 2: '__LINE__' macro multi-line value}.
//:
//: 'BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES':
//:     This macro is defined if alias templates are supported by the current
//:     compiler settings for this platform.
//:
//: 'BSLS_COMPILERFEATURES_SUPPORT_ALIGNAS'
//:     This macro is defined if 'alignas' alignment specifier is supported by
//:     the current compiler settings for this platform.
//:
//: 'BSLS_COMPILERFEATURES_SUPPORT_ATTRIBUTE_NORETURN':
//:     This macro is defined if the '[[noreturn]]' attribute is supported by
//:     the current compiler settings for this platform.
//:
//: 'BSLS_COMPILERFEATURES_SUPPORT_ATTRIBUTE_NODISCARD':
//:     This macro is defined if the '[[nodiscard]]' attribute is supported by
//:     the current compiler settings for this platform.
//:
//: 'BSLS_COMPILERFEATURES_SUPPORT_ATTRIBUTE_FALLTHROUGH':
//:     This macro is defined if the '[[fallthrough]]' attribute is supported
//:     by the current compiler settings for this platform.
//:
//: 'BSLS_COMPILERFEATURES_SUPPORT_ATTRIBUTE_MAYBE_UNUSED':
//:     This macro is defined if the '[[maybe_unused]]' attribute is supported
//:     by the current compiler settings for this platform.
//:
//: 'BSLS_COMPILERFEATURES_SUPPORT_CONSTEXPR':
//:     This macro is defined if 'constexpr' is supported by the current
//:     compiler settings for this platform.
//:
//: 'BSLS_COMPILERFEATURES_SUPPORT_CONSTEXPR_CPP14':
//:     This macro is defined if 'constexpr' with C++14 semantics is supported
//:     by the current compiler settings for this platform.  In particular,
//:     this allows multiple statements in a 'constexpr' function; changing
//:     state of local variables within the function; and making 'non-const'
//:     member functions 'constexpr'.
//:
//: 'BSLS_COMPILERFEATURES_SUPPORT_CONSTEXPR_CPP17':
//:     This macro is defined if 'constexpr' with C++17 semantics is supported
//:     by the current compiler settings for this platform.  In particular,
//:     this allows lambda functions to be defined in a 'constexpr' function.
//:
//: 'BSLS_COMPILERFEATURES_SUPPORT_DECLTYPE':
//:     This macro is defined if 'decltype' is supported by the current
//:     compiler settings for this platform.
//:
//: 'BSLS_COMPILERFEATURES_SUPPORT_DEFAULT_TEMPLATE_ARGS':
//:     This macro is defined if default template arguments for function
//:     templates are supported by the current compiler settings for this
//:     platform.
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
//: 'BSLS_COMPILERFEATURES_SUPPORT_HAS_INCLUDE':
//:     This macro is defined if '__has_include' is supported by the current
//:     compiler settings for this platform.
//:
//: 'BSLS_COMPILERFEATURES_SUPPORT_INCLUDE_NEXT':
//:     This macro is defined if 'include_next' is supported by the current
//:     compiler settings for this platform.
//:
//: 'BSLS_COMPILERFEATURES_SUPPORT_INLINE_NAMESPACE'
//:     This macro is defined if 'inline' namespaces introduced in C++11
//:     standard are supported by the current compiler settings for this
//:     platform.
//:
//: 'BSLS_COMPILERFEATURES_SUPPORT_INLINE_VARIABLES'
//:     This macro is defined if 'inline' variables introduced in C++17
//:     standard are supported by the current compiler settings for this
//:     platform.
//:
//: 'BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT'
//:     This macro is defined if the 'noexcept' keyword is supported by the
//:     current compiler, both for designating a function as not throwing and
//:     for testing if an expression may throw.  The definition of this macro
//:     does not depend on whether the current compiler configuration has
//:     disabled support for exceptions.
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
//: 'BSLS_COMPILERFEATURES_SUPPORT_RAW_STRINGS'
//:     This macro is defined if the compiler supports C++-11 style
//:     R"tag(string)tag" strings.
//:
//: 'BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS'
//:     This macro is defined if member functions with trailing reference
//:     qualifiers (e.g., 'void myfunc(int) &&') are supported by the current
//:     compiler settings for this platform.
//:
//: 'BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES'
//:     This macro is defined if rvalue references are supported by the current
//:     compiler settings for this platform.
//:
//: 'BSLS_COMPILERFEATURES_SUPPORT_STATIC_ASSERT'
//:     This macro is defined if 'static_assert' is supported by the current
//:     compiler settings for this platform.
//:
//: 'BSLS_COMPILERFEATURES_SUPPORT_THROW_SPECIFICATIONS'
//:     This macro is defined if dynamic exception specifications are supported
//:     by the current compiler.  Dynamic exception specifications were
//:     deprecated in C++11, and actively removed from the  language in C++17.
//:     The definition of this macro does not depend on whether the current
//:     compiler configuration has disabled support for exceptions.
//:
//: 'BSLS_COMPILERFEATURES_SUPPORT_TRAITS_HEADER'
//:     This macro is defined if the standard library for the current compiler
//:     supports some form of the standard <type_traits> header.  Note that
//:     many standard library implementations provided partial support for a
//:     long time, and those libraries WILL be identified as providing the
//:     <type_traits> header.
//:
//: 'BSLS_COMPILERFEATURES_SUPPORT_UNICODE_CHAR_TYPES'
//:     This macro is defined if the compiler supports the 'char16_t' and
//:     'char32_t' types and the related u and U prefixes for character- and
//:     string-literal values.
//:
//: 'BSLS_COMPILERFEATURES_SUPPORT_VARIADIC_TEMPLATES'
//:     This macro is defined if variadic template parameters are supported by
//:     the current compiler settings for this platform.
//:
//: 'BSLS_COMPILERFEATURES_SUPPORT_VARIABLE_TEMPLATES'
//:     This macro is defined if variable templates introduced in C++14
//:     standard are supported by the current compiler settings for this
//:     platform.
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
///Example 2: '__LINE__' macro multi-line value differences demonstration
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Note that this isn't an example of use, it is a demonstration of compiler
// preprocessor behavior and the 'BSLS_COMPILERFEATURES_PP_LINE_IS_ON_FIRST'
// macro.
//
// Sometimes we write code that uses line numbers for logging or other
// purposes.  Although most of the time the precise values of those line
// numbers (in program output, such as assertions, or logs) is unimportant
// (the output is read by humans who are good at finding the line that
// actually emitted the text), sometimes programs read other programs' output.
// In such cases the precise values for the line numbers may matter.  This
// example demonstrates the two ways our currectly suported C++ compilers
// generate line numbers in multi-line macro expansion contexts (from the
// '__LINE__' macro), and how the presence (or absence) of the macro
// 'BSLS_COMPILERFEATURES_PP_LINE_IS_ON_FIRST' indicates which method the
// current compiler uses.  First, we define a macro that uses '__LINE__' in its
// replacement text:
//..
//    #define THATS_MY_LINE(dummy) __LINE__
//..
// Note that this macro has the function-like syntax so we can easily span its
// invocation to multiple lines.
//
// Next, we record the current line number in a constant, and also record the
// line number from our macro, but we span the macro invocation multiple lines
// to invoke the unspecified behavior.
//
// The two lines must follow each other due to working with line numbering:
//..
//    const long A_LINE = __LINE__;
//    const long LINE_FROM_MACRO = THATS_MY_LINE
//        (
//            "dummy"
//        )
//.       ;
//..
// We deliberately extended the macro invocation to more than 2 physical source
// code lines so that we can demonstrate the two distinct behaviors: using the
// line number of the first character or the last.  Extending the number of
// lines *beyond* the macro invocation (by placing the semicolon on its own
// line) has no effect on the line number substitution inside the macro.  The
// dummy argument is required for C++03 compatibility.
//
// If we follow the definition of 'A_LINE' without any intervening empty lines
// the line number of the first character of the macro invocation will be
// 'A_LINE + 1', while the last falls on line 'A_LINE + 4'.
//
// Now we demonstrate the two different behaviors and how the presence of
// 'BSLS_COMPILERFEATURES_PP_LINE_IS_ON_FIRST' indicates which one will occur:
//..
//    #ifdef BSLS_COMPILERFEATURES_PP_LINE_IS_ON_FIRST
//        assert(A_LINE + 1 == LINE_FROM_MACRO);
//    #else
//        assert(A_LINE + 4 == LINE_FROM_MACRO);
//    #endif
//..
// Finally note that WG14 N2322 defines this behavior is *unspecified*,
// therefore it is in the realm of possibilities, although not likely (in C++
// compilers) that further, more complicated or even indeterminite behaviors
// may arise.
//
///Bugs in Compilers
///-----------------
//
///'BSLS_COMPILERFEATURES_INITIALIZER_LIST_LEAKS_ON_EXCEPTIONS'
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// This macro is defined if the compiler leaks temporary objects constructed
// trying to initialize a 'std::initializer_list' object in the event that one
// of the elements of the list throws from its constructor.
//
//: o Compilers affected:
//:   o GCC
//:   o Oracle CC 12.6
//
///Feature Support in Compilers
///----------------------------
//
// Note that https://en.cppreference.com/w/cpp/compiler_support is a useful
// reference for initial versions to test for support for various features.
//
///'BSLS_COMPILERFEATURES_PP_LINE_IS_ON_FIRST'
///- - - - - - - - - - - - - - - - - - - - - -
// This macro is defined if the compiler substitutes the '__LINE__' macro in
// macro expansions with the line number of the first character of the macro
// invocation, which is the recommended practice for the C preprocessor (see
// WG14 N2322 at http://www.open-std.org/jtc1/sc22/wg14/www/docs/n2322.htm).
// Note that all BDE supported compilers that do not define this macro use the
// line number of the last character of the macro invocation instead.
//
//: o Compilers that implement N2322 Recommended practice:
//:   o GCC 9.0 and later
//
///'BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES'
///- - - - - - - - - - - - - - - - - - - - - - - -
// This macro is defined if the compiler supports syntax to introduce a
// 'typedef'-name using alias-declaration syntax, declaring a name for a family
// of types.
//
//: o Compiler support:
//:   o GCC 4.7
//:   o Clang 3.0
//:   o Visual Studio 2013 version 12.0 (_MSC_VER 1800)
//:   o Oracle CC 12.4
//
///'BSLS_COMPILERFEATURES_SUPPORT_ALIGNAS'
///- - - - - - - - - - - - - - - - - - - -
// This macro is defined if the compiler supports the 'alignas' alignment
// specifier.
//
//: o Compiler support:
//:   o GCC 4.8
//:   o Clang 3.0
//
// This feature is not yet supported in Visual Studio, IBM xlC, Oracle CC.
//
///'BSLS_COMPILERFEATURES_SUPPORT_ATTRIBUTE_NORETURN'
/// - - - - - - - - - - - - - - - - - - - - - - - - -
// This macro is defined if the compiler supports '[[noreturn]]' C++11
// attribute syntax.  MSVC supports the attribute with alternative syntax
// __declspec(noreturn), earlier versions of GCC and Clang support the
// alternative syntax '__attribute__((noreturn))', and xlC supports the
// alternative syntax '_Noreturn'.  This macro does not support alternative
// syntaxes.
//
//: o Compiler support:
//:   o GCC 4.8
//:   o Clang 3.3
//:   o Oracle CC 12.4
//
///'BSLS_COMPILERFEATURES_SUPPORT_ATTRIBUTE_NODISCARD'
/// - - - - - - - - - - - - - - - - - - - - - - - - -
// This macro is defined if the compiler supports '[[nodiscard]]' C++17
// attribute syntax.
//
//: o Compiler support:
//:   o Visual Studio 2017 version 15.3 (_MSC_VER 1911)
//
///'BSLS_COMPILERFEATURES_SUPPORT_ATTRIBUTE_FALLTHROUGH'
/// - - - - - - - - - - - - - - - - - - - - - - - - - -
// This macro is defined if the compiler supports '[[fallthrough]]' C++17
// attribute syntax.
//
//: o Compiler support:
//:   o GCC 7.3
//:   o Clang 3.3
//:   o Visual Studio 2017 version 15.0 (_MSC_VER 1910)
//
///'BSLS_COMPILERFEATURES_SUPPORT_ATTRIBUTE_MAYBE_UNUSED'
/// - - - - - - - - - - - - - - - - - - - - - - - - - - -
// This macro is defined if the compiler supports '[[maybe_unused]]' C++17
// attribute syntax.
//
//: o Compiler support:
//:   o GCC 4.8
//:   o Clang 3.3
//:   o Visual Studio 2017 version 15.3 (_MSC_VER 1911)
//
///'BSLS_COMPILERFEATURES_SUPPORT_CONSTEXPR'
///- - - - - - - - - - - - - - - - - - - - -
// This macro is defined in the compiler supports the 'constexpr' reserved
// keyword.
//
//: o Compiler support:
//:   o GCC 4.7
//:   o Clang 3.1
//:   o IBM xlC 13
//:   o Oracle CC 12.4
//
///'BSLS_COMPILERFEATURES_SUPPORT_CONSTEXPR_CPP14'
///- - - - - - - - - - - - - - - - - - - - - - - - -
// This macro is defined in the compiler supports the 'constexpr' reserved
// keyword with C++14 semantics.
//
//: o Compiler support:
//:   o gcc 5
//:   o clang 3.4
//:   o Visual Studio 2017 version 15.0 (_MSC_VER 1910)
//
///'BSLS_COMPILERFEATURES_SUPPORT_CONSTEXPR_CPP17'
///- - - - - - - - - - - - - - - - - - - - - - - - -
// This macro is defined in the compiler supports the 'constexpr' reserved
// keyword with C++17 semantics.
//
//: o Compiler support:
//:   o gcc 7.3
//:   o clang 5
//:   o Visual Studio 2017 version 15.0 (_MSC_VER 1910)
//
///'BSLS_COMPILERFEATURES_SUPPORT_DECLTYPE'
/// - - - - - - - - - - - - - - - - - - - -
// This macro is defined if the compiler supports the 'decltype' reserved word.
//
//: o Compiler support:
//:   o GCC 4.3
//:   o Clang 3.3
//:   o Visual Studio 2010 version 10.0 (_MSC_VER 1600)
//:   o IBM xlC 11.1
//:   o Oracle CC 12.4
//
///'BSLS_COMPILERFEATURES_SUPPORT_DEFAULT_TEMPLATE_ARGS'
///- - - - - - - - - - - - - - - - - - - - - - - - - - -
// This macro is defined if the compiler supports syntax to introduce defaulted
// functions.
//
//: o Compiler support:
//:   o GCC 4.3
//:   o Clang 2.9
//:   o Visual Studio 2013 version 12.0 (_MSC_VER 1800)
//:   o IBM xlC not supported?
//:   o Oracle CC 12.4
//
///'BSLS_COMPILERFEATURES_SUPPORT_DEFAULTED_FUNCTIONS'
///- - - - - - - - - - - - - - - - - - - - - - - - - -
// This macro is defined if the compiler supports syntax to introduce defaulted
// functions.
//
//: o Compiler support:
//:   o GCC 4.4
//:   o Clang 2.9
//:   o Visual Studio 2013 version 12.0 (_MSC_VER 1800)
//:   o IBM xlC 13.1
//:   o Oracle CC 12.4
//
///'BSLS_COMPILERFEATURES_SUPPORT_DELETED_FUNCTIONS'
///- - - - - - - - - - - - - - - - - - - - - - - - -
// This macro is defined if the compiler supports syntax to introduce deleted
// functions.
//
//: o Compiler support:
//:   o GCC 4.4
//:   o Clang 2.9
//:   o Visual Studio 2013 version 12.0 (_MSC_VER 1800)
//:   o IBM xlC 13.1
//:   o Oracle CC 12.4
//
///'BSLS_COMPILERFEATURES_SUPPORT_EXTERN_TEMPLATE'
///- - - - - - - - - - - - - - - - - - - - - - - -
// This macro is defined if the compiler supports allowing suppression of
// implicit instantiation of templates by prefixing an explicit instantiation
// directive with the 'extern' keyword.
//
//: o Compiler support:
//:   o GCC 3.3
//:   o Clang (any)
//:   o Visual Studio 2010 version 10.0 (_MSC_VER 1600)
//:   o IBM xlC 11.1
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
//:   o GCC 4.7
//:   o Clang 3.0
//:   o Visual Studio 2012 version 11.0 (_MSC_VER 1700)
//:   o IBM xlC 11.1
//:   o Oracle CC 12.4
//
///'BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS'
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// This macro is defined if the compiler supports generalized initializers.
//
//: o Compiler support:
//:   o GCC 4.4
//:   o Clang 3.1
//:   o Visual Studio 2013 version 12.0 (_MSC_VER 1800)
//:   o IBM xlC not supported
//:   o Oracle CC 13
//
///'BSLS_COMPILERFEATURES_SUPPORT_HAS_INCLUDE'
///- - - - - - - - - - - - - - - - - - - - - -
// This macro is defined if the compiler supports '__has_include' semantics as
// defined in the C++17 Standard (see also P006R1).  Note that this
// preprocessor feature is often available in earlier language dialects.
//
//: o Compiler support:
//:   o GCC 5.0
//:   o clang (any)
//:   o Visual Studio 2017 version 15.3 (_MSC_VER 1911)
//
///'BSLS_COMPILERFEATURES_SUPPORT_INCLUDE_NEXT'
/// - - - - - - - - - - - - - - - - - - - - - -
// This macro is defined if the compiler supports #include_next semantics
// expected by the deprecated and internal include code wrapped by
// BSL_OVERRIDES_STD macro.
//
//: o Compiler support:
//:   o GCC (any)
//:   o Clang (any)
//:   o IBM xlC 8
//:   o Oracle CC 12.4
//
///'BSLS_COMPILERFEATURES_SUPPORT_INLINE_NAMESPACE'
/// - - - - - - - - - - - - - - - - - - - - - - - - -
// This macro is defined if the compiler supports 'inline' namespaces
// introduced in C++11 standard.
//
//: o Compiler support:
//:   o GCC 4.4
//:   o Clang 2.9
//:   o Visual Studio 2015 version 14.0 (_MSC_VER 1900)
//:   o IBM xlC 11.1
//:   o Oracle CC 12.4
//
///'BSLS_COMPILERFEATURES_SUPPORT_INLINE_VARIABLES'
/// - - - - - - - - - - - - - - - - - - - - - - - - -
// This macro is defined if the compiler supports 'inline' variables introduced
// in C++17 standard.
//
//: o Compiler support:
//:   o GCC 7.0
//:   o Clang 3.9
//:   o Visual Studio 2017 version 15.5 (_MSC_VER 1912)
//
///'BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT'
/// - - - - - - - - - - - - - - - - - - - -
// This macro is defined if the compiler supports the 'noexcept' reserved
// keyword.
//
//: o Compiler support:
//:   o GCC 4.6
//:   o Clang 3.0
//
///'BSLS_COMPILERFEATURES_SUPPORT_NULLPTR'
///- - - - - - - - - - - - - - - - - - - -
// This macro is defined if the compiler supports the 'nullptr' reserved word.
//
//: o Compiler support:
//:   o GCC 4.6
//:   o Clang 3.0
//:   o Visual Studio 2010 version 10.0 (_MSC_VER 1600)
//:   o IBM xlC 13.1
//:   o Oracle CC 12.4
//
///'BSLS_COMPILERFEATURES_SUPPORT_OPERATOR_EXPLICIT'
///- - - - - - - - - - - - - - - - - - - - - - - - -
// This macro is defined if the compiler supports use of the 'explicit' keyword
// to indicate that a conversion operator only available for explicit
// conversions.
//
//: o Compiler support:
//:   o GCC 4.5
//:   o Clang 3.0
//:   o Visual Studio 2013 version 12.0 (_MSC_VER 1800)
//:   o IBM xlC 11.1
//:   o Oracle CC 12.4
//
///'BSLS_COMPILERFEATURES_SUPPORT_OVERRIDE'
/// - - - - - - - - - - - - - - - - - - - -
// This macro is defined if the compiler supports use of the 'override' keyword
// to indicate that a member function is meant to override a 'virtual' function
// (and cause a failure if that is not the case).
//
//: o Compiler support:
//:   o GCC 4.7
//:   o Clang 3.0
//:   o Visual Studio 2012 version 11.0 (_MSC_VER 1700)
//:   o IBM xlC 11.1
//:   o Oracle CC 12.4
//
///'BSLS_COMPILERFEATURES_SUPPORT_RAW_STRINGS'
///- - - - - - - - - - - - - - - - - - - - - -
// This macro is defined if the compiler supports the use of C++11-style
// R"tag(string)tag" strings.
//
//: o Compiler support:
//:   o GCC 4.5
//:   o Clang 3.3
//:   o Visual Studio 2013 version 12.0 (_MSC_VER 1800)
//:   o xlC 13.1.3 (Not yet supported by this component)
//:   o Oracle CC 12.4
//
///'BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS'
///- - - - - - - - - - - - - - - - - - - - - - - - -
// This macro is defined if member functions with trailing reference qualifiers
// (e.g., 'void myfunc(int) &') and rvalue reference qualifiers (e.g.,
// 'void myFunc(int) &&') are supported.  Note that this macro implies that
// 'BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES' is also defined.
//
//: o Compiler support:
//:   o gcc 4.5 (rvalue references v2.1; original draft support in gcc 4.3 is
//:     not correct with respect to final spec (v3.0))
//:   o clang 2.9
//:   o Visual Studio 2015 version 14.0 (_MSC_VER 1900)
//:   o xlC 12.1
//:   o Oracle CC 12.4
//
///'BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES'
///- - - - - - - - - - - - - - - - - - - - - - - - -
// This macro is defined if the compiler supports rvalue references and move
// semantics compatible with the final C++11 specification.  (Semantics have
// changed since early draft proposals.)
//
//: o Compiler support:
//:   o GCC 4.5 (rvalue references v2.1; original draft support in GCC 4.3 is
//:     not correct with respect to final spec (v3.0))
//:   o Clang 2.9
//:   o Visual Studio 2010 version 10.0 (_MSC_VER 1600)
//:   o IBM xlC 12.1
//:   o Oracle CC 12.4
//
///'BSLS_COMPILERFEATURES_SUPPORT_STATIC_ASSERT'
///- - - - - - - - - - - - - - - - - - - - - - -
// This macro is defined if the compiler supports the 'static_assert' reserved
// word.
//
//: o Compiler support:
//:   o GCC 4.3
//:   o Clang 2.9
//:   o Visual Studio 2010 version 10.0 (_MSC_VER 1600)
//:   o IBM xlC 11.1
//:   o Oracle CC 12.4
//
///'BSLS_COMPILERFEATURES_SUPPORT_VARIADIC_TEMPLATES'
/// - - - - - - - - - - - - - - - - - - - - - - - - -
// This macro is defined if the compiler supports the ability to express a
// template that accepts an arbitrary number of parameters in a type-safe
// manner.
//
//: o Compiler support:
//:   o GCC 4.3
//:   o Clang 2.9
//:   o Visual Studio 2015 version 14.0 (_MSC_VER 1900)
//:   o IBM xlC 11.1
//:   o Oracle CC 12.4
//
///'BSLS_COMPILERFEATURES_SUPPORT_VARIABLE_TEMPLATES'
/// - - - - - - - - - - - - - - - - - - - - - - - - -
// This macro is defined if the compiler supports variable templates introduced
// in C++14 standard.
//
//: o Compiler support:
//:   o gcc 5.0
//:   o clang 3.4
//:   o Visual Studio 2015 version 14.0 (_MSC_VER 1900)
//:   o Oracle CC 12.4

#include <bsls_platform.h>
#include <bsls_macrorepeat.h>

// ============================================================================
//                      UNIVERSAL MACRO DEFINITIONS
// ============================================================================

// First, define feature macros for any C++98 features that should be available
// on all compilers, until removed by a later standard.  These macros will be
// specifically undefined when compilers stop supporting that feature in a
// modern build mode.
#define BSLS_COMPILERFEATURES_SUPPORT_THROW_SPECIFICATIONS

// Use the standard compiler-independent feature-test macros (SD-6) for
// compilers that support them.  This support will be mandated by C++20, and it
// is expected that, at some point, future compilers will need only these
// universal definitions, and the platoform-specific detection below will need
// no further maintenance.
#if defined(__cpp_unicode_characters) && defined(__cpp_unicode_literals)
# define BSLS_COMPILERFEATURES_SUPPORT_UNICODE_CHAR_TYPES
#endif

// ============================================================================
//                      ATTRIBUTE DETECTION
// ============================================================================
#if (__cplusplus >= 201103L) && defined(__has_cpp_attribute)

#if __has_cpp_attribute(fallthrough)
#   define BSLS_COMPILERFEATURES_SUPPORT_ATTRIBUTE_FALLTHROUGH
#endif

#if __has_cpp_attribute(noreturn)
#   define BSLS_COMPILERFEATURES_SUPPORT_ATTRIBUTE_NORETURN
#endif

#if  __has_cpp_attribute(nodiscard)
#   define BSLS_COMPILERFEATURES_SUPPORT_ATTRIBUTE_NODISCARD
#endif

#if __has_cpp_attribute(maybe_unused)
#   define BSLS_COMPILERFEATURES_SUPPORT_ATTRIBUTE_MAYBE_UNUSED
#endif

#endif

#if defined(__cplusplus)
# define BSLS_COMPILERFEATURES_CPLUSPLUS __cplusplus
#else
# define BSLS_COMPILERFEATURES_CPLUSPLUS 199711L
#endif

// ============================================================================
//      PLATFORM SPECIFIC FEATURE DETECTION AND MACRO DEFINITIONS
// ============================================================================

//GCC
//  https://wiki.apache.org/stdcxx/C%2B%2B0xCompilerSupport
// As GCC tracked the evolving C++11 standard, earlier versions of the compiler
// have implementations of earlier specifications of features that can show up
// as bugs compared to the final standard.  Therefore, BDE does not attempt to
// support C++11 in GCC compilers prior to the 4.8 release.
#if defined(BSLS_PLATFORM_CMP_GNU)
# define BSLS_COMPILERFEATURES_INITIALIZER_LIST_LEAKS_ON_EXCEPTIONS 1

# define BSLS_COMPILERFEATURES_SUPPORT_INCLUDE_NEXT

# if BSLS_PLATFORM_CMP_VERSION >= 30300
#  define BSLS_COMPILERFEATURES_SUPPORT_EXTERN_TEMPLATE
# endif
# if BSLS_PLATFORM_CMP_VERSION >= 50000
#    define BSLS_COMPILERFEATURES_SUPPORT_HAS_INCLUDE
//   GCC provides this support prior to C++17, independant of language dialect.
# endif
// GCC -std=c++11 or -std=c++0x or -std=gnu++11 or -std=gnu++0x
# if defined(__GXX_EXPERIMENTAL_CXX0X__)
#  if BSLS_COMPILERFEATURES_CPLUSPLUS < 201103L
#    undef  BSLS_COMPILERFEATURES_CPLUSPLUS
#    define BSLS_COMPILERFEATURES_CPLUSPLUS 201103L
#  endif
#  if BSLS_PLATFORM_CMP_VERSION >= 40800
#    define BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES
#    define BSLS_COMPILERFEATURES_SUPPORT_ALIGNAS
#    define BSLS_COMPILERFEATURES_SUPPORT_ATTRIBUTE_NORETURN
// GCC supports __attribute__((noreturn)) in earlier versions
#    define BSLS_COMPILERFEATURES_SUPPORT_CONSTEXPR
#    define BSLS_COMPILERFEATURES_SUPPORT_DECLTYPE
#    define BSLS_COMPILERFEATURES_SUPPORT_DEFAULT_TEMPLATE_ARGS
#    define BSLS_COMPILERFEATURES_SUPPORT_DEFAULTED_FUNCTIONS
#    define BSLS_COMPILERFEATURES_SUPPORT_DELETED_FUNCTIONS
#    define BSLS_COMPILERFEATURES_SUPPORT_ENUM_CLASS
#    define BSLS_COMPILERFEATURES_SUPPORT_FINAL
#    define BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS
#    define BSLS_COMPILERFEATURES_SUPPORT_INLINE_NAMESPACE
#    define BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT
#    define BSLS_COMPILERFEATURES_SUPPORT_NULLPTR
#    define BSLS_COMPILERFEATURES_SUPPORT_OPERATOR_EXPLICIT
#    define BSLS_COMPILERFEATURES_SUPPORT_OVERRIDE
#    define BSLS_COMPILERFEATURES_SUPPORT_RAW_STRINGS
#    define BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS
#    define BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
#    define BSLS_COMPILERFEATURES_SUPPORT_STATIC_ASSERT
#    define BSLS_COMPILERFEATURES_SUPPORT_VARIADIC_TEMPLATES
#  endif
#  if BSLS_PLATFORM_CMP_VERSION >= 50000
#    define BSLS_COMPILERFEATURES_SUPPORT_TRAITS_HEADER
//   Note that while basic support is available in earlier versions of the
//   library, the full header is not implemented until gcc 5.0.
#  endif
# endif
# if BSLS_PLATFORM_CMP_VERSION >= 90000
#   define BSLS_COMPILERFEATURES_PP_LINE_IS_ON_FIRST
# endif
# if __cplusplus >= 201402L
#    define BSLS_COMPILERFEATURES_SUPPORT_CONSTEXPR_CPP14
#    define BSLS_COMPILERFEATURES_SUPPORT_VARIABLE_TEMPLATES
# endif
# if __cplusplus >= 201500
// GCC defines the '__cplusplus' macro to the non-standard value of 201500 to
// indicate experimental C++17 support.
#  if BSLS_PLATFORM_CMP_VERSION >= 70000
#    define BSLS_COMPILERFEATURES_SUPPORT_ATTRIBUTE_FALLTHROUGH
#    define BSLS_COMPILERFEATURES_SUPPORT_ATTRIBUTE_MAYBE_UNUSED
#    define BSLS_COMPILERFEATURES_SUPPORT_CONSTEXPR_CPP17
#    define BSLS_COMPILERFEATURES_SUPPORT_INLINE_VARIABLES
#    undef  BSLS_COMPILERFEATURES_SUPPORT_THROW_SPECIFICATIONS
#  endif
#  if BSLS_PLATFORM_CMP_VERSION >= 70200
// Note that early release of gcc 7.0 have a name-mangling bug with 'noexcept'
// on "abominable" function types.
#    define BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT_TYPES
#  endif
# endif
#endif

//Clang
// http://clang.llvm.org/cxx_status.html
// http://clang.llvm.org/docs/LanguageExtensions.html
#if defined(BSLS_PLATFORM_CMP_CLANG)
// Clang supported
#define BSLS_COMPILERFEATURES_SUPPORT_HAS_INCLUDE
#define BSLS_COMPILERFEATURES_SUPPORT_INCLUDE_NEXT
#define BSLS_COMPILERFEATURES_SUPPORT_EXTERN_TEMPLATE
// clang -std=c++11 or -std=c++0x or -std=gnu++11 or -std=gnu++0x Clang 2.9
#if __has_feature(cxx_rvalue_references)
#define BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
#endif
#if __has_feature(cxx_reference_qualified_functions) &&                       \
    __has_feature(cxx_rvalue_references)
#define BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS
#endif
#if __has_feature(cxx_default_function_template_args)
#define BSLS_COMPILERFEATURES_SUPPORT_DEFAULT_TEMPLATE_ARGS
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
#if __has_feature(cxx_inline_namespaces) || \
    __has_extension(cxx_inline_namespaces)
#define BSLS_COMPILERFEATURES_SUPPORT_INLINE_NAMESPACE
#endif
#if __has_feature(cxx_variadic_templates)
#define BSLS_COMPILERFEATURES_SUPPORT_VARIADIC_TEMPLATES
#endif
// Clang 3.0
#if __has_feature(cxx_alias_templates)
#define BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES
#endif
#if __has_feature(cxx_noexcept)
#define BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT
#endif
#if __has_feature(cxx_raw_string_literals)
#define BSLS_COMPILERFEATURES_SUPPORT_RAW_STRINGS
#endif
// Clang 3.1
#if __has_feature(cxx_constexpr)
#define BSLS_COMPILERFEATURES_SUPPORT_CONSTEXPR
#endif
// clang 3.4
#if __has_feature(cxx_relaxed_constexpr)
#define BSLS_COMPILERFEATURES_SUPPORT_CONSTEXPR_CPP14
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
// Clang 3.1
#if __has_feature(cxx_generalized_initializers)
#define BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS
#endif
#if __has_feature(cxx_alignas)
#define BSLS_COMPILERFEATURES_SUPPORT_ALIGNAS
#endif
// Clang 3.3
#if __has_feature(cxx_decltype)
#define BSLS_COMPILERFEATURES_SUPPORT_DECLTYPE
#endif
// clang 3.4
#if __has_feature(cxx_variable_templates)
#define BSLS_COMPILERFEATURES_SUPPORT_VARIABLE_TEMPLATES
#endif
#if __has_feature(cxx_attributes)
#define BSLS_COMPILERFEATURES_SUPPORT_ATTRIBUTE_NORETURN
// Clang supports __attribute__((noreturn)) in earlier versions
#endif
#if BSLS_COMPILERFEATURES_CPLUSPLUS < 201103L &&                              \
    defined(__GXX_EXPERIMENTAL_CXX0X__)
# undef  BSLS_COMPILERFEATURES_CPLUSPLUS
# define BSLS_COMPILERFEATURES_CPLUSPLUS 201103L
#endif
#if (__cplusplus >= 201103L ||                                                \
    (defined(__GXX_EXPERIMENTAL_CXX0X__) && defined(__APPLE_CC__)))           \
    && __has_include(<type_traits>)
#define BSLS_COMPILERFEATURES_SUPPORT_TRAITS_HEADER
#endif
// work only with --std=c++1z
#if __cplusplus >= 201703L  // C++17
#define BSLS_COMPILERFEATURES_SUPPORT_CONSTEXPR_CPP17
#define BSLS_COMPILERFEATURES_SUPPORT_INLINE_VARIABLES
#define BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT_TYPES
#endif
#if defined(__GXX_EXPERIMENTAL_CXX0X__)
    // When compiling in >= C++11 mode on a non-Darwin platform, assume Clang
    // >= 3.0 and a minimum GCC 4.8 toolchain, which supports it.
    #if (!defined(__APPLE_CC__) || __APPLE_CC__ >= 6000)
        #define BSLS_COMPILERFEATURES_SUPPORT_TRAITS_HEADER
    #endif
#endif

// clang does not yet follow WG14 N2322 Recommended practice
// #define BSLS_COMPILERFEATURES_PP_LINE_IS_ON_FIRST
#endif


//MSVC (Minimum supported version is MSVC 2013)
// http://msdn.microsoft.com/en-us/library/hh567368.aspx
// http://blogs.msdn.com/b/vcblog/archive/2014/06/11/c-11-14-feature-tables-for-visual-studio-14-ctp1.aspx
// MSVC enables C++11 features automatically in versions that provide the
// feature.  Features can not be disabled.
//
//: * extern template is not supported.  It is documented as being "supported"
//:   but behaves in a non-conforming manner.
#if defined(BSLS_PLATFORM_CMP_MSVC)

// Not only does Microsoft not always report the language dialect properly in
// '__cplusplus', many users depends upon it not reflecting the right version,
// therefore we need to use the Microsoft specific predefined macro
// '_MSVC_LANG'.  See https://goo.gl/ikfyDw and
// https://docs.microsoft.com/en-us/cpp/preprocessor/predefined-macros
// Note that '_MSVC_LANG' is only useful for distinguishing MSVC 2017 from MSVC
// 2014.

# undef   BSLS_COMPILERFEATURES_CPLUSPLUS
# if defined(_MSVC_LANG)
#  define BSLS_COMPILERFEATURES_CPLUSPLUS _MSVC_LANG
# else
#  define BSLS_COMPILERFEATURES_CPLUSPLUS 201103L
# endif

# undef BSLS_COMPILERFEATURES_SUPPORT_THROW_SPECIFICATIONS
// MSVC has never properly implemented this feature, even before it was removed
// by C++17.  It would parse the syntax, but the runtime behavior simply would
// simply ignore the exception specification.

# define BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES
# define BSLS_COMPILERFEATURES_SUPPORT_DECLTYPE
# define BSLS_COMPILERFEATURES_SUPPORT_DEFAULT_TEMPLATE_ARGS
# define BSLS_COMPILERFEATURES_SUPPORT_DEFAULTED_FUNCTIONS
# define BSLS_COMPILERFEATURES_SUPPORT_ENUM_CLASS
# define BSLS_COMPILERFEATURES_SUPPORT_FINAL
# define BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS
# define BSLS_COMPILERFEATURES_SUPPORT_NULLPTR
# define BSLS_COMPILERFEATURES_SUPPORT_OPERATOR_EXPLICIT
# define BSLS_COMPILERFEATURES_SUPPORT_OVERRIDE
# define BSLS_COMPILERFEATURES_SUPPORT_RAW_STRINGS
# define BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
# define BSLS_COMPILERFEATURES_SUPPORT_STATIC_ASSERT
# define BSLS_COMPILERFEATURES_SUPPORT_TRAITS_HEADER

# if BSLS_PLATFORM_CMP_VERSION >= 1900  // Microsoft Visual Studio 2015
// Note that while MSVC 2013 supports variadic templates in principle, there
// are sufficient problems with the implementation that we defer support until
// the 2015 compiler where those issues are ironed out.  Note that while MSVC
// 2013 supports deleted functions in principle, the only use we had caused a
// C1001 compiler internal error.  Also note that the variable template C++14
// compiler feature is supported since the 2015 update 2 compiler.
#   define BSLS_COMPILERFEATURES_SUPPORT_ALIGNAS
#   define BSLS_COMPILERFEATURES_SUPPORT_DELETED_FUNCTIONS
#   define BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT
#   define BSLS_COMPILERFEATURES_SUPPORT_INLINE_NAMESPACE
#   define BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS
#   define BSLS_COMPILERFEATURES_SUPPORT_UNICODE_CHAR_TYPES
#   define BSLS_COMPILERFEATURES_SUPPORT_VARIABLE_TEMPLATES
#   define BSLS_COMPILERFEATURES_SUPPORT_VARIADIC_TEMPLATES
# endif

// Not only does Microsoft not always report the language dialect properly in
// '__cplusplus', many non-BDE users depend upon it not reflecting the right
// version, therefore we need to use the Microsoft specific predefined macro
// '_MSVC_LANG'.  See https://goo.gl/ikfyDw and
// https://docs.microsoft.com/en-us/cpp/preprocessor/predefined-macros
// Note that '_MSVC_LANG' is only useful for distinguishing MSVC 2017 from MSVC
// 2014.

# if BSLS_PLATFORM_CMP_VERSION >= 1910  // Microsoft Visual Studio 2017
#   define BSLS_COMPILERFEATURES_SUPPORT_CONSTEXPR
#   define BSLS_COMPILERFEATURES_SUPPORT_CONSTEXPR_CPP14
#   if BSLS_COMPILERFEATURES_CPLUSPLUS >= 201703L
#     define BSLS_COMPILERFEATURES_SUPPORT_ATTRIBUTE_FALLTHROUGH
#     define BSLS_COMPILERFEATURES_SUPPORT_CONSTEXPR_CPP17
#   endif
# endif
// (not yet supported in MSVC)
//# define BSLS_COMPILERFEATURES_SUPPORT_INCLUDE_NEXT

// Not yet enabling C++17 support, but pro-active test drivers may want to add
// coverage.
// # define BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT_TYPES

# if BSLS_PLATFORM_CMP_VERSION >= 1911  // Microsoft Visual Studio 2017
                                        // version 15.3
#   define BSLS_COMPILERFEATURES_SUPPORT_HAS_INCLUDE
#   if BSLS_COMPILERFEATURES_CPLUSPLUS >= 201703L
#     define BSLS_COMPILERFEATURES_SUPPORT_ATTRIBUTE_MAYBE_UNUSED
#     define BSLS_COMPILERFEATURES_SUPPORT_ATTRIBUTE_NODISCARD
#   endif
# endif

# if BSLS_PLATFORM_CMP_VERSION >= 1912  // Microsoft Visual Studio 2017
                                        // version 15.5
#  if BSLS_COMPILERFEATURES_CPLUSPLUS >= 201703L
#   define BSLS_COMPILERFEATURES_SUPPORT_INLINE_VARIABLES
#   define BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT_TYPES
#  endif
# endif

// IBM Visual Age does not yet follow WG14 N2322 Recommended practice
// #define BSLS_COMPILERFEATURES_PP_LINE_IS_ON_FIRST
#endif

// IBM Visual Age xlC 11.1 and better include increasing support for C++11
// http://www-01.ibm.com/support/knowledgecenter/SSGH3R_13.1.0/com.ibm.xlcpp131.aix.doc/compiler_ref/macros_lang_levels.html
// http://www-01.ibm.com/support/knowledgecenter/SSGH3R_13.1.0/com.ibm.xlcpp131.aix.doc/compiler_ref/opt_langlvl.html
//  xlC_r -qlanglvl=extended    (include next, extern template, noreturn)
//  xlC_r -qlanglvl=extended0x  (additional features)
#if defined(BSLS_PLATFORM_CMP_IBM)
#if defined(__IBMCPP_CONSTEXPR)
#define BSLS_COMPILERFEATURES_SUPPORT_CONSTEXPR
#endif
#if defined(__IBMCPP_DECLTYPE)
#define BSLS_COMPILERFEATURES_SUPPORT_DECLTYPE
#endif
#if defined(__IBMCPP_DEFAULTED_AND_DELETED_FUNCTIONS)
#define BSLS_COMPILERFEATURES_SUPPORT_DEFAULTED_FUNCTIONS
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
#if defined(__IBMCPP_INLINE_NAMESPACE)
#define BSLS_COMPILERFEATURES_SUPPORT_INLINE_NAMESPACE
#endif
#if defined(__IBMCPP_NULLPTR)
#define BSLS_COMPILERFEATURES_SUPPORT_NULLPTR
#endif
#if defined(__IBMCPP_OVERRIDE)
#define BSLS_COMPILERFEATURES_SUPPORT_FINAL
#define BSLS_COMPILERFEATURES_SUPPORT_OVERRIDE
#endif
#if defined(__IBMCPP_RVALUE_REFERENCES)
#define BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS
#define BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
#endif
#if defined(__IBMCPP_STATIC_ASSERT)
#define BSLS_COMPILERFEATURES_SUPPORT_STATIC_ASSERT
#endif
#if defined(__IBMCPP_VARIADIC_TEMPLATES)
#define BSLS_COMPILERFEATURES_SUPPORT_VARIADIC_TEMPLATES
#endif
#if defined(__IBMC_NORETURN)
// xlC has '_Noreturn'
//#define BSLS_COMPILERFEATURES_SUPPORT_ATTRIBUTE_NORETURN
#endif
// (not yet supported in xlC)
//#define BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES
//#define BSLS_COMPILERFEATURES_SUPPORT_CONSTEXPR_CPP14
//#define BSLS_COMPILERFEATURES_SUPPORT_CONSTEXPR_CPP17
//#define BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT

// Not yet tested for support
// # define BSLS_COMPILERFEATURES_SUPPORT_RAW_STRINGS
// # define BSLS_COMPILERFEATURES_SUPPORT_VARIABLE_TEMPLATES

// Not yet enabling C++17 support, but pro-active test drivers may want to add
// coverage.
// # define BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT_TYPES
// # define BSLS_COMPILERFEATURES_SUPPORT_INLINE_VARIABLES
// # define BSLS_COMPILERFEATURES_SUPPORT_HAS_INCLUDE

// IBM Visual Age suite does not yet follow WG14 N2322 Recommended practice
// #define BSLS_COMPILERFEATURES_PP_LINE_IS_ON_FIRST
#endif


// Oracle Solaris Studio 12.4 claims C++11 support except for C++11 concurrency
// and atomic operations, and for user-defined literals
// http://docs.oracle.com/cd/E37069_01/html/E37071/gncix.html#scrolltoc
// No C++11 features are available by default.  To use any C++11 features, you
// must use the -std=c++11 option with the CC compiler.
// ('__cplusplus >= 201103L' when Oracle Solaris Studio 'CC -std=c++11' is
// invoked)
#if defined(BSLS_PLATFORM_CMP_SUN)

// CC -std=c++11
#if  __cplusplus >= 201103L
# if BSLS_PLATFORM_CMP_VERSION >= 0x5130
#   define BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES
#   define BSLS_COMPILERFEATURES_SUPPORT_DECLTYPE
#   define BSLS_COMPILERFEATURES_SUPPORT_DEFAULTED_FUNCTIONS
#   define BSLS_COMPILERFEATURES_SUPPORT_DELETED_FUNCTIONS
#   define BSLS_COMPILERFEATURES_SUPPORT_ENUM_CLASS
#   define BSLS_COMPILERFEATURES_SUPPORT_EXTERN_TEMPLATE
#   define BSLS_COMPILERFEATURES_SUPPORT_FINAL
#   define BSLS_COMPILERFEATURES_SUPPORT_INLINE_NAMESPACE
#   define BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT
#   define BSLS_COMPILERFEATURES_SUPPORT_NULLPTR
#   define BSLS_COMPILERFEATURES_SUPPORT_OPERATOR_EXPLICIT
#   define BSLS_COMPILERFEATURES_SUPPORT_OVERRIDE
#   define BSLS_COMPILERFEATURES_SUPPORT_RAW_STRINGS
#   define BSLS_COMPILERFEATURES_SUPPORT_STATIC_ASSERT
#   define BSLS_COMPILERFEATURES_SUPPORT_ATTRIBUTE_NORETURN
# endif

# if BSLS_PLATFORM_CMP_VERSION >= 0x5140
#   define BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS
    // CC 12.4 will overly aggressively match an initializer list when it sees
    // brace initalization, leading to rejection of valid code when there is no
    // valid initializer list conversion, but another method creating a
    // temporary object from the braced arguments should have been chosen.

#   define BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS
    // Although this feature might be available in an earlier compiler, we are
    // asking for trouble if we don't keep this feature together with rvalue
    // references.

#   define BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
    // CC 12.4 hits an awkward bug when performing deduction in some corner
    // cases, which happen to be important to our vector implementation.

#  define BSLS_COMPILERFEATURES_SUPPORT_TRAITS_HEADER 1
    // The previous compiler ships with a mostly-complete version of the
    // <type_traits> header, but we insist on a full implementation before
    // defining this macro.

#   define BSLS_COMPILERFEATURES_SUPPORT_VARIADIC_TEMPLATES
    // CC 12.4 has problems partially ordering template parameter packs that
    // typically result in failing to compile with ambiguity errors.
# endif

# if BSLS_PLATFORM_CMP_VERSION == 0x5150
#   undef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
    // CC 12.6 (beta) has a nasty bug with reference collapsing rvalue and
    // lvalue references that crashes the compiler.
#endif

# if BSLS_PLATFORM_CMP_VERSION > 0x5150
#   define BSLS_COMPILERFEATURES_SUPPORT_CONSTEXPR
    // CC 12.4 'constexpr' implementation almost satisfies our testing, but the
    // compiler crashes when for some rare-but-reasonable data structures.  CC
    // 12.5 has different corner cases, although we are still trying to track
    // down a narrowed test case.  Both 12.5 and 12.6 have a regresion compared
    // to 12.4 though with the (implicit) 'constexpr' constructors of
    // aggregates that have dependent base classes, such as most type traits.

#   define BSLS_COMPILERFEATURES_SUPPORT_DEFAULT_TEMPLATE_ARGS
    // CC 12.4, CC 12.5, and 12.6 all fail in a very specific way, which
    // unfortuntely breaks for 'shared_ptr' in a way that is widely used.  Note
    // that the version check assumes the next revision of the compiler will
    // have this fix, or the test driver will force us to update again.
# endif

# if BSLS_PLATFORM_CMP_VERSION < 0x5160
#   define BSLS_COMPILERFEATURES_INITIALIZER_LIST_LEAKS_ON_EXCEPTIONS 1
# endif

    // # define BSLS_COMPILERFEATURES_SUPPORT_CONSTEXPR_CPP14
    // # define BSLS_COMPILERFEATURES_SUPPORT_CONSTEXPR_CPP17
    // # define BSLS_COMPILERFEATURES_SUPPORT_INCLUDE_NEXT
#endif
// No C++14 features are available by default.  To use any C++14 features, you
// must use the -std=c++14 option with the CC compiler.
// ('__cplusplus >= 201402L' when Oracle Solaris Studio 'CC -std=c++14' is
// invoked)

//CC -std=c++14
#if  __cplusplus >= 201402L
# if BSLS_PLATFORM_CMP_VERSION > 0x5150
#   define BSLS_COMPILERFEATURES_SUPPORT_VARIABLE_TEMPLATES
# endif
#endif

// No C++17 features
// # define BSLS_COMPILERFEATURES_SUPPORT_HAS_INCLUDE

// Oracle Solaris Studio does not yet follow WG14 N2322 Recommended practice
// #define BSLS_COMPILERFEATURES_PP_LINE_IS_ON_FIRST
#endif

// ============================================================================
//              DISABLE FEATURES REMOVED BY LATER STANDARDS
// ============================================================================

// Undefine macros defined for earlier dialacts (including C++98) that are
// removed from later editions of the C++ Standard.

#if BSLS_COMPILERFEATURES_CPLUSPLUS >= 201703L
# undef BSLS_COMPILERFEATURES_SUPPORT_THROW_SPECIFICATIONS
#endif

// ============================================================================
//                      ENFORCE FEATURE INTERACTIONS
// ============================================================================

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) &&  \
   !defined(BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES)
#   undef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
    // BDE move-emulation for C++03 requires support for alias templates in
    // order to provide a transparent upgrade to the C++11 syntax that also
    // supports implicit-move from rvalues.
#endif

// ============================================================================
//                      SIMULATE VARIOUS C++11 FEATURES
// ============================================================================

#ifndef BSLS_COMPILERFEATURES_SUPPORT_VARIADIC_TEMPLATES
#   define BSLS_COMPILERFEATURES_SIMULATE_VARIADIC_TEMPLATES 1

#   ifndef BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
#       define BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES 1
#   endif

#   if BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
#       ifdef __cplusplus

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

}  // close package namespace
}  // close enterprise namespace

#       endif // __cplusplus
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
    // i.e., an argument that can be perfect-forwarded; otherwise, replace with
    // 'const T&', the classic way to accept arguments of unknown
    // rvalue/lvalue-ness.

#   define BSLS_COMPILERFEATURES_FORWARD(T,V) \
        ::BloombergLP::bslmf::Util::forward(V)
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
// Copyright 2020 Bloomberg Finance L.P.
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
