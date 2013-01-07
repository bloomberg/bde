// bcem_errorcode.cpp                                                 -*-C++-*-
#include <bcem_errorcode.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bcem_errorcode_cpp,"$Id$ $CSID$")

#include <bdeu_print.h>

#include <bsl_ostream.h>

namespace BloombergLP {

                     // ---------------------
                     // struct bcem_ErrorCode
                     // ---------------------

// CLASS METHODS
bsl::ostream& bcem_ErrorCode::print(bsl::ostream&        stream,
                                    bcem_ErrorCode::Code value,
                                    int                  level,
                                    int                  spacesPerLevel)
{
    bdeu_Print::indent(stream, level, spacesPerLevel);

    stream << bcem_ErrorCode::toAscii(value);

    if (spacesPerLevel >= 0) {
        stream << '\n';
    }

    return stream;
}

const char *bcem_ErrorCode::toAscii(bcem_ErrorCode::Code value)
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

}  // close namespace BloombergLP

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2012
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ----------------------------------
