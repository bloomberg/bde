// balber_berencoderoptions.h        *DO NOT EDIT*         @generated -*-C++-*-
#ifndef INCLUDED_BALBER_BERENCODEROPTIONS
#define INCLUDED_BALBER_BERENCODEROPTIONS

#include <bsls_ident.h>
BSLS_IDENT_RCSID(balber_berencoderoptions_h,"$Id$ $CSID$")
BSLS_IDENT_PRAGMA_ONCE

//@PURPOSE: Provide value-semantic attribute classes

#include <bslalg_typetraits.h>

#include <bdlat_attributeinfo.h>

#include <bdlat_selectioninfo.h>

#include <bdlat_typetraits.h>

#include <bsls_objectbuffer.h>

#include <bslx_instreamfunctions.h>
#include <bslx_outstreamfunctions.h>

#include <bsls_assert.h>

#include <bsl_iosfwd.h>
#include <bsl_limits.h>

namespace BloombergLP {

namespace balber { class BerEncoderOptions; }
namespace balber {

                          // =======================
                          // class BerEncoderOptions
                          // =======================

class BerEncoderOptions {
    // BER encoding options

    // INSTANCE DATA
    int   d_traceLevel;
        // trace (verbosity) level
    int   d_bdeVersionConformance;
        // The largest BDE version that can be assumed of the corresponding
        // decoder for the encoded message, expressed as 10000*majorVersion +
        // 100*minorVersion + patchVersion (e.g.  1.5.0 is expressed as 10500).
        //
        // Ideally, the BER encoder should be permitted to generate any BER
        // that conforms to X.690 (Basic Encoding Rules) and X.694 (mapping of
        // XSD to ASN.1).  In practice, however, certain unimplemented features
        // and missunderstandings of these standards have resulted in a decoder
        // that cannot accept the full range of legal inputs.  Even when the
        // encoder and decoder are both upgraded to a richer subset of BER, the
        // program receiving the encoded values may not have been recompiled
        // with the latest version and, thus restricting the encoder to emit
        // BER that can be understood by the decoder at the other end of the
        // wire.  If it is that the receiver has a more modern decoder, set
        // this variable to a larger value to allow the encoder to produce BER
        // that is richer and more standards conformant.  The default should be
        // increased only when old copies of the decoder are completely out of
        // circulation.
    int   d_datetimeFractionalSecondPrecision;
        // This option controls the number of decimal places used for seconds
        // when encoding 'Datetime' and 'DatetimeTz'.
    bool  d_encodeEmptyArrays;
        // This option allows users to control if empty arrays are encoded.  By
        // default empty arrays are encoded as not encoding empty arrays is
        // non-compliant with the BER encoding specification.
    bool  d_encodeDateAndTimeTypesAsBinary;
        // This option allows users to control if date and time types are
        // encoded as binary integers.  By default these types are encoded as
        // strings in the ISO 8601 format.
    bool  d_disableUnselectedChoiceEncoding;
        // This encode option allows users to control if it is an error to try
        // and encoded any element with an unselected choice.  By default the
        // encoder allows unselected choices by eliding them from the encoding.
    bool  d_preserveSignOfNegativeZero;
        // This *backward*-*compatibility* option controls whether or not
        // negative zero floating-point values are encoded as the BER
        // representation of negative zero or positive zero.  If this option is
        // 'true', negative zero floating-point values are encoded as the  BER
        // representation of negative zero, such that they will decode back to
        // negative zero.  If this option is 'false', negative zero
        // floating-point values are encoded as the BER representation of
        // positive zero, such they they will decode to positive zero.  For
        // backward-compatibility purposes, the default value of this option is
        // 'false'.  Setting this option to 'true' requires the receiving
        // decoder to come from BDE release '3.90.x' or later, or otherwise
        // comply to the ISO/IEC 8825-1:2015 standard.  Clients are encouraged
        // to update their recipients to the latest version of BDE and set this
        // option to 'true'.  Note that the 'false' value of this  option will
        // eventually be deprecated, and the default value changed to 'true'.

