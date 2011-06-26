// baesu_stacktrace.cpp                                               -*-C++-*-
#include <baesu_stacktrace.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(baesu_stacktrace_cpp,"$Id$ $CSID$")

#include <bslim_printer.h>
#include <bsls_assert.h>

#include <bsl_ostream.h>

namespace BloombergLP {

// ACCESSORS
bsl::ostream& baesu_StackTrace::print(bsl::ostream& stream,
                                      int           level,
                                      int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    for (int i = 0; i < length(); ++i) {
        d_frames[i].print(stream, level + 1, spacesPerLevel);
    }
    printer.end();

    return stream;
}

}  // close namespace BloombergLP

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2011
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ------------------------------ END-OF-FILE ---------------------------------
