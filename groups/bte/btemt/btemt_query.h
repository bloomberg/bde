// btemt_query.h                  -*-C++-*-
#ifndef INCLUDED_BTEMT_QUERY
#define INCLUDED_BTEMT_QUERY

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Define the communication packet structures between client,
// btemt_QueryDispatcher, btemt_QueryProcessor, and server.
//
//@CLASSES: btemt_Query btemt_QueryRequest btemt_QueryResult
//          btemt_QueryResponse
//
//@SEE_ALSO: btemt_QueryDispatcher btemt_QueryProcessor
//
//@AUTHOR: Xiheng Xu (xxu)
//
//@DESCRIPTION: This class defines four communication packet structures
// between client, btemt_QueryDispatcher, btemt_QueryProcessor, and server.
// The following table shows how the four parties communicate through the
// btemt_Query* framework using these packets.
//..
// * btemt_Query         - client prepares this and asks btemt_QueryDispatcher
//                         to send it to a connected btemt_QueryProcessor
//
// * btemt_QueryRequest  - btemt_QueryProcessor reads the client's query
//                         request in the format of this packet for server to
//                         process.
//
// * btemt_QueryResult   - server prepares this and asks btemt_QueryProcessor
//                         to send it back to the originating
//                         btemt_QueryDispatcher.
//
// * btemt_QueryResponse - btemt_QueryDispatcher reads the server's query
//                         response in the format of this packet for client to
//                         process
//..
//
// All four packet classes have their underlying data object as a bdem_List,
// which the client and the server prepare and process as query and query
// response.  They all use bdema_ManagedPtr to the underlying data object and
// therefore they're decidedly not value semantic.  This arrangement achieves
// efficient passing of data between the communicating entities.
//
///Usage
///-----
// TBD

#ifndef INCLUDED_BTESCM_VERSION
#include <btescm_version.h>
#endif

#ifndef INCLUDED_BDEM_LIST
#include <bdem_list.h>
#endif

#ifndef INCLUDED_BSLMA_DEFAULT
#include <bslma_default.h>
#endif

#ifndef INCLUDED_BSLS_PLATFORMUTIL
#include <bsls_platformutil.h>
#endif

#ifndef INCLUDED_BDEMA_MANAGEDPTR
#include <bdema_managedptr.h>
#endif

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#endif

namespace BloombergLP {

                        // =================
                        // class btemt_Query
                        // =================

class btemt_Query {
    // This is a packet structure for client to give queries to the
    // btemt_QueryDispatcher.  The dispatcher in turn serializes both this
    // packet and an assigned queryId for the packet onto the wire.  This is
    // not a value-semantic object as it implements bsl::auto_ptr
    // semantics.  (see bdema_managedptr)
    bdema_ManagedPtr<bdem_List> d_query;
                                    // query data.
    void                       *d_category;
                                    // category of the query.  It's a value,
                                    // not a pointer to object, it's not
                                    // serialized or deserialized.  That is,
                                    // when this packet is deserialized from
                                    // wire, this field is always filled as
                                    // zero.
    // FRIENDS
    friend bool operator==(const btemt_Query& lhs, const btemt_Query& rhs);

  public:
    // CREATORS
    btemt_Query();
        // Create an empty object that doesn't manage any bdem_List.

    btemt_Query(btemt_Query& rhs);
        // Create an object that manages the bdem_List previously managed by
        // 'rhs'.  This is not a regular copy constructor.  'rhs' no longer
        // owns the managed bdem_List after this constructor.

    ~btemt_Query();
        // Destroy the object.  The managed bdem_List no longer exists after
        // this destructor.

    // MANIPULATORS
    btemt_Query& operator=(btemt_Query& rhs);
        // Assign the 'rhs' to this object, which after this call manages the
        // bdem_List that was managed by 'rhs'.  This is not a regular
        // operator=.  'rhs' no longer manages the bdem_List after this
        // assignment.

    void setQuery(bdema_ManagedPtr<bdem_List>& query);
        // Transfer the managed bdem_List in 'query' to this object.  'query'
        // no longer manages the just-transferred bdem_List.

    void setCategory(void *category);
        // Set 'category' in this object.

    bdema_ManagedPtr<bdem_List>& query();
        // Give away the managed bdem_List, which is no longer owned by this
        // object if the return value is assigned.  If return value is not
        // assigned, the call has no effect.

    // ACCESSORS
    const bdem_List& query() const;
        // Return a constant reference to the managed bdem_List.  The returned
        // reference is valid as long as the bdem_List is being managed by
        // this object.  Behavior is undefined unless this object manages a
        // bdem_List.

