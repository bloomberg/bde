// baljsn_encoder_testtypes.h        *DO NOT EDIT*         @generated -*-C++-*-
#ifndef INCLUDED_BALJSN_ENCODER_TESTTYPES
#define INCLUDED_BALJSN_ENCODER_TESTTYPES

#include <bsls_ident.h>
BSLS_IDENT_RCSID(baljsn_encoder_testtypes_h, "$Id$ $CSID$")
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

    // PRIVATE ACCESSORS
    template <typename t_HASH_ALGORITHM>
    void hashAppendImpl(t_HASH_ALGORITHM& hashAlgorithm) const;

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

    template <typename t_MANIPULATOR>
    int manipulateAttributes(t_MANIPULATOR& manipulator);
        // Invoke the specified 'manipulator' sequentially on the address of
        // each (modifiable) attribute of this object, supplying 'manipulator'
        // with the corresponding attribute information structure until such
        // invocation returns a non-zero value.  Return the value from the
        // last invocation of 'manipulator' (i.e., the invocation that
        // terminated the sequence).

    template <typename t_MANIPULATOR>
    int manipulateAttribute(t_MANIPULATOR& manipulator, int id);
        // Invoke the specified 'manipulator' on the address of
        // the (modifiable) attribute indicated by the specified 'id',
        // supplying 'manipulator' with the corresponding attribute
        // information structure.  Return the value returned from the
        // invocation of 'manipulator' if 'id' identifies an attribute of this
        // class, and -1 otherwise.

    template <typename t_MANIPULATOR>
    int manipulateAttribute(t_MANIPULATOR&  manipulator,
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
                        int           level          = 0,
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

    template <typename t_ACCESSOR>
    int accessAttributes(t_ACCESSOR& accessor) const;
        // Invoke the specified 'accessor' sequentially on each
        // (non-modifiable) attribute of this object, supplying 'accessor'
        // with the corresponding attribute information structure until such
        // invocation returns a non-zero value.  Return the value from the
        // last invocation of 'accessor' (i.e., the invocation that terminated
        // the sequence).

    template <typename t_ACCESSOR>
    int accessAttribute(t_ACCESSOR& accessor, int id) const;
        // Invoke the specified 'accessor' on the (non-modifiable) attribute
        // of this object indicated by the specified 'id', supplying 'accessor'
        // with the corresponding attribute information structure.  Return the
        // value returned from the invocation of 'accessor' if 'id' identifies
        // an attribute of this class, and -1 otherwise.

    template <typename t_ACCESSOR>
    int accessAttribute(t_ACCESSOR&   accessor,
                        const char *name,
                        int         nameLength) const;
        // Invoke the specified 'accessor' on the (non-modifiable) attribute
        // of this object indicated by the specified 'name' of the specified
        // 'nameLength', supplying 'accessor' with the corresponding attribute
        // information structure.  Return the value returned from the
        // invocation of 'accessor' if 'name' identifies an attribute of this
        // class, and -1 otherwise.

    const bsl::string& street() const;
        // Return a reference offering non-modifiable access to the "Street"
        // attribute of this object.

    const bsl::string& city() const;
        // Return a reference offering non-modifiable access to the "City"
        // attribute of this object.

    const bsl::string& state() const;
        // Return a reference offering non-modifiable access to the "State"
        // attribute of this object.

    // HIDDEN FRIENDS
    friend bool operator==(const EncoderTestAddress& lhs,
                           const EncoderTestAddress& rhs)
        // Return 'true' if the specified 'lhs' and 'rhs' attribute objects
        // have the same value, and 'false' otherwise.  Two attribute objects
        // have the same value if each respective attribute has the same value.
    {
        return lhs.street() == rhs.street() &&
               lhs.city() == rhs.city() &&
               lhs.state() == rhs.state();
    }

    friend bool operator!=(const EncoderTestAddress& lhs,
                           const EncoderTestAddress& rhs)
        // Returns '!(lhs == rhs)'
    {
        return !(lhs == rhs);
    }

    friend bsl::ostream& operator<<(bsl::ostream&             stream,
                                    const EncoderTestAddress& rhs)
        // Format the specified 'rhs' to the specified output 'stream' and
        // return a reference to the modifiable 'stream'.
    {
        return rhs.print(stream, 0, -1);
    }

    template <typename t_HASH_ALGORITHM>
    friend void hashAppend(t_HASH_ALGORITHM&         hashAlg,
                           const EncoderTestAddress& object)
        // Pass the specified 'object' to the specified 'hashAlg'.  This
        // function integrates with the 'bslh' modular hashing system and
        // effectively provides a 'bsl::hash' specialization for
        // 'EncoderTestAddress'.
    {
        object.hashAppendImpl(hashAlg);
    }
};

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

    // PRIVATE ACCESSORS
    template <typename t_HASH_ALGORITHM>
    void hashAppendImpl(t_HASH_ALGORITHM& hashAlgorithm) const;

    bool isEqualTo(const EncoderTestChoiceWithAllCategoriesChoice& rhs) const;

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

    template <typename t_MANIPULATOR>
    int manipulateSelection(t_MANIPULATOR& manipulator);
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

    template <typename t_ACCESSOR>
    int accessSelection(t_ACCESSOR& accessor) const;
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

    // HIDDEN FRIENDS
    friend bool operator==(const EncoderTestChoiceWithAllCategoriesChoice& lhs,
                           const EncoderTestChoiceWithAllCategoriesChoice& rhs)
        // Return 'true' if the specified 'lhs' and 'rhs' objects have the same
        // value, and 'false' otherwise.  Two
        // 'EncoderTestChoiceWithAllCategoriesChoice' objects have the same
        // value if either the selections in both objects have the same ids and
        // the same values, or both selections are undefined.
    {
        return lhs.isEqualTo(rhs);
    }

    friend bool operator!=(const EncoderTestChoiceWithAllCategoriesChoice& lhs,
                           const EncoderTestChoiceWithAllCategoriesChoice& rhs)
        // Return 'true' if the specified 'lhs' and 'rhs' objects do not have
        // the same values, as determined by 'operator==', and 'false'
        // otherwise.
    {
        return !(lhs == rhs);
    }

    friend bsl::ostream& operator<<(
                        bsl::ostream&                                   stream,
                        const EncoderTestChoiceWithAllCategoriesChoice& rhs)
        // Format the specified 'rhs' to the specified output 'stream' and
        // return a reference to the modifiable 'stream'.
    {
        return rhs.print(stream, 0, -1);
    }

    template <typename t_HASH_ALGORITHM>
    friend void hashAppend(
                       t_HASH_ALGORITHM&                               hashAlg,
                       const EncoderTestChoiceWithAllCategoriesChoice& object)
        // Pass the specified 'object' to the specified 'hashAlg'.  This
        // function integrates with the 'bslh' modular hashing system and
        // effectively provides a 'bsl::hash' specialization for
        // 'EncoderTestChoiceWithAllCategoriesChoice'.
    {
        return object.hashAppendImpl(hashAlg);
    }
};

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
        // successful and non-zero otherwise.  Note that, if 'value' is an
        // invalid UTF-8 string and
        // "EncoderTestChoiceWithAllCategoriesCustomizedType" has length
        // restrictions, this function will fail and keep the underlying string
        // unchanged.
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

    // HIDDEN FRIENDS
    friend bool operator==(
                   const EncoderTestChoiceWithAllCategoriesCustomizedType& lhs,
                   const EncoderTestChoiceWithAllCategoriesCustomizedType& rhs)
        // Return 'true' if the specified 'lhs' and 'rhs' attribute objects
        // have the same value, and 'false' otherwise.  Two attribute objects
        // have the same value if each respective attribute has the same value.
    {
        return lhs.d_value == rhs.d_value;
    }

    friend bool operator!=(
                   const EncoderTestChoiceWithAllCategoriesCustomizedType& lhs,
                   const EncoderTestChoiceWithAllCategoriesCustomizedType& rhs)
        // Return 'true' if the specified 'lhs' and 'rhs' attribute objects do
        // not have the same value, and 'false' otherwise.  Two attribute
        // objects do not have the same value if one or more respective
        // attributes differ in values.
    {
        return lhs.d_value != rhs.d_value;
    }

    friend bsl::ostream& operator<<(
                bsl::ostream&                                           stream,
                const EncoderTestChoiceWithAllCategoriesCustomizedType& rhs)
        // Format the specified 'rhs' to the specified output 'stream' and
        // return a reference to the modifiable 'stream'.
    {
        return rhs.print(stream, 0, -1);
    }

    template <typename t_HASH_ALGORITHM>
    friend void hashAppend(
               t_HASH_ALGORITHM&                                       hashAlg,
               const EncoderTestChoiceWithAllCategoriesCustomizedType& object)
        // Pass the specified 'object' to the specified 'hashAlg'.  This
        // function integrates with the 'bslh' modular hashing system and
        // effectively provides a 'bsl::hash' specialization for
        // 'EncoderTestChoiceWithAllCategoriesCustomizedType'.
    {
        using bslh::hashAppend;
        hashAppend(hashAlg, object.toString());
    }
};

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

    // HIDDEN FRIENDS
    friend bsl::ostream& operator<<(bsl::ostream& stream, Value rhs)
        // Format the specified 'rhs' to the specified output 'stream' and
        // return a reference to the modifiable 'stream'.
    {
        return EncoderTestChoiceWithAllCategoriesEnumeration::print(stream, rhs);
    }
};

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


    // MANIPULATORS
    void reset();
        // Reset this object to the default value (i.e., its value upon
        // default construction).

    template <typename t_MANIPULATOR>
    int manipulateAttributes(t_MANIPULATOR& manipulator);
        // Invoke the specified 'manipulator' sequentially on the address of
        // each (modifiable) attribute of this object, supplying 'manipulator'
        // with the corresponding attribute information structure until such
        // invocation returns a non-zero value.  Return the value from the
        // last invocation of 'manipulator' (i.e., the invocation that
        // terminated the sequence).

    template <typename t_MANIPULATOR>
    int manipulateAttribute(t_MANIPULATOR& manipulator, int id);
        // Invoke the specified 'manipulator' on the address of
        // the (modifiable) attribute indicated by the specified 'id',
        // supplying 'manipulator' with the corresponding attribute
        // information structure.  Return the value returned from the
        // invocation of 'manipulator' if 'id' identifies an attribute of this
        // class, and -1 otherwise.

    template <typename t_MANIPULATOR>
    int manipulateAttribute(t_MANIPULATOR&  manipulator,
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
                        int           level          = 0,
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

    template <typename t_ACCESSOR>
    int accessAttributes(t_ACCESSOR& accessor) const;
        // Invoke the specified 'accessor' sequentially on each
        // (non-modifiable) attribute of this object, supplying 'accessor'
        // with the corresponding attribute information structure until such
        // invocation returns a non-zero value.  Return the value from the
        // last invocation of 'accessor' (i.e., the invocation that terminated
        // the sequence).

    template <typename t_ACCESSOR>
    int accessAttribute(t_ACCESSOR& accessor, int id) const;
        // Invoke the specified 'accessor' on the (non-modifiable) attribute
        // of this object indicated by the specified 'id', supplying 'accessor'
        // with the corresponding attribute information structure.  Return the
        // value returned from the invocation of 'accessor' if 'id' identifies
        // an attribute of this class, and -1 otherwise.

    template <typename t_ACCESSOR>
    int accessAttribute(t_ACCESSOR&   accessor,
                        const char *name,
                        int         nameLength) const;
        // Invoke the specified 'accessor' on the (non-modifiable) attribute
        // of this object indicated by the specified 'name' of the specified
        // 'nameLength', supplying 'accessor' with the corresponding attribute
        // information structure.  Return the value returned from the
        // invocation of 'accessor' if 'name' identifies an attribute of this
        // class, and -1 otherwise.

    int attribute() const;
        // Return the value of the "Attribute" attribute of this object.

    // HIDDEN FRIENDS
    friend bool operator==(
                         const EncoderTestChoiceWithAllCategoriesSequence& lhs,
                         const EncoderTestChoiceWithAllCategoriesSequence& rhs)
        // Return 'true' if the specified 'lhs' and 'rhs' attribute objects
        // have the same value, and 'false' otherwise.  Two attribute objects
        // have the same value if each respective attribute has the same value.
    {
        return lhs.attribute() == rhs.attribute();
    }

    friend bool operator!=(
                         const EncoderTestChoiceWithAllCategoriesSequence& lhs,
                         const EncoderTestChoiceWithAllCategoriesSequence& rhs)
        // Returns '!(lhs == rhs)'
    {
        return !(lhs == rhs);
    }

    friend bsl::ostream& operator<<(
                      bsl::ostream&                                     stream,
                      const EncoderTestChoiceWithAllCategoriesSequence& rhs)
        // Format the specified 'rhs' to the specified output 'stream' and
        // return a reference to the modifiable 'stream'.
    {
        return rhs.print(stream, 0, -1);
    }

    template <typename t_HASH_ALGORITHM>
    friend void hashAppend(
                     t_HASH_ALGORITHM&                                 hashAlg,
                     const EncoderTestChoiceWithAllCategoriesSequence& object)
        // Pass the specified 'object' to the specified 'hashAlg'.  This
        // function integrates with the 'bslh' modular hashing system and
        // effectively provides a 'bsl::hash' specialization for
        // 'EncoderTestChoiceWithAllCategoriesSequence'.
    {
        using bslh::hashAppend;
        hashAppend(hashAlg, object.attribute());
    }
};

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

    // MANIPULATORS
    void reset();
        // Reset this object to the default value (i.e., its value upon
        // default construction).

    template <typename t_MANIPULATOR>
    int manipulateAttributes(t_MANIPULATOR& manipulator);
        // Invoke the specified 'manipulator' sequentially on the address of
        // each (modifiable) attribute of this object, supplying 'manipulator'
        // with the corresponding attribute information structure until such
        // invocation returns a non-zero value.  Return the value from the
        // last invocation of 'manipulator' (i.e., the invocation that
        // terminated the sequence).

    template <typename t_MANIPULATOR>
    int manipulateAttribute(t_MANIPULATOR& manipulator, int id);
        // Invoke the specified 'manipulator' on the address of
        // the (modifiable) attribute indicated by the specified 'id',
        // supplying 'manipulator' with the corresponding attribute
        // information structure.  Return the value returned from the
        // invocation of 'manipulator' if 'id' identifies an attribute of this
        // class, and -1 otherwise.

    template <typename t_MANIPULATOR>
    int manipulateAttribute(t_MANIPULATOR&  manipulator,
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
                        int           level          = 0,
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

    template <typename t_ACCESSOR>
    int accessAttributes(t_ACCESSOR& accessor) const;
        // Invoke the specified 'accessor' sequentially on each
        // (non-modifiable) attribute of this object, supplying 'accessor'
        // with the corresponding attribute information structure until such
        // invocation returns a non-zero value.  Return the value from the
        // last invocation of 'accessor' (i.e., the invocation that terminated
        // the sequence).

    template <typename t_ACCESSOR>
    int accessAttribute(t_ACCESSOR& accessor, int id) const;
        // Invoke the specified 'accessor' on the (non-modifiable) attribute
        // of this object indicated by the specified 'id', supplying 'accessor'
        // with the corresponding attribute information structure.  Return the
        // value returned from the invocation of 'accessor' if 'id' identifies
        // an attribute of this class, and -1 otherwise.

    template <typename t_ACCESSOR>
    int accessAttribute(t_ACCESSOR&   accessor,
                        const char *name,
                        int         nameLength) const;
        // Invoke the specified 'accessor' on the (non-modifiable) attribute
        // of this object indicated by the specified 'name' of the specified
        // 'nameLength', supplying 'accessor' with the corresponding attribute
        // information structure.  Return the value returned from the
        // invocation of 'accessor' if 'name' identifies an attribute of this
        // class, and -1 otherwise.

    // HIDDEN FRIENDS
    friend bool operator==(const EncoderTestDegenerateChoice1Sequence&,
                           const EncoderTestDegenerateChoice1Sequence&)
        // Returns 'true' as this type has no attributes and so all objects of
        // this type are considered equal.
    {
        return true;
    }

    friend bool operator!=(const EncoderTestDegenerateChoice1Sequence& lhs,
                           const EncoderTestDegenerateChoice1Sequence& rhs)
        // Returns '!(lhs == rhs)'
    {
        return !(lhs == rhs);
    }

    friend bsl::ostream& operator<<(
                            bsl::ostream&                               stream,
                            const EncoderTestDegenerateChoice1Sequence& rhs)
        // Format the specified 'rhs' to the specified output 'stream' and
        // return a reference to the modifiable 'stream'.
    {
        return rhs.print(stream, 0, -1);
    }

    template <typename t_HASH_ALGORITHM>
    friend void hashAppend(t_HASH_ALGORITHM&                          ,
                           const EncoderTestDegenerateChoice1Sequence&)
        // Pass the specified 'object' to the specified 'hashAlg'.  This
        // function integrates with the 'bslh' modular hashing system and
        // effectively provides a 'bsl::hash' specialization for
        // 'EncoderTestDegenerateChoice1Sequence'.
    {
    }
};

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

    // PRIVATE ACCESSORS
    template <typename t_HASH_ALGORITHM>
    void hashAppendImpl(t_HASH_ALGORITHM& hashAlgorithm) const;

    bool isEqualTo(const EncoderTestSequenceWithAllCategoriesChoice& rhs) const;

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

    template <typename t_MANIPULATOR>
    int manipulateSelection(t_MANIPULATOR& manipulator);
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

    template <typename t_ACCESSOR>
    int accessSelection(t_ACCESSOR& accessor) const;
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

    // HIDDEN FRIENDS
    friend bool operator==(
                         const EncoderTestSequenceWithAllCategoriesChoice& lhs,
                         const EncoderTestSequenceWithAllCategoriesChoice& rhs)
        // Return 'true' if the specified 'lhs' and 'rhs' objects have the same
        // value, and 'false' otherwise.  Two
        // 'EncoderTestSequenceWithAllCategoriesChoice' objects have the same
        // value if either the selections in both objects have the same ids and
        // the same values, or both selections are undefined.
    {
        return lhs.isEqualTo(rhs);
    }

    friend bool operator!=(
                         const EncoderTestSequenceWithAllCategoriesChoice& lhs,
                         const EncoderTestSequenceWithAllCategoriesChoice& rhs)
        // Return 'true' if the specified 'lhs' and 'rhs' objects do not have
        // the same values, as determined by 'operator==', and 'false'
        // otherwise.
    {
        return !(lhs == rhs);
    }

    friend bsl::ostream& operator<<(
                      bsl::ostream&                                     stream,
                      const EncoderTestSequenceWithAllCategoriesChoice& rhs)
        // Format the specified 'rhs' to the specified output 'stream' and
        // return a reference to the modifiable 'stream'.
    {
        return rhs.print(stream, 0, -1);
    }

    template <typename t_HASH_ALGORITHM>
    friend void hashAppend(
                     t_HASH_ALGORITHM&                                 hashAlg,
                     const EncoderTestSequenceWithAllCategoriesChoice& object)
        // Pass the specified 'object' to the specified 'hashAlg'.  This
        // function integrates with the 'bslh' modular hashing system and
        // effectively provides a 'bsl::hash' specialization for
        // 'EncoderTestSequenceWithAllCategoriesChoice'.
    {
        return object.hashAppendImpl(hashAlg);
    }
};

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
        // successful and non-zero otherwise.  Note that, if 'value' is an
        // invalid UTF-8 string and
        // "EncoderTestSequenceWithAllCategoriesCustomizedType" has length
        // restrictions, this function will fail and keep the underlying string
        // unchanged.
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

    // HIDDEN FRIENDS
    friend bool operator==(
                 const EncoderTestSequenceWithAllCategoriesCustomizedType& lhs,
                 const EncoderTestSequenceWithAllCategoriesCustomizedType& rhs)
        // Return 'true' if the specified 'lhs' and 'rhs' attribute objects
        // have the same value, and 'false' otherwise.  Two attribute objects
        // have the same value if each respective attribute has the same value.
    {
        return lhs.d_value == rhs.d_value;
    }

    friend bool operator!=(
                 const EncoderTestSequenceWithAllCategoriesCustomizedType& lhs,
                 const EncoderTestSequenceWithAllCategoriesCustomizedType& rhs)
        // Return 'true' if the specified 'lhs' and 'rhs' attribute objects do
        // not have the same value, and 'false' otherwise.  Two attribute
        // objects do not have the same value if one or more respective
        // attributes differ in values.
    {
        return lhs.d_value != rhs.d_value;
    }

    friend bsl::ostream& operator<<(
              bsl::ostream&                                             stream,
              const EncoderTestSequenceWithAllCategoriesCustomizedType& rhs)
        // Format the specified 'rhs' to the specified output 'stream' and
        // return a reference to the modifiable 'stream'.
    {
        return rhs.print(stream, 0, -1);
    }

    template <typename t_HASH_ALGORITHM>
    friend void hashAppend(
             t_HASH_ALGORITHM&                                         hashAlg,
             const EncoderTestSequenceWithAllCategoriesCustomizedType& object)
        // Pass the specified 'object' to the specified 'hashAlg'.  This
        // function integrates with the 'bslh' modular hashing system and
        // effectively provides a 'bsl::hash' specialization for
        // 'EncoderTestSequenceWithAllCategoriesCustomizedType'.
    {
        using bslh::hashAppend;
        hashAppend(hashAlg, object.toString());
    }
};

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

    // HIDDEN FRIENDS
    friend bsl::ostream& operator<<(bsl::ostream& stream, Value rhs)
        // Format the specified 'rhs' to the specified output 'stream' and
        // return a reference to the modifiable 'stream'.
    {
        return EncoderTestSequenceWithAllCategoriesEnumeration::print(stream, rhs);
    }
};

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


    // MANIPULATORS
    void reset();
        // Reset this object to the default value (i.e., its value upon
        // default construction).

    template <typename t_MANIPULATOR>
    int manipulateAttributes(t_MANIPULATOR& manipulator);
        // Invoke the specified 'manipulator' sequentially on the address of
        // each (modifiable) attribute of this object, supplying 'manipulator'
        // with the corresponding attribute information structure until such
        // invocation returns a non-zero value.  Return the value from the
        // last invocation of 'manipulator' (i.e., the invocation that
        // terminated the sequence).

    template <typename t_MANIPULATOR>
    int manipulateAttribute(t_MANIPULATOR& manipulator, int id);
        // Invoke the specified 'manipulator' on the address of
        // the (modifiable) attribute indicated by the specified 'id',
        // supplying 'manipulator' with the corresponding attribute
        // information structure.  Return the value returned from the
        // invocation of 'manipulator' if 'id' identifies an attribute of this
        // class, and -1 otherwise.

    template <typename t_MANIPULATOR>
    int manipulateAttribute(t_MANIPULATOR&  manipulator,
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
                        int           level          = 0,
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

    template <typename t_ACCESSOR>
    int accessAttributes(t_ACCESSOR& accessor) const;
        // Invoke the specified 'accessor' sequentially on each
        // (non-modifiable) attribute of this object, supplying 'accessor'
        // with the corresponding attribute information structure until such
        // invocation returns a non-zero value.  Return the value from the
        // last invocation of 'accessor' (i.e., the invocation that terminated
        // the sequence).

    template <typename t_ACCESSOR>
    int accessAttribute(t_ACCESSOR& accessor, int id) const;
        // Invoke the specified 'accessor' on the (non-modifiable) attribute
        // of this object indicated by the specified 'id', supplying 'accessor'
        // with the corresponding attribute information structure.  Return the
        // value returned from the invocation of 'accessor' if 'id' identifies
        // an attribute of this class, and -1 otherwise.

    template <typename t_ACCESSOR>
    int accessAttribute(t_ACCESSOR&   accessor,
                        const char *name,
                        int         nameLength) const;
        // Invoke the specified 'accessor' on the (non-modifiable) attribute
        // of this object indicated by the specified 'name' of the specified
        // 'nameLength', supplying 'accessor' with the corresponding attribute
        // information structure.  Return the value returned from the
        // invocation of 'accessor' if 'name' identifies an attribute of this
        // class, and -1 otherwise.

    int attribute() const;
        // Return the value of the "Attribute" attribute of this object.

    // HIDDEN FRIENDS
    friend bool operator==(
                       const EncoderTestSequenceWithAllCategoriesSequence& lhs,
                       const EncoderTestSequenceWithAllCategoriesSequence& rhs)
        // Return 'true' if the specified 'lhs' and 'rhs' attribute objects
        // have the same value, and 'false' otherwise.  Two attribute objects
        // have the same value if each respective attribute has the same value.
    {
        return lhs.attribute() == rhs.attribute();
    }

    friend bool operator!=(
                       const EncoderTestSequenceWithAllCategoriesSequence& lhs,
                       const EncoderTestSequenceWithAllCategoriesSequence& rhs)
        // Returns '!(lhs == rhs)'
    {
        return !(lhs == rhs);
    }

    friend bsl::ostream& operator<<(
                    bsl::ostream&                                       stream,
                    const EncoderTestSequenceWithAllCategoriesSequence& rhs)
        // Format the specified 'rhs' to the specified output 'stream' and
        // return a reference to the modifiable 'stream'.
    {
        return rhs.print(stream, 0, -1);
    }

    template <typename t_HASH_ALGORITHM>
    friend void hashAppend(
                   t_HASH_ALGORITHM&                                   hashAlg,
                   const EncoderTestSequenceWithAllCategoriesSequence& object)
        // Pass the specified 'object' to the specified 'hashAlg'.  This
        // function integrates with the 'bslh' modular hashing system and
        // effectively provides a 'bsl::hash' specialization for
        // 'EncoderTestSequenceWithAllCategoriesSequence'.
    {
        using bslh::hashAppend;
        hashAppend(hashAlg, object.attribute());
    }
};

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

    // MANIPULATORS
    void reset();
        // Reset this object to the default value (i.e., its value upon
        // default construction).

    template <typename t_MANIPULATOR>
    int manipulateAttributes(t_MANIPULATOR& manipulator);
        // Invoke the specified 'manipulator' sequentially on the address of
        // each (modifiable) attribute of this object, supplying 'manipulator'
        // with the corresponding attribute information structure until such
        // invocation returns a non-zero value.  Return the value from the
        // last invocation of 'manipulator' (i.e., the invocation that
        // terminated the sequence).

    template <typename t_MANIPULATOR>
    int manipulateAttribute(t_MANIPULATOR& manipulator, int id);
        // Invoke the specified 'manipulator' on the address of
        // the (modifiable) attribute indicated by the specified 'id',
        // supplying 'manipulator' with the corresponding attribute
        // information structure.  Return the value returned from the
        // invocation of 'manipulator' if 'id' identifies an attribute of this
        // class, and -1 otherwise.

    template <typename t_MANIPULATOR>
    int manipulateAttribute(t_MANIPULATOR&  manipulator,
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
                        int           level          = 0,
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

    template <typename t_ACCESSOR>
    int accessAttributes(t_ACCESSOR& accessor) const;
        // Invoke the specified 'accessor' sequentially on each
        // (non-modifiable) attribute of this object, supplying 'accessor'
        // with the corresponding attribute information structure until such
        // invocation returns a non-zero value.  Return the value from the
        // last invocation of 'accessor' (i.e., the invocation that terminated
        // the sequence).

    template <typename t_ACCESSOR>
    int accessAttribute(t_ACCESSOR& accessor, int id) const;
        // Invoke the specified 'accessor' on the (non-modifiable) attribute
        // of this object indicated by the specified 'id', supplying 'accessor'
        // with the corresponding attribute information structure.  Return the
        // value returned from the invocation of 'accessor' if 'id' identifies
        // an attribute of this class, and -1 otherwise.

    template <typename t_ACCESSOR>
    int accessAttribute(t_ACCESSOR&   accessor,
                        const char *name,
                        int         nameLength) const;
        // Invoke the specified 'accessor' on the (non-modifiable) attribute
        // of this object indicated by the specified 'name' of the specified
        // 'nameLength', supplying 'accessor' with the corresponding attribute
        // information structure.  Return the value returned from the
        // invocation of 'accessor' if 'name' identifies an attribute of this
        // class, and -1 otherwise.

    // HIDDEN FRIENDS
    friend bool operator==(const EncoderTestSequenceWithUntagged0&,
                           const EncoderTestSequenceWithUntagged0&)
        // Returns 'true' as this type has no attributes and so all objects of
        // this type are considered equal.
    {
        return true;
    }

    friend bool operator!=(const EncoderTestSequenceWithUntagged0& lhs,
                           const EncoderTestSequenceWithUntagged0& rhs)
        // Returns '!(lhs == rhs)'
    {
        return !(lhs == rhs);
    }

    friend bsl::ostream& operator<<(
                                bsl::ostream&                           stream,
                                const EncoderTestSequenceWithUntagged0& rhs)
        // Format the specified 'rhs' to the specified output 'stream' and
        // return a reference to the modifiable 'stream'.
    {
        return rhs.print(stream, 0, -1);
    }

    template <typename t_HASH_ALGORITHM>
    friend void hashAppend(t_HASH_ALGORITHM&                      ,
                           const EncoderTestSequenceWithUntagged0&)
        // Pass the specified 'object' to the specified 'hashAlg'.  This
        // function integrates with the 'bslh' modular hashing system and
        // effectively provides a 'bsl::hash' specialization for
        // 'EncoderTestSequenceWithUntagged0'.
    {
    }
};

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

    // MANIPULATORS
    void reset();
        // Reset this object to the default value (i.e., its value upon
        // default construction).

    template <typename t_MANIPULATOR>
    int manipulateAttributes(t_MANIPULATOR& manipulator);
        // Invoke the specified 'manipulator' sequentially on the address of
        // each (modifiable) attribute of this object, supplying 'manipulator'
        // with the corresponding attribute information structure until such
        // invocation returns a non-zero value.  Return the value from the
        // last invocation of 'manipulator' (i.e., the invocation that
        // terminated the sequence).

    template <typename t_MANIPULATOR>
    int manipulateAttribute(t_MANIPULATOR& manipulator, int id);
        // Invoke the specified 'manipulator' on the address of
        // the (modifiable) attribute indicated by the specified 'id',
        // supplying 'manipulator' with the corresponding attribute
        // information structure.  Return the value returned from the
        // invocation of 'manipulator' if 'id' identifies an attribute of this
        // class, and -1 otherwise.

    template <typename t_MANIPULATOR>
    int manipulateAttribute(t_MANIPULATOR&  manipulator,
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
                        int           level          = 0,
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

    template <typename t_ACCESSOR>
    int accessAttributes(t_ACCESSOR& accessor) const;
        // Invoke the specified 'accessor' sequentially on each
        // (non-modifiable) attribute of this object, supplying 'accessor'
        // with the corresponding attribute information structure until such
        // invocation returns a non-zero value.  Return the value from the
        // last invocation of 'accessor' (i.e., the invocation that terminated
        // the sequence).

    template <typename t_ACCESSOR>
    int accessAttribute(t_ACCESSOR& accessor, int id) const;
        // Invoke the specified 'accessor' on the (non-modifiable) attribute
        // of this object indicated by the specified 'id', supplying 'accessor'
        // with the corresponding attribute information structure.  Return the
        // value returned from the invocation of 'accessor' if 'id' identifies
        // an attribute of this class, and -1 otherwise.

    template <typename t_ACCESSOR>
    int accessAttribute(t_ACCESSOR&   accessor,
                        const char *name,
                        int         nameLength) const;
        // Invoke the specified 'accessor' on the (non-modifiable) attribute
        // of this object indicated by the specified 'name' of the specified
        // 'nameLength', supplying 'accessor' with the corresponding attribute
        // information structure.  Return the value returned from the
        // invocation of 'accessor' if 'name' identifies an attribute of this
        // class, and -1 otherwise.

    // HIDDEN FRIENDS
    friend bool operator==(const EncoderTestSequenceWithUntagged10Sequence&,
                           const EncoderTestSequenceWithUntagged10Sequence&)
        // Returns 'true' as this type has no attributes and so all objects of
        // this type are considered equal.
    {
        return true;
    }

    friend bool operator!=(
                          const EncoderTestSequenceWithUntagged10Sequence& lhs,
                          const EncoderTestSequenceWithUntagged10Sequence& rhs)
        // Returns '!(lhs == rhs)'
    {
        return !(lhs == rhs);
    }

    friend bsl::ostream& operator<<(
                       bsl::ostream&                                    stream,
                       const EncoderTestSequenceWithUntagged10Sequence& rhs)
        // Format the specified 'rhs' to the specified output 'stream' and
        // return a reference to the modifiable 'stream'.
    {
        return rhs.print(stream, 0, -1);
    }

    template <typename t_HASH_ALGORITHM>
    friend void hashAppend(t_HASH_ALGORITHM&                               ,
                           const EncoderTestSequenceWithUntagged10Sequence&)
        // Pass the specified 'object' to the specified 'hashAlg'.  This
        // function integrates with the 'bslh' modular hashing system and
        // effectively provides a 'bsl::hash' specialization for
        // 'EncoderTestSequenceWithUntagged10Sequence'.
    {
    }
};

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

    // MANIPULATORS
    void reset();
        // Reset this object to the default value (i.e., its value upon
        // default construction).

    template <typename t_MANIPULATOR>
    int manipulateAttributes(t_MANIPULATOR& manipulator);
        // Invoke the specified 'manipulator' sequentially on the address of
        // each (modifiable) attribute of this object, supplying 'manipulator'
        // with the corresponding attribute information structure until such
        // invocation returns a non-zero value.  Return the value from the
        // last invocation of 'manipulator' (i.e., the invocation that
        // terminated the sequence).

    template <typename t_MANIPULATOR>
    int manipulateAttribute(t_MANIPULATOR& manipulator, int id);
        // Invoke the specified 'manipulator' on the address of
        // the (modifiable) attribute indicated by the specified 'id',
        // supplying 'manipulator' with the corresponding attribute
        // information structure.  Return the value returned from the
        // invocation of 'manipulator' if 'id' identifies an attribute of this
        // class, and -1 otherwise.

    template <typename t_MANIPULATOR>
    int manipulateAttribute(t_MANIPULATOR&  manipulator,
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
                        int           level          = 0,
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

    template <typename t_ACCESSOR>
    int accessAttributes(t_ACCESSOR& accessor) const;
        // Invoke the specified 'accessor' sequentially on each
        // (non-modifiable) attribute of this object, supplying 'accessor'
        // with the corresponding attribute information structure until such
        // invocation returns a non-zero value.  Return the value from the
        // last invocation of 'accessor' (i.e., the invocation that terminated
        // the sequence).

    template <typename t_ACCESSOR>
    int accessAttribute(t_ACCESSOR& accessor, int id) const;
        // Invoke the specified 'accessor' on the (non-modifiable) attribute
        // of this object indicated by the specified 'id', supplying 'accessor'
        // with the corresponding attribute information structure.  Return the
        // value returned from the invocation of 'accessor' if 'id' identifies
        // an attribute of this class, and -1 otherwise.

    template <typename t_ACCESSOR>
    int accessAttribute(t_ACCESSOR&   accessor,
                        const char *name,
                        int         nameLength) const;
        // Invoke the specified 'accessor' on the (non-modifiable) attribute
        // of this object indicated by the specified 'name' of the specified
        // 'nameLength', supplying 'accessor' with the corresponding attribute
        // information structure.  Return the value returned from the
        // invocation of 'accessor' if 'name' identifies an attribute of this
        // class, and -1 otherwise.

    // HIDDEN FRIENDS
    friend bool operator==(const EncoderTestSequenceWithUntagged11Sequence&,
                           const EncoderTestSequenceWithUntagged11Sequence&)
        // Returns 'true' as this type has no attributes and so all objects of
        // this type are considered equal.
    {
        return true;
    }

    friend bool operator!=(
                          const EncoderTestSequenceWithUntagged11Sequence& lhs,
                          const EncoderTestSequenceWithUntagged11Sequence& rhs)
        // Returns '!(lhs == rhs)'
    {
        return !(lhs == rhs);
    }

    friend bsl::ostream& operator<<(
                       bsl::ostream&                                    stream,
                       const EncoderTestSequenceWithUntagged11Sequence& rhs)
        // Format the specified 'rhs' to the specified output 'stream' and
        // return a reference to the modifiable 'stream'.
    {
        return rhs.print(stream, 0, -1);
    }

    template <typename t_HASH_ALGORITHM>
    friend void hashAppend(t_HASH_ALGORITHM&                               ,
                           const EncoderTestSequenceWithUntagged11Sequence&)
        // Pass the specified 'object' to the specified 'hashAlg'.  This
        // function integrates with the 'bslh' modular hashing system and
        // effectively provides a 'bsl::hash' specialization for
        // 'EncoderTestSequenceWithUntagged11Sequence'.
    {
    }
};

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

    // MANIPULATORS
    void reset();
        // Reset this object to the default value (i.e., its value upon
        // default construction).

    template <typename t_MANIPULATOR>
    int manipulateAttributes(t_MANIPULATOR& manipulator);
        // Invoke the specified 'manipulator' sequentially on the address of
        // each (modifiable) attribute of this object, supplying 'manipulator'
        // with the corresponding attribute information structure until such
        // invocation returns a non-zero value.  Return the value from the
        // last invocation of 'manipulator' (i.e., the invocation that
        // terminated the sequence).

    template <typename t_MANIPULATOR>
    int manipulateAttribute(t_MANIPULATOR& manipulator, int id);
        // Invoke the specified 'manipulator' on the address of
        // the (modifiable) attribute indicated by the specified 'id',
        // supplying 'manipulator' with the corresponding attribute
        // information structure.  Return the value returned from the
        // invocation of 'manipulator' if 'id' identifies an attribute of this
        // class, and -1 otherwise.

    template <typename t_MANIPULATOR>
    int manipulateAttribute(t_MANIPULATOR&  manipulator,
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
                        int           level          = 0,
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

    template <typename t_ACCESSOR>
    int accessAttributes(t_ACCESSOR& accessor) const;
        // Invoke the specified 'accessor' sequentially on each
        // (non-modifiable) attribute of this object, supplying 'accessor'
        // with the corresponding attribute information structure until such
        // invocation returns a non-zero value.  Return the value from the
        // last invocation of 'accessor' (i.e., the invocation that terminated
        // the sequence).

    template <typename t_ACCESSOR>
    int accessAttribute(t_ACCESSOR& accessor, int id) const;
        // Invoke the specified 'accessor' on the (non-modifiable) attribute
        // of this object indicated by the specified 'id', supplying 'accessor'
        // with the corresponding attribute information structure.  Return the
        // value returned from the invocation of 'accessor' if 'id' identifies
        // an attribute of this class, and -1 otherwise.

    template <typename t_ACCESSOR>
    int accessAttribute(t_ACCESSOR&   accessor,
                        const char *name,
                        int         nameLength) const;
        // Invoke the specified 'accessor' on the (non-modifiable) attribute
        // of this object indicated by the specified 'name' of the specified
        // 'nameLength', supplying 'accessor' with the corresponding attribute
        // information structure.  Return the value returned from the
        // invocation of 'accessor' if 'name' identifies an attribute of this
        // class, and -1 otherwise.

    // HIDDEN FRIENDS
    friend bool operator==(const EncoderTestSequenceWithUntagged11Sequence1&,
                           const EncoderTestSequenceWithUntagged11Sequence1&)
        // Returns 'true' as this type has no attributes and so all objects of
        // this type are considered equal.
    {
        return true;
    }

    friend bool operator!=(
                         const EncoderTestSequenceWithUntagged11Sequence1& lhs,
                         const EncoderTestSequenceWithUntagged11Sequence1& rhs)
        // Returns '!(lhs == rhs)'
    {
        return !(lhs == rhs);
    }

    friend bsl::ostream& operator<<(
                      bsl::ostream&                                     stream,
                      const EncoderTestSequenceWithUntagged11Sequence1& rhs)
        // Format the specified 'rhs' to the specified output 'stream' and
        // return a reference to the modifiable 'stream'.
    {
        return rhs.print(stream, 0, -1);
    }

    template <typename t_HASH_ALGORITHM>
    friend void hashAppend(t_HASH_ALGORITHM&                                ,
                           const EncoderTestSequenceWithUntagged11Sequence1&)
        // Pass the specified 'object' to the specified 'hashAlg'.  This
        // function integrates with the 'bslh' modular hashing system and
        // effectively provides a 'bsl::hash' specialization for
        // 'EncoderTestSequenceWithUntagged11Sequence1'.
    {
    }
};

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

    // MANIPULATORS
    void reset();
        // Reset this object to the default value (i.e., its value upon
        // default construction).

    template <typename t_MANIPULATOR>
    int manipulateAttributes(t_MANIPULATOR& manipulator);
        // Invoke the specified 'manipulator' sequentially on the address of
        // each (modifiable) attribute of this object, supplying 'manipulator'
        // with the corresponding attribute information structure until such
        // invocation returns a non-zero value.  Return the value from the
        // last invocation of 'manipulator' (i.e., the invocation that
        // terminated the sequence).

    template <typename t_MANIPULATOR>
    int manipulateAttribute(t_MANIPULATOR& manipulator, int id);
        // Invoke the specified 'manipulator' on the address of
        // the (modifiable) attribute indicated by the specified 'id',
        // supplying 'manipulator' with the corresponding attribute
        // information structure.  Return the value returned from the
        // invocation of 'manipulator' if 'id' identifies an attribute of this
        // class, and -1 otherwise.

    template <typename t_MANIPULATOR>
    int manipulateAttribute(t_MANIPULATOR&  manipulator,
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
                        int           level          = 0,
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

    template <typename t_ACCESSOR>
    int accessAttributes(t_ACCESSOR& accessor) const;
        // Invoke the specified 'accessor' sequentially on each
        // (non-modifiable) attribute of this object, supplying 'accessor'
        // with the corresponding attribute information structure until such
        // invocation returns a non-zero value.  Return the value from the
        // last invocation of 'accessor' (i.e., the invocation that terminated
        // the sequence).

    template <typename t_ACCESSOR>
    int accessAttribute(t_ACCESSOR& accessor, int id) const;
        // Invoke the specified 'accessor' on the (non-modifiable) attribute
        // of this object indicated by the specified 'id', supplying 'accessor'
        // with the corresponding attribute information structure.  Return the
        // value returned from the invocation of 'accessor' if 'id' identifies
        // an attribute of this class, and -1 otherwise.

    template <typename t_ACCESSOR>
    int accessAttribute(t_ACCESSOR&   accessor,
                        const char *name,
                        int         nameLength) const;
        // Invoke the specified 'accessor' on the (non-modifiable) attribute
        // of this object indicated by the specified 'name' of the specified
        // 'nameLength', supplying 'accessor' with the corresponding attribute
        // information structure.  Return the value returned from the
        // invocation of 'accessor' if 'name' identifies an attribute of this
        // class, and -1 otherwise.

    // HIDDEN FRIENDS
    friend bool operator==(const EncoderTestSequenceWithUntagged12Sequence&,
                           const EncoderTestSequenceWithUntagged12Sequence&)
        // Returns 'true' as this type has no attributes and so all objects of
        // this type are considered equal.
    {
        return true;
    }

    friend bool operator!=(
                          const EncoderTestSequenceWithUntagged12Sequence& lhs,
                          const EncoderTestSequenceWithUntagged12Sequence& rhs)
        // Returns '!(lhs == rhs)'
    {
        return !(lhs == rhs);
    }

    friend bsl::ostream& operator<<(
                       bsl::ostream&                                    stream,
                       const EncoderTestSequenceWithUntagged12Sequence& rhs)
        // Format the specified 'rhs' to the specified output 'stream' and
        // return a reference to the modifiable 'stream'.
    {
        return rhs.print(stream, 0, -1);
    }

    template <typename t_HASH_ALGORITHM>
    friend void hashAppend(t_HASH_ALGORITHM&                               ,
                           const EncoderTestSequenceWithUntagged12Sequence&)
        // Pass the specified 'object' to the specified 'hashAlg'.  This
        // function integrates with the 'bslh' modular hashing system and
        // effectively provides a 'bsl::hash' specialization for
        // 'EncoderTestSequenceWithUntagged12Sequence'.
    {
    }
};

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

    // MANIPULATORS
    void reset();
        // Reset this object to the default value (i.e., its value upon
        // default construction).

    template <typename t_MANIPULATOR>
    int manipulateAttributes(t_MANIPULATOR& manipulator);
        // Invoke the specified 'manipulator' sequentially on the address of
        // each (modifiable) attribute of this object, supplying 'manipulator'
        // with the corresponding attribute information structure until such
        // invocation returns a non-zero value.  Return the value from the
        // last invocation of 'manipulator' (i.e., the invocation that
        // terminated the sequence).

    template <typename t_MANIPULATOR>
    int manipulateAttribute(t_MANIPULATOR& manipulator, int id);
        // Invoke the specified 'manipulator' on the address of
        // the (modifiable) attribute indicated by the specified 'id',
        // supplying 'manipulator' with the corresponding attribute
        // information structure.  Return the value returned from the
        // invocation of 'manipulator' if 'id' identifies an attribute of this
        // class, and -1 otherwise.

    template <typename t_MANIPULATOR>
    int manipulateAttribute(t_MANIPULATOR&  manipulator,
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
                        int           level          = 0,
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

    template <typename t_ACCESSOR>
    int accessAttributes(t_ACCESSOR& accessor) const;
        // Invoke the specified 'accessor' sequentially on each
        // (non-modifiable) attribute of this object, supplying 'accessor'
        // with the corresponding attribute information structure until such
        // invocation returns a non-zero value.  Return the value from the
        // last invocation of 'accessor' (i.e., the invocation that terminated
        // the sequence).

    template <typename t_ACCESSOR>
    int accessAttribute(t_ACCESSOR& accessor, int id) const;
        // Invoke the specified 'accessor' on the (non-modifiable) attribute
        // of this object indicated by the specified 'id', supplying 'accessor'
        // with the corresponding attribute information structure.  Return the
        // value returned from the invocation of 'accessor' if 'id' identifies
        // an attribute of this class, and -1 otherwise.

    template <typename t_ACCESSOR>
    int accessAttribute(t_ACCESSOR&   accessor,
                        const char *name,
                        int         nameLength) const;
        // Invoke the specified 'accessor' on the (non-modifiable) attribute
        // of this object indicated by the specified 'name' of the specified
        // 'nameLength', supplying 'accessor' with the corresponding attribute
        // information structure.  Return the value returned from the
        // invocation of 'accessor' if 'name' identifies an attribute of this
        // class, and -1 otherwise.

    // HIDDEN FRIENDS
    friend bool operator==(const EncoderTestSequenceWithUntagged13Sequence&,
                           const EncoderTestSequenceWithUntagged13Sequence&)
        // Returns 'true' as this type has no attributes and so all objects of
        // this type are considered equal.
    {
        return true;
    }

    friend bool operator!=(
                          const EncoderTestSequenceWithUntagged13Sequence& lhs,
                          const EncoderTestSequenceWithUntagged13Sequence& rhs)
        // Returns '!(lhs == rhs)'
    {
        return !(lhs == rhs);
    }

    friend bsl::ostream& operator<<(
                       bsl::ostream&                                    stream,
                       const EncoderTestSequenceWithUntagged13Sequence& rhs)
        // Format the specified 'rhs' to the specified output 'stream' and
        // return a reference to the modifiable 'stream'.
    {
        return rhs.print(stream, 0, -1);
    }

    template <typename t_HASH_ALGORITHM>
    friend void hashAppend(t_HASH_ALGORITHM&                               ,
                           const EncoderTestSequenceWithUntagged13Sequence&)
        // Pass the specified 'object' to the specified 'hashAlg'.  This
        // function integrates with the 'bslh' modular hashing system and
        // effectively provides a 'bsl::hash' specialization for
        // 'EncoderTestSequenceWithUntagged13Sequence'.
    {
    }
};

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

    // PRIVATE ACCESSORS
    template <typename t_HASH_ALGORITHM>
    void hashAppendImpl(t_HASH_ALGORITHM& hashAlgorithm) const;

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


    // MANIPULATORS
    void reset();
        // Reset this object to the default value (i.e., its value upon
        // default construction).

    template <typename t_MANIPULATOR>
    int manipulateAttributes(t_MANIPULATOR& manipulator);
        // Invoke the specified 'manipulator' sequentially on the address of
        // each (modifiable) attribute of this object, supplying 'manipulator'
        // with the corresponding attribute information structure until such
        // invocation returns a non-zero value.  Return the value from the
        // last invocation of 'manipulator' (i.e., the invocation that
        // terminated the sequence).

    template <typename t_MANIPULATOR>
    int manipulateAttribute(t_MANIPULATOR& manipulator, int id);
        // Invoke the specified 'manipulator' on the address of
        // the (modifiable) attribute indicated by the specified 'id',
        // supplying 'manipulator' with the corresponding attribute
        // information structure.  Return the value returned from the
        // invocation of 'manipulator' if 'id' identifies an attribute of this
        // class, and -1 otherwise.

    template <typename t_MANIPULATOR>
    int manipulateAttribute(t_MANIPULATOR&  manipulator,
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
                        int           level          = 0,
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

    template <typename t_ACCESSOR>
    int accessAttributes(t_ACCESSOR& accessor) const;
        // Invoke the specified 'accessor' sequentially on each
        // (non-modifiable) attribute of this object, supplying 'accessor'
        // with the corresponding attribute information structure until such
        // invocation returns a non-zero value.  Return the value from the
        // last invocation of 'accessor' (i.e., the invocation that terminated
        // the sequence).

    template <typename t_ACCESSOR>
    int accessAttribute(t_ACCESSOR& accessor, int id) const;
        // Invoke the specified 'accessor' on the (non-modifiable) attribute
        // of this object indicated by the specified 'id', supplying 'accessor'
        // with the corresponding attribute information structure.  Return the
        // value returned from the invocation of 'accessor' if 'id' identifies
        // an attribute of this class, and -1 otherwise.

    template <typename t_ACCESSOR>
    int accessAttribute(t_ACCESSOR&   accessor,
                        const char *name,
                        int         nameLength) const;
        // Invoke the specified 'accessor' on the (non-modifiable) attribute
        // of this object indicated by the specified 'name' of the specified
        // 'nameLength', supplying 'accessor' with the corresponding attribute
        // information structure.  Return the value returned from the
        // invocation of 'accessor' if 'name' identifies an attribute of this
        // class, and -1 otherwise.

    int attribute0() const;
        // Return the value of the "Attribute0" attribute of this object.

    int attribute1() const;
        // Return the value of the "Attribute1" attribute of this object.

    int attribute2() const;
        // Return the value of the "Attribute2" attribute of this object.

    // HIDDEN FRIENDS
    friend bool operator==(const EncoderTestSequenceWithUntagged14& lhs,
                           const EncoderTestSequenceWithUntagged14& rhs)
        // Return 'true' if the specified 'lhs' and 'rhs' attribute objects
        // have the same value, and 'false' otherwise.  Two attribute objects
        // have the same value if each respective attribute has the same value.
    {
        return lhs.attribute0() == rhs.attribute0() &&
               lhs.attribute1() == rhs.attribute1() &&
               lhs.attribute2() == rhs.attribute2();
    }

    friend bool operator!=(const EncoderTestSequenceWithUntagged14& lhs,
                           const EncoderTestSequenceWithUntagged14& rhs)
        // Returns '!(lhs == rhs)'
    {
        return !(lhs == rhs);
    }

    friend bsl::ostream& operator<<(
                               bsl::ostream&                            stream,
                               const EncoderTestSequenceWithUntagged14& rhs)
        // Format the specified 'rhs' to the specified output 'stream' and
        // return a reference to the modifiable 'stream'.
    {
        return rhs.print(stream, 0, -1);
    }

    template <typename t_HASH_ALGORITHM>
    friend void hashAppend(t_HASH_ALGORITHM&                        hashAlg,
                           const EncoderTestSequenceWithUntagged14& object)
        // Pass the specified 'object' to the specified 'hashAlg'.  This
        // function integrates with the 'bslh' modular hashing system and
        // effectively provides a 'bsl::hash' specialization for
        // 'EncoderTestSequenceWithUntagged14'.
    {
        object.hashAppendImpl(hashAlg);
    }
};

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

    // MANIPULATORS
    void reset();
        // Reset this object to the default value (i.e., its value upon
        // default construction).

    template <typename t_MANIPULATOR>
    int manipulateAttributes(t_MANIPULATOR& manipulator);
        // Invoke the specified 'manipulator' sequentially on the address of
        // each (modifiable) attribute of this object, supplying 'manipulator'
        // with the corresponding attribute information structure until such
        // invocation returns a non-zero value.  Return the value from the
        // last invocation of 'manipulator' (i.e., the invocation that
        // terminated the sequence).

    template <typename t_MANIPULATOR>
    int manipulateAttribute(t_MANIPULATOR& manipulator, int id);
        // Invoke the specified 'manipulator' on the address of
        // the (modifiable) attribute indicated by the specified 'id',
        // supplying 'manipulator' with the corresponding attribute
        // information structure.  Return the value returned from the
        // invocation of 'manipulator' if 'id' identifies an attribute of this
        // class, and -1 otherwise.

    template <typename t_MANIPULATOR>
    int manipulateAttribute(t_MANIPULATOR&  manipulator,
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
                        int           level          = 0,
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

    template <typename t_ACCESSOR>
    int accessAttributes(t_ACCESSOR& accessor) const;
        // Invoke the specified 'accessor' sequentially on each
        // (non-modifiable) attribute of this object, supplying 'accessor'
        // with the corresponding attribute information structure until such
        // invocation returns a non-zero value.  Return the value from the
        // last invocation of 'accessor' (i.e., the invocation that terminated
        // the sequence).

    template <typename t_ACCESSOR>
    int accessAttribute(t_ACCESSOR& accessor, int id) const;
        // Invoke the specified 'accessor' on the (non-modifiable) attribute
        // of this object indicated by the specified 'id', supplying 'accessor'
        // with the corresponding attribute information structure.  Return the
        // value returned from the invocation of 'accessor' if 'id' identifies
        // an attribute of this class, and -1 otherwise.

    template <typename t_ACCESSOR>
    int accessAttribute(t_ACCESSOR&   accessor,
                        const char *name,
                        int         nameLength) const;
        // Invoke the specified 'accessor' on the (non-modifiable) attribute
        // of this object indicated by the specified 'name' of the specified
        // 'nameLength', supplying 'accessor' with the corresponding attribute
        // information structure.  Return the value returned from the
        // invocation of 'accessor' if 'name' identifies an attribute of this
        // class, and -1 otherwise.

    // HIDDEN FRIENDS
    friend bool operator==(const EncoderTestSequenceWithUntagged1Sequence&,
                           const EncoderTestSequenceWithUntagged1Sequence&)
        // Returns 'true' as this type has no attributes and so all objects of
        // this type are considered equal.
    {
        return true;
    }

    friend bool operator!=(const EncoderTestSequenceWithUntagged1Sequence& lhs,
                           const EncoderTestSequenceWithUntagged1Sequence& rhs)
        // Returns '!(lhs == rhs)'
    {
        return !(lhs == rhs);
    }

    friend bsl::ostream& operator<<(
                        bsl::ostream&                                   stream,
                        const EncoderTestSequenceWithUntagged1Sequence& rhs)
        // Format the specified 'rhs' to the specified output 'stream' and
        // return a reference to the modifiable 'stream'.
    {
        return rhs.print(stream, 0, -1);
    }

    template <typename t_HASH_ALGORITHM>
    friend void hashAppend(t_HASH_ALGORITHM&                              ,
                           const EncoderTestSequenceWithUntagged1Sequence&)
        // Pass the specified 'object' to the specified 'hashAlg'.  This
        // function integrates with the 'bslh' modular hashing system and
        // effectively provides a 'bsl::hash' specialization for
        // 'EncoderTestSequenceWithUntagged1Sequence'.
    {
    }
};

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


    // MANIPULATORS
    void reset();
        // Reset this object to the default value (i.e., its value upon
        // default construction).

    template <typename t_MANIPULATOR>
    int manipulateAttributes(t_MANIPULATOR& manipulator);
        // Invoke the specified 'manipulator' sequentially on the address of
        // each (modifiable) attribute of this object, supplying 'manipulator'
        // with the corresponding attribute information structure until such
        // invocation returns a non-zero value.  Return the value from the
        // last invocation of 'manipulator' (i.e., the invocation that
        // terminated the sequence).

    template <typename t_MANIPULATOR>
    int manipulateAttribute(t_MANIPULATOR& manipulator, int id);
        // Invoke the specified 'manipulator' on the address of
        // the (modifiable) attribute indicated by the specified 'id',
        // supplying 'manipulator' with the corresponding attribute
        // information structure.  Return the value returned from the
        // invocation of 'manipulator' if 'id' identifies an attribute of this
        // class, and -1 otherwise.

    template <typename t_MANIPULATOR>
    int manipulateAttribute(t_MANIPULATOR&  manipulator,
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
                        int           level          = 0,
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

    template <typename t_ACCESSOR>
    int accessAttributes(t_ACCESSOR& accessor) const;
        // Invoke the specified 'accessor' sequentially on each
        // (non-modifiable) attribute of this object, supplying 'accessor'
        // with the corresponding attribute information structure until such
        // invocation returns a non-zero value.  Return the value from the
        // last invocation of 'accessor' (i.e., the invocation that terminated
        // the sequence).

    template <typename t_ACCESSOR>
    int accessAttribute(t_ACCESSOR& accessor, int id) const;
        // Invoke the specified 'accessor' on the (non-modifiable) attribute
        // of this object indicated by the specified 'id', supplying 'accessor'
        // with the corresponding attribute information structure.  Return the
        // value returned from the invocation of 'accessor' if 'id' identifies
        // an attribute of this class, and -1 otherwise.

    template <typename t_ACCESSOR>
    int accessAttribute(t_ACCESSOR&   accessor,
                        const char *name,
                        int         nameLength) const;
        // Invoke the specified 'accessor' on the (non-modifiable) attribute
        // of this object indicated by the specified 'name' of the specified
        // 'nameLength', supplying 'accessor' with the corresponding attribute
        // information structure.  Return the value returned from the
        // invocation of 'accessor' if 'name' identifies an attribute of this
        // class, and -1 otherwise.

    int attribute0() const;
        // Return the value of the "Attribute0" attribute of this object.

    // HIDDEN FRIENDS
    friend bool operator==(const EncoderTestSequenceWithUntagged2& lhs,
                           const EncoderTestSequenceWithUntagged2& rhs)
        // Return 'true' if the specified 'lhs' and 'rhs' attribute objects
        // have the same value, and 'false' otherwise.  Two attribute objects
        // have the same value if each respective attribute has the same value.
    {
        return lhs.attribute0() == rhs.attribute0();
    }

    friend bool operator!=(const EncoderTestSequenceWithUntagged2& lhs,
                           const EncoderTestSequenceWithUntagged2& rhs)
        // Returns '!(lhs == rhs)'
    {
        return !(lhs == rhs);
    }

    friend bsl::ostream& operator<<(
                                bsl::ostream&                           stream,
                                const EncoderTestSequenceWithUntagged2& rhs)
        // Format the specified 'rhs' to the specified output 'stream' and
        // return a reference to the modifiable 'stream'.
    {
        return rhs.print(stream, 0, -1);
    }

    template <typename t_HASH_ALGORITHM>
    friend void hashAppend(t_HASH_ALGORITHM&                       hashAlg,
                           const EncoderTestSequenceWithUntagged2& object)
        // Pass the specified 'object' to the specified 'hashAlg'.  This
        // function integrates with the 'bslh' modular hashing system and
        // effectively provides a 'bsl::hash' specialization for
        // 'EncoderTestSequenceWithUntagged2'.
    {
        using bslh::hashAppend;
        hashAppend(hashAlg, object.attribute0());
    }
};

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

    // MANIPULATORS
    void reset();
        // Reset this object to the default value (i.e., its value upon
        // default construction).

    template <typename t_MANIPULATOR>
    int manipulateAttributes(t_MANIPULATOR& manipulator);
        // Invoke the specified 'manipulator' sequentially on the address of
        // each (modifiable) attribute of this object, supplying 'manipulator'
        // with the corresponding attribute information structure until such
        // invocation returns a non-zero value.  Return the value from the
        // last invocation of 'manipulator' (i.e., the invocation that
        // terminated the sequence).

    template <typename t_MANIPULATOR>
    int manipulateAttribute(t_MANIPULATOR& manipulator, int id);
        // Invoke the specified 'manipulator' on the address of
        // the (modifiable) attribute indicated by the specified 'id',
        // supplying 'manipulator' with the corresponding attribute
        // information structure.  Return the value returned from the
        // invocation of 'manipulator' if 'id' identifies an attribute of this
        // class, and -1 otherwise.

    template <typename t_MANIPULATOR>
    int manipulateAttribute(t_MANIPULATOR&  manipulator,
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
                        int           level          = 0,
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

    template <typename t_ACCESSOR>
    int accessAttributes(t_ACCESSOR& accessor) const;
        // Invoke the specified 'accessor' sequentially on each
        // (non-modifiable) attribute of this object, supplying 'accessor'
        // with the corresponding attribute information structure until such
        // invocation returns a non-zero value.  Return the value from the
        // last invocation of 'accessor' (i.e., the invocation that terminated
        // the sequence).

    template <typename t_ACCESSOR>
    int accessAttribute(t_ACCESSOR& accessor, int id) const;
        // Invoke the specified 'accessor' on the (non-modifiable) attribute
        // of this object indicated by the specified 'id', supplying 'accessor'
        // with the corresponding attribute information structure.  Return the
        // value returned from the invocation of 'accessor' if 'id' identifies
        // an attribute of this class, and -1 otherwise.

    template <typename t_ACCESSOR>
    int accessAttribute(t_ACCESSOR&   accessor,
                        const char *name,
                        int         nameLength) const;
        // Invoke the specified 'accessor' on the (non-modifiable) attribute
        // of this object indicated by the specified 'name' of the specified
        // 'nameLength', supplying 'accessor' with the corresponding attribute
        // information structure.  Return the value returned from the
        // invocation of 'accessor' if 'name' identifies an attribute of this
        // class, and -1 otherwise.

    // HIDDEN FRIENDS
    friend bool operator==(const EncoderTestSequenceWithUntagged3Sequence&,
                           const EncoderTestSequenceWithUntagged3Sequence&)
        // Returns 'true' as this type has no attributes and so all objects of
        // this type are considered equal.
    {
        return true;
    }

    friend bool operator!=(const EncoderTestSequenceWithUntagged3Sequence& lhs,
                           const EncoderTestSequenceWithUntagged3Sequence& rhs)
        // Returns '!(lhs == rhs)'
    {
        return !(lhs == rhs);
    }

    friend bsl::ostream& operator<<(
                        bsl::ostream&                                   stream,
                        const EncoderTestSequenceWithUntagged3Sequence& rhs)
        // Format the specified 'rhs' to the specified output 'stream' and
        // return a reference to the modifiable 'stream'.
    {
        return rhs.print(stream, 0, -1);
    }

    template <typename t_HASH_ALGORITHM>
    friend void hashAppend(t_HASH_ALGORITHM&                              ,
                           const EncoderTestSequenceWithUntagged3Sequence&)
        // Pass the specified 'object' to the specified 'hashAlg'.  This
        // function integrates with the 'bslh' modular hashing system and
        // effectively provides a 'bsl::hash' specialization for
        // 'EncoderTestSequenceWithUntagged3Sequence'.
    {
    }
};

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

    // MANIPULATORS
    void reset();
        // Reset this object to the default value (i.e., its value upon
        // default construction).

    template <typename t_MANIPULATOR>
    int manipulateAttributes(t_MANIPULATOR& manipulator);
        // Invoke the specified 'manipulator' sequentially on the address of
        // each (modifiable) attribute of this object, supplying 'manipulator'
        // with the corresponding attribute information structure until such
        // invocation returns a non-zero value.  Return the value from the
        // last invocation of 'manipulator' (i.e., the invocation that
        // terminated the sequence).

    template <typename t_MANIPULATOR>
    int manipulateAttribute(t_MANIPULATOR& manipulator, int id);
        // Invoke the specified 'manipulator' on the address of
        // the (modifiable) attribute indicated by the specified 'id',
        // supplying 'manipulator' with the corresponding attribute
        // information structure.  Return the value returned from the
        // invocation of 'manipulator' if 'id' identifies an attribute of this
        // class, and -1 otherwise.

    template <typename t_MANIPULATOR>
    int manipulateAttribute(t_MANIPULATOR&  manipulator,
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
                        int           level          = 0,
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

    template <typename t_ACCESSOR>
    int accessAttributes(t_ACCESSOR& accessor) const;
        // Invoke the specified 'accessor' sequentially on each
        // (non-modifiable) attribute of this object, supplying 'accessor'
        // with the corresponding attribute information structure until such
        // invocation returns a non-zero value.  Return the value from the
        // last invocation of 'accessor' (i.e., the invocation that terminated
        // the sequence).

    template <typename t_ACCESSOR>
    int accessAttribute(t_ACCESSOR& accessor, int id) const;
        // Invoke the specified 'accessor' on the (non-modifiable) attribute
        // of this object indicated by the specified 'id', supplying 'accessor'
        // with the corresponding attribute information structure.  Return the
        // value returned from the invocation of 'accessor' if 'id' identifies
        // an attribute of this class, and -1 otherwise.

    template <typename t_ACCESSOR>
    int accessAttribute(t_ACCESSOR&   accessor,
                        const char *name,
                        int         nameLength) const;
        // Invoke the specified 'accessor' on the (non-modifiable) attribute
        // of this object indicated by the specified 'name' of the specified
        // 'nameLength', supplying 'accessor' with the corresponding attribute
        // information structure.  Return the value returned from the
        // invocation of 'accessor' if 'name' identifies an attribute of this
        // class, and -1 otherwise.

    // HIDDEN FRIENDS
    friend bool operator==(const EncoderTestSequenceWithUntagged3Sequence1&,
                           const EncoderTestSequenceWithUntagged3Sequence1&)
        // Returns 'true' as this type has no attributes and so all objects of
        // this type are considered equal.
    {
        return true;
    }

    friend bool operator!=(
                          const EncoderTestSequenceWithUntagged3Sequence1& lhs,
                          const EncoderTestSequenceWithUntagged3Sequence1& rhs)
        // Returns '!(lhs == rhs)'
    {
        return !(lhs == rhs);
    }

    friend bsl::ostream& operator<<(
                       bsl::ostream&                                    stream,
                       const EncoderTestSequenceWithUntagged3Sequence1& rhs)
        // Format the specified 'rhs' to the specified output 'stream' and
        // return a reference to the modifiable 'stream'.
    {
        return rhs.print(stream, 0, -1);
    }

    template <typename t_HASH_ALGORITHM>
    friend void hashAppend(t_HASH_ALGORITHM&                               ,
                           const EncoderTestSequenceWithUntagged3Sequence1&)
        // Pass the specified 'object' to the specified 'hashAlg'.  This
        // function integrates with the 'bslh' modular hashing system and
        // effectively provides a 'bsl::hash' specialization for
        // 'EncoderTestSequenceWithUntagged3Sequence1'.
    {
    }
};

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

    // MANIPULATORS
    void reset();
        // Reset this object to the default value (i.e., its value upon
        // default construction).

    template <typename t_MANIPULATOR>
    int manipulateAttributes(t_MANIPULATOR& manipulator);
        // Invoke the specified 'manipulator' sequentially on the address of
        // each (modifiable) attribute of this object, supplying 'manipulator'
        // with the corresponding attribute information structure until such
        // invocation returns a non-zero value.  Return the value from the
        // last invocation of 'manipulator' (i.e., the invocation that
        // terminated the sequence).

    template <typename t_MANIPULATOR>
    int manipulateAttribute(t_MANIPULATOR& manipulator, int id);
        // Invoke the specified 'manipulator' on the address of
        // the (modifiable) attribute indicated by the specified 'id',
        // supplying 'manipulator' with the corresponding attribute
        // information structure.  Return the value returned from the
        // invocation of 'manipulator' if 'id' identifies an attribute of this
        // class, and -1 otherwise.

    template <typename t_MANIPULATOR>
    int manipulateAttribute(t_MANIPULATOR&  manipulator,
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
                        int           level          = 0,
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

    template <typename t_ACCESSOR>
    int accessAttributes(t_ACCESSOR& accessor) const;
        // Invoke the specified 'accessor' sequentially on each
        // (non-modifiable) attribute of this object, supplying 'accessor'
        // with the corresponding attribute information structure until such
        // invocation returns a non-zero value.  Return the value from the
        // last invocation of 'accessor' (i.e., the invocation that terminated
        // the sequence).

    template <typename t_ACCESSOR>
    int accessAttribute(t_ACCESSOR& accessor, int id) const;
        // Invoke the specified 'accessor' on the (non-modifiable) attribute
        // of this object indicated by the specified 'id', supplying 'accessor'
        // with the corresponding attribute information structure.  Return the
        // value returned from the invocation of 'accessor' if 'id' identifies
        // an attribute of this class, and -1 otherwise.

    template <typename t_ACCESSOR>
    int accessAttribute(t_ACCESSOR&   accessor,
                        const char *name,
                        int         nameLength) const;
        // Invoke the specified 'accessor' on the (non-modifiable) attribute
        // of this object indicated by the specified 'name' of the specified
        // 'nameLength', supplying 'accessor' with the corresponding attribute
        // information structure.  Return the value returned from the
        // invocation of 'accessor' if 'name' identifies an attribute of this
        // class, and -1 otherwise.

    // HIDDEN FRIENDS
    friend bool operator==(const EncoderTestSequenceWithUntagged4Sequence&,
                           const EncoderTestSequenceWithUntagged4Sequence&)
        // Returns 'true' as this type has no attributes and so all objects of
        // this type are considered equal.
    {
        return true;
    }

    friend bool operator!=(const EncoderTestSequenceWithUntagged4Sequence& lhs,
                           const EncoderTestSequenceWithUntagged4Sequence& rhs)
        // Returns '!(lhs == rhs)'
    {
        return !(lhs == rhs);
    }

    friend bsl::ostream& operator<<(
                        bsl::ostream&                                   stream,
                        const EncoderTestSequenceWithUntagged4Sequence& rhs)
        // Format the specified 'rhs' to the specified output 'stream' and
        // return a reference to the modifiable 'stream'.
    {
        return rhs.print(stream, 0, -1);
    }

    template <typename t_HASH_ALGORITHM>
    friend void hashAppend(t_HASH_ALGORITHM&                              ,
                           const EncoderTestSequenceWithUntagged4Sequence&)
        // Pass the specified 'object' to the specified 'hashAlg'.  This
        // function integrates with the 'bslh' modular hashing system and
        // effectively provides a 'bsl::hash' specialization for
        // 'EncoderTestSequenceWithUntagged4Sequence'.
    {
    }
};

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

    // MANIPULATORS
    void reset();
        // Reset this object to the default value (i.e., its value upon
        // default construction).

    template <typename t_MANIPULATOR>
    int manipulateAttributes(t_MANIPULATOR& manipulator);
        // Invoke the specified 'manipulator' sequentially on the address of
        // each (modifiable) attribute of this object, supplying 'manipulator'
        // with the corresponding attribute information structure until such
        // invocation returns a non-zero value.  Return the value from the
        // last invocation of 'manipulator' (i.e., the invocation that
        // terminated the sequence).

    template <typename t_MANIPULATOR>
    int manipulateAttribute(t_MANIPULATOR& manipulator, int id);
        // Invoke the specified 'manipulator' on the address of
        // the (modifiable) attribute indicated by the specified 'id',
        // supplying 'manipulator' with the corresponding attribute
        // information structure.  Return the value returned from the
        // invocation of 'manipulator' if 'id' identifies an attribute of this
        // class, and -1 otherwise.

    template <typename t_MANIPULATOR>
    int manipulateAttribute(t_MANIPULATOR&  manipulator,
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
                        int           level          = 0,
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

    template <typename t_ACCESSOR>
    int accessAttributes(t_ACCESSOR& accessor) const;
        // Invoke the specified 'accessor' sequentially on each
        // (non-modifiable) attribute of this object, supplying 'accessor'
        // with the corresponding attribute information structure until such
        // invocation returns a non-zero value.  Return the value from the
        // last invocation of 'accessor' (i.e., the invocation that terminated
        // the sequence).

    template <typename t_ACCESSOR>
    int accessAttribute(t_ACCESSOR& accessor, int id) const;
        // Invoke the specified 'accessor' on the (non-modifiable) attribute
        // of this object indicated by the specified 'id', supplying 'accessor'
        // with the corresponding attribute information structure.  Return the
        // value returned from the invocation of 'accessor' if 'id' identifies
        // an attribute of this class, and -1 otherwise.

    template <typename t_ACCESSOR>
    int accessAttribute(t_ACCESSOR&   accessor,
                        const char *name,
                        int         nameLength) const;
        // Invoke the specified 'accessor' on the (non-modifiable) attribute
        // of this object indicated by the specified 'name' of the specified
        // 'nameLength', supplying 'accessor' with the corresponding attribute
        // information structure.  Return the value returned from the
        // invocation of 'accessor' if 'name' identifies an attribute of this
        // class, and -1 otherwise.

    // HIDDEN FRIENDS
    friend bool operator==(const EncoderTestSequenceWithUntagged5Sequence&,
                           const EncoderTestSequenceWithUntagged5Sequence&)
        // Returns 'true' as this type has no attributes and so all objects of
        // this type are considered equal.
    {
        return true;
    }

    friend bool operator!=(const EncoderTestSequenceWithUntagged5Sequence& lhs,
                           const EncoderTestSequenceWithUntagged5Sequence& rhs)
        // Returns '!(lhs == rhs)'
    {
        return !(lhs == rhs);
    }

    friend bsl::ostream& operator<<(
                        bsl::ostream&                                   stream,
                        const EncoderTestSequenceWithUntagged5Sequence& rhs)
        // Format the specified 'rhs' to the specified output 'stream' and
        // return a reference to the modifiable 'stream'.
    {
        return rhs.print(stream, 0, -1);
    }

    template <typename t_HASH_ALGORITHM>
    friend void hashAppend(t_HASH_ALGORITHM&                              ,
                           const EncoderTestSequenceWithUntagged5Sequence&)
        // Pass the specified 'object' to the specified 'hashAlg'.  This
        // function integrates with the 'bslh' modular hashing system and
        // effectively provides a 'bsl::hash' specialization for
        // 'EncoderTestSequenceWithUntagged5Sequence'.
    {
    }
};

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


    // MANIPULATORS
    void reset();
        // Reset this object to the default value (i.e., its value upon
        // default construction).

    template <typename t_MANIPULATOR>
    int manipulateAttributes(t_MANIPULATOR& manipulator);
        // Invoke the specified 'manipulator' sequentially on the address of
        // each (modifiable) attribute of this object, supplying 'manipulator'
        // with the corresponding attribute information structure until such
        // invocation returns a non-zero value.  Return the value from the
        // last invocation of 'manipulator' (i.e., the invocation that
        // terminated the sequence).

    template <typename t_MANIPULATOR>
    int manipulateAttribute(t_MANIPULATOR& manipulator, int id);
        // Invoke the specified 'manipulator' on the address of
        // the (modifiable) attribute indicated by the specified 'id',
        // supplying 'manipulator' with the corresponding attribute
        // information structure.  Return the value returned from the
        // invocation of 'manipulator' if 'id' identifies an attribute of this
        // class, and -1 otherwise.

    template <typename t_MANIPULATOR>
    int manipulateAttribute(t_MANIPULATOR&  manipulator,
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
                        int           level          = 0,
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

    template <typename t_ACCESSOR>
    int accessAttributes(t_ACCESSOR& accessor) const;
        // Invoke the specified 'accessor' sequentially on each
        // (non-modifiable) attribute of this object, supplying 'accessor'
        // with the corresponding attribute information structure until such
        // invocation returns a non-zero value.  Return the value from the
        // last invocation of 'accessor' (i.e., the invocation that terminated
        // the sequence).

    template <typename t_ACCESSOR>
    int accessAttribute(t_ACCESSOR& accessor, int id) const;
        // Invoke the specified 'accessor' on the (non-modifiable) attribute
        // of this object indicated by the specified 'id', supplying 'accessor'
        // with the corresponding attribute information structure.  Return the
        // value returned from the invocation of 'accessor' if 'id' identifies
        // an attribute of this class, and -1 otherwise.

    template <typename t_ACCESSOR>
    int accessAttribute(t_ACCESSOR&   accessor,
                        const char *name,
                        int         nameLength) const;
        // Invoke the specified 'accessor' on the (non-modifiable) attribute
        // of this object indicated by the specified 'name' of the specified
        // 'nameLength', supplying 'accessor' with the corresponding attribute
        // information structure.  Return the value returned from the
        // invocation of 'accessor' if 'name' identifies an attribute of this
        // class, and -1 otherwise.

    int attribute0() const;
        // Return the value of the "Attribute0" attribute of this object.

    int attribute1() const;
        // Return the value of the "Attribute1" attribute of this object.

    // HIDDEN FRIENDS
    friend bool operator==(const EncoderTestSequenceWithUntagged6& lhs,
                           const EncoderTestSequenceWithUntagged6& rhs)
        // Return 'true' if the specified 'lhs' and 'rhs' attribute objects
        // have the same value, and 'false' otherwise.  Two attribute objects
        // have the same value if each respective attribute has the same value.
    {
        return lhs.attribute0() == rhs.attribute0() &&
               lhs.attribute1() == rhs.attribute1();
    }

    friend bool operator!=(const EncoderTestSequenceWithUntagged6& lhs,
                           const EncoderTestSequenceWithUntagged6& rhs)
        // Returns '!(lhs == rhs)'
    {
        return !(lhs == rhs);
    }

    friend bsl::ostream& operator<<(
                                bsl::ostream&                           stream,
                                const EncoderTestSequenceWithUntagged6& rhs)
        // Format the specified 'rhs' to the specified output 'stream' and
        // return a reference to the modifiable 'stream'.
    {
        return rhs.print(stream, 0, -1);
    }

    template <typename t_HASH_ALGORITHM>
    friend void hashAppend(t_HASH_ALGORITHM&                       hashAlg,
                           const EncoderTestSequenceWithUntagged6& object)
        // Pass the specified 'object' to the specified 'hashAlg'.  This
        // function integrates with the 'bslh' modular hashing system and
        // effectively provides a 'bsl::hash' specialization for
        // 'EncoderTestSequenceWithUntagged6'.
    {
        using bslh::hashAppend;
        hashAppend(hashAlg, object.attribute0());
        hashAppend(hashAlg, object.attribute1());
    }
};

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

    // MANIPULATORS
    void reset();
        // Reset this object to the default value (i.e., its value upon
        // default construction).

    template <typename t_MANIPULATOR>
    int manipulateAttributes(t_MANIPULATOR& manipulator);
        // Invoke the specified 'manipulator' sequentially on the address of
        // each (modifiable) attribute of this object, supplying 'manipulator'
        // with the corresponding attribute information structure until such
        // invocation returns a non-zero value.  Return the value from the
        // last invocation of 'manipulator' (i.e., the invocation that
        // terminated the sequence).

    template <typename t_MANIPULATOR>
    int manipulateAttribute(t_MANIPULATOR& manipulator, int id);
        // Invoke the specified 'manipulator' on the address of
        // the (modifiable) attribute indicated by the specified 'id',
        // supplying 'manipulator' with the corresponding attribute
        // information structure.  Return the value returned from the
        // invocation of 'manipulator' if 'id' identifies an attribute of this
        // class, and -1 otherwise.

    template <typename t_MANIPULATOR>
    int manipulateAttribute(t_MANIPULATOR&  manipulator,
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
                        int           level          = 0,
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

    template <typename t_ACCESSOR>
    int accessAttributes(t_ACCESSOR& accessor) const;
        // Invoke the specified 'accessor' sequentially on each
        // (non-modifiable) attribute of this object, supplying 'accessor'
        // with the corresponding attribute information structure until such
        // invocation returns a non-zero value.  Return the value from the
        // last invocation of 'accessor' (i.e., the invocation that terminated
        // the sequence).

    template <typename t_ACCESSOR>
    int accessAttribute(t_ACCESSOR& accessor, int id) const;
        // Invoke the specified 'accessor' on the (non-modifiable) attribute
        // of this object indicated by the specified 'id', supplying 'accessor'
        // with the corresponding attribute information structure.  Return the
        // value returned from the invocation of 'accessor' if 'id' identifies
        // an attribute of this class, and -1 otherwise.

    template <typename t_ACCESSOR>
    int accessAttribute(t_ACCESSOR&   accessor,
                        const char *name,
                        int         nameLength) const;
        // Invoke the specified 'accessor' on the (non-modifiable) attribute
        // of this object indicated by the specified 'name' of the specified
        // 'nameLength', supplying 'accessor' with the corresponding attribute
        // information structure.  Return the value returned from the
        // invocation of 'accessor' if 'name' identifies an attribute of this
        // class, and -1 otherwise.

    // HIDDEN FRIENDS
    friend bool operator==(const EncoderTestSequenceWithUntagged7Sequence&,
                           const EncoderTestSequenceWithUntagged7Sequence&)
        // Returns 'true' as this type has no attributes and so all objects of
        // this type are considered equal.
    {
        return true;
    }

    friend bool operator!=(const EncoderTestSequenceWithUntagged7Sequence& lhs,
                           const EncoderTestSequenceWithUntagged7Sequence& rhs)
        // Returns '!(lhs == rhs)'
    {
        return !(lhs == rhs);
    }

    friend bsl::ostream& operator<<(
                        bsl::ostream&                                   stream,
                        const EncoderTestSequenceWithUntagged7Sequence& rhs)
        // Format the specified 'rhs' to the specified output 'stream' and
        // return a reference to the modifiable 'stream'.
    {
        return rhs.print(stream, 0, -1);
    }

    template <typename t_HASH_ALGORITHM>
    friend void hashAppend(t_HASH_ALGORITHM&                              ,
                           const EncoderTestSequenceWithUntagged7Sequence&)
        // Pass the specified 'object' to the specified 'hashAlg'.  This
        // function integrates with the 'bslh' modular hashing system and
        // effectively provides a 'bsl::hash' specialization for
        // 'EncoderTestSequenceWithUntagged7Sequence'.
    {
    }
};

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

    // MANIPULATORS
    void reset();
        // Reset this object to the default value (i.e., its value upon
        // default construction).

    template <typename t_MANIPULATOR>
    int manipulateAttributes(t_MANIPULATOR& manipulator);
        // Invoke the specified 'manipulator' sequentially on the address of
        // each (modifiable) attribute of this object, supplying 'manipulator'
        // with the corresponding attribute information structure until such
        // invocation returns a non-zero value.  Return the value from the
        // last invocation of 'manipulator' (i.e., the invocation that
        // terminated the sequence).

    template <typename t_MANIPULATOR>
    int manipulateAttribute(t_MANIPULATOR& manipulator, int id);
        // Invoke the specified 'manipulator' on the address of
        // the (modifiable) attribute indicated by the specified 'id',
        // supplying 'manipulator' with the corresponding attribute
        // information structure.  Return the value returned from the
        // invocation of 'manipulator' if 'id' identifies an attribute of this
        // class, and -1 otherwise.

    template <typename t_MANIPULATOR>
    int manipulateAttribute(t_MANIPULATOR&  manipulator,
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
                        int           level          = 0,
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

    template <typename t_ACCESSOR>
    int accessAttributes(t_ACCESSOR& accessor) const;
        // Invoke the specified 'accessor' sequentially on each
        // (non-modifiable) attribute of this object, supplying 'accessor'
        // with the corresponding attribute information structure until such
        // invocation returns a non-zero value.  Return the value from the
        // last invocation of 'accessor' (i.e., the invocation that terminated
        // the sequence).

    template <typename t_ACCESSOR>
    int accessAttribute(t_ACCESSOR& accessor, int id) const;
        // Invoke the specified 'accessor' on the (non-modifiable) attribute
        // of this object indicated by the specified 'id', supplying 'accessor'
        // with the corresponding attribute information structure.  Return the
        // value returned from the invocation of 'accessor' if 'id' identifies
        // an attribute of this class, and -1 otherwise.

    template <typename t_ACCESSOR>
    int accessAttribute(t_ACCESSOR&   accessor,
                        const char *name,
                        int         nameLength) const;
        // Invoke the specified 'accessor' on the (non-modifiable) attribute
        // of this object indicated by the specified 'name' of the specified
        // 'nameLength', supplying 'accessor' with the corresponding attribute
        // information structure.  Return the value returned from the
        // invocation of 'accessor' if 'name' identifies an attribute of this
        // class, and -1 otherwise.

    // HIDDEN FRIENDS
    friend bool operator==(const EncoderTestSequenceWithUntagged7Sequence1&,
                           const EncoderTestSequenceWithUntagged7Sequence1&)
        // Returns 'true' as this type has no attributes and so all objects of
        // this type are considered equal.
    {
        return true;
    }

    friend bool operator!=(
                          const EncoderTestSequenceWithUntagged7Sequence1& lhs,
                          const EncoderTestSequenceWithUntagged7Sequence1& rhs)
        // Returns '!(lhs == rhs)'
    {
        return !(lhs == rhs);
    }

    friend bsl::ostream& operator<<(
                       bsl::ostream&                                    stream,
                       const EncoderTestSequenceWithUntagged7Sequence1& rhs)
        // Format the specified 'rhs' to the specified output 'stream' and
        // return a reference to the modifiable 'stream'.
    {
        return rhs.print(stream, 0, -1);
    }

    template <typename t_HASH_ALGORITHM>
    friend void hashAppend(t_HASH_ALGORITHM&                               ,
                           const EncoderTestSequenceWithUntagged7Sequence1&)
        // Pass the specified 'object' to the specified 'hashAlg'.  This
        // function integrates with the 'bslh' modular hashing system and
        // effectively provides a 'bsl::hash' specialization for
        // 'EncoderTestSequenceWithUntagged7Sequence1'.
    {
    }
};

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

    // MANIPULATORS
    void reset();
        // Reset this object to the default value (i.e., its value upon
        // default construction).

    template <typename t_MANIPULATOR>
    int manipulateAttributes(t_MANIPULATOR& manipulator);
        // Invoke the specified 'manipulator' sequentially on the address of
        // each (modifiable) attribute of this object, supplying 'manipulator'
        // with the corresponding attribute information structure until such
        // invocation returns a non-zero value.  Return the value from the
        // last invocation of 'manipulator' (i.e., the invocation that
        // terminated the sequence).

    template <typename t_MANIPULATOR>
    int manipulateAttribute(t_MANIPULATOR& manipulator, int id);
        // Invoke the specified 'manipulator' on the address of
        // the (modifiable) attribute indicated by the specified 'id',
        // supplying 'manipulator' with the corresponding attribute
        // information structure.  Return the value returned from the
        // invocation of 'manipulator' if 'id' identifies an attribute of this
        // class, and -1 otherwise.

    template <typename t_MANIPULATOR>
    int manipulateAttribute(t_MANIPULATOR&  manipulator,
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
                        int           level          = 0,
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

    template <typename t_ACCESSOR>
    int accessAttributes(t_ACCESSOR& accessor) const;
        // Invoke the specified 'accessor' sequentially on each
        // (non-modifiable) attribute of this object, supplying 'accessor'
        // with the corresponding attribute information structure until such
        // invocation returns a non-zero value.  Return the value from the
        // last invocation of 'accessor' (i.e., the invocation that terminated
        // the sequence).

    template <typename t_ACCESSOR>
    int accessAttribute(t_ACCESSOR& accessor, int id) const;
        // Invoke the specified 'accessor' on the (non-modifiable) attribute
        // of this object indicated by the specified 'id', supplying 'accessor'
        // with the corresponding attribute information structure.  Return the
        // value returned from the invocation of 'accessor' if 'id' identifies
        // an attribute of this class, and -1 otherwise.

    template <typename t_ACCESSOR>
    int accessAttribute(t_ACCESSOR&   accessor,
                        const char *name,
                        int         nameLength) const;
        // Invoke the specified 'accessor' on the (non-modifiable) attribute
        // of this object indicated by the specified 'name' of the specified
        // 'nameLength', supplying 'accessor' with the corresponding attribute
        // information structure.  Return the value returned from the
        // invocation of 'accessor' if 'name' identifies an attribute of this
        // class, and -1 otherwise.

    // HIDDEN FRIENDS
    friend bool operator==(const EncoderTestSequenceWithUntagged7Sequence2&,
                           const EncoderTestSequenceWithUntagged7Sequence2&)
        // Returns 'true' as this type has no attributes and so all objects of
        // this type are considered equal.
    {
        return true;
    }

    friend bool operator!=(
                          const EncoderTestSequenceWithUntagged7Sequence2& lhs,
                          const EncoderTestSequenceWithUntagged7Sequence2& rhs)
        // Returns '!(lhs == rhs)'
    {
        return !(lhs == rhs);
    }

    friend bsl::ostream& operator<<(
                       bsl::ostream&                                    stream,
                       const EncoderTestSequenceWithUntagged7Sequence2& rhs)
        // Format the specified 'rhs' to the specified output 'stream' and
        // return a reference to the modifiable 'stream'.
    {
        return rhs.print(stream, 0, -1);
    }

    template <typename t_HASH_ALGORITHM>
    friend void hashAppend(t_HASH_ALGORITHM&                               ,
                           const EncoderTestSequenceWithUntagged7Sequence2&)
        // Pass the specified 'object' to the specified 'hashAlg'.  This
        // function integrates with the 'bslh' modular hashing system and
        // effectively provides a 'bsl::hash' specialization for
        // 'EncoderTestSequenceWithUntagged7Sequence2'.
    {
    }
};

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

    // MANIPULATORS
    void reset();
        // Reset this object to the default value (i.e., its value upon
        // default construction).

    template <typename t_MANIPULATOR>
    int manipulateAttributes(t_MANIPULATOR& manipulator);
        // Invoke the specified 'manipulator' sequentially on the address of
        // each (modifiable) attribute of this object, supplying 'manipulator'
        // with the corresponding attribute information structure until such
        // invocation returns a non-zero value.  Return the value from the
        // last invocation of 'manipulator' (i.e., the invocation that
        // terminated the sequence).

    template <typename t_MANIPULATOR>
    int manipulateAttribute(t_MANIPULATOR& manipulator, int id);
        // Invoke the specified 'manipulator' on the address of
        // the (modifiable) attribute indicated by the specified 'id',
        // supplying 'manipulator' with the corresponding attribute
        // information structure.  Return the value returned from the
        // invocation of 'manipulator' if 'id' identifies an attribute of this
        // class, and -1 otherwise.

    template <typename t_MANIPULATOR>
    int manipulateAttribute(t_MANIPULATOR&  manipulator,
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
                        int           level          = 0,
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

    template <typename t_ACCESSOR>
    int accessAttributes(t_ACCESSOR& accessor) const;
        // Invoke the specified 'accessor' sequentially on each
        // (non-modifiable) attribute of this object, supplying 'accessor'
        // with the corresponding attribute information structure until such
        // invocation returns a non-zero value.  Return the value from the
        // last invocation of 'accessor' (i.e., the invocation that terminated
        // the sequence).

    template <typename t_ACCESSOR>
    int accessAttribute(t_ACCESSOR& accessor, int id) const;
        // Invoke the specified 'accessor' on the (non-modifiable) attribute
        // of this object indicated by the specified 'id', supplying 'accessor'
        // with the corresponding attribute information structure.  Return the
        // value returned from the invocation of 'accessor' if 'id' identifies
        // an attribute of this class, and -1 otherwise.

    template <typename t_ACCESSOR>
    int accessAttribute(t_ACCESSOR&   accessor,
                        const char *name,
                        int         nameLength) const;
        // Invoke the specified 'accessor' on the (non-modifiable) attribute
        // of this object indicated by the specified 'name' of the specified
        // 'nameLength', supplying 'accessor' with the corresponding attribute
        // information structure.  Return the value returned from the
        // invocation of 'accessor' if 'name' identifies an attribute of this
        // class, and -1 otherwise.

    // HIDDEN FRIENDS
    friend bool operator==(const EncoderTestSequenceWithUntagged8Sequence&,
                           const EncoderTestSequenceWithUntagged8Sequence&)
        // Returns 'true' as this type has no attributes and so all objects of
        // this type are considered equal.
    {
        return true;
    }

    friend bool operator!=(const EncoderTestSequenceWithUntagged8Sequence& lhs,
                           const EncoderTestSequenceWithUntagged8Sequence& rhs)
        // Returns '!(lhs == rhs)'
    {
        return !(lhs == rhs);
    }

    friend bsl::ostream& operator<<(
                        bsl::ostream&                                   stream,
                        const EncoderTestSequenceWithUntagged8Sequence& rhs)
        // Format the specified 'rhs' to the specified output 'stream' and
        // return a reference to the modifiable 'stream'.
    {
        return rhs.print(stream, 0, -1);
    }

    template <typename t_HASH_ALGORITHM>
    friend void hashAppend(t_HASH_ALGORITHM&                              ,
                           const EncoderTestSequenceWithUntagged8Sequence&)
        // Pass the specified 'object' to the specified 'hashAlg'.  This
        // function integrates with the 'bslh' modular hashing system and
        // effectively provides a 'bsl::hash' specialization for
        // 'EncoderTestSequenceWithUntagged8Sequence'.
    {
    }
};

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

    // MANIPULATORS
    void reset();
        // Reset this object to the default value (i.e., its value upon
        // default construction).

    template <typename t_MANIPULATOR>
    int manipulateAttributes(t_MANIPULATOR& manipulator);
        // Invoke the specified 'manipulator' sequentially on the address of
        // each (modifiable) attribute of this object, supplying 'manipulator'
        // with the corresponding attribute information structure until such
        // invocation returns a non-zero value.  Return the value from the
        // last invocation of 'manipulator' (i.e., the invocation that
        // terminated the sequence).

    template <typename t_MANIPULATOR>
    int manipulateAttribute(t_MANIPULATOR& manipulator, int id);
        // Invoke the specified 'manipulator' on the address of
        // the (modifiable) attribute indicated by the specified 'id',
        // supplying 'manipulator' with the corresponding attribute
        // information structure.  Return the value returned from the
        // invocation of 'manipulator' if 'id' identifies an attribute of this
        // class, and -1 otherwise.

    template <typename t_MANIPULATOR>
    int manipulateAttribute(t_MANIPULATOR&  manipulator,
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
                        int           level          = 0,
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

    template <typename t_ACCESSOR>
    int accessAttributes(t_ACCESSOR& accessor) const;
        // Invoke the specified 'accessor' sequentially on each
        // (non-modifiable) attribute of this object, supplying 'accessor'
        // with the corresponding attribute information structure until such
        // invocation returns a non-zero value.  Return the value from the
        // last invocation of 'accessor' (i.e., the invocation that terminated
        // the sequence).

    template <typename t_ACCESSOR>
    int accessAttribute(t_ACCESSOR& accessor, int id) const;
        // Invoke the specified 'accessor' on the (non-modifiable) attribute
        // of this object indicated by the specified 'id', supplying 'accessor'
        // with the corresponding attribute information structure.  Return the
        // value returned from the invocation of 'accessor' if 'id' identifies
        // an attribute of this class, and -1 otherwise.

    template <typename t_ACCESSOR>
    int accessAttribute(t_ACCESSOR&   accessor,
                        const char *name,
                        int         nameLength) const;
        // Invoke the specified 'accessor' on the (non-modifiable) attribute
        // of this object indicated by the specified 'name' of the specified
        // 'nameLength', supplying 'accessor' with the corresponding attribute
        // information structure.  Return the value returned from the
        // invocation of 'accessor' if 'name' identifies an attribute of this
        // class, and -1 otherwise.

    // HIDDEN FRIENDS
    friend bool operator==(const EncoderTestSequenceWithUntagged8Sequence1&,
                           const EncoderTestSequenceWithUntagged8Sequence1&)
        // Returns 'true' as this type has no attributes and so all objects of
        // this type are considered equal.
    {
        return true;
    }

    friend bool operator!=(
                          const EncoderTestSequenceWithUntagged8Sequence1& lhs,
                          const EncoderTestSequenceWithUntagged8Sequence1& rhs)
        // Returns '!(lhs == rhs)'
    {
        return !(lhs == rhs);
    }

    friend bsl::ostream& operator<<(
                       bsl::ostream&                                    stream,
                       const EncoderTestSequenceWithUntagged8Sequence1& rhs)
        // Format the specified 'rhs' to the specified output 'stream' and
        // return a reference to the modifiable 'stream'.
    {
        return rhs.print(stream, 0, -1);
    }

    template <typename t_HASH_ALGORITHM>
    friend void hashAppend(t_HASH_ALGORITHM&                               ,
                           const EncoderTestSequenceWithUntagged8Sequence1&)
        // Pass the specified 'object' to the specified 'hashAlg'.  This
        // function integrates with the 'bslh' modular hashing system and
        // effectively provides a 'bsl::hash' specialization for
        // 'EncoderTestSequenceWithUntagged8Sequence1'.
    {
    }
};

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

    // MANIPULATORS
    void reset();
        // Reset this object to the default value (i.e., its value upon
        // default construction).

    template <typename t_MANIPULATOR>
    int manipulateAttributes(t_MANIPULATOR& manipulator);
        // Invoke the specified 'manipulator' sequentially on the address of
        // each (modifiable) attribute of this object, supplying 'manipulator'
        // with the corresponding attribute information structure until such
        // invocation returns a non-zero value.  Return the value from the
        // last invocation of 'manipulator' (i.e., the invocation that
        // terminated the sequence).

    template <typename t_MANIPULATOR>
    int manipulateAttribute(t_MANIPULATOR& manipulator, int id);
        // Invoke the specified 'manipulator' on the address of
        // the (modifiable) attribute indicated by the specified 'id',
        // supplying 'manipulator' with the corresponding attribute
        // information structure.  Return the value returned from the
        // invocation of 'manipulator' if 'id' identifies an attribute of this
        // class, and -1 otherwise.

    template <typename t_MANIPULATOR>
    int manipulateAttribute(t_MANIPULATOR&  manipulator,
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
                        int           level          = 0,
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

    template <typename t_ACCESSOR>
    int accessAttributes(t_ACCESSOR& accessor) const;
        // Invoke the specified 'accessor' sequentially on each
        // (non-modifiable) attribute of this object, supplying 'accessor'
        // with the corresponding attribute information structure until such
        // invocation returns a non-zero value.  Return the value from the
        // last invocation of 'accessor' (i.e., the invocation that terminated
        // the sequence).

    template <typename t_ACCESSOR>
    int accessAttribute(t_ACCESSOR& accessor, int id) const;
        // Invoke the specified 'accessor' on the (non-modifiable) attribute
        // of this object indicated by the specified 'id', supplying 'accessor'
        // with the corresponding attribute information structure.  Return the
        // value returned from the invocation of 'accessor' if 'id' identifies
        // an attribute of this class, and -1 otherwise.

    template <typename t_ACCESSOR>
    int accessAttribute(t_ACCESSOR&   accessor,
                        const char *name,
                        int         nameLength) const;
        // Invoke the specified 'accessor' on the (non-modifiable) attribute
        // of this object indicated by the specified 'name' of the specified
        // 'nameLength', supplying 'accessor' with the corresponding attribute
        // information structure.  Return the value returned from the
        // invocation of 'accessor' if 'name' identifies an attribute of this
        // class, and -1 otherwise.

    // HIDDEN FRIENDS
    friend bool operator==(const EncoderTestSequenceWithUntagged9Sequence&,
                           const EncoderTestSequenceWithUntagged9Sequence&)
        // Returns 'true' as this type has no attributes and so all objects of
        // this type are considered equal.
    {
        return true;
    }

    friend bool operator!=(const EncoderTestSequenceWithUntagged9Sequence& lhs,
                           const EncoderTestSequenceWithUntagged9Sequence& rhs)
        // Returns '!(lhs == rhs)'
    {
        return !(lhs == rhs);
    }

    friend bsl::ostream& operator<<(
                        bsl::ostream&                                   stream,
                        const EncoderTestSequenceWithUntagged9Sequence& rhs)
        // Format the specified 'rhs' to the specified output 'stream' and
        // return a reference to the modifiable 'stream'.
    {
        return rhs.print(stream, 0, -1);
    }

    template <typename t_HASH_ALGORITHM>
    friend void hashAppend(t_HASH_ALGORITHM&                              ,
                           const EncoderTestSequenceWithUntagged9Sequence&)
        // Pass the specified 'object' to the specified 'hashAlg'.  This
        // function integrates with the 'bslh' modular hashing system and
        // effectively provides a 'bsl::hash' specialization for
        // 'EncoderTestSequenceWithUntagged9Sequence'.
    {
    }
};

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

    // MANIPULATORS
    void reset();
        // Reset this object to the default value (i.e., its value upon
        // default construction).

    template <typename t_MANIPULATOR>
    int manipulateAttributes(t_MANIPULATOR& manipulator);
        // Invoke the specified 'manipulator' sequentially on the address of
        // each (modifiable) attribute of this object, supplying 'manipulator'
        // with the corresponding attribute information structure until such
        // invocation returns a non-zero value.  Return the value from the
        // last invocation of 'manipulator' (i.e., the invocation that
        // terminated the sequence).

    template <typename t_MANIPULATOR>
    int manipulateAttribute(t_MANIPULATOR& manipulator, int id);
        // Invoke the specified 'manipulator' on the address of
        // the (modifiable) attribute indicated by the specified 'id',
        // supplying 'manipulator' with the corresponding attribute
        // information structure.  Return the value returned from the
        // invocation of 'manipulator' if 'id' identifies an attribute of this
        // class, and -1 otherwise.

    template <typename t_MANIPULATOR>
    int manipulateAttribute(t_MANIPULATOR&  manipulator,
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
                        int           level          = 0,
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

    template <typename t_ACCESSOR>
    int accessAttributes(t_ACCESSOR& accessor) const;
        // Invoke the specified 'accessor' sequentially on each
        // (non-modifiable) attribute of this object, supplying 'accessor'
        // with the corresponding attribute information structure until such
        // invocation returns a non-zero value.  Return the value from the
        // last invocation of 'accessor' (i.e., the invocation that terminated
        // the sequence).

    template <typename t_ACCESSOR>
    int accessAttribute(t_ACCESSOR& accessor, int id) const;
        // Invoke the specified 'accessor' on the (non-modifiable) attribute
        // of this object indicated by the specified 'id', supplying 'accessor'
        // with the corresponding attribute information structure.  Return the
        // value returned from the invocation of 'accessor' if 'id' identifies
        // an attribute of this class, and -1 otherwise.

    template <typename t_ACCESSOR>
    int accessAttribute(t_ACCESSOR&   accessor,
                        const char *name,
                        int         nameLength) const;
        // Invoke the specified 'accessor' on the (non-modifiable) attribute
        // of this object indicated by the specified 'name' of the specified
        // 'nameLength', supplying 'accessor' with the corresponding attribute
        // information structure.  Return the value returned from the
        // invocation of 'accessor' if 'name' identifies an attribute of this
        // class, and -1 otherwise.

    // HIDDEN FRIENDS
    friend bool operator==(const EncoderTestSequenceWithUntagged9Sequence1&,
                           const EncoderTestSequenceWithUntagged9Sequence1&)
        // Returns 'true' as this type has no attributes and so all objects of
        // this type are considered equal.
    {
        return true;
    }

    friend bool operator!=(
                          const EncoderTestSequenceWithUntagged9Sequence1& lhs,
                          const EncoderTestSequenceWithUntagged9Sequence1& rhs)
        // Returns '!(lhs == rhs)'
    {
        return !(lhs == rhs);
    }

    friend bsl::ostream& operator<<(
                       bsl::ostream&                                    stream,
                       const EncoderTestSequenceWithUntagged9Sequence1& rhs)
        // Format the specified 'rhs' to the specified output 'stream' and
        // return a reference to the modifiable 'stream'.
    {
        return rhs.print(stream, 0, -1);
    }

    template <typename t_HASH_ALGORITHM>
    friend void hashAppend(t_HASH_ALGORITHM&                               ,
                           const EncoderTestSequenceWithUntagged9Sequence1&)
        // Pass the specified 'object' to the specified 'hashAlg'.  This
        // function integrates with the 'bslh' modular hashing system and
        // effectively provides a 'bsl::hash' specialization for
        // 'EncoderTestSequenceWithUntagged9Sequence1'.
    {
    }
};

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

    // PRIVATE ACCESSORS
    template <typename t_HASH_ALGORITHM>
    void hashAppendImpl(t_HASH_ALGORITHM& hashAlgorithm) const;

    bool isEqualTo(const EncoderTestChoiceWithAllCategories& rhs) const;

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

    template <typename t_MANIPULATOR>
    int manipulateSelection(t_MANIPULATOR& manipulator);
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

    template <typename t_ACCESSOR>
    int accessSelection(t_ACCESSOR& accessor) const;
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

    // HIDDEN FRIENDS
    friend bool operator==(const EncoderTestChoiceWithAllCategories& lhs,
                           const EncoderTestChoiceWithAllCategories& rhs)
        // Return 'true' if the specified 'lhs' and 'rhs' objects have the same
        // value, and 'false' otherwise.  Two
        // 'EncoderTestChoiceWithAllCategories' objects have the same value if
        // either the selections in both objects have the same ids and the same
        // values, or both selections are undefined.
    {
        return lhs.isEqualTo(rhs);
    }

    friend bool operator!=(const EncoderTestChoiceWithAllCategories& lhs,
                           const EncoderTestChoiceWithAllCategories& rhs)
        // Return 'true' if the specified 'lhs' and 'rhs' objects do not have
        // the same values, as determined by 'operator==', and 'false'
        // otherwise.
    {
        return !(lhs == rhs);
    }

    friend bsl::ostream& operator<<(
                              bsl::ostream&                             stream,
                              const EncoderTestChoiceWithAllCategories& rhs)
        // Format the specified 'rhs' to the specified output 'stream' and
        // return a reference to the modifiable 'stream'.
    {
        return rhs.print(stream, 0, -1);
    }

    template <typename t_HASH_ALGORITHM>
    friend void hashAppend(t_HASH_ALGORITHM&                         hashAlg,
                           const EncoderTestChoiceWithAllCategories& object)
        // Pass the specified 'object' to the specified 'hashAlg'.  This
        // function integrates with the 'bslh' modular hashing system and
        // effectively provides a 'bsl::hash' specialization for
        // 'EncoderTestChoiceWithAllCategories'.
    {
        return object.hashAppendImpl(hashAlg);
    }
};

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

    // PRIVATE ACCESSORS
    template <typename t_HASH_ALGORITHM>
    void hashAppendImpl(t_HASH_ALGORITHM& hashAlgorithm) const;

    bool isEqualTo(const EncoderTestDegenerateChoice1& rhs) const;

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

    template <typename t_MANIPULATOR>
    int manipulateSelection(t_MANIPULATOR& manipulator);
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

    template <typename t_ACCESSOR>
    int accessSelection(t_ACCESSOR& accessor) const;
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

    // HIDDEN FRIENDS
    friend bool operator==(const EncoderTestDegenerateChoice1& lhs,
                           const EncoderTestDegenerateChoice1& rhs)
        // Return 'true' if the specified 'lhs' and 'rhs' objects have the same
        // value, and 'false' otherwise.  Two 'EncoderTestDegenerateChoice1'
        // objects have the same value if either the selections in both objects
        // have the same ids and the same values, or both selections are
        // undefined.
    {
        return lhs.isEqualTo(rhs);
    }

    friend bool operator!=(const EncoderTestDegenerateChoice1& lhs,
                           const EncoderTestDegenerateChoice1& rhs)
        // Return 'true' if the specified 'lhs' and 'rhs' objects do not have
        // the same values, as determined by 'operator==', and 'false'
        // otherwise.
    {
        return !(lhs == rhs);
    }

    friend bsl::ostream& operator<<(bsl::ostream&                       stream,
                                    const EncoderTestDegenerateChoice1& rhs)
        // Format the specified 'rhs' to the specified output 'stream' and
        // return a reference to the modifiable 'stream'.
    {
        return rhs.print(stream, 0, -1);
    }

    template <typename t_HASH_ALGORITHM>
    friend void hashAppend(t_HASH_ALGORITHM&                   hashAlg,
                           const EncoderTestDegenerateChoice1& object)
        // Pass the specified 'object' to the specified 'hashAlg'.  This
        // function integrates with the 'bslh' modular hashing system and
        // effectively provides a 'bsl::hash' specialization for
        // 'EncoderTestDegenerateChoice1'.
    {
        return object.hashAppendImpl(hashAlg);
    }
};

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

    // PRIVATE ACCESSORS
    template <typename t_HASH_ALGORITHM>
    void hashAppendImpl(t_HASH_ALGORITHM& hashAlgorithm) const;

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

    template <typename t_MANIPULATOR>
    int manipulateAttributes(t_MANIPULATOR& manipulator);
        // Invoke the specified 'manipulator' sequentially on the address of
        // each (modifiable) attribute of this object, supplying 'manipulator'
        // with the corresponding attribute information structure until such
        // invocation returns a non-zero value.  Return the value from the
        // last invocation of 'manipulator' (i.e., the invocation that
        // terminated the sequence).

    template <typename t_MANIPULATOR>
    int manipulateAttribute(t_MANIPULATOR& manipulator, int id);
        // Invoke the specified 'manipulator' on the address of
        // the (modifiable) attribute indicated by the specified 'id',
        // supplying 'manipulator' with the corresponding attribute
        // information structure.  Return the value returned from the
        // invocation of 'manipulator' if 'id' identifies an attribute of this
        // class, and -1 otherwise.

    template <typename t_MANIPULATOR>
    int manipulateAttribute(t_MANIPULATOR&  manipulator,
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
                        int           level          = 0,
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

    template <typename t_ACCESSOR>
    int accessAttributes(t_ACCESSOR& accessor) const;
        // Invoke the specified 'accessor' sequentially on each
        // (non-modifiable) attribute of this object, supplying 'accessor'
        // with the corresponding attribute information structure until such
        // invocation returns a non-zero value.  Return the value from the
        // last invocation of 'accessor' (i.e., the invocation that terminated
        // the sequence).

    template <typename t_ACCESSOR>
    int accessAttribute(t_ACCESSOR& accessor, int id) const;
        // Invoke the specified 'accessor' on the (non-modifiable) attribute
        // of this object indicated by the specified 'id', supplying 'accessor'
        // with the corresponding attribute information structure.  Return the
        // value returned from the invocation of 'accessor' if 'id' identifies
        // an attribute of this class, and -1 otherwise.

    template <typename t_ACCESSOR>
    int accessAttribute(t_ACCESSOR&   accessor,
                        const char *name,
                        int         nameLength) const;
        // Invoke the specified 'accessor' on the (non-modifiable) attribute
        // of this object indicated by the specified 'name' of the specified
        // 'nameLength', supplying 'accessor' with the corresponding attribute
        // information structure.  Return the value returned from the
        // invocation of 'accessor' if 'name' identifies an attribute of this
        // class, and -1 otherwise.

    const bsl::string& name() const;
        // Return a reference offering non-modifiable access to the "Name"
        // attribute of this object.

    const EncoderTestAddress& homeAddress() const;
        // Return a reference offering non-modifiable access to the
        // "HomeAddress" attribute of this object.

    int age() const;
        // Return the value of the "Age" attribute of this object.

    // HIDDEN FRIENDS
    friend bool operator==(const EncoderTestEmployee& lhs,
                           const EncoderTestEmployee& rhs)
        // Return 'true' if the specified 'lhs' and 'rhs' attribute objects
        // have the same value, and 'false' otherwise.  Two attribute objects
        // have the same value if each respective attribute has the same value.
    {
        return lhs.name() == rhs.name() &&
               lhs.homeAddress() == rhs.homeAddress() &&
               lhs.age() == rhs.age();
    }

    friend bool operator!=(const EncoderTestEmployee& lhs,
                           const EncoderTestEmployee& rhs)
        // Returns '!(lhs == rhs)'
    {
        return !(lhs == rhs);
    }

    friend bsl::ostream& operator<<(bsl::ostream&              stream,
                                    const EncoderTestEmployee& rhs)
        // Format the specified 'rhs' to the specified output 'stream' and
        // return a reference to the modifiable 'stream'.
    {
        return rhs.print(stream, 0, -1);
    }

    template <typename t_HASH_ALGORITHM>
    friend void hashAppend(t_HASH_ALGORITHM&          hashAlg,
                           const EncoderTestEmployee& object)
        // Pass the specified 'object' to the specified 'hashAlg'.  This
        // function integrates with the 'bslh' modular hashing system and
        // effectively provides a 'bsl::hash' specialization for
        // 'EncoderTestEmployee'.
    {
        object.hashAppendImpl(hashAlg);
    }
};

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
    bsl::string                                             d_aString;
    EncoderTestSequenceWithAllCategoriesSequence            d_sequence;
    EncoderTestSequenceWithAllCategoriesCustomizedType      d_customizedType;
    EncoderTestSequenceWithAllCategoriesChoice              d_choice;
    int                                                     d_simple;
    bdlb::NullableValue<int>                                d_nullableValue;
    EncoderTestSequenceWithAllCategoriesEnumeration::Value  d_enumeration;

    // PRIVATE ACCESSORS
    template <typename t_HASH_ALGORITHM>
    void hashAppendImpl(t_HASH_ALGORITHM& hashAlgorithm) const;

    bool isEqualTo(const EncoderTestSequenceWithAllCategories& rhs) const;

  public:
    // TYPES
    enum {
        ATTRIBUTE_ID_CHAR_ARRAY      = 0
      , ATTRIBUTE_ID_A_STRING        = 1
      , ATTRIBUTE_ID_ARRAY           = 2
      , ATTRIBUTE_ID_CHOICE          = 3
      , ATTRIBUTE_ID_CUSTOMIZED_TYPE = 4
      , ATTRIBUTE_ID_ENUMERATION     = 5
      , ATTRIBUTE_ID_NULLABLE_VALUE  = 6
      , ATTRIBUTE_ID_SEQUENCE        = 7
      , ATTRIBUTE_ID_SIMPLE          = 8
    };

    enum {
        NUM_ATTRIBUTES = 9
    };

    enum {
        ATTRIBUTE_INDEX_CHAR_ARRAY      = 0
      , ATTRIBUTE_INDEX_A_STRING        = 1
      , ATTRIBUTE_INDEX_ARRAY           = 2
      , ATTRIBUTE_INDEX_CHOICE          = 3
      , ATTRIBUTE_INDEX_CUSTOMIZED_TYPE = 4
      , ATTRIBUTE_INDEX_ENUMERATION     = 5
      , ATTRIBUTE_INDEX_NULLABLE_VALUE  = 6
      , ATTRIBUTE_INDEX_SEQUENCE        = 7
      , ATTRIBUTE_INDEX_SIMPLE          = 8
    };

    // CONSTANTS
    static const char CLASS_NAME[];

    static const char DEFAULT_INITIALIZER_A_STRING[];

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

    template <typename t_MANIPULATOR>
    int manipulateAttributes(t_MANIPULATOR& manipulator);
        // Invoke the specified 'manipulator' sequentially on the address of
        // each (modifiable) attribute of this object, supplying 'manipulator'
        // with the corresponding attribute information structure until such
        // invocation returns a non-zero value.  Return the value from the
        // last invocation of 'manipulator' (i.e., the invocation that
        // terminated the sequence).

    template <typename t_MANIPULATOR>
    int manipulateAttribute(t_MANIPULATOR& manipulator, int id);
        // Invoke the specified 'manipulator' on the address of
        // the (modifiable) attribute indicated by the specified 'id',
        // supplying 'manipulator' with the corresponding attribute
        // information structure.  Return the value returned from the
        // invocation of 'manipulator' if 'id' identifies an attribute of this
        // class, and -1 otherwise.

    template <typename t_MANIPULATOR>
    int manipulateAttribute(t_MANIPULATOR&  manipulator,
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

    bsl::string& aString();
        // Return a reference to the modifiable "AString" attribute of this
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
                        int           level          = 0,
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

    template <typename t_ACCESSOR>
    int accessAttributes(t_ACCESSOR& accessor) const;
        // Invoke the specified 'accessor' sequentially on each
        // (non-modifiable) attribute of this object, supplying 'accessor'
        // with the corresponding attribute information structure until such
        // invocation returns a non-zero value.  Return the value from the
        // last invocation of 'accessor' (i.e., the invocation that terminated
        // the sequence).

    template <typename t_ACCESSOR>
    int accessAttribute(t_ACCESSOR& accessor, int id) const;
        // Invoke the specified 'accessor' on the (non-modifiable) attribute
        // of this object indicated by the specified 'id', supplying 'accessor'
        // with the corresponding attribute information structure.  Return the
        // value returned from the invocation of 'accessor' if 'id' identifies
        // an attribute of this class, and -1 otherwise.

    template <typename t_ACCESSOR>
    int accessAttribute(t_ACCESSOR&   accessor,
                        const char *name,
                        int         nameLength) const;
        // Invoke the specified 'accessor' on the (non-modifiable) attribute
        // of this object indicated by the specified 'name' of the specified
        // 'nameLength', supplying 'accessor' with the corresponding attribute
        // information structure.  Return the value returned from the
        // invocation of 'accessor' if 'name' identifies an attribute of this
        // class, and -1 otherwise.

    const bsl::vector<char>& charArray() const;
        // Return a reference offering non-modifiable access to the "CharArray"
        // attribute of this object.

    const bsl::string& aString() const;
        // Return a reference offering non-modifiable access to the "AString"
        // attribute of this object.

    const bsl::vector<int>& array() const;
        // Return a reference offering non-modifiable access to the "Array"
        // attribute of this object.

    const EncoderTestSequenceWithAllCategoriesChoice& choice() const;
        // Return a reference offering non-modifiable access to the "Choice"
        // attribute of this object.

    const EncoderTestSequenceWithAllCategoriesCustomizedType& customizedType() const;
        // Return a reference offering non-modifiable access to the
        // "CustomizedType" attribute of this object.

    EncoderTestSequenceWithAllCategoriesEnumeration::Value enumeration() const;
        // Return the value of the "Enumeration" attribute of this object.

    const bdlb::NullableValue<int>& nullableValue() const;
        // Return a reference offering non-modifiable access to the
        // "NullableValue" attribute of this object.

    const EncoderTestSequenceWithAllCategoriesSequence& sequence() const;
        // Return a reference offering non-modifiable access to the "Sequence"
        // attribute of this object.

    int simple() const;
        // Return the value of the "Simple" attribute of this object.

    // HIDDEN FRIENDS
    friend bool operator==(const EncoderTestSequenceWithAllCategories& lhs,
                           const EncoderTestSequenceWithAllCategories& rhs)
        // Return 'true' if the specified 'lhs' and 'rhs' attribute objects
        // have the same value, and 'false' otherwise.  Two attribute objects
        // have the same value if each respective attribute has the same value.
    {
        return lhs.isEqualTo(rhs);
    }

    friend bool operator!=(const EncoderTestSequenceWithAllCategories& lhs,
                           const EncoderTestSequenceWithAllCategories& rhs)
        // Returns '!(lhs == rhs)'
    {
        return !(lhs == rhs);
    }

    friend bsl::ostream& operator<<(
                            bsl::ostream&                               stream,
                            const EncoderTestSequenceWithAllCategories& rhs)
        // Format the specified 'rhs' to the specified output 'stream' and
        // return a reference to the modifiable 'stream'.
    {
        return rhs.print(stream, 0, -1);
    }

    template <typename t_HASH_ALGORITHM>
    friend void hashAppend(t_HASH_ALGORITHM&                           hashAlg,
                           const EncoderTestSequenceWithAllCategories& object)
        // Pass the specified 'object' to the specified 'hashAlg'.  This
        // function integrates with the 'bslh' modular hashing system and
        // effectively provides a 'bsl::hash' specialization for
        // 'EncoderTestSequenceWithAllCategories'.
    {
        object.hashAppendImpl(hashAlg);
    }
};

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


    // MANIPULATORS
    void reset();
        // Reset this object to the default value (i.e., its value upon
        // default construction).

    template <typename t_MANIPULATOR>
    int manipulateAttributes(t_MANIPULATOR& manipulator);
        // Invoke the specified 'manipulator' sequentially on the address of
        // each (modifiable) attribute of this object, supplying 'manipulator'
        // with the corresponding attribute information structure until such
        // invocation returns a non-zero value.  Return the value from the
        // last invocation of 'manipulator' (i.e., the invocation that
        // terminated the sequence).

    template <typename t_MANIPULATOR>
    int manipulateAttribute(t_MANIPULATOR& manipulator, int id);
        // Invoke the specified 'manipulator' on the address of
        // the (modifiable) attribute indicated by the specified 'id',
        // supplying 'manipulator' with the corresponding attribute
        // information structure.  Return the value returned from the
        // invocation of 'manipulator' if 'id' identifies an attribute of this
        // class, and -1 otherwise.

    template <typename t_MANIPULATOR>
    int manipulateAttribute(t_MANIPULATOR&  manipulator,
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
                        int           level          = 0,
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

    template <typename t_ACCESSOR>
    int accessAttributes(t_ACCESSOR& accessor) const;
        // Invoke the specified 'accessor' sequentially on each
        // (non-modifiable) attribute of this object, supplying 'accessor'
        // with the corresponding attribute information structure until such
        // invocation returns a non-zero value.  Return the value from the
        // last invocation of 'accessor' (i.e., the invocation that terminated
        // the sequence).

    template <typename t_ACCESSOR>
    int accessAttribute(t_ACCESSOR& accessor, int id) const;
        // Invoke the specified 'accessor' on the (non-modifiable) attribute
        // of this object indicated by the specified 'id', supplying 'accessor'
        // with the corresponding attribute information structure.  Return the
        // value returned from the invocation of 'accessor' if 'id' identifies
        // an attribute of this class, and -1 otherwise.

    template <typename t_ACCESSOR>
    int accessAttribute(t_ACCESSOR&   accessor,
                        const char *name,
                        int         nameLength) const;
        // Invoke the specified 'accessor' on the (non-modifiable) attribute
        // of this object indicated by the specified 'name' of the specified
        // 'nameLength', supplying 'accessor' with the corresponding attribute
        // information structure.  Return the value returned from the
        // invocation of 'accessor' if 'name' identifies an attribute of this
        // class, and -1 otherwise.

    const EncoderTestSequenceWithUntagged1Sequence& sequence() const;
        // Return a reference offering non-modifiable access to the "Sequence"
        // attribute of this object.

    // HIDDEN FRIENDS
    friend bool operator==(const EncoderTestSequenceWithUntagged1& lhs,
                           const EncoderTestSequenceWithUntagged1& rhs)
        // Return 'true' if the specified 'lhs' and 'rhs' attribute objects
        // have the same value, and 'false' otherwise.  Two attribute objects
        // have the same value if each respective attribute has the same value.
    {
        return lhs.sequence() == rhs.sequence();
    }

    friend bool operator!=(const EncoderTestSequenceWithUntagged1& lhs,
                           const EncoderTestSequenceWithUntagged1& rhs)
        // Returns '!(lhs == rhs)'
    {
        return !(lhs == rhs);
    }

    friend bsl::ostream& operator<<(
                                bsl::ostream&                           stream,
                                const EncoderTestSequenceWithUntagged1& rhs)
        // Format the specified 'rhs' to the specified output 'stream' and
        // return a reference to the modifiable 'stream'.
    {
        return rhs.print(stream, 0, -1);
    }

    template <typename t_HASH_ALGORITHM>
    friend void hashAppend(t_HASH_ALGORITHM&                       hashAlg,
                           const EncoderTestSequenceWithUntagged1& object)
        // Pass the specified 'object' to the specified 'hashAlg'.  This
        // function integrates with the 'bslh' modular hashing system and
        // effectively provides a 'bsl::hash' specialization for
        // 'EncoderTestSequenceWithUntagged1'.
    {
        using bslh::hashAppend;
        hashAppend(hashAlg, object.sequence());
    }
};

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

    // PRIVATE ACCESSORS
    template <typename t_HASH_ALGORITHM>
    void hashAppendImpl(t_HASH_ALGORITHM& hashAlgorithm) const;

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


    // MANIPULATORS
    void reset();
        // Reset this object to the default value (i.e., its value upon
        // default construction).

    template <typename t_MANIPULATOR>
    int manipulateAttributes(t_MANIPULATOR& manipulator);
        // Invoke the specified 'manipulator' sequentially on the address of
        // each (modifiable) attribute of this object, supplying 'manipulator'
        // with the corresponding attribute information structure until such
        // invocation returns a non-zero value.  Return the value from the
        // last invocation of 'manipulator' (i.e., the invocation that
        // terminated the sequence).

    template <typename t_MANIPULATOR>
    int manipulateAttribute(t_MANIPULATOR& manipulator, int id);
        // Invoke the specified 'manipulator' on the address of
        // the (modifiable) attribute indicated by the specified 'id',
        // supplying 'manipulator' with the corresponding attribute
        // information structure.  Return the value returned from the
        // invocation of 'manipulator' if 'id' identifies an attribute of this
        // class, and -1 otherwise.

    template <typename t_MANIPULATOR>
    int manipulateAttribute(t_MANIPULATOR&  manipulator,
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
                        int           level          = 0,
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

    template <typename t_ACCESSOR>
    int accessAttributes(t_ACCESSOR& accessor) const;
        // Invoke the specified 'accessor' sequentially on each
        // (non-modifiable) attribute of this object, supplying 'accessor'
        // with the corresponding attribute information structure until such
        // invocation returns a non-zero value.  Return the value from the
        // last invocation of 'accessor' (i.e., the invocation that terminated
        // the sequence).

    template <typename t_ACCESSOR>
    int accessAttribute(t_ACCESSOR& accessor, int id) const;
        // Invoke the specified 'accessor' on the (non-modifiable) attribute
        // of this object indicated by the specified 'id', supplying 'accessor'
        // with the corresponding attribute information structure.  Return the
        // value returned from the invocation of 'accessor' if 'id' identifies
        // an attribute of this class, and -1 otherwise.

    template <typename t_ACCESSOR>
    int accessAttribute(t_ACCESSOR&   accessor,
                        const char *name,
                        int         nameLength) const;
        // Invoke the specified 'accessor' on the (non-modifiable) attribute
        // of this object indicated by the specified 'name' of the specified
        // 'nameLength', supplying 'accessor' with the corresponding attribute
        // information structure.  Return the value returned from the
        // invocation of 'accessor' if 'name' identifies an attribute of this
        // class, and -1 otherwise.

    const EncoderTestSequenceWithUntagged10Sequence& sequence() const;
        // Return a reference offering non-modifiable access to the "Sequence"
        // attribute of this object.

    int attribute0() const;
        // Return the value of the "Attribute0" attribute of this object.

    int attribute1() const;
        // Return the value of the "Attribute1" attribute of this object.

    // HIDDEN FRIENDS
    friend bool operator==(const EncoderTestSequenceWithUntagged10& lhs,
                           const EncoderTestSequenceWithUntagged10& rhs)
        // Return 'true' if the specified 'lhs' and 'rhs' attribute objects
        // have the same value, and 'false' otherwise.  Two attribute objects
        // have the same value if each respective attribute has the same value.
    {
        return lhs.sequence() == rhs.sequence() &&
               lhs.attribute0() == rhs.attribute0() &&
               lhs.attribute1() == rhs.attribute1();
    }

    friend bool operator!=(const EncoderTestSequenceWithUntagged10& lhs,
                           const EncoderTestSequenceWithUntagged10& rhs)
        // Returns '!(lhs == rhs)'
    {
        return !(lhs == rhs);
    }

    friend bsl::ostream& operator<<(
                               bsl::ostream&                            stream,
                               const EncoderTestSequenceWithUntagged10& rhs)
        // Format the specified 'rhs' to the specified output 'stream' and
        // return a reference to the modifiable 'stream'.
    {
        return rhs.print(stream, 0, -1);
    }

    template <typename t_HASH_ALGORITHM>
    friend void hashAppend(t_HASH_ALGORITHM&                        hashAlg,
                           const EncoderTestSequenceWithUntagged10& object)
        // Pass the specified 'object' to the specified 'hashAlg'.  This
        // function integrates with the 'bslh' modular hashing system and
        // effectively provides a 'bsl::hash' specialization for
        // 'EncoderTestSequenceWithUntagged10'.
    {
        object.hashAppendImpl(hashAlg);
    }
};

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

    // PRIVATE ACCESSORS
    template <typename t_HASH_ALGORITHM>
    void hashAppendImpl(t_HASH_ALGORITHM& hashAlgorithm) const;

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


    // MANIPULATORS
    void reset();
        // Reset this object to the default value (i.e., its value upon
        // default construction).

    template <typename t_MANIPULATOR>
    int manipulateAttributes(t_MANIPULATOR& manipulator);
        // Invoke the specified 'manipulator' sequentially on the address of
        // each (modifiable) attribute of this object, supplying 'manipulator'
        // with the corresponding attribute information structure until such
        // invocation returns a non-zero value.  Return the value from the
        // last invocation of 'manipulator' (i.e., the invocation that
        // terminated the sequence).

    template <typename t_MANIPULATOR>
    int manipulateAttribute(t_MANIPULATOR& manipulator, int id);
        // Invoke the specified 'manipulator' on the address of
        // the (modifiable) attribute indicated by the specified 'id',
        // supplying 'manipulator' with the corresponding attribute
        // information structure.  Return the value returned from the
        // invocation of 'manipulator' if 'id' identifies an attribute of this
        // class, and -1 otherwise.

    template <typename t_MANIPULATOR>
    int manipulateAttribute(t_MANIPULATOR&  manipulator,
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
                        int           level          = 0,
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

    template <typename t_ACCESSOR>
    int accessAttributes(t_ACCESSOR& accessor) const;
        // Invoke the specified 'accessor' sequentially on each
        // (non-modifiable) attribute of this object, supplying 'accessor'
        // with the corresponding attribute information structure until such
        // invocation returns a non-zero value.  Return the value from the
        // last invocation of 'accessor' (i.e., the invocation that terminated
        // the sequence).

    template <typename t_ACCESSOR>
    int accessAttribute(t_ACCESSOR& accessor, int id) const;
        // Invoke the specified 'accessor' on the (non-modifiable) attribute
        // of this object indicated by the specified 'id', supplying 'accessor'
        // with the corresponding attribute information structure.  Return the
        // value returned from the invocation of 'accessor' if 'id' identifies
        // an attribute of this class, and -1 otherwise.

    template <typename t_ACCESSOR>
    int accessAttribute(t_ACCESSOR&   accessor,
                        const char *name,
                        int         nameLength) const;
        // Invoke the specified 'accessor' on the (non-modifiable) attribute
        // of this object indicated by the specified 'name' of the specified
        // 'nameLength', supplying 'accessor' with the corresponding attribute
        // information structure.  Return the value returned from the
        // invocation of 'accessor' if 'name' identifies an attribute of this
        // class, and -1 otherwise.

    int attribute0() const;
        // Return the value of the "Attribute0" attribute of this object.

    const EncoderTestSequenceWithUntagged11Sequence& sequence() const;
        // Return a reference offering non-modifiable access to the "Sequence"
        // attribute of this object.

    const EncoderTestSequenceWithUntagged11Sequence1& sequence1() const;
        // Return a reference offering non-modifiable access to the "Sequence1"
        // attribute of this object.

    // HIDDEN FRIENDS
    friend bool operator==(const EncoderTestSequenceWithUntagged11& lhs,
                           const EncoderTestSequenceWithUntagged11& rhs)
        // Return 'true' if the specified 'lhs' and 'rhs' attribute objects
        // have the same value, and 'false' otherwise.  Two attribute objects
        // have the same value if each respective attribute has the same value.
    {
        return lhs.attribute0() == rhs.attribute0() &&
               lhs.sequence() == rhs.sequence() &&
               lhs.sequence1() == rhs.sequence1();
    }

    friend bool operator!=(const EncoderTestSequenceWithUntagged11& lhs,
                           const EncoderTestSequenceWithUntagged11& rhs)
        // Returns '!(lhs == rhs)'
    {
        return !(lhs == rhs);
    }

    friend bsl::ostream& operator<<(
                               bsl::ostream&                            stream,
                               const EncoderTestSequenceWithUntagged11& rhs)
        // Format the specified 'rhs' to the specified output 'stream' and
        // return a reference to the modifiable 'stream'.
    {
        return rhs.print(stream, 0, -1);
    }

    template <typename t_HASH_ALGORITHM>
    friend void hashAppend(t_HASH_ALGORITHM&                        hashAlg,
                           const EncoderTestSequenceWithUntagged11& object)
        // Pass the specified 'object' to the specified 'hashAlg'.  This
        // function integrates with the 'bslh' modular hashing system and
        // effectively provides a 'bsl::hash' specialization for
        // 'EncoderTestSequenceWithUntagged11'.
    {
        object.hashAppendImpl(hashAlg);
    }
};

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

    // PRIVATE ACCESSORS
    template <typename t_HASH_ALGORITHM>
    void hashAppendImpl(t_HASH_ALGORITHM& hashAlgorithm) const;

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


    // MANIPULATORS
    void reset();
        // Reset this object to the default value (i.e., its value upon
        // default construction).

    template <typename t_MANIPULATOR>
    int manipulateAttributes(t_MANIPULATOR& manipulator);
        // Invoke the specified 'manipulator' sequentially on the address of
        // each (modifiable) attribute of this object, supplying 'manipulator'
        // with the corresponding attribute information structure until such
        // invocation returns a non-zero value.  Return the value from the
        // last invocation of 'manipulator' (i.e., the invocation that
        // terminated the sequence).

    template <typename t_MANIPULATOR>
    int manipulateAttribute(t_MANIPULATOR& manipulator, int id);
        // Invoke the specified 'manipulator' on the address of
        // the (modifiable) attribute indicated by the specified 'id',
        // supplying 'manipulator' with the corresponding attribute
        // information structure.  Return the value returned from the
        // invocation of 'manipulator' if 'id' identifies an attribute of this
        // class, and -1 otherwise.

    template <typename t_MANIPULATOR>
    int manipulateAttribute(t_MANIPULATOR&  manipulator,
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
                        int           level          = 0,
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

    template <typename t_ACCESSOR>
    int accessAttributes(t_ACCESSOR& accessor) const;
        // Invoke the specified 'accessor' sequentially on each
        // (non-modifiable) attribute of this object, supplying 'accessor'
        // with the corresponding attribute information structure until such
        // invocation returns a non-zero value.  Return the value from the
        // last invocation of 'accessor' (i.e., the invocation that terminated
        // the sequence).

    template <typename t_ACCESSOR>
    int accessAttribute(t_ACCESSOR& accessor, int id) const;
        // Invoke the specified 'accessor' on the (non-modifiable) attribute
        // of this object indicated by the specified 'id', supplying 'accessor'
        // with the corresponding attribute information structure.  Return the
        // value returned from the invocation of 'accessor' if 'id' identifies
        // an attribute of this class, and -1 otherwise.

    template <typename t_ACCESSOR>
    int accessAttribute(t_ACCESSOR&   accessor,
                        const char *name,
                        int         nameLength) const;
        // Invoke the specified 'accessor' on the (non-modifiable) attribute
        // of this object indicated by the specified 'name' of the specified
        // 'nameLength', supplying 'accessor' with the corresponding attribute
        // information structure.  Return the value returned from the
        // invocation of 'accessor' if 'name' identifies an attribute of this
        // class, and -1 otherwise.

    int attribute0() const;
        // Return the value of the "Attribute0" attribute of this object.

    const EncoderTestSequenceWithUntagged12Sequence& sequence() const;
        // Return a reference offering non-modifiable access to the "Sequence"
        // attribute of this object.

    int attribute1() const;
        // Return the value of the "Attribute1" attribute of this object.

    // HIDDEN FRIENDS
    friend bool operator==(const EncoderTestSequenceWithUntagged12& lhs,
                           const EncoderTestSequenceWithUntagged12& rhs)
        // Return 'true' if the specified 'lhs' and 'rhs' attribute objects
        // have the same value, and 'false' otherwise.  Two attribute objects
        // have the same value if each respective attribute has the same value.
    {
        return lhs.attribute0() == rhs.attribute0() &&
               lhs.sequence() == rhs.sequence() &&
               lhs.attribute1() == rhs.attribute1();
    }

    friend bool operator!=(const EncoderTestSequenceWithUntagged12& lhs,
                           const EncoderTestSequenceWithUntagged12& rhs)
        // Returns '!(lhs == rhs)'
    {
        return !(lhs == rhs);
    }

    friend bsl::ostream& operator<<(
                               bsl::ostream&                            stream,
                               const EncoderTestSequenceWithUntagged12& rhs)
        // Format the specified 'rhs' to the specified output 'stream' and
        // return a reference to the modifiable 'stream'.
    {
        return rhs.print(stream, 0, -1);
    }

    template <typename t_HASH_ALGORITHM>
    friend void hashAppend(t_HASH_ALGORITHM&                        hashAlg,
                           const EncoderTestSequenceWithUntagged12& object)
        // Pass the specified 'object' to the specified 'hashAlg'.  This
        // function integrates with the 'bslh' modular hashing system and
        // effectively provides a 'bsl::hash' specialization for
        // 'EncoderTestSequenceWithUntagged12'.
    {
        object.hashAppendImpl(hashAlg);
    }
};

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

    // PRIVATE ACCESSORS
    template <typename t_HASH_ALGORITHM>
    void hashAppendImpl(t_HASH_ALGORITHM& hashAlgorithm) const;

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


    // MANIPULATORS
    void reset();
        // Reset this object to the default value (i.e., its value upon
        // default construction).

    template <typename t_MANIPULATOR>
    int manipulateAttributes(t_MANIPULATOR& manipulator);
        // Invoke the specified 'manipulator' sequentially on the address of
        // each (modifiable) attribute of this object, supplying 'manipulator'
        // with the corresponding attribute information structure until such
        // invocation returns a non-zero value.  Return the value from the
        // last invocation of 'manipulator' (i.e., the invocation that
        // terminated the sequence).

    template <typename t_MANIPULATOR>
    int manipulateAttribute(t_MANIPULATOR& manipulator, int id);
        // Invoke the specified 'manipulator' on the address of
        // the (modifiable) attribute indicated by the specified 'id',
        // supplying 'manipulator' with the corresponding attribute
        // information structure.  Return the value returned from the
        // invocation of 'manipulator' if 'id' identifies an attribute of this
        // class, and -1 otherwise.

    template <typename t_MANIPULATOR>
    int manipulateAttribute(t_MANIPULATOR&  manipulator,
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
                        int           level          = 0,
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

    template <typename t_ACCESSOR>
    int accessAttributes(t_ACCESSOR& accessor) const;
        // Invoke the specified 'accessor' sequentially on each
        // (non-modifiable) attribute of this object, supplying 'accessor'
        // with the corresponding attribute information structure until such
        // invocation returns a non-zero value.  Return the value from the
        // last invocation of 'accessor' (i.e., the invocation that terminated
        // the sequence).

    template <typename t_ACCESSOR>
    int accessAttribute(t_ACCESSOR& accessor, int id) const;
        // Invoke the specified 'accessor' on the (non-modifiable) attribute
        // of this object indicated by the specified 'id', supplying 'accessor'
        // with the corresponding attribute information structure.  Return the
        // value returned from the invocation of 'accessor' if 'id' identifies
        // an attribute of this class, and -1 otherwise.

    template <typename t_ACCESSOR>
    int accessAttribute(t_ACCESSOR&   accessor,
                        const char *name,
                        int         nameLength) const;
        // Invoke the specified 'accessor' on the (non-modifiable) attribute
        // of this object indicated by the specified 'name' of the specified
        // 'nameLength', supplying 'accessor' with the corresponding attribute
        // information structure.  Return the value returned from the
        // invocation of 'accessor' if 'name' identifies an attribute of this
        // class, and -1 otherwise.

    int attribute0() const;
        // Return the value of the "Attribute0" attribute of this object.

    int attribute1() const;
        // Return the value of the "Attribute1" attribute of this object.

    const EncoderTestSequenceWithUntagged13Sequence& sequence() const;
        // Return a reference offering non-modifiable access to the "Sequence"
        // attribute of this object.

    // HIDDEN FRIENDS
    friend bool operator==(const EncoderTestSequenceWithUntagged13& lhs,
                           const EncoderTestSequenceWithUntagged13& rhs)
        // Return 'true' if the specified 'lhs' and 'rhs' attribute objects
        // have the same value, and 'false' otherwise.  Two attribute objects
        // have the same value if each respective attribute has the same value.
    {
        return lhs.attribute0() == rhs.attribute0() &&
               lhs.attribute1() == rhs.attribute1() &&
               lhs.sequence() == rhs.sequence();
    }

    friend bool operator!=(const EncoderTestSequenceWithUntagged13& lhs,
                           const EncoderTestSequenceWithUntagged13& rhs)
        // Returns '!(lhs == rhs)'
    {
        return !(lhs == rhs);
    }

    friend bsl::ostream& operator<<(
                               bsl::ostream&                            stream,
                               const EncoderTestSequenceWithUntagged13& rhs)
        // Format the specified 'rhs' to the specified output 'stream' and
        // return a reference to the modifiable 'stream'.
    {
        return rhs.print(stream, 0, -1);
    }

    template <typename t_HASH_ALGORITHM>
    friend void hashAppend(t_HASH_ALGORITHM&                        hashAlg,
                           const EncoderTestSequenceWithUntagged13& object)
        // Pass the specified 'object' to the specified 'hashAlg'.  This
        // function integrates with the 'bslh' modular hashing system and
        // effectively provides a 'bsl::hash' specialization for
        // 'EncoderTestSequenceWithUntagged13'.
    {
        object.hashAppendImpl(hashAlg);
    }
};

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


    // MANIPULATORS
    void reset();
        // Reset this object to the default value (i.e., its value upon
        // default construction).

    template <typename t_MANIPULATOR>
    int manipulateAttributes(t_MANIPULATOR& manipulator);
        // Invoke the specified 'manipulator' sequentially on the address of
        // each (modifiable) attribute of this object, supplying 'manipulator'
        // with the corresponding attribute information structure until such
        // invocation returns a non-zero value.  Return the value from the
        // last invocation of 'manipulator' (i.e., the invocation that
        // terminated the sequence).

    template <typename t_MANIPULATOR>
    int manipulateAttribute(t_MANIPULATOR& manipulator, int id);
        // Invoke the specified 'manipulator' on the address of
        // the (modifiable) attribute indicated by the specified 'id',
        // supplying 'manipulator' with the corresponding attribute
        // information structure.  Return the value returned from the
        // invocation of 'manipulator' if 'id' identifies an attribute of this
        // class, and -1 otherwise.

    template <typename t_MANIPULATOR>
    int manipulateAttribute(t_MANIPULATOR&  manipulator,
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
                        int           level          = 0,
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

    template <typename t_ACCESSOR>
    int accessAttributes(t_ACCESSOR& accessor) const;
        // Invoke the specified 'accessor' sequentially on each
        // (non-modifiable) attribute of this object, supplying 'accessor'
        // with the corresponding attribute information structure until such
        // invocation returns a non-zero value.  Return the value from the
        // last invocation of 'accessor' (i.e., the invocation that terminated
        // the sequence).

    template <typename t_ACCESSOR>
    int accessAttribute(t_ACCESSOR& accessor, int id) const;
        // Invoke the specified 'accessor' on the (non-modifiable) attribute
        // of this object indicated by the specified 'id', supplying 'accessor'
        // with the corresponding attribute information structure.  Return the
        // value returned from the invocation of 'accessor' if 'id' identifies
        // an attribute of this class, and -1 otherwise.

    template <typename t_ACCESSOR>
    int accessAttribute(t_ACCESSOR&   accessor,
                        const char *name,
                        int         nameLength) const;
        // Invoke the specified 'accessor' on the (non-modifiable) attribute
        // of this object indicated by the specified 'name' of the specified
        // 'nameLength', supplying 'accessor' with the corresponding attribute
        // information structure.  Return the value returned from the
        // invocation of 'accessor' if 'name' identifies an attribute of this
        // class, and -1 otherwise.

    const EncoderTestSequenceWithUntagged3Sequence& sequence() const;
        // Return a reference offering non-modifiable access to the "Sequence"
        // attribute of this object.

    const EncoderTestSequenceWithUntagged3Sequence1& sequence1() const;
        // Return a reference offering non-modifiable access to the "Sequence1"
        // attribute of this object.

    // HIDDEN FRIENDS
    friend bool operator==(const EncoderTestSequenceWithUntagged3& lhs,
                           const EncoderTestSequenceWithUntagged3& rhs)
        // Return 'true' if the specified 'lhs' and 'rhs' attribute objects
        // have the same value, and 'false' otherwise.  Two attribute objects
        // have the same value if each respective attribute has the same value.
    {
        return lhs.sequence() == rhs.sequence() &&
               lhs.sequence1() == rhs.sequence1();
    }

    friend bool operator!=(const EncoderTestSequenceWithUntagged3& lhs,
                           const EncoderTestSequenceWithUntagged3& rhs)
        // Returns '!(lhs == rhs)'
    {
        return !(lhs == rhs);
    }

    friend bsl::ostream& operator<<(
                                bsl::ostream&                           stream,
                                const EncoderTestSequenceWithUntagged3& rhs)
        // Format the specified 'rhs' to the specified output 'stream' and
        // return a reference to the modifiable 'stream'.
    {
        return rhs.print(stream, 0, -1);
    }

    template <typename t_HASH_ALGORITHM>
    friend void hashAppend(t_HASH_ALGORITHM&                       hashAlg,
                           const EncoderTestSequenceWithUntagged3& object)
        // Pass the specified 'object' to the specified 'hashAlg'.  This
        // function integrates with the 'bslh' modular hashing system and
        // effectively provides a 'bsl::hash' specialization for
        // 'EncoderTestSequenceWithUntagged3'.
    {
        using bslh::hashAppend;
        hashAppend(hashAlg, object.sequence());
        hashAppend(hashAlg, object.sequence1());
    }
};

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


    // MANIPULATORS
    void reset();
        // Reset this object to the default value (i.e., its value upon
        // default construction).

    template <typename t_MANIPULATOR>
    int manipulateAttributes(t_MANIPULATOR& manipulator);
        // Invoke the specified 'manipulator' sequentially on the address of
        // each (modifiable) attribute of this object, supplying 'manipulator'
        // with the corresponding attribute information structure until such
        // invocation returns a non-zero value.  Return the value from the
        // last invocation of 'manipulator' (i.e., the invocation that
        // terminated the sequence).

    template <typename t_MANIPULATOR>
    int manipulateAttribute(t_MANIPULATOR& manipulator, int id);
        // Invoke the specified 'manipulator' on the address of
        // the (modifiable) attribute indicated by the specified 'id',
        // supplying 'manipulator' with the corresponding attribute
        // information structure.  Return the value returned from the
        // invocation of 'manipulator' if 'id' identifies an attribute of this
        // class, and -1 otherwise.

    template <typename t_MANIPULATOR>
    int manipulateAttribute(t_MANIPULATOR&  manipulator,
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
                        int           level          = 0,
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

    template <typename t_ACCESSOR>
    int accessAttributes(t_ACCESSOR& accessor) const;
        // Invoke the specified 'accessor' sequentially on each
        // (non-modifiable) attribute of this object, supplying 'accessor'
        // with the corresponding attribute information structure until such
        // invocation returns a non-zero value.  Return the value from the
        // last invocation of 'accessor' (i.e., the invocation that terminated
        // the sequence).

    template <typename t_ACCESSOR>
    int accessAttribute(t_ACCESSOR& accessor, int id) const;
        // Invoke the specified 'accessor' on the (non-modifiable) attribute
        // of this object indicated by the specified 'id', supplying 'accessor'
        // with the corresponding attribute information structure.  Return the
        // value returned from the invocation of 'accessor' if 'id' identifies
        // an attribute of this class, and -1 otherwise.

    template <typename t_ACCESSOR>
    int accessAttribute(t_ACCESSOR&   accessor,
                        const char *name,
                        int         nameLength) const;
        // Invoke the specified 'accessor' on the (non-modifiable) attribute
        // of this object indicated by the specified 'name' of the specified
        // 'nameLength', supplying 'accessor' with the corresponding attribute
        // information structure.  Return the value returned from the
        // invocation of 'accessor' if 'name' identifies an attribute of this
        // class, and -1 otherwise.

    const EncoderTestSequenceWithUntagged4Sequence& sequence() const;
        // Return a reference offering non-modifiable access to the "Sequence"
        // attribute of this object.

    int attribute0() const;
        // Return the value of the "Attribute0" attribute of this object.

    // HIDDEN FRIENDS
    friend bool operator==(const EncoderTestSequenceWithUntagged4& lhs,
                           const EncoderTestSequenceWithUntagged4& rhs)
        // Return 'true' if the specified 'lhs' and 'rhs' attribute objects
        // have the same value, and 'false' otherwise.  Two attribute objects
        // have the same value if each respective attribute has the same value.
    {
        return lhs.sequence() == rhs.sequence() &&
               lhs.attribute0() == rhs.attribute0();
    }

    friend bool operator!=(const EncoderTestSequenceWithUntagged4& lhs,
                           const EncoderTestSequenceWithUntagged4& rhs)
        // Returns '!(lhs == rhs)'
    {
        return !(lhs == rhs);
    }

    friend bsl::ostream& operator<<(
                                bsl::ostream&                           stream,
                                const EncoderTestSequenceWithUntagged4& rhs)
        // Format the specified 'rhs' to the specified output 'stream' and
        // return a reference to the modifiable 'stream'.
    {
        return rhs.print(stream, 0, -1);
    }

    template <typename t_HASH_ALGORITHM>
    friend void hashAppend(t_HASH_ALGORITHM&                       hashAlg,
                           const EncoderTestSequenceWithUntagged4& object)
        // Pass the specified 'object' to the specified 'hashAlg'.  This
        // function integrates with the 'bslh' modular hashing system and
        // effectively provides a 'bsl::hash' specialization for
        // 'EncoderTestSequenceWithUntagged4'.
    {
        using bslh::hashAppend;
        hashAppend(hashAlg, object.sequence());
        hashAppend(hashAlg, object.attribute0());
    }
};

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


    // MANIPULATORS
    void reset();
        // Reset this object to the default value (i.e., its value upon
        // default construction).

    template <typename t_MANIPULATOR>
    int manipulateAttributes(t_MANIPULATOR& manipulator);
        // Invoke the specified 'manipulator' sequentially on the address of
        // each (modifiable) attribute of this object, supplying 'manipulator'
        // with the corresponding attribute information structure until such
        // invocation returns a non-zero value.  Return the value from the
        // last invocation of 'manipulator' (i.e., the invocation that
        // terminated the sequence).

    template <typename t_MANIPULATOR>
    int manipulateAttribute(t_MANIPULATOR& manipulator, int id);
        // Invoke the specified 'manipulator' on the address of
        // the (modifiable) attribute indicated by the specified 'id',
        // supplying 'manipulator' with the corresponding attribute
        // information structure.  Return the value returned from the
        // invocation of 'manipulator' if 'id' identifies an attribute of this
        // class, and -1 otherwise.

    template <typename t_MANIPULATOR>
    int manipulateAttribute(t_MANIPULATOR&  manipulator,
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
                        int           level          = 0,
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

    template <typename t_ACCESSOR>
    int accessAttributes(t_ACCESSOR& accessor) const;
        // Invoke the specified 'accessor' sequentially on each
        // (non-modifiable) attribute of this object, supplying 'accessor'
        // with the corresponding attribute information structure until such
        // invocation returns a non-zero value.  Return the value from the
        // last invocation of 'accessor' (i.e., the invocation that terminated
        // the sequence).

    template <typename t_ACCESSOR>
    int accessAttribute(t_ACCESSOR& accessor, int id) const;
        // Invoke the specified 'accessor' on the (non-modifiable) attribute
        // of this object indicated by the specified 'id', supplying 'accessor'
        // with the corresponding attribute information structure.  Return the
        // value returned from the invocation of 'accessor' if 'id' identifies
        // an attribute of this class, and -1 otherwise.

    template <typename t_ACCESSOR>
    int accessAttribute(t_ACCESSOR&   accessor,
                        const char *name,
                        int         nameLength) const;
        // Invoke the specified 'accessor' on the (non-modifiable) attribute
        // of this object indicated by the specified 'name' of the specified
        // 'nameLength', supplying 'accessor' with the corresponding attribute
        // information structure.  Return the value returned from the
        // invocation of 'accessor' if 'name' identifies an attribute of this
        // class, and -1 otherwise.

    int attribute0() const;
        // Return the value of the "Attribute0" attribute of this object.

    const EncoderTestSequenceWithUntagged5Sequence& sequence() const;
        // Return a reference offering non-modifiable access to the "Sequence"
        // attribute of this object.

    // HIDDEN FRIENDS
    friend bool operator==(const EncoderTestSequenceWithUntagged5& lhs,
                           const EncoderTestSequenceWithUntagged5& rhs)
        // Return 'true' if the specified 'lhs' and 'rhs' attribute objects
        // have the same value, and 'false' otherwise.  Two attribute objects
        // have the same value if each respective attribute has the same value.
    {
        return lhs.attribute0() == rhs.attribute0() &&
               lhs.sequence() == rhs.sequence();
    }

    friend bool operator!=(const EncoderTestSequenceWithUntagged5& lhs,
                           const EncoderTestSequenceWithUntagged5& rhs)
        // Returns '!(lhs == rhs)'
    {
        return !(lhs == rhs);
    }

    friend bsl::ostream& operator<<(
                                bsl::ostream&                           stream,
                                const EncoderTestSequenceWithUntagged5& rhs)
        // Format the specified 'rhs' to the specified output 'stream' and
        // return a reference to the modifiable 'stream'.
    {
        return rhs.print(stream, 0, -1);
    }

    template <typename t_HASH_ALGORITHM>
    friend void hashAppend(t_HASH_ALGORITHM&                       hashAlg,
                           const EncoderTestSequenceWithUntagged5& object)
        // Pass the specified 'object' to the specified 'hashAlg'.  This
        // function integrates with the 'bslh' modular hashing system and
        // effectively provides a 'bsl::hash' specialization for
        // 'EncoderTestSequenceWithUntagged5'.
    {
        using bslh::hashAppend;
        hashAppend(hashAlg, object.attribute0());
        hashAppend(hashAlg, object.sequence());
    }
};

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

    // PRIVATE ACCESSORS
    template <typename t_HASH_ALGORITHM>
    void hashAppendImpl(t_HASH_ALGORITHM& hashAlgorithm) const;

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


    // MANIPULATORS
    void reset();
        // Reset this object to the default value (i.e., its value upon
        // default construction).

    template <typename t_MANIPULATOR>
    int manipulateAttributes(t_MANIPULATOR& manipulator);
        // Invoke the specified 'manipulator' sequentially on the address of
        // each (modifiable) attribute of this object, supplying 'manipulator'
        // with the corresponding attribute information structure until such
        // invocation returns a non-zero value.  Return the value from the
        // last invocation of 'manipulator' (i.e., the invocation that
        // terminated the sequence).

    template <typename t_MANIPULATOR>
    int manipulateAttribute(t_MANIPULATOR& manipulator, int id);
        // Invoke the specified 'manipulator' on the address of
        // the (modifiable) attribute indicated by the specified 'id',
        // supplying 'manipulator' with the corresponding attribute
        // information structure.  Return the value returned from the
        // invocation of 'manipulator' if 'id' identifies an attribute of this
        // class, and -1 otherwise.

    template <typename t_MANIPULATOR>
    int manipulateAttribute(t_MANIPULATOR&  manipulator,
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
                        int           level          = 0,
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

    template <typename t_ACCESSOR>
    int accessAttributes(t_ACCESSOR& accessor) const;
        // Invoke the specified 'accessor' sequentially on each
        // (non-modifiable) attribute of this object, supplying 'accessor'
        // with the corresponding attribute information structure until such
        // invocation returns a non-zero value.  Return the value from the
        // last invocation of 'accessor' (i.e., the invocation that terminated
        // the sequence).

    template <typename t_ACCESSOR>
    int accessAttribute(t_ACCESSOR& accessor, int id) const;
        // Invoke the specified 'accessor' on the (non-modifiable) attribute
        // of this object indicated by the specified 'id', supplying 'accessor'
        // with the corresponding attribute information structure.  Return the
        // value returned from the invocation of 'accessor' if 'id' identifies
        // an attribute of this class, and -1 otherwise.

    template <typename t_ACCESSOR>
    int accessAttribute(t_ACCESSOR&   accessor,
                        const char *name,
                        int         nameLength) const;
        // Invoke the specified 'accessor' on the (non-modifiable) attribute
        // of this object indicated by the specified 'name' of the specified
        // 'nameLength', supplying 'accessor' with the corresponding attribute
        // information structure.  Return the value returned from the
        // invocation of 'accessor' if 'name' identifies an attribute of this
        // class, and -1 otherwise.

    const EncoderTestSequenceWithUntagged7Sequence& sequence() const;
        // Return a reference offering non-modifiable access to the "Sequence"
        // attribute of this object.

    const EncoderTestSequenceWithUntagged7Sequence1& sequence1() const;
        // Return a reference offering non-modifiable access to the "Sequence1"
        // attribute of this object.

    const EncoderTestSequenceWithUntagged7Sequence2& sequence2() const;
        // Return a reference offering non-modifiable access to the "Sequence2"
        // attribute of this object.

    // HIDDEN FRIENDS
    friend bool operator==(const EncoderTestSequenceWithUntagged7& lhs,
                           const EncoderTestSequenceWithUntagged7& rhs)
        // Return 'true' if the specified 'lhs' and 'rhs' attribute objects
        // have the same value, and 'false' otherwise.  Two attribute objects
        // have the same value if each respective attribute has the same value.
    {
        return lhs.sequence() == rhs.sequence() &&
               lhs.sequence1() == rhs.sequence1() &&
               lhs.sequence2() == rhs.sequence2();
    }

    friend bool operator!=(const EncoderTestSequenceWithUntagged7& lhs,
                           const EncoderTestSequenceWithUntagged7& rhs)
        // Returns '!(lhs == rhs)'
    {
        return !(lhs == rhs);
    }

    friend bsl::ostream& operator<<(
                                bsl::ostream&                           stream,
                                const EncoderTestSequenceWithUntagged7& rhs)
        // Format the specified 'rhs' to the specified output 'stream' and
        // return a reference to the modifiable 'stream'.
    {
        return rhs.print(stream, 0, -1);
    }

    template <typename t_HASH_ALGORITHM>
    friend void hashAppend(t_HASH_ALGORITHM&                       hashAlg,
                           const EncoderTestSequenceWithUntagged7& object)
        // Pass the specified 'object' to the specified 'hashAlg'.  This
        // function integrates with the 'bslh' modular hashing system and
        // effectively provides a 'bsl::hash' specialization for
        // 'EncoderTestSequenceWithUntagged7'.
    {
        object.hashAppendImpl(hashAlg);
    }
};

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

    // PRIVATE ACCESSORS
    template <typename t_HASH_ALGORITHM>
    void hashAppendImpl(t_HASH_ALGORITHM& hashAlgorithm) const;

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


    // MANIPULATORS
    void reset();
        // Reset this object to the default value (i.e., its value upon
        // default construction).

    template <typename t_MANIPULATOR>
    int manipulateAttributes(t_MANIPULATOR& manipulator);
        // Invoke the specified 'manipulator' sequentially on the address of
        // each (modifiable) attribute of this object, supplying 'manipulator'
        // with the corresponding attribute information structure until such
        // invocation returns a non-zero value.  Return the value from the
        // last invocation of 'manipulator' (i.e., the invocation that
        // terminated the sequence).

    template <typename t_MANIPULATOR>
    int manipulateAttribute(t_MANIPULATOR& manipulator, int id);
        // Invoke the specified 'manipulator' on the address of
        // the (modifiable) attribute indicated by the specified 'id',
        // supplying 'manipulator' with the corresponding attribute
        // information structure.  Return the value returned from the
        // invocation of 'manipulator' if 'id' identifies an attribute of this
        // class, and -1 otherwise.

    template <typename t_MANIPULATOR>
    int manipulateAttribute(t_MANIPULATOR&  manipulator,
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
                        int           level          = 0,
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

    template <typename t_ACCESSOR>
    int accessAttributes(t_ACCESSOR& accessor) const;
        // Invoke the specified 'accessor' sequentially on each
        // (non-modifiable) attribute of this object, supplying 'accessor'
        // with the corresponding attribute information structure until such
        // invocation returns a non-zero value.  Return the value from the
        // last invocation of 'accessor' (i.e., the invocation that terminated
        // the sequence).

    template <typename t_ACCESSOR>
    int accessAttribute(t_ACCESSOR& accessor, int id) const;
        // Invoke the specified 'accessor' on the (non-modifiable) attribute
        // of this object indicated by the specified 'id', supplying 'accessor'
        // with the corresponding attribute information structure.  Return the
        // value returned from the invocation of 'accessor' if 'id' identifies
        // an attribute of this class, and -1 otherwise.

    template <typename t_ACCESSOR>
    int accessAttribute(t_ACCESSOR&   accessor,
                        const char *name,
                        int         nameLength) const;
        // Invoke the specified 'accessor' on the (non-modifiable) attribute
        // of this object indicated by the specified 'name' of the specified
        // 'nameLength', supplying 'accessor' with the corresponding attribute
        // information structure.  Return the value returned from the
        // invocation of 'accessor' if 'name' identifies an attribute of this
        // class, and -1 otherwise.

    const EncoderTestSequenceWithUntagged8Sequence& sequence() const;
        // Return a reference offering non-modifiable access to the "Sequence"
        // attribute of this object.

    const EncoderTestSequenceWithUntagged8Sequence1& sequence1() const;
        // Return a reference offering non-modifiable access to the "Sequence1"
        // attribute of this object.

    int attribute0() const;
        // Return the value of the "Attribute0" attribute of this object.

    // HIDDEN FRIENDS
    friend bool operator==(const EncoderTestSequenceWithUntagged8& lhs,
                           const EncoderTestSequenceWithUntagged8& rhs)
        // Return 'true' if the specified 'lhs' and 'rhs' attribute objects
        // have the same value, and 'false' otherwise.  Two attribute objects
        // have the same value if each respective attribute has the same value.
    {
        return lhs.sequence() == rhs.sequence() &&
               lhs.sequence1() == rhs.sequence1() &&
               lhs.attribute0() == rhs.attribute0();
    }

    friend bool operator!=(const EncoderTestSequenceWithUntagged8& lhs,
                           const EncoderTestSequenceWithUntagged8& rhs)
        // Returns '!(lhs == rhs)'
    {
        return !(lhs == rhs);
    }

    friend bsl::ostream& operator<<(
                                bsl::ostream&                           stream,
                                const EncoderTestSequenceWithUntagged8& rhs)
        // Format the specified 'rhs' to the specified output 'stream' and
        // return a reference to the modifiable 'stream'.
    {
        return rhs.print(stream, 0, -1);
    }

    template <typename t_HASH_ALGORITHM>
    friend void hashAppend(t_HASH_ALGORITHM&                       hashAlg,
                           const EncoderTestSequenceWithUntagged8& object)
        // Pass the specified 'object' to the specified 'hashAlg'.  This
        // function integrates with the 'bslh' modular hashing system and
        // effectively provides a 'bsl::hash' specialization for
        // 'EncoderTestSequenceWithUntagged8'.
    {
        object.hashAppendImpl(hashAlg);
    }
};

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

    // PRIVATE ACCESSORS
    template <typename t_HASH_ALGORITHM>
    void hashAppendImpl(t_HASH_ALGORITHM& hashAlgorithm) const;

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


    // MANIPULATORS
    void reset();
        // Reset this object to the default value (i.e., its value upon
        // default construction).

    template <typename t_MANIPULATOR>
    int manipulateAttributes(t_MANIPULATOR& manipulator);
        // Invoke the specified 'manipulator' sequentially on the address of
        // each (modifiable) attribute of this object, supplying 'manipulator'
        // with the corresponding attribute information structure until such
        // invocation returns a non-zero value.  Return the value from the
        // last invocation of 'manipulator' (i.e., the invocation that
        // terminated the sequence).

    template <typename t_MANIPULATOR>
    int manipulateAttribute(t_MANIPULATOR& manipulator, int id);
        // Invoke the specified 'manipulator' on the address of
        // the (modifiable) attribute indicated by the specified 'id',
        // supplying 'manipulator' with the corresponding attribute
        // information structure.  Return the value returned from the
        // invocation of 'manipulator' if 'id' identifies an attribute of this
        // class, and -1 otherwise.

    template <typename t_MANIPULATOR>
    int manipulateAttribute(t_MANIPULATOR&  manipulator,
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
                        int           level          = 0,
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

    template <typename t_ACCESSOR>
    int accessAttributes(t_ACCESSOR& accessor) const;
        // Invoke the specified 'accessor' sequentially on each
        // (non-modifiable) attribute of this object, supplying 'accessor'
        // with the corresponding attribute information structure until such
        // invocation returns a non-zero value.  Return the value from the
        // last invocation of 'accessor' (i.e., the invocation that terminated
        // the sequence).

    template <typename t_ACCESSOR>
    int accessAttribute(t_ACCESSOR& accessor, int id) const;
        // Invoke the specified 'accessor' on the (non-modifiable) attribute
        // of this object indicated by the specified 'id', supplying 'accessor'
        // with the corresponding attribute information structure.  Return the
        // value returned from the invocation of 'accessor' if 'id' identifies
        // an attribute of this class, and -1 otherwise.

    template <typename t_ACCESSOR>
    int accessAttribute(t_ACCESSOR&   accessor,
                        const char *name,
                        int         nameLength) const;
        // Invoke the specified 'accessor' on the (non-modifiable) attribute
        // of this object indicated by the specified 'name' of the specified
        // 'nameLength', supplying 'accessor' with the corresponding attribute
        // information structure.  Return the value returned from the
        // invocation of 'accessor' if 'name' identifies an attribute of this
        // class, and -1 otherwise.

    const EncoderTestSequenceWithUntagged9Sequence& sequence() const;
        // Return a reference offering non-modifiable access to the "Sequence"
        // attribute of this object.

    int attribute0() const;
        // Return the value of the "Attribute0" attribute of this object.

    const EncoderTestSequenceWithUntagged9Sequence1& sequence1() const;
        // Return a reference offering non-modifiable access to the "Sequence1"
        // attribute of this object.

    // HIDDEN FRIENDS
    friend bool operator==(const EncoderTestSequenceWithUntagged9& lhs,
                           const EncoderTestSequenceWithUntagged9& rhs)
        // Return 'true' if the specified 'lhs' and 'rhs' attribute objects
        // have the same value, and 'false' otherwise.  Two attribute objects
        // have the same value if each respective attribute has the same value.
    {
        return lhs.sequence() == rhs.sequence() &&
               lhs.attribute0() == rhs.attribute0() &&
               lhs.sequence1() == rhs.sequence1();
    }

    friend bool operator!=(const EncoderTestSequenceWithUntagged9& lhs,
                           const EncoderTestSequenceWithUntagged9& rhs)
        // Returns '!(lhs == rhs)'
    {
        return !(lhs == rhs);
    }

    friend bsl::ostream& operator<<(
                                bsl::ostream&                           stream,
                                const EncoderTestSequenceWithUntagged9& rhs)
        // Format the specified 'rhs' to the specified output 'stream' and
        // return a reference to the modifiable 'stream'.
    {
        return rhs.print(stream, 0, -1);
    }

    template <typename t_HASH_ALGORITHM>
    friend void hashAppend(t_HASH_ALGORITHM&                       hashAlg,
                           const EncoderTestSequenceWithUntagged9& object)
        // Pass the specified 'object' to the specified 'hashAlg'.  This
        // function integrates with the 'bslh' modular hashing system and
        // effectively provides a 'bsl::hash' specialization for
        // 'EncoderTestSequenceWithUntagged9'.
    {
        object.hashAppendImpl(hashAlg);
    }
};

}  // close package namespace

