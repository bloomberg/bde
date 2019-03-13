// bsla_annotations.h                                                 -*-C++-*-
#ifndef INCLUDED_BSLA_ANNOTATIONS
#define INCLUDED_BSLA_ANNOTATIONS

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide support for compiler annotations for compile-time safety.
//
//@CLASSES:
//
//@MACROS:
//  BSLA_ALLOC_SIZE(x): optimize when returning memory
//  BSLA_ALLOC_SIZE_MUL(x, y): optimize when returning memory
//  BSLA_ARG_NON_NULL(...): warn if listed arguments are NULL
//  BSLA_ARGS_NON_NULL: warn if any arguments are NULL
//  BSLA_DEPRECATED: warn if annotated entity is used
//  BSLA_ERROR("msg"): emit error message and fail compilation
//  BSLA_FALLTHROUGH: do not warn if case fall through
//  BSLA_FORMAT(n): validate 'printf' format in 'n'th argument
//  BSLA_NODISCARD: warn if annotated function result is not used
//  BSLA_NORETURN: error if function returns normally
//  BSLA_NULL_TERMINATED: warn if last argument is non-NULL
//  BSLA_NULL_TERMINATED_AT(x): warn if argument at 'x' is non-NULL
//  BSLA_PRINTF(s, n): validate 'printf' format and arguments
//  BSLA_SCANF(s, n): validate 'scanf' format and arguments
//  BSLA_USED: emit annotated entity even if not referenced
//  BSLA_UNUSED: do not warn if annotated entity is unused
//  BSLA_WARNING("msg"): emit warning message during compilation
//
//@AUTHOR: Andrew Paprocki (apaprock), Bill Chapman (bchapman2)
//
//@DESCRIPTION: This component provides a suite of preprocessor macros that
// define compiler-specific compile-time annotations.  These macros, which
// correspond to various compiler features, can be used to annotate code for
// specific compile-time safety checks.
//
// For the most part, these compile-time annotations are supported only when
// the 'BSLS_PLATFORM_CMP_GNU' or 'BSLS_PLATFORM_CMP_CLANG' preprocessor macro
// is defined.  Other compilers may implement a few annotations, but the macros
// should be expected to work only with compilers for which
// 'BSLS_PLATFORM_CMP_GNU' or 'BSLS_PLATFORM_CMP_CLANG' is defined.
//
///Annotations
///-----------
//..
//  BSLA_ALLOC_SIZE(argIndex)
//  BSLA_ALLOC_SIZE_MUL(argIndex1, argIndex2)
//..
// This annotation is used to inform the compiler that the return value of the
// so-annotated function is the address of an allocated block of memory whose
// size (in bytes) is given by one or two of the function parameters.  Certain
// compilers use this information to improve the correctness of built-in
// object-size functions (e.g., '__builtin_object_size' with
// 'BSLS_PLATFORM_CMP_GNU' or 'BSLS_PLATFORM_CMP_CLANG').
//
// The function parameter(s) denoting the size of the block are specified by
// one or two integer argument indeces supplied to the macro, where the first
// argument is denoted by '1'.  The allocated size (in bytes) is either the
// value of the single function argument or the product of the two arguments.
// Argument numbering starts at one.  For example:
//..
//  void *my_calloc(size_t a, size_t b) BSLA_ALLOC_SIZE_MUL(1, 2);
//      // The attribute indicates that the size of the allocated memory
//      // returned is 'a * b'.
//  void my_realloc(void * a, size_t b) BSLA_ALLOC_SIZE(2);
//      // The attribute indicates that the size of the allocated memory
//      // returned is 'b'.
//..
//
//..
//  BSLA_ARG_NON_NULL(...)
//  BSLA_ARGS_NON_NULL
//..
// These annotations are used to tell the compiler that a function argument
// must not be null.  If the compiler determines that a null pointer is passed
// to an argument slot marked by this annotation, a warning is issued.  If the
// 'BSLA_ARG_NON_NULL' annotation is used, it expects a variable list of
// argument indexes to be specified, where the first argument has an index of
// 1.  An argument slot is a one-based index of the argument in the function
// parameters.  The 'BSLA_ARGS_NON_NULL' annotation specifies that all pointer
// arguments must not be null.
//..
//  void my_function(void *a, void *b, void *c) BSLA_ARG_NON_NULL(2, 3);
//      // The attribute indicates that 'b' and 'c' must not be null.
//  void my_function(void *a, void *b, void *c) BSLA_ARGS_NON_NULL;
//      // The attribute indicates that 'a, 'b' and 'c' must not be null.
//..
//
//..
//  BSLA_DEPRECATED
//..
// This annotation will, when used, cause a compile-time warning if the
// so-annotated function, variable, or type is used anywhere within the source
// file.  This is useful, for example, when identifying functions that are
// expected to be removed in a future version of a library.  The warning
// includes the location of the declaration of the deprecated entity to enable
// users to find further information about the deprecation, or what they should
// use instead.  For example:
//..
//  int oldFnc() BSLA_DEPRECATED;
//  int oldFnc();
//  int (*fncPtr)() = oldFnc;
//..
// In the above code, the third line results in a compiler warning.
//
//..
//  BSLA_ERROR("message")
//..
// This annotation, when used, will cause a compile-time error when a call to
// the so-annotated function is not removed through dead-code elimination or
// other optimizations.  While it is possible to leave the function undefined,
// thus incurring a link-time failure, with the use of this macro the invalid
// call will be diagnosed earlier (i.e., at compile time), and the diagnostic
// will include the exact location of the function call.
//
//..
//  BSLA_FALLTHROUGH
//..
// This annotation should be placed as a the statement before a 'case' in a
// 'switch' statement that is expceted to allow control to fall through instead
// of ending with a 'break', 'continue', or 'return'.  This will prevent
// compilers from warning about fallthrough.
//
//..
//  BSLA_FORMAT(stringIndex)
//..
// This annotation specifies that the so-annotated function takes an argument
// parameter that is a valid format string for a 'printf'-style function and
// returns a format string that is consistent with that format.  This allows
// format strings manipulated by translation functions to be checked against
// arguments.  Without this annotation, attempting to manipulate the format
// string via this kind of function might generate warnings about non-literal
// formats.  For example:
//..
//  const char *translateFormat(const char *locale, const char *format)
//                                                   BSLA_FORMAT(2);
//..
// On a conforming compiler, this will validate the "Mike" argument against the
// 'format' specification passed to 'translateFormat'.
//..
//   printf(translateFormat("FR", "Name: %s"), "Mike");
//..
//
//..
//  BSLA_NODISCARD
//..
// This annotation causes a warning to be emitted if the caller of a
// so-annotated function does not use its return value.  This is useful for
// functions where not checking the result is either a security problem or
// always a bug, such as with the 'realloc' function.
//
//..
//  BSLA_NORETURN
//..
// This annotation is used to tell the compiler that a specified function will
// not return in a normal fashion.  The function can still exit via other means
// such as throwing an exception or aborting the process.
//
//..
//  BSLA_NULL_TERMINATED
//  BSLA_NULL_TERMINATED_AT(x)
//..
// This annotation ensures that a parameter in a function call is an explicit
// 'NULL'.  The annotation is valid only on variadic functions.  By default,
// the sentinel is located at position 0, the last parameter of the function
// call.  If an optional position is specified, the sentinel must be located at
// that index, counting backwards from the end of the argument list.
//
//..
//  BSLA_PRINTF(stringIndex, firstToCheck)
//..
// These annotations perform additional compile-time checks on so-annotated
// functions that take 'printf'-style arguments, which should be type-checked
// against a format string.
//
// The 'stringIndex' parameter is the one-based index to the 'const' format
// string.  The 'firstToCheck' parameter is the one-based index to the first
// variable argument to type-check against that format string.  For example:
//..
//  extern int my_printf(void *obj, const char *format, ...) BSLA_PRINTF(2, 3);
//..
//
//..
//  BSLA_SCANF(stringIndex, firstToCheck)
//..
// These annotations perform additional compile-time checks on so-annotated
// functions that take 'scantf'-style arguments, which should be type-checked
// against a format string.
//
// The 'stringIndex' parameter is the one-based index to the 'const' format
// string.  The 'firstToCheck' parameter is the one-based index to the first
// variable argument to type-check against that format string.  For example:
//..
//  extern int my_scantf(void *obj, const char *format, ...) BSLA_SCANF(2, 3);
//..
//
//..
//  BSLA_UNUSED
//..
// This annotation indicates that the so-annotated function, variable, or type
// is possibly unused and the compiler should not generate a warning for the
// unused identifier.
//
//..
//  BSLA_USED
//..
// This annotation indicates that the so-annotated function, variable, or type
// must be emitted even if it appears that the variable is not referenced.
//
//..
//  BSLA_WARNING("message")
//..
// This annotation, when used, will cause a compile-time ewarning when a call
// to the so-annotated function is not removed through dead-code elimination or
// other optimizations.  While it is possible to leave the function undefined,
// thus incurring a link-time failure, with the use of this macro the invalid
// call will be diagnosed earlier (i.e., at compile time), and the diagnostic
// will include the exact location of the function call.
//
///Usage
///-----
// Function annotations must be specified after the function declaration, prior
// to the terminating semi-colon:
//..
//  void function() BSLA_ABC BSLA_XYZ;
//..
// Annotations cannot be specified on function definitions, only on
// declarations.
//
// Variable annotations must be specified after the variable declaration, prior
// to the terminating semi-colon:
//..
//  int foo BSLA_ABC BSLA_XYZ;
//..

