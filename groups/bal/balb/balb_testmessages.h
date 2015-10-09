// balb_testmessages.h       -- GENERATED FILE - DO NOT EDIT --       -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BALB_TESTMESSAGES
#define INCLUDED_BALB_TESTMESSAGES

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT_RCSID(baea_testmessages_h,"$Id$ $CSID$")
BSLS_IDENT_PRAGMA_ONCE

//@PURPOSE: Provide value-semantic attribute classes
//
//@DESCRIPTION: This is a generated file for testing purposes and should *not*
// be used in any production code.
//
// The file is generated with the command:
//..
//  bas_codegen.pl -mmsg -Ctestmessages --noAggregateConversion <backslaah>
//                 test/balb_testmessages.xsd
//..
// (Sustitute '\' for '<backslash>' -- some compilers won't accept a backslash
// at the end of a C++ comment line).
//
// After the message component is generated, the following modifications are
// made:
//: o This @DESCRIPTION section is added.

#ifndef INCLUDED_BALSCM_VERSION
#include <balscm_version.h>
#endif

#ifndef INCLUDED_BDLAT_ATTRIBUTEINFO
#include <bdlat_attributeinfo.h>
#endif

#ifndef INCLUDED_BDLAT_ENUMERATORINFO
#include <bdlat_enumeratorinfo.h>
#endif

#ifndef INCLUDED_BDLAT_SELECTIONINFO
#include <bdlat_selectioninfo.h>
#endif

#ifndef INCLUDED_BDLAT_TYPETRAITS
#include <bdlat_typetraits.h>
#endif

#ifndef INCLUDED_BDLAT_VALUETYPEFUNCTIONS
#include <bdlat_valuetypefunctions.h>
#endif

#ifndef INCLUDED_BDLT_DATETIMETZ
#include <bdlt_datetimetz.h>
#endif

#ifndef INCLUDED_BDLB_NULLABLEALLOCATEDVALUE
#include <bdlb_nullableallocatedvalue.h>
#endif

#ifndef INCLUDED_BDLB_NULLABLEVALUE
#include <bdlb_nullablevalue.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITS
#include <bslalg_typetraits.h>
#endif

#ifndef INCLUDED_BSLMA_ALLOCATOR
#include <bslma_allocator.h>
#endif

#ifndef INCLUDED_BSLMA_DEFAULT
#include <bslma_default.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSLS_OBJECTBUFFER
#include <bsls_objectbuffer.h>
#endif

#ifndef INCLUDED_BSLS_TYPES
#include <bsls_types.h>
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

#ifndef INCLUDED_BSL_VECTOR
#include <bsl_vector.h>
#endif

namespace BloombergLP {

namespace balb { class CustomInt; }
namespace balb { class CustomString; }
namespace balb { class SequenceWithAnonymityChoice1; }
namespace balb { class SimpleRequest; }
namespace balb { class UnsignedSequence; }
namespace balb { class VoidSequence; }
namespace balb { class Sequence3; }
namespace balb { class Sequence5; }
namespace balb { class Sequence6; }
namespace balb { class Choice3; }
namespace balb { class SequenceWithAnonymityChoice; }
namespace balb { class Choice1; }
namespace balb { class Choice2; }
namespace balb { class Sequence4; }
namespace balb { class Sequence1; }
namespace balb { class Sequence2; }
namespace balb { class SequenceWithAnonymityChoice2; }
namespace balb { class SequenceWithAnonymity; }
namespace balb { class FeatureTestMessage; }
namespace balb { class Request; }
namespace balb { class Response; }
namespace balb {

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

    void reset();
        // Reset this object to the default value (i.e., its value upon
        // default construction).

    int fromInt(const int& value);
        // Convert from the specified 'value' to this type.  Return 0 if
        // successful and non-zero otherwise.

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
bsl::ostream& operator<<(bsl::ostream& stream, const CustomInt& rhs);
    // Format the specified 'rhs' to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.

}  // close package namespace

// TRAITS

BDLAT_DECL_CUSTOMIZEDTYPE_WITH_BITWISEMOVEABLE_TRAITS(balb::CustomInt)

namespace balb {

                             // ==================
                             // class CustomString
                             // ==================

class CustomString {

    // INSTANCE DATA
    bsl::string d_value;

    // FRIENDS
    friend bool operator==(const CustomString& lhs, const CustomString& rhs);
    friend bool operator!=(const CustomString& lhs, const CustomString& rhs);

    // PRIVATE CLASS METHODS
    static int checkRestrictions(const bsl::string& value);
        // Check if the specified 'value' satisfies the restrictions of this
        // class (i.e., "CustomString").  Return 0 if successful (i.e., the
        // restrictions are satisfied) and non-zero otherwise.

  public:
    // TYPES
    typedef bsl::string BaseType;

    // CONSTANTS
    static const char CLASS_NAME[];

    // CREATORS
    explicit CustomString(bslma::Allocator *basicAllocator = 0);
        // Create an object of type 'CustomString' having the default value.
        // Use the optionally specified 'basicAllocator' to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.

    CustomString(const CustomString&  original,
                 bslma::Allocator    *basicAllocator = 0);
        // Create an object of type 'CustomString' having the value
        // of the specified 'original' object.  Use the optionally specified
        // 'basicAllocator' to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.

    explicit CustomString(const bsl::string&  value,
                          bslma::Allocator   *basicAllocator = 0);
        // Create an object of type 'CustomString' having the specified
        // 'value'.  Use the optionally specified 'basicAllocator' to supply
        // memory.  If 'basicAllocator' is 0, the currently installed default
        // allocator is used.

    ~CustomString();
        // Destroy this object.

    // MANIPULATORS
    CustomString& operator=(const CustomString& rhs);
        // Assign to this object the value of the specified 'rhs' object.

    void reset();
        // Reset this object to the default value (i.e., its value upon
        // default construction).

    int fromString(const bsl::string& value);
        // Convert from the specified 'value' to this type.  Return 0 if
        // successful and non-zero otherwise.

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

    const bsl::string& toString() const;
        // Convert this value to 'bsl::string'.
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
bsl::ostream& operator<<(bsl::ostream& stream, const CustomString& rhs);
    // Format the specified 'rhs' to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.

}  // close package namespace

// TRAITS

BDLAT_DECL_CUSTOMIZEDTYPE_WITH_ALLOCATOR_BITWISEMOVEABLE_TRAITS(balb::CustomString)

namespace balb {

                              // ================
                              // class Enumerated
                              // ================

struct Enumerated {

  public:
    // TYPES
    enum Value {
        e_NEW_YORK   = 0
      , e_NEW_JERSEY = 1
      , e_LONDON     = 2
    };

    enum {
        k_NUM_ENUMERATORS = 3
    };

    // CONSTANTS
    static const char CLASS_NAME[];

    static const bdlat_EnumeratorInfo ENUMERATOR_INFO_ARRAY[];

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
        // Write to the specified 'stream' the string representation of
        // the specified enumeration 'value'.  Return a reference to
        // the modifiable 'stream'.
};

// FREE OPERATORS
inline
bsl::ostream& operator<<(bsl::ostream& stream, Enumerated::Value rhs);
    // Format the specified 'rhs' to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.

}  // close package namespace

// TRAITS

BDLAT_DECL_ENUMERATION_TRAITS(balb::Enumerated)


namespace balb {

                     // ==================================
                     // class SequenceWithAnonymityChoice1
                     // ==================================

class SequenceWithAnonymityChoice1 {

    // INSTANCE DATA
    union {
        bsls::ObjectBuffer< bool >         d_selection5;
        bsls::ObjectBuffer< bsl::string >  d_selection6;
    };

    int                                    d_selectionId;
    bslma::Allocator                      *d_allocator_p;

  public:
    // TYPES
    enum {
        e_SELECTION_ID_UNDEFINED  = -1

      , e_SELECTION_ID_SELECTION5 = 0
      , e_SELECTION_ID_SELECTION6 = 1
    };

    enum {
        k_NUM_SELECTIONS = 2
    };

    enum {
        e_SELECTION_INDEX_SELECTION5 = 0
      , e_SELECTION_INDEX_SELECTION6 = 1
    };

    // CONSTANTS
    static const char CLASS_NAME[];

    static const bdlat_SelectionInfo SELECTION_INFO_ARRAY[];

    // CLASS METHODS
    static const bdlat_SelectionInfo *lookupSelectionInfo(int id);
        // Return selection information for the selection indicated by the
        // specified 'id' if the selection exists, and 0 otherwise.

    static const bdlat_SelectionInfo *lookupSelectionInfo(
                                                    const char *name,
                                                    int         nameLength);
        // Return selection information for the selection indicated by the
        // specified 'name' of the specified 'nameLength' if the selection
        // exists, and 0 otherwise.

    // CREATORS
    explicit SequenceWithAnonymityChoice1(bslma::Allocator *basicAllocator = 0);
        // Create an object of type 'SequenceWithAnonymityChoice1' having the
        // default value.  Use the optionally specified 'basicAllocator' to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.

    SequenceWithAnonymityChoice1(const SequenceWithAnonymityChoice1& original,
                                bslma::Allocator *basicAllocator = 0);
        // Create an object of type 'SequenceWithAnonymityChoice1' having the
        // value of the specified 'original' object.  Use the optionally
        // specified 'basicAllocator' to supply memory.  If 'basicAllocator' is
        // 0, the currently installed default allocator is used.

    ~SequenceWithAnonymityChoice1();
        // Destroy this object.

    // MANIPULATORS
    SequenceWithAnonymityChoice1& operator=(const SequenceWithAnonymityChoice1& rhs);
        // Assign to this object the value of the specified 'rhs' object.

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

    bool& makeSelection5();
    bool& makeSelection5(bool value);
        // Set the value of this object to be a "Selection5" value.  Optionally
        // specify the 'value' of the "Selection5".  If 'value' is not
        // specified, the default "Selection5" value is used.

    bsl::string& makeSelection6();
    bsl::string& makeSelection6(const bsl::string& value);
        // Set the value of this object to be a "Selection6" value.  Optionally
        // specify the 'value' of the "Selection6".  If 'value' is not
        // specified, the default "Selection6" value is used.

    template<class MANIPULATOR>
    int manipulateSelection(MANIPULATOR& manipulator);
        // Invoke the specified 'manipulator' on the address of the modifiable
        // selection, supplying 'manipulator' with the corresponding selection
        // information structure.  Return the value returned from the
        // invocation of 'manipulator' if this object has a defined selection,
        // and -1 otherwise.

    bool& selection5();
        // Return a reference to the modifiable "Selection5" selection of this
        // object if "Selection5" is the current selection.  The behavior is
        // undefined unless "Selection5" is the selection of this object.

    bsl::string& selection6();
        // Return a reference to the modifiable "Selection6" selection of this
        // object if "Selection6" is the current selection.  The behavior is
        // undefined unless "Selection6" is the selection of this object.

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

    int selectionId() const;
        // Return the id of the current selection if the selection is defined,
        // and -1 otherwise.

    template<class ACCESSOR>
    int accessSelection(ACCESSOR& accessor) const;
        // Invoke the specified 'accessor' on the non-modifiable selection,
        // supplying 'accessor' with the corresponding selection information
        // structure.  Return the value returned from the invocation of
        // 'accessor' if this object has a defined selection, and -1 otherwise.

    const bool& selection5() const;
        // Return a reference to the non-modifiable "Selection5" selection of
        // this object if "Selection5" is the current selection.  The behavior
        // is undefined unless "Selection5" is the selection of this object.

    const bsl::string& selection6() const;
        // Return a reference to the non-modifiable "Selection6" selection of
        // this object if "Selection6" is the current selection.  The behavior
        // is undefined unless "Selection6" is the selection of this object.

    bool isSelection5Value() const;
        // Return 'true' if the value of this object is a "Selection5" value,
        // and return 'false' otherwise.

    bool isSelection6Value() const;
        // Return 'true' if the value of this object is a "Selection6" value,
        // and return 'false' otherwise.

    bool isUndefinedValue() const;
        // Return 'true' if the value of this object is undefined, and 'false'
        // otherwise.

