// s_baltst_depthtestmessageutil.t.cpp                              -*-C++-*-
#include <s_baltst_depthtestmessageutil.h>

#include <bslim_testutil.h>

#include <bsl_iostream.h>

using namespace BloombergLP;
using bsl::cout;
using bsl::cerr;
using bsl::endl;

// ============================================================================
//                             TEST PLAN
// ----------------------------------------------------------------------------
//                             Overview
//                             --------
// The component under test provides a set of test `DepthTestMessage` values.
// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST

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

#define ASSERT_EQ(X, Y) ASSERTV(X,Y,X == Y)
#define ASSERT_NE(X, Y) ASSERTV(X,Y,X != Y)
#define ASSERT_LE(X, Y) ASSERTV(X,Y,X <= Y)
#define ASSERT_GE(X, Y) ASSERTV(X,Y,X >= Y)

#define LOOP1_ASSERT_EQ(L,X,Y) ASSERTV(L,X,Y,X == Y)
#define LOOP1_ASSERT_NE(L,X,Y) ASSERTV(L,X,Y,X != Y)

#define Q            BSLIM_TESTUTIL_Q   // Quote identifier literally.
#define P            BSLIM_TESTUTIL_P   // Print identifier and value.
#define P_           BSLIM_TESTUTIL_P_  // P(X) without '\n'.
#define T_           BSLIM_TESTUTIL_T_  // Print a tab (w/o newline).
#define L_           BSLIM_TESTUTIL_L_  // current Line number

// ============================================================================
//                       GLOBAL FUNCTIONS FOR TESTING
// ----------------------------------------------------------------------------

/// Check that the specified `xml` looks reasonable (i.e., starts with '<' and
/// ends with ">\n").
void checkXMLText(const char *xml)
{
    const std::size_t len = std::strlen(xml);

    ASSERTV(xml[0], xml[0] == '<');
    ASSERTV(len, xml[len - 1], xml[len - 1] == '\n');
    ASSERTV(len, xml[len - 2], xml[len - 2] == '>');
}

// ============================================================================
//                   GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int             test = argc > 1 ? atoi(argv[1]) : 0;
    bool         verbose = argc > 2;
    bool     veryVerbose = argc > 3;
    bool veryVeryVerbose = argc > 4;

    static_cast<void>(verbose);
    static_cast<void>(veryVerbose);
    static_cast<void>(veryVeryVerbose);

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //
        // Concerns:
        //: 1 The static `s_TEST_MESSAGES` array that contain encoded
        //:   feature test message objects has exactly `k_NUM_MESSAGES`
        //:   elements, which are reasonable `DepthTestMessage` objects.
        //
        // Plan:
        //: 1 Check that all `k_NUM_MESSAGES` elements of the array are
        //:   reasonable `DepthTestMessage` objects, and the
        //:   `k_NUM_MESSAGES + 1`th entry is the expected canary value.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        typedef s_baltst::DepthTestMessageUtil Obj;

        for (int i = 0; i != Obj::k_NUM_MESSAGES; ++i) {
            const s_baltst::DepthTestMessage& MSG = Obj::s_TEST_MESSAGES[i];

            checkXMLText(MSG.d_XML_text_p);
            ASSERTV(MSG.d_depthBER,
                    MSG.d_depthBER >= 0 && MSG.d_depthBER < 100);
            ASSERTV(MSG.d_depthJSON,
                    MSG.d_depthJSON >= 0 && MSG.d_depthJSON < 100);
            ASSERTV(MSG.d_depthXML,
                    MSG.d_depthXML >= 0 && MSG.d_depthXML < 100);
        }

        // check canary value
        {
            const s_baltst::DepthTestMessage& CANARY_MSG =
                                     Obj::s_TEST_MESSAGES[Obj::k_NUM_MESSAGES];

            ASSERT(0 ==
                   memcmp(CANARY_MSG.d_XML_text_p, "\x00\x01\x02\x03\x00", 5));
            ASSERTV(CANARY_MSG.d_depthBER, CANARY_MSG.d_depthBER == INT_MIN);
            ASSERTV(CANARY_MSG.d_depthJSON,
                   CANARY_MSG.d_depthJSON == INT_MIN + 1);
            ASSERTV(CANARY_MSG.d_depthXML,
                   CANARY_MSG.d_depthXML == INT_MIN + 2);
            ASSERT(0 == memcmp(CANARY_MSG.d_prettyJSON_p,
                               "\x01\x02\x03\x04\x00",
                               5));
            ASSERT(0 == memcmp(CANARY_MSG.d_compactJSON_p,
                               "\x02\x03\x04\x05\x00",
                               5));
            ASSERTV(CANARY_MSG.d_isValidForGeneratedMessages,
                    CANARY_MSG.d_isValidForGeneratedMessages == false);
            ASSERTV(CANARY_MSG.d_isValidForAggregate,
                    CANARY_MSG.d_isValidForAggregate == false);
        }
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
