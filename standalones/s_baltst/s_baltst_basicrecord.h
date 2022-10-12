// s_baltst_basicrecord.h          *DO NOT EDIT*           @generated -*-C++-*-
#ifndef INCLUDED_S_BALTST_BASICRECORD
#define INCLUDED_S_BALTST_BASICRECORD

#include <bsls_ident.h>
BSLS_IDENT_RCSID(s_baltst_basicrecord_h, "$Id$ $CSID$")
BSLS_IDENT_PRAGMA_ONCE

//@PURPOSE: Provide value-semantic attribute classes

#include <bslalg_typetraits.h>

#include <bdlat_attributeinfo.h>

#include <bdlat_selectioninfo.h>

#include <bdlat_typetraits.h>

#include <bsls_objectbuffer.h>

#include <bslma_default.h>

#include <bsls_assert.h>

#include <bdlt_datetimetz.h>

#include <bsl_string.h>

#include <bsl_iosfwd.h>
#include <bsl_limits.h>

namespace BloombergLP {

namespace bslma { class Allocator; }

namespace s_baltst { class BasicRecord; }
namespace s_baltst {

                             // =================
                             // class BasicRecord
                             // =================

class BasicRecord {
    // A representative small record type

    // INSTANCE DATA
    bsl::string       d_s;
    bdlt::DatetimeTz  d_dt;
    int               d_i1;
    int               d_i2;

  public:
    // TYPES
    enum {
        ATTRIBUTE_ID_I1 = 0
      , ATTRIBUTE_ID_I2 = 1
      , ATTRIBUTE_ID_DT = 2
      , ATTRIBUTE_ID_S  = 3
    };

    enum {
        NUM_ATTRIBUTES = 4
    };

    enum {
        ATTRIBUTE_INDEX_I1 = 0
      , ATTRIBUTE_INDEX_I2 = 1
      , ATTRIBUTE_INDEX_DT = 2
      , ATTRIBUTE_INDEX_S  = 3
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
    explicit BasicRecord(bslma::Allocator *basicAllocator = 0);
        // Create an object of type 'BasicRecord' having the default value.
        // Use the optionally specified 'basicAllocator' to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.

    BasicRecord(const BasicRecord& original,
                bslma::Allocator *basicAllocator = 0);
        // Create an object of type 'BasicRecord' having the value of the
        // specified 'original' object.  Use the optionally specified
        // 'basicAllocator' to supply memory.  If 'basicAllocator' is 0, the
        // currently installed default allocator is used.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    BasicRecord(BasicRecord&& original) noexcept;
        // Create an object of type 'BasicRecord' having the value of the
        // specified 'original' object.  After performing this action, the
        // 'original' object will be left in a valid, but unspecified state.

    BasicRecord(BasicRecord&& original,
                bslma::Allocator *basicAllocator);
        // Create an object of type 'BasicRecord' having the value of the
        // specified 'original' object.  After performing this action, the
        // 'original' object will be left in a valid, but unspecified state.
        // Use the optionally specified 'basicAllocator' to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.
#endif

    ~BasicRecord();
        // Destroy this object.

    // MANIPULATORS
    BasicRecord& operator=(const BasicRecord& rhs);
        // Assign to this object the value of the specified 'rhs' object.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    BasicRecord& operator=(BasicRecord&& rhs);
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

    int& i1();
        // Return a reference to the modifiable "I1" attribute of this object.

    int& i2();
        // Return a reference to the modifiable "I2" attribute of this object.

    bdlt::DatetimeTz& dt();
        // Return a reference to the modifiable "Dt" attribute of this object.

    bsl::string& s();
        // Return a reference to the modifiable "S" attribute of this object.

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

    int i1() const;
        // Return the value of the "I1" attribute of this object.

    int i2() const;
        // Return the value of the "I2" attribute of this object.

    const bdlt::DatetimeTz& dt() const;
        // Return a reference offering non-modifiable access to the "Dt"
        // attribute of this object.

