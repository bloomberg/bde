// bdld_datummaker.h                                                  -*-C++-*-
#ifndef INCLUDED_BDLD_DATUMMAKER
#define INCLUDED_BDLD_DATUMMAKER

//@PURPOSE: Provide a mechanism for easily creating 'bdld::Datum' objects.
//
//@CLASSES:
//  DatumMaker: a mechanism for easily creating 'bdld::Datum' objects
//
//@SEE_ALSO:
//
//@DESCRIPTION: This component defines a concrete mechanism, 'DatumMaker' that
// allows 'bdld::Datum' objects to be created with minimal syntax.  Note that
// this is frequently useful in testing contexts.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Testing of a function
/// - - - - - - - - - - - - - - - -
// Suppose we want to test a function, 'numCount', that returns the number of
// numeric elements in a 'bdld::Datum' array.
//
// First we implement the function:
//..
//  bdld::Datum numCount(const bdld::Datum arrray)
//  {
//      bdld::DatumArrayRef aRef = arrray.theArray();
//
//      int count = 0;
//
//      for (bdld::DatumArrayRef::SizeType i = 0; i < aRef.length(); ++i) {
//          if (aRef[i].isInteger()   ||
//              aRef[i].isInteger64() ||
//              aRef[i].isDouble()) {
//              ++count;
//          }
//      }
//
//      return bdld::Datum::createInteger(count);
//  }
//..
// Then, within the test driver for 'numCount', we define a 'bdld::DatumMaker',
// and use it to initialize an array to test 'numCount':
//..
//  bdld::DatumMaker m(&sa);
//..
// Here, we create the array we want to use as an argument to 'numCount':
//..
//  bdld::Datum array = m.a(
//      m(),
//      m(bdld::DatumError(-1)),
//      m.a(
//          m(true),
//          m(false)),
//      m(42.0),
//      m(false),
//      m(0),
//      m(true),
//      m(bsls::Types::Int64(424242)),
//      m.m(
//          "firstName", "Bart",
//          "lastName",  "Simpson",
//          "age",       10
//      ),
//      m(bdlt::Date(2016, 10, 14)),
//      m(bdlt::Time(13, 00, 00, 000)),
//      m(bdlt::Datetime(2016, 10, 14, 13, 01, 30, 87)),
//      m(bdlt::DatetimeInterval(280, 13, 41, 12, 321)),
//      m("foobar")
//  );
//..
// Next we call the function with the array-'Datum' as its first argument:
//..
//  bdld::Datum retVal = numCount(array);
//..
// Finally we verify the return value:
//..
//  assert(retVal.theInteger() == 3);
//..

#ifndef INCLUDED_BDLSCM_VERSION
#include <bdlscm_version.h>
#endif

#ifndef INCLUDED_BDLD_DATUM
#include <bdld_datum.h>
#endif

#ifndef INCLUDED_BDLD_DATUMARRAYBUILDER
#include <bdld_datumarraybuilder.h>
#endif

#ifndef INCLUDED_BDLD_DATUMMAPBUILDER
#include <bdld_datummapbuilder.h>
#endif

#ifndef INCLUDED_BDLD_DATUMMAPOWNINGKEYSBUILDER
#include <bdld_datummapowningkeysbuilder.h>
#endif

#ifndef INCLUDED_BDLDFP_DECIMAL
#include <bdldfp_decimal.h>
#endif

#ifndef INCLUDED_BDLB_NULLABLEVALUE
#include <bdlb_nullablevalue.h>
#endif

#ifndef INCLUDED_BSLMF_ASSERT
#include <bslmf_assert.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSL_UTILITY
#include <bsl_utility.h> // for std::forward
#endif

#ifndef INCLUDED_BSLS_COMPILERFEATURES
#include <bsls_compilerfeatures.h>
#endif

#ifndef INCLUDED_BSLS_TYPES
#include <bsls_types.h>
#endif

namespace BloombergLP {
namespace bdld {

                              // ================
                              // class DatumMaker
                              // ================

class DatumMaker {
    // This concrete mechanism class provides "sugar" for easily creating
    // 'bdld::Datum' objects for testing.

    // DATA
    bslma::Allocator *d_allocator_p;  // allocator (held, not owned)

    // NOT IMPLEMENTED
    DatumMaker(const DatumMaker&);
    DatumMaker& operator=(const DatumMaker&);

    template <class T> void operator()(T *) const;
        // This overload precludes an implicit (and unintended) conversion to
        // 'bool'.  This (unimplemented) function template should not be
        // instantiated unless 'operator()' is called with an unsupported type.

    // PRIVATE ACCESSORS
    void pushBackHelper(bdld::DatumArrayBuilder *) const;
        // Do nothing, ends template recursion.

    void pushBackHelper(bdld::DatumMapBuilder *) const;
        // Do nothing, ends template recursion.

    void pushBackHelper(bdld::DatumMapOwningKeysBuilder *) const;
        // Do nothing, ends template recursion.

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES // $var-args=32
    template <typename TYPE>
    void pushBackHelper(bdld::DatumArrayBuilder *builder,
                        TYPE&&                   element) const;
        // 'push_back' the specified 'element' into the specified 'builder'.

    template <typename TYPE, typename... ELEMENTS>
    void pushBackHelper(bdld::DatumArrayBuilder *builder,
                        TYPE&&                   element,
                        ELEMENTS&&...            elements) const;
        // 'push_back' the specified 'element' into the specified 'builder',
        // then call 'pushBackHelper' with the specified (variadic) 'elements'.

    template <typename TYPE>
    void pushBackHelper(bdld::DatumMapBuilder    *builder,
                        const bslstl::StringRef&  key,
                        TYPE&&                    value) const;
        // 'push_back' the specified 'key' and 'value' pair (forming a
        // property) into the specified 'builder'.

    template <typename TYPE, typename... ENTRIES>
    void pushBackHelper(bdld::DatumMapBuilder    *builder,
                        const bslstl::StringRef&  key,
                        TYPE&&                    value,
                        ENTRIES&&...              entries) const;
        // 'push_back' the specified 'key' and 'value' pair (forming a
        // property) into the specified 'builder', then call 'pushBackHelper'
        // with the specified (variadic) entries.

    template <typename TYPE>
    void pushBackHelper(bdld::DatumMapOwningKeysBuilder *builder,
                        const bslstl::StringRef&         key,
                        TYPE&&                           value) const;
        // 'push_back' the specified 'key' and 'value' pair (forming a
        // property) into the specified 'builder'.

    template <typename TYPE, typename... ENTRIES>
    void pushBackHelper(bdld::DatumMapOwningKeysBuilder *builder,
                        const bslstl::StringRef&         key,
                        TYPE&&                           value,
                        ENTRIES&&...                     entries) const;
        // 'push_back' the specified 'key' and 'value' pair (forming a
        // property) into the specified 'builder', then call 'pushBackHelper'
        // with the specified (variadic) entries.

// IMPORTANT NOTE: The section below was manually modified to reduce the
// maximum number of parameters for the array builder to 16.
#elif BSLS_COMPILERFEATURES_SIMULATE_VARIADIC_TEMPLATES
    template <typename TYPE>
    void pushBackHelper(bdld::DatumArrayBuilder *builder,
                        BSLS_COMPILERFEATURES_FORWARD_REF(TYPE) element
                        ) const;

    template <typename TYPE, typename ELEMENTS_01>
    void pushBackHelper(bdld::DatumArrayBuilder *builder,
                        BSLS_COMPILERFEATURES_FORWARD_REF(TYPE) element,
                     BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_01) elements_01
                        ) const;

