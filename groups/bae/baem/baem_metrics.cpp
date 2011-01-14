// baem_metrics.cpp                                                   -*-C++-*-
#include <baem_metrics.h>

#include <bdes_ident.h>

BDES_IDENT_RCSID(baem_metrics_cpp,"$Id$ $CSID$")

#include <bael_log.h>

namespace BloombergLP {

// STATIC DATA
static
const char LOG_CATEGORY[] = "BAEM.METRICS";

static
const char *TYPE_STRING[] = {
    "category",
    "metric",
};

                          // ------------------
                          // class baem_Metrics
                          // ------------------

// CLASS METHODS
void baem_Metrics_Helper::logEmptyName(const char *name,
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

    BAEL_LOG_SET_CATEGORY(LOG_CATEGORY);

    BAEL_LOG_WARN << "Empty " << TYPE_STRING[type] << " \"" << name
                  << "\" added at " << file << ":" << line << BAEL_LOG_END;
}

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2009
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
