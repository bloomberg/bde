// bslfmt_print_imp.h                                                 -*-C++-*-
#ifndef INCLUDED_BSLFMT_PRINT_IMP
#define INCLUDED_BSLFMT_PRINT_IMP

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a standard compliant `print(FILE)` implementation.
//
//@CLASSES:
//
//@CANONICAL_HEADER: bsl_print.h
//
//@SEE_ALSO: ISO C++ Standard, <print>
//
//@DESCRIPTION: This component provides, in the `BloombergLP::bslfmt`
// namespace, functions exposed by the standard <print> header.  These are
// available for C++03 and later.
//
// There are 2 high-level sets of functions, that most users should use:
//
// * `print`
// * `println` (incl. no-args version)
//
// The only difference is that `println` always appends '\n' to the formatted
// output.  The no-arguments version of `println` just writes '\n'.
//
// Also there are 4 variants of low-level `vprint` function:
//
// * `vprint_nonunicode`
// * `vprint_nonunicode_buffered`
// * `vprint_unicode`
// * `vprint_unicode_buffered`
//
// The `_buffered` flavour of functions formats the result into a temporary
// string and then writes the result to the stream.  The functions without the
// `_buffered` suffix permit potentially more efficient implementation that
// involves explicit stream locking and writing directly to it, but it can
// cause deadlocks when used inappropriately.  The current implementation
// always uses an intermediate buffer for formatting, despite the presence of
// the `_buffered` suffix.
//
// The `_unicode` set of functions is used by `print` and `println` if the
// ordinary literal encoding is UTF-8; otherwise the `_nonunicode` functions
// are used.  Note that for most non-Windows target platforms, there is no
// implementation difference between the 2 sets.

#include <bslscm_version.h>

#include <bslfmt_format.h>

#include <bslstl_string.h>
#include <bslstl_stringview.h>

#include <cstdio>

#if BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
// clang-format off
// Include version that can be compiled with C++03
// Generated on Wed Nov 19 07:03:37 2025
// Command line: sim_cpp11_features.pl bslfmt_print_imp.h

# define COMPILING_BSLFMT_PRINT_IMP_H
# include <bslfmt_print_imp_cpp03.h>
# undef COMPILING_BSLFMT_PRINT_IMP_H

// clang-format on
#else

#if defined(BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES) &&                 \
    defined(BSLS_COMPILERFEATURES_SUPPORT_VARIADIC_TEMPLATES)
# define BSLFMT_FORMAT_STRING_PARAMETER bsl::format_string<t_ARGS...>
#else
// We cannot define format_string<t_ARGS...> in a C++03 compliant manner, so
// have to use non-template versions instead.
# define BSLFMT_FORMAT_STRING_PARAMETER bsl::format_string
#endif

namespace BloombergLP {
namespace bslfmt {

