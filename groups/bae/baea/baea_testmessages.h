// baea_testmessages.h       -- GENERATED FILE - DO NOT EDIT --       -*-C++-*-
#ifndef INCLUDED_BAEA_TESTMESSAGES
#define INCLUDED_BAEA_TESTMESSAGES

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT_RCSID(baea_testmessages_h,"$Id$ $CSID$")
BDES_IDENT_PRAGMA_ONCE

//@PURPOSE: Provide value-semantic attribute classes
//
//@AUTHOR: Raymond Seehei CHIU (schiu49@bloomberg.net)

#ifndef INCLUDED_BCEM_AGGREGATE
#include <bcem_aggregate.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITS
#include <bslalg_typetraits.h>
#endif

#ifndef INCLUDED_BDEAT_ATTRIBUTEINFO
#include <bdeat_attributeinfo.h>
#endif

#ifndef INCLUDED_BDEAT_ENUMERATORINFO
#include <bdeat_enumeratorinfo.h>
#endif

#ifndef INCLUDED_BDEAT_SELECTIONINFO
#include <bdeat_selectioninfo.h>
#endif

#ifndef INCLUDED_BDEAT_TYPETRAITS
#include <bdeat_typetraits.h>
#endif

#ifndef INCLUDED_BDEAT_VALUETYPEFUNCTIONS
#include <bdeat_valuetypefunctions.h>
#endif

#ifndef INCLUDED_BSLS_OBJECTBUFFER
#include <bsls_objectbuffer.h>
#endif

#ifndef INCLUDED_BDEX_INSTREAMFUNCTIONS
#include <bdex_instreamfunctions.h>
#endif

#ifndef INCLUDED_BDEX_OUTSTREAMFUNCTIONS
#include <bdex_outstreamfunctions.h>
#endif

#ifndef INCLUDED_BDEMA_DEFAULT
#include <bdema_default.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BDES_PLATFORMUTIL
#include <bdes_platformutil.h>
#endif

#ifndef INCLUDED_BDET_DATETIMETZ
#include <bdet_datetimetz.h>
#endif

#ifndef INCLUDED_BDEUT_NULLABLEALLOCATEDVALUE
#include <bdeut_nullableallocatedvalue.h>
#endif

#ifndef INCLUDED_BDEUT_NULLABLEVALUE
#include <bdeut_nullablevalue.h>
#endif

#ifndef INCLUDED_STRING
#include <string>
#endif

#ifndef INCLUDED_VECTOR
#include <vector>
#endif

#ifndef INCLUDED_IOSFWD
#include <iosfwd>
#endif

#ifndef INCLUDED_OSTREAM
#include <ostream>
#endif

#ifndef INCLUDED_BSLFWD_BSLMA_ALLOCATOR
#include <bslfwd_bslma_allocator.h>
#endif

namespace BloombergLP {

namespace baea { class CustomInt; }
namespace baea { class CustomString; }
namespace baea { class SimpleRequest; }
namespace baea { class UnsignedSequence; }
namespace baea { class VoidSequence; }
namespace baea { class Sequence3; }
namespace baea { class Sequence5; }
namespace baea { class Sequence6; }
namespace baea { class Choice3; }
namespace baea { class Choice1; }
namespace baea { class Choice2; }
namespace baea { class Sequence4; }
namespace baea { class Sequence1; }
namespace baea { class Sequence2; }
namespace baea { class FeatureTestMessage; }
namespace baea { class Request; }
namespace baea { class Response; }
namespace baea {

                              // ===============                               
                              // class CustomInt                               
                              // ===============                               

class CustomInt {

    // INSTANCE DATA
    int d_value;

    // FRIENDS
    friend bool operator==(const CustomInt& lhs, const CustomInt& rhs);
    friend bool operator!=(const CustomInt& lhs, const CustomInt& rhs);

    // PRIVATE CLASS METHODS
    static int checkRestrictions(const int& value);
        // Check if the specified 'value' satisfies the restrictions of this
        // class (i.e., "CustomInt").  Return 0 if successful (i.e., the
        // restrictions are satisfied) and non-zero otherwise.

  public:
    // TYPES
    typedef int BaseType;

    // CONSTANTS
    static const char CLASS_NAME[];

    // CREATORS
    CustomInt();
        // Create an object of type 'CustomInt' having the default value.

    CustomInt(const CustomInt& original);
        // Create an object of type 'CustomInt' having the value of the
        // specified 'original' object.

    explicit CustomInt(const int& value);
        // Create an object of type 'CustomInt' having the specified 'value'.

    ~CustomInt();
        // Destroy this object.

    // MANIPULATORS
    CustomInt& operator=(const CustomInt& rhs);
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

    int fromInt(const int& value);
        // Convert from the specified 'value' to this type.  Return 0 if
        // successful and non-zero otherwise.

    // ACCESSORS
    template <class STREAM>
    STREAM& bdexStreamOut(STREAM& stream, int version) const;
        // Write the value of this object to the specified output 'stream'
        // using the specified 'version' format and return a reference to the
        // modifiable 'stream'.  If 'version' is not supported, 'stream' is
        // unmodified.  Note that 'version' is not written to 'stream'.
        // See the 'bdex' package-level documentation for more information
        // on 'bdex' streaming of value-semantic types and containers.

    int maxSupportedBdexVersion() const;
        // Return the most current 'bdex' streaming version number supported by
        // this class.  See the 'bdex' package-level documentation for more
        // information on 'bdex' streaming of value-semantic types and
        // containers.