    const char *selectionName() const;
        // Return the symbolic name of the current selection of this object.
};

// FREE OPERATORS
inline
bool operator==(const SequenceWithAnonymityChoice1& lhs, const SequenceWithAnonymityChoice1& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects have the same
    // value, and 'false' otherwise.  Two 'SequenceWithAnonymityChoice1' objects have the same
    // value if either the selections in both objects have the same ids and
    // the same values, or both selections are undefined.

inline
bool operator!=(const SequenceWithAnonymityChoice1& lhs, const SequenceWithAnonymityChoice1& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects do not have the
    // same values, as determined by 'operator==', and 'false' otherwise.

inline
bsl::ostream& operator<<(bsl::ostream& stream, const SequenceWithAnonymityChoice1& rhs);
    // Format the specified 'rhs' to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.

}  // close package namespace

// TRAITS

BDLAT_DECL_CHOICE_WITH_ALLOCATOR_BITWISEMOVEABLE_TRAITS(balb::SequenceWithAnonymityChoice1)

namespace balb {

                            // ===================
                            // class SimpleRequest
                            // ===================

class SimpleRequest {

    // INSTANCE DATA
    bsl::string  d_data;
    int          d_responseLength;

  public:
    // TYPES
    enum {
        e_ATTRIBUTE_ID_DATA            = 0
      , e_ATTRIBUTE_ID_RESPONSE_LENGTH = 1
    };

    enum {
        k_NUM_ATTRIBUTES = 2
    };

    enum {
        e_ATTRIBUTE_INDEX_DATA            = 0
      , e_ATTRIBUTE_INDEX_RESPONSE_LENGTH = 1
    };

    // CONSTANTS
    static const char CLASS_NAME[];

    static const bdlat_AttributeInfo ATTRIBUTE_INFO_ARRAY[];

  public:
    // CLASS METHODS
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
    explicit SimpleRequest(bslma::Allocator *basicAllocator = 0);
        // Create an object of type 'SimpleRequest' having the default value.
        // Use the optionally specified 'basicAllocator' to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.

    SimpleRequest(const SimpleRequest&  original,
                  bslma::Allocator     *basicAllocator = 0);
        // Create an object of type 'SimpleRequest' having the value of the
        // specified 'original' object.  Use the optionally specified
        // 'basicAllocator' to supply memory.  If 'basicAllocator' is 0, the
        // currently installed default allocator is used.

    ~SimpleRequest();
        // Destroy this object.

    // MANIPULATORS
    SimpleRequest& operator=(const SimpleRequest& rhs);
        // Assign to this object the value of the specified 'rhs' object.

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

    bsl::string& data();
        // Return a reference to the modifiable "Data" attribute of this
        // object.

    int& responseLength();
        // Return a reference to the modifiable "ResponseLength" attribute of
        // this object.

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

    const bsl::string& data() const;
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
bsl::ostream& operator<<(bsl::ostream& stream, const SimpleRequest& rhs);
    // Format the specified 'rhs' to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.

}  // close package namespace

// TRAITS

BDLAT_DECL_SEQUENCE_WITH_ALLOCATOR_BITWISEMOVEABLE_TRAITS(balb::SimpleRequest)

namespace balb {

                           // ======================
                           // class UnsignedSequence
                           // ======================

class UnsignedSequence {

    // INSTANCE DATA
    bsls::Types::Uint64 d_element3;
    unsigned int        d_element1;
    unsigned short      d_element2;

  public:
    // TYPES
    enum {
        e_ATTRIBUTE_ID_ELEMENT1 = 0
      , e_ATTRIBUTE_ID_ELEMENT2 = 1
      , e_ATTRIBUTE_ID_ELEMENT3 = 2
    };

    enum {
        k_NUM_ATTRIBUTES = 3
    };

    enum {
        e_ATTRIBUTE_INDEX_ELEMENT1 = 0
      , e_ATTRIBUTE_INDEX_ELEMENT2 = 1
      , e_ATTRIBUTE_INDEX_ELEMENT3 = 2
    };

    // CONSTANTS
    static const char CLASS_NAME[];

    static const bdlat_AttributeInfo ATTRIBUTE_INFO_ARRAY[];

  public:
    // CLASS METHODS
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

    bsls::Types::Uint64& element3();
        // Return a reference to the modifiable "Element3" attribute of this
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

    bsls::Types::Uint64 element3() const;
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
bsl::ostream& operator<<(bsl::ostream& stream, const UnsignedSequence& rhs);
    // Format the specified 'rhs' to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.

}  // close package namespace

// TRAITS

BDLAT_DECL_SEQUENCE_WITH_BITWISEMOVEABLE_TRAITS(balb::UnsignedSequence)

namespace balb {

                             // ==================
                             // class VoidSequence
                             // ==================

class VoidSequence {

    // INSTANCE DATA

  public:
    // TYPES
    enum {
        k_NUM_ATTRIBUTES = 0
    };


    // CONSTANTS
    static const char CLASS_NAME[];

  public:
    // CLASS METHODS
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
bsl::ostream& operator<<(bsl::ostream& stream, const VoidSequence& rhs);
    // Format the specified 'rhs' to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.

}  // close package namespace

// TRAITS

BDLAT_DECL_SEQUENCE_WITH_BITWISEMOVEABLE_TRAITS(balb::VoidSequence)

namespace balb {

                              // ===============
                              // class Sequence3
                              // ===============

class Sequence3 {

    // INSTANCE DATA
    bsl::vector<bsl::string>                              d_element2;
    bdlb::NullableValue<bsl::string>                      d_element4;
    bdlb::NullableAllocatedValue<Sequence5>               d_element5;
    bsl::vector<bdlb::NullableValue<Enumerated::Value> >  d_element6;
    bsl::vector<Enumerated::Value>                        d_element1;
    bdlb::NullableValue<bool>                             d_element3;

  public:
    // TYPES
    enum {
        e_ATTRIBUTE_ID_ELEMENT1 = 0
      , e_ATTRIBUTE_ID_ELEMENT2 = 1
      , e_ATTRIBUTE_ID_ELEMENT3 = 2
      , e_ATTRIBUTE_ID_ELEMENT4 = 3
      , e_ATTRIBUTE_ID_ELEMENT5 = 4
      , e_ATTRIBUTE_ID_ELEMENT6 = 5
    };

    enum {
        k_NUM_ATTRIBUTES = 6
    };

    enum {
        e_ATTRIBUTE_INDEX_ELEMENT1 = 0
      , e_ATTRIBUTE_INDEX_ELEMENT2 = 1
      , e_ATTRIBUTE_INDEX_ELEMENT3 = 2
      , e_ATTRIBUTE_INDEX_ELEMENT4 = 3
      , e_ATTRIBUTE_INDEX_ELEMENT5 = 4
      , e_ATTRIBUTE_INDEX_ELEMENT6 = 5
    };

    // CONSTANTS
    static const char CLASS_NAME[];

    static const bdlat_AttributeInfo ATTRIBUTE_INFO_ARRAY[];

  public:
    // CLASS METHODS
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
    explicit Sequence3(bslma::Allocator *basicAllocator = 0);
        // Create an object of type 'Sequence3' having the default value.  Use
        // the optionally specified 'basicAllocator' to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.

    Sequence3(const Sequence3&  original,
              bslma::Allocator *basicAllocator = 0);
        // Create an object of type 'Sequence3' having the value of the
        // specified 'original' object.  Use the optionally specified
        // 'basicAllocator' to supply memory.  If 'basicAllocator' is 0, the
        // currently installed default allocator is used.

    ~Sequence3();
        // Destroy this object.

    // MANIPULATORS
    Sequence3& operator=(const Sequence3& rhs);
        // Assign to this object the value of the specified 'rhs' object.

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

    bsl::vector<Enumerated::Value>& element1();
        // Return a reference to the modifiable "Element1" attribute of this
        // object.

    bsl::vector<bsl::string>& element2();
        // Return a reference to the modifiable "Element2" attribute of this
        // object.

    bdlb::NullableValue<bool>& element3();
        // Return a reference to the modifiable "Element3" attribute of this
        // object.

    bdlb::NullableValue<bsl::string>& element4();
        // Return a reference to the modifiable "Element4" attribute of this
        // object.

    bdlb::NullableAllocatedValue<Sequence5>& element5();
        // Return a reference to the modifiable "Element5" attribute of this
        // object.

    bsl::vector<bdlb::NullableValue<Enumerated::Value> >& element6();
        // Return a reference to the modifiable "Element6" attribute of this
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

    const bsl::vector<Enumerated::Value>& element1() const;
        // Return a reference to the non-modifiable "Element1" attribute of
        // this object.

    const bsl::vector<bsl::string>& element2() const;
        // Return a reference to the non-modifiable "Element2" attribute of
        // this object.

    const bdlb::NullableValue<bool>& element3() const;
        // Return a reference to the non-modifiable "Element3" attribute of
        // this object.

    const bdlb::NullableValue<bsl::string>& element4() const;
        // Return a reference to the non-modifiable "Element4" attribute of
        // this object.

    const bdlb::NullableAllocatedValue<Sequence5>& element5() const;
        // Return a reference to the non-modifiable "Element5" attribute of
        // this object.

    const bsl::vector<bdlb::NullableValue<Enumerated::Value> >& element6() const;
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
bsl::ostream& operator<<(bsl::ostream& stream, const Sequence3& rhs);
    // Format the specified 'rhs' to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.

}  // close package namespace

// TRAITS

BDLAT_DECL_SEQUENCE_WITH_ALLOCATOR_BITWISEMOVEABLE_TRAITS(balb::Sequence3)

namespace balb {

                              // ===============
                              // class Sequence5
                              // ===============

class Sequence5 {

    // INSTANCE DATA
    bslma::Allocator                                      *d_allocator_p;
    bsl::vector<bdlb::NullableValue<int> >                 d_element5;
    bsl::vector<bdlb::NullableValue<double> >              d_element3;
    bsl::vector<bdlb::NullableValue<bsl::vector<char> > >  d_element4;
    bsl::vector<bdlb::NullableValue<bool> >                d_element2;
    bsl::vector<bdlb::NullableValue<bdlt::DatetimeTz> >     d_element6;
    bsl::vector<bdlb::NullableAllocatedValue<Sequence3> >  d_element7;
    Sequence3                                             *d_element1;

  public:
    // TYPES
    enum {
        e_ATTRIBUTE_ID_ELEMENT1 = 0
      , e_ATTRIBUTE_ID_ELEMENT2 = 1
      , e_ATTRIBUTE_ID_ELEMENT3 = 2
      , e_ATTRIBUTE_ID_ELEMENT4 = 3
      , e_ATTRIBUTE_ID_ELEMENT5 = 4
      , e_ATTRIBUTE_ID_ELEMENT6 = 5
      , e_ATTRIBUTE_ID_ELEMENT7 = 6
    };

    enum {
        k_NUM_ATTRIBUTES = 7
    };

    enum {
        e_ATTRIBUTE_INDEX_ELEMENT1 = 0
      , e_ATTRIBUTE_INDEX_ELEMENT2 = 1
      , e_ATTRIBUTE_INDEX_ELEMENT3 = 2
      , e_ATTRIBUTE_INDEX_ELEMENT4 = 3
      , e_ATTRIBUTE_INDEX_ELEMENT5 = 4
      , e_ATTRIBUTE_INDEX_ELEMENT6 = 5
      , e_ATTRIBUTE_INDEX_ELEMENT7 = 6
    };

    // CONSTANTS
    static const char CLASS_NAME[];

    static const bdlat_AttributeInfo ATTRIBUTE_INFO_ARRAY[];

  public:
    // CLASS METHODS
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
    explicit Sequence5(bslma::Allocator *basicAllocator = 0);
        // Create an object of type 'Sequence5' having the default value.  Use
        // the optionally specified 'basicAllocator' to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.

    Sequence5(const Sequence5&  original,
              bslma::Allocator *basicAllocator = 0);
        // Create an object of type 'Sequence5' having the value of the
        // specified 'original' object.  Use the optionally specified
        // 'basicAllocator' to supply memory.  If 'basicAllocator' is 0, the
        // currently installed default allocator is used.

    ~Sequence5();
        // Destroy this object.

    // MANIPULATORS
    Sequence5& operator=(const Sequence5& rhs);
        // Assign to this object the value of the specified 'rhs' object.

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

    bsl::vector<bdlb::NullableValue<bool> >& element2();
        // Return a reference to the modifiable "Element2" attribute of this
        // object.

    bsl::vector<bdlb::NullableValue<double> >& element3();
        // Return a reference to the modifiable "Element3" attribute of this
        // object.

    bsl::vector<bdlb::NullableValue<bsl::vector<char> > >& element4();
        // Return a reference to the modifiable "Element4" attribute of this
        // object.

    bsl::vector<bdlb::NullableValue<int> >& element5();
        // Return a reference to the modifiable "Element5" attribute of this
        // object.

    bsl::vector<bdlb::NullableValue<bdlt::DatetimeTz> >& element6();
        // Return a reference to the modifiable "Element6" attribute of this
        // object.

    bsl::vector<bdlb::NullableAllocatedValue<Sequence3> >& element7();
        // Return a reference to the modifiable "Element7" attribute of this
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

    const bsl::vector<bdlb::NullableValue<bool> >& element2() const;
        // Return a reference to the non-modifiable "Element2" attribute of
        // this object.

    const bsl::vector<bdlb::NullableValue<double> >& element3() const;
        // Return a reference to the non-modifiable "Element3" attribute of
        // this object.

    const bsl::vector<bdlb::NullableValue<bsl::vector<char> > >& element4() const;
        // Return a reference to the non-modifiable "Element4" attribute of
        // this object.

    const bsl::vector<bdlb::NullableValue<int> >& element5() const;
        // Return a reference to the non-modifiable "Element5" attribute of
        // this object.

    const bsl::vector<bdlb::NullableValue<bdlt::DatetimeTz> >& element6() const;
        // Return a reference to the non-modifiable "Element6" attribute of
        // this object.

    const bsl::vector<bdlb::NullableAllocatedValue<Sequence3> >& element7() const;
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
bsl::ostream& operator<<(bsl::ostream& stream, const Sequence5& rhs);
    // Format the specified 'rhs' to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.

}  // close package namespace

// TRAITS

BDLAT_DECL_SEQUENCE_WITH_ALLOCATOR_BITWISEMOVEABLE_TRAITS(balb::Sequence5)

namespace balb {

                              // ===============
                              // class Sequence6
                              // ===============

class Sequence6 {

    // INSTANCE DATA
    bsl::vector<unsigned int>                         d_element12;
    bsl::vector<unsigned char>                        d_element10;
    bsl::vector<bdlb::NullableValue<unsigned int> >   d_element15;
    bsl::vector<bdlb::NullableValue<unsigned char> >  d_element13;
    bsl::vector<CustomString>                         d_element11;
    bdlb::NullableValue<CustomString>                 d_element2;
    CustomString                                      d_element7;
    unsigned int                                      d_element4;
    bsl::vector<bdlb::NullableValue<CustomInt> >      d_element6;
    bsl::vector<CustomInt>                            d_element14;
    bdlb::NullableValue<unsigned int>                 d_element9;
    bdlb::NullableValue<CustomInt>                    d_element3;
    CustomInt                                         d_element8;
    unsigned char                                     d_element5;
    bdlb::NullableValue<unsigned char>                d_element1;

  public:
    // TYPES
    enum {
        e_ATTRIBUTE_ID_ELEMENT1  = 0
      , e_ATTRIBUTE_ID_ELEMENT2  = 1
      , e_ATTRIBUTE_ID_ELEMENT3  = 2
      , e_ATTRIBUTE_ID_ELEMENT4  = 3
      , e_ATTRIBUTE_ID_ELEMENT5  = 4
      , e_ATTRIBUTE_ID_ELEMENT6  = 5
      , e_ATTRIBUTE_ID_ELEMENT7  = 6
      , e_ATTRIBUTE_ID_ELEMENT8  = 7
      , e_ATTRIBUTE_ID_ELEMENT9  = 8
      , e_ATTRIBUTE_ID_ELEMENT10 = 9
      , e_ATTRIBUTE_ID_ELEMENT11 = 10
      , e_ATTRIBUTE_ID_ELEMENT12 = 11
      , e_ATTRIBUTE_ID_ELEMENT13 = 12
      , e_ATTRIBUTE_ID_ELEMENT14 = 13
      , e_ATTRIBUTE_ID_ELEMENT15 = 14
    };

    enum {
        k_NUM_ATTRIBUTES = 15
    };

    enum {
        e_ATTRIBUTE_INDEX_ELEMENT1  = 0
      , e_ATTRIBUTE_INDEX_ELEMENT2  = 1
      , e_ATTRIBUTE_INDEX_ELEMENT3  = 2
      , e_ATTRIBUTE_INDEX_ELEMENT4  = 3
      , e_ATTRIBUTE_INDEX_ELEMENT5  = 4
      , e_ATTRIBUTE_INDEX_ELEMENT6  = 5
      , e_ATTRIBUTE_INDEX_ELEMENT7  = 6
      , e_ATTRIBUTE_INDEX_ELEMENT8  = 7
      , e_ATTRIBUTE_INDEX_ELEMENT9  = 8
      , e_ATTRIBUTE_INDEX_ELEMENT10 = 9
      , e_ATTRIBUTE_INDEX_ELEMENT11 = 10
      , e_ATTRIBUTE_INDEX_ELEMENT12 = 11
      , e_ATTRIBUTE_INDEX_ELEMENT13 = 12
      , e_ATTRIBUTE_INDEX_ELEMENT14 = 13
      , e_ATTRIBUTE_INDEX_ELEMENT15 = 14
    };

    // CONSTANTS
    static const char CLASS_NAME[];

    static const bdlat_AttributeInfo ATTRIBUTE_INFO_ARRAY[];

  public:
    // CLASS METHODS
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
    explicit Sequence6(bslma::Allocator *basicAllocator = 0);
        // Create an object of type 'Sequence6' having the default value.  Use
        // the optionally specified 'basicAllocator' to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.

    Sequence6(const Sequence6&  original,
              bslma::Allocator *basicAllocator = 0);
        // Create an object of type 'Sequence6' having the value of the
        // specified 'original' object.  Use the optionally specified
        // 'basicAllocator' to supply memory.  If 'basicAllocator' is 0, the
        // currently installed default allocator is used.

    ~Sequence6();
        // Destroy this object.

    // MANIPULATORS
    Sequence6& operator=(const Sequence6& rhs);
        // Assign to this object the value of the specified 'rhs' object.

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

    bdlb::NullableValue<unsigned char>& element1();
        // Return a reference to the modifiable "Element1" attribute of this
        // object.

    bdlb::NullableValue<CustomString>& element2();
        // Return a reference to the modifiable "Element2" attribute of this
        // object.

    bdlb::NullableValue<CustomInt>& element3();
        // Return a reference to the modifiable "Element3" attribute of this
        // object.

    unsigned int& element4();
        // Return a reference to the modifiable "Element4" attribute of this
        // object.

    unsigned char& element5();
        // Return a reference to the modifiable "Element5" attribute of this
        // object.

    bsl::vector<bdlb::NullableValue<CustomInt> >& element6();
        // Return a reference to the modifiable "Element6" attribute of this
        // object.

    CustomString& element7();
        // Return a reference to the modifiable "Element7" attribute of this
        // object.

    CustomInt& element8();
        // Return a reference to the modifiable "Element8" attribute of this
        // object.

    bdlb::NullableValue<unsigned int>& element9();
        // Return a reference to the modifiable "Element9" attribute of this
        // object.

    bsl::vector<unsigned char>& element10();
        // Return a reference to the modifiable "Element10" attribute of this
        // object.

    bsl::vector<CustomString>& element11();
        // Return a reference to the modifiable "Element11" attribute of this
        // object.

    bsl::vector<unsigned int>& element12();
        // Return a reference to the modifiable "Element12" attribute of this
        // object.

    bsl::vector<bdlb::NullableValue<unsigned char> >& element13();
        // Return a reference to the modifiable "Element13" attribute of this
        // object.

    bsl::vector<CustomInt>& element14();
        // Return a reference to the modifiable "Element14" attribute of this
        // object.

    bsl::vector<bdlb::NullableValue<unsigned int> >& element15();
        // Return a reference to the modifiable "Element15" attribute of this
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

    const bdlb::NullableValue<unsigned char>& element1() const;
        // Return a reference to the non-modifiable "Element1" attribute of
        // this object.

    const bdlb::NullableValue<CustomString>& element2() const;
        // Return a reference to the non-modifiable "Element2" attribute of
        // this object.

    const bdlb::NullableValue<CustomInt>& element3() const;
        // Return a reference to the non-modifiable "Element3" attribute of
        // this object.

    unsigned int element4() const;
        // Return a reference to the non-modifiable "Element4" attribute of
        // this object.

    unsigned char element5() const;
        // Return a reference to the non-modifiable "Element5" attribute of
        // this object.

    const bsl::vector<bdlb::NullableValue<CustomInt> >& element6() const;
        // Return a reference to the non-modifiable "Element6" attribute of
        // this object.

    const CustomString& element7() const;
        // Return a reference to the non-modifiable "Element7" attribute of
        // this object.

    const CustomInt& element8() const;
        // Return a reference to the non-modifiable "Element8" attribute of
        // this object.

    const bdlb::NullableValue<unsigned int>& element9() const;
        // Return a reference to the non-modifiable "Element9" attribute of
        // this object.

    const bsl::vector<unsigned char>& element10() const;
        // Return a reference to the non-modifiable "Element10" attribute of
        // this object.

    const bsl::vector<CustomString>& element11() const;
        // Return a reference to the non-modifiable "Element11" attribute of
        // this object.

    const bsl::vector<unsigned int>& element12() const;
        // Return a reference to the non-modifiable "Element12" attribute of
        // this object.

    const bsl::vector<bdlb::NullableValue<unsigned char> >& element13() const;
        // Return a reference to the non-modifiable "Element13" attribute of
        // this object.

    const bsl::vector<CustomInt>& element14() const;
        // Return a reference to the non-modifiable "Element14" attribute of
        // this object.

    const bsl::vector<bdlb::NullableValue<unsigned int> >& element15() const;
        // Return a reference to the non-modifiable "Element15" attribute of
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
bsl::ostream& operator<<(bsl::ostream& stream, const Sequence6& rhs);
    // Format the specified 'rhs' to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.

}  // close package namespace

// TRAITS

BDLAT_DECL_SEQUENCE_WITH_ALLOCATOR_BITWISEMOVEABLE_TRAITS(balb::Sequence6)

namespace balb {

                               // =============
                               // class Choice3
                               // =============

class Choice3 {

    // INSTANCE DATA
    union {
        bsls::ObjectBuffer< Sequence6 >     d_selection1;
        bsls::ObjectBuffer< unsigned char > d_selection2;
        bsls::ObjectBuffer< CustomString >  d_selection3;
        bsls::ObjectBuffer< CustomInt >     d_selection4;
    };

    int                                     d_selectionId;
    bslma::Allocator                       *d_allocator_p;

  public:
    // TYPES
    enum {
        e_SELECTION_ID_UNDEFINED  = -1

      , e_SELECTION_ID_SELECTION1 = 0
      , e_SELECTION_ID_SELECTION2 = 1
      , e_SELECTION_ID_SELECTION3 = 2
      , e_SELECTION_ID_SELECTION4 = 3
    };

    enum {
        k_NUM_SELECTIONS = 4
    };

    enum {
        e_SELECTION_INDEX_SELECTION1 = 0
      , e_SELECTION_INDEX_SELECTION2 = 1
      , e_SELECTION_INDEX_SELECTION3 = 2
      , e_SELECTION_INDEX_SELECTION4 = 3
    };

    // CONSTANTS
    static const char CLASS_NAME[];

    static const bdlat_SelectionInfo SELECTION_INFO_ARRAY[];

    // CLASS METHODS
    static const bdlat_SelectionInfo *lookupSelectionInfo(int id);
        // Return selection information for the selection indicated by the
        // specified 'id' if the selection exists, and 0 otherwise.

    static const bdlat_SelectionInfo *lookupSelectionInfo(
                                                    const char *name,
                                                    int         nameLength);
        // Return selection information for the selection indicated by the
        // specified 'name' of the specified 'nameLength' if the selection
        // exists, and 0 otherwise.

    // CREATORS
    explicit Choice3(bslma::Allocator *basicAllocator = 0);
        // Create an object of type 'Choice3' having the default value.  Use
        // the optionally specified 'basicAllocator' to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.

    Choice3(const Choice3&  original,
           bslma::Allocator *basicAllocator = 0);
        // Create an object of type 'Choice3' having the value of the specified
        // 'original' object.  Use the optionally specified 'basicAllocator' to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.

    ~Choice3();
        // Destroy this object.

    // MANIPULATORS
    Choice3& operator=(const Choice3& rhs);
        // Assign to this object the value of the specified 'rhs' object.

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
bsl::ostream& operator<<(bsl::ostream& stream, const Choice3& rhs);
    // Format the specified 'rhs' to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.

}  // close package namespace

// TRAITS

BDLAT_DECL_CHOICE_WITH_ALLOCATOR_BITWISEMOVEABLE_TRAITS(balb::Choice3)

namespace balb {

                     // =================================
                     // class SequenceWithAnonymityChoice
                     // =================================

class SequenceWithAnonymityChoice {

    // INSTANCE DATA
    union {
        bsls::ObjectBuffer< Sequence6 >     d_selection1;
        bsls::ObjectBuffer< unsigned char > d_selection2;
        bsls::ObjectBuffer< CustomString >  d_selection3;
        bsls::ObjectBuffer< CustomInt >     d_selection4;
    };

    int                                     d_selectionId;
    bslma::Allocator                       *d_allocator_p;

  public:
    // TYPES
    enum {
        e_SELECTION_ID_UNDEFINED  = -1

      , e_SELECTION_ID_SELECTION1 = 0
      , e_SELECTION_ID_SELECTION2 = 1
      , e_SELECTION_ID_SELECTION3 = 2
      , e_SELECTION_ID_SELECTION4 = 3
    };

    enum {
        k_NUM_SELECTIONS = 4
    };

    enum {
        e_SELECTION_INDEX_SELECTION1 = 0
      , e_SELECTION_INDEX_SELECTION2 = 1
      , e_SELECTION_INDEX_SELECTION3 = 2
      , e_SELECTION_INDEX_SELECTION4 = 3
    };

    // CONSTANTS
    static const char CLASS_NAME[];

    static const bdlat_SelectionInfo SELECTION_INFO_ARRAY[];

    // CLASS METHODS
    static const bdlat_SelectionInfo *lookupSelectionInfo(int id);
        // Return selection information for the selection indicated by the
        // specified 'id' if the selection exists, and 0 otherwise.

    static const bdlat_SelectionInfo *lookupSelectionInfo(
                                                    const char *name,
                                                    int         nameLength);
        // Return selection information for the selection indicated by the
        // specified 'name' of the specified 'nameLength' if the selection
        // exists, and 0 otherwise.

    // CREATORS
    explicit SequenceWithAnonymityChoice(bslma::Allocator *basicAllocator = 0);
        // Create an object of type 'SequenceWithAnonymityChoice' having the
        // default value.  Use the optionally specified 'basicAllocator' to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.

    SequenceWithAnonymityChoice(const SequenceWithAnonymityChoice& original,
                               bslma::Allocator *basicAllocator = 0);
        // Create an object of type 'SequenceWithAnonymityChoice' having the
        // value of the specified 'original' object.  Use the optionally
        // specified 'basicAllocator' to supply memory.  If 'basicAllocator' is
        // 0, the currently installed default allocator is used.

    ~SequenceWithAnonymityChoice();
        // Destroy this object.

    // MANIPULATORS
    SequenceWithAnonymityChoice& operator=(const SequenceWithAnonymityChoice& rhs);
        // Assign to this object the value of the specified 'rhs' object.

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
bool operator==(const SequenceWithAnonymityChoice& lhs, const SequenceWithAnonymityChoice& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects have the same
    // value, and 'false' otherwise.  Two 'SequenceWithAnonymityChoice' objects have the same
    // value if either the selections in both objects have the same ids and
    // the same values, or both selections are undefined.

inline
bool operator!=(const SequenceWithAnonymityChoice& lhs, const SequenceWithAnonymityChoice& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects do not have the
    // same values, as determined by 'operator==', and 'false' otherwise.

inline
bsl::ostream& operator<<(bsl::ostream& stream, const SequenceWithAnonymityChoice& rhs);
    // Format the specified 'rhs' to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.

}  // close package namespace

// TRAITS

BDLAT_DECL_CHOICE_WITH_ALLOCATOR_BITWISEMOVEABLE_TRAITS(balb::SequenceWithAnonymityChoice)

namespace balb {

                               // =============
                               // class Choice1
                               // =============

class Choice1 {

    // INSTANCE DATA
    union {
        bsls::ObjectBuffer< int >     d_selection1;
        bsls::ObjectBuffer< double >  d_selection2;
        Sequence4                    *d_selection3;
        Choice2                      *d_selection4;
    };

    int                               d_selectionId;
    bslma::Allocator                 *d_allocator_p;

  public:
    // TYPES
    enum {
        e_SELECTION_ID_UNDEFINED  = -1

      , e_SELECTION_ID_SELECTION1 = 0
      , e_SELECTION_ID_SELECTION2 = 1
      , e_SELECTION_ID_SELECTION3 = 2
      , e_SELECTION_ID_SELECTION4 = 3
    };

    enum {
        k_NUM_SELECTIONS = 4
    };

    enum {
        e_SELECTION_INDEX_SELECTION1 = 0
      , e_SELECTION_INDEX_SELECTION2 = 1
      , e_SELECTION_INDEX_SELECTION3 = 2
      , e_SELECTION_INDEX_SELECTION4 = 3
    };

    // CONSTANTS
    static const char CLASS_NAME[];

    static const bdlat_SelectionInfo SELECTION_INFO_ARRAY[];

    // CLASS METHODS
    static const bdlat_SelectionInfo *lookupSelectionInfo(int id);
        // Return selection information for the selection indicated by the
        // specified 'id' if the selection exists, and 0 otherwise.

    static const bdlat_SelectionInfo *lookupSelectionInfo(
                                                    const char *name,
                                                    int         nameLength);
        // Return selection information for the selection indicated by the
        // specified 'name' of the specified 'nameLength' if the selection
        // exists, and 0 otherwise.

    // CREATORS
    explicit Choice1(bslma::Allocator *basicAllocator = 0);
        // Create an object of type 'Choice1' having the default value.  Use
        // the optionally specified 'basicAllocator' to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.

    Choice1(const Choice1& original,
           bslma::Allocator *basicAllocator = 0);
        // Create an object of type 'Choice1' having the value of the specified
        // 'original' object.  Use the optionally specified 'basicAllocator' to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.

    ~Choice1();
        // Destroy this object.

    // MANIPULATORS
    Choice1& operator=(const Choice1& rhs);
        // Assign to this object the value of the specified 'rhs' object.

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
bsl::ostream& operator<<(bsl::ostream& stream, const Choice1& rhs);
    // Format the specified 'rhs' to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.

}  // close package namespace

// TRAITS

BDLAT_DECL_CHOICE_WITH_ALLOCATOR_BITWISEMOVEABLE_TRAITS(balb::Choice1)

namespace balb {

                               // =============
                               // class Choice2
                               // =============

class Choice2 {

    // INSTANCE DATA
    union {
        bsls::ObjectBuffer< bool >          d_selection1;
        bsls::ObjectBuffer< bsl::string >   d_selection2;
        Choice1                           *d_selection3;
        bsls::ObjectBuffer< unsigned int >  d_selection4;
    };

    int                                    d_selectionId;
    bslma::Allocator                       *d_allocator_p;

  public:
    // TYPES
    enum {
        e_SELECTION_ID_UNDEFINED  = -1

      , e_SELECTION_ID_SELECTION1 = 0
      , e_SELECTION_ID_SELECTION2 = 1
      , e_SELECTION_ID_SELECTION3 = 2
      , e_SELECTION_ID_SELECTION4 = 3
    };

    enum {
        k_NUM_SELECTIONS = 4
    };

    enum {
        e_SELECTION_INDEX_SELECTION1 = 0
      , e_SELECTION_INDEX_SELECTION2 = 1
      , e_SELECTION_INDEX_SELECTION3 = 2
      , e_SELECTION_INDEX_SELECTION4 = 3
    };

    // CONSTANTS
    static const char CLASS_NAME[];

    static const bdlat_SelectionInfo SELECTION_INFO_ARRAY[];

    // CLASS METHODS
    static const bdlat_SelectionInfo *lookupSelectionInfo(int id);
        // Return selection information for the selection indicated by the
        // specified 'id' if the selection exists, and 0 otherwise.

    static const bdlat_SelectionInfo *lookupSelectionInfo(
                                                    const char *name,
                                                    int         nameLength);
        // Return selection information for the selection indicated by the
        // specified 'name' of the specified 'nameLength' if the selection
        // exists, and 0 otherwise.

    // CREATORS
    explicit Choice2(bslma::Allocator *basicAllocator = 0);
        // Create an object of type 'Choice2' having the default value.  Use
        // the optionally specified 'basicAllocator' to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.

    Choice2(const Choice2& original,
           bslma::Allocator *basicAllocator = 0);
        // Create an object of type 'Choice2' having the value of the specified
        // 'original' object.  Use the optionally specified 'basicAllocator' to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.

    ~Choice2();
        // Destroy this object.

    // MANIPULATORS
    Choice2& operator=(const Choice2& rhs);
        // Assign to this object the value of the specified 'rhs' object.

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

    bsl::string& makeSelection2();
    bsl::string& makeSelection2(const bsl::string& value);
        // Set the value of this object to be a "Selection2" value.  Optionally
        // specify the 'value' of the "Selection2".  If 'value' is not
        // specified, the default "Selection2" value is used.

    Choice1& makeSelection3();
    Choice1& makeSelection3(const Choice1& value);
        // Set the value of this object to be a "Selection3" value.  Optionally
        // specify the 'value' of the "Selection3".  If 'value' is not
        // specified, the default "Selection3" value is used.

    unsigned int& makeSelection4();
    unsigned int& makeSelection4(unsigned int value);
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

    bool& selection1();
        // Return a reference to the modifiable "Selection1" selection of this
        // object if "Selection1" is the current selection.  The behavior is
        // undefined unless "Selection1" is the selection of this object.

    bsl::string& selection2();
        // Return a reference to the modifiable "Selection2" selection of this
        // object if "Selection2" is the current selection.  The behavior is
        // undefined unless "Selection2" is the selection of this object.

    Choice1& selection3();
        // Return a reference to the modifiable "Selection3" selection of this
        // object if "Selection3" is the current selection.  The behavior is
        // undefined unless "Selection3" is the selection of this object.

    unsigned int& selection4();
        // Return a reference to the modifiable "Selection4" selection of this
        // object if "Selection4" is the current selection.  The behavior is
        // undefined unless "Selection4" is the selection of this object.

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

    const bsl::string& selection2() const;
        // Return a reference to the non-modifiable "Selection2" selection of
        // this object if "Selection2" is the current selection.  The behavior
        // is undefined unless "Selection2" is the selection of this object.

    const Choice1& selection3() const;
        // Return a reference to the non-modifiable "Selection3" selection of
        // this object if "Selection3" is the current selection.  The behavior
        // is undefined unless "Selection3" is the selection of this object.

    const unsigned int& selection4() const;
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
bsl::ostream& operator<<(bsl::ostream& stream, const Choice2& rhs);
    // Format the specified 'rhs' to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.

}  // close package namespace

// TRAITS

BDLAT_DECL_CHOICE_WITH_ALLOCATOR_BITWISEMOVEABLE_TRAITS(balb::Choice2)

namespace balb {

                              // ===============
                              // class Sequence4
                              // ===============

class Sequence4 {

    // INSTANCE DATA
    double                                   d_element10;
    bsl::vector<int>                         d_element17;
    bsl::vector<double>                      d_element15;
    bsl::vector<char>                        d_element11;
    bsl::vector<bsl::vector<char> >          d_element16;
    bsl::vector<bool>                        d_element14;
    bsl::vector<bdlt::DatetimeTz>             d_element18;
    bsl::vector<Sequence3>                   d_element1;
    bsl::vector<CustomString>                d_element19;
    bsl::vector<Choice1>                     d_element2;
    bsl::string                              d_element9;
    bdlb::NullableValue<bsl::vector<char> >  d_element3;
    bdlb::NullableValue<bdlt::DatetimeTz>     d_element5;
    bdlb::NullableValue<CustomString>        d_element6;
    int                                      d_element12;
    bdlb::NullableValue<int>                 d_element4;
    bdlb::NullableValue<Enumerated::Value>   d_element7;
    Enumerated::Value                        d_element13;
    bool                                     d_element8;

  public:
    // TYPES
    enum {
        e_ATTRIBUTE_ID_ELEMENT1  = 0
      , e_ATTRIBUTE_ID_ELEMENT2  = 1
      , e_ATTRIBUTE_ID_ELEMENT3  = 2
      , e_ATTRIBUTE_ID_ELEMENT4  = 3
      , e_ATTRIBUTE_ID_ELEMENT5  = 4
      , e_ATTRIBUTE_ID_ELEMENT6  = 5
      , e_ATTRIBUTE_ID_ELEMENT7  = 6
      , e_ATTRIBUTE_ID_ELEMENT8  = 7
      , e_ATTRIBUTE_ID_ELEMENT9  = 8
      , e_ATTRIBUTE_ID_ELEMENT10 = 9
      , e_ATTRIBUTE_ID_ELEMENT11 = 10
      , e_ATTRIBUTE_ID_ELEMENT12 = 11
      , e_ATTRIBUTE_ID_ELEMENT13 = 12
      , e_ATTRIBUTE_ID_ELEMENT14 = 13
      , e_ATTRIBUTE_ID_ELEMENT15 = 14
      , e_ATTRIBUTE_ID_ELEMENT16 = 15
      , e_ATTRIBUTE_ID_ELEMENT17 = 16
      , e_ATTRIBUTE_ID_ELEMENT18 = 17
      , e_ATTRIBUTE_ID_ELEMENT19 = 18
    };

    enum {
        k_NUM_ATTRIBUTES = 19
    };

    enum {
        e_ATTRIBUTE_INDEX_ELEMENT1  = 0
      , e_ATTRIBUTE_INDEX_ELEMENT2  = 1
      , e_ATTRIBUTE_INDEX_ELEMENT3  = 2
      , e_ATTRIBUTE_INDEX_ELEMENT4  = 3
      , e_ATTRIBUTE_INDEX_ELEMENT5  = 4
      , e_ATTRIBUTE_INDEX_ELEMENT6  = 5
      , e_ATTRIBUTE_INDEX_ELEMENT7  = 6
      , e_ATTRIBUTE_INDEX_ELEMENT8  = 7
      , e_ATTRIBUTE_INDEX_ELEMENT9  = 8
      , e_ATTRIBUTE_INDEX_ELEMENT10 = 9
      , e_ATTRIBUTE_INDEX_ELEMENT11 = 10
      , e_ATTRIBUTE_INDEX_ELEMENT12 = 11
      , e_ATTRIBUTE_INDEX_ELEMENT13 = 12
      , e_ATTRIBUTE_INDEX_ELEMENT14 = 13
      , e_ATTRIBUTE_INDEX_ELEMENT15 = 14
      , e_ATTRIBUTE_INDEX_ELEMENT16 = 15
      , e_ATTRIBUTE_INDEX_ELEMENT17 = 16
      , e_ATTRIBUTE_INDEX_ELEMENT18 = 17
      , e_ATTRIBUTE_INDEX_ELEMENT19 = 18
    };

    // CONSTANTS
    static const char CLASS_NAME[];

    static const bdlat_AttributeInfo ATTRIBUTE_INFO_ARRAY[];

  public:
    // CLASS METHODS
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
    explicit Sequence4(bslma::Allocator *basicAllocator = 0);
        // Create an object of type 'Sequence4' having the default value.  Use
        // the optionally specified 'basicAllocator' to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.

    Sequence4(const Sequence4& original,
              bslma::Allocator *basicAllocator = 0);
        // Create an object of type 'Sequence4' having the value of the
        // specified 'original' object.  Use the optionally specified
        // 'basicAllocator' to supply memory.  If 'basicAllocator' is 0, the
        // currently installed default allocator is used.

    ~Sequence4();
        // Destroy this object.

    // MANIPULATORS
    Sequence4& operator=(const Sequence4& rhs);
        // Assign to this object the value of the specified 'rhs' object.

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

    bsl::vector<Sequence3>& element1();
        // Return a reference to the modifiable "Element1" attribute of this
        // object.

    bsl::vector<Choice1>& element2();
        // Return a reference to the modifiable "Element2" attribute of this
        // object.

    bdlb::NullableValue<bsl::vector<char> >& element3();
        // Return a reference to the modifiable "Element3" attribute of this
        // object.

    bdlb::NullableValue<int>& element4();
        // Return a reference to the modifiable "Element4" attribute of this
        // object.

    bdlb::NullableValue<bdlt::DatetimeTz>& element5();
        // Return a reference to the modifiable "Element5" attribute of this
        // object.

    bdlb::NullableValue<CustomString>& element6();
        // Return a reference to the modifiable "Element6" attribute of this
        // object.

    bdlb::NullableValue<Enumerated::Value>& element7();
        // Return a reference to the modifiable "Element7" attribute of this
        // object.

    bool& element8();
        // Return a reference to the modifiable "Element8" attribute of this
        // object.

    bsl::string& element9();
        // Return a reference to the modifiable "Element9" attribute of this
        // object.

    double& element10();
        // Return a reference to the modifiable "Element10" attribute of this
        // object.

    bsl::vector<char>& element11();
        // Return a reference to the modifiable "Element11" attribute of this
        // object.

    int& element12();
        // Return a reference to the modifiable "Element12" attribute of this
        // object.

    Enumerated::Value& element13();
        // Return a reference to the modifiable "Element13" attribute of this
        // object.

    bsl::vector<bool>& element14();
        // Return a reference to the modifiable "Element14" attribute of this
        // object.

    bsl::vector<double>& element15();
        // Return a reference to the modifiable "Element15" attribute of this
        // object.

    bsl::vector<bsl::vector<char> >& element16();
        // Return a reference to the modifiable "Element16" attribute of this
        // object.

    bsl::vector<int>& element17();
        // Return a reference to the modifiable "Element17" attribute of this
        // object.

    bsl::vector<bdlt::DatetimeTz>& element18();
        // Return a reference to the modifiable "Element18" attribute of this
        // object.

    bsl::vector<CustomString>& element19();
        // Return a reference to the modifiable "Element19" attribute of this
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

    const bsl::vector<Sequence3>& element1() const;
        // Return a reference to the non-modifiable "Element1" attribute of
        // this object.

    const bsl::vector<Choice1>& element2() const;
        // Return a reference to the non-modifiable "Element2" attribute of
        // this object.

    const bdlb::NullableValue<bsl::vector<char> >& element3() const;
        // Return a reference to the non-modifiable "Element3" attribute of
        // this object.

    const bdlb::NullableValue<int>& element4() const;
        // Return a reference to the non-modifiable "Element4" attribute of
        // this object.

    const bdlb::NullableValue<bdlt::DatetimeTz>& element5() const;
        // Return a reference to the non-modifiable "Element5" attribute of
        // this object.

    const bdlb::NullableValue<CustomString>& element6() const;
        // Return a reference to the non-modifiable "Element6" attribute of
        // this object.

    const bdlb::NullableValue<Enumerated::Value>& element7() const;
        // Return a reference to the non-modifiable "Element7" attribute of
        // this object.

    bool element8() const;
        // Return a reference to the non-modifiable "Element8" attribute of
        // this object.

    const bsl::string& element9() const;
        // Return a reference to the non-modifiable "Element9" attribute of
        // this object.

    double element10() const;
        // Return a reference to the non-modifiable "Element10" attribute of
        // this object.

    const bsl::vector<char>& element11() const;
        // Return a reference to the non-modifiable "Element11" attribute of
        // this object.

    int element12() const;
        // Return a reference to the non-modifiable "Element12" attribute of
        // this object.

    Enumerated::Value element13() const;
        // Return a reference to the non-modifiable "Element13" attribute of
        // this object.

    const bsl::vector<bool>& element14() const;
        // Return a reference to the non-modifiable "Element14" attribute of
        // this object.

    const bsl::vector<double>& element15() const;
        // Return a reference to the non-modifiable "Element15" attribute of
        // this object.

    const bsl::vector<bsl::vector<char> >& element16() const;
        // Return a reference to the non-modifiable "Element16" attribute of
        // this object.

    const bsl::vector<int>& element17() const;
        // Return a reference to the non-modifiable "Element17" attribute of
        // this object.

    const bsl::vector<bdlt::DatetimeTz>& element18() const;
        // Return a reference to the non-modifiable "Element18" attribute of
        // this object.

    const bsl::vector<CustomString>& element19() const;
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
bsl::ostream& operator<<(bsl::ostream& stream, const Sequence4& rhs);
    // Format the specified 'rhs' to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.

}  // close package namespace

// TRAITS

BDLAT_DECL_SEQUENCE_WITH_ALLOCATOR_BITWISEMOVEABLE_TRAITS(balb::Sequence4)

namespace balb {

                              // ===============
                              // class Sequence1
                              // ===============

class Sequence1 {

    // INSTANCE DATA
    bslma::Allocator                            *d_allocator_p;
    bsl::vector<bdlb::NullableValue<Choice1> >  d_element4;
    bsl::vector<Choice3>                        d_element5;
    bsl::vector<Choice1>                        d_element2;
    bdlb::NullableValue<Choice3>                d_element1;
    Choice2                                    *d_element3;

  public:
    // TYPES
    enum {
        e_ATTRIBUTE_ID_ELEMENT1 = 0
      , e_ATTRIBUTE_ID_ELEMENT2 = 1
      , e_ATTRIBUTE_ID_ELEMENT3 = 2
      , e_ATTRIBUTE_ID_ELEMENT4 = 3
      , e_ATTRIBUTE_ID_ELEMENT5 = 4
    };

    enum {
        k_NUM_ATTRIBUTES = 5
    };

    enum {
        e_ATTRIBUTE_INDEX_ELEMENT1 = 0
      , e_ATTRIBUTE_INDEX_ELEMENT2 = 1
      , e_ATTRIBUTE_INDEX_ELEMENT3 = 2
      , e_ATTRIBUTE_INDEX_ELEMENT4 = 3
      , e_ATTRIBUTE_INDEX_ELEMENT5 = 4
    };

    // CONSTANTS
    static const char CLASS_NAME[];

    static const bdlat_AttributeInfo ATTRIBUTE_INFO_ARRAY[];

  public:
    // CLASS METHODS
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
    explicit Sequence1(bslma::Allocator *basicAllocator = 0);
        // Create an object of type 'Sequence1' having the default value.  Use
        // the optionally specified 'basicAllocator' to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.

    Sequence1(const Sequence1& original,
              bslma::Allocator *basicAllocator = 0);
        // Create an object of type 'Sequence1' having the value of the
        // specified 'original' object.  Use the optionally specified
        // 'basicAllocator' to supply memory.  If 'basicAllocator' is 0, the
        // currently installed default allocator is used.

    ~Sequence1();
        // Destroy this object.

    // MANIPULATORS
    Sequence1& operator=(const Sequence1& rhs);
        // Assign to this object the value of the specified 'rhs' object.

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

    bdlb::NullableValue<Choice3>& element1();
        // Return a reference to the modifiable "Element1" attribute of this
        // object.

    bsl::vector<Choice1>& element2();
        // Return a reference to the modifiable "Element2" attribute of this
        // object.

    Choice2& element3();
        // Return a reference to the modifiable "Element3" attribute of this
        // object.

    bsl::vector<bdlb::NullableValue<Choice1> >& element4();
        // Return a reference to the modifiable "Element4" attribute of this
        // object.

    bsl::vector<Choice3>& element5();
        // Return a reference to the modifiable "Element5" attribute of this
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

    const bdlb::NullableValue<Choice3>& element1() const;
        // Return a reference to the non-modifiable "Element1" attribute of
        // this object.

    const bsl::vector<Choice1>& element2() const;
        // Return a reference to the non-modifiable "Element2" attribute of
        // this object.

    const Choice2& element3() const;
        // Return a reference to the non-modifiable "Element3" attribute of
        // this object.

    const bsl::vector<bdlb::NullableValue<Choice1> >& element4() const;
        // Return a reference to the non-modifiable "Element4" attribute of
        // this object.

    const bsl::vector<Choice3>& element5() const;
        // Return a reference to the non-modifiable "Element5" attribute of
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
bsl::ostream& operator<<(bsl::ostream& stream, const Sequence1& rhs);
    // Format the specified 'rhs' to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.

}  // close package namespace

// TRAITS

BDLAT_DECL_SEQUENCE_WITH_ALLOCATOR_BITWISEMOVEABLE_TRAITS(balb::Sequence1)

namespace balb {

                              // ===============
                              // class Sequence2
                              // ===============

class Sequence2 {

    // INSTANCE DATA
    bdlb::NullableValue<double>   d_element5;
    bdlb::NullableValue<Choice1>  d_element4;
    bdlt::DatetimeTz               d_element3;
    CustomString                  d_element1;
    unsigned char                 d_element2;

  public:
    // TYPES
    enum {
        e_ATTRIBUTE_ID_ELEMENT1 = 0
      , e_ATTRIBUTE_ID_ELEMENT2 = 1
      , e_ATTRIBUTE_ID_ELEMENT3 = 2
      , e_ATTRIBUTE_ID_ELEMENT4 = 3
      , e_ATTRIBUTE_ID_ELEMENT5 = 4
    };

    enum {
        k_NUM_ATTRIBUTES = 5
    };

    enum {
        e_ATTRIBUTE_INDEX_ELEMENT1 = 0
      , e_ATTRIBUTE_INDEX_ELEMENT2 = 1
      , e_ATTRIBUTE_INDEX_ELEMENT3 = 2
      , e_ATTRIBUTE_INDEX_ELEMENT4 = 3
      , e_ATTRIBUTE_INDEX_ELEMENT5 = 4
    };

    // CONSTANTS
    static const char CLASS_NAME[];

    static const bdlat_AttributeInfo ATTRIBUTE_INFO_ARRAY[];

  public:
    // CLASS METHODS
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
    explicit Sequence2(bslma::Allocator *basicAllocator = 0);
        // Create an object of type 'Sequence2' having the default value.  Use
        // the optionally specified 'basicAllocator' to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.

    Sequence2(const Sequence2& original,
              bslma::Allocator *basicAllocator = 0);
        // Create an object of type 'Sequence2' having the value of the
        // specified 'original' object.  Use the optionally specified
        // 'basicAllocator' to supply memory.  If 'basicAllocator' is 0, the
        // currently installed default allocator is used.

    ~Sequence2();
        // Destroy this object.

    // MANIPULATORS
    Sequence2& operator=(const Sequence2& rhs);
        // Assign to this object the value of the specified 'rhs' object.

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

    bdlt::DatetimeTz& element3();
        // Return a reference to the modifiable "Element3" attribute of this
        // object.

    bdlb::NullableValue<Choice1>& element4();
        // Return a reference to the modifiable "Element4" attribute of this
        // object.

    bdlb::NullableValue<double>& element5();
        // Return a reference to the modifiable "Element5" attribute of this
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

    const bdlt::DatetimeTz& element3() const;
        // Return a reference to the non-modifiable "Element3" attribute of
        // this object.

    const bdlb::NullableValue<Choice1>& element4() const;
        // Return a reference to the non-modifiable "Element4" attribute of
        // this object.

    const bdlb::NullableValue<double>& element5() const;
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
bsl::ostream& operator<<(bsl::ostream& stream, const Sequence2& rhs);
    // Format the specified 'rhs' to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.

}  // close package namespace

// TRAITS

BDLAT_DECL_SEQUENCE_WITH_ALLOCATOR_BITWISEMOVEABLE_TRAITS(balb::Sequence2)

namespace balb {

                     // ==================================
                     // class SequenceWithAnonymityChoice2
                     // ==================================

class SequenceWithAnonymityChoice2 {

    // INSTANCE DATA
    union {
        Sequence4             *d_selection7;
        Choice2               *d_selection8;
    };

    int                        d_selectionId;
    bslma::Allocator           *d_allocator_p;

  public:
    // TYPES
    enum {
        e_SELECTION_ID_UNDEFINED  = -1

      , e_SELECTION_ID_SELECTION7 = 0
      , e_SELECTION_ID_SELECTION8 = 1
    };

    enum {
        k_NUM_SELECTIONS = 2
    };

    enum {
        e_SELECTION_INDEX_SELECTION7 = 0
      , e_SELECTION_INDEX_SELECTION8 = 1
    };

    // CONSTANTS
    static const char CLASS_NAME[];

    static const bdlat_SelectionInfo SELECTION_INFO_ARRAY[];

    // CLASS METHODS
    static const bdlat_SelectionInfo *lookupSelectionInfo(int id);
        // Return selection information for the selection indicated by the
        // specified 'id' if the selection exists, and 0 otherwise.

    static const bdlat_SelectionInfo *lookupSelectionInfo(
                                                    const char *name,
                                                    int         nameLength);
        // Return selection information for the selection indicated by the
        // specified 'name' of the specified 'nameLength' if the selection
        // exists, and 0 otherwise.

    // CREATORS
    explicit SequenceWithAnonymityChoice2(bslma::Allocator *basicAllocator = 0);
        // Create an object of type 'SequenceWithAnonymityChoice2' having the
        // default value.  Use the optionally specified 'basicAllocator' to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.

    SequenceWithAnonymityChoice2(const SequenceWithAnonymityChoice2& original,
                                bslma::Allocator *basicAllocator = 0);
        // Create an object of type 'SequenceWithAnonymityChoice2' having the
        // value of the specified 'original' object.  Use the optionally
        // specified 'basicAllocator' to supply memory.  If 'basicAllocator' is
        // 0, the currently installed default allocator is used.

    ~SequenceWithAnonymityChoice2();
        // Destroy this object.

    // MANIPULATORS
    SequenceWithAnonymityChoice2& operator=(const SequenceWithAnonymityChoice2& rhs);
        // Assign to this object the value of the specified 'rhs' object.

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

    Sequence4& makeSelection7();
    Sequence4& makeSelection7(const Sequence4& value);
        // Set the value of this object to be a "Selection7" value.  Optionally
        // specify the 'value' of the "Selection7".  If 'value' is not
        // specified, the default "Selection7" value is used.

    Choice2& makeSelection8();
    Choice2& makeSelection8(const Choice2& value);
        // Set the value of this object to be a "Selection8" value.  Optionally
        // specify the 'value' of the "Selection8".  If 'value' is not
        // specified, the default "Selection8" value is used.

    template<class MANIPULATOR>
    int manipulateSelection(MANIPULATOR& manipulator);
        // Invoke the specified 'manipulator' on the address of the modifiable
        // selection, supplying 'manipulator' with the corresponding selection
        // information structure.  Return the value returned from the
        // invocation of 'manipulator' if this object has a defined selection,
        // and -1 otherwise.

    Sequence4& selection7();
        // Return a reference to the modifiable "Selection7" selection of this
        // object if "Selection7" is the current selection.  The behavior is
        // undefined unless "Selection7" is the selection of this object.

    Choice2& selection8();
        // Return a reference to the modifiable "Selection8" selection of this
        // object if "Selection8" is the current selection.  The behavior is
        // undefined unless "Selection8" is the selection of this object.

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

    int selectionId() const;
        // Return the id of the current selection if the selection is defined,
        // and -1 otherwise.

    template<class ACCESSOR>
    int accessSelection(ACCESSOR& accessor) const;
        // Invoke the specified 'accessor' on the non-modifiable selection,
        // supplying 'accessor' with the corresponding selection information
        // structure.  Return the value returned from the invocation of
        // 'accessor' if this object has a defined selection, and -1 otherwise.

    const Sequence4& selection7() const;
        // Return a reference to the non-modifiable "Selection7" selection of
        // this object if "Selection7" is the current selection.  The behavior
        // is undefined unless "Selection7" is the selection of this object.

    const Choice2& selection8() const;
        // Return a reference to the non-modifiable "Selection8" selection of
        // this object if "Selection8" is the current selection.  The behavior
        // is undefined unless "Selection8" is the selection of this object.

    bool isSelection7Value() const;
        // Return 'true' if the value of this object is a "Selection7" value,
        // and return 'false' otherwise.

    bool isSelection8Value() const;
        // Return 'true' if the value of this object is a "Selection8" value,
        // and return 'false' otherwise.

    bool isUndefinedValue() const;
        // Return 'true' if the value of this object is undefined, and 'false'
        // otherwise.

    const char *selectionName() const;
        // Return the symbolic name of the current selection of this object.
};

// FREE OPERATORS
inline
bool operator==(const SequenceWithAnonymityChoice2& lhs, const SequenceWithAnonymityChoice2& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects have the same
    // value, and 'false' otherwise.  Two 'SequenceWithAnonymityChoice2' objects have the same
    // value if either the selections in both objects have the same ids and
    // the same values, or both selections are undefined.

inline
bool operator!=(const SequenceWithAnonymityChoice2& lhs, const SequenceWithAnonymityChoice2& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects do not have the
    // same values, as determined by 'operator==', and 'false' otherwise.

inline
bsl::ostream& operator<<(bsl::ostream& stream, const SequenceWithAnonymityChoice2& rhs);
    // Format the specified 'rhs' to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.

}  // close package namespace

// TRAITS

BDLAT_DECL_CHOICE_WITH_ALLOCATOR_BITWISEMOVEABLE_TRAITS(balb::SequenceWithAnonymityChoice2)

namespace balb {

                        // ===========================
                        // class SequenceWithAnonymity
                        // ===========================

class SequenceWithAnonymity {

    // INSTANCE DATA
    bdlb::NullableValue<SequenceWithAnonymityChoice2>  d_choice2;
    SequenceWithAnonymityChoice1                       d_choice1;
    SequenceWithAnonymityChoice                        d_choice;
    Sequence6                                          d_element4;

  public:
    // TYPES
    enum {
        e_ATTRIBUTE_ID_CHOICE   = 0
      , e_ATTRIBUTE_ID_CHOICE1  = 1
      , e_ATTRIBUTE_ID_CHOICE2  = 2
      , e_ATTRIBUTE_ID_ELEMENT4 = 3
    };

    enum {
        k_NUM_ATTRIBUTES = 4
    };

    enum {
        e_ATTRIBUTE_INDEX_CHOICE   = 0
      , e_ATTRIBUTE_INDEX_CHOICE1  = 1
      , e_ATTRIBUTE_INDEX_CHOICE2  = 2
      , e_ATTRIBUTE_INDEX_ELEMENT4 = 3
    };

    // CONSTANTS
    static const char CLASS_NAME[];

    static const bdlat_AttributeInfo ATTRIBUTE_INFO_ARRAY[];

  public:
    // CLASS METHODS
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
    explicit SequenceWithAnonymity(bslma::Allocator *basicAllocator = 0);
        // Create an object of type 'SequenceWithAnonymity' having the default
        // value.  Use the optionally specified 'basicAllocator' to supply
        // memory.  If 'basicAllocator' is 0, the currently installed default
        // allocator is used.

    SequenceWithAnonymity(const SequenceWithAnonymity& original,
                          bslma::Allocator *basicAllocator = 0);
        // Create an object of type 'SequenceWithAnonymity' having the value of
        // the specified 'original' object.  Use the optionally specified
        // 'basicAllocator' to supply memory.  If 'basicAllocator' is 0, the
        // currently installed default allocator is used.

    ~SequenceWithAnonymity();
        // Destroy this object.

    // MANIPULATORS
    SequenceWithAnonymity& operator=(const SequenceWithAnonymity& rhs);
        // Assign to this object the value of the specified 'rhs' object.

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

    SequenceWithAnonymityChoice& choice();
        // Return a reference to the modifiable "Choice" attribute of this
        // object.

    SequenceWithAnonymityChoice1& choice1();
        // Return a reference to the modifiable "Choice1" attribute of this
        // object.

    bdlb::NullableValue<SequenceWithAnonymityChoice2>& choice2();
        // Return a reference to the modifiable "Choice2" attribute of this
        // object.

    Sequence6& element4();
        // Return a reference to the modifiable "Element4" attribute of this
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

    const SequenceWithAnonymityChoice& choice() const;
        // Return a reference to the non-modifiable "Choice" attribute of this
        // object.

    const SequenceWithAnonymityChoice1& choice1() const;
        // Return a reference to the non-modifiable "Choice1" attribute of this
        // object.

    const bdlb::NullableValue<SequenceWithAnonymityChoice2>& choice2() const;
        // Return a reference to the non-modifiable "Choice2" attribute of this
        // object.

    const Sequence6& element4() const;
        // Return a reference to the non-modifiable "Element4" attribute of
        // this object.
};

// FREE OPERATORS
inline
bool operator==(const SequenceWithAnonymity& lhs, const SequenceWithAnonymity& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
    // the same value, and 'false' otherwise.  Two attribute objects have the
    // same value if each respective attribute has the same value.

inline
bool operator!=(const SequenceWithAnonymity& lhs, const SequenceWithAnonymity& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
    // have the same value, and 'false' otherwise.  Two attribute objects do
    // not have the same value if one or more respective attributes differ in
    // values.

inline
bsl::ostream& operator<<(bsl::ostream& stream, const SequenceWithAnonymity& rhs);
    // Format the specified 'rhs' to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.

}  // close package namespace

// TRAITS

BDLAT_DECL_SEQUENCE_WITH_ALLOCATOR_BITWISEMOVEABLE_TRAITS(balb::SequenceWithAnonymity)

namespace balb {

                          // ========================
                          // class FeatureTestMessage
                          // ========================

class FeatureTestMessage {

    // INSTANCE DATA
    union {
        bsls::ObjectBuffer< Sequence1 >              d_selection1;
        bsls::ObjectBuffer< bsl::vector<char> >      d_selection2;
        bsls::ObjectBuffer< Sequence2 >              d_selection3;
        Sequence3                                   *d_selection4;
        bsls::ObjectBuffer< bdlt::DatetimeTz >        d_selection5;
        bsls::ObjectBuffer< CustomString >           d_selection6;
        bsls::ObjectBuffer< Enumerated::Value >      d_selection7;
        bsls::ObjectBuffer< Choice3 >                d_selection8;
        bsls::ObjectBuffer< VoidSequence >           d_selection9;
        bsls::ObjectBuffer< UnsignedSequence >       d_selection10;
        bsls::ObjectBuffer< SequenceWithAnonymity >  d_selection11;
    };

    int                                              d_selectionId;
    bslma::Allocator                                *d_allocator_p;

  public:
    // TYPES
    enum {
        e_SELECTION_ID_UNDEFINED   = -1

      , e_SELECTION_ID_SELECTION1  = 0
      , e_SELECTION_ID_SELECTION2  = 1
      , e_SELECTION_ID_SELECTION3  = 2
      , e_SELECTION_ID_SELECTION4  = 3
      , e_SELECTION_ID_SELECTION5  = 4
      , e_SELECTION_ID_SELECTION6  = 5
      , e_SELECTION_ID_SELECTION7  = 6
      , e_SELECTION_ID_SELECTION8  = 7
      , e_SELECTION_ID_SELECTION9  = 8
      , e_SELECTION_ID_SELECTION10 = 9
      , e_SELECTION_ID_SELECTION11 = 10
    };

    enum {
        k_NUM_SELECTIONS = 11
    };

    enum {
        e_SELECTION_INDEX_SELECTION1  = 0
      , e_SELECTION_INDEX_SELECTION2  = 1
      , e_SELECTION_INDEX_SELECTION3  = 2
      , e_SELECTION_INDEX_SELECTION4  = 3
      , e_SELECTION_INDEX_SELECTION5  = 4
      , e_SELECTION_INDEX_SELECTION6  = 5
      , e_SELECTION_INDEX_SELECTION7  = 6
      , e_SELECTION_INDEX_SELECTION8  = 7
      , e_SELECTION_INDEX_SELECTION9  = 8
      , e_SELECTION_INDEX_SELECTION10 = 9
      , e_SELECTION_INDEX_SELECTION11 = 10
    };

    // CONSTANTS
    static const char CLASS_NAME[];

    static const bdlat_SelectionInfo SELECTION_INFO_ARRAY[];

    // CLASS METHODS
    static const bdlat_SelectionInfo *lookupSelectionInfo(int id);
        // Return selection information for the selection indicated by the
        // specified 'id' if the selection exists, and 0 otherwise.

    static const bdlat_SelectionInfo *lookupSelectionInfo(
                                                    const char *name,
                                                    int         nameLength);
        // Return selection information for the selection indicated by the
        // specified 'name' of the specified 'nameLength' if the selection
        // exists, and 0 otherwise.

    // CREATORS
    explicit FeatureTestMessage(bslma::Allocator *basicAllocator = 0);
        // Create an object of type 'FeatureTestMessage' having the default
        // value.  Use the optionally specified 'basicAllocator' to supply
        // memory.  If 'basicAllocator' is 0, the currently installed default
        // allocator is used.

    FeatureTestMessage(const FeatureTestMessage&  original,
                       bslma::Allocator          *basicAllocator = 0);
        // Create an object of type 'FeatureTestMessage' having the value of
        // the specified 'original' object.  Use the optionally specified
        // 'basicAllocator' to supply memory.  If 'basicAllocator' is 0, the
        // currently installed default allocator is used.

    ~FeatureTestMessage();
        // Destroy this object.

    // MANIPULATORS
    FeatureTestMessage& operator=(const FeatureTestMessage& rhs);
        // Assign to this object the value of the specified 'rhs' object.

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

    bsl::vector<char>& makeSelection2();
    bsl::vector<char>& makeSelection2(const bsl::vector<char>& value);
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

    bdlt::DatetimeTz& makeSelection5();
    bdlt::DatetimeTz& makeSelection5(const bdlt::DatetimeTz& value);
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

    SequenceWithAnonymity& makeSelection11();
    SequenceWithAnonymity& makeSelection11(const SequenceWithAnonymity& value);
        // Set the value of this object to be a "Selection11" value.
        // Optionally specify the 'value' of the "Selection11".  If 'value' is
        // not specified, the default "Selection11" value is used.

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

    bsl::vector<char>& selection2();
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

    bdlt::DatetimeTz& selection5();
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

    SequenceWithAnonymity& selection11();
        // Return a reference to the modifiable "Selection11" selection of this
        // object if "Selection11" is the current selection.  The behavior is
        // undefined unless "Selection11" is the selection of this object.

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

    const bsl::vector<char>& selection2() const;
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

    const bdlt::DatetimeTz& selection5() const;
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

    const SequenceWithAnonymity& selection11() const;
        // Return a reference to the non-modifiable "Selection11" selection of
        // this object if "Selection11" is the current selection.  The behavior
        // is undefined unless "Selection11" is the selection of this object.

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

    bool isSelection11Value() const;
        // Return 'true' if the value of this object is a "Selection11" value,
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
bsl::ostream& operator<<(bsl::ostream& stream, const FeatureTestMessage& rhs);
    // Format the specified 'rhs' to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.

}  // close package namespace

// TRAITS

BDLAT_DECL_CHOICE_WITH_ALLOCATOR_BITWISEMOVEABLE_TRAITS(balb::FeatureTestMessage)

namespace balb {

                               // =============
                               // class Request
                               // =============

class Request {

    // INSTANCE DATA
    union {
        bsls::ObjectBuffer< SimpleRequest >      d_simpleRequest;
        bsls::ObjectBuffer< FeatureTestMessage > d_featureRequest;
    };

    int                                          d_selectionId;
    bslma::Allocator                            *d_allocator_p;

  public:
    // TYPES
    enum {
        e_SELECTION_ID_UNDEFINED       = -1

      , e_SELECTION_ID_SIMPLE_REQUEST  = 0
      , e_SELECTION_ID_FEATURE_REQUEST = 1
    };

    enum {
        k_NUM_SELECTIONS = 2
    };

    enum {
        e_SELECTION_INDEX_SIMPLE_REQUEST  = 0
      , e_SELECTION_INDEX_FEATURE_REQUEST = 1
    };

    // CONSTANTS
    static const char CLASS_NAME[];

    static const bdlat_SelectionInfo SELECTION_INFO_ARRAY[];

    // CLASS METHODS
    static const bdlat_SelectionInfo *lookupSelectionInfo(int id);
        // Return selection information for the selection indicated by the
        // specified 'id' if the selection exists, and 0 otherwise.

    static const bdlat_SelectionInfo *lookupSelectionInfo(
                                                    const char *name,
                                                    int         nameLength);
        // Return selection information for the selection indicated by the
        // specified 'name' of the specified 'nameLength' if the selection
        // exists, and 0 otherwise.

    // CREATORS
    explicit Request(bslma::Allocator *basicAllocator = 0);
        // Create an object of type 'Request' having the default value.  Use
        // the optionally specified 'basicAllocator' to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.

    Request(const Request&    original,
            bslma::Allocator *basicAllocator = 0);
        // Create an object of type 'Request' having the value of the specified
        // 'original' object.  Use the optionally specified 'basicAllocator' to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.

    ~Request();
        // Destroy this object.

    // MANIPULATORS
    Request& operator=(const Request& rhs);
        // Assign to this object the value of the specified 'rhs' object.

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
bsl::ostream& operator<<(bsl::ostream& stream, const Request& rhs);
    // Format the specified 'rhs' to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.

}  // close package namespace

// TRAITS

BDLAT_DECL_CHOICE_WITH_ALLOCATOR_BITWISEMOVEABLE_TRAITS(balb::Request)

namespace balb {

                               // ==============
                               // class Response
                               // ==============

class Response {

    // INSTANCE DATA
    union {
        bsls::ObjectBuffer< bsl::string >        d_responseData;
        bsls::ObjectBuffer< FeatureTestMessage > d_featureResponse;
    };

    int                                          d_selectionId;
    bslma::Allocator                            *d_allocator_p;

  public:
    // TYPES
    enum {
        e_SELECTION_ID_UNDEFINED        = -1

      , e_SELECTION_ID_RESPONSE_DATA    = 0
      , e_SELECTION_ID_FEATURE_RESPONSE = 1
    };

    enum {
        k_NUM_SELECTIONS = 2
    };

    enum {
        e_SELECTION_INDEX_RESPONSE_DATA    = 0
      , e_SELECTION_INDEX_FEATURE_RESPONSE = 1
    };

    // CONSTANTS
    static const char CLASS_NAME[];

    static const bdlat_SelectionInfo SELECTION_INFO_ARRAY[];

    // CLASS METHODS
    static const bdlat_SelectionInfo *lookupSelectionInfo(int id);
        // Return selection information for the selection indicated by the
        // specified 'id' if the selection exists, and 0 otherwise.

    static const bdlat_SelectionInfo *lookupSelectionInfo(
                                                    const char *name,
                                                    int         nameLength);
        // Return selection information for the selection indicated by the
        // specified 'name' of the specified 'nameLength' if the selection
        // exists, and 0 otherwise.

    // CREATORS
    explicit Response(bslma::Allocator *basicAllocator = 0);
        // Create an object of type 'Response' having the default value.  Use
        // the optionally specified 'basicAllocator' to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.

    Response(const Response&   original,
             bslma::Allocator *basicAllocator = 0);
        // Create an object of type 'Response' having the value of the
        // specified 'original' object.  Use the optionally specified
        // 'basicAllocator' to supply memory.  If 'basicAllocator' is 0, the
        // currently installed default allocator is used.

    ~Response();
        // Destroy this object.

    // MANIPULATORS
    Response& operator=(const Response& rhs);
        // Assign to this object the value of the specified 'rhs' object.

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

    bsl::string& makeResponseData();
    bsl::string& makeResponseData(const bsl::string& value);
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

    bsl::string& responseData();
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

    int selectionId() const;
        // Return the id of the current selection if the selection is defined,
        // and -1 otherwise.

    template<class ACCESSOR>
    int accessSelection(ACCESSOR& accessor) const;
        // Invoke the specified 'accessor' on the non-modifiable selection,
        // supplying 'accessor' with the corresponding selection information
        // structure.  Return the value returned from the invocation of
        // 'accessor' if this object has a defined selection, and -1 otherwise.

    const bsl::string& responseData() const;
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
bsl::ostream& operator<<(bsl::ostream& stream, const Response& rhs);
    // Format the specified 'rhs' to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.

}  // close package namespace

// TRAITS

BDLAT_DECL_CHOICE_WITH_ALLOCATOR_BITWISEMOVEABLE_TRAITS(balb::Response)

namespace balb {

                             // ==================
                             // class Testmessages
                             // ==================

struct Testmessages {
    // This class serves as a place holder to reserve a type having the same
    // name as this component.  Doing so ensures that such a type cannot be
    // defined outside of this component in the current namespace.
};

}  // close package namespace

// ============================================================================
//                        INLINE FUNCTION DEFINITIONS
// ============================================================================

namespace balb {

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

inline
void CustomInt::reset()
{
    bdlat_ValueTypeFunctions::reset(&d_value);
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
inline
bsl::ostream& CustomInt::print(bsl::ostream& stream,
                               int           level,
                               int           spacesPerLevel) const
{
    return bdlb::PrintMethods::print(stream, d_value, level, spacesPerLevel);
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
CustomString::CustomString(bslma::Allocator *basicAllocator)
: d_value(basicAllocator)
{
}

inline
CustomString::CustomString(const CustomString&  original,
                           bslma::Allocator    *basicAllocator)
: d_value(original.d_value, basicAllocator)
{
}

inline
CustomString::CustomString(const bsl::string&  value,
                           bslma::Allocator   *basicAllocator)
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

inline
void CustomString::reset()
{
    bdlat_ValueTypeFunctions::reset(&d_value);
}

inline
int CustomString::fromString(const bsl::string& value)
{
    int ret = checkRestrictions(value);
    if (0 == ret) {
        d_value = value;
    }

    return ret;
}

// ACCESSORS
inline
bsl::ostream& CustomString::print(bsl::ostream& stream,
                                 int           level,
                                 int           spacesPerLevel) const
{
    return bdlb::PrintMethods::print(stream, d_value, level, spacesPerLevel);
}

inline
const bsl::string& CustomString::toString() const
{
    return d_value;
}



                              // ----------------
                              // class Enumerated
                              // ----------------

// CLASS METHODS
inline
int Enumerated::fromString(Value *result, const bsl::string& string)
{
    return fromString(result, string.c_str(), string.length());
}

inline
bsl::ostream& Enumerated::print(bsl::ostream&      stream,
                                 Enumerated::Value value)
{
    return stream << toString(value);
}



                     // ----------------------------------
                     // class SequenceWithAnonymityChoice1
                     // ----------------------------------

// CREATORS
inline
SequenceWithAnonymityChoice1::SequenceWithAnonymityChoice1(bslma::Allocator *basicAllocator)
: d_selectionId(e_SELECTION_ID_UNDEFINED)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

inline
SequenceWithAnonymityChoice1::~SequenceWithAnonymityChoice1()
{
    reset();
}

// MANIPULATORS
template <class MANIPULATOR>
int SequenceWithAnonymityChoice1::manipulateSelection(MANIPULATOR& manipulator)
{
    switch (d_selectionId) {
      case SequenceWithAnonymityChoice1::e_SELECTION_ID_SELECTION5:
        return manipulator(&d_selection5.object(),
                SELECTION_INFO_ARRAY[e_SELECTION_INDEX_SELECTION5]);
      case SequenceWithAnonymityChoice1::e_SELECTION_ID_SELECTION6:
        return manipulator(&d_selection6.object(),
                SELECTION_INFO_ARRAY[e_SELECTION_INDEX_SELECTION6]);
      default:
        BSLS_ASSERT(SequenceWithAnonymityChoice1::e_SELECTION_ID_UNDEFINED == d_selectionId);
        return -1;                                                    // RETURN
    }
}

inline
bool& SequenceWithAnonymityChoice1::selection5()
{
    BSLS_ASSERT(e_SELECTION_ID_SELECTION5 == d_selectionId);
    return d_selection5.object();
}

inline
bsl::string& SequenceWithAnonymityChoice1::selection6()
{
    BSLS_ASSERT(e_SELECTION_ID_SELECTION6 == d_selectionId);
    return d_selection6.object();
}

// ACCESSORS
inline
int SequenceWithAnonymityChoice1::selectionId() const
{
    return d_selectionId;
}

template <class ACCESSOR>
int SequenceWithAnonymityChoice1::accessSelection(ACCESSOR& accessor) const
{
    switch (d_selectionId) {
      case e_SELECTION_ID_SELECTION5:
        return accessor(d_selection5.object(),
                SELECTION_INFO_ARRAY[e_SELECTION_INDEX_SELECTION5]);
      case e_SELECTION_ID_SELECTION6:
        return accessor(d_selection6.object(),
                SELECTION_INFO_ARRAY[e_SELECTION_INDEX_SELECTION6]);
      default:
        BSLS_ASSERT(e_SELECTION_ID_UNDEFINED == d_selectionId);
        return -1;                                                    // RETURN
    }
}

inline
const bool& SequenceWithAnonymityChoice1::selection5() const
{
    BSLS_ASSERT(e_SELECTION_ID_SELECTION5 == d_selectionId);
    return d_selection5.object();
}

inline
const bsl::string& SequenceWithAnonymityChoice1::selection6() const
{
    BSLS_ASSERT(e_SELECTION_ID_SELECTION6 == d_selectionId);
    return d_selection6.object();
}

inline
bool SequenceWithAnonymityChoice1::isSelection5Value() const
{
    return e_SELECTION_ID_SELECTION5 == d_selectionId;
}

inline
bool SequenceWithAnonymityChoice1::isSelection6Value() const
{
    return e_SELECTION_ID_SELECTION6 == d_selectionId;
}

inline
bool SequenceWithAnonymityChoice1::isUndefinedValue() const
{
    return e_SELECTION_ID_UNDEFINED == d_selectionId;
}


                            // -------------------
                            // class SimpleRequest
                            // -------------------

// MANIPULATORS
template <class MANIPULATOR>
int SimpleRequest::manipulateAttributes(MANIPULATOR& manipulator)
{
    int ret;

    ret = manipulator(&d_data, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_DATA]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = manipulator(&d_responseLength, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_RESPONSE_LENGTH]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    return ret;
}

template <class MANIPULATOR>
int SimpleRequest::manipulateAttribute(MANIPULATOR& manipulator, int id)
{
    enum { k_NOT_FOUND = -1 };

    switch (id) {
      case e_ATTRIBUTE_ID_DATA: {
        return manipulator(&d_data, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_DATA]);
                                                                      // RETURN
      } break;
      case e_ATTRIBUTE_ID_RESPONSE_LENGTH: {
        return manipulator(&d_responseLength, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_RESPONSE_LENGTH]);
                                                                      // RETURN
      } break;
      default:
        return k_NOT_FOUND;                                           // RETURN
    }
}

template <class MANIPULATOR>
int SimpleRequest::manipulateAttribute(
        MANIPULATOR&  manipulator,
        const char   *name,
        int           nameLength)
{
    enum { k_NOT_FOUND = -1 };

    const bdlat_AttributeInfo *attributeInfo =
           lookupAttributeInfo(name, nameLength);
    if (0 == attributeInfo) {
        return k_NOT_FOUND;                                           // RETURN
    }

    return manipulateAttribute(manipulator, attributeInfo->d_id);
}

inline
bsl::string& SimpleRequest::data()
{
    return d_data;
}

inline
int& SimpleRequest::responseLength()
{
    return d_responseLength;
}

// ACCESSORS
template <class ACCESSOR>
int SimpleRequest::accessAttributes(ACCESSOR& accessor) const
{
    int ret;

    ret = accessor(d_data, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_DATA]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = accessor(d_responseLength, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_RESPONSE_LENGTH]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    return ret;
}

template <class ACCESSOR>
int SimpleRequest::accessAttribute(ACCESSOR& accessor, int id) const
{
    enum { k_NOT_FOUND = -1 };

    switch (id) {
      case e_ATTRIBUTE_ID_DATA: {
        return accessor(d_data, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_DATA]);
                                                                      // RETURN
      } break;
      case e_ATTRIBUTE_ID_RESPONSE_LENGTH: {
        return accessor(d_responseLength, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_RESPONSE_LENGTH]);
                                                                      // RETURN
      } break;
      default:
        return k_NOT_FOUND;                                           // RETURN
    }
}

template <class ACCESSOR>
int SimpleRequest::accessAttribute(
        ACCESSOR&   accessor,
        const char *name,
        int         nameLength) const
{
    enum { k_NOT_FOUND = -1 };

    const bdlat_AttributeInfo *attributeInfo =
          lookupAttributeInfo(name, nameLength);
    if (0 == attributeInfo) {
       return k_NOT_FOUND;                                            // RETURN
    }

    return accessAttribute(accessor, attributeInfo->d_id);
}

inline
const bsl::string& SimpleRequest::data() const
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

// MANIPULATORS
template <class MANIPULATOR>
int UnsignedSequence::manipulateAttributes(MANIPULATOR& manipulator)
{
    int ret;

    ret = manipulator(&d_element1, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT1]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = manipulator(&d_element2, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT2]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = manipulator(&d_element3, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT3]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    return ret;
}

template <class MANIPULATOR>
int UnsignedSequence::manipulateAttribute(MANIPULATOR& manipulator, int id)
{
    enum { k_NOT_FOUND = -1 };

    switch (id) {
      case e_ATTRIBUTE_ID_ELEMENT1: {
        return manipulator(&d_element1, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT1]);
                                                                      // RETURN
      } break;
      case e_ATTRIBUTE_ID_ELEMENT2: {
        return manipulator(&d_element2, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT2]);
                                                                      // RETURN
      } break;
      case e_ATTRIBUTE_ID_ELEMENT3: {
        return manipulator(&d_element3, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT3]);
                                                                      // RETURN
      } break;
      default:
        return k_NOT_FOUND;                                           // RETURN
    }
}

template <class MANIPULATOR>
int UnsignedSequence::manipulateAttribute(
        MANIPULATOR&  manipulator,
        const char   *name,
        int           nameLength)
{
    enum { k_NOT_FOUND = -1 };

    const bdlat_AttributeInfo *attributeInfo =
           lookupAttributeInfo(name, nameLength);
    if (0 == attributeInfo) {
        return k_NOT_FOUND;                                           // RETURN
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
bsls::Types::Uint64& UnsignedSequence::element3()
{
    return d_element3;
}

// ACCESSORS
template <class ACCESSOR>
int UnsignedSequence::accessAttributes(ACCESSOR& accessor) const
{
    int ret;

    ret = accessor(d_element1, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT1]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = accessor(d_element2, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT2]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = accessor(d_element3, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT3]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    return ret;
}

template <class ACCESSOR>
int UnsignedSequence::accessAttribute(ACCESSOR& accessor, int id) const
{
    enum { k_NOT_FOUND = -1 };

    switch (id) {
      case e_ATTRIBUTE_ID_ELEMENT1: {
        return accessor(d_element1, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT1]);
                                                                      // RETURN
      } break;
      case e_ATTRIBUTE_ID_ELEMENT2: {
        return accessor(d_element2, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT2]);
                                                                      // RETURN
      } break;
      case e_ATTRIBUTE_ID_ELEMENT3: {
        return accessor(d_element3, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT3]);
                                                                      // RETURN
      } break;
      default:
        return k_NOT_FOUND;                                           // RETURN
    }
}

template <class ACCESSOR>
int UnsignedSequence::accessAttribute(
        ACCESSOR&   accessor,
        const char *name,
        int         nameLength) const
{
    enum { k_NOT_FOUND = -1 };

    const bdlat_AttributeInfo *attributeInfo =
          lookupAttributeInfo(name, nameLength);
    if (0 == attributeInfo) {
       return k_NOT_FOUND;                                            // RETURN
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
bsls::Types::Uint64 UnsignedSequence::element3() const
{
    return d_element3;
}



                             // ------------------
                             // class VoidSequence
                             // ------------------

// MANIPULATORS
template <class MANIPULATOR>
int VoidSequence::manipulateAttributes(MANIPULATOR& /*manipulator*/)
{
    int ret = 0;

    return ret;
}

template <class MANIPULATOR>
int VoidSequence::manipulateAttribute(MANIPULATOR& /*manipulator*/, int id)
{
    enum { k_NOT_FOUND = -1 };

    switch (id) {
      default:
        return k_NOT_FOUND;
    }
}

template <class MANIPULATOR>
int VoidSequence::manipulateAttribute(
        MANIPULATOR&  manipulator,
        const char   *name,
        int           nameLength)
{
    enum { k_NOT_FOUND = -1 };

    const bdlat_AttributeInfo *attributeInfo =
           lookupAttributeInfo(name, nameLength);
    if (0 == attributeInfo) {
        return k_NOT_FOUND;                                           // RETURN
    }

    return manipulateAttribute(manipulator, attributeInfo->d_id);
}

// ACCESSORS
template <class ACCESSOR>
int VoidSequence::accessAttributes(ACCESSOR& /*accessor*/) const
{
    int ret = 0;

    return ret;
}

template <class ACCESSOR>
int VoidSequence::accessAttribute(ACCESSOR& /*accessor*/, int id) const
{
    enum { k_NOT_FOUND = -1 };

    switch (id) {
      default:
        return k_NOT_FOUND;
    }
}

template <class ACCESSOR>
int VoidSequence::accessAttribute(
        ACCESSOR&   accessor,
        const char *name,
        int         nameLength) const
{
    enum { k_NOT_FOUND = -1 };

    const bdlat_AttributeInfo *attributeInfo =
          lookupAttributeInfo(name, nameLength);
    if (0 == attributeInfo) {
       return k_NOT_FOUND;                                            // RETURN
    }

    return accessAttribute(accessor, attributeInfo->d_id);
}



                              // ---------------
                              // class Sequence3
                              // ---------------

// MANIPULATORS
template <class MANIPULATOR>
int Sequence3::manipulateAttributes(MANIPULATOR& manipulator)
{
    int ret;

    ret = manipulator(&d_element1, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT1]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = manipulator(&d_element2, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT2]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = manipulator(&d_element3, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT3]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = manipulator(&d_element4, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT4]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = manipulator(&d_element5, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT5]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = manipulator(&d_element6, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT6]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    return ret;
}

template <class MANIPULATOR>
int Sequence3::manipulateAttribute(MANIPULATOR& manipulator, int id)
{
    enum { k_NOT_FOUND = -1 };

    switch (id) {
      case e_ATTRIBUTE_ID_ELEMENT1: {
        return manipulator(&d_element1, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT1]);
                                                                      // RETURN
      } break;
      case e_ATTRIBUTE_ID_ELEMENT2: {
        return manipulator(&d_element2, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT2]);
                                                                      // RETURN
      } break;
      case e_ATTRIBUTE_ID_ELEMENT3: {
        return manipulator(&d_element3, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT3]);
                                                                      // RETURN
      } break;
      case e_ATTRIBUTE_ID_ELEMENT4: {
        return manipulator(&d_element4, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT4]);
                                                                      // RETURN
      } break;
      case e_ATTRIBUTE_ID_ELEMENT5: {
        return manipulator(&d_element5, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT5]);
                                                                      // RETURN
      } break;
      case e_ATTRIBUTE_ID_ELEMENT6: {
        return manipulator(&d_element6, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT6]);
                                                                      // RETURN
      } break;
      default:
        return k_NOT_FOUND;                                           // RETURN
    }
}

template <class MANIPULATOR>
int Sequence3::manipulateAttribute(
        MANIPULATOR&  manipulator,
        const char   *name,
        int           nameLength)
{
    enum { k_NOT_FOUND = -1 };

    const bdlat_AttributeInfo *attributeInfo =
           lookupAttributeInfo(name, nameLength);
    if (0 == attributeInfo) {
        return k_NOT_FOUND;                                           // RETURN
    }

    return manipulateAttribute(manipulator, attributeInfo->d_id);
}

inline
bsl::vector<Enumerated::Value>& Sequence3::element1()
{
    return d_element1;
}

inline
bsl::vector<bsl::string>& Sequence3::element2()
{
    return d_element2;
}

inline
bdlb::NullableValue<bool>& Sequence3::element3()
{
    return d_element3;
}

inline
bdlb::NullableValue<bsl::string>& Sequence3::element4()
{
    return d_element4;
}

inline
bdlb::NullableAllocatedValue<Sequence5>& Sequence3::element5()
{
    return d_element5;
}

inline
bsl::vector<bdlb::NullableValue<Enumerated::Value> >& Sequence3::element6()
{
    return d_element6;
}

// ACCESSORS
template <class ACCESSOR>
int Sequence3::accessAttributes(ACCESSOR& accessor) const
{
    int ret;

    ret = accessor(d_element1, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT1]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = accessor(d_element2, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT2]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = accessor(d_element3, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT3]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = accessor(d_element4, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT4]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = accessor(d_element5, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT5]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = accessor(d_element6, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT6]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    return ret;
}

template <class ACCESSOR>
int Sequence3::accessAttribute(ACCESSOR& accessor, int id) const
{
    enum { k_NOT_FOUND = -1 };

    switch (id) {
      case e_ATTRIBUTE_ID_ELEMENT1: {
        return accessor(d_element1, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT1]);
                                                                      // RETURN
      } break;
      case e_ATTRIBUTE_ID_ELEMENT2: {
        return accessor(d_element2, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT2]);
                                                                      // RETURN
      } break;
      case e_ATTRIBUTE_ID_ELEMENT3: {
        return accessor(d_element3, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT3]);
                                                                      // RETURN
      } break;
      case e_ATTRIBUTE_ID_ELEMENT4: {
        return accessor(d_element4, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT4]);
                                                                      // RETURN
      } break;
      case e_ATTRIBUTE_ID_ELEMENT5: {
        return accessor(d_element5, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT5]);
                                                                      // RETURN
      } break;
      case e_ATTRIBUTE_ID_ELEMENT6: {
        return accessor(d_element6, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT6]);
                                                                      // RETURN
      } break;
      default:
        return k_NOT_FOUND;                                           // RETURN
    }
}

template <class ACCESSOR>
int Sequence3::accessAttribute(
        ACCESSOR&   accessor,
        const char *name,
        int         nameLength) const
{
    enum { k_NOT_FOUND = -1 };

    const bdlat_AttributeInfo *attributeInfo =
          lookupAttributeInfo(name, nameLength);
    if (0 == attributeInfo) {
       return k_NOT_FOUND;                                            // RETURN
    }

    return accessAttribute(accessor, attributeInfo->d_id);
}

inline
const bsl::vector<Enumerated::Value>& Sequence3::element1() const
{
    return d_element1;
}

inline
const bsl::vector<bsl::string>& Sequence3::element2() const
{
    return d_element2;
}

inline
const bdlb::NullableValue<bool>& Sequence3::element3() const
{
    return d_element3;
}

inline
const bdlb::NullableValue<bsl::string>& Sequence3::element4() const
{
    return d_element4;
}

inline
const bdlb::NullableAllocatedValue<Sequence5>& Sequence3::element5() const
{
    return d_element5;
}

inline
const bsl::vector<bdlb::NullableValue<Enumerated::Value> >& Sequence3::element6() const
{
    return d_element6;
}



                              // ---------------
                              // class Sequence5
                              // ---------------

// MANIPULATORS
template <class MANIPULATOR>
int Sequence5::manipulateAttributes(MANIPULATOR& manipulator)
{
    int ret;

    ret = manipulator(d_element1, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT1]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = manipulator(&d_element2, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT2]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = manipulator(&d_element3, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT3]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = manipulator(&d_element4, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT4]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = manipulator(&d_element5, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT5]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = manipulator(&d_element6, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT6]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = manipulator(&d_element7, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT7]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    return ret;
}

template <class MANIPULATOR>
int Sequence5::manipulateAttribute(MANIPULATOR& manipulator, int id)
{
    enum { k_NOT_FOUND = -1 };

    switch (id) {
      case e_ATTRIBUTE_ID_ELEMENT1: {
        return manipulator(d_element1, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT1]);
                                                                      // RETURN
      } break;
      case e_ATTRIBUTE_ID_ELEMENT2: {
        return manipulator(&d_element2, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT2]);
                                                                      // RETURN
      } break;
      case e_ATTRIBUTE_ID_ELEMENT3: {
        return manipulator(&d_element3, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT3]);
                                                                      // RETURN
      } break;
      case e_ATTRIBUTE_ID_ELEMENT4: {
        return manipulator(&d_element4, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT4]);
                                                                      // RETURN
      } break;
      case e_ATTRIBUTE_ID_ELEMENT5: {
        return manipulator(&d_element5, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT5]);
                                                                      // RETURN
      } break;
      case e_ATTRIBUTE_ID_ELEMENT6: {
        return manipulator(&d_element6, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT6]);
                                                                      // RETURN
      } break;
      case e_ATTRIBUTE_ID_ELEMENT7: {
        return manipulator(&d_element7, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT7]);
                                                                      // RETURN
      } break;
      default:
        return k_NOT_FOUND;                                           // RETURN
    }
}

template <class MANIPULATOR>
int Sequence5::manipulateAttribute(
        MANIPULATOR&  manipulator,
        const char   *name,
        int           nameLength)
{
    enum { k_NOT_FOUND = -1 };

    const bdlat_AttributeInfo *attributeInfo =
           lookupAttributeInfo(name, nameLength);
    if (0 == attributeInfo) {
        return k_NOT_FOUND;                                           // RETURN
    }

    return manipulateAttribute(manipulator, attributeInfo->d_id);
}

inline
Sequence3& Sequence5::element1()
{
    return *d_element1;
}

inline
bsl::vector<bdlb::NullableValue<bool> >& Sequence5::element2()
{
    return d_element2;
}

inline
bsl::vector<bdlb::NullableValue<double> >& Sequence5::element3()
{
    return d_element3;
}

inline
bsl::vector<bdlb::NullableValue<bsl::vector<char> > >& Sequence5::element4()
{
    return d_element4;
}

inline
bsl::vector<bdlb::NullableValue<int> >& Sequence5::element5()
{
    return d_element5;
}

inline
bsl::vector<bdlb::NullableValue<bdlt::DatetimeTz> >& Sequence5::element6()
{
    return d_element6;
}

inline
bsl::vector<bdlb::NullableAllocatedValue<Sequence3> >& Sequence5::element7()
{
    return d_element7;
}

// ACCESSORS
template <class ACCESSOR>
int Sequence5::accessAttributes(ACCESSOR& accessor) const
{
    int ret;

    ret = accessor(*d_element1, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT1]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = accessor(d_element2, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT2]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = accessor(d_element3, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT3]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = accessor(d_element4, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT4]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = accessor(d_element5, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT5]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = accessor(d_element6, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT6]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = accessor(d_element7, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT7]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    return ret;
}

template <class ACCESSOR>
int Sequence5::accessAttribute(ACCESSOR& accessor, int id) const
{
    enum { k_NOT_FOUND = -1 };

    switch (id) {
      case e_ATTRIBUTE_ID_ELEMENT1: {
        return accessor(*d_element1, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT1]);
                                                                      // RETURN
      } break;
      case e_ATTRIBUTE_ID_ELEMENT2: {
        return accessor(d_element2, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT2]);
                                                                      // RETURN
      } break;
      case e_ATTRIBUTE_ID_ELEMENT3: {
        return accessor(d_element3, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT3]);
                                                                      // RETURN
      } break;
      case e_ATTRIBUTE_ID_ELEMENT4: {
        return accessor(d_element4, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT4]);
                                                                      // RETURN
      } break;
      case e_ATTRIBUTE_ID_ELEMENT5: {
        return accessor(d_element5, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT5]);
                                                                      // RETURN
      } break;
      case e_ATTRIBUTE_ID_ELEMENT6: {
        return accessor(d_element6, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT6]);
                                                                      // RETURN
      } break;
      case e_ATTRIBUTE_ID_ELEMENT7: {
        return accessor(d_element7, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT7]);
                                                                      // RETURN
      } break;
      default:
        return k_NOT_FOUND;                                           // RETURN
    }
}

