// ball_loggercategoryutil.cpp                                        -*-C++-*-
#include <ball_loggercategoryutil.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ball_loggercategoryutil_cpp,"$Id$ $CSID$")

#include <ball_categorymanager.h>
#include <ball_loggermanager.h>

#include <bsls_assert.h>

#include <bsl_cstring.h>    // strlen(), strncmp()
#include <bsl_string.h>

namespace BloombergLP {
namespace ball {
                            // -------------------------
                            // struct LoggerCategoryUtil
                            // -------------------------
// CLASS METHODS
int LoggerCategoryUtil::setThresholdLevels(LoggerManager *loggerManager,
                                           const char    *pattern,
                                           int            recordLevel,
                                           int            passLevel,
                                           int            triggerLevel,
                                           int            triggerAllLevel)
{
    BSLS_ASSERT(loggerManager);
    BSLS_ASSERT(pattern);

    if (!Category::areValidThresholdLevels(recordLevel,
                                           passLevel,
                                           triggerLevel,
                                           triggerAllLevel)) {
        return -1;                                                    // RETURN
    }

    const bsl::size_t len = bsl::strlen(pattern);

    if (0 == len || '*' != pattern[len - 1]) {
        Category *category = loggerManager->lookupCategory(pattern);
        if (!category) {
            return 0;                                                 // RETURN
        }
        const int s = category->setLevels(recordLevel,
                                          passLevel,
                                          triggerLevel,
                                          triggerAllLevel);
        (void)s; BSLS_ASSERT(0 == s);
        return 1;                                                     // RETURN
    }

    const bsl::string withoutTheAsterix(pattern, &pattern[len - 1]);

    return loggerManager->setThresholdLevelsHierarchically(
                                                     withoutTheAsterix.c_str(),
                                                     recordLevel,
                                                     passLevel,
                                                     triggerLevel,
                                                     triggerAllLevel);
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
