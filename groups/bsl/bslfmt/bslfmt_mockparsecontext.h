// bslfmt_mockparsecontext.h                                          -*-C++-*-

#ifndef INCLUDED_BSLFMT_MOCKPARSECONTEXT
#define INCLUDED_BSLFMT_MOCKPARSECONTEXT

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide mock context to test formatter specializations
//
//@CLASSES:
//  MockParseContext: parsing context for use in formatter tests
//
//@DESCRIPTION: This component provides a class that holds the format string
// parsing state and is used for formatter testing.
//
///Usage
///-----
// This section illustrates intended usage of this component.
//
///Example 1: Testing Formatter's `parse` Method
///- - - - - - - - - - - - - - - - - - - - - - -
// Suppose we have a formatter for our custom type representing a month and we
// want to test it.  The following example demonstrates how we can test its
// `parse` method using `bslfmt::MockParseContext`.
//
// First, we define our `Month` class:
// ```
// /// This class implements a complex-constrained, value-semantic type for
// /// representing months.
// class Month {
//   private:
//     // DATA
//     int d_index;   // month's index
//
//   public:
//     // CREATORS
//
//     /// Create an object having the value represented by the specified
//     /// `index`.
//     Month(int index)
//     : d_index(index)
//     {
//         assert((1 <= index) && (12 >= index));
//     }
//
//     // ACCESSORS
//
//     /// Return the index of this month.
//     int index() const { return d_index; }
// };
// ```
// Then, we define our custom formatter for this class.  In it, two methods are
// necessary: `parse` and `format`.  The `parse` method parses the format
// string itself to determine the formatting to be used by the `format` method,
// which writes the formatted object into user-supplied output iterator.
// ```
// /// This struct is a base class for `bsl::formatter` specializations for
// /// the `Month` class.
// template <class t_CHAR>
// struct MonthFormatter {
// ```
// The convenience of using the `bsl::format` function is that the users can
// come up with the description language themselves.  In our case, for
// simplicity, we will present month in two formats - numeric ("03") and verbal
// ("March").  Accordingly, to indicate the desired type, we will use one of
// the two letters in the format description: 'n' ('N') or 'v' ('V').
// Additionally, user can specify minimal width of the output either string via
// digit in the format specification or via additional parameter for
// `bsl::format` function.
// ```
//     // TYPES
//     enum Format {
//         e_NUMERIC,  // "03"
//         e_VERBAL    // "March"
//     };
//
//     typedef bslfmt::FormatterSpecificationNumericValue NumericValue;
//
//     // DATA
//     Format       d_format;    // output format
//     NumericValue d_rawWidth;  // minimal output width
//
//   public:
//     // CREATORS
//
//     /// Create a formatter that outputs values in the `e_NUMERIC` format.
//     /// Thus, numeric is the default format for the `Month` object.
//     BSLS_KEYWORD_CONSTEXPR_CPP20 MonthFormatter()
//     : d_format(e_NUMERIC)
//     {
//     }
//
//     // MANIPULATORS
//
//     /// Parse the specified `context` and return end iterator of parsed
//     /// range.
//     template <class t_PARSE_CONTEXT>
//     BSLS_KEYWORD_CONSTEXPR_CPP20 typename t_PARSE_CONTEXT::iterator parse(
//                                                    t_PARSE_CONTEXT& context)
//     {
// ```
// `MockParseContext` completely repeats the interface and behavior of the
// `bslfmt::basic_format_parse_context`, but provides an additional accessor
// that allows users to get information about the parsing process.  Therefore
// users do not need to declare a separate overload of `parse` for test
// purposes as long as their formatter's `parse` method is templated.
// ```
//         typename t_PARSE_CONTEXT::const_iterator current = context.begin();
//         typename t_PARSE_CONTEXT::const_iterator end     = context.end();
//
//         // Handling empty string or empty specification
//         if (current == end || *current == '}') {
//             return context.begin();                                // RETURN
//         }
//
//         d_rawWidth.parse(&current, end, false);
//         // Non-relative widths must be strictly positive.
//         if (d_rawWidth == NumericValue(NumericValue::e_VALUE, 0)) {
//             BSLS_THROW(bsl::format_error("Field widths must be > 0."));
//         }
//
//         if (d_rawWidth.category() == NumericValue::e_ARG_ID) {
//             context.check_arg_id(d_rawWidth.value());
//         }
//         else if (d_rawWidth.category() == NumericValue::e_NEXT_ARG) {
//             d_rawWidth = NumericValue(
//                                   NumericValue::e_ARG_ID,
//                                   static_cast<int>(context.next_arg_id()));
//         }
//
//         if (current == end || *current == '}') {
//             return context.begin();                                // RETURN
//         }
//
//         // Reading format specification
//         switch (*current) {
//             case 'V':
//             case 'v': {
//               d_format = e_VERBAL;
//             } break;
//             case 'N':
//             case 'n': {
//               // `e_NUMERIC` value is assigned at object construction
//             } break;
//             default: {
//               BSLS_THROW(bsl::format_error(
//                    "Unexpected symbol in format specification"));   // THROW
//             }
//         }
//
//         // Move the iterator to the next position and check that there are
//         // no extra characters in the description.
//
//         ++current;
//
//         if (current != end && *current != '}') {
//             BSLS_THROW(bsl::format_error(
//                     "Too many symbols in format specification"));   // THROW
//         }
//
//         context.advance_to(current);
//         return context.begin();
//     }
// ```
// To reduce the size of this example, we will omit the implementation of the
// `format` method as it is not essential for our purposes.
// ```
// };
// ```
// Finally, we can test the operation of the `parse` function for different
// input specifications:
// ```
// typedef bslfmt::MockParseContext<char> Context;
// typedef Context::iterator              ContextIterator;
//
// {
//     MonthFormatter<char> formatter;
//     Context              context("v");
//
//     ContextIterator iterator = formatter.parse(context);
//
//     assert(context.end() == iterator);
// ```
// Since width is not presented in the format specification, we don't expect
// our context to change its indexing mode.
// ```
//     assert(Context::e_UNKNOWN == context.indexingMode());
// }
// {
//     MonthFormatter<char> formatter;
//     Context              context("8v");
//
//     ContextIterator iterator = formatter.parse(context);
//
//     assert(context.end()      == iterator);
//     assert(Context::e_UNKNOWN == context.indexingMode());
// }
// {
//     MonthFormatter<char> formatter;
//     Context              context("{}v", 1);
//
//     ContextIterator iterator = formatter.parse(context);
//
//     assert(context.end()        == iterator);
// ```
// And here it is assumed that the width will be determined by the next
// parameter of the `bsl::format` function.  The indexing mode of the context
// changes accordingly.
// ```
//     assert(Context::e_AUTOMATIC == context.indexingMode());
// }
// {
//     MonthFormatter<char> formatter;
//     Context              context("{1}v", 2);
//
//     ContextIterator iterator = formatter.parse(context);
//
//     assert(context.end()     == iterator);
// ```
// Here we explicitly indicate the ordinal number of the `bsl::format`
// parameter storing the width value.
// ```
//     assert(Context::e_MANUAL == context.indexingMode());
// }
//
// ```

#include <bslscm_version.h>

#include <bslfmt_formaterror.h>

#include <bsls_exceptionutil.h>
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
class MockParseContext {

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
    /// Create an object having the specified `fmt` as a format specification
    /// and the specified `numArgs`.
    BSLS_KEYWORD_CONSTEXPR_CPP20 explicit MockParseContext(
             bsl::basic_string_view<t_CHAR> fmt,
             size_t                         numArgs = 0) BSLS_KEYWORD_NOEXCEPT;

    // MANIPULATORS

    /// Update the held iterator to the unparsed portion of the format string
    /// to be the specified `it`.  Subsequent calls to `begin` will return this
    /// value.
    BSLS_KEYWORD_CONSTEXPR_CPP20 void advance_to(const_iterator it);

    /// Enter automatic indexing mode and return the next argument index.
    /// Throw `bslfmt::format_error` if this object has already entered manual
    /// indexing mode.
    BSLS_KEYWORD_CONSTEXPR_CPP20 size_t next_arg_id();

    /// Check whether the specified `id` is in range of number of arguments and
    /// enter manual indexing mode.  Throw `bslfmt::format_error` if this
    /// object has already entered automatic indexing mode.
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

}  // close package namespace
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

