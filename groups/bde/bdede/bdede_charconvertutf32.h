// bdede_charconvertutf32.h                                           -*-C++-*-
#ifndef INCLUDED_BDEDE_CHARCONVERTUTF32
#define INCLUDED_BDEDE_CHARCONVERTUTF32

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide fast, safe conversion between UTF-8 encoding and utf-32.
//
//@CLASSES:
//  bdede_CharConvertUtf16: namespace for conversion between UTF-8 and utf-32.
//
//@AUTHOR: Bill Chapman (bchapman2)
//
//@DESCRIPTION: 

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BDEDE_CHARCONVERTSTATUS
#include <bdede_charconvertstatus.h>
#endif

#ifndef INCLUDED_BSL_CSTDLIB
#include <bsl_cstdlib.h>            // 'bsl::size_t'
#endif

#ifndef INCLUDED_BSL_STRING
#include <bsl_string.h>
#endif

#ifndef INCLUDED_BSL_VECTOR
#include <bsl_vector.h>
#endif

namespace BloombergLP {

                        // =============================
                        // struct bdede_CharConvertUtf32
                        // =============================

struct bdede_CharConvertUtf32 {

                        // -- UTF-8 to UTF-32 Methods

    // CLASS METHODS
    static int utf8ToUtf32(bsl::vector<unsigned int> *dstVector,
                           const char                *srcString,
                           bsl::size_t               *numCharsWritten = 0,
                           unsigned int               errorCharacter  = '?');

    static int utf8ToUtf32(unsigned int *dstBuffer,
                           int           dstCapacity,
                           const char   *srcString,
                           bsl::size_t  *numCharsWritten = 0,
                           unsigned int  errorCharacter  = '?');

                        // -- UTF-32 to UTF-8 Methods

    static int utf32ToUtf8(bsl::string        *dstString,
                           const unsigned int *srcString,
                           bsl::size_t        *numCharsWritten = 0,
                           unsigned char       errorCharacter  = '?');

    static int utf32ToUtf8(bsl::vector<char>  *dstString,
                           const unsigned int *srcString,
                           bsl::size_t        *numCharsWritten = 0,
                           unsigned char       errorCharacter  = '?');

    static int utf32ToUtf8(char               *dstBuffer,
			   int                 dstCapacity,
                           const unsigned int *srcString,
                           bsl::size_t        *numCharsWritten = 0,
                           unsigned char       errorCharacter  = '?');
};

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2013
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