#include <bsls_platform.h>
#include <bsls_compilerfeatures.h>

#if (defined(BSLS_PLATFORM_CMP_GNU) && BSLS_PLATFORM_CMP_VERSION >= 40300) || \
    defined(BSLS_PLATFORM_CMP_CLANG)
    #define BSLA_ALLOC_SIZE(argIndex)                                         \
                          __attribute__((__alloc_size__(argIndex)))
    #define BSLA_ALLOC_SIZE_MUL(argIndex1, argIndex2)                         \
                          __attribute__((__alloc_size__(argIndex1, argIndex2)))

    #define BSLA_ALLOC_SIZE_IS_ACTIVE     1
    #define BSLA_ALLOC_SIZE_MUL_IS_ACTIVE 1
#else
    #define BSLA_ALLOC_SIZE(x)
    #define BSLA_ALLOC_SIZE_MUL(x, y)

    #define BSLA_ALLOC_SIZE_IS_ACTIVE     0
    #define BSLA_ALLOC_SIZE_MUL_IS_ACTIVE 0
#endif

#if defined(BSLS_PLATFORM_CMP_GNU) || defined(BSLS_PLATFORM_CMP_CLANG)
    #define BSLA_ARG_NON_NULL(...)   __attribute__((__nonnull__(__VA_ARGS__)))
    #define BSLA_ARGS_NON_NULL       __attribute__((__nonnull__))

    #define BSLA_ARG_NON_NULL_IS_ACTIVE  1
    #define BSLA_ARGS_NON_NULL_IS_ACTIVE 1
