// s_baltst_mysequencewithnullableanonymouschoice.h*DO NOT EDIT*@generated -*-C++-*-
#ifndef INCLUDED_S_BALTST_MYSEQUENCEWITHNULLABLEANONYMOUSCHOICE
#define INCLUDED_S_BALTST_MYSEQUENCEWITHNULLABLEANONYMOUSCHOICE

#include <bsls_ident.h>
BSLS_IDENT_RCSID(s_baltst_mysequencewithnullableanonymouschoice_h,"$Id$ $CSID$")
BSLS_IDENT_PRAGMA_ONCE

//@PURPOSE: Provide value-semantic attribute classes

#include <bslalg_typetraits.h>

#include <bdlat_attributeinfo.h>

#include <bdlat_selectioninfo.h>

#include <bdlat_typetraits.h>

#include <bsls_objectbuffer.h>

#include <bsls_assert.h>

#include <bdlb_nullablevalue.h>

#include <bsl_iosfwd.h>
#include <bsl_limits.h>

namespace BloombergLP {

namespace s_baltst { class MySequenceWithNullableAnonymousChoiceChoice; }
namespace s_baltst { class MySequenceWithNullableAnonymousChoice; }
namespace s_baltst {

             // =================================================
             // class MySequenceWithNullableAnonymousChoiceChoice
             // =================================================

class MySequenceWithNullableAnonymousChoiceChoice {

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

    /// Return selection information for the selection indicated by the
    /// specified `id` if the selection exists, and 0 otherwise.
    static const bdlat_SelectionInfo *lookupSelectionInfo(int id);

    /// Return selection information for the selection indicated by the
    /// specified `name` of the specified `nameLength` if the selection
    /// exists, and 0 otherwise.
    static const bdlat_SelectionInfo *lookupSelectionInfo(
                                                       const char *name,
                                                       int         nameLength);

    // CREATORS

    /// Create an object of type
    /// `MySequenceWithNullableAnonymousChoiceChoice` having the default
    /// value.
    MySequenceWithNullableAnonymousChoiceChoice();

    /// Create an object of type
    /// `MySequenceWithNullableAnonymousChoiceChoice` having the value of
    /// the specified `original` object.
    MySequenceWithNullableAnonymousChoiceChoice(const MySequenceWithNullableAnonymousChoiceChoice& original);

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    /// Create an object of type
    /// `MySequenceWithNullableAnonymousChoiceChoice` having the value of
    /// the specified `original` object.  After performing this action, the
    /// `original` object will be left in a valid, but unspecified state.
    MySequenceWithNullableAnonymousChoiceChoice(MySequenceWithNullableAnonymousChoiceChoice&& original) noexcept;
#endif

    /// Destroy this object.
    ~MySequenceWithNullableAnonymousChoiceChoice();

    // MANIPULATORS

    /// Assign to this object the value of the specified `rhs` object.
    MySequenceWithNullableAnonymousChoiceChoice& operator=(const MySequenceWithNullableAnonymousChoiceChoice& rhs);

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    /// Assign to this object the value of the specified `rhs` object.
    /// After performing this action, the `rhs` object will be left in a
    /// valid, but unspecified state.
    MySequenceWithNullableAnonymousChoiceChoice& operator=(MySequenceWithNullableAnonymousChoiceChoice&& rhs);
#endif

    /// Reset this object to the default value (i.e., its value upon default
    /// construction).
    void reset();

    /// Set the value of this object to be the default for the selection
    /// indicated by the specified `selectionId`.  Return 0 on success, and
    /// non-zero value otherwise (i.e., the selection is not found).
    int makeSelection(int selectionId);

    /// Set the value of this object to be the default for the selection
    /// indicated by the specified `name` of the specified `nameLength`.
    /// Return 0 on success, and non-zero value otherwise (i.e., the
    /// selection is not found).
    int makeSelection(const char *name, int nameLength);

    /// Set the value of this object to be a "ChoiceA" value.  Optionally
    /// specify the `value` of the "ChoiceA".  If `value` is not specified,
    /// the default "ChoiceA" value is used.
    int& makeChoiceA();
    int& makeChoiceA(int value);

    /// Set the value of this object to be a "ChoiceB" value.  Optionally
    /// specify the `value` of the "ChoiceB".  If `value` is not specified,
    /// the default "ChoiceB" value is used.
    int& makeChoiceB();
    int& makeChoiceB(int value);

