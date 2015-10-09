// ball_administration.h                                              -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BALL_ADMINISTRATION
#define INCLUDED_BALL_ADMINISTRATION

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a suite of utility functions for logging administration.
//
//@CLASSES:
//  ball::Administration: namespace for logging administration utilities
//
//@SEE_ALSO: ball_loggermanager
//
//@DESCRIPTION: This component provides a namespace, 'ball::Administration',
// containing a suite of utility functions to facilitate administration of the
// 'ball' logging subsystem from a console operator's perspective.  Utilities
// are provided for adding a category to the registry maintained by the
// singleton instance of a 'ball::LoggerManager' (hereafter "the logger
// manager"), for setting the threshold levels of a category, for setting a
// limit on the maximum number of categories allowed, and for retrieving the
// threshold levels of (established) categories.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Managing Categories
/// - - - - - - - - - - - - - - -
// The code fragments in this example demonstrate several administration
// utilities that are used to create categories, and to set and access their
// threshold levels.
//
// First we initialize the logger manager (for the purposes of this example,
// we use a minimal configuration):
//..
//     ball::DefaultObserver observer(cout);
//     ball::LoggerManagerConfiguration configuration;
//     ball::LoggerManagerScopedGuard guard(&observer, configuration);
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
// each of the category names in 'equityCategories'.  The threshold levels for
// each of the categories are set to slightly different values to help
// distinguish them when they are printed later.  The 'addCategory' method
// returns the address of the new category:
//..
//     for (int i = 0; i < NUM_CATEGORIES; ++i) {
//         int retValue = ball::Administration::addCategory(
//                                             equityCategories[i],
//                                             ball::Severity::e_TRACE + i,
//                                             ball::Severity::e_WARN  + i,
//                                             ball::Severity::e_ERROR + i,
//                                             ball::Severity::e_FATAL + i);
//         assert(0 == retValue);  // added new category
//     }
//..
// In the following, each of the new categories is accessed from the registry
// and its name and threshold levels are printed to 'bsl::cout':
//..
//     for (int i = 0; i < NUM_CATEGORIES; ++i) {
//         const char* name    = equityCategories[i];
//         int recordLevel     = ball::Administration::recordLevel(name);
//         int passLevel       = ball::Administration::passLevel(name);
//         int triggerLevel    = ball::Administration::triggerLevel(name);
//         int triggerAllLevel = ball::Administration::triggerAllLevel(name);
//
//         using namespace bsl;
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
//                   ball::Administration::setThresholdLevels(
//                                             equityCategories[i],
//                                             ball::Severity::e_TRACE - i,
//                                             ball::Severity::e_WARN  - i,
//                                             ball::Severity::e_ERROR - i,
//                                             ball::Severity::e_FATAL - i);
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
//     ball::Administration::setMaxNumCategories(NUM_CATEGORIES);
//..
// Following this call to 'setMaxNumCategories', subsequent calls to
// 'addCategory' will fail (until such time as 'setMaxNumCategories' is called
// again with an argument value of either 0 or one that is greater than
// 'NUM_CATEGORIES').

#ifndef INCLUDED_BALSCM_VERSION
#include <balscm_version.h>
#endif

namespace BloombergLP {

namespace ball {
                      // =====================
                      // struct Administration
                      // =====================

struct Administration {
    // This struct provides a namespace for a suite of utility functions that
    // simplify administration of the 'ball' logging subsystem, and insulate
    // administrative clients from changes to lower-level components of the
    // 'ball' package.  Note that all manipulation of categories is by name
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


    static int setAllThresholdLevels(int recordLevel,
                                     int passLevel,
                                     int triggerLevel,
                                     int triggerAllLevel);
        // Set *both* the default threshold levels and threshold levels of all
        // currently existing categories to the specified 'recordLevel',
        // 'passLevel', 'triggerLevel', and 'triggerAllLevel' values,
        // respectively, if each of the level values is in the range
        // [0 .. 255].  Return 0 on success, and a negative value otherwise
        // (with no effect on the default threshold levels).  The behavior is
        // undefined unless the logger manager singleton has been initialized.


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
        // 'numCategories() < maxNumCategories()'.  Note that
        // '0 < maxNumCategories() < numCategories()' *is* a valid state,
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
}  // close package namespace

// ============================================================================
//                              INLINE DEFINITIONS
// ============================================================================

// NONE BY DESIGN

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