#else
    #define BSLA_ARG_NON_NULL(...)
    #define BSLA_ARGS_NON_NULL

    #define BSLA_ARG_NON_NULL_IS_ACTIVE  0
    #define BSLA_ARGS_NON_NULL_IS_ACTIVE 0
#endif

#if defined(BSLS_PLATFORM_CMP_GNU) || defined(BSLS_PLATFORM_CMP_CLANG)
    #define BSLA_DEPRECATED __attribute__((__deprecated__))

    #define BSLA_DEPRECATED_IS_ACTIVE 1
#else
    #define BSLA_DEPRECATED

    #define BSLA_DEPRECATED_IS_ACTIVE 0
#endif

#if defined(BSLS_PLATFORM_CMP_GNU)
    // '__error__' and '__warning__' attributes are not supported by clang as
    // of version 7.0.
    #define BSLA_ERROR(x)   __attribute__((__error__(x)))

    #define BSLA_ERROR_IS_ACTIVE 1
#else
    #define BSLA_ERROR(x)

    #define BSLA_ERROR_IS_ACTIVE 0
#endif

#ifdef BSLA_FALLTHROUGH
#error BSLA_FALLTHROUGH previously #defined
#endif
#if defined(BSLS_COMPILERFEATURES_SUPPORT_ATTRIBUTE_FALLTHROUGH)
    #define BSLA_FALLTHROUGH [[ fallthrough ]]
#elif defined(BSLS_PLATFORM_CMP_GNU) && BSLS_PLATFORM_CMP_VERSION >= 70000
    #define BSLA_FALLTHROUGH __attribute__((fallthrough))
#elif defined(BSLS_PLATFORM_CMP_CLANG)
    #if __cplusplus >= 201103L && defined(__has_warning)
        #if  __has_feature(cxx_attributes) && \
             __has_warning("-Wimplicit-fallthrough")
            #define BSLA_FALLTHROUGH [[clang::fallthrough]]
        #endif
    #endif
#endif
#ifdef BSLA_FALLTHROUGH
    #define BSLA_FALLTHROUGH_IS_ACTIVE 1
#else
    #define BSLA_FALLTHROUGH

    #define BSLA_FALLTHROUGH_IS_ACTIVE 0
#endif

#if defined(BSLS_PLATFORM_CMP_GNU)   ||                                      \
    defined(BSLS_PLATFORM_CMP_CLANG) ||                                      \
    defined(BSLS_PLATFORM_CMP_IBM)
    #define BSLA_FORMAT(arg) __attribute__((format_arg(arg)))

    #define BSLA_FORMAT_IS_ACTIVE 1
#else
    #define BSLA_FORMAT(arg)

    #define BSLA_FORMAT_IS_ACTIVE 0
