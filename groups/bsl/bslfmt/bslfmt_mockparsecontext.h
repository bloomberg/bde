// bslfmt_mockparsecontext.h                                          -*-C++-*-

#ifndef INCLUDED_BSLFMT_MOCKPARSECONTEXT
#define INCLUDED_BSLFMT_MOCKPARSECONTEXT

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide mock context to test formatter specializations
//
//@CLASSES:
//  MockParseContext: Parsing context for use in formatter tests
//
//@DESCRIPTION: This component provides a class that holds the format string
// parsing state and is used for formatter testing.
//
///Usage
///-----
// This section illustrates intended usage of this component.
//
///Example: Simulating format specification parsing
/// - - - - - - - - - - - - - - - - - - - - - - - -
// `MockParseContext` is designed to imitate the behavior of the
// `basic_format_parse_context`. Therefore, let's look at the examples of
// real-life context usage (`FormatterSpecificationSplitter::parse()`), repeat
// these steps and see the results.
//
// First, create a `MockParseContext` based on the defined specification:
// ```
//  typedef bslfmt::MockParseContext<char> ParseContext;
//  typedef ParseContext::const_iterator   ContextIterator;
//
//  const char   *formatSpecification       = "*<5x";
//  const int     formatSpecificationLength = 4;
//  const int     numArgs                   = 1;
//  ParseContext  mpc(formatSpecification, numArgs);
// ```
// Next call basic accessors:
// ```
//  typename ParseContext::const_iterator current = mpc.begin();
//  typename ParseContext::const_iterator end     = mpc.end();
//
//  assert(formatSpecification == BSLS_UTIL_ADDRESSOF(*current));
//  assert(current + formatSpecificationLength == end);
// ```
// Now check the work with arguments:
// ```
//  mpc.check_arg_id(0);
//  assert(ParseContext::e_MANUAL == mpc.indexingMode());
// ```
// Finally, advance our mock context:
// ```
//  ++current;
//  assert(current != mpc.begin());
//  mpc.advance_to(current);
//  assert(current == mpc.begin());
// ```

#include <bslscm_version.h>

#include <bslfmt_format_arg.h>
#include <bslfmt_format_args.h>
#include <bslfmt_format_context.h>

#include <bsls_keyword.h>

#include <bslstl_array.h>
#include <bslstl_stringview.h>

namespace BloombergLP {
namespace bslfmt {

                  // ======================
                  // class MockParseContext
                  // ======================

/// This class template provides an access to the format specification current
/// parsing state.
template <class t_CHAR>
struct MockParseContext {

  public:
    // TYPES
    typedef
        typename bsl::basic_string_view<t_CHAR>::const_iterator const_iterator;
    typedef const_iterator                                      iterator;

    /// Argument indexing mode
    enum IndexingMode {
        e_UNKNOWN,   // default mode
        e_MANUAL,    // manual mode
        e_AUTOMATIC  // automatic mode
    };

  private:
    // DATA
    iterator     d_begin;         // beginning of the format spec
    iterator     d_end;           // end of the format spec
    IndexingMode d_indexingMode;  // current indexing mode
    size_t       d_next_arg_id;   // argument index
    size_t       d_num_args;      // number of arguments

    // NOT IMPLEMENTED
    MockParseContext(const MockParseContext&) BSLS_KEYWORD_DELETED;
    MockParseContext& operator=(const MockParseContext&) BSLS_KEYWORD_DELETED;

  public:
    // TYPES
    typedef t_CHAR char_type;

    // CREATORS
    /// Create an object having the specified `fmt` as a fomat specification
    /// and the specified `numArgs`.
    BSLS_KEYWORD_CONSTEXPR_CPP20 explicit MockParseContext(
             bsl::basic_string_view<t_CHAR> fmt,
             size_t                         numArgs = 0) BSLS_KEYWORD_NOEXCEPT;

    // MANIPULATORS

    /// Update the held iterator to the unparsed portion of the format string
    /// to be the specified `it`. Subsequent calls to `begin` will return this
    /// value.
    BSLS_KEYWORD_CONSTEXPR_CPP20 void advance_to(const_iterator it);

