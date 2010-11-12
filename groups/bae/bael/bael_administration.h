// bael_administration.h               -*-C++-*-
#ifndef INCLUDED_BAEL_ADMINISTRATION
#define INCLUDED_BAEL_ADMINISTRATION

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")



//@PURPOSE: Provide a suite of utility functions for logging administration.
//
//@CLASSES:
//     bael_Administration: namespace for logging administration utilities
//
//@SEE_ALSO: bael_loggermanager
//
//@AUTHOR: Hong Shi (hshi2)
//
//@DESCRIPTION: This component provides a suite of utility functions to
// facilitate administration of the 'bael' logging subsystem from a console
// operator's perspective.  Utilities are provided for adding a category to
// the registry maintained by the singleton instance of a 'bael_LoggerManager'
// (hereafter "the logger manager"), for setting the threshold levels of a
// category, for setting a limit on the maximum number of categories allowed,
// and for retrieving the threshold levels of (established) categories.
//
///USAGE
///-----
// The code fragments in this example demonstrate several administration
// utilities that are used to create categories, and to set and access their
// threshold levels.
//
// First the logger manager must be initialized.  It is sufficient for this
// example to have a null 'bael_Observer':
//..
//     bael_LoggerManager::initSingleton(0, 0, 0);
//..
// Next define some hypothetical category names:
//..
//     const char *equityCategories[] = {
//         "EQUITY.MARKET.NYSE",
//         "EQUITY.MARKET.NASDAQ",
//         "EQUITY.GRAPHICS.MATH.FACTORIAL",
//         "EQUITY.GRAPHICS.MATH.ACKERMANN"
//     };
//     const int NUM_CATEGORIES = sizeof equityCategories
//                              / sizeof equityCategories[0];
//..
// Category naming is by convention only.  In this example, we have chosen a
// hierarchical naming convention that uses '.' to separate the constituents
// of category names.
//
// In the following, the 'addCategory' method is used to define a category for
// each of the category names in 'equityCategories'.  The threshold levels
// for each of the categories are set to slightly different values to help
// distinguish them when they are printed later.  The 'addCategory' method
// returns the address of the new category:
//..
//     for (int i = 0; i < NUM_CATEGORIES; ++i) {
//         int retValue = bael_Administration::addCategory(
//                                              equityCategories[i],
//                                              bael_Severity::BAEL_TRACE + i,
//                                              bael_Severity::BAEL_WARN  + i,
//                                              bael_Severity::BAEL_ERROR + i,
//                                              bael_Severity::BAEL_FATAL + i);
//         assert(0 == retValue);  // added new category
//     }
//..
// In the following, each of the new categories is accessed from the registry
// and its name and threshold levels are printed to 'bsl::cout':
//..
//     for (int i = 0; i < NUM_CATEGORIES; ++i) {
//         const char* name    = equityCategories[i];
//         int recordLevel     = bael_Administration::recordLevel(name);
//         int passLevel       = bael_Administration::passLevel(name);
//         int triggerLevel    = bael_Administration::triggerLevel(name);
//         int triggerAllLevel = bael_Administration::triggerAllLevel(name);
//
//         using namespace std;
//         cout << "Category name: "       << name            << endl;
//         cout << "\tRecord level:      " << recordLevel     << endl;
//         cout << "\tPass level:        " << passLevel       << endl;
//         cout << "\tTrigger level:     " << triggerLevel    << endl;
//         cout << "\tTrigger-all level: " << triggerAllLevel << endl
//              << endl;
//     }
//..
// The following is printed to 'stdout':
//..
//     Category name: EQUITY.MARKET.NYSE
//             Record level:      192
//             Pass level:        96
//             Trigger level:     64
//             Trigger-all level: 32
//
//     Category name: EQUITY.MARKET.NASDAQ
//             Record level:      193
//             Pass level:        97
//             Trigger level:     65
//             Trigger-all level: 33
//
//     Category name: EQUITY.GRAPHICS.MATH.FACTORIAL
//             Record level:      194
//             Pass level:        98
//             Trigger level:     66
//             Trigger-all level: 34
//
//     Category name: EQUITY.GRAPHICS.MATH.ACKERMANN
//             Record level:      195
//             Pass level:        99
//             Trigger level:     67
//             Trigger-all level: 35
//..
// The following is similar to the first for-loop above, but this time the
// 'setThresholdLevels' method is used to modify the threshold levels of
// existing categories.  The 'setThresholdLevels' method returns 1 to indicate
// the number of existing categories that were affected by the call:
//..
//     for (int i = 0; i < NUM_CATEGORIES; ++i) {
//         const int returnValue =
//                   bael_Administration::setThresholdLevels(
//                                              equityCategories[i],
//                                              bael_Severity::BAEL_TRACE - i,
//                                              bael_Severity::BAEL_WARN  - i,
//                                              bael_Severity::BAEL_ERROR - i,
//                                              bael_Severity::BAEL_FATAL - i);
//         assert(1 == returnValue);  // modified one category
//     }
//..
// When the 'NUM_CATEGORIES' categories are accessed from the registry a second
// time and printed, the following is output to 'stdout' showing the new
// threshold levels of the categories:
//..
//     Category name: EQUITY.MARKET.NYSE
//             Record level:      192
//             Pass level:        96
//             Trigger level:     64
//             Trigger-all level: 32
//
//     Category name: EQUITY.MARKET.NASDAQ
//             Record level:      191
//             Pass level:        95
//             Trigger level:     63
//             Trigger-all level: 31
//
//     Category name: EQUITY.GRAPHICS.MATH.FACTORIAL
//             Record level:      190
//             Pass level:        94
//             Trigger level:     62
//             Trigger-all level: 30
//
//     Category name: EQUITY.GRAPHICS.MATH.ACKERMANN
//             Record level:      189
//             Pass level:        93
//             Trigger level:     61
//             Trigger-all level: 29
//..
// Finally, the category registry is closed to further additions by setting its
// maximum capacity to (the original) 'NUM_CATEGORIES':
//..
//     bael_Administration::setMaxNumCategories(NUM_CATEGORIES);
//..
// Following this call to 'setMaxNumCategories', subsequent calls to
// 'addCategory' will fail (until such time as 'setMaxNumCategories' is called
// again with an argument value of either 0 or one that is greater than
// 'NUM_CATEGORIES').