    /// Invoke the specified `manipulator` on the address of the modifiable
    /// selection, supplying `manipulator` with the corresponding selection
    /// information structure.  Return the value returned from the
    /// invocation of `manipulator` if this object has a defined selection,
    /// and -1 otherwise.
    template<class MANIPULATOR>
    int manipulateSelection(MANIPULATOR& manipulator);

    /// Return a reference to the modifiable "ChoiceA" selection of this
    /// object if "ChoiceA" is the current selection.  The behavior is
    /// undefined unless "ChoiceA" is the selection of this object.
    int& choiceA();

    /// Return a reference to the modifiable "ChoiceB" selection of this
    /// object if "ChoiceB" is the current selection.  The behavior is
    /// undefined unless "ChoiceB" is the selection of this object.
    int& choiceB();

    // ACCESSORS

    /// Format this object to the specified output `stream` at the
    /// optionally specified indentation `level` and return a reference to
    /// the modifiable `stream`.  If `level` is specified, optionally
    /// specify `spacesPerLevel`, the number of spaces per indentation level
    /// for this and all of its nested objects.  Each line is indented by
    /// the absolute value of `level * spacesPerLevel`.  If `level` is
    /// negative, suppress indentation of the first line.  If
    /// `spacesPerLevel` is negative, suppress line breaks and format the
    /// entire output on one line.  If `stream` is initially invalid, this
    /// operation has no effect.  Note that a trailing newline is provided
    /// in multiline mode only.
    bsl::ostream& print(bsl::ostream& stream,
                        int           level = 0,
                        int           spacesPerLevel = 4) const;

    /// Return the id of the current selection if the selection is defined,
    /// and -1 otherwise.
    int selectionId() const;

    /// Invoke the specified `accessor` on the non-modifiable selection,
    /// supplying `accessor` with the corresponding selection information
    /// structure.  Return the value returned from the invocation of
    /// `accessor` if this object has a defined selection, and -1 otherwise.
    template<class ACCESSOR>
    int accessSelection(ACCESSOR& accessor) const;

    /// Return a reference to the non-modifiable "ChoiceA" selection of this
    /// object if "ChoiceA" is the current selection.  The behavior is
    /// undefined unless "ChoiceA" is the selection of this object.
    const int& choiceA() const;

    /// Return a reference to the non-modifiable "ChoiceB" selection of this
    /// object if "ChoiceB" is the current selection.  The behavior is
    /// undefined unless "ChoiceB" is the selection of this object.
    const int& choiceB() const;

    /// Return `true` if the value of this object is a "ChoiceA" value, and
    /// return `false` otherwise.
    bool isChoiceAValue() const;

    /// Return `true` if the value of this object is a "ChoiceB" value, and
    /// return `false` otherwise.
    bool isChoiceBValue() const;

    /// Return `true` if the value of this object is undefined, and `false`
    /// otherwise.
    bool isUndefinedValue() const;

    /// Return the symbolic name of the current selection of this object.
    const char *selectionName() const;
};

// FREE OPERATORS

/// Return `true` if the specified `lhs` and `rhs` objects have the same
/// value, and `false` otherwise.  Two `MySequenceWithNullableAnonymousChoiceChoice` objects have the same
/// value if either the selections in both objects have the same ids and
/// the same values, or both selections are undefined.
inline
bool operator==(const MySequenceWithNullableAnonymousChoiceChoice& lhs, const MySequenceWithNullableAnonymousChoiceChoice& rhs);

/// Return `true` if the specified `lhs` and `rhs` objects do not have the
/// same values, as determined by `operator==`, and `false` otherwise.
inline
bool operator!=(const MySequenceWithNullableAnonymousChoiceChoice& lhs, const MySequenceWithNullableAnonymousChoiceChoice& rhs);

/// Format the specified `rhs` to the specified output `stream` and
/// return a reference to the modifiable `stream`.
inline
bsl::ostream& operator<<(bsl::ostream& stream, const MySequenceWithNullableAnonymousChoiceChoice& rhs);

}  // close package namespace

// TRAITS

BDLAT_DECL_CHOICE_WITH_BITWISEMOVEABLE_TRAITS(s_baltst::MySequenceWithNullableAnonymousChoiceChoice)

namespace s_baltst {

                // ===========================================
                // class MySequenceWithNullableAnonymousChoice
                // ===========================================

class MySequenceWithNullableAnonymousChoice {

    // INSTANCE DATA
    bdlb::NullableValue<MySequenceWithNullableAnonymousChoiceChoice>  d_choice;

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

    /// Return attribute information for the attribute indicated by the
    /// specified `id` if the attribute exists, and 0 otherwise.
    static const bdlat_AttributeInfo *lookupAttributeInfo(int id);

