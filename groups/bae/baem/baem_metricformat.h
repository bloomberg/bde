// baem_metricformat.h              -*-C++-*-
#ifndef INCLUDED_BAEM_METRICFORMAT
#define INCLUDED_BAEM_METRICFORMAT

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: baem_metricformat.h,v 1.8 2008/04/16 20:00:49 hversche Exp $")

//@PURPOSE: Provide a formatting specification for a metric.
//
//@CLASSES:
//      baem_MetricFormat: description for how to format a metric
//  baem_MetricFormatSpec: specification for formatting an individual aggregate
//
//@SEE_ALSO: baem_metricdescription
//
//@AUTHOR: Henry Verschell (hverschell)
//
//@DESCRIPTION: This component provides classes for describing the formatting
// for a metric.  For each published aggregate type (e.g., count, total, min,
// max, etc.), a 'baem_MetricFormat' object holds a 'baem_MetricFormatSpec'
// object describing how values of that aggregate may be formatted.
// 'baem_MetricFormat' provides the 'setFormatSpec' method to set the format
// specification for a particular publication type, and the 'formatSpec' method
// to retrieve the format specification for a publication type (or null if no
// format specification has been provided for the indicated publication type).
//
// 'beam_MetricFormatSpec' is an unconstrainted pure-attribute class that
// represents the specification for formatting a particular publication type of
// a metric (e.g., total, count, min, max, etc.).  The attributes held by
// 'baem_MetricFormatSpec' are given in the following table:
//..
//  Attribute       Type                  Description               Default
//  ---------   ------------   ----------------------------------   -------
//  scale       float          multiplier for scaling value         1.0
//  format      const char *   'printf'-style format for 'double'   "%f"
//..
// The string provided must be a 'printf'-style format valid for formatting a
// single 'double' value.
//
// Note that 'baem_Publisher' implementations determine how to use the
// format information associated with a metric (i.e., there is no guarantee
// that every publisher will format a metric using its 'baem_MetricFormat').
//
///Thread Safety
///-------------
// 'baem_MetricFormat' is *const* *thread-safe*, meaning that accessors
// may be invoked concurrently from different threads, but it is not safe to
// access or modify a 'baem_MetricFormat' in one thread while another
// thread modifies the same object.
//
// 'baem_MetricFormatSpec' is *const* *thread-safe*, meaning that accessors
// may be invoked concurrently from different threads, but it is not safe to
// access or modify a 'baem_MetricFormatSpec' in one thread while another
// thread modifies the same object.
//
///Usage
///-----
// The following example demonstrates how to create and configure a
// 'baem_MetricFormat'.  Note that clients of the 'baem' package can set the
// format for a metric through 'baem_configurationutil' or
// 'baem_metricregistry'.
//
// We start by creating a 'baem_MetricFormat' object:
//..
//  bslma_Allocator   *allocator = bslma_Default::allocator(0);
//  baem_MetricFormat  format(allocator);
//..
// Next we specify that average values should only be printed to two decimal
// places:
//..
//  format.setFormatSpec(baem_PublicationType::BAEM_AVG,
//                       baem_MetricFormatSpec(1.0, "%.2f"));
//..
// Next we specify that rate values should be formatted as a percentage --
// i.e., multiplied by 100, and then displayed with a "%" character:
//..
//  format.setFormatSpec(baem_PublicationType::BAEM_RATE,
//                       baem_MetricFormatSpec(100.0, "%.2f%%"));
//..
// We can verify that the correct format specifications have been set:
//..
//  assert(baem_MetricFormatSpec(1.0, "%.2f") ==
//         *format.formatSpec(baem_PublicationType::BAEM_AVG));
//  assert(baem_MetricFormatSpec(100.0, "%.2f%%") ==
//         *format.formatSpec(baem_PublicationType::BAEM_RATE));
//  assert(0 == format.formatSpec(baem_PublicationType::BAEM_TOTAL));
//..
// We can use the 'baem_MetricFormatSpec::formatValue' utility function to
// format the value 0.055 to the console.  Note however, that there is no
// guarantee that every implementation of 'baem_Publisher' will format metrics
// in this way.
//..
//  baem_MetricFormatSpec::formatValue(
//       bsl::cout, .055, *format.formatSpec(baem_PublicationType::BAEM_AVG));
//  bsl::cout << bsl::endl;
//  baem_MetricFormatSpec::formatValue(
//       bsl::cout, .055, *format.formatSpec(baem_PublicationType::BAEM_RATE));
//  bsl::cout << bsl::endl;
//..
// The resulting console output will be:
//..
//  0.06
//  5.50%
//..

