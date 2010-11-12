// bael_scopedattribute.t.cpp                                         -*-C++-*-
#include <bael_scopedattribute.h>

#include <bael_attributecontext.h>
#include <bael_severity.h>
#include <bael_categorymanager.h>

#include <bcema_testallocator.h>

#include <bslma_defaultallocatorguard.h>
#include <bsls_platformutil.h>

#include <bsl_cstdlib.h>
#include <bsl_iostream.h>

using namespace BloombergLP;

using bsl::cout;
using bsl::cerr;
using bsl::endl;
using bsl::flush;

//=============================================================================
//                        STANDARD BDE ASSERT TEST MACROS
//-----------------------------------------------------------------------------
static int testStatus = 0;

static void aSsErT(int c, const char *s, int i)
{
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (0 <= testStatus && testStatus <= 100) ++testStatus;
    }
}

#define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }
//-----------------------------------------------------------------------------
#define LOOP_ASSERT(I,X) { \
   if (!(X)) { cout << "Error: " << #I << ": " << I << "\n"; \
               aSsErT(1, #X, __LINE__); }}

#define LOOP2_ASSERT(I,J,X) { \
   if (!(X)) { cout << "Error: " << #I << ": " << I << "\t" << #J << ": " \
              << J << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP3_ASSERT(I,J,K,X) { \
   if (!(X)) { cout << "Error: " << #I << ": " << I << "\t" << #J << ": " \
              << J << "\t" << #K << ": " << K << "\n"; \
               aSsErT(1, #X, __LINE__); } }

#define LOOP4_ASSERT(I,J,K,L,X) { \
   if (!(X)) { cout << "Error: " << #I << ": " << I << "\t" << #J << ": " \
              << J << "\t" << #K << ": " << K << "\t" << #L << ": " << L \
              << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP5_ASSERT(I,J,K,L,M,X) { \
   if (!(X)) { cout << "Error: " << #I << ": " << I << "\t" << #J << ": " \
              << J << "\t" << #K << ": " << K << "\t" << #L << ": " << L \
              << "\t" << #M << ": " << M << "\n"; aSsErT(1, #X, __LINE__); } }

//=============================================================================
//                       SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", "<< flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number
#define T_()  cout << "\t" << flush;          // Print tab w/o newline
#define PF(X) (cout << #X " = " << (X) << endl, false)

//=============================================================================
//          GLOBAL TYPEDEFS/CONSTANTS/VARIABLES/FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------
typedef bael_ScopedAttribute Obj;

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

    bcema_TestAllocator         da("da", veryVeryVeryVerbose);
    bslma_DefaultAllocatorGuard guard(&da);

    switch (test) { case 0:  // Zero is always the leading case.
      case 2: {
        // ------------------------------------------------------------------
        // TESTING SCOPING
        //
        // Concerns: When a bael_ScopedAttribute goes out of scope, it removes
        // its attributes from the system.
        // ------------------------------------------------------------------

        if (verbose) {
            cout << "SCOPING TEST" << endl
                 << "------------" << endl;
        }

        bael_CategoryManager testManager;
        bael_AttributeContext::initialize(&testManager);

        const bael_Category *cat = testManager.addCategory(
                                                      "MyCategory",
                                                      bael_Severity::BAEL_OFF,
                                                      bael_Severity::BAEL_WARN,
                                                      bael_Severity::BAEL_OFF,
                                                      bael_Severity::BAEL_OFF);

        bael_Rule myRule1("*",
                          bael_Severity::BAEL_TRACE,
                          bael_Severity::BAEL_DEBUG,
                          bael_Severity::BAEL_INFO,
                          bael_Severity::BAEL_WARN);
        myRule1.addPredicate(bael_Predicate("serviceId", 999));
        testManager.addRule(myRule1);

        bael_Rule myRule2("*",
                          bael_Severity::BAEL_INFO,
                          bael_Severity::BAEL_WARN,
                          bael_Severity::BAEL_ERROR,
                          bael_Severity::BAEL_FATAL);
        myRule2.addPredicate(bael_Predicate("uuid", 123456));
        testManager.addRule(myRule2);

        bael_ThresholdAggregate thresholdLevels(0, 0, 0, 0);
        bael_AttributeContext::getContext()->determineThresholdLevels(
                                                  &thresholdLevels, cat);

        ASSERT(bael_Severity::BAEL_WARN == thresholdLevels.passLevel());

        {
            Obj mX("serviceId", 999);

            bael_AttributeContext::getContext()->determineThresholdLevels(
                                                  &thresholdLevels, cat);

            ASSERT(bael_Severity::BAEL_TRACE == thresholdLevels.recordLevel());
            ASSERT(bael_Severity::BAEL_DEBUG == thresholdLevels.passLevel());
            ASSERT(bael_Severity::BAEL_INFO  ==
                                            thresholdLevels.triggerLevel());
            ASSERT(bael_Severity::BAEL_WARN  ==
                                            thresholdLevels.triggerAllLevel());
        }

        {
            Obj mX("uuid", 123456);

            bael_AttributeContext::getContext()->determineThresholdLevels(
                                                  &thresholdLevels, cat);

            ASSERT(bael_Severity::BAEL_INFO  == thresholdLevels.recordLevel());
            ASSERT(bael_Severity::BAEL_WARN  == thresholdLevels.passLevel());
            ASSERT(bael_Severity::BAEL_ERROR ==
                                            thresholdLevels.triggerLevel());
            ASSERT(bael_Severity::BAEL_FATAL ==
                                            thresholdLevels.triggerAllLevel());
        }

        // The guard created below should not match *any* rules
        {
            Obj mX("serviceId", 123);

            bael_AttributeContext::getContext()->determineThresholdLevels(
                                                  &thresholdLevels, cat);

            ASSERT(bael_Severity::BAEL_OFF  == thresholdLevels.recordLevel());
            ASSERT(bael_Severity::BAEL_WARN == thresholdLevels.passLevel());
            ASSERT(bael_Severity::BAEL_OFF  == thresholdLevels.triggerLevel());
            ASSERT(bael_Severity::BAEL_OFF  ==
                                            thresholdLevels.triggerAllLevel());
        }
      }  break;
      case 1: {
        // ------------------------------------------------------------------
        // BREATHING TEST
        //
        // Concerns: When a bael_ScopedAttribute object is created, it adds a
        // "serviceId" attribute to the attribute context and a rule will match
        // against it.
        // ------------------------------------------------------------------

        if (verbose) {
            cout << "BREATHING TEST" << endl
                 << "--------------" << endl;
        }

        bcema_TestAllocator ta("ta", veryVeryVeryVerbose);
        {
            bael_CategoryManager testManager;
            bael_AttributeContext::initialize(&testManager);

            const bael_Category *cat = testManager.addCategory(
                                                      "MyCategory",
                                                      bael_Severity::BAEL_OFF,
                                                      bael_Severity::BAEL_WARN,
                                                      bael_Severity::BAEL_OFF,
                                                      bael_Severity::BAEL_OFF);

            bael_Rule myRule1("*",
                              bael_Severity::BAEL_TRACE,
                              bael_Severity::BAEL_INFO,
                              bael_Severity::BAEL_ERROR,
                              bael_Severity::BAEL_FATAL);
            myRule1.addPredicate(bael_Predicate("theInt",    1974));
            testManager.addRule(myRule1);

            bael_Rule myRule2("*",
                              bael_Severity::BAEL_OFF,
                              bael_Severity::BAEL_TRACE,
                              bael_Severity::BAEL_DEBUG,
                              bael_Severity::BAEL_INFO);
            myRule2.addPredicate(bael_Predicate("theInt64",  9876543210LL));
            testManager.addRule(myRule2);

            bael_Rule myRule3("*",
                              bael_Severity::BAEL_OFF,
                              bael_Severity::BAEL_DEBUG,
                              bael_Severity::BAEL_ERROR,
                              bael_Severity::BAEL_OFF);
            myRule3.addPredicate(bael_Predicate("theString", "furpicus"));
            testManager.addRule(myRule3);

            bael_ThresholdAggregate thresholdLevels(0, 0, 0, 0);
            bael_AttributeContext::getContext()->determineThresholdLevels(
                                                      &thresholdLevels, cat);

            ASSERT(bael_Severity::BAEL_OFF  == thresholdLevels.recordLevel());
            ASSERT(bael_Severity::BAEL_WARN == thresholdLevels.passLevel());
            ASSERT(bael_Severity::BAEL_OFF  == thresholdLevels.triggerLevel());
            ASSERT(bael_Severity::BAEL_OFF  ==
                                            thresholdLevels.triggerAllLevel());

            // Int constructor
            {
                bael_ThresholdAggregate  tl(0, 0, 0, 0);
                bael_AttributeContext   *ac =
                    bael_AttributeContext::getContext();

                Obj mX("theInt", 1974);
                ac->determineThresholdLevels(&tl, cat);

                ASSERT(bael_Severity::BAEL_TRACE == tl.recordLevel());
                ASSERT(bael_Severity::BAEL_INFO  == tl.passLevel());
                ASSERT(bael_Severity::BAEL_ERROR == tl.triggerLevel());
                ASSERT(bael_Severity::BAEL_FATAL == tl.triggerAllLevel());
            }

            // Int64 constructor
            {
                bael_ThresholdAggregate  tl(0, 0, 0, 0);
                bael_AttributeContext   *ac =
                    bael_AttributeContext::getContext();

                Obj mX("theInt64", bsls_PlatformUtil::Int64(987654321) * 10);
                ac->determineThresholdLevels(&tl, cat);

                ASSERT(bael_Severity::BAEL_OFF   == tl.recordLevel());
                ASSERT(bael_Severity::BAEL_TRACE == tl.passLevel());
                ASSERT(bael_Severity::BAEL_DEBUG == tl.triggerLevel());
                ASSERT(bael_Severity::BAEL_INFO  == tl.triggerAllLevel());
            }

            // String constructor
            {
                bael_ThresholdAggregate  tl(0, 0, 0, 0);
                bael_AttributeContext   *ac =
                    bael_AttributeContext::getContext();

                Obj mX("theString", "furpicus", &ta);
                ac->determineThresholdLevels(&tl, cat);

                ASSERT(bael_Severity::BAEL_OFF   == tl.recordLevel());
                ASSERT(bael_Severity::BAEL_DEBUG == tl.passLevel());
                ASSERT(bael_Severity::BAEL_ERROR == tl.triggerLevel());
                ASSERT(bael_Severity::BAEL_OFF   == tl.triggerAllLevel());
            }
        }
        ASSERT(0 <  ta.numAllocations());
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
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2009
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ------------------------------ END-OF-FILE ---------------------------------