    std::ostream& print(std::ostream& stream,
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

    const int& toInt() const;
        // Convert this value to 'int'.
};

// FREE OPERATORS
inline
bool operator==(const CustomInt& lhs, const CustomInt& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
    // the same value, and 'false' otherwise.  Two attribute objects have the
    // same value if each respective attribute has the same value.

inline
bool operator!=(const CustomInt& lhs, const CustomInt& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
    // have the same value, and 'false' otherwise.  Two attribute objects do
    // not have the same value if one or more respective attributes differ in
    // values.

inline
std::ostream& operator<<(std::ostream& stream, const CustomInt& rhs);
    // Format the specified 'rhs' to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.

}  // close namespace baea

// TRAITS

BDEAT_DECL_CUSTOMIZEDTYPE_WITH_BITWISEMOVEABLE_TRAITS(baea::CustomInt)

namespace baea {

                             // ==================                             
                             // class CustomString                             
                             // ==================                             

class CustomString {

    // INSTANCE DATA
    std::string d_value;

    // FRIENDS
    friend bool operator==(const CustomString& lhs, const CustomString& rhs);
    friend bool operator!=(const CustomString& lhs, const CustomString& rhs);

    // PRIVATE CLASS METHODS
    static int checkRestrictions(const std::string& value);
        // Check if the specified 'value' satisfies the restrictions of this
        // class (i.e., "CustomString").  Return 0 if successful (i.e., the
        // restrictions are satisfied) and non-zero otherwise.

  public:
    // TYPES
    typedef std::string BaseType;

    // CONSTANTS
    static const char CLASS_NAME[];

    // CREATORS
    explicit CustomString(bdema_Allocator *basicAllocator = 0);
        // Create an object of type 'CustomString' having the default value. 
        // Use the optionally specified 'basicAllocator' to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.

    CustomString(const CustomString& original,
                bdema_Allocator *basicAllocator = 0);
        // Create an object of type 'CustomString' having the value
        // of the specified 'original' object.  Use the optionally specified
        // 'basicAllocator' to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.

    explicit CustomString(const std::string& value,
                         bdema_Allocator *basicAllocator = 0);
        // Create an object of type 'CustomString' having the specified
        // 'value'.  Use the optionally specified 'basicAllocator' to supply
        // memory.  If 'basicAllocator' is 0, the currently installed default
        // allocator is used.

    ~CustomString();
        // Destroy this object.

    // MANIPULATORS
    CustomString& operator=(const CustomString& rhs);
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

    int fromString(const std::string& value);
        // Convert from the specified 'value' to this type.  Return 0 if
        // successful and non-zero otherwise.

    // ACCESSORS
    template <class STREAM>
    STREAM& bdexStreamOut(STREAM& stream, int version) const;
        // Write the value of this object to the specified output 'stream'
        // using the specified 'version' format and return a reference to the
        // modifiable 'stream'.  If 'version' is not supported, 'stream' is
        // unmodified.  Note that 'version' is not written to 'stream'.
        // See the 'bdex' package-level documentation for more information
        // on 'bdex' streaming of value-semantic types and containers.

    int maxSupportedBdexVersion() const;
        // Return the most current 'bdex' streaming version number supported by
        // this class.  See the 'bdex' package-level documentation for more
        // information on 'bdex' streaming of value-semantic types and
        // containers.

    std::ostream& print(std::ostream& stream,
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

    const std::string& toString() const;
        // Convert this value to 'std::string'.
};

// FREE OPERATORS
inline
bool operator==(const CustomString& lhs, const CustomString& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
    // the same value, and 'false' otherwise.  Two attribute objects have the
    // same value if each respective attribute has the same value.

inline
bool operator!=(const CustomString& lhs, const CustomString& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
    // have the same value, and 'false' otherwise.  Two attribute objects do
    // not have the same value if one or more respective attributes differ in
    // values.

inline
std::ostream& operator<<(std::ostream& stream, const CustomString& rhs);
    // Format the specified 'rhs' to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.

}  // close namespace baea

// TRAITS

BDEAT_DECL_CUSTOMIZEDTYPE_WITH_ALLOCATOR_BITWISEMOVEABLE_TRAITS(baea::CustomString)

namespace baea {

                              // ================                              
                              // class Enumerated                              
                              // ================                              

struct Enumerated {

  public:
    // TYPES
    enum Value {
        NEW_YORK   = 0
      , NEW_JERSEY = 1
      , LONDON     = 2
    };

    enum {
        NUM_ENUMERATORS = 3
    };

    // CONSTANTS
    static const char CLASS_NAME[];

    static const bdeat_EnumeratorInfo ENUMERATOR_INFO_ARRAY[];

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
                          const std::string&  string);
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

    static std::ostream& print(std::ostream& stream, Value value);
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
std::ostream& operator<<(std::ostream& stream, Enumerated::Value rhs);
    // Format the specified 'rhs' to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.

}  // close namespace baea

// TRAITS

BDEAT_DECL_ENUMERATION_TRAITS(baea::Enumerated)


namespace baea {

                            // ===================                             
                            // class SimpleRequest                             
                            // ===================                             

class SimpleRequest {

    // INSTANCE DATA
    std::string  d_data;
    int          d_responseLength;

  public:
    // TYPES
    enum {
        ATTRIBUTE_ID_DATA            = 0
      , ATTRIBUTE_ID_RESPONSE_LENGTH = 1
    };

    enum {
        NUM_ATTRIBUTES = 2
    };

    enum {
        ATTRIBUTE_INDEX_DATA            = 0
      , ATTRIBUTE_INDEX_RESPONSE_LENGTH = 1
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
    explicit SimpleRequest(bdema_Allocator *basicAllocator = 0);
        // Create an object of type 'SimpleRequest' having the default value. 
        // Use the optionally specified 'basicAllocator' to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.

    SimpleRequest(const SimpleRequest& original,
                  bdema_Allocator *basicAllocator = 0);
        // Create an object of type 'SimpleRequest' having the value of the
        // specified 'original' object.  Use the optionally specified
        // 'basicAllocator' to supply memory.  If 'basicAllocator' is 0, the
        // currently installed default allocator is used.

    ~SimpleRequest();
        // Destroy this object.

    // MANIPULATORS
    SimpleRequest& operator=(const SimpleRequest& rhs);
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

    std::string& data();
        // Return a reference to the modifiable "Data" attribute of this
        // object.

    int& responseLength();
        // Return a reference to the modifiable "ResponseLength" attribute of
        // this object.

    // ACCESSORS
    std::ostream& print(std::ostream& stream,
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

    const std::string& data() const;
        // Return a reference to the non-modifiable "Data" attribute of this
        // object.

    int responseLength() const;
        // Return a reference to the non-modifiable "ResponseLength" attribute
        // of this object.
};

// FREE OPERATORS
inline
bool operator==(const SimpleRequest& lhs, const SimpleRequest& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
    // the same value, and 'false' otherwise.  Two attribute objects have the
    // same value if each respective attribute has the same value.

inline
bool operator!=(const SimpleRequest& lhs, const SimpleRequest& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
    // have the same value, and 'false' otherwise.  Two attribute objects do
    // not have the same value if one or more respective attributes differ in
    // values.

inline
std::ostream& operator<<(std::ostream& stream, const SimpleRequest& rhs);
    // Format the specified 'rhs' to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.

}  // close namespace baea

// TRAITS

BDEAT_DECL_SEQUENCE_WITH_ALLOCATOR_BITWISEMOVEABLE_TRAITS(baea::SimpleRequest)

namespace baea {

                           // ======================                           
                           // class UnsignedSequence                           
                           // ======================                           

class UnsignedSequence {

    // INSTANCE DATA
    bsls_Types::Uint64  d_element3;
    unsigned int        d_element1;
    unsigned short      d_element2;

  public:
    // TYPES
    enum {
        ATTRIBUTE_ID_ELEMENT1 = 0
      , ATTRIBUTE_ID_ELEMENT2 = 1
      , ATTRIBUTE_ID_ELEMENT3 = 2
    };

    enum {
        NUM_ATTRIBUTES = 3
    };

    enum {
        ATTRIBUTE_INDEX_ELEMENT1 = 0
      , ATTRIBUTE_INDEX_ELEMENT2 = 1
      , ATTRIBUTE_INDEX_ELEMENT3 = 2
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
    UnsignedSequence();
        // Create an object of type 'UnsignedSequence' having the default
        // value.

    UnsignedSequence(const UnsignedSequence& original);
        // Create an object of type 'UnsignedSequence' having the value of the
        // specified 'original' object.

    ~UnsignedSequence();
        // Destroy this object.

    // MANIPULATORS
    UnsignedSequence& operator=(const UnsignedSequence& rhs);
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

    unsigned int& element1();
        // Return a reference to the modifiable "Element1" attribute of this
        // object.

    unsigned short& element2();
        // Return a reference to the modifiable "Element2" attribute of this
        // object.

    bsls_Types::Uint64& element3();
        // Return a reference to the modifiable "Element3" attribute of this
        // object.

    // ACCESSORS
    std::ostream& print(std::ostream& stream,
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

    unsigned int element1() const;
        // Return a reference to the non-modifiable "Element1" attribute of
        // this object.

    unsigned short element2() const;
        // Return a reference to the non-modifiable "Element2" attribute of
        // this object.

    bsls_Types::Uint64 element3() const;
        // Return a reference to the non-modifiable "Element3" attribute of
        // this object.
};

// FREE OPERATORS
inline
bool operator==(const UnsignedSequence& lhs, const UnsignedSequence& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
    // the same value, and 'false' otherwise.  Two attribute objects have the
    // same value if each respective attribute has the same value.

inline
bool operator!=(const UnsignedSequence& lhs, const UnsignedSequence& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
    // have the same value, and 'false' otherwise.  Two attribute objects do
    // not have the same value if one or more respective attributes differ in
    // values.

inline
std::ostream& operator<<(std::ostream& stream, const UnsignedSequence& rhs);
    // Format the specified 'rhs' to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.

}  // close namespace baea

// TRAITS

BDEAT_DECL_SEQUENCE_WITH_BITWISEMOVEABLE_TRAITS(baea::UnsignedSequence)

namespace baea {

                             // ==================                             
                             // class VoidSequence                             
                             // ==================                             

class VoidSequence {

    // INSTANCE DATA

  public:
    // TYPES
    enum {
        NUM_ATTRIBUTES = 0
    };


    // CONSTANTS
    static const char CLASS_NAME[];

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
    VoidSequence();
        // Create an object of type 'VoidSequence' having the default value.

    VoidSequence(const VoidSequence& original);
        // Create an object of type 'VoidSequence' having the value of the
        // specified 'original' object.

    ~VoidSequence();
        // Destroy this object.

    // MANIPULATORS
    VoidSequence& operator=(const VoidSequence& rhs);
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

    // ACCESSORS
    std::ostream& print(std::ostream& stream,
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
};

// FREE OPERATORS
inline
bool operator==(const VoidSequence& lhs, const VoidSequence& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
    // the same value, and 'false' otherwise.  Two attribute objects have the
    // same value if each respective attribute has the same value.

inline
bool operator!=(const VoidSequence& lhs, const VoidSequence& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
    // have the same value, and 'false' otherwise.  Two attribute objects do
    // not have the same value if one or more respective attributes differ in
    // values.

inline
std::ostream& operator<<(std::ostream& stream, const VoidSequence& rhs);
    // Format the specified 'rhs' to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.

}  // close namespace baea

// TRAITS

BDEAT_DECL_SEQUENCE_WITH_BITWISEMOVEABLE_TRAITS(baea::VoidSequence)

namespace baea {

                              // ===============                               
                              // class Sequence3                               
                              // ===============                               

class Sequence3 {

    // INSTANCE DATA
    std::vector<std::string>                              d_element2;
    bdeut_NullableValue<std::string>                      d_element4;
    bdeut_NullableAllocatedValue<Sequence5>               d_element5;
    std::vector<bdeut_NullableValue<Enumerated::Value> >  d_element6;
    std::vector<Enumerated::Value>                        d_element1;
    bdeut_NullableValue<bool>                             d_element3;

  public:
    // TYPES
    enum {
        ATTRIBUTE_ID_ELEMENT1 = 0
      , ATTRIBUTE_ID_ELEMENT2 = 1
      , ATTRIBUTE_ID_ELEMENT3 = 2
      , ATTRIBUTE_ID_ELEMENT4 = 3
      , ATTRIBUTE_ID_ELEMENT5 = 4
      , ATTRIBUTE_ID_ELEMENT6 = 5
    };

    enum {
        NUM_ATTRIBUTES = 6
    };

    enum {
        ATTRIBUTE_INDEX_ELEMENT1 = 0
      , ATTRIBUTE_INDEX_ELEMENT2 = 1
      , ATTRIBUTE_INDEX_ELEMENT3 = 2
      , ATTRIBUTE_INDEX_ELEMENT4 = 3
      , ATTRIBUTE_INDEX_ELEMENT5 = 4
      , ATTRIBUTE_INDEX_ELEMENT6 = 5
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
    explicit Sequence3(bdema_Allocator *basicAllocator = 0);
        // Create an object of type 'Sequence3' having the default value.  Use
        // the optionally specified 'basicAllocator' to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.

    Sequence3(const Sequence3& original,
              bdema_Allocator *basicAllocator = 0);
        // Create an object of type 'Sequence3' having the value of the
        // specified 'original' object.  Use the optionally specified
        // 'basicAllocator' to supply memory.  If 'basicAllocator' is 0, the
        // currently installed default allocator is used.

    ~Sequence3();
        // Destroy this object.

    // MANIPULATORS
    Sequence3& operator=(const Sequence3& rhs);
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

    std::vector<Enumerated::Value>& element1();
        // Return a reference to the modifiable "Element1" attribute of this
        // object.

    std::vector<std::string>& element2();
        // Return a reference to the modifiable "Element2" attribute of this
        // object.

    bdeut_NullableValue<bool>& element3();
        // Return a reference to the modifiable "Element3" attribute of this
        // object.

    bdeut_NullableValue<std::string>& element4();
        // Return a reference to the modifiable "Element4" attribute of this
        // object.

    bdeut_NullableAllocatedValue<Sequence5>& element5();
        // Return a reference to the modifiable "Element5" attribute of this
        // object.

    std::vector<bdeut_NullableValue<Enumerated::Value> >& element6();
        // Return a reference to the modifiable "Element6" attribute of this
        // object.

    // ACCESSORS
    std::ostream& print(std::ostream& stream,
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

    const std::vector<Enumerated::Value>& element1() const;
        // Return a reference to the non-modifiable "Element1" attribute of
        // this object.

    const std::vector<std::string>& element2() const;
        // Return a reference to the non-modifiable "Element2" attribute of
        // this object.

    const bdeut_NullableValue<bool>& element3() const;
        // Return a reference to the non-modifiable "Element3" attribute of
        // this object.

    const bdeut_NullableValue<std::string>& element4() const;
        // Return a reference to the non-modifiable "Element4" attribute of
        // this object.

    const bdeut_NullableAllocatedValue<Sequence5>& element5() const;
        // Return a reference to the non-modifiable "Element5" attribute of
        // this object.

    const std::vector<bdeut_NullableValue<Enumerated::Value> >& element6() const;
        // Return a reference to the non-modifiable "Element6" attribute of
        // this object.
};

// FREE OPERATORS
inline
bool operator==(const Sequence3& lhs, const Sequence3& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
    // the same value, and 'false' otherwise.  Two attribute objects have the
    // same value if each respective attribute has the same value.

inline
bool operator!=(const Sequence3& lhs, const Sequence3& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
    // have the same value, and 'false' otherwise.  Two attribute objects do
    // not have the same value if one or more respective attributes differ in
    // values.

inline
std::ostream& operator<<(std::ostream& stream, const Sequence3& rhs);
    // Format the specified 'rhs' to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.

}  // close namespace baea

// TRAITS

BDEAT_DECL_SEQUENCE_WITH_ALLOCATOR_BITWISEMOVEABLE_TRAITS(baea::Sequence3)

namespace baea {

                              // ===============                               
                              // class Sequence5                               
                              // ===============                               

class Sequence5 {

    // INSTANCE DATA
    bdema_Allocator                                       *d_allocator_p;
    std::vector<bdeut_NullableValue<int> >                 d_element5;
    std::vector<bdeut_NullableValue<double> >              d_element3;
    std::vector<bdeut_NullableValue<std::vector<char> > >  d_element4;
    std::vector<bdeut_NullableValue<bool> >                d_element2;
    std::vector<bdeut_NullableValue<bdet_DatetimeTz> >     d_element6;
    std::vector<bdeut_NullableAllocatedValue<Sequence3> >  d_element7;
    Sequence3                                             *d_element1;

  public:
    // TYPES
    enum {
        ATTRIBUTE_ID_ELEMENT1 = 0
      , ATTRIBUTE_ID_ELEMENT2 = 1
      , ATTRIBUTE_ID_ELEMENT3 = 2
      , ATTRIBUTE_ID_ELEMENT4 = 3
      , ATTRIBUTE_ID_ELEMENT5 = 4
      , ATTRIBUTE_ID_ELEMENT6 = 5
      , ATTRIBUTE_ID_ELEMENT7 = 6
    };

    enum {
        NUM_ATTRIBUTES = 7
    };

    enum {
        ATTRIBUTE_INDEX_ELEMENT1 = 0
      , ATTRIBUTE_INDEX_ELEMENT2 = 1
      , ATTRIBUTE_INDEX_ELEMENT3 = 2
      , ATTRIBUTE_INDEX_ELEMENT4 = 3
      , ATTRIBUTE_INDEX_ELEMENT5 = 4
      , ATTRIBUTE_INDEX_ELEMENT6 = 5
      , ATTRIBUTE_INDEX_ELEMENT7 = 6
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
    explicit Sequence5(bdema_Allocator *basicAllocator = 0);
        // Create an object of type 'Sequence5' having the default value.  Use
        // the optionally specified 'basicAllocator' to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.

    Sequence5(const Sequence5& original,
              bdema_Allocator *basicAllocator = 0);
        // Create an object of type 'Sequence5' having the value of the
        // specified 'original' object.  Use the optionally specified
        // 'basicAllocator' to supply memory.  If 'basicAllocator' is 0, the
        // currently installed default allocator is used.

    ~Sequence5();
        // Destroy this object.

    // MANIPULATORS
    Sequence5& operator=(const Sequence5& rhs);
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

    Sequence3& element1();
        // Return a reference to the modifiable "Element1" attribute of this
        // object.

    std::vector<bdeut_NullableValue<bool> >& element2();
        // Return a reference to the modifiable "Element2" attribute of this
        // object.

    std::vector<bdeut_NullableValue<double> >& element3();
        // Return a reference to the modifiable "Element3" attribute of this
        // object.

    std::vector<bdeut_NullableValue<std::vector<char> > >& element4();
        // Return a reference to the modifiable "Element4" attribute of this
        // object.

    std::vector<bdeut_NullableValue<int> >& element5();
        // Return a reference to the modifiable "Element5" attribute of this
        // object.

    std::vector<bdeut_NullableValue<bdet_DatetimeTz> >& element6();
        // Return a reference to the modifiable "Element6" attribute of this
        // object.

    std::vector<bdeut_NullableAllocatedValue<Sequence3> >& element7();
        // Return a reference to the modifiable "Element7" attribute of this
        // object.

    // ACCESSORS
    std::ostream& print(std::ostream& stream,
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

    const Sequence3& element1() const;
        // Return a reference to the non-modifiable "Element1" attribute of
        // this object.

    const std::vector<bdeut_NullableValue<bool> >& element2() const;
        // Return a reference to the non-modifiable "Element2" attribute of
        // this object.

    const std::vector<bdeut_NullableValue<double> >& element3() const;
        // Return a reference to the non-modifiable "Element3" attribute of
        // this object.

    const std::vector<bdeut_NullableValue<std::vector<char> > >& element4() const;
        // Return a reference to the non-modifiable "Element4" attribute of
        // this object.

    const std::vector<bdeut_NullableValue<int> >& element5() const;
        // Return a reference to the non-modifiable "Element5" attribute of
        // this object.

    const std::vector<bdeut_NullableValue<bdet_DatetimeTz> >& element6() const;
        // Return a reference to the non-modifiable "Element6" attribute of
        // this object.

    const std::vector<bdeut_NullableAllocatedValue<Sequence3> >& element7() const;
        // Return a reference to the non-modifiable "Element7" attribute of
        // this object.
};

// FREE OPERATORS
inline
bool operator==(const Sequence5& lhs, const Sequence5& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
    // the same value, and 'false' otherwise.  Two attribute objects have the
    // same value if each respective attribute has the same value.

inline
bool operator!=(const Sequence5& lhs, const Sequence5& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
    // have the same value, and 'false' otherwise.  Two attribute objects do
    // not have the same value if one or more respective attributes differ in
    // values.

inline
std::ostream& operator<<(std::ostream& stream, const Sequence5& rhs);
    // Format the specified 'rhs' to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.

}  // close namespace baea

// TRAITS

BDEAT_DECL_SEQUENCE_WITH_ALLOCATOR_BITWISEMOVEABLE_TRAITS(baea::Sequence5)

namespace baea {

                              // ===============                               
                              // class Sequence6                               
                              // ===============                               

class Sequence6 {

    // INSTANCE DATA
    std::vector<unsigned char>                        d_element8;
    std::vector<bdeut_NullableValue<unsigned char> >  d_element10;
    std::vector<CustomString>                         d_element9;
    bdeut_NullableValue<CustomString>                 d_element2;
    CustomString                                      d_element6;
    std::vector<bdeut_NullableValue<CustomInt> >      d_element5;
    std::vector<CustomInt>                            d_element11;
    bdeut_NullableValue<CustomInt>                    d_element3;
    CustomInt                                         d_element7;
    unsigned char                                     d_element4;
    bdeut_NullableValue<unsigned char>                d_element1;

  public:
    // TYPES
    enum {
        ATTRIBUTE_ID_ELEMENT1  = 0
      , ATTRIBUTE_ID_ELEMENT2  = 1
      , ATTRIBUTE_ID_ELEMENT3  = 2
      , ATTRIBUTE_ID_ELEMENT4  = 3
      , ATTRIBUTE_ID_ELEMENT5  = 4
      , ATTRIBUTE_ID_ELEMENT6  = 5
      , ATTRIBUTE_ID_ELEMENT7  = 6
      , ATTRIBUTE_ID_ELEMENT8  = 7
      , ATTRIBUTE_ID_ELEMENT9  = 8
      , ATTRIBUTE_ID_ELEMENT10 = 9
      , ATTRIBUTE_ID_ELEMENT11 = 10
    };

    enum {
        NUM_ATTRIBUTES = 11
    };

    enum {
        ATTRIBUTE_INDEX_ELEMENT1  = 0
      , ATTRIBUTE_INDEX_ELEMENT2  = 1
      , ATTRIBUTE_INDEX_ELEMENT3  = 2
      , ATTRIBUTE_INDEX_ELEMENT4  = 3
      , ATTRIBUTE_INDEX_ELEMENT5  = 4
      , ATTRIBUTE_INDEX_ELEMENT6  = 5
      , ATTRIBUTE_INDEX_ELEMENT7  = 6
      , ATTRIBUTE_INDEX_ELEMENT8  = 7
      , ATTRIBUTE_INDEX_ELEMENT9  = 8
      , ATTRIBUTE_INDEX_ELEMENT10 = 9
      , ATTRIBUTE_INDEX_ELEMENT11 = 10
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
    explicit Sequence6(bdema_Allocator *basicAllocator = 0);
        // Create an object of type 'Sequence6' having the default value.  Use
        // the optionally specified 'basicAllocator' to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.

    Sequence6(const Sequence6& original,
              bdema_Allocator *basicAllocator = 0);
        // Create an object of type 'Sequence6' having the value of the
        // specified 'original' object.  Use the optionally specified
        // 'basicAllocator' to supply memory.  If 'basicAllocator' is 0, the
        // currently installed default allocator is used.

    ~Sequence6();
        // Destroy this object.

    // MANIPULATORS
    Sequence6& operator=(const Sequence6& rhs);
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

    bdeut_NullableValue<unsigned char>& element1();
        // Return a reference to the modifiable "Element1" attribute of this
        // object.

    bdeut_NullableValue<CustomString>& element2();
        // Return a reference to the modifiable "Element2" attribute of this
        // object.

    bdeut_NullableValue<CustomInt>& element3();
        // Return a reference to the modifiable "Element3" attribute of this
        // object.

    unsigned char& element4();
        // Return a reference to the modifiable "Element4" attribute of this
        // object.

    std::vector<bdeut_NullableValue<CustomInt> >& element5();
        // Return a reference to the modifiable "Element5" attribute of this
        // object.

    CustomString& element6();
        // Return a reference to the modifiable "Element6" attribute of this
        // object.

    CustomInt& element7();
        // Return a reference to the modifiable "Element7" attribute of this
        // object.

    std::vector<unsigned char>& element8();
        // Return a reference to the modifiable "Element8" attribute of this
        // object.

    std::vector<CustomString>& element9();
        // Return a reference to the modifiable "Element9" attribute of this
        // object.

    std::vector<bdeut_NullableValue<unsigned char> >& element10();
        // Return a reference to the modifiable "Element10" attribute of this
        // object.

    std::vector<CustomInt>& element11();
        // Return a reference to the modifiable "Element11" attribute of this
        // object.

    // ACCESSORS
    std::ostream& print(std::ostream& stream,
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

    const bdeut_NullableValue<unsigned char>& element1() const;
        // Return a reference to the non-modifiable "Element1" attribute of
        // this object.

    const bdeut_NullableValue<CustomString>& element2() const;
        // Return a reference to the non-modifiable "Element2" attribute of
        // this object.

    const bdeut_NullableValue<CustomInt>& element3() const;
        // Return a reference to the non-modifiable "Element3" attribute of
        // this object.

    unsigned char element4() const;
        // Return a reference to the non-modifiable "Element4" attribute of
        // this object.

    const std::vector<bdeut_NullableValue<CustomInt> >& element5() const;
        // Return a reference to the non-modifiable "Element5" attribute of
        // this object.

    const CustomString& element6() const;
        // Return a reference to the non-modifiable "Element6" attribute of
        // this object.

    const CustomInt& element7() const;
        // Return a reference to the non-modifiable "Element7" attribute of
        // this object.

    const std::vector<unsigned char>& element8() const;
        // Return a reference to the non-modifiable "Element8" attribute of
        // this object.

    const std::vector<CustomString>& element9() const;
        // Return a reference to the non-modifiable "Element9" attribute of
        // this object.

    const std::vector<bdeut_NullableValue<unsigned char> >& element10() const;
        // Return a reference to the non-modifiable "Element10" attribute of
        // this object.

    const std::vector<CustomInt>& element11() const;
        // Return a reference to the non-modifiable "Element11" attribute of
        // this object.
};

// FREE OPERATORS
inline
bool operator==(const Sequence6& lhs, const Sequence6& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
    // the same value, and 'false' otherwise.  Two attribute objects have the
    // same value if each respective attribute has the same value.

inline
bool operator!=(const Sequence6& lhs, const Sequence6& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
    // have the same value, and 'false' otherwise.  Two attribute objects do
    // not have the same value if one or more respective attributes differ in
    // values.

inline
std::ostream& operator<<(std::ostream& stream, const Sequence6& rhs);
    // Format the specified 'rhs' to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.

}  // close namespace baea

// TRAITS

BDEAT_DECL_SEQUENCE_WITH_ALLOCATOR_BITWISEMOVEABLE_TRAITS(baea::Sequence6)

namespace baea {

                               // =============                                
                               // class Choice3                                
                               // =============                                

class Choice3 {

    // INSTANCE DATA
    union {
        bsls_ObjectBuffer< Sequence6 >     d_selection1;
        bsls_ObjectBuffer< unsigned char > d_selection2;
        bsls_ObjectBuffer< CustomString >  d_selection3;
        bsls_ObjectBuffer< CustomInt >     d_selection4;
    };

    int                                    d_selectionId;
    bdema_Allocator                       *d_allocator_p;

  public:
    // TYPES
    enum {
        SELECTION_ID_UNDEFINED  = -1

      , SELECTION_ID_SELECTION1 = 0
      , SELECTION_ID_SELECTION2 = 1
      , SELECTION_ID_SELECTION3 = 2
      , SELECTION_ID_SELECTION4 = 3
    };

    enum {
        NUM_SELECTIONS = 4
    };

    enum {
        SELECTION_INDEX_SELECTION1 = 0
      , SELECTION_INDEX_SELECTION2 = 1
      , SELECTION_INDEX_SELECTION3 = 2
      , SELECTION_INDEX_SELECTION4 = 3
    };

    // CONSTANTS
    static const char CLASS_NAME[];

    static const bdeat_SelectionInfo SELECTION_INFO_ARRAY[];

    // CLASS METHODS
    static int maxSupportedBdexVersion();
        // Return the most current 'bdex' streaming version number supported by
        // this class.  See the 'bdex' package-level documentation for more
        // information on 'bdex' streaming of value-semantic types and
        // containers.

    static const bdeat_SelectionInfo *lookupSelectionInfo(int id);
        // Return selection information for the selection indicated by the
        // specified 'id' if the selection exists, and 0 otherwise.

    static const bdeat_SelectionInfo *lookupSelectionInfo(
                                                    const char *name,
                                                    int         nameLength);
        // Return selection information for the selection indicated by the
        // specified 'name' of the specified 'nameLength' if the selection
        // exists, and 0 otherwise.

    // CREATORS
    explicit Choice3(bdema_Allocator *basicAllocator = 0);
        // Create an object of type 'Choice3' having the default value.  Use
        // the optionally specified 'basicAllocator' to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.

    Choice3(const Choice3& original,
           bdema_Allocator *basicAllocator = 0);
        // Create an object of type 'Choice3' having the value of the specified
        // 'original' object.  Use the optionally specified 'basicAllocator' to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.

    ~Choice3();
        // Destroy this object.

    // MANIPULATORS
    Choice3& operator=(const Choice3& rhs);
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
        // Reset this object to the default value (i.e., its value upon default
        // construction).

    int makeSelection(int selectionId);
        // Set the value of this object to be the default for the selection
        // indicated by the specified 'selectionId'.  Return 0 on success, and
        // non-zero value otherwise (i.e., the selection is not found).

    int makeSelection(const char *name, int nameLength);
        // Set the value of this object to be the default for the selection
        // indicated by the specified 'name' of the specified 'nameLength'.
        // Return 0 on success, and non-zero value otherwise (i.e., the
        // selection is not found).

    Sequence6& makeSelection1();
    Sequence6& makeSelection1(const Sequence6& value);
        // Set the value of this object to be a "Selection1" value.  Optionally
        // specify the 'value' of the "Selection1".  If 'value' is not
        // specified, the default "Selection1" value is used.

    unsigned char& makeSelection2();
    unsigned char& makeSelection2(unsigned char value);
        // Set the value of this object to be a "Selection2" value.  Optionally
        // specify the 'value' of the "Selection2".  If 'value' is not
        // specified, the default "Selection2" value is used.

    CustomString& makeSelection3();
    CustomString& makeSelection3(const CustomString& value);
        // Set the value of this object to be a "Selection3" value.  Optionally
        // specify the 'value' of the "Selection3".  If 'value' is not
        // specified, the default "Selection3" value is used.

    CustomInt& makeSelection4();
    CustomInt& makeSelection4(const CustomInt& value);
        // Set the value of this object to be a "Selection4" value.  Optionally
        // specify the 'value' of the "Selection4".  If 'value' is not
        // specified, the default "Selection4" value is used.

    template<class MANIPULATOR>
    int manipulateSelection(MANIPULATOR& manipulator);
        // Invoke the specified 'manipulator' on the address of the modifiable
        // selection, supplying 'manipulator' with the corresponding selection
        // information structure.  Return the value returned from the
        // invocation of 'manipulator' if this object has a defined selection,
        // and -1 otherwise.

    Sequence6& selection1();
        // Return a reference to the modifiable "Selection1" selection of this
        // object if "Selection1" is the current selection.  The behavior is
        // undefined unless "Selection1" is the selection of this object.

    unsigned char& selection2();
        // Return a reference to the modifiable "Selection2" selection of this
        // object if "Selection2" is the current selection.  The behavior is
        // undefined unless "Selection2" is the selection of this object.

    CustomString& selection3();
        // Return a reference to the modifiable "Selection3" selection of this
        // object if "Selection3" is the current selection.  The behavior is
        // undefined unless "Selection3" is the selection of this object.

    CustomInt& selection4();
        // Return a reference to the modifiable "Selection4" selection of this
        // object if "Selection4" is the current selection.  The behavior is
        // undefined unless "Selection4" is the selection of this object.

    // ACCESSORS
    std::ostream& print(std::ostream& stream,
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

    int selectionId() const;
        // Return the id of the current selection if the selection is defined,
        // and -1 otherwise.

    template<class ACCESSOR>
    int accessSelection(ACCESSOR& accessor) const;
        // Invoke the specified 'accessor' on the non-modifiable selection,
        // supplying 'accessor' with the corresponding selection information
        // structure.  Return the value returned from the invocation of
        // 'accessor' if this object has a defined selection, and -1 otherwise.

    const Sequence6& selection1() const;
        // Return a reference to the non-modifiable "Selection1" selection of
        // this object if "Selection1" is the current selection.  The behavior
        // is undefined unless "Selection1" is the selection of this object.

    const unsigned char& selection2() const;
        // Return a reference to the non-modifiable "Selection2" selection of
        // this object if "Selection2" is the current selection.  The behavior
        // is undefined unless "Selection2" is the selection of this object.

    const CustomString& selection3() const;
        // Return a reference to the non-modifiable "Selection3" selection of
        // this object if "Selection3" is the current selection.  The behavior
        // is undefined unless "Selection3" is the selection of this object.

    const CustomInt& selection4() const;
        // Return a reference to the non-modifiable "Selection4" selection of
        // this object if "Selection4" is the current selection.  The behavior
        // is undefined unless "Selection4" is the selection of this object.

    bool isSelection1Value() const;
        // Return 'true' if the value of this object is a "Selection1" value,
        // and return 'false' otherwise.

    bool isSelection2Value() const;
        // Return 'true' if the value of this object is a "Selection2" value,
        // and return 'false' otherwise.

    bool isSelection3Value() const;
        // Return 'true' if the value of this object is a "Selection3" value,
        // and return 'false' otherwise.

    bool isSelection4Value() const;
        // Return 'true' if the value of this object is a "Selection4" value,
        // and return 'false' otherwise.

    bool isUndefinedValue() const;
        // Return 'true' if the value of this object is undefined, and 'false'
        // otherwise.

    const char *selectionName() const;
        // Return the symbolic name of the current selection of this object.
};

// FREE OPERATORS
inline
bool operator==(const Choice3& lhs, const Choice3& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects have the same
    // value, and 'false' otherwise.  Two 'Choice3' objects have the same
    // value if either the selections in both objects have the same ids and
    // the same values, or both selections are undefined.

inline
bool operator!=(const Choice3& lhs, const Choice3& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects do not have the
    // same values, as determined by 'operator==', and 'false' otherwise.

inline
std::ostream& operator<<(std::ostream& stream, const Choice3& rhs);
    // Format the specified 'rhs' to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.

}  // close namespace baea

// TRAITS

BDEAT_DECL_CHOICE_WITH_ALLOCATOR_BITWISEMOVEABLE_TRAITS(baea::Choice3)

namespace baea {

                               // =============                                
                               // class Choice1                                
                               // =============                                

class Choice1 {

    // INSTANCE DATA
    union {
        bsls_ObjectBuffer< int >     d_selection1;
        bsls_ObjectBuffer< double >  d_selection2;
        Sequence4                   *d_selection3;
        Choice2                     *d_selection4;
    };

    int                              d_selectionId;
    bdema_Allocator                 *d_allocator_p;

  public:
    // TYPES
    enum {
        SELECTION_ID_UNDEFINED  = -1

      , SELECTION_ID_SELECTION1 = 0
      , SELECTION_ID_SELECTION2 = 1
      , SELECTION_ID_SELECTION3 = 2
      , SELECTION_ID_SELECTION4 = 3
    };

    enum {
        NUM_SELECTIONS = 4
    };

    enum {
        SELECTION_INDEX_SELECTION1 = 0
      , SELECTION_INDEX_SELECTION2 = 1
      , SELECTION_INDEX_SELECTION3 = 2
      , SELECTION_INDEX_SELECTION4 = 3
    };

    // CONSTANTS
    static const char CLASS_NAME[];

    static const bdeat_SelectionInfo SELECTION_INFO_ARRAY[];

    // CLASS METHODS
    static int maxSupportedBdexVersion();
        // Return the most current 'bdex' streaming version number supported by
        // this class.  See the 'bdex' package-level documentation for more
        // information on 'bdex' streaming of value-semantic types and
        // containers.

    static const bdeat_SelectionInfo *lookupSelectionInfo(int id);
        // Return selection information for the selection indicated by the
        // specified 'id' if the selection exists, and 0 otherwise.

    static const bdeat_SelectionInfo *lookupSelectionInfo(
                                                    const char *name,
                                                    int         nameLength);
        // Return selection information for the selection indicated by the
        // specified 'name' of the specified 'nameLength' if the selection
        // exists, and 0 otherwise.

    // CREATORS
    explicit Choice1(bdema_Allocator *basicAllocator = 0);
        // Create an object of type 'Choice1' having the default value.  Use
        // the optionally specified 'basicAllocator' to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.

    Choice1(const Choice1& original,
           bdema_Allocator *basicAllocator = 0);
        // Create an object of type 'Choice1' having the value of the specified
        // 'original' object.  Use the optionally specified 'basicAllocator' to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.

    ~Choice1();
        // Destroy this object.

    // MANIPULATORS
    Choice1& operator=(const Choice1& rhs);
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
        // Reset this object to the default value (i.e., its value upon default
        // construction).

    int makeSelection(int selectionId);
        // Set the value of this object to be the default for the selection
        // indicated by the specified 'selectionId'.  Return 0 on success, and
        // non-zero value otherwise (i.e., the selection is not found).

    int makeSelection(const char *name, int nameLength);
        // Set the value of this object to be the default for the selection
        // indicated by the specified 'name' of the specified 'nameLength'.
        // Return 0 on success, and non-zero value otherwise (i.e., the
        // selection is not found).

    int& makeSelection1();
    int& makeSelection1(int value);
        // Set the value of this object to be a "Selection1" value.  Optionally
        // specify the 'value' of the "Selection1".  If 'value' is not
        // specified, the default "Selection1" value is used.

    double& makeSelection2();
    double& makeSelection2(double value);
        // Set the value of this object to be a "Selection2" value.  Optionally
        // specify the 'value' of the "Selection2".  If 'value' is not
        // specified, the default "Selection2" value is used.

    Sequence4& makeSelection3();
    Sequence4& makeSelection3(const Sequence4& value);
        // Set the value of this object to be a "Selection3" value.  Optionally
        // specify the 'value' of the "Selection3".  If 'value' is not
        // specified, the default "Selection3" value is used.

    Choice2& makeSelection4();
    Choice2& makeSelection4(const Choice2& value);
        // Set the value of this object to be a "Selection4" value.  Optionally
        // specify the 'value' of the "Selection4".  If 'value' is not
        // specified, the default "Selection4" value is used.

    template<class MANIPULATOR>
    int manipulateSelection(MANIPULATOR& manipulator);
        // Invoke the specified 'manipulator' on the address of the modifiable
        // selection, supplying 'manipulator' with the corresponding selection
        // information structure.  Return the value returned from the
        // invocation of 'manipulator' if this object has a defined selection,
        // and -1 otherwise.

    int& selection1();
        // Return a reference to the modifiable "Selection1" selection of this
        // object if "Selection1" is the current selection.  The behavior is
        // undefined unless "Selection1" is the selection of this object.

    double& selection2();
        // Return a reference to the modifiable "Selection2" selection of this
        // object if "Selection2" is the current selection.  The behavior is
        // undefined unless "Selection2" is the selection of this object.

    Sequence4& selection3();
        // Return a reference to the modifiable "Selection3" selection of this
        // object if "Selection3" is the current selection.  The behavior is
        // undefined unless "Selection3" is the selection of this object.

    Choice2& selection4();
        // Return a reference to the modifiable "Selection4" selection of this
        // object if "Selection4" is the current selection.  The behavior is
        // undefined unless "Selection4" is the selection of this object.

    // ACCESSORS
    std::ostream& print(std::ostream& stream,
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

    int selectionId() const;
        // Return the id of the current selection if the selection is defined,
        // and -1 otherwise.

    template<class ACCESSOR>
    int accessSelection(ACCESSOR& accessor) const;
        // Invoke the specified 'accessor' on the non-modifiable selection,
        // supplying 'accessor' with the corresponding selection information
        // structure.  Return the value returned from the invocation of
        // 'accessor' if this object has a defined selection, and -1 otherwise.

    const int& selection1() const;
        // Return a reference to the non-modifiable "Selection1" selection of
        // this object if "Selection1" is the current selection.  The behavior
        // is undefined unless "Selection1" is the selection of this object.

    const double& selection2() const;
        // Return a reference to the non-modifiable "Selection2" selection of
        // this object if "Selection2" is the current selection.  The behavior
        // is undefined unless "Selection2" is the selection of this object.

    const Sequence4& selection3() const;
        // Return a reference to the non-modifiable "Selection3" selection of
        // this object if "Selection3" is the current selection.  The behavior
        // is undefined unless "Selection3" is the selection of this object.

    const Choice2& selection4() const;
        // Return a reference to the non-modifiable "Selection4" selection of
        // this object if "Selection4" is the current selection.  The behavior
        // is undefined unless "Selection4" is the selection of this object.

    bool isSelection1Value() const;
        // Return 'true' if the value of this object is a "Selection1" value,
        // and return 'false' otherwise.

    bool isSelection2Value() const;
        // Return 'true' if the value of this object is a "Selection2" value,
        // and return 'false' otherwise.

    bool isSelection3Value() const;
        // Return 'true' if the value of this object is a "Selection3" value,
        // and return 'false' otherwise.

    bool isSelection4Value() const;
        // Return 'true' if the value of this object is a "Selection4" value,
        // and return 'false' otherwise.

    bool isUndefinedValue() const;
        // Return 'true' if the value of this object is undefined, and 'false'
        // otherwise.

    const char *selectionName() const;
        // Return the symbolic name of the current selection of this object.
};

// FREE OPERATORS
inline
bool operator==(const Choice1& lhs, const Choice1& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects have the same
    // value, and 'false' otherwise.  Two 'Choice1' objects have the same
    // value if either the selections in both objects have the same ids and
    // the same values, or both selections are undefined.

inline
bool operator!=(const Choice1& lhs, const Choice1& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects do not have the
    // same values, as determined by 'operator==', and 'false' otherwise.

inline
std::ostream& operator<<(std::ostream& stream, const Choice1& rhs);
    // Format the specified 'rhs' to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.

}  // close namespace baea

// TRAITS

BDEAT_DECL_CHOICE_WITH_ALLOCATOR_BITWISEMOVEABLE_TRAITS(baea::Choice1)

namespace baea {

                               // =============                                
                               // class Choice2                                
                               // =============                                

class Choice2 {

    // INSTANCE DATA
    union {
        bsls_ObjectBuffer< bool >         d_selection1;
        bsls_ObjectBuffer< std::string >  d_selection2;
        Choice1                          *d_selection3;
    };

    int                                   d_selectionId;
    bdema_Allocator                      *d_allocator_p;

  public:
    // TYPES
    enum {
        SELECTION_ID_UNDEFINED  = -1

      , SELECTION_ID_SELECTION1 = 0
      , SELECTION_ID_SELECTION2 = 1
      , SELECTION_ID_SELECTION3 = 2
    };

    enum {
        NUM_SELECTIONS = 3
    };

    enum {
        SELECTION_INDEX_SELECTION1 = 0
      , SELECTION_INDEX_SELECTION2 = 1
      , SELECTION_INDEX_SELECTION3 = 2
    };

    // CONSTANTS
    static const char CLASS_NAME[];

    static const bdeat_SelectionInfo SELECTION_INFO_ARRAY[];

    // CLASS METHODS
    static int maxSupportedBdexVersion();
        // Return the most current 'bdex' streaming version number supported by
        // this class.  See the 'bdex' package-level documentation for more
        // information on 'bdex' streaming of value-semantic types and
        // containers.

    static const bdeat_SelectionInfo *lookupSelectionInfo(int id);
        // Return selection information for the selection indicated by the
        // specified 'id' if the selection exists, and 0 otherwise.

    static const bdeat_SelectionInfo *lookupSelectionInfo(
                                                    const char *name,
                                                    int         nameLength);
        // Return selection information for the selection indicated by the
        // specified 'name' of the specified 'nameLength' if the selection
        // exists, and 0 otherwise.

    // CREATORS
    explicit Choice2(bdema_Allocator *basicAllocator = 0);
        // Create an object of type 'Choice2' having the default value.  Use
        // the optionally specified 'basicAllocator' to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.

    Choice2(const Choice2& original,
           bdema_Allocator *basicAllocator = 0);
        // Create an object of type 'Choice2' having the value of the specified
        // 'original' object.  Use the optionally specified 'basicAllocator' to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.

    ~Choice2();
        // Destroy this object.

    // MANIPULATORS
    Choice2& operator=(const Choice2& rhs);
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
        // Reset this object to the default value (i.e., its value upon default
        // construction).

    int makeSelection(int selectionId);
        // Set the value of this object to be the default for the selection
        // indicated by the specified 'selectionId'.  Return 0 on success, and
        // non-zero value otherwise (i.e., the selection is not found).

    int makeSelection(const char *name, int nameLength);
        // Set the value of this object to be the default for the selection
        // indicated by the specified 'name' of the specified 'nameLength'.
        // Return 0 on success, and non-zero value otherwise (i.e., the
        // selection is not found).

    bool& makeSelection1();
    bool& makeSelection1(bool value);
        // Set the value of this object to be a "Selection1" value.  Optionally
        // specify the 'value' of the "Selection1".  If 'value' is not
        // specified, the default "Selection1" value is used.

    std::string& makeSelection2();
    std::string& makeSelection2(const std::string& value);
        // Set the value of this object to be a "Selection2" value.  Optionally
        // specify the 'value' of the "Selection2".  If 'value' is not
        // specified, the default "Selection2" value is used.

    Choice1& makeSelection3();
    Choice1& makeSelection3(const Choice1& value);
        // Set the value of this object to be a "Selection3" value.  Optionally
        // specify the 'value' of the "Selection3".  If 'value' is not
        // specified, the default "Selection3" value is used.

    template<class MANIPULATOR>
    int manipulateSelection(MANIPULATOR& manipulator);
        // Invoke the specified 'manipulator' on the address of the modifiable
        // selection, supplying 'manipulator' with the corresponding selection
        // information structure.  Return the value returned from the
        // invocation of 'manipulator' if this object has a defined selection,
        // and -1 otherwise.

    bool& selection1();
        // Return a reference to the modifiable "Selection1" selection of this
        // object if "Selection1" is the current selection.  The behavior is
        // undefined unless "Selection1" is the selection of this object.

    std::string& selection2();
        // Return a reference to the modifiable "Selection2" selection of this
        // object if "Selection2" is the current selection.  The behavior is
        // undefined unless "Selection2" is the selection of this object.

    Choice1& selection3();
        // Return a reference to the modifiable "Selection3" selection of this
        // object if "Selection3" is the current selection.  The behavior is
        // undefined unless "Selection3" is the selection of this object.

    // ACCESSORS
    std::ostream& print(std::ostream& stream,
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

    int selectionId() const;
        // Return the id of the current selection if the selection is defined,
        // and -1 otherwise.

    template<class ACCESSOR>
    int accessSelection(ACCESSOR& accessor) const;
        // Invoke the specified 'accessor' on the non-modifiable selection,
        // supplying 'accessor' with the corresponding selection information
        // structure.  Return the value returned from the invocation of
        // 'accessor' if this object has a defined selection, and -1 otherwise.

    const bool& selection1() const;
        // Return a reference to the non-modifiable "Selection1" selection of
        // this object if "Selection1" is the current selection.  The behavior
        // is undefined unless "Selection1" is the selection of this object.

    const std::string& selection2() const;
        // Return a reference to the non-modifiable "Selection2" selection of
        // this object if "Selection2" is the current selection.  The behavior
        // is undefined unless "Selection2" is the selection of this object.

    const Choice1& selection3() const;
        // Return a reference to the non-modifiable "Selection3" selection of
        // this object if "Selection3" is the current selection.  The behavior
        // is undefined unless "Selection3" is the selection of this object.

    bool isSelection1Value() const;
        // Return 'true' if the value of this object is a "Selection1" value,
        // and return 'false' otherwise.

    bool isSelection2Value() const;
        // Return 'true' if the value of this object is a "Selection2" value,
        // and return 'false' otherwise.

    bool isSelection3Value() const;
        // Return 'true' if the value of this object is a "Selection3" value,
        // and return 'false' otherwise.

    bool isUndefinedValue() const;
        // Return 'true' if the value of this object is undefined, and 'false'
        // otherwise.

    const char *selectionName() const;
        // Return the symbolic name of the current selection of this object.
};

// FREE OPERATORS
inline
bool operator==(const Choice2& lhs, const Choice2& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects have the same
    // value, and 'false' otherwise.  Two 'Choice2' objects have the same
    // value if either the selections in both objects have the same ids and
    // the same values, or both selections are undefined.

inline
bool operator!=(const Choice2& lhs, const Choice2& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects do not have the
    // same values, as determined by 'operator==', and 'false' otherwise.

inline
std::ostream& operator<<(std::ostream& stream, const Choice2& rhs);
    // Format the specified 'rhs' to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.

}  // close namespace baea

// TRAITS

BDEAT_DECL_CHOICE_WITH_ALLOCATOR_BITWISEMOVEABLE_TRAITS(baea::Choice2)

namespace baea {

                              // ===============                               
                              // class Sequence4                               
                              // ===============                               

class Sequence4 {

    // INSTANCE DATA
    double                                   d_element10;
    std::vector<int>                         d_element17;
    std::vector<double>                      d_element15;
    std::vector<char>                        d_element11;
    std::vector<std::vector<char> >          d_element16;
    std::vector<bool>                        d_element14;
    std::vector<bdet_DatetimeTz>             d_element18;
    std::vector<Sequence3>                   d_element1;
    std::vector<CustomString>                d_element19;
    std::vector<Choice1>                     d_element2;
    std::string                              d_element9;
    bdeut_NullableValue<std::vector<char> >  d_element3;
    bdeut_NullableValue<bdet_DatetimeTz>     d_element5;
    bdeut_NullableValue<CustomString>        d_element6;
    int                                      d_element12;
    bdeut_NullableValue<int>                 d_element4;
    bdeut_NullableValue<Enumerated::Value>   d_element7;
    Enumerated::Value                        d_element13;
    bool                                     d_element8;

  public:
    // TYPES
    enum {
        ATTRIBUTE_ID_ELEMENT1  = 0
      , ATTRIBUTE_ID_ELEMENT2  = 1
      , ATTRIBUTE_ID_ELEMENT3  = 2
      , ATTRIBUTE_ID_ELEMENT4  = 3
      , ATTRIBUTE_ID_ELEMENT5  = 4
      , ATTRIBUTE_ID_ELEMENT6  = 5
      , ATTRIBUTE_ID_ELEMENT7  = 6
      , ATTRIBUTE_ID_ELEMENT8  = 7
      , ATTRIBUTE_ID_ELEMENT9  = 8
      , ATTRIBUTE_ID_ELEMENT10 = 9
      , ATTRIBUTE_ID_ELEMENT11 = 10
      , ATTRIBUTE_ID_ELEMENT12 = 11
      , ATTRIBUTE_ID_ELEMENT13 = 12
      , ATTRIBUTE_ID_ELEMENT14 = 13
      , ATTRIBUTE_ID_ELEMENT15 = 14
      , ATTRIBUTE_ID_ELEMENT16 = 15
      , ATTRIBUTE_ID_ELEMENT17 = 16
      , ATTRIBUTE_ID_ELEMENT18 = 17
      , ATTRIBUTE_ID_ELEMENT19 = 18
    };

    enum {
        NUM_ATTRIBUTES = 19
    };

    enum {
        ATTRIBUTE_INDEX_ELEMENT1  = 0
      , ATTRIBUTE_INDEX_ELEMENT2  = 1
      , ATTRIBUTE_INDEX_ELEMENT3  = 2
      , ATTRIBUTE_INDEX_ELEMENT4  = 3
      , ATTRIBUTE_INDEX_ELEMENT5  = 4
      , ATTRIBUTE_INDEX_ELEMENT6  = 5
      , ATTRIBUTE_INDEX_ELEMENT7  = 6
      , ATTRIBUTE_INDEX_ELEMENT8  = 7
      , ATTRIBUTE_INDEX_ELEMENT9  = 8
      , ATTRIBUTE_INDEX_ELEMENT10 = 9
      , ATTRIBUTE_INDEX_ELEMENT11 = 10
      , ATTRIBUTE_INDEX_ELEMENT12 = 11
      , ATTRIBUTE_INDEX_ELEMENT13 = 12
      , ATTRIBUTE_INDEX_ELEMENT14 = 13
      , ATTRIBUTE_INDEX_ELEMENT15 = 14
      , ATTRIBUTE_INDEX_ELEMENT16 = 15
      , ATTRIBUTE_INDEX_ELEMENT17 = 16
      , ATTRIBUTE_INDEX_ELEMENT18 = 17
      , ATTRIBUTE_INDEX_ELEMENT19 = 18
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
    explicit Sequence4(bdema_Allocator *basicAllocator = 0);
        // Create an object of type 'Sequence4' having the default value.  Use
        // the optionally specified 'basicAllocator' to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.

    Sequence4(const Sequence4& original,
              bdema_Allocator *basicAllocator = 0);
        // Create an object of type 'Sequence4' having the value of the
        // specified 'original' object.  Use the optionally specified
        // 'basicAllocator' to supply memory.  If 'basicAllocator' is 0, the
        // currently installed default allocator is used.

    ~Sequence4();
        // Destroy this object.

    // MANIPULATORS
    Sequence4& operator=(const Sequence4& rhs);
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

    std::vector<Sequence3>& element1();
        // Return a reference to the modifiable "Element1" attribute of this
        // object.

    std::vector<Choice1>& element2();
        // Return a reference to the modifiable "Element2" attribute of this
        // object.

    bdeut_NullableValue<std::vector<char> >& element3();
        // Return a reference to the modifiable "Element3" attribute of this
        // object.

    bdeut_NullableValue<int>& element4();
        // Return a reference to the modifiable "Element4" attribute of this
        // object.

    bdeut_NullableValue<bdet_DatetimeTz>& element5();
        // Return a reference to the modifiable "Element5" attribute of this
        // object.

    bdeut_NullableValue<CustomString>& element6();
        // Return a reference to the modifiable "Element6" attribute of this
        // object.

    bdeut_NullableValue<Enumerated::Value>& element7();
        // Return a reference to the modifiable "Element7" attribute of this
        // object.

    bool& element8();
        // Return a reference to the modifiable "Element8" attribute of this
        // object.

    std::string& element9();
        // Return a reference to the modifiable "Element9" attribute of this
        // object.

    double& element10();
        // Return a reference to the modifiable "Element10" attribute of this
        // object.

    std::vector<char>& element11();
        // Return a reference to the modifiable "Element11" attribute of this
        // object.

    int& element12();
        // Return a reference to the modifiable "Element12" attribute of this
        // object.

    Enumerated::Value& element13();
        // Return a reference to the modifiable "Element13" attribute of this
        // object.

    std::vector<bool>& element14();
        // Return a reference to the modifiable "Element14" attribute of this
        // object.

    std::vector<double>& element15();
        // Return a reference to the modifiable "Element15" attribute of this
        // object.

    std::vector<std::vector<char> >& element16();
        // Return a reference to the modifiable "Element16" attribute of this
        // object.

    std::vector<int>& element17();
        // Return a reference to the modifiable "Element17" attribute of this
        // object.

    std::vector<bdet_DatetimeTz>& element18();
        // Return a reference to the modifiable "Element18" attribute of this
        // object.

    std::vector<CustomString>& element19();
        // Return a reference to the modifiable "Element19" attribute of this
        // object.

    // ACCESSORS
    std::ostream& print(std::ostream& stream,
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

    const std::vector<Sequence3>& element1() const;
        // Return a reference to the non-modifiable "Element1" attribute of
        // this object.

    const std::vector<Choice1>& element2() const;
        // Return a reference to the non-modifiable "Element2" attribute of
        // this object.

    const bdeut_NullableValue<std::vector<char> >& element3() const;
        // Return a reference to the non-modifiable "Element3" attribute of
        // this object.

    const bdeut_NullableValue<int>& element4() const;
        // Return a reference to the non-modifiable "Element4" attribute of
        // this object.

    const bdeut_NullableValue<bdet_DatetimeTz>& element5() const;
        // Return a reference to the non-modifiable "Element5" attribute of
        // this object.

    const bdeut_NullableValue<CustomString>& element6() const;
        // Return a reference to the non-modifiable "Element6" attribute of
        // this object.

    const bdeut_NullableValue<Enumerated::Value>& element7() const;
        // Return a reference to the non-modifiable "Element7" attribute of
        // this object.

    bool element8() const;
        // Return a reference to the non-modifiable "Element8" attribute of
        // this object.

    const std::string& element9() const;
        // Return a reference to the non-modifiable "Element9" attribute of
        // this object.

    double element10() const;
        // Return a reference to the non-modifiable "Element10" attribute of
        // this object.

    const std::vector<char>& element11() const;
        // Return a reference to the non-modifiable "Element11" attribute of
        // this object.

    int element12() const;
        // Return a reference to the non-modifiable "Element12" attribute of
        // this object.

    Enumerated::Value element13() const;
        // Return a reference to the non-modifiable "Element13" attribute of
        // this object.

    const std::vector<bool>& element14() const;
        // Return a reference to the non-modifiable "Element14" attribute of
        // this object.

    const std::vector<double>& element15() const;
        // Return a reference to the non-modifiable "Element15" attribute of
        // this object.

    const std::vector<std::vector<char> >& element16() const;
        // Return a reference to the non-modifiable "Element16" attribute of
        // this object.

    const std::vector<int>& element17() const;
        // Return a reference to the non-modifiable "Element17" attribute of
        // this object.

    const std::vector<bdet_DatetimeTz>& element18() const;
        // Return a reference to the non-modifiable "Element18" attribute of
        // this object.

    const std::vector<CustomString>& element19() const;
        // Return a reference to the non-modifiable "Element19" attribute of
        // this object.
};

// FREE OPERATORS
inline
bool operator==(const Sequence4& lhs, const Sequence4& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
    // the same value, and 'false' otherwise.  Two attribute objects have the
    // same value if each respective attribute has the same value.

inline
bool operator!=(const Sequence4& lhs, const Sequence4& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
    // have the same value, and 'false' otherwise.  Two attribute objects do
    // not have the same value if one or more respective attributes differ in
    // values.

inline
std::ostream& operator<<(std::ostream& stream, const Sequence4& rhs);
    // Format the specified 'rhs' to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.

}  // close namespace baea

// TRAITS

BDEAT_DECL_SEQUENCE_WITH_ALLOCATOR_BITWISEMOVEABLE_TRAITS(baea::Sequence4)

namespace baea {

                              // ===============                               
                              // class Sequence1                               
                              // ===============                               

class Sequence1 {

    // INSTANCE DATA
    bdema_Allocator              *d_allocator_p;
    std::vector<Choice3>          d_element4;
    std::vector<Choice1>          d_element2;
    bdeut_NullableValue<Choice3>  d_element1;
    Choice2                      *d_element3;

  public:
    // TYPES
    enum {
        ATTRIBUTE_ID_ELEMENT1 = 0
      , ATTRIBUTE_ID_ELEMENT2 = 1
      , ATTRIBUTE_ID_ELEMENT3 = 2
      , ATTRIBUTE_ID_ELEMENT4 = 3
    };

    enum {
        NUM_ATTRIBUTES = 4
    };

    enum {
        ATTRIBUTE_INDEX_ELEMENT1 = 0
      , ATTRIBUTE_INDEX_ELEMENT2 = 1
      , ATTRIBUTE_INDEX_ELEMENT3 = 2
      , ATTRIBUTE_INDEX_ELEMENT4 = 3
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
    explicit Sequence1(bdema_Allocator *basicAllocator = 0);
        // Create an object of type 'Sequence1' having the default value.  Use
        // the optionally specified 'basicAllocator' to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.

    Sequence1(const Sequence1& original,
              bdema_Allocator *basicAllocator = 0);
        // Create an object of type 'Sequence1' having the value of the
        // specified 'original' object.  Use the optionally specified
        // 'basicAllocator' to supply memory.  If 'basicAllocator' is 0, the
        // currently installed default allocator is used.

    ~Sequence1();
        // Destroy this object.

    // MANIPULATORS
    Sequence1& operator=(const Sequence1& rhs);
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

    bdeut_NullableValue<Choice3>& element1();
        // Return a reference to the modifiable "Element1" attribute of this
        // object.

    std::vector<Choice1>& element2();
        // Return a reference to the modifiable "Element2" attribute of this
        // object.

    Choice2& element3();
        // Return a reference to the modifiable "Element3" attribute of this
        // object.

    std::vector<Choice3>& element4();
        // Return a reference to the modifiable "Element4" attribute of this
        // object.

    // ACCESSORS
    std::ostream& print(std::ostream& stream,
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

    const bdeut_NullableValue<Choice3>& element1() const;
        // Return a reference to the non-modifiable "Element1" attribute of
        // this object.

    const std::vector<Choice1>& element2() const;
        // Return a reference to the non-modifiable "Element2" attribute of
        // this object.

    const Choice2& element3() const;
        // Return a reference to the non-modifiable "Element3" attribute of
        // this object.

    const std::vector<Choice3>& element4() const;
        // Return a reference to the non-modifiable "Element4" attribute of
        // this object.
};

// FREE OPERATORS
inline
bool operator==(const Sequence1& lhs, const Sequence1& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
    // the same value, and 'false' otherwise.  Two attribute objects have the
    // same value if each respective attribute has the same value.

inline
bool operator!=(const Sequence1& lhs, const Sequence1& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
    // have the same value, and 'false' otherwise.  Two attribute objects do
    // not have the same value if one or more respective attributes differ in
    // values.

inline
std::ostream& operator<<(std::ostream& stream, const Sequence1& rhs);
    // Format the specified 'rhs' to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.

}  // close namespace baea

// TRAITS

BDEAT_DECL_SEQUENCE_WITH_ALLOCATOR_BITWISEMOVEABLE_TRAITS(baea::Sequence1)

namespace baea {

                              // ===============                               
                              // class Sequence2                               
                              // ===============                               

class Sequence2 {

    // INSTANCE DATA
    bdeut_NullableValue<double>   d_element5;
    bdeut_NullableValue<Choice1>  d_element4;
    bdet_DatetimeTz               d_element3;
    CustomString                  d_element1;
    unsigned char                 d_element2;

  public:
    // TYPES
    enum {
        ATTRIBUTE_ID_ELEMENT1 = 0
      , ATTRIBUTE_ID_ELEMENT2 = 1
      , ATTRIBUTE_ID_ELEMENT3 = 2
      , ATTRIBUTE_ID_ELEMENT4 = 3
      , ATTRIBUTE_ID_ELEMENT5 = 4
    };

    enum {
        NUM_ATTRIBUTES = 5
    };

    enum {
        ATTRIBUTE_INDEX_ELEMENT1 = 0
      , ATTRIBUTE_INDEX_ELEMENT2 = 1
      , ATTRIBUTE_INDEX_ELEMENT3 = 2
      , ATTRIBUTE_INDEX_ELEMENT4 = 3
      , ATTRIBUTE_INDEX_ELEMENT5 = 4
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
    explicit Sequence2(bdema_Allocator *basicAllocator = 0);
        // Create an object of type 'Sequence2' having the default value.  Use
        // the optionally specified 'basicAllocator' to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.

    Sequence2(const Sequence2& original,
              bdema_Allocator *basicAllocator = 0);
        // Create an object of type 'Sequence2' having the value of the
        // specified 'original' object.  Use the optionally specified
        // 'basicAllocator' to supply memory.  If 'basicAllocator' is 0, the
        // currently installed default allocator is used.

    ~Sequence2();
        // Destroy this object.

    // MANIPULATORS
    Sequence2& operator=(const Sequence2& rhs);
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

    CustomString& element1();
        // Return a reference to the modifiable "Element1" attribute of this
        // object.

    unsigned char& element2();
        // Return a reference to the modifiable "Element2" attribute of this
        // object.

    bdet_DatetimeTz& element3();
        // Return a reference to the modifiable "Element3" attribute of this
        // object.

    bdeut_NullableValue<Choice1>& element4();
        // Return a reference to the modifiable "Element4" attribute of this
        // object.

    bdeut_NullableValue<double>& element5();
        // Return a reference to the modifiable "Element5" attribute of this
        // object.

    // ACCESSORS
    std::ostream& print(std::ostream& stream,
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

    const CustomString& element1() const;
        // Return a reference to the non-modifiable "Element1" attribute of
        // this object.

    unsigned char element2() const;
        // Return a reference to the non-modifiable "Element2" attribute of
        // this object.

    const bdet_DatetimeTz& element3() const;
        // Return a reference to the non-modifiable "Element3" attribute of
        // this object.

    const bdeut_NullableValue<Choice1>& element4() const;
        // Return a reference to the non-modifiable "Element4" attribute of
        // this object.

    const bdeut_NullableValue<double>& element5() const;
        // Return a reference to the non-modifiable "Element5" attribute of
        // this object.
};

// FREE OPERATORS
inline
bool operator==(const Sequence2& lhs, const Sequence2& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
    // the same value, and 'false' otherwise.  Two attribute objects have the
    // same value if each respective attribute has the same value.

inline
bool operator!=(const Sequence2& lhs, const Sequence2& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
    // have the same value, and 'false' otherwise.  Two attribute objects do
    // not have the same value if one or more respective attributes differ in
    // values.

inline
std::ostream& operator<<(std::ostream& stream, const Sequence2& rhs);
    // Format the specified 'rhs' to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.

}  // close namespace baea

// TRAITS

BDEAT_DECL_SEQUENCE_WITH_ALLOCATOR_BITWISEMOVEABLE_TRAITS(baea::Sequence2)

namespace baea {

                          // ========================                          
                          // class FeatureTestMessage                          
                          // ========================                          

class FeatureTestMessage {

    // INSTANCE DATA
    union {
        bsls_ObjectBuffer< Sequence1 >          d_selection1;
        bsls_ObjectBuffer< std::vector<char> >  d_selection2;
        bsls_ObjectBuffer< Sequence2 >          d_selection3;
        Sequence3                              *d_selection4;
        bsls_ObjectBuffer< bdet_DatetimeTz >    d_selection5;
        bsls_ObjectBuffer< CustomString >       d_selection6;
        bsls_ObjectBuffer< Enumerated::Value >  d_selection7;
        bsls_ObjectBuffer< Choice3 >            d_selection8;
        bsls_ObjectBuffer< VoidSequence >       d_selection9;
        bsls_ObjectBuffer< UnsignedSequence >   d_selection10;
    };

    int                                         d_selectionId;
    bdema_Allocator                            *d_allocator_p;

  public:
    // TYPES
    enum {
        SELECTION_ID_UNDEFINED   = -1

      , SELECTION_ID_SELECTION1  = 0
      , SELECTION_ID_SELECTION2  = 1
      , SELECTION_ID_SELECTION3  = 2
      , SELECTION_ID_SELECTION4  = 3
      , SELECTION_ID_SELECTION5  = 4
      , SELECTION_ID_SELECTION6  = 5
      , SELECTION_ID_SELECTION7  = 6
      , SELECTION_ID_SELECTION8  = 7
      , SELECTION_ID_SELECTION9  = 8
      , SELECTION_ID_SELECTION10 = 9
    };

    enum {
        NUM_SELECTIONS = 10
    };

    enum {
        SELECTION_INDEX_SELECTION1  = 0
      , SELECTION_INDEX_SELECTION2  = 1
      , SELECTION_INDEX_SELECTION3  = 2
      , SELECTION_INDEX_SELECTION4  = 3
      , SELECTION_INDEX_SELECTION5  = 4
      , SELECTION_INDEX_SELECTION6  = 5
      , SELECTION_INDEX_SELECTION7  = 6
      , SELECTION_INDEX_SELECTION8  = 7
      , SELECTION_INDEX_SELECTION9  = 8
      , SELECTION_INDEX_SELECTION10 = 9
    };

    // CONSTANTS
    static const char CLASS_NAME[];

    static const bdeat_SelectionInfo SELECTION_INFO_ARRAY[];

    // CLASS METHODS
    static int maxSupportedBdexVersion();
        // Return the most current 'bdex' streaming version number supported by
        // this class.  See the 'bdex' package-level documentation for more
        // information on 'bdex' streaming of value-semantic types and
        // containers.

    static const bdeat_SelectionInfo *lookupSelectionInfo(int id);
        // Return selection information for the selection indicated by the
        // specified 'id' if the selection exists, and 0 otherwise.

    static const bdeat_SelectionInfo *lookupSelectionInfo(
                                                    const char *name,
                                                    int         nameLength);
        // Return selection information for the selection indicated by the
        // specified 'name' of the specified 'nameLength' if the selection
        // exists, and 0 otherwise.

    // CREATORS
    explicit FeatureTestMessage(bdema_Allocator *basicAllocator = 0);
        // Create an object of type 'FeatureTestMessage' having the default
        // value.  Use the optionally specified 'basicAllocator' to supply
        // memory.  If 'basicAllocator' is 0, the currently installed default
        // allocator is used.

    FeatureTestMessage(const FeatureTestMessage& original,
                      bdema_Allocator *basicAllocator = 0);
        // Create an object of type 'FeatureTestMessage' having the value of
        // the specified 'original' object.  Use the optionally specified
        // 'basicAllocator' to supply memory.  If 'basicAllocator' is 0, the
        // currently installed default allocator is used.

    ~FeatureTestMessage();
        // Destroy this object.

    // MANIPULATORS
    FeatureTestMessage& operator=(const FeatureTestMessage& rhs);
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
        // Reset this object to the default value (i.e., its value upon default
        // construction).

    int makeSelection(int selectionId);
        // Set the value of this object to be the default for the selection
        // indicated by the specified 'selectionId'.  Return 0 on success, and
        // non-zero value otherwise (i.e., the selection is not found).

    int makeSelection(const char *name, int nameLength);
        // Set the value of this object to be the default for the selection
        // indicated by the specified 'name' of the specified 'nameLength'.
        // Return 0 on success, and non-zero value otherwise (i.e., the
        // selection is not found).

    Sequence1& makeSelection1();
    Sequence1& makeSelection1(const Sequence1& value);
        // Set the value of this object to be a "Selection1" value.  Optionally
        // specify the 'value' of the "Selection1".  If 'value' is not
        // specified, the default "Selection1" value is used.

    std::vector<char>& makeSelection2();
    std::vector<char>& makeSelection2(const std::vector<char>& value);
        // Set the value of this object to be a "Selection2" value.  Optionally
        // specify the 'value' of the "Selection2".  If 'value' is not
        // specified, the default "Selection2" value is used.

    Sequence2& makeSelection3();
    Sequence2& makeSelection3(const Sequence2& value);
        // Set the value of this object to be a "Selection3" value.  Optionally
        // specify the 'value' of the "Selection3".  If 'value' is not
        // specified, the default "Selection3" value is used.

    Sequence3& makeSelection4();
    Sequence3& makeSelection4(const Sequence3& value);
        // Set the value of this object to be a "Selection4" value.  Optionally
        // specify the 'value' of the "Selection4".  If 'value' is not
        // specified, the default "Selection4" value is used.

    bdet_DatetimeTz& makeSelection5();
    bdet_DatetimeTz& makeSelection5(const bdet_DatetimeTz& value);
        // Set the value of this object to be a "Selection5" value.  Optionally
        // specify the 'value' of the "Selection5".  If 'value' is not
        // specified, the default "Selection5" value is used.

    CustomString& makeSelection6();
    CustomString& makeSelection6(const CustomString& value);
        // Set the value of this object to be a "Selection6" value.  Optionally
        // specify the 'value' of the "Selection6".  If 'value' is not
        // specified, the default "Selection6" value is used.

    Enumerated::Value& makeSelection7();
    Enumerated::Value& makeSelection7(Enumerated::Value value);
        // Set the value of this object to be a "Selection7" value.  Optionally
        // specify the 'value' of the "Selection7".  If 'value' is not
        // specified, the default "Selection7" value is used.

    Choice3& makeSelection8();
    Choice3& makeSelection8(const Choice3& value);
        // Set the value of this object to be a "Selection8" value.  Optionally
        // specify the 'value' of the "Selection8".  If 'value' is not
        // specified, the default "Selection8" value is used.

    VoidSequence& makeSelection9();
    VoidSequence& makeSelection9(const VoidSequence& value);
        // Set the value of this object to be a "Selection9" value.  Optionally
        // specify the 'value' of the "Selection9".  If 'value' is not
        // specified, the default "Selection9" value is used.

    UnsignedSequence& makeSelection10();
    UnsignedSequence& makeSelection10(const UnsignedSequence& value);
        // Set the value of this object to be a "Selection10" value. 
        // Optionally specify the 'value' of the "Selection10".  If 'value' is
        // not specified, the default "Selection10" value is used.

    template<class MANIPULATOR>
    int manipulateSelection(MANIPULATOR& manipulator);
        // Invoke the specified 'manipulator' on the address of the modifiable
        // selection, supplying 'manipulator' with the corresponding selection
        // information structure.  Return the value returned from the
        // invocation of 'manipulator' if this object has a defined selection,
        // and -1 otherwise.

    Sequence1& selection1();
        // Return a reference to the modifiable "Selection1" selection of this
        // object if "Selection1" is the current selection.  The behavior is
        // undefined unless "Selection1" is the selection of this object.

    std::vector<char>& selection2();
        // Return a reference to the modifiable "Selection2" selection of this
        // object if "Selection2" is the current selection.  The behavior is
        // undefined unless "Selection2" is the selection of this object.

    Sequence2& selection3();
        // Return a reference to the modifiable "Selection3" selection of this
        // object if "Selection3" is the current selection.  The behavior is
        // undefined unless "Selection3" is the selection of this object.

    Sequence3& selection4();
        // Return a reference to the modifiable "Selection4" selection of this
        // object if "Selection4" is the current selection.  The behavior is
        // undefined unless "Selection4" is the selection of this object.

    bdet_DatetimeTz& selection5();
        // Return a reference to the modifiable "Selection5" selection of this
        // object if "Selection5" is the current selection.  The behavior is
        // undefined unless "Selection5" is the selection of this object.

    CustomString& selection6();
        // Return a reference to the modifiable "Selection6" selection of this
        // object if "Selection6" is the current selection.  The behavior is
        // undefined unless "Selection6" is the selection of this object.

    Enumerated::Value& selection7();
        // Return a reference to the modifiable "Selection7" selection of this
        // object if "Selection7" is the current selection.  The behavior is
        // undefined unless "Selection7" is the selection of this object.

    Choice3& selection8();
        // Return a reference to the modifiable "Selection8" selection of this
        // object if "Selection8" is the current selection.  The behavior is
        // undefined unless "Selection8" is the selection of this object.

    VoidSequence& selection9();
        // Return a reference to the modifiable "Selection9" selection of this
        // object if "Selection9" is the current selection.  The behavior is
        // undefined unless "Selection9" is the selection of this object.

    UnsignedSequence& selection10();
        // Return a reference to the modifiable "Selection10" selection of this
        // object if "Selection10" is the current selection.  The behavior is
        // undefined unless "Selection10" is the selection of this object.

    // ACCESSORS
    std::ostream& print(std::ostream& stream,
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

    int selectionId() const;
        // Return the id of the current selection if the selection is defined,
        // and -1 otherwise.

    template<class ACCESSOR>
    int accessSelection(ACCESSOR& accessor) const;
        // Invoke the specified 'accessor' on the non-modifiable selection,
        // supplying 'accessor' with the corresponding selection information
        // structure.  Return the value returned from the invocation of
        // 'accessor' if this object has a defined selection, and -1 otherwise.

    const Sequence1& selection1() const;
        // Return a reference to the non-modifiable "Selection1" selection of
        // this object if "Selection1" is the current selection.  The behavior
        // is undefined unless "Selection1" is the selection of this object.

    const std::vector<char>& selection2() const;
        // Return a reference to the non-modifiable "Selection2" selection of
        // this object if "Selection2" is the current selection.  The behavior
        // is undefined unless "Selection2" is the selection of this object.

    const Sequence2& selection3() const;
        // Return a reference to the non-modifiable "Selection3" selection of
        // this object if "Selection3" is the current selection.  The behavior
        // is undefined unless "Selection3" is the selection of this object.

    const Sequence3& selection4() const;
        // Return a reference to the non-modifiable "Selection4" selection of
        // this object if "Selection4" is the current selection.  The behavior
        // is undefined unless "Selection4" is the selection of this object.

    const bdet_DatetimeTz& selection5() const;
        // Return a reference to the non-modifiable "Selection5" selection of
        // this object if "Selection5" is the current selection.  The behavior
        // is undefined unless "Selection5" is the selection of this object.

    const CustomString& selection6() const;
        // Return a reference to the non-modifiable "Selection6" selection of
        // this object if "Selection6" is the current selection.  The behavior
        // is undefined unless "Selection6" is the selection of this object.

    const Enumerated::Value& selection7() const;
        // Return a reference to the non-modifiable "Selection7" selection of
        // this object if "Selection7" is the current selection.  The behavior
        // is undefined unless "Selection7" is the selection of this object.

    const Choice3& selection8() const;
        // Return a reference to the non-modifiable "Selection8" selection of
        // this object if "Selection8" is the current selection.  The behavior
        // is undefined unless "Selection8" is the selection of this object.

    const VoidSequence& selection9() const;
        // Return a reference to the non-modifiable "Selection9" selection of
        // this object if "Selection9" is the current selection.  The behavior
        // is undefined unless "Selection9" is the selection of this object.

    const UnsignedSequence& selection10() const;
        // Return a reference to the non-modifiable "Selection10" selection of
        // this object if "Selection10" is the current selection.  The behavior
        // is undefined unless "Selection10" is the selection of this object.

    bool isSelection1Value() const;
        // Return 'true' if the value of this object is a "Selection1" value,
        // and return 'false' otherwise.

    bool isSelection2Value() const;
        // Return 'true' if the value of this object is a "Selection2" value,
        // and return 'false' otherwise.

    bool isSelection3Value() const;
        // Return 'true' if the value of this object is a "Selection3" value,
        // and return 'false' otherwise.

    bool isSelection4Value() const;
        // Return 'true' if the value of this object is a "Selection4" value,
        // and return 'false' otherwise.

    bool isSelection5Value() const;
        // Return 'true' if the value of this object is a "Selection5" value,
        // and return 'false' otherwise.

    bool isSelection6Value() const;
        // Return 'true' if the value of this object is a "Selection6" value,
        // and return 'false' otherwise.

    bool isSelection7Value() const;
        // Return 'true' if the value of this object is a "Selection7" value,
        // and return 'false' otherwise.

    bool isSelection8Value() const;
        // Return 'true' if the value of this object is a "Selection8" value,
        // and return 'false' otherwise.

    bool isSelection9Value() const;
        // Return 'true' if the value of this object is a "Selection9" value,
        // and return 'false' otherwise.

    bool isSelection10Value() const;
        // Return 'true' if the value of this object is a "Selection10" value,
        // and return 'false' otherwise.

    bool isUndefinedValue() const;
        // Return 'true' if the value of this object is undefined, and 'false'
        // otherwise.

    const char *selectionName() const;
        // Return the symbolic name of the current selection of this object.
};

// FREE OPERATORS
inline
bool operator==(const FeatureTestMessage& lhs, const FeatureTestMessage& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects have the same
    // value, and 'false' otherwise.  Two 'FeatureTestMessage' objects have the same
    // value if either the selections in both objects have the same ids and
    // the same values, or both selections are undefined.

inline
bool operator!=(const FeatureTestMessage& lhs, const FeatureTestMessage& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects do not have the
    // same values, as determined by 'operator==', and 'false' otherwise.

inline
std::ostream& operator<<(std::ostream& stream, const FeatureTestMessage& rhs);
    // Format the specified 'rhs' to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.

}  // close namespace baea

// TRAITS

BDEAT_DECL_CHOICE_WITH_ALLOCATOR_BITWISEMOVEABLE_TRAITS(baea::FeatureTestMessage)

namespace baea {

                               // =============                                
                               // class Request                                
                               // =============                                

class Request {

    // INSTANCE DATA
    union {
        bsls_ObjectBuffer< SimpleRequest >      d_simpleRequest;
        bsls_ObjectBuffer< FeatureTestMessage > d_featureRequest;
    };

    int                                         d_selectionId;
    bdema_Allocator                            *d_allocator_p;

  public:
    // TYPES
    enum {
        SELECTION_ID_UNDEFINED       = -1

      , SELECTION_ID_SIMPLE_REQUEST  = 0
      , SELECTION_ID_FEATURE_REQUEST = 1
    };

    enum {
        NUM_SELECTIONS = 2
    };

    enum {
        SELECTION_INDEX_SIMPLE_REQUEST  = 0
      , SELECTION_INDEX_FEATURE_REQUEST = 1
    };

    // CONSTANTS
    static const char CLASS_NAME[];

    static const bdeat_SelectionInfo SELECTION_INFO_ARRAY[];

    // CLASS METHODS
    static int maxSupportedBdexVersion();
        // Return the most current 'bdex' streaming version number supported by
        // this class.  See the 'bdex' package-level documentation for more
        // information on 'bdex' streaming of value-semantic types and
        // containers.

    static const bdeat_SelectionInfo *lookupSelectionInfo(int id);
        // Return selection information for the selection indicated by the
        // specified 'id' if the selection exists, and 0 otherwise.

    static const bdeat_SelectionInfo *lookupSelectionInfo(
                                                    const char *name,
                                                    int         nameLength);
        // Return selection information for the selection indicated by the
        // specified 'name' of the specified 'nameLength' if the selection
        // exists, and 0 otherwise.

    // CREATORS
    explicit Request(bdema_Allocator *basicAllocator = 0);
        // Create an object of type 'Request' having the default value.  Use
        // the optionally specified 'basicAllocator' to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.

    Request(const Request& original,
           bdema_Allocator *basicAllocator = 0);
        // Create an object of type 'Request' having the value of the specified
        // 'original' object.  Use the optionally specified 'basicAllocator' to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.

    ~Request();
        // Destroy this object.

    // MANIPULATORS
    Request& operator=(const Request& rhs);
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
        // Reset this object to the default value (i.e., its value upon default
        // construction).

    int makeSelection(int selectionId);
        // Set the value of this object to be the default for the selection
        // indicated by the specified 'selectionId'.  Return 0 on success, and
        // non-zero value otherwise (i.e., the selection is not found).

    int makeSelection(const char *name, int nameLength);
        // Set the value of this object to be the default for the selection
        // indicated by the specified 'name' of the specified 'nameLength'.
        // Return 0 on success, and non-zero value otherwise (i.e., the
        // selection is not found).

    SimpleRequest& makeSimpleRequest();
    SimpleRequest& makeSimpleRequest(const SimpleRequest& value);
        // Set the value of this object to be a "SimpleRequest" value. 
        // Optionally specify the 'value' of the "SimpleRequest".  If 'value'
        // is not specified, the default "SimpleRequest" value is used.

    FeatureTestMessage& makeFeatureRequest();
    FeatureTestMessage& makeFeatureRequest(const FeatureTestMessage& value);
        // Set the value of this object to be a "FeatureRequest" value. 
        // Optionally specify the 'value' of the "FeatureRequest".  If 'value'
        // is not specified, the default "FeatureRequest" value is used.

    template<class MANIPULATOR>
    int manipulateSelection(MANIPULATOR& manipulator);
        // Invoke the specified 'manipulator' on the address of the modifiable
        // selection, supplying 'manipulator' with the corresponding selection
        // information structure.  Return the value returned from the
        // invocation of 'manipulator' if this object has a defined selection,
        // and -1 otherwise.

    SimpleRequest& simpleRequest();
        // Return a reference to the modifiable "SimpleRequest" selection of
        // this object if "SimpleRequest" is the current selection.  The
        // behavior is undefined unless "SimpleRequest" is the selection of
        // this object.

    FeatureTestMessage& featureRequest();
        // Return a reference to the modifiable "FeatureRequest" selection of
        // this object if "FeatureRequest" is the current selection.  The
        // behavior is undefined unless "FeatureRequest" is the selection of
        // this object.

    // ACCESSORS
    std::ostream& print(std::ostream& stream,
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

    int selectionId() const;
        // Return the id of the current selection if the selection is defined,
        // and -1 otherwise.

    template<class ACCESSOR>
    int accessSelection(ACCESSOR& accessor) const;
        // Invoke the specified 'accessor' on the non-modifiable selection,
        // supplying 'accessor' with the corresponding selection information
        // structure.  Return the value returned from the invocation of
        // 'accessor' if this object has a defined selection, and -1 otherwise.

    const SimpleRequest& simpleRequest() const;
        // Return a reference to the non-modifiable "SimpleRequest" selection
        // of this object if "SimpleRequest" is the current selection.  The
        // behavior is undefined unless "SimpleRequest" is the selection of
        // this object.

    const FeatureTestMessage& featureRequest() const;
        // Return a reference to the non-modifiable "FeatureRequest" selection
        // of this object if "FeatureRequest" is the current selection.  The
        // behavior is undefined unless "FeatureRequest" is the selection of
        // this object.

    bool isSimpleRequestValue() const;
        // Return 'true' if the value of this object is a "SimpleRequest"
        // value, and return 'false' otherwise.

    bool isFeatureRequestValue() const;
        // Return 'true' if the value of this object is a "FeatureRequest"
        // value, and return 'false' otherwise.

    bool isUndefinedValue() const;
        // Return 'true' if the value of this object is undefined, and 'false'
        // otherwise.

    const char *selectionName() const;
        // Return the symbolic name of the current selection of this object.
};

// FREE OPERATORS
inline
bool operator==(const Request& lhs, const Request& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects have the same
    // value, and 'false' otherwise.  Two 'Request' objects have the same
    // value if either the selections in both objects have the same ids and
    // the same values, or both selections are undefined.

inline
bool operator!=(const Request& lhs, const Request& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects do not have the
    // same values, as determined by 'operator==', and 'false' otherwise.

inline
std::ostream& operator<<(std::ostream& stream, const Request& rhs);
    // Format the specified 'rhs' to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.

}  // close namespace baea

// TRAITS

BDEAT_DECL_CHOICE_WITH_ALLOCATOR_BITWISEMOVEABLE_TRAITS(baea::Request)

namespace baea {

                               // ==============                               
                               // class Response                               
                               // ==============                               

class Response {

    // INSTANCE DATA
    union {
        bsls_ObjectBuffer< std::string >        d_responseData;
        bsls_ObjectBuffer< FeatureTestMessage > d_featureResponse;
    };

    int                                         d_selectionId;
    bdema_Allocator                            *d_allocator_p;

  public:
    // TYPES
    enum {
        SELECTION_ID_UNDEFINED        = -1

      , SELECTION_ID_RESPONSE_DATA    = 0
      , SELECTION_ID_FEATURE_RESPONSE = 1
    };

    enum {
        NUM_SELECTIONS = 2
    };

    enum {
        SELECTION_INDEX_RESPONSE_DATA    = 0
      , SELECTION_INDEX_FEATURE_RESPONSE = 1
    };

    // CONSTANTS
    static const char CLASS_NAME[];

    static const bdeat_SelectionInfo SELECTION_INFO_ARRAY[];

    // CLASS METHODS
    static int maxSupportedBdexVersion();
        // Return the most current 'bdex' streaming version number supported by
        // this class.  See the 'bdex' package-level documentation for more
        // information on 'bdex' streaming of value-semantic types and
        // containers.

    static const bdeat_SelectionInfo *lookupSelectionInfo(int id);
        // Return selection information for the selection indicated by the
        // specified 'id' if the selection exists, and 0 otherwise.

    static const bdeat_SelectionInfo *lookupSelectionInfo(
                                                    const char *name,
                                                    int         nameLength);
        // Return selection information for the selection indicated by the
        // specified 'name' of the specified 'nameLength' if the selection
        // exists, and 0 otherwise.

    // CREATORS
    explicit Response(bdema_Allocator *basicAllocator = 0);
        // Create an object of type 'Response' having the default value.  Use
        // the optionally specified 'basicAllocator' to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.

    Response(const Response& original,
            bdema_Allocator *basicAllocator = 0);
        // Create an object of type 'Response' having the value of the
        // specified 'original' object.  Use the optionally specified
        // 'basicAllocator' to supply memory.  If 'basicAllocator' is 0, the
        // currently installed default allocator is used.

    ~Response();
        // Destroy this object.

    // MANIPULATORS
    Response& operator=(const Response& rhs);
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
        // Reset this object to the default value (i.e., its value upon default
        // construction).

    int makeSelection(int selectionId);
        // Set the value of this object to be the default for the selection
        // indicated by the specified 'selectionId'.  Return 0 on success, and
        // non-zero value otherwise (i.e., the selection is not found).

    int makeSelection(const char *name, int nameLength);
        // Set the value of this object to be the default for the selection
        // indicated by the specified 'name' of the specified 'nameLength'.
        // Return 0 on success, and non-zero value otherwise (i.e., the
        // selection is not found).

    std::string& makeResponseData();
    std::string& makeResponseData(const std::string& value);
        // Set the value of this object to be a "ResponseData" value. 
        // Optionally specify the 'value' of the "ResponseData".  If 'value' is
        // not specified, the default "ResponseData" value is used.

    FeatureTestMessage& makeFeatureResponse();
    FeatureTestMessage& makeFeatureResponse(const FeatureTestMessage& value);
        // Set the value of this object to be a "FeatureResponse" value. 
        // Optionally specify the 'value' of the "FeatureResponse".  If 'value'
        // is not specified, the default "FeatureResponse" value is used.

    template<class MANIPULATOR>
    int manipulateSelection(MANIPULATOR& manipulator);
        // Invoke the specified 'manipulator' on the address of the modifiable
        // selection, supplying 'manipulator' with the corresponding selection
        // information structure.  Return the value returned from the
        // invocation of 'manipulator' if this object has a defined selection,
        // and -1 otherwise.

    std::string& responseData();
        // Return a reference to the modifiable "ResponseData" selection of
        // this object if "ResponseData" is the current selection.  The
        // behavior is undefined unless "ResponseData" is the selection of this
        // object.

    FeatureTestMessage& featureResponse();
        // Return a reference to the modifiable "FeatureResponse" selection of
        // this object if "FeatureResponse" is the current selection.  The
        // behavior is undefined unless "FeatureResponse" is the selection of
        // this object.

    // ACCESSORS
    std::ostream& print(std::ostream& stream,
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

    int selectionId() const;
        // Return the id of the current selection if the selection is defined,
        // and -1 otherwise.

    template<class ACCESSOR>
    int accessSelection(ACCESSOR& accessor) const;
        // Invoke the specified 'accessor' on the non-modifiable selection,
        // supplying 'accessor' with the corresponding selection information
        // structure.  Return the value returned from the invocation of
        // 'accessor' if this object has a defined selection, and -1 otherwise.

    const std::string& responseData() const;
        // Return a reference to the non-modifiable "ResponseData" selection of
        // this object if "ResponseData" is the current selection.  The
        // behavior is undefined unless "ResponseData" is the selection of this
        // object.

    const FeatureTestMessage& featureResponse() const;
        // Return a reference to the non-modifiable "FeatureResponse" selection
        // of this object if "FeatureResponse" is the current selection.  The
        // behavior is undefined unless "FeatureResponse" is the selection of
        // this object.

    bool isResponseDataValue() const;
        // Return 'true' if the value of this object is a "ResponseData" value,
        // and return 'false' otherwise.

    bool isFeatureResponseValue() const;
        // Return 'true' if the value of this object is a "FeatureResponse"
        // value, and return 'false' otherwise.

    bool isUndefinedValue() const;
        // Return 'true' if the value of this object is undefined, and 'false'
        // otherwise.

    const char *selectionName() const;
        // Return the symbolic name of the current selection of this object.
};

// FREE OPERATORS
inline
bool operator==(const Response& lhs, const Response& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects have the same
    // value, and 'false' otherwise.  Two 'Response' objects have the same
    // value if either the selections in both objects have the same ids and
    // the same values, or both selections are undefined.

inline
bool operator!=(const Response& lhs, const Response& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects do not have the
    // same values, as determined by 'operator==', and 'false' otherwise.

inline
std::ostream& operator<<(std::ostream& stream, const Response& rhs);
    // Format the specified 'rhs' to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.

}  // close namespace baea

// TRAITS

BDEAT_DECL_CHOICE_WITH_ALLOCATOR_BITWISEMOVEABLE_TRAITS(baea::Response)

namespace baea {

                             // ==================                              
                             // class Testmessages                              
                             // ==================                              

struct Testmessages {
    // This class serves as a place holder to reserve a type having the same
    // name as this component.  Doing so ensures that such a type cannot be
    // defined outside of this component in the current namespace.
};

}  // close namespace baea

// ============================================================================
//                         INLINE FUNCTION DEFINITIONS
// ============================================================================

namespace baea {

                              // ---------------                               
                              // class CustomInt                               
                              // ---------------                               

// CREATORS
inline
CustomInt::CustomInt()
: d_value()
{
}

inline
CustomInt::CustomInt(const CustomInt& original)
: d_value(original.d_value)
{
}

inline
CustomInt::CustomInt(const int& value)
: d_value(value)
{
    BSLS_ASSERT(checkRestrictions(value) == 0);
}

inline
CustomInt::~CustomInt()
{
}

// MANIPULATORS
inline
CustomInt& CustomInt::operator=(const CustomInt& rhs)
{
    d_value = rhs.d_value;
    return *this;
}

template <class STREAM>
STREAM& CustomInt::bdexStreamIn(STREAM& stream, int version)
{
    int temp;

    bdex_InStreamFunctions::streamIn(stream, temp, version);

    if (!stream) {
        return stream;
    }

    if (fromInt(temp)!=0) {
        stream.invalidate();
    }

    return stream;
}

inline
void CustomInt::reset()
{
    bdeat_ValueTypeFunctions::reset(&d_value);
}

inline
int CustomInt::fromInt(const int& value)
{
    int ret = checkRestrictions(value);
    if (0 == ret) {
        d_value = value;
    }

    return ret;
}

// ACCESSORS
template <class STREAM>
STREAM& CustomInt::bdexStreamOut(STREAM& stream, int version) const
{
    return bdex_OutStreamFunctions::streamOut(stream, d_value, version);
}

inline
int CustomInt::maxSupportedBdexVersion() const
{
    return bdex_VersionFunctions::maxSupportedVersion(d_value);
}

inline
std::ostream& CustomInt::print(std::ostream& stream,
                                 int           level,
                                 int           spacesPerLevel) const
{
    return bdeu_PrintMethods::print(stream, d_value, level, spacesPerLevel);
}

inline
const int& CustomInt::toInt() const
{
    return d_value;
}



                             // ------------------                             
                             // class CustomString                             
                             // ------------------                             

// CREATORS
inline
CustomString::CustomString(bdema_Allocator *basicAllocator)
: d_value(basicAllocator)
{
}

inline
CustomString::CustomString(const CustomString& original, bdema_Allocator *basicAllocator)
: d_value(original.d_value, basicAllocator)
{
}

inline
CustomString::CustomString(const std::string& value, bdema_Allocator *basicAllocator)
: d_value(value, basicAllocator)
{
    BSLS_ASSERT(checkRestrictions(value) == 0);
}

inline
CustomString::~CustomString()
{
}

// MANIPULATORS
inline
CustomString& CustomString::operator=(const CustomString& rhs)
{
    d_value = rhs.d_value;
    return *this;
}

template <class STREAM>
STREAM& CustomString::bdexStreamIn(STREAM& stream, int version)
{
    std::string temp;

    bdex_InStreamFunctions::streamIn(stream, temp, version);

    if (!stream) {
        return stream;
    }

    if (fromString(temp)!=0) {
        stream.invalidate();
    }

    return stream;
}

inline
void CustomString::reset()
{
    bdeat_ValueTypeFunctions::reset(&d_value);
}

inline
int CustomString::fromString(const std::string& value)
{
    int ret = checkRestrictions(value);
    if (0 == ret) {
        d_value = value;
    }

    return ret;
}

// ACCESSORS
template <class STREAM>
STREAM& CustomString::bdexStreamOut(STREAM& stream, int version) const
{
    return bdex_OutStreamFunctions::streamOut(stream, d_value, version);
}

inline
int CustomString::maxSupportedBdexVersion() const
{
    return bdex_VersionFunctions::maxSupportedVersion(d_value);
}

inline
std::ostream& CustomString::print(std::ostream& stream,
                                 int           level,
                                 int           spacesPerLevel) const
{
    return bdeu_PrintMethods::print(stream, d_value, level, spacesPerLevel);
}

inline
const std::string& CustomString::toString() const
{
    return d_value;
}



                              // ----------------                              
                              // class Enumerated                              
                              // ----------------                              

// CLASS METHODS
inline
int Enumerated::maxSupportedBdexVersion()
{
    return 1;  // versions start at 1
}

inline
int Enumerated::fromString(Value *result, const std::string& string)
{
    return fromString(result, string.c_str(), string.length());
}

inline
std::ostream& Enumerated::print(std::ostream&      stream,
                                 Enumerated::Value value)
{
    return stream << toString(value);
}

template <class STREAM>
STREAM& Enumerated::bdexStreamIn(STREAM&             stream,
                                   Enumerated::Value& value,
                                   int                 version)
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
STREAM& Enumerated::bdexStreamOut(STREAM&              stream,
                                    Enumerated::Value value,
                                    int                version)
{
    switch (version) {
      case 1: {
        stream.putInt32(value);  // Write the value as an int
      } break;
    }
    return stream;
}



                            // -------------------                             
                            // class SimpleRequest                             
                            // -------------------                             

// CLASS METHODS
inline
int SimpleRequest::maxSupportedBdexVersion()
{
    return 1;  // versions start at 1.
}

// MANIPULATORS
template <class STREAM>
STREAM& SimpleRequest::bdexStreamIn(STREAM& stream, int version)
{
    if (stream) {
        switch (version) {
          case 1: {
            bdex_InStreamFunctions::streamIn(stream, d_data, 1);
            bdex_InStreamFunctions::streamIn(stream, d_responseLength, 1);
          } break;
          default: {
            stream.invalidate();
          }
        }
    }
    return stream;
}

template <class MANIPULATOR>
int SimpleRequest::manipulateAttributes(MANIPULATOR& manipulator)
{
    int ret;

    ret = manipulator(&d_data, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_DATA]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_responseLength, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_RESPONSE_LENGTH]);
    if (ret) {
        return ret;
    }

    return ret;
}

template <class MANIPULATOR>
int SimpleRequest::manipulateAttribute(MANIPULATOR& manipulator, int id)
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_DATA: {
        return manipulator(&d_data, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_DATA]);
      } break;
      case ATTRIBUTE_ID_RESPONSE_LENGTH: {
        return manipulator(&d_responseLength, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_RESPONSE_LENGTH]);
      } break;
      default:
        return NOT_FOUND;
    }
}

template <class MANIPULATOR>
int SimpleRequest::manipulateAttribute(
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
std::string& SimpleRequest::data()
{
    return d_data;
}

inline
int& SimpleRequest::responseLength()
{
    return d_responseLength;
}

// ACCESSORS
template <class STREAM>
STREAM& SimpleRequest::bdexStreamOut(STREAM& stream, int version) const
{
    switch (version) {
      case 1: {
        bdex_OutStreamFunctions::streamOut(stream, d_data, 1);
        bdex_OutStreamFunctions::streamOut(stream, d_responseLength, 1);
      } break;
    }
    return stream;
}

template <class ACCESSOR>
int SimpleRequest::accessAttributes(ACCESSOR& accessor) const
{
    int ret;

    ret = accessor(d_data, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_DATA]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_responseLength, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_RESPONSE_LENGTH]);
    if (ret) {
        return ret;
    }

    return ret;
}

template <class ACCESSOR>
int SimpleRequest::accessAttribute(ACCESSOR& accessor, int id) const
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_DATA: {
        return accessor(d_data, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_DATA]);
      } break;
      case ATTRIBUTE_ID_RESPONSE_LENGTH: {
        return accessor(d_responseLength, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_RESPONSE_LENGTH]);
      } break;
      default:
        return NOT_FOUND;
    }
}

template <class ACCESSOR>
int SimpleRequest::accessAttribute(
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
const std::string& SimpleRequest::data() const
{
    return d_data;
}

inline
int SimpleRequest::responseLength() const
{
    return d_responseLength;
}



                           // ----------------------                           
                           // class UnsignedSequence                           
                           // ----------------------                           

// CLASS METHODS
inline
int UnsignedSequence::maxSupportedBdexVersion()
{
    return 1;  // versions start at 1.
}

// MANIPULATORS
template <class STREAM>
STREAM& UnsignedSequence::bdexStreamIn(STREAM& stream, int version)
{
    if (stream) {
        switch (version) {
          case 1: {
            bdex_InStreamFunctions::streamIn(stream, d_element1, 1);
            bdex_InStreamFunctions::streamIn(stream, d_element2, 1);
            bdex_InStreamFunctions::streamIn(stream, d_element3, 1);
          } break;
          default: {
            stream.invalidate();
          }
        }
    }
    return stream;
}

template <class MANIPULATOR>
int UnsignedSequence::manipulateAttributes(MANIPULATOR& manipulator)
{
    int ret;

    ret = manipulator(&d_element1, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT1]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_element2, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT2]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_element3, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT3]);
    if (ret) {
        return ret;
    }

    return ret;
}

template <class MANIPULATOR>
int UnsignedSequence::manipulateAttribute(MANIPULATOR& manipulator, int id)
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_ELEMENT1: {
        return manipulator(&d_element1, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT1]);
      } break;
      case ATTRIBUTE_ID_ELEMENT2: {
        return manipulator(&d_element2, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT2]);
      } break;
      case ATTRIBUTE_ID_ELEMENT3: {
        return manipulator(&d_element3, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT3]);
      } break;
      default:
        return NOT_FOUND;
    }
}

template <class MANIPULATOR>
int UnsignedSequence::manipulateAttribute(
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
unsigned int& UnsignedSequence::element1()
{
    return d_element1;
}

inline
unsigned short& UnsignedSequence::element2()
{
    return d_element2;
}

inline
bsls_Types::Uint64& UnsignedSequence::element3()
{
    return d_element3;
}

// ACCESSORS
template <class STREAM>
STREAM& UnsignedSequence::bdexStreamOut(STREAM& stream, int version) const
{
    switch (version) {
      case 1: {
        bdex_OutStreamFunctions::streamOut(stream, d_element1, 1);
        bdex_OutStreamFunctions::streamOut(stream, d_element2, 1);
        bdex_OutStreamFunctions::streamOut(stream, d_element3, 1);
      } break;
    }
    return stream;
}

template <class ACCESSOR>
int UnsignedSequence::accessAttributes(ACCESSOR& accessor) const
{
    int ret;

    ret = accessor(d_element1, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT1]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_element2, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT2]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_element3, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT3]);
    if (ret) {
        return ret;
    }

    return ret;
}

template <class ACCESSOR>
int UnsignedSequence::accessAttribute(ACCESSOR& accessor, int id) const
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_ELEMENT1: {
        return accessor(d_element1, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT1]);
      } break;
      case ATTRIBUTE_ID_ELEMENT2: {
        return accessor(d_element2, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT2]);
      } break;
      case ATTRIBUTE_ID_ELEMENT3: {
        return accessor(d_element3, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT3]);
      } break;
      default:
        return NOT_FOUND;
    }
}

template <class ACCESSOR>
int UnsignedSequence::accessAttribute(
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
unsigned int UnsignedSequence::element1() const
{
    return d_element1;
}

inline
unsigned short UnsignedSequence::element2() const
{
    return d_element2;
}

inline
bsls_Types::Uint64 UnsignedSequence::element3() const
{
    return d_element3;
}



                             // ------------------                             
                             // class VoidSequence                             
                             // ------------------                             

// CLASS METHODS
inline
int VoidSequence::maxSupportedBdexVersion()
{
    return 1;  // versions start at 1.
}

// MANIPULATORS
template <class STREAM>
STREAM& VoidSequence::bdexStreamIn(STREAM& stream, int version)
{
    if (stream) {
        switch (version) {
          case 1: {
          } break;
          default: {
            stream.invalidate();
          }
        }
    }
    return stream;
}

template <class MANIPULATOR>
int VoidSequence::manipulateAttributes(MANIPULATOR& manipulator)
{
    int ret = 0;

    return ret;
}

template <class MANIPULATOR>
int VoidSequence::manipulateAttribute(MANIPULATOR& manipulator, int id)
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      default:
        return NOT_FOUND;
    }
}

template <class MANIPULATOR>
int VoidSequence::manipulateAttribute(
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

// ACCESSORS
template <class STREAM>
STREAM& VoidSequence::bdexStreamOut(STREAM& stream, int version) const
{
    switch (version) {
      case 1: {
      } break;
    }
    return stream;
}

template <class ACCESSOR>
int VoidSequence::accessAttributes(ACCESSOR& accessor) const
{
    int ret = 0;

    return ret;
}

template <class ACCESSOR>
int VoidSequence::accessAttribute(ACCESSOR& accessor, int id) const
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      default:
        return NOT_FOUND;
    }
}

template <class ACCESSOR>
int VoidSequence::accessAttribute(
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



                              // ---------------                               
                              // class Sequence3                               
                              // ---------------                               

// CLASS METHODS
inline
int Sequence3::maxSupportedBdexVersion()
{
    return 1;  // versions start at 1.
}

// MANIPULATORS
template <class STREAM>
STREAM& Sequence3::bdexStreamIn(STREAM& stream, int version)
{
    if (stream) {
        switch (version) {
          case 1: {
            bdex_InStreamFunctions::streamIn(stream, d_element1, 1);
            bdex_InStreamFunctions::streamIn(stream, d_element2, 1);
            bdex_InStreamFunctions::streamIn(stream, d_element3, 1);
            bdex_InStreamFunctions::streamIn(stream, d_element4, 1);
            bdex_InStreamFunctions::streamIn(stream, d_element5, 1);
            bdex_InStreamFunctions::streamIn(stream, d_element6, 1);
          } break;
          default: {
            stream.invalidate();
          }
        }
    }
    return stream;
}

template <class MANIPULATOR>
int Sequence3::manipulateAttributes(MANIPULATOR& manipulator)
{
    int ret;

    ret = manipulator(&d_element1, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT1]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_element2, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT2]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_element3, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT3]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_element4, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT4]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_element5, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT5]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_element6, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT6]);
    if (ret) {
        return ret;
    }

    return ret;
}

template <class MANIPULATOR>
int Sequence3::manipulateAttribute(MANIPULATOR& manipulator, int id)
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_ELEMENT1: {
        return manipulator(&d_element1, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT1]);
      } break;
      case ATTRIBUTE_ID_ELEMENT2: {
        return manipulator(&d_element2, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT2]);
      } break;
      case ATTRIBUTE_ID_ELEMENT3: {
        return manipulator(&d_element3, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT3]);
      } break;
      case ATTRIBUTE_ID_ELEMENT4: {
        return manipulator(&d_element4, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT4]);
      } break;
      case ATTRIBUTE_ID_ELEMENT5: {
        return manipulator(&d_element5, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT5]);
      } break;
      case ATTRIBUTE_ID_ELEMENT6: {
        return manipulator(&d_element6, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT6]);
      } break;
      default:
        return NOT_FOUND;
    }
}

template <class MANIPULATOR>
int Sequence3::manipulateAttribute(
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
std::vector<Enumerated::Value>& Sequence3::element1()
{
    return d_element1;
}

inline
std::vector<std::string>& Sequence3::element2()
{
    return d_element2;
}

inline
bdeut_NullableValue<bool>& Sequence3::element3()
{
    return d_element3;
}

inline
bdeut_NullableValue<std::string>& Sequence3::element4()
{
    return d_element4;
}

inline
bdeut_NullableAllocatedValue<Sequence5>& Sequence3::element5()
{
    return d_element5;
}

inline
std::vector<bdeut_NullableValue<Enumerated::Value> >& Sequence3::element6()
{
    return d_element6;
}

// ACCESSORS
template <class STREAM>
STREAM& Sequence3::bdexStreamOut(STREAM& stream, int version) const
{
    switch (version) {
      case 1: {
        bdex_OutStreamFunctions::streamOut(stream, d_element1, 1);
        bdex_OutStreamFunctions::streamOut(stream, d_element2, 1);
        bdex_OutStreamFunctions::streamOut(stream, d_element3, 1);
        bdex_OutStreamFunctions::streamOut(stream, d_element4, 1);
        bdex_OutStreamFunctions::streamOut(stream, d_element5, 1);
        bdex_OutStreamFunctions::streamOut(stream, d_element6, 1);
      } break;
    }
    return stream;
}

template <class ACCESSOR>
int Sequence3::accessAttributes(ACCESSOR& accessor) const
{
    int ret;

    ret = accessor(d_element1, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT1]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_element2, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT2]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_element3, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT3]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_element4, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT4]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_element5, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT5]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_element6, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT6]);
    if (ret) {
        return ret;
    }

    return ret;
}

template <class ACCESSOR>
int Sequence3::accessAttribute(ACCESSOR& accessor, int id) const
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_ELEMENT1: {
        return accessor(d_element1, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT1]);
      } break;
      case ATTRIBUTE_ID_ELEMENT2: {
        return accessor(d_element2, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT2]);
      } break;
      case ATTRIBUTE_ID_ELEMENT3: {
        return accessor(d_element3, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT3]);
      } break;
      case ATTRIBUTE_ID_ELEMENT4: {
        return accessor(d_element4, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT4]);
      } break;
      case ATTRIBUTE_ID_ELEMENT5: {
        return accessor(d_element5, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT5]);
      } break;
      case ATTRIBUTE_ID_ELEMENT6: {
        return accessor(d_element6, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT6]);
      } break;
      default:
        return NOT_FOUND;
    }
}

template <class ACCESSOR>
int Sequence3::accessAttribute(
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
const std::vector<Enumerated::Value>& Sequence3::element1() const
{
    return d_element1;
}

inline
const std::vector<std::string>& Sequence3::element2() const
{
    return d_element2;
}

inline
const bdeut_NullableValue<bool>& Sequence3::element3() const
{
    return d_element3;
}

inline
const bdeut_NullableValue<std::string>& Sequence3::element4() const
{
    return d_element4;
}

inline
const bdeut_NullableAllocatedValue<Sequence5>& Sequence3::element5() const
{
    return d_element5;
}

inline
const std::vector<bdeut_NullableValue<Enumerated::Value> >& Sequence3::element6() const
{
    return d_element6;
}



                              // ---------------                               
                              // class Sequence5                               
                              // ---------------                               

// CLASS METHODS
inline
int Sequence5::maxSupportedBdexVersion()
{
    return 1;  // versions start at 1.
}

// MANIPULATORS
template <class STREAM>
STREAM& Sequence5::bdexStreamIn(STREAM& stream, int version)
{
    if (stream) {
        switch (version) {
          case 1: {
            bdex_InStreamFunctions::streamIn(stream, *d_element1, 1);
            bdex_InStreamFunctions::streamIn(stream, d_element2, 1);
            bdex_InStreamFunctions::streamIn(stream, d_element3, 1);
            bdex_InStreamFunctions::streamIn(stream, d_element4, 1);
            bdex_InStreamFunctions::streamIn(stream, d_element5, 1);
            bdex_InStreamFunctions::streamIn(stream, d_element6, 1);
            bdex_InStreamFunctions::streamIn(stream, d_element7, 1);
          } break;
          default: {
            stream.invalidate();
          }
        }
    }
    return stream;
}

template <class MANIPULATOR>
int Sequence5::manipulateAttributes(MANIPULATOR& manipulator)
{
    int ret;

    ret = manipulator(d_element1, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT1]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_element2, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT2]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_element3, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT3]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_element4, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT4]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_element5, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT5]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_element6, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT6]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_element7, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT7]);
    if (ret) {
        return ret;
    }

