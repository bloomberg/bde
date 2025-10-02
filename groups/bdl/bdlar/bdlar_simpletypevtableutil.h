// bdlar_simpletypevtableutil.h                                       -*-C++-*-
#ifndef INCLUDED_BDLAR_SIMPLETYPEEVTABLEUTIL
#define INCLUDED_BDLAR_SIMPLETYPEEVTABLEUTIL

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide ...
//
//@CLASSES:
//  bdlar::SimpleTypeVtableUtil: ...
//
//@DESCRIPTION: This component provides ...
//

#include <bdlar_simpletypevtable.h>
#include <bdlar_typecategory.h>

#include <bdlat_typename.h>
#include <bdlat_valuetypefunctions.h>

namespace BloombergLP {
namespace bdlar {

                           // ==========================
                           // class SimpleTypeVtableUtil
                           // ==========================

class SimpleTypeVtableUtil {
    // PRIVATE CLASS METHODS

    /// Reset the object pointed by the specified `object` to the default
    /// value.
    template <class t_SIMPLE>
    static void reset(void *object);

    /// Return the type ID corresponding to the simple type held by the
    /// type-erased `SimpleRef`.
    template <class t_SIMPLE>
    static SimpleTypeId typeId();

    /// Return the `bdlat_TypeName::xsdName` value for `object`.
    template <class t_SIMPLE>
    static const char *xsdName(const void *object, int format);

  public:
    // TYPES
    typedef SimpleTypeVtable      VtableType;
    typedef SimpleTypeConstVtable ConstVtableType;

    // CLASS METHODS

    /// Construct and return a `SimpleTypeConstVtable` object with a series of
    /// function pointers that implement the "simple type" category.
    template <class t_SIMPLE>
    static const SimpleTypeConstVtable *getConstVtable();

    /// Construct and return a `SimpleTypeVtable` object with a series of
    /// function pointers that implement the "simple type" category.
    template <class t_SIMPLE>
    static const SimpleTypeVtable *getVtable();
};

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

                          // --------------------------
                          // class SimpleTypeVtableUtil
                          // --------------------------

// PRIVATE CLASS METHODS
template <class t_SIMPLE>
void SimpleTypeVtableUtil::reset(void *object)
{
    bdlat_ValueTypeFunctions::reset(static_cast<t_SIMPLE *>(object));
}

template <class t_SIMPLE>
SimpleTypeId SimpleTypeVtableUtil::typeId()
{
    return SimpleTypeIdOf<t_SIMPLE>::value;
}

template <class t_SIMPLE>
const char *SimpleTypeVtableUtil::xsdName(const void *object, int format)
{
    return bdlat_TypeName::xsdName(*static_cast<const t_SIMPLE *>(object),
                                   format);
}

// CLASS METHODS
template <class t_SIMPLE>
const SimpleTypeConstVtable *SimpleTypeVtableUtil::getConstVtable()
{
    static const SimpleTypeConstVtable vtable = {
        &typeId<t_SIMPLE>,
        &xsdName<t_SIMPLE>
    };
    return &vtable;
}

template <class t_SIMPLE>
const SimpleTypeVtable *SimpleTypeVtableUtil::getVtable()
{
    static const SimpleTypeVtable vtable = {
        {
            &typeId<t_SIMPLE>,
            &xsdName<t_SIMPLE>

        },
        &reset<t_SIMPLE>
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
