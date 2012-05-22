// bteso_socketoptions.t.cpp                                          -*-C++-*-

#include <bdes_ident.h>
BDES_IDENT_RCSID(bteso_socketoptions_t_cpp,"$Id$ $CSID$")

#include <bteso_socketoptions.h>

// #include <bteso_socketoptutil.h>
// #include <bteso_socketimputil.h>

#include <bsl_cstdlib.h>
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
#define T_() bsl::cout << '\t' << bsl::flush; // Print tab w/o newline.

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------


static int verbose = 0;
static int veryVerbose = 0;
static int veryVeryVerbose = 0;
static int veryVeryVeryVerbose = 0;

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? bsl::atoi(argv[1]) : 0;
    verbose = (argc > 2);
    veryVerbose = (argc > 3);
    veryVeryVerbose = (argc > 4);
    veryVeryVeryVerbose = (argc > 5);

    bsl::cout << "TEST " << __FILE__ << " CASE " << test << bsl::endl;;

    switch (test) { case 0:  // Zero is always the leading case.
      case 2: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE TEST
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

        if (verbose) bsl::cout << bsl::endl
                               << "USAGE EXAMPLE TEST" << bsl::endl
                               << "==================" << bsl::endl;

//         bteso_SocketHandle::Handle socketHandle;

//         int rc;
//         bteso_SocketImpUtil::open<bteso_IPv4Address>(
//                                    &socketHandle,
//                                    bteso_SocketImpUtil::BTESO_SOCKET_STREAM,
//                                    &rc);
//         ASSERT(0 == rc);

///Usage
///-----
// The 'bteso_SocketOptions' can be used to specify whether local addresses
// should be reused.  The following snippets of code illustrate how to set
// the 'BTESO_REUSEADDRESS' flag on a socket.  Note that we assume that a
// socket of type 'SOCK_STREAM' has already been created and that it can be
// accessed using 'socketHandle':
//..
//  bool                reuseLocalAddr = true;
//  bteso_SocketOptions options;
//  options.reuseAddress().makeValue(reuseLocalAddr);
//
//  rc = bteso_SocketOptUtil::setSocketOptions(socketHandle, options);
//  ASSERT(0 == rc);
//..
// Now we will verify that the address option was set correctly:
//..
//  int addropt = 0;
//  rc = bteso_SocketOptUtil::getOption(
//                                    &addropt,
//                                    socketHandle,
//                                    bteso_SocketOptUtil::BTESO_SOCKETLEVEL,
//                                    bteso_SocketOptUtil::BTESO_REUSEADDRESS);
//
//  ASSERT(0 == rc);
//  ASSERT(0 != addropt);
//..
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // Basic Attribute Test:
        // --------------------------------------------------------------------

        if (verbose) {
            bsl::cout << "Basic Attribute Test" << bsl::endl
                      << "====================" << bsl::endl;
        }

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

// GENERATED BY BLP_BAS_CODEGEN_3.4.x_DEV Thu Dec  9 09:55:03 2010
// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2010
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ------------------------------ END-OF-FILE ---------------------------------
