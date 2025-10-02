// bdlar_choicevtable.h                                               -*-C++-*-
#ifndef INCLUDED_BDLAR_CHOICEVTABLE
#define INCLUDED_BDLAR_CHOICEVTABLE

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide ...
//
//@CLASSES:
//  bdlar::ChoiceVtable: ...
//  bdlar::ChoiceConstVtable: ...
//
//@DESCRIPTION: This component provides ...
//

namespace BloombergLP {
namespace bdlar {

// Forward
class AccessorWithInfoRef;
class ManipulatorWithInfoRef;

                           // ========================
                           // struct ChoiceConstVtable
                           // ========================

/// This struct stores a series of function pointers to wrappers that invoke
/// the underlying methods with read-only access of the supplied type erased
/// object that implement the "choice" type category.
struct ChoiceConstVtable {
    // TYPES
    typedef int SelectionId(const void *object);
    typedef bool HasSelectionId(const void *object, int selectionId);
    typedef bool HasSelectionName(const void *object,
                                  const char *selectionName,
                                  int         selectionNameLength);
    typedef int AccessSelection(const void           *object,
                                AccessorWithInfoRef&  accessor);
    typedef const char *XsdName(const void *object, int format);

    // PUBLIC DATA
    SelectionId      *d_selectionId_fp;
    HasSelectionId   *d_hasSelectionId_fp;
    HasSelectionName *d_hasSelectionName_fp;
    AccessSelection  *d_accessSelection_fp;
    XsdName          *d_xsdName_fp;
};

                           // ===================
                           // struct ChoiceVtable
                           // ===================

/// This struct stores a series of function pointers to wrappers that invoke
/// the underlying methods of the supplied type erased object that implement
/// the "choice" type category.
struct ChoiceVtable {
    // TYPES
    typedef int MakeSelectionById(void *object, int selectionId);
    typedef int MakeSelectionByName(void       *object,
                                    const char *selectionName,
                                    int         selectionNameLength);
    typedef void Reset(void *object);
    typedef int ManipulateSelection(void                    *object,
                                    ManipulatorWithInfoRef&  manipulator);

    // PUBLIC DATA
    ChoiceConstVtable    d_const;
    MakeSelectionById   *d_makeSelectionById_fp;
    MakeSelectionByName *d_makeSelectionByName_fp;
    ManipulateSelection *d_manipulateSelection_fp;
    Reset               *d_reset_fp;
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
