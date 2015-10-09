// balber_berencoderoptions.h-- GENERATED FILE - DO NOT EDIT ---*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BALBER_BERENCODEROPTIONS
#define INCLUDED_BALBER_BERENCODEROPTIONS

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT_RCSID(bdem_berencoderoptions_h,"$Id$ $CSID$")
BSLS_IDENT_PRAGMA_ONCE

//@PURPOSE: Provide value-semantic attribute classes
//
//@CLASSES:
//  balber::BerEncoderOptions: BER encoding options
//
//@DESCRIPTION: TBD

#ifndef INCLUDED_BSLALG_TYPETRAITS
#include <bslalg_typetraits.h>
#endif

#ifndef INCLUDED_BDLAT_ATTRIBUTEINFO
#include <bdlat_attributeinfo.h>
#endif

#ifndef INCLUDED_BDLAT_SELECTIONINFO
#include <bdlat_selectioninfo.h>
#endif

#ifndef INCLUDED_BDLAT_TYPETRAITS
#include <bdlat_typetraits.h>
#endif

#ifndef INCLUDED_BSLS_OBJECTBUFFER
#include <bsls_objectbuffer.h>
#endif

#ifndef INCLUDED_BSLX_INSTREAMFUNCTIONS
#include <bslx_instreamfunctions.h>
#endif

#ifndef INCLUDED_BSLX_OUTSTREAMFUNCTIONS
#include <bslx_outstreamfunctions.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#define INCLUDED_BSL_IOSFWD
#endif

namespace BloombergLP {
namespace balber {

                          // =======================
                          // class BerEncoderOptions
                          // =======================

class BerEncoderOptions {
    // BER encoding options

    // DATA
    int d_traceLevel;
        // trace (verbosity) level

    int d_bdeVersionConformance;
        // The largest BDE version that can be assumed of the corresponding
        // decoder for the encoded message, expressed as
        // '10000*majorVersion + 100*minorVersion + patchVersion' (e.g.,  1.5.0
        // is expressed as 10500).  Ideally, the BER encoder should be
        // permitted to generate any BER that conforms to X.690 (Basic Encoding
        // Rules) and X.694 (mapping of XSD to ASN.1).  In practice, however,
        // certain unimplemented features and missunderstandings of these
        // standards have resulted in a decoder that cannot accept the full
        // range of legal inputs.  Even when the encoder and decoder are both
        // upgraded to a richer subset of BER, the program receiving the
        // encoded values may not have been recompiled with the latest version
        // and, thus restricting the encoder to emit BER that can be understood
        // by the decoder at the other end of the wire.  If it is that the
        // receiver has a more modern decoder, set this variable to a larger
        // value to allow the encoder to produce BER that is richer and more
        // standards conformant.  The default should be increased only when old
        // copies of the decoder are completely out of circulation.

    bool d_encodeEmptyArrays;
        // This option allows users to control if empty arrays are encoded.  By
        // default empty arrays are encoded as not encoding empty arrays is
        // non-compliant with the BER encoding specification.

    bool d_encodeDateAndTimeTypesAsBinary;
        // This option allows users to control if date and time types are
        // encoded as binary integers.  By default these types are encoded as
        // strings in the ISO 8601 format.

  public:
    // TYPES
    enum {
        e_ATTRIBUTE_ID_TRACE_LEVEL                          = 0
      , e_ATTRIBUTE_ID_BDE_VERSION_CONFORMANCE              = 1
      , e_ATTRIBUTE_ID_ENCODE_EMPTY_ARRAYS                  = 2
      , e_ATTRIBUTE_ID_ENCODE_DATE_AND_TIME_TYPES_AS_BINARY = 3
    };

    enum {
        k_NUM_ATTRIBUTES = 4
    };

    enum {
        e_ATTRIBUTE_INDEX_TRACE_LEVEL                          = 0
      , e_ATTRIBUTE_INDEX_BDE_VERSION_CONFORMANCE              = 1
      , e_ATTRIBUTE_INDEX_ENCODE_EMPTY_ARRAYS                  = 2
      , e_ATTRIBUTE_INDEX_ENCODE_DATE_AND_TIME_TYPES_AS_BINARY = 3
    };

    // CONSTANTS
    static const char CLASS_NAME[];
    static const int  DEFAULT_INITIALIZER_TRACE_LEVEL;
    static const int  DEFAULT_INITIALIZER_BDE_VERSION_CONFORMANCE;
    static const bool DEFAULT_INITIALIZER_ENCODE_EMPTY_ARRAYS;
    static const bool DEFAULT_INITIALIZER_ENCODE_DATE_AND_TIME_TYPES_AS_BINARY;
    static const bdlat_AttributeInfo ATTRIBUTE_INFO_ARRAY[];

