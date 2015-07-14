// ball_administration.cpp                                            -*-C++-*-
#include <ball_administration.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ball_administration_cpp,"$Id$ $CSID$")

#include <ball_categorymanager.h>
#include <ball_loggercategoryutil.h>
#include <ball_loggermanager.h>

#include <ball_defaultobserver.h>         // for testing only
#include <ball_testobserver.h>            // for testing only

namespace BloombergLP {

namespace ball {
                      // --------------------------
                      // struct Administration
                      // --------------------------

// CLASS METHODS

int Administration::addCategory(const char *categoryName,
                                     int         recordLevel,
                                     int         passLevel,
                                     int         triggerLevel,
                                     int         triggerAllLevel)
{
    return 0 == LoggerManager::singleton().addCategory(categoryName,
                                                            recordLevel,
                                                            passLevel,
                                                            triggerLevel,
                                                            triggerAllLevel);
}

int Administration::setAllThresholdLevels(int recordLevel,
                                               int passLevel,
                                               int triggerLevel,
                                               int triggerAllLevel)
{
    int rc = setDefaultThresholdLevels(recordLevel,
                                       passLevel,
                                       triggerLevel,
                                       triggerAllLevel);
    if (0 != rc) {
        return rc;                                                    // RETURN
    }
    setThresholdLevels("*",
                       recordLevel,
                       passLevel,
                       triggerLevel,
                       triggerAllLevel);

    // 'setThresholdLevels' is documented to only return an error status if
    // the thresholds are invalid (in which case 'setDefaultThresholdLevels'
    // will have returned an error.

    BSLS_ASSERT(0 <= rc);

    return rc;
}

int Administration::setDefaultThresholdLevels(int recordLevel,
                                                   int passLevel,
                                                   int triggerLevel,
                                                   int triggerAllLevel)
{
    return LoggerManager::singleton().setDefaultThresholdLevels(
                                                              recordLevel,
                                                              passLevel,
                                                              triggerLevel,
                                                              triggerAllLevel);
}

int Administration::setThresholdLevels(const char *regularExpression,
                                            int         recordLevel,
                                            int         passLevel,
                                            int         triggerLevel,
                                            int         triggerAllLevel)
{
    return LoggerCategoryUtil::setThresholdLevels(
                                              &LoggerManager::singleton(),
                                              regularExpression,
                                              recordLevel,
                                              passLevel,
                                              triggerLevel,
                                              triggerAllLevel);
}

void Administration::resetDefaultThresholdLevels()
{
    LoggerManager::singleton().resetDefaultThresholdLevels();
}

int Administration::defaultRecordThresholdLevel()
{
    return LoggerManager::singleton().defaultRecordThresholdLevel();
}

int Administration::defaultPassThresholdLevel()
{
    return LoggerManager::singleton().defaultPassThresholdLevel();
}

int Administration::defaultTriggerThresholdLevel()
{
    return LoggerManager::singleton().defaultTriggerThresholdLevel();
}

int Administration::defaultTriggerAllThresholdLevel()
{
    return LoggerManager::singleton().defaultTriggerAllThresholdLevel();
}

int Administration::recordLevel(const char *categoryName)
{
    enum { FAILURE = -1 };
    const Category *category =
                  LoggerManager::singleton().lookupCategory(categoryName);
    return category ? category->recordLevel() : FAILURE;
}

int Administration::passLevel(const char *categoryName)
{
    enum { FAILURE = -1 };
    const Category *category =
                  LoggerManager::singleton().lookupCategory(categoryName);
    return category ? category->passLevel() : FAILURE;
}

int Administration::triggerLevel(const char *categoryName)
{
    enum { FAILURE = -1 };
    const Category *category =
                  LoggerManager::singleton().lookupCategory(categoryName);
    return category ? category->triggerLevel() : FAILURE;
}

int Administration::triggerAllLevel(const char *categoryName)
{
    enum { FAILURE = -1 };
    const Category *category =
                  LoggerManager::singleton().lookupCategory(categoryName);
    return category ? category->triggerAllLevel() : FAILURE;
}

int Administration::maxNumCategories()
{
    return LoggerManager::singleton().maxNumCategories();
}

int Administration::numCategories()
{
    return LoggerManager::singleton().numCategories();
}

void Administration::setMaxNumCategories(int length)
{
    LoggerManager::singleton().setMaxNumCategories(length);
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
