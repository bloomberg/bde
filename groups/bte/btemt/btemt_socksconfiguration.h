// btemt_socksconfiguration.h   -*-C++-*-   GENERATED FILE -- DO NOT EDIT
#ifndef INCLUDED_BTEMT_SOCKSCONFIGURATION
#define INCLUDED_BTEMT_SOCKSCONFIGURATION

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT_RCSID(bbcn_socksconfiguration_h,"$Id$ $CSID$ $CCId$")
BDES_IDENT_PRAGMA_ONCE

//@PURPOSE: Provide value-semantic attribute classes
//
//@AUTHOR: UCORE

#ifndef INCLUDED_BDEALG_TYPETRAITS
#include <bdealg_typetraits.h>
#endif

#ifndef INCLUDED_BDEAT_ATTRIBUTEINFO
#include <bdeat_attributeinfo.h>
#endif

#ifndef INCLUDED_BDEAT_SELECTIONINFO
#include <bdeat_selectioninfo.h>
#endif

#ifndef INCLUDED_BDEAT_TYPETRAITS
#include <bdeat_typetraits.h>
#endif

#ifndef INCLUDED_BDES_OBJECTBUFFER
#include <bdes_objectbuffer.h>
#endif

#ifndef INCLUDED_BDEX_INSTREAMFUNCTIONS
#include <bdex_instreamfunctions.h>
#endif

#ifndef INCLUDED_BDEX_OUTSTREAMFUNCTIONS
#include <bdex_outstreamfunctions.h>
#endif

#ifndef INCLUDED_BSLMA_DEFAULT
#include <bslma_default.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSL_STRING
#include <bsl_string.h>
#endif

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#define INCLUDED_BSL_IOSFWD
#endif

namespace BloombergLP {

namespace bslma {class Allocator;}

namespace btemt { class SocksConfiguration; }
namespace btemt {

                          // ========================                          
                          // class SocksConfiguration                          
                          // ========================                          

class SocksConfiguration {

    // INSTANCE DATA
    bsl::string  d_destinationIpAddr;
    bsl::string  d_socksIpAddr;
    bsl::string  d_username;
    bsl::string  d_password;
    int          d_destinationPort;
    int          d_socksPort;
    int          d_timeout;
    bool         d_doAuthentication;

  public:
    // TYPES
    enum {
        ATTRIBUTE_ID_DESTINATION_IP_ADDR = 0
      , ATTRIBUTE_ID_DESTINATION_PORT    = 1
      , ATTRIBUTE_ID_SOCKS_IP_ADDR       = 2
      , ATTRIBUTE_ID_SOCKS_PORT          = 3
      , ATTRIBUTE_ID_TIMEOUT             = 4
      , ATTRIBUTE_ID_DO_AUTHENTICATION   = 5
      , ATTRIBUTE_ID_USERNAME            = 6
      , ATTRIBUTE_ID_PASSWORD            = 7
    };

    enum {
        NUM_ATTRIBUTES = 8
    };

    enum {
        ATTRIBUTE_INDEX_DESTINATION_IP_ADDR = 0
      , ATTRIBUTE_INDEX_DESTINATION_PORT    = 1
      , ATTRIBUTE_INDEX_SOCKS_IP_ADDR       = 2
      , ATTRIBUTE_INDEX_SOCKS_PORT          = 3
      , ATTRIBUTE_INDEX_TIMEOUT             = 4
      , ATTRIBUTE_INDEX_DO_AUTHENTICATION   = 5
      , ATTRIBUTE_INDEX_USERNAME            = 6
      , ATTRIBUTE_INDEX_PASSWORD            = 7
    };

    // CONSTANTS
    static const char CLASS_NAME[];

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
    explicit SocksConfiguration(bslma::Allocator *basicAllocator = 0);
        // Create an object of type 'SocksConfiguration' having the default
        // value.  Use the optionally specified 'basicAllocator' to supply
        // memory.  If 'basicAllocator' is 0, the currently installed default
        // allocator is used.

    SocksConfiguration(const SocksConfiguration& original,
                       bslma::Allocator *basicAllocator = 0);
        // Create an object of type 'SocksConfiguration' having the value of
        // the specified 'original' object.  Use the optionally specified
        // 'basicAllocator' to supply memory.  If 'basicAllocator' is 0, the
        // currently installed default allocator is used.

