// s_baltst_mysequencewithchoice.h      *DO NOT EDIT*      @generated -*-C++-*-
#ifndef INCLUDED_S_BALTST_MYSEQUENCEWITHCHOICE
#define INCLUDED_S_BALTST_MYSEQUENCEWITHCHOICE

#include <bsls_ident.h>
BSLS_IDENT_RCSID(s_baltst_mysequencewithchoice_h, "$Id$ $CSID$")
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

namespace s_baltst { class MySequenceWithChoiceChoice; }
namespace s_baltst { class MySequenceWithChoice; }
namespace s_baltst {

                      // ================================
                      // class MySequenceWithChoiceChoice
                      // ================================

class MySequenceWithChoiceChoice {

    // INSTANCE DATA
    union {
        bsls::ObjectBuffer< int > d_choiceA;
    };

    int                           d_selectionId;

    // PRIVATE ACCESSORS
    bool isEqualTo(const MySequenceWithChoiceChoice& rhs) const;

  public:
    // TYPES

    enum {
        SELECTION_ID_UNDEFINED = -1
      , SELECTION_ID_CHOICE_A = 0
    };

    enum {
        NUM_SELECTIONS = 1
    };

    enum {
        SELECTION_INDEX_CHOICE_A = 0
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

    /// Create an object of type `MySequenceWithChoiceChoice` having the
    /// default value.
    MySequenceWithChoiceChoice();

    /// Create an object of type `MySequenceWithChoiceChoice` having the
    /// value of the specified `original` object.
    MySequenceWithChoiceChoice(const MySequenceWithChoiceChoice& original);

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    /// Create an object of type `MySequenceWithChoiceChoice` having the
    /// value of the specified `original` object.  After performing this
    /// action, the `original` object will be left in a valid, but
    /// unspecified state.
    MySequenceWithChoiceChoice(MySequenceWithChoiceChoice&& original) noexcept;
#endif

    /// Destroy this object.
    ~MySequenceWithChoiceChoice();

    // MANIPULATORS

    /// Assign to this object the value of the specified `rhs` object.
    MySequenceWithChoiceChoice& operator=(const MySequenceWithChoiceChoice& rhs);

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    /// Assign to this object the value of the specified `rhs` object.
    /// After performing this action, the `rhs` object will be left in a
    /// valid, but unspecified state.
    MySequenceWithChoiceChoice& operator=(MySequenceWithChoiceChoice&& rhs);
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

    /// Invoke the specified `manipulator` on the address of the modifiable
    /// selection, supplying `manipulator` with the corresponding selection
    /// information structure.  Return the value returned from the
    /// invocation of `manipulator` if this object has a defined selection,
    /// and -1 otherwise.
    template <typename t_MANIPULATOR>
    int manipulateSelection(t_MANIPULATOR& manipulator);

    /// Return a reference to the modifiable "ChoiceA" selection of this
    /// object if "ChoiceA" is the current selection.  The behavior is
    /// undefined unless "ChoiceA" is the selection of this object.
    int& choiceA();

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
    template <typename t_ACCESSOR>
    int accessSelection(t_ACCESSOR& accessor) const;

    /// Return a reference to the non-modifiable "ChoiceA" selection of this
    /// object if "ChoiceA" is the current selection.  The behavior is
    /// undefined unless "ChoiceA" is the selection of this object.
    const int& choiceA() const;

    /// Return `true` if the value of this object is a "ChoiceA" value, and
    /// return `false` otherwise.
    bool isChoiceAValue() const;

    /// Return `true` if the value of this object is undefined, and `false`
    /// otherwise.
    bool isUndefinedValue() const;

    /// Return the symbolic name of the current selection of this object.
    const char *selectionName() const;

    // HIDDEN FRIENDS

    /// Return `true` if the specified `lhs` and `rhs` objects have the same
    /// value, and `false` otherwise.  Two `MySequenceWithChoiceChoice`
    /// objects have the same value if either the selections in both objects
    /// have the same ids and the same values, or both selections are
    /// undefined.
    friend bool operator==(const MySequenceWithChoiceChoice& lhs,
                           const MySequenceWithChoiceChoice& rhs)
    {
        return lhs.isEqualTo(rhs);
    }

    /// Return `true` if the specified `lhs` and `rhs` objects do not have
    /// the same values, as determined by `operator==`, and `false`
    /// otherwise.
    friend bool operator!=(const MySequenceWithChoiceChoice& lhs,
                           const MySequenceWithChoiceChoice& rhs)
    {
        return !(lhs == rhs);
    }

    /// Format the specified `rhs` to the specified output `stream` and
    /// return a reference to the modifiable `stream`.
    friend bsl::ostream& operator<<(bsl::ostream&                     stream,
                                    const MySequenceWithChoiceChoice& rhs)
    {
        return rhs.print(stream, 0, -1);
    }
};

}  // close package namespace

// TRAITS

BDLAT_DECL_CHOICE_WITH_BITWISEMOVEABLE_TRAITS(s_baltst::MySequenceWithChoiceChoice)

namespace s_baltst {

