// btemt_queryxferutil.cpp                  -*-C++-*-
#include <btemt_queryxferutil.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(btemt_queryxferutil_cpp,"$Id$ $CSID$")

#include <btemt_message.h>

#include <bdef_function.h>

#include <bdema_managedptr.h>
#include <bdema_sequentialallocator.h>

#include <bdex_byteinstreamformatter.h>
#include <bdex_byteoutstreamformatter.h>
#include <bdex_instreamfunctions.h>
#include <bdex_outstreamfunctions.h>

#include <bcesb_pooledbufferchainstreambuf.h>
#include <bcema_pooledbufferchain.h>
#include <bcema_sharedptr.h>

#include <bsls_assert.h>
#include <bsls_platform.h>
#include <bsls_platformutil.h>

#if defined(BSLS_PLATFORM__OS_UNIX)
# include <netinet/in.h>  // ntohl()
#else
# include <winsock2.h>    // ntohl()
#endif

namespace BloombergLP {

                        // =========================
                        // class btemt_QueryXferUtil
                        // =========================

int btemt_QueryXferUtil::parseQueryRequest(
    const bdef_Function<void (*)(
        const bcema_SharedPtr<btemt_QueryRequest>&)>& procFunctor,
    int *numConsumed, int *numNeeded,
    const btemt_DataMsg& data,
    bslma_Allocator *allocator)
{
    BSLS_ASSERT(numConsumed && numNeeded);

    allocator = bslma_Default::allocator(allocator);

    bcema_PooledBufferChain *bufferChain = data.data();
    bcesb_PooledBufferChainStreamBuf streamBuf(bufferChain);
    bdex_ByteInStreamFormatter is(&streamBuf);

    int processedPackets = 0;
    *numConsumed = 0;
    *numNeeded = 0;

    while (is) {
        int packetLength;
        bdex_InStreamFunctions::streamIn(is, packetLength, 0);
        if (!is) {
            *numNeeded =
                sizeof(int) - (bufferChain->length() - *numConsumed);
            BSLS_ASSERT(*numNeeded > 0);
            return processedPackets;
        }
        BSLS_ASSERT(packetLength > (int) sizeof(int));

        *numConsumed += sizeof(int);

        if ((bufferChain->length() - *numConsumed) <
            (packetLength - (int) sizeof(int))) {

            *numNeeded = (packetLength - sizeof(int)) -
                (bufferChain->length() - *numConsumed);
            *numConsumed -= sizeof(int); // rollback numConsumed

            BSLS_ASSERT(*numNeeded > 0);
            return processedPackets;
        }

        bdema_ManagedPtr<bdema_SequentialAllocator> sa(
            new (*allocator) bdema_SequentialAllocator(allocator),
            allocator);
        bdem_List *qry =
            new (*sa) bdem_List(bdem_AggregateOption::BDEM_SUBORDINATE,
                                       sa.ptr());
        bdema_ManagedPtr<bdem_List> query(sa, qry);
        bsls_PlatformUtil::Int64    queryId;

        int version;
        is.getVersion(version);
        if (!is || version > bdem_List::maxSupportedBdexVersion()) {
            // numNeeded = desired query length minus what's left in the
            // bufferChain
            *numNeeded = (packetLength - sizeof(int)) -
                (bufferChain->length() - *numConsumed);
            *numConsumed -= sizeof(int);
            BSLS_ASSERT(*numNeeded > 0);
            return processedPackets;
        }

        bdex_InStreamFunctions::streamIn(is, *query, version);
        bdex_InStreamFunctions::streamIn(is, queryId, 0);

        if (!is) {
            // numNeeded = desired query length minus what's left in the
            // bufferChain
            *numNeeded = (packetLength - sizeof(int)) -
                (bufferChain->length() - *numConsumed);
            *numConsumed -= sizeof(int);
            BSLS_ASSERT(*numNeeded > 0);
            return processedPackets;
        }

        bcema_SharedPtr<btemt_QueryRequest> queryRequest;
        queryRequest.createInplace(allocator);

        queryRequest->setQuery(query);
        queryRequest->setQueryId(queryId);
        procFunctor(queryRequest);

        *numConsumed += packetLength - sizeof(int);
        *numNeeded = sizeof(int);
        ++processedPackets;
    }
    return processedPackets;
}

int btemt_QueryXferUtil::parseQueryResponse(
    const bdef_Function<void (*)(
        const bcema_SharedPtr<btemt_QueryResponse>&)>& procFunctor,
    int *numConsumed, int *numNeeded,
    const btemt_DataMsg& data,
    bslma_Allocator *allocator)
{
    BSLS_ASSERT(numConsumed && numNeeded);

    allocator = bslma_Default::allocator(allocator);

    bcema_PooledBufferChain *bufferChain = data.data();
    bcesb_PooledBufferChainStreamBuf streamBuf(bufferChain);
    bdex_ByteInStreamFormatter is(&streamBuf);

    int processedPackets = 0;
    *numConsumed = 0;
    *numNeeded = 0;

    while (is) {
        int packetLength;
        bdex_InStreamFunctions::streamIn(is, packetLength, 0);
        if (!is) {
            *numNeeded =
                sizeof(int) - (bufferChain->length() - *numConsumed);
            BSLS_ASSERT(*numNeeded > 0);
            return processedPackets;
        }

        BSLS_ASSERT(packetLength > (int) sizeof(int));

        *numConsumed += sizeof(int);

        if ((bufferChain->length() - *numConsumed) <
            (packetLength - (int) sizeof(int))) {

            *numNeeded = (packetLength - sizeof(int)) -
                (bufferChain->length() - *numConsumed);
            *numConsumed -= sizeof(int);

            BSLS_ASSERT(*numNeeded > 0);
            return processedPackets;
        }

        bdema_ManagedPtr<bdema_SequentialAllocator> sa(
            new (*allocator) bdema_SequentialAllocator(allocator),
            allocator);
        bdem_List *resp =
            new (*sa) bdem_List(bdem_AggregateOption::BDEM_SUBORDINATE,
                                sa.ptr());
        bdema_ManagedPtr<bdem_List> response(sa, resp);

        int sequenceNum;
        bsls_PlatformUtil::Int64 queryId;
        int status;
        int version;
        is.getVersion(version);

        if (!is || version > bdem_List::maxSupportedBdexVersion()) {
            *numNeeded = (packetLength - sizeof(int)) -
                (bufferChain->length() - *numConsumed);
            *numConsumed -= sizeof(int);
            BSLS_ASSERT(*numNeeded > 0);
            return processedPackets;
        }

        bdex_InStreamFunctions::streamIn(is, *response, version);
        bdex_InStreamFunctions::streamIn(is, sequenceNum, 0);
        bdex_InStreamFunctions::streamIn(is, queryId, 0);
        bdex_InStreamFunctions::streamIn(is, status, 0);

        if (!is) {
            *numNeeded = (packetLength - sizeof(int)) -
                (bufferChain->length() - *numConsumed);
            *numConsumed -= sizeof(int);
            BSLS_ASSERT(*numNeeded > 0);
            return processedPackets;
        }

        bcema_SharedPtr<btemt_QueryResponse> queryResponse;
        queryResponse.createInplace(allocator);

        queryResponse->setQueryResponse(response);
        queryResponse->setSequenceNum(sequenceNum);
        queryResponse->setQueryId(queryId);
        queryResponse->setStatus((btemt_QueryResponse::Status)status);
        procFunctor(queryResponse);

        *numConsumed += packetLength - sizeof(int);
        *numNeeded = sizeof(int);
        ++processedPackets;
    }
    return processedPackets;
}

int btemt_QueryXferUtil::serializeQuery(
    btemt_DataMsg *result,
    const btemt_Query& query,
    const bsls_PlatformUtil::Int64& queryId)
{
    BSLS_ASSERT(result);

    bcema_PooledBufferChain *bufferChain = result->data();
    bcesb_PooledBufferChainStreamBuf streamBuf(bufferChain);
    bdex_ByteOutStreamFormatter os(&streamBuf);

    // 0 holds place for size of int at the beginning of streamBuf
    bdex_OutStreamFunctions::streamOut(os, 0, 0);
    const int version = 3; // current bdem_List::maxSupportedBdexVersion()
    os.putVersion(version);
    bdex_OutStreamFunctions::streamOut(os, query.query(), version);
    bdex_OutStreamFunctions::streamOut(os, queryId, 0);
    os.flush();

    int packetLengthNbo = htonl(bufferChain->length());

    bufferChain->replace(0, (const char *)&packetLengthNbo,
                         sizeof(int));

    return bufferChain->length();
}

int btemt_QueryXferUtil::serializeQueryResult(
    btemt_DataMsg *result,
    const btemt_QueryResult& queryResult,
    const bsls_PlatformUtil::Int64& queryId,
    btemt_QueryResponse::Status status)
{
    BSLS_ASSERT(result);

    bcema_PooledBufferChain *bufferChain = result->data();
    bcesb_PooledBufferChainStreamBuf streamBuf(bufferChain);
    bdex_ByteOutStreamFormatter os(&streamBuf);

    bdex_OutStreamFunctions::streamOut(os, 0, 0);
    const int version = 3; // current bdem_List::maxSupportedBdexVersion()
    os.putVersion(version);
    bdex_OutStreamFunctions::streamOut(os,
                                       queryResult.queryResponse(),
                                       version);
    bdex_OutStreamFunctions::streamOut(os, queryResult.sequenceNum(), 0);
    bdex_OutStreamFunctions::streamOut(os, queryId, 0);
    bdex_OutStreamFunctions::streamOut(os, status, 0);
    os.flush();

    int packetLengthNbo = htonl(bufferChain->length());

    bufferChain->replace(0, (const char *)&packetLengthNbo,
                         sizeof(int));

    return bufferChain->length();
}

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2007
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
