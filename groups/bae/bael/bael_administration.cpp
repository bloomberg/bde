// bael_administration.cpp         -*-C++-*-
#include <bael_administration.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bael_administration_cpp,"$Id$ $CSID$")

#include <bael_categorymanager.h>
#include <bael_loggercategoryutil.h>
#include <bael_loggermanager.h>

#include <bael_testobserver.h>            // for testing only

namespace BloombergLP {

                      // --------------------------
                      // struct bael_Administration
                      // --------------------------

// CLASS METHODS

int bael_Administration::addCategory(const char *categoryName,
                                     int         recordLevel,
                                     int         passLevel,
                                     int         triggerLevel,
                                     int         triggerAllLevel)
{
    return 0 == bael_LoggerManager::singleton().addCategory(categoryName,
                                                            recordLevel,
                                                            passLevel,
                                                            triggerLevel,
                                                            triggerAllLevel);
}

int bael_Administration::setThresholdLevels(const char *regularExpression,
                                            int         recordLevel,
                                            int         passLevel,
                                            int         triggerLevel,
                                            int         triggerAllLevel)
{
    return bael_LoggerCategoryUtil::setThresholdLevels(
                                              &bael_LoggerManager::singleton(),
                                              regularExpression,
                                              recordLevel,
                                              passLevel,
                                              triggerLevel,
                                              triggerAllLevel);
}

int bael_Administration::setDefaultThresholdLevels(int recordLevel,
                                                   int passLevel,
                                                   int triggerLevel,
                                                   int triggerAllLevel)
{
    return bael_LoggerManager::singleton().setDefaultThresholdLevels(
                                                              recordLevel,
                                                              passLevel,
                                                              triggerLevel,
                                                              triggerAllLevel);
}

void bael_Administration::resetDefaultThresholdLevels()
{
    bael_LoggerManager::singleton().resetDefaultThresholdLevels();
}

int bael_Administration::defaultRecordThresholdLevel()
{
    return bael_LoggerManager::singleton().defaultRecordThresholdLevel();
}

int bael_Administration::defaultPassThresholdLevel()
{
    return bael_LoggerManager::singleton().defaultPassThresholdLevel();
}

int bael_Administration::defaultTriggerThresholdLevel()
{
    return bael_LoggerManager::singleton().defaultTriggerThresholdLevel();
}

int bael_Administration::defaultTriggerAllThresholdLevel()
{
    return bael_LoggerManager::singleton().defaultTriggerAllThresholdLevel();
}

int bael_Administration::recordLevel(const char *categoryName)
{
    enum { FAILURE = -1 };
    const bael_Category *category =
                  bael_LoggerManager::singleton().lookupCategory(categoryName);
    return category ? category->recordLevel() : FAILURE;
}

int bael_Administration::passLevel(const char *categoryName)
{
    enum { FAILURE = -1 };
    const bael_Category *category =
                  bael_LoggerManager::singleton().lookupCategory(categoryName);
    return category ? category->passLevel() : FAILURE;
}

int bael_Administration::triggerLevel(const char *categoryName)
{
    enum { FAILURE = -1 };
    const bael_Category *category =
                  bael_LoggerManager::singleton().lookupCategory(categoryName);
    return category ? category->triggerLevel() : FAILURE;
}

int bael_Administration::triggerAllLevel(const char *categoryName)
{
    enum { FAILURE = -1 };
    const bael_Category *category =
                  bael_LoggerManager::singleton().lookupCategory(categoryName);
    return category ? category->triggerAllLevel() : FAILURE;
}

int bael_Administration::maxNumCategories()
{
    return bael_LoggerManager::singleton().maxNumCategories();
}

int bael_Administration::numCategories()
{
    return bael_LoggerManager::singleton().numCategories();
}

void bael_Administration::setMaxNumCategories(int length)
{
    bael_LoggerManager::singleton().setMaxNumCategories(length);
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
