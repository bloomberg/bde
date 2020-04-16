// s_baltst_sequencewithanonymity.h         *DO NOT EDIT*          @generated -*-C++-*-
#ifndef INCLUDED_S_BALTST_SEQUENCEWITHANONYMITY
#define INCLUDED_S_BALTST_SEQUENCEWITHANONYMITY

#include <bsls_ident.h>
BSLS_IDENT_RCSID(s_baltst_sequencewithanonymity_h,"$Id$ $CSID$")
BSLS_IDENT_PRAGMA_ONCE

//@PURPOSE: Provide value-semantic attribute classes

#include <bslalg_typetraits.h>

#include <bdlat_attributeinfo.h>

#include <bdlat_selectioninfo.h>

#include <bdlat_typetraits.h>

#include <bsls_objectbuffer.h>

#include <bslma_default.h>

#include <bsls_assert.h>

#include <bdlb_nullablevalue.h>

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

#include <s_baltst_sequencewithanonymitychoice2.h>

#include <bsl_iosfwd.h>
#include <bsl_limits.h>

namespace BloombergLP {

namespace bslma { class Allocator; }

namespace s_baltst { class SequenceWithAnonymity; }
namespace s_baltst {

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
        ATTRIBUTE_ID_CHOICE   = 0
      , ATTRIBUTE_ID_CHOICE1  = 1
      , ATTRIBUTE_ID_CHOICE2  = 2
      , ATTRIBUTE_ID_ELEMENT4 = 3
    };

    enum {
        NUM_ATTRIBUTES = 4
    };

    enum {
        ATTRIBUTE_INDEX_CHOICE   = 0
      , ATTRIBUTE_INDEX_CHOICE1  = 1
      , ATTRIBUTE_INDEX_CHOICE2  = 2
      , ATTRIBUTE_INDEX_ELEMENT4 = 3
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

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    SequenceWithAnonymity(SequenceWithAnonymity&& original) noexcept;
        // Create an object of type 'SequenceWithAnonymity' having the value of
        // the specified 'original' object.  After performing this action, the
        // 'original' object will be left in a valid, but unspecified state.

    SequenceWithAnonymity(SequenceWithAnonymity&& original,
                          bslma::Allocator *basicAllocator);
        // Create an object of type 'SequenceWithAnonymity' having the value of
        // the specified 'original' object.  After performing this action, the
        // 'original' object will be left in a valid, but unspecified state.
        // Use the optionally specified 'basicAllocator' to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.
#endif

    ~SequenceWithAnonymity();
        // Destroy this object.

    // MANIPULATORS
    SequenceWithAnonymity& operator=(const SequenceWithAnonymity& rhs);
        // Assign to this object the value of the specified 'rhs' object.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    SequenceWithAnonymity& operator=(SequenceWithAnonymity&& rhs);
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

BDLAT_DECL_SEQUENCE_WITH_ALLOCATOR_BITWISEMOVEABLE_TRAITS(s_baltst::SequenceWithAnonymity)

// ============================================================================
//                         INLINE FUNCTION DEFINITIONS
// ============================================================================

namespace s_baltst {

                        // ---------------------------
                        // class SequenceWithAnonymity
                        // ---------------------------

// CLASS METHODS
// MANIPULATORS
template <class MANIPULATOR>
int SequenceWithAnonymity::manipulateAttributes(MANIPULATOR& manipulator)
{
    int ret;

    ret = manipulator(&d_choice, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CHOICE]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_choice1, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CHOICE1]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_choice2, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CHOICE2]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_element4, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT4]);
    if (ret) {
        return ret;
    }

    return ret;
}

template <class MANIPULATOR>
int SequenceWithAnonymity::manipulateAttribute(MANIPULATOR& manipulator, int id)
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_CHOICE: {
        return manipulator(&d_choice, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CHOICE]);
      }
      case ATTRIBUTE_ID_CHOICE1: {
        return manipulator(&d_choice1, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CHOICE1]);
      }
      case ATTRIBUTE_ID_CHOICE2: {
        return manipulator(&d_choice2, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CHOICE2]);
      }
      case ATTRIBUTE_ID_ELEMENT4: {
        return manipulator(&d_element4, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT4]);
      }
      default:
        return NOT_FOUND;
    }
}

template <class MANIPULATOR>
int SequenceWithAnonymity::manipulateAttribute(
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

    ret = accessor(d_choice, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CHOICE]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_choice1, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CHOICE1]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_choice2, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CHOICE2]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_element4, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT4]);
    if (ret) {
        return ret;
    }

    return ret;
}

template <class ACCESSOR>
int SequenceWithAnonymity::accessAttribute(ACCESSOR& accessor, int id) const
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_CHOICE: {
        return accessor(d_choice, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CHOICE]);
      }
      case ATTRIBUTE_ID_CHOICE1: {
        return accessor(d_choice1, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CHOICE1]);
      }
      case ATTRIBUTE_ID_CHOICE2: {
        return accessor(d_choice2, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CHOICE2]);
      }
      case ATTRIBUTE_ID_ELEMENT4: {
        return accessor(d_element4, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT4]);
      }
      default:
        return NOT_FOUND;
    }
}

template <class ACCESSOR>
int SequenceWithAnonymity::accessAttribute(
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

}  // close package namespace

// FREE FUNCTIONS

inline
bool s_baltst::operator==(
        const s_baltst::SequenceWithAnonymity& lhs,
        const s_baltst::SequenceWithAnonymity& rhs)
{
    return  lhs.choice() == rhs.choice()
         && lhs.choice1() == rhs.choice1()
         && lhs.choice2() == rhs.choice2()
         && lhs.element4() == rhs.element4();
}

inline
bool s_baltst::operator!=(
        const s_baltst::SequenceWithAnonymity& lhs,
        const s_baltst::SequenceWithAnonymity& rhs)
{
    return !(lhs == rhs);
}

inline
bsl::ostream& s_baltst::operator<<(
        bsl::ostream& stream,
        const s_baltst::SequenceWithAnonymity& rhs)
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
