// s_baltst_mysequencewithchoices.h     *DO NOT EDIT*      @generated -*-C++-*-
#ifndef INCLUDED_S_BALTST_MYSEQUENCEWITHCHOICES
#define INCLUDED_S_BALTST_MYSEQUENCEWITHCHOICES

#include <bsls_ident.h>
BSLS_IDENT_RCSID(s_baltst_mysequencewithchoices_h, "$Id$ $CSID$")
BSLS_IDENT_PRAGMA_ONCE

//@PURPOSE: Provide value-semantic attribute classes

#include <bslalg_typetraits.h>

#include <bdlat_attributeinfo.h>

#include <bdlat_selectioninfo.h>

#include <bdlat_typetraits.h>

#include <bsls_objectbuffer.h>

#include <bsls_assert.h>

#include <bsl_iosfwd.h>
#include <bsl_limits.h>

namespace BloombergLP {

namespace s_baltst { class MySequenceWithChoicesChoice1; }
namespace s_baltst { class MySequenceWithChoicesChoice2; }
namespace s_baltst { class MySequenceWithChoices; }
namespace s_baltst {

                     // ==================================
                     // class MySequenceWithChoicesChoice1
                     // ==================================

class MySequenceWithChoicesChoice1 {

    // INSTANCE DATA
    union {
        bsls::ObjectBuffer< int > d_choiceA;
        bsls::ObjectBuffer< int > d_choiceB;
    };

    int                           d_selectionId;

  public:
    // TYPES

    enum {
        SELECTION_ID_UNDEFINED = -1
      , SELECTION_ID_CHOICE_A = 0
      , SELECTION_ID_CHOICE_B = 1
    };

    enum {
        NUM_SELECTIONS = 2
    };

    enum {
        SELECTION_INDEX_CHOICE_A = 0
      , SELECTION_INDEX_CHOICE_B = 1
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
    MySequenceWithChoicesChoice1();
        // Create an object of type 'MySequenceWithChoicesChoice1' having the
        // default value.

    MySequenceWithChoicesChoice1(const MySequenceWithChoicesChoice1& original);
        // Create an object of type 'MySequenceWithChoicesChoice1' having the
        // value of the specified 'original' object.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    MySequenceWithChoicesChoice1(MySequenceWithChoicesChoice1&& original) noexcept;
        // Create an object of type 'MySequenceWithChoicesChoice1' having the
        // value of the specified 'original' object.  After performing this
        // action, the 'original' object will be left in a valid, but
        // unspecified state.
#endif

    ~MySequenceWithChoicesChoice1();
        // Destroy this object.

    // MANIPULATORS
    MySequenceWithChoicesChoice1& operator=(const MySequenceWithChoicesChoice1& rhs);
        // Assign to this object the value of the specified 'rhs' object.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    MySequenceWithChoicesChoice1& operator=(MySequenceWithChoicesChoice1&& rhs);
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

    int& makeChoiceA();
    int& makeChoiceA(int value);
        // Set the value of this object to be a "ChoiceA" value.  Optionally
        // specify the 'value' of the "ChoiceA".  If 'value' is not specified,
        // the default "ChoiceA" value is used.

    int& makeChoiceB();
    int& makeChoiceB(int value);
        // Set the value of this object to be a "ChoiceB" value.  Optionally
        // specify the 'value' of the "ChoiceB".  If 'value' is not specified,
        // the default "ChoiceB" value is used.

    template<class MANIPULATOR>
    int manipulateSelection(MANIPULATOR& manipulator);
        // Invoke the specified 'manipulator' on the address of the modifiable
        // selection, supplying 'manipulator' with the corresponding selection
        // information structure.  Return the value returned from the
        // invocation of 'manipulator' if this object has a defined selection,
        // and -1 otherwise.

    int& choiceA();
        // Return a reference to the modifiable "ChoiceA" selection of this
        // object if "ChoiceA" is the current selection.  The behavior is
        // undefined unless "ChoiceA" is the selection of this object.

    int& choiceB();
        // Return a reference to the modifiable "ChoiceB" selection of this
        // object if "ChoiceB" is the current selection.  The behavior is
        // undefined unless "ChoiceB" is the selection of this object.

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

    const int& choiceA() const;
        // Return a reference to the non-modifiable "ChoiceA" selection of this
        // object if "ChoiceA" is the current selection.  The behavior is
        // undefined unless "ChoiceA" is the selection of this object.