#endif

#if defined(BSLS_COMPILERFEATURES_SUPPORT_ATTRIBUTE_NODISCARD)
    #define BSLA_NODISCARD [[ nodiscard ]]

    #define BSLA_NODISCARD_IS_ACTIVE 1
#elif defined(BSLS_PLATFORM_CMP_GNU) || defined(BSLS_PLATFORM_CMP_CLANG)
    #define BSLA_NODISCARD __attribute__((warn_unused_result))

    #define BSLA_NODISCARD_IS_ACTIVE 1
#else
    #define BSLA_NODISCARD

    #define BSLA_NODISCARD_IS_ACTIVE 0
#endif

#if defined(BSLS_COMPILERFEATURES_SUPPORT_ATTRIBUTE_NORETURN)
    #define BSLA_NORETURN [[ noreturn ]]

    #define BSLA_NORETURN 1
#elif defined(BSLS_PLATFORM_CMP_MSVC)
    #define BSLA_NORETURN __declspec(noreturn)

    #define BSLA_NORETURN_IS_ACTIVE 1
#else
    #define BSLA_NORETURN

    #define BSLA_NORETURN_IS_ACTIVE 0
#endif

#if defined(BSLS_PLATFORM_CMP_GNU) || defined(BSLS_PLATFORM_CMP_CLANG)
    #define BSLA_NULL_TERMINATED         __attribute__((__sentinel__))
    #define BSLA_NULL_TERMINATED_AT(x)   __attribute__((__sentinel__(x)))

    #define BSLA_NULL_TERMINATED_IS_ACTIVE    1
    #define BSLA_NULL_TERMINATED_AT_IS_ACTIVE 1
#else
    #define BSLA_NULL_TERMINATED
    #define BSLA_NULL_TERMINATED_AT(x)

    #define BSLA_NULL_TERMINATED_IS_ACTIVE    0
    #define BSLA_NULL_TERMINATED_AT_IS_ACTIVE 0
#endif

#if defined(BSLS_PLATFORM_CMP_GNU)   ||                                      \
    defined(BSLS_PLATFORM_CMP_CLANG) ||                                      \
    defined(BSLS_PLATFORM_CMP_HP)    ||                                      \
    defined(BSLS_PLATFORM_CMP_IBM)
    #define BSLA_PRINTF(fmt, arg) __attribute__((format(printf, fmt, arg)))

    #define BSLA_PRINTF_IS_ACTIVE 1
#else
    #define BSLA_PRINTF(fmt, arg)

    #define BSLA_PRINTF_IS_ACTIVE 0
#endif

#if defined(BSLS_PLATFORM_CMP_GNU)   ||                                      \
    defined(BSLS_PLATFORM_CMP_CLANG) ||                                      \
    defined(BSLS_PLATFORM_CMP_HP)    ||                                      \
    defined(BSLS_PLATFORM_CMP_IBM)
    #define BSLA_SCANF(fmt, arg) __attribute__((format(scanf,  fmt, arg)))

    #define BSLA_SCANF_IS_ACTIVE 1
#else
    #define BSLA_SCANF(fmt, arg)

    #define BSLA_SCANF_IS_ACTIVE 0
#endif

// Note that we could conceivably migrate this to use '[[maybe_unused]]' when
// available, but that has more specific constraints over where it can be
// syntactically placed than the older vendor annotations.

#if defined(BSLS_PLATFORM_CMP_GNU) || defined(BSLS_PLATFORM_CMP_CLANG)
    #define BSLA_UNUSED     __attribute__((__unused__))

    #define BSLA_UNUSED_IS_ACTIVE 1
#else
    #define BSLA_UNUSED

    #define BSLA_UNUSED_IS_ACTIVE 0
#endif

#if defined(BSLS_PLATFORM_CMP_GNU) || defined(BSLS_PLATFORM_CMP_CLANG)
    #define BSLA_USED       __attribute__((__used__))

    #define BSLA_USED_IS_ACTIVE 1
#else
    #define BSLA_USED

    #define BSLA_USED_IS_ACTIVE 0
#endif

#if defined(BSLS_PLATFORM_CMP_GNU)
    // '__error__' and '__warning__' attributes are not supported by clang as
    // of version 7.0.

    #define BSLA_WARNING(x) __attribute__((__warning__(x)))

    #define BSLA_WARNING_IS_ACTIVE 1
#else
    #define BSLA_WARNING(x)

    #define BSLA_WARNING_IS_ACTIVE 0
#endif

#endif

// ----------------------------------------------------------------------------
// Copyright 2019 Bloomberg Finance L.P.
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
