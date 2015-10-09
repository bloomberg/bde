// ball_loggercategoryutil.h                                          -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BALL_LOGGERCATEGORYUTIL
#define INCLUDED_BALL_LOGGERCATEGORYUTIL

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a suite of utility functions for category management.
//
//@CLASSES:
//  ball::LoggerCategoryUtil: namespace for category management utilities
//
//@SEE_ALSO: ball_loggermanager, ball_categorymanager
//
//@DESCRIPTION: This component defines a 'struct', 'ball::LoggerCategoryUtil',
// that provides a set of utility functions for managing the categories
// contained in a 'ball::LoggerManager' based on the notion of hierarchy.  In
// particular, the 'setThresholdLevelsHierarchically' function modifies the
// threshold levels of each category in 'ball::LoggerManager' whose name has
// the specified string as a prefix.  The 'addCategoryHierarchically' function
// creates a new category that inherits threshold levels from the exiting
// category whose name is the longest prefix match, if such a category exists.
//
///Deprecation Notice
///------------------
// The 'setThresholdLevels' function is deprecated in favor of
// 'setThresholdLevelsHierarchically'.  The former is data-sensitive in the
// sense that the '*' located at the end of the specified category name will
// be treated as a special flag to turn on the prefix name matching, thus
// causing trouble for categories whose name ends with '*'.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Managing Categories
/// - - - - - - - - - - - - - - -
// The following code fragments illustrate basic usage of this component's
// 'setThresholdLevelsHierarchically' and 'addCategoryHierarchically' methods.
//
// For convenience, we first create an auxiliary function that prints out the
// names and threshold level values of all the categories currently in the
// logger manager, except the default one:
//..
//     void printAllCategories()
//     {
//         ball::LoggerManager& lm = ball::LoggerManager::singleton();
//         ball::LoggerCategoryManip manip(&lm);
//         for (manip.advance(); manip; manip.advance()) { // skip the default
//                                                         // category
//             const ball::Category *category
//                 = lm.lookupCategory(manip().categoryName());
//             bsl::cout << "[ " << category->categoryName()
//                       << ", " << category->recordLevel()
//                       << ", " << category->passLevel()
//                       << ", " << category->triggerLevel()
//                       << ", " << category->triggerAllLevel()
//                       << " ]" << bsl::endl;
//         }
//     }
//..
// Now we initialize the logging environment by creating a test observer
// object and a logger manager object, and set the default threshold levels of
// the logger manager object to [191, 95, 63, 31]:
//..
//     BloombergLP::ball::TestObserver  testObserver(bsl::cout);
//     BloombergLP::ball::TestObserver *TO = &testObserver;
//     ball::LoggerManager::initSingleton(TO);
//     ball::LoggerManager& lm = ball::LoggerManager::singleton();
//     lm.setDefaultThresholdLevels(191, 95, 63, 31);
//..
// We then create two new categories, "EQ" and "EQ.MARKET", by calling the
// 'addCategory' method of the logger manager class, with their threshold
// levels explicitly set to different values (which are also different from
// the default threshold levels):
//..
//     lm.addCategory("EQ", 192, 96, 64, 32);
//     lm.addCategory("EQ.MARKET", 193, 97, 65, 33);
//     printAllCategories();
//..
// The following is printed out by 'printallCategories':
//..
//     [ EQ, 192, 96, 64, 32 ]
//     [ EQ.MARKET, 193, 97, 65, 33 ]
//..
// We add a new category using 'addCategoryHierarchically':
//..
//     ball::LoggerCategoryUtil::addCategoryHierarchically(&lm,
//                                                        "EQ.MARKET.NYSE");
//     printAllCategories();
//..
// The new category with name "EQ.MARKET.NYSE" inherits its threshold levels
// from the category "EQ.MARKET" rather than having the default threshold
// levels or inheriting them from "EQ" because of the longest prefix matching
// rule:
//..
//     [ EQ, 192, 96, 64, 32 ]
//     [ EQ.MARKET, 193, 97, 65, 33 ]
//     [ EQ.MARKET.NYSE, 193, 97, 65, 33 ]
//..
// We now want to adjust the threshold levels for all categories whose name
// starts with "EQ.MARKET" using 'setThresholdLevelsHierarchically':
//..
//     ball::LoggerCategoryUtil::setThresholdLevelsHierarchically(&lm,
//                                                               "EQ.MARKET",
//                                                               194,
//                                                               98,
//                                                               66,
//                                                               34);
//     printAllCategories();
//..
// We will notice that the threshold levels of "EQ.MARKET" and
// "EQ.MARKET.NYSE" have been changed to the new values, while those of "EQ"
// remain unchanged:
//..
//     [ EQ, 192, 96, 64, 32 ]
//     [ EQ.MARKET, 194, 98, 66, 34 ]
//     [ EQ.MARKET.NYSE, 194, 98, 66, 34 ]
//..

