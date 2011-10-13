// bslstl_stringrefdata.h                                             -*-C++-*-
#ifndef INCLUDED_BSLSTL_STRINGREFDATA
#define INCLUDED_BSLSTL_STRINGREFDATA

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a base class for 'bslstl_StringRef'.
//
//@CLASSES:
//  bslstl_StringRefData: a base class for 'bslstl_StringRef'
//
//@AUTHOR: Alexei Zakharov (azakhar1)
//
//@DESCRIPTION: This component provides a complex-constrained in-core
// (value-semantic) attribute class, 'bslstl_StringRefData', that represents a
// reference to character string data.  Note that 'bslstl_StringRefData' is
// intended for use as a base class for 'bslstl_StringRef' and as an argument
// to 'bsl::string' constructor, enabling a convertion from 'bslstl_StringRef'
// to 'bsl::string' without a circular dependency between these two classes.
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
// This section illustrates intended usage of this component.
//
///Example 1: Breaking circular dependency between string and StringRef classes
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// In this example we demonstrace how 'bslstl_StringRefData' allows to break
// the circular dependency between a hypothetical 'string' and 'StringRef'
// classes.
//
// Objects of our hypothetical 'string' and 'StringRef' classes need to be
// convertable to each other.  However only one of these classes can depend on
// the definition of another class, otherwise they will be circularly
// dependent.
//
// First, we define a hypothetical 'string' class.  Its implementation is
// intentionally simple and constains only the essential constructors and
// accessor methods.  The impotant thing to notice is that 'string' doesn't
// depend on 'StringRef', which hasn't been defined yet.
//..
//  namespace Usage {
//
//  class string {
//    private:
//      const char *d_begin;
//      const char *d_end;
//
//      void construct(const char *begin, const char *end)
//      {
//          d_begin = begin;
//          d_end = end;
//      }
//
//    public:
//      typedef const char * const_iterator;
//
//      string(bslstl_StringRefData<char> const& stringRef)
//      {
//          construct(stringRef.begin(), stringRef.end());
//      }
//
//      const_iterator begin() const
//      {
//          return d_begin;
//      }
//
//      const_iterator end() const
//      {
//          return d_end;
//      }
//  };
//..
// Notice that the constructor of 'string' takes a 'bslstl_StringRefData'
// argument and then uses its members 'begin' and 'end' to initialize the
// string object.
//
// Then, we define a hypothetical 'StringRef' class.  It can be initialized
// with two 'const char *' pointers or with a 'string' object to enable the
// conversion from 'string' to 'StringRef'.  It also derives from
// 'bslstl_StringRefData' so that an object of 'StringRef' can be passed to the
// constructor of 'string' as reference to 'bslstl_StringRefData', which
// enables the conversion from 'StringRef' to 'string'.
//..
//  class StringRef : public bslstl_StringRefData<char>
//  {
//    public:
//      StringRef(const char *begin, const char *end)
//          : bslstl_StringRefData<char>(begin, end)
//      {}
//
//      StringRef(const string& str)
//          : bslstl_StringRefData<char>(&*str.begin(), &*str.end())
//      {}
//  };
//
//  }  // close namespace Usage
//..
// Finally, we verify that the conversions between 'string' and "StringRef'
// work:
//..
//  using Usage::string;
//  using Usage::StringRef;
//
//  const char str[] = "test string";
//  StringRef  strRef(str, str + sizeof(str));
//
//  string     strObj = strRef;     // convert 'StringRef' to 'string'
//  StringRef  strRf2 = strObj;     // convert 'string' to 'StringRef'
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
    //: o is *exception-neutral* (agnostic)
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
        // Return the address of the first character of the string or 0 if the
        // string is null.  Note that if the return value is 0, 'end()' returns
        // 0 as well.

    const CHAR_TYPE *end() const;
        // Return the address past the end of the string or 0 if the string is
        // null.  Note that if the return value is 0, 'begin()' returns 0 as
        // well.
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
