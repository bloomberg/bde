// bslstl_stringrefdata.h                                             -*-C++-*-
#ifndef INCLUDED_BSLSTL_STRINGREFDATA
#define INCLUDED_BSLSTL_STRINGREFDATA

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a base class for 'bslstl::StringRef'.
//
//@CLASSES:
//  bslstl::StringRefData: intermediate class providing StringRef compatibility
//
//@DESCRIPTION: Initially, this component provided a complex-constrained,
// in-core (value-semantic) attribute class, 'bslstl::StringRefData', that
// represented a reference to character string data.  Note that
// 'bslstl::StringRefData' was intended for use as a base class for
// 'bslstl::StringRef' and as parameter of 'bsl::string' constructor, enabling
// a conversion from 'bslstl::StringRef' to 'bsl::string' without having a
// cyclic dependency among these three classes.
//
// But nowadays it only provides compatibility between the two constructors of
// 'bsl::string' (implicit constructor, accepting 'StringRefData' object by
// value and explicit constructor, accepting 'bsl::string_view' object by
// value) to allow existing users of 'bslstl::StringRef' not to change their
// code.
//
// The dependencies between these components are shown on the following
// diagram:
//..
//                              +-----------------------+
//                              |                       |
//                     /--------o   bslstl::StringRef   |
//                    /         |                       |
//                   /          +-----------------------+
//                  /                      |
//                 /                       |
//  +------------------+        +----------V------------+
//  |                  |        |                       |
//  |   bsl::string    o--------| bslstl::StringRefData |
//  |                  |        |                       |
//  +--------------o---+        +-----------------------+
//                  \                      |
//                   \                     |
//                    \         +----------V------------+
//                     \        |                       |
//                      \-------|    bsl::string_view   |
//                              |                       |
//                              +-----------------------+
//..
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Breaking Cyclic Dependency Between 'String' and 'StringRef'
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// In this example we demonstrate how 'bslstl::StringRefData' allows us to
// break the cyclic dependency between hypothetical 'String' and 'StringRef'
// classes.
//
// Objects of our 'String' and 'StringRef' classes need to be convertible to
// each other.  However only one of these classes can depend on the definition
// of the other one, otherwise they will be cyclically dependent.
//
// First, we define a hypothetical 'String' class, whose implementation is
// intentionally simple and contains only the essential constructors and
// accessor methods; the important thing to notice is that 'String' does not
// depend on 'StringRef', which has not been defined yet:
//..
//  namespace Usage {
//
//  class String {
//    private:
//      const char *d_begin_p;
//      const char *d_end_p;
//
//    public:
//      typedef const char *const_iterator;
//
//      String(bslstl::StringRefData<char> const& stringRef)
//          : d_begin_p(stringRef.begin())
//          , d_end_p(stringRef.end())
//      {
//      }
//
//      const_iterator begin() const
//      {
//          return d_begin_p;
//      }
//
//      const_iterator end() const
//      {
//          return d_end_p;
//      }
//  };
//..
// Notice that the constructor of 'String' takes a 'bslstl::StringRefData'
// argument and then uses its members 'begin' and 'end' to initialize the
// string object.
//
// Then, we define a hypothetical 'StringRef' class, whose can be initialized
// either with a 'String' object (to enable the conversion from 'String' to
// 'StringRef') or with two 'const char *' pointers:
//..
//  class StringRef : public bslstl::StringRefData<char>
//  {
//    public:
//      StringRef(const char *begin, const char *end)
//          : bslstl::StringRefData<char>(begin, end)
//      {}
//
//      StringRef(const String& str)
//          : bslstl::StringRefData<char>(&*str.begin(), &*str.end())
//      {}
//  };
//
//  }  // close namespace Usage
//..
// Note that 'StringRef' also derives from 'bslstl::StringRefData' so that an
// object of 'StringRef' can be passed to the constructor of 'String' as
// reference to 'bslstl::StringRefData', which enables the conversion from
// 'StringRef' to 'String'.
//
// Finally, we verify that the conversions between 'String' and 'StringRef'
// work:
//..
//  using Usage::String;
//  using Usage::StringRef;
//
//  const char str[] = "test string";
//  StringRef  strRef(str, str + sizeof(str));
//
//  String     strObj = strRef;     // convert 'StringRef' to 'String'
//  StringRef  strRf2 = strObj;     // convert 'String' to 'StringRef'
//
//  assert(&*strObj.begin() == strRef.begin());
//  assert(&*strObj.end()   == strRef.end());
//  assert(&*strObj.begin() == strRf2.begin());
//  assert(&*strObj.end()   == strRf2.end());
//..

