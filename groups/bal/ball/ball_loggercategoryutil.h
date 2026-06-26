// ball_loggercategoryutil.h                                          -*-C++-*-
#ifndef INCLUDED_BALL_LOGGERCATEGORYUTIL
#define INCLUDED_BALL_LOGGERCATEGORYUTIL

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a suite of utility functions for category management.
//
//@CLASSES:
//  ball::LoggerCategoryUtil: namespace for category management utilities
//
//@SEE_ALSO: ball_loggermanager, ball_categorymanager
//
//@DESCRIPTION: This component defines a `struct`, `ball::LoggerCategoryUtil`,
// that provides a set of utility functions for managing the categories
// contained in a `ball::LoggerManager` based on the notion of hierarchy.
//
// **NOTE**: The functions in this component forward their calls through
// `ball::LoggerManager` to `ball::CategoryManager`, which contains the
// implementations of hierarchical category management.  See
// `ball_categorymanager` for comprehensive documentation on hierarchical
// category management, including detailed usage examples.
//
// In particular, the `setThresholdLevelsHierarchically` function modifies the
// threshold levels of each category whose name has the specified string as a
// prefix, and the `addCategoryHierarchically` function creates a new category
// that inherits threshold levels from the existing category whose name is the
// longest prefix match, if such a category exists.
//
///Deprecation Notice
///------------------
// The `setThresholdLevels` function is deprecated in favor of
// `setThresholdLevelsHierarchically`.  The former is data-sensitive in the
// sense that the `*` located at the end of the specified category name will
// be treated as a special flag to turn on the prefix name matching, thus
// causing trouble for categories whose name ends with `*`.
//
///Usage
///-----
// This section illustrates basic usage of this component's functions.  For
// comprehensive usage examples demonstrating hierarchical category management,
// including inheritance of threshold levels and prefix-based threshold
// updates, see the "Usage" section in `ball_categorymanager`.
//
///Example 1: Basic Hierarchical Category Operations
/// - - - - - - - - - - - - - - - - - - - - - - - -
// The following example demonstrates basic usage of
// `addCategoryHierarchically` and `setThresholdLevelsHierarchically`.
//
// First, assume we have initialized the logger manager with default
// thresholds:
// ```
// ball::LoggerManager& lm = ball::LoggerManager::singleton();
// lm.setDefaultThresholdLevels(191, 95, 63, 31);
// ```
// Then, we create a base category:
// ```
// lm.addCategory("EQ", 192, 96, 64, 32);
// ```
// Next, we use `addCategoryHierarchically` to create a category that inherits
// from "EQ":
// ```
// ball::LoggerCategoryUtil::addCategoryHierarchically(&lm, "EQ.MARKET");
// ```
// The new category "EQ.MARKET" will inherit threshold levels [192, 96, 64, 32]
// from "EQ".
//
// Finally, we update all categories under "EQ" using
// `setThresholdLevelsHierarchically`:
// ```
// ball::LoggerCategoryUtil::setThresholdLevelsHierarchically(&lm,
//                                                            "EQ",
//                                                            194,
//                                                            98,
//                                                            66,
//                                                            34);
// ```
// This will update threshold levels for both "EQ" and "EQ.MARKET".
//
// For more detailed examples illustrating hierarchical category management,
// longest prefix matching, and threshold inheritance strategies, see the usage
// examples in `ball_categorymanager`.

#include <balscm_version.h>

#include <ball_loggermanager.h>

#include <bsls_assert.h>

namespace BloombergLP {
namespace ball {

class LoggerManager;
class Category;

                        // =========================
                        // struct LoggerCategoryUtil
                        // =========================

/// This struct provides a suite of utility functions that facilitate the
/// management of the categories in `LoggerManager`.
struct LoggerCategoryUtil {

    // CLASS METHODS

