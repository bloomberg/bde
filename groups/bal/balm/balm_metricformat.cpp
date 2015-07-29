// balm_metricformat.cpp      -*-C++-*-
#include <balm_metricformat.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(balm_metricformat_cpp,"$Id$ $CSID$")

#include <bdlb_print.h>

#include <bsl_limits.h>
#include <bsl_ostream.h>

#include <bsl_c_stdio.h>   // for 'snprintf'

#if defined(BSLS_PLATFORM_CMP_MSVC)
#define snprintf _snprintf
#endif

namespace BloombergLP {

                      // ---------------------------
                      // class balm::MetricFormatSpec
                      // ---------------------------

// PRIVATE CONSTANTS
const char *balm::MetricFormatSpec::DEFAULT_FORMAT = "%f";

namespace balm {
// CLASS METHODS
bsl::ostream& MetricFormatSpec::formatValue(
                                       bsl::ostream&                stream,
                                       double                       value,
                                       const MetricFormatSpec& formatSpec)
{
    enum { e_INITIAL_BUFFER_SIZE = 32 };

    char buffer[e_INITIAL_BUFFER_SIZE];
    int rc = snprintf(buffer,
                      e_INITIAL_BUFFER_SIZE,
                      formatSpec.format(),
                      value * formatSpec.scale());
    if (rc < 0) {
        stream << "Invalid format " << formatSpec << " applied to " << value
               << bsl::flush;
        return stream;                                                // RETURN
    }

    if (rc < e_INITIAL_BUFFER_SIZE) {
        stream << buffer << bsl::flush;
        return stream;                                                // RETURN
    }
    // The string is too big to fit in the buffer (that's a long floating-
    // point value!).  Note that if 'rc == e_INITIAL_BUFFER_SIZE', then there
    // was not room for the null-terminator.

    bsl::vector<char> newBuffer;
    newBuffer.resize(rc + 1);
    rc = snprintf(newBuffer.data(),
                  newBuffer.size(),
                  formatSpec.format(),
                  value * formatSpec.scale());
    if ((unsigned)rc >= newBuffer.size()) {
        stream << "Invalid format " << formatSpec << " applied to " << value
               << bsl::flush;
        return stream;
    }
    stream << newBuffer.data() << bsl::flush;
    return stream;
}

// ACCESSORS
bsl::ostream& MetricFormatSpec::print(bsl::ostream& stream) const
{
    stream << "[ scale = " << d_scale << " format = \"" << d_format << "\" ]";
    return stream;
}

                          // -----------------------
                          // class MetricFormat
                          // -----------------------

// MANIPULATORS
void MetricFormat::clearFormatSpec(
                                   PublicationType::Value publicationType)
{
    d_formatSpecs[publicationType].reset();

    // If there are no non-null format specs, then resize 'd_formatSpecs'
    // to be an empty vector.

    bsl::vector<AggregateFormatSpec>::const_iterator it =
                                                         d_formatSpecs.begin();
    for (; it != d_formatSpecs.end(); ++it) {
        if (!it->isNull()) {
            return;
        }
    }
    d_formatSpecs.clear();
}

// ACCESSORS
bsl::ostream& MetricFormat::print(bsl::ostream& stream,
                                       int           level,
                                       int           spacesPerLevel) const
{
    const char *NL = (spacesPerLevel > 0) ? "\n" : " ";

    bdlb::Print::indent(stream, level, spacesPerLevel);
    stream << "[";

    for (int i = 0; i < (int)d_formatSpecs.size(); ++i) {
        if (0 == i) {
            stream << NL;
        }

        bdlb::Print::indent(stream, level + 1, spacesPerLevel);
        stream << (PublicationType::Value)i << " = "
               << d_formatSpecs[i] << NL;
    }

    bdlb::Print::indent(stream, level, spacesPerLevel);
    stream << " ]" << NL;
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
