// btemt_queryxferutil.h                                              -*-C++-*-
#ifndef INCLUDED_BTEMT_QUERYXFERUTIL
#define INCLUDED_BTEMT_QUERYXFERUTIL

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide utilities for parsing & serializing 'btemt_Query' packets.
//
//@CLASSES:
//  btemt_QueryXferUtil: 'btemt_Query' packet parsing and serializing functions
//
//@AUTHOR: Xiheng Xu (xxu)
//
//@DESCRIPTION: This component defines a class, 'btemt_QueryXferUtil', that
// provides a namespace for parsing and serializing utility functions that
// operate on 'btemt_Query' objects.
//
///Usage
///-----
// TBD

#ifndef INCLUDED_BTESCM_VERSION
#include <btescm_version.h>
#endif

#ifndef INCLUDED_BTEMT_QUERY
#include <btemt_query.h>
#endif

#ifndef INCLUDED_BCEMA_SHAREDPTR
#include <bcema_sharedptr.h>
#endif

#ifndef INCLUDED_BDEF_FUNCTION
#include <bdef_function.h>
#endif

#ifndef INCLUDED_BSLS_PLATFORMUTIL
#include <bsls_platformutil.h>
#endif

#ifndef INCLUDED_BSL_VECTOR
#include <bsl_vector.h>
#endif

#ifndef INCLUDED_BSLFWD_BSLMA_ALLOCATOR
#include <bslfwd_bslma_allocator.h>
#endif

namespace BloombergLP {

class bdem_List;
class btemt_DataMsg;

struct btemt_QueryXferUtil {
    // This 'struct' provides a namespace for parsing and serializing utility
    // functions that operate on 'btemt_Query' packets.
    // TBD: relation between the four functions.

    // CLASS METHODS
    static int serializeQuery(btemt_DataMsg *result,
                              const btemt_Query& query,
                              const bsls_PlatformUtil::Int64& queryId);
        // Load into the 'result' the serialized representation of the 'query'
        // followed by serialized 'queryId'.  Return the number of bytes
        // written.  One such serialization includes the size of packet as well
        // as the 'query' and 'queryId'.  Behavior is undefined if 'query'
        // doesn't contain query data.  The '*result' can be parsed using
        // 'parseQueryRequest()'.

    static int parseQueryRequest(
        const bdef_Function<void (*)(
            const bcema_SharedPtr<btemt_QueryRequest>&)>& procFunctor,
        int *numConsumed, int *numNeeded,
        const btemt_DataMsg& data,
        bslma_Allocator *allocator = 0);
        // Parse the 'data' and use the 'procFunctor' to process parsed
        // btemt_QueryRequest packets.  Return number of consumed bytes in
        // 'numConsumed', and number of needed bytes in 'numNeeded' if the last
        // packet provided in 'data' is only partial.  Note: one packet is what
        // 'serializeQuery()' can serialize for one query into a btemt_DataMsg.
        // Return value is the number of complete btemt_QueryRequest packets
        // thus parsed.  Each parsed btemt_QueryRequest manages its managed
        // object using 'allocator' if it's specified, using new/delete
        // otherwise.  TBD: examples of consumed and needed

    static int serializeQueryResult(btemt_DataMsg *result,
                                    const btemt_QueryResult& queryResult,
                                    const bsls_PlatformUtil::Int64& queryId,
                                    btemt_QueryResponse::Status status);
        // Load into the 'result' the serialized representation of the
        // 'queryResult' followed by serialized 'queryId' and 'status'.  Return
        // the number of bytes written.  One such serialization includes the
        // size of packet as well as the 'queryResult', 'queryId' and 'status'.
        // Behavior is undefined if 'queryResult' doesn't contain query result
        // data.  The '*result' can be parsed using 'parseQueryResponse()'.

    static int parseQueryResponse(
        const bdef_Function<void (*)(
            const bcema_SharedPtr<btemt_QueryResponse>&)>& procFunctor,
        int *numConsumed, int *numNeeded,
        const btemt_DataMsg& data,
        bslma_Allocator *allocator = 0);
        // Parse the 'data' and use the 'procFunctor' to process parsed
        // btemt_QueryResponse packets.  Return number of consumed bytes in
        // 'numConsumed', and number of needed bytes in 'numNeeded' if the last
        // packet provided in 'data' is only partial.  Note: one packet is what
        // 'serializeQueryResult()' can serialize for one queryResult into a
        // btemt_DataMsg.  Return value is the number of complete
        // btemt_QueryResponse packets thus parsed.  Each parsed
        // btemt_QueryResponse manages its managed object using 'allocator' if
        // it's specified, using new/delete otherwise.
};

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2004
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
