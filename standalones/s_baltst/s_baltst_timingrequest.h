// s_baltst_timingrequest.h         *DO NOT EDIT*          @generated -*-C++-*-
#ifndef INCLUDED_S_BALTST_TIMINGREQUEST
#define INCLUDED_S_BALTST_TIMINGREQUEST

#include <bsls_ident.h>
BSLS_IDENT_RCSID(s_baltst_timingrequest_h, "$Id$ $CSID$")
BSLS_IDENT_PRAGMA_ONCE

//@PURPOSE: Provide value-semantic attribute classes

#include <bslalg_typetraits.h>

#include <bdlat_attributeinfo.h>

#include <bdlat_selectioninfo.h>

#include <bdlat_typetraits.h>

#include <bsls_objectbuffer.h>

#include <bslma_default.h>

#include <bsls_assert.h>

#include <s_baltst_basicrecord.h>

#include <s_baltst_bigrecord.h>

#include <s_baltst_sqrt.h>

#include <bsl_iosfwd.h>
#include <bsl_limits.h>

namespace BloombergLP {

namespace bslma { class Allocator; }

namespace s_baltst { class TimingRequest; }
namespace s_baltst {

                            // ===================
                            // class TimingRequest
                            // ===================

class TimingRequest {
    // A choice record representative of a typical request object.

    // INSTANCE DATA
    union {
        bsls::ObjectBuffer< s_baltst::Sqrt >        d_sqrt;
        bsls::ObjectBuffer< s_baltst::BasicRecord > d_basic;
        bsls::ObjectBuffer< s_baltst::BigRecord >   d_big;
    };

    int                                             d_selectionId;
    bslma::Allocator                               *d_allocator_p;

  public:
    // TYPES

    enum {
        SELECTION_ID_UNDEFINED = -1
      , SELECTION_ID_SQRT  = 0
      , SELECTION_ID_BASIC = 1
      , SELECTION_ID_BIG   = 2
    };

    enum {
        NUM_SELECTIONS = 3
    };

    enum {
        SELECTION_INDEX_SQRT  = 0
      , SELECTION_INDEX_BASIC = 1
      , SELECTION_INDEX_BIG   = 2
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
    explicit TimingRequest(bslma::Allocator *basicAllocator = 0);
        // Create an object of type 'TimingRequest' having the default value.
        // Use the optionally specified 'basicAllocator' to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.

    TimingRequest(const TimingRequest& original,
                 bslma::Allocator *basicAllocator = 0);
        // Create an object of type 'TimingRequest' having the value of the
        // specified 'original' object.  Use the optionally specified
        // 'basicAllocator' to supply memory.  If 'basicAllocator' is 0, the
        // currently installed default allocator is used.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    TimingRequest(TimingRequest&& original) noexcept;
        // Create an object of type 'TimingRequest' having the value of the
        // specified 'original' object.  After performing this action, the
        // 'original' object will be left in a valid, but unspecified state.

    TimingRequest(TimingRequest&& original,
                 bslma::Allocator *basicAllocator);
        // Create an object of type 'TimingRequest' having the value of the
        // specified 'original' object.  After performing this action, the
        // 'original' object will be left in a valid, but unspecified state.
        // Use the optionally specified 'basicAllocator' to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.
#endif

    ~TimingRequest();
        // Destroy this object.

    // MANIPULATORS
    TimingRequest& operator=(const TimingRequest& rhs);
        // Assign to this object the value of the specified 'rhs' object.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    TimingRequest& operator=(TimingRequest&& rhs);
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

    s_baltst::Sqrt& makeSqrt();
    s_baltst::Sqrt& makeSqrt(const s_baltst::Sqrt& value);
#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    s_baltst::Sqrt& makeSqrt(s_baltst::Sqrt&& value);
#endif
        // Set the value of this object to be a "Sqrt" value.  Optionally
        // specify the 'value' of the "Sqrt".  If 'value' is not specified, the
        // default "Sqrt" value is used.

    s_baltst::BasicRecord& makeBasic();
    s_baltst::BasicRecord& makeBasic(const s_baltst::BasicRecord& value);
#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    s_baltst::BasicRecord& makeBasic(s_baltst::BasicRecord&& value);
#endif
        // Set the value of this object to be a "Basic" value.  Optionally
        // specify the 'value' of the "Basic".  If 'value' is not specified,
        // the default "Basic" value is used.