template <class ACCESSOR>
int Sequence5::accessAttribute(
        ACCESSOR&   accessor,
        const char *name,
        int         nameLength) const
{
    enum { k_NOT_FOUND = -1 };

    const bdlat_AttributeInfo *attributeInfo =
          lookupAttributeInfo(name, nameLength);
    if (0 == attributeInfo) {
       return k_NOT_FOUND;                                            // RETURN
    }

    return accessAttribute(accessor, attributeInfo->d_id);
}

inline
const Sequence3& Sequence5::element1() const
{
    return *d_element1;
}

inline
const bsl::vector<bdlb::NullableValue<bool> >& Sequence5::element2() const
{
    return d_element2;
}

inline
const bsl::vector<bdlb::NullableValue<double> >& Sequence5::element3() const
{
    return d_element3;
}

inline
const bsl::vector<bdlb::NullableValue<bsl::vector<char> > >& Sequence5::element4() const
{
    return d_element4;
}

inline
const bsl::vector<bdlb::NullableValue<int> >& Sequence5::element5() const
{
    return d_element5;
}

inline
const bsl::vector<bdlb::NullableValue<bdlt::DatetimeTz> >& Sequence5::element6() const
{
    return d_element6;
}

inline
const bsl::vector<bdlb::NullableAllocatedValue<Sequence3> >& Sequence5::element7() const
{
    return d_element7;
}



                              // ---------------
                              // class Sequence6
                              // ---------------

