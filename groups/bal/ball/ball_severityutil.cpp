// ball_severityutil.cpp                                              -*-C++-*-
#include <ball_severityutil.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ball_severityutil_cpp,"$Id$ $CSID$")

#include <bsls_assert.h>
#include <bdlb_string.h>

#include <bsl_cstring.h>   // 'strcmp'

namespace BloombergLP {

namespace ball {
                        // -------------------
                        // struct SeverityUtil
                        // -------------------

// CLASS METHODS
int SeverityUtil::fromAsciiCaseless(Severity::Level *level,
                                    const char      *name)
{
    BSLS_ASSERT(level);
    BSLS_ASSERT(name);

    enum { BAEL_SUCCESS = 0, BAEL_FAILURE = -1 };

    if (bdlb::String::areEqualCaseless("OFF", 3, name)) {
        *level = Severity::e_OFF;
        return BAEL_SUCCESS;                                          // RETURN
    }
    if (bdlb::String::areEqualCaseless("FATAL", 5, name)) {
        *level = Severity::e_FATAL;
        return BAEL_SUCCESS;                                          // RETURN
    }
    if (bdlb::String::areEqualCaseless("ERROR", 5, name)) {
        *level = Severity::e_ERROR;
        return BAEL_SUCCESS;                                          // RETURN
    }
    if (bdlb::String::areEqualCaseless("WARN", 4, name)) {
        *level = Severity::e_WARN;
        return BAEL_SUCCESS;                                          // RETURN
    }
    if (bdlb::String::areEqualCaseless("INFO", 4, name)) {
        *level = Severity::e_INFO;
        return BAEL_SUCCESS;                                          // RETURN
    }
    if (bdlb::String::areEqualCaseless("DEBUG", 5, name)) {
        *level = Severity::e_DEBUG;
        return BAEL_SUCCESS;                                          // RETURN
    }
    if (bdlb::String::areEqualCaseless("TRACE", 5, name)) {
        *level = Severity::e_TRACE;
        return BAEL_SUCCESS;                                          // RETURN
    }
#ifndef BDE_OMIT_INTERNAL_DEPRECATED
    if (bdlb::String::areEqualCaseless("NONE", 4, name)) {
        *level = Severity::BAEL_NONE;
        return BAEL_SUCCESS;                                          // RETURN
    }
#endif // BDE_OMIT_INTERNAL_DEPRECATED
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
#ifndef BDE_OMIT_INTERNAL_DEPRECATED
        || bdlb::String::areEqualCaseless("NONE",  4, name)
#endif // BDE_OMIT_INTERNAL_DEPRECATED
        || bdlb::String::areEqualCaseless("TRACE", 5, name);
}
}  // close package namespace

}  // close enterprise namespace

// ----------------------------------------------------------------------------
// Copyright 2015 Bloomberg Finance L.P.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// ----------------------------- END-OF-FILE ----------------------------------
