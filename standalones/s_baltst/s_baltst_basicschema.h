// s_baltst_basicschema.h          *DO NOT EDIT*           @generated -*-C++-*-
#ifndef INCLUDED_S_BALTST_BASICSCHEMA
#define INCLUDED_S_BALTST_BASICSCHEMA

#include <bsls_ident.h>
BSLS_IDENT_RCSID(s_baltst_basicschema_h,"$Id$ $CSID$")
BSLS_IDENT_PRAGMA_ONCE

//@PURPOSE: Provide value-semantic attribute classes

#include <bslalg_typetraits.h>

#include <bdlat_attributeinfo.h>

#include <bdlat_selectioninfo.h>

#include <bdlat_typetraits.h>

#include <bslh_hash.h>
#include <bsls_objectbuffer.h>

#include <bslma_default.h>

#include <bsls_assert.h>

#include <bdlb_nullablevalue.h>

#include <bsl_string.h>

#include <bsl_vector.h>

#include <bsl_iosfwd.h>
#include <bsl_limits.h>

namespace BloombergLP {

namespace bslma { class Allocator; }

namespace s_baltst { class BasicSchemaAnnotationElement; }
namespace s_baltst { class BasicSchemaEnumerationElementChoice; }
namespace s_baltst { class BasicSchemaEnumerationElement; }
namespace s_baltst { class BasicSchemaRestrictionElementChoice; }
namespace s_baltst { class BasicSchemaRestrictionElement; }
namespace s_baltst { class BasicSchemaSimpleTypeElementChoice; }
namespace s_baltst { class BasicSchemaSimpleTypeElement; }
namespace s_baltst { class BasicSchemaChoiceElement; }
namespace s_baltst { class BasicSchemaChoiceElementChoice; }
namespace s_baltst { class BasicSchemaComplexTypeElement; }
namespace s_baltst { class BasicSchemaComplexTypeElementChoice; }
namespace s_baltst { class BasicSchemaElement; }
namespace s_baltst { class BasicSchemaElementChoice; }
namespace s_baltst { class BasicSchemaSequenceElement; }
namespace s_baltst { class BasicSchemaSequenceElementChoice; }
namespace s_baltst { class BasicSchemaChoice; }
namespace s_baltst { class BasicSchema; }
namespace s_baltst {

                     // ==================================
                     // class BasicSchemaAnnotationElement
                     // ==================================

class BasicSchemaAnnotationElement {

    // INSTANCE DATA
    bdlb::NullableValue<bsl::string>  d_documentation;

  public:
    // TYPES
    enum {
        ATTRIBUTE_ID_DOCUMENTATION = 0
    };

    enum {
        NUM_ATTRIBUTES = 1
    };

    enum {
        ATTRIBUTE_INDEX_DOCUMENTATION = 0
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
    explicit BasicSchemaAnnotationElement(bslma::Allocator *basicAllocator = 0);
        // Create an object of type 'BasicSchemaAnnotationElement' having the
        // default value.  Use the optionally specified 'basicAllocator' to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.

    BasicSchemaAnnotationElement(const BasicSchemaAnnotationElement& original,
                                 bslma::Allocator *basicAllocator = 0);
        // Create an object of type 'BasicSchemaAnnotationElement' having the
        // value of the specified 'original' object.  Use the optionally
        // specified 'basicAllocator' to supply memory.  If 'basicAllocator' is
        // 0, the currently installed default allocator is used.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    BasicSchemaAnnotationElement(BasicSchemaAnnotationElement&& original) noexcept;
        // Create an object of type 'BasicSchemaAnnotationElement' having the
        // value of the specified 'original' object.  After performing this
        // action, the 'original' object will be left in a valid, but
        // unspecified state.

    BasicSchemaAnnotationElement(BasicSchemaAnnotationElement&& original,
                                 bslma::Allocator *basicAllocator);
        // Create an object of type 'BasicSchemaAnnotationElement' having the
        // value of the specified 'original' object.  After performing this
        // action, the 'original' object will be left in a valid, but
        // unspecified state.  Use the optionally specified 'basicAllocator' to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.
#endif

    ~BasicSchemaAnnotationElement();
        // Destroy this object.

    // MANIPULATORS
    BasicSchemaAnnotationElement& operator=(const BasicSchemaAnnotationElement& rhs);
        // Assign to this object the value of the specified 'rhs' object.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    BasicSchemaAnnotationElement& operator=(BasicSchemaAnnotationElement&& rhs);
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

    bdlb::NullableValue<bsl::string>& documentation();
        // Return a reference to the modifiable "Documentation" attribute of
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

