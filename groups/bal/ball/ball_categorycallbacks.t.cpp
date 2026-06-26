// ball_categorycallbacks.t.cpp                                       -*-C++-*-
#include <ball_categorycallbacks.h>

#include <bslim_testutil.h>

#include <bsls_compilerfeatures.h>

#include <bsl_cstdlib.h>     // atoi()
#include <bsl_cstring.h>     // strncmp()
#include <bsl_iostream.h>
#include <bsl_string.h>

using namespace BloombergLP;
using bsl::cout;
using bsl::cerr;
using bsl::endl;

//=============================================================================
//                                 TEST PLAN
//-----------------------------------------------------------------------------
//                                 Overview
//                                 --------
// The component under test provides a namespace for two callback function
// types: `NameFilter` and `DefaultThresholdLevels`.  These are typedef'd
// function objects with specific signatures.  The test driver verifies that
// these types can be instantiated, assigned, and invoked correctly.
//-----------------------------------------------------------------------------
// TYPES
// [ 2] typedef NameFilter
// [ 3] typedef DefaultThresholdLevels
//
// CREATORS
// [ 1] Basic type validation
//
//-----------------------------------------------------------------------------
// [ 4] USAGE EXAMPLE
//-----------------------------------------------------------------------------

//=============================================================================
//                      STANDARD BDE ASSERT TEST MACROS
//-----------------------------------------------------------------------------

// BSLIM_TESTUTIL_* macros are provided by bslim_testutil.h
namespace {
int testStatus = 0;

void aSsErT(bool condition, const char *message, int line)
{
    if (condition) {
        cout << "Error " << __FILE__ << "(" << line << "): " << message
             << "    (failed)" << endl;

        if (0 <= testStatus && testStatus <= 100) {
            ++testStatus;
        }
    }
}

}  // close unnamed namespace

// ============================================================================
//               STANDARD BDE TEST DRIVER MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT       BSLIM_TESTUTIL_ASSERT
#define ASSERTV      BSLIM_TESTUTIL_ASSERTV

#define LOOP_ASSERT  BSLIM_TESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLIM_TESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLIM_TESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLIM_TESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLIM_TESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLIM_TESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLIM_TESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLIM_TESTUTIL_LOOP6_ASSERT

#define Q            BSLIM_TESTUTIL_Q   // Quote identifier literally.
#define P            BSLIM_TESTUTIL_P   // Print identifier and value.
#define P_           BSLIM_TESTUTIL_P_  // P(X) without '\n'.
#define T_           BSLIM_TESTUTIL_T_  // Print a tab (w/o newline).
#define L_           BSLIM_TESTUTIL_L_  // current Line number

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

typedef ball::CategoryCallbacks Obj;

// Sample NameFilter callback for testing
void sampleNameFilter(bsl::string *result, const char *input)
{
    ASSERT(result);
    ASSERT(input);
    *result = bsl::string("filtered_") + input;
}

// Sample DefaultThresholdLevels callback for testing
void sampleThresholdLevels(int        *recordLevel,
                           int        *passLevel,
                           int        *triggerLevel,
                           int        *triggerAllLevel,
                           const char *categoryName)
{
    ASSERT(recordLevel);
    ASSERT(passLevel);
    ASSERT(triggerLevel);
    ASSERT(triggerAllLevel);
    ASSERT(categoryName);

    *recordLevel     = 160;
    *passLevel       = 96;
    *triggerLevel    = 64;
    *triggerAllLevel = 32;
}

// ============================================================================
//                             USAGE EXAMPLE
// ----------------------------------------------------------------------------

///Example 1: Using NameFilter
///- - - - - - - - - - - - - -
// In this example, we demonstrate how to use the `NameFilter` callback type
// to translate category names.  Suppose we want to add a prefix to all
// category names:
//
// First, we define a function that implements the name filtering logic:
// ```
void myNameFilter(bsl::string *result, const char *categoryName)
{
    *result = bsl::string("filtered_") + categoryName;
}
// ```

