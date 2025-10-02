// bdlar_arrayvtable.h                                                -*-C++-*-
#ifndef INCLUDED_BDLAR_ARRAYVTABLE
#define INCLUDED_BDLAR_ARRAYVTABLE

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide ...
//
//@CLASSES:
//  bdlar::ArrayVtable: ...
//  bdlar::ArrayConstVtable: ...
//
//@DESCRIPTION: This component provides ...
//

#include <bdlat_typecategory.h>

namespace BloombergLP {
namespace bdlar {

// Forward
class AccessorRef;
class ManipulatorRef;

                           // =======================
                           // struct ArrayConstVtable
                           // =======================

/// This struct stores a series of function pointers to wrappers that invoke
/// the underlying methods with read-only access of the supplied type erased
/// object that implement the "array" type category.
struct ArrayConstVtable {
    // TYPES
    typedef bdlat_TypeCategory::Value ElementCategory();
    typedef int AccessElement(const void   *array,
                              AccessorRef&  accessor,
                              int           index);
    typedef bsl::size_t Size(const void *array);
    typedef const char *XsdName(const void *object, int format);

    // PUBLIC DATA
    ElementCategory *d_elementCategory_fp;
    AccessElement   *d_accessElement_fp;
    Size            *d_size_fp;
    XsdName         *d_xsdName_fp;
};

                           // ==================
                           // struct ArrayVtable
                           // ==================

/// This struct stores a series of function pointers to wrappers that invoke
/// the underlying methods of the supplied type erased object that implement
/// the "array" type category.
struct ArrayVtable {
    // TYPES
    typedef int ManipulateElement(void            *array,
                                  ManipulatorRef&  manipulator,
                                  int              index);
    typedef void Reset(void *object);
    typedef void Resize(void *array, int newSize);

    // PUBLIC DATA
    ArrayConstVtable   d_const;
    ManipulateElement *d_manipulateElement_fp;
    Reset             *d_reset_fp;
    Resize            *d_resize_fp;
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
