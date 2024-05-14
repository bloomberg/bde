// bdlm_metricdescriptor.h                                            -*-C++-*-
#ifndef INCLUDED_BDLM_METRICDESCRIPTOR
#define INCLUDED_BDLM_METRICDESCRIPTOR

#include <bsls_ident.h>
BSLS_IDENT("$Id$")

//@PURPOSE: Provide an attribute class to describe a metric.
//
//@DEPRECATED: This component is not ready for public use.
//
//@CLASSES:
//  bdlm::MetricDescriptor: descriptive information for a metric
//
//@DESCRIPTION: This component provides a single unconstrained attribute class,
// 'bdlm::MetricDescriptor', that is used to supply descriptive information for
// a metric.  A descriptor is supplied to a 'MetricsRegistry' to identify a
// metric, which in-turn supplies that description to a higher-level metric
// publication system (like BALM or GUTS) via a concrete implementation of the
// 'bdlm::MetricsAdapter' protocol.
//
///Attributes
///----------
//..
//  Name                   Type         Default
//  ------------------     -----------  -------
//  metricNamespace        bsl::string  [1]
//  metricName             bsl::string  ""     
//  instanceNumber         Uint64       0      
//  objectTypeName         bsl::string  ""
//  objectTypeAbbreviation bsl::string  ""
//  objectIdentifier       bsl::string  [2]
//  
// [1] k_USE_METRICS_ADAPTER_NAMESPACE_SELECTION
// [2] k_USE_METRICS_ADAPTER_OBJECT_ID_SELECTION
//..
//: o 'metricNamespace' - a grouping value for published metrics, which is
//:   expected to be have implementation specific details from the metric
//:   publishing system.  Recommended practice is to use
//:   'k_USE_METRICS_ADAPTER_NAMESPACE_SELECTION' indicating concrete
//:   implementations of 'MetricsAdapter' should determine an appropriate
//:   value for the metrics framework it adapts (this is often a configured
//:   value for an application, e.g., a service name).
//:
//: o 'metricName' - the name of the metric (e.g., "requestCount")
//:
//: o 'instanceCount' - an instance number expected to ensure that a unique
//:   'MetricsDescriptor' is created for each instance of an object reporting
//:   metrics.  Recommended practice is to use 'bdlm::InstanceCount' to
//:   generate this value.
//:
//: o 'objectTypeName' - a name that uniquely identifies the type generating
//:   the metric (e.g., "bdlmt.fixedthreadpool").
//:
//: o 'objectTypeAbbreviation' - a shortened, but still unique, version of the
//:   object type name (e.g., "ftp").
//:
//: o 'objectIdentifier' - an application unique value for identifying a
//:   metric.  Recommended practice is to use
//:   'k_USE_METRICS_ADAPTER_OBJECT_ID_SELECTION' indicating concrete
//:   implementations of 'MetricsAdapter' should determine an appropriate value
//:   for the metrics framework it adapts (this is often a value computed from
//:   the other descriptor properties).
//..
// For example, for GUTS (an internal Bloomberg metric framework) using a
// 'guta::BdlmMetricsAdapter' configured with the namespace "bdlm" and object
// identifier prefix "svc", supplying (metricNamespace:
// k_USE_METRICS_ADAPTER_NAMESPACE_SELECTION, metricName: "backlog",
// instanceNumber: InstanceCount::value<FixedThreadPool>(), objectTypeName:
// "bdlmt.fixedthreadpool", objectTypeAbbreviation: "ftp",
// objectIdentifier: '_USE_METRICS_ADAPTER_OBJECT_ID_SELECTION') would result
// in a metric that is within the namespace "bdlm", had the metric name
// "backlog.g", and the tags
// {"identifier":"svc.ftp.1","type":"bdlmt.fixedthreadpool"}, assuming this
// was the first invocation of 'InstanceCount::value<FixedThreadPool>()'.
// Note that the ".g" in "backlock.g" indicates a guage metric type.
//
///Usage
///-----
// The intended use of this component is illustrated in
// {'bdlm_metricsadapter'|Usage}.

