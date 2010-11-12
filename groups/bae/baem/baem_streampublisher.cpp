// baem_streampublisher.cpp -*-C++-*-
#include <baem_streampublisher.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(baem_streampublisher_cpp,"$Id$ $CSID$")

#include <baem_metricformat.h>
#include <baem_metricrecord.h>
#include <baem_metricsample.h>

#include <bsls_assert.h>

#include <bsl_ostream.h>

namespace BloombergLP {

namespace {

const char *publicationTypeString(baem_PublicationType::Value value)
{
    switch (value) {
      case baem_PublicationType::BAEM_UNSPECIFIED: {
        BSLS_ASSERT(false && "Invalid publication type");
      } break;
      case baem_PublicationType::BAEM_TOTAL: {
        return "total";
      }
      case baem_PublicationType::BAEM_COUNT: {
        return "count";
      }
      case baem_PublicationType::BAEM_MIN: {
        return "min";
      }
      case baem_PublicationType::BAEM_MAX: {
        return "max";
      }
      case baem_PublicationType::BAEM_AVG: {
        return "avg (total/count)";
      }
      case baem_PublicationType::BAEM_RATE: {
        return "rate (total/elapsedTime)";
      }
      case baem_PublicationType::BAEM_RATE_COUNT: {
        return "rate (count/elapsedTime)";
      }
    }
    return "Undefined";
}

void formatValue(bsl::ostream&                stream,
                 int                          value,
                 const baem_MetricFormatSpec *formatSpec)
{
    if (0 == formatSpec) {
        stream << value;
    }
    else {
        baem_MetricFormatSpec::formatValue(stream, (double)value, *formatSpec);
    }
}

void formatValue(bsl::ostream&                stream,
                 double                       value,
                 const baem_MetricFormatSpec *formatSpec)
{
    if (0 == formatSpec) {
        stream << value;
    }
    else {
        baem_MetricFormatSpec::formatValue(stream, value, *formatSpec);
    }
}

void formatValue(bsl::ostream&                stream,
                 const baem_MetricRecord&     record,
                 double                       elapsedTime,
                 baem_PublicationType::Value  publicationType,
                 const baem_MetricFormatSpec *formatSpec)
{
    switch (publicationType) {
      case baem_PublicationType::BAEM_UNSPECIFIED: {
        BSLS_ASSERT(false && "Invalid publication type");
      } break;
      case baem_PublicationType::BAEM_TOTAL: {
        formatValue(stream, record.total(), formatSpec);
      } break;
      case baem_PublicationType::BAEM_COUNT: {
        formatValue(stream, record.count(), formatSpec);
      } break;
      case baem_PublicationType::BAEM_MIN: {
        formatValue(stream, record.min(), formatSpec);
      } break;
      case baem_PublicationType::BAEM_MAX: {
        formatValue(stream, record.max(), formatSpec);
      } break;
      case baem_PublicationType::BAEM_AVG: {
        formatValue(stream, record.total() / record.count(), formatSpec);
      } break;
      case baem_PublicationType::BAEM_RATE: {
        formatValue(stream, record.total() / elapsedTime, formatSpec);
      } break;
      case baem_PublicationType::BAEM_RATE_COUNT: {
        formatValue(stream, record.count() / elapsedTime, formatSpec);
      } break;
    }
}

void publishRecord(bsl::ostream&            stream,
                   const baem_MetricRecord& record,
                   double                   elapsedTime)
    // Publish, to the specified 'stream', the specified 'record'.
{
    baem_PublicationType::Value publicationType =
                   record.metricId().description()->preferredPublicationType();
    bcema_SharedPtr<const baem_MetricFormat> format =
                                   record.metricId().description()->format();

    stream << "\t\t" << record.metricId() << "[ ";

    if (publicationType != baem_PublicationType::BAEM_UNSPECIFIED) {
        stream << publicationTypeString(publicationType) << " = ";
        const baem_MetricFormatSpec *formatSpec =
                              format ? format->formatSpec(publicationType) : 0;

        formatValue(stream, record, elapsedTime, publicationType, formatSpec);
    }
    else {
        const baem_MetricFormatSpec *countSpec = 0;
        const baem_MetricFormatSpec *totalSpec = 0;
        const baem_MetricFormatSpec *minSpec   = 0;
        const baem_MetricFormatSpec *maxSpec   = 0;

        if (format) {
            countSpec = format->formatSpec(baem_PublicationType::BAEM_COUNT);
            totalSpec = format->formatSpec(baem_PublicationType::BAEM_TOTAL);
            minSpec   = format->formatSpec(baem_PublicationType::BAEM_MIN);
            maxSpec   = format->formatSpec(baem_PublicationType::BAEM_MAX);
        }
        stream << "count = ";
        formatValue(stream, record.count(), countSpec);
        stream << ", total = ";
        formatValue(stream, record.total(), totalSpec);
        if (baem_MetricRecord::DEFAULT_MIN == record.min()) {
            stream << ", min = undefined";
        }
        else {
            stream << ", min = ";
            formatValue(stream, record.min(), minSpec);
        }
        if (baem_MetricRecord::DEFAULT_MAX == record.max()) {
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

                      // --------------------------
                      // class baem_StreamPublisher
                      // --------------------------

// CREATORS
baem_StreamPublisher::~baem_StreamPublisher()
{
}

// MANIPULATORS
void baem_StreamPublisher::publish(const baem_MetricSample& metricValues)
{
    if (metricValues.numRecords() > 0) {
        d_stream << metricValues.timeStamp() << " "
                 << metricValues.numRecords() << " Records" << bsl::endl;

        baem_MetricSample::const_iterator gIt = metricValues.begin();
        baem_MetricSample::const_iterator prev;
        for (; gIt != metricValues.end(); ++gIt) {
            const double elapsedTime =
                                     gIt->elapsedTime().totalSecondsAsDouble();

            if (gIt == metricValues.begin()
             || gIt->elapsedTime() != prev->elapsedTime()) {
                d_stream << "\tElapsed Time: " << elapsedTime << "s"
                         << bsl::endl;
            }
            baem_MetricSampleGroup::const_iterator rIt = gIt->begin();
            for (; rIt != gIt->end(); ++rIt) {
                publishRecord(d_stream, *rIt, elapsedTime);
            }
            prev = gIt;
        }
    }
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