    const bdlb::NullableValue<bsl::string>& documentation() const;
        // Return a reference to the non-modifiable "Documentation" attribute
        // of this object.
};

// FREE OPERATORS
inline
bool operator==(const BasicSchemaAnnotationElement& lhs, const BasicSchemaAnnotationElement& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
    // the same value, and 'false' otherwise.  Two attribute objects have the
    // same value if each respective attribute has the same value.

inline
bool operator!=(const BasicSchemaAnnotationElement& lhs, const BasicSchemaAnnotationElement& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
    // have the same value, and 'false' otherwise.  Two attribute objects do
    // not have the same value if one or more respective attributes differ in
    // values.

inline
bsl::ostream& operator<<(bsl::ostream& stream, const BasicSchemaAnnotationElement& rhs);
    // Format the specified 'rhs' to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& hashAlg, const s_baltst::BasicSchemaAnnotationElement& object);
    // Pass the specified 'object' to the specified 'hashAlg'.  This function
    // integrates with the 'bslh' modular hashing system and effectively
    // provides a 'bsl::hash' specialization for 'BasicSchemaAnnotationElement'.

}  // close package namespace

// TRAITS

BDLAT_DECL_SEQUENCE_WITH_ALLOCATOR_BITWISEMOVEABLE_TRAITS(s_baltst::BasicSchemaAnnotationElement)

namespace s_baltst {

                 // =========================================
                 // class BasicSchemaEnumerationElementChoice
                 // =========================================

class BasicSchemaEnumerationElementChoice {

    // INSTANCE DATA
    union {
        bsls::ObjectBuffer< BasicSchemaAnnotationElement > d_annotation;
    };

    int                                                    d_selectionId;
    bslma::Allocator                                      *d_allocator_p;

  public:
    // TYPES

    enum {
        SELECTION_ID_UNDEFINED  = -1
      , SELECTION_ID_ANNOTATION = 0
    };

    enum {
        NUM_SELECTIONS = 1
    };

    enum {
        SELECTION_INDEX_ANNOTATION = 0
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
    explicit BasicSchemaEnumerationElementChoice(bslma::Allocator *basicAllocator = 0);
        // Create an object of type 'BasicSchemaEnumerationElementChoice'
        // having the default value.  Use the optionally specified
        // 'basicAllocator' to supply memory.  If 'basicAllocator' is 0, the
        // currently installed default allocator is used.

    BasicSchemaEnumerationElementChoice(const BasicSchemaEnumerationElementChoice& original,
                                       bslma::Allocator *basicAllocator = 0);
        // Create an object of type 'BasicSchemaEnumerationElementChoice'
        // having the value of the specified 'original' object.  Use the
        // optionally specified 'basicAllocator' to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    BasicSchemaEnumerationElementChoice(BasicSchemaEnumerationElementChoice&& original) noexcept;
        // Create an object of type 'BasicSchemaEnumerationElementChoice'
        // having the value of the specified 'original' object.  After
        // performing this action, the 'original' object will be left in a
        // valid, but unspecified state.

    BasicSchemaEnumerationElementChoice(BasicSchemaEnumerationElementChoice&& original,
                                       bslma::Allocator *basicAllocator);
        // Create an object of type 'BasicSchemaEnumerationElementChoice'
        // having the value of the specified 'original' object.  After
        // performing this action, the 'original' object will be left in a
        // valid, but unspecified state.  Use the optionally specified
        // 'basicAllocator' to supply memory.  If 'basicAllocator' is 0, the
        // currently installed default allocator is used.
#endif

    ~BasicSchemaEnumerationElementChoice();
        // Destroy this object.

    // MANIPULATORS
    BasicSchemaEnumerationElementChoice& operator=(const BasicSchemaEnumerationElementChoice& rhs);
        // Assign to this object the value of the specified 'rhs' object.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    BasicSchemaEnumerationElementChoice& operator=(BasicSchemaEnumerationElementChoice&& rhs);
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

    BasicSchemaAnnotationElement& makeAnnotation();
    BasicSchemaAnnotationElement& makeAnnotation(const BasicSchemaAnnotationElement& value);
#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    BasicSchemaAnnotationElement& makeAnnotation(BasicSchemaAnnotationElement&& value);
#endif
        // Set the value of this object to be a "Annotation" value.  Optionally
        // specify the 'value' of the "Annotation".  If 'value' is not
        // specified, the default "Annotation" value is used.

    template<class MANIPULATOR>
    int manipulateSelection(MANIPULATOR& manipulator);
        // Invoke the specified 'manipulator' on the address of the modifiable
        // selection, supplying 'manipulator' with the corresponding selection
        // information structure.  Return the value returned from the
        // invocation of 'manipulator' if this object has a defined selection,
        // and -1 otherwise.

    BasicSchemaAnnotationElement& annotation();
        // Return a reference to the modifiable "Annotation" selection of this
        // object if "Annotation" is the current selection.  The behavior is
        // undefined unless "Annotation" is the selection of this object.

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

    const BasicSchemaAnnotationElement& annotation() const;
        // Return a reference to the non-modifiable "Annotation" selection of
        // this object if "Annotation" is the current selection.  The behavior
        // is undefined unless "Annotation" is the selection of this object.

    bool isAnnotationValue() const;
        // Return 'true' if the value of this object is a "Annotation" value,
        // and return 'false' otherwise.

    bool isUndefinedValue() const;
        // Return 'true' if the value of this object is undefined, and 'false'
        // otherwise.

    const char *selectionName() const;
        // Return the symbolic name of the current selection of this object.
};

// FREE OPERATORS
inline
bool operator==(const BasicSchemaEnumerationElementChoice& lhs, const BasicSchemaEnumerationElementChoice& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects have the same
    // value, and 'false' otherwise.  Two 'BasicSchemaEnumerationElementChoice' objects have the same
    // value if either the selections in both objects have the same ids and
    // the same values, or both selections are undefined.

inline
bool operator!=(const BasicSchemaEnumerationElementChoice& lhs, const BasicSchemaEnumerationElementChoice& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects do not have the
    // same values, as determined by 'operator==', and 'false' otherwise.

inline
bsl::ostream& operator<<(bsl::ostream& stream, const BasicSchemaEnumerationElementChoice& rhs);
    // Format the specified 'rhs' to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& hashAlg, const s_baltst::BasicSchemaEnumerationElementChoice& object);
    // Pass the specified 'object' to the specified 'hashAlg'.  This function
    // integrates with the 'bslh' modular hashing system and effectively
    // provides a 'bsl::hash' specialization for 'BasicSchemaEnumerationElementChoice'.

}  // close package namespace

// TRAITS

BDLAT_DECL_CHOICE_WITH_ALLOCATOR_BITWISEMOVEABLE_TRAITS(s_baltst::BasicSchemaEnumerationElementChoice)

namespace s_baltst {

                    // ===================================
                    // class BasicSchemaEnumerationElement
                    // ===================================

class BasicSchemaEnumerationElement {

    // INSTANCE DATA
    bsl::vector<BasicSchemaEnumerationElementChoice>  d_choice;
    bdlb::NullableValue<bsl::string>                  d_value;

  public:
    // TYPES
    enum {
        ATTRIBUTE_ID_VALUE  = 0
      , ATTRIBUTE_ID_CHOICE = 1
    };

    enum {
        NUM_ATTRIBUTES = 2
    };

    enum {
        ATTRIBUTE_INDEX_VALUE  = 0
      , ATTRIBUTE_INDEX_CHOICE = 1
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
    explicit BasicSchemaEnumerationElement(bslma::Allocator *basicAllocator = 0);
        // Create an object of type 'BasicSchemaEnumerationElement' having the
        // default value.  Use the optionally specified 'basicAllocator' to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.

    BasicSchemaEnumerationElement(const BasicSchemaEnumerationElement& original,
                                  bslma::Allocator *basicAllocator = 0);
        // Create an object of type 'BasicSchemaEnumerationElement' having the
        // value of the specified 'original' object.  Use the optionally
        // specified 'basicAllocator' to supply memory.  If 'basicAllocator' is
        // 0, the currently installed default allocator is used.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    BasicSchemaEnumerationElement(BasicSchemaEnumerationElement&& original) noexcept;
        // Create an object of type 'BasicSchemaEnumerationElement' having the
        // value of the specified 'original' object.  After performing this
        // action, the 'original' object will be left in a valid, but
        // unspecified state.

    BasicSchemaEnumerationElement(BasicSchemaEnumerationElement&& original,
                                  bslma::Allocator *basicAllocator);
        // Create an object of type 'BasicSchemaEnumerationElement' having the
        // value of the specified 'original' object.  After performing this
        // action, the 'original' object will be left in a valid, but
        // unspecified state.  Use the optionally specified 'basicAllocator' to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.
#endif

    ~BasicSchemaEnumerationElement();
        // Destroy this object.

    // MANIPULATORS
    BasicSchemaEnumerationElement& operator=(const BasicSchemaEnumerationElement& rhs);
        // Assign to this object the value of the specified 'rhs' object.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    BasicSchemaEnumerationElement& operator=(BasicSchemaEnumerationElement&& rhs);
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

    bdlb::NullableValue<bsl::string>& value();
        // Return a reference to the modifiable "Value" attribute of this
        // object.

    bsl::vector<BasicSchemaEnumerationElementChoice>& choice();
        // Return a reference to the modifiable "Choice" attribute of this
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

    const bdlb::NullableValue<bsl::string>& value() const;
        // Return a reference to the non-modifiable "Value" attribute of this
        // object.

    const bsl::vector<BasicSchemaEnumerationElementChoice>& choice() const;
        // Return a reference to the non-modifiable "Choice" attribute of this
        // object.
};

// FREE OPERATORS
inline
bool operator==(const BasicSchemaEnumerationElement& lhs, const BasicSchemaEnumerationElement& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
    // the same value, and 'false' otherwise.  Two attribute objects have the
    // same value if each respective attribute has the same value.

inline
bool operator!=(const BasicSchemaEnumerationElement& lhs, const BasicSchemaEnumerationElement& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
    // have the same value, and 'false' otherwise.  Two attribute objects do
    // not have the same value if one or more respective attributes differ in
    // values.

inline
bsl::ostream& operator<<(bsl::ostream& stream, const BasicSchemaEnumerationElement& rhs);
    // Format the specified 'rhs' to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& hashAlg, const s_baltst::BasicSchemaEnumerationElement& object);
    // Pass the specified 'object' to the specified 'hashAlg'.  This function
    // integrates with the 'bslh' modular hashing system and effectively
    // provides a 'bsl::hash' specialization for 'BasicSchemaEnumerationElement'.

}  // close package namespace

// TRAITS

BDLAT_DECL_SEQUENCE_WITH_ALLOCATOR_BITWISEMOVEABLE_TRAITS(s_baltst::BasicSchemaEnumerationElement)

namespace s_baltst {

                 // =========================================
                 // class BasicSchemaRestrictionElementChoice
                 // =========================================

class BasicSchemaRestrictionElementChoice {

    // INSTANCE DATA
    union {
        bsls::ObjectBuffer< BasicSchemaAnnotationElement >  d_annotation;
        bsls::ObjectBuffer< BasicSchemaEnumerationElement > d_enumeration;
    };

    int                                                     d_selectionId;
    bslma::Allocator                                       *d_allocator_p;

  public:
    // TYPES

    enum {
        SELECTION_ID_UNDEFINED   = -1
      , SELECTION_ID_ANNOTATION  = 0
      , SELECTION_ID_ENUMERATION = 1
    };

    enum {
        NUM_SELECTIONS = 2
    };

    enum {
        SELECTION_INDEX_ANNOTATION  = 0
      , SELECTION_INDEX_ENUMERATION = 1
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
    explicit BasicSchemaRestrictionElementChoice(bslma::Allocator *basicAllocator = 0);
        // Create an object of type 'BasicSchemaRestrictionElementChoice'
        // having the default value.  Use the optionally specified
        // 'basicAllocator' to supply memory.  If 'basicAllocator' is 0, the
        // currently installed default allocator is used.

    BasicSchemaRestrictionElementChoice(const BasicSchemaRestrictionElementChoice& original,
                                       bslma::Allocator *basicAllocator = 0);
        // Create an object of type 'BasicSchemaRestrictionElementChoice'
        // having the value of the specified 'original' object.  Use the
        // optionally specified 'basicAllocator' to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    BasicSchemaRestrictionElementChoice(BasicSchemaRestrictionElementChoice&& original) noexcept;
        // Create an object of type 'BasicSchemaRestrictionElementChoice'
        // having the value of the specified 'original' object.  After
        // performing this action, the 'original' object will be left in a
        // valid, but unspecified state.

    BasicSchemaRestrictionElementChoice(BasicSchemaRestrictionElementChoice&& original,
                                       bslma::Allocator *basicAllocator);
        // Create an object of type 'BasicSchemaRestrictionElementChoice'
        // having the value of the specified 'original' object.  After
        // performing this action, the 'original' object will be left in a
        // valid, but unspecified state.  Use the optionally specified
        // 'basicAllocator' to supply memory.  If 'basicAllocator' is 0, the
        // currently installed default allocator is used.
#endif

    ~BasicSchemaRestrictionElementChoice();
        // Destroy this object.

    // MANIPULATORS
    BasicSchemaRestrictionElementChoice& operator=(const BasicSchemaRestrictionElementChoice& rhs);
        // Assign to this object the value of the specified 'rhs' object.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    BasicSchemaRestrictionElementChoice& operator=(BasicSchemaRestrictionElementChoice&& rhs);
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

    BasicSchemaAnnotationElement& makeAnnotation();
    BasicSchemaAnnotationElement& makeAnnotation(const BasicSchemaAnnotationElement& value);
#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    BasicSchemaAnnotationElement& makeAnnotation(BasicSchemaAnnotationElement&& value);
#endif
        // Set the value of this object to be a "Annotation" value.  Optionally
        // specify the 'value' of the "Annotation".  If 'value' is not
        // specified, the default "Annotation" value is used.

    BasicSchemaEnumerationElement& makeEnumeration();
    BasicSchemaEnumerationElement& makeEnumeration(const BasicSchemaEnumerationElement& value);
#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    BasicSchemaEnumerationElement& makeEnumeration(BasicSchemaEnumerationElement&& value);
#endif
        // Set the value of this object to be a "Enumeration" value.
        // Optionally specify the 'value' of the "Enumeration".  If 'value' is
        // not specified, the default "Enumeration" value is used.

    template<class MANIPULATOR>
    int manipulateSelection(MANIPULATOR& manipulator);
        // Invoke the specified 'manipulator' on the address of the modifiable
        // selection, supplying 'manipulator' with the corresponding selection
        // information structure.  Return the value returned from the
        // invocation of 'manipulator' if this object has a defined selection,
        // and -1 otherwise.

    BasicSchemaAnnotationElement& annotation();
        // Return a reference to the modifiable "Annotation" selection of this
        // object if "Annotation" is the current selection.  The behavior is
        // undefined unless "Annotation" is the selection of this object.

    BasicSchemaEnumerationElement& enumeration();
        // Return a reference to the modifiable "Enumeration" selection of this
        // object if "Enumeration" is the current selection.  The behavior is
        // undefined unless "Enumeration" is the selection of this object.

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

    const BasicSchemaAnnotationElement& annotation() const;
        // Return a reference to the non-modifiable "Annotation" selection of
        // this object if "Annotation" is the current selection.  The behavior
        // is undefined unless "Annotation" is the selection of this object.

    const BasicSchemaEnumerationElement& enumeration() const;
        // Return a reference to the non-modifiable "Enumeration" selection of
        // this object if "Enumeration" is the current selection.  The behavior
        // is undefined unless "Enumeration" is the selection of this object.

    bool isAnnotationValue() const;
        // Return 'true' if the value of this object is a "Annotation" value,
        // and return 'false' otherwise.

    bool isEnumerationValue() const;
        // Return 'true' if the value of this object is a "Enumeration" value,
        // and return 'false' otherwise.

    bool isUndefinedValue() const;
        // Return 'true' if the value of this object is undefined, and 'false'
        // otherwise.

    const char *selectionName() const;
        // Return the symbolic name of the current selection of this object.
};

// FREE OPERATORS
inline
bool operator==(const BasicSchemaRestrictionElementChoice& lhs, const BasicSchemaRestrictionElementChoice& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects have the same
    // value, and 'false' otherwise.  Two 'BasicSchemaRestrictionElementChoice' objects have the same
    // value if either the selections in both objects have the same ids and
    // the same values, or both selections are undefined.

inline
bool operator!=(const BasicSchemaRestrictionElementChoice& lhs, const BasicSchemaRestrictionElementChoice& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects do not have the
    // same values, as determined by 'operator==', and 'false' otherwise.

inline
bsl::ostream& operator<<(bsl::ostream& stream, const BasicSchemaRestrictionElementChoice& rhs);
    // Format the specified 'rhs' to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& hashAlg, const s_baltst::BasicSchemaRestrictionElementChoice& object);
    // Pass the specified 'object' to the specified 'hashAlg'.  This function
    // integrates with the 'bslh' modular hashing system and effectively
    // provides a 'bsl::hash' specialization for 'BasicSchemaRestrictionElementChoice'.

}  // close package namespace

// TRAITS

BDLAT_DECL_CHOICE_WITH_ALLOCATOR_BITWISEMOVEABLE_TRAITS(s_baltst::BasicSchemaRestrictionElementChoice)

namespace s_baltst {

                    // ===================================
                    // class BasicSchemaRestrictionElement
                    // ===================================

class BasicSchemaRestrictionElement {

    // INSTANCE DATA
    bsl::vector<BasicSchemaRestrictionElementChoice>  d_choice;
    bdlb::NullableValue<bsl::string>                  d_base;

  public:
    // TYPES
    enum {
        ATTRIBUTE_ID_BASE   = 0
      , ATTRIBUTE_ID_CHOICE = 1
    };

    enum {
        NUM_ATTRIBUTES = 2
    };

    enum {
        ATTRIBUTE_INDEX_BASE   = 0
      , ATTRIBUTE_INDEX_CHOICE = 1
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
    explicit BasicSchemaRestrictionElement(bslma::Allocator *basicAllocator = 0);
        // Create an object of type 'BasicSchemaRestrictionElement' having the
        // default value.  Use the optionally specified 'basicAllocator' to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.

    BasicSchemaRestrictionElement(const BasicSchemaRestrictionElement& original,
                                  bslma::Allocator *basicAllocator = 0);
        // Create an object of type 'BasicSchemaRestrictionElement' having the
        // value of the specified 'original' object.  Use the optionally
        // specified 'basicAllocator' to supply memory.  If 'basicAllocator' is
        // 0, the currently installed default allocator is used.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    BasicSchemaRestrictionElement(BasicSchemaRestrictionElement&& original) noexcept;
        // Create an object of type 'BasicSchemaRestrictionElement' having the
        // value of the specified 'original' object.  After performing this
        // action, the 'original' object will be left in a valid, but
        // unspecified state.

    BasicSchemaRestrictionElement(BasicSchemaRestrictionElement&& original,
                                  bslma::Allocator *basicAllocator);
        // Create an object of type 'BasicSchemaRestrictionElement' having the
        // value of the specified 'original' object.  After performing this
        // action, the 'original' object will be left in a valid, but
        // unspecified state.  Use the optionally specified 'basicAllocator' to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.
#endif

    ~BasicSchemaRestrictionElement();
        // Destroy this object.

    // MANIPULATORS
    BasicSchemaRestrictionElement& operator=(const BasicSchemaRestrictionElement& rhs);
        // Assign to this object the value of the specified 'rhs' object.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    BasicSchemaRestrictionElement& operator=(BasicSchemaRestrictionElement&& rhs);
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

    bdlb::NullableValue<bsl::string>& base();
        // Return a reference to the modifiable "Base" attribute of this
        // object.

    bsl::vector<BasicSchemaRestrictionElementChoice>& choice();
        // Return a reference to the modifiable "Choice" attribute of this
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

    const bdlb::NullableValue<bsl::string>& base() const;
        // Return a reference to the non-modifiable "Base" attribute of this
        // object.

    const bsl::vector<BasicSchemaRestrictionElementChoice>& choice() const;
        // Return a reference to the non-modifiable "Choice" attribute of this
        // object.
};

// FREE OPERATORS
inline
bool operator==(const BasicSchemaRestrictionElement& lhs, const BasicSchemaRestrictionElement& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
    // the same value, and 'false' otherwise.  Two attribute objects have the
    // same value if each respective attribute has the same value.

inline
bool operator!=(const BasicSchemaRestrictionElement& lhs, const BasicSchemaRestrictionElement& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
    // have the same value, and 'false' otherwise.  Two attribute objects do
    // not have the same value if one or more respective attributes differ in
    // values.

inline
bsl::ostream& operator<<(bsl::ostream& stream, const BasicSchemaRestrictionElement& rhs);
    // Format the specified 'rhs' to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& hashAlg, const s_baltst::BasicSchemaRestrictionElement& object);
    // Pass the specified 'object' to the specified 'hashAlg'.  This function
    // integrates with the 'bslh' modular hashing system and effectively
    // provides a 'bsl::hash' specialization for 'BasicSchemaRestrictionElement'.

}  // close package namespace

// TRAITS

BDLAT_DECL_SEQUENCE_WITH_ALLOCATOR_BITWISEMOVEABLE_TRAITS(s_baltst::BasicSchemaRestrictionElement)

namespace s_baltst {

                  // ========================================
                  // class BasicSchemaSimpleTypeElementChoice
                  // ========================================

class BasicSchemaSimpleTypeElementChoice {

    // INSTANCE DATA
    union {
        bsls::ObjectBuffer< BasicSchemaAnnotationElement >  d_annotation;
        bsls::ObjectBuffer< BasicSchemaRestrictionElement > d_restriction;
    };

    int                                                     d_selectionId;
    bslma::Allocator                                       *d_allocator_p;

  public:
    // TYPES

    enum {
        SELECTION_ID_UNDEFINED   = -1
      , SELECTION_ID_ANNOTATION  = 0
      , SELECTION_ID_RESTRICTION = 1
    };

    enum {
        NUM_SELECTIONS = 2
    };

    enum {
        SELECTION_INDEX_ANNOTATION  = 0
      , SELECTION_INDEX_RESTRICTION = 1
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
    explicit BasicSchemaSimpleTypeElementChoice(bslma::Allocator *basicAllocator = 0);
        // Create an object of type 'BasicSchemaSimpleTypeElementChoice' having
        // the default value.  Use the optionally specified 'basicAllocator' to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.

    BasicSchemaSimpleTypeElementChoice(const BasicSchemaSimpleTypeElementChoice& original,
                                      bslma::Allocator *basicAllocator = 0);
        // Create an object of type 'BasicSchemaSimpleTypeElementChoice' having
        // the value of the specified 'original' object.  Use the optionally
        // specified 'basicAllocator' to supply memory.  If 'basicAllocator' is
        // 0, the currently installed default allocator is used.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    BasicSchemaSimpleTypeElementChoice(BasicSchemaSimpleTypeElementChoice&& original) noexcept;
        // Create an object of type 'BasicSchemaSimpleTypeElementChoice' having
        // the value of the specified 'original' object.  After performing this
        // action, the 'original' object will be left in a valid, but
        // unspecified state.

    BasicSchemaSimpleTypeElementChoice(BasicSchemaSimpleTypeElementChoice&& original,
                                      bslma::Allocator *basicAllocator);
        // Create an object of type 'BasicSchemaSimpleTypeElementChoice' having
        // the value of the specified 'original' object.  After performing this
        // action, the 'original' object will be left in a valid, but
        // unspecified state.  Use the optionally specified 'basicAllocator' to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.
#endif

    ~BasicSchemaSimpleTypeElementChoice();
        // Destroy this object.

    // MANIPULATORS
    BasicSchemaSimpleTypeElementChoice& operator=(const BasicSchemaSimpleTypeElementChoice& rhs);
        // Assign to this object the value of the specified 'rhs' object.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    BasicSchemaSimpleTypeElementChoice& operator=(BasicSchemaSimpleTypeElementChoice&& rhs);
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

    BasicSchemaAnnotationElement& makeAnnotation();
    BasicSchemaAnnotationElement& makeAnnotation(const BasicSchemaAnnotationElement& value);
#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    BasicSchemaAnnotationElement& makeAnnotation(BasicSchemaAnnotationElement&& value);
#endif
        // Set the value of this object to be a "Annotation" value.  Optionally
        // specify the 'value' of the "Annotation".  If 'value' is not
        // specified, the default "Annotation" value is used.

    BasicSchemaRestrictionElement& makeRestriction();
    BasicSchemaRestrictionElement& makeRestriction(const BasicSchemaRestrictionElement& value);
#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    BasicSchemaRestrictionElement& makeRestriction(BasicSchemaRestrictionElement&& value);
#endif
        // Set the value of this object to be a "Restriction" value.
        // Optionally specify the 'value' of the "Restriction".  If 'value' is
        // not specified, the default "Restriction" value is used.

    template<class MANIPULATOR>
    int manipulateSelection(MANIPULATOR& manipulator);
        // Invoke the specified 'manipulator' on the address of the modifiable
        // selection, supplying 'manipulator' with the corresponding selection
        // information structure.  Return the value returned from the
        // invocation of 'manipulator' if this object has a defined selection,
        // and -1 otherwise.

    BasicSchemaAnnotationElement& annotation();
        // Return a reference to the modifiable "Annotation" selection of this
        // object if "Annotation" is the current selection.  The behavior is
        // undefined unless "Annotation" is the selection of this object.

    BasicSchemaRestrictionElement& restriction();
        // Return a reference to the modifiable "Restriction" selection of this
        // object if "Restriction" is the current selection.  The behavior is
        // undefined unless "Restriction" is the selection of this object.

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

    const BasicSchemaAnnotationElement& annotation() const;
        // Return a reference to the non-modifiable "Annotation" selection of
        // this object if "Annotation" is the current selection.  The behavior
        // is undefined unless "Annotation" is the selection of this object.

    const BasicSchemaRestrictionElement& restriction() const;
        // Return a reference to the non-modifiable "Restriction" selection of
        // this object if "Restriction" is the current selection.  The behavior
        // is undefined unless "Restriction" is the selection of this object.

    bool isAnnotationValue() const;
        // Return 'true' if the value of this object is a "Annotation" value,
        // and return 'false' otherwise.

    bool isRestrictionValue() const;
        // Return 'true' if the value of this object is a "Restriction" value,
        // and return 'false' otherwise.

    bool isUndefinedValue() const;
        // Return 'true' if the value of this object is undefined, and 'false'
        // otherwise.

    const char *selectionName() const;
        // Return the symbolic name of the current selection of this object.
};

// FREE OPERATORS
inline
bool operator==(const BasicSchemaSimpleTypeElementChoice& lhs, const BasicSchemaSimpleTypeElementChoice& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects have the same
    // value, and 'false' otherwise.  Two 'BasicSchemaSimpleTypeElementChoice' objects have the same
    // value if either the selections in both objects have the same ids and
    // the same values, or both selections are undefined.

inline
bool operator!=(const BasicSchemaSimpleTypeElementChoice& lhs, const BasicSchemaSimpleTypeElementChoice& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects do not have the
    // same values, as determined by 'operator==', and 'false' otherwise.

inline
bsl::ostream& operator<<(bsl::ostream& stream, const BasicSchemaSimpleTypeElementChoice& rhs);
    // Format the specified 'rhs' to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& hashAlg, const s_baltst::BasicSchemaSimpleTypeElementChoice& object);
    // Pass the specified 'object' to the specified 'hashAlg'.  This function
    // integrates with the 'bslh' modular hashing system and effectively
    // provides a 'bsl::hash' specialization for 'BasicSchemaSimpleTypeElementChoice'.

}  // close package namespace

// TRAITS

BDLAT_DECL_CHOICE_WITH_ALLOCATOR_BITWISEMOVEABLE_TRAITS(s_baltst::BasicSchemaSimpleTypeElementChoice)

namespace s_baltst {

                     // ==================================
                     // class BasicSchemaSimpleTypeElement
                     // ==================================

class BasicSchemaSimpleTypeElement {

    // INSTANCE DATA
    bsl::vector<BasicSchemaSimpleTypeElementChoice>  d_choice;
    bdlb::NullableValue<bsl::string>                 d_name;
    bdlb::NullableValue<bsl::string>                 d_preserveEnumOrder;

  public:
    // TYPES
    enum {
        ATTRIBUTE_ID_NAME                = 0
      , ATTRIBUTE_ID_PRESERVE_ENUM_ORDER = 1
      , ATTRIBUTE_ID_CHOICE              = 2
    };

    enum {
        NUM_ATTRIBUTES = 3
    };

    enum {
        ATTRIBUTE_INDEX_NAME                = 0
      , ATTRIBUTE_INDEX_PRESERVE_ENUM_ORDER = 1
      , ATTRIBUTE_INDEX_CHOICE              = 2
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
    explicit BasicSchemaSimpleTypeElement(bslma::Allocator *basicAllocator = 0);
        // Create an object of type 'BasicSchemaSimpleTypeElement' having the
        // default value.  Use the optionally specified 'basicAllocator' to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.

    BasicSchemaSimpleTypeElement(const BasicSchemaSimpleTypeElement& original,
                                 bslma::Allocator *basicAllocator = 0);
        // Create an object of type 'BasicSchemaSimpleTypeElement' having the
        // value of the specified 'original' object.  Use the optionally
        // specified 'basicAllocator' to supply memory.  If 'basicAllocator' is
        // 0, the currently installed default allocator is used.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    BasicSchemaSimpleTypeElement(BasicSchemaSimpleTypeElement&& original) noexcept;
        // Create an object of type 'BasicSchemaSimpleTypeElement' having the
        // value of the specified 'original' object.  After performing this
        // action, the 'original' object will be left in a valid, but
        // unspecified state.

    BasicSchemaSimpleTypeElement(BasicSchemaSimpleTypeElement&& original,
                                 bslma::Allocator *basicAllocator);
        // Create an object of type 'BasicSchemaSimpleTypeElement' having the
        // value of the specified 'original' object.  After performing this
        // action, the 'original' object will be left in a valid, but
        // unspecified state.  Use the optionally specified 'basicAllocator' to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.
#endif

    ~BasicSchemaSimpleTypeElement();
        // Destroy this object.

    // MANIPULATORS
    BasicSchemaSimpleTypeElement& operator=(const BasicSchemaSimpleTypeElement& rhs);
        // Assign to this object the value of the specified 'rhs' object.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    BasicSchemaSimpleTypeElement& operator=(BasicSchemaSimpleTypeElement&& rhs);
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

    bdlb::NullableValue<bsl::string>& name();
        // Return a reference to the modifiable "Name" attribute of this
        // object.

    bdlb::NullableValue<bsl::string>& preserveEnumOrder();
        // Return a reference to the modifiable "PreserveEnumOrder" attribute
        // of this object.

    bsl::vector<BasicSchemaSimpleTypeElementChoice>& choice();
        // Return a reference to the modifiable "Choice" attribute of this
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

    const bdlb::NullableValue<bsl::string>& name() const;
        // Return a reference to the non-modifiable "Name" attribute of this
        // object.

    const bdlb::NullableValue<bsl::string>& preserveEnumOrder() const;
        // Return a reference to the non-modifiable "PreserveEnumOrder"
        // attribute of this object.

    const bsl::vector<BasicSchemaSimpleTypeElementChoice>& choice() const;
        // Return a reference to the non-modifiable "Choice" attribute of this
        // object.
};

// FREE OPERATORS
inline
bool operator==(const BasicSchemaSimpleTypeElement& lhs, const BasicSchemaSimpleTypeElement& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
    // the same value, and 'false' otherwise.  Two attribute objects have the
    // same value if each respective attribute has the same value.

inline
bool operator!=(const BasicSchemaSimpleTypeElement& lhs, const BasicSchemaSimpleTypeElement& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
    // have the same value, and 'false' otherwise.  Two attribute objects do
    // not have the same value if one or more respective attributes differ in
    // values.

inline
bsl::ostream& operator<<(bsl::ostream& stream, const BasicSchemaSimpleTypeElement& rhs);
    // Format the specified 'rhs' to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& hashAlg, const s_baltst::BasicSchemaSimpleTypeElement& object);
    // Pass the specified 'object' to the specified 'hashAlg'.  This function
    // integrates with the 'bslh' modular hashing system and effectively
    // provides a 'bsl::hash' specialization for 'BasicSchemaSimpleTypeElement'.

}  // close package namespace

// TRAITS

BDLAT_DECL_SEQUENCE_WITH_ALLOCATOR_BITWISEMOVEABLE_TRAITS(s_baltst::BasicSchemaSimpleTypeElement)

namespace s_baltst {

                       // ==============================
                       // class BasicSchemaChoiceElement
                       // ==============================

class BasicSchemaChoiceElement {

    // INSTANCE DATA
    bsl::vector<BasicSchemaChoiceElementChoice>  d_choice;

  public:
    // TYPES
    enum {
        ATTRIBUTE_ID_CHOICE = 0
    };

    enum {
        NUM_ATTRIBUTES = 1
    };

    enum {
        ATTRIBUTE_INDEX_CHOICE = 0
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
    explicit BasicSchemaChoiceElement(bslma::Allocator *basicAllocator = 0);
        // Create an object of type 'BasicSchemaChoiceElement' having the
        // default value.  Use the optionally specified 'basicAllocator' to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.

    BasicSchemaChoiceElement(const BasicSchemaChoiceElement& original,
                             bslma::Allocator *basicAllocator = 0);
        // Create an object of type 'BasicSchemaChoiceElement' having the value
        // of the specified 'original' object.  Use the optionally specified
        // 'basicAllocator' to supply memory.  If 'basicAllocator' is 0, the
        // currently installed default allocator is used.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    BasicSchemaChoiceElement(BasicSchemaChoiceElement&& original) noexcept;
        // Create an object of type 'BasicSchemaChoiceElement' having the value
        // of the specified 'original' object.  After performing this action,
        // the 'original' object will be left in a valid, but unspecified
        // state.

    BasicSchemaChoiceElement(BasicSchemaChoiceElement&& original,
                             bslma::Allocator *basicAllocator);
        // Create an object of type 'BasicSchemaChoiceElement' having the value
        // of the specified 'original' object.  After performing this action,
        // the 'original' object will be left in a valid, but unspecified
        // state.  Use the optionally specified 'basicAllocator' to supply
        // memory.  If 'basicAllocator' is 0, the currently installed default
        // allocator is used.
#endif

    ~BasicSchemaChoiceElement();
        // Destroy this object.

    // MANIPULATORS
    BasicSchemaChoiceElement& operator=(const BasicSchemaChoiceElement& rhs);
        // Assign to this object the value of the specified 'rhs' object.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    BasicSchemaChoiceElement& operator=(BasicSchemaChoiceElement&& rhs);
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

    bsl::vector<BasicSchemaChoiceElementChoice>& choice();
        // Return a reference to the modifiable "Choice" attribute of this
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

    const bsl::vector<BasicSchemaChoiceElementChoice>& choice() const;
        // Return a reference to the non-modifiable "Choice" attribute of this
        // object.
};

// FREE OPERATORS
inline
bool operator==(const BasicSchemaChoiceElement& lhs, const BasicSchemaChoiceElement& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
    // the same value, and 'false' otherwise.  Two attribute objects have the
    // same value if each respective attribute has the same value.

inline
bool operator!=(const BasicSchemaChoiceElement& lhs, const BasicSchemaChoiceElement& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
    // have the same value, and 'false' otherwise.  Two attribute objects do
    // not have the same value if one or more respective attributes differ in
    // values.

inline
bsl::ostream& operator<<(bsl::ostream& stream, const BasicSchemaChoiceElement& rhs);
    // Format the specified 'rhs' to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& hashAlg, const s_baltst::BasicSchemaChoiceElement& object);
    // Pass the specified 'object' to the specified 'hashAlg'.  This function
    // integrates with the 'bslh' modular hashing system and effectively
    // provides a 'bsl::hash' specialization for 'BasicSchemaChoiceElement'.

}  // close package namespace

// TRAITS

BDLAT_DECL_SEQUENCE_WITH_ALLOCATOR_BITWISEMOVEABLE_TRAITS(s_baltst::BasicSchemaChoiceElement)

namespace s_baltst {

                    // ====================================
                    // class BasicSchemaChoiceElementChoice
                    // ====================================

class BasicSchemaChoiceElementChoice {

    // INSTANCE DATA
    union {
        bsls::ObjectBuffer< BasicSchemaAnnotationElement >  d_annotation;
        BasicSchemaElement                                *d_element;
    };

    int                                                     d_selectionId;
    bslma::Allocator                                       *d_allocator_p;

  public:
    // TYPES

    enum {
        SELECTION_ID_UNDEFINED  = -1
      , SELECTION_ID_ANNOTATION = 0
      , SELECTION_ID_ELEMENT    = 1
    };

    enum {
        NUM_SELECTIONS = 2
    };

    enum {
        SELECTION_INDEX_ANNOTATION = 0
      , SELECTION_INDEX_ELEMENT    = 1
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
    explicit BasicSchemaChoiceElementChoice(bslma::Allocator *basicAllocator = 0);
        // Create an object of type 'BasicSchemaChoiceElementChoice' having the
        // default value.  Use the optionally specified 'basicAllocator' to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.

    BasicSchemaChoiceElementChoice(const BasicSchemaChoiceElementChoice& original,
                                  bslma::Allocator *basicAllocator = 0);
        // Create an object of type 'BasicSchemaChoiceElementChoice' having the
        // value of the specified 'original' object.  Use the optionally
        // specified 'basicAllocator' to supply memory.  If 'basicAllocator' is
        // 0, the currently installed default allocator is used.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    BasicSchemaChoiceElementChoice(BasicSchemaChoiceElementChoice&& original) noexcept;
        // Create an object of type 'BasicSchemaChoiceElementChoice' having the
        // value of the specified 'original' object.  After performing this
        // action, the 'original' object will be left in a valid, but
        // unspecified state.

    BasicSchemaChoiceElementChoice(BasicSchemaChoiceElementChoice&& original,
                                  bslma::Allocator *basicAllocator);
        // Create an object of type 'BasicSchemaChoiceElementChoice' having the
        // value of the specified 'original' object.  After performing this
        // action, the 'original' object will be left in a valid, but
        // unspecified state.  Use the optionally specified 'basicAllocator' to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.
#endif

    ~BasicSchemaChoiceElementChoice();
        // Destroy this object.

    // MANIPULATORS
    BasicSchemaChoiceElementChoice& operator=(const BasicSchemaChoiceElementChoice& rhs);
        // Assign to this object the value of the specified 'rhs' object.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    BasicSchemaChoiceElementChoice& operator=(BasicSchemaChoiceElementChoice&& rhs);
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

    BasicSchemaAnnotationElement& makeAnnotation();
    BasicSchemaAnnotationElement& makeAnnotation(const BasicSchemaAnnotationElement& value);
#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    BasicSchemaAnnotationElement& makeAnnotation(BasicSchemaAnnotationElement&& value);
#endif
        // Set the value of this object to be a "Annotation" value.  Optionally
        // specify the 'value' of the "Annotation".  If 'value' is not
        // specified, the default "Annotation" value is used.

    BasicSchemaElement& makeElement();
    BasicSchemaElement& makeElement(const BasicSchemaElement& value);
#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    BasicSchemaElement& makeElement(BasicSchemaElement&& value);
#endif
        // Set the value of this object to be a "Element" value.  Optionally
        // specify the 'value' of the "Element".  If 'value' is not specified,
        // the default "Element" value is used.

    template<class MANIPULATOR>
    int manipulateSelection(MANIPULATOR& manipulator);
        // Invoke the specified 'manipulator' on the address of the modifiable
        // selection, supplying 'manipulator' with the corresponding selection
        // information structure.  Return the value returned from the
        // invocation of 'manipulator' if this object has a defined selection,
        // and -1 otherwise.

    BasicSchemaAnnotationElement& annotation();
        // Return a reference to the modifiable "Annotation" selection of this
        // object if "Annotation" is the current selection.  The behavior is
        // undefined unless "Annotation" is the selection of this object.

    BasicSchemaElement& element();
        // Return a reference to the modifiable "Element" selection of this
        // object if "Element" is the current selection.  The behavior is
        // undefined unless "Element" is the selection of this object.

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

    const BasicSchemaAnnotationElement& annotation() const;
        // Return a reference to the non-modifiable "Annotation" selection of
        // this object if "Annotation" is the current selection.  The behavior
        // is undefined unless "Annotation" is the selection of this object.

    const BasicSchemaElement& element() const;
        // Return a reference to the non-modifiable "Element" selection of this
        // object if "Element" is the current selection.  The behavior is
        // undefined unless "Element" is the selection of this object.

    bool isAnnotationValue() const;
        // Return 'true' if the value of this object is a "Annotation" value,
        // and return 'false' otherwise.

    bool isElementValue() const;
        // Return 'true' if the value of this object is a "Element" value, and
        // return 'false' otherwise.

    bool isUndefinedValue() const;
        // Return 'true' if the value of this object is undefined, and 'false'
        // otherwise.

    const char *selectionName() const;
        // Return the symbolic name of the current selection of this object.
};

// FREE OPERATORS
inline
bool operator==(const BasicSchemaChoiceElementChoice& lhs, const BasicSchemaChoiceElementChoice& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects have the same
    // value, and 'false' otherwise.  Two 'BasicSchemaChoiceElementChoice' objects have the same
    // value if either the selections in both objects have the same ids and
    // the same values, or both selections are undefined.

inline
bool operator!=(const BasicSchemaChoiceElementChoice& lhs, const BasicSchemaChoiceElementChoice& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects do not have the
    // same values, as determined by 'operator==', and 'false' otherwise.

inline
bsl::ostream& operator<<(bsl::ostream& stream, const BasicSchemaChoiceElementChoice& rhs);
    // Format the specified 'rhs' to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& hashAlg, const s_baltst::BasicSchemaChoiceElementChoice& object);
    // Pass the specified 'object' to the specified 'hashAlg'.  This function
    // integrates with the 'bslh' modular hashing system and effectively
    // provides a 'bsl::hash' specialization for 'BasicSchemaChoiceElementChoice'.

}  // close package namespace

// TRAITS

BDLAT_DECL_CHOICE_WITH_ALLOCATOR_BITWISEMOVEABLE_TRAITS(s_baltst::BasicSchemaChoiceElementChoice)

namespace s_baltst {

                    // ===================================
                    // class BasicSchemaComplexTypeElement
                    // ===================================

class BasicSchemaComplexTypeElement {

    // INSTANCE DATA
    bsl::vector<BasicSchemaComplexTypeElementChoice>  d_choice;
    bdlb::NullableValue<bsl::string>                  d_name;

  public:
    // TYPES
    enum {
        ATTRIBUTE_ID_NAME   = 0
      , ATTRIBUTE_ID_CHOICE = 1
    };

    enum {
        NUM_ATTRIBUTES = 2
    };

    enum {
        ATTRIBUTE_INDEX_NAME   = 0
      , ATTRIBUTE_INDEX_CHOICE = 1
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
    explicit BasicSchemaComplexTypeElement(bslma::Allocator *basicAllocator = 0);
        // Create an object of type 'BasicSchemaComplexTypeElement' having the
        // default value.  Use the optionally specified 'basicAllocator' to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.

    BasicSchemaComplexTypeElement(const BasicSchemaComplexTypeElement& original,
                                  bslma::Allocator *basicAllocator = 0);
        // Create an object of type 'BasicSchemaComplexTypeElement' having the
        // value of the specified 'original' object.  Use the optionally
        // specified 'basicAllocator' to supply memory.  If 'basicAllocator' is
        // 0, the currently installed default allocator is used.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    BasicSchemaComplexTypeElement(BasicSchemaComplexTypeElement&& original) noexcept;
        // Create an object of type 'BasicSchemaComplexTypeElement' having the
        // value of the specified 'original' object.  After performing this
        // action, the 'original' object will be left in a valid, but
        // unspecified state.

    BasicSchemaComplexTypeElement(BasicSchemaComplexTypeElement&& original,
                                  bslma::Allocator *basicAllocator);
        // Create an object of type 'BasicSchemaComplexTypeElement' having the
        // value of the specified 'original' object.  After performing this
        // action, the 'original' object will be left in a valid, but
        // unspecified state.  Use the optionally specified 'basicAllocator' to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.
#endif

    ~BasicSchemaComplexTypeElement();
        // Destroy this object.

    // MANIPULATORS
    BasicSchemaComplexTypeElement& operator=(const BasicSchemaComplexTypeElement& rhs);
        // Assign to this object the value of the specified 'rhs' object.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    BasicSchemaComplexTypeElement& operator=(BasicSchemaComplexTypeElement&& rhs);
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

    bdlb::NullableValue<bsl::string>& name();
        // Return a reference to the modifiable "Name" attribute of this
        // object.

    bsl::vector<BasicSchemaComplexTypeElementChoice>& choice();
        // Return a reference to the modifiable "Choice" attribute of this
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

    const bdlb::NullableValue<bsl::string>& name() const;
        // Return a reference to the non-modifiable "Name" attribute of this
        // object.

    const bsl::vector<BasicSchemaComplexTypeElementChoice>& choice() const;
        // Return a reference to the non-modifiable "Choice" attribute of this
        // object.
};

// FREE OPERATORS
inline
bool operator==(const BasicSchemaComplexTypeElement& lhs, const BasicSchemaComplexTypeElement& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
    // the same value, and 'false' otherwise.  Two attribute objects have the
    // same value if each respective attribute has the same value.

inline
bool operator!=(const BasicSchemaComplexTypeElement& lhs, const BasicSchemaComplexTypeElement& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
    // have the same value, and 'false' otherwise.  Two attribute objects do
    // not have the same value if one or more respective attributes differ in
    // values.

inline
bsl::ostream& operator<<(bsl::ostream& stream, const BasicSchemaComplexTypeElement& rhs);
    // Format the specified 'rhs' to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& hashAlg, const s_baltst::BasicSchemaComplexTypeElement& object);
    // Pass the specified 'object' to the specified 'hashAlg'.  This function
    // integrates with the 'bslh' modular hashing system and effectively
    // provides a 'bsl::hash' specialization for 'BasicSchemaComplexTypeElement'.

}  // close package namespace

// TRAITS

BDLAT_DECL_SEQUENCE_WITH_ALLOCATOR_BITWISEMOVEABLE_TRAITS(s_baltst::BasicSchemaComplexTypeElement)

namespace s_baltst {

                 // =========================================
                 // class BasicSchemaComplexTypeElementChoice
                 // =========================================

class BasicSchemaComplexTypeElementChoice {

    // INSTANCE DATA
    union {
        bsls::ObjectBuffer< BasicSchemaAnnotationElement >  d_annotation;
        BasicSchemaSequenceElement                        *d_sequence;
        BasicSchemaChoiceElement                          *d_choice;
    };

    int                                                     d_selectionId;
    bslma::Allocator                                       *d_allocator_p;

  public:
    // TYPES

    enum {
        SELECTION_ID_UNDEFINED  = -1
      , SELECTION_ID_ANNOTATION = 0
      , SELECTION_ID_SEQUENCE   = 1
      , SELECTION_ID_CHOICE     = 2
    };

    enum {
        NUM_SELECTIONS = 3
    };

    enum {
        SELECTION_INDEX_ANNOTATION = 0
      , SELECTION_INDEX_SEQUENCE   = 1
      , SELECTION_INDEX_CHOICE     = 2
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
    explicit BasicSchemaComplexTypeElementChoice(bslma::Allocator *basicAllocator = 0);
        // Create an object of type 'BasicSchemaComplexTypeElementChoice'
        // having the default value.  Use the optionally specified
        // 'basicAllocator' to supply memory.  If 'basicAllocator' is 0, the
        // currently installed default allocator is used.

    BasicSchemaComplexTypeElementChoice(const BasicSchemaComplexTypeElementChoice& original,
                                       bslma::Allocator *basicAllocator = 0);
        // Create an object of type 'BasicSchemaComplexTypeElementChoice'
        // having the value of the specified 'original' object.  Use the
        // optionally specified 'basicAllocator' to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    BasicSchemaComplexTypeElementChoice(BasicSchemaComplexTypeElementChoice&& original) noexcept;
        // Create an object of type 'BasicSchemaComplexTypeElementChoice'
        // having the value of the specified 'original' object.  After
        // performing this action, the 'original' object will be left in a
        // valid, but unspecified state.

    BasicSchemaComplexTypeElementChoice(BasicSchemaComplexTypeElementChoice&& original,
                                       bslma::Allocator *basicAllocator);
        // Create an object of type 'BasicSchemaComplexTypeElementChoice'
        // having the value of the specified 'original' object.  After
        // performing this action, the 'original' object will be left in a
        // valid, but unspecified state.  Use the optionally specified
        // 'basicAllocator' to supply memory.  If 'basicAllocator' is 0, the
        // currently installed default allocator is used.
#endif

    ~BasicSchemaComplexTypeElementChoice();
        // Destroy this object.

    // MANIPULATORS
    BasicSchemaComplexTypeElementChoice& operator=(const BasicSchemaComplexTypeElementChoice& rhs);
        // Assign to this object the value of the specified 'rhs' object.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    BasicSchemaComplexTypeElementChoice& operator=(BasicSchemaComplexTypeElementChoice&& rhs);
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

    BasicSchemaAnnotationElement& makeAnnotation();
    BasicSchemaAnnotationElement& makeAnnotation(const BasicSchemaAnnotationElement& value);
#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    BasicSchemaAnnotationElement& makeAnnotation(BasicSchemaAnnotationElement&& value);
#endif
        // Set the value of this object to be a "Annotation" value.  Optionally
        // specify the 'value' of the "Annotation".  If 'value' is not
        // specified, the default "Annotation" value is used.

    BasicSchemaSequenceElement& makeSequence();
    BasicSchemaSequenceElement& makeSequence(const BasicSchemaSequenceElement& value);
#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    BasicSchemaSequenceElement& makeSequence(BasicSchemaSequenceElement&& value);
#endif
        // Set the value of this object to be a "Sequence" value.  Optionally
        // specify the 'value' of the "Sequence".  If 'value' is not specified,
        // the default "Sequence" value is used.

    BasicSchemaChoiceElement& makeChoice();
    BasicSchemaChoiceElement& makeChoice(const BasicSchemaChoiceElement& value);
#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    BasicSchemaChoiceElement& makeChoice(BasicSchemaChoiceElement&& value);
#endif
        // Set the value of this object to be a "Choice" value.  Optionally
        // specify the 'value' of the "Choice".  If 'value' is not specified,
        // the default "Choice" value is used.

    template<class MANIPULATOR>
    int manipulateSelection(MANIPULATOR& manipulator);
        // Invoke the specified 'manipulator' on the address of the modifiable
        // selection, supplying 'manipulator' with the corresponding selection
        // information structure.  Return the value returned from the
        // invocation of 'manipulator' if this object has a defined selection,
        // and -1 otherwise.

    BasicSchemaAnnotationElement& annotation();
        // Return a reference to the modifiable "Annotation" selection of this
        // object if "Annotation" is the current selection.  The behavior is
        // undefined unless "Annotation" is the selection of this object.

    BasicSchemaSequenceElement& sequence();
        // Return a reference to the modifiable "Sequence" selection of this
        // object if "Sequence" is the current selection.  The behavior is
        // undefined unless "Sequence" is the selection of this object.

    BasicSchemaChoiceElement& choice();
        // Return a reference to the modifiable "Choice" selection of this
        // object if "Choice" is the current selection.  The behavior is
        // undefined unless "Choice" is the selection of this object.

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

    const BasicSchemaAnnotationElement& annotation() const;
        // Return a reference to the non-modifiable "Annotation" selection of
        // this object if "Annotation" is the current selection.  The behavior
        // is undefined unless "Annotation" is the selection of this object.

    const BasicSchemaSequenceElement& sequence() const;
        // Return a reference to the non-modifiable "Sequence" selection of
        // this object if "Sequence" is the current selection.  The behavior is
        // undefined unless "Sequence" is the selection of this object.

    const BasicSchemaChoiceElement& choice() const;
        // Return a reference to the non-modifiable "Choice" selection of this
        // object if "Choice" is the current selection.  The behavior is
        // undefined unless "Choice" is the selection of this object.

    bool isAnnotationValue() const;
        // Return 'true' if the value of this object is a "Annotation" value,
        // and return 'false' otherwise.

    bool isSequenceValue() const;
        // Return 'true' if the value of this object is a "Sequence" value, and
        // return 'false' otherwise.

    bool isChoiceValue() const;
        // Return 'true' if the value of this object is a "Choice" value, and
        // return 'false' otherwise.

    bool isUndefinedValue() const;
        // Return 'true' if the value of this object is undefined, and 'false'
        // otherwise.

    const char *selectionName() const;
        // Return the symbolic name of the current selection of this object.
};

// FREE OPERATORS
inline
bool operator==(const BasicSchemaComplexTypeElementChoice& lhs, const BasicSchemaComplexTypeElementChoice& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects have the same
    // value, and 'false' otherwise.  Two 'BasicSchemaComplexTypeElementChoice' objects have the same
    // value if either the selections in both objects have the same ids and
    // the same values, or both selections are undefined.

inline
bool operator!=(const BasicSchemaComplexTypeElementChoice& lhs, const BasicSchemaComplexTypeElementChoice& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects do not have the
    // same values, as determined by 'operator==', and 'false' otherwise.

inline
bsl::ostream& operator<<(bsl::ostream& stream, const BasicSchemaComplexTypeElementChoice& rhs);
    // Format the specified 'rhs' to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& hashAlg, const s_baltst::BasicSchemaComplexTypeElementChoice& object);
    // Pass the specified 'object' to the specified 'hashAlg'.  This function
    // integrates with the 'bslh' modular hashing system and effectively
    // provides a 'bsl::hash' specialization for 'BasicSchemaComplexTypeElementChoice'.

}  // close package namespace

// TRAITS

BDLAT_DECL_CHOICE_WITH_ALLOCATOR_BITWISEMOVEABLE_TRAITS(s_baltst::BasicSchemaComplexTypeElementChoice)

namespace s_baltst {

                          // ========================
                          // class BasicSchemaElement
                          // ========================

class BasicSchemaElement {

    // INSTANCE DATA
    bsl::vector<BasicSchemaElementChoice>  d_choice;
    bdlb::NullableValue<bsl::string>       d_name;
    bdlb::NullableValue<bsl::string>       d_type;
    bdlb::NullableValue<bsl::string>       d_minOccurs;
    bdlb::NullableValue<bsl::string>       d_maxOccurs;
    bdlb::NullableValue<bsl::string>       d_default;
    bdlb::NullableValue<bsl::string>       d_cppdefault;
    bdlb::NullableValue<bsl::string>       d_cpptype;
    bdlb::NullableValue<bsl::string>       d_allocatesMemory;
    bdlb::NullableValue<bsl::string>       d_allowsDirectManipulation;

  public:
    // TYPES
    enum {
        ATTRIBUTE_ID_NAME                       = 0
      , ATTRIBUTE_ID_TYPE                       = 1
      , ATTRIBUTE_ID_MIN_OCCURS                 = 2
      , ATTRIBUTE_ID_MAX_OCCURS                 = 3
      , ATTRIBUTE_ID_DEFAULT_VALUE              = 4
      , ATTRIBUTE_ID_CPPDEFAULT                 = 5
      , ATTRIBUTE_ID_CPPTYPE                    = 6
      , ATTRIBUTE_ID_ALLOCATES_MEMORY           = 7
      , ATTRIBUTE_ID_ALLOWS_DIRECT_MANIPULATION = 8
      , ATTRIBUTE_ID_CHOICE                     = 9
    };

    enum {
        NUM_ATTRIBUTES = 10
    };

    enum {
        ATTRIBUTE_INDEX_NAME                       = 0
      , ATTRIBUTE_INDEX_TYPE                       = 1
      , ATTRIBUTE_INDEX_MIN_OCCURS                 = 2
      , ATTRIBUTE_INDEX_MAX_OCCURS                 = 3
      , ATTRIBUTE_INDEX_DEFAULT_VALUE              = 4
      , ATTRIBUTE_INDEX_CPPDEFAULT                 = 5
      , ATTRIBUTE_INDEX_CPPTYPE                    = 6
      , ATTRIBUTE_INDEX_ALLOCATES_MEMORY           = 7
      , ATTRIBUTE_INDEX_ALLOWS_DIRECT_MANIPULATION = 8
      , ATTRIBUTE_INDEX_CHOICE                     = 9
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
    explicit BasicSchemaElement(bslma::Allocator *basicAllocator = 0);
        // Create an object of type 'BasicSchemaElement' having the default
        // value.  Use the optionally specified 'basicAllocator' to supply
        // memory.  If 'basicAllocator' is 0, the currently installed default
        // allocator is used.

    BasicSchemaElement(const BasicSchemaElement& original,
                       bslma::Allocator *basicAllocator = 0);
        // Create an object of type 'BasicSchemaElement' having the value of
        // the specified 'original' object.  Use the optionally specified
        // 'basicAllocator' to supply memory.  If 'basicAllocator' is 0, the
        // currently installed default allocator is used.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    BasicSchemaElement(BasicSchemaElement&& original) noexcept;
        // Create an object of type 'BasicSchemaElement' having the value of
        // the specified 'original' object.  After performing this action, the
        // 'original' object will be left in a valid, but unspecified state.

    BasicSchemaElement(BasicSchemaElement&& original,
                       bslma::Allocator *basicAllocator);
        // Create an object of type 'BasicSchemaElement' having the value of
        // the specified 'original' object.  After performing this action, the
        // 'original' object will be left in a valid, but unspecified state.
        // Use the optionally specified 'basicAllocator' to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.
#endif

    ~BasicSchemaElement();
        // Destroy this object.

    // MANIPULATORS
    BasicSchemaElement& operator=(const BasicSchemaElement& rhs);
        // Assign to this object the value of the specified 'rhs' object.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    BasicSchemaElement& operator=(BasicSchemaElement&& rhs);
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

    bdlb::NullableValue<bsl::string>& name();
        // Return a reference to the modifiable "Name" attribute of this
        // object.

    bdlb::NullableValue<bsl::string>& type();
        // Return a reference to the modifiable "Type" attribute of this
        // object.

    bdlb::NullableValue<bsl::string>& minOccurs();
        // Return a reference to the modifiable "MinOccurs" attribute of this
        // object.

    bdlb::NullableValue<bsl::string>& maxOccurs();
        // Return a reference to the modifiable "MaxOccurs" attribute of this
        // object.

    bdlb::NullableValue<bsl::string>& defaultValue();
        // Return a reference to the modifiable "DefaultValue" attribute of
        // this object.

    bdlb::NullableValue<bsl::string>& cppdefault();
        // Return a reference to the modifiable "Cppdefault" attribute of this
        // object.

    bdlb::NullableValue<bsl::string>& cpptype();
        // Return a reference to the modifiable "Cpptype" attribute of this
        // object.

    bdlb::NullableValue<bsl::string>& allocatesMemory();
        // Return a reference to the modifiable "AllocatesMemory" attribute of
        // this object.

    bdlb::NullableValue<bsl::string>& allowsDirectManipulation();
        // Return a reference to the modifiable "AllowsDirectManipulation"
        // attribute of this object.

    bsl::vector<BasicSchemaElementChoice>& choice();
        // Return a reference to the modifiable "Choice" attribute of this
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

    const bdlb::NullableValue<bsl::string>& name() const;
        // Return a reference to the non-modifiable "Name" attribute of this
        // object.

    const bdlb::NullableValue<bsl::string>& type() const;
        // Return a reference to the non-modifiable "Type" attribute of this
        // object.

    const bdlb::NullableValue<bsl::string>& minOccurs() const;
        // Return a reference to the non-modifiable "MinOccurs" attribute of
        // this object.

    const bdlb::NullableValue<bsl::string>& maxOccurs() const;
        // Return a reference to the non-modifiable "MaxOccurs" attribute of
        // this object.

    const bdlb::NullableValue<bsl::string>& defaultValue() const;
        // Return a reference to the non-modifiable "DefaultValue" attribute of
        // this object.

    const bdlb::NullableValue<bsl::string>& cppdefault() const;
        // Return a reference to the non-modifiable "Cppdefault" attribute of
        // this object.

    const bdlb::NullableValue<bsl::string>& cpptype() const;
        // Return a reference to the non-modifiable "Cpptype" attribute of this
        // object.

    const bdlb::NullableValue<bsl::string>& allocatesMemory() const;
        // Return a reference to the non-modifiable "AllocatesMemory" attribute
        // of this object.

    const bdlb::NullableValue<bsl::string>& allowsDirectManipulation() const;
        // Return a reference to the non-modifiable "AllowsDirectManipulation"
        // attribute of this object.

    const bsl::vector<BasicSchemaElementChoice>& choice() const;
        // Return a reference to the non-modifiable "Choice" attribute of this
        // object.
};

// FREE OPERATORS
inline
bool operator==(const BasicSchemaElement& lhs, const BasicSchemaElement& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
    // the same value, and 'false' otherwise.  Two attribute objects have the
    // same value if each respective attribute has the same value.

inline
bool operator!=(const BasicSchemaElement& lhs, const BasicSchemaElement& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
    // have the same value, and 'false' otherwise.  Two attribute objects do
    // not have the same value if one or more respective attributes differ in
    // values.

inline
bsl::ostream& operator<<(bsl::ostream& stream, const BasicSchemaElement& rhs);
    // Format the specified 'rhs' to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& hashAlg, const s_baltst::BasicSchemaElement& object);
    // Pass the specified 'object' to the specified 'hashAlg'.  This function
    // integrates with the 'bslh' modular hashing system and effectively
    // provides a 'bsl::hash' specialization for 'BasicSchemaElement'.

}  // close package namespace

// TRAITS

BDLAT_DECL_SEQUENCE_WITH_ALLOCATOR_BITWISEMOVEABLE_TRAITS(s_baltst::BasicSchemaElement)

namespace s_baltst {

                       // ==============================
                       // class BasicSchemaElementChoice
                       // ==============================

class BasicSchemaElementChoice {

    // INSTANCE DATA
    union {
        bsls::ObjectBuffer< BasicSchemaAnnotationElement >  d_annotation;
        bsls::ObjectBuffer< BasicSchemaSimpleTypeElement >  d_simpleType;
        BasicSchemaComplexTypeElement                     *d_complexType;
    };

    int                                                     d_selectionId;
    bslma::Allocator                                       *d_allocator_p;

  public:
    // TYPES

    enum {
        SELECTION_ID_UNDEFINED    = -1
      , SELECTION_ID_ANNOTATION   = 0
      , SELECTION_ID_SIMPLE_TYPE  = 1
      , SELECTION_ID_COMPLEX_TYPE = 2
    };

    enum {
        NUM_SELECTIONS = 3
    };

    enum {
        SELECTION_INDEX_ANNOTATION   = 0
      , SELECTION_INDEX_SIMPLE_TYPE  = 1
      , SELECTION_INDEX_COMPLEX_TYPE = 2
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
    explicit BasicSchemaElementChoice(bslma::Allocator *basicAllocator = 0);
        // Create an object of type 'BasicSchemaElementChoice' having the
        // default value.  Use the optionally specified 'basicAllocator' to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.

    BasicSchemaElementChoice(const BasicSchemaElementChoice& original,
                            bslma::Allocator *basicAllocator = 0);
        // Create an object of type 'BasicSchemaElementChoice' having the value
        // of the specified 'original' object.  Use the optionally specified
        // 'basicAllocator' to supply memory.  If 'basicAllocator' is 0, the
        // currently installed default allocator is used.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    BasicSchemaElementChoice(BasicSchemaElementChoice&& original) noexcept;
        // Create an object of type 'BasicSchemaElementChoice' having the value
        // of the specified 'original' object.  After performing this action,
        // the 'original' object will be left in a valid, but unspecified
        // state.

    BasicSchemaElementChoice(BasicSchemaElementChoice&& original,
                            bslma::Allocator *basicAllocator);
        // Create an object of type 'BasicSchemaElementChoice' having the value
        // of the specified 'original' object.  After performing this action,
        // the 'original' object will be left in a valid, but unspecified
        // state.  Use the optionally specified 'basicAllocator' to supply
        // memory.  If 'basicAllocator' is 0, the currently installed default
        // allocator is used.
#endif

    ~BasicSchemaElementChoice();
        // Destroy this object.

    // MANIPULATORS
    BasicSchemaElementChoice& operator=(const BasicSchemaElementChoice& rhs);
        // Assign to this object the value of the specified 'rhs' object.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    BasicSchemaElementChoice& operator=(BasicSchemaElementChoice&& rhs);
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

    BasicSchemaAnnotationElement& makeAnnotation();
    BasicSchemaAnnotationElement& makeAnnotation(const BasicSchemaAnnotationElement& value);
#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    BasicSchemaAnnotationElement& makeAnnotation(BasicSchemaAnnotationElement&& value);
#endif
        // Set the value of this object to be a "Annotation" value.  Optionally
        // specify the 'value' of the "Annotation".  If 'value' is not
        // specified, the default "Annotation" value is used.

    BasicSchemaSimpleTypeElement& makeSimpleType();
    BasicSchemaSimpleTypeElement& makeSimpleType(const BasicSchemaSimpleTypeElement& value);
#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    BasicSchemaSimpleTypeElement& makeSimpleType(BasicSchemaSimpleTypeElement&& value);
#endif
        // Set the value of this object to be a "SimpleType" value.  Optionally
        // specify the 'value' of the "SimpleType".  If 'value' is not
        // specified, the default "SimpleType" value is used.

    BasicSchemaComplexTypeElement& makeComplexType();
    BasicSchemaComplexTypeElement& makeComplexType(const BasicSchemaComplexTypeElement& value);
#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    BasicSchemaComplexTypeElement& makeComplexType(BasicSchemaComplexTypeElement&& value);
#endif
        // Set the value of this object to be a "ComplexType" value.
        // Optionally specify the 'value' of the "ComplexType".  If 'value' is
        // not specified, the default "ComplexType" value is used.

    template<class MANIPULATOR>
    int manipulateSelection(MANIPULATOR& manipulator);
        // Invoke the specified 'manipulator' on the address of the modifiable
        // selection, supplying 'manipulator' with the corresponding selection
        // information structure.  Return the value returned from the
        // invocation of 'manipulator' if this object has a defined selection,
        // and -1 otherwise.

    BasicSchemaAnnotationElement& annotation();
        // Return a reference to the modifiable "Annotation" selection of this
        // object if "Annotation" is the current selection.  The behavior is
        // undefined unless "Annotation" is the selection of this object.

    BasicSchemaSimpleTypeElement& simpleType();
        // Return a reference to the modifiable "SimpleType" selection of this
        // object if "SimpleType" is the current selection.  The behavior is
        // undefined unless "SimpleType" is the selection of this object.

    BasicSchemaComplexTypeElement& complexType();
        // Return a reference to the modifiable "ComplexType" selection of this
        // object if "ComplexType" is the current selection.  The behavior is
        // undefined unless "ComplexType" is the selection of this object.

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

    const BasicSchemaAnnotationElement& annotation() const;
        // Return a reference to the non-modifiable "Annotation" selection of
        // this object if "Annotation" is the current selection.  The behavior
        // is undefined unless "Annotation" is the selection of this object.

    const BasicSchemaSimpleTypeElement& simpleType() const;
        // Return a reference to the non-modifiable "SimpleType" selection of
        // this object if "SimpleType" is the current selection.  The behavior
        // is undefined unless "SimpleType" is the selection of this object.

    const BasicSchemaComplexTypeElement& complexType() const;
        // Return a reference to the non-modifiable "ComplexType" selection of
        // this object if "ComplexType" is the current selection.  The behavior
        // is undefined unless "ComplexType" is the selection of this object.

    bool isAnnotationValue() const;
        // Return 'true' if the value of this object is a "Annotation" value,
        // and return 'false' otherwise.

    bool isSimpleTypeValue() const;
        // Return 'true' if the value of this object is a "SimpleType" value,
        // and return 'false' otherwise.

    bool isComplexTypeValue() const;
        // Return 'true' if the value of this object is a "ComplexType" value,
        // and return 'false' otherwise.

    bool isUndefinedValue() const;
        // Return 'true' if the value of this object is undefined, and 'false'
        // otherwise.

    const char *selectionName() const;
        // Return the symbolic name of the current selection of this object.
};

// FREE OPERATORS
inline
bool operator==(const BasicSchemaElementChoice& lhs, const BasicSchemaElementChoice& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects have the same
    // value, and 'false' otherwise.  Two 'BasicSchemaElementChoice' objects have the same
    // value if either the selections in both objects have the same ids and
    // the same values, or both selections are undefined.

inline
bool operator!=(const BasicSchemaElementChoice& lhs, const BasicSchemaElementChoice& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects do not have the
    // same values, as determined by 'operator==', and 'false' otherwise.

inline
bsl::ostream& operator<<(bsl::ostream& stream, const BasicSchemaElementChoice& rhs);
    // Format the specified 'rhs' to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& hashAlg, const s_baltst::BasicSchemaElementChoice& object);
    // Pass the specified 'object' to the specified 'hashAlg'.  This function
    // integrates with the 'bslh' modular hashing system and effectively
    // provides a 'bsl::hash' specialization for 'BasicSchemaElementChoice'.

}  // close package namespace

// TRAITS

BDLAT_DECL_CHOICE_WITH_ALLOCATOR_BITWISEMOVEABLE_TRAITS(s_baltst::BasicSchemaElementChoice)

namespace s_baltst {

                      // ================================
                      // class BasicSchemaSequenceElement
                      // ================================

class BasicSchemaSequenceElement {

    // INSTANCE DATA
    bsl::vector<BasicSchemaSequenceElementChoice>  d_choice;

  public:
    // TYPES
    enum {
        ATTRIBUTE_ID_CHOICE = 0
    };

    enum {
        NUM_ATTRIBUTES = 1
    };

    enum {
        ATTRIBUTE_INDEX_CHOICE = 0
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
    explicit BasicSchemaSequenceElement(bslma::Allocator *basicAllocator = 0);
        // Create an object of type 'BasicSchemaSequenceElement' having the
        // default value.  Use the optionally specified 'basicAllocator' to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.

    BasicSchemaSequenceElement(const BasicSchemaSequenceElement& original,
                               bslma::Allocator *basicAllocator = 0);
        // Create an object of type 'BasicSchemaSequenceElement' having the
        // value of the specified 'original' object.  Use the optionally
        // specified 'basicAllocator' to supply memory.  If 'basicAllocator' is
        // 0, the currently installed default allocator is used.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    BasicSchemaSequenceElement(BasicSchemaSequenceElement&& original) noexcept;
        // Create an object of type 'BasicSchemaSequenceElement' having the
        // value of the specified 'original' object.  After performing this
        // action, the 'original' object will be left in a valid, but
        // unspecified state.

    BasicSchemaSequenceElement(BasicSchemaSequenceElement&& original,
                               bslma::Allocator *basicAllocator);
        // Create an object of type 'BasicSchemaSequenceElement' having the
        // value of the specified 'original' object.  After performing this
        // action, the 'original' object will be left in a valid, but
        // unspecified state.  Use the optionally specified 'basicAllocator' to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.
#endif

    ~BasicSchemaSequenceElement();
        // Destroy this object.

    // MANIPULATORS
    BasicSchemaSequenceElement& operator=(const BasicSchemaSequenceElement& rhs);
        // Assign to this object the value of the specified 'rhs' object.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    BasicSchemaSequenceElement& operator=(BasicSchemaSequenceElement&& rhs);
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

    bsl::vector<BasicSchemaSequenceElementChoice>& choice();
        // Return a reference to the modifiable "Choice" attribute of this
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

    const bsl::vector<BasicSchemaSequenceElementChoice>& choice() const;
        // Return a reference to the non-modifiable "Choice" attribute of this
        // object.
};

// FREE OPERATORS
inline
bool operator==(const BasicSchemaSequenceElement& lhs, const BasicSchemaSequenceElement& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
    // the same value, and 'false' otherwise.  Two attribute objects have the
    // same value if each respective attribute has the same value.

inline
bool operator!=(const BasicSchemaSequenceElement& lhs, const BasicSchemaSequenceElement& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
    // have the same value, and 'false' otherwise.  Two attribute objects do
    // not have the same value if one or more respective attributes differ in
    // values.

inline
bsl::ostream& operator<<(bsl::ostream& stream, const BasicSchemaSequenceElement& rhs);
    // Format the specified 'rhs' to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& hashAlg, const s_baltst::BasicSchemaSequenceElement& object);
    // Pass the specified 'object' to the specified 'hashAlg'.  This function
    // integrates with the 'bslh' modular hashing system and effectively
    // provides a 'bsl::hash' specialization for 'BasicSchemaSequenceElement'.

}  // close package namespace

// TRAITS

BDLAT_DECL_SEQUENCE_WITH_ALLOCATOR_BITWISEMOVEABLE_TRAITS(s_baltst::BasicSchemaSequenceElement)

namespace s_baltst {

                   // ======================================
                   // class BasicSchemaSequenceElementChoice
                   // ======================================

class BasicSchemaSequenceElementChoice {

    // INSTANCE DATA
    union {
        bsls::ObjectBuffer< BasicSchemaAnnotationElement >  d_annotation;
        BasicSchemaElement                                *d_element;
    };

    int                                                     d_selectionId;
    bslma::Allocator                                       *d_allocator_p;

  public:
    // TYPES

    enum {
        SELECTION_ID_UNDEFINED  = -1
      , SELECTION_ID_ANNOTATION = 0
      , SELECTION_ID_ELEMENT    = 1
    };

    enum {
        NUM_SELECTIONS = 2
    };

    enum {
        SELECTION_INDEX_ANNOTATION = 0
      , SELECTION_INDEX_ELEMENT    = 1
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
    explicit BasicSchemaSequenceElementChoice(bslma::Allocator *basicAllocator = 0);
        // Create an object of type 'BasicSchemaSequenceElementChoice' having
        // the default value.  Use the optionally specified 'basicAllocator' to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.

    BasicSchemaSequenceElementChoice(const BasicSchemaSequenceElementChoice& original,
                                    bslma::Allocator *basicAllocator = 0);
        // Create an object of type 'BasicSchemaSequenceElementChoice' having
        // the value of the specified 'original' object.  Use the optionally
        // specified 'basicAllocator' to supply memory.  If 'basicAllocator' is
        // 0, the currently installed default allocator is used.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    BasicSchemaSequenceElementChoice(BasicSchemaSequenceElementChoice&& original) noexcept;
        // Create an object of type 'BasicSchemaSequenceElementChoice' having
        // the value of the specified 'original' object.  After performing this
        // action, the 'original' object will be left in a valid, but
        // unspecified state.

    BasicSchemaSequenceElementChoice(BasicSchemaSequenceElementChoice&& original,
                                    bslma::Allocator *basicAllocator);
        // Create an object of type 'BasicSchemaSequenceElementChoice' having
        // the value of the specified 'original' object.  After performing this
        // action, the 'original' object will be left in a valid, but
        // unspecified state.  Use the optionally specified 'basicAllocator' to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.
#endif

    ~BasicSchemaSequenceElementChoice();
        // Destroy this object.

    // MANIPULATORS
    BasicSchemaSequenceElementChoice& operator=(const BasicSchemaSequenceElementChoice& rhs);
        // Assign to this object the value of the specified 'rhs' object.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    BasicSchemaSequenceElementChoice& operator=(BasicSchemaSequenceElementChoice&& rhs);
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

    BasicSchemaAnnotationElement& makeAnnotation();
    BasicSchemaAnnotationElement& makeAnnotation(const BasicSchemaAnnotationElement& value);
#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    BasicSchemaAnnotationElement& makeAnnotation(BasicSchemaAnnotationElement&& value);
#endif
        // Set the value of this object to be a "Annotation" value.  Optionally
        // specify the 'value' of the "Annotation".  If 'value' is not
        // specified, the default "Annotation" value is used.

    BasicSchemaElement& makeElement();
    BasicSchemaElement& makeElement(const BasicSchemaElement& value);
#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    BasicSchemaElement& makeElement(BasicSchemaElement&& value);
#endif
        // Set the value of this object to be a "Element" value.  Optionally
        // specify the 'value' of the "Element".  If 'value' is not specified,
        // the default "Element" value is used.

    template<class MANIPULATOR>
    int manipulateSelection(MANIPULATOR& manipulator);
        // Invoke the specified 'manipulator' on the address of the modifiable
        // selection, supplying 'manipulator' with the corresponding selection
        // information structure.  Return the value returned from the
        // invocation of 'manipulator' if this object has a defined selection,
        // and -1 otherwise.

    BasicSchemaAnnotationElement& annotation();
        // Return a reference to the modifiable "Annotation" selection of this
        // object if "Annotation" is the current selection.  The behavior is
        // undefined unless "Annotation" is the selection of this object.

    BasicSchemaElement& element();
        // Return a reference to the modifiable "Element" selection of this
        // object if "Element" is the current selection.  The behavior is
        // undefined unless "Element" is the selection of this object.

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

    const BasicSchemaAnnotationElement& annotation() const;
        // Return a reference to the non-modifiable "Annotation" selection of
        // this object if "Annotation" is the current selection.  The behavior
        // is undefined unless "Annotation" is the selection of this object.

    const BasicSchemaElement& element() const;
        // Return a reference to the non-modifiable "Element" selection of this
        // object if "Element" is the current selection.  The behavior is
        // undefined unless "Element" is the selection of this object.

    bool isAnnotationValue() const;
        // Return 'true' if the value of this object is a "Annotation" value,
        // and return 'false' otherwise.

    bool isElementValue() const;
        // Return 'true' if the value of this object is a "Element" value, and
        // return 'false' otherwise.

    bool isUndefinedValue() const;
        // Return 'true' if the value of this object is undefined, and 'false'
        // otherwise.

    const char *selectionName() const;
        // Return the symbolic name of the current selection of this object.
};

// FREE OPERATORS
inline
bool operator==(const BasicSchemaSequenceElementChoice& lhs, const BasicSchemaSequenceElementChoice& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects have the same
    // value, and 'false' otherwise.  Two 'BasicSchemaSequenceElementChoice' objects have the same
    // value if either the selections in both objects have the same ids and
    // the same values, or both selections are undefined.

inline
bool operator!=(const BasicSchemaSequenceElementChoice& lhs, const BasicSchemaSequenceElementChoice& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects do not have the
    // same values, as determined by 'operator==', and 'false' otherwise.

inline
bsl::ostream& operator<<(bsl::ostream& stream, const BasicSchemaSequenceElementChoice& rhs);
    // Format the specified 'rhs' to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& hashAlg, const s_baltst::BasicSchemaSequenceElementChoice& object);
    // Pass the specified 'object' to the specified 'hashAlg'.  This function
    // integrates with the 'bslh' modular hashing system and effectively
    // provides a 'bsl::hash' specialization for 'BasicSchemaSequenceElementChoice'.

}  // close package namespace

// TRAITS

BDLAT_DECL_CHOICE_WITH_ALLOCATOR_BITWISEMOVEABLE_TRAITS(s_baltst::BasicSchemaSequenceElementChoice)

namespace s_baltst {

                          // =======================
                          // class BasicSchemaChoice
                          // =======================

class BasicSchemaChoice {

    // INSTANCE DATA
    union {
        bsls::ObjectBuffer< BasicSchemaAnnotationElement >  d_annotation;
        bsls::ObjectBuffer< BasicSchemaSimpleTypeElement >  d_simpleType;
        BasicSchemaComplexTypeElement                     *d_complexType;
    };

    int                                                     d_selectionId;
    bslma::Allocator                                       *d_allocator_p;

  public:
    // TYPES

    enum {
        SELECTION_ID_UNDEFINED    = -1
      , SELECTION_ID_ANNOTATION   = 0
      , SELECTION_ID_SIMPLE_TYPE  = 1
      , SELECTION_ID_COMPLEX_TYPE = 2
    };

    enum {
        NUM_SELECTIONS = 3
    };

    enum {
        SELECTION_INDEX_ANNOTATION   = 0
      , SELECTION_INDEX_SIMPLE_TYPE  = 1
      , SELECTION_INDEX_COMPLEX_TYPE = 2
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
    explicit BasicSchemaChoice(bslma::Allocator *basicAllocator = 0);
        // Create an object of type 'BasicSchemaChoice' having the default
        // value.  Use the optionally specified 'basicAllocator' to supply
        // memory.  If 'basicAllocator' is 0, the currently installed default
        // allocator is used.

    BasicSchemaChoice(const BasicSchemaChoice& original,
                     bslma::Allocator *basicAllocator = 0);
        // Create an object of type 'BasicSchemaChoice' having the value of the
        // specified 'original' object.  Use the optionally specified
        // 'basicAllocator' to supply memory.  If 'basicAllocator' is 0, the
        // currently installed default allocator is used.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    BasicSchemaChoice(BasicSchemaChoice&& original) noexcept;
        // Create an object of type 'BasicSchemaChoice' having the value of the
        // specified 'original' object.  After performing this action, the
        // 'original' object will be left in a valid, but unspecified state.

    BasicSchemaChoice(BasicSchemaChoice&& original,
                     bslma::Allocator *basicAllocator);
        // Create an object of type 'BasicSchemaChoice' having the value of the
        // specified 'original' object.  After performing this action, the
        // 'original' object will be left in a valid, but unspecified state.
        // Use the optionally specified 'basicAllocator' to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.
#endif

    ~BasicSchemaChoice();
        // Destroy this object.

    // MANIPULATORS
    BasicSchemaChoice& operator=(const BasicSchemaChoice& rhs);
        // Assign to this object the value of the specified 'rhs' object.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    BasicSchemaChoice& operator=(BasicSchemaChoice&& rhs);
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

    BasicSchemaAnnotationElement& makeAnnotation();
    BasicSchemaAnnotationElement& makeAnnotation(const BasicSchemaAnnotationElement& value);
#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    BasicSchemaAnnotationElement& makeAnnotation(BasicSchemaAnnotationElement&& value);
#endif
        // Set the value of this object to be a "Annotation" value.  Optionally
        // specify the 'value' of the "Annotation".  If 'value' is not
        // specified, the default "Annotation" value is used.

    BasicSchemaSimpleTypeElement& makeSimpleType();
    BasicSchemaSimpleTypeElement& makeSimpleType(const BasicSchemaSimpleTypeElement& value);
#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    BasicSchemaSimpleTypeElement& makeSimpleType(BasicSchemaSimpleTypeElement&& value);
#endif
        // Set the value of this object to be a "SimpleType" value.  Optionally
        // specify the 'value' of the "SimpleType".  If 'value' is not
        // specified, the default "SimpleType" value is used.

    BasicSchemaComplexTypeElement& makeComplexType();
    BasicSchemaComplexTypeElement& makeComplexType(const BasicSchemaComplexTypeElement& value);
#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    BasicSchemaComplexTypeElement& makeComplexType(BasicSchemaComplexTypeElement&& value);
#endif
        // Set the value of this object to be a "ComplexType" value.
        // Optionally specify the 'value' of the "ComplexType".  If 'value' is
        // not specified, the default "ComplexType" value is used.

    template<class MANIPULATOR>
    int manipulateSelection(MANIPULATOR& manipulator);
        // Invoke the specified 'manipulator' on the address of the modifiable
        // selection, supplying 'manipulator' with the corresponding selection
        // information structure.  Return the value returned from the
        // invocation of 'manipulator' if this object has a defined selection,
        // and -1 otherwise.

    BasicSchemaAnnotationElement& annotation();
        // Return a reference to the modifiable "Annotation" selection of this
        // object if "Annotation" is the current selection.  The behavior is
        // undefined unless "Annotation" is the selection of this object.

    BasicSchemaSimpleTypeElement& simpleType();
        // Return a reference to the modifiable "SimpleType" selection of this
        // object if "SimpleType" is the current selection.  The behavior is
        // undefined unless "SimpleType" is the selection of this object.

    BasicSchemaComplexTypeElement& complexType();
        // Return a reference to the modifiable "ComplexType" selection of this
        // object if "ComplexType" is the current selection.  The behavior is
        // undefined unless "ComplexType" is the selection of this object.

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

    const BasicSchemaAnnotationElement& annotation() const;
        // Return a reference to the non-modifiable "Annotation" selection of
        // this object if "Annotation" is the current selection.  The behavior
        // is undefined unless "Annotation" is the selection of this object.

    const BasicSchemaSimpleTypeElement& simpleType() const;
        // Return a reference to the non-modifiable "SimpleType" selection of
        // this object if "SimpleType" is the current selection.  The behavior
        // is undefined unless "SimpleType" is the selection of this object.

    const BasicSchemaComplexTypeElement& complexType() const;
        // Return a reference to the non-modifiable "ComplexType" selection of
        // this object if "ComplexType" is the current selection.  The behavior
        // is undefined unless "ComplexType" is the selection of this object.

    bool isAnnotationValue() const;
        // Return 'true' if the value of this object is a "Annotation" value,
        // and return 'false' otherwise.

    bool isSimpleTypeValue() const;
        // Return 'true' if the value of this object is a "SimpleType" value,
        // and return 'false' otherwise.

    bool isComplexTypeValue() const;
        // Return 'true' if the value of this object is a "ComplexType" value,
        // and return 'false' otherwise.

    bool isUndefinedValue() const;
        // Return 'true' if the value of this object is undefined, and 'false'
        // otherwise.

    const char *selectionName() const;
        // Return the symbolic name of the current selection of this object.
};

// FREE OPERATORS
inline
bool operator==(const BasicSchemaChoice& lhs, const BasicSchemaChoice& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects have the same
    // value, and 'false' otherwise.  Two 'BasicSchemaChoice' objects have the same
    // value if either the selections in both objects have the same ids and
    // the same values, or both selections are undefined.

inline
bool operator!=(const BasicSchemaChoice& lhs, const BasicSchemaChoice& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects do not have the
    // same values, as determined by 'operator==', and 'false' otherwise.

inline
bsl::ostream& operator<<(bsl::ostream& stream, const BasicSchemaChoice& rhs);
    // Format the specified 'rhs' to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& hashAlg, const s_baltst::BasicSchemaChoice& object);
    // Pass the specified 'object' to the specified 'hashAlg'.  This function
    // integrates with the 'bslh' modular hashing system and effectively
    // provides a 'bsl::hash' specialization for 'BasicSchemaChoice'.

}  // close package namespace

// TRAITS

BDLAT_DECL_CHOICE_WITH_ALLOCATOR_BITWISEMOVEABLE_TRAITS(s_baltst::BasicSchemaChoice)

namespace s_baltst {

                             // =================
                             // class BasicSchema
                             // =================

class BasicSchema {

    // INSTANCE DATA
    bsl::vector<BasicSchemaChoice>    d_choice;
    bdlb::NullableValue<bsl::string>  d_xs;
    bdlb::NullableValue<bsl::string>  d_bdem;
    bdlb::NullableValue<bsl::string>  d_package;
    bdlb::NullableValue<bsl::string>  d_elementFormDefault;

  public:
    // TYPES
    enum {
        ATTRIBUTE_ID_XS                   = 0
      , ATTRIBUTE_ID_BDEM                 = 1
      , ATTRIBUTE_ID_PACKAGE              = 2
      , ATTRIBUTE_ID_ELEMENT_FORM_DEFAULT = 3
      , ATTRIBUTE_ID_CHOICE               = 4
    };

    enum {
        NUM_ATTRIBUTES = 5
    };

    enum {
        ATTRIBUTE_INDEX_XS                   = 0
      , ATTRIBUTE_INDEX_BDEM                 = 1
      , ATTRIBUTE_INDEX_PACKAGE              = 2
      , ATTRIBUTE_INDEX_ELEMENT_FORM_DEFAULT = 3
      , ATTRIBUTE_INDEX_CHOICE               = 4
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
    explicit BasicSchema(bslma::Allocator *basicAllocator = 0);
        // Create an object of type 'BasicSchema' having the default value.
        // Use the optionally specified 'basicAllocator' to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.

    BasicSchema(const BasicSchema& original,
                bslma::Allocator *basicAllocator = 0);
        // Create an object of type 'BasicSchema' having the value of the
        // specified 'original' object.  Use the optionally specified
        // 'basicAllocator' to supply memory.  If 'basicAllocator' is 0, the
        // currently installed default allocator is used.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    BasicSchema(BasicSchema&& original) noexcept;
        // Create an object of type 'BasicSchema' having the value of the
        // specified 'original' object.  After performing this action, the
        // 'original' object will be left in a valid, but unspecified state.

    BasicSchema(BasicSchema&& original,
                bslma::Allocator *basicAllocator);
        // Create an object of type 'BasicSchema' having the value of the
        // specified 'original' object.  After performing this action, the
        // 'original' object will be left in a valid, but unspecified state.
        // Use the optionally specified 'basicAllocator' to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.
#endif

    ~BasicSchema();
        // Destroy this object.

    // MANIPULATORS
    BasicSchema& operator=(const BasicSchema& rhs);
        // Assign to this object the value of the specified 'rhs' object.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    BasicSchema& operator=(BasicSchema&& rhs);
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

    bdlb::NullableValue<bsl::string>& xs();
        // Return a reference to the modifiable "Xs" attribute of this object.

    bdlb::NullableValue<bsl::string>& bdem();
        // Return a reference to the modifiable "Bdem" attribute of this
        // object.

    bdlb::NullableValue<bsl::string>& package();
        // Return a reference to the modifiable "Package" attribute of this
        // object.

    bdlb::NullableValue<bsl::string>& elementFormDefault();
        // Return a reference to the modifiable "ElementFormDefault" attribute
        // of this object.

    bsl::vector<BasicSchemaChoice>& choice();
        // Return a reference to the modifiable "Choice" attribute of this
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

    const bdlb::NullableValue<bsl::string>& xs() const;
        // Return a reference to the non-modifiable "Xs" attribute of this
        // object.

    const bdlb::NullableValue<bsl::string>& bdem() const;
        // Return a reference to the non-modifiable "Bdem" attribute of this
        // object.

    const bdlb::NullableValue<bsl::string>& package() const;
        // Return a reference to the non-modifiable "Package" attribute of this
        // object.

    const bdlb::NullableValue<bsl::string>& elementFormDefault() const;
        // Return a reference to the non-modifiable "ElementFormDefault"
        // attribute of this object.

    const bsl::vector<BasicSchemaChoice>& choice() const;
        // Return a reference to the non-modifiable "Choice" attribute of this
        // object.
};

// FREE OPERATORS
inline
bool operator==(const BasicSchema& lhs, const BasicSchema& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
    // the same value, and 'false' otherwise.  Two attribute objects have the
    // same value if each respective attribute has the same value.

inline
bool operator!=(const BasicSchema& lhs, const BasicSchema& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
    // have the same value, and 'false' otherwise.  Two attribute objects do
    // not have the same value if one or more respective attributes differ in
    // values.

inline
bsl::ostream& operator<<(bsl::ostream& stream, const BasicSchema& rhs);
    // Format the specified 'rhs' to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& hashAlg, const s_baltst::BasicSchema& object);
    // Pass the specified 'object' to the specified 'hashAlg'.  This function
    // integrates with the 'bslh' modular hashing system and effectively
    // provides a 'bsl::hash' specialization for 'BasicSchema'.

}  // close package namespace

// TRAITS

BDLAT_DECL_SEQUENCE_WITH_ALLOCATOR_BITWISEMOVEABLE_TRAITS(s_baltst::BasicSchema)

// ============================================================================
//                         INLINE FUNCTION DEFINITIONS
// ============================================================================

namespace s_baltst {

                     // ----------------------------------
                     // class BasicSchemaAnnotationElement
                     // ----------------------------------

// CLASS METHODS
// MANIPULATORS
template <class MANIPULATOR>
int BasicSchemaAnnotationElement::manipulateAttributes(MANIPULATOR& manipulator)
{
    int ret;

    ret = manipulator(&d_documentation, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_DOCUMENTATION]);
    if (ret) {
        return ret;
    }

    return ret;
}

template <class MANIPULATOR>
int BasicSchemaAnnotationElement::manipulateAttribute(MANIPULATOR& manipulator, int id)
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_DOCUMENTATION: {
        return manipulator(&d_documentation, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_DOCUMENTATION]);
      }
      default:
        return NOT_FOUND;
    }
}

template <class MANIPULATOR>
int BasicSchemaAnnotationElement::manipulateAttribute(
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
bdlb::NullableValue<bsl::string>& BasicSchemaAnnotationElement::documentation()
{
    return d_documentation;
}

// ACCESSORS
template <class ACCESSOR>
int BasicSchemaAnnotationElement::accessAttributes(ACCESSOR& accessor) const
{
    int ret;

    ret = accessor(d_documentation, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_DOCUMENTATION]);
    if (ret) {
        return ret;
    }

    return ret;
}

template <class ACCESSOR>
int BasicSchemaAnnotationElement::accessAttribute(ACCESSOR& accessor, int id) const
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_DOCUMENTATION: {
        return accessor(d_documentation, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_DOCUMENTATION]);
      }
      default:
        return NOT_FOUND;
    }
}

template <class ACCESSOR>
int BasicSchemaAnnotationElement::accessAttribute(
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
const bdlb::NullableValue<bsl::string>& BasicSchemaAnnotationElement::documentation() const
{
    return d_documentation;
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& hashAlg, const s_baltst::BasicSchemaAnnotationElement& object)
{
    (void)hashAlg;
    (void)object;
    using bslh::hashAppend;
    hashAppend(hashAlg, object.documentation());
}



                 // -----------------------------------------
                 // class BasicSchemaEnumerationElementChoice
                 // -----------------------------------------

// CLASS METHODS
// CREATORS
inline
BasicSchemaEnumerationElementChoice::BasicSchemaEnumerationElementChoice(bslma::Allocator *basicAllocator)
: d_selectionId(SELECTION_ID_UNDEFINED)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

inline
BasicSchemaEnumerationElementChoice::~BasicSchemaEnumerationElementChoice()
{
    reset();
}

// MANIPULATORS
template <class MANIPULATOR>
int BasicSchemaEnumerationElementChoice::manipulateSelection(MANIPULATOR& manipulator)
{
    switch (d_selectionId) {
      case BasicSchemaEnumerationElementChoice::SELECTION_ID_ANNOTATION:
        return manipulator(&d_annotation.object(),
                SELECTION_INFO_ARRAY[SELECTION_INDEX_ANNOTATION]);
      default:
        BSLS_ASSERT(BasicSchemaEnumerationElementChoice::SELECTION_ID_UNDEFINED == d_selectionId);
        return -1;
    }
}

inline
BasicSchemaAnnotationElement& BasicSchemaEnumerationElementChoice::annotation()
{
    BSLS_ASSERT(SELECTION_ID_ANNOTATION == d_selectionId);
    return d_annotation.object();
}

// ACCESSORS
inline
int BasicSchemaEnumerationElementChoice::selectionId() const
{
    return d_selectionId;
}

template <class ACCESSOR>
int BasicSchemaEnumerationElementChoice::accessSelection(ACCESSOR& accessor) const
{
    switch (d_selectionId) {
      case SELECTION_ID_ANNOTATION:
        return accessor(d_annotation.object(),
                SELECTION_INFO_ARRAY[SELECTION_INDEX_ANNOTATION]);
      default:
        BSLS_ASSERT(SELECTION_ID_UNDEFINED == d_selectionId);
        return -1;
    }
}

inline
const BasicSchemaAnnotationElement& BasicSchemaEnumerationElementChoice::annotation() const
{
    BSLS_ASSERT(SELECTION_ID_ANNOTATION == d_selectionId);
    return d_annotation.object();
}

inline
bool BasicSchemaEnumerationElementChoice::isAnnotationValue() const
{
    return SELECTION_ID_ANNOTATION == d_selectionId;
}

inline
bool BasicSchemaEnumerationElementChoice::isUndefinedValue() const
{
    return SELECTION_ID_UNDEFINED == d_selectionId;
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& hashAlg, const s_baltst::BasicSchemaEnumerationElementChoice& object)
{
    typedef s_baltst::BasicSchemaEnumerationElementChoice Class;
    using bslh::hashAppend;
    hashAppend(hashAlg, object.selectionId());
    switch (object.selectionId()) {
      case Class::SELECTION_ID_ANNOTATION:
        hashAppend(hashAlg, object.annotation());
        break;
      default:
        BSLS_ASSERT(Class::SELECTION_ID_UNDEFINED == object.selectionId());
    }
}


                    // -----------------------------------
                    // class BasicSchemaEnumerationElement
                    // -----------------------------------

// CLASS METHODS
// MANIPULATORS
template <class MANIPULATOR>
int BasicSchemaEnumerationElement::manipulateAttributes(MANIPULATOR& manipulator)
{
    int ret;

    ret = manipulator(&d_value, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_VALUE]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_choice, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CHOICE]);
    if (ret) {
        return ret;
    }

    return ret;
}

template <class MANIPULATOR>
int BasicSchemaEnumerationElement::manipulateAttribute(MANIPULATOR& manipulator, int id)
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_VALUE: {
        return manipulator(&d_value, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_VALUE]);
      }
      case ATTRIBUTE_ID_CHOICE: {
        return manipulator(&d_choice, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CHOICE]);
      }
      default:
        return NOT_FOUND;
    }
}

template <class MANIPULATOR>
int BasicSchemaEnumerationElement::manipulateAttribute(
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
bdlb::NullableValue<bsl::string>& BasicSchemaEnumerationElement::value()
{
    return d_value;
}

inline
bsl::vector<BasicSchemaEnumerationElementChoice>& BasicSchemaEnumerationElement::choice()
{
    return d_choice;
}

// ACCESSORS
template <class ACCESSOR>
int BasicSchemaEnumerationElement::accessAttributes(ACCESSOR& accessor) const
{
    int ret;

    ret = accessor(d_value, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_VALUE]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_choice, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CHOICE]);
    if (ret) {
        return ret;
    }

    return ret;
}

template <class ACCESSOR>
int BasicSchemaEnumerationElement::accessAttribute(ACCESSOR& accessor, int id) const
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_VALUE: {
        return accessor(d_value, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_VALUE]);
      }
      case ATTRIBUTE_ID_CHOICE: {
        return accessor(d_choice, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CHOICE]);
      }
      default:
        return NOT_FOUND;
    }
}

template <class ACCESSOR>
int BasicSchemaEnumerationElement::accessAttribute(
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
const bdlb::NullableValue<bsl::string>& BasicSchemaEnumerationElement::value() const
{
    return d_value;
}

inline
const bsl::vector<BasicSchemaEnumerationElementChoice>& BasicSchemaEnumerationElement::choice() const
{
    return d_choice;
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& hashAlg, const s_baltst::BasicSchemaEnumerationElement& object)
{
    (void)hashAlg;
    (void)object;
    using bslh::hashAppend;
    hashAppend(hashAlg, object.value());
    hashAppend(hashAlg, object.choice());
}



                 // -----------------------------------------
                 // class BasicSchemaRestrictionElementChoice
                 // -----------------------------------------

// CLASS METHODS
// CREATORS
inline
BasicSchemaRestrictionElementChoice::BasicSchemaRestrictionElementChoice(bslma::Allocator *basicAllocator)
: d_selectionId(SELECTION_ID_UNDEFINED)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

inline
BasicSchemaRestrictionElementChoice::~BasicSchemaRestrictionElementChoice()
{
    reset();
}

// MANIPULATORS
template <class MANIPULATOR>
int BasicSchemaRestrictionElementChoice::manipulateSelection(MANIPULATOR& manipulator)
{
    switch (d_selectionId) {
      case BasicSchemaRestrictionElementChoice::SELECTION_ID_ANNOTATION:
        return manipulator(&d_annotation.object(),
                SELECTION_INFO_ARRAY[SELECTION_INDEX_ANNOTATION]);
      case BasicSchemaRestrictionElementChoice::SELECTION_ID_ENUMERATION:
        return manipulator(&d_enumeration.object(),
                SELECTION_INFO_ARRAY[SELECTION_INDEX_ENUMERATION]);
      default:
        BSLS_ASSERT(BasicSchemaRestrictionElementChoice::SELECTION_ID_UNDEFINED == d_selectionId);
        return -1;
    }
}

inline
BasicSchemaAnnotationElement& BasicSchemaRestrictionElementChoice::annotation()
{
    BSLS_ASSERT(SELECTION_ID_ANNOTATION == d_selectionId);
    return d_annotation.object();
}

inline
BasicSchemaEnumerationElement& BasicSchemaRestrictionElementChoice::enumeration()
{
    BSLS_ASSERT(SELECTION_ID_ENUMERATION == d_selectionId);
    return d_enumeration.object();
}

// ACCESSORS
inline
int BasicSchemaRestrictionElementChoice::selectionId() const
{
    return d_selectionId;
}

template <class ACCESSOR>
int BasicSchemaRestrictionElementChoice::accessSelection(ACCESSOR& accessor) const
{
    switch (d_selectionId) {
      case SELECTION_ID_ANNOTATION:
        return accessor(d_annotation.object(),
                SELECTION_INFO_ARRAY[SELECTION_INDEX_ANNOTATION]);
      case SELECTION_ID_ENUMERATION:
        return accessor(d_enumeration.object(),
                SELECTION_INFO_ARRAY[SELECTION_INDEX_ENUMERATION]);
      default:
        BSLS_ASSERT(SELECTION_ID_UNDEFINED == d_selectionId);
        return -1;
    }
}

inline
const BasicSchemaAnnotationElement& BasicSchemaRestrictionElementChoice::annotation() const
{
    BSLS_ASSERT(SELECTION_ID_ANNOTATION == d_selectionId);
    return d_annotation.object();
}

inline
const BasicSchemaEnumerationElement& BasicSchemaRestrictionElementChoice::enumeration() const
{
    BSLS_ASSERT(SELECTION_ID_ENUMERATION == d_selectionId);
    return d_enumeration.object();
}

inline
bool BasicSchemaRestrictionElementChoice::isAnnotationValue() const
{
    return SELECTION_ID_ANNOTATION == d_selectionId;
}

inline
bool BasicSchemaRestrictionElementChoice::isEnumerationValue() const
{
    return SELECTION_ID_ENUMERATION == d_selectionId;
}

inline
bool BasicSchemaRestrictionElementChoice::isUndefinedValue() const
{
    return SELECTION_ID_UNDEFINED == d_selectionId;
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& hashAlg, const s_baltst::BasicSchemaRestrictionElementChoice& object)
{
    typedef s_baltst::BasicSchemaRestrictionElementChoice Class;
    using bslh::hashAppend;
    hashAppend(hashAlg, object.selectionId());
    switch (object.selectionId()) {
      case Class::SELECTION_ID_ANNOTATION:
        hashAppend(hashAlg, object.annotation());
        break;
      case Class::SELECTION_ID_ENUMERATION:
        hashAppend(hashAlg, object.enumeration());
        break;
      default:
        BSLS_ASSERT(Class::SELECTION_ID_UNDEFINED == object.selectionId());
    }
}


                    // -----------------------------------
                    // class BasicSchemaRestrictionElement
                    // -----------------------------------

// CLASS METHODS
// MANIPULATORS
template <class MANIPULATOR>
int BasicSchemaRestrictionElement::manipulateAttributes(MANIPULATOR& manipulator)
{
    int ret;

    ret = manipulator(&d_base, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_BASE]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_choice, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CHOICE]);
    if (ret) {
        return ret;
    }

    return ret;
}

template <class MANIPULATOR>
int BasicSchemaRestrictionElement::manipulateAttribute(MANIPULATOR& manipulator, int id)
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_BASE: {
        return manipulator(&d_base, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_BASE]);
      }
      case ATTRIBUTE_ID_CHOICE: {
        return manipulator(&d_choice, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CHOICE]);
      }
      default:
        return NOT_FOUND;
    }
}

template <class MANIPULATOR>
int BasicSchemaRestrictionElement::manipulateAttribute(
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
bdlb::NullableValue<bsl::string>& BasicSchemaRestrictionElement::base()
{
    return d_base;
}

inline
bsl::vector<BasicSchemaRestrictionElementChoice>& BasicSchemaRestrictionElement::choice()
{
    return d_choice;
}

// ACCESSORS
template <class ACCESSOR>
int BasicSchemaRestrictionElement::accessAttributes(ACCESSOR& accessor) const
{
    int ret;

    ret = accessor(d_base, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_BASE]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_choice, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CHOICE]);
    if (ret) {
        return ret;
    }

    return ret;
}

template <class ACCESSOR>
int BasicSchemaRestrictionElement::accessAttribute(ACCESSOR& accessor, int id) const
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_BASE: {
        return accessor(d_base, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_BASE]);
      }
      case ATTRIBUTE_ID_CHOICE: {
        return accessor(d_choice, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CHOICE]);
      }
      default:
        return NOT_FOUND;
    }
}

template <class ACCESSOR>
int BasicSchemaRestrictionElement::accessAttribute(
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
const bdlb::NullableValue<bsl::string>& BasicSchemaRestrictionElement::base() const
{
    return d_base;
}

inline
const bsl::vector<BasicSchemaRestrictionElementChoice>& BasicSchemaRestrictionElement::choice() const
{
    return d_choice;
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& hashAlg, const s_baltst::BasicSchemaRestrictionElement& object)
{
    (void)hashAlg;
    (void)object;
    using bslh::hashAppend;
    hashAppend(hashAlg, object.base());
    hashAppend(hashAlg, object.choice());
}



                  // ----------------------------------------
                  // class BasicSchemaSimpleTypeElementChoice
                  // ----------------------------------------

// CLASS METHODS
// CREATORS
inline
BasicSchemaSimpleTypeElementChoice::BasicSchemaSimpleTypeElementChoice(bslma::Allocator *basicAllocator)
: d_selectionId(SELECTION_ID_UNDEFINED)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

inline
BasicSchemaSimpleTypeElementChoice::~BasicSchemaSimpleTypeElementChoice()
{
    reset();
}

// MANIPULATORS
template <class MANIPULATOR>
int BasicSchemaSimpleTypeElementChoice::manipulateSelection(MANIPULATOR& manipulator)
{
    switch (d_selectionId) {
      case BasicSchemaSimpleTypeElementChoice::SELECTION_ID_ANNOTATION:
        return manipulator(&d_annotation.object(),
                SELECTION_INFO_ARRAY[SELECTION_INDEX_ANNOTATION]);
      case BasicSchemaSimpleTypeElementChoice::SELECTION_ID_RESTRICTION:
        return manipulator(&d_restriction.object(),
                SELECTION_INFO_ARRAY[SELECTION_INDEX_RESTRICTION]);
      default:
        BSLS_ASSERT(BasicSchemaSimpleTypeElementChoice::SELECTION_ID_UNDEFINED == d_selectionId);
        return -1;
    }
}

inline
BasicSchemaAnnotationElement& BasicSchemaSimpleTypeElementChoice::annotation()
{
    BSLS_ASSERT(SELECTION_ID_ANNOTATION == d_selectionId);
    return d_annotation.object();
}

inline
BasicSchemaRestrictionElement& BasicSchemaSimpleTypeElementChoice::restriction()
{
    BSLS_ASSERT(SELECTION_ID_RESTRICTION == d_selectionId);
    return d_restriction.object();
}

// ACCESSORS
inline
int BasicSchemaSimpleTypeElementChoice::selectionId() const
{
    return d_selectionId;
}

template <class ACCESSOR>
int BasicSchemaSimpleTypeElementChoice::accessSelection(ACCESSOR& accessor) const
{
    switch (d_selectionId) {
      case SELECTION_ID_ANNOTATION:
        return accessor(d_annotation.object(),
                SELECTION_INFO_ARRAY[SELECTION_INDEX_ANNOTATION]);
      case SELECTION_ID_RESTRICTION:
        return accessor(d_restriction.object(),
                SELECTION_INFO_ARRAY[SELECTION_INDEX_RESTRICTION]);
      default:
        BSLS_ASSERT(SELECTION_ID_UNDEFINED == d_selectionId);
        return -1;
    }
}

inline
const BasicSchemaAnnotationElement& BasicSchemaSimpleTypeElementChoice::annotation() const
{
    BSLS_ASSERT(SELECTION_ID_ANNOTATION == d_selectionId);
    return d_annotation.object();
}

inline
const BasicSchemaRestrictionElement& BasicSchemaSimpleTypeElementChoice::restriction() const
{
    BSLS_ASSERT(SELECTION_ID_RESTRICTION == d_selectionId);
    return d_restriction.object();
}

inline
bool BasicSchemaSimpleTypeElementChoice::isAnnotationValue() const
{
    return SELECTION_ID_ANNOTATION == d_selectionId;
}

inline
bool BasicSchemaSimpleTypeElementChoice::isRestrictionValue() const
{
    return SELECTION_ID_RESTRICTION == d_selectionId;
}

inline
bool BasicSchemaSimpleTypeElementChoice::isUndefinedValue() const
{
    return SELECTION_ID_UNDEFINED == d_selectionId;
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& hashAlg, const s_baltst::BasicSchemaSimpleTypeElementChoice& object)
{
    typedef s_baltst::BasicSchemaSimpleTypeElementChoice Class;
    using bslh::hashAppend;
    hashAppend(hashAlg, object.selectionId());
    switch (object.selectionId()) {
      case Class::SELECTION_ID_ANNOTATION:
        hashAppend(hashAlg, object.annotation());
        break;
      case Class::SELECTION_ID_RESTRICTION:
        hashAppend(hashAlg, object.restriction());
        break;
      default:
        BSLS_ASSERT(Class::SELECTION_ID_UNDEFINED == object.selectionId());
    }
}


                     // ----------------------------------
                     // class BasicSchemaSimpleTypeElement
                     // ----------------------------------

// CLASS METHODS
// MANIPULATORS
template <class MANIPULATOR>
int BasicSchemaSimpleTypeElement::manipulateAttributes(MANIPULATOR& manipulator)
{
    int ret;

    ret = manipulator(&d_name, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_NAME]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_preserveEnumOrder, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_PRESERVE_ENUM_ORDER]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_choice, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CHOICE]);
    if (ret) {
        return ret;
    }

    return ret;
}

template <class MANIPULATOR>
int BasicSchemaSimpleTypeElement::manipulateAttribute(MANIPULATOR& manipulator, int id)
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_NAME: {
        return manipulator(&d_name, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_NAME]);
      }
      case ATTRIBUTE_ID_PRESERVE_ENUM_ORDER: {
        return manipulator(&d_preserveEnumOrder, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_PRESERVE_ENUM_ORDER]);
      }
      case ATTRIBUTE_ID_CHOICE: {
        return manipulator(&d_choice, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CHOICE]);
      }
      default:
        return NOT_FOUND;
    }
}

template <class MANIPULATOR>
int BasicSchemaSimpleTypeElement::manipulateAttribute(
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
bdlb::NullableValue<bsl::string>& BasicSchemaSimpleTypeElement::name()
{
    return d_name;
}

inline
bdlb::NullableValue<bsl::string>& BasicSchemaSimpleTypeElement::preserveEnumOrder()
{
    return d_preserveEnumOrder;
}

inline
bsl::vector<BasicSchemaSimpleTypeElementChoice>& BasicSchemaSimpleTypeElement::choice()
{
    return d_choice;
}

// ACCESSORS
template <class ACCESSOR>
int BasicSchemaSimpleTypeElement::accessAttributes(ACCESSOR& accessor) const
{
    int ret;

    ret = accessor(d_name, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_NAME]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_preserveEnumOrder, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_PRESERVE_ENUM_ORDER]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_choice, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CHOICE]);
    if (ret) {
        return ret;
    }

    return ret;
}

template <class ACCESSOR>
int BasicSchemaSimpleTypeElement::accessAttribute(ACCESSOR& accessor, int id) const
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_NAME: {
        return accessor(d_name, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_NAME]);
      }
      case ATTRIBUTE_ID_PRESERVE_ENUM_ORDER: {
        return accessor(d_preserveEnumOrder, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_PRESERVE_ENUM_ORDER]);
      }
      case ATTRIBUTE_ID_CHOICE: {
        return accessor(d_choice, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CHOICE]);
      }
      default:
        return NOT_FOUND;
    }
}

template <class ACCESSOR>
int BasicSchemaSimpleTypeElement::accessAttribute(
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
const bdlb::NullableValue<bsl::string>& BasicSchemaSimpleTypeElement::name() const
{
    return d_name;
}

inline
const bdlb::NullableValue<bsl::string>& BasicSchemaSimpleTypeElement::preserveEnumOrder() const
{
    return d_preserveEnumOrder;
}

inline
const bsl::vector<BasicSchemaSimpleTypeElementChoice>& BasicSchemaSimpleTypeElement::choice() const
{
    return d_choice;
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& hashAlg, const s_baltst::BasicSchemaSimpleTypeElement& object)
{
    (void)hashAlg;
    (void)object;
    using bslh::hashAppend;
    hashAppend(hashAlg, object.name());
    hashAppend(hashAlg, object.preserveEnumOrder());
    hashAppend(hashAlg, object.choice());
}



                       // ------------------------------
                       // class BasicSchemaChoiceElement
                       // ------------------------------

// CLASS METHODS
// MANIPULATORS
template <class MANIPULATOR>
int BasicSchemaChoiceElement::manipulateAttributes(MANIPULATOR& manipulator)
{
    int ret;

    ret = manipulator(&d_choice, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CHOICE]);
    if (ret) {
        return ret;
    }

    return ret;
}

template <class MANIPULATOR>
int BasicSchemaChoiceElement::manipulateAttribute(MANIPULATOR& manipulator, int id)
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_CHOICE: {
        return manipulator(&d_choice, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CHOICE]);
      }
      default:
        return NOT_FOUND;
    }
}

template <class MANIPULATOR>
int BasicSchemaChoiceElement::manipulateAttribute(
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
bsl::vector<BasicSchemaChoiceElementChoice>& BasicSchemaChoiceElement::choice()
{
    return d_choice;
}

// ACCESSORS
template <class ACCESSOR>
int BasicSchemaChoiceElement::accessAttributes(ACCESSOR& accessor) const
{
    int ret;

    ret = accessor(d_choice, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CHOICE]);
    if (ret) {
        return ret;
    }

    return ret;
}

template <class ACCESSOR>
int BasicSchemaChoiceElement::accessAttribute(ACCESSOR& accessor, int id) const
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_CHOICE: {
        return accessor(d_choice, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CHOICE]);
      }
      default:
        return NOT_FOUND;
    }
}

template <class ACCESSOR>
int BasicSchemaChoiceElement::accessAttribute(
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
const bsl::vector<BasicSchemaChoiceElementChoice>& BasicSchemaChoiceElement::choice() const
{
    return d_choice;
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& hashAlg, const s_baltst::BasicSchemaChoiceElement& object)
{
    (void)hashAlg;
    (void)object;
    using bslh::hashAppend;
    hashAppend(hashAlg, object.choice());
}



                    // ------------------------------------
                    // class BasicSchemaChoiceElementChoice
                    // ------------------------------------

// CLASS METHODS
// CREATORS
inline
BasicSchemaChoiceElementChoice::BasicSchemaChoiceElementChoice(bslma::Allocator *basicAllocator)
: d_selectionId(SELECTION_ID_UNDEFINED)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

inline
BasicSchemaChoiceElementChoice::~BasicSchemaChoiceElementChoice()
{
    reset();
}

// MANIPULATORS
template <class MANIPULATOR>
int BasicSchemaChoiceElementChoice::manipulateSelection(MANIPULATOR& manipulator)
{
    switch (d_selectionId) {
      case BasicSchemaChoiceElementChoice::SELECTION_ID_ANNOTATION:
        return manipulator(&d_annotation.object(),
                SELECTION_INFO_ARRAY[SELECTION_INDEX_ANNOTATION]);
      case BasicSchemaChoiceElementChoice::SELECTION_ID_ELEMENT:
        return manipulator(d_element,
                SELECTION_INFO_ARRAY[SELECTION_INDEX_ELEMENT]);
      default:
        BSLS_ASSERT(BasicSchemaChoiceElementChoice::SELECTION_ID_UNDEFINED == d_selectionId);
        return -1;
    }
}

inline
BasicSchemaAnnotationElement& BasicSchemaChoiceElementChoice::annotation()
{
    BSLS_ASSERT(SELECTION_ID_ANNOTATION == d_selectionId);
    return d_annotation.object();
}

inline
BasicSchemaElement& BasicSchemaChoiceElementChoice::element()
{
    BSLS_ASSERT(SELECTION_ID_ELEMENT == d_selectionId);
    return *d_element;
}

// ACCESSORS
inline
int BasicSchemaChoiceElementChoice::selectionId() const
{
    return d_selectionId;
}

template <class ACCESSOR>
int BasicSchemaChoiceElementChoice::accessSelection(ACCESSOR& accessor) const
{
    switch (d_selectionId) {
      case SELECTION_ID_ANNOTATION:
        return accessor(d_annotation.object(),
                SELECTION_INFO_ARRAY[SELECTION_INDEX_ANNOTATION]);
      case SELECTION_ID_ELEMENT:
        return accessor(*d_element,
                SELECTION_INFO_ARRAY[SELECTION_INDEX_ELEMENT]);
      default:
        BSLS_ASSERT(SELECTION_ID_UNDEFINED == d_selectionId);
        return -1;
    }
}

inline
const BasicSchemaAnnotationElement& BasicSchemaChoiceElementChoice::annotation() const
{
    BSLS_ASSERT(SELECTION_ID_ANNOTATION == d_selectionId);
    return d_annotation.object();
}

inline
const BasicSchemaElement& BasicSchemaChoiceElementChoice::element() const
{
    BSLS_ASSERT(SELECTION_ID_ELEMENT == d_selectionId);
    return *d_element;
}

inline
bool BasicSchemaChoiceElementChoice::isAnnotationValue() const
{
    return SELECTION_ID_ANNOTATION == d_selectionId;
}

inline
bool BasicSchemaChoiceElementChoice::isElementValue() const
{
    return SELECTION_ID_ELEMENT == d_selectionId;
}

inline
bool BasicSchemaChoiceElementChoice::isUndefinedValue() const
{
    return SELECTION_ID_UNDEFINED == d_selectionId;
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& hashAlg, const s_baltst::BasicSchemaChoiceElementChoice& object)
{
    typedef s_baltst::BasicSchemaChoiceElementChoice Class;
    using bslh::hashAppend;
    hashAppend(hashAlg, object.selectionId());
    switch (object.selectionId()) {
      case Class::SELECTION_ID_ANNOTATION:
        hashAppend(hashAlg, object.annotation());
        break;
      case Class::SELECTION_ID_ELEMENT:
        hashAppend(hashAlg, object.element());
        break;
      default:
        BSLS_ASSERT(Class::SELECTION_ID_UNDEFINED == object.selectionId());
    }
}


                    // -----------------------------------
                    // class BasicSchemaComplexTypeElement
                    // -----------------------------------

// CLASS METHODS
// MANIPULATORS
template <class MANIPULATOR>
int BasicSchemaComplexTypeElement::manipulateAttributes(MANIPULATOR& manipulator)
{
    int ret;

    ret = manipulator(&d_name, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_NAME]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_choice, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CHOICE]);
    if (ret) {
        return ret;
    }

    return ret;
}

template <class MANIPULATOR>
int BasicSchemaComplexTypeElement::manipulateAttribute(MANIPULATOR& manipulator, int id)
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_NAME: {
        return manipulator(&d_name, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_NAME]);
      }
      case ATTRIBUTE_ID_CHOICE: {
        return manipulator(&d_choice, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CHOICE]);
      }
      default:
        return NOT_FOUND;
    }
}

template <class MANIPULATOR>
int BasicSchemaComplexTypeElement::manipulateAttribute(
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
bdlb::NullableValue<bsl::string>& BasicSchemaComplexTypeElement::name()
{
    return d_name;
}

inline
bsl::vector<BasicSchemaComplexTypeElementChoice>& BasicSchemaComplexTypeElement::choice()
{
    return d_choice;
}

// ACCESSORS
template <class ACCESSOR>
int BasicSchemaComplexTypeElement::accessAttributes(ACCESSOR& accessor) const
{
    int ret;

    ret = accessor(d_name, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_NAME]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_choice, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CHOICE]);
    if (ret) {
        return ret;
    }

