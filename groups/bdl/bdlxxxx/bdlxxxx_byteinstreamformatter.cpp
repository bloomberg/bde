// bdlxxxx_byteinstreamformatter.cpp                                  -*-C++-*-
#include <bdlxxxx_byteinstreamformatter.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlxxxx_byteinstreamformatter_cpp,"$Id$ $CSID$")

#include <bdlxxxx_byteoutstreamformatter.h>    // for testing only

#include <bsls_types.h>

namespace BloombergLP {

// The following assertions are now checked in the
// 'bdex_genericbyteoutformatter' component.
//..
//  struct bdex_ByteInStreamFormatter_Assertions {
//      char assertion1[8 == sizeof(bsls::Types::Int64)];
//      char assertion2[4 == sizeof(int)];
//      char assertion3[2 == sizeof(short)];
//      char assertion4[8 == sizeof(double)];
//      char assertion5[4 == sizeof(float)];
//  };
//..

}  // close enterprise namespace

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2007
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ----------------------------------