#ifndef INCLUDED_BAESCM_VERSION
#include <baescm_version.h>
#endif

namespace BloombergLP {

                      // ==========================
                      // struct bael_Administration
                      // ==========================

struct bael_Administration {
    // This struct provides a namespace for a suite of utility functions that
    // simplify administration of the 'bael' logging subsystem, and insulate
    // administrative clients from changes to lower-level components of the
    // 'bael' package.  Note that all manipulation of categories is by name
    // only.

    // CLASS METHODS
    static int addCategory(const char *categoryName,
                           int         recordLevel,
                           int         passLevel,
                           int         triggerLevel,
                           int         triggerAllLevel);
        // Add to the registry of the logger manager singleton a new category
        // having the specified 'categoryName' and the specified 'recordLevel',
        // 'passLevel', 'triggerLevel', and 'triggerAllLevel' threshold levels,
        // respectively, if (1) 'categoryName' is not present in the category
        // registry, (2) the number of categories in the registry is less than
        // the registry capacity, and (3) each of the level values is in the
        // range [0 .. 255].  Return 0 on success, and a non-zero value
        // otherwise.  The behavior is undefined unless the logger manager
        // singleton has been initialized and 'categoryName' is
        // null-terminated.

    static int setThresholdLevels(const char *regularExpression,
                                  int         recordLevel,
                                  int         passLevel,
                                  int         triggerLevel,
                                  int         triggerAllLevel);
        // Set the threshold levels of each category in the registry of the
        // logger manager singleton whose name matches the specified
        // 'regularExpression' to the specified 'recordLevel', 'passLevel',
        // 'triggerLevel', and 'triggerAllLevel' values, respectively, if each
        // of the threshold values is in the range [0 .. 255].  Return the
        // number of categories whose threshold levels were set, or a negative
        // value if the threshold values were invalid.  The behavior is
        // undefined unless the logger manager singleton has been initialized
        // and 'regularExpression' is null-terminated.  Note that the only
        // regular expressions supported in this release are of the form (1)
        // "X*" which matches every category name in the registry of the form
        // "XY" ("X" may be the empty string), and (2) "X" where X is a valid
        // category name which matches at most one category in the registry;
        // that is, only a '*' located at the end of 'regularExpression' is
        // recognized as a special character.

