// bdlmxxx_bdemencoderoptions.h   -*-C++-*-   GENERATED FILE -- DO NOT EDIT
#ifndef INCLUDED_BDLMXXX_BDEMENCODEROPTIONS
#define INCLUDED_BDLMXXX_BDEMENCODEROPTIONS

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide value-semantic attribute classes
//
//@DEPRECATED: Do not use.
//
//@CLASSES:
// bdlmxxx::BdemEncoderOptions:
//
//@AUTHOR: Rohan Bhindwale (rbhindwale@bloomberg.net)
//
//@DESCRIPTION:
// Schema of options records for bdem codecs

#ifndef INCLUDED_BDLSCM_VERSION
#include <bdlscm_version.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITS
#include <bslalg_typetraits.h>
#endif

#ifndef INCLUDED_BDLAT_ATTRIBUTEINFO
#include <bdlat_attributeinfo.h>
#endif

#ifndef INCLUDED_BDLAT_TYPETRAITS
#include <bdlat_typetraits.h>
#endif

#ifndef INCLUDED_BDLAT_VALUETYPEFUNCTIONS
#include <bdlat_valuetypefunctions.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BDLXXXX_INSTREAMFUNCTIONS
#include <bdlxxxx_instreamfunctions.h>
#endif

#ifndef INCLUDED_BDLXXXX_OUTSTREAMFUNCTIONS
#include <bdlxxxx_outstreamfunctions.h>
#endif

#ifndef INCLUDED_BDLB_PRINTMETHODS
#include <bdlb_printmethods.h>
#endif

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#endif


namespace BloombergLP {

namespace bdlmxxx {

                    // ===================================
                    // class BdemEncoderOptions
                    // ===================================

class BdemEncoderOptions {
    // BDEM encoding options

  private:
    int  d_bdemVersion;
        // BDEM encoding version

  public:
    // TYPES
    enum {
        NUM_ATTRIBUTES = 1
    };

    enum {
        ATTRIBUTE_INDEX_BDEM_VERSION = 0
    };

    enum {
        ATTRIBUTE_ID_BDEM_VERSION = 0
    };

  public:
    // CONSTANTS
    static const char CLASS_NAME[];

    static const int DEFAULT_BDEM_VERSION;

    static const bdeat_AttributeInfo ATTRIBUTE_INFO_ARRAY[];

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
    BdemEncoderOptions();
        // Create an object of type 'BdemEncoderOptions' having the
        // default value.

    BdemEncoderOptions(const BdemEncoderOptions& original);
        // Create an object of type 'BdemEncoderOptions' having the
        // value of the specified 'original' object.

    // ~BdemEncoderOptions();
        // Destroy this object.  Note that this trivial destructor is generated
        // by the compiler.

    // MANIPULATORS
    BdemEncoderOptions& operator=(const BdemEncoderOptions& rhs);
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

    void setBdemVersion(int value);
        // Set the "BdemVersion" attribute of this object to the specified
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

    const int& bdemVersion() const;
        // Return a reference to the non-modifiable "BdemVersion" attribute of
        // this object.
};

// FREE OPERATORS
inline
bool operator==(const BdemEncoderOptions& lhs,
                const BdemEncoderOptions& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
    // the same value, and 'false' otherwise.  Two attribute objects have the
    // same value if each respective attribute has the same value.

inline
bool operator!=(const BdemEncoderOptions& lhs,
                const BdemEncoderOptions& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
    // have the same value, and 'false' otherwise.  Two attribute objects do
    // not have the same value if one or more respective attributes differ in
    // values.

inline
bsl::ostream& operator<<(bsl::ostream& stream,
                const BdemEncoderOptions& rhs);
    // Format the specified 'rhs' to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.


// TRAITS
}

BDLAT_DECL_SEQUENCE_TRAITS(bdlmxxx::BdemEncoderOptions)

namespace bdlmxxx {

// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================


                    // -----------------------------------
                    // class BdemEncoderOptions
                    // -----------------------------------

// CLASS METHODS
inline
int BdemEncoderOptions::maxSupportedBdexVersion()
{
    return 1;  // versions start at 1.
}

// CREATORS
inline
BdemEncoderOptions::BdemEncoderOptions()
: d_bdemVersion(DEFAULT_BDEM_VERSION)
{
}

inline
BdemEncoderOptions::BdemEncoderOptions(
                                       const BdemEncoderOptions& original)
: d_bdemVersion(original.d_bdemVersion)
{
}

// MANIPULATORS
inline
BdemEncoderOptions&
BdemEncoderOptions::operator=(const BdemEncoderOptions& rhs)
{
    if (this != &rhs) {
        d_bdemVersion = rhs.d_bdemVersion;
    }
    return *this;
}

template <class STREAM>
inline
STREAM& BdemEncoderOptions::bdexStreamIn(STREAM& stream, int version)
{
    if (stream) {
        switch (version) {
          case 1: {
            bdex_InStreamFunctions::streamIn(stream, d_bdemVersion, 1);
          } break;
          default: {
            stream.invalidate();
          }
        }
    }
    return stream;
}

inline
void BdemEncoderOptions::reset()
{
    d_bdemVersion = DEFAULT_BDEM_VERSION;
}

template <class MANIPULATOR>
inline
int BdemEncoderOptions::manipulateAttributes(MANIPULATOR& manipulator)
{
    int ret;

    ret = manipulator(&d_bdemVersion,
                      ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_BDEM_VERSION]);
    if (ret) {
        return ret;
    }

    return ret;
}

template <class MANIPULATOR>
inline
int BdemEncoderOptions::manipulateAttribute(MANIPULATOR& manipulator,
                                                 int id)
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_BDEM_VERSION: {
        return manipulator(&d_bdemVersion,
                           ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_BDEM_VERSION]);
      } break;
      default:
        return NOT_FOUND;
    }
}

