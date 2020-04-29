// baljsn_encoder_testtypes.h        *DO NOT EDIT*         @generated -*-C++-*-
#ifndef INCLUDED_BALJSN_ENCODER_TESTTYPES
#define INCLUDED_BALJSN_ENCODER_TESTTYPES

#include <bsls_ident.h>
BSLS_IDENT_RCSID(baljsn_encoder_testtypes_h,"$Id$ $CSID$")
BSLS_IDENT_PRAGMA_ONCE

//@PURPOSE: Provide value-semantic attribute classes

#include <bslalg_typetraits.h>

#include <bdlat_attributeinfo.h>

#include <bdlat_enumeratorinfo.h>

#include <bdlat_selectioninfo.h>

#include <bdlat_typetraits.h>

#include <bdlat_valuetypefunctions.h>

#include <bslh_hash.h>
#include <bsls_objectbuffer.h>

#include <bslma_default.h>

#include <bsls_assert.h>

#include <bdlb_nullablevalue.h>

#include <bsl_string.h>

#include <bsl_vector.h>

#include <bsl_iosfwd.h>
#include <bsl_limits.h>

#include <bsl_ostream.h>
#include <bsl_string.h>

namespace BloombergLP {

namespace bslma { class Allocator; }

namespace baljsn { class EncoderTestAddress; }
namespace baljsn { class EncoderTestChoiceWithAllCategoriesChoice; }
namespace baljsn { class EncoderTestChoiceWithAllCategoriesCustomizedType; }
namespace baljsn { class EncoderTestChoiceWithAllCategoriesSequence; }
namespace baljsn { class EncoderTestDegenerateChoice1Sequence; }
namespace baljsn { class EncoderTestSequenceWithAllCategoriesChoice; }
namespace baljsn { class EncoderTestSequenceWithAllCategoriesCustomizedType; }
namespace baljsn { class EncoderTestSequenceWithAllCategoriesSequence; }
namespace baljsn { class EncoderTestSequenceWithUntagged0; }
namespace baljsn { class EncoderTestSequenceWithUntagged10Sequence; }
namespace baljsn { class EncoderTestSequenceWithUntagged11Sequence; }
namespace baljsn { class EncoderTestSequenceWithUntagged11Sequence1; }
namespace baljsn { class EncoderTestSequenceWithUntagged12Sequence; }
namespace baljsn { class EncoderTestSequenceWithUntagged13Sequence; }
namespace baljsn { class EncoderTestSequenceWithUntagged14; }
namespace baljsn { class EncoderTestSequenceWithUntagged1Sequence; }
namespace baljsn { class EncoderTestSequenceWithUntagged2; }
namespace baljsn { class EncoderTestSequenceWithUntagged3Sequence; }
namespace baljsn { class EncoderTestSequenceWithUntagged3Sequence1; }
namespace baljsn { class EncoderTestSequenceWithUntagged4Sequence; }
namespace baljsn { class EncoderTestSequenceWithUntagged5Sequence; }
namespace baljsn { class EncoderTestSequenceWithUntagged6; }
namespace baljsn { class EncoderTestSequenceWithUntagged7Sequence; }
namespace baljsn { class EncoderTestSequenceWithUntagged7Sequence1; }
namespace baljsn { class EncoderTestSequenceWithUntagged7Sequence2; }
namespace baljsn { class EncoderTestSequenceWithUntagged8Sequence; }
namespace baljsn { class EncoderTestSequenceWithUntagged8Sequence1; }
namespace baljsn { class EncoderTestSequenceWithUntagged9Sequence; }
namespace baljsn { class EncoderTestSequenceWithUntagged9Sequence1; }
namespace baljsn { class EncoderTestChoiceWithAllCategories; }
namespace baljsn { class EncoderTestDegenerateChoice1; }
namespace baljsn { class EncoderTestEmployee; }
namespace baljsn { class EncoderTestSequenceWithAllCategories; }
namespace baljsn { class EncoderTestSequenceWithUntagged1; }
namespace baljsn { class EncoderTestSequenceWithUntagged10; }
namespace baljsn { class EncoderTestSequenceWithUntagged11; }
namespace baljsn { class EncoderTestSequenceWithUntagged12; }
namespace baljsn { class EncoderTestSequenceWithUntagged13; }
namespace baljsn { class EncoderTestSequenceWithUntagged3; }
namespace baljsn { class EncoderTestSequenceWithUntagged4; }
namespace baljsn { class EncoderTestSequenceWithUntagged5; }
namespace baljsn { class EncoderTestSequenceWithUntagged7; }
namespace baljsn { class EncoderTestSequenceWithUntagged8; }
namespace baljsn { class EncoderTestSequenceWithUntagged9; }
namespace baljsn {

                          // ========================
                          // class EncoderTestAddress
                          // ========================

class EncoderTestAddress {

    // INSTANCE DATA
    bsl::string  d_street;
    bsl::string  d_city;
    bsl::string  d_state;

  public:
    // TYPES
    enum {
        ATTRIBUTE_ID_STREET = 0
      , ATTRIBUTE_ID_CITY   = 1
      , ATTRIBUTE_ID_STATE  = 2
    };

    enum {
        NUM_ATTRIBUTES = 3
    };

    enum {
        ATTRIBUTE_INDEX_STREET = 0
      , ATTRIBUTE_INDEX_CITY   = 1
      , ATTRIBUTE_INDEX_STATE  = 2
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
    explicit EncoderTestAddress(bslma::Allocator *basicAllocator = 0);
        // Create an object of type 'EncoderTestAddress' having the default
        // value.  Use the optionally specified 'basicAllocator' to supply
        // memory.  If 'basicAllocator' is 0, the currently installed default
        // allocator is used.

    EncoderTestAddress(const EncoderTestAddress& original,
                       bslma::Allocator *basicAllocator = 0);
        // Create an object of type 'EncoderTestAddress' having the value of
        // the specified 'original' object.  Use the optionally specified
        // 'basicAllocator' to supply memory.  If 'basicAllocator' is 0, the
        // currently installed default allocator is used.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    EncoderTestAddress(EncoderTestAddress&& original) noexcept;
        // Create an object of type 'EncoderTestAddress' having the value of
        // the specified 'original' object.  After performing this action, the
        // 'original' object will be left in a valid, but unspecified state.

    EncoderTestAddress(EncoderTestAddress&& original,
                       bslma::Allocator *basicAllocator);
        // Create an object of type 'EncoderTestAddress' having the value of
        // the specified 'original' object.  After performing this action, the
        // 'original' object will be left in a valid, but unspecified state.
        // Use the optionally specified 'basicAllocator' to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.
#endif

    ~EncoderTestAddress();
        // Destroy this object.

    // MANIPULATORS
    EncoderTestAddress& operator=(const EncoderTestAddress& rhs);
        // Assign to this object the value of the specified 'rhs' object.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    EncoderTestAddress& operator=(EncoderTestAddress&& rhs);
        // Assign to this object the value of the specified 'rhs' object.
        // After performing this action, the 'rhs' object will be left in a
        // valid, but unspecified state.
#endif

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

    bsl::string& street();
        // Return a reference to the modifiable "Street" attribute of this
        // object.

    bsl::string& city();
        // Return a reference to the modifiable "City" attribute of this
        // object.

    bsl::string& state();
        // Return a reference to the modifiable "State" attribute of this
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

    const bsl::string& street() const;
        // Return a reference to the non-modifiable "Street" attribute of this
        // object.

    const bsl::string& city() const;
        // Return a reference to the non-modifiable "City" attribute of this
        // object.