    void *category() const;
        // Return the category.
};

// FREE OPERATORS
bool operator==(const btemt_Query& lhs, const btemt_Query& rhs);
    // Return true 1) if both 'lhs' and 'rhs' have the same category, and
    // operator== returns true for their managed bdem_List objects, or 2) if
    // neither manages a bdem_List object, but both have the same category.
    // Return false otherwise.

inline
bool operator!=(const btemt_Query& lhs, const btemt_Query& rhs);
    // Return true 1) if 'lhs' and 'rhs' don't have the same category, or
    // 2) if operator!= returns true on their managed bdem_List objects, or
    // 3) if only one of 'lhs' and 'rhs' manages a bdem_List object.  Return
    // false otherwise.

bsl::ostream& operator<<(bsl::ostream& os, const btemt_Query& rhs);
    // Output 'rhs' onto 'os'.  Behavior is undefined unless 'rhs' manages a
    // bdem_List object.

                        // ========================
                        // class btemt_QueryRequest
                        // ========================
class btemt_QueryRequest {
    // This is a packet structure that a btemt_QueryProcessor parses out of
    // the wire from a btemt_QueryDispatcher.  The packet contains query data
    // for the server to process and a queryId for the btemt_QueryProcessor to
    // prepare query response.  This is not a value-semantic object as it
    // implements bsl::auto_ptr semantics (see bdema_managedptr).

    typedef bsls_PlatformUtil::Int64 Int64;

    bdema_ManagedPtr<bdem_List> d_query;
                                    // query data.
    Int64                       d_queryId;
                                    // a unique query Id for this query
    // FRIENDS
    friend bool operator==(const btemt_QueryRequest& lhs,
                           const btemt_QueryRequest& rhs);

  public:
    // CREATORS
    btemt_QueryRequest();
        // Create an empty object that doesn't manage any bdem_List.

    btemt_QueryRequest(btemt_QueryRequest& rhs);
        // Create an object that manages the bdem_List previously managed by
        // 'rhs'.  This is not a regular copy constructor.  'rhs' no longer
        // owns the managed bdem_List after this constructor.

    ~btemt_QueryRequest();
        // Destroy the object.  The managed bdem_List no longer exists after
        // this destructor.

    // MANIPULATORS
    btemt_QueryRequest& operator=(btemt_QueryRequest& rhs);
        // Assign the 'rhs' to this object, which after this call manages the
        // bdem_List that was managed by 'rhs'.  This is not a regular
        // operator=.  'rhs' no longer manages the bdem_List after this
        // assignment.

    void setQuery(bdema_ManagedPtr<bdem_List>& query);
        // Transfer the managed bdem_List in 'query' to this object.  'query'
        // no longer manages the just-transferred bdem_List.

    void setQueryId(const Int64& queryId);
        // Set 'queryId' in this object.

    bdema_ManagedPtr<bdem_List>& query();
        // Give away the managed bdem_List, which is no longer owned by this
        // object if the return value is assigned.  If return value is not
        // assigned, the call has no effect.

    // ACCESSORS
    const bdem_List& query() const;
        // Return a constant reference to the managed bdem_List.  The returned
        // reference is valid as long as the bdem_List is being managed by
        // this object.  Behavior is undefined unless this object manages a
        // bdem_List.

    Int64 queryId() const;
        // Return the queryId.
};

// FREE OPERATORS
bool operator==(const btemt_QueryRequest& lhs, const btemt_QueryRequest& rhs);
    // Return true if 1) if both 'lhs' and 'rhs' have the same queryId, and
    // operator== returns true for their managed bdem_List objects, or 2) if
    // neither manages a bdem_List object, but both have the same queryId.
    // Return false otherwise.

inline
bool operator!=(const btemt_QueryRequest& lhs, const btemt_QueryRequest& rhs);
    // Return true 1) if 'lhs' and 'rhs' don't have the same queryId, or 2) if
    // operator!= returns true on their managed bdem_List objects, or 3) if
    // only one of 'lhs' and 'rhs' manages a bdem_List object.  Return false
    // otherwise.

bsl::ostream& operator<<(bsl::ostream& os, const btemt_QueryRequest& rhs);
    // Output 'rhs' onto 'os'.  Behavior is undefined unless 'rhs' manages a
    // bdem_List object.

                        // =======================
                        // class btemt_QueryResult
                        // =======================

class btemt_QueryResult {
    // This is a packet structure for server to give query results back to the
    // btemt_QueryProcessor.  The processor in turn serializes both this packet
    // and the queryId it keeps while receiving the corresponding
    // btemt_QueryRequest packet, as well as a BTEMT_SUCCESS status onto the
    // wire to the btemt_QueryDispatcher.  This is not a value-semantic object
    // as it implements bsl::auto_ptr semantics (see bdema_managedptr).