  public:
    // CLASS METHODS
    static int maxSupportedBdexVersion(int versionSelector);
        // Return the maximum valid BDEX format version, as indicated by the
        // specified 'versionSelector', to be passed to the 'bdexStreamOut'
        // method.  Note that the 'versionSelector' is expected to be formatted
        // as 'yyyymmdd', a date representation.  See the 'bslx' package-level
        // documentation for more information on BDEX streaming of
        // value-semantic types and containers.


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

    ~BerEncoderOptions();
        // Destroy this object.

    // MANIPULATORS
    BerEncoderOptions& operator=(const BerEncoderOptions& rhs);
        // Assign to this object the value of the specified 'rhs' object.

    template <class STREAM>
    STREAM& bdexStreamIn(STREAM& stream, int version);
        // Assign to this object the value read from the specified input
        // 'stream' using the specified 'version' format, and return a
        // reference to 'stream'.  If 'stream' is initially invalid, this
        // operation has no effect.  If 'version' is not supported, this object
        // is unaltered and 'stream' is invalidated, but otherwise unmodified.
        // If 'version' is supported but 'stream' becomes invalid during this
        // operation, this object has an undefined, but valid, state.  Note
        // that no version is read from 'stream'.  See the 'bslx' package-level
        // documentation for more information on BDEX streaming of
        // value-semantic types and containers.

    void reset();
        // Reset this object to the default value (i.e., its value upon default
        // construction).

    template<class MANIPULATOR>
    int manipulateAttributes(MANIPULATOR& manipulator);
        // Invoke the specified 'manipulator' sequentially on the address of
        // each (modifiable) attribute of this object, supplying 'manipulator'
        // with the corresponding attribute information structure until such
        // invocation returns a non-zero value.  Return the value from the last
        // invocation of 'manipulator' (i.e., the invocation that terminated
        // the sequence).

    template<class MANIPULATOR>
    int manipulateAttribute(MANIPULATOR& manipulator, int id);
        // Invoke the specified 'manipulator' on the address of the
        // (modifiable) attribute indicated by the specified 'id', supplying
        // 'manipulator' with the corresponding attribute information
        // structure.  Return the value returned from the invocation of
        // 'manipulator' if 'id' identifies an attribute of this class, and -1
        // otherwise.

    template<class MANIPULATOR>
    int manipulateAttribute(MANIPULATOR&  manipulator,
                            const char   *name,
                            int           nameLength);
        // Invoke the specified 'manipulator' on the address of the
        // (modifiable) attribute indicated by the specified 'name' of the
        // specified 'nameLength', supplying 'manipulator' with the
        // corresponding attribute information structure.  Return the value
        // returned from the invocation of 'manipulator' if 'name' identifies
        // an attribute of this class, and -1 otherwise.

    void setTraceLevel(int value);
        // Set the 'TraceLevel' attribute of this object to the specified
        // 'value'.

    int& bdeVersionConformance();
        // Return a reference to the modifiable 'BdeVersionConformance'
        // attribute of this object.

    void setEncodeEmptyArrays(bool value);
        // Set the 'EncodeEmptyArrays' attribute of this object to the
        // specified 'value'.

    void setEncodeDateAndTimeTypesAsBinary(bool value);
        // Set the 'EncodeDateAndTimeTypesAsBinary' attribute of this object to
        // the specified 'value'.  If this option is set to 'true' then date
        // and time types will be encoded (in a standard-incompliant way) as an
        // octet string as opposed to as a string in the ISO 8601 format as
        // required by the standard.  Note that the binary encoding format is
        // incompatible with the string encoding format and must be used after
        // ensuring that the ber decoder can decode the binary format.

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
        // Write the value of this object, using the specified 'version'
        // format, to the specified output 'stream', and return a reference to
        // 'stream'.  If 'stream' is initially invalid, this operation has no
        // effect.  If 'version' is not supported, 'stream' is invalidated, but
        // otherwise unmodified.  Note that 'version' is not written to
        // 'stream'.  See the 'bslx' package-level documentation for more
        // information on BDEX streaming of value-semantic types and
        // containers.

    template<class ACCESSOR>
    int accessAttributes(ACCESSOR& accessor) const;
        // Invoke the specified 'accessor' sequentially on each
        // (non-modifiable) attribute of this object, supplying 'accessor' with
        // the corresponding attribute information structure until such
        // invocation returns a non-zero value.  Return the value from the last
        // invocation of 'accessor' (i.e., the invocation that terminated the
        // sequence).

    template<class ACCESSOR>
    int accessAttribute(ACCESSOR& accessor, int id) const;
        // Invoke the specified 'accessor' on the (non-modifiable) attribute of
        // this object indicated by the specified 'id', supplying 'accessor'
        // with the corresponding attribute information structure.  Return the
        // value returned from the invocation of 'accessor' if 'id' identifies
        // an attribute of this class, and -1 otherwise.

