// btemt_socks5negotiator.t.cpp     -*-C++-*-
#include <btemt_socks5negotiator.h>

#include <btemt_socksconfiguration.h>

#include <bdex_bytestreamimputil.h>

#include <bslma_testallocator.h>
#include <bslma_defaultallocatorguard.h> // for testing only
#include <bslma_default.h>               // for testing only
#include <bsls_assert.h>
#include <bsls_platform.h>

#include <bdeut_stringref.h>
#include <bteso_ipv4address.h>

#include <bsl_iostream.h>
#include <bsl_string.h>

#include <cstring>

#ifdef BSLS_PLATFORM_OS_WINDOWS
#include <Winsock2.h>
#else
#include <netinet/in.h>
#endif

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

#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value
#define Q(X) cout << "<! " #X " |>" << endl;  // Quote identifier literally
#define P_(X) cout << #X " = " << (X) << ", " << flush; // P(X) wihtout '\n'
#define L_ __LINE__
#define T_ cout << "\t" << flush;           // Print tab w/o newline

//=============================================================================
//               GLOBAL HELPER CLASSES AND FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

void assertHandler(const char *text, const char *file, int line)
{
	LOOP3_ASSERT(text, file, line, false);
}

void cbSuccess(int status, Socks5Negotiator *negotiator)
{
    ASSERT(status == Socks5Negotiator::SUCCESS);
}


struct MethodRequestPkt {
    char d_ver;
    char d_nmethods;
    char d_methods[2];
};

struct MethodResponsePkt {
    char d_ver;
    char d_method;
};

struct ConnectBase {
    char d_ver;
    char d_cmd;
    char d_rsv;
    char d_atype;
};

struct ConnectIPv4AddressRequest {
    ConnectBase                d_connectBase;
    char                       d_address[4];
    char                       d_port[2];
};

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

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:  // Zero is always the leading case.
      case 5: {
        // --------------------------------------------------------------------
        // <<TODO>>
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

        if (verbose) cout << endl << "<<TODO>>" << endl
                                  << "========" << endl;
        
        // . . . 
        
      } break;
      case 4: {
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
   
        enum { BUFFER_SIZE = 1024 };
        char buffer[BUFFER_SIZE];
// TODO: need to create a channel pair for testing
/*
        TestChannelUtil::TestChannelPtr clientChannel = 0;
        TestChannelUtil::TestChannelPtr serverChannel = 0;
        TestChannelUtil::allocateChannelPair(&clientChannel, 
                                             &serverChannel,
                                             &ta);
        SocksConfiguration config;
        config.destinationIpAddr()  = "192.168.1.1";
        config.destinationPort()    = 10075;
        bcema_PooledBlobBufferFactory bbf(BUFFER_SIZE);
        Socks5Negotiator s5n(clientChannel.ptr(), config, &cbSuccess, &bbf,
                             Socks5Negotiator::TCP_CONNECTION, &ta);
        s5n.start();

        // verify greeting request
        MethodRequestPkt mreqPkt;
        mreqPkt.d_ver = 0x05;
        mreqPkt.d_nmethods = 0x02;
        mreqPkt.d_methods[0] = 0x00;
        mreqPkt.d_methods[1] = 0x02;

        int count = serverChannel->read(buffer, BUFFER_SIZE);
        ASSERT(count == sizeof(mreqPkt));
        ASSERT(!bsl::memcmp(buffer, &mreqPkt, bsl::min<int>(sizeof(mreqPkt), count)));

        // send greeting response
        MethodResponsePkt mrspPkt;
        mrspPkt.d_ver    = 0x05;
        mrspPkt.d_method = 0x00;
        count =  serverChannel->write((char*)&mrspPkt, sizeof(mrspPkt));
        BSLS_ASSERT(count == sizeof(mrspPkt));

        // verify connection request
        bteso_IPv4Address dstAddr(config.destinationIpAddr().c_str(), 
                                  config.destinationPort());
        ConnectIPv4AddressRequest cipv4reqPkt;
        cipv4reqPkt.d_connectBase.d_ver   = 0x05;
        cipv4reqPkt.d_connectBase.d_cmd   = 0x01;
        cipv4reqPkt.d_connectBase.d_rsv   = 0x00;
        cipv4reqPkt.d_connectBase.d_atype = 0x01;

        int address = ntohl(dstAddr.ipAddr())); // convert to host order
        bdex_ByteStreamImpUtil::putInt32(cipv4reqPkt.d_address, address);
                                         dstAddr.portNumber());

        count = serverChannel->read(buffer, BUFFER_SIZE);
        ASSERT(count == sizeof(cipv4reqPkt));
        ASSERT(!bsl::memcmp(buffer, &cipv4reqPkt, bsl::min<int>(sizeof(cipv4reqPkt), count)));
        cipv4reqPkt.d_connectBase.d_cmd = 0x00;
        count = serverChannel->write((char*)&cipv4reqPkt, count);

*/
      } break;
      default: {
        cerr << "WARNING: CASE '" << test << "' NOT FOUND." << endl;
        return -1;
      }
    }

    if (testStatus > 0) {
        cerr << "Error, non-zero test status = " << testStatus << "." << endl;
    }
    return testStatus;
}

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2010
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