    const bsl::string& state() const;
        // Return a reference to the non-modifiable "State" attribute of this
        // object.
};

// FREE OPERATORS
inline
bool operator==(const EncoderTestAddress& lhs, const EncoderTestAddress& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
    // the same value, and 'false' otherwise.  Two attribute objects have the
    // same value if each respective attribute has the same value.

inline
bool operator!=(const EncoderTestAddress& lhs, const EncoderTestAddress& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
    // have the same value, and 'false' otherwise.  Two attribute objects do
    // not have the same value if one or more respective attributes differ in
    // values.

inline
bsl::ostream& operator<<(bsl::ostream& stream, const EncoderTestAddress& rhs);
    // Format the specified 'rhs' to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& hashAlg, const baljsn::EncoderTestAddress& object);
    // Pass the specified 'object' to the specified 'hashAlg'.  This function
    // integrates with the 'bslh' modular hashing system and effectively
    // provides a 'bsl::hash' specialization for 'EncoderTestAddress'.

}  // close package namespace

// TRAITS

BDLAT_DECL_SEQUENCE_WITH_ALLOCATOR_BITWISEMOVEABLE_TRAITS(baljsn::EncoderTestAddress)

namespace baljsn {

               // ==============================================
               // class EncoderTestChoiceWithAllCategoriesChoice
               // ==============================================

class EncoderTestChoiceWithAllCategoriesChoice {

    // INSTANCE DATA
    union {
        bsls::ObjectBuffer< int > d_selection0;
    };

    int                           d_selectionId;

  public:
    // TYPES

    enum {
        SELECTION_ID_UNDEFINED  = -1
      , SELECTION_ID_SELECTION0 = 0
    };

    enum {
        NUM_SELECTIONS = 1
    };

    enum {
        SELECTION_INDEX_SELECTION0 = 0
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
    EncoderTestChoiceWithAllCategoriesChoice();
        // Create an object of type 'EncoderTestChoiceWithAllCategoriesChoice'
        // having the default value.

    EncoderTestChoiceWithAllCategoriesChoice(const EncoderTestChoiceWithAllCategoriesChoice& original);
        // Create an object of type 'EncoderTestChoiceWithAllCategoriesChoice'
        // having the value of the specified 'original' object.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    EncoderTestChoiceWithAllCategoriesChoice(EncoderTestChoiceWithAllCategoriesChoice&& original) noexcept;
        // Create an object of type 'EncoderTestChoiceWithAllCategoriesChoice'
        // having the value of the specified 'original' object.  After
        // performing this action, the 'original' object will be left in a
        // valid, but unspecified state.
#endif

    ~EncoderTestChoiceWithAllCategoriesChoice();
        // Destroy this object.

    // MANIPULATORS
    EncoderTestChoiceWithAllCategoriesChoice& operator=(const EncoderTestChoiceWithAllCategoriesChoice& rhs);
        // Assign to this object the value of the specified 'rhs' object.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    EncoderTestChoiceWithAllCategoriesChoice& operator=(EncoderTestChoiceWithAllCategoriesChoice&& rhs);
        // Assign to this object the value of the specified 'rhs' object.
        // After performing this action, the 'rhs' object will be left in a
        // valid, but unspecified state.
#endif

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

    int& makeSelection0();
    int& makeSelection0(int value);
        // Set the value of this object to be a "Selection0" value.  Optionally
        // specify the 'value' of the "Selection0".  If 'value' is not
        // specified, the default "Selection0" value is used.

    template<class MANIPULATOR>
    int manipulateSelection(MANIPULATOR& manipulator);
        // Invoke the specified 'manipulator' on the address of the modifiable
        // selection, supplying 'manipulator' with the corresponding selection
        // information structure.  Return the value returned from the
        // invocation of 'manipulator' if this object has a defined selection,
        // and -1 otherwise.

    int& selection0();
        // Return a reference to the modifiable "Selection0" selection of this
        // object if "Selection0" is the current selection.  The behavior is
        // undefined unless "Selection0" is the selection of this object.

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

    const int& selection0() const;
        // Return a reference to the non-modifiable "Selection0" selection of
        // this object if "Selection0" is the current selection.  The behavior
        // is undefined unless "Selection0" is the selection of this object.

    bool isSelection0Value() const;
        // Return 'true' if the value of this object is a "Selection0" value,
        // and return 'false' otherwise.

    bool isUndefinedValue() const;
        // Return 'true' if the value of this object is undefined, and 'false'
        // otherwise.

    const char *selectionName() const;
        // Return the symbolic name of the current selection of this object.
};

// FREE OPERATORS
inline
bool operator==(const EncoderTestChoiceWithAllCategoriesChoice& lhs, const EncoderTestChoiceWithAllCategoriesChoice& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects have the same
    // value, and 'false' otherwise.  Two 'EncoderTestChoiceWithAllCategoriesChoice' objects have the same
    // value if either the selections in both objects have the same ids and
    // the same values, or both selections are undefined.

inline
bool operator!=(const EncoderTestChoiceWithAllCategoriesChoice& lhs, const EncoderTestChoiceWithAllCategoriesChoice& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects do not have the
    // same values, as determined by 'operator==', and 'false' otherwise.

inline
bsl::ostream& operator<<(bsl::ostream& stream, const EncoderTestChoiceWithAllCategoriesChoice& rhs);
    // Format the specified 'rhs' to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& hashAlg, const baljsn::EncoderTestChoiceWithAllCategoriesChoice& object);
    // Pass the specified 'object' to the specified 'hashAlg'.  This function
    // integrates with the 'bslh' modular hashing system and effectively
    // provides a 'bsl::hash' specialization for 'EncoderTestChoiceWithAllCategoriesChoice'.

}  // close package namespace

// TRAITS

BDLAT_DECL_CHOICE_WITH_BITWISEMOVEABLE_TRAITS(baljsn::EncoderTestChoiceWithAllCategoriesChoice)

namespace baljsn {

           // ======================================================
           // class EncoderTestChoiceWithAllCategoriesCustomizedType
           // ======================================================

class EncoderTestChoiceWithAllCategoriesCustomizedType {

    // INSTANCE DATA
    bsl::string d_value;

    // FRIENDS
    friend bool operator==(const EncoderTestChoiceWithAllCategoriesCustomizedType& lhs, const EncoderTestChoiceWithAllCategoriesCustomizedType& rhs);
    friend bool operator!=(const EncoderTestChoiceWithAllCategoriesCustomizedType& lhs, const EncoderTestChoiceWithAllCategoriesCustomizedType& rhs);

  public:
    // TYPES
    typedef bsl::string BaseType;

    // CONSTANTS
    static const char CLASS_NAME[];

    // CREATORS
    explicit EncoderTestChoiceWithAllCategoriesCustomizedType(bslma::Allocator *basicAllocator = 0);
        // Create an object of type
        // 'EncoderTestChoiceWithAllCategoriesCustomizedType' having the
        // default value.  Use the optionally specified 'basicAllocator' to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.

    EncoderTestChoiceWithAllCategoriesCustomizedType(const EncoderTestChoiceWithAllCategoriesCustomizedType& original,
                                                    bslma::Allocator *basicAllocator = 0);
        // Create an object of type 'EncoderTestChoiceWithAllCategoriesCustomizedType' having the value
        // of the specified 'original' object.  Use the optionally specified
        // 'basicAllocator' to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    EncoderTestChoiceWithAllCategoriesCustomizedType(EncoderTestChoiceWithAllCategoriesCustomizedType&& original) = default;
        // Create an object of type
        // 'EncoderTestChoiceWithAllCategoriesCustomizedType' having the value
        // of the specified 'original' object.  After performing this action,
        // the 'original' object will be left in a valid, but unspecified
        // state.

    EncoderTestChoiceWithAllCategoriesCustomizedType(EncoderTestChoiceWithAllCategoriesCustomizedType&& original,
                                                    bslma::Allocator *basicAllocator);
        // Create an object of type
        // 'EncoderTestChoiceWithAllCategoriesCustomizedType' having the value
        // of the specified 'original' object.  After performing this action,
        // the 'original' object will be left in a valid, but unspecified
        // state.  Use the optionally specified 'basicAllocator' to supply
        // memory.  If 'basicAllocator' is 0, the currently installed default
        // allocator is used.
#endif

    explicit EncoderTestChoiceWithAllCategoriesCustomizedType(const bsl::string& value,
                                                             bslma::Allocator *basicAllocator = 0);
        // Create an object of type
        // 'EncoderTestChoiceWithAllCategoriesCustomizedType' having the
        // specified 'value'.  Use the optionally specified 'basicAllocator' to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.

    ~EncoderTestChoiceWithAllCategoriesCustomizedType();
        // Destroy this object.

    // MANIPULATORS
    EncoderTestChoiceWithAllCategoriesCustomizedType& operator=(const EncoderTestChoiceWithAllCategoriesCustomizedType& rhs);
        // Assign to this object the value of the specified 'rhs' object.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    EncoderTestChoiceWithAllCategoriesCustomizedType& operator=(EncoderTestChoiceWithAllCategoriesCustomizedType&& rhs) = default;
        // Assign to this object the value of the specified 'rhs' object.
        // After performing this action, the 'rhs' object will be left in a
        // valid, but unspecified state.
#endif

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

    // PUBLIC CLASS METHODS
    static int checkRestrictions(const bsl::string& value);
        // Check if the specified 'value' satisfies the restrictions of this
        // class (i.e., "EncoderTestChoiceWithAllCategoriesCustomizedType").
        // Return 0 if successful (i.e., the restrictions are satisfied) and
        // non-zero otherwise.
};

// FREE OPERATORS
inline
bool operator==(const EncoderTestChoiceWithAllCategoriesCustomizedType& lhs, const EncoderTestChoiceWithAllCategoriesCustomizedType& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
    // the same value, and 'false' otherwise.  Two attribute objects have the
    // same value if each respective attribute has the same value.

inline
bool operator!=(const EncoderTestChoiceWithAllCategoriesCustomizedType& lhs, const EncoderTestChoiceWithAllCategoriesCustomizedType& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
    // have the same value, and 'false' otherwise.  Two attribute objects do
    // not have the same value if one or more respective attributes differ in
    // values.

inline
bsl::ostream& operator<<(bsl::ostream& stream, const EncoderTestChoiceWithAllCategoriesCustomizedType& rhs);
    // Format the specified 'rhs' to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& hashAlg, const baljsn::EncoderTestChoiceWithAllCategoriesCustomizedType& object);
    // Pass the specified 'object' to the specified 'hashAlg'.  This function
    // integrates with the 'bslh' modular hashing system and effectively
    // provides a 'bsl::hash' specialization for 'EncoderTestChoiceWithAllCategoriesCustomizedType'.

}  // close package namespace

// TRAITS

BDLAT_DECL_CUSTOMIZEDTYPE_WITH_ALLOCATOR_BITWISEMOVEABLE_TRAITS(baljsn::EncoderTestChoiceWithAllCategoriesCustomizedType)

namespace baljsn {

            // ===================================================
            // class EncoderTestChoiceWithAllCategoriesEnumeration
            // ===================================================

struct EncoderTestChoiceWithAllCategoriesEnumeration {

  public:
    // TYPES
    enum Value {
        A = 0
      , B = 1
    };

    enum {
        NUM_ENUMERATORS = 2
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
bsl::ostream& operator<<(bsl::ostream& stream, EncoderTestChoiceWithAllCategoriesEnumeration::Value rhs);
    // Format the specified 'rhs' to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.

}  // close package namespace

// TRAITS

BDLAT_DECL_ENUMERATION_TRAITS(baljsn::EncoderTestChoiceWithAllCategoriesEnumeration)


namespace baljsn {

              // ================================================
              // class EncoderTestChoiceWithAllCategoriesSequence
              // ================================================

class EncoderTestChoiceWithAllCategoriesSequence {

    // INSTANCE DATA
    int  d_attribute;

  public:
    // TYPES
    enum {
        ATTRIBUTE_ID_ATTRIBUTE = 0
    };

    enum {
        NUM_ATTRIBUTES = 1
    };

    enum {
        ATTRIBUTE_INDEX_ATTRIBUTE = 0
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
    EncoderTestChoiceWithAllCategoriesSequence();
        // Create an object of type
        // 'EncoderTestChoiceWithAllCategoriesSequence' having the default
        // value.

    EncoderTestChoiceWithAllCategoriesSequence(const EncoderTestChoiceWithAllCategoriesSequence& original);
        // Create an object of type
        // 'EncoderTestChoiceWithAllCategoriesSequence' having the value of the
        // specified 'original' object.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    EncoderTestChoiceWithAllCategoriesSequence(EncoderTestChoiceWithAllCategoriesSequence&& original) = default;
        // Create an object of type
        // 'EncoderTestChoiceWithAllCategoriesSequence' having the value of the
        // specified 'original' object.  After performing this action, the
        // 'original' object will be left in a valid, but unspecified state.
#endif

    ~EncoderTestChoiceWithAllCategoriesSequence();
        // Destroy this object.

    // MANIPULATORS
    EncoderTestChoiceWithAllCategoriesSequence& operator=(const EncoderTestChoiceWithAllCategoriesSequence& rhs);
        // Assign to this object the value of the specified 'rhs' object.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    EncoderTestChoiceWithAllCategoriesSequence& operator=(EncoderTestChoiceWithAllCategoriesSequence&& rhs);
        // Assign to this object the value of the specified 'rhs' object.
        // After performing this action, the 'rhs' object will be left in a
        // valid, but unspecified state.
#endif

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

    int& attribute();
        // Return a reference to the modifiable "Attribute" attribute of this
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

    int attribute() const;
        // Return a reference to the non-modifiable "Attribute" attribute of
        // this object.
};

// FREE OPERATORS
inline
bool operator==(const EncoderTestChoiceWithAllCategoriesSequence& lhs, const EncoderTestChoiceWithAllCategoriesSequence& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
    // the same value, and 'false' otherwise.  Two attribute objects have the
    // same value if each respective attribute has the same value.

inline
bool operator!=(const EncoderTestChoiceWithAllCategoriesSequence& lhs, const EncoderTestChoiceWithAllCategoriesSequence& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
    // have the same value, and 'false' otherwise.  Two attribute objects do
    // not have the same value if one or more respective attributes differ in
    // values.

inline
bsl::ostream& operator<<(bsl::ostream& stream, const EncoderTestChoiceWithAllCategoriesSequence& rhs);
    // Format the specified 'rhs' to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& hashAlg, const baljsn::EncoderTestChoiceWithAllCategoriesSequence& object);
    // Pass the specified 'object' to the specified 'hashAlg'.  This function
    // integrates with the 'bslh' modular hashing system and effectively
    // provides a 'bsl::hash' specialization for 'EncoderTestChoiceWithAllCategoriesSequence'.

}  // close package namespace

// TRAITS

BDLAT_DECL_SEQUENCE_WITH_BITWISEMOVEABLE_TRAITS(baljsn::EncoderTestChoiceWithAllCategoriesSequence)

namespace baljsn {

                 // ==========================================
                 // class EncoderTestDegenerateChoice1Sequence
                 // ==========================================

class EncoderTestDegenerateChoice1Sequence {

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
    EncoderTestDegenerateChoice1Sequence();
        // Create an object of type 'EncoderTestDegenerateChoice1Sequence'
        // having the default value.

    EncoderTestDegenerateChoice1Sequence(const EncoderTestDegenerateChoice1Sequence& original);
        // Create an object of type 'EncoderTestDegenerateChoice1Sequence'
        // having the value of the specified 'original' object.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    EncoderTestDegenerateChoice1Sequence(EncoderTestDegenerateChoice1Sequence&& original) = default;
        // Create an object of type 'EncoderTestDegenerateChoice1Sequence'
        // having the value of the specified 'original' object.  After
        // performing this action, the 'original' object will be left in a
        // valid, but unspecified state.
#endif

    ~EncoderTestDegenerateChoice1Sequence();
        // Destroy this object.

    // MANIPULATORS
    EncoderTestDegenerateChoice1Sequence& operator=(const EncoderTestDegenerateChoice1Sequence& rhs);
        // Assign to this object the value of the specified 'rhs' object.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    EncoderTestDegenerateChoice1Sequence& operator=(EncoderTestDegenerateChoice1Sequence&& rhs);
        // Assign to this object the value of the specified 'rhs' object.
        // After performing this action, the 'rhs' object will be left in a
        // valid, but unspecified state.
#endif

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
bool operator==(const EncoderTestDegenerateChoice1Sequence& lhs, const EncoderTestDegenerateChoice1Sequence& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
    // the same value, and 'false' otherwise.  Two attribute objects have the
    // same value if each respective attribute has the same value.

inline
bool operator!=(const EncoderTestDegenerateChoice1Sequence& lhs, const EncoderTestDegenerateChoice1Sequence& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
    // have the same value, and 'false' otherwise.  Two attribute objects do
    // not have the same value if one or more respective attributes differ in
    // values.

inline
bsl::ostream& operator<<(bsl::ostream& stream, const EncoderTestDegenerateChoice1Sequence& rhs);
    // Format the specified 'rhs' to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& hashAlg, const baljsn::EncoderTestDegenerateChoice1Sequence& object);
    // Pass the specified 'object' to the specified 'hashAlg'.  This function
    // integrates with the 'bslh' modular hashing system and effectively
    // provides a 'bsl::hash' specialization for 'EncoderTestDegenerateChoice1Sequence'.

}  // close package namespace

// TRAITS

BDLAT_DECL_SEQUENCE_WITH_BITWISEMOVEABLE_TRAITS(baljsn::EncoderTestDegenerateChoice1Sequence)

namespace baljsn {

              // ================================================
              // class EncoderTestSequenceWithAllCategoriesChoice
              // ================================================

class EncoderTestSequenceWithAllCategoriesChoice {

    // INSTANCE DATA
    union {
        bsls::ObjectBuffer< int > d_selection0;
    };

    int                           d_selectionId;

  public:
    // TYPES

    enum {
        SELECTION_ID_UNDEFINED  = -1
      , SELECTION_ID_SELECTION0 = 0
    };

    enum {
        NUM_SELECTIONS = 1
    };

    enum {
        SELECTION_INDEX_SELECTION0 = 0
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
    EncoderTestSequenceWithAllCategoriesChoice();
        // Create an object of type
        // 'EncoderTestSequenceWithAllCategoriesChoice' having the default
        // value.

    EncoderTestSequenceWithAllCategoriesChoice(const EncoderTestSequenceWithAllCategoriesChoice& original);
        // Create an object of type
        // 'EncoderTestSequenceWithAllCategoriesChoice' having the value of the
        // specified 'original' object.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    EncoderTestSequenceWithAllCategoriesChoice(EncoderTestSequenceWithAllCategoriesChoice&& original) noexcept;
        // Create an object of type
        // 'EncoderTestSequenceWithAllCategoriesChoice' having the value of the
        // specified 'original' object.  After performing this action, the
        // 'original' object will be left in a valid, but unspecified state.
#endif

    ~EncoderTestSequenceWithAllCategoriesChoice();
        // Destroy this object.

    // MANIPULATORS
    EncoderTestSequenceWithAllCategoriesChoice& operator=(const EncoderTestSequenceWithAllCategoriesChoice& rhs);
        // Assign to this object the value of the specified 'rhs' object.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    EncoderTestSequenceWithAllCategoriesChoice& operator=(EncoderTestSequenceWithAllCategoriesChoice&& rhs);
        // Assign to this object the value of the specified 'rhs' object.
        // After performing this action, the 'rhs' object will be left in a
        // valid, but unspecified state.
#endif

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

    int& makeSelection0();
    int& makeSelection0(int value);
        // Set the value of this object to be a "Selection0" value.  Optionally
        // specify the 'value' of the "Selection0".  If 'value' is not
        // specified, the default "Selection0" value is used.

    template<class MANIPULATOR>
    int manipulateSelection(MANIPULATOR& manipulator);
        // Invoke the specified 'manipulator' on the address of the modifiable
        // selection, supplying 'manipulator' with the corresponding selection
        // information structure.  Return the value returned from the
        // invocation of 'manipulator' if this object has a defined selection,
        // and -1 otherwise.

    int& selection0();
        // Return a reference to the modifiable "Selection0" selection of this
        // object if "Selection0" is the current selection.  The behavior is
        // undefined unless "Selection0" is the selection of this object.

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

    const int& selection0() const;
        // Return a reference to the non-modifiable "Selection0" selection of
        // this object if "Selection0" is the current selection.  The behavior
        // is undefined unless "Selection0" is the selection of this object.

    bool isSelection0Value() const;
        // Return 'true' if the value of this object is a "Selection0" value,
        // and return 'false' otherwise.

    bool isUndefinedValue() const;
        // Return 'true' if the value of this object is undefined, and 'false'
        // otherwise.

    const char *selectionName() const;
        // Return the symbolic name of the current selection of this object.
};

// FREE OPERATORS
inline
bool operator==(const EncoderTestSequenceWithAllCategoriesChoice& lhs, const EncoderTestSequenceWithAllCategoriesChoice& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects have the same
    // value, and 'false' otherwise.  Two 'EncoderTestSequenceWithAllCategoriesChoice' objects have the same
    // value if either the selections in both objects have the same ids and
    // the same values, or both selections are undefined.

inline
bool operator!=(const EncoderTestSequenceWithAllCategoriesChoice& lhs, const EncoderTestSequenceWithAllCategoriesChoice& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects do not have the
    // same values, as determined by 'operator==', and 'false' otherwise.

inline
bsl::ostream& operator<<(bsl::ostream& stream, const EncoderTestSequenceWithAllCategoriesChoice& rhs);
    // Format the specified 'rhs' to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& hashAlg, const baljsn::EncoderTestSequenceWithAllCategoriesChoice& object);
    // Pass the specified 'object' to the specified 'hashAlg'.  This function
    // integrates with the 'bslh' modular hashing system and effectively
    // provides a 'bsl::hash' specialization for 'EncoderTestSequenceWithAllCategoriesChoice'.

}  // close package namespace

// TRAITS

BDLAT_DECL_CHOICE_WITH_BITWISEMOVEABLE_TRAITS(baljsn::EncoderTestSequenceWithAllCategoriesChoice)

namespace baljsn {

          // ========================================================
          // class EncoderTestSequenceWithAllCategoriesCustomizedType
          // ========================================================

class EncoderTestSequenceWithAllCategoriesCustomizedType {

    // INSTANCE DATA
    bsl::string d_value;

    // FRIENDS
    friend bool operator==(const EncoderTestSequenceWithAllCategoriesCustomizedType& lhs, const EncoderTestSequenceWithAllCategoriesCustomizedType& rhs);
    friend bool operator!=(const EncoderTestSequenceWithAllCategoriesCustomizedType& lhs, const EncoderTestSequenceWithAllCategoriesCustomizedType& rhs);

  public:
    // TYPES
    typedef bsl::string BaseType;

    // CONSTANTS
    static const char CLASS_NAME[];

    // CREATORS
    explicit EncoderTestSequenceWithAllCategoriesCustomizedType(bslma::Allocator *basicAllocator = 0);
        // Create an object of type
        // 'EncoderTestSequenceWithAllCategoriesCustomizedType' having the
        // default value.  Use the optionally specified 'basicAllocator' to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.

    EncoderTestSequenceWithAllCategoriesCustomizedType(const EncoderTestSequenceWithAllCategoriesCustomizedType& original,
                                                      bslma::Allocator *basicAllocator = 0);
        // Create an object of type 'EncoderTestSequenceWithAllCategoriesCustomizedType' having the value
        // of the specified 'original' object.  Use the optionally specified
        // 'basicAllocator' to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    EncoderTestSequenceWithAllCategoriesCustomizedType(EncoderTestSequenceWithAllCategoriesCustomizedType&& original) = default;
        // Create an object of type
        // 'EncoderTestSequenceWithAllCategoriesCustomizedType' having the
        // value of the specified 'original' object.  After performing this
        // action, the 'original' object will be left in a valid, but
        // unspecified state.

    EncoderTestSequenceWithAllCategoriesCustomizedType(EncoderTestSequenceWithAllCategoriesCustomizedType&& original,
                                                      bslma::Allocator *basicAllocator);
        // Create an object of type
        // 'EncoderTestSequenceWithAllCategoriesCustomizedType' having the
        // value of the specified 'original' object.  After performing this
        // action, the 'original' object will be left in a valid, but
        // unspecified state.  Use the optionally specified 'basicAllocator' to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.
#endif

    explicit EncoderTestSequenceWithAllCategoriesCustomizedType(const bsl::string& value,
                                                               bslma::Allocator *basicAllocator = 0);
        // Create an object of type
        // 'EncoderTestSequenceWithAllCategoriesCustomizedType' having the
        // specified 'value'.  Use the optionally specified 'basicAllocator' to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.

    ~EncoderTestSequenceWithAllCategoriesCustomizedType();
        // Destroy this object.

    // MANIPULATORS
    EncoderTestSequenceWithAllCategoriesCustomizedType& operator=(const EncoderTestSequenceWithAllCategoriesCustomizedType& rhs);
        // Assign to this object the value of the specified 'rhs' object.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    EncoderTestSequenceWithAllCategoriesCustomizedType& operator=(EncoderTestSequenceWithAllCategoriesCustomizedType&& rhs) = default;
        // Assign to this object the value of the specified 'rhs' object.
        // After performing this action, the 'rhs' object will be left in a
        // valid, but unspecified state.
#endif

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

    // PUBLIC CLASS METHODS
    static int checkRestrictions(const bsl::string& value);
        // Check if the specified 'value' satisfies the restrictions of this
        // class (i.e., "EncoderTestSequenceWithAllCategoriesCustomizedType").
        // Return 0 if successful (i.e., the restrictions are satisfied) and
        // non-zero otherwise.
};

// FREE OPERATORS
inline
bool operator==(const EncoderTestSequenceWithAllCategoriesCustomizedType& lhs, const EncoderTestSequenceWithAllCategoriesCustomizedType& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
    // the same value, and 'false' otherwise.  Two attribute objects have the
    // same value if each respective attribute has the same value.

inline
bool operator!=(const EncoderTestSequenceWithAllCategoriesCustomizedType& lhs, const EncoderTestSequenceWithAllCategoriesCustomizedType& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
    // have the same value, and 'false' otherwise.  Two attribute objects do
    // not have the same value if one or more respective attributes differ in
    // values.

inline
bsl::ostream& operator<<(bsl::ostream& stream, const EncoderTestSequenceWithAllCategoriesCustomizedType& rhs);
    // Format the specified 'rhs' to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& hashAlg, const baljsn::EncoderTestSequenceWithAllCategoriesCustomizedType& object);
    // Pass the specified 'object' to the specified 'hashAlg'.  This function
    // integrates with the 'bslh' modular hashing system and effectively
    // provides a 'bsl::hash' specialization for 'EncoderTestSequenceWithAllCategoriesCustomizedType'.

}  // close package namespace

// TRAITS

BDLAT_DECL_CUSTOMIZEDTYPE_WITH_ALLOCATOR_BITWISEMOVEABLE_TRAITS(baljsn::EncoderTestSequenceWithAllCategoriesCustomizedType)

namespace baljsn {

           // =====================================================
           // class EncoderTestSequenceWithAllCategoriesEnumeration
           // =====================================================

struct EncoderTestSequenceWithAllCategoriesEnumeration {

  public:
    // TYPES
    enum Value {
        A = 0
      , B = 1
    };

    enum {
        NUM_ENUMERATORS = 2
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
bsl::ostream& operator<<(bsl::ostream& stream, EncoderTestSequenceWithAllCategoriesEnumeration::Value rhs);
    // Format the specified 'rhs' to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.

}  // close package namespace

// TRAITS

BDLAT_DECL_ENUMERATION_TRAITS(baljsn::EncoderTestSequenceWithAllCategoriesEnumeration)


namespace baljsn {

             // ==================================================
             // class EncoderTestSequenceWithAllCategoriesSequence
             // ==================================================

class EncoderTestSequenceWithAllCategoriesSequence {

    // INSTANCE DATA
    int  d_attribute;

  public:
    // TYPES
    enum {
        ATTRIBUTE_ID_ATTRIBUTE = 0
    };

    enum {
        NUM_ATTRIBUTES = 1
    };

    enum {
        ATTRIBUTE_INDEX_ATTRIBUTE = 0
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
    EncoderTestSequenceWithAllCategoriesSequence();
        // Create an object of type
        // 'EncoderTestSequenceWithAllCategoriesSequence' having the default
        // value.

    EncoderTestSequenceWithAllCategoriesSequence(const EncoderTestSequenceWithAllCategoriesSequence& original);
        // Create an object of type
        // 'EncoderTestSequenceWithAllCategoriesSequence' having the value of
        // the specified 'original' object.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    EncoderTestSequenceWithAllCategoriesSequence(EncoderTestSequenceWithAllCategoriesSequence&& original) = default;
        // Create an object of type
        // 'EncoderTestSequenceWithAllCategoriesSequence' having the value of
        // the specified 'original' object.  After performing this action, the
        // 'original' object will be left in a valid, but unspecified state.
#endif

    ~EncoderTestSequenceWithAllCategoriesSequence();
        // Destroy this object.

    // MANIPULATORS
    EncoderTestSequenceWithAllCategoriesSequence& operator=(const EncoderTestSequenceWithAllCategoriesSequence& rhs);
        // Assign to this object the value of the specified 'rhs' object.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    EncoderTestSequenceWithAllCategoriesSequence& operator=(EncoderTestSequenceWithAllCategoriesSequence&& rhs);
        // Assign to this object the value of the specified 'rhs' object.
        // After performing this action, the 'rhs' object will be left in a
        // valid, but unspecified state.
#endif

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

    int& attribute();
        // Return a reference to the modifiable "Attribute" attribute of this
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

    int attribute() const;
        // Return a reference to the non-modifiable "Attribute" attribute of
        // this object.
};

// FREE OPERATORS
inline
bool operator==(const EncoderTestSequenceWithAllCategoriesSequence& lhs, const EncoderTestSequenceWithAllCategoriesSequence& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
    // the same value, and 'false' otherwise.  Two attribute objects have the
    // same value if each respective attribute has the same value.

inline
bool operator!=(const EncoderTestSequenceWithAllCategoriesSequence& lhs, const EncoderTestSequenceWithAllCategoriesSequence& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
    // have the same value, and 'false' otherwise.  Two attribute objects do
    // not have the same value if one or more respective attributes differ in
    // values.

inline
bsl::ostream& operator<<(bsl::ostream& stream, const EncoderTestSequenceWithAllCategoriesSequence& rhs);
    // Format the specified 'rhs' to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& hashAlg, const baljsn::EncoderTestSequenceWithAllCategoriesSequence& object);
    // Pass the specified 'object' to the specified 'hashAlg'.  This function
    // integrates with the 'bslh' modular hashing system and effectively
    // provides a 'bsl::hash' specialization for 'EncoderTestSequenceWithAllCategoriesSequence'.

}  // close package namespace

// TRAITS

BDLAT_DECL_SEQUENCE_WITH_BITWISEMOVEABLE_TRAITS(baljsn::EncoderTestSequenceWithAllCategoriesSequence)

namespace baljsn {

                   // ======================================
                   // class EncoderTestSequenceWithUntagged0
                   // ======================================

class EncoderTestSequenceWithUntagged0 {

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
    EncoderTestSequenceWithUntagged0();
        // Create an object of type 'EncoderTestSequenceWithUntagged0' having
        // the default value.

    EncoderTestSequenceWithUntagged0(const EncoderTestSequenceWithUntagged0& original);
        // Create an object of type 'EncoderTestSequenceWithUntagged0' having
        // the value of the specified 'original' object.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    EncoderTestSequenceWithUntagged0(EncoderTestSequenceWithUntagged0&& original) = default;
        // Create an object of type 'EncoderTestSequenceWithUntagged0' having
        // the value of the specified 'original' object.  After performing this
        // action, the 'original' object will be left in a valid, but
        // unspecified state.
#endif

    ~EncoderTestSequenceWithUntagged0();
        // Destroy this object.

    // MANIPULATORS
    EncoderTestSequenceWithUntagged0& operator=(const EncoderTestSequenceWithUntagged0& rhs);
        // Assign to this object the value of the specified 'rhs' object.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    EncoderTestSequenceWithUntagged0& operator=(EncoderTestSequenceWithUntagged0&& rhs);
        // Assign to this object the value of the specified 'rhs' object.
        // After performing this action, the 'rhs' object will be left in a
        // valid, but unspecified state.
#endif

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
bool operator==(const EncoderTestSequenceWithUntagged0& lhs, const EncoderTestSequenceWithUntagged0& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
    // the same value, and 'false' otherwise.  Two attribute objects have the
    // same value if each respective attribute has the same value.

inline
bool operator!=(const EncoderTestSequenceWithUntagged0& lhs, const EncoderTestSequenceWithUntagged0& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
    // have the same value, and 'false' otherwise.  Two attribute objects do
    // not have the same value if one or more respective attributes differ in
    // values.

inline
bsl::ostream& operator<<(bsl::ostream& stream, const EncoderTestSequenceWithUntagged0& rhs);
    // Format the specified 'rhs' to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& hashAlg, const baljsn::EncoderTestSequenceWithUntagged0& object);
    // Pass the specified 'object' to the specified 'hashAlg'.  This function
    // integrates with the 'bslh' modular hashing system and effectively
    // provides a 'bsl::hash' specialization for 'EncoderTestSequenceWithUntagged0'.

}  // close package namespace

// TRAITS

BDLAT_DECL_SEQUENCE_WITH_BITWISEMOVEABLE_TRAITS(baljsn::EncoderTestSequenceWithUntagged0)

namespace baljsn {

              // ===============================================
              // class EncoderTestSequenceWithUntagged10Sequence
              // ===============================================

class EncoderTestSequenceWithUntagged10Sequence {

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
    EncoderTestSequenceWithUntagged10Sequence();
        // Create an object of type 'EncoderTestSequenceWithUntagged10Sequence'
        // having the default value.

    EncoderTestSequenceWithUntagged10Sequence(const EncoderTestSequenceWithUntagged10Sequence& original);
        // Create an object of type 'EncoderTestSequenceWithUntagged10Sequence'
        // having the value of the specified 'original' object.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    EncoderTestSequenceWithUntagged10Sequence(EncoderTestSequenceWithUntagged10Sequence&& original) = default;
        // Create an object of type 'EncoderTestSequenceWithUntagged10Sequence'
        // having the value of the specified 'original' object.  After
        // performing this action, the 'original' object will be left in a
        // valid, but unspecified state.
#endif

    ~EncoderTestSequenceWithUntagged10Sequence();
        // Destroy this object.

    // MANIPULATORS
    EncoderTestSequenceWithUntagged10Sequence& operator=(const EncoderTestSequenceWithUntagged10Sequence& rhs);
        // Assign to this object the value of the specified 'rhs' object.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    EncoderTestSequenceWithUntagged10Sequence& operator=(EncoderTestSequenceWithUntagged10Sequence&& rhs);
        // Assign to this object the value of the specified 'rhs' object.
        // After performing this action, the 'rhs' object will be left in a
        // valid, but unspecified state.
#endif

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
bool operator==(const EncoderTestSequenceWithUntagged10Sequence& lhs, const EncoderTestSequenceWithUntagged10Sequence& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
    // the same value, and 'false' otherwise.  Two attribute objects have the
    // same value if each respective attribute has the same value.

inline
bool operator!=(const EncoderTestSequenceWithUntagged10Sequence& lhs, const EncoderTestSequenceWithUntagged10Sequence& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
    // have the same value, and 'false' otherwise.  Two attribute objects do
    // not have the same value if one or more respective attributes differ in
    // values.

inline
bsl::ostream& operator<<(bsl::ostream& stream, const EncoderTestSequenceWithUntagged10Sequence& rhs);
    // Format the specified 'rhs' to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& hashAlg, const baljsn::EncoderTestSequenceWithUntagged10Sequence& object);
    // Pass the specified 'object' to the specified 'hashAlg'.  This function
    // integrates with the 'bslh' modular hashing system and effectively
    // provides a 'bsl::hash' specialization for 'EncoderTestSequenceWithUntagged10Sequence'.

}  // close package namespace

// TRAITS

BDLAT_DECL_SEQUENCE_WITH_BITWISEMOVEABLE_TRAITS(baljsn::EncoderTestSequenceWithUntagged10Sequence)

namespace baljsn {

              // ===============================================
              // class EncoderTestSequenceWithUntagged11Sequence
              // ===============================================

class EncoderTestSequenceWithUntagged11Sequence {

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
    EncoderTestSequenceWithUntagged11Sequence();
        // Create an object of type 'EncoderTestSequenceWithUntagged11Sequence'
        // having the default value.

    EncoderTestSequenceWithUntagged11Sequence(const EncoderTestSequenceWithUntagged11Sequence& original);
        // Create an object of type 'EncoderTestSequenceWithUntagged11Sequence'
        // having the value of the specified 'original' object.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    EncoderTestSequenceWithUntagged11Sequence(EncoderTestSequenceWithUntagged11Sequence&& original) = default;
        // Create an object of type 'EncoderTestSequenceWithUntagged11Sequence'
        // having the value of the specified 'original' object.  After
        // performing this action, the 'original' object will be left in a
        // valid, but unspecified state.
#endif

    ~EncoderTestSequenceWithUntagged11Sequence();
        // Destroy this object.

    // MANIPULATORS
    EncoderTestSequenceWithUntagged11Sequence& operator=(const EncoderTestSequenceWithUntagged11Sequence& rhs);
        // Assign to this object the value of the specified 'rhs' object.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    EncoderTestSequenceWithUntagged11Sequence& operator=(EncoderTestSequenceWithUntagged11Sequence&& rhs);
        // Assign to this object the value of the specified 'rhs' object.
        // After performing this action, the 'rhs' object will be left in a
        // valid, but unspecified state.
#endif

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
bool operator==(const EncoderTestSequenceWithUntagged11Sequence& lhs, const EncoderTestSequenceWithUntagged11Sequence& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
    // the same value, and 'false' otherwise.  Two attribute objects have the
    // same value if each respective attribute has the same value.

inline
bool operator!=(const EncoderTestSequenceWithUntagged11Sequence& lhs, const EncoderTestSequenceWithUntagged11Sequence& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
    // have the same value, and 'false' otherwise.  Two attribute objects do
    // not have the same value if one or more respective attributes differ in
    // values.

inline
bsl::ostream& operator<<(bsl::ostream& stream, const EncoderTestSequenceWithUntagged11Sequence& rhs);
    // Format the specified 'rhs' to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& hashAlg, const baljsn::EncoderTestSequenceWithUntagged11Sequence& object);
    // Pass the specified 'object' to the specified 'hashAlg'.  This function
    // integrates with the 'bslh' modular hashing system and effectively
    // provides a 'bsl::hash' specialization for 'EncoderTestSequenceWithUntagged11Sequence'.

}  // close package namespace

// TRAITS

BDLAT_DECL_SEQUENCE_WITH_BITWISEMOVEABLE_TRAITS(baljsn::EncoderTestSequenceWithUntagged11Sequence)

namespace baljsn {

              // ================================================
              // class EncoderTestSequenceWithUntagged11Sequence1
              // ================================================

class EncoderTestSequenceWithUntagged11Sequence1 {

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
    EncoderTestSequenceWithUntagged11Sequence1();
        // Create an object of type
        // 'EncoderTestSequenceWithUntagged11Sequence1' having the default
        // value.

    EncoderTestSequenceWithUntagged11Sequence1(const EncoderTestSequenceWithUntagged11Sequence1& original);
        // Create an object of type
        // 'EncoderTestSequenceWithUntagged11Sequence1' having the value of the
        // specified 'original' object.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    EncoderTestSequenceWithUntagged11Sequence1(EncoderTestSequenceWithUntagged11Sequence1&& original) = default;
        // Create an object of type
        // 'EncoderTestSequenceWithUntagged11Sequence1' having the value of the
        // specified 'original' object.  After performing this action, the
        // 'original' object will be left in a valid, but unspecified state.
#endif

    ~EncoderTestSequenceWithUntagged11Sequence1();
        // Destroy this object.

    // MANIPULATORS
    EncoderTestSequenceWithUntagged11Sequence1& operator=(const EncoderTestSequenceWithUntagged11Sequence1& rhs);
        // Assign to this object the value of the specified 'rhs' object.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    EncoderTestSequenceWithUntagged11Sequence1& operator=(EncoderTestSequenceWithUntagged11Sequence1&& rhs);
        // Assign to this object the value of the specified 'rhs' object.
        // After performing this action, the 'rhs' object will be left in a
        // valid, but unspecified state.
#endif

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
bool operator==(const EncoderTestSequenceWithUntagged11Sequence1& lhs, const EncoderTestSequenceWithUntagged11Sequence1& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
    // the same value, and 'false' otherwise.  Two attribute objects have the
    // same value if each respective attribute has the same value.

inline
bool operator!=(const EncoderTestSequenceWithUntagged11Sequence1& lhs, const EncoderTestSequenceWithUntagged11Sequence1& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
    // have the same value, and 'false' otherwise.  Two attribute objects do
    // not have the same value if one or more respective attributes differ in
    // values.

inline
bsl::ostream& operator<<(bsl::ostream& stream, const EncoderTestSequenceWithUntagged11Sequence1& rhs);
    // Format the specified 'rhs' to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& hashAlg, const baljsn::EncoderTestSequenceWithUntagged11Sequence1& object);
    // Pass the specified 'object' to the specified 'hashAlg'.  This function
    // integrates with the 'bslh' modular hashing system and effectively
    // provides a 'bsl::hash' specialization for 'EncoderTestSequenceWithUntagged11Sequence1'.

}  // close package namespace

// TRAITS

BDLAT_DECL_SEQUENCE_WITH_BITWISEMOVEABLE_TRAITS(baljsn::EncoderTestSequenceWithUntagged11Sequence1)

namespace baljsn {

              // ===============================================
              // class EncoderTestSequenceWithUntagged12Sequence
              // ===============================================

class EncoderTestSequenceWithUntagged12Sequence {

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
    EncoderTestSequenceWithUntagged12Sequence();
        // Create an object of type 'EncoderTestSequenceWithUntagged12Sequence'
        // having the default value.

    EncoderTestSequenceWithUntagged12Sequence(const EncoderTestSequenceWithUntagged12Sequence& original);
        // Create an object of type 'EncoderTestSequenceWithUntagged12Sequence'
        // having the value of the specified 'original' object.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    EncoderTestSequenceWithUntagged12Sequence(EncoderTestSequenceWithUntagged12Sequence&& original) = default;
        // Create an object of type 'EncoderTestSequenceWithUntagged12Sequence'
        // having the value of the specified 'original' object.  After
        // performing this action, the 'original' object will be left in a
        // valid, but unspecified state.
#endif

    ~EncoderTestSequenceWithUntagged12Sequence();
        // Destroy this object.

    // MANIPULATORS
    EncoderTestSequenceWithUntagged12Sequence& operator=(const EncoderTestSequenceWithUntagged12Sequence& rhs);
        // Assign to this object the value of the specified 'rhs' object.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    EncoderTestSequenceWithUntagged12Sequence& operator=(EncoderTestSequenceWithUntagged12Sequence&& rhs);
        // Assign to this object the value of the specified 'rhs' object.
        // After performing this action, the 'rhs' object will be left in a
        // valid, but unspecified state.
#endif

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
bool operator==(const EncoderTestSequenceWithUntagged12Sequence& lhs, const EncoderTestSequenceWithUntagged12Sequence& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
    // the same value, and 'false' otherwise.  Two attribute objects have the
    // same value if each respective attribute has the same value.

inline
bool operator!=(const EncoderTestSequenceWithUntagged12Sequence& lhs, const EncoderTestSequenceWithUntagged12Sequence& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
    // have the same value, and 'false' otherwise.  Two attribute objects do
    // not have the same value if one or more respective attributes differ in
    // values.

inline
bsl::ostream& operator<<(bsl::ostream& stream, const EncoderTestSequenceWithUntagged12Sequence& rhs);
    // Format the specified 'rhs' to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& hashAlg, const baljsn::EncoderTestSequenceWithUntagged12Sequence& object);
    // Pass the specified 'object' to the specified 'hashAlg'.  This function
    // integrates with the 'bslh' modular hashing system and effectively
    // provides a 'bsl::hash' specialization for 'EncoderTestSequenceWithUntagged12Sequence'.

}  // close package namespace

// TRAITS

BDLAT_DECL_SEQUENCE_WITH_BITWISEMOVEABLE_TRAITS(baljsn::EncoderTestSequenceWithUntagged12Sequence)

namespace baljsn {

              // ===============================================
              // class EncoderTestSequenceWithUntagged13Sequence
              // ===============================================

class EncoderTestSequenceWithUntagged13Sequence {

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
    EncoderTestSequenceWithUntagged13Sequence();
        // Create an object of type 'EncoderTestSequenceWithUntagged13Sequence'
        // having the default value.

    EncoderTestSequenceWithUntagged13Sequence(const EncoderTestSequenceWithUntagged13Sequence& original);
        // Create an object of type 'EncoderTestSequenceWithUntagged13Sequence'
        // having the value of the specified 'original' object.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    EncoderTestSequenceWithUntagged13Sequence(EncoderTestSequenceWithUntagged13Sequence&& original) = default;
        // Create an object of type 'EncoderTestSequenceWithUntagged13Sequence'
        // having the value of the specified 'original' object.  After
        // performing this action, the 'original' object will be left in a
        // valid, but unspecified state.
#endif

    ~EncoderTestSequenceWithUntagged13Sequence();
        // Destroy this object.

    // MANIPULATORS
    EncoderTestSequenceWithUntagged13Sequence& operator=(const EncoderTestSequenceWithUntagged13Sequence& rhs);
        // Assign to this object the value of the specified 'rhs' object.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    EncoderTestSequenceWithUntagged13Sequence& operator=(EncoderTestSequenceWithUntagged13Sequence&& rhs);
        // Assign to this object the value of the specified 'rhs' object.
        // After performing this action, the 'rhs' object will be left in a
        // valid, but unspecified state.
#endif

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
bool operator==(const EncoderTestSequenceWithUntagged13Sequence& lhs, const EncoderTestSequenceWithUntagged13Sequence& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
    // the same value, and 'false' otherwise.  Two attribute objects have the
    // same value if each respective attribute has the same value.

inline
bool operator!=(const EncoderTestSequenceWithUntagged13Sequence& lhs, const EncoderTestSequenceWithUntagged13Sequence& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
    // have the same value, and 'false' otherwise.  Two attribute objects do
    // not have the same value if one or more respective attributes differ in
    // values.

inline
bsl::ostream& operator<<(bsl::ostream& stream, const EncoderTestSequenceWithUntagged13Sequence& rhs);
    // Format the specified 'rhs' to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& hashAlg, const baljsn::EncoderTestSequenceWithUntagged13Sequence& object);
    // Pass the specified 'object' to the specified 'hashAlg'.  This function
    // integrates with the 'bslh' modular hashing system and effectively
    // provides a 'bsl::hash' specialization for 'EncoderTestSequenceWithUntagged13Sequence'.

}  // close package namespace

// TRAITS

BDLAT_DECL_SEQUENCE_WITH_BITWISEMOVEABLE_TRAITS(baljsn::EncoderTestSequenceWithUntagged13Sequence)

namespace baljsn {

                  // =======================================
                  // class EncoderTestSequenceWithUntagged14
                  // =======================================

class EncoderTestSequenceWithUntagged14 {

    // INSTANCE DATA
    int  d_attribute0;
    int  d_attribute1;
    int  d_attribute2;

  public:
    // TYPES
    enum {
        ATTRIBUTE_ID_ATTRIBUTE0 = 0
      , ATTRIBUTE_ID_ATTRIBUTE1 = 1
      , ATTRIBUTE_ID_ATTRIBUTE2 = 2
    };

    enum {
        NUM_ATTRIBUTES = 3
    };

    enum {
        ATTRIBUTE_INDEX_ATTRIBUTE0 = 0
      , ATTRIBUTE_INDEX_ATTRIBUTE1 = 1
      , ATTRIBUTE_INDEX_ATTRIBUTE2 = 2
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
    EncoderTestSequenceWithUntagged14();
        // Create an object of type 'EncoderTestSequenceWithUntagged14' having
        // the default value.

    EncoderTestSequenceWithUntagged14(const EncoderTestSequenceWithUntagged14& original);
        // Create an object of type 'EncoderTestSequenceWithUntagged14' having
        // the value of the specified 'original' object.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    EncoderTestSequenceWithUntagged14(EncoderTestSequenceWithUntagged14&& original) = default;
        // Create an object of type 'EncoderTestSequenceWithUntagged14' having
        // the value of the specified 'original' object.  After performing this
        // action, the 'original' object will be left in a valid, but
        // unspecified state.
#endif

    ~EncoderTestSequenceWithUntagged14();
        // Destroy this object.

    // MANIPULATORS
    EncoderTestSequenceWithUntagged14& operator=(const EncoderTestSequenceWithUntagged14& rhs);
        // Assign to this object the value of the specified 'rhs' object.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    EncoderTestSequenceWithUntagged14& operator=(EncoderTestSequenceWithUntagged14&& rhs);
        // Assign to this object the value of the specified 'rhs' object.
        // After performing this action, the 'rhs' object will be left in a
        // valid, but unspecified state.
#endif

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

    int& attribute0();
        // Return a reference to the modifiable "Attribute0" attribute of this
        // object.

    int& attribute1();
        // Return a reference to the modifiable "Attribute1" attribute of this
        // object.

    int& attribute2();
        // Return a reference to the modifiable "Attribute2" attribute of this
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

    int attribute0() const;
        // Return a reference to the non-modifiable "Attribute0" attribute of
        // this object.

    int attribute1() const;
        // Return a reference to the non-modifiable "Attribute1" attribute of
        // this object.

    int attribute2() const;
        // Return a reference to the non-modifiable "Attribute2" attribute of
        // this object.
};

// FREE OPERATORS
inline
bool operator==(const EncoderTestSequenceWithUntagged14& lhs, const EncoderTestSequenceWithUntagged14& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
    // the same value, and 'false' otherwise.  Two attribute objects have the
    // same value if each respective attribute has the same value.

inline
bool operator!=(const EncoderTestSequenceWithUntagged14& lhs, const EncoderTestSequenceWithUntagged14& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
    // have the same value, and 'false' otherwise.  Two attribute objects do
    // not have the same value if one or more respective attributes differ in
    // values.

inline
bsl::ostream& operator<<(bsl::ostream& stream, const EncoderTestSequenceWithUntagged14& rhs);
    // Format the specified 'rhs' to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& hashAlg, const baljsn::EncoderTestSequenceWithUntagged14& object);
    // Pass the specified 'object' to the specified 'hashAlg'.  This function
    // integrates with the 'bslh' modular hashing system and effectively
    // provides a 'bsl::hash' specialization for 'EncoderTestSequenceWithUntagged14'.

}  // close package namespace

// TRAITS

BDLAT_DECL_SEQUENCE_WITH_BITWISEMOVEABLE_TRAITS(baljsn::EncoderTestSequenceWithUntagged14)

namespace baljsn {

               // ==============================================
               // class EncoderTestSequenceWithUntagged1Sequence
               // ==============================================

class EncoderTestSequenceWithUntagged1Sequence {

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
    EncoderTestSequenceWithUntagged1Sequence();
        // Create an object of type 'EncoderTestSequenceWithUntagged1Sequence'
        // having the default value.

    EncoderTestSequenceWithUntagged1Sequence(const EncoderTestSequenceWithUntagged1Sequence& original);
        // Create an object of type 'EncoderTestSequenceWithUntagged1Sequence'
        // having the value of the specified 'original' object.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    EncoderTestSequenceWithUntagged1Sequence(EncoderTestSequenceWithUntagged1Sequence&& original) = default;
        // Create an object of type 'EncoderTestSequenceWithUntagged1Sequence'
        // having the value of the specified 'original' object.  After
        // performing this action, the 'original' object will be left in a
        // valid, but unspecified state.
#endif

    ~EncoderTestSequenceWithUntagged1Sequence();
        // Destroy this object.

    // MANIPULATORS
    EncoderTestSequenceWithUntagged1Sequence& operator=(const EncoderTestSequenceWithUntagged1Sequence& rhs);
        // Assign to this object the value of the specified 'rhs' object.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    EncoderTestSequenceWithUntagged1Sequence& operator=(EncoderTestSequenceWithUntagged1Sequence&& rhs);
        // Assign to this object the value of the specified 'rhs' object.
        // After performing this action, the 'rhs' object will be left in a
        // valid, but unspecified state.
#endif

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
bool operator==(const EncoderTestSequenceWithUntagged1Sequence& lhs, const EncoderTestSequenceWithUntagged1Sequence& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
    // the same value, and 'false' otherwise.  Two attribute objects have the
    // same value if each respective attribute has the same value.

inline
bool operator!=(const EncoderTestSequenceWithUntagged1Sequence& lhs, const EncoderTestSequenceWithUntagged1Sequence& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
    // have the same value, and 'false' otherwise.  Two attribute objects do
    // not have the same value if one or more respective attributes differ in
    // values.

inline
bsl::ostream& operator<<(bsl::ostream& stream, const EncoderTestSequenceWithUntagged1Sequence& rhs);
    // Format the specified 'rhs' to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& hashAlg, const baljsn::EncoderTestSequenceWithUntagged1Sequence& object);
    // Pass the specified 'object' to the specified 'hashAlg'.  This function
    // integrates with the 'bslh' modular hashing system and effectively
    // provides a 'bsl::hash' specialization for 'EncoderTestSequenceWithUntagged1Sequence'.

}  // close package namespace

// TRAITS

BDLAT_DECL_SEQUENCE_WITH_BITWISEMOVEABLE_TRAITS(baljsn::EncoderTestSequenceWithUntagged1Sequence)

namespace baljsn {

                   // ======================================
                   // class EncoderTestSequenceWithUntagged2
                   // ======================================

class EncoderTestSequenceWithUntagged2 {

    // INSTANCE DATA
    int  d_attribute0;

  public:
    // TYPES
    enum {
        ATTRIBUTE_ID_ATTRIBUTE0 = 0
    };

    enum {
        NUM_ATTRIBUTES = 1
    };

    enum {
        ATTRIBUTE_INDEX_ATTRIBUTE0 = 0
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
    EncoderTestSequenceWithUntagged2();
        // Create an object of type 'EncoderTestSequenceWithUntagged2' having
        // the default value.

    EncoderTestSequenceWithUntagged2(const EncoderTestSequenceWithUntagged2& original);
        // Create an object of type 'EncoderTestSequenceWithUntagged2' having
        // the value of the specified 'original' object.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    EncoderTestSequenceWithUntagged2(EncoderTestSequenceWithUntagged2&& original) = default;
        // Create an object of type 'EncoderTestSequenceWithUntagged2' having
        // the value of the specified 'original' object.  After performing this
        // action, the 'original' object will be left in a valid, but
        // unspecified state.
#endif

    ~EncoderTestSequenceWithUntagged2();
        // Destroy this object.

    // MANIPULATORS
    EncoderTestSequenceWithUntagged2& operator=(const EncoderTestSequenceWithUntagged2& rhs);
        // Assign to this object the value of the specified 'rhs' object.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    EncoderTestSequenceWithUntagged2& operator=(EncoderTestSequenceWithUntagged2&& rhs);
        // Assign to this object the value of the specified 'rhs' object.
        // After performing this action, the 'rhs' object will be left in a
        // valid, but unspecified state.
#endif

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

    int& attribute0();
        // Return a reference to the modifiable "Attribute0" attribute of this
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

    int attribute0() const;
        // Return a reference to the non-modifiable "Attribute0" attribute of
        // this object.
};

// FREE OPERATORS
inline
bool operator==(const EncoderTestSequenceWithUntagged2& lhs, const EncoderTestSequenceWithUntagged2& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
    // the same value, and 'false' otherwise.  Two attribute objects have the
    // same value if each respective attribute has the same value.

inline
bool operator!=(const EncoderTestSequenceWithUntagged2& lhs, const EncoderTestSequenceWithUntagged2& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
    // have the same value, and 'false' otherwise.  Two attribute objects do
    // not have the same value if one or more respective attributes differ in
    // values.

inline
bsl::ostream& operator<<(bsl::ostream& stream, const EncoderTestSequenceWithUntagged2& rhs);
    // Format the specified 'rhs' to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& hashAlg, const baljsn::EncoderTestSequenceWithUntagged2& object);
    // Pass the specified 'object' to the specified 'hashAlg'.  This function
    // integrates with the 'bslh' modular hashing system and effectively
    // provides a 'bsl::hash' specialization for 'EncoderTestSequenceWithUntagged2'.

}  // close package namespace

// TRAITS

BDLAT_DECL_SEQUENCE_WITH_BITWISEMOVEABLE_TRAITS(baljsn::EncoderTestSequenceWithUntagged2)

namespace baljsn {

               // ==============================================
               // class EncoderTestSequenceWithUntagged3Sequence
               // ==============================================

class EncoderTestSequenceWithUntagged3Sequence {

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
    EncoderTestSequenceWithUntagged3Sequence();
        // Create an object of type 'EncoderTestSequenceWithUntagged3Sequence'
        // having the default value.

    EncoderTestSequenceWithUntagged3Sequence(const EncoderTestSequenceWithUntagged3Sequence& original);
        // Create an object of type 'EncoderTestSequenceWithUntagged3Sequence'
        // having the value of the specified 'original' object.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    EncoderTestSequenceWithUntagged3Sequence(EncoderTestSequenceWithUntagged3Sequence&& original) = default;
        // Create an object of type 'EncoderTestSequenceWithUntagged3Sequence'
        // having the value of the specified 'original' object.  After
        // performing this action, the 'original' object will be left in a
        // valid, but unspecified state.
#endif

    ~EncoderTestSequenceWithUntagged3Sequence();
        // Destroy this object.

    // MANIPULATORS
    EncoderTestSequenceWithUntagged3Sequence& operator=(const EncoderTestSequenceWithUntagged3Sequence& rhs);
        // Assign to this object the value of the specified 'rhs' object.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    EncoderTestSequenceWithUntagged3Sequence& operator=(EncoderTestSequenceWithUntagged3Sequence&& rhs);
        // Assign to this object the value of the specified 'rhs' object.
        // After performing this action, the 'rhs' object will be left in a
        // valid, but unspecified state.
#endif

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
bool operator==(const EncoderTestSequenceWithUntagged3Sequence& lhs, const EncoderTestSequenceWithUntagged3Sequence& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
    // the same value, and 'false' otherwise.  Two attribute objects have the
    // same value if each respective attribute has the same value.

inline
bool operator!=(const EncoderTestSequenceWithUntagged3Sequence& lhs, const EncoderTestSequenceWithUntagged3Sequence& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
    // have the same value, and 'false' otherwise.  Two attribute objects do
    // not have the same value if one or more respective attributes differ in
    // values.

inline
bsl::ostream& operator<<(bsl::ostream& stream, const EncoderTestSequenceWithUntagged3Sequence& rhs);
    // Format the specified 'rhs' to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& hashAlg, const baljsn::EncoderTestSequenceWithUntagged3Sequence& object);
    // Pass the specified 'object' to the specified 'hashAlg'.  This function
    // integrates with the 'bslh' modular hashing system and effectively
    // provides a 'bsl::hash' specialization for 'EncoderTestSequenceWithUntagged3Sequence'.

}  // close package namespace

// TRAITS

BDLAT_DECL_SEQUENCE_WITH_BITWISEMOVEABLE_TRAITS(baljsn::EncoderTestSequenceWithUntagged3Sequence)

namespace baljsn {

              // ===============================================
              // class EncoderTestSequenceWithUntagged3Sequence1
              // ===============================================

class EncoderTestSequenceWithUntagged3Sequence1 {

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
    EncoderTestSequenceWithUntagged3Sequence1();
        // Create an object of type 'EncoderTestSequenceWithUntagged3Sequence1'
        // having the default value.

    EncoderTestSequenceWithUntagged3Sequence1(const EncoderTestSequenceWithUntagged3Sequence1& original);
        // Create an object of type 'EncoderTestSequenceWithUntagged3Sequence1'
        // having the value of the specified 'original' object.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    EncoderTestSequenceWithUntagged3Sequence1(EncoderTestSequenceWithUntagged3Sequence1&& original) = default;
        // Create an object of type 'EncoderTestSequenceWithUntagged3Sequence1'
        // having the value of the specified 'original' object.  After
        // performing this action, the 'original' object will be left in a
        // valid, but unspecified state.
#endif

    ~EncoderTestSequenceWithUntagged3Sequence1();
        // Destroy this object.

    // MANIPULATORS
    EncoderTestSequenceWithUntagged3Sequence1& operator=(const EncoderTestSequenceWithUntagged3Sequence1& rhs);
        // Assign to this object the value of the specified 'rhs' object.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    EncoderTestSequenceWithUntagged3Sequence1& operator=(EncoderTestSequenceWithUntagged3Sequence1&& rhs);
        // Assign to this object the value of the specified 'rhs' object.
        // After performing this action, the 'rhs' object will be left in a
        // valid, but unspecified state.
#endif

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
bool operator==(const EncoderTestSequenceWithUntagged3Sequence1& lhs, const EncoderTestSequenceWithUntagged3Sequence1& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
    // the same value, and 'false' otherwise.  Two attribute objects have the
    // same value if each respective attribute has the same value.

inline
bool operator!=(const EncoderTestSequenceWithUntagged3Sequence1& lhs, const EncoderTestSequenceWithUntagged3Sequence1& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
    // have the same value, and 'false' otherwise.  Two attribute objects do
    // not have the same value if one or more respective attributes differ in
    // values.

inline
bsl::ostream& operator<<(bsl::ostream& stream, const EncoderTestSequenceWithUntagged3Sequence1& rhs);
    // Format the specified 'rhs' to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& hashAlg, const baljsn::EncoderTestSequenceWithUntagged3Sequence1& object);
    // Pass the specified 'object' to the specified 'hashAlg'.  This function
    // integrates with the 'bslh' modular hashing system and effectively
    // provides a 'bsl::hash' specialization for 'EncoderTestSequenceWithUntagged3Sequence1'.

}  // close package namespace

// TRAITS

BDLAT_DECL_SEQUENCE_WITH_BITWISEMOVEABLE_TRAITS(baljsn::EncoderTestSequenceWithUntagged3Sequence1)

namespace baljsn {

               // ==============================================
               // class EncoderTestSequenceWithUntagged4Sequence
               // ==============================================

class EncoderTestSequenceWithUntagged4Sequence {

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
    EncoderTestSequenceWithUntagged4Sequence();
        // Create an object of type 'EncoderTestSequenceWithUntagged4Sequence'
        // having the default value.

    EncoderTestSequenceWithUntagged4Sequence(const EncoderTestSequenceWithUntagged4Sequence& original);
        // Create an object of type 'EncoderTestSequenceWithUntagged4Sequence'
        // having the value of the specified 'original' object.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    EncoderTestSequenceWithUntagged4Sequence(EncoderTestSequenceWithUntagged4Sequence&& original) = default;
        // Create an object of type 'EncoderTestSequenceWithUntagged4Sequence'
        // having the value of the specified 'original' object.  After
        // performing this action, the 'original' object will be left in a
        // valid, but unspecified state.
#endif

    ~EncoderTestSequenceWithUntagged4Sequence();
        // Destroy this object.

    // MANIPULATORS
    EncoderTestSequenceWithUntagged4Sequence& operator=(const EncoderTestSequenceWithUntagged4Sequence& rhs);
        // Assign to this object the value of the specified 'rhs' object.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    EncoderTestSequenceWithUntagged4Sequence& operator=(EncoderTestSequenceWithUntagged4Sequence&& rhs);
        // Assign to this object the value of the specified 'rhs' object.
        // After performing this action, the 'rhs' object will be left in a
        // valid, but unspecified state.
#endif

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
bool operator==(const EncoderTestSequenceWithUntagged4Sequence& lhs, const EncoderTestSequenceWithUntagged4Sequence& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
    // the same value, and 'false' otherwise.  Two attribute objects have the
    // same value if each respective attribute has the same value.

inline
bool operator!=(const EncoderTestSequenceWithUntagged4Sequence& lhs, const EncoderTestSequenceWithUntagged4Sequence& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
    // have the same value, and 'false' otherwise.  Two attribute objects do
    // not have the same value if one or more respective attributes differ in
    // values.

inline
bsl::ostream& operator<<(bsl::ostream& stream, const EncoderTestSequenceWithUntagged4Sequence& rhs);
    // Format the specified 'rhs' to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& hashAlg, const baljsn::EncoderTestSequenceWithUntagged4Sequence& object);
    // Pass the specified 'object' to the specified 'hashAlg'.  This function
    // integrates with the 'bslh' modular hashing system and effectively
    // provides a 'bsl::hash' specialization for 'EncoderTestSequenceWithUntagged4Sequence'.

}  // close package namespace

// TRAITS

BDLAT_DECL_SEQUENCE_WITH_BITWISEMOVEABLE_TRAITS(baljsn::EncoderTestSequenceWithUntagged4Sequence)

namespace baljsn {

               // ==============================================
               // class EncoderTestSequenceWithUntagged5Sequence
               // ==============================================

class EncoderTestSequenceWithUntagged5Sequence {

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
    EncoderTestSequenceWithUntagged5Sequence();
        // Create an object of type 'EncoderTestSequenceWithUntagged5Sequence'
        // having the default value.

    EncoderTestSequenceWithUntagged5Sequence(const EncoderTestSequenceWithUntagged5Sequence& original);
        // Create an object of type 'EncoderTestSequenceWithUntagged5Sequence'
        // having the value of the specified 'original' object.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    EncoderTestSequenceWithUntagged5Sequence(EncoderTestSequenceWithUntagged5Sequence&& original) = default;
        // Create an object of type 'EncoderTestSequenceWithUntagged5Sequence'
        // having the value of the specified 'original' object.  After
        // performing this action, the 'original' object will be left in a
        // valid, but unspecified state.
#endif

    ~EncoderTestSequenceWithUntagged5Sequence();
        // Destroy this object.

    // MANIPULATORS
    EncoderTestSequenceWithUntagged5Sequence& operator=(const EncoderTestSequenceWithUntagged5Sequence& rhs);
        // Assign to this object the value of the specified 'rhs' object.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    EncoderTestSequenceWithUntagged5Sequence& operator=(EncoderTestSequenceWithUntagged5Sequence&& rhs);
        // Assign to this object the value of the specified 'rhs' object.
        // After performing this action, the 'rhs' object will be left in a
        // valid, but unspecified state.
#endif

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
bool operator==(const EncoderTestSequenceWithUntagged5Sequence& lhs, const EncoderTestSequenceWithUntagged5Sequence& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
    // the same value, and 'false' otherwise.  Two attribute objects have the
    // same value if each respective attribute has the same value.

inline
bool operator!=(const EncoderTestSequenceWithUntagged5Sequence& lhs, const EncoderTestSequenceWithUntagged5Sequence& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
    // have the same value, and 'false' otherwise.  Two attribute objects do
    // not have the same value if one or more respective attributes differ in
    // values.

inline
bsl::ostream& operator<<(bsl::ostream& stream, const EncoderTestSequenceWithUntagged5Sequence& rhs);
    // Format the specified 'rhs' to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& hashAlg, const baljsn::EncoderTestSequenceWithUntagged5Sequence& object);
    // Pass the specified 'object' to the specified 'hashAlg'.  This function
    // integrates with the 'bslh' modular hashing system and effectively
    // provides a 'bsl::hash' specialization for 'EncoderTestSequenceWithUntagged5Sequence'.

}  // close package namespace

// TRAITS

BDLAT_DECL_SEQUENCE_WITH_BITWISEMOVEABLE_TRAITS(baljsn::EncoderTestSequenceWithUntagged5Sequence)

namespace baljsn {

                   // ======================================
                   // class EncoderTestSequenceWithUntagged6
                   // ======================================

class EncoderTestSequenceWithUntagged6 {

    // INSTANCE DATA
    int  d_attribute0;
    int  d_attribute1;

  public:
    // TYPES
    enum {
        ATTRIBUTE_ID_ATTRIBUTE0 = 0
      , ATTRIBUTE_ID_ATTRIBUTE1 = 1
    };

    enum {
        NUM_ATTRIBUTES = 2
    };

    enum {
        ATTRIBUTE_INDEX_ATTRIBUTE0 = 0
      , ATTRIBUTE_INDEX_ATTRIBUTE1 = 1
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
    EncoderTestSequenceWithUntagged6();
        // Create an object of type 'EncoderTestSequenceWithUntagged6' having
        // the default value.

    EncoderTestSequenceWithUntagged6(const EncoderTestSequenceWithUntagged6& original);
        // Create an object of type 'EncoderTestSequenceWithUntagged6' having
        // the value of the specified 'original' object.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    EncoderTestSequenceWithUntagged6(EncoderTestSequenceWithUntagged6&& original) = default;
        // Create an object of type 'EncoderTestSequenceWithUntagged6' having
        // the value of the specified 'original' object.  After performing this
        // action, the 'original' object will be left in a valid, but
        // unspecified state.
#endif

    ~EncoderTestSequenceWithUntagged6();
        // Destroy this object.

    // MANIPULATORS
    EncoderTestSequenceWithUntagged6& operator=(const EncoderTestSequenceWithUntagged6& rhs);
        // Assign to this object the value of the specified 'rhs' object.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    EncoderTestSequenceWithUntagged6& operator=(EncoderTestSequenceWithUntagged6&& rhs);
        // Assign to this object the value of the specified 'rhs' object.
        // After performing this action, the 'rhs' object will be left in a
        // valid, but unspecified state.
#endif

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

    int& attribute0();
        // Return a reference to the modifiable "Attribute0" attribute of this
        // object.

    int& attribute1();
        // Return a reference to the modifiable "Attribute1" attribute of this
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

    int attribute0() const;
        // Return a reference to the non-modifiable "Attribute0" attribute of
        // this object.

    int attribute1() const;
        // Return a reference to the non-modifiable "Attribute1" attribute of
        // this object.
};

// FREE OPERATORS
inline
bool operator==(const EncoderTestSequenceWithUntagged6& lhs, const EncoderTestSequenceWithUntagged6& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
    // the same value, and 'false' otherwise.  Two attribute objects have the
    // same value if each respective attribute has the same value.

inline
bool operator!=(const EncoderTestSequenceWithUntagged6& lhs, const EncoderTestSequenceWithUntagged6& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
    // have the same value, and 'false' otherwise.  Two attribute objects do
    // not have the same value if one or more respective attributes differ in
    // values.

inline
bsl::ostream& operator<<(bsl::ostream& stream, const EncoderTestSequenceWithUntagged6& rhs);
    // Format the specified 'rhs' to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& hashAlg, const baljsn::EncoderTestSequenceWithUntagged6& object);
    // Pass the specified 'object' to the specified 'hashAlg'.  This function
    // integrates with the 'bslh' modular hashing system and effectively
    // provides a 'bsl::hash' specialization for 'EncoderTestSequenceWithUntagged6'.

}  // close package namespace

// TRAITS

BDLAT_DECL_SEQUENCE_WITH_BITWISEMOVEABLE_TRAITS(baljsn::EncoderTestSequenceWithUntagged6)

namespace baljsn {

               // ==============================================
               // class EncoderTestSequenceWithUntagged7Sequence
               // ==============================================

class EncoderTestSequenceWithUntagged7Sequence {

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
    EncoderTestSequenceWithUntagged7Sequence();
        // Create an object of type 'EncoderTestSequenceWithUntagged7Sequence'
        // having the default value.

    EncoderTestSequenceWithUntagged7Sequence(const EncoderTestSequenceWithUntagged7Sequence& original);
        // Create an object of type 'EncoderTestSequenceWithUntagged7Sequence'
        // having the value of the specified 'original' object.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    EncoderTestSequenceWithUntagged7Sequence(EncoderTestSequenceWithUntagged7Sequence&& original) = default;
        // Create an object of type 'EncoderTestSequenceWithUntagged7Sequence'
        // having the value of the specified 'original' object.  After
        // performing this action, the 'original' object will be left in a
        // valid, but unspecified state.
#endif

    ~EncoderTestSequenceWithUntagged7Sequence();
        // Destroy this object.

    // MANIPULATORS
    EncoderTestSequenceWithUntagged7Sequence& operator=(const EncoderTestSequenceWithUntagged7Sequence& rhs);
        // Assign to this object the value of the specified 'rhs' object.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    EncoderTestSequenceWithUntagged7Sequence& operator=(EncoderTestSequenceWithUntagged7Sequence&& rhs);
        // Assign to this object the value of the specified 'rhs' object.
        // After performing this action, the 'rhs' object will be left in a
        // valid, but unspecified state.
#endif

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
bool operator==(const EncoderTestSequenceWithUntagged7Sequence& lhs, const EncoderTestSequenceWithUntagged7Sequence& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
    // the same value, and 'false' otherwise.  Two attribute objects have the
    // same value if each respective attribute has the same value.

inline
bool operator!=(const EncoderTestSequenceWithUntagged7Sequence& lhs, const EncoderTestSequenceWithUntagged7Sequence& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
    // have the same value, and 'false' otherwise.  Two attribute objects do
    // not have the same value if one or more respective attributes differ in
    // values.

inline
bsl::ostream& operator<<(bsl::ostream& stream, const EncoderTestSequenceWithUntagged7Sequence& rhs);
    // Format the specified 'rhs' to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& hashAlg, const baljsn::EncoderTestSequenceWithUntagged7Sequence& object);
    // Pass the specified 'object' to the specified 'hashAlg'.  This function
    // integrates with the 'bslh' modular hashing system and effectively
    // provides a 'bsl::hash' specialization for 'EncoderTestSequenceWithUntagged7Sequence'.

}  // close package namespace

// TRAITS

BDLAT_DECL_SEQUENCE_WITH_BITWISEMOVEABLE_TRAITS(baljsn::EncoderTestSequenceWithUntagged7Sequence)

namespace baljsn {

              // ===============================================
              // class EncoderTestSequenceWithUntagged7Sequence1
              // ===============================================

class EncoderTestSequenceWithUntagged7Sequence1 {

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
    EncoderTestSequenceWithUntagged7Sequence1();
        // Create an object of type 'EncoderTestSequenceWithUntagged7Sequence1'
        // having the default value.

    EncoderTestSequenceWithUntagged7Sequence1(const EncoderTestSequenceWithUntagged7Sequence1& original);
        // Create an object of type 'EncoderTestSequenceWithUntagged7Sequence1'
        // having the value of the specified 'original' object.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    EncoderTestSequenceWithUntagged7Sequence1(EncoderTestSequenceWithUntagged7Sequence1&& original) = default;
        // Create an object of type 'EncoderTestSequenceWithUntagged7Sequence1'
        // having the value of the specified 'original' object.  After
        // performing this action, the 'original' object will be left in a
        // valid, but unspecified state.
#endif

    ~EncoderTestSequenceWithUntagged7Sequence1();
        // Destroy this object.

    // MANIPULATORS
    EncoderTestSequenceWithUntagged7Sequence1& operator=(const EncoderTestSequenceWithUntagged7Sequence1& rhs);
        // Assign to this object the value of the specified 'rhs' object.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    EncoderTestSequenceWithUntagged7Sequence1& operator=(EncoderTestSequenceWithUntagged7Sequence1&& rhs);
        // Assign to this object the value of the specified 'rhs' object.
        // After performing this action, the 'rhs' object will be left in a
        // valid, but unspecified state.
#endif

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
bool operator==(const EncoderTestSequenceWithUntagged7Sequence1& lhs, const EncoderTestSequenceWithUntagged7Sequence1& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
    // the same value, and 'false' otherwise.  Two attribute objects have the
    // same value if each respective attribute has the same value.

inline
bool operator!=(const EncoderTestSequenceWithUntagged7Sequence1& lhs, const EncoderTestSequenceWithUntagged7Sequence1& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
    // have the same value, and 'false' otherwise.  Two attribute objects do
    // not have the same value if one or more respective attributes differ in
    // values.

inline
bsl::ostream& operator<<(bsl::ostream& stream, const EncoderTestSequenceWithUntagged7Sequence1& rhs);
    // Format the specified 'rhs' to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& hashAlg, const baljsn::EncoderTestSequenceWithUntagged7Sequence1& object);
    // Pass the specified 'object' to the specified 'hashAlg'.  This function
    // integrates with the 'bslh' modular hashing system and effectively
    // provides a 'bsl::hash' specialization for 'EncoderTestSequenceWithUntagged7Sequence1'.

}  // close package namespace

// TRAITS

BDLAT_DECL_SEQUENCE_WITH_BITWISEMOVEABLE_TRAITS(baljsn::EncoderTestSequenceWithUntagged7Sequence1)

namespace baljsn {

              // ===============================================
              // class EncoderTestSequenceWithUntagged7Sequence2
              // ===============================================

class EncoderTestSequenceWithUntagged7Sequence2 {

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
    EncoderTestSequenceWithUntagged7Sequence2();
        // Create an object of type 'EncoderTestSequenceWithUntagged7Sequence2'
        // having the default value.

    EncoderTestSequenceWithUntagged7Sequence2(const EncoderTestSequenceWithUntagged7Sequence2& original);
        // Create an object of type 'EncoderTestSequenceWithUntagged7Sequence2'
        // having the value of the specified 'original' object.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    EncoderTestSequenceWithUntagged7Sequence2(EncoderTestSequenceWithUntagged7Sequence2&& original) = default;
        // Create an object of type 'EncoderTestSequenceWithUntagged7Sequence2'
        // having the value of the specified 'original' object.  After
        // performing this action, the 'original' object will be left in a
        // valid, but unspecified state.
#endif

    ~EncoderTestSequenceWithUntagged7Sequence2();
        // Destroy this object.

    // MANIPULATORS
    EncoderTestSequenceWithUntagged7Sequence2& operator=(const EncoderTestSequenceWithUntagged7Sequence2& rhs);
        // Assign to this object the value of the specified 'rhs' object.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    EncoderTestSequenceWithUntagged7Sequence2& operator=(EncoderTestSequenceWithUntagged7Sequence2&& rhs);
        // Assign to this object the value of the specified 'rhs' object.
        // After performing this action, the 'rhs' object will be left in a
        // valid, but unspecified state.
#endif

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
bool operator==(const EncoderTestSequenceWithUntagged7Sequence2& lhs, const EncoderTestSequenceWithUntagged7Sequence2& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
    // the same value, and 'false' otherwise.  Two attribute objects have the
    // same value if each respective attribute has the same value.

inline
bool operator!=(const EncoderTestSequenceWithUntagged7Sequence2& lhs, const EncoderTestSequenceWithUntagged7Sequence2& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
    // have the same value, and 'false' otherwise.  Two attribute objects do
    // not have the same value if one or more respective attributes differ in
    // values.

inline
bsl::ostream& operator<<(bsl::ostream& stream, const EncoderTestSequenceWithUntagged7Sequence2& rhs);
    // Format the specified 'rhs' to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& hashAlg, const baljsn::EncoderTestSequenceWithUntagged7Sequence2& object);
    // Pass the specified 'object' to the specified 'hashAlg'.  This function
    // integrates with the 'bslh' modular hashing system and effectively
    // provides a 'bsl::hash' specialization for 'EncoderTestSequenceWithUntagged7Sequence2'.

}  // close package namespace

// TRAITS

BDLAT_DECL_SEQUENCE_WITH_BITWISEMOVEABLE_TRAITS(baljsn::EncoderTestSequenceWithUntagged7Sequence2)

namespace baljsn {

               // ==============================================
               // class EncoderTestSequenceWithUntagged8Sequence
               // ==============================================

class EncoderTestSequenceWithUntagged8Sequence {

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
    EncoderTestSequenceWithUntagged8Sequence();
        // Create an object of type 'EncoderTestSequenceWithUntagged8Sequence'
        // having the default value.

    EncoderTestSequenceWithUntagged8Sequence(const EncoderTestSequenceWithUntagged8Sequence& original);
        // Create an object of type 'EncoderTestSequenceWithUntagged8Sequence'
        // having the value of the specified 'original' object.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    EncoderTestSequenceWithUntagged8Sequence(EncoderTestSequenceWithUntagged8Sequence&& original) = default;
        // Create an object of type 'EncoderTestSequenceWithUntagged8Sequence'
        // having the value of the specified 'original' object.  After
        // performing this action, the 'original' object will be left in a
        // valid, but unspecified state.
#endif

    ~EncoderTestSequenceWithUntagged8Sequence();
        // Destroy this object.

    // MANIPULATORS
    EncoderTestSequenceWithUntagged8Sequence& operator=(const EncoderTestSequenceWithUntagged8Sequence& rhs);
        // Assign to this object the value of the specified 'rhs' object.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    EncoderTestSequenceWithUntagged8Sequence& operator=(EncoderTestSequenceWithUntagged8Sequence&& rhs);
        // Assign to this object the value of the specified 'rhs' object.
        // After performing this action, the 'rhs' object will be left in a
        // valid, but unspecified state.
#endif

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
bool operator==(const EncoderTestSequenceWithUntagged8Sequence& lhs, const EncoderTestSequenceWithUntagged8Sequence& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
    // the same value, and 'false' otherwise.  Two attribute objects have the
    // same value if each respective attribute has the same value.

inline
bool operator!=(const EncoderTestSequenceWithUntagged8Sequence& lhs, const EncoderTestSequenceWithUntagged8Sequence& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
    // have the same value, and 'false' otherwise.  Two attribute objects do
    // not have the same value if one or more respective attributes differ in
    // values.

inline
bsl::ostream& operator<<(bsl::ostream& stream, const EncoderTestSequenceWithUntagged8Sequence& rhs);
    // Format the specified 'rhs' to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& hashAlg, const baljsn::EncoderTestSequenceWithUntagged8Sequence& object);
    // Pass the specified 'object' to the specified 'hashAlg'.  This function
    // integrates with the 'bslh' modular hashing system and effectively
    // provides a 'bsl::hash' specialization for 'EncoderTestSequenceWithUntagged8Sequence'.

}  // close package namespace

// TRAITS

BDLAT_DECL_SEQUENCE_WITH_BITWISEMOVEABLE_TRAITS(baljsn::EncoderTestSequenceWithUntagged8Sequence)

namespace baljsn {

              // ===============================================
              // class EncoderTestSequenceWithUntagged8Sequence1
              // ===============================================

class EncoderTestSequenceWithUntagged8Sequence1 {

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
    EncoderTestSequenceWithUntagged8Sequence1();
        // Create an object of type 'EncoderTestSequenceWithUntagged8Sequence1'
        // having the default value.

    EncoderTestSequenceWithUntagged8Sequence1(const EncoderTestSequenceWithUntagged8Sequence1& original);
        // Create an object of type 'EncoderTestSequenceWithUntagged8Sequence1'
        // having the value of the specified 'original' object.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    EncoderTestSequenceWithUntagged8Sequence1(EncoderTestSequenceWithUntagged8Sequence1&& original) = default;
        // Create an object of type 'EncoderTestSequenceWithUntagged8Sequence1'
        // having the value of the specified 'original' object.  After
        // performing this action, the 'original' object will be left in a
        // valid, but unspecified state.
#endif

    ~EncoderTestSequenceWithUntagged8Sequence1();
        // Destroy this object.

    // MANIPULATORS
    EncoderTestSequenceWithUntagged8Sequence1& operator=(const EncoderTestSequenceWithUntagged8Sequence1& rhs);
        // Assign to this object the value of the specified 'rhs' object.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    EncoderTestSequenceWithUntagged8Sequence1& operator=(EncoderTestSequenceWithUntagged8Sequence1&& rhs);
        // Assign to this object the value of the specified 'rhs' object.
        // After performing this action, the 'rhs' object will be left in a
        // valid, but unspecified state.
#endif

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
bool operator==(const EncoderTestSequenceWithUntagged8Sequence1& lhs, const EncoderTestSequenceWithUntagged8Sequence1& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
    // the same value, and 'false' otherwise.  Two attribute objects have the
    // same value if each respective attribute has the same value.

inline
bool operator!=(const EncoderTestSequenceWithUntagged8Sequence1& lhs, const EncoderTestSequenceWithUntagged8Sequence1& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
    // have the same value, and 'false' otherwise.  Two attribute objects do
    // not have the same value if one or more respective attributes differ in
    // values.

inline
bsl::ostream& operator<<(bsl::ostream& stream, const EncoderTestSequenceWithUntagged8Sequence1& rhs);
    // Format the specified 'rhs' to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& hashAlg, const baljsn::EncoderTestSequenceWithUntagged8Sequence1& object);
    // Pass the specified 'object' to the specified 'hashAlg'.  This function
    // integrates with the 'bslh' modular hashing system and effectively
    // provides a 'bsl::hash' specialization for 'EncoderTestSequenceWithUntagged8Sequence1'.

}  // close package namespace

// TRAITS

BDLAT_DECL_SEQUENCE_WITH_BITWISEMOVEABLE_TRAITS(baljsn::EncoderTestSequenceWithUntagged8Sequence1)

namespace baljsn {

               // ==============================================
               // class EncoderTestSequenceWithUntagged9Sequence
               // ==============================================

class EncoderTestSequenceWithUntagged9Sequence {

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
    EncoderTestSequenceWithUntagged9Sequence();
        // Create an object of type 'EncoderTestSequenceWithUntagged9Sequence'
        // having the default value.

    EncoderTestSequenceWithUntagged9Sequence(const EncoderTestSequenceWithUntagged9Sequence& original);
        // Create an object of type 'EncoderTestSequenceWithUntagged9Sequence'
        // having the value of the specified 'original' object.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    EncoderTestSequenceWithUntagged9Sequence(EncoderTestSequenceWithUntagged9Sequence&& original) = default;
        // Create an object of type 'EncoderTestSequenceWithUntagged9Sequence'
        // having the value of the specified 'original' object.  After
        // performing this action, the 'original' object will be left in a
        // valid, but unspecified state.
#endif

    ~EncoderTestSequenceWithUntagged9Sequence();
        // Destroy this object.

    // MANIPULATORS
    EncoderTestSequenceWithUntagged9Sequence& operator=(const EncoderTestSequenceWithUntagged9Sequence& rhs);
        // Assign to this object the value of the specified 'rhs' object.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    EncoderTestSequenceWithUntagged9Sequence& operator=(EncoderTestSequenceWithUntagged9Sequence&& rhs);
        // Assign to this object the value of the specified 'rhs' object.
        // After performing this action, the 'rhs' object will be left in a
        // valid, but unspecified state.
#endif

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
bool operator==(const EncoderTestSequenceWithUntagged9Sequence& lhs, const EncoderTestSequenceWithUntagged9Sequence& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
    // the same value, and 'false' otherwise.  Two attribute objects have the
    // same value if each respective attribute has the same value.

inline
bool operator!=(const EncoderTestSequenceWithUntagged9Sequence& lhs, const EncoderTestSequenceWithUntagged9Sequence& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
    // have the same value, and 'false' otherwise.  Two attribute objects do
    // not have the same value if one or more respective attributes differ in
    // values.

inline
bsl::ostream& operator<<(bsl::ostream& stream, const EncoderTestSequenceWithUntagged9Sequence& rhs);
    // Format the specified 'rhs' to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& hashAlg, const baljsn::EncoderTestSequenceWithUntagged9Sequence& object);
    // Pass the specified 'object' to the specified 'hashAlg'.  This function
    // integrates with the 'bslh' modular hashing system and effectively
    // provides a 'bsl::hash' specialization for 'EncoderTestSequenceWithUntagged9Sequence'.

}  // close package namespace

// TRAITS

BDLAT_DECL_SEQUENCE_WITH_BITWISEMOVEABLE_TRAITS(baljsn::EncoderTestSequenceWithUntagged9Sequence)

namespace baljsn {

              // ===============================================
              // class EncoderTestSequenceWithUntagged9Sequence1
              // ===============================================

class EncoderTestSequenceWithUntagged9Sequence1 {

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
    EncoderTestSequenceWithUntagged9Sequence1();
        // Create an object of type 'EncoderTestSequenceWithUntagged9Sequence1'
        // having the default value.

    EncoderTestSequenceWithUntagged9Sequence1(const EncoderTestSequenceWithUntagged9Sequence1& original);
        // Create an object of type 'EncoderTestSequenceWithUntagged9Sequence1'
        // having the value of the specified 'original' object.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    EncoderTestSequenceWithUntagged9Sequence1(EncoderTestSequenceWithUntagged9Sequence1&& original) = default;
        // Create an object of type 'EncoderTestSequenceWithUntagged9Sequence1'
        // having the value of the specified 'original' object.  After
        // performing this action, the 'original' object will be left in a
        // valid, but unspecified state.
#endif

    ~EncoderTestSequenceWithUntagged9Sequence1();
        // Destroy this object.

    // MANIPULATORS
    EncoderTestSequenceWithUntagged9Sequence1& operator=(const EncoderTestSequenceWithUntagged9Sequence1& rhs);
        // Assign to this object the value of the specified 'rhs' object.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    EncoderTestSequenceWithUntagged9Sequence1& operator=(EncoderTestSequenceWithUntagged9Sequence1&& rhs);
        // Assign to this object the value of the specified 'rhs' object.
        // After performing this action, the 'rhs' object will be left in a
        // valid, but unspecified state.
#endif

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
bool operator==(const EncoderTestSequenceWithUntagged9Sequence1& lhs, const EncoderTestSequenceWithUntagged9Sequence1& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
    // the same value, and 'false' otherwise.  Two attribute objects have the
    // same value if each respective attribute has the same value.

inline
bool operator!=(const EncoderTestSequenceWithUntagged9Sequence1& lhs, const EncoderTestSequenceWithUntagged9Sequence1& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
    // have the same value, and 'false' otherwise.  Two attribute objects do
    // not have the same value if one or more respective attributes differ in
    // values.

inline
bsl::ostream& operator<<(bsl::ostream& stream, const EncoderTestSequenceWithUntagged9Sequence1& rhs);
    // Format the specified 'rhs' to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& hashAlg, const baljsn::EncoderTestSequenceWithUntagged9Sequence1& object);
    // Pass the specified 'object' to the specified 'hashAlg'.  This function
    // integrates with the 'bslh' modular hashing system and effectively
    // provides a 'bsl::hash' specialization for 'EncoderTestSequenceWithUntagged9Sequence1'.

}  // close package namespace

// TRAITS

BDLAT_DECL_SEQUENCE_WITH_BITWISEMOVEABLE_TRAITS(baljsn::EncoderTestSequenceWithUntagged9Sequence1)

namespace baljsn {

                  // ========================================
                  // class EncoderTestChoiceWithAllCategories
                  // ========================================

class EncoderTestChoiceWithAllCategories {

    // INSTANCE DATA
    union {
        bsls::ObjectBuffer< bsl::vector<char> >                                    d_charArray;
        bsls::ObjectBuffer< EncoderTestChoiceWithAllCategoriesChoice >             d_choice;
        bsls::ObjectBuffer< EncoderTestChoiceWithAllCategoriesCustomizedType >     d_customizedType;
        bsls::ObjectBuffer< EncoderTestChoiceWithAllCategoriesEnumeration::Value > d_enumeration;
        bsls::ObjectBuffer< EncoderTestChoiceWithAllCategoriesSequence >           d_sequence;
        bsls::ObjectBuffer< int >                                                  d_simple;
    };

    int                                                                            d_selectionId;
    bslma::Allocator                                                              *d_allocator_p;

  public:
    // TYPES

    enum {
        SELECTION_ID_UNDEFINED       = -1
      , SELECTION_ID_CHAR_ARRAY      = 0
      , SELECTION_ID_CHOICE          = 1
      , SELECTION_ID_CUSTOMIZED_TYPE = 2
      , SELECTION_ID_ENUMERATION     = 3
      , SELECTION_ID_SEQUENCE        = 4
      , SELECTION_ID_SIMPLE          = 5
    };

    enum {
        NUM_SELECTIONS = 6
    };

    enum {
        SELECTION_INDEX_CHAR_ARRAY      = 0
      , SELECTION_INDEX_CHOICE          = 1
      , SELECTION_INDEX_CUSTOMIZED_TYPE = 2
      , SELECTION_INDEX_ENUMERATION     = 3
      , SELECTION_INDEX_SEQUENCE        = 4
      , SELECTION_INDEX_SIMPLE          = 5
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
    explicit EncoderTestChoiceWithAllCategories(bslma::Allocator *basicAllocator = 0);
        // Create an object of type 'EncoderTestChoiceWithAllCategories' having
        // the default value.  Use the optionally specified 'basicAllocator' to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.

    EncoderTestChoiceWithAllCategories(const EncoderTestChoiceWithAllCategories& original,
                                      bslma::Allocator *basicAllocator = 0);
        // Create an object of type 'EncoderTestChoiceWithAllCategories' having
        // the value of the specified 'original' object.  Use the optionally
        // specified 'basicAllocator' to supply memory.  If 'basicAllocator' is
        // 0, the currently installed default allocator is used.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    EncoderTestChoiceWithAllCategories(EncoderTestChoiceWithAllCategories&& original) noexcept;
        // Create an object of type 'EncoderTestChoiceWithAllCategories' having
        // the value of the specified 'original' object.  After performing this
        // action, the 'original' object will be left in a valid, but
        // unspecified state.

    EncoderTestChoiceWithAllCategories(EncoderTestChoiceWithAllCategories&& original,
                                      bslma::Allocator *basicAllocator);
        // Create an object of type 'EncoderTestChoiceWithAllCategories' having
        // the value of the specified 'original' object.  After performing this
        // action, the 'original' object will be left in a valid, but
        // unspecified state.  Use the optionally specified 'basicAllocator' to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.
#endif

    ~EncoderTestChoiceWithAllCategories();
        // Destroy this object.

    // MANIPULATORS
    EncoderTestChoiceWithAllCategories& operator=(const EncoderTestChoiceWithAllCategories& rhs);
        // Assign to this object the value of the specified 'rhs' object.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    EncoderTestChoiceWithAllCategories& operator=(EncoderTestChoiceWithAllCategories&& rhs);
        // Assign to this object the value of the specified 'rhs' object.
        // After performing this action, the 'rhs' object will be left in a
        // valid, but unspecified state.
#endif

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

    bsl::vector<char>& makeCharArray();
    bsl::vector<char>& makeCharArray(const bsl::vector<char>& value);
#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    bsl::vector<char>& makeCharArray(bsl::vector<char>&& value);
#endif
        // Set the value of this object to be a "CharArray" value.  Optionally
        // specify the 'value' of the "CharArray".  If 'value' is not
        // specified, the default "CharArray" value is used.

    EncoderTestChoiceWithAllCategoriesChoice& makeChoice();
    EncoderTestChoiceWithAllCategoriesChoice& makeChoice(const EncoderTestChoiceWithAllCategoriesChoice& value);
#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    EncoderTestChoiceWithAllCategoriesChoice& makeChoice(EncoderTestChoiceWithAllCategoriesChoice&& value);
#endif
        // Set the value of this object to be a "Choice" value.  Optionally
        // specify the 'value' of the "Choice".  If 'value' is not specified,
        // the default "Choice" value is used.

    EncoderTestChoiceWithAllCategoriesCustomizedType& makeCustomizedType();
    EncoderTestChoiceWithAllCategoriesCustomizedType& makeCustomizedType(const EncoderTestChoiceWithAllCategoriesCustomizedType& value);
#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    EncoderTestChoiceWithAllCategoriesCustomizedType& makeCustomizedType(EncoderTestChoiceWithAllCategoriesCustomizedType&& value);
#endif
        // Set the value of this object to be a "CustomizedType" value.
        // Optionally specify the 'value' of the "CustomizedType".  If 'value'
        // is not specified, the default "CustomizedType" value is used.

    EncoderTestChoiceWithAllCategoriesEnumeration::Value& makeEnumeration();
    EncoderTestChoiceWithAllCategoriesEnumeration::Value& makeEnumeration(EncoderTestChoiceWithAllCategoriesEnumeration::Value value);
        // Set the value of this object to be a "Enumeration" value.
        // Optionally specify the 'value' of the "Enumeration".  If 'value' is
        // not specified, the default "Enumeration" value is used.

    EncoderTestChoiceWithAllCategoriesSequence& makeSequence();
    EncoderTestChoiceWithAllCategoriesSequence& makeSequence(const EncoderTestChoiceWithAllCategoriesSequence& value);
#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    EncoderTestChoiceWithAllCategoriesSequence& makeSequence(EncoderTestChoiceWithAllCategoriesSequence&& value);
#endif
        // Set the value of this object to be a "Sequence" value.  Optionally
        // specify the 'value' of the "Sequence".  If 'value' is not specified,
        // the default "Sequence" value is used.

    int& makeSimple();
    int& makeSimple(int value);
        // Set the value of this object to be a "Simple" value.  Optionally
        // specify the 'value' of the "Simple".  If 'value' is not specified,
        // the default "Simple" value is used.

    template<class MANIPULATOR>
    int manipulateSelection(MANIPULATOR& manipulator);
        // Invoke the specified 'manipulator' on the address of the modifiable
        // selection, supplying 'manipulator' with the corresponding selection
        // information structure.  Return the value returned from the
        // invocation of 'manipulator' if this object has a defined selection,
        // and -1 otherwise.

    bsl::vector<char>& charArray();
        // Return a reference to the modifiable "CharArray" selection of this
        // object if "CharArray" is the current selection.  The behavior is
        // undefined unless "CharArray" is the selection of this object.

    EncoderTestChoiceWithAllCategoriesChoice& choice();
        // Return a reference to the modifiable "Choice" selection of this
        // object if "Choice" is the current selection.  The behavior is
        // undefined unless "Choice" is the selection of this object.

    EncoderTestChoiceWithAllCategoriesCustomizedType& customizedType();
        // Return a reference to the modifiable "CustomizedType" selection of
        // this object if "CustomizedType" is the current selection.  The
        // behavior is undefined unless "CustomizedType" is the selection of
        // this object.

    EncoderTestChoiceWithAllCategoriesEnumeration::Value& enumeration();
        // Return a reference to the modifiable "Enumeration" selection of this
        // object if "Enumeration" is the current selection.  The behavior is
        // undefined unless "Enumeration" is the selection of this object.

    EncoderTestChoiceWithAllCategoriesSequence& sequence();
        // Return a reference to the modifiable "Sequence" selection of this
        // object if "Sequence" is the current selection.  The behavior is
        // undefined unless "Sequence" is the selection of this object.

    int& simple();
        // Return a reference to the modifiable "Simple" selection of this
        // object if "Simple" is the current selection.  The behavior is
        // undefined unless "Simple" is the selection of this object.

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

    const bsl::vector<char>& charArray() const;
        // Return a reference to the non-modifiable "CharArray" selection of
        // this object if "CharArray" is the current selection.  The behavior
        // is undefined unless "CharArray" is the selection of this object.

    const EncoderTestChoiceWithAllCategoriesChoice& choice() const;
        // Return a reference to the non-modifiable "Choice" selection of this
        // object if "Choice" is the current selection.  The behavior is
        // undefined unless "Choice" is the selection of this object.

    const EncoderTestChoiceWithAllCategoriesCustomizedType& customizedType() const;
        // Return a reference to the non-modifiable "CustomizedType" selection
        // of this object if "CustomizedType" is the current selection.  The
        // behavior is undefined unless "CustomizedType" is the selection of
        // this object.

    const EncoderTestChoiceWithAllCategoriesEnumeration::Value& enumeration() const;
        // Return a reference to the non-modifiable "Enumeration" selection of
        // this object if "Enumeration" is the current selection.  The behavior
        // is undefined unless "Enumeration" is the selection of this object.

    const EncoderTestChoiceWithAllCategoriesSequence& sequence() const;
        // Return a reference to the non-modifiable "Sequence" selection of
        // this object if "Sequence" is the current selection.  The behavior is
        // undefined unless "Sequence" is the selection of this object.

    const int& simple() const;
        // Return a reference to the non-modifiable "Simple" selection of this
        // object if "Simple" is the current selection.  The behavior is
        // undefined unless "Simple" is the selection of this object.

    bool isCharArrayValue() const;
        // Return 'true' if the value of this object is a "CharArray" value,
        // and return 'false' otherwise.

    bool isChoiceValue() const;
        // Return 'true' if the value of this object is a "Choice" value, and
        // return 'false' otherwise.

    bool isCustomizedTypeValue() const;
        // Return 'true' if the value of this object is a "CustomizedType"
        // value, and return 'false' otherwise.

    bool isEnumerationValue() const;
        // Return 'true' if the value of this object is a "Enumeration" value,
        // and return 'false' otherwise.

    bool isSequenceValue() const;
        // Return 'true' if the value of this object is a "Sequence" value, and
        // return 'false' otherwise.

    bool isSimpleValue() const;
        // Return 'true' if the value of this object is a "Simple" value, and
        // return 'false' otherwise.

    bool isUndefinedValue() const;
        // Return 'true' if the value of this object is undefined, and 'false'
        // otherwise.

    const char *selectionName() const;
        // Return the symbolic name of the current selection of this object.
};

// FREE OPERATORS
inline
bool operator==(const EncoderTestChoiceWithAllCategories& lhs, const EncoderTestChoiceWithAllCategories& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects have the same
    // value, and 'false' otherwise.  Two 'EncoderTestChoiceWithAllCategories' objects have the same
    // value if either the selections in both objects have the same ids and
    // the same values, or both selections are undefined.

inline
bool operator!=(const EncoderTestChoiceWithAllCategories& lhs, const EncoderTestChoiceWithAllCategories& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects do not have the
    // same values, as determined by 'operator==', and 'false' otherwise.

inline
bsl::ostream& operator<<(bsl::ostream& stream, const EncoderTestChoiceWithAllCategories& rhs);
    // Format the specified 'rhs' to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& hashAlg, const baljsn::EncoderTestChoiceWithAllCategories& object);
    // Pass the specified 'object' to the specified 'hashAlg'.  This function
    // integrates with the 'bslh' modular hashing system and effectively
    // provides a 'bsl::hash' specialization for 'EncoderTestChoiceWithAllCategories'.

}  // close package namespace

// TRAITS

BDLAT_DECL_CHOICE_WITH_ALLOCATOR_BITWISEMOVEABLE_TRAITS(baljsn::EncoderTestChoiceWithAllCategories)

namespace baljsn {

                     // ==================================
                     // class EncoderTestDegenerateChoice1
                     // ==================================

class EncoderTestDegenerateChoice1 {

    // INSTANCE DATA
    union {
        bsls::ObjectBuffer< EncoderTestDegenerateChoice1Sequence > d_sequence;
    };

    int                                                            d_selectionId;

  public:
    // TYPES

    enum {
        SELECTION_ID_UNDEFINED = -1
      , SELECTION_ID_SEQUENCE = 0
    };

    enum {
        NUM_SELECTIONS = 1
    };

    enum {
        SELECTION_INDEX_SEQUENCE = 0
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
    EncoderTestDegenerateChoice1();
        // Create an object of type 'EncoderTestDegenerateChoice1' having the
        // default value.

    EncoderTestDegenerateChoice1(const EncoderTestDegenerateChoice1& original);
        // Create an object of type 'EncoderTestDegenerateChoice1' having the
        // value of the specified 'original' object.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    EncoderTestDegenerateChoice1(EncoderTestDegenerateChoice1&& original) noexcept;
        // Create an object of type 'EncoderTestDegenerateChoice1' having the
        // value of the specified 'original' object.  After performing this
        // action, the 'original' object will be left in a valid, but
        // unspecified state.
#endif

    ~EncoderTestDegenerateChoice1();
        // Destroy this object.

    // MANIPULATORS
    EncoderTestDegenerateChoice1& operator=(const EncoderTestDegenerateChoice1& rhs);
        // Assign to this object the value of the specified 'rhs' object.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    EncoderTestDegenerateChoice1& operator=(EncoderTestDegenerateChoice1&& rhs);
        // Assign to this object the value of the specified 'rhs' object.
        // After performing this action, the 'rhs' object will be left in a
        // valid, but unspecified state.
#endif

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

    EncoderTestDegenerateChoice1Sequence& makeSequence();
    EncoderTestDegenerateChoice1Sequence& makeSequence(const EncoderTestDegenerateChoice1Sequence& value);
#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    EncoderTestDegenerateChoice1Sequence& makeSequence(EncoderTestDegenerateChoice1Sequence&& value);
#endif
        // Set the value of this object to be a "Sequence" value.  Optionally
        // specify the 'value' of the "Sequence".  If 'value' is not specified,
        // the default "Sequence" value is used.

    template<class MANIPULATOR>
    int manipulateSelection(MANIPULATOR& manipulator);
        // Invoke the specified 'manipulator' on the address of the modifiable
        // selection, supplying 'manipulator' with the corresponding selection
        // information structure.  Return the value returned from the
        // invocation of 'manipulator' if this object has a defined selection,
        // and -1 otherwise.

    EncoderTestDegenerateChoice1Sequence& sequence();
        // Return a reference to the modifiable "Sequence" selection of this
        // object if "Sequence" is the current selection.  The behavior is
        // undefined unless "Sequence" is the selection of this object.

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

    const EncoderTestDegenerateChoice1Sequence& sequence() const;
        // Return a reference to the non-modifiable "Sequence" selection of
        // this object if "Sequence" is the current selection.  The behavior is
        // undefined unless "Sequence" is the selection of this object.

    bool isSequenceValue() const;
        // Return 'true' if the value of this object is a "Sequence" value, and
        // return 'false' otherwise.

    bool isUndefinedValue() const;
        // Return 'true' if the value of this object is undefined, and 'false'
        // otherwise.

    const char *selectionName() const;
        // Return the symbolic name of the current selection of this object.
};

// FREE OPERATORS
inline
bool operator==(const EncoderTestDegenerateChoice1& lhs, const EncoderTestDegenerateChoice1& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects have the same
    // value, and 'false' otherwise.  Two 'EncoderTestDegenerateChoice1' objects have the same
    // value if either the selections in both objects have the same ids and
    // the same values, or both selections are undefined.

inline
bool operator!=(const EncoderTestDegenerateChoice1& lhs, const EncoderTestDegenerateChoice1& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects do not have the
    // same values, as determined by 'operator==', and 'false' otherwise.

inline
bsl::ostream& operator<<(bsl::ostream& stream, const EncoderTestDegenerateChoice1& rhs);
    // Format the specified 'rhs' to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& hashAlg, const baljsn::EncoderTestDegenerateChoice1& object);
    // Pass the specified 'object' to the specified 'hashAlg'.  This function
    // integrates with the 'bslh' modular hashing system and effectively
    // provides a 'bsl::hash' specialization for 'EncoderTestDegenerateChoice1'.

}  // close package namespace

// TRAITS

BDLAT_DECL_CHOICE_WITH_BITWISEMOVEABLE_TRAITS(baljsn::EncoderTestDegenerateChoice1)

namespace baljsn {

                         // =========================
                         // class EncoderTestEmployee
                         // =========================

class EncoderTestEmployee {

    // INSTANCE DATA
    bsl::string         d_name;
    EncoderTestAddress  d_homeAddress;
    int                 d_age;

  public:
    // TYPES
    enum {
        ATTRIBUTE_ID_NAME         = 0
      , ATTRIBUTE_ID_HOME_ADDRESS = 1
      , ATTRIBUTE_ID_AGE          = 2
    };

    enum {
        NUM_ATTRIBUTES = 3
    };

    enum {
        ATTRIBUTE_INDEX_NAME         = 0
      , ATTRIBUTE_INDEX_HOME_ADDRESS = 1
      , ATTRIBUTE_INDEX_AGE          = 2
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
    explicit EncoderTestEmployee(bslma::Allocator *basicAllocator = 0);
        // Create an object of type 'EncoderTestEmployee' having the default
        // value.  Use the optionally specified 'basicAllocator' to supply
        // memory.  If 'basicAllocator' is 0, the currently installed default
        // allocator is used.

    EncoderTestEmployee(const EncoderTestEmployee& original,
                        bslma::Allocator *basicAllocator = 0);
        // Create an object of type 'EncoderTestEmployee' having the value of
        // the specified 'original' object.  Use the optionally specified
        // 'basicAllocator' to supply memory.  If 'basicAllocator' is 0, the
        // currently installed default allocator is used.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    EncoderTestEmployee(EncoderTestEmployee&& original) noexcept;
        // Create an object of type 'EncoderTestEmployee' having the value of
        // the specified 'original' object.  After performing this action, the
        // 'original' object will be left in a valid, but unspecified state.

    EncoderTestEmployee(EncoderTestEmployee&& original,
                        bslma::Allocator *basicAllocator);
        // Create an object of type 'EncoderTestEmployee' having the value of
        // the specified 'original' object.  After performing this action, the
        // 'original' object will be left in a valid, but unspecified state.
        // Use the optionally specified 'basicAllocator' to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.
#endif

    ~EncoderTestEmployee();
        // Destroy this object.

    // MANIPULATORS
    EncoderTestEmployee& operator=(const EncoderTestEmployee& rhs);
        // Assign to this object the value of the specified 'rhs' object.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    EncoderTestEmployee& operator=(EncoderTestEmployee&& rhs);
        // Assign to this object the value of the specified 'rhs' object.
        // After performing this action, the 'rhs' object will be left in a
        // valid, but unspecified state.
#endif

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

    bsl::string& name();
        // Return a reference to the modifiable "Name" attribute of this
        // object.

    EncoderTestAddress& homeAddress();
        // Return a reference to the modifiable "HomeAddress" attribute of this
        // object.

    int& age();
        // Return a reference to the modifiable "Age" attribute of this object.

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

    const bsl::string& name() const;
        // Return a reference to the non-modifiable "Name" attribute of this
        // object.

    const EncoderTestAddress& homeAddress() const;
        // Return a reference to the non-modifiable "HomeAddress" attribute of
        // this object.

    int age() const;
        // Return a reference to the non-modifiable "Age" attribute of this
        // object.
};

// FREE OPERATORS
inline
bool operator==(const EncoderTestEmployee& lhs, const EncoderTestEmployee& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
    // the same value, and 'false' otherwise.  Two attribute objects have the
    // same value if each respective attribute has the same value.

inline
bool operator!=(const EncoderTestEmployee& lhs, const EncoderTestEmployee& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
    // have the same value, and 'false' otherwise.  Two attribute objects do
    // not have the same value if one or more respective attributes differ in
    // values.

inline
bsl::ostream& operator<<(bsl::ostream& stream, const EncoderTestEmployee& rhs);
    // Format the specified 'rhs' to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& hashAlg, const baljsn::EncoderTestEmployee& object);
    // Pass the specified 'object' to the specified 'hashAlg'.  This function
    // integrates with the 'bslh' modular hashing system and effectively
    // provides a 'bsl::hash' specialization for 'EncoderTestEmployee'.

}  // close package namespace

// TRAITS

BDLAT_DECL_SEQUENCE_WITH_ALLOCATOR_BITWISEMOVEABLE_TRAITS(baljsn::EncoderTestEmployee)

namespace baljsn {

                 // ==========================================
                 // class EncoderTestSequenceWithAllCategories
                 // ==========================================

class EncoderTestSequenceWithAllCategories {

    // INSTANCE DATA
    bsl::vector<int>                                        d_array;
    bsl::vector<char>                                       d_charArray;
    EncoderTestSequenceWithAllCategoriesSequence            d_sequence;
    EncoderTestSequenceWithAllCategoriesCustomizedType      d_customizedType;
    EncoderTestSequenceWithAllCategoriesChoice              d_choice;
    int                                                     d_simple;
    bdlb::NullableValue<int>                                d_nullableValue;
    EncoderTestSequenceWithAllCategoriesEnumeration::Value  d_enumeration;

  public:
    // TYPES
    enum {
        ATTRIBUTE_ID_CHAR_ARRAY      = 0
      , ATTRIBUTE_ID_ARRAY           = 1
      , ATTRIBUTE_ID_CHOICE          = 2
      , ATTRIBUTE_ID_CUSTOMIZED_TYPE = 3
      , ATTRIBUTE_ID_ENUMERATION     = 4
      , ATTRIBUTE_ID_NULLABLE_VALUE  = 5
      , ATTRIBUTE_ID_SEQUENCE        = 6
      , ATTRIBUTE_ID_SIMPLE          = 7
    };

    enum {
        NUM_ATTRIBUTES = 8
    };

    enum {
        ATTRIBUTE_INDEX_CHAR_ARRAY      = 0
      , ATTRIBUTE_INDEX_ARRAY           = 1
      , ATTRIBUTE_INDEX_CHOICE          = 2
      , ATTRIBUTE_INDEX_CUSTOMIZED_TYPE = 3
      , ATTRIBUTE_INDEX_ENUMERATION     = 4
      , ATTRIBUTE_INDEX_NULLABLE_VALUE  = 5
      , ATTRIBUTE_INDEX_SEQUENCE        = 6
      , ATTRIBUTE_INDEX_SIMPLE          = 7
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
    explicit EncoderTestSequenceWithAllCategories(bslma::Allocator *basicAllocator = 0);
        // Create an object of type 'EncoderTestSequenceWithAllCategories'
        // having the default value.  Use the optionally specified
        // 'basicAllocator' to supply memory.  If 'basicAllocator' is 0, the
        // currently installed default allocator is used.

    EncoderTestSequenceWithAllCategories(const EncoderTestSequenceWithAllCategories& original,
                                         bslma::Allocator *basicAllocator = 0);
        // Create an object of type 'EncoderTestSequenceWithAllCategories'
        // having the value of the specified 'original' object.  Use the
        // optionally specified 'basicAllocator' to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    EncoderTestSequenceWithAllCategories(EncoderTestSequenceWithAllCategories&& original) noexcept;
        // Create an object of type 'EncoderTestSequenceWithAllCategories'
        // having the value of the specified 'original' object.  After
        // performing this action, the 'original' object will be left in a
        // valid, but unspecified state.

    EncoderTestSequenceWithAllCategories(EncoderTestSequenceWithAllCategories&& original,
                                         bslma::Allocator *basicAllocator);
        // Create an object of type 'EncoderTestSequenceWithAllCategories'
        // having the value of the specified 'original' object.  After
        // performing this action, the 'original' object will be left in a
        // valid, but unspecified state.  Use the optionally specified
        // 'basicAllocator' to supply memory.  If 'basicAllocator' is 0, the
        // currently installed default allocator is used.
#endif

    ~EncoderTestSequenceWithAllCategories();
        // Destroy this object.

    // MANIPULATORS
    EncoderTestSequenceWithAllCategories& operator=(const EncoderTestSequenceWithAllCategories& rhs);
        // Assign to this object the value of the specified 'rhs' object.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    EncoderTestSequenceWithAllCategories& operator=(EncoderTestSequenceWithAllCategories&& rhs);
        // Assign to this object the value of the specified 'rhs' object.
        // After performing this action, the 'rhs' object will be left in a
        // valid, but unspecified state.
#endif

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

    bsl::vector<char>& charArray();
        // Return a reference to the modifiable "CharArray" attribute of this
        // object.

    bsl::vector<int>& array();
        // Return a reference to the modifiable "Array" attribute of this
        // object.

    EncoderTestSequenceWithAllCategoriesChoice& choice();
        // Return a reference to the modifiable "Choice" attribute of this
        // object.

    EncoderTestSequenceWithAllCategoriesCustomizedType& customizedType();
        // Return a reference to the modifiable "CustomizedType" attribute of
        // this object.

    EncoderTestSequenceWithAllCategoriesEnumeration::Value& enumeration();
        // Return a reference to the modifiable "Enumeration" attribute of this
        // object.

    bdlb::NullableValue<int>& nullableValue();
        // Return a reference to the modifiable "NullableValue" attribute of
        // this object.

    EncoderTestSequenceWithAllCategoriesSequence& sequence();
        // Return a reference to the modifiable "Sequence" attribute of this
        // object.

    int& simple();
        // Return a reference to the modifiable "Simple" attribute of this
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

    const bsl::vector<char>& charArray() const;
        // Return a reference to the non-modifiable "CharArray" attribute of
        // this object.

    const bsl::vector<int>& array() const;
        // Return a reference to the non-modifiable "Array" attribute of this
        // object.

    const EncoderTestSequenceWithAllCategoriesChoice& choice() const;
        // Return a reference to the non-modifiable "Choice" attribute of this
        // object.

    const EncoderTestSequenceWithAllCategoriesCustomizedType& customizedType() const;
        // Return a reference to the non-modifiable "CustomizedType" attribute
        // of this object.

    EncoderTestSequenceWithAllCategoriesEnumeration::Value enumeration() const;
        // Return a reference to the non-modifiable "Enumeration" attribute of
        // this object.

    const bdlb::NullableValue<int>& nullableValue() const;
        // Return a reference to the non-modifiable "NullableValue" attribute
        // of this object.

    const EncoderTestSequenceWithAllCategoriesSequence& sequence() const;
        // Return a reference to the non-modifiable "Sequence" attribute of
        // this object.

    int simple() const;
        // Return a reference to the non-modifiable "Simple" attribute of this
        // object.
};

// FREE OPERATORS
inline
bool operator==(const EncoderTestSequenceWithAllCategories& lhs, const EncoderTestSequenceWithAllCategories& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
    // the same value, and 'false' otherwise.  Two attribute objects have the
    // same value if each respective attribute has the same value.

inline
bool operator!=(const EncoderTestSequenceWithAllCategories& lhs, const EncoderTestSequenceWithAllCategories& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
    // have the same value, and 'false' otherwise.  Two attribute objects do
    // not have the same value if one or more respective attributes differ in
    // values.

inline
bsl::ostream& operator<<(bsl::ostream& stream, const EncoderTestSequenceWithAllCategories& rhs);
    // Format the specified 'rhs' to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& hashAlg, const baljsn::EncoderTestSequenceWithAllCategories& object);
    // Pass the specified 'object' to the specified 'hashAlg'.  This function
    // integrates with the 'bslh' modular hashing system and effectively
    // provides a 'bsl::hash' specialization for 'EncoderTestSequenceWithAllCategories'.

}  // close package namespace

// TRAITS

BDLAT_DECL_SEQUENCE_WITH_ALLOCATOR_BITWISEMOVEABLE_TRAITS(baljsn::EncoderTestSequenceWithAllCategories)

namespace baljsn {

                   // ======================================
                   // class EncoderTestSequenceWithUntagged1
                   // ======================================

class EncoderTestSequenceWithUntagged1 {

    // INSTANCE DATA
    EncoderTestSequenceWithUntagged1Sequence  d_sequence;

  public:
    // TYPES
    enum {
        ATTRIBUTE_ID_SEQUENCE = 0
    };

    enum {
        NUM_ATTRIBUTES = 1
    };

    enum {
        ATTRIBUTE_INDEX_SEQUENCE = 0
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
    EncoderTestSequenceWithUntagged1();
        // Create an object of type 'EncoderTestSequenceWithUntagged1' having
        // the default value.

    EncoderTestSequenceWithUntagged1(const EncoderTestSequenceWithUntagged1& original);
        // Create an object of type 'EncoderTestSequenceWithUntagged1' having
        // the value of the specified 'original' object.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    EncoderTestSequenceWithUntagged1(EncoderTestSequenceWithUntagged1&& original) = default;
        // Create an object of type 'EncoderTestSequenceWithUntagged1' having
        // the value of the specified 'original' object.  After performing this
        // action, the 'original' object will be left in a valid, but
        // unspecified state.
#endif

    ~EncoderTestSequenceWithUntagged1();
        // Destroy this object.

    // MANIPULATORS
    EncoderTestSequenceWithUntagged1& operator=(const EncoderTestSequenceWithUntagged1& rhs);
        // Assign to this object the value of the specified 'rhs' object.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    EncoderTestSequenceWithUntagged1& operator=(EncoderTestSequenceWithUntagged1&& rhs);
        // Assign to this object the value of the specified 'rhs' object.
        // After performing this action, the 'rhs' object will be left in a
        // valid, but unspecified state.
#endif

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

    EncoderTestSequenceWithUntagged1Sequence& sequence();
        // Return a reference to the modifiable "Sequence" attribute of this
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

    const EncoderTestSequenceWithUntagged1Sequence& sequence() const;
        // Return a reference to the non-modifiable "Sequence" attribute of
        // this object.
};

// FREE OPERATORS
inline
bool operator==(const EncoderTestSequenceWithUntagged1& lhs, const EncoderTestSequenceWithUntagged1& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
    // the same value, and 'false' otherwise.  Two attribute objects have the
    // same value if each respective attribute has the same value.

inline
bool operator!=(const EncoderTestSequenceWithUntagged1& lhs, const EncoderTestSequenceWithUntagged1& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
    // have the same value, and 'false' otherwise.  Two attribute objects do
    // not have the same value if one or more respective attributes differ in
    // values.

inline
bsl::ostream& operator<<(bsl::ostream& stream, const EncoderTestSequenceWithUntagged1& rhs);
    // Format the specified 'rhs' to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& hashAlg, const baljsn::EncoderTestSequenceWithUntagged1& object);
    // Pass the specified 'object' to the specified 'hashAlg'.  This function
    // integrates with the 'bslh' modular hashing system and effectively
    // provides a 'bsl::hash' specialization for 'EncoderTestSequenceWithUntagged1'.

}  // close package namespace

// TRAITS

BDLAT_DECL_SEQUENCE_WITH_BITWISEMOVEABLE_TRAITS(baljsn::EncoderTestSequenceWithUntagged1)

namespace baljsn {

                  // =======================================
                  // class EncoderTestSequenceWithUntagged10
                  // =======================================

class EncoderTestSequenceWithUntagged10 {

    // INSTANCE DATA
    EncoderTestSequenceWithUntagged10Sequence  d_sequence;
    int                                        d_attribute0;
    int                                        d_attribute1;

  public:
    // TYPES
    enum {
        ATTRIBUTE_ID_SEQUENCE   = 0
      , ATTRIBUTE_ID_ATTRIBUTE0 = 1
      , ATTRIBUTE_ID_ATTRIBUTE1 = 2
    };

    enum {
        NUM_ATTRIBUTES = 3
    };

    enum {
        ATTRIBUTE_INDEX_SEQUENCE   = 0
      , ATTRIBUTE_INDEX_ATTRIBUTE0 = 1
      , ATTRIBUTE_INDEX_ATTRIBUTE1 = 2
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
    EncoderTestSequenceWithUntagged10();
        // Create an object of type 'EncoderTestSequenceWithUntagged10' having
        // the default value.

    EncoderTestSequenceWithUntagged10(const EncoderTestSequenceWithUntagged10& original);
        // Create an object of type 'EncoderTestSequenceWithUntagged10' having
        // the value of the specified 'original' object.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    EncoderTestSequenceWithUntagged10(EncoderTestSequenceWithUntagged10&& original) = default;
        // Create an object of type 'EncoderTestSequenceWithUntagged10' having
        // the value of the specified 'original' object.  After performing this
        // action, the 'original' object will be left in a valid, but
        // unspecified state.
#endif

    ~EncoderTestSequenceWithUntagged10();
        // Destroy this object.

    // MANIPULATORS
    EncoderTestSequenceWithUntagged10& operator=(const EncoderTestSequenceWithUntagged10& rhs);
        // Assign to this object the value of the specified 'rhs' object.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    EncoderTestSequenceWithUntagged10& operator=(EncoderTestSequenceWithUntagged10&& rhs);
        // Assign to this object the value of the specified 'rhs' object.
        // After performing this action, the 'rhs' object will be left in a
        // valid, but unspecified state.
#endif

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

    EncoderTestSequenceWithUntagged10Sequence& sequence();
        // Return a reference to the modifiable "Sequence" attribute of this
        // object.

    int& attribute0();
        // Return a reference to the modifiable "Attribute0" attribute of this
        // object.

    int& attribute1();
        // Return a reference to the modifiable "Attribute1" attribute of this
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

    const EncoderTestSequenceWithUntagged10Sequence& sequence() const;
        // Return a reference to the non-modifiable "Sequence" attribute of
        // this object.

    int attribute0() const;
        // Return a reference to the non-modifiable "Attribute0" attribute of
        // this object.

    int attribute1() const;
        // Return a reference to the non-modifiable "Attribute1" attribute of
        // this object.
};

// FREE OPERATORS
inline
bool operator==(const EncoderTestSequenceWithUntagged10& lhs, const EncoderTestSequenceWithUntagged10& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
    // the same value, and 'false' otherwise.  Two attribute objects have the
    // same value if each respective attribute has the same value.

inline
bool operator!=(const EncoderTestSequenceWithUntagged10& lhs, const EncoderTestSequenceWithUntagged10& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
    // have the same value, and 'false' otherwise.  Two attribute objects do
    // not have the same value if one or more respective attributes differ in
    // values.

inline
bsl::ostream& operator<<(bsl::ostream& stream, const EncoderTestSequenceWithUntagged10& rhs);
    // Format the specified 'rhs' to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& hashAlg, const baljsn::EncoderTestSequenceWithUntagged10& object);
    // Pass the specified 'object' to the specified 'hashAlg'.  This function
    // integrates with the 'bslh' modular hashing system and effectively
    // provides a 'bsl::hash' specialization for 'EncoderTestSequenceWithUntagged10'.

}  // close package namespace

// TRAITS

BDLAT_DECL_SEQUENCE_WITH_BITWISEMOVEABLE_TRAITS(baljsn::EncoderTestSequenceWithUntagged10)

namespace baljsn {

                  // =======================================
                  // class EncoderTestSequenceWithUntagged11
                  // =======================================

class EncoderTestSequenceWithUntagged11 {

    // INSTANCE DATA
    EncoderTestSequenceWithUntagged11Sequence1  d_sequence1;
    EncoderTestSequenceWithUntagged11Sequence   d_sequence;
    int                                         d_attribute0;

  public:
    // TYPES
    enum {
        ATTRIBUTE_ID_ATTRIBUTE0 = 0
      , ATTRIBUTE_ID_SEQUENCE   = 1
      , ATTRIBUTE_ID_SEQUENCE1  = 2
    };

    enum {
        NUM_ATTRIBUTES = 3
    };

    enum {
        ATTRIBUTE_INDEX_ATTRIBUTE0 = 0
      , ATTRIBUTE_INDEX_SEQUENCE   = 1
      , ATTRIBUTE_INDEX_SEQUENCE1  = 2
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
    EncoderTestSequenceWithUntagged11();
        // Create an object of type 'EncoderTestSequenceWithUntagged11' having
        // the default value.

    EncoderTestSequenceWithUntagged11(const EncoderTestSequenceWithUntagged11& original);
        // Create an object of type 'EncoderTestSequenceWithUntagged11' having
        // the value of the specified 'original' object.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    EncoderTestSequenceWithUntagged11(EncoderTestSequenceWithUntagged11&& original) = default;
        // Create an object of type 'EncoderTestSequenceWithUntagged11' having
        // the value of the specified 'original' object.  After performing this
        // action, the 'original' object will be left in a valid, but
        // unspecified state.
#endif

    ~EncoderTestSequenceWithUntagged11();
        // Destroy this object.

    // MANIPULATORS
    EncoderTestSequenceWithUntagged11& operator=(const EncoderTestSequenceWithUntagged11& rhs);
        // Assign to this object the value of the specified 'rhs' object.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    EncoderTestSequenceWithUntagged11& operator=(EncoderTestSequenceWithUntagged11&& rhs);
        // Assign to this object the value of the specified 'rhs' object.
        // After performing this action, the 'rhs' object will be left in a
        // valid, but unspecified state.
#endif

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

    int& attribute0();
        // Return a reference to the modifiable "Attribute0" attribute of this
        // object.

    EncoderTestSequenceWithUntagged11Sequence& sequence();
        // Return a reference to the modifiable "Sequence" attribute of this
        // object.

    EncoderTestSequenceWithUntagged11Sequence1& sequence1();
        // Return a reference to the modifiable "Sequence1" attribute of this
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

    int attribute0() const;
        // Return a reference to the non-modifiable "Attribute0" attribute of
        // this object.

    const EncoderTestSequenceWithUntagged11Sequence& sequence() const;
        // Return a reference to the non-modifiable "Sequence" attribute of
        // this object.

    const EncoderTestSequenceWithUntagged11Sequence1& sequence1() const;
        // Return a reference to the non-modifiable "Sequence1" attribute of
        // this object.
};

// FREE OPERATORS
inline
bool operator==(const EncoderTestSequenceWithUntagged11& lhs, const EncoderTestSequenceWithUntagged11& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
    // the same value, and 'false' otherwise.  Two attribute objects have the
    // same value if each respective attribute has the same value.

inline
bool operator!=(const EncoderTestSequenceWithUntagged11& lhs, const EncoderTestSequenceWithUntagged11& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
    // have the same value, and 'false' otherwise.  Two attribute objects do
    // not have the same value if one or more respective attributes differ in
    // values.

inline
bsl::ostream& operator<<(bsl::ostream& stream, const EncoderTestSequenceWithUntagged11& rhs);
    // Format the specified 'rhs' to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& hashAlg, const baljsn::EncoderTestSequenceWithUntagged11& object);
    // Pass the specified 'object' to the specified 'hashAlg'.  This function
    // integrates with the 'bslh' modular hashing system and effectively
    // provides a 'bsl::hash' specialization for 'EncoderTestSequenceWithUntagged11'.

}  // close package namespace

// TRAITS

BDLAT_DECL_SEQUENCE_WITH_BITWISEMOVEABLE_TRAITS(baljsn::EncoderTestSequenceWithUntagged11)

namespace baljsn {

                  // =======================================
                  // class EncoderTestSequenceWithUntagged12
                  // =======================================

class EncoderTestSequenceWithUntagged12 {

    // INSTANCE DATA
    EncoderTestSequenceWithUntagged12Sequence  d_sequence;
    int                                        d_attribute0;
    int                                        d_attribute1;

  public:
    // TYPES
    enum {
        ATTRIBUTE_ID_ATTRIBUTE0 = 0
      , ATTRIBUTE_ID_SEQUENCE   = 1
      , ATTRIBUTE_ID_ATTRIBUTE1 = 2
    };

    enum {
        NUM_ATTRIBUTES = 3
    };

    enum {
        ATTRIBUTE_INDEX_ATTRIBUTE0 = 0
      , ATTRIBUTE_INDEX_SEQUENCE   = 1
      , ATTRIBUTE_INDEX_ATTRIBUTE1 = 2
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
    EncoderTestSequenceWithUntagged12();
        // Create an object of type 'EncoderTestSequenceWithUntagged12' having
        // the default value.

    EncoderTestSequenceWithUntagged12(const EncoderTestSequenceWithUntagged12& original);
        // Create an object of type 'EncoderTestSequenceWithUntagged12' having
        // the value of the specified 'original' object.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    EncoderTestSequenceWithUntagged12(EncoderTestSequenceWithUntagged12&& original) = default;
        // Create an object of type 'EncoderTestSequenceWithUntagged12' having
        // the value of the specified 'original' object.  After performing this
        // action, the 'original' object will be left in a valid, but
        // unspecified state.
#endif

    ~EncoderTestSequenceWithUntagged12();
        // Destroy this object.

    // MANIPULATORS
    EncoderTestSequenceWithUntagged12& operator=(const EncoderTestSequenceWithUntagged12& rhs);
        // Assign to this object the value of the specified 'rhs' object.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    EncoderTestSequenceWithUntagged12& operator=(EncoderTestSequenceWithUntagged12&& rhs);
        // Assign to this object the value of the specified 'rhs' object.
        // After performing this action, the 'rhs' object will be left in a
        // valid, but unspecified state.
#endif

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

    int& attribute0();
        // Return a reference to the modifiable "Attribute0" attribute of this
        // object.

    EncoderTestSequenceWithUntagged12Sequence& sequence();
        // Return a reference to the modifiable "Sequence" attribute of this
        // object.

    int& attribute1();
        // Return a reference to the modifiable "Attribute1" attribute of this
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

    int attribute0() const;
        // Return a reference to the non-modifiable "Attribute0" attribute of
        // this object.

    const EncoderTestSequenceWithUntagged12Sequence& sequence() const;
        // Return a reference to the non-modifiable "Sequence" attribute of
        // this object.

    int attribute1() const;
        // Return a reference to the non-modifiable "Attribute1" attribute of
        // this object.
};

// FREE OPERATORS
inline
bool operator==(const EncoderTestSequenceWithUntagged12& lhs, const EncoderTestSequenceWithUntagged12& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
    // the same value, and 'false' otherwise.  Two attribute objects have the
    // same value if each respective attribute has the same value.

inline
bool operator!=(const EncoderTestSequenceWithUntagged12& lhs, const EncoderTestSequenceWithUntagged12& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
    // have the same value, and 'false' otherwise.  Two attribute objects do
    // not have the same value if one or more respective attributes differ in
    // values.

inline
bsl::ostream& operator<<(bsl::ostream& stream, const EncoderTestSequenceWithUntagged12& rhs);
    // Format the specified 'rhs' to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& hashAlg, const baljsn::EncoderTestSequenceWithUntagged12& object);
    // Pass the specified 'object' to the specified 'hashAlg'.  This function
    // integrates with the 'bslh' modular hashing system and effectively
    // provides a 'bsl::hash' specialization for 'EncoderTestSequenceWithUntagged12'.

}  // close package namespace

// TRAITS

BDLAT_DECL_SEQUENCE_WITH_BITWISEMOVEABLE_TRAITS(baljsn::EncoderTestSequenceWithUntagged12)

namespace baljsn {

                  // =======================================
                  // class EncoderTestSequenceWithUntagged13
                  // =======================================

class EncoderTestSequenceWithUntagged13 {

    // INSTANCE DATA
    EncoderTestSequenceWithUntagged13Sequence  d_sequence;
    int                                        d_attribute0;
    int                                        d_attribute1;

  public:
    // TYPES
    enum {
        ATTRIBUTE_ID_ATTRIBUTE0 = 0
      , ATTRIBUTE_ID_ATTRIBUTE1 = 1
      , ATTRIBUTE_ID_SEQUENCE   = 2
    };

    enum {
        NUM_ATTRIBUTES = 3
    };

    enum {
        ATTRIBUTE_INDEX_ATTRIBUTE0 = 0
      , ATTRIBUTE_INDEX_ATTRIBUTE1 = 1
      , ATTRIBUTE_INDEX_SEQUENCE   = 2
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
    EncoderTestSequenceWithUntagged13();
        // Create an object of type 'EncoderTestSequenceWithUntagged13' having
        // the default value.

    EncoderTestSequenceWithUntagged13(const EncoderTestSequenceWithUntagged13& original);
        // Create an object of type 'EncoderTestSequenceWithUntagged13' having
        // the value of the specified 'original' object.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    EncoderTestSequenceWithUntagged13(EncoderTestSequenceWithUntagged13&& original) = default;
        // Create an object of type 'EncoderTestSequenceWithUntagged13' having
        // the value of the specified 'original' object.  After performing this
        // action, the 'original' object will be left in a valid, but
        // unspecified state.
#endif

    ~EncoderTestSequenceWithUntagged13();
        // Destroy this object.

    // MANIPULATORS
    EncoderTestSequenceWithUntagged13& operator=(const EncoderTestSequenceWithUntagged13& rhs);
        // Assign to this object the value of the specified 'rhs' object.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    EncoderTestSequenceWithUntagged13& operator=(EncoderTestSequenceWithUntagged13&& rhs);
        // Assign to this object the value of the specified 'rhs' object.
        // After performing this action, the 'rhs' object will be left in a
        // valid, but unspecified state.
#endif

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

    int& attribute0();
        // Return a reference to the modifiable "Attribute0" attribute of this
        // object.

    int& attribute1();
        // Return a reference to the modifiable "Attribute1" attribute of this
        // object.

    EncoderTestSequenceWithUntagged13Sequence& sequence();
        // Return a reference to the modifiable "Sequence" attribute of this
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

    int attribute0() const;
        // Return a reference to the non-modifiable "Attribute0" attribute of
        // this object.

    int attribute1() const;
        // Return a reference to the non-modifiable "Attribute1" attribute of
        // this object.

    const EncoderTestSequenceWithUntagged13Sequence& sequence() const;
        // Return a reference to the non-modifiable "Sequence" attribute of
        // this object.
};

// FREE OPERATORS
inline
bool operator==(const EncoderTestSequenceWithUntagged13& lhs, const EncoderTestSequenceWithUntagged13& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
    // the same value, and 'false' otherwise.  Two attribute objects have the
    // same value if each respective attribute has the same value.

inline
bool operator!=(const EncoderTestSequenceWithUntagged13& lhs, const EncoderTestSequenceWithUntagged13& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
    // have the same value, and 'false' otherwise.  Two attribute objects do
    // not have the same value if one or more respective attributes differ in
    // values.

inline
bsl::ostream& operator<<(bsl::ostream& stream, const EncoderTestSequenceWithUntagged13& rhs);
    // Format the specified 'rhs' to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& hashAlg, const baljsn::EncoderTestSequenceWithUntagged13& object);
    // Pass the specified 'object' to the specified 'hashAlg'.  This function
    // integrates with the 'bslh' modular hashing system and effectively
    // provides a 'bsl::hash' specialization for 'EncoderTestSequenceWithUntagged13'.

}  // close package namespace

// TRAITS

BDLAT_DECL_SEQUENCE_WITH_BITWISEMOVEABLE_TRAITS(baljsn::EncoderTestSequenceWithUntagged13)

namespace baljsn {

                   // ======================================
                   // class EncoderTestSequenceWithUntagged3
                   // ======================================

class EncoderTestSequenceWithUntagged3 {

    // INSTANCE DATA
    EncoderTestSequenceWithUntagged3Sequence1  d_sequence1;
    EncoderTestSequenceWithUntagged3Sequence   d_sequence;

  public:
    // TYPES
    enum {
        ATTRIBUTE_ID_SEQUENCE  = 0
      , ATTRIBUTE_ID_SEQUENCE1 = 1
    };

    enum {
        NUM_ATTRIBUTES = 2
    };

    enum {
        ATTRIBUTE_INDEX_SEQUENCE  = 0
      , ATTRIBUTE_INDEX_SEQUENCE1 = 1
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
    EncoderTestSequenceWithUntagged3();
        // Create an object of type 'EncoderTestSequenceWithUntagged3' having
        // the default value.

    EncoderTestSequenceWithUntagged3(const EncoderTestSequenceWithUntagged3& original);
        // Create an object of type 'EncoderTestSequenceWithUntagged3' having
        // the value of the specified 'original' object.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    EncoderTestSequenceWithUntagged3(EncoderTestSequenceWithUntagged3&& original) = default;
        // Create an object of type 'EncoderTestSequenceWithUntagged3' having
        // the value of the specified 'original' object.  After performing this
        // action, the 'original' object will be left in a valid, but
        // unspecified state.
#endif

    ~EncoderTestSequenceWithUntagged3();
        // Destroy this object.

    // MANIPULATORS
    EncoderTestSequenceWithUntagged3& operator=(const EncoderTestSequenceWithUntagged3& rhs);
        // Assign to this object the value of the specified 'rhs' object.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    EncoderTestSequenceWithUntagged3& operator=(EncoderTestSequenceWithUntagged3&& rhs);
        // Assign to this object the value of the specified 'rhs' object.
        // After performing this action, the 'rhs' object will be left in a
        // valid, but unspecified state.
#endif

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

    EncoderTestSequenceWithUntagged3Sequence& sequence();
        // Return a reference to the modifiable "Sequence" attribute of this
        // object.

    EncoderTestSequenceWithUntagged3Sequence1& sequence1();
        // Return a reference to the modifiable "Sequence1" attribute of this
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

    const EncoderTestSequenceWithUntagged3Sequence& sequence() const;
        // Return a reference to the non-modifiable "Sequence" attribute of
        // this object.

    const EncoderTestSequenceWithUntagged3Sequence1& sequence1() const;
        // Return a reference to the non-modifiable "Sequence1" attribute of
        // this object.
};

// FREE OPERATORS
inline
bool operator==(const EncoderTestSequenceWithUntagged3& lhs, const EncoderTestSequenceWithUntagged3& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
    // the same value, and 'false' otherwise.  Two attribute objects have the
    // same value if each respective attribute has the same value.

inline
bool operator!=(const EncoderTestSequenceWithUntagged3& lhs, const EncoderTestSequenceWithUntagged3& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
    // have the same value, and 'false' otherwise.  Two attribute objects do
    // not have the same value if one or more respective attributes differ in
    // values.

inline
bsl::ostream& operator<<(bsl::ostream& stream, const EncoderTestSequenceWithUntagged3& rhs);
    // Format the specified 'rhs' to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& hashAlg, const baljsn::EncoderTestSequenceWithUntagged3& object);
    // Pass the specified 'object' to the specified 'hashAlg'.  This function
    // integrates with the 'bslh' modular hashing system and effectively
    // provides a 'bsl::hash' specialization for 'EncoderTestSequenceWithUntagged3'.

}  // close package namespace

// TRAITS

BDLAT_DECL_SEQUENCE_WITH_BITWISEMOVEABLE_TRAITS(baljsn::EncoderTestSequenceWithUntagged3)

namespace baljsn {

                   // ======================================
                   // class EncoderTestSequenceWithUntagged4
                   // ======================================

class EncoderTestSequenceWithUntagged4 {

    // INSTANCE DATA
    EncoderTestSequenceWithUntagged4Sequence  d_sequence;
    int                                       d_attribute0;

  public:
    // TYPES
    enum {
        ATTRIBUTE_ID_SEQUENCE   = 0
      , ATTRIBUTE_ID_ATTRIBUTE0 = 1
    };

    enum {
        NUM_ATTRIBUTES = 2
    };

    enum {
        ATTRIBUTE_INDEX_SEQUENCE   = 0
      , ATTRIBUTE_INDEX_ATTRIBUTE0 = 1
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
    EncoderTestSequenceWithUntagged4();
        // Create an object of type 'EncoderTestSequenceWithUntagged4' having
        // the default value.

    EncoderTestSequenceWithUntagged4(const EncoderTestSequenceWithUntagged4& original);
        // Create an object of type 'EncoderTestSequenceWithUntagged4' having
        // the value of the specified 'original' object.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    EncoderTestSequenceWithUntagged4(EncoderTestSequenceWithUntagged4&& original) = default;
        // Create an object of type 'EncoderTestSequenceWithUntagged4' having
        // the value of the specified 'original' object.  After performing this
        // action, the 'original' object will be left in a valid, but
        // unspecified state.
#endif

    ~EncoderTestSequenceWithUntagged4();
        // Destroy this object.

    // MANIPULATORS
    EncoderTestSequenceWithUntagged4& operator=(const EncoderTestSequenceWithUntagged4& rhs);
        // Assign to this object the value of the specified 'rhs' object.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    EncoderTestSequenceWithUntagged4& operator=(EncoderTestSequenceWithUntagged4&& rhs);
        // Assign to this object the value of the specified 'rhs' object.
        // After performing this action, the 'rhs' object will be left in a
        // valid, but unspecified state.
#endif

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

    EncoderTestSequenceWithUntagged4Sequence& sequence();
        // Return a reference to the modifiable "Sequence" attribute of this
        // object.

    int& attribute0();
        // Return a reference to the modifiable "Attribute0" attribute of this
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

    const EncoderTestSequenceWithUntagged4Sequence& sequence() const;
        // Return a reference to the non-modifiable "Sequence" attribute of
        // this object.

    int attribute0() const;
        // Return a reference to the non-modifiable "Attribute0" attribute of
        // this object.
};

// FREE OPERATORS
inline
bool operator==(const EncoderTestSequenceWithUntagged4& lhs, const EncoderTestSequenceWithUntagged4& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
    // the same value, and 'false' otherwise.  Two attribute objects have the
    // same value if each respective attribute has the same value.

inline
bool operator!=(const EncoderTestSequenceWithUntagged4& lhs, const EncoderTestSequenceWithUntagged4& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
    // have the same value, and 'false' otherwise.  Two attribute objects do
    // not have the same value if one or more respective attributes differ in
    // values.

inline
bsl::ostream& operator<<(bsl::ostream& stream, const EncoderTestSequenceWithUntagged4& rhs);
    // Format the specified 'rhs' to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& hashAlg, const baljsn::EncoderTestSequenceWithUntagged4& object);
    // Pass the specified 'object' to the specified 'hashAlg'.  This function
    // integrates with the 'bslh' modular hashing system and effectively
    // provides a 'bsl::hash' specialization for 'EncoderTestSequenceWithUntagged4'.

}  // close package namespace

// TRAITS

BDLAT_DECL_SEQUENCE_WITH_BITWISEMOVEABLE_TRAITS(baljsn::EncoderTestSequenceWithUntagged4)

namespace baljsn {

                   // ======================================
                   // class EncoderTestSequenceWithUntagged5
                   // ======================================

class EncoderTestSequenceWithUntagged5 {

    // INSTANCE DATA
    EncoderTestSequenceWithUntagged5Sequence  d_sequence;
    int                                       d_attribute0;

  public:
    // TYPES
    enum {
        ATTRIBUTE_ID_ATTRIBUTE0 = 0
      , ATTRIBUTE_ID_SEQUENCE   = 1
    };

    enum {
        NUM_ATTRIBUTES = 2
    };

    enum {
        ATTRIBUTE_INDEX_ATTRIBUTE0 = 0
      , ATTRIBUTE_INDEX_SEQUENCE   = 1
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
    EncoderTestSequenceWithUntagged5();
        // Create an object of type 'EncoderTestSequenceWithUntagged5' having
        // the default value.

    EncoderTestSequenceWithUntagged5(const EncoderTestSequenceWithUntagged5& original);
        // Create an object of type 'EncoderTestSequenceWithUntagged5' having
        // the value of the specified 'original' object.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    EncoderTestSequenceWithUntagged5(EncoderTestSequenceWithUntagged5&& original) = default;
        // Create an object of type 'EncoderTestSequenceWithUntagged5' having
        // the value of the specified 'original' object.  After performing this
        // action, the 'original' object will be left in a valid, but
        // unspecified state.
#endif

    ~EncoderTestSequenceWithUntagged5();
        // Destroy this object.

    // MANIPULATORS
    EncoderTestSequenceWithUntagged5& operator=(const EncoderTestSequenceWithUntagged5& rhs);
        // Assign to this object the value of the specified 'rhs' object.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    EncoderTestSequenceWithUntagged5& operator=(EncoderTestSequenceWithUntagged5&& rhs);
        // Assign to this object the value of the specified 'rhs' object.
        // After performing this action, the 'rhs' object will be left in a
        // valid, but unspecified state.
#endif

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

    int& attribute0();
        // Return a reference to the modifiable "Attribute0" attribute of this
        // object.

    EncoderTestSequenceWithUntagged5Sequence& sequence();
        // Return a reference to the modifiable "Sequence" attribute of this
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

    int attribute0() const;
        // Return a reference to the non-modifiable "Attribute0" attribute of
        // this object.

    const EncoderTestSequenceWithUntagged5Sequence& sequence() const;
        // Return a reference to the non-modifiable "Sequence" attribute of
        // this object.
};

// FREE OPERATORS
inline
bool operator==(const EncoderTestSequenceWithUntagged5& lhs, const EncoderTestSequenceWithUntagged5& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
    // the same value, and 'false' otherwise.  Two attribute objects have the
    // same value if each respective attribute has the same value.

inline
bool operator!=(const EncoderTestSequenceWithUntagged5& lhs, const EncoderTestSequenceWithUntagged5& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
    // have the same value, and 'false' otherwise.  Two attribute objects do
    // not have the same value if one or more respective attributes differ in
    // values.

inline
bsl::ostream& operator<<(bsl::ostream& stream, const EncoderTestSequenceWithUntagged5& rhs);
    // Format the specified 'rhs' to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& hashAlg, const baljsn::EncoderTestSequenceWithUntagged5& object);
    // Pass the specified 'object' to the specified 'hashAlg'.  This function
    // integrates with the 'bslh' modular hashing system and effectively
    // provides a 'bsl::hash' specialization for 'EncoderTestSequenceWithUntagged5'.

}  // close package namespace

// TRAITS

BDLAT_DECL_SEQUENCE_WITH_BITWISEMOVEABLE_TRAITS(baljsn::EncoderTestSequenceWithUntagged5)

namespace baljsn {

                   // ======================================
                   // class EncoderTestSequenceWithUntagged7
                   // ======================================

class EncoderTestSequenceWithUntagged7 {

    // INSTANCE DATA
    EncoderTestSequenceWithUntagged7Sequence2  d_sequence2;
    EncoderTestSequenceWithUntagged7Sequence1  d_sequence1;
    EncoderTestSequenceWithUntagged7Sequence   d_sequence;

  public:
    // TYPES
    enum {
        ATTRIBUTE_ID_SEQUENCE  = 0
      , ATTRIBUTE_ID_SEQUENCE1 = 1
      , ATTRIBUTE_ID_SEQUENCE2 = 2
    };

    enum {
        NUM_ATTRIBUTES = 3
    };

    enum {
        ATTRIBUTE_INDEX_SEQUENCE  = 0
      , ATTRIBUTE_INDEX_SEQUENCE1 = 1
      , ATTRIBUTE_INDEX_SEQUENCE2 = 2
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
    EncoderTestSequenceWithUntagged7();
        // Create an object of type 'EncoderTestSequenceWithUntagged7' having
        // the default value.

    EncoderTestSequenceWithUntagged7(const EncoderTestSequenceWithUntagged7& original);
        // Create an object of type 'EncoderTestSequenceWithUntagged7' having
        // the value of the specified 'original' object.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    EncoderTestSequenceWithUntagged7(EncoderTestSequenceWithUntagged7&& original) = default;
        // Create an object of type 'EncoderTestSequenceWithUntagged7' having
        // the value of the specified 'original' object.  After performing this
        // action, the 'original' object will be left in a valid, but
        // unspecified state.
#endif

    ~EncoderTestSequenceWithUntagged7();
        // Destroy this object.

    // MANIPULATORS
    EncoderTestSequenceWithUntagged7& operator=(const EncoderTestSequenceWithUntagged7& rhs);
        // Assign to this object the value of the specified 'rhs' object.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    EncoderTestSequenceWithUntagged7& operator=(EncoderTestSequenceWithUntagged7&& rhs);
        // Assign to this object the value of the specified 'rhs' object.
        // After performing this action, the 'rhs' object will be left in a
        // valid, but unspecified state.
#endif

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

    EncoderTestSequenceWithUntagged7Sequence& sequence();
        // Return a reference to the modifiable "Sequence" attribute of this
        // object.

    EncoderTestSequenceWithUntagged7Sequence1& sequence1();
        // Return a reference to the modifiable "Sequence1" attribute of this
        // object.

    EncoderTestSequenceWithUntagged7Sequence2& sequence2();
        // Return a reference to the modifiable "Sequence2" attribute of this
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

    const EncoderTestSequenceWithUntagged7Sequence& sequence() const;
        // Return a reference to the non-modifiable "Sequence" attribute of
        // this object.

    const EncoderTestSequenceWithUntagged7Sequence1& sequence1() const;
        // Return a reference to the non-modifiable "Sequence1" attribute of
        // this object.

    const EncoderTestSequenceWithUntagged7Sequence2& sequence2() const;
        // Return a reference to the non-modifiable "Sequence2" attribute of
        // this object.
};

// FREE OPERATORS
inline
bool operator==(const EncoderTestSequenceWithUntagged7& lhs, const EncoderTestSequenceWithUntagged7& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
    // the same value, and 'false' otherwise.  Two attribute objects have the
    // same value if each respective attribute has the same value.

inline
bool operator!=(const EncoderTestSequenceWithUntagged7& lhs, const EncoderTestSequenceWithUntagged7& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
    // have the same value, and 'false' otherwise.  Two attribute objects do
    // not have the same value if one or more respective attributes differ in
    // values.

inline
bsl::ostream& operator<<(bsl::ostream& stream, const EncoderTestSequenceWithUntagged7& rhs);
    // Format the specified 'rhs' to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& hashAlg, const baljsn::EncoderTestSequenceWithUntagged7& object);
    // Pass the specified 'object' to the specified 'hashAlg'.  This function
    // integrates with the 'bslh' modular hashing system and effectively
    // provides a 'bsl::hash' specialization for 'EncoderTestSequenceWithUntagged7'.

}  // close package namespace

// TRAITS

BDLAT_DECL_SEQUENCE_WITH_BITWISEMOVEABLE_TRAITS(baljsn::EncoderTestSequenceWithUntagged7)

namespace baljsn {

                   // ======================================
                   // class EncoderTestSequenceWithUntagged8
                   // ======================================

class EncoderTestSequenceWithUntagged8 {

    // INSTANCE DATA
    EncoderTestSequenceWithUntagged8Sequence1  d_sequence1;
    EncoderTestSequenceWithUntagged8Sequence   d_sequence;
    int                                        d_attribute0;

  public:
    // TYPES
    enum {
        ATTRIBUTE_ID_SEQUENCE   = 0
      , ATTRIBUTE_ID_SEQUENCE1  = 1
      , ATTRIBUTE_ID_ATTRIBUTE0 = 2
    };

    enum {
        NUM_ATTRIBUTES = 3
    };

    enum {
        ATTRIBUTE_INDEX_SEQUENCE   = 0
      , ATTRIBUTE_INDEX_SEQUENCE1  = 1
      , ATTRIBUTE_INDEX_ATTRIBUTE0 = 2
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
    EncoderTestSequenceWithUntagged8();
        // Create an object of type 'EncoderTestSequenceWithUntagged8' having
        // the default value.

    EncoderTestSequenceWithUntagged8(const EncoderTestSequenceWithUntagged8& original);
        // Create an object of type 'EncoderTestSequenceWithUntagged8' having
        // the value of the specified 'original' object.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    EncoderTestSequenceWithUntagged8(EncoderTestSequenceWithUntagged8&& original) = default;
        // Create an object of type 'EncoderTestSequenceWithUntagged8' having
        // the value of the specified 'original' object.  After performing this
        // action, the 'original' object will be left in a valid, but
        // unspecified state.
#endif

    ~EncoderTestSequenceWithUntagged8();
        // Destroy this object.

    // MANIPULATORS
    EncoderTestSequenceWithUntagged8& operator=(const EncoderTestSequenceWithUntagged8& rhs);
        // Assign to this object the value of the specified 'rhs' object.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    EncoderTestSequenceWithUntagged8& operator=(EncoderTestSequenceWithUntagged8&& rhs);
        // Assign to this object the value of the specified 'rhs' object.
        // After performing this action, the 'rhs' object will be left in a
        // valid, but unspecified state.
#endif

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

    EncoderTestSequenceWithUntagged8Sequence& sequence();
        // Return a reference to the modifiable "Sequence" attribute of this
        // object.

    EncoderTestSequenceWithUntagged8Sequence1& sequence1();
        // Return a reference to the modifiable "Sequence1" attribute of this
        // object.

    int& attribute0();
        // Return a reference to the modifiable "Attribute0" attribute of this
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

    const EncoderTestSequenceWithUntagged8Sequence& sequence() const;
        // Return a reference to the non-modifiable "Sequence" attribute of
        // this object.

    const EncoderTestSequenceWithUntagged8Sequence1& sequence1() const;
        // Return a reference to the non-modifiable "Sequence1" attribute of
        // this object.

    int attribute0() const;
        // Return a reference to the non-modifiable "Attribute0" attribute of
        // this object.
};

// FREE OPERATORS
inline
bool operator==(const EncoderTestSequenceWithUntagged8& lhs, const EncoderTestSequenceWithUntagged8& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
    // the same value, and 'false' otherwise.  Two attribute objects have the
    // same value if each respective attribute has the same value.

inline
bool operator!=(const EncoderTestSequenceWithUntagged8& lhs, const EncoderTestSequenceWithUntagged8& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
    // have the same value, and 'false' otherwise.  Two attribute objects do
    // not have the same value if one or more respective attributes differ in
    // values.

inline
bsl::ostream& operator<<(bsl::ostream& stream, const EncoderTestSequenceWithUntagged8& rhs);
    // Format the specified 'rhs' to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& hashAlg, const baljsn::EncoderTestSequenceWithUntagged8& object);
    // Pass the specified 'object' to the specified 'hashAlg'.  This function
    // integrates with the 'bslh' modular hashing system and effectively
    // provides a 'bsl::hash' specialization for 'EncoderTestSequenceWithUntagged8'.

}  // close package namespace

// TRAITS

BDLAT_DECL_SEQUENCE_WITH_BITWISEMOVEABLE_TRAITS(baljsn::EncoderTestSequenceWithUntagged8)

namespace baljsn {

                   // ======================================
                   // class EncoderTestSequenceWithUntagged9
                   // ======================================

class EncoderTestSequenceWithUntagged9 {

    // INSTANCE DATA
    EncoderTestSequenceWithUntagged9Sequence1  d_sequence1;
    EncoderTestSequenceWithUntagged9Sequence   d_sequence;
    int                                        d_attribute0;

  public:
    // TYPES
    enum {
        ATTRIBUTE_ID_SEQUENCE   = 0
      , ATTRIBUTE_ID_ATTRIBUTE0 = 1
      , ATTRIBUTE_ID_SEQUENCE1  = 2
    };

    enum {
        NUM_ATTRIBUTES = 3
    };

    enum {
        ATTRIBUTE_INDEX_SEQUENCE   = 0
      , ATTRIBUTE_INDEX_ATTRIBUTE0 = 1
      , ATTRIBUTE_INDEX_SEQUENCE1  = 2
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
    EncoderTestSequenceWithUntagged9();
        // Create an object of type 'EncoderTestSequenceWithUntagged9' having
        // the default value.

    EncoderTestSequenceWithUntagged9(const EncoderTestSequenceWithUntagged9& original);
        // Create an object of type 'EncoderTestSequenceWithUntagged9' having
        // the value of the specified 'original' object.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    EncoderTestSequenceWithUntagged9(EncoderTestSequenceWithUntagged9&& original) = default;
        // Create an object of type 'EncoderTestSequenceWithUntagged9' having
        // the value of the specified 'original' object.  After performing this
        // action, the 'original' object will be left in a valid, but
        // unspecified state.
#endif

    ~EncoderTestSequenceWithUntagged9();
        // Destroy this object.

    // MANIPULATORS
    EncoderTestSequenceWithUntagged9& operator=(const EncoderTestSequenceWithUntagged9& rhs);
        // Assign to this object the value of the specified 'rhs' object.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    EncoderTestSequenceWithUntagged9& operator=(EncoderTestSequenceWithUntagged9&& rhs);
        // Assign to this object the value of the specified 'rhs' object.
        // After performing this action, the 'rhs' object will be left in a
        // valid, but unspecified state.
#endif

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

    EncoderTestSequenceWithUntagged9Sequence& sequence();
        // Return a reference to the modifiable "Sequence" attribute of this
        // object.

    int& attribute0();
        // Return a reference to the modifiable "Attribute0" attribute of this
        // object.

    EncoderTestSequenceWithUntagged9Sequence1& sequence1();
        // Return a reference to the modifiable "Sequence1" attribute of this
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

    const EncoderTestSequenceWithUntagged9Sequence& sequence() const;
        // Return a reference to the non-modifiable "Sequence" attribute of
        // this object.

    int attribute0() const;
        // Return a reference to the non-modifiable "Attribute0" attribute of
        // this object.

    const EncoderTestSequenceWithUntagged9Sequence1& sequence1() const;
        // Return a reference to the non-modifiable "Sequence1" attribute of
        // this object.
};

// FREE OPERATORS
inline
bool operator==(const EncoderTestSequenceWithUntagged9& lhs, const EncoderTestSequenceWithUntagged9& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
    // the same value, and 'false' otherwise.  Two attribute objects have the
    // same value if each respective attribute has the same value.

inline
bool operator!=(const EncoderTestSequenceWithUntagged9& lhs, const EncoderTestSequenceWithUntagged9& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
    // have the same value, and 'false' otherwise.  Two attribute objects do
    // not have the same value if one or more respective attributes differ in
    // values.

inline
bsl::ostream& operator<<(bsl::ostream& stream, const EncoderTestSequenceWithUntagged9& rhs);
    // Format the specified 'rhs' to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& hashAlg, const baljsn::EncoderTestSequenceWithUntagged9& object);
    // Pass the specified 'object' to the specified 'hashAlg'.  This function
    // integrates with the 'bslh' modular hashing system and effectively
    // provides a 'bsl::hash' specialization for 'EncoderTestSequenceWithUntagged9'.

}  // close package namespace

// TRAITS

BDLAT_DECL_SEQUENCE_WITH_BITWISEMOVEABLE_TRAITS(baljsn::EncoderTestSequenceWithUntagged9)

// ============================================================================
//                         INLINE FUNCTION DEFINITIONS
// ============================================================================

namespace baljsn {

                          // ------------------------
                          // class EncoderTestAddress
                          // ------------------------

// CLASS METHODS
// MANIPULATORS
template <class MANIPULATOR>
int EncoderTestAddress::manipulateAttributes(MANIPULATOR& manipulator)
{
    int ret;

    ret = manipulator(&d_street, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_STREET]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_city, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CITY]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_state, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_STATE]);
    if (ret) {
        return ret;
    }

    return ret;
}

template <class MANIPULATOR>
int EncoderTestAddress::manipulateAttribute(MANIPULATOR& manipulator, int id)
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_STREET: {
        return manipulator(&d_street, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_STREET]);
      }
      case ATTRIBUTE_ID_CITY: {
        return manipulator(&d_city, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CITY]);
      }
      case ATTRIBUTE_ID_STATE: {
        return manipulator(&d_state, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_STATE]);
      }
      default:
        return NOT_FOUND;
    }
}

template <class MANIPULATOR>
int EncoderTestAddress::manipulateAttribute(
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
bsl::string& EncoderTestAddress::street()
{
    return d_street;
}

inline
bsl::string& EncoderTestAddress::city()
{
    return d_city;
}

inline
bsl::string& EncoderTestAddress::state()
{
    return d_state;
}

// ACCESSORS
template <class ACCESSOR>
int EncoderTestAddress::accessAttributes(ACCESSOR& accessor) const
{
    int ret;

    ret = accessor(d_street, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_STREET]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_city, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CITY]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_state, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_STATE]);
    if (ret) {
        return ret;
    }

    return ret;
}

template <class ACCESSOR>
int EncoderTestAddress::accessAttribute(ACCESSOR& accessor, int id) const
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_STREET: {
        return accessor(d_street, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_STREET]);
      }
      case ATTRIBUTE_ID_CITY: {
        return accessor(d_city, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CITY]);
      }
      case ATTRIBUTE_ID_STATE: {
        return accessor(d_state, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_STATE]);
      }
      default:
        return NOT_FOUND;
    }
}

template <class ACCESSOR>
int EncoderTestAddress::accessAttribute(
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
const bsl::string& EncoderTestAddress::street() const
{
    return d_street;
}

inline
const bsl::string& EncoderTestAddress::city() const
{
    return d_city;
}

inline
const bsl::string& EncoderTestAddress::state() const
{
    return d_state;
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& hashAlg, const baljsn::EncoderTestAddress& object)
{
    (void)hashAlg;
    (void)object;
    using bslh::hashAppend;
    hashAppend(hashAlg, object.street());
    hashAppend(hashAlg, object.city());
    hashAppend(hashAlg, object.state());
}



               // ----------------------------------------------
               // class EncoderTestChoiceWithAllCategoriesChoice
               // ----------------------------------------------

// CLASS METHODS
// CREATORS
inline
EncoderTestChoiceWithAllCategoriesChoice::EncoderTestChoiceWithAllCategoriesChoice()
: d_selectionId(SELECTION_ID_UNDEFINED)
{
}

inline
EncoderTestChoiceWithAllCategoriesChoice::~EncoderTestChoiceWithAllCategoriesChoice()
{
    reset();
}

// MANIPULATORS
template <class MANIPULATOR>
int EncoderTestChoiceWithAllCategoriesChoice::manipulateSelection(MANIPULATOR& manipulator)
{
    switch (d_selectionId) {
      case EncoderTestChoiceWithAllCategoriesChoice::SELECTION_ID_SELECTION0:
        return manipulator(&d_selection0.object(),
                SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION0]);
      default:
        BSLS_ASSERT(EncoderTestChoiceWithAllCategoriesChoice::SELECTION_ID_UNDEFINED == d_selectionId);
        return -1;
    }
}

inline
int& EncoderTestChoiceWithAllCategoriesChoice::selection0()
{
    BSLS_ASSERT(SELECTION_ID_SELECTION0 == d_selectionId);
    return d_selection0.object();
}

// ACCESSORS
inline
int EncoderTestChoiceWithAllCategoriesChoice::selectionId() const
{
    return d_selectionId;
}

template <class ACCESSOR>
int EncoderTestChoiceWithAllCategoriesChoice::accessSelection(ACCESSOR& accessor) const
{
    switch (d_selectionId) {
      case SELECTION_ID_SELECTION0:
        return accessor(d_selection0.object(),
                SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION0]);
      default:
        BSLS_ASSERT(SELECTION_ID_UNDEFINED == d_selectionId);
        return -1;
    }
}

inline
const int& EncoderTestChoiceWithAllCategoriesChoice::selection0() const
{
    BSLS_ASSERT(SELECTION_ID_SELECTION0 == d_selectionId);
    return d_selection0.object();
}

inline
bool EncoderTestChoiceWithAllCategoriesChoice::isSelection0Value() const
{
    return SELECTION_ID_SELECTION0 == d_selectionId;
}

inline
bool EncoderTestChoiceWithAllCategoriesChoice::isUndefinedValue() const
{
    return SELECTION_ID_UNDEFINED == d_selectionId;
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& hashAlg, const baljsn::EncoderTestChoiceWithAllCategoriesChoice& object)
{
    typedef baljsn::EncoderTestChoiceWithAllCategoriesChoice Class;
    using bslh::hashAppend;
    hashAppend(hashAlg, object.selectionId());
    switch (object.selectionId()) {
      case Class::SELECTION_ID_SELECTION0:
        hashAppend(hashAlg, object.selection0());
        break;
      default:
        BSLS_ASSERT(Class::SELECTION_ID_UNDEFINED == object.selectionId());
    }
}


           // ------------------------------------------------------
           // class EncoderTestChoiceWithAllCategoriesCustomizedType
           // ------------------------------------------------------

// CREATORS
inline
EncoderTestChoiceWithAllCategoriesCustomizedType::EncoderTestChoiceWithAllCategoriesCustomizedType(bslma::Allocator *basicAllocator)
: d_value(basicAllocator)
{
}

inline
EncoderTestChoiceWithAllCategoriesCustomizedType::EncoderTestChoiceWithAllCategoriesCustomizedType(const EncoderTestChoiceWithAllCategoriesCustomizedType& original, bslma::Allocator *basicAllocator)
: d_value(original.d_value, basicAllocator)
{
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
inline
EncoderTestChoiceWithAllCategoriesCustomizedType::EncoderTestChoiceWithAllCategoriesCustomizedType(EncoderTestChoiceWithAllCategoriesCustomizedType&& original, bslma::Allocator *basicAllocator)
: d_value(bsl::move(original.d_value), basicAllocator)
{
}
#endif

inline
EncoderTestChoiceWithAllCategoriesCustomizedType::EncoderTestChoiceWithAllCategoriesCustomizedType(const bsl::string& value, bslma::Allocator *basicAllocator)
: d_value(value, basicAllocator)
{
    BSLS_ASSERT(checkRestrictions(value) == 0);
}

inline
EncoderTestChoiceWithAllCategoriesCustomizedType::~EncoderTestChoiceWithAllCategoriesCustomizedType()
{
}

// MANIPULATORS
inline
EncoderTestChoiceWithAllCategoriesCustomizedType& EncoderTestChoiceWithAllCategoriesCustomizedType::operator=(const EncoderTestChoiceWithAllCategoriesCustomizedType& rhs)
{
    d_value = rhs.d_value;
    return *this;
}

inline
void EncoderTestChoiceWithAllCategoriesCustomizedType::reset()
{
    bdlat_ValueTypeFunctions::reset(&d_value);
}

inline
int EncoderTestChoiceWithAllCategoriesCustomizedType::fromString(const bsl::string& value)
{
    int ret = checkRestrictions(value);
    if (0 == ret) {
        d_value = value;
    }

    return ret;
}

// ACCESSORS
inline
bsl::ostream& EncoderTestChoiceWithAllCategoriesCustomizedType::print(bsl::ostream& stream,
                                 int           level,
                                 int           spacesPerLevel) const
{
    return bdlb::PrintMethods::print(stream, d_value, level, spacesPerLevel);
}

inline
const bsl::string& EncoderTestChoiceWithAllCategoriesCustomizedType::toString() const
{
    return d_value;
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& hashAlg, const baljsn::EncoderTestChoiceWithAllCategoriesCustomizedType& object)
{
    using bslh::hashAppend;
    hashAppend(hashAlg, object.toString());
}



            // ---------------------------------------------------
            // class EncoderTestChoiceWithAllCategoriesEnumeration
            // ---------------------------------------------------

// CLASS METHODS
inline
int EncoderTestChoiceWithAllCategoriesEnumeration::fromString(Value *result, const bsl::string& string)
{
    return fromString(result, string.c_str(), static_cast<int>(string.length()));
}

inline
bsl::ostream& EncoderTestChoiceWithAllCategoriesEnumeration::print(bsl::ostream&      stream,
                                 EncoderTestChoiceWithAllCategoriesEnumeration::Value value)
{
    return stream << toString(value);
}



              // ------------------------------------------------
              // class EncoderTestChoiceWithAllCategoriesSequence
              // ------------------------------------------------

// CLASS METHODS
// MANIPULATORS
template <class MANIPULATOR>
int EncoderTestChoiceWithAllCategoriesSequence::manipulateAttributes(MANIPULATOR& manipulator)
{
    int ret;

    ret = manipulator(&d_attribute, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE]);
    if (ret) {
        return ret;
    }

    return ret;
}

template <class MANIPULATOR>
int EncoderTestChoiceWithAllCategoriesSequence::manipulateAttribute(MANIPULATOR& manipulator, int id)
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_ATTRIBUTE: {
        return manipulator(&d_attribute, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE]);
      }
      default:
        return NOT_FOUND;
    }
}

template <class MANIPULATOR>
int EncoderTestChoiceWithAllCategoriesSequence::manipulateAttribute(
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
int& EncoderTestChoiceWithAllCategoriesSequence::attribute()
{
    return d_attribute;
}

// ACCESSORS
template <class ACCESSOR>
int EncoderTestChoiceWithAllCategoriesSequence::accessAttributes(ACCESSOR& accessor) const
{
    int ret;

    ret = accessor(d_attribute, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE]);
    if (ret) {
        return ret;
    }

    return ret;
}

template <class ACCESSOR>
int EncoderTestChoiceWithAllCategoriesSequence::accessAttribute(ACCESSOR& accessor, int id) const
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_ATTRIBUTE: {
        return accessor(d_attribute, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE]);
      }
      default:
        return NOT_FOUND;
    }
}

template <class ACCESSOR>
int EncoderTestChoiceWithAllCategoriesSequence::accessAttribute(
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
int EncoderTestChoiceWithAllCategoriesSequence::attribute() const
{
    return d_attribute;
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& hashAlg, const baljsn::EncoderTestChoiceWithAllCategoriesSequence& object)
{
    (void)hashAlg;
    (void)object;
    using bslh::hashAppend;
    hashAppend(hashAlg, object.attribute());
}



                 // ------------------------------------------
                 // class EncoderTestDegenerateChoice1Sequence
                 // ------------------------------------------

// CLASS METHODS
// MANIPULATORS
template <class MANIPULATOR>
int EncoderTestDegenerateChoice1Sequence::manipulateAttributes(MANIPULATOR& manipulator)
{
    (void)manipulator;
    int ret = 0;

    return ret;
}

template <class MANIPULATOR>
int EncoderTestDegenerateChoice1Sequence::manipulateAttribute(MANIPULATOR& manipulator, int id)
{
    (void)manipulator;
    enum { NOT_FOUND = -1 };

    switch (id) {
      default:
        return NOT_FOUND;
    }
}

template <class MANIPULATOR>
int EncoderTestDegenerateChoice1Sequence::manipulateAttribute(
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

// ACCESSORS
template <class ACCESSOR>
int EncoderTestDegenerateChoice1Sequence::accessAttributes(ACCESSOR& accessor) const
{
    (void)accessor;
    int ret = 0;

    return ret;
}

template <class ACCESSOR>
int EncoderTestDegenerateChoice1Sequence::accessAttribute(ACCESSOR& accessor, int id) const
{
    (void)accessor;
    enum { NOT_FOUND = -1 };

    switch (id) {
      default:
        return NOT_FOUND;
    }
}

template <class ACCESSOR>
int EncoderTestDegenerateChoice1Sequence::accessAttribute(
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

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& hashAlg, const baljsn::EncoderTestDegenerateChoice1Sequence& object)
{
    (void)hashAlg;
    (void)object;
    using bslh::hashAppend;
}



              // ------------------------------------------------
              // class EncoderTestSequenceWithAllCategoriesChoice
              // ------------------------------------------------

// CLASS METHODS
// CREATORS
inline
EncoderTestSequenceWithAllCategoriesChoice::EncoderTestSequenceWithAllCategoriesChoice()
: d_selectionId(SELECTION_ID_UNDEFINED)
{
}

inline
EncoderTestSequenceWithAllCategoriesChoice::~EncoderTestSequenceWithAllCategoriesChoice()
{
    reset();
}

// MANIPULATORS
template <class MANIPULATOR>
int EncoderTestSequenceWithAllCategoriesChoice::manipulateSelection(MANIPULATOR& manipulator)
{
    switch (d_selectionId) {
      case EncoderTestSequenceWithAllCategoriesChoice::SELECTION_ID_SELECTION0:
        return manipulator(&d_selection0.object(),
                SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION0]);
      default:
        BSLS_ASSERT(EncoderTestSequenceWithAllCategoriesChoice::SELECTION_ID_UNDEFINED == d_selectionId);
        return -1;
    }
}

inline
int& EncoderTestSequenceWithAllCategoriesChoice::selection0()
{
    BSLS_ASSERT(SELECTION_ID_SELECTION0 == d_selectionId);
    return d_selection0.object();
}

// ACCESSORS
inline
int EncoderTestSequenceWithAllCategoriesChoice::selectionId() const
{
    return d_selectionId;
}

template <class ACCESSOR>
int EncoderTestSequenceWithAllCategoriesChoice::accessSelection(ACCESSOR& accessor) const
{
    switch (d_selectionId) {
      case SELECTION_ID_SELECTION0:
        return accessor(d_selection0.object(),
                SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION0]);
      default:
        BSLS_ASSERT(SELECTION_ID_UNDEFINED == d_selectionId);
        return -1;
    }
}

inline
const int& EncoderTestSequenceWithAllCategoriesChoice::selection0() const
{
    BSLS_ASSERT(SELECTION_ID_SELECTION0 == d_selectionId);
    return d_selection0.object();
}

inline
bool EncoderTestSequenceWithAllCategoriesChoice::isSelection0Value() const
{
    return SELECTION_ID_SELECTION0 == d_selectionId;
}

inline
bool EncoderTestSequenceWithAllCategoriesChoice::isUndefinedValue() const
{
    return SELECTION_ID_UNDEFINED == d_selectionId;
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& hashAlg, const baljsn::EncoderTestSequenceWithAllCategoriesChoice& object)
{
    typedef baljsn::EncoderTestSequenceWithAllCategoriesChoice Class;
    using bslh::hashAppend;
    hashAppend(hashAlg, object.selectionId());
    switch (object.selectionId()) {
      case Class::SELECTION_ID_SELECTION0:
        hashAppend(hashAlg, object.selection0());
        break;
      default:
        BSLS_ASSERT(Class::SELECTION_ID_UNDEFINED == object.selectionId());
    }
}


          // --------------------------------------------------------
          // class EncoderTestSequenceWithAllCategoriesCustomizedType
          // --------------------------------------------------------

// CREATORS
inline
EncoderTestSequenceWithAllCategoriesCustomizedType::EncoderTestSequenceWithAllCategoriesCustomizedType(bslma::Allocator *basicAllocator)
: d_value(basicAllocator)
{
}

inline
EncoderTestSequenceWithAllCategoriesCustomizedType::EncoderTestSequenceWithAllCategoriesCustomizedType(const EncoderTestSequenceWithAllCategoriesCustomizedType& original, bslma::Allocator *basicAllocator)
: d_value(original.d_value, basicAllocator)
{
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
inline
EncoderTestSequenceWithAllCategoriesCustomizedType::EncoderTestSequenceWithAllCategoriesCustomizedType(EncoderTestSequenceWithAllCategoriesCustomizedType&& original, bslma::Allocator *basicAllocator)
: d_value(bsl::move(original.d_value), basicAllocator)
{
}
#endif

inline
EncoderTestSequenceWithAllCategoriesCustomizedType::EncoderTestSequenceWithAllCategoriesCustomizedType(const bsl::string& value, bslma::Allocator *basicAllocator)
: d_value(value, basicAllocator)
{
    BSLS_ASSERT(checkRestrictions(value) == 0);
}

inline
EncoderTestSequenceWithAllCategoriesCustomizedType::~EncoderTestSequenceWithAllCategoriesCustomizedType()
{
}

// MANIPULATORS
inline
EncoderTestSequenceWithAllCategoriesCustomizedType& EncoderTestSequenceWithAllCategoriesCustomizedType::operator=(const EncoderTestSequenceWithAllCategoriesCustomizedType& rhs)
{
    d_value = rhs.d_value;
    return *this;
}

inline
void EncoderTestSequenceWithAllCategoriesCustomizedType::reset()
{
    bdlat_ValueTypeFunctions::reset(&d_value);
}

inline
int EncoderTestSequenceWithAllCategoriesCustomizedType::fromString(const bsl::string& value)
{
    int ret = checkRestrictions(value);
    if (0 == ret) {
        d_value = value;
    }

    return ret;
}

// ACCESSORS
inline
bsl::ostream& EncoderTestSequenceWithAllCategoriesCustomizedType::print(bsl::ostream& stream,
                                 int           level,
                                 int           spacesPerLevel) const
{
    return bdlb::PrintMethods::print(stream, d_value, level, spacesPerLevel);
}

inline
const bsl::string& EncoderTestSequenceWithAllCategoriesCustomizedType::toString() const
{
    return d_value;
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& hashAlg, const baljsn::EncoderTestSequenceWithAllCategoriesCustomizedType& object)
{
    using bslh::hashAppend;
    hashAppend(hashAlg, object.toString());
}



           // -----------------------------------------------------
           // class EncoderTestSequenceWithAllCategoriesEnumeration
           // -----------------------------------------------------

// CLASS METHODS
inline
int EncoderTestSequenceWithAllCategoriesEnumeration::fromString(Value *result, const bsl::string& string)
{
    return fromString(result, string.c_str(), static_cast<int>(string.length()));
}

inline
bsl::ostream& EncoderTestSequenceWithAllCategoriesEnumeration::print(bsl::ostream&      stream,
                                 EncoderTestSequenceWithAllCategoriesEnumeration::Value value)
{
    return stream << toString(value);
}



             // --------------------------------------------------
             // class EncoderTestSequenceWithAllCategoriesSequence
             // --------------------------------------------------

// CLASS METHODS
// MANIPULATORS
template <class MANIPULATOR>
int EncoderTestSequenceWithAllCategoriesSequence::manipulateAttributes(MANIPULATOR& manipulator)
{
    int ret;

    ret = manipulator(&d_attribute, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE]);
    if (ret) {
        return ret;
    }

    return ret;
}

template <class MANIPULATOR>
int EncoderTestSequenceWithAllCategoriesSequence::manipulateAttribute(MANIPULATOR& manipulator, int id)
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_ATTRIBUTE: {
        return manipulator(&d_attribute, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE]);
      }
      default:
        return NOT_FOUND;
    }
}

template <class MANIPULATOR>
int EncoderTestSequenceWithAllCategoriesSequence::manipulateAttribute(
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
int& EncoderTestSequenceWithAllCategoriesSequence::attribute()
{
    return d_attribute;
}

// ACCESSORS
template <class ACCESSOR>
int EncoderTestSequenceWithAllCategoriesSequence::accessAttributes(ACCESSOR& accessor) const
{
    int ret;

    ret = accessor(d_attribute, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE]);
    if (ret) {
        return ret;
    }

    return ret;
}

template <class ACCESSOR>
int EncoderTestSequenceWithAllCategoriesSequence::accessAttribute(ACCESSOR& accessor, int id) const
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_ATTRIBUTE: {
        return accessor(d_attribute, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE]);
      }
      default:
        return NOT_FOUND;
    }
}

template <class ACCESSOR>
int EncoderTestSequenceWithAllCategoriesSequence::accessAttribute(
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
int EncoderTestSequenceWithAllCategoriesSequence::attribute() const
{
    return d_attribute;
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& hashAlg, const baljsn::EncoderTestSequenceWithAllCategoriesSequence& object)
{
    (void)hashAlg;
    (void)object;
    using bslh::hashAppend;
    hashAppend(hashAlg, object.attribute());
}



                   // --------------------------------------
                   // class EncoderTestSequenceWithUntagged0
                   // --------------------------------------

// CLASS METHODS
// MANIPULATORS
template <class MANIPULATOR>
int EncoderTestSequenceWithUntagged0::manipulateAttributes(MANIPULATOR& manipulator)
{
    (void)manipulator;
    int ret = 0;

    return ret;
}

template <class MANIPULATOR>
int EncoderTestSequenceWithUntagged0::manipulateAttribute(MANIPULATOR& manipulator, int id)
{
    (void)manipulator;
    enum { NOT_FOUND = -1 };

    switch (id) {
      default:
        return NOT_FOUND;
    }
}

template <class MANIPULATOR>
int EncoderTestSequenceWithUntagged0::manipulateAttribute(
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

// ACCESSORS
template <class ACCESSOR>
int EncoderTestSequenceWithUntagged0::accessAttributes(ACCESSOR& accessor) const
{
    (void)accessor;
    int ret = 0;

    return ret;
}

template <class ACCESSOR>
int EncoderTestSequenceWithUntagged0::accessAttribute(ACCESSOR& accessor, int id) const
{
    (void)accessor;
    enum { NOT_FOUND = -1 };

    switch (id) {
      default:
        return NOT_FOUND;
    }
}

template <class ACCESSOR>
int EncoderTestSequenceWithUntagged0::accessAttribute(
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

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& hashAlg, const baljsn::EncoderTestSequenceWithUntagged0& object)
{
    (void)hashAlg;
    (void)object;
    using bslh::hashAppend;
}



              // -----------------------------------------------
              // class EncoderTestSequenceWithUntagged10Sequence
              // -----------------------------------------------

// CLASS METHODS
// MANIPULATORS
template <class MANIPULATOR>
int EncoderTestSequenceWithUntagged10Sequence::manipulateAttributes(MANIPULATOR& manipulator)
{
    (void)manipulator;
    int ret = 0;

    return ret;
}

template <class MANIPULATOR>
int EncoderTestSequenceWithUntagged10Sequence::manipulateAttribute(MANIPULATOR& manipulator, int id)
{
    (void)manipulator;
    enum { NOT_FOUND = -1 };

    switch (id) {
      default:
        return NOT_FOUND;
    }
}

template <class MANIPULATOR>
int EncoderTestSequenceWithUntagged10Sequence::manipulateAttribute(
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

// ACCESSORS
template <class ACCESSOR>
int EncoderTestSequenceWithUntagged10Sequence::accessAttributes(ACCESSOR& accessor) const
{
    (void)accessor;
    int ret = 0;

    return ret;
}

template <class ACCESSOR>
int EncoderTestSequenceWithUntagged10Sequence::accessAttribute(ACCESSOR& accessor, int id) const
{
    (void)accessor;
    enum { NOT_FOUND = -1 };

    switch (id) {
      default:
        return NOT_FOUND;
    }
}

template <class ACCESSOR>
int EncoderTestSequenceWithUntagged10Sequence::accessAttribute(
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

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& hashAlg, const baljsn::EncoderTestSequenceWithUntagged10Sequence& object)
{
    (void)hashAlg;
    (void)object;
    using bslh::hashAppend;
}



              // -----------------------------------------------
              // class EncoderTestSequenceWithUntagged11Sequence
              // -----------------------------------------------

// CLASS METHODS
// MANIPULATORS
template <class MANIPULATOR>
int EncoderTestSequenceWithUntagged11Sequence::manipulateAttributes(MANIPULATOR& manipulator)
{
    (void)manipulator;
    int ret = 0;

    return ret;
}

template <class MANIPULATOR>
int EncoderTestSequenceWithUntagged11Sequence::manipulateAttribute(MANIPULATOR& manipulator, int id)
{
    (void)manipulator;
    enum { NOT_FOUND = -1 };

    switch (id) {
      default:
        return NOT_FOUND;
    }
}

template <class MANIPULATOR>
int EncoderTestSequenceWithUntagged11Sequence::manipulateAttribute(
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

// ACCESSORS
template <class ACCESSOR>
int EncoderTestSequenceWithUntagged11Sequence::accessAttributes(ACCESSOR& accessor) const
{
    (void)accessor;
    int ret = 0;

    return ret;
}

template <class ACCESSOR>
int EncoderTestSequenceWithUntagged11Sequence::accessAttribute(ACCESSOR& accessor, int id) const
{
    (void)accessor;
    enum { NOT_FOUND = -1 };

    switch (id) {
      default:
        return NOT_FOUND;
    }
}

template <class ACCESSOR>
int EncoderTestSequenceWithUntagged11Sequence::accessAttribute(
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

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& hashAlg, const baljsn::EncoderTestSequenceWithUntagged11Sequence& object)
{
    (void)hashAlg;
    (void)object;
    using bslh::hashAppend;
}



              // ------------------------------------------------
              // class EncoderTestSequenceWithUntagged11Sequence1
              // ------------------------------------------------

// CLASS METHODS
// MANIPULATORS
template <class MANIPULATOR>
int EncoderTestSequenceWithUntagged11Sequence1::manipulateAttributes(MANIPULATOR& manipulator)
{
    (void)manipulator;
    int ret = 0;

    return ret;
}

template <class MANIPULATOR>
int EncoderTestSequenceWithUntagged11Sequence1::manipulateAttribute(MANIPULATOR& manipulator, int id)
{
    (void)manipulator;
    enum { NOT_FOUND = -1 };

    switch (id) {
      default:
        return NOT_FOUND;
    }
}

template <class MANIPULATOR>
int EncoderTestSequenceWithUntagged11Sequence1::manipulateAttribute(
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

// ACCESSORS
template <class ACCESSOR>
int EncoderTestSequenceWithUntagged11Sequence1::accessAttributes(ACCESSOR& accessor) const
{
    (void)accessor;
    int ret = 0;

    return ret;
}

template <class ACCESSOR>
int EncoderTestSequenceWithUntagged11Sequence1::accessAttribute(ACCESSOR& accessor, int id) const
{
    (void)accessor;
    enum { NOT_FOUND = -1 };

    switch (id) {
      default:
        return NOT_FOUND;
    }
}

template <class ACCESSOR>
int EncoderTestSequenceWithUntagged11Sequence1::accessAttribute(
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

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& hashAlg, const baljsn::EncoderTestSequenceWithUntagged11Sequence1& object)
{
    (void)hashAlg;
    (void)object;
    using bslh::hashAppend;
}



              // -----------------------------------------------
              // class EncoderTestSequenceWithUntagged12Sequence
              // -----------------------------------------------

// CLASS METHODS
// MANIPULATORS
template <class MANIPULATOR>
int EncoderTestSequenceWithUntagged12Sequence::manipulateAttributes(MANIPULATOR& manipulator)
{
    (void)manipulator;
    int ret = 0;

    return ret;
}

template <class MANIPULATOR>
int EncoderTestSequenceWithUntagged12Sequence::manipulateAttribute(MANIPULATOR& manipulator, int id)
{
    (void)manipulator;
    enum { NOT_FOUND = -1 };

    switch (id) {
      default:
        return NOT_FOUND;
    }
}

template <class MANIPULATOR>
int EncoderTestSequenceWithUntagged12Sequence::manipulateAttribute(
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

// ACCESSORS
template <class ACCESSOR>
int EncoderTestSequenceWithUntagged12Sequence::accessAttributes(ACCESSOR& accessor) const
{
    (void)accessor;
    int ret = 0;

    return ret;
}

template <class ACCESSOR>
int EncoderTestSequenceWithUntagged12Sequence::accessAttribute(ACCESSOR& accessor, int id) const
{
    (void)accessor;
    enum { NOT_FOUND = -1 };

    switch (id) {
      default:
        return NOT_FOUND;
    }
}

template <class ACCESSOR>
int EncoderTestSequenceWithUntagged12Sequence::accessAttribute(
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

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& hashAlg, const baljsn::EncoderTestSequenceWithUntagged12Sequence& object)
{
    (void)hashAlg;
    (void)object;
    using bslh::hashAppend;
}



              // -----------------------------------------------
              // class EncoderTestSequenceWithUntagged13Sequence
              // -----------------------------------------------

// CLASS METHODS
// MANIPULATORS
template <class MANIPULATOR>
int EncoderTestSequenceWithUntagged13Sequence::manipulateAttributes(MANIPULATOR& manipulator)
{
    (void)manipulator;
    int ret = 0;

    return ret;
}

template <class MANIPULATOR>
int EncoderTestSequenceWithUntagged13Sequence::manipulateAttribute(MANIPULATOR& manipulator, int id)
{
    (void)manipulator;
    enum { NOT_FOUND = -1 };

    switch (id) {
      default:
        return NOT_FOUND;
    }
}

template <class MANIPULATOR>
int EncoderTestSequenceWithUntagged13Sequence::manipulateAttribute(
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

// ACCESSORS
template <class ACCESSOR>
int EncoderTestSequenceWithUntagged13Sequence::accessAttributes(ACCESSOR& accessor) const
{
    (void)accessor;
    int ret = 0;

    return ret;
}

template <class ACCESSOR>
int EncoderTestSequenceWithUntagged13Sequence::accessAttribute(ACCESSOR& accessor, int id) const
{
    (void)accessor;
    enum { NOT_FOUND = -1 };

    switch (id) {
      default:
        return NOT_FOUND;
    }
}

template <class ACCESSOR>
int EncoderTestSequenceWithUntagged13Sequence::accessAttribute(
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

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& hashAlg, const baljsn::EncoderTestSequenceWithUntagged13Sequence& object)
{
    (void)hashAlg;
    (void)object;
    using bslh::hashAppend;
}



                  // ---------------------------------------
                  // class EncoderTestSequenceWithUntagged14
                  // ---------------------------------------

// CLASS METHODS
// MANIPULATORS
template <class MANIPULATOR>
int EncoderTestSequenceWithUntagged14::manipulateAttributes(MANIPULATOR& manipulator)
{
    int ret;

    ret = manipulator(&d_attribute0, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE0]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_attribute1, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE1]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_attribute2, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE2]);
    if (ret) {
        return ret;
    }

    return ret;
}

template <class MANIPULATOR>
int EncoderTestSequenceWithUntagged14::manipulateAttribute(MANIPULATOR& manipulator, int id)
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_ATTRIBUTE0: {
        return manipulator(&d_attribute0, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE0]);
      }
      case ATTRIBUTE_ID_ATTRIBUTE1: {
        return manipulator(&d_attribute1, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE1]);
      }
      case ATTRIBUTE_ID_ATTRIBUTE2: {
        return manipulator(&d_attribute2, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE2]);
      }
      default:
        return NOT_FOUND;
    }
}

template <class MANIPULATOR>
int EncoderTestSequenceWithUntagged14::manipulateAttribute(
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
int& EncoderTestSequenceWithUntagged14::attribute0()
{
    return d_attribute0;
}

inline
int& EncoderTestSequenceWithUntagged14::attribute1()
{
    return d_attribute1;
}

inline
int& EncoderTestSequenceWithUntagged14::attribute2()
{
    return d_attribute2;
}

// ACCESSORS
template <class ACCESSOR>
int EncoderTestSequenceWithUntagged14::accessAttributes(ACCESSOR& accessor) const
{
    int ret;

    ret = accessor(d_attribute0, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE0]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_attribute1, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE1]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_attribute2, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE2]);
    if (ret) {
        return ret;
    }

    return ret;
}

template <class ACCESSOR>
int EncoderTestSequenceWithUntagged14::accessAttribute(ACCESSOR& accessor, int id) const
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_ATTRIBUTE0: {
        return accessor(d_attribute0, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE0]);
      }
      case ATTRIBUTE_ID_ATTRIBUTE1: {
        return accessor(d_attribute1, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE1]);
      }
      case ATTRIBUTE_ID_ATTRIBUTE2: {
        return accessor(d_attribute2, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE2]);
      }
      default:
        return NOT_FOUND;
    }
}

template <class ACCESSOR>
int EncoderTestSequenceWithUntagged14::accessAttribute(
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
int EncoderTestSequenceWithUntagged14::attribute0() const
{
    return d_attribute0;
}

inline
int EncoderTestSequenceWithUntagged14::attribute1() const
{
    return d_attribute1;
}

inline
int EncoderTestSequenceWithUntagged14::attribute2() const
{
    return d_attribute2;
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& hashAlg, const baljsn::EncoderTestSequenceWithUntagged14& object)
{
    (void)hashAlg;
    (void)object;
    using bslh::hashAppend;
    hashAppend(hashAlg, object.attribute0());
    hashAppend(hashAlg, object.attribute1());
    hashAppend(hashAlg, object.attribute2());
}



               // ----------------------------------------------
               // class EncoderTestSequenceWithUntagged1Sequence
               // ----------------------------------------------

// CLASS METHODS
// MANIPULATORS
template <class MANIPULATOR>
int EncoderTestSequenceWithUntagged1Sequence::manipulateAttributes(MANIPULATOR& manipulator)
{
    (void)manipulator;
    int ret = 0;

    return ret;
}

template <class MANIPULATOR>
int EncoderTestSequenceWithUntagged1Sequence::manipulateAttribute(MANIPULATOR& manipulator, int id)
{
    (void)manipulator;
    enum { NOT_FOUND = -1 };

    switch (id) {
      default:
        return NOT_FOUND;
    }
}

template <class MANIPULATOR>
int EncoderTestSequenceWithUntagged1Sequence::manipulateAttribute(
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

// ACCESSORS
template <class ACCESSOR>
int EncoderTestSequenceWithUntagged1Sequence::accessAttributes(ACCESSOR& accessor) const
{
    (void)accessor;
    int ret = 0;

    return ret;
}

template <class ACCESSOR>
int EncoderTestSequenceWithUntagged1Sequence::accessAttribute(ACCESSOR& accessor, int id) const
{
    (void)accessor;
    enum { NOT_FOUND = -1 };

    switch (id) {
      default:
        return NOT_FOUND;
    }
}

template <class ACCESSOR>
int EncoderTestSequenceWithUntagged1Sequence::accessAttribute(
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

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& hashAlg, const baljsn::EncoderTestSequenceWithUntagged1Sequence& object)
{
    (void)hashAlg;
    (void)object;
    using bslh::hashAppend;
}



                   // --------------------------------------
                   // class EncoderTestSequenceWithUntagged2
                   // --------------------------------------

// CLASS METHODS
// MANIPULATORS
template <class MANIPULATOR>
int EncoderTestSequenceWithUntagged2::manipulateAttributes(MANIPULATOR& manipulator)
{
    int ret;

    ret = manipulator(&d_attribute0, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE0]);
    if (ret) {
        return ret;
    }

    return ret;
}

template <class MANIPULATOR>
int EncoderTestSequenceWithUntagged2::manipulateAttribute(MANIPULATOR& manipulator, int id)
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_ATTRIBUTE0: {
        return manipulator(&d_attribute0, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE0]);
      }
      default:
        return NOT_FOUND;
    }
}

template <class MANIPULATOR>
int EncoderTestSequenceWithUntagged2::manipulateAttribute(
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
int& EncoderTestSequenceWithUntagged2::attribute0()
{
    return d_attribute0;
}

// ACCESSORS
template <class ACCESSOR>
int EncoderTestSequenceWithUntagged2::accessAttributes(ACCESSOR& accessor) const
{
    int ret;

    ret = accessor(d_attribute0, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE0]);
    if (ret) {
        return ret;
    }

    return ret;
}

template <class ACCESSOR>
int EncoderTestSequenceWithUntagged2::accessAttribute(ACCESSOR& accessor, int id) const
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_ATTRIBUTE0: {
        return accessor(d_attribute0, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE0]);
      }
      default:
        return NOT_FOUND;
    }
}

template <class ACCESSOR>
int EncoderTestSequenceWithUntagged2::accessAttribute(
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
int EncoderTestSequenceWithUntagged2::attribute0() const
{
    return d_attribute0;
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& hashAlg, const baljsn::EncoderTestSequenceWithUntagged2& object)
{
    (void)hashAlg;
    (void)object;
    using bslh::hashAppend;
    hashAppend(hashAlg, object.attribute0());
}



               // ----------------------------------------------
               // class EncoderTestSequenceWithUntagged3Sequence
               // ----------------------------------------------

// CLASS METHODS
// MANIPULATORS
template <class MANIPULATOR>
int EncoderTestSequenceWithUntagged3Sequence::manipulateAttributes(MANIPULATOR& manipulator)
{
    (void)manipulator;
    int ret = 0;

    return ret;
}

template <class MANIPULATOR>
int EncoderTestSequenceWithUntagged3Sequence::manipulateAttribute(MANIPULATOR& manipulator, int id)
{
    (void)manipulator;
    enum { NOT_FOUND = -1 };

    switch (id) {
      default:
        return NOT_FOUND;
    }
}

template <class MANIPULATOR>
int EncoderTestSequenceWithUntagged3Sequence::manipulateAttribute(
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

// ACCESSORS
template <class ACCESSOR>
int EncoderTestSequenceWithUntagged3Sequence::accessAttributes(ACCESSOR& accessor) const
{
    (void)accessor;
    int ret = 0;

    return ret;
}

template <class ACCESSOR>
int EncoderTestSequenceWithUntagged3Sequence::accessAttribute(ACCESSOR& accessor, int id) const
{
    (void)accessor;
    enum { NOT_FOUND = -1 };

    switch (id) {
      default:
        return NOT_FOUND;
    }
}

template <class ACCESSOR>
int EncoderTestSequenceWithUntagged3Sequence::accessAttribute(
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

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& hashAlg, const baljsn::EncoderTestSequenceWithUntagged3Sequence& object)
{
    (void)hashAlg;
    (void)object;
    using bslh::hashAppend;
}



              // -----------------------------------------------
              // class EncoderTestSequenceWithUntagged3Sequence1
              // -----------------------------------------------

// CLASS METHODS
// MANIPULATORS
template <class MANIPULATOR>
int EncoderTestSequenceWithUntagged3Sequence1::manipulateAttributes(MANIPULATOR& manipulator)
{
    (void)manipulator;
    int ret = 0;

    return ret;
}

template <class MANIPULATOR>
int EncoderTestSequenceWithUntagged3Sequence1::manipulateAttribute(MANIPULATOR& manipulator, int id)
{
    (void)manipulator;
    enum { NOT_FOUND = -1 };

    switch (id) {
      default:
        return NOT_FOUND;
    }
}

template <class MANIPULATOR>
int EncoderTestSequenceWithUntagged3Sequence1::manipulateAttribute(
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

// ACCESSORS
template <class ACCESSOR>
int EncoderTestSequenceWithUntagged3Sequence1::accessAttributes(ACCESSOR& accessor) const
{
    (void)accessor;
    int ret = 0;

    return ret;
}

template <class ACCESSOR>
int EncoderTestSequenceWithUntagged3Sequence1::accessAttribute(ACCESSOR& accessor, int id) const
{
    (void)accessor;
    enum { NOT_FOUND = -1 };

    switch (id) {
      default:
        return NOT_FOUND;
    }
}

template <class ACCESSOR>
int EncoderTestSequenceWithUntagged3Sequence1::accessAttribute(
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

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& hashAlg, const baljsn::EncoderTestSequenceWithUntagged3Sequence1& object)
{
    (void)hashAlg;
    (void)object;
    using bslh::hashAppend;
}



               // ----------------------------------------------
               // class EncoderTestSequenceWithUntagged4Sequence
               // ----------------------------------------------

// CLASS METHODS
// MANIPULATORS
template <class MANIPULATOR>
int EncoderTestSequenceWithUntagged4Sequence::manipulateAttributes(MANIPULATOR& manipulator)
{
    (void)manipulator;
    int ret = 0;

    return ret;
}

template <class MANIPULATOR>
int EncoderTestSequenceWithUntagged4Sequence::manipulateAttribute(MANIPULATOR& manipulator, int id)
{
    (void)manipulator;
    enum { NOT_FOUND = -1 };

    switch (id) {
      default:
        return NOT_FOUND;
    }
}

template <class MANIPULATOR>
int EncoderTestSequenceWithUntagged4Sequence::manipulateAttribute(
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

// ACCESSORS
template <class ACCESSOR>
int EncoderTestSequenceWithUntagged4Sequence::accessAttributes(ACCESSOR& accessor) const
{
    (void)accessor;
    int ret = 0;

    return ret;
}

template <class ACCESSOR>
int EncoderTestSequenceWithUntagged4Sequence::accessAttribute(ACCESSOR& accessor, int id) const
{
    (void)accessor;
    enum { NOT_FOUND = -1 };

    switch (id) {
      default:
        return NOT_FOUND;
    }
}

template <class ACCESSOR>
int EncoderTestSequenceWithUntagged4Sequence::accessAttribute(
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

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& hashAlg, const baljsn::EncoderTestSequenceWithUntagged4Sequence& object)
{
    (void)hashAlg;
    (void)object;
    using bslh::hashAppend;
}



               // ----------------------------------------------
               // class EncoderTestSequenceWithUntagged5Sequence
               // ----------------------------------------------

// CLASS METHODS
// MANIPULATORS
template <class MANIPULATOR>
int EncoderTestSequenceWithUntagged5Sequence::manipulateAttributes(MANIPULATOR& manipulator)
{
    (void)manipulator;
    int ret = 0;

    return ret;
}

template <class MANIPULATOR>
int EncoderTestSequenceWithUntagged5Sequence::manipulateAttribute(MANIPULATOR& manipulator, int id)
{
    (void)manipulator;
    enum { NOT_FOUND = -1 };

    switch (id) {
      default:
        return NOT_FOUND;
    }
}

template <class MANIPULATOR>
int EncoderTestSequenceWithUntagged5Sequence::manipulateAttribute(
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

// ACCESSORS
template <class ACCESSOR>
int EncoderTestSequenceWithUntagged5Sequence::accessAttributes(ACCESSOR& accessor) const
{
    (void)accessor;
    int ret = 0;

    return ret;
}

template <class ACCESSOR>
int EncoderTestSequenceWithUntagged5Sequence::accessAttribute(ACCESSOR& accessor, int id) const
{
    (void)accessor;
    enum { NOT_FOUND = -1 };

    switch (id) {
      default:
        return NOT_FOUND;
    }
}

template <class ACCESSOR>
int EncoderTestSequenceWithUntagged5Sequence::accessAttribute(
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

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& hashAlg, const baljsn::EncoderTestSequenceWithUntagged5Sequence& object)
{
    (void)hashAlg;
    (void)object;
    using bslh::hashAppend;
}



                   // --------------------------------------
                   // class EncoderTestSequenceWithUntagged6
                   // --------------------------------------

// CLASS METHODS
// MANIPULATORS
template <class MANIPULATOR>
int EncoderTestSequenceWithUntagged6::manipulateAttributes(MANIPULATOR& manipulator)
{
    int ret;

    ret = manipulator(&d_attribute0, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE0]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_attribute1, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE1]);
    if (ret) {
        return ret;
    }

    return ret;
}

template <class MANIPULATOR>
int EncoderTestSequenceWithUntagged6::manipulateAttribute(MANIPULATOR& manipulator, int id)
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_ATTRIBUTE0: {
        return manipulator(&d_attribute0, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE0]);
      }
      case ATTRIBUTE_ID_ATTRIBUTE1: {
        return manipulator(&d_attribute1, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE1]);
      }
      default:
        return NOT_FOUND;
    }
}

template <class MANIPULATOR>
int EncoderTestSequenceWithUntagged6::manipulateAttribute(
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
int& EncoderTestSequenceWithUntagged6::attribute0()
{
    return d_attribute0;
}

inline
int& EncoderTestSequenceWithUntagged6::attribute1()
{
    return d_attribute1;
}

// ACCESSORS
template <class ACCESSOR>
int EncoderTestSequenceWithUntagged6::accessAttributes(ACCESSOR& accessor) const
{
    int ret;

    ret = accessor(d_attribute0, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE0]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_attribute1, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE1]);
    if (ret) {
        return ret;
    }

    return ret;
}

template <class ACCESSOR>
int EncoderTestSequenceWithUntagged6::accessAttribute(ACCESSOR& accessor, int id) const
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_ATTRIBUTE0: {
        return accessor(d_attribute0, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE0]);
      }
      case ATTRIBUTE_ID_ATTRIBUTE1: {
        return accessor(d_attribute1, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE1]);
      }
      default:
        return NOT_FOUND;
    }
}

template <class ACCESSOR>
int EncoderTestSequenceWithUntagged6::accessAttribute(
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
int EncoderTestSequenceWithUntagged6::attribute0() const
{
    return d_attribute0;
}

inline
int EncoderTestSequenceWithUntagged6::attribute1() const
{
    return d_attribute1;
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& hashAlg, const baljsn::EncoderTestSequenceWithUntagged6& object)
{
    (void)hashAlg;
    (void)object;
    using bslh::hashAppend;
    hashAppend(hashAlg, object.attribute0());
    hashAppend(hashAlg, object.attribute1());
}



               // ----------------------------------------------
               // class EncoderTestSequenceWithUntagged7Sequence
               // ----------------------------------------------

// CLASS METHODS
// MANIPULATORS
template <class MANIPULATOR>
int EncoderTestSequenceWithUntagged7Sequence::manipulateAttributes(MANIPULATOR& manipulator)
{
    (void)manipulator;
    int ret = 0;

    return ret;
}

template <class MANIPULATOR>
int EncoderTestSequenceWithUntagged7Sequence::manipulateAttribute(MANIPULATOR& manipulator, int id)
{
    (void)manipulator;
    enum { NOT_FOUND = -1 };

    switch (id) {
      default:
        return NOT_FOUND;
    }
}

template <class MANIPULATOR>
int EncoderTestSequenceWithUntagged7Sequence::manipulateAttribute(
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

// ACCESSORS
template <class ACCESSOR>
int EncoderTestSequenceWithUntagged7Sequence::accessAttributes(ACCESSOR& accessor) const
{
    (void)accessor;
    int ret = 0;

    return ret;
}

template <class ACCESSOR>
int EncoderTestSequenceWithUntagged7Sequence::accessAttribute(ACCESSOR& accessor, int id) const
{
    (void)accessor;
    enum { NOT_FOUND = -1 };

    switch (id) {
      default:
        return NOT_FOUND;
    }
}

template <class ACCESSOR>
int EncoderTestSequenceWithUntagged7Sequence::accessAttribute(
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

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& hashAlg, const baljsn::EncoderTestSequenceWithUntagged7Sequence& object)
{
    (void)hashAlg;
    (void)object;
    using bslh::hashAppend;
}



              // -----------------------------------------------
              // class EncoderTestSequenceWithUntagged7Sequence1
              // -----------------------------------------------

// CLASS METHODS
// MANIPULATORS
template <class MANIPULATOR>
int EncoderTestSequenceWithUntagged7Sequence1::manipulateAttributes(MANIPULATOR& manipulator)
{
    (void)manipulator;
    int ret = 0;

    return ret;
}

template <class MANIPULATOR>
int EncoderTestSequenceWithUntagged7Sequence1::manipulateAttribute(MANIPULATOR& manipulator, int id)
{
    (void)manipulator;
    enum { NOT_FOUND = -1 };

    switch (id) {
      default:
        return NOT_FOUND;
    }
}

template <class MANIPULATOR>
int EncoderTestSequenceWithUntagged7Sequence1::manipulateAttribute(
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

// ACCESSORS
template <class ACCESSOR>
int EncoderTestSequenceWithUntagged7Sequence1::accessAttributes(ACCESSOR& accessor) const
{
    (void)accessor;
    int ret = 0;

    return ret;
}

template <class ACCESSOR>
int EncoderTestSequenceWithUntagged7Sequence1::accessAttribute(ACCESSOR& accessor, int id) const
{
    (void)accessor;
    enum { NOT_FOUND = -1 };

    switch (id) {
      default:
        return NOT_FOUND;
    }
}

template <class ACCESSOR>
int EncoderTestSequenceWithUntagged7Sequence1::accessAttribute(
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

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& hashAlg, const baljsn::EncoderTestSequenceWithUntagged7Sequence1& object)
{
    (void)hashAlg;
    (void)object;
    using bslh::hashAppend;
}



              // -----------------------------------------------
              // class EncoderTestSequenceWithUntagged7Sequence2
              // -----------------------------------------------

// CLASS METHODS
// MANIPULATORS
template <class MANIPULATOR>
int EncoderTestSequenceWithUntagged7Sequence2::manipulateAttributes(MANIPULATOR& manipulator)
{
    (void)manipulator;
    int ret = 0;

    return ret;
}

template <class MANIPULATOR>
int EncoderTestSequenceWithUntagged7Sequence2::manipulateAttribute(MANIPULATOR& manipulator, int id)
{
    (void)manipulator;
    enum { NOT_FOUND = -1 };

    switch (id) {
      default:
        return NOT_FOUND;
    }
}

template <class MANIPULATOR>
int EncoderTestSequenceWithUntagged7Sequence2::manipulateAttribute(
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

// ACCESSORS
template <class ACCESSOR>
int EncoderTestSequenceWithUntagged7Sequence2::accessAttributes(ACCESSOR& accessor) const
{
    (void)accessor;
    int ret = 0;

    return ret;
}

template <class ACCESSOR>
int EncoderTestSequenceWithUntagged7Sequence2::accessAttribute(ACCESSOR& accessor, int id) const
{
    (void)accessor;
    enum { NOT_FOUND = -1 };

    switch (id) {
      default:
        return NOT_FOUND;
    }
}

template <class ACCESSOR>
int EncoderTestSequenceWithUntagged7Sequence2::accessAttribute(
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

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& hashAlg, const baljsn::EncoderTestSequenceWithUntagged7Sequence2& object)
{
    (void)hashAlg;
    (void)object;
    using bslh::hashAppend;
}



               // ----------------------------------------------
               // class EncoderTestSequenceWithUntagged8Sequence
               // ----------------------------------------------

// CLASS METHODS
// MANIPULATORS
template <class MANIPULATOR>
int EncoderTestSequenceWithUntagged8Sequence::manipulateAttributes(MANIPULATOR& manipulator)
{
    (void)manipulator;
    int ret = 0;

    return ret;
}

template <class MANIPULATOR>
int EncoderTestSequenceWithUntagged8Sequence::manipulateAttribute(MANIPULATOR& manipulator, int id)
{
    (void)manipulator;
    enum { NOT_FOUND = -1 };

    switch (id) {
      default:
        return NOT_FOUND;
    }
}

template <class MANIPULATOR>
int EncoderTestSequenceWithUntagged8Sequence::manipulateAttribute(
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

// ACCESSORS
template <class ACCESSOR>
int EncoderTestSequenceWithUntagged8Sequence::accessAttributes(ACCESSOR& accessor) const
{
    (void)accessor;
    int ret = 0;

    return ret;
}

template <class ACCESSOR>
int EncoderTestSequenceWithUntagged8Sequence::accessAttribute(ACCESSOR& accessor, int id) const
{
    (void)accessor;
    enum { NOT_FOUND = -1 };

    switch (id) {
      default:
        return NOT_FOUND;
    }
}

template <class ACCESSOR>
int EncoderTestSequenceWithUntagged8Sequence::accessAttribute(
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

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& hashAlg, const baljsn::EncoderTestSequenceWithUntagged8Sequence& object)
{
    (void)hashAlg;
    (void)object;
    using bslh::hashAppend;
}



              // -----------------------------------------------
              // class EncoderTestSequenceWithUntagged8Sequence1
              // -----------------------------------------------

// CLASS METHODS
// MANIPULATORS
template <class MANIPULATOR>
int EncoderTestSequenceWithUntagged8Sequence1::manipulateAttributes(MANIPULATOR& manipulator)
{
    (void)manipulator;
    int ret = 0;

    return ret;
}

template <class MANIPULATOR>
int EncoderTestSequenceWithUntagged8Sequence1::manipulateAttribute(MANIPULATOR& manipulator, int id)
{
    (void)manipulator;
    enum { NOT_FOUND = -1 };

    switch (id) {
      default:
        return NOT_FOUND;
    }
}

template <class MANIPULATOR>
int EncoderTestSequenceWithUntagged8Sequence1::manipulateAttribute(
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

// ACCESSORS
template <class ACCESSOR>
int EncoderTestSequenceWithUntagged8Sequence1::accessAttributes(ACCESSOR& accessor) const
{
    (void)accessor;
    int ret = 0;

    return ret;
}

template <class ACCESSOR>
int EncoderTestSequenceWithUntagged8Sequence1::accessAttribute(ACCESSOR& accessor, int id) const
{
    (void)accessor;
    enum { NOT_FOUND = -1 };

    switch (id) {
      default:
        return NOT_FOUND;
    }
}

template <class ACCESSOR>
int EncoderTestSequenceWithUntagged8Sequence1::accessAttribute(
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

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& hashAlg, const baljsn::EncoderTestSequenceWithUntagged8Sequence1& object)
{
    (void)hashAlg;
    (void)object;
    using bslh::hashAppend;
}



               // ----------------------------------------------
               // class EncoderTestSequenceWithUntagged9Sequence
               // ----------------------------------------------

// CLASS METHODS
// MANIPULATORS
template <class MANIPULATOR>
int EncoderTestSequenceWithUntagged9Sequence::manipulateAttributes(MANIPULATOR& manipulator)
{
    (void)manipulator;
    int ret = 0;

    return ret;
}

template <class MANIPULATOR>
int EncoderTestSequenceWithUntagged9Sequence::manipulateAttribute(MANIPULATOR& manipulator, int id)
{
    (void)manipulator;
    enum { NOT_FOUND = -1 };

    switch (id) {
      default:
        return NOT_FOUND;
    }
}

template <class MANIPULATOR>
int EncoderTestSequenceWithUntagged9Sequence::manipulateAttribute(
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

// ACCESSORS
template <class ACCESSOR>
int EncoderTestSequenceWithUntagged9Sequence::accessAttributes(ACCESSOR& accessor) const
{
    (void)accessor;
    int ret = 0;

    return ret;
}

template <class ACCESSOR>
int EncoderTestSequenceWithUntagged9Sequence::accessAttribute(ACCESSOR& accessor, int id) const
{
    (void)accessor;
    enum { NOT_FOUND = -1 };

    switch (id) {
      default:
        return NOT_FOUND;
    }
}

template <class ACCESSOR>
int EncoderTestSequenceWithUntagged9Sequence::accessAttribute(
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

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& hashAlg, const baljsn::EncoderTestSequenceWithUntagged9Sequence& object)
{
    (void)hashAlg;
    (void)object;
    using bslh::hashAppend;
}



              // -----------------------------------------------
              // class EncoderTestSequenceWithUntagged9Sequence1
              // -----------------------------------------------

// CLASS METHODS
// MANIPULATORS
template <class MANIPULATOR>
int EncoderTestSequenceWithUntagged9Sequence1::manipulateAttributes(MANIPULATOR& manipulator)
{
    (void)manipulator;
    int ret = 0;

    return ret;
}

template <class MANIPULATOR>
int EncoderTestSequenceWithUntagged9Sequence1::manipulateAttribute(MANIPULATOR& manipulator, int id)
{
    (void)manipulator;
    enum { NOT_FOUND = -1 };

    switch (id) {
      default:
        return NOT_FOUND;
    }
}

template <class MANIPULATOR>
int EncoderTestSequenceWithUntagged9Sequence1::manipulateAttribute(
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

// ACCESSORS
template <class ACCESSOR>
int EncoderTestSequenceWithUntagged9Sequence1::accessAttributes(ACCESSOR& accessor) const
{
    (void)accessor;
    int ret = 0;

    return ret;
}

template <class ACCESSOR>
int EncoderTestSequenceWithUntagged9Sequence1::accessAttribute(ACCESSOR& accessor, int id) const
{
    (void)accessor;
    enum { NOT_FOUND = -1 };

    switch (id) {
      default:
        return NOT_FOUND;
    }
}

template <class ACCESSOR>
int EncoderTestSequenceWithUntagged9Sequence1::accessAttribute(
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

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& hashAlg, const baljsn::EncoderTestSequenceWithUntagged9Sequence1& object)
{
    (void)hashAlg;
    (void)object;
    using bslh::hashAppend;
}



                  // ----------------------------------------
                  // class EncoderTestChoiceWithAllCategories
                  // ----------------------------------------

// CLASS METHODS
// CREATORS
inline
EncoderTestChoiceWithAllCategories::EncoderTestChoiceWithAllCategories(bslma::Allocator *basicAllocator)
: d_selectionId(SELECTION_ID_UNDEFINED)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

inline
EncoderTestChoiceWithAllCategories::~EncoderTestChoiceWithAllCategories()
{
    reset();
}

// MANIPULATORS
template <class MANIPULATOR>
int EncoderTestChoiceWithAllCategories::manipulateSelection(MANIPULATOR& manipulator)
{
    switch (d_selectionId) {
      case EncoderTestChoiceWithAllCategories::SELECTION_ID_CHAR_ARRAY:
        return manipulator(&d_charArray.object(),
                SELECTION_INFO_ARRAY[SELECTION_INDEX_CHAR_ARRAY]);
      case EncoderTestChoiceWithAllCategories::SELECTION_ID_CHOICE:
        return manipulator(&d_choice.object(),
                SELECTION_INFO_ARRAY[SELECTION_INDEX_CHOICE]);
      case EncoderTestChoiceWithAllCategories::SELECTION_ID_CUSTOMIZED_TYPE:
        return manipulator(&d_customizedType.object(),
                SELECTION_INFO_ARRAY[SELECTION_INDEX_CUSTOMIZED_TYPE]);
      case EncoderTestChoiceWithAllCategories::SELECTION_ID_ENUMERATION:
        return manipulator(&d_enumeration.object(),
                SELECTION_INFO_ARRAY[SELECTION_INDEX_ENUMERATION]);
      case EncoderTestChoiceWithAllCategories::SELECTION_ID_SEQUENCE:
        return manipulator(&d_sequence.object(),
                SELECTION_INFO_ARRAY[SELECTION_INDEX_SEQUENCE]);
      case EncoderTestChoiceWithAllCategories::SELECTION_ID_SIMPLE:
        return manipulator(&d_simple.object(),
                SELECTION_INFO_ARRAY[SELECTION_INDEX_SIMPLE]);
      default:
        BSLS_ASSERT(EncoderTestChoiceWithAllCategories::SELECTION_ID_UNDEFINED == d_selectionId);
        return -1;
    }
}

inline
bsl::vector<char>& EncoderTestChoiceWithAllCategories::charArray()
{
    BSLS_ASSERT(SELECTION_ID_CHAR_ARRAY == d_selectionId);
    return d_charArray.object();
}

inline
EncoderTestChoiceWithAllCategoriesChoice& EncoderTestChoiceWithAllCategories::choice()
{
    BSLS_ASSERT(SELECTION_ID_CHOICE == d_selectionId);
    return d_choice.object();
}

inline
EncoderTestChoiceWithAllCategoriesCustomizedType& EncoderTestChoiceWithAllCategories::customizedType()
{
    BSLS_ASSERT(SELECTION_ID_CUSTOMIZED_TYPE == d_selectionId);
    return d_customizedType.object();
}

inline
EncoderTestChoiceWithAllCategoriesEnumeration::Value& EncoderTestChoiceWithAllCategories::enumeration()
{
    BSLS_ASSERT(SELECTION_ID_ENUMERATION == d_selectionId);
    return d_enumeration.object();
}

inline
EncoderTestChoiceWithAllCategoriesSequence& EncoderTestChoiceWithAllCategories::sequence()
{
    BSLS_ASSERT(SELECTION_ID_SEQUENCE == d_selectionId);
    return d_sequence.object();
}

inline
int& EncoderTestChoiceWithAllCategories::simple()
{
    BSLS_ASSERT(SELECTION_ID_SIMPLE == d_selectionId);
    return d_simple.object();
}

// ACCESSORS
inline
int EncoderTestChoiceWithAllCategories::selectionId() const
{
    return d_selectionId;
}

template <class ACCESSOR>
int EncoderTestChoiceWithAllCategories::accessSelection(ACCESSOR& accessor) const
{
    switch (d_selectionId) {
      case SELECTION_ID_CHAR_ARRAY:
        return accessor(d_charArray.object(),
                SELECTION_INFO_ARRAY[SELECTION_INDEX_CHAR_ARRAY]);
      case SELECTION_ID_CHOICE:
        return accessor(d_choice.object(),
                SELECTION_INFO_ARRAY[SELECTION_INDEX_CHOICE]);
      case SELECTION_ID_CUSTOMIZED_TYPE:
        return accessor(d_customizedType.object(),
                SELECTION_INFO_ARRAY[SELECTION_INDEX_CUSTOMIZED_TYPE]);
      case SELECTION_ID_ENUMERATION:
        return accessor(d_enumeration.object(),
                SELECTION_INFO_ARRAY[SELECTION_INDEX_ENUMERATION]);
      case SELECTION_ID_SEQUENCE:
        return accessor(d_sequence.object(),
                SELECTION_INFO_ARRAY[SELECTION_INDEX_SEQUENCE]);
      case SELECTION_ID_SIMPLE:
        return accessor(d_simple.object(),
                SELECTION_INFO_ARRAY[SELECTION_INDEX_SIMPLE]);
      default:
        BSLS_ASSERT(SELECTION_ID_UNDEFINED == d_selectionId);
        return -1;
    }
}

inline
const bsl::vector<char>& EncoderTestChoiceWithAllCategories::charArray() const
{
    BSLS_ASSERT(SELECTION_ID_CHAR_ARRAY == d_selectionId);
    return d_charArray.object();
}

inline
const EncoderTestChoiceWithAllCategoriesChoice& EncoderTestChoiceWithAllCategories::choice() const
{
    BSLS_ASSERT(SELECTION_ID_CHOICE == d_selectionId);
    return d_choice.object();
}

inline
const EncoderTestChoiceWithAllCategoriesCustomizedType& EncoderTestChoiceWithAllCategories::customizedType() const
{
    BSLS_ASSERT(SELECTION_ID_CUSTOMIZED_TYPE == d_selectionId);
    return d_customizedType.object();
}

inline
const EncoderTestChoiceWithAllCategoriesEnumeration::Value& EncoderTestChoiceWithAllCategories::enumeration() const
{
    BSLS_ASSERT(SELECTION_ID_ENUMERATION == d_selectionId);
    return d_enumeration.object();
}

inline
const EncoderTestChoiceWithAllCategoriesSequence& EncoderTestChoiceWithAllCategories::sequence() const
{
    BSLS_ASSERT(SELECTION_ID_SEQUENCE == d_selectionId);
    return d_sequence.object();
}

inline
const int& EncoderTestChoiceWithAllCategories::simple() const
{
    BSLS_ASSERT(SELECTION_ID_SIMPLE == d_selectionId);
    return d_simple.object();
}

inline
bool EncoderTestChoiceWithAllCategories::isCharArrayValue() const
{
    return SELECTION_ID_CHAR_ARRAY == d_selectionId;
}

inline
bool EncoderTestChoiceWithAllCategories::isChoiceValue() const
{
    return SELECTION_ID_CHOICE == d_selectionId;
}

inline
bool EncoderTestChoiceWithAllCategories::isCustomizedTypeValue() const
{
    return SELECTION_ID_CUSTOMIZED_TYPE == d_selectionId;
}

inline
bool EncoderTestChoiceWithAllCategories::isEnumerationValue() const
{
    return SELECTION_ID_ENUMERATION == d_selectionId;
}

inline
bool EncoderTestChoiceWithAllCategories::isSequenceValue() const
{
    return SELECTION_ID_SEQUENCE == d_selectionId;
}

inline
bool EncoderTestChoiceWithAllCategories::isSimpleValue() const
{
    return SELECTION_ID_SIMPLE == d_selectionId;
}

inline
bool EncoderTestChoiceWithAllCategories::isUndefinedValue() const
{
    return SELECTION_ID_UNDEFINED == d_selectionId;
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& hashAlg, const baljsn::EncoderTestChoiceWithAllCategories& object)
{
    typedef baljsn::EncoderTestChoiceWithAllCategories Class;
    using bslh::hashAppend;
    hashAppend(hashAlg, object.selectionId());
    switch (object.selectionId()) {
      case Class::SELECTION_ID_CHAR_ARRAY:
        hashAppend(hashAlg, object.charArray());
        break;
      case Class::SELECTION_ID_CHOICE:
        hashAppend(hashAlg, object.choice());
        break;
      case Class::SELECTION_ID_CUSTOMIZED_TYPE:
        hashAppend(hashAlg, object.customizedType());
        break;
      case Class::SELECTION_ID_ENUMERATION:
        hashAppend(hashAlg, object.enumeration());
        break;
      case Class::SELECTION_ID_SEQUENCE:
        hashAppend(hashAlg, object.sequence());
        break;
      case Class::SELECTION_ID_SIMPLE:
        hashAppend(hashAlg, object.simple());
        break;
      default:
        BSLS_ASSERT(Class::SELECTION_ID_UNDEFINED == object.selectionId());
    }
}


                     // ----------------------------------
                     // class EncoderTestDegenerateChoice1
                     // ----------------------------------

// CLASS METHODS
// CREATORS
inline
EncoderTestDegenerateChoice1::EncoderTestDegenerateChoice1()
: d_selectionId(SELECTION_ID_UNDEFINED)
{
}

inline
EncoderTestDegenerateChoice1::~EncoderTestDegenerateChoice1()
{
    reset();
}

// MANIPULATORS
template <class MANIPULATOR>
int EncoderTestDegenerateChoice1::manipulateSelection(MANIPULATOR& manipulator)
{
    switch (d_selectionId) {
      case EncoderTestDegenerateChoice1::SELECTION_ID_SEQUENCE:
        return manipulator(&d_sequence.object(),
                SELECTION_INFO_ARRAY[SELECTION_INDEX_SEQUENCE]);
      default:
        BSLS_ASSERT(EncoderTestDegenerateChoice1::SELECTION_ID_UNDEFINED == d_selectionId);
        return -1;
    }
}

inline
EncoderTestDegenerateChoice1Sequence& EncoderTestDegenerateChoice1::sequence()
{
    BSLS_ASSERT(SELECTION_ID_SEQUENCE == d_selectionId);
    return d_sequence.object();
}

// ACCESSORS
inline
int EncoderTestDegenerateChoice1::selectionId() const
{
    return d_selectionId;
}

template <class ACCESSOR>
int EncoderTestDegenerateChoice1::accessSelection(ACCESSOR& accessor) const
{
    switch (d_selectionId) {
      case SELECTION_ID_SEQUENCE:
        return accessor(d_sequence.object(),
                SELECTION_INFO_ARRAY[SELECTION_INDEX_SEQUENCE]);
      default:
        BSLS_ASSERT(SELECTION_ID_UNDEFINED == d_selectionId);
        return -1;
    }
}

inline
const EncoderTestDegenerateChoice1Sequence& EncoderTestDegenerateChoice1::sequence() const
{
    BSLS_ASSERT(SELECTION_ID_SEQUENCE == d_selectionId);
    return d_sequence.object();
}

inline
bool EncoderTestDegenerateChoice1::isSequenceValue() const
{
    return SELECTION_ID_SEQUENCE == d_selectionId;
}

inline
bool EncoderTestDegenerateChoice1::isUndefinedValue() const
{
    return SELECTION_ID_UNDEFINED == d_selectionId;
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& hashAlg, const baljsn::EncoderTestDegenerateChoice1& object)
{
    typedef baljsn::EncoderTestDegenerateChoice1 Class;
    using bslh::hashAppend;
    hashAppend(hashAlg, object.selectionId());
    switch (object.selectionId()) {
      case Class::SELECTION_ID_SEQUENCE:
        hashAppend(hashAlg, object.sequence());
        break;
      default:
        BSLS_ASSERT(Class::SELECTION_ID_UNDEFINED == object.selectionId());
    }
}


                         // -------------------------
                         // class EncoderTestEmployee
                         // -------------------------

// CLASS METHODS
// MANIPULATORS
template <class MANIPULATOR>
int EncoderTestEmployee::manipulateAttributes(MANIPULATOR& manipulator)
{
    int ret;

    ret = manipulator(&d_name, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_NAME]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_homeAddress, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_HOME_ADDRESS]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_age, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_AGE]);
    if (ret) {
        return ret;
    }

    return ret;
}

template <class MANIPULATOR>
int EncoderTestEmployee::manipulateAttribute(MANIPULATOR& manipulator, int id)
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_NAME: {
        return manipulator(&d_name, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_NAME]);
      }
      case ATTRIBUTE_ID_HOME_ADDRESS: {
        return manipulator(&d_homeAddress, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_HOME_ADDRESS]);
      }
      case ATTRIBUTE_ID_AGE: {
        return manipulator(&d_age, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_AGE]);
      }
      default:
        return NOT_FOUND;
    }
}

template <class MANIPULATOR>
int EncoderTestEmployee::manipulateAttribute(
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
bsl::string& EncoderTestEmployee::name()
{
    return d_name;
}

inline
EncoderTestAddress& EncoderTestEmployee::homeAddress()
{
    return d_homeAddress;
}

inline
int& EncoderTestEmployee::age()
{
    return d_age;
}

// ACCESSORS
template <class ACCESSOR>
int EncoderTestEmployee::accessAttributes(ACCESSOR& accessor) const
{
    int ret;

    ret = accessor(d_name, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_NAME]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_homeAddress, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_HOME_ADDRESS]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_age, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_AGE]);
    if (ret) {
        return ret;
    }

    return ret;
}

template <class ACCESSOR>
int EncoderTestEmployee::accessAttribute(ACCESSOR& accessor, int id) const
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_NAME: {
        return accessor(d_name, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_NAME]);
      }
      case ATTRIBUTE_ID_HOME_ADDRESS: {
        return accessor(d_homeAddress, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_HOME_ADDRESS]);
      }
      case ATTRIBUTE_ID_AGE: {
        return accessor(d_age, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_AGE]);
      }
      default:
        return NOT_FOUND;
    }
}

template <class ACCESSOR>
int EncoderTestEmployee::accessAttribute(
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
const bsl::string& EncoderTestEmployee::name() const
{
    return d_name;
}

inline
const EncoderTestAddress& EncoderTestEmployee::homeAddress() const
{
    return d_homeAddress;
}

inline
int EncoderTestEmployee::age() const
{
    return d_age;
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& hashAlg, const baljsn::EncoderTestEmployee& object)
{
    (void)hashAlg;
    (void)object;
    using bslh::hashAppend;
    hashAppend(hashAlg, object.name());
    hashAppend(hashAlg, object.homeAddress());
    hashAppend(hashAlg, object.age());
}



                 // ------------------------------------------
                 // class EncoderTestSequenceWithAllCategories
                 // ------------------------------------------

// CLASS METHODS
// MANIPULATORS
template <class MANIPULATOR>
int EncoderTestSequenceWithAllCategories::manipulateAttributes(MANIPULATOR& manipulator)
{
    int ret;

    ret = manipulator(&d_charArray, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CHAR_ARRAY]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_array, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ARRAY]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_choice, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CHOICE]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_customizedType, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CUSTOMIZED_TYPE]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_enumeration, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ENUMERATION]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_nullableValue, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_NULLABLE_VALUE]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_sequence, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SEQUENCE]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_simple, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SIMPLE]);
    if (ret) {
        return ret;
    }

    return ret;
}

template <class MANIPULATOR>
int EncoderTestSequenceWithAllCategories::manipulateAttribute(MANIPULATOR& manipulator, int id)
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_CHAR_ARRAY: {
        return manipulator(&d_charArray, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CHAR_ARRAY]);
      }
      case ATTRIBUTE_ID_ARRAY: {
        return manipulator(&d_array, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ARRAY]);
      }
      case ATTRIBUTE_ID_CHOICE: {
        return manipulator(&d_choice, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CHOICE]);
      }
      case ATTRIBUTE_ID_CUSTOMIZED_TYPE: {
        return manipulator(&d_customizedType, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CUSTOMIZED_TYPE]);
      }
      case ATTRIBUTE_ID_ENUMERATION: {
        return manipulator(&d_enumeration, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ENUMERATION]);
      }
      case ATTRIBUTE_ID_NULLABLE_VALUE: {
        return manipulator(&d_nullableValue, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_NULLABLE_VALUE]);
      }
      case ATTRIBUTE_ID_SEQUENCE: {
        return manipulator(&d_sequence, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SEQUENCE]);
      }
      case ATTRIBUTE_ID_SIMPLE: {
        return manipulator(&d_simple, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SIMPLE]);
      }
      default:
        return NOT_FOUND;
    }
}

template <class MANIPULATOR>
int EncoderTestSequenceWithAllCategories::manipulateAttribute(
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
bsl::vector<char>& EncoderTestSequenceWithAllCategories::charArray()
{
    return d_charArray;
}

inline
bsl::vector<int>& EncoderTestSequenceWithAllCategories::array()
{
    return d_array;
}

inline
EncoderTestSequenceWithAllCategoriesChoice& EncoderTestSequenceWithAllCategories::choice()
{
    return d_choice;
}

inline
EncoderTestSequenceWithAllCategoriesCustomizedType& EncoderTestSequenceWithAllCategories::customizedType()
{
    return d_customizedType;
}

inline
EncoderTestSequenceWithAllCategoriesEnumeration::Value& EncoderTestSequenceWithAllCategories::enumeration()
{
    return d_enumeration;
}

inline
bdlb::NullableValue<int>& EncoderTestSequenceWithAllCategories::nullableValue()
{
    return d_nullableValue;
}

inline
EncoderTestSequenceWithAllCategoriesSequence& EncoderTestSequenceWithAllCategories::sequence()
{
    return d_sequence;
}

inline
int& EncoderTestSequenceWithAllCategories::simple()
{
    return d_simple;
}

// ACCESSORS
template <class ACCESSOR>
int EncoderTestSequenceWithAllCategories::accessAttributes(ACCESSOR& accessor) const
{
    int ret;

    ret = accessor(d_charArray, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CHAR_ARRAY]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_array, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ARRAY]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_choice, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CHOICE]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_customizedType, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CUSTOMIZED_TYPE]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_enumeration, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ENUMERATION]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_nullableValue, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_NULLABLE_VALUE]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_sequence, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SEQUENCE]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_simple, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SIMPLE]);
    if (ret) {
        return ret;
    }

    return ret;
}

template <class ACCESSOR>
int EncoderTestSequenceWithAllCategories::accessAttribute(ACCESSOR& accessor, int id) const
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_CHAR_ARRAY: {
        return accessor(d_charArray, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CHAR_ARRAY]);
      }
      case ATTRIBUTE_ID_ARRAY: {
        return accessor(d_array, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ARRAY]);
      }
      case ATTRIBUTE_ID_CHOICE: {
        return accessor(d_choice, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CHOICE]);
      }
      case ATTRIBUTE_ID_CUSTOMIZED_TYPE: {
        return accessor(d_customizedType, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CUSTOMIZED_TYPE]);
      }
      case ATTRIBUTE_ID_ENUMERATION: {
        return accessor(d_enumeration, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ENUMERATION]);
      }
      case ATTRIBUTE_ID_NULLABLE_VALUE: {
        return accessor(d_nullableValue, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_NULLABLE_VALUE]);
      }
      case ATTRIBUTE_ID_SEQUENCE: {
        return accessor(d_sequence, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SEQUENCE]);
      }
      case ATTRIBUTE_ID_SIMPLE: {
        return accessor(d_simple, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SIMPLE]);
      }
      default:
        return NOT_FOUND;
    }
}

template <class ACCESSOR>
int EncoderTestSequenceWithAllCategories::accessAttribute(
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
const bsl::vector<char>& EncoderTestSequenceWithAllCategories::charArray() const
{
    return d_charArray;
}

inline
const bsl::vector<int>& EncoderTestSequenceWithAllCategories::array() const
{
    return d_array;
}

inline
const EncoderTestSequenceWithAllCategoriesChoice& EncoderTestSequenceWithAllCategories::choice() const
{
    return d_choice;
}

inline
const EncoderTestSequenceWithAllCategoriesCustomizedType& EncoderTestSequenceWithAllCategories::customizedType() const
{
    return d_customizedType;
}

inline
EncoderTestSequenceWithAllCategoriesEnumeration::Value EncoderTestSequenceWithAllCategories::enumeration() const
{
    return d_enumeration;
}

inline
const bdlb::NullableValue<int>& EncoderTestSequenceWithAllCategories::nullableValue() const
{
    return d_nullableValue;
}

inline
const EncoderTestSequenceWithAllCategoriesSequence& EncoderTestSequenceWithAllCategories::sequence() const
{
    return d_sequence;
}

inline
int EncoderTestSequenceWithAllCategories::simple() const
{
    return d_simple;
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& hashAlg, const baljsn::EncoderTestSequenceWithAllCategories& object)
{
    (void)hashAlg;
    (void)object;
    using bslh::hashAppend;
    hashAppend(hashAlg, object.charArray());
    hashAppend(hashAlg, object.array());
    hashAppend(hashAlg, object.choice());
    hashAppend(hashAlg, object.customizedType());
    hashAppend(hashAlg, object.enumeration());
    hashAppend(hashAlg, object.nullableValue());
    hashAppend(hashAlg, object.sequence());
    hashAppend(hashAlg, object.simple());
}



                   // --------------------------------------
                   // class EncoderTestSequenceWithUntagged1
                   // --------------------------------------

// CLASS METHODS
// MANIPULATORS
template <class MANIPULATOR>
int EncoderTestSequenceWithUntagged1::manipulateAttributes(MANIPULATOR& manipulator)
{
    int ret;

    ret = manipulator(&d_sequence, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SEQUENCE]);
    if (ret) {
        return ret;
    }

    return ret;
}

template <class MANIPULATOR>
int EncoderTestSequenceWithUntagged1::manipulateAttribute(MANIPULATOR& manipulator, int id)
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_SEQUENCE: {
        return manipulator(&d_sequence, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SEQUENCE]);
      }
      default:
        return NOT_FOUND;
    }
}

template <class MANIPULATOR>
int EncoderTestSequenceWithUntagged1::manipulateAttribute(
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
EncoderTestSequenceWithUntagged1Sequence& EncoderTestSequenceWithUntagged1::sequence()
{
    return d_sequence;
}

// ACCESSORS
template <class ACCESSOR>
int EncoderTestSequenceWithUntagged1::accessAttributes(ACCESSOR& accessor) const
{
    int ret;

    ret = accessor(d_sequence, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SEQUENCE]);
    if (ret) {
        return ret;
    }

    return ret;
}

template <class ACCESSOR>
int EncoderTestSequenceWithUntagged1::accessAttribute(ACCESSOR& accessor, int id) const
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_SEQUENCE: {
        return accessor(d_sequence, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SEQUENCE]);
      }
      default:
        return NOT_FOUND;
    }
}

template <class ACCESSOR>
int EncoderTestSequenceWithUntagged1::accessAttribute(
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
const EncoderTestSequenceWithUntagged1Sequence& EncoderTestSequenceWithUntagged1::sequence() const
{
    return d_sequence;
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& hashAlg, const baljsn::EncoderTestSequenceWithUntagged1& object)
{
    (void)hashAlg;
    (void)object;
    using bslh::hashAppend;
    hashAppend(hashAlg, object.sequence());
}



                  // ---------------------------------------
                  // class EncoderTestSequenceWithUntagged10
                  // ---------------------------------------

// CLASS METHODS
// MANIPULATORS
template <class MANIPULATOR>
int EncoderTestSequenceWithUntagged10::manipulateAttributes(MANIPULATOR& manipulator)
{
    int ret;

    ret = manipulator(&d_sequence, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SEQUENCE]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_attribute0, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE0]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_attribute1, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE1]);
    if (ret) {
        return ret;
    }

    return ret;
}

template <class MANIPULATOR>
int EncoderTestSequenceWithUntagged10::manipulateAttribute(MANIPULATOR& manipulator, int id)
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_SEQUENCE: {
        return manipulator(&d_sequence, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SEQUENCE]);
      }
      case ATTRIBUTE_ID_ATTRIBUTE0: {
        return manipulator(&d_attribute0, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE0]);
      }
      case ATTRIBUTE_ID_ATTRIBUTE1: {
        return manipulator(&d_attribute1, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE1]);
      }
      default:
        return NOT_FOUND;
    }
}

template <class MANIPULATOR>
int EncoderTestSequenceWithUntagged10::manipulateAttribute(
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
EncoderTestSequenceWithUntagged10Sequence& EncoderTestSequenceWithUntagged10::sequence()
{
    return d_sequence;
}

inline
int& EncoderTestSequenceWithUntagged10::attribute0()
{
    return d_attribute0;
}

inline
int& EncoderTestSequenceWithUntagged10::attribute1()
{
    return d_attribute1;
}

// ACCESSORS
template <class ACCESSOR>
int EncoderTestSequenceWithUntagged10::accessAttributes(ACCESSOR& accessor) const
{
    int ret;

    ret = accessor(d_sequence, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SEQUENCE]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_attribute0, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE0]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_attribute1, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE1]);
    if (ret) {
        return ret;
    }

    return ret;
}

template <class ACCESSOR>
int EncoderTestSequenceWithUntagged10::accessAttribute(ACCESSOR& accessor, int id) const
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_SEQUENCE: {
        return accessor(d_sequence, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SEQUENCE]);
      }
      case ATTRIBUTE_ID_ATTRIBUTE0: {
        return accessor(d_attribute0, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE0]);
      }
      case ATTRIBUTE_ID_ATTRIBUTE1: {
        return accessor(d_attribute1, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE1]);
      }
      default:
        return NOT_FOUND;
    }
}

template <class ACCESSOR>
int EncoderTestSequenceWithUntagged10::accessAttribute(
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
const EncoderTestSequenceWithUntagged10Sequence& EncoderTestSequenceWithUntagged10::sequence() const
{
    return d_sequence;
}

inline
int EncoderTestSequenceWithUntagged10::attribute0() const
{
    return d_attribute0;
}

inline
int EncoderTestSequenceWithUntagged10::attribute1() const
{
    return d_attribute1;
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& hashAlg, const baljsn::EncoderTestSequenceWithUntagged10& object)
{
    (void)hashAlg;
    (void)object;
    using bslh::hashAppend;
    hashAppend(hashAlg, object.sequence());
    hashAppend(hashAlg, object.attribute0());
    hashAppend(hashAlg, object.attribute1());
}



                  // ---------------------------------------
                  // class EncoderTestSequenceWithUntagged11
                  // ---------------------------------------

// CLASS METHODS
// MANIPULATORS
template <class MANIPULATOR>
int EncoderTestSequenceWithUntagged11::manipulateAttributes(MANIPULATOR& manipulator)
{
    int ret;

    ret = manipulator(&d_attribute0, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE0]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_sequence, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SEQUENCE]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_sequence1, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SEQUENCE1]);
    if (ret) {
        return ret;
    }

    return ret;
}

template <class MANIPULATOR>
int EncoderTestSequenceWithUntagged11::manipulateAttribute(MANIPULATOR& manipulator, int id)
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_ATTRIBUTE0: {
        return manipulator(&d_attribute0, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE0]);
      }
      case ATTRIBUTE_ID_SEQUENCE: {
        return manipulator(&d_sequence, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SEQUENCE]);
      }
      case ATTRIBUTE_ID_SEQUENCE1: {
        return manipulator(&d_sequence1, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SEQUENCE1]);
      }
      default:
        return NOT_FOUND;
    }
}

template <class MANIPULATOR>
int EncoderTestSequenceWithUntagged11::manipulateAttribute(
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
int& EncoderTestSequenceWithUntagged11::attribute0()
{
    return d_attribute0;
}

inline
EncoderTestSequenceWithUntagged11Sequence& EncoderTestSequenceWithUntagged11::sequence()
{
    return d_sequence;
}

inline
EncoderTestSequenceWithUntagged11Sequence1& EncoderTestSequenceWithUntagged11::sequence1()
{
    return d_sequence1;
}

// ACCESSORS
template <class ACCESSOR>
int EncoderTestSequenceWithUntagged11::accessAttributes(ACCESSOR& accessor) const
{
    int ret;

    ret = accessor(d_attribute0, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE0]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_sequence, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SEQUENCE]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_sequence1, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SEQUENCE1]);
    if (ret) {
        return ret;
    }

    return ret;
}

template <class ACCESSOR>
int EncoderTestSequenceWithUntagged11::accessAttribute(ACCESSOR& accessor, int id) const
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_ATTRIBUTE0: {
        return accessor(d_attribute0, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE0]);
      }
      case ATTRIBUTE_ID_SEQUENCE: {
        return accessor(d_sequence, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SEQUENCE]);
      }
      case ATTRIBUTE_ID_SEQUENCE1: {
        return accessor(d_sequence1, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SEQUENCE1]);
      }
      default:
        return NOT_FOUND;
    }
}

template <class ACCESSOR>
int EncoderTestSequenceWithUntagged11::accessAttribute(
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
int EncoderTestSequenceWithUntagged11::attribute0() const
{
    return d_attribute0;
}

inline
const EncoderTestSequenceWithUntagged11Sequence& EncoderTestSequenceWithUntagged11::sequence() const
{
    return d_sequence;
}

inline
const EncoderTestSequenceWithUntagged11Sequence1& EncoderTestSequenceWithUntagged11::sequence1() const
{
    return d_sequence1;
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& hashAlg, const baljsn::EncoderTestSequenceWithUntagged11& object)
{
    (void)hashAlg;
    (void)object;
    using bslh::hashAppend;
    hashAppend(hashAlg, object.attribute0());
    hashAppend(hashAlg, object.sequence());
    hashAppend(hashAlg, object.sequence1());
}



                  // ---------------------------------------
                  // class EncoderTestSequenceWithUntagged12
                  // ---------------------------------------

// CLASS METHODS
// MANIPULATORS
template <class MANIPULATOR>
int EncoderTestSequenceWithUntagged12::manipulateAttributes(MANIPULATOR& manipulator)
{
    int ret;

    ret = manipulator(&d_attribute0, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE0]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_sequence, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SEQUENCE]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_attribute1, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE1]);
    if (ret) {
        return ret;
    }

    return ret;
}

template <class MANIPULATOR>
int EncoderTestSequenceWithUntagged12::manipulateAttribute(MANIPULATOR& manipulator, int id)
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_ATTRIBUTE0: {
        return manipulator(&d_attribute0, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE0]);
      }
      case ATTRIBUTE_ID_SEQUENCE: {
        return manipulator(&d_sequence, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SEQUENCE]);
      }
      case ATTRIBUTE_ID_ATTRIBUTE1: {
        return manipulator(&d_attribute1, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE1]);
      }
      default:
        return NOT_FOUND;
    }
}

template <class MANIPULATOR>
int EncoderTestSequenceWithUntagged12::manipulateAttribute(
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
int& EncoderTestSequenceWithUntagged12::attribute0()
{
    return d_attribute0;
}

inline
EncoderTestSequenceWithUntagged12Sequence& EncoderTestSequenceWithUntagged12::sequence()
{
    return d_sequence;
}

inline
int& EncoderTestSequenceWithUntagged12::attribute1()
{
    return d_attribute1;
}

// ACCESSORS
template <class ACCESSOR>
int EncoderTestSequenceWithUntagged12::accessAttributes(ACCESSOR& accessor) const
{
    int ret;

    ret = accessor(d_attribute0, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE0]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_sequence, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SEQUENCE]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_attribute1, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE1]);
    if (ret) {
        return ret;
    }

    return ret;
}

template <class ACCESSOR>
int EncoderTestSequenceWithUntagged12::accessAttribute(ACCESSOR& accessor, int id) const
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_ATTRIBUTE0: {
        return accessor(d_attribute0, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE0]);
      }
      case ATTRIBUTE_ID_SEQUENCE: {
        return accessor(d_sequence, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SEQUENCE]);
      }
      case ATTRIBUTE_ID_ATTRIBUTE1: {
        return accessor(d_attribute1, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE1]);
      }
      default:
        return NOT_FOUND;
    }
}

