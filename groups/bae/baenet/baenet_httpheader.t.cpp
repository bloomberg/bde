// baenet_httpheader.t.cpp                                            -*-C++-*-
#include <baenet_httpheader.h>

#include <bdeut_stringref.h>

#include <bsls_protocoltest.h>

#include <bsl_cstdlib.h>     // 'atoi'
#include <bsl_cstring.h>     // 'strlen'
#include <bsl_iostream.h>
#include <bsl_sstream.h>

using namespace BloombergLP;

//=============================================================================
//                                 TEST PLAN
//-----------------------------------------------------------------------------
//                                 Overview
//                                 --------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

//=============================================================================
//                      STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
static int testStatus = 0;

static void aSsErT(int c, const char *s, int i)
{
    if (c) {
        bsl::cout << "Error " << __FILE__ << "(" << i << "): " << s
                  << "    (failed)" << bsl::endl;
        if (0 <= testStatus && testStatus <= 100) ++testStatus;
    }
}

#define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

//=============================================================================
//                  STANDARD BDE LOOP-ASSERT TEST MACROS
//-----------------------------------------------------------------------------
#define LOOP_ASSERT(I,X) { \
    if (!(X)) { bsl::cout << #I << ": " << I << "\n"; \
                aSsErT(1, #X, __LINE__); }}

#define LOOP2_ASSERT(I,J,X) { \
    if (!(X)) { bsl::cout << #I << ": " << I << "\t"  \
                          << #J << ": " << J << "\n"; \
                aSsErT(1, #X, __LINE__); } }

#define LOOP3_ASSERT(I,J,K,X) { \
   if (!(X)) { bsl::cout << #I << ": " << I << "\t" \
                         << #J << ": " << J << "\t" \
                         << #K << ": " << K << "\n";\
               aSsErT(1, #X, __LINE__); } }

//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define P(X) bsl::cout << #X " = " << (X) << bsl::endl;
                                              // Print identifier and value.
#define Q(X) bsl::cout << "<| " #X " |>" << bsl::endl;
                                              // Quote identifier literally.
#define P_(X) bsl::cout << #X " = " << (X) << ", " << bsl::flush;
                                              // P(X) without '\n'
#define L_ __LINE__                           // current Line number
#define NL "\n"

// ============================================================================
//                               TEST APPARATUS
// ----------------------------------------------------------------------------

namespace BloombergLP {

class baenet_HttpBasicHeaderFields {
    // This 'class' is a trivial implementation of the
    // 'baenet_HttpBasicHeaderFields' type that is used (*in* *name* *only*) in
    // the protocol under test.

  public:
    // CREATORS
    baenet_HttpBasicHeaderFields();
    baenet_HttpBasicHeaderFields(const baenet_HttpBasicHeaderFields& original);
    ~baenet_HttpBasicHeaderFields();

    // MANIPULATORS
    baenet_HttpBasicHeaderFields& operator=(
                                      const baenet_HttpBasicHeaderFields& rhs);
};

// CREATORS
baenet_HttpBasicHeaderFields::baenet_HttpBasicHeaderFields()
{
}

baenet_HttpBasicHeaderFields::baenet_HttpBasicHeaderFields(
                                           const baenet_HttpBasicHeaderFields&)
{
}

baenet_HttpBasicHeaderFields::~baenet_HttpBasicHeaderFields()
{
}

// MANIPULATORS
baenet_HttpBasicHeaderFields&
baenet_HttpBasicHeaderFields::operator=(const baenet_HttpBasicHeaderFields&)
{
    return *this;
}

}  // close namespace BloombergLP

//=============================================================================
//               GLOBAL TYPEDEFS/CLASSES/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

struct HttpHeaderTest : bsls_ProtocolTestImp<baenet_HttpHeader> {
    int addField(const bdeut_StringRef&, const bdeut_StringRef&)
                                                      { return markDone(); }
    const baenet_HttpBasicHeaderFields& basicFields() const
                                                      { return markDoneRef(); }
};

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? bsl::atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;
    int veryVeryVerbose = argc > 4;

    bsl::cout << "TEST " << __FILE__ << " CASE " << test << bsl::endl;;

    switch (test) { case 0:  // Zero is always the leading case.
      case 2: {
        // --------------------------------------------------------------------
        // PROTOCOL TEST
        // --------------------------------------------------------------------

        bsls_ProtocolTest<HttpHeaderTest> t(veryVerbose);

        ASSERT(t.testAbstract());
        ASSERT(t.testNoDataMembers());
        ASSERT(t.testVirtualDestructor());

        BSLS_PROTOCOLTEST_ASSERT(t,
                               addField(bdeut_StringRef(), bdeut_StringRef()));
        BSLS_PROTOCOLTEST_ASSERT(t, basicFields());
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << bsl::endl
                               << "BREATHING TEST" << bsl::endl
                               << "==============" << bsl::endl;

      } break;
      default: {
        bsl::cerr << "WARNING: CASE `" << test << "' NOT FOUND." << bsl::endl;
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        bsl::cerr << "Error, non-zero test status = " << testStatus << "."
                  << bsl::endl;
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
