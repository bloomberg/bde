// bdlaggxxx_errorcode.cpp                                            -*-C++-*-
#include <bdlaggxxx_errorcode.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlaggxxx_errorcode_cpp,"$Id$ $CSID$")

#include <bdlb_print.h>

#include <bsl_ostream.h>

namespace BloombergLP {

namespace bdlaggxxx {
                     // ---------------------
                     // struct ErrorCode
                     // ---------------------

// CLASS METHODS
bsl::ostream& ErrorCode::print(bsl::ostream&        stream,
                                    ErrorCode::Code value,
                                    int                  level,
                                    int                  spacesPerLevel)
{
    bdlb::Print::indent(stream, level, spacesPerLevel);

    stream << ErrorCode::toAscii(value);

    if (spacesPerLevel >= 0) {
        stream << '\n';
    }

    return stream;
}

const char *ErrorCode::toAscii(ErrorCode::Code value)
{
#define CASE(X) case(BCEM_ ## X): return #X;

    switch (value) {
      CASE(SUCCESS)
      CASE(UNKNOWN_ERROR)
      CASE(NOT_A_RECORD)
      CASE(NOT_A_SEQUENCE)
      CASE(NOT_A_CHOICE)
      CASE(NOT_AN_ARRAY)
      CASE(BAD_FIELDNAME)
      CASE(BAD_FIELDID)
      CASE(BAD_FIELDINDEX)
      CASE(BAD_ARRAYINDEX)
      CASE(NOT_SELECTED)
      CASE(BAD_CONVERSION)
      CASE(BAD_ENUMVALUE)
      CASE(NON_CONFORMANT)
      CASE(AMBIGUOUS_ANON)
      default: return "(* UNKNOWN *)";
    }

#undef CASE
}
}  // close package namespace

}  // close enterprise namespace

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2012
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ----------------------------------
