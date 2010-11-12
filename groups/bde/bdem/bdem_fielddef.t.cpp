// bdem_fielddef.t.cpp                  -*-C++-*-

#include <bdem_fielddef.h>

#include <bslma_allocator.h>
#include <bslma_default.h>

#include <bdema_sequentialallocator.h>

#include <bsl_cstdlib.h>
#include <bsl_iostream.h>

using namespace BloombergLP;
using namespace bsl;  // automatically added by script


//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//
//
//-----------------------------------------------------------------------------

//=============================================================================
//                      STANDARD BDE ASSERT TEST MACRO
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
#define T_ cout << "\t" << flush;             // Print tab w/o newline

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

//=============================================================================
//                           CLASSES FOR TESTING
//-----------------------------------------------------------------------------

//=============================================================================
//                               USAGE EXAMPLE
//-----------------------------------------------------------------------------

namespace BloombergLP {
    class bdem_RecordDef {
    };
    class bdem_Schema {
    };
}
//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;
    int veryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

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
        //   comment characters, and replace 'assert' with 'ASSERT'.  Suppress
        //   all 'bsl::cout' statements in non-verbose mode, and add streaming
        //   to a buffer to test programmatically the printing examples.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nTesting Usage Examples"
                               << "\n======================" << bsl::endl;

///Usage 
///-----
// The following usage examples demonstrate how to construct, manipulate, and
// access a field definition in isolation.  In general, clients should *not*
// create a 'bdem_FieldDef' directly, but obtain one from a record definition
// (see 'bdem_recorddef'). 
//
///Directly Creating a 'bdem_FieldDef' (*Not Recommended*)
///- - - - - - - - - - - - - - - - - - - - - - - - - - - -
// The following example demontrates how to create a 'bdem_FieldDef' in
// isolation -- this is *not* the recommended usage of 'bdem_FieldDef'.  A
// 'bdem_FieldDef' is meant to be used in conjuction with the types defined in
// 'bdem_recorddef', 'bdem_enumerationdef', and 'bdem_schema'.  Clients should
// not create a 'bdem_FieldDef' directly, but obtain one from a
// 'bdem_RecordDef'.  For the purpose of this usage example, we define dummy
// stand-in types for, 'bdem_RecordDef, and 'bdem_EnumerationDef'.  These
// types are used in name-only, so their definition is not important: 
//..

//..
// Now we create a couple field definition objects:
//..
    bslma_Allocator           *allocator = bslma_Default::allocator();
    bdema_SequentialAllocator  seqAllocator;
//
    bdem_FieldDefAttributes  intAttr(bdem_ElemType::BDEM_INT,  allocator);
    bdem_FieldDefAttributes listAttr(bdem_ElemType::BDEM_LIST, allocator);
//
    bdem_FieldDef  intField("intField",  0,  intAttr, allocator);
    bdem_FieldDef listField("listField", 1, listAttr, allocator);
//..
// We now provide constraints for these fields using objects of the dummy
// 'bdem_RecordDef', and 'bdem_EnumerationDef' types defined above.  Clients
// of 'bdem' should refer to the documentation of 'bdem_schema' and
// 'bdem_recorddef' for more information on field constraints.
//..
    bdem_Schema         schema;
    bdem_EnumerationDef enumerationDef(&schema, 0, "enum", &seqAllocator);
    bdem_RecordDef      recordDef;
//
    intField.setConstraint(&enumerationDef);
    listField.setConstraint(&recordDef);
//..
//
///Accessing a 'bdem_FieldDef'
///- - - - - - - - - - - - - -
// In this example, we demonstrate how to access the properties of a
// 'bdem_FieldDef' object.  The two objects, 'intField' and 'listField' were
// defined in the preceeding example:
//..
    ASSERT(0 == bsl::strcmp("intField",  intField.fieldName()));
    ASSERT(0 == bsl::strcmp("listField", listField.fieldName()));
//
    ASSERT(0 == intField.fieldId());
    ASSERT(1 == listField.fieldId());
//
// An enumeration definition constraint can be provided for fields of type
// 'BDEM_INT', 'BDEM_STRING', 'BDEM_INT_ARRAY', and 'BDEM_STRING_ARRAY':
//..
    ASSERT(&enumerationDef == intField.enumerationConstraint());
    ASSERT(0               == listField.enumerationConstraint());
//..
// A record definition constraint be provided for fields of an aggregate type 
// ('BDEM_LIST', 'BDEM_CHOICE', 'BDEM_TABLE', 'BDEM_CHOICE_ARRAY'):
//..
    ASSERT(0          == intField.recordConstraint());
    ASSERT(&recordDef == listField.recordConstraint());

    } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING/USAGE TEST
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        //
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BREATHING TEST" << endl
                          << "==============" << endl;


        if (verbose) cout << "\tChoice and Sequence traits." << endl;

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
