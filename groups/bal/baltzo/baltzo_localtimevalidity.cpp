// baltzo_localtimevalidity.cpp                                       -*-C++-*-
#include <baltzo_localtimevalidity.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(baltzo_localtimevalidity_cpp,"$Id$ $CSID$")

#include <bslim_printer.h>

#include <bsl_ostream.h>

namespace BloombergLP {

namespace baltzo {
                     // -------------------------------
                     // struct LocalTimeValidity
                     // -------------------------------

// CLASS METHODS
bsl::ostream& LocalTimeValidity::print(
                                 bsl::ostream&                  stream,
                                 LocalTimeValidity::Enum value,
                                 int                            level,
                                 int                            spacesPerLevel)
{
    bslim::Printer printer(&stream, level, spacesPerLevel);

    printer.start(true);
    stream << toAscii(value);
    printer.end(true);

    return stream;
}

const char *LocalTimeValidity::toAscii(
                                          LocalTimeValidity::Enum value)
{
#define CASE(X) case(BAETZO_ ## X): return #X;

    switch (value) {
      CASE(VALID_UNIQUE)                                              // RETURN
      CASE(VALID_AMBIGUOUS)                                           // RETURN
      CASE(INVALID)                                                   // RETURN
      default: return "(* UNKNOWN *)";                                // RETURN
    }

#undef CASE
}
}  // close package namespace

}  // close namespace BloombergLP

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2011
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ----------------------------------
