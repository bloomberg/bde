// ball_loggerfunctorpayloads.cpp                                     -*-C++-*-
#include <ball_loggerfunctorpayloads.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ball_loggerfunctorpayloads_cpp,"$Id$ $CSID$")

#include <ball_categorymanager.h>
#include <ball_loggermanager.h>
#include <ball_testobserver.h>                  // for testing only

#include <bsls_assert.h>

#include <bsl_string.h>

namespace BloombergLP {

namespace ball {
                    // ---------------------------------
                    // struct LoggerFunctorPayloads
                    // ---------------------------------

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

    bsl::string               parentName(categoryName);
    const LoggerManager& manager = LoggerManager::singleton();

    int delimIdx;
    while ((int)bsl::string::npos != (delimIdx = parentName.rfind(delimiter)))
                                                                              {
        parentName.erase(delimIdx);  // candidate for "proximate parent" name
        const Category *parent =
                                    manager.lookupCategory(parentName.c_str());
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

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2007
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
