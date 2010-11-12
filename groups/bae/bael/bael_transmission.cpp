// bael_transmission.cpp            -*-C++-*-
#include <bael_transmission.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bael_transmission_cpp,"$Id$ $CSID$")

#include <bsl_ostream.h>

namespace BloombergLP {

// PRIVATE CLASS METHODS

void bael_Transmission::print(bsl::ostream&            stream,
                              bael_Transmission::Cause value)
{
    stream << bael_Transmission::toAscii(value);
}

// CLASS METHODS

const char *bael_Transmission::toAscii(bael_Transmission::Cause value)
{
#define CASE(X) case(X): return #X;

    switch(value) {
      case(BAEL_PASSTHROUGH)       : return "PASSTHROUGH";
      case(BAEL_TRIGGER)           : return "TRIGGER";
      case(BAEL_TRIGGER_ALL)       : return "TRIGGER_ALL";
      case(BAEL_MANUAL_PUBLISH)    : return "MANUAL_PUBLISH";
      case(BAEL_MANUAL_PUBLISH_ALL): return "MANUAL_PUBLISH_ALL";
      default: return "(* UNKNOWN *)";
    }

#undef CASE
}

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2007
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
