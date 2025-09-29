// s_baltst_rawdataswitched.h        *DO NOT EDIT*         @generated -*-C++-*-
#ifndef INCLUDED_S_BALTST_RAWDATASWITCHED
#define INCLUDED_S_BALTST_RAWDATASWITCHED

#include <bsls_ident.h>
BSLS_IDENT_RCSID(s_baltst_rawdataswitched_h, "$Id$ $CSID$")
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

namespace s_baltst { class RawDataSwitched; }
namespace s_baltst {

                           // =====================
                           // class RawDataSwitched
                           // =====================

class RawDataSwitched {

    // INSTANCE DATA
    bsl::vector<unsigned char>  d_ucharvec;
    bsl::vector<char>           d_charvec;

  public:
    // TYPES
    enum {
        ATTRIBUTE_ID_CHARVEC  = 1
      , ATTRIBUTE_ID_UCHARVEC = 0
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

    /// Create an object of type `RawDataSwitched` having the default value.
    ///  Use the optionally specified `basicAllocator` to supply memory.  If
    /// `basicAllocator` is 0, the currently installed default allocator is
    /// used.
    explicit RawDataSwitched(bslma::Allocator *basicAllocator = 0);

    /// Create an object of type `RawDataSwitched` having the value of the
    /// specified `original` object.  Use the optionally specified
    /// `basicAllocator` to supply memory.  If `basicAllocator` is 0, the
    /// currently installed default allocator is used.
    RawDataSwitched(const RawDataSwitched& original,
                    bslma::Allocator *basicAllocator = 0);

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    /// Create an object of type `RawDataSwitched` having the value of the
    /// specified `original` object.  After performing this action, the
    /// `original` object will be left in a valid, but unspecified state.
    RawDataSwitched(RawDataSwitched&& original) noexcept;

    /// Create an object of type `RawDataSwitched` having the value of the
    /// specified `original` object.  After performing this action, the
    /// `original` object will be left in a valid, but unspecified state.
    /// Use the optionally specified `basicAllocator` to supply memory.  If
    /// `basicAllocator` is 0, the currently installed default allocator is
    /// used.
    RawDataSwitched(RawDataSwitched&& original,
                    bslma::Allocator *basicAllocator);
#endif

    /// Destroy this object.
    ~RawDataSwitched();

    // MANIPULATORS

    /// Assign to this object the value of the specified `rhs` object.
    RawDataSwitched& operator=(const RawDataSwitched& rhs);

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    /// Assign to this object the value of the specified `rhs` object.
    /// After performing this action, the `rhs` object will be left in a
    /// valid, but unspecified state.
    RawDataSwitched& operator=(RawDataSwitched&& rhs);
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

    /// Return a reference to the modifiable "Charvec" attribute of this
    /// object.
    bsl::vector<char>& charvec();

    /// Return a reference to the modifiable "Ucharvec" attribute of this
    /// object.
    bsl::vector<unsigned char>& ucharvec();

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

    /// Return a reference offering non-modifiable access to the "Charvec"
    /// attribute of this object.
    const bsl::vector<char>& charvec() const;

    /// Return a reference offering non-modifiable access to the "Ucharvec"
    /// attribute of this object.
    const bsl::vector<unsigned char>& ucharvec() const;

    // HIDDEN FRIENDS

    /// Return `true` if the specified `lhs` and `rhs` attribute objects
    /// have the same value, and `false` otherwise.  Two attribute objects
    /// have the same value if each respective attribute has the same value.
    friend bool operator==(const RawDataSwitched& lhs,
                           const RawDataSwitched& rhs)
    {
        return lhs.charvec() == rhs.charvec() &&
               lhs.ucharvec() == rhs.ucharvec();
    }

    /// Returns `!(lhs == rhs)`
    friend bool operator!=(const RawDataSwitched& lhs,
                           const RawDataSwitched& rhs)
    {
        return !(lhs == rhs);
    }

    /// Format the specified `rhs` to the specified output `stream` and
    /// return a reference to the modifiable `stream`.
    friend bsl::ostream& operator<<(bsl::ostream&          stream,
                                    const RawDataSwitched& rhs)
    {
        return rhs.print(stream, 0, -1);
    }
};

}  // close package namespace

// TRAITS

BDLAT_DECL_SEQUENCE_WITH_ALLOCATOR_BITWISEMOVEABLE_TRAITS(s_baltst::RawDataSwitched)
template <>
struct bdlat_UsesDefaultValueFlag<s_baltst::RawDataSwitched> : bsl::true_type {};

// ============================================================================
//                          INLINE DEFINITIONS
// ============================================================================

namespace s_baltst {

                           // ---------------------
                           // class RawDataSwitched
                           // ---------------------

// CLASS METHODS
// MANIPULATORS
template <typename t_MANIPULATOR>
int RawDataSwitched::manipulateAttributes(t_MANIPULATOR& manipulator)
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

template <typename t_MANIPULATOR>
int RawDataSwitched::manipulateAttribute(t_MANIPULATOR& manipulator, int id)
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

template <typename t_MANIPULATOR>
int RawDataSwitched::manipulateAttribute(
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
bsl::vector<char>& RawDataSwitched::charvec()
{
    return d_charvec;
}

inline
bsl::vector<unsigned char>& RawDataSwitched::ucharvec()
{
    return d_ucharvec;
}

// ACCESSORS
template <typename t_ACCESSOR>
int RawDataSwitched::accessAttributes(t_ACCESSOR& accessor) const
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

template <typename t_ACCESSOR>
int RawDataSwitched::accessAttribute(t_ACCESSOR& accessor, int id) const
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

template <typename t_ACCESSOR>
int RawDataSwitched::accessAttribute(
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
const bsl::vector<char>& RawDataSwitched::charvec() const
{
    return d_charvec;
}

inline
const bsl::vector<unsigned char>& RawDataSwitched::ucharvec() const
{
    return d_ucharvec;
}

}  // close package namespace

// FREE FUNCTIONS

}  // close enterprise namespace
#endif

// GENERATED BY BLP_BAS_CODEGEN_2025.08.21
// USING bas_codegen.pl s_baltst_rawdataswitched.xsd --mode msg --includedir . --msgComponent rawdataswitched --noRecurse --noExternalization --noHashSupport --noAggregateConversion
// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright 2025 Bloomberg Finance L.P. All rights reserved.
//      Property of Bloomberg Finance L.P. (BFLP)
//      This software is made available solely pursuant to the
//      terms of a BFLP license agreement which governs its use.
// ------------------------------- END-OF-FILE --------------------------------