// MANIPULATORS
template <class MANIPULATOR>
int Sequence6::manipulateAttributes(MANIPULATOR& manipulator)
{
    int ret;

    ret = manipulator(&d_element1, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT1]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = manipulator(&d_element2, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT2]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = manipulator(&d_element3, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT3]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = manipulator(&d_element4, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT4]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = manipulator(&d_element5, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT5]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = manipulator(&d_element6, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT6]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = manipulator(&d_element7, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT7]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = manipulator(&d_element8, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT8]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = manipulator(&d_element9, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT9]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = manipulator(&d_element10, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT10]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = manipulator(&d_element11, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT11]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = manipulator(&d_element12, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT12]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = manipulator(&d_element13, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT13]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = manipulator(&d_element14, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT14]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = manipulator(&d_element15, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT15]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    return ret;
}

template <class MANIPULATOR>
int Sequence6::manipulateAttribute(MANIPULATOR& manipulator, int id)
{
    enum { k_NOT_FOUND = -1 };

    switch (id) {
      case e_ATTRIBUTE_ID_ELEMENT1: {
        return manipulator(&d_element1, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT1]);
                                                                      // RETURN
      } break;
      case e_ATTRIBUTE_ID_ELEMENT2: {
        return manipulator(&d_element2, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT2]);
                                                                      // RETURN
      } break;
      case e_ATTRIBUTE_ID_ELEMENT3: {
        return manipulator(&d_element3, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT3]);
                                                                      // RETURN
      } break;
      case e_ATTRIBUTE_ID_ELEMENT4: {
        return manipulator(&d_element4, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT4]);
                                                                      // RETURN
      } break;
      case e_ATTRIBUTE_ID_ELEMENT5: {
        return manipulator(&d_element5, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT5]);
                                                                      // RETURN
      } break;
      case e_ATTRIBUTE_ID_ELEMENT6: {
        return manipulator(&d_element6, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT6]);
                                                                      // RETURN
      } break;
      case e_ATTRIBUTE_ID_ELEMENT7: {
        return manipulator(&d_element7, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT7]);
                                                                      // RETURN
      } break;
      case e_ATTRIBUTE_ID_ELEMENT8: {
        return manipulator(&d_element8, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT8]);
                                                                      // RETURN
      } break;
      case e_ATTRIBUTE_ID_ELEMENT9: {
        return manipulator(&d_element9, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT9]);
                                                                      // RETURN
      } break;
      case e_ATTRIBUTE_ID_ELEMENT10: {
        return manipulator(&d_element10, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT10]);
                                                                      // RETURN
      } break;
      case e_ATTRIBUTE_ID_ELEMENT11: {
        return manipulator(&d_element11, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT11]);
                                                                      // RETURN
      } break;
      case e_ATTRIBUTE_ID_ELEMENT12: {
        return manipulator(&d_element12, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT12]);
                                                                      // RETURN
      } break;
      case e_ATTRIBUTE_ID_ELEMENT13: {
        return manipulator(&d_element13, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT13]);
                                                                      // RETURN
      } break;
      case e_ATTRIBUTE_ID_ELEMENT14: {
        return manipulator(&d_element14, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT14]);
                                                                      // RETURN
      } break;
      case e_ATTRIBUTE_ID_ELEMENT15: {
        return manipulator(&d_element15, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT15]);
                                                                      // RETURN
      } break;
      default:
        return k_NOT_FOUND;                                           // RETURN
    }
}