    return ret;
}

template <class ACCESSOR>
int BasicSchemaComplexTypeElement::accessAttribute(ACCESSOR& accessor, int id) const
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_NAME: {
        return accessor(d_name, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_NAME]);
      }
      case ATTRIBUTE_ID_CHOICE: {
        return accessor(d_choice, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CHOICE]);
      }
      default:
        return NOT_FOUND;
    }
}

template <class ACCESSOR>
int BasicSchemaComplexTypeElement::accessAttribute(
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
const bdlb::NullableValue<bsl::string>& BasicSchemaComplexTypeElement::name() const
{
    return d_name;
}

inline
const bsl::vector<BasicSchemaComplexTypeElementChoice>& BasicSchemaComplexTypeElement::choice() const
{
    return d_choice;
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& hashAlg, const s_baltst::BasicSchemaComplexTypeElement& object)
{
    (void)hashAlg;
    (void)object;
    using bslh::hashAppend;
    hashAppend(hashAlg, object.name());
    hashAppend(hashAlg, object.choice());
}



                 // -----------------------------------------
                 // class BasicSchemaComplexTypeElementChoice
                 // -----------------------------------------

// CLASS METHODS
// CREATORS
inline
BasicSchemaComplexTypeElementChoice::BasicSchemaComplexTypeElementChoice(bslma::Allocator *basicAllocator)
: d_selectionId(SELECTION_ID_UNDEFINED)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

inline
BasicSchemaComplexTypeElementChoice::~BasicSchemaComplexTypeElementChoice()
{
    reset();
}

// MANIPULATORS
template <class MANIPULATOR>
int BasicSchemaComplexTypeElementChoice::manipulateSelection(MANIPULATOR& manipulator)
{
    switch (d_selectionId) {
      case BasicSchemaComplexTypeElementChoice::SELECTION_ID_ANNOTATION:
        return manipulator(&d_annotation.object(),
                SELECTION_INFO_ARRAY[SELECTION_INDEX_ANNOTATION]);
      case BasicSchemaComplexTypeElementChoice::SELECTION_ID_SEQUENCE:
        return manipulator(d_sequence,
                SELECTION_INFO_ARRAY[SELECTION_INDEX_SEQUENCE]);
      case BasicSchemaComplexTypeElementChoice::SELECTION_ID_CHOICE:
        return manipulator(d_choice,
                SELECTION_INFO_ARRAY[SELECTION_INDEX_CHOICE]);
      default:
        BSLS_ASSERT(BasicSchemaComplexTypeElementChoice::SELECTION_ID_UNDEFINED == d_selectionId);
        return -1;
    }
}

inline
BasicSchemaAnnotationElement& BasicSchemaComplexTypeElementChoice::annotation()
{
    BSLS_ASSERT(SELECTION_ID_ANNOTATION == d_selectionId);
    return d_annotation.object();
}

inline
BasicSchemaSequenceElement& BasicSchemaComplexTypeElementChoice::sequence()
{
    BSLS_ASSERT(SELECTION_ID_SEQUENCE == d_selectionId);
    return *d_sequence;
}

inline
BasicSchemaChoiceElement& BasicSchemaComplexTypeElementChoice::choice()
{
    BSLS_ASSERT(SELECTION_ID_CHOICE == d_selectionId);
    return *d_choice;
}

// ACCESSORS
inline
int BasicSchemaComplexTypeElementChoice::selectionId() const
{
    return d_selectionId;
}

template <class ACCESSOR>
int BasicSchemaComplexTypeElementChoice::accessSelection(ACCESSOR& accessor) const
{
    switch (d_selectionId) {
      case SELECTION_ID_ANNOTATION:
        return accessor(d_annotation.object(),
                SELECTION_INFO_ARRAY[SELECTION_INDEX_ANNOTATION]);
      case SELECTION_ID_SEQUENCE:
        return accessor(*d_sequence,
                SELECTION_INFO_ARRAY[SELECTION_INDEX_SEQUENCE]);
      case SELECTION_ID_CHOICE:
        return accessor(*d_choice,
                SELECTION_INFO_ARRAY[SELECTION_INDEX_CHOICE]);
      default:
        BSLS_ASSERT(SELECTION_ID_UNDEFINED == d_selectionId);
        return -1;
    }
}

inline
const BasicSchemaAnnotationElement& BasicSchemaComplexTypeElementChoice::annotation() const
{
    BSLS_ASSERT(SELECTION_ID_ANNOTATION == d_selectionId);
    return d_annotation.object();
}

inline
const BasicSchemaSequenceElement& BasicSchemaComplexTypeElementChoice::sequence() const
{
    BSLS_ASSERT(SELECTION_ID_SEQUENCE == d_selectionId);
    return *d_sequence;
}

inline
const BasicSchemaChoiceElement& BasicSchemaComplexTypeElementChoice::choice() const
{
    BSLS_ASSERT(SELECTION_ID_CHOICE == d_selectionId);
    return *d_choice;
}

inline
bool BasicSchemaComplexTypeElementChoice::isAnnotationValue() const
{
    return SELECTION_ID_ANNOTATION == d_selectionId;
}

inline
bool BasicSchemaComplexTypeElementChoice::isSequenceValue() const
{
    return SELECTION_ID_SEQUENCE == d_selectionId;
}

inline
bool BasicSchemaComplexTypeElementChoice::isChoiceValue() const
{
    return SELECTION_ID_CHOICE == d_selectionId;
}

inline
bool BasicSchemaComplexTypeElementChoice::isUndefinedValue() const
{
    return SELECTION_ID_UNDEFINED == d_selectionId;
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& hashAlg, const s_baltst::BasicSchemaComplexTypeElementChoice& object)
{
    typedef s_baltst::BasicSchemaComplexTypeElementChoice Class;
    using bslh::hashAppend;
    hashAppend(hashAlg, object.selectionId());
    switch (object.selectionId()) {
      case Class::SELECTION_ID_ANNOTATION:
        hashAppend(hashAlg, object.annotation());
        break;
      case Class::SELECTION_ID_SEQUENCE:
        hashAppend(hashAlg, object.sequence());
        break;
      case Class::SELECTION_ID_CHOICE:
        hashAppend(hashAlg, object.choice());
        break;
      default:
        BSLS_ASSERT(Class::SELECTION_ID_UNDEFINED == object.selectionId());
    }
}


                          // ------------------------
                          // class BasicSchemaElement
                          // ------------------------

// CLASS METHODS
// MANIPULATORS
template <class MANIPULATOR>
int BasicSchemaElement::manipulateAttributes(MANIPULATOR& manipulator)
{
    int ret;

    ret = manipulator(&d_name, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_NAME]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_type, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_TYPE]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_minOccurs, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_MIN_OCCURS]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_maxOccurs, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_MAX_OCCURS]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_default, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_DEFAULT_VALUE]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_cppdefault, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CPPDEFAULT]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_cpptype, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CPPTYPE]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_allocatesMemory, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ALLOCATES_MEMORY]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_allowsDirectManipulation, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ALLOWS_DIRECT_MANIPULATION]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_choice, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CHOICE]);
    if (ret) {
        return ret;
    }

    return ret;
}