///Example 2: Using DefaultThresholdLevels
/// - - - - - - - - - - - - - - - - - - -
// In this example, we demonstrate how to use the `DefaultThresholdLevels`
// callback type to set default threshold levels for categories.
//
// First, we define a function that implements the threshold level logic.  In
// this example, we provide more verbose logging for categories starting with
// "CORE":
// ```
void myThresholdLevels(int        *recordLevel,
                       int        *passLevel,
                       int        *triggerLevel,
                       int        *triggerAllLevel,
                       const char *categoryName)
{
    if (0 == bsl::strncmp(categoryName, "CORE", 4)) {
        // More verbose logging for CORE categories
        *recordLevel     = 192;
        *passLevel       = 128;
        *triggerLevel    = 96;
        *triggerAllLevel = 64;
    }
    else {
        // Standard logging for other categories
        *recordLevel     = 160;
        *passLevel       = 96;
        *triggerLevel    = 64;
        *triggerAllLevel = 32;
    }
}
// ```

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int        test = argc > 1 ? atoi(argv[1]) : 0;
    int     verbose = argc > 2;
    int veryVerbose = argc > 3;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:  // Zero is always the leading case.
      case 4: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //
        // Concerns:
        // 1. The usage example provided in the component header file compiles,
        //    links, and runs as shown.
        //
        // Plan:
        // 1. Incorporate usage example from header into test driver, remove
        //    leading comment characters, and replace 'assert' with 'ASSERT'.
        //    (C-1)
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << "USAGE EXAMPLE\n"
                             "=============\n";

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
        ball::CategoryCallbacks::NameFilter filter = myNameFilter;
// ```
// Now, we can use the filter to transform category names:
// ```
        bsl::string result;
        filter(&result, "myCategory");
        ASSERT(result == "filtered_myCategory");
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
        ball::CategoryCallbacks::DefaultThresholdLevels thresholds =
                                                             myThresholdLevels;
// ```
// Now, we can use the callback to obtain threshold levels for a CORE
// category:
// ```
        int recordLevel, passLevel, triggerLevel, triggerAllLevel;
        thresholds(&recordLevel,
                   &passLevel,
                   &triggerLevel,
                   &triggerAllLevel,
                   "CORE.NETWORK");

        ASSERT(192 == recordLevel);
        ASSERT(128 == passLevel);
        ASSERT(96  == triggerLevel);
        ASSERT(64  == triggerAllLevel);
// ```
// Finally, we can verify that non-CORE categories get standard thresholds:
// ```
        thresholds(&recordLevel,
                   &passLevel,
                   &triggerLevel,
                   &triggerAllLevel,
                   "TEST.CATEGORY");

        ASSERT(160 == recordLevel);
        ASSERT(96  == passLevel);
        ASSERT(64  == triggerLevel);
        ASSERT(32  == triggerAllLevel);
// ```
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING `DefaultThresholdLevels` TYPE
        //
        // Concerns:
        // 1. The `DefaultThresholdLevels` type can be instantiated.
        //
        // 2. A `DefaultThresholdLevels` object can be assigned from a
        //    compatible function or functor.
        //
        // 3. A `DefaultThresholdLevels` object can be invoked with the correct
        //    signature.
        //
        // 4. The callback receives all parameters correctly.
        //
        // Plan:
        // 1. Create a `DefaultThresholdLevels` object and assign a function
        //    to it. (C-1, C-2)
        //
        // 2. Invoke the callback and verify it can modify output parameters.
        //    (C-3, C-4)
        //
        // 3. Test with lambda function to verify compatibility with modern
        //    C++ features.
        //
        // Testing:
        //   typedef DefaultThresholdLevels
        // --------------------------------------------------------------------

        if (verbose) cout << "TESTING `DefaultThresholdLevels` TYPE\n"
                             "=====================================\n";

        if (verbose) cout << "\tTesting with function pointer.\n";
        {
            Obj::DefaultThresholdLevels callback = sampleThresholdLevels;

            int r, p, t, ta;
            callback(&r, &p, &t, &ta, "category");

            ASSERT(160 == r);
            ASSERT(96  == p);
            ASSERT(64  == t);
            ASSERT(32  == ta);

            if (veryVerbose) {
                P_(r); P_(p); P_(t); P(ta);
            }
        }

