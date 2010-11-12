// bdem_berdecoderoptions.h   -*-C++-*-
#ifndef INCLUDED_BDEM_BERDECODEROPTIONS
#define INCLUDED_BDEM_BERDECODEROPTIONS

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: TODO: Provide purpose
//
//@CLASSES:
// bdem::bdem_BerDecoderOptions: TODO: Provide purpose
//
//@AUTHOR: Alexander Libman (alibman1@bloomberg.net)
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
                     // class bdem_BerDecoderOptions
                     // ============================

class bdem_BerDecoderOptions {
    // BER decoding options

  private:
    int   d_maxDepth;
        // maximum recursion depth
    bool  d_skipUnknownElements;
        // Option to skip unknown elements
    int   d_traceLevel;
        // trace (verbosity) level
    int   d_maxSequenceSize;
        // maximum sequence size

  public:
    // TYPES
    enum {
        NUM_ATTRIBUTES = 4  // the number of attributes in this class
    };

    enum {
        ATTRIBUTE_INDEX_MAX_DEPTH = 0,
            // index for "MaxDepth" attribute
        ATTRIBUTE_INDEX_SKIP_UNKNOWN_ELEMENTS = 1,
            // index for "SkipUnknownElements" attribute
        ATTRIBUTE_INDEX_TRACE_LEVEL = 2,
            // index for "TraceLevel" attribute
        ATTRIBUTE_INDEX_MAX_SEQUENCE_SIZE = 3
            // index for "MaxSequenceSize" attribute
    };

    enum {
        ATTRIBUTE_ID_MAX_DEPTH = 0,
            // id for "MaxDepth" attribute
        ATTRIBUTE_ID_SKIP_UNKNOWN_ELEMENTS = 1,
            // id for "SkipUnknownElements" attribute
        ATTRIBUTE_ID_TRACE_LEVEL = 2,
            // id for "TraceLevel" attribute
        ATTRIBUTE_ID_MAX_SEQUENCE_SIZE = 3
            // id for "MaxSequenceSize" attribute
    };

  public:
    // CONSTANTS
    static const char CLASS_NAME[];
        // the name of this class (i.e., "bdem_BerDecoderOptions")

    static const int DEFAULT_MAX_DEPTH;
        // default value of "MaxDepth" attribute

    static const bool DEFAULT_SKIP_UNKNOWN_ELEMENTS;
        // default value of "SkipUnknownElements" attribute

    static const int DEFAULT_TRACE_LEVEL;
        // default value of "TraceLevel" attribute

    static const int DEFAULT_MAX_SEQUENCE_SIZE;
        // default value of "MaxSequenceSize" attribute

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
    bdem_BerDecoderOptions();
        // Create an object of type 'bdem_BerDecoderOptions' having the default
        // value.

    bdem_BerDecoderOptions(const bdem_BerDecoderOptions& original);
        // Create an object of type 'bdem_BerDecoderOptions' having the value
        // of the specified 'original' object.

    // ~bdem_BerDecoderOptions();
        // Destroy this object.  Note that this trivial destructor is generated
        // by the compiler.

    // MANIPULATORS
    bdem_BerDecoderOptions& operator=(const bdem_BerDecoderOptions& rhs);
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

    void setMaxDepth(int value);
        // Set the "MaxDepth" attribute of this object to the specified
        // 'value'.

    void setSkipUnknownElements(bool value);
        // Set the "SkipUnknownElements" attribute of this object to the
        // specified 'value'.

    void setTraceLevel(int value);
        // Set the "TraceLevel" attribute of this object to the specified
        // 'value'.

    void setMaxSequenceSize(int value);
        // Set the "MaxSequenceSize" attribute of this object to the specified
        // 'value'.

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

    const int& maxDepth() const;
        // Return a reference to the non-modifiable "MaxDepth" attribute of
        // this object.

    const bool& skipUnknownElements() const;
        // Return a reference to the non-modifiable "SkipUnknownElements"
        // attribute of this object.

    const int& traceLevel() const;
        // Return a reference to the non-modifiable "TraceLevel" attribute of
        // this object.

    const int& maxSequenceSize() const;
        // Return a reference to the non-modifiable "MaxSequenceSize" attribute
        // of this object.
};

