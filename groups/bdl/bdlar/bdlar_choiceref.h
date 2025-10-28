// bdlar_choiceref.h                                                  -*-C++-*-
#ifndef INCLUDED_BDLAR_CHOICEREF
#define INCLUDED_BDLAR_CHOICEREF

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide ...
//
//@CLASSES:
//  bdlar::ChoiceRef: ...
//  bdlar::ChoiceConstRef: ...
//
//@DESCRIPTION: This component provides ...
//

#include <bdlar_choicevtable.h>
#include <bdlar_choicevtableutil.h>
#include <bdlar_typecategory.h>

#include <bdlat_valuetypefunctions.h>

#include <bslmf_enableif.h>

namespace BloombergLP {
namespace bdlar {

                           // ===============
                           // class ChoiceRef
                           // ===============

class ChoiceRef {
    // DATA
    void               *d_object_p;
    const ChoiceVtable *d_vtable_p;

  public:
    // CREATORS

    /// Create `ChoiceRef` object that type erases the supplied `choice`
    /// object.
    template <class t_CHOICE>
    explicit ChoiceRef(t_CHOICE& choice,
                      typename bsl::enable_if<
                          IsChoice<t_CHOICE>::value>::type * = 0);

    /// Create `ChoiceRef` object.  The behaviour is undefined unless
    /// `objectAddress` and `vtable` point to the valid objects.
    ChoiceRef(void *objectAddress, const ChoiceVtable *vtable);

    // MANIPULATORS

    /// Set the value to be the default for the selection indicated by the
    /// specified `selectionId`.  Return 0 on success, and non-zero value
    /// otherwise (i.e., the selection is not found).
    int makeSelection(int selectionId) const;

    /// Set the value to be the default for the selection indicated by the
    /// specified `selectionName` of the specified `selectionNameLength`.
    /// Return 0 on success, and non-zero value otherwise (i.e., the selection
    /// is not found).
    int makeSelection(const char *selectionName,
                      int         selectionNameLength) const;

    /// Invoke the specified `manipulator` on the address of the (modifiable)
    /// selection, supplying `manipulator` with the corresponding selection
    /// information structure. The supplied `manipulator` must be a callable
    /// type that can be called as if it had the following signature:
    /// ```
    /// template <class t_INFO>
    /// int manipulator(SELECTION *refOrValue, const t_INFO& info);
    /// ```
    /// Return the value returned from the invocation of `manipulator`.  The
    /// behavior is undefined unless `selectionId() != -1`.
    template <typename t_MANIPULATOR>
    int manipulateSelection(t_MANIPULATOR& manipulator) const;

    /// Reset the referred object to the default value.
    void reset() const;

    // ACCESSORS

    /// Invoke the specified `accessor` on the (non-modifiable) selection,
    /// supplying `accessor` with the corresponding selection information
    /// structure.  The supplied `accessor` must be a callable type that can be
    /// called as if it had the following signature:
    /// ```
    /// template <class t_INFO>
    /// int accessor(const SELECTION& refOrValue, const t_INFO& info);
    /// ```
    /// Return the value returned from the invocation of `accessor`.  The
    /// behavior is undefined unless `selectionId() != -1`.
    template <typename t_ACCESSOR>
    int accessSelection(t_ACCESSOR& accessor) const;

    /// Return a pointer to the vtable.
    const ChoiceVtable *vtable() const;

    /// Return true if the object has a selection with the specified
    /// `selectionId`, and false otherwise.
    bool hasSelection(int selectionId) const;

    /// Return true if the object has a selection with the specified
    /// `selectionName` of the specified `selectionNameLength`, and false
    /// otherwise.
    bool hasSelection(const char *selectionName,
                      int         selectionNameLength) const;

    /// Return a pointer to the wrapped array.
    void *objectAddress() const;

    /// Return the id of the current selection if the selection is defined, and
    /// -1 otherwise.
    int selectionId() const;
};

                           // ====================
                           // class ChoiceConstRef
                           // ====================

class ChoiceConstRef {
    // DATA
    const void              *d_object_p;
    const ChoiceConstVtable *d_vtable_p;

  public:
    // CREATORS

    /// Create `ChoiceConstRef` object that type erases the supplied `choice`
    /// object.
    template <class t_CHOICE>
    explicit ChoiceConstRef(const t_CHOICE& choice,
                            typename bsl::enable_if<
                                IsChoice<t_CHOICE>::value>::type * = 0);

    /// Create `ChoiceConstRef` object.  The behaviour is undefined unless
    /// `objectAddress` and `vtable` point to the valid objects.
    ChoiceConstRef(const void *objectAddress, const ChoiceConstVtable *vtable);