  public:
    // TYPES
    enum {
        ATTRIBUTE_ID_TRACE_LEVEL                          = 0
      , ATTRIBUTE_ID_BDE_VERSION_CONFORMANCE              = 1
      , ATTRIBUTE_ID_ENCODE_EMPTY_ARRAYS                  = 2
      , ATTRIBUTE_ID_ENCODE_DATE_AND_TIME_TYPES_AS_BINARY = 3
      , ATTRIBUTE_ID_DATETIME_FRACTIONAL_SECOND_PRECISION = 4
      , ATTRIBUTE_ID_DISABLE_UNSELECTED_CHOICE_ENCODING   = 5
      , ATTRIBUTE_ID_PRESERVE_SIGN_OF_NEGATIVE_ZERO       = 6
    };

    enum {
        NUM_ATTRIBUTES = 7
    };

    enum {
        ATTRIBUTE_INDEX_TRACE_LEVEL                          = 0
      , ATTRIBUTE_INDEX_BDE_VERSION_CONFORMANCE              = 1
      , ATTRIBUTE_INDEX_ENCODE_EMPTY_ARRAYS                  = 2
      , ATTRIBUTE_INDEX_ENCODE_DATE_AND_TIME_TYPES_AS_BINARY = 3
      , ATTRIBUTE_INDEX_DATETIME_FRACTIONAL_SECOND_PRECISION = 4
      , ATTRIBUTE_INDEX_DISABLE_UNSELECTED_CHOICE_ENCODING   = 5
      , ATTRIBUTE_INDEX_PRESERVE_SIGN_OF_NEGATIVE_ZERO       = 6
    };

    // CONSTANTS
    static const char CLASS_NAME[];

    static const int DEFAULT_INITIALIZER_TRACE_LEVEL;

    static const int DEFAULT_INITIALIZER_BDE_VERSION_CONFORMANCE;

    static const bool DEFAULT_INITIALIZER_ENCODE_EMPTY_ARRAYS;

    static const bool DEFAULT_INITIALIZER_ENCODE_DATE_AND_TIME_TYPES_AS_BINARY;

    static const int DEFAULT_INITIALIZER_DATETIME_FRACTIONAL_SECOND_PRECISION;

    static const bool DEFAULT_INITIALIZER_DISABLE_UNSELECTED_CHOICE_ENCODING;

    static const bool DEFAULT_INITIALIZER_PRESERVE_SIGN_OF_NEGATIVE_ZERO;

    static const bdlat_AttributeInfo ATTRIBUTE_INFO_ARRAY[];

  public:
    // CLASS METHODS
    static int maxSupportedBdexVersion();
        // Return the most current 'bdex' streaming version number supported by
        // this class.  See the 'bslx' package-level documentation for more
        // information on 'bdex' streaming of value-semantic types and
        // containers.

    static const bdlat_AttributeInfo *lookupAttributeInfo(int id);
        // Return attribute information for the attribute indicated by the
        // specified 'id' if the attribute exists, and 0 otherwise.

    static const bdlat_AttributeInfo *lookupAttributeInfo(
                                                       const char *name,
                                                       int         nameLength);
        // Return attribute information for the attribute indicated by the
        // specified 'name' of the specified 'nameLength' if the attribute
        // exists, and 0 otherwise.

    // CREATORS
    BerEncoderOptions();
        // Create an object of type 'BerEncoderOptions' having the default
        // value.

    BerEncoderOptions(const BerEncoderOptions& original);
        // Create an object of type 'BerEncoderOptions' having the value of the
        // specified 'original' object.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    BerEncoderOptions(BerEncoderOptions&& original) = default;
        // Create an object of type 'BerEncoderOptions' having the value of the
        // specified 'original' object.  After performing this action, the
        // 'original' object will be left in a valid, but unspecified state.
#endif

    ~BerEncoderOptions();
        // Destroy this object.

    // MANIPULATORS
    BerEncoderOptions& operator=(const BerEncoderOptions& rhs);
        // Assign to this object the value of the specified 'rhs' object.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    BerEncoderOptions& operator=(BerEncoderOptions&& rhs);
        // Assign to this object the value of the specified 'rhs' object.
        // After performing this action, the 'rhs' object will be left in a
        // valid, but unspecified state.
#endif

