// bslstl_iomanip.h                                                   -*-C++-*-
#ifndef INCLUDED_BSLSTL_IOMANIP
#define INCLUDED_BSLSTL_IOMANIP

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide BSL implementations for standard <iomanip> features.
//
//@CLASSES:
//
//@CANONICAL_HEADER: bsl_iomanip.h
//
//@SEE_ALSO: bsl+bslhdrs
//

//@DESCRIPTION: This component is for internal use only.  Please include
// '<bsl_iomanip.h>' instead.
//
// This component exists to provide BSL implementations for facilities in the
// standard <iomanip> header.  While most of the facilities in 'bsl_iomanip.h'
// are simply aliases to the platform standard library, some of them need BSL
// specific implementations.  For example, this component provides
// implementations for 'bsl::quoted' overloads that accept 'bsl::basic_string'
// and proprietary 'bsl::string_view' objects.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Basic Use of 'bsl::quoted'
///- - - - - - - - - - - - - - - - - - -
// Suppose we want to serialize some data into JSON.
//
// First, we define a struct, 'Employee', to contain the data:
//..
//  struct Employee {
//      bsl::string d_firstName;
//      bsl::string d_lastName;
//      int         d_age;
//  };
//..
// Then, we create an 'Employee' object and populate it with data:
//..
//  Employee john;
//  john.d_firstName = "John";
//  john.d_lastName  = "Doe";
//  john.d_age       = 20;
//..
//  Now, we create an output stream and manually construct the JSON string
//  using 'bsl::quoted':
//..
//  bsl::stringstream ss;
//  ss << '{' << '\n';
//  ss << bsl::quoted("firstName");
//  ss << ':';
//  ss << bsl::quoted(john.d_firstName);
//  ss << ',' << '\n';
//  ss << bsl::quoted("lastName");
//  ss << ':';
//  ss << bsl::quoted(john.d_lastName);
//  ss << ',' << '\n';
//  ss << bsl::quoted("age");
//  ss << ':';
//  ss << john.d_age;
//  ss << '\n' << '}';
//..
//  Finally, we check out the JSON string:
//..
//  bsl::string expected = "{\n"
//                         "\"firstName\":\"John\",\n"
//                         "\"lastName\":\"Doe\",\n"
//                         "\"age\":20\n"
//                         "}";
//  assert(expected == ss.str());
//..
//  The output should look like:
//..
//  {
//  "firstName":"John",
//  "lastName":"Doe",
//  "age":20
//  }
//..

#include <bslstl_string.h>
#include <bslstl_stringview.h>

#include <bsls_libraryfeatures.h>
#include <bsls_platform.h>

#include <iomanip>
#include <istream>
#include <ostream>

namespace bsl {
    // Import selected symbols into bsl namespace

    using std::resetiosflags;
    using std::setbase;
    using std::setfill;
    using std::setiosflags;
    using std::setprecision;
    using std::setw;

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
    using std::get_money;
    using std::put_money;
#endif  // BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_MISCELLANEOUS_UTILITIES
    using std::get_time;
    using std::put_time;
#endif  // BSLS_LIBRARYFEATURES_HAS_CPP11_MISCELLANEOUS_UTILITIES

#ifndef BDE_OMIT_INTERNAL_DEPRECATED
    // Export additional names, leaked to support transitive dependencies in
    // higher level (non BDE) Bloomberg code.
# if !defined(BSLS_PLATFORM_CMP_MSVC) && __cplusplus < 201703L
    // As some of these names are removed from C++17, take a sledgehammer to
    // crack this nut, and remove all non-standard exports.
    using std::bad_exception;
    using std::basic_ios;
    using std::basic_iostream;
    using std::basic_istream;
    using std::basic_ostream;
    using std::basic_streambuf;
    using std::bidirectional_iterator_tag;
    using std::ctype;
    using std::ctype_base;
    using std::ctype_byname;
    using std::exception;
    using std::forward_iterator_tag;
    using std::input_iterator_tag;
    using std::ios_base;
    using std::istreambuf_iterator;
    using std::iterator;
    using std::locale;
    using std::num_get;
    using std::numpunct;
    using std::numpunct_byname;
    using std::ostreambuf_iterator;
    using std::output_iterator_tag;
    using std::random_access_iterator_tag;
    using std::set_terminate;
    using std::set_unexpected;
    using std::swap;
    using std::terminate;
    using std::terminate_handler;
    using std::uncaught_exception;
    using std::unexpected;
    using std::unexpected_handler;
    using std::use_facet;
# endif // MSVC, or C++2017
#endif  // BDE_OMIT_INTERNAL_DEPRECATED

#if defined BSLS_LIBRARYFEATURES_HAS_CPP14_BASELINE_LIBRARY
   using std::quoted;
#endif  // BSLS_LIBRARYFEATURES_HAS_CPP14_BASELINE_LIBRARY

}  // close namespace bsl

