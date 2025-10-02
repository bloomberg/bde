// bdlar_sequenceref.h                                                -*-C++-*-
#ifndef INCLUDED_BDLAR_SEQUENCEREF
#define INCLUDED_BDLAR_SEQUENCEREF

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide ...
//
//@CLASSES:
//  bdlar::SequenceRef: ...
//  bdlar::SequenceConstRef: ...
//
//@DESCRIPTION: This component provides ...
//

#include <bdlar_sequencevtable.h>
#include <bdlar_sequencevtableutil.h>
#include <bdlar_typecategory.h>

#include <bdlat_valuetypefunctions.h>

#include <bslmf_enableif.h>

namespace BloombergLP {
namespace bdlar {

                           // =================
                           // class SequenceRef
                           // =================

class SequenceRef {
    // DATA
    void                 *d_object_p;
    const SequenceVtable *d_vtable_p;

  public:
    // CREATORS

    /// Create `SequenceRef` object that type erases the supplied `sequence`
    /// object.
    template <class t_SEQUENCE>
    explicit SequenceRef(t_SEQUENCE& sequence,
                         typename bsl::enable_if<
                             IsSequence<t_SEQUENCE>::value>::type * = 0);

    /// Create `SequenceRef` object.  The behaviour is undefined unless
    /// `objectAddress` and `vtable` point to the valid objects.
    SequenceRef(void *objectAddress, const SequenceVtable *vtable);

    // MANIPULATORS

    /// Invoke the specified `manipulator` on the (modifiable) attribute
    /// indicated by the specified `attributeId`, supplying `manipulator` with
    /// the corresponding attribute information structure.  The supplied
    /// `manipulator` must be a callable type that can be called as if it had
    /// the following signature:
    /// ```
    /// template <class t_INFO>
    /// int manipulator(ATTRIBUTE *refOrValue, const t_INFO& info);
    /// ```
    /// Return non-zero value if the attribute is not found, and the value
    /// returned from the invocation of `manipulator` otherwise.
    template <class t_MANIPULATOR>
    int manipulateAttribute(t_MANIPULATOR& manipulator, int attributeId) const;

    /// Invoke the specified `manipulator` on the (modifiable) attribute
    /// indicated by the specified `attributeName` and `attributeNameLength`,
    /// supplying `manipulator` with the corresponding attribute information
    /// structure.  The supplied `manipulator` must be a callable type that can
    /// be called as if it had the following signature:
    /// ```
    /// template <class t_INFO>
    /// int manipulator(ATTRIBUTE *refOrValue, const t_INFO& info);
    /// ```
    /// Return non-zero value if the attribute is not found, and the value
    /// returned from the invocation of `manipulator` otherwise.
    template <class t_MANIPULATOR>
    int manipulateAttribute(t_MANIPULATOR&  manipulator,
                            const char     *attributeName,
                            int             attributeNameLength) const;

    /// Invoke the specified `manipulator` sequentially on each (modifiable)
    /// attribute, supplying `manipulator` with the corresponding attribute
    /// information structure until such invocation returns non-zero value.
    /// The supplied `manipulator` must be a callable type that can be called
    /// as if it had the following signature:
    /// ```
    /// template <class t_INFO>
    /// int manipulator(ATTRIBUTE *refOrValue, const t_INFO& info);
    /// ```
    /// Return the value from the last invocation of `manipulator` (i.e., the
    /// invocation that terminated the sequence).
    template <class t_MANIPULATOR>
    int manipulateAttributes(t_MANIPULATOR& manipulator) const;


    /// Reset the referred object to the default value.
    void reset() const;

    // ACCESSORS

    /// Invoke the specified `accessor` on the (non-modifiable) attribute of
    /// the referred sequence object with the given `attributeId`, supplying
    /// `accessor` with the corresponding attribute information structure.  The
    /// supplied `accessor` must be a callable type that can be called as if it
    /// had the following signature:
    /// ```
    /// template <class t_INFO>
    /// int accessor(const ATTRIBUTE& refOrValue, const t_INFO& info);
    /// ```
    /// Return non-zero if the attribute is not found, and the value returned
    /// from the invocation of `accessor` otherwise.
    template <class t_ACCESSOR>
    int accessAttribute(t_ACCESSOR& accessor, int attributeId) const;