    return ret;
}

template <class MANIPULATOR>
int Sequence5::manipulateAttribute(MANIPULATOR& manipulator, int id)
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_ELEMENT1: {
        return manipulator(d_element1, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT1]);
      } break;
      case ATTRIBUTE_ID_ELEMENT2: {
        return manipulator(&d_element2, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT2]);
      } break;
      case ATTRIBUTE_ID_ELEMENT3: {
        return manipulator(&d_element3, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT3]);
      } break;
      case ATTRIBUTE_ID_ELEMENT4: {
        return manipulator(&d_element4, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT4]);
      } break;
      case ATTRIBUTE_ID_ELEMENT5: {
        return manipulator(&d_element5, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT5]);
      } break;
      case ATTRIBUTE_ID_ELEMENT6: {
        return manipulator(&d_element6, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT6]);
      } break;
      case ATTRIBUTE_ID_ELEMENT7: {
        return manipulator(&d_element7, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT7]);
      } break;
      default:
        return NOT_FOUND;
    }
}

template <class MANIPULATOR>
int Sequence5::manipulateAttribute(
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
Sequence3& Sequence5::element1()
{
    return *d_element1;
}

inline
std::vector<bdeut_NullableValue<bool> >& Sequence5::element2()
{
    return d_element2;
}

inline
std::vector<bdeut_NullableValue<double> >& Sequence5::element3()
{
    return d_element3;
}

inline
std::vector<bdeut_NullableValue<std::vector<char> > >& Sequence5::element4()
{
    return d_element4;
}

inline
std::vector<bdeut_NullableValue<int> >& Sequence5::element5()
{
    return d_element5;
}

inline
std::vector<bdeut_NullableValue<bdet_DatetimeTz> >& Sequence5::element6()
{
    return d_element6;
}

inline
std::vector<bdeut_NullableAllocatedValue<Sequence3> >& Sequence5::element7()
{
    return d_element7;
}

// ACCESSORS
template <class STREAM>
STREAM& Sequence5::bdexStreamOut(STREAM& stream, int version) const
{
    switch (version) {
      case 1: {
        bdex_OutStreamFunctions::streamOut(stream, *d_element1, 1);
        bdex_OutStreamFunctions::streamOut(stream, d_element2, 1);
        bdex_OutStreamFunctions::streamOut(stream, d_element3, 1);
        bdex_OutStreamFunctions::streamOut(stream, d_element4, 1);
        bdex_OutStreamFunctions::streamOut(stream, d_element5, 1);
        bdex_OutStreamFunctions::streamOut(stream, d_element6, 1);
        bdex_OutStreamFunctions::streamOut(stream, d_element7, 1);
      } break;
    }
    return stream;
}

template <class ACCESSOR>
int Sequence5::accessAttributes(ACCESSOR& accessor) const
{
    int ret;

    ret = accessor(*d_element1, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT1]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_element2, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT2]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_element3, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT3]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_element4, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT4]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_element5, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT5]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_element6, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT6]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_element7, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT7]);
    if (ret) {
        return ret;
    }

    return ret;
}

