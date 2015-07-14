// balm_metrics.cpp                                                   -*-C++-*-
#include <balm_metrics.h>

#include <bsls_ident.h>

BSLS_IDENT_RCSID(balm_metrics_cpp,"$Id$ $CSID$")

#include <ball_log.h>

namespace BloombergLP {

// STATIC DATA
static
const char LOG_CATEGORY[] = "BAEM.METRICS";

static
const char *TYPE_STRING[] = {
    "category",
    "metric",
};

namespace balm {
                          // ------------------
                          // class Metrics
                          // ------------------

// CLASS METHODS
void Metrics_Helper::logEmptyName(const char *name,
                                       NameType    type,
                                       const char *file,
                                       int         line) {

    // Check to see if 'name' is empty or contains only spaces.

    const char *namePtr = name;
    while (*namePtr != '\0') {
        if (*namePtr != ' ') {
            return;                                                   // RETURN
        }
        ++namePtr;
    }

    // Log the problem.

    BALL_LOG_SET_CATEGORY(LOG_CATEGORY);

    BALL_LOG_WARN << "Empty " << TYPE_STRING[type] << " \"" << name
                  << "\" added at " << file << ":" << line << BALL_LOG_END;
}
}  // close package namespace

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2009
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
