// baelu_loggermanagerconfiguration.t.cpp         -*-C++-*-

#include <baelu_loggermanagerconfiguration.h>
#include <bdem_schemaaggregateutil.h>
#include <bdema_testallocator.h>

#include <cstdlib>     // atoi()
#include <cstring>     // strlen(), memset(), memcpy(), memcmp()

#include <new>          // placement 'new' syntax
#include <iostream>
#include <strstream>

using namespace BloombergLP;
using namespace std;

//=============================================================================
// TEST PLAN
//-----------------------------------------------------------------------------
// The procedures in this method create specialized instances of
// 'bael_LoggerManagerConfiguration'.  The test strategy follows the path of
// that construction.  First, we determine if configurations are created, then
// we confirm that the configurations exhibit the expected behaviors.
//-----------------------------------------------------------------------------
// [ 1] static bool isValidSchemaOneInt(const bdem_Schema&  schema,
//                                      const char         *recordName,
//                                      const char         *fieldName);
//
// [ 5] static bael_LoggerManagerConfiguration makeLmcOneInt(
//                                     const bdem_Schema&  schema,
//                                     const char         *recordName,
//                                     const char         *fieldName,
//                                     const int          *value,
//                                     bdema_Allocator    *basicAllocator = 0);
//
//-----------------------------------------------------------------------------
// [ 6] USAGE EXAMPLE
//-----------------------------------------------------------------------------
//=============================================================================
// STANDARD BDE LOOP-ASSERT TEST MACROS
//=============================================================================
// STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
static int testStatus = 0;