namespace BloombergLP {
namespace bslstl {
#if defined BSLS_LIBRARYFEATURES_HAS_CPP14_BASELINE_LIBRARY
// 'std::quoted' function does not support the 'bsl::basic_string' and
// proprietary implementation of the 'bsl::basic_string_view' classes.  To fix
// this we need to add special overloads that return objects of the special
// classes, that can be used in stream input/output operations and construct a
// bridge between 'std' and 'bsl' 'string'/'string_view' implementations.

                    // ===================================
                    // class IoManip_QuotedStringFormatter
                    // ===================================

template <class t_CHAR_TYPE, class t_CHAR_TRAITS, class t_ALLOC>
class IoManip_QuotedStringFormatter {
    // Private class: do not use outside of 'bslstl_iomanip.h' header.  This
    // private class provides a temporary storage that can be extracted to/from
    // a stream and provides data to the standard implementation of the
    // 'quoted' function.  Note that this class does not contain the storage
    // itself, but only points to an external object.  Note that
    // 'QuotedStringViewFormatter' is designed so that its objects are returned
    // as temporary objects (of opaque type) from 'bsl::quoted`, and then
    // passed to a streaming operator; it serves as a temporary proxy for a
    // string value.

    // DATA
    bsl::basic_string<t_CHAR_TYPE, t_CHAR_TRAITS, t_ALLOC> *d_str_p;
        // string for quoting (held, not owned)

    t_CHAR_TYPE                                             d_delim;
        // delimiter

    t_CHAR_TYPE                                             d_escape;
        // escape character

  public:
    // CREATORS
    explicit IoManip_QuotedStringFormatter(
               bsl::basic_string<t_CHAR_TYPE, t_CHAR_TRAITS, t_ALLOC> *str,
               t_CHAR_TYPE                                             delim =
                                                              t_CHAR_TYPE('"'),
               t_CHAR_TYPE                                             escape =
                                                            t_CHAR_TYPE('\\'));
        // Create an object pointing to the specified 'str' and having the
        // optionally specified 'delim' and 'escape' characters.

    // ACCESSORS
    t_CHAR_TYPE delim() const;
        // Return the delimiter character.

    t_CHAR_TYPE escape() const;
        // Return the escape character.

    bsl::basic_string<t_CHAR_TYPE, t_CHAR_TRAITS, t_ALLOC> *str() const;
        // Return a reference providing modifiable access to the underlying
        // string.  Note that this operation provides modifiable access to the
        // string because this type is designed so that its objects are created
        // as temporary objects that proxy an underlying string (see class
        // documentation).
};

                  // =======================================
                  // class IoManip_QuotedStringViewFormatter
                  // =======================================

template <class t_CHAR_TYPE, class t_CHAR_TRAITS>
class IoManip_QuotedStringViewFormatter {
    // Private class: do not use outside of 'bslstl_iomanip.h' header.  This
    // private class provides a temporary storage that can be extracted to a
    // stream and provides data to the standard implementation of the 'quoted'
    // function.  Note that 'QuotedStringViewFormatter' is designed to be
    // returned as a temporary object (of opaque type) from 'bsl::quoted`, and
    // passed to a streaming operator; it serves as a temporary proxy for a
    // string value.  Also note that this value is copied to 'bsl::string' data
    // member that is owned by the object of this class.

    // DATA
    bsl::basic_string<t_CHAR_TYPE, t_CHAR_TRAITS> d_str;     // string for
                                                             // quoting

    t_CHAR_TYPE                                   d_delim;   // delimiter

    t_CHAR_TYPE                                   d_escape;  // escape
                                                             // character

  public:
    // CREATORS
    explicit IoManip_QuotedStringViewFormatter(
             const bsl::basic_string_view<t_CHAR_TYPE, t_CHAR_TRAITS>& strView,
             t_CHAR_TYPE                                               delim =
                                                              t_CHAR_TYPE('"'),
             t_CHAR_TYPE                                               escape =
                                                            t_CHAR_TYPE('\\'));
        // Create an object having the value of the specified 'strView' and the
        // optionally specified 'delim' and 'escape' characters.

