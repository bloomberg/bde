// baejsn_decoderoptions.cpp                                          -*-C++-*-

#include <bdes_ident.h>
BDES_IDENT_RCSID(baejsn_decoderoptions_cpp,"$Id$ $CSID$")

#include <baejsn_decoderoptions.h>

#include <bdeu_print.h>
#include <bdeu_printmethods.h>

#include <bsls_assert.h>

namespace BloombergLP {

                      // ---------------------------
                      // class baejsn_DecoderOptions
                      // ---------------------------

// CREATORS
baejsn_DecoderOptions::baejsn_DecoderOptions()
: d_maxDepth(MAX_DEPTH)
, d_skipUnknownElements(false)
{
}

baejsn_DecoderOptions::baejsn_DecoderOptions(
                                         const baejsn_DecoderOptions& original)
: d_maxDepth(original.d_maxDepth)
, d_skipUnknownElements(original.d_skipUnknownElements)
{
}

baejsn_DecoderOptions::~baejsn_DecoderOptions()
{
}

// MANIPULATORS
baejsn_DecoderOptions&
baejsn_DecoderOptions::operator=(const baejsn_DecoderOptions& rhs)
{
    d_maxDepth            = rhs.d_maxDepth;
    d_skipUnknownElements = rhs.d_skipUnknownElements;
    return *this;
}

void baejsn_DecoderOptions::reset()
{
    d_maxDepth            = MAX_DEPTH;
    d_skipUnknownElements = false;
}

// ACCESSORS
bsl::ostream& baejsn_DecoderOptions::print(bsl::ostream& stream,
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
