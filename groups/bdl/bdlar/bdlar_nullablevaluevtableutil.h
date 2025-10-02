// bdlar_nullablevaluevtableutil.h                                    -*-C++-*-
#ifndef INCLUDED_BDLAR_NULLABLEVALUEVTABLEUTIL
#define INCLUDED_BDLAR_NULLABLEVALUEVTABLEUTIL

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide ...
//
//@CLASSES:
//  bdlar::NullableValueVtableUtil: ...
//
//@DESCRIPTION: This component provides ...
//

#include <bdlar_nullablevaluevtable.h>
#include <bdlar_typecategory.h>

#include <bdlat_nullablevaluefunctions.h>
#include <bdlat_typename.h>
#include <bdlat_valuetypefunctions.h>

namespace BloombergLP {
namespace bdlar {

                           // =============================
                           // class NullableValueVtableUtil
                           // =============================

class NullableValueVtableUtil {
    // PRIVATE CLASS METHODS

    /// Invoke the specified `accessor` on non-null value of the specified
    /// `object`.  Return the value from the invocation of `accessor`.  The
    /// behavior is undefined if the referred object is null.
    template <class t_NULLABLE>
    static int accessValue(const void *object, AccessorRef& accessor);

    /// Return `true` if the specified "nullable" `object` contains a null
    /// value, and `false` otherwise.
    template <class t_NULLABLE>
    static bool isNull(const void *object);

    /// Assign to the specified "nullable" `object` the default value for the
    /// contained type.
    template <class t_NULLABLE>
    static void makeValue(void *object);

    /// Invoke the specified `manipulator` on non-null value of the specified
    /// `object`.  Return the value from the invocation of `manipulator`.  The
    /// behavior is undefined if the referred object is null.
    template <class t_NULLABLE>
    static int manipulateValue(void *object, ManipulatorRef& manipulator);

    /// Reset the object pointed by the specified `object` to the default
    /// value.
    template <class t_NULLABLE>
    static void reset(void *object);

    /// Return the value type category of the specified `t_NULLABLE` type.
    template <class t_NULLABLE>
    static bdlat_TypeCategory::Value valueCategory();

    /// Return the `bdlat_TypeName::xsdName` value for `object`.
    template <class t_NULLABLE>
    static const char *xsdName(const void *object, int format);

  public:
    // TYPES
    typedef NullableValueVtable      VtableType;
    typedef NullableValueConstVtable ConstVtableType;

    // CLASS METHODS

    /// Construct and return a `NullableValueConstVtable` object with a series
    /// of function pointers that implement the "nullable value" type category.
    template <class t_NULLABLE>
    static const NullableValueConstVtable *getConstVtable();

    /// Construct and return a `NullableValueVtable` object with a series of
    /// function pointers that implement the "nullable value" type category.
    template <class t_NULLABLE>
    static const NullableValueVtable *getVtable();
};

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

                           // -----------------------------
                           // class NullableValueVtableUtil
                           // -----------------------------

// PRIVATE CLASS METHODS
template <class t_NULLABLE>
int NullableValueVtableUtil::accessValue(const void   *object,
                                         AccessorRef&  accessor)
{
    return bdlat_NullableValueFunctions::accessValue(
                                      *static_cast<const t_NULLABLE *>(object),
                                      accessor);
}

template <class t_NULLABLE>
bool NullableValueVtableUtil::isNull(const void *object)
{
    return bdlat_NullableValueFunctions::isNull(
                                     *static_cast<const t_NULLABLE *>(object));
}

template <class t_NULLABLE>
void NullableValueVtableUtil::makeValue(void *object)
{
    bdlat_NullableValueFunctions::makeValue(static_cast<t_NULLABLE *>(object));
}

template <class t_NULLABLE>
int NullableValueVtableUtil::manipulateValue(void            *object,
                                             ManipulatorRef&  manipulator)
{
    return bdlat_NullableValueFunctions::manipulateValue(
                                             static_cast<t_NULLABLE *>(object),
                                             manipulator);
}

template <class t_NULLABLE>
void NullableValueVtableUtil::reset(void *object)
{
    bdlat_ValueTypeFunctions::reset(static_cast<t_NULLABLE *>(object));
}

template <class t_NULLABLE>
bdlat_TypeCategory::Value NullableValueVtableUtil::valueCategory()
{
    typedef typename bdlat_NullableValueFunctions::
                                         ValueType<t_NULLABLE>::Type ValueType;
    return SelectCategory<ValueType>::e_SELECTION;
}

template <class t_NULLABLE>
const char *NullableValueVtableUtil::xsdName(const void *object, int format)
{
    return bdlat_TypeName::xsdName(*static_cast<const t_NULLABLE *>(object),
                                   format);
}

// CLASS METHODS
template <class t_NULLABLE>
const NullableValueConstVtable *NullableValueVtableUtil::getConstVtable()
{
    static const NullableValueConstVtable vtable = {
        &valueCategory<t_NULLABLE>,
        &accessValue<t_NULLABLE>,
        &isNull<t_NULLABLE>,
        &xsdName<t_NULLABLE>
    };
    return &vtable;
}

template <class t_NULLABLE>
const NullableValueVtable *NullableValueVtableUtil::getVtable()
{
    static const NullableValueVtable vtable = {
        {
            &valueCategory<t_NULLABLE>,
            &accessValue<t_NULLABLE>,
            &isNull<t_NULLABLE>,
            &xsdName<t_NULLABLE>
        },
        &manipulateValue<t_NULLABLE>,
        &makeValue<t_NULLABLE>,
        &reset<t_NULLABLE>
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
