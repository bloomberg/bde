// bdlb_indexspanstringutil.h                                         -*-C++-*-
#ifndef INCLUDED_BDLB_INDEXSPANSTRINGUTIL
#define INCLUDED_BDLB_INDEXSPANSTRINGUTIL

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide functions that operate on 'IndexSpan' and string objects.
//
//@CLASSES:
//  IndexSpanStringUtil: namespace for methods on 'IndexSpan' and strings
//
//@SEE_ALSO: bdlb_indexspan
//
//@DESCRIPTION: This component provides a struct, 'IndexSpanStringUtil', that
// serves as a namespace for utility functions that operate on 'IndexSpan' and
// string objects.  This component is designed to work on several
// representations of strings, including 'bsl::string' and 'bslstl::StringRef'.
// Key operations of this class include 'bind', for creating a 'StringRef' from
// an 'IndexSpan' and a 'string', and the 'create' methods that provide a way
// to create 'IndexSpan' objects of a string using positions defined by
// iterators and/or positions and/or length.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Creating 'IndexSpan' Objects Safely
/// - - - - - - - - - - - - - - - - - - - - - - -
// Suppose that we are creating a parser, and we want the results of the
// parsing to be stored in 'IndexSpan' objects.  The parser will have either a
// pointer (or "begin" iterator) into the original string input and then
// another pointer (or iterator) or a length to identify the end of the input.
// Turning the beginning and ending identifiers into an 'IndexSpan' is a simple
// calculation, but one that is verbose and potentially error prone.  Instead
// of implementing the calculation ourselves we use the convenience function
// 'create' from 'IndexSpanStringUtil'.
//
// First, we define a string that we want to parse:
//..
//  const bsl::string full("James Tiberius Kirk");
//..
// Then, we implement the parsing of the first name:
//..
//  typedef bsl::string::const_iterator Cit;
//  Cit it = bsl::find(full.begin(), full.end(), ' ');
//  // Error checking omitted since we know the string
//  bdlb::IndexSpan first = bdlb::IndexSpanStringUtil::create(full,
//                                                            full.begin(),
//                                                            it);
//..
// Next, we implement the parsing of the middle name, this time using length,
// rather than an end iterator (demonstrating an alternative 'create' overload
// provided by 'IndexSpanStringUtil'):
//..
//  ++it;  // Skip the space
//  Cit it2 = bsl::find(it, full.end(), ' ');
//  bdlb::IndexSpan middle;
//  if (full.end() != it2) {
//      middle = bdlb::IndexSpanStringUtil::create(full, it, it2 - it);
//      it = it2 + 1;
//  }
//..
// Then, we create the 'IndexSpan' for the last name, using two positions:
//..
//  bdlb::IndexSpan last = bdlb::IndexSpanStringUtil::createFromPositions(
//                                                           full,
//                                                           it - full.begin(),
//                                                           full.length());
//..
// Finally, we verify that the resulting 'IndexSpan' objects correctly describe
// the parsed names of the 'full' name:
//..
//  assert(full.substr(first.position(), first.length()) == "James");
//
//  assert(full.substr(middle.position(), middle.length()) == "Tiberius");
//
//  assert(full.substr(last.position(), last.length()) == "Kirk");
//..
//
///Example 2: Creating String References
///- - - - - - - - - - - - - - - - - - -
// Suppose that we have 'IndexSpan' objects that define the 'first', 'middle',
// and 'last' part of a string that has a full name in it and we want to get
// actual string references that correspond to those parts of the string.  The
// 'bind' functions of 'IndexSpanStringUtil' provide that functionality.  The
// 'bind' functions return a 'StringRef' into the original string (so the
// characters of the string are not copied).  Note that this example builds on
// Example 1.
//
// First, we define a string reference of the parsed string to show that 'bind'
// works both on strings and string references:
//..
//  const bslstl::StringRef fullRef(full);
//..
// Then we demonstrate binding 'IndexSpan' object to that reference:
//..
//  assert(bdlb::IndexSpanStringUtil::bind(fullRef, first) == "James");
//
//  assert(bdlb::IndexSpanStringUtil::bind(fullRef, middle) == "Tiberius");
//
//  assert(bdlb::IndexSpanStringUtil::bind(fullRef, last) == "Kirk");
//..
// Finally we demonstrate binding 'IndexSpan' object to a string:
//..
//  assert(bdlb::IndexSpanStringUtil::bind(full, first) == "James");
//
//  assert(bdlb::IndexSpanStringUtil::bind(full, middle) == "Tiberius");
//
//  assert(bdlb::IndexSpanStringUtil::bind(full, last) == "Kirk");
//..

