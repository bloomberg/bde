// ball_loggercategoryutil.cpp                                        -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <ball_loggercategoryutil.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ball_loggercategoryutil_cpp,"$Id$ $CSID$")

#include <ball_categorymanager.h>
#include <ball_loggermanager.h>
#include <ball_testobserver.h>                // for testing only
#include <ball_thresholdaggregate.h>

#include <bdlf_bind.h>
#include <bdlf_placeholder.h>

#include <bsls_assert.h>

#include <bsl_cstring.h>    // strlen(), strncmp()
#include <bsl_string.h>

namespace BloombergLP {
namespace ball {

// STATIC HELPER FUNCTIONS
static void setThesholdIfMatchingCategoryPrefix(
                                     Category                 *category,
                                     int                      *matchCount,
                                     const bslstl::StringRef&  prefix,
                                     int                       recordLevel,
                                     int                       passLevel,
                                     int                       triggerLevel,
                                     int                       triggerAllLevel)
    // If the specified 'category' has a name that begins with the specified
    // 'prefix', then set the category levels to the specified 'recordLevel',
    // 'passLevel', 'triggerLevel', and 'triggerAllLevel' (respectively) and
    // increment 'matchCount'.  This operation has no effect if the name of
    // 'category' does not being with 'prefix'.
{
    if (0 == bsl::strncmp(category->categoryName(),
                          prefix.data(),
                          prefix.length())) {
        const int s = category->setLevels(recordLevel,
                                          passLevel,
                                          triggerLevel,
                                          triggerAllLevel);
        (void)s; BSLS_ASSERT(0 == s);
        ++(*matchCount);
    }
}

static void accumulateLongerPrefixCategory(const Category **result,
                                           int             *minPrefixLength,
                                           const char      *categoryName,
                                           const Category  *category)
    // If the specified 'category' has a name that is a prefix of
    // 'categoryName' longer than 'minPrefixLength', then populate 'result'
    // with 'category', and populate 'minPrefixLength' with the length of
    // 'category'.  This operation has no effect if the name of 'category' is
    // not a prefix of 'categoryName' longer than the supplied
    // 'minPrefixLength'.
{
    int length = static_cast<int>(bsl::strlen(category->categoryName()));
    if (*minPrefixLength < length &&
        0 == bsl::strncmp(category->categoryName(), categoryName, length)) {
        *result          = category;
        *minPrefixLength = length;
    }
}

                            // -------------------------
                            // struct LoggerCategoryUtil
                            // -------------------------

// CLASS METHODS
Category *LoggerCategoryUtil::addCategoryHierarchically(
                                                  LoggerManager *loggerManager,
                                                  const char    *categoryName)
{
    BSLS_ASSERT(loggerManager);
    BSLS_ASSERT(categoryName);

    if (loggerManager->lookupCategory(categoryName)) {
        return 0;                                                     // RETURN
    }

    const Category *longestPrefixCategory =  0;
    int             longestPrefixLength   = -1;
    {
        using namespace bdlf::PlaceHolders;
        loggerManager->visitCategories(
                           bdlf::BindUtil::bind(accumulateLongerPrefixCategory,
                                                &longestPrefixCategory,
                                                &longestPrefixLength,
                                                categoryName,
                                                _1));
    }

    Category *category = 0;
    if (0 < longestPrefixLength) {    // exclude match of default category
        category = loggerManager->addCategory(
                                     categoryName,
                                     longestPrefixCategory->recordLevel(),
                                     longestPrefixCategory->passLevel(),
                                     longestPrefixCategory->triggerLevel(),
                                     longestPrefixCategory->triggerAllLevel());
    }
    else {
        ThresholdAggregate levels;
        if (0 != loggerManager->thresholdLevelsForNewCategory(&levels,
                                                              categoryName)) {
            return 0;                                                 // RETURN
        }

        category = loggerManager->addCategory(categoryName,
                                              levels.recordLevel(),
                                              levels.passLevel(),
                                              levels.triggerLevel(),
                                              levels.triggerAllLevel());
    }

    return category;
}

int LoggerCategoryUtil::setThresholdLevelsHierarchically(
                                             LoggerManager *loggerManager,
                                             const char    *categoryNamePrefix,
                                             int            recordLevel,
                                             int            passLevel,
                                             int            triggerLevel,
                                             int            triggerAllLevel)
{
    BSLS_ASSERT(loggerManager);
    BSLS_ASSERT(categoryNamePrefix);

    if (!Category::areValidThresholdLevels(recordLevel,
                                           passLevel,
                                           triggerLevel,
                                           triggerAllLevel)) {
        return -1;                                                    // RETURN
    }

    int matchCount = 0;

    using namespace bdlf::PlaceHolders;
    loggerManager->visitCategories(
        bdlf::BindUtil::bind(setThesholdIfMatchingCategoryPrefix,
                             _1,
                             &matchCount,
                             bslstl::StringRef(categoryNamePrefix),
                             recordLevel,
                             passLevel,
                             triggerLevel,
                             triggerAllLevel));
    return matchCount;
}

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

    bsl::size_t len = bsl::strlen(pattern);

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

    bsl::string truncated(pattern, &pattern[len - 1]);

    return setThresholdLevelsHierarchically(loggerManager,
                                            truncated.c_str(),
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
