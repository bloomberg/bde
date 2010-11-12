// bdem_enumerationdef.t.cpp                  -*-C++-*-

#include <bdem_enumerationdef.h>

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
  
class bdem_Schema {
        // Dummy type.
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


///Directly Creating a 'bdem_RecordDef' (*Not Recommended*)
///- - - - - - - - - - - - - - - - - - - - - - - - - - - -
// The following section demontrates how to create a 'bdem_EnumerationDef' in
// isolation -- this is *not* the recommended usage of 'bdem_EnumerationDef'.
// A 'bdem_RecordDef' is meant to be used in conjuction with the types defined
// in 'bdem_schema'.  Clients should not create a 'bdem_EnumerationDef'
// directly, but obtain one from a 'bdem_Schema'.  For the purpose of this
// usage example, we define a dummy stand-in type for 'bdem_Schema'.  This
// type is used in name-only by 'bdem_EnumerationDef', so its definition is
// not necessary: 
//..
//  namespace BloombergLP {
//  
//  class bdem_Schema {
//      // Dummy type.
//  };
//  
//  }
//..
// Now we create an enumeration definition with an index of 0, and a name of
// "COLOR", that will contain an enumeration of color identifiers (e.g., red,
// green, blue):
//..
    bslma_Allocator           *allocator = bslma_Default::allocator(); 
    bdema_SequentialAllocator  seqAllocator(allocator);
    bdem_Schema                dummySchema;
//
    bdem_EnumerationDef colorEnumDef(&dummySchema, 0, "COLOR", &seqAllocator);
//.. 
//
///Manipulating and Accessing a 'bdem_EnumerationDef'
///- - - - - - - - - - - - - - - - - - - - - - - - -
// The following section demonstrates how to add enumerated values to an
// enumeration definition, and then access those values.  We will use the
// enumeration definition, 'colorEnumDef' created in the preceeding section.
// In practice clients should obtain a enumeration definition from a schema
// (see 'bdem_schema').
//
// We start by examining the properties of an empty enumeration definition.
// Note that 'enumerationName' and 'enumerationIndex' are supplied at
// construction:
//..
    ASSERT(0 == colorEnumDef.enumerationIndex());
    ASSERT(0 == bsl::strcmp("COLOR", colorEnumDef.enumerationName()));
    ASSERT(0 == colorEnumDef.numEnumerators());
    ASSERT(0 == colorEnumDef.nextLargerName(0).first);
    ASSERT(0 == colorEnumDef.nextLargerId(0).first);
    ASSERT(&dummySchema == &colorEnumDef.schema());
//..
// Next we add 3 different values to the enumeration.  Since we do not
// explicitly specify an ID for the color values, one is assigned
// automatically.  By default, the first added enumerator is given the ID 0,
// subsequent values (if no ID is explicitly provided) are given the current
// maximum ID + 1:
//..
    int redId   = colorEnumDef.addEnumerator("RED");
    int greenId = colorEnumDef.addEnumerator("GREEN");
    int blueId  = colorEnumDef.addEnumerator("BLUE");
//..
// We verify that the IDs of the new values:
//..    
    ASSERT(3 == colorEnumDef.numEnumerators());
    ASSERT(2 == colorEnumDef.maxId());

    ASSERT(0 == redId);
    ASSERT(1 == greenId);
    ASSERT(2 == blueId);
//..
// Next we add a enumerator, and explicitly provide an ID (in this case -5):
//..
    int yellowId = colorEnumDef.addEnumerator("YELLOW", -5);

    ASSERT(4 == colorEnumDef.numEnumerators());
    ASSERT(2 == colorEnumDef.maxId());

    ASSERT(-5 == yellowId);
//..
// We can perform lookups, either by name, or by ID:
//..
    ASSERT(2 == colorEnumDef.lookupId("BLUE"));
    ASSERT(0 == bsl::strcmp("BLUE", colorEnumDef.lookupName(2)));
//..
// We can use a 'bdem_EnumerationDefIterator' to efficiently iterate over the
// values in the enumeration definition:
//..
    bdem_EnumerationDefIterator enumIt = colorEnumDef.begin();
    for ( ; enumIt != colorEnumDef.end(); ++enumIt) {
        bsl::cout << "[ " << enumIt.name() << " " << enumIt.id() << " ]"
                  << bsl::endl;
    }
//..
// The console output of the preceeding loop will be:
//..
//  [ YELLOW -5 ]
//  [ RED 0 ]
//  [ GREEN 1 ]
//  [ BLUE 2 ]
//..
// Finally we can use the accessors 'nextLargerId' and 'nextLargerName' to
// find the next larger ID after the ID supplied, or next larger name after
// the name supplied, respectively.  Note that this can be used to iterate
// over the elements of an enumeration definition, either in ID or name
// order.  Each call to 'nextLargerId' and 'nextLargerName' has 'O(log(N))'
// complexity (where 'N' is the number of enumerators). 
//
// The next larger ID in 'colorEnumDef' after 0 is 1 ("GREEN"):
//..
    bsl::pair<const char *, int> enumerator = colorEnumDef.nextLargerId(0);
    ASSERT(0 == bsl::strcmp("GREEN", enumerator.first));
    ASSERT(1 == enumerator.second);
//..
// The next larger name in 'colorEnumDef' after "RED" is "YELLOW" (-5):
//..
    enumerator = colorEnumDef.nextLargerName("RED");
    ASSERT(0  == bsl::strcmp("YELLOW", enumerator.first));
    ASSERT(-5 == enumerator.second);
//..
    
           
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