    template <typename TYPE, typename ELEMENTS_01,
                             typename ELEMENTS_02>
    void pushBackHelper(bdld::DatumArrayBuilder *builder,
                        BSLS_COMPILERFEATURES_FORWARD_REF(TYPE) element,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_01) elements_01,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_02) elements_02
                        ) const;

    template <typename TYPE, typename ELEMENTS_01,
                             typename ELEMENTS_02,
                             typename ELEMENTS_03>
    void pushBackHelper(bdld::DatumArrayBuilder *builder,
                        BSLS_COMPILERFEATURES_FORWARD_REF(TYPE) element,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_01) elements_01,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_02) elements_02,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_03) elements_03
                        ) const;

    template <typename TYPE, typename ELEMENTS_01,
                             typename ELEMENTS_02,
                             typename ELEMENTS_03,
                             typename ELEMENTS_04>
    void pushBackHelper(bdld::DatumArrayBuilder *builder,
                        BSLS_COMPILERFEATURES_FORWARD_REF(TYPE) element,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_01) elements_01,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_02) elements_02,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_03) elements_03,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_04) elements_04
                        ) const;

    template <typename TYPE, typename ELEMENTS_01,
                             typename ELEMENTS_02,
                             typename ELEMENTS_03,
                             typename ELEMENTS_04,
                             typename ELEMENTS_05>
    void pushBackHelper(bdld::DatumArrayBuilder *builder,
                        BSLS_COMPILERFEATURES_FORWARD_REF(TYPE) element,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_01) elements_01,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_02) elements_02,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_03) elements_03,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_04) elements_04,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_05) elements_05
                        ) const;

    template <typename TYPE, typename ELEMENTS_01,
                             typename ELEMENTS_02,
                             typename ELEMENTS_03,
                             typename ELEMENTS_04,
                             typename ELEMENTS_05,
                             typename ELEMENTS_06>
    void pushBackHelper(bdld::DatumArrayBuilder *builder,
                        BSLS_COMPILERFEATURES_FORWARD_REF(TYPE) element,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_01) elements_01,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_02) elements_02,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_03) elements_03,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_04) elements_04,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_05) elements_05,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_06) elements_06
                        ) const;

    template <typename TYPE, typename ELEMENTS_01,
                             typename ELEMENTS_02,
                             typename ELEMENTS_03,
                             typename ELEMENTS_04,
                             typename ELEMENTS_05,
                             typename ELEMENTS_06,
                             typename ELEMENTS_07>
    void pushBackHelper(bdld::DatumArrayBuilder *builder,
                        BSLS_COMPILERFEATURES_FORWARD_REF(TYPE) element,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_01) elements_01,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_02) elements_02,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_03) elements_03,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_04) elements_04,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_05) elements_05,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_06) elements_06,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_07) elements_07
                        ) const;

    template <typename TYPE, typename ELEMENTS_01,
                             typename ELEMENTS_02,
                             typename ELEMENTS_03,
                             typename ELEMENTS_04,
                             typename ELEMENTS_05,
                             typename ELEMENTS_06,
                             typename ELEMENTS_07,
                             typename ELEMENTS_08>
    void pushBackHelper(bdld::DatumArrayBuilder *builder,
                        BSLS_COMPILERFEATURES_FORWARD_REF(TYPE) element,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_01) elements_01,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_02) elements_02,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_03) elements_03,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_04) elements_04,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_05) elements_05,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_06) elements_06,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_07) elements_07,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_08) elements_08
                        ) const;

    template <typename TYPE, typename ELEMENTS_01,
                             typename ELEMENTS_02,
                             typename ELEMENTS_03,
                             typename ELEMENTS_04,
                             typename ELEMENTS_05,
                             typename ELEMENTS_06,
                             typename ELEMENTS_07,
                             typename ELEMENTS_08,
                             typename ELEMENTS_09>
    void pushBackHelper(bdld::DatumArrayBuilder *builder,
                        BSLS_COMPILERFEATURES_FORWARD_REF(TYPE) element,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_01) elements_01,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_02) elements_02,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_03) elements_03,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_04) elements_04,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_05) elements_05,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_06) elements_06,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_07) elements_07,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_08) elements_08,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_09) elements_09
                        ) const;

    template <typename TYPE, typename ELEMENTS_01,
                             typename ELEMENTS_02,
                             typename ELEMENTS_03,
                             typename ELEMENTS_04,
                             typename ELEMENTS_05,
                             typename ELEMENTS_06,
                             typename ELEMENTS_07,
                             typename ELEMENTS_08,
                             typename ELEMENTS_09,
                             typename ELEMENTS_10>
    void pushBackHelper(bdld::DatumArrayBuilder *builder,
                        BSLS_COMPILERFEATURES_FORWARD_REF(TYPE) element,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_01) elements_01,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_02) elements_02,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_03) elements_03,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_04) elements_04,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_05) elements_05,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_06) elements_06,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_07) elements_07,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_08) elements_08,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_09) elements_09,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_10) elements_10
                        ) const;

    template <typename TYPE, typename ELEMENTS_01,
                             typename ELEMENTS_02,
                             typename ELEMENTS_03,
                             typename ELEMENTS_04,
                             typename ELEMENTS_05,
                             typename ELEMENTS_06,
                             typename ELEMENTS_07,
                             typename ELEMENTS_08,
                             typename ELEMENTS_09,
                             typename ELEMENTS_10,
                             typename ELEMENTS_11>
    void pushBackHelper(bdld::DatumArrayBuilder *builder,
                        BSLS_COMPILERFEATURES_FORWARD_REF(TYPE) element,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_01) elements_01,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_02) elements_02,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_03) elements_03,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_04) elements_04,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_05) elements_05,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_06) elements_06,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_07) elements_07,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_08) elements_08,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_09) elements_09,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_10) elements_10,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_11) elements_11
                        ) const;

    template <typename TYPE, typename ELEMENTS_01,
                             typename ELEMENTS_02,
                             typename ELEMENTS_03,
                             typename ELEMENTS_04,
                             typename ELEMENTS_05,
                             typename ELEMENTS_06,
                             typename ELEMENTS_07,
                             typename ELEMENTS_08,
                             typename ELEMENTS_09,
                             typename ELEMENTS_10,
                             typename ELEMENTS_11,
                             typename ELEMENTS_12>
    void pushBackHelper(bdld::DatumArrayBuilder *builder,
                        BSLS_COMPILERFEATURES_FORWARD_REF(TYPE) element,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_01) elements_01,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_02) elements_02,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_03) elements_03,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_04) elements_04,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_05) elements_05,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_06) elements_06,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_07) elements_07,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_08) elements_08,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_09) elements_09,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_10) elements_10,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_11) elements_11,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_12) elements_12
                        ) const;

    template <typename TYPE, typename ELEMENTS_01,
                             typename ELEMENTS_02,
                             typename ELEMENTS_03,
                             typename ELEMENTS_04,
                             typename ELEMENTS_05,
                             typename ELEMENTS_06,
                             typename ELEMENTS_07,
                             typename ELEMENTS_08,
                             typename ELEMENTS_09,
                             typename ELEMENTS_10,
                             typename ELEMENTS_11,
                             typename ELEMENTS_12,
                             typename ELEMENTS_13>
    void pushBackHelper(bdld::DatumArrayBuilder *builder,
                        BSLS_COMPILERFEATURES_FORWARD_REF(TYPE) element,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_01) elements_01,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_02) elements_02,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_03) elements_03,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_04) elements_04,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_05) elements_05,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_06) elements_06,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_07) elements_07,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_08) elements_08,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_09) elements_09,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_10) elements_10,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_11) elements_11,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_12) elements_12,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_13) elements_13
                        ) const;

    template <typename TYPE, typename ELEMENTS_01,
                             typename ELEMENTS_02,
                             typename ELEMENTS_03,
                             typename ELEMENTS_04,
                             typename ELEMENTS_05,
                             typename ELEMENTS_06,
                             typename ELEMENTS_07,
                             typename ELEMENTS_08,
                             typename ELEMENTS_09,
                             typename ELEMENTS_10,
                             typename ELEMENTS_11,
                             typename ELEMENTS_12,
                             typename ELEMENTS_13,
                             typename ELEMENTS_14>
    void pushBackHelper(bdld::DatumArrayBuilder *builder,
                        BSLS_COMPILERFEATURES_FORWARD_REF(TYPE) element,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_01) elements_01,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_02) elements_02,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_03) elements_03,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_04) elements_04,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_05) elements_05,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_06) elements_06,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_07) elements_07,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_08) elements_08,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_09) elements_09,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_10) elements_10,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_11) elements_11,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_12) elements_12,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_13) elements_13,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_14) elements_14
                        ) const;

    template <typename TYPE, typename ELEMENTS_01,
                             typename ELEMENTS_02,
                             typename ELEMENTS_03,
                             typename ELEMENTS_04,
                             typename ELEMENTS_05,
                             typename ELEMENTS_06,
                             typename ELEMENTS_07,
                             typename ELEMENTS_08,
                             typename ELEMENTS_09,
                             typename ELEMENTS_10,
                             typename ELEMENTS_11,
                             typename ELEMENTS_12,
                             typename ELEMENTS_13,
                             typename ELEMENTS_14,
                             typename ELEMENTS_15>
    void pushBackHelper(bdld::DatumArrayBuilder *builder,
                        BSLS_COMPILERFEATURES_FORWARD_REF(TYPE) element,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_01) elements_01,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_02) elements_02,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_03) elements_03,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_04) elements_04,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_05) elements_05,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_06) elements_06,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_07) elements_07,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_08) elements_08,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_09) elements_09,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_10) elements_10,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_11) elements_11,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_12) elements_12,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_13) elements_13,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_14) elements_14,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_15) elements_15
                        ) const;

    template <typename TYPE, typename ELEMENTS_01,
                             typename ELEMENTS_02,
                             typename ELEMENTS_03,
                             typename ELEMENTS_04,
                             typename ELEMENTS_05,
                             typename ELEMENTS_06,
                             typename ELEMENTS_07,
                             typename ELEMENTS_08,
                             typename ELEMENTS_09,
                             typename ELEMENTS_10,
                             typename ELEMENTS_11,
                             typename ELEMENTS_12,
                             typename ELEMENTS_13,
                             typename ELEMENTS_14,
                             typename ELEMENTS_15,
                             typename ELEMENTS_16>
    void pushBackHelper(bdld::DatumArrayBuilder *builder,
                        BSLS_COMPILERFEATURES_FORWARD_REF(TYPE) element,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_01) elements_01,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_02) elements_02,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_03) elements_03,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_04) elements_04,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_05) elements_05,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_06) elements_06,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_07) elements_07,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_08) elements_08,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_09) elements_09,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_10) elements_10,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_11) elements_11,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_12) elements_12,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_13) elements_13,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_14) elements_14,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_15) elements_15,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_16) elements_16
                        ) const;

    template <typename TYPE>
    void pushBackHelper(bdld::DatumMapBuilder                  *builder,
                        const bslstl::StringRef&                key,
                        BSLS_COMPILERFEATURES_FORWARD_REF(TYPE) value
                        ) const;

    template <typename TYPE, typename ENTRIES_01,
                             typename ENTRIES_02>
    void pushBackHelper(bdld::DatumMapBuilder                  *builder,
                        const bslstl::StringRef&                key,
                        BSLS_COMPILERFEATURES_FORWARD_REF(TYPE) value,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_01) entries_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_02) entries_02
                        ) const;

    template <typename TYPE, typename ENTRIES_01,
                             typename ENTRIES_02,
                             typename ENTRIES_03,
                             typename ENTRIES_04>
    void pushBackHelper(bdld::DatumMapBuilder                  *builder,
                        const bslstl::StringRef&                key,
                        BSLS_COMPILERFEATURES_FORWARD_REF(TYPE) value,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_01) entries_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_02) entries_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_03) entries_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_04) entries_04
                        ) const;

    template <typename TYPE, typename ENTRIES_01,
                             typename ENTRIES_02,
                             typename ENTRIES_03,
                             typename ENTRIES_04,
                             typename ENTRIES_05,
                             typename ENTRIES_06>
    void pushBackHelper(bdld::DatumMapBuilder                  *builder,
                        const bslstl::StringRef&                key,
                        BSLS_COMPILERFEATURES_FORWARD_REF(TYPE) value,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_01) entries_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_02) entries_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_03) entries_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_04) entries_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_05) entries_05,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_06) entries_06
                        ) const;

    template <typename TYPE, typename ENTRIES_01,
                             typename ENTRIES_02,
                             typename ENTRIES_03,
                             typename ENTRIES_04,
                             typename ENTRIES_05,
                             typename ENTRIES_06,
                             typename ENTRIES_07,
                             typename ENTRIES_08>
    void pushBackHelper(bdld::DatumMapBuilder                  *builder,
                        const bslstl::StringRef&                key,
                        BSLS_COMPILERFEATURES_FORWARD_REF(TYPE) value,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_01) entries_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_02) entries_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_03) entries_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_04) entries_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_05) entries_05,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_06) entries_06,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_07) entries_07,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_08) entries_08
                        ) const;

    template <typename TYPE, typename ENTRIES_01,
                             typename ENTRIES_02,
                             typename ENTRIES_03,
                             typename ENTRIES_04,
                             typename ENTRIES_05,
                             typename ENTRIES_06,
                             typename ENTRIES_07,
                             typename ENTRIES_08,
                             typename ENTRIES_09,
                             typename ENTRIES_10>
    void pushBackHelper(bdld::DatumMapBuilder                  *builder,
                        const bslstl::StringRef&                key,
                        BSLS_COMPILERFEATURES_FORWARD_REF(TYPE) value,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_01) entries_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_02) entries_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_03) entries_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_04) entries_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_05) entries_05,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_06) entries_06,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_07) entries_07,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_08) entries_08,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_09) entries_09,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_10) entries_10
                        ) const;

    template <typename TYPE, typename ENTRIES_01,
                             typename ENTRIES_02,
                             typename ENTRIES_03,
                             typename ENTRIES_04,
                             typename ENTRIES_05,
                             typename ENTRIES_06,
                             typename ENTRIES_07,
                             typename ENTRIES_08,
                             typename ENTRIES_09,
                             typename ENTRIES_10,
                             typename ENTRIES_11,
                             typename ENTRIES_12>
    void pushBackHelper(bdld::DatumMapBuilder                  *builder,
                        const bslstl::StringRef&                key,
                        BSLS_COMPILERFEATURES_FORWARD_REF(TYPE) value,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_01) entries_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_02) entries_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_03) entries_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_04) entries_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_05) entries_05,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_06) entries_06,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_07) entries_07,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_08) entries_08,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_09) entries_09,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_10) entries_10,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_11) entries_11,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_12) entries_12
                        ) const;

    template <typename TYPE, typename ENTRIES_01,
                             typename ENTRIES_02,
                             typename ENTRIES_03,
                             typename ENTRIES_04,
                             typename ENTRIES_05,
                             typename ENTRIES_06,
                             typename ENTRIES_07,
                             typename ENTRIES_08,
                             typename ENTRIES_09,
                             typename ENTRIES_10,
                             typename ENTRIES_11,
                             typename ENTRIES_12,
                             typename ENTRIES_13,
                             typename ENTRIES_14>
    void pushBackHelper(bdld::DatumMapBuilder                  *builder,
                        const bslstl::StringRef&                key,
                        BSLS_COMPILERFEATURES_FORWARD_REF(TYPE) value,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_01) entries_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_02) entries_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_03) entries_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_04) entries_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_05) entries_05,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_06) entries_06,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_07) entries_07,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_08) entries_08,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_09) entries_09,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_10) entries_10,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_11) entries_11,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_12) entries_12,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_13) entries_13,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_14) entries_14
                        ) const;

    template <typename TYPE, typename ENTRIES_01,
                             typename ENTRIES_02,
                             typename ENTRIES_03,
                             typename ENTRIES_04,
                             typename ENTRIES_05,
                             typename ENTRIES_06,
                             typename ENTRIES_07,
                             typename ENTRIES_08,
                             typename ENTRIES_09,
                             typename ENTRIES_10,
                             typename ENTRIES_11,
                             typename ENTRIES_12,
                             typename ENTRIES_13,
                             typename ENTRIES_14,
                             typename ENTRIES_15,
                             typename ENTRIES_16>
    void pushBackHelper(bdld::DatumMapBuilder                  *builder,
                        const bslstl::StringRef&                key,
                        BSLS_COMPILERFEATURES_FORWARD_REF(TYPE) value,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_01) entries_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_02) entries_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_03) entries_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_04) entries_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_05) entries_05,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_06) entries_06,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_07) entries_07,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_08) entries_08,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_09) entries_09,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_10) entries_10,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_11) entries_11,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_12) entries_12,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_13) entries_13,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_14) entries_14,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_15) entries_15,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_16) entries_16
                        ) const;

    template <typename TYPE, typename ENTRIES_01,
                             typename ENTRIES_02,
                             typename ENTRIES_03,
                             typename ENTRIES_04,
                             typename ENTRIES_05,
                             typename ENTRIES_06,
                             typename ENTRIES_07,
                             typename ENTRIES_08,
                             typename ENTRIES_09,
                             typename ENTRIES_10,
                             typename ENTRIES_11,
                             typename ENTRIES_12,
                             typename ENTRIES_13,
                             typename ENTRIES_14,
                             typename ENTRIES_15,
                             typename ENTRIES_16,
                             typename ENTRIES_17,
                             typename ENTRIES_18>
    void pushBackHelper(bdld::DatumMapBuilder                  *builder,
                        const bslstl::StringRef&                key,
                        BSLS_COMPILERFEATURES_FORWARD_REF(TYPE) value,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_01) entries_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_02) entries_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_03) entries_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_04) entries_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_05) entries_05,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_06) entries_06,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_07) entries_07,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_08) entries_08,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_09) entries_09,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_10) entries_10,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_11) entries_11,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_12) entries_12,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_13) entries_13,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_14) entries_14,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_15) entries_15,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_16) entries_16,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_17) entries_17,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_18) entries_18
                        ) const;

    template <typename TYPE, typename ENTRIES_01,
                             typename ENTRIES_02,
                             typename ENTRIES_03,
                             typename ENTRIES_04,
                             typename ENTRIES_05,
                             typename ENTRIES_06,
                             typename ENTRIES_07,
                             typename ENTRIES_08,
                             typename ENTRIES_09,
                             typename ENTRIES_10,
                             typename ENTRIES_11,
                             typename ENTRIES_12,
                             typename ENTRIES_13,
                             typename ENTRIES_14,
                             typename ENTRIES_15,
                             typename ENTRIES_16,
                             typename ENTRIES_17,
                             typename ENTRIES_18,
                             typename ENTRIES_19,
                             typename ENTRIES_20>
    void pushBackHelper(bdld::DatumMapBuilder                  *builder,
                        const bslstl::StringRef&                key,
                        BSLS_COMPILERFEATURES_FORWARD_REF(TYPE) value,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_01) entries_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_02) entries_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_03) entries_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_04) entries_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_05) entries_05,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_06) entries_06,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_07) entries_07,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_08) entries_08,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_09) entries_09,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_10) entries_10,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_11) entries_11,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_12) entries_12,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_13) entries_13,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_14) entries_14,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_15) entries_15,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_16) entries_16,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_17) entries_17,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_18) entries_18,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_19) entries_19,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_20) entries_20
                        ) const;

    template <typename TYPE, typename ENTRIES_01,
                             typename ENTRIES_02,
                             typename ENTRIES_03,
                             typename ENTRIES_04,
                             typename ENTRIES_05,
                             typename ENTRIES_06,
                             typename ENTRIES_07,
                             typename ENTRIES_08,
                             typename ENTRIES_09,
                             typename ENTRIES_10,
                             typename ENTRIES_11,
                             typename ENTRIES_12,
                             typename ENTRIES_13,
                             typename ENTRIES_14,
                             typename ENTRIES_15,
                             typename ENTRIES_16,
                             typename ENTRIES_17,
                             typename ENTRIES_18,
                             typename ENTRIES_19,
                             typename ENTRIES_20,
                             typename ENTRIES_21,
                             typename ENTRIES_22>
    void pushBackHelper(bdld::DatumMapBuilder                  *builder,
                        const bslstl::StringRef&                key,
                        BSLS_COMPILERFEATURES_FORWARD_REF(TYPE) value,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_01) entries_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_02) entries_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_03) entries_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_04) entries_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_05) entries_05,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_06) entries_06,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_07) entries_07,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_08) entries_08,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_09) entries_09,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_10) entries_10,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_11) entries_11,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_12) entries_12,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_13) entries_13,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_14) entries_14,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_15) entries_15,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_16) entries_16,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_17) entries_17,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_18) entries_18,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_19) entries_19,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_20) entries_20,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_21) entries_21,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_22) entries_22
                        ) const;

    template <typename TYPE, typename ENTRIES_01,
                             typename ENTRIES_02,
                             typename ENTRIES_03,
                             typename ENTRIES_04,
                             typename ENTRIES_05,
                             typename ENTRIES_06,
                             typename ENTRIES_07,
                             typename ENTRIES_08,
                             typename ENTRIES_09,
                             typename ENTRIES_10,
                             typename ENTRIES_11,
                             typename ENTRIES_12,
                             typename ENTRIES_13,
                             typename ENTRIES_14,
                             typename ENTRIES_15,
                             typename ENTRIES_16,
                             typename ENTRIES_17,
                             typename ENTRIES_18,
                             typename ENTRIES_19,
                             typename ENTRIES_20,
                             typename ENTRIES_21,
                             typename ENTRIES_22,
                             typename ENTRIES_23,
                             typename ENTRIES_24>
    void pushBackHelper(bdld::DatumMapBuilder                  *builder,
                        const bslstl::StringRef&                key,
                        BSLS_COMPILERFEATURES_FORWARD_REF(TYPE) value,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_01) entries_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_02) entries_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_03) entries_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_04) entries_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_05) entries_05,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_06) entries_06,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_07) entries_07,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_08) entries_08,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_09) entries_09,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_10) entries_10,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_11) entries_11,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_12) entries_12,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_13) entries_13,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_14) entries_14,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_15) entries_15,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_16) entries_16,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_17) entries_17,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_18) entries_18,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_19) entries_19,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_20) entries_20,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_21) entries_21,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_22) entries_22,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_23) entries_23,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_24) entries_24
                        ) const;

    template <typename TYPE, typename ENTRIES_01,
                             typename ENTRIES_02,
                             typename ENTRIES_03,
                             typename ENTRIES_04,
                             typename ENTRIES_05,
                             typename ENTRIES_06,
                             typename ENTRIES_07,
                             typename ENTRIES_08,
                             typename ENTRIES_09,
                             typename ENTRIES_10,
                             typename ENTRIES_11,
                             typename ENTRIES_12,
                             typename ENTRIES_13,
                             typename ENTRIES_14,
                             typename ENTRIES_15,
                             typename ENTRIES_16,
                             typename ENTRIES_17,
                             typename ENTRIES_18,
                             typename ENTRIES_19,
                             typename ENTRIES_20,
                             typename ENTRIES_21,
                             typename ENTRIES_22,
                             typename ENTRIES_23,
                             typename ENTRIES_24,
                             typename ENTRIES_25,
                             typename ENTRIES_26>
    void pushBackHelper(bdld::DatumMapBuilder                  *builder,
                        const bslstl::StringRef&                key,
                        BSLS_COMPILERFEATURES_FORWARD_REF(TYPE) value,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_01) entries_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_02) entries_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_03) entries_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_04) entries_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_05) entries_05,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_06) entries_06,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_07) entries_07,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_08) entries_08,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_09) entries_09,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_10) entries_10,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_11) entries_11,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_12) entries_12,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_13) entries_13,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_14) entries_14,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_15) entries_15,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_16) entries_16,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_17) entries_17,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_18) entries_18,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_19) entries_19,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_20) entries_20,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_21) entries_21,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_22) entries_22,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_23) entries_23,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_24) entries_24,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_25) entries_25,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_26) entries_26
                        ) const;

    template <typename TYPE, typename ENTRIES_01,
                             typename ENTRIES_02,
                             typename ENTRIES_03,
                             typename ENTRIES_04,
                             typename ENTRIES_05,
                             typename ENTRIES_06,
                             typename ENTRIES_07,
                             typename ENTRIES_08,
                             typename ENTRIES_09,
                             typename ENTRIES_10,
                             typename ENTRIES_11,
                             typename ENTRIES_12,
                             typename ENTRIES_13,
                             typename ENTRIES_14,
                             typename ENTRIES_15,
                             typename ENTRIES_16,
                             typename ENTRIES_17,
                             typename ENTRIES_18,
                             typename ENTRIES_19,
                             typename ENTRIES_20,
                             typename ENTRIES_21,
                             typename ENTRIES_22,
                             typename ENTRIES_23,
                             typename ENTRIES_24,
                             typename ENTRIES_25,
                             typename ENTRIES_26,
                             typename ENTRIES_27,
                             typename ENTRIES_28>
    void pushBackHelper(bdld::DatumMapBuilder                  *builder,
                        const bslstl::StringRef&                key,
                        BSLS_COMPILERFEATURES_FORWARD_REF(TYPE) value,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_01) entries_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_02) entries_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_03) entries_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_04) entries_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_05) entries_05,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_06) entries_06,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_07) entries_07,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_08) entries_08,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_09) entries_09,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_10) entries_10,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_11) entries_11,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_12) entries_12,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_13) entries_13,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_14) entries_14,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_15) entries_15,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_16) entries_16,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_17) entries_17,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_18) entries_18,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_19) entries_19,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_20) entries_20,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_21) entries_21,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_22) entries_22,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_23) entries_23,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_24) entries_24,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_25) entries_25,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_26) entries_26,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_27) entries_27,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_28) entries_28
                        ) const;

    template <typename TYPE, typename ENTRIES_01,
                             typename ENTRIES_02,
                             typename ENTRIES_03,
                             typename ENTRIES_04,
                             typename ENTRIES_05,
                             typename ENTRIES_06,
                             typename ENTRIES_07,
                             typename ENTRIES_08,
                             typename ENTRIES_09,
                             typename ENTRIES_10,
                             typename ENTRIES_11,
                             typename ENTRIES_12,
                             typename ENTRIES_13,
                             typename ENTRIES_14,
                             typename ENTRIES_15,
                             typename ENTRIES_16,
                             typename ENTRIES_17,
                             typename ENTRIES_18,
                             typename ENTRIES_19,
                             typename ENTRIES_20,
                             typename ENTRIES_21,
                             typename ENTRIES_22,
                             typename ENTRIES_23,
                             typename ENTRIES_24,
                             typename ENTRIES_25,
                             typename ENTRIES_26,
                             typename ENTRIES_27,
                             typename ENTRIES_28,
                             typename ENTRIES_29,
                             typename ENTRIES_30>
    void pushBackHelper(bdld::DatumMapBuilder                  *builder,
                        const bslstl::StringRef&                key,
                        BSLS_COMPILERFEATURES_FORWARD_REF(TYPE) value,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_01) entries_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_02) entries_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_03) entries_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_04) entries_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_05) entries_05,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_06) entries_06,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_07) entries_07,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_08) entries_08,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_09) entries_09,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_10) entries_10,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_11) entries_11,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_12) entries_12,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_13) entries_13,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_14) entries_14,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_15) entries_15,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_16) entries_16,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_17) entries_17,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_18) entries_18,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_19) entries_19,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_20) entries_20,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_21) entries_21,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_22) entries_22,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_23) entries_23,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_24) entries_24,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_25) entries_25,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_26) entries_26,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_27) entries_27,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_28) entries_28,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_29) entries_29,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_30) entries_30
                        ) const;

    template <typename TYPE, typename ENTRIES_01,
                             typename ENTRIES_02,
                             typename ENTRIES_03,
                             typename ENTRIES_04,
                             typename ENTRIES_05,
                             typename ENTRIES_06,
                             typename ENTRIES_07,
                             typename ENTRIES_08,
                             typename ENTRIES_09,
                             typename ENTRIES_10,
                             typename ENTRIES_11,
                             typename ENTRIES_12,
                             typename ENTRIES_13,
                             typename ENTRIES_14,
                             typename ENTRIES_15,
                             typename ENTRIES_16,
                             typename ENTRIES_17,
                             typename ENTRIES_18,
                             typename ENTRIES_19,
                             typename ENTRIES_20,
                             typename ENTRIES_21,
                             typename ENTRIES_22,
                             typename ENTRIES_23,
                             typename ENTRIES_24,
                             typename ENTRIES_25,
                             typename ENTRIES_26,
                             typename ENTRIES_27,
                             typename ENTRIES_28,
                             typename ENTRIES_29,
                             typename ENTRIES_30,
                             typename ENTRIES_31,
                             typename ENTRIES_32>
    void pushBackHelper(bdld::DatumMapBuilder                  *builder,
                        const bslstl::StringRef&                key,
                        BSLS_COMPILERFEATURES_FORWARD_REF(TYPE) value,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_01) entries_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_02) entries_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_03) entries_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_04) entries_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_05) entries_05,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_06) entries_06,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_07) entries_07,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_08) entries_08,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_09) entries_09,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_10) entries_10,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_11) entries_11,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_12) entries_12,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_13) entries_13,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_14) entries_14,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_15) entries_15,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_16) entries_16,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_17) entries_17,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_18) entries_18,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_19) entries_19,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_20) entries_20,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_21) entries_21,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_22) entries_22,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_23) entries_23,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_24) entries_24,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_25) entries_25,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_26) entries_26,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_27) entries_27,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_28) entries_28,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_29) entries_29,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_30) entries_30,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_31) entries_31,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_32) entries_32
                        ) const;

    template <typename TYPE>
    void pushBackHelper(bdld::DatumMapOwningKeysBuilder *builder,
                        const bslstl::StringRef&         key,
                        BSLS_COMPILERFEATURES_FORWARD_REF(TYPE) value
                        ) const;

    template <typename TYPE, typename ENTRIES_01,
                             typename ENTRIES_02>
    void pushBackHelper(bdld::DatumMapOwningKeysBuilder *builder,
                        const bslstl::StringRef&         key,
                        BSLS_COMPILERFEATURES_FORWARD_REF(TYPE) value,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_01) entries_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_02) entries_02
                        ) const;

    template <typename TYPE, typename ENTRIES_01,
                             typename ENTRIES_02,
                             typename ENTRIES_03,
                             typename ENTRIES_04>
    void pushBackHelper(bdld::DatumMapOwningKeysBuilder *builder,
                        const bslstl::StringRef&         key,
                        BSLS_COMPILERFEATURES_FORWARD_REF(TYPE) value,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_01) entries_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_02) entries_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_03) entries_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_04) entries_04
                        ) const;

    template <typename TYPE, typename ENTRIES_01,
                             typename ENTRIES_02,
                             typename ENTRIES_03,
                             typename ENTRIES_04,
                             typename ENTRIES_05,
                             typename ENTRIES_06>
    void pushBackHelper(bdld::DatumMapOwningKeysBuilder *builder,
                        const bslstl::StringRef&         key,
                        BSLS_COMPILERFEATURES_FORWARD_REF(TYPE) value,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_01) entries_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_02) entries_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_03) entries_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_04) entries_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_05) entries_05,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_06) entries_06
                        ) const;

    template <typename TYPE, typename ENTRIES_01,
                             typename ENTRIES_02,
                             typename ENTRIES_03,
                             typename ENTRIES_04,
                             typename ENTRIES_05,
                             typename ENTRIES_06,
                             typename ENTRIES_07,
                             typename ENTRIES_08>
    void pushBackHelper(bdld::DatumMapOwningKeysBuilder *builder,
                        const bslstl::StringRef&         key,
                        BSLS_COMPILERFEATURES_FORWARD_REF(TYPE) value,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_01) entries_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_02) entries_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_03) entries_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_04) entries_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_05) entries_05,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_06) entries_06,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_07) entries_07,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_08) entries_08
                        ) const;

    template <typename TYPE, typename ENTRIES_01,
                             typename ENTRIES_02,
                             typename ENTRIES_03,
                             typename ENTRIES_04,
                             typename ENTRIES_05,
                             typename ENTRIES_06,
                             typename ENTRIES_07,
                             typename ENTRIES_08,
                             typename ENTRIES_09,
                             typename ENTRIES_10>
    void pushBackHelper(bdld::DatumMapOwningKeysBuilder *builder,
                        const bslstl::StringRef&         key,
                        BSLS_COMPILERFEATURES_FORWARD_REF(TYPE) value,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_01) entries_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_02) entries_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_03) entries_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_04) entries_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_05) entries_05,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_06) entries_06,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_07) entries_07,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_08) entries_08,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_09) entries_09,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_10) entries_10
                        ) const;

    template <typename TYPE, typename ENTRIES_01,
                             typename ENTRIES_02,
                             typename ENTRIES_03,
                             typename ENTRIES_04,
                             typename ENTRIES_05,
                             typename ENTRIES_06,
                             typename ENTRIES_07,
                             typename ENTRIES_08,
                             typename ENTRIES_09,
                             typename ENTRIES_10,
                             typename ENTRIES_11,
                             typename ENTRIES_12>
    void pushBackHelper(bdld::DatumMapOwningKeysBuilder *builder,
                        const bslstl::StringRef&         key,
                        BSLS_COMPILERFEATURES_FORWARD_REF(TYPE) value,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_01) entries_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_02) entries_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_03) entries_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_04) entries_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_05) entries_05,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_06) entries_06,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_07) entries_07,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_08) entries_08,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_09) entries_09,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_10) entries_10,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_11) entries_11,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_12) entries_12
                        ) const;

    template <typename TYPE, typename ENTRIES_01,
                             typename ENTRIES_02,
                             typename ENTRIES_03,
                             typename ENTRIES_04,
                             typename ENTRIES_05,
                             typename ENTRIES_06,
                             typename ENTRIES_07,
                             typename ENTRIES_08,
                             typename ENTRIES_09,
                             typename ENTRIES_10,
                             typename ENTRIES_11,
                             typename ENTRIES_12,
                             typename ENTRIES_13,
                             typename ENTRIES_14>
    void pushBackHelper(bdld::DatumMapOwningKeysBuilder *builder,
                        const bslstl::StringRef&         key,
                        BSLS_COMPILERFEATURES_FORWARD_REF(TYPE) value,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_01) entries_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_02) entries_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_03) entries_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_04) entries_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_05) entries_05,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_06) entries_06,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_07) entries_07,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_08) entries_08,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_09) entries_09,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_10) entries_10,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_11) entries_11,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_12) entries_12,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_13) entries_13,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_14) entries_14
                        ) const;

    template <typename TYPE, typename ENTRIES_01,
                             typename ENTRIES_02,
                             typename ENTRIES_03,
                             typename ENTRIES_04,
                             typename ENTRIES_05,
                             typename ENTRIES_06,
                             typename ENTRIES_07,
                             typename ENTRIES_08,
                             typename ENTRIES_09,
                             typename ENTRIES_10,
                             typename ENTRIES_11,
                             typename ENTRIES_12,
                             typename ENTRIES_13,
                             typename ENTRIES_14,
                             typename ENTRIES_15,
                             typename ENTRIES_16>
    void pushBackHelper(bdld::DatumMapOwningKeysBuilder *builder,
                        const bslstl::StringRef&         key,
                        BSLS_COMPILERFEATURES_FORWARD_REF(TYPE) value,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_01) entries_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_02) entries_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_03) entries_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_04) entries_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_05) entries_05,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_06) entries_06,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_07) entries_07,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_08) entries_08,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_09) entries_09,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_10) entries_10,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_11) entries_11,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_12) entries_12,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_13) entries_13,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_14) entries_14,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_15) entries_15,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_16) entries_16
                        ) const;

    template <typename TYPE, typename ENTRIES_01,
                             typename ENTRIES_02,
                             typename ENTRIES_03,
                             typename ENTRIES_04,
                             typename ENTRIES_05,
                             typename ENTRIES_06,
                             typename ENTRIES_07,
                             typename ENTRIES_08,
                             typename ENTRIES_09,
                             typename ENTRIES_10,
                             typename ENTRIES_11,
                             typename ENTRIES_12,
                             typename ENTRIES_13,
                             typename ENTRIES_14,
                             typename ENTRIES_15,
                             typename ENTRIES_16,
                             typename ENTRIES_17,
                             typename ENTRIES_18>
    void pushBackHelper(bdld::DatumMapOwningKeysBuilder *builder,
                        const bslstl::StringRef&         key,
                        BSLS_COMPILERFEATURES_FORWARD_REF(TYPE) value,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_01) entries_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_02) entries_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_03) entries_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_04) entries_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_05) entries_05,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_06) entries_06,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_07) entries_07,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_08) entries_08,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_09) entries_09,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_10) entries_10,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_11) entries_11,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_12) entries_12,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_13) entries_13,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_14) entries_14,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_15) entries_15,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_16) entries_16,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_17) entries_17,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_18) entries_18
                        ) const;

    template <typename TYPE, typename ENTRIES_01,
                             typename ENTRIES_02,
                             typename ENTRIES_03,
                             typename ENTRIES_04,
                             typename ENTRIES_05,
                             typename ENTRIES_06,
                             typename ENTRIES_07,
                             typename ENTRIES_08,
                             typename ENTRIES_09,
                             typename ENTRIES_10,
                             typename ENTRIES_11,
                             typename ENTRIES_12,
                             typename ENTRIES_13,
                             typename ENTRIES_14,
                             typename ENTRIES_15,
                             typename ENTRIES_16,
                             typename ENTRIES_17,
                             typename ENTRIES_18,
                             typename ENTRIES_19,
                             typename ENTRIES_20>
    void pushBackHelper(bdld::DatumMapOwningKeysBuilder *builder,
                        const bslstl::StringRef&         key,
                        BSLS_COMPILERFEATURES_FORWARD_REF(TYPE) value,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_01) entries_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_02) entries_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_03) entries_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_04) entries_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_05) entries_05,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_06) entries_06,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_07) entries_07,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_08) entries_08,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_09) entries_09,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_10) entries_10,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_11) entries_11,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_12) entries_12,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_13) entries_13,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_14) entries_14,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_15) entries_15,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_16) entries_16,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_17) entries_17,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_18) entries_18,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_19) entries_19,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_20) entries_20
                        ) const;

    template <typename TYPE, typename ENTRIES_01,
                             typename ENTRIES_02,
                             typename ENTRIES_03,
                             typename ENTRIES_04,
                             typename ENTRIES_05,
                             typename ENTRIES_06,
                             typename ENTRIES_07,
                             typename ENTRIES_08,
                             typename ENTRIES_09,
                             typename ENTRIES_10,
                             typename ENTRIES_11,
                             typename ENTRIES_12,
                             typename ENTRIES_13,
                             typename ENTRIES_14,
                             typename ENTRIES_15,
                             typename ENTRIES_16,
                             typename ENTRIES_17,
                             typename ENTRIES_18,
                             typename ENTRIES_19,
                             typename ENTRIES_20,
                             typename ENTRIES_21,
                             typename ENTRIES_22>
    void pushBackHelper(bdld::DatumMapOwningKeysBuilder *builder,
                        const bslstl::StringRef&         key,
                        BSLS_COMPILERFEATURES_FORWARD_REF(TYPE) value,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_01) entries_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_02) entries_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_03) entries_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_04) entries_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_05) entries_05,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_06) entries_06,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_07) entries_07,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_08) entries_08,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_09) entries_09,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_10) entries_10,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_11) entries_11,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_12) entries_12,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_13) entries_13,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_14) entries_14,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_15) entries_15,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_16) entries_16,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_17) entries_17,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_18) entries_18,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_19) entries_19,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_20) entries_20,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_21) entries_21,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_22) entries_22
                        ) const;

    template <typename TYPE, typename ENTRIES_01,
                             typename ENTRIES_02,
                             typename ENTRIES_03,
                             typename ENTRIES_04,
                             typename ENTRIES_05,
                             typename ENTRIES_06,
                             typename ENTRIES_07,
                             typename ENTRIES_08,
                             typename ENTRIES_09,
                             typename ENTRIES_10,
                             typename ENTRIES_11,
                             typename ENTRIES_12,
                             typename ENTRIES_13,
                             typename ENTRIES_14,
                             typename ENTRIES_15,
                             typename ENTRIES_16,
                             typename ENTRIES_17,
                             typename ENTRIES_18,
                             typename ENTRIES_19,
                             typename ENTRIES_20,
                             typename ENTRIES_21,
                             typename ENTRIES_22,
                             typename ENTRIES_23,
                             typename ENTRIES_24>
    void pushBackHelper(bdld::DatumMapOwningKeysBuilder *builder,
                        const bslstl::StringRef&         key,
                        BSLS_COMPILERFEATURES_FORWARD_REF(TYPE) value,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_01) entries_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_02) entries_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_03) entries_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_04) entries_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_05) entries_05,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_06) entries_06,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_07) entries_07,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_08) entries_08,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_09) entries_09,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_10) entries_10,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_11) entries_11,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_12) entries_12,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_13) entries_13,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_14) entries_14,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_15) entries_15,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_16) entries_16,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_17) entries_17,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_18) entries_18,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_19) entries_19,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_20) entries_20,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_21) entries_21,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_22) entries_22,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_23) entries_23,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_24) entries_24
                        ) const;

    template <typename TYPE, typename ENTRIES_01,
                             typename ENTRIES_02,
                             typename ENTRIES_03,
                             typename ENTRIES_04,
                             typename ENTRIES_05,
                             typename ENTRIES_06,
                             typename ENTRIES_07,
                             typename ENTRIES_08,
                             typename ENTRIES_09,
                             typename ENTRIES_10,
                             typename ENTRIES_11,
                             typename ENTRIES_12,
                             typename ENTRIES_13,
                             typename ENTRIES_14,
                             typename ENTRIES_15,
                             typename ENTRIES_16,
                             typename ENTRIES_17,
                             typename ENTRIES_18,
                             typename ENTRIES_19,
                             typename ENTRIES_20,
                             typename ENTRIES_21,
                             typename ENTRIES_22,
                             typename ENTRIES_23,
                             typename ENTRIES_24,
                             typename ENTRIES_25,
                             typename ENTRIES_26>
    void pushBackHelper(bdld::DatumMapOwningKeysBuilder *builder,
                        const bslstl::StringRef&         key,
                        BSLS_COMPILERFEATURES_FORWARD_REF(TYPE) value,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_01) entries_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_02) entries_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_03) entries_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_04) entries_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_05) entries_05,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_06) entries_06,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_07) entries_07,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_08) entries_08,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_09) entries_09,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_10) entries_10,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_11) entries_11,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_12) entries_12,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_13) entries_13,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_14) entries_14,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_15) entries_15,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_16) entries_16,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_17) entries_17,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_18) entries_18,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_19) entries_19,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_20) entries_20,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_21) entries_21,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_22) entries_22,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_23) entries_23,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_24) entries_24,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_25) entries_25,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_26) entries_26
                        ) const;

    template <typename TYPE, typename ENTRIES_01,
                             typename ENTRIES_02,
                             typename ENTRIES_03,
                             typename ENTRIES_04,
                             typename ENTRIES_05,
                             typename ENTRIES_06,
                             typename ENTRIES_07,
                             typename ENTRIES_08,
                             typename ENTRIES_09,
                             typename ENTRIES_10,
                             typename ENTRIES_11,
                             typename ENTRIES_12,
                             typename ENTRIES_13,
                             typename ENTRIES_14,
                             typename ENTRIES_15,
                             typename ENTRIES_16,
                             typename ENTRIES_17,
                             typename ENTRIES_18,
                             typename ENTRIES_19,
                             typename ENTRIES_20,
                             typename ENTRIES_21,
                             typename ENTRIES_22,
                             typename ENTRIES_23,
                             typename ENTRIES_24,
                             typename ENTRIES_25,
                             typename ENTRIES_26,
                             typename ENTRIES_27,
                             typename ENTRIES_28>
    void pushBackHelper(bdld::DatumMapOwningKeysBuilder *builder,
                        const bslstl::StringRef&         key,
                        BSLS_COMPILERFEATURES_FORWARD_REF(TYPE) value,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_01) entries_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_02) entries_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_03) entries_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_04) entries_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_05) entries_05,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_06) entries_06,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_07) entries_07,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_08) entries_08,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_09) entries_09,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_10) entries_10,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_11) entries_11,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_12) entries_12,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_13) entries_13,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_14) entries_14,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_15) entries_15,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_16) entries_16,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_17) entries_17,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_18) entries_18,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_19) entries_19,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_20) entries_20,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_21) entries_21,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_22) entries_22,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_23) entries_23,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_24) entries_24,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_25) entries_25,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_26) entries_26,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_27) entries_27,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_28) entries_28
                        ) const;

    template <typename TYPE, typename ENTRIES_01,
                             typename ENTRIES_02,
                             typename ENTRIES_03,
                             typename ENTRIES_04,
                             typename ENTRIES_05,
                             typename ENTRIES_06,
                             typename ENTRIES_07,
                             typename ENTRIES_08,
                             typename ENTRIES_09,
                             typename ENTRIES_10,
                             typename ENTRIES_11,
                             typename ENTRIES_12,
                             typename ENTRIES_13,
                             typename ENTRIES_14,
                             typename ENTRIES_15,
                             typename ENTRIES_16,
                             typename ENTRIES_17,
                             typename ENTRIES_18,
                             typename ENTRIES_19,
                             typename ENTRIES_20,
                             typename ENTRIES_21,
                             typename ENTRIES_22,
                             typename ENTRIES_23,
                             typename ENTRIES_24,
                             typename ENTRIES_25,
                             typename ENTRIES_26,
                             typename ENTRIES_27,
                             typename ENTRIES_28,
                             typename ENTRIES_29,
                             typename ENTRIES_30>
    void pushBackHelper(bdld::DatumMapOwningKeysBuilder *builder,
                        const bslstl::StringRef&         key,
                        BSLS_COMPILERFEATURES_FORWARD_REF(TYPE) value,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_01) entries_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_02) entries_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_03) entries_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_04) entries_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_05) entries_05,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_06) entries_06,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_07) entries_07,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_08) entries_08,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_09) entries_09,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_10) entries_10,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_11) entries_11,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_12) entries_12,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_13) entries_13,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_14) entries_14,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_15) entries_15,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_16) entries_16,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_17) entries_17,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_18) entries_18,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_19) entries_19,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_20) entries_20,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_21) entries_21,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_22) entries_22,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_23) entries_23,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_24) entries_24,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_25) entries_25,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_26) entries_26,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_27) entries_27,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_28) entries_28,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_29) entries_29,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_30) entries_30
                        ) const;

    template <typename TYPE, typename ENTRIES_01,
                             typename ENTRIES_02,
                             typename ENTRIES_03,
                             typename ENTRIES_04,
                             typename ENTRIES_05,
                             typename ENTRIES_06,
                             typename ENTRIES_07,
                             typename ENTRIES_08,
                             typename ENTRIES_09,
                             typename ENTRIES_10,
                             typename ENTRIES_11,
                             typename ENTRIES_12,
                             typename ENTRIES_13,
                             typename ENTRIES_14,
                             typename ENTRIES_15,
                             typename ENTRIES_16,
                             typename ENTRIES_17,
                             typename ENTRIES_18,
                             typename ENTRIES_19,
                             typename ENTRIES_20,
                             typename ENTRIES_21,
                             typename ENTRIES_22,
                             typename ENTRIES_23,
                             typename ENTRIES_24,
                             typename ENTRIES_25,
                             typename ENTRIES_26,
                             typename ENTRIES_27,
                             typename ENTRIES_28,
                             typename ENTRIES_29,
                             typename ENTRIES_30,
                             typename ENTRIES_31,
                             typename ENTRIES_32>
    void pushBackHelper(bdld::DatumMapOwningKeysBuilder *builder,
                        const bslstl::StringRef&         key,
                        BSLS_COMPILERFEATURES_FORWARD_REF(TYPE) value,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_01) entries_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_02) entries_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_03) entries_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_04) entries_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_05) entries_05,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_06) entries_06,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_07) entries_07,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_08) entries_08,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_09) entries_09,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_10) entries_10,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_11) entries_11,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_12) entries_12,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_13) entries_13,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_14) entries_14,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_15) entries_15,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_16) entries_16,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_17) entries_17,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_18) entries_18,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_19) entries_19,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_20) entries_20,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_21) entries_21,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_22) entries_22,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_23) entries_23,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_24) entries_24,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_25) entries_25,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_26) entries_26,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_27) entries_27,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_28) entries_28,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_29) entries_29,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_30) entries_30,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_31) entries_31,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_32) entries_32
                        ) const;

#else
// The generated code below is a workaround for the absence of perfect
// forwarding in some compilers.
    template <typename TYPE>
    void pushBackHelper(bdld::DatumArrayBuilder *builder,
                        BSLS_COMPILERFEATURES_FORWARD_REF(TYPE) element) const;

    template <typename TYPE, typename... ELEMENTS>
    void pushBackHelper(bdld::DatumArrayBuilder *builder,
                        BSLS_COMPILERFEATURES_FORWARD_REF(TYPE) element,
                        BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS)... elements
                        ) const;

    template <typename TYPE>
    void pushBackHelper(bdld::DatumMapBuilder                  *builder,
                        const bslstl::StringRef&                key,
                        BSLS_COMPILERFEATURES_FORWARD_REF(TYPE) value) const;

    template <typename TYPE, typename... ENTRIES>
    void pushBackHelper(bdld::DatumMapBuilder                  *builder,
                        const bslstl::StringRef&                key,
                        BSLS_COMPILERFEATURES_FORWARD_REF(TYPE) value,
                        BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES)... entries
                        ) const;

    template <typename TYPE>
    void pushBackHelper(bdld::DatumMapOwningKeysBuilder *builder,
                        const bslstl::StringRef&         key,
                        BSLS_COMPILERFEATURES_FORWARD_REF(TYPE) value) const;

    template <typename TYPE, typename... ENTRIES>
    void pushBackHelper(bdld::DatumMapOwningKeysBuilder *builder,
                        const bslstl::StringRef&         key,
                        BSLS_COMPILERFEATURES_FORWARD_REF(TYPE) value,
                        BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES)... entries
                        ) const;
#endif

  public:
    // CREATORS
    explicit DatumMaker(bslma::Allocator *basicAllocator);
        // Create a new 'DatumMaker' object using the specified
        // 'basicAllocator' to supply memory for created 'bdld::Datum'
        // objects.

    // ACCESSORS
    bslma::Allocator *allocator() const;
        // Return the allocator used by this object (the one supplied at
        // construction time.

    bdld::Datum operator()() const;
        // Return a 'bdld::Datum' having a null value.

    bdld::Datum operator()(const bslmf::Nil&                value) const;
    bdld::Datum operator()(int                              value) const;
    bdld::Datum operator()(double                           value) const;
    bdld::Datum operator()(bool                             value) const;
    bdld::Datum operator()(const bdld::DatumError&          value) const;
    bdld::Datum operator()(const bdlt::Date&                value) const;
    bdld::Datum operator()(const bdlt::Time&                value) const;
    bdld::Datum operator()(const bdlt::Datetime&            value) const;
    bdld::Datum operator()(const bdlt::DatetimeInterval&    value) const;
    bdld::Datum operator()(bdldfp::Decimal64                value) const;
    bdld::Datum operator()(bsls::Types::Int64               value) const;
    bdld::Datum operator()(const bdld::DatumUdt&            value) const;
    bdld::Datum operator()(const bdld::Datum&               value) const;
    bdld::Datum operator()(const bdld::DatumArrayRef&       value) const;
    bdld::Datum operator()(const bdld::DatumMutableMapRef&  value) const;
        // Return a 'bdld::Datum' having the specified 'value'.  Note that
        // where possible, no memory is allocated - array are returned as
        // references.  Note that 'ConstDatumMapRef' is not supported at the
        // moment.

    bdld::Datum operator()(const bdld::Datum         *elements,
                           int                        size)  const;
    bdld::Datum operator()(const bdld::DatumMapEntry *elements,
                           int                        size,
                           bool                       sorted = false)  const;
        // Return a 'bdld::Datum' having the specified 'size' number of
        // 'elements'.  Note that where possible, no memory is allocated -
        // arrays are returned as references.  Note that 'DatumMapRef' is not
        // supported at the moment.

    bdld::Datum operator()(const bslstl::StringRef&  value) const;
    bdld::Datum operator()(const char               *value) const;
        // Return a 'bdld::Datum' having the specified 'value'.  The returned
        // 'bdld::Datum' object will contain a deep-copy of 'value'.

    template <class TYPE>
    bdld::Datum operator()(const bdlb::NullableValue<TYPE>& value) const;
        // Return a 'bdld::Datum' having the specified 'value', or null if
        // 'value' is unset.

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
    template <typename... ELEMENTS>
    bdld::Datum a(ELEMENTS&&... elements) const;
        // Return a 'bdld::Datum' having an array value of the specified
        // 'elements'.