    bdema_ManagedPtr<bdem_List> d_queryResponse;
                                    // query response data.
    int                         d_sequenceNum;
                                    // sequence number associated with this
                                    // object, as there may be more than one
                                    // such query result to a single query
                                    // request.
    // FRIENDS
    friend bool operator==(const btemt_QueryResult& lhs,
                           const btemt_QueryResult& rhs);

  public:
    // CREATORS
    btemt_QueryResult();
        // Create an empty object that doesn't manage any bdem_List.

    btemt_QueryResult(btemt_QueryResult& rhs);
        // Create an object that manages the bdem_List previously managed by
        // 'rhs'.  This is not a regular copy constructor.  'rhs' no longer
        // owns the managed bdem_List after this constructor.

    ~btemt_QueryResult();
        // Destroy the object.  The managed bdem_List no longer exists after
        // this destructor.

    // MANIPULATORS
    btemt_QueryResult& operator=(btemt_QueryResult& rhs);
        // Assign the 'rhs' to this object, which after this call manages the
        // bdem_List that was managed by 'rhs'.  This is not a regular
        // operator=.  'rhs' no longer owns the managed bdem_List after this
        // assignment.

    void setQueryResponse(bdema_ManagedPtr<bdem_List>& queryResponse);
        // Transfer the managed bdem_List in 'queryResponse' to this object.
        // 'queryResponse' no longer manages the just-transferred bdem_List.

    void setSequenceNum(int sequenceNum);
        // Set 'sequenceNum' in this object.

    bdema_ManagedPtr<bdem_List>& queryResponse();
        // Give away the managed bdem_List, which is no longer owned by this
        // object if the return value is assigned.  If return value is not
        // assigned, the call has no effect.

    // ACCESSORS
    const bdem_List& queryResponse() const;
        // Return a constant reference to the managed bdem_List.  The returned
        // reference is valid as long as the bdem_List is being managed by
        // this object.  Behavior is undefined unless this object manages a
        // bdem_List.

    int sequenceNum() const;
        // Return the sequence number.
};

// FREE OPERATORS
bool operator==(const btemt_QueryResult& lhs, const btemt_QueryResult& rhs);
    // Return true if 1) both 'lhs' and 'rhs' have the same sequence number,
    // and operator== returns true for their managed bdem_List objects, or
    // 2) if neither manages a bdem_List object, but both have the same
    // sequence number.  Return false otherwise.

inline
bool operator!=(const btemt_QueryResult& lhs, const btemt_QueryResult& rhs);
    // Return true 1) if 'lhs' and 'rhs' don't have the same sequence number,
    // or 2) if operator!= returns true on their managed bdem_List objects, or
    // 3) if only one of 'lhs' and 'rhs' manages a bdem_List object.  Return
    // false otherwise.

bsl::ostream& operator<<(bsl::ostream& os, const btemt_QueryResult& rhs);
    // Output 'rhs' onto 'os'.  Behavior is undefined unless 'rhs' manages a
    // bdem_List object.

                        // =========================
                        // class btemt_QueryResponse
                        // =========================

class btemt_QueryResponse {
    // This is a packet structure that btemt_QueryDispatcher parses out of
    // the wire from a btemt_QueryProcessor and gives back to originating
    // client.  The packet contains the query result data, sequence number of
    // this result, status and a queryId the query result is responding
    // to.  This is not a value-semantic object as it implements bsl::auto_ptr
    // semantics (see bdema_managedptr).
  public:
    enum Status {
        // The packet can use this to indicate server processing status to the
        // client.
        BTEMT_SUCCESS,
        BTEMT_TIMEOUT,
        BTEMT_FAILURE
#if !defined(BSL_LEGACY) || 1 == BSL_LEGACY
      , SUCCESS = BTEMT_SUCCESS
      , TIMEOUT = BTEMT_TIMEOUT
      , FAILURE = BTEMT_FAILURE
#endif
    };
  private:
    typedef bsls_PlatformUtil::Int64 Int64;

    bdema_ManagedPtr<bdem_List> d_queryResponse;
                                    // query response data.
    int                         d_sequenceNum;
                                    // sequence number associated with this
                                    // packet, as there may be more than one
                                    // query response to a single query
                                    // request.
    Int64                       d_queryId;
                                    // unique queryId of the query to which
                                    // this query response responds to.
    Status                      d_status;
                                    // indicates status of the server
                                    // processing for this query
    // FRIENDS
    friend bool operator==(const btemt_QueryResponse& lhs,
                           const btemt_QueryResponse& rhs);