template <class ACCESSOR>
int EncoderTestSequenceWithUntagged12::accessAttribute(
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
int EncoderTestSequenceWithUntagged12::attribute0() const
{
    return d_attribute0;
}

inline
const EncoderTestSequenceWithUntagged12Sequence& EncoderTestSequenceWithUntagged12::sequence() const
{
    return d_sequence;
}

inline
int EncoderTestSequenceWithUntagged12::attribute1() const
{
    return d_attribute1;
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& hashAlg, const baljsn::EncoderTestSequenceWithUntagged12& object)
{
    (void)hashAlg;
    (void)object;
    using bslh::hashAppend;
    hashAppend(hashAlg, object.attribute0());
    hashAppend(hashAlg, object.sequence());
    hashAppend(hashAlg, object.attribute1());
}



                  // ---------------------------------------
                  // class EncoderTestSequenceWithUntagged13
                  // ---------------------------------------

// CLASS METHODS
// MANIPULATORS
template <class MANIPULATOR>
int EncoderTestSequenceWithUntagged13::manipulateAttributes(MANIPULATOR& manipulator)
{
    int ret;

    ret = manipulator(&d_attribute0, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE0]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_attribute1, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE1]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_sequence, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SEQUENCE]);
    if (ret) {
        return ret;
    }

    return ret;
}