    /// Add, to the specified `loggerManager`, a new category having the
    /// specified `categoryName`; return the address of the modifiable new
    /// category on success, and 0, with no effect, if a category by that
    /// name already exists or if the number of existing categories has reached
    /// the maximum capacity.  The newly created category will have its
    /// threshold levels chosen primarily from two sources: threshold levels of
    /// an existing category or a category setting (to which no category
    /// exists) choosing the one whose name is the longest non-empty prefix of
    /// `categoryName`.  If such a category or setting exists, and the default
    /// threshold levels (which might be overridden by a default threshold
    /// levels callback) otherwise.  See also
    /// `ball::CategoryManager::addCategoryHierarchically`.
    static Category *addCategoryHierarchically(LoggerManager *loggerManager,
                                               const char    *categoryName);

    /// In case the specified `categoryNamePrefix` is not empty set, in the
    /// specified `loggerManager`, the threshold levels of every existing and
    /// future category whose name has, as a prefix, `categoryNamePrefix` to
    /// the specified threshold values, `recordLevel`, `passLevel`,
    /// `triggerLevel`, and `triggerAllLevel`.  In case `categoryNamePrefix` is
    /// empty set the threshold level of every existing category, and remove
    /// all preliminary settings (for categories that do not exist yet).
    /// Return the number of categories whose threshold levels were set, and a
    /// negative value, with no effect, if any of the specified threshold
    /// values is outside the range `[0 .. 255]`.  The behavior is undefined
    /// unless `loggerManager` is not in the process of being destroyed.  See
    /// also `ball::CategoryManager::setThresholdLevelsHierarchically`.
    static int setThresholdLevelsHierarchically(
                                             LoggerManager *loggerManager,
                                             const char    *categoryNamePrefix,
                                             int            recordLevel,
                                             int            passLevel,
                                             int            triggerLevel,
                                             int            triggerAllLevel);

    /// Same as `setThresholdLevelsHierarchically` except that a `*` located at
    /// the end of `pattern` is recognized as a special character and removed
    /// before performing the operation.  Thus, this function sets, in the
    /// specified `loggerManager`, the threshold levels of every existing and
    /// future category whose name has, as a prefix, the substring of
    /// `pattern` excluding an optional trailing `*`, to the specified
    /// threshold values, `recordLevel`, `passLevel`, `triggerLevel`, and
    /// `triggerAllLevel`.  In case `pattern` is `*` or empty, set the
    /// threshold level of every existing category, and remove all preliminary
    /// settings (for categories that do not exist yet).  Return the number of
    /// categories whose threshold levels were set, and a negative value, with
    /// no effect, if any of the specified threshold values is outside the
    /// range `[0 .. 255]`.  The behavior is undefined unless `loggerManager`
    /// is not in the process of being destroyed.  See also
    /// `setThresholdLevelsHierarchically` and
    /// `ball::CategoryManager::setThresholdLevelsHierarchically`.
    ///
    /// @DEPRECATED: Use `setThresholdLevelsHierarchically` instead.
    static int setThresholdLevels(LoggerManager *loggerManager,
                                  const char    *pattern,
                                  int            recordLevel,
                                  int            passLevel,
                                  int            triggerLevel,
                                  int            triggerAllLevel);
};


// ============================================================================
//                              INLINE DEFINITIONS
// ============================================================================

                            // -------------------------
                            // struct LoggerCategoryUtil
                            // -------------------------
// CLASS METHODS
inline
Category *LoggerCategoryUtil::addCategoryHierarchically(
                                                  LoggerManager *loggerManager,
                                                  const char    *categoryName)
{
    BSLS_ASSERT(loggerManager);
    BSLS_ASSERT(categoryName);
    return loggerManager->addCategoryHierarchically(categoryName);
}

inline
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
    return loggerManager->setThresholdLevelsHierarchically(categoryNamePrefix,
                                                           recordLevel,
                                                           passLevel,
                                                           triggerLevel,
                                                           triggerAllLevel);
}


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