    s_baltst::BigRecord& makeBig();
    s_baltst::BigRecord& makeBig(const s_baltst::BigRecord& value);
#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    s_baltst::BigRecord& makeBig(s_baltst::BigRecord&& value);
#endif
        // Set the value of this object to be a "Big" value.  Optionally
        // specify the 'value' of the "Big".  If 'value' is not specified, the
        // default "Big" value is used.

    template<class MANIPULATOR>
    int manipulateSelection(MANIPULATOR& manipulator);
        // Invoke the specified 'manipulator' on the address of the modifiable
        // selection, supplying 'manipulator' with the corresponding selection
        // information structure.  Return the value returned from the
        // invocation of 'manipulator' if this object has a defined selection,
        // and -1 otherwise.

    s_baltst::Sqrt& sqrt();
        // Return a reference to the modifiable "Sqrt" selection of this object
        // if "Sqrt" is the current selection.  The behavior is undefined
        // unless "Sqrt" is the selection of this object.

    s_baltst::BasicRecord& basic();
        // Return a reference to the modifiable "Basic" selection of this
        // object if "Basic" is the current selection.  The behavior is
        // undefined unless "Basic" is the selection of this object.

    s_baltst::BigRecord& big();
        // Return a reference to the modifiable "Big" selection of this object
        // if "Big" is the current selection.  The behavior is undefined unless
        // "Big" is the selection of this object.

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

    const s_baltst::Sqrt& sqrt() const;
        // Return a reference to the non-modifiable "Sqrt" selection of this
        // object if "Sqrt" is the current selection.  The behavior is
        // undefined unless "Sqrt" is the selection of this object.

    const s_baltst::BasicRecord& basic() const;
        // Return a reference to the non-modifiable "Basic" selection of this
        // object if "Basic" is the current selection.  The behavior is
        // undefined unless "Basic" is the selection of this object.

    const s_baltst::BigRecord& big() const;
        // Return a reference to the non-modifiable "Big" selection of this
        // object if "Big" is the current selection.  The behavior is undefined
        // unless "Big" is the selection of this object.

    bool isSqrtValue() const;
        // Return 'true' if the value of this object is a "Sqrt" value, and
        // return 'false' otherwise.

    bool isBasicValue() const;
        // Return 'true' if the value of this object is a "Basic" value, and
        // return 'false' otherwise.

    bool isBigValue() const;
        // Return 'true' if the value of this object is a "Big" value, and
        // return 'false' otherwise.

    bool isUndefinedValue() const;
        // Return 'true' if the value of this object is undefined, and 'false'
        // otherwise.