    ~SocksConfiguration();
        // Destroy this object.

    // MANIPULATORS
    SocksConfiguration& operator=(const SocksConfiguration& rhs);
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

    bsl::string& destinationIpAddr();
        // Return a reference to the modifiable "DestinationIpAddr" attribute
        // of this object.

    int& destinationPort();
        // Return a reference to the modifiable "DestinationPort" attribute of
        // this object.

    bsl::string& socksIpAddr();
        // Return a reference to the modifiable "SocksIpAddr" attribute of this
        // object.

    int& socksPort();
        // Return a reference to the modifiable "SocksPort" attribute of this
        // object.

    int& timeout();
        // Return a reference to the modifiable "Timeout" attribute of this
        // object.

    bool& doAuthentication();
        // Return a reference to the modifiable "DoAuthentication" attribute of
        // this object.

    bsl::string& username();
        // Return a reference to the modifiable "Username" attribute of this
        // object.

    bsl::string& password();
        // Return a reference to the modifiable "Password" attribute of this
        // object.

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

    const bsl::string& destinationIpAddr() const;
        // Return a reference to the non-modifiable "DestinationIpAddr"
        // attribute of this object.

    const int& destinationPort() const;
        // Return a reference to the non-modifiable "DestinationPort" attribute
        // of this object.

    const bsl::string& socksIpAddr() const;
        // Return a reference to the non-modifiable "SocksIpAddr" attribute of
        // this object.

    const int& socksPort() const;
        // Return a reference to the non-modifiable "SocksPort" attribute of
        // this object.

    const int& timeout() const;
        // Return a reference to the non-modifiable "Timeout" attribute of this
        // object.

    const bool& doAuthentication() const;
        // Return a reference to the non-modifiable "DoAuthentication"
        // attribute of this object.

    const bsl::string& username() const;
        // Return a reference to the non-modifiable "Username" attribute of
        // this object.

    const bsl::string& password() const;
        // Return a reference to the non-modifiable "Password" attribute of
        // this object.
};

// FREE OPERATORS
inline
bool operator==(const SocksConfiguration& lhs, const SocksConfiguration& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
    // the same value, and 'false' otherwise.  Two attribute objects have the
    // same value if each respective attribute has the same value.

inline
bool operator!=(const SocksConfiguration& lhs, const SocksConfiguration& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
    // have the same value, and 'false' otherwise.  Two attribute objects do
    // not have the same value if one or more respective attributes differ in
    // values.

inline
bsl::ostream& operator<<(bsl::ostream& stream, const SocksConfiguration& rhs);
    // Format the specified 'rhs' to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.

}  // close namespace bbcn

// TRAITS

BDEAT_DECL_SEQUENCE_WITH_ALLOCATOR_BITWISEMOVEABLE_TRAITS(btemt::SocksConfiguration)

namespace btemt {

                          // ========================                           
                          // class Socksconfiguration                           
                          // ========================                           

struct Socksconfiguration {
    // This class serves as a place holder to reserve a type having the same
    // name as this component.  Doing so ensures that such a type cannot be
    // defined outside of this component in the current namespace.
};

}  // close namespace bbcn

// ============================================================================
//                         INLINE FUNCTION DEFINITIONS
// ============================================================================

namespace btemt {

                          // ------------------------                          
                          // class SocksConfiguration                          
                          // ------------------------                          

// CLASS METHODS
inline
int SocksConfiguration::maxSupportedBdexVersion()
{
    return 1;  // versions start at 1.
}

// MANIPULATORS
template <class STREAM>
STREAM& SocksConfiguration::bdexStreamIn(STREAM& stream, int version)
{
    if (stream) {
        switch (version) {
          case 1: {
            bdex_InStreamFunctions::streamIn(stream, d_destinationIpAddr, 1);
            bdex_InStreamFunctions::streamIn(stream, d_destinationPort, 1);
            bdex_InStreamFunctions::streamIn(stream, d_socksIpAddr, 1);
            bdex_InStreamFunctions::streamIn(stream, d_socksPort, 1);
            bdex_InStreamFunctions::streamIn(stream, d_timeout, 1);
            bdex_InStreamFunctions::streamIn(stream, d_doAuthentication, 1);
            bdex_InStreamFunctions::streamIn(stream, d_username, 1);
            bdex_InStreamFunctions::streamIn(stream, d_password, 1);
          } break;
          default: {
            stream.invalidate();
          }
        }
    }
    return stream;
}

template <class MANIPULATOR>
int SocksConfiguration::manipulateAttributes(MANIPULATOR& manipulator)
{
    int ret;

    ret = manipulator(&d_destinationIpAddr, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_DESTINATION_IP_ADDR]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_destinationPort, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_DESTINATION_PORT]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_socksIpAddr, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SOCKS_IP_ADDR]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_socksPort, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SOCKS_PORT]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_timeout, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_TIMEOUT]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_doAuthentication, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_DO_AUTHENTICATION]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_username, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_USERNAME]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_password, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_PASSWORD]);
    if (ret) {
        return ret;
    }

    return ret;
}

