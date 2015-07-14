// balm_metricsample.cpp      -*-C++-*-
#include <balm_metricsample.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(balm_metricsample_cpp,"$Id$ $CSID$")

#include <bdlb_print.h>

#include <bsl_ostream.h>

namespace BloombergLP {

namespace balm {
                        // ----------------------------
                        // class MetricSampleGroup
                        // ----------------------------

// ACCESSORS
bsl::ostream&
MetricSampleGroup::print(bsl::ostream& stream,
                              int           level,
                              int           spacesPerLevel) const
{
    const char *NL = (spacesPerLevel > 0) ? "\n" : " ";

    bdlb::Print::indent(stream, level, spacesPerLevel);
    stream << "[ " << d_elapsedTime << NL;

    bdlb::Print::indent(stream, level + 1, spacesPerLevel);
    stream << "[" << NL;

    const_iterator it = begin();
    for (; it != end(); ++it) {
        bdlb::Print::indent(stream, level + 2, spacesPerLevel);
        stream << *it << NL;
    }

    bdlb::Print::indent(stream, level + 1, spacesPerLevel);
    stream << "]" << NL;
    bdlb::Print::indent(stream, level, spacesPerLevel);
    stream << "]" << NL;
    return stream;
}

                       // -----------------------
                       // class MetricSample
                       // -----------------------

// CREATORS
MetricSample::MetricSample(const MetricSample&  original,
                                     bslma::Allocator         *basicAllocator)
: d_timeStamp(original.d_timeStamp)
, d_records(original.d_records, basicAllocator)
, d_numRecords(original.d_numRecords)
{
}

// MANIPULATORS
MetricSample& MetricSample::operator=(const MetricSample& rhs)
{
    if (this != &rhs) {
        d_records     = rhs.d_records;
        d_timeStamp   = rhs.d_timeStamp;
        d_numRecords  = rhs.d_numRecords;
    }
    return *this;
}

// ACCESSORS
bsl::ostream& MetricSample::print(bsl::ostream& stream,
                                       int           level,
                                       int           spacesPerLevel) const
{
    const char *NL = (spacesPerLevel > 0) ? "\n" : " ";

    bdlb::Print::indent(stream, level, spacesPerLevel);
    stream << "[ " << d_timeStamp << NL;

    bdlb::Print::indent(stream, level + 1, spacesPerLevel);
    stream << "[" << NL;

    const_iterator it = begin();
    for (; it != end(); ++it) {
        it->print(stream, level + 2, spacesPerLevel);
    }

    bdlb::Print::indent(stream, level + 1, spacesPerLevel);
    stream << "]" << NL;
    bdlb::Print::indent(stream, level, spacesPerLevel);
    stream << "]" << NL;
    return stream;
}
}  // close package namespace

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2009
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