    const bsl::string& s() const;
        // Return a reference offering non-modifiable access to the "S"
        // attribute of this object.
};

// FREE OPERATORS
inline
bool operator==(const BasicRecord& lhs, const BasicRecord& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
    // the same value, and 'false' otherwise.  Two attribute objects have the
    // same value if each respective attribute has the same value.

inline
bool operator!=(const BasicRecord& lhs, const BasicRecord& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
    // have the same value, and 'false' otherwise.  Two attribute objects do
    // not have the same value if one or more respective attributes differ in
    // values.

inline
bsl::ostream& operator<<(bsl::ostream& stream, const BasicRecord& rhs);
    // Format the specified 'rhs' to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.

}  // close package namespace

// TRAITS

BDLAT_DECL_SEQUENCE_WITH_ALLOCATOR_BITWISEMOVEABLE_TRAITS(s_baltst::BasicRecord)

// ============================================================================
//                         INLINE FUNCTION DEFINITIONS
// ============================================================================

namespace s_baltst {

                             // -----------------
                             // class BasicRecord
                             // -----------------

// CLASS METHODS
// MANIPULATORS
template <class MANIPULATOR>
int BasicRecord::manipulateAttributes(MANIPULATOR& manipulator)
{
    int ret;

    ret = manipulator(&d_i1, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_I1]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_i2, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_I2]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_dt, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_DT]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_s, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_S]);
    if (ret) {
        return ret;
    }

    return 0;
}

template <class MANIPULATOR>
int BasicRecord::manipulateAttribute(MANIPULATOR& manipulator, int id)
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_I1: {
        return manipulator(&d_i1, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_I1]);
      }
      case ATTRIBUTE_ID_I2: {
        return manipulator(&d_i2, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_I2]);
      }
      case ATTRIBUTE_ID_DT: {
        return manipulator(&d_dt, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_DT]);
      }
      case ATTRIBUTE_ID_S: {
        return manipulator(&d_s, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_S]);
      }
      default:
        return NOT_FOUND;
    }
}

template <class MANIPULATOR>
int BasicRecord::manipulateAttribute(
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
int& BasicRecord::i1()
{
    return d_i1;
}

inline
int& BasicRecord::i2()
{
    return d_i2;
}

inline
bdlt::DatetimeTz& BasicRecord::dt()
{
    return d_dt;
}

inline
bsl::string& BasicRecord::s()
{
    return d_s;
}

// ACCESSORS
template <class ACCESSOR>
int BasicRecord::accessAttributes(ACCESSOR& accessor) const
{
    int ret;

    ret = accessor(d_i1, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_I1]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_i2, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_I2]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_dt, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_DT]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_s, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_S]);
    if (ret) {
        return ret;
    }

    return 0;
}

template <class ACCESSOR>
int BasicRecord::accessAttribute(ACCESSOR& accessor, int id) const
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_I1: {
        return accessor(d_i1, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_I1]);
      }
      case ATTRIBUTE_ID_I2: {
        return accessor(d_i2, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_I2]);
      }
      case ATTRIBUTE_ID_DT: {
        return accessor(d_dt, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_DT]);
      }
      case ATTRIBUTE_ID_S: {
        return accessor(d_s, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_S]);
      }
      default:
        return NOT_FOUND;
    }
}

template <class ACCESSOR>
int BasicRecord::accessAttribute(
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
int BasicRecord::i1() const
{
    return d_i1;
}

inline
int BasicRecord::i2() const
{
    return d_i2;
}

inline
const bdlt::DatetimeTz& BasicRecord::dt() const
{
    return d_dt;
}

inline
const bsl::string& BasicRecord::s() const
{
    return d_s;
}

}  // close package namespace

// FREE FUNCTIONS

inline
bool s_baltst::operator==(
        const s_baltst::BasicRecord& lhs,
        const s_baltst::BasicRecord& rhs)
{
    return  lhs.i1() == rhs.i1()
         && lhs.i2() == rhs.i2()
         && lhs.dt() == rhs.dt()
         && lhs.s() == rhs.s();
}

inline
bool s_baltst::operator!=(
        const s_baltst::BasicRecord& lhs,
        const s_baltst::BasicRecord& rhs)
{
    return !(lhs == rhs);
}

inline
bsl::ostream& s_baltst::operator<<(
        bsl::ostream& stream,
        const s_baltst::BasicRecord& rhs)
{
    return rhs.print(stream, 0, -1);
}

}  // close enterprise namespace
#endif

// GENERATED BY @BLP_BAS_CODEGEN_VERSION@
// USING bas_codegen.pl s_baltst_basicrecord.xsd --mode msg --includedir . --msgComponent basicrecord --noRecurse --noExternalization --noHashSupport --noAggregateConversion
// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright 2022 Bloomberg Finance L.P. All rights reserved.
//      Property of Bloomberg Finance L.P. (BFLP)
//      This software is made available solely pursuant to the
//      terms of a BFLP license agreement which governs its use.
// ------------------------------- END-OF-FILE --------------------------------