template <class MANIPULATOR>
int EncoderTestSequenceWithUntagged13::manipulateAttribute(MANIPULATOR& manipulator, int id)
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_ATTRIBUTE0: {
        return manipulator(&d_attribute0, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE0]);
      }
      case ATTRIBUTE_ID_ATTRIBUTE1: {
        return manipulator(&d_attribute1, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE1]);
      }
      case ATTRIBUTE_ID_SEQUENCE: {
        return manipulator(&d_sequence, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SEQUENCE]);
      }
      default:
        return NOT_FOUND;
    }
}

template <class MANIPULATOR>
int EncoderTestSequenceWithUntagged13::manipulateAttribute(
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
int& EncoderTestSequenceWithUntagged13::attribute0()
{
    return d_attribute0;
}

inline
int& EncoderTestSequenceWithUntagged13::attribute1()
{
    return d_attribute1;
}

inline
EncoderTestSequenceWithUntagged13Sequence& EncoderTestSequenceWithUntagged13::sequence()
{
    return d_sequence;
}

// ACCESSORS
template <class ACCESSOR>
int EncoderTestSequenceWithUntagged13::accessAttributes(ACCESSOR& accessor) const
{
    int ret;

    ret = accessor(d_attribute0, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE0]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_attribute1, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE1]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_sequence, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SEQUENCE]);
    if (ret) {
        return ret;
    }

    return ret;
}