    template <class STREAM>
    STREAM& bdexStreamIn(STREAM& stream, int version);
        // Assign to this object the value read from the specified input
        // 'stream' using the specified 'version' format and return a reference
        // to the modifiable 'stream'.  If 'stream' is initially invalid, this
        // operation has no effect.  If 'stream' becomes invalid during this
        // operation, this object is valid, but its value is undefined.  If
        // 'version' is not supported, 'stream' is marked invalid and this
        // object is unaltered.  Note that no version is read from 'stream'.
        // See the 'bslx' package-level documentation for more information on
        // 'bdex' streaming of value-semantic types and containers.

    void reset();
        // Reset this object to the default value (i.e., its value upon
        // default construction).

    template<class MANIPULATOR>
    int manipulateAttributes(MANIPULATOR& manipulator);
        // Invoke the specified 'manipulator' sequentially on the address of
        // each (modifiable) attribute of this object, supplying 'manipulator'
        // with the corresponding attribute information structure until such
        // invocation returns a non-zero value.  Return the value from the
        // last invocation of 'manipulator' (i.e., the invocation that
        // terminated the sequence).

    template<class MANIPULATOR>
    int manipulateAttribute(MANIPULATOR& manipulator, int id);
        // Invoke the specified 'manipulator' on the address of
        // the (modifiable) attribute indicated by the specified 'id',
        // supplying 'manipulator' with the corresponding attribute
        // information structure.  Return the value returned from the
        // invocation of 'manipulator' if 'id' identifies an attribute of this
        // class, and -1 otherwise.

    template<class MANIPULATOR>
    int manipulateAttribute(MANIPULATOR&  manipulator,
                            const char   *name,
                            int           nameLength);
        // Invoke the specified 'manipulator' on the address of
        // the (modifiable) attribute indicated by the specified 'name' of the
        // specified 'nameLength', supplying 'manipulator' with the
        // corresponding attribute information structure.  Return the value
        // returned from the invocation of 'manipulator' if 'name' identifies
        // an attribute of this class, and -1 otherwise.

    void setTraceLevel(int value);
        // Set the "TraceLevel" attribute of this object to the specified
        // 'value'.

    int& bdeVersionConformance();
        // Return a reference to the modifiable "BdeVersionConformance"
        // attribute of this object.

    void setEncodeEmptyArrays(bool value);
        // Set the "EncodeEmptyArrays" attribute of this object to the
        // specified 'value'.

    void setEncodeDateAndTimeTypesAsBinary(bool value);
        // Set the "EncodeDateAndTimeTypesAsBinary" attribute of this object to
        // the specified 'value'.

    void setDatetimeFractionalSecondPrecision(int value);
        // Set the "DatetimeFractionalSecondPrecision" attribute of this object
        // to the specified 'value'.

    void setDisableUnselectedChoiceEncoding(bool value);
        // Set the "DisableUnselectedChoiceEncoding" attribute of this object
        // to the specified 'value'.

    void setPreserveSignOfNegativeZero(bool value);
        // Set the "PreserveSignOfNegativeZero" attribute of this object to the
        // specified 'value'.

    // ACCESSORS
    bsl::ostream& print(bsl::ostream& stream,
                        int           level = 0,
                        int           spacesPerLevel = 4) const;
        // Format this object to the specified output 'stream' at the
        // optionally specified indentation 'level' and return a reference to
        // the modifiable 'stream'.  If 'level' is specified, optionally
        // specify 'spacesPerLevel', the number of spaces per indentation level
        // for this and all of its nested objects.  Each line is indented by
        // the absolute value of 'level * spacesPerLevel'.  If 'level' is
        // negative, suppress indentation of the first line.  If
        // 'spacesPerLevel' is negative, suppress line breaks and format the
        // entire output on one line.  If 'stream' is initially invalid, this
        // operation has no effect.  Note that a trailing newline is provided
        // in multiline mode only.

    template <class STREAM>
    STREAM& bdexStreamOut(STREAM& stream, int version) const;
        // Write the value of this object to the specified output 'stream'
        // using the specified 'version' format and return a reference to the
        // modifiable 'stream'.  If 'version' is not supported, 'stream' is
        // unmodified.  Note that 'version' is not written to 'stream'.
        // See the 'bslx' package-level documentation for more information
        // on 'bdex' streaming of value-semantic types and containers.