#include <bdlscm_version.h>

#include <bdlb_indexspan.h>

#include <bsls_assert.h>

#include <bsl_string.h>

namespace BloombergLP {
namespace bdlb {

                      // ==========================
                      // struct IndexSpanStringUtil
                      // ==========================

struct IndexSpanStringUtil {
    // This struct serves as a namespace for utility functions that operate on
    // 'IndexSpan' and string objects.

  private:
    // PRIVATE CLASS METHODS
    template <class CHAR_TYPE>
    static bslstl::StringRefImp<CHAR_TYPE> bindImp(
                                 const bslstl::StringRefImp<CHAR_TYPE>& string,
                                 const IndexSpan&                       span);
        // Return a string reference to the substring of the specified 'string'
        // as described by the specified 'span', meaning the substring starting
        // at the 'span.position()' index in 'string' and having
        // 'span.length()' characters.    The behavior is undefined unless
        // 'span.position() <= string.length()' and
        // 'span.position() + span.length() <= string.length()'.

    template <class CHAR_TYPE>
    static IndexSpan createFromPosImp(
                                 const bslstl::StringRefImp<CHAR_TYPE>& string,
                                 IndexSpan::size_type                   begin,
                                 IndexSpan::size_type                   end);
        // Return an 'IndexSpan' describing the substring of the specified
        // 'string' as starting at the specified 'begin' and ending at the
        // character preceding the specified 'end'.  The behavior is undefined
        // unless 'begin <= string.end()', 'end <= string.end()', and
        // 'begin <= end'.

    template <class CHAR_TYPE>
    static IndexSpan createImp(const bslstl::StringRefImp<CHAR_TYPE>& string,
                               IndexSpan::size_type                   begin,
                               IndexSpan::size_type                   length);
        // Return an 'IndexSpan' describing the substring of the specified
        // 'string' as starting at the specified 'begin' and having the
        // specified 'length'.  The behavior is undefined unless
        // 'begin <= string.length()' and 'begin + length <= string.length()'.

    template <class CHAR_TYPE>
    static IndexSpan createImp(
                             const bslstl::StringRefImp<CHAR_TYPE>& string,
                             const bslstl::StringRefImp<CHAR_TYPE>& subString);
        // Return an 'IndexSpan' describing the substring of the specified
        // 'string' as defined by the 'begin()' and 'end()' of the specified
        // 'subString'.  The behavior is undefined unless
        // 'subString.begin() <= string.end()',
        // 'subString.end() <= string.end()',
        // 'subString.begin() >= string.begin()', and
        // 'subString.end() <= string.end()'.

    template <class CHAR_TYPE>
    static IndexSpan createImp(
              const bslstl::StringRefImp<CHAR_TYPE>&                   string,
              typename bslstl::StringRefImp<CHAR_TYPE>::const_iterator begin,
              IndexSpan::size_type                                     length);
        // Return an 'IndexSpan' describing the substring of the specified
        // 'string' starting at the specified 'begin' and having the specified
        // 'length'.  The behavior is undefined unless'begin <= string.end()'
        // and 'begin + length <= string.end()'.

    template <class CHAR_TYPE>
    static IndexSpan createImp(
               const bslstl::StringRefImp<CHAR_TYPE>&                   string,
               typename bslstl::StringRefImp<CHAR_TYPE>::const_iterator begin,
               typename bslstl::StringRefImp<CHAR_TYPE>::const_iterator end);
        // Return an 'IndexSpan' describing the substring of the specified
        // 'string' starting at the specified 'begin' and ending (not
        // including) the specified 'end'.  The behavior is undefined unless
        // 'begin >= string.begin()', 'end >= string.begin()',
        // 'begin <= string.end()', 'end <= string.end()', and 'begin <= end'.

    template <class CHAR_TYPE, class TRAITS, class ALLOCATOR>
    static IndexSpan createImp(
                const bsl::basic_string<CHAR_TYPE, TRAITS, ALLOCATOR>& string,
                typename bsl::basic_string<CHAR_TYPE,
                                           TRAITS,
                                           ALLOCATOR>::const_iterator  begin,
                IndexSpan::size_type                                   length);
        // Return an 'IndexSpan' describing the substring of the specified
        // 'string' starting at the specified 'begin' and having the specified
        // 'length'.  The behavior is undefined unless
        // 'begin >= string.begin()', 'begin <= string.end()', and
        // 'begin + length <= string.end()'.

