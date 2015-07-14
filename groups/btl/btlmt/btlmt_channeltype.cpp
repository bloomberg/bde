// btlmt_channeltype.cpp           -*-C++-*-
#include <btlmt_channeltype.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(btlmt_channeltype_cpp,"$Id$ $CSID$")

#include <bsl_ostream.h>

namespace BloombergLP {

namespace btlmt {
const char *ChannelType::toAscii(Value channelType)
{
#define CASE(X) case(BTEMT_ ## X): return #X

    switch (channelType) {
      CASE(LISTENING_CHANNEL);
      CASE(ACCEPTED_CHANNEL);
      CASE(CONNECTING_CHANNEL);
      CASE(CONNECTED_CHANNEL);
      CASE(IMPORTED_CHANNEL);
      default: return "(* UNKNOWN *)";
    }

#undef CASE
}
}  // close package namespace

bsl::ostream&
btlmt::operator<<(bsl::ostream& output, ChannelType::Value rhs)
{
    return output << ChannelType::toAscii(rhs);
}

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2008
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