template <class ACCESSOR>
int Sequence5::accessAttribute(ACCESSOR& accessor, int id) const
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_ELEMENT1: {
        return accessor(*d_element1, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT1]);
      } break;
      case ATTRIBUTE_ID_ELEMENT2: {
        return accessor(d_element2, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT2]);
      } break;
      case ATTRIBUTE_ID_ELEMENT3: {
        return accessor(d_element3, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT3]);
      } break;
      case ATTRIBUTE_ID_ELEMENT4: {
        return accessor(d_element4, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT4]);
      } break;
      case ATTRIBUTE_ID_ELEMENT5: {
        return accessor(d_element5, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT5]);
      } break;
      case ATTRIBUTE_ID_ELEMENT6: {
        return accessor(d_element6, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT6]);
      } break;
      case ATTRIBUTE_ID_ELEMENT7: {
        return accessor(d_element7, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT7]);
      } break;
      default:
        return NOT_FOUND;
    }
}

template <class ACCESSOR>
int Sequence5::accessAttribute(
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
const Sequence3& Sequence5::element1() const
{
    return *d_element1;
}

inline
const std::vector<bdeut_NullableValue<bool> >& Sequence5::element2() const
{
    return d_element2;
}

inline
const std::vector<bdeut_NullableValue<double> >& Sequence5::element3() const
{
    return d_element3;
}

inline
const std::vector<bdeut_NullableValue<std::vector<char> > >& Sequence5::element4() const
{
    return d_element4;
}

inline
const std::vector<bdeut_NullableValue<int> >& Sequence5::element5() const
{
    return d_element5;
}

inline
const std::vector<bdeut_NullableValue<bdet_DatetimeTz> >& Sequence5::element6() const
{
    return d_element6;
}

inline
const std::vector<bdeut_NullableAllocatedValue<Sequence3> >& Sequence5::element7() const
{
    return d_element7;
}



                              // ---------------                               
                              // class Sequence6                               
                              // ---------------                               

// CLASS METHODS
inline
int Sequence6::maxSupportedBdexVersion()
{
    return 1;  // versions start at 1.
}

// MANIPULATORS
template <class STREAM>
STREAM& Sequence6::bdexStreamIn(STREAM& stream, int version)
{
    if (stream) {
        switch (version) {
          case 1: {
            bdex_InStreamFunctions::streamIn(stream, d_element1, 1);
            bdex_InStreamFunctions::streamIn(stream, d_element2, 1);
            bdex_InStreamFunctions::streamIn(stream, d_element3, 1);
            bdex_InStreamFunctions::streamIn(stream, d_element4, 1);
            bdex_InStreamFunctions::streamIn(stream, d_element5, 1);
            bdex_InStreamFunctions::streamIn(stream, d_element6, 1);
            bdex_InStreamFunctions::streamIn(stream, d_element7, 1);
            bdex_InStreamFunctions::streamIn(stream, d_element8, 1);
            bdex_InStreamFunctions::streamIn(stream, d_element9, 1);
            bdex_InStreamFunctions::streamIn(stream, d_element10, 1);
            bdex_InStreamFunctions::streamIn(stream, d_element11, 1);
          } break;
          default: {
            stream.invalidate();
          }
        }
    }
    return stream;
}

template <class MANIPULATOR>
int Sequence6::manipulateAttributes(MANIPULATOR& manipulator)
{
    int ret;

    ret = manipulator(&d_element1, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT1]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_element2, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT2]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_element3, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT3]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_element4, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT4]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_element5, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT5]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_element6, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT6]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_element7, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT7]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_element8, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT8]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_element9, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT9]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_element10, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT10]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_element11, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT11]);
    if (ret) {
        return ret;
    }

    return ret;
}