    template<class ACCESSOR>
    int accessAttributes(ACCESSOR& accessor) const;
        // Invoke the specified 'accessor' sequentially on each
        // (non-modifiable) attribute of this object, supplying 'accessor'
        // with the corresponding attribute information structure until such
        // invocation returns a non-zero value.  Return the value from the
        // last invocation of 'accessor' (i.e., the invocation that terminated
        // the sequence).

    template<class ACCESSOR>
    int accessAttribute(ACCESSOR& accessor, int id) const;
        // Invoke the specified 'accessor' on the (non-modifiable) attribute
        // of this object indicated by the specified 'id', supplying 'accessor'
        // with the corresponding attribute information structure.  Return the
        // value returned from the invocation of 'accessor' if 'id' identifies
        // an attribute of this class, and -1 otherwise.

    template<class ACCESSOR>
    int accessAttribute(ACCESSOR&   accessor,
                        const char *name,
                        int         nameLength) const;
        // Invoke the specified 'accessor' on the (non-modifiable) attribute
        // of this object indicated by the specified 'name' of the specified
        // 'nameLength', supplying 'accessor' with the corresponding attribute
        // information structure.  Return the value returned from the
        // invocation of 'accessor' if 'name' identifies an attribute of this
        // class, and -1 otherwise.

    int traceLevel() const;
        // Return the value of the "TraceLevel" attribute of this object.

    int bdeVersionConformance() const;
        // Return the value of the "BdeVersionConformance" attribute of this
        // object.

    bool encodeEmptyArrays() const;
        // Return the value of the "EncodeEmptyArrays" attribute of this
        // object.

    bool encodeDateAndTimeTypesAsBinary() const;
        // Return the value of the "EncodeDateAndTimeTypesAsBinary" attribute
        // of this object.

    int datetimeFractionalSecondPrecision() const;
        // Return the value of the "DatetimeFractionalSecondPrecision"
        // attribute of this object.

    bool disableUnselectedChoiceEncoding() const;
        // Return the value of the "DisableUnselectedChoiceEncoding" attribute
        // of this object.

    bool preserveSignOfNegativeZero() const;
        // Return the value of the "PreserveSignOfNegativeZero" attribute of
        // this object.
};

// FREE OPERATORS
inline
bool operator==(const BerEncoderOptions& lhs, const BerEncoderOptions& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
    // the same value, and 'false' otherwise.  Two attribute objects have the
    // same value if each respective attribute has the same value.

inline
bool operator!=(const BerEncoderOptions& lhs, const BerEncoderOptions& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
    // have the same value, and 'false' otherwise.  Two attribute objects do
    // not have the same value if one or more respective attributes differ in
    // values.

inline
bsl::ostream& operator<<(bsl::ostream& stream, const BerEncoderOptions& rhs);
    // Format the specified 'rhs' to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.

}  // close package namespace

// TRAITS

BDLAT_DECL_SEQUENCE_WITH_BITWISEMOVEABLE_TRAITS(balber::BerEncoderOptions)

// ============================================================================
//                         INLINE FUNCTION DEFINITIONS
// ============================================================================

namespace balber {

                          // -----------------------
                          // class BerEncoderOptions
                          // -----------------------

// CLASS METHODS
inline
int BerEncoderOptions::maxSupportedBdexVersion()
{
    return 2;  // versions start at 1.
}

// MANIPULATORS
template <class STREAM>
STREAM& BerEncoderOptions::bdexStreamIn(STREAM& stream, int version)
{
    if (stream) {
        switch (version) {
          case 2: {
            bslx::InStreamFunctions::bdexStreamIn(stream, d_traceLevel, 1);
            bslx::InStreamFunctions::bdexStreamIn(stream, d_bdeVersionConformance, 1);
            bslx::InStreamFunctions::bdexStreamIn(stream, d_encodeEmptyArrays, 1);
            bslx::InStreamFunctions::bdexStreamIn(stream, d_encodeDateAndTimeTypesAsBinary, 1);
            bslx::InStreamFunctions::bdexStreamIn(stream, d_datetimeFractionalSecondPrecision, 1);
            bslx::InStreamFunctions::bdexStreamIn(stream, d_disableUnselectedChoiceEncoding, 1);
            bslx::InStreamFunctions::bdexStreamIn(stream, d_preserveSignOfNegativeZero, 1);
          } break;
          case 1: {
            reset();

            bslx::InStreamFunctions::bdexStreamIn(stream, d_traceLevel, 1);
            bslx::InStreamFunctions::bdexStreamIn(stream, d_bdeVersionConformance, 1);
            bslx::InStreamFunctions::bdexStreamIn(stream, d_encodeEmptyArrays, 1);
            bslx::InStreamFunctions::bdexStreamIn(stream, d_encodeDateAndTimeTypesAsBinary, 1);
            bslx::InStreamFunctions::bdexStreamIn(stream, d_datetimeFractionalSecondPrecision, 1);
            bslx::InStreamFunctions::bdexStreamIn(stream, d_disableUnselectedChoiceEncoding, 1);
          } break;
          default: {
            stream.invalidate();
          }
        }
    }
    return stream;
}

template <class MANIPULATOR>
int BerEncoderOptions::manipulateAttributes(MANIPULATOR& manipulator)
{
    int ret;

    ret = manipulator(&d_traceLevel, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_TRACE_LEVEL]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_bdeVersionConformance, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_BDE_VERSION_CONFORMANCE]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_encodeEmptyArrays, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ENCODE_EMPTY_ARRAYS]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_encodeDateAndTimeTypesAsBinary, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ENCODE_DATE_AND_TIME_TYPES_AS_BINARY]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_datetimeFractionalSecondPrecision, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_DATETIME_FRACTIONAL_SECOND_PRECISION]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_disableUnselectedChoiceEncoding, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_DISABLE_UNSELECTED_CHOICE_ENCODING]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_preserveSignOfNegativeZero, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_PRESERVE_SIGN_OF_NEGATIVE_ZERO]);
    if (ret) {
        return ret;
    }

    return ret;
}

