// bsls_deprecatefeature.t.cpp                                        -*-C++-*-

#include <bsls_deprecatefeature.h>

// See the 'MACRO TESTING' section for an explanation of this macro.

#if !defined(RE_INCLUDE_CONFIGURATION)

#include <bsls_bsltestutil.h>        // for testing purposes only
#include <bsls_compilerfeatures.h>

#include <string.h>     // strcmp
#include <stdio.h>
#include <stdlib.h>     // atoi()
#include <typeinfo>     // for typeid, but bad levelization.

using namespace BloombergLP;

//=============================================================================
//                             TEST PLAN
//                             ---------
// This component provides compile-time facilities to annotate C++ entities as
// deprecated using the C++ '[[deprecated]]' annotation on platforms where it
// is available.  The instantiation of the macros are configured via build
// flags.  This test driver is designed to re-include itself, and the component
// header multiple times to simulate the inclusion of the header under
// different build configurations.  See the 'MACRO TESTING' section for more
// documentation.
//-----------------------------------------------------------------------------
// [ 1] BSLS_DEPRECATE_FEATURE(UOR, FEATURE, MESSAGE)
// [ 1] BSLS_DEPRECATE_FEATURE_IS SUPPORTED
// [ 1] BSLS_DEPRECATE_FEATURE_ANNOTATION_IS_ACTIVE
//-----------------------------------------------------------------------------
// [ 1] MACRO TEST
// [ 2] USAGE EXAMPLE

//=============================================================================
//                  STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
// NOTE: THIS IS A LOW-LEVEL COMPONENT AND MAY NOT USE ANY C++ LIBRARY
// FUNCTIONS, INCLUDING IOSTREAMS.
static int testStatus = 0;

