// btemt_channelerror.t.cpp                                           -*-C++-*-

#include <btemt_channelerror.h>

#include <bcema_blob.h>
#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>

#include <bsls_assert.h>
#include <bsls_asserttest.h>

#include <bsl_iostream.h>
#include <bsl_sstream.h>

using namespace BloombergLP;
using namespace bsl;
using namespace BloombergLP::btemt;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// The component under test implements a set of enumerations listing status
// codes returned from certain channel operations.  The enumerators in each
// enumeration may not be a contiguously growing sequence starting from 0,
// because the status codes represented by the enumerators maybe written before
// its definition.
//-----------------------------------------------------------------------------
// TYPES
// [1] btemt::ChannelErrorRead::Enum
// [2] btemt::ChannelErrorWrite::Enum

// ============================================================================
//                    STANDARD BDE ASSERT TEST MACROS
// ----------------------------------------------------------------------------

static int testStatus = 0;

static void aSsErT(int c, const char *s, int i)
{
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}
# define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

// ============================================================================
//                  STANDARD BDE LOOP-ASSERT TEST MACROS
// ----------------------------------------------------------------------------

#define LOOP_ASSERT(I,X) {                                                    \
    if (!(X)) { cout << #I << ": " << I << "\n"; aSsErT(1, #X, __LINE__);}}

#define LOOP2_ASSERT(I,J,X) {                                                 \
    if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": "                 \
              << J << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP3_ASSERT(I,J,K,X) {                                               \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t"     \
              << #K << ": " << K << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP4_ASSERT(I,J,K,L,X) {                                             \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" <<  \
       #K << ": " << K << "\t" << #L << ": " << L << "\n";                    \
       aSsErT(1, #X, __LINE__); } }

#define LOOP5_ASSERT(I,J,K,L,M,X) {                                           \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" <<  \
       #K << ": " << K << "\t" << #L << ": " << L << "\t" <<                  \
       #M << ": " << M << "\n";                                               \
       aSsErT(1, #X, __LINE__); } }

#define LOOP0_ASSERT ASSERT
#define LOOP1_ASSERT LOOP_ASSERT

//=============================================================================
//                  STANDARD BDE VARIADIC ASSERT TEST MACROS
//-----------------------------------------------------------------------------

#define NUM_ARGS_IMPL(X5, X4, X3, X2, X1, X0, N, ...)   N
#define NUM_ARGS(...) NUM_ARGS_IMPL(__VA_ARGS__, 5, 4, 3, 2, 1, 0, "")

#define LOOPN_ASSERT_IMPL(N, ...) LOOP ## N ## _ASSERT(__VA_ARGS__)
#define LOOPN_ASSERT(N, ...)      LOOPN_ASSERT_IMPL(N, __VA_ARGS__)

#define ASSERTV(...) LOOPN_ASSERT(NUM_ARGS(__VA_ARGS__), __VA_ARGS__)

// ============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
// ----------------------------------------------------------------------------

#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", " << flush; // 'P(X)' without '\n'
#define T_ cout << "\t" << flush;             // Print tab w/o newline.
#define L_ __LINE__                           // current Line number

// ============================================================================
//                  NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT_SAFE_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(EXPR)
#define ASSERT_SAFE_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPR)
#define ASSERT_PASS(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS(EXPR)
#define ASSERT_FAIL(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL(EXPR)
#define ASSERT_OPT_PASS(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS(EXPR)
#define ASSERT_OPT_FAIL(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL(EXPR)

// ============================================================================
//                     GLOBAL TYPEDEFS FOR TESTING
// ----------------------------------------------------------------------------

namespace {

class MyChannel {
    // This class represents a channel over which data may be sent and
    // received.

  public:
    //...
    int write(const bcema_Blob& blob);
        // Enqueue the specified 'blob' message to be written to this
        // channel.  Return 0 on success, and non-zero value in the
        // enumeration 'btemt::ChannelErrorWrite::Enum' otherwise.

    //...
};

int MyChannel::write(const bcema_Blob& blob)
{
    return btemt::ChannelErrorWrite::BTEMT_SUCCESS;
}

}  // close unnamed namespace

// ============================================================================
//                            MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int                 test = argc > 1 ? atoi(argv[1]) : 0;
    bool             verbose = argc > 2;
    bool         veryVerbose = argc > 3;
    bool     veryVeryVerbose = argc > 4;
    bool veryVeryVeryVerbose = argc > 5;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:
      case 3: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
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

        if (verbose) cout << endl
                          << "USAGE EXAMPLE" << endl
                          << "=============" << endl;

///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Basic Syntax
///- - - - - - - - - - - -
// Suppose that we have a class 'MyChannel' that defines the following elided
// interface:
//..

//..
// Further suppose that we have a 'bcema_Blob' object, 'blob', and a
// 'MyChannel' object, 'channel'.
        MyChannel channel;
        bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);
        bcema_Blob blob(&scratch);
//
// First, we use the 'write' method to write 'blob' into 'channel':
//..
        int rc = channel.write(blob);
//..
// Now, we compare the return code the enumerators in
// 'btemt::ChannelErrorRead::Enum' to handle the different error conditions:
//..
        switch(rc) {
          case btemt::ChannelErrorWrite::BTEMT_SUCCESS:  // Success
          {
              // ...
          } break;

          // We handle the different types of failures in the cases below.

          case btemt::ChannelErrorWrite::BTEMT_CACHE_HIWAT:
          {
              // ...
          } break;
          case btemt::ChannelErrorWrite::BTEMT_HIT_CACHE_HIWAT:
          {
              // ...
          } break;
          case btemt::ChannelErrorWrite::BTEMT_CHANNEL_DOWN:
          {
              // ...
          } break;
          default:  // Handle other failures.
          {
              //...
          }
        };
//..
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // 'ChannelErrorWrite::Enum'
        //
        // Concerns:
        //: 1 The enumerators in the 'enum' has the correct values.
        //
        // Testing:
        //   btemt::ChannelErrorWrite::Enum
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "'ChannelErrorWrite::Enum'" << endl
                          << "=========================" << endl;


        ASSERTV(ChannelErrorWrite::BTEMT_SUCCESS,
                 0 == ChannelErrorWrite::BTEMT_SUCCESS);
        ASSERTV(ChannelErrorWrite::BTEMT_CACHE_HIWAT,
                -1 == ChannelErrorWrite::BTEMT_CACHE_HIWAT);
        ASSERTV(ChannelErrorWrite::BTEMT_HIT_CACHE_HIWAT,
                -2 == ChannelErrorWrite::BTEMT_HIT_CACHE_HIWAT);
        ASSERTV(ChannelErrorWrite::BTEMT_CHANNEL_DOWN,
                -3 == ChannelErrorWrite::BTEMT_CHANNEL_DOWN);
        ASSERTV(ChannelErrorWrite::BTEMT_ENQUEUE_WAT,
                -4 == ChannelErrorWrite::BTEMT_ENQUEUE_WAT);
        ASSERTV(ChannelErrorWrite::BTEMT_UNKNOWN,
                -5 == ChannelErrorWrite::BTEMT_UNKNOWN);

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // 'ChannelErrorRead::Enum'
        //
        // Concerns:
        //: 1 The enumerators in the 'enum' has the correct values.
        //
        // Testing:
        //   btemt::ChannelErrorRead::Enum
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "'ChannelErrorRead::Enum'" << endl
                          << "========================" << endl;


        ASSERTV(ChannelErrorRead::BTEMT_SUCCESS,
                 0 == ChannelErrorRead::BTEMT_SUCCESS);
        ASSERTV(ChannelErrorRead::BTEMT_CHANNEL_CLOSED,
                -2 == ChannelErrorRead::BTEMT_CHANNEL_CLOSED);

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
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2011
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ----------------------------------
