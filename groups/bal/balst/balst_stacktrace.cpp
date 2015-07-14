// balst_stacktrace.cpp                                               -*-C++-*-
#include <balst_stacktrace.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(balst_stacktrace_cpp,"$Id$ $CSID$")

#include <bslim_printer.h>
#include <bsls_assert.h>

#include <bsl_ostream.h>

namespace BloombergLP {

namespace balst {
// ACCESSORS
bsl::ostream& StackTrace::print(bsl::ostream& stream,
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
}  // close package namespace

}  // close namespace BloombergLP

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2011
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ------------------------------ END-OF-FILE ---------------------------------
