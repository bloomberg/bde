// btemt_query.t.cpp                  -*-C++-*-

#include <btemt_query.h>

#include <bslma_testallocator.h>
#include <bdem_list.h>
#include <bdex_instreamfunctions.h>             // for testing only
#include <bdex_outstreamfunctions.h>            // for testing only
#include <bdex_testoutstream.h>
#include <bdex_testinstream.h>
#include <bdema_managedptr.h>
#include <bsls_platformutil.h>

#include <bsl_cstdio.h>
#include <bsl_cstdlib.h>
#include <bsl_cstring.h>
#include <bsl_iostream.h>

using namespace BloombergLP;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
//   This component consists of four packet classes, each of which owns a
// bdema_ManagedPtr that points to a bdem_List.  The methods are in the
// following general form:
// [] Constructor: default constructor         -btemt_Query()
// [] Constructor: 'stealing' constructor      -btemt_Query(btemt_Query& rhs)
// [] Assignment : 'stealing' assignment       -operator=(btemt_Query& rhs)
// [] Manipulator: set bdema_ManagedPtr        -setXxxx(bdema_ManagedPtr& Xxxx)
// [] Manipulator: set other properties        -setProperty(type property)
// [] Manipulator: 'release' bdema_ManagedPtr  -xxxx()
// [] Accessor   : managed list                -xxxx()
// [] Accessor   : other property              -property()
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
   if (!(X)) { bsl::cout << #I << ": " << I << "\t" << #J << ": " \
              << J << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP3_ASSERT(I,J,K,X) { \
   if (!(X)) { \
              bsl::cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" \
              << #K << ": " << K << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP4_ASSERT(I,J,K,L,X) { \
   if (!(X)) { \
       bsl::cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" << \
       #K << ": " << K << "\t" << #L << ": " << L << "\n"; \
       aSsErT(1, #X, __LINE__); } }

#define LOOP5_ASSERT(I,J,K,L,M,X) { \
   if (!(X)) { \
       bsl::cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" << \
       #K << ": " << K << "\t" << #L << ": " << L << "\t" << \
       #M << ": " << M << "\n"; \
       aSsErT(1, #X, __LINE__); } }

#define LOOP6_ASSERT(I,J,K,L,M,N,X) { \
   if (!(X)) { \
       bsl::cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" << \
       #K << ": " << K << "\t" << #L << ": " << L << "\t" << \
       #M << ": " << M << "\t" << #N << ": " << N << "\n"; \
       aSsErT(1, #X, __LINE__); } }

//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define P(X) bsl::cout << #X " = " << (X) << bsl::endl;
    // Print identifier and value.
#define Q(X) bsl::cout << "<| " #X " |>" << bsl::endl;
    // Quote identifier literally.
#define P_(X) bsl::cout << #X " = " << (X) << ", "<< bsl::flush;
    // P(X) without '\n'
#define L_ __LINE__                           // current Line number

namespace {
//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------
typedef bsls_PlatformUtil::Int64 Int64;
//=============================================================================
//                  CLASSES FOR TESTING USAGE EXAMPLES
//-----------------------------------------------------------------------------

} // close anonymous namespace

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

    bslma_TestAllocator testAllocator(veryVeryVerbose);

    switch (test) { case 0:  // Zero is always the leading case.
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING/USAGE TEST
        //
        // Concerns:
        //   The streamin and stream out of packet structures
        //   The setQuery(/Response) of packet structures
        // Plan:
        //   Create a bdem_List of integers in a bdem_ManagedPtr.
        //   Create a btemt_Query and setQuery with the managed bdem_List.
        //   stream out btemt_Query and an Int64 queryId into test stream
        //   stream in from the test stream into btemt_QueryRequest
        //   assert queryRequest.query() == query.query()
        //   Create a bdem_List of integers in a bdem_ManagedPtr.
        //   Create a btemt_QueryRequest and setQuery the managed bdem_List
        //   queryRequest.query() to a local managed ptr
        //   Create an empty btemt_QueryResult.
        //   Double the integers in the bdem_List and setQueryResponse
        //   queryResult.setSeqNum
        //   stream out queryResult and the Int64 queryId and status
        //   BTEMT_SUCCESS
        //   stream in from the test stream into btemt_QueryResponse
        //   assert queryResponse.queryResponse()==queryResult.queryResponse()
        // --------------------------------------------------------------------
          if (verbose) {
              bsl::cout << "\nBREATHING TEST\n==============" << bsl::endl;
          }
          bdem_List *list1 = new(testAllocator) bdem_List(&testAllocator);
          bdema_ManagedPtr<bdem_List> listFromClient(list1, &testAllocator);
          for (int i = 0; i < 5; ++i) {
              listFromClient->appendInt(i * 100 + i * 10 + i);
          }

          btemt_Query query;
          query.setQuery(listFromClient);
          query.setCategory((void *)123);
          const Int64 QUERYID = 314159;

          bdex_TestOutStream osToProcessor;
          const int VERSION = bdem_List::maxSupportedBdexVersion();
          osToProcessor.putVersion(VERSION);
          bdex_OutStreamFunctions::streamOut(
                                           osToProcessor,
                                           ((const btemt_Query&)query).query(),
                                           VERSION);
          bdex_OutStreamFunctions::streamOut(osToProcessor, QUERYID, 0);

          char buffer[1024];
          bsl::memcpy(buffer, osToProcessor.data(), osToProcessor.length());

          bdex_TestInStream isFromDispatcher(buffer, sizeof buffer);
          bdem_List *list2 = new(testAllocator) bdem_List(&testAllocator);
          bdema_ManagedPtr<bdem_List> listFromDispatcher(list2,
                                                         &testAllocator);
          Int64 queryId;
          int version;
          isFromDispatcher.getVersion(version);
          bdex_InStreamFunctions::streamIn(isFromDispatcher,
                                           *listFromDispatcher,
                                           version);
          bdex_InStreamFunctions::streamIn(isFromDispatcher, queryId, 0);

          btemt_QueryRequest queryRequest;
          queryRequest.setQuery(listFromDispatcher);
          queryRequest.setQueryId(queryId);

          ASSERT(*queryRequest.query() == *query.query());
          ASSERT(queryRequest.queryId() == QUERYID);

          bdema_ManagedPtr<bdem_List> listFromServer =
              queryRequest.query();
          for (int i = 0; i < listFromServer->length(); ++i) {
              listFromServer->theModifiableInt(i) *= 2;
          }

          const int SEQNUM = 1;

          btemt_QueryResult queryResult;
          queryResult.setQueryResponse(listFromServer);
          queryResult.setSequenceNum(SEQNUM);

          bdex_TestOutStream osToDispatcher;
          osToDispatcher.putVersion(VERSION);
          bdex_OutStreamFunctions::streamOut(
                       osToDispatcher,
                       ((const btemt_QueryResult&)queryResult).queryResponse(),
                       VERSION);
          bdex_OutStreamFunctions::streamOut(osToDispatcher,
                                             queryResult.sequenceNum(),
                                             0);
          bdex_OutStreamFunctions::streamOut(osToDispatcher, QUERYID, 0);
          bdex_OutStreamFunctions::streamOut(
                                      osToDispatcher,
                                      (int) btemt_QueryResponse::BTEMT_SUCCESS,
                                      0);

          bsl::memcpy(buffer, osToDispatcher.data(),
                      osToDispatcher.length());
          bdex_TestInStream isFromProcessor(buffer, sizeof buffer);
          bdem_List *list3 = new(testAllocator) bdem_List(&testAllocator);
          bdema_ManagedPtr<bdem_List> listFromProcessor(list3,
                                                        &testAllocator);
          int seqNum;
          int status;

          isFromProcessor.getVersion(version);
          bdex_InStreamFunctions::streamIn(isFromProcessor,
                                           *listFromProcessor,
                                           version);
          bdex_InStreamFunctions::streamIn(isFromProcessor, seqNum, 0);
          bdex_InStreamFunctions::streamIn(isFromProcessor, queryId, 0);
          bdex_InStreamFunctions::streamIn(isFromProcessor, status, 0);

          btemt_QueryResponse queryResponse;
          queryResponse.setQueryResponse(listFromProcessor);
          queryResponse.setSequenceNum(seqNum);
          queryResponse.setStatus((btemt_QueryResponse::Status)status);
          queryResponse.setQueryId(queryId);

          ASSERT(*queryResponse.queryResponse() ==
                 *queryResult.queryResponse());
          ASSERT(queryResponse.sequenceNum() == queryResult.sequenceNum());
          ASSERT(queryResponse.queryId() == QUERYID);
          ASSERT(queryResponse.status() == btemt_QueryResponse::BTEMT_SUCCESS);

      } break;
      default: {
        bsl::cerr << "WARNING: CASE `" << test << "' NOT FOUND." << bsl::endl;
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        bsl::cerr << "Error, non-zero test status = "
                  << testStatus << "." << bsl::endl;
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
