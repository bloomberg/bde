// bael_loggerfunctorpayloads.cpp                                     -*-C++-*-
#include <bael_loggerfunctorpayloads.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bael_loggerfunctorpayloads_cpp,"$Id$ $CSID$")

#include <bael_categorymanager.h>
#include <bael_loggermanager.h>
#include <bael_testobserver.h>                  // for testing only

#include <bsls_assert.h>

#include <bsl_string.h>

namespace BloombergLP {

                    // ---------------------------------
                    // struct bael_LoggerFunctorPayloads
                    // ---------------------------------

// CLASS METHODS

void bael_LoggerFunctorPayloads::loadParentCategoryThresholdValues(
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
    const bael_LoggerManager& manager = bael_LoggerManager::singleton();

    int delimIdx;
    while ((int)bsl::string::npos != (delimIdx = parentName.rfind(delimiter)))
                                                                              {
        parentName.erase(delimIdx);  // candidate for "proximate parent" name
        const bael_Category *parent =
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

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2007
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