#ifndef INCLUDED_BALSCM_VERSION
#include <balscm_version.h>
#endif

namespace BloombergLP {

                    // ===============================
                    // struct ball::LoggerCategoryUtil
                    // ===============================


namespace ball {

class LoggerManager;
class Category;

struct LoggerCategoryUtil {
    // This struct provides a suite of utility functions that facilitate the
    // management of the categories in 'LoggerManager'.

    // CLASS METHODS

    static Category *addCategoryHierarchically(LoggerManager *loggerManager,
                                               const char    *categoryName);
        // Add, to the specified 'loggerManager', a new category having the
        // specified 'categoryName'; return the address of the modifiable new
        // category on success, and 0, with no effect, if a category by that
        // name already exists or if the number of existing categories in
        // 'loggerManager' has reached the maximum capacity.  The newly created
        // category will have the same threshold levels as the category in
        // 'loggerManager' whose name is the longest prefix of 'categoryName',
        // and the default levels, if no such category exists.  The behavior is
        // undefined unless 'loggerManager' is not null and 'categoryName' is
        // null-terminated.

    static int setThresholdLevelsHierarchically(
                                            LoggerManager *loggerManager,
                                            const char    *categoryNamePrefix,
                                            int            recordLevel,
                                            int            passLevel,
                                            int            triggerLevel,
                                            int            triggerAllLevel);
        // Set, in the specified 'loggerManager', the threshold levels of every
        // category whose name has, as a prefix, the specified
        // 'categoryNamePrefix' to the specified threshold values,
        // 'recordLevel', 'passLevel', 'triggerLevel', and 'triggerAllLevel'.
        // Return the number of categories whose threshold levels were set, and
        // a negative value, with no effect, if any of the specified threshold
        // values is outside the range [0 .. 255].  The behavior is undefined
        // unless 'loggerManager' is not null and 'categoryNamePrefix' is
        // null-terminated.

    static int setThresholdLevels(LoggerManager *loggerManager,
                                  const char    *regularExpression,
                                  int            recordLevel,
                                  int            passLevel,
                                  int            triggerLevel,
                                  int            triggerAllLevel);
        // DEPRECATED:: Use 'setThresholdLevelsHierarchically' instead.
        //
        // Set the threshold levels of each category in the specified
        // 'loggerManager' whose name matches the specified 'regularExpression'
        // to the specified 'recordLevel', 'passLevel', 'triggerLevel', and
        // 'triggerAllLevel' values, respectively, if each of the threshold
        // values is in the range [0 .. 255].  Return the number of categories
        // whose threshold levels were set, or a negative value if the
        // threshold values were invalid.  The behavior is undefined unless
        // 'loggerManager' is non-null and 'regularExpression' is
        // null-terminated.  Note that the only regular expressions supported
        // in this release are of the form (1) "X*", matching every category
        // name in the registry of the form "XY", where X and Y are arbitrary
        // strings (including the empty string), and (2) "X", matching at most
        // one category in the registry, where X is an arbitrary string not
        // ending in '*'; that is, only a '*' located at the end of
        // 'regularExpression' is recognized as a special character.
};

}  // close package namespace

}  // close enterprise namespace

#endif

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