template <class MANIPULATOR>
int BasicSchemaElement::manipulateAttribute(MANIPULATOR& manipulator, int id)
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_NAME: {
        return manipulator(&d_name, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_NAME]);
      }
      case ATTRIBUTE_ID_TYPE: {
        return manipulator(&d_type, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_TYPE]);
      }
      case ATTRIBUTE_ID_MIN_OCCURS: {
        return manipulator(&d_minOccurs, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_MIN_OCCURS]);
      }
      case ATTRIBUTE_ID_MAX_OCCURS: {
        return manipulator(&d_maxOccurs, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_MAX_OCCURS]);
      }
      case ATTRIBUTE_ID_DEFAULT_VALUE: {
        return manipulator(&d_default, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_DEFAULT_VALUE]);
      }
      case ATTRIBUTE_ID_CPPDEFAULT: {
        return manipulator(&d_cppdefault, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CPPDEFAULT]);
      }
      case ATTRIBUTE_ID_CPPTYPE: {
        return manipulator(&d_cpptype, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CPPTYPE]);
      }
      case ATTRIBUTE_ID_ALLOCATES_MEMORY: {
        return manipulator(&d_allocatesMemory, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ALLOCATES_MEMORY]);
      }
      case ATTRIBUTE_ID_ALLOWS_DIRECT_MANIPULATION: {
        return manipulator(&d_allowsDirectManipulation, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ALLOWS_DIRECT_MANIPULATION]);
      }
      case ATTRIBUTE_ID_CHOICE: {
        return manipulator(&d_choice, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CHOICE]);
      }
      default:
        return NOT_FOUND;
    }
}