// IMPORTANT NOTE: The section below was manually modified to reduce the
// maximum number of parameters to 16.
#elif BSLS_COMPILERFEATURES_SIMULATE_VARIADIC_TEMPLATES
    bdld::Datum a(
                  ) const;

    template <typename ELEMENTS_01>
    bdld::Datum a(BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_01) elements_01
                  ) const;

    template <typename ELEMENTS_01,
           typename ELEMENTS_02>
    bdld::Datum a(BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_01) elements_01,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_02) elements_02
                  ) const;

    template <typename ELEMENTS_01,
           typename ELEMENTS_02,
           typename ELEMENTS_03>
    bdld::Datum a(BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_01) elements_01,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_02) elements_02,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_03) elements_03
                  ) const;

    template <typename ELEMENTS_01,
           typename ELEMENTS_02,
           typename ELEMENTS_03,
           typename ELEMENTS_04>
    bdld::Datum a(BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_01) elements_01,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_02) elements_02,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_03) elements_03,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_04) elements_04
                  ) const;

    template <typename ELEMENTS_01,
           typename ELEMENTS_02,
           typename ELEMENTS_03,
           typename ELEMENTS_04,
           typename ELEMENTS_05>
    bdld::Datum a(BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_01) elements_01,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_02) elements_02,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_03) elements_03,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_04) elements_04,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_05) elements_05
                  ) const;

    template <typename ELEMENTS_01,
           typename ELEMENTS_02,
           typename ELEMENTS_03,
           typename ELEMENTS_04,
           typename ELEMENTS_05,
           typename ELEMENTS_06>
    bdld::Datum a(BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_01) elements_01,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_02) elements_02,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_03) elements_03,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_04) elements_04,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_05) elements_05,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_06) elements_06
                  ) const;

    template <typename ELEMENTS_01,
           typename ELEMENTS_02,
           typename ELEMENTS_03,
           typename ELEMENTS_04,
           typename ELEMENTS_05,
           typename ELEMENTS_06,
           typename ELEMENTS_07>
    bdld::Datum a(BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_01) elements_01,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_02) elements_02,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_03) elements_03,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_04) elements_04,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_05) elements_05,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_06) elements_06,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_07) elements_07
                  ) const;

    template <typename ELEMENTS_01,
           typename ELEMENTS_02,
           typename ELEMENTS_03,
           typename ELEMENTS_04,
           typename ELEMENTS_05,
           typename ELEMENTS_06,
           typename ELEMENTS_07,
           typename ELEMENTS_08>
    bdld::Datum a(BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_01) elements_01,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_02) elements_02,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_03) elements_03,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_04) elements_04,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_05) elements_05,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_06) elements_06,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_07) elements_07,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_08) elements_08
                  ) const;

    template <typename ELEMENTS_01,
           typename ELEMENTS_02,
           typename ELEMENTS_03,
           typename ELEMENTS_04,
           typename ELEMENTS_05,
           typename ELEMENTS_06,
           typename ELEMENTS_07,
           typename ELEMENTS_08,
           typename ELEMENTS_09>
    bdld::Datum a(BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_01) elements_01,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_02) elements_02,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_03) elements_03,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_04) elements_04,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_05) elements_05,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_06) elements_06,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_07) elements_07,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_08) elements_08,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_09) elements_09
                  ) const;

    template <typename ELEMENTS_01,
           typename ELEMENTS_02,
           typename ELEMENTS_03,
           typename ELEMENTS_04,
           typename ELEMENTS_05,
           typename ELEMENTS_06,
           typename ELEMENTS_07,
           typename ELEMENTS_08,
           typename ELEMENTS_09,
           typename ELEMENTS_10>
    bdld::Datum a(BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_01) elements_01,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_02) elements_02,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_03) elements_03,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_04) elements_04,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_05) elements_05,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_06) elements_06,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_07) elements_07,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_08) elements_08,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_09) elements_09,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_10) elements_10
                  ) const;

    template <typename ELEMENTS_01,
           typename ELEMENTS_02,
           typename ELEMENTS_03,
           typename ELEMENTS_04,
           typename ELEMENTS_05,
           typename ELEMENTS_06,
           typename ELEMENTS_07,
           typename ELEMENTS_08,
           typename ELEMENTS_09,
           typename ELEMENTS_10,
           typename ELEMENTS_11>
    bdld::Datum a(BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_01) elements_01,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_02) elements_02,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_03) elements_03,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_04) elements_04,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_05) elements_05,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_06) elements_06,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_07) elements_07,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_08) elements_08,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_09) elements_09,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_10) elements_10,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_11) elements_11
                  ) const;

    template <typename ELEMENTS_01,
           typename ELEMENTS_02,
           typename ELEMENTS_03,
           typename ELEMENTS_04,
           typename ELEMENTS_05,
           typename ELEMENTS_06,
           typename ELEMENTS_07,
           typename ELEMENTS_08,
           typename ELEMENTS_09,
           typename ELEMENTS_10,
           typename ELEMENTS_11,
           typename ELEMENTS_12>
    bdld::Datum a(BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_01) elements_01,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_02) elements_02,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_03) elements_03,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_04) elements_04,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_05) elements_05,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_06) elements_06,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_07) elements_07,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_08) elements_08,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_09) elements_09,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_10) elements_10,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_11) elements_11,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_12) elements_12
                  ) const;

    template <typename ELEMENTS_01,
           typename ELEMENTS_02,
           typename ELEMENTS_03,
           typename ELEMENTS_04,
           typename ELEMENTS_05,
           typename ELEMENTS_06,
           typename ELEMENTS_07,
           typename ELEMENTS_08,
           typename ELEMENTS_09,
           typename ELEMENTS_10,
           typename ELEMENTS_11,
           typename ELEMENTS_12,
           typename ELEMENTS_13>
    bdld::Datum a(BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_01) elements_01,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_02) elements_02,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_03) elements_03,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_04) elements_04,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_05) elements_05,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_06) elements_06,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_07) elements_07,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_08) elements_08,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_09) elements_09,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_10) elements_10,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_11) elements_11,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_12) elements_12,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_13) elements_13
                  ) const;

    template <typename ELEMENTS_01,
           typename ELEMENTS_02,
           typename ELEMENTS_03,
           typename ELEMENTS_04,
           typename ELEMENTS_05,
           typename ELEMENTS_06,
           typename ELEMENTS_07,
           typename ELEMENTS_08,
           typename ELEMENTS_09,
           typename ELEMENTS_10,
           typename ELEMENTS_11,
           typename ELEMENTS_12,
           typename ELEMENTS_13,
           typename ELEMENTS_14>
    bdld::Datum a(BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_01) elements_01,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_02) elements_02,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_03) elements_03,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_04) elements_04,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_05) elements_05,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_06) elements_06,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_07) elements_07,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_08) elements_08,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_09) elements_09,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_10) elements_10,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_11) elements_11,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_12) elements_12,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_13) elements_13,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_14) elements_14
                  ) const;

    template <typename ELEMENTS_01,
           typename ELEMENTS_02,
           typename ELEMENTS_03,
           typename ELEMENTS_04,
           typename ELEMENTS_05,
           typename ELEMENTS_06,
           typename ELEMENTS_07,
           typename ELEMENTS_08,
           typename ELEMENTS_09,
           typename ELEMENTS_10,
           typename ELEMENTS_11,
           typename ELEMENTS_12,
           typename ELEMENTS_13,
           typename ELEMENTS_14,
           typename ELEMENTS_15>
    bdld::Datum a(BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_01) elements_01,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_02) elements_02,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_03) elements_03,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_04) elements_04,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_05) elements_05,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_06) elements_06,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_07) elements_07,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_08) elements_08,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_09) elements_09,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_10) elements_10,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_11) elements_11,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_12) elements_12,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_13) elements_13,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_14) elements_14,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_15) elements_15
                  ) const;

    template <typename ELEMENTS_01,
           typename ELEMENTS_02,
           typename ELEMENTS_03,
           typename ELEMENTS_04,
           typename ELEMENTS_05,
           typename ELEMENTS_06,
           typename ELEMENTS_07,
           typename ELEMENTS_08,
           typename ELEMENTS_09,
           typename ELEMENTS_10,
           typename ELEMENTS_11,
           typename ELEMENTS_12,
           typename ELEMENTS_13,
           typename ELEMENTS_14,
           typename ELEMENTS_15,
           typename ELEMENTS_16>
    bdld::Datum a(BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_01) elements_01,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_02) elements_02,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_03) elements_03,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_04) elements_04,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_05) elements_05,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_06) elements_06,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_07) elements_07,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_08) elements_08,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_09) elements_09,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_10) elements_10,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_11) elements_11,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_12) elements_12,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_13) elements_13,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_14) elements_14,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_15) elements_15,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_16) elements_16
                  ) const;
#else
// The generated code below is a workaround for the absence of perfect
// forwarding in some compilers.
    template <typename... ELEMENTS>
    bdld::Datum a(BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS)... elements
                  ) const;
#endif

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
    template <typename... ENTRIES>
    bdld::Datum m(ENTRIES&&... entries) const;
        // Return a 'bdld::Datum' object containing a map of the specified
        // 'entries'.  The 'entries' are supplied as pairs (odd number of
        // 'sizeof.,..(entries)' being an error) where the first specified
        // element is the key, and the second is its corresponding value.  The
        // behavior is undefined if the same key is supplied more than once.
#elif BSLS_COMPILERFEATURES_SIMULATE_VARIADIC_TEMPLATES
    bdld::Datum m() const;

    template <typename ENTRIES_01,
           typename ENTRIES_02>
    bdld::Datum m(BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_01) entries_01,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_02) entries_02
                  ) const;

    template <typename ENTRIES_01,
           typename ENTRIES_02,
           typename ENTRIES_03,
           typename ENTRIES_04>
    bdld::Datum m(BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_01) entries_01,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_02) entries_02,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_03) entries_03,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_04) entries_04
                  ) const;

    template <typename ENTRIES_01,
           typename ENTRIES_02,
           typename ENTRIES_03,
           typename ENTRIES_04,
           typename ENTRIES_05,
           typename ENTRIES_06>
    bdld::Datum m(BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_01) entries_01,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_02) entries_02,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_03) entries_03,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_04) entries_04,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_05) entries_05,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_06) entries_06
                  ) const;

    template <typename ENTRIES_01,
           typename ENTRIES_02,
           typename ENTRIES_03,
           typename ENTRIES_04,
           typename ENTRIES_05,
           typename ENTRIES_06,
           typename ENTRIES_07,
           typename ENTRIES_08>
    bdld::Datum m(BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_01) entries_01,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_02) entries_02,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_03) entries_03,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_04) entries_04,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_05) entries_05,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_06) entries_06,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_07) entries_07,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_08) entries_08
                  ) const;

    template <typename ENTRIES_01,
           typename ENTRIES_02,
           typename ENTRIES_03,
           typename ENTRIES_04,
           typename ENTRIES_05,
           typename ENTRIES_06,
           typename ENTRIES_07,
           typename ENTRIES_08,
           typename ENTRIES_09,
           typename ENTRIES_10>
    bdld::Datum m(BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_01) entries_01,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_02) entries_02,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_03) entries_03,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_04) entries_04,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_05) entries_05,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_06) entries_06,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_07) entries_07,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_08) entries_08,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_09) entries_09,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_10) entries_10
                  ) const;

    template <typename ENTRIES_01,
           typename ENTRIES_02,
           typename ENTRIES_03,
           typename ENTRIES_04,
           typename ENTRIES_05,
           typename ENTRIES_06,
           typename ENTRIES_07,
           typename ENTRIES_08,
           typename ENTRIES_09,
           typename ENTRIES_10,
           typename ENTRIES_11,
           typename ENTRIES_12>
    bdld::Datum m(BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_01) entries_01,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_02) entries_02,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_03) entries_03,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_04) entries_04,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_05) entries_05,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_06) entries_06,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_07) entries_07,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_08) entries_08,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_09) entries_09,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_10) entries_10,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_11) entries_11,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_12) entries_12
                  ) const;

    template <typename ENTRIES_01,
           typename ENTRIES_02,
           typename ENTRIES_03,
           typename ENTRIES_04,
           typename ENTRIES_05,
           typename ENTRIES_06,
           typename ENTRIES_07,
           typename ENTRIES_08,
           typename ENTRIES_09,
           typename ENTRIES_10,
           typename ENTRIES_11,
           typename ENTRIES_12,
           typename ENTRIES_13,
           typename ENTRIES_14>
    bdld::Datum m(BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_01) entries_01,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_02) entries_02,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_03) entries_03,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_04) entries_04,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_05) entries_05,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_06) entries_06,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_07) entries_07,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_08) entries_08,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_09) entries_09,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_10) entries_10,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_11) entries_11,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_12) entries_12,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_13) entries_13,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_14) entries_14
                  ) const;

    template <typename ENTRIES_01,
           typename ENTRIES_02,
           typename ENTRIES_03,
           typename ENTRIES_04,
           typename ENTRIES_05,
           typename ENTRIES_06,
           typename ENTRIES_07,
           typename ENTRIES_08,
           typename ENTRIES_09,
           typename ENTRIES_10,
           typename ENTRIES_11,
           typename ENTRIES_12,
           typename ENTRIES_13,
           typename ENTRIES_14,
           typename ENTRIES_15,
           typename ENTRIES_16>
    bdld::Datum m(BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_01) entries_01,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_02) entries_02,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_03) entries_03,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_04) entries_04,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_05) entries_05,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_06) entries_06,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_07) entries_07,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_08) entries_08,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_09) entries_09,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_10) entries_10,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_11) entries_11,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_12) entries_12,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_13) entries_13,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_14) entries_14,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_15) entries_15,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_16) entries_16
                  ) const;

    template <typename ENTRIES_01,
           typename ENTRIES_02,
           typename ENTRIES_03,
           typename ENTRIES_04,
           typename ENTRIES_05,
           typename ENTRIES_06,
           typename ENTRIES_07,
           typename ENTRIES_08,
           typename ENTRIES_09,
           typename ENTRIES_10,
           typename ENTRIES_11,
           typename ENTRIES_12,
           typename ENTRIES_13,
           typename ENTRIES_14,
           typename ENTRIES_15,
           typename ENTRIES_16,
           typename ENTRIES_17,
           typename ENTRIES_18>
    bdld::Datum m(BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_01) entries_01,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_02) entries_02,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_03) entries_03,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_04) entries_04,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_05) entries_05,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_06) entries_06,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_07) entries_07,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_08) entries_08,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_09) entries_09,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_10) entries_10,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_11) entries_11,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_12) entries_12,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_13) entries_13,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_14) entries_14,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_15) entries_15,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_16) entries_16,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_17) entries_17,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_18) entries_18
                  ) const;

    template <typename ENTRIES_01,
           typename ENTRIES_02,
           typename ENTRIES_03,
           typename ENTRIES_04,
           typename ENTRIES_05,
           typename ENTRIES_06,
           typename ENTRIES_07,
           typename ENTRIES_08,
           typename ENTRIES_09,
           typename ENTRIES_10,
           typename ENTRIES_11,
           typename ENTRIES_12,
           typename ENTRIES_13,
           typename ENTRIES_14,
           typename ENTRIES_15,
           typename ENTRIES_16,
           typename ENTRIES_17,
           typename ENTRIES_18,
           typename ENTRIES_19,
           typename ENTRIES_20>
    bdld::Datum m(BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_01) entries_01,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_02) entries_02,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_03) entries_03,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_04) entries_04,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_05) entries_05,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_06) entries_06,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_07) entries_07,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_08) entries_08,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_09) entries_09,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_10) entries_10,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_11) entries_11,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_12) entries_12,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_13) entries_13,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_14) entries_14,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_15) entries_15,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_16) entries_16,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_17) entries_17,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_18) entries_18,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_19) entries_19,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_20) entries_20
                  ) const;

    template <typename ENTRIES_01,
           typename ENTRIES_02,
           typename ENTRIES_03,
           typename ENTRIES_04,
           typename ENTRIES_05,
           typename ENTRIES_06,
           typename ENTRIES_07,
           typename ENTRIES_08,
           typename ENTRIES_09,
           typename ENTRIES_10,
           typename ENTRIES_11,
           typename ENTRIES_12,
           typename ENTRIES_13,
           typename ENTRIES_14,
           typename ENTRIES_15,
           typename ENTRIES_16,
           typename ENTRIES_17,
           typename ENTRIES_18,
           typename ENTRIES_19,
           typename ENTRIES_20,
           typename ENTRIES_21,
           typename ENTRIES_22>
    bdld::Datum m(BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_01) entries_01,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_02) entries_02,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_03) entries_03,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_04) entries_04,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_05) entries_05,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_06) entries_06,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_07) entries_07,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_08) entries_08,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_09) entries_09,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_10) entries_10,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_11) entries_11,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_12) entries_12,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_13) entries_13,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_14) entries_14,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_15) entries_15,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_16) entries_16,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_17) entries_17,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_18) entries_18,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_19) entries_19,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_20) entries_20,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_21) entries_21,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_22) entries_22
                  ) const;

    template <typename ENTRIES_01,
           typename ENTRIES_02,
           typename ENTRIES_03,
           typename ENTRIES_04,
           typename ENTRIES_05,
           typename ENTRIES_06,
           typename ENTRIES_07,
           typename ENTRIES_08,
           typename ENTRIES_09,
           typename ENTRIES_10,
           typename ENTRIES_11,
           typename ENTRIES_12,
           typename ENTRIES_13,
           typename ENTRIES_14,
           typename ENTRIES_15,
           typename ENTRIES_16,
           typename ENTRIES_17,
           typename ENTRIES_18,
           typename ENTRIES_19,
           typename ENTRIES_20,
           typename ENTRIES_21,
           typename ENTRIES_22,
           typename ENTRIES_23,
           typename ENTRIES_24>
    bdld::Datum m(BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_01) entries_01,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_02) entries_02,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_03) entries_03,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_04) entries_04,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_05) entries_05,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_06) entries_06,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_07) entries_07,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_08) entries_08,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_09) entries_09,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_10) entries_10,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_11) entries_11,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_12) entries_12,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_13) entries_13,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_14) entries_14,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_15) entries_15,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_16) entries_16,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_17) entries_17,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_18) entries_18,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_19) entries_19,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_20) entries_20,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_21) entries_21,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_22) entries_22,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_23) entries_23,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_24) entries_24
                  ) const;

    template <typename ENTRIES_01,
           typename ENTRIES_02,
           typename ENTRIES_03,
           typename ENTRIES_04,
           typename ENTRIES_05,
           typename ENTRIES_06,
           typename ENTRIES_07,
           typename ENTRIES_08,
           typename ENTRIES_09,
           typename ENTRIES_10,
           typename ENTRIES_11,
           typename ENTRIES_12,
           typename ENTRIES_13,
           typename ENTRIES_14,
           typename ENTRIES_15,
           typename ENTRIES_16,
           typename ENTRIES_17,
           typename ENTRIES_18,
           typename ENTRIES_19,
           typename ENTRIES_20,
           typename ENTRIES_21,
           typename ENTRIES_22,
           typename ENTRIES_23,
           typename ENTRIES_24,
           typename ENTRIES_25,
           typename ENTRIES_26>
    bdld::Datum m(BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_01) entries_01,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_02) entries_02,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_03) entries_03,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_04) entries_04,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_05) entries_05,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_06) entries_06,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_07) entries_07,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_08) entries_08,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_09) entries_09,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_10) entries_10,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_11) entries_11,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_12) entries_12,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_13) entries_13,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_14) entries_14,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_15) entries_15,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_16) entries_16,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_17) entries_17,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_18) entries_18,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_19) entries_19,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_20) entries_20,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_21) entries_21,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_22) entries_22,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_23) entries_23,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_24) entries_24,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_25) entries_25,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_26) entries_26
                  ) const;

    template <typename ENTRIES_01,
           typename ENTRIES_02,
           typename ENTRIES_03,
           typename ENTRIES_04,
           typename ENTRIES_05,
           typename ENTRIES_06,
           typename ENTRIES_07,
           typename ENTRIES_08,
           typename ENTRIES_09,
           typename ENTRIES_10,
           typename ENTRIES_11,
           typename ENTRIES_12,
           typename ENTRIES_13,
           typename ENTRIES_14,
           typename ENTRIES_15,
           typename ENTRIES_16,
           typename ENTRIES_17,
           typename ENTRIES_18,
           typename ENTRIES_19,
           typename ENTRIES_20,
           typename ENTRIES_21,
           typename ENTRIES_22,
           typename ENTRIES_23,
           typename ENTRIES_24,
           typename ENTRIES_25,
           typename ENTRIES_26,
           typename ENTRIES_27,
           typename ENTRIES_28>
    bdld::Datum m(BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_01) entries_01,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_02) entries_02,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_03) entries_03,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_04) entries_04,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_05) entries_05,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_06) entries_06,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_07) entries_07,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_08) entries_08,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_09) entries_09,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_10) entries_10,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_11) entries_11,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_12) entries_12,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_13) entries_13,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_14) entries_14,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_15) entries_15,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_16) entries_16,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_17) entries_17,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_18) entries_18,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_19) entries_19,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_20) entries_20,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_21) entries_21,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_22) entries_22,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_23) entries_23,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_24) entries_24,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_25) entries_25,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_26) entries_26,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_27) entries_27,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_28) entries_28
                  ) const;

    template <typename ENTRIES_01,
           typename ENTRIES_02,
           typename ENTRIES_03,
           typename ENTRIES_04,
           typename ENTRIES_05,
           typename ENTRIES_06,
           typename ENTRIES_07,
           typename ENTRIES_08,
           typename ENTRIES_09,
           typename ENTRIES_10,
           typename ENTRIES_11,
           typename ENTRIES_12,
           typename ENTRIES_13,
           typename ENTRIES_14,
           typename ENTRIES_15,
           typename ENTRIES_16,
           typename ENTRIES_17,
           typename ENTRIES_18,
           typename ENTRIES_19,
           typename ENTRIES_20,
           typename ENTRIES_21,
           typename ENTRIES_22,
           typename ENTRIES_23,
           typename ENTRIES_24,
           typename ENTRIES_25,
           typename ENTRIES_26,
           typename ENTRIES_27,
           typename ENTRIES_28,
           typename ENTRIES_29,
           typename ENTRIES_30>
    bdld::Datum m(BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_01) entries_01,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_02) entries_02,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_03) entries_03,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_04) entries_04,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_05) entries_05,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_06) entries_06,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_07) entries_07,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_08) entries_08,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_09) entries_09,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_10) entries_10,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_11) entries_11,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_12) entries_12,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_13) entries_13,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_14) entries_14,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_15) entries_15,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_16) entries_16,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_17) entries_17,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_18) entries_18,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_19) entries_19,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_20) entries_20,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_21) entries_21,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_22) entries_22,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_23) entries_23,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_24) entries_24,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_25) entries_25,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_26) entries_26,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_27) entries_27,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_28) entries_28,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_29) entries_29,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_30) entries_30
                  ) const;

    template <typename ENTRIES_01,
           typename ENTRIES_02,
           typename ENTRIES_03,
           typename ENTRIES_04,
           typename ENTRIES_05,
           typename ENTRIES_06,
           typename ENTRIES_07,
           typename ENTRIES_08,
           typename ENTRIES_09,
           typename ENTRIES_10,
           typename ENTRIES_11,
           typename ENTRIES_12,
           typename ENTRIES_13,
           typename ENTRIES_14,
           typename ENTRIES_15,
           typename ENTRIES_16,
           typename ENTRIES_17,
           typename ENTRIES_18,
           typename ENTRIES_19,
           typename ENTRIES_20,
           typename ENTRIES_21,
           typename ENTRIES_22,
           typename ENTRIES_23,
           typename ENTRIES_24,
           typename ENTRIES_25,
           typename ENTRIES_26,
           typename ENTRIES_27,
           typename ENTRIES_28,
           typename ENTRIES_29,
           typename ENTRIES_30,
           typename ENTRIES_31,
           typename ENTRIES_32>
    bdld::Datum m(BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_01) entries_01,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_02) entries_02,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_03) entries_03,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_04) entries_04,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_05) entries_05,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_06) entries_06,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_07) entries_07,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_08) entries_08,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_09) entries_09,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_10) entries_10,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_11) entries_11,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_12) entries_12,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_13) entries_13,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_14) entries_14,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_15) entries_15,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_16) entries_16,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_17) entries_17,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_18) entries_18,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_19) entries_19,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_20) entries_20,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_21) entries_21,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_22) entries_22,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_23) entries_23,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_24) entries_24,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_25) entries_25,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_26) entries_26,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_27) entries_27,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_28) entries_28,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_29) entries_29,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_30) entries_30,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_31) entries_31,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_32) entries_32
                  ) const;

#else
// The generated code below is a workaround for the absence of perfect
// forwarding in some compilers.
    template <typename... ENTRIES>
    bdld::Datum m(BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES)... entries) const;
#endif

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
    template <typename... ENTRIES>
    bdld::Datum mok(ENTRIES&&... entries) const;
        // Return a 'bdld::Datum' object containing a map with owned keys
        // consisting of the specified 'entries'.  The 'entries' are supplied
        // as pairs (odd number of 'sizeof.,..(entries)' being an error) where
        // the first specified element is the key, and the second is its
        // corresponding value.  The behavior is undefined if the same key is
        // supplied more than once.