                              // --------------
                              // FREE FUNCTIONS
                              // --------------

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
/// Format the specified `args` using `bsl::format_to` according to the
/// specification given by the specified `fmt`, and write the result of this
/// operation to `stdout`.  In the event of formatting error throw the
/// exception `format_error`, in the event of I/O error throw the exception
/// `system_error`, on allocation failure throw `bad_alloc`.
template <class... t_ARGS>
void print(BSLFMT_FORMAT_STRING_PARAMETER fmt, t_ARGS&&... args);

/// Format the specified `args` using `bsl::format_to` according to the
/// specification given by the specified `fmt`, and write the result of this
/// operation to the specified `stream`.  In the event of formatting error
/// throw the exception `format_error`, in the event of I/O error throw the
/// exception `system_error`, on allocation failure throw `bad_alloc`.  The
/// behavior is undefined unless `stream` is a valid pointer to an output C
/// stream.
template <class... t_ARGS>
void print(std::FILE                      *stream,
           BSLFMT_FORMAT_STRING_PARAMETER  fmt,
           t_ARGS&&...                     args);

/// Format the specified `args` using `bsl::format_to` according to the
/// specification given by the specified `fmt`, append '\n', and write the
/// result of this operation to `stdout`.  In the event of formatting error
/// throw the exception `format_error`, in the event of I/O error throw the
/// exception `system_error`, on allocation failure throw `bad_alloc`.
template <class... t_ARGS>
void println(BSLFMT_FORMAT_STRING_PARAMETER fmt, t_ARGS&&... args);

/// Format the specified `args` using `bsl::format_to` according to the
/// specification given by the specified `fmt`, append '\n', and write the
/// result of this operation to the specified `stream`.  In the event of
/// formatting error throw the exception `format_error`, in the event of I/O
/// error throw the exception `system_error`, on allocation failure throw
/// `bad_alloc`.  The behavior is undefined unless `stream` is a valid pointer
/// to an output C stream.
template <class... t_ARGS>
void println(std::FILE                      *stream,
             BSLFMT_FORMAT_STRING_PARAMETER  fmt,
             t_ARGS&&...                     args);
#endif

/// Write '\n' into `stdout`.
void println();

/// Write '\n' to the specified `stream`.  The behavior is undefined unless
/// `stream` is a valid pointer to an output C stream.
void println(std::FILE *stream);

/// Format the specified `args` using `bsl::vformat_to` according to the
/// specification given by the specified `fmt`, and write the result of this
/// operation to `stdout`.  In the event of formatting error throw the
/// exception `format_error`, in the event of I/O error throw the exception
/// `system_error`, on allocation failure throw `bad_alloc`.
void vprint_nonunicode(bsl::string_view fmt, bsl::format_args args);

/// Format the specified `args` using `bsl::vformat_to` according to the
/// specification given by the specified `fmt`, and write the result of this
/// operation to the specified `stream`.  In the event of formatting error
/// throw the exception `format_error`, in the event of I/O error throw the
/// exception `system_error`, on allocation failure throw `bad_alloc`.  The
/// behavior is undefined unless `stream` is a valid pointer to an output C
/// stream.
void vprint_nonunicode(std::FILE        *stream,
                       bsl::string_view  fmt,
                       bsl::format_args  args);

/// Format the specified `args` using `bsl::vformat_to` according to the
/// specification given by the specified `fmt` to a temporary buffer, and write
/// the resulting buffer to the specified `stream`.  In the event of formatting
/// error throw the exception `format_error`, in the event of I/O error throw
/// the exception `system_error`, on allocation failure throw `bad_alloc`.  The
/// behavior is undefined unless `stream` is a valid pointer to an output C
/// stream.
void vprint_nonunicode_buffered(std::FILE        *stream,
                                bsl::string_view  fmt,
                                bsl::format_args  args);

/// Format the specified `args` using `bsl::vformat_to` according to the
/// specification given by the specified `fmt`, and write the result of this
/// operation to `stdout`.  In the event of formatting error throw the
/// exception `format_error`, in the event of I/O error throw the exception
/// `system_error`, on allocation failure throw `bad_alloc`.
void vprint_unicode(bsl::string_view fmt, bsl::format_args args);

/// Format the specified `args` using `bsl::vformat_to` according to the
/// specification given by the specified `fmt`, and write the result of this
/// operation to the specified `stream`.  In the event of formatting error
/// throw the exception `format_error`, in the event of I/O error throw the
/// exception `system_error`, on allocation failure throw `bad_alloc`.  The
/// behavior is undefined unless `stream` is a valid pointer to an output C
/// stream.
void vprint_unicode(std::FILE        *stream,
                    bsl::string_view  fmt,
                    bsl::format_args args);

/// Format the specified `args` using `bsl::vformat_to` according to the
/// specification given by the specified `fmt` to a temporary buffer, and write
/// the resulting buffer to the specified `stream`.  In the event of formatting
/// error throw the exception `format_error`, in the event of I/O error throw
/// the exception `system_error`, on allocation failure throw `bad_alloc`.  The
/// behavior is undefined unless `stream` is a valid pointer to an output C
/// stream.
void vprint_unicode_buffered(std::FILE        *stream,
                             bsl::string_view  fmt,
                             bsl::format_args  args);

/// Throw `system_error` with `errc::io_error`.
void Print_Imp_ThrowIoError();

// ============================================================================
//                              INLINE DEFINITIONS
// ============================================================================

                              // --------------
                              // FREE FUNCTIONS
                              // --------------

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
template <class... t_ARGS>
inline
void print(std::FILE                      *stream,
           BSLFMT_FORMAT_STRING_PARAMETER  fmt,
           t_ARGS&&...                     args)
{
    bslfmt::vprint_nonunicode_buffered(stream,
                                       fmt.get(),
                                       bsl::make_format_args(args...));
}

template <class... t_ARGS>
inline
void print(BSLFMT_FORMAT_STRING_PARAMETER fmt, t_ARGS&&... args)
{
    bslfmt::print(stdout, fmt, std::forward<t_ARGS>(args)...);
}

template <class... t_ARGS>
inline
void println(std::FILE                      *stream,
             BSLFMT_FORMAT_STRING_PARAMETER  fmt,
             t_ARGS&&...                     args)
{
    bslfmt::print(stream,
                  "{}\n",
                  bsl::vformat(fmt.get(), bsl::make_format_args(args...)));
}

template <class... t_ARGS>
inline
void println(BSLFMT_FORMAT_STRING_PARAMETER fmt, t_ARGS&&... args)
{
    bslfmt::println(stdout, fmt, std::forward<t_ARGS>(args)...);
}
#endif

inline
void println()
{
    bslfmt::println(stdout);
}

inline
void vprint_nonunicode(bsl::string_view fmt, bsl::format_args args)
{
    return bslfmt::vprint_nonunicode(stdout, fmt, args);
}

inline
void vprint_unicode(bsl::string_view fmt, bsl::format_args args)
{
    return bslfmt::vprint_unicode(stdout, fmt, args);
}

}  // close package namespace
}  // close enterprise namespace

#undef BSLFMT_FORMAT_STRING_PARAMETER

#endif // End C++11 code

#endif

// ----------------------------------------------------------------------------
// Copyright 2025 Bloomberg Finance L.P.
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
