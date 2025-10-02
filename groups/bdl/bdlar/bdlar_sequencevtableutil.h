// bdlar_sequencevtableutil.h                                         -*-C++-*-
#ifndef INCLUDED_BDLAR_SEQUENCEVTABLEUTIL
#define INCLUDED_BDLAR_SEQUENCEVTABLEUTIL

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide ...
//
//@CLASSES:
//  bdlar::SequenceVtableUtil: ...
//
//@DESCRIPTION: This component provides ...
//

#include <bdlar_sequencevtable.h>

#include <bdlat_sequencefunctions.h>
#include <bdlat_typename.h>
#include <bdlat_valuetypefunctions.h>

namespace BloombergLP {
namespace bdlar {

                           // ========================
                           // class SequenceVtableUtil
                           // ========================

class SequenceVtableUtil {
    // PRIVATE CLASS METHODS

    /// Invoke the specified `accessor` on the (non-modifiable) attribute of
    /// the referred sequence object with the given `attributeId`, supplying
    /// `accessor` with the corresponding attribute information structure.
    /// Return non-zero if the attribute is not found, and the value returned
    /// from the invocation of `accessor` otherwise.
    template <class t_SEQUENCE>
    static int accessAttribute(const void           *object,
                               AccessorWithInfoRef&  accessor,
                               int                   attributeId);

    /// Invoke the specified `accessor` on the (non-modifiable) attribute of
    /// the referred sequence object indicated by the specified `attributeName`
    /// and `attributeNameLength`, supplying `accessor` with the corresponding
    /// attribute information structure.  Return non-zero value if the
    /// attribute is not found, and the value returned from the invocation of
    /// `accessor` otherwise.
    template <class t_SEQUENCE>
    static int accessAttribute(const void           *object,
                               AccessorWithInfoRef&  accessor,
                               const char           *attributeName,
                               int                   attributeNameLength);

    /// Invoke the specified `accessor` sequentially on each attribute,
    /// supplying `accessor` with the corresponding attribute information
    /// structure until such invocation returns a non-zero value.  Return the
    /// value from the last invocation of `accessor` (i.e., the invocation that
    /// terminated the sequence).
    template <class t_SEQUENCE>
    static int accessAttributes(const void           *object,
                                AccessorWithInfoRef&  accessor);

    /// Return `true` if the specified `object` has an attribute with the
    /// specified `attributeId`, and `false` otherwise.
    template <class t_SEQUENCE>
    static bool hasAttributeId(const void *object, int attributeId);

    /// Return `true` if the specified `object` has an attribute with the
    /// specified `attributeName` of the specified `attributeNameLength`,
    /// and `false` otherwise.
    template <class t_SEQUENCE>
    static bool hasAttributeName(const void *object,
                                 const char *attributeName,
                                 int         attributeNameLength);

    /// Invoke the specified `manipulator` on the (modifiable) attribute
    /// indicated by the specified `attributeId`, supplying `manipulator` with
    /// the corresponding attribute information structure.  Return non-zero
    /// value if the attribute is not found, and the value returned from the
    /// invocation of `manipulator` otherwise.
    template <class t_SEQUENCE>
    static int manipulateAttribute(void                    *object,
                                   ManipulatorWithInfoRef&  manipulator,
                                   int                      attributeId);

    /// Invoke the specified `manipulator` on the (modifiable) attribute
    /// indicated by the specified `attributeName` and `attributeNameLength`,
    /// supplying `manipulator` with the corresponding attribute information
    /// structure.  Return non-zero value if the attribute is not found, and
    /// the value returned from the invocation of `manipulator` otherwise.
    template <class t_SEQUENCE>
    static int manipulateAttribute(
                                 void                    *object,
                                 ManipulatorWithInfoRef&  manipulator,
                                 const char              *attributeName,
                                 int                      attributeNameLength);

    /// Invoke the specified `manipulator` sequentially on each (modifiable)
    /// attribute, supplying `manipulator` with the corresponding attribute
    /// information structure until such invocation returns non-zero value.
    /// Return the value from the last invocation of `manipulator` (i.e., the
    /// invocation that terminated the sequence).
    template <class t_SEQUENCE>
    static int manipulateAttributes(void                    *object,
                                    ManipulatorWithInfoRef&  manipulator);

    /// Reset the object pointed by the specified `object` to the default
    /// value.
    template <class t_SEQUENCE>
    static void reset(void *object);

    /// Return the `bdlat_TypeName::xsdName` value for `object`.
    template <class t_SEQUENCE>
    static const char *xsdName(const void *object, int format);

  public:
    // TYPES
    typedef SequenceVtable      VtableType;
    typedef SequenceConstVtable ConstVtableType;

    // CLASS METHODS

    /// Construct and return a `SequenceConstVtable` object with a series of
    /// function pointers that implement the "sequence" type category.
    template <class t_SEQUENCE>
    static const SequenceConstVtable *getConstVtable();

