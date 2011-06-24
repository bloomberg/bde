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
//@DESCRIPTION: ...
//

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
    bslstl_StringArgumentData(const CHAR_TYPE *begin, const CHAR_TYPE *end);
        // Construct a 'bslstl_StringArgumentData' object with the specified
        // 'begin' and 'end' pointers to the start and end of a string.  The
        // behavior is undefined unless 'begin <= end'.  Both 'begin' and 'end'
        // can be NULL.

    // ACCESSORS
    const CHAR_TYPE *begin() const;
        // Return the pointer to the start of the string.  Note that the return
        // value can be 'NULL', in which case 'end()' returns 'NULL' as well.

    const CHAR_TYPE *end() const;
        // Return the pointer past the end of the string.  Note that the return
        // value can be 'NULL, in which case 'begin()' returns 'NULL' as well.
};

// ==========================================================================
//                      TEMPLATE FUNCTION DEFINITIONS
// ==========================================================================

                     // -------------------------------
                     // class bslstl_StringArgumentData
                     // -------------------------------

// CREATORS
template <typename CHAR_TYPE>
inline
bslstl_StringArgumentData<CHAR_TYPE>
    ::bslstl_StringArgumentData(const CHAR_TYPE *begin,
                                 const CHAR_TYPE *end)
: d_begin(begin)
, d_end(end)
{
    BSLS_ASSERT_SAFE(d_begin <= d_end);
    BSLS_ASSERT_SAFE((d_begin == NULL) == (d_end == NULL));
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