    template <class CHAR_TYPE, class TRAITS, class ALLOCATOR>
    static IndexSpan createImp(
                 const bsl::basic_string<CHAR_TYPE, TRAITS, ALLOCATOR>& string,
                 typename bsl::basic_string<CHAR_TYPE,
                                            TRAITS,
                                            ALLOCATOR>::const_iterator  begin,
                               typename bsl::basic_string<CHAR_TYPE,
                                            TRAITS,
                                            ALLOCATOR>::const_iterator  end);
        // Return an 'IndexSpan' describing the substring of the specified
        // 'string' starting at the specified 'begin' and ending (not
        // including) the specified 'end'.  The behavior is undefined unless
        // 'begin >= string.begin()', 'begin <= string.end()',
        // 'end <= string.end()', and 'begin <= end'.

  public:
    // CLASS METHODS
    static bslstl::StringRef     bind(const bslstl::StringRef&     string,
                                      const IndexSpan&             span);
    static bslstl::StringRefWide bind(const bslstl::StringRefWide& string,
                                      const IndexSpan&             span);
        // Return a string reference to the substring of the specified 'string'
        // as described by the specified 'span', meaning the substring starting
        // at the 'span.position()' index in 'string' and having
        // 'span.length()' characters.  The behavior is undefined unless
        // 'span.position() <= string.length()' and
        // 'span.position() + span.length() <= string.length()'.

    static IndexSpan createFromPositions(const bslstl::StringRef&     string,
                                         IndexSpan::size_type         begin,
                                         IndexSpan::size_type         end);
    static IndexSpan createFromPositions(const bslstl::StringRefWide& string,
                                         IndexSpan::size_type         begin,
                                         IndexSpan::size_type         end);
        // Return an 'IndexSpan' describing the substring of the specified
        // 'string' starting at the specified 'begin' and ending (not
        // including) the specified 'end'.  The behavior is undefined unless
        // 'begin <= string.length()' and 'end <= string.length()'.

    static IndexSpan create(const bslstl::StringRef&     string,
                            IndexSpan::size_type         begin,
                            IndexSpan::size_type         length);
    static IndexSpan create(const bslstl::StringRefWide& string,
                            IndexSpan::size_type         begin,
                            IndexSpan::size_type         length);
        // Return an 'IndexSpan' describing the substring of the specified
        // 'string' starting at the specified 'begin' and having the specified
        // 'length'.  The behavior is undefined unless
        // 'begin <= string.length()' and 'begin + length <= string.length()'.

    static IndexSpan create(const bslstl::StringRef&     string,
                            const bslstl::StringRef&     subString);
    static IndexSpan create(const bslstl::StringRefWide& string,
                            const bslstl::StringRefWide& subString);
        // Return an 'IndexSpan' describing the substring of the specified
        // 'string' as defined by the 'begin()' and 'end()' of the specified
        // 'subString'.  The behavior is undefined unless
        // 'subString.begin() <= string.end()',
        // 'subString.end() <= string.end()',
        // 'subString.begin() >= string.begin()', and
        // 'subString.end() <= string.begin()'.

    static IndexSpan create(const bslstl::StringRef&                   string,
                            bslstl::StringRef::const_iterator          begin,
                            IndexSpan::size_type                       length);
    static IndexSpan create(const bslstl::StringRefWide&               string,
                            bslstl::StringRefWide::const_iterator      begin,
                            IndexSpan::size_type                       length);
    static IndexSpan create(const bsl::string&                         string,
                            bsl::string::const_iterator                begin,
                            IndexSpan::size_type                       length);
    static IndexSpan create(const bsl::wstring&                        string,
                            bsl::wstring::const_iterator               begin,
                            IndexSpan::size_type                       length);
        // Return an 'IndexSpan' describing the substring of the specified
        // 'string' starting at the specified 'begin' and having the specified
        // 'length'.  The behavior is undefined unless
        // 'begin >= string.begin()', 'begin <= string.end()', and
        // 'begin + length <= string.end()'.