#include <bdlm_instancecount.h>

#include <bslma_allocator.h>
#include <bslma_usesbslmaallocator.h>

#include <bslmf_nestedtraitdeclaration.h>

#include <bsl_string.h>

namespace BloombergLP {
namespace bdlm {

                          // ======================
                          // class MetricDescriptor
                          // ======================

class MetricDescriptor {
    // This attribute class characterizes how to describe a metric.

  private:
    // DATA
    bsl::string          d_metricNamespace;         // namespace for the metric

    bsl::string          d_metricName;              // name of the metric

    InstanceCount::Value d_instanceNumber;          // instance number of the
                                                    // type generating the
                                                    // metric

    bsl::string          d_objectTypeName;          // name of the type
                                                    // generating the metric

    bsl::string          d_objectTypeAbbreviation;  // abbreviation for the
                                                    // type generating the
                                                    // metric

    bsl::string          d_objectIdentifier;        // object identifier
                                                    // generating the metric

    // FRIENDS
    friend bool operator==(const MetricDescriptor&, const MetricDescriptor&);
    friend bool operator!=(const MetricDescriptor&, const MetricDescriptor&);

  public:
    // CONSTANTS
    static const char *k_USE_METRICS_ADAPTER_NAMESPACE_SELECTION;
    static const char *k_USE_METRICS_ADAPTER_OBJECT_ID_SELECTION;

    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(MetricDescriptor,
                                   bslma::UsesBslmaAllocator);

    // CREATORS
    MetricDescriptor(bslma::Allocator *basicAllocator = 0);
        // Create an object of this class having the default attribute values.
        // Optionally specify a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.  The default value for the namespace attribute is
        // 'k_USE_METRICS_ADAPTER_NAMESPACE_SELECTION' and the default value
        // for the object identifier attribute is
        // 'k_USE_METRICS_ADAPTER_OBJECT_ID_SELECTION'.  The residual attibutes
        // default to an empty string, except the instance number which
        // defaults to 0.  Note that implementations of the
        // 'bdlm::MetricsAdapter' protocal will typically use an implementation
        // specific value for the namespace attribute when
        // 'metricNamespace == k_USE_METRICS_ADAPTER_NAMESPACE_SELECTION' and
        // the object identifier attribute when
        // 'objectIdentrifier == k_USE_METRICS_ADAPTER_OBJECT_ID_SELECTION'.

    MetricDescriptor(const bsl::string_view&  metricNamespace,
                     const bsl::string_view&  metricName,
                     InstanceCount::Value     instanceNumber,
                     const bsl::string_view&  objectTypeName,
                     const bsl::string_view&  objectTypeAbbreviation,
                     const bsl::string_view&  objectIdentifier,
                     bslma::Allocator        *basicAllocator = 0);
        // Create an object of this class having the specified
        // 'metricNamespace', 'metricName', 'instanceNumber', 'objectTypeName',
        // 'objectTypeAbbreviation', and 'objectIdentifier' attribute values.
        // Optionally specify a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.  Note that implementations of the 'bdlm::MetricsAdapter'
        // protocal will typically use an implementation specific value for the
        // namespace attribute when
        // 'metricNamespace == k_USE_METRICS_ADAPTER_NAMESPACE_SELECTION' and
        // the object identifier attribute when
        // 'objectIdentrifier == k_USE_METRICS_ADAPTER_OBJECT_ID_SELECTION'.

    MetricDescriptor(const MetricDescriptor&  original,
                     bslma::Allocator        *basicAllocator = 0);
        // Create an object of this class having the same value as the
        // specified 'original' one.  Optionally specify a 'basicAllocator'
        // used to supply memory.  If 'basicAllocator' is 0, the currently
        // installed default allocator is used.

    //! ~MetricDescriptor() = default;
        // Destroy this object.