#elif BSLS_COMPILERFEATURES_SIMULATE_VARIADIC_TEMPLATES
    bdld::Datum mok(
                    ) const;

    template <typename ENTRIES_01,
           typename ENTRIES_02>
    bdld::Datum mok(BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_01) entries_01,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_02) entries_02
                    ) const;

    template <typename ENTRIES_01,
           typename ENTRIES_02,
           typename ENTRIES_03,
           typename ENTRIES_04>
    bdld::Datum mok(BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_01) entries_01,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_02) entries_02,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_03) entries_03,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_04) entries_04
                    ) const;

    template <typename ENTRIES_01,
           typename ENTRIES_02,
           typename ENTRIES_03,
           typename ENTRIES_04,
           typename ENTRIES_05,
           typename ENTRIES_06>
    bdld::Datum mok(BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_01) entries_01,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_02) entries_02,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_03) entries_03,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_04) entries_04,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_05) entries_05,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_06) entries_06
                    ) const;

    template <typename ENTRIES_01,
           typename ENTRIES_02,
           typename ENTRIES_03,
           typename ENTRIES_04,
           typename ENTRIES_05,
           typename ENTRIES_06,
           typename ENTRIES_07,
           typename ENTRIES_08>
    bdld::Datum mok(BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_01) entries_01,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_02) entries_02,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_03) entries_03,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_04) entries_04,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_05) entries_05,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_06) entries_06,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_07) entries_07,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_08) entries_08
                    ) const;

    template <typename ENTRIES_01,
           typename ENTRIES_02,
           typename ENTRIES_03,
           typename ENTRIES_04,
           typename ENTRIES_05,
           typename ENTRIES_06,
           typename ENTRIES_07,
           typename ENTRIES_08,
           typename ENTRIES_09,
           typename ENTRIES_10>
    bdld::Datum mok(BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_01) entries_01,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_02) entries_02,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_03) entries_03,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_04) entries_04,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_05) entries_05,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_06) entries_06,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_07) entries_07,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_08) entries_08,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_09) entries_09,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_10) entries_10
                    ) const;

    template <typename ENTRIES_01,
           typename ENTRIES_02,
           typename ENTRIES_03,
           typename ENTRIES_04,
           typename ENTRIES_05,
           typename ENTRIES_06,
           typename ENTRIES_07,
           typename ENTRIES_08,
           typename ENTRIES_09,
           typename ENTRIES_10,
           typename ENTRIES_11,
           typename ENTRIES_12>
    bdld::Datum mok(BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_01) entries_01,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_02) entries_02,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_03) entries_03,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_04) entries_04,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_05) entries_05,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_06) entries_06,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_07) entries_07,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_08) entries_08,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_09) entries_09,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_10) entries_10,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_11) entries_11,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_12) entries_12
                    ) const;

    template <typename ENTRIES_01,
           typename ENTRIES_02,
           typename ENTRIES_03,
           typename ENTRIES_04,
           typename ENTRIES_05,
           typename ENTRIES_06,
           typename ENTRIES_07,
           typename ENTRIES_08,
           typename ENTRIES_09,
           typename ENTRIES_10,
           typename ENTRIES_11,
           typename ENTRIES_12,
           typename ENTRIES_13,
           typename ENTRIES_14>
    bdld::Datum mok(BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_01) entries_01,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_02) entries_02,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_03) entries_03,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_04) entries_04,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_05) entries_05,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_06) entries_06,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_07) entries_07,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_08) entries_08,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_09) entries_09,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_10) entries_10,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_11) entries_11,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_12) entries_12,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_13) entries_13,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_14) entries_14
                    ) const;

    template <typename ENTRIES_01,
           typename ENTRIES_02,
           typename ENTRIES_03,
           typename ENTRIES_04,
           typename ENTRIES_05,
           typename ENTRIES_06,
           typename ENTRIES_07,
           typename ENTRIES_08,
           typename ENTRIES_09,
           typename ENTRIES_10,
           typename ENTRIES_11,
           typename ENTRIES_12,
           typename ENTRIES_13,
           typename ENTRIES_14,
           typename ENTRIES_15,
           typename ENTRIES_16>
    bdld::Datum mok(BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_01) entries_01,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_02) entries_02,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_03) entries_03,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_04) entries_04,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_05) entries_05,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_06) entries_06,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_07) entries_07,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_08) entries_08,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_09) entries_09,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_10) entries_10,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_11) entries_11,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_12) entries_12,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_13) entries_13,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_14) entries_14,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_15) entries_15,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_16) entries_16
                    ) const;

    template <typename ENTRIES_01,
           typename ENTRIES_02,
           typename ENTRIES_03,
           typename ENTRIES_04,
           typename ENTRIES_05,
           typename ENTRIES_06,
           typename ENTRIES_07,
           typename ENTRIES_08,
           typename ENTRIES_09,
           typename ENTRIES_10,
           typename ENTRIES_11,
           typename ENTRIES_12,
           typename ENTRIES_13,
           typename ENTRIES_14,
           typename ENTRIES_15,
           typename ENTRIES_16,
           typename ENTRIES_17,
           typename ENTRIES_18>
    bdld::Datum mok(BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_01) entries_01,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_02) entries_02,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_03) entries_03,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_04) entries_04,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_05) entries_05,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_06) entries_06,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_07) entries_07,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_08) entries_08,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_09) entries_09,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_10) entries_10,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_11) entries_11,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_12) entries_12,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_13) entries_13,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_14) entries_14,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_15) entries_15,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_16) entries_16,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_17) entries_17,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_18) entries_18
                    ) const;

    template <typename ENTRIES_01,
           typename ENTRIES_02,
           typename ENTRIES_03,
           typename ENTRIES_04,
           typename ENTRIES_05,
           typename ENTRIES_06,
           typename ENTRIES_07,
           typename ENTRIES_08,
           typename ENTRIES_09,
           typename ENTRIES_10,
           typename ENTRIES_11,
           typename ENTRIES_12,
           typename ENTRIES_13,
           typename ENTRIES_14,
           typename ENTRIES_15,
           typename ENTRIES_16,
           typename ENTRIES_17,
           typename ENTRIES_18,
           typename ENTRIES_19,
           typename ENTRIES_20>
    bdld::Datum mok(BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_01) entries_01,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_02) entries_02,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_03) entries_03,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_04) entries_04,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_05) entries_05,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_06) entries_06,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_07) entries_07,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_08) entries_08,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_09) entries_09,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_10) entries_10,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_11) entries_11,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_12) entries_12,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_13) entries_13,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_14) entries_14,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_15) entries_15,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_16) entries_16,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_17) entries_17,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_18) entries_18,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_19) entries_19,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_20) entries_20
                    ) const;

    template <typename ENTRIES_01,
           typename ENTRIES_02,
           typename ENTRIES_03,
           typename ENTRIES_04,
           typename ENTRIES_05,
           typename ENTRIES_06,
           typename ENTRIES_07,
           typename ENTRIES_08,
           typename ENTRIES_09,
           typename ENTRIES_10,
           typename ENTRIES_11,
           typename ENTRIES_12,
           typename ENTRIES_13,
           typename ENTRIES_14,
           typename ENTRIES_15,
           typename ENTRIES_16,
           typename ENTRIES_17,
           typename ENTRIES_18,
           typename ENTRIES_19,
           typename ENTRIES_20,
           typename ENTRIES_21,
           typename ENTRIES_22>
    bdld::Datum mok(BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_01) entries_01,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_02) entries_02,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_03) entries_03,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_04) entries_04,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_05) entries_05,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_06) entries_06,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_07) entries_07,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_08) entries_08,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_09) entries_09,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_10) entries_10,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_11) entries_11,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_12) entries_12,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_13) entries_13,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_14) entries_14,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_15) entries_15,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_16) entries_16,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_17) entries_17,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_18) entries_18,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_19) entries_19,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_20) entries_20,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_21) entries_21,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_22) entries_22
                    ) const;

    template <typename ENTRIES_01,
           typename ENTRIES_02,
           typename ENTRIES_03,
           typename ENTRIES_04,
           typename ENTRIES_05,
           typename ENTRIES_06,
           typename ENTRIES_07,
           typename ENTRIES_08,
           typename ENTRIES_09,
           typename ENTRIES_10,
           typename ENTRIES_11,
           typename ENTRIES_12,
           typename ENTRIES_13,
           typename ENTRIES_14,
           typename ENTRIES_15,
           typename ENTRIES_16,
           typename ENTRIES_17,
           typename ENTRIES_18,
           typename ENTRIES_19,
           typename ENTRIES_20,
           typename ENTRIES_21,
           typename ENTRIES_22,
           typename ENTRIES_23,
           typename ENTRIES_24>
    bdld::Datum mok(BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_01) entries_01,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_02) entries_02,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_03) entries_03,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_04) entries_04,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_05) entries_05,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_06) entries_06,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_07) entries_07,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_08) entries_08,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_09) entries_09,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_10) entries_10,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_11) entries_11,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_12) entries_12,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_13) entries_13,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_14) entries_14,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_15) entries_15,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_16) entries_16,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_17) entries_17,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_18) entries_18,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_19) entries_19,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_20) entries_20,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_21) entries_21,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_22) entries_22,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_23) entries_23,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_24) entries_24
                    ) const;

    template <typename ENTRIES_01,
           typename ENTRIES_02,
           typename ENTRIES_03,
           typename ENTRIES_04,
           typename ENTRIES_05,
           typename ENTRIES_06,
           typename ENTRIES_07,
           typename ENTRIES_08,
           typename ENTRIES_09,
           typename ENTRIES_10,
           typename ENTRIES_11,
           typename ENTRIES_12,
           typename ENTRIES_13,
           typename ENTRIES_14,
           typename ENTRIES_15,
           typename ENTRIES_16,
           typename ENTRIES_17,
           typename ENTRIES_18,
           typename ENTRIES_19,
           typename ENTRIES_20,
           typename ENTRIES_21,
           typename ENTRIES_22,
           typename ENTRIES_23,
           typename ENTRIES_24,
           typename ENTRIES_25,
           typename ENTRIES_26>
    bdld::Datum mok(BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_01) entries_01,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_02) entries_02,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_03) entries_03,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_04) entries_04,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_05) entries_05,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_06) entries_06,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_07) entries_07,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_08) entries_08,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_09) entries_09,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_10) entries_10,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_11) entries_11,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_12) entries_12,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_13) entries_13,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_14) entries_14,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_15) entries_15,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_16) entries_16,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_17) entries_17,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_18) entries_18,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_19) entries_19,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_20) entries_20,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_21) entries_21,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_22) entries_22,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_23) entries_23,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_24) entries_24,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_25) entries_25,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_26) entries_26
                    ) const;

    template <typename ENTRIES_01,
           typename ENTRIES_02,
           typename ENTRIES_03,
           typename ENTRIES_04,
           typename ENTRIES_05,
           typename ENTRIES_06,
           typename ENTRIES_07,
           typename ENTRIES_08,
           typename ENTRIES_09,
           typename ENTRIES_10,
           typename ENTRIES_11,
           typename ENTRIES_12,
           typename ENTRIES_13,
           typename ENTRIES_14,
           typename ENTRIES_15,
           typename ENTRIES_16,
           typename ENTRIES_17,
           typename ENTRIES_18,
           typename ENTRIES_19,
           typename ENTRIES_20,
           typename ENTRIES_21,
           typename ENTRIES_22,
           typename ENTRIES_23,
           typename ENTRIES_24,
           typename ENTRIES_25,
           typename ENTRIES_26,
           typename ENTRIES_27,
           typename ENTRIES_28>
    bdld::Datum mok(BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_01) entries_01,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_02) entries_02,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_03) entries_03,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_04) entries_04,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_05) entries_05,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_06) entries_06,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_07) entries_07,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_08) entries_08,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_09) entries_09,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_10) entries_10,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_11) entries_11,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_12) entries_12,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_13) entries_13,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_14) entries_14,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_15) entries_15,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_16) entries_16,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_17) entries_17,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_18) entries_18,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_19) entries_19,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_20) entries_20,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_21) entries_21,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_22) entries_22,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_23) entries_23,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_24) entries_24,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_25) entries_25,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_26) entries_26,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_27) entries_27,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_28) entries_28
                    ) const;

    template <typename ENTRIES_01,
           typename ENTRIES_02,
           typename ENTRIES_03,
           typename ENTRIES_04,
           typename ENTRIES_05,
           typename ENTRIES_06,
           typename ENTRIES_07,
           typename ENTRIES_08,
           typename ENTRIES_09,
           typename ENTRIES_10,
           typename ENTRIES_11,
           typename ENTRIES_12,
           typename ENTRIES_13,
           typename ENTRIES_14,
           typename ENTRIES_15,
           typename ENTRIES_16,
           typename ENTRIES_17,
           typename ENTRIES_18,
           typename ENTRIES_19,
           typename ENTRIES_20,
           typename ENTRIES_21,
           typename ENTRIES_22,
           typename ENTRIES_23,
           typename ENTRIES_24,
           typename ENTRIES_25,
           typename ENTRIES_26,
           typename ENTRIES_27,
           typename ENTRIES_28,
           typename ENTRIES_29,
           typename ENTRIES_30>
    bdld::Datum mok(BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_01) entries_01,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_02) entries_02,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_03) entries_03,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_04) entries_04,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_05) entries_05,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_06) entries_06,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_07) entries_07,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_08) entries_08,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_09) entries_09,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_10) entries_10,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_11) entries_11,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_12) entries_12,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_13) entries_13,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_14) entries_14,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_15) entries_15,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_16) entries_16,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_17) entries_17,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_18) entries_18,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_19) entries_19,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_20) entries_20,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_21) entries_21,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_22) entries_22,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_23) entries_23,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_24) entries_24,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_25) entries_25,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_26) entries_26,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_27) entries_27,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_28) entries_28,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_29) entries_29,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_30) entries_30
                    ) const;

    template <typename ENTRIES_01,
           typename ENTRIES_02,
           typename ENTRIES_03,
           typename ENTRIES_04,
           typename ENTRIES_05,
           typename ENTRIES_06,
           typename ENTRIES_07,
           typename ENTRIES_08,
           typename ENTRIES_09,
           typename ENTRIES_10,
           typename ENTRIES_11,
           typename ENTRIES_12,
           typename ENTRIES_13,
           typename ENTRIES_14,
           typename ENTRIES_15,
           typename ENTRIES_16,
           typename ENTRIES_17,
           typename ENTRIES_18,
           typename ENTRIES_19,
           typename ENTRIES_20,
           typename ENTRIES_21,
           typename ENTRIES_22,
           typename ENTRIES_23,
           typename ENTRIES_24,
           typename ENTRIES_25,
           typename ENTRIES_26,
           typename ENTRIES_27,
           typename ENTRIES_28,
           typename ENTRIES_29,
           typename ENTRIES_30,
           typename ENTRIES_31,
           typename ENTRIES_32>
    bdld::Datum mok(BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_01) entries_01,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_02) entries_02,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_03) entries_03,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_04) entries_04,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_05) entries_05,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_06) entries_06,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_07) entries_07,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_08) entries_08,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_09) entries_09,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_10) entries_10,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_11) entries_11,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_12) entries_12,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_13) entries_13,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_14) entries_14,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_15) entries_15,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_16) entries_16,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_17) entries_17,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_18) entries_18,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_19) entries_19,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_20) entries_20,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_21) entries_21,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_22) entries_22,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_23) entries_23,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_24) entries_24,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_25) entries_25,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_26) entries_26,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_27) entries_27,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_28) entries_28,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_29) entries_29,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_30) entries_30,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_31) entries_31,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_32) entries_32
                    ) const;

#else
// The generated code below is a workaround for the absence of perfect
// forwarding in some compilers.
    template <typename... ENTRIES>
    bdld::Datum mok(BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES)... entries
                    ) const;
#endif
};

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

                              // ----------------
                              // class DatumMaker
                              // ----------------

// PRIVATE ACCESSORS
inline
void DatumMaker::pushBackHelper(bdld::DatumArrayBuilder *) const
{
}

inline
void DatumMaker::pushBackHelper(bdld::DatumMapBuilder *) const
{
}

inline
void DatumMaker::pushBackHelper(bdld::DatumMapOwningKeysBuilder *) const
{
}

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
template <typename TYPE>
inline
void DatumMaker::pushBackHelper(bdld::DatumArrayBuilder *builder,
                                TYPE&&                   element) const
{
    builder->pushBack((*this)(element));
}

template <typename TYPE, typename... ELEMENTS>
inline
void DatumMaker::pushBackHelper(bdld::DatumArrayBuilder *builder,
                                TYPE&&                   element,
                                ELEMENTS&&...            elements) const
{
    builder->pushBack((*this)(element));
    pushBackHelper(builder, elements...);
}

template <typename TYPE>
inline
void DatumMaker::pushBackHelper(bdld::DatumMapBuilder    *builder,
                                const bslstl::StringRef&  key,
                                TYPE&&                    value) const
{
    builder->pushBack(key, (*this)(value));
}

template <typename TYPE, typename... ENTRIES>
inline
void DatumMaker::pushBackHelper(bdld::DatumMapBuilder    *builder,
                                const bslstl::StringRef&  key,
                                TYPE&&                    value,
                                ENTRIES&&...              entries) const
{
    builder->pushBack(key, (*this)(value));
    pushBackHelper(builder, entries...);
}

template <typename TYPE>
inline
void DatumMaker::pushBackHelper(bdld::DatumMapOwningKeysBuilder *builder,
                                const bslstl::StringRef&         key,
                                TYPE&&                           value) const
{
    builder->pushBack(key, (*this)(value));
}

template <typename TYPE, typename... ENTRIES>
inline
void DatumMaker::pushBackHelper(
                               bdld::DatumMapOwningKeysBuilder *builder,
                               const bslstl::StringRef&         key,
                               TYPE&&                           value,
                               ENTRIES&&...                     entries) const
{
    builder->pushBack(key, (*this)(value));
    pushBackHelper(builder, entries...);
}
#elif BSLS_COMPILERFEATURES_SIMULATE_VARIADIC_TEMPLATES
template <typename TYPE>
inline
void DatumMaker::pushBackHelper(bdld::DatumArrayBuilder *builder,
                               BSLS_COMPILERFEATURES_FORWARD_REF(TYPE) element
                        ) const
{
    builder->pushBack((*this)(element));
    pushBackHelper(builder);
}

template <typename TYPE, typename ELEMENTS_01>
inline
void DatumMaker::pushBackHelper(bdld::DatumArrayBuilder *builder,
                               BSLS_COMPILERFEATURES_FORWARD_REF(TYPE) element,
                     BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_01) elements_01
                        ) const
{
    builder->pushBack((*this)(element));
    pushBackHelper(builder, elements_01);
}

template <typename TYPE, typename ELEMENTS_01,
                         typename ELEMENTS_02>
inline
void DatumMaker::pushBackHelper(bdld::DatumArrayBuilder *builder,
                               BSLS_COMPILERFEATURES_FORWARD_REF(TYPE) element,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_01) elements_01,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_02) elements_02
                        ) const
{
    builder->pushBack((*this)(element));
    pushBackHelper(builder, elements_01,
                            elements_02);
}

template <typename TYPE, typename ELEMENTS_01,
                         typename ELEMENTS_02,
                         typename ELEMENTS_03>
inline
void DatumMaker::pushBackHelper(bdld::DatumArrayBuilder *builder,
                               BSLS_COMPILERFEATURES_FORWARD_REF(TYPE) element,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_01) elements_01,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_02) elements_02,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_03) elements_03
                        ) const
{
    builder->pushBack((*this)(element));
    pushBackHelper(builder, elements_01,
                            elements_02,
                            elements_03);
}

template <typename TYPE, typename ELEMENTS_01,
                         typename ELEMENTS_02,
                         typename ELEMENTS_03,
                         typename ELEMENTS_04>
inline
void DatumMaker::pushBackHelper(bdld::DatumArrayBuilder *builder,
                               BSLS_COMPILERFEATURES_FORWARD_REF(TYPE) element,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_01) elements_01,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_02) elements_02,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_03) elements_03,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_04) elements_04
                        ) const
{
    builder->pushBack((*this)(element));
    pushBackHelper(builder, elements_01,
                            elements_02,
                            elements_03,
                            elements_04);
}

template <typename TYPE, typename ELEMENTS_01,
                         typename ELEMENTS_02,
                         typename ELEMENTS_03,
                         typename ELEMENTS_04,
                         typename ELEMENTS_05>
inline
void DatumMaker::pushBackHelper(bdld::DatumArrayBuilder *builder,
                               BSLS_COMPILERFEATURES_FORWARD_REF(TYPE) element,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_01) elements_01,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_02) elements_02,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_03) elements_03,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_04) elements_04,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_05) elements_05
                        ) const
{
    builder->pushBack((*this)(element));
    pushBackHelper(builder, elements_01,
                            elements_02,
                            elements_03,
                            elements_04,
                            elements_05);
}

template <typename TYPE, typename ELEMENTS_01,
                         typename ELEMENTS_02,
                         typename ELEMENTS_03,
                         typename ELEMENTS_04,
                         typename ELEMENTS_05,
                         typename ELEMENTS_06>
inline
void DatumMaker::pushBackHelper(bdld::DatumArrayBuilder *builder,
                               BSLS_COMPILERFEATURES_FORWARD_REF(TYPE) element,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_01) elements_01,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_02) elements_02,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_03) elements_03,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_04) elements_04,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_05) elements_05,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_06) elements_06
                        ) const
{
    builder->pushBack((*this)(element));
    pushBackHelper(builder, elements_01,
                            elements_02,
                            elements_03,
                            elements_04,
                            elements_05,
                            elements_06);
}

template <typename TYPE, typename ELEMENTS_01,
                         typename ELEMENTS_02,
                         typename ELEMENTS_03,
                         typename ELEMENTS_04,
                         typename ELEMENTS_05,
                         typename ELEMENTS_06,
                         typename ELEMENTS_07>
inline
void DatumMaker::pushBackHelper(bdld::DatumArrayBuilder *builder,
                               BSLS_COMPILERFEATURES_FORWARD_REF(TYPE) element,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_01) elements_01,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_02) elements_02,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_03) elements_03,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_04) elements_04,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_05) elements_05,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_06) elements_06,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_07) elements_07
                        ) const
{
    builder->pushBack((*this)(element));
    pushBackHelper(builder, elements_01,
                            elements_02,
                            elements_03,
                            elements_04,
                            elements_05,
                            elements_06,
                            elements_07);
}

template <typename TYPE, typename ELEMENTS_01,
                         typename ELEMENTS_02,
                         typename ELEMENTS_03,
                         typename ELEMENTS_04,
                         typename ELEMENTS_05,
                         typename ELEMENTS_06,
                         typename ELEMENTS_07,
                         typename ELEMENTS_08>
inline
void DatumMaker::pushBackHelper(bdld::DatumArrayBuilder *builder,
                               BSLS_COMPILERFEATURES_FORWARD_REF(TYPE) element,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_01) elements_01,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_02) elements_02,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_03) elements_03,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_04) elements_04,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_05) elements_05,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_06) elements_06,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_07) elements_07,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_08) elements_08
                        ) const
{
    builder->pushBack((*this)(element));
    pushBackHelper(builder, elements_01,
                            elements_02,
                            elements_03,
                            elements_04,
                            elements_05,
                            elements_06,
                            elements_07,
                            elements_08);
}

template <typename TYPE, typename ELEMENTS_01,
                         typename ELEMENTS_02,
                         typename ELEMENTS_03,
                         typename ELEMENTS_04,
                         typename ELEMENTS_05,
                         typename ELEMENTS_06,
                         typename ELEMENTS_07,
                         typename ELEMENTS_08,
                         typename ELEMENTS_09>
inline
void DatumMaker::pushBackHelper(bdld::DatumArrayBuilder *builder,
                               BSLS_COMPILERFEATURES_FORWARD_REF(TYPE) element,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_01) elements_01,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_02) elements_02,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_03) elements_03,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_04) elements_04,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_05) elements_05,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_06) elements_06,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_07) elements_07,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_08) elements_08,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_09) elements_09
                        ) const
{
    builder->pushBack((*this)(element));
    pushBackHelper(builder, elements_01,
                            elements_02,
                            elements_03,
                            elements_04,
                            elements_05,
                            elements_06,
                            elements_07,
                            elements_08,
                            elements_09);
}

template <typename TYPE, typename ELEMENTS_01,
                         typename ELEMENTS_02,
                         typename ELEMENTS_03,
                         typename ELEMENTS_04,
                         typename ELEMENTS_05,
                         typename ELEMENTS_06,
                         typename ELEMENTS_07,
                         typename ELEMENTS_08,
                         typename ELEMENTS_09,
                         typename ELEMENTS_10>
inline
void DatumMaker::pushBackHelper(bdld::DatumArrayBuilder *builder,
                               BSLS_COMPILERFEATURES_FORWARD_REF(TYPE) element,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_01) elements_01,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_02) elements_02,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_03) elements_03,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_04) elements_04,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_05) elements_05,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_06) elements_06,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_07) elements_07,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_08) elements_08,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_09) elements_09,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_10) elements_10
                        ) const
{
    builder->pushBack((*this)(element));
    pushBackHelper(builder, elements_01,
                            elements_02,
                            elements_03,
                            elements_04,
                            elements_05,
                            elements_06,
                            elements_07,
                            elements_08,
                            elements_09,
                            elements_10);
}

template <typename TYPE, typename ELEMENTS_01,
                         typename ELEMENTS_02,
                         typename ELEMENTS_03,
                         typename ELEMENTS_04,
                         typename ELEMENTS_05,
                         typename ELEMENTS_06,
                         typename ELEMENTS_07,
                         typename ELEMENTS_08,
                         typename ELEMENTS_09,
                         typename ELEMENTS_10,
                         typename ELEMENTS_11>
inline
void DatumMaker::pushBackHelper(bdld::DatumArrayBuilder *builder,
                               BSLS_COMPILERFEATURES_FORWARD_REF(TYPE) element,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_01) elements_01,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_02) elements_02,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_03) elements_03,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_04) elements_04,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_05) elements_05,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_06) elements_06,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_07) elements_07,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_08) elements_08,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_09) elements_09,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_10) elements_10,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_11) elements_11
                        ) const
{
    builder->pushBack((*this)(element));
    pushBackHelper(builder, elements_01,
                            elements_02,
                            elements_03,
                            elements_04,
                            elements_05,
                            elements_06,
                            elements_07,
                            elements_08,
                            elements_09,
                            elements_10,
                            elements_11);
}

template <typename TYPE, typename ELEMENTS_01,
                         typename ELEMENTS_02,
                         typename ELEMENTS_03,
                         typename ELEMENTS_04,
                         typename ELEMENTS_05,
                         typename ELEMENTS_06,
                         typename ELEMENTS_07,
                         typename ELEMENTS_08,
                         typename ELEMENTS_09,
                         typename ELEMENTS_10,
                         typename ELEMENTS_11,
                         typename ELEMENTS_12>
inline
void DatumMaker::pushBackHelper(bdld::DatumArrayBuilder *builder,
                               BSLS_COMPILERFEATURES_FORWARD_REF(TYPE) element,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_01) elements_01,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_02) elements_02,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_03) elements_03,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_04) elements_04,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_05) elements_05,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_06) elements_06,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_07) elements_07,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_08) elements_08,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_09) elements_09,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_10) elements_10,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_11) elements_11,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_12) elements_12
                        ) const
{
    builder->pushBack((*this)(element));
    pushBackHelper(builder, elements_01,
                            elements_02,
                            elements_03,
                            elements_04,
                            elements_05,
                            elements_06,
                            elements_07,
                            elements_08,
                            elements_09,
                            elements_10,
                            elements_11,
                            elements_12);
}

template <typename TYPE, typename ELEMENTS_01,
                         typename ELEMENTS_02,
                         typename ELEMENTS_03,
                         typename ELEMENTS_04,
                         typename ELEMENTS_05,
                         typename ELEMENTS_06,
                         typename ELEMENTS_07,
                         typename ELEMENTS_08,
                         typename ELEMENTS_09,
                         typename ELEMENTS_10,
                         typename ELEMENTS_11,
                         typename ELEMENTS_12,
                         typename ELEMENTS_13>
inline
void DatumMaker::pushBackHelper(bdld::DatumArrayBuilder *builder,
                               BSLS_COMPILERFEATURES_FORWARD_REF(TYPE) element,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_01) elements_01,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_02) elements_02,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_03) elements_03,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_04) elements_04,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_05) elements_05,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_06) elements_06,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_07) elements_07,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_08) elements_08,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_09) elements_09,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_10) elements_10,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_11) elements_11,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_12) elements_12,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_13) elements_13
                        ) const
{
    builder->pushBack((*this)(element));
    pushBackHelper(builder, elements_01,
                            elements_02,
                            elements_03,
                            elements_04,
                            elements_05,
                            elements_06,
                            elements_07,
                            elements_08,
                            elements_09,
                            elements_10,
                            elements_11,
                            elements_12,
                            elements_13);
}

template <typename TYPE, typename ELEMENTS_01,
                         typename ELEMENTS_02,
                         typename ELEMENTS_03,
                         typename ELEMENTS_04,
                         typename ELEMENTS_05,
                         typename ELEMENTS_06,
                         typename ELEMENTS_07,
                         typename ELEMENTS_08,
                         typename ELEMENTS_09,
                         typename ELEMENTS_10,
                         typename ELEMENTS_11,
                         typename ELEMENTS_12,
                         typename ELEMENTS_13,
                         typename ELEMENTS_14>
inline
void DatumMaker::pushBackHelper(bdld::DatumArrayBuilder *builder,
                               BSLS_COMPILERFEATURES_FORWARD_REF(TYPE) element,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_01) elements_01,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_02) elements_02,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_03) elements_03,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_04) elements_04,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_05) elements_05,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_06) elements_06,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_07) elements_07,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_08) elements_08,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_09) elements_09,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_10) elements_10,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_11) elements_11,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_12) elements_12,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_13) elements_13,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_14) elements_14
                        ) const
{
    builder->pushBack((*this)(element));
    pushBackHelper(builder, elements_01,
                            elements_02,
                            elements_03,
                            elements_04,
                            elements_05,
                            elements_06,
                            elements_07,
                            elements_08,
                            elements_09,
                            elements_10,
                            elements_11,
                            elements_12,
                            elements_13,
                            elements_14);
}

template <typename TYPE, typename ELEMENTS_01,
                         typename ELEMENTS_02,
                         typename ELEMENTS_03,
                         typename ELEMENTS_04,
                         typename ELEMENTS_05,
                         typename ELEMENTS_06,
                         typename ELEMENTS_07,
                         typename ELEMENTS_08,
                         typename ELEMENTS_09,
                         typename ELEMENTS_10,
                         typename ELEMENTS_11,
                         typename ELEMENTS_12,
                         typename ELEMENTS_13,
                         typename ELEMENTS_14,
                         typename ELEMENTS_15>
inline
void DatumMaker::pushBackHelper(bdld::DatumArrayBuilder *builder,
                               BSLS_COMPILERFEATURES_FORWARD_REF(TYPE) element,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_01) elements_01,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_02) elements_02,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_03) elements_03,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_04) elements_04,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_05) elements_05,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_06) elements_06,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_07) elements_07,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_08) elements_08,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_09) elements_09,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_10) elements_10,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_11) elements_11,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_12) elements_12,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_13) elements_13,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_14) elements_14,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_15) elements_15
                        ) const
{
    builder->pushBack((*this)(element));
    pushBackHelper(builder, elements_01,
                            elements_02,
                            elements_03,
                            elements_04,
                            elements_05,
                            elements_06,
                            elements_07,
                            elements_08,
                            elements_09,
                            elements_10,
                            elements_11,
                            elements_12,
                            elements_13,
                            elements_14,
                            elements_15);
}

template <typename TYPE, typename ELEMENTS_01,
                         typename ELEMENTS_02,
                         typename ELEMENTS_03,
                         typename ELEMENTS_04,
                         typename ELEMENTS_05,
                         typename ELEMENTS_06,
                         typename ELEMENTS_07,
                         typename ELEMENTS_08,
                         typename ELEMENTS_09,
                         typename ELEMENTS_10,
                         typename ELEMENTS_11,
                         typename ELEMENTS_12,
                         typename ELEMENTS_13,
                         typename ELEMENTS_14,
                         typename ELEMENTS_15,
                         typename ELEMENTS_16>
inline
void DatumMaker::pushBackHelper(bdld::DatumArrayBuilder *builder,
                               BSLS_COMPILERFEATURES_FORWARD_REF(TYPE) element,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_01) elements_01,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_02) elements_02,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_03) elements_03,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_04) elements_04,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_05) elements_05,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_06) elements_06,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_07) elements_07,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_08) elements_08,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_09) elements_09,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_10) elements_10,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_11) elements_11,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_12) elements_12,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_13) elements_13,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_14) elements_14,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_15) elements_15,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_16) elements_16
                        ) const
{
    builder->pushBack((*this)(element));
    pushBackHelper(builder, elements_01,
                            elements_02,
                            elements_03,
                            elements_04,
                            elements_05,
                            elements_06,
                            elements_07,
                            elements_08,
                            elements_09,
                            elements_10,
                            elements_11,
                            elements_12,
                            elements_13,
                            elements_14,
                            elements_15,
                            elements_16);
}

template <typename TYPE>
inline
void DatumMaker::pushBackHelper(bdld::DatumMapBuilder                 *builder,
                                const bslstl::StringRef&               key,
                                BSLS_COMPILERFEATURES_FORWARD_REF(TYPE) value
                          ) const
{
    builder->pushBack(key, (*this)(value));
    pushBackHelper(builder);
}

template <typename TYPE, typename ENTRIES_01,
                         typename ENTRIES_02>
inline
void DatumMaker::pushBackHelper(bdld::DatumMapBuilder                 *builder,
                                const bslstl::StringRef&               key,
                                BSLS_COMPILERFEATURES_FORWARD_REF(TYPE) value,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_01) entries_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_02) entries_02
                          ) const
{
    builder->pushBack(key, (*this)(value));
    pushBackHelper(builder, entries_01,
                            entries_02);
}

template <typename TYPE, typename ENTRIES_01,
                         typename ENTRIES_02,
                         typename ENTRIES_03,
                         typename ENTRIES_04>
inline
void DatumMaker::pushBackHelper(bdld::DatumMapBuilder                 *builder,
                                const bslstl::StringRef&               key,
                                BSLS_COMPILERFEATURES_FORWARD_REF(TYPE) value,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_01) entries_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_02) entries_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_03) entries_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_04) entries_04
                          ) const
{
    builder->pushBack(key, (*this)(value));
    pushBackHelper(builder, entries_01,
                            entries_02,
                            entries_03,
                            entries_04);
}

template <typename TYPE, typename ENTRIES_01,
                         typename ENTRIES_02,
                         typename ENTRIES_03,
                         typename ENTRIES_04,
                         typename ENTRIES_05,
                         typename ENTRIES_06>
inline
void DatumMaker::pushBackHelper(bdld::DatumMapBuilder                 *builder,
                                const bslstl::StringRef&               key,
                                BSLS_COMPILERFEATURES_FORWARD_REF(TYPE) value,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_01) entries_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_02) entries_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_03) entries_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_04) entries_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_05) entries_05,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_06) entries_06
                          ) const
{
    builder->pushBack(key, (*this)(value));
    pushBackHelper(builder, entries_01,
                            entries_02,
                            entries_03,
                            entries_04,
                            entries_05,
                            entries_06);
}

template <typename TYPE, typename ENTRIES_01,
                         typename ENTRIES_02,
                         typename ENTRIES_03,
                         typename ENTRIES_04,
                         typename ENTRIES_05,
                         typename ENTRIES_06,
                         typename ENTRIES_07,
                         typename ENTRIES_08>
inline
void DatumMaker::pushBackHelper(bdld::DatumMapBuilder                 *builder,
                                const bslstl::StringRef&               key,
                                BSLS_COMPILERFEATURES_FORWARD_REF(TYPE) value,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_01) entries_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_02) entries_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_03) entries_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_04) entries_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_05) entries_05,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_06) entries_06,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_07) entries_07,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_08) entries_08
                          ) const
{
    builder->pushBack(key, (*this)(value));
    pushBackHelper(builder, entries_01,
                            entries_02,
                            entries_03,
                            entries_04,
                            entries_05,
                            entries_06,
                            entries_07,
                            entries_08);
}