  public:
    // CREATORS
    btemt_QueryResponse();
        // Create an empty object that doesn't manage any bdem_List.

    btemt_QueryResponse(btemt_QueryResponse& rhs);
        // Create an object that manages the bdem_List previously managed by
        // 'rhs'.  This is not a regular copy constructor.  'rhs' no longer
        // owns the managed bdem_List after this constructor.

    ~btemt_QueryResponse();
        // Destroy the object.  The managed bdem_List no longer exists after
        // this destructor.

    // MANIPULATORS
    btemt_QueryResponse& operator=(btemt_QueryResponse& rhs);
        // Assign the 'rhs' to this object, which after this call manages the
        // bdem_List that was managed by 'rhs'.  This is not a regular
        // operator=.  'rhs' no longer owns the managed bdem_List after this
        // assignment.

    void setQueryResponse(bdema_ManagedPtr<bdem_List>& queryResponse);
        // Transfer the managed bdem_List in 'queryResponse' to this object.
        // 'queryResponse' no longer manages the just-transferred bdem_List.

    void setQueryId(const Int64& queryId);
        // Set 'queryId' in this object.

    void setSequenceNum(int sequenceNum);
        // Set 'sequenceNum' in this object.

    void setStatus(Status status);
        // Set 'status' in this object.

    bdema_ManagedPtr<bdem_List>& queryResponse();
        // Return a constant reference to the managed bdem_List.  The returned
        // reference is valid as long as the bdem_List is being managed by
        // this object.  Behavior is undefined unless this object manages a
        // bdem_List.

    // ACCESSORS
    const bdem_List& queryResponse() const;
        // Return a constant reference to the managed bdem_List.  The returned
        // reference is valid as long as the bdem_List is being managed by
        // this object.  Behavior is undefined unless this object manages a
        // bdem_List.

    Int64 queryId() const;
        // Return the queryId.

    int sequenceNum() const;
        // Return the sequence number.

    Status status() const;
        // Return the status.
};

// FREE OPERATORS
bool operator==(const btemt_QueryResponse& lhs,
                const btemt_QueryResponse& rhs);
    // Return true if 1) both 'lhs' and 'rhs' have the same sequence number,
    // same queryId and same status, and operator== returns true for their
    // managed bdem_List objects, or 2) if neither manages a bdem_List object,
    // but both have the same sequence number, same queryId, and same status.
    // Return false otherwise.

inline
bool operator!=(const btemt_QueryResponse& lhs,
               const btemt_QueryResponse& rhs);
    // Return true 1) if 'lhs' and 'rhs' don't have the same sequence number,
    // or same queryId, or same status, or 2) if operator!= returns true on
    // their managed bdem_List objects, or 3) if only one of 'lhs' and 'rhs'
    // manages a bdem_List object.  Return false otherwise.

bsl::ostream& operator<<(bsl::ostream& os,
                         const btemt_QueryResponse& rhs);
    // Output 'rhs' onto 'os'.  Behavior is undefined unless 'rhs' manages a
    // bdem_List object.

// ===========================================================================
//                      INLINE AND TEMPLATE FUNCTION DEFINITIONS
// ===========================================================================
                        // =================
                        // class btemt_Query
                        // =================
// CREATORS
inline
btemt_Query::btemt_Query()
: d_category(0)
{
}

inline
btemt_Query::btemt_Query(btemt_Query& rhs)
: d_query(rhs.d_query)
, d_category(rhs.d_category)
{
}

inline
btemt_Query::~btemt_Query()
{
}

// MANIPULATORS
inline
void btemt_Query::setQuery(bdema_ManagedPtr<bdem_List>& query)
{
    d_query = query;
}

inline
void btemt_Query::setCategory(void *category)
{
    d_category = category;
}

inline
bdema_ManagedPtr<bdem_List>& btemt_Query::query()
{
    return d_query;
}

// ACCESSORS
inline
const bdem_List& btemt_Query::query() const
{
    return *d_query;
}

inline
void *btemt_Query::category() const
{
    return d_category;
}

// FREE OPERATORS
inline
bool operator!=(const btemt_Query& lhs, const btemt_Query& rhs)
{
    return !(lhs == rhs);
}

