// bdlat_enumutil.t.cpp                                               -*-C++-*-
#include <bdlat_enumutil.h>

#include <bdlat_enumfunctions.h>

#include <bdlb_string.h>

#include <bsla_maybeunused.h>

#include <bslim_testutil.h>

#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>
#include <bslma_testallocatormonitor.h>

#include <bsls_assert.h>
#include <bsls_asserttest.h>
#include <bsls_review.h>

#include <bsl_iostream.h>
#include <bsl_string.h>

using namespace BloombergLP;
using namespace bsl;

// ============================================================================
//                                 TEST PLAN
// ----------------------------------------------------------------------------
//                                 Overview
//                                 --------
// This component contains two utility function templates, which delegate most
// of their functionality to another component.  The logic directly provided by
// this component only has three possible cases, and we can test each of them
// directly for each of the two utility function templates.
// ----------------------------------------------------------------------------
// [ 1] template <class E> int fromIntOrFallbackIfEnabled(*)
// [ 1] template <class E> int fromStringOrFallbackIfEnabled(*)
// ----------------------------------------------------------------------------
// [ 2] USAGE EXAMPLE

// ============================================================================
//                     STANDARD BDE ASSERT TEST FUNCTION
// ----------------------------------------------------------------------------

namespace {

int testStatus = 0;

void aSsErT(bool condition, const char *message, int line)
{
    if (condition) {
        cout << "Error " __FILE__ "(" << line << "): " << message
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

// ============================================================================
//                     NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT_SAFE_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(EXPR)
#define ASSERT_SAFE_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPR)

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------
typedef bdlat::EnumUtil Util;

// ============================================================================
//                            CLASSES FOR TESTING
// ----------------------------------------------------------------------------

namespace test {

// A simple enumeration type that will expose 'bdlat' enumeration
// functionality, copied from bdlat_enumfunctions.t.cpp.

enum PolygonType {
    e_RHOMBUS   = 5,  // rhombus
    e_RECTANGLE = 3,  // rectangle
    e_TRIANGLE  = 1   // triangle
};

// MANIPULATORS
int bdlat_enumFromInt(PolygonType *result, int number)
    // Load into the specified 'result' the enumerator matching the specified
    // 'number'.  Return 0 on success, and a non-zero value with no effect on
    // 'result' if 'number' does not match any enumerator.
{
    switch (number) {
      case 5: {
        *result = e_RHOMBUS;
        return 0;                                                     // RETURN
      }
      case 3: {
        *result = e_RECTANGLE;
        return 0;                                                     // RETURN
      }
      case 1: {
        *result = e_TRIANGLE;
        return 0;                                                     // RETURN
      }
      default: {
        return -1;                                                    // RETURN
      }
    }
}

int bdlat_enumFromString(PolygonType *result,
                         const char  *string,
                         int          stringLength)
    // Load into the specified 'result' the enumerator matching the specified
    // 'string' of the specified 'stringLength'.  Return 0 on success, and a
    // non-zero value with no effect on 'result' if 'string' and 'stringLength'
    // do not match any enumerator.
{
    const bsl::string s(string, stringLength);
    if ("RHOMBUS" == s) {
        *result = e_RHOMBUS;
        return 0;                                                     // RETURN
    }
    if ("RECTANGLE" == s) {
        *result = e_RECTANGLE;
        return 0;                                                     // RETURN
    }
    if ("TRIANGLE" == s) {
        *result = e_TRIANGLE;
        return 0;                                                     // RETURN
    }
    return -1;
}

// ACCESSORS
void bdlat_enumToInt(int *result, const PolygonType& value)
    // Load into the specified 'result' the integer representation of the
    // enumerator value held by the specified 'value'.
{
    switch (value) {
      case e_RHOMBUS: {
        *result = 5;
        return;                                                       // RETURN
      }
      case e_RECTANGLE: {
        *result = 3;
        return;                                                       // RETURN
      }
      case e_TRIANGLE: {
        *result = 1;
        return;                                                       // RETURN
      }
    }
    ASSERT(!"invalid enumerator");
}

void bdlat_enumToString(bsl::string *result, const PolygonType& value)
    // Load into the specified 'result' the string representation of the
    // enumerator value held by the specified 'value'.
{
    switch (value) {
      case e_RHOMBUS: {
        *result = "RHOMBUS";
        return;                                                       // RETURN
      }
      case e_RECTANGLE: {
        *result = "RECTANGLE";
        return;                                                       // RETURN
      }
      case e_TRIANGLE: {
        *result = "TRIANGLE";
        return;                                                       // RETURN
      }
    }
    ASSERT(!"invalid enumerator");
}

}  // close namespace test

namespace BloombergLP {
namespace bdlat_EnumFunctions {
template <>
struct IsEnumeration<test::PolygonType> {
    enum { VALUE = 1 };
};
}  // close namespace bdlat_EnumFunctions
}  // close enterprise namespace

// ============================================================================
//                               USAGE EXAMPLE
// ----------------------------------------------------------------------------
///Usage
///-----
// This section illustrates intended use of this component.
//
// Suppose you have a C++ 'enum' type called 'ImageType' whose enumerators
// represent supported formats for image files, and it exposes "enumeration"
// behavior as described in 'bdlat_enumfunctions.h':
//..
namespace BloombergLP {
namespace mine {

enum ImageType { JPG = 0, PNG = 1, GIF = 2, UNKNOWN = 100 };

// MANIPULATORS

int bdlat_enumFromInt(ImageType *result, int number);

int bdlat_enumFromString(ImageType  *result,
                         const char *string,
                         int         stringLength);

int bdlat_enumMakeFallback(ImageType *result);

// ACCESSORS

void bdlat_enumToInt(int *result, const ImageType& value);

void bdlat_enumToString(bsl::string      *result,
                        const ImageType&  value);

bool bdlat_enumHasFallback(const ImageType&);

bool bdlat_enumIsFallback(const ImageType& value);

}  // close namespace mine
//..
// Next, we provide definitions for the 'bdlat_enum*' customization
// point function overloads:
//..
// MANIPULATORS

inline
int mine::bdlat_enumFromInt(ImageType *result, int number)
{
    enum { SUCCESS = 0, NOT_FOUND = -1 };

    switch (number) {
      case JPG: {
        *result = JPG;
        return SUCCESS;                                               // RETURN
      }
      case PNG: {
        *result = PNG;
        return SUCCESS;                                               // RETURN
      }
      case GIF: {
        *result = GIF;
        return SUCCESS;                                               // RETURN
      }
      case UNKNOWN: {
        *result = UNKNOWN;
        return SUCCESS;                                               // RETURN
      }
      default: {
        return NOT_FOUND;                                             // RETURN
      }
    }
}

inline
int mine::bdlat_enumFromString(ImageType  *result,
                               const char *string,
                               int         stringLength)
{
    enum { SUCCESS = 0, NOT_FOUND = -1 };

    if (bdlb::String::areEqualCaseless(
            "jpg", string, stringLength)) {
        *result = JPG;
        return SUCCESS;                                               // RETURN
    }

    if (bdlb::String::areEqualCaseless(
            "png", string, stringLength)) {
        *result = PNG;
        return SUCCESS;                                               // RETURN
    }

    if (bdlb::String::areEqualCaseless(
            "gif", string, stringLength)) {
        *result = GIF;
        return SUCCESS;                                               // RETURN
    }
    if (bdlb::String::areEqualCaseless(
            "unknown", string, stringLength)) {
        *result = UNKNOWN;
        return SUCCESS;                                               // RETURN
    }

    return NOT_FOUND;
}

inline
int mine::bdlat_enumMakeFallback(ImageType *result)
{
    *result = UNKNOWN;
    return 0;
}

// ACCESSORS

inline
void mine::bdlat_enumToInt(int *result, const ImageType& value)
{
    *result = static_cast<int>(value);
}

inline
void mine::bdlat_enumToString(bsl::string      *result,
                              const ImageType&  value)
{
    switch (value) {
      case JPG: {
        *result = "JPG";
      } break;
      case PNG: {
        *result = "PNG";
      } break;
      case GIF: {
        *result = "GIF";
      } break;
      case UNKNOWN: {
        *result = "UNKNOWN";
      } break;
      default: {
        *result = "INVALID";
      } break;
    }
}

inline
bool mine::bdlat_enumHasFallback(const ImageType&)
{
    return true;
}

inline
bool mine::bdlat_enumIsFallback(const ImageType& value)
{
    return value == UNKNOWN;
}
//..
// To complete the implementation of 'mine::ImageType' as an
// "enumeration" type with fallback enumerator recognized by the
// 'bdlat' framework, we specialize the necessary traits:
//..
namespace bdlat_EnumFunctions {
template <>
struct IsEnumeration<mine::ImageType> {
    enum { VALUE = 1 };
};
template <>
struct HasFallbackEnumerator<mine::ImageType> {
    enum { VALUE = 1 };
};
}  // close namespace bdlat_EnumFunctions
}  // close enterprise namespace
//..
// We can now use the methods in 'EnumUtil' to decode integral and
// string values into 'mine::ImageType' values, falling back to the
// 'mine::UNKNOWN' enumerator value when the integral or string
// value does not correspond to any enumerator:
//..
void usageExample()
{
    using namespace BloombergLP;

    mine::ImageType imageType;
    int             rc;

    rc = bdlat::EnumUtil::fromIntOrFallbackIfEnabled(&imageType, 1);
    ASSERT(0 == rc);
    ASSERT(mine::PNG == imageType);

    rc = bdlat::EnumUtil::fromIntOrFallbackIfEnabled(&imageType, 4);
    ASSERT(0 == rc);
    ASSERT(mine::UNKNOWN == imageType);

    rc = bdlat::EnumUtil::fromStringOrFallbackIfEnabled(&imageType, "GIF", 3);
    ASSERT(0 == rc);
    ASSERT(mine::GIF == imageType);

    rc = bdlat::EnumUtil::fromStringOrFallbackIfEnabled(&imageType, "WEBP", 4);
    ASSERT(0 == rc);
    ASSERT(mine::UNKNOWN == imageType);
}

// ============================================================================
//                               MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    using namespace BloombergLP;

    const int test = argc > 1 ? atoi(argv[1]) : 0;

    BSLA_MAYBE_UNUSED const bool verbose             = argc > 2;
    BSLA_MAYBE_UNUSED const bool veryVerbose         = argc > 3;
    BSLA_MAYBE_UNUSED const bool veryVeryVerbose     = argc > 4;
    BSLA_MAYBE_UNUSED const bool veryVeryVeryVerbose = argc > 5;

    bsl::cout << "TEST " << __FILE__ << " CASE " << test
              << bsl::endl;

    // CONCERN: 'BSLS_REVIEW' failures should lead to test failures.
    bsls::ReviewFailureHandlerGuard reviewGuard(
        &bsls::Review::failByAbort);

    // CONCERN: In no case does memory come from the global allocator.

    bslma::TestAllocator globalAllocator("global",
                                         veryVeryVeryVerbose);
    bslma::Default::setGlobalAllocator(&globalAllocator);

    switch (test) {
      case 0:
      case 2: {
        // --------------------------------------------------------------------
        // TESTING USAGE EXAMPLE
        //   This will test the usage example provided in the component header
        //   file.
        //
        // Concerns:
        //: 1 The usage example provided in the component header file compiles,
        //:   links, and runs as shown.
        //
        // Plan:
        //: 1 Incorporate usage example from header into test driver, remove
        //:   leading comment characters, and replace 'assert' with 'ASSERT'.
        //:   (C-1)
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose)
            bsl::cout << "TESTING USAGE EXAMPLE" "\n"
                      << "=====================" "\n";

        usageExample();
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // TESTING DECODING FUNCTIONS
        //   This will test the (only) two utility function templates provided
        //   in this component, which decode integral and string values into an
        //   enumeration, possibly with fallback behavior.
        //
        // Concerns:
        //: 1 When 'E' is a 'bdlat' enumeration type with a fallback
        //:   enumerator, the two functions below always succeed and store
        //:   either the enumerator value corresponding to the input, or (if
        //:   none exists) the fallback enumerator value.
        //: 2 When 'E' is a 'bdlat' enumeration type without a fallback
        //:   enumerator, the two functions below succeed and store the
        //:   enumerator value corresponding to the input if one exists, and
        //:   otherwise fail and leave the result unchanged.
        //
        // Plan:
        //: 1 For a 'bdlat' enumeration type that has a fallback enumerator,
        //:   call the 2 functions below with inputs that do and do not
        //:   correspond to enumerators and verify that they behave as
        //:   documented.  (C-1)
        //: 2 Repeat step 1 with a 'bdlat' enumeration type that does *not*
        //:   have a fallback enumerator.  (C-2)
        //
        // Testing:
        //   template <class E> int fromIntOrFallbackIfEnabled(*)
        //   template <class E> int fromStringOrFallbackIfEnabled(*)
        // --------------------------------------------------------------------
        if (verbose)
            bsl::cout << "TESTING DECODING FUNCTIONS" "\n"
                      << "==========================" "\n";

        {
            typedef mine::ImageType Obj;
            Obj                     mX;
            const Obj&              X = mX;
            int                     rc;

            mX = mine::PNG;

            // Transition from a known value to a known value
            rc = Util::fromIntOrFallbackIfEnabled(&mX, 2);
            ASSERT(0         == rc);
            ASSERT(mine::GIF == X);

            // Transition from a known value to an unknown value
            rc = Util::fromIntOrFallbackIfEnabled(&mX, 4);
            ASSERT(0             == rc);
            ASSERT(mine::UNKNOWN == X);

            // Transition from an unknown value to an unknown value
            rc = Util::fromIntOrFallbackIfEnabled(&mX, 4);
            ASSERT(0             == rc);
            ASSERT(mine::UNKNOWN == X);

            // Transition from a unknown value to a known value
            rc = Util::fromIntOrFallbackIfEnabled(&mX, 1);
            ASSERT(0         == rc);
            ASSERT(mine::PNG == X);

            // Repeat with the 'fromString...' function
            rc = Util::fromStringOrFallbackIfEnabled(&mX, "GIF", 3);
            ASSERT(0         == rc);
            ASSERT(mine::GIF == X);

            rc = Util::fromStringOrFallbackIfEnabled(&mX, "WEBP", 4);
            ASSERT(0             == rc);
            ASSERT(mine::UNKNOWN == X);

            rc = Util::fromStringOrFallbackIfEnabled(&mX, "WEBP", 4);
            ASSERT(0             == rc);
            ASSERT(mine::UNKNOWN == X);

            rc = Util::fromStringOrFallbackIfEnabled(&mX, "GIF", 3);
            ASSERT(0         == rc);
            ASSERT(mine::GIF == X);
        }

        {
            typedef test::PolygonType Obj;
            Obj                       mX;
            const Obj&                X = mX;
            int                       rc;

            mX = test::e_TRIANGLE;

            // Transition from a known value to a known value
            rc = Util::fromIntOrFallbackIfEnabled(&mX, 3);
            ASSERT(0                 == rc);
            ASSERT(test::e_RECTANGLE == X);

            // Now try with an unknown value
            rc = Util::fromIntOrFallbackIfEnabled(&mX, 2);
            ASSERT(0                 != rc);
            ASSERT(test::e_RECTANGLE == X);

            // Repeat with the 'fromString...' function
            rc = Util::fromStringOrFallbackIfEnabled(&mX, "TRIANGLE", 8);
            ASSERT(0                == rc);
            ASSERT(test::e_TRIANGLE == X);

            rc = Util::fromStringOrFallbackIfEnabled(&mX, "PENTAGON", 8);
            ASSERT(0                != rc);
            ASSERT(test::e_TRIANGLE == X);
        }
      } break;
      default: {
        bsl::cerr << "WARNING: CASE `" << test << "' NOT FOUND."
                  << bsl::endl;
        testStatus = -1;
      } break;
    }

    // CONCERN: In no case does memory come from the global allocator.

    ASSERTV(globalAllocator.numBlocksTotal(),
            0 == globalAllocator.numBlocksTotal());

    if (testStatus > 0) {
        bsl::cerr << "Error, non-zero test status = " << testStatus
                  << "." << bsl::endl;
    }

    return testStatus;
}

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