template <class ACCESSOR>
int EncoderTestSequenceWithUntagged13::accessAttribute(ACCESSOR& accessor, int id) const
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_ATTRIBUTE0: {
        return accessor(d_attribute0, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE0]);
      }
      case ATTRIBUTE_ID_ATTRIBUTE1: {
        return accessor(d_attribute1, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE1]);
      }
      case ATTRIBUTE_ID_SEQUENCE: {
        return accessor(d_sequence, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SEQUENCE]);
      }
      default:
        return NOT_FOUND;
    }
}

template <class ACCESSOR>
int EncoderTestSequenceWithUntagged13::accessAttribute(
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
int EncoderTestSequenceWithUntagged13::attribute0() const
{
    return d_attribute0;
}

inline
int EncoderTestSequenceWithUntagged13::attribute1() const
{
    return d_attribute1;
}

inline
const EncoderTestSequenceWithUntagged13Sequence& EncoderTestSequenceWithUntagged13::sequence() const
{
    return d_sequence;
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& hashAlg, const baljsn::EncoderTestSequenceWithUntagged13& object)
{
    (void)hashAlg;
    (void)object;
    using bslh::hashAppend;
    hashAppend(hashAlg, object.attribute0());
    hashAppend(hashAlg, object.attribute1());
    hashAppend(hashAlg, object.sequence());
}



                   // --------------------------------------
                   // class EncoderTestSequenceWithUntagged3
                   // --------------------------------------

// CLASS METHODS
// MANIPULATORS
template <class MANIPULATOR>
int EncoderTestSequenceWithUntagged3::manipulateAttributes(MANIPULATOR& manipulator)
{
    int ret;

    ret = manipulator(&d_sequence, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SEQUENCE]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_sequence1, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SEQUENCE1]);
    if (ret) {
        return ret;
    }

    return ret;
}