void aSsErT(int c, const char *s, int i)
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
   if (!(X)) { cout << #I << ": " << I << "\n"; aSsErT(1, #X, __LINE__); }}

#define LOOP2_ASSERT(I,J,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " \
              << J << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP3_ASSERT(I,J,K,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" \
              << #K << ": " << K << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP4_ASSERT(I,J,K,L,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" << \
       #K << ": " << K << "\t" << #L << ": " << L << "\n"; \
       aSsErT(1, #X, __LINE__); } }

#define LOOP5_ASSERT(I,J,K,L,M,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" << \
       #K << ": " << K << "\t" << #L << ": " << L << "\t" << \
       #M << ": " << M << "\n"; \
       aSsErT(1, #X, __LINE__); } }

#define LOOP6_ASSERT(I,J,K,L,M,N,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" << \
       #K << ": " << K << "\t" << #L << ": " << L << "\t" << \
       #M << ": " << M << "\t" << #N << ": " << N << "\n"; \
       aSsErT(1, #X, __LINE__); } }

//=============================================================================
// SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", "<< flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number
#define T_()  cout << "\t" << flush;          // Print tab w/o newline

//=============================================================================
// GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

//=============================================================================
// GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

//=============================================================================
// MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int         verbose = argc > 2;
    int     veryVerbose = argc > 3;
    int veryVeryVerbose = argc > 4;

    bdem_Schema basicSchema;
    bdem_RecordDef *recordDef = basicSchema.createRecord("ROOT-RECORD");
    recordDef->appendField(bdem_ElemType::INT, "luw");

    const bdem_RecordDef *basicRecordDef  = basicSchema.lookupRecord(
                                                                "ROOT-RECORD");
    ASSERT(basicRecordDef);
    const int             basicFieldIndex = basicRecordDef->fieldIndex("luw");
    ASSERT(0 <= basicFieldIndex);

    cout << "TEST " << __FILE__ << " CASE " << test << endl;;

    switch (test) { case 0:  // Zero is always the leading case.
      case 6: {
        // --------------------------------------------------------------------
        // TESTING USAGE EXAMPLE
        // Concerns:
        //   The usage example provided in the component header file must
        //   compile, link, and run on all platforms as shown.
        //
        // Plan:
        //   Incorporate usage example from header into driver, remove leading
        //   comment characters, and replace 'assert' with 'ASSERT'.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << "USAGE EXAMPLE"
                          << "=============" << endl;

        // Create 'schema' for 'populator'.
        bdem_Schema schema;
        bdem_RecordDef *recordDef = schema.createRecord("ROOT-RECORD");
        recordDef->appendField(bdem_ElemType::INT, "luw");
            ASSERT(baelu_LoggerManagerConfiguration::isValidSchemaOneInt(
                                                                 schema,
                                                                 "ROOT-RECORD",
                                                                 "luw"));

        // Obtain 'fieldIndex' for later use
        const int fieldIndex = recordDef->fieldIndex("luw");
            ASSERT(0 <= fieldIndex);

        // Define data source for 'populator'
        int referenceValue = 0xcafecafe;

        // Create configuration.
        bael_LoggerManagerConfiguration lmc =
            baelu_LoggerManagerConfiguration::makeLmcOneInt(schema,
                                                            "ROOT-RECORD",
                                                            "luw",
                                                            &referenceValue);

        // Confirm that configuration has specified 'schema'.
        const bdem_Schema& userSchema = lmc.userSchema();
            ASSERT(basicSchema == userSchema);

        // Obtain access to 'populator' functor.
        const bdef_Vfunc2<bdem_List *, bdem_Schema>& populator
                                                     = lmc.userPopulator();

        // Exercise 'populator' as used by 'bael_Logger'.
        bdem_List list; // empty
        int       testValue;
        populator(&list, schema);

        // Confirm that the list received the intended referenceValue.
        testValue = list.theInt(fieldIndex);
            ASSERT(referenceValue == testValue);

        // Update 'referenceValue' and re-engage 'populator'.
        ++referenceValue;
        populator(&list, schema);

        // Confirm that the generated list gets new 'referenceValue'.
        testValue = list.theInt(fieldIndex);
            ASSERT(referenceValue == testValue);

      } break;
      case 5: {
        // --------------------------------------------------------------------
        // ALLOCATOR TEST
        //    Test that a non-default allocator is actually used.
        //
        // Concerns:
        //    Is the optional allocator actually used when it
        //    it is supplied?
        //
        // Plan:
        //    - Create an instance of 'testAllocator' and record
        //      its usage metrics.
        //    - Supply the instance of 'testAllocator' as the allocator
        //      for 'makeLmcOneInt' for the creation of a configuration.
        //    - Compare the post-creation metrics of the allocator
        //      to those recorded earlier.  They should have increased
        //      due to use of the allocator.
        //
        // Testing:
        // bael_LoggerManagerConfiguration makeLmcOneInt(
        //                             const bdem_Schema&  schema,
        //                             const char         *recordName,
        //                             const char         *fieldName,
        //                             const int          *referenceValue,
        //                             bdema_Allocator    *basicAllocator = 0);
        // --------------------------------------------------------------------
            bdema_TestAllocator basicAllocator;
            int initialNumBytesTotal = basicAllocator.numBytesTotal();
            ASSERT(0 == initialNumBytesTotal);

            int referenceValue = 0xbde0bde0, testValue;
            bael_LoggerManagerConfiguration lmc =
            baelu_LoggerManagerConfiguration::makeLmcOneInt(basicSchema,
                                                            "ROOT-RECORD",
                                                            "luw",
                                                            &referenceValue,
                                                            &basicAllocator);

            int currentNumBytesTotal = basicAllocator.numBytesTotal();
            ASSERT(initialNumBytesTotal < currentNumBytesTotal);

      } break;
      case 4: {
        // FUNCTIONAL TEST - non-empty, non-compatible list
        //    Test the configuration returned by 'makeLmcOneInt' contains a
        //    'populator' that will respect existing, non-compatible list
        //    structure.
        //
        // Concerns:
        //    - The 'bdem_List' received by the populator need not be empty.
        //    - If the received list has a structure in which the popular
        //      cannot assign its integer value, then the popular must
        //      not modify the list.
        //    - Was this feature implemented correctly?
        //
        // Plan:
        //    - Replicate the test case "FUNCTIONAL TEST - empty list"
        //      but provide a non-empty and non-compatible list to the
        //      populator.
        //    - The list should not be modified.
        // --------------------------------------------------------------------
            if (verbose)
                cout << "FUNCTIONAL TEST - non-empty, non-compatible list"
                     << endl
                     << "================================================"
                     << endl;

            int referenceValueFLOAT = 355.0/113.0;
            int referenceValueINT   = 0xbae1bae1;
            int referenceValue      = 0xbae0bae0;
            bael_LoggerManagerConfiguration lmc =
            baelu_LoggerManagerConfiguration::makeLmcOneInt(basicSchema,
                                                            "ROOT-RECORD",
                                                            "luw",
                                                            &referenceValue);

            const bdef_Vfunc2<bdem_List *, bdem_Schema>& populator
                                                         = lmc.userPopulator();

            // define a different, but compatible schema.
            bdem_Schema fancySchema;
            bdem_RecordDef *recordDef = fancySchema.createRecord("not-ROOT");
            recordDef->appendField(bdem_ElemType::FLOAT, "some other type");
            recordDef->appendField(bdem_ElemType::INT,   "some other int" );

            const bdem_RecordDef *fancyRecordDef  =
                                          fancySchema.lookupRecord("not-ROOT");
            const int fancyFieldIndex0 =
                                 fancyRecordDef->fieldIndex("some other type");
            const int fancyFieldIndex1 =
                                  fancyRecordDef->fieldIndex("some other int");

            // initialize list
            bdem_List list; // initially empty
            bdem_SchemaAggregateUtil::initListShallow(&list, *recordDef);
            list.theFloat(fancyFieldIndex0) = referenceValueFLOAT;
            list.theInt(  fancyFieldIndex1) = referenceValueINT;

            bdem_List listCopy(list);

            // exercise in the same manner 'bael_Logger'
            populator(&list, basicSchema);
            ASSERT(list == listCopy);

      } break;
      case 3: {
        // --------------------------------------------------------------------
        // FUNCTIONAL TEST - non-empty, compatible list
        //    Test the configuration returned by 'makeLmcOneInt' contains a
        //    'populator' that will use existing, compatible list structure.
        //
        // Concerns:
        //    - The 'bdem_List' received by the populator need not be empty.
        //    - If the received list has a structure in which the popular
        //      can safely assign its integer value, then the popular must
        //      do so.
        //    - Was this feature implemented correctly?
        //
        // Plan:
        //    - Replicate the test case "FUNCTIONAL TEST - empty list"
        //      but provide a non-empty but compatible list to the populator.
        //    - The test behavior should not be changed.
        // --------------------------------------------------------------------
            if (verbose)
                cout << "FUNCTIONAL TEST - non-empty, compatible list" << endl
                     << "============================================" << endl;

            int referenceValue = 0xbae1bae1, testValue;
            bael_LoggerManagerConfiguration lmc =
            baelu_LoggerManagerConfiguration::makeLmcOneInt(basicSchema,
                                                            "ROOT-RECORD",
                                                            "luw",
                                                            &referenceValue);

            const bdef_Vfunc2<bdem_List *, bdem_Schema>& populator
                                                         = lmc.userPopulator();

            // define a different, but compatible schema.
            bdem_Schema fancySchema;
            bdem_RecordDef *recordDef = fancySchema.createRecord("not-ROOT");
            recordDef->appendField(bdem_ElemType::INT, "not-luw");
            recordDef->appendField(bdem_ElemType::INT, "some other int");

            const bdem_RecordDef *fancyRecordDef  =
                                          fancySchema.lookupRecord("not-ROOT");
            const int             fancyFieldIndex0 =
                                         fancyRecordDef->fieldIndex("not-luw");
            const int             fancyFieldIndex1 =
                                  fancyRecordDef->fieldIndex("some other int");

            // initialize list
            bdem_List list; // initially empty
            bdem_SchemaAggregateUtil::initListShallow(&list, *recordDef);
            list.theInt(fancyFieldIndex0) = ~referenceValue;
            list.theInt(fancyFieldIndex1) =  referenceValue;

            // exercise in the same manner 'bael_Logger'
            populator(&list, basicSchema);
            ASSERT(referenceValue == list.theInt(fancyFieldIndex0)); // reset?
            ASSERT(referenceValue == list.theInt(fancyFieldIndex1)); // kept?

        } break;
      case 2: {
        // --------------------------------------------------------------------
        // FUNCTIONAL TEST - empty list
        //    Test the configuration returned by 'makeLmcOneInt'
        //    contains a 'populator' that will perform its intended role.
        //
        // Concerns:
        //    - When the 'populator' is used as it will be used by a
        //      'bael_Logger', it should on each invocation fetch the
        //      integer value at the location specified to 'makeLmcOneInt'.
        //    - Is the value obtained from the correct location?
        //    - Is the current value obtained on each invocation of
        //      the 'populator'?
        //
        // Plan:
        //    - Create a configuration that targets the 'populator'
        //      at an address containing a distinctive value.
        //    - Obtain access to the 'populator' from the returned
        //      configuration.
        //    - Invoke the 'populator' as it will be used by 'bael_Logger'.
        //    - Confirm that the 'bdem_List' provided to the 'populator'
        //      has been given a element containing the distinctive value.
        //    - Update the value value and repeat to confirm that
        //      the integer value is fetched on each invocation.
        //
        // Testing:
        // bael_LoggerManagerConfiguration makeLmcOneInt(
        //                             const bdem_Schema&  schema,
        //                             const char         *recordName,
        //                             const char         *fieldName,
        //                             const int          *value,
        //                             bdema_Allocator    *basicAllocator = 0);
        // --------------------------------------------------------------------
            if (verbose) cout << "FUNCTIONAL TEST - empty list" << endl
                              << "============================" << endl;
            int referenceValue = 0xbae1bae1, testValue;
            bael_LoggerManagerConfiguration lmc =
            baelu_LoggerManagerConfiguration::makeLmcOneInt(basicSchema,
                                                            "ROOT-RECORD",
                                                            "luw",
                                                            &referenceValue);

            const bdef_Vfunc2<bdem_List *, bdem_Schema>& populator
                                                         = lmc.userPopulator();
            bdem_List list;

            for (int i = 0; i < 5; ++i, ++referenceValue) {
                populator(&list, basicSchema);
                const int testValue = list.theInt(basicFieldIndex);
                ASSERT(referenceValue == testValue);
            }

        } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //
        // Concerns:
        //   We want to exercise basic functionality of this component.
        //
        // Plan:
        //   First, create 'schema' with nominally correct values
        //   of 'recordName' and  'fieldName'.
        //
        //   Vet the schema via 'isValidSchemaOneInt'.
        //
        //   Call 'makeLmcOneInt' with 'schema' and other required arguments
        //   to obtain a 'bael_LoggerManagerConfiguration'.
        //
        //   Confirm that the 'userSchema' in the returned
        //   'bael_LoggerManagerConfiguration' matches the 'schema' that was
        //   given to the 'makeLmcOneInt' procedure.
        //
        // Testing:
        //
        // bool isValidSchemaOneInt(const bdem_Schema&  schema,
        //                          const char         *recordName,
        //                          const char         *fieldName);
        //
        // bael_LoggerManagerConfiguration makeLmcOneInt(
        //                             const bdem_Schema&  schema,
        //                             const char         *recordName,
        //                             const char         *fieldName,
        //                             const int          *value,
        //                             bdema_Allocator    *basicAllocator = 0);
        // --------------------------------------------------------------------

            if (verbose) cout << "BREATHING TEST" << endl
                              << "==============" << endl;
            bdem_Schema schema;
            bdem_RecordDef *recordDef = schema.createRecord("ROOT-RECORD");
            recordDef->appendField(bdem_ElemType::INT, "luw");

            ASSERT(baelu_LoggerManagerConfiguration::isValidSchemaOneInt(
                                                                 schema,
                                                                 "ROOT-RECORD",
                                                                 "luw"));
            int value;

            bael_LoggerManagerConfiguration lmc =
                baelu_LoggerManagerConfiguration::makeLmcOneInt(basicSchema,
                                                                "ROOT-RECORD",
                                                                "luw",
                                                               &value);
            const bdem_Schema& userSchema = lmc.userSchema();
            ASSERT(schema == userSchema);

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

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2005
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
