// bdlxxxx_fieldcode.cpp                                              -*-C++-*-
#include <bdlxxxx_fieldcode.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlxxxx_fieldcode_cpp,"$Id$ $CSID$")

#include <bsl_ostream.h>

namespace BloombergLP {

namespace bdlxxxx {
                        // ====================
                        // class FieldCode
                        // ====================

// CLASS METHODS
const char *FieldCode::toAscii(Type code)
{
#define CASE(X) case(X): return #X;

    switch (code) {
      case(BDEX_INT8)    : return "INT8";
      case(BDEX_UINT8)   : return "UINT8";
      case(BDEX_INT16)   : return "INT16";
      case(BDEX_UINT16)  : return "UINT16";
      case(BDEX_INT24)   : return "INT24";
      case(BDEX_UINT24)  : return "UINT24";
      case(BDEX_INT32)   : return "INT32";
      case(BDEX_UINT32)  : return "UINT32";
      case(BDEX_INT40)   : return "INT40";
      case(BDEX_UINT40)  : return "UINT40";
      case(BDEX_INT48)   : return "INT48";
      case(BDEX_UINT48)  : return "UINT48";
      case(BDEX_INT56)   : return "INT56";
      case(BDEX_UINT56)  : return "UINT56";
      case(BDEX_INT64)   : return "INT64";
      case(BDEX_UINT64)  : return "UINT64";
      case(BDEX_FLOAT32) : return "FLOAT32";
      case(BDEX_FLOAT64) : return "FLOAT64";
      case(BDEX_INVALID) : return "INVALID";
      default: return "(* UNKNOWN *)";
    }
#undef CASE
}
}  // close package namespace

// FREE OPERATORS
bsl::ostream& bdlxxxx::operator<<(bsl::ostream& stream, FieldCode::Type rhs)
{
    return stream << FieldCode::toAscii(rhs);
}

}  // close enterprise namespace

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2002
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ----------------------------------
