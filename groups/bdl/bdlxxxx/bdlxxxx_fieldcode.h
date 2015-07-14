// bdlxxxx_fieldcode.h                                                   -*-C++-*-
#ifndef INCLUDED_BDLXXXX_FIELDCODE
#define INCLUDED_BDLXXXX_FIELDCODE

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Enumerate all supported 'bdex' externalization types.
//
//@CLASSES:
//   bdlxxxx::FieldCode: namespace for enumerating 'bdex' externalization types
//
//@AUTHOR: Shao-wei Hung
//
//@SEE_ALSO:
//
//@DESCRIPTION: This component provides a namespace, 'bdlxxxx::FieldCode', for
// enumerating all of the supported types in the 'bdex' externalization
// package.  Functionality is provided to convert each of the enumerated
// values to its corresponding string representation and to write the string
// form to a standard 'ostream'.

#ifndef INCLUDED_BDLSCM_VERSION
#include <bdlscm_version.h>
#endif

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#endif

namespace BloombergLP {

namespace bdlxxxx {
                        // ====================
                        // class FieldCode
                        // ====================

struct FieldCode {
    // This 'struct' provides a namespace for enumerating all of the supported
    // types in the 'bdex' externalization package.

    enum { BDEX_OFFSET = 0xe0 };
        // Value to offset each enumerator in Type

    // TYPES
    enum Type {
        BDEX_INT8     =  BDEX_OFFSET + 0,
        BDEX_UINT8    =  BDEX_OFFSET + 1,
        BDEX_INT16    =  BDEX_OFFSET + 2,
        BDEX_UINT16   =  BDEX_OFFSET + 3,
        BDEX_INT24    =  BDEX_OFFSET + 4,
        BDEX_UINT24   =  BDEX_OFFSET + 5,
        BDEX_INT32    =  BDEX_OFFSET + 6,
        BDEX_UINT32   =  BDEX_OFFSET + 7,
        BDEX_INT40    =  BDEX_OFFSET + 8,
        BDEX_UINT40   =  BDEX_OFFSET + 9,
        BDEX_INT48    =  BDEX_OFFSET + 10,
        BDEX_UINT48   =  BDEX_OFFSET + 11,
        BDEX_INT56    =  BDEX_OFFSET + 12,
        BDEX_UINT56   =  BDEX_OFFSET + 13,
        BDEX_INT64    =  BDEX_OFFSET + 14,
        BDEX_UINT64   =  BDEX_OFFSET + 15,
        BDEX_FLOAT32  =  BDEX_OFFSET + 16,
        BDEX_FLOAT64  =  BDEX_OFFSET + 17,
        BDEX_INVALID  =  BDEX_OFFSET + 18   // last entry -- see 'LENGTH' below

#ifndef BDE_OMIT_INTERNAL_DEPRECATED
      , INT8    = BDEX_INT8
      , UINT8   = BDEX_UINT8
      , INT16   = BDEX_INT16
      , UINT16  = BDEX_UINT16
      , INT24   = BDEX_INT24
      , UINT24  = BDEX_UINT24
      , INT32   = BDEX_INT32
      , UINT32  = BDEX_UINT32
      , INT40   = BDEX_INT40
      , UINT40  = BDEX_UINT40
      , INT48   = BDEX_INT48
      , UINT48  = BDEX_UINT48
      , INT56   = BDEX_INT56
      , UINT56  = BDEX_UINT56
      , INT64   = BDEX_INT64
      , UINT64  = BDEX_UINT64
      , FLOAT32 = BDEX_FLOAT32
      , FLOAT64 = BDEX_FLOAT64
      , INVALID = BDEX_INVALID
#endif // BDE_OMIT_INTERNAL_DEPRECATED
    };

    enum { BDEX_LENGTH = BDEX_INVALID + 1 - BDEX_OFFSET };
        // Define 'LENGTH' to be the number of (consecutively valued)
        // enumerators in 'Type'.

    // CLASS METHODS
    static const char *toAscii(Type code);
        // Return the string representation exactly matching the enumerator
        // name corresponding to the specified 'code'.
};

// FREE OPERATORS
bsl::ostream& operator<<(bsl::ostream& stream, FieldCode::Type rhs);
}  // close package namespace
    // Write to the specified 'stream' the string representation exactly
    // matching the name corresponding to the specified 'rhs' value.

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2002
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