#ifndef INCLUDED_BAESCM_VERSION
#include <baescm_version.h>
#endif

#ifndef INCLUDED_BAEM_PUBLICATIONTYPE
#include <baem_publicationtype.h>
#endif

#ifndef INCLUDED_BDEUT_NULLABLEVALUE
#include <bdeut_nullablevalue.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITS
#include <bslalg_typetraits.h>
#endif

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#endif

#ifndef INCLUDED_BSL_VECTOR
#include <bsl_vector.h>
#endif

#ifndef INCLUDED_BSL_CSTRING
#include <bsl_cstring.h>       // for 'bsl::strcmp'
#endif

namespace BloombergLP {

                      // ===========================
                      // class baem_MetricFormatSpec
                      // ===========================

class baem_MetricFormatSpec {
    // This class provides a value-semantic representation of the formatting
    // specification for a metric aggregate value.  The 'scale()' is a
    // multiplier used to scale the numeric value.  The 'format()' is a
    // 'printf'-style format string suitable for formatting a single
    // floating-point value.

    // DATA
    float       d_scale;   // multiplier for scaling published values

    const char *d_format;  // 'printf'-style format string for formatting a
                           // single floating-point numeric value

    // PRIVATE CONSTANTS
    static const char *DEFAULT_FORMAT;  // default format ("%f")

  public:
    // CLASS METHODS
    static bsl::ostream& formatValue(bsl::ostream&                stream,
                                     double                       value,
                                     const baem_MetricFormatSpec& format);
        // Write the specified 'value' to the specified 'stream' using the
        // specified 'format', and return a reference to the modifiable
        // 'stream'.

    // CREATORS
    baem_MetricFormatSpec();
        // Create a metric format spec having default values for 'scale' and
        // 'format'.  The default value for 'scale' is 1.0 and the default
        // value for 'format' is "%f".

    baem_MetricFormatSpec(float scale, const char *format);
        // Create a metric format spec having the specified 'scale' and
        // 'format'.  The 'scale' indicates the multiplier that may be used
        // when formatting values, and 'format' must be a 'printf'-style format
        // string for formatting a single floating-point value.  The behavior
        // is undefined unless 'format' is null-terminated, contains a
        // 'printf'-style format string valid for a single floating-point
        // value, and remains valid and unmodified for the lifetime of this
        // object.

    baem_MetricFormatSpec(const baem_MetricFormatSpec& original);
        // Create a metric format spec having the same value as the specified
        // 'original' format spec.  The behavior is undefined unless
        // 'original.format()' remains valid and unmodified for the lifetime of
        // this object.

    // ~baem_MetricFormatSpec();
        // Destroy this format spec.  Note that this trivial destructor is
        // generated by the compiler.

    // MANIPULATORS
    baem_MetricFormatSpec& operator=(const baem_MetricFormatSpec& rhs);
        // Assign to this format spec the value of the specified 'rhs' format
        // spec, and return a reference to this modifiable format spec.

    void setScale(float scale);
        // Set, to the specified 'scale', the scale multiplier that may be
        // applied when formatting values.

    void setFormat(const char *format);
        // Set, to the specified 'format', the 'printf'-style formatting string
        // that may be applied when formatting values.  The behavior is
        // undefined unless 'format' is null-terminated, contains a
        // 'printf'-style format string valid for a single floating-point
        // value, and remains valid and unmodified for the lifetime of this
        // object.

    // ACCESSORS
    float scale() const;
        // Return the floating-point multiplier value that may be applied to
        // scale formatted values.

    const char *format() const;
        // Return the address of the null-terminated string containing the
        // 'printf'-style format that may be used to format values.

