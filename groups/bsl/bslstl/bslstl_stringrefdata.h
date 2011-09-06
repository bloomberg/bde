// bslstl_stringrefdata.h                                             -*-C++-*-
#ifndef INCLUDED_BSLSTL_STRINGREFDATA
#define INCLUDED_BSLSTL_STRINGREFDATA

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a base class for 'bslstl_StringRef'
//
//@CLASSES:
//  bslstl_StringRefData: a base class for 'bslstl_StringRef'
//
//@AUTHOR: Alexei Zakharov (azakhar1)
//
//@DESCRIPTION: This component provides a complex-constrained in-core
// (value-semantic) attribute class, 'bslstl_StringRefData', that represents a
// reference to character string data.  Note that 'bslstl_StringRefData' is
// used as a base class for 'bslstl_StringRef' and as an argument to
// 'bsl::string' constructor enabling a convertion from 'bslstl_StringRef' to
// 'bsl::string' without a circular dependency between these two classes.
//
// A 'bslstl_StringRefData' object holds two pointers; 'begin' points to the
// first character of a contiguous array of characters forming a string, and
// 'end' points to an address one past the last character in the string.  If
// 'begin' and 'end' are equal the string is empty.  The referenced string may
// not be null terminated, and may contain embedded nulls.
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
// In this section we show intended usage of this component
//
///Example 1: Finding the Position of One String Inside Another
///- - - - - - - - - - - - - - - - - - - -
// In this example we demonstrate how to search for the first occurrence of one
// string in another using 'bslstl_StringRefData' objects to represent the
// strings.
//
// First, we define a function, 'findSubstring', that takes a string to be
// searched, a string to search for, and returns the position of the second
// string inside the first string:
//..
//  const char *findSubstring(const bslstl_StringRefData<char>& string,
//                            const bslstl_StringRefData<char>& substr)
//  {
//      ptrdiff_t count1 = string.end() - string.begin();
//      ptrdiff_t count2 = substr.end() - substr.begin();
//
//      for (const char *p1 = string.begin();
//           count1 >= count2;
//           ++p1, --count1)
//      {
//          const char *q1 = p1;
//
//          for (const char *q2 = substr.begin(); ; ++q1, ++q2)
//          {
//              if (q2 == substr.end()) {
//                  return p1;
//              }
//              else {
//                  if (*q1 != *q2) {
//                      break;
//                  }
//              }
//          }
//      }
//
//      return string.end();
//  }
//..
// Notice that we're using 'begin' and 'end' attributes of the
// 'bslstl_StringRefData' object to access the string characters.
//
// Now, we call the function we just defined with two string literal arguments:
//..
//  const char string[] = "find substring";
//  const char substr[] = "ring";
//  const char * pos    =
//      findSubstring(
//          bslstl_StringRefData<char>(string, string + sizeof(string)),
//          bslstl_StringRefData<char>(substr, substr + sizeof(substr)));
//..
// Finally, we check that the function produced the correct result:
//..
//  assert(pos == string + 10);
//..

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

namespace BloombergLP {

                         // ==========================
                         // class bslstl_StringRefData
                         // ==========================

template <typename CHAR_TYPE>
class bslstl_StringRefData {
    // This complex-constrained in-core (value-semantic) attribute class
    // represents a reference to character string data.  See the Attributes
    // section under @DESCRIPTION in the component-level documentation for
    // information on the class attributes.  Note that the class invariants are
    // identically the constraints on the individual attributes.
    //
    // This class:
    //: o supports a complete set of *value-semantic* operations
    //:   o except for 'bdex' serialization
    //: o is *exception-neutral* (agnostic) TBD
    //: o is *alias-safe*
    //: o is 'const' *thread-safe*
    // For terminology see 'bsldoc_glossary'.

    // DATA
    const CHAR_TYPE *d_begin;   // address of first character of a string or 0

    const CHAR_TYPE *d_end;     // address one past last character of a string
                                // or 0 if 'd_begin==0'

  public:
    // CREATORS
    bslstl_StringRefData();
        // Create a 'bslstl_StringRefData' object the default attribute values:
        //..
        //  begin() == 0
        //  end()   == 0
        //..

    bslstl_StringRefData(const CHAR_TYPE *begin, const CHAR_TYPE *end);
        // Create a 'bslstl_StringRefData' object the specified 'begin' and
        // 'end' attribute values.  The behavior is undefined unless 'begin <=
        // end' and '!begin == !end'.

    //! bslstl_StringRefData(const bslstl_StringRefData&) = default;
    //! ~bslstl_StringRefData() = default;

    // MANIPULATORS
    //! bslstl_StringRefData& operator=(const bslstl_StringRefData&) = default;

    // ACCESSORS
    const CHAR_TYPE *begin() const;
        // Return the address of the first character of the string.  Note that
        // the return value can be 0, in which case 'end()' returns 0 as well.

    const CHAR_TYPE *end() const;
        // Return the address past the end of the string.  Note that the return
        // value can be 0, in which case 'begin()' returns 0 as well.
};

// FREE OPERATORS
template <typename CHAR_TYPE>
bool operator==(const bslstl_StringRefData<CHAR_TYPE>& lhs,
                const bslstl_StringRefData<CHAR_TYPE>& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects have the same
    // value, and 'false' otherwise.  Two 'bslstl_StringRefData' objects have
    // the same value if the corresponding values of their 'begin' and 'end'
    // attributes are the same.

template <typename CHAR_TYPE>
bool operator!=(const bslstl_StringRefData<CHAR_TYPE>& lhs,
                const bslstl_StringRefData<CHAR_TYPE>& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects do not have the
    // same value, and 'false' otherwise.  Two 'bslstl_StringRefData' objects
    // do not have the same value if the corresponding values of their 'begin'
    // and 'end' attributes are not the same.

// ===========================================================================
//                      TEMPLATE FUNCTION DEFINITIONS
// ===========================================================================

                         // --------------------------
                         // class bslstl_StringRefData
                         // --------------------------

// CREATORS
template <typename CHAR_TYPE>
inline
bslstl_StringRefData<CHAR_TYPE>::bslstl_StringRefData()
: d_begin(0)
, d_end(0)
{
}

template <typename CHAR_TYPE>
inline
bslstl_StringRefData<CHAR_TYPE>
    ::bslstl_StringRefData(const CHAR_TYPE *begin,
                                const CHAR_TYPE *end)
: d_begin(begin)
, d_end(end)
{
    BSLS_ASSERT_SAFE(d_begin <= d_end);
    BSLS_ASSERT_SAFE(!d_begin == !d_end);
}

// ACCESSORS
template <typename CHAR_TYPE>
inline
const CHAR_TYPE *bslstl_StringRefData<CHAR_TYPE>::begin() const
{
    return d_begin;
}

template <typename CHAR_TYPE>
inline
const CHAR_TYPE *bslstl_StringRefData<CHAR_TYPE>::end() const
{
    return d_end;
}

// FREE OPERATORS
template <typename CHAR_TYPE>
inline
bool operator==(const bslstl_StringRefData<CHAR_TYPE>& lhs,
                const bslstl_StringRefData<CHAR_TYPE>& rhs)
{
    return lhs.begin() == rhs.begin() && lhs.end() == rhs.end();
}

template <typename CHAR_TYPE>
inline
bool operator!=(const bslstl_StringRefData<CHAR_TYPE>& lhs,
                const bslstl_StringRefData<CHAR_TYPE>& rhs)
{
    return !(lhs == rhs);
}

}  // close enterprise namespace


#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2011
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
