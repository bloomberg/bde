// btemt_channelerror.cpp                                             -*-C++-*-
#include <btemt_channelerror.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(btemt_channelerror_cpp,"$Id$ $CSID$")

#include <bdeu_print.h>

#include <bsl_ostream.h>

namespace BloombergLP {
namespace btemt {

                        // -----------------------
                        // struct ChannelErrorRead
                        // -----------------------

// CLASS METHODS
bsl::ostream& ChannelErrorRead::print(
                                    bsl::ostream&          stream,
                                    ChannelErrorRead::Enum value,
                                    int                    level,
                                    int                    spacesPerLevel)
{
    bdeu_Print::indent(stream, level, spacesPerLevel);

    stream << ChannelErrorRead::toAscii(value);

    if (spacesPerLevel >= 0) {
        stream << '\n';
    }

    return stream;
}

const char *ChannelErrorRead::toAscii(ChannelErrorRead::Enum value)
{
#define CASE(X) case(BTEMT_ ## X): return #X;

    switch (value) {
      CASE(SUCCESS)
      CASE(CHANNEL_CLOSED)
      default: return "(* UNKNOWN *)";
    }

#undef CASE
}

                        // ------------------------
                        // struct ChannelErrorWrite
                        // ------------------------

// CLASS METHODS
bsl::ostream& ChannelErrorWrite::print(
                                    bsl::ostream&           stream,
                                    ChannelErrorWrite::Enum value,
                                    int                     level,
                                    int                     spacesPerLevel)
{
    bdeu_Print::indent(stream, level, spacesPerLevel);

    stream << ChannelErrorWrite::toAscii(value);

    if (spacesPerLevel >= 0) {
        stream << '\n';
    }

    return stream;
}

const char *ChannelErrorWrite::toAscii(ChannelErrorWrite::Enum value)
{
#define CASE(X) case(BTEMT_ ## X): return #X;

    switch (value) {
      CASE(SUCCESS)
      CASE(CACHE_HIWAT)
      CASE(HIT_CACHE_HIWAT)
      CASE(CHANNEL_DOWN)
      CASE(ENQUEUE_WAT)
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