    /// Return attribute information for the attribute indicated by the
    /// specified `name` of the specified `nameLength` if the attribute
    /// exists, and 0 otherwise.
    static const bdlat_AttributeInfo *lookupAttributeInfo(
                                                       const char *name,
                                                       int         nameLength);

    // CREATORS

    /// Create an object of type `MySequenceWithNullableAnonymousChoice`
    /// having the default value.
    MySequenceWithNullableAnonymousChoice();

    /// Create an object of type `MySequenceWithNullableAnonymousChoice`
    /// having the value of the specified `original` object.
    MySequenceWithNullableAnonymousChoice(const MySequenceWithNullableAnonymousChoice& original);

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    /// Create an object of type `MySequenceWithNullableAnonymousChoice`
    /// having the value of the specified `original` object.  After
    /// performing this action, the `original` object will be left in a
    /// valid, but unspecified state.
    MySequenceWithNullableAnonymousChoice(MySequenceWithNullableAnonymousChoice&& original) = default;
#endif

    /// Destroy this object.
    ~MySequenceWithNullableAnonymousChoice();

    // MANIPULATORS

    /// Assign to this object the value of the specified `rhs` object.
    MySequenceWithNullableAnonymousChoice& operator=(const MySequenceWithNullableAnonymousChoice& rhs);

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    /// Assign to this object the value of the specified `rhs` object.
    /// After performing this action, the `rhs` object will be left in a
    /// valid, but unspecified state.
    MySequenceWithNullableAnonymousChoice& operator=(MySequenceWithNullableAnonymousChoice&& rhs);
#endif

    /// Reset this object to the default value (i.e., its value upon
    /// default construction).
    void reset();

    /// Invoke the specified `manipulator` sequentially on the address of
    /// each (modifiable) attribute of this object, supplying `manipulator`
    /// with the corresponding attribute information structure until such
    /// invocation returns a non-zero value.  Return the value from the
    /// last invocation of `manipulator` (i.e., the invocation that
    /// terminated the sequence).
    template<class MANIPULATOR>
    int manipulateAttributes(MANIPULATOR& manipulator);

    /// Invoke the specified `manipulator` on the address of
    /// the (modifiable) attribute indicated by the specified `id`,
    /// supplying `manipulator` with the corresponding attribute
    /// information structure.  Return the value returned from the
    /// invocation of `manipulator` if `id` identifies an attribute of this
    /// class, and -1 otherwise.
    template<class MANIPULATOR>
    int manipulateAttribute(MANIPULATOR& manipulator, int id);

    /// Invoke the specified `manipulator` on the address of
    /// the (modifiable) attribute indicated by the specified `name` of the
    /// specified `nameLength`, supplying `manipulator` with the
    /// corresponding attribute information structure.  Return the value
    /// returned from the invocation of `manipulator` if `name` identifies
    /// an attribute of this class, and -1 otherwise.
    template<class MANIPULATOR>
    int manipulateAttribute(MANIPULATOR&  manipulator,
                            const char   *name,
                            int           nameLength);

    /// Return a reference to the modifiable "Choice" attribute of this
    /// object.
    bdlb::NullableValue<MySequenceWithNullableAnonymousChoiceChoice>& choice();

    // ACCESSORS

    /// Format this object to the specified output `stream` at the
    /// optionally specified indentation `level` and return a reference to
    /// the modifiable `stream`.  If `level` is specified, optionally
    /// specify `spacesPerLevel`, the number of spaces per indentation level
    /// for this and all of its nested objects.  Each line is indented by
    /// the absolute value of `level * spacesPerLevel`.  If `level` is
    /// negative, suppress indentation of the first line.  If
    /// `spacesPerLevel` is negative, suppress line breaks and format the
    /// entire output on one line.  If `stream` is initially invalid, this
    /// operation has no effect.  Note that a trailing newline is provided
    /// in multiline mode only.
    bsl::ostream& print(bsl::ostream& stream,
                        int           level = 0,
                        int           spacesPerLevel = 4) const;

    /// Invoke the specified `accessor` sequentially on each
    /// (non-modifiable) attribute of this object, supplying `accessor`
    /// with the corresponding attribute information structure until such
    /// invocation returns a non-zero value.  Return the value from the
    /// last invocation of `accessor` (i.e., the invocation that terminated
    /// the sequence).
    template<class ACCESSOR>
    int accessAttributes(ACCESSOR& accessor) const;

