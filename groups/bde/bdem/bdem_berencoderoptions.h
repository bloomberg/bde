// bdem_berencoderoptions.h   -*-C++-*-
#ifndef INCLUDED_BDEM_BERENCODEROPTIONS
#define INCLUDED_BDEM_BERENCODEROPTIONS

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: TODO: Provide purpose
//
//@CLASSES:
// bdem_BerEncoderOptions: TODO: Provide purpose
//
//@AUTHOR: Pablo Halpern (phalpern@bloomberg.net)
//
//@DESCRIPTION:
// Schema of options records for bdem codecs

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITS
#include <bslalg_typetraits.h>
#endif

#ifndef INCLUDED_BDEAT_ATTRIBUTEINFO
#include <bdeat_attributeinfo.h>
#endif

#ifndef INCLUDED_BDEAT_TYPETRAITS
#include <bdeat_typetraits.h>
#endif

#ifndef INCLUDED_BDEAT_VALUETYPEFUNCTIONS
#include <bdeat_valuetypefunctions.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BDEX_INSTREAMFUNCTIONS
#include <bdex_instreamfunctions.h>
#endif

#ifndef INCLUDED_BDEX_OUTSTREAMFUNCTIONS
#include <bdex_outstreamfunctions.h>
#endif

#ifndef INCLUDED_BDEU_PRINTMETHODS
#include <bdeu_printmethods.h>
#endif

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#endif

namespace BloombergLP {

                     // ============================
                     // class bdem_BerEncoderOptions
                     // ============================

class bdem_BerEncoderOptions {
    // BER encoding options

  private:
    int  d_traceLevel;
        // trace (verbosity) level
    int  d_bdeVersionConformance;
        // The largest BDE version that can be assumed of the corresponding
        // decoder for the encoded message, expressed as 10000*majorVersion +
        // 100*minorVersion + patchVersion (e.g.  1.5.0 is expressed as 10500).
        //  Ideally, the BER encoder should be permitted to generate any BER
        // that conforms to X.690 (Basic Encoding Rules) and X.694 (mapping of
        // XSD to ASN.1).  In practice, however, certain unimplemented features
        // and misunderstandings of these standards have resulted in a decoder
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

  public:
    // TYPES
    enum {
        NUM_ATTRIBUTES = 2  // the number of attributes in this class
    };

    enum {
        ATTRIBUTE_INDEX_TRACE_LEVEL = 0,
            // index for "TraceLevel" attribute
        ATTRIBUTE_INDEX_BDE_VERSION_CONFORMANCE = 1
            // index for "BdeVersionConformance" attribute
    };

    enum {
        ATTRIBUTE_ID_TRACE_LEVEL = 0,
            // id for "TraceLevel" attribute
        ATTRIBUTE_ID_BDE_VERSION_CONFORMANCE = 1
            // id for "BdeVersionConformance" attribute
    };

  public:
    // CONSTANTS
    static const char CLASS_NAME[];
        // the name of this class (i.e., "bdem_BerEncoderOptions")

    static const int DEFAULT_TRACE_LEVEL;
        // default value of "TraceLevel" attribute

    static const int DEFAULT_BDE_VERSION_CONFORMANCE;
        // default value of "BdeVersionConformance" attribute

    static const bdeat_AttributeInfo ATTRIBUTE_INFO_ARRAY[];
        // attribute information for each attribute

  public:
    // CLASS METHODS
    static int maxSupportedBdexVersion();
        // Return the most current 'bdex' streaming version number supported by
        // this class.  See the 'bdex' package-level documentation for more
        // information on 'bdex' streaming of value-semantic types and
        // containers.

    static const bdeat_AttributeInfo *lookupAttributeInfo(int id);
        // Return attribute information for the attribute indicated by the
        // specified 'id' if the attribute exists, and 0 otherwise.

    static const bdeat_AttributeInfo *lookupAttributeInfo(
                                                    const char *name,
                                                    int         nameLength);
        // Return attribute information for the attribute indicated by the
        // specified 'name' of the specified 'nameLength' if the attribute
        // exists, and 0 otherwise.

    // CREATORS
    bdem_BerEncoderOptions();
        // Create an object of type 'bdem_BerEncoderOptions' having the
        // default value.

    bdem_BerEncoderOptions(const bdem_BerEncoderOptions& original);
        // Create an object of type 'bdem_BerEncoderOptions' having the
        // value of the specified 'original' object.