    const int& choiceB() const;
        // Return a reference to the non-modifiable "ChoiceB" selection of this
        // object if "ChoiceB" is the current selection.  The behavior is
        // undefined unless "ChoiceB" is the selection of this object.

    bool isChoiceAValue() const;
        // Return 'true' if the value of this object is a "ChoiceA" value, and
        // return 'false' otherwise.

    bool isChoiceBValue() const;
        // Return 'true' if the value of this object is a "ChoiceB" value, and
        // return 'false' otherwise.

    bool isUndefinedValue() const;
        // Return 'true' if the value of this object is undefined, and 'false'
        // otherwise.

    const char *selectionName() const;
        // Return the symbolic name of the current selection of this object.
};

// FREE OPERATORS
inline
bool operator==(const MySequenceWithChoicesChoice1& lhs, const MySequenceWithChoicesChoice1& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects have the same
    // value, and 'false' otherwise.  Two 'MySequenceWithChoicesChoice1' objects have the same
    // value if either the selections in both objects have the same ids and
    // the same values, or both selections are undefined.

inline
bool operator!=(const MySequenceWithChoicesChoice1& lhs, const MySequenceWithChoicesChoice1& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects do not have the
    // same values, as determined by 'operator==', and 'false' otherwise.

inline
bsl::ostream& operator<<(bsl::ostream& stream, const MySequenceWithChoicesChoice1& rhs);
    // Format the specified 'rhs' to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.

}  // close package namespace

// TRAITS

BDLAT_DECL_CHOICE_WITH_BITWISEMOVEABLE_TRAITS(s_baltst::MySequenceWithChoicesChoice1)

namespace s_baltst {

                     // ==================================
                     // class MySequenceWithChoicesChoice2
                     // ==================================

class MySequenceWithChoicesChoice2 {

    // INSTANCE DATA
    union {
        bsls::ObjectBuffer< int > d_choiceC;
        bsls::ObjectBuffer< int > d_choiceD;
    };

    int                           d_selectionId;

  public:
    // TYPES

    enum {
        SELECTION_ID_UNDEFINED = -1
      , SELECTION_ID_CHOICE_C = 0
      , SELECTION_ID_CHOICE_D = 1
    };

    enum {
        NUM_SELECTIONS = 2
    };

    enum {
        SELECTION_INDEX_CHOICE_C = 0
      , SELECTION_INDEX_CHOICE_D = 1
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
    MySequenceWithChoicesChoice2();
        // Create an object of type 'MySequenceWithChoicesChoice2' having the
        // default value.

    MySequenceWithChoicesChoice2(const MySequenceWithChoicesChoice2& original);
        // Create an object of type 'MySequenceWithChoicesChoice2' having the
        // value of the specified 'original' object.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    MySequenceWithChoicesChoice2(MySequenceWithChoicesChoice2&& original) noexcept;
        // Create an object of type 'MySequenceWithChoicesChoice2' having the
        // value of the specified 'original' object.  After performing this
        // action, the 'original' object will be left in a valid, but
        // unspecified state.
#endif

    ~MySequenceWithChoicesChoice2();
        // Destroy this object.

    // MANIPULATORS
    MySequenceWithChoicesChoice2& operator=(const MySequenceWithChoicesChoice2& rhs);
        // Assign to this object the value of the specified 'rhs' object.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    MySequenceWithChoicesChoice2& operator=(MySequenceWithChoicesChoice2&& rhs);
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

    int& makeChoiceC();
    int& makeChoiceC(int value);
        // Set the value of this object to be a "ChoiceC" value.  Optionally
        // specify the 'value' of the "ChoiceC".  If 'value' is not specified,
        // the default "ChoiceC" value is used.

    int& makeChoiceD();
    int& makeChoiceD(int value);
        // Set the value of this object to be a "ChoiceD" value.  Optionally
        // specify the 'value' of the "ChoiceD".  If 'value' is not specified,
        // the default "ChoiceD" value is used.

    template<class MANIPULATOR>
    int manipulateSelection(MANIPULATOR& manipulator);
        // Invoke the specified 'manipulator' on the address of the modifiable
        // selection, supplying 'manipulator' with the corresponding selection
        // information structure.  Return the value returned from the
        // invocation of 'manipulator' if this object has a defined selection,
        // and -1 otherwise.

    int& choiceC();
        // Return a reference to the modifiable "ChoiceC" selection of this
        // object if "ChoiceC" is the current selection.  The behavior is
        // undefined unless "ChoiceC" is the selection of this object.

    int& choiceD();
        // Return a reference to the modifiable "ChoiceD" selection of this
        // object if "ChoiceD" is the current selection.  The behavior is
        // undefined unless "ChoiceD" is the selection of this object.

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

    const int& choiceC() const;
        // Return a reference to the non-modifiable "ChoiceC" selection of this
        // object if "ChoiceC" is the current selection.  The behavior is
        // undefined unless "ChoiceC" is the selection of this object.

    const int& choiceD() const;
        // Return a reference to the non-modifiable "ChoiceD" selection of this
        // object if "ChoiceD" is the current selection.  The behavior is
        // undefined unless "ChoiceD" is the selection of this object.

    bool isChoiceCValue() const;
        // Return 'true' if the value of this object is a "ChoiceC" value, and
        // return 'false' otherwise.

    bool isChoiceDValue() const;
        // Return 'true' if the value of this object is a "ChoiceD" value, and
        // return 'false' otherwise.

    bool isUndefinedValue() const;
        // Return 'true' if the value of this object is undefined, and 'false'
        // otherwise.

    const char *selectionName() const;
        // Return the symbolic name of the current selection of this object.
};

// FREE OPERATORS
inline
bool operator==(const MySequenceWithChoicesChoice2& lhs, const MySequenceWithChoicesChoice2& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects have the same
    // value, and 'false' otherwise.  Two 'MySequenceWithChoicesChoice2' objects have the same
    // value if either the selections in both objects have the same ids and
    // the same values, or both selections are undefined.

inline
bool operator!=(const MySequenceWithChoicesChoice2& lhs, const MySequenceWithChoicesChoice2& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects do not have the
    // same values, as determined by 'operator==', and 'false' otherwise.

inline
bsl::ostream& operator<<(bsl::ostream& stream, const MySequenceWithChoicesChoice2& rhs);
    // Format the specified 'rhs' to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.

}  // close package namespace

// TRAITS

BDLAT_DECL_CHOICE_WITH_BITWISEMOVEABLE_TRAITS(s_baltst::MySequenceWithChoicesChoice2)

namespace s_baltst {

                        // ===========================
                        // class MySequenceWithChoices
                        // ===========================

class MySequenceWithChoices {

    // INSTANCE DATA
    MySequenceWithChoicesChoice2  d_second;
    MySequenceWithChoicesChoice1  d_first;

  public:
    // TYPES
    enum {
        ATTRIBUTE_ID_FIRST  = 0
      , ATTRIBUTE_ID_SECOND = 1
    };

    enum {
        NUM_ATTRIBUTES = 2
    };

    enum {
        ATTRIBUTE_INDEX_FIRST  = 0
      , ATTRIBUTE_INDEX_SECOND = 1
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
    MySequenceWithChoices();
        // Create an object of type 'MySequenceWithChoices' having the default
        // value.

    MySequenceWithChoices(const MySequenceWithChoices& original);
        // Create an object of type 'MySequenceWithChoices' having the value of
        // the specified 'original' object.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    MySequenceWithChoices(MySequenceWithChoices&& original) = default;
        // Create an object of type 'MySequenceWithChoices' having the value of
        // the specified 'original' object.  After performing this action, the
        // 'original' object will be left in a valid, but unspecified state.
#endif

    ~MySequenceWithChoices();
        // Destroy this object.

    // MANIPULATORS
    MySequenceWithChoices& operator=(const MySequenceWithChoices& rhs);
        // Assign to this object the value of the specified 'rhs' object.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    MySequenceWithChoices& operator=(MySequenceWithChoices&& rhs);
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

    MySequenceWithChoicesChoice1& first();
        // Return a reference to the modifiable "First" attribute of this
        // object.

    MySequenceWithChoicesChoice2& second();
        // Return a reference to the modifiable "Second" attribute of this
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

    const MySequenceWithChoicesChoice1& first() const;
        // Return a reference offering non-modifiable access to the "First"
        // attribute of this object.

    const MySequenceWithChoicesChoice2& second() const;
        // Return a reference offering non-modifiable access to the "Second"
        // attribute of this object.
};

// FREE OPERATORS
inline
bool operator==(const MySequenceWithChoices& lhs, const MySequenceWithChoices& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
    // the same value, and 'false' otherwise.  Two attribute objects have the
    // same value if each respective attribute has the same value.

inline
bool operator!=(const MySequenceWithChoices& lhs, const MySequenceWithChoices& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
    // have the same value, and 'false' otherwise.  Two attribute objects do
    // not have the same value if one or more respective attributes differ in
    // values.

inline
bsl::ostream& operator<<(bsl::ostream& stream, const MySequenceWithChoices& rhs);
    // Format the specified 'rhs' to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.

}  // close package namespace

// TRAITS

BDLAT_DECL_SEQUENCE_WITH_BITWISEMOVEABLE_TRAITS(s_baltst::MySequenceWithChoices)

// ============================================================================
//                         INLINE FUNCTION DEFINITIONS
// ============================================================================

namespace s_baltst {

                     // ----------------------------------
                     // class MySequenceWithChoicesChoice1
                     // ----------------------------------

// CLASS METHODS
// CREATORS
inline
MySequenceWithChoicesChoice1::MySequenceWithChoicesChoice1()
: d_selectionId(SELECTION_ID_UNDEFINED)
{
}

inline
MySequenceWithChoicesChoice1::~MySequenceWithChoicesChoice1()
{
    reset();
}

// MANIPULATORS
template <class MANIPULATOR>
int MySequenceWithChoicesChoice1::manipulateSelection(MANIPULATOR& manipulator)
{
    switch (d_selectionId) {
      case MySequenceWithChoicesChoice1::SELECTION_ID_CHOICE_A:
        return manipulator(&d_choiceA.object(),
                SELECTION_INFO_ARRAY[SELECTION_INDEX_CHOICE_A]);
      case MySequenceWithChoicesChoice1::SELECTION_ID_CHOICE_B:
        return manipulator(&d_choiceB.object(),
                SELECTION_INFO_ARRAY[SELECTION_INDEX_CHOICE_B]);
      default:
        BSLS_ASSERT(MySequenceWithChoicesChoice1::SELECTION_ID_UNDEFINED == d_selectionId);
        return -1;
    }
}

inline
int& MySequenceWithChoicesChoice1::choiceA()
{
    BSLS_ASSERT(SELECTION_ID_CHOICE_A == d_selectionId);
    return d_choiceA.object();
}

inline
int& MySequenceWithChoicesChoice1::choiceB()
{
    BSLS_ASSERT(SELECTION_ID_CHOICE_B == d_selectionId);
    return d_choiceB.object();
}

// ACCESSORS
inline
int MySequenceWithChoicesChoice1::selectionId() const
{
    return d_selectionId;
}

template <class ACCESSOR>
int MySequenceWithChoicesChoice1::accessSelection(ACCESSOR& accessor) const
{
    switch (d_selectionId) {
      case SELECTION_ID_CHOICE_A:
        return accessor(d_choiceA.object(),
                SELECTION_INFO_ARRAY[SELECTION_INDEX_CHOICE_A]);
      case SELECTION_ID_CHOICE_B:
        return accessor(d_choiceB.object(),
                SELECTION_INFO_ARRAY[SELECTION_INDEX_CHOICE_B]);
      default:
        BSLS_ASSERT(SELECTION_ID_UNDEFINED == d_selectionId);
        return -1;
    }
}

inline
const int& MySequenceWithChoicesChoice1::choiceA() const
{
    BSLS_ASSERT(SELECTION_ID_CHOICE_A == d_selectionId);
    return d_choiceA.object();
}

inline
const int& MySequenceWithChoicesChoice1::choiceB() const
{
    BSLS_ASSERT(SELECTION_ID_CHOICE_B == d_selectionId);
    return d_choiceB.object();
}

inline
bool MySequenceWithChoicesChoice1::isChoiceAValue() const
{
    return SELECTION_ID_CHOICE_A == d_selectionId;
}

inline
bool MySequenceWithChoicesChoice1::isChoiceBValue() const
{
    return SELECTION_ID_CHOICE_B == d_selectionId;
}

inline
bool MySequenceWithChoicesChoice1::isUndefinedValue() const
{
    return SELECTION_ID_UNDEFINED == d_selectionId;
}


                     // ----------------------------------
                     // class MySequenceWithChoicesChoice2
                     // ----------------------------------

// CLASS METHODS
// CREATORS
inline
MySequenceWithChoicesChoice2::MySequenceWithChoicesChoice2()
: d_selectionId(SELECTION_ID_UNDEFINED)
{
}

inline
MySequenceWithChoicesChoice2::~MySequenceWithChoicesChoice2()
{
    reset();
}

// MANIPULATORS
template <class MANIPULATOR>
int MySequenceWithChoicesChoice2::manipulateSelection(MANIPULATOR& manipulator)
{
    switch (d_selectionId) {
      case MySequenceWithChoicesChoice2::SELECTION_ID_CHOICE_C:
        return manipulator(&d_choiceC.object(),
                SELECTION_INFO_ARRAY[SELECTION_INDEX_CHOICE_C]);
      case MySequenceWithChoicesChoice2::SELECTION_ID_CHOICE_D:
        return manipulator(&d_choiceD.object(),
                SELECTION_INFO_ARRAY[SELECTION_INDEX_CHOICE_D]);
      default:
        BSLS_ASSERT(MySequenceWithChoicesChoice2::SELECTION_ID_UNDEFINED == d_selectionId);
        return -1;
    }
}

inline
int& MySequenceWithChoicesChoice2::choiceC()
{
    BSLS_ASSERT(SELECTION_ID_CHOICE_C == d_selectionId);
    return d_choiceC.object();
}

inline
int& MySequenceWithChoicesChoice2::choiceD()
{
    BSLS_ASSERT(SELECTION_ID_CHOICE_D == d_selectionId);
    return d_choiceD.object();
}

// ACCESSORS
inline
int MySequenceWithChoicesChoice2::selectionId() const
{
    return d_selectionId;
}

template <class ACCESSOR>
int MySequenceWithChoicesChoice2::accessSelection(ACCESSOR& accessor) const
{
    switch (d_selectionId) {
      case SELECTION_ID_CHOICE_C:
        return accessor(d_choiceC.object(),
                SELECTION_INFO_ARRAY[SELECTION_INDEX_CHOICE_C]);
      case SELECTION_ID_CHOICE_D:
        return accessor(d_choiceD.object(),
                SELECTION_INFO_ARRAY[SELECTION_INDEX_CHOICE_D]);
      default:
        BSLS_ASSERT(SELECTION_ID_UNDEFINED == d_selectionId);
        return -1;
    }
}

inline
const int& MySequenceWithChoicesChoice2::choiceC() const
{
    BSLS_ASSERT(SELECTION_ID_CHOICE_C == d_selectionId);
    return d_choiceC.object();
}

inline
const int& MySequenceWithChoicesChoice2::choiceD() const
{
    BSLS_ASSERT(SELECTION_ID_CHOICE_D == d_selectionId);
    return d_choiceD.object();
}

inline
bool MySequenceWithChoicesChoice2::isChoiceCValue() const
{
    return SELECTION_ID_CHOICE_C == d_selectionId;
}

inline
bool MySequenceWithChoicesChoice2::isChoiceDValue() const
{
    return SELECTION_ID_CHOICE_D == d_selectionId;
}

inline
bool MySequenceWithChoicesChoice2::isUndefinedValue() const
{
    return SELECTION_ID_UNDEFINED == d_selectionId;
}


                        // ---------------------------
                        // class MySequenceWithChoices
                        // ---------------------------

// CLASS METHODS
// MANIPULATORS
template <class MANIPULATOR>
int MySequenceWithChoices::manipulateAttributes(MANIPULATOR& manipulator)
{
    int ret;

    ret = manipulator(&d_first, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_FIRST]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_second, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SECOND]);
    if (ret) {
        return ret;
    }

    return 0;
}

template <class MANIPULATOR>
int MySequenceWithChoices::manipulateAttribute(MANIPULATOR& manipulator, int id)
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_FIRST: {
        return manipulator(&d_first, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_FIRST]);
      }
      case ATTRIBUTE_ID_SECOND: {
        return manipulator(&d_second, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SECOND]);
      }
      default:
        return NOT_FOUND;
    }
}

template <class MANIPULATOR>
int MySequenceWithChoices::manipulateAttribute(
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
MySequenceWithChoicesChoice1& MySequenceWithChoices::first()
{
    return d_first;
}

inline
MySequenceWithChoicesChoice2& MySequenceWithChoices::second()
{
    return d_second;
}

// ACCESSORS
template <class ACCESSOR>
int MySequenceWithChoices::accessAttributes(ACCESSOR& accessor) const
{
    int ret;

    ret = accessor(d_first, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_FIRST]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_second, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SECOND]);
    if (ret) {
        return ret;
    }

    return 0;
}

template <class ACCESSOR>
int MySequenceWithChoices::accessAttribute(ACCESSOR& accessor, int id) const
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_FIRST: {
        return accessor(d_first, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_FIRST]);
      }
      case ATTRIBUTE_ID_SECOND: {
        return accessor(d_second, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SECOND]);
      }
      default:
        return NOT_FOUND;
    }
}

