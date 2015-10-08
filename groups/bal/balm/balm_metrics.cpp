// balm_metrics.cpp                                                   -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <balm_metrics.h>

#include <bsls_ident.h>
#include <bsl_ostream.h>

BSLS_IDENT_RCSID(balm_metrics_cpp,"$Id$ $CSID$")

#include <ball_log.h>

namespace BloombergLP {

namespace balm {

// STATIC DATA
namespace {

const char k_LOG_CATEGORY[] = "BALM.METRICS";
const char *k_TYPE_STRING[] = { "category", "metric", };

}  // close unnamed namespace

                               // -------------
                               // class Metrics
                               // -------------

// CLASS METHODS
void Metrics_Helper::logEmptyName(const char *name,
                                  NameType    type,
                                  const char *file,
                                  int         line)
{
    // Check to see if 'name' is empty or contains only spaces.

    const char *namePtr = name;
    while (*namePtr != '\0') {
        if (*namePtr != ' ') {
            return;                                                   // RETURN
        }
        ++namePtr;
    }

    // Log the problem.

    BALL_LOG_SET_CATEGORY(k_LOG_CATEGORY);

    BALL_LOG_WARN << "Empty " << k_TYPE_STRING[type] << " \"" << name
                  << "\" added at " << file << ":" << line << BALL_LOG_END;
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
