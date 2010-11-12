// baem_metricsample.cpp      -*-C++-*-
#include <baem_metricsample.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(baem_metricsample_cpp,"$Id$ $CSID$")

#include <bdeu_print.h>

#include <bsl_ostream.h>

namespace BloombergLP {

                        // ----------------------------
                        // class baem_MetricSampleGroup
                        // ----------------------------

// ACCESSORS
bsl::ostream&
baem_MetricSampleGroup::print(bsl::ostream& stream,
                              int           level,
                              int           spacesPerLevel) const
{
    const char *NL = (spacesPerLevel > 0) ? "\n" : " ";

    bdeu_Print::indent(stream, level, spacesPerLevel);
    stream << "[ " << d_elapsedTime << NL;

    bdeu_Print::indent(stream, level + 1, spacesPerLevel);
    stream << "[" << NL;

    const_iterator it = begin();
    for (; it != end(); ++it) {
        bdeu_Print::indent(stream, level + 2, spacesPerLevel);
        stream << *it << NL;
    }

    bdeu_Print::indent(stream, level + 1, spacesPerLevel);
    stream << "]" << NL;
    bdeu_Print::indent(stream, level, spacesPerLevel);
    stream << "]" << NL;
    return stream;
}

                       // -----------------------
                       // class baem_MetricSample
                       // -----------------------

// CREATORS
baem_MetricSample::baem_MetricSample(const baem_MetricSample&  original,
                                     bslma_Allocator          *basicAllocator)
: d_timeStamp(original.d_timeStamp)
, d_records(original.d_records, basicAllocator)
, d_numRecords(original.d_numRecords)
{
}

// MANIPULATORS
baem_MetricSample& baem_MetricSample::operator=(const baem_MetricSample& rhs)
{
    if (this != &rhs) {
        d_records     = rhs.d_records;
        d_timeStamp   = rhs.d_timeStamp;
        d_numRecords  = rhs.d_numRecords;
    }
    return *this;
}

// ACCESSORS
bsl::ostream& baem_MetricSample::print(bsl::ostream& stream,
                                       int           level,
                                       int           spacesPerLevel) const
{
    const char *NL = (spacesPerLevel > 0) ? "\n" : " ";

    bdeu_Print::indent(stream, level, spacesPerLevel);
    stream << "[ " << d_timeStamp << NL;

    bdeu_Print::indent(stream, level + 1, spacesPerLevel);
    stream << "[" << NL;

    const_iterator it = begin();
    for (; it != end(); ++it) {
        it->print(stream, level + 2, spacesPerLevel);
    }

    bdeu_Print::indent(stream, level + 1, spacesPerLevel);
    stream << "]" << NL;
    bdeu_Print::indent(stream, level, spacesPerLevel);
    stream << "]" << NL;
    return stream;
}

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2009
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
