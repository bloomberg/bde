// s_baltst_rawdataunformatted.h       *DO NOT EDIT*       @generated -*-C++-*-
#ifndef INCLUDED_S_BALTST_RAWDATAUNFORMATTED
#define INCLUDED_S_BALTST_RAWDATAUNFORMATTED

#include <bsls_ident.h>
BSLS_IDENT_RCSID(s_baltst_rawdataunformatted_h, "$Id$ $CSID$")
BSLS_IDENT_PRAGMA_ONCE

//@PURPOSE: Provide value-semantic attribute classes

#include <bslalg_typetraits.h>

#include <bdlat_attributeinfo.h>

#include <bdlat_selectioninfo.h>

#include <bdlat_typetraits.h>

#include <bsls_objectbuffer.h>

#include <bslma_default.h>

#include <bsls_assert.h>

#include <bsl_vector.h>

#include <bsl_iosfwd.h>
#include <bsl_limits.h>

namespace BloombergLP {

namespace bslma { class Allocator; }

namespace s_baltst { class RawDataUnformatted; }
namespace s_baltst {

                          // ========================
                          // class RawDataUnformatted
                          // ========================

class RawDataUnformatted {

    // INSTANCE DATA
    bsl::vector<unsigned char>  d_ucharvec;
    bsl::vector<char>           d_charvec;

  public:
    // TYPES
    enum {
        ATTRIBUTE_ID_CHARVEC  = 0
      , ATTRIBUTE_ID_UCHARVEC = 1
    };

    enum {
        NUM_ATTRIBUTES = 2
    };

    enum {
        ATTRIBUTE_INDEX_CHARVEC  = 0
      , ATTRIBUTE_INDEX_UCHARVEC = 1
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
    explicit RawDataUnformatted(bslma::Allocator *basicAllocator = 0);
        // Create an object of type 'RawDataUnformatted' having the default
        // value.  Use the optionally specified 'basicAllocator' to supply
        // memory.  If 'basicAllocator' is 0, the currently installed default
        // allocator is used.

    RawDataUnformatted(const RawDataUnformatted& original,
                       bslma::Allocator *basicAllocator = 0);
        // Create an object of type 'RawDataUnformatted' having the value of
        // the specified 'original' object.  Use the optionally specified
        // 'basicAllocator' to supply memory.  If 'basicAllocator' is 0, the
        // currently installed default allocator is used.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    RawDataUnformatted(RawDataUnformatted&& original) noexcept;
        // Create an object of type 'RawDataUnformatted' having the value of
        // the specified 'original' object.  After performing this action, the
        // 'original' object will be left in a valid, but unspecified state.

    RawDataUnformatted(RawDataUnformatted&& original,
                       bslma::Allocator *basicAllocator);
        // Create an object of type 'RawDataUnformatted' having the value of
        // the specified 'original' object.  After performing this action, the
        // 'original' object will be left in a valid, but unspecified state.
        // Use the optionally specified 'basicAllocator' to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.
#endif

    ~RawDataUnformatted();
        // Destroy this object.

    // MANIPULATORS
    RawDataUnformatted& operator=(const RawDataUnformatted& rhs);
        // Assign to this object the value of the specified 'rhs' object.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    RawDataUnformatted& operator=(RawDataUnformatted&& rhs);
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

    bsl::vector<char>& charvec();
        // Return a reference to the modifiable "Charvec" attribute of this
        // object.

    bsl::vector<unsigned char>& ucharvec();
        // Return a reference to the modifiable "Ucharvec" attribute of this
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

    const bsl::vector<char>& charvec() const;
        // Return a reference offering non-modifiable access to the "Charvec"
        // attribute of this object.