template <class ACCESSOR>
int MySequenceWithChoices::accessAttribute(
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
const MySequenceWithChoicesChoice1& MySequenceWithChoices::first() const
{
    return d_first;
}

inline
const MySequenceWithChoicesChoice2& MySequenceWithChoices::second() const
{
    return d_second;
}

}  // close package namespace

// FREE FUNCTIONS

inline
bool s_baltst::operator==(
        const s_baltst::MySequenceWithChoicesChoice1& lhs,
        const s_baltst::MySequenceWithChoicesChoice1& rhs)
{
    typedef s_baltst::MySequenceWithChoicesChoice1 Class;
    if (lhs.selectionId() == rhs.selectionId()) {
        switch (rhs.selectionId()) {
          case Class::SELECTION_ID_CHOICE_A:
            return lhs.choiceA() == rhs.choiceA();
          case Class::SELECTION_ID_CHOICE_B:
            return lhs.choiceB() == rhs.choiceB();
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
        const s_baltst::MySequenceWithChoicesChoice1& lhs,
        const s_baltst::MySequenceWithChoicesChoice1& rhs)
{
    return !(lhs == rhs);
}

inline
bsl::ostream& s_baltst::operator<<(
        bsl::ostream& stream,
        const s_baltst::MySequenceWithChoicesChoice1& rhs)
{
    return rhs.print(stream, 0, -1);
}


inline
bool s_baltst::operator==(
        const s_baltst::MySequenceWithChoicesChoice2& lhs,
        const s_baltst::MySequenceWithChoicesChoice2& rhs)
{
    typedef s_baltst::MySequenceWithChoicesChoice2 Class;
    if (lhs.selectionId() == rhs.selectionId()) {
        switch (rhs.selectionId()) {
          case Class::SELECTION_ID_CHOICE_C:
            return lhs.choiceC() == rhs.choiceC();
          case Class::SELECTION_ID_CHOICE_D:
            return lhs.choiceD() == rhs.choiceD();
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
        const s_baltst::MySequenceWithChoicesChoice2& lhs,
        const s_baltst::MySequenceWithChoicesChoice2& rhs)
{
    return !(lhs == rhs);
}

inline
bsl::ostream& s_baltst::operator<<(
        bsl::ostream& stream,
        const s_baltst::MySequenceWithChoicesChoice2& rhs)
{
    return rhs.print(stream, 0, -1);
}


inline
bool s_baltst::operator==(
        const s_baltst::MySequenceWithChoices& lhs,
        const s_baltst::MySequenceWithChoices& rhs)
{
    return  lhs.first() == rhs.first()
         && lhs.second() == rhs.second();
}

inline
bool s_baltst::operator!=(
        const s_baltst::MySequenceWithChoices& lhs,
        const s_baltst::MySequenceWithChoices& rhs)
{
    return !(lhs == rhs);
}

inline
bsl::ostream& s_baltst::operator<<(
        bsl::ostream& stream,
        const s_baltst::MySequenceWithChoices& rhs)
{
    return rhs.print(stream, 0, -1);
}

}  // close enterprise namespace
#endif

// GENERATED BY @BLP_BAS_CODEGEN_VERSION@
// USING bas_codegen.pl s_baltst_mysequencewithchoices.xsd --mode msg --includedir . --msgComponent mysequencewithchoices --noRecurse --noExternalization --noHashSupport --noAggregateConversion
// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright 2022 Bloomberg Finance L.P. All rights reserved.
//      Property of Bloomberg Finance L.P. (BFLP)
//      This software is made available solely pursuant to the
//      terms of a BFLP license agreement which governs its use.
// ------------------------------- END-OF-FILE --------------------------------
