// ball_context.cpp                                                   -*-C++-*-
#include <ball_context.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ball_context_cpp,"$Id$ $CSID$")

#include <bdlb_print.h>
#include <bsl_ostream.h>

namespace BloombergLP {

namespace ball {
                        // ------------------
                        // class Context
                        // ------------------

// CLASS METHODS
bool Context::isValid(Transmission::Cause transmissionCause,
                           int                      recordIndex,
                           int                      sequenceLength)
{
    bool status;

    switch (transmissionCause) {
      case Transmission::BAEL_PASSTHROUGH: {
        status = 0 == recordIndex && 1 == sequenceLength;
      } break;
      case Transmission::BAEL_TRIGGER:
      case Transmission::BAEL_TRIGGER_ALL:
      case Transmission::BAEL_MANUAL_PUBLISH:
      case Transmission::BAEL_MANUAL_PUBLISH_ALL: {
        status = 0 <= recordIndex
              && 1 <= sequenceLength
              && recordIndex < sequenceLength;
      } break;
      default: {
        status = false;
      } break;
    }
    return status;
}

// ACCESSORS
bsl::ostream& Context::print(bsl::ostream& stream,
                                  int           level,
                                  int           spacesPerLevel) const
{
    if (0 <= spacesPerLevel) {
        bdlb::Print::indent(stream, level, spacesPerLevel);
        stream << "[\n";
    }
    else {
        // No newlines if 0 > spacesPerLevel.
        stream << '[';
    }

    if (level < 0) {
        level = -level;
    }

    int levelPlus1 = level + 1;

    if (0 <= spacesPerLevel) {
        bdlb::Print::indent(stream, levelPlus1, spacesPerLevel);
    }
    else {
        stream << ' ';
    }
    stream << d_transmissionCause;

    if (0 <= spacesPerLevel) {
        stream << '\n';
        bdlb::Print::indent(stream, levelPlus1, spacesPerLevel);
    }
    else {
        stream << ' ';
    }
    stream << d_recordIndex;

    if (0 <= spacesPerLevel) {
        stream << '\n';
        bdlb::Print::indent(stream, levelPlus1, spacesPerLevel);
    }
    else {
        stream << ' ';
    }
    stream << d_sequenceLength;

    if (0 <= spacesPerLevel) {
        stream << '\n';
        bdlb::Print::indent(stream, level, spacesPerLevel);
        stream << "]\n";
    }
    else {
        stream << " ]";
    }

    return stream << bsl::flush;
}
}  // close package namespace

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2007
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
