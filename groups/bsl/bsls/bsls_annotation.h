// bsls_annotation.h                                                  -*-C++-*-
#ifndef INCLUDED_BSLS_ANNOTATION
#define INCLUDED_BSLS_ANNOTATION

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide support for compiler annotations for compile-time safety.
//
//@CLASSES:
//
//@MACROS:
//  BSLS_ANNOTATION_ALLOC_SIZE(x): optimize when returning memory
//  BSLS_ANNOTATION_ALLOC_SIZE_MUL(x, y): optimize when returning memory
//  BSLS_ANNOTATION_ERROR("msg"): emit error message and fail compilation
//  BSLS_ANNOTATION_WARNING("msg"): emit warning message during compilation
//  BSLS_ANNOTATION_PRINTF(s, n): validate 'printf' format and arguments
//  BSLS_ANNOTATION_SCANF(s, n): validate 'scanf' format and arguments
//  BSLS_ANNOTATION_FORMAT(n): validate 'printf' format in 'n'th argument
//  BSLS_ANNOTATION_ARG_NON_NULL(...): warn if listed arguments are NULL
//  BSLS_ANNOTATION_ARGS_NON_NULL: warn if any arguments are NULL
//  BSLS_ANNOTATION_NULL_TERMINATED: warn if last argument is non-NULL
//  BSLS_ANNOTATION_NULL_TERMINATED_AT(x): warn if argument at 'x' is non-NULL
//  BSLS_ANNOTATION_WARN_UNUSED_RESULT: warn if annotated function result used
//  BSLS_ANNOTATION_DEPRECATED: warn if annotated entity is used
//  BSLS_ANNOTATION_USED: emit annotated entity even if not referenced
//  BSLS_ANNOTATION_UNUSED: do not warn if annotated entity is unused
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
///Function Annotations
///--------------------
// The following macros pertain to function declarations (only):
//
//..
//  BSLS_ANNOTATION_ALLOC_SIZE(x)
//  BSLS_ANNOTATION_ALLOC_SIZE_MUL(x, y)
//..
// This annotation is used to inform the compiler that the return value of the
// so-annotated function is the address of an allocated block of memory whose
// size (in bytes) is given by one or two of the function parameters.  Certain
// compilers use this information to improve the correctness of built-in
// object-size functions (e.g., '__builtin_object_size' with
// 'BSLS_PLATFORM_CMP_GNU' or 'BSLS_PLATFORM_CMP_CLANG').
//
// The function parameter(s) denoting the size of the block are specified by
// one or two integer arguments supplied to the macro.  The allocated size (in
// bytes) is either the value of the single function argument or the product of
// the two arguments.  Argument numbering starts at one.  For example:
//..
//  void *my_calloc(size_t, size_t) BSLS_ANNOTATION_ALLOC_SIZE_MUL(1, 2);
//  void my_realloc(void *, size_t) BSLS_ANNOTATION_ALLOC_SIZE(2);
//..
//
//..
//  BSLS_ANNOTATION_ERROR("message")
//  BSLS_ANNOTATION_WARNING("message")
//..
// This annotation, when used, will cause a compile-time error (or warning)
// when a call to the so-annotated function is not removed through dead-code
// elimination or other optimizations.  While it is possible to leave the
// function undefined, thus incurring a link-time failure, with the use of this
// macro the invalid call will be diagnosed earlier (i.e., at compile time),
// and the diagnostic will include the exact location of the function call.
//
//..
//  BSLS_ANNOTATION_PRINTF(stringIndex, firstToCheck)
//  BSLS_ANNOTATION_SCANF(stringIndex, firstToCheck)
//..
// These annotations perform additional compile-time checks on so-annotated
// functions that take 'printf'-style arguments, which should be type-checked
// against a format string.
//
// The 'stringIndex' parameter is the one-based index to the 'const' format
// string.  The 'firstToCheck' parameter is the one-based index to the first
// variable argument to type-check against that format string.  For example:
//..
//  extern int my_printf(void *obj, const char *format, ...)
//                                                BSLS_ANNOTATION_PRINTF(2, 3);
//..
//
//..
//  BSLS_ANNOTATION_FORMAT(stringIndex)
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
//                                                   BSLS_ANNOTATION_FORMAT(2);
//..
// On a conforming compiler, this will validate the "Mike" argument against the
// 'format' specification passed to 'translateFormat'.
//..
//   printf(translateFormat("FR", "Name: %s"), "Mike");
//..
//
//..
//  BSLS_ANNOTATION_ARG_NON_NULL(...)
//  BSLS_ANNOTATION_ARGS_NON_NULL
//..
// These annotations are used to tell the compiler that a function argument
// must not be null.  If the compiler determines that a null pointer is passed
// to an argument slot marked by this annotation, a warning is issued.  If the
// 'BSLS_ANNOTATION_ARG_NON_NULL' annotation is used, it expects a variable
// list of argument slots to be specified.  An argument slot is a one-based
// index of the argument in the function parameters.  The
// 'BSLS_ANNOTATION_ARGS_NON_NULL' annotation specifies that all pointer
// arguments must not be null.
//
//..
//  BSLS_ANNOTATION_NULL_TERMINATED
//  BSLS_ANNOTATION_NULL_TERMINATED_AT(x)
//..
// This annotation ensures that a parameter in a function call is an explicit
// 'NULL'.  The annotation is valid only on variadic functions.  By default,
// the sentinel is located at position 0, the last parameter of the function
// call.  If an optional position is specified, the sentinel must be located at
// that index, counting backwards from the end of the argument list.
//
//..
//  BSLS_ANNOTATION_WARN_UNUSED_RESULT
//..
// This annotation causes a warning to be emitted if the caller of a
// so-annotated function does not use its return value.  This is useful for
// functions where not checking the result is either a security problem or
// always a bug, such as with the 'realloc' function.
//
///Function, Variable, and Type Annotations
///----------------------------------------
// The following macros pertain to function, variable, and type declarations:
//..
//  BSLS_ANNOTATION_DEPRECATED
//..
// This annotation will, when used, cause a compile-time warning if the
// so-annotated function, variable, or type is used anywhere within the source
// file.  This is useful, for example, when identifying functions that are
// expected to be removed in a future version of a library.  The warning
// includes the location of the declaration of the deprecated entity to enable
// users to find further information about the deprecation, or what they should
// use instead.  For example:
//..
//  int oldFnc() BSLS_ANNOTATION_DEPRECATED;
//  int oldFnc();
//  int (*fncPtr)() = oldFnc;
//..
// In the above code, the third line results in a compiler warning.
//
//..
//  BSLS_ANNOTATION_USED
//..
// This annotation indicates that the so-annotated function, variable, or type
// must be emitted even if it appears that the variable is not referenced.
//
//..
//  BSLS_ANNOTATION_UNUSED
//..
// This annotation indicates that the so-annotated function, variable, or type
// is possibly unused and the compiler should not generate a warning for the
// unused identifier.
//
///Usage
///-----
// Function annotations must be specified after the function declaration, prior
// to the terminating semi-colon:
//..
//  void function() BSLS_ANNOTATION_ABC BSLS_ANNOTATION_XYZ;
//..
// Annotations cannot be specified on function definitions, only on
// declarations.
//
// Variable annotations must be specified after the variable declaration, prior
// to the terminating semi-colon:
//..
//  int foo BSLS_ANNOTATION_ABC BSLS_ANNOTATION_XYZ;
//..

