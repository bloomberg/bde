// bcem_aggregateerror.cpp                                            -*-C++-*-

#include <bcem_aggregateerror.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bcem_aggregateerror_cpp,"$Id$ $CSID$")

#include <bslim_printer.h>

namespace BloombergLP {

int bcem_AggregateError::fromInt(bcem_AggregateError::Code *result,
                                 int                        number)
{
    int rc;
    switch (number) {
      case BCEM_SUCCESS:
      case BCEM_ERR_UNKNOWN_ERROR:
      case BCEM_ERR_NOT_A_RECORD:
      case BCEM_ERR_NOT_A_SEQUENCE:
      case BCEM_ERR_NOT_A_CHOICE:
      case BCEM_ERR_NOT_AN_ARRAY:
      case BCEM_ERR_BAD_FIELDNAME:
      case BCEM_ERR_BAD_FIELDID:
      case BCEM_ERR_BAD_FIELDINDEX:
      case BCEM_ERR_BAD_ARRAYINDEX:
      case BCEM_ERR_NOT_SELECTED:
      case BCEM_ERR_BAD_CONVERSION:
      case BCEM_ERR_BAD_ENUMVALUE:
      case BCEM_ERR_NON_CONFORMANT:
      case BCEM_ERR_AMBIGUOUS_ANON: {
        *result = (bcem_AggregateError::Code) number;
        rc = 0;
      } break;
      default: {
        rc = -1;
      }
    }
    return rc;
}

bsl::ostream& bcem_AggregateError::print(bsl::ostream& stream,
                                         int           level,
                                         int           spacesPerLevel) const
{
    if (stream.bad()) {
        return stream;                                                // RETURN
    }

    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();

    printer.printAttribute("description", d_description.c_str());
    printer.printAttribute("code", d_code);

    printer.end();

    return stream;
}

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2011
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
