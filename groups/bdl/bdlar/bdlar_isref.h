// bdlar_isref.h                                                      -*-C++-*-
#ifndef INCLUDED_BDLAR_ISREF
#define INCLUDED_BDLAR_ISREF

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide ...
//
//@CLASSES:
//  bdlar::IsConstRef: ...
//  bdlar::IsMutableRef: ...
//  bdlar::IsRef: ...
//
//@DESCRIPTION: This component provides ...
//

#include <bsl_type_traits.h>

namespace BloombergLP {
namespace bdlar {

// Forward
class ArrayRef;
class ArrayConstRef;
class ChoiceRef;
class ChoiceConstRef;
class CustomizedTypeRef;
class CustomizedTypeConstRef;
class DynamicTypeRef;
class DynamicTypeConstRef;
class EnumRef;
class EnumConstRef;
class NullableValueRef;
class NullableValueConstRef;
class SequenceRef;
class SequenceConstRef;
class SimpleTypeRef;
class SimpleTypeConstRef;

                           // =================
                           // struct IsConstRef
                           // =================

template <class t_TYPE>
struct IsConstRef : bsl::false_type {
};

template <>
struct IsConstRef<ArrayConstRef> : bsl::true_type {
};

template <>
struct IsConstRef<ChoiceConstRef> : bsl::true_type {
};

template <>
struct IsConstRef<CustomizedTypeConstRef> : bsl::true_type {
};

template <>
struct IsConstRef<DynamicTypeConstRef> : bsl::true_type {
};

template <>
struct IsConstRef<EnumConstRef> : bsl::true_type {
};

template <>
struct IsConstRef<NullableValueConstRef> : bsl::true_type {
};

template <>
struct IsConstRef<SequenceConstRef> : bsl::true_type {
};

template <>
struct IsConstRef<SimpleTypeConstRef> : bsl::true_type {
};

                           // ===================
                           // struct IsMutableRef
                           // ===================

template <class t_TYPE>
struct IsMutableRef : bsl::false_type {
};

template <>
struct IsMutableRef<ArrayRef> : bsl::true_type {
};

template <>
struct IsMutableRef<ChoiceRef> : bsl::true_type {
};

template <>
struct IsMutableRef<CustomizedTypeRef> : bsl::true_type {
};

template <>
struct IsMutableRef<DynamicTypeRef> : bsl::true_type {
};

template <>
struct IsMutableRef<EnumRef> : bsl::true_type {
};

template <>
struct IsMutableRef<NullableValueRef> : bsl::true_type {
};

template <>
struct IsMutableRef<SequenceRef> : bsl::true_type {
};

template <>
struct IsMutableRef<SimpleTypeRef> : bsl::true_type {
};

                           // ============
                           // struct IsRef
                           // =============

template <class t_TYPE>
struct IsRef : bsl::disjunction<IsConstRef<t_TYPE>, IsMutableRef<t_TYPE> > {
};

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
