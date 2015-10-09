// balm_streampublisher.cpp                                           -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <balm_streampublisher.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(balm_streampublisher_cpp,"$Id$ $CSID$")

#include <bdlt_datetimetz.h>

#include <balm_metricformat.h>
#include <balm_metricrecord.h>
#include <balm_metricsample.h>
#include <balm_metricid.h>

#include <bsls_timeinterval.h>
#include <bsls_assert.h>

#include <bsl_ostream.h>

namespace BloombergLP {

namespace {

const char *publicationTypeString(balm::PublicationType::Value value)
{
    switch (value) {
      case balm::PublicationType::e_UNSPECIFIED: {
        BSLS_ASSERT(false && "Invalid publication type");
      } break;
      case balm::PublicationType::e_TOTAL: {
        return "total";                                               // RETURN
      }
      case balm::PublicationType::e_COUNT: {
        return "count";                                               // RETURN
      }
      case balm::PublicationType::e_MIN: {
        return "min";                                                 // RETURN
      }
      case balm::PublicationType::e_MAX: {
        return "max";                                                 // RETURN
      }
      case balm::PublicationType::e_AVG: {
        return "avg (total/count)";                                   // RETURN
      }
      case balm::PublicationType::e_RATE: {
        return "rate (total/elapsedTime)";                            // RETURN
      }
      case balm::PublicationType::e_RATE_COUNT: {
        return "rate (count/elapsedTime)";                            // RETURN
      }
    }
    return "Undefined";
}

void formatValue(bsl::ostream&                 stream,
                 int                           value,
                 const balm::MetricFormatSpec *formatSpec)
{
    if (0 == formatSpec) {
        stream << value;
    }
    else {
        balm::MetricFormatSpec::formatValue(
                                           stream, (double)value, *formatSpec);
    }
}

void formatValue(bsl::ostream&                 stream,
                 double                        value,
                 const balm::MetricFormatSpec *formatSpec)
{
    if (0 == formatSpec) {
        stream << value;
    }
    else {
        balm::MetricFormatSpec::formatValue(stream, value, *formatSpec);
    }
}

void formatValue(bsl::ostream&                 stream,
                 const balm::MetricRecord&     record,
                 double                        elapsedTime,
                 balm::PublicationType::Value  publicationType,
                 const balm::MetricFormatSpec *formatSpec)
{
    switch (publicationType) {
      case balm::PublicationType::e_UNSPECIFIED: {
        BSLS_ASSERT(false && "Invalid publication type");
      } break;
      case balm::PublicationType::e_TOTAL: {
        formatValue(stream, record.total(), formatSpec);
      } break;
      case balm::PublicationType::e_COUNT: {
        formatValue(stream, record.count(), formatSpec);
      } break;
      case balm::PublicationType::e_MIN: {
        formatValue(stream, record.min(), formatSpec);
      } break;
      case balm::PublicationType::e_MAX: {
        formatValue(stream, record.max(), formatSpec);
      } break;
      case balm::PublicationType::e_AVG: {
        formatValue(stream, record.total() / record.count(), formatSpec);
      } break;
      case balm::PublicationType::e_RATE: {
        formatValue(stream, record.total() / elapsedTime, formatSpec);
      } break;
      case balm::PublicationType::e_RATE_COUNT: {
        formatValue(stream, record.count() / elapsedTime, formatSpec);
      } break;
    }
}

void publishRecord(bsl::ostream&             stream,
                   const balm::MetricRecord& record,
                   double                    elapsedTime)
    // Publish, to the specified 'stream', the specified 'record'.
{
    balm::PublicationType::Value publicationType =
                   record.metricId().description()->preferredPublicationType();
    bsl::shared_ptr<const balm::MetricFormat> format =
                                   record.metricId().description()->format();

    stream << "\t\t" << record.metricId() << "[ ";

    if (publicationType != balm::PublicationType::e_UNSPECIFIED) {
        stream << publicationTypeString(publicationType) << " = ";
        const balm::MetricFormatSpec *formatSpec =
                              format ? format->formatSpec(publicationType) : 0;

        formatValue(stream, record, elapsedTime, publicationType, formatSpec);
    }
    else {
        const balm::MetricFormatSpec *countSpec = 0;
        const balm::MetricFormatSpec *totalSpec = 0;
        const balm::MetricFormatSpec *minSpec   = 0;
        const balm::MetricFormatSpec *maxSpec   = 0;

        if (format) {
           countSpec = format->formatSpec(balm::PublicationType::e_COUNT);
           totalSpec = format->formatSpec(balm::PublicationType::e_TOTAL);
           minSpec   = format->formatSpec(balm::PublicationType::e_MIN);
           maxSpec   = format->formatSpec(balm::PublicationType::e_MAX);
        }
        stream << "count = ";
        formatValue(stream, record.count(), countSpec);
        stream << ", total = ";
        formatValue(stream, record.total(), totalSpec);
        if (balm::MetricRecord::k_DEFAULT_MIN == record.min()) {
            stream << ", min = undefined";
        }
        else {
            stream << ", min = ";
            formatValue(stream, record.min(), minSpec);
        }
        if (balm::MetricRecord::k_DEFAULT_MAX == record.max()) {
            stream << ", max = undefined";
        }
        else {
            stream << ", max = ";
            formatValue(stream, record.max(), maxSpec);
        }
    }

    stream << " ]\n";
}

}  // close unnamed namespace

namespace balm {
                           // ---------------------
                           // class StreamPublisher
                           // ---------------------

// CREATORS
StreamPublisher::~StreamPublisher()
{
}

// MANIPULATORS
void StreamPublisher::publish(const MetricSample& metricValues)
{
    if (metricValues.numRecords() > 0) {
        d_stream << metricValues.timeStamp() << " "
                 << metricValues.numRecords() << " Records" << bsl::endl;

        MetricSample::const_iterator gIt = metricValues.begin();
        MetricSample::const_iterator prev;
        for (; gIt != metricValues.end(); ++gIt) {
            const double elapsedTime =
                                     gIt->elapsedTime().totalSecondsAsDouble();

            if (gIt == metricValues.begin()
             || gIt->elapsedTime() != prev->elapsedTime()) {
                d_stream << "\tElapsed Time: " << elapsedTime << "s"
                         << bsl::endl;
            }
            MetricSampleGroup::const_iterator rIt = gIt->begin();
            for (; rIt != gIt->end(); ++rIt) {
                publishRecord(d_stream, *rIt, elapsedTime);
            }
            prev = gIt;
        }
    }
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
