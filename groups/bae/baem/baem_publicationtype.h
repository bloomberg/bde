// baem_publicationtype.h   -*-C++-*-   GENERATED FILE -- DO NOT EDIT
#ifndef INCLUDED_BAEM_PUBLICATIONTYPE
#define INCLUDED_BAEM_PUBLICATIONTYPE

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT_RCSID(baem_publicationtype_h,"$Id$ $CSID$ $CCId$")
BDES_IDENT_PRAGMA_ONCE

//@PURPOSE: Provide an enumeration of aggregate types used to publish metrics.
//
//@CLASSES:
//   baem_PublicationType: a namespace for an enumeration of publication types.
//
//@SEE_ALSO: baem_publisher
//
//@AUTHOR: Henry Verschell (hverschell@bloomberg.net)
//
//@DESCRIPTION: This component defines an enumeration of aggregation
// types that metrics may be published using.  Concrete 'baem_Publisher'
// implementations may use this these types to configure their output.

#ifndef INCLUDED_BAESCM_VERSION
#include <baescm_version.h>
#endif

#ifndef INCLUDED_BDEAT_TYPETRAITS
#include <bdeat_typetraits.h>
#endif

#ifndef INCLUDED_BDEX_INSTREAMFUNCTIONS
#include <bdex_instreamfunctions.h>
#endif

#ifndef INCLUDED_BDEX_OUTSTREAMFUNCTIONS
#include <bdex_outstreamfunctions.h>
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

namespace BloombergLP {

                           // ==========================
                           // class baem_PublicationType
                           // ==========================

struct baem_PublicationType {
    // This class defines an enumeration of aggregation type that can be used
    // to describe the published output of a metric.  Concrete 'baem_Publisher'
    // implementations may use this these types to configure their output.

  public:
    // TYPES
    enum Value {
        BAEM_UNSPECIFIED = 0
            // There is no defined publication type for the metric.
      , BAEM_TOTAL       = 1
            // The total of the measured metric values over the published
            // interval.
      , BAEM_COUNT       = 2
            // The count of updates over the published interval.
      , BAEM_MIN         = 3
            // The minimum measured metric value over the published interval.
      , BAEM_MAX         = 4
            // The maximum measured metric value over the published interval.
      , BAEM_AVG         = 5
            // The average measured metric value over published interval (i.e.,
            // total / count).
      , BAEM_RATE        = 6
            // The total measured metric value per second over the published
            // interval (i.e., total / sample interval).
      , BAEM_RATE_COUNT  = 7
            // The count of measured events per second over the published
            // interval (i.e., count / sample interval).
    };

    enum {
        BAEM_LENGTH = 8
    };

    // CLASS METHODS
    static int maxSupportedBdexVersion();
        // Return the most current 'bdex' streaming version number supported by
        // this class.  See the 'bdex' package-level documentation for more
        // information on 'bdex' streaming of value-semantic types and
        // containers.

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

    template <class STREAM>
    static STREAM& bdexStreamIn(STREAM&  stream,
                                Value&   value,
                                int      version);
        // Assign to the specified 'value' the value read from the specified
        // input 'stream' using the specified 'version' format and return a
        // reference to the modifiable 'stream'.  If 'stream' is initially
        // invalid, this operation has no effect.  If 'stream' becomes invalid
        // during this operation, the 'value' is valid, but its value is
        // undefined.  If the specified 'version' is not supported, 'stream' is
        // marked invalid, but 'value' is unaltered.  Note that no version is
        // read from 'stream'.  (See the package-group-level documentation for
        // more information on 'bdex' streaming of container types.)

    static bsl::ostream& print(bsl::ostream& stream, Value value);
        // Write to the specified 'stream' the string representation of
        // the specified enumeration 'value'.  Return a reference to
        // the modifiable 'stream'.

    template <class STREAM>
    static STREAM& bdexStreamOut(STREAM&  stream,
                                 Value    value,
                                 int      version);
        // Write the specified 'value' to the specified output 'stream' and
        // return a reference to the modifiable 'stream'.  Optionally specify
        // an explicit 'version' format; by default, the maximum supported
        // version is written to 'stream' and used as the format.  If 'version'
        // is specified, that format is used but *not* written to 'stream'.  If
        // 'version' is not supported, 'stream' is left unmodified.  (See the
        // package-group-level documentation for more information on 'bdex'
        // streaming of container types).
};

// FREE OPERATORS
inline
bsl::ostream& operator<<(bsl::ostream&               stream,
                         baem_PublicationType::Value rhs);
    // Format the specified 'rhs' to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.

// TRAITS
BDEAT_DECL_ENUMERATION_TRAITS(baem_PublicationType)

// ============================================================================
//                         INLINE FUNCTION DEFINITIONS
// ============================================================================

                           // --------------------------
                           // class baem_PublicationType
                           // --------------------------

// CLASS METHODS
inline
int baem_PublicationType::maxSupportedBdexVersion()
{
    return 1;  // versions start at 1
}

inline
int baem_PublicationType::fromString(Value *result, const bsl::string& string)
{
    return fromString(result, string.c_str(), string.length());
}

inline
bsl::ostream& baem_PublicationType::print(bsl::ostream&               stream,
                                          baem_PublicationType::Value value)
{
    return stream << toString(value);
}

template <class STREAM>
STREAM& baem_PublicationType::bdexStreamIn(
                                      STREAM&                      stream,
                                      baem_PublicationType::Value& value,
                                      int                          version)
{
    switch(version) {
      case 1: {
        int readValue;
        stream.getInt32(readValue);
        if (stream) {
            if (fromInt(&value, readValue)) {
               stream.invalidate();   // bad value in stream
            }
        }
      } break;
      default: {
        stream.invalidate();          // unrecognized version number
      } break;
    }
    return stream;
}

template <class STREAM>
STREAM& baem_PublicationType::bdexStreamOut(
                                           STREAM&                     stream,
                                           baem_PublicationType::Value value,
                                           int                         version)
{
    switch (version) {
      case 1: {
        stream.putInt32(value);  // Write the value as an int
      } break;
    }
    return stream;
}

// FREE FUNCTIONS

inline
bsl::ostream& operator<<(bsl::ostream&               stream,
                         baem_PublicationType::Value rhs)
{
    return baem_PublicationType::print(stream, rhs);
}

}  // close namespace BloombergLP
#endif

// GENERATED BY BLP_BAS_CODEGEN_3.3.x_DEV Tue Aug  4 13:07:24 2009
// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2009
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ------------------------------ END-OF-FILE ---------------------------------
