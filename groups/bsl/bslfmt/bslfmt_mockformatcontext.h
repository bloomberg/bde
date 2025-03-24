// bslfmt_mockformatcontext.h                                         -*-C++-*-

#ifndef INCLUDED_BSLFMT_MOCKFORMATCONTEXT
#define INCLUDED_BSLFMT_MOCKFORMATCONTEXT

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide mock context to test formatter specializations
//
//@CLASSES:
//  MockFormatContext: format context for use in formatter tests
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
// Suppose we want to test `format` function of some formatter that meets
// `BasicFormatter` requirements.  For example a formatter that formats integer
// values:
// ```
//  template <class t_VALUE>
//  class IntegerFormatter {
//    public:
//      // CREATORS
//
//      /// Create a formatter object.
//      IntegerFormatter();
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
// First, we create an object of our formatter:
// ```
//  IntegerFormatter<int> formatter;
// ```
// Next, we specify a value to format and define expected result of formatting.
// In this example we will skip the spec parsing step, but let's say we want to
// format the number `42` with the following spec: "*<5x".
// ```
//  const int     value                = 42;
//  const char   *expectedResult       = "2a***";
//  const size_t  expectedResultLength = std::strlen(expectedResult);
// ```
// Now create a `MockFormatContext` and format the value using our formatter:
// ```
//  typedef bslfmt::MockFormatContext<char> FormatContext;
//
//  FormatContext           mfc(value);
//  FormatContext::iterator begin  = mfc.out();
//
//  mfc.advance_to(bsl::as_const(formatter).format(value, mfc));
//  FormatContext::iterator end  = mfc.out();
// ```
// Finally, verify that `format` function returns the correct past-the-end
// iterator and produces the expected result string:
// ```
//  const size_t actualResultLength = static_cast<size_t>(end.rawPointer() -
//                                                        begin.rawPointer());
//  ASSERT(expectedResultLength == actualResultLength);
//  assert(expectedResult       == mfc.finalString());
// ```

#include <bslscm_version.h>

#include <bslfmt_format_arg.h>
#include <bslfmt_format_context.h>
#include <bslfmt_formatterbase.h>

#include <bsls_keyword.h>
#include <bsls_compilerfeatures.h>

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

/// This unconstrained (value-semantic) class provides an access to the current
/// state of the format operation and is used for testing `bsl::formatter`
/// specializations.
template <class t_CHAR>
class MockFormatContext {
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

  private:
    // NOT IMPLEMENTED
    MockFormatContext(const MockFormatContext& original);
    MockFormatContext& operator=(const MockFormatContext& original);

  public:
    // TYPES
    typedef t_CHAR char_type;

#ifdef BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES
    template <class t_TYPE>
    using formatter_type = bsl::formatter<t_TYPE, t_CHAR>;
#endif

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
    Format_ArgUtil::makeFormatArgArray(&arr, arg_0);
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
    Format_ArgUtil::makeFormatArgArray(&arr, arg_0, arg_1);
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
    Format_ArgUtil::makeFormatArgArray(&arr, arg_0, arg_1, arg_2);
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
    Format_ArgUtil::makeFormatArgArray(&arr, arg_0, arg_1, arg_2, arg_3);
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

}  // close package namespace
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