    // ACCESSORS
    const t_CHAR_TYPE *data() const;
        // Return a reference providing modifiable access to the character
        // buffer of the underlying string.

    t_CHAR_TYPE delim() const;
        // Return the delimiter character.

    t_CHAR_TYPE escape() const;
        // Return the escape character.
};

// FREE FUNCTIONS
template <class t_CHAR_TYPE, class t_CHAR_TRAITS, class t_ALLOC>
std::basic_istream<t_CHAR_TYPE, t_CHAR_TRAITS>& operator>>(
                       std::basic_istream<t_CHAR_TYPE, t_CHAR_TRAITS>& input,
                       const IoManip_QuotedStringFormatter<t_CHAR_TYPE,
                                                           t_CHAR_TRAITS,
                                                           t_ALLOC>&   object);
    // Read quoted string from the specified 'input' stream into the underlying
    // string of the specified 'object'.  Note that the 'object' here is
    // 'const' because the 'QuotedStringFormatter' is designed to be returned
    // as a temporary object from 'bsl::quoted`, serving as a proxy for the
    // underlying string.

template <class t_CHAR_TYPE, class t_CHAR_TRAITS, class t_ALLOC>
std::basic_ostream<t_CHAR_TYPE, t_CHAR_TRAITS>& operator<<(
                       std::basic_ostream<t_CHAR_TYPE, t_CHAR_TRAITS>& output,
                       const IoManip_QuotedStringFormatter<t_CHAR_TYPE,
                                                           t_CHAR_TRAITS,
                                                           t_ALLOC>&   object);
    // Write the value of the specified 'object' to the specified 'output'
    // stream.

template <class t_CHAR_TYPE, class t_CHAR_TRAITS>
std::basic_ostream<t_CHAR_TYPE, t_CHAR_TRAITS>& operator<<(
               std::basic_ostream<t_CHAR_TYPE, t_CHAR_TRAITS>&         output,
               const IoManip_QuotedStringViewFormatter<t_CHAR_TYPE,
                                                       t_CHAR_TRAITS>& object);
    // Write the value of the specified 'object' to the specified 'output'
    // stream.

#endif  // BSLS_LIBRARYFEATURES_HAS_CPP14_BASELINE_LIBRARY
}  // close package namespace
}  // close enterprise namespace

namespace bsl {
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP14_BASELINE_LIBRARY
template <class t_CHAR_TYPE, class t_CHAR_TRAITS, class t_ALLOC>
decltype(auto)
quoted(const bsl::basic_string<t_CHAR_TYPE, t_CHAR_TRAITS, t_ALLOC>& value,
       t_CHAR_TYPE                                                   delim =
                                                              t_CHAR_TYPE('"'),
       t_CHAR_TYPE                                                   escape =
                                                            t_CHAR_TYPE('\\'));
    // Return an object, containing quoted version of the specified 'value'
    // obtained using the optionally specified 'delim' and 'escape' characters,
    // and that can be inserted to output stream.

template <class t_CHAR_TYPE, class t_CHAR_TRAITS, class t_ALLOC>
BloombergLP::bslstl::IoManip_QuotedStringFormatter<t_CHAR_TYPE,
                                                    t_CHAR_TRAITS,
                                                   t_ALLOC>
quoted(bsl::basic_string<t_CHAR_TYPE, t_CHAR_TRAITS, t_ALLOC>& value,
       t_CHAR_TYPE                                             delim =
                                                              t_CHAR_TYPE('"'),
       t_CHAR_TYPE                                             escape =
                                                            t_CHAR_TYPE('\\'));
    // Return an object, containing quoted version of the specified 'value'
    // obtained using the optionally specified 'delim' and 'escape' characters,
    // and that can be inserted to output (or extracted from input) stream.

#ifndef BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY
template <class t_CHAR_TYPE, class t_CHAR_TRAITS>
BloombergLP::bslstl::IoManip_QuotedStringViewFormatter<t_CHAR_TYPE,
                                                       t_CHAR_TRAITS>
quoted(const bsl::basic_string_view<t_CHAR_TYPE, t_CHAR_TRAITS>& value,
       t_CHAR_TYPE                                               delim =
                                                              t_CHAR_TYPE('"'),
       t_CHAR_TYPE                                               escape =
                                                            t_CHAR_TYPE('\\'));
    // Return an object, containing quoted version of the specified 'value'
    // obtained using the optionally specified 'delim' and 'escape' characters,
    // and that can be inserted to output stream.
#endif  // BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY
#endif  // BSLS_LIBRARYFEATURES_HAS_CPP14_BASELINE_LIBRARY
}  // close namespace bsl