    /// Construct and return a `SequenceVtable` object with a series of
    /// function pointers that implement the "sequence" type category.
    template <class t_SEQUENCE>
    static const SequenceVtable *getVtable();
};

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

                           // ------------------------
                           // class SequenceVtableUtil
                           // ------------------------

// PRIVATE CLASS METHODS
template <class t_SEQUENCE>
int SequenceVtableUtil::accessAttribute(const void           *object,
                                        AccessorWithInfoRef&  accessor,
                                        int                   attributeId)
{
    return bdlat_SequenceFunctions::accessAttribute(
                                      *static_cast<const t_SEQUENCE *>(object),
                                      accessor,
                                      attributeId);
}

template <class t_SEQUENCE>
int SequenceVtableUtil::accessAttribute(
                                     const void           *object,
                                     AccessorWithInfoRef&  accessor,
                                     const char           *attributeName,
                                     int                   attributeNameLength)
{
    return bdlat_SequenceFunctions::accessAttribute(
                                      *static_cast<const t_SEQUENCE *>(object),
                                      accessor,
                                      attributeName,
                                      attributeNameLength);
}

template <class t_SEQUENCE>
int SequenceVtableUtil::accessAttributes(const void           *object,
                                         AccessorWithInfoRef&  accessor)
{
    return bdlat_SequenceFunctions::accessAttributes(
                                      *static_cast<const t_SEQUENCE *>(object),
                                      accessor);
}

template <class t_SEQUENCE>
bool SequenceVtableUtil::hasAttributeId(const void *object, int attributeId)
{
    return bdlat_SequenceFunctions::hasAttribute(
                                      *static_cast<const t_SEQUENCE *>(object),
                                      attributeId);
}

template <class t_SEQUENCE>
bool SequenceVtableUtil::hasAttributeName(const void *object,
                                          const char *attributeName,
                                          int         attributeNameLength)
{
    return bdlat_SequenceFunctions::hasAttribute(
                                      *static_cast<const t_SEQUENCE *>(object),
                                      attributeName,
                                      attributeNameLength);
}

template <class t_SEQUENCE>
int SequenceVtableUtil::manipulateAttribute(
                                          void                    *object,
                                          ManipulatorWithInfoRef&  manipulator,
                                          int                      attributeId)
{
    return bdlat_SequenceFunctions::manipulateAttribute(
                                             static_cast<t_SEQUENCE *>(object),
                                             manipulator,
                                             attributeId);
}

template <class t_SEQUENCE>
int SequenceVtableUtil::manipulateAttribute(
                                  void                    *object,
                                  ManipulatorWithInfoRef&  manipulator,
                                  const char              *attributeName,
                                  int                      attributeNameLength)
{
    return bdlat_SequenceFunctions::manipulateAttribute(
                                             static_cast<t_SEQUENCE *>(object),
                                             manipulator,
                                             attributeName,
                                             attributeNameLength);
}

template <class t_SEQUENCE>
int SequenceVtableUtil::manipulateAttributes(
                                          void                    *object,
                                          ManipulatorWithInfoRef&  manipulator)
{
    return bdlat_SequenceFunctions::manipulateAttributes(
                                             static_cast<t_SEQUENCE *>(object),
                                             manipulator);
}

template <class t_SEQUENCE>
void SequenceVtableUtil::reset(void *object)
{
    bdlat_ValueTypeFunctions::reset(static_cast<t_SEQUENCE *>(object));
}

template <class t_SEQUENCE>
const char *SequenceVtableUtil::xsdName(const void *object, int format)
{
    return bdlat_TypeName::xsdName(*static_cast<const t_SEQUENCE *>(object),
                                   format);
}

// CLASS METHODS
template <class t_SEQUENCE>
const SequenceConstVtable *SequenceVtableUtil::getConstVtable()
{
    static const SequenceConstVtable vtable = {
        &hasAttributeName<t_SEQUENCE>,
        &hasAttributeId<t_SEQUENCE>,
        &accessAttribute<t_SEQUENCE>,
        &accessAttribute<t_SEQUENCE>,
        &accessAttributes<t_SEQUENCE>,
        &xsdName<t_SEQUENCE>
    };
    return &vtable;
}

template <class t_SEQUENCE>
const SequenceVtable *SequenceVtableUtil::getVtable()
{
    static const SequenceVtable vtable = {
        {
            &hasAttributeName<t_SEQUENCE>,
            &hasAttributeId<t_SEQUENCE>,
            &accessAttribute<t_SEQUENCE>,
            &accessAttribute<t_SEQUENCE>,
            &accessAttributes<t_SEQUENCE>,
            &xsdName<t_SEQUENCE>
        },
        &manipulateAttribute<t_SEQUENCE>,
        &manipulateAttribute<t_SEQUENCE>,
        &manipulateAttributes<t_SEQUENCE>,
        &reset<t_SEQUENCE>
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
