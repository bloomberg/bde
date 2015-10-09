// ball_loggerfunctorpayloads.cpp                                     -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <ball_loggerfunctorpayloads.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ball_loggerfunctorpayloads_cpp,"$Id$ $CSID$")

#include <ball_categorymanager.h>
#include <ball_loggermanager.h>
#include <ball_loggermanagerconfiguration.h>    // for testing only
#include <ball_testobserver.h>                  // for testing only

#include <bsls_assert.h>

#include <bsl_string.h>

namespace BloombergLP {

namespace ball {
                    // ----------------------------
                    // struct LoggerFunctorPayloads
                    // ----------------------------

// CLASS METHODS

void LoggerFunctorPayloads::loadParentCategoryThresholdValues(
                                           int                *recordLevel,
                                           int                *passLevel,
                                           int                *triggerLevel,
                                           int                *triggerAllLevel,
                                           const char         *categoryName,
                                           char                delimiter)
{
    BSLS_ASSERT(recordLevel);
    BSLS_ASSERT(passLevel);
    BSLS_ASSERT(triggerLevel);
    BSLS_ASSERT(triggerAllLevel);
    BSLS_ASSERT(categoryName);

    bsl::string parentName(categoryName);
    const LoggerManager& manager = LoggerManager::singleton();

    int delimIdx;
    while ((int)bsl::string::npos != (delimIdx = parentName.rfind(delimiter)))
                                                                              {
        parentName.erase(delimIdx);  // candidate for "proximate parent" name

        const Category *parent = manager.lookupCategory(parentName.c_str());
        if (parent) {
            // found "parent" category, so use parent's thresholds
            *recordLevel     = parent->recordLevel();
            *passLevel       = parent->passLevel();
            *triggerLevel    = parent->triggerLevel();
            *triggerAllLevel = parent->triggerAllLevel();
            return;                                                   // RETURN
        }
    }

    // no category having a "parent" name, so use default thresholds
    *recordLevel     = manager.defaultRecordThresholdLevel();
    *passLevel       = manager.defaultPassThresholdLevel();
    *triggerLevel    = manager.defaultTriggerThresholdLevel();
    *triggerAllLevel = manager.defaultTriggerAllThresholdLevel();
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