    /// Invoke the specified `accessor` on the (non-modifiable) attribute of
    /// the referred sequence object indicated by the specified `attributeName`
    /// and `attributeNameLength`, supplying `accessor` with the corresponding
    /// attribute information structure.  The supplied `accessor` must be a
    /// callable type that can be called as if it had the following signature:
    /// ```
    /// template <class t_INFO>
    /// int accessor(const ATTRIBUTE& refOrValue, const t_INFO& info);
    /// ```
    /// Return non-zero value if the attribute is not found, and the value
    /// returned from the invocation of `accessor` otherwise.
    template <class t_ACCESSOR>
    int accessAttribute(t_ACCESSOR&  accessor,
                        const char  *attributeName,
                        int          attributeNameLength) const;

    /// Invoke the specified `accessor` sequentially on each attribute,
    /// supplying `accessor` with the corresponding attribute information
    /// structure until such invocation returns a non-zero value.  The supplied
    /// `accessor` must be a callable type that can be called as if it had the
    /// following signature:
    /// ```
    /// template <class t_INFO>
    /// int accessor(const ATTRIBUTE& refOrValue, const t_INFO& info);
    /// ```
    /// Return the value from the last invocation of `accessor` (i.e., the
    /// invocation that terminated the sequence).
    template <class t_ACCESSOR>
    int accessAttributes(t_ACCESSOR& accessor) const;

    /// Return a pointer to the vtable.
    const SequenceVtable *vtable() const;

    /// Return `true` if the referred sequence object has an attribute with the
    /// specified `attributeId`, and `false` otherwise.
    bool hasAttribute(int attributeId) const;

    /// Return `true` if the referred sequence object has an attribute with the
    /// specified `attributeName` of the specified `attributeNameLength`, and
    /// `false` otherwise.
    bool hasAttribute(const char *attributeName,
                      int         attributeNameLength) const;

    /// Return a pointer to the wrapped object.
    void *objectAddress() const;
};

                           // ======================
                           // class SequenceConstRef
                           // ======================

class SequenceConstRef {
    // DATA
    const void                *d_object_p;
    const SequenceConstVtable *d_vtable_p;

  public:
    // CREATORS

    /// Create `SequenceConstRef` object that type erases the supplied
    /// `sequence` object.
    template <class t_SEQUENCE>
    explicit SequenceConstRef(const t_SEQUENCE& sequence,
                              typename bsl::enable_if<
                                  IsSequence<t_SEQUENCE>::value>::type * = 0);

    /// Create `SequenceConstRef` object.  The behaviour is undefined unless
    /// `objectAddress` and `vtable` point to the valid objects.
    SequenceConstRef(const void                 *objectAddress,
                     const SequenceConstVtable  *vtable);

    /// Create `SequenceConstRef` object that wraps the same object as the
    /// specified `object`.
    SequenceConstRef(const SequenceRef& object); // IMPLICIT

    // ACCESSORS

    /// Invoke the specified `accessor` on the (non-modifiable) attribute of
    /// the referred sequence object with the given `attributeId`, supplying
    /// `accessor` with the corresponding attribute information structure.  The
    /// supplied `accessor` must be a callable type that can be called as if it
    /// had the following signature:
    /// ```
    /// template <class t_INFO>
    /// int accessor(const ATTRIBUTE& refOrValue, const t_INFO& info);
    /// ```
    /// Return non-zero if the attribute is not found, and the value returned
    /// from the invocation of `accessor` otherwise.
    template <class t_ACCESSOR>
    int accessAttribute(t_ACCESSOR& accessor, int attributeId) const;

    /// Invoke the specified `accessor` on the (non-modifiable) attribute of
    /// the referred sequence object indicated by the specified `attributeName`
    /// and `attributeNameLength`, supplying `accessor` with the corresponding
    /// attribute information structure.  The supplied `accessor` must be a
    /// callable type that can be called as if it had the following signature:
    /// ```
    /// template <class t_INFO>
    /// int accessor(const ATTRIBUTE& refOrValue, const t_INFO& info);
    /// ```
    /// Return non-zero value if the attribute is not found, and the value
    /// returned from the invocation of `accessor` otherwise.
    template <class t_ACCESSOR>
    int accessAttribute(t_ACCESSOR&  accessor,
                        const char  *attributeName,
                        int          attributeNameLength) const;