    // ~bdem_BerEncoderOptions();
        // Destroy this object.  Note that this trivial destructor is generated
        // by the compiler.

    // MANIPULATORS
    bdem_BerEncoderOptions& operator=(const bdem_BerEncoderOptions& rhs);
        // Assign to this object the value of the specified 'rhs' object.

    template <class STREAM>
    STREAM& bdexStreamIn(STREAM& stream, int version);
        // Assign to this object the value read from the specified input
        // 'stream' using the specified 'version' format and return a reference
        // to the modifiable 'stream'.  If 'stream' is initially invalid, this
        // operation has no effect.  If 'stream' becomes invalid during this
        // operation, this object is valid, but its value is undefined.  If
        // 'version' is not supported, 'stream' is marked invalid and this
        // object is unaltered.  Note that no version is read from 'stream'.
        // See the 'bdex' package-level documentation for more information on
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
        // See the 'bdex' package-level documentation for more information
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

    const int& traceLevel() const;
        // Return a reference to the non-modifiable "TraceLevel" attribute of
        // this object.

    const int& bdeVersionConformance() const;
        // Return a reference to the non-modifiable "BdeVersionConformance"
        // attribute of this object.
};

// FREE OPERATORS
inline
bool operator==(const bdem_BerEncoderOptions& lhs,
                const bdem_BerEncoderOptions& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
    // the same value, and 'false' otherwise.  Two attribute objects have the
    // same value if each respective attribute has the same value.

inline
bool operator!=(const bdem_BerEncoderOptions& lhs,
                const bdem_BerEncoderOptions& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
    // have the same value, and 'false' otherwise.  Two attribute objects do
    // not have the same value if one or more respective attributes differ in
    // values.

inline
bsl::ostream& operator<<(bsl::ostream&                 stream,
                         const bdem_BerEncoderOptions& rhs);
    // Format the specified 'rhs' to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.

// TRAITS
BDEAT_DECL_SEQUENCE_TRAITS(bdem_BerEncoderOptions)

// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================

                     // ----------------------------
                     // class bdem_BerEncoderOptions
                     // ----------------------------

// CLASS METHODS
inline
int bdem_BerEncoderOptions::maxSupportedBdexVersion()
{
    return 1;  // versions start at 1
}

// CREATORS
inline
bdem_BerEncoderOptions::bdem_BerEncoderOptions()
: d_traceLevel(DEFAULT_TRACE_LEVEL)
, d_bdeVersionConformance(DEFAULT_BDE_VERSION_CONFORMANCE)
{
}

inline
bdem_BerEncoderOptions::bdem_BerEncoderOptions(
                                        const bdem_BerEncoderOptions& original)
: d_traceLevel(original.d_traceLevel)
, d_bdeVersionConformance(original.d_bdeVersionConformance)
{
}

// MANIPULATORS
inline
bdem_BerEncoderOptions&
bdem_BerEncoderOptions::operator=(const bdem_BerEncoderOptions& rhs)
{
    if (this != &rhs) {
        d_traceLevel = rhs.d_traceLevel;
        d_bdeVersionConformance = rhs.d_bdeVersionConformance;
    }
    return *this;
}

template <class STREAM>
inline
STREAM& bdem_BerEncoderOptions::bdexStreamIn(STREAM& stream, int version)
{
    if (stream) {
        switch (version) {  // Switch on the schema version (starting with 1).
          case 1: {
            bdex_InStreamFunctions::streamIn(stream, d_traceLevel, 1);
            bdex_InStreamFunctions::streamIn(stream,
                                             d_bdeVersionConformance,
                                             1);
          } break;
          default: {
            stream.invalidate();
          }
        }
    }
    return stream;
}

inline
void bdem_BerEncoderOptions::reset()
{
    d_traceLevel = DEFAULT_TRACE_LEVEL;
    d_bdeVersionConformance = DEFAULT_BDE_VERSION_CONFORMANCE;
}

template <class MANIPULATOR>
inline
int bdem_BerEncoderOptions::manipulateAttributes(MANIPULATOR& manipulator)
{
    int ret;

    ret = manipulator(&d_traceLevel,
                      ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_TRACE_LEVEL]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = manipulator(&d_bdeVersionConformance,
           ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_BDE_VERSION_CONFORMANCE]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    return ret;
}

template <class MANIPULATOR>
inline
int bdem_BerEncoderOptions::manipulateAttribute(MANIPULATOR& manipulator,
                                                int          id)
{
    enum { BDEM_NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_TRACE_LEVEL: {
        return manipulator(&d_traceLevel,
                      ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_TRACE_LEVEL]);
                                                                      // RETURN
      } break;
      case ATTRIBUTE_ID_BDE_VERSION_CONFORMANCE: {
        return manipulator(&d_bdeVersionConformance,
           ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_BDE_VERSION_CONFORMANCE]);
                                                                      // RETURN
      } break;
      default:
        return BDEM_NOT_FOUND;
    }
}

template <class MANIPULATOR>
inline
int bdem_BerEncoderOptions::manipulateAttribute(
        MANIPULATOR&  manipulator,
        const char   *name,
        int           nameLength)
{
    enum { BDEM_NOT_FOUND = -1 };

    const bdeat_AttributeInfo *attributeInfo =
           lookupAttributeInfo(name, nameLength);
    if (0 == attributeInfo) {
        return BDEM_NOT_FOUND;                                             // RETURN
    }

    return manipulateAttribute(manipulator, attributeInfo->d_id);
}

inline
void bdem_BerEncoderOptions::setTraceLevel(int value)
{
    d_traceLevel = value;
}

inline
int& bdem_BerEncoderOptions::bdeVersionConformance()
{
    return d_bdeVersionConformance;
}

// ACCESSORS
template <class STREAM>
inline
STREAM& bdem_BerEncoderOptions::bdexStreamOut(STREAM& stream,
                                              int     version) const
{
    switch (version) {
      case 1: {
        bdex_OutStreamFunctions::streamOut(stream, d_traceLevel, 1);
        bdex_OutStreamFunctions::streamOut(stream, d_bdeVersionConformance, 1);
      } break;
    }
    return stream;
}

template <class ACCESSOR>
inline
int bdem_BerEncoderOptions::accessAttributes(ACCESSOR& accessor) const
{
    int ret;

    ret = accessor(d_traceLevel,
                   ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_TRACE_LEVEL]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = accessor(d_bdeVersionConformance,
           ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_BDE_VERSION_CONFORMANCE]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    return ret;
}

template <class ACCESSOR>
inline
int bdem_BerEncoderOptions::accessAttribute(ACCESSOR& accessor, int id) const
{
    enum { BDEM_NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_TRACE_LEVEL: {
        return accessor(d_traceLevel,
                       ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_TRACE_LEVEL]);
                                                                      // RETURN
      } break;
      case ATTRIBUTE_ID_BDE_VERSION_CONFORMANCE: {
        return accessor(d_bdeVersionConformance,
           ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_BDE_VERSION_CONFORMANCE]);
                                                                      // RETURN
      } break;
      default:
        return BDEM_NOT_FOUND;
    }
}