template <class MANIPULATOR>
int EncoderTestSequenceWithUntagged3::manipulateAttribute(MANIPULATOR& manipulator, int id)
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_SEQUENCE: {
        return manipulator(&d_sequence, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SEQUENCE]);
      }
      case ATTRIBUTE_ID_SEQUENCE1: {
        return manipulator(&d_sequence1, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SEQUENCE1]);
      }
      default:
        return NOT_FOUND;
    }
}

template <class MANIPULATOR>
int EncoderTestSequenceWithUntagged3::manipulateAttribute(
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
EncoderTestSequenceWithUntagged3Sequence& EncoderTestSequenceWithUntagged3::sequence()
{
    return d_sequence;
}

inline
EncoderTestSequenceWithUntagged3Sequence1& EncoderTestSequenceWithUntagged3::sequence1()
{
    return d_sequence1;
}

// ACCESSORS
template <class ACCESSOR>
int EncoderTestSequenceWithUntagged3::accessAttributes(ACCESSOR& accessor) const
{
    int ret;

    ret = accessor(d_sequence, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SEQUENCE]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_sequence1, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SEQUENCE1]);
    if (ret) {
        return ret;
    }

    return ret;
}

template <class ACCESSOR>
int EncoderTestSequenceWithUntagged3::accessAttribute(ACCESSOR& accessor, int id) const
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_SEQUENCE: {
        return accessor(d_sequence, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SEQUENCE]);
      }
      case ATTRIBUTE_ID_SEQUENCE1: {
        return accessor(d_sequence1, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SEQUENCE1]);
      }
      default:
        return NOT_FOUND;
    }
}

