// ball_scopedattribute.t.cpp                                         -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <ball_scopedattribute.h>

#include <ball_categorymanager.h>
#include <ball_attributecontext.h>
#include <ball_rule.h>
#include <ball_predicate.h>
#include <ball_severity.h>
#include <ball_thresholdaggregate.h>


#include <bslim_testutil.h>
#include <bslma_testallocator.h>

#include <bslma_defaultallocatorguard.h>

#include <bsls_types.h>

#include <bsl_cstdlib.h>
#include <bsl_iostream.h>

using namespace BloombergLP;

using bsl::cout;
using bsl::cerr;
using bsl::endl;
using bsl::flush;

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
//                  NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT_SAFE_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(EXPR)
#define ASSERT_SAFE_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPR)
#define ASSERT_PASS(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS(EXPR)
#define ASSERT_FAIL(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL(EXPR)
#define ASSERT_OPT_PASS(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS(EXPR)
#define ASSERT_OPT_FAIL(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL(EXPR)


//=============================================================================
//          GLOBAL TYPEDEFS/CONSTANTS/VARIABLES/FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------
typedef ball::ScopedAttribute Obj;

static int verbose;
static int veryVerbose;
static int veryVeryVerbose;
static int veryVeryVeryVerbose;

//=============================================================================
//                               MAIN PROGRAM
//-----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
    int test = argc > 1 ? bsl::atoi(argv[1]) : 0;
    verbose = argc > 2;
    veryVerbose = argc > 3;
    veryVeryVerbose = argc > 4;
    veryVeryVeryVerbose = argc > 5;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    bslma::TestAllocator          da("da", veryVeryVeryVerbose);
    bslma::DefaultAllocatorGuard guard(&da);

    switch (test) { case 0:  // Zero is always the leading case.
      case 2: {
        // ------------------------------------------------------------------
        // TESTING SCOPING
        //
        // Concerns: When a ball::ScopedAttribute goes out of scope, it removes
        // its attributes from the system.
        // ------------------------------------------------------------------

        if (verbose) {
            cout << "SCOPING TEST" << endl
                 << "------------" << endl;
        }

        ball::CategoryManager testManager;
        ball::AttributeContext::initialize(&testManager);

        const ball::Category *cat = testManager.addCategory(
                                                      "MyCategory",
                                                      ball::Severity::e_OFF,
                                                      ball::Severity::e_WARN,
                                                      ball::Severity::e_OFF,
                                                      ball::Severity::e_OFF);

        ball::Rule myRule1("*",
                          ball::Severity::e_TRACE,
                          ball::Severity::e_DEBUG,
                          ball::Severity::e_INFO,
                          ball::Severity::e_WARN);
        myRule1.addPredicate(ball::Predicate("serviceId", 999));
        testManager.addRule(myRule1);

        ball::Rule myRule2("*",
                          ball::Severity::e_INFO,
                          ball::Severity::e_WARN,
                          ball::Severity::e_ERROR,
                          ball::Severity::e_FATAL);
        myRule2.addPredicate(ball::Predicate("uuid", 123456));
        testManager.addRule(myRule2);

        ball::ThresholdAggregate thresholdLevels(0, 0, 0, 0);
        ball::AttributeContext::getContext()->determineThresholdLevels(
                                                  &thresholdLevels, cat);

        ASSERT(ball::Severity::e_WARN == thresholdLevels.passLevel());

        {
            Obj mX("serviceId", 999);

            ball::AttributeContext::getContext()->determineThresholdLevels(
                                                  &thresholdLevels, cat);

            ASSERT(ball::Severity::e_TRACE == thresholdLevels.recordLevel());
            ASSERT(ball::Severity::e_DEBUG == thresholdLevels.passLevel());
            ASSERT(ball::Severity::e_INFO  ==
                                            thresholdLevels.triggerLevel());
            ASSERT(ball::Severity::e_WARN  ==
                                            thresholdLevels.triggerAllLevel());
        }

        {
            Obj mX("uuid", 123456);

            ball::AttributeContext::getContext()->determineThresholdLevels(
                                                  &thresholdLevels, cat);

            ASSERT(ball::Severity::e_INFO  == thresholdLevels.recordLevel());
            ASSERT(ball::Severity::e_WARN  == thresholdLevels.passLevel());
            ASSERT(ball::Severity::e_ERROR ==
                                            thresholdLevels.triggerLevel());
            ASSERT(ball::Severity::e_FATAL ==
                                            thresholdLevels.triggerAllLevel());
        }

        // The guard created below should not match *any* rules
        {
            Obj mX("serviceId", 123);

            ball::AttributeContext::getContext()->determineThresholdLevels(
                                                  &thresholdLevels, cat);

            ASSERT(ball::Severity::e_OFF  == thresholdLevels.recordLevel());
            ASSERT(ball::Severity::e_WARN == thresholdLevels.passLevel());
            ASSERT(ball::Severity::e_OFF  == thresholdLevels.triggerLevel());
            ASSERT(ball::Severity::e_OFF  ==
                                            thresholdLevels.triggerAllLevel());
        }
      }  break;
      case 1: {
        // ------------------------------------------------------------------
        // BREATHING TEST
        //
        // Concerns: When a ball::ScopedAttribute object is created, it adds a
        // "serviceId" attribute to the attribute context and a rule will match
        // against it.
        // ------------------------------------------------------------------

        if (verbose) {
            cout << "BREATHING TEST" << endl
                 << "--------------" << endl;
        }

        bslma::TestAllocator ta("ta", veryVeryVeryVerbose);
        {
            ball::CategoryManager testManager;
            ball::AttributeContext::initialize(&testManager);

            const ball::Category *cat = testManager.addCategory(
                                                      "MyCategory",
                                                      ball::Severity::e_OFF,
                                                      ball::Severity::e_WARN,
                                                      ball::Severity::e_OFF,
                                                      ball::Severity::e_OFF);

            ball::Rule myRule1("*",
                              ball::Severity::e_TRACE,
                              ball::Severity::e_INFO,
                              ball::Severity::e_ERROR,
                              ball::Severity::e_FATAL);
            myRule1.addPredicate(ball::Predicate("theInt",    1974));
            testManager.addRule(myRule1);

            ball::Rule myRule2("*",
                              ball::Severity::e_OFF,
                              ball::Severity::e_TRACE,
                              ball::Severity::e_DEBUG,
                              ball::Severity::e_INFO);
            myRule2.addPredicate(ball::Predicate("theInt64",  9876543210LL));
            testManager.addRule(myRule2);

            ball::Rule myRule3("*",
                              ball::Severity::e_OFF,
                              ball::Severity::e_DEBUG,
                              ball::Severity::e_ERROR,
                              ball::Severity::e_OFF);
            myRule3.addPredicate(ball::Predicate("theString", "furpicus"));
            testManager.addRule(myRule3);

            ball::ThresholdAggregate thresholdLevels(0, 0, 0, 0);
            ball::AttributeContext::getContext()->determineThresholdLevels(
                                                      &thresholdLevels, cat);

            ASSERT(ball::Severity::e_OFF  == thresholdLevels.recordLevel());
            ASSERT(ball::Severity::e_WARN == thresholdLevels.passLevel());
            ASSERT(ball::Severity::e_OFF  == thresholdLevels.triggerLevel());
            ASSERT(ball::Severity::e_OFF  ==
                                            thresholdLevels.triggerAllLevel());

            // Int constructor
            {
                ball::ThresholdAggregate  tl(0, 0, 0, 0);
                ball::AttributeContext   *ac =
                    ball::AttributeContext::getContext();

                Obj mX("theInt", 1974);
                ac->determineThresholdLevels(&tl, cat);

                ASSERT(ball::Severity::e_TRACE == tl.recordLevel());
                ASSERT(ball::Severity::e_INFO  == tl.passLevel());
                ASSERT(ball::Severity::e_ERROR == tl.triggerLevel());
                ASSERT(ball::Severity::e_FATAL == tl.triggerAllLevel());
            }

            // Int64 constructor
            {
                ball::ThresholdAggregate  tl(0, 0, 0, 0);
                ball::AttributeContext   *ac =
                    ball::AttributeContext::getContext();

                Obj mX("theInt64", bsls::Types::Int64(987654321) * 10);
                ac->determineThresholdLevels(&tl, cat);

                ASSERT(ball::Severity::e_OFF   == tl.recordLevel());
                ASSERT(ball::Severity::e_TRACE == tl.passLevel());
                ASSERT(ball::Severity::e_DEBUG == tl.triggerLevel());
                ASSERT(ball::Severity::e_INFO  == tl.triggerAllLevel());
            }

            // String constructor
            {
                ball::ThresholdAggregate  tl(0, 0, 0, 0);
                ball::AttributeContext   *ac =
                    ball::AttributeContext::getContext();

                Obj mX("theString", "furpicus", &ta);
                ac->determineThresholdLevels(&tl, cat);

                ASSERT(ball::Severity::e_OFF   == tl.recordLevel());
                ASSERT(ball::Severity::e_DEBUG == tl.passLevel());
                ASSERT(ball::Severity::e_ERROR == tl.triggerLevel());
                ASSERT(ball::Severity::e_OFF   == tl.triggerAllLevel());
            }
        }
        ASSERT(0 == ta.numBytesInUse());
      }  break;
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
