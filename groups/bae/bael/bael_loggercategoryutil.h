// bael_loggercategoryutil.h              -*-C++-*-
#ifndef INCLUDED_BAEL_LOGGERCATEGORYUTIL
#define INCLUDED_BAEL_LOGGERCATEGORYUTIL

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")



//@PURPOSE: Provide a suite of utility functions for category management.
//
//@CLASSES:
//      bael_LoggerCategoryUtil: namespace for category management utilities
//
//@SEE_ALSO: bael_loggermanager, bael_categorymanager
//
//@AUTHOR: Hong Shi (hshi2), Gang Chen (gchen20)
//
//@DESCRIPTION: This component provides a set of utility functions for
// managing the categories contained in 'bael_LoggerManager' based on the
// notion of hierarchy.  In particular, the 'setThresholdLevelsHierarchically'
// function modifies the threshold levels of each category in
// 'bael_LoggerManager' whose name has the specified string as a prefix.  The
// 'addCategoryHierarchically' function creates a new category that inherits
// threshold levels from the exiting category whose name is the longest prefix
// match, if such a category exists.
//
///Deprecation Notice
///------------------
// The 'setThresholdLevels' function is deprecated in favor of
// 'setThresholdLevelsHierarchically'.  The former is data-sensitive in the
// sense that the '*' located at the end of the specified category name will
// be treated as a special flag to turn on the prefix name matching, thus
// causing trouble for categories whose name ends with '*'.
//
///USAGE
///-----
// The following code fragments illustrate basic usage of this component's
// 'setThresholdLevelsHierarchically' and 'addCategoryHierarchically' methods.
//
// For convenience, we first create an auxiliary function that prints out the
// names and threshold level values of all the categories currently in the
// logger manager, except the default one:
//..
//     void printAllCategories()
//     {
//         bael_LoggerManager& lm = bael_LoggerManager::singleton();
//         bael_LoggerCategoryManip manip(&lm);
//         for (manip.advance(); manip; manip.advance()) { // skip the default
//                                                         // category
//             const bael_Category *category
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
//     BloombergLP::bael_TestObserver  testObserver(bsl::cout);
//     BloombergLP::bael_TestObserver *TO = &testObserver;
//     bael_LoggerManager::initSingleton(TO);
//     bael_LoggerManager& lm = bael_LoggerManager::singleton();
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
//     bael_LoggerCategoryUtil::addCategoryHierarchically(&lm,
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
//     bael_LoggerCategoryUtil::setThresholdLevelsHierarchically(&lm,
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

#ifndef INCLUDED_BAESCM_VERSION
#include <baescm_version.h>
#endif

namespace BloombergLP {

                    // ==============================
                    // struct bael_LoggerCategoryUtil
                    // ==============================

class bael_LoggerManager;
class bael_Category;

struct bael_LoggerCategoryUtil {
    // This struct provides a suite of utility functions that facilitate the
    // management of the categories in 'bael_LoggerManager'.

    // CLASS METHODS

    static bael_Category *addCategoryHierarchically(
                                  bael_LoggerManager *loggerManager,
                                  const char         *categoryName);
        // Add, to the specified 'loggerManager', a new category having the
        // specified 'categoryName'; return the address of the modifiable new
        // category on success, and 0, with no effect, if a category by that
        // name already exists or if the number of existing categories in
        // 'loggerManager' has reached the maximum capacity.  The newly
        // created category will have the same threshold levels as the
        // category in 'loggerManager' whose name is the longest prefix of
        // 'categoryName', and the default levels, if no such category exists.
        // The behavior is undefined unless 'loggerManager' is not null and
        // 'categoryName' is null-terminated.

    static int setThresholdLevelsHierarchically(
                                  bael_LoggerManager *loggerManager,
                                  const char         *categoryNamePrefix,
                                  int                 recordLevel,
                                  int                 passLevel,
                                  int                 triggerLevel,
                                  int                 triggerAllLevel);
        // Set, in the specified 'loggerManager', the threshold levels of
        // every category whose name has, as a prefix, the specified
        // 'categoryNamePrefix' to the specified threshold values,
        // 'recordLevel', 'passLevel', 'triggerLevel', and 'triggerAllLevel'.
        // Return the number of categories whose threshold levels were set,
        // and a negative value, with no effect, if any of the specified
        // threshold values is outside the range [0 .. 255].  The behavior is
        // undefined unless 'loggerManager' is not null and
        // 'categoryNamePrefix' is null-terminated.

    static int setThresholdLevels(bael_LoggerManager *loggerManager,
                                  const char         *regularExpression,
                                  int                 recordLevel,
                                  int                 passLevel,
                                  int                 triggerLevel,
                                  int                 triggerAllLevel);
        // DEPRECATED:: Use 'setThresholdLevelsHierarchically' instead.
        //
        // Set the threshold levels of each category in the specified
        // 'loggerManager' whose name matches the specified
        // 'regularExpression' to the specified 'recordLevel', 'passLevel',
        // 'triggerLevel', and 'triggerAllLevel' values, respectively, if each
        // of the threshold values is in the range [0 .. 255].  Return the
        // number of categories whose threshold levels were set, or a negative
        // value if the threshold values were invalid.  The behavior is
        // undefined unless 'loggerManager' is non-null and
        // 'regularExpression' is null-terminated.  Note that the only regular
        // expressions supported in this release are of the form (1) "X*",
        // matching every category name in the registry of the form "XY",
        // where X and Y are arbitrary strings (including the empty string),
        // and (2) "X", matching at most one category in the registry, where X
        // is an arbitrary string not ending in '*'; that is, only a '*'
        // located at the end of 'regularExpression' is recognized as a
        // special character.
};

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2004
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