template <typename TYPE, typename ENTRIES_01,
                         typename ENTRIES_02,
                         typename ENTRIES_03,
                         typename ENTRIES_04,
                         typename ENTRIES_05,
                         typename ENTRIES_06,
                         typename ENTRIES_07,
                         typename ENTRIES_08,
                         typename ENTRIES_09,
                         typename ENTRIES_10>
inline
void DatumMaker::pushBackHelper(bdld::DatumMapBuilder                 *builder,
                                const bslstl::StringRef&               key,
                                BSLS_COMPILERFEATURES_FORWARD_REF(TYPE) value,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_01) entries_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_02) entries_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_03) entries_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_04) entries_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_05) entries_05,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_06) entries_06,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_07) entries_07,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_08) entries_08,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_09) entries_09,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_10) entries_10
                          ) const
{
    builder->pushBack(key, (*this)(value));
    pushBackHelper(builder, entries_01,
                            entries_02,
                            entries_03,
                            entries_04,
                            entries_05,
                            entries_06,
                            entries_07,
                            entries_08,
                            entries_09,
                            entries_10);
}

template <typename TYPE, typename ENTRIES_01,
                         typename ENTRIES_02,
                         typename ENTRIES_03,
                         typename ENTRIES_04,
                         typename ENTRIES_05,
                         typename ENTRIES_06,
                         typename ENTRIES_07,
                         typename ENTRIES_08,
                         typename ENTRIES_09,
                         typename ENTRIES_10,
                         typename ENTRIES_11,
                         typename ENTRIES_12>
inline
void DatumMaker::pushBackHelper(bdld::DatumMapBuilder                 *builder,
                                const bslstl::StringRef&               key,
                                BSLS_COMPILERFEATURES_FORWARD_REF(TYPE) value,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_01) entries_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_02) entries_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_03) entries_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_04) entries_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_05) entries_05,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_06) entries_06,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_07) entries_07,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_08) entries_08,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_09) entries_09,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_10) entries_10,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_11) entries_11,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_12) entries_12
                          ) const
{
    builder->pushBack(key, (*this)(value));
    pushBackHelper(builder, entries_01,
                            entries_02,
                            entries_03,
                            entries_04,
                            entries_05,
                            entries_06,
                            entries_07,
                            entries_08,
                            entries_09,
                            entries_10,
                            entries_11,
                            entries_12);
}

template <typename TYPE, typename ENTRIES_01,
                         typename ENTRIES_02,
                         typename ENTRIES_03,
                         typename ENTRIES_04,
                         typename ENTRIES_05,
                         typename ENTRIES_06,
                         typename ENTRIES_07,
                         typename ENTRIES_08,
                         typename ENTRIES_09,
                         typename ENTRIES_10,
                         typename ENTRIES_11,
                         typename ENTRIES_12,
                         typename ENTRIES_13,
                         typename ENTRIES_14>
inline
void DatumMaker::pushBackHelper(bdld::DatumMapBuilder                 *builder,
                                const bslstl::StringRef&               key,
                                BSLS_COMPILERFEATURES_FORWARD_REF(TYPE) value,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_01) entries_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_02) entries_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_03) entries_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_04) entries_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_05) entries_05,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_06) entries_06,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_07) entries_07,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_08) entries_08,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_09) entries_09,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_10) entries_10,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_11) entries_11,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_12) entries_12,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_13) entries_13,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_14) entries_14
                          ) const
{
    builder->pushBack(key, (*this)(value));
    pushBackHelper(builder, entries_01,
                            entries_02,
                            entries_03,
                            entries_04,
                            entries_05,
                            entries_06,
                            entries_07,
                            entries_08,
                            entries_09,
                            entries_10,
                            entries_11,
                            entries_12,
                            entries_13,
                            entries_14);
}

template <typename TYPE, typename ENTRIES_01,
                         typename ENTRIES_02,
                         typename ENTRIES_03,
                         typename ENTRIES_04,
                         typename ENTRIES_05,
                         typename ENTRIES_06,
                         typename ENTRIES_07,
                         typename ENTRIES_08,
                         typename ENTRIES_09,
                         typename ENTRIES_10,
                         typename ENTRIES_11,
                         typename ENTRIES_12,
                         typename ENTRIES_13,
                         typename ENTRIES_14,
                         typename ENTRIES_15,
                         typename ENTRIES_16>
inline
void DatumMaker::pushBackHelper(bdld::DatumMapBuilder                 *builder,
                                const bslstl::StringRef&               key,
                                BSLS_COMPILERFEATURES_FORWARD_REF(TYPE) value,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_01) entries_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_02) entries_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_03) entries_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_04) entries_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_05) entries_05,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_06) entries_06,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_07) entries_07,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_08) entries_08,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_09) entries_09,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_10) entries_10,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_11) entries_11,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_12) entries_12,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_13) entries_13,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_14) entries_14,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_15) entries_15,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_16) entries_16
                          ) const
{
    builder->pushBack(key, (*this)(value));
    pushBackHelper(builder, entries_01,
                            entries_02,
                            entries_03,
                            entries_04,
                            entries_05,
                            entries_06,
                            entries_07,
                            entries_08,
                            entries_09,
                            entries_10,
                            entries_11,
                            entries_12,
                            entries_13,
                            entries_14,
                            entries_15,
                            entries_16);
}

template <typename TYPE, typename ENTRIES_01,
                         typename ENTRIES_02,
                         typename ENTRIES_03,
                         typename ENTRIES_04,
                         typename ENTRIES_05,
                         typename ENTRIES_06,
                         typename ENTRIES_07,
                         typename ENTRIES_08,
                         typename ENTRIES_09,
                         typename ENTRIES_10,
                         typename ENTRIES_11,
                         typename ENTRIES_12,
                         typename ENTRIES_13,
                         typename ENTRIES_14,
                         typename ENTRIES_15,
                         typename ENTRIES_16,
                         typename ENTRIES_17,
                         typename ENTRIES_18>
inline
void DatumMaker::pushBackHelper(bdld::DatumMapBuilder                 *builder,
                                const bslstl::StringRef&               key,
                                BSLS_COMPILERFEATURES_FORWARD_REF(TYPE) value,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_01) entries_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_02) entries_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_03) entries_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_04) entries_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_05) entries_05,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_06) entries_06,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_07) entries_07,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_08) entries_08,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_09) entries_09,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_10) entries_10,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_11) entries_11,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_12) entries_12,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_13) entries_13,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_14) entries_14,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_15) entries_15,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_16) entries_16,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_17) entries_17,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_18) entries_18
                          ) const
{
    builder->pushBack(key, (*this)(value));
    pushBackHelper(builder, entries_01,
                            entries_02,
                            entries_03,
                            entries_04,
                            entries_05,
                            entries_06,
                            entries_07,
                            entries_08,
                            entries_09,
                            entries_10,
                            entries_11,
                            entries_12,
                            entries_13,
                            entries_14,
                            entries_15,
                            entries_16,
                            entries_17,
                            entries_18);
}

template <typename TYPE, typename ENTRIES_01,
                         typename ENTRIES_02,
                         typename ENTRIES_03,
                         typename ENTRIES_04,
                         typename ENTRIES_05,
                         typename ENTRIES_06,
                         typename ENTRIES_07,
                         typename ENTRIES_08,
                         typename ENTRIES_09,
                         typename ENTRIES_10,
                         typename ENTRIES_11,
                         typename ENTRIES_12,
                         typename ENTRIES_13,
                         typename ENTRIES_14,
                         typename ENTRIES_15,
                         typename ENTRIES_16,
                         typename ENTRIES_17,
                         typename ENTRIES_18,
                         typename ENTRIES_19,
                         typename ENTRIES_20>
inline
void DatumMaker::pushBackHelper(bdld::DatumMapBuilder                 *builder,
                                const bslstl::StringRef&               key,
                                BSLS_COMPILERFEATURES_FORWARD_REF(TYPE) value,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_01) entries_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_02) entries_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_03) entries_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_04) entries_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_05) entries_05,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_06) entries_06,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_07) entries_07,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_08) entries_08,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_09) entries_09,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_10) entries_10,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_11) entries_11,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_12) entries_12,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_13) entries_13,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_14) entries_14,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_15) entries_15,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_16) entries_16,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_17) entries_17,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_18) entries_18,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_19) entries_19,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_20) entries_20
                          ) const
{
    builder->pushBack(key, (*this)(value));
    pushBackHelper(builder, entries_01,
                            entries_02,
                            entries_03,
                            entries_04,
                            entries_05,
                            entries_06,
                            entries_07,
                            entries_08,
                            entries_09,
                            entries_10,
                            entries_11,
                            entries_12,
                            entries_13,
                            entries_14,
                            entries_15,
                            entries_16,
                            entries_17,
                            entries_18,
                            entries_19,
                            entries_20);
}

template <typename TYPE, typename ENTRIES_01,
                         typename ENTRIES_02,
                         typename ENTRIES_03,
                         typename ENTRIES_04,
                         typename ENTRIES_05,
                         typename ENTRIES_06,
                         typename ENTRIES_07,
                         typename ENTRIES_08,
                         typename ENTRIES_09,
                         typename ENTRIES_10,
                         typename ENTRIES_11,
                         typename ENTRIES_12,
                         typename ENTRIES_13,
                         typename ENTRIES_14,
                         typename ENTRIES_15,
                         typename ENTRIES_16,
                         typename ENTRIES_17,
                         typename ENTRIES_18,
                         typename ENTRIES_19,
                         typename ENTRIES_20,
                         typename ENTRIES_21,
                         typename ENTRIES_22>
inline
void DatumMaker::pushBackHelper(bdld::DatumMapBuilder                 *builder,
                                const bslstl::StringRef&               key,
                                BSLS_COMPILERFEATURES_FORWARD_REF(TYPE) value,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_01) entries_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_02) entries_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_03) entries_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_04) entries_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_05) entries_05,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_06) entries_06,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_07) entries_07,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_08) entries_08,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_09) entries_09,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_10) entries_10,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_11) entries_11,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_12) entries_12,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_13) entries_13,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_14) entries_14,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_15) entries_15,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_16) entries_16,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_17) entries_17,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_18) entries_18,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_19) entries_19,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_20) entries_20,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_21) entries_21,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_22) entries_22
                          ) const
{
    builder->pushBack(key, (*this)(value));
    pushBackHelper(builder, entries_01,
                            entries_02,
                            entries_03,
                            entries_04,
                            entries_05,
                            entries_06,
                            entries_07,
                            entries_08,
                            entries_09,
                            entries_10,
                            entries_11,
                            entries_12,
                            entries_13,
                            entries_14,
                            entries_15,
                            entries_16,
                            entries_17,
                            entries_18,
                            entries_19,
                            entries_20,
                            entries_21,
                            entries_22);
}

template <typename TYPE, typename ENTRIES_01,
                         typename ENTRIES_02,
                         typename ENTRIES_03,
                         typename ENTRIES_04,
                         typename ENTRIES_05,
                         typename ENTRIES_06,
                         typename ENTRIES_07,
                         typename ENTRIES_08,
                         typename ENTRIES_09,
                         typename ENTRIES_10,
                         typename ENTRIES_11,
                         typename ENTRIES_12,
                         typename ENTRIES_13,
                         typename ENTRIES_14,
                         typename ENTRIES_15,
                         typename ENTRIES_16,
                         typename ENTRIES_17,
                         typename ENTRIES_18,
                         typename ENTRIES_19,
                         typename ENTRIES_20,
                         typename ENTRIES_21,
                         typename ENTRIES_22,
                         typename ENTRIES_23,
                         typename ENTRIES_24>
inline
void DatumMaker::pushBackHelper(bdld::DatumMapBuilder                 *builder,
                                const bslstl::StringRef&               key,
                                BSLS_COMPILERFEATURES_FORWARD_REF(TYPE) value,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_01) entries_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_02) entries_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_03) entries_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_04) entries_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_05) entries_05,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_06) entries_06,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_07) entries_07,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_08) entries_08,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_09) entries_09,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_10) entries_10,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_11) entries_11,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_12) entries_12,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_13) entries_13,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_14) entries_14,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_15) entries_15,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_16) entries_16,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_17) entries_17,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_18) entries_18,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_19) entries_19,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_20) entries_20,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_21) entries_21,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_22) entries_22,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_23) entries_23,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_24) entries_24
                          ) const
{
    builder->pushBack(key, (*this)(value));
    pushBackHelper(builder, entries_01,
                            entries_02,
                            entries_03,
                            entries_04,
                            entries_05,
                            entries_06,
                            entries_07,
                            entries_08,
                            entries_09,
                            entries_10,
                            entries_11,
                            entries_12,
                            entries_13,
                            entries_14,
                            entries_15,
                            entries_16,
                            entries_17,
                            entries_18,
                            entries_19,
                            entries_20,
                            entries_21,
                            entries_22,
                            entries_23,
                            entries_24);
}

template <typename TYPE, typename ENTRIES_01,
                         typename ENTRIES_02,
                         typename ENTRIES_03,
                         typename ENTRIES_04,
                         typename ENTRIES_05,
                         typename ENTRIES_06,
                         typename ENTRIES_07,
                         typename ENTRIES_08,
                         typename ENTRIES_09,
                         typename ENTRIES_10,
                         typename ENTRIES_11,
                         typename ENTRIES_12,
                         typename ENTRIES_13,
                         typename ENTRIES_14,
                         typename ENTRIES_15,
                         typename ENTRIES_16,
                         typename ENTRIES_17,
                         typename ENTRIES_18,
                         typename ENTRIES_19,
                         typename ENTRIES_20,
                         typename ENTRIES_21,
                         typename ENTRIES_22,
                         typename ENTRIES_23,
                         typename ENTRIES_24,
                         typename ENTRIES_25,
                         typename ENTRIES_26>
inline
void DatumMaker::pushBackHelper(bdld::DatumMapBuilder                 *builder,
                                const bslstl::StringRef&               key,
                                BSLS_COMPILERFEATURES_FORWARD_REF(TYPE) value,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_01) entries_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_02) entries_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_03) entries_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_04) entries_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_05) entries_05,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_06) entries_06,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_07) entries_07,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_08) entries_08,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_09) entries_09,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_10) entries_10,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_11) entries_11,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_12) entries_12,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_13) entries_13,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_14) entries_14,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_15) entries_15,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_16) entries_16,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_17) entries_17,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_18) entries_18,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_19) entries_19,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_20) entries_20,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_21) entries_21,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_22) entries_22,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_23) entries_23,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_24) entries_24,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_25) entries_25,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_26) entries_26
                          ) const
{
    builder->pushBack(key, (*this)(value));
    pushBackHelper(builder, entries_01,
                            entries_02,
                            entries_03,
                            entries_04,
                            entries_05,
                            entries_06,
                            entries_07,
                            entries_08,
                            entries_09,
                            entries_10,
                            entries_11,
                            entries_12,
                            entries_13,
                            entries_14,
                            entries_15,
                            entries_16,
                            entries_17,
                            entries_18,
                            entries_19,
                            entries_20,
                            entries_21,
                            entries_22,
                            entries_23,
                            entries_24,
                            entries_25,
                            entries_26);
}

template <typename TYPE, typename ENTRIES_01,
                         typename ENTRIES_02,
                         typename ENTRIES_03,
                         typename ENTRIES_04,
                         typename ENTRIES_05,
                         typename ENTRIES_06,
                         typename ENTRIES_07,
                         typename ENTRIES_08,
                         typename ENTRIES_09,
                         typename ENTRIES_10,
                         typename ENTRIES_11,
                         typename ENTRIES_12,
                         typename ENTRIES_13,
                         typename ENTRIES_14,
                         typename ENTRIES_15,
                         typename ENTRIES_16,
                         typename ENTRIES_17,
                         typename ENTRIES_18,
                         typename ENTRIES_19,
                         typename ENTRIES_20,
                         typename ENTRIES_21,
                         typename ENTRIES_22,
                         typename ENTRIES_23,
                         typename ENTRIES_24,
                         typename ENTRIES_25,
                         typename ENTRIES_26,
                         typename ENTRIES_27,
                         typename ENTRIES_28>
inline
void DatumMaker::pushBackHelper(bdld::DatumMapBuilder                 *builder,
                                const bslstl::StringRef&               key,
                                BSLS_COMPILERFEATURES_FORWARD_REF(TYPE) value,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_01) entries_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_02) entries_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_03) entries_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_04) entries_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_05) entries_05,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_06) entries_06,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_07) entries_07,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_08) entries_08,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_09) entries_09,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_10) entries_10,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_11) entries_11,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_12) entries_12,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_13) entries_13,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_14) entries_14,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_15) entries_15,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_16) entries_16,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_17) entries_17,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_18) entries_18,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_19) entries_19,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_20) entries_20,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_21) entries_21,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_22) entries_22,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_23) entries_23,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_24) entries_24,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_25) entries_25,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_26) entries_26,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_27) entries_27,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_28) entries_28
                          ) const
{
    builder->pushBack(key, (*this)(value));
    pushBackHelper(builder, entries_01,
                            entries_02,
                            entries_03,
                            entries_04,
                            entries_05,
                            entries_06,
                            entries_07,
                            entries_08,
                            entries_09,
                            entries_10,
                            entries_11,
                            entries_12,
                            entries_13,
                            entries_14,
                            entries_15,
                            entries_16,
                            entries_17,
                            entries_18,
                            entries_19,
                            entries_20,
                            entries_21,
                            entries_22,
                            entries_23,
                            entries_24,
                            entries_25,
                            entries_26,
                            entries_27,
                            entries_28);
}

template <typename TYPE, typename ENTRIES_01,
                         typename ENTRIES_02,
                         typename ENTRIES_03,
                         typename ENTRIES_04,
                         typename ENTRIES_05,
                         typename ENTRIES_06,
                         typename ENTRIES_07,
                         typename ENTRIES_08,
                         typename ENTRIES_09,
                         typename ENTRIES_10,
                         typename ENTRIES_11,
                         typename ENTRIES_12,
                         typename ENTRIES_13,
                         typename ENTRIES_14,
                         typename ENTRIES_15,
                         typename ENTRIES_16,
                         typename ENTRIES_17,
                         typename ENTRIES_18,
                         typename ENTRIES_19,
                         typename ENTRIES_20,
                         typename ENTRIES_21,
                         typename ENTRIES_22,
                         typename ENTRIES_23,
                         typename ENTRIES_24,
                         typename ENTRIES_25,
                         typename ENTRIES_26,
                         typename ENTRIES_27,
                         typename ENTRIES_28,
                         typename ENTRIES_29,
                         typename ENTRIES_30>
inline
void DatumMaker::pushBackHelper(bdld::DatumMapBuilder                 *builder,
                                const bslstl::StringRef&               key,
                                BSLS_COMPILERFEATURES_FORWARD_REF(TYPE) value,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_01) entries_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_02) entries_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_03) entries_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_04) entries_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_05) entries_05,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_06) entries_06,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_07) entries_07,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_08) entries_08,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_09) entries_09,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_10) entries_10,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_11) entries_11,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_12) entries_12,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_13) entries_13,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_14) entries_14,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_15) entries_15,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_16) entries_16,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_17) entries_17,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_18) entries_18,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_19) entries_19,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_20) entries_20,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_21) entries_21,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_22) entries_22,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_23) entries_23,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_24) entries_24,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_25) entries_25,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_26) entries_26,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_27) entries_27,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_28) entries_28,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_29) entries_29,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_30) entries_30
                          ) const
{
    builder->pushBack(key, (*this)(value));
    pushBackHelper(builder, entries_01,
                            entries_02,
                            entries_03,
                            entries_04,
                            entries_05,
                            entries_06,
                            entries_07,
                            entries_08,
                            entries_09,
                            entries_10,
                            entries_11,
                            entries_12,
                            entries_13,
                            entries_14,
                            entries_15,
                            entries_16,
                            entries_17,
                            entries_18,
                            entries_19,
                            entries_20,
                            entries_21,
                            entries_22,
                            entries_23,
                            entries_24,
                            entries_25,
                            entries_26,
                            entries_27,
                            entries_28,
                            entries_29,
                            entries_30);
}

template <typename TYPE, typename ENTRIES_01,
                         typename ENTRIES_02,
                         typename ENTRIES_03,
                         typename ENTRIES_04,
                         typename ENTRIES_05,
                         typename ENTRIES_06,
                         typename ENTRIES_07,
                         typename ENTRIES_08,
                         typename ENTRIES_09,
                         typename ENTRIES_10,
                         typename ENTRIES_11,
                         typename ENTRIES_12,
                         typename ENTRIES_13,
                         typename ENTRIES_14,
                         typename ENTRIES_15,
                         typename ENTRIES_16,
                         typename ENTRIES_17,
                         typename ENTRIES_18,
                         typename ENTRIES_19,
                         typename ENTRIES_20,
                         typename ENTRIES_21,
                         typename ENTRIES_22,
                         typename ENTRIES_23,
                         typename ENTRIES_24,
                         typename ENTRIES_25,
                         typename ENTRIES_26,
                         typename ENTRIES_27,
                         typename ENTRIES_28,
                         typename ENTRIES_29,
                         typename ENTRIES_30,
                         typename ENTRIES_31,
                         typename ENTRIES_32>
inline
void DatumMaker::pushBackHelper(bdld::DatumMapBuilder                 *builder,
                                const bslstl::StringRef&               key,
                                BSLS_COMPILERFEATURES_FORWARD_REF(TYPE) value,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_01) entries_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_02) entries_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_03) entries_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_04) entries_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_05) entries_05,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_06) entries_06,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_07) entries_07,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_08) entries_08,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_09) entries_09,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_10) entries_10,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_11) entries_11,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_12) entries_12,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_13) entries_13,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_14) entries_14,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_15) entries_15,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_16) entries_16,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_17) entries_17,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_18) entries_18,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_19) entries_19,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_20) entries_20,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_21) entries_21,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_22) entries_22,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_23) entries_23,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_24) entries_24,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_25) entries_25,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_26) entries_26,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_27) entries_27,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_28) entries_28,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_29) entries_29,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_30) entries_30,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_31) entries_31,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_32) entries_32
                          ) const
{
    builder->pushBack(key, (*this)(value));
    pushBackHelper(builder, entries_01,
                            entries_02,
                            entries_03,
                            entries_04,
                            entries_05,
                            entries_06,
                            entries_07,
                            entries_08,
                            entries_09,
                            entries_10,
                            entries_11,
                            entries_12,
                            entries_13,
                            entries_14,
                            entries_15,
                            entries_16,
                            entries_17,
                            entries_18,
                            entries_19,
                            entries_20,
                            entries_21,
                            entries_22,
                            entries_23,
                            entries_24,
                            entries_25,
                            entries_26,
                            entries_27,
                            entries_28,
                            entries_29,
                            entries_30,
                            entries_31,
                            entries_32);
}

template <typename TYPE>
inline
void DatumMaker::pushBackHelper(
                               bdld::DatumMapOwningKeysBuilder *builder,
                               const bslstl::StringRef&         key,
                               BSLS_COMPILERFEATURES_FORWARD_REF(TYPE) value
                          ) const
{
    builder->pushBack(key, (*this)(value));
    pushBackHelper(builder);
}

template <typename TYPE, typename ENTRIES_01,
                         typename ENTRIES_02>
inline
void DatumMaker::pushBackHelper(
                               bdld::DatumMapOwningKeysBuilder *builder,
                               const bslstl::StringRef&         key,
                               BSLS_COMPILERFEATURES_FORWARD_REF(TYPE) value,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_01) entries_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_02) entries_02
                          ) const
{
    builder->pushBack(key, (*this)(value));
    pushBackHelper(builder, entries_01,
                            entries_02);
}

template <typename TYPE, typename ENTRIES_01,
                         typename ENTRIES_02,
                         typename ENTRIES_03,
                         typename ENTRIES_04>
inline
void DatumMaker::pushBackHelper(
                               bdld::DatumMapOwningKeysBuilder *builder,
                               const bslstl::StringRef&         key,
                               BSLS_COMPILERFEATURES_FORWARD_REF(TYPE) value,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_01) entries_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_02) entries_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_03) entries_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_04) entries_04
                          ) const
{
    builder->pushBack(key, (*this)(value));
    pushBackHelper(builder, entries_01,
                            entries_02,
                            entries_03,
                            entries_04);
}

template <typename TYPE, typename ENTRIES_01,
                         typename ENTRIES_02,
                         typename ENTRIES_03,
                         typename ENTRIES_04,
                         typename ENTRIES_05,
                         typename ENTRIES_06>
inline
void DatumMaker::pushBackHelper(
                               bdld::DatumMapOwningKeysBuilder *builder,
                               const bslstl::StringRef&         key,
                               BSLS_COMPILERFEATURES_FORWARD_REF(TYPE) value,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_01) entries_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_02) entries_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_03) entries_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_04) entries_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_05) entries_05,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_06) entries_06
                          ) const
{
    builder->pushBack(key, (*this)(value));
    pushBackHelper(builder, entries_01,
                            entries_02,
                            entries_03,
                            entries_04,
                            entries_05,
                            entries_06);
}

template <typename TYPE, typename ENTRIES_01,
                         typename ENTRIES_02,
                         typename ENTRIES_03,
                         typename ENTRIES_04,
                         typename ENTRIES_05,
                         typename ENTRIES_06,
                         typename ENTRIES_07,
                         typename ENTRIES_08>
inline
void DatumMaker::pushBackHelper(
                               bdld::DatumMapOwningKeysBuilder *builder,
                               const bslstl::StringRef&         key,
                               BSLS_COMPILERFEATURES_FORWARD_REF(TYPE) value,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_01) entries_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_02) entries_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_03) entries_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_04) entries_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_05) entries_05,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_06) entries_06,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_07) entries_07,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_08) entries_08
                          ) const
{
    builder->pushBack(key, (*this)(value));
    pushBackHelper(builder, entries_01,
                            entries_02,
                            entries_03,
                            entries_04,
                            entries_05,
                            entries_06,
                            entries_07,
                            entries_08);
}

template <typename TYPE, typename ENTRIES_01,
                         typename ENTRIES_02,
                         typename ENTRIES_03,
                         typename ENTRIES_04,
                         typename ENTRIES_05,
                         typename ENTRIES_06,
                         typename ENTRIES_07,
                         typename ENTRIES_08,
                         typename ENTRIES_09,
                         typename ENTRIES_10>
inline
void DatumMaker::pushBackHelper(
                               bdld::DatumMapOwningKeysBuilder *builder,
                               const bslstl::StringRef&         key,
                               BSLS_COMPILERFEATURES_FORWARD_REF(TYPE) value,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_01) entries_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_02) entries_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_03) entries_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_04) entries_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_05) entries_05,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_06) entries_06,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_07) entries_07,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_08) entries_08,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_09) entries_09,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_10) entries_10
                          ) const
{
    builder->pushBack(key, (*this)(value));
    pushBackHelper(builder, entries_01,
                            entries_02,
                            entries_03,
                            entries_04,
                            entries_05,
                            entries_06,
                            entries_07,
                            entries_08,
                            entries_09,
                            entries_10);
}

template <typename TYPE, typename ENTRIES_01,
                         typename ENTRIES_02,
                         typename ENTRIES_03,
                         typename ENTRIES_04,
                         typename ENTRIES_05,
                         typename ENTRIES_06,
                         typename ENTRIES_07,
                         typename ENTRIES_08,
                         typename ENTRIES_09,
                         typename ENTRIES_10,
                         typename ENTRIES_11,
                         typename ENTRIES_12>
inline
void DatumMaker::pushBackHelper(
                               bdld::DatumMapOwningKeysBuilder *builder,
                               const bslstl::StringRef&         key,
                               BSLS_COMPILERFEATURES_FORWARD_REF(TYPE) value,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_01) entries_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_02) entries_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_03) entries_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_04) entries_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_05) entries_05,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_06) entries_06,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_07) entries_07,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_08) entries_08,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_09) entries_09,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_10) entries_10,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_11) entries_11,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_12) entries_12
                          ) const
{
    builder->pushBack(key, (*this)(value));
    pushBackHelper(builder, entries_01,
                            entries_02,
                            entries_03,
                            entries_04,
                            entries_05,
                            entries_06,
                            entries_07,
                            entries_08,
                            entries_09,
                            entries_10,
                            entries_11,
                            entries_12);
}

template <typename TYPE, typename ENTRIES_01,
                         typename ENTRIES_02,
                         typename ENTRIES_03,
                         typename ENTRIES_04,
                         typename ENTRIES_05,
                         typename ENTRIES_06,
                         typename ENTRIES_07,
                         typename ENTRIES_08,
                         typename ENTRIES_09,
                         typename ENTRIES_10,
                         typename ENTRIES_11,
                         typename ENTRIES_12,
                         typename ENTRIES_13,
                         typename ENTRIES_14>
inline
void DatumMaker::pushBackHelper(
                               bdld::DatumMapOwningKeysBuilder *builder,
                               const bslstl::StringRef&         key,
                               BSLS_COMPILERFEATURES_FORWARD_REF(TYPE) value,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_01) entries_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_02) entries_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_03) entries_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_04) entries_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_05) entries_05,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_06) entries_06,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_07) entries_07,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_08) entries_08,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_09) entries_09,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_10) entries_10,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_11) entries_11,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_12) entries_12,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_13) entries_13,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_14) entries_14
                          ) const
{
    builder->pushBack(key, (*this)(value));
    pushBackHelper(builder, entries_01,
                            entries_02,
                            entries_03,
                            entries_04,
                            entries_05,
                            entries_06,
                            entries_07,
                            entries_08,
                            entries_09,
                            entries_10,
                            entries_11,
                            entries_12,
                            entries_13,
                            entries_14);
}

template <typename TYPE, typename ENTRIES_01,
                         typename ENTRIES_02,
                         typename ENTRIES_03,
                         typename ENTRIES_04,
                         typename ENTRIES_05,
                         typename ENTRIES_06,
                         typename ENTRIES_07,
                         typename ENTRIES_08,
                         typename ENTRIES_09,
                         typename ENTRIES_10,
                         typename ENTRIES_11,
                         typename ENTRIES_12,
                         typename ENTRIES_13,
                         typename ENTRIES_14,
                         typename ENTRIES_15,
                         typename ENTRIES_16>
