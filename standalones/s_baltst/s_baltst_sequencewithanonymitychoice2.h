// s_baltst_sequencewithanonymitychoice2.h         *DO NOT EDIT*          @generated -*-C++-*-
#ifndef INCLUDED_S_BALTST_SEQUENCEWITHANONYMITYCHOICE2
#define INCLUDED_S_BALTST_SEQUENCEWITHANONYMITYCHOICE2

#include <bsls_ident.h>
BSLS_IDENT_RCSID(s_baltst_sequencewithanonymitychoice2_h,"$Id$ $CSID$")
BSLS_IDENT_PRAGMA_ONCE

//@PURPOSE: Provide value-semantic attribute classes

#include <bslalg_typetraits.h>

#include <bdlat_attributeinfo.h>

#include <bdlat_selectioninfo.h>

#include <bdlat_typetraits.h>

#include <bsls_objectbuffer.h>

#include <bslma_default.h>

#include <bsls_assert.h>

#include <s_baltst_address.h>

#include <s_baltst_basicrecord.h>

#include <s_baltst_customint.h>

#include <s_baltst_customstring.h>

#include <s_baltst_customizedstring.h>

#include <s_baltst_enumerated.h>

#include <s_baltst_mychoice.h>

#include <s_baltst_myenumeration.h>

#include <s_baltst_mysequence.h>

#include <s_baltst_mysequencewithanonymouschoicechoice.h>

#include <s_baltst_mysequencewitharray.h>

#include <s_baltst_mysequencewithattributes.h>

#include <s_baltst_mysequencewithnillable.h>

#include <s_baltst_mysequencewithnullable.h>

#include <s_baltst_mysimplecontent.h>

#include <s_baltst_mysimpleintcontent.h>

#include <s_baltst_rawdata.h>

#include <s_baltst_rawdataswitched.h>

#include <s_baltst_rawdataunformatted.h>

#include <s_baltst_sequencewithanonymitychoice1.h>

#include <s_baltst_simplerequest.h>

#include <s_baltst_sqrt.h>

#include <s_baltst_unsignedsequence.h>

#include <s_baltst_voidsequence.h>

#include <s_baltst_bigrecord.h>

#include <s_baltst_employee.h>

#include <s_baltst_mysequencewithanonymouschoice.h>

#include <s_baltst_mysequencewithnillables.h>

#include <s_baltst_mysequencewithnullables.h>

#include <s_baltst_ratsnest.h>

#include <s_baltst_ratsnest.h>

#include <s_baltst_ratsnest.h>

#include <s_baltst_ratsnest.h>

#include <s_baltst_sequencewithanonymitychoice.h>

#include <s_baltst_timingrequest.h>

#include <s_baltst_ratsnest.h>

#include <s_baltst_ratsnest.h>

#include <s_baltst_ratsnest.h>

#include <s_baltst_ratsnest.h>

#include <s_baltst_ratsnest.h>

#include <bsl_iosfwd.h>
#include <bsl_limits.h>

namespace BloombergLP {

namespace bslma { class Allocator; }

namespace s_baltst { class SequenceWithAnonymityChoice2; }
namespace s_baltst {

                     // ==================================
                     // class SequenceWithAnonymityChoice2
                     // ==================================

class SequenceWithAnonymityChoice2 {

    // INSTANCE DATA
    union {
        Sequence4             *d_selection7;
        Choice2               *d_selection8;
    };

    int                         d_selectionId;
    bslma::Allocator           *d_allocator_p;

  public:
    // TYPES

    enum {
        SELECTION_ID_UNDEFINED  = -1
      , SELECTION_ID_SELECTION7 = 0
      , SELECTION_ID_SELECTION8 = 1
    };

    enum {
        NUM_SELECTIONS = 2
    };

    enum {
        SELECTION_INDEX_SELECTION7 = 0
      , SELECTION_INDEX_SELECTION8 = 1
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

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    SequenceWithAnonymityChoice2(SequenceWithAnonymityChoice2&& original) noexcept;
        // Create an object of type 'SequenceWithAnonymityChoice2' having the
        // value of the specified 'original' object.  After performing this
        // action, the 'original' object will be left in a valid, but
        // unspecified state.

    SequenceWithAnonymityChoice2(SequenceWithAnonymityChoice2&& original,
                                bslma::Allocator *basicAllocator);
        // Create an object of type 'SequenceWithAnonymityChoice2' having the
        // value of the specified 'original' object.  After performing this
        // action, the 'original' object will be left in a valid, but
        // unspecified state.  Use the optionally specified 'basicAllocator' to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.
#endif

    ~SequenceWithAnonymityChoice2();
        // Destroy this object.

    // MANIPULATORS
    SequenceWithAnonymityChoice2& operator=(const SequenceWithAnonymityChoice2& rhs);
        // Assign to this object the value of the specified 'rhs' object.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    SequenceWithAnonymityChoice2& operator=(SequenceWithAnonymityChoice2&& rhs);
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

