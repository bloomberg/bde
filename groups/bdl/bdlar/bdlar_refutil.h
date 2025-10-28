// bdlar_refutil.h                                                    -*-C++-*-
#ifndef INCLUDED_BDLAR_REFUTIL
#define INCLUDED_BDLAR_REFUTIL

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide ...
//
//@CLASSES:
//  bdlar::RefUtil: ...
//
//@DESCRIPTION: This component provides ...
//

#include <bdlar_anyref.h>
#include <bdlar_arrayref.h>
#include <bdlar_choiceref.h>
#include <bdlar_customizedtyperef.h>
#include <bdlar_customizedtypetraits.h>
#include <bdlar_dynamictyperef.h>
#include <bdlar_enumref.h>
#include <bdlar_nullablevalueref.h>
#include <bdlar_sequenceref.h>
#include <bdlar_simpletyperef.h>

#include <bdlar_anycastutil.h>
#include <bdlar_anymanipulators.h> // not used here, included for bdlar users

#include <bdlar_typecategory.h>

#include <bdlat_arrayfunctions.h>
#include <bdlat_choicefunctions.h>
#include <bdlat_customizedtypefunctions.h>
#include <bdlat_enumfunctions.h>
#include <bdlat_nullablevaluefunctions.h>
#include <bdlat_sequencefunctions.h>

#include <bsls_assert.h>

#include <bsl_type_traits.h>

namespace BloombergLP {
namespace bdlar {

                           // =============
                           // class RefUtil
                           // =============

class RefUtil {
  public:
    // CLASS METHODS

    /// Make `AnyConstRef` to the specified `object`.
    template <class t_TYPE>
    static typename bsl::enable_if<!IsDynamic<t_TYPE>::value,AnyConstRef>::type
    makeAnyConstRef(const t_TYPE& object);

    /// Make `AnyConstRef` to the specified dynamic `object`.
    template <class t_TYPE>
    static typename bsl::enable_if<IsDynamic<t_TYPE>::value, AnyConstRef>::type
    makeAnyConstRef(const t_TYPE& object);

    /// Make `AnyRef` to the specified `object`.
    template <class t_TYPE>
    static typename bsl::enable_if<!IsDynamic<t_TYPE>::value, AnyRef>::type
    makeAnyRef(t_TYPE& object);

    /// Make `AnyRef` to the specified dynamic `object`.
    template <class t_TYPE>
    static typename bsl::enable_if<IsDynamic<t_TYPE>::value, AnyRef>::type
    makeAnyRef(t_TYPE& object);

    /// Make `ArrayRef` to the specified "array" `object`.
    template <class t_TYPE>
    static typename bsl::enable_if<IsArray<t_TYPE>::value,
    ArrayRef>::type wrap(t_TYPE& object);

    /// Make `ArrayConstRef` to the specified "array" `object`.
    template <class t_TYPE>
    static typename bsl::enable_if<IsArray<t_TYPE>::value,
    ArrayConstRef>::type wrap(const t_TYPE& object);

    /// Make `ChoiceRef` to the specified "choice" `object`.
    template <class t_TYPE>
    static typename bsl::enable_if<IsChoice<t_TYPE>::value,
    ChoiceRef>::type wrap(t_TYPE& object);

    /// Make `ChoiceConstRef` to the specified "choice" `object`.
    template <class t_TYPE>
    static typename bsl::enable_if<IsChoice<t_TYPE>::value,
    ChoiceConstRef>::type wrap(const t_TYPE& object);

    /// Make `CustomizedTypeRef` to the specified "customized type" `object`.
    template <class t_TYPE>
    static typename bsl::enable_if<IsCustomized<t_TYPE>::value,
    CustomizedTypeRef>::type wrap(t_TYPE& object);

    /// Make `CustomizedTypeConstRef` to the specified "customized type"
    /// `object`.
    template <class t_TYPE>
    static typename bsl::enable_if<IsCustomized<t_TYPE>::value,
    CustomizedTypeConstRef>::type wrap(const t_TYPE& object);

    /// Make `EnumRef` to the specified "enumeration" `object`.
    template <class t_TYPE>
    static typename bsl::enable_if<IsEnum<t_TYPE>::value,
    EnumRef>::type wrap(t_TYPE& object);

    /// Make `EnumConstRef` to the specified "enumeration" `object`.
    template <class t_TYPE>
    static typename bsl::enable_if<IsEnum<t_TYPE>::value,
    EnumConstRef>::type wrap(const t_TYPE& object);

    /// Make `NullableValueRef` to the specified "nullable value" `object`.
    template <class t_TYPE>
    static typename bsl::enable_if<IsNullable<t_TYPE>::value,
    NullableValueRef>::type wrap(t_TYPE& object);

    /// Make `NullableValueConstRef` to the specified "nullable value"
    /// `object`.
    template <class t_TYPE>
    static typename bsl::enable_if<IsNullable<t_TYPE>::value,
    NullableValueConstRef>::type wrap(const t_TYPE& object);

    /// Make `SequenceRef` to the specified "sequence" `object`.
    template <class t_TYPE>
    static typename bsl::enable_if<IsSequence<t_TYPE>::value,
    SequenceRef>::type wrap(t_TYPE& object);

