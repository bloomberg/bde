// bdlar_arrayref.h                                                   -*-C++-*-
#ifndef INCLUDED_BDLAR_ARRAYREF
#define INCLUDED_BDLAR_ARRAYREF

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide ...
//
//@CLASSES:
//  bdlar::ArrayRef: ...
//  bdlar::ArrayConstRef: ...
//
//@DESCRIPTION: This component provides ...
//

#include <bdlar_arrayvtable.h>
#include <bdlar_arrayvtableutil.h>
#include <bdlar_typecategory.h>

#include <bdlat_valuetypefunctions.h>

#include <bsl_type_traits.h>

namespace BloombergLP {
namespace bdlar {

// Forward
class AnyRef;
class AnyConstRef;

                           // ==============
                           // class ArrayRef
                           // ==============

class ArrayRef {
    // DATA
    void              *d_object_p;
    const ArrayVtable *d_vtable_p;

  public:
    // CREATORS

    /// Create `ArrayRef` object that type erases the supplied `array` object.
    template <class t_ARRAY>
    explicit ArrayRef(t_ARRAY& array,
                      typename bsl::enable_if<
                          IsArray<t_ARRAY>::value>::type * = 0);

    /// Create `ArrayRef` object.  The behaviour is undefined unless
    /// `objectAddress` and `vtable` point to the valid objects.
    ArrayRef(void *objectAddress, const ArrayVtable *vtable);

    // MANIPULATORS

    /// Invoke the specified `manipulator` on the array element at the
    /// specified `index`.  The supplied `manipulator` must be a callable type
    /// that can be called as if it had the following signature:
    /// ```
    /// int manipulator(ELEMENT *refOrValue);
    /// ```
    /// Return the value from the invocation of `manipulator`.  The behavior is
    /// undefined unless `0 <= index < size()`.
    template <class t_MANIPULATOR>
    int manipulateElement(t_MANIPULATOR& manipulator, int index) const;

    /// Reset the referred object to the default value.
    void reset() const;

    /// Set the size to the specified `newSize`.  If `newSize > size()`, then
    /// `newSize - size()` elements with default values are appended to the
    /// array.  If `newSize < size()`, then the `size() - newSize` elements at
    /// the end of the array are destroyed.  The behavior is undefined unless
    /// `0 <= newSize`.
    void resize(int newSize) const;

    // ACCESSORS

    /// Invoke the specified `accessor` on the array element at the specified
    /// `index`.  The supplied `accessor` must be a callable type that can be
    /// called as if it had the following signature:
    /// ```
    /// int accessor(const ELEMENT& refOrValue);
    /// ```
    /// Return the value from the invocation of `accessor`.  The behavior is
    /// undefined unless `0 <= index < size()`.
    template <class t_ACCESSOR>
    int accessElement(t_ACCESSOR& accessor, int index) const;

    /// Return the element type category of the wrapped array.
    bdlat_TypeCategory::Value elementCategory() const;

    /// Return a pointer to the vtable.
    const ArrayVtable *vtable() const;

    /// Return a pointer to the wrapped array.
    void *objectAddress() const;

    /// Return the number of elements in the wrapped array.
    bsl::size_t size() const;
};

                           // ===================
                           // class ArrayConstRef
                           // ===================

class ArrayConstRef {
    // DATA
    const void             *d_object_p;
    const ArrayConstVtable *d_vtable_p;

  public:
    // CREATORS

    /// Create `ArrayConstRef` object that type erases the supplied `array`
    /// object.
    template <class t_ARRAY>
    explicit ArrayConstRef(const t_ARRAY& array,
                           typename bsl::enable_if<
                               IsArray<t_ARRAY>::value>::type * = 0);

    /// Create `ArrayConstRef` object.  The behaviour is undefined unless
    /// `objectAddress` and `vtable` point to the valid objects.
    ArrayConstRef(const void *objectAddress, const ArrayConstVtable *vtable);

    /// Create `ArrayConstRef` object that wraps the same object as the
    /// specified `object`.
    ArrayConstRef(const ArrayRef& object); // IMPLICIT

    // ACCESSORS

    /// Invoke the specified `accessor` on the array element at the specified
    /// `index`.  The supplied `accessor` must be a callable type that can be
    /// called as if it had the following signature:
    /// ```
    /// int accessor(const ELEMENT& ref);
    /// ```
    /// Return the value from the invocation of `accessor`.  The behavior is
    /// undefined unless `0 <= index < size()`.
    template <class t_ACCESSOR>
    int accessElement(t_ACCESSOR& accessor, int index) const;

    /// Return the element type category of the wrapped array.
    bdlat_TypeCategory::Value elementCategory() const;

    /// Return a pointer to the vtable.
    const ArrayConstVtable *vtable() const;

    /// Return a pointer to the wrapped array.
    const void *objectAddress() const;

    /// Return the number of elements in the wrapped array.
    bsl::size_t size() const;

    /// Return the `bdlat_TypeName::xsdName` value for the underlying object.
    const char *xsdName(int format) const;
};

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

