// bael_context.cpp                                                   -*-C++-*-
#include <bael_context.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bael_context_cpp,"$Id$ $CSID$")

#include <bdeu_print.h>
#include <bsl_ostream.h>

namespace BloombergLP {

                        // ------------------
                        // class bael_Context
                        // ------------------

// CLASS METHODS
bool bael_Context::isValid(bael_Transmission::Cause transmissionCause,
                           int                      recordIndex,
                           int                      sequenceLength)
{
    bool status;

    switch (transmissionCause) {
      case bael_Transmission::BAEL_PASSTHROUGH: {
        status = 0 == recordIndex && 1 == sequenceLength;
      } break;
      case bael_Transmission::BAEL_TRIGGER:
      case bael_Transmission::BAEL_TRIGGER_ALL:
      case bael_Transmission::BAEL_MANUAL_PUBLISH:
      case bael_Transmission::BAEL_MANUAL_PUBLISH_ALL: {
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
bsl::ostream& bael_Context::print(bsl::ostream& stream,
                                  int           level,
                                  int           spacesPerLevel) const
{
    if (0 <= spacesPerLevel) {
        bdeu_Print::indent(stream, level, spacesPerLevel);
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
        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
    }
    else {
        stream << ' ';
    }
    stream << d_transmissionCause;

    if (0 <= spacesPerLevel) {
        stream << '\n';
        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
    }
    else {
        stream << ' ';
    }
    stream << d_recordIndex;

    if (0 <= spacesPerLevel) {
        stream << '\n';
        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
    }
    else {
        stream << ' ';
    }
    stream << d_sequenceLength;

    if (0 <= spacesPerLevel) {
        stream << '\n';
        bdeu_Print::indent(stream, level, spacesPerLevel);
        stream << "]\n";
    }
    else {
        stream << " ]";
    }

    return stream << bsl::flush;
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
