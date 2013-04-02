// btemt_udpasyncchannel.t.cpp     -*-C++-*-
#include <btemt_udpasyncchannel.h>

#include <btemt_blobutil.h>

#include <bslma_testallocator.h>
#include <bslma_defaultallocatorguard.h> // for testing only
#include <bslma_default.h>               // for testing only
#include <bsls_assert.h>

#include <bsl_iostream.h>

#include <bteso_ipv4address.h>
#include <bteso_socketimputil.h>
#include <bcema_blob.h>
#include <bcema_blobutil.h>
#include <bcema_pooledblobbufferfactory.h>

using namespace BloombergLP;
using namespace bsl;
using namespace BloombergLP::btemt;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
//
// ...
//
//-----------------------------------------------------------------------------
// [1] constructors
// [2] assign and reset
// [3] operators for equality and inequality
// [4] operators less/greater than
// [5] comparators
// [6] output (printing)
// [7] conversion
// [8] ....
//-----------------------------------------------------------------------------
// [0] BREATHING TEST
//=============================================================================
//                  STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
static int testStatus = 0;

static void aSsErT(int c, const char *s, int i) {
    if (c) {
        bsl::cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << bsl::endl;
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}

#define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }


//=============================================================================
//                    STANDARD BDE LOOP-ASSERT TEST MACROS
//-----------------------------------------------------------------------------

#define LOOP_ASSERT(I,X) { \
    if (!(X)) { bsl::cout << #I << ": " << I << "\n"; aSsErT(1, #X, __LINE__);}}

#define LOOP2_ASSERT(I,J,X) { \
    if (!(X)) { bsl::cout << #I << ": " << I << "\t" << #J << ": " \
                          << J << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP3_ASSERT(I,J,K,X) { \
    if (!(X)) { bsl::cout << #I << ": " << I << "\t" << #J << ": " \
          << J << "\t" << #K << ": " << K << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP4_ASSERT(I,J,K,L,X) { \
    if (!(X)) { bsl::cout << #I << ": " << I << "\t" << #J << ": " << J \
                          << "\t" << #K << ": " << K << "\t" << #L << ": " << L \
                          << "\n"; aSsErT(1, #X, __LINE__); } }


//=============================================================================
//                    SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------

#define P(X) cout << #X " = " << (X) << endl;           // Print identifier and value
#define Q(X) cout << "<! " #X " |>" << endl;            // Quote identifier literally
#define P_(X) cout << #X " = " << (X) << ", " << flush; // P(X) wihtout '\n'
#define L_ __LINE__
#define T_ cout << "\t" << flush;                       // Print tab w/o newline

//=============================================================================
//               GLOBAL HELPER CLASSES AND FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

void assertHandler(const char *text, const char *file, int line)
{
    LOOP3_ASSERT(text, file, line, false);
}

bool SPIN = true;

void echoCallback(int result, int *numNeeded, bcema_Blob *blob, int channelId)
{
    *numNeeded = 0;
    SPIN = false;

    if (result == btemt_AsyncChannel::BTEMT_CANCELED) {
        bsl::cerr << "WARNING: 'echoCallback' was canceled" << bsl::endl;
        return;
    } else if (result == btemt_AsyncChannel::BTEMT_CLOSED) {
        bsl::cerr << "WARNING: 'echoCallback' was closed" << bsl::endl;
        return;
    } else if (result == btemt_AsyncChannel::BTEMT_FAILURE) {
        bsl::cerr << "WARNING: 'echoCallback' has failed" << bsl::endl;
        return;
    } else if (result == btemt_AsyncChannel::BTEMT_TIMEOUT) {
        bsl::cerr << "WARNING: 'echoCallback' has timed out" << bsl::endl;
        return;
    } 
    ASSERT(result == btemt_AsyncChannel::BTEMT_SUCCESS);

    char buffer[1024];
    btemt::BlobUtil::copyOut(buffer, *blob, blob->length());

    bsl::cout << "echoed back: " << buffer << bsl::endl;
}

//=============================================================================
//                                 MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[]) {
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;
    int veryVeryVerbose = argc > 4;

    bslma_TestAllocator ta(veryVeryVerbose);
    bslma_Default::setDefaultAllocatorRaw(&ta);
    bsls_Assert::setFailureHandler(assertHandler);

    bteso_SocketImpUtil::startup();

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:  // Zero is always the leading case.
      case 5: {
        // --------------------------------------------------------------------
        // TESTING '<<TODO>>'
        //
        // Concerns:
        //
        //
        // Plan:
        //
        //
        // Testing:
        //
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "TESTING '<<TODO>>'" << endl
                                  << "==================" << endl;

        // . . .

      } break;
      case 4: {
        // --------------------------------------------------------------------
        // PRIMARY ACCESSORS
        //
        // Concerns:
        //
        //
        // Plan:
        //
        //
        // Testing:
        //
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "PRIMARY ACCESSORS" << endl
                                  << "=================" << endl;

        // . . .

      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING APPARATUS
        //
        // Concerns:
        //
        //
        // Plan:
        //
        //
        // Testing:
        //
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "TESTING APPARATUS" << endl
                                  << "=================" << endl;

        // . . .

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // PRIMARY MANIPULATORS
        //
        // Concerns:
        //
        //
        // Plan:
        //
        //
        // Testing:
        //
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "PRIMARY MANIPULATORS" << endl
                                  << "====================" << endl;

        // . . .

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //
        // Concerns:
        //   This test exercises basic functionality but tests nothing.
        //
        // Plan:
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "BREATHING TEST" << endl
                                  << "==============" << endl;
        bcema_PooledBlobBufferFactory blobBufferFactory(1024);
        bteso_IPv4Address peerAddress("127.0.0.1", 48734);
        char DATA[] = "This is a test!";
        int DATA_SIZE = sizeof(DATA);

	btemt_TcpTimerEventManager eventManager;
	int rv = eventManager.enable();
        ASSERT(0 == rv);
        UdpAsyncChannel channel(&eventManager,
                                peerAddress, 
                                &blobBufferFactory, 
                                &ta);

        btemt_AsyncChannel::BlobBasedReadCallback callback 
            = &echoCallback;
        channel.timedRead(0, bdet_TimeInterval(1), callback);

        bcema_Blob blob(&blobBufferFactory);
        bcema_BlobUtil::append(&blob, DATA, DATA_SIZE);
        channel.write(blob);

        while (SPIN) {};
      } break;
      default: {
        cerr << "WARNING: CASE '" << test << "' NOT FOUND." << endl;
        return -1;
      }
    }

    if (testStatus > 0) {
        cerr << "Error, non-zero test status = " << testStatus << "." << endl;
    }
    bteso_SocketImpUtil::cleanup();
    return testStatus;
}

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2011
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
