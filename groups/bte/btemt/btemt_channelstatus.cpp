// btemt_channelstatus.cpp                                            -*-C++-*-
#include <btemt_channelstatus.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(btemt_channelstatus_cpp,"$Id$ $CSID$")

#include <bdeu_print.h>

#include <bsl_ostream.h>

namespace BloombergLP {

                        // --------------------
                        // struct btemt_ChannelStatus
                        // --------------------

// CLASS METHODS
bsl::ostream& btemt_ChannelStatus::print(
                                      bsl::ostream&             stream,
                                      btemt_ChannelStatus::Enum value,
                                      int                       level,
                                      int                       spacesPerLevel)
{
    bdeu_Print::indent(stream, level, spacesPerLevel);

    stream << btemt_ChannelStatus::toAscii(value);

    if (spacesPerLevel >= 0) {
        stream << '\n';
    }

    return stream;
}

const char *btemt_ChannelStatus::toAscii(btemt_ChannelStatus::Enum value)
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

}  // close enterprise namespace

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2012
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ----------------------------------