#ifndef INCLUDED_BSLS_PLATFORM
#include <bsls_platform.h>
#endif

#if defined(BSLS_PLATFORM_CMP_GNU) || defined(BSLS_PLATFORM_CMP_CLANG)
    #define BSLS_ANNOTATION_USED       __attribute__((__used__))
    #define BSLS_ANNOTATION_UNUSED     __attribute__((__unused__))
    #define BSLS_ANNOTATION_ERROR(x)   __attribute__((__error__(x)))
    #define BSLS_ANNOTATION_WARNING(x) __attribute__((__warning__(x)))
#else
    #define BSLS_ANNOTATION_USED
    #define BSLS_ANNOTATION_UNUSED
    #define BSLS_ANNOTATION_ERROR(x)
    #define BSLS_ANNOTATION_WARNING(x)
#endif

#if (defined(BSLS_PLATFORM_CMP_GNU) && BSLS_PLATFORM_CMP_VER_MAJOR >= 40300) || \
    defined(BSLS_PLATFORM_CMP_CLANG)
    #define BSLS_ANNOTATION_ALLOC_SIZE(x) __attribute__((__alloc_size__(x)))
    #define BSLS_ANNOTATION_ALLOC_SIZE_MUL(x, y) \
                                          __attribute__((__alloc_size__(x, y)))
