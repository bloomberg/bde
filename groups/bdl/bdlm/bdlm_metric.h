// bdlm_metric.h                                                      -*-C++-*-
#ifndef INCLUDED_BDLM_METRIC
#define INCLUDED_BDLM_METRIC

#include <bsls_ident.h>
BSLS_IDENT("$Id$")

//@PURPOSE: Provide a class to store metric values of different types.
//
//@CLASSES:
//  bdlm::Metric: variant of metric values
//
//@DESCRIPTION: This component defines a value semantic class, `bdlmt::Metric`,
// that represents a measurement (or measurements) for a metric.
// A `bdlm::Metric` value is collected from an object that reports metrics
// using a callback that is registered with the `bdlm::MetricsRegistry`.  A
// `bdlm::Metric` value is a variant type, which can hold one of several types
// of measurements (in theory).  Currently the only supported metric type is a
// `Guage`, which represents a single instantaneous value for the most recent
// measurement.
//
///Usage
///-----
// The intended use of this component is illustrated in
// {`bdlm_metricsregistrar`|Usage}.

#include <bdlb_variant.h>

#include <bsls_assert.h>

namespace BloombergLP {
namespace bdlm {

/// This class provides storage for various types of metric values.
class Metric {

  public:
    // PUBLIC TYPES
    typedef double Gauge;

  private:
    // DATA
    bdlb::Variant<Gauge> d_value;  // value of the metric

    // FRIENDS
    friend bool operator==(const Metric& lhs, const Metric& rhs);
    friend bool operator!=(const Metric& lhs, const Metric& rhs);

  public:
    // CREATORS

    /// Create a default (empty) metric.
    Metric();

    /// Create a metric with the specified `value`.
    explicit Metric(const Gauge& value);

    //! ~Metric() = default;
        // Destroy this 'Metric' object.

    // MANIPULATORS

    /// Assign to this metric the specified `value`.
    Metric& operator=(const Gauge& value);

    // ACCESSORS

    /// Return `true` if the value of this metric is of gauge type, and
    /// `false` otherwise.
    bool isGauge() const;

    /// Return a `const` reference to the value of this metric of gauge
    /// type.  The behavior is undefined unless `true == isGuage()`.
    const Gauge& theGauge() const;
};

// FREE OPERATORS

/// Return `true` if the specified `lhs` and `rhs` have the same value, and
/// `false` otherwise.  Two `Metric` objects have the same value if their
/// types are the same and they have the same value.
bool operator==(const Metric& lhs, const Metric& rhs);

/// Return `true` if the specified `lhs` command-line metric has a different
/// value from the specified `rhs` command-line metric, and `false`
/// otherwise.  Two `Metric` objects do not have the same value if their
/// types are not the same or they have different values.
bool operator!=(const Metric& lhs, const Metric& rhs);

// ============================================================================
//                             INLINE DEFINITIONS
// ============================================================================

                               // ------------
                               // class Metric
                               // ------------

// CREATORS
inline
Metric::Metric()
: d_value()
{
}

inline
Metric::Metric(const Gauge& value)
: d_value(value)
{
}

// MANIPULATORS
inline
Metric& Metric::operator=(const Gauge& value)
{
    d_value = value;
    return *this;
}

// ACCESSORS
inline
bool Metric::isGauge() const
{
    return d_value.is<Gauge>();
}

inline
const Metric::Gauge& Metric::theGauge() const
{
    BSLS_ASSERT(isGauge());

    return d_value.the<Gauge>();
}

// FREE OPERATORS
inline
bool operator==(const Metric& lhs, const Metric& rhs)
{
    return lhs.d_value == rhs.d_value;
}

inline
bool operator!=(const Metric& lhs, const Metric& rhs)
{
    return lhs.d_value != rhs.d_value;
}

}  // close package namespace
}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2024 Bloomberg Finance L.P.
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
