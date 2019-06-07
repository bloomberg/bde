// bsla_annotations.h                                                 -*-C++-*-
#ifndef INCLUDED_BSLA_ANNOTATIONS
#define INCLUDED_BSLA_ANNOTATIONS

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide support for compiler annotations for compile-time safety.
//
//@MACROS:
//  BSLA_DEPRECATED: warn if annotated (deprecated) entity is used
//  BSLA_ERROR(QUOTED_MESSAGE): emit error message and fail compilation
//  BSLA_FALLTHROUGH: do not warn if 'switch' 'case' falls through
//  BSLA_FORMAT(FMT_IDX): validate 'printf'-style format spec in 'n'th argument
//  BSLA_NODISCARD: warn if annotated function result is not used
//  BSLA_NONNULLARG(...): warn if indexed arguments are NULL
//  BSLA_NONNULLARGS: warn if any arguments are NULL
//  BSLA_NORETURN: issue a compiler warning if function returns normally
//  BSLA_NULLTERMINATED:            warn if last argument is non-NULL
//  BSLA_NULLTERMINATEDAT(ARG_IDX): warn if argument at 'ARG_IDX' is non-NULL
//  BSLA_PRINTF(FMTIDX, STARTIDX): validate 'printf' format and arguments
//  BSLA_SCANF(FMTIDX, STARTIDX): validate 'scanf' format and arguments
//  BSLA_UNUSED: do not warn if annotated entity is unused
//  BSLA_USED: emit annotated entity even if not referenced
//  BSLA_WARNING(QUOTED_MESSAGE): emit warning message during compilation
//
//  BSLA_DEPRECATED_IS_ACTIVE: 1 if 'BSLA_DEPRECATED is active, 0 otherwise
//  BSLA_ERROR_IS_ACTIVE: 1 if 'BSLA_ERROR' is active and 0 otherwise
//  BSLA_FALLTHROUGH_IS_ACTIVE: 1 if 'BSLA_FALLTHROUGH' is active, else 0
//  BSLA_FORMAT_IS_ACTIVE: 1 if 'BSLA_FORMAT' is active and 0 otherwise
//  BSLA_NODISCARD_IS_ACTIVE: 1 if 'BSLA_NODISCARD' is active and 0 otherwise
//  BSLA_NONNULLARG_IS_ACTIVE:  1 if 'BSLA_NONNULLARG' is active, 0 otherwise
//  BSLA_NONNULLARGS_IS_ACTIVE: 1 if 'BSLA_NONNULLARGS' is active, 0 otherwise
//  BSLA_NORETURN_IS_ACTIVE: 1 if 'BSLA_NORETURN' is active and 0 otherwise
//  BSLA_NULLTERMINATED_IS_ACTIVE:   1 if BSLA_NULLTERMINATED is active
//  BSLA_NULLTERMINATEDAT_IS_ACTIVE: 1 if BSLA_NULLTERMINATEDAT is active
//  BSLA_PRINTF_IS_ACTIVE: 1 if 'BSLA_PRINTF' is active and 0 otherwise
//  BSLA_SCANF_IS_ACTIVE: 0 if 'BSLA_SCANF' expands to nothing and 1 otherwise
//  BSLA_UNUSED_IS_ACTIVE: 0 if 'BSLA_UNUSED' expands to nothing, else 1
//  BSLA_USED_IS_ACTIVE: 0 if 'BSLA_USED' expands to nothing and 1 otherwise
//  BSLA_WARNING_IS_ACTIVE: 0 if 'BSLA_WARNING' expands to nothing else 1
//
//@AUTHOR: Andrew Paprocki (apaprock), Bill Chapman (bchapman2)
//
//@DESCRIPTION: This component exists to provide a single component whose
// header can be included to transitively include all of the annotation macros
// defined in the 'bsla' package.  The macros that are transitively included by
// this component correspond to various compiler features, and can be used to
// annotate code for specific compile-time safety checks.
//
// For the most part, these compile-time annotations are supported only when
// the 'BSLS_PLATFORM_CMP_GNU' or 'BSLS_PLATFORM_CMP_CLANG' preprocessor macros
// are defined.  Other compilers may implement a few annotations, but the
// macros should be expected to work only with compilers for which
// 'BSLS_PLATFORM_CMP_GNU' or 'BSLS_PLATFORM_CMP_CLANG' is defined.
//
///Macro Reference
///---------------
//: 'BSLA_DEPRECATED'
//:     This annotation will, when used, cause a compile-time warning if the
//:     so-annotated function, variable, or type is used anywhere within the
//:     source file.  This is useful, for example, when identifying functions
//:     that are expected to be removed in a future version of a library.  The
//:     warning includes the location of the declaration of the deprecated
//:     entity to enable users to find further information about the
//:     deprecation, or what they should use instead.
//
//: 'BSLA_ERROR(QUOTED_MESSAGE)'
//:     This annotation, when used, will cause a compile-time error when a call
//:     to the so-annotated function is not removed through dead-code
//:     elimination or other optimizations.  While it is possible to leave the
//:     function undefined, thus incurring a link-time failure, with the use of
//:     this macro the invalid call will be diagnosed earlier (i.e., at compile
//:     time), and the diagnostic will include the location of the function
//:     call.  The message 'QUOTED_MESSAGE', which should be a double-quoted
//:     string, will appear in the error message.
//
//: 'BSLA_FALLTHROUGH'
//:     This annotation should be placed in a 'case' clause as the last
//:     statement within a flow of control that is expected to allow control to
//:     fall through instead of ending with a 'break', 'continue', or 'return'.
//:     This will prevent compilers from warning about fall-through.  The
//:     'BSLA_FALLTHROUGH' must be followed by a semicolon and may be nested
//:     within blocks, 'if's, or 'else's.
//
//: 'BSLA_FORMAT(FMT_IDX)'
//:     This annotation specifies that the so-annotated function takes an
//:     argument that is a valid format string for a 'printf'-style function
//:     and returns a format string that is consistent with that format.  This
//:     allows format strings manipulated by translation functions to be
//:     checked against arguments.  Without this annotation, attempting to
//:     manipulate the format string via this kind of function might generate
//:     warnings about non-literal formats, or fail to generate warnings about
//:     mismatched arguments.
//
//: 'BSLA_NODISCARD'
//:     This annotation causes a warning to be emitted if the caller of a
//:     so-annotated function does not use its return value.  This is useful
//:     for functions where not checking the result is either a security
//:     problem or always a bug, such as with the 'realloc' function.
//
//: 'BSLA_NONNULLARGS'
//:     This annotation indicates that a compiler warning is to be issued if
//:     any of the pointer arguments to this function are passed null.
//
//: 'BSLA_NONNULLARG(...)'
//:     This annotation, passed a variable-length list of positive integers,
//:     indicates that a compiler warning is to be issued if null is passed to
//:     a pointer argument at any of the specified indices, where the first
//:     argument of the annotated function has an index of 1.
//
//: 'BSLA_NORETURN'
//:     This annotation is used to tell the compiler that a specified function
//:     will not return in a normal fashion.  The function can still exit via
//:     other means such as throwing an exception or aborting the process.
//
//: 'BSLA_NULLTERMINATED'
//:     This annotation on a variadic macro indicates that a warning should be
//:     issued unless the last argument to the function is explicitly 'NULL'.
//
//: 'BSLA_NULLTERMINATEDAT(ARG_IDX)'
//:     This annotation on a variadic function indicates that a warning should
//:     be issued unless the argument at 'ARG_IDX' is 'NULL', where 'ARG_IDX'
//:     is the number of arguments from the last, the last argument having
//:     'ARG_IDX == 0'.  Thus, 'BSLA_NULLTERMINATED' is equivalent to
//:     'BSLA_NULLTERMINATEDAT(0)'.
//
//: 'BSLA_PRINTF(FMTIDX, STARTIDX)'
//:     This annotation instructs the compiler to perform additional
//:     compile-time checks on so-annotated functions that take 'printf'-style
//:     arguments, which should be type-checked against a format string.  The
//:     'FMTIDX' parameter is the one-based index to the 'const char *' format
//:     string.  The 'STARTIDX' parameter is the one-based index to the first
//:     variable argument to type-check against that format string.  For
//:     example:
//..
//  extern int my_printf(void *obj, const char *format, ...) BSLA_PRINTF(2, 3);
//..
//
//: 'BSLA_SCANF(FMTIDX, STARTIDX)'
//:     This annotation instructs the compiler to perform additional checks on
//:     so-annotated functions that take 'scanf'-style arguments, which should
//:     be type-checked against a format string.
//:
//:     The 'FMTIDX' parameter is the one-based index to the 'const' format
//:     string.  The 'STARTIDX' parameter is the one-based index to the first
//:     variable argument to type-check against that format string.  For
//:     example:
//..
//  extern int my_scanf(void *obj, const char *format, ...) BSLA_SCANF(2, 3);
//..
//
//: 'BSLA_UNREACHABLE'
//:     This annotation will, when used, indicate that a given statement in a
//:     code path is intended to be unreachable, causing compilers, where
//:     supported, to issue warnings if there is actually a way that the code
//:     can be reached.  Note that the behavior is undefined if control
//:     actually reaches a 'BSLA_UNREACHABLE' statement.
//
//: 'BSLA_UNUSED'
//:     This annotation indicates that the so-annotated function, variable, or
//:     type is possibly unused and the compiler should not generate a warning
//:     for the unused identifier.
//
//: 'BSLA_USED'
//:     This annotation indicates that the so-annotated function, variable, or
//:     type must be emitted even if it appears that the variable is not
//:     referenced.
//
//: 'BSLA_WARNING(QUOTED_MESSAGE)'
//:     This annotation, when used, will cause a compile-time warning
//:     containing the specified 'QUOTED_MESSAGE', which must be a string
//:     contained in double quotes, when a call to the so-annotated function is
//:     not removed through dead-code elimination or other optimizations.
//:     While it is possible to leave the function undefined, thus incurring a
//:     link-time failure, with the use of this macro the invalid call will be
//:     diagnosed earlier (i.e., at compile time), and the diagnostic will
//:     include the location of the function call.
//
//: 'BSLA_DEPRECATED_IS_ACTIVE'
//: 'BSLA_ERROR_IS_ACTIVE'
//: 'BSLA_FALLTHROUGH_IS_ACTIVE'
//: 'BSLA_FORMAT_IS_ACTIVE'
//: 'BSLA_NODISCARD_IS_ACTIVE'
//: 'BSLA_NONNULLARG_IS_ACTIVE'
//: 'BSLS_NONNULLARGS_IS_ACTIVE'
//: 'BSLA_NORETURN_IS_ACTIVE'
//: 'BSLA_NULLTERMINATED_IS_ACTIVE'
//: 'BSLA_NULLTERMINATEDAT_IS_ACTIVE'
//: 'BSLA_PRINTF_IS_ACTIVE'
//: 'BSLA_SCANF_IS_ACTIVE'
//: 'BSLA_UNREACHABLE_IS_ACTIVE'
//: 'BSLA_UNUSED_IS_ACTIVE'
//: 'BSLA_USED_IS_ACTIVE'
//: 'BSLA_WARNING_IS_ACTIVE'
//:     In all cases where there is a 'BSLA_{X}' macro, there is a macro
//:     'BSLA_{X}_IS_ACTIVE' that is defined to 0 if 'BSLA_{X}' expands to
//:     nothing and defined to 1 if 'BSLA_{X}' expands to something and works.
//
///Usage
///-----
// See the other 'bsla' components for usage examples for the macros offered
// (transitively) by this component.

