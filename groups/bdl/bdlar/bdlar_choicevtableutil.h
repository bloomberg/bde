// bdlar_choicevtableutil.h                                           -*-C++-*-
#ifndef INCLUDED_BDLAR_CHOICEVTABLEUTIL
#define INCLUDED_BDLAR_CHOICEVTABLEUTIL

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide ...
//
//@CLASSES:
//  bdlar::ChoiceVtableUtil: ...
//
//@DESCRIPTION: This component provides ...
//

#include <bdlar_choicevtable.h>

#include <bdlat_choicefunctions.h>
#include <bdlat_typename.h>
#include <bdlat_valuetypefunctions.h>

namespace BloombergLP {
namespace bdlar {

                           // ======================
                           // class ChoiceVtableUtil
                           // ======================

class ChoiceVtableUtil {
    // PRIVATE CLASS METHODS

    /// Invoke the specified `accessor` on the (non-modifiable) selection,
    /// supplying `accessor` with the corresponding selection information
    /// structure.  Return the value returned from the invocation of
    /// `accessor`.  The behavior is undefined unless
    /// `selectionId(object) != -1`.
    template <class t_CHOICE>
    static int accessSelection(const void           *object,
                               AccessorWithInfoRef&  accessor);

    /// Return true if the specified `object` has a selection with the
    /// specified `selectionId`, and `false` otherwise.
    template <class t_CHOICE>
    static bool hasSelectionId(const void *object, int selectionId);

    /// Return true if the specified `object` has a selection with the
    /// specified `selectionName` of the specified `selectionNameLength`, and
    /// `false` otherwise.
    template <class t_CHOICE>
    static bool hasSelectionName(const void *object,
                                 const char *selectionName,
                                 int         selectionNameLength);

    /// Set the value of the specified `object` to be the default for the
    /// selection indicated by the specified `selectionId`.  Return 0 on
    /// success, and non-zero value otherwise (i.e., the selection is not
    /// found).
    template <class t_CHOICE>
    static int makeSelectionById(void *object, int selectionId);

    /// Set the value of the specified `object` to be the default for the
    /// selection indicated by the specified `selectionName` of the specified
    /// `selectionNameLength`.  Return 0 on success, and non-zero value
    /// otherwise (i.e., the selection is not found).
    template <class t_CHOICE>
    static int makeSelectionByName(void       *object,
                                   const char *selectionName,
                                   int         selectionNameLength);

    /// Invoke the specified `manipulator` on the address of the (modifiable)
    /// selection, supplying `manipulator` with the corresponding selection
    /// information structure.  Return the value returned from the invocation
    /// of `manipulator`.  The behavior is undefined unless
    /// `selectionId(object) != -1`.
    template <class t_CHOICE>
    static int manipulateSelection(void                    *object,
                                   ManipulatorWithInfoRef&  manipulator);

    /// Reset the object pointed by the specified `object` to the default
    /// value.
    template <class t_CHOICE>
    static void reset(void *object);

    /// Return the id of the current selection if the selection is defined, and
    /// -1 otherwise.
    template <class t_CHOICE>
    static int selectionId(const void *object);

    /// Return the `bdlat_TypeName::xsdName` value for `object`.
    template <class t_CHOICE>
    static const char *xsdName(const void *object, int format);

  public:
    // TYPES
    typedef ChoiceVtable      VtableType;
    typedef ChoiceConstVtable ConstVtableType;

    // CLASS METHODS

    /// Construct and return a `ChoiceConstVtable` object with a series of
    /// function pointers that implement the "choice" type category.
    template <class t_CHOICE>
    static const ChoiceConstVtable *getConstVtable();

    /// Construct and return a `ChoiceVtable` object with a series of function
    /// pointers that implement the "choice" type category.
    template <class t_CHOICE>
    static const ChoiceVtable *getVtable();
};

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

                           // ----------------------
                           // class ChoiceVtableUtil
                           // ----------------------

// PRIVATE CLASS METHODS
template <class t_CHOICE>
int ChoiceVtableUtil::accessSelection(const void           *object,
                                      AccessorWithInfoRef&  accessor)
{
    return bdlat_ChoiceFunctions::accessSelection(
                                        *static_cast<const t_CHOICE *>(object),
                                        accessor);
}

template <class t_CHOICE>
bool ChoiceVtableUtil::hasSelectionId(const void *object, int selectionId)
{
    return bdlat_ChoiceFunctions::hasSelection(
                                        *static_cast<const t_CHOICE *>(object),
                                        selectionId);
}

template <class t_CHOICE>
bool ChoiceVtableUtil::hasSelectionName(const void *object,
                                        const char *selectionName,
                                        int         selectionNameLength)
{
    return bdlat_ChoiceFunctions::hasSelection(
                                        *static_cast<const t_CHOICE *>(object),
                                        selectionName,
                                        selectionNameLength);
}

template <class t_CHOICE>
int ChoiceVtableUtil::makeSelectionById(void *object, int selectionId)
{
    return bdlat_ChoiceFunctions::makeSelection(
                                               static_cast<t_CHOICE *>(object),
                                               selectionId);
}

template <class t_CHOICE>
int ChoiceVtableUtil::makeSelectionByName(void       *object,
                                          const char *selectionName,
                                          int         selectionNameLength)
{
    return bdlat_ChoiceFunctions::makeSelection(
                                               static_cast<t_CHOICE *>(object),
                                               selectionName,
                                               selectionNameLength);
}

template <class t_CHOICE>
int ChoiceVtableUtil::manipulateSelection(void                    *object,
                                          ManipulatorWithInfoRef&  manipulator)
{
    return bdlat_ChoiceFunctions::manipulateSelection(
                                               static_cast<t_CHOICE *>(object),
                                               manipulator);
}

template <class t_CHOICE>
void ChoiceVtableUtil::reset(void *object)
{
    bdlat_ValueTypeFunctions::reset(static_cast<t_CHOICE *>(object));
}

template <class t_CHOICE>
int ChoiceVtableUtil::selectionId(const void *object)
{
    return bdlat_ChoiceFunctions::selectionId(
                                       *static_cast<const t_CHOICE *>(object));
}

template <class t_CHOICE>
const char *ChoiceVtableUtil::xsdName(const void *object, int format)
{
    return bdlat_TypeName::xsdName(*static_cast<const t_CHOICE *>(object),
                                   format);
}

// CLASS METHODS
template <class t_CHOICE>
const ChoiceConstVtable *ChoiceVtableUtil::getConstVtable()
{
    static const ChoiceConstVtable vtable = {
        &selectionId<t_CHOICE>,
        &hasSelectionId<t_CHOICE>,
        &hasSelectionName<t_CHOICE>,
        &accessSelection<t_CHOICE>,
        &xsdName<t_CHOICE>
    };
    return &vtable;
}

template <class t_CHOICE>
const ChoiceVtable *ChoiceVtableUtil::getVtable()
{
    static const ChoiceVtable vtable = {
        {
            &selectionId<t_CHOICE>,
            &hasSelectionId<t_CHOICE>,
            &hasSelectionName<t_CHOICE>,
            &accessSelection<t_CHOICE>,
            &xsdName<t_CHOICE>
        },
        &makeSelectionById<t_CHOICE>,
        &makeSelectionByName<t_CHOICE>,
        &manipulateSelection<t_CHOICE>,
        &reset<t_CHOICE>
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