// TRAITS

BDLAT_DECL_SEQUENCE_WITH_BITWISEMOVEABLE_TRAITS(baljsn::EncoderTestSequenceWithUntagged9)

// ============================================================================
//                          INLINE DEFINITIONS
// ============================================================================

namespace baljsn {

                          // ------------------------
                          // class EncoderTestAddress
                          // ------------------------

// PRIVATE ACCESSORS
template <typename t_HASH_ALGORITHM>
void EncoderTestAddress::hashAppendImpl(t_HASH_ALGORITHM& hashAlgorithm) const
{
    using bslh::hashAppend;
    hashAppend(hashAlgorithm, this->street());
    hashAppend(hashAlgorithm, this->city());
    hashAppend(hashAlgorithm, this->state());
}

// CLASS METHODS
// MANIPULATORS
template <typename t_MANIPULATOR>
int EncoderTestAddress::manipulateAttributes(t_MANIPULATOR& manipulator)
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

    return 0;
}

template <typename t_MANIPULATOR>
int EncoderTestAddress::manipulateAttribute(t_MANIPULATOR& manipulator, int id)
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

template <typename t_MANIPULATOR>
int EncoderTestAddress::manipulateAttribute(
        t_MANIPULATOR& manipulator,
        const char    *name,
        int            nameLength)
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
template <typename t_ACCESSOR>
int EncoderTestAddress::accessAttributes(t_ACCESSOR& accessor) const
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

    return 0;
}