template <class MANIPULATOR>
int SocksConfiguration::manipulateAttribute(MANIPULATOR& manipulator, int id)
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_DESTINATION_IP_ADDR: {
        return manipulator(&d_destinationIpAddr, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_DESTINATION_IP_ADDR]);
      } break;
      case ATTRIBUTE_ID_DESTINATION_PORT: {
        return manipulator(&d_destinationPort, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_DESTINATION_PORT]);
      } break;
      case ATTRIBUTE_ID_SOCKS_IP_ADDR: {
        return manipulator(&d_socksIpAddr, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SOCKS_IP_ADDR]);
      } break;
      case ATTRIBUTE_ID_SOCKS_PORT: {
        return manipulator(&d_socksPort, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SOCKS_PORT]);
      } break;
      case ATTRIBUTE_ID_TIMEOUT: {
        return manipulator(&d_timeout, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_TIMEOUT]);
      } break;
      case ATTRIBUTE_ID_DO_AUTHENTICATION: {
        return manipulator(&d_doAuthentication, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_DO_AUTHENTICATION]);
      } break;
      case ATTRIBUTE_ID_USERNAME: {
        return manipulator(&d_username, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_USERNAME]);
      } break;
      case ATTRIBUTE_ID_PASSWORD: {
        return manipulator(&d_password, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_PASSWORD]);
      } break;
      default:
        return NOT_FOUND;
    }
}

template <class MANIPULATOR>
int SocksConfiguration::manipulateAttribute(
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
bsl::string& SocksConfiguration::destinationIpAddr()
{
    return d_destinationIpAddr;
}

inline
int& SocksConfiguration::destinationPort()
{
    return d_destinationPort;
}

inline
bsl::string& SocksConfiguration::socksIpAddr()
{
    return d_socksIpAddr;
}

inline
int& SocksConfiguration::socksPort()
{
    return d_socksPort;
}

inline
int& SocksConfiguration::timeout()
{
    return d_timeout;
}

inline
bool& SocksConfiguration::doAuthentication()
{
    return d_doAuthentication;
}

inline
bsl::string& SocksConfiguration::username()
{
    return d_username;
}

inline
bsl::string& SocksConfiguration::password()
{
    return d_password;
}

// ACCESSORS
template <class STREAM>
STREAM& SocksConfiguration::bdexStreamOut(STREAM& stream, int version) const
{
    switch (version) {
      case 1: {
        bdex_OutStreamFunctions::streamOut(stream, d_destinationIpAddr, 1);
        bdex_OutStreamFunctions::streamOut(stream, d_destinationPort, 1);
        bdex_OutStreamFunctions::streamOut(stream, d_socksIpAddr, 1);
        bdex_OutStreamFunctions::streamOut(stream, d_socksPort, 1);
        bdex_OutStreamFunctions::streamOut(stream, d_timeout, 1);
        bdex_OutStreamFunctions::streamOut(stream, d_doAuthentication, 1);
        bdex_OutStreamFunctions::streamOut(stream, d_username, 1);
        bdex_OutStreamFunctions::streamOut(stream, d_password, 1);
      } break;
    }
    return stream;
}

template <class ACCESSOR>
int SocksConfiguration::accessAttributes(ACCESSOR& accessor) const
{
    int ret;

    ret = accessor(d_destinationIpAddr, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_DESTINATION_IP_ADDR]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_destinationPort, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_DESTINATION_PORT]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_socksIpAddr, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SOCKS_IP_ADDR]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_socksPort, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SOCKS_PORT]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_timeout, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_TIMEOUT]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_doAuthentication, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_DO_AUTHENTICATION]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_username, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_USERNAME]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_password, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_PASSWORD]);
    if (ret) {
        return ret;
    }

    return ret;
}

