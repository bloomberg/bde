// baem_metricformat.cpp      -*-C++-*-
#include <baem_metricformat.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(baem_metricformat_cpp,"$Id$ $CSID$")

#include <bdeu_print.h>

#include <bsl_limits.h>
#include <bsl_ostream.h>

#include <bsl_c_stdio.h>   // for 'snprintf'

#if defined(BSLS_PLATFORM__CMP_MSVC)
#define snprintf _snprintf
#endif

namespace BloombergLP {

                      // ---------------------------
                      // class baem_MetricFormatSpec
                      // ---------------------------

// PRIVATE CONSTANTS
const char *baem_MetricFormatSpec::DEFAULT_FORMAT = "%f";

// CLASS METHODS
bsl::ostream& baem_MetricFormatSpec::formatValue(
                                       bsl::ostream&                stream,
                                       double                       value,
                                       const baem_MetricFormatSpec& formatSpec)
{
    enum { INITIAL_BUFFER_SIZE = 32 };

    char buffer[INITIAL_BUFFER_SIZE];
    int rc = snprintf(buffer,
                      INITIAL_BUFFER_SIZE,
                      formatSpec.format(),
                      value * formatSpec.scale());
    if (rc < 0) {
        stream << "Invalid format " << formatSpec << " applied to " << value
               << bsl::flush;
        return stream;                                                // RETURN
    }

    if (rc < INITIAL_BUFFER_SIZE) {
        stream << buffer << bsl::flush;
        return stream;                                                // RETURN
    }
    // The string is too big to fit in the buffer (that's a long floating-
    // point value!).  Note that if 'rc == INITIAL_BUFFER_SIZE', then there
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
bsl::ostream& baem_MetricFormatSpec::print(bsl::ostream& stream) const
{
    stream << "[ scale = " << d_scale << " format = \"" << d_format << "\" ]";
    return stream;
}

                          // -----------------------
                          // class baem_MetricFormat
                          // -----------------------

// MANIPULATORS
void baem_MetricFormat::clearFormatSpec(
                                   baem_PublicationType::Value publicationType)
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
bsl::ostream& baem_MetricFormat::print(bsl::ostream& stream,
                                       int           level,
                                       int           spacesPerLevel) const
{
    const char *NL = (spacesPerLevel > 0) ? "\n" : " ";

    bdeu_Print::indent(stream, level, spacesPerLevel);
    stream << "[";

    for (int i = 0; i < (int)d_formatSpecs.size(); ++i) {
        if (0 == i) {
            stream << NL;
        }

        bdeu_Print::indent(stream, level + 1, spacesPerLevel);
        stream << (baem_PublicationType::Value)i << " = "
               << d_formatSpecs[i] << NL;
    }

    bdeu_Print::indent(stream, level, spacesPerLevel);
    stream << " ]" << NL;
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
