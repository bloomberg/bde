// s_baltst_mychoice.h            *DO NOT EDIT*            @generated -*-C++-*-
#ifndef INCLUDED_S_BALTST_MYCHOICE
#define INCLUDED_S_BALTST_MYCHOICE

#include <bsls_ident.h>
BSLS_IDENT_RCSID(s_baltst_mychoice_h, "$Id$ $CSID$")
BSLS_IDENT_PRAGMA_ONCE

//@PURPOSE: Provide value-semantic attribute classes

#include <bslalg_typetraits.h>

#include <bdlat_attributeinfo.h>

#include <bdlat_selectioninfo.h>

#include <bdlat_typetraits.h>

#include <bsls_objectbuffer.h>

#include <bslma_default.h>

#include <bsls_assert.h>

#include <bsl_string.h>

#include <bsl_iosfwd.h>
#include <bsl_limits.h>

namespace BloombergLP {

namespace bslma { class Allocator; }

namespace s_baltst { class MyChoice; }
namespace s_baltst {

                               // ==============
                               // class MyChoice
                               // ==============

class MyChoice {

    // INSTANCE DATA
    union {
        bsls::ObjectBuffer< int >         d_selection1;
        bsls::ObjectBuffer< bsl::string > d_selection2;
    };

    int                                   d_selectionId;
    bslma::Allocator                     *d_allocator_p;

    // PRIVATE ACCESSORS
    bool isEqualTo(const MyChoice& rhs) const;

  public:
    // TYPES

    enum {
        SELECTION_ID_UNDEFINED  = -1
      , SELECTION_ID_SELECTION1 = 0
      , SELECTION_ID_SELECTION2 = 1
    };

    enum {
        NUM_SELECTIONS = 2
    };

    enum {
        SELECTION_INDEX_SELECTION1 = 0
      , SELECTION_INDEX_SELECTION2 = 1
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

    /// Create an object of type `MyChoice` having the default value.  Use
    /// the optionally specified `basicAllocator` to supply memory.  If
    /// `basicAllocator` is 0, the currently installed default allocator is
    /// used.
    explicit MyChoice(bslma::Allocator *basicAllocator = 0);

    /// Create an object of type `MyChoice` having the value of the
    /// specified `original` object.  Use the optionally specified
    /// `basicAllocator` to supply memory.  If `basicAllocator` is 0, the
    /// currently installed default allocator is used.
    MyChoice(const MyChoice& original,
            bslma::Allocator *basicAllocator = 0);

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    /// Create an object of type `MyChoice` having the value of the
    /// specified `original` object.  After performing this action, the
    /// `original` object will be left in a valid, but unspecified state.
    MyChoice(MyChoice&& original) noexcept;

    /// Create an object of type `MyChoice` having the value of the
    /// specified `original` object.  After performing this action, the
    /// `original` object will be left in a valid, but unspecified state.
    /// Use the optionally specified `basicAllocator` to supply memory.  If
    /// `basicAllocator` is 0, the currently installed default allocator is
    /// used.
    MyChoice(MyChoice&& original,
            bslma::Allocator *basicAllocator);
#endif

    /// Destroy this object.
    ~MyChoice();

    // MANIPULATORS

    /// Assign to this object the value of the specified `rhs` object.
    MyChoice& operator=(const MyChoice& rhs);

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    /// Assign to this object the value of the specified `rhs` object.
    /// After performing this action, the `rhs` object will be left in a
    /// valid, but unspecified state.
    MyChoice& operator=(MyChoice&& rhs);
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

    /// Set the value of this object to be a "Selection1" value.  Optionally
    /// specify the `value` of the "Selection1".  If `value` is not
    /// specified, the default "Selection1" value is used.
    int& makeSelection1();
    int& makeSelection1(int value);

    bsl::string& makeSelection2();
    bsl::string& makeSelection2(const bsl::string& value);
#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    bsl::string& makeSelection2(bsl::string&& value);
#endif
        // Set the value of this object to be a "Selection2" value.  Optionally
        // specify the 'value' of the "Selection2".  If 'value' is not
        // specified, the default "Selection2" value is used.

    /// Invoke the specified `manipulator` on the address of the modifiable
    /// selection, supplying `manipulator` with the corresponding selection
    /// information structure.  Return the value returned from the
    /// invocation of `manipulator` if this object has a defined selection,
    /// and -1 otherwise.
    template <typename t_MANIPULATOR>
    int manipulateSelection(t_MANIPULATOR& manipulator);

    /// Return a reference to the modifiable "Selection1" selection of this
    /// object if "Selection1" is the current selection.  The behavior is
    /// undefined unless "Selection1" is the selection of this object.
    int& selection1();

    /// Return a reference to the modifiable "Selection2" selection of this
    /// object if "Selection2" is the current selection.  The behavior is
    /// undefined unless "Selection2" is the selection of this object.
    bsl::string& selection2();

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

    /// Return a reference to the non-modifiable "Selection1" selection of
    /// this object if "Selection1" is the current selection.  The behavior
    /// is undefined unless "Selection1" is the selection of this object.
    const int& selection1() const;

    /// Return a reference to the non-modifiable "Selection2" selection of
    /// this object if "Selection2" is the current selection.  The behavior
    /// is undefined unless "Selection2" is the selection of this object.
    const bsl::string& selection2() const;