template <class MANIPULATOR>
int Sequence6::manipulateAttribute(MANIPULATOR& manipulator, int id)
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_ELEMENT1: {
        return manipulator(&d_element1, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT1]);
      } break;
      case ATTRIBUTE_ID_ELEMENT2: {
        return manipulator(&d_element2, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT2]);
      } break;
      case ATTRIBUTE_ID_ELEMENT3: {
        return manipulator(&d_element3, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT3]);
      } break;
      case ATTRIBUTE_ID_ELEMENT4: {
        return manipulator(&d_element4, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT4]);
      } break;
      case ATTRIBUTE_ID_ELEMENT5: {
        return manipulator(&d_element5, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT5]);
      } break;
      case ATTRIBUTE_ID_ELEMENT6: {
        return manipulator(&d_element6, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT6]);
      } break;
      case ATTRIBUTE_ID_ELEMENT7: {
        return manipulator(&d_element7, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT7]);
      } break;
      case ATTRIBUTE_ID_ELEMENT8: {
        return manipulator(&d_element8, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT8]);
      } break;
      case ATTRIBUTE_ID_ELEMENT9: {
        return manipulator(&d_element9, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT9]);
      } break;
      case ATTRIBUTE_ID_ELEMENT10: {
        return manipulator(&d_element10, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT10]);
      } break;
      case ATTRIBUTE_ID_ELEMENT11: {
        return manipulator(&d_element11, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT11]);
      } break;
      default:
        return NOT_FOUND;
    }
}

template <class MANIPULATOR>
int Sequence6::manipulateAttribute(
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
bdeut_NullableValue<unsigned char>& Sequence6::element1()
{
    return d_element1;
}

inline
bdeut_NullableValue<CustomString>& Sequence6::element2()
{
    return d_element2;
}

inline
bdeut_NullableValue<CustomInt>& Sequence6::element3()
{
    return d_element3;
}

inline
unsigned char& Sequence6::element4()
{
    return d_element4;
}

inline
std::vector<bdeut_NullableValue<CustomInt> >& Sequence6::element5()
{
    return d_element5;
}

inline
CustomString& Sequence6::element6()
{
    return d_element6;
}

inline
CustomInt& Sequence6::element7()
{
    return d_element7;
}

inline
std::vector<unsigned char>& Sequence6::element8()
{
    return d_element8;
}

inline
std::vector<CustomString>& Sequence6::element9()
{
    return d_element9;
}

inline
std::vector<bdeut_NullableValue<unsigned char> >& Sequence6::element10()
{
    return d_element10;
}

inline
std::vector<CustomInt>& Sequence6::element11()
{
    return d_element11;
}

// ACCESSORS
template <class STREAM>
STREAM& Sequence6::bdexStreamOut(STREAM& stream, int version) const
{
    switch (version) {
      case 1: {
        bdex_OutStreamFunctions::streamOut(stream, d_element1, 1);
        bdex_OutStreamFunctions::streamOut(stream, d_element2, 1);
        bdex_OutStreamFunctions::streamOut(stream, d_element3, 1);
        bdex_OutStreamFunctions::streamOut(stream, d_element4, 1);
        bdex_OutStreamFunctions::streamOut(stream, d_element5, 1);
        bdex_OutStreamFunctions::streamOut(stream, d_element6, 1);
        bdex_OutStreamFunctions::streamOut(stream, d_element7, 1);
        bdex_OutStreamFunctions::streamOut(stream, d_element8, 1);
        bdex_OutStreamFunctions::streamOut(stream, d_element9, 1);
        bdex_OutStreamFunctions::streamOut(stream, d_element10, 1);
        bdex_OutStreamFunctions::streamOut(stream, d_element11, 1);
      } break;
    }
    return stream;
}

template <class ACCESSOR>
int Sequence6::accessAttributes(ACCESSOR& accessor) const
{
    int ret;

    ret = accessor(d_element1, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT1]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_element2, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT2]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_element3, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT3]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_element4, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT4]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_element5, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT5]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_element6, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT6]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_element7, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT7]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_element8, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT8]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_element9, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT9]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_element10, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT10]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_element11, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT11]);
    if (ret) {
        return ret;
    }

    return ret;
}

template <class ACCESSOR>
int Sequence6::accessAttribute(ACCESSOR& accessor, int id) const
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_ELEMENT1: {
        return accessor(d_element1, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT1]);
      } break;
      case ATTRIBUTE_ID_ELEMENT2: {
        return accessor(d_element2, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT2]);
      } break;
      case ATTRIBUTE_ID_ELEMENT3: {
        return accessor(d_element3, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT3]);
      } break;
      case ATTRIBUTE_ID_ELEMENT4: {
        return accessor(d_element4, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT4]);
      } break;
      case ATTRIBUTE_ID_ELEMENT5: {
        return accessor(d_element5, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT5]);
      } break;
      case ATTRIBUTE_ID_ELEMENT6: {
        return accessor(d_element6, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT6]);
      } break;
      case ATTRIBUTE_ID_ELEMENT7: {
        return accessor(d_element7, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT7]);
      } break;
      case ATTRIBUTE_ID_ELEMENT8: {
        return accessor(d_element8, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT8]);
      } break;
      case ATTRIBUTE_ID_ELEMENT9: {
        return accessor(d_element9, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT9]);
      } break;
      case ATTRIBUTE_ID_ELEMENT10: {
        return accessor(d_element10, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT10]);
      } break;
      case ATTRIBUTE_ID_ELEMENT11: {
        return accessor(d_element11, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT11]);
      } break;
      default:
        return NOT_FOUND;
    }
}