    /// Invoke the specified `accessor` on the (non-modifiable) attribute
    /// of this object indicated by the specified `id`, supplying `accessor`
    /// with the corresponding attribute information structure.  Return the
    /// value returned from the invocation of `accessor` if `id` identifies
    /// an attribute of this class, and -1 otherwise.
    template<class ACCESSOR>
    int accessAttribute(ACCESSOR& accessor, int id) const;

    /// Invoke the specified `accessor` on the (non-modifiable) attribute
    /// of this object indicated by the specified `name` of the specified
    /// `nameLength`, supplying `accessor` with the corresponding attribute
    /// information structure.  Return the value returned from the
    /// invocation of `accessor` if `name` identifies an attribute of this
    /// class, and -1 otherwise.
    template<class ACCESSOR>
    int accessAttribute(ACCESSOR&   accessor,
                        const char *name,
                        int         nameLength) const;

    /// Return a reference offering non-modifiable access to the "Choice"
    /// attribute of this object.
    const bdlb::NullableValue<MySequenceWithNullableAnonymousChoiceChoice>& choice() const;
};

// FREE OPERATORS

/// Return `true` if the specified `lhs` and `rhs` attribute objects have
/// the same value, and `false` otherwise.  Two attribute objects have the
/// same value if each respective attribute has the same value.
inline
bool operator==(const MySequenceWithNullableAnonymousChoice& lhs, const MySequenceWithNullableAnonymousChoice& rhs);

/// Return `true` if the specified `lhs` and `rhs` attribute objects do not
/// have the same value, and `false` otherwise.  Two attribute objects do
/// not have the same value if one or more respective attributes differ in
/// values.
inline
bool operator!=(const MySequenceWithNullableAnonymousChoice& lhs, const MySequenceWithNullableAnonymousChoice& rhs);

/// Format the specified `rhs` to the specified output `stream` and
/// return a reference to the modifiable `stream`.
inline
bsl::ostream& operator<<(bsl::ostream& stream, const MySequenceWithNullableAnonymousChoice& rhs);

}  // close package namespace

// TRAITS

BDLAT_DECL_SEQUENCE_WITH_BITWISEMOVEABLE_TRAITS(s_baltst::MySequenceWithNullableAnonymousChoice)

// ============================================================================
//                         INLINE FUNCTION DEFINITIONS
// ============================================================================

namespace s_baltst {

             // -------------------------------------------------
             // class MySequenceWithNullableAnonymousChoiceChoice
             // -------------------------------------------------

// CLASS METHODS
// CREATORS
inline
MySequenceWithNullableAnonymousChoiceChoice::MySequenceWithNullableAnonymousChoiceChoice()
: d_selectionId(SELECTION_ID_UNDEFINED)
{
}

inline
MySequenceWithNullableAnonymousChoiceChoice::~MySequenceWithNullableAnonymousChoiceChoice()
{
    reset();
}

// MANIPULATORS
template <class MANIPULATOR>
int MySequenceWithNullableAnonymousChoiceChoice::manipulateSelection(MANIPULATOR& manipulator)
{
    switch (d_selectionId) {
      case MySequenceWithNullableAnonymousChoiceChoice::SELECTION_ID_CHOICE_A:
        return manipulator(&d_choiceA.object(),
                SELECTION_INFO_ARRAY[SELECTION_INDEX_CHOICE_A]);
      case MySequenceWithNullableAnonymousChoiceChoice::SELECTION_ID_CHOICE_B:
        return manipulator(&d_choiceB.object(),
                SELECTION_INFO_ARRAY[SELECTION_INDEX_CHOICE_B]);
      default:
        BSLS_ASSERT(MySequenceWithNullableAnonymousChoiceChoice::SELECTION_ID_UNDEFINED == d_selectionId);
        return -1;
    }
}

inline
int& MySequenceWithNullableAnonymousChoiceChoice::choiceA()
{
    BSLS_ASSERT(SELECTION_ID_CHOICE_A == d_selectionId);
    return d_choiceA.object();
}

inline
int& MySequenceWithNullableAnonymousChoiceChoice::choiceB()
{
    BSLS_ASSERT(SELECTION_ID_CHOICE_B == d_selectionId);
    return d_choiceB.object();
}

// ACCESSORS
inline
int MySequenceWithNullableAnonymousChoiceChoice::selectionId() const
{
    return d_selectionId;
}

template <class ACCESSOR>
int MySequenceWithNullableAnonymousChoiceChoice::accessSelection(ACCESSOR& accessor) const
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
const int& MySequenceWithNullableAnonymousChoiceChoice::choiceA() const
{
    BSLS_ASSERT(SELECTION_ID_CHOICE_A == d_selectionId);
    return d_choiceA.object();
}

inline
const int& MySequenceWithNullableAnonymousChoiceChoice::choiceB() const
{
    BSLS_ASSERT(SELECTION_ID_CHOICE_B == d_selectionId);
    return d_choiceB.object();
}

inline
bool MySequenceWithNullableAnonymousChoiceChoice::isChoiceAValue() const
{
    return SELECTION_ID_CHOICE_A == d_selectionId;
}

inline
bool MySequenceWithNullableAnonymousChoiceChoice::isChoiceBValue() const
{
    return SELECTION_ID_CHOICE_B == d_selectionId;
}

inline
bool MySequenceWithNullableAnonymousChoiceChoice::isUndefinedValue() const
{
    return SELECTION_ID_UNDEFINED == d_selectionId;
}