// ============================================================================
//                         INLINE FUNCTION DEFINITIONS
// ============================================================================

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP14_BASELINE_LIBRARY
                    // -----------------------------------
                    // class IoManip_QuotedStringFormatter
                    // -----------------------------------

// CREATORS
template <class t_CHAR_TYPE, class t_CHAR_TRAITS, class t_ALLOC>
inline
BloombergLP::bslstl::
    IoManip_QuotedStringFormatter<t_CHAR_TYPE, t_CHAR_TRAITS, t_ALLOC>::
        IoManip_QuotedStringFormatter(
                bsl::basic_string<t_CHAR_TYPE, t_CHAR_TRAITS, t_ALLOC> *str,
                t_CHAR_TYPE                                             delim,
                t_CHAR_TYPE                                             escape)
: d_str_p(str)
, d_delim(delim)
, d_escape(escape)
{
}

// ACCESSORS
template <class t_CHAR_TYPE, class t_CHAR_TRAITS, class t_ALLOC>
inline
t_CHAR_TYPE
BloombergLP::bslstl::IoManip_QuotedStringFormatter<t_CHAR_TYPE,
                                                   t_CHAR_TRAITS,
                                                   t_ALLOC>::delim() const
{
    return d_delim;
}

template <class t_CHAR_TYPE, class t_CHAR_TRAITS, class t_ALLOC>
inline
t_CHAR_TYPE
BloombergLP::bslstl::IoManip_QuotedStringFormatter<t_CHAR_TYPE,
                                                   t_CHAR_TRAITS,
                                                   t_ALLOC>::escape() const
{
    return d_escape;
}

template <class t_CHAR_TYPE, class t_CHAR_TRAITS, class t_ALLOC>
inline
bsl::basic_string<t_CHAR_TYPE, t_CHAR_TRAITS, t_ALLOC> *
BloombergLP::bslstl::IoManip_QuotedStringFormatter<t_CHAR_TYPE,
                                                   t_CHAR_TRAITS,
                                                   t_ALLOC>::str() const
{
    return d_str_p;
}

                  // ---------------------------------------
                  // class IoManip_QuotedStringViewFormatter
                  // ---------------------------------------

// CREATORS
template <class t_CHAR_TYPE, class t_CHAR_TRAITS>
inline
BloombergLP::bslstl::IoManip_QuotedStringViewFormatter<t_CHAR_TYPE,
                                                       t_CHAR_TRAITS>::
    IoManip_QuotedStringViewFormatter(
             const bsl::basic_string_view<t_CHAR_TYPE, t_CHAR_TRAITS>& strView,
             t_CHAR_TYPE                                               delim,
             t_CHAR_TYPE                                               escape)
: d_str(strView)
, d_delim(delim)
, d_escape(escape)
{
}

// ACCESSORS
template <class t_CHAR_TYPE, class t_CHAR_TRAITS>
inline
const t_CHAR_TYPE *
BloombergLP::bslstl::IoManip_QuotedStringViewFormatter<t_CHAR_TYPE,
                                                       t_CHAR_TRAITS>::data()
    const
{
    return d_str.c_str();
}

template <class t_CHAR_TYPE, class t_CHAR_TRAITS>
inline
t_CHAR_TYPE
BloombergLP::bslstl::IoManip_QuotedStringViewFormatter<t_CHAR_TYPE,
                                                       t_CHAR_TRAITS>::delim()
    const
{
    return d_delim;
}

template <class t_CHAR_TYPE, class t_CHAR_TRAITS>
inline
t_CHAR_TYPE
BloombergLP::bslstl::IoManip_QuotedStringViewFormatter<t_CHAR_TYPE,
                                                       t_CHAR_TRAITS>::escape()
    const
{
    return d_escape;
}