template <class MANIPULATOR>
int BasicSchemaElement::manipulateAttribute(
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
bdlb::NullableValue<bsl::string>& BasicSchemaElement::name()
{
    return d_name;
}

inline
bdlb::NullableValue<bsl::string>& BasicSchemaElement::type()
{
    return d_type;
}

inline
bdlb::NullableValue<bsl::string>& BasicSchemaElement::minOccurs()
{
    return d_minOccurs;
}

inline
bdlb::NullableValue<bsl::string>& BasicSchemaElement::maxOccurs()
{
    return d_maxOccurs;
}

inline
bdlb::NullableValue<bsl::string>& BasicSchemaElement::defaultValue()
{
    return d_default;
}

inline
bdlb::NullableValue<bsl::string>& BasicSchemaElement::cppdefault()
{
    return d_cppdefault;
}

inline
bdlb::NullableValue<bsl::string>& BasicSchemaElement::cpptype()
{
    return d_cpptype;
}

inline
bdlb::NullableValue<bsl::string>& BasicSchemaElement::allocatesMemory()
{
    return d_allocatesMemory;
}

inline
bdlb::NullableValue<bsl::string>& BasicSchemaElement::allowsDirectManipulation()
{
    return d_allowsDirectManipulation;
}

inline
bsl::vector<BasicSchemaElementChoice>& BasicSchemaElement::choice()
{
    return d_choice;
}

// ACCESSORS
template <class ACCESSOR>
int BasicSchemaElement::accessAttributes(ACCESSOR& accessor) const
{
    int ret;

    ret = accessor(d_name, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_NAME]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_type, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_TYPE]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_minOccurs, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_MIN_OCCURS]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_maxOccurs, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_MAX_OCCURS]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_default, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_DEFAULT_VALUE]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_cppdefault, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CPPDEFAULT]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_cpptype, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CPPTYPE]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_allocatesMemory, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ALLOCATES_MEMORY]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_allowsDirectManipulation, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ALLOWS_DIRECT_MANIPULATION]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_choice, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CHOICE]);
    if (ret) {
        return ret;
    }

    return ret;
}