static void aSsErT(bool b, const char *s, int i) {
    if (b) {
        printf("Error " __FILE__ "(%d): %s    (failed)\n", i, s);
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}

# define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

//=============================================================================
//                       STANDARD BDE TEST DRIVER MACROS
//-----------------------------------------------------------------------------
#define LOOP_ASSERT  BSLS_BSLTESTUTIL_LOOP_ASSERT
#define LOOP2_ASSERT BSLS_BSLTESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLS_BSLTESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLS_BSLTESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLS_BSLTESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLS_BSLTESTUTIL_LOOP6_ASSERT

#define Q   BSLS_BSLTESTUTIL_Q   // Quote identifier literally.
#define P   BSLS_BSLTESTUTIL_P   // Print identifier and value.
#define P_  BSLS_BSLTESTUTIL_P_  // P(X) without '\n'.
#define T_  BSLS_BSLTESTUTIL_T_  // Print a tab (w/o newline).
#define L_  BSLS_BSLTESTUTIL_L_  // current Line number

// ============================================================================
//                              TEST APPARATUS
// ----------------------------------------------------------------------------

static int verbose;
static int veryVerbose;

#define EMPTY_STRING ""
#define MACRO_TO_STRING(...) MACRO_TO_STRING_A((__VA_ARGS__, EMPTY_STRING))
#define MACRO_TO_STRING_A(T) MACRO_TO_STRING_B T
#define MACRO_TO_STRING_B(M, ...) #M

//=============================================================================
//                         HELPER CLASSES FOR TESTING
//-----------------------------------------------------------------------------


//=============================================================================
//                                USAGE EXAMPLE
//-----------------------------------------------------------------------------

namespace USAGE_EXAMPLE {

namespace bdlt {
class Date {
};
}  // close namespace bdlt

///Usage
///-----
// In this section we show intended usage of this component.
//
///Example 1: Deprecating a Feature
/// - - - - - - - - - - - - - - - -
// The following example demonstrates using the 'BSLS_DEPRECATE_FEATURE' macro
// to deprecate several C++ entities.
//
// The 'BSLS_DEPRECATE_FEATURE' macro can be applied in the same way as the C++
// '[[deprecated]]' annotation.  For example, imagine we are deprecating a
// function 'oldFunction' in the 'bsl' library as part of migrating software to
// the linux platform, we might write:
//..
    BSLS_DEPRECATE_FEATURE("bsl", "oldFunction", "Use newFunction instead")
    void oldFunction();
//..
// Here the string "bsl" refers to the library or Unit-Of-Release (UOR) that
// the deprecation occurs in.  "oldFunction" is an arbitrary identifier for
// the feature being deprecated.  Together the 'UOR' and 'FEATURE' are
// intended to form a unique enterprise-wide identifier for the feature being
// deprecated.  Finally the string "Use newFunction instead" is a message for
// users of the deprecated feature.
//
// Marking 'oldFunction' in this way makes the deprecation of 'oldFunction'
// visible to code analysis tools.  In addition, in a local build, warnings
// for uses of the deprecated entity can be enabled using a build macro
// 'BB_DEPRECATE_ENABLE_ALL_DEPRECATIONS_FOR_TESTING' (this macro *MUST* *NOT*
// be used as part of a cross-organization integration build such as a
// 'unstable' dpkg build, see {Concerns with Compiler Warnings}).
//
// Similarly, if we were deprecating a class 'OldType' we might write:
//..
//
    class BSLS_DEPRECATE_FEATURE("bsl", "OldType", "Use NewType instead")
                                                                      OldType {
        // ...
    };
//..
// Frequently, more than one C++ related entity may be associated with a
// deprecated feature.  In that case we would want to use the same identifier
// for each entity we mark deprecated.  To simplify this we might create a
// deprecation macro that is local to the component.  For example, if we were
// deprecating a queue and its iterator in the 'bde' library we might write:
//..
    #define BDEC_QUEUE_DEPRECATE                                              \
        BSLS_DEPRECATE_FEATURE("bde", "bdec_queue", "Use bsl::queue instead")
//
    class BDEC_QUEUE_DEPRECATE bdec_Queue {
        //...
    };
//
    class BDEC_QUEUE_DEPRECATE bdec_QueueIterator {
        //...
    };
//..
// Sometimes several entities are deprecated as part of the same
// feature where separate messages are appropriate.  For example, imagine we
// had a component 'bsls_measurementutil' that we were converting from
// imperial to metric units:
//..
    #define BSLS_MEASUREMEANTUTIL_DEPRECATE_IMPERIAL(MESSAGE)                 \
        BSLS_DEPRECATE_FEATURE("bsl", "deprecate-imperial-units", MESSAGE)
//
    struct MeasurementUtil {
//
        BSLS_MEASUREMEANTUTIL_DEPRECATE_IMPERIAL("Use getKilometers instead")
        static double getMiles();
//
        BSLS_MEASUREMEANTUTIL_DEPRECATE_IMPERIAL("Use getKilograms instead")
        static double getPounds();
    };
//
///Deprecating a Feature Across Multiple Headers
///- - - - - - - - - - - - - - - - - - - - - - -
// Frequently a feature being deprecated may span multiple components.  For
// example, we may want to deprecate all the date and time types in the 'bde'
// library.  In those instances one may define a macro in the lowest level
// component (e.g., define 'BDET_DATE_DEPRECATE_DATE_AND_TIME' in 'bdet_date').
// Alternatively, one might create a component specifically for the deprecation
// (e.g., define 'BDET_DEPRECATE_DATE_AND_TIME' in a newly created
// 'bdet_deprecate' component).  The following code shows the latter, creating
// a new component, 'bdet_deprecate' in which to provide macros to deprecate
// code across 'bdet'.
//
// First, we create a new component, 'bdet_deprecate` and define the following
// macro:
//..
    // bdet_deprecate.h
//
    #define BDET_DEPRECATE_DATE_AND_TIME(MESSAGE)                            \
        BSLS_DEPRECATE_FEATURE("bde", "date-and-time", MESSAGE)
//..
// We can use that macro to mark various components deprecated.  Next, we mark
// an old type name as deprecated:
//..
    // bdet_date.h
//
    BDET_DEPRECATE_DATE_AND_TIME("Use bdlt::Date") typedef bdlt::Date Date;
//..
// Then we mark a class declaration as deprecated:
//..
    // bdet_calendar.h
//
    class BDET_DEPRECATE_DATE_AND_TIME("Use bdlt::PackedCalendar") Calendar {
       // ...
    };
//..
// Finally we mark a function as deprecated:
//..
    // bdet_dateimputil.h
//
    struct DateUtil {
//
        BDET_DEPRECATE_DATE_AND_TIME("Use bdlt::DateUtil instead")
        static bool isValidYYYYMMDD(int yyyymmddValue);

        // ...
    };
//..

void oldFunction()
{
}

double MeasurementUtil::getMiles()
{
   return 0;
}

double MeasurementUtil::getPounds()
{
    return 0;
}

bool DateUtil::isValidYYYYMMDD(int)
{
    return false;
}

}  // close namespace USAGE_EXAMPLE

//=============================================================================
//                             MACRO TESTING
//-----------------------------------------------------------------------------
// In order to observe the behavior of the macros defined in
// 'bsls_deprecatefeature.h' under different build conditions, the test
// machinery below is designed to re-include this test driver, and the
// component header multiple times under different conditions.  Each of these
// re-inclusions will define a specialization of the 'collectMacroInformation'
// function appropriate to the build configuration.  At runtime, the test
// driver will call all the 'collectMacroInformation' specializations and
// observe the collected information for a variety of build configurations.
//
// The collection functions defined by this machinery are specializations of
// the function template:
// 'template <int CONFIGURATION> collectMacroInforation();'
//
// Where 'CONFIGURATION' is one of the enumerated values for the
// 'MACRO_CONFIGURATION' macro.
//
// The re-inclusions of this test driver and the component header are
// configured through the 'RE_INCLUDE_CONFIGURATION' macro whose value is
// interpretted as follows:
//
//: o unset: Normal (top-level) compilation of the test-driver.  This is how
//:   the compiler initially builds the test driver translation unit.
//:
//: o set: The test driver is being re-included for a particular
//:   configuration.  set the build configuration macros used by this
//:   component based on 'MACRO_CONFIGURATION', include
//:   "bsls_deprecatefeature.h" and create a specialization of
//:   'collectMacroInformation' for the current build configuration described
//:   by 'MACRO_CONFIGURATION'.
//
// The macro 'MACRO_CONFIGURATION' will be set to an integer between 0 and
// 'k_NUM_CONFIGURATION', where the bits of the integer are used to indicate
// whether the build configuration flag associated with each bit should be
// defined.  In this way the test driver "iterates" (via the preprocessor)
// through the possible build configurations.  The association between bits of
// the 'MACRO_CONFIGURATION' value and build configuration flags are defined by
// the 'k_*_BIT' constants below.

// The following constants define a bit masks that can be applied to the
// 'MACRO_CONFIGURATION' macro (which contains an integer).

#define k_BB_ENABLE_ALL_BIT   1  // set BB_DEPRECATE_ENABLE_ALL_..._FOR_TESTING
#define k_BB_JSON_BIT         2  // set BB_DEPRECATE_ENABLE_JSON_MESSAGE
#define k_BSLS_ENABLE_ALL_BIT 4  // set BSLS_DEPRECATE_FEATURE_ENABLE_ALL_...
#define k_BSLS_JSON_BIT       8  // set DEPRECATE_ENABLE_JSON_MESSAGE

// The number of possible values for 'MACRO_CONFIGURATION'.

#define k_NUM_CONFIGURATIONS  16

struct CollectedData {
    // A struct holding information about the instantiations of the macros in
    // 'bsls_deprecatefeature.h' for a particular build configuration.

    int         d_isSupportedDefined;
    int         d_isEnabledDefined;
    const char *d_macroText;
};

template <int CONFIGURATION>
void collectMacroInformation(CollectedData *result);
    // Populate the specified 'result' for the specified (template-parameter)
    // 'CONFIGURATION'.  There is no definition for the generic template.
    // Specializations for this template are defined by re-including this
    // test driver with 'RE_INCLUDE_CONFIGURATION' set to 'k_SET_CONFIGURATION'
    // and 'MACRO_CONFIGURATION' set to the appropriate 'CONFIGURATION' value.

// Re-include this test driver with 'RE_INCLUDE_CONFIGURATION' and
// 'MACRO_CONFIGURATION' set to define specializations for
// 'collectMacroInformation' for each configuration.

#define RE_INCLUDE_CONFIGURATION 1

#define MACRO_CONFIGURATION 0
#include "bsls_deprecatefeature.t.cpp"
#undef MACRO_CONFIGURATION
#define MACRO_CONFIGURATION 1
#include "bsls_deprecatefeature.t.cpp"
#undef MACRO_CONFIGURATION
#define MACRO_CONFIGURATION 2
#include "bsls_deprecatefeature.t.cpp"
#undef MACRO_CONFIGURATION
#define MACRO_CONFIGURATION 3
#include "bsls_deprecatefeature.t.cpp"
#undef MACRO_CONFIGURATION
#define MACRO_CONFIGURATION 4
#include "bsls_deprecatefeature.t.cpp"
#undef MACRO_CONFIGURATION
#define MACRO_CONFIGURATION 5
#include "bsls_deprecatefeature.t.cpp"
#undef MACRO_CONFIGURATION
#define MACRO_CONFIGURATION 6
#include "bsls_deprecatefeature.t.cpp"
#undef MACRO_CONFIGURATION
#define MACRO_CONFIGURATION 7
#include "bsls_deprecatefeature.t.cpp"
#undef MACRO_CONFIGURATION
#define MACRO_CONFIGURATION 8
#include "bsls_deprecatefeature.t.cpp"
#undef MACRO_CONFIGURATION
#define MACRO_CONFIGURATION 9
#include "bsls_deprecatefeature.t.cpp"
#undef MACRO_CONFIGURATION
#define MACRO_CONFIGURATION 10
#include "bsls_deprecatefeature.t.cpp"
#undef MACRO_CONFIGURATION
#define MACRO_CONFIGURATION 11
#include "bsls_deprecatefeature.t.cpp"
#undef MACRO_CONFIGURATION
#define MACRO_CONFIGURATION 12
#include "bsls_deprecatefeature.t.cpp"
#undef MACRO_CONFIGURATION
#define MACRO_CONFIGURATION 13
#include "bsls_deprecatefeature.t.cpp"
#undef MACRO_CONFIGURATION
#define MACRO_CONFIGURATION 14
#include "bsls_deprecatefeature.t.cpp"
#undef MACRO_CONFIGURATION
#define MACRO_CONFIGURATION 15
#include "bsls_deprecatefeature.t.cpp"

#undef RE_INCLUDE_CONFIGURATION

#endif  // #if !defined(RE_INCLUDE_CONFIGURATION)

///Re-Include Step
//----------------
// The following logic is performed when this test driver is re-included with
// 'RE_INCLUDE_CONFIGURATION' set.

#if defined(RE_INCLUDE_CONFIGURATION)

// Clean the macro environment.

#undef BSLS_DEPRECATE_FEATURE
#undef BSLS_DEPRECATE_FEATURE_IS_SUPPORTED
#undef BSLS_DEPRECATE_FEATURE_ANNOTATION_IS_ACTIVE
#undef BSLS_DEPRECATE_FEATURE_IMP
#undef BSLS_DEPRECATE_FEATURE_EXPAND
#undef BSLS_DEPRECATE_FEATURE_NUM_ARGS_IMPL
#undef BSLS_DEPRECATE_FEATURE_NUM_ARGS
#undef BB_DEPRECATE_ENABLE_ALL_DEPRECATIONS_FOR_TESTING
#undef BB_DEPRECATE_ENABLE_JSON_MESSAGE
#undef BSLS_DEPRECATE_FEATURE_ENABLE_ALL_DEPRECATIONS_FOR_TESTING
#undef BSLS_DEPRECATE_FEATURE_ENABLE_JSON_MESSAGE

// Based on 'MACRO_CONFIGURATION' set up a build configuration.

#if (MACRO_CONFIGURATION & k_BB_ENABLE_ALL_BIT)
#define BB_DEPRECATE_ENABLE_ALL_DEPRECATIONS_FOR_TESTING 1
#endif

#if (MACRO_CONFIGURATION & k_BB_JSON_BIT)
#define BB_DEPRECATE_ENABLE_JSON_MESSAGE 1
#endif

#if (MACRO_CONFIGURATION & k_BSLS_ENABLE_ALL_BIT)
#define BSLS_DEPRECATE_FEATURE_ENABLE_ALL_DEPRECATIONS_FOR_TESTING 1
#endif

#if (MACRO_CONFIGURATION & k_BSLS_JSON_BIT)
#define BSLS_DEPRECATE_FEATURE_ENABLE_JSON_MESSAGE 1
#endif


// Include the component header.

#undef INCLUDED_BSLS_DEPRECATEFEATURE
#include <bsls_deprecatefeature.h>

// Define a specialization for 'collectMacroInformation' for the current build
// configuration.

template <>
void collectMacroInformation<MACRO_CONFIGURATION>(CollectedData *result)
{
    if (veryVerbose) {
        int config = MACRO_CONFIGURATION;
        printf("Collecting: %d\n", config);
    }


#ifdef BSLS_DEPRECATE_FEATURE_IS_SUPPORTED
    result->d_isSupportedDefined = 1;
#else
    result->d_isSupportedDefined = 0;
#endif

#ifdef BSLS_DEPRECATE_FEATURE_ANNOTATION_IS_ACTIVE
    result->d_isEnabledDefined = 1;
#else
    result->d_isEnabledDefined = 0;
#endif

    result->d_macroText =
        MACRO_TO_STRING(BSLS_DEPRECATE_FEATURE("bsl", "test", "text"));
}

#endif // defined(RE_INCLUDE_CONFIGURATION)

#if !defined(RE_INCLUDE_CONFIGURATION)

//=============================================================================
//                              TEST PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int        test = argc > 1 ? atoi(argv[1]) : 0;
            verbose = argc > 2;
        veryVerbose = argc > 3;

    (void)     verbose;
    (void) veryVerbose;

    setbuf(stdout, 0);    // Use unbuffered output

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:
      case 2: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //
        // Concerns:
        //: 1 The usage example provided in the component header file compiles,
        //:   links, and runs as shown.
        //
        // Plan:
        //: 1 Incorporate usage example from header into test driver, remove
        //:   leading comment characters, and replace 'assert' with 'ASSERT'.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose)
            printf("\nUSAGE EXAMPLE"
                   "\n=============\n");

        using namespace USAGE_EXAMPLE;

        oldFunction();

        OldType a;

        bdec_Queue         b;
        bdec_QueueIterator c;
        MeasurementUtil::getMiles();
        MeasurementUtil::getPounds();

        Date d;
        Calendar e;
        DateUtil::isValidYYYYMMDD(0);

        (void)a;
        (void)b;
        (void)c;
        (void)d;
        (void)e;

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // MACRO TEST
        //
        // Concerns:
        //: 1 That the "IS_SUPPORTED" macro is instantiated only
        //:   if the platform supported the '[[deprecated]]' C++ attribute.
        //:
        //: 2 The "IS_ENABLED" macro is instantiated only if the platform
        //:   supports the '[[deprecated]]' C++ attribute, and either
        //:   of the enablement macros is defined
        //:   ('BB_DEPRECATE_ENABLE_ALL_...' or
        //:   'BSLS_DEPRECATE_FEATURE_ENABLE_ALL...)'
        //:
        //: 3 If the deprecation macros are enabled, they are instantiated
        //:   either with the supplied message, or with a JSON text describing
        //:   the deprecation, depending on 'BB_DEPRECATE_ENABLE_JSON_MESSAGE'
        //:
        //
        // Plan:
        //: 1 Use the machinery described in the 'MACRO TESTING' section to
        //:   define specializations of 'collectMacroInformation' for
        //:   different build configurations.
        //:
        //: 2 Call 'collectMacroInformation' for each configuration, and
        //:   compare the returned results with expected results.
        //
        // Testing:
        //   BSLS_DEPRECATE_FEATURE(UOR, FEATURE, MESSAGE)
        //   BSLS_DEPRECATE_FEATURE_IS SUPPORTED
        //   BSLS_DEPRECATE_FEATURE_ANNOTATION_IS_ACTIVE
        // --------------------------------------------------------------------

        if (verbose) printf("\nMACRO TEST"
                            "\n==========\n");

#if (BSLS_COMPILERFEATURES_CPLUSPLUS >= 201703L)

        // The text is deliberately a long line.
        const char * JSON =
            R"([[deprecated("{\"library\": \"" "bsl" "\", \"feature\": \"" "test" "\", \"message\": \"" "text" "\"}")]])";

        // Bit Mask Description
        // --------------------
        // BE = k_BB_ENABLE_ALL_BIT   (1)
        // BJ = k_BB_JSON_BIT         (2)
        // DE = k_BSLS_ENABLE_ALL_BIT (4)
        // DJ = k_BSLS_JSON_BIT       (8)

        CollectedData EXPECTED_DATA[k_NUM_CONFIGURATIONS] = {
                                                        // DJ DE BJ BE
            { 1,  0, ""},                               //  0  0  0  0
            { 1,  1, "[[deprecated(\"text\")]]"},       //  0  0  0  1
            { 1,  0, "" },                              //  0  0  1  0
            { 1,  1, JSON },                            //  0  0  1  1
            { 1,  1, "[[deprecated(\"text\")]]"},       //  0  1  0  0
            { 1,  1, "[[deprecated(\"text\")]]"},       //  0  1  0  1
            { 1,  1, JSON },                            //  0  1  1  0
            { 1,  1, JSON },                            //  0  1  1  1
            { 1,  0, ""},                               //  1  0  0  0
            { 1,  1, JSON},                             //  1  0  0  1
            { 1,  0, ""},                               //  1  0  1  0
            { 1,  1, JSON },                            //  1  0  1  1
            { 1,  1, JSON },                            //  1  1  0  0
            { 1,  1, JSON },                            //  1  1  0  1
            { 1,  1, JSON },                            //  1  1  1  0
            { 1,  1, JSON },                            //  1  1  1  1
        };