template <typename t_ACCESSOR>
int EncoderTestAddress::accessAttribute(t_ACCESSOR& accessor, int id) const
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

template <typename t_ACCESSOR>
int EncoderTestAddress::accessAttribute(
        t_ACCESSOR&  accessor,
        const char  *name,
        int          nameLength) const
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



               // ----------------------------------------------
               // class EncoderTestChoiceWithAllCategoriesChoice
               // ----------------------------------------------

// CLASS METHODS
// PRIVATE ACCESSORS
template <typename t_HASH_ALGORITHM>
void EncoderTestChoiceWithAllCategoriesChoice::hashAppendImpl(t_HASH_ALGORITHM& hashAlgorithm) const
{
    typedef EncoderTestChoiceWithAllCategoriesChoice Class;
    using bslh::hashAppend;
    hashAppend(hashAlgorithm, this->selectionId());
    switch (this->selectionId()) {
      case Class::SELECTION_ID_SELECTION0:
        hashAppend(hashAlgorithm, this->selection0());
        break;
      default:
        BSLS_ASSERT(this->selectionId() == Class::SELECTION_ID_UNDEFINED);
    }
}

inline
bool EncoderTestChoiceWithAllCategoriesChoice::isEqualTo(const EncoderTestChoiceWithAllCategoriesChoice& rhs) const
{
    typedef EncoderTestChoiceWithAllCategoriesChoice Class;
    if (this->selectionId() == rhs.selectionId()) {
        switch (rhs.selectionId()) {
          case Class::SELECTION_ID_SELECTION0:
            return this->selection0() == rhs.selection0();
          default:
            BSLS_ASSERT(Class::SELECTION_ID_UNDEFINED == rhs.selectionId());
            return true;
        }
    }
    else {
        return false;
    }
}

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
template <typename t_MANIPULATOR>
int EncoderTestChoiceWithAllCategoriesChoice::manipulateSelection(t_MANIPULATOR& manipulator)
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