    static int setDefaultThresholdLevels(int recordLevel,
                                         int passLevel,
                                         int triggerLevel,
                                         int triggerAllLevel);
        // Set the default threshold levels to the specified 'recordLevel',
        // 'passLevel', 'triggerLevel', and 'triggerAllLevel' values,
        // respectively, if each of the level values is in the range
        // [0 .. 255].  Return 0 on success, and a negative value otherwise
        // (with no effect on the default threshold levels).  The behavior is
        // undefined unless the logger manager singleton has been initialized.

    static void resetDefaultThresholdLevels();
        // Reset the default threshold levels to the original
        // "factory-supplied" values.  The behavior is undefined unless the
        // logger manager singleton has been initialized.

    static int recordLevel(const char *categoryName);
        // Return the record threshold level currently set for the category
        // having the specified 'categoryName' or a negative value if no such
        // category exists.  The behavior is undefined unless the logger
        // manager singleton has been initialized and 'categoryName' is
        // null-terminated.

    static int passLevel(const char *categoryName);
        // Return the pass threshold level currently set for the category
        // having the specified 'categoryName' or a negative value if no such
        // category exists.  The behavior is undefined unless the logger
        // manager singleton has been initialized.

    static int triggerLevel(const char *categoryName);
        // Return the trigger threshold level currently set for the category
        // having the specified 'categoryName' or a negative value if no such
        // category exists.  The behavior is undefined unless the logger
        // manager singleton has been initialized.

    static int triggerAllLevel(const char *categoryName);
        // Return the trigger-all threshold level currently set for the
        // category having the specified 'categoryName' or a negative value if
        // no such category exists.  The behavior is undefined unless the
        // logger manager singleton has been initialized.

    static int defaultRecordThresholdLevel();
        // Return the default record threshold level.

    static int defaultPassThresholdLevel();
        // Return the default pass threshold level.

    static int defaultTriggerThresholdLevel();
        // Return the default trigger threshold level.

    static int defaultTriggerAllThresholdLevel();
        // Return the default trigger-all threshold level.

    static int maxNumCategories();
        // Return the current capacity of the registry of the logger manager
        // singleton.  A capacity of 0 implies no limit will be imposed;
        // otherwise, new categories may be added only if
        // numCategories() < maxNumCategories().  Note that
        // 0 < maxNumCategories() < numCategories() *is* a valid state,
        // implying no new categories may be added.

    static int numCategories();
        // Return the number of categories in the registry of the logger
        // manager singleton.

    static void setMaxNumCategories(int length);
        // Set the capacity of the registry of the logger manager singleton to
        // the specified 'length'.  If 'length' is 0, no limit will be imposed.
        // No categories are removed from the registry if the current number of
        // categories exceeds 'length'; however, subsequent attempts to add
        // categories to the registry will fail.  The behavior is undefined
        // unless the logger manager singleton has been initialized and
        // 0 <= length.
};

// ============================================================================
//                       INLINE FUNCTION DEFINITIONS
// ============================================================================

// NONE BY DESIGN

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
