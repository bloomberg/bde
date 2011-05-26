// baesu_stacktrace.cpp                                               -*-C++-*-
#include <baesu_stacktrace.h>

#include <bsls_assert.h>

#include <bsl_ostream.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(baesu_stacktrace_cpp,"$Id$ $CSID$")

namespace BloombergLP {

// ACCESSORS
bsl::ostream& baesu_StackTrace::print(bsl::ostream& stream,
                                      int           level,
                                      int           spacesPerLevel) const
{
    // TBD
    return stream;
}

// FREE OPERATORS
bool operator==(const baesu_StackTrace& lhs,
                const baesu_StackTrace& rhs)
{
    if (lhs.length() != rhs.length()) {
        return false;
    }

    for (int i = 0;  i < lhs.length(); ++i) {
        if (lhs[i] != rhs[i]) {
            return false;
        }
    }

    return true;
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