    /// Invoke the specified `accessor` sequentially on each attribute,
    /// supplying `accessor` with the corresponding attribute information
    /// structure until such invocation returns a non-zero value.  The supplied
    /// `accessor` must be a callable type that can be called as if it had the
    /// following signature:
    /// ```
    /// template <class t_INFO>
    /// int accessor(const ATTRIBUTE& refOrValue, const t_INFO& info);
    /// ```
    /// Return the value from the last invocation of `accessor` (i.e., the
    /// invocation that terminated the sequence).
    template <class t_ACCESSOR>
    int accessAttributes(t_ACCESSOR& accessor) const;

    /// Return a pointer to the vtable.
    const SequenceConstVtable *vtable() const;

    /// Return `true` if the referred sequence object has an attribute with the
    /// specified `attributeId`, and `false` otherwise.
    bool hasAttribute(int attributeId) const;

    /// Return `true` if the referred sequence object has an attribute with the
    /// specified `attributeName` of the specified `attributeNameLength`, and
    /// `false` otherwise.
    bool hasAttribute(const char *attributeName,
                      int         attributeNameLength) const;

    /// Return a pointer to the wrapped object.
    const void *objectAddress() const;

    /// Return the `bdlat_TypeName::xsdName` value for the underlying object.
    const char *xsdName(int format) const;
};

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

