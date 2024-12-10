// bslfmt_mockformatcontext.h                                         -*-C++-*-

#ifndef INCLUDED_BSLFMT_MOCKFORMATCONTEXT
#define INCLUDED_BSLFMT_MOCKFORMATCONTEXT

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide mock context to test formatter specializations
//
//@CLASSES:
//  MockFormatContext: Format context for use in formatter tests
//
//@DESCRIPTION: This component provides a class that holds the state of the
// format operation and is used for formatter testing.
//
///Usage
///-----
// This section illustrates intended usage of this component.
//
///Example: Testing a formatter
/// - - - - - - - - - - - - - -
// Suppose we need to test some formatter meeting `BasicFormatter`
// requirements.
//
// First we define our `IntegerFormatter` template class.  Actually the
// implementation features are not of particular importance.  In this case,
// what is important to us is that the class contains `parse` and `format`
// methods with the expected interfaces. In place of this class, we can
// always substitute one of the existing `bsl::formatter` specializations, such
// as `bsl::formatter<long, char>` or `bsl::formatter<bool, wchar_t>`.
// ```
//  template <class t_VALUE>
//  class IntegerFormatter {
//      // DATA
//      bsl::string_view d_resultString;  // pre-defined result string
//
//    public:
//      // CREATORS
//
//      /// Creates the formatter having the specified 'resultString'.
//      IntegerFormatter(const char* resultString);
//
//      // MANIPULATORS
//
//      /// Parse the specified `parseContext` and return an iterator, pointing
//      /// to the end of the format string.
//      template <class t_PARSE_CONTEXT>
//      typename t_PARSE_CONTEXT::iterator parse(
//                                              t_PARSE_CONTEXT& parseContext);
//
//      // ACCESSORS
//
//      /// Create string representation of the specified `value`, customized
//      /// in accordance with the requested format and the specified
//      /// `formatContext`, and copy it to the output that the output iterator
//      /// of the `formatContext` points to.
//      template <class t_FORMAT_CONTEXT>
//      typename t_FORMAT_CONTEXT::iterator format(
//                                      t_VALUE           value,
//                                      t_FORMAT_CONTEXT& formatContext) const;
//  };
// ```
// Next we define our `ParseContext` template class.  As with the formatter, in
// this example we are interested in the expected interface, not the actual
// implementation:
// ```
//  template <class t_CHAR>
//  struct ParseContext {
//    public:
//      // TYPES
//      typedef typename bsl::basic_string_view<t_CHAR>::const_iterator
//                                                              const_iterator;
//      typedef const_iterator                                  iterator;
//
//    private:
//      // DATA
//      iterator d_begin;  // beginning of the unparsed part of the format spec
//      iterator d_end;    // end of the unparsed part of the format spec
//
//      // NOT IMPLEMENTED
//      ParseContext(const ParseContext&) BSLS_KEYWORD_DELETED;
//      ParseContext& operator=(const ParseContext&) BSLS_KEYWORD_DELETED;
//
//    public:
//      // CREATORS
//      /// Create an object having the specified `fmt` as a format
//      /// specification and the specified `numArgs`.
//      explicit ParseContext(bsl::basic_string_view<t_CHAR> fmt,
//                            size_t                         numArgs = 0);
//
//      // MANIPULATORS
//      /// Update the held iterator to the unparsed portion of the format
//      /// string to be the specified `it`. Subsequent calls to `begin` will
//      /// return this value.
//      BSLS_KEYWORD_CONSTEXPR_CPP20 void advance_to(const_iterator it);
//
//      // ACCESSORS
//      /// Return an iterator to the end of the format specification.
//      const_iterator end() const;
//  };
// ```
// Then, define format specification, value to output and the expected
// operation result:
// ```
//  const char *formatSpecification = "*<5x";
//  const char *expectedResult      = "2a***";
//  const int   value               = 42;
// ```
// Next, create a `ParseContext` object based on the defined specification and
// parse it:
// ```
//  ParseContext<char> pc(formatSpecification, 1);
//
//  IntegerFormatter<int> formatter(expectedResult);
//  pc.advance_to(formatter.parse(pc));
// ```
// Now create a `MockFormatContext` and format previously specified value using
// our formatter:
// ```
//  bslfmt::MockFormatContext<char> mfc(value, 0, 0);
//
//  mfc.advance_to(bsl::as_const(formatter).format(value, mfc));
// ```
// Finally check the resulting string:
// ```
//  assert(expectedResult == mfc.finalString());
// ```

#include <bslscm_version.h>

#include <bslfmt_formatarg.h>
#include <bslfmt_formatargs.h>
#include <bslfmt_formatcontext.h>

#include <bsls_keyword.h>