    /// Return `true` if the value of this object is a "Selection1" value,
    /// and return `false` otherwise.
    bool isSelection1Value() const;

    /// Return `true` if the value of this object is a "Selection2" value,
    /// and return `false` otherwise.
    bool isSelection2Value() const;

    /// Return `true` if the value of this object is undefined, and `false`
    /// otherwise.
    bool isUndefinedValue() const;

    /// Return the symbolic name of the current selection of this object.
    const char *selectionName() const;

    // HIDDEN FRIENDS

    /// Return `true` if the specified `lhs` and `rhs` objects have the same
    /// value, and `false` otherwise.  Two `MyChoice` objects have the same
    /// value if either the selections in both objects have the same ids and
    /// the same values, or both selections are undefined.
    friend bool operator==(const MyChoice& lhs, const MyChoice& rhs)
    {
        return lhs.isEqualTo(rhs);
    }

    /// Return `true` if the specified `lhs` and `rhs` objects do not have
    /// the same values, as determined by `operator==`, and `false`
    /// otherwise.
    friend bool operator!=(const MyChoice& lhs, const MyChoice& rhs)
    {
        return !(lhs == rhs);
    }

    /// Format the specified `rhs` to the specified output `stream` and
    /// return a reference to the modifiable `stream`.
    friend bsl::ostream& operator<<(bsl::ostream& stream, const MyChoice& rhs)
    {
        return rhs.print(stream, 0, -1);
    }
};

}  // close package namespace

// TRAITS

BDLAT_DECL_CHOICE_WITH_ALLOCATOR_BITWISEMOVEABLE_TRAITS(s_baltst::MyChoice)

// ============================================================================
//                          INLINE DEFINITIONS
// ============================================================================

namespace s_baltst {

                               // --------------
                               // class MyChoice
                               // --------------

// CLASS METHODS
// PRIVATE ACCESSORS
inline
bool MyChoice::isEqualTo(const MyChoice& rhs) const
{
    typedef MyChoice Class;
    if (this->selectionId() == rhs.selectionId()) {
        switch (rhs.selectionId()) {
          case Class::SELECTION_ID_SELECTION1:
            return this->selection1() == rhs.selection1();
          case Class::SELECTION_ID_SELECTION2:
            return this->selection2() == rhs.selection2();
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
MyChoice::MyChoice(bslma::Allocator *basicAllocator)
: d_selectionId(SELECTION_ID_UNDEFINED)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

inline
MyChoice::~MyChoice()
{
    reset();
}

// MANIPULATORS
template <typename t_MANIPULATOR>
int MyChoice::manipulateSelection(t_MANIPULATOR& manipulator)
{
    switch (d_selectionId) {
      case MyChoice::SELECTION_ID_SELECTION1:
        return manipulator(&d_selection1.object(),
                SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION1]);
      case MyChoice::SELECTION_ID_SELECTION2:
        return manipulator(&d_selection2.object(),
                SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION2]);
      default:
        BSLS_ASSERT(MyChoice::SELECTION_ID_UNDEFINED == d_selectionId);
        return -1;
    }
}

inline
int& MyChoice::selection1()
{
    BSLS_ASSERT(SELECTION_ID_SELECTION1 == d_selectionId);
    return d_selection1.object();
}

inline
bsl::string& MyChoice::selection2()
{
    BSLS_ASSERT(SELECTION_ID_SELECTION2 == d_selectionId);
    return d_selection2.object();
}

// ACCESSORS
inline
int MyChoice::selectionId() const
{
    return d_selectionId;
}

template <typename t_ACCESSOR>
int MyChoice::accessSelection(t_ACCESSOR& accessor) const
{
    switch (d_selectionId) {
      case SELECTION_ID_SELECTION1:
        return accessor(d_selection1.object(),
                SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION1]);
      case SELECTION_ID_SELECTION2:
        return accessor(d_selection2.object(),
                SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION2]);
      default:
        BSLS_ASSERT(SELECTION_ID_UNDEFINED == d_selectionId);
        return -1;
    }
}

inline
const int& MyChoice::selection1() const
{
    BSLS_ASSERT(SELECTION_ID_SELECTION1 == d_selectionId);
    return d_selection1.object();
}

inline
const bsl::string& MyChoice::selection2() const
{
    BSLS_ASSERT(SELECTION_ID_SELECTION2 == d_selectionId);
    return d_selection2.object();
}

inline
bool MyChoice::isSelection1Value() const
{
    return SELECTION_ID_SELECTION1 == d_selectionId;
}

inline
bool MyChoice::isSelection2Value() const
{
    return SELECTION_ID_SELECTION2 == d_selectionId;
}

inline
bool MyChoice::isUndefinedValue() const
{
    return SELECTION_ID_UNDEFINED == d_selectionId;
}
}  // close package namespace

// FREE FUNCTIONS

}  // close enterprise namespace
#endif

// GENERATED BY BLP_BAS_CODEGEN_2025.08.21
// USING bas_codegen.pl s_baltst_mychoice.xsd --mode msg --includedir . --msgComponent mychoice --noRecurse --noExternalization --noHashSupport --noAggregateConversion
// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright 2025 Bloomberg Finance L.P. All rights reserved.
//      Property of Bloomberg Finance L.P. (BFLP)
//      This software is made available solely pursuant to the
//      terms of a BFLP license agreement which governs its use.
// ------------------------------- END-OF-FILE --------------------------------
