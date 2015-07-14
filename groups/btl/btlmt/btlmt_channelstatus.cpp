// btlmt_channelstatus.cpp                                            -*-C++-*-
#include <btlmt_channelstatus.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(btlmt_channelstatus_cpp,"$Id$ $CSID$")

#include <bdlb_print.h>

#include <bsl_ostream.h>

namespace BloombergLP {

namespace btlmt {
                        // --------------------
                        // struct ChannelStatus
                        // --------------------

// CLASS METHODS
bsl::ostream& ChannelStatus::print(
                                      bsl::ostream&             stream,
                                      ChannelStatus::Enum value,
                                      int                       level,
                                      int                       spacesPerLevel)
{
    bdlb::Print::indent(stream, level, spacesPerLevel);

    stream << ChannelStatus::toAscii(value);

    if (spacesPerLevel >= 0) {
        stream << '\n';
    }

    return stream;
}

const char *ChannelStatus::toAscii(ChannelStatus::Enum value)
{
#define CASE(X) case(BTEMT_ ## X): return #X;

    switch (value) {
      CASE(SUCCESS)
      CASE(CACHE_OVERFLOW)
      CASE(CACHE_HIGHWATER)
      CASE(READ_CHANNEL_DOWN)
      CASE(WRITE_CHANNEL_DOWN)
      CASE(ENQUEUE_HIGHWATER)
      CASE(UNKNOWN_ID)
      default: return "(* UNKNOWN *)";
    }

#undef CASE
}
}  // close package namespace

}  // close enterprise namespace

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2012
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ----------------------------------