#include <bsla_deprecated.h>
#include <bsla_error.h>
#include <bsla_fallthrough.h>
#include <bsla_format.h>
#include <bsla_nodiscard.h>
#include <bsla_nonnullarg.h>
#include <bsla_noreturn.h>
#include <bsla_nullterminated.h>
#include <bsla_printf.h>
#include <bsla_scanf.h>
#include <bsla_unreachable.h>
#include <bsla_unused.h>
#include <bsla_used.h>
#include <bsla_warning.h>

// BDE_VERIFY pragma: re-export <bsla_deprecated.h>
// BDE_VERIFY pragma: re-export <bsla_error.h>
// BDE_VERIFY pragma: re-export <bsla_fallthrough.h>
// BDE_VERIFY pragma: re-export <bsla_format.h>
// BDE_VERIFY pragma: re-export <bsla_nodiscard.h>
// BDE_VERIFY pragma: re-export <bsla_nonnullarg.h>
// BDE_VERIFY pragma: re-export <bsla_noreturn.h>
// BDE_VERIFY pragma: re-export <bsla_nullterminated.h>
// BDE_VERIFY pragma: re-export <bsla_printf.h>
// BDE_VERIFY pragma: re-export <bsla_scanf.h>
// BDE_VERIFY pragma: re-export <bsla_unreachable.h>
// BDE_VERIFY pragma: re-export <bsla_unused.h>
// BDE_VERIFY pragma: re-export <bsla_used.h>
// BDE_VERIFY pragma: re-export <bsla_warning.h>

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