template <class ACCESSOR>
int EncoderTestSequenceWithUntagged3::accessAttribute(
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
const EncoderTestSequenceWithUntagged3Sequence& EncoderTestSequenceWithUntagged3::sequence() const
{
    return d_sequence;
}

inline
const EncoderTestSequenceWithUntagged3Sequence1& EncoderTestSequenceWithUntagged3::sequence1() const
{
    return d_sequence1;
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& hashAlg, const baljsn::EncoderTestSequenceWithUntagged3& object)
{
    (void)hashAlg;
    (void)object;
    using bslh::hashAppend;
    hashAppend(hashAlg, object.sequence());
    hashAppend(hashAlg, object.sequence1());
}



                   // --------------------------------------
                   // class EncoderTestSequenceWithUntagged4
                   // --------------------------------------

// CLASS METHODS
// MANIPULATORS
template <class MANIPULATOR>
int EncoderTestSequenceWithUntagged4::manipulateAttributes(MANIPULATOR& manipulator)
{
    int ret;

    ret = manipulator(&d_sequence, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SEQUENCE]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_attribute0, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE0]);
    if (ret) {
        return ret;
    }

    return ret;
}

template <class MANIPULATOR>
int EncoderTestSequenceWithUntagged4::manipulateAttribute(MANIPULATOR& manipulator, int id)
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_SEQUENCE: {
        return manipulator(&d_sequence, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SEQUENCE]);
      }
      case ATTRIBUTE_ID_ATTRIBUTE0: {
        return manipulator(&d_attribute0, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE0]);
      }
      default:
        return NOT_FOUND;
    }
}

template <class MANIPULATOR>
int EncoderTestSequenceWithUntagged4::manipulateAttribute(
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
EncoderTestSequenceWithUntagged4Sequence& EncoderTestSequenceWithUntagged4::sequence()
{
    return d_sequence;
}

inline
int& EncoderTestSequenceWithUntagged4::attribute0()
{
    return d_attribute0;
}

// ACCESSORS
template <class ACCESSOR>
int EncoderTestSequenceWithUntagged4::accessAttributes(ACCESSOR& accessor) const
{
    int ret;

    ret = accessor(d_sequence, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SEQUENCE]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_attribute0, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE0]);
    if (ret) {
        return ret;
    }

    return ret;
}

template <class ACCESSOR>
int EncoderTestSequenceWithUntagged4::accessAttribute(ACCESSOR& accessor, int id) const
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_SEQUENCE: {
        return accessor(d_sequence, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SEQUENCE]);
      }
      case ATTRIBUTE_ID_ATTRIBUTE0: {
        return accessor(d_attribute0, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE0]);
      }
      default:
        return NOT_FOUND;
    }
}

template <class ACCESSOR>
int EncoderTestSequenceWithUntagged4::accessAttribute(
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
const EncoderTestSequenceWithUntagged4Sequence& EncoderTestSequenceWithUntagged4::sequence() const
{
    return d_sequence;
}

inline
int EncoderTestSequenceWithUntagged4::attribute0() const
{
    return d_attribute0;
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& hashAlg, const baljsn::EncoderTestSequenceWithUntagged4& object)
{
    (void)hashAlg;
    (void)object;
    using bslh::hashAppend;
    hashAppend(hashAlg, object.sequence());
    hashAppend(hashAlg, object.attribute0());
}



                   // --------------------------------------
                   // class EncoderTestSequenceWithUntagged5
                   // --------------------------------------

// CLASS METHODS
// MANIPULATORS
template <class MANIPULATOR>
int EncoderTestSequenceWithUntagged5::manipulateAttributes(MANIPULATOR& manipulator)
{
    int ret;

    ret = manipulator(&d_attribute0, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE0]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_sequence, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SEQUENCE]);
    if (ret) {
        return ret;
    }

    return ret;
}

template <class MANIPULATOR>
int EncoderTestSequenceWithUntagged5::manipulateAttribute(MANIPULATOR& manipulator, int id)
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_ATTRIBUTE0: {
        return manipulator(&d_attribute0, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE0]);
      }
      case ATTRIBUTE_ID_SEQUENCE: {
        return manipulator(&d_sequence, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SEQUENCE]);
      }
      default:
        return NOT_FOUND;
    }
}

template <class MANIPULATOR>
int EncoderTestSequenceWithUntagged5::manipulateAttribute(
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
int& EncoderTestSequenceWithUntagged5::attribute0()
{
    return d_attribute0;
}

inline
EncoderTestSequenceWithUntagged5Sequence& EncoderTestSequenceWithUntagged5::sequence()
{
    return d_sequence;
}

// ACCESSORS
template <class ACCESSOR>
int EncoderTestSequenceWithUntagged5::accessAttributes(ACCESSOR& accessor) const
{
    int ret;

    ret = accessor(d_attribute0, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE0]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_sequence, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SEQUENCE]);
    if (ret) {
        return ret;
    }

    return ret;
}

template <class ACCESSOR>
int EncoderTestSequenceWithUntagged5::accessAttribute(ACCESSOR& accessor, int id) const
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_ATTRIBUTE0: {
        return accessor(d_attribute0, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE0]);
      }
      case ATTRIBUTE_ID_SEQUENCE: {
        return accessor(d_sequence, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SEQUENCE]);
      }
      default:
        return NOT_FOUND;
    }
}

template <class ACCESSOR>
int EncoderTestSequenceWithUntagged5::accessAttribute(
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
int EncoderTestSequenceWithUntagged5::attribute0() const
{
    return d_attribute0;
}

inline
const EncoderTestSequenceWithUntagged5Sequence& EncoderTestSequenceWithUntagged5::sequence() const
{
    return d_sequence;
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& hashAlg, const baljsn::EncoderTestSequenceWithUntagged5& object)
{
    (void)hashAlg;
    (void)object;
    using bslh::hashAppend;
    hashAppend(hashAlg, object.attribute0());
    hashAppend(hashAlg, object.sequence());
}



                   // --------------------------------------
                   // class EncoderTestSequenceWithUntagged7
                   // --------------------------------------

// CLASS METHODS
// MANIPULATORS
template <class MANIPULATOR>
int EncoderTestSequenceWithUntagged7::manipulateAttributes(MANIPULATOR& manipulator)
{
    int ret;

    ret = manipulator(&d_sequence, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SEQUENCE]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_sequence1, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SEQUENCE1]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_sequence2, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SEQUENCE2]);
    if (ret) {
        return ret;
    }

    return ret;
}

template <class MANIPULATOR>
int EncoderTestSequenceWithUntagged7::manipulateAttribute(MANIPULATOR& manipulator, int id)
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_SEQUENCE: {
        return manipulator(&d_sequence, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SEQUENCE]);
      }
      case ATTRIBUTE_ID_SEQUENCE1: {
        return manipulator(&d_sequence1, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SEQUENCE1]);
      }
      case ATTRIBUTE_ID_SEQUENCE2: {
        return manipulator(&d_sequence2, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SEQUENCE2]);
      }
      default:
        return NOT_FOUND;
    }
}

template <class MANIPULATOR>
int EncoderTestSequenceWithUntagged7::manipulateAttribute(
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
EncoderTestSequenceWithUntagged7Sequence& EncoderTestSequenceWithUntagged7::sequence()
{
    return d_sequence;
}

inline
EncoderTestSequenceWithUntagged7Sequence1& EncoderTestSequenceWithUntagged7::sequence1()
{
    return d_sequence1;
}

inline
EncoderTestSequenceWithUntagged7Sequence2& EncoderTestSequenceWithUntagged7::sequence2()
{
    return d_sequence2;
}

// ACCESSORS
template <class ACCESSOR>
int EncoderTestSequenceWithUntagged7::accessAttributes(ACCESSOR& accessor) const
{
    int ret;

    ret = accessor(d_sequence, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SEQUENCE]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_sequence1, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SEQUENCE1]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_sequence2, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SEQUENCE2]);
    if (ret) {
        return ret;
    }

    return ret;
}

template <class ACCESSOR>
int EncoderTestSequenceWithUntagged7::accessAttribute(ACCESSOR& accessor, int id) const
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_SEQUENCE: {
        return accessor(d_sequence, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SEQUENCE]);
      }
      case ATTRIBUTE_ID_SEQUENCE1: {
        return accessor(d_sequence1, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SEQUENCE1]);
      }
      case ATTRIBUTE_ID_SEQUENCE2: {
        return accessor(d_sequence2, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SEQUENCE2]);
      }
      default:
        return NOT_FOUND;
    }
}

template <class ACCESSOR>
int EncoderTestSequenceWithUntagged7::accessAttribute(
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
const EncoderTestSequenceWithUntagged7Sequence& EncoderTestSequenceWithUntagged7::sequence() const
{
    return d_sequence;
}

inline
const EncoderTestSequenceWithUntagged7Sequence1& EncoderTestSequenceWithUntagged7::sequence1() const
{
    return d_sequence1;
}

inline
const EncoderTestSequenceWithUntagged7Sequence2& EncoderTestSequenceWithUntagged7::sequence2() const
{
    return d_sequence2;
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& hashAlg, const baljsn::EncoderTestSequenceWithUntagged7& object)
{
    (void)hashAlg;
    (void)object;
    using bslh::hashAppend;
    hashAppend(hashAlg, object.sequence());
    hashAppend(hashAlg, object.sequence1());
    hashAppend(hashAlg, object.sequence2());
}



                   // --------------------------------------
                   // class EncoderTestSequenceWithUntagged8
                   // --------------------------------------

// CLASS METHODS
// MANIPULATORS
template <class MANIPULATOR>
int EncoderTestSequenceWithUntagged8::manipulateAttributes(MANIPULATOR& manipulator)
{
    int ret;

    ret = manipulator(&d_sequence, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SEQUENCE]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_sequence1, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SEQUENCE1]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_attribute0, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE0]);
    if (ret) {
        return ret;
    }

    return ret;
}

template <class MANIPULATOR>
int EncoderTestSequenceWithUntagged8::manipulateAttribute(MANIPULATOR& manipulator, int id)
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_SEQUENCE: {
        return manipulator(&d_sequence, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SEQUENCE]);
      }
      case ATTRIBUTE_ID_SEQUENCE1: {
        return manipulator(&d_sequence1, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SEQUENCE1]);
      }
      case ATTRIBUTE_ID_ATTRIBUTE0: {
        return manipulator(&d_attribute0, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE0]);
      }
      default:
        return NOT_FOUND;
    }
}