    static IndexSpan create(const bslstl::StringRef&              string,
                            bslstl::StringRef::const_iterator     begin,
                            bslstl::StringRef::const_iterator     end);
    static IndexSpan create(const bslstl::StringRefWide&          string,
                            bslstl::StringRefWide::const_iterator begin,
                            bslstl::StringRefWide::const_iterator end);
    static IndexSpan create(const bsl::string&                    string,
                            bsl::string::const_iterator           begin,
                            bsl::string::const_iterator           end);
    static IndexSpan create(const bsl::wstring&                   string,
                            bsl::wstring::const_iterator          begin,
                            bsl::wstring::const_iterator          end);
        // Return an 'IndexSpan' describing the substring of the specified
        // 'string' starting at the specified 'begin' and ending (not
        // including) the specified 'end'.  The behavior is undefined unless
        // 'begin >= string.begin()', 'begin <= string.end()',
        // 'end <= string.end()', and 'begin <= end'.
};

// ============================================================================
//                             INLINE DEFINITIONS
// ============================================================================

                       // --------------------------
                       // struct IndexSpanStringUtil
                       // --------------------------

// PRIVATE CLASS METHODS
template <class CHAR_TYPE>
inline
bslstl::StringRefImp<CHAR_TYPE>
IndexSpanStringUtil::bindImp(const bslstl::StringRefImp<CHAR_TYPE>& string,
                             const IndexSpan&                       span)
{
    BSLS_ASSERT(span.position()                 <= string.length());
    BSLS_ASSERT(span.position() + span.length() <= string.length());

    return bslstl::StringRefImp<CHAR_TYPE>(string.data() + span.position(),
                                           span.length());
}

template <class CHAR_TYPE>
inline
IndexSpan
IndexSpanStringUtil::createFromPosImp(
                                 const bslstl::StringRefImp<CHAR_TYPE>& string,
                                 IndexSpan::size_type                   begin,
                                 IndexSpan::size_type                   end)
{
    BSLS_ASSERT(begin <= string.size());
    BSLS_ASSERT(end   <= string.size());
    BSLS_ASSERT(begin <= end);

    return IndexSpan(begin, end - begin);
}

template <class CHAR_TYPE>
inline
IndexSpan
IndexSpanStringUtil::createImp(const bslstl::StringRefImp<CHAR_TYPE>& string,
                               IndexSpan::size_type                   begin,
                               IndexSpan::size_type                   length)
{
    BSLS_ASSERT(begin          <= string.size());
    BSLS_ASSERT(begin + length <= string.size());

    return IndexSpan(begin, length);
}

template <class CHAR_TYPE>
inline
IndexSpan
IndexSpanStringUtil::createImp(
                              const bslstl::StringRefImp<CHAR_TYPE>& string,
                              const bslstl::StringRefImp<CHAR_TYPE>& subString)
{
    BSLS_ASSERT(subString.begin() <= string.end());
    BSLS_ASSERT(subString.end()   <= string.end());
    BSLS_ASSERT(subString.begin() >= string.begin());
    BSLS_ASSERT(subString.end()   >= string.begin());

    return IndexSpan(subString.begin() - string.begin(), subString.length());
}
template <class CHAR_TYPE>
inline
IndexSpan
IndexSpanStringUtil::createImp(
              const bslstl::StringRefImp<CHAR_TYPE>&                   string,
              typename bslstl::StringRefImp<CHAR_TYPE>::const_iterator begin,
              IndexSpan::size_type                                     length)
{
    BSLS_ASSERT(begin          >= string.begin());
    BSLS_ASSERT(begin          <= string.end());
    BSLS_ASSERT(begin + length <= string.end());

    return IndexSpan(begin - string.begin(), length);
}

template <class CHAR_TYPE>
inline
IndexSpan
IndexSpanStringUtil::createImp(
               const bslstl::StringRefImp<CHAR_TYPE>&                   string,
               typename bslstl::StringRefImp<CHAR_TYPE>::const_iterator begin,
               typename bslstl::StringRefImp<CHAR_TYPE>::const_iterator end)
{
    BSLS_ASSERT(begin >= string.begin());
    BSLS_ASSERT(begin <= end);
    BSLS_ASSERT(begin <= string.end());
    BSLS_ASSERT(end   <= string.end());

    return IndexSpan(begin - string.begin(), end - begin);
}

template <class CHAR_TYPE, class TRAITS, class ALLOCATOR>
inline
IndexSpan
IndexSpanStringUtil::createImp(
                 const bsl::basic_string<CHAR_TYPE, TRAITS, ALLOCATOR>& string,
                 typename bsl::basic_string<CHAR_TYPE,
                                             TRAITS,
                                             ALLOCATOR>::const_iterator begin,
                 IndexSpan::size_type                                   length)
{
    BSLS_ASSERT(begin          >= string.begin());
    BSLS_ASSERT(begin          <= string.end());
    BSLS_ASSERT(begin + length <= string.end());

    return IndexSpan(begin - string.begin(), length);
}

template <class CHAR_TYPE, class TRAITS, class ALLOCATOR>
inline
IndexSpan
IndexSpanStringUtil::createImp(
                 const bsl::basic_string<CHAR_TYPE, TRAITS, ALLOCATOR>& string,
                 typename bsl::basic_string<CHAR_TYPE,
                                             TRAITS,
                                             ALLOCATOR>::const_iterator begin,
                 typename bsl::basic_string<CHAR_TYPE,
                                             TRAITS,
                                             ALLOCATOR>::const_iterator end)
{
    BSLS_ASSERT(begin >= string.begin());
    BSLS_ASSERT(begin <= end);
    BSLS_ASSERT(begin <= string.end());
    BSLS_ASSERT(end   <= string.end());

    return IndexSpan(begin - string.begin(), end - begin);
}


// CLASS METHODS
inline
bslstl::StringRef
IndexSpanStringUtil::bind(const bslstl::StringRef& string,
                          const IndexSpan&         span)
{
    return bindImp(string, span);
}

inline
bslstl::StringRefWide
IndexSpanStringUtil::bind(const bslstl::StringRefWide& string,
                          const IndexSpan&             span)
{
    return bindImp(string, span);
}

inline
IndexSpan
IndexSpanStringUtil::createFromPositions(const bslstl::StringRef& string,
                                         IndexSpan::size_type     begin,
                                         IndexSpan::size_type     end)
{
    return createFromPosImp(string, begin, end);
}

inline
IndexSpan
IndexSpanStringUtil::createFromPositions(const bslstl::StringRefWide& string,
                                         IndexSpan::size_type         begin,
                                         IndexSpan::size_type         end)
{
    return createFromPosImp(string, begin, end);
}

inline
IndexSpan
IndexSpanStringUtil::create(const bslstl::StringRef& string,
                            IndexSpan::size_type     begin,
                            IndexSpan::size_type     length)
{
    return createImp(string, begin, length);
}

inline
IndexSpan
IndexSpanStringUtil::create(const bslstl::StringRefWide& string,
                            IndexSpan::size_type         begin,
                            IndexSpan::size_type         length)
{
    return createImp(string, begin, length);
}

inline
IndexSpan
IndexSpanStringUtil::create(const bslstl::StringRef& string,
                            const bslstl::StringRef& subString)
{
    return createImp(string, subString);
}

inline
IndexSpan
IndexSpanStringUtil::create(const bslstl::StringRefWide& string,
                            const bslstl::StringRefWide& subString)
{
    return createImp(string, subString);
}

inline
IndexSpan
IndexSpanStringUtil::create(const bslstl::StringRef&          string,
                            bslstl::StringRef::const_iterator begin,
                            IndexSpan::size_type              length)
{
    return createImp(string, begin, length);
}

inline
IndexSpan
IndexSpanStringUtil::create(const bslstl::StringRefWide&          string,
                            bslstl::StringRefWide::const_iterator begin,
                            IndexSpan::size_type                  length)
{
    return createImp(string, begin, length);
}

inline
IndexSpan
IndexSpanStringUtil::create(const bsl::string&          string,
                            bsl::string::const_iterator begin,
                            IndexSpan::size_type        length)
{
    return createImp(string, begin, length);
}

inline
IndexSpan
IndexSpanStringUtil::create(const bsl::wstring&          string,
                            bsl::wstring::const_iterator begin,
                            IndexSpan::size_type         length)
{
    return createImp(string, begin, length);
}

inline
IndexSpan
IndexSpanStringUtil::create(const bslstl::StringRef&          string,
                            bslstl::StringRef::const_iterator begin,
                            bslstl::StringRef::const_iterator end)
{
    return createImp(string, begin, end);
}

inline
IndexSpan
IndexSpanStringUtil::create(const bslstl::StringRefWide&          string,
                            bslstl::StringRefWide::const_iterator begin,
                            bslstl::StringRefWide::const_iterator end)
{
    return createImp(string, begin, end);
}

inline
IndexSpan
IndexSpanStringUtil::create(const bsl::string&          string,
                            bsl::string::const_iterator begin,
                            bsl::string::const_iterator end)
{
    return createImp(string, begin, end);
}

inline
IndexSpan
IndexSpanStringUtil::create(const bsl::wstring&          string,
                            bsl::wstring::const_iterator begin,
                            bsl::wstring::const_iterator end)
{
    return createImp(string, begin, end);
}

}  // close package namespace
}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2018 Bloomberg Finance L.P.
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