                // -------------------------------------------
                // class MySequenceWithNullableAnonymousChoice
                // -------------------------------------------

// CLASS METHODS
// MANIPULATORS
template <class MANIPULATOR>
int MySequenceWithNullableAnonymousChoice::manipulateAttributes(MANIPULATOR& manipulator)
{
    int ret;

    ret = manipulator(&d_choice, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CHOICE]);
    if (ret) {
        return ret;
    }

    return ret;
}

template <class MANIPULATOR>
int MySequenceWithNullableAnonymousChoice::manipulateAttribute(MANIPULATOR& manipulator, int id)
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
int MySequenceWithNullableAnonymousChoice::manipulateAttribute(
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
bdlb::NullableValue<MySequenceWithNullableAnonymousChoiceChoice>& MySequenceWithNullableAnonymousChoice::choice()
{
    return d_choice;
}

// ACCESSORS
template <class ACCESSOR>
int MySequenceWithNullableAnonymousChoice::accessAttributes(ACCESSOR& accessor) const
{
    int ret;

    ret = accessor(d_choice, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CHOICE]);
    if (ret) {
        return ret;
    }

    return ret;
}

template <class ACCESSOR>
int MySequenceWithNullableAnonymousChoice::accessAttribute(ACCESSOR& accessor, int id) const
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
int MySequenceWithNullableAnonymousChoice::accessAttribute(
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
const bdlb::NullableValue<MySequenceWithNullableAnonymousChoiceChoice>& MySequenceWithNullableAnonymousChoice::choice() const
{
    return d_choice;
}

}  // close package namespace

// FREE FUNCTIONS

inline
bool s_baltst::operator==(
        const s_baltst::MySequenceWithNullableAnonymousChoiceChoice& lhs,
        const s_baltst::MySequenceWithNullableAnonymousChoiceChoice& rhs)
{
    typedef s_baltst::MySequenceWithNullableAnonymousChoiceChoice Class;
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
        const s_baltst::MySequenceWithNullableAnonymousChoiceChoice& lhs,
        const s_baltst::MySequenceWithNullableAnonymousChoiceChoice& rhs)
{
    return !(lhs == rhs);
}

inline
bsl::ostream& s_baltst::operator<<(
        bsl::ostream& stream,
        const s_baltst::MySequenceWithNullableAnonymousChoiceChoice& rhs)
{
    return rhs.print(stream, 0, -1);
}


inline
bool s_baltst::operator==(
        const s_baltst::MySequenceWithNullableAnonymousChoice& lhs,
        const s_baltst::MySequenceWithNullableAnonymousChoice& rhs)
{
    return  lhs.choice() == rhs.choice();
}

inline
bool s_baltst::operator!=(
        const s_baltst::MySequenceWithNullableAnonymousChoice& lhs,
        const s_baltst::MySequenceWithNullableAnonymousChoice& rhs)
{
    return !(lhs == rhs);
}

inline
bsl::ostream& s_baltst::operator<<(
        bsl::ostream& stream,
        const s_baltst::MySequenceWithNullableAnonymousChoice& rhs)
{
    return rhs.print(stream, 0, -1);
}

}  // close enterprise namespace
#endif

// GENERATED BY BLP_BAS_CODEGEN_2021.08.08
// USING bas_codegen.pl s_baltst_mysequencewithnullableanonymouschoice.xsd -m msg -C mysequencewithnullableanonymouschoice -p s_baltst --noExternalization --noHashSupport --noAggregateConversion
// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright 2021 Bloomberg Finance L.P. All rights reserved.
//      Property of Bloomberg Finance L.P. (BFLP)
//      This software is made available solely pursuant to the
//      terms of a BFLP license agreement which governs its use.
// ------------------------------- END-OF-FILE --------------------------------