// FREE FUNCTIONS
template <class t_CHAR_TYPE, class t_CHAR_TRAITS, class t_ALLOC>
std::basic_istream<t_CHAR_TYPE, t_CHAR_TRAITS>&
BloombergLP::bslstl::operator>>(
                        std::basic_istream<t_CHAR_TYPE, t_CHAR_TRAITS>& input,
                        const IoManip_QuotedStringFormatter<t_CHAR_TYPE,
                                                            t_CHAR_TRAITS,
                                                            t_ALLOC>&   object)
{
    std::basic_string<t_CHAR_TYPE, t_CHAR_TRAITS> temp;
    input >> std::quoted(temp, object.delim(), object.escape());
    *object.str() = temp;
    return input;
}

template <class t_CHAR_TYPE, class t_CHAR_TRAITS, class t_ALLOC>
std::basic_ostream<t_CHAR_TYPE, t_CHAR_TRAITS>&
BloombergLP::bslstl::operator<<(
                        std::basic_ostream<t_CHAR_TYPE, t_CHAR_TRAITS>& output,
                        const IoManip_QuotedStringFormatter<t_CHAR_TYPE,
                                                            t_CHAR_TRAITS,
                                                            t_ALLOC>&   object)
{
    output << std::quoted(object.str()->c_str(),
                          object.delim(),
                          object.escape());
    return output;
}

template <class t_CHAR_TYPE, class t_CHAR_TRAITS>
std::basic_ostream<t_CHAR_TYPE, t_CHAR_TRAITS>&
BloombergLP::bslstl::operator<<(
   std::basic_ostream<t_CHAR_TYPE, t_CHAR_TRAITS>&                      output,
   const IoManip_QuotedStringViewFormatter<t_CHAR_TYPE, t_CHAR_TRAITS>& object)
{
    output << std::quoted(object.data(), object.delim(), object.escape());
    return output;
}

                       // -----------------------------
                       // 'bsl::quoted' implementations
                       // -----------------------------

template <class t_CHAR_TYPE, class t_CHAR_TRAITS, class t_ALLOC>
decltype(auto)
bsl::quoted(
          const bsl::basic_string<t_CHAR_TYPE, t_CHAR_TRAITS, t_ALLOC>& value,
          t_CHAR_TYPE                                                   delim,
          t_CHAR_TYPE                                                   escape)
    // Return an object, containing quoted version of the specified 'value'
    // obtained using the optionally specified 'delim' and 'escape' characters,
    // and that can be inserted to output stream.
{
    return bsl::quoted(value.c_str(), delim, escape);
}

template <class t_CHAR_TYPE, class t_CHAR_TRAITS, class t_ALLOC>
BloombergLP::bslstl::IoManip_QuotedStringFormatter<t_CHAR_TYPE,
                                                   t_CHAR_TRAITS,
                                                   t_ALLOC>
bsl::quoted(bsl::basic_string<t_CHAR_TYPE, t_CHAR_TRAITS, t_ALLOC>& value,
            t_CHAR_TYPE                                             delim,
            t_CHAR_TYPE                                             escape)
    // Return an object, containing quoted version of the specified 'value'
    // obtained using the optionally specified 'delim' and 'escape' characters,
    // and that can be inserted to output (or extracted from input) stream.
{
    return BloombergLP::bslstl::IoManip_QuotedStringFormatter<t_CHAR_TYPE,
                                                              t_CHAR_TRAITS,
                                                              t_ALLOC>(&value,
                                                                       delim,
                                                                       escape);
}

#ifndef BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY
template <class t_CHAR_TYPE, class t_CHAR_TRAITS>
BloombergLP::bslstl::IoManip_QuotedStringViewFormatter<t_CHAR_TYPE,
                                                       t_CHAR_TRAITS>
bsl::quoted(const bsl::basic_string_view<t_CHAR_TYPE, t_CHAR_TRAITS>& value,
            t_CHAR_TYPE                                               delim,
            t_CHAR_TYPE                                               escape)
    // Return an object, containing quoted version of the specified 'value'
    // obtained using the optionally specified 'delim' and 'escape' characters,
    // and that can be inserted to output stream.
{
    return BloombergLP::bslstl::IoManip_QuotedStringViewFormatter<
                                                        t_CHAR_TYPE,
                                                        t_CHAR_TRAITS>(value,
                                                                       delim,
                                                                       escape);
}
#endif  // BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY
#endif  // BSLS_LIBRARYFEATURES_HAS_CPP14_BASELINE_LIBRARY

#endif

// ----------------------------------------------------------------------------
// Copyright 2022 Bloomberg Finance L.P.
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
