// bslfmt_print_ostream_imp.h                                         -*-C++-*-
#ifndef INCLUDED_BSLFMT_PRINT_OSTREAM_IMP
#define INCLUDED_BSLFMT_PRINT_OSTREAM_IMP

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a standard compliant `print(ostream)` implementation.
//
//@CLASSES:
//
//@CANONICAL_HEADER: bsl_ostream.h
//
//@SEE_ALSO: ISO C++ Standard, <ostream>, <print>
//
//@DESCRIPTION: This component provides, in the `BloombergLP::bslfmt`
// namespace, the `print` and related functions exposed by the standard
// <ostream> header.  These are available for C++03 and later.

#include <bslscm_version.h>

#include <bslfmt_format.h>

#include <bslstl_string.h>
#include <bslstl_stringview.h>

#include <ostream>

#if BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
// clang-format off
// Include version that can be compiled with C++03
// Generated on Wed Nov 19 07:03:37 2025
// Command line: sim_cpp11_features.pl bslfmt_print_ostream_imp.h

# define COMPILING_BSLFMT_PRINT_OSTREAM_IMP_H
# include <bslfmt_print_ostream_imp_cpp03.h>
# undef COMPILING_BSLFMT_PRINT_OSTREAM_IMP_H

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
/// operation to the specified `stream`.  In the event of formatting error
/// throw the exception `format_error`, in the event of I/O error throw the
/// exception `system_error`, on allocation failure throw `bad_alloc`.  The
/// behavior is undefined unless `stream` is a valid pointer to an output C
/// stream.
template <class... t_ARGS>
void print(std::ostream&                  stream,
           BSLFMT_FORMAT_STRING_PARAMETER fmt,
           t_ARGS&&...                    args);

/// Format the specified `args` using `bsl::format_to` according to the
/// specification given by the specified `fmt`, append '\n', and write the
/// result of this operation to the specified `stream`.  In the event of
/// formatting error throw the exception `format_error`, in the event of I/O
/// error throw the exception `system_error`, on allocation failure throw
/// `bad_alloc`.  The behavior is undefined unless `stream` is a valid pointer
/// to an output C stream.
template <class... t_ARGS>
void println(std::ostream&                  stream,
             BSLFMT_FORMAT_STRING_PARAMETER fmt,
             t_ARGS&&...                    args);
#endif

/// Write '\n' to the specified `stream`.  The behavior is undefined unless
/// `stream` is a valid pointer to an output C stream.
void println(std::ostream& stream);

/// Format the specified `args` using `bsl::vformat_to` according to the
/// specification given by the specified `fmt`, and write the result of this
/// operation to the specified `stream`.  In the event of formatting error
/// throw the exception `format_error`, in the event of I/O error throw the
/// exception `system_error`, on allocation failure throw `bad_alloc`.  The
/// behavior is undefined unless `stream` is a valid pointer to an output C
/// stream.
void vprint_nonunicode(std::ostream&    stream,
                       bsl::string_view fmt,
                       bsl::format_args args);

/// Format the specified `args` using `bsl::vformat_to` according to the
/// specification given by the specified `fmt`, and write the result of this
/// operation to the specified `stream`.  In the event of formatting error
/// throw the exception `format_error`, in the event of I/O error throw the
/// exception `system_error`, on allocation failure throw `bad_alloc`.  The
/// behavior is undefined unless `stream` is a valid pointer to an output C
/// stream.
void vprint_unicode(std::ostream&    stream,
                    bsl::string_view fmt,
                    bsl::format_args args);

// ============================================================================
//                              INLINE DEFINITIONS
// ============================================================================

                              // --------------
                              // FREE FUNCTIONS
                              // --------------

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
template <class... t_ARGS>
inline
void print(std::ostream&                  stream,
           BSLFMT_FORMAT_STRING_PARAMETER fmt,
           t_ARGS&&...                    args)
{
    bslfmt::vprint_nonunicode(stream,
                              fmt.get(),
                              bsl::make_format_args(args...));
}

template <class... t_ARGS>
inline
void println(std::ostream&                  stream,
             BSLFMT_FORMAT_STRING_PARAMETER fmt,
             t_ARGS&&...                    args)
{
    bslfmt::print(stream,
                  "{}\n",
                  bsl::vformat(fmt.get(), bsl::make_format_args(args...)));
}
#endif

inline
void println(std::ostream& stream)
{
    bslfmt::print(stream, "\n");
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