#else
        CollectedData EXPECTED_DATA[k_NUM_CONFIGURATIONS] = {
            { 0,  0, ""},
            { 0,  0, ""},
            { 0,  0, ""},
            { 0,  0, ""},
            { 0,  0, ""},
            { 0,  0, ""},
            { 0,  0, ""},
            { 0,  0, ""},
            { 0,  0, ""},
            { 0,  0, ""},
            { 0,  0, ""},
            { 0,  0, ""},
            { 0,  0, ""},
            { 0,  0, ""},
            { 0,  0, ""},
            { 0,  0, ""}
        };
#endif

        CollectedData collectedData[k_NUM_CONFIGURATIONS] = {
            {-1, -1, 0},
            {-1, -1, 0},
            {-1, -1, 0},
            {-1, -1, 0},
            {-1, -1, 0},
            {-1, -1, 0},
            {-1, -1, 0},
            {-1, -1, 0},
            {-1, -1, 0},
            {-1, -1, 0},
            {-1, -1, 0},
            {-1, -1, 0},
            {-1, -1, 0},
            {-1, -1, 0},
            {-1, -1, 0},
            {-1, -1, 0}
        };

        collectMacroInformation< 0>(&collectedData[ 0]);
        collectMacroInformation< 1>(&collectedData[ 1]);
        collectMacroInformation< 2>(&collectedData[ 2]);
        collectMacroInformation< 3>(&collectedData[ 3]);
        collectMacroInformation< 4>(&collectedData[ 4]);
        collectMacroInformation< 5>(&collectedData[ 5]);
        collectMacroInformation< 6>(&collectedData[ 6]);
        collectMacroInformation< 7>(&collectedData[ 7]);
        collectMacroInformation< 8>(&collectedData[ 8]);
        collectMacroInformation< 9>(&collectedData[ 9]);
        collectMacroInformation<10>(&collectedData[10]);
        collectMacroInformation<11>(&collectedData[11]);
        collectMacroInformation<12>(&collectedData[12]);
        collectMacroInformation<13>(&collectedData[13]);
        collectMacroInformation<14>(&collectedData[14]);
        collectMacroInformation<15>(&collectedData[15]);


        if (veryVerbose) {
            for (int i = 0; i < k_NUM_CONFIGURATIONS; ++i) {
                P_(collectedData[i].d_isSupportedDefined);
                P_(collectedData[i].d_isEnabledDefined);
                P(collectedData[i].d_macroText);
            }
        }
        for (int i = 0; i < k_NUM_CONFIGURATIONS; ++i) {
            const CollectedData& collected = collectedData[i];
            const CollectedData& EXPECTED = EXPECTED_DATA[i];

            LOOP3_ASSERT(i,
                         collected.d_isSupportedDefined,
                         EXPECTED.d_isSupportedDefined,
                         collected.d_isSupportedDefined ==
                         EXPECTED.d_isSupportedDefined);

            LOOP3_ASSERT(i,
                         collected.d_isEnabledDefined,
                         EXPECTED.d_isEnabledDefined,
                         collected.d_isEnabledDefined ==
                         EXPECTED.d_isEnabledDefined);

            LOOP3_ASSERT(i,
                         collected.d_macroText,
                         EXPECTED.d_macroText,
                         0 == strcmp(collected.d_macroText,
                                     EXPECTED.d_macroText));
        }
      } break;
      default: {
        fprintf(stderr, "WARNING: CASE `%d' NOT FOUND.\n", test);
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        fprintf(stderr, "Error, non-zero test status = %d.\n", testStatus);
    }

    return testStatus;
}

#endif  // #if !defined(RE_INCLUDE_CONFIGURATION)

// ----------------------------------------------------------------------------
// Copyright 2022 Bloomberg Finance L.P.
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
