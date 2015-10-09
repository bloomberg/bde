// balm_metricformat.cpp                                              -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

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

                        // ----------------------------
                        // class balm::MetricFormatSpec
                        // ----------------------------

// PRIVATE CONSTANTS
const char *balm::MetricFormatSpec::k_DEFAULT_FORMAT = "%f";

namespace balm {

// CLASS METHODS
bsl::ostream& MetricFormatSpec::formatValue(bsl::ostream&           stream,
                                            double                  value,
                                            const MetricFormatSpec& format)
{
    enum { e_INITIAL_BUFFER_SIZE = 32 };

    char buffer[e_INITIAL_BUFFER_SIZE];
    int rc = snprintf(buffer, e_INITIAL_BUFFER_SIZE, format.format(),
                                                       value * format.scale());
    if (rc < 0) {
        stream << "Invalid format " << format << " applied to " << value
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
    rc = snprintf(newBuffer.data(), newBuffer.size(), format.format(),
                                                       value * format.scale());
    if ((unsigned)rc >= newBuffer.size()) {
        stream << "Invalid format " << format << " applied to " << value
               << bsl::flush;
        return stream;                                                // RETURN
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

                             // ------------------
                             // class MetricFormat
                             // ------------------

// MANIPULATORS
void MetricFormat::clearFormatSpec(
                                   PublicationType::Value publicationType)
{
    d_formatSpecs[publicationType].reset();

    // If there are no non-null format specs, then resize 'd_formatSpecs' to be
    // an empty vector.

    bsl::vector<AggregateFormatSpec>::const_iterator it =
                                                         d_formatSpecs.begin();
    for (; it != d_formatSpecs.end(); ++it) {
        if (!it->isNull()) {
            return;                                                   // RETURN
        }
    }
    d_formatSpecs.clear();
}

// ACCESSORS
bsl::ostream& MetricFormat::print(bsl::ostream& stream,
                                       int      level,
                                       int      spacesPerLevel) const
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
}  // close enterprise namespace

// ----------------------------------------------------------------------------
// Copyright 2015 Bloomberg Finance L.P.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// ----------------------------- END-OF-FILE ----------------------------------