template <class MANIPULATOR>
inline
int BdemEncoderOptions::manipulateAttribute(
        MANIPULATOR&  manipulator,
        const char   *name,
        int           nameLength)
{
    enum { NOT_FOUND = -1 };

    const bdeat_AttributeInfo *attributeInfo =
           lookupAttributeInfo(name, nameLength);
    if (0 == attributeInfo) {
        return NOT_FOUND;
    }

    return manipulateAttribute(manipulator, attributeInfo->d_id);
}

inline
void BdemEncoderOptions::setBdemVersion(int value)
{
    d_bdemVersion = value;
}

// ACCESSORS
template <class STREAM>
inline
STREAM& BdemEncoderOptions::bdexStreamOut(STREAM& stream,
                                               int version) const
{
    switch (version) {
      case 1: {
        bdex_OutStreamFunctions::streamOut(stream, d_bdemVersion, 1);
      } break;
    }
    return stream;
}

template <class ACCESSOR>
inline
int BdemEncoderOptions::accessAttributes(ACCESSOR& accessor) const
{
    int ret;

    ret = accessor(d_bdemVersion,
                   ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_BDEM_VERSION]);
    if (ret) {
        return ret;
    }

    return ret;
}

template <class ACCESSOR>
inline
int BdemEncoderOptions::accessAttribute(ACCESSOR& accessor, int id) const
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_BDEM_VERSION: {
        return accessor(d_bdemVersion,
                        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_BDEM_VERSION]);
      } break;
      default:
        return NOT_FOUND;
    }
}

template <class ACCESSOR>
inline
int BdemEncoderOptions::accessAttribute(
        ACCESSOR&   accessor,
        const char *name,
        int         nameLength) const
{
    enum { NOT_FOUND = -1 };

    const bdeat_AttributeInfo *attributeInfo =
          lookupAttributeInfo(name, nameLength);
    if (0 == attributeInfo) {
       return NOT_FOUND;
    }

    return accessAttribute(accessor, attributeInfo->d_id);
}

inline
const int& BdemEncoderOptions::bdemVersion() const
{
    return d_bdemVersion;
}
}  // close package namespace


// FREE FUNCTIONS

inline
bool bdlmxxx::operator==(
        const BdemEncoderOptions& lhs,
        const BdemEncoderOptions& rhs)
{
    return  lhs.bdemVersion() == rhs.bdemVersion();
}

inline
bool bdlmxxx::operator!=(
        const BdemEncoderOptions& lhs,
        const BdemEncoderOptions& rhs)
{
    return  lhs.bdemVersion() != rhs.bdemVersion();
}

inline
bsl::ostream& bdlmxxx::operator<<(
        bsl::ostream& stream,
        const BdemEncoderOptions& rhs)
{
    return rhs.print(stream, 0, -1);
}

}  // close namespace BloombergLP
#endif

// GENERATED BY BLP_BAS_CODEGEN_3.0.19 Thu Jul 31 13:45:53 2008
// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2008
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ------------------------------ END-OF-FILE ---------------------------------