    const bsl::vector<unsigned char>& ucharvec() const;
        // Return a reference offering non-modifiable access to the "Ucharvec"
        // attribute of this object.
};

// FREE OPERATORS
inline
bool operator==(const RawDataUnformatted& lhs, const RawDataUnformatted& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
    // the same value, and 'false' otherwise.  Two attribute objects have the
    // same value if each respective attribute has the same value.

inline
bool operator!=(const RawDataUnformatted& lhs, const RawDataUnformatted& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
    // have the same value, and 'false' otherwise.  Two attribute objects do
    // not have the same value if one or more respective attributes differ in
    // values.

inline
bsl::ostream& operator<<(bsl::ostream& stream, const RawDataUnformatted& rhs);
    // Format the specified 'rhs' to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.

}  // close package namespace

// TRAITS

BDLAT_DECL_SEQUENCE_WITH_ALLOCATOR_BITWISEMOVEABLE_TRAITS(s_baltst::RawDataUnformatted)

// ============================================================================
//                         INLINE FUNCTION DEFINITIONS
// ============================================================================

namespace s_baltst {

                          // ------------------------
                          // class RawDataUnformatted
                          // ------------------------

// CLASS METHODS
// MANIPULATORS
template <class MANIPULATOR>
int RawDataUnformatted::manipulateAttributes(MANIPULATOR& manipulator)
{
    int ret;

    ret = manipulator(&d_charvec, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CHARVEC]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_ucharvec, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_UCHARVEC]);
    if (ret) {
        return ret;
    }

    return 0;
}

template <class MANIPULATOR>
int RawDataUnformatted::manipulateAttribute(MANIPULATOR& manipulator, int id)
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_CHARVEC: {
        return manipulator(&d_charvec, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CHARVEC]);
      }
      case ATTRIBUTE_ID_UCHARVEC: {
        return manipulator(&d_ucharvec, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_UCHARVEC]);
      }
      default:
        return NOT_FOUND;
    }
}

template <class MANIPULATOR>
int RawDataUnformatted::manipulateAttribute(
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
bsl::vector<char>& RawDataUnformatted::charvec()
{
    return d_charvec;
}

inline
bsl::vector<unsigned char>& RawDataUnformatted::ucharvec()
{
    return d_ucharvec;
}

// ACCESSORS
template <class ACCESSOR>
int RawDataUnformatted::accessAttributes(ACCESSOR& accessor) const
{
    int ret;

    ret = accessor(d_charvec, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CHARVEC]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_ucharvec, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_UCHARVEC]);
    if (ret) {
        return ret;
    }

    return 0;
}

template <class ACCESSOR>
int RawDataUnformatted::accessAttribute(ACCESSOR& accessor, int id) const
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_CHARVEC: {
        return accessor(d_charvec, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CHARVEC]);
      }
      case ATTRIBUTE_ID_UCHARVEC: {
        return accessor(d_ucharvec, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_UCHARVEC]);
      }
      default:
        return NOT_FOUND;
    }
}

template <class ACCESSOR>
int RawDataUnformatted::accessAttribute(
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
const bsl::vector<char>& RawDataUnformatted::charvec() const
{
    return d_charvec;
}

inline
const bsl::vector<unsigned char>& RawDataUnformatted::ucharvec() const
{
    return d_ucharvec;
}

}  // close package namespace

// FREE FUNCTIONS

inline
bool s_baltst::operator==(
        const s_baltst::RawDataUnformatted& lhs,
        const s_baltst::RawDataUnformatted& rhs)
{
    return  lhs.charvec() == rhs.charvec()
         && lhs.ucharvec() == rhs.ucharvec();
}

inline
bool s_baltst::operator!=(
        const s_baltst::RawDataUnformatted& lhs,
        const s_baltst::RawDataUnformatted& rhs)
{
    return !(lhs == rhs);
}

inline
bsl::ostream& s_baltst::operator<<(
        bsl::ostream& stream,
        const s_baltst::RawDataUnformatted& rhs)
{
    return rhs.print(stream, 0, -1);
}

}  // close enterprise namespace
#endif

// GENERATED BY @BLP_BAS_CODEGEN_VERSION@
// USING bas_codegen.pl s_baltst_rawdataunformatted.xsd --mode msg --includedir . --msgComponent rawdataunformatted --noRecurse --noExternalization --noHashSupport --noAggregateConversion
// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright 2022 Bloomberg Finance L.P. All rights reserved.
//      Property of Bloomberg Finance L.P. (BFLP)
//      This software is made available solely pursuant to the
//      terms of a BFLP license agreement which governs its use.
// ------------------------------- END-OF-FILE --------------------------------