template <class MANIPULATOR>
int Sequence6::manipulateAttribute(
        MANIPULATOR&  manipulator,
        const char   *name,
        int           nameLength)
{
    enum { k_NOT_FOUND = -1 };

    const bdlat_AttributeInfo *attributeInfo =
           lookupAttributeInfo(name, nameLength);
    if (0 == attributeInfo) {
        return k_NOT_FOUND;                                           // RETURN
    }

    return manipulateAttribute(manipulator, attributeInfo->d_id);
}

inline
bdlb::NullableValue<unsigned char>& Sequence6::element1()
{
    return d_element1;
}

inline
bdlb::NullableValue<CustomString>& Sequence6::element2()
{
    return d_element2;
}

inline
bdlb::NullableValue<CustomInt>& Sequence6::element3()
{
    return d_element3;
}

inline
unsigned int& Sequence6::element4()
{
    return d_element4;
}

inline
unsigned char& Sequence6::element5()
{
    return d_element5;
}

inline
bsl::vector<bdlb::NullableValue<CustomInt> >& Sequence6::element6()
{
    return d_element6;
}

inline
CustomString& Sequence6::element7()
{
    return d_element7;
}

inline
CustomInt& Sequence6::element8()
{
    return d_element8;
}

inline
bdlb::NullableValue<unsigned int>& Sequence6::element9()
{
    return d_element9;
}

inline
bsl::vector<unsigned char>& Sequence6::element10()
{
    return d_element10;
}

inline
bsl::vector<CustomString>& Sequence6::element11()
{
    return d_element11;
}

inline
bsl::vector<unsigned int>& Sequence6::element12()
{
    return d_element12;
}

inline
bsl::vector<bdlb::NullableValue<unsigned char> >& Sequence6::element13()
{
    return d_element13;
}

inline
bsl::vector<CustomInt>& Sequence6::element14()
{
    return d_element14;
}

inline
bsl::vector<bdlb::NullableValue<unsigned int> >& Sequence6::element15()
{
    return d_element15;
}

// ACCESSORS
template <class ACCESSOR>
int Sequence6::accessAttributes(ACCESSOR& accessor) const
{
    int ret;

    ret = accessor(d_element1, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT1]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = accessor(d_element2, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT2]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = accessor(d_element3, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT3]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = accessor(d_element4, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT4]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = accessor(d_element5, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT5]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = accessor(d_element6, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT6]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = accessor(d_element7, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT7]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = accessor(d_element8, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT8]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = accessor(d_element9, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT9]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = accessor(d_element10, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT10]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = accessor(d_element11, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT11]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = accessor(d_element12, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT12]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = accessor(d_element13, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT13]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = accessor(d_element14, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT14]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = accessor(d_element15, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT15]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    return ret;
}

template <class ACCESSOR>
int Sequence6::accessAttribute(ACCESSOR& accessor, int id) const
{
    enum { k_NOT_FOUND = -1 };

    switch (id) {
      case e_ATTRIBUTE_ID_ELEMENT1: {
        return accessor(d_element1, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT1]);
                                                                      // RETURN
      } break;
      case e_ATTRIBUTE_ID_ELEMENT2: {
        return accessor(d_element2, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT2]);
                                                                      // RETURN
      } break;
      case e_ATTRIBUTE_ID_ELEMENT3: {
        return accessor(d_element3, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT3]);
                                                                      // RETURN
      } break;
      case e_ATTRIBUTE_ID_ELEMENT4: {
        return accessor(d_element4, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT4]);
                                                                      // RETURN
      } break;
      case e_ATTRIBUTE_ID_ELEMENT5: {
        return accessor(d_element5, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT5]);
                                                                      // RETURN
      } break;
      case e_ATTRIBUTE_ID_ELEMENT6: {
        return accessor(d_element6, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT6]);
                                                                      // RETURN
      } break;
      case e_ATTRIBUTE_ID_ELEMENT7: {
        return accessor(d_element7, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT7]);
                                                                      // RETURN
      } break;
      case e_ATTRIBUTE_ID_ELEMENT8: {
        return accessor(d_element8, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT8]);
                                                                      // RETURN
      } break;
      case e_ATTRIBUTE_ID_ELEMENT9: {
        return accessor(d_element9, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT9]);
                                                                      // RETURN
      } break;
      case e_ATTRIBUTE_ID_ELEMENT10: {
        return accessor(d_element10, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT10]);
                                                                      // RETURN
      } break;
      case e_ATTRIBUTE_ID_ELEMENT11: {
        return accessor(d_element11, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT11]);
                                                                      // RETURN
      } break;
      case e_ATTRIBUTE_ID_ELEMENT12: {
        return accessor(d_element12, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT12]);
                                                                      // RETURN
      } break;
      case e_ATTRIBUTE_ID_ELEMENT13: {
        return accessor(d_element13, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT13]);
                                                                      // RETURN
      } break;
      case e_ATTRIBUTE_ID_ELEMENT14: {
        return accessor(d_element14, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT14]);
                                                                      // RETURN
      } break;
      case e_ATTRIBUTE_ID_ELEMENT15: {
        return accessor(d_element15, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT15]);
                                                                      // RETURN
      } break;
      default:
        return k_NOT_FOUND;                                           // RETURN
    }
}

template <class ACCESSOR>
int Sequence6::accessAttribute(
        ACCESSOR&   accessor,
        const char *name,
        int         nameLength) const
{
    enum { k_NOT_FOUND = -1 };

    const bdlat_AttributeInfo *attributeInfo =
          lookupAttributeInfo(name, nameLength);
    if (0 == attributeInfo) {
       return k_NOT_FOUND;                                            // RETURN
    }

    return accessAttribute(accessor, attributeInfo->d_id);
}

inline
const bdlb::NullableValue<unsigned char>& Sequence6::element1() const
{
    return d_element1;
}

inline
const bdlb::NullableValue<CustomString>& Sequence6::element2() const
{
    return d_element2;
}

inline
const bdlb::NullableValue<CustomInt>& Sequence6::element3() const
{
    return d_element3;
}

inline
unsigned int Sequence6::element4() const
{
    return d_element4;
}

inline
unsigned char Sequence6::element5() const
{
    return d_element5;
}

inline
const bsl::vector<bdlb::NullableValue<CustomInt> >& Sequence6::element6() const
{
    return d_element6;
}

inline
const CustomString& Sequence6::element7() const
{
    return d_element7;
}

inline
const CustomInt& Sequence6::element8() const
{
    return d_element8;
}

inline
const bdlb::NullableValue<unsigned int>& Sequence6::element9() const
{
    return d_element9;
}

inline
const bsl::vector<unsigned char>& Sequence6::element10() const
{
    return d_element10;
}

inline
const bsl::vector<CustomString>& Sequence6::element11() const
{
    return d_element11;
}

inline
const bsl::vector<unsigned int>& Sequence6::element12() const
{
    return d_element12;
}

inline
const bsl::vector<bdlb::NullableValue<unsigned char> >& Sequence6::element13() const
{
    return d_element13;
}

inline
const bsl::vector<CustomInt>& Sequence6::element14() const
{
    return d_element14;
}

inline
const bsl::vector<bdlb::NullableValue<unsigned int> >& Sequence6::element15() const
{
    return d_element15;
}



                               // -------------
                               // class Choice3
                               // -------------

// CREATORS
inline
Choice3::Choice3(bslma::Allocator *basicAllocator)
: d_selectionId(e_SELECTION_ID_UNDEFINED)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

inline
Choice3::~Choice3()
{
    reset();
}

// MANIPULATORS
template <class MANIPULATOR>
int Choice3::manipulateSelection(MANIPULATOR& manipulator)
{
    switch (d_selectionId) {
      case Choice3::e_SELECTION_ID_SELECTION1:
        return manipulator(&d_selection1.object(),
                SELECTION_INFO_ARRAY[e_SELECTION_INDEX_SELECTION1]);
      case Choice3::e_SELECTION_ID_SELECTION2:
        return manipulator(&d_selection2.object(),
                SELECTION_INFO_ARRAY[e_SELECTION_INDEX_SELECTION2]);
      case Choice3::e_SELECTION_ID_SELECTION3:
        return manipulator(&d_selection3.object(),
                SELECTION_INFO_ARRAY[e_SELECTION_INDEX_SELECTION3]);
      case Choice3::e_SELECTION_ID_SELECTION4:
        return manipulator(&d_selection4.object(),
                SELECTION_INFO_ARRAY[e_SELECTION_INDEX_SELECTION4]);
      default:
        BSLS_ASSERT(Choice3::e_SELECTION_ID_UNDEFINED == d_selectionId);
        return -1;                                                    // RETURN
    }
}

inline
Sequence6& Choice3::selection1()
{
    BSLS_ASSERT(e_SELECTION_ID_SELECTION1 == d_selectionId);
    return d_selection1.object();
}

inline
unsigned char& Choice3::selection2()
{
    BSLS_ASSERT(e_SELECTION_ID_SELECTION2 == d_selectionId);
    return d_selection2.object();
}

inline
CustomString& Choice3::selection3()
{
    BSLS_ASSERT(e_SELECTION_ID_SELECTION3 == d_selectionId);
    return d_selection3.object();
}

inline
CustomInt& Choice3::selection4()
{
    BSLS_ASSERT(e_SELECTION_ID_SELECTION4 == d_selectionId);
    return d_selection4.object();
}

// ACCESSORS
inline
int Choice3::selectionId() const
{
    return d_selectionId;
}

template <class ACCESSOR>
int Choice3::accessSelection(ACCESSOR& accessor) const
{
    switch (d_selectionId) {
      case e_SELECTION_ID_SELECTION1:
        return accessor(d_selection1.object(),
                SELECTION_INFO_ARRAY[e_SELECTION_INDEX_SELECTION1]);
      case e_SELECTION_ID_SELECTION2:
        return accessor(d_selection2.object(),
                SELECTION_INFO_ARRAY[e_SELECTION_INDEX_SELECTION2]);
      case e_SELECTION_ID_SELECTION3:
        return accessor(d_selection3.object(),
                SELECTION_INFO_ARRAY[e_SELECTION_INDEX_SELECTION3]);
      case e_SELECTION_ID_SELECTION4:
        return accessor(d_selection4.object(),
                SELECTION_INFO_ARRAY[e_SELECTION_INDEX_SELECTION4]);
      default:
        BSLS_ASSERT(e_SELECTION_ID_UNDEFINED == d_selectionId);
        return -1;                                                    // RETURN
    }
}

inline
const Sequence6& Choice3::selection1() const
{
    BSLS_ASSERT(e_SELECTION_ID_SELECTION1 == d_selectionId);
    return d_selection1.object();
}

inline
const unsigned char& Choice3::selection2() const
{
    BSLS_ASSERT(e_SELECTION_ID_SELECTION2 == d_selectionId);
    return d_selection2.object();
}

inline
const CustomString& Choice3::selection3() const
{
    BSLS_ASSERT(e_SELECTION_ID_SELECTION3 == d_selectionId);
    return d_selection3.object();
}

inline
const CustomInt& Choice3::selection4() const
{
    BSLS_ASSERT(e_SELECTION_ID_SELECTION4 == d_selectionId);
    return d_selection4.object();
}

inline
bool Choice3::isSelection1Value() const
{
    return e_SELECTION_ID_SELECTION1 == d_selectionId;
}

inline
bool Choice3::isSelection2Value() const
{
    return e_SELECTION_ID_SELECTION2 == d_selectionId;
}

inline
bool Choice3::isSelection3Value() const
{
    return e_SELECTION_ID_SELECTION3 == d_selectionId;
}

inline
bool Choice3::isSelection4Value() const
{
    return e_SELECTION_ID_SELECTION4 == d_selectionId;
}

inline
bool Choice3::isUndefinedValue() const
{
    return e_SELECTION_ID_UNDEFINED == d_selectionId;
}


                     // ---------------------------------
                     // class SequenceWithAnonymityChoice
                     // ---------------------------------

// CREATORS
inline
SequenceWithAnonymityChoice::SequenceWithAnonymityChoice(bslma::Allocator *basicAllocator)
: d_selectionId(e_SELECTION_ID_UNDEFINED)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

inline
SequenceWithAnonymityChoice::~SequenceWithAnonymityChoice()
{
    reset();
}

// MANIPULATORS
template <class MANIPULATOR>
int SequenceWithAnonymityChoice::manipulateSelection(MANIPULATOR& manipulator)
{
    switch (d_selectionId) {
      case SequenceWithAnonymityChoice::e_SELECTION_ID_SELECTION1:
        return manipulator(&d_selection1.object(),
                SELECTION_INFO_ARRAY[e_SELECTION_INDEX_SELECTION1]);
      case SequenceWithAnonymityChoice::e_SELECTION_ID_SELECTION2:
        return manipulator(&d_selection2.object(),
                SELECTION_INFO_ARRAY[e_SELECTION_INDEX_SELECTION2]);
      case SequenceWithAnonymityChoice::e_SELECTION_ID_SELECTION3:
        return manipulator(&d_selection3.object(),
                SELECTION_INFO_ARRAY[e_SELECTION_INDEX_SELECTION3]);
      case SequenceWithAnonymityChoice::e_SELECTION_ID_SELECTION4:
        return manipulator(&d_selection4.object(),
                SELECTION_INFO_ARRAY[e_SELECTION_INDEX_SELECTION4]);
      default:
        BSLS_ASSERT(SequenceWithAnonymityChoice::e_SELECTION_ID_UNDEFINED == d_selectionId);
        return -1;                                                    // RETURN
    }
}

inline
Sequence6& SequenceWithAnonymityChoice::selection1()
{
    BSLS_ASSERT(e_SELECTION_ID_SELECTION1 == d_selectionId);
    return d_selection1.object();
}

inline
unsigned char& SequenceWithAnonymityChoice::selection2()
{
    BSLS_ASSERT(e_SELECTION_ID_SELECTION2 == d_selectionId);
    return d_selection2.object();
}

inline
CustomString& SequenceWithAnonymityChoice::selection3()
{
    BSLS_ASSERT(e_SELECTION_ID_SELECTION3 == d_selectionId);
    return d_selection3.object();
}

inline
CustomInt& SequenceWithAnonymityChoice::selection4()
{
    BSLS_ASSERT(e_SELECTION_ID_SELECTION4 == d_selectionId);
    return d_selection4.object();
}

// ACCESSORS
inline
int SequenceWithAnonymityChoice::selectionId() const
{
    return d_selectionId;
}

template <class ACCESSOR>
int SequenceWithAnonymityChoice::accessSelection(ACCESSOR& accessor) const
{
    switch (d_selectionId) {
      case e_SELECTION_ID_SELECTION1:
        return accessor(d_selection1.object(),
                SELECTION_INFO_ARRAY[e_SELECTION_INDEX_SELECTION1]);
      case e_SELECTION_ID_SELECTION2:
        return accessor(d_selection2.object(),
                SELECTION_INFO_ARRAY[e_SELECTION_INDEX_SELECTION2]);
      case e_SELECTION_ID_SELECTION3:
        return accessor(d_selection3.object(),
                SELECTION_INFO_ARRAY[e_SELECTION_INDEX_SELECTION3]);
      case e_SELECTION_ID_SELECTION4:
        return accessor(d_selection4.object(),
                SELECTION_INFO_ARRAY[e_SELECTION_INDEX_SELECTION4]);
      default:
        BSLS_ASSERT(e_SELECTION_ID_UNDEFINED == d_selectionId);
        return -1;                                                    // RETURN
    }
}

