// baejsn_encoderoptions.cpp                                          -*-C++-*-

#include <bdes_ident.h>
BDES_IDENT_RCSID(baejsn_encoderoptions_cpp,"$Id$ $CSID$")

#include <baejsn_encoderoptions.h>

#include <bdeu_print.h>
#include <bdeu_printmethods.h>
#include <bdeu_string.h>

#include <bsls_assert.h>

namespace BloombergLP {

                      // ---------------------------
                      // class baejsn_EncoderOptions
                      // ---------------------------

// CREATORS
baejsn_EncoderOptions::baejsn_EncoderOptions()
: d_initialIndentLevel(0)
, d_spacesPerLevel(0)
, d_encodingStyle(BAEJSN_COMPACT)
{
}

baejsn_EncoderOptions::baejsn_EncoderOptions(
                                         const baejsn_EncoderOptions& original)
: d_initialIndentLevel(original.d_initialIndentLevel)
, d_spacesPerLevel(original.d_spacesPerLevel)
, d_encodingStyle(original.d_encodingStyle)
{
}

baejsn_EncoderOptions::~baejsn_EncoderOptions()
{
}

// MANIPULATORS
baejsn_EncoderOptions&
baejsn_EncoderOptions::operator=(const baejsn_EncoderOptions& rhs)
{
    d_initialIndentLevel = rhs.d_initialIndentLevel;
    d_spacesPerLevel     = rhs.d_spacesPerLevel;
    d_encodingStyle      = rhs.d_encodingStyle;
    return *this;
}

void baejsn_EncoderOptions::reset()
{
    d_initialIndentLevel = 0;
    d_spacesPerLevel     = 0;
    d_encodingStyle      = BAEJSN_COMPACT;
}

// ACCESSORS
bsl::ostream& baejsn_EncoderOptions::print(bsl::ostream& stream,
                                           int           level,
                                           int           spacesPerLevel) const
{
    // TBD:

    return stream << bsl::flush;
}

}  // close namespace BloombergLP

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2012
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ------------------------------ END-OF-FILE ---------------------------------