template <class ACCESSOR>
int Sequence6::accessAttribute(
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
const bdeut_NullableValue<unsigned char>& Sequence6::element1() const
{
    return d_element1;
}

inline
const bdeut_NullableValue<CustomString>& Sequence6::element2() const
{
    return d_element2;
}

inline
const bdeut_NullableValue<CustomInt>& Sequence6::element3() const
{
    return d_element3;
}

inline
unsigned char Sequence6::element4() const
{
    return d_element4;
}

inline
const std::vector<bdeut_NullableValue<CustomInt> >& Sequence6::element5() const
{
    return d_element5;
}

inline
const CustomString& Sequence6::element6() const
{
    return d_element6;
}

inline
const CustomInt& Sequence6::element7() const
{
    return d_element7;
}

inline
const std::vector<unsigned char>& Sequence6::element8() const
{
    return d_element8;
}

inline
const std::vector<CustomString>& Sequence6::element9() const
{
    return d_element9;
}

inline
const std::vector<bdeut_NullableValue<unsigned char> >& Sequence6::element10() const
{
    return d_element10;
}

inline
const std::vector<CustomInt>& Sequence6::element11() const
{
    return d_element11;
}



                               // -------------                                
                               // class Choice3                                
                               // -------------                                

// CLASS METHODS
inline
int Choice3::maxSupportedBdexVersion()
{
    return 1;  // versions start at 1.
}

// CREATORS
inline
Choice3::Choice3(bdema_Allocator *basicAllocator)
: d_selectionId(SELECTION_ID_UNDEFINED)
, d_allocator_p(bdema_Default::allocator(basicAllocator))
{
}

inline
Choice3::~Choice3()
{
    reset();
}

// MANIPULATORS
template <class STREAM>
STREAM& Choice3::bdexStreamIn(STREAM& stream, int version)
{
    if (stream) {
        switch (version) {
          case 1: {
            short selectionId;
            stream.getInt16(selectionId);
            if (!stream) {
                return stream;
            }
            switch (selectionId) {
              case SELECTION_ID_SELECTION1: {
                makeSelection1();
                bdex_InStreamFunctions::streamIn(
                    stream, d_selection1.object(), 1);
              } break;
              case SELECTION_ID_SELECTION2: {
                makeSelection2();
                bdex_InStreamFunctions::streamIn(
                    stream, d_selection2.object(), 1);
              } break;
              case SELECTION_ID_SELECTION3: {
                makeSelection3();
                bdex_InStreamFunctions::streamIn(
                    stream, d_selection3.object(), 1);
              } break;
              case SELECTION_ID_SELECTION4: {
                makeSelection4();
                bdex_InStreamFunctions::streamIn(
                    stream, d_selection4.object(), 1);
              } break;
              case SELECTION_ID_UNDEFINED: {
                reset();
              } break;
              default:
                stream.invalidate();
            }
          } break;
          default: {
            stream.invalidate();
          }
        }
    }
    return stream;
}

template <class MANIPULATOR>
int Choice3::manipulateSelection(MANIPULATOR& manipulator)
{
    switch (d_selectionId) {
      case Choice3::SELECTION_ID_SELECTION1:
        return manipulator(&d_selection1.object(),
                SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION1]);
      case Choice3::SELECTION_ID_SELECTION2:
        return manipulator(&d_selection2.object(),
                SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION2]);
      case Choice3::SELECTION_ID_SELECTION3:
        return manipulator(&d_selection3.object(),
                SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION3]);
      case Choice3::SELECTION_ID_SELECTION4:
        return manipulator(&d_selection4.object(),
                SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION4]);
      default:
        BSLS_ASSERT(Choice3::SELECTION_ID_UNDEFINED == d_selectionId);
        return -1;
    }
}

inline
Sequence6& Choice3::selection1()
{
    BSLS_ASSERT(SELECTION_ID_SELECTION1 == d_selectionId);
    return d_selection1.object();
}

inline
unsigned char& Choice3::selection2()
{
    BSLS_ASSERT(SELECTION_ID_SELECTION2 == d_selectionId);
    return d_selection2.object();
}

inline
CustomString& Choice3::selection3()
{
    BSLS_ASSERT(SELECTION_ID_SELECTION3 == d_selectionId);
    return d_selection3.object();
}

inline
CustomInt& Choice3::selection4()
{
    BSLS_ASSERT(SELECTION_ID_SELECTION4 == d_selectionId);
    return d_selection4.object();
}

// ACCESSORS
template <class STREAM>
STREAM& Choice3::bdexStreamOut(STREAM& stream, int version) const
{
    switch (version) {
      case 1: {
            stream.putInt16(d_selectionId);
            switch (d_selectionId) {
              case SELECTION_ID_SELECTION1: {
                bdex_OutStreamFunctions::streamOut(
                    stream, d_selection1.object(), 1);
              } break;
              case SELECTION_ID_SELECTION2: {
                bdex_OutStreamFunctions::streamOut(
                    stream, d_selection2.object(), 1);
              } break;
              case SELECTION_ID_SELECTION3: {
                bdex_OutStreamFunctions::streamOut(
                    stream, d_selection3.object(), 1);
              } break;
              case SELECTION_ID_SELECTION4: {
                bdex_OutStreamFunctions::streamOut(
                    stream, d_selection4.object(), 1);
              } break;
              default:
                BSLS_ASSERT(SELECTION_ID_UNDEFINED == d_selectionId);
            }
      } break;
    }
    return stream;
}

inline
int Choice3::selectionId() const
{
    return d_selectionId;
}

template <class ACCESSOR>
int Choice3::accessSelection(ACCESSOR& accessor) const
{
    switch (d_selectionId) {
      case SELECTION_ID_SELECTION1:
        return accessor(d_selection1.object(),
                SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION1]);
      case SELECTION_ID_SELECTION2:
        return accessor(d_selection2.object(),
                SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION2]);
      case SELECTION_ID_SELECTION3:
        return accessor(d_selection3.object(),
                SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION3]);
      case SELECTION_ID_SELECTION4:
        return accessor(d_selection4.object(),
                SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION4]);
      default:
        BSLS_ASSERT(SELECTION_ID_UNDEFINED == d_selectionId);
        return -1;
    }
}

inline
const Sequence6& Choice3::selection1() const
{
    BSLS_ASSERT(SELECTION_ID_SELECTION1 == d_selectionId);
    return d_selection1.object();
}

inline
const unsigned char& Choice3::selection2() const
{
    BSLS_ASSERT(SELECTION_ID_SELECTION2 == d_selectionId);
    return d_selection2.object();
}

inline
const CustomString& Choice3::selection3() const
{
    BSLS_ASSERT(SELECTION_ID_SELECTION3 == d_selectionId);
    return d_selection3.object();
}

inline
const CustomInt& Choice3::selection4() const
{
    BSLS_ASSERT(SELECTION_ID_SELECTION4 == d_selectionId);
    return d_selection4.object();
}

inline
bool Choice3::isSelection1Value() const
{
    return SELECTION_ID_SELECTION1 == d_selectionId;
}

inline
bool Choice3::isSelection2Value() const
{
    return SELECTION_ID_SELECTION2 == d_selectionId;
}

inline
bool Choice3::isSelection3Value() const
{
    return SELECTION_ID_SELECTION3 == d_selectionId;
}

inline
bool Choice3::isSelection4Value() const
{
    return SELECTION_ID_SELECTION4 == d_selectionId;
}

inline
bool Choice3::isUndefinedValue() const
{
    return SELECTION_ID_UNDEFINED == d_selectionId;
}


                               // -------------                                
                               // class Choice1                                
                               // -------------                                

// CLASS METHODS
inline
int Choice1::maxSupportedBdexVersion()
{
    return 1;  // versions start at 1.
}

// CREATORS
inline
Choice1::Choice1(bdema_Allocator *basicAllocator)
: d_selectionId(SELECTION_ID_UNDEFINED)
, d_allocator_p(bdema_Default::allocator(basicAllocator))
{
}

inline
Choice1::~Choice1()
{
    reset();
}

// MANIPULATORS
template <class STREAM>
STREAM& Choice1::bdexStreamIn(STREAM& stream, int version)
{
    if (stream) {
        switch (version) {
          case 1: {
            short selectionId;
            stream.getInt16(selectionId);
            if (!stream) {
                return stream;
            }
            switch (selectionId) {
              case SELECTION_ID_SELECTION1: {
                makeSelection1();
                bdex_InStreamFunctions::streamIn(
                    stream, d_selection1.object(), 1);
              } break;
              case SELECTION_ID_SELECTION2: {
                makeSelection2();
                bdex_InStreamFunctions::streamIn(
                    stream, d_selection2.object(), 1);
              } break;
              case SELECTION_ID_SELECTION3: {
                makeSelection3();
                bdex_InStreamFunctions::streamIn(
                    stream, *d_selection3, 1);
              } break;
              case SELECTION_ID_SELECTION4: {
                makeSelection4();
                bdex_InStreamFunctions::streamIn(
                    stream, *d_selection4, 1);
              } break;
              case SELECTION_ID_UNDEFINED: {
                reset();
              } break;
              default:
                stream.invalidate();
            }
          } break;
          default: {
            stream.invalidate();
          }
        }
    }
    return stream;
}

template <class MANIPULATOR>
int Choice1::manipulateSelection(MANIPULATOR& manipulator)
{
    switch (d_selectionId) {
      case Choice1::SELECTION_ID_SELECTION1:
        return manipulator(&d_selection1.object(),
                SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION1]);
      case Choice1::SELECTION_ID_SELECTION2:
        return manipulator(&d_selection2.object(),
                SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION2]);
      case Choice1::SELECTION_ID_SELECTION3:
        return manipulator(d_selection3,
                SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION3]);
      case Choice1::SELECTION_ID_SELECTION4:
        return manipulator(d_selection4,
                SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION4]);
      default:
        BSLS_ASSERT(Choice1::SELECTION_ID_UNDEFINED == d_selectionId);
        return -1;
    }
}

inline
int& Choice1::selection1()
{
    BSLS_ASSERT(SELECTION_ID_SELECTION1 == d_selectionId);
    return d_selection1.object();
}

inline
double& Choice1::selection2()
{
    BSLS_ASSERT(SELECTION_ID_SELECTION2 == d_selectionId);
    return d_selection2.object();
}

inline
Sequence4& Choice1::selection3()
{
    BSLS_ASSERT(SELECTION_ID_SELECTION3 == d_selectionId);
    return *d_selection3;
}

inline
Choice2& Choice1::selection4()
{
    BSLS_ASSERT(SELECTION_ID_SELECTION4 == d_selectionId);
    return *d_selection4;
}

// ACCESSORS
template <class STREAM>
STREAM& Choice1::bdexStreamOut(STREAM& stream, int version) const
{
    switch (version) {
      case 1: {
            stream.putInt16(d_selectionId);
            switch (d_selectionId) {
              case SELECTION_ID_SELECTION1: {
                bdex_OutStreamFunctions::streamOut(
                    stream, d_selection1.object(), 1);
              } break;
              case SELECTION_ID_SELECTION2: {
                bdex_OutStreamFunctions::streamOut(
                    stream, d_selection2.object(), 1);
              } break;
              case SELECTION_ID_SELECTION3: {
                bdex_OutStreamFunctions::streamOut(
                    stream, *d_selection3, 1);
              } break;
              case SELECTION_ID_SELECTION4: {
                bdex_OutStreamFunctions::streamOut(
                    stream, *d_selection4, 1);
              } break;
              default:
                BSLS_ASSERT(SELECTION_ID_UNDEFINED == d_selectionId);
            }
      } break;
    }
    return stream;
}

inline
int Choice1::selectionId() const
{
    return d_selectionId;
}

template <class ACCESSOR>
int Choice1::accessSelection(ACCESSOR& accessor) const
{
    switch (d_selectionId) {
      case SELECTION_ID_SELECTION1:
        return accessor(d_selection1.object(),
                SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION1]);
      case SELECTION_ID_SELECTION2:
        return accessor(d_selection2.object(),
                SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION2]);
      case SELECTION_ID_SELECTION3:
        return accessor(*d_selection3,
                SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION3]);
      case SELECTION_ID_SELECTION4:
        return accessor(*d_selection4,
                SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION4]);
      default:
        BSLS_ASSERT(SELECTION_ID_UNDEFINED == d_selectionId);
        return -1;
    }
}

inline
const int& Choice1::selection1() const
{
    BSLS_ASSERT(SELECTION_ID_SELECTION1 == d_selectionId);
    return d_selection1.object();
}

inline
const double& Choice1::selection2() const
{
    BSLS_ASSERT(SELECTION_ID_SELECTION2 == d_selectionId);
    return d_selection2.object();
}

inline
const Sequence4& Choice1::selection3() const
{
    BSLS_ASSERT(SELECTION_ID_SELECTION3 == d_selectionId);
    return *d_selection3;
}

inline
const Choice2& Choice1::selection4() const
{
    BSLS_ASSERT(SELECTION_ID_SELECTION4 == d_selectionId);
    return *d_selection4;
}

inline
bool Choice1::isSelection1Value() const
{
    return SELECTION_ID_SELECTION1 == d_selectionId;
}

inline
bool Choice1::isSelection2Value() const
{
    return SELECTION_ID_SELECTION2 == d_selectionId;
}

inline
bool Choice1::isSelection3Value() const
{
    return SELECTION_ID_SELECTION3 == d_selectionId;
}

inline
bool Choice1::isSelection4Value() const
{
    return SELECTION_ID_SELECTION4 == d_selectionId;
}

inline
bool Choice1::isUndefinedValue() const
{
    return SELECTION_ID_UNDEFINED == d_selectionId;
}


                               // -------------                                
                               // class Choice2                                
                               // -------------                                

// CLASS METHODS
inline
int Choice2::maxSupportedBdexVersion()
{
    return 1;  // versions start at 1.
}

// CREATORS
inline
Choice2::Choice2(bdema_Allocator *basicAllocator)
: d_selectionId(SELECTION_ID_UNDEFINED)
, d_allocator_p(bdema_Default::allocator(basicAllocator))
{
}

inline
Choice2::~Choice2()
{
    reset();
}

// MANIPULATORS
template <class STREAM>
STREAM& Choice2::bdexStreamIn(STREAM& stream, int version)
{
    if (stream) {
        switch (version) {
          case 1: {
            short selectionId;
            stream.getInt16(selectionId);
            if (!stream) {
                return stream;
            }
            switch (selectionId) {
              case SELECTION_ID_SELECTION1: {
                makeSelection1();
                bdex_InStreamFunctions::streamIn(
                    stream, d_selection1.object(), 1);
              } break;
              case SELECTION_ID_SELECTION2: {
                makeSelection2();
                bdex_InStreamFunctions::streamIn(
                    stream, d_selection2.object(), 1);
              } break;
              case SELECTION_ID_SELECTION3: {
                makeSelection3();
                bdex_InStreamFunctions::streamIn(
                    stream, *d_selection3, 1);
              } break;
              case SELECTION_ID_UNDEFINED: {
                reset();
              } break;
              default:
                stream.invalidate();
            }
          } break;
          default: {
            stream.invalidate();
          }
        }
    }
    return stream;
}

template <class MANIPULATOR>
int Choice2::manipulateSelection(MANIPULATOR& manipulator)
{
    switch (d_selectionId) {
      case Choice2::SELECTION_ID_SELECTION1:
        return manipulator(&d_selection1.object(),
                SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION1]);
      case Choice2::SELECTION_ID_SELECTION2:
        return manipulator(&d_selection2.object(),
                SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION2]);
      case Choice2::SELECTION_ID_SELECTION3:
        return manipulator(d_selection3,
                SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION3]);
      default:
        BSLS_ASSERT(Choice2::SELECTION_ID_UNDEFINED == d_selectionId);
        return -1;
    }
}

inline
bool& Choice2::selection1()
{
    BSLS_ASSERT(SELECTION_ID_SELECTION1 == d_selectionId);
    return d_selection1.object();
}

inline
std::string& Choice2::selection2()
{
    BSLS_ASSERT(SELECTION_ID_SELECTION2 == d_selectionId);
    return d_selection2.object();
}

inline
Choice1& Choice2::selection3()
{
    BSLS_ASSERT(SELECTION_ID_SELECTION3 == d_selectionId);
    return *d_selection3;
}

// ACCESSORS
template <class STREAM>
STREAM& Choice2::bdexStreamOut(STREAM& stream, int version) const
{
    switch (version) {
      case 1: {
            stream.putInt16(d_selectionId);
            switch (d_selectionId) {
              case SELECTION_ID_SELECTION1: {
                bdex_OutStreamFunctions::streamOut(
                    stream, d_selection1.object(), 1);
              } break;
              case SELECTION_ID_SELECTION2: {
                bdex_OutStreamFunctions::streamOut(
                    stream, d_selection2.object(), 1);
              } break;
              case SELECTION_ID_SELECTION3: {
                bdex_OutStreamFunctions::streamOut(
                    stream, *d_selection3, 1);
              } break;
              default:
                BSLS_ASSERT(SELECTION_ID_UNDEFINED == d_selectionId);
            }
      } break;
    }
    return stream;
}

inline
int Choice2::selectionId() const
{
    return d_selectionId;
}

template <class ACCESSOR>
int Choice2::accessSelection(ACCESSOR& accessor) const
{
    switch (d_selectionId) {
      case SELECTION_ID_SELECTION1:
        return accessor(d_selection1.object(),
                SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION1]);
      case SELECTION_ID_SELECTION2:
        return accessor(d_selection2.object(),
                SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION2]);
      case SELECTION_ID_SELECTION3:
        return accessor(*d_selection3,
                SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION3]);
      default:
        BSLS_ASSERT(SELECTION_ID_UNDEFINED == d_selectionId);
        return -1;
    }
}

inline
const bool& Choice2::selection1() const
{
    BSLS_ASSERT(SELECTION_ID_SELECTION1 == d_selectionId);
    return d_selection1.object();
}

inline
const std::string& Choice2::selection2() const
{
    BSLS_ASSERT(SELECTION_ID_SELECTION2 == d_selectionId);
    return d_selection2.object();
}

inline
const Choice1& Choice2::selection3() const
{
    BSLS_ASSERT(SELECTION_ID_SELECTION3 == d_selectionId);
    return *d_selection3;
}

inline
bool Choice2::isSelection1Value() const
{
    return SELECTION_ID_SELECTION1 == d_selectionId;
}

inline
bool Choice2::isSelection2Value() const
{
    return SELECTION_ID_SELECTION2 == d_selectionId;
}

inline
bool Choice2::isSelection3Value() const
{
    return SELECTION_ID_SELECTION3 == d_selectionId;
}

inline
bool Choice2::isUndefinedValue() const
{
    return SELECTION_ID_UNDEFINED == d_selectionId;
}


                              // ---------------                               
                              // class Sequence4                               
                              // ---------------                               

// CLASS METHODS
inline
int Sequence4::maxSupportedBdexVersion()
{
    return 1;  // versions start at 1.
}

// MANIPULATORS
template <class STREAM>
STREAM& Sequence4::bdexStreamIn(STREAM& stream, int version)
{
    if (stream) {
        switch (version) {
          case 1: {
            bdex_InStreamFunctions::streamIn(stream, d_element1, 1);
            bdex_InStreamFunctions::streamIn(stream, d_element2, 1);
            bdex_InStreamFunctions::streamIn(stream, d_element3, 1);
            bdex_InStreamFunctions::streamIn(stream, d_element4, 1);
            bdex_InStreamFunctions::streamIn(stream, d_element5, 1);
            bdex_InStreamFunctions::streamIn(stream, d_element6, 1);
            bdex_InStreamFunctions::streamIn(stream, d_element7, 1);
            bdex_InStreamFunctions::streamIn(stream, d_element8, 1);
            bdex_InStreamFunctions::streamIn(stream, d_element9, 1);
            bdex_InStreamFunctions::streamIn(stream, d_element10, 1);
            bdex_InStreamFunctions::streamIn(stream, d_element11, 1);
            bdex_InStreamFunctions::streamIn(stream, d_element12, 1);
            Enumerated::bdexStreamIn(stream, d_element13, 1);
            bdex_InStreamFunctions::streamIn(stream, d_element14, 1);
            bdex_InStreamFunctions::streamIn(stream, d_element15, 1);
            bdex_InStreamFunctions::streamIn(stream, d_element16, 1);
            bdex_InStreamFunctions::streamIn(stream, d_element17, 1);
            bdex_InStreamFunctions::streamIn(stream, d_element18, 1);
            bdex_InStreamFunctions::streamIn(stream, d_element19, 1);
          } break;
          default: {
            stream.invalidate();
          }
        }
    }
    return stream;
}

template <class MANIPULATOR>
int Sequence4::manipulateAttributes(MANIPULATOR& manipulator)
{
    int ret;

    ret = manipulator(&d_element1, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT1]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_element2, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT2]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_element3, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT3]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_element4, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT4]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_element5, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT5]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_element6, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT6]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_element7, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT7]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_element8, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT8]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_element9, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT9]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_element10, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT10]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_element11, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT11]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_element12, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT12]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_element13, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT13]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_element14, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT14]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_element15, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT15]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_element16, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT16]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_element17, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT17]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_element18, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT18]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_element19, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT19]);
    if (ret) {
        return ret;
    }

    return ret;
}

template <class MANIPULATOR>
int Sequence4::manipulateAttribute(MANIPULATOR& manipulator, int id)
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_ELEMENT1: {
        return manipulator(&d_element1, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT1]);
      } break;
      case ATTRIBUTE_ID_ELEMENT2: {
        return manipulator(&d_element2, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT2]);
      } break;
      case ATTRIBUTE_ID_ELEMENT3: {
        return manipulator(&d_element3, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT3]);
      } break;
      case ATTRIBUTE_ID_ELEMENT4: {
        return manipulator(&d_element4, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT4]);
      } break;
      case ATTRIBUTE_ID_ELEMENT5: {
        return manipulator(&d_element5, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT5]);
      } break;
      case ATTRIBUTE_ID_ELEMENT6: {
        return manipulator(&d_element6, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT6]);
      } break;
      case ATTRIBUTE_ID_ELEMENT7: {
        return manipulator(&d_element7, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT7]);
      } break;
      case ATTRIBUTE_ID_ELEMENT8: {
        return manipulator(&d_element8, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT8]);
      } break;
      case ATTRIBUTE_ID_ELEMENT9: {
        return manipulator(&d_element9, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT9]);
      } break;
      case ATTRIBUTE_ID_ELEMENT10: {
        return manipulator(&d_element10, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT10]);
      } break;
      case ATTRIBUTE_ID_ELEMENT11: {
        return manipulator(&d_element11, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT11]);
      } break;
      case ATTRIBUTE_ID_ELEMENT12: {
        return manipulator(&d_element12, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT12]);
      } break;
      case ATTRIBUTE_ID_ELEMENT13: {
        return manipulator(&d_element13, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT13]);
      } break;
      case ATTRIBUTE_ID_ELEMENT14: {
        return manipulator(&d_element14, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT14]);
      } break;
      case ATTRIBUTE_ID_ELEMENT15: {
        return manipulator(&d_element15, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT15]);
      } break;
      case ATTRIBUTE_ID_ELEMENT16: {
        return manipulator(&d_element16, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT16]);
      } break;
      case ATTRIBUTE_ID_ELEMENT17: {
        return manipulator(&d_element17, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT17]);
      } break;
      case ATTRIBUTE_ID_ELEMENT18: {
        return manipulator(&d_element18, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT18]);
      } break;
      case ATTRIBUTE_ID_ELEMENT19: {
        return manipulator(&d_element19, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT19]);
      } break;
      default:
        return NOT_FOUND;
    }
}

template <class MANIPULATOR>
int Sequence4::manipulateAttribute(
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
std::vector<Sequence3>& Sequence4::element1()
{
    return d_element1;
}

inline
std::vector<Choice1>& Sequence4::element2()
{
    return d_element2;
}

inline
bdeut_NullableValue<std::vector<char> >& Sequence4::element3()
{
    return d_element3;
}

inline
bdeut_NullableValue<int>& Sequence4::element4()
{
    return d_element4;
}

inline
bdeut_NullableValue<bdet_DatetimeTz>& Sequence4::element5()
{
    return d_element5;
}

inline
bdeut_NullableValue<CustomString>& Sequence4::element6()
{
    return d_element6;
}

inline
bdeut_NullableValue<Enumerated::Value>& Sequence4::element7()
{
    return d_element7;
}

inline
bool& Sequence4::element8()
{
    return d_element8;
}

inline
std::string& Sequence4::element9()
{
    return d_element9;
}

inline
double& Sequence4::element10()
{
    return d_element10;
}

inline
std::vector<char>& Sequence4::element11()
{
    return d_element11;
}

inline
int& Sequence4::element12()
{
    return d_element12;
}

inline
Enumerated::Value& Sequence4::element13()
{
    return d_element13;
}

inline
std::vector<bool>& Sequence4::element14()
{
    return d_element14;
}

inline
std::vector<double>& Sequence4::element15()
{
    return d_element15;
}

inline
std::vector<std::vector<char> >& Sequence4::element16()
{
    return d_element16;
}

inline
std::vector<int>& Sequence4::element17()
{
    return d_element17;
}

inline
std::vector<bdet_DatetimeTz>& Sequence4::element18()
{
    return d_element18;
}

inline
std::vector<CustomString>& Sequence4::element19()
{
    return d_element19;
}

// ACCESSORS
template <class STREAM>
STREAM& Sequence4::bdexStreamOut(STREAM& stream, int version) const
{
    switch (version) {
      case 1: {
        bdex_OutStreamFunctions::streamOut(stream, d_element1, 1);
        bdex_OutStreamFunctions::streamOut(stream, d_element2, 1);
        bdex_OutStreamFunctions::streamOut(stream, d_element3, 1);
        bdex_OutStreamFunctions::streamOut(stream, d_element4, 1);
        bdex_OutStreamFunctions::streamOut(stream, d_element5, 1);
        bdex_OutStreamFunctions::streamOut(stream, d_element6, 1);
        bdex_OutStreamFunctions::streamOut(stream, d_element7, 1);
        bdex_OutStreamFunctions::streamOut(stream, d_element8, 1);
        bdex_OutStreamFunctions::streamOut(stream, d_element9, 1);
        bdex_OutStreamFunctions::streamOut(stream, d_element10, 1);
        bdex_OutStreamFunctions::streamOut(stream, d_element11, 1);
        bdex_OutStreamFunctions::streamOut(stream, d_element12, 1);
        bdex_OutStreamFunctions::streamOut(stream, d_element13, 1);
        bdex_OutStreamFunctions::streamOut(stream, d_element14, 1);
        bdex_OutStreamFunctions::streamOut(stream, d_element15, 1);
        bdex_OutStreamFunctions::streamOut(stream, d_element16, 1);
        bdex_OutStreamFunctions::streamOut(stream, d_element17, 1);
        bdex_OutStreamFunctions::streamOut(stream, d_element18, 1);
        bdex_OutStreamFunctions::streamOut(stream, d_element19, 1);
      } break;
    }
    return stream;
}

template <class ACCESSOR>
int Sequence4::accessAttributes(ACCESSOR& accessor) const
{
    int ret;

    ret = accessor(d_element1, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT1]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_element2, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT2]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_element3, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT3]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_element4, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT4]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_element5, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT5]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_element6, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT6]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_element7, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT7]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_element8, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT8]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_element9, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT9]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_element10, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT10]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_element11, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT11]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_element12, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT12]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_element13, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT13]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_element14, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT14]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_element15, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT15]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_element16, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT16]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_element17, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT17]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_element18, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT18]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_element19, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT19]);
    if (ret) {
        return ret;
    }

    return ret;
}

