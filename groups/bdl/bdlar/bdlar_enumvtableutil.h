// bdlar_enumvtableutil.h                                             -*-C++-*-
#ifndef INCLUDED_BDLAR_ENUMVTABLEUTIL
#define INCLUDED_BDLAR_ENUMVTABLEUTIL

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide ...
//
//@CLASSES:
//  bdlar::EnumVtableUtil: ...
//
//@DESCRIPTION: This component provides ...
//

#include <bdlar_enumvtable.h>

#include <bdlat_enumfunctions.h>
#include <bdlat_typename.h>
#include <bdlat_valuetypefunctions.h>

namespace BloombergLP {
namespace bdlar {

                           // ====================
                           // class EnumVtableUtil
                           // ====================

class EnumVtableUtil {
    // PRIVATE CLASS METHODS

    /// Load into the specified type erased enumerator `result` the enumerator
    /// matching the specified `number`.  Return 0 on success, and a non-zero
    /// value with no effect on `result` if `number` does not match any
    /// enumerator.
    template <class t_ENUM>
    static int fromInt(void *result, int number);

    /// Load into the specified type erased enumerator `result` the enumerator
    /// matching the specified `string` of the specified `stringLength`.
    /// Return 0 on success, and a non-zero value with no effect on `result` if
    /// `string` and `stringLength` do not match any enumerator.
    template <class t_ENUM>
    static int fromString(void *result, const char *string, int stringLength);

    /// Return `true` if the specified type erased enumerator `t_ENUM` supports
    /// a fallback enumerator, and `false` otherwise.
    template <class t_ENUM>
    static bool hasFallback(const void *value);

    /// Return `true` if the specified type erased enumerator `value` is equal
    /// to a fallback enumerator, and `false` otherwise.
    template <class t_ENUM>
    static bool isFallback(const void *value);

    /// Load into the specified type erased enumerator `result` the fallback
    /// enumerator value.  Return 0 on success, and a non-zero value with no
    /// effect on `result` if it does not have a fallback enumerator.
    template <class t_ENUM>
    static int makeFallback(void *result);

    /// Reset the object pointed by the specified `object` to the default
    /// value.
    template <class t_ENUM>
    static void reset(void *object);

    /// Load into the specified `result` the string representation of the
    /// enumerator value held by the specified type erased enumerator `value`.
    template <class t_ENUM>
    static void toInt(int *result, const void *value);

    /// Load into the specified `result` the integer representation of the
    /// enumerator value held by the specified type erased enumerator `value`.
    template <class t_ENUM>
    static void toString(bsl::string *result, const void *value);

    /// Return the `bdlat_TypeName::xsdName` value for `object`.
    template <class t_ENUM>
    static const char *xsdName(const void *object, int format);

  public:
    // TYPES
    typedef EnumVtable      VtableType;
    typedef EnumConstVtable ConstVtableType;

    // CLASS METHODS

    /// Construct and return a `EnumConstVtable` object with a series of
    /// function pointers that implement the "enum" type category.
    template <class t_ENUM>
    static const EnumConstVtable *getConstVtable();

    /// Construct and return a `EnumVtable` object with a series of function
    /// pointers that implement the "enum" type category.
    template <class t_ENUM>
    static const EnumVtable *getVtable();
};

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

                           // --------------------
                           // class EnumVtableUtil
                           // --------------------

// PRIVATE CLASS METHODS
template <class t_ENUM>
int EnumVtableUtil::fromInt(void *result, int number)
{
    return bdlat_EnumFunctions::fromInt(static_cast<t_ENUM *>(result), number);
}

template <class t_ENUM>
int EnumVtableUtil::fromString(void       *result,
                               const char *string,
                               int         stringLength)
{
    return bdlat_EnumFunctions::fromString(static_cast<t_ENUM *>(result),
                                           string,
                                           stringLength);
}

template <class t_ENUM>
bool EnumVtableUtil::hasFallback(const void *value)
{
    return bdlat_EnumFunctions::hasFallback(
                                          *static_cast<const t_ENUM *>(value));
}

template <class t_ENUM>
bool EnumVtableUtil::isFallback(const void *value)
{
    return bdlat_EnumFunctions::isFallback(
                                          *static_cast<const t_ENUM *>(value));
}

template <class t_ENUM>
int EnumVtableUtil::makeFallback(void *result)
{
    return bdlat_EnumFunctions::makeFallback(static_cast<t_ENUM *>(result));
}

template <class t_ENUM>
void EnumVtableUtil::reset(void *object)
{
    bdlat_ValueTypeFunctions::reset(static_cast<t_ENUM *>(object));
}

template <class t_ENUM>
void EnumVtableUtil::toInt(int *result, const void *value)
{
    bdlat_EnumFunctions::toInt(result, *static_cast<const t_ENUM *>(value));
}

template <class t_ENUM>
void EnumVtableUtil::toString(bsl::string *result, const void *value)
{
    bdlat_EnumFunctions::toString(result, *static_cast<const t_ENUM *>(value));
}

template <class t_ENUM>
const char *EnumVtableUtil::xsdName(const void *object, int format)
{
    return bdlat_TypeName::xsdName(*static_cast<const t_ENUM *>(object),
                                   format);
}

// CLASS METHODS
template <class t_ENUM>
const EnumConstVtable *EnumVtableUtil::getConstVtable()
{
    static const EnumConstVtable vtable = {
        &toInt<t_ENUM>,
        &toString<t_ENUM>,
        &hasFallback<t_ENUM>,
        &isFallback<t_ENUM>,
        &xsdName<t_ENUM>
    };
    return &vtable;
}

template <class t_ENUM>
const EnumVtable *EnumVtableUtil::getVtable()
{
    static const EnumVtable vtable = {
        {
            &toInt<t_ENUM>,
            &toString<t_ENUM>,
            &hasFallback<t_ENUM>,
            &isFallback<t_ENUM>,
            &xsdName<t_ENUM>
        },
        &fromInt<t_ENUM>,
        &fromString<t_ENUM>,
        &makeFallback<t_ENUM>,
        &reset<t_ENUM>
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