// FREE OPERATORS
inline
bool operator==(const bdem_BerDecoderOptions& lhs,
                const bdem_BerDecoderOptions& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
    // the same value, and 'false' otherwise.  Two attribute objects have the
    // same value if each respective attribute has the same value.

inline
bool operator!=(const bdem_BerDecoderOptions& lhs,
                const bdem_BerDecoderOptions& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
    // have the same value, and 'false' otherwise.  Two attribute objects do
    // not have the same value if one or more respective attributes differ in
    // values.

inline
bsl::ostream& operator<<(bsl::ostream&                 stream,
                         const bdem_BerDecoderOptions& rhs);
    // Format the specified 'rhs' to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.

// TRAITS
BDEAT_DECL_SEQUENCE_TRAITS(bdem_BerDecoderOptions)

// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================

                          // ----------------------------
                          // class bdem_BerDecoderOptions
                          // ----------------------------

// CLASS METHODS
inline
int bdem_BerDecoderOptions::maxSupportedBdexVersion()
{
    return 1;  // versions start at 1
}

// CREATORS
inline
bdem_BerDecoderOptions::bdem_BerDecoderOptions()
: d_maxDepth(DEFAULT_MAX_DEPTH)
, d_skipUnknownElements(DEFAULT_SKIP_UNKNOWN_ELEMENTS)
, d_traceLevel(DEFAULT_TRACE_LEVEL)
, d_maxSequenceSize(DEFAULT_MAX_SEQUENCE_SIZE)
{
}

inline
bdem_BerDecoderOptions::bdem_BerDecoderOptions(
                                        const bdem_BerDecoderOptions& original)
: d_maxDepth(original.d_maxDepth)
, d_skipUnknownElements(original.d_skipUnknownElements)
, d_traceLevel(original.d_traceLevel)
, d_maxSequenceSize(original.d_maxSequenceSize)
{
}

// MANIPULATORS
inline
bdem_BerDecoderOptions&
bdem_BerDecoderOptions::operator=(const bdem_BerDecoderOptions& rhs)
{
    if (this != &rhs) {
        d_maxDepth = rhs.d_maxDepth;
        d_skipUnknownElements = rhs.d_skipUnknownElements;
        d_traceLevel = rhs.d_traceLevel;
        d_maxSequenceSize = rhs.d_maxSequenceSize;
    }
    return *this;
}

template <class STREAM>
inline
STREAM& bdem_BerDecoderOptions::bdexStreamIn(STREAM& stream, int version)
{
    if (stream) {
        switch (version) {  // Switch on the schema version (starting with 1).
          case 1: {
            bdex_InStreamFunctions::streamIn(stream, d_maxDepth, 1);
            bdex_InStreamFunctions::streamIn(stream, d_skipUnknownElements, 1);
            bdex_InStreamFunctions::streamIn(stream, d_traceLevel, 1);
            bdex_InStreamFunctions::streamIn(stream, d_maxSequenceSize, 1);
          } break;
          default: {
            stream.invalidate();
          }
        }
    }
    return stream;
}

inline
void bdem_BerDecoderOptions::reset()
{
    d_maxDepth = DEFAULT_MAX_DEPTH;
    d_skipUnknownElements = DEFAULT_SKIP_UNKNOWN_ELEMENTS;
    d_traceLevel = DEFAULT_TRACE_LEVEL;
    d_maxSequenceSize = DEFAULT_MAX_SEQUENCE_SIZE;
}

template <class MANIPULATOR>
inline
int bdem_BerDecoderOptions::manipulateAttributes(MANIPULATOR& manipulator)
{
    int ret;

    ret = manipulator(&d_maxDepth,
                      ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_MAX_DEPTH]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = manipulator(&d_skipUnknownElements,
                  ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SKIP_UNKNOWN_ELEMENTS]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = manipulator(&d_traceLevel,
                      ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_TRACE_LEVEL]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = manipulator(&d_maxSequenceSize,
                      ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_MAX_SEQUENCE_SIZE]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    return ret;
}

template <class MANIPULATOR>
inline
int bdem_BerDecoderOptions::manipulateAttribute(MANIPULATOR& manipulator,
                                                int          id)
{
    enum { BDEM_NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_MAX_DEPTH: {
        return manipulator(&d_maxDepth,
                           ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_MAX_DEPTH]);
                                                                      // RETURN
      } break;
      case ATTRIBUTE_ID_SKIP_UNKNOWN_ELEMENTS: {
        return manipulator(&d_skipUnknownElements,
                  ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SKIP_UNKNOWN_ELEMENTS]);
                                                                      // RETURN
      } break;
      case ATTRIBUTE_ID_TRACE_LEVEL: {
        return manipulator(&d_traceLevel,
                           ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_TRACE_LEVEL]);
                                                                      // RETURN
      } break;
      case ATTRIBUTE_ID_MAX_SEQUENCE_SIZE: {
        return manipulator(&d_maxSequenceSize,
                      ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_MAX_SEQUENCE_SIZE]);
                                                                      // RETURN
      } break;
      default:
        return BDEM_NOT_FOUND;
    }
}

template <class MANIPULATOR>
inline
int bdem_BerDecoderOptions::manipulateAttribute(
        MANIPULATOR&  manipulator,
        const char   *name,
        int           nameLength)
{
    enum { BDEM_NOT_FOUND = -1 };

    const bdeat_AttributeInfo *attributeInfo =
           lookupAttributeInfo(name, nameLength);
    if (0 == attributeInfo) {
        return BDEM_NOT_FOUND;                                        // RETURN
    }

    return manipulateAttribute(manipulator, attributeInfo->d_id);
}

