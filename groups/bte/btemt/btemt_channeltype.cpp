// btemt_channeltype.cpp           -*-C++-*-
#include <btemt_channeltype.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(btemt_channeltype_cpp,"$Id$ $CSID$")

#include <bsl_ostream.h>

namespace BloombergLP {

const char *btemt_ChannelType::toAscii(Value channelType)
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

bsl::ostream&
operator<<(bsl::ostream& output, btemt_ChannelType::Value rhs)
{
    return output << btemt_ChannelType::toAscii(rhs);
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