    template<class ACCESSOR>
    int accessAttribute(ACCESSOR&   accessor,
                        const char *name,
                        int         nameLength) const;
        // Invoke the specified 'accessor' on the (non-modifiable) attribute of
        // this object indicated by the specified 'name' of the specified
        // 'nameLength', supplying 'accessor' with the corresponding attribute
        // information structure.  Return the value returned from the
        // invocation of 'accessor' if 'name' identifies an attribute of this
        // class, and -1 otherwise.

    int traceLevel() const;
        // Return a reference to the non-modifiable 'TraceLevel' attribute of
        // this object.

    int bdeVersionConformance() const;
        // Return a reference to the non-modifiable 'BdeVersionConformance'
        // attribute of this object.

    bool encodeEmptyArrays() const;
        // Return a reference to the non-modifiable 'EncodeEmptyArrays'
        // attribute of this object.

    bool encodeDateAndTimeTypesAsBinary() const;
        // Return a reference to the non-modifiable
        // 'EncodeDateAndTimeTypesAsBinary' attribute of this object.
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
    // Format the specified 'rhs' to the specified output 'stream' and return a
    // reference to the modifiable 'stream'.
}

// TRAITS
BDLAT_DECL_SEQUENCE_WITH_BITWISEMOVEABLE_TRAITS(balber::BerEncoderOptions)

// ============================================================================
//                        INLINE FUNCTION DEFINITIONS
// ============================================================================

namespace balber {