inline
const Sequence6& SequenceWithAnonymityChoice::selection1() const
{
    BSLS_ASSERT(e_SELECTION_ID_SELECTION1 == d_selectionId);
    return d_selection1.object();
}

inline
const unsigned char& SequenceWithAnonymityChoice::selection2() const
{
    BSLS_ASSERT(e_SELECTION_ID_SELECTION2 == d_selectionId);
    return d_selection2.object();
}

inline
const CustomString& SequenceWithAnonymityChoice::selection3() const
{
    BSLS_ASSERT(e_SELECTION_ID_SELECTION3 == d_selectionId);
    return d_selection3.object();
}

inline
const CustomInt& SequenceWithAnonymityChoice::selection4() const
{
    BSLS_ASSERT(e_SELECTION_ID_SELECTION4 == d_selectionId);
    return d_selection4.object();
}

inline
bool SequenceWithAnonymityChoice::isSelection1Value() const
{
    return e_SELECTION_ID_SELECTION1 == d_selectionId;
}

inline
bool SequenceWithAnonymityChoice::isSelection2Value() const
{
    return e_SELECTION_ID_SELECTION2 == d_selectionId;
}

inline
bool SequenceWithAnonymityChoice::isSelection3Value() const
{
    return e_SELECTION_ID_SELECTION3 == d_selectionId;
}

inline
bool SequenceWithAnonymityChoice::isSelection4Value() const
{
    return e_SELECTION_ID_SELECTION4 == d_selectionId;
}

inline
bool SequenceWithAnonymityChoice::isUndefinedValue() const
{
    return e_SELECTION_ID_UNDEFINED == d_selectionId;
}


                               // -------------
                               // class Choice1
                               // -------------

// CREATORS
inline
Choice1::Choice1(bslma::Allocator *basicAllocator)
: d_selectionId(e_SELECTION_ID_UNDEFINED)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

inline
Choice1::~Choice1()
{
    reset();
}

// MANIPULATORS
template <class MANIPULATOR>
int Choice1::manipulateSelection(MANIPULATOR& manipulator)
{
    switch (d_selectionId) {
      case Choice1::e_SELECTION_ID_SELECTION1:
        return manipulator(&d_selection1.object(),
                SELECTION_INFO_ARRAY[e_SELECTION_INDEX_SELECTION1]);
      case Choice1::e_SELECTION_ID_SELECTION2:
        return manipulator(&d_selection2.object(),
                SELECTION_INFO_ARRAY[e_SELECTION_INDEX_SELECTION2]);
      case Choice1::e_SELECTION_ID_SELECTION3:
        return manipulator(d_selection3,
                SELECTION_INFO_ARRAY[e_SELECTION_INDEX_SELECTION3]);
      case Choice1::e_SELECTION_ID_SELECTION4:
        return manipulator(d_selection4,
                SELECTION_INFO_ARRAY[e_SELECTION_INDEX_SELECTION4]);
      default:
        BSLS_ASSERT(Choice1::e_SELECTION_ID_UNDEFINED == d_selectionId);
        return -1;                                                    // RETURN
    }
}

inline
int& Choice1::selection1()
{
    BSLS_ASSERT(e_SELECTION_ID_SELECTION1 == d_selectionId);
    return d_selection1.object();
}

inline
double& Choice1::selection2()
{
    BSLS_ASSERT(e_SELECTION_ID_SELECTION2 == d_selectionId);
    return d_selection2.object();
}

inline
Sequence4& Choice1::selection3()
{
    BSLS_ASSERT(e_SELECTION_ID_SELECTION3 == d_selectionId);
    return *d_selection3;
}

inline
Choice2& Choice1::selection4()
{
    BSLS_ASSERT(e_SELECTION_ID_SELECTION4 == d_selectionId);
    return *d_selection4;
}

// ACCESSORS
inline
int Choice1::selectionId() const
{
    return d_selectionId;
}

template <class ACCESSOR>
int Choice1::accessSelection(ACCESSOR& accessor) const
{
    switch (d_selectionId) {
      case e_SELECTION_ID_SELECTION1:
        return accessor(d_selection1.object(),
                SELECTION_INFO_ARRAY[e_SELECTION_INDEX_SELECTION1]);
      case e_SELECTION_ID_SELECTION2:
        return accessor(d_selection2.object(),
                SELECTION_INFO_ARRAY[e_SELECTION_INDEX_SELECTION2]);
      case e_SELECTION_ID_SELECTION3:
        return accessor(*d_selection3,
                SELECTION_INFO_ARRAY[e_SELECTION_INDEX_SELECTION3]);
      case e_SELECTION_ID_SELECTION4:
        return accessor(*d_selection4,
                SELECTION_INFO_ARRAY[e_SELECTION_INDEX_SELECTION4]);
      default:
        BSLS_ASSERT(e_SELECTION_ID_UNDEFINED == d_selectionId);
        return -1;                                                    // RETURN
    }
}

inline
const int& Choice1::selection1() const
{
    BSLS_ASSERT(e_SELECTION_ID_SELECTION1 == d_selectionId);
    return d_selection1.object();
}

inline
const double& Choice1::selection2() const
{
    BSLS_ASSERT(e_SELECTION_ID_SELECTION2 == d_selectionId);
    return d_selection2.object();
}

inline
const Sequence4& Choice1::selection3() const
{
    BSLS_ASSERT(e_SELECTION_ID_SELECTION3 == d_selectionId);
    return *d_selection3;
}

inline
const Choice2& Choice1::selection4() const
{
    BSLS_ASSERT(e_SELECTION_ID_SELECTION4 == d_selectionId);
    return *d_selection4;
}

inline
bool Choice1::isSelection1Value() const
{
    return e_SELECTION_ID_SELECTION1 == d_selectionId;
}

inline
bool Choice1::isSelection2Value() const
{
    return e_SELECTION_ID_SELECTION2 == d_selectionId;
}

inline
bool Choice1::isSelection3Value() const
{
    return e_SELECTION_ID_SELECTION3 == d_selectionId;
}

inline
bool Choice1::isSelection4Value() const
{
    return e_SELECTION_ID_SELECTION4 == d_selectionId;
}

inline
bool Choice1::isUndefinedValue() const
{
    return e_SELECTION_ID_UNDEFINED == d_selectionId;
}


                               // -------------
                               // class Choice2
                               // -------------

// CREATORS
inline
Choice2::Choice2(bslma::Allocator *basicAllocator)
: d_selectionId(e_SELECTION_ID_UNDEFINED)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

inline
Choice2::~Choice2()
{
    reset();
}

// MANIPULATORS
template <class MANIPULATOR>
int Choice2::manipulateSelection(MANIPULATOR& manipulator)
{
    switch (d_selectionId) {
      case Choice2::e_SELECTION_ID_SELECTION1:
        return manipulator(&d_selection1.object(),
                SELECTION_INFO_ARRAY[e_SELECTION_INDEX_SELECTION1]);
      case Choice2::e_SELECTION_ID_SELECTION2:
        return manipulator(&d_selection2.object(),
                SELECTION_INFO_ARRAY[e_SELECTION_INDEX_SELECTION2]);
      case Choice2::e_SELECTION_ID_SELECTION3:
        return manipulator(d_selection3,
                SELECTION_INFO_ARRAY[e_SELECTION_INDEX_SELECTION3]);
      case Choice2::e_SELECTION_ID_SELECTION4:
        return manipulator(&d_selection4.object(),
                SELECTION_INFO_ARRAY[e_SELECTION_INDEX_SELECTION4]);
      default:
        BSLS_ASSERT(Choice2::e_SELECTION_ID_UNDEFINED == d_selectionId);
        return -1;                                                    // RETURN
    }
}

inline
bool& Choice2::selection1()
{
    BSLS_ASSERT(e_SELECTION_ID_SELECTION1 == d_selectionId);
    return d_selection1.object();
}

inline
bsl::string& Choice2::selection2()
{
    BSLS_ASSERT(e_SELECTION_ID_SELECTION2 == d_selectionId);
    return d_selection2.object();
}

inline
Choice1& Choice2::selection3()
{
    BSLS_ASSERT(e_SELECTION_ID_SELECTION3 == d_selectionId);
    return *d_selection3;
}

inline
unsigned int& Choice2::selection4()
{
    BSLS_ASSERT(e_SELECTION_ID_SELECTION4 == d_selectionId);
    return d_selection4.object();
}

// ACCESSORS
inline
int Choice2::selectionId() const
{
    return d_selectionId;
}

template <class ACCESSOR>
int Choice2::accessSelection(ACCESSOR& accessor) const
{
    switch (d_selectionId) {
      case e_SELECTION_ID_SELECTION1:
        return accessor(d_selection1.object(),
                SELECTION_INFO_ARRAY[e_SELECTION_INDEX_SELECTION1]);
      case e_SELECTION_ID_SELECTION2:
        return accessor(d_selection2.object(),
                SELECTION_INFO_ARRAY[e_SELECTION_INDEX_SELECTION2]);
      case e_SELECTION_ID_SELECTION3:
        return accessor(*d_selection3,
                SELECTION_INFO_ARRAY[e_SELECTION_INDEX_SELECTION3]);
      case e_SELECTION_ID_SELECTION4:
        return accessor(d_selection4.object(),
                SELECTION_INFO_ARRAY[e_SELECTION_INDEX_SELECTION4]);
      default:
        BSLS_ASSERT(e_SELECTION_ID_UNDEFINED == d_selectionId);
        return -1;                                                    // RETURN
    }
}

inline
const bool& Choice2::selection1() const
{
    BSLS_ASSERT(e_SELECTION_ID_SELECTION1 == d_selectionId);
    return d_selection1.object();
}

inline
const bsl::string& Choice2::selection2() const
{
    BSLS_ASSERT(e_SELECTION_ID_SELECTION2 == d_selectionId);
    return d_selection2.object();
}

inline
const Choice1& Choice2::selection3() const
{
    BSLS_ASSERT(e_SELECTION_ID_SELECTION3 == d_selectionId);
    return *d_selection3;
}

inline
const unsigned int& Choice2::selection4() const
{
    BSLS_ASSERT(e_SELECTION_ID_SELECTION4 == d_selectionId);
    return d_selection4.object();
}

inline
bool Choice2::isSelection1Value() const
{
    return e_SELECTION_ID_SELECTION1 == d_selectionId;
}

inline
bool Choice2::isSelection2Value() const
{
    return e_SELECTION_ID_SELECTION2 == d_selectionId;
}

inline
bool Choice2::isSelection3Value() const
{
    return e_SELECTION_ID_SELECTION3 == d_selectionId;
}

inline
bool Choice2::isSelection4Value() const
{
    return e_SELECTION_ID_SELECTION4 == d_selectionId;
}

inline
bool Choice2::isUndefinedValue() const
{
    return e_SELECTION_ID_UNDEFINED == d_selectionId;
}


                              // ---------------
                              // class Sequence4
                              // ---------------

// MANIPULATORS
template <class MANIPULATOR>
int Sequence4::manipulateAttributes(MANIPULATOR& manipulator)
{
    int ret;

    ret = manipulator(&d_element1, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT1]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = manipulator(&d_element2, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT2]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = manipulator(&d_element3, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT3]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = manipulator(&d_element4, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT4]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = manipulator(&d_element5, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT5]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = manipulator(&d_element6, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT6]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = manipulator(&d_element7, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT7]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = manipulator(&d_element8, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT8]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = manipulator(&d_element9, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT9]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = manipulator(&d_element10, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT10]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = manipulator(&d_element11, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT11]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = manipulator(&d_element12, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT12]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = manipulator(&d_element13, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT13]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = manipulator(&d_element14, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT14]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = manipulator(&d_element15, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT15]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = manipulator(&d_element16, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT16]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = manipulator(&d_element17, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT17]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = manipulator(&d_element18, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT18]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = manipulator(&d_element19, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT19]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    return ret;
}

template <class MANIPULATOR>
int Sequence4::manipulateAttribute(MANIPULATOR& manipulator, int id)
{
    enum { k_NOT_FOUND = -1 };

    switch (id) {
      case e_ATTRIBUTE_ID_ELEMENT1: {
        return manipulator(&d_element1, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT1]);
                                                                      // RETURN
      } break;
      case e_ATTRIBUTE_ID_ELEMENT2: {
        return manipulator(&d_element2, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT2]);
                                                                      // RETURN
      } break;
      case e_ATTRIBUTE_ID_ELEMENT3: {
        return manipulator(&d_element3, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT3]);
                                                                      // RETURN
      } break;
      case e_ATTRIBUTE_ID_ELEMENT4: {
        return manipulator(&d_element4, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT4]);
                                                                      // RETURN
      } break;
      case e_ATTRIBUTE_ID_ELEMENT5: {
        return manipulator(&d_element5, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT5]);
                                                                      // RETURN
      } break;
      case e_ATTRIBUTE_ID_ELEMENT6: {
        return manipulator(&d_element6, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT6]);
                                                                      // RETURN
      } break;
      case e_ATTRIBUTE_ID_ELEMENT7: {
        return manipulator(&d_element7, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT7]);
                                                                      // RETURN
      } break;
      case e_ATTRIBUTE_ID_ELEMENT8: {
        return manipulator(&d_element8, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT8]);
                                                                      // RETURN
      } break;
      case e_ATTRIBUTE_ID_ELEMENT9: {
        return manipulator(&d_element9, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT9]);
                                                                      // RETURN
      } break;
      case e_ATTRIBUTE_ID_ELEMENT10: {
        return manipulator(&d_element10, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT10]);
                                                                      // RETURN
      } break;
      case e_ATTRIBUTE_ID_ELEMENT11: {
        return manipulator(&d_element11, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT11]);
                                                                      // RETURN
      } break;
      case e_ATTRIBUTE_ID_ELEMENT12: {
        return manipulator(&d_element12, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT12]);
                                                                      // RETURN
      } break;
      case e_ATTRIBUTE_ID_ELEMENT13: {
        return manipulator(&d_element13, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT13]);
                                                                      // RETURN
      } break;
      case e_ATTRIBUTE_ID_ELEMENT14: {
        return manipulator(&d_element14, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT14]);
                                                                      // RETURN
      } break;
      case e_ATTRIBUTE_ID_ELEMENT15: {
        return manipulator(&d_element15, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT15]);
                                                                      // RETURN
      } break;
      case e_ATTRIBUTE_ID_ELEMENT16: {
        return manipulator(&d_element16, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT16]);
                                                                      // RETURN
      } break;
      case e_ATTRIBUTE_ID_ELEMENT17: {
        return manipulator(&d_element17, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT17]);
                                                                      // RETURN
      } break;
      case e_ATTRIBUTE_ID_ELEMENT18: {
        return manipulator(&d_element18, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT18]);
                                                                      // RETURN
      } break;
      case e_ATTRIBUTE_ID_ELEMENT19: {
        return manipulator(&d_element19, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT19]);
                                                                      // RETURN
      } break;
      default:
        return k_NOT_FOUND;                                           // RETURN
    }
}

template <class MANIPULATOR>
int Sequence4::manipulateAttribute(
        MANIPULATOR&  manipulator,
        const char   *name,
        int           nameLength)
{
    enum { k_NOT_FOUND = -1 };

    const bdlat_AttributeInfo *attributeInfo =
           lookupAttributeInfo(name, nameLength);
    if (0 == attributeInfo) {
        return k_NOT_FOUND;                                           // RETURN
    }

    return manipulateAttribute(manipulator, attributeInfo->d_id);
}

inline
bsl::vector<Sequence3>& Sequence4::element1()
{
    return d_element1;
}

inline
bsl::vector<Choice1>& Sequence4::element2()
{
    return d_element2;
}

inline
bdlb::NullableValue<bsl::vector<char> >& Sequence4::element3()
{
    return d_element3;
}

inline
bdlb::NullableValue<int>& Sequence4::element4()
{
    return d_element4;
}

inline
bdlb::NullableValue<bdlt::DatetimeTz>& Sequence4::element5()
{
    return d_element5;
}

inline
bdlb::NullableValue<CustomString>& Sequence4::element6()
{
    return d_element6;
}

inline
bdlb::NullableValue<Enumerated::Value>& Sequence4::element7()
{
    return d_element7;
}

inline
bool& Sequence4::element8()
{
    return d_element8;
}

inline
bsl::string& Sequence4::element9()
{
    return d_element9;
}

inline
double& Sequence4::element10()
{
    return d_element10;
}

inline
bsl::vector<char>& Sequence4::element11()
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
bsl::vector<bool>& Sequence4::element14()
{
    return d_element14;
}

inline
bsl::vector<double>& Sequence4::element15()
{
    return d_element15;
}

inline
bsl::vector<bsl::vector<char> >& Sequence4::element16()
{
    return d_element16;
}

inline
bsl::vector<int>& Sequence4::element17()
{
    return d_element17;
}

inline
bsl::vector<bdlt::DatetimeTz>& Sequence4::element18()
{
    return d_element18;
}

inline
bsl::vector<CustomString>& Sequence4::element19()
{
    return d_element19;
}

// ACCESSORS
template <class ACCESSOR>
int Sequence4::accessAttributes(ACCESSOR& accessor) const
{
    int ret;

    ret = accessor(d_element1, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT1]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = accessor(d_element2, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT2]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = accessor(d_element3, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT3]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = accessor(d_element4, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT4]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = accessor(d_element5, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT5]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = accessor(d_element6, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT6]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = accessor(d_element7, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT7]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = accessor(d_element8, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT8]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = accessor(d_element9, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT9]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = accessor(d_element10, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT10]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = accessor(d_element11, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT11]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = accessor(d_element12, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT12]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = accessor(d_element13, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT13]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = accessor(d_element14, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT14]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = accessor(d_element15, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT15]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = accessor(d_element16, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT16]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = accessor(d_element17, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT17]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = accessor(d_element18, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT18]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = accessor(d_element19, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT19]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    return ret;
}

template <class ACCESSOR>
int Sequence4::accessAttribute(ACCESSOR& accessor, int id) const
{
    enum { k_NOT_FOUND = -1 };

    switch (id) {
      case e_ATTRIBUTE_ID_ELEMENT1: {
        return accessor(d_element1, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT1]);
                                                                      // RETURN
      } break;
      case e_ATTRIBUTE_ID_ELEMENT2: {
        return accessor(d_element2, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT2]);
                                                                      // RETURN
      } break;
      case e_ATTRIBUTE_ID_ELEMENT3: {
        return accessor(d_element3, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT3]);
                                                                      // RETURN
      } break;
      case e_ATTRIBUTE_ID_ELEMENT4: {
        return accessor(d_element4, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT4]);
                                                                      // RETURN
      } break;
      case e_ATTRIBUTE_ID_ELEMENT5: {
        return accessor(d_element5, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT5]);
                                                                      // RETURN
      } break;
      case e_ATTRIBUTE_ID_ELEMENT6: {
        return accessor(d_element6, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT6]);
                                                                      // RETURN
      } break;
      case e_ATTRIBUTE_ID_ELEMENT7: {
        return accessor(d_element7, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT7]);
                                                                      // RETURN
      } break;
      case e_ATTRIBUTE_ID_ELEMENT8: {
        return accessor(d_element8, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT8]);
                                                                      // RETURN
      } break;
      case e_ATTRIBUTE_ID_ELEMENT9: {
        return accessor(d_element9, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT9]);
                                                                      // RETURN
      } break;
      case e_ATTRIBUTE_ID_ELEMENT10: {
        return accessor(d_element10, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT10]);
                                                                      // RETURN
      } break;
      case e_ATTRIBUTE_ID_ELEMENT11: {
        return accessor(d_element11, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT11]);
                                                                      // RETURN
      } break;
      case e_ATTRIBUTE_ID_ELEMENT12: {
        return accessor(d_element12, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT12]);
                                                                      // RETURN
      } break;
      case e_ATTRIBUTE_ID_ELEMENT13: {
        return accessor(d_element13, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT13]);
                                                                      // RETURN
      } break;
      case e_ATTRIBUTE_ID_ELEMENT14: {
        return accessor(d_element14, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT14]);
                                                                      // RETURN
      } break;
      case e_ATTRIBUTE_ID_ELEMENT15: {
        return accessor(d_element15, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT15]);
                                                                      // RETURN
      } break;
      case e_ATTRIBUTE_ID_ELEMENT16: {
        return accessor(d_element16, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT16]);
                                                                      // RETURN
      } break;
      case e_ATTRIBUTE_ID_ELEMENT17: {
        return accessor(d_element17, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT17]);
                                                                      // RETURN
      } break;
      case e_ATTRIBUTE_ID_ELEMENT18: {
        return accessor(d_element18, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT18]);
                                                                      // RETURN
      } break;
      case e_ATTRIBUTE_ID_ELEMENT19: {
        return accessor(d_element19, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT19]);
                                                                      // RETURN
      } break;
      default:
        return k_NOT_FOUND;                                           // RETURN
    }
}

template <class ACCESSOR>
int Sequence4::accessAttribute(
        ACCESSOR&   accessor,
        const char *name,
        int         nameLength) const
{
    enum { k_NOT_FOUND = -1 };

    const bdlat_AttributeInfo *attributeInfo =
          lookupAttributeInfo(name, nameLength);
    if (0 == attributeInfo) {
       return k_NOT_FOUND;                                            // RETURN
    }

    return accessAttribute(accessor, attributeInfo->d_id);
}

inline
const bsl::vector<Sequence3>& Sequence4::element1() const
{
    return d_element1;
}

inline
const bsl::vector<Choice1>& Sequence4::element2() const
{
    return d_element2;
}

inline
const bdlb::NullableValue<bsl::vector<char> >& Sequence4::element3() const
{
    return d_element3;
}

inline
const bdlb::NullableValue<int>& Sequence4::element4() const
{
    return d_element4;
}

inline
const bdlb::NullableValue<bdlt::DatetimeTz>& Sequence4::element5() const
{
    return d_element5;
}

inline
const bdlb::NullableValue<CustomString>& Sequence4::element6() const
{
    return d_element6;
}

inline
const bdlb::NullableValue<Enumerated::Value>& Sequence4::element7() const
{
    return d_element7;
}

inline
bool Sequence4::element8() const
{
    return d_element8;
}

inline
const bsl::string& Sequence4::element9() const
{
    return d_element9;
}

inline
double Sequence4::element10() const
{
    return d_element10;
}

inline
const bsl::vector<char>& Sequence4::element11() const
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
const bsl::vector<bool>& Sequence4::element14() const
{
    return d_element14;
}

inline
const bsl::vector<double>& Sequence4::element15() const
{
    return d_element15;
}

inline
const bsl::vector<bsl::vector<char> >& Sequence4::element16() const
{
    return d_element16;
}

inline
const bsl::vector<int>& Sequence4::element17() const
{
    return d_element17;
}

inline
const bsl::vector<bdlt::DatetimeTz>& Sequence4::element18() const
{
    return d_element18;
}

inline
const bsl::vector<CustomString>& Sequence4::element19() const
{
    return d_element19;
}



                              // ---------------
                              // class Sequence1
                              // ---------------

// MANIPULATORS
template <class MANIPULATOR>
int Sequence1::manipulateAttributes(MANIPULATOR& manipulator)
{
    int ret;

    ret = manipulator(&d_element1, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT1]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = manipulator(&d_element2, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT2]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = manipulator(d_element3, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT3]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = manipulator(&d_element4, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT4]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = manipulator(&d_element5, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT5]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    return ret;
}

template <class MANIPULATOR>
int Sequence1::manipulateAttribute(MANIPULATOR& manipulator, int id)
{
    enum { k_NOT_FOUND = -1 };

    switch (id) {
      case e_ATTRIBUTE_ID_ELEMENT1: {
        return manipulator(&d_element1, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT1]);
                                                                      // RETURN
      } break;
      case e_ATTRIBUTE_ID_ELEMENT2: {
        return manipulator(&d_element2, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT2]);
                                                                      // RETURN
      } break;
      case e_ATTRIBUTE_ID_ELEMENT3: {
        return manipulator(d_element3, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT3]);
                                                                      // RETURN
      } break;
      case e_ATTRIBUTE_ID_ELEMENT4: {
        return manipulator(&d_element4, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT4]);
                                                                      // RETURN
      } break;
      case e_ATTRIBUTE_ID_ELEMENT5: {
        return manipulator(&d_element5, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT5]);
                                                                      // RETURN
      } break;
      default:
        return k_NOT_FOUND;                                           // RETURN
    }
}

template <class MANIPULATOR>
int Sequence1::manipulateAttribute(
        MANIPULATOR&  manipulator,
        const char   *name,
        int           nameLength)
{
    enum { k_NOT_FOUND = -1 };

    const bdlat_AttributeInfo *attributeInfo =
           lookupAttributeInfo(name, nameLength);
    if (0 == attributeInfo) {
        return k_NOT_FOUND;                                           // RETURN
    }

    return manipulateAttribute(manipulator, attributeInfo->d_id);
}

inline
bdlb::NullableValue<Choice3>& Sequence1::element1()
{
    return d_element1;
}

inline
bsl::vector<Choice1>& Sequence1::element2()
{
    return d_element2;
}

inline
Choice2& Sequence1::element3()
{
    return *d_element3;
}

