// bdlar_manipulatorvtableutil.h                                      -*-C++-*-
#ifndef INCLUDED_BDLAR_MANIPULATORVTABLEUTIL
#define INCLUDED_BDLAR_MANIPULATORVTABLEUTIL

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide ...
//
//@CLASSES:
//  bdlar::ManipulatorVtableUtil: ...
//
//@DESCRIPTION: This component provides ...
//

#include <bdlar_manipulatorvtable.h>
#include <bdlar_simpletyperef.h>

namespace BloombergLP {
namespace bdlar {

                           // ===========================
                           // class ManipulatorVtableUtil
                           // ===========================

class ManipulatorVtableUtil {
    // PRIVATE CLASS METHODS

    /// Invoke the specified manipulator on the specified `ref`.
    template <class t_MANIPULATOR, class t_REF>
    static int manipulate(const void *manipulatorPtr, t_REF *ref);

    /// Invoke the specified manipulator on the specified simple type `ref`.
    template <class t_MANIPULATOR>
    static int manipulateSimple(const void    *manipulatorPtr,
                                SimpleTypeRef *ref);

    /// Invoke the specified manipulator on the specified simple type `ref`
    /// with the specified `info`.
    template <class t_MANIPULATOR>
    static int manipulateSimpleWithInfo(const void          *manipulatorPtr,
                                        SimpleTypeRef       *ref,
                                        const FieldInfoRef&  info);

    /// Invoke the specified manipulator on the specified `ref` with the
    /// specified `info`.
    template <class t_MANIPULATOR, class t_REF>
    static int manipulateWithInfo(const void          *manipulatorPtr,
                                  t_REF               *ref,
                                  const FieldInfoRef&  info);

  public:
    // CLASS METHODS

    /// Construct and return a `ManipulatorVtable` object with a series of
    /// function pointers that implement the manipulators.
    template <class t_MANIPULATOR>
    static const ManipulatorVtable *getVtable();

    /// Construct and return a `ManipulatorWithInfoVtable` object with a series
    /// of function pointers that implement the manipulators.
    template <class t_MANIPULATOR>
    static const ManipulatorWithInfoVtable *getWithInfoVtable();
};

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

                           // ---------------------------
                           // class ManipulatorVtableUtil
                           // ---------------------------

// PRIVATE CLASS METHODS
template <class t_MANIPULATOR, class t_REF>
int ManipulatorVtableUtil::manipulate(const void *manipulatorPtr, t_REF *ref)
{
    t_MANIPULATOR& manipulator = *reinterpret_cast<t_MANIPULATOR *>(
                                           const_cast<void *>(manipulatorPtr));
    return manipulator(ref);
}

template <class t_MANIPULATOR>
int ManipulatorVtableUtil::manipulateSimple(const void    *manipulatorPtr,
                                            SimpleTypeRef *ref)
{
    return ref->manipulate(*reinterpret_cast<t_MANIPULATOR *>(
                                          const_cast<void *>(manipulatorPtr)));
}

template <class t_MANIPULATOR>
int ManipulatorVtableUtil::manipulateSimpleWithInfo(
                                           const void          *manipulatorPtr,
                                           SimpleTypeRef       *ref,
                                           const FieldInfoRef&  info)
{
    return ref->manipulate(*reinterpret_cast<t_MANIPULATOR *>(
                                           const_cast<void *>(manipulatorPtr)),
                           info);
}

template <class t_MANIPULATOR, class t_REF>
int ManipulatorVtableUtil::manipulateWithInfo(
                                           const void          *manipulatorPtr,
                                           t_REF               *ref,
                                           const FieldInfoRef&  info)
{
    t_MANIPULATOR& manipulator = *reinterpret_cast<t_MANIPULATOR *>(
                                           const_cast<void *>(manipulatorPtr));
    return manipulator(ref, info);
}

// CLASS METHODS
template <class t_MANIPULATOR>
const ManipulatorVtable *ManipulatorVtableUtil::getVtable()
{
    static const ManipulatorVtable vtable = {
        &manipulate<t_MANIPULATOR, DynamicTypeRef>,
        &manipulate<t_MANIPULATOR, ArrayRef>,
        &manipulate<t_MANIPULATOR, ChoiceRef>,
        &manipulate<t_MANIPULATOR, CustomizedTypeRef>,
        &manipulate<t_MANIPULATOR, EnumRef>,
        &manipulate<t_MANIPULATOR, NullableValueRef>,
        &manipulate<t_MANIPULATOR, SequenceRef>,
        &manipulateSimple<t_MANIPULATOR>
    };
    return &vtable;
}

template <class t_MANIPULATOR>
const ManipulatorWithInfoVtable *ManipulatorVtableUtil::getWithInfoVtable()
{
    static const ManipulatorWithInfoVtable vtable = {
        &manipulateWithInfo<t_MANIPULATOR, DynamicTypeRef>,
        &manipulateWithInfo<t_MANIPULATOR, ArrayRef>,
        &manipulateWithInfo<t_MANIPULATOR, ChoiceRef>,
        &manipulateWithInfo<t_MANIPULATOR, CustomizedTypeRef>,
        &manipulateWithInfo<t_MANIPULATOR, EnumRef>,
        &manipulateWithInfo<t_MANIPULATOR, NullableValueRef>,
        &manipulateWithInfo<t_MANIPULATOR, SequenceRef>,
        &manipulateSimpleWithInfo<t_MANIPULATOR>
    };
    return &vtable;
}

}  // close package namespace
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
