// bdlat_fuzzutil.h                                                   -*-C++-*-
#ifndef INCLUDED_BDLAT_FUZZUTIL
#define INCLUDED_BDLAT_FUZZUTIL

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide fuzz test utilities for `bdlat`-types.
//
//@CLASSES:
// bdlat::FuzzUtil: functions to create messages from fuzz data
//
//@SEE_ALSO: bslim_fuzzdataview, bslim_fuzzutil
//
//@DESCRIPTION: This component provides a namespace, `bdlat::FuzzUtil`,
// containing functions that create `bdlat` message types from fuzz data
// provided by a fuzz harness (e.g., `libFuzzer`).
//
// Effectively, given a random sequence of bytes provided by a fuzz harness,
// like `libFuzzer`, this component can be used to populate a
// `bdlat`-compatible message with arbitrary data determined by that random
// sequence.  This can be used to fuzz test an interface (like a service
// interface) taking a `bdlat`-compatible message.  See the
// [BDE fuzz-testing guide](https://bloomberg.github.io/bde/articles/fuzz_testing.html)
// for more detail.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Creating a Message
// - - - - - - - - - - - - - - -
// Suppose we have a message type `MyMessage` and we wish to fuzz test it.
//
// We have to provide a function like this for libFuzzer:
// ```
// extern "C"
// int LLVMFuzzerTestOneInput(const uint8_t *bytes, size_t size)
// {
//     bslim::FuzzDataView fuzzData(bytes, size);
//
//     MyMessage msg;
//     bdlat::FuzzUtil::consumeMessage(&msg, &fuzzData);
//     // Use `msg`...
//
//     return 0;
// }
// ```
// After the `FuzzUtil::consumeMessage` call, the `msg` object contains a
// well-formed message, which can then be serialized or processed in another
// way.

#include <bslscm_version.h>

#include <bdlat_arrayfunctions.h>
#include <bdlat_choicefunctions.h>
#include <bdlat_customizedtypefunctions.h>
#include <bdlat_enumfunctions.h>
#include <bdlat_fuzzutiloptions.h>
#include <bdlat_nullablevaluefunctions.h>
#include <bdlat_sequencefunctions.h>
#include <bdlat_typecategory.h>
#include <bdlat_valuetypefunctions.h>

#include <bslim_fuzzdataview.h>
#include <bslim_fuzzutil.h>

#include <bslmf_assert.h>

#include <bsls_platform.h>

#include <bsl_limits.h>
#include <bsl_type_traits.h>

namespace BloombergLP {
namespace bdlat {

                            // ==========================
                            // class FuzzUtil_Manipulator
                            // ==========================

/// A generic functor (manipulator) with function call operators that match the
/// signature expected by the `bdlat_*Functions::manipulate*` operations.
struct FuzzUtil_Manipulator {
    // PUBLIC DATA
    bslim::FuzzDataView   *fuzzData;
    const FuzzUtilOptions *options;

    // MANIPULATORS

    /// Call `FuzzUtil::consume(object, fuzzData)`.  Return 0.
    template <class t_TYPE>
    int operator()(t_TYPE *object);

    /// Call `FuzzUtil::consume(object, fuzzData)`.  Return 0.
    template <class t_TYPE, class t_INFO>
    int operator()(t_TYPE *object, const t_INFO&);
};

                            // ======================================
                            // class FuzzUtil_ManipulatorWithCategory
                            // ======================================

/// A generic functor (manipulator) with a function call operator that matches
/// the signature expected by the
/// `bdlat_TypeCategoryUtil::manipulateByCategory` operation.
struct FuzzUtil_ManipulatorWithCategory {
    // PUBLIC DATA
    bslim::FuzzDataView   *fuzzData;
    const FuzzUtilOptions *options;

    // MANIPULATORS

    /// Call `FuzzUtil::consume(object, fuzzData)`.  Return 0.
    template <class t_TYPE, class t_CATEGORY_TAG>
    int operator()(t_TYPE *object, t_CATEGORY_TAG);
};