template <class ACCESSOR>
int BasicSchemaElement::accessAttribute(ACCESSOR& accessor, int id) const
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_NAME: {
        return accessor(d_name, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_NAME]);
      }
      case ATTRIBUTE_ID_TYPE: {
        return accessor(d_type, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_TYPE]);
      }
      case ATTRIBUTE_ID_MIN_OCCURS: {
        return accessor(d_minOccurs, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_MIN_OCCURS]);
      }
      case ATTRIBUTE_ID_MAX_OCCURS: {
        return accessor(d_maxOccurs, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_MAX_OCCURS]);
      }
      case ATTRIBUTE_ID_DEFAULT_VALUE: {
        return accessor(d_default, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_DEFAULT_VALUE]);
      }
      case ATTRIBUTE_ID_CPPDEFAULT: {
        return accessor(d_cppdefault, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CPPDEFAULT]);
      }
      case ATTRIBUTE_ID_CPPTYPE: {
        return accessor(d_cpptype, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CPPTYPE]);
      }
      case ATTRIBUTE_ID_ALLOCATES_MEMORY: {
        return accessor(d_allocatesMemory, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ALLOCATES_MEMORY]);
      }
      case ATTRIBUTE_ID_ALLOWS_DIRECT_MANIPULATION: {
        return accessor(d_allowsDirectManipulation, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ALLOWS_DIRECT_MANIPULATION]);
      }
      case ATTRIBUTE_ID_CHOICE: {
        return accessor(d_choice, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CHOICE]);
      }
      default:
        return NOT_FOUND;
    }
}