template <class MANIPULATOR>
int BerEncoderOptions::manipulateAttribute(MANIPULATOR& manipulator, int id)
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_TRACE_LEVEL: {
        return manipulator(&d_traceLevel, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_TRACE_LEVEL]);
      }
      case ATTRIBUTE_ID_BDE_VERSION_CONFORMANCE: {
        return manipulator(&d_bdeVersionConformance, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_BDE_VERSION_CONFORMANCE]);
      }
      case ATTRIBUTE_ID_ENCODE_EMPTY_ARRAYS: {
        return manipulator(&d_encodeEmptyArrays, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ENCODE_EMPTY_ARRAYS]);
      }
      case ATTRIBUTE_ID_ENCODE_DATE_AND_TIME_TYPES_AS_BINARY: {
        return manipulator(&d_encodeDateAndTimeTypesAsBinary, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ENCODE_DATE_AND_TIME_TYPES_AS_BINARY]);
      }
      case ATTRIBUTE_ID_DATETIME_FRACTIONAL_SECOND_PRECISION: {
        return manipulator(&d_datetimeFractionalSecondPrecision, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_DATETIME_FRACTIONAL_SECOND_PRECISION]);
      }
      case ATTRIBUTE_ID_DISABLE_UNSELECTED_CHOICE_ENCODING: {
        return manipulator(&d_disableUnselectedChoiceEncoding, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_DISABLE_UNSELECTED_CHOICE_ENCODING]);
      }
      case ATTRIBUTE_ID_PRESERVE_SIGN_OF_NEGATIVE_ZERO: {
        return manipulator(&d_preserveSignOfNegativeZero, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_PRESERVE_SIGN_OF_NEGATIVE_ZERO]);
      }
      default:
        return NOT_FOUND;
    }
}

template <class MANIPULATOR>
int BerEncoderOptions::manipulateAttribute(
        MANIPULATOR&  manipulator,
        const char   *name,
        int           nameLength)
{
    enum { NOT_FOUND = -1 };

    const bdlat_AttributeInfo *attributeInfo =
                                         lookupAttributeInfo(name, nameLength);
    if (0 == attributeInfo) {
        return NOT_FOUND;
    }

    return manipulateAttribute(manipulator, attributeInfo->d_id);
}

inline
void BerEncoderOptions::setTraceLevel(int value)
{
    d_traceLevel = value;
}

inline
int& BerEncoderOptions::bdeVersionConformance()
{
    return d_bdeVersionConformance;
}