                           // -----------------
                           // class SequenceRef
                           // -----------------

// CREATORS
template <class t_SEQUENCE>
inline
SequenceRef::SequenceRef(t_SEQUENCE& sequence,
                         typename bsl::enable_if<
                             IsSequence<t_SEQUENCE>::value>::type *)
: d_object_p(&sequence)
, d_vtable_p(SequenceVtableUtil::getVtable<t_SEQUENCE>())
{
}

inline
SequenceRef::SequenceRef(void *objectAddress, const SequenceVtable *vtable)
: d_object_p(objectAddress)
, d_vtable_p(vtable)
{
}

// MANIPULATORS
template <class t_MANIPULATOR>
inline
int SequenceRef::manipulateAttribute(t_MANIPULATOR& manipulator,
                                     int            attributeId) const
{
    typename FakeDependency<t_MANIPULATOR, ManipulatorWithInfoRef>::Type
                                                   manipulatorRef(manipulator);
    return d_vtable_p->d_manipulateAttributeById_fp(d_object_p,
                                                    manipulatorRef,
                                                    attributeId);
}

template <class t_MANIPULATOR>
inline
int SequenceRef::manipulateAttribute(t_MANIPULATOR&  manipulator,
                                     const char     *attributeName,
                                     int             attributeNameLength) const
{
    typename FakeDependency<t_MANIPULATOR, ManipulatorWithInfoRef>::Type
                                                   manipulatorRef(manipulator);
    return d_vtable_p->d_manipulateAttributeByName_fp(d_object_p,
                                                      manipulatorRef,
                                                      attributeName,
                                                      attributeNameLength);
}

template <class t_MANIPULATOR>
inline
int SequenceRef::manipulateAttributes(t_MANIPULATOR& manipulator) const
{
    typename FakeDependency<t_MANIPULATOR, ManipulatorWithInfoRef>::Type
                                                   manipulatorRef(manipulator);
    return d_vtable_p->d_manipulateAttributes_fp(d_object_p, manipulatorRef);
}

inline
void SequenceRef::reset() const
{
    return d_vtable_p->d_reset_fp(d_object_p);
}

// ACCESSORS
template <class t_ACCESSOR>
inline
int SequenceRef::accessAttribute(t_ACCESSOR& accessor,
                                 int         attributeId) const
{
    typename FakeDependency<t_ACCESSOR, AccessorWithInfoRef>::Type
                                                         accessorRef(accessor);
    return d_vtable_p->d_const.d_accessAttributeById_fp(d_object_p,
                                                        accessorRef,
                                                        attributeId);
}

template <class t_ACCESSOR>
inline
int SequenceRef::accessAttribute(t_ACCESSOR&  accessor,
                                 const char  *attributeName,
                                 int          attributeNameLength) const
{
    typename FakeDependency<t_ACCESSOR, AccessorWithInfoRef>::Type
                                                         accessorRef(accessor);
    return d_vtable_p->d_const.d_accessAttributeByName_fp(d_object_p,
                                                          accessorRef,
                                                          attributeName,
                                                          attributeNameLength);
}

template <class t_ACCESSOR>
inline
int SequenceRef::accessAttributes(t_ACCESSOR& accessor) const
{
    typename FakeDependency<t_ACCESSOR, AccessorWithInfoRef>::Type
                                                         accessorRef(accessor);
    return d_vtable_p->d_const.d_accessAttributes_fp(d_object_p, accessorRef);
}

inline
const SequenceVtable *SequenceRef::vtable() const
{
    return d_vtable_p;
}

inline
bool SequenceRef::hasAttribute(int attributeId) const
{
    return d_vtable_p->d_const.d_hasAttributeId_fp(d_object_p, attributeId);
}

inline
bool SequenceRef::hasAttribute(const char *attributeName,
                               int         attributeNameLength) const
{
    return d_vtable_p->d_const.d_hasAttributeName_fp(d_object_p,
                                                     attributeName,
                                                     attributeNameLength);
}

inline
void *SequenceRef::objectAddress() const
{
    return d_object_p;
}

// FREE FUNCTIONS
// Customization-point functions (`bdlat_SequenceFunctions`)

template <class t_MANIPULATOR>
inline
int bdlat_sequenceManipulateAttribute(SequenceRef    *ref,
                                      t_MANIPULATOR&  manipulator,
                                      const char     *attributeName,
                                      int             attributeNameLength)
{
    return ref->manipulateAttribute(manipulator,
                                    attributeName,
                                    attributeNameLength);
}

template <class t_MANIPULATOR>
inline
int bdlat_sequenceManipulateAttribute(SequenceRef    *ref,
                                      t_MANIPULATOR&  manipulator,
                                      int             attributeId)
{
    return ref->manipulateAttribute(manipulator, attributeId);
}

template <class t_MANIPULATOR>
inline
int bdlat_sequenceManipulateAttributes(SequenceRef    *ref,
                                       t_MANIPULATOR&  manipulator)
{
    return ref->manipulateAttributes(manipulator);
}

template <class t_ACCESSOR>
inline
int bdlat_sequenceAccessAttribute(const SequenceRef&  ref,
                                  t_ACCESSOR&         accessor,
                                  const char         *attributeName,
                                  int                 attributeNameLength)
{
    return ref.accessAttribute(accessor, attributeName, attributeNameLength);
}

template <class t_ACCESSOR>
inline
int bdlat_sequenceAccessAttribute(const SequenceRef& ref,
                                  t_ACCESSOR&        accessor,
                                  int                attributeId)
{
    return ref.accessAttribute(accessor, attributeId);
}

template <class t_ACCESSOR>
inline
int bdlat_sequenceAccessAttributes(const SequenceRef& ref,
                                   t_ACCESSOR&        accessor)
{
    return ref.accessAttributes(accessor);
}

inline
bool bdlat_sequenceHasAttribute(const SequenceRef&  ref,
                                const char         *attributeName,
                                int                 attributeNameLength)
{
    return ref.hasAttribute(attributeName, attributeNameLength);
}

inline
bool bdlat_sequenceHasAttribute(const SequenceRef& ref, int attributeId)
{
    return ref.hasAttribute(attributeId);
}

inline
void bdlat_valueTypeReset(SequenceRef *ref)
{
    ref->reset();
}

