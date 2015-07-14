// ball_severityutil.cpp                                              -*-C++-*-
#include <ball_severityutil.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ball_severityutil_cpp,"$Id$ $CSID$")

#include <bsls_assert.h>
#include <bdlb_string.h>

#include <bsl_cstring.h>   // 'strcmp'

namespace BloombergLP {

namespace ball {
                        // ------------------------
                        // struct SeverityUtil
                        // ------------------------

// CLASS METHODS
int SeverityUtil::fromAsciiCaseless(Severity::Level *level,
                                         const char           *name)
{
    BSLS_ASSERT(level);
    BSLS_ASSERT(name);

    enum { BAEL_SUCCESS = 0, BAEL_FAILURE = -1 };

    if (bdlb::String::areEqualCaseless("OFF", 3, name)) {
        *level = Severity::BAEL_OFF;
        return BAEL_SUCCESS;                                          // RETURN
    }
    if (bdlb::String::areEqualCaseless("FATAL", 5, name)) {
        *level = Severity::BAEL_FATAL;
        return BAEL_SUCCESS;                                          // RETURN
    }
    if (bdlb::String::areEqualCaseless("ERROR", 5, name)) {
        *level = Severity::BAEL_ERROR;
        return BAEL_SUCCESS;                                          // RETURN
    }
    if (bdlb::String::areEqualCaseless("WARN", 4, name)) {
        *level = Severity::BAEL_WARN;
        return BAEL_SUCCESS;                                          // RETURN
    }
    if (bdlb::String::areEqualCaseless("INFO", 4, name)) {
        *level = Severity::BAEL_INFO;
        return BAEL_SUCCESS;                                          // RETURN
    }
    if (bdlb::String::areEqualCaseless("DEBUG", 5, name)) {
        *level = Severity::BAEL_DEBUG;
        return BAEL_SUCCESS;                                          // RETURN
    }
    if (bdlb::String::areEqualCaseless("TRACE", 5, name)) {
        *level = Severity::BAEL_TRACE;
        return BAEL_SUCCESS;                                          // RETURN
    }
    if (bdlb::String::areEqualCaseless("NONE", 4, name)) {
        *level = Severity::BAEL_NONE;
        return BAEL_SUCCESS;                                          // RETURN
    }
    return BAEL_FAILURE;
}

bool SeverityUtil::isValidNameCaseless(const char *name)
{
    BSLS_ASSERT(name);

    return bdlb::String::areEqualCaseless("OFF",   3, name)
        || bdlb::String::areEqualCaseless("FATAL", 5, name)
        || bdlb::String::areEqualCaseless("ERROR", 5, name)
        || bdlb::String::areEqualCaseless("WARN",  4, name)
        || bdlb::String::areEqualCaseless("INFO",  4, name)
        || bdlb::String::areEqualCaseless("DEBUG", 5, name)
        || bdlb::String::areEqualCaseless("TRACE", 5, name)
        || bdlb::String::areEqualCaseless("NONE",  4, name);
}
}  // close package namespace

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2007
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
