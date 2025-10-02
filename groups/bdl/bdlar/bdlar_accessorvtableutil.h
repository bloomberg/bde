// bdlar_accessorvtableutil.h                                         -*-C++-*-
#ifndef INCLUDED_BDLAR_ACCESSORVTABLEUTIL
#define INCLUDED_BDLAR_ACCESSORVTABLEUTIL

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide ...
//
//@CLASSES:
//  bdlar::AccessorVtableUtil: ...
//
//@DESCRIPTION: This component provides ...
//

#include <bdlar_accessorvtable.h>
#include <bdlar_simpletyperef.h>

namespace BloombergLP {
namespace bdlar {

                           // ========================
                           // class AccessorVtableUtil
                           // ========================

class AccessorVtableUtil {
    // PRIVATE CLASS METHODS

    /// Invoke the specified accessor on the specified `ref`.
    template <class t_ACCESSOR, class t_REF>
    static int access(const void *accessorPtr, const t_REF& ref);

    /// Invoke the specified accessor on the specified simple type `ref`.
    template <class t_ACCESSOR>
    static int accessSimple(const void                *accessorPtr,
                            const SimpleTypeConstRef&  ref);

    /// Invoke the specified accessor on the specified simple type `ref` with
    /// the specified `info`.
    template <class t_ACCESSOR>
    static int accessSimpleWithInfo(const void                *accessorPtr,
                                    const SimpleTypeConstRef&  ref,
                                    const FieldInfoRef&        info);

    /// Invoke the specified accessor on the specified `ref` with the specified
    /// `info`.
    template <class t_ACCESSOR, class t_REF>
    static int accessWithInfo(const void          *accessorPtr,
                              const t_REF&         ref,
                              const FieldInfoRef&  info);

  public:
    // CLASS METHODS

    /// Construct and return a `AccessorVtable` object with a series of
    /// function pointers that implement the accessors.
    template <class t_ACCESSOR>
    static const AccessorVtable *getVtable();

    /// Construct and return a `AccessorWithInfoVtable` object with a series of
    /// function pointers that implement the accessors.
    template <class t_ACCESSOR>
    static const AccessorWithInfoVtable *getWithInfoVtable();
};

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

                           // ------------------------
                           // class AccessorVtableUtil
                           // ------------------------

// PRIVATE CLASS METHODS
template <class t_ACCESSOR, class t_REF>
int AccessorVtableUtil::access(const void *accessorPtr, const t_REF& ref)
{
    t_ACCESSOR& accessor = *reinterpret_cast<t_ACCESSOR *>(
                                              const_cast<void *>(accessorPtr));
    return accessor(ref);
}

template <class t_ACCESSOR>
int AccessorVtableUtil::accessSimple(const void                *accessorPtr,
                                     const SimpleTypeConstRef&  ref)
{
    return ref.access(*reinterpret_cast<t_ACCESSOR *>(
                                             const_cast<void *>(accessorPtr)));
}

template <class t_ACCESSOR>
int AccessorVtableUtil::accessSimpleWithInfo(
                                        const void                *accessorPtr,
                                        const SimpleTypeConstRef&  ref,
                                        const FieldInfoRef&        info)
{
    return ref.access(*reinterpret_cast<t_ACCESSOR *>(
                                              const_cast<void *>(accessorPtr)),
                      info);
}

template <class t_ACCESSOR, class t_REF>
int AccessorVtableUtil::accessWithInfo(const void          *accessorPtr,
                                       const t_REF&         ref,
                                       const FieldInfoRef&  info)
{
    t_ACCESSOR& accessor = *reinterpret_cast<t_ACCESSOR *>(
                                              const_cast<void *>(accessorPtr));
    return accessor(ref, info);
}

// CLASS METHODS
template <class t_ACCESSOR>
const AccessorVtable *AccessorVtableUtil::getVtable()
{
    static const AccessorVtable vtable = {
        &access<t_ACCESSOR, DynamicTypeConstRef>,
        &access<t_ACCESSOR, ArrayConstRef>,
        &access<t_ACCESSOR, ChoiceConstRef>,
        &access<t_ACCESSOR, CustomizedTypeConstRef>,
        &access<t_ACCESSOR, EnumConstRef>,
        &access<t_ACCESSOR, NullableValueConstRef>,
        &access<t_ACCESSOR, SequenceConstRef>,
        &accessSimple<t_ACCESSOR>
    };
    return &vtable;
}

template <class t_ACCESSOR>
const AccessorWithInfoVtable *AccessorVtableUtil::getWithInfoVtable()
{
    static const AccessorWithInfoVtable vtable = {
        &accessWithInfo<t_ACCESSOR, DynamicTypeConstRef>,
        &accessWithInfo<t_ACCESSOR, ArrayConstRef>,
        &accessWithInfo<t_ACCESSOR, ChoiceConstRef>,
        &accessWithInfo<t_ACCESSOR, CustomizedTypeConstRef>,
        &accessWithInfo<t_ACCESSOR, EnumConstRef>,
        &accessWithInfo<t_ACCESSOR, NullableValueConstRef>,
        &accessWithInfo<t_ACCESSOR, SequenceConstRef>,
        &accessSimpleWithInfo<t_ACCESSOR>
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