template <class ACCESSOR>
int BasicSchemaElement::accessAttribute(
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
const bdlb::NullableValue<bsl::string>& BasicSchemaElement::name() const
{
    return d_name;
}

inline
const bdlb::NullableValue<bsl::string>& BasicSchemaElement::type() const
{
    return d_type;
}

inline
const bdlb::NullableValue<bsl::string>& BasicSchemaElement::minOccurs() const
{
    return d_minOccurs;
}

inline
const bdlb::NullableValue<bsl::string>& BasicSchemaElement::maxOccurs() const
{
    return d_maxOccurs;
}

inline
const bdlb::NullableValue<bsl::string>& BasicSchemaElement::defaultValue() const
{
    return d_default;
}

inline
const bdlb::NullableValue<bsl::string>& BasicSchemaElement::cppdefault() const
{
    return d_cppdefault;
}

inline
const bdlb::NullableValue<bsl::string>& BasicSchemaElement::cpptype() const
{
    return d_cpptype;
}

inline
const bdlb::NullableValue<bsl::string>& BasicSchemaElement::allocatesMemory() const
{
    return d_allocatesMemory;
}

inline
const bdlb::NullableValue<bsl::string>& BasicSchemaElement::allowsDirectManipulation() const
{
    return d_allowsDirectManipulation;
}

inline
const bsl::vector<BasicSchemaElementChoice>& BasicSchemaElement::choice() const
{
    return d_choice;
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& hashAlg, const s_baltst::BasicSchemaElement& object)
{
    (void)hashAlg;
    (void)object;
    using bslh::hashAppend;
    hashAppend(hashAlg, object.name());
    hashAppend(hashAlg, object.type());
    hashAppend(hashAlg, object.minOccurs());
    hashAppend(hashAlg, object.maxOccurs());
    hashAppend(hashAlg, object.defaultValue());
    hashAppend(hashAlg, object.cppdefault());
    hashAppend(hashAlg, object.cpptype());
    hashAppend(hashAlg, object.allocatesMemory());
    hashAppend(hashAlg, object.allowsDirectManipulation());
    hashAppend(hashAlg, object.choice());
}



                       // ------------------------------
                       // class BasicSchemaElementChoice
                       // ------------------------------

// CLASS METHODS
// CREATORS
inline
BasicSchemaElementChoice::BasicSchemaElementChoice(bslma::Allocator *basicAllocator)
: d_selectionId(SELECTION_ID_UNDEFINED)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

inline
BasicSchemaElementChoice::~BasicSchemaElementChoice()
{
    reset();
}

// MANIPULATORS
template <class MANIPULATOR>
int BasicSchemaElementChoice::manipulateSelection(MANIPULATOR& manipulator)
{
    switch (d_selectionId) {
      case BasicSchemaElementChoice::SELECTION_ID_ANNOTATION:
        return manipulator(&d_annotation.object(),
                SELECTION_INFO_ARRAY[SELECTION_INDEX_ANNOTATION]);
      case BasicSchemaElementChoice::SELECTION_ID_SIMPLE_TYPE:
        return manipulator(&d_simpleType.object(),
                SELECTION_INFO_ARRAY[SELECTION_INDEX_SIMPLE_TYPE]);
      case BasicSchemaElementChoice::SELECTION_ID_COMPLEX_TYPE:
        return manipulator(d_complexType,
                SELECTION_INFO_ARRAY[SELECTION_INDEX_COMPLEX_TYPE]);
      default:
        BSLS_ASSERT(BasicSchemaElementChoice::SELECTION_ID_UNDEFINED == d_selectionId);
        return -1;
    }
}

inline
BasicSchemaAnnotationElement& BasicSchemaElementChoice::annotation()
{
    BSLS_ASSERT(SELECTION_ID_ANNOTATION == d_selectionId);
    return d_annotation.object();
}

inline
BasicSchemaSimpleTypeElement& BasicSchemaElementChoice::simpleType()
{
    BSLS_ASSERT(SELECTION_ID_SIMPLE_TYPE == d_selectionId);
    return d_simpleType.object();
}

inline
BasicSchemaComplexTypeElement& BasicSchemaElementChoice::complexType()
{
    BSLS_ASSERT(SELECTION_ID_COMPLEX_TYPE == d_selectionId);
    return *d_complexType;
}

// ACCESSORS
inline
int BasicSchemaElementChoice::selectionId() const
{
    return d_selectionId;
}

template <class ACCESSOR>
int BasicSchemaElementChoice::accessSelection(ACCESSOR& accessor) const
{
    switch (d_selectionId) {
      case SELECTION_ID_ANNOTATION:
        return accessor(d_annotation.object(),
                SELECTION_INFO_ARRAY[SELECTION_INDEX_ANNOTATION]);
      case SELECTION_ID_SIMPLE_TYPE:
        return accessor(d_simpleType.object(),
                SELECTION_INFO_ARRAY[SELECTION_INDEX_SIMPLE_TYPE]);
      case SELECTION_ID_COMPLEX_TYPE:
        return accessor(*d_complexType,
                SELECTION_INFO_ARRAY[SELECTION_INDEX_COMPLEX_TYPE]);
      default:
        BSLS_ASSERT(SELECTION_ID_UNDEFINED == d_selectionId);
        return -1;
    }
}

inline
const BasicSchemaAnnotationElement& BasicSchemaElementChoice::annotation() const
{
    BSLS_ASSERT(SELECTION_ID_ANNOTATION == d_selectionId);
    return d_annotation.object();
}

inline
const BasicSchemaSimpleTypeElement& BasicSchemaElementChoice::simpleType() const
{
    BSLS_ASSERT(SELECTION_ID_SIMPLE_TYPE == d_selectionId);
    return d_simpleType.object();
}

inline
const BasicSchemaComplexTypeElement& BasicSchemaElementChoice::complexType() const
{
    BSLS_ASSERT(SELECTION_ID_COMPLEX_TYPE == d_selectionId);
    return *d_complexType;
}

inline
bool BasicSchemaElementChoice::isAnnotationValue() const
{
    return SELECTION_ID_ANNOTATION == d_selectionId;
}

inline
bool BasicSchemaElementChoice::isSimpleTypeValue() const
{
    return SELECTION_ID_SIMPLE_TYPE == d_selectionId;
}

inline
bool BasicSchemaElementChoice::isComplexTypeValue() const
{
    return SELECTION_ID_COMPLEX_TYPE == d_selectionId;
}

inline
bool BasicSchemaElementChoice::isUndefinedValue() const
{
    return SELECTION_ID_UNDEFINED == d_selectionId;
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& hashAlg, const s_baltst::BasicSchemaElementChoice& object)
{
    typedef s_baltst::BasicSchemaElementChoice Class;
    using bslh::hashAppend;
    hashAppend(hashAlg, object.selectionId());
    switch (object.selectionId()) {
      case Class::SELECTION_ID_ANNOTATION:
        hashAppend(hashAlg, object.annotation());
        break;
      case Class::SELECTION_ID_SIMPLE_TYPE:
        hashAppend(hashAlg, object.simpleType());
        break;
      case Class::SELECTION_ID_COMPLEX_TYPE:
        hashAppend(hashAlg, object.complexType());
        break;
      default:
        BSLS_ASSERT(Class::SELECTION_ID_UNDEFINED == object.selectionId());
    }
}


                      // --------------------------------
                      // class BasicSchemaSequenceElement
                      // --------------------------------

// CLASS METHODS
// MANIPULATORS
template <class MANIPULATOR>
int BasicSchemaSequenceElement::manipulateAttributes(MANIPULATOR& manipulator)
{
    int ret;

    ret = manipulator(&d_choice, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CHOICE]);
    if (ret) {
        return ret;
    }

    return ret;
}

template <class MANIPULATOR>
int BasicSchemaSequenceElement::manipulateAttribute(MANIPULATOR& manipulator, int id)
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_CHOICE: {
        return manipulator(&d_choice, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CHOICE]);
      }
      default:
        return NOT_FOUND;
    }
}

template <class MANIPULATOR>
int BasicSchemaSequenceElement::manipulateAttribute(
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
bsl::vector<BasicSchemaSequenceElementChoice>& BasicSchemaSequenceElement::choice()
{
    return d_choice;
}

// ACCESSORS
template <class ACCESSOR>
int BasicSchemaSequenceElement::accessAttributes(ACCESSOR& accessor) const
{
    int ret;

    ret = accessor(d_choice, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CHOICE]);
    if (ret) {
        return ret;
    }

    return ret;
}

template <class ACCESSOR>
int BasicSchemaSequenceElement::accessAttribute(ACCESSOR& accessor, int id) const
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_CHOICE: {
        return accessor(d_choice, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CHOICE]);
      }
      default:
        return NOT_FOUND;
    }
}

template <class ACCESSOR>
int BasicSchemaSequenceElement::accessAttribute(
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
const bsl::vector<BasicSchemaSequenceElementChoice>& BasicSchemaSequenceElement::choice() const
{
    return d_choice;
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& hashAlg, const s_baltst::BasicSchemaSequenceElement& object)
{
    (void)hashAlg;
    (void)object;
    using bslh::hashAppend;
    hashAppend(hashAlg, object.choice());
}



                   // --------------------------------------
                   // class BasicSchemaSequenceElementChoice
                   // --------------------------------------

// CLASS METHODS
// CREATORS
inline
BasicSchemaSequenceElementChoice::BasicSchemaSequenceElementChoice(bslma::Allocator *basicAllocator)
: d_selectionId(SELECTION_ID_UNDEFINED)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

inline
BasicSchemaSequenceElementChoice::~BasicSchemaSequenceElementChoice()
{
    reset();
}

// MANIPULATORS
template <class MANIPULATOR>
int BasicSchemaSequenceElementChoice::manipulateSelection(MANIPULATOR& manipulator)
{
    switch (d_selectionId) {
      case BasicSchemaSequenceElementChoice::SELECTION_ID_ANNOTATION:
        return manipulator(&d_annotation.object(),
                SELECTION_INFO_ARRAY[SELECTION_INDEX_ANNOTATION]);
      case BasicSchemaSequenceElementChoice::SELECTION_ID_ELEMENT:
        return manipulator(d_element,
                SELECTION_INFO_ARRAY[SELECTION_INDEX_ELEMENT]);
      default:
        BSLS_ASSERT(BasicSchemaSequenceElementChoice::SELECTION_ID_UNDEFINED == d_selectionId);
        return -1;
    }
}

inline
BasicSchemaAnnotationElement& BasicSchemaSequenceElementChoice::annotation()
{
    BSLS_ASSERT(SELECTION_ID_ANNOTATION == d_selectionId);
    return d_annotation.object();
}

inline
BasicSchemaElement& BasicSchemaSequenceElementChoice::element()
{
    BSLS_ASSERT(SELECTION_ID_ELEMENT == d_selectionId);
    return *d_element;
}

// ACCESSORS
inline
int BasicSchemaSequenceElementChoice::selectionId() const
{
    return d_selectionId;
}

template <class ACCESSOR>
int BasicSchemaSequenceElementChoice::accessSelection(ACCESSOR& accessor) const
{
    switch (d_selectionId) {
      case SELECTION_ID_ANNOTATION:
        return accessor(d_annotation.object(),
                SELECTION_INFO_ARRAY[SELECTION_INDEX_ANNOTATION]);
      case SELECTION_ID_ELEMENT:
        return accessor(*d_element,
                SELECTION_INFO_ARRAY[SELECTION_INDEX_ELEMENT]);
      default:
        BSLS_ASSERT(SELECTION_ID_UNDEFINED == d_selectionId);
        return -1;
    }
}

inline
const BasicSchemaAnnotationElement& BasicSchemaSequenceElementChoice::annotation() const
{
    BSLS_ASSERT(SELECTION_ID_ANNOTATION == d_selectionId);
    return d_annotation.object();
}

inline
const BasicSchemaElement& BasicSchemaSequenceElementChoice::element() const
{
    BSLS_ASSERT(SELECTION_ID_ELEMENT == d_selectionId);
    return *d_element;
}

inline
bool BasicSchemaSequenceElementChoice::isAnnotationValue() const
{
    return SELECTION_ID_ANNOTATION == d_selectionId;
}

inline
bool BasicSchemaSequenceElementChoice::isElementValue() const
{
    return SELECTION_ID_ELEMENT == d_selectionId;
}

inline
bool BasicSchemaSequenceElementChoice::isUndefinedValue() const
{
    return SELECTION_ID_UNDEFINED == d_selectionId;
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& hashAlg, const s_baltst::BasicSchemaSequenceElementChoice& object)
{
    typedef s_baltst::BasicSchemaSequenceElementChoice Class;
    using bslh::hashAppend;
    hashAppend(hashAlg, object.selectionId());
    switch (object.selectionId()) {
      case Class::SELECTION_ID_ANNOTATION:
        hashAppend(hashAlg, object.annotation());
        break;
      case Class::SELECTION_ID_ELEMENT:
        hashAppend(hashAlg, object.element());
        break;
      default:
        BSLS_ASSERT(Class::SELECTION_ID_UNDEFINED == object.selectionId());
    }
}


                          // -----------------------
                          // class BasicSchemaChoice
                          // -----------------------

// CLASS METHODS
// CREATORS
inline
BasicSchemaChoice::BasicSchemaChoice(bslma::Allocator *basicAllocator)
: d_selectionId(SELECTION_ID_UNDEFINED)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

inline
BasicSchemaChoice::~BasicSchemaChoice()
{
    reset();
}

// MANIPULATORS
template <class MANIPULATOR>
int BasicSchemaChoice::manipulateSelection(MANIPULATOR& manipulator)
{
    switch (d_selectionId) {
      case BasicSchemaChoice::SELECTION_ID_ANNOTATION:
        return manipulator(&d_annotation.object(),
                SELECTION_INFO_ARRAY[SELECTION_INDEX_ANNOTATION]);
      case BasicSchemaChoice::SELECTION_ID_SIMPLE_TYPE:
        return manipulator(&d_simpleType.object(),
                SELECTION_INFO_ARRAY[SELECTION_INDEX_SIMPLE_TYPE]);
      case BasicSchemaChoice::SELECTION_ID_COMPLEX_TYPE:
        return manipulator(d_complexType,
                SELECTION_INFO_ARRAY[SELECTION_INDEX_COMPLEX_TYPE]);
      default:
        BSLS_ASSERT(BasicSchemaChoice::SELECTION_ID_UNDEFINED == d_selectionId);
        return -1;
    }
}

inline
BasicSchemaAnnotationElement& BasicSchemaChoice::annotation()
{
    BSLS_ASSERT(SELECTION_ID_ANNOTATION == d_selectionId);
    return d_annotation.object();
}

inline
BasicSchemaSimpleTypeElement& BasicSchemaChoice::simpleType()
{
    BSLS_ASSERT(SELECTION_ID_SIMPLE_TYPE == d_selectionId);
    return d_simpleType.object();
}

inline
BasicSchemaComplexTypeElement& BasicSchemaChoice::complexType()
{
    BSLS_ASSERT(SELECTION_ID_COMPLEX_TYPE == d_selectionId);
    return *d_complexType;
}

// ACCESSORS
inline
int BasicSchemaChoice::selectionId() const
{
    return d_selectionId;
}

template <class ACCESSOR>
int BasicSchemaChoice::accessSelection(ACCESSOR& accessor) const
{
    switch (d_selectionId) {
      case SELECTION_ID_ANNOTATION:
        return accessor(d_annotation.object(),
                SELECTION_INFO_ARRAY[SELECTION_INDEX_ANNOTATION]);
      case SELECTION_ID_SIMPLE_TYPE:
        return accessor(d_simpleType.object(),
                SELECTION_INFO_ARRAY[SELECTION_INDEX_SIMPLE_TYPE]);
      case SELECTION_ID_COMPLEX_TYPE:
        return accessor(*d_complexType,
                SELECTION_INFO_ARRAY[SELECTION_INDEX_COMPLEX_TYPE]);
      default:
        BSLS_ASSERT(SELECTION_ID_UNDEFINED == d_selectionId);
        return -1;
    }
}

inline
const BasicSchemaAnnotationElement& BasicSchemaChoice::annotation() const
{
    BSLS_ASSERT(SELECTION_ID_ANNOTATION == d_selectionId);
    return d_annotation.object();
}

inline
const BasicSchemaSimpleTypeElement& BasicSchemaChoice::simpleType() const
{
    BSLS_ASSERT(SELECTION_ID_SIMPLE_TYPE == d_selectionId);
    return d_simpleType.object();
}

inline
const BasicSchemaComplexTypeElement& BasicSchemaChoice::complexType() const
{
    BSLS_ASSERT(SELECTION_ID_COMPLEX_TYPE == d_selectionId);
    return *d_complexType;
}

inline
bool BasicSchemaChoice::isAnnotationValue() const
{
    return SELECTION_ID_ANNOTATION == d_selectionId;
}

inline
bool BasicSchemaChoice::isSimpleTypeValue() const
{
    return SELECTION_ID_SIMPLE_TYPE == d_selectionId;
}

inline
bool BasicSchemaChoice::isComplexTypeValue() const
{
    return SELECTION_ID_COMPLEX_TYPE == d_selectionId;
}

inline
bool BasicSchemaChoice::isUndefinedValue() const
{
    return SELECTION_ID_UNDEFINED == d_selectionId;
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& hashAlg, const s_baltst::BasicSchemaChoice& object)
{
    typedef s_baltst::BasicSchemaChoice Class;
    using bslh::hashAppend;
    hashAppend(hashAlg, object.selectionId());
    switch (object.selectionId()) {
      case Class::SELECTION_ID_ANNOTATION:
        hashAppend(hashAlg, object.annotation());
        break;
      case Class::SELECTION_ID_SIMPLE_TYPE:
        hashAppend(hashAlg, object.simpleType());
        break;
      case Class::SELECTION_ID_COMPLEX_TYPE:
        hashAppend(hashAlg, object.complexType());
        break;
      default:
        BSLS_ASSERT(Class::SELECTION_ID_UNDEFINED == object.selectionId());
    }
}


                             // -----------------
                             // class BasicSchema
                             // -----------------

// CLASS METHODS
// MANIPULATORS
template <class MANIPULATOR>
int BasicSchema::manipulateAttributes(MANIPULATOR& manipulator)
{
    int ret;

    ret = manipulator(&d_xs, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_XS]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_bdem, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_BDEM]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_package, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_PACKAGE]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_elementFormDefault, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT_FORM_DEFAULT]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_choice, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CHOICE]);
    if (ret) {
        return ret;
    }

    return ret;
}