    /// Create `ChoiceConstRef` object that wraps the same object as the
    /// specified `object`.
    ChoiceConstRef(const ChoiceRef& object); // IMPLICIT

    // ACCESSORS

    /// Invoke the specified `accessor` on the (non-modifiable) selection,
    /// supplying `accessor` with the corresponding selection information
    /// structure.  The supplied `accessor` must be a callable type that can be
    /// called as if it had the following signature:
    /// ```
    /// template <class t_INFO>
    /// int accessor(const SELECTION& refOrValue, const t_INFO& info);
    /// ```
    /// Return the value returned from the invocation of `accessor`.  The
    /// behavior is undefined unless `selectionId() != -1`.
    template <typename t_ACCESSOR>
    int accessSelection(t_ACCESSOR& accessor) const;

    /// Return a pointer to the vtable.
    const ChoiceConstVtable *vtable() const;

    /// Return true if the object has a selection with the specified
    /// `selectionId`, and false otherwise.
    bool hasSelection(int selectionId) const;

    /// Return true if the object has a selection with the specified
    /// `selectionName` of the specified `selectionNameLength`, and false
    /// otherwise.
    bool hasSelection(const char *selectionName,
                      int         selectionNameLength) const;

    /// Return a pointer to the wrapped array.
    const void *objectAddress() const;

    /// Return the id of the current selection if the selection is defined, and
    /// -1 otherwise.
    int selectionId() const;

    /// Return the `bdlat_TypeName::xsdName` value for the underlying object.
    const char *xsdName(int format) const;
};

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

                           // ---------------
                           // class ChoiceRef
                           // ---------------

// CREATORS
template <class t_CHOICE>
inline
ChoiceRef::ChoiceRef(t_CHOICE& choice,
                     typename bsl::enable_if<
                         IsChoice<t_CHOICE>::value>::type *)
: d_object_p(&choice)
, d_vtable_p(ChoiceVtableUtil::getVtable<t_CHOICE>())
{
}

inline
ChoiceRef::ChoiceRef(void *objectAddress, const ChoiceVtable *vtable)
: d_object_p(objectAddress)
, d_vtable_p(vtable)
{
}

// MANIPULATORS
inline
int ChoiceRef::makeSelection(int selectionId) const
{
    return d_vtable_p->d_makeSelectionById_fp(d_object_p, selectionId);
}

inline
int ChoiceRef::makeSelection(const char *selectionName,
                             int         selectionNameLength) const
{
    return d_vtable_p->d_makeSelectionByName_fp(d_object_p,
                                                selectionName,
                                                selectionNameLength);
}

template <typename t_MANIPULATOR>
inline
int ChoiceRef::manipulateSelection(t_MANIPULATOR& manipulator) const
{
    typename FakeDependency<t_MANIPULATOR, ManipulatorWithInfoRef>::Type
                                                   manipulatorRef(manipulator);
    return d_vtable_p->d_manipulateSelection_fp(d_object_p, manipulatorRef);
}

inline
void ChoiceRef::reset() const
{
    return d_vtable_p->d_reset_fp(d_object_p);
}

// ACCESSORS
template <typename t_ACCESSOR>
inline
int ChoiceRef::accessSelection(t_ACCESSOR& accessor) const
{
    typename FakeDependency<t_ACCESSOR, AccessorWithInfoRef>::Type
                                                         accessorRef(accessor);
    return d_vtable_p->d_const.d_accessSelection_fp(d_object_p, accessorRef);
}

inline
bool ChoiceRef::hasSelection(int selectionId) const
{
    return d_vtable_p->d_const.d_hasSelectionId_fp(d_object_p, selectionId);
}

inline
bool ChoiceRef::hasSelection(const char *selectionName,
                             int         selectionNameLength) const
{
    return d_vtable_p->d_const.d_hasSelectionName_fp(d_object_p,
                                                     selectionName,
                                                     selectionNameLength);
}

inline
const ChoiceVtable *ChoiceRef::vtable() const
{
    return d_vtable_p;
}

inline
void *ChoiceRef::objectAddress() const
{
    return d_object_p;
}

inline
int ChoiceRef::selectionId() const
{
    return d_vtable_p->d_const.d_selectionId_fp(d_object_p);
}

// FREE FUNCTIONS
// Customization-point functions (`bdlat_ChoiceFunctions`)

inline
int bdlat_choiceMakeSelection(ChoiceRef *ref, int selectionId)
{
    return ref->makeSelection(selectionId);
}

inline
int bdlat_choiceMakeSelection(ChoiceRef  *ref,
                              const char *selectionName,
                              int         selectionNameLength)
{
    return ref->makeSelection(selectionName, selectionNameLength);
}

template <class t_MANIPULATOR>
inline
int bdlat_choiceManipulateSelection(ChoiceRef *ref, t_MANIPULATOR& manipulator)
{
    return ref->manipulateSelection(manipulator);
}

template <class t_ACCESSOR>
inline
int bdlat_choiceAccessSelection(const ChoiceRef& ref, t_ACCESSOR& accessor)
{
    return ref.accessSelection(accessor);
}

inline
bool bdlat_choiceHasSelection(const ChoiceRef&  ref,
                              const char       *selectionName,
                              int               selectionNameLength)
{
    return ref.hasSelection(selectionName, selectionNameLength);
}

inline
bool bdlat_choiceHasSelection(const ChoiceRef& ref, int selectionId)
{
    return ref.hasSelection(selectionId);
}

inline
int bdlat_choiceSelectionId(const ChoiceRef& ref)
{
    return ref.selectionId();
}

inline
void bdlat_valueTypeReset(ChoiceRef *ref)
{
    ref->reset();
}