template <class ACCESSOR>
int SocksConfiguration::accessAttribute(ACCESSOR& accessor, int id) const
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_DESTINATION_IP_ADDR: {
        return accessor(d_destinationIpAddr, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_DESTINATION_IP_ADDR]);
      } break;
      case ATTRIBUTE_ID_DESTINATION_PORT: {
        return accessor(d_destinationPort, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_DESTINATION_PORT]);
      } break;
      case ATTRIBUTE_ID_SOCKS_IP_ADDR: {
        return accessor(d_socksIpAddr, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SOCKS_IP_ADDR]);
      } break;
      case ATTRIBUTE_ID_SOCKS_PORT: {
        return accessor(d_socksPort, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SOCKS_PORT]);
      } break;
      case ATTRIBUTE_ID_TIMEOUT: {
        return accessor(d_timeout, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_TIMEOUT]);
      } break;
      case ATTRIBUTE_ID_DO_AUTHENTICATION: {
        return accessor(d_doAuthentication, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_DO_AUTHENTICATION]);
      } break;
      case ATTRIBUTE_ID_USERNAME: {
        return accessor(d_username, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_USERNAME]);
      } break;
      case ATTRIBUTE_ID_PASSWORD: {
        return accessor(d_password, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_PASSWORD]);
      } break;
      default:
        return NOT_FOUND;
    }
}

template <class ACCESSOR>
int SocksConfiguration::accessAttribute(
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
const bsl::string& SocksConfiguration::destinationIpAddr() const
{
    return d_destinationIpAddr;
}

inline
const int& SocksConfiguration::destinationPort() const
{
    return d_destinationPort;
}

inline
const bsl::string& SocksConfiguration::socksIpAddr() const
{
    return d_socksIpAddr;
}

inline
const int& SocksConfiguration::socksPort() const
{
    return d_socksPort;
}

inline
const int& SocksConfiguration::timeout() const
{
    return d_timeout;
}

inline
const bool& SocksConfiguration::doAuthentication() const
{
    return d_doAuthentication;
}

inline
const bsl::string& SocksConfiguration::username() const
{
    return d_username;
}

inline
const bsl::string& SocksConfiguration::password() const
{
    return d_password;
}

}  // close namespace bbcn

// FREE FUNCTIONS

inline
bool btemt::operator==(
        const btemt::SocksConfiguration& lhs,
        const btemt::SocksConfiguration& rhs)
{
    return  lhs.destinationIpAddr() == rhs.destinationIpAddr()
         && lhs.destinationPort() == rhs.destinationPort()
         && lhs.socksIpAddr() == rhs.socksIpAddr()
         && lhs.socksPort() == rhs.socksPort()
         && lhs.timeout() == rhs.timeout()
         && lhs.doAuthentication() == rhs.doAuthentication()
         && lhs.username() == rhs.username()
         && lhs.password() == rhs.password();
}

inline
bool btemt::operator!=(
        const btemt::SocksConfiguration& lhs,
        const btemt::SocksConfiguration& rhs)
{
    return  lhs.destinationIpAddr() != rhs.destinationIpAddr()
         || lhs.destinationPort() != rhs.destinationPort()
         || lhs.socksIpAddr() != rhs.socksIpAddr()
         || lhs.socksPort() != rhs.socksPort()
         || lhs.timeout() != rhs.timeout()
         || lhs.doAuthentication() != rhs.doAuthentication()
         || lhs.username() != rhs.username()
         || lhs.password() != rhs.password();
}

inline
bsl::ostream& btemt::operator<<(
        bsl::ostream& stream,
        const btemt::SocksConfiguration& rhs)
{
    return rhs.print(stream, 0, -1);
}

}  // close namespace BloombergLP
#endif

// GENERATED BY BLP_BAS_CODEGEN_3.5.3 
// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2010
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ------------------------------ END-OF-FILE ---------------------------------
