// bdlar_sequencevtable.h                                             -*-C++-*-
#ifndef INCLUDED_BDLAR_SEQUENCEVTABLE
#define INCLUDED_BDLAR_SEQUENCEVTABLE

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide ...
//
//@CLASSES:
//  bdlar::SequenceVtable: ...
//  bdlar::SequenceConstVtable: ...
//
//@DESCRIPTION: This component provides ...
//

namespace BloombergLP {
namespace bdlar {

// Forward
class AccessorWithInfoRef;
class ManipulatorWithInfoRef;

                           // ==========================
                           // struct SequenceConstVtable
                           // ==========================

/// This struct stores a series of function pointers to wrappers that invoke
/// the underlying methods with read-only access of the supplied type erased
/// object that implement the "sequence" type category.
struct SequenceConstVtable {
    // TYPES
    typedef bool HasAttributeName(const void *object,
                                  const char *attributeName,
                                  int         attributeNameLength);
    typedef bool HasAttributeId(const void *object, int attributeId);
    typedef int AccessAttributeById(const void           *object,
                                    AccessorWithInfoRef&  accessor,
                                    int                   attributeId);
    typedef int AccessAttributeByName(
                                    const void           *object,
                                    AccessorWithInfoRef&  accessor,
                                    const char           *attributeName,
                                    int                   attributeNameLength);
    typedef int AccessAttributes(const void           *object,
                                 AccessorWithInfoRef&  accessor);
    typedef const char *XsdName(const void *object, int format);

    // PUBLIC DATA
    HasAttributeName      *d_hasAttributeName_fp;
    HasAttributeId        *d_hasAttributeId_fp;
    AccessAttributeById   *d_accessAttributeById_fp;
    AccessAttributeByName *d_accessAttributeByName_fp;
    AccessAttributes      *d_accessAttributes_fp;
    XsdName               *d_xsdName_fp;
};

                           // =====================
                           // struct SequenceVtable
                           // =====================

/// This struct stores a series of function pointers to wrappers that invoke
/// the underlying methods of the supplied type erased object that implement
/// the "sequence" type category.
struct SequenceVtable {
    // TYPES
    typedef int ManipulateAttributeById(void                    *object,
                                        ManipulatorWithInfoRef&  manipilator,
                                        int                      attributeId);
    typedef int ManipulateAttributeByName(
                                 void                    *object,
                                 ManipulatorWithInfoRef&  manipilator,
                                 const char              *attributeName,
                                 int                      attributeNameLength);
    typedef int ManipulateAttributes(void                    *object,
                                     ManipulatorWithInfoRef&  manipilator);
    typedef void Reset(void *object);

    // PUBLIC DATA
    SequenceConstVtable        d_const;
    ManipulateAttributeById   *d_manipulateAttributeById_fp;
    ManipulateAttributeByName *d_manipulateAttributeByName_fp;
    ManipulateAttributes      *d_manipulateAttributes_fp;
    Reset                     *d_reset_fp;
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