                           // --------------
                           // class ArrayRef
                           // --------------

// CREATORS
template <class t_ARRAY>
inline
ArrayRef::ArrayRef(t_ARRAY& array,
                   typename bsl::enable_if<
                       IsArray<t_ARRAY>::value>::type *)
: d_object_p(&array)
, d_vtable_p(ArrayVtableUtil::getVtable<t_ARRAY>())
{
}

inline
ArrayRef::ArrayRef(void *objectAddress, const ArrayVtable *vtable)
: d_object_p(objectAddress)
, d_vtable_p(vtable)
{
}

// MANIPULATORS
template <class t_MANIPULATOR>
inline
int ArrayRef::manipulateElement(t_MANIPULATOR& manipulator, int index) const
{
    typename FakeDependency<t_MANIPULATOR, ManipulatorRef>::Type
                                                   manipulatorRef(manipulator);
    return d_vtable_p->d_manipulateElement_fp(d_object_p,
                                              manipulatorRef,
                                              index);
}

inline
void ArrayRef::reset() const
{
    return d_vtable_p->d_reset_fp(d_object_p);
}

inline
void ArrayRef::resize(int newSize) const
{
    return d_vtable_p->d_resize_fp(d_object_p, newSize);
}

// ACCESSORS
template <class t_ACCESSOR>
inline
int ArrayRef::accessElement(t_ACCESSOR& accessor, int index) const
{
    typename FakeDependency<t_ACCESSOR, AccessorRef>::Type
                                                         accessorRef(accessor);
    return d_vtable_p->d_const.d_accessElement_fp(d_object_p,
                                                  accessorRef,
                                                  index);
}

inline
bdlat_TypeCategory::Value ArrayRef::elementCategory() const
{
    return d_vtable_p->d_const.d_elementCategory_fp();
}

inline
const ArrayVtable *ArrayRef::vtable() const
{
    return d_vtable_p;
}

inline
void *ArrayRef::objectAddress() const
{
    return d_object_p;
}

inline
bsl::size_t ArrayRef::size() const
{
    return d_vtable_p->d_const.d_size_fp(d_object_p);
}

// FREE FUNCTIONS
// Customization-point functions (`bdlat_ArrayFunctions`)

template <class t_MANIPULATOR>
inline
int bdlat_arrayManipulateElement(ArrayRef       *array,
                                 t_MANIPULATOR&  manipulator,
                                 int             index)
{
    return array->manipulateElement(manipulator, index);
}

inline
void bdlat_arrayResize(ArrayRef *array, int newSize)
{
    return array->resize(newSize);
}

template <class t_ACCESSOR>
inline
int bdlat_arrayAccessElement(const ArrayRef& array,
                             t_ACCESSOR&     accessor,
                             int             index)
{
    return array.accessElement(accessor, index);
}

inline
bsl::size_t bdlat_arraySize(const ArrayRef& array)
{
    return array.size();
}

inline
void bdlat_valueTypeReset(ArrayRef *ref)
{
    ref->reset();
}

                           // -------------------
                           // class ArrayConstRef
                           // -------------------

// CREATORS
template <class t_ARRAY>
inline
ArrayConstRef::ArrayConstRef(const t_ARRAY& array,
                             typename bsl::enable_if<
                                 IsArray<t_ARRAY>::value>::type *)
: d_object_p(&array)
, d_vtable_p(ArrayVtableUtil::getConstVtable<t_ARRAY>())
{
}

inline
ArrayConstRef::ArrayConstRef(const void             *objectAddress,
                             const ArrayConstVtable *vtable)
: d_object_p(objectAddress)
, d_vtable_p(vtable)
{
}

inline
ArrayConstRef::ArrayConstRef(const ArrayRef& object)
: d_object_p(object.objectAddress())
, d_vtable_p(&object.vtable()->d_const)
{
}

// ACCESSORS
template <class t_ACCESSOR>
inline
int ArrayConstRef::accessElement(t_ACCESSOR& accessor, int index) const
{
    typename FakeDependency<t_ACCESSOR, AccessorRef>::Type
                                                         accessorRef(accessor);
    return d_vtable_p->d_accessElement_fp(d_object_p, accessorRef, index);
}

inline
bdlat_TypeCategory::Value ArrayConstRef::elementCategory() const
{
    return d_vtable_p->d_elementCategory_fp();
}

inline
const ArrayConstVtable *ArrayConstRef::vtable() const
{
    return d_vtable_p;
}

inline
const void *ArrayConstRef::objectAddress() const
{
    return d_object_p;
}

inline
bsl::size_t ArrayConstRef::size() const
{
    return d_vtable_p->d_size_fp(d_object_p);
}

inline
const char *ArrayConstRef::xsdName(int format) const
{
    return d_vtable_p->d_xsdName_fp(d_object_p, format);
}

// FREE FUNCTIONS
// Customization-point functions (`bdlat_ArrayFunctions`)

template <class t_ACCESSOR>
inline
int bdlat_arrayAccessElement(const ArrayConstRef& array,
                             t_ACCESSOR&          accessor,
                             int                  index)
{
    return array.accessElement(accessor, index);
}

inline
bsl::size_t bdlat_arraySize(const ArrayConstRef& array)
{
    return array.size();
}

inline
const char *bdlat_TypeName_xsdName(const ArrayConstRef& ref, int format)
{
    return ref.xsdName(format);
}

}  // close package namespace

namespace bdlat_ArrayFunctions {

template <>
struct IsArray<bdlar::ArrayRef> : bsl::true_type {
};

template <>
struct IsArray<bdlar::ArrayConstRef> : bsl::true_type {
};

template <>
struct ElementType<bdlar::ArrayRef> {
    typedef bdlar::AnyRef Type;
};

template <>
struct ElementType<bdlar::ArrayConstRef> {
    typedef bdlar::AnyConstRef Type;
};

}  // close namespace bdlat_ArrayFunctions
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
