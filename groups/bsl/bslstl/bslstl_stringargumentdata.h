// bslstl_stringargumentdata.h                                        -*-C++-*-
#ifndef INCLUDED_BSLSTL_STRINGARGUMENTDATA
#define INCLUDED_BSLSTL_STRINGARGUMENTDATA

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a base class for 'bslstl_StringArgument'
//
//@CLASSES:
//  bslstl_StringArgumentData: a base class for 'bslstl_StringArgument'
//
//@AUTHOR: Alexei Zakharov (azakhar1)
//
//@DESCRIPTION: This component provides a complex-constrained in-core
// (value-semantic) attribute class, 'bslstl_StringArgumentData', that is used
// as a simple base class for 'bslstl_StringArgument' and surves as a data
// container for 'bslstl_StringArgument' without providing much of the
// functionality of its own.  This allows to use 'bslstl_StringArgumentData' in
// the 'bsl::string' class and enable the convertion from
// 'bslstl_StringArgument' to 'bsl::string'.  Without this class
// 'bslstl_StringArgument' and 'bsl::string' would have a circular dependency
// on each other.
//
// 'bslstl_StringArgumentData' holds two pointers: a pointer to the start of a
// string and a pointer to the end of the string.  It's parameterized with type
// 'CHAR_TYPE' and its supposed to work with strings composed of 'CHAR_TYPE'
// characters.
//
///Attributes
///----------
//..
//  Name   Type               Default  Constraints
//  -----  -----------------  -------  --------------------------------
//  begin  const CHAR_TYPE *  NULL     begin <= end && (!begin == !end)
//  end    const CHAR_TYPE *  NULL     begin <= end && (!begin == !end)
//..
//: o begin: a pointer to the start of the string.
//:
//: o end: a pointer to the end of the string.
//
///Usage
///-----
// In this section we show intended usage of this component.
//
///Example 1: Computing a hash of a string
///- - - - - - - - - - - - - - - - - - - -
// Let's suppose we need to compute a hash of a string which is defined by two
// pointers: to the start and to the end of the string.
//
// First, we define a function 'computeHash' that takes a
// 'bslstl_StringArgumentData' string as an argument and returns an
// 'unsigned int' hash of that string:
//..
//  unsigned computeHash(const bslstl_StringArgumentData<char>& str)
//  {
//      unsigned hash = 3069134613U;
//
//      for (const char *p = str.begin(); p != str.end(); ++p)
//          hash = (hash << 5) ^ (hash >> 27) ^ *p;
//
//      return hash;
//  }
//..
// Note that we're using 'begin' and 'end' attributes of the
// 'bslstl_StringArgumentData' object to access the string characters.
//
// Then, we call it with a simple 'C string' argument:
//..
//      const char str[] = "C string";
//      unsigned hash = computeHash(bslstl_StringArgumentData<char>(
//                                                    str, str + sizeof(str)));
//..
// Finally, we compare the computed hash with the expected value:
//..
//      assert(hash == 3354902561U);
//..

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

namespace BloombergLP {

                     // ===============================
                     // class bslstl_StringArgumentData
                     // ===============================

template <typename CHAR_TYPE>
class bslstl_StringArgumentData
    // This is a base class for 'bslstl_StringArgument'.  It's defined here to
    // break a circular dependency between 'bslstl_StringArgument' and
    // 'bsl::string'.  'bsl::string' has a constructor that takes
    // 'const bslstl_StringArgumentData&' parameter, so that an object of the
    // derived 'bslstl_StringArgument' class can be passed to that constructor.
    // This is the only valid use of this class.
{
  private:
    // PRIVATE DATA
    const CHAR_TYPE *d_begin;  // address of first character in bound string
                               // (held, not owned), or 0 if unbound

    const CHAR_TYPE *d_end; // address one past last character in bound string,
                            // or 0 if unbound

  public:
    // CREATORS
    bslstl_StringArgumentData();
        // Construct a value-initialized 'bslstl_StringArgumentData' object
        // with both 'd_begin' and 'd_end' pointers assigned a 'NULL' value.

    bslstl_StringArgumentData(const CHAR_TYPE *begin, const CHAR_TYPE *end);
        // Construct a 'bslstl_StringArgumentData' object with the specified
        // 'begin' and 'end' pointers to the start and end of a string.  The
        // behavior is undefined unless 'begin <= end'.  Both 'begin' and 'end'
        // can be NULL.

    //! bslstl_StringArgumentData(const bslstl_StringArgumentData&) = default;
    //! ~bslstl_StringArgumentData() = default;

    // MANIPULATORS
    //! bslstl_StringArgumentData& operator=(const bslstl_StringArgumentData&)
    //                                                               = default;

    // ACCESSORS
    const CHAR_TYPE *begin() const;
        // Return the pointer to the start of the string.  Note that the return
        // value can be 'NULL', in which case 'end()' returns 'NULL' as well.

    const CHAR_TYPE *end() const;
        // Return the pointer past the end of the string.  Note that the return
        // value can be 'NULL, in which case 'begin()' returns 'NULL' as well.
};

// FREE OPERATORS
template <typename CHAR_TYPE>
bool operator==(const bslstl_StringArgumentData<CHAR_TYPE>& lhs,
                const bslstl_StringArgumentData<CHAR_TYPE>& rhs);
    // Return 'true' if the 'begin' and 'end' pointers of 'lhs' and 'rhs'
    // compare equal and 'false' otherwise.

template <typename CHAR_TYPE>
bool operator!=(const bslstl_StringArgumentData<CHAR_TYPE>& lhs,
                const bslstl_StringArgumentData<CHAR_TYPE>& rhs);
    // Return 'true' if either the 'begin' or 'end' pointers of 'lhs' and 'rhs'
    // do not compare equal and 'false' otherwise.

// ==========================================================================
//                      TEMPLATE FUNCTION DEFINITIONS
// ==========================================================================

                     // -------------------------------
                     // class bslstl_StringArgumentData
                     // -------------------------------

// CREATORS
template <typename CHAR_TYPE>
inline
bslstl_StringArgumentData<CHAR_TYPE>::bslstl_StringArgumentData()
: d_begin(0)
, d_end(0)
{
}

template <typename CHAR_TYPE>
inline
bslstl_StringArgumentData<CHAR_TYPE>
    ::bslstl_StringArgumentData(const CHAR_TYPE *begin,
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
const CHAR_TYPE *bslstl_StringArgumentData<CHAR_TYPE>::begin() const
{
    return d_begin;
}

template <typename CHAR_TYPE>
inline
const CHAR_TYPE *bslstl_StringArgumentData<CHAR_TYPE>::end() const
{
    return d_end;
}

// FREE OPERATORS
template <typename CHAR_TYPE>
inline
bool operator==(const bslstl_StringArgumentData<CHAR_TYPE>& lhs,
                const bslstl_StringArgumentData<CHAR_TYPE>& rhs)
{
    return lhs.begin() == rhs.begin() && lhs.end() == rhs.end();
}

template <typename CHAR_TYPE>
inline
bool operator!=(const bslstl_StringArgumentData<CHAR_TYPE>& lhs,
                const bslstl_StringArgumentData<CHAR_TYPE>& rhs)
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
