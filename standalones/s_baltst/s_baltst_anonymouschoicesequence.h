// s_baltst_anonymouschoicesequence.h    *DO NOT EDIT*     @generated -*-C++-*-
#ifndef INCLUDED_S_BALTST_ANONYMOUSCHOICESEQUENCE
#define INCLUDED_S_BALTST_ANONYMOUSCHOICESEQUENCE

#include <bsls_ident.h>
BSLS_IDENT_RCSID(s_baltst_anonymouschoicesequence_h, "$Id$ $CSID$")
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

namespace s_baltst { class AnonChoiceSequence; }
namespace s_baltst { class AnonChoice; }
namespace s_baltst {

                          // ========================
                          // class AnonChoiceSequence
                          // ========================

class AnonChoiceSequence {

    // INSTANCE DATA
    int  d_a;

  public:
    // TYPES
    enum {
        ATTRIBUTE_ID_A = 0
    };

    enum {
        NUM_ATTRIBUTES = 1
    };

    enum {
        ATTRIBUTE_INDEX_A = 0
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

    /// Create an object of type `AnonChoiceSequence` having the default
    /// value.
    AnonChoiceSequence();


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

    /// Return a reference to the modifiable "A" attribute of this object.
    int& a();

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

    /// Return the value of the "A" attribute of this object.
    int a() const;

    // HIDDEN FRIENDS

    /// Return `true` if the specified `lhs` and `rhs` attribute objects
    /// have the same value, and `false` otherwise.  Two attribute objects
    /// have the same value if each respective attribute has the same value.
    friend bool operator==(const AnonChoiceSequence& lhs,
                           const AnonChoiceSequence& rhs)
    {
        return lhs.a() == rhs.a();
    }

    /// Returns `!(lhs == rhs)`
    friend bool operator!=(const AnonChoiceSequence& lhs,
                           const AnonChoiceSequence& rhs)
    {
        return !(lhs == rhs);
    }

    /// Format the specified `rhs` to the specified output `stream` and
    /// return a reference to the modifiable `stream`.
    friend bsl::ostream& operator<<(bsl::ostream&             stream,
                                    const AnonChoiceSequence& rhs)
    {
        return rhs.print(stream, 0, -1);
    }
};

}  // close package namespace

// TRAITS

BDLAT_DECL_SEQUENCE_WITH_BITWISEMOVEABLE_TRAITS(s_baltst::AnonChoiceSequence)

namespace s_baltst {

                              // ================
                              // class AnonChoice
                              // ================

class AnonChoice {

    // INSTANCE DATA
    union {
        bsls::ObjectBuffer< AnonChoiceSequence > d_sequence;
    };

    int                                          d_selectionId;

    // PRIVATE ACCESSORS
    bool isEqualTo(const AnonChoice& rhs) const;

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

    /// Create an object of type `AnonChoice` having the default value.
    AnonChoice();

    /// Create an object of type `AnonChoice` having the value of the
    /// specified `original` object.
    AnonChoice(const AnonChoice& original);

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    /// Create an object of type `AnonChoice` having the value of the
    /// specified `original` object.  After performing this action, the
    /// `original` object will be left in a valid, but unspecified state.
    AnonChoice(AnonChoice&& original) noexcept;
#endif

    /// Destroy this object.
    ~AnonChoice();

    // MANIPULATORS

    /// Assign to this object the value of the specified `rhs` object.
    AnonChoice& operator=(const AnonChoice& rhs);

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    /// Assign to this object the value of the specified `rhs` object.
    /// After performing this action, the `rhs` object will be left in a
    /// valid, but unspecified state.
    AnonChoice& operator=(AnonChoice&& rhs);
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

    AnonChoiceSequence& makeSequence();
    AnonChoiceSequence& makeSequence(const AnonChoiceSequence& value);
#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    AnonChoiceSequence& makeSequence(AnonChoiceSequence&& value);
#endif
        // Set the value of this object to be a "Sequence" value.  Optionally
        // specify the 'value' of the "Sequence".  If 'value' is not specified,
        // the default "Sequence" value is used.

    /// Invoke the specified `manipulator` on the address of the modifiable
    /// selection, supplying `manipulator` with the corresponding selection
    /// information structure.  Return the value returned from the
    /// invocation of `manipulator` if this object has a defined selection,
    /// and -1 otherwise.
    template <typename t_MANIPULATOR>
    int manipulateSelection(t_MANIPULATOR& manipulator);

    /// Return a reference to the modifiable "Sequence" selection of this
    /// object if "Sequence" is the current selection.  The behavior is
    /// undefined unless "Sequence" is the selection of this object.
    AnonChoiceSequence& sequence();

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

    /// Return a reference to the non-modifiable "Sequence" selection of
    /// this object if "Sequence" is the current selection.  The behavior is
    /// undefined unless "Sequence" is the selection of this object.
    const AnonChoiceSequence& sequence() const;

    /// Return `true` if the value of this object is a "Sequence" value, and
    /// return `false` otherwise.
    bool isSequenceValue() const;

    /// Return `true` if the value of this object is undefined, and `false`
    /// otherwise.
    bool isUndefinedValue() const;

    /// Return the symbolic name of the current selection of this object.
    const char *selectionName() const;

    // HIDDEN FRIENDS

