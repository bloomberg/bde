// bdlar_manipulatorvtable.h                                          -*-C++-*-
#ifndef INCLUDED_BDLAR_MANIPULATORVTABLE
#define INCLUDED_BDLAR_MANIPULATORVTABLE

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide ...
//
//@CLASSES:
//  bdlar::ManipulatorVtable: ...
//  bdlar::ManipulatorWithInfoVtable: ...
//
//@DESCRIPTION: This component provides ...
//

namespace BloombergLP {
namespace bdlar {

// Forward
class DynamicTypeRef;
class ArrayRef;
class ChoiceRef;
class CustomizedTypeRef;
class EnumRef;
class NullableValueRef;
class SequenceRef;
class SimpleTypeRef;

class FieldInfoRef;

                           // ========================
                           // struct ManipulatorVtable
                           // ========================

struct ManipulatorVtable {
    // TYPES
    typedef int ManipulateDynamic(const void     *manipulator,
                                  DynamicTypeRef *ref);
    typedef int ManipulateArray(const void *manipulator, ArrayRef *ref);
    typedef int ManipulateChoice(const void *manipulator, ChoiceRef *ref);
    typedef int ManipulateCustomized(const void        *manipulator,
                                     CustomizedTypeRef *ref);
    typedef int ManipulateEnum(const void *manipulator, EnumRef *ref);
    typedef int ManipulateNullable(const void       *manipulator,
                                   NullableValueRef *ref);
    typedef int ManipulateSequence(const void *manipulator, SequenceRef *ref);
    typedef int ManipulateSimple(const void *manipulator, SimpleTypeRef *ref);

    // PUBLIC DATA
    ManipulateDynamic    *d_manipulateDynamic_fp;
    ManipulateArray      *d_manipulateArray_fp;
    ManipulateChoice     *d_manipulateChoice_fp;
    ManipulateCustomized *d_manipulateCustomized_fp;
    ManipulateEnum       *d_manipulateEnum_fp;
    ManipulateNullable   *d_manipulateNullable_fp;
    ManipulateSequence   *d_manipulateSequence_fp;
    ManipulateSimple     *d_manipulateSimple_fp;
};

                           // ================================
                           // struct ManipulatorWithInfoVtable
                           // ================================

struct ManipulatorWithInfoVtable {
    // TYPES
    typedef int ManipulateDynamic(const void          *manipulator,
                                  DynamicTypeRef      *ref,
                                  const FieldInfoRef&  info);
    typedef int ManipulateArray(const void          *manipulator,
                                ArrayRef            *ref,
                                const FieldInfoRef&  info);
    typedef int ManipulateChoice(const void          *manipulator,
                                 ChoiceRef           *ref,
                                 const FieldInfoRef&  info);
    typedef int ManipulateCustomized(const void          *manipulator,
                                     CustomizedTypeRef   *ref,
                                     const FieldInfoRef&  info);
    typedef int ManipulateEnum(const void          *manipulator,
                               EnumRef             *ref,
                               const FieldInfoRef&  info);
    typedef int ManipulateNullable(const void          *manipulator,
                                   NullableValueRef    *ref,
                                   const FieldInfoRef&  info);
    typedef int ManipulateSequence(const void          *manipulator,
                                   SequenceRef         *ref,
                                   const FieldInfoRef&  info);
    typedef int ManipulateSimple(const void          *manipulator,
                                 SimpleTypeRef       *ref,
                                 const FieldInfoRef&  info);

    // PUBLIC DATA
    ManipulateDynamic    *d_manipulateDynamic_fp;
    ManipulateArray      *d_manipulateArray_fp;
    ManipulateChoice     *d_manipulateChoice_fp;
    ManipulateCustomized *d_manipulateCustomized_fp;
    ManipulateEnum       *d_manipulateEnum_fp;
    ManipulateNullable   *d_manipulateNullable_fp;
    ManipulateSequence   *d_manipulateSequence_fp;
    ManipulateSimple     *d_manipulateSimple_fp;
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