template <class ACCESSOR>
int Sequence4::accessAttribute(ACCESSOR& accessor, int id) const
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_ELEMENT1: {
        return accessor(d_element1, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT1]);
      } break;
      case ATTRIBUTE_ID_ELEMENT2: {
        return accessor(d_element2, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT2]);
      } break;
      case ATTRIBUTE_ID_ELEMENT3: {
        return accessor(d_element3, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT3]);
      } break;
      case ATTRIBUTE_ID_ELEMENT4: {
        return accessor(d_element4, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT4]);
      } break;
      case ATTRIBUTE_ID_ELEMENT5: {
        return accessor(d_element5, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT5]);
      } break;
      case ATTRIBUTE_ID_ELEMENT6: {
        return accessor(d_element6, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT6]);
      } break;
      case ATTRIBUTE_ID_ELEMENT7: {
        return accessor(d_element7, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT7]);
      } break;
      case ATTRIBUTE_ID_ELEMENT8: {
        return accessor(d_element8, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT8]);
      } break;
      case ATTRIBUTE_ID_ELEMENT9: {
        return accessor(d_element9, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT9]);
      } break;
      case ATTRIBUTE_ID_ELEMENT10: {
        return accessor(d_element10, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT10]);
      } break;
      case ATTRIBUTE_ID_ELEMENT11: {
        return accessor(d_element11, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT11]);
      } break;
      case ATTRIBUTE_ID_ELEMENT12: {
        return accessor(d_element12, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT12]);
      } break;
      case ATTRIBUTE_ID_ELEMENT13: {
        return accessor(d_element13, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT13]);
      } break;
      case ATTRIBUTE_ID_ELEMENT14: {
        return accessor(d_element14, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT14]);
      } break;
      case ATTRIBUTE_ID_ELEMENT15: {
        return accessor(d_element15, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT15]);
      } break;
      case ATTRIBUTE_ID_ELEMENT16: {
        return accessor(d_element16, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT16]);
      } break;
      case ATTRIBUTE_ID_ELEMENT17: {
        return accessor(d_element17, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT17]);
      } break;
      case ATTRIBUTE_ID_ELEMENT18: {
        return accessor(d_element18, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT18]);
      } break;
      case ATTRIBUTE_ID_ELEMENT19: {
        return accessor(d_element19, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT19]);
      } break;
      default:
        return NOT_FOUND;
    }
}

template <class ACCESSOR>
int Sequence4::accessAttribute(
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
const std::vector<Sequence3>& Sequence4::element1() const
{
    return d_element1;
}

inline
const std::vector<Choice1>& Sequence4::element2() const
{
    return d_element2;
}

inline
const bdeut_NullableValue<std::vector<char> >& Sequence4::element3() const
{
    return d_element3;
}

inline
const bdeut_NullableValue<int>& Sequence4::element4() const
{
    return d_element4;
}

inline
const bdeut_NullableValue<bdet_DatetimeTz>& Sequence4::element5() const
{
    return d_element5;
}

inline
const bdeut_NullableValue<CustomString>& Sequence4::element6() const
{
    return d_element6;
}

inline
const bdeut_NullableValue<Enumerated::Value>& Sequence4::element7() const
{
    return d_element7;
}

inline
bool Sequence4::element8() const
{
    return d_element8;
}

inline
const std::string& Sequence4::element9() const
{
    return d_element9;
}

inline
double Sequence4::element10() const
{
    return d_element10;
}

inline
const std::vector<char>& Sequence4::element11() const
{
    return d_element11;
}

inline
int Sequence4::element12() const
{
    return d_element12;
}

inline
Enumerated::Value Sequence4::element13() const
{
    return d_element13;
}

inline
const std::vector<bool>& Sequence4::element14() const
{
    return d_element14;
}

inline
const std::vector<double>& Sequence4::element15() const
{
    return d_element15;
}

inline
const std::vector<std::vector<char> >& Sequence4::element16() const
{
    return d_element16;
}

inline
const std::vector<int>& Sequence4::element17() const
{
    return d_element17;
}

inline
const std::vector<bdet_DatetimeTz>& Sequence4::element18() const
{
    return d_element18;
}

inline
const std::vector<CustomString>& Sequence4::element19() const
{
    return d_element19;
}



                              // ---------------                               
                              // class Sequence1                               
                              // ---------------                               

// CLASS METHODS
inline
int Sequence1::maxSupportedBdexVersion()
{
    return 1;  // versions start at 1.
}

// MANIPULATORS
template <class STREAM>
STREAM& Sequence1::bdexStreamIn(STREAM& stream, int version)
{
    if (stream) {
        switch (version) {
          case 1: {
            bdex_InStreamFunctions::streamIn(stream, d_element1, 1);
            bdex_InStreamFunctions::streamIn(stream, d_element2, 1);
            bdex_InStreamFunctions::streamIn(stream, *d_element3, 1);
            bdex_InStreamFunctions::streamIn(stream, d_element4, 1);
          } break;
          default: {
            stream.invalidate();
          }
        }
    }
    return stream;
}

template <class MANIPULATOR>
int Sequence1::manipulateAttributes(MANIPULATOR& manipulator)
{
    int ret;

    ret = manipulator(&d_element1, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT1]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_element2, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT2]);
    if (ret) {
        return ret;
    }

    ret = manipulator(d_element3, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT3]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_element4, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT4]);
    if (ret) {
        return ret;
    }

    return ret;
}

template <class MANIPULATOR>
int Sequence1::manipulateAttribute(MANIPULATOR& manipulator, int id)
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_ELEMENT1: {
        return manipulator(&d_element1, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT1]);
      } break;
      case ATTRIBUTE_ID_ELEMENT2: {
        return manipulator(&d_element2, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT2]);
      } break;
      case ATTRIBUTE_ID_ELEMENT3: {
        return manipulator(d_element3, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT3]);
      } break;
      case ATTRIBUTE_ID_ELEMENT4: {
        return manipulator(&d_element4, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT4]);
      } break;
      default:
        return NOT_FOUND;
    }
}

template <class MANIPULATOR>
int Sequence1::manipulateAttribute(
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
bdeut_NullableValue<Choice3>& Sequence1::element1()
{
    return d_element1;
}

inline
std::vector<Choice1>& Sequence1::element2()
{
    return d_element2;
}

inline
Choice2& Sequence1::element3()
{
    return *d_element3;
}

inline
std::vector<Choice3>& Sequence1::element4()
{
    return d_element4;
}

// ACCESSORS
template <class STREAM>
STREAM& Sequence1::bdexStreamOut(STREAM& stream, int version) const
{
    switch (version) {
      case 1: {
        bdex_OutStreamFunctions::streamOut(stream, d_element1, 1);
        bdex_OutStreamFunctions::streamOut(stream, d_element2, 1);
        bdex_OutStreamFunctions::streamOut(stream, *d_element3, 1);
        bdex_OutStreamFunctions::streamOut(stream, d_element4, 1);
      } break;
    }
    return stream;
}

template <class ACCESSOR>
int Sequence1::accessAttributes(ACCESSOR& accessor) const
{
    int ret;

    ret = accessor(d_element1, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT1]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_element2, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT2]);
    if (ret) {
        return ret;
    }

    ret = accessor(*d_element3, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT3]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_element4, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT4]);
    if (ret) {
        return ret;
    }

    return ret;
}

template <class ACCESSOR>
int Sequence1::accessAttribute(ACCESSOR& accessor, int id) const
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_ELEMENT1: {
        return accessor(d_element1, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT1]);
      } break;
      case ATTRIBUTE_ID_ELEMENT2: {
        return accessor(d_element2, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT2]);
      } break;
      case ATTRIBUTE_ID_ELEMENT3: {
        return accessor(*d_element3, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT3]);
      } break;
      case ATTRIBUTE_ID_ELEMENT4: {
        return accessor(d_element4, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT4]);
      } break;
      default:
        return NOT_FOUND;
    }
}

template <class ACCESSOR>
int Sequence1::accessAttribute(
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
const bdeut_NullableValue<Choice3>& Sequence1::element1() const
{
    return d_element1;
}

inline
const std::vector<Choice1>& Sequence1::element2() const
{
    return d_element2;
}

inline
const Choice2& Sequence1::element3() const
{
    return *d_element3;
}

inline
const std::vector<Choice3>& Sequence1::element4() const
{
    return d_element4;
}



                              // ---------------                               
                              // class Sequence2                               
                              // ---------------                               

// CLASS METHODS
inline
int Sequence2::maxSupportedBdexVersion()
{
    return 1;  // versions start at 1.
}

// MANIPULATORS
template <class STREAM>
STREAM& Sequence2::bdexStreamIn(STREAM& stream, int version)
{
    if (stream) {
        switch (version) {
          case 1: {
            bdex_InStreamFunctions::streamIn(stream, d_element1, 1);
            bdex_InStreamFunctions::streamIn(stream, d_element2, 1);
            bdex_InStreamFunctions::streamIn(stream, d_element3, 1);
            bdex_InStreamFunctions::streamIn(stream, d_element4, 1);
            bdex_InStreamFunctions::streamIn(stream, d_element5, 1);
          } break;
          default: {
            stream.invalidate();
          }
        }
    }
    return stream;
}

template <class MANIPULATOR>
int Sequence2::manipulateAttributes(MANIPULATOR& manipulator)
{
    int ret;

    ret = manipulator(&d_element1, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT1]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_element2, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT2]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_element3, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT3]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_element4, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT4]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_element5, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT5]);
    if (ret) {
        return ret;
    }

    return ret;
}

template <class MANIPULATOR>
int Sequence2::manipulateAttribute(MANIPULATOR& manipulator, int id)
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_ELEMENT1: {
        return manipulator(&d_element1, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT1]);
      } break;
      case ATTRIBUTE_ID_ELEMENT2: {
        return manipulator(&d_element2, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT2]);
      } break;
      case ATTRIBUTE_ID_ELEMENT3: {
        return manipulator(&d_element3, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT3]);
      } break;
      case ATTRIBUTE_ID_ELEMENT4: {
        return manipulator(&d_element4, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT4]);
      } break;
      case ATTRIBUTE_ID_ELEMENT5: {
        return manipulator(&d_element5, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT5]);
      } break;
      default:
        return NOT_FOUND;
    }
}

template <class MANIPULATOR>
int Sequence2::manipulateAttribute(
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
CustomString& Sequence2::element1()
{
    return d_element1;
}

inline
unsigned char& Sequence2::element2()
{
    return d_element2;
}

inline
bdet_DatetimeTz& Sequence2::element3()
{
    return d_element3;
}

inline
bdeut_NullableValue<Choice1>& Sequence2::element4()
{
    return d_element4;
}

inline
bdeut_NullableValue<double>& Sequence2::element5()
{
    return d_element5;
}

// ACCESSORS
template <class STREAM>
STREAM& Sequence2::bdexStreamOut(STREAM& stream, int version) const
{
    switch (version) {
      case 1: {
        bdex_OutStreamFunctions::streamOut(stream, d_element1, 1);
        bdex_OutStreamFunctions::streamOut(stream, d_element2, 1);
        bdex_OutStreamFunctions::streamOut(stream, d_element3, 1);
        bdex_OutStreamFunctions::streamOut(stream, d_element4, 1);
        bdex_OutStreamFunctions::streamOut(stream, d_element5, 1);
      } break;
    }
    return stream;
}

template <class ACCESSOR>
int Sequence2::accessAttributes(ACCESSOR& accessor) const
{
    int ret;

    ret = accessor(d_element1, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT1]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_element2, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT2]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_element3, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT3]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_element4, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT4]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_element5, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT5]);
    if (ret) {
        return ret;
    }

    return ret;
}

template <class ACCESSOR>
int Sequence2::accessAttribute(ACCESSOR& accessor, int id) const
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_ELEMENT1: {
        return accessor(d_element1, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT1]);
      } break;
      case ATTRIBUTE_ID_ELEMENT2: {
        return accessor(d_element2, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT2]);
      } break;
      case ATTRIBUTE_ID_ELEMENT3: {
        return accessor(d_element3, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT3]);
      } break;
      case ATTRIBUTE_ID_ELEMENT4: {
        return accessor(d_element4, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT4]);
      } break;
      case ATTRIBUTE_ID_ELEMENT5: {
        return accessor(d_element5, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT5]);
      } break;
      default:
        return NOT_FOUND;
    }
}

template <class ACCESSOR>
int Sequence2::accessAttribute(
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
const CustomString& Sequence2::element1() const
{
    return d_element1;
}

inline
unsigned char Sequence2::element2() const
{
    return d_element2;
}

inline
const bdet_DatetimeTz& Sequence2::element3() const
{
    return d_element3;
}

inline
const bdeut_NullableValue<Choice1>& Sequence2::element4() const
{
    return d_element4;
}

inline
const bdeut_NullableValue<double>& Sequence2::element5() const
{
    return d_element5;
}



                          // ------------------------                          
                          // class FeatureTestMessage                          
                          // ------------------------                          

// CLASS METHODS
inline
int FeatureTestMessage::maxSupportedBdexVersion()
{
    return 1;  // versions start at 1.
}

// CREATORS
inline
FeatureTestMessage::FeatureTestMessage(bdema_Allocator *basicAllocator)
: d_selectionId(SELECTION_ID_UNDEFINED)
, d_allocator_p(bdema_Default::allocator(basicAllocator))
{
}

inline
FeatureTestMessage::~FeatureTestMessage()
{
    reset();
}

// MANIPULATORS
template <class STREAM>
STREAM& FeatureTestMessage::bdexStreamIn(STREAM& stream, int version)
{
    if (stream) {
        switch (version) {
          case 1: {
            short selectionId;
            stream.getInt16(selectionId);
            if (!stream) {
                return stream;
            }
            switch (selectionId) {
              case SELECTION_ID_SELECTION1: {
                makeSelection1();
                bdex_InStreamFunctions::streamIn(
                    stream, d_selection1.object(), 1);
              } break;
              case SELECTION_ID_SELECTION2: {
                makeSelection2();
                bdex_InStreamFunctions::streamIn(
                    stream, d_selection2.object(), 1);
              } break;
              case SELECTION_ID_SELECTION3: {
                makeSelection3();
                bdex_InStreamFunctions::streamIn(
                    stream, d_selection3.object(), 1);
              } break;
              case SELECTION_ID_SELECTION4: {
                makeSelection4();
                bdex_InStreamFunctions::streamIn(
                    stream, *d_selection4, 1);
              } break;
              case SELECTION_ID_SELECTION5: {
                makeSelection5();
                bdex_InStreamFunctions::streamIn(
                    stream, d_selection5.object(), 1);
              } break;
              case SELECTION_ID_SELECTION6: {
                makeSelection6();
                bdex_InStreamFunctions::streamIn(
                    stream, d_selection6.object(), 1);
              } break;
              case SELECTION_ID_SELECTION7: {
                makeSelection7();
                Enumerated::bdexStreamIn(
                    stream, d_selection7.object(), 1);
              } break;
              case SELECTION_ID_SELECTION8: {
                makeSelection8();
                bdex_InStreamFunctions::streamIn(
                    stream, d_selection8.object(), 1);
              } break;
              case SELECTION_ID_SELECTION9: {
                makeSelection9();
                bdex_InStreamFunctions::streamIn(
                    stream, d_selection9.object(), 1);
              } break;
              case SELECTION_ID_SELECTION10: {
                makeSelection10();
                bdex_InStreamFunctions::streamIn(
                    stream, d_selection10.object(), 1);
              } break;
              case SELECTION_ID_UNDEFINED: {
                reset();
              } break;
              default:
                stream.invalidate();
            }
          } break;
          default: {
            stream.invalidate();
          }
        }
    }
    return stream;
}

template <class MANIPULATOR>
int FeatureTestMessage::manipulateSelection(MANIPULATOR& manipulator)
{
    switch (d_selectionId) {
      case FeatureTestMessage::SELECTION_ID_SELECTION1:
        return manipulator(&d_selection1.object(),
                SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION1]);
      case FeatureTestMessage::SELECTION_ID_SELECTION2:
        return manipulator(&d_selection2.object(),
                SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION2]);
      case FeatureTestMessage::SELECTION_ID_SELECTION3:
        return manipulator(&d_selection3.object(),
                SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION3]);
      case FeatureTestMessage::SELECTION_ID_SELECTION4:
        return manipulator(d_selection4,
                SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION4]);
      case FeatureTestMessage::SELECTION_ID_SELECTION5:
        return manipulator(&d_selection5.object(),
                SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION5]);
      case FeatureTestMessage::SELECTION_ID_SELECTION6:
        return manipulator(&d_selection6.object(),
                SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION6]);
      case FeatureTestMessage::SELECTION_ID_SELECTION7:
        return manipulator(&d_selection7.object(),
                SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION7]);
      case FeatureTestMessage::SELECTION_ID_SELECTION8:
        return manipulator(&d_selection8.object(),
                SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION8]);
      case FeatureTestMessage::SELECTION_ID_SELECTION9:
        return manipulator(&d_selection9.object(),
                SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION9]);
      case FeatureTestMessage::SELECTION_ID_SELECTION10:
        return manipulator(&d_selection10.object(),
                SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION10]);
      default:
        BSLS_ASSERT(FeatureTestMessage::SELECTION_ID_UNDEFINED == d_selectionId);
        return -1;
    }
}

inline
Sequence1& FeatureTestMessage::selection1()
{
    BSLS_ASSERT(SELECTION_ID_SELECTION1 == d_selectionId);
    return d_selection1.object();
}

inline
std::vector<char>& FeatureTestMessage::selection2()
{
    BSLS_ASSERT(SELECTION_ID_SELECTION2 == d_selectionId);
    return d_selection2.object();
}

inline
Sequence2& FeatureTestMessage::selection3()
{
    BSLS_ASSERT(SELECTION_ID_SELECTION3 == d_selectionId);
    return d_selection3.object();
}

inline
Sequence3& FeatureTestMessage::selection4()
{
    BSLS_ASSERT(SELECTION_ID_SELECTION4 == d_selectionId);
    return *d_selection4;
}

inline
bdet_DatetimeTz& FeatureTestMessage::selection5()
{
    BSLS_ASSERT(SELECTION_ID_SELECTION5 == d_selectionId);
    return d_selection5.object();
}

inline
CustomString& FeatureTestMessage::selection6()
{
    BSLS_ASSERT(SELECTION_ID_SELECTION6 == d_selectionId);
    return d_selection6.object();
}

inline
Enumerated::Value& FeatureTestMessage::selection7()
{
    BSLS_ASSERT(SELECTION_ID_SELECTION7 == d_selectionId);
    return d_selection7.object();
}

inline
Choice3& FeatureTestMessage::selection8()
{
    BSLS_ASSERT(SELECTION_ID_SELECTION8 == d_selectionId);
    return d_selection8.object();
}

inline
VoidSequence& FeatureTestMessage::selection9()
{
    BSLS_ASSERT(SELECTION_ID_SELECTION9 == d_selectionId);
    return d_selection9.object();
}

inline
UnsignedSequence& FeatureTestMessage::selection10()
{
    BSLS_ASSERT(SELECTION_ID_SELECTION10 == d_selectionId);
    return d_selection10.object();
}

// ACCESSORS
template <class STREAM>
STREAM& FeatureTestMessage::bdexStreamOut(STREAM& stream, int version) const
{
    switch (version) {
      case 1: {
            stream.putInt16(d_selectionId);
            switch (d_selectionId) {
              case SELECTION_ID_SELECTION1: {
                bdex_OutStreamFunctions::streamOut(
                    stream, d_selection1.object(), 1);
              } break;
              case SELECTION_ID_SELECTION2: {
                bdex_OutStreamFunctions::streamOut(
                    stream, d_selection2.object(), 1);
              } break;
              case SELECTION_ID_SELECTION3: {
                bdex_OutStreamFunctions::streamOut(
                    stream, d_selection3.object(), 1);
              } break;
              case SELECTION_ID_SELECTION4: {
                bdex_OutStreamFunctions::streamOut(
                    stream, *d_selection4, 1);
              } break;
              case SELECTION_ID_SELECTION5: {
                bdex_OutStreamFunctions::streamOut(
                    stream, d_selection5.object(), 1);
              } break;
              case SELECTION_ID_SELECTION6: {
                bdex_OutStreamFunctions::streamOut(
                    stream, d_selection6.object(), 1);
              } break;
              case SELECTION_ID_SELECTION7: {
                bdex_OutStreamFunctions::streamOut(
                    stream, d_selection7.object(), 1);
              } break;
              case SELECTION_ID_SELECTION8: {
                bdex_OutStreamFunctions::streamOut(
                    stream, d_selection8.object(), 1);
              } break;
              case SELECTION_ID_SELECTION9: {
                bdex_OutStreamFunctions::streamOut(
                    stream, d_selection9.object(), 1);
              } break;
              case SELECTION_ID_SELECTION10: {
                bdex_OutStreamFunctions::streamOut(
                    stream, d_selection10.object(), 1);
              } break;
              default:
                BSLS_ASSERT(SELECTION_ID_UNDEFINED == d_selectionId);
            }
      } break;
    }
    return stream;
}

inline
int FeatureTestMessage::selectionId() const
{
    return d_selectionId;
}

template <class ACCESSOR>
int FeatureTestMessage::accessSelection(ACCESSOR& accessor) const
{
    switch (d_selectionId) {
      case SELECTION_ID_SELECTION1:
        return accessor(d_selection1.object(),
                SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION1]);
      case SELECTION_ID_SELECTION2:
        return accessor(d_selection2.object(),
                SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION2]);
      case SELECTION_ID_SELECTION3:
        return accessor(d_selection3.object(),
                SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION3]);
      case SELECTION_ID_SELECTION4:
        return accessor(*d_selection4,
                SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION4]);
      case SELECTION_ID_SELECTION5:
        return accessor(d_selection5.object(),
                SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION5]);
      case SELECTION_ID_SELECTION6:
        return accessor(d_selection6.object(),
                SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION6]);
      case SELECTION_ID_SELECTION7:
        return accessor(d_selection7.object(),
                SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION7]);
      case SELECTION_ID_SELECTION8:
        return accessor(d_selection8.object(),
                SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION8]);
      case SELECTION_ID_SELECTION9:
        return accessor(d_selection9.object(),
                SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION9]);
      case SELECTION_ID_SELECTION10:
        return accessor(d_selection10.object(),
                SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION10]);
      default:
        BSLS_ASSERT(SELECTION_ID_UNDEFINED == d_selectionId);
        return -1;
    }
}

inline
const Sequence1& FeatureTestMessage::selection1() const
{
    BSLS_ASSERT(SELECTION_ID_SELECTION1 == d_selectionId);
    return d_selection1.object();
}

inline
const std::vector<char>& FeatureTestMessage::selection2() const
{
    BSLS_ASSERT(SELECTION_ID_SELECTION2 == d_selectionId);
    return d_selection2.object();
}

inline
const Sequence2& FeatureTestMessage::selection3() const
{
    BSLS_ASSERT(SELECTION_ID_SELECTION3 == d_selectionId);
    return d_selection3.object();
}

inline
const Sequence3& FeatureTestMessage::selection4() const
{
    BSLS_ASSERT(SELECTION_ID_SELECTION4 == d_selectionId);
    return *d_selection4;
}

inline
const bdet_DatetimeTz& FeatureTestMessage::selection5() const
{
    BSLS_ASSERT(SELECTION_ID_SELECTION5 == d_selectionId);
    return d_selection5.object();
}

inline
const CustomString& FeatureTestMessage::selection6() const
{
    BSLS_ASSERT(SELECTION_ID_SELECTION6 == d_selectionId);
    return d_selection6.object();
}

inline
const Enumerated::Value& FeatureTestMessage::selection7() const
{
    BSLS_ASSERT(SELECTION_ID_SELECTION7 == d_selectionId);
    return d_selection7.object();
}

inline
const Choice3& FeatureTestMessage::selection8() const
{
    BSLS_ASSERT(SELECTION_ID_SELECTION8 == d_selectionId);
    return d_selection8.object();
}

inline
const VoidSequence& FeatureTestMessage::selection9() const
{
    BSLS_ASSERT(SELECTION_ID_SELECTION9 == d_selectionId);
    return d_selection9.object();
}

inline
const UnsignedSequence& FeatureTestMessage::selection10() const
{
    BSLS_ASSERT(SELECTION_ID_SELECTION10 == d_selectionId);
    return d_selection10.object();
}

inline
bool FeatureTestMessage::isSelection1Value() const
{
    return SELECTION_ID_SELECTION1 == d_selectionId;
}

inline
bool FeatureTestMessage::isSelection2Value() const
{
    return SELECTION_ID_SELECTION2 == d_selectionId;
}

inline
bool FeatureTestMessage::isSelection3Value() const
{
    return SELECTION_ID_SELECTION3 == d_selectionId;
}

inline
bool FeatureTestMessage::isSelection4Value() const
{
    return SELECTION_ID_SELECTION4 == d_selectionId;
}

inline
bool FeatureTestMessage::isSelection5Value() const
{
    return SELECTION_ID_SELECTION5 == d_selectionId;
}

inline
bool FeatureTestMessage::isSelection6Value() const
{
    return SELECTION_ID_SELECTION6 == d_selectionId;
}

inline
bool FeatureTestMessage::isSelection7Value() const
{
    return SELECTION_ID_SELECTION7 == d_selectionId;
}

inline
bool FeatureTestMessage::isSelection8Value() const
{
    return SELECTION_ID_SELECTION8 == d_selectionId;
}

inline
bool FeatureTestMessage::isSelection9Value() const
{
    return SELECTION_ID_SELECTION9 == d_selectionId;
}

inline
bool FeatureTestMessage::isSelection10Value() const
{
    return SELECTION_ID_SELECTION10 == d_selectionId;
}

inline
bool FeatureTestMessage::isUndefinedValue() const
{
    return SELECTION_ID_UNDEFINED == d_selectionId;
}


                               // -------------                                
                               // class Request                                
                               // -------------                                

// CLASS METHODS
inline
int Request::maxSupportedBdexVersion()
{
    return 1;  // versions start at 1.
}

