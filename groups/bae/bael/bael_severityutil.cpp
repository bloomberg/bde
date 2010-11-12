// bael_severityutil.cpp                                              -*-C++-*-
#include <bael_severityutil.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bael_severityutil_cpp,"$Id$ $CSID$")

#include <bsls_assert.h>
#include <bdeu_string.h>

#include <bsl_cstring.h>   // 'strcmp'

namespace BloombergLP {

                        // ------------------------
                        // struct bael_SeverityUtil
                        // ------------------------

// CLASS METHODS
int bael_SeverityUtil::fromAsciiCaseless(bael_Severity::Level *level,
                                         const char           *name)
{
    BSLS_ASSERT(level);
    BSLS_ASSERT(name);

    enum { BAEL_SUCCESS = 0, BAEL_FAILURE = -1 };

    if (bdeu_String::areEqualCaseless("OFF", 3, name)) {
        *level = bael_Severity::BAEL_OFF;
        return BAEL_SUCCESS;                                          // RETURN
    }
    if (bdeu_String::areEqualCaseless("FATAL", 5, name)) {
        *level = bael_Severity::BAEL_FATAL;
        return BAEL_SUCCESS;                                          // RETURN
    }
    if (bdeu_String::areEqualCaseless("ERROR", 5, name)) {
        *level = bael_Severity::BAEL_ERROR;
        return BAEL_SUCCESS;                                          // RETURN
    }
    if (bdeu_String::areEqualCaseless("WARN", 4, name)) {
        *level = bael_Severity::BAEL_WARN;
        return BAEL_SUCCESS;                                          // RETURN
    }
    if (bdeu_String::areEqualCaseless("INFO", 4, name)) {
        *level = bael_Severity::BAEL_INFO;
        return BAEL_SUCCESS;                                          // RETURN
    }
    if (bdeu_String::areEqualCaseless("DEBUG", 5, name)) {
        *level = bael_Severity::BAEL_DEBUG;
        return BAEL_SUCCESS;                                          // RETURN
    }
    if (bdeu_String::areEqualCaseless("TRACE", 5, name)) {
        *level = bael_Severity::BAEL_TRACE;
        return BAEL_SUCCESS;                                          // RETURN
    }
    if (bdeu_String::areEqualCaseless("NONE", 4, name)) {
        *level = bael_Severity::BAEL_NONE;
        return BAEL_SUCCESS;                                          // RETURN
    }
    return BAEL_FAILURE;
}

bool bael_SeverityUtil::isValidNameCaseless(const char *name)
{
    BSLS_ASSERT(name);

    return bdeu_String::areEqualCaseless("OFF",   3, name)
        || bdeu_String::areEqualCaseless("FATAL", 5, name)
        || bdeu_String::areEqualCaseless("ERROR", 5, name)
        || bdeu_String::areEqualCaseless("WARN",  4, name)
        || bdeu_String::areEqualCaseless("INFO",  4, name)
        || bdeu_String::areEqualCaseless("DEBUG", 5, name)
        || bdeu_String::areEqualCaseless("TRACE", 5, name)
        || bdeu_String::areEqualCaseless("NONE",  4, name);
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
