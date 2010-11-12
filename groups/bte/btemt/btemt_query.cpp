// btemt_query.cpp                  -*-C++-*-
#include <btemt_query.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(btemt_query_cpp,"$Id$ $CSID$")

#include <bsl_ostream.h>

namespace BloombergLP {

                        // =================
                        // class btemt_Query
                        // =================
btemt_Query& btemt_Query::operator=(btemt_Query& rhs)
{
    if (this != &rhs) {
        d_query    = rhs.d_query;
        d_category = rhs.d_category;
    }
    return *this;
}

bool operator==(const btemt_Query& lhs, const btemt_Query& rhs)
{
    if (&lhs == &rhs) {
        return true;
    }
    else if (lhs.category() != rhs.category()) {
        return false;
    }
    else if (0 == lhs.d_query.ptr() && 0 == rhs.d_query.ptr()) {
        return true;
    }
    else if (0 != lhs.d_query.ptr() && 0 != rhs.d_query.ptr()) {
        return lhs.query() == rhs.query();
    }
    else { // either lhs or rhs doesn't manage a bdem_List, but not both
        return false;
    }
}

bsl::ostream& operator<<(bsl::ostream& os, const btemt_Query& rhs)
{
    os << "query = " << rhs.query()
       << ", category = " << rhs.category();
    return os;
}

                        // ========================
                        // class btemt_QueryRequest
                        // ========================
btemt_QueryRequest& btemt_QueryRequest::operator=(btemt_QueryRequest& rhs)
{
    if (this != &rhs) {
        d_query   = rhs.d_query;
        d_queryId = rhs.d_queryId;
    }
    return *this;
}

bool operator==(const btemt_QueryRequest& lhs, const btemt_QueryRequest& rhs)
{
    if (&lhs == &rhs) {
        return true;
    }
    else if (lhs.queryId() != rhs.queryId()) {
        return false;
    }
    else if (0 == lhs.d_query.ptr() && 0 == rhs.d_query.ptr()) {
        return true;
    }
    else if (0 != lhs.d_query.ptr() && 0 != rhs.d_query.ptr()) {
        return lhs.query() == rhs.query();
    }
    else {
        return false;
    }
}

bsl::ostream& operator<<(bsl::ostream& os, const btemt_QueryRequest& rhs)
{
    os << "query = " << rhs.query()
       << ", queryId = " << rhs.queryId();
    return os;
}

                        // =======================
                        // class btemt_QueryResult
                        // =======================
btemt_QueryResult& btemt_QueryResult::operator=(btemt_QueryResult& rhs)
{
    if (this != &rhs) {
        d_queryResponse = rhs.d_queryResponse;
        d_sequenceNum   = rhs.d_sequenceNum;
    }
    return *this;
}

bool operator==(const btemt_QueryResult& lhs, const btemt_QueryResult& rhs)
{
    if (&lhs == &rhs) {
        return true;
    }
    else if (lhs.sequenceNum() != rhs.sequenceNum()) {
        return false;
    }
    else if (0 == lhs.d_queryResponse.ptr() &&
             0 == rhs.d_queryResponse.ptr()) {
        return true;
    }
    else if (0 != lhs.d_queryResponse.ptr() &&
             0 != rhs.d_queryResponse.ptr()) {
        return lhs.queryResponse() == rhs.queryResponse();
    }
    else {
        return false;
    }
}

bsl::ostream& operator<<(bsl::ostream& os, const btemt_QueryResult& rhs)
{
    os << "queryResponse = " << rhs.queryResponse()
       << ", sequenceNum = " << rhs.sequenceNum();
    return os;
}


                        // =========================
                        // class btemt_QueryResponse
                        // =========================
btemt_QueryResponse& btemt_QueryResponse::operator=(
    btemt_QueryResponse& rhs)
{
    if (this != &rhs) {
        d_queryResponse = rhs.d_queryResponse;
        d_sequenceNum   = rhs.d_sequenceNum;
        d_queryId       = rhs.d_queryId;
        d_status        = rhs.d_status;
    }
    return *this;
}

bool operator==(const btemt_QueryResponse& lhs, const btemt_QueryResponse& rhs)
{
    if (&lhs == &rhs) {
        return true;
    }
    else if (lhs.sequenceNum() != rhs.sequenceNum() ||
             lhs.queryId()     != rhs.queryId()     ||
             lhs.status()      != rhs.status()) {
        return false;
    }
    else if (0 == lhs.d_queryResponse.ptr() &&
             0 == rhs.d_queryResponse.ptr()) {
        return true;
    }
    else if (0 != lhs.d_queryResponse.ptr() &&
             0 != rhs.d_queryResponse.ptr()) {
        return lhs.queryResponse() == rhs.queryResponse();
    }
    else {
        return false;
    }
}

bsl::ostream& operator<<(bsl::ostream& os, const btemt_QueryResponse& rhs)
{
    os << "result = " << rhs.queryResponse()
       << ", sequenceNum = " << rhs.sequenceNum()
       << ", queryId = " << rhs.queryId()
       << ", status = " << rhs.status();
    return os;
}

} // Close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2007
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