    bsl::ostream& print(bsl::ostream& stream) const;
        // Write a description of this format spec to the specified 'stream',
        // and return a reference to the modifiable 'stream'.
};

// FREE OPERATORS
inline
bool operator==(const baem_MetricFormatSpec& lhs,
                const baem_MetricFormatSpec& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' metric format specs
    // have the same value, and 'false' otherwise.  Two format specs have the
    // same value if they have the same values for their 'scale' and 'format'
    // attributes, respectively.

inline
bool operator!=(const baem_MetricFormatSpec& lhs,
                const baem_MetricFormatSpec& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' metric format specs do
    // not have the same value, and 'false' otherwise.  Two format specs do
    // not have same value if they differ in their respective values for
    // 'scale' or 'format' attributes.

inline
bsl::ostream& operator<<(bsl::ostream&                stream,
                         const baem_MetricFormatSpec& rhs);
    // Write a formatted description of the specified 'rhs' format spec to the
    // specified 'stream', and return a reference to the modifiable 'stream'.

                      // =======================
                      // class baem_MetricFormat
                      // =======================

class baem_MetricFormat {
    // This class provides a value-semantic description for the formatting
    // of a metric.  For each published aggregate type of a metric (e.g.,
    // count, total, min, max, etc.), a 'baem_MetricFormat' contains a
    // 'baem_MetricFormatSpec' object describing how to format values
    // of that aggregate, or null if no formatting information is supplied.
    // 'baem_Metricformat' provides the 'setFormatSpec' method to set the
    // format spec for a publication type, and the 'formatSpec' method to
    // retrieve the format spec for a publication type (or 0 if no format spec
    // has been provided for the indicated publication type).  Note that the
    // types of published aggregates explicitly provided by the 'baem' package
    // are defined in the 'baem_PublicationType' enumeration.

    // TYPES
    typedef bdeut_NullableValue<baem_MetricFormatSpec> AggregateFormatSpec;

    // DATA
    bsl::vector<AggregateFormatSpec> d_formatSpecs;
                              // array of length 0, or of length
                              // 'baem_PublicationType::BAEM_LENGTH',
                              // holding a mapping of the publication type to
                              // the (possibly null) formatting options for
                              // that type

    // FRIENDS
    friend bool operator==(const baem_MetricFormat& lhs,
                           const baem_MetricFormat& rhs);

  public:
    // PUBLIC TRAITS
    BSLALG_DECLARE_NESTED_TRAITS(baem_MetricFormat,
                                 bslalg_TypeTraitUsesBslmaAllocator);

    // CREATORS
    baem_MetricFormat(bslma_Allocator *basicAllocator = 0);
        // Create an empty metric format object.  Optionally specify a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.  Note that
        // 'formatSpec' will return 0 for all supplied publication types.

    baem_MetricFormat(const baem_MetricFormat&  original,
                      bslma_Allocator          *basicAllocator = 0);
        // Create a metric format object having the same value as the specified
        // 'original' format.  Optionally specify a 'basicAllocator' used to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.

    // ~baem_MetricFormat();
        // Destroy this metric format object.  Note that this trivial
        // destructor is generated by the compiler.

    // MANIPULATORS
    baem_MetricFormat& operator=(const baem_MetricFormat& rhs);
        // Assign to this metric format object the value of the specified 'rhs'
        // metric format, and return a reference to this modifiable metric
        // format.

    void setFormatSpec(baem_PublicationType::Value  publicationType,
                       const baem_MetricFormatSpec& formatSpec);
        // Set the format spec for the metric aggregate indicated by the
        // specified 'publicationType' to the specified 'formatSpec'.

    void clearFormatSpecs();
        // Remove all format specs from this metric format object and put this
        // object into its default-constructed state.  After this method
        // returns, 'formatSpec' will return 0 for all supplied publication
        // types.

    void clearFormatSpec(baem_PublicationType::Value publicationType);
        // Remove the format spec for the specified 'publicationType' from this
        // metric format object.  After this methods returns,
        // 'formatSpec(publicationType)' will return 0.

    // ACCESSORS
    const baem_MetricFormatSpec *formatSpec(
                            baem_PublicationType::Value publicationType) const;
        // Return the address of the non-modifiable format spec for the
        // specified 'publicationType', or 0 if no format spec has been
        // provided for 'publicationType'.

    bsl::ostream& print(bsl::ostream& stream,
                        int           level = 0,
                        int           spacesPerLevel = 4) const;
        // Format this object to the specified output 'stream' at the (absolute
        // value of) the optionally specified indentation 'level' and return a
        // reference to 'stream'.  If 'level' is specified, optionally specify
        // 'spacesPerLevel', the number of spaces per indentation level for
        // this and all of its nested objects.  If 'level' is negative,
        // suppress indentation of the first line.  If 'spacesPerLevel' is
        // negative, format the entire output on one line, suppressing all but
        // the initial indentation (as governed by 'level').  If 'stream' is
        // not valid on entry, this operation has no effect.
};

// FREE OPERATORS
inline
bool operator==(const baem_MetricFormat& lhs, const baem_MetricFormat& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' metric formats have the
    // same value, and 'false' otherwise.  Two metric formats have the same
    // value if they have the same value for 'formatSpec' for each of the
    // enumerated publication types.

inline
bool operator!=(const baem_MetricFormat& lhs, const baem_MetricFormat& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' metric formats do not
    // have the same value, and 'false' otherwise.  Two metric formats do not
    // have same value if they differ in their 'formatSpec' for any of the
    // enumerated publication types.

inline
bsl::ostream& operator<<(bsl::ostream& stream, const baem_MetricFormat& rhs);
    // Write a formatted description of the specified 'rhs' metric format to
    // the specified 'stream', and return a reference to the modifiable
    // 'stream'.

// ===========================================================================
//                      INLINE FUNCTION DEFINITIONS
// ===========================================================================

                      // ---------------------------
                      // class baem_MetricFormatSpec
                      // ---------------------------

// CREATORS
inline
baem_MetricFormatSpec::baem_MetricFormatSpec()
: d_scale(1.0f)
, d_format(DEFAULT_FORMAT)
{
}

inline
baem_MetricFormatSpec::baem_MetricFormatSpec(float scale, const char *format)
: d_scale(scale)
, d_format(format)
{
}

inline
baem_MetricFormatSpec::baem_MetricFormatSpec(
                                         const baem_MetricFormatSpec& original)
: d_scale(original.d_scale)
, d_format(original.d_format)
{
}

// MANIPULATORS
inline
baem_MetricFormatSpec& baem_MetricFormatSpec::operator=(
                                              const baem_MetricFormatSpec& rhs)
{
    d_scale  = rhs.d_scale;
    d_format = rhs.d_format;
    return *this;
}

inline
void baem_MetricFormatSpec::setScale(float scale)
{
    d_scale = scale;
}

inline
void baem_MetricFormatSpec::setFormat(const char *format)
{
    d_format = format;
}

// ACCESSORS
inline
float baem_MetricFormatSpec::scale() const
{
    return d_scale;
}

inline
const char *baem_MetricFormatSpec::format() const
{
    return d_format;
}

// FREE OPERATORS
inline
bool operator==(const baem_MetricFormatSpec& lhs,
                const baem_MetricFormatSpec& rhs)
{
    return lhs.scale() == rhs.scale()
        && 0 == bsl::strcmp(lhs.format(), rhs.format());
}

inline
bool operator!=(const baem_MetricFormatSpec& lhs,
                const baem_MetricFormatSpec& rhs)
{
    return !(lhs == rhs);
}

inline
bsl::ostream& operator<<(bsl::ostream&                stream,
                         const baem_MetricFormatSpec& rhs)
{
    return rhs.print(stream);
}

                        // -----------------------
                        // class baem_MetricFormat
                        // -----------------------

// CREATORS
inline
baem_MetricFormat::baem_MetricFormat(bslma_Allocator *basicAllocator)
: d_formatSpecs(basicAllocator)
{
}

inline
baem_MetricFormat::baem_MetricFormat(const baem_MetricFormat&  original,
                                     bslma_Allocator          *basicAllocator)
: d_formatSpecs(original.d_formatSpecs, basicAllocator)
{
}

// MANIPULATORS
inline
baem_MetricFormat&
baem_MetricFormat::operator=(const baem_MetricFormat& rhs)
{
    d_formatSpecs = rhs.d_formatSpecs;
    return *this;
}

inline
void baem_MetricFormat::setFormatSpec(
                                  baem_PublicationType::Value  publicationType,
                                  const baem_MetricFormatSpec& formatSpec)
{
    if (d_formatSpecs.empty()) {
        d_formatSpecs.resize(baem_PublicationType::BAEM_LENGTH);
    }
    d_formatSpecs[(int)publicationType].makeValue(formatSpec);
}

inline
void baem_MetricFormat::clearFormatSpecs()
{
    d_formatSpecs.clear();
}

// ACCESSORS
inline
const baem_MetricFormatSpec *baem_MetricFormat::formatSpec(
                             baem_PublicationType::Value publicationType) const
{
    if (d_formatSpecs.empty()) {
        return 0;
    }
    const AggregateFormatSpec& spec = d_formatSpecs[publicationType];
    return spec.isNull() ? 0 : &spec.value();
}

// FREE OPERATORS
inline
bool operator==(const baem_MetricFormat& lhs,
                const baem_MetricFormat& rhs)
{
    return lhs.d_formatSpecs == rhs.d_formatSpecs;
}

inline
bool operator!=(const baem_MetricFormat& lhs,
                const baem_MetricFormat& rhs)
{
    return !(lhs == rhs);
}

inline
bsl::ostream& operator<<(bsl::ostream& stream, const baem_MetricFormat& rhs)
{
    return rhs.print(stream, 0, -1);
}

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2009
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