template <class MANIPULATOR>
int BasicSchema::manipulateAttribute(MANIPULATOR& manipulator, int id)
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_XS: {
        return manipulator(&d_xs, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_XS]);
      }
      case ATTRIBUTE_ID_BDEM: {
        return manipulator(&d_bdem, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_BDEM]);
      }
      case ATTRIBUTE_ID_PACKAGE: {
        return manipulator(&d_package, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_PACKAGE]);
      }
      case ATTRIBUTE_ID_ELEMENT_FORM_DEFAULT: {
        return manipulator(&d_elementFormDefault, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT_FORM_DEFAULT]);
      }
      case ATTRIBUTE_ID_CHOICE: {
        return manipulator(&d_choice, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CHOICE]);
      }
      default:
        return NOT_FOUND;
    }
}

template <class MANIPULATOR>
int BasicSchema::manipulateAttribute(
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
bdlb::NullableValue<bsl::string>& BasicSchema::xs()
{
    return d_xs;
}

inline
bdlb::NullableValue<bsl::string>& BasicSchema::bdem()
{
    return d_bdem;
}

inline
bdlb::NullableValue<bsl::string>& BasicSchema::package()
{
    return d_package;
}

inline
bdlb::NullableValue<bsl::string>& BasicSchema::elementFormDefault()
{
    return d_elementFormDefault;
}

inline
bsl::vector<BasicSchemaChoice>& BasicSchema::choice()
{
    return d_choice;
}

// ACCESSORS
template <class ACCESSOR>
int BasicSchema::accessAttributes(ACCESSOR& accessor) const
{
    int ret;

    ret = accessor(d_xs, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_XS]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_bdem, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_BDEM]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_package, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_PACKAGE]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_elementFormDefault, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT_FORM_DEFAULT]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_choice, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CHOICE]);
    if (ret) {
        return ret;
    }

    return ret;
}

template <class ACCESSOR>
int BasicSchema::accessAttribute(ACCESSOR& accessor, int id) const
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_XS: {
        return accessor(d_xs, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_XS]);
      }
      case ATTRIBUTE_ID_BDEM: {
        return accessor(d_bdem, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_BDEM]);
      }
      case ATTRIBUTE_ID_PACKAGE: {
        return accessor(d_package, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_PACKAGE]);
      }
      case ATTRIBUTE_ID_ELEMENT_FORM_DEFAULT: {
        return accessor(d_elementFormDefault, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT_FORM_DEFAULT]);
      }
      case ATTRIBUTE_ID_CHOICE: {
        return accessor(d_choice, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CHOICE]);
      }
      default:
        return NOT_FOUND;
    }
}

template <class ACCESSOR>
int BasicSchema::accessAttribute(
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
const bdlb::NullableValue<bsl::string>& BasicSchema::xs() const
{
    return d_xs;
}

inline
const bdlb::NullableValue<bsl::string>& BasicSchema::bdem() const
{
    return d_bdem;
}

inline
const bdlb::NullableValue<bsl::string>& BasicSchema::package() const
{
    return d_package;
}

inline
const bdlb::NullableValue<bsl::string>& BasicSchema::elementFormDefault() const
{
    return d_elementFormDefault;
}

inline
const bsl::vector<BasicSchemaChoice>& BasicSchema::choice() const
{
    return d_choice;
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& hashAlg, const s_baltst::BasicSchema& object)
{
    (void)hashAlg;
    (void)object;
    using bslh::hashAppend;
    hashAppend(hashAlg, object.xs());
    hashAppend(hashAlg, object.bdem());
    hashAppend(hashAlg, object.package());
    hashAppend(hashAlg, object.elementFormDefault());
    hashAppend(hashAlg, object.choice());
}

}  // close package namespace

// FREE FUNCTIONS

inline
bool s_baltst::operator==(
        const s_baltst::BasicSchemaAnnotationElement& lhs,
        const s_baltst::BasicSchemaAnnotationElement& rhs)
{
    return  lhs.documentation() == rhs.documentation();
}

inline
bool s_baltst::operator!=(
        const s_baltst::BasicSchemaAnnotationElement& lhs,
        const s_baltst::BasicSchemaAnnotationElement& rhs)
{
    return !(lhs == rhs);
}

inline
bsl::ostream& s_baltst::operator<<(
        bsl::ostream& stream,
        const s_baltst::BasicSchemaAnnotationElement& rhs)
{
    return rhs.print(stream, 0, -1);
}


inline
bool s_baltst::operator==(
        const s_baltst::BasicSchemaEnumerationElementChoice& lhs,
        const s_baltst::BasicSchemaEnumerationElementChoice& rhs)
{
    typedef s_baltst::BasicSchemaEnumerationElementChoice Class;
    if (lhs.selectionId() == rhs.selectionId()) {
        switch (rhs.selectionId()) {
          case Class::SELECTION_ID_ANNOTATION:
            return lhs.annotation() == rhs.annotation();
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
bool s_baltst::operator!=(
        const s_baltst::BasicSchemaEnumerationElementChoice& lhs,
        const s_baltst::BasicSchemaEnumerationElementChoice& rhs)
{
    return !(lhs == rhs);
}

inline
bsl::ostream& s_baltst::operator<<(
        bsl::ostream& stream,
        const s_baltst::BasicSchemaEnumerationElementChoice& rhs)
{
    return rhs.print(stream, 0, -1);
}


inline
bool s_baltst::operator==(
        const s_baltst::BasicSchemaEnumerationElement& lhs,
        const s_baltst::BasicSchemaEnumerationElement& rhs)
{
    return  lhs.value() == rhs.value()
         && lhs.choice() == rhs.choice();
}

inline
bool s_baltst::operator!=(
        const s_baltst::BasicSchemaEnumerationElement& lhs,
        const s_baltst::BasicSchemaEnumerationElement& rhs)
{
    return !(lhs == rhs);
}

inline
bsl::ostream& s_baltst::operator<<(
        bsl::ostream& stream,
        const s_baltst::BasicSchemaEnumerationElement& rhs)
{
    return rhs.print(stream, 0, -1);
}


inline
bool s_baltst::operator==(
        const s_baltst::BasicSchemaRestrictionElementChoice& lhs,
        const s_baltst::BasicSchemaRestrictionElementChoice& rhs)
{
    typedef s_baltst::BasicSchemaRestrictionElementChoice Class;
    if (lhs.selectionId() == rhs.selectionId()) {
        switch (rhs.selectionId()) {
          case Class::SELECTION_ID_ANNOTATION:
            return lhs.annotation() == rhs.annotation();
          case Class::SELECTION_ID_ENUMERATION:
            return lhs.enumeration() == rhs.enumeration();
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
bool s_baltst::operator!=(
        const s_baltst::BasicSchemaRestrictionElementChoice& lhs,
        const s_baltst::BasicSchemaRestrictionElementChoice& rhs)
{
    return !(lhs == rhs);
}

inline
bsl::ostream& s_baltst::operator<<(
        bsl::ostream& stream,
        const s_baltst::BasicSchemaRestrictionElementChoice& rhs)
{
    return rhs.print(stream, 0, -1);
}


inline
bool s_baltst::operator==(
        const s_baltst::BasicSchemaRestrictionElement& lhs,
        const s_baltst::BasicSchemaRestrictionElement& rhs)
{
    return  lhs.base() == rhs.base()
         && lhs.choice() == rhs.choice();
}

inline
bool s_baltst::operator!=(
        const s_baltst::BasicSchemaRestrictionElement& lhs,
        const s_baltst::BasicSchemaRestrictionElement& rhs)
{
    return !(lhs == rhs);
}

inline
bsl::ostream& s_baltst::operator<<(
        bsl::ostream& stream,
        const s_baltst::BasicSchemaRestrictionElement& rhs)
{
    return rhs.print(stream, 0, -1);
}


inline
bool s_baltst::operator==(
        const s_baltst::BasicSchemaSimpleTypeElementChoice& lhs,
        const s_baltst::BasicSchemaSimpleTypeElementChoice& rhs)
{
    typedef s_baltst::BasicSchemaSimpleTypeElementChoice Class;
    if (lhs.selectionId() == rhs.selectionId()) {
        switch (rhs.selectionId()) {
          case Class::SELECTION_ID_ANNOTATION:
            return lhs.annotation() == rhs.annotation();
          case Class::SELECTION_ID_RESTRICTION:
            return lhs.restriction() == rhs.restriction();
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
bool s_baltst::operator!=(
        const s_baltst::BasicSchemaSimpleTypeElementChoice& lhs,
        const s_baltst::BasicSchemaSimpleTypeElementChoice& rhs)
{
    return !(lhs == rhs);
}

inline
bsl::ostream& s_baltst::operator<<(
        bsl::ostream& stream,
        const s_baltst::BasicSchemaSimpleTypeElementChoice& rhs)
{
    return rhs.print(stream, 0, -1);
}


inline
bool s_baltst::operator==(
        const s_baltst::BasicSchemaSimpleTypeElement& lhs,
        const s_baltst::BasicSchemaSimpleTypeElement& rhs)
{
    return  lhs.name() == rhs.name()
         && lhs.preserveEnumOrder() == rhs.preserveEnumOrder()
         && lhs.choice() == rhs.choice();
}

inline
bool s_baltst::operator!=(
        const s_baltst::BasicSchemaSimpleTypeElement& lhs,
        const s_baltst::BasicSchemaSimpleTypeElement& rhs)
{
    return !(lhs == rhs);
}

inline
bsl::ostream& s_baltst::operator<<(
        bsl::ostream& stream,
        const s_baltst::BasicSchemaSimpleTypeElement& rhs)
{
    return rhs.print(stream, 0, -1);
}


inline
bool s_baltst::operator==(
        const s_baltst::BasicSchemaChoiceElement& lhs,
        const s_baltst::BasicSchemaChoiceElement& rhs)
{
    return  lhs.choice() == rhs.choice();
}

inline
bool s_baltst::operator!=(
        const s_baltst::BasicSchemaChoiceElement& lhs,
        const s_baltst::BasicSchemaChoiceElement& rhs)
{
    return !(lhs == rhs);
}

inline
bsl::ostream& s_baltst::operator<<(
        bsl::ostream& stream,
        const s_baltst::BasicSchemaChoiceElement& rhs)
{
    return rhs.print(stream, 0, -1);
}


inline
bool s_baltst::operator==(
        const s_baltst::BasicSchemaChoiceElementChoice& lhs,
        const s_baltst::BasicSchemaChoiceElementChoice& rhs)
{
    typedef s_baltst::BasicSchemaChoiceElementChoice Class;
    if (lhs.selectionId() == rhs.selectionId()) {
        switch (rhs.selectionId()) {
          case Class::SELECTION_ID_ANNOTATION:
            return lhs.annotation() == rhs.annotation();
          case Class::SELECTION_ID_ELEMENT:
            return lhs.element() == rhs.element();
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
bool s_baltst::operator!=(
        const s_baltst::BasicSchemaChoiceElementChoice& lhs,
        const s_baltst::BasicSchemaChoiceElementChoice& rhs)
{
    return !(lhs == rhs);
}

inline
bsl::ostream& s_baltst::operator<<(
        bsl::ostream& stream,
        const s_baltst::BasicSchemaChoiceElementChoice& rhs)
{
    return rhs.print(stream, 0, -1);
}


inline
bool s_baltst::operator==(
        const s_baltst::BasicSchemaComplexTypeElement& lhs,
        const s_baltst::BasicSchemaComplexTypeElement& rhs)
{
    return  lhs.name() == rhs.name()
         && lhs.choice() == rhs.choice();
}

inline
bool s_baltst::operator!=(
        const s_baltst::BasicSchemaComplexTypeElement& lhs,
        const s_baltst::BasicSchemaComplexTypeElement& rhs)
{
    return !(lhs == rhs);
}

inline
bsl::ostream& s_baltst::operator<<(
        bsl::ostream& stream,
        const s_baltst::BasicSchemaComplexTypeElement& rhs)
{
    return rhs.print(stream, 0, -1);
}


inline
bool s_baltst::operator==(
        const s_baltst::BasicSchemaComplexTypeElementChoice& lhs,
        const s_baltst::BasicSchemaComplexTypeElementChoice& rhs)
{
    typedef s_baltst::BasicSchemaComplexTypeElementChoice Class;
    if (lhs.selectionId() == rhs.selectionId()) {
        switch (rhs.selectionId()) {
          case Class::SELECTION_ID_ANNOTATION:
            return lhs.annotation() == rhs.annotation();
          case Class::SELECTION_ID_SEQUENCE:
            return lhs.sequence() == rhs.sequence();
          case Class::SELECTION_ID_CHOICE:
            return lhs.choice() == rhs.choice();
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
bool s_baltst::operator!=(
        const s_baltst::BasicSchemaComplexTypeElementChoice& lhs,
        const s_baltst::BasicSchemaComplexTypeElementChoice& rhs)
{
    return !(lhs == rhs);
}

inline
bsl::ostream& s_baltst::operator<<(
        bsl::ostream& stream,
        const s_baltst::BasicSchemaComplexTypeElementChoice& rhs)
{
    return rhs.print(stream, 0, -1);
}


inline
bool s_baltst::operator==(
        const s_baltst::BasicSchemaElement& lhs,
        const s_baltst::BasicSchemaElement& rhs)
{
    return  lhs.name() == rhs.name()
         && lhs.type() == rhs.type()
         && lhs.minOccurs() == rhs.minOccurs()
         && lhs.maxOccurs() == rhs.maxOccurs()
         && lhs.defaultValue() == rhs.defaultValue()
         && lhs.cppdefault() == rhs.cppdefault()
         && lhs.cpptype() == rhs.cpptype()
         && lhs.allocatesMemory() == rhs.allocatesMemory()
         && lhs.allowsDirectManipulation() == rhs.allowsDirectManipulation()
         && lhs.choice() == rhs.choice();
}

inline
bool s_baltst::operator!=(
        const s_baltst::BasicSchemaElement& lhs,
        const s_baltst::BasicSchemaElement& rhs)
{
    return !(lhs == rhs);
}

inline
bsl::ostream& s_baltst::operator<<(
        bsl::ostream& stream,
        const s_baltst::BasicSchemaElement& rhs)
{
    return rhs.print(stream, 0, -1);
}


inline
bool s_baltst::operator==(
        const s_baltst::BasicSchemaElementChoice& lhs,
        const s_baltst::BasicSchemaElementChoice& rhs)
{
    typedef s_baltst::BasicSchemaElementChoice Class;
    if (lhs.selectionId() == rhs.selectionId()) {
        switch (rhs.selectionId()) {
          case Class::SELECTION_ID_ANNOTATION:
            return lhs.annotation() == rhs.annotation();
          case Class::SELECTION_ID_SIMPLE_TYPE:
            return lhs.simpleType() == rhs.simpleType();
          case Class::SELECTION_ID_COMPLEX_TYPE:
            return lhs.complexType() == rhs.complexType();
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
bool s_baltst::operator!=(
        const s_baltst::BasicSchemaElementChoice& lhs,
        const s_baltst::BasicSchemaElementChoice& rhs)
{
    return !(lhs == rhs);
}

inline
bsl::ostream& s_baltst::operator<<(
        bsl::ostream& stream,
        const s_baltst::BasicSchemaElementChoice& rhs)
{
    return rhs.print(stream, 0, -1);
}


inline
bool s_baltst::operator==(
        const s_baltst::BasicSchemaSequenceElement& lhs,
        const s_baltst::BasicSchemaSequenceElement& rhs)
{
    return  lhs.choice() == rhs.choice();
}

inline
bool s_baltst::operator!=(
        const s_baltst::BasicSchemaSequenceElement& lhs,
        const s_baltst::BasicSchemaSequenceElement& rhs)
{
    return !(lhs == rhs);
}

inline
bsl::ostream& s_baltst::operator<<(
        bsl::ostream& stream,
        const s_baltst::BasicSchemaSequenceElement& rhs)
{
    return rhs.print(stream, 0, -1);
}


inline
bool s_baltst::operator==(
        const s_baltst::BasicSchemaSequenceElementChoice& lhs,
        const s_baltst::BasicSchemaSequenceElementChoice& rhs)
{
    typedef s_baltst::BasicSchemaSequenceElementChoice Class;
    if (lhs.selectionId() == rhs.selectionId()) {
        switch (rhs.selectionId()) {
          case Class::SELECTION_ID_ANNOTATION:
            return lhs.annotation() == rhs.annotation();
          case Class::SELECTION_ID_ELEMENT:
            return lhs.element() == rhs.element();
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
bool s_baltst::operator!=(
        const s_baltst::BasicSchemaSequenceElementChoice& lhs,
        const s_baltst::BasicSchemaSequenceElementChoice& rhs)
{
    return !(lhs == rhs);
}

inline
bsl::ostream& s_baltst::operator<<(
        bsl::ostream& stream,
        const s_baltst::BasicSchemaSequenceElementChoice& rhs)
{
    return rhs.print(stream, 0, -1);
}


inline
bool s_baltst::operator==(
        const s_baltst::BasicSchemaChoice& lhs,
        const s_baltst::BasicSchemaChoice& rhs)
{
    typedef s_baltst::BasicSchemaChoice Class;
    if (lhs.selectionId() == rhs.selectionId()) {
        switch (rhs.selectionId()) {
          case Class::SELECTION_ID_ANNOTATION:
            return lhs.annotation() == rhs.annotation();
          case Class::SELECTION_ID_SIMPLE_TYPE:
            return lhs.simpleType() == rhs.simpleType();
          case Class::SELECTION_ID_COMPLEX_TYPE:
            return lhs.complexType() == rhs.complexType();
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
bool s_baltst::operator!=(
        const s_baltst::BasicSchemaChoice& lhs,
        const s_baltst::BasicSchemaChoice& rhs)
{
    return !(lhs == rhs);
}

inline
bsl::ostream& s_baltst::operator<<(
        bsl::ostream& stream,
        const s_baltst::BasicSchemaChoice& rhs)
{
    return rhs.print(stream, 0, -1);
}


inline
bool s_baltst::operator==(
        const s_baltst::BasicSchema& lhs,
        const s_baltst::BasicSchema& rhs)
{
    return  lhs.xs() == rhs.xs()
         && lhs.bdem() == rhs.bdem()
         && lhs.package() == rhs.package()
         && lhs.elementFormDefault() == rhs.elementFormDefault()
         && lhs.choice() == rhs.choice();
}

inline
bool s_baltst::operator!=(
        const s_baltst::BasicSchema& lhs,
        const s_baltst::BasicSchema& rhs)
{
    return !(lhs == rhs);
}

inline
bsl::ostream& s_baltst::operator<<(
        bsl::ostream& stream,
        const s_baltst::BasicSchema& rhs)
{
    return rhs.print(stream, 0, -1);
}

}  // close enterprise namespace
#endif

// GENERATED BY BLP_BAS_CODEGEN_VERSION
// USING bas_codegen.pl --mode msg --noExternalization --noAggregateConversion --package s_baltst --msgComponent basicschema s_baltst_basicschema.xsd
// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright 2020 Bloomberg Finance L.P. All rights reserved.
//      Property of Bloomberg Finance L.P. (BFLP)
//      This software is made available solely pursuant to the
//      terms of a BFLP license agreement which governs its use.
// ------------------------------- END-OF-FILE --------------------------------