inline
bsl::vector<bdlb::NullableValue<Choice1> >& Sequence1::element4()
{
    return d_element4;
}

inline
bsl::vector<Choice3>& Sequence1::element5()
{
    return d_element5;
}

// ACCESSORS
template <class ACCESSOR>
int Sequence1::accessAttributes(ACCESSOR& accessor) const
{
    int ret;

    ret = accessor(d_element1, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT1]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = accessor(d_element2, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT2]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = accessor(*d_element3, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT3]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = accessor(d_element4, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT4]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = accessor(d_element5, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT5]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    return ret;
}

template <class ACCESSOR>
int Sequence1::accessAttribute(ACCESSOR& accessor, int id) const
{
    enum { k_NOT_FOUND = -1 };

    switch (id) {
      case e_ATTRIBUTE_ID_ELEMENT1: {
        return accessor(d_element1, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT1]);
                                                                      // RETURN
      } break;
      case e_ATTRIBUTE_ID_ELEMENT2: {
        return accessor(d_element2, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT2]);
                                                                      // RETURN
      } break;
      case e_ATTRIBUTE_ID_ELEMENT3: {
        return accessor(*d_element3, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT3]);
                                                                      // RETURN
      } break;
      case e_ATTRIBUTE_ID_ELEMENT4: {
        return accessor(d_element4, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT4]);
                                                                      // RETURN
      } break;
      case e_ATTRIBUTE_ID_ELEMENT5: {
        return accessor(d_element5, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT5]);
                                                                      // RETURN
      } break;
      default:
        return k_NOT_FOUND;                                           // RETURN
    }
}

template <class ACCESSOR>
int Sequence1::accessAttribute(
        ACCESSOR&   accessor,
        const char *name,
        int         nameLength) const
{
    enum { k_NOT_FOUND = -1 };

    const bdlat_AttributeInfo *attributeInfo =
          lookupAttributeInfo(name, nameLength);
    if (0 == attributeInfo) {
       return k_NOT_FOUND;                                            // RETURN
    }

    return accessAttribute(accessor, attributeInfo->d_id);
}

inline
const bdlb::NullableValue<Choice3>& Sequence1::element1() const
{
    return d_element1;
}

inline
const bsl::vector<Choice1>& Sequence1::element2() const
{
    return d_element2;
}

inline
const Choice2& Sequence1::element3() const
{
    return *d_element3;
}

inline
const bsl::vector<bdlb::NullableValue<Choice1> >& Sequence1::element4() const
{
    return d_element4;
}

inline
const bsl::vector<Choice3>& Sequence1::element5() const
{
    return d_element5;
}



                              // ---------------
                              // class Sequence2
                              // ---------------

// MANIPULATORS
template <class MANIPULATOR>
int Sequence2::manipulateAttributes(MANIPULATOR& manipulator)
{
    int ret;

    ret = manipulator(&d_element1, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT1]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = manipulator(&d_element2, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT2]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = manipulator(&d_element3, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT3]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = manipulator(&d_element4, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT4]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = manipulator(&d_element5, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT5]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    return ret;
}

template <class MANIPULATOR>
int Sequence2::manipulateAttribute(MANIPULATOR& manipulator, int id)
{
    enum { k_NOT_FOUND = -1 };

    switch (id) {
      case e_ATTRIBUTE_ID_ELEMENT1: {
        return manipulator(&d_element1, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT1]);
                                                                      // RETURN
      } break;
      case e_ATTRIBUTE_ID_ELEMENT2: {
        return manipulator(&d_element2, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT2]);
                                                                      // RETURN
      } break;
      case e_ATTRIBUTE_ID_ELEMENT3: {
        return manipulator(&d_element3, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT3]);
                                                                      // RETURN
      } break;
      case e_ATTRIBUTE_ID_ELEMENT4: {
        return manipulator(&d_element4, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT4]);
                                                                      // RETURN
      } break;
      case e_ATTRIBUTE_ID_ELEMENT5: {
        return manipulator(&d_element5, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT5]);
                                                                      // RETURN
      } break;
      default:
        return k_NOT_FOUND;                                           // RETURN
    }
}

template <class MANIPULATOR>
int Sequence2::manipulateAttribute(
        MANIPULATOR&  manipulator,
        const char   *name,
        int           nameLength)
{
    enum { k_NOT_FOUND = -1 };

    const bdlat_AttributeInfo *attributeInfo =
           lookupAttributeInfo(name, nameLength);
    if (0 == attributeInfo) {
        return k_NOT_FOUND;                                           // RETURN
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
bdlt::DatetimeTz& Sequence2::element3()
{
    return d_element3;
}

inline
bdlb::NullableValue<Choice1>& Sequence2::element4()
{
    return d_element4;
}

inline
bdlb::NullableValue<double>& Sequence2::element5()
{
    return d_element5;
}

// ACCESSORS
template <class ACCESSOR>
int Sequence2::accessAttributes(ACCESSOR& accessor) const
{
    int ret;

    ret = accessor(d_element1, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT1]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = accessor(d_element2, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT2]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = accessor(d_element3, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT3]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = accessor(d_element4, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT4]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = accessor(d_element5, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT5]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    return ret;
}

template <class ACCESSOR>
int Sequence2::accessAttribute(ACCESSOR& accessor, int id) const
{
    enum { k_NOT_FOUND = -1 };

    switch (id) {
      case e_ATTRIBUTE_ID_ELEMENT1: {
        return accessor(d_element1, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT1]);
                                                                      // RETURN
      } break;
      case e_ATTRIBUTE_ID_ELEMENT2: {
        return accessor(d_element2, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT2]);
                                                                      // RETURN
      } break;
      case e_ATTRIBUTE_ID_ELEMENT3: {
        return accessor(d_element3, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT3]);
                                                                      // RETURN
      } break;
      case e_ATTRIBUTE_ID_ELEMENT4: {
        return accessor(d_element4, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT4]);
                                                                      // RETURN
      } break;
      case e_ATTRIBUTE_ID_ELEMENT5: {
        return accessor(d_element5, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT5]);
                                                                      // RETURN
      } break;
      default:
        return k_NOT_FOUND;                                           // RETURN
    }
}

template <class ACCESSOR>
int Sequence2::accessAttribute(
        ACCESSOR&   accessor,
        const char *name,
        int         nameLength) const
{
    enum { k_NOT_FOUND = -1 };

    const bdlat_AttributeInfo *attributeInfo =
          lookupAttributeInfo(name, nameLength);
    if (0 == attributeInfo) {
       return k_NOT_FOUND;                                            // RETURN
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
const bdlt::DatetimeTz& Sequence2::element3() const
{
    return d_element3;
}

inline
const bdlb::NullableValue<Choice1>& Sequence2::element4() const
{
    return d_element4;
}

inline
const bdlb::NullableValue<double>& Sequence2::element5() const
{
    return d_element5;
}



                     // ----------------------------------
                     // class SequenceWithAnonymityChoice2
                     // ----------------------------------

// CREATORS
inline
SequenceWithAnonymityChoice2::SequenceWithAnonymityChoice2(bslma::Allocator *basicAllocator)
: d_selectionId(e_SELECTION_ID_UNDEFINED)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

inline
SequenceWithAnonymityChoice2::~SequenceWithAnonymityChoice2()
{
    reset();
}

// MANIPULATORS
template <class MANIPULATOR>
int SequenceWithAnonymityChoice2::manipulateSelection(MANIPULATOR& manipulator)
{
    switch (d_selectionId) {
      case SequenceWithAnonymityChoice2::e_SELECTION_ID_SELECTION7:
        return manipulator(d_selection7,
                SELECTION_INFO_ARRAY[e_SELECTION_INDEX_SELECTION7]);
      case SequenceWithAnonymityChoice2::e_SELECTION_ID_SELECTION8:
        return manipulator(d_selection8,
                SELECTION_INFO_ARRAY[e_SELECTION_INDEX_SELECTION8]);
      default:
        BSLS_ASSERT(SequenceWithAnonymityChoice2::e_SELECTION_ID_UNDEFINED == d_selectionId);
        return -1;                                                    // RETURN
    }
}

inline
Sequence4& SequenceWithAnonymityChoice2::selection7()
{
    BSLS_ASSERT(e_SELECTION_ID_SELECTION7 == d_selectionId);
    return *d_selection7;
}

inline
Choice2& SequenceWithAnonymityChoice2::selection8()
{
    BSLS_ASSERT(e_SELECTION_ID_SELECTION8 == d_selectionId);
    return *d_selection8;
}

// ACCESSORS
inline
int SequenceWithAnonymityChoice2::selectionId() const
{
    return d_selectionId;
}

template <class ACCESSOR>
int SequenceWithAnonymityChoice2::accessSelection(ACCESSOR& accessor) const
{
    switch (d_selectionId) {
      case e_SELECTION_ID_SELECTION7:
        return accessor(*d_selection7,
                SELECTION_INFO_ARRAY[e_SELECTION_INDEX_SELECTION7]);
      case e_SELECTION_ID_SELECTION8:
        return accessor(*d_selection8,
                SELECTION_INFO_ARRAY[e_SELECTION_INDEX_SELECTION8]);
      default:
        BSLS_ASSERT(e_SELECTION_ID_UNDEFINED == d_selectionId);
        return -1;                                                    // RETURN
    }
}

inline
const Sequence4& SequenceWithAnonymityChoice2::selection7() const
{
    BSLS_ASSERT(e_SELECTION_ID_SELECTION7 == d_selectionId);
    return *d_selection7;
}

inline
const Choice2& SequenceWithAnonymityChoice2::selection8() const
{
    BSLS_ASSERT(e_SELECTION_ID_SELECTION8 == d_selectionId);
    return *d_selection8;
}

inline
bool SequenceWithAnonymityChoice2::isSelection7Value() const
{
    return e_SELECTION_ID_SELECTION7 == d_selectionId;
}

inline
bool SequenceWithAnonymityChoice2::isSelection8Value() const
{
    return e_SELECTION_ID_SELECTION8 == d_selectionId;
}

inline
bool SequenceWithAnonymityChoice2::isUndefinedValue() const
{
    return e_SELECTION_ID_UNDEFINED == d_selectionId;
}


                        // ---------------------------
                        // class SequenceWithAnonymity
                        // ---------------------------

// MANIPULATORS
template <class MANIPULATOR>
int SequenceWithAnonymity::manipulateAttributes(MANIPULATOR& manipulator)
{
    int ret;

    ret = manipulator(&d_choice, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_CHOICE]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = manipulator(&d_choice1, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_CHOICE1]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = manipulator(&d_choice2, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_CHOICE2]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = manipulator(&d_element4, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT4]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    return ret;
}

template <class MANIPULATOR>
int SequenceWithAnonymity::manipulateAttribute(MANIPULATOR& manipulator, int id)
{
    enum { k_NOT_FOUND = -1 };

    switch (id) {
      case e_ATTRIBUTE_ID_CHOICE: {
        return manipulator(&d_choice, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_CHOICE]);
                                                                      // RETURN
      } break;
      case e_ATTRIBUTE_ID_CHOICE1: {
        return manipulator(&d_choice1, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_CHOICE1]);
                                                                      // RETURN
      } break;
      case e_ATTRIBUTE_ID_CHOICE2: {
        return manipulator(&d_choice2, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_CHOICE2]);
                                                                      // RETURN
      } break;
      case e_ATTRIBUTE_ID_ELEMENT4: {
        return manipulator(&d_element4, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT4]);
                                                                      // RETURN
      } break;
      default:
        return k_NOT_FOUND;                                           // RETURN
    }
}

template <class MANIPULATOR>
int SequenceWithAnonymity::manipulateAttribute(
        MANIPULATOR&  manipulator,
        const char   *name,
        int           nameLength)
{
    enum { k_NOT_FOUND = -1 };

    const bdlat_AttributeInfo *attributeInfo =
           lookupAttributeInfo(name, nameLength);
    if (0 == attributeInfo) {
        return k_NOT_FOUND;                                           // RETURN
    }

    return manipulateAttribute(manipulator, attributeInfo->d_id);
}

inline
SequenceWithAnonymityChoice& SequenceWithAnonymity::choice()
{
    return d_choice;
}

inline
SequenceWithAnonymityChoice1& SequenceWithAnonymity::choice1()
{
    return d_choice1;
}

inline
bdlb::NullableValue<SequenceWithAnonymityChoice2>& SequenceWithAnonymity::choice2()
{
    return d_choice2;
}

inline
Sequence6& SequenceWithAnonymity::element4()
{
    return d_element4;
}

// ACCESSORS
template <class ACCESSOR>
int SequenceWithAnonymity::accessAttributes(ACCESSOR& accessor) const
{
    int ret;

    ret = accessor(d_choice, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_CHOICE]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = accessor(d_choice1, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_CHOICE1]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = accessor(d_choice2, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_CHOICE2]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = accessor(d_element4, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT4]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    return ret;
}

template <class ACCESSOR>
int SequenceWithAnonymity::accessAttribute(ACCESSOR& accessor, int id) const
{
    enum { k_NOT_FOUND = -1 };

    switch (id) {
      case e_ATTRIBUTE_ID_CHOICE: {
        return accessor(d_choice, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_CHOICE]);
                                                                      // RETURN
      } break;
      case e_ATTRIBUTE_ID_CHOICE1: {
        return accessor(d_choice1, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_CHOICE1]);
                                                                      // RETURN
      } break;
      case e_ATTRIBUTE_ID_CHOICE2: {
        return accessor(d_choice2, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_CHOICE2]);
                                                                      // RETURN
      } break;
      case e_ATTRIBUTE_ID_ELEMENT4: {
        return accessor(d_element4, ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT4]);
                                                                      // RETURN
      } break;
      default:
        return k_NOT_FOUND;                                           // RETURN
    }
}

template <class ACCESSOR>
int SequenceWithAnonymity::accessAttribute(
        ACCESSOR&   accessor,
        const char *name,
        int         nameLength) const
{
    enum { k_NOT_FOUND = -1 };

    const bdlat_AttributeInfo *attributeInfo =
          lookupAttributeInfo(name, nameLength);
    if (0 == attributeInfo) {
       return k_NOT_FOUND;                                            // RETURN
    }

    return accessAttribute(accessor, attributeInfo->d_id);
}

inline
const SequenceWithAnonymityChoice& SequenceWithAnonymity::choice() const
{
    return d_choice;
}

inline
const SequenceWithAnonymityChoice1& SequenceWithAnonymity::choice1() const
{
    return d_choice1;
}

inline
const bdlb::NullableValue<SequenceWithAnonymityChoice2>& SequenceWithAnonymity::choice2() const
{
    return d_choice2;
}

inline
const Sequence6& SequenceWithAnonymity::element4() const
{
    return d_element4;
}



                          // ------------------------
                          // class FeatureTestMessage
                          // ------------------------

// CREATORS
inline
FeatureTestMessage::FeatureTestMessage(bslma::Allocator *basicAllocator)
: d_selectionId(e_SELECTION_ID_UNDEFINED)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

inline
FeatureTestMessage::~FeatureTestMessage()
{
    reset();
}

// MANIPULATORS
template <class MANIPULATOR>
int FeatureTestMessage::manipulateSelection(MANIPULATOR& manipulator)
{
    switch (d_selectionId) {
      case FeatureTestMessage::e_SELECTION_ID_SELECTION1:
        return manipulator(&d_selection1.object(),
                SELECTION_INFO_ARRAY[e_SELECTION_INDEX_SELECTION1]);
      case FeatureTestMessage::e_SELECTION_ID_SELECTION2:
        return manipulator(&d_selection2.object(),
                SELECTION_INFO_ARRAY[e_SELECTION_INDEX_SELECTION2]);
      case FeatureTestMessage::e_SELECTION_ID_SELECTION3:
        return manipulator(&d_selection3.object(),
                SELECTION_INFO_ARRAY[e_SELECTION_INDEX_SELECTION3]);
      case FeatureTestMessage::e_SELECTION_ID_SELECTION4:
        return manipulator(d_selection4,
                SELECTION_INFO_ARRAY[e_SELECTION_INDEX_SELECTION4]);
      case FeatureTestMessage::e_SELECTION_ID_SELECTION5:
        return manipulator(&d_selection5.object(),
                SELECTION_INFO_ARRAY[e_SELECTION_INDEX_SELECTION5]);
      case FeatureTestMessage::e_SELECTION_ID_SELECTION6:
        return manipulator(&d_selection6.object(),
                SELECTION_INFO_ARRAY[e_SELECTION_INDEX_SELECTION6]);
      case FeatureTestMessage::e_SELECTION_ID_SELECTION7:
        return manipulator(&d_selection7.object(),
                SELECTION_INFO_ARRAY[e_SELECTION_INDEX_SELECTION7]);
      case FeatureTestMessage::e_SELECTION_ID_SELECTION8:
        return manipulator(&d_selection8.object(),
                SELECTION_INFO_ARRAY[e_SELECTION_INDEX_SELECTION8]);
      case FeatureTestMessage::e_SELECTION_ID_SELECTION9:
        return manipulator(&d_selection9.object(),
                SELECTION_INFO_ARRAY[e_SELECTION_INDEX_SELECTION9]);
      case FeatureTestMessage::e_SELECTION_ID_SELECTION10:
        return manipulator(&d_selection10.object(),
                SELECTION_INFO_ARRAY[e_SELECTION_INDEX_SELECTION10]);
      case FeatureTestMessage::e_SELECTION_ID_SELECTION11:
        return manipulator(&d_selection11.object(),
                SELECTION_INFO_ARRAY[e_SELECTION_INDEX_SELECTION11]);
      default:
        BSLS_ASSERT(FeatureTestMessage::e_SELECTION_ID_UNDEFINED == d_selectionId);
        return -1;                                                    // RETURN
    }
}

inline
Sequence1& FeatureTestMessage::selection1()
{
    BSLS_ASSERT(e_SELECTION_ID_SELECTION1 == d_selectionId);
    return d_selection1.object();
}

inline
bsl::vector<char>& FeatureTestMessage::selection2()
{
    BSLS_ASSERT(e_SELECTION_ID_SELECTION2 == d_selectionId);
    return d_selection2.object();
}

inline
Sequence2& FeatureTestMessage::selection3()
{
    BSLS_ASSERT(e_SELECTION_ID_SELECTION3 == d_selectionId);
    return d_selection3.object();
}

inline
Sequence3& FeatureTestMessage::selection4()
{
    BSLS_ASSERT(e_SELECTION_ID_SELECTION4 == d_selectionId);
    return *d_selection4;
}

inline
bdlt::DatetimeTz& FeatureTestMessage::selection5()
{
    BSLS_ASSERT(e_SELECTION_ID_SELECTION5 == d_selectionId);
    return d_selection5.object();
}

inline
CustomString& FeatureTestMessage::selection6()
{
    BSLS_ASSERT(e_SELECTION_ID_SELECTION6 == d_selectionId);
    return d_selection6.object();
}

inline
Enumerated::Value& FeatureTestMessage::selection7()
{
    BSLS_ASSERT(e_SELECTION_ID_SELECTION7 == d_selectionId);
    return d_selection7.object();
}

inline
Choice3& FeatureTestMessage::selection8()
{
    BSLS_ASSERT(e_SELECTION_ID_SELECTION8 == d_selectionId);
    return d_selection8.object();
}

inline
VoidSequence& FeatureTestMessage::selection9()
{
    BSLS_ASSERT(e_SELECTION_ID_SELECTION9 == d_selectionId);
    return d_selection9.object();
}

inline
UnsignedSequence& FeatureTestMessage::selection10()
{
    BSLS_ASSERT(e_SELECTION_ID_SELECTION10 == d_selectionId);
    return d_selection10.object();
}

inline
SequenceWithAnonymity& FeatureTestMessage::selection11()
{
    BSLS_ASSERT(e_SELECTION_ID_SELECTION11 == d_selectionId);
    return d_selection11.object();
}

// ACCESSORS
inline
int FeatureTestMessage::selectionId() const
{
    return d_selectionId;
}

template <class ACCESSOR>
int FeatureTestMessage::accessSelection(ACCESSOR& accessor) const
{
    switch (d_selectionId) {
      case e_SELECTION_ID_SELECTION1:
        return accessor(d_selection1.object(),
                SELECTION_INFO_ARRAY[e_SELECTION_INDEX_SELECTION1]);
      case e_SELECTION_ID_SELECTION2:
        return accessor(d_selection2.object(),
                SELECTION_INFO_ARRAY[e_SELECTION_INDEX_SELECTION2]);
      case e_SELECTION_ID_SELECTION3:
        return accessor(d_selection3.object(),
                SELECTION_INFO_ARRAY[e_SELECTION_INDEX_SELECTION3]);
      case e_SELECTION_ID_SELECTION4:
        return accessor(*d_selection4,
                SELECTION_INFO_ARRAY[e_SELECTION_INDEX_SELECTION4]);
      case e_SELECTION_ID_SELECTION5:
        return accessor(d_selection5.object(),
                SELECTION_INFO_ARRAY[e_SELECTION_INDEX_SELECTION5]);
      case e_SELECTION_ID_SELECTION6:
        return accessor(d_selection6.object(),
                SELECTION_INFO_ARRAY[e_SELECTION_INDEX_SELECTION6]);
      case e_SELECTION_ID_SELECTION7:
        return accessor(d_selection7.object(),
                SELECTION_INFO_ARRAY[e_SELECTION_INDEX_SELECTION7]);
      case e_SELECTION_ID_SELECTION8:
        return accessor(d_selection8.object(),
                SELECTION_INFO_ARRAY[e_SELECTION_INDEX_SELECTION8]);
      case e_SELECTION_ID_SELECTION9:
        return accessor(d_selection9.object(),
                SELECTION_INFO_ARRAY[e_SELECTION_INDEX_SELECTION9]);
      case e_SELECTION_ID_SELECTION10:
        return accessor(d_selection10.object(),
                SELECTION_INFO_ARRAY[e_SELECTION_INDEX_SELECTION10]);
      case e_SELECTION_ID_SELECTION11:
        return accessor(d_selection11.object(),
                SELECTION_INFO_ARRAY[e_SELECTION_INDEX_SELECTION11]);
      default:
        BSLS_ASSERT(e_SELECTION_ID_UNDEFINED == d_selectionId);
        return -1;                                                    // RETURN
    }
}

inline
const Sequence1& FeatureTestMessage::selection1() const
{
    BSLS_ASSERT(e_SELECTION_ID_SELECTION1 == d_selectionId);
    return d_selection1.object();
}

inline
const bsl::vector<char>& FeatureTestMessage::selection2() const
{
    BSLS_ASSERT(e_SELECTION_ID_SELECTION2 == d_selectionId);
    return d_selection2.object();
}

inline
const Sequence2& FeatureTestMessage::selection3() const
{
    BSLS_ASSERT(e_SELECTION_ID_SELECTION3 == d_selectionId);
    return d_selection3.object();
}

inline
const Sequence3& FeatureTestMessage::selection4() const
{
    BSLS_ASSERT(e_SELECTION_ID_SELECTION4 == d_selectionId);
    return *d_selection4;
}

inline
const bdlt::DatetimeTz& FeatureTestMessage::selection5() const
{
    BSLS_ASSERT(e_SELECTION_ID_SELECTION5 == d_selectionId);
    return d_selection5.object();
}

inline
const CustomString& FeatureTestMessage::selection6() const
{
    BSLS_ASSERT(e_SELECTION_ID_SELECTION6 == d_selectionId);
    return d_selection6.object();
}

inline
const Enumerated::Value& FeatureTestMessage::selection7() const
{
    BSLS_ASSERT(e_SELECTION_ID_SELECTION7 == d_selectionId);
    return d_selection7.object();
}

inline
const Choice3& FeatureTestMessage::selection8() const
{
    BSLS_ASSERT(e_SELECTION_ID_SELECTION8 == d_selectionId);
    return d_selection8.object();
}

inline
const VoidSequence& FeatureTestMessage::selection9() const
{
    BSLS_ASSERT(e_SELECTION_ID_SELECTION9 == d_selectionId);
    return d_selection9.object();
}

inline
const UnsignedSequence& FeatureTestMessage::selection10() const
{
    BSLS_ASSERT(e_SELECTION_ID_SELECTION10 == d_selectionId);
    return d_selection10.object();
}

inline
const SequenceWithAnonymity& FeatureTestMessage::selection11() const
{
    BSLS_ASSERT(e_SELECTION_ID_SELECTION11 == d_selectionId);
    return d_selection11.object();
}

inline
bool FeatureTestMessage::isSelection1Value() const
{
    return e_SELECTION_ID_SELECTION1 == d_selectionId;
}

inline
bool FeatureTestMessage::isSelection2Value() const
{
    return e_SELECTION_ID_SELECTION2 == d_selectionId;
}

inline
bool FeatureTestMessage::isSelection3Value() const
{
    return e_SELECTION_ID_SELECTION3 == d_selectionId;
}

inline
bool FeatureTestMessage::isSelection4Value() const
{
    return e_SELECTION_ID_SELECTION4 == d_selectionId;
}

inline
bool FeatureTestMessage::isSelection5Value() const
{
    return e_SELECTION_ID_SELECTION5 == d_selectionId;
}

inline
bool FeatureTestMessage::isSelection6Value() const
{
    return e_SELECTION_ID_SELECTION6 == d_selectionId;
}

inline
bool FeatureTestMessage::isSelection7Value() const
{
    return e_SELECTION_ID_SELECTION7 == d_selectionId;
}

inline
bool FeatureTestMessage::isSelection8Value() const
{
    return e_SELECTION_ID_SELECTION8 == d_selectionId;
}

inline
bool FeatureTestMessage::isSelection9Value() const
{
    return e_SELECTION_ID_SELECTION9 == d_selectionId;
}