                         // ==========================
                         // class MySequenceWithChoice
                         // ==========================

class MySequenceWithChoice {

    // INSTANCE DATA
    MySequenceWithChoiceChoice  d_mode;

  public:
    // TYPES
    enum {
        ATTRIBUTE_ID_MODE = 0
    };

    enum {
        NUM_ATTRIBUTES = 1
    };

    enum {
        ATTRIBUTE_INDEX_MODE = 0
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

    /// Create an object of type `MySequenceWithChoice` having the default
    /// value.
    MySequenceWithChoice();


    // MANIPULATORS

    /// Reset this object to the default value (i.e., its value upon
    /// default construction).
    void reset();

    /// Invoke the specified `manipulator` sequentially on the address of
    /// each (modifiable) attribute of this object, supplying `manipulator`
    /// with the corresponding attribute information structure until such
    /// invocation returns a non-zero value.  Return the value from the
    /// last invocation of `manipulator` (i.e., the invocation that
    /// terminated the sequence).
    template <typename t_MANIPULATOR>
    int manipulateAttributes(t_MANIPULATOR& manipulator);

    /// Invoke the specified `manipulator` on the address of
    /// the (modifiable) attribute indicated by the specified `id`,
    /// supplying `manipulator` with the corresponding attribute
    /// information structure.  Return the value returned from the
    /// invocation of `manipulator` if `id` identifies an attribute of this
    /// class, and -1 otherwise.
    template <typename t_MANIPULATOR>
    int manipulateAttribute(t_MANIPULATOR& manipulator, int id);

    /// Invoke the specified `manipulator` on the address of
    /// the (modifiable) attribute indicated by the specified `name` of the
    /// specified `nameLength`, supplying `manipulator` with the
    /// corresponding attribute information structure.  Return the value
    /// returned from the invocation of `manipulator` if `name` identifies
    /// an attribute of this class, and -1 otherwise.
    template <typename t_MANIPULATOR>
    int manipulateAttribute(t_MANIPULATOR&  manipulator,
                            const char   *name,
                            int           nameLength);

    /// Return a reference to the modifiable "Mode" attribute of this
    /// object.
    MySequenceWithChoiceChoice& mode();

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
                        int           level          = 0,
                        int           spacesPerLevel = 4) const;

    /// Invoke the specified `accessor` sequentially on each
    /// (non-modifiable) attribute of this object, supplying `accessor`
    /// with the corresponding attribute information structure until such
    /// invocation returns a non-zero value.  Return the value from the
    /// last invocation of `accessor` (i.e., the invocation that terminated
    /// the sequence).
    template <typename t_ACCESSOR>
    int accessAttributes(t_ACCESSOR& accessor) const;

    /// Invoke the specified `accessor` on the (non-modifiable) attribute
    /// of this object indicated by the specified `id`, supplying `accessor`
    /// with the corresponding attribute information structure.  Return the
    /// value returned from the invocation of `accessor` if `id` identifies
    /// an attribute of this class, and -1 otherwise.
    template <typename t_ACCESSOR>
    int accessAttribute(t_ACCESSOR& accessor, int id) const;

    /// Invoke the specified `accessor` on the (non-modifiable) attribute
    /// of this object indicated by the specified `name` of the specified
    /// `nameLength`, supplying `accessor` with the corresponding attribute
    /// information structure.  Return the value returned from the
    /// invocation of `accessor` if `name` identifies an attribute of this
    /// class, and -1 otherwise.
    template <typename t_ACCESSOR>
    int accessAttribute(t_ACCESSOR&   accessor,
                        const char *name,
                        int         nameLength) const;

    /// Return a reference offering non-modifiable access to the "Mode"
    /// attribute of this object.
    const MySequenceWithChoiceChoice& mode() const;

    // HIDDEN FRIENDS

    /// Return `true` if the specified `lhs` and `rhs` attribute objects
    /// have the same value, and `false` otherwise.  Two attribute objects
    /// have the same value if each respective attribute has the same value.
    friend bool operator==(const MySequenceWithChoice& lhs,
                           const MySequenceWithChoice& rhs)
    {
        return lhs.mode() == rhs.mode();
    }

    /// Returns `!(lhs == rhs)`
    friend bool operator!=(const MySequenceWithChoice& lhs,
                           const MySequenceWithChoice& rhs)
    {
        return !(lhs == rhs);
    }

    /// Format the specified `rhs` to the specified output `stream` and
    /// return a reference to the modifiable `stream`.
    friend bsl::ostream& operator<<(bsl::ostream&               stream,
                                    const MySequenceWithChoice& rhs)
    {
        return rhs.print(stream, 0, -1);
    }
};

}  // close package namespace

// TRAITS

BDLAT_DECL_SEQUENCE_WITH_BITWISEMOVEABLE_TRAITS(s_baltst::MySequenceWithChoice)
template <>
struct bdlat_UsesDefaultValueFlag<s_baltst::MySequenceWithChoice> : bsl::true_type {};

// ============================================================================
//                          INLINE DEFINITIONS
// ============================================================================

namespace s_baltst {