template <typename t_ACCESSOR>
int EncoderTestChoiceWithAllCategoriesChoice::accessSelection(t_ACCESSOR& accessor) const
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
template <typename t_MANIPULATOR>
int EncoderTestChoiceWithAllCategoriesSequence::manipulateAttributes(t_MANIPULATOR& manipulator)
{
    int ret;

    ret = manipulator(&d_attribute, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE]);
    if (ret) {
        return ret;
    }

    return 0;
}

template <typename t_MANIPULATOR>
int EncoderTestChoiceWithAllCategoriesSequence::manipulateAttribute(t_MANIPULATOR& manipulator, int id)
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

template <typename t_MANIPULATOR>
int EncoderTestChoiceWithAllCategoriesSequence::manipulateAttribute(
        t_MANIPULATOR& manipulator,
        const char    *name,
        int            nameLength)
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
template <typename t_ACCESSOR>
int EncoderTestChoiceWithAllCategoriesSequence::accessAttributes(t_ACCESSOR& accessor) const
{
    int ret;

    ret = accessor(d_attribute, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE]);
    if (ret) {
        return ret;
    }

    return 0;
}

template <typename t_ACCESSOR>
int EncoderTestChoiceWithAllCategoriesSequence::accessAttribute(t_ACCESSOR& accessor, int id) const
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