inline
void DatumMaker::pushBackHelper(
                               bdld::DatumMapOwningKeysBuilder *builder,
                               const bslstl::StringRef&         key,
                               BSLS_COMPILERFEATURES_FORWARD_REF(TYPE) value,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_01) entries_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_02) entries_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_03) entries_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_04) entries_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_05) entries_05,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_06) entries_06,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_07) entries_07,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_08) entries_08,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_09) entries_09,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_10) entries_10,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_11) entries_11,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_12) entries_12,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_13) entries_13,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_14) entries_14,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_15) entries_15,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_16) entries_16
                          ) const
{
    builder->pushBack(key, (*this)(value));
    pushBackHelper(builder, entries_01,
                            entries_02,
                            entries_03,
                            entries_04,
                            entries_05,
                            entries_06,
                            entries_07,
                            entries_08,
                            entries_09,
                            entries_10,
                            entries_11,
                            entries_12,
                            entries_13,
                            entries_14,
                            entries_15,
                            entries_16);
}
template <typename TYPE, typename ENTRIES_01,
                         typename ENTRIES_02,
                         typename ENTRIES_03,
                         typename ENTRIES_04,
                         typename ENTRIES_05,
                         typename ENTRIES_06,
                         typename ENTRIES_07,
                         typename ENTRIES_08,
                         typename ENTRIES_09,
                         typename ENTRIES_10,
                         typename ENTRIES_11,
                         typename ENTRIES_12,
                         typename ENTRIES_13,
                         typename ENTRIES_14,
                         typename ENTRIES_15,
                         typename ENTRIES_16,
                         typename ENTRIES_17,
                         typename ENTRIES_18>
inline
void DatumMaker::pushBackHelper(
                               bdld::DatumMapOwningKeysBuilder *builder,
                               const bslstl::StringRef&         key,
                               BSLS_COMPILERFEATURES_FORWARD_REF(TYPE) value,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_01) entries_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_02) entries_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_03) entries_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_04) entries_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_05) entries_05,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_06) entries_06,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_07) entries_07,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_08) entries_08,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_09) entries_09,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_10) entries_10,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_11) entries_11,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_12) entries_12,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_13) entries_13,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_14) entries_14,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_15) entries_15,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_16) entries_16,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_17) entries_17,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_18) entries_18
                          ) const
{
    builder->pushBack(key, (*this)(value));
    pushBackHelper(builder, entries_01,
                            entries_02,
                            entries_03,
                            entries_04,
                            entries_05,
                            entries_06,
                            entries_07,
                            entries_08,
                            entries_09,
                            entries_10,
                            entries_11,
                            entries_12,
                            entries_13,
                            entries_14,
                            entries_15,
                            entries_16,
                            entries_17,
                            entries_18);
}

template <typename TYPE, typename ENTRIES_01,
                         typename ENTRIES_02,
                         typename ENTRIES_03,
                         typename ENTRIES_04,
                         typename ENTRIES_05,
                         typename ENTRIES_06,
                         typename ENTRIES_07,
                         typename ENTRIES_08,
                         typename ENTRIES_09,
                         typename ENTRIES_10,
                         typename ENTRIES_11,
                         typename ENTRIES_12,
                         typename ENTRIES_13,
                         typename ENTRIES_14,
                         typename ENTRIES_15,
                         typename ENTRIES_16,
                         typename ENTRIES_17,
                         typename ENTRIES_18,
                         typename ENTRIES_19,
                         typename ENTRIES_20>
inline
void DatumMaker::pushBackHelper(
                               bdld::DatumMapOwningKeysBuilder *builder,
                               const bslstl::StringRef&         key,
                               BSLS_COMPILERFEATURES_FORWARD_REF(TYPE) value,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_01) entries_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_02) entries_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_03) entries_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_04) entries_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_05) entries_05,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_06) entries_06,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_07) entries_07,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_08) entries_08,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_09) entries_09,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_10) entries_10,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_11) entries_11,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_12) entries_12,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_13) entries_13,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_14) entries_14,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_15) entries_15,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_16) entries_16,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_17) entries_17,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_18) entries_18,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_19) entries_19,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_20) entries_20
                          ) const
{
    builder->pushBack(key, (*this)(value));
    pushBackHelper(builder, entries_01,
                            entries_02,
                            entries_03,
                            entries_04,
                            entries_05,
                            entries_06,
                            entries_07,
                            entries_08,
                            entries_09,
                            entries_10,
                            entries_11,
                            entries_12,
                            entries_13,
                            entries_14,
                            entries_15,
                            entries_16,
                            entries_17,
                            entries_18,
                            entries_19,
                            entries_20);
}

template <typename TYPE, typename ENTRIES_01,
                         typename ENTRIES_02,
                         typename ENTRIES_03,
                         typename ENTRIES_04,
                         typename ENTRIES_05,
                         typename ENTRIES_06,
                         typename ENTRIES_07,
                         typename ENTRIES_08,
                         typename ENTRIES_09,
                         typename ENTRIES_10,
                         typename ENTRIES_11,
                         typename ENTRIES_12,
                         typename ENTRIES_13,
                         typename ENTRIES_14,
                         typename ENTRIES_15,
                         typename ENTRIES_16,
                         typename ENTRIES_17,
                         typename ENTRIES_18,
                         typename ENTRIES_19,
                         typename ENTRIES_20,
                         typename ENTRIES_21,
                         typename ENTRIES_22>
inline
void DatumMaker::pushBackHelper(
                               bdld::DatumMapOwningKeysBuilder *builder,
                               const bslstl::StringRef&         key,
                               BSLS_COMPILERFEATURES_FORWARD_REF(TYPE) value,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_01) entries_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_02) entries_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_03) entries_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_04) entries_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_05) entries_05,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_06) entries_06,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_07) entries_07,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_08) entries_08,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_09) entries_09,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_10) entries_10,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_11) entries_11,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_12) entries_12,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_13) entries_13,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_14) entries_14,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_15) entries_15,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_16) entries_16,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_17) entries_17,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_18) entries_18,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_19) entries_19,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_20) entries_20,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_21) entries_21,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_22) entries_22
                          ) const
{
    builder->pushBack(key, (*this)(value));
    pushBackHelper(builder, entries_01,
                            entries_02,
                            entries_03,
                            entries_04,
                            entries_05,
                            entries_06,
                            entries_07,
                            entries_08,
                            entries_09,
                            entries_10,
                            entries_11,
                            entries_12,
                            entries_13,
                            entries_14,
                            entries_15,
                            entries_16,
                            entries_17,
                            entries_18,
                            entries_19,
                            entries_20,
                            entries_21,
                            entries_22);
}

template <typename TYPE, typename ENTRIES_01,
                         typename ENTRIES_02,
                         typename ENTRIES_03,
                         typename ENTRIES_04,
                         typename ENTRIES_05,
                         typename ENTRIES_06,
                         typename ENTRIES_07,
                         typename ENTRIES_08,
                         typename ENTRIES_09,
                         typename ENTRIES_10,
                         typename ENTRIES_11,
                         typename ENTRIES_12,
                         typename ENTRIES_13,
                         typename ENTRIES_14,
                         typename ENTRIES_15,
                         typename ENTRIES_16,
                         typename ENTRIES_17,
                         typename ENTRIES_18,
                         typename ENTRIES_19,
                         typename ENTRIES_20,
                         typename ENTRIES_21,
                         typename ENTRIES_22,
                         typename ENTRIES_23,
                         typename ENTRIES_24>
inline
void DatumMaker::pushBackHelper(
                               bdld::DatumMapOwningKeysBuilder *builder,
                               const bslstl::StringRef&         key,
                               BSLS_COMPILERFEATURES_FORWARD_REF(TYPE) value,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_01) entries_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_02) entries_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_03) entries_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_04) entries_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_05) entries_05,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_06) entries_06,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_07) entries_07,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_08) entries_08,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_09) entries_09,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_10) entries_10,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_11) entries_11,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_12) entries_12,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_13) entries_13,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_14) entries_14,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_15) entries_15,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_16) entries_16,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_17) entries_17,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_18) entries_18,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_19) entries_19,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_20) entries_20,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_21) entries_21,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_22) entries_22,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_23) entries_23,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_24) entries_24
                          ) const
{
    builder->pushBack(key, (*this)(value));
    pushBackHelper(builder, entries_01,
                            entries_02,
                            entries_03,
                            entries_04,
                            entries_05,
                            entries_06,
                            entries_07,
                            entries_08,
                            entries_09,
                            entries_10,
                            entries_11,
                            entries_12,
                            entries_13,
                            entries_14,
                            entries_15,
                            entries_16,
                            entries_17,
                            entries_18,
                            entries_19,
                            entries_20,
                            entries_21,
                            entries_22,
                            entries_23,
                            entries_24);
}

template <typename TYPE, typename ENTRIES_01,
                         typename ENTRIES_02,
                         typename ENTRIES_03,
                         typename ENTRIES_04,
                         typename ENTRIES_05,
                         typename ENTRIES_06,
                         typename ENTRIES_07,
                         typename ENTRIES_08,
                         typename ENTRIES_09,
                         typename ENTRIES_10,
                         typename ENTRIES_11,
                         typename ENTRIES_12,
                         typename ENTRIES_13,
                         typename ENTRIES_14,
                         typename ENTRIES_15,
                         typename ENTRIES_16,
                         typename ENTRIES_17,
                         typename ENTRIES_18,
                         typename ENTRIES_19,
                         typename ENTRIES_20,
                         typename ENTRIES_21,
                         typename ENTRIES_22,
                         typename ENTRIES_23,
                         typename ENTRIES_24,
                         typename ENTRIES_25,
                         typename ENTRIES_26>
inline
void DatumMaker::pushBackHelper(
                               bdld::DatumMapOwningKeysBuilder *builder,
                               const bslstl::StringRef&         key,
                               BSLS_COMPILERFEATURES_FORWARD_REF(TYPE) value,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_01) entries_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_02) entries_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_03) entries_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_04) entries_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_05) entries_05,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_06) entries_06,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_07) entries_07,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_08) entries_08,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_09) entries_09,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_10) entries_10,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_11) entries_11,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_12) entries_12,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_13) entries_13,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_14) entries_14,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_15) entries_15,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_16) entries_16,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_17) entries_17,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_18) entries_18,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_19) entries_19,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_20) entries_20,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_21) entries_21,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_22) entries_22,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_23) entries_23,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_24) entries_24,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_25) entries_25,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_26) entries_26
                          ) const
{
    builder->pushBack(key, (*this)(value));
    pushBackHelper(builder, entries_01,
                            entries_02,
                            entries_03,
                            entries_04,
                            entries_05,
                            entries_06,
                            entries_07,
                            entries_08,
                            entries_09,
                            entries_10,
                            entries_11,
                            entries_12,
                            entries_13,
                            entries_14,
                            entries_15,
                            entries_16,
                            entries_17,
                            entries_18,
                            entries_19,
                            entries_20,
                            entries_21,
                            entries_22,
                            entries_23,
                            entries_24,
                            entries_25,
                            entries_26);
}

template <typename TYPE, typename ENTRIES_01,
                         typename ENTRIES_02,
                         typename ENTRIES_03,
                         typename ENTRIES_04,
                         typename ENTRIES_05,
                         typename ENTRIES_06,
                         typename ENTRIES_07,
                         typename ENTRIES_08,
                         typename ENTRIES_09,
                         typename ENTRIES_10,
                         typename ENTRIES_11,
                         typename ENTRIES_12,
                         typename ENTRIES_13,
                         typename ENTRIES_14,
                         typename ENTRIES_15,
                         typename ENTRIES_16,
                         typename ENTRIES_17,
                         typename ENTRIES_18,
                         typename ENTRIES_19,
                         typename ENTRIES_20,
                         typename ENTRIES_21,
                         typename ENTRIES_22,
                         typename ENTRIES_23,
                         typename ENTRIES_24,
                         typename ENTRIES_25,
                         typename ENTRIES_26,
                         typename ENTRIES_27,
                         typename ENTRIES_28>
inline
void DatumMaker::pushBackHelper(
                               bdld::DatumMapOwningKeysBuilder *builder,
                               const bslstl::StringRef&         key,
                               BSLS_COMPILERFEATURES_FORWARD_REF(TYPE) value,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_01) entries_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_02) entries_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_03) entries_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_04) entries_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_05) entries_05,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_06) entries_06,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_07) entries_07,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_08) entries_08,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_09) entries_09,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_10) entries_10,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_11) entries_11,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_12) entries_12,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_13) entries_13,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_14) entries_14,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_15) entries_15,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_16) entries_16,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_17) entries_17,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_18) entries_18,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_19) entries_19,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_20) entries_20,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_21) entries_21,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_22) entries_22,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_23) entries_23,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_24) entries_24,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_25) entries_25,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_26) entries_26,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_27) entries_27,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_28) entries_28
                          ) const
{
    builder->pushBack(key, (*this)(value));
    pushBackHelper(builder, entries_01,
                            entries_02,
                            entries_03,
                            entries_04,
                            entries_05,
                            entries_06,
                            entries_07,
                            entries_08,
                            entries_09,
                            entries_10,
                            entries_11,
                            entries_12,
                            entries_13,
                            entries_14,
                            entries_15,
                            entries_16,
                            entries_17,
                            entries_18,
                            entries_19,
                            entries_20,
                            entries_21,
                            entries_22,
                            entries_23,
                            entries_24,
                            entries_25,
                            entries_26,
                            entries_27,
                            entries_28);
}

template <typename TYPE, typename ENTRIES_01,
                         typename ENTRIES_02,
                         typename ENTRIES_03,
                         typename ENTRIES_04,
                         typename ENTRIES_05,
                         typename ENTRIES_06,
                         typename ENTRIES_07,
                         typename ENTRIES_08,
                         typename ENTRIES_09,
                         typename ENTRIES_10,
                         typename ENTRIES_11,
                         typename ENTRIES_12,
                         typename ENTRIES_13,
                         typename ENTRIES_14,
                         typename ENTRIES_15,
                         typename ENTRIES_16,
                         typename ENTRIES_17,
                         typename ENTRIES_18,
                         typename ENTRIES_19,
                         typename ENTRIES_20,
                         typename ENTRIES_21,
                         typename ENTRIES_22,
                         typename ENTRIES_23,
                         typename ENTRIES_24,
                         typename ENTRIES_25,
                         typename ENTRIES_26,
                         typename ENTRIES_27,
                         typename ENTRIES_28,
                         typename ENTRIES_29,
                         typename ENTRIES_30>
inline
void DatumMaker::pushBackHelper(
                               bdld::DatumMapOwningKeysBuilder *builder,
                               const bslstl::StringRef&         key,
                               BSLS_COMPILERFEATURES_FORWARD_REF(TYPE) value,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_01) entries_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_02) entries_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_03) entries_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_04) entries_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_05) entries_05,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_06) entries_06,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_07) entries_07,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_08) entries_08,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_09) entries_09,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_10) entries_10,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_11) entries_11,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_12) entries_12,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_13) entries_13,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_14) entries_14,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_15) entries_15,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_16) entries_16,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_17) entries_17,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_18) entries_18,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_19) entries_19,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_20) entries_20,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_21) entries_21,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_22) entries_22,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_23) entries_23,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_24) entries_24,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_25) entries_25,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_26) entries_26,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_27) entries_27,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_28) entries_28,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_29) entries_29,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_30) entries_30
                          ) const
{
    builder->pushBack(key, (*this)(value));
    pushBackHelper(builder, entries_01,
                            entries_02,
                            entries_03,
                            entries_04,
                            entries_05,
                            entries_06,
                            entries_07,
                            entries_08,
                            entries_09,
                            entries_10,
                            entries_11,
                            entries_12,
                            entries_13,
                            entries_14,
                            entries_15,
                            entries_16,
                            entries_17,
                            entries_18,
                            entries_19,
                            entries_20,
                            entries_21,
                            entries_22,
                            entries_23,
                            entries_24,
                            entries_25,
                            entries_26,
                            entries_27,
                            entries_28,
                            entries_29,
                            entries_30);
}

template <typename TYPE, typename ENTRIES_01,
                         typename ENTRIES_02,
                         typename ENTRIES_03,
                         typename ENTRIES_04,
                         typename ENTRIES_05,
                         typename ENTRIES_06,
                         typename ENTRIES_07,
                         typename ENTRIES_08,
                         typename ENTRIES_09,
                         typename ENTRIES_10,
                         typename ENTRIES_11,
                         typename ENTRIES_12,
                         typename ENTRIES_13,
                         typename ENTRIES_14,
                         typename ENTRIES_15,
                         typename ENTRIES_16,
                         typename ENTRIES_17,
                         typename ENTRIES_18,
                         typename ENTRIES_19,
                         typename ENTRIES_20,
                         typename ENTRIES_21,
                         typename ENTRIES_22,
                         typename ENTRIES_23,
                         typename ENTRIES_24,
                         typename ENTRIES_25,
                         typename ENTRIES_26,
                         typename ENTRIES_27,
                         typename ENTRIES_28,
                         typename ENTRIES_29,
                         typename ENTRIES_30,
                         typename ENTRIES_31,
                         typename ENTRIES_32>
inline
void DatumMaker::pushBackHelper(
                               bdld::DatumMapOwningKeysBuilder *builder,
                               const bslstl::StringRef&         key,
                               BSLS_COMPILERFEATURES_FORWARD_REF(TYPE) value,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_01) entries_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_02) entries_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_03) entries_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_04) entries_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_05) entries_05,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_06) entries_06,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_07) entries_07,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_08) entries_08,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_09) entries_09,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_10) entries_10,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_11) entries_11,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_12) entries_12,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_13) entries_13,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_14) entries_14,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_15) entries_15,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_16) entries_16,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_17) entries_17,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_18) entries_18,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_19) entries_19,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_20) entries_20,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_21) entries_21,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_22) entries_22,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_23) entries_23,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_24) entries_24,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_25) entries_25,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_26) entries_26,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_27) entries_27,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_28) entries_28,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_29) entries_29,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_30) entries_30,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_31) entries_31,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_32) entries_32
                          ) const
{
    builder->pushBack(key, (*this)(value));
    pushBackHelper(builder, entries_01,
                            entries_02,
                            entries_03,
                            entries_04,
                            entries_05,
                            entries_06,
                            entries_07,
                            entries_08,
                            entries_09,
                            entries_10,
                            entries_11,
                            entries_12,
                            entries_13,
                            entries_14,
                            entries_15,
                            entries_16,
                            entries_17,
                            entries_18,
                            entries_19,
                            entries_20,
                            entries_21,
                            entries_22,
                            entries_23,
                            entries_24,
                            entries_25,
                            entries_26,
                            entries_27,
                            entries_28,
                            entries_29,
                            entries_30,
                            entries_31,
                            entries_32);
}

#else
// The generated code below is a workaround for the absence of perfect
// forwarding in some compilers.
template <typename TYPE>
inline
void DatumMaker::pushBackHelper(bdld::DatumArrayBuilder *builder,
                                BSLS_COMPILERFEATURES_FORWARD_REF(TYPE) element
                                ) const
{
    builder->pushBack((*this)(element));
}

template <typename TYPE, typename... ELEMENTS>
inline
void DatumMaker::pushBackHelper(bdld::DatumArrayBuilder *builder,
                               BSLS_COMPILERFEATURES_FORWARD_REF(TYPE) element,
                        BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS)... elements
                        ) const
{
    builder->pushBack((*this)(element));
    pushBackHelper(builder, elements...);
}

template <typename TYPE>
inline
void DatumMaker::pushBackHelper(bdld::DatumMapBuilder                  *builder,
                                const bslstl::StringRef&                key,
                                BSLS_COMPILERFEATURES_FORWARD_REF(TYPE) value
                                ) const
{
    builder->pushBack(key, (*this)(value));
}

template <typename TYPE, typename... ENTRIES>
inline
void DatumMaker::pushBackHelper(bdld::DatumMapBuilder                  *builder,
                                const bslstl::StringRef&                key,
                                BSLS_COMPILERFEATURES_FORWARD_REF(TYPE) value,
                          BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES)... entries
                          ) const
{
    builder->pushBack(key, (*this)(value));
    pushBackHelper(builder, entries...);
}

template <typename TYPE>
inline
void DatumMaker::pushBackHelper(bdld::DatumMapOwningKeysBuilder *builder,
                                const bslstl::StringRef&         key,
                                BSLS_COMPILERFEATURES_FORWARD_REF(TYPE) value
                                ) const
{
    builder->pushBack(key, (*this)(value));
}

template <typename TYPE, typename... ENTRIES>
inline
void DatumMaker::pushBackHelper(
                               bdld::DatumMapOwningKeysBuilder *builder,
                               const bslstl::StringRef&         key,
                               BSLS_COMPILERFEATURES_FORWARD_REF(TYPE) value,
                          BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES)... entries
                          ) const
{
    builder->pushBack(key, (*this)(value));
    pushBackHelper(builder, entries...);
}
#endif

// CREATORS
inline
DatumMaker::DatumMaker(bslma::Allocator *basicAllocator)
: d_allocator_p(basicAllocator)
{
    BSLS_ASSERT_SAFE(basicAllocator);
}

// ACCESSORS
inline
bslma::Allocator *DatumMaker::allocator() const
{
    return d_allocator_p;
}

inline
bdld::Datum DatumMaker::operator()() const
{
    return bdld::Datum::createNull();
}

inline
bdld::Datum DatumMaker::operator()(const bslmf::Nil&) const
{
    return (*this)();
}

inline
bdld::Datum DatumMaker::operator()(int value) const
{
    return bdld::Datum::createInteger(value);
}

inline
bdld::Datum DatumMaker::operator()(double value) const
{
    return bdld::Datum::createDouble(value);
}

inline
bdld::Datum DatumMaker::operator()(const bslstl::StringRef& value) const
{
    return bdld::Datum::copyString(value, d_allocator_p);
}

inline
bdld::Datum DatumMaker::operator()(const char *value) const
{
    return (*this)(bslstl::StringRef(value));
}

inline
bdld::Datum DatumMaker::operator()(bool value) const
{
    return bdld::Datum::createBoolean(value);
}

inline
bdld::Datum DatumMaker::operator()(const bdld::DatumError& value) const
{
    return bdld::Datum::createError(value.code(),
                                    value.message(),
                                    d_allocator_p);
}

inline
bdld::Datum DatumMaker::operator()(const bdlt::Date& value) const
{
    return bdld::Datum::createDate(value);
}

inline
bdld::Datum DatumMaker::operator()(const bdlt::Time& value) const
{
    return bdld::Datum::createTime(value);
}

inline
bdld::Datum DatumMaker::operator()(const bdlt::Datetime& value) const
{
    return bdld::Datum::createDatetime(value, d_allocator_p);
}

inline
bdld::Datum DatumMaker::operator()(const bdlt::DatetimeInterval& value) const
{
    return bdld::Datum::createDatetimeInterval(value, d_allocator_p);
}

inline
bdld::Datum DatumMaker::operator()(bdldfp::Decimal64 value) const
{
    return bdld::Datum::createDecimal64(value, d_allocator_p);
}

inline
bdld::Datum DatumMaker::operator()(bsls::Types::Int64 value) const
{
    return bdld::Datum::createInteger64(value, d_allocator_p);
}

inline
bdld::Datum DatumMaker::operator()(const bdld::DatumUdt& value) const
{
    return bdld::Datum::createUdt(value.data(), value.type());
}

inline
bdld::Datum DatumMaker::operator()(const bdld::Datum& value) const
{
    return value;
}

inline
bdld::Datum DatumMaker::operator()(const bdld::DatumArrayRef& value) const
{
    return bdld::Datum::createArrayReference(value, d_allocator_p);
}

inline
bdld::Datum DatumMaker::operator()(const bdld::Datum *elements,
                                   int                size) const
{
    return (*this)(bdld::DatumArrayRef(elements, size));
}

inline
bdld::Datum DatumMaker::operator()(const bdld::DatumMutableMapRef& value) const
{
    return bdld::Datum::adoptMap(value);
}

inline
bdld::Datum DatumMaker::operator()(const bdld::DatumMapEntry *elements,
                                   int                        size,
                                   bool                       sorted) const
{
    bdld::DatumMutableMapRef map;
    bdld::Datum::createUninitializedMap(&map, size, d_allocator_p);
    for (int i = 0; i < size; ++i) {
        map.data()[i] = elements[i];
    }
    *map.size()   = size;
    *map.sorted() = sorted;
    return bdld::Datum::adoptMap(map);
}

template <class TYPE>
inline
bdld::Datum DatumMaker::operator()(
                                  const bdlb::NullableValue<TYPE>& value) const
{
    return value.isNull() ? (*this)() : (*this)(value.value());
}

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
template <typename... ELEMENTS>
inline
bdld::Datum DatumMaker::a(ELEMENTS&&... elements) const
{
    const int numElements = sizeof...(ELEMENTS);
    bdld::DatumArrayBuilder builder(numElements, d_allocator_p);
    pushBackHelper(&builder, elements...);
    return builder.commit();
}
#elif BSLS_COMPILERFEATURES_SIMULATE_VARIADIC_TEMPLATES
inline
bdld::Datum DatumMaker::a(
                        ) const
{
    const int numElements =  0u;
    bdld::DatumArrayBuilder builder(numElements, d_allocator_p);
    pushBackHelper(&builder);
    return builder.commit();
}

template <typename ELEMENTS_01>
inline
bdld::Datum DatumMaker::a(
                     BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_01) elements_01
                        ) const
{
    const int numElements =  1u;
    bdld::DatumArrayBuilder builder(numElements, d_allocator_p);
    pushBackHelper(&builder, elements_01);
    return builder.commit();
}

template <typename ELEMENTS_01,
          typename ELEMENTS_02>
inline
bdld::Datum DatumMaker::a(
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_01) elements_01,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_02) elements_02
                        ) const
{
    const int numElements =  2u;
    bdld::DatumArrayBuilder builder(numElements, d_allocator_p);
    pushBackHelper(&builder, elements_01,
                             elements_02);
    return builder.commit();
}

template <typename ELEMENTS_01,
          typename ELEMENTS_02,
          typename ELEMENTS_03>
inline
bdld::Datum DatumMaker::a(
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_01) elements_01,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_02) elements_02,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_03) elements_03
                        ) const
{
    const int numElements =  3u;
    bdld::DatumArrayBuilder builder(numElements, d_allocator_p);
    pushBackHelper(&builder, elements_01,
                             elements_02,
                             elements_03);
    return builder.commit();
}

template <typename ELEMENTS_01,
          typename ELEMENTS_02,
          typename ELEMENTS_03,
          typename ELEMENTS_04>
inline
bdld::Datum DatumMaker::a(
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_01) elements_01,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_02) elements_02,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_03) elements_03,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_04) elements_04
                        ) const
{
    const int numElements =  4u;
    bdld::DatumArrayBuilder builder(numElements, d_allocator_p);
    pushBackHelper(&builder, elements_01,
                             elements_02,
                             elements_03,
                             elements_04);
    return builder.commit();
}

template <typename ELEMENTS_01,
          typename ELEMENTS_02,
          typename ELEMENTS_03,
          typename ELEMENTS_04,
          typename ELEMENTS_05>
inline
bdld::Datum DatumMaker::a(
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_01) elements_01,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_02) elements_02,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_03) elements_03,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_04) elements_04,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_05) elements_05
                        ) const
{
    const int numElements =  5u;
    bdld::DatumArrayBuilder builder(numElements, d_allocator_p);
    pushBackHelper(&builder, elements_01,
                             elements_02,
                             elements_03,
                             elements_04,
                             elements_05);
    return builder.commit();
}

template <typename ELEMENTS_01,
          typename ELEMENTS_02,
          typename ELEMENTS_03,
          typename ELEMENTS_04,
          typename ELEMENTS_05,
          typename ELEMENTS_06>
inline
bdld::Datum DatumMaker::a(
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_01) elements_01,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_02) elements_02,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_03) elements_03,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_04) elements_04,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_05) elements_05,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_06) elements_06
                        ) const
{
    const int numElements =  6u;
    bdld::DatumArrayBuilder builder(numElements, d_allocator_p);
    pushBackHelper(&builder, elements_01,
                             elements_02,
                             elements_03,
                             elements_04,
                             elements_05,
                             elements_06);
    return builder.commit();
}

template <typename ELEMENTS_01,
          typename ELEMENTS_02,
          typename ELEMENTS_03,
          typename ELEMENTS_04,
          typename ELEMENTS_05,
          typename ELEMENTS_06,
          typename ELEMENTS_07>
inline
bdld::Datum DatumMaker::a(
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_01) elements_01,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_02) elements_02,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_03) elements_03,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_04) elements_04,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_05) elements_05,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_06) elements_06,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_07) elements_07
                        ) const
{
    const int numElements =  7u;
    bdld::DatumArrayBuilder builder(numElements, d_allocator_p);
    pushBackHelper(&builder, elements_01,
                             elements_02,
                             elements_03,
                             elements_04,
                             elements_05,
                             elements_06,
                             elements_07);
    return builder.commit();
}

template <typename ELEMENTS_01,
          typename ELEMENTS_02,
          typename ELEMENTS_03,
          typename ELEMENTS_04,
          typename ELEMENTS_05,
          typename ELEMENTS_06,
          typename ELEMENTS_07,
          typename ELEMENTS_08>
inline
bdld::Datum DatumMaker::a(
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_01) elements_01,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_02) elements_02,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_03) elements_03,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_04) elements_04,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_05) elements_05,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_06) elements_06,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_07) elements_07,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_08) elements_08
                        ) const
{
    const int numElements =  8u;
    bdld::DatumArrayBuilder builder(numElements, d_allocator_p);
    pushBackHelper(&builder, elements_01,
                             elements_02,
                             elements_03,
                             elements_04,
                             elements_05,
                             elements_06,
                             elements_07,
                             elements_08);
    return builder.commit();
}

template <typename ELEMENTS_01,
          typename ELEMENTS_02,
          typename ELEMENTS_03,
          typename ELEMENTS_04,
          typename ELEMENTS_05,
          typename ELEMENTS_06,
          typename ELEMENTS_07,
          typename ELEMENTS_08,
          typename ELEMENTS_09>
inline
bdld::Datum DatumMaker::a(
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_01) elements_01,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_02) elements_02,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_03) elements_03,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_04) elements_04,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_05) elements_05,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_06) elements_06,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_07) elements_07,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_08) elements_08,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_09) elements_09
                        ) const
{
    const int numElements =  9u;
    bdld::DatumArrayBuilder builder(numElements, d_allocator_p);
    pushBackHelper(&builder, elements_01,
                             elements_02,
                             elements_03,
                             elements_04,
                             elements_05,
                             elements_06,
                             elements_07,
                             elements_08,
                             elements_09);
    return builder.commit();
}

template <typename ELEMENTS_01,
          typename ELEMENTS_02,
          typename ELEMENTS_03,
          typename ELEMENTS_04,
          typename ELEMENTS_05,
          typename ELEMENTS_06,
          typename ELEMENTS_07,
          typename ELEMENTS_08,
          typename ELEMENTS_09,
          typename ELEMENTS_10>