    // MANIPULATORS
    MetricDescriptor& operator=(const MetricDescriptor& rhs);
        // Assign to this object the value of the specified 'rhs' object, and
        // return a reference providing modifiable access to this object.

    void setInstanceNumber(const InstanceCount::Value& value);
        // Set the 'instanceNumber' attribute of this object to the specified
        // 'value'.

    void setMetricName(const bsl::string_view& value);
        // Set the 'metricName' attribute of this object to the specified
        // 'value'.

    void setMetricNamespace(const bsl::string_view& value);
        // Set the 'metricNamespace' attribute of this object to the specified
        // 'value'.

    void setObjectIdentifier(const bsl::string_view& value);
        // Set the 'objectIdentifier' attribute of this object to the specified
        // 'value'.

    void setObjectTypeAbbreviation(const bsl::string_view& value);
        // Set the 'objectTypeAbbreviation' attribute of this object to the
        // specified 'value'.

    void setObjectTypeName(const bsl::string_view& value);
        // Set the 'objectTypeName' attribute of this object to the specified
        // 'value'.

    // ACCESSORS
    const InstanceCount::Value& instanceNumber() const;
        // Return the 'instanceNumber' attribute.

    const bsl::string& metricName() const;
        // Return the 'metricName' attribute.

    const bsl::string& metricNamespace() const;
        // Return the 'metricNamespace' attribute.

    const bsl::string& objectIdentifier() const;
        // Return the 'objectIdentifier' attribute.

    const bsl::string& objectTypeAbbreviation() const;
        // Return the 'objectTypeAbbreviation' attribute.

    const bsl::string& objectTypeName() const;
        // Return the 'objectTypeName' attribute.