#include <bslstl_array.h>
#include <bslstl_stringview.h>

namespace BloombergLP {
namespace bslfmt {


                  // ================================
                  // class MockFormatContext_Iterator
                  // ================================

/// This class template provides an output iterator to the `MockFormatContext`
/// output buffer allowing appending formatted data.
template <class t_CHAR>
class MockFormatContext_Iterator {
  private:
    // DATA
    t_CHAR *d_ptr;  // pointer to the beginning of output buffer
    t_CHAR *d_max;  // pointer to the end of output buffer

  public:
    // TYPES
    typedef std::ptrdiff_t           difference_type;
    typedef t_CHAR                   value_type;
    typedef t_CHAR *                 pointer;
    typedef t_CHAR&                  reference;
    typedef std::output_iterator_tag iterator_category;

    // CREATORS

    /// Create an iterator, pointing to the specified `ptr` and having the
    /// specified `max` as the end of the buffer available for output.  The
    /// behavior is undefined unless `max >=ptr`.
    MockFormatContext_Iterator(t_CHAR *ptr, t_CHAR *max);

    // MANIPULATORS

    /// Increment this object to refer to the next character in the output
    /// buffer.  Throw `bslfmt::format_error` if the iterator goes beyond the
    /// allowed buffer bounds.
    MockFormatContext_Iterator& operator++();

    /// Copy this iterator, increment it, and return by value the copy that was
    /// made prior the increment.
    MockFormatContext_Iterator operator++(int);

    // ACCESSORS

    /// Return a reference to the character referred to by this iterator.
    t_CHAR& operator*() const;

    /// Return a pointer pointing to the character referred to by this
    /// iterator.
    t_CHAR *rawPointer() const;
};

                  // =======================
                  // class MockFormatContext
                  // =======================

/// This
template <class t_CHAR>
struct MockFormatContext {
  public:
    // TYPES
    typedef basic_format_arg<basic_format_context<t_CHAR *, t_CHAR> > Arg;
    typedef MockFormatContext_Iterator<t_CHAR>                        iterator;

  private:
    // PRIVATE TYPES
    enum {
        k_BUFFER_SIZE = 512  // maximum output buffer size
    };

    // DATA
    Arg      d_arg_0;                  // first argument
    Arg      d_arg_1;                  // second argument
    Arg      d_arg_2;                  // third argument
    Arg      d_arg_3;                  // forth argument

    t_CHAR   d_buffer[k_BUFFER_SIZE];  // output buffer
    iterator d_iterator;               // output iterator

  public:
    // TYPES
    typedef t_CHAR char_type;

    // CREATORS

    /// Create an object having the specified `arg_0` as an argument.
    template <class t_ARG0>
    MockFormatContext(const t_ARG0& arg_0);

    /// Create an object having the specified `arg_0` and `arg_1` as arguments.
    template <class t_ARG0, class t_ARG1>
    MockFormatContext(const t_ARG0& arg_0, const t_ARG1& arg_1);

    /// Create an object having the specified `arg_0`, `arg_1` and `arg_2` as
    /// arguments.
    template <class t_ARG0, class t_ARG1, class t_ARG2>
    MockFormatContext(const t_ARG0& arg_0,
                      const t_ARG1& arg_1,
                      const t_ARG2& arg_2);

    /// Create an object having the specified `arg_0`, `arg_1`, `arg_2` and
    /// `arg_3` as arguments.
    template <class t_ARG0, class t_ARG1, class t_ARG2, class t_ARG3>
    MockFormatContext(const t_ARG0& arg_0,
                      const t_ARG1& arg_1,
                      const t_ARG2& arg_2,
                      const t_ARG3& arg_3);

    // MANIPULATORS

    /// Advance the output iterator to the position referred by the specified
    /// `it`.
    void advance_to(iterator it);

    // ACCESSORS

    /// Return the argument with the specified `id` or the default constructed
    /// object if there is no argument with such `id`.
    Arg arg(size_t id) const BSLS_KEYWORD_NOEXCEPT;

    /// Return this object's output iterator.
    iterator out() const;

    /// Return output string.
    bsl::basic_string_view<t_CHAR> finalString() const;
};

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

