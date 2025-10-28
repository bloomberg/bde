// bdlar_arrayvtableutil.h                                            -*-C++-*-
#ifndef INCLUDED_BDLAR_ARRAYVTABLEUTIL
#define INCLUDED_BDLAR_ARRAYVTABLEUTIL

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide ...
//
//@CLASSES:
//  bdlar::ArrayVtableUtil: ...
//
//@DESCRIPTION: This component provides ...
//

#include <bdlar_arrayvtable.h>
#include <bdlar_typecategory.h>

#include <bdlat_arrayfunctions.h>
#include <bdlat_typename.h>
#include <bdlat_valuetypefunctions.h>

namespace BloombergLP {
namespace bdlar {

                           // =====================
                           // class ArrayVtableUtil
                           // =====================

class ArrayVtableUtil {
    // PRIVATE CLASS METHODS

    /// Invoke the specified `accessor` on the element of the specified `array`
    /// at the specified `index`.  Return the value from the invocation of
    /// `accessor`.  The behavior is undefined unless
    /// `0 <= index < size(array)`.
    template <class t_ARRAY>
    static int accessElement(const void   *array,
                             AccessorRef&  accessor,
                             int           index);

    /// Return the element type category of the specified `t_ARRAY` type.
    template <class t_ARRAY>
    static bdlat_TypeCategory::Value elementCategory();

    /// Invoke the specified `manipulator` on the element of the specified
    /// `array` at the specified `index`.  Return the value from the invocation
    /// of `manipulator`.  The behavior is undefined unless
    /// `0 <= index < size(array)`.
    template <class t_ARRAY>
    static int manipulateElement(void            *array,
                                 ManipulatorRef&  manipulator,
                                 int              index);

    /// Reset the object pointed by the specified `object` to the default
    /// value.
    template <class t_ARRAY>
    static void reset(void *object);

    /// Set the size of the specified modifiable `array` to the specified
    /// `newSize`.  If `newSize > size(array)`, then `newSize - size(array)`
    /// elements with default values are appended to `array`.  If
    /// `newSize < size(array)`, then the `size(array) - newSize` elements at
    /// the end of `array` are destroyed.  The behavior is undefined unless
    /// `0 <= newSize`.
    template <class t_ARRAY>
    static void resize(void *array, int newSize);

    /// Return the number of elements in the specified `array`.
    template <class t_ARRAY>
    static bsl::size_t size(const void *array);

    /// Return the `bdlat_TypeName::xsdName` value for `object`.
    template <class t_ARRAY>
    static const char *xsdName(const void *object, int format);

  public:
    // TYPES
    typedef ArrayVtable      VtableType;
    typedef ArrayConstVtable ConstVtableType;

    // CLASS METHODS

    /// Construct and return a `ArrayConstVtable` object with a series of
    /// function pointers that implement the "array" type category.
    template <class t_ARRAY>
    static const ArrayConstVtable *getConstVtable();

    /// Construct and return a `ArrayVtable` object with a series of function
    /// pointers that implement the "array" type category.
    template <class t_ARRAY>
    static const ArrayVtable *getVtable();
};

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

                           // ---------------------
                           // class ArrayVtableUtil
                           // ---------------------

// PRIVATE CLASS METHODS
template <class t_ARRAY>
int ArrayVtableUtil::accessElement(const void   *array,
                                   AccessorRef&  accessor,
                                   int           index)
{
    return bdlat_ArrayFunctions::accessElement(
                                          *static_cast<const t_ARRAY *>(array),
                                          accessor,
                                          index);
}

template <class t_ARRAY>
bdlat_TypeCategory::Value ArrayVtableUtil::elementCategory()
{
    typedef typename bdlat_ArrayFunctions::ElementType<t_ARRAY>::Type
                                                                   ElementType;
    return SelectCategory<ElementType>::e_SELECTION;
}

template <class t_ARRAY>
int ArrayVtableUtil::manipulateElement(void            *array,
                                       ManipulatorRef&  manipulator,
                                       int              index)
{
    return bdlat_ArrayFunctions::manipulateElement(
                                                 static_cast<t_ARRAY *>(array),
                                                 manipulator,
                                                 index);
}

template <class t_ARRAY>
void ArrayVtableUtil::reset(void *object)
{
    bdlat_ValueTypeFunctions::reset(static_cast<t_ARRAY *>(object));
}

template <class t_ARRAY>
void ArrayVtableUtil::resize(void *array, int newSize)
{
     bdlat_ArrayFunctions::resize(static_cast<t_ARRAY *>(array), newSize);
}

template <class t_ARRAY>
bsl::size_t ArrayVtableUtil::size(const void *array)
{
    return bdlat_ArrayFunctions::size(*static_cast<const t_ARRAY *>(array));
}

template <class t_ARRAY>
const char *ArrayVtableUtil::xsdName(const void *object, int format)
{
    return bdlat_TypeName::xsdName(*static_cast<const t_ARRAY *>(object),
                                   format);
}

// CLASS METHODS
template <class t_ARRAY>
const ArrayConstVtable *ArrayVtableUtil::getConstVtable()
{
    static const ArrayConstVtable vtable = {
        &elementCategory<t_ARRAY>,
        &accessElement<t_ARRAY>,
        &size<t_ARRAY>,
        &xsdName<t_ARRAY>
    };
    return &vtable;
}

template <class t_ARRAY>
const ArrayVtable *ArrayVtableUtil::getVtable()
{
    static const ArrayVtable vtable = {
        {
            &elementCategory<t_ARRAY>,
            &accessElement<t_ARRAY>,
            &size<t_ARRAY>,
            &xsdName<t_ARRAY>
        },
        &manipulateElement<t_ARRAY>,
        &reset<t_ARRAY>,
        &resize<t_ARRAY>
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