#else
    #define BSLS_ANNOTATION_ALLOC_SIZE(x)
    #define BSLS_ANNOTATION_ALLOC_SIZE_MUL(x, y)
#endif

#if defined(BSLS_PLATFORM_CMP_GNU) || defined(BSLS_PLATFORM_CMP_CLANG)
    #define BSLS_ANNOTATION_ARG_NON_NULL(...) \
                                      __attribute__((__nonnull__(__VA_ARGS__)))
    #define BSLS_ANNOTATION_ARGS_NON_NULL     \
                                      __attribute__((__nonnull__))
#else
    #define BSLS_ANNOTATION_ARG_NON_NULL(...)
    #define BSLS_ANNOTATION_ARGS_NON_NULL
#endif

#if defined(BSLS_PLATFORM_CMP_GNU) || defined(BSLS_PLATFORM_CMP_CLANG)
    #define BSLS_ANNOTATION_DEPRECATED __attribute__((__deprecated__))
#else
    #define BSLS_ANNOTATION_DEPRECATED
#endif

#if defined(BSLS_PLATFORM_CMP_GNU)   || \
    defined(BSLS_PLATFORM_CMP_CLANG) || \
    defined(BSLS_PLATFORM_CMP_IBM)
    #define BSLS_ANNOTATION_FORMAT(arg) __attribute__((format_arg(arg)))
#else
    #define BSLS_ANNOTATION_FORMAT(arg)
#endif

#if defined(BSLS_PLATFORM_CMP_GNU) || defined(BSLS_PLATFORM_CMP_CLANG)
    #define BSLS_ANNOTATION_NULL_TERMINATED __attribute__((__sentinel__))
    #define BSLS_ANNOTATION_NULL_TERMINATED_AT(x) \
                                            __attribute__((__sentinel__(x)))
#else
    #define BSLS_ANNOTATION_NULL_TERMINATED
    #define BSLS_ANNOTATION_NULL_TERMINATED_AT(x)
#endif

#if defined(BSLS_PLATFORM_CMP_GNU)   || \
    defined(BSLS_PLATFORM_CMP_CLANG) || \
    defined(BSLS_PLATFORM_CMP_HP)    || \
    defined(BSLS_PLATFORM_CMP_IBM)
    #define BSLS_ANNOTATION_PRINTF(fmt, arg) \
                                      __attribute__((format(printf, fmt, arg)))
    #define BSLS_ANNOTATION_SCANF(fmt, arg) \
                                      __attribute__((format(scanf,  fmt, arg)))
#else
    #define BSLS_ANNOTATION_PRINTF(fmt, arg)
    #define BSLS_ANNOTATION_SCANF(fmt, arg)
#endif

#if defined(BSLS_PLATFORM_CMP_GNU) || defined(BSLS_PLATFORM_CMP_CLANG)
    #define BSLS_ANNOTATION_WARN_UNUSED_RESULT \
                                            __attribute__((warn_unused_result))
#else
    #define BSLS_ANNOTATION_WARN_UNUSED_RESULT
#endif

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