    bslma::Allocator *allocator() const;
        // Return the allocator used by this object to supply memory.
};

// FREE OPERATORS
bool operator==(const MetricDescriptor& lhs, const MetricDescriptor& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects have the same
    // value, and 'false' otherwise.  Two 'MetricDescriptor' objects have the
    // same value if each of their attributes (respectively) have the same
    // value.

bool operator!=(const MetricDescriptor& lhs, const MetricDescriptor& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects do not have the
    // same value, and 'false' otherwise.  Two 'MetricDescriptor' objects do
    // not have the same value if any of their attributes (respectively) do not
    // have the same value.

// ============================================================================
//                             INLINE DEFINITIONS
// ============================================================================

                          // ----------------------
                          // class MetricDescriptor
                          // ----------------------

// CREATORS
inline
MetricDescriptor::MetricDescriptor(bslma::Allocator *basicAllocator)
: d_metricNamespace(k_USE_METRICS_ADAPTER_NAMESPACE_SELECTION, basicAllocator)
, d_metricName(basicAllocator)
, d_instanceNumber(0)
, d_objectTypeName(basicAllocator)
, d_objectTypeAbbreviation(basicAllocator)
, d_objectIdentifier(k_USE_METRICS_ADAPTER_OBJECT_ID_SELECTION, basicAllocator)
{
}

inline
MetricDescriptor::MetricDescriptor(
                       const bsl::string_view&  metricNamespace,
                       const bsl::string_view&  metricName,
                       InstanceCount::Value     instanceNumber,
                       const bsl::string_view&  objectTypeName,
                       const bsl::string_view&  objectTypeAbbreviation,
                       const bsl::string_view&  objectIdentifier,
                       bslma::Allocator        *basicAllocator)
: d_metricNamespace(metricNamespace, basicAllocator)
, d_metricName(metricName, basicAllocator)
, d_instanceNumber(instanceNumber)
, d_objectTypeName(objectTypeName, basicAllocator)
, d_objectTypeAbbreviation(objectTypeAbbreviation, basicAllocator)
, d_objectIdentifier(objectIdentifier, basicAllocator)
{
}

inline
MetricDescriptor::MetricDescriptor(const MetricDescriptor&  original,
                                   bslma::Allocator        *basicAllocator)
: d_metricNamespace(original.d_metricNamespace, basicAllocator)
, d_metricName(original.d_metricName, basicAllocator)
, d_instanceNumber(original.d_instanceNumber)
, d_objectTypeName(original.d_objectTypeName, basicAllocator)
, d_objectTypeAbbreviation(original.d_objectTypeAbbreviation, basicAllocator)
, d_objectIdentifier(original.d_objectIdentifier, basicAllocator)
{
}

// MANIPULATORS
inline
MetricDescriptor& MetricDescriptor::operator=(const MetricDescriptor& rhs)
{
    d_metricNamespace        = rhs.d_metricNamespace;
    d_metricName             = rhs.d_metricName;
    d_instanceNumber         = rhs.d_instanceNumber;
    d_objectTypeName         = rhs.d_objectTypeName;
    d_objectTypeAbbreviation = rhs.d_objectTypeAbbreviation;
    d_objectIdentifier       = rhs.d_objectIdentifier;

    return *this;
}

inline
void MetricDescriptor::setInstanceNumber(const InstanceCount::Value& value)
{
    d_instanceNumber = value;
}

inline
void MetricDescriptor::setMetricName(const bsl::string_view& value)
{
    d_metricName = value;
}

inline
void MetricDescriptor::setMetricNamespace(const bsl::string_view& value)
{
    d_metricNamespace = value;
}

inline
void MetricDescriptor::setObjectIdentifier(const bsl::string_view& value)
{
    d_objectIdentifier = value;
}

inline
void MetricDescriptor::setObjectTypeAbbreviation(const bsl::string_view& value)
{
    d_objectTypeAbbreviation = value;
}

inline
void MetricDescriptor::setObjectTypeName(const bsl::string_view& value)
{
    d_objectTypeName = value;
}

// ACCESSORS
inline
const InstanceCount::Value& MetricDescriptor::instanceNumber() const
{
    return d_instanceNumber;
}

inline
const bsl::string& MetricDescriptor::metricName() const
{
    return d_metricName;
}

inline
const bsl::string& MetricDescriptor::metricNamespace() const
{
    return d_metricNamespace;
}

inline
const bsl::string& MetricDescriptor::objectIdentifier() const
{
    return d_objectIdentifier;
}

inline
const bsl::string& MetricDescriptor::objectTypeAbbreviation() const
{
    return d_objectTypeAbbreviation;
}

inline
const bsl::string& MetricDescriptor::objectTypeName() const
{
    return d_objectTypeName;
}

                                  // Aspects

inline
bslma::Allocator *MetricDescriptor::allocator() const
{
    return d_metricNamespace.get_allocator().mechanism();
}

// FREE OPERATORS
inline
bool operator==(const MetricDescriptor& lhs, const MetricDescriptor& rhs)
{
    return lhs.d_metricNamespace        == rhs.d_metricNamespace        &&
           lhs.d_metricName             == rhs.d_metricName             &&
           lhs.d_instanceNumber         == rhs.d_instanceNumber         &&
           lhs.d_objectTypeName         == rhs.d_objectTypeName         &&
           lhs.d_objectTypeAbbreviation == rhs.d_objectTypeAbbreviation &&
           lhs.d_objectIdentifier       == rhs.d_objectIdentifier;
}

inline
bool operator!=(const MetricDescriptor& lhs, const MetricDescriptor& rhs)
{
    return lhs.d_metricNamespace        != rhs.d_metricNamespace        ||
           lhs.d_metricName             != rhs.d_metricName             ||
           lhs.d_instanceNumber         != rhs.d_instanceNumber         ||
           lhs.d_objectTypeName         != rhs.d_objectTypeName         ||
           lhs.d_objectTypeAbbreviation != rhs.d_objectTypeAbbreviation ||
           lhs.d_objectIdentifier       != rhs.d_objectIdentifier;
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