    /// Return `true` if the specified `lhs` and `rhs` objects have the same
    /// value, and `false` otherwise.  Two `AnonChoice` objects have the
    /// same value if either the selections in both objects have the same
    /// ids and the same values, or both selections are undefined.
    friend bool operator==(const AnonChoice& lhs, const AnonChoice& rhs)
    {
        return lhs.isEqualTo(rhs);
    }

    /// Return `true` if the specified `lhs` and `rhs` objects do not have
    /// the same values, as determined by `operator==`, and `false`
    /// otherwise.
    friend bool operator!=(const AnonChoice& lhs, const AnonChoice& rhs)
    {
        return !(lhs == rhs);
    }

    /// Format the specified `rhs` to the specified output `stream` and
    /// return a reference to the modifiable `stream`.
    friend bsl::ostream& operator<<(bsl::ostream&     stream,
                                    const AnonChoice& rhs)
    {
        return rhs.print(stream, 0, -1);
    }
};

}  // close package namespace

// TRAITS

BDLAT_DECL_CHOICE_WITH_BITWISEMOVEABLE_TRAITS(s_baltst::AnonChoice)

// ============================================================================
//                          INLINE DEFINITIONS
// ============================================================================

namespace s_baltst {

                          // ------------------------
                          // class AnonChoiceSequence
                          // ------------------------

// CLASS METHODS
// MANIPULATORS
template <typename t_MANIPULATOR>
int AnonChoiceSequence::manipulateAttributes(t_MANIPULATOR& manipulator)
{
    int ret;

    ret = manipulator(&d_a, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_A]);
    if (ret) {
        return ret;
    }

    return 0;
}

template <typename t_MANIPULATOR>
int AnonChoiceSequence::manipulateAttribute(t_MANIPULATOR& manipulator, int id)
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_A: {
        return manipulator(&d_a, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_A]);
      }
      default:
        return NOT_FOUND;
    }
}

template <typename t_MANIPULATOR>
int AnonChoiceSequence::manipulateAttribute(
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
int& AnonChoiceSequence::a()
{
    return d_a;
}

// ACCESSORS
template <typename t_ACCESSOR>
int AnonChoiceSequence::accessAttributes(t_ACCESSOR& accessor) const
{
    int ret;

    ret = accessor(d_a, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_A]);
    if (ret) {
        return ret;
    }

    return 0;
}

template <typename t_ACCESSOR>
int AnonChoiceSequence::accessAttribute(t_ACCESSOR& accessor, int id) const
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_A: {
        return accessor(d_a, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_A]);
      }
      default:
        return NOT_FOUND;
    }
}

template <typename t_ACCESSOR>
int AnonChoiceSequence::accessAttribute(
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
int AnonChoiceSequence::a() const
{
    return d_a;
}



                              // ----------------
                              // class AnonChoice
                              // ----------------

// CLASS METHODS
// PRIVATE ACCESSORS
inline
bool AnonChoice::isEqualTo(const AnonChoice& rhs) const
{
    typedef AnonChoice Class;
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
AnonChoice::AnonChoice()
: d_selectionId(SELECTION_ID_UNDEFINED)
{
}

inline
AnonChoice::~AnonChoice()
{
    reset();
}

// MANIPULATORS
template <typename t_MANIPULATOR>
int AnonChoice::manipulateSelection(t_MANIPULATOR& manipulator)
{
    switch (d_selectionId) {
      case AnonChoice::SELECTION_ID_SEQUENCE:
        return manipulator(&d_sequence.object(),
                SELECTION_INFO_ARRAY[SELECTION_INDEX_SEQUENCE]);
      default:
        BSLS_ASSERT(AnonChoice::SELECTION_ID_UNDEFINED == d_selectionId);
        return -1;
    }
}

inline
AnonChoiceSequence& AnonChoice::sequence()
{
    BSLS_ASSERT(SELECTION_ID_SEQUENCE == d_selectionId);
    return d_sequence.object();
}

// ACCESSORS
inline
int AnonChoice::selectionId() const
{
    return d_selectionId;
}

template <typename t_ACCESSOR>
int AnonChoice::accessSelection(t_ACCESSOR& accessor) const
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
const AnonChoiceSequence& AnonChoice::sequence() const
{
    BSLS_ASSERT(SELECTION_ID_SEQUENCE == d_selectionId);
    return d_sequence.object();
}

inline
bool AnonChoice::isSequenceValue() const
{
    return SELECTION_ID_SEQUENCE == d_selectionId;
}

inline
bool AnonChoice::isUndefinedValue() const
{
    return SELECTION_ID_UNDEFINED == d_selectionId;
}
}  // close package namespace

// FREE FUNCTIONS

}  // close enterprise namespace
#endif

// GENERATED BY BLP_BAS_CODEGEN_2025.03.13
// USING bas_codegen.pl s_baltst_anonymouschoicesequence.xsd --mode msg --includedir . --msgComponent anonymouschoicesequence --noRecurse --noExternalization --noHashSupport --noAggregateConversion
// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright 2025 Bloomberg Finance L.P. All rights reserved.
//      Property of Bloomberg Finance L.P. (BFLP)
//      This software is made available solely pursuant to the
//      terms of a BFLP license agreement which governs its use.
// ------------------------------- END-OF-FILE --------------------------------