    /// Enter automatic indexing mode and return the next argument index.
    /// Throw `bslfmt::format_error` if this object has already entered manual
    /// indexing mode.
    BSLS_KEYWORD_CONSTEXPR_CPP20 size_t next_arg_id();

    /// Check whether the specified `id` is in range of number of arguments and
    /// enter manual indexing mode. Throw `bslfmt::format_error` if this object
    /// has already entered automatic indexing mode.
    BSLS_KEYWORD_CONSTEXPR_CPP20 void check_arg_id(size_t id);

    // ACCESSORS

    /// Return an iterator to the beginning of the format specification.
    BSLS_KEYWORD_CONSTEXPR_CPP20 const_iterator
    begin() const BSLS_KEYWORD_NOEXCEPT;

    /// Return an iterator to the end of the format specification.
    BSLS_KEYWORD_CONSTEXPR_CPP20 const_iterator
    end() const BSLS_KEYWORD_NOEXCEPT;

    /// Return the current indexing mode.
    BSLS_KEYWORD_CONSTEXPR_CPP20 IndexingMode
    indexingMode() const BSLS_KEYWORD_NOEXCEPT;
};

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

                  // ----------------------
                  // class MockParseContext
                  // ----------------------

// CREATORS
template <class t_CHAR>
BSLS_KEYWORD_CONSTEXPR_CPP20
MockParseContext<t_CHAR>::MockParseContext(
                  bsl::basic_string_view<t_CHAR> fmt,
                  size_t                         numArgs) BSLS_KEYWORD_NOEXCEPT
: d_begin(fmt.begin())
, d_end(fmt.end())
, d_indexingMode(e_UNKNOWN)
, d_next_arg_id(0)
, d_num_args(numArgs)
{
}

// MANIPULATORS
template <class t_CHAR>
BSLS_KEYWORD_CONSTEXPR_CPP20 void
MockParseContext<t_CHAR>::advance_to(const_iterator it)
{
    d_begin = it;
}

template <class t_CHAR>
BSLS_KEYWORD_CONSTEXPR_CPP20 size_t
MockParseContext<t_CHAR>::next_arg_id()
{
    if (e_MANUAL == d_indexingMode) {
        BSLS_THROW(format_error("mixing of automatic and manual indexing"));
    }
    if (d_next_arg_id >= d_num_args) {
        BSLS_THROW(format_error("number of conversion specifiers exceeds "
        "number of arguments"));
    }
    if (e_UNKNOWN == d_indexingMode) {
        d_indexingMode = e_AUTOMATIC;
    }
    return d_next_arg_id++;
}

template <class t_CHAR>
BSLS_KEYWORD_CONSTEXPR_CPP20 void
MockParseContext<t_CHAR>::check_arg_id(size_t id)
{
    if (e_AUTOMATIC == d_indexingMode) {
        BSLS_THROW(format_error("mixing of automatic and manual indexing"));
    }
    if (id >= d_num_args) {
        BSLS_THROW(format_error("invalid argument index"));
    }
    if (e_UNKNOWN == d_indexingMode) {
        d_indexingMode = e_MANUAL;
    }
}

// ACCESSORS
template <class t_CHAR>
BSLS_KEYWORD_CONSTEXPR_CPP20
typename MockParseContext<t_CHAR>::const_iterator
MockParseContext<t_CHAR>::begin() const BSLS_KEYWORD_NOEXCEPT
{
    return d_begin;
}

template <class t_CHAR>
BSLS_KEYWORD_CONSTEXPR_CPP20
typename MockParseContext<t_CHAR>::const_iterator
MockParseContext<t_CHAR>::end() const BSLS_KEYWORD_NOEXCEPT
{
    return d_end;
}

template <class t_CHAR>
BSLS_KEYWORD_CONSTEXPR_CPP20
typename MockParseContext<t_CHAR>::IndexingMode
MockParseContext<t_CHAR>::indexingMode() const BSLS_KEYWORD_NOEXCEPT
{
    return d_indexingMode;
}

}  // close namespace bslfmt
}  // close enterprise namespace


#endif  // INCLUDED_BSLFMT_MOCKPARSECONTEXT

// ----------------------------------------------------------------------------
// Copyright 2024 Bloomberg Finance L.P.
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