                           // ----------------------
                           // class SequenceConstRef
                           // ----------------------

// CREATORS
template <class t_SEQUENCE>
inline
SequenceConstRef::SequenceConstRef(const t_SEQUENCE& sequence,
                                   typename bsl::enable_if<
                                       IsSequence<t_SEQUENCE>::value>::type *)
: d_object_p(&sequence)
, d_vtable_p(SequenceVtableUtil::getConstVtable<t_SEQUENCE>())
{
}

inline
SequenceConstRef::SequenceConstRef(const void                *objectAddress,
                                   const SequenceConstVtable *vtable)
: d_object_p(objectAddress)
, d_vtable_p(vtable)
{
}

inline
SequenceConstRef::SequenceConstRef(const SequenceRef& object)
: d_object_p(object.objectAddress())
, d_vtable_p(&object.vtable()->d_const)
{
}

// ACCESSORS
template <class t_ACCESSOR>
inline
int SequenceConstRef::accessAttribute(t_ACCESSOR& accessor,
                                      int         attributeId) const
{
    typename FakeDependency<t_ACCESSOR, AccessorWithInfoRef>::Type
                                                         accessorRef(accessor);
    return d_vtable_p->d_accessAttributeById_fp(d_object_p,
                                                accessorRef,
                                                attributeId);
}

template <class t_ACCESSOR>
inline
int SequenceConstRef::accessAttribute(t_ACCESSOR&  accessor,
                                      const char  *attributeName,
                                      int          attributeNameLength) const
{
    typename FakeDependency<t_ACCESSOR, AccessorWithInfoRef>::Type
                                                         accessorRef(accessor);
    return d_vtable_p->d_accessAttributeByName_fp(d_object_p,
                                                  accessorRef,
                                                  attributeName,
                                                  attributeNameLength);
}

template <class t_ACCESSOR>
inline
int SequenceConstRef::accessAttributes(t_ACCESSOR& accessor) const
{
    typename FakeDependency<t_ACCESSOR, AccessorWithInfoRef>::Type
                                                         accessorRef(accessor);
    return d_vtable_p->d_accessAttributes_fp(d_object_p, accessorRef);
}

inline
const SequenceConstVtable *SequenceConstRef::vtable() const
{
    return d_vtable_p;
}

inline
bool SequenceConstRef::hasAttribute(int attributeId) const
{
    return d_vtable_p->d_hasAttributeId_fp(d_object_p, attributeId);
}

inline
bool SequenceConstRef::hasAttribute(const char *attributeName,
                                    int         attributeNameLength) const
{
    return d_vtable_p->d_hasAttributeName_fp(d_object_p,
                                             attributeName,
                                             attributeNameLength);
}

inline
const void *SequenceConstRef::objectAddress() const
{
    return d_object_p;
}

inline
const char *SequenceConstRef::xsdName(int format) const
{
    return d_vtable_p->d_xsdName_fp(d_object_p, format);
}

// FREE FUNCTIONS
// Customization-point functions (`bdlat_SequenceFunctions`)

template <class t_ACCESSOR>
inline
int bdlat_sequenceAccessAttribute(const SequenceConstRef&  ref,
                                  t_ACCESSOR&              accessor,
                                  const char              *attributeName,
                                  int                      attributeNameLength)
{
    return ref.accessAttribute(accessor, attributeName, attributeNameLength);
}

template <class t_ACCESSOR>
inline
int bdlat_sequenceAccessAttribute(const SequenceConstRef& ref,
                                  t_ACCESSOR&             accessor,
                                  int                     attributeId)
{
    return ref.accessAttribute(accessor, attributeId);
}

template <class t_ACCESSOR>
inline
int bdlat_sequenceAccessAttributes(const SequenceConstRef& ref,
                                   t_ACCESSOR&             accessor)
{
    return ref.accessAttributes(accessor);
}

inline
bool bdlat_sequenceHasAttribute(const SequenceConstRef&  ref,
                                const char              *attributeName,
                                int                      attributeNameLength)
{
    return ref.hasAttribute(attributeName, attributeNameLength);
}

inline
bool bdlat_sequenceHasAttribute(const SequenceConstRef& ref, int attributeId)
{
    return ref.hasAttribute(attributeId);
}

inline
const char *bdlat_TypeName_xsdName(const SequenceConstRef& ref, int format)
{
    return ref.xsdName(format);
}

}  // close package namespace

namespace bdlat_SequenceFunctions {

template <>
struct IsSequence<bdlar::SequenceRef> : bsl::true_type {
};

template <>
struct IsSequence<bdlar::SequenceConstRef> : bsl::true_type {
};

}  // close namespace bdlat_SequenceFunctions
}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2024 Bloomberg Finance L.P.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// ----------------------------- END-OF-FILE ----------------------------------