inline
bdld::Datum DatumMaker::a(
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_01) elements_01,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_02) elements_02,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_03) elements_03,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_04) elements_04,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_05) elements_05,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_06) elements_06,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_07) elements_07,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_08) elements_08,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_09) elements_09,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_10) elements_10
                        ) const
{
    const int numElements = 10u;
    bdld::DatumArrayBuilder builder(numElements, d_allocator_p);
    pushBackHelper(&builder, elements_01,
                             elements_02,
                             elements_03,
                             elements_04,
                             elements_05,
                             elements_06,
                             elements_07,
                             elements_08,
                             elements_09,
                             elements_10);
    return builder.commit();
}

template <typename ELEMENTS_01,
          typename ELEMENTS_02,
          typename ELEMENTS_03,
          typename ELEMENTS_04,
          typename ELEMENTS_05,
          typename ELEMENTS_06,
          typename ELEMENTS_07,
          typename ELEMENTS_08,
          typename ELEMENTS_09,
          typename ELEMENTS_10,
          typename ELEMENTS_11>
inline
bdld::Datum DatumMaker::a(
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_01) elements_01,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_02) elements_02,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_03) elements_03,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_04) elements_04,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_05) elements_05,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_06) elements_06,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_07) elements_07,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_08) elements_08,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_09) elements_09,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_10) elements_10,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_11) elements_11
                        ) const
{
    const int numElements = 11u;
    bdld::DatumArrayBuilder builder(numElements, d_allocator_p);
    pushBackHelper(&builder, elements_01,
                             elements_02,
                             elements_03,
                             elements_04,
                             elements_05,
                             elements_06,
                             elements_07,
                             elements_08,
                             elements_09,
                             elements_10,
                             elements_11);
    return builder.commit();
}

template <typename ELEMENTS_01,
          typename ELEMENTS_02,
          typename ELEMENTS_03,
          typename ELEMENTS_04,
          typename ELEMENTS_05,
          typename ELEMENTS_06,
          typename ELEMENTS_07,
          typename ELEMENTS_08,
          typename ELEMENTS_09,
          typename ELEMENTS_10,
          typename ELEMENTS_11,
          typename ELEMENTS_12>
inline
bdld::Datum DatumMaker::a(
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_01) elements_01,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_02) elements_02,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_03) elements_03,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_04) elements_04,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_05) elements_05,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_06) elements_06,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_07) elements_07,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_08) elements_08,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_09) elements_09,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_10) elements_10,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_11) elements_11,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_12) elements_12
                        ) const
{
    const int numElements = 12u;
    bdld::DatumArrayBuilder builder(numElements, d_allocator_p);
    pushBackHelper(&builder, elements_01,
                             elements_02,
                             elements_03,
                             elements_04,
                             elements_05,
                             elements_06,
                             elements_07,
                             elements_08,
                             elements_09,
                             elements_10,
                             elements_11,
                             elements_12);
    return builder.commit();
}

template <typename ELEMENTS_01,
          typename ELEMENTS_02,
          typename ELEMENTS_03,
          typename ELEMENTS_04,
          typename ELEMENTS_05,
          typename ELEMENTS_06,
          typename ELEMENTS_07,
          typename ELEMENTS_08,
          typename ELEMENTS_09,
          typename ELEMENTS_10,
          typename ELEMENTS_11,
          typename ELEMENTS_12,
          typename ELEMENTS_13>
inline
bdld::Datum DatumMaker::a(
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_01) elements_01,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_02) elements_02,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_03) elements_03,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_04) elements_04,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_05) elements_05,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_06) elements_06,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_07) elements_07,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_08) elements_08,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_09) elements_09,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_10) elements_10,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_11) elements_11,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_12) elements_12,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_13) elements_13
                        ) const
{
    const int numElements = 13u;
    bdld::DatumArrayBuilder builder(numElements, d_allocator_p);
    pushBackHelper(&builder, elements_01,
                             elements_02,
                             elements_03,
                             elements_04,
                             elements_05,
                             elements_06,
                             elements_07,
                             elements_08,
                             elements_09,
                             elements_10,
                             elements_11,
                             elements_12,
                             elements_13);
    return builder.commit();
}

template <typename ELEMENTS_01,
          typename ELEMENTS_02,
          typename ELEMENTS_03,
          typename ELEMENTS_04,
          typename ELEMENTS_05,
          typename ELEMENTS_06,
          typename ELEMENTS_07,
          typename ELEMENTS_08,
          typename ELEMENTS_09,
          typename ELEMENTS_10,
          typename ELEMENTS_11,
          typename ELEMENTS_12,
          typename ELEMENTS_13,
          typename ELEMENTS_14>
inline
bdld::Datum DatumMaker::a(
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_01) elements_01,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_02) elements_02,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_03) elements_03,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_04) elements_04,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_05) elements_05,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_06) elements_06,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_07) elements_07,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_08) elements_08,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_09) elements_09,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_10) elements_10,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_11) elements_11,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_12) elements_12,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_13) elements_13,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_14) elements_14
                        ) const
{
    const int numElements = 14u;
    bdld::DatumArrayBuilder builder(numElements, d_allocator_p);
    pushBackHelper(&builder, elements_01,
                             elements_02,
                             elements_03,
                             elements_04,
                             elements_05,
                             elements_06,
                             elements_07,
                             elements_08,
                             elements_09,
                             elements_10,
                             elements_11,
                             elements_12,
                             elements_13,
                             elements_14);
    return builder.commit();
}

template <typename ELEMENTS_01,
          typename ELEMENTS_02,
          typename ELEMENTS_03,
          typename ELEMENTS_04,
          typename ELEMENTS_05,
          typename ELEMENTS_06,
          typename ELEMENTS_07,
          typename ELEMENTS_08,
          typename ELEMENTS_09,
          typename ELEMENTS_10,
          typename ELEMENTS_11,
          typename ELEMENTS_12,
          typename ELEMENTS_13,
          typename ELEMENTS_14,
          typename ELEMENTS_15>
inline
bdld::Datum DatumMaker::a(
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_01) elements_01,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_02) elements_02,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_03) elements_03,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_04) elements_04,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_05) elements_05,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_06) elements_06,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_07) elements_07,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_08) elements_08,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_09) elements_09,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_10) elements_10,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_11) elements_11,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_12) elements_12,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_13) elements_13,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_14) elements_14,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_15) elements_15
                        ) const
{
    const int numElements = 15u;
    bdld::DatumArrayBuilder builder(numElements, d_allocator_p);
    pushBackHelper(&builder, elements_01,
                             elements_02,
                             elements_03,
                             elements_04,
                             elements_05,
                             elements_06,
                             elements_07,
                             elements_08,
                             elements_09,
                             elements_10,
                             elements_11,
                             elements_12,
                             elements_13,
                             elements_14,
                             elements_15);
    return builder.commit();
}

template <typename ELEMENTS_01,
          typename ELEMENTS_02,
          typename ELEMENTS_03,
          typename ELEMENTS_04,
          typename ELEMENTS_05,
          typename ELEMENTS_06,
          typename ELEMENTS_07,
          typename ELEMENTS_08,
          typename ELEMENTS_09,
          typename ELEMENTS_10,
          typename ELEMENTS_11,
          typename ELEMENTS_12,
          typename ELEMENTS_13,
          typename ELEMENTS_14,
          typename ELEMENTS_15,
          typename ELEMENTS_16>
inline
bdld::Datum DatumMaker::a(
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_01) elements_01,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_02) elements_02,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_03) elements_03,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_04) elements_04,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_05) elements_05,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_06) elements_06,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_07) elements_07,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_08) elements_08,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_09) elements_09,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_10) elements_10,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_11) elements_11,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_12) elements_12,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_13) elements_13,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_14) elements_14,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_15) elements_15,
                    BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS_16) elements_16
                        ) const
{
    const int numElements = 16u;
    bdld::DatumArrayBuilder builder(numElements, d_allocator_p);
    pushBackHelper(&builder, elements_01,
                             elements_02,
                             elements_03,
                             elements_04,
                             elements_05,
                             elements_06,
                             elements_07,
                             elements_08,
                             elements_09,
                             elements_10,
                             elements_11,
                             elements_12,
                             elements_13,
                             elements_14,
                             elements_15,
                             elements_16);
    return builder.commit();
}

#else
// The generated code below is a workaround for the absence of perfect
// forwarding in some compilers.
template <typename... ELEMENTS>
inline
bdld::Datum DatumMaker::a(
                        BSLS_COMPILERFEATURES_FORWARD_REF(ELEMENTS)... elements
                        ) const
{
    const int numElements = sizeof...(ELEMENTS);
    bdld::DatumArrayBuilder builder(numElements, d_allocator_p);
    pushBackHelper(&builder, elements...);
    return builder.commit();
}
#endif

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
template <typename... ENTRIES>
inline
bdld::Datum DatumMaker::m(ENTRIES&&... entries) const
{
    const int numArguments = sizeof...(ENTRIES);

    // Due to MSVC not recognizing bitwise and of a constant expression and a
    // string literal as a constant expression, we don't use a meaningful error
    // as part of this assert.
    //
    // See: https://connect.microsoft.com/VisualStudio/feedback/details/1523001
    BSLMF_ASSERT(0 == numArguments % 2);

    const int mapElements = numArguments / 2;
    bdld::DatumMapBuilder builder(mapElements, d_allocator_p);
    pushBackHelper(&builder, entries...);
    return builder.commit();
}
#elif BSLS_COMPILERFEATURES_SIMULATE_VARIADIC_TEMPLATES
inline
bdld::Datum DatumMaker::m(
                          ) const
{
    const int numArguments =  0u;

    BSLMF_ASSERT(0 == numArguments % 2);

    const int mapElements = numArguments / 2;
    bdld::DatumMapBuilder builder(mapElements, d_allocator_p);
    pushBackHelper(&builder);
    return builder.commit();
}

template <typename ENTRIES_01,
          typename ENTRIES_02>
inline
bdld::Datum DatumMaker::m(
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_01) entries_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_02) entries_02
                          ) const
{
    const int numArguments =  2u;

    BSLMF_ASSERT(0 == numArguments % 2);

    const int mapElements = numArguments / 2;
    bdld::DatumMapBuilder builder(mapElements, d_allocator_p);
    pushBackHelper(&builder, entries_01,
                             entries_02);
    return builder.commit();
}

template <typename ENTRIES_01,
          typename ENTRIES_02,
          typename ENTRIES_03,
          typename ENTRIES_04>
inline
bdld::Datum DatumMaker::m(
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_01) entries_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_02) entries_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_03) entries_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_04) entries_04
                          ) const
{
    const int numArguments =  4u;

    BSLMF_ASSERT(0 == numArguments % 2);

    const int mapElements = numArguments / 2;
    bdld::DatumMapBuilder builder(mapElements, d_allocator_p);
    pushBackHelper(&builder, entries_01,
                             entries_02,
                             entries_03,
                             entries_04);
    return builder.commit();
}

template <typename ENTRIES_01,
          typename ENTRIES_02,
          typename ENTRIES_03,
          typename ENTRIES_04,
          typename ENTRIES_05,
          typename ENTRIES_06>
inline
bdld::Datum DatumMaker::m(
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_01) entries_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_02) entries_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_03) entries_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_04) entries_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_05) entries_05,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_06) entries_06
                          ) const
{
    const int numArguments =  6u;

    BSLMF_ASSERT(0 == numArguments % 2);

    const int mapElements = numArguments / 2;
    bdld::DatumMapBuilder builder(mapElements, d_allocator_p);
    pushBackHelper(&builder, entries_01,
                             entries_02,
                             entries_03,
                             entries_04,
                             entries_05,
                             entries_06);
    return builder.commit();
}

template <typename ENTRIES_01,
          typename ENTRIES_02,
          typename ENTRIES_03,
          typename ENTRIES_04,
          typename ENTRIES_05,
          typename ENTRIES_06,
          typename ENTRIES_07,
          typename ENTRIES_08>
inline
bdld::Datum DatumMaker::m(
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_01) entries_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_02) entries_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_03) entries_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_04) entries_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_05) entries_05,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_06) entries_06,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_07) entries_07,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_08) entries_08
                          ) const
{
    const int numArguments =  8u;

    BSLMF_ASSERT(0 == numArguments % 2);

    const int mapElements = numArguments / 2;
    bdld::DatumMapBuilder builder(mapElements, d_allocator_p);
    pushBackHelper(&builder, entries_01,
                             entries_02,
                             entries_03,
                             entries_04,
                             entries_05,
                             entries_06,
                             entries_07,
                             entries_08);
    return builder.commit();
}

template <typename ENTRIES_01,
          typename ENTRIES_02,
          typename ENTRIES_03,
          typename ENTRIES_04,
          typename ENTRIES_05,
          typename ENTRIES_06,
          typename ENTRIES_07,
          typename ENTRIES_08,
          typename ENTRIES_09,
          typename ENTRIES_10>
inline
bdld::Datum DatumMaker::m(
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_01) entries_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_02) entries_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_03) entries_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_04) entries_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_05) entries_05,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_06) entries_06,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_07) entries_07,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_08) entries_08,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_09) entries_09,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_10) entries_10
                          ) const
{
    const int numArguments = 10u;

    BSLMF_ASSERT(0 == numArguments % 2);

    const int mapElements = numArguments / 2;
    bdld::DatumMapBuilder builder(mapElements, d_allocator_p);
    pushBackHelper(&builder, entries_01,
                             entries_02,
                             entries_03,
                             entries_04,
                             entries_05,
                             entries_06,
                             entries_07,
                             entries_08,
                             entries_09,
                             entries_10);
    return builder.commit();
}

template <typename ENTRIES_01,
          typename ENTRIES_02,
          typename ENTRIES_03,
          typename ENTRIES_04,
          typename ENTRIES_05,
          typename ENTRIES_06,
          typename ENTRIES_07,
          typename ENTRIES_08,
          typename ENTRIES_09,
          typename ENTRIES_10,
          typename ENTRIES_11,
          typename ENTRIES_12>
inline
bdld::Datum DatumMaker::m(
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_01) entries_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_02) entries_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_03) entries_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_04) entries_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_05) entries_05,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_06) entries_06,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_07) entries_07,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_08) entries_08,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_09) entries_09,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_10) entries_10,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_11) entries_11,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_12) entries_12
                          ) const
{
    const int numArguments = 12u;

    BSLMF_ASSERT(0 == numArguments % 2);

    const int mapElements = numArguments / 2;
    bdld::DatumMapBuilder builder(mapElements, d_allocator_p);
    pushBackHelper(&builder, entries_01,
                             entries_02,
                             entries_03,
                             entries_04,
                             entries_05,
                             entries_06,
                             entries_07,
                             entries_08,
                             entries_09,
                             entries_10,
                             entries_11,
                             entries_12);
    return builder.commit();
}

template <typename ENTRIES_01,
          typename ENTRIES_02,
          typename ENTRIES_03,
          typename ENTRIES_04,
          typename ENTRIES_05,
          typename ENTRIES_06,
          typename ENTRIES_07,
          typename ENTRIES_08,
          typename ENTRIES_09,
          typename ENTRIES_10,
          typename ENTRIES_11,
          typename ENTRIES_12,
          typename ENTRIES_13,
          typename ENTRIES_14>
inline
bdld::Datum DatumMaker::m(
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_01) entries_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_02) entries_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_03) entries_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_04) entries_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_05) entries_05,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_06) entries_06,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_07) entries_07,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_08) entries_08,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_09) entries_09,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_10) entries_10,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_11) entries_11,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_12) entries_12,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_13) entries_13,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_14) entries_14
                          ) const
{
    const int numArguments = 14u;

    BSLMF_ASSERT(0 == numArguments % 2);

    const int mapElements = numArguments / 2;
    bdld::DatumMapBuilder builder(mapElements, d_allocator_p);
    pushBackHelper(&builder, entries_01,
                             entries_02,
                             entries_03,
                             entries_04,
                             entries_05,
                             entries_06,
                             entries_07,
                             entries_08,
                             entries_09,
                             entries_10,
                             entries_11,
                             entries_12,
                             entries_13,
                             entries_14);
    return builder.commit();
}

template <typename ENTRIES_01,
          typename ENTRIES_02,
          typename ENTRIES_03,
          typename ENTRIES_04,
          typename ENTRIES_05,
          typename ENTRIES_06,
          typename ENTRIES_07,
          typename ENTRIES_08,
          typename ENTRIES_09,
          typename ENTRIES_10,
          typename ENTRIES_11,
          typename ENTRIES_12,
          typename ENTRIES_13,
          typename ENTRIES_14,
          typename ENTRIES_15,
          typename ENTRIES_16>
inline
bdld::Datum DatumMaker::m(
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_01) entries_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_02) entries_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_03) entries_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_04) entries_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_05) entries_05,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_06) entries_06,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_07) entries_07,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_08) entries_08,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_09) entries_09,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_10) entries_10,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_11) entries_11,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_12) entries_12,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_13) entries_13,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_14) entries_14,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_15) entries_15,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_16) entries_16
                          ) const
{
    const int numArguments = 16u;

    BSLMF_ASSERT(0 == numArguments % 2);

    const int mapElements = numArguments / 2;
    bdld::DatumMapBuilder builder(mapElements, d_allocator_p);
    pushBackHelper(&builder, entries_01,
                             entries_02,
                             entries_03,
                             entries_04,
                             entries_05,
                             entries_06,
                             entries_07,
                             entries_08,
                             entries_09,
                             entries_10,
                             entries_11,
                             entries_12,
                             entries_13,
                             entries_14,
                             entries_15,
                             entries_16);
    return builder.commit();
}

template <typename ENTRIES_01,
          typename ENTRIES_02,
          typename ENTRIES_03,
          typename ENTRIES_04,
          typename ENTRIES_05,
          typename ENTRIES_06,
          typename ENTRIES_07,
          typename ENTRIES_08,
          typename ENTRIES_09,
          typename ENTRIES_10,
          typename ENTRIES_11,
          typename ENTRIES_12,
          typename ENTRIES_13,
          typename ENTRIES_14,
          typename ENTRIES_15,
          typename ENTRIES_16,
          typename ENTRIES_17,
          typename ENTRIES_18>
inline
bdld::Datum DatumMaker::m(
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_01) entries_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_02) entries_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_03) entries_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_04) entries_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_05) entries_05,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_06) entries_06,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_07) entries_07,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_08) entries_08,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_09) entries_09,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_10) entries_10,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_11) entries_11,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_12) entries_12,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_13) entries_13,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_14) entries_14,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_15) entries_15,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_16) entries_16,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_17) entries_17,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_18) entries_18
                          ) const
{
    const int numArguments = 18u;

    BSLMF_ASSERT(0 == numArguments % 2);

    const int mapElements = numArguments / 2;
    bdld::DatumMapBuilder builder(mapElements, d_allocator_p);
    pushBackHelper(&builder, entries_01,
                             entries_02,
                             entries_03,
                             entries_04,
                             entries_05,
                             entries_06,
                             entries_07,
                             entries_08,
                             entries_09,
                             entries_10,
                             entries_11,
                             entries_12,
                             entries_13,
                             entries_14,
                             entries_15,
                             entries_16,
                             entries_17,
                             entries_18);
    return builder.commit();
}

template <typename ENTRIES_01,
          typename ENTRIES_02,
          typename ENTRIES_03,
          typename ENTRIES_04,
          typename ENTRIES_05,
          typename ENTRIES_06,
          typename ENTRIES_07,
          typename ENTRIES_08,
          typename ENTRIES_09,
          typename ENTRIES_10,
          typename ENTRIES_11,
          typename ENTRIES_12,
          typename ENTRIES_13,
          typename ENTRIES_14,
          typename ENTRIES_15,
          typename ENTRIES_16,
          typename ENTRIES_17,
          typename ENTRIES_18,
          typename ENTRIES_19,
          typename ENTRIES_20>
inline
bdld::Datum DatumMaker::m(
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_01) entries_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_02) entries_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_03) entries_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_04) entries_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_05) entries_05,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_06) entries_06,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_07) entries_07,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_08) entries_08,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_09) entries_09,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_10) entries_10,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_11) entries_11,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_12) entries_12,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_13) entries_13,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_14) entries_14,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_15) entries_15,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_16) entries_16,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_17) entries_17,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_18) entries_18,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_19) entries_19,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_20) entries_20
                          ) const
{
    const int numArguments = 20u;

    BSLMF_ASSERT(0 == numArguments % 2);

    const int mapElements = numArguments / 2;
    bdld::DatumMapBuilder builder(mapElements, d_allocator_p);
    pushBackHelper(&builder, entries_01,
                             entries_02,
                             entries_03,
                             entries_04,
                             entries_05,
                             entries_06,
                             entries_07,
                             entries_08,
                             entries_09,
                             entries_10,
                             entries_11,
                             entries_12,
                             entries_13,
                             entries_14,
                             entries_15,
                             entries_16,
                             entries_17,
                             entries_18,
                             entries_19,
                             entries_20);
    return builder.commit();
}

template <typename ENTRIES_01,
          typename ENTRIES_02,
          typename ENTRIES_03,
          typename ENTRIES_04,
          typename ENTRIES_05,
          typename ENTRIES_06,
          typename ENTRIES_07,
          typename ENTRIES_08,
          typename ENTRIES_09,
          typename ENTRIES_10,
          typename ENTRIES_11,
          typename ENTRIES_12,
          typename ENTRIES_13,
          typename ENTRIES_14,
          typename ENTRIES_15,
          typename ENTRIES_16,
          typename ENTRIES_17,
          typename ENTRIES_18,
          typename ENTRIES_19,
          typename ENTRIES_20,
          typename ENTRIES_21,
          typename ENTRIES_22>
inline
bdld::Datum DatumMaker::m(
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_01) entries_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_02) entries_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_03) entries_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_04) entries_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_05) entries_05,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_06) entries_06,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_07) entries_07,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_08) entries_08,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_09) entries_09,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_10) entries_10,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_11) entries_11,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_12) entries_12,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_13) entries_13,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_14) entries_14,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_15) entries_15,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_16) entries_16,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_17) entries_17,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_18) entries_18,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_19) entries_19,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_20) entries_20,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_21) entries_21,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_22) entries_22
                          ) const
{
    const int numArguments = 22u;

    BSLMF_ASSERT(0 == numArguments % 2);

    const int mapElements = numArguments / 2;
    bdld::DatumMapBuilder builder(mapElements, d_allocator_p);
    pushBackHelper(&builder, entries_01,
                             entries_02,
                             entries_03,
                             entries_04,
                             entries_05,
                             entries_06,
                             entries_07,
                             entries_08,
                             entries_09,
                             entries_10,
                             entries_11,
                             entries_12,
                             entries_13,
                             entries_14,
                             entries_15,
                             entries_16,
                             entries_17,
                             entries_18,
                             entries_19,
                             entries_20,
                             entries_21,
                             entries_22);
    return builder.commit();
}

template <typename ENTRIES_01,
          typename ENTRIES_02,
          typename ENTRIES_03,
          typename ENTRIES_04,
          typename ENTRIES_05,
          typename ENTRIES_06,
          typename ENTRIES_07,
          typename ENTRIES_08,
          typename ENTRIES_09,
          typename ENTRIES_10,
          typename ENTRIES_11,
          typename ENTRIES_12,
          typename ENTRIES_13,
          typename ENTRIES_14,
          typename ENTRIES_15,
          typename ENTRIES_16,
          typename ENTRIES_17,
          typename ENTRIES_18,
          typename ENTRIES_19,
          typename ENTRIES_20,
          typename ENTRIES_21,
          typename ENTRIES_22,
          typename ENTRIES_23,
          typename ENTRIES_24>
inline
bdld::Datum DatumMaker::m(
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_01) entries_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_02) entries_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_03) entries_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_04) entries_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_05) entries_05,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_06) entries_06,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_07) entries_07,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_08) entries_08,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_09) entries_09,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_10) entries_10,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_11) entries_11,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_12) entries_12,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_13) entries_13,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_14) entries_14,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_15) entries_15,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_16) entries_16,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_17) entries_17,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_18) entries_18,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_19) entries_19,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_20) entries_20,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_21) entries_21,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_22) entries_22,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_23) entries_23,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_24) entries_24
                          ) const
{
    const int numArguments = 24u;

    BSLMF_ASSERT(0 == numArguments % 2);

    const int mapElements = numArguments / 2;
    bdld::DatumMapBuilder builder(mapElements, d_allocator_p);
    pushBackHelper(&builder, entries_01,
                             entries_02,
                             entries_03,
                             entries_04,
                             entries_05,
                             entries_06,
                             entries_07,
                             entries_08,
                             entries_09,
                             entries_10,
                             entries_11,
                             entries_12,
                             entries_13,
                             entries_14,
                             entries_15,
                             entries_16,
                             entries_17,
                             entries_18,
                             entries_19,
                             entries_20,
                             entries_21,
                             entries_22,
                             entries_23,
                             entries_24);
    return builder.commit();
}

template <typename ENTRIES_01,
          typename ENTRIES_02,
          typename ENTRIES_03,
          typename ENTRIES_04,
          typename ENTRIES_05,
          typename ENTRIES_06,
          typename ENTRIES_07,
          typename ENTRIES_08,
          typename ENTRIES_09,
          typename ENTRIES_10,
          typename ENTRIES_11,
          typename ENTRIES_12,
          typename ENTRIES_13,
          typename ENTRIES_14,
          typename ENTRIES_15,
          typename ENTRIES_16,
          typename ENTRIES_17,
          typename ENTRIES_18,
          typename ENTRIES_19,
          typename ENTRIES_20,
          typename ENTRIES_21,
          typename ENTRIES_22,
          typename ENTRIES_23,
          typename ENTRIES_24,
          typename ENTRIES_25,
          typename ENTRIES_26>
inline
bdld::Datum DatumMaker::m(
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_01) entries_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_02) entries_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_03) entries_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_04) entries_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_05) entries_05,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_06) entries_06,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_07) entries_07,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_08) entries_08,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_09) entries_09,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_10) entries_10,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_11) entries_11,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_12) entries_12,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_13) entries_13,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_14) entries_14,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_15) entries_15,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_16) entries_16,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_17) entries_17,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_18) entries_18,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_19) entries_19,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_20) entries_20,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_21) entries_21,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_22) entries_22,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_23) entries_23,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_24) entries_24,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_25) entries_25,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_26) entries_26
                          ) const
{
    const int numArguments = 26u;

    BSLMF_ASSERT(0 == numArguments % 2);

    const int mapElements = numArguments / 2;
    bdld::DatumMapBuilder builder(mapElements, d_allocator_p);
    pushBackHelper(&builder, entries_01,
                             entries_02,
                             entries_03,
                             entries_04,
                             entries_05,
                             entries_06,
                             entries_07,
                             entries_08,
                             entries_09,
                             entries_10,
                             entries_11,
                             entries_12,
                             entries_13,
                             entries_14,
                             entries_15,
                             entries_16,
                             entries_17,
                             entries_18,
                             entries_19,
                             entries_20,
                             entries_21,
                             entries_22,
                             entries_23,
                             entries_24,
                             entries_25,
                             entries_26);
    return builder.commit();
}

template <typename ENTRIES_01,
          typename ENTRIES_02,
          typename ENTRIES_03,
          typename ENTRIES_04,
          typename ENTRIES_05,
          typename ENTRIES_06,
          typename ENTRIES_07,
          typename ENTRIES_08,
          typename ENTRIES_09,
          typename ENTRIES_10,
          typename ENTRIES_11,
          typename ENTRIES_12,
          typename ENTRIES_13,
          typename ENTRIES_14,
          typename ENTRIES_15,
          typename ENTRIES_16,
          typename ENTRIES_17,
          typename ENTRIES_18,
          typename ENTRIES_19,
          typename ENTRIES_20,
          typename ENTRIES_21,
          typename ENTRIES_22,
          typename ENTRIES_23,
          typename ENTRIES_24,
          typename ENTRIES_25,
          typename ENTRIES_26,
          typename ENTRIES_27,
          typename ENTRIES_28>
inline
bdld::Datum DatumMaker::m(
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_01) entries_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_02) entries_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_03) entries_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_04) entries_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_05) entries_05,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_06) entries_06,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_07) entries_07,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_08) entries_08,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_09) entries_09,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_10) entries_10,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_11) entries_11,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_12) entries_12,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_13) entries_13,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_14) entries_14,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_15) entries_15,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_16) entries_16,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_17) entries_17,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_18) entries_18,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_19) entries_19,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_20) entries_20,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_21) entries_21,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_22) entries_22,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_23) entries_23,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_24) entries_24,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_25) entries_25,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_26) entries_26,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_27) entries_27,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_28) entries_28
                          ) const
{
    const int numArguments = 28u;

    BSLMF_ASSERT(0 == numArguments % 2);

    const int mapElements = numArguments / 2;
    bdld::DatumMapBuilder builder(mapElements, d_allocator_p);
    pushBackHelper(&builder, entries_01,
                             entries_02,
                             entries_03,
                             entries_04,
                             entries_05,
                             entries_06,
                             entries_07,
                             entries_08,
                             entries_09,
                             entries_10,
                             entries_11,
                             entries_12,
                             entries_13,
                             entries_14,
                             entries_15,
                             entries_16,
                             entries_17,
                             entries_18,
                             entries_19,
                             entries_20,
                             entries_21,
                             entries_22,
                             entries_23,
                             entries_24,
                             entries_25,
                             entries_26,
                             entries_27,
                             entries_28);
    return builder.commit();
}

template <typename ENTRIES_01,
          typename ENTRIES_02,
          typename ENTRIES_03,
          typename ENTRIES_04,
          typename ENTRIES_05,
          typename ENTRIES_06,
          typename ENTRIES_07,
          typename ENTRIES_08,
          typename ENTRIES_09,
          typename ENTRIES_10,
          typename ENTRIES_11,
          typename ENTRIES_12,
          typename ENTRIES_13,
          typename ENTRIES_14,
          typename ENTRIES_15,
          typename ENTRIES_16,
          typename ENTRIES_17,
          typename ENTRIES_18,
          typename ENTRIES_19,
          typename ENTRIES_20,
          typename ENTRIES_21,
          typename ENTRIES_22,
          typename ENTRIES_23,
          typename ENTRIES_24,
          typename ENTRIES_25,
          typename ENTRIES_26,
          typename ENTRIES_27,
          typename ENTRIES_28,
          typename ENTRIES_29,
          typename ENTRIES_30>