                           // --------------------
                           // class ChoiceConstRef
                           // --------------------

// CREATORS
template <class t_CHOICE>
inline
ChoiceConstRef::ChoiceConstRef(const t_CHOICE& choice,
                               typename bsl::enable_if<
                                   IsChoice<t_CHOICE>::value>::type *)
: d_object_p(&choice)
, d_vtable_p(ChoiceVtableUtil::getConstVtable<t_CHOICE>())
{
}

inline
ChoiceConstRef::ChoiceConstRef(const void              *objectAddress,
                               const ChoiceConstVtable *vtable)
: d_object_p(objectAddress)
, d_vtable_p(vtable)
{
}

inline
ChoiceConstRef::ChoiceConstRef(const ChoiceRef& object)
: d_object_p(object.objectAddress())
, d_vtable_p(&object.vtable()->d_const)
{
}

// ACCESSORS
template <typename t_ACCESSOR>
inline
int ChoiceConstRef::accessSelection(t_ACCESSOR& accessor) const
{
    typename FakeDependency<t_ACCESSOR, AccessorWithInfoRef>::Type
                                                         accessorRef(accessor);
    return d_vtable_p->d_accessSelection_fp(d_object_p, accessorRef);
}

inline
bool ChoiceConstRef::hasSelection(int selectionId) const
{
    return d_vtable_p->d_hasSelectionId_fp(d_object_p, selectionId);
}

inline
bool ChoiceConstRef::hasSelection(const char *selectionName,
                                  int         selectionNameLength) const
{
    return d_vtable_p->d_hasSelectionName_fp(d_object_p,
                                             selectionName,
                                             selectionNameLength);
}

inline
const ChoiceConstVtable *ChoiceConstRef::vtable() const
{
    return d_vtable_p;
}

inline
const void *ChoiceConstRef::objectAddress() const
{
    return d_object_p;
}

inline
int ChoiceConstRef::selectionId() const
{
    return d_vtable_p->d_selectionId_fp(d_object_p);
}

inline
const char *ChoiceConstRef::xsdName(int format) const
{
    return d_vtable_p->d_xsdName_fp(d_object_p, format);
}

// FREE FUNCTIONS
// Customization-point functions (`bdlat_ChoiceFunctions`)

template <class t_ACCESSOR>
inline
int bdlat_choiceAccessSelection(const ChoiceConstRef& ref,
                                t_ACCESSOR&           accessor)
{
    return ref.accessSelection(accessor);
}

inline
bool bdlat_choiceHasSelection(const ChoiceConstRef&  ref,
                              const char            *selectionName,
                              int                    selectionNameLength)
{
    return ref.hasSelection(selectionName, selectionNameLength);
}

inline
bool bdlat_choiceHasSelection(const ChoiceConstRef& ref, int selectionId)
{
    return ref.hasSelection(selectionId);
}

inline
int bdlat_choiceSelectionId(const ChoiceConstRef& ref)
{
    return ref.selectionId();
}

inline
const char *bdlat_TypeName_xsdName(const ChoiceConstRef& ref, int format)
{
    return ref.xsdName(format);
}

}  // close package namespace

namespace bdlat_ChoiceFunctions {

template <>
struct IsChoice<bdlar::ChoiceRef> : bsl::true_type {
};

template <>
struct IsChoice<bdlar::ChoiceConstRef> : bsl::true_type {
};

}  // close namespace bdlat_ChoiceFunctions
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