    const char *selectionName() const;
        // Return the symbolic name of the current selection of this object.
};

// FREE OPERATORS
inline
bool operator==(const TimingRequest& lhs, const TimingRequest& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects have the same
    // value, and 'false' otherwise.  Two 'TimingRequest' objects have the same
    // value if either the selections in both objects have the same ids and
    // the same values, or both selections are undefined.

inline
bool operator!=(const TimingRequest& lhs, const TimingRequest& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects do not have the
    // same values, as determined by 'operator==', and 'false' otherwise.

inline
bsl::ostream& operator<<(bsl::ostream& stream, const TimingRequest& rhs);
    // Format the specified 'rhs' to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.

}  // close package namespace

// TRAITS

BDLAT_DECL_CHOICE_WITH_ALLOCATOR_BITWISEMOVEABLE_TRAITS(s_baltst::TimingRequest)

// ============================================================================
//                         INLINE FUNCTION DEFINITIONS
// ============================================================================

namespace s_baltst {

                            // -------------------
                            // class TimingRequest
                            // -------------------

// CLASS METHODS
// CREATORS
inline
TimingRequest::TimingRequest(bslma::Allocator *basicAllocator)
: d_selectionId(SELECTION_ID_UNDEFINED)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

inline
TimingRequest::~TimingRequest()
{
    reset();
}

// MANIPULATORS
template <class MANIPULATOR>
int TimingRequest::manipulateSelection(MANIPULATOR& manipulator)
{
    switch (d_selectionId) {
      case TimingRequest::SELECTION_ID_SQRT:
        return manipulator(&d_sqrt.object(),
                SELECTION_INFO_ARRAY[SELECTION_INDEX_SQRT]);
      case TimingRequest::SELECTION_ID_BASIC:
        return manipulator(&d_basic.object(),
                SELECTION_INFO_ARRAY[SELECTION_INDEX_BASIC]);
      case TimingRequest::SELECTION_ID_BIG:
        return manipulator(&d_big.object(),
                SELECTION_INFO_ARRAY[SELECTION_INDEX_BIG]);
      default:
        BSLS_ASSERT(TimingRequest::SELECTION_ID_UNDEFINED == d_selectionId);
        return -1;
    }
}

inline
s_baltst::Sqrt& TimingRequest::sqrt()
{
    BSLS_ASSERT(SELECTION_ID_SQRT == d_selectionId);
    return d_sqrt.object();
}

inline
s_baltst::BasicRecord& TimingRequest::basic()
{
    BSLS_ASSERT(SELECTION_ID_BASIC == d_selectionId);
    return d_basic.object();
}

inline
s_baltst::BigRecord& TimingRequest::big()
{
    BSLS_ASSERT(SELECTION_ID_BIG == d_selectionId);
    return d_big.object();
}

// ACCESSORS
inline
int TimingRequest::selectionId() const
{
    return d_selectionId;
}

template <class ACCESSOR>
int TimingRequest::accessSelection(ACCESSOR& accessor) const
{
    switch (d_selectionId) {
      case SELECTION_ID_SQRT:
        return accessor(d_sqrt.object(),
                SELECTION_INFO_ARRAY[SELECTION_INDEX_SQRT]);
      case SELECTION_ID_BASIC:
        return accessor(d_basic.object(),
                SELECTION_INFO_ARRAY[SELECTION_INDEX_BASIC]);
      case SELECTION_ID_BIG:
        return accessor(d_big.object(),
                SELECTION_INFO_ARRAY[SELECTION_INDEX_BIG]);
      default:
        BSLS_ASSERT(SELECTION_ID_UNDEFINED == d_selectionId);
        return -1;
    }
}

inline
const s_baltst::Sqrt& TimingRequest::sqrt() const
{
    BSLS_ASSERT(SELECTION_ID_SQRT == d_selectionId);
    return d_sqrt.object();
}

inline
const s_baltst::BasicRecord& TimingRequest::basic() const
{
    BSLS_ASSERT(SELECTION_ID_BASIC == d_selectionId);
    return d_basic.object();
}

inline
const s_baltst::BigRecord& TimingRequest::big() const
{
    BSLS_ASSERT(SELECTION_ID_BIG == d_selectionId);
    return d_big.object();
}

inline
bool TimingRequest::isSqrtValue() const
{
    return SELECTION_ID_SQRT == d_selectionId;
}

inline
bool TimingRequest::isBasicValue() const
{
    return SELECTION_ID_BASIC == d_selectionId;
}

inline
bool TimingRequest::isBigValue() const
{
    return SELECTION_ID_BIG == d_selectionId;
}

inline
bool TimingRequest::isUndefinedValue() const
{
    return SELECTION_ID_UNDEFINED == d_selectionId;
}
}  // close package namespace

// FREE FUNCTIONS

inline
bool s_baltst::operator==(
        const s_baltst::TimingRequest& lhs,
        const s_baltst::TimingRequest& rhs)
{
    typedef s_baltst::TimingRequest Class;
    if (lhs.selectionId() == rhs.selectionId()) {
        switch (rhs.selectionId()) {
          case Class::SELECTION_ID_SQRT:
            return lhs.sqrt() == rhs.sqrt();
          case Class::SELECTION_ID_BASIC:
            return lhs.basic() == rhs.basic();
          case Class::SELECTION_ID_BIG:
            return lhs.big() == rhs.big();
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
        const s_baltst::TimingRequest& lhs,
        const s_baltst::TimingRequest& rhs)
{
    return !(lhs == rhs);
}

inline
bsl::ostream& s_baltst::operator<<(
        bsl::ostream& stream,
        const s_baltst::TimingRequest& rhs)
{
    return rhs.print(stream, 0, -1);
}

}  // close enterprise namespace
#endif

// GENERATED BY @BLP_BAS_CODEGEN_VERSION@
// USING bas_codegen.pl s_baltst_timingrequest.xsd --mode msg --includedir . --msgComponent timingrequest --noRecurse --noExternalization --noHashSupport --noAggregateConversion
// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright 2022 Bloomberg Finance L.P. All rights reserved.
//      Property of Bloomberg Finance L.P. (BFLP)
//      This software is made available solely pursuant to the
//      terms of a BFLP license agreement which governs its use.
// ------------------------------- END-OF-FILE --------------------------------