template <typename t_ACCESSOR>
int EncoderTestChoiceWithAllCategoriesSequence::accessAttribute(
        t_ACCESSOR&  accessor,
        const char  *name,
        int          nameLength) const
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



                 // ------------------------------------------
                 // class EncoderTestDegenerateChoice1Sequence
                 // ------------------------------------------

// CLASS METHODS
// MANIPULATORS
template <typename t_MANIPULATOR>
int EncoderTestDegenerateChoice1Sequence::manipulateAttributes(t_MANIPULATOR& manipulator)
{
    (void)manipulator;
    return 0;
}

template <typename t_MANIPULATOR>
int EncoderTestDegenerateChoice1Sequence::manipulateAttribute(t_MANIPULATOR& manipulator, int id)
{
    (void)manipulator;
    enum { NOT_FOUND = -1 };

    switch (id) {
      default:
        return NOT_FOUND;
    }
}

template <typename t_MANIPULATOR>
int EncoderTestDegenerateChoice1Sequence::manipulateAttribute(
        t_MANIPULATOR& manipulator,
        const char    *name,
        int            nameLength)
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
template <typename t_ACCESSOR>
int EncoderTestDegenerateChoice1Sequence::accessAttributes(t_ACCESSOR& accessor) const
{
    (void)accessor;
    return 0;
}