// CREATORS
inline
Request::Request(bdema_Allocator *basicAllocator)
: d_selectionId(SELECTION_ID_UNDEFINED)
, d_allocator_p(bdema_Default::allocator(basicAllocator))
{
}

inline
Request::~Request()
{
    reset();
}

// MANIPULATORS
template <class STREAM>
STREAM& Request::bdexStreamIn(STREAM& stream, int version)
{
    if (stream) {
        switch (version) {
          case 1: {
            short selectionId;
            stream.getInt16(selectionId);
            if (!stream) {
                return stream;
            }
            switch (selectionId) {
              case SELECTION_ID_SIMPLE_REQUEST: {
                makeSimpleRequest();
                bdex_InStreamFunctions::streamIn(
                    stream, d_simpleRequest.object(), 1);
              } break;
              case SELECTION_ID_FEATURE_REQUEST: {
                makeFeatureRequest();
                bdex_InStreamFunctions::streamIn(
                    stream, d_featureRequest.object(), 1);
              } break;
              case SELECTION_ID_UNDEFINED: {
                reset();
              } break;
              default:
                stream.invalidate();
            }
          } break;
          default: {
            stream.invalidate();
          }
        }
    }
    return stream;
}

template <class MANIPULATOR>
int Request::manipulateSelection(MANIPULATOR& manipulator)
{
    switch (d_selectionId) {
      case Request::SELECTION_ID_SIMPLE_REQUEST:
        return manipulator(&d_simpleRequest.object(),
                SELECTION_INFO_ARRAY[SELECTION_INDEX_SIMPLE_REQUEST]);
      case Request::SELECTION_ID_FEATURE_REQUEST:
        return manipulator(&d_featureRequest.object(),
                SELECTION_INFO_ARRAY[SELECTION_INDEX_FEATURE_REQUEST]);
      default:
        BSLS_ASSERT(Request::SELECTION_ID_UNDEFINED == d_selectionId);
        return -1;
    }
}

inline
SimpleRequest& Request::simpleRequest()
{
    BSLS_ASSERT(SELECTION_ID_SIMPLE_REQUEST == d_selectionId);
    return d_simpleRequest.object();
}

inline
FeatureTestMessage& Request::featureRequest()
{
    BSLS_ASSERT(SELECTION_ID_FEATURE_REQUEST == d_selectionId);
    return d_featureRequest.object();
}

// ACCESSORS
template <class STREAM>
STREAM& Request::bdexStreamOut(STREAM& stream, int version) const
{
    switch (version) {
      case 1: {
            stream.putInt16(d_selectionId);
            switch (d_selectionId) {
              case SELECTION_ID_SIMPLE_REQUEST: {
                bdex_OutStreamFunctions::streamOut(
                    stream, d_simpleRequest.object(), 1);
              } break;
              case SELECTION_ID_FEATURE_REQUEST: {
                bdex_OutStreamFunctions::streamOut(
                    stream, d_featureRequest.object(), 1);
              } break;
              default:
                BSLS_ASSERT(SELECTION_ID_UNDEFINED == d_selectionId);
            }
      } break;
    }
    return stream;
}

inline
int Request::selectionId() const
{
    return d_selectionId;
}

template <class ACCESSOR>
int Request::accessSelection(ACCESSOR& accessor) const
{
    switch (d_selectionId) {
      case SELECTION_ID_SIMPLE_REQUEST:
        return accessor(d_simpleRequest.object(),
                SELECTION_INFO_ARRAY[SELECTION_INDEX_SIMPLE_REQUEST]);
      case SELECTION_ID_FEATURE_REQUEST:
        return accessor(d_featureRequest.object(),
                SELECTION_INFO_ARRAY[SELECTION_INDEX_FEATURE_REQUEST]);
      default:
        BSLS_ASSERT(SELECTION_ID_UNDEFINED == d_selectionId);
        return -1;
    }
}

inline
const SimpleRequest& Request::simpleRequest() const
{
    BSLS_ASSERT(SELECTION_ID_SIMPLE_REQUEST == d_selectionId);
    return d_simpleRequest.object();
}

inline
const FeatureTestMessage& Request::featureRequest() const
{
    BSLS_ASSERT(SELECTION_ID_FEATURE_REQUEST == d_selectionId);
    return d_featureRequest.object();
}

inline
bool Request::isSimpleRequestValue() const
{
    return SELECTION_ID_SIMPLE_REQUEST == d_selectionId;
}

inline
bool Request::isFeatureRequestValue() const
{
    return SELECTION_ID_FEATURE_REQUEST == d_selectionId;
}

inline
bool Request::isUndefinedValue() const
{
    return SELECTION_ID_UNDEFINED == d_selectionId;
}


                               // --------------                               
                               // class Response                               
                               // --------------                               

// CLASS METHODS
inline
int Response::maxSupportedBdexVersion()
{
    return 1;  // versions start at 1.
}

// CREATORS
inline
Response::Response(bdema_Allocator *basicAllocator)
: d_selectionId(SELECTION_ID_UNDEFINED)
, d_allocator_p(bdema_Default::allocator(basicAllocator))
{
}

inline
Response::~Response()
{
    reset();
}

// MANIPULATORS
template <class STREAM>
STREAM& Response::bdexStreamIn(STREAM& stream, int version)
{
    if (stream) {
        switch (version) {
          case 1: {
            short selectionId;
            stream.getInt16(selectionId);
            if (!stream) {
                return stream;
            }
            switch (selectionId) {
              case SELECTION_ID_RESPONSE_DATA: {
                makeResponseData();
                bdex_InStreamFunctions::streamIn(
                    stream, d_responseData.object(), 1);
              } break;
              case SELECTION_ID_FEATURE_RESPONSE: {
                makeFeatureResponse();
                bdex_InStreamFunctions::streamIn(
                    stream, d_featureResponse.object(), 1);
              } break;
              case SELECTION_ID_UNDEFINED: {
                reset();
              } break;
              default:
                stream.invalidate();
            }
          } break;
          default: {
            stream.invalidate();
          }
        }
    }
    return stream;
}

template <class MANIPULATOR>
int Response::manipulateSelection(MANIPULATOR& manipulator)
{
    switch (d_selectionId) {
      case Response::SELECTION_ID_RESPONSE_DATA:
        return manipulator(&d_responseData.object(),
                SELECTION_INFO_ARRAY[SELECTION_INDEX_RESPONSE_DATA]);
      case Response::SELECTION_ID_FEATURE_RESPONSE:
        return manipulator(&d_featureResponse.object(),
                SELECTION_INFO_ARRAY[SELECTION_INDEX_FEATURE_RESPONSE]);
      default:
        BSLS_ASSERT(Response::SELECTION_ID_UNDEFINED == d_selectionId);
        return -1;
    }
}

inline
std::string& Response::responseData()
{
    BSLS_ASSERT(SELECTION_ID_RESPONSE_DATA == d_selectionId);
    return d_responseData.object();
}

inline
FeatureTestMessage& Response::featureResponse()
{
    BSLS_ASSERT(SELECTION_ID_FEATURE_RESPONSE == d_selectionId);
    return d_featureResponse.object();
}

// ACCESSORS
template <class STREAM>
STREAM& Response::bdexStreamOut(STREAM& stream, int version) const
{
    switch (version) {
      case 1: {
            stream.putInt16(d_selectionId);
            switch (d_selectionId) {
              case SELECTION_ID_RESPONSE_DATA: {
                bdex_OutStreamFunctions::streamOut(
                    stream, d_responseData.object(), 1);
              } break;
              case SELECTION_ID_FEATURE_RESPONSE: {
                bdex_OutStreamFunctions::streamOut(
                    stream, d_featureResponse.object(), 1);
              } break;
              default:
                BSLS_ASSERT(SELECTION_ID_UNDEFINED == d_selectionId);
            }
      } break;
    }
    return stream;
}

inline
int Response::selectionId() const
{
    return d_selectionId;
}

template <class ACCESSOR>
int Response::accessSelection(ACCESSOR& accessor) const
{
    switch (d_selectionId) {
      case SELECTION_ID_RESPONSE_DATA:
        return accessor(d_responseData.object(),
                SELECTION_INFO_ARRAY[SELECTION_INDEX_RESPONSE_DATA]);
      case SELECTION_ID_FEATURE_RESPONSE:
        return accessor(d_featureResponse.object(),
                SELECTION_INFO_ARRAY[SELECTION_INDEX_FEATURE_RESPONSE]);
      default:
        BSLS_ASSERT(SELECTION_ID_UNDEFINED == d_selectionId);
        return -1;
    }
}

inline
const std::string& Response::responseData() const
{
    BSLS_ASSERT(SELECTION_ID_RESPONSE_DATA == d_selectionId);
    return d_responseData.object();
}

inline
const FeatureTestMessage& Response::featureResponse() const
{
    BSLS_ASSERT(SELECTION_ID_FEATURE_RESPONSE == d_selectionId);
    return d_featureResponse.object();
}

inline
bool Response::isResponseDataValue() const
{
    return SELECTION_ID_RESPONSE_DATA == d_selectionId;
}

inline
bool Response::isFeatureResponseValue() const
{
    return SELECTION_ID_FEATURE_RESPONSE == d_selectionId;
}

inline
bool Response::isUndefinedValue() const
{
    return SELECTION_ID_UNDEFINED == d_selectionId;
}
}  // close namespace baea

// FREE FUNCTIONS

inline
bool baea::operator==(
        const baea::CustomInt& lhs,
        const baea::CustomInt& rhs)
{
    return lhs.d_value == rhs.d_value;
}

inline
bool baea::operator!=(
        const baea::CustomInt& lhs,
        const baea::CustomInt& rhs)
{
    return lhs.d_value != rhs.d_value;
}

inline
std::ostream& baea::operator<<(
        std::ostream& stream,
        const baea::CustomInt& rhs)
{
    return rhs.print(stream, 0, -1);
}


inline
bool baea::operator==(
        const baea::CustomString& lhs,
        const baea::CustomString& rhs)
{
    return lhs.d_value == rhs.d_value;
}

inline
bool baea::operator!=(
        const baea::CustomString& lhs,
        const baea::CustomString& rhs)
{
    return lhs.d_value != rhs.d_value;
}

inline
std::ostream& baea::operator<<(
        std::ostream& stream,
        const baea::CustomString& rhs)
{
    return rhs.print(stream, 0, -1);
}


inline
std::ostream& baea::operator<<(
        std::ostream& stream,
        baea::Enumerated::Value rhs)
{
    return baea::Enumerated::print(stream, rhs);
}


inline
bool baea::operator==(
        const baea::SimpleRequest& lhs,
        const baea::SimpleRequest& rhs)
{
    return  lhs.data() == rhs.data()
         && lhs.responseLength() == rhs.responseLength();
}

inline
bool baea::operator!=(
        const baea::SimpleRequest& lhs,
        const baea::SimpleRequest& rhs)
{
    return  lhs.data() != rhs.data()
         || lhs.responseLength() != rhs.responseLength();
}

inline
std::ostream& baea::operator<<(
        std::ostream& stream,
        const baea::SimpleRequest& rhs)
{
    return rhs.print(stream, 0, -1);
}


inline
bool baea::operator==(
        const baea::UnsignedSequence& lhs,
        const baea::UnsignedSequence& rhs)
{
    return  lhs.element1() == rhs.element1()
         && lhs.element2() == rhs.element2()
         && lhs.element3() == rhs.element3();
}

inline
bool baea::operator!=(
        const baea::UnsignedSequence& lhs,
        const baea::UnsignedSequence& rhs)
{
    return  lhs.element1() != rhs.element1()
         || lhs.element2() != rhs.element2()
         || lhs.element3() != rhs.element3();
}

inline
std::ostream& baea::operator<<(
        std::ostream& stream,
        const baea::UnsignedSequence& rhs)
{
    return rhs.print(stream, 0, -1);
}


inline
bool baea::operator==(
        const baea::VoidSequence&,
        const baea::VoidSequence&)
{
    return true;
}

inline
bool baea::operator!=(
        const baea::VoidSequence&,
        const baea::VoidSequence&)
{
    return false;
}

inline
std::ostream& baea::operator<<(
        std::ostream& stream,
        const baea::VoidSequence& rhs)
{
    return rhs.print(stream, 0, -1);
}


inline
bool baea::operator==(
        const baea::Sequence3& lhs,
        const baea::Sequence3& rhs)
{
    return  lhs.element1() == rhs.element1()
         && lhs.element2() == rhs.element2()
         && lhs.element3() == rhs.element3()
         && lhs.element4() == rhs.element4()
         && lhs.element5() == rhs.element5()
         && lhs.element6() == rhs.element6();
}

inline
bool baea::operator!=(
        const baea::Sequence3& lhs,
        const baea::Sequence3& rhs)
{
    return  lhs.element1() != rhs.element1()
         || lhs.element2() != rhs.element2()
         || lhs.element3() != rhs.element3()
         || lhs.element4() != rhs.element4()
         || lhs.element5() != rhs.element5()
         || lhs.element6() != rhs.element6();
}

inline
std::ostream& baea::operator<<(
        std::ostream& stream,
        const baea::Sequence3& rhs)
{
    return rhs.print(stream, 0, -1);
}


inline
bool baea::operator==(
        const baea::Sequence5& lhs,
        const baea::Sequence5& rhs)
{
    return  lhs.element1() == rhs.element1()
         && lhs.element2() == rhs.element2()
         && lhs.element3() == rhs.element3()
         && lhs.element4() == rhs.element4()
         && lhs.element5() == rhs.element5()
         && lhs.element6() == rhs.element6()
         && lhs.element7() == rhs.element7();
}

inline
bool baea::operator!=(
        const baea::Sequence5& lhs,
        const baea::Sequence5& rhs)
{
    return  lhs.element1() != rhs.element1()
         || lhs.element2() != rhs.element2()
         || lhs.element3() != rhs.element3()
         || lhs.element4() != rhs.element4()
         || lhs.element5() != rhs.element5()
         || lhs.element6() != rhs.element6()
         || lhs.element7() != rhs.element7();
}

inline
std::ostream& baea::operator<<(
        std::ostream& stream,
        const baea::Sequence5& rhs)
{
    return rhs.print(stream, 0, -1);
}


inline
bool baea::operator==(
        const baea::Sequence6& lhs,
        const baea::Sequence6& rhs)
{
    return  lhs.element1() == rhs.element1()
         && lhs.element2() == rhs.element2()
         && lhs.element3() == rhs.element3()
         && lhs.element4() == rhs.element4()
         && lhs.element5() == rhs.element5()
         && lhs.element6() == rhs.element6()
         && lhs.element7() == rhs.element7()
         && lhs.element8() == rhs.element8()
         && lhs.element9() == rhs.element9()
         && lhs.element10() == rhs.element10()
         && lhs.element11() == rhs.element11();
}

inline
bool baea::operator!=(
        const baea::Sequence6& lhs,
        const baea::Sequence6& rhs)
{
    return  lhs.element1() != rhs.element1()
         || lhs.element2() != rhs.element2()
         || lhs.element3() != rhs.element3()
         || lhs.element4() != rhs.element4()
         || lhs.element5() != rhs.element5()
         || lhs.element6() != rhs.element6()
         || lhs.element7() != rhs.element7()
         || lhs.element8() != rhs.element8()
         || lhs.element9() != rhs.element9()
         || lhs.element10() != rhs.element10()
         || lhs.element11() != rhs.element11();
}

inline
std::ostream& baea::operator<<(
        std::ostream& stream,
        const baea::Sequence6& rhs)
{
    return rhs.print(stream, 0, -1);
}


inline
bool baea::operator==(
        const baea::Choice3& lhs,
        const baea::Choice3& rhs)
{
    typedef baea::Choice3 Class;
    if (lhs.selectionId() == rhs.selectionId()) {
        switch (rhs.selectionId()) {
          case Class::SELECTION_ID_SELECTION1:
            return lhs.selection1() == rhs.selection1();
          case Class::SELECTION_ID_SELECTION2:
            return lhs.selection2() == rhs.selection2();
          case Class::SELECTION_ID_SELECTION3:
            return lhs.selection3() == rhs.selection3();
          case Class::SELECTION_ID_SELECTION4:
            return lhs.selection4() == rhs.selection4();
          default:
            BSLS_ASSERT(Class::SELECTION_ID_UNDEFINED == rhs.selectionId());
            return true;
        }
    }
    else {
        return false;
   }
}

inline
bool baea::operator!=(
        const baea::Choice3& lhs,
        const baea::Choice3& rhs)
{
    return !(lhs == rhs);
}

inline
std::ostream& baea::operator<<(
        std::ostream& stream,
        const baea::Choice3& rhs)
{
    return rhs.print(stream, 0, -1);
}


inline
bool baea::operator==(
        const baea::Choice1& lhs,
        const baea::Choice1& rhs)
{
    typedef baea::Choice1 Class;
    if (lhs.selectionId() == rhs.selectionId()) {
        switch (rhs.selectionId()) {
          case Class::SELECTION_ID_SELECTION1:
            return lhs.selection1() == rhs.selection1();
          case Class::SELECTION_ID_SELECTION2:
            return lhs.selection2() == rhs.selection2();
          case Class::SELECTION_ID_SELECTION3:
            return lhs.selection3() == rhs.selection3();
          case Class::SELECTION_ID_SELECTION4:
            return lhs.selection4() == rhs.selection4();
          default:
            BSLS_ASSERT(Class::SELECTION_ID_UNDEFINED == rhs.selectionId());
            return true;
        }
    }
    else {
        return false;
   }
}

inline
bool baea::operator!=(
        const baea::Choice1& lhs,
        const baea::Choice1& rhs)
{
    return !(lhs == rhs);
}

inline
std::ostream& baea::operator<<(
        std::ostream& stream,
        const baea::Choice1& rhs)
{
    return rhs.print(stream, 0, -1);
}


inline
bool baea::operator==(
        const baea::Choice2& lhs,
        const baea::Choice2& rhs)
{
    typedef baea::Choice2 Class;
    if (lhs.selectionId() == rhs.selectionId()) {
        switch (rhs.selectionId()) {
          case Class::SELECTION_ID_SELECTION1:
            return lhs.selection1() == rhs.selection1();
          case Class::SELECTION_ID_SELECTION2:
            return lhs.selection2() == rhs.selection2();
          case Class::SELECTION_ID_SELECTION3:
            return lhs.selection3() == rhs.selection3();
          default:
            BSLS_ASSERT(Class::SELECTION_ID_UNDEFINED == rhs.selectionId());
            return true;
        }
    }
    else {
        return false;
   }
}

inline
bool baea::operator!=(
        const baea::Choice2& lhs,
        const baea::Choice2& rhs)
{
    return !(lhs == rhs);
}

inline
std::ostream& baea::operator<<(
        std::ostream& stream,
        const baea::Choice2& rhs)
{
    return rhs.print(stream, 0, -1);
}


inline
bool baea::operator==(
        const baea::Sequence4& lhs,
        const baea::Sequence4& rhs)
{
    return  lhs.element1() == rhs.element1()
         && lhs.element2() == rhs.element2()
         && lhs.element3() == rhs.element3()
         && lhs.element4() == rhs.element4()
         && lhs.element5() == rhs.element5()
         && lhs.element6() == rhs.element6()
         && lhs.element7() == rhs.element7()
         && lhs.element8() == rhs.element8()
         && lhs.element9() == rhs.element9()
         && lhs.element10() == rhs.element10()
         && lhs.element11() == rhs.element11()
         && lhs.element12() == rhs.element12()
         && lhs.element13() == rhs.element13()
         && lhs.element14() == rhs.element14()
         && lhs.element15() == rhs.element15()
         && lhs.element16() == rhs.element16()
         && lhs.element17() == rhs.element17()
         && lhs.element18() == rhs.element18()
         && lhs.element19() == rhs.element19();
}

inline
bool baea::operator!=(
        const baea::Sequence4& lhs,
        const baea::Sequence4& rhs)
{
    return  lhs.element1() != rhs.element1()
         || lhs.element2() != rhs.element2()
         || lhs.element3() != rhs.element3()
         || lhs.element4() != rhs.element4()
         || lhs.element5() != rhs.element5()
         || lhs.element6() != rhs.element6()
         || lhs.element7() != rhs.element7()
         || lhs.element8() != rhs.element8()
         || lhs.element9() != rhs.element9()
         || lhs.element10() != rhs.element10()
         || lhs.element11() != rhs.element11()
         || lhs.element12() != rhs.element12()
         || lhs.element13() != rhs.element13()
         || lhs.element14() != rhs.element14()
         || lhs.element15() != rhs.element15()
         || lhs.element16() != rhs.element16()
         || lhs.element17() != rhs.element17()
         || lhs.element18() != rhs.element18()
         || lhs.element19() != rhs.element19();
}

inline
std::ostream& baea::operator<<(
        std::ostream& stream,
        const baea::Sequence4& rhs)
{
    return rhs.print(stream, 0, -1);
}


inline
bool baea::operator==(
        const baea::Sequence1& lhs,
        const baea::Sequence1& rhs)
{
    return  lhs.element1() == rhs.element1()
         && lhs.element2() == rhs.element2()
         && lhs.element3() == rhs.element3()
         && lhs.element4() == rhs.element4();
}

inline
bool baea::operator!=(
        const baea::Sequence1& lhs,
        const baea::Sequence1& rhs)
{
    return  lhs.element1() != rhs.element1()
         || lhs.element2() != rhs.element2()
         || lhs.element3() != rhs.element3()
         || lhs.element4() != rhs.element4();
}

inline
std::ostream& baea::operator<<(
        std::ostream& stream,
        const baea::Sequence1& rhs)
{
    return rhs.print(stream, 0, -1);
}


inline
bool baea::operator==(
        const baea::Sequence2& lhs,
        const baea::Sequence2& rhs)
{
    return  lhs.element1() == rhs.element1()
         && lhs.element2() == rhs.element2()
         && lhs.element3() == rhs.element3()
         && lhs.element4() == rhs.element4()
         && lhs.element5() == rhs.element5();
}

inline
bool baea::operator!=(
        const baea::Sequence2& lhs,
        const baea::Sequence2& rhs)
{
    return  lhs.element1() != rhs.element1()
         || lhs.element2() != rhs.element2()
         || lhs.element3() != rhs.element3()
         || lhs.element4() != rhs.element4()
         || lhs.element5() != rhs.element5();
}

inline
std::ostream& baea::operator<<(
        std::ostream& stream,
        const baea::Sequence2& rhs)
{
    return rhs.print(stream, 0, -1);
}


inline
bool baea::operator==(
        const baea::FeatureTestMessage& lhs,
        const baea::FeatureTestMessage& rhs)
{
    typedef baea::FeatureTestMessage Class;
    if (lhs.selectionId() == rhs.selectionId()) {
        switch (rhs.selectionId()) {
          case Class::SELECTION_ID_SELECTION1:
            return lhs.selection1() == rhs.selection1();
          case Class::SELECTION_ID_SELECTION2:
            return lhs.selection2() == rhs.selection2();
          case Class::SELECTION_ID_SELECTION3:
            return lhs.selection3() == rhs.selection3();
          case Class::SELECTION_ID_SELECTION4:
            return lhs.selection4() == rhs.selection4();
          case Class::SELECTION_ID_SELECTION5:
            return lhs.selection5() == rhs.selection5();
          case Class::SELECTION_ID_SELECTION6:
            return lhs.selection6() == rhs.selection6();
          case Class::SELECTION_ID_SELECTION7:
            return lhs.selection7() == rhs.selection7();
          case Class::SELECTION_ID_SELECTION8:
            return lhs.selection8() == rhs.selection8();
          case Class::SELECTION_ID_SELECTION9:
            return lhs.selection9() == rhs.selection9();
          case Class::SELECTION_ID_SELECTION10:
            return lhs.selection10() == rhs.selection10();
          default:
            BSLS_ASSERT(Class::SELECTION_ID_UNDEFINED == rhs.selectionId());
            return true;
        }
    }
    else {
        return false;
   }
}

inline
bool baea::operator!=(
        const baea::FeatureTestMessage& lhs,
        const baea::FeatureTestMessage& rhs)
{
    return !(lhs == rhs);
}

inline
std::ostream& baea::operator<<(
        std::ostream& stream,
        const baea::FeatureTestMessage& rhs)
{
    return rhs.print(stream, 0, -1);
}


inline
bool baea::operator==(
        const baea::Request& lhs,
        const baea::Request& rhs)
{
    typedef baea::Request Class;
    if (lhs.selectionId() == rhs.selectionId()) {
        switch (rhs.selectionId()) {
          case Class::SELECTION_ID_SIMPLE_REQUEST:
            return lhs.simpleRequest() == rhs.simpleRequest();
          case Class::SELECTION_ID_FEATURE_REQUEST:
            return lhs.featureRequest() == rhs.featureRequest();
          default:
            BSLS_ASSERT(Class::SELECTION_ID_UNDEFINED == rhs.selectionId());
            return true;
        }
    }
    else {
        return false;
   }
}

inline
bool baea::operator!=(
        const baea::Request& lhs,
        const baea::Request& rhs)
{
    return !(lhs == rhs);
}

inline
std::ostream& baea::operator<<(
        std::ostream& stream,
        const baea::Request& rhs)
{
    return rhs.print(stream, 0, -1);
}


inline
bool baea::operator==(
        const baea::Response& lhs,
        const baea::Response& rhs)
{
    typedef baea::Response Class;
    if (lhs.selectionId() == rhs.selectionId()) {
        switch (rhs.selectionId()) {
          case Class::SELECTION_ID_RESPONSE_DATA:
            return lhs.responseData() == rhs.responseData();
          case Class::SELECTION_ID_FEATURE_RESPONSE:
            return lhs.featureResponse() == rhs.featureResponse();
          default:
            BSLS_ASSERT(Class::SELECTION_ID_UNDEFINED == rhs.selectionId());
            return true;
        }
    }
    else {
        return false;
   }
}

inline
bool baea::operator!=(
        const baea::Response& lhs,
        const baea::Response& rhs)
{
    return !(lhs == rhs);
}

inline
std::ostream& baea::operator<<(
        std::ostream& stream,
        const baea::Response& rhs)
{
    return rhs.print(stream, 0, -1);
}

}  // close namespace BloombergLP
#endif

// GENERATED BY BLP_BAS_CODEGEN_3.6.13 Sun May 20 16:22:40 2012
// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2012
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ------------------------------ END-OF-FILE ---------------------------------