                            // ==============
                            // class FuzzUtil
                            // ==============

/// This utility `class` provides a namespace for a suite of functions
/// operating on objects of type `bslim::FuzzDataView` and providing the
/// consumption of fuzz data bytes into `bdlat`-compatible types.
class FuzzUtil {
    // PRIVATE TYPES
    template <class t_TYPE, bdlat_TypeCategory::Value t_VALUE>
    struct EnableIf : bsl::enable_if<
        static_cast<bdlat_TypeCategory::Value>(
           bdlat_TypeCategory::Select<t_TYPE>::e_SELECTION) == t_VALUE> {
    };

    // PRIVATE CLASS METHODS

    /// Initialize the specified `object` with a value generated using the
    /// specified `fuzzData` and the specified `options`.
    template <class t_ARRAY>
    static typename EnableIf<t_ARRAY,
                             bdlat_TypeCategory::e_ARRAY_CATEGORY
    >::type consume(t_ARRAY               *object,
                    bslim::FuzzDataView   *fuzzData,
                    const FuzzUtilOptions *options);
    template <class t_CHOICE>
    static typename EnableIf<t_CHOICE,
                             bdlat_TypeCategory::e_CHOICE_CATEGORY
    >::type consume(t_CHOICE              *object,
                    bslim::FuzzDataView   *fuzzData,
                    const FuzzUtilOptions *options);
    template <class t_CUSTOMIZED>
    static typename EnableIf<t_CUSTOMIZED,
                             bdlat_TypeCategory::e_CUSTOMIZED_TYPE_CATEGORY
    >::type consume(t_CUSTOMIZED          *object,
                    bslim::FuzzDataView   *fuzzData,
                    const FuzzUtilOptions *options);
    template <class t_DYNAMIC>
    static typename EnableIf<t_DYNAMIC,
                             bdlat_TypeCategory::e_DYNAMIC_CATEGORY
    >::type consume(t_DYNAMIC             *object,
                    bslim::FuzzDataView   *fuzzData,
                    const FuzzUtilOptions *options);
    template <class t_ENUM>
    static typename EnableIf<t_ENUM,
                             bdlat_TypeCategory::e_ENUMERATION_CATEGORY
    >::type consume(t_ENUM                *object,
                    bslim::FuzzDataView   *fuzzData,
                    const FuzzUtilOptions *options);
    template <class t_NULLABLE>
    static typename EnableIf<t_NULLABLE,
                             bdlat_TypeCategory::e_NULLABLE_VALUE_CATEGORY
    >::type consume(t_NULLABLE            *object,
                    bslim::FuzzDataView   *fuzzData,
                    const FuzzUtilOptions *options);
    template <class t_SEQUENCE>
    static typename EnableIf<t_SEQUENCE,
                             bdlat_TypeCategory::e_SEQUENCE_CATEGORY
    >::type consume(t_SEQUENCE            *object,
                    bslim::FuzzDataView   *fuzzData,
                    const FuzzUtilOptions *options);
    template <class t_SIMPLE>
    static typename EnableIf<t_SIMPLE,
                             bdlat_TypeCategory::e_SIMPLE_CATEGORY
    >::type consume(t_SIMPLE              *object,
                    bslim::FuzzDataView   *fuzzData,
                    const FuzzUtilOptions *options);

    // FRIENDS
    friend struct FuzzUtil_Manipulator;
    friend struct FuzzUtil_ManipulatorWithCategory;
  public:
    // CLASS METHODS

    /// Load into the specified `message` content generated using bytes from
    /// the specified `fuzzData`, and update `fuzzData` to reflect the bytes
    /// consumed.  Optionally specify an `options` to customize the generated
    /// values.  If `options` is not supplied, default values for the options
    /// are used.  In general, this operation uses `fuzzData` to load the root
    /// message element, and then recursively applies `consumeMessage` to any
    /// child elements.  More specifically:
    ///
    /// * sequence types: recursively `consumeMessage` on each of the elements
    ///   of the sequence
    /// * array types: use `fuzzData` to determine the size of the array, use
    ///   `consumeMessage` on each of the elements in the array
    /// * choice types: use `fuzzData` to select the active element of the
    ///   choice, and then recursively `consumeMessage` for that element
    /// * nullable values: use `fuzzData` to select whether the object contains
    ///   a value, and if it is engaged, recursively `consumeMessage` on it.
    /// * customized and dynamic types: recursively `consumeMessage` on the
    ///   underlying value.
    ///
    /// `t_MESSAGE` must be a `bdlat`-compatible type.
    template <class t_MESSAGE>
    static void consumeMessage(t_MESSAGE           *message,
                               bslim::FuzzDataView *fuzzData);
    template <class t_MESSAGE>
    static void consumeMessage(t_MESSAGE              *message,
                               bslim::FuzzDataView    *fuzzData,
                               const FuzzUtilOptions&  options);
};

// ============================================================================
//                              INLINE DEFINITIONS
// ============================================================================