template <typename t_ACCESSOR>
int EncoderTestDegenerateChoice1Sequence::accessAttribute(t_ACCESSOR& accessor, int id) const
{
    (void)accessor;
    enum { NOT_FOUND = -1 };

    switch (id) {
      default:
        return NOT_FOUND;
    }
}

template <typename t_ACCESSOR>
int EncoderTestDegenerateChoice1Sequence::accessAttribute(
        t_ACCESSOR&  accessor,
        const char  *name,
        int          nameLength) const
{
    enum { NOT_FOUND = -1 };

    const bdlat_AttributeInfo *attributeInfo =
          lookupAttributeInfo(name, nameLength);
    if (0 == attributeInfo) {
       return NOT_FOUND;
    }

    return accessAttribute(accessor, attributeInfo->d_id);
}



              // ------------------------------------------------
              // class EncoderTestSequenceWithAllCategoriesChoice
              // ------------------------------------------------

// CLASS METHODS
// PRIVATE ACCESSORS
template <typename t_HASH_ALGORITHM>
void EncoderTestSequenceWithAllCategoriesChoice::hashAppendImpl(t_HASH_ALGORITHM& hashAlgorithm) const
{
    typedef EncoderTestSequenceWithAllCategoriesChoice Class;
    using bslh::hashAppend;
    hashAppend(hashAlgorithm, this->selectionId());
    switch (this->selectionId()) {
      case Class::SELECTION_ID_SELECTION0:
        hashAppend(hashAlgorithm, this->selection0());
        break;
      default:
        BSLS_ASSERT(this->selectionId() == Class::SELECTION_ID_UNDEFINED);
    }
}

inline
bool EncoderTestSequenceWithAllCategoriesChoice::isEqualTo(const EncoderTestSequenceWithAllCategoriesChoice& rhs) const
{
    typedef EncoderTestSequenceWithAllCategoriesChoice Class;
    if (this->selectionId() == rhs.selectionId()) {
        switch (rhs.selectionId()) {
          case Class::SELECTION_ID_SELECTION0:
            return this->selection0() == rhs.selection0();
          default:
            BSLS_ASSERT(Class::SELECTION_ID_UNDEFINED == rhs.selectionId());
            return true;
        }
    }
    else {
        return false;
    }
}

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
template <typename t_MANIPULATOR>
int EncoderTestSequenceWithAllCategoriesChoice::manipulateSelection(t_MANIPULATOR& manipulator)
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

template <typename t_ACCESSOR>
int EncoderTestSequenceWithAllCategoriesChoice::accessSelection(t_ACCESSOR& accessor) const
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
template <typename t_MANIPULATOR>
int EncoderTestSequenceWithAllCategoriesSequence::manipulateAttributes(t_MANIPULATOR& manipulator)
{
    int ret;

    ret = manipulator(&d_attribute, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE]);
    if (ret) {
        return ret;
    }

    return 0;
}

template <typename t_MANIPULATOR>
int EncoderTestSequenceWithAllCategoriesSequence::manipulateAttribute(t_MANIPULATOR& manipulator, int id)
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

template <typename t_MANIPULATOR>
int EncoderTestSequenceWithAllCategoriesSequence::manipulateAttribute(
        t_MANIPULATOR& manipulator,
        const char    *name,
        int            nameLength)
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
template <typename t_ACCESSOR>
int EncoderTestSequenceWithAllCategoriesSequence::accessAttributes(t_ACCESSOR& accessor) const
{
    int ret;

    ret = accessor(d_attribute, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE]);
    if (ret) {
        return ret;
    }

    return 0;
}

template <typename t_ACCESSOR>
int EncoderTestSequenceWithAllCategoriesSequence::accessAttribute(t_ACCESSOR& accessor, int id) const
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

template <typename t_ACCESSOR>
int EncoderTestSequenceWithAllCategoriesSequence::accessAttribute(
        t_ACCESSOR&  accessor,
        const char  *name,
        int          nameLength) const
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



                   // --------------------------------------
                   // class EncoderTestSequenceWithUntagged0
                   // --------------------------------------

// CLASS METHODS
// MANIPULATORS
template <typename t_MANIPULATOR>
int EncoderTestSequenceWithUntagged0::manipulateAttributes(t_MANIPULATOR& manipulator)
{
    (void)manipulator;
    return 0;
}

template <typename t_MANIPULATOR>
int EncoderTestSequenceWithUntagged0::manipulateAttribute(t_MANIPULATOR& manipulator, int id)
{
    (void)manipulator;
    enum { NOT_FOUND = -1 };

    switch (id) {
      default:
        return NOT_FOUND;
    }
}

template <typename t_MANIPULATOR>
int EncoderTestSequenceWithUntagged0::manipulateAttribute(
        t_MANIPULATOR& manipulator,
        const char    *name,
        int            nameLength)
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
template <typename t_ACCESSOR>
int EncoderTestSequenceWithUntagged0::accessAttributes(t_ACCESSOR& accessor) const
{
    (void)accessor;
    return 0;
}

template <typename t_ACCESSOR>
int EncoderTestSequenceWithUntagged0::accessAttribute(t_ACCESSOR& accessor, int id) const
{
    (void)accessor;
    enum { NOT_FOUND = -1 };

    switch (id) {
      default:
        return NOT_FOUND;
    }
}

template <typename t_ACCESSOR>
int EncoderTestSequenceWithUntagged0::accessAttribute(
        t_ACCESSOR&  accessor,
        const char  *name,
        int          nameLength) const
{
    enum { NOT_FOUND = -1 };

    const bdlat_AttributeInfo *attributeInfo =
          lookupAttributeInfo(name, nameLength);
    if (0 == attributeInfo) {
       return NOT_FOUND;
    }

    return accessAttribute(accessor, attributeInfo->d_id);
}



              // -----------------------------------------------
              // class EncoderTestSequenceWithUntagged10Sequence
              // -----------------------------------------------

// CLASS METHODS
// MANIPULATORS
template <typename t_MANIPULATOR>
int EncoderTestSequenceWithUntagged10Sequence::manipulateAttributes(t_MANIPULATOR& manipulator)
{
    (void)manipulator;
    return 0;
}

template <typename t_MANIPULATOR>
int EncoderTestSequenceWithUntagged10Sequence::manipulateAttribute(t_MANIPULATOR& manipulator, int id)
{
    (void)manipulator;
    enum { NOT_FOUND = -1 };

    switch (id) {
      default:
        return NOT_FOUND;
    }
}

template <typename t_MANIPULATOR>
int EncoderTestSequenceWithUntagged10Sequence::manipulateAttribute(
        t_MANIPULATOR& manipulator,
        const char    *name,
        int            nameLength)
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
template <typename t_ACCESSOR>
int EncoderTestSequenceWithUntagged10Sequence::accessAttributes(t_ACCESSOR& accessor) const
{
    (void)accessor;
    return 0;
}

template <typename t_ACCESSOR>
int EncoderTestSequenceWithUntagged10Sequence::accessAttribute(t_ACCESSOR& accessor, int id) const
{
    (void)accessor;
    enum { NOT_FOUND = -1 };

    switch (id) {
      default:
        return NOT_FOUND;
    }
}

