// baea_metric.cpp   -*-C++-*-
#include <baea_metric.h>

#include <bslma_default.h>
#include <bsls_assert.h>
#include <bdet_datetime.h>
#include <bdet_datetimeinterval.h>
#include <bdetu_datetimeinterval.h>
#include <bdetu_epoch.h>
#include <bdeu_print.h>

#include <bsl_ostream.h>
#include <bsl_iomanip.h>

namespace BloombergLP {

                          // -----------------
                          // class baea_Metric
                          // -----------------

// CREATORS

baea_Metric::baea_Metric(bslma_Allocator *basicAllocator)
: d_metricName(basicAllocator)
, d_category(basicAllocator)
, d_reportingName(basicAllocator)
, d_updateInterval(0.0)
, d_updateTimestamp(0.0)
, d_value(0.0)
, d_metricCb(basicAllocator)
{
}

baea_Metric::baea_Metric(
        const baea_Metric&  original,
        bslma_Allocator    *basicAllocator)
: d_metricName(original.d_metricName, basicAllocator)
, d_category(original.d_category, basicAllocator)
, d_reportingName(original.d_reportingName, basicAllocator)
, d_updateInterval(original.d_updateInterval)
, d_updateTimestamp(original.d_updateTimestamp)
, d_value(original.d_value)
, d_metricCb(basicAllocator)
{
}

baea_Metric::~baea_Metric()
{
}

// MANIPULATORS

baea_Metric& baea_Metric::operator=(const baea_Metric& original)
{
    d_metricName = original.d_metricName;
    d_category = original.d_category;
    d_reportingName = original.d_reportingName;
    d_updateInterval = original.d_updateInterval;
    d_updateTimestamp = original.d_updateTimestamp;
    d_value = original.d_value;
    return *this;
}

void baea_Metric::setCategory(const char *category)
{
    BSLS_ASSERT(category);
    d_category = category;
}

void baea_Metric::setMetricCb(const MetricCb& metricCb)
{
    d_metricCb = metricCb;
}

void baea_Metric::setMetricName(const char *metricName)
{
    BSLS_ASSERT(metricName);
    d_metricName = metricName;
}

void baea_Metric::setReportingName(const char *name)
{
    BSLS_ASSERT(name);
    d_reportingName = name;
}

void baea_Metric::setUpdateInterval(const bdet_TimeInterval& interval)
{
    d_updateInterval = interval;
}

void baea_Metric::setUpdateTimestamp(const bdet_TimeInterval& timestamp)
{
    d_updateTimestamp = timestamp;
}

void baea_Metric::setValue(double value)
{
    d_value = value;
}

// ACCESSORS

const bsl::string& baea_Metric::category() const
{
    return d_category;
}

const baea_Metric::MetricCb& baea_Metric::metricCb() const
{
    return d_metricCb;
}

const bsl::string& baea_Metric::metricName() const
{
    return d_metricName;
}

const bsl::string& baea_Metric::reportingName() const
{
    return d_reportingName;
}

const bdet_TimeInterval& baea_Metric::updateInterval() const
{
    return d_updateInterval;
}

const bdet_TimeInterval& baea_Metric::updateTimestamp() const
{
    return d_updateTimestamp;
}

double baea_Metric::value() const
{
    return d_value;
}

bsl::ostream& baea_Metric::prettyPrint(
        bsl::ostream& stream,
        int           level,
        int           spacesPerLevel) const
{
    if (level < 0) {
        level = -level;
    }
    else {
        bdeu_Print::indent(stream, level, spacesPerLevel);
    }

    int levelPlus1 = level + 1;

    bdet_DatetimeInterval timestampInterval;
    bdetu_DatetimeInterval::convertToDatetimeInterval(&timestampInterval,
                                                      d_updateTimestamp);
    bdet_Datetime timestampDate = bdetu_Epoch::epoch();
    timestampDate += timestampInterval;

    if (0 <= spacesPerLevel) {
        // multiline

        stream << "[\n";

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "MetricName = ";
        bdeu_PrintMethods::print(stream, d_metricName,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Category = ";
        bdeu_PrintMethods::print(stream, d_category,
                                 -levelPlus1, spacesPerLevel);

        if (!d_reportingName.empty()) {
            bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
            stream << "ReportingName = ";
            bdeu_PrintMethods::print(stream, d_reportingName,
                                     -levelPlus1, spacesPerLevel);
        }

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "UpdateInterval = ";
        stream << d_updateInterval.seconds() << " seconds" << bsl::endl;

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "UpdateTimestamp = ";
        bdeu_PrintMethods::print(stream, timestampDate,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Value = ";
        bdeu_PrintMethods::print(stream, d_value,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, level, spacesPerLevel);
        stream << "\n]\n";
    }
    else {
        // single line

        stream << '[';

        stream << ' ';
        stream << "MetricName = ";
        bdeu_PrintMethods::print(stream, d_metricName,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "Category = ";
        bdeu_PrintMethods::print(stream, d_category,
                                 -levelPlus1, spacesPerLevel);

        if (!d_reportingName.empty()) {
            stream << ' ';
            stream << "ReportingName = ";
            bdeu_PrintMethods::print(stream, d_reportingName,
                                     -levelPlus1, spacesPerLevel);
        }

        stream << ' ';
        stream << "UpdateInterval = ";
        stream << d_updateInterval.seconds() << " seconds ";

        stream << ' ';
        stream << "UpdateTimestamp = ";
        bdeu_PrintMethods::print(stream, timestampDate,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "Value = ";
        bdeu_PrintMethods::print(stream, d_value,
                                 -levelPlus1, spacesPerLevel);

        stream << " ]";
    }

    return stream << bsl::flush;
}

bsl::ostream& baea_Metric::print(
        bsl::ostream& stream,
        int           level,
        int           spacesPerLevel) const
{
    if (level < 0) {
        level = -level;
    }
    else {
        bdeu_Print::indent(stream, level, spacesPerLevel);
    }

    int levelPlus1 = level + 1;

    if (0 <= spacesPerLevel) {
        // multiline

        stream << "[\n";

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "MetricName = ";
        bdeu_PrintMethods::print(stream, d_metricName,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Category = ";
        bdeu_PrintMethods::print(stream, d_category,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "ReportingName = ";
        bdeu_PrintMethods::print(stream, d_reportingName,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "UpdateInterval = ";
        bdeu_PrintMethods::print(stream, d_updateInterval,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "UpdateTimestamp = ";
        bdeu_PrintMethods::print(stream, d_updateTimestamp,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Value = ";
        bdeu_PrintMethods::print(stream, d_value,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, level, spacesPerLevel);
        stream << "]\n";
    }
    else {
        // single line

        stream << '[';

        stream << ' ';
        stream << "MetricName = ";
        bdeu_PrintMethods::print(stream, d_metricName,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "Category = ";
        bdeu_PrintMethods::print(stream, d_category,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "ReportingName = ";
        bdeu_PrintMethods::print(stream, d_reportingName,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "UpdateInterval = ";
        bdeu_PrintMethods::print(stream, d_updateInterval,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "UpdateTimestamp = ";
        bdeu_PrintMethods::print(stream, d_updateTimestamp,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "Value = ";
        bdeu_PrintMethods::print(stream, d_value,
                                 -levelPlus1, spacesPerLevel);

        stream << " ]";
    }

    return stream << bsl::flush;
}

bsl::ostream& baea_Metric::rowPrint(bsl::ostream& stream) const
{
    enum { NAMEWIDTH=40, INTERVALWIDTH=3, VALUEWIDTH=16 };
    bdet_DatetimeInterval timestampInterval;
    bdetu_DatetimeInterval::convertToDatetimeInterval(&timestampInterval,
                                                      d_updateTimestamp);
    bdet_Datetime timestampDate = bdetu_Epoch::epoch();
    timestampDate += timestampInterval;
    return stream << bsl::setw(NAMEWIDTH)       << d_metricName
                  << bsl::setw(VALUEWIDTH)      << d_value
                  << bsl::setw(INTERVALWIDTH)   << "("
                  << d_updateInterval.seconds() << "s )";
}

                         // -------------------------
                         // class baea_MetricReporter
                         // -------------------------

// CREATORS

baea_MetricReporter::~baea_MetricReporter()
{
}

// FREE OPERATORS

bool operator==(const baea_Metric& lhs,
                const baea_Metric& rhs)
{
    return lhs.d_metricName      == rhs.d_metricName
        && lhs.d_category        == rhs.d_category
        && lhs.d_reportingName   == rhs.d_reportingName
        && lhs.d_updateInterval  == rhs.d_updateInterval
        && lhs.d_updateTimestamp == rhs.d_updateTimestamp
        && lhs.d_value           == rhs.d_value;
}

bool operator!=(const baea_Metric& lhs,
                const baea_Metric& rhs)
{
    return lhs.d_metricName      != rhs.d_metricName
        || lhs.d_category        != rhs.d_category
        || lhs.d_reportingName   != rhs.d_reportingName
        || lhs.d_updateInterval  != rhs.d_updateInterval
        || lhs.d_updateTimestamp != rhs.d_updateTimestamp
        || lhs.d_value           != rhs.d_value;
}

}  // close namespace BloombergLP

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2007, 2008
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ------------------------------- END-OF-FILE --------------------------------