#include <bslscm_version.h>

#include <bslstl_stringview.h>

#include <bslmf_istriviallycopyable.h>
#include <bslmf_nestedtraitdeclaration.h>

#include <bsls_assert.h>
#include <bsls_nativestd.h>

#include <iosfwd>

namespace BloombergLP {

namespace bslstl {

                         // ===================
                         // class StringRefData
                         // ===================

template <class CHAR_TYPE>
class StringRefData : public bsl::basic_string_view<CHAR_TYPE> {
    // This class is an intermediate link between 'bslstl::StringRef' used in
    // BDE and 'bsl::string_view' and is kept only for compatibility with
    // legacy code.

  private:
    // PRIVATE TYPES
    typedef bsl::basic_string_view<CHAR_TYPE> Base;

  public:
    // CLASS METHODS
    static native_std::size_t cStringLength(const CHAR_TYPE *data);
        // Return the number of 'CHAR_TYPE' characters in the specified
        // null-terminated 'data' string, up to but not including the terminal
        // null value.

    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(StringRefData, bsl::is_trivially_copyable);

    // CREATORS
    StringRefData();
        // Create a 'StringRefData' object having default attribute values:
        //..
        //  begin() == 0
        //  end()   == 0
        //..

    StringRefData(const CHAR_TYPE *begin, const CHAR_TYPE *end);
        // Create a 'StringRefData' object the specified 'begin' and 'end'
        // attribute values.  The behavior is undefined unless 'begin <= end'
        // and '!begin == !end'.

    StringRefData(const bsl::basic_string_view<CHAR_TYPE>& view);
        // Create a 'StringRefData' object from the specified 'view'.

    //! StringRefData(const StringRefData&) = default;
    //! ~StringRefData() = default;

    // MANIPULATORS
    //! StringRefData& operator=(const StringRefData&) = default;

};

// ============================================================================
//                       TEMPLATE FUNCTION DEFINITIONS
// ============================================================================

                         // -------------------
                         // class StringRefData
                         // -------------------

// CLASS METHODS
template <class CHAR_TYPE>
inline
native_std::size_t StringRefData<CHAR_TYPE>::cStringLength(
                                                         const CHAR_TYPE *data)
{
    BSLS_ASSERT_SAFE(data);

    return native_std::char_traits<CHAR_TYPE>::length(data);
}

// CREATORS
template <class CHAR_TYPE>
inline
StringRefData<CHAR_TYPE>::StringRefData()
: Base()
{
}

template <class CHAR_TYPE>
inline
StringRefData<CHAR_TYPE>
    ::StringRefData(const CHAR_TYPE *begin,
                    const CHAR_TYPE *end)
: Base(begin, end - begin)
{
    BSLS_ASSERT_SAFE(begin <= end);
    BSLS_ASSERT_SAFE(!begin == !end);
}

template <class CHAR_TYPE>
inline
StringRefData<CHAR_TYPE>
    ::StringRefData(const bsl::basic_string_view<CHAR_TYPE>& view)
: Base(view)
{
}

}  // close package namespace

#ifndef BDE_OPENSOURCE_PUBLICATION  // BACKWARD_COMPATIBILITY
// ============================================================================
//                           BACKWARD COMPATIBILITY
// ============================================================================

#ifdef bslstl_StringRefData
#undef bslstl_StringRefData
#endif
#define bslstl_StringRefData bslstl::StringRefData
    // This alias is defined for backward compatibility.
#endif  // BDE_OPENSOURCE_PUBLICATION -- BACKWARD_COMPATIBILITY

}  // close enterprise namespace

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