inline
void bdem_BerDecoderOptions::setMaxDepth(int value)
{
    d_maxDepth = value;
}

inline
void bdem_BerDecoderOptions::setSkipUnknownElements(bool value)
{
    d_skipUnknownElements = value;
}

inline
void bdem_BerDecoderOptions::setTraceLevel(int value)
{
    d_traceLevel = value;
}

inline
void bdem_BerDecoderOptions::setMaxSequenceSize(int value)
{
    d_maxSequenceSize = value;
}

// ACCESSORS
template <class STREAM>
inline
STREAM& bdem_BerDecoderOptions::bdexStreamOut(STREAM& stream,
                                              int     version) const
{
    switch (version) {
      case 1: {
        bdex_OutStreamFunctions::streamOut(stream, d_maxDepth, 1);
        bdex_OutStreamFunctions::streamOut(stream, d_skipUnknownElements, 1);
        bdex_OutStreamFunctions::streamOut(stream, d_traceLevel, 1);
        bdex_OutStreamFunctions::streamOut(stream, d_maxSequenceSize, 1);
      } break;
    }
    return stream;
}

template <class ACCESSOR>
inline
int bdem_BerDecoderOptions::accessAttributes(ACCESSOR& accessor) const
{
    int ret;

    ret = accessor(d_maxDepth,
                   ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_MAX_DEPTH]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = accessor(d_skipUnknownElements,
                  ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SKIP_UNKNOWN_ELEMENTS]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = accessor(d_traceLevel,
                   ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_TRACE_LEVEL]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = accessor(d_maxSequenceSize,
                   ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_MAX_SEQUENCE_SIZE]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    return ret;
}

template <class ACCESSOR>
inline
int bdem_BerDecoderOptions::accessAttribute(ACCESSOR& accessor, int id) const
{
    enum { BDEM_NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_MAX_DEPTH: {
        return accessor(d_maxDepth,
                        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_MAX_DEPTH]);
                                                                      // RETURN
      } break;
      case ATTRIBUTE_ID_SKIP_UNKNOWN_ELEMENTS: {
        return accessor(d_skipUnknownElements,
                  ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SKIP_UNKNOWN_ELEMENTS]);
                                                                      // RETURN
      } break;
      case ATTRIBUTE_ID_TRACE_LEVEL: {
        return accessor(d_traceLevel,
                        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_TRACE_LEVEL]);
                                                                      // RETURN
      } break;
      case ATTRIBUTE_ID_MAX_SEQUENCE_SIZE: {
        return accessor(d_maxSequenceSize,
                      ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_MAX_SEQUENCE_SIZE]);
                                                                      // RETURN
      } break;
      default:
        return BDEM_NOT_FOUND;
    }
}

template <class ACCESSOR>
inline
int bdem_BerDecoderOptions::accessAttribute(
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
const int& bdem_BerDecoderOptions::maxDepth() const
{
    return d_maxDepth;
}

inline
const bool& bdem_BerDecoderOptions::skipUnknownElements() const
{
    return d_skipUnknownElements;
}

inline
const int& bdem_BerDecoderOptions::traceLevel() const
{
    return d_traceLevel;
}

inline
const int& bdem_BerDecoderOptions::maxSequenceSize() const
{
    return d_maxSequenceSize;
}

// FREE FUNCTIONS
inline
bool operator==(const bdem_BerDecoderOptions& lhs,
                const bdem_BerDecoderOptions& rhs)
{
    return  lhs.maxDepth() == rhs.maxDepth()
         && lhs.skipUnknownElements() == rhs.skipUnknownElements()
         && lhs.traceLevel() == rhs.traceLevel()
         && lhs.maxSequenceSize() == rhs.maxSequenceSize();
}

inline
bool operator!=(const bdem_BerDecoderOptions& lhs,
                const bdem_BerDecoderOptions& rhs)
{
    return  lhs.maxDepth() != rhs.maxDepth()
         || lhs.skipUnknownElements() != rhs.skipUnknownElements()
         || lhs.traceLevel() != rhs.traceLevel()
         || lhs.maxSequenceSize() != rhs.maxSequenceSize();
}

inline
bsl::ostream& operator<<(bsl::ostream&                 stream,
                         const bdem_BerDecoderOptions& rhs)
{
    return rhs.print(stream, 0, -1);
}

}  // close namespace BloombergLP

#endif

// GENERATED BY BLP_BAS_CODEGEN_2.1.11 Tue Mar 11 14:28:56 2008
// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2008
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ------------------------------ END-OF-FILE ---------------------------------