                          // -----------------------
                          // class BerEncoderOptions
                          // -----------------------

// CLASS METHODS
inline
int BerEncoderOptions::maxSupportedBdexVersion(int /* versionSelector */)
{
    return 1;  // versions start at 1.
}


// MANIPULATORS
template <class STREAM>
STREAM& BerEncoderOptions::bdexStreamIn(STREAM& stream, int version)
{
    if (stream) {
        switch (version) {
          case 1: {
            bslx::InStreamFunctions::bdexStreamIn(stream,
                                                  d_traceLevel,
                                                  1);
            bslx::InStreamFunctions::bdexStreamIn(stream,
                                                  d_bdeVersionConformance,
                                                  1);
            bslx::InStreamFunctions::bdexStreamIn(stream,
                                                  d_encodeEmptyArrays,
                                                  1);
            bslx::InStreamFunctions::bdexStreamIn(
                                              stream,
                                              d_encodeDateAndTimeTypesAsBinary,
                                              1);
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

    ret = manipulator(&d_traceLevel,
                      ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_TRACE_LEVEL]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = manipulator(
              &d_bdeVersionConformance,
              ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_BDE_VERSION_CONFORMANCE]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = manipulator(
                  &d_encodeEmptyArrays,
                  ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ENCODE_EMPTY_ARRAYS]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = manipulator(&d_encodeDateAndTimeTypesAsBinary,
                      ATTRIBUTE_INFO_ARRAY[
                      e_ATTRIBUTE_INDEX_ENCODE_DATE_AND_TIME_TYPES_AS_BINARY]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    return ret;
}

template <class MANIPULATOR>
int BerEncoderOptions::manipulateAttribute(MANIPULATOR& manipulator, int id)
{
    enum { k_NOT_FOUND = -1 };

    switch (id) {
      case e_ATTRIBUTE_ID_TRACE_LEVEL: {
        return manipulator(
                          &d_traceLevel,
                          ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_TRACE_LEVEL]);
      } break;
      case e_ATTRIBUTE_ID_BDE_VERSION_CONFORMANCE: {
        return manipulator(
              &d_bdeVersionConformance,
              ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_BDE_VERSION_CONFORMANCE]);
      } break;
      case e_ATTRIBUTE_ID_ENCODE_EMPTY_ARRAYS: {
        return manipulator(
                  &d_encodeEmptyArrays,
                  ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ENCODE_EMPTY_ARRAYS]);
      } break;
      case e_ATTRIBUTE_ID_ENCODE_DATE_AND_TIME_TYPES_AS_BINARY: {
        return manipulator(
                      d_encodeDateAndTimeTypesAsBinary,
                      ATTRIBUTE_INFO_ARRAY[
                      e_ATTRIBUTE_INDEX_ENCODE_DATE_AND_TIME_TYPES_AS_BINARY]);
      } break;
      default:
        return k_NOT_FOUND;
    }
}

template <class MANIPULATOR>
int BerEncoderOptions::manipulateAttribute(MANIPULATOR&  manipulator,
                                           const char   *name,
                                           int           nameLength)
{
    enum { k_NOT_FOUND = -1 };

    const bdlat_AttributeInfo *attributeInfo = lookupAttributeInfo(name,
                                                                   nameLength);
    if (!attributeInfo) {
        return k_NOT_FOUND;
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

// ACCESSORS
template <class STREAM>
STREAM& BerEncoderOptions::bdexStreamOut(STREAM& stream, int version) const
{
    switch (version) {
      case 1: {
        bslx::OutStreamFunctions::bdexStreamOut(stream,
                                                d_traceLevel,
                                                1);
        bslx::OutStreamFunctions::bdexStreamOut(stream,
                                                d_bdeVersionConformance,
                                                1);
        bslx::OutStreamFunctions::bdexStreamOut(stream,
                                                d_encodeEmptyArrays,
                                                1);
        bslx::OutStreamFunctions::bdexStreamOut(
                                              stream,
                                              d_encodeDateAndTimeTypesAsBinary,
                                              1);
      } break;
      default: {
        stream.invalidate();
      } break;
    }
    return stream;
}

template <class ACCESSOR>
int BerEncoderOptions::accessAttributes(ACCESSOR& accessor) const
{
    int ret;

    ret = accessor(d_traceLevel,
                   ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_TRACE_LEVEL]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = accessor(d_bdeVersionConformance,
                   ATTRIBUTE_INFO_ARRAY[
                                   e_ATTRIBUTE_INDEX_BDE_VERSION_CONFORMANCE]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = accessor(d_encodeEmptyArrays,
                   ATTRIBUTE_INFO_ARRAY[
                                       e_ATTRIBUTE_INDEX_ENCODE_EMPTY_ARRAYS]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = accessor(d_encodeDateAndTimeTypesAsBinary,
                   ATTRIBUTE_INFO_ARRAY[
                      e_ATTRIBUTE_INDEX_ENCODE_DATE_AND_TIME_TYPES_AS_BINARY]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    return ret;
}

template <class ACCESSOR>
int BerEncoderOptions::accessAttribute(ACCESSOR& accessor, int id) const
{
    enum { k_NOT_FOUND = -1 };

    switch (id) {
      case e_ATTRIBUTE_ID_TRACE_LEVEL: {
        return accessor(d_traceLevel,
                        ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_TRACE_LEVEL]);
      } break;
      case e_ATTRIBUTE_ID_BDE_VERSION_CONFORMANCE: {
        return accessor(d_bdeVersionConformance,
                        ATTRIBUTE_INFO_ARRAY[
                                   e_ATTRIBUTE_INDEX_BDE_VERSION_CONFORMANCE]);
      } break;
      case e_ATTRIBUTE_ID_ENCODE_EMPTY_ARRAYS: {
        return accessor(d_encodeEmptyArrays,
                        ATTRIBUTE_INFO_ARRAY[
                                       e_ATTRIBUTE_INDEX_ENCODE_EMPTY_ARRAYS]);
      } break;
      case e_ATTRIBUTE_ID_ENCODE_DATE_AND_TIME_TYPES_AS_BINARY: {
        return accessor(
                      d_encodeDateAndTimeTypesAsBinary,
                      ATTRIBUTE_INFO_ARRAY[
                      e_ATTRIBUTE_INDEX_ENCODE_DATE_AND_TIME_TYPES_AS_BINARY]);
      } break;
      default:
        return k_NOT_FOUND;
    }
}

template <class ACCESSOR>
int BerEncoderOptions::accessAttribute(ACCESSOR&   accessor,
                                       const char *name,
                                       int         nameLength) const
{
    enum { k_NOT_FOUND = -1 };

    const bdlat_AttributeInfo *attributeInfo = lookupAttributeInfo(name,
                                                                   nameLength);
    if (!attributeInfo) {
       return k_NOT_FOUND;
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
}  // close package namespace


// FREE FUNCTIONS

inline
bool balber::operator==(const BerEncoderOptions& lhs,
                        const BerEncoderOptions& rhs)
{
    return  lhs.traceLevel()                     == rhs.traceLevel()
         && lhs.bdeVersionConformance()          == rhs.bdeVersionConformance()
         && lhs.encodeEmptyArrays()              == rhs.encodeEmptyArrays()
         && lhs.encodeDateAndTimeTypesAsBinary() ==
                                          rhs.encodeDateAndTimeTypesAsBinary();
}

inline
bool balber::operator!=(const BerEncoderOptions& lhs,
                        const BerEncoderOptions& rhs)
{
    return  lhs.traceLevel()                     != rhs.traceLevel()
         || lhs.bdeVersionConformance()          != rhs.bdeVersionConformance()
         || lhs.encodeEmptyArrays()              != rhs.encodeEmptyArrays()
         || lhs.encodeDateAndTimeTypesAsBinary() !=
                                          rhs.encodeDateAndTimeTypesAsBinary();
}

inline
bsl::ostream& balber::operator<<(bsl::ostream&            stream,
                                 const BerEncoderOptions& rhs)
{
    return rhs.print(stream, 0, -1);
}

}  // close enterpsie namespace
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