template <typename t_ACCESSOR>
int EncoderTestSequenceWithUntagged10Sequence::accessAttribute(
        t_ACCESSOR&  accessor,
        const char  *name,
        int          nameLength) const
{
    enum { NOT_FOUND = -1 };

    const bdlat_AttributeInfo *attributeInfo =
          lookupAttributeInfo(name, nameLength);
    if (0 == attributeInfo) {
       return NOT_FOUND;
    }

    return accessAttribute(accessor, attributeInfo->d_id);
}



              // -----------------------------------------------
              // class EncoderTestSequenceWithUntagged11Sequence
              // -----------------------------------------------

// CLASS METHODS
// MANIPULATORS
template <typename t_MANIPULATOR>
int EncoderTestSequenceWithUntagged11Sequence::manipulateAttributes(t_MANIPULATOR& manipulator)
{
    (void)manipulator;
    return 0;
}

template <typename t_MANIPULATOR>
int EncoderTestSequenceWithUntagged11Sequence::manipulateAttribute(t_MANIPULATOR& manipulator, int id)
{
    (void)manipulator;
    enum { NOT_FOUND = -1 };

    switch (id) {
      default:
        return NOT_FOUND;
    }
}

template <typename t_MANIPULATOR>
int EncoderTestSequenceWithUntagged11Sequence::manipulateAttribute(
        t_MANIPULATOR& manipulator,
        const char    *name,
        int            nameLength)
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
template <typename t_ACCESSOR>
int EncoderTestSequenceWithUntagged11Sequence::accessAttributes(t_ACCESSOR& accessor) const
{
    (void)accessor;
    return 0;
}

template <typename t_ACCESSOR>
int EncoderTestSequenceWithUntagged11Sequence::accessAttribute(t_ACCESSOR& accessor, int id) const
{
    (void)accessor;
    enum { NOT_FOUND = -1 };

    switch (id) {
      default:
        return NOT_FOUND;
    }
}

template <typename t_ACCESSOR>
int EncoderTestSequenceWithUntagged11Sequence::accessAttribute(
        t_ACCESSOR&  accessor,
        const char  *name,
        int          nameLength) const
{
    enum { NOT_FOUND = -1 };

    const bdlat_AttributeInfo *attributeInfo =
          lookupAttributeInfo(name, nameLength);
    if (0 == attributeInfo) {
       return NOT_FOUND;
    }

    return accessAttribute(accessor, attributeInfo->d_id);
}



              // ------------------------------------------------
              // class EncoderTestSequenceWithUntagged11Sequence1
              // ------------------------------------------------

// CLASS METHODS
// MANIPULATORS
template <typename t_MANIPULATOR>
int EncoderTestSequenceWithUntagged11Sequence1::manipulateAttributes(t_MANIPULATOR& manipulator)
{
    (void)manipulator;
    return 0;
}

template <typename t_MANIPULATOR>
int EncoderTestSequenceWithUntagged11Sequence1::manipulateAttribute(t_MANIPULATOR& manipulator, int id)
{
    (void)manipulator;
    enum { NOT_FOUND = -1 };

    switch (id) {
      default:
        return NOT_FOUND;
    }
}

template <typename t_MANIPULATOR>
int EncoderTestSequenceWithUntagged11Sequence1::manipulateAttribute(
        t_MANIPULATOR& manipulator,
        const char    *name,
        int            nameLength)
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
template <typename t_ACCESSOR>
int EncoderTestSequenceWithUntagged11Sequence1::accessAttributes(t_ACCESSOR& accessor) const
{
    (void)accessor;
    return 0;
}

template <typename t_ACCESSOR>
int EncoderTestSequenceWithUntagged11Sequence1::accessAttribute(t_ACCESSOR& accessor, int id) const
{
    (void)accessor;
    enum { NOT_FOUND = -1 };

    switch (id) {
      default:
        return NOT_FOUND;
    }
}

template <typename t_ACCESSOR>
int EncoderTestSequenceWithUntagged11Sequence1::accessAttribute(
        t_ACCESSOR&  accessor,
        const char  *name,
        int          nameLength) const
{
    enum { NOT_FOUND = -1 };

    const bdlat_AttributeInfo *attributeInfo =
          lookupAttributeInfo(name, nameLength);
    if (0 == attributeInfo) {
       return NOT_FOUND;
    }

    return accessAttribute(accessor, attributeInfo->d_id);
}



              // -----------------------------------------------
              // class EncoderTestSequenceWithUntagged12Sequence
              // -----------------------------------------------

// CLASS METHODS
// MANIPULATORS
template <typename t_MANIPULATOR>
int EncoderTestSequenceWithUntagged12Sequence::manipulateAttributes(t_MANIPULATOR& manipulator)
{
    (void)manipulator;
    return 0;
}

template <typename t_MANIPULATOR>
int EncoderTestSequenceWithUntagged12Sequence::manipulateAttribute(t_MANIPULATOR& manipulator, int id)
{
    (void)manipulator;
    enum { NOT_FOUND = -1 };

    switch (id) {
      default:
        return NOT_FOUND;
    }
}

template <typename t_MANIPULATOR>
int EncoderTestSequenceWithUntagged12Sequence::manipulateAttribute(
        t_MANIPULATOR& manipulator,
        const char    *name,
        int            nameLength)
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
template <typename t_ACCESSOR>
int EncoderTestSequenceWithUntagged12Sequence::accessAttributes(t_ACCESSOR& accessor) const
{
    (void)accessor;
    return 0;
}

template <typename t_ACCESSOR>
int EncoderTestSequenceWithUntagged12Sequence::accessAttribute(t_ACCESSOR& accessor, int id) const
{
    (void)accessor;
    enum { NOT_FOUND = -1 };

    switch (id) {
      default:
        return NOT_FOUND;
    }
}

template <typename t_ACCESSOR>
int EncoderTestSequenceWithUntagged12Sequence::accessAttribute(
        t_ACCESSOR&  accessor,
        const char  *name,
        int          nameLength) const
{
    enum { NOT_FOUND = -1 };

    const bdlat_AttributeInfo *attributeInfo =
          lookupAttributeInfo(name, nameLength);
    if (0 == attributeInfo) {
       return NOT_FOUND;
    }

    return accessAttribute(accessor, attributeInfo->d_id);
}



              // -----------------------------------------------
              // class EncoderTestSequenceWithUntagged13Sequence
              // -----------------------------------------------

// CLASS METHODS
// MANIPULATORS
template <typename t_MANIPULATOR>
int EncoderTestSequenceWithUntagged13Sequence::manipulateAttributes(t_MANIPULATOR& manipulator)
{
    (void)manipulator;
    return 0;
}

template <typename t_MANIPULATOR>
int EncoderTestSequenceWithUntagged13Sequence::manipulateAttribute(t_MANIPULATOR& manipulator, int id)
{
    (void)manipulator;
    enum { NOT_FOUND = -1 };

    switch (id) {
      default:
        return NOT_FOUND;
    }
}

template <typename t_MANIPULATOR>
int EncoderTestSequenceWithUntagged13Sequence::manipulateAttribute(
        t_MANIPULATOR& manipulator,
        const char    *name,
        int            nameLength)
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
template <typename t_ACCESSOR>
int EncoderTestSequenceWithUntagged13Sequence::accessAttributes(t_ACCESSOR& accessor) const
{
    (void)accessor;
    return 0;
}

template <typename t_ACCESSOR>
int EncoderTestSequenceWithUntagged13Sequence::accessAttribute(t_ACCESSOR& accessor, int id) const
{
    (void)accessor;
    enum { NOT_FOUND = -1 };

    switch (id) {
      default:
        return NOT_FOUND;
    }
}

template <typename t_ACCESSOR>
int EncoderTestSequenceWithUntagged13Sequence::accessAttribute(
        t_ACCESSOR&  accessor,
        const char  *name,
        int          nameLength) const
{
    enum { NOT_FOUND = -1 };

    const bdlat_AttributeInfo *attributeInfo =
          lookupAttributeInfo(name, nameLength);
    if (0 == attributeInfo) {
       return NOT_FOUND;
    }

    return accessAttribute(accessor, attributeInfo->d_id);
}



                  // ---------------------------------------
                  // class EncoderTestSequenceWithUntagged14
                  // ---------------------------------------

// PRIVATE ACCESSORS
template <typename t_HASH_ALGORITHM>
void EncoderTestSequenceWithUntagged14::hashAppendImpl(t_HASH_ALGORITHM& hashAlgorithm) const
{
    using bslh::hashAppend;
    hashAppend(hashAlgorithm, this->attribute0());
    hashAppend(hashAlgorithm, this->attribute1());
    hashAppend(hashAlgorithm, this->attribute2());
}

// CLASS METHODS
// MANIPULATORS
template <typename t_MANIPULATOR>
int EncoderTestSequenceWithUntagged14::manipulateAttributes(t_MANIPULATOR& manipulator)
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

    return 0;
}

template <typename t_MANIPULATOR>
int EncoderTestSequenceWithUntagged14::manipulateAttribute(t_MANIPULATOR& manipulator, int id)
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

template <typename t_MANIPULATOR>
int EncoderTestSequenceWithUntagged14::manipulateAttribute(
        t_MANIPULATOR& manipulator,
        const char    *name,
        int            nameLength)
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
template <typename t_ACCESSOR>
int EncoderTestSequenceWithUntagged14::accessAttributes(t_ACCESSOR& accessor) const
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

    return 0;
}

template <typename t_ACCESSOR>
int EncoderTestSequenceWithUntagged14::accessAttribute(t_ACCESSOR& accessor, int id) const
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

template <typename t_ACCESSOR>
int EncoderTestSequenceWithUntagged14::accessAttribute(
        t_ACCESSOR&  accessor,
        const char  *name,
        int          nameLength) const
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



               // ----------------------------------------------
               // class EncoderTestSequenceWithUntagged1Sequence
               // ----------------------------------------------

// CLASS METHODS
// MANIPULATORS
template <typename t_MANIPULATOR>
int EncoderTestSequenceWithUntagged1Sequence::manipulateAttributes(t_MANIPULATOR& manipulator)
{
    (void)manipulator;
    return 0;
}

template <typename t_MANIPULATOR>
int EncoderTestSequenceWithUntagged1Sequence::manipulateAttribute(t_MANIPULATOR& manipulator, int id)
{
    (void)manipulator;
    enum { NOT_FOUND = -1 };

    switch (id) {
      default:
        return NOT_FOUND;
    }
}

template <typename t_MANIPULATOR>
int EncoderTestSequenceWithUntagged1Sequence::manipulateAttribute(
        t_MANIPULATOR& manipulator,
        const char    *name,
        int            nameLength)
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
template <typename t_ACCESSOR>
int EncoderTestSequenceWithUntagged1Sequence::accessAttributes(t_ACCESSOR& accessor) const
{
    (void)accessor;
    return 0;
}

template <typename t_ACCESSOR>
int EncoderTestSequenceWithUntagged1Sequence::accessAttribute(t_ACCESSOR& accessor, int id) const
{
    (void)accessor;
    enum { NOT_FOUND = -1 };

    switch (id) {
      default:
        return NOT_FOUND;
    }
}

template <typename t_ACCESSOR>
int EncoderTestSequenceWithUntagged1Sequence::accessAttribute(
        t_ACCESSOR&  accessor,
        const char  *name,
        int          nameLength) const
{
    enum { NOT_FOUND = -1 };

    const bdlat_AttributeInfo *attributeInfo =
          lookupAttributeInfo(name, nameLength);
    if (0 == attributeInfo) {
       return NOT_FOUND;
    }

    return accessAttribute(accessor, attributeInfo->d_id);
}



                   // --------------------------------------
                   // class EncoderTestSequenceWithUntagged2
                   // --------------------------------------

// CLASS METHODS
// MANIPULATORS
template <typename t_MANIPULATOR>
int EncoderTestSequenceWithUntagged2::manipulateAttributes(t_MANIPULATOR& manipulator)
{
    int ret;

    ret = manipulator(&d_attribute0, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE0]);
    if (ret) {
        return ret;
    }

    return 0;
}

template <typename t_MANIPULATOR>
int EncoderTestSequenceWithUntagged2::manipulateAttribute(t_MANIPULATOR& manipulator, int id)
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

template <typename t_MANIPULATOR>
int EncoderTestSequenceWithUntagged2::manipulateAttribute(
        t_MANIPULATOR& manipulator,
        const char    *name,
        int            nameLength)
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
template <typename t_ACCESSOR>
int EncoderTestSequenceWithUntagged2::accessAttributes(t_ACCESSOR& accessor) const
{
    int ret;

    ret = accessor(d_attribute0, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE0]);
    if (ret) {
        return ret;
    }

    return 0;
}

template <typename t_ACCESSOR>
int EncoderTestSequenceWithUntagged2::accessAttribute(t_ACCESSOR& accessor, int id) const
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

template <typename t_ACCESSOR>
int EncoderTestSequenceWithUntagged2::accessAttribute(
        t_ACCESSOR&  accessor,
        const char  *name,
        int          nameLength) const
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



               // ----------------------------------------------
               // class EncoderTestSequenceWithUntagged3Sequence
               // ----------------------------------------------

// CLASS METHODS
// MANIPULATORS
template <typename t_MANIPULATOR>
int EncoderTestSequenceWithUntagged3Sequence::manipulateAttributes(t_MANIPULATOR& manipulator)
{
    (void)manipulator;
    return 0;
}

template <typename t_MANIPULATOR>
int EncoderTestSequenceWithUntagged3Sequence::manipulateAttribute(t_MANIPULATOR& manipulator, int id)
{
    (void)manipulator;
    enum { NOT_FOUND = -1 };

    switch (id) {
      default:
        return NOT_FOUND;
    }
}

template <typename t_MANIPULATOR>
int EncoderTestSequenceWithUntagged3Sequence::manipulateAttribute(
        t_MANIPULATOR& manipulator,
        const char    *name,
        int            nameLength)
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
template <typename t_ACCESSOR>
int EncoderTestSequenceWithUntagged3Sequence::accessAttributes(t_ACCESSOR& accessor) const
{
    (void)accessor;
    return 0;
}

template <typename t_ACCESSOR>
int EncoderTestSequenceWithUntagged3Sequence::accessAttribute(t_ACCESSOR& accessor, int id) const
{
    (void)accessor;
    enum { NOT_FOUND = -1 };

    switch (id) {
      default:
        return NOT_FOUND;
    }
}

template <typename t_ACCESSOR>
int EncoderTestSequenceWithUntagged3Sequence::accessAttribute(
        t_ACCESSOR&  accessor,
        const char  *name,
        int          nameLength) const
{
    enum { NOT_FOUND = -1 };

    const bdlat_AttributeInfo *attributeInfo =
          lookupAttributeInfo(name, nameLength);
    if (0 == attributeInfo) {
       return NOT_FOUND;
    }

    return accessAttribute(accessor, attributeInfo->d_id);
}



              // -----------------------------------------------
              // class EncoderTestSequenceWithUntagged3Sequence1
              // -----------------------------------------------

// CLASS METHODS
// MANIPULATORS
template <typename t_MANIPULATOR>
int EncoderTestSequenceWithUntagged3Sequence1::manipulateAttributes(t_MANIPULATOR& manipulator)
{
    (void)manipulator;
    return 0;
}

template <typename t_MANIPULATOR>
int EncoderTestSequenceWithUntagged3Sequence1::manipulateAttribute(t_MANIPULATOR& manipulator, int id)
{
    (void)manipulator;
    enum { NOT_FOUND = -1 };

    switch (id) {
      default:
        return NOT_FOUND;
    }
}

template <typename t_MANIPULATOR>
int EncoderTestSequenceWithUntagged3Sequence1::manipulateAttribute(
        t_MANIPULATOR& manipulator,
        const char    *name,
        int            nameLength)
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
template <typename t_ACCESSOR>
int EncoderTestSequenceWithUntagged3Sequence1::accessAttributes(t_ACCESSOR& accessor) const
{
    (void)accessor;
    return 0;
}

template <typename t_ACCESSOR>
int EncoderTestSequenceWithUntagged3Sequence1::accessAttribute(t_ACCESSOR& accessor, int id) const
{
    (void)accessor;
    enum { NOT_FOUND = -1 };

    switch (id) {
      default:
        return NOT_FOUND;
    }
}

template <typename t_ACCESSOR>
int EncoderTestSequenceWithUntagged3Sequence1::accessAttribute(
        t_ACCESSOR&  accessor,
        const char  *name,
        int          nameLength) const
{
    enum { NOT_FOUND = -1 };

    const bdlat_AttributeInfo *attributeInfo =
          lookupAttributeInfo(name, nameLength);
    if (0 == attributeInfo) {
       return NOT_FOUND;
    }

    return accessAttribute(accessor, attributeInfo->d_id);
}



               // ----------------------------------------------
               // class EncoderTestSequenceWithUntagged4Sequence
               // ----------------------------------------------

// CLASS METHODS
// MANIPULATORS
template <typename t_MANIPULATOR>
int EncoderTestSequenceWithUntagged4Sequence::manipulateAttributes(t_MANIPULATOR& manipulator)
{
    (void)manipulator;
    return 0;
}

template <typename t_MANIPULATOR>
int EncoderTestSequenceWithUntagged4Sequence::manipulateAttribute(t_MANIPULATOR& manipulator, int id)
{
    (void)manipulator;
    enum { NOT_FOUND = -1 };

    switch (id) {
      default:
        return NOT_FOUND;
    }
}

template <typename t_MANIPULATOR>
int EncoderTestSequenceWithUntagged4Sequence::manipulateAttribute(
        t_MANIPULATOR& manipulator,
        const char    *name,
        int            nameLength)
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
template <typename t_ACCESSOR>
int EncoderTestSequenceWithUntagged4Sequence::accessAttributes(t_ACCESSOR& accessor) const
{
    (void)accessor;
    return 0;
}

template <typename t_ACCESSOR>
int EncoderTestSequenceWithUntagged4Sequence::accessAttribute(t_ACCESSOR& accessor, int id) const
{
    (void)accessor;
    enum { NOT_FOUND = -1 };

    switch (id) {
      default:
        return NOT_FOUND;
    }
}

template <typename t_ACCESSOR>
int EncoderTestSequenceWithUntagged4Sequence::accessAttribute(
        t_ACCESSOR&  accessor,
        const char  *name,
        int          nameLength) const
{
    enum { NOT_FOUND = -1 };

    const bdlat_AttributeInfo *attributeInfo =
          lookupAttributeInfo(name, nameLength);
    if (0 == attributeInfo) {
       return NOT_FOUND;
    }

    return accessAttribute(accessor, attributeInfo->d_id);
}



               // ----------------------------------------------
               // class EncoderTestSequenceWithUntagged5Sequence
               // ----------------------------------------------

// CLASS METHODS
// MANIPULATORS
template <typename t_MANIPULATOR>
int EncoderTestSequenceWithUntagged5Sequence::manipulateAttributes(t_MANIPULATOR& manipulator)
{
    (void)manipulator;
    return 0;
}

template <typename t_MANIPULATOR>
int EncoderTestSequenceWithUntagged5Sequence::manipulateAttribute(t_MANIPULATOR& manipulator, int id)
{
    (void)manipulator;
    enum { NOT_FOUND = -1 };

    switch (id) {
      default:
        return NOT_FOUND;
    }
}

template <typename t_MANIPULATOR>
int EncoderTestSequenceWithUntagged5Sequence::manipulateAttribute(
        t_MANIPULATOR& manipulator,
        const char    *name,
        int            nameLength)
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
template <typename t_ACCESSOR>
int EncoderTestSequenceWithUntagged5Sequence::accessAttributes(t_ACCESSOR& accessor) const
{
    (void)accessor;
    return 0;
}

template <typename t_ACCESSOR>
int EncoderTestSequenceWithUntagged5Sequence::accessAttribute(t_ACCESSOR& accessor, int id) const
{
    (void)accessor;
    enum { NOT_FOUND = -1 };

    switch (id) {
      default:
        return NOT_FOUND;
    }
}

template <typename t_ACCESSOR>
int EncoderTestSequenceWithUntagged5Sequence::accessAttribute(
        t_ACCESSOR&  accessor,
        const char  *name,
        int          nameLength) const
{
    enum { NOT_FOUND = -1 };

    const bdlat_AttributeInfo *attributeInfo =
          lookupAttributeInfo(name, nameLength);
    if (0 == attributeInfo) {
       return NOT_FOUND;
    }

    return accessAttribute(accessor, attributeInfo->d_id);
}



                   // --------------------------------------
                   // class EncoderTestSequenceWithUntagged6
                   // --------------------------------------

// CLASS METHODS
// MANIPULATORS
template <typename t_MANIPULATOR>
int EncoderTestSequenceWithUntagged6::manipulateAttributes(t_MANIPULATOR& manipulator)
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

    return 0;
}

template <typename t_MANIPULATOR>
int EncoderTestSequenceWithUntagged6::manipulateAttribute(t_MANIPULATOR& manipulator, int id)
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

template <typename t_MANIPULATOR>
int EncoderTestSequenceWithUntagged6::manipulateAttribute(
        t_MANIPULATOR& manipulator,
        const char    *name,
        int            nameLength)
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
template <typename t_ACCESSOR>
int EncoderTestSequenceWithUntagged6::accessAttributes(t_ACCESSOR& accessor) const
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

    return 0;
}

template <typename t_ACCESSOR>
int EncoderTestSequenceWithUntagged6::accessAttribute(t_ACCESSOR& accessor, int id) const
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

template <typename t_ACCESSOR>
int EncoderTestSequenceWithUntagged6::accessAttribute(
        t_ACCESSOR&  accessor,
        const char  *name,
        int          nameLength) const
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



               // ----------------------------------------------
               // class EncoderTestSequenceWithUntagged7Sequence
               // ----------------------------------------------

// CLASS METHODS
// MANIPULATORS
template <typename t_MANIPULATOR>
int EncoderTestSequenceWithUntagged7Sequence::manipulateAttributes(t_MANIPULATOR& manipulator)
{
    (void)manipulator;
    return 0;
}

template <typename t_MANIPULATOR>
int EncoderTestSequenceWithUntagged7Sequence::manipulateAttribute(t_MANIPULATOR& manipulator, int id)
{
    (void)manipulator;
    enum { NOT_FOUND = -1 };

    switch (id) {
      default:
        return NOT_FOUND;
    }
}

template <typename t_MANIPULATOR>
int EncoderTestSequenceWithUntagged7Sequence::manipulateAttribute(
        t_MANIPULATOR& manipulator,
        const char    *name,
        int            nameLength)
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
template <typename t_ACCESSOR>
int EncoderTestSequenceWithUntagged7Sequence::accessAttributes(t_ACCESSOR& accessor) const
{
    (void)accessor;
    return 0;
}

template <typename t_ACCESSOR>
int EncoderTestSequenceWithUntagged7Sequence::accessAttribute(t_ACCESSOR& accessor, int id) const
{
    (void)accessor;
    enum { NOT_FOUND = -1 };

    switch (id) {
      default:
        return NOT_FOUND;
    }
}

template <typename t_ACCESSOR>
int EncoderTestSequenceWithUntagged7Sequence::accessAttribute(
        t_ACCESSOR&  accessor,
        const char  *name,
        int          nameLength) const
{
    enum { NOT_FOUND = -1 };

    const bdlat_AttributeInfo *attributeInfo =
          lookupAttributeInfo(name, nameLength);
    if (0 == attributeInfo) {
       return NOT_FOUND;
    }

    return accessAttribute(accessor, attributeInfo->d_id);
}



              // -----------------------------------------------
              // class EncoderTestSequenceWithUntagged7Sequence1
              // -----------------------------------------------

// CLASS METHODS
// MANIPULATORS
template <typename t_MANIPULATOR>
int EncoderTestSequenceWithUntagged7Sequence1::manipulateAttributes(t_MANIPULATOR& manipulator)
{
    (void)manipulator;
    return 0;
}

template <typename t_MANIPULATOR>
int EncoderTestSequenceWithUntagged7Sequence1::manipulateAttribute(t_MANIPULATOR& manipulator, int id)
{
    (void)manipulator;
    enum { NOT_FOUND = -1 };

    switch (id) {
      default:
        return NOT_FOUND;
    }
}

template <typename t_MANIPULATOR>
int EncoderTestSequenceWithUntagged7Sequence1::manipulateAttribute(
        t_MANIPULATOR& manipulator,
        const char    *name,
        int            nameLength)
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
template <typename t_ACCESSOR>
int EncoderTestSequenceWithUntagged7Sequence1::accessAttributes(t_ACCESSOR& accessor) const
{
    (void)accessor;
    return 0;
}

template <typename t_ACCESSOR>
int EncoderTestSequenceWithUntagged7Sequence1::accessAttribute(t_ACCESSOR& accessor, int id) const
{
    (void)accessor;
    enum { NOT_FOUND = -1 };

    switch (id) {
      default:
        return NOT_FOUND;
    }
}

template <typename t_ACCESSOR>
int EncoderTestSequenceWithUntagged7Sequence1::accessAttribute(
        t_ACCESSOR&  accessor,
        const char  *name,
        int          nameLength) const
{
    enum { NOT_FOUND = -1 };

    const bdlat_AttributeInfo *attributeInfo =
          lookupAttributeInfo(name, nameLength);
    if (0 == attributeInfo) {
       return NOT_FOUND;
    }

    return accessAttribute(accessor, attributeInfo->d_id);
}



              // -----------------------------------------------
              // class EncoderTestSequenceWithUntagged7Sequence2
              // -----------------------------------------------

// CLASS METHODS
// MANIPULATORS
template <typename t_MANIPULATOR>
int EncoderTestSequenceWithUntagged7Sequence2::manipulateAttributes(t_MANIPULATOR& manipulator)
{
    (void)manipulator;
    return 0;
}

template <typename t_MANIPULATOR>
int EncoderTestSequenceWithUntagged7Sequence2::manipulateAttribute(t_MANIPULATOR& manipulator, int id)
{
    (void)manipulator;
    enum { NOT_FOUND = -1 };

    switch (id) {
      default:
        return NOT_FOUND;
    }
}

template <typename t_MANIPULATOR>
int EncoderTestSequenceWithUntagged7Sequence2::manipulateAttribute(
        t_MANIPULATOR& manipulator,
        const char    *name,
        int            nameLength)
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
template <typename t_ACCESSOR>
int EncoderTestSequenceWithUntagged7Sequence2::accessAttributes(t_ACCESSOR& accessor) const
{
    (void)accessor;
    return 0;
}

template <typename t_ACCESSOR>
int EncoderTestSequenceWithUntagged7Sequence2::accessAttribute(t_ACCESSOR& accessor, int id) const
{
    (void)accessor;
    enum { NOT_FOUND = -1 };

    switch (id) {
      default:
        return NOT_FOUND;
    }
}

template <typename t_ACCESSOR>
int EncoderTestSequenceWithUntagged7Sequence2::accessAttribute(
        t_ACCESSOR&  accessor,
        const char  *name,
        int          nameLength) const
{
    enum { NOT_FOUND = -1 };

    const bdlat_AttributeInfo *attributeInfo =
          lookupAttributeInfo(name, nameLength);
    if (0 == attributeInfo) {
       return NOT_FOUND;
    }

    return accessAttribute(accessor, attributeInfo->d_id);
}



               // ----------------------------------------------
               // class EncoderTestSequenceWithUntagged8Sequence
               // ----------------------------------------------

// CLASS METHODS
// MANIPULATORS
template <typename t_MANIPULATOR>
int EncoderTestSequenceWithUntagged8Sequence::manipulateAttributes(t_MANIPULATOR& manipulator)
{
    (void)manipulator;
    return 0;
}

template <typename t_MANIPULATOR>
int EncoderTestSequenceWithUntagged8Sequence::manipulateAttribute(t_MANIPULATOR& manipulator, int id)
{
    (void)manipulator;
    enum { NOT_FOUND = -1 };

    switch (id) {
      default:
        return NOT_FOUND;
    }
}

template <typename t_MANIPULATOR>
int EncoderTestSequenceWithUntagged8Sequence::manipulateAttribute(
        t_MANIPULATOR& manipulator,
        const char    *name,
        int            nameLength)
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
template <typename t_ACCESSOR>
int EncoderTestSequenceWithUntagged8Sequence::accessAttributes(t_ACCESSOR& accessor) const
{
    (void)accessor;
    return 0;
}

template <typename t_ACCESSOR>
int EncoderTestSequenceWithUntagged8Sequence::accessAttribute(t_ACCESSOR& accessor, int id) const
{
    (void)accessor;
    enum { NOT_FOUND = -1 };

    switch (id) {
      default:
        return NOT_FOUND;
    }
}

template <typename t_ACCESSOR>
int EncoderTestSequenceWithUntagged8Sequence::accessAttribute(
        t_ACCESSOR&  accessor,
        const char  *name,
        int          nameLength) const
{
    enum { NOT_FOUND = -1 };

    const bdlat_AttributeInfo *attributeInfo =
          lookupAttributeInfo(name, nameLength);
    if (0 == attributeInfo) {
       return NOT_FOUND;
    }

    return accessAttribute(accessor, attributeInfo->d_id);
}



              // -----------------------------------------------
              // class EncoderTestSequenceWithUntagged8Sequence1
              // -----------------------------------------------

// CLASS METHODS
// MANIPULATORS
template <typename t_MANIPULATOR>
int EncoderTestSequenceWithUntagged8Sequence1::manipulateAttributes(t_MANIPULATOR& manipulator)
{
    (void)manipulator;
    return 0;
}

template <typename t_MANIPULATOR>
int EncoderTestSequenceWithUntagged8Sequence1::manipulateAttribute(t_MANIPULATOR& manipulator, int id)
{
    (void)manipulator;
    enum { NOT_FOUND = -1 };

    switch (id) {
      default:
        return NOT_FOUND;
    }
}

template <typename t_MANIPULATOR>
int EncoderTestSequenceWithUntagged8Sequence1::manipulateAttribute(
        t_MANIPULATOR& manipulator,
        const char    *name,
        int            nameLength)
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
template <typename t_ACCESSOR>
int EncoderTestSequenceWithUntagged8Sequence1::accessAttributes(t_ACCESSOR& accessor) const
{
    (void)accessor;
    return 0;
}

template <typename t_ACCESSOR>
int EncoderTestSequenceWithUntagged8Sequence1::accessAttribute(t_ACCESSOR& accessor, int id) const
{
    (void)accessor;
    enum { NOT_FOUND = -1 };

    switch (id) {
      default:
        return NOT_FOUND;
    }
}