    Sequence4& makeSelection7();
    Sequence4& makeSelection7(const Sequence4& value);
#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    Sequence4& makeSelection7(Sequence4&& value);
#endif
        // Set the value of this object to be a "Selection7" value.  Optionally
        // specify the 'value' of the "Selection7".  If 'value' is not
        // specified, the default "Selection7" value is used.

    Choice2& makeSelection8();
    Choice2& makeSelection8(const Choice2& value);
#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    Choice2& makeSelection8(Choice2&& value);
#endif
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

BDLAT_DECL_CHOICE_WITH_ALLOCATOR_BITWISEMOVEABLE_TRAITS(s_baltst::SequenceWithAnonymityChoice2)

// ============================================================================
//                         INLINE FUNCTION DEFINITIONS
// ============================================================================

namespace s_baltst {

                     // ----------------------------------
                     // class SequenceWithAnonymityChoice2
                     // ----------------------------------

// CLASS METHODS
// CREATORS
inline
SequenceWithAnonymityChoice2::SequenceWithAnonymityChoice2(bslma::Allocator *basicAllocator)
: d_selectionId(SELECTION_ID_UNDEFINED)
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
      case SequenceWithAnonymityChoice2::SELECTION_ID_SELECTION7:
        return manipulator(d_selection7,
                SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION7]);
      case SequenceWithAnonymityChoice2::SELECTION_ID_SELECTION8:
        return manipulator(d_selection8,
                SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION8]);
      default:
        BSLS_ASSERT(SequenceWithAnonymityChoice2::SELECTION_ID_UNDEFINED == d_selectionId);
        return -1;
    }
}

inline
Sequence4& SequenceWithAnonymityChoice2::selection7()
{
    BSLS_ASSERT(SELECTION_ID_SELECTION7 == d_selectionId);
    return *d_selection7;
}

inline
Choice2& SequenceWithAnonymityChoice2::selection8()
{
    BSLS_ASSERT(SELECTION_ID_SELECTION8 == d_selectionId);
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
      case SELECTION_ID_SELECTION7:
        return accessor(*d_selection7,
                SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION7]);
      case SELECTION_ID_SELECTION8:
        return accessor(*d_selection8,
                SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION8]);
      default:
        BSLS_ASSERT(SELECTION_ID_UNDEFINED == d_selectionId);
        return -1;
    }
}

inline
const Sequence4& SequenceWithAnonymityChoice2::selection7() const
{
    BSLS_ASSERT(SELECTION_ID_SELECTION7 == d_selectionId);
    return *d_selection7;
}

inline
const Choice2& SequenceWithAnonymityChoice2::selection8() const
{
    BSLS_ASSERT(SELECTION_ID_SELECTION8 == d_selectionId);
    return *d_selection8;
}

inline
bool SequenceWithAnonymityChoice2::isSelection7Value() const
{
    return SELECTION_ID_SELECTION7 == d_selectionId;
}

inline
bool SequenceWithAnonymityChoice2::isSelection8Value() const
{
    return SELECTION_ID_SELECTION8 == d_selectionId;
}

inline
bool SequenceWithAnonymityChoice2::isUndefinedValue() const
{
    return SELECTION_ID_UNDEFINED == d_selectionId;
}
}  // close package namespace

// FREE FUNCTIONS

inline
bool s_baltst::operator==(
        const s_baltst::SequenceWithAnonymityChoice2& lhs,
        const s_baltst::SequenceWithAnonymityChoice2& rhs)
{
    typedef s_baltst::SequenceWithAnonymityChoice2 Class;
    if (lhs.selectionId() == rhs.selectionId()) {
        switch (rhs.selectionId()) {
          case Class::SELECTION_ID_SELECTION7:
            return lhs.selection7() == rhs.selection7();
          case Class::SELECTION_ID_SELECTION8:
            return lhs.selection8() == rhs.selection8();
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
        const s_baltst::SequenceWithAnonymityChoice2& lhs,
        const s_baltst::SequenceWithAnonymityChoice2& rhs)
{
    return !(lhs == rhs);
}

inline
bsl::ostream& s_baltst::operator<<(
        bsl::ostream& stream,
        const s_baltst::SequenceWithAnonymityChoice2& rhs)
{
    return rhs.print(stream, 0, -1);
}

}  // close enterprise namespace
#endif

// GENERATED BY BLP_BAS_CODEGEN_2020.03.30
// USING bas_codegen.pl s_baltst.xsd -m msg -p s_baltst -C tmp_singles --msgSplit 1 --noExternalization --noHashSupport --noAggregateConversion
// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright 2020 Bloomberg Finance L.P. All rights reserved.
//      Property of Bloomberg Finance L.P. (BFLP)
//      This software is made available solely pursuant to the
//      terms of a BFLP license agreement which governs its use.
// ------------------------------- END-OF-FILE --------------------------------