inline
bool FeatureTestMessage::isSelection10Value() const
{
    return e_SELECTION_ID_SELECTION10 == d_selectionId;
}

inline
bool FeatureTestMessage::isSelection11Value() const
{
    return e_SELECTION_ID_SELECTION11 == d_selectionId;
}

inline
bool FeatureTestMessage::isUndefinedValue() const
{
    return e_SELECTION_ID_UNDEFINED == d_selectionId;
}


                               // -------------
                               // class Request
                               // -------------

// CREATORS
inline
Request::Request(bslma::Allocator *basicAllocator)
: d_selectionId(e_SELECTION_ID_UNDEFINED)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

inline
Request::~Request()
{
    reset();
}

// MANIPULATORS
template <class MANIPULATOR>
int Request::manipulateSelection(MANIPULATOR& manipulator)
{
    switch (d_selectionId) {
      case Request::e_SELECTION_ID_SIMPLE_REQUEST:
        return manipulator(&d_simpleRequest.object(),
                SELECTION_INFO_ARRAY[e_SELECTION_INDEX_SIMPLE_REQUEST]);
      case Request::e_SELECTION_ID_FEATURE_REQUEST:
        return manipulator(&d_featureRequest.object(),
                SELECTION_INFO_ARRAY[e_SELECTION_INDEX_FEATURE_REQUEST]);
      default:
        BSLS_ASSERT(Request::e_SELECTION_ID_UNDEFINED == d_selectionId);
        return -1;                                                    // RETURN
    }
}

inline
SimpleRequest& Request::simpleRequest()
{
    BSLS_ASSERT(e_SELECTION_ID_SIMPLE_REQUEST == d_selectionId);
    return d_simpleRequest.object();
}

inline
FeatureTestMessage& Request::featureRequest()
{
    BSLS_ASSERT(e_SELECTION_ID_FEATURE_REQUEST == d_selectionId);
    return d_featureRequest.object();
}

// ACCESSORS
inline
int Request::selectionId() const
{
    return d_selectionId;
}

template <class ACCESSOR>
int Request::accessSelection(ACCESSOR& accessor) const
{
    switch (d_selectionId) {
      case e_SELECTION_ID_SIMPLE_REQUEST:
        return accessor(d_simpleRequest.object(),
                SELECTION_INFO_ARRAY[e_SELECTION_INDEX_SIMPLE_REQUEST]);
      case e_SELECTION_ID_FEATURE_REQUEST:
        return accessor(d_featureRequest.object(),
                SELECTION_INFO_ARRAY[e_SELECTION_INDEX_FEATURE_REQUEST]);
      default:
        BSLS_ASSERT(e_SELECTION_ID_UNDEFINED == d_selectionId);
        return -1;                                                    // RETURN
    }
}

inline
const SimpleRequest& Request::simpleRequest() const
{
    BSLS_ASSERT(e_SELECTION_ID_SIMPLE_REQUEST == d_selectionId);
    return d_simpleRequest.object();
}

inline
const FeatureTestMessage& Request::featureRequest() const
{
    BSLS_ASSERT(e_SELECTION_ID_FEATURE_REQUEST == d_selectionId);
    return d_featureRequest.object();
}

inline
bool Request::isSimpleRequestValue() const
{
    return e_SELECTION_ID_SIMPLE_REQUEST == d_selectionId;
}

inline
bool Request::isFeatureRequestValue() const
{
    return e_SELECTION_ID_FEATURE_REQUEST == d_selectionId;
}

inline
bool Request::isUndefinedValue() const
{
    return e_SELECTION_ID_UNDEFINED == d_selectionId;
}


                               // --------------
                               // class Response
                               // --------------

// CREATORS
inline
Response::Response(bslma::Allocator *basicAllocator)
: d_selectionId(e_SELECTION_ID_UNDEFINED)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

inline
Response::~Response()
{
    reset();
}

// MANIPULATORS
template <class MANIPULATOR>
int Response::manipulateSelection(MANIPULATOR& manipulator)
{
    switch (d_selectionId) {
      case Response::e_SELECTION_ID_RESPONSE_DATA:
        return manipulator(&d_responseData.object(),
                SELECTION_INFO_ARRAY[e_SELECTION_INDEX_RESPONSE_DATA]);
      case Response::e_SELECTION_ID_FEATURE_RESPONSE:
        return manipulator(&d_featureResponse.object(),
                SELECTION_INFO_ARRAY[e_SELECTION_INDEX_FEATURE_RESPONSE]);
      default:
        BSLS_ASSERT(Response::e_SELECTION_ID_UNDEFINED == d_selectionId);
        return -1;                                                    // RETURN
    }
}

inline
bsl::string& Response::responseData()
{
    BSLS_ASSERT(e_SELECTION_ID_RESPONSE_DATA == d_selectionId);
    return d_responseData.object();
}

inline
FeatureTestMessage& Response::featureResponse()
{
    BSLS_ASSERT(e_SELECTION_ID_FEATURE_RESPONSE == d_selectionId);
    return d_featureResponse.object();
}

// ACCESSORS
inline
int Response::selectionId() const
{
    return d_selectionId;
}

template <class ACCESSOR>
int Response::accessSelection(ACCESSOR& accessor) const
{
    switch (d_selectionId) {
      case e_SELECTION_ID_RESPONSE_DATA:
        return accessor(d_responseData.object(),
                SELECTION_INFO_ARRAY[e_SELECTION_INDEX_RESPONSE_DATA]);
      case e_SELECTION_ID_FEATURE_RESPONSE:
        return accessor(d_featureResponse.object(),
                SELECTION_INFO_ARRAY[e_SELECTION_INDEX_FEATURE_RESPONSE]);
      default:
        BSLS_ASSERT(e_SELECTION_ID_UNDEFINED == d_selectionId);
        return -1;                                                    // RETURN
    }
}

inline
const bsl::string& Response::responseData() const
{
    BSLS_ASSERT(e_SELECTION_ID_RESPONSE_DATA == d_selectionId);
    return d_responseData.object();
}

inline
const FeatureTestMessage& Response::featureResponse() const
{
    BSLS_ASSERT(e_SELECTION_ID_FEATURE_RESPONSE == d_selectionId);
    return d_featureResponse.object();
}

inline
bool Response::isResponseDataValue() const
{
    return e_SELECTION_ID_RESPONSE_DATA == d_selectionId;
}

inline
bool Response::isFeatureResponseValue() const
{
    return e_SELECTION_ID_FEATURE_RESPONSE == d_selectionId;
}

inline
bool Response::isUndefinedValue() const
{
    return e_SELECTION_ID_UNDEFINED == d_selectionId;
}
}  // close package namespace

// FREE FUNCTIONS

inline
bool balb::operator==(
        const balb::CustomInt& lhs,
        const balb::CustomInt& rhs)
{
    return lhs.d_value == rhs.d_value;
}

inline
bool balb::operator!=(
        const balb::CustomInt& lhs,
        const balb::CustomInt& rhs)
{
    return lhs.d_value != rhs.d_value;
}

inline
bsl::ostream& balb::operator<<(
        bsl::ostream& stream,
        const balb::CustomInt& rhs)
{
    return rhs.print(stream, 0, -1);
}


inline
bool balb::operator==(
        const balb::CustomString& lhs,
        const balb::CustomString& rhs)
{
    return lhs.d_value == rhs.d_value;
}

inline
bool balb::operator!=(
        const balb::CustomString& lhs,
        const balb::CustomString& rhs)
{
    return lhs.d_value != rhs.d_value;
}

inline
bsl::ostream& balb::operator<<(
        bsl::ostream& stream,
        const balb::CustomString& rhs)
{
    return rhs.print(stream, 0, -1);
}


inline
bsl::ostream& balb::operator<<(
        bsl::ostream& stream,
        balb::Enumerated::Value rhs)
{
    return balb::Enumerated::print(stream, rhs);
}


inline
bool balb::operator==(
        const balb::SequenceWithAnonymityChoice1& lhs,
        const balb::SequenceWithAnonymityChoice1& rhs)
{
    typedef balb::SequenceWithAnonymityChoice1 Class;
    if (lhs.selectionId() == rhs.selectionId()) {
        switch (rhs.selectionId()) {
          case Class::e_SELECTION_ID_SELECTION5:
            return lhs.selection5() == rhs.selection5();
          case Class::e_SELECTION_ID_SELECTION6:
            return lhs.selection6() == rhs.selection6();
          default:
            BSLS_ASSERT(Class::e_SELECTION_ID_UNDEFINED == rhs.selectionId());
            return true;                                              // RETURN
        }
    }
    else {
        return false;                                                 // RETURN
   }
}

inline
bool balb::operator!=(
        const balb::SequenceWithAnonymityChoice1& lhs,
        const balb::SequenceWithAnonymityChoice1& rhs)
{
    return !(lhs == rhs);
}

inline
bsl::ostream& balb::operator<<(
        bsl::ostream& stream,
        const balb::SequenceWithAnonymityChoice1& rhs)
{
    return rhs.print(stream, 0, -1);
}


inline
bool balb::operator==(
        const balb::SimpleRequest& lhs,
        const balb::SimpleRequest& rhs)
{
    return  lhs.data() == rhs.data()
         && lhs.responseLength() == rhs.responseLength();
}

inline
bool balb::operator!=(
        const balb::SimpleRequest& lhs,
        const balb::SimpleRequest& rhs)
{
    return  lhs.data() != rhs.data()
         || lhs.responseLength() != rhs.responseLength();
}

inline
bsl::ostream& balb::operator<<(
        bsl::ostream& stream,
        const balb::SimpleRequest& rhs)
{
    return rhs.print(stream, 0, -1);
}


inline
bool balb::operator==(
        const balb::UnsignedSequence& lhs,
        const balb::UnsignedSequence& rhs)
{
    return  lhs.element1() == rhs.element1()
         && lhs.element2() == rhs.element2()
         && lhs.element3() == rhs.element3();
}

inline
bool balb::operator!=(
        const balb::UnsignedSequence& lhs,
        const balb::UnsignedSequence& rhs)
{
    return  lhs.element1() != rhs.element1()
         || lhs.element2() != rhs.element2()
         || lhs.element3() != rhs.element3();
}

inline
bsl::ostream& balb::operator<<(
        bsl::ostream& stream,
        const balb::UnsignedSequence& rhs)
{
    return rhs.print(stream, 0, -1);
}


inline
bool balb::operator==(
        const balb::VoidSequence&,
        const balb::VoidSequence&)
{
    return true;
}

inline
bool balb::operator!=(
        const balb::VoidSequence&,
        const balb::VoidSequence&)
{
    return false;
}

inline
bsl::ostream& balb::operator<<(
        bsl::ostream& stream,
        const balb::VoidSequence& rhs)
{
    return rhs.print(stream, 0, -1);
}


inline
bool balb::operator==(
        const balb::Sequence3& lhs,
        const balb::Sequence3& rhs)
{
    return  lhs.element1() == rhs.element1()
         && lhs.element2() == rhs.element2()
         && lhs.element3() == rhs.element3()
         && lhs.element4() == rhs.element4()
         && lhs.element5() == rhs.element5()
         && lhs.element6() == rhs.element6();
}

inline
bool balb::operator!=(
        const balb::Sequence3& lhs,
        const balb::Sequence3& rhs)
{
    return  lhs.element1() != rhs.element1()
         || lhs.element2() != rhs.element2()
         || lhs.element3() != rhs.element3()
         || lhs.element4() != rhs.element4()
         || lhs.element5() != rhs.element5()
         || lhs.element6() != rhs.element6();
}

inline
bsl::ostream& balb::operator<<(
        bsl::ostream& stream,
        const balb::Sequence3& rhs)
{
    return rhs.print(stream, 0, -1);
}


inline
bool balb::operator==(
        const balb::Sequence5& lhs,
        const balb::Sequence5& rhs)
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
bool balb::operator!=(
        const balb::Sequence5& lhs,
        const balb::Sequence5& rhs)
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
bsl::ostream& balb::operator<<(
        bsl::ostream& stream,
        const balb::Sequence5& rhs)
{
    return rhs.print(stream, 0, -1);
}


inline
bool balb::operator==(
        const balb::Sequence6& lhs,
        const balb::Sequence6& rhs)
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
         && lhs.element15() == rhs.element15();
}

inline
bool balb::operator!=(
        const balb::Sequence6& lhs,
        const balb::Sequence6& rhs)
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
         || lhs.element15() != rhs.element15();
}

inline
bsl::ostream& balb::operator<<(
        bsl::ostream& stream,
        const balb::Sequence6& rhs)
{
    return rhs.print(stream, 0, -1);
}


inline
bool balb::operator==(
        const balb::Choice3& lhs,
        const balb::Choice3& rhs)
{
    typedef balb::Choice3 Class;
    if (lhs.selectionId() == rhs.selectionId()) {
        switch (rhs.selectionId()) {
          case Class::e_SELECTION_ID_SELECTION1:
            return lhs.selection1() == rhs.selection1();
          case Class::e_SELECTION_ID_SELECTION2:
            return lhs.selection2() == rhs.selection2();
          case Class::e_SELECTION_ID_SELECTION3:
            return lhs.selection3() == rhs.selection3();
          case Class::e_SELECTION_ID_SELECTION4:
            return lhs.selection4() == rhs.selection4();
          default:
            BSLS_ASSERT(Class::e_SELECTION_ID_UNDEFINED == rhs.selectionId());
            return true;                                              // RETURN
        }
    }
    else {
        return false;                                                 // RETURN
   }
}

inline
bool balb::operator!=(
        const balb::Choice3& lhs,
        const balb::Choice3& rhs)
{
    return !(lhs == rhs);
}

inline
bsl::ostream& balb::operator<<(
        bsl::ostream& stream,
        const balb::Choice3& rhs)
{
    return rhs.print(stream, 0, -1);
}


inline
bool balb::operator==(
        const balb::SequenceWithAnonymityChoice& lhs,
        const balb::SequenceWithAnonymityChoice& rhs)
{
    typedef balb::SequenceWithAnonymityChoice Class;
    if (lhs.selectionId() == rhs.selectionId()) {
        switch (rhs.selectionId()) {
          case Class::e_SELECTION_ID_SELECTION1:
            return lhs.selection1() == rhs.selection1();
          case Class::e_SELECTION_ID_SELECTION2:
            return lhs.selection2() == rhs.selection2();
          case Class::e_SELECTION_ID_SELECTION3:
            return lhs.selection3() == rhs.selection3();
          case Class::e_SELECTION_ID_SELECTION4:
            return lhs.selection4() == rhs.selection4();
          default:
            BSLS_ASSERT(Class::e_SELECTION_ID_UNDEFINED == rhs.selectionId());
            return true;                                              // RETURN
        }
    }
    else {
        return false;                                                 // RETURN
   }
}

inline
bool balb::operator!=(
        const balb::SequenceWithAnonymityChoice& lhs,
        const balb::SequenceWithAnonymityChoice& rhs)
{
    return !(lhs == rhs);
}

inline
bsl::ostream& balb::operator<<(
        bsl::ostream& stream,
        const balb::SequenceWithAnonymityChoice& rhs)
{
    return rhs.print(stream, 0, -1);
}


inline
bool balb::operator==(
        const balb::Choice1& lhs,
        const balb::Choice1& rhs)
{
    typedef balb::Choice1 Class;
    if (lhs.selectionId() == rhs.selectionId()) {
        switch (rhs.selectionId()) {
          case Class::e_SELECTION_ID_SELECTION1:
            return lhs.selection1() == rhs.selection1();
          case Class::e_SELECTION_ID_SELECTION2:
            return lhs.selection2() == rhs.selection2();
          case Class::e_SELECTION_ID_SELECTION3:
            return lhs.selection3() == rhs.selection3();
          case Class::e_SELECTION_ID_SELECTION4:
            return lhs.selection4() == rhs.selection4();
          default:
            BSLS_ASSERT(Class::e_SELECTION_ID_UNDEFINED == rhs.selectionId());
            return true;                                              // RETURN
        }
    }
    else {
        return false;                                                 // RETURN
   }
}

inline
bool balb::operator!=(
        const balb::Choice1& lhs,
        const balb::Choice1& rhs)
{
    return !(lhs == rhs);
}

inline
bsl::ostream& balb::operator<<(
        bsl::ostream& stream,
        const balb::Choice1& rhs)
{
    return rhs.print(stream, 0, -1);
}


inline
bool balb::operator==(
        const balb::Choice2& lhs,
        const balb::Choice2& rhs)
{
    typedef balb::Choice2 Class;
    if (lhs.selectionId() == rhs.selectionId()) {
        switch (rhs.selectionId()) {
          case Class::e_SELECTION_ID_SELECTION1:
            return lhs.selection1() == rhs.selection1();
          case Class::e_SELECTION_ID_SELECTION2:
            return lhs.selection2() == rhs.selection2();
          case Class::e_SELECTION_ID_SELECTION3:
            return lhs.selection3() == rhs.selection3();
          case Class::e_SELECTION_ID_SELECTION4:
            return lhs.selection4() == rhs.selection4();
          default:
            BSLS_ASSERT(Class::e_SELECTION_ID_UNDEFINED == rhs.selectionId());
            return true;                                              // RETURN
        }
    }
    else {
        return false;                                                 // RETURN
   }
}

inline
bool balb::operator!=(
        const balb::Choice2& lhs,
        const balb::Choice2& rhs)
{
    return !(lhs == rhs);
}

inline
bsl::ostream& balb::operator<<(
        bsl::ostream& stream,
        const balb::Choice2& rhs)
{
    return rhs.print(stream, 0, -1);
}


inline
bool balb::operator==(
        const balb::Sequence4& lhs,
        const balb::Sequence4& rhs)
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
bool balb::operator!=(
        const balb::Sequence4& lhs,
        const balb::Sequence4& rhs)
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
bsl::ostream& balb::operator<<(
        bsl::ostream& stream,
        const balb::Sequence4& rhs)
{
    return rhs.print(stream, 0, -1);
}


inline
bool balb::operator==(
        const balb::Sequence1& lhs,
        const balb::Sequence1& rhs)
{
    return  lhs.element1() == rhs.element1()
         && lhs.element2() == rhs.element2()
         && lhs.element3() == rhs.element3()
         && lhs.element4() == rhs.element4()
         && lhs.element5() == rhs.element5();
}

inline
bool balb::operator!=(
        const balb::Sequence1& lhs,
        const balb::Sequence1& rhs)
{
    return  lhs.element1() != rhs.element1()
         || lhs.element2() != rhs.element2()
         || lhs.element3() != rhs.element3()
         || lhs.element4() != rhs.element4()
         || lhs.element5() != rhs.element5();
}

inline
bsl::ostream& balb::operator<<(
        bsl::ostream& stream,
        const balb::Sequence1& rhs)
{
    return rhs.print(stream, 0, -1);
}


inline
bool balb::operator==(
        const balb::Sequence2& lhs,
        const balb::Sequence2& rhs)
{
    return  lhs.element1() == rhs.element1()
         && lhs.element2() == rhs.element2()
         && lhs.element3() == rhs.element3()
         && lhs.element4() == rhs.element4()
         && lhs.element5() == rhs.element5();
}

inline
bool balb::operator!=(
        const balb::Sequence2& lhs,
        const balb::Sequence2& rhs)
{
    return  lhs.element1() != rhs.element1()
         || lhs.element2() != rhs.element2()
         || lhs.element3() != rhs.element3()
         || lhs.element4() != rhs.element4()
         || lhs.element5() != rhs.element5();
}

inline
bsl::ostream& balb::operator<<(
        bsl::ostream& stream,
        const balb::Sequence2& rhs)
{
    return rhs.print(stream, 0, -1);
}


inline
bool balb::operator==(
        const balb::SequenceWithAnonymityChoice2& lhs,
        const balb::SequenceWithAnonymityChoice2& rhs)
{
    typedef balb::SequenceWithAnonymityChoice2 Class;
    if (lhs.selectionId() == rhs.selectionId()) {
        switch (rhs.selectionId()) {
          case Class::e_SELECTION_ID_SELECTION7:
            return lhs.selection7() == rhs.selection7();
          case Class::e_SELECTION_ID_SELECTION8:
            return lhs.selection8() == rhs.selection8();
          default:
            BSLS_ASSERT(Class::e_SELECTION_ID_UNDEFINED == rhs.selectionId());
            return true;                                              // RETURN
        }
    }
    else {
        return false;                                                 // RETURN
   }
}

inline
bool balb::operator!=(
        const balb::SequenceWithAnonymityChoice2& lhs,
        const balb::SequenceWithAnonymityChoice2& rhs)
{
    return !(lhs == rhs);
}

inline
bsl::ostream& balb::operator<<(
        bsl::ostream& stream,
        const balb::SequenceWithAnonymityChoice2& rhs)
{
    return rhs.print(stream, 0, -1);
}


inline
bool balb::operator==(
        const balb::SequenceWithAnonymity& lhs,
        const balb::SequenceWithAnonymity& rhs)
{
    return  lhs.choice() == rhs.choice()
         && lhs.choice1() == rhs.choice1()
         && lhs.choice2() == rhs.choice2()
         && lhs.element4() == rhs.element4();
}

inline
bool balb::operator!=(
        const balb::SequenceWithAnonymity& lhs,
        const balb::SequenceWithAnonymity& rhs)
{
    return  lhs.choice() != rhs.choice()
         || lhs.choice1() != rhs.choice1()
         || lhs.choice2() != rhs.choice2()
         || lhs.element4() != rhs.element4();
}

inline
bsl::ostream& balb::operator<<(
        bsl::ostream& stream,
        const balb::SequenceWithAnonymity& rhs)
{
    return rhs.print(stream, 0, -1);
}


inline
bool balb::operator==(
        const balb::FeatureTestMessage& lhs,
        const balb::FeatureTestMessage& rhs)
{
    typedef balb::FeatureTestMessage Class;
    if (lhs.selectionId() == rhs.selectionId()) {
        switch (rhs.selectionId()) {
          case Class::e_SELECTION_ID_SELECTION1:
            return lhs.selection1() == rhs.selection1();
          case Class::e_SELECTION_ID_SELECTION2:
            return lhs.selection2() == rhs.selection2();
          case Class::e_SELECTION_ID_SELECTION3:
            return lhs.selection3() == rhs.selection3();
          case Class::e_SELECTION_ID_SELECTION4:
            return lhs.selection4() == rhs.selection4();
          case Class::e_SELECTION_ID_SELECTION5:
            return lhs.selection5() == rhs.selection5();
          case Class::e_SELECTION_ID_SELECTION6:
            return lhs.selection6() == rhs.selection6();
          case Class::e_SELECTION_ID_SELECTION7:
            return lhs.selection7() == rhs.selection7();
          case Class::e_SELECTION_ID_SELECTION8:
            return lhs.selection8() == rhs.selection8();
          case Class::e_SELECTION_ID_SELECTION9:
            return lhs.selection9() == rhs.selection9();
          case Class::e_SELECTION_ID_SELECTION10:
            return lhs.selection10() == rhs.selection10();
          case Class::e_SELECTION_ID_SELECTION11:
            return lhs.selection11() == rhs.selection11();
          default:
            BSLS_ASSERT(Class::e_SELECTION_ID_UNDEFINED == rhs.selectionId());
            return true;                                              // RETURN
        }
    }
    else {
        return false;                                                 // RETURN
   }
}

inline
bool balb::operator!=(
        const balb::FeatureTestMessage& lhs,
        const balb::FeatureTestMessage& rhs)
{
    return !(lhs == rhs);
}

inline
bsl::ostream& balb::operator<<(
        bsl::ostream& stream,
        const balb::FeatureTestMessage& rhs)
{
    return rhs.print(stream, 0, -1);
}


inline
bool balb::operator==(
        const balb::Request& lhs,
        const balb::Request& rhs)
{
    typedef balb::Request Class;
    if (lhs.selectionId() == rhs.selectionId()) {
        switch (rhs.selectionId()) {
          case Class::e_SELECTION_ID_SIMPLE_REQUEST:
            return lhs.simpleRequest() == rhs.simpleRequest();
          case Class::e_SELECTION_ID_FEATURE_REQUEST:
            return lhs.featureRequest() == rhs.featureRequest();
          default:
            BSLS_ASSERT(Class::e_SELECTION_ID_UNDEFINED == rhs.selectionId());
            return true;                                              // RETURN
        }
    }
    else {
        return false;                                                 // RETURN
   }
}

inline
bool balb::operator!=(
        const balb::Request& lhs,
        const balb::Request& rhs)
{
    return !(lhs == rhs);
}

inline
bsl::ostream& balb::operator<<(
        bsl::ostream& stream,
        const balb::Request& rhs)
{
    return rhs.print(stream, 0, -1);
}


inline
bool balb::operator==(
        const balb::Response& lhs,
        const balb::Response& rhs)
{
    typedef balb::Response Class;
    if (lhs.selectionId() == rhs.selectionId()) {
        switch (rhs.selectionId()) {
          case Class::e_SELECTION_ID_RESPONSE_DATA:
            return lhs.responseData() == rhs.responseData();
          case Class::e_SELECTION_ID_FEATURE_RESPONSE:
            return lhs.featureResponse() == rhs.featureResponse();
          default:
            BSLS_ASSERT(Class::e_SELECTION_ID_UNDEFINED == rhs.selectionId());
            return true;                                              // RETURN
        }
    }
    else {
        return false;                                                 // RETURN
   }
}

inline
bool balb::operator!=(
        const balb::Response& lhs,
        const balb::Response& rhs)
{
    return !(lhs == rhs);
}

inline
bsl::ostream& balb::operator<<(
        bsl::ostream& stream,
        const balb::Response& rhs)
{
    return rhs.print(stream, 0, -1);
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