inline
bdld::Datum DatumMaker::m(
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_01) entries_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_02) entries_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_03) entries_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_04) entries_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_05) entries_05,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_06) entries_06,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_07) entries_07,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_08) entries_08,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_09) entries_09,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_10) entries_10,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_11) entries_11,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_12) entries_12,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_13) entries_13,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_14) entries_14,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_15) entries_15,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_16) entries_16,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_17) entries_17,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_18) entries_18,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_19) entries_19,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_20) entries_20,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_21) entries_21,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_22) entries_22,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_23) entries_23,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_24) entries_24,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_25) entries_25,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_26) entries_26,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_27) entries_27,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_28) entries_28,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_29) entries_29,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_30) entries_30
                          ) const
{
    const int numArguments = 30u;

    BSLMF_ASSERT(0 == numArguments % 2);

    const int mapElements = numArguments / 2;
    bdld::DatumMapBuilder builder(mapElements, d_allocator_p);
    pushBackHelper(&builder, entries_01,
                             entries_02,
                             entries_03,
                             entries_04,
                             entries_05,
                             entries_06,
                             entries_07,
                             entries_08,
                             entries_09,
                             entries_10,
                             entries_11,
                             entries_12,
                             entries_13,
                             entries_14,
                             entries_15,
                             entries_16,
                             entries_17,
                             entries_18,
                             entries_19,
                             entries_20,
                             entries_21,
                             entries_22,
                             entries_23,
                             entries_24,
                             entries_25,
                             entries_26,
                             entries_27,
                             entries_28,
                             entries_29,
                             entries_30);
    return builder.commit();
}

template <typename ENTRIES_01,
          typename ENTRIES_02,
          typename ENTRIES_03,
          typename ENTRIES_04,
          typename ENTRIES_05,
          typename ENTRIES_06,
          typename ENTRIES_07,
          typename ENTRIES_08,
          typename ENTRIES_09,
          typename ENTRIES_10,
          typename ENTRIES_11,
          typename ENTRIES_12,
          typename ENTRIES_13,
          typename ENTRIES_14,
          typename ENTRIES_15,
          typename ENTRIES_16,
          typename ENTRIES_17,
          typename ENTRIES_18,
          typename ENTRIES_19,
          typename ENTRIES_20,
          typename ENTRIES_21,
          typename ENTRIES_22,
          typename ENTRIES_23,
          typename ENTRIES_24,
          typename ENTRIES_25,
          typename ENTRIES_26,
          typename ENTRIES_27,
          typename ENTRIES_28,
          typename ENTRIES_29,
          typename ENTRIES_30,
          typename ENTRIES_31,
          typename ENTRIES_32>
inline
bdld::Datum DatumMaker::m(
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_01) entries_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_02) entries_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_03) entries_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_04) entries_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_05) entries_05,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_06) entries_06,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_07) entries_07,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_08) entries_08,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_09) entries_09,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_10) entries_10,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_11) entries_11,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_12) entries_12,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_13) entries_13,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_14) entries_14,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_15) entries_15,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_16) entries_16,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_17) entries_17,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_18) entries_18,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_19) entries_19,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_20) entries_20,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_21) entries_21,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_22) entries_22,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_23) entries_23,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_24) entries_24,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_25) entries_25,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_26) entries_26,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_27) entries_27,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_28) entries_28,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_29) entries_29,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_30) entries_30,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_31) entries_31,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_32) entries_32
                          ) const
{
    const int numArguments = 32u;

    BSLMF_ASSERT(0 == numArguments % 2);

    const int mapElements = numArguments / 2;
    bdld::DatumMapBuilder builder(mapElements, d_allocator_p);
    pushBackHelper(&builder, entries_01,
                             entries_02,
                             entries_03,
                             entries_04,
                             entries_05,
                             entries_06,
                             entries_07,
                             entries_08,
                             entries_09,
                             entries_10,
                             entries_11,
                             entries_12,
                             entries_13,
                             entries_14,
                             entries_15,
                             entries_16,
                             entries_17,
                             entries_18,
                             entries_19,
                             entries_20,
                             entries_21,
                             entries_22,
                             entries_23,
                             entries_24,
                             entries_25,
                             entries_26,
                             entries_27,
                             entries_28,
                             entries_29,
                             entries_30,
                             entries_31,
                             entries_32);
    return builder.commit();
}

#else
// The generated code below is a workaround for the absence of perfect
// forwarding in some compilers.
template <typename... ENTRIES>
inline
bdld::Datum DatumMaker::m(BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES)... entries
                          ) const
{
    const int numArguments = sizeof...(ENTRIES);

    BSLMF_ASSERT(0 == numArguments % 2);

    const int mapElements = numArguments / 2;
    bdld::DatumMapBuilder builder(mapElements, d_allocator_p);
    pushBackHelper(&builder, entries...);
    return builder.commit();
}
#endif

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
template <typename... ENTRIES>
inline
bdld::Datum DatumMaker::mok(ENTRIES&&... entries) const
{
    const int numArguments = sizeof...(ENTRIES);

    // Due to MSVC not recognizing bitwise and of a constant expression and a
    // string literal as a constant expression, we don't use a meaningful error
    // as part of this assert.
    //
    // See: https://connect.microsoft.com/VisualStudio/feedback/details/1523001
    BSLMF_ASSERT(0 == numArguments % 2);

    bdld::DatumMapOwningKeysBuilder builder(d_allocator_p);
    pushBackHelper(&builder, entries...);
    return builder.commit();
}
#elif BSLS_COMPILERFEATURES_SIMULATE_VARIADIC_TEMPLATES
inline
bdld::Datum DatumMaker::mok(
                          ) const
{
    const int numArguments =  0u;

    BSLMF_ASSERT(0 == numArguments % 2);

    bdld::DatumMapOwningKeysBuilder builder(d_allocator_p);
    pushBackHelper(&builder);
    return builder.commit();
}

template <typename ENTRIES_01,
          typename ENTRIES_02>
inline
bdld::Datum DatumMaker::mok(
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_01) entries_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_02) entries_02
                          ) const
{
    const int numArguments =  2u;

    BSLMF_ASSERT(0 == numArguments % 2);

    bdld::DatumMapOwningKeysBuilder builder(d_allocator_p);
    pushBackHelper(&builder, entries_01,
                             entries_02);
    return builder.commit();
}

template <typename ENTRIES_01,
          typename ENTRIES_02,
          typename ENTRIES_03,
          typename ENTRIES_04>
inline
bdld::Datum DatumMaker::mok(
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_01) entries_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_02) entries_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_03) entries_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_04) entries_04
                          ) const
{
    const int numArguments =  4u;

    BSLMF_ASSERT(0 == numArguments % 2);

    bdld::DatumMapOwningKeysBuilder builder(d_allocator_p);
    pushBackHelper(&builder, entries_01,
                             entries_02,
                             entries_03,
                             entries_04);
    return builder.commit();
}

template <typename ENTRIES_01,
          typename ENTRIES_02,
          typename ENTRIES_03,
          typename ENTRIES_04,
          typename ENTRIES_05,
          typename ENTRIES_06>
inline
bdld::Datum DatumMaker::mok(
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_01) entries_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_02) entries_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_03) entries_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_04) entries_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_05) entries_05,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_06) entries_06
                          ) const
{
    const int numArguments =  6u;

    BSLMF_ASSERT(0 == numArguments % 2);

    bdld::DatumMapOwningKeysBuilder builder(d_allocator_p);
    pushBackHelper(&builder, entries_01,
                             entries_02,
                             entries_03,
                             entries_04,
                             entries_05,
                             entries_06);
    return builder.commit();
}

template <typename ENTRIES_01,
          typename ENTRIES_02,
          typename ENTRIES_03,
          typename ENTRIES_04,
          typename ENTRIES_05,
          typename ENTRIES_06,
          typename ENTRIES_07,
          typename ENTRIES_08>
inline
bdld::Datum DatumMaker::mok(
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_01) entries_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_02) entries_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_03) entries_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_04) entries_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_05) entries_05,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_06) entries_06,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_07) entries_07,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_08) entries_08
                          ) const
{
    const int numArguments =  8u;

    BSLMF_ASSERT(0 == numArguments % 2);

    bdld::DatumMapOwningKeysBuilder builder(d_allocator_p);
    pushBackHelper(&builder, entries_01,
                             entries_02,
                             entries_03,
                             entries_04,
                             entries_05,
                             entries_06,
                             entries_07,
                             entries_08);
    return builder.commit();
}

template <typename ENTRIES_01,
          typename ENTRIES_02,
          typename ENTRIES_03,
          typename ENTRIES_04,
          typename ENTRIES_05,
          typename ENTRIES_06,
          typename ENTRIES_07,
          typename ENTRIES_08,
          typename ENTRIES_09,
          typename ENTRIES_10>
inline
bdld::Datum DatumMaker::mok(
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_01) entries_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_02) entries_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_03) entries_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_04) entries_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_05) entries_05,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_06) entries_06,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_07) entries_07,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_08) entries_08,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_09) entries_09,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_10) entries_10
                          ) const
{
    const int numArguments = 10u;

    BSLMF_ASSERT(0 == numArguments % 2);

    bdld::DatumMapOwningKeysBuilder builder(d_allocator_p);
    pushBackHelper(&builder, entries_01,
                             entries_02,
                             entries_03,
                             entries_04,
                             entries_05,
                             entries_06,
                             entries_07,
                             entries_08,
                             entries_09,
                             entries_10);
    return builder.commit();
}

template <typename ENTRIES_01,
          typename ENTRIES_02,
          typename ENTRIES_03,
          typename ENTRIES_04,
          typename ENTRIES_05,
          typename ENTRIES_06,
          typename ENTRIES_07,
          typename ENTRIES_08,
          typename ENTRIES_09,
          typename ENTRIES_10,
          typename ENTRIES_11,
          typename ENTRIES_12>
inline
bdld::Datum DatumMaker::mok(
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_01) entries_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_02) entries_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_03) entries_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_04) entries_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_05) entries_05,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_06) entries_06,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_07) entries_07,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_08) entries_08,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_09) entries_09,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_10) entries_10,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_11) entries_11,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_12) entries_12
                          ) const
{
    const int numArguments = 12u;

    BSLMF_ASSERT(0 == numArguments % 2);

    bdld::DatumMapOwningKeysBuilder builder(d_allocator_p);
    pushBackHelper(&builder, entries_01,
                             entries_02,
                             entries_03,
                             entries_04,
                             entries_05,
                             entries_06,
                             entries_07,
                             entries_08,
                             entries_09,
                             entries_10,
                             entries_11,
                             entries_12);
    return builder.commit();
}

template <typename ENTRIES_01,
          typename ENTRIES_02,
          typename ENTRIES_03,
          typename ENTRIES_04,
          typename ENTRIES_05,
          typename ENTRIES_06,
          typename ENTRIES_07,
          typename ENTRIES_08,
          typename ENTRIES_09,
          typename ENTRIES_10,
          typename ENTRIES_11,
          typename ENTRIES_12,
          typename ENTRIES_13,
          typename ENTRIES_14>
inline
bdld::Datum DatumMaker::mok(
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_01) entries_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_02) entries_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_03) entries_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_04) entries_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_05) entries_05,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_06) entries_06,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_07) entries_07,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_08) entries_08,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_09) entries_09,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_10) entries_10,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_11) entries_11,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_12) entries_12,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_13) entries_13,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_14) entries_14
                          ) const
{
    const int numArguments = 14u;

    BSLMF_ASSERT(0 == numArguments % 2);

    bdld::DatumMapOwningKeysBuilder builder(d_allocator_p);
    pushBackHelper(&builder, entries_01,
                             entries_02,
                             entries_03,
                             entries_04,
                             entries_05,
                             entries_06,
                             entries_07,
                             entries_08,
                             entries_09,
                             entries_10,
                             entries_11,
                             entries_12,
                             entries_13,
                             entries_14);
    return builder.commit();
}

template <typename ENTRIES_01,
          typename ENTRIES_02,
          typename ENTRIES_03,
          typename ENTRIES_04,
          typename ENTRIES_05,
          typename ENTRIES_06,
          typename ENTRIES_07,
          typename ENTRIES_08,
          typename ENTRIES_09,
          typename ENTRIES_10,
          typename ENTRIES_11,
          typename ENTRIES_12,
          typename ENTRIES_13,
          typename ENTRIES_14,
          typename ENTRIES_15,
          typename ENTRIES_16>
inline
bdld::Datum DatumMaker::mok(
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_01) entries_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_02) entries_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_03) entries_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_04) entries_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_05) entries_05,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_06) entries_06,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_07) entries_07,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_08) entries_08,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_09) entries_09,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_10) entries_10,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_11) entries_11,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_12) entries_12,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_13) entries_13,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_14) entries_14,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_15) entries_15,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_16) entries_16
                          ) const
{
    const int numArguments = 16u;

    BSLMF_ASSERT(0 == numArguments % 2);

    bdld::DatumMapOwningKeysBuilder builder(d_allocator_p);
    pushBackHelper(&builder, entries_01,
                             entries_02,
                             entries_03,
                             entries_04,
                             entries_05,
                             entries_06,
                             entries_07,
                             entries_08,
                             entries_09,
                             entries_10,
                             entries_11,
                             entries_12,
                             entries_13,
                             entries_14,
                             entries_15,
                             entries_16);
    return builder.commit();
}

template <typename ENTRIES_01,
          typename ENTRIES_02,
          typename ENTRIES_03,
          typename ENTRIES_04,
          typename ENTRIES_05,
          typename ENTRIES_06,
          typename ENTRIES_07,
          typename ENTRIES_08,
          typename ENTRIES_09,
          typename ENTRIES_10,
          typename ENTRIES_11,
          typename ENTRIES_12,
          typename ENTRIES_13,
          typename ENTRIES_14,
          typename ENTRIES_15,
          typename ENTRIES_16,
          typename ENTRIES_17,
          typename ENTRIES_18>
inline
bdld::Datum DatumMaker::mok(
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_01) entries_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_02) entries_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_03) entries_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_04) entries_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_05) entries_05,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_06) entries_06,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_07) entries_07,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_08) entries_08,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_09) entries_09,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_10) entries_10,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_11) entries_11,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_12) entries_12,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_13) entries_13,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_14) entries_14,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_15) entries_15,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_16) entries_16,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_17) entries_17,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_18) entries_18
                          ) const
{
    const int numArguments = 18u;

    BSLMF_ASSERT(0 == numArguments % 2);

    bdld::DatumMapOwningKeysBuilder builder(d_allocator_p);
    pushBackHelper(&builder, entries_01,
                             entries_02,
                             entries_03,
                             entries_04,
                             entries_05,
                             entries_06,
                             entries_07,
                             entries_08,
                             entries_09,
                             entries_10,
                             entries_11,
                             entries_12,
                             entries_13,
                             entries_14,
                             entries_15,
                             entries_16,
                             entries_17,
                             entries_18);
    return builder.commit();
}

template <typename ENTRIES_01,
          typename ENTRIES_02,
          typename ENTRIES_03,
          typename ENTRIES_04,
          typename ENTRIES_05,
          typename ENTRIES_06,
          typename ENTRIES_07,
          typename ENTRIES_08,
          typename ENTRIES_09,
          typename ENTRIES_10,
          typename ENTRIES_11,
          typename ENTRIES_12,
          typename ENTRIES_13,
          typename ENTRIES_14,
          typename ENTRIES_15,
          typename ENTRIES_16,
          typename ENTRIES_17,
          typename ENTRIES_18,
          typename ENTRIES_19,
          typename ENTRIES_20>
inline
bdld::Datum DatumMaker::mok(
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_01) entries_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_02) entries_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_03) entries_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_04) entries_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_05) entries_05,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_06) entries_06,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_07) entries_07,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_08) entries_08,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_09) entries_09,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_10) entries_10,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_11) entries_11,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_12) entries_12,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_13) entries_13,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_14) entries_14,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_15) entries_15,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_16) entries_16,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_17) entries_17,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_18) entries_18,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_19) entries_19,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_20) entries_20
                          ) const
{
    const int numArguments = 20u;

    BSLMF_ASSERT(0 == numArguments % 2);

    bdld::DatumMapOwningKeysBuilder builder(d_allocator_p);
    pushBackHelper(&builder, entries_01,
                             entries_02,
                             entries_03,
                             entries_04,
                             entries_05,
                             entries_06,
                             entries_07,
                             entries_08,
                             entries_09,
                             entries_10,
                             entries_11,
                             entries_12,
                             entries_13,
                             entries_14,
                             entries_15,
                             entries_16,
                             entries_17,
                             entries_18,
                             entries_19,
                             entries_20);
    return builder.commit();
}

template <typename ENTRIES_01,
          typename ENTRIES_02,
          typename ENTRIES_03,
          typename ENTRIES_04,
          typename ENTRIES_05,
          typename ENTRIES_06,
          typename ENTRIES_07,
          typename ENTRIES_08,
          typename ENTRIES_09,
          typename ENTRIES_10,
          typename ENTRIES_11,
          typename ENTRIES_12,
          typename ENTRIES_13,
          typename ENTRIES_14,
          typename ENTRIES_15,
          typename ENTRIES_16,
          typename ENTRIES_17,
          typename ENTRIES_18,
          typename ENTRIES_19,
          typename ENTRIES_20,
          typename ENTRIES_21,
          typename ENTRIES_22>
inline
bdld::Datum DatumMaker::mok(
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_01) entries_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_02) entries_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_03) entries_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_04) entries_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_05) entries_05,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_06) entries_06,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_07) entries_07,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_08) entries_08,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_09) entries_09,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_10) entries_10,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_11) entries_11,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_12) entries_12,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_13) entries_13,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_14) entries_14,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_15) entries_15,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_16) entries_16,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_17) entries_17,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_18) entries_18,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_19) entries_19,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_20) entries_20,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_21) entries_21,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_22) entries_22
                          ) const
{
    const int numArguments = 22u;

    BSLMF_ASSERT(0 == numArguments % 2);

    bdld::DatumMapOwningKeysBuilder builder(d_allocator_p);
    pushBackHelper(&builder, entries_01,
                             entries_02,
                             entries_03,
                             entries_04,
                             entries_05,
                             entries_06,
                             entries_07,
                             entries_08,
                             entries_09,
                             entries_10,
                             entries_11,
                             entries_12,
                             entries_13,
                             entries_14,
                             entries_15,
                             entries_16,
                             entries_17,
                             entries_18,
                             entries_19,
                             entries_20,
                             entries_21,
                             entries_22);
    return builder.commit();
}

template <typename ENTRIES_01,
          typename ENTRIES_02,
          typename ENTRIES_03,
          typename ENTRIES_04,
          typename ENTRIES_05,
          typename ENTRIES_06,
          typename ENTRIES_07,
          typename ENTRIES_08,
          typename ENTRIES_09,
          typename ENTRIES_10,
          typename ENTRIES_11,
          typename ENTRIES_12,
          typename ENTRIES_13,
          typename ENTRIES_14,
          typename ENTRIES_15,
          typename ENTRIES_16,
          typename ENTRIES_17,
          typename ENTRIES_18,
          typename ENTRIES_19,
          typename ENTRIES_20,
          typename ENTRIES_21,
          typename ENTRIES_22,
          typename ENTRIES_23,
          typename ENTRIES_24>
inline
bdld::Datum DatumMaker::mok(
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_01) entries_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_02) entries_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_03) entries_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_04) entries_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_05) entries_05,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_06) entries_06,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_07) entries_07,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_08) entries_08,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_09) entries_09,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_10) entries_10,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_11) entries_11,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_12) entries_12,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_13) entries_13,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_14) entries_14,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_15) entries_15,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_16) entries_16,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_17) entries_17,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_18) entries_18,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_19) entries_19,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_20) entries_20,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_21) entries_21,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_22) entries_22,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_23) entries_23,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_24) entries_24
                          ) const
{
    const int numArguments = 24u;

    BSLMF_ASSERT(0 == numArguments % 2);

    bdld::DatumMapOwningKeysBuilder builder(d_allocator_p);
    pushBackHelper(&builder, entries_01,
                             entries_02,
                             entries_03,
                             entries_04,
                             entries_05,
                             entries_06,
                             entries_07,
                             entries_08,
                             entries_09,
                             entries_10,
                             entries_11,
                             entries_12,
                             entries_13,
                             entries_14,
                             entries_15,
                             entries_16,
                             entries_17,
                             entries_18,
                             entries_19,
                             entries_20,
                             entries_21,
                             entries_22,
                             entries_23,
                             entries_24);
    return builder.commit();
}

template <typename ENTRIES_01,
          typename ENTRIES_02,
          typename ENTRIES_03,
          typename ENTRIES_04,
          typename ENTRIES_05,
          typename ENTRIES_06,
          typename ENTRIES_07,
          typename ENTRIES_08,
          typename ENTRIES_09,
          typename ENTRIES_10,
          typename ENTRIES_11,
          typename ENTRIES_12,
          typename ENTRIES_13,
          typename ENTRIES_14,
          typename ENTRIES_15,
          typename ENTRIES_16,
          typename ENTRIES_17,
          typename ENTRIES_18,
          typename ENTRIES_19,
          typename ENTRIES_20,
          typename ENTRIES_21,
          typename ENTRIES_22,
          typename ENTRIES_23,
          typename ENTRIES_24,
          typename ENTRIES_25,
          typename ENTRIES_26>
inline
bdld::Datum DatumMaker::mok(
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_01) entries_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_02) entries_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_03) entries_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_04) entries_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_05) entries_05,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_06) entries_06,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_07) entries_07,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_08) entries_08,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_09) entries_09,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_10) entries_10,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_11) entries_11,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_12) entries_12,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_13) entries_13,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_14) entries_14,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_15) entries_15,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_16) entries_16,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_17) entries_17,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_18) entries_18,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_19) entries_19,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_20) entries_20,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_21) entries_21,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_22) entries_22,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_23) entries_23,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_24) entries_24,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_25) entries_25,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_26) entries_26
                          ) const
{
    const int numArguments = 26u;

    BSLMF_ASSERT(0 == numArguments % 2);

    bdld::DatumMapOwningKeysBuilder builder(d_allocator_p);
    pushBackHelper(&builder, entries_01,
                             entries_02,
                             entries_03,
                             entries_04,
                             entries_05,
                             entries_06,
                             entries_07,
                             entries_08,
                             entries_09,
                             entries_10,
                             entries_11,
                             entries_12,
                             entries_13,
                             entries_14,
                             entries_15,
                             entries_16,
                             entries_17,
                             entries_18,
                             entries_19,
                             entries_20,
                             entries_21,
                             entries_22,
                             entries_23,
                             entries_24,
                             entries_25,
                             entries_26);
    return builder.commit();
}

template <typename ENTRIES_01,
          typename ENTRIES_02,
          typename ENTRIES_03,
          typename ENTRIES_04,
          typename ENTRIES_05,
          typename ENTRIES_06,
          typename ENTRIES_07,
          typename ENTRIES_08,
          typename ENTRIES_09,
          typename ENTRIES_10,
          typename ENTRIES_11,
          typename ENTRIES_12,
          typename ENTRIES_13,
          typename ENTRIES_14,
          typename ENTRIES_15,
          typename ENTRIES_16,
          typename ENTRIES_17,
          typename ENTRIES_18,
          typename ENTRIES_19,
          typename ENTRIES_20,
          typename ENTRIES_21,
          typename ENTRIES_22,
          typename ENTRIES_23,
          typename ENTRIES_24,
          typename ENTRIES_25,
          typename ENTRIES_26,
          typename ENTRIES_27,
          typename ENTRIES_28>
inline
bdld::Datum DatumMaker::mok(
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_01) entries_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_02) entries_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_03) entries_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_04) entries_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_05) entries_05,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_06) entries_06,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_07) entries_07,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_08) entries_08,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_09) entries_09,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_10) entries_10,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_11) entries_11,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_12) entries_12,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_13) entries_13,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_14) entries_14,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_15) entries_15,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_16) entries_16,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_17) entries_17,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_18) entries_18,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_19) entries_19,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_20) entries_20,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_21) entries_21,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_22) entries_22,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_23) entries_23,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_24) entries_24,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_25) entries_25,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_26) entries_26,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_27) entries_27,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_28) entries_28
                          ) const
{
    const int numArguments = 28u;

    BSLMF_ASSERT(0 == numArguments % 2);

    bdld::DatumMapOwningKeysBuilder builder(d_allocator_p);
    pushBackHelper(&builder, entries_01,
                             entries_02,
                             entries_03,
                             entries_04,
                             entries_05,
                             entries_06,
                             entries_07,
                             entries_08,
                             entries_09,
                             entries_10,
                             entries_11,
                             entries_12,
                             entries_13,
                             entries_14,
                             entries_15,
                             entries_16,
                             entries_17,
                             entries_18,
                             entries_19,
                             entries_20,
                             entries_21,
                             entries_22,
                             entries_23,
                             entries_24,
                             entries_25,
                             entries_26,
                             entries_27,
                             entries_28);
    return builder.commit();
}

template <typename ENTRIES_01,
          typename ENTRIES_02,
          typename ENTRIES_03,
          typename ENTRIES_04,
          typename ENTRIES_05,
          typename ENTRIES_06,
          typename ENTRIES_07,
          typename ENTRIES_08,
          typename ENTRIES_09,
          typename ENTRIES_10,
          typename ENTRIES_11,
          typename ENTRIES_12,
          typename ENTRIES_13,
          typename ENTRIES_14,
          typename ENTRIES_15,
          typename ENTRIES_16,
          typename ENTRIES_17,
          typename ENTRIES_18,
          typename ENTRIES_19,
          typename ENTRIES_20,
          typename ENTRIES_21,
          typename ENTRIES_22,
          typename ENTRIES_23,
          typename ENTRIES_24,
          typename ENTRIES_25,
          typename ENTRIES_26,
          typename ENTRIES_27,
          typename ENTRIES_28,
          typename ENTRIES_29,
          typename ENTRIES_30>
inline
bdld::Datum DatumMaker::mok(
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_01) entries_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_02) entries_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_03) entries_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_04) entries_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_05) entries_05,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_06) entries_06,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_07) entries_07,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_08) entries_08,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_09) entries_09,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_10) entries_10,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_11) entries_11,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_12) entries_12,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_13) entries_13,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_14) entries_14,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_15) entries_15,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_16) entries_16,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_17) entries_17,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_18) entries_18,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_19) entries_19,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_20) entries_20,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_21) entries_21,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_22) entries_22,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_23) entries_23,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_24) entries_24,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_25) entries_25,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_26) entries_26,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_27) entries_27,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_28) entries_28,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_29) entries_29,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_30) entries_30
                          ) const
{
    const int numArguments = 30u;

    BSLMF_ASSERT(0 == numArguments % 2);

    bdld::DatumMapOwningKeysBuilder builder(d_allocator_p);
    pushBackHelper(&builder, entries_01,
                             entries_02,
                             entries_03,
                             entries_04,
                             entries_05,
                             entries_06,
                             entries_07,
                             entries_08,
                             entries_09,
                             entries_10,
                             entries_11,
                             entries_12,
                             entries_13,
                             entries_14,
                             entries_15,
                             entries_16,
                             entries_17,
                             entries_18,
                             entries_19,
                             entries_20,
                             entries_21,
                             entries_22,
                             entries_23,
                             entries_24,
                             entries_25,
                             entries_26,
                             entries_27,
                             entries_28,
                             entries_29,
                             entries_30);
    return builder.commit();
}

template <typename ENTRIES_01,
          typename ENTRIES_02,
          typename ENTRIES_03,
          typename ENTRIES_04,
          typename ENTRIES_05,
          typename ENTRIES_06,
          typename ENTRIES_07,
          typename ENTRIES_08,
          typename ENTRIES_09,
          typename ENTRIES_10,
          typename ENTRIES_11,
          typename ENTRIES_12,
          typename ENTRIES_13,
          typename ENTRIES_14,
          typename ENTRIES_15,
          typename ENTRIES_16,
          typename ENTRIES_17,
          typename ENTRIES_18,
          typename ENTRIES_19,
          typename ENTRIES_20,
          typename ENTRIES_21,
          typename ENTRIES_22,
          typename ENTRIES_23,
          typename ENTRIES_24,
          typename ENTRIES_25,
          typename ENTRIES_26,
          typename ENTRIES_27,
          typename ENTRIES_28,
          typename ENTRIES_29,
          typename ENTRIES_30,
          typename ENTRIES_31,
          typename ENTRIES_32>
inline
bdld::Datum DatumMaker::mok(
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_01) entries_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_02) entries_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_03) entries_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_04) entries_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_05) entries_05,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_06) entries_06,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_07) entries_07,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_08) entries_08,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_09) entries_09,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_10) entries_10,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_11) entries_11,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_12) entries_12,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_13) entries_13,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_14) entries_14,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_15) entries_15,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_16) entries_16,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_17) entries_17,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_18) entries_18,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_19) entries_19,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_20) entries_20,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_21) entries_21,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_22) entries_22,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_23) entries_23,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_24) entries_24,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_25) entries_25,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_26) entries_26,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_27) entries_27,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_28) entries_28,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_29) entries_29,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_30) entries_30,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_31) entries_31,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES_32) entries_32
                          ) const
{
    const int numArguments = 32u;

    BSLMF_ASSERT(0 == numArguments % 2);

    bdld::DatumMapOwningKeysBuilder builder(d_allocator_p);
    pushBackHelper(&builder, entries_01,
                             entries_02,
                             entries_03,
                             entries_04,
                             entries_05,
                             entries_06,
                             entries_07,
                             entries_08,
                             entries_09,
                             entries_10,
                             entries_11,
                             entries_12,
                             entries_13,
                             entries_14,
                             entries_15,
                             entries_16,
                             entries_17,
                             entries_18,
                             entries_19,
                             entries_20,
                             entries_21,
                             entries_22,
                             entries_23,
                             entries_24,
                             entries_25,
                             entries_26,
                             entries_27,
                             entries_28,
                             entries_29,
                             entries_30,
                             entries_31,
                             entries_32);
    return builder.commit();
}

#else
// The generated code below is a workaround for the absence of perfect
// forwarding in some compilers.
template <typename... ENTRIES>
inline
bdld::Datum DatumMaker::mok(
                          BSLS_COMPILERFEATURES_FORWARD_REF(ENTRIES)... entries
                          ) const
{
    const int numArguments = sizeof...(ENTRIES);

    BSLMF_ASSERT(0 == numArguments % 2);

    bdld::DatumMapOwningKeysBuilder builder(d_allocator_p);
    pushBackHelper(&builder, entries...);
    return builder.commit();
}
#endif

}  // close package namespace
}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2016 Bloomberg Finance L.P.
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