inline
void BerEncoderOptions::setEncodeEmptyArrays(bool value)
{
    d_encodeEmptyArrays = value;
}

inline
void BerEncoderOptions::setEncodeDateAndTimeTypesAsBinary(bool value)
{
    d_encodeDateAndTimeTypesAsBinary = value;
}

inline
void BerEncoderOptions::setDatetimeFractionalSecondPrecision(int value)
{
    d_datetimeFractionalSecondPrecision = value;
}

inline
void BerEncoderOptions::setDisableUnselectedChoiceEncoding(bool value)
{
    d_disableUnselectedChoiceEncoding = value;
}

inline
void BerEncoderOptions::setPreserveSignOfNegativeZero(bool value)
{
    d_preserveSignOfNegativeZero = value;
}

// ACCESSORS
template <class STREAM>
STREAM& BerEncoderOptions::bdexStreamOut(STREAM& stream, int version) const
{
    switch (version) {
      case 2: {
        bslx::OutStreamFunctions::bdexStreamOut(stream, this->traceLevel(), 1);
        bslx::OutStreamFunctions::bdexStreamOut(stream, this->bdeVersionConformance(), 1);
        bslx::OutStreamFunctions::bdexStreamOut(stream, this->encodeEmptyArrays(), 1);
        bslx::OutStreamFunctions::bdexStreamOut(stream, this->encodeDateAndTimeTypesAsBinary(), 1);
        bslx::OutStreamFunctions::bdexStreamOut(stream, this->datetimeFractionalSecondPrecision(), 1);
        bslx::OutStreamFunctions::bdexStreamOut(stream, this->disableUnselectedChoiceEncoding(), 1);
        bslx::OutStreamFunctions::bdexStreamOut(stream, this->preserveSignOfNegativeZero(), 1);
      } break;
      case 1: {
        bslx::OutStreamFunctions::bdexStreamOut(stream, this->traceLevel(), 1);
        bslx::OutStreamFunctions::bdexStreamOut(stream, this->bdeVersionConformance(), 1);
        bslx::OutStreamFunctions::bdexStreamOut(stream, this->encodeEmptyArrays(), 1);
        bslx::OutStreamFunctions::bdexStreamOut(stream, this->encodeDateAndTimeTypesAsBinary(), 1);
        bslx::OutStreamFunctions::bdexStreamOut(stream, this->datetimeFractionalSecondPrecision(), 1);
        bslx::OutStreamFunctions::bdexStreamOut(stream, this->disableUnselectedChoiceEncoding(), 1);
      } break;
      default: {
        stream.invalidate();
      }
    }
    return stream;
}

template <class ACCESSOR>
int BerEncoderOptions::accessAttributes(ACCESSOR& accessor) const
{
    int ret;

    ret = accessor(d_traceLevel, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_TRACE_LEVEL]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_bdeVersionConformance, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_BDE_VERSION_CONFORMANCE]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_encodeEmptyArrays, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ENCODE_EMPTY_ARRAYS]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_encodeDateAndTimeTypesAsBinary, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ENCODE_DATE_AND_TIME_TYPES_AS_BINARY]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_datetimeFractionalSecondPrecision, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_DATETIME_FRACTIONAL_SECOND_PRECISION]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_disableUnselectedChoiceEncoding, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_DISABLE_UNSELECTED_CHOICE_ENCODING]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_preserveSignOfNegativeZero, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_PRESERVE_SIGN_OF_NEGATIVE_ZERO]);
    if (ret) {
        return ret;
    }

    return ret;
}

