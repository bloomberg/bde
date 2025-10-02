// bdlar_anyvtable.h                                                  -*-C++-*-
#ifndef INCLUDED_BDLAR_ANYVTABLE
#define INCLUDED_BDLAR_ANYVTABLE

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide ...
//
//@CLASSES:
//  bdlar::AnyVtable: ...
//  bdlar::AnyConstVtable: ...
//
//@DESCRIPTION: This component provides ...
//

#include <bdlat_typecategory.h>

namespace BloombergLP {
namespace bdlar {

// Forward
struct SimpleTypeVtable;
struct SimpleTypeConstVtable;
struct EnumVtable;
struct EnumConstVtable;
struct NullableValueVtable;
struct NullableValueConstVtable;
struct ArrayVtable;
struct ArrayConstVtable;
struct SequenceVtable;
struct SequenceConstVtable;
struct ChoiceVtable;
struct ChoiceConstVtable;
struct CustomizedTypeVtable;
struct CustomizedTypeConstVtable;
struct DynamicTypeVtable;
struct DynamicTypeConstVtable;

                           // ===============
                           // union AnyVtable
                           // ===============

union AnyVtable {
    const SimpleTypeVtable     *d_simple_p;
    const EnumVtable           *d_enum_p;
    const NullableValueVtable  *d_nullable_p;
    const ArrayVtable          *d_array_p;
    const SequenceVtable       *d_sequence_p;
    const ChoiceVtable         *d_choice_p;
    const CustomizedTypeVtable *d_customized_p;
    const DynamicTypeVtable    *d_dynamic_p;

    // CREATORS
    AnyVtable();
    AnyVtable(const SimpleTypeVtable     *vt);
    AnyVtable(const EnumVtable           *vt);
    AnyVtable(const NullableValueVtable  *vt);
    AnyVtable(const ArrayVtable          *vt);
    AnyVtable(const SequenceVtable       *vt);
    AnyVtable(const ChoiceVtable         *vt);
    AnyVtable(const CustomizedTypeVtable *vt);
    AnyVtable(const DynamicTypeVtable    *vt);
};

                           // ====================
                           // union AnyConstVtable
                           // ====================

union AnyConstVtable {
    const SimpleTypeConstVtable     *d_simple_p;
    const EnumConstVtable           *d_enum_p;
    const NullableValueConstVtable  *d_nullable_p;
    const ArrayConstVtable          *d_array_p;
    const SequenceConstVtable       *d_sequence_p;
    const ChoiceConstVtable         *d_choice_p;
    const CustomizedTypeConstVtable *d_customized_p;
    const DynamicTypeConstVtable    *d_dynamic_p;

    // CREATORS
    AnyConstVtable();
    AnyConstVtable(const SimpleTypeConstVtable     *vt);
    AnyConstVtable(const EnumConstVtable           *vt);
    AnyConstVtable(const NullableValueConstVtable  *vt);
    AnyConstVtable(const ArrayConstVtable          *vt);
    AnyConstVtable(const SequenceConstVtable       *vt);
    AnyConstVtable(const ChoiceConstVtable         *vt);
    AnyConstVtable(const CustomizedTypeConstVtable *vt);
    AnyConstVtable(const DynamicTypeConstVtable    *vt);
};

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

                           // ---------------
                           // union AnyVtable
                           // ---------------

// CREATORS
inline
AnyVtable::AnyVtable()
{
}

inline
AnyVtable::AnyVtable(const SimpleTypeVtable *vt)
: d_simple_p(vt)
{
}

inline
AnyVtable::AnyVtable(const EnumVtable *vt)
: d_enum_p(vt)
{
}

inline
AnyVtable::AnyVtable(const NullableValueVtable *vt)
: d_nullable_p(vt)
{
}

inline
AnyVtable::AnyVtable(const ArrayVtable *vt)
: d_array_p(vt)
{
}

inline
AnyVtable::AnyVtable(const SequenceVtable *vt)
: d_sequence_p(vt)
{
}

inline
AnyVtable::AnyVtable(const ChoiceVtable *vt)
: d_choice_p(vt)
{
}

inline
AnyVtable::AnyVtable(const CustomizedTypeVtable *vt)
: d_customized_p(vt)
{
}

inline
AnyVtable::AnyVtable(const DynamicTypeVtable *vt)
: d_dynamic_p(vt)
{
}

                        // --------------------
                        // union AnyConstVtable
                        // --------------------

// CREATORS
inline
AnyConstVtable::AnyConstVtable()
{
}

inline
AnyConstVtable::AnyConstVtable(const SimpleTypeConstVtable *vt)
: d_simple_p(vt)
{
}

inline
AnyConstVtable::AnyConstVtable(const EnumConstVtable *vt)
: d_enum_p(vt)
{
}

inline
AnyConstVtable::AnyConstVtable(const NullableValueConstVtable *vt)
: d_nullable_p(vt)
{
}

inline
AnyConstVtable::AnyConstVtable(const ArrayConstVtable *vt)
: d_array_p(vt)
{
}

inline
AnyConstVtable::AnyConstVtable(const SequenceConstVtable *vt)
: d_sequence_p(vt)
{
}

inline
AnyConstVtable::AnyConstVtable(const ChoiceConstVtable *vt)
: d_choice_p(vt)
{
}

inline
AnyConstVtable::AnyConstVtable(const CustomizedTypeConstVtable *vt)
: d_customized_p(vt)
{
}

inline
AnyConstVtable::AnyConstVtable(const DynamicTypeConstVtable *vt)
: d_dynamic_p(vt)
{
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