    /// Make `SequenceConstRef` to the specified "sequence" `object`.
    template <class t_TYPE>
    static typename bsl::enable_if<IsSequence<t_TYPE>::value,
    SequenceConstRef>::type wrap(const t_TYPE& object);

    /// Make `SimpleTypeRef` to the specified "simple" `object`.
    template <class t_TYPE>
    static typename bsl::enable_if<IsSimple<t_TYPE>::value,
    SimpleTypeRef>::type wrap(t_TYPE& object);

    /// Make `SimpleTypeConstRef` to the specified "simple" `object`.
    template <class t_TYPE>
    static typename bsl::enable_if<IsSimple<t_TYPE>::value,
    SimpleTypeConstRef>::type wrap(const t_TYPE& object);
};

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

                           // -------------
                           // class RefUtil
                           // -------------

template <class t_TYPE>
inline
typename bsl::enable_if<!IsDynamic<t_TYPE>::value, AnyConstRef>::type
RefUtil::makeAnyConstRef(const t_TYPE& object)
{
    return AnyCastUtil::toAnyConstRef(wrap(object));
}

template <class t_TYPE>
typename bsl::enable_if<IsDynamic<t_TYPE>::value, AnyConstRef>::type
RefUtil::makeAnyConstRef(const t_TYPE& object)
{
    return AnyCastUtil::toAnyConstRef(DynamicTypeConstRef(object));
}

template <class t_TYPE>
inline
typename bsl::enable_if<!IsDynamic<t_TYPE>::value, AnyRef>::type
RefUtil::makeAnyRef(t_TYPE& object)
{
    return AnyCastUtil::toAnyRef(wrap(object));
}

template <class t_TYPE>
typename bsl::enable_if<IsDynamic<t_TYPE>::value, AnyRef>::type
RefUtil::makeAnyRef(t_TYPE& object)
{
    return AnyCastUtil::toAnyRef(DynamicTypeRef(object));
}

template <class t_TYPE>
inline
typename bsl::enable_if<IsArray<t_TYPE>::value,
ArrayRef>::type RefUtil::wrap(t_TYPE& object)
{
    return ArrayRef(object);
}

template <class t_TYPE>
inline
typename bsl::enable_if<IsArray<t_TYPE>::value,
ArrayConstRef>::type RefUtil::wrap(const t_TYPE& object)
{
    return ArrayConstRef(object);
}

template <class t_TYPE>
inline
typename bsl::enable_if<IsChoice<t_TYPE>::value,
ChoiceRef>::type RefUtil::wrap(t_TYPE& object)
{
    return ChoiceRef(object);
}

template <class t_TYPE>
inline
typename bsl::enable_if<IsChoice<t_TYPE>::value,
ChoiceConstRef>::type RefUtil::wrap(const t_TYPE& object)
{
    return ChoiceConstRef(object);
}

template <class t_TYPE>
inline
typename bsl::enable_if<IsCustomized<t_TYPE>::value,
CustomizedTypeRef>::type RefUtil::wrap(t_TYPE& object)
{
    return CustomizedTypeRef(object);
}

template <class t_TYPE>
inline
typename bsl::enable_if<IsCustomized<t_TYPE>::value,
CustomizedTypeConstRef>::type RefUtil::wrap(const t_TYPE& object)
{
    return CustomizedTypeConstRef(object);
}

template <class t_TYPE>
inline
typename bsl::enable_if<IsEnum<t_TYPE>::value,
EnumRef>::type RefUtil::wrap(t_TYPE& object)
{
    return EnumRef(object);
}

template <class t_TYPE>
inline
typename bsl::enable_if<IsEnum<t_TYPE>::value,
EnumConstRef>::type RefUtil::wrap(const t_TYPE& object)
{
    return EnumConstRef(object);
}

template <class t_TYPE>
inline
typename bsl::enable_if<IsNullable<t_TYPE>::value,
NullableValueRef>::type RefUtil::wrap(t_TYPE& object)
{
    return NullableValueRef(object);
}

template <class t_TYPE>
inline
typename bsl::enable_if<IsNullable<t_TYPE>::value,
NullableValueConstRef>::type RefUtil::wrap(const t_TYPE& object)
{
    return NullableValueConstRef(object);
}

template <class t_TYPE>
inline
typename bsl::enable_if<IsSequence<t_TYPE>::value,
SequenceRef>::type RefUtil::wrap(t_TYPE& object)
{
    return SequenceRef(object);
}

template <class t_TYPE>
inline
typename bsl::enable_if<IsSequence<t_TYPE>::value,
SequenceConstRef>::type RefUtil::wrap(const t_TYPE& object)
{
    return SequenceConstRef(object);
}

template <class t_TYPE>
inline
typename bsl::enable_if<IsSimple<t_TYPE>::value,
SimpleTypeRef>::type RefUtil::wrap(t_TYPE& object)
{
    return SimpleTypeRef(object);
}

template <class t_TYPE>
inline
typename bsl::enable_if<IsSimple<t_TYPE>::value,
SimpleTypeConstRef>::type RefUtil::wrap(const t_TYPE& object)
{
    return SimpleTypeConstRef(object);
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
