// bdlar_accessorvtable.h                                             -*-C++-*-
#ifndef INCLUDED_BDLAR_ACCESSORVTABLE
#define INCLUDED_BDLAR_ACCESSORVTABLE

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide ...
//
//@CLASSES:
//  bdlar::AccessorVtable: ...
//
//@DESCRIPTION: This component provides ...
//

namespace BloombergLP {
namespace bdlar {

// Forward
class DynamicTypeConstRef;
class ArrayConstRef;
class ChoiceConstRef;
class CustomizedTypeConstRef;
class EnumConstRef;
class NullableValueConstRef;
class SequenceConstRef;
class SimpleTypeConstRef;

class FieldInfoRef;

                           // =====================
                           // struct AccessorVtable
                           // =====================

struct AccessorVtable {
    // TYPES
    typedef int AccessDynamic(const void                 *accessor,
                              const DynamicTypeConstRef&  ref);
    typedef int AccessArray(const void *accessor, const ArrayConstRef& ref);
    typedef int AccessChoice(const void *accessor, const ChoiceConstRef& ref);
    typedef int AccessCustomized(const void                    *accessor,
                                 const CustomizedTypeConstRef&  ref);
    typedef int AccessEnum(const void *accessor, const EnumConstRef& ref);
    typedef int AccessNullable(const void                   *accessor,
                               const NullableValueConstRef&  ref);
    typedef int AccessSequence(const void              *accessor,
                               const SequenceConstRef&  ref);
    typedef int AccessSimple(const void                *accessor,
                             const SimpleTypeConstRef&  ref);

    // PUBLIC DATA
    AccessDynamic    *d_accessDynamic_fp;
    AccessArray      *d_accessArray_fp;
    AccessChoice     *d_accessChoice_fp;
    AccessCustomized *d_accessCustomized_fp;
    AccessEnum       *d_accessEnum_fp;
    AccessNullable   *d_accessNullable_fp;
    AccessSequence   *d_accessSequence_fp;
    AccessSimple     *d_accessSimple_fp;
};

                           // =============================
                           // struct AccessorWithInfoVtable
                           // =============================

struct AccessorWithInfoVtable {
    // TYPES
    typedef int AccessDynamic(const void                 *accessor,
                              const DynamicTypeConstRef&  ref,
                              const FieldInfoRef&         info);
    typedef int AccessArray(const void           *accessor,
                            const ArrayConstRef&  ref,
                            const FieldInfoRef&   info);
    typedef int AccessChoice(const void            *accessor,
                             const ChoiceConstRef&  ref,
                             const FieldInfoRef&    info);
    typedef int AccessCustomized(const void                    *accessor,
                                 const CustomizedTypeConstRef&  ref,
                                 const FieldInfoRef&            info);
    typedef int AccessEnum(const void          *accessor,
                           const EnumConstRef&  ref,
                           const FieldInfoRef&  info);
    typedef int AccessNullable(const void                   *accessor,
                               const NullableValueConstRef&  ref,
                               const FieldInfoRef&           info);
    typedef int AccessSequence(const void              *accessor,
                               const SequenceConstRef&  ref,
                               const FieldInfoRef&      info);
    typedef int AccessSimple(const void                *accessor,
                             const SimpleTypeConstRef&  ref,
                             const FieldInfoRef&        info);

    // PUBLIC DATA
    AccessDynamic    *d_accessDynamic_fp;
    AccessArray      *d_accessArray_fp;
    AccessChoice     *d_accessChoice_fp;
    AccessCustomized *d_accessCustomized_fp;
    AccessEnum       *d_accessEnum_fp;
    AccessNullable   *d_accessNullable_fp;
    AccessSequence   *d_accessSequence_fp;
    AccessSimple     *d_accessSimple_fp;
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