                      // --------------------------------
                      // class MySequenceWithChoiceChoice
                      // --------------------------------

// CLASS METHODS
// PRIVATE ACCESSORS
inline
bool MySequenceWithChoiceChoice::isEqualTo(const MySequenceWithChoiceChoice& rhs) const
{
    typedef MySequenceWithChoiceChoice Class;
    if (this->selectionId() == rhs.selectionId()) {
        switch (rhs.selectionId()) {
          case Class::SELECTION_ID_CHOICE_A:
            return this->choiceA() == rhs.choiceA();
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
MySequenceWithChoiceChoice::MySequenceWithChoiceChoice()
: d_selectionId(SELECTION_ID_UNDEFINED)
{
}

inline
MySequenceWithChoiceChoice::~MySequenceWithChoiceChoice()
{
    reset();
}

// MANIPULATORS
template <typename t_MANIPULATOR>
int MySequenceWithChoiceChoice::manipulateSelection(t_MANIPULATOR& manipulator)
{
    switch (d_selectionId) {
      case MySequenceWithChoiceChoice::SELECTION_ID_CHOICE_A:
        return manipulator(&d_choiceA.object(),
                SELECTION_INFO_ARRAY[SELECTION_INDEX_CHOICE_A]);
      default:
        BSLS_ASSERT(MySequenceWithChoiceChoice::SELECTION_ID_UNDEFINED == d_selectionId);
        return -1;
    }
}

inline
int& MySequenceWithChoiceChoice::choiceA()
{
    BSLS_ASSERT(SELECTION_ID_CHOICE_A == d_selectionId);
    return d_choiceA.object();
}

// ACCESSORS
inline
int MySequenceWithChoiceChoice::selectionId() const
{
    return d_selectionId;
}

template <typename t_ACCESSOR>
int MySequenceWithChoiceChoice::accessSelection(t_ACCESSOR& accessor) const
{
    switch (d_selectionId) {
      case SELECTION_ID_CHOICE_A:
        return accessor(d_choiceA.object(),
                SELECTION_INFO_ARRAY[SELECTION_INDEX_CHOICE_A]);
      default:
        BSLS_ASSERT(SELECTION_ID_UNDEFINED == d_selectionId);
        return -1;
    }
}

inline
const int& MySequenceWithChoiceChoice::choiceA() const
{
    BSLS_ASSERT(SELECTION_ID_CHOICE_A == d_selectionId);
    return d_choiceA.object();
}

inline
bool MySequenceWithChoiceChoice::isChoiceAValue() const
{
    return SELECTION_ID_CHOICE_A == d_selectionId;
}

inline
bool MySequenceWithChoiceChoice::isUndefinedValue() const
{
    return SELECTION_ID_UNDEFINED == d_selectionId;
}


                         // --------------------------
                         // class MySequenceWithChoice
                         // --------------------------

// CLASS METHODS
// MANIPULATORS
template <typename t_MANIPULATOR>
int MySequenceWithChoice::manipulateAttributes(t_MANIPULATOR& manipulator)
{
    int ret;

    ret = manipulator(&d_mode, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_MODE]);
    if (ret) {
        return ret;
    }

    return 0;
}

template <typename t_MANIPULATOR>
int MySequenceWithChoice::manipulateAttribute(t_MANIPULATOR& manipulator, int id)
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_MODE: {
        return manipulator(&d_mode, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_MODE]);
      }
      default:
        return NOT_FOUND;
    }
}

template <typename t_MANIPULATOR>
int MySequenceWithChoice::manipulateAttribute(
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
MySequenceWithChoiceChoice& MySequenceWithChoice::mode()
{
    return d_mode;
}

// ACCESSORS
template <typename t_ACCESSOR>
int MySequenceWithChoice::accessAttributes(t_ACCESSOR& accessor) const
{
    int ret;

    ret = accessor(d_mode, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_MODE]);
    if (ret) {
        return ret;
    }

    return 0;
}

template <typename t_ACCESSOR>
int MySequenceWithChoice::accessAttribute(t_ACCESSOR& accessor, int id) const
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_MODE: {
        return accessor(d_mode, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_MODE]);
      }
      default:
        return NOT_FOUND;
    }
}

template <typename t_ACCESSOR>
int MySequenceWithChoice::accessAttribute(
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
const MySequenceWithChoiceChoice& MySequenceWithChoice::mode() const
{
    return d_mode;
}

}  // close package namespace

// FREE FUNCTIONS

}  // close enterprise namespace
#endif

// GENERATED BY BLP_BAS_CODEGEN_2025.08.21
// USING bas_codegen.pl s_baltst_mysequencewithchoice.xsd --mode msg --includedir . --msgComponent mysequencewithchoice --noRecurse --noExternalization --noHashSupport --noAggregateConversion
// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright 2025 Bloomberg Finance L.P. All rights reserved.
//      Property of Bloomberg Finance L.P. (BFLP)
//      This software is made available solely pursuant to the
//      terms of a BFLP license agreement which governs its use.
// ------------------------------- END-OF-FILE --------------------------------