#if BSLS_COMPILERFEATURES_FULL_CPP14
        if (verbose) cout << "\tTesting with lambda.\n";
        {
            Obj::DefaultThresholdLevels callback =
                [](int *r, int *p, int *t, int *ta, const char *name) {
                    ASSERT(name);
                    *r  = 200;
                    *p  = 100;
                    *t  = 50;
                    *ta = 25;
                };

            int r, p, t, ta;
            callback(&r, &p, &t, &ta, "test");

            ASSERT(200 == r);
            ASSERT(100 == p);
            ASSERT(50  == t);
            ASSERT(25  == ta);

            if (veryVerbose) {
                P_(r); P_(p); P_(t); P(ta);
            }
        }
#endif  // BSLS_COMPILERFEATURES_FULL_CPP14

        if (verbose) cout << "\tTesting copy construction.\n";
        {
            Obj::DefaultThresholdLevels callback1 = sampleThresholdLevels;
            Obj::DefaultThresholdLevels callback2 = callback1;

            int r, p, t, ta;
            callback2(&r, &p, &t, &ta, "category");

            ASSERT(160 == r);
            ASSERT(96  == p);
            ASSERT(64  == t);
            ASSERT(32  == ta);
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING `NameFilter` TYPE
        //
        // Concerns:
        // 1. The `NameFilter` type can be instantiated.
        //
        // 2. A `NameFilter` object can be assigned from a compatible function
        //    or functor.
        //
        // 3. A `NameFilter` object can be invoked with the correct signature.
        //
        // 4 The callback receives input correctly and can modify the output.
        //
        // Plan:
        // 1. Create a `NameFilter` object and assign a function to it.
        //    (C-1, C-2)
        //
        // 2. Invoke the callback and verify it modifies the output string.
        //    (C-3, C-4)
        //
        // 3. Test with lambda function to verify compatibility with modern
        //    C++ features.
        //
        // Testing:
        //   typedef NameFilter
        // --------------------------------------------------------------------

        if (verbose) cout << "TESTING `NameFilter` TYPE\n"
                             "=========================\n";

        if (verbose) cout << "\tTesting with function pointer.\n";
        {
            Obj::NameFilter filter = sampleNameFilter;

            bsl::string result;
            filter(&result, "input");

            ASSERT(result == "filtered_input");

            if (veryVerbose) { P(result); }
        }

#if BSLS_COMPILERFEATURES_FULL_CPP14
        if (verbose) cout << "\tTesting with lambda.\n";
        {
            Obj::NameFilter filter = [](bsl::string *out, const char *in) {
                ASSERT(out);
                ASSERT(in);
                *out = bsl::string("prefix.") + in + ".suffix";
            };

            bsl::string result;
            filter(&result, "category");

            ASSERT(result == "prefix.category.suffix");

            if (veryVerbose) {
                P(result);
            }
        }
#endif  // BSLS_COMPILERFEATURES_FULL_CPP14

        if (verbose) cout << "\tTesting copy construction.\n";
        {
            Obj::NameFilter filter1 = sampleNameFilter;
            Obj::NameFilter filter2 = filter1;

            bsl::string result;
            filter2(&result, "test");

            ASSERT(result == "filtered_test");
        }

        if (verbose) cout << "\tTesting with empty input.\n";
        {
            Obj::NameFilter filter = sampleNameFilter;

            bsl::string result;
            filter(&result, "");

            ASSERT(result == "filtered_");
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //
        // Concerns:
        // 1. The types defined in `ball::CategoryCallbacks` exist.
        //
        // 2. Objects of these types can be default constructed.
        //
        // 3. Basic operations compile.
        //
        // Plan:
        // 1. Define objects of each typedef type. (C-1, C-2)
        //
        // 2. Verify that basic operations compile. (C-3)
        //
        // Testing:
        //   Basic type validation
        // --------------------------------------------------------------------

        if (verbose) cout << "BREATHING TEST\n"
                             "==============\n";

        if (verbose) cout << "\tTesting NameFilter type.\n";
        {
            Obj::NameFilter filter;
            (void)filter;  // Suppress unused variable warning
        }

        if (verbose) cout << "\tTesting `DefaultThresholdLevels` type.\n";
        {
            Obj::DefaultThresholdLevels thresholds;
            (void)thresholds;  // Suppress unused variable warning
        }

        if (verbose) cout << "\tBasic operations compile successfully.\n";
      } break;
      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        cerr << "Error, non-zero test status = " << testStatus << "." << endl;
    }
    return testStatus;
}

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
