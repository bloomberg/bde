// bael_testobserver.t.cpp         -*-C++-*-

#include <bael_testobserver.h>

#include <bael_context.h>                       // for testing only
#include <bael_record.h>                        // for testing only
#include <bael_recordattributes.h>              // for testing only

#include <bdetu_datetime.h>                     // for testing only
#include <bdex_testinstream.h>                  // for testing only
#include <bdex_testinstreamexception.h>         // for testing only
#include <bdex_testoutstream.h>                 // for testing only

#include <bslma_testallocator.h>                // for testing only
#include <bslma_testallocatorexception.h>       // for testing only
#include <bsls_platformutil.h>                  // for testing only

#include <bsl_cstdlib.h>     // atoi()
#include <bsl_cstring.h>     // strlen(), memset(), memcpy(), memcmp()
#include <bsl_ctime.h>       // time()

#include <bsl_new.h>         // placement 'new' syntax
#include <bsl_iostream.h>
#include <bsl_strstream.h>

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

//=============================================================================
//                              TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// TBD
//-----------------------------------------------------------------------------
// [  ] bael_TestObserver(bslma_Allocator *ba = 0);
// [  ] bael_TestObserver(bael_Observer *observer, *ba = 0);
// [  ] virtual ~bael_TestObserver();
// [  ] virtual void publish(const record&, const attributes&) = 0;
// [  ] int registerObserver(bael_Observer *observer);
// [  ] int deregisterObserver(bael_Observer *observer);
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST - BUG
// [ 2] USAGE TEST - Make sure main usage example compiles and works properly.

//=============================================================================
//                      STANDARD BDE ASSERT TEST MACRO
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

//=============================================================================
//                  STANDARD BDE LOOP-ASSERT TEST MACROS
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
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", "<< flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number
#define T_()  cout << "\t" << flush;          // Print tab w/o newline

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

typedef bael_TestObserver Obj;

//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

//=============================================================================
//              GENERATOR FUNCTIONS 'g' AND 'gg' FOR TESTING
//-----------------------------------------------------------------------------

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;
    int veryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;;

    bslma_TestAllocator testAllocator(veryVeryVerbose);

    switch (test) { case 0:  // Zero is always the leading case.
      case 2: {
        // --------------------------------------------------------------------
        // TESTING USAGE EXAMPLE
        //
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

        if (verbose) cout << endl << "Testing Usage Example" << endl
                                  << "=====================" << endl;

        {
            bael_RecordAttributes attributes;
            bdem_List             list;
            bael_Record           record(attributes, list);
            bael_Context          context;

                                              ASSERT(0 == Obj::numInstances());
            bael_TestObserver to1(bsl::cout); ASSERT(1 == to1.id());
                                              ASSERT(1 == Obj::numInstances());
            bael_TestObserver to2(bsl::cout); ASSERT(2 == to2.id());
                                              ASSERT(2 == Obj::numInstances());
            bael_TestObserver to3(bsl::cout); ASSERT(3 == to3.id());
                                              ASSERT(3 == Obj::numInstances());

                                        ASSERT(0 == to1.numPublishedRecords());
                                        ASSERT(0 == to2.numPublishedRecords());
                                        ASSERT(0 == to3.numPublishedRecords());

            to1.setVerbose(verbose); // silences 'publish' in production
            to1.publish(record, context);
                                        ASSERT(1 == to1.numPublishedRecords());
            to2.publish(record, context);
                                        ASSERT(1 == to2.numPublishedRecords());
                                        ASSERT(0 == to3.numPublishedRecords());
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //
        // Concerns: BUG
        //   We must ensure that (1) a subclass of the 'bael_Observer' class
        //   compiles and links when all virtual functions are defined, and
        //   (2) the functions are in fact virtual.
        //
        // Plan: BUG
        //   Construct an object of a class derived from 'bael_Observer' and
        //   bind a 'bael_Observer' reference to the object.  Using the base
        //   class reference, invoke the 'publish' method and destructor.
        //   Verify that the correct implementations of the methods are called.
        //
        // Testing:
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "BREATHING TEST" << endl
                                  << "==============" << endl;

        {
            bael_RecordAttributes attributes;
            bdem_List             list;
            bael_Record           record(attributes, list);
            bael_Context          context;

            ASSERT(0 == Obj::numInstances());

            if (verbose)
                cout << "\nInstantiate a test observer 'mX1':" << endl;

            Obj mX1(bsl::cout);  const Obj& X1 = mX1;
            ASSERT(1 == Obj::numInstances());
            ASSERT(1 == X1.id());
            ASSERT(0 == X1.numPublishedRecords());
            if (verbose) { T_(); P_(X1.id());  P(X1.numPublishedRecords()); }

            if (verbose)
                cout << "\nInstantiate a test observer 'mX2':" << endl;

            Obj mX2(bsl::cout);  const Obj& X2 = mX2;
            ASSERT(2 == Obj::numInstances());
            ASSERT(2 == X2.id());
            ASSERT(0 == X2.numPublishedRecords());
            if (verbose) { T_(); P_(X2.id());  P(X2.numPublishedRecords()); }

            if (verbose) cout << "\nPublish a record on 'mX1':" << endl;

            mX1.setVerbose(veryVerbose);
            mX1.publish(record, context);
            ASSERT(1 == X1.numPublishedRecords());
            ASSERT(0 == X2.numPublishedRecords());
            ASSERT(X1.lastPublishedRecord() == record);
            ASSERT(X1.lastPublishedContext() == context);
            if (verbose) { T_(); P_(X1.id());  P(X1.numPublishedRecords()); }

            if (verbose) cout << "\nPublish the record again:" << endl;

            mX1.setVerbose(veryVerbose);
            mX1.publish(record, context);
            ASSERT(2 == X1.numPublishedRecords());
            ASSERT(0 == X2.numPublishedRecords());
            ASSERT(X1.lastPublishedRecord() == record);
            ASSERT(X1.lastPublishedContext() == context);
            if (verbose) { T_(); P_(X1.id());  P(X1.numPublishedRecords()); }

            if (verbose) cout << "\nPublish a record on 'mX2':" << endl;

            mX2.setVerbose(veryVerbose);
            mX2.publish(record, context);
            ASSERT(2 == X1.numPublishedRecords());
            ASSERT(1 == X2.numPublishedRecords());
            ASSERT(X2.lastPublishedRecord() == record);
            ASSERT(X2.lastPublishedContext() == context);
            if (verbose) { T_(); P_(X2.id());  P(X2.numPublishedRecords()); }

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

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2004
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
