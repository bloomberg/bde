// balm_publicationtype.h   -*-C++-*-   GENERATED FILE -- DO NOT EDIT

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BALM_PUBLICATIONTYPE
#define INCLUDED_BALM_PUBLICATIONTYPE

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT_RCSID(balm_publicationtype_h,"$Id$ $CSID$ $CCId$")
BSLS_IDENT_PRAGMA_ONCE

//@PURPOSE: Provide an enumeration of aggregate types used to publish metrics.
//
//@CLASSES:
//   balm::PublicationType: a namespace to enumerate publication types.
//
//@SEE_ALSO: balm_publisher
//
//@DESCRIPTION: This component defines an enumeration
// 'balm::PublicationType::Value' of aggregation types to use to publish
// metrics.  Concrete 'balm::Publisher' implementations may use this these
// types to configure their output.

#ifndef INCLUDED_BALSCM_VERSION
#include <balscm_version.h>
#endif

#ifndef INCLUDED_BDLAT_TYPETRAITS
#include <bdlat_typetraits.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#endif

#ifndef INCLUDED_BSL_OSTREAM
#include <bsl_ostream.h>
#endif

#ifndef INCLUDED_BSL_STRING
#include <bsl_string.h>
#endif

namespace BloombergLP {

namespace balm {
                           // =====================
                           // class PublicationType
                           // =====================

struct PublicationType {
    // This class defines an enumeration of aggregation type that can be used
    // to describe the published output of a metric.  Concrete 'Publisher'
    // implementations may use this these types to configure their output.

  public:
    // TYPES
    enum Value {
        e_UNSPECIFIED = 0
            // There is no defined publication type for the metric.
      , e_TOTAL       = 1
            // The total of the measured metric values over the published
            // interval.
      , e_COUNT       = 2
            // The count of updates over the published interval.
      , e_MIN         = 3
            // The minimum measured metric value over the published interval.
      , e_MAX         = 4
            // The maximum measured metric value over the published interval.
      , e_AVG         = 5
            // The average measured metric value over published interval (i.e.,
            // total / count).
      , e_RATE        = 6
            // The total measured metric value per second over the published
            // interval (i.e., total / sample interval).
      , e_RATE_COUNT  = 7
            // The count of measured events per second over the published
            // interval (i.e., count / sample interval).

    };

    enum {
        k_LENGTH = 8
    };

    // CLASS METHODS
    static const char *toString(Value value);
        // Return the string representation exactly matching the enumerator
        // name corresponding to the specified enumeration 'value'.

    static int fromString(Value        *result,
                          const char   *string,
                          int           stringLength);
        // Load into the specified 'result' the enumerator matching the
        // specified 'string' of the specified 'stringLength'.  Return 0 on
        // success, and a non-zero value with no effect on 'result' otherwise
        // (i.e., 'string' does not match any enumerator).

    static int fromString(Value              *result,
                          const bsl::string&  string);
        // Load into the specified 'result' the enumerator matching the
        // specified 'string'.  Return 0 on success, and a non-zero value with
        // no effect on 'result' otherwise (i.e., 'string' does not match any
        // enumerator).

    static int fromInt(Value *result, int number);
        // Load into the specified 'result' the enumerator matching the
        // specified 'number'.  Return 0 on success, and a non-zero value with
        // no effect on 'result' otherwise (i.e., 'number' does not match any
        // enumerator).

    static bsl::ostream& print(bsl::ostream& stream, Value value);
        // Write to the specified 'stream' the string representation of the
        // specified enumeration 'value'.  Return a reference to the modifiable
        // 'stream'.
};

// FREE OPERATORS
bsl::ostream& operator<<(bsl::ostream&          stream,
                         PublicationType::Value rhs);
    // Format the specified 'rhs' to the specified output 'stream' and return a
    // reference to the modifiable 'stream'.

// TRAITS
}  // close package namespace

BDLAT_DECL_ENUMERATION_TRAITS(balm::PublicationType)

namespace balm {

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

                           // ---------------------
                           // class PublicationType
                           // ---------------------

// CLASS METHODS
inline
int PublicationType::fromString(Value *result, const bsl::string& string)
{
    return fromString(result,
                      string.c_str(),
                      static_cast<int>(string.length()));
}

inline
bsl::ostream& PublicationType::print(bsl::ostream&          stream,
                                     PublicationType::Value value)
{
    return stream << toString(value);
}

}  // close package namespace

// FREE FUNCTIONS

inline
bsl::ostream& balm::operator<<(bsl::ostream&          stream,
                               PublicationType::Value rhs)
{
    return PublicationType::print(stream, rhs);
}

}  // close enterprise namespace
#endif

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
