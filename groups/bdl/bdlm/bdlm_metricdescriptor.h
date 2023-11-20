// bdlm_metricdescriptor.h                                            -*-C++-*-
#ifndef INCLUDED_BDLM_METRICDESCRIPTOR
#define INCLUDED_BDLM_METRICDESCRIPTOR

#include <bsls_ident.h>
BSLS_IDENT("$Id$")

//@PURPOSE: Provide an attribute class to describe a metric.
//
//@CLASSES:
//  bdlm::MetricDescriptor: descriptive information for a metric
//
//@DESCRIPTION: This component provides a single attribute class,
// 'bdlm::MetricDescriptor', that is used to supply descriptive information for
// creating a metric.
//
///Usage
///-----
// The intended use of this component is illustrated in
// {'bdlm_metricsregistrar'|Usage}.

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
    bsl::string d_metricNamespace;   // namespace for the metric
    bsl::string d_metricName;        // name of the metric
    bsl::string d_objectTypeName;    // name of the type generating the metric
    bsl::string d_objectIdentifier;  // object identifier generating the metric

    // FRIENDS
    friend bool operator==(const MetricDescriptor&, const MetricDescriptor&);
    friend bool operator!=(const MetricDescriptor&, const MetricDescriptor&);

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(MetricDescriptor,
                                   bslma::UsesBslmaAllocator);

    // CREATORS
    MetricDescriptor(bslma::Allocator *basicAllocator = 0);
        // Create an object of this class having the default attribute values.
        // Optionally specify a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.

    MetricDescriptor(const bsl::string_view&  metricNamespace,
                     const bsl::string_view&  metricName,
                     const bsl::string_view&  objectTypeName,
                     const bsl::string_view&  objectIdentifier,
                     bslma::Allocator        *basicAllocator = 0);
        // Create an object of this class having the specified
        // 'metricNamespace', 'metricName', 'objectTypeName', and
        // 'objectIdentifier' attribute values.  Optionally specify a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.

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

    void setMetricNamespace(const bsl::string_view& vale);
        // Set the 'metricNamespace' attribute of this object to the specified
        // 'value'.

    void setMetricName(const bsl::string_view& vale);
        // Set the 'metricName' attribute of this object to the specified
        // 'value'.

    void setObjectTypeName(const bsl::string_view& vale);
        // Set the 'objectTypeName' attribute of this object to the specified
        // 'value'.

    void setObjectIdentifier(const bsl::string_view& vale);
        // Set the 'objectIdentifier' attribute of this object to the specified
        // 'value'.

    // ACCESSORS
    const bsl::string& metricNamespace() const;
        // Return the 'metricNamespace' attribute.

    const bsl::string& metricName() const;
        // Return the 'metricName' attribute.

    const bsl::string& objectTypeName() const;
        // Return the 'objectTypeName' attribute.

    const bsl::string& objectIdentifier() const;
        // Return the 'objectIdentifier' attribute.
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
: d_metricNamespace(basicAllocator)
, d_metricName(basicAllocator)
, d_objectTypeName(basicAllocator)
, d_objectIdentifier(basicAllocator)
{
}

inline
MetricDescriptor::MetricDescriptor(const bsl::string_view&  metricNamespace,
                                   const bsl::string_view&  metricName,
                                   const bsl::string_view&  objectTypeName,
                                   const bsl::string_view&  objectIdentifier,
                                   bslma::Allocator        *basicAllocator)
: d_metricNamespace(metricNamespace, basicAllocator)
, d_metricName(metricName, basicAllocator)
, d_objectTypeName(objectTypeName, basicAllocator)
, d_objectIdentifier(objectIdentifier, basicAllocator)
{
}

inline
MetricDescriptor::MetricDescriptor(const MetricDescriptor&  original,
                                   bslma::Allocator        *basicAllocator)
: d_metricNamespace(original.d_metricNamespace, basicAllocator)
, d_metricName(original.d_metricName, basicAllocator)
, d_objectTypeName(original.d_objectTypeName, basicAllocator)
, d_objectIdentifier(original.d_objectIdentifier, basicAllocator)
{
}

// MANIPULATORS
inline
MetricDescriptor& MetricDescriptor::operator=(const MetricDescriptor& rhs)
{
    d_metricNamespace  = rhs.d_metricNamespace;
    d_metricName       = rhs.d_metricName;
    d_objectTypeName   = rhs.d_objectTypeName;
    d_objectIdentifier = rhs.d_objectIdentifier;

    return *this;
}

inline
void MetricDescriptor::setMetricNamespace(const bsl::string_view& value)
{
    d_metricNamespace = value;
}

inline
void MetricDescriptor::setMetricName(const bsl::string_view& value)
{
    d_metricName = value;
}

inline
void MetricDescriptor::setObjectTypeName(const bsl::string_view& value)
{
    d_objectTypeName = value;
}

inline
void MetricDescriptor::setObjectIdentifier(const bsl::string_view& value)
{
    d_objectIdentifier = value;
}

// ACCESSORS
inline
const bsl::string& MetricDescriptor::metricNamespace() const
{
    return d_metricNamespace;
}

inline
const bsl::string& MetricDescriptor::metricName() const
{
    return d_metricName;
}

inline
const bsl::string& MetricDescriptor::objectTypeName() const
{
    return d_objectTypeName;
}

inline
const bsl::string& MetricDescriptor::objectIdentifier() const
{
    return d_objectIdentifier;
}

// FREE OPERATORS
inline
bool operator==(const MetricDescriptor& lhs, const MetricDescriptor& rhs)
{
    return lhs.d_metricNamespace  == rhs.d_metricNamespace &&
           lhs.d_metricName       == rhs.d_metricName      &&
           lhs.d_objectTypeName   == rhs.d_objectTypeName  &&
           lhs.d_objectIdentifier == rhs.d_objectIdentifier;
}

inline
bool operator!=(const MetricDescriptor& lhs, const MetricDescriptor& rhs)
{
    return lhs.d_metricNamespace  != rhs.d_metricNamespace ||
           lhs.d_metricName       != rhs.d_metricName      ||
           lhs.d_objectTypeName   != rhs.d_objectTypeName  ||
           lhs.d_objectIdentifier != rhs.d_objectIdentifier;
}

}  // close package namespace
}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2023 Bloomberg Finance L.P.
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