template <class ACCESSOR>
int BerEncoderOptions::accessAttribute(ACCESSOR& accessor, int id) const
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_TRACE_LEVEL: {
        return accessor(d_traceLevel, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_TRACE_LEVEL]);
      }
      case ATTRIBUTE_ID_BDE_VERSION_CONFORMANCE: {
        return accessor(d_bdeVersionConformance, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_BDE_VERSION_CONFORMANCE]);
      }
      case ATTRIBUTE_ID_ENCODE_EMPTY_ARRAYS: {
        return accessor(d_encodeEmptyArrays, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ENCODE_EMPTY_ARRAYS]);
      }
      case ATTRIBUTE_ID_ENCODE_DATE_AND_TIME_TYPES_AS_BINARY: {
        return accessor(d_encodeDateAndTimeTypesAsBinary, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ENCODE_DATE_AND_TIME_TYPES_AS_BINARY]);
      }
      case ATTRIBUTE_ID_DATETIME_FRACTIONAL_SECOND_PRECISION: {
        return accessor(d_datetimeFractionalSecondPrecision, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_DATETIME_FRACTIONAL_SECOND_PRECISION]);
      }
      case ATTRIBUTE_ID_DISABLE_UNSELECTED_CHOICE_ENCODING: {
        return accessor(d_disableUnselectedChoiceEncoding, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_DISABLE_UNSELECTED_CHOICE_ENCODING]);
      }
      case ATTRIBUTE_ID_PRESERVE_SIGN_OF_NEGATIVE_ZERO: {
        return accessor(d_preserveSignOfNegativeZero, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_PRESERVE_SIGN_OF_NEGATIVE_ZERO]);
      }
      default:
        return NOT_FOUND;
    }
}

template <class ACCESSOR>
int BerEncoderOptions::accessAttribute(
        ACCESSOR&   accessor,
        const char *name,
        int         nameLength) const
{
    enum { NOT_FOUND = -1 };

    const bdlat_AttributeInfo *attributeInfo =
          lookupAttributeInfo(name, nameLength);
    if (0 == attributeInfo) {
       return NOT_FOUND;
    }

    return accessAttribute(accessor, attributeInfo->d_id);
}

inline
int BerEncoderOptions::traceLevel() const
{
    return d_traceLevel;
}

inline
int BerEncoderOptions::bdeVersionConformance() const
{
    return d_bdeVersionConformance;
}

inline
bool BerEncoderOptions::encodeEmptyArrays() const
{
    return d_encodeEmptyArrays;
}

inline
bool BerEncoderOptions::encodeDateAndTimeTypesAsBinary() const
{
    return d_encodeDateAndTimeTypesAsBinary;
}

inline
int BerEncoderOptions::datetimeFractionalSecondPrecision() const
{
    return d_datetimeFractionalSecondPrecision;
}

inline
bool BerEncoderOptions::disableUnselectedChoiceEncoding() const
{
    return d_disableUnselectedChoiceEncoding;
}

inline
bool BerEncoderOptions::preserveSignOfNegativeZero() const
{
    return d_preserveSignOfNegativeZero;
}

}  // close package namespace

// FREE FUNCTIONS

inline
bool balber::operator==(
        const balber::BerEncoderOptions& lhs,
        const balber::BerEncoderOptions& rhs)
{
    return  lhs.traceLevel() == rhs.traceLevel()
         && lhs.bdeVersionConformance() == rhs.bdeVersionConformance()
         && lhs.encodeEmptyArrays() == rhs.encodeEmptyArrays()
         && lhs.encodeDateAndTimeTypesAsBinary() == rhs.encodeDateAndTimeTypesAsBinary()
         && lhs.datetimeFractionalSecondPrecision() == rhs.datetimeFractionalSecondPrecision()
         && lhs.disableUnselectedChoiceEncoding() == rhs.disableUnselectedChoiceEncoding()
         && lhs.preserveSignOfNegativeZero() == rhs.preserveSignOfNegativeZero();
}

inline
bool balber::operator!=(
        const balber::BerEncoderOptions& lhs,
        const balber::BerEncoderOptions& rhs)
{
    return !(lhs == rhs);
}

inline
bsl::ostream& balber::operator<<(
        bsl::ostream& stream,
        const balber::BerEncoderOptions& rhs)
{
    return rhs.print(stream, 0, -1);
}

}  // close enterprise namespace
#endif

// GENERATED BY BLP_BAS_CODEGEN_2021.07.12.1
// USING bas_codegen.pl -m msg --msgExpand -p balber --noAggregateConversion --noHashSupport -c berencoderoptions balber.xsd
// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright 2021 Bloomberg Finance L.P. All rights reserved.
//      Property of Bloomberg Finance L.P. (BFLP)
//      This software is made available solely pursuant to the
//      terms of a BFLP license agreement which governs its use.
// ------------------------------- END-OF-FILE --------------------------------