template <class MANIPULATOR>
int EncoderTestSequenceWithUntagged8::manipulateAttribute(
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
EncoderTestSequenceWithUntagged8Sequence& EncoderTestSequenceWithUntagged8::sequence()
{
    return d_sequence;
}

inline
EncoderTestSequenceWithUntagged8Sequence1& EncoderTestSequenceWithUntagged8::sequence1()
{
    return d_sequence1;
}

inline
int& EncoderTestSequenceWithUntagged8::attribute0()
{
    return d_attribute0;
}

// ACCESSORS
template <class ACCESSOR>
int EncoderTestSequenceWithUntagged8::accessAttributes(ACCESSOR& accessor) const
{
    int ret;

    ret = accessor(d_sequence, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SEQUENCE]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_sequence1, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SEQUENCE1]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_attribute0, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE0]);
    if (ret) {
        return ret;
    }

    return ret;
}

template <class ACCESSOR>
int EncoderTestSequenceWithUntagged8::accessAttribute(ACCESSOR& accessor, int id) const
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_SEQUENCE: {
        return accessor(d_sequence, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SEQUENCE]);
      }
      case ATTRIBUTE_ID_SEQUENCE1: {
        return accessor(d_sequence1, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SEQUENCE1]);
      }
      case ATTRIBUTE_ID_ATTRIBUTE0: {
        return accessor(d_attribute0, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE0]);
      }
      default:
        return NOT_FOUND;
    }
}

template <class ACCESSOR>
int EncoderTestSequenceWithUntagged8::accessAttribute(
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
const EncoderTestSequenceWithUntagged8Sequence& EncoderTestSequenceWithUntagged8::sequence() const
{
    return d_sequence;
}

inline
const EncoderTestSequenceWithUntagged8Sequence1& EncoderTestSequenceWithUntagged8::sequence1() const
{
    return d_sequence1;
}

inline
int EncoderTestSequenceWithUntagged8::attribute0() const
{
    return d_attribute0;
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& hashAlg, const baljsn::EncoderTestSequenceWithUntagged8& object)
{
    (void)hashAlg;
    (void)object;
    using bslh::hashAppend;
    hashAppend(hashAlg, object.sequence());
    hashAppend(hashAlg, object.sequence1());
    hashAppend(hashAlg, object.attribute0());
}



                   // --------------------------------------
                   // class EncoderTestSequenceWithUntagged9
                   // --------------------------------------

// CLASS METHODS
// MANIPULATORS
template <class MANIPULATOR>
int EncoderTestSequenceWithUntagged9::manipulateAttributes(MANIPULATOR& manipulator)
{
    int ret;

    ret = manipulator(&d_sequence, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SEQUENCE]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_attribute0, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE0]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_sequence1, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SEQUENCE1]);
    if (ret) {
        return ret;
    }

    return ret;
}

template <class MANIPULATOR>
int EncoderTestSequenceWithUntagged9::manipulateAttribute(MANIPULATOR& manipulator, int id)
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_SEQUENCE: {
        return manipulator(&d_sequence, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SEQUENCE]);
      }
      case ATTRIBUTE_ID_ATTRIBUTE0: {
        return manipulator(&d_attribute0, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE0]);
      }
      case ATTRIBUTE_ID_SEQUENCE1: {
        return manipulator(&d_sequence1, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SEQUENCE1]);
      }
      default:
        return NOT_FOUND;
    }
}

template <class MANIPULATOR>
int EncoderTestSequenceWithUntagged9::manipulateAttribute(
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
EncoderTestSequenceWithUntagged9Sequence& EncoderTestSequenceWithUntagged9::sequence()
{
    return d_sequence;
}

inline
int& EncoderTestSequenceWithUntagged9::attribute0()
{
    return d_attribute0;
}

inline
EncoderTestSequenceWithUntagged9Sequence1& EncoderTestSequenceWithUntagged9::sequence1()
{
    return d_sequence1;
}

// ACCESSORS
template <class ACCESSOR>
int EncoderTestSequenceWithUntagged9::accessAttributes(ACCESSOR& accessor) const
{
    int ret;

    ret = accessor(d_sequence, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SEQUENCE]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_attribute0, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE0]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_sequence1, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SEQUENCE1]);
    if (ret) {
        return ret;
    }

    return ret;
}

template <class ACCESSOR>
int EncoderTestSequenceWithUntagged9::accessAttribute(ACCESSOR& accessor, int id) const
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_SEQUENCE: {
        return accessor(d_sequence, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SEQUENCE]);
      }
      case ATTRIBUTE_ID_ATTRIBUTE0: {
        return accessor(d_attribute0, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE0]);
      }
      case ATTRIBUTE_ID_SEQUENCE1: {
        return accessor(d_sequence1, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SEQUENCE1]);
      }
      default:
        return NOT_FOUND;
    }
}

template <class ACCESSOR>
int EncoderTestSequenceWithUntagged9::accessAttribute(
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
const EncoderTestSequenceWithUntagged9Sequence& EncoderTestSequenceWithUntagged9::sequence() const
{
    return d_sequence;
}

inline
int EncoderTestSequenceWithUntagged9::attribute0() const
{
    return d_attribute0;
}

inline
const EncoderTestSequenceWithUntagged9Sequence1& EncoderTestSequenceWithUntagged9::sequence1() const
{
    return d_sequence1;
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& hashAlg, const baljsn::EncoderTestSequenceWithUntagged9& object)
{
    (void)hashAlg;
    (void)object;
    using bslh::hashAppend;
    hashAppend(hashAlg, object.sequence());
    hashAppend(hashAlg, object.attribute0());
    hashAppend(hashAlg, object.sequence1());
}

}  // close package namespace

// FREE FUNCTIONS

inline
bool baljsn::operator==(
        const baljsn::EncoderTestAddress& lhs,
        const baljsn::EncoderTestAddress& rhs)
{
    return  lhs.street() == rhs.street()
         && lhs.city() == rhs.city()
         && lhs.state() == rhs.state();
}

inline
bool baljsn::operator!=(
        const baljsn::EncoderTestAddress& lhs,
        const baljsn::EncoderTestAddress& rhs)
{
    return !(lhs == rhs);
}

inline
bsl::ostream& baljsn::operator<<(
        bsl::ostream& stream,
        const baljsn::EncoderTestAddress& rhs)
{
    return rhs.print(stream, 0, -1);
}


inline
bool baljsn::operator==(
        const baljsn::EncoderTestChoiceWithAllCategoriesChoice& lhs,
        const baljsn::EncoderTestChoiceWithAllCategoriesChoice& rhs)
{
    typedef baljsn::EncoderTestChoiceWithAllCategoriesChoice Class;
    if (lhs.selectionId() == rhs.selectionId()) {
        switch (rhs.selectionId()) {
          case Class::SELECTION_ID_SELECTION0:
            return lhs.selection0() == rhs.selection0();
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
bool baljsn::operator!=(
        const baljsn::EncoderTestChoiceWithAllCategoriesChoice& lhs,
        const baljsn::EncoderTestChoiceWithAllCategoriesChoice& rhs)
{
    return !(lhs == rhs);
}

inline
bsl::ostream& baljsn::operator<<(
        bsl::ostream& stream,
        const baljsn::EncoderTestChoiceWithAllCategoriesChoice& rhs)
{
    return rhs.print(stream, 0, -1);
}


inline
bool baljsn::operator==(
        const baljsn::EncoderTestChoiceWithAllCategoriesCustomizedType& lhs,
        const baljsn::EncoderTestChoiceWithAllCategoriesCustomizedType& rhs)
{
    return lhs.d_value == rhs.d_value;
}

inline
bool baljsn::operator!=(
        const baljsn::EncoderTestChoiceWithAllCategoriesCustomizedType& lhs,
        const baljsn::EncoderTestChoiceWithAllCategoriesCustomizedType& rhs)
{
    return lhs.d_value != rhs.d_value;
}

inline
bsl::ostream& baljsn::operator<<(
        bsl::ostream& stream,
        const baljsn::EncoderTestChoiceWithAllCategoriesCustomizedType& rhs)
{
    return rhs.print(stream, 0, -1);
}


inline
bsl::ostream& baljsn::operator<<(
        bsl::ostream& stream,
        baljsn::EncoderTestChoiceWithAllCategoriesEnumeration::Value rhs)
{
    return baljsn::EncoderTestChoiceWithAllCategoriesEnumeration::print(stream, rhs);
}


inline
bool baljsn::operator==(
        const baljsn::EncoderTestChoiceWithAllCategoriesSequence& lhs,
        const baljsn::EncoderTestChoiceWithAllCategoriesSequence& rhs)
{
    return  lhs.attribute() == rhs.attribute();
}

inline
bool baljsn::operator!=(
        const baljsn::EncoderTestChoiceWithAllCategoriesSequence& lhs,
        const baljsn::EncoderTestChoiceWithAllCategoriesSequence& rhs)
{
    return !(lhs == rhs);
}

inline
bsl::ostream& baljsn::operator<<(
        bsl::ostream& stream,
        const baljsn::EncoderTestChoiceWithAllCategoriesSequence& rhs)
{
    return rhs.print(stream, 0, -1);
}


inline
bool baljsn::operator==(
        const baljsn::EncoderTestDegenerateChoice1Sequence&,
        const baljsn::EncoderTestDegenerateChoice1Sequence&)
{
    return true;
}

inline
bool baljsn::operator!=(
        const baljsn::EncoderTestDegenerateChoice1Sequence&,
        const baljsn::EncoderTestDegenerateChoice1Sequence&)
{
    return false;
}

inline
bsl::ostream& baljsn::operator<<(
        bsl::ostream& stream,
        const baljsn::EncoderTestDegenerateChoice1Sequence& rhs)
{
    return rhs.print(stream, 0, -1);
}


inline
bool baljsn::operator==(
        const baljsn::EncoderTestSequenceWithAllCategoriesChoice& lhs,
        const baljsn::EncoderTestSequenceWithAllCategoriesChoice& rhs)
{
    typedef baljsn::EncoderTestSequenceWithAllCategoriesChoice Class;
    if (lhs.selectionId() == rhs.selectionId()) {
        switch (rhs.selectionId()) {
          case Class::SELECTION_ID_SELECTION0:
            return lhs.selection0() == rhs.selection0();
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
bool baljsn::operator!=(
        const baljsn::EncoderTestSequenceWithAllCategoriesChoice& lhs,
        const baljsn::EncoderTestSequenceWithAllCategoriesChoice& rhs)
{
    return !(lhs == rhs);
}

inline
bsl::ostream& baljsn::operator<<(
        bsl::ostream& stream,
        const baljsn::EncoderTestSequenceWithAllCategoriesChoice& rhs)
{
    return rhs.print(stream, 0, -1);
}


inline
bool baljsn::operator==(
        const baljsn::EncoderTestSequenceWithAllCategoriesCustomizedType& lhs,
        const baljsn::EncoderTestSequenceWithAllCategoriesCustomizedType& rhs)
{
    return lhs.d_value == rhs.d_value;
}

inline
bool baljsn::operator!=(
        const baljsn::EncoderTestSequenceWithAllCategoriesCustomizedType& lhs,
        const baljsn::EncoderTestSequenceWithAllCategoriesCustomizedType& rhs)
{
    return lhs.d_value != rhs.d_value;
}

inline
bsl::ostream& baljsn::operator<<(
        bsl::ostream& stream,
        const baljsn::EncoderTestSequenceWithAllCategoriesCustomizedType& rhs)
{
    return rhs.print(stream, 0, -1);
}


inline
bsl::ostream& baljsn::operator<<(
        bsl::ostream& stream,
        baljsn::EncoderTestSequenceWithAllCategoriesEnumeration::Value rhs)
{
    return baljsn::EncoderTestSequenceWithAllCategoriesEnumeration::print(stream, rhs);
}


inline
bool baljsn::operator==(
        const baljsn::EncoderTestSequenceWithAllCategoriesSequence& lhs,
        const baljsn::EncoderTestSequenceWithAllCategoriesSequence& rhs)
{
    return  lhs.attribute() == rhs.attribute();
}

inline
bool baljsn::operator!=(
        const baljsn::EncoderTestSequenceWithAllCategoriesSequence& lhs,
        const baljsn::EncoderTestSequenceWithAllCategoriesSequence& rhs)
{
    return !(lhs == rhs);
}

inline
bsl::ostream& baljsn::operator<<(
        bsl::ostream& stream,
        const baljsn::EncoderTestSequenceWithAllCategoriesSequence& rhs)
{
    return rhs.print(stream, 0, -1);
}


inline
bool baljsn::operator==(
        const baljsn::EncoderTestSequenceWithUntagged0&,
        const baljsn::EncoderTestSequenceWithUntagged0&)
{
    return true;
}

inline
bool baljsn::operator!=(
        const baljsn::EncoderTestSequenceWithUntagged0&,
        const baljsn::EncoderTestSequenceWithUntagged0&)
{
    return false;
}

inline
bsl::ostream& baljsn::operator<<(
        bsl::ostream& stream,
        const baljsn::EncoderTestSequenceWithUntagged0& rhs)
{
    return rhs.print(stream, 0, -1);
}


inline
bool baljsn::operator==(
        const baljsn::EncoderTestSequenceWithUntagged10Sequence&,
        const baljsn::EncoderTestSequenceWithUntagged10Sequence&)
{
    return true;
}

inline
bool baljsn::operator!=(
        const baljsn::EncoderTestSequenceWithUntagged10Sequence&,
        const baljsn::EncoderTestSequenceWithUntagged10Sequence&)
{
    return false;
}

inline
bsl::ostream& baljsn::operator<<(
        bsl::ostream& stream,
        const baljsn::EncoderTestSequenceWithUntagged10Sequence& rhs)
{
    return rhs.print(stream, 0, -1);
}


inline
bool baljsn::operator==(
        const baljsn::EncoderTestSequenceWithUntagged11Sequence&,
        const baljsn::EncoderTestSequenceWithUntagged11Sequence&)
{
    return true;
}

inline
bool baljsn::operator!=(
        const baljsn::EncoderTestSequenceWithUntagged11Sequence&,
        const baljsn::EncoderTestSequenceWithUntagged11Sequence&)
{
    return false;
}

inline
bsl::ostream& baljsn::operator<<(
        bsl::ostream& stream,
        const baljsn::EncoderTestSequenceWithUntagged11Sequence& rhs)
{
    return rhs.print(stream, 0, -1);
}


inline
bool baljsn::operator==(
        const baljsn::EncoderTestSequenceWithUntagged11Sequence1&,
        const baljsn::EncoderTestSequenceWithUntagged11Sequence1&)
{
    return true;
}

inline
bool baljsn::operator!=(
        const baljsn::EncoderTestSequenceWithUntagged11Sequence1&,
        const baljsn::EncoderTestSequenceWithUntagged11Sequence1&)
{
    return false;
}

inline
bsl::ostream& baljsn::operator<<(
        bsl::ostream& stream,
        const baljsn::EncoderTestSequenceWithUntagged11Sequence1& rhs)
{
    return rhs.print(stream, 0, -1);
}


inline
bool baljsn::operator==(
        const baljsn::EncoderTestSequenceWithUntagged12Sequence&,
        const baljsn::EncoderTestSequenceWithUntagged12Sequence&)
{
    return true;
}

inline
bool baljsn::operator!=(
        const baljsn::EncoderTestSequenceWithUntagged12Sequence&,
        const baljsn::EncoderTestSequenceWithUntagged12Sequence&)
{
    return false;
}

inline
bsl::ostream& baljsn::operator<<(
        bsl::ostream& stream,
        const baljsn::EncoderTestSequenceWithUntagged12Sequence& rhs)
{
    return rhs.print(stream, 0, -1);
}


inline
bool baljsn::operator==(
        const baljsn::EncoderTestSequenceWithUntagged13Sequence&,
        const baljsn::EncoderTestSequenceWithUntagged13Sequence&)
{
    return true;
}

inline
bool baljsn::operator!=(
        const baljsn::EncoderTestSequenceWithUntagged13Sequence&,
        const baljsn::EncoderTestSequenceWithUntagged13Sequence&)
{
    return false;
}

inline
bsl::ostream& baljsn::operator<<(
        bsl::ostream& stream,
        const baljsn::EncoderTestSequenceWithUntagged13Sequence& rhs)
{
    return rhs.print(stream, 0, -1);
}


inline
bool baljsn::operator==(
        const baljsn::EncoderTestSequenceWithUntagged14& lhs,
        const baljsn::EncoderTestSequenceWithUntagged14& rhs)
{
    return  lhs.attribute0() == rhs.attribute0()
         && lhs.attribute1() == rhs.attribute1()
         && lhs.attribute2() == rhs.attribute2();
}

inline
bool baljsn::operator!=(
        const baljsn::EncoderTestSequenceWithUntagged14& lhs,
        const baljsn::EncoderTestSequenceWithUntagged14& rhs)
{
    return !(lhs == rhs);
}

inline
bsl::ostream& baljsn::operator<<(
        bsl::ostream& stream,
        const baljsn::EncoderTestSequenceWithUntagged14& rhs)
{
    return rhs.print(stream, 0, -1);
}


inline
bool baljsn::operator==(
        const baljsn::EncoderTestSequenceWithUntagged1Sequence&,
        const baljsn::EncoderTestSequenceWithUntagged1Sequence&)
{
    return true;
}

inline
bool baljsn::operator!=(
        const baljsn::EncoderTestSequenceWithUntagged1Sequence&,
        const baljsn::EncoderTestSequenceWithUntagged1Sequence&)
{
    return false;
}

inline
bsl::ostream& baljsn::operator<<(
        bsl::ostream& stream,
        const baljsn::EncoderTestSequenceWithUntagged1Sequence& rhs)
{
    return rhs.print(stream, 0, -1);
}


inline
bool baljsn::operator==(
        const baljsn::EncoderTestSequenceWithUntagged2& lhs,
        const baljsn::EncoderTestSequenceWithUntagged2& rhs)
{
    return  lhs.attribute0() == rhs.attribute0();
}

inline
bool baljsn::operator!=(
        const baljsn::EncoderTestSequenceWithUntagged2& lhs,
        const baljsn::EncoderTestSequenceWithUntagged2& rhs)
{
    return !(lhs == rhs);
}

inline
bsl::ostream& baljsn::operator<<(
        bsl::ostream& stream,
        const baljsn::EncoderTestSequenceWithUntagged2& rhs)
{
    return rhs.print(stream, 0, -1);
}


inline
bool baljsn::operator==(
        const baljsn::EncoderTestSequenceWithUntagged3Sequence&,
        const baljsn::EncoderTestSequenceWithUntagged3Sequence&)
{
    return true;
}

inline
bool baljsn::operator!=(
        const baljsn::EncoderTestSequenceWithUntagged3Sequence&,
        const baljsn::EncoderTestSequenceWithUntagged3Sequence&)
{
    return false;
}

inline
bsl::ostream& baljsn::operator<<(
        bsl::ostream& stream,
        const baljsn::EncoderTestSequenceWithUntagged3Sequence& rhs)
{
    return rhs.print(stream, 0, -1);
}


inline
bool baljsn::operator==(
        const baljsn::EncoderTestSequenceWithUntagged3Sequence1&,
        const baljsn::EncoderTestSequenceWithUntagged3Sequence1&)
{
    return true;
}

inline
bool baljsn::operator!=(
        const baljsn::EncoderTestSequenceWithUntagged3Sequence1&,
        const baljsn::EncoderTestSequenceWithUntagged3Sequence1&)
{
    return false;
}

inline
bsl::ostream& baljsn::operator<<(
        bsl::ostream& stream,
        const baljsn::EncoderTestSequenceWithUntagged3Sequence1& rhs)
{
    return rhs.print(stream, 0, -1);
}


inline
bool baljsn::operator==(
        const baljsn::EncoderTestSequenceWithUntagged4Sequence&,
        const baljsn::EncoderTestSequenceWithUntagged4Sequence&)
{
    return true;
}

inline
bool baljsn::operator!=(
        const baljsn::EncoderTestSequenceWithUntagged4Sequence&,
        const baljsn::EncoderTestSequenceWithUntagged4Sequence&)
{
    return false;
}

inline
bsl::ostream& baljsn::operator<<(
        bsl::ostream& stream,
        const baljsn::EncoderTestSequenceWithUntagged4Sequence& rhs)
{
    return rhs.print(stream, 0, -1);
}


inline
bool baljsn::operator==(
        const baljsn::EncoderTestSequenceWithUntagged5Sequence&,
        const baljsn::EncoderTestSequenceWithUntagged5Sequence&)
{
    return true;
}

inline
bool baljsn::operator!=(
        const baljsn::EncoderTestSequenceWithUntagged5Sequence&,
        const baljsn::EncoderTestSequenceWithUntagged5Sequence&)
{
    return false;
}

inline
bsl::ostream& baljsn::operator<<(
        bsl::ostream& stream,
        const baljsn::EncoderTestSequenceWithUntagged5Sequence& rhs)
{
    return rhs.print(stream, 0, -1);
}


inline
bool baljsn::operator==(
        const baljsn::EncoderTestSequenceWithUntagged6& lhs,
        const baljsn::EncoderTestSequenceWithUntagged6& rhs)
{
    return  lhs.attribute0() == rhs.attribute0()
         && lhs.attribute1() == rhs.attribute1();
}

inline
bool baljsn::operator!=(
        const baljsn::EncoderTestSequenceWithUntagged6& lhs,
        const baljsn::EncoderTestSequenceWithUntagged6& rhs)
{
    return !(lhs == rhs);
}

inline
bsl::ostream& baljsn::operator<<(
        bsl::ostream& stream,
        const baljsn::EncoderTestSequenceWithUntagged6& rhs)
{
    return rhs.print(stream, 0, -1);
}


inline
bool baljsn::operator==(
        const baljsn::EncoderTestSequenceWithUntagged7Sequence&,
        const baljsn::EncoderTestSequenceWithUntagged7Sequence&)
{
    return true;
}

inline
bool baljsn::operator!=(
        const baljsn::EncoderTestSequenceWithUntagged7Sequence&,
        const baljsn::EncoderTestSequenceWithUntagged7Sequence&)
{
    return false;
}

inline
bsl::ostream& baljsn::operator<<(
        bsl::ostream& stream,
        const baljsn::EncoderTestSequenceWithUntagged7Sequence& rhs)
{
    return rhs.print(stream, 0, -1);
}


inline
bool baljsn::operator==(
        const baljsn::EncoderTestSequenceWithUntagged7Sequence1&,
        const baljsn::EncoderTestSequenceWithUntagged7Sequence1&)
{
    return true;
}

inline
bool baljsn::operator!=(
        const baljsn::EncoderTestSequenceWithUntagged7Sequence1&,
        const baljsn::EncoderTestSequenceWithUntagged7Sequence1&)
{
    return false;
}

inline
bsl::ostream& baljsn::operator<<(
        bsl::ostream& stream,
        const baljsn::EncoderTestSequenceWithUntagged7Sequence1& rhs)
{
    return rhs.print(stream, 0, -1);
}


inline
bool baljsn::operator==(
        const baljsn::EncoderTestSequenceWithUntagged7Sequence2&,
        const baljsn::EncoderTestSequenceWithUntagged7Sequence2&)
{
    return true;
}

inline
bool baljsn::operator!=(
        const baljsn::EncoderTestSequenceWithUntagged7Sequence2&,
        const baljsn::EncoderTestSequenceWithUntagged7Sequence2&)
{
    return false;
}

inline
bsl::ostream& baljsn::operator<<(
        bsl::ostream& stream,
        const baljsn::EncoderTestSequenceWithUntagged7Sequence2& rhs)
{
    return rhs.print(stream, 0, -1);
}


inline
bool baljsn::operator==(
        const baljsn::EncoderTestSequenceWithUntagged8Sequence&,
        const baljsn::EncoderTestSequenceWithUntagged8Sequence&)
{
    return true;
}

inline
bool baljsn::operator!=(
        const baljsn::EncoderTestSequenceWithUntagged8Sequence&,
        const baljsn::EncoderTestSequenceWithUntagged8Sequence&)
{
    return false;
}

inline
bsl::ostream& baljsn::operator<<(
        bsl::ostream& stream,
        const baljsn::EncoderTestSequenceWithUntagged8Sequence& rhs)
{
    return rhs.print(stream, 0, -1);
}


inline
bool baljsn::operator==(
        const baljsn::EncoderTestSequenceWithUntagged8Sequence1&,
        const baljsn::EncoderTestSequenceWithUntagged8Sequence1&)
{
    return true;
}

inline
bool baljsn::operator!=(
        const baljsn::EncoderTestSequenceWithUntagged8Sequence1&,
        const baljsn::EncoderTestSequenceWithUntagged8Sequence1&)
{
    return false;
}

inline
bsl::ostream& baljsn::operator<<(
        bsl::ostream& stream,
        const baljsn::EncoderTestSequenceWithUntagged8Sequence1& rhs)
{
    return rhs.print(stream, 0, -1);
}


inline
bool baljsn::operator==(
        const baljsn::EncoderTestSequenceWithUntagged9Sequence&,
        const baljsn::EncoderTestSequenceWithUntagged9Sequence&)
{
    return true;
}

inline
bool baljsn::operator!=(
        const baljsn::EncoderTestSequenceWithUntagged9Sequence&,
        const baljsn::EncoderTestSequenceWithUntagged9Sequence&)
{
    return false;
}

inline
bsl::ostream& baljsn::operator<<(
        bsl::ostream& stream,
        const baljsn::EncoderTestSequenceWithUntagged9Sequence& rhs)
{
    return rhs.print(stream, 0, -1);
}


inline
bool baljsn::operator==(
        const baljsn::EncoderTestSequenceWithUntagged9Sequence1&,
        const baljsn::EncoderTestSequenceWithUntagged9Sequence1&)
{
    return true;
}

inline
bool baljsn::operator!=(
        const baljsn::EncoderTestSequenceWithUntagged9Sequence1&,
        const baljsn::EncoderTestSequenceWithUntagged9Sequence1&)
{
    return false;
}

inline
bsl::ostream& baljsn::operator<<(
        bsl::ostream& stream,
        const baljsn::EncoderTestSequenceWithUntagged9Sequence1& rhs)
{
    return rhs.print(stream, 0, -1);
}


inline
bool baljsn::operator==(
        const baljsn::EncoderTestChoiceWithAllCategories& lhs,
        const baljsn::EncoderTestChoiceWithAllCategories& rhs)
{
    typedef baljsn::EncoderTestChoiceWithAllCategories Class;
    if (lhs.selectionId() == rhs.selectionId()) {
        switch (rhs.selectionId()) {
          case Class::SELECTION_ID_CHAR_ARRAY:
            return lhs.charArray() == rhs.charArray();
          case Class::SELECTION_ID_CHOICE:
            return lhs.choice() == rhs.choice();
          case Class::SELECTION_ID_CUSTOMIZED_TYPE:
            return lhs.customizedType() == rhs.customizedType();
          case Class::SELECTION_ID_ENUMERATION:
            return lhs.enumeration() == rhs.enumeration();
          case Class::SELECTION_ID_SEQUENCE:
            return lhs.sequence() == rhs.sequence();
          case Class::SELECTION_ID_SIMPLE:
            return lhs.simple() == rhs.simple();
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
bool baljsn::operator!=(
        const baljsn::EncoderTestChoiceWithAllCategories& lhs,
        const baljsn::EncoderTestChoiceWithAllCategories& rhs)
{
    return !(lhs == rhs);
}

inline
bsl::ostream& baljsn::operator<<(
        bsl::ostream& stream,
        const baljsn::EncoderTestChoiceWithAllCategories& rhs)
{
    return rhs.print(stream, 0, -1);
}


inline
bool baljsn::operator==(
        const baljsn::EncoderTestDegenerateChoice1& lhs,
        const baljsn::EncoderTestDegenerateChoice1& rhs)
{
    typedef baljsn::EncoderTestDegenerateChoice1 Class;
    if (lhs.selectionId() == rhs.selectionId()) {
        switch (rhs.selectionId()) {
          case Class::SELECTION_ID_SEQUENCE:
            return lhs.sequence() == rhs.sequence();
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
bool baljsn::operator!=(
        const baljsn::EncoderTestDegenerateChoice1& lhs,
        const baljsn::EncoderTestDegenerateChoice1& rhs)
{
    return !(lhs == rhs);
}

inline
bsl::ostream& baljsn::operator<<(
        bsl::ostream& stream,
        const baljsn::EncoderTestDegenerateChoice1& rhs)
{
    return rhs.print(stream, 0, -1);
}


inline
bool baljsn::operator==(
        const baljsn::EncoderTestEmployee& lhs,
        const baljsn::EncoderTestEmployee& rhs)
{
    return  lhs.name() == rhs.name()
         && lhs.homeAddress() == rhs.homeAddress()
         && lhs.age() == rhs.age();
}

inline
bool baljsn::operator!=(
        const baljsn::EncoderTestEmployee& lhs,
        const baljsn::EncoderTestEmployee& rhs)
{
    return !(lhs == rhs);
}

inline
bsl::ostream& baljsn::operator<<(
        bsl::ostream& stream,
        const baljsn::EncoderTestEmployee& rhs)
{
    return rhs.print(stream, 0, -1);
}


inline
bool baljsn::operator==(
        const baljsn::EncoderTestSequenceWithAllCategories& lhs,
        const baljsn::EncoderTestSequenceWithAllCategories& rhs)
{
    return  lhs.charArray() == rhs.charArray()
         && lhs.array() == rhs.array()
         && lhs.choice() == rhs.choice()
         && lhs.customizedType() == rhs.customizedType()
         && lhs.enumeration() == rhs.enumeration()
         && lhs.nullableValue() == rhs.nullableValue()
         && lhs.sequence() == rhs.sequence()
         && lhs.simple() == rhs.simple();
}

inline
bool baljsn::operator!=(
        const baljsn::EncoderTestSequenceWithAllCategories& lhs,
        const baljsn::EncoderTestSequenceWithAllCategories& rhs)
{
    return !(lhs == rhs);
}

inline
bsl::ostream& baljsn::operator<<(
        bsl::ostream& stream,
        const baljsn::EncoderTestSequenceWithAllCategories& rhs)
{
    return rhs.print(stream, 0, -1);
}


inline
bool baljsn::operator==(
        const baljsn::EncoderTestSequenceWithUntagged1& lhs,
        const baljsn::EncoderTestSequenceWithUntagged1& rhs)
{
    return  lhs.sequence() == rhs.sequence();
}

inline
bool baljsn::operator!=(
        const baljsn::EncoderTestSequenceWithUntagged1& lhs,
        const baljsn::EncoderTestSequenceWithUntagged1& rhs)
{
    return !(lhs == rhs);
}

inline
bsl::ostream& baljsn::operator<<(
        bsl::ostream& stream,
        const baljsn::EncoderTestSequenceWithUntagged1& rhs)
{
    return rhs.print(stream, 0, -1);
}


inline
bool baljsn::operator==(
        const baljsn::EncoderTestSequenceWithUntagged10& lhs,
        const baljsn::EncoderTestSequenceWithUntagged10& rhs)
{
    return  lhs.sequence() == rhs.sequence()
         && lhs.attribute0() == rhs.attribute0()
         && lhs.attribute1() == rhs.attribute1();
}

inline
bool baljsn::operator!=(
        const baljsn::EncoderTestSequenceWithUntagged10& lhs,
        const baljsn::EncoderTestSequenceWithUntagged10& rhs)
{
    return !(lhs == rhs);
}

inline
bsl::ostream& baljsn::operator<<(
        bsl::ostream& stream,
        const baljsn::EncoderTestSequenceWithUntagged10& rhs)
{
    return rhs.print(stream, 0, -1);
}


inline
bool baljsn::operator==(
        const baljsn::EncoderTestSequenceWithUntagged11& lhs,
        const baljsn::EncoderTestSequenceWithUntagged11& rhs)
{
    return  lhs.attribute0() == rhs.attribute0()
         && lhs.sequence() == rhs.sequence()
         && lhs.sequence1() == rhs.sequence1();
}

inline
bool baljsn::operator!=(
        const baljsn::EncoderTestSequenceWithUntagged11& lhs,
        const baljsn::EncoderTestSequenceWithUntagged11& rhs)
{
    return !(lhs == rhs);
}

inline
bsl::ostream& baljsn::operator<<(
        bsl::ostream& stream,
        const baljsn::EncoderTestSequenceWithUntagged11& rhs)
{
    return rhs.print(stream, 0, -1);
}


inline
bool baljsn::operator==(
        const baljsn::EncoderTestSequenceWithUntagged12& lhs,
        const baljsn::EncoderTestSequenceWithUntagged12& rhs)
{
    return  lhs.attribute0() == rhs.attribute0()
         && lhs.sequence() == rhs.sequence()
         && lhs.attribute1() == rhs.attribute1();
}

inline
bool baljsn::operator!=(
        const baljsn::EncoderTestSequenceWithUntagged12& lhs,
        const baljsn::EncoderTestSequenceWithUntagged12& rhs)
{
    return !(lhs == rhs);
}

inline
bsl::ostream& baljsn::operator<<(
        bsl::ostream& stream,
        const baljsn::EncoderTestSequenceWithUntagged12& rhs)
{
    return rhs.print(stream, 0, -1);
}


inline
bool baljsn::operator==(
        const baljsn::EncoderTestSequenceWithUntagged13& lhs,
        const baljsn::EncoderTestSequenceWithUntagged13& rhs)
{
    return  lhs.attribute0() == rhs.attribute0()
         && lhs.attribute1() == rhs.attribute1()
         && lhs.sequence() == rhs.sequence();
}

inline
bool baljsn::operator!=(
        const baljsn::EncoderTestSequenceWithUntagged13& lhs,
        const baljsn::EncoderTestSequenceWithUntagged13& rhs)
{
    return !(lhs == rhs);
}

inline
bsl::ostream& baljsn::operator<<(
        bsl::ostream& stream,
        const baljsn::EncoderTestSequenceWithUntagged13& rhs)
{
    return rhs.print(stream, 0, -1);
}


inline
bool baljsn::operator==(
        const baljsn::EncoderTestSequenceWithUntagged3& lhs,
        const baljsn::EncoderTestSequenceWithUntagged3& rhs)
{
    return  lhs.sequence() == rhs.sequence()
         && lhs.sequence1() == rhs.sequence1();
}

inline
bool baljsn::operator!=(
        const baljsn::EncoderTestSequenceWithUntagged3& lhs,
        const baljsn::EncoderTestSequenceWithUntagged3& rhs)
{
    return !(lhs == rhs);
}

inline
bsl::ostream& baljsn::operator<<(
        bsl::ostream& stream,
        const baljsn::EncoderTestSequenceWithUntagged3& rhs)
{
    return rhs.print(stream, 0, -1);
}


inline
bool baljsn::operator==(
        const baljsn::EncoderTestSequenceWithUntagged4& lhs,
        const baljsn::EncoderTestSequenceWithUntagged4& rhs)
{
    return  lhs.sequence() == rhs.sequence()
         && lhs.attribute0() == rhs.attribute0();
}

inline
bool baljsn::operator!=(
        const baljsn::EncoderTestSequenceWithUntagged4& lhs,
        const baljsn::EncoderTestSequenceWithUntagged4& rhs)
{
    return !(lhs == rhs);
}

inline
bsl::ostream& baljsn::operator<<(
        bsl::ostream& stream,
        const baljsn::EncoderTestSequenceWithUntagged4& rhs)
{
    return rhs.print(stream, 0, -1);
}


inline
bool baljsn::operator==(
        const baljsn::EncoderTestSequenceWithUntagged5& lhs,
        const baljsn::EncoderTestSequenceWithUntagged5& rhs)
{
    return  lhs.attribute0() == rhs.attribute0()
         && lhs.sequence() == rhs.sequence();
}

inline
bool baljsn::operator!=(
        const baljsn::EncoderTestSequenceWithUntagged5& lhs,
        const baljsn::EncoderTestSequenceWithUntagged5& rhs)
{
    return !(lhs == rhs);
}

inline
bsl::ostream& baljsn::operator<<(
        bsl::ostream& stream,
        const baljsn::EncoderTestSequenceWithUntagged5& rhs)
{
    return rhs.print(stream, 0, -1);
}


inline
bool baljsn::operator==(
        const baljsn::EncoderTestSequenceWithUntagged7& lhs,
        const baljsn::EncoderTestSequenceWithUntagged7& rhs)
{
    return  lhs.sequence() == rhs.sequence()
         && lhs.sequence1() == rhs.sequence1()
         && lhs.sequence2() == rhs.sequence2();
}

inline
bool baljsn::operator!=(
        const baljsn::EncoderTestSequenceWithUntagged7& lhs,
        const baljsn::EncoderTestSequenceWithUntagged7& rhs)
{
    return !(lhs == rhs);
}

inline
bsl::ostream& baljsn::operator<<(
        bsl::ostream& stream,
        const baljsn::EncoderTestSequenceWithUntagged7& rhs)
{
    return rhs.print(stream, 0, -1);
}


inline
bool baljsn::operator==(
        const baljsn::EncoderTestSequenceWithUntagged8& lhs,
        const baljsn::EncoderTestSequenceWithUntagged8& rhs)
{
    return  lhs.sequence() == rhs.sequence()
         && lhs.sequence1() == rhs.sequence1()
         && lhs.attribute0() == rhs.attribute0();
}

inline
bool baljsn::operator!=(
        const baljsn::EncoderTestSequenceWithUntagged8& lhs,
        const baljsn::EncoderTestSequenceWithUntagged8& rhs)
{
    return !(lhs == rhs);
}

inline
bsl::ostream& baljsn::operator<<(
        bsl::ostream& stream,
        const baljsn::EncoderTestSequenceWithUntagged8& rhs)
{
    return rhs.print(stream, 0, -1);
}


inline
bool baljsn::operator==(
        const baljsn::EncoderTestSequenceWithUntagged9& lhs,
        const baljsn::EncoderTestSequenceWithUntagged9& rhs)
{
    return  lhs.sequence() == rhs.sequence()
         && lhs.attribute0() == rhs.attribute0()
         && lhs.sequence1() == rhs.sequence1();
}

inline
bool baljsn::operator!=(
        const baljsn::EncoderTestSequenceWithUntagged9& lhs,
        const baljsn::EncoderTestSequenceWithUntagged9& rhs)
{
    return !(lhs == rhs);
}

inline
bsl::ostream& baljsn::operator<<(
        bsl::ostream& stream,
        const baljsn::EncoderTestSequenceWithUntagged9& rhs)
{
    return rhs.print(stream, 0, -1);
}

}  // close enterprise namespace
#endif

// GENERATED BY BLP_BAS_CODEGEN_VERSION
// USING bas_codegen.pl --mode msg --noAggregateConversion --noExternalization --msgComponent=encoder_testtypes --package=baljsn baljsn_encoder_testtypes.xsd
// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright 2020 Bloomberg Finance L.P. All rights reserved.
//      Property of Bloomberg Finance L.P. (BFLP)
//      This software is made available solely pursuant to the
//      terms of a BFLP license agreement which governs its use.
// ------------------------------- END-OF-FILE --------------------------------