                  // --------------------------------
                  // class MockFormatContext_Iterator
                  // --------------------------------

// CREATORS
template <class t_CHAR>
MockFormatContext_Iterator<t_CHAR>::MockFormatContext_Iterator(t_CHAR *ptr,
                                                               t_CHAR *max)
: d_ptr(ptr)
, d_max(max)
{
    BSLS_ASSERT(max >=ptr);
}

// MANIPULATORS
template <class t_CHAR>
MockFormatContext_Iterator<t_CHAR>&
MockFormatContext_Iterator<t_CHAR>::operator++()
{
    d_ptr++;
    if (d_ptr >= d_max)
        BSLS_THROW(format_error("MockFormatContext buffer overrun"));
    return *this;
}

template <class t_CHAR>
MockFormatContext_Iterator<t_CHAR>
MockFormatContext_Iterator<t_CHAR>::operator++(int)
{
    MockFormatContext_Iterator copy = *this;
    ++*this;
    return copy;
}

// ACCESSORS
template <class t_CHAR>
t_CHAR& MockFormatContext_Iterator<t_CHAR>::operator*() const
{
    return *d_ptr;
}

template <class t_CHAR>
t_CHAR *MockFormatContext_Iterator<t_CHAR>::rawPointer() const
{
    return d_ptr;
}

                  // -----------------------
                  // class MockFormatContext
                  // -----------------------

template <class t_CHAR>
template <class t_ARG0>
MockFormatContext<t_CHAR>::MockFormatContext(const t_ARG0& arg_0)
: d_buffer()
, d_iterator(d_buffer, d_buffer + k_BUFFER_SIZE - 1)
{
    bsl::array<Arg, 1> arr;
    Format_FormatArg_ImpUtil::makeFormatArgArray(&arr, arg_0);
    d_arg_0 = Arg(arr[0]);
}

template <class t_CHAR>
template <class t_ARG0, class t_ARG1>
MockFormatContext<t_CHAR>::MockFormatContext(const t_ARG0& arg_0,
                                             const t_ARG1& arg_1)
: d_buffer()
, d_iterator(d_buffer, d_buffer + k_BUFFER_SIZE - 1)
{
    bsl::array<Arg, 2> arr;
    Format_FormatArg_ImpUtil::makeFormatArgArray(&arr, arg_0, arg_1);
    d_arg_0 = Arg(arr[0]);
    d_arg_1 = Arg(arr[1]);
}

template <class t_CHAR>
template <class t_ARG0, class t_ARG1, class t_ARG2>
MockFormatContext<t_CHAR>::MockFormatContext(const t_ARG0& arg_0,
                                             const t_ARG1& arg_1,
                                             const t_ARG2& arg_2)
: d_buffer()
, d_iterator(d_buffer, d_buffer + k_BUFFER_SIZE - 1)
{
    bsl::array<Arg, 3> arr;
    Format_FormatArg_ImpUtil::makeFormatArgArray(&arr, arg_0, arg_1, arg_2);
    d_arg_0 = Arg(arr[0]);
    d_arg_1 = Arg(arr[1]);
    d_arg_2 = Arg(arr[2]);
}

template <class t_CHAR>
template <class t_ARG0, class t_ARG1, class t_ARG2, class t_ARG3>
MockFormatContext<t_CHAR>::MockFormatContext(const t_ARG0& arg_0,
                                             const t_ARG1& arg_1,
                                             const t_ARG2& arg_2,
                                             const t_ARG3& arg_3)
: d_buffer()
, d_iterator(d_buffer, d_buffer + k_BUFFER_SIZE - 1)
{
    bsl::array<Arg, 4> arr;
    Format_FormatArg_ImpUtil::makeFormatArgArray(&arr,
                                                 arg_0,
                                                 arg_1,
                                                 arg_2,
                                                 arg_3);
    d_arg_0 = Arg(arr[0]);
    d_arg_1 = Arg(arr[1]);
    d_arg_2 = Arg(arr[2]);
    d_arg_3 = Arg(arr[3]);
}

// MANIPULATORS
template <class t_CHAR>
void MockFormatContext<t_CHAR>::advance_to(iterator it)
{
    d_iterator = it;
}

// ACCESSORS
template <class t_CHAR>
typename MockFormatContext<t_CHAR>::Arg MockFormatContext<t_CHAR>::arg(
                                         size_t id) const BSLS_KEYWORD_NOEXCEPT
{
    if (id == 0)
        return d_arg_0;
    if (id == 1)
        return d_arg_1;
    if (id == 2)
        return d_arg_2;
    if (id == 3)
        return d_arg_3;

    return Arg();
}

template <class t_CHAR>
typename MockFormatContext<t_CHAR>::iterator
MockFormatContext<t_CHAR>::out() const
{
    return d_iterator;
}

template <class t_CHAR>
bsl::basic_string_view<t_CHAR> MockFormatContext<t_CHAR>::finalString() const
{
    return bsl::basic_string_view<t_CHAR>(d_buffer, d_iterator.rawPointer());
}

}  // close namespace bslfmt
}  // close enterprise namespace


#endif  // INCLUDED_BSLFMT_MOCKFORMATCONTEXT

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

