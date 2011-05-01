// baenet_httpentityprocessor.t.cpp                                   -*-C++-*-
#include <baenet_httpentityprocessor.h>

#include <bcema_blob.h>

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

class baenet_HttpStartLine {
    // This 'class' is a trivial implementation of the 'baenet_HttpStartLine'
    // type that is used (*in* *name* *only*) in the protocol under test.

  public:
    // CREATORS
    baenet_HttpStartLine();
    baenet_HttpStartLine(const baenet_HttpStartLine& original);
    ~baenet_HttpStartLine();

    // MANIPULATORS
    baenet_HttpStartLine& operator=(const baenet_HttpStartLine& rhs);
};

// CREATORS
baenet_HttpStartLine::baenet_HttpStartLine()
{
}

baenet_HttpStartLine::baenet_HttpStartLine(const baenet_HttpStartLine&)
{
}

baenet_HttpStartLine::~baenet_HttpStartLine()
{
}

// MANIPULATORS
baenet_HttpStartLine&
baenet_HttpStartLine::operator=(const baenet_HttpStartLine&)
{
    return *this;
}

}  // close namespace BloombergLP

//=============================================================================
//               GLOBAL TYPEDEFS/CLASSES/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

struct HttpEntityProcessorTest : bsls_ProtocolTest<baenet_HttpEntityProcessor>
{
    void onStartEntity(const baenet_HttpStartLine&,
                       const bcema_SharedPtr<baenet_HttpHeader>&)   { exit(); }
    void onEntityData(const bcema_Blob&)                            { exit(); }
    void onEndEntity()                                              { exit(); }
    bcema_SharedPtr<baenet_HttpHeader> createHeader() const  { return exit(); }
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

        bsls_ProtocolTestDriver<HttpEntityProcessorTest> t;

        ASSERT(t.testAbstract());
        ASSERT(t.testNoDataMembers());
        ASSERT(t.testVirtualDestructor());

        BSLS_PROTOCOLTEST_ASSERT(t,
                onStartEntity(baenet_HttpStartLine(),
                              bcema_SharedPtr<baenet_HttpHeader>()));
        BSLS_PROTOCOLTEST_ASSERT(t, onEntityData(bcema_Blob()));
        BSLS_PROTOCOLTEST_ASSERT(t, onEndEntity());
        BSLS_PROTOCOLTEST_ASSERT(t, createHeader());

        testStatus = t.failures();
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