                            // --------------------------
                            // class FuzzUtil_Manipulator
                            // --------------------------

// MANIPULATORS
template <class t_TYPE>
inline
int FuzzUtil_Manipulator::operator()(t_TYPE *object)
{
    FuzzUtil::consume(object, fuzzData, options);
    return 0;
}

template <class t_TYPE, class t_INFO>
inline
int FuzzUtil_Manipulator::operator()(t_TYPE *object, const t_INFO&)
{
    return (*this)(object);
}

                            // --------------------------------------
                            // class FuzzUtil_ManipulatorWithCategory
                            // --------------------------------------

// MANIPULATORS
template <class t_TYPE, class t_CATEGORY_TAG>
inline
int FuzzUtil_ManipulatorWithCategory::operator()(t_TYPE         *object,
                                                 t_CATEGORY_TAG  )
{
    FuzzUtil::consume(object, fuzzData, options);
    return 0;
}

                            // --------------
                            // class FuzzUtil
                            // --------------

// PRIVATE CLASS METHODS
template <class t_ARRAY>
inline
typename FuzzUtil::EnableIf<t_ARRAY,
                            bdlat_TypeCategory::e_ARRAY_CATEGORY
>::type FuzzUtil::consume(t_ARRAY               *object,
                          bslim::FuzzDataView   *fuzzData,
                          const FuzzUtilOptions *options)
{
    unsigned maxSize = options ? options->maxArrayLength()
                               : FuzzUtilOptions::k_MAX_ARRAY_LENGTH_DEFAULT;
    const unsigned maxInt = static_cast<unsigned>(
                                              bsl::numeric_limits<int>::max());
    if (maxSize > maxInt) {
        maxSize = maxInt;
    }
    unsigned newSize = bslim::FuzzUtil::consumeNumberInRange<unsigned>(
                                                                      fuzzData,
                                                                      0U,
                                                                      maxSize);
    bsl::size_t size = bdlat_ArrayFunctions::size(*object);
    if (newSize != size) {
        bdlat_ArrayFunctions::resize(object, static_cast<int>(newSize));
        FuzzUtil_Manipulator manipulator = {fuzzData, options};
        // init added elements
        for(; size < newSize; size++) {
            bdlat_ArrayFunctions::manipulateElement(object,
                                                    manipulator,
                                                    static_cast<int>(size));
        }
    }
}

template <class t_CHOICE>
inline
typename FuzzUtil::EnableIf<t_CHOICE,
                            bdlat_TypeCategory::e_CHOICE_CATEGORY
>::type FuzzUtil::consume(t_CHOICE              *object,
                          bslim::FuzzDataView   *fuzzData,
                          const FuzzUtilOptions *options)
{
    BSLMF_ASSERT(bdlat_IsBasicChoice<t_CHOICE>::value);
    int selectionIndex = bslim::FuzzUtil::consumeNumberInRange<int>(
                                                 fuzzData,
                                                 0,
                                                 t_CHOICE::NUM_SELECTIONS - 1);
    int selectionId = t_CHOICE::SELECTION_INFO_ARRAY[selectionIndex].id();
    if (object->makeSelection(selectionId) == 0) {
        FuzzUtil_Manipulator manipulator = {fuzzData, options};
        object->manipulateSelection(manipulator);
    }
}

template <class t_CUSTOMIZED>
inline
typename FuzzUtil::EnableIf<t_CUSTOMIZED,
                            bdlat_TypeCategory::e_CUSTOMIZED_TYPE_CATEGORY
>::type FuzzUtil::consume(t_CUSTOMIZED          *object,
                          bslim::FuzzDataView   *fuzzData,
                          const FuzzUtilOptions *options)
{
#ifdef BSLS_PLATFORM_HAS_PRAGMA_GCC_DIAGNOSTIC
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wuninitialized"
#ifndef BSLS_PLATFORM_CMP_CLANG
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
#endif
#endif
    typedef typename bdlat_CustomizedTypeFunctions::
                                         BaseType<t_CUSTOMIZED>::Type BaseType;
    BaseType base;
    consume(&base, fuzzData, options);
    bdlat_CustomizedTypeFunctions::convertFromBaseType(object, base);
#ifdef BSLS_PLATFORM_HAS_PRAGMA_GCC_DIAGNOSTIC
#pragma GCC diagnostic pop
#endif
}

template <class t_DYNAMIC>
inline
typename FuzzUtil::EnableIf<t_DYNAMIC,
                            bdlat_TypeCategory::e_DYNAMIC_CATEGORY
>::type FuzzUtil::consume(t_DYNAMIC             *object,
                          bslim::FuzzDataView   *fuzzData,
                          const FuzzUtilOptions *options)
{
    FuzzUtil_ManipulatorWithCategory manipulator = {fuzzData, options};
    bdlat_TypeCategoryUtil::manipulateByCategory(object, manipulator);
}

template <class t_ENUM>
inline
typename FuzzUtil::EnableIf<t_ENUM,
                            bdlat_TypeCategory::e_ENUMERATION_CATEGORY
>::type FuzzUtil::consume(t_ENUM                *object,
                          bslim::FuzzDataView   *fuzzData,
                          const FuzzUtilOptions *)
{
    BSLMF_ASSERT(bdlat_IsBasicEnumeration<t_ENUM>::value);
    typedef typename bdlat_BasicEnumerationWrapper<t_ENUM>::Wrapper Wrapper;
    int index = bslim::FuzzUtil::consumeNumberInRange<int>(
                                                 fuzzData,
                                                 0,
                                                 Wrapper::NUM_ENUMERATORS - 1);
    int intValue = Wrapper::ENUMERATOR_INFO_ARRAY[index].value();
    Wrapper::fromInt(object, intValue);
}

template <class t_NULLABLE>
inline
typename FuzzUtil::EnableIf<t_NULLABLE,
                            bdlat_TypeCategory::e_NULLABLE_VALUE_CATEGORY
>::type FuzzUtil::consume(t_NULLABLE            *object,
                          bslim::FuzzDataView   *fuzzData,
                          const FuzzUtilOptions *options)
{
    bool addValue = bslim::FuzzUtil::consumeBool(fuzzData);
    if (addValue) {
        bdlat_NullableValueFunctions::makeValue(object);
        FuzzUtil_Manipulator manipulator = {fuzzData, options};
        bdlat_NullableValueFunctions::manipulateValue(object, manipulator);
    }
}

template <class t_SEQUENCE>
inline
typename FuzzUtil::EnableIf<t_SEQUENCE,
                            bdlat_TypeCategory::e_SEQUENCE_CATEGORY
>::type FuzzUtil::consume(t_SEQUENCE            *object,
                          bslim::FuzzDataView   *fuzzData,
                          const FuzzUtilOptions *options)
{
    FuzzUtil_Manipulator manipulator = {fuzzData, options};
    bdlat_SequenceFunctions::manipulateAttributes(object, manipulator);
}

template <class t_SIMPLE>
inline
typename FuzzUtil::EnableIf<t_SIMPLE,
                            bdlat_TypeCategory::e_SIMPLE_CATEGORY
>::type FuzzUtil::consume(t_SIMPLE              *object,
                          bslim::FuzzDataView   *,
                          const FuzzUtilOptions *)
{
    bdlat_ValueTypeFunctions::reset(object);
}

// CLASS METHODS
template <class t_MESSAGE>
void FuzzUtil::consumeMessage(t_MESSAGE           *message,
                              bslim::FuzzDataView *fuzzData)
{
    consume(message, fuzzData, 0);
}

template <class t_MESSAGE>
void FuzzUtil::consumeMessage(t_MESSAGE              *message,
                              bslim::FuzzDataView    *fuzzData,
                              const FuzzUtilOptions&  options)
{
    consume(message, fuzzData, &options);
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