template <typename t_ACCESSOR>
int EncoderTestSequenceWithUntagged8Sequence1::accessAttribute(
        t_ACCESSOR&  accessor,
        const char  *name,
        int          nameLength) const
{
    enum { NOT_FOUND = -1 };

    const bdlat_AttributeInfo *attributeInfo =
          lookupAttributeInfo(name, nameLength);
    if (0 == attributeInfo) {
       return NOT_FOUND;
    }

    return accessAttribute(accessor, attributeInfo->d_id);
}



               // ----------------------------------------------
               // class EncoderTestSequenceWithUntagged9Sequence
               // ----------------------------------------------

// CLASS METHODS
// MANIPULATORS
template <typename t_MANIPULATOR>
int EncoderTestSequenceWithUntagged9Sequence::manipulateAttributes(t_MANIPULATOR& manipulator)
{
    (void)manipulator;
    return 0;
}

template <typename t_MANIPULATOR>
int EncoderTestSequenceWithUntagged9Sequence::manipulateAttribute(t_MANIPULATOR& manipulator, int id)
{
    (void)manipulator;
    enum { NOT_FOUND = -1 };

    switch (id) {
      default:
        return NOT_FOUND;
    }
}

template <typename t_MANIPULATOR>
int EncoderTestSequenceWithUntagged9Sequence::manipulateAttribute(
        t_MANIPULATOR& manipulator,
        const char    *name,
        int            nameLength)
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
template <typename t_ACCESSOR>
int EncoderTestSequenceWithUntagged9Sequence::accessAttributes(t_ACCESSOR& accessor) const
{
    (void)accessor;
    return 0;
}

template <typename t_ACCESSOR>
int EncoderTestSequenceWithUntagged9Sequence::accessAttribute(t_ACCESSOR& accessor, int id) const
{
    (void)accessor;
    enum { NOT_FOUND = -1 };

    switch (id) {
      default:
        return NOT_FOUND;
    }
}

template <typename t_ACCESSOR>
int EncoderTestSequenceWithUntagged9Sequence::accessAttribute(
        t_ACCESSOR&  accessor,
        const char  *name,
        int          nameLength) const
{
    enum { NOT_FOUND = -1 };

    const bdlat_AttributeInfo *attributeInfo =
          lookupAttributeInfo(name, nameLength);
    if (0 == attributeInfo) {
       return NOT_FOUND;
    }

    return accessAttribute(accessor, attributeInfo->d_id);
}



              // -----------------------------------------------
              // class EncoderTestSequenceWithUntagged9Sequence1
              // -----------------------------------------------

// CLASS METHODS
// MANIPULATORS
template <typename t_MANIPULATOR>
int EncoderTestSequenceWithUntagged9Sequence1::manipulateAttributes(t_MANIPULATOR& manipulator)
{
    (void)manipulator;
    return 0;
}

template <typename t_MANIPULATOR>
int EncoderTestSequenceWithUntagged9Sequence1::manipulateAttribute(t_MANIPULATOR& manipulator, int id)
{
    (void)manipulator;
    enum { NOT_FOUND = -1 };

    switch (id) {
      default:
        return NOT_FOUND;
    }
}

template <typename t_MANIPULATOR>
int EncoderTestSequenceWithUntagged9Sequence1::manipulateAttribute(
        t_MANIPULATOR& manipulator,
        const char    *name,
        int            nameLength)
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
template <typename t_ACCESSOR>
int EncoderTestSequenceWithUntagged9Sequence1::accessAttributes(t_ACCESSOR& accessor) const
{
    (void)accessor;
    return 0;
}

template <typename t_ACCESSOR>
int EncoderTestSequenceWithUntagged9Sequence1::accessAttribute(t_ACCESSOR& accessor, int id) const
{
    (void)accessor;
    enum { NOT_FOUND = -1 };

    switch (id) {
      default:
        return NOT_FOUND;
    }
}

template <typename t_ACCESSOR>
int EncoderTestSequenceWithUntagged9Sequence1::accessAttribute(
        t_ACCESSOR&  accessor,
        const char  *name,
        int          nameLength) const
{
    enum { NOT_FOUND = -1 };

    const bdlat_AttributeInfo *attributeInfo =
          lookupAttributeInfo(name, nameLength);
    if (0 == attributeInfo) {
       return NOT_FOUND;
    }

    return accessAttribute(accessor, attributeInfo->d_id);
}



                  // ----------------------------------------
                  // class EncoderTestChoiceWithAllCategories
                  // ----------------------------------------

// CLASS METHODS
// PRIVATE ACCESSORS
template <typename t_HASH_ALGORITHM>
void EncoderTestChoiceWithAllCategories::hashAppendImpl(t_HASH_ALGORITHM& hashAlgorithm) const
{
    typedef EncoderTestChoiceWithAllCategories Class;
    using bslh::hashAppend;
    hashAppend(hashAlgorithm, this->selectionId());
    switch (this->selectionId()) {
      case Class::SELECTION_ID_CHAR_ARRAY:
        hashAppend(hashAlgorithm, this->charArray());
        break;
      case Class::SELECTION_ID_CHOICE:
        hashAppend(hashAlgorithm, this->choice());
        break;
      case Class::SELECTION_ID_CUSTOMIZED_TYPE:
        hashAppend(hashAlgorithm, this->customizedType());
        break;
      case Class::SELECTION_ID_ENUMERATION:
        hashAppend(hashAlgorithm, this->enumeration());
        break;
      case Class::SELECTION_ID_SEQUENCE:
        hashAppend(hashAlgorithm, this->sequence());
        break;
      case Class::SELECTION_ID_SIMPLE:
        hashAppend(hashAlgorithm, this->simple());
        break;
      default:
        BSLS_ASSERT(this->selectionId() == Class::SELECTION_ID_UNDEFINED);
    }
}

inline
bool EncoderTestChoiceWithAllCategories::isEqualTo(const EncoderTestChoiceWithAllCategories& rhs) const
{
    typedef EncoderTestChoiceWithAllCategories Class;
    if (this->selectionId() == rhs.selectionId()) {
        switch (rhs.selectionId()) {
          case Class::SELECTION_ID_CHAR_ARRAY:
            return this->charArray() == rhs.charArray();
          case Class::SELECTION_ID_CHOICE:
            return this->choice() == rhs.choice();
          case Class::SELECTION_ID_CUSTOMIZED_TYPE:
            return this->customizedType() == rhs.customizedType();
          case Class::SELECTION_ID_ENUMERATION:
            return this->enumeration() == rhs.enumeration();
          case Class::SELECTION_ID_SEQUENCE:
            return this->sequence() == rhs.sequence();
          case Class::SELECTION_ID_SIMPLE:
            return this->simple() == rhs.simple();
          default:
            BSLS_ASSERT(Class::SELECTION_ID_UNDEFINED == rhs.selectionId());
            return true;
        }
    }
    else {
        return false;
    }
}

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
template <typename t_MANIPULATOR>
int EncoderTestChoiceWithAllCategories::manipulateSelection(t_MANIPULATOR& manipulator)
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

template <typename t_ACCESSOR>
int EncoderTestChoiceWithAllCategories::accessSelection(t_ACCESSOR& accessor) const
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


                     // ----------------------------------
                     // class EncoderTestDegenerateChoice1
                     // ----------------------------------

// CLASS METHODS
// PRIVATE ACCESSORS
template <typename t_HASH_ALGORITHM>
void EncoderTestDegenerateChoice1::hashAppendImpl(t_HASH_ALGORITHM& hashAlgorithm) const
{
    typedef EncoderTestDegenerateChoice1 Class;
    using bslh::hashAppend;
    hashAppend(hashAlgorithm, this->selectionId());
    switch (this->selectionId()) {
      case Class::SELECTION_ID_SEQUENCE:
        hashAppend(hashAlgorithm, this->sequence());
        break;
      default:
        BSLS_ASSERT(this->selectionId() == Class::SELECTION_ID_UNDEFINED);
    }
}

inline
bool EncoderTestDegenerateChoice1::isEqualTo(const EncoderTestDegenerateChoice1& rhs) const
{
    typedef EncoderTestDegenerateChoice1 Class;
    if (this->selectionId() == rhs.selectionId()) {
        switch (rhs.selectionId()) {
          case Class::SELECTION_ID_SEQUENCE:
            return this->sequence() == rhs.sequence();
          default:
            BSLS_ASSERT(Class::SELECTION_ID_UNDEFINED == rhs.selectionId());
            return true;
        }
    }
    else {
        return false;
    }
}

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
template <typename t_MANIPULATOR>
int EncoderTestDegenerateChoice1::manipulateSelection(t_MANIPULATOR& manipulator)
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

template <typename t_ACCESSOR>
int EncoderTestDegenerateChoice1::accessSelection(t_ACCESSOR& accessor) const
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


                         // -------------------------
                         // class EncoderTestEmployee
                         // -------------------------

// PRIVATE ACCESSORS
template <typename t_HASH_ALGORITHM>
void EncoderTestEmployee::hashAppendImpl(t_HASH_ALGORITHM& hashAlgorithm) const
{
    using bslh::hashAppend;
    hashAppend(hashAlgorithm, this->name());
    hashAppend(hashAlgorithm, this->homeAddress());
    hashAppend(hashAlgorithm, this->age());
}

// CLASS METHODS
// MANIPULATORS
template <typename t_MANIPULATOR>
int EncoderTestEmployee::manipulateAttributes(t_MANIPULATOR& manipulator)
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

    return 0;
}

template <typename t_MANIPULATOR>
int EncoderTestEmployee::manipulateAttribute(t_MANIPULATOR& manipulator, int id)
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

template <typename t_MANIPULATOR>
int EncoderTestEmployee::manipulateAttribute(
        t_MANIPULATOR& manipulator,
        const char    *name,
        int            nameLength)
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
template <typename t_ACCESSOR>
int EncoderTestEmployee::accessAttributes(t_ACCESSOR& accessor) const
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

    return 0;
}

template <typename t_ACCESSOR>
int EncoderTestEmployee::accessAttribute(t_ACCESSOR& accessor, int id) const
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

template <typename t_ACCESSOR>
int EncoderTestEmployee::accessAttribute(
        t_ACCESSOR&  accessor,
        const char  *name,
        int          nameLength) const
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



                 // ------------------------------------------
                 // class EncoderTestSequenceWithAllCategories
                 // ------------------------------------------

// PRIVATE ACCESSORS
template <typename t_HASH_ALGORITHM>
void EncoderTestSequenceWithAllCategories::hashAppendImpl(t_HASH_ALGORITHM& hashAlgorithm) const
{
    using bslh::hashAppend;
    hashAppend(hashAlgorithm, this->charArray());
    hashAppend(hashAlgorithm, this->aString());
    hashAppend(hashAlgorithm, this->array());
    hashAppend(hashAlgorithm, this->choice());
    hashAppend(hashAlgorithm, this->customizedType());
    hashAppend(hashAlgorithm, this->enumeration());
    hashAppend(hashAlgorithm, this->nullableValue());
    hashAppend(hashAlgorithm, this->sequence());
    hashAppend(hashAlgorithm, this->simple());
}

inline
bool EncoderTestSequenceWithAllCategories::isEqualTo(const EncoderTestSequenceWithAllCategories& rhs) const
{
    return this->charArray() == rhs.charArray() &&
           this->aString() == rhs.aString() &&
           this->array() == rhs.array() &&
           this->choice() == rhs.choice() &&
           this->customizedType() == rhs.customizedType() &&
           this->enumeration() == rhs.enumeration() &&
           this->nullableValue() == rhs.nullableValue() &&
           this->sequence() == rhs.sequence() &&
           this->simple() == rhs.simple();
}

// CLASS METHODS
// MANIPULATORS
template <typename t_MANIPULATOR>
int EncoderTestSequenceWithAllCategories::manipulateAttributes(t_MANIPULATOR& manipulator)
{
    int ret;

    ret = manipulator(&d_charArray, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CHAR_ARRAY]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_aString, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_A_STRING]);
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

    return 0;
}

template <typename t_MANIPULATOR>
int EncoderTestSequenceWithAllCategories::manipulateAttribute(t_MANIPULATOR& manipulator, int id)
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_CHAR_ARRAY: {
        return manipulator(&d_charArray, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CHAR_ARRAY]);
      }
      case ATTRIBUTE_ID_A_STRING: {
        return manipulator(&d_aString, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_A_STRING]);
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

template <typename t_MANIPULATOR>
int EncoderTestSequenceWithAllCategories::manipulateAttribute(
        t_MANIPULATOR& manipulator,
        const char    *name,
        int            nameLength)
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
bsl::string& EncoderTestSequenceWithAllCategories::aString()
{
    return d_aString;
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
template <typename t_ACCESSOR>
int EncoderTestSequenceWithAllCategories::accessAttributes(t_ACCESSOR& accessor) const
{
    int ret;

    ret = accessor(d_charArray, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CHAR_ARRAY]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_aString, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_A_STRING]);
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

    return 0;
}

template <typename t_ACCESSOR>
int EncoderTestSequenceWithAllCategories::accessAttribute(t_ACCESSOR& accessor, int id) const
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_CHAR_ARRAY: {
        return accessor(d_charArray, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CHAR_ARRAY]);
      }
      case ATTRIBUTE_ID_A_STRING: {
        return accessor(d_aString, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_A_STRING]);
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

template <typename t_ACCESSOR>
int EncoderTestSequenceWithAllCategories::accessAttribute(
        t_ACCESSOR&  accessor,
        const char  *name,
        int          nameLength) const
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
const bsl::string& EncoderTestSequenceWithAllCategories::aString() const
{
    return d_aString;
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



                   // --------------------------------------
                   // class EncoderTestSequenceWithUntagged1
                   // --------------------------------------

// CLASS METHODS
// MANIPULATORS
template <typename t_MANIPULATOR>
int EncoderTestSequenceWithUntagged1::manipulateAttributes(t_MANIPULATOR& manipulator)
{
    int ret;

    ret = manipulator(&d_sequence, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SEQUENCE]);
    if (ret) {
        return ret;
    }

    return 0;
}

template <typename t_MANIPULATOR>
int EncoderTestSequenceWithUntagged1::manipulateAttribute(t_MANIPULATOR& manipulator, int id)
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

template <typename t_MANIPULATOR>
int EncoderTestSequenceWithUntagged1::manipulateAttribute(
        t_MANIPULATOR& manipulator,
        const char    *name,
        int            nameLength)
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
template <typename t_ACCESSOR>
int EncoderTestSequenceWithUntagged1::accessAttributes(t_ACCESSOR& accessor) const
{
    int ret;

    ret = accessor(d_sequence, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SEQUENCE]);
    if (ret) {
        return ret;
    }

    return 0;
}

template <typename t_ACCESSOR>
int EncoderTestSequenceWithUntagged1::accessAttribute(t_ACCESSOR& accessor, int id) const
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

template <typename t_ACCESSOR>
int EncoderTestSequenceWithUntagged1::accessAttribute(
        t_ACCESSOR&  accessor,
        const char  *name,
        int          nameLength) const
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



                  // ---------------------------------------
                  // class EncoderTestSequenceWithUntagged10
                  // ---------------------------------------

// PRIVATE ACCESSORS
template <typename t_HASH_ALGORITHM>
void EncoderTestSequenceWithUntagged10::hashAppendImpl(t_HASH_ALGORITHM& hashAlgorithm) const
{
    using bslh::hashAppend;
    hashAppend(hashAlgorithm, this->sequence());
    hashAppend(hashAlgorithm, this->attribute0());
    hashAppend(hashAlgorithm, this->attribute1());
}

// CLASS METHODS
// MANIPULATORS
template <typename t_MANIPULATOR>
int EncoderTestSequenceWithUntagged10::manipulateAttributes(t_MANIPULATOR& manipulator)
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

    return 0;
}

template <typename t_MANIPULATOR>
int EncoderTestSequenceWithUntagged10::manipulateAttribute(t_MANIPULATOR& manipulator, int id)
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

template <typename t_MANIPULATOR>
int EncoderTestSequenceWithUntagged10::manipulateAttribute(
        t_MANIPULATOR& manipulator,
        const char    *name,
        int            nameLength)
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
template <typename t_ACCESSOR>
int EncoderTestSequenceWithUntagged10::accessAttributes(t_ACCESSOR& accessor) const
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

    return 0;
}

template <typename t_ACCESSOR>
int EncoderTestSequenceWithUntagged10::accessAttribute(t_ACCESSOR& accessor, int id) const
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

template <typename t_ACCESSOR>
int EncoderTestSequenceWithUntagged10::accessAttribute(
        t_ACCESSOR&  accessor,
        const char  *name,
        int          nameLength) const
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



                  // ---------------------------------------
                  // class EncoderTestSequenceWithUntagged11
                  // ---------------------------------------

// PRIVATE ACCESSORS
template <typename t_HASH_ALGORITHM>
void EncoderTestSequenceWithUntagged11::hashAppendImpl(t_HASH_ALGORITHM& hashAlgorithm) const
{
    using bslh::hashAppend;
    hashAppend(hashAlgorithm, this->attribute0());
    hashAppend(hashAlgorithm, this->sequence());
    hashAppend(hashAlgorithm, this->sequence1());
}

// CLASS METHODS
// MANIPULATORS
template <typename t_MANIPULATOR>
int EncoderTestSequenceWithUntagged11::manipulateAttributes(t_MANIPULATOR& manipulator)
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

    return 0;
}

template <typename t_MANIPULATOR>
int EncoderTestSequenceWithUntagged11::manipulateAttribute(t_MANIPULATOR& manipulator, int id)
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

template <typename t_MANIPULATOR>
int EncoderTestSequenceWithUntagged11::manipulateAttribute(
        t_MANIPULATOR& manipulator,
        const char    *name,
        int            nameLength)
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
template <typename t_ACCESSOR>
int EncoderTestSequenceWithUntagged11::accessAttributes(t_ACCESSOR& accessor) const
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

    return 0;
}

template <typename t_ACCESSOR>
int EncoderTestSequenceWithUntagged11::accessAttribute(t_ACCESSOR& accessor, int id) const
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

template <typename t_ACCESSOR>
int EncoderTestSequenceWithUntagged11::accessAttribute(
        t_ACCESSOR&  accessor,
        const char  *name,
        int          nameLength) const
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



                  // ---------------------------------------
                  // class EncoderTestSequenceWithUntagged12
                  // ---------------------------------------

// PRIVATE ACCESSORS
template <typename t_HASH_ALGORITHM>
void EncoderTestSequenceWithUntagged12::hashAppendImpl(t_HASH_ALGORITHM& hashAlgorithm) const
{
    using bslh::hashAppend;
    hashAppend(hashAlgorithm, this->attribute0());
    hashAppend(hashAlgorithm, this->sequence());
    hashAppend(hashAlgorithm, this->attribute1());
}

// CLASS METHODS
// MANIPULATORS
template <typename t_MANIPULATOR>
int EncoderTestSequenceWithUntagged12::manipulateAttributes(t_MANIPULATOR& manipulator)
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

    return 0;
}

template <typename t_MANIPULATOR>
int EncoderTestSequenceWithUntagged12::manipulateAttribute(t_MANIPULATOR& manipulator, int id)
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

template <typename t_MANIPULATOR>
int EncoderTestSequenceWithUntagged12::manipulateAttribute(
        t_MANIPULATOR& manipulator,
        const char    *name,
        int            nameLength)
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
template <typename t_ACCESSOR>
int EncoderTestSequenceWithUntagged12::accessAttributes(t_ACCESSOR& accessor) const
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

    return 0;
}

template <typename t_ACCESSOR>
int EncoderTestSequenceWithUntagged12::accessAttribute(t_ACCESSOR& accessor, int id) const
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

template <typename t_ACCESSOR>
int EncoderTestSequenceWithUntagged12::accessAttribute(
        t_ACCESSOR&  accessor,
        const char  *name,
        int          nameLength) const
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



                  // ---------------------------------------
                  // class EncoderTestSequenceWithUntagged13
                  // ---------------------------------------

// PRIVATE ACCESSORS
template <typename t_HASH_ALGORITHM>
void EncoderTestSequenceWithUntagged13::hashAppendImpl(t_HASH_ALGORITHM& hashAlgorithm) const
{
    using bslh::hashAppend;
    hashAppend(hashAlgorithm, this->attribute0());
    hashAppend(hashAlgorithm, this->attribute1());
    hashAppend(hashAlgorithm, this->sequence());
}

// CLASS METHODS
// MANIPULATORS
template <typename t_MANIPULATOR>
int EncoderTestSequenceWithUntagged13::manipulateAttributes(t_MANIPULATOR& manipulator)
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

    return 0;
}

template <typename t_MANIPULATOR>
int EncoderTestSequenceWithUntagged13::manipulateAttribute(t_MANIPULATOR& manipulator, int id)
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

template <typename t_MANIPULATOR>
int EncoderTestSequenceWithUntagged13::manipulateAttribute(
        t_MANIPULATOR& manipulator,
        const char    *name,
        int            nameLength)
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
template <typename t_ACCESSOR>
int EncoderTestSequenceWithUntagged13::accessAttributes(t_ACCESSOR& accessor) const
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

    return 0;
}

template <typename t_ACCESSOR>
int EncoderTestSequenceWithUntagged13::accessAttribute(t_ACCESSOR& accessor, int id) const
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

template <typename t_ACCESSOR>
int EncoderTestSequenceWithUntagged13::accessAttribute(
        t_ACCESSOR&  accessor,
        const char  *name,
        int          nameLength) const
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



                   // --------------------------------------
                   // class EncoderTestSequenceWithUntagged3
                   // --------------------------------------

// CLASS METHODS
// MANIPULATORS
template <typename t_MANIPULATOR>
int EncoderTestSequenceWithUntagged3::manipulateAttributes(t_MANIPULATOR& manipulator)
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

    return 0;
}

template <typename t_MANIPULATOR>
int EncoderTestSequenceWithUntagged3::manipulateAttribute(t_MANIPULATOR& manipulator, int id)
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

template <typename t_MANIPULATOR>
int EncoderTestSequenceWithUntagged3::manipulateAttribute(
        t_MANIPULATOR& manipulator,
        const char    *name,
        int            nameLength)
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
template <typename t_ACCESSOR>
int EncoderTestSequenceWithUntagged3::accessAttributes(t_ACCESSOR& accessor) const
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

    return 0;
}

template <typename t_ACCESSOR>
int EncoderTestSequenceWithUntagged3::accessAttribute(t_ACCESSOR& accessor, int id) const
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

template <typename t_ACCESSOR>
int EncoderTestSequenceWithUntagged3::accessAttribute(
        t_ACCESSOR&  accessor,
        const char  *name,
        int          nameLength) const
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



                   // --------------------------------------
                   // class EncoderTestSequenceWithUntagged4
                   // --------------------------------------

// CLASS METHODS
// MANIPULATORS
template <typename t_MANIPULATOR>
int EncoderTestSequenceWithUntagged4::manipulateAttributes(t_MANIPULATOR& manipulator)
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

    return 0;
}

template <typename t_MANIPULATOR>
int EncoderTestSequenceWithUntagged4::manipulateAttribute(t_MANIPULATOR& manipulator, int id)
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

template <typename t_MANIPULATOR>
int EncoderTestSequenceWithUntagged4::manipulateAttribute(
        t_MANIPULATOR& manipulator,
        const char    *name,
        int            nameLength)
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
template <typename t_ACCESSOR>
int EncoderTestSequenceWithUntagged4::accessAttributes(t_ACCESSOR& accessor) const
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

    return 0;
}

template <typename t_ACCESSOR>
int EncoderTestSequenceWithUntagged4::accessAttribute(t_ACCESSOR& accessor, int id) const
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

template <typename t_ACCESSOR>
int EncoderTestSequenceWithUntagged4::accessAttribute(
        t_ACCESSOR&  accessor,
        const char  *name,
        int          nameLength) const
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



                   // --------------------------------------
                   // class EncoderTestSequenceWithUntagged5
                   // --------------------------------------

// CLASS METHODS
// MANIPULATORS
template <typename t_MANIPULATOR>
int EncoderTestSequenceWithUntagged5::manipulateAttributes(t_MANIPULATOR& manipulator)
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

    return 0;
}

template <typename t_MANIPULATOR>
int EncoderTestSequenceWithUntagged5::manipulateAttribute(t_MANIPULATOR& manipulator, int id)
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

template <typename t_MANIPULATOR>
int EncoderTestSequenceWithUntagged5::manipulateAttribute(
        t_MANIPULATOR& manipulator,
        const char    *name,
        int            nameLength)
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
template <typename t_ACCESSOR>
int EncoderTestSequenceWithUntagged5::accessAttributes(t_ACCESSOR& accessor) const
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

    return 0;
}

template <typename t_ACCESSOR>
int EncoderTestSequenceWithUntagged5::accessAttribute(t_ACCESSOR& accessor, int id) const
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

template <typename t_ACCESSOR>
int EncoderTestSequenceWithUntagged5::accessAttribute(
        t_ACCESSOR&  accessor,
        const char  *name,
        int          nameLength) const
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



                   // --------------------------------------
                   // class EncoderTestSequenceWithUntagged7
                   // --------------------------------------

// PRIVATE ACCESSORS
template <typename t_HASH_ALGORITHM>
void EncoderTestSequenceWithUntagged7::hashAppendImpl(t_HASH_ALGORITHM& hashAlgorithm) const
{
    using bslh::hashAppend;
    hashAppend(hashAlgorithm, this->sequence());
    hashAppend(hashAlgorithm, this->sequence1());
    hashAppend(hashAlgorithm, this->sequence2());
}

// CLASS METHODS
// MANIPULATORS
template <typename t_MANIPULATOR>
int EncoderTestSequenceWithUntagged7::manipulateAttributes(t_MANIPULATOR& manipulator)
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

    return 0;
}

template <typename t_MANIPULATOR>
int EncoderTestSequenceWithUntagged7::manipulateAttribute(t_MANIPULATOR& manipulator, int id)
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

template <typename t_MANIPULATOR>
int EncoderTestSequenceWithUntagged7::manipulateAttribute(
        t_MANIPULATOR& manipulator,
        const char    *name,
        int            nameLength)
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
template <typename t_ACCESSOR>
int EncoderTestSequenceWithUntagged7::accessAttributes(t_ACCESSOR& accessor) const
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

    return 0;
}

template <typename t_ACCESSOR>
int EncoderTestSequenceWithUntagged7::accessAttribute(t_ACCESSOR& accessor, int id) const
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

template <typename t_ACCESSOR>
int EncoderTestSequenceWithUntagged7::accessAttribute(
        t_ACCESSOR&  accessor,
        const char  *name,
        int          nameLength) const
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



                   // --------------------------------------
                   // class EncoderTestSequenceWithUntagged8
                   // --------------------------------------

// PRIVATE ACCESSORS
template <typename t_HASH_ALGORITHM>
void EncoderTestSequenceWithUntagged8::hashAppendImpl(t_HASH_ALGORITHM& hashAlgorithm) const
{
    using bslh::hashAppend;
    hashAppend(hashAlgorithm, this->sequence());
    hashAppend(hashAlgorithm, this->sequence1());
    hashAppend(hashAlgorithm, this->attribute0());
}

// CLASS METHODS
// MANIPULATORS
template <typename t_MANIPULATOR>
int EncoderTestSequenceWithUntagged8::manipulateAttributes(t_MANIPULATOR& manipulator)
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

    return 0;
}

template <typename t_MANIPULATOR>
int EncoderTestSequenceWithUntagged8::manipulateAttribute(t_MANIPULATOR& manipulator, int id)
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

template <typename t_MANIPULATOR>
int EncoderTestSequenceWithUntagged8::manipulateAttribute(
        t_MANIPULATOR& manipulator,
        const char    *name,
        int            nameLength)
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
template <typename t_ACCESSOR>
int EncoderTestSequenceWithUntagged8::accessAttributes(t_ACCESSOR& accessor) const
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

    return 0;
}

template <typename t_ACCESSOR>
int EncoderTestSequenceWithUntagged8::accessAttribute(t_ACCESSOR& accessor, int id) const
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

template <typename t_ACCESSOR>
int EncoderTestSequenceWithUntagged8::accessAttribute(
        t_ACCESSOR&  accessor,
        const char  *name,
        int          nameLength) const
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



                   // --------------------------------------
                   // class EncoderTestSequenceWithUntagged9
                   // --------------------------------------

// PRIVATE ACCESSORS
template <typename t_HASH_ALGORITHM>
void EncoderTestSequenceWithUntagged9::hashAppendImpl(t_HASH_ALGORITHM& hashAlgorithm) const
{
    using bslh::hashAppend;
    hashAppend(hashAlgorithm, this->sequence());
    hashAppend(hashAlgorithm, this->attribute0());
    hashAppend(hashAlgorithm, this->sequence1());
}

// CLASS METHODS
// MANIPULATORS
template <typename t_MANIPULATOR>
int EncoderTestSequenceWithUntagged9::manipulateAttributes(t_MANIPULATOR& manipulator)
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

    return 0;
}

template <typename t_MANIPULATOR>
int EncoderTestSequenceWithUntagged9::manipulateAttribute(t_MANIPULATOR& manipulator, int id)
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

template <typename t_MANIPULATOR>
int EncoderTestSequenceWithUntagged9::manipulateAttribute(
        t_MANIPULATOR& manipulator,
        const char    *name,
        int            nameLength)
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
template <typename t_ACCESSOR>
int EncoderTestSequenceWithUntagged9::accessAttributes(t_ACCESSOR& accessor) const
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

    return 0;
}

template <typename t_ACCESSOR>
int EncoderTestSequenceWithUntagged9::accessAttribute(t_ACCESSOR& accessor, int id) const
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

template <typename t_ACCESSOR>
int EncoderTestSequenceWithUntagged9::accessAttribute(
        t_ACCESSOR&  accessor,
        const char  *name,
        int          nameLength) const
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

}  // close package namespace

// FREE FUNCTIONS

}  // close enterprise namespace
#endif

// GENERATED BY BLP_BAS_CODEGEN_2025.02.27.1
// USING bas_codegen.pl --mode msg --noAggregateConversion --noExternalization --msgComponent=encoder_testtypes --package=baljsn baljsn_encoder_testtypes.xsd
// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright 2025 Bloomberg Finance L.P. All rights reserved.
//      Property of Bloomberg Finance L.P. (BFLP)
//      This software is made available solely pursuant to the
//      terms of a BFLP license agreement which governs its use.
// ------------------------------- END-OF-FILE --------------------------------