                        // ========================
                        // class btemt_QueryRequest
                        // ========================
// CREATORS
inline
btemt_QueryRequest::btemt_QueryRequest()
: d_queryId(0)
{
}

inline
btemt_QueryRequest::btemt_QueryRequest(btemt_QueryRequest& rhs)
: d_query(rhs.d_query)
, d_queryId(rhs.d_queryId)
{
}

inline
btemt_QueryRequest::~btemt_QueryRequest()
{
}

// MANIPULATORS
inline
void btemt_QueryRequest::setQuery(bdema_ManagedPtr<bdem_List>& query)
{
    d_query = query;
}

inline
void btemt_QueryRequest::setQueryId(const Int64& queryId)
{
    d_queryId = queryId;
}

inline
bdema_ManagedPtr<bdem_List>& btemt_QueryRequest::query()
{
    return d_query;
}

// ACCESSORS
inline
const bdem_List& btemt_QueryRequest::query() const
{
    return *d_query;
}

inline
bsls_PlatformUtil::Int64 btemt_QueryRequest::queryId() const
{
    return d_queryId;
}

// FREE OPERATORS
inline
bool operator!=(const btemt_QueryRequest& lhs, const btemt_QueryRequest& rhs)
{
    return !(lhs == rhs);
}

                        // =======================
                        // class btemt_QueryResult
                        // =======================
// CREATORS
inline
btemt_QueryResult::btemt_QueryResult()
: d_sequenceNum(-1)
{
}

inline
btemt_QueryResult::btemt_QueryResult(btemt_QueryResult& rhs)
: d_queryResponse(rhs.d_queryResponse)
, d_sequenceNum(rhs.d_sequenceNum)
{
}

inline
btemt_QueryResult::~btemt_QueryResult()
{
}

// MANIPULATORS
inline
void btemt_QueryResult::setQueryResponse(
    bdema_ManagedPtr<bdem_List>& queryResponse)
{
    d_queryResponse = queryResponse;
}

inline
void btemt_QueryResult::setSequenceNum(int sequenceNum)
{
    d_sequenceNum = sequenceNum;
}

inline
bdema_ManagedPtr<bdem_List>& btemt_QueryResult::queryResponse()
{
    return d_queryResponse;
}

// ACCESSORS
inline
const bdem_List& btemt_QueryResult::queryResponse() const
{
    return *d_queryResponse;
}

inline
int btemt_QueryResult::sequenceNum() const
{
    return d_sequenceNum;
}

// FREE OPERATORS
inline
bool operator!=(const btemt_QueryResult& lhs, const btemt_QueryResult& rhs)
{
    return !(lhs == rhs);
}

                        // =========================
                        // class btemt_QueryResponse
                        // =========================
//CREATORS
inline
btemt_QueryResponse::btemt_QueryResponse()
: d_sequenceNum(-1)
, d_queryId(0)
, d_status(BTEMT_SUCCESS)
{
}

inline
btemt_QueryResponse::btemt_QueryResponse(btemt_QueryResponse& rhs)
: d_queryResponse(rhs.d_queryResponse)
, d_sequenceNum(rhs.d_sequenceNum)
, d_queryId(rhs.d_queryId)
, d_status(rhs.d_status)
{
}

inline
btemt_QueryResponse::~btemt_QueryResponse()
{
}

// MANIPULATORS
inline
void btemt_QueryResponse::setQueryResponse(
    bdema_ManagedPtr<bdem_List>& queryResponse)
{
    d_queryResponse = queryResponse;
}

inline
void btemt_QueryResponse::setQueryId(const Int64& queryId)
{
    d_queryId = queryId;
}

inline
void btemt_QueryResponse::setSequenceNum(int sequenceNum)
{
    d_sequenceNum = sequenceNum;
}

inline
void btemt_QueryResponse::setStatus(btemt_QueryResponse::Status status)
{
    d_status = status;
}

inline
bdema_ManagedPtr<bdem_List>& btemt_QueryResponse::queryResponse()
{
    return d_queryResponse;
}

// ACCESSORS
inline
const bdem_List& btemt_QueryResponse::queryResponse() const
{
    return *d_queryResponse;
}

inline
bsls_PlatformUtil::Int64 btemt_QueryResponse::queryId() const
{
    return d_queryId;
}

inline
int btemt_QueryResponse::sequenceNum() const
{
    return d_sequenceNum;
}

inline
btemt_QueryResponse::Status btemt_QueryResponse::status() const
{
    return d_status;
}

// FREE OPERATORS
inline
bool operator!=(const btemt_QueryResponse& lhs, const btemt_QueryResponse& rhs)
{
    return !(lhs == rhs);
}

}  // close namespace BloombergLP

#endif // ! defined(INCLUDED_BTEMT_QUERY)

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2004
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
