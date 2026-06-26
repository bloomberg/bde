// ball_categorycallbacks.h                                           -*-C++-*-
#ifndef INCLUDED_BALL_CATEGORYCALLBACKS
#define INCLUDED_BALL_CATEGORYCALLBACKS

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide category related callback function types.
//
//@CLASSES:
//  ball::CategoryCallbacks: category related callback function types
//
//@SEE_ALSO: ball_loggermanagerdefaults, ball_categorymanager
//
//@DESCRIPTION: This component provides a namespace for category related
// callback function types.  `CategoryCallbacks::NameFilter` is used to
// translate external category names to internal names, while
// `CategoryCallbacks::DefaultThresholdLevels` is used to determine default
// threshold levels for new categories.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Using NameFilter
///- - - - - - - - - - - - - -
// In this example, we demonstrate how to use the `NameFilter` callback type
// to translate category names.  Suppose we want to add a prefix to all
// category names:
//
// First, we define a function that implements the name filtering logic:
// ```
//  void myNameFilter(bsl::string *result, const char *categoryName)
//  {
//      *result = bsl::string("filtered_") + categoryName;
//  }
// ```
// Then, we create a `NameFilter` object and assign our function to it:
// ```
//  ball::CategoryCallbacks::NameFilter filter = myNameFilter;
// ```
// Now, we can use the filter to transform category names:
// ```
//  bsl::string result;
//  filter(&result, "myCategory");
//  assert(result == "filtered_myCategory");
// ```
//
///Example 2: Using DefaultThresholdLevels
/// - - - - - - - - - - - - - - - - - - -
// In this example, we demonstrate how to use the `DefaultThresholdLevels`
// callback type to set default threshold levels for categories.
//
// First, we define a function that implements the threshold level logic.  In
// this example, we provide more verbose logging for categories starting with
// "CORE":
// ```
//  void myThresholdLevels(int        *recordLevel,
//                         int        *passLevel,
//                         int        *triggerLevel,
//                         int        *triggerAllLevel,
//                         const char *categoryName)
//  {
//      if (0 == bsl::strncmp(categoryName, "CORE", 4)) {
//          // More verbose logging for CORE categories
//          *recordLevel     = 192;
//          *passLevel       = 128;
//          *triggerLevel    = 96;
//          *triggerAllLevel = 64;
//      }
//      else {
//          // Standard logging for other categories
//          *recordLevel     = 160;
//          *passLevel       = 96;
//          *triggerLevel    = 64;
//          *triggerAllLevel = 32;
//      }
//  }
// ```
// Then, we create a `DefaultThresholdLevels` object and assign our function
// to it:
// ```
//  ball::CategoryCallbacks::DefaultThresholdLevels thresholds =
//                                                        myThresholdLevels;
// ```
// Finally, we can use the callback to obtain threshold levels:
// ```
//  int recordLevel, passLevel, triggerLevel, triggerAllLevel;
//  thresholds(&recordLevel,
//             &passLevel,
//             &triggerLevel,
//             &triggerAllLevel,
//             "TEST.CATEGORY");
//
//  assert(160 == recordLevel);
//  assert(96  == passLevel);
//  assert(64  == triggerLevel);
//  assert(32  == triggerAllLevel);
// ```

#include <balscm_version.h>

#include <bsl_functional.h>
#include <bsl_string.h>

namespace BloombergLP {
namespace ball {

                          // =======================
                          // class CategoryCallbacks
                          // =======================

/// This struct provides a namespace for default threshold level related
/// callback function types.
struct CategoryCallbacks {

    // TYPES

    /// An alias to a function meeting the following contract:
    /// ```
    /// /// Load into the specified `buffer` the internal name to be used as
    /// /// the key in the `CategoryManager` registry for the specified
    /// /// external `categoryName`, or leave `buffer` unmodified to use
    /// /// `categoryName` as-is.
    /// void nameFilter(bsl::string *buffer, const char *categoryName);
    /// ```
    /// See also `CategoryManager`.
    typedef bsl::function<void(bsl::string *buffer, const char *categoryName)>
                                                                    NameFilter;

    /// An alias to a function meeting the following contract:
    /// ```
    /// /// Load into the specified `recordLevel`, `passLevel`, `triggerLevel`,
    /// /// and `triggerAllLevel` the default hierarchical threshold level
    /// /// settings for the specified `categoryName`.
    /// void defaultThresholdLevels(int        *recordLevel,
    ///                             int        *passLevel,
    ///                             int        *triggerLevel,
    ///                             int        *triggerAllLevel,
    ///                             const char *categoryName);
    /// ```
    typedef bsl::function<void(int        *recordLevel,
                               int        *passLevel,
                               int        *triggerLevel,
                               int        *triggerAllLevel,
                               const char *categoryName)>
                                                        DefaultThresholdLevels;
};

}  // close package namespace
}  // close enterprise namespace

#endif  // INCLUDED_BALL_CATEGORYCALLBACKS

// ----------------------------------------------------------------------------
// Copyright 2025 Bloomberg Finance L.P.
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
