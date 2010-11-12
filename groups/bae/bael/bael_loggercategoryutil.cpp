// bael_loggercategoryutil.cpp              -*-C++-*-
#include <bael_loggercategoryutil.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bael_loggercategoryutil_cpp,"$Id$ $CSID$")

#include <bael_categorymanager.h>
#include <bael_loggermanager.h>
#include <bael_testobserver.h>                // for testing only

#include <bsls_assert.h>

#include <bsl_cstring.h>    // strlen(), strncmp()

namespace BloombergLP {

                            // ------------------------------
                            // struct bael_LoggerCategoryUtil
                            // ------------------------------

// CLASS METHODS

bael_Category *bael_LoggerCategoryUtil::addCategoryHierarchically(
                                             bael_LoggerManager *loggerManager,
                                             const char         *categoryName)
{
    BSLS_ASSERT(loggerManager);
    BSLS_ASSERT(categoryName);

    if (loggerManager->lookupCategory(categoryName)) {
        return 0;                                                     // RETURN
    }

    bael_Category *longestPrefixCategory =  0;
    int            longestPrefixLength   = -1;

    for (bael_LoggerCategoryManip manip(loggerManager);
         manip;
         manip.advance()) {
        int length = bsl::strlen(manip().categoryName());
        if (longestPrefixLength < length
         && 0 == bsl::strncmp(manip().categoryName(), categoryName, length)) {
            longestPrefixCategory = &manip();
            longestPrefixLength   = length;
        }
    }

    bael_Category *category = 0;
    if (longestPrefixCategory) {
        category = loggerManager->addCategory(
                                     categoryName,
                                     longestPrefixCategory->recordLevel(),
                                     longestPrefixCategory->passLevel(),
                                     longestPrefixCategory->triggerLevel(),
                                     longestPrefixCategory->triggerAllLevel());
    }
    else {
        category = loggerManager->addCategory(
                             categoryName,
                             loggerManager->defaultRecordThresholdLevel(),
                             loggerManager->defaultPassThresholdLevel(),
                             loggerManager->defaultTriggerThresholdLevel(),
                             loggerManager->defaultTriggerAllThresholdLevel());
    }

    return category;
}

int bael_LoggerCategoryUtil::setThresholdLevelsHierarchically(
                                        bael_LoggerManager *loggerManager,
                                        const char         *categoryNamePrefix,
                                        int                 recordLevel,
                                        int                 passLevel,
                                        int                 triggerLevel,
                                        int                 triggerAllLevel)
{
    BSLS_ASSERT(loggerManager);
    BSLS_ASSERT(categoryNamePrefix);

    if (!bael_Category::areValidThresholdLevels(recordLevel,
                                                passLevel,
                                                triggerLevel,
                                                triggerAllLevel)) {
        return -1;                                                    // RETURN
    }

    int matchCount = 0;
    bsl::size_t length = bsl::strlen(categoryNamePrefix);

    for (bael_LoggerCategoryManip manip(loggerManager);
         manip;
         manip.advance()) {
        if (0 == bsl::strncmp(manip().categoryName(),
                              categoryNamePrefix,
                              length)) {
            const int s = manip().setLevels(recordLevel,
                                            passLevel,
                                            triggerLevel,
                                            triggerAllLevel);
            BSLS_ASSERT(0 == s);
            ++matchCount;
        }
    }
    return matchCount;
}

int bael_LoggerCategoryUtil::setThresholdLevels(
                                         bael_LoggerManager *loggerManager,
                                         const char         *regularExpression,
                                         int                 recordLevel,
                                         int                 passLevel,
                                         int                 triggerLevel,
                                         int                 triggerAllLevel)
{
    BSLS_ASSERT(loggerManager);
    BSLS_ASSERT(regularExpression);

    if (!bael_Category::areValidThresholdLevels(recordLevel,
                                                passLevel,
                                                triggerLevel,
                                                triggerAllLevel)) {
        return -1;                                                    // RETURN
    }

    bsl::size_t len = bsl::strlen(regularExpression);

    if (0 == len || '*' != regularExpression[len - 1]) {
        bael_Category *category =
                              loggerManager->lookupCategory(regularExpression);
        if (!category) {
            return 0;                                                 // RETURN
        }

        const int s = category->setLevels(recordLevel,
                                          passLevel,
                                          triggerLevel,
                                          triggerAllLevel);
        BSLS_ASSERT(0 == s);
        return 1;                                                     // RETURN
    }

    int matchCount = 0;

    if (1 == len) {  // all categories match "*"
        BSLS_ASSERT('*' == regularExpression[0]);
        for (bael_LoggerCategoryManip manip(loggerManager); manip;
                                                             manip.advance()) {
            const int s = manip().setLevels(recordLevel,
                                            passLevel,
                                            triggerLevel,
                                            triggerAllLevel);
            BSLS_ASSERT(0 == s);
            ++matchCount;
        }
        return matchCount;                                            // RETURN
    }

    // 'regularExpression' is of the form "X*".
    --len;

    for (bael_LoggerCategoryManip manip(loggerManager); manip;
                                                             manip.advance()) {
        if (0 == bsl::strncmp(manip().categoryName(),
                              regularExpression, len)) {
            const int s = manip().setLevels(recordLevel,
                                            passLevel,
                                            triggerLevel,
                                            triggerAllLevel);
            BSLS_ASSERT(0 == s);
            ++matchCount;
        }
    }
    return matchCount;
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