template <class ACCESSOR>
inline
int bdem_BerEncoderOptions::accessAttribute(
        ACCESSOR&   accessor,
        const char *name,
        int         nameLength) const
{
    enum { BDEM_NOT_FOUND = -1 };

    const bdeat_AttributeInfo *attributeInfo =
          lookupAttributeInfo(name, nameLength);
    if (0 == attributeInfo) {
       return BDEM_NOT_FOUND;                                         // RETURN
    }

    return accessAttribute(accessor, attributeInfo->d_id);
}

inline
const int& bdem_BerEncoderOptions::traceLevel() const
{
    return d_traceLevel;
}

inline
const int& bdem_BerEncoderOptions::bdeVersionConformance() const
{
    return d_bdeVersionConformance;
}

// FREE FUNCTIONS
inline
bool operator==(const bdem_BerEncoderOptions& lhs,
                const bdem_BerEncoderOptions& rhs)
{
    return  lhs.traceLevel() == rhs.traceLevel()
         && lhs.bdeVersionConformance() == rhs.bdeVersionConformance();
}

inline
bool operator!=(const bdem_BerEncoderOptions& lhs,
                const bdem_BerEncoderOptions& rhs)
{
    return  lhs.traceLevel() != rhs.traceLevel()
         || lhs.bdeVersionConformance() != rhs.bdeVersionConformance();
}

inline
bsl::ostream& operator<<(bsl::ostream&                 stream,
                         const bdem_BerEncoderOptions& rhs)
{
    return rhs.print(stream, 0, -1);
}

}  // close namespace BloombergLP;
#endif

// GENERATED BY BLP_BAS_CODEGEN_2.1.6 Thu Jun  7 12:17:03 2007
// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2007
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ------------------------------ END-OF-FILE ---------------------------------
