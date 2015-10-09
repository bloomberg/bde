// bslstl_stringrefdata.h                                             -*-C++-*-
#ifndef INCLUDED_BSLSTL_STRINGREFDATA
#define INCLUDED_BSLSTL_STRINGREFDATA

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide an attribute-only base class for 'bslstl::StringRef'.
//
//@CLASSES:
//  bslstl::StringRefData: attribute class referencing a sub-string.
//
//@DESCRIPTION: This component provides a complex-constrained, in-core
// (value-semantic) attribute class (except for the equality comparison, see
// below), 'bslstl::StringRefData', that represents a reference to character
// string data.  Note that 'bslstl::StringRefData' is intended for use as a
// base class for 'bslstl::StringRef' and as parameter of 'bsl::string'
// constructor, enabling a conversion from 'bslstl::StringRef' to 'bsl::string'
// without having a cyclic dependency among these three classes.
//
// The dependencies between these components are shown on the following
// diagram:
//..
//            +-----------------------+
//            |  ,----------------.  3|
//            | ( bslstl::StringRef ) |
//            |  `o-------|-------'   |
//            +--/--------|-----------+
//              /         |
//  +----------/-+        |
//  |  ,------. 2|        |
//  | ( string ) |        |
//  |  `---o--'  |        |
//  +-------\----+        |
//           \            |
//       +----\-----------V----------+
//       |  ,--------------------.  1|
//       | ( bslstl::StringRefData ) |
//       |  `--------------------'   |
//       +---------------------------+
//..
//
// 'bslstl::StringRefData' does not define the equality comparison because its
// behavior would conflict with the behavior of the equality comparison in the
// derived 'bslstl::StringRef' class.
//
// A 'bslstl::StringRefData' object holds two pointers; 'begin' points to the
// first character of a contiguous array of characters forming a string, and
// 'end' points to an address one past the last character in the string.  If
// 'begin' and 'end' are equal the string is empty.  Note that the referenced
// string may not be null terminated, and may contain embedded nulls.
//
///Attributes
///----------
//..
//  Name   Type               Default  Constraints
//  -----  -----------------  -------  ------------------------------
//  begin  const CHAR_TYPE *  0        begin <= end && !begin == !end
//  end    const CHAR_TYPE *  0        begin <= end && !begin == !end
//..
//: o begin: a pointer to the first character of the string.
//:
//: o end: a pointer to one past the last character of the string.
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

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

namespace BloombergLP {

namespace bslstl {

                         // ===================
                         // class StringRefData
                         // ===================

template <class CHAR_TYPE>
class StringRefData {
    // This complex-constrained in-core (value-semantic) attribute class
    // represents a reference to character string data.  See the Attributes
    // section under @DESCRIPTION in the component-level documentation for
    // information on the class attributes.  Note that the class invariants are
    // identically the constraints on the individual attributes.
    //
    // This class:
    //: o supports a complete set of *value-semantic* operations
    //:   o except for 'bdex' serialization
    //: o is *exception-neutral* (agnostic)
    //: o is *alias-safe*
    //: o is 'const' *thread-safe*
    // For terminology see 'bsldoc_glossary'.

    // DATA
    const CHAR_TYPE *d_begin_p; // address of first character of a string or 0

    const CHAR_TYPE *d_end_p;   // address one past last character of a string
                                // or 0 if 'd_begin_p==0'

  public:
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

    //! StringRefData(const StringRefData&) = default;
    //! ~StringRefData() = default;

    // MANIPULATORS
    //! StringRefData& operator=(const StringRefData&) = default;

    // ACCESSORS
    const CHAR_TYPE *begin() const;
        // Return the address of the first character of the string or 0 for
        // empty string.  Empty string value is represented as (0, 0).

    const CHAR_TYPE *end() const;
        // Return the address past the end of the string or 0 for empty string.
        // Empty string value is represented as (0, 0).
};

// ============================================================================
//                       TEMPLATE FUNCTION DEFINITIONS
// ============================================================================

                         // -------------------
                         // class StringRefData
                         // -------------------

// CREATORS
template <class CHAR_TYPE>
inline
StringRefData<CHAR_TYPE>::StringRefData()
: d_begin_p(0)
, d_end_p(0)
{
}

template <class CHAR_TYPE>
inline
StringRefData<CHAR_TYPE>
    ::StringRefData(const CHAR_TYPE *begin,
                    const CHAR_TYPE *end)
: d_begin_p(begin)
, d_end_p(end)
{
    BSLS_ASSERT_SAFE(d_begin_p <= d_end_p);
    BSLS_ASSERT_SAFE(!d_begin_p == !d_end_p);
}

// ACCESSORS
template <class CHAR_TYPE>
inline
const CHAR_TYPE *StringRefData<CHAR_TYPE>::begin() const
{
    return d_begin_p;
}

template <class CHAR_TYPE>
inline
const CHAR_TYPE *StringRefData<CHAR_TYPE>::end() const
{
    return d_end_p;
}

}  // close package namespace


}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2013 Bloomberg Finance L.P.
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
