// bdld_datummaker.h                                                  -*-C++-*-
#ifndef INCLUDED_BDLD_DATUMMAKER
#define INCLUDED_BDLD_DATUMMAKER

//@PURPOSE: Provide a mechanism for easily creating 'bdld::Datum' objects.
//
//@CLASSES:
//  bdld::DatumMaker: a mechanism for easily creating 'bdld::Datum' objects
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

#include <bdlscm_version.h>

#include <bdld_datum.h>
#include <bdld_datumarraybuilder.h>
#include <bdld_datummapbuilder.h>
#include <bdld_datummapowningkeysbuilder.h>
#include <bdld_datumintmapbuilder.h>

#include <bdldfp_decimal.h>

#include <bdlb_nullablevalue.h>

#include <bslmf_assert.h>
#include <bsls_review.h>

#include <bsls_assert.h>
#include <bsls_compilerfeatures.h>
#include <bsls_types.h>

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

    void pushBackHelper(bdld::DatumIntMapBuilder *) const;
        // Do nothing, ends template recursion.

#if !BSLS_COMPILERFEATURES_SIMULATE_VARIADIC_TEMPLATES // $var-args=32
    template <typename TYPE>
    void pushBackHelper(bdld::DatumArrayBuilder *builder,
                        const TYPE&              element) const;
        // 'push_back' the specified 'element' into the specified 'builder'.

    template <typename TYPE, typename... ELEMENTS>
    void pushBackHelper(bdld::DatumArrayBuilder *builder,
                        const TYPE&              element,
                        const ELEMENTS&...       elements) const;
        // 'push_back' the specified 'element' into the specified 'builder',
        // then call 'pushBackHelper' with the specified (variadic) 'elements'.

    template <typename TYPE>
    void pushBackHelper(bdld::DatumMapBuilder    *builder,
                        const bslstl::StringRef&  key,
                        const TYPE&               value) const;
        // 'push_back' the specified 'key' and 'value' pair (forming a
        // property) into the specified 'builder'.

    template <typename TYPE, typename... ENTRIES>
    void pushBackHelper(bdld::DatumMapBuilder    *builder,
                        const bslstl::StringRef&  key,
                        const TYPE&               value,
                        const ENTRIES&...         entries) const;
        // 'push_back' the specified 'key' and 'value' pair (forming a
        // property) into the specified 'builder', then call 'pushBackHelper'
        // with the specified (variadic) entries.

    template <typename TYPE>
    void pushBackHelper(bdld::DatumMapOwningKeysBuilder *builder,
                        const bslstl::StringRef&         key,
                        const TYPE&                      value) const;
        // 'push_back' the specified 'key' and 'value' pair (forming a
        // property) into the specified 'builder'.

    template <typename TYPE, typename... ENTRIES>
    void pushBackHelper(bdld::DatumMapOwningKeysBuilder *builder,
                        const bslstl::StringRef&         key,
                        const TYPE&                      value,
                        const ENTRIES&...                entries) const;
        // 'push_back' the specified 'key' and 'value' pair (forming a
        // property) into the specified 'builder', then call 'pushBackHelper'
        // with the specified (variadic) entries.

    template <typename TYPE>
    void pushBackHelper(bdld::DatumIntMapBuilder *builder,
                        int                       key,
                        const TYPE&               value) const;
        // 'push_back' the specified 'key' and 'value' pair (forming a
        // property) into the specified 'builder'.

    template <typename TYPE, typename... ENTRIES>
    void pushBackHelper(bdld::DatumIntMapBuilder *builder,
                        int                       key,
                        const TYPE&               value,
                        const ENTRIES&...         entries) const;
        // 'push_back' the specified 'key' and 'value' pair (forming a
        // property) into the specified 'builder', then call 'pushBackHelper'
        // with the specified (variadic) entries.

// IMPORTANT NOTE: The section below was manually modified to reduce the
// maximum number of parameters for the array builder to 16.
#else
    template <typename TYPE>
    void pushBackHelper(bdld::DatumArrayBuilder *builder,
                        const TYPE&              element
                        ) const;

    template <typename TYPE, typename ELEMENTS_01>
    void pushBackHelper(bdld::DatumArrayBuilder *builder,
                        const TYPE&              element,
                        const ELEMENTS_01&       elements_01
                        ) const;

    template <typename TYPE, typename ELEMENTS_01,
                             typename ELEMENTS_02>
    void pushBackHelper(bdld::DatumArrayBuilder *builder,
                        const TYPE&              element,
                        const ELEMENTS_01&       elements_01,
                        const ELEMENTS_02&       elements_02
                        ) const;

    template <typename TYPE, typename ELEMENTS_01,
                             typename ELEMENTS_02,
                             typename ELEMENTS_03>
    void pushBackHelper(bdld::DatumArrayBuilder *builder,
                        const TYPE&              element,
                        const ELEMENTS_01&       elements_01,
                        const ELEMENTS_02&       elements_02,
                        const ELEMENTS_03&       elements_03
                        ) const;

    template <typename TYPE, typename ELEMENTS_01,
                             typename ELEMENTS_02,
                             typename ELEMENTS_03,
                             typename ELEMENTS_04>
    void pushBackHelper(bdld::DatumArrayBuilder *builder,
                        const TYPE&              element,
                        const ELEMENTS_01&       elements_01,
                        const ELEMENTS_02&       elements_02,
                        const ELEMENTS_03&       elements_03,
                        const ELEMENTS_04&       elements_04
                        ) const;

    template <typename TYPE, typename ELEMENTS_01,
                             typename ELEMENTS_02,
                             typename ELEMENTS_03,
                             typename ELEMENTS_04,
                             typename ELEMENTS_05>
    void pushBackHelper(bdld::DatumArrayBuilder *builder,
                        const TYPE&              element,
                        const ELEMENTS_01&       elements_01,
                        const ELEMENTS_02&       elements_02,
                        const ELEMENTS_03&       elements_03,
                        const ELEMENTS_04&       elements_04,
                        const ELEMENTS_05&       elements_05
                        ) const;

    template <typename TYPE, typename ELEMENTS_01,
                             typename ELEMENTS_02,
                             typename ELEMENTS_03,
                             typename ELEMENTS_04,
                             typename ELEMENTS_05,
                             typename ELEMENTS_06>
    void pushBackHelper(bdld::DatumArrayBuilder *builder,
                        const TYPE&              element,
                        const ELEMENTS_01&       elements_01,
                        const ELEMENTS_02&       elements_02,
                        const ELEMENTS_03&       elements_03,
                        const ELEMENTS_04&       elements_04,
                        const ELEMENTS_05&       elements_05,
                        const ELEMENTS_06&       elements_06
                        ) const;

    template <typename TYPE, typename ELEMENTS_01,
                             typename ELEMENTS_02,
                             typename ELEMENTS_03,
                             typename ELEMENTS_04,
                             typename ELEMENTS_05,
                             typename ELEMENTS_06,
                             typename ELEMENTS_07>
    void pushBackHelper(bdld::DatumArrayBuilder *builder,
                        const TYPE&              element,
                        const ELEMENTS_01&       elements_01,
                        const ELEMENTS_02&       elements_02,
                        const ELEMENTS_03&       elements_03,
                        const ELEMENTS_04&       elements_04,
                        const ELEMENTS_05&       elements_05,
                        const ELEMENTS_06&       elements_06,
                        const ELEMENTS_07&       elements_07
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
                        const TYPE&              element,
                        const ELEMENTS_01&       elements_01,
                        const ELEMENTS_02&       elements_02,
                        const ELEMENTS_03&       elements_03,
                        const ELEMENTS_04&       elements_04,
                        const ELEMENTS_05&       elements_05,
                        const ELEMENTS_06&       elements_06,
                        const ELEMENTS_07&       elements_07,
                        const ELEMENTS_08&       elements_08
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
                        const TYPE&              element,
                        const ELEMENTS_01&       elements_01,
                        const ELEMENTS_02&       elements_02,
                        const ELEMENTS_03&       elements_03,
                        const ELEMENTS_04&       elements_04,
                        const ELEMENTS_05&       elements_05,
                        const ELEMENTS_06&       elements_06,
                        const ELEMENTS_07&       elements_07,
                        const ELEMENTS_08&       elements_08,
                        const ELEMENTS_09&       elements_09
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
                        const TYPE&              element,
                        const ELEMENTS_01&       elements_01,
                        const ELEMENTS_02&       elements_02,
                        const ELEMENTS_03&       elements_03,
                        const ELEMENTS_04&       elements_04,
                        const ELEMENTS_05&       elements_05,
                        const ELEMENTS_06&       elements_06,
                        const ELEMENTS_07&       elements_07,
                        const ELEMENTS_08&       elements_08,
                        const ELEMENTS_09&       elements_09,
                        const ELEMENTS_10&       elements_10
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
                        const TYPE&              element,
                        const ELEMENTS_01&       elements_01,
                        const ELEMENTS_02&       elements_02,
                        const ELEMENTS_03&       elements_03,
                        const ELEMENTS_04&       elements_04,
                        const ELEMENTS_05&       elements_05,
                        const ELEMENTS_06&       elements_06,
                        const ELEMENTS_07&       elements_07,
                        const ELEMENTS_08&       elements_08,
                        const ELEMENTS_09&       elements_09,
                        const ELEMENTS_10&       elements_10,
                        const ELEMENTS_11&       elements_11
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
                        const TYPE&              element,
                        const ELEMENTS_01&       elements_01,
                        const ELEMENTS_02&       elements_02,
                        const ELEMENTS_03&       elements_03,
                        const ELEMENTS_04&       elements_04,
                        const ELEMENTS_05&       elements_05,
                        const ELEMENTS_06&       elements_06,
                        const ELEMENTS_07&       elements_07,
                        const ELEMENTS_08&       elements_08,
                        const ELEMENTS_09&       elements_09,
                        const ELEMENTS_10&       elements_10,
                        const ELEMENTS_11&       elements_11,
                        const ELEMENTS_12&       elements_12
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
                        const TYPE&              element,
                        const ELEMENTS_01&       elements_01,
                        const ELEMENTS_02&       elements_02,
                        const ELEMENTS_03&       elements_03,
                        const ELEMENTS_04&       elements_04,
                        const ELEMENTS_05&       elements_05,
                        const ELEMENTS_06&       elements_06,
                        const ELEMENTS_07&       elements_07,
                        const ELEMENTS_08&       elements_08,
                        const ELEMENTS_09&       elements_09,
                        const ELEMENTS_10&       elements_10,
                        const ELEMENTS_11&       elements_11,
                        const ELEMENTS_12&       elements_12,
                        const ELEMENTS_13&       elements_13
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
                        const TYPE&              element,
                        const ELEMENTS_01&       elements_01,
                        const ELEMENTS_02&       elements_02,
                        const ELEMENTS_03&       elements_03,
                        const ELEMENTS_04&       elements_04,
                        const ELEMENTS_05&       elements_05,
                        const ELEMENTS_06&       elements_06,
                        const ELEMENTS_07&       elements_07,
                        const ELEMENTS_08&       elements_08,
                        const ELEMENTS_09&       elements_09,
                        const ELEMENTS_10&       elements_10,
                        const ELEMENTS_11&       elements_11,
                        const ELEMENTS_12&       elements_12,
                        const ELEMENTS_13&       elements_13,
                        const ELEMENTS_14&       elements_14
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
                        const TYPE&              element,
                        const ELEMENTS_01&       elements_01,
                        const ELEMENTS_02&       elements_02,
                        const ELEMENTS_03&       elements_03,
                        const ELEMENTS_04&       elements_04,
                        const ELEMENTS_05&       elements_05,
                        const ELEMENTS_06&       elements_06,
                        const ELEMENTS_07&       elements_07,
                        const ELEMENTS_08&       elements_08,
                        const ELEMENTS_09&       elements_09,
                        const ELEMENTS_10&       elements_10,
                        const ELEMENTS_11&       elements_11,
                        const ELEMENTS_12&       elements_12,
                        const ELEMENTS_13&       elements_13,
                        const ELEMENTS_14&       elements_14,
                        const ELEMENTS_15&       elements_15
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
                        const TYPE&              element,
                        const ELEMENTS_01&       elements_01,
                        const ELEMENTS_02&       elements_02,
                        const ELEMENTS_03&       elements_03,
                        const ELEMENTS_04&       elements_04,
                        const ELEMENTS_05&       elements_05,
                        const ELEMENTS_06&       elements_06,
                        const ELEMENTS_07&       elements_07,
                        const ELEMENTS_08&       elements_08,
                        const ELEMENTS_09&       elements_09,
                        const ELEMENTS_10&       elements_10,
                        const ELEMENTS_11&       elements_11,
                        const ELEMENTS_12&       elements_12,
                        const ELEMENTS_13&       elements_13,
                        const ELEMENTS_14&       elements_14,
                        const ELEMENTS_15&       elements_15,
                        const ELEMENTS_16&       elements_16
                        ) const;

    template <typename TYPE>
    void pushBackHelper(bdld::DatumMapBuilder    *builder,
                        const bslstl::StringRef&  key,
                        const TYPE&               value
                        ) const;

    template <typename TYPE, typename ENTRIES_01,
                             typename ENTRIES_02>
    void pushBackHelper(bdld::DatumMapBuilder    *builder,
                        const bslstl::StringRef&  key,
                        const TYPE&               value,
                        const ENTRIES_01&         entries_01,
                        const ENTRIES_02&         entries_02
                        ) const;

    template <typename TYPE, typename ENTRIES_01,
                             typename ENTRIES_02,
                             typename ENTRIES_03,
                             typename ENTRIES_04>
    void pushBackHelper(bdld::DatumMapBuilder    *builder,
                        const bslstl::StringRef&  key,
                        const TYPE&               value,
                        const ENTRIES_01&         entries_01,
                        const ENTRIES_02&         entries_02,
                        const ENTRIES_03&         entries_03,
                        const ENTRIES_04&         entries_04
                        ) const;

    template <typename TYPE, typename ENTRIES_01,
                             typename ENTRIES_02,
                             typename ENTRIES_03,
                             typename ENTRIES_04,
                             typename ENTRIES_05,
                             typename ENTRIES_06>
    void pushBackHelper(bdld::DatumMapBuilder    *builder,
                        const bslstl::StringRef&  key,
                        const TYPE&               value,
                        const ENTRIES_01&         entries_01,
                        const ENTRIES_02&         entries_02,
                        const ENTRIES_03&         entries_03,
                        const ENTRIES_04&         entries_04,
                        const ENTRIES_05&         entries_05,
                        const ENTRIES_06&         entries_06
                        ) const;

    template <typename TYPE, typename ENTRIES_01,
                             typename ENTRIES_02,
                             typename ENTRIES_03,
                             typename ENTRIES_04,
                             typename ENTRIES_05,
                             typename ENTRIES_06,
                             typename ENTRIES_07,
                             typename ENTRIES_08>
    void pushBackHelper(bdld::DatumMapBuilder    *builder,
                        const bslstl::StringRef&  key,
                        const TYPE&               value,
                        const ENTRIES_01&         entries_01,
                        const ENTRIES_02&         entries_02,
                        const ENTRIES_03&         entries_03,
                        const ENTRIES_04&         entries_04,
                        const ENTRIES_05&         entries_05,
                        const ENTRIES_06&         entries_06,
                        const ENTRIES_07&         entries_07,
                        const ENTRIES_08&         entries_08
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
    void pushBackHelper(bdld::DatumMapBuilder    *builder,
                        const bslstl::StringRef&  key,
                        const TYPE&               value,
                        const ENTRIES_01&         entries_01,
                        const ENTRIES_02&         entries_02,
                        const ENTRIES_03&         entries_03,
                        const ENTRIES_04&         entries_04,
                        const ENTRIES_05&         entries_05,
                        const ENTRIES_06&         entries_06,
                        const ENTRIES_07&         entries_07,
                        const ENTRIES_08&         entries_08,
                        const ENTRIES_09&         entries_09,
                        const ENTRIES_10&         entries_10
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
    void pushBackHelper(bdld::DatumMapBuilder    *builder,
                        const bslstl::StringRef&  key,
                        const TYPE&               value,
                        const ENTRIES_01&         entries_01,
                        const ENTRIES_02&         entries_02,
                        const ENTRIES_03&         entries_03,
                        const ENTRIES_04&         entries_04,
                        const ENTRIES_05&         entries_05,
                        const ENTRIES_06&         entries_06,
                        const ENTRIES_07&         entries_07,
                        const ENTRIES_08&         entries_08,
                        const ENTRIES_09&         entries_09,
                        const ENTRIES_10&         entries_10,
                        const ENTRIES_11&         entries_11,
                        const ENTRIES_12&         entries_12
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
    void pushBackHelper(bdld::DatumMapBuilder    *builder,
                        const bslstl::StringRef&  key,
                        const TYPE&               value,
                        const ENTRIES_01&         entries_01,
                        const ENTRIES_02&         entries_02,
                        const ENTRIES_03&         entries_03,
                        const ENTRIES_04&         entries_04,
                        const ENTRIES_05&         entries_05,
                        const ENTRIES_06&         entries_06,
                        const ENTRIES_07&         entries_07,
                        const ENTRIES_08&         entries_08,
                        const ENTRIES_09&         entries_09,
                        const ENTRIES_10&         entries_10,
                        const ENTRIES_11&         entries_11,
                        const ENTRIES_12&         entries_12,
                        const ENTRIES_13&         entries_13,
                        const ENTRIES_14&         entries_14
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
    void pushBackHelper(bdld::DatumMapBuilder    *builder,
                        const bslstl::StringRef&  key,
                        const TYPE&               value,
                        const ENTRIES_01&         entries_01,
                        const ENTRIES_02&         entries_02,
                        const ENTRIES_03&         entries_03,
                        const ENTRIES_04&         entries_04,
                        const ENTRIES_05&         entries_05,
                        const ENTRIES_06&         entries_06,
                        const ENTRIES_07&         entries_07,
                        const ENTRIES_08&         entries_08,
                        const ENTRIES_09&         entries_09,
                        const ENTRIES_10&         entries_10,
                        const ENTRIES_11&         entries_11,
                        const ENTRIES_12&         entries_12,
                        const ENTRIES_13&         entries_13,
                        const ENTRIES_14&         entries_14,
                        const ENTRIES_15&         entries_15,
                        const ENTRIES_16&         entries_16
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
    void pushBackHelper(bdld::DatumMapBuilder    *builder,
                        const bslstl::StringRef&  key,
                        const TYPE&               value,
                        const ENTRIES_01&         entries_01,
                        const ENTRIES_02&         entries_02,
                        const ENTRIES_03&         entries_03,
                        const ENTRIES_04&         entries_04,
                        const ENTRIES_05&         entries_05,
                        const ENTRIES_06&         entries_06,
                        const ENTRIES_07&         entries_07,
                        const ENTRIES_08&         entries_08,
                        const ENTRIES_09&         entries_09,
                        const ENTRIES_10&         entries_10,
                        const ENTRIES_11&         entries_11,
                        const ENTRIES_12&         entries_12,
                        const ENTRIES_13&         entries_13,
                        const ENTRIES_14&         entries_14,
                        const ENTRIES_15&         entries_15,
                        const ENTRIES_16&         entries_16,
                        const ENTRIES_17&         entries_17,
                        const ENTRIES_18&         entries_18
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
    void pushBackHelper(bdld::DatumMapBuilder    *builder,
                        const bslstl::StringRef&  key,
                        const TYPE&               value,
                        const ENTRIES_01&         entries_01,
                        const ENTRIES_02&         entries_02,
                        const ENTRIES_03&         entries_03,
                        const ENTRIES_04&         entries_04,
                        const ENTRIES_05&         entries_05,
                        const ENTRIES_06&         entries_06,
                        const ENTRIES_07&         entries_07,
                        const ENTRIES_08&         entries_08,
                        const ENTRIES_09&         entries_09,
                        const ENTRIES_10&         entries_10,
                        const ENTRIES_11&         entries_11,
                        const ENTRIES_12&         entries_12,
                        const ENTRIES_13&         entries_13,
                        const ENTRIES_14&         entries_14,
                        const ENTRIES_15&         entries_15,
                        const ENTRIES_16&         entries_16,
                        const ENTRIES_17&         entries_17,
                        const ENTRIES_18&         entries_18,
                        const ENTRIES_19&         entries_19,
                        const ENTRIES_20&         entries_20
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
    void pushBackHelper(bdld::DatumMapBuilder    *builder,
                        const bslstl::StringRef&  key,
                        const TYPE&               value,
                        const ENTRIES_01&         entries_01,
                        const ENTRIES_02&         entries_02,
                        const ENTRIES_03&         entries_03,
                        const ENTRIES_04&         entries_04,
                        const ENTRIES_05&         entries_05,
                        const ENTRIES_06&         entries_06,
                        const ENTRIES_07&         entries_07,
                        const ENTRIES_08&         entries_08,
                        const ENTRIES_09&         entries_09,
                        const ENTRIES_10&         entries_10,
                        const ENTRIES_11&         entries_11,
                        const ENTRIES_12&         entries_12,
                        const ENTRIES_13&         entries_13,
                        const ENTRIES_14&         entries_14,
                        const ENTRIES_15&         entries_15,
                        const ENTRIES_16&         entries_16,
                        const ENTRIES_17&         entries_17,
                        const ENTRIES_18&         entries_18,
                        const ENTRIES_19&         entries_19,
                        const ENTRIES_20&         entries_20,
                        const ENTRIES_21&         entries_21,
                        const ENTRIES_22&         entries_22
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
    void pushBackHelper(bdld::DatumMapBuilder    *builder,
                        const bslstl::StringRef&  key,
                        const TYPE&               value,
                        const ENTRIES_01&         entries_01,
                        const ENTRIES_02&         entries_02,
                        const ENTRIES_03&         entries_03,
                        const ENTRIES_04&         entries_04,
                        const ENTRIES_05&         entries_05,
                        const ENTRIES_06&         entries_06,
                        const ENTRIES_07&         entries_07,
                        const ENTRIES_08&         entries_08,
                        const ENTRIES_09&         entries_09,
                        const ENTRIES_10&         entries_10,
                        const ENTRIES_11&         entries_11,
                        const ENTRIES_12&         entries_12,
                        const ENTRIES_13&         entries_13,
                        const ENTRIES_14&         entries_14,
                        const ENTRIES_15&         entries_15,
                        const ENTRIES_16&         entries_16,
                        const ENTRIES_17&         entries_17,
                        const ENTRIES_18&         entries_18,
                        const ENTRIES_19&         entries_19,
                        const ENTRIES_20&         entries_20,
                        const ENTRIES_21&         entries_21,
                        const ENTRIES_22&         entries_22,
                        const ENTRIES_23&         entries_23,
                        const ENTRIES_24&         entries_24
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
    void pushBackHelper(bdld::DatumMapBuilder    *builder,
                        const bslstl::StringRef&  key,
                        const TYPE&               value,
                        const ENTRIES_01&         entries_01,
                        const ENTRIES_02&         entries_02,
                        const ENTRIES_03&         entries_03,
                        const ENTRIES_04&         entries_04,
                        const ENTRIES_05&         entries_05,
                        const ENTRIES_06&         entries_06,
                        const ENTRIES_07&         entries_07,
                        const ENTRIES_08&         entries_08,
                        const ENTRIES_09&         entries_09,
                        const ENTRIES_10&         entries_10,
                        const ENTRIES_11&         entries_11,
                        const ENTRIES_12&         entries_12,
                        const ENTRIES_13&         entries_13,
                        const ENTRIES_14&         entries_14,
                        const ENTRIES_15&         entries_15,
                        const ENTRIES_16&         entries_16,
                        const ENTRIES_17&         entries_17,
                        const ENTRIES_18&         entries_18,
                        const ENTRIES_19&         entries_19,
                        const ENTRIES_20&         entries_20,
                        const ENTRIES_21&         entries_21,
                        const ENTRIES_22&         entries_22,
                        const ENTRIES_23&         entries_23,
                        const ENTRIES_24&         entries_24,
                        const ENTRIES_25&         entries_25,
                        const ENTRIES_26&         entries_26
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
    void pushBackHelper(bdld::DatumMapBuilder    *builder,
                        const bslstl::StringRef&  key,
                        const TYPE&               value,
                        const ENTRIES_01&         entries_01,
                        const ENTRIES_02&         entries_02,
                        const ENTRIES_03&         entries_03,
                        const ENTRIES_04&         entries_04,
                        const ENTRIES_05&         entries_05,
                        const ENTRIES_06&         entries_06,
                        const ENTRIES_07&         entries_07,
                        const ENTRIES_08&         entries_08,
                        const ENTRIES_09&         entries_09,
                        const ENTRIES_10&         entries_10,
                        const ENTRIES_11&         entries_11,
                        const ENTRIES_12&         entries_12,
                        const ENTRIES_13&         entries_13,
                        const ENTRIES_14&         entries_14,
                        const ENTRIES_15&         entries_15,
                        const ENTRIES_16&         entries_16,
                        const ENTRIES_17&         entries_17,
                        const ENTRIES_18&         entries_18,
                        const ENTRIES_19&         entries_19,
                        const ENTRIES_20&         entries_20,
                        const ENTRIES_21&         entries_21,
                        const ENTRIES_22&         entries_22,
                        const ENTRIES_23&         entries_23,
                        const ENTRIES_24&         entries_24,
                        const ENTRIES_25&         entries_25,
                        const ENTRIES_26&         entries_26,
                        const ENTRIES_27&         entries_27,
                        const ENTRIES_28&         entries_28
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
    void pushBackHelper(bdld::DatumMapBuilder    *builder,
                        const bslstl::StringRef&  key,
                        const TYPE&               value,
                        const ENTRIES_01&         entries_01,
                        const ENTRIES_02&         entries_02,
                        const ENTRIES_03&         entries_03,
                        const ENTRIES_04&         entries_04,
                        const ENTRIES_05&         entries_05,
                        const ENTRIES_06&         entries_06,
                        const ENTRIES_07&         entries_07,
                        const ENTRIES_08&         entries_08,
                        const ENTRIES_09&         entries_09,
                        const ENTRIES_10&         entries_10,
                        const ENTRIES_11&         entries_11,
                        const ENTRIES_12&         entries_12,
                        const ENTRIES_13&         entries_13,
                        const ENTRIES_14&         entries_14,
                        const ENTRIES_15&         entries_15,
                        const ENTRIES_16&         entries_16,
                        const ENTRIES_17&         entries_17,
                        const ENTRIES_18&         entries_18,
                        const ENTRIES_19&         entries_19,
                        const ENTRIES_20&         entries_20,
                        const ENTRIES_21&         entries_21,
                        const ENTRIES_22&         entries_22,
                        const ENTRIES_23&         entries_23,
                        const ENTRIES_24&         entries_24,
                        const ENTRIES_25&         entries_25,
                        const ENTRIES_26&         entries_26,
                        const ENTRIES_27&         entries_27,
                        const ENTRIES_28&         entries_28,
                        const ENTRIES_29&         entries_29,
                        const ENTRIES_30&         entries_30
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
    void pushBackHelper(bdld::DatumMapBuilder    *builder,
                        const bslstl::StringRef&  key,
                        const TYPE&               value,
                        const ENTRIES_01&         entries_01,
                        const ENTRIES_02&         entries_02,
                        const ENTRIES_03&         entries_03,
                        const ENTRIES_04&         entries_04,
                        const ENTRIES_05&         entries_05,
                        const ENTRIES_06&         entries_06,
                        const ENTRIES_07&         entries_07,
                        const ENTRIES_08&         entries_08,
                        const ENTRIES_09&         entries_09,
                        const ENTRIES_10&         entries_10,
                        const ENTRIES_11&         entries_11,
                        const ENTRIES_12&         entries_12,
                        const ENTRIES_13&         entries_13,
                        const ENTRIES_14&         entries_14,
                        const ENTRIES_15&         entries_15,
                        const ENTRIES_16&         entries_16,
                        const ENTRIES_17&         entries_17,
                        const ENTRIES_18&         entries_18,
                        const ENTRIES_19&         entries_19,
                        const ENTRIES_20&         entries_20,
                        const ENTRIES_21&         entries_21,
                        const ENTRIES_22&         entries_22,
                        const ENTRIES_23&         entries_23,
                        const ENTRIES_24&         entries_24,
                        const ENTRIES_25&         entries_25,
                        const ENTRIES_26&         entries_26,
                        const ENTRIES_27&         entries_27,
                        const ENTRIES_28&         entries_28,
                        const ENTRIES_29&         entries_29,
                        const ENTRIES_30&         entries_30,
                        const ENTRIES_31&         entries_31,
                        const ENTRIES_32&         entries_32
                        ) const;

    template <typename TYPE>
    void pushBackHelper(bdld::DatumMapOwningKeysBuilder *builder,
                        const bslstl::StringRef&         key,
                        const TYPE&                      value
                        ) const;

    template <typename TYPE, typename ENTRIES_01,
                             typename ENTRIES_02>
    void pushBackHelper(bdld::DatumMapOwningKeysBuilder *builder,
                        const bslstl::StringRef&         key,
                        const TYPE&                      value,
                        const ENTRIES_01&                entries_01,
                        const ENTRIES_02&                entries_02
                        ) const;

    template <typename TYPE, typename ENTRIES_01,
                             typename ENTRIES_02,
                             typename ENTRIES_03,
                             typename ENTRIES_04>
    void pushBackHelper(bdld::DatumMapOwningKeysBuilder *builder,
                        const bslstl::StringRef&         key,
                        const TYPE&                      value,
                        const ENTRIES_01&                entries_01,
                        const ENTRIES_02&                entries_02,
                        const ENTRIES_03&                entries_03,
                        const ENTRIES_04&                entries_04
                        ) const;

    template <typename TYPE, typename ENTRIES_01,
                             typename ENTRIES_02,
                             typename ENTRIES_03,
                             typename ENTRIES_04,
                             typename ENTRIES_05,
                             typename ENTRIES_06>
    void pushBackHelper(bdld::DatumMapOwningKeysBuilder *builder,
                        const bslstl::StringRef&         key,
                        const TYPE&                      value,
                        const ENTRIES_01&                entries_01,
                        const ENTRIES_02&                entries_02,
                        const ENTRIES_03&                entries_03,
                        const ENTRIES_04&                entries_04,
                        const ENTRIES_05&                entries_05,
                        const ENTRIES_06&                entries_06
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
                        const TYPE&                      value,
                        const ENTRIES_01&                entries_01,
                        const ENTRIES_02&                entries_02,
                        const ENTRIES_03&                entries_03,
                        const ENTRIES_04&                entries_04,
                        const ENTRIES_05&                entries_05,
                        const ENTRIES_06&                entries_06,
                        const ENTRIES_07&                entries_07,
                        const ENTRIES_08&                entries_08
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
                        const TYPE&                      value,
                        const ENTRIES_01&                entries_01,
                        const ENTRIES_02&                entries_02,
                        const ENTRIES_03&                entries_03,
                        const ENTRIES_04&                entries_04,
                        const ENTRIES_05&                entries_05,
                        const ENTRIES_06&                entries_06,
                        const ENTRIES_07&                entries_07,
                        const ENTRIES_08&                entries_08,
                        const ENTRIES_09&                entries_09,
                        const ENTRIES_10&                entries_10
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
                        const TYPE&                      value,
                        const ENTRIES_01&                entries_01,
                        const ENTRIES_02&                entries_02,
                        const ENTRIES_03&                entries_03,
                        const ENTRIES_04&                entries_04,
                        const ENTRIES_05&                entries_05,
                        const ENTRIES_06&                entries_06,
                        const ENTRIES_07&                entries_07,
                        const ENTRIES_08&                entries_08,
                        const ENTRIES_09&                entries_09,
                        const ENTRIES_10&                entries_10,
                        const ENTRIES_11&                entries_11,
                        const ENTRIES_12&                entries_12
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
                        const TYPE&                      value,
                        const ENTRIES_01&                entries_01,
                        const ENTRIES_02&                entries_02,
                        const ENTRIES_03&                entries_03,
                        const ENTRIES_04&                entries_04,
                        const ENTRIES_05&                entries_05,
                        const ENTRIES_06&                entries_06,
                        const ENTRIES_07&                entries_07,
                        const ENTRIES_08&                entries_08,
                        const ENTRIES_09&                entries_09,
                        const ENTRIES_10&                entries_10,
                        const ENTRIES_11&                entries_11,
                        const ENTRIES_12&                entries_12,
                        const ENTRIES_13&                entries_13,
                        const ENTRIES_14&                entries_14
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
                        const TYPE&                      value,
                        const ENTRIES_01&                entries_01,
                        const ENTRIES_02&                entries_02,
                        const ENTRIES_03&                entries_03,
                        const ENTRIES_04&                entries_04,
                        const ENTRIES_05&                entries_05,
                        const ENTRIES_06&                entries_06,
                        const ENTRIES_07&                entries_07,
                        const ENTRIES_08&                entries_08,
                        const ENTRIES_09&                entries_09,
                        const ENTRIES_10&                entries_10,
                        const ENTRIES_11&                entries_11,
                        const ENTRIES_12&                entries_12,
                        const ENTRIES_13&                entries_13,
                        const ENTRIES_14&                entries_14,
                        const ENTRIES_15&                entries_15,
                        const ENTRIES_16&                entries_16
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
                        const TYPE&                      value,
                        const ENTRIES_01&                entries_01,
                        const ENTRIES_02&                entries_02,
                        const ENTRIES_03&                entries_03,
                        const ENTRIES_04&                entries_04,
                        const ENTRIES_05&                entries_05,
                        const ENTRIES_06&                entries_06,
                        const ENTRIES_07&                entries_07,
                        const ENTRIES_08&                entries_08,
                        const ENTRIES_09&                entries_09,
                        const ENTRIES_10&                entries_10,
                        const ENTRIES_11&                entries_11,
                        const ENTRIES_12&                entries_12,
                        const ENTRIES_13&                entries_13,
                        const ENTRIES_14&                entries_14,
                        const ENTRIES_15&                entries_15,
                        const ENTRIES_16&                entries_16,
                        const ENTRIES_17&                entries_17,
                        const ENTRIES_18&                entries_18
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
                        const TYPE&                      value,
                        const ENTRIES_01&                entries_01,
                        const ENTRIES_02&                entries_02,
                        const ENTRIES_03&                entries_03,
                        const ENTRIES_04&                entries_04,
                        const ENTRIES_05&                entries_05,
                        const ENTRIES_06&                entries_06,
                        const ENTRIES_07&                entries_07,
                        const ENTRIES_08&                entries_08,
                        const ENTRIES_09&                entries_09,
                        const ENTRIES_10&                entries_10,
                        const ENTRIES_11&                entries_11,
                        const ENTRIES_12&                entries_12,
                        const ENTRIES_13&                entries_13,
                        const ENTRIES_14&                entries_14,
                        const ENTRIES_15&                entries_15,
                        const ENTRIES_16&                entries_16,
                        const ENTRIES_17&                entries_17,
                        const ENTRIES_18&                entries_18,
                        const ENTRIES_19&                entries_19,
                        const ENTRIES_20&                entries_20
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
                        const TYPE&                      value,
                        const ENTRIES_01&                entries_01,
                        const ENTRIES_02&                entries_02,
                        const ENTRIES_03&                entries_03,
                        const ENTRIES_04&                entries_04,
                        const ENTRIES_05&                entries_05,
                        const ENTRIES_06&                entries_06,
                        const ENTRIES_07&                entries_07,
                        const ENTRIES_08&                entries_08,
                        const ENTRIES_09&                entries_09,
                        const ENTRIES_10&                entries_10,
                        const ENTRIES_11&                entries_11,
                        const ENTRIES_12&                entries_12,
                        const ENTRIES_13&                entries_13,
                        const ENTRIES_14&                entries_14,
                        const ENTRIES_15&                entries_15,
                        const ENTRIES_16&                entries_16,
                        const ENTRIES_17&                entries_17,
                        const ENTRIES_18&                entries_18,
                        const ENTRIES_19&                entries_19,
                        const ENTRIES_20&                entries_20,
                        const ENTRIES_21&                entries_21,
                        const ENTRIES_22&                entries_22
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
                        const TYPE&                      value,
                        const ENTRIES_01&                entries_01,
                        const ENTRIES_02&                entries_02,
                        const ENTRIES_03&                entries_03,
                        const ENTRIES_04&                entries_04,
                        const ENTRIES_05&                entries_05,
                        const ENTRIES_06&                entries_06,
                        const ENTRIES_07&                entries_07,
                        const ENTRIES_08&                entries_08,
                        const ENTRIES_09&                entries_09,
                        const ENTRIES_10&                entries_10,
                        const ENTRIES_11&                entries_11,
                        const ENTRIES_12&                entries_12,
                        const ENTRIES_13&                entries_13,
                        const ENTRIES_14&                entries_14,
                        const ENTRIES_15&                entries_15,
                        const ENTRIES_16&                entries_16,
                        const ENTRIES_17&                entries_17,
                        const ENTRIES_18&                entries_18,
                        const ENTRIES_19&                entries_19,
                        const ENTRIES_20&                entries_20,
                        const ENTRIES_21&                entries_21,
                        const ENTRIES_22&                entries_22,
                        const ENTRIES_23&                entries_23,
                        const ENTRIES_24&                entries_24
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
                        const TYPE&                      value,
                        const ENTRIES_01&                entries_01,
                        const ENTRIES_02&                entries_02,
                        const ENTRIES_03&                entries_03,
                        const ENTRIES_04&                entries_04,
                        const ENTRIES_05&                entries_05,
                        const ENTRIES_06&                entries_06,
                        const ENTRIES_07&                entries_07,
                        const ENTRIES_08&                entries_08,
                        const ENTRIES_09&                entries_09,
                        const ENTRIES_10&                entries_10,
                        const ENTRIES_11&                entries_11,
                        const ENTRIES_12&                entries_12,
                        const ENTRIES_13&                entries_13,
                        const ENTRIES_14&                entries_14,
                        const ENTRIES_15&                entries_15,
                        const ENTRIES_16&                entries_16,
                        const ENTRIES_17&                entries_17,
                        const ENTRIES_18&                entries_18,
                        const ENTRIES_19&                entries_19,
                        const ENTRIES_20&                entries_20,
                        const ENTRIES_21&                entries_21,
                        const ENTRIES_22&                entries_22,
                        const ENTRIES_23&                entries_23,
                        const ENTRIES_24&                entries_24,
                        const ENTRIES_25&                entries_25,
                        const ENTRIES_26&                entries_26
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
                        const TYPE&                      value,
                        const ENTRIES_01&                entries_01,
                        const ENTRIES_02&                entries_02,
                        const ENTRIES_03&                entries_03,
                        const ENTRIES_04&                entries_04,
                        const ENTRIES_05&                entries_05,
                        const ENTRIES_06&                entries_06,
                        const ENTRIES_07&                entries_07,
                        const ENTRIES_08&                entries_08,
                        const ENTRIES_09&                entries_09,
                        const ENTRIES_10&                entries_10,
                        const ENTRIES_11&                entries_11,
                        const ENTRIES_12&                entries_12,
                        const ENTRIES_13&                entries_13,
                        const ENTRIES_14&                entries_14,
                        const ENTRIES_15&                entries_15,
                        const ENTRIES_16&                entries_16,
                        const ENTRIES_17&                entries_17,
                        const ENTRIES_18&                entries_18,
                        const ENTRIES_19&                entries_19,
                        const ENTRIES_20&                entries_20,
                        const ENTRIES_21&                entries_21,
                        const ENTRIES_22&                entries_22,
                        const ENTRIES_23&                entries_23,
                        const ENTRIES_24&                entries_24,
                        const ENTRIES_25&                entries_25,
                        const ENTRIES_26&                entries_26,
                        const ENTRIES_27&                entries_27,
                        const ENTRIES_28&                entries_28
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
                        const TYPE&                      value,
                        const ENTRIES_01&                entries_01,
                        const ENTRIES_02&                entries_02,
                        const ENTRIES_03&                entries_03,
                        const ENTRIES_04&                entries_04,
                        const ENTRIES_05&                entries_05,
                        const ENTRIES_06&                entries_06,
                        const ENTRIES_07&                entries_07,
                        const ENTRIES_08&                entries_08,
                        const ENTRIES_09&                entries_09,
                        const ENTRIES_10&                entries_10,
                        const ENTRIES_11&                entries_11,
                        const ENTRIES_12&                entries_12,
                        const ENTRIES_13&                entries_13,
                        const ENTRIES_14&                entries_14,
                        const ENTRIES_15&                entries_15,
                        const ENTRIES_16&                entries_16,
                        const ENTRIES_17&                entries_17,
                        const ENTRIES_18&                entries_18,
                        const ENTRIES_19&                entries_19,
                        const ENTRIES_20&                entries_20,
                        const ENTRIES_21&                entries_21,
                        const ENTRIES_22&                entries_22,
                        const ENTRIES_23&                entries_23,
                        const ENTRIES_24&                entries_24,
                        const ENTRIES_25&                entries_25,
                        const ENTRIES_26&                entries_26,
                        const ENTRIES_27&                entries_27,
                        const ENTRIES_28&                entries_28,
                        const ENTRIES_29&                entries_29,
                        const ENTRIES_30&                entries_30
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
                        const TYPE&                      value,
                        const ENTRIES_01&                entries_01,
                        const ENTRIES_02&                entries_02,
                        const ENTRIES_03&                entries_03,
                        const ENTRIES_04&                entries_04,
                        const ENTRIES_05&                entries_05,
                        const ENTRIES_06&                entries_06,
                        const ENTRIES_07&                entries_07,
                        const ENTRIES_08&                entries_08,
                        const ENTRIES_09&                entries_09,
                        const ENTRIES_10&                entries_10,
                        const ENTRIES_11&                entries_11,
                        const ENTRIES_12&                entries_12,
                        const ENTRIES_13&                entries_13,
                        const ENTRIES_14&                entries_14,
                        const ENTRIES_15&                entries_15,
                        const ENTRIES_16&                entries_16,
                        const ENTRIES_17&                entries_17,
                        const ENTRIES_18&                entries_18,
                        const ENTRIES_19&                entries_19,
                        const ENTRIES_20&                entries_20,
                        const ENTRIES_21&                entries_21,
                        const ENTRIES_22&                entries_22,
                        const ENTRIES_23&                entries_23,
                        const ENTRIES_24&                entries_24,
                        const ENTRIES_25&                entries_25,
                        const ENTRIES_26&                entries_26,
                        const ENTRIES_27&                entries_27,
                        const ENTRIES_28&                entries_28,
                        const ENTRIES_29&                entries_29,
                        const ENTRIES_30&                entries_30,
                        const ENTRIES_31&                entries_31,
                        const ENTRIES_32&                entries_32
                        ) const;


    template <typename TYPE>
    void pushBackHelper(bdld::DatumIntMapBuilder *builder,
                        int                       key,
                        const TYPE&               value
                        ) const;

    template <typename TYPE, typename ENTRY_01>
    void pushBackHelper(bdld::DatumIntMapBuilder *builder,
                        int                       key,
                        const TYPE&               value,
                        int                       key_01,
                        const ENTRY_01&           entry_01
                        ) const;

    template <typename TYPE, typename ENTRY_01,
                             typename ENTRY_02>
    void pushBackHelper(bdld::DatumIntMapBuilder *builder,
                        int                       key,
                        const TYPE&               value,
                        int                       key_01,
                        const ENTRY_01&           entry_01,
                        int                       key_02,
                        const ENTRY_02&           entry_02
                        ) const;

    template <typename TYPE, typename ENTRY_01,
                             typename ENTRY_02,
                             typename ENTRY_03>
    void pushBackHelper(bdld::DatumIntMapBuilder *builder,
                        int                       key,
                        const TYPE&               value,
                        int                       key_01,
                        const ENTRY_01&           entry_01,
                        int                       key_02,
                        const ENTRY_02&           entry_02,
                        int                       key_03,
                        const ENTRY_03&           entry_03
                        ) const;

    template <typename TYPE, typename ENTRY_01,
                             typename ENTRY_02,
                             typename ENTRY_03,
                             typename ENTRY_04>
    void pushBackHelper(bdld::DatumIntMapBuilder *builder,
                        int                       key,
                        const TYPE&               value,
                        int                       key_01,
                        const ENTRY_01&           entry_01,
                        int                       key_02,
                        const ENTRY_02&           entry_02,
                        int                       key_03,
                        const ENTRY_03&           entry_03,
                        int                       key_04,
                        const ENTRY_04&           entry_04
                        ) const;

    template <typename TYPE, typename ENTRY_01,
                             typename ENTRY_02,
                             typename ENTRY_03,
                             typename ENTRY_04,
                             typename ENTRY_05>
    void pushBackHelper(bdld::DatumIntMapBuilder *builder,
                        int                       key,
                        const TYPE&               value,
                        int                       key_01,
                        const ENTRY_01&           entry_01,
                        int                       key_02,
                        const ENTRY_02&           entry_02,
                        int                       key_03,
                        const ENTRY_03&           entry_03,
                        int                       key_04,
                        const ENTRY_04&           entry_04,
                        int                       key_05,
                        const ENTRY_05&           entry_05
                        ) const;

    template <typename TYPE, typename ENTRY_01,
                             typename ENTRY_02,
                             typename ENTRY_03,
                             typename ENTRY_04,
                             typename ENTRY_05,
                             typename ENTRY_06>
    void pushBackHelper(bdld::DatumIntMapBuilder *builder,
                        int                       key,
                        const TYPE&               value,
                        int                       key_01,
                        const ENTRY_01&           entry_01,
                        int                       key_02,
                        const ENTRY_02&           entry_02,
                        int                       key_03,
                        const ENTRY_03&           entry_03,
                        int                       key_04,
                        const ENTRY_04&           entry_04,
                        int                       key_05,
                        const ENTRY_05&           entry_05,
                        int                       key_06,
                        const ENTRY_06&           entry_06
                        ) const;

    template <typename TYPE, typename ENTRY_01,
                             typename ENTRY_02,
                             typename ENTRY_03,
                             typename ENTRY_04,
                             typename ENTRY_05,
                             typename ENTRY_06,
                             typename ENTRY_07>
    void pushBackHelper(bdld::DatumIntMapBuilder *builder,
                        int                       key,
                        const TYPE&               value,
                        int                       key_01,
                        const ENTRY_01&           entry_01,
                        int                       key_02,
                        const ENTRY_02&           entry_02,
                        int                       key_03,
                        const ENTRY_03&           entry_03,
                        int                       key_04,
                        const ENTRY_04&           entry_04,
                        int                       key_05,
                        const ENTRY_05&           entry_05,
                        int                       key_06,
                        const ENTRY_06&           entry_06,
                        int                       key_07,
                        const ENTRY_07&           entry_07
                        ) const;

    template <typename TYPE, typename ENTRY_01,
                             typename ENTRY_02,
                             typename ENTRY_03,
                             typename ENTRY_04,
                             typename ENTRY_05,
                             typename ENTRY_06,
                             typename ENTRY_07,
                             typename ENTRY_08>
    void pushBackHelper(bdld::DatumIntMapBuilder *builder,
                        int                       key,
                        const TYPE&               value,
                        int                       key_01,
                        const ENTRY_01&           entry_01,
                        int                       key_02,
                        const ENTRY_02&           entry_02,
                        int                       key_03,
                        const ENTRY_03&           entry_03,
                        int                       key_04,
                        const ENTRY_04&           entry_04,
                        int                       key_05,
                        const ENTRY_05&           entry_05,
                        int                       key_06,
                        const ENTRY_06&           entry_06,
                        int                       key_07,
                        const ENTRY_07&           entry_07,
                        int                       key_08,
                        const ENTRY_08&           entry_08
                        ) const;

    template <typename TYPE, typename ENTRY_01,
                             typename ENTRY_02,
                             typename ENTRY_03,
                             typename ENTRY_04,
                             typename ENTRY_05,
                             typename ENTRY_06,
                             typename ENTRY_07,
                             typename ENTRY_08,
                             typename ENTRY_09>
    void pushBackHelper(bdld::DatumIntMapBuilder *builder,
                        int                       key,
                        const TYPE&               value,
                        int                       key_01,
                        const ENTRY_01&           entry_01,
                        int                       key_02,
                        const ENTRY_02&           entry_02,
                        int                       key_03,
                        const ENTRY_03&           entry_03,
                        int                       key_04,
                        const ENTRY_04&           entry_04,
                        int                       key_05,
                        const ENTRY_05&           entry_05,
                        int                       key_06,
                        const ENTRY_06&           entry_06,
                        int                       key_07,
                        const ENTRY_07&           entry_07,
                        int                       key_08,
                        const ENTRY_08&           entry_08,
                        int                       key_09,
                        const ENTRY_09&           entry_09
                        ) const;

    template <typename TYPE, typename ENTRY_01,
                             typename ENTRY_02,
                             typename ENTRY_03,
                             typename ENTRY_04,
                             typename ENTRY_05,
                             typename ENTRY_06,
                             typename ENTRY_07,
                             typename ENTRY_08,
                             typename ENTRY_09,
                             typename ENTRY_10>
    void pushBackHelper(bdld::DatumIntMapBuilder *builder,
                        int                       key,
                        const TYPE&               value,
                        int                       key_01,
                        const ENTRY_01&           entry_01,
                        int                       key_02,
                        const ENTRY_02&           entry_02,
                        int                       key_03,
                        const ENTRY_03&           entry_03,
                        int                       key_04,
                        const ENTRY_04&           entry_04,
                        int                       key_05,
                        const ENTRY_05&           entry_05,
                        int                       key_06,
                        const ENTRY_06&           entry_06,
                        int                       key_07,
                        const ENTRY_07&           entry_07,
                        int                       key_08,
                        const ENTRY_08&           entry_08,
                        int                       key_09,
                        const ENTRY_09&           entry_09,
                        int                       key_10,
                        const ENTRY_10&           entry_10
                        ) const;

    template <typename TYPE, typename ENTRY_01,
                             typename ENTRY_02,
                             typename ENTRY_03,
                             typename ENTRY_04,
                             typename ENTRY_05,
                             typename ENTRY_06,
                             typename ENTRY_07,
                             typename ENTRY_08,
                             typename ENTRY_09,
                             typename ENTRY_10,
                             typename ENTRY_11>
    void pushBackHelper(bdld::DatumIntMapBuilder *builder,
                        int                       key,
                        const TYPE&               value,
                        int                       key_01,
                        const ENTRY_01&           entry_01,
                        int                       key_02,
                        const ENTRY_02&           entry_02,
                        int                       key_03,
                        const ENTRY_03&           entry_03,
                        int                       key_04,
                        const ENTRY_04&           entry_04,
                        int                       key_05,
                        const ENTRY_05&           entry_05,
                        int                       key_06,
                        const ENTRY_06&           entry_06,
                        int                       key_07,
                        const ENTRY_07&           entry_07,
                        int                       key_08,
                        const ENTRY_08&           entry_08,
                        int                       key_09,
                        const ENTRY_09&           entry_09,
                        int                       key_10,
                        const ENTRY_10&           entry_10,
                        int                       key_11,
                        const ENTRY_11&           entry_11
                        ) const;

    template <typename TYPE, typename ENTRY_01,
                             typename ENTRY_02,
                             typename ENTRY_03,
                             typename ENTRY_04,
                             typename ENTRY_05,
                             typename ENTRY_06,
                             typename ENTRY_07,
                             typename ENTRY_08,
                             typename ENTRY_09,
                             typename ENTRY_10,
                             typename ENTRY_11,
                             typename ENTRY_12>
    void pushBackHelper(bdld::DatumIntMapBuilder *builder,
                        int                       key,
                        const TYPE&               value,
                        int                       key_01,
                        const ENTRY_01&           entry_01,
                        int                       key_02,
                        const ENTRY_02&           entry_02,
                        int                       key_03,
                        const ENTRY_03&           entry_03,
                        int                       key_04,
                        const ENTRY_04&           entry_04,
                        int                       key_05,
                        const ENTRY_05&           entry_05,
                        int                       key_06,
                        const ENTRY_06&           entry_06,
                        int                       key_07,
                        const ENTRY_07&           entry_07,
                        int                       key_08,
                        const ENTRY_08&           entry_08,
                        int                       key_09,
                        const ENTRY_09&           entry_09,
                        int                       key_10,
                        const ENTRY_10&           entry_10,
                        int                       key_11,
                        const ENTRY_11&           entry_11,
                        int                       key_12,
                        const ENTRY_12&           entry_12
                        ) const;

    template <typename TYPE, typename ENTRY_01,
                             typename ENTRY_02,
                             typename ENTRY_03,
                             typename ENTRY_04,
                             typename ENTRY_05,
                             typename ENTRY_06,
                             typename ENTRY_07,
                             typename ENTRY_08,
                             typename ENTRY_09,
                             typename ENTRY_10,
                             typename ENTRY_11,
                             typename ENTRY_12,
                             typename ENTRY_13>
    void pushBackHelper(bdld::DatumIntMapBuilder *builder,
                        int                       key,
                        const TYPE&               value,
                        int                       key_01,
                        const ENTRY_01&           entry_01,
                        int                       key_02,
                        const ENTRY_02&           entry_02,
                        int                       key_03,
                        const ENTRY_03&           entry_03,
                        int                       key_04,
                        const ENTRY_04&           entry_04,
                        int                       key_05,
                        const ENTRY_05&           entry_05,
                        int                       key_06,
                        const ENTRY_06&           entry_06,
                        int                       key_07,
                        const ENTRY_07&           entry_07,
                        int                       key_08,
                        const ENTRY_08&           entry_08,
                        int                       key_09,
                        const ENTRY_09&           entry_09,
                        int                       key_10,
                        const ENTRY_10&           entry_10,
                        int                       key_11,
                        const ENTRY_11&           entry_11,
                        int                       key_12,
                        const ENTRY_12&           entry_12,
                        int                       key_13,
                        const ENTRY_13&           entry_13
                        ) const;

    template <typename TYPE, typename ENTRY_01,
                             typename ENTRY_02,
                             typename ENTRY_03,
                             typename ENTRY_04,
                             typename ENTRY_05,
                             typename ENTRY_06,
                             typename ENTRY_07,
                             typename ENTRY_08,
                             typename ENTRY_09,
                             typename ENTRY_10,
                             typename ENTRY_11,
                             typename ENTRY_12,
                             typename ENTRY_13,
                             typename ENTRY_14>
    void pushBackHelper(bdld::DatumIntMapBuilder *builder,
                        int                       key,
                        const TYPE&               value,
                        int                       key_01,
                        const ENTRY_01&           entry_01,
                        int                       key_02,
                        const ENTRY_02&           entry_02,
                        int                       key_03,
                        const ENTRY_03&           entry_03,
                        int                       key_04,
                        const ENTRY_04&           entry_04,
                        int                       key_05,
                        const ENTRY_05&           entry_05,
                        int                       key_06,
                        const ENTRY_06&           entry_06,
                        int                       key_07,
                        const ENTRY_07&           entry_07,
                        int                       key_08,
                        const ENTRY_08&           entry_08,
                        int                       key_09,
                        const ENTRY_09&           entry_09,
                        int                       key_10,
                        const ENTRY_10&           entry_10,
                        int                       key_11,
                        const ENTRY_11&           entry_11,
                        int                       key_12,
                        const ENTRY_12&           entry_12,
                        int                       key_13,
                        const ENTRY_13&           entry_13,
                        int                       key_14,
                        const ENTRY_14&           entry_14
                        ) const;

    template <typename TYPE, typename ENTRY_01,
                             typename ENTRY_02,
                             typename ENTRY_03,
                             typename ENTRY_04,
                             typename ENTRY_05,
                             typename ENTRY_06,
                             typename ENTRY_07,
                             typename ENTRY_08,
                             typename ENTRY_09,
                             typename ENTRY_10,
                             typename ENTRY_11,
                             typename ENTRY_12,
                             typename ENTRY_13,
                             typename ENTRY_14,
                             typename ENTRY_15>
    void pushBackHelper(bdld::DatumIntMapBuilder *builder,
                        int                       key,
                        const TYPE&               value,
                        int                       key_01,
                        const ENTRY_01&           entry_01,
                        int                       key_02,
                        const ENTRY_02&           entry_02,
                        int                       key_03,
                        const ENTRY_03&           entry_03,
                        int                       key_04,
                        const ENTRY_04&           entry_04,
                        int                       key_05,
                        const ENTRY_05&           entry_05,
                        int                       key_06,
                        const ENTRY_06&           entry_06,
                        int                       key_07,
                        const ENTRY_07&           entry_07,
                        int                       key_08,
                        const ENTRY_08&           entry_08,
                        int                       key_09,
                        const ENTRY_09&           entry_09,
                        int                       key_10,
                        const ENTRY_10&           entry_10,
                        int                       key_11,
                        const ENTRY_11&           entry_11,
                        int                       key_12,
                        const ENTRY_12&           entry_12,
                        int                       key_13,
                        const ENTRY_13&           entry_13,
                        int                       key_14,
                        const ENTRY_14&           entry_14,
                        int                       key_15,
                        const ENTRY_15&           entry_15
                        ) const;

    template <typename TYPE, typename ENTRY_01,
                             typename ENTRY_02,
                             typename ENTRY_03,
                             typename ENTRY_04,
                             typename ENTRY_05,
                             typename ENTRY_06,
                             typename ENTRY_07,
                             typename ENTRY_08,
                             typename ENTRY_09,
                             typename ENTRY_10,
                             typename ENTRY_11,
                             typename ENTRY_12,
                             typename ENTRY_13,
                             typename ENTRY_14,
                             typename ENTRY_15,
                             typename ENTRY_16>
    void pushBackHelper(bdld::DatumIntMapBuilder *builder,
                        int                       key,
                        const TYPE&               value,
                        int                       key_01,
                        const ENTRY_01&           entry_01,
                        int                       key_02,
                        const ENTRY_02&           entry_02,
                        int                       key_03,
                        const ENTRY_03&           entry_03,
                        int                       key_04,
                        const ENTRY_04&           entry_04,
                        int                       key_05,
                        const ENTRY_05&           entry_05,
                        int                       key_06,
                        const ENTRY_06&           entry_06,
                        int                       key_07,
                        const ENTRY_07&           entry_07,
                        int                       key_08,
                        const ENTRY_08&           entry_08,
                        int                       key_09,
                        const ENTRY_09&           entry_09,
                        int                       key_10,
                        const ENTRY_10&           entry_10,
                        int                       key_11,
                        const ENTRY_11&           entry_11,
                        int                       key_12,
                        const ENTRY_12&           entry_12,
                        int                       key_13,
                        const ENTRY_13&           entry_13,
                        int                       key_14,
                        const ENTRY_14&           entry_14,
                        int                       key_15,
                        const ENTRY_15&           entry_15,
                        int                       key_16,
                        const ENTRY_16&           entry_16
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

    bdld::Datum operator()(const bslmf::Nil&                  value) const;
    bdld::Datum operator()(int                                value) const;
    bdld::Datum operator()(double                             value) const;
    bdld::Datum operator()(bool                               value) const;
    bdld::Datum operator()(const bdld::DatumError&            value) const;
    bdld::Datum operator()(const bdlt::Date&                  value) const;
    bdld::Datum operator()(const bdlt::Time&                  value) const;
    bdld::Datum operator()(const bdlt::Datetime&              value) const;
    bdld::Datum operator()(const bdlt::DatetimeInterval&      value) const;
    bdld::Datum operator()(bdldfp::Decimal64                  value) const;
    bdld::Datum operator()(bsls::Types::Int64                 value) const;
    bdld::Datum operator()(const bdld::DatumUdt&              value) const;
    bdld::Datum operator()(const bdld::Datum&                 value) const;
    bdld::Datum operator()(const bdld::DatumArrayRef&         value) const;
    bdld::Datum operator()(const bdld::DatumMutableMapRef&    value) const;
    bdld::Datum operator()(const bdld::DatumMutableIntMapRef& value) const;
        // Return a 'bdld::Datum' having the specified 'value'.  Note that
        // where possible, no memory is allocated - array are returned as
        // references.  Note that 'DatumMapRef' and 'DatumIntMapRef' are not
        // supported at the moment.

    bdld::Datum operator()(const bdld::Datum         *elements,
                           int                        size) const;
    bdld::Datum operator()(const bdld::DatumMapEntry *elements,
                           int                        size,
                           bool                       sorted = false) const;
    bdld::Datum operator()(
                          const bdld::DatumIntMapEntry *elements,
                          int                           size,
                          bool                          sorted = false) const;
        // Return a 'bdld::Datum' having the specified 'size' number of
        // 'elements'.  Note that where possible, no memory is allocated -
        // arrays are returned as references.  Note that 'DatumMapRef' and
        // 'DatumIntMapRef' are not supported at the moment.

    bdld::Datum operator()(const bslstl::StringRef&  value) const;
    bdld::Datum operator()(const char               *value) const;
        // Return a 'bdld::Datum' having the specified 'value'.  The returned
        // 'bdld::Datum' object will contain a deep-copy of 'value'.

    template <class TYPE>
    bdld::Datum operator()(const bdlb::NullableValue<TYPE>& value) const;
        // Return a 'bdld::Datum' having the specified 'value', or null if
        // 'value' is unset.

#if !BSLS_COMPILERFEATURES_SIMULATE_VARIADIC_TEMPLATES
    template <typename... ELEMENTS>
    bdld::Datum a(const ELEMENTS&... elements) const;
        // Return a 'bdld::Datum' having an array value of the specified
        // 'elements'.

// IMPORTANT NOTE: The section below was manually modified to reduce the
// maximum number of parameters to 16.
#else
    bdld::Datum a() const;

    template <typename ELEMENTS_01>
    bdld::Datum a(const ELEMENTS_01& elements_01
                  ) const;

    template <typename ELEMENTS_01,
              typename ELEMENTS_02>
    bdld::Datum a(const ELEMENTS_01& elements_01,
                  const ELEMENTS_02& elements_02
                  ) const;

    template <typename ELEMENTS_01,
              typename ELEMENTS_02,
              typename ELEMENTS_03>
    bdld::Datum a(const ELEMENTS_01& elements_01,
                  const ELEMENTS_02& elements_02,
                  const ELEMENTS_03& elements_03
                  ) const;

    template <typename ELEMENTS_01,
              typename ELEMENTS_02,
              typename ELEMENTS_03,
              typename ELEMENTS_04>
    bdld::Datum a(const ELEMENTS_01& elements_01,
                  const ELEMENTS_02& elements_02,
                  const ELEMENTS_03& elements_03,
                  const ELEMENTS_04& elements_04
                  ) const;

    template <typename ELEMENTS_01,
              typename ELEMENTS_02,
              typename ELEMENTS_03,
              typename ELEMENTS_04,
              typename ELEMENTS_05>
    bdld::Datum a(const ELEMENTS_01& elements_01,
                  const ELEMENTS_02& elements_02,
                  const ELEMENTS_03& elements_03,
                  const ELEMENTS_04& elements_04,
                  const ELEMENTS_05& elements_05
                  ) const;

    template <typename ELEMENTS_01,
              typename ELEMENTS_02,
              typename ELEMENTS_03,
              typename ELEMENTS_04,
              typename ELEMENTS_05,
              typename ELEMENTS_06>
    bdld::Datum a(const ELEMENTS_01& elements_01,
                  const ELEMENTS_02& elements_02,
                  const ELEMENTS_03& elements_03,
                  const ELEMENTS_04& elements_04,
                  const ELEMENTS_05& elements_05,
                  const ELEMENTS_06& elements_06
                  ) const;

    template <typename ELEMENTS_01,
              typename ELEMENTS_02,
              typename ELEMENTS_03,
              typename ELEMENTS_04,
              typename ELEMENTS_05,
              typename ELEMENTS_06,
              typename ELEMENTS_07>
    bdld::Datum a(const ELEMENTS_01& elements_01,
                  const ELEMENTS_02& elements_02,
                  const ELEMENTS_03& elements_03,
                  const ELEMENTS_04& elements_04,
                  const ELEMENTS_05& elements_05,
                  const ELEMENTS_06& elements_06,
                  const ELEMENTS_07& elements_07
                  ) const;

    template <typename ELEMENTS_01,
              typename ELEMENTS_02,
              typename ELEMENTS_03,
              typename ELEMENTS_04,
              typename ELEMENTS_05,
              typename ELEMENTS_06,
              typename ELEMENTS_07,
              typename ELEMENTS_08>
    bdld::Datum a(const ELEMENTS_01& elements_01,
                  const ELEMENTS_02& elements_02,
                  const ELEMENTS_03& elements_03,
                  const ELEMENTS_04& elements_04,
                  const ELEMENTS_05& elements_05,
                  const ELEMENTS_06& elements_06,
                  const ELEMENTS_07& elements_07,
                  const ELEMENTS_08& elements_08
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
    bdld::Datum a(const ELEMENTS_01& elements_01,
                  const ELEMENTS_02& elements_02,
                  const ELEMENTS_03& elements_03,
                  const ELEMENTS_04& elements_04,
                  const ELEMENTS_05& elements_05,
                  const ELEMENTS_06& elements_06,
                  const ELEMENTS_07& elements_07,
                  const ELEMENTS_08& elements_08,
                  const ELEMENTS_09& elements_09
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
    bdld::Datum a(const ELEMENTS_01& elements_01,
                  const ELEMENTS_02& elements_02,
                  const ELEMENTS_03& elements_03,
                  const ELEMENTS_04& elements_04,
                  const ELEMENTS_05& elements_05,
                  const ELEMENTS_06& elements_06,
                  const ELEMENTS_07& elements_07,
                  const ELEMENTS_08& elements_08,
                  const ELEMENTS_09& elements_09,
                  const ELEMENTS_10& elements_10
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
    bdld::Datum a(const ELEMENTS_01& elements_01,
                  const ELEMENTS_02& elements_02,
                  const ELEMENTS_03& elements_03,
                  const ELEMENTS_04& elements_04,
                  const ELEMENTS_05& elements_05,
                  const ELEMENTS_06& elements_06,
                  const ELEMENTS_07& elements_07,
                  const ELEMENTS_08& elements_08,
                  const ELEMENTS_09& elements_09,
                  const ELEMENTS_10& elements_10,
                  const ELEMENTS_11& elements_11
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
    bdld::Datum a(const ELEMENTS_01& elements_01,
                  const ELEMENTS_02& elements_02,
                  const ELEMENTS_03& elements_03,
                  const ELEMENTS_04& elements_04,
                  const ELEMENTS_05& elements_05,
                  const ELEMENTS_06& elements_06,
                  const ELEMENTS_07& elements_07,
                  const ELEMENTS_08& elements_08,
                  const ELEMENTS_09& elements_09,
                  const ELEMENTS_10& elements_10,
                  const ELEMENTS_11& elements_11,
                  const ELEMENTS_12& elements_12
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
    bdld::Datum a(const ELEMENTS_01& elements_01,
                  const ELEMENTS_02& elements_02,
                  const ELEMENTS_03& elements_03,
                  const ELEMENTS_04& elements_04,
                  const ELEMENTS_05& elements_05,
                  const ELEMENTS_06& elements_06,
                  const ELEMENTS_07& elements_07,
                  const ELEMENTS_08& elements_08,
                  const ELEMENTS_09& elements_09,
                  const ELEMENTS_10& elements_10,
                  const ELEMENTS_11& elements_11,
                  const ELEMENTS_12& elements_12,
                  const ELEMENTS_13& elements_13
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
    bdld::Datum a(const ELEMENTS_01& elements_01,
                  const ELEMENTS_02& elements_02,
                  const ELEMENTS_03& elements_03,
                  const ELEMENTS_04& elements_04,
                  const ELEMENTS_05& elements_05,
                  const ELEMENTS_06& elements_06,
                  const ELEMENTS_07& elements_07,
                  const ELEMENTS_08& elements_08,
                  const ELEMENTS_09& elements_09,
                  const ELEMENTS_10& elements_10,
                  const ELEMENTS_11& elements_11,
                  const ELEMENTS_12& elements_12,
                  const ELEMENTS_13& elements_13,
                  const ELEMENTS_14& elements_14
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
    bdld::Datum a(const ELEMENTS_01& elements_01,
                  const ELEMENTS_02& elements_02,
                  const ELEMENTS_03& elements_03,
                  const ELEMENTS_04& elements_04,
                  const ELEMENTS_05& elements_05,
                  const ELEMENTS_06& elements_06,
                  const ELEMENTS_07& elements_07,
                  const ELEMENTS_08& elements_08,
                  const ELEMENTS_09& elements_09,
                  const ELEMENTS_10& elements_10,
                  const ELEMENTS_11& elements_11,
                  const ELEMENTS_12& elements_12,
                  const ELEMENTS_13& elements_13,
                  const ELEMENTS_14& elements_14,
                  const ELEMENTS_15& elements_15
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
    bdld::Datum a(const ELEMENTS_01& elements_01,
                  const ELEMENTS_02& elements_02,
                  const ELEMENTS_03& elements_03,
                  const ELEMENTS_04& elements_04,
                  const ELEMENTS_05& elements_05,
                  const ELEMENTS_06& elements_06,
                  const ELEMENTS_07& elements_07,
                  const ELEMENTS_08& elements_08,
                  const ELEMENTS_09& elements_09,
                  const ELEMENTS_10& elements_10,
                  const ELEMENTS_11& elements_11,
                  const ELEMENTS_12& elements_12,
                  const ELEMENTS_13& elements_13,
                  const ELEMENTS_14& elements_14,
                  const ELEMENTS_15& elements_15,
                  const ELEMENTS_16& elements_16
                  ) const;

#endif

#if !BSLS_COMPILERFEATURES_SIMULATE_VARIADIC_TEMPLATES
    template <typename... ENTRIES>
    bdld::Datum m(const ENTRIES&... entries) const;
        // Return a 'bdld::Datum' object containing a map of the specified
        // 'entries'.  The 'entries' are supplied as pairs (odd number of
        // 'sizeof...(entries)' being an error) where the first specified
        // element is the key, and the second is its corresponding value.  The
        // behavior is undefined if the same key is supplied more than once.
#else
    bdld::Datum m() const;

    template <typename ENTRIES_01,
              typename ENTRIES_02>
    bdld::Datum m(const ENTRIES_01& entrie_01,
                  const ENTRIES_02& entrie_02
                  ) const;

    template <typename ENTRIES_01,
              typename ENTRIES_02,
              typename ENTRIES_03,
              typename ENTRIES_04>
    bdld::Datum m(const ENTRIES_01& entries_01,
                  const ENTRIES_02& entries_02,
                  const ENTRIES_03& entries_03,
                  const ENTRIES_04& entries_04
                  ) const;

    template <typename ENTRIES_01,
              typename ENTRIES_02,
              typename ENTRIES_03,
              typename ENTRIES_04,
              typename ENTRIES_05,
              typename ENTRIES_06>
    bdld::Datum m(const ENTRIES_01& entries_01,
                  const ENTRIES_02& entries_02,
                  const ENTRIES_03& entries_03,
                  const ENTRIES_04& entries_04,
                  const ENTRIES_05& entries_05,
                  const ENTRIES_06& entries_06
                  ) const;

    template <typename ENTRIES_01,
              typename ENTRIES_02,
              typename ENTRIES_03,
              typename ENTRIES_04,
              typename ENTRIES_05,
              typename ENTRIES_06,
              typename ENTRIES_07,
              typename ENTRIES_08>
    bdld::Datum m(const ENTRIES_01& entries_01,
                  const ENTRIES_02& entries_02,
                  const ENTRIES_03& entries_03,
                  const ENTRIES_04& entries_04,
                  const ENTRIES_05& entries_05,
                  const ENTRIES_06& entries_06,
                  const ENTRIES_07& entries_07,
                  const ENTRIES_08& entries_08
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
    bdld::Datum m(const ENTRIES_01& entries_01,
                  const ENTRIES_02& entries_02,
                  const ENTRIES_03& entries_03,
                  const ENTRIES_04& entries_04,
                  const ENTRIES_05& entries_05,
                  const ENTRIES_06& entries_06,
                  const ENTRIES_07& entries_07,
                  const ENTRIES_08& entries_08,
                  const ENTRIES_09& entries_09,
                  const ENTRIES_10& entries_10
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
    bdld::Datum m(const ENTRIES_01& entries_01,
                  const ENTRIES_02& entries_02,
                  const ENTRIES_03& entries_03,
                  const ENTRIES_04& entries_04,
                  const ENTRIES_05& entries_05,
                  const ENTRIES_06& entries_06,
                  const ENTRIES_07& entries_07,
                  const ENTRIES_08& entries_08,
                  const ENTRIES_09& entries_09,
                  const ENTRIES_10& entries_10,
                  const ENTRIES_11& entries_11,
                  const ENTRIES_12& entries_12
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
    bdld::Datum m(const ENTRIES_01& entries_01,
                  const ENTRIES_02& entries_02,
                  const ENTRIES_03& entries_03,
                  const ENTRIES_04& entries_04,
                  const ENTRIES_05& entries_05,
                  const ENTRIES_06& entries_06,
                  const ENTRIES_07& entries_07,
                  const ENTRIES_08& entries_08,
                  const ENTRIES_09& entries_09,
                  const ENTRIES_10& entries_10,
                  const ENTRIES_11& entries_11,
                  const ENTRIES_12& entries_12,
                  const ENTRIES_13& entries_13,
                  const ENTRIES_14& entries_14
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
    bdld::Datum m(const ENTRIES_01& entries_01,
                  const ENTRIES_02& entries_02,
                  const ENTRIES_03& entries_03,
                  const ENTRIES_04& entries_04,
                  const ENTRIES_05& entries_05,
                  const ENTRIES_06& entries_06,
                  const ENTRIES_07& entries_07,
                  const ENTRIES_08& entries_08,
                  const ENTRIES_09& entries_09,
                  const ENTRIES_10& entries_10,
                  const ENTRIES_11& entries_11,
                  const ENTRIES_12& entries_12,
                  const ENTRIES_13& entries_13,
                  const ENTRIES_14& entries_14,
                  const ENTRIES_15& entries_15,
                  const ENTRIES_16& entries_16
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
    bdld::Datum m(const ENTRIES_01& entries_01,
                  const ENTRIES_02& entries_02,
                  const ENTRIES_03& entries_03,
                  const ENTRIES_04& entries_04,
                  const ENTRIES_05& entries_05,
                  const ENTRIES_06& entries_06,
                  const ENTRIES_07& entries_07,
                  const ENTRIES_08& entries_08,
                  const ENTRIES_09& entries_09,
                  const ENTRIES_10& entries_10,
                  const ENTRIES_11& entries_11,
                  const ENTRIES_12& entries_12,
                  const ENTRIES_13& entries_13,
                  const ENTRIES_14& entries_14,
                  const ENTRIES_15& entries_15,
                  const ENTRIES_16& entries_16,
                  const ENTRIES_17& entries_17,
                  const ENTRIES_18& entries_18
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
    bdld::Datum m(const ENTRIES_01& entries_01,
                  const ENTRIES_02& entries_02,
                  const ENTRIES_03& entries_03,
                  const ENTRIES_04& entries_04,
                  const ENTRIES_05& entries_05,
                  const ENTRIES_06& entries_06,
                  const ENTRIES_07& entries_07,
                  const ENTRIES_08& entries_08,
                  const ENTRIES_09& entries_09,
                  const ENTRIES_10& entries_10,
                  const ENTRIES_11& entries_11,
                  const ENTRIES_12& entries_12,
                  const ENTRIES_13& entries_13,
                  const ENTRIES_14& entries_14,
                  const ENTRIES_15& entries_15,
                  const ENTRIES_16& entries_16,
                  const ENTRIES_17& entries_17,
                  const ENTRIES_18& entries_18,
                  const ENTRIES_19& entries_19,
                  const ENTRIES_20& entries_20
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
    bdld::Datum m(const ENTRIES_01& entries_01,
                  const ENTRIES_02& entries_02,
                  const ENTRIES_03& entries_03,
                  const ENTRIES_04& entries_04,
                  const ENTRIES_05& entries_05,
                  const ENTRIES_06& entries_06,
                  const ENTRIES_07& entries_07,
                  const ENTRIES_08& entries_08,
                  const ENTRIES_09& entries_09,
                  const ENTRIES_10& entries_10,
                  const ENTRIES_11& entries_11,
                  const ENTRIES_12& entries_12,
                  const ENTRIES_13& entries_13,
                  const ENTRIES_14& entries_14,
                  const ENTRIES_15& entries_15,
                  const ENTRIES_16& entries_16,
                  const ENTRIES_17& entries_17,
                  const ENTRIES_18& entries_18,
                  const ENTRIES_19& entries_19,
                  const ENTRIES_20& entries_20,
                  const ENTRIES_21& entries_21,
                  const ENTRIES_22& entries_22
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
    bdld::Datum m(const ENTRIES_01& entries_01,
                  const ENTRIES_02& entries_02,
                  const ENTRIES_03& entries_03,
                  const ENTRIES_04& entries_04,
                  const ENTRIES_05& entries_05,
                  const ENTRIES_06& entries_06,
                  const ENTRIES_07& entries_07,
                  const ENTRIES_08& entries_08,
                  const ENTRIES_09& entries_09,
                  const ENTRIES_10& entries_10,
                  const ENTRIES_11& entries_11,
                  const ENTRIES_12& entries_12,
                  const ENTRIES_13& entries_13,
                  const ENTRIES_14& entries_14,
                  const ENTRIES_15& entries_15,
                  const ENTRIES_16& entries_16,
                  const ENTRIES_17& entries_17,
                  const ENTRIES_18& entries_18,
                  const ENTRIES_19& entries_19,
                  const ENTRIES_20& entries_20,
                  const ENTRIES_21& entries_21,
                  const ENTRIES_22& entries_22,
                  const ENTRIES_23& entries_23,
                  const ENTRIES_24& entries_24
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
    bdld::Datum m(const ENTRIES_01& entries_01,
                  const ENTRIES_02& entries_02,
                  const ENTRIES_03& entries_03,
                  const ENTRIES_04& entries_04,
                  const ENTRIES_05& entries_05,
                  const ENTRIES_06& entries_06,
                  const ENTRIES_07& entries_07,
                  const ENTRIES_08& entries_08,
                  const ENTRIES_09& entries_09,
                  const ENTRIES_10& entries_10,
                  const ENTRIES_11& entries_11,
                  const ENTRIES_12& entries_12,
                  const ENTRIES_13& entries_13,
                  const ENTRIES_14& entries_14,
                  const ENTRIES_15& entries_15,
                  const ENTRIES_16& entries_16,
                  const ENTRIES_17& entries_17,
                  const ENTRIES_18& entries_18,
                  const ENTRIES_19& entries_19,
                  const ENTRIES_20& entries_20,
                  const ENTRIES_21& entries_21,
                  const ENTRIES_22& entries_22,
                  const ENTRIES_23& entries_23,
                  const ENTRIES_24& entries_24,
                  const ENTRIES_25& entries_25,
                  const ENTRIES_26& entries_26
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
    bdld::Datum m(const ENTRIES_01& entries_01,
                  const ENTRIES_02& entries_02,
                  const ENTRIES_03& entries_03,
                  const ENTRIES_04& entries_04,
                  const ENTRIES_05& entries_05,
                  const ENTRIES_06& entries_06,
                  const ENTRIES_07& entries_07,
                  const ENTRIES_08& entries_08,
                  const ENTRIES_09& entries_09,
                  const ENTRIES_10& entries_10,
                  const ENTRIES_11& entries_11,
                  const ENTRIES_12& entries_12,
                  const ENTRIES_13& entries_13,
                  const ENTRIES_14& entries_14,
                  const ENTRIES_15& entries_15,
                  const ENTRIES_16& entries_16,
                  const ENTRIES_17& entries_17,
                  const ENTRIES_18& entries_18,
                  const ENTRIES_19& entries_19,
                  const ENTRIES_20& entries_20,
                  const ENTRIES_21& entries_21,
                  const ENTRIES_22& entries_22,
                  const ENTRIES_23& entries_23,
                  const ENTRIES_24& entries_24,
                  const ENTRIES_25& entries_25,
                  const ENTRIES_26& entries_26,
                  const ENTRIES_27& entries_27,
                  const ENTRIES_28& entries_28
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
    bdld::Datum m(const ENTRIES_01& entries_01,
                  const ENTRIES_02& entries_02,
                  const ENTRIES_03& entries_03,
                  const ENTRIES_04& entries_04,
                  const ENTRIES_05& entries_05,
                  const ENTRIES_06& entries_06,
                  const ENTRIES_07& entries_07,
                  const ENTRIES_08& entries_08,
                  const ENTRIES_09& entries_09,
                  const ENTRIES_10& entries_10,
                  const ENTRIES_11& entries_11,
                  const ENTRIES_12& entries_12,
                  const ENTRIES_13& entries_13,
                  const ENTRIES_14& entries_14,
                  const ENTRIES_15& entries_15,
                  const ENTRIES_16& entries_16,
                  const ENTRIES_17& entries_17,
                  const ENTRIES_18& entries_18,
                  const ENTRIES_19& entries_19,
                  const ENTRIES_20& entries_20,
                  const ENTRIES_21& entries_21,
                  const ENTRIES_22& entries_22,
                  const ENTRIES_23& entries_23,
                  const ENTRIES_24& entries_24,
                  const ENTRIES_25& entries_25,
                  const ENTRIES_26& entries_26,
                  const ENTRIES_27& entries_27,
                  const ENTRIES_28& entries_28,
                  const ENTRIES_29& entries_29,
                  const ENTRIES_30& entries_30
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
    bdld::Datum m(const ENTRIES_01& entries_01,
                  const ENTRIES_02& entries_02,
                  const ENTRIES_03& entries_03,
                  const ENTRIES_04& entries_04,
                  const ENTRIES_05& entries_05,
                  const ENTRIES_06& entries_06,
                  const ENTRIES_07& entries_07,
                  const ENTRIES_08& entries_08,
                  const ENTRIES_09& entries_09,
                  const ENTRIES_10& entries_10,
                  const ENTRIES_11& entries_11,
                  const ENTRIES_12& entries_12,
                  const ENTRIES_13& entries_13,
                  const ENTRIES_14& entries_14,
                  const ENTRIES_15& entries_15,
                  const ENTRIES_16& entries_16,
                  const ENTRIES_17& entries_17,
                  const ENTRIES_18& entries_18,
                  const ENTRIES_19& entries_19,
                  const ENTRIES_20& entries_20,
                  const ENTRIES_21& entries_21,
                  const ENTRIES_22& entries_22,
                  const ENTRIES_23& entries_23,
                  const ENTRIES_24& entries_24,
                  const ENTRIES_25& entries_25,
                  const ENTRIES_26& entries_26,
                  const ENTRIES_27& entries_27,
                  const ENTRIES_28& entries_28,
                  const ENTRIES_29& entries_29,
                  const ENTRIES_30& entries_30,
                  const ENTRIES_31& entries_31,
                  const ENTRIES_32& entries_32
                  ) const;

#endif

#if !BSLS_COMPILERFEATURES_SIMULATE_VARIADIC_TEMPLATES
    template <typename... ENTRIES>
    bdld::Datum mok(const ENTRIES&... entries) const;
        // Return a 'bdld::Datum' object containing a map with owned keys
        // consisting of the specified 'entries'.  The 'entries' are supplied
        // as pairs (odd number of 'sizeof...(entries)' being an error) where
        // the first specified element is the key, and the second is its
        // corresponding value.  The behavior is undefined if the same key is
        // supplied more than once.
#else
    bdld::Datum mok() const;

    template <typename ENTRIES_01,
              typename ENTRIES_02>
    bdld::Datum mok(const ENTRIES_01& entries_01,
                    const ENTRIES_02& entries_02
                    ) const;

    template <typename ENTRIES_01,
              typename ENTRIES_02,
              typename ENTRIES_03,
              typename ENTRIES_04>
    bdld::Datum mok(const ENTRIES_01& entries_01,
                    const ENTRIES_02& entries_02,
                    const ENTRIES_03& entries_03,
                    const ENTRIES_04& entries_04
                    ) const;

    template <typename ENTRIES_01,
              typename ENTRIES_02,
              typename ENTRIES_03,
              typename ENTRIES_04,
              typename ENTRIES_05,
              typename ENTRIES_06>
    bdld::Datum mok(const ENTRIES_01& entries_01,
                    const ENTRIES_02& entries_02,
                    const ENTRIES_03& entries_03,
                    const ENTRIES_04& entries_04,
                    const ENTRIES_05& entries_05,
                    const ENTRIES_06& entries_06
                    ) const;

    template <typename ENTRIES_01,
              typename ENTRIES_02,
              typename ENTRIES_03,
              typename ENTRIES_04,
              typename ENTRIES_05,
              typename ENTRIES_06,
              typename ENTRIES_07,
              typename ENTRIES_08>
    bdld::Datum mok(const ENTRIES_01& entries_01,
                    const ENTRIES_02& entries_02,
                    const ENTRIES_03& entries_03,
                    const ENTRIES_04& entries_04,
                    const ENTRIES_05& entries_05,
                    const ENTRIES_06& entries_06,
                    const ENTRIES_07& entries_07,
                    const ENTRIES_08& entries_08
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
    bdld::Datum mok(const ENTRIES_01& entries_01,
                    const ENTRIES_02& entries_02,
                    const ENTRIES_03& entries_03,
                    const ENTRIES_04& entries_04,
                    const ENTRIES_05& entries_05,
                    const ENTRIES_06& entries_06,
                    const ENTRIES_07& entries_07,
                    const ENTRIES_08& entries_08,
                    const ENTRIES_09& entries_09,
                    const ENTRIES_10& entries_10
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
    bdld::Datum mok(const ENTRIES_01& entries_01,
                    const ENTRIES_02& entries_02,
                    const ENTRIES_03& entries_03,
                    const ENTRIES_04& entries_04,
                    const ENTRIES_05& entries_05,
                    const ENTRIES_06& entries_06,
                    const ENTRIES_07& entries_07,
                    const ENTRIES_08& entries_08,
                    const ENTRIES_09& entries_09,
                    const ENTRIES_10& entries_10,
                    const ENTRIES_11& entries_11,
                    const ENTRIES_12& entries_12
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
    bdld::Datum mok(const ENTRIES_01& entries_01,
                    const ENTRIES_02& entries_02,
                    const ENTRIES_03& entries_03,
                    const ENTRIES_04& entries_04,
                    const ENTRIES_05& entries_05,
                    const ENTRIES_06& entries_06,
                    const ENTRIES_07& entries_07,
                    const ENTRIES_08& entries_08,
                    const ENTRIES_09& entries_09,
                    const ENTRIES_10& entries_10,
                    const ENTRIES_11& entries_11,
                    const ENTRIES_12& entries_12,
                    const ENTRIES_13& entries_13,
                    const ENTRIES_14& entries_14
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
    bdld::Datum mok(const ENTRIES_01& entries_01,
                    const ENTRIES_02& entries_02,
                    const ENTRIES_03& entries_03,
                    const ENTRIES_04& entries_04,
                    const ENTRIES_05& entries_05,
                    const ENTRIES_06& entries_06,
                    const ENTRIES_07& entries_07,
                    const ENTRIES_08& entries_08,
                    const ENTRIES_09& entries_09,
                    const ENTRIES_10& entries_10,
                    const ENTRIES_11& entries_11,
                    const ENTRIES_12& entries_12,
                    const ENTRIES_13& entries_13,
                    const ENTRIES_14& entries_14,
                    const ENTRIES_15& entries_15,
                    const ENTRIES_16& entries_16
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
    bdld::Datum mok(const ENTRIES_01& entries_01,
                    const ENTRIES_02& entries_02,
                    const ENTRIES_03& entries_03,
                    const ENTRIES_04& entries_04,
                    const ENTRIES_05& entries_05,
                    const ENTRIES_06& entries_06,
                    const ENTRIES_07& entries_07,
                    const ENTRIES_08& entries_08,
                    const ENTRIES_09& entries_09,
                    const ENTRIES_10& entries_10,
                    const ENTRIES_11& entries_11,
                    const ENTRIES_12& entries_12,
                    const ENTRIES_13& entries_13,
                    const ENTRIES_14& entries_14,
                    const ENTRIES_15& entries_15,
                    const ENTRIES_16& entries_16,
                    const ENTRIES_17& entries_17,
                    const ENTRIES_18& entries_18
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
    bdld::Datum mok(const ENTRIES_01& entries_01,
                    const ENTRIES_02& entries_02,
                    const ENTRIES_03& entries_03,
                    const ENTRIES_04& entries_04,
                    const ENTRIES_05& entries_05,
                    const ENTRIES_06& entries_06,
                    const ENTRIES_07& entries_07,
                    const ENTRIES_08& entries_08,
                    const ENTRIES_09& entries_09,
                    const ENTRIES_10& entries_10,
                    const ENTRIES_11& entries_11,
                    const ENTRIES_12& entries_12,
                    const ENTRIES_13& entries_13,
                    const ENTRIES_14& entries_14,
                    const ENTRIES_15& entries_15,
                    const ENTRIES_16& entries_16,
                    const ENTRIES_17& entries_17,
                    const ENTRIES_18& entries_18,
                    const ENTRIES_19& entries_19,
                    const ENTRIES_20& entries_20
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
    bdld::Datum mok(const ENTRIES_01& entries_01,
                    const ENTRIES_02& entries_02,
                    const ENTRIES_03& entries_03,
                    const ENTRIES_04& entries_04,
                    const ENTRIES_05& entries_05,
                    const ENTRIES_06& entries_06,
                    const ENTRIES_07& entries_07,
                    const ENTRIES_08& entries_08,
                    const ENTRIES_09& entries_09,
                    const ENTRIES_10& entries_10,
                    const ENTRIES_11& entries_11,
                    const ENTRIES_12& entries_12,
                    const ENTRIES_13& entries_13,
                    const ENTRIES_14& entries_14,
                    const ENTRIES_15& entries_15,
                    const ENTRIES_16& entries_16,
                    const ENTRIES_17& entries_17,
                    const ENTRIES_18& entries_18,
                    const ENTRIES_19& entries_19,
                    const ENTRIES_20& entries_20,
                    const ENTRIES_21& entries_21,
                    const ENTRIES_22& entries_22
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
    bdld::Datum mok(const ENTRIES_01& entries_01,
                    const ENTRIES_02& entries_02,
                    const ENTRIES_03& entries_03,
                    const ENTRIES_04& entries_04,
                    const ENTRIES_05& entries_05,
                    const ENTRIES_06& entries_06,
                    const ENTRIES_07& entries_07,
                    const ENTRIES_08& entries_08,
                    const ENTRIES_09& entries_09,
                    const ENTRIES_10& entries_10,
                    const ENTRIES_11& entries_11,
                    const ENTRIES_12& entries_12,
                    const ENTRIES_13& entries_13,
                    const ENTRIES_14& entries_14,
                    const ENTRIES_15& entries_15,
                    const ENTRIES_16& entries_16,
                    const ENTRIES_17& entries_17,
                    const ENTRIES_18& entries_18,
                    const ENTRIES_19& entries_19,
                    const ENTRIES_20& entries_20,
                    const ENTRIES_21& entries_21,
                    const ENTRIES_22& entries_22,
                    const ENTRIES_23& entries_23,
                    const ENTRIES_24& entries_24
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
    bdld::Datum mok(const ENTRIES_01& entries_01,
                    const ENTRIES_02& entries_02,
                    const ENTRIES_03& entries_03,
                    const ENTRIES_04& entries_04,
                    const ENTRIES_05& entries_05,
                    const ENTRIES_06& entries_06,
                    const ENTRIES_07& entries_07,
                    const ENTRIES_08& entries_08,
                    const ENTRIES_09& entries_09,
                    const ENTRIES_10& entries_10,
                    const ENTRIES_11& entries_11,
                    const ENTRIES_12& entries_12,
                    const ENTRIES_13& entries_13,
                    const ENTRIES_14& entries_14,
                    const ENTRIES_15& entries_15,
                    const ENTRIES_16& entries_16,
                    const ENTRIES_17& entries_17,
                    const ENTRIES_18& entries_18,
                    const ENTRIES_19& entries_19,
                    const ENTRIES_20& entries_20,
                    const ENTRIES_21& entries_21,
                    const ENTRIES_22& entries_22,
                    const ENTRIES_23& entries_23,
                    const ENTRIES_24& entries_24,
                    const ENTRIES_25& entries_25,
                    const ENTRIES_26& entries_26
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
    bdld::Datum mok(const ENTRIES_01& entries_01,
                    const ENTRIES_02& entries_02,
                    const ENTRIES_03& entries_03,
                    const ENTRIES_04& entries_04,
                    const ENTRIES_05& entries_05,
                    const ENTRIES_06& entries_06,
                    const ENTRIES_07& entries_07,
                    const ENTRIES_08& entries_08,
                    const ENTRIES_09& entries_09,
                    const ENTRIES_10& entries_10,
                    const ENTRIES_11& entries_11,
                    const ENTRIES_12& entries_12,
                    const ENTRIES_13& entries_13,
                    const ENTRIES_14& entries_14,
                    const ENTRIES_15& entries_15,
                    const ENTRIES_16& entries_16,
                    const ENTRIES_17& entries_17,
                    const ENTRIES_18& entries_18,
                    const ENTRIES_19& entries_19,
                    const ENTRIES_20& entries_20,
                    const ENTRIES_21& entries_21,
                    const ENTRIES_22& entries_22,
                    const ENTRIES_23& entries_23,
                    const ENTRIES_24& entries_24,
                    const ENTRIES_25& entries_25,
                    const ENTRIES_26& entries_26,
                    const ENTRIES_27& entries_27,
                    const ENTRIES_28& entries_28
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
    bdld::Datum mok(const ENTRIES_01& entries_01,
                    const ENTRIES_02& entries_02,
                    const ENTRIES_03& entries_03,
                    const ENTRIES_04& entries_04,
                    const ENTRIES_05& entries_05,
                    const ENTRIES_06& entries_06,
                    const ENTRIES_07& entries_07,
                    const ENTRIES_08& entries_08,
                    const ENTRIES_09& entries_09,
                    const ENTRIES_10& entries_10,
                    const ENTRIES_11& entries_11,
                    const ENTRIES_12& entries_12,
                    const ENTRIES_13& entries_13,
                    const ENTRIES_14& entries_14,
                    const ENTRIES_15& entries_15,
                    const ENTRIES_16& entries_16,
                    const ENTRIES_17& entries_17,
                    const ENTRIES_18& entries_18,
                    const ENTRIES_19& entries_19,
                    const ENTRIES_20& entries_20,
                    const ENTRIES_21& entries_21,
                    const ENTRIES_22& entries_22,
                    const ENTRIES_23& entries_23,
                    const ENTRIES_24& entries_24,
                    const ENTRIES_25& entries_25,
                    const ENTRIES_26& entries_26,
                    const ENTRIES_27& entries_27,
                    const ENTRIES_28& entries_28,
                    const ENTRIES_29& entries_29,
                    const ENTRIES_30& entries_30
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
    bdld::Datum mok(const ENTRIES_01& entries_01,
                    const ENTRIES_02& entries_02,
                    const ENTRIES_03& entries_03,
                    const ENTRIES_04& entries_04,
                    const ENTRIES_05& entries_05,
                    const ENTRIES_06& entries_06,
                    const ENTRIES_07& entries_07,
                    const ENTRIES_08& entries_08,
                    const ENTRIES_09& entries_09,
                    const ENTRIES_10& entries_10,
                    const ENTRIES_11& entries_11,
                    const ENTRIES_12& entries_12,
                    const ENTRIES_13& entries_13,
                    const ENTRIES_14& entries_14,
                    const ENTRIES_15& entries_15,
                    const ENTRIES_16& entries_16,
                    const ENTRIES_17& entries_17,
                    const ENTRIES_18& entries_18,
                    const ENTRIES_19& entries_19,
                    const ENTRIES_20& entries_20,
                    const ENTRIES_21& entries_21,
                    const ENTRIES_22& entries_22,
                    const ENTRIES_23& entries_23,
                    const ENTRIES_24& entries_24,
                    const ENTRIES_25& entries_25,
                    const ENTRIES_26& entries_26,
                    const ENTRIES_27& entries_27,
                    const ENTRIES_28& entries_28,
                    const ENTRIES_29& entries_29,
                    const ENTRIES_30& entries_30,
                    const ENTRIES_31& entries_31,
                    const ENTRIES_32& entries_32
                    ) const;

#endif


#if !BSLS_COMPILERFEATURES_SIMULATE_VARIADIC_TEMPLATES
    template <typename... ENTRIES>
    bdld::Datum im(const ENTRIES&... entries) const;
        // Return a 'bdld::Datum' object containing an integer-map of the
        // specified 'entries'.  The 'entries' are supplied  in pairs
        // (supplying an odd number will result in a compilation failure) where
        // the first supplied argument is an integer key, and the second is its
        // corresponding value.  The behavior is undefined if the same key is
        // supplied more than once.
#else
    bdld::Datum im() const;

    template <typename ENTRIES_01>
    bdld::Datum im(int               key_01,
                   const ENTRIES_01& entries_01
                   ) const;

    template <typename ENTRIES_01,
              typename ENTRIES_02>
    bdld::Datum im(int               key_01,
                   const ENTRIES_01& entries_01,
                   int               key_02,
                   const ENTRIES_02& entries_02
                   ) const;

    template <typename ENTRIES_01,
              typename ENTRIES_02,
              typename ENTRIES_03>
    bdld::Datum im(int               key_01,
                   const ENTRIES_01& entries_01,
                   int               key_02,
                   const ENTRIES_02& entries_02,
                   int               key_03,
                   const ENTRIES_03& entries_03
                   ) const;

    template <typename ENTRIES_01,
              typename ENTRIES_02,
              typename ENTRIES_03,
              typename ENTRIES_04>
    bdld::Datum im(int               key_01,
                   const ENTRIES_01& entries_01,
                   int               key_02,
                   const ENTRIES_02& entries_02,
                   int               key_03,
                   const ENTRIES_03& entries_03,
                   int               key_04,
                   const ENTRIES_04& entries_04
                   ) const;

    template <typename ENTRIES_01,
              typename ENTRIES_02,
              typename ENTRIES_03,
              typename ENTRIES_04,
              typename ENTRIES_05>
    bdld::Datum im(int               key_01,
                   const ENTRIES_01& entries_01,
                   int               key_02,
                   const ENTRIES_02& entries_02,
                   int               key_03,
                   const ENTRIES_03& entries_03,
                   int               key_04,
                   const ENTRIES_04& entries_04,
                   int               key_05,
                   const ENTRIES_05& entries_05
                   ) const;

    template <typename ENTRIES_01,
              typename ENTRIES_02,
              typename ENTRIES_03,
              typename ENTRIES_04,
              typename ENTRIES_05,
              typename ENTRIES_06>
    bdld::Datum im(int               key_01,
                   const ENTRIES_01& entries_01,
                   int               key_02,
                   const ENTRIES_02& entries_02,
                   int               key_03,
                   const ENTRIES_03& entries_03,
                   int               key_04,
                   const ENTRIES_04& entries_04,
                   int               key_05,
                   const ENTRIES_05& entries_05,
                   int               key_06,
                   const ENTRIES_06& entries_06
                   ) const;

    template <typename ENTRIES_01,
              typename ENTRIES_02,
              typename ENTRIES_03,
              typename ENTRIES_04,
              typename ENTRIES_05,
              typename ENTRIES_06,
              typename ENTRIES_07>
    bdld::Datum im(int               key_01,
                   const ENTRIES_01& entries_01,
                   int               key_02,
                   const ENTRIES_02& entries_02,
                   int               key_03,
                   const ENTRIES_03& entries_03,
                   int               key_04,
                   const ENTRIES_04& entries_04,
                   int               key_05,
                   const ENTRIES_05& entries_05,
                   int               key_06,
                   const ENTRIES_06& entries_06,
                   int               key_07,
                   const ENTRIES_07& entries_07
                   ) const;

    template <typename ENTRIES_01,
              typename ENTRIES_02,
              typename ENTRIES_03,
              typename ENTRIES_04,
              typename ENTRIES_05,
              typename ENTRIES_06,
              typename ENTRIES_07,
              typename ENTRIES_08>
    bdld::Datum im(int               key_01,
                   const ENTRIES_01& entries_01,
                   int               key_02,
                   const ENTRIES_02& entries_02,
                   int               key_03,
                   const ENTRIES_03& entries_03,
                   int               key_04,
                   const ENTRIES_04& entries_04,
                   int               key_05,
                   const ENTRIES_05& entries_05,
                   int               key_06,
                   const ENTRIES_06& entries_06,
                   int               key_07,
                   const ENTRIES_07& entries_07,
                   int               key_08,
                   const ENTRIES_08& entries_08
                   ) const;

    template <typename ENTRIES_01,
              typename ENTRIES_02,
              typename ENTRIES_03,
              typename ENTRIES_04,
              typename ENTRIES_05,
              typename ENTRIES_06,
              typename ENTRIES_07,
              typename ENTRIES_08,
              typename ENTRIES_09>
    bdld::Datum im(int               key_01,
                   const ENTRIES_01& entries_01,
                   int               key_02,
                   const ENTRIES_02& entries_02,
                   int               key_03,
                   const ENTRIES_03& entries_03,
                   int               key_04,
                   const ENTRIES_04& entries_04,
                   int               key_05,
                   const ENTRIES_05& entries_05,
                   int               key_06,
                   const ENTRIES_06& entries_06,
                   int               key_07,
                   const ENTRIES_07& entries_07,
                   int               key_08,
                   const ENTRIES_08& entries_08,
                   int               key_09,
                   const ENTRIES_09& entries_09
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
    bdld::Datum im(int               key_01,
                   const ENTRIES_01& entries_01,
                   int               key_02,
                   const ENTRIES_02& entries_02,
                   int               key_03,
                   const ENTRIES_03& entries_03,
                   int               key_04,
                   const ENTRIES_04& entries_04,
                   int               key_05,
                   const ENTRIES_05& entries_05,
                   int               key_06,
                   const ENTRIES_06& entries_06,
                   int               key_07,
                   const ENTRIES_07& entries_07,
                   int               key_08,
                   const ENTRIES_08& entries_08,
                   int               key_09,
                   const ENTRIES_09& entries_09,
                   int               key_10,
                   const ENTRIES_10& entries_10
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
              typename ENTRIES_11>
    bdld::Datum im(int               key_01,
                   const ENTRIES_01& entries_01,
                   int               key_02,
                   const ENTRIES_02& entries_02,
                   int               key_03,
                   const ENTRIES_03& entries_03,
                   int               key_04,
                   const ENTRIES_04& entries_04,
                   int               key_05,
                   const ENTRIES_05& entries_05,
                   int               key_06,
                   const ENTRIES_06& entries_06,
                   int               key_07,
                   const ENTRIES_07& entries_07,
                   int               key_08,
                   const ENTRIES_08& entries_08,
                   int               key_09,
                   const ENTRIES_09& entries_09,
                   int               key_10,
                   const ENTRIES_10& entries_10,
                   int               key_11,
                   const ENTRIES_11& entries_11
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
    bdld::Datum im(int               key_01,
                   const ENTRIES_01& entries_01,
                   int               key_02,
                   const ENTRIES_02& entries_02,
                   int               key_03,
                   const ENTRIES_03& entries_03,
                   int               key_04,
                   const ENTRIES_04& entries_04,
                   int               key_05,
                   const ENTRIES_05& entries_05,
                   int               key_06,
                   const ENTRIES_06& entries_06,
                   int               key_07,
                   const ENTRIES_07& entries_07,
                   int               key_08,
                   const ENTRIES_08& entries_08,
                   int               key_09,
                   const ENTRIES_09& entries_09,
                   int               key_10,
                   const ENTRIES_10& entries_10,
                   int               key_11,
                   const ENTRIES_11& entries_11,
                   int               key_12,
                   const ENTRIES_12& entries_12
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
              typename ENTRIES_13>
    bdld::Datum im(int               key_01,
                   const ENTRIES_01& entries_01,
                   int               key_02,
                   const ENTRIES_02& entries_02,
                   int               key_03,
                   const ENTRIES_03& entries_03,
                   int               key_04,
                   const ENTRIES_04& entries_04,
                   int               key_05,
                   const ENTRIES_05& entries_05,
                   int               key_06,
                   const ENTRIES_06& entries_06,
                   int               key_07,
                   const ENTRIES_07& entries_07,
                   int               key_08,
                   const ENTRIES_08& entries_08,
                   int               key_09,
                   const ENTRIES_09& entries_09,
                   int               key_10,
                   const ENTRIES_10& entries_10,
                   int               key_11,
                   const ENTRIES_11& entries_11,
                   int               key_12,
                   const ENTRIES_12& entries_12,
                   int               key_13,
                   const ENTRIES_13& entries_13
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
    bdld::Datum im(int               key_01,
                   const ENTRIES_01& entries_01,
                   int               key_02,
                   const ENTRIES_02& entries_02,
                   int               key_03,
                   const ENTRIES_03& entries_03,
                   int               key_04,
                   const ENTRIES_04& entries_04,
                   int               key_05,
                   const ENTRIES_05& entries_05,
                   int               key_06,
                   const ENTRIES_06& entries_06,
                   int               key_07,
                   const ENTRIES_07& entries_07,
                   int               key_08,
                   const ENTRIES_08& entries_08,
                   int               key_09,
                   const ENTRIES_09& entries_09,
                   int               key_10,
                   const ENTRIES_10& entries_10,
                   int               key_11,
                   const ENTRIES_11& entries_11,
                   int               key_12,
                   const ENTRIES_12& entries_12,
                   int               key_13,
                   const ENTRIES_13& entries_13,
                   int               key_14,
                   const ENTRIES_14& entries_14
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
              typename ENTRIES_15>
    bdld::Datum im(int               key_01,
                   const ENTRIES_01& entries_01,
                   int               key_02,
                   const ENTRIES_02& entries_02,
                   int               key_03,
                   const ENTRIES_03& entries_03,
                   int               key_04,
                   const ENTRIES_04& entries_04,
                   int               key_05,
                   const ENTRIES_05& entries_05,
                   int               key_06,
                   const ENTRIES_06& entries_06,
                   int               key_07,
                   const ENTRIES_07& entries_07,
                   int               key_08,
                   const ENTRIES_08& entries_08,
                   int               key_09,
                   const ENTRIES_09& entries_09,
                   int               key_10,
                   const ENTRIES_10& entries_10,
                   int               key_11,
                   const ENTRIES_11& entries_11,
                   int               key_12,
                   const ENTRIES_12& entries_12,
                   int               key_13,
                   const ENTRIES_13& entries_13,
                   int               key_14,
                   const ENTRIES_14& entries_14,
                   int               key_15,
                   const ENTRIES_15& entries_15
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
    bdld::Datum im(int               key_01,
                   const ENTRIES_01& entries_01,
                   int               key_02,
                   const ENTRIES_02& entries_02,
                   int               key_03,
                   const ENTRIES_03& entries_03,
                   int               key_04,
                   const ENTRIES_04& entries_04,
                   int               key_05,
                   const ENTRIES_05& entries_05,
                   int               key_06,
                   const ENTRIES_06& entries_06,
                   int               key_07,
                   const ENTRIES_07& entries_07,
                   int               key_08,
                   const ENTRIES_08& entries_08,
                   int               key_09,
                   const ENTRIES_09& entries_09,
                   int               key_10,
                   const ENTRIES_10& entries_10,
                   int               key_11,
                   const ENTRIES_11& entries_11,
                   int               key_12,
                   const ENTRIES_12& entries_12,
                   int               key_13,
                   const ENTRIES_13& entries_13,
                   int               key_14,
                   const ENTRIES_14& entries_14,
                   int               key_15,
                   const ENTRIES_15& entries_15,
                   int               key_16,
                   const ENTRIES_16& entries_16
                   ) const;

#endif

    bdld::Datum ref(const bslstl::StringRef& string) const;
        // Return a 'bdld::Datum' object that references, but does not own the
        // specified 'string', possibly using the allocator of this object to
        // obtain memory.  Note that this can be used to refer to string
        // literals.  See 'bdld::Datum::createStringRef()'.
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

inline
void DatumMaker::pushBackHelper(bdld::DatumIntMapBuilder *) const
{
}

#if !BSLS_COMPILERFEATURES_SIMULATE_VARIADIC_TEMPLATES
template <typename TYPE>
inline
void DatumMaker::pushBackHelper(bdld::DatumArrayBuilder *builder,
                                const TYPE&              element) const
{
    builder->pushBack((*this)(element));
}

template <typename TYPE, typename... ELEMENTS>
inline
void DatumMaker::pushBackHelper(bdld::DatumArrayBuilder *builder,
                                const TYPE&              element,
                                const ELEMENTS&...       elements) const
{
    builder->pushBack((*this)(element));
    pushBackHelper(builder, elements...);
}

template <typename TYPE>
inline
void DatumMaker::pushBackHelper(bdld::DatumMapBuilder    *builder,
                                const bslstl::StringRef&  key,
                                const TYPE&               value) const
{
    builder->pushBack(key, (*this)(value));
}

template <typename TYPE, typename... ENTRIES>
inline
void DatumMaker::pushBackHelper(bdld::DatumMapBuilder    *builder,
                                const bslstl::StringRef&  key,
                                const TYPE&               value,
                                const ENTRIES&...         entries) const
{
    builder->pushBack(key, (*this)(value));
    pushBackHelper(builder, entries...);
}

template <typename TYPE>
inline
void DatumMaker::pushBackHelper(bdld::DatumMapOwningKeysBuilder *builder,
                                const bslstl::StringRef&         key,
                                const TYPE&                      value) const
{
    builder->pushBack(key, (*this)(value));
}

template <typename TYPE, typename... ENTRIES>
inline
void DatumMaker::pushBackHelper(bdld::DatumMapOwningKeysBuilder *builder,
                                const bslstl::StringRef&         key,
                                const TYPE&                      value,
                                const ENTRIES&...                entries) const
{
    builder->pushBack(key, (*this)(value));
    pushBackHelper(builder, entries...);
}

template <typename TYPE>
inline
void DatumMaker::pushBackHelper(bdld::DatumIntMapBuilder *builder,
                                int                       key,
                                const TYPE&               value) const
{
    builder->pushBack(key, (*this)(value));
}

template <typename TYPE, typename... ENTRIES>
inline
void DatumMaker::pushBackHelper(bdld::DatumIntMapBuilder *builder,
                                int                       key,
                                const TYPE&               value,
                                const ENTRIES&...         entries) const
{
    builder->pushBack(key, (*this)(value));
    pushBackHelper(builder, entries...);
}
#else
template <typename TYPE>
inline
void DatumMaker::pushBackHelper(bdld::DatumArrayBuilder *builder,
                                const TYPE&              element
                                ) const
{
    builder->pushBack((*this)(element));
    pushBackHelper(builder);
}

template <typename TYPE, typename ELEMENTS_01>
inline
void DatumMaker::pushBackHelper(bdld::DatumArrayBuilder *builder,
                                const TYPE&              element,
                                const ELEMENTS_01&       elements_01
                                ) const
{
    builder->pushBack((*this)(element));
    pushBackHelper(builder, elements_01);
}

template <typename TYPE, typename ELEMENTS_01,
                         typename ELEMENTS_02>
inline
void DatumMaker::pushBackHelper(bdld::DatumArrayBuilder *builder,
                                const TYPE&              element,
                                const ELEMENTS_01&       elements_01,
                                const ELEMENTS_02&       elements_02
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
                                const TYPE&              element,
                                const ELEMENTS_01&       elements_01,
                                const ELEMENTS_02&       elements_02,
                                const ELEMENTS_03&       elements_03
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
                                const TYPE&              element,
                                const ELEMENTS_01&       elements_01,
                                const ELEMENTS_02&       elements_02,
                                const ELEMENTS_03&       elements_03,
                                const ELEMENTS_04&       elements_04
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
                                const TYPE&              element,
                                const ELEMENTS_01&       elements_01,
                                const ELEMENTS_02&       elements_02,
                                const ELEMENTS_03&       elements_03,
                                const ELEMENTS_04&       elements_04,
                                const ELEMENTS_05&       elements_05
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
                                const TYPE&              element,
                                const ELEMENTS_01&       elements_01,
                                const ELEMENTS_02&       elements_02,
                                const ELEMENTS_03&       elements_03,
                                const ELEMENTS_04&       elements_04,
                                const ELEMENTS_05&       elements_05,
                                const ELEMENTS_06&       elements_06
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
                                const TYPE&              element,
                                const ELEMENTS_01&       elements_01,
                                const ELEMENTS_02&       elements_02,
                                const ELEMENTS_03&       elements_03,
                                const ELEMENTS_04&       elements_04,
                                const ELEMENTS_05&       elements_05,
                                const ELEMENTS_06&       elements_06,
                                const ELEMENTS_07&       elements_07
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
                                const TYPE&              element,
                                const ELEMENTS_01&       elements_01,
                                const ELEMENTS_02&       elements_02,
                                const ELEMENTS_03&       elements_03,
                                const ELEMENTS_04&       elements_04,
                                const ELEMENTS_05&       elements_05,
                                const ELEMENTS_06&       elements_06,
                                const ELEMENTS_07&       elements_07,
                                const ELEMENTS_08&       elements_08
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
                                const TYPE&              element,
                                const ELEMENTS_01&       elements_01,
                                const ELEMENTS_02&       elements_02,
                                const ELEMENTS_03&       elements_03,
                                const ELEMENTS_04&       elements_04,
                                const ELEMENTS_05&       elements_05,
                                const ELEMENTS_06&       elements_06,
                                const ELEMENTS_07&       elements_07,
                                const ELEMENTS_08&       elements_08,
                                const ELEMENTS_09&       elements_09
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
                                const TYPE&              element,
                                const ELEMENTS_01&       elements_01,
                                const ELEMENTS_02&       elements_02,
                                const ELEMENTS_03&       elements_03,
                                const ELEMENTS_04&       elements_04,
                                const ELEMENTS_05&       elements_05,
                                const ELEMENTS_06&       elements_06,
                                const ELEMENTS_07&       elements_07,
                                const ELEMENTS_08&       elements_08,
                                const ELEMENTS_09&       elements_09,
                                const ELEMENTS_10&       elements_10
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
                                const TYPE&              element,
                                const ELEMENTS_01&       elements_01,
                                const ELEMENTS_02&       elements_02,
                                const ELEMENTS_03&       elements_03,
                                const ELEMENTS_04&       elements_04,
                                const ELEMENTS_05&       elements_05,
                                const ELEMENTS_06&       elements_06,
                                const ELEMENTS_07&       elements_07,
                                const ELEMENTS_08&       elements_08,
                                const ELEMENTS_09&       elements_09,
                                const ELEMENTS_10&       elements_10,
                                const ELEMENTS_11&       elements_11
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
                                const TYPE&              element,
                                const ELEMENTS_01&       elements_01,
                                const ELEMENTS_02&       elements_02,
                                const ELEMENTS_03&       elements_03,
                                const ELEMENTS_04&       elements_04,
                                const ELEMENTS_05&       elements_05,
                                const ELEMENTS_06&       elements_06,
                                const ELEMENTS_07&       elements_07,
                                const ELEMENTS_08&       elements_08,
                                const ELEMENTS_09&       elements_09,
                                const ELEMENTS_10&       elements_10,
                                const ELEMENTS_11&       elements_11,
                                const ELEMENTS_12&       elements_12
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
                                const TYPE&              element,
                                const ELEMENTS_01&       elements_01,
                                const ELEMENTS_02&       elements_02,
                                const ELEMENTS_03&       elements_03,
                                const ELEMENTS_04&       elements_04,
                                const ELEMENTS_05&       elements_05,
                                const ELEMENTS_06&       elements_06,
                                const ELEMENTS_07&       elements_07,
                                const ELEMENTS_08&       elements_08,
                                const ELEMENTS_09&       elements_09,
                                const ELEMENTS_10&       elements_10,
                                const ELEMENTS_11&       elements_11,
                                const ELEMENTS_12&       elements_12,
                                const ELEMENTS_13&       elements_13
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
                                const TYPE&              element,
                                const ELEMENTS_01&       elements_01,
                                const ELEMENTS_02&       elements_02,
                                const ELEMENTS_03&       elements_03,
                                const ELEMENTS_04&       elements_04,
                                const ELEMENTS_05&       elements_05,
                                const ELEMENTS_06&       elements_06,
                                const ELEMENTS_07&       elements_07,
                                const ELEMENTS_08&       elements_08,
                                const ELEMENTS_09&       elements_09,
                                const ELEMENTS_10&       elements_10,
                                const ELEMENTS_11&       elements_11,
                                const ELEMENTS_12&       elements_12,
                                const ELEMENTS_13&       elements_13,
                                const ELEMENTS_14&       elements_14
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
                                const TYPE&              element,
                                const ELEMENTS_01&       elements_01,
                                const ELEMENTS_02&       elements_02,
                                const ELEMENTS_03&       elements_03,
                                const ELEMENTS_04&       elements_04,
                                const ELEMENTS_05&       elements_05,
                                const ELEMENTS_06&       elements_06,
                                const ELEMENTS_07&       elements_07,
                                const ELEMENTS_08&       elements_08,
                                const ELEMENTS_09&       elements_09,
                                const ELEMENTS_10&       elements_10,
                                const ELEMENTS_11&       elements_11,
                                const ELEMENTS_12&       elements_12,
                                const ELEMENTS_13&       elements_13,
                                const ELEMENTS_14&       elements_14,
                                const ELEMENTS_15&       elements_15
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
                                const TYPE&              element,
                                const ELEMENTS_01&       elements_01,
                                const ELEMENTS_02&       elements_02,
                                const ELEMENTS_03&       elements_03,
                                const ELEMENTS_04&       elements_04,
                                const ELEMENTS_05&       elements_05,
                                const ELEMENTS_06&       elements_06,
                                const ELEMENTS_07&       elements_07,
                                const ELEMENTS_08&       elements_08,
                                const ELEMENTS_09&       elements_09,
                                const ELEMENTS_10&       elements_10,
                                const ELEMENTS_11&       elements_11,
                                const ELEMENTS_12&       elements_12,
                                const ELEMENTS_13&       elements_13,
                                const ELEMENTS_14&       elements_14,
                                const ELEMENTS_15&       elements_15,
                                const ELEMENTS_16&       elements_16
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
void DatumMaker::pushBackHelper(bdld::DatumMapBuilder    *builder,
                                const bslstl::StringRef&  key,
                                const TYPE&               value
                                ) const
{
    builder->pushBack(key, (*this)(value));
    pushBackHelper(builder);
}

template <typename TYPE, typename ENTRIES_01,
                         typename ENTRIES_02>
inline
void DatumMaker::pushBackHelper(bdld::DatumMapBuilder    *builder,
                                const bslstl::StringRef&  key,
                                const TYPE&               value,
                                const ENTRIES_01&         entries_01,
                                const ENTRIES_02&         entries_02
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
void DatumMaker::pushBackHelper(bdld::DatumMapBuilder    *builder,
                                const bslstl::StringRef&  key,
                                const TYPE&               value,
                                const ENTRIES_01&         entries_01,
                                const ENTRIES_02&         entries_02,
                                const ENTRIES_03&         entries_03,
                                const ENTRIES_04&         entries_04
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
void DatumMaker::pushBackHelper(bdld::DatumMapBuilder    *builder,
                                const bslstl::StringRef&  key,
                                const TYPE&               value,
                                const ENTRIES_01&         entries_01,
                                const ENTRIES_02&         entries_02,
                                const ENTRIES_03&         entries_03,
                                const ENTRIES_04&         entries_04,
                                const ENTRIES_05&         entries_05,
                                const ENTRIES_06&         entries_06
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
void DatumMaker::pushBackHelper(bdld::DatumMapBuilder    *builder,
                                const bslstl::StringRef&  key,
                                const TYPE&               value,
                                const ENTRIES_01&         entries_01,
                                const ENTRIES_02&         entries_02,
                                const ENTRIES_03&         entries_03,
                                const ENTRIES_04&         entries_04,
                                const ENTRIES_05&         entries_05,
                                const ENTRIES_06&         entries_06,
                                const ENTRIES_07&         entries_07,
                                const ENTRIES_08&         entries_08
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
void DatumMaker::pushBackHelper(bdld::DatumMapBuilder    *builder,
                                const bslstl::StringRef&  key,
                                const TYPE&               value,
                                const ENTRIES_01&         entries_01,
                                const ENTRIES_02&         entries_02,
                                const ENTRIES_03&         entries_03,
                                const ENTRIES_04&         entries_04,
                                const ENTRIES_05&         entries_05,
                                const ENTRIES_06&         entries_06,
                                const ENTRIES_07&         entries_07,
                                const ENTRIES_08&         entries_08,
                                const ENTRIES_09&         entries_09,
                                const ENTRIES_10&         entries_10
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
void DatumMaker::pushBackHelper(bdld::DatumMapBuilder    *builder,
                                const bslstl::StringRef&  key,
                                const TYPE&               value,
                                const ENTRIES_01&         entries_01,
                                const ENTRIES_02&         entries_02,
                                const ENTRIES_03&         entries_03,
                                const ENTRIES_04&         entries_04,
                                const ENTRIES_05&         entries_05,
                                const ENTRIES_06&         entries_06,
                                const ENTRIES_07&         entries_07,
                                const ENTRIES_08&         entries_08,
                                const ENTRIES_09&         entries_09,
                                const ENTRIES_10&         entries_10,
                                const ENTRIES_11&         entries_11,
                                const ENTRIES_12&         entries_12
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
void DatumMaker::pushBackHelper(bdld::DatumMapBuilder    *builder,
                                const bslstl::StringRef&  key,
                                const TYPE&               value,
                                const ENTRIES_01&         entries_01,
                                const ENTRIES_02&         entries_02,
                                const ENTRIES_03&         entries_03,
                                const ENTRIES_04&         entries_04,
                                const ENTRIES_05&         entries_05,
                                const ENTRIES_06&         entries_06,
                                const ENTRIES_07&         entries_07,
                                const ENTRIES_08&         entries_08,
                                const ENTRIES_09&         entries_09,
                                const ENTRIES_10&         entries_10,
                                const ENTRIES_11&         entries_11,
                                const ENTRIES_12&         entries_12,
                                const ENTRIES_13&         entries_13,
                                const ENTRIES_14&         entries_14
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
void DatumMaker::pushBackHelper(bdld::DatumMapBuilder    *builder,
                                const bslstl::StringRef&  key,
                                const TYPE&               value,
                                const ENTRIES_01&         entries_01,
                                const ENTRIES_02&         entries_02,
                                const ENTRIES_03&         entries_03,
                                const ENTRIES_04&         entries_04,
                                const ENTRIES_05&         entries_05,
                                const ENTRIES_06&         entries_06,
                                const ENTRIES_07&         entries_07,
                                const ENTRIES_08&         entries_08,
                                const ENTRIES_09&         entries_09,
                                const ENTRIES_10&         entries_10,
                                const ENTRIES_11&         entries_11,
                                const ENTRIES_12&         entries_12,
                                const ENTRIES_13&         entries_13,
                                const ENTRIES_14&         entries_14,
                                const ENTRIES_15&         entries_15,
                                const ENTRIES_16&         entries_16
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
void DatumMaker::pushBackHelper(bdld::DatumMapBuilder    *builder,
                                const bslstl::StringRef&  key,
                                const TYPE&               value,
                                const ENTRIES_01&         entries_01,
                                const ENTRIES_02&         entries_02,
                                const ENTRIES_03&         entries_03,
                                const ENTRIES_04&         entries_04,
                                const ENTRIES_05&         entries_05,
                                const ENTRIES_06&         entries_06,
                                const ENTRIES_07&         entries_07,
                                const ENTRIES_08&         entries_08,
                                const ENTRIES_09&         entries_09,
                                const ENTRIES_10&         entries_10,
                                const ENTRIES_11&         entries_11,
                                const ENTRIES_12&         entries_12,
                                const ENTRIES_13&         entries_13,
                                const ENTRIES_14&         entries_14,
                                const ENTRIES_15&         entries_15,
                                const ENTRIES_16&         entries_16,
                                const ENTRIES_17&         entries_17,
                                const ENTRIES_18&         entries_18
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
void DatumMaker::pushBackHelper(bdld::DatumMapBuilder    *builder,
                                const bslstl::StringRef&  key,
                                const TYPE&               value,
                                const ENTRIES_01&         entries_01,
                                const ENTRIES_02&         entries_02,
                                const ENTRIES_03&         entries_03,
                                const ENTRIES_04&         entries_04,
                                const ENTRIES_05&         entries_05,
                                const ENTRIES_06&         entries_06,
                                const ENTRIES_07&         entries_07,
                                const ENTRIES_08&         entries_08,
                                const ENTRIES_09&         entries_09,
                                const ENTRIES_10&         entries_10,
                                const ENTRIES_11&         entries_11,
                                const ENTRIES_12&         entries_12,
                                const ENTRIES_13&         entries_13,
                                const ENTRIES_14&         entries_14,
                                const ENTRIES_15&         entries_15,
                                const ENTRIES_16&         entries_16,
                                const ENTRIES_17&         entries_17,
                                const ENTRIES_18&         entries_18,
                                const ENTRIES_19&         entries_19,
                                const ENTRIES_20&         entries_20
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
void DatumMaker::pushBackHelper(bdld::DatumMapBuilder    *builder,
                                const bslstl::StringRef&  key,
                                const TYPE&               value,
                                const ENTRIES_01&         entries_01,
                                const ENTRIES_02&         entries_02,
                                const ENTRIES_03&         entries_03,
                                const ENTRIES_04&         entries_04,
                                const ENTRIES_05&         entries_05,
                                const ENTRIES_06&         entries_06,
                                const ENTRIES_07&         entries_07,
                                const ENTRIES_08&         entries_08,
                                const ENTRIES_09&         entries_09,
                                const ENTRIES_10&         entries_10,
                                const ENTRIES_11&         entries_11,
                                const ENTRIES_12&         entries_12,
                                const ENTRIES_13&         entries_13,
                                const ENTRIES_14&         entries_14,
                                const ENTRIES_15&         entries_15,
                                const ENTRIES_16&         entries_16,
                                const ENTRIES_17&         entries_17,
                                const ENTRIES_18&         entries_18,
                                const ENTRIES_19&         entries_19,
                                const ENTRIES_20&         entries_20,
                                const ENTRIES_21&         entries_21,
                                const ENTRIES_22&         entries_22
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
void DatumMaker::pushBackHelper(bdld::DatumMapBuilder    *builder,
                                const bslstl::StringRef&  key,
                                const TYPE&               value,
                                const ENTRIES_01&         entries_01,
                                const ENTRIES_02&         entries_02,
                                const ENTRIES_03&         entries_03,
                                const ENTRIES_04&         entries_04,
                                const ENTRIES_05&         entries_05,
                                const ENTRIES_06&         entries_06,
                                const ENTRIES_07&         entries_07,
                                const ENTRIES_08&         entries_08,
                                const ENTRIES_09&         entries_09,
                                const ENTRIES_10&         entries_10,
                                const ENTRIES_11&         entries_11,
                                const ENTRIES_12&         entries_12,
                                const ENTRIES_13&         entries_13,
                                const ENTRIES_14&         entries_14,
                                const ENTRIES_15&         entries_15,
                                const ENTRIES_16&         entries_16,
                                const ENTRIES_17&         entries_17,
                                const ENTRIES_18&         entries_18,
                                const ENTRIES_19&         entries_19,
                                const ENTRIES_20&         entries_20,
                                const ENTRIES_21&         entries_21,
                                const ENTRIES_22&         entries_22,
                                const ENTRIES_23&         entries_23,
                                const ENTRIES_24&         entries_24
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
void DatumMaker::pushBackHelper(bdld::DatumMapBuilder    *builder,
                                const bslstl::StringRef&  key,
                                const TYPE&               value,
                                const ENTRIES_01&         entries_01,
                                const ENTRIES_02&         entries_02,
                                const ENTRIES_03&         entries_03,
                                const ENTRIES_04&         entries_04,
                                const ENTRIES_05&         entries_05,
                                const ENTRIES_06&         entries_06,
                                const ENTRIES_07&         entries_07,
                                const ENTRIES_08&         entries_08,
                                const ENTRIES_09&         entries_09,
                                const ENTRIES_10&         entries_10,
                                const ENTRIES_11&         entries_11,
                                const ENTRIES_12&         entries_12,
                                const ENTRIES_13&         entries_13,
                                const ENTRIES_14&         entries_14,
                                const ENTRIES_15&         entries_15,
                                const ENTRIES_16&         entries_16,
                                const ENTRIES_17&         entries_17,
                                const ENTRIES_18&         entries_18,
                                const ENTRIES_19&         entries_19,
                                const ENTRIES_20&         entries_20,
                                const ENTRIES_21&         entries_21,
                                const ENTRIES_22&         entries_22,
                                const ENTRIES_23&         entries_23,
                                const ENTRIES_24&         entries_24,
                                const ENTRIES_25&         entries_25,
                                const ENTRIES_26&         entries_26
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
void DatumMaker::pushBackHelper(bdld::DatumMapBuilder    *builder,
                                const bslstl::StringRef&  key,
                                const TYPE&               value,
                                const ENTRIES_01&         entries_01,
                                const ENTRIES_02&         entries_02,
                                const ENTRIES_03&         entries_03,
                                const ENTRIES_04&         entries_04,
                                const ENTRIES_05&         entries_05,
                                const ENTRIES_06&         entries_06,
                                const ENTRIES_07&         entries_07,
                                const ENTRIES_08&         entries_08,
                                const ENTRIES_09&         entries_09,
                                const ENTRIES_10&         entries_10,
                                const ENTRIES_11&         entries_11,
                                const ENTRIES_12&         entries_12,
                                const ENTRIES_13&         entries_13,
                                const ENTRIES_14&         entries_14,
                                const ENTRIES_15&         entries_15,
                                const ENTRIES_16&         entries_16,
                                const ENTRIES_17&         entries_17,
                                const ENTRIES_18&         entries_18,
                                const ENTRIES_19&         entries_19,
                                const ENTRIES_20&         entries_20,
                                const ENTRIES_21&         entries_21,
                                const ENTRIES_22&         entries_22,
                                const ENTRIES_23&         entries_23,
                                const ENTRIES_24&         entries_24,
                                const ENTRIES_25&         entries_25,
                                const ENTRIES_26&         entries_26,
                                const ENTRIES_27&         entries_27,
                                const ENTRIES_28&         entries_28
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
void DatumMaker::pushBackHelper(bdld::DatumMapBuilder    *builder,
                                const bslstl::StringRef&  key,
                                const TYPE&               value,
                                const ENTRIES_01&         entries_01,
                                const ENTRIES_02&         entries_02,
                                const ENTRIES_03&         entries_03,
                                const ENTRIES_04&         entries_04,
                                const ENTRIES_05&         entries_05,
                                const ENTRIES_06&         entries_06,
                                const ENTRIES_07&         entries_07,
                                const ENTRIES_08&         entries_08,
                                const ENTRIES_09&         entries_09,
                                const ENTRIES_10&         entries_10,
                                const ENTRIES_11&         entries_11,
                                const ENTRIES_12&         entries_12,
                                const ENTRIES_13&         entries_13,
                                const ENTRIES_14&         entries_14,
                                const ENTRIES_15&         entries_15,
                                const ENTRIES_16&         entries_16,
                                const ENTRIES_17&         entries_17,
                                const ENTRIES_18&         entries_18,
                                const ENTRIES_19&         entries_19,
                                const ENTRIES_20&         entries_20,
                                const ENTRIES_21&         entries_21,
                                const ENTRIES_22&         entries_22,
                                const ENTRIES_23&         entries_23,
                                const ENTRIES_24&         entries_24,
                                const ENTRIES_25&         entries_25,
                                const ENTRIES_26&         entries_26,
                                const ENTRIES_27&         entries_27,
                                const ENTRIES_28&         entries_28,
                                const ENTRIES_29&         entries_29,
                                const ENTRIES_30&         entries_30
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
void DatumMaker::pushBackHelper(bdld::DatumMapBuilder    *builder,
                                const bslstl::StringRef&  key,
                                const TYPE&               value,
                                const ENTRIES_01&         entries_01,
                                const ENTRIES_02&         entries_02,
                                const ENTRIES_03&         entries_03,
                                const ENTRIES_04&         entries_04,
                                const ENTRIES_05&         entries_05,
                                const ENTRIES_06&         entries_06,
                                const ENTRIES_07&         entries_07,
                                const ENTRIES_08&         entries_08,
                                const ENTRIES_09&         entries_09,
                                const ENTRIES_10&         entries_10,
                                const ENTRIES_11&         entries_11,
                                const ENTRIES_12&         entries_12,
                                const ENTRIES_13&         entries_13,
                                const ENTRIES_14&         entries_14,
                                const ENTRIES_15&         entries_15,
                                const ENTRIES_16&         entries_16,
                                const ENTRIES_17&         entries_17,
                                const ENTRIES_18&         entries_18,
                                const ENTRIES_19&         entries_19,
                                const ENTRIES_20&         entries_20,
                                const ENTRIES_21&         entries_21,
                                const ENTRIES_22&         entries_22,
                                const ENTRIES_23&         entries_23,
                                const ENTRIES_24&         entries_24,
                                const ENTRIES_25&         entries_25,
                                const ENTRIES_26&         entries_26,
                                const ENTRIES_27&         entries_27,
                                const ENTRIES_28&         entries_28,
                                const ENTRIES_29&         entries_29,
                                const ENTRIES_30&         entries_30,
                                const ENTRIES_31&         entries_31,
                                const ENTRIES_32&         entries_32
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
void DatumMaker::pushBackHelper(bdld::DatumMapOwningKeysBuilder *builder,
                                const bslstl::StringRef&         key,
                                const TYPE&                      value
                                ) const
{
    builder->pushBack(key, (*this)(value));
    pushBackHelper(builder);
}

template <typename TYPE, typename ENTRIES_01,
                         typename ENTRIES_02>
inline
void DatumMaker::pushBackHelper(bdld::DatumMapOwningKeysBuilder *builder,
                                const bslstl::StringRef&         key,
                                const TYPE&                      value,
                                const ENTRIES_01&                entries_01,
                                const ENTRIES_02&                entries_02
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
void DatumMaker::pushBackHelper(bdld::DatumMapOwningKeysBuilder *builder,
                                const bslstl::StringRef&         key,
                                const TYPE&                      value,
                                const ENTRIES_01&                entries_01,
                                const ENTRIES_02&                entries_02,
                                const ENTRIES_03&                entries_03,
                                const ENTRIES_04&                entries_04
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
void DatumMaker::pushBackHelper(bdld::DatumMapOwningKeysBuilder *builder,
                                const bslstl::StringRef&         key,
                                const TYPE&                      value,
                                const ENTRIES_01&                entries_01,
                                const ENTRIES_02&                entries_02,
                                const ENTRIES_03&                entries_03,
                                const ENTRIES_04&                entries_04,
                                const ENTRIES_05&                entries_05,
                                const ENTRIES_06&                entries_06
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
void DatumMaker::pushBackHelper(bdld::DatumMapOwningKeysBuilder *builder,
                                const bslstl::StringRef&         key,
                                const TYPE&                      value,
                                const ENTRIES_01&                entries_01,
                                const ENTRIES_02&                entries_02,
                                const ENTRIES_03&                entries_03,
                                const ENTRIES_04&                entries_04,
                                const ENTRIES_05&                entries_05,
                                const ENTRIES_06&                entries_06,
                                const ENTRIES_07&                entries_07,
                                const ENTRIES_08&                entries_08
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
void DatumMaker::pushBackHelper(bdld::DatumMapOwningKeysBuilder *builder,
                                const bslstl::StringRef&         key,
                                const TYPE&                      value,
                                const ENTRIES_01&                entries_01,
                                const ENTRIES_02&                entries_02,
                                const ENTRIES_03&                entries_03,
                                const ENTRIES_04&                entries_04,
                                const ENTRIES_05&                entries_05,
                                const ENTRIES_06&                entries_06,
                                const ENTRIES_07&                entries_07,
                                const ENTRIES_08&                entries_08,
                                const ENTRIES_09&                entries_09,
                                const ENTRIES_10&                entries_10
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
void DatumMaker::pushBackHelper(bdld::DatumMapOwningKeysBuilder *builder,
                                const bslstl::StringRef&         key,
                                const TYPE&                      value,
                                const ENTRIES_01&                entries_01,
                                const ENTRIES_02&                entries_02,
                                const ENTRIES_03&                entries_03,
                                const ENTRIES_04&                entries_04,
                                const ENTRIES_05&                entries_05,
                                const ENTRIES_06&                entries_06,
                                const ENTRIES_07&                entries_07,
                                const ENTRIES_08&                entries_08,
                                const ENTRIES_09&                entries_09,
                                const ENTRIES_10&                entries_10,
                                const ENTRIES_11&                entries_11,
                                const ENTRIES_12&                entries_12
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
void DatumMaker::pushBackHelper(bdld::DatumMapOwningKeysBuilder *builder,
                                const bslstl::StringRef&         key,
                                const TYPE&                      value,
                                const ENTRIES_01&                entries_01,
                                const ENTRIES_02&                entries_02,
                                const ENTRIES_03&                entries_03,
                                const ENTRIES_04&                entries_04,
                                const ENTRIES_05&                entries_05,
                                const ENTRIES_06&                entries_06,
                                const ENTRIES_07&                entries_07,
                                const ENTRIES_08&                entries_08,
                                const ENTRIES_09&                entries_09,
                                const ENTRIES_10&                entries_10,
                                const ENTRIES_11&                entries_11,
                                const ENTRIES_12&                entries_12,
                                const ENTRIES_13&                entries_13,
                                const ENTRIES_14&                entries_14
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
void DatumMaker::pushBackHelper(bdld::DatumMapOwningKeysBuilder *builder,
                                const bslstl::StringRef&         key,
                                const TYPE&                      value,
                                const ENTRIES_01&                entries_01,
                                const ENTRIES_02&                entries_02,
                                const ENTRIES_03&                entries_03,
                                const ENTRIES_04&                entries_04,
                                const ENTRIES_05&                entries_05,
                                const ENTRIES_06&                entries_06,
                                const ENTRIES_07&                entries_07,
                                const ENTRIES_08&                entries_08,
                                const ENTRIES_09&                entries_09,
                                const ENTRIES_10&                entries_10,
                                const ENTRIES_11&                entries_11,
                                const ENTRIES_12&                entries_12,
                                const ENTRIES_13&                entries_13,
                                const ENTRIES_14&                entries_14,
                                const ENTRIES_15&                entries_15,
                                const ENTRIES_16&                entries_16
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
void DatumMaker::pushBackHelper(bdld::DatumMapOwningKeysBuilder *builder,
                                const bslstl::StringRef&         key,
                                const TYPE&                      value,
                                const ENTRIES_01&                entries_01,
                                const ENTRIES_02&                entries_02,
                                const ENTRIES_03&                entries_03,
                                const ENTRIES_04&                entries_04,
                                const ENTRIES_05&                entries_05,
                                const ENTRIES_06&                entries_06,
                                const ENTRIES_07&                entries_07,
                                const ENTRIES_08&                entries_08,
                                const ENTRIES_09&                entries_09,
                                const ENTRIES_10&                entries_10,
                                const ENTRIES_11&                entries_11,
                                const ENTRIES_12&                entries_12,
                                const ENTRIES_13&                entries_13,
                                const ENTRIES_14&                entries_14,
                                const ENTRIES_15&                entries_15,
                                const ENTRIES_16&                entries_16,
                                const ENTRIES_17&                entries_17,
                                const ENTRIES_18&                entries_18
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
void DatumMaker::pushBackHelper(bdld::DatumMapOwningKeysBuilder *builder,
                                const bslstl::StringRef&         key,
                                const TYPE&                      value,
                                const ENTRIES_01&                entries_01,
                                const ENTRIES_02&                entries_02,
                                const ENTRIES_03&                entries_03,
                                const ENTRIES_04&                entries_04,
                                const ENTRIES_05&                entries_05,
                                const ENTRIES_06&                entries_06,
                                const ENTRIES_07&                entries_07,
                                const ENTRIES_08&                entries_08,
                                const ENTRIES_09&                entries_09,
                                const ENTRIES_10&                entries_10,
                                const ENTRIES_11&                entries_11,
                                const ENTRIES_12&                entries_12,
                                const ENTRIES_13&                entries_13,
                                const ENTRIES_14&                entries_14,
                                const ENTRIES_15&                entries_15,
                                const ENTRIES_16&                entries_16,
                                const ENTRIES_17&                entries_17,
                                const ENTRIES_18&                entries_18,
                                const ENTRIES_19&                entries_19,
                                const ENTRIES_20&                entries_20
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
void DatumMaker::pushBackHelper(bdld::DatumMapOwningKeysBuilder *builder,
                                const bslstl::StringRef&         key,
                                const TYPE&                      value,
                                const ENTRIES_01&                entries_01,
                                const ENTRIES_02&                entries_02,
                                const ENTRIES_03&                entries_03,
                                const ENTRIES_04&                entries_04,
                                const ENTRIES_05&                entries_05,
                                const ENTRIES_06&                entries_06,
                                const ENTRIES_07&                entries_07,
                                const ENTRIES_08&                entries_08,
                                const ENTRIES_09&                entries_09,
                                const ENTRIES_10&                entries_10,
                                const ENTRIES_11&                entries_11,
                                const ENTRIES_12&                entries_12,
                                const ENTRIES_13&                entries_13,
                                const ENTRIES_14&                entries_14,
                                const ENTRIES_15&                entries_15,
                                const ENTRIES_16&                entries_16,
                                const ENTRIES_17&                entries_17,
                                const ENTRIES_18&                entries_18,
                                const ENTRIES_19&                entries_19,
                                const ENTRIES_20&                entries_20,
                                const ENTRIES_21&                entries_21,
                                const ENTRIES_22&                entries_22
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
void DatumMaker::pushBackHelper(bdld::DatumMapOwningKeysBuilder *builder,
                                const bslstl::StringRef&         key,
                                const TYPE&                      value,
                                const ENTRIES_01&                entries_01,
                                const ENTRIES_02&                entries_02,
                                const ENTRIES_03&                entries_03,
                                const ENTRIES_04&                entries_04,
                                const ENTRIES_05&                entries_05,
                                const ENTRIES_06&                entries_06,
                                const ENTRIES_07&                entries_07,
                                const ENTRIES_08&                entries_08,
                                const ENTRIES_09&                entries_09,
                                const ENTRIES_10&                entries_10,
                                const ENTRIES_11&                entries_11,
                                const ENTRIES_12&                entries_12,
                                const ENTRIES_13&                entries_13,
                                const ENTRIES_14&                entries_14,
                                const ENTRIES_15&                entries_15,
                                const ENTRIES_16&                entries_16,
                                const ENTRIES_17&                entries_17,
                                const ENTRIES_18&                entries_18,
                                const ENTRIES_19&                entries_19,
                                const ENTRIES_20&                entries_20,
                                const ENTRIES_21&                entries_21,
                                const ENTRIES_22&                entries_22,
                                const ENTRIES_23&                entries_23,
                                const ENTRIES_24&                entries_24
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
void DatumMaker::pushBackHelper(bdld::DatumMapOwningKeysBuilder *builder,
                                const bslstl::StringRef&         key,
                                const TYPE&                      value,
                                const ENTRIES_01&                entries_01,
                                const ENTRIES_02&                entries_02,
                                const ENTRIES_03&                entries_03,
                                const ENTRIES_04&                entries_04,
                                const ENTRIES_05&                entries_05,
                                const ENTRIES_06&                entries_06,
                                const ENTRIES_07&                entries_07,
                                const ENTRIES_08&                entries_08,
                                const ENTRIES_09&                entries_09,
                                const ENTRIES_10&                entries_10,
                                const ENTRIES_11&                entries_11,
                                const ENTRIES_12&                entries_12,
                                const ENTRIES_13&                entries_13,
                                const ENTRIES_14&                entries_14,
                                const ENTRIES_15&                entries_15,
                                const ENTRIES_16&                entries_16,
                                const ENTRIES_17&                entries_17,
                                const ENTRIES_18&                entries_18,
                                const ENTRIES_19&                entries_19,
                                const ENTRIES_20&                entries_20,
                                const ENTRIES_21&                entries_21,
                                const ENTRIES_22&                entries_22,
                                const ENTRIES_23&                entries_23,
                                const ENTRIES_24&                entries_24,
                                const ENTRIES_25&                entries_25,
                                const ENTRIES_26&                entries_26
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
void DatumMaker::pushBackHelper(bdld::DatumMapOwningKeysBuilder *builder,
                                const bslstl::StringRef&         key,
                                const TYPE&                      value,
                                const ENTRIES_01&                entries_01,
                                const ENTRIES_02&                entries_02,
                                const ENTRIES_03&                entries_03,
                                const ENTRIES_04&                entries_04,
                                const ENTRIES_05&                entries_05,
                                const ENTRIES_06&                entries_06,
                                const ENTRIES_07&                entries_07,
                                const ENTRIES_08&                entries_08,
                                const ENTRIES_09&                entries_09,
                                const ENTRIES_10&                entries_10,
                                const ENTRIES_11&                entries_11,
                                const ENTRIES_12&                entries_12,
                                const ENTRIES_13&                entries_13,
                                const ENTRIES_14&                entries_14,
                                const ENTRIES_15&                entries_15,
                                const ENTRIES_16&                entries_16,
                                const ENTRIES_17&                entries_17,
                                const ENTRIES_18&                entries_18,
                                const ENTRIES_19&                entries_19,
                                const ENTRIES_20&                entries_20,
                                const ENTRIES_21&                entries_21,
                                const ENTRIES_22&                entries_22,
                                const ENTRIES_23&                entries_23,
                                const ENTRIES_24&                entries_24,
                                const ENTRIES_25&                entries_25,
                                const ENTRIES_26&                entries_26,
                                const ENTRIES_27&                entries_27,
                                const ENTRIES_28&                entries_28
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
void DatumMaker::pushBackHelper(bdld::DatumMapOwningKeysBuilder *builder,
                                const bslstl::StringRef&         key,
                                const TYPE&                      value,
                                const ENTRIES_01&                entries_01,
                                const ENTRIES_02&                entries_02,
                                const ENTRIES_03&                entries_03,
                                const ENTRIES_04&                entries_04,
                                const ENTRIES_05&                entries_05,
                                const ENTRIES_06&                entries_06,
                                const ENTRIES_07&                entries_07,
                                const ENTRIES_08&                entries_08,
                                const ENTRIES_09&                entries_09,
                                const ENTRIES_10&                entries_10,
                                const ENTRIES_11&                entries_11,
                                const ENTRIES_12&                entries_12,
                                const ENTRIES_13&                entries_13,
                                const ENTRIES_14&                entries_14,
                                const ENTRIES_15&                entries_15,
                                const ENTRIES_16&                entries_16,
                                const ENTRIES_17&                entries_17,
                                const ENTRIES_18&                entries_18,
                                const ENTRIES_19&                entries_19,
                                const ENTRIES_20&                entries_20,
                                const ENTRIES_21&                entries_21,
                                const ENTRIES_22&                entries_22,
                                const ENTRIES_23&                entries_23,
                                const ENTRIES_24&                entries_24,
                                const ENTRIES_25&                entries_25,
                                const ENTRIES_26&                entries_26,
                                const ENTRIES_27&                entries_27,
                                const ENTRIES_28&                entries_28,
                                const ENTRIES_29&                entries_29,
                                const ENTRIES_30&                entries_30
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
void DatumMaker::pushBackHelper(bdld::DatumMapOwningKeysBuilder *builder,
                                const bslstl::StringRef&         key,
                                const TYPE&                      value,
                                const ENTRIES_01&                entries_01,
                                const ENTRIES_02&                entries_02,
                                const ENTRIES_03&                entries_03,
                                const ENTRIES_04&                entries_04,
                                const ENTRIES_05&                entries_05,
                                const ENTRIES_06&                entries_06,
                                const ENTRIES_07&                entries_07,
                                const ENTRIES_08&                entries_08,
                                const ENTRIES_09&                entries_09,
                                const ENTRIES_10&                entries_10,
                                const ENTRIES_11&                entries_11,
                                const ENTRIES_12&                entries_12,
                                const ENTRIES_13&                entries_13,
                                const ENTRIES_14&                entries_14,
                                const ENTRIES_15&                entries_15,
                                const ENTRIES_16&                entries_16,
                                const ENTRIES_17&                entries_17,
                                const ENTRIES_18&                entries_18,
                                const ENTRIES_19&                entries_19,
                                const ENTRIES_20&                entries_20,
                                const ENTRIES_21&                entries_21,
                                const ENTRIES_22&                entries_22,
                                const ENTRIES_23&                entries_23,
                                const ENTRIES_24&                entries_24,
                                const ENTRIES_25&                entries_25,
                                const ENTRIES_26&                entries_26,
                                const ENTRIES_27&                entries_27,
                                const ENTRIES_28&                entries_28,
                                const ENTRIES_29&                entries_29,
                                const ENTRIES_30&                entries_30,
                                const ENTRIES_31&                entries_31,
                                const ENTRIES_32&                entries_32
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
void DatumMaker::pushBackHelper(bdld::DatumIntMapBuilder *builder,
                                int                       key,
                                const TYPE&               value
                                ) const
{
    builder->pushBack(key, (*this)(value));
    pushBackHelper(builder);
}

template <typename TYPE, typename ENTRIES_01>
inline
void DatumMaker::pushBackHelper(bdld::DatumIntMapBuilder *builder,
                                int                       key,
                                const TYPE&               value,
                                int                       key_01,
                                const ENTRIES_01&         entries_01
                                ) const
{
    builder->pushBack(key, (*this)(value));
    pushBackHelper(builder, key_01, entries_01);
}

template <typename TYPE, typename ENTRIES_01,
                         typename ENTRIES_02>
inline
void DatumMaker::pushBackHelper(bdld::DatumIntMapBuilder *builder,
                                int                       key,
                                const TYPE&               value,
                                int                       key_01,
                                const ENTRIES_01&         entries_01,
                                int                       key_02,
                                const ENTRIES_02&         entries_02
                                ) const
{
    builder->pushBack(key, (*this)(value));
    pushBackHelper(builder, key_01, entries_01,
                            key_02, entries_02);
}

template <typename TYPE, typename ENTRIES_01,
                         typename ENTRIES_02,
                         typename ENTRIES_03>
inline
void DatumMaker::pushBackHelper(bdld::DatumIntMapBuilder *builder,
                                int                       key,
                                const TYPE&               value,
                                int                       key_01,
                                const ENTRIES_01&         entries_01,
                                int                       key_02,
                                const ENTRIES_02&         entries_02,
                                int                       key_03,
                                const ENTRIES_03&         entries_03
                                ) const
{
    builder->pushBack(key, (*this)(value));
    pushBackHelper(builder, key_01, entries_01,
                            key_02, entries_02,
                            key_03, entries_03);
}

template <typename TYPE, typename ENTRIES_01,
                         typename ENTRIES_02,
                         typename ENTRIES_03,
                         typename ENTRIES_04>
inline
void DatumMaker::pushBackHelper(bdld::DatumIntMapBuilder *builder,
                                int                       key,
                                const TYPE&               value,
                                int                       key_01,
                                const ENTRIES_01&         entries_01,
                                int                       key_02,
                                const ENTRIES_02&         entries_02,
                                int                       key_03,
                                const ENTRIES_03&         entries_03,
                                int                       key_04,
                                const ENTRIES_04&         entries_04
                                ) const
{
    builder->pushBack(key, (*this)(value));
    pushBackHelper(builder, key_01, entries_01,
                            key_02, entries_02,
                            key_03, entries_03,
                            key_04, entries_04);
}

template <typename TYPE, typename ENTRIES_01,
                         typename ENTRIES_02,
                         typename ENTRIES_03,
                         typename ENTRIES_04,
                         typename ENTRIES_05>
inline
void DatumMaker::pushBackHelper(bdld::DatumIntMapBuilder *builder,
                                int                       key,
                                const TYPE&               value,
                                int                       key_01,
                                const ENTRIES_01&         entries_01,
                                int                       key_02,
                                const ENTRIES_02&         entries_02,
                                int                       key_03,
                                const ENTRIES_03&         entries_03,
                                int                       key_04,
                                const ENTRIES_04&         entries_04,
                                int                       key_05,
                                const ENTRIES_05&         entries_05
                                ) const
{
    builder->pushBack(key, (*this)(value));
    pushBackHelper(builder, key_01, entries_01,
                            key_02, entries_02,
                            key_03, entries_03,
                            key_04, entries_04,
                            key_05, entries_05);
}

template <typename TYPE, typename ENTRIES_01,
                         typename ENTRIES_02,
                         typename ENTRIES_03,
                         typename ENTRIES_04,
                         typename ENTRIES_05,
                         typename ENTRIES_06>
inline
void DatumMaker::pushBackHelper(bdld::DatumIntMapBuilder *builder,
                                int                       key,
                                const TYPE&               value,
                                int                       key_01,
                                const ENTRIES_01&         entries_01,
                                int                       key_02,
                                const ENTRIES_02&         entries_02,
                                int                       key_03,
                                const ENTRIES_03&         entries_03,
                                int                       key_04,
                                const ENTRIES_04&         entries_04,
                                int                       key_05,
                                const ENTRIES_05&         entries_05,
                                int                       key_06,
                                const ENTRIES_06&         entries_06
                                ) const
{
    builder->pushBack(key, (*this)(value));
    pushBackHelper(builder, key_01, entries_01,
                            key_02, entries_02,
                            key_03, entries_03,
                            key_04, entries_04,
                            key_05, entries_05,
                            key_06, entries_06);
}

template <typename TYPE, typename ENTRIES_01,
                         typename ENTRIES_02,
                         typename ENTRIES_03,
                         typename ENTRIES_04,
                         typename ENTRIES_05,
                         typename ENTRIES_06,
                         typename ENTRIES_07>
inline
void DatumMaker::pushBackHelper(bdld::DatumIntMapBuilder *builder,
                                int                       key,
                                const TYPE&               value,
                                int                       key_01,
                                const ENTRIES_01&         entries_01,
                                int                       key_02,
                                const ENTRIES_02&         entries_02,
                                int                       key_03,
                                const ENTRIES_03&         entries_03,
                                int                       key_04,
                                const ENTRIES_04&         entries_04,
                                int                       key_05,
                                const ENTRIES_05&         entries_05,
                                int                       key_06,
                                const ENTRIES_06&         entries_06,
                                int                       key_07,
                                const ENTRIES_07&         entries_07
                                ) const
{
    builder->pushBack(key, (*this)(value));
    pushBackHelper(builder, key_01, entries_01,
                            key_02, entries_02,
                            key_03, entries_03,
                            key_04, entries_04,
                            key_05, entries_05,
                            key_06, entries_06,
                            key_07, entries_07);
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
void DatumMaker::pushBackHelper(bdld::DatumIntMapBuilder *builder,
                                int                       key,
                                const TYPE&               value,
                                int                       key_01,
                                const ENTRIES_01&         entries_01,
                                int                       key_02,
                                const ENTRIES_02&         entries_02,
                                int                       key_03,
                                const ENTRIES_03&         entries_03,
                                int                       key_04,
                                const ENTRIES_04&         entries_04,
                                int                       key_05,
                                const ENTRIES_05&         entries_05,
                                int                       key_06,
                                const ENTRIES_06&         entries_06,
                                int                       key_07,
                                const ENTRIES_07&         entries_07,
                                int                       key_08,
                                const ENTRIES_08&         entries_08
                                ) const
{
    builder->pushBack(key, (*this)(value));
    pushBackHelper(builder, key_01, entries_01,
                            key_02, entries_02,
                            key_03, entries_03,
                            key_04, entries_04,
                            key_05, entries_05,
                            key_06, entries_06,
                            key_07, entries_07,
                            key_08, entries_08);
}

template <typename TYPE, typename ENTRIES_01,
                         typename ENTRIES_02,
                         typename ENTRIES_03,
                         typename ENTRIES_04,
                         typename ENTRIES_05,
                         typename ENTRIES_06,
                         typename ENTRIES_07,
                         typename ENTRIES_08,
                         typename ENTRIES_09>
inline
void DatumMaker::pushBackHelper(bdld::DatumIntMapBuilder *builder,
                                int                       key,
                                const TYPE&               value,
                                int                       key_01,
                                const ENTRIES_01&         entries_01,
                                int                       key_02,
                                const ENTRIES_02&         entries_02,
                                int                       key_03,
                                const ENTRIES_03&         entries_03,
                                int                       key_04,
                                const ENTRIES_04&         entries_04,
                                int                       key_05,
                                const ENTRIES_05&         entries_05,
                                int                       key_06,
                                const ENTRIES_06&         entries_06,
                                int                       key_07,
                                const ENTRIES_07&         entries_07,
                                int                       key_08,
                                const ENTRIES_08&         entries_08,
                                int                       key_09,
                                const ENTRIES_09&         entries_09
                                ) const
{
    builder->pushBack(key, (*this)(value));
    pushBackHelper(builder, key_01, entries_01,
                            key_02, entries_02,
                            key_03, entries_03,
                            key_04, entries_04,
                            key_05, entries_05,
                            key_06, entries_06,
                            key_07, entries_07,
                            key_08, entries_08,
                            key_09, entries_09);
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
void DatumMaker::pushBackHelper(bdld::DatumIntMapBuilder *builder,
                                int                       key,
                                const TYPE&               value,
                                int                       key_01,
                                const ENTRIES_01&         entries_01,
                                int                       key_02,
                                const ENTRIES_02&         entries_02,
                                int                       key_03,
                                const ENTRIES_03&         entries_03,
                                int                       key_04,
                                const ENTRIES_04&         entries_04,
                                int                       key_05,
                                const ENTRIES_05&         entries_05,
                                int                       key_06,
                                const ENTRIES_06&         entries_06,
                                int                       key_07,
                                const ENTRIES_07&         entries_07,
                                int                       key_08,
                                const ENTRIES_08&         entries_08,
                                int                       key_09,
                                const ENTRIES_09&         entries_09,
                                int                       key_10,
                                const ENTRIES_10&         entries_10
                                ) const
{
    builder->pushBack(key, (*this)(value));
    pushBackHelper(builder, key_01, entries_01,
                            key_02, entries_02,
                            key_03, entries_03,
                            key_04, entries_04,
                            key_05, entries_05,
                            key_06, entries_06,
                            key_07, entries_07,
                            key_08, entries_08,
                            key_09, entries_09,
                            key_10, entries_10);
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
                         typename ENTRIES_11>
inline
void DatumMaker::pushBackHelper(bdld::DatumIntMapBuilder *builder,
                                int                       key,
                                const TYPE&               value,
                                int                       key_01,
                                const ENTRIES_01&         entries_01,
                                int                       key_02,
                                const ENTRIES_02&         entries_02,
                                int                       key_03,
                                const ENTRIES_03&         entries_03,
                                int                       key_04,
                                const ENTRIES_04&         entries_04,
                                int                       key_05,
                                const ENTRIES_05&         entries_05,
                                int                       key_06,
                                const ENTRIES_06&         entries_06,
                                int                       key_07,
                                const ENTRIES_07&         entries_07,
                                int                       key_08,
                                const ENTRIES_08&         entries_08,
                                int                       key_09,
                                const ENTRIES_09&         entries_09,
                                int                       key_10,
                                const ENTRIES_10&         entries_10,
                                int                       key_11,
                                const ENTRIES_11&         entries_11
                                ) const
{
    builder->pushBack(key, (*this)(value));
    pushBackHelper(builder, key_01, entries_01,
                            key_02, entries_02,
                            key_03, entries_03,
                            key_04, entries_04,
                            key_05, entries_05,
                            key_06, entries_06,
                            key_07, entries_07,
                            key_08, entries_08,
                            key_09, entries_09,
                            key_10, entries_10,
                            key_11, entries_11);
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
void DatumMaker::pushBackHelper(bdld::DatumIntMapBuilder *builder,
                                int                       key,
                                const TYPE&               value,
                                int                       key_01,
                                const ENTRIES_01&         entries_01,
                                int                       key_02,
                                const ENTRIES_02&         entries_02,
                                int                       key_03,
                                const ENTRIES_03&         entries_03,
                                int                       key_04,
                                const ENTRIES_04&         entries_04,
                                int                       key_05,
                                const ENTRIES_05&         entries_05,
                                int                       key_06,
                                const ENTRIES_06&         entries_06,
                                int                       key_07,
                                const ENTRIES_07&         entries_07,
                                int                       key_08,
                                const ENTRIES_08&         entries_08,
                                int                       key_09,
                                const ENTRIES_09&         entries_09,
                                int                       key_10,
                                const ENTRIES_10&         entries_10,
                                int                       key_11,
                                const ENTRIES_11&         entries_11,
                                int                       key_12,
                                const ENTRIES_12&         entries_12
                                ) const
{
    builder->pushBack(key, (*this)(value));
    pushBackHelper(builder, key_01, entries_01,
                            key_02, entries_02,
                            key_03, entries_03,
                            key_04, entries_04,
                            key_05, entries_05,
                            key_06, entries_06,
                            key_07, entries_07,
                            key_08, entries_08,
                            key_09, entries_09,
                            key_10, entries_10,
                            key_11, entries_11,
                            key_12, entries_12);
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
                         typename ENTRIES_13>
inline
void DatumMaker::pushBackHelper(bdld::DatumIntMapBuilder *builder,
                                int                       key,
                                const TYPE&               value,
                                int                       key_01,
                                const ENTRIES_01&         entries_01,
                                int                       key_02,
                                const ENTRIES_02&         entries_02,
                                int                       key_03,
                                const ENTRIES_03&         entries_03,
                                int                       key_04,
                                const ENTRIES_04&         entries_04,
                                int                       key_05,
                                const ENTRIES_05&         entries_05,
                                int                       key_06,
                                const ENTRIES_06&         entries_06,
                                int                       key_07,
                                const ENTRIES_07&         entries_07,
                                int                       key_08,
                                const ENTRIES_08&         entries_08,
                                int                       key_09,
                                const ENTRIES_09&         entries_09,
                                int                       key_10,
                                const ENTRIES_10&         entries_10,
                                int                       key_11,
                                const ENTRIES_11&         entries_11,
                                int                       key_12,
                                const ENTRIES_12&         entries_12,
                                int                       key_13,
                                const ENTRIES_13&         entries_13
                                ) const
{
    builder->pushBack(key, (*this)(value));
    pushBackHelper(builder, key_01, entries_01,
                            key_02, entries_02,
                            key_03, entries_03,
                            key_04, entries_04,
                            key_05, entries_05,
                            key_06, entries_06,
                            key_07, entries_07,
                            key_08, entries_08,
                            key_09, entries_09,
                            key_10, entries_10,
                            key_11, entries_11,
                            key_12, entries_12,
                            key_13, entries_13);
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
void DatumMaker::pushBackHelper(bdld::DatumIntMapBuilder *builder,
                                int                       key,
                                const TYPE&               value,
                                int                       key_01,
                                const ENTRIES_01&         entries_01,
                                int                       key_02,
                                const ENTRIES_02&         entries_02,
                                int                       key_03,
                                const ENTRIES_03&         entries_03,
                                int                       key_04,
                                const ENTRIES_04&         entries_04,
                                int                       key_05,
                                const ENTRIES_05&         entries_05,
                                int                       key_06,
                                const ENTRIES_06&         entries_06,
                                int                       key_07,
                                const ENTRIES_07&         entries_07,
                                int                       key_08,
                                const ENTRIES_08&         entries_08,
                                int                       key_09,
                                const ENTRIES_09&         entries_09,
                                int                       key_10,
                                const ENTRIES_10&         entries_10,
                                int                       key_11,
                                const ENTRIES_11&         entries_11,
                                int                       key_12,
                                const ENTRIES_12&         entries_12,
                                int                       key_13,
                                const ENTRIES_13&         entries_13,
                                int                       key_14,
                                const ENTRIES_14&         entries_14
                                ) const
{
    builder->pushBack(key, (*this)(value));
    pushBackHelper(builder, key_01, entries_01,
                            key_02, entries_02,
                            key_03, entries_03,
                            key_04, entries_04,
                            key_05, entries_05,
                            key_06, entries_06,
                            key_07, entries_07,
                            key_08, entries_08,
                            key_09, entries_09,
                            key_10, entries_10,
                            key_11, entries_11,
                            key_12, entries_12,
                            key_13, entries_13,
                            key_14, entries_14);
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
                         typename ENTRIES_15>
inline
void DatumMaker::pushBackHelper(bdld::DatumIntMapBuilder *builder,
                                int                       key,
                                const TYPE&               value,
                                int                       key_01,
                                const ENTRIES_01&         entries_01,
                                int                       key_02,
                                const ENTRIES_02&         entries_02,
                                int                       key_03,
                                const ENTRIES_03&         entries_03,
                                int                       key_04,
                                const ENTRIES_04&         entries_04,
                                int                       key_05,
                                const ENTRIES_05&         entries_05,
                                int                       key_06,
                                const ENTRIES_06&         entries_06,
                                int                       key_07,
                                const ENTRIES_07&         entries_07,
                                int                       key_08,
                                const ENTRIES_08&         entries_08,
                                int                       key_09,
                                const ENTRIES_09&         entries_09,
                                int                       key_10,
                                const ENTRIES_10&         entries_10,
                                int                       key_11,
                                const ENTRIES_11&         entries_11,
                                int                       key_12,
                                const ENTRIES_12&         entries_12,
                                int                       key_13,
                                const ENTRIES_13&         entries_13,
                                int                       key_14,
                                const ENTRIES_14&         entries_14,
                                int                       key_15,
                                const ENTRIES_15&         entries_15
                                ) const
{
    builder->pushBack(key, (*this)(value));
    pushBackHelper(builder, key_01, entries_01,
                            key_02, entries_02,
                            key_03, entries_03,
                            key_04, entries_04,
                            key_05, entries_05,
                            key_06, entries_06,
                            key_07, entries_07,
                            key_08, entries_08,
                            key_09, entries_09,
                            key_10, entries_10,
                            key_11, entries_11,
                            key_12, entries_12,
                            key_13, entries_13,
                            key_14, entries_14,
                            key_15, entries_15);
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
void DatumMaker::pushBackHelper(bdld::DatumIntMapBuilder *builder,
                                int                       key,
                                const TYPE&               value,
                                int                       key_01,
                                const ENTRIES_01&         entries_01,
                                int                       key_02,
                                const ENTRIES_02&         entries_02,
                                int                       key_03,
                                const ENTRIES_03&         entries_03,
                                int                       key_04,
                                const ENTRIES_04&         entries_04,
                                int                       key_05,
                                const ENTRIES_05&         entries_05,
                                int                       key_06,
                                const ENTRIES_06&         entries_06,
                                int                       key_07,
                                const ENTRIES_07&         entries_07,
                                int                       key_08,
                                const ENTRIES_08&         entries_08,
                                int                       key_09,
                                const ENTRIES_09&         entries_09,
                                int                       key_10,
                                const ENTRIES_10&         entries_10,
                                int                       key_11,
                                const ENTRIES_11&         entries_11,
                                int                       key_12,
                                const ENTRIES_12&         entries_12,
                                int                       key_13,
                                const ENTRIES_13&         entries_13,
                                int                       key_14,
                                const ENTRIES_14&         entries_14,
                                int                       key_15,
                                const ENTRIES_15&         entries_15,
                                int                       key_16,
                                const ENTRIES_16&         entries_16
                                ) const
{
    builder->pushBack(key, (*this)(value));
    pushBackHelper(builder, key_01, entries_01,
                            key_02, entries_02,
                            key_03, entries_03,
                            key_04, entries_04,
                            key_05, entries_05,
                            key_06, entries_06,
                            key_07, entries_07,
                            key_08, entries_08,
                            key_09, entries_09,
                            key_10, entries_10,
                            key_11, entries_11,
                            key_12, entries_12,
                            key_13, entries_13,
                            key_14, entries_14,
                            key_15, entries_15,
                            key_16, entries_16);
}

#endif

// CREATORS
inline
DatumMaker::DatumMaker(bslma::Allocator *basicAllocator)
: d_allocator_p(basicAllocator)
{
    BSLS_REVIEW(basicAllocator);
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
bdld::Datum DatumMaker::operator()(
                                const bdld::DatumMutableIntMapRef& value) const
{
    return bdld::Datum::adoptIntMap(value);
}

template <class TYPE>
inline
bdld::Datum DatumMaker::operator()(
                                  const bdlb::NullableValue<TYPE>& value) const
{
    return value.isNull() ? (*this)() : (*this)(value.value());
}

#if !BSLS_COMPILERFEATURES_SIMULATE_VARIADIC_TEMPLATES
template <typename... ELEMENTS>
inline
bdld::Datum DatumMaker::a(const ELEMENTS&... elements) const
{
    const int numElements = sizeof...(ELEMENTS);
    bdld::DatumArrayBuilder builder(numElements, d_allocator_p);
    pushBackHelper(&builder, elements...);
    return builder.commit();
}
#else
inline
bdld::Datum DatumMaker::a() const
{
    const int numElements =  0u;
    bdld::DatumArrayBuilder builder(numElements, d_allocator_p);
    pushBackHelper(&builder);
    return builder.commit();
}

template <typename ELEMENTS_01>
inline
bdld::Datum DatumMaker::a(const ELEMENTS_01& elements_01
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
bdld::Datum DatumMaker::a(const ELEMENTS_01& elements_01,
                          const ELEMENTS_02& elements_02
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
bdld::Datum DatumMaker::a(const ELEMENTS_01& elements_01,
                          const ELEMENTS_02& elements_02,
                          const ELEMENTS_03& elements_03
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
bdld::Datum DatumMaker::a(const ELEMENTS_01& elements_01,
                          const ELEMENTS_02& elements_02,
                          const ELEMENTS_03& elements_03,
                          const ELEMENTS_04& elements_04
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
bdld::Datum DatumMaker::a(const ELEMENTS_01& elements_01,
                          const ELEMENTS_02& elements_02,
                          const ELEMENTS_03& elements_03,
                          const ELEMENTS_04& elements_04,
                          const ELEMENTS_05& elements_05
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
bdld::Datum DatumMaker::a(const ELEMENTS_01& elements_01,
                          const ELEMENTS_02& elements_02,
                          const ELEMENTS_03& elements_03,
                          const ELEMENTS_04& elements_04,
                          const ELEMENTS_05& elements_05,
                          const ELEMENTS_06& elements_06
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
bdld::Datum DatumMaker::a(const ELEMENTS_01& elements_01,
                          const ELEMENTS_02& elements_02,
                          const ELEMENTS_03& elements_03,
                          const ELEMENTS_04& elements_04,
                          const ELEMENTS_05& elements_05,
                          const ELEMENTS_06& elements_06,
                          const ELEMENTS_07& elements_07
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
bdld::Datum DatumMaker::a(const ELEMENTS_01& elements_01,
                          const ELEMENTS_02& elements_02,
                          const ELEMENTS_03& elements_03,
                          const ELEMENTS_04& elements_04,
                          const ELEMENTS_05& elements_05,
                          const ELEMENTS_06& elements_06,
                          const ELEMENTS_07& elements_07,
                          const ELEMENTS_08& elements_08
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
bdld::Datum DatumMaker::a(const ELEMENTS_01& elements_01,
                          const ELEMENTS_02& elements_02,
                          const ELEMENTS_03& elements_03,
                          const ELEMENTS_04& elements_04,
                          const ELEMENTS_05& elements_05,
                          const ELEMENTS_06& elements_06,
                          const ELEMENTS_07& elements_07,
                          const ELEMENTS_08& elements_08,
                          const ELEMENTS_09& elements_09
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
bdld::Datum DatumMaker::a(const ELEMENTS_01& elements_01,
                          const ELEMENTS_02& elements_02,
                          const ELEMENTS_03& elements_03,
                          const ELEMENTS_04& elements_04,
                          const ELEMENTS_05& elements_05,
                          const ELEMENTS_06& elements_06,
                          const ELEMENTS_07& elements_07,
                          const ELEMENTS_08& elements_08,
                          const ELEMENTS_09& elements_09,
                          const ELEMENTS_10& elements_10
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
bdld::Datum DatumMaker::a(const ELEMENTS_01& elements_01,
                          const ELEMENTS_02& elements_02,
                          const ELEMENTS_03& elements_03,
                          const ELEMENTS_04& elements_04,
                          const ELEMENTS_05& elements_05,
                          const ELEMENTS_06& elements_06,
                          const ELEMENTS_07& elements_07,
                          const ELEMENTS_08& elements_08,
                          const ELEMENTS_09& elements_09,
                          const ELEMENTS_10& elements_10,
                          const ELEMENTS_11& elements_11
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
bdld::Datum DatumMaker::a(const ELEMENTS_01& elements_01,
                          const ELEMENTS_02& elements_02,
                          const ELEMENTS_03& elements_03,
                          const ELEMENTS_04& elements_04,
                          const ELEMENTS_05& elements_05,
                          const ELEMENTS_06& elements_06,
                          const ELEMENTS_07& elements_07,
                          const ELEMENTS_08& elements_08,
                          const ELEMENTS_09& elements_09,
                          const ELEMENTS_10& elements_10,
                          const ELEMENTS_11& elements_11,
                          const ELEMENTS_12& elements_12
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
bdld::Datum DatumMaker::a(const ELEMENTS_01& elements_01,
                          const ELEMENTS_02& elements_02,
                          const ELEMENTS_03& elements_03,
                          const ELEMENTS_04& elements_04,
                          const ELEMENTS_05& elements_05,
                          const ELEMENTS_06& elements_06,
                          const ELEMENTS_07& elements_07,
                          const ELEMENTS_08& elements_08,
                          const ELEMENTS_09& elements_09,
                          const ELEMENTS_10& elements_10,
                          const ELEMENTS_11& elements_11,
                          const ELEMENTS_12& elements_12,
                          const ELEMENTS_13& elements_13
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
bdld::Datum DatumMaker::a(const ELEMENTS_01& elements_01,
                          const ELEMENTS_02& elements_02,
                          const ELEMENTS_03& elements_03,
                          const ELEMENTS_04& elements_04,
                          const ELEMENTS_05& elements_05,
                          const ELEMENTS_06& elements_06,
                          const ELEMENTS_07& elements_07,
                          const ELEMENTS_08& elements_08,
                          const ELEMENTS_09& elements_09,
                          const ELEMENTS_10& elements_10,
                          const ELEMENTS_11& elements_11,
                          const ELEMENTS_12& elements_12,
                          const ELEMENTS_13& elements_13,
                          const ELEMENTS_14& elements_14
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
bdld::Datum DatumMaker::a(const ELEMENTS_01& elements_01,
                          const ELEMENTS_02& elements_02,
                          const ELEMENTS_03& elements_03,
                          const ELEMENTS_04& elements_04,
                          const ELEMENTS_05& elements_05,
                          const ELEMENTS_06& elements_06,
                          const ELEMENTS_07& elements_07,
                          const ELEMENTS_08& elements_08,
                          const ELEMENTS_09& elements_09,
                          const ELEMENTS_10& elements_10,
                          const ELEMENTS_11& elements_11,
                          const ELEMENTS_12& elements_12,
                          const ELEMENTS_13& elements_13,
                          const ELEMENTS_14& elements_14,
                          const ELEMENTS_15& elements_15
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
bdld::Datum DatumMaker::a(const ELEMENTS_01& elements_01,
                          const ELEMENTS_02& elements_02,
                          const ELEMENTS_03& elements_03,
                          const ELEMENTS_04& elements_04,
                          const ELEMENTS_05& elements_05,
                          const ELEMENTS_06& elements_06,
                          const ELEMENTS_07& elements_07,
                          const ELEMENTS_08& elements_08,
                          const ELEMENTS_09& elements_09,
                          const ELEMENTS_10& elements_10,
                          const ELEMENTS_11& elements_11,
                          const ELEMENTS_12& elements_12,
                          const ELEMENTS_13& elements_13,
                          const ELEMENTS_14& elements_14,
                          const ELEMENTS_15& elements_15,
                          const ELEMENTS_16& elements_16
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

#endif

#if !BSLS_COMPILERFEATURES_SIMULATE_VARIADIC_TEMPLATES
template <typename... ENTRIES>
inline
bdld::Datum DatumMaker::m(const ENTRIES&... entries) const
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
#else
inline
bdld::Datum DatumMaker::m() const
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
bdld::Datum DatumMaker::m(const ENTRIES_01& entries_01,
                          const ENTRIES_02& entries_02
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
bdld::Datum DatumMaker::m(const ENTRIES_01& entries_01,
                          const ENTRIES_02& entries_02,
                          const ENTRIES_03& entries_03,
                          const ENTRIES_04& entries_04
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
bdld::Datum DatumMaker::m(const ENTRIES_01& entries_01,
                          const ENTRIES_02& entries_02,
                          const ENTRIES_03& entries_03,
                          const ENTRIES_04& entries_04,
                          const ENTRIES_05& entries_05,
                          const ENTRIES_06& entries_06
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
bdld::Datum DatumMaker::m(const ENTRIES_01& entries_01,
                          const ENTRIES_02& entries_02,
                          const ENTRIES_03& entries_03,
                          const ENTRIES_04& entries_04,
                          const ENTRIES_05& entries_05,
                          const ENTRIES_06& entries_06,
                          const ENTRIES_07& entries_07,
                          const ENTRIES_08& entries_08
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
bdld::Datum DatumMaker::m(const ENTRIES_01& entries_01,
                          const ENTRIES_02& entries_02,
                          const ENTRIES_03& entries_03,
                          const ENTRIES_04& entries_04,
                          const ENTRIES_05& entries_05,
                          const ENTRIES_06& entries_06,
                          const ENTRIES_07& entries_07,
                          const ENTRIES_08& entries_08,
                          const ENTRIES_09& entries_09,
                          const ENTRIES_10& entries_10
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
bdld::Datum DatumMaker::m(const ENTRIES_01& entries_01,
                          const ENTRIES_02& entries_02,
                          const ENTRIES_03& entries_03,
                          const ENTRIES_04& entries_04,
                          const ENTRIES_05& entries_05,
                          const ENTRIES_06& entries_06,
                          const ENTRIES_07& entries_07,
                          const ENTRIES_08& entries_08,
                          const ENTRIES_09& entries_09,
                          const ENTRIES_10& entries_10,
                          const ENTRIES_11& entries_11,
                          const ENTRIES_12& entries_12
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
bdld::Datum DatumMaker::m(const ENTRIES_01& entries_01,
                          const ENTRIES_02& entries_02,
                          const ENTRIES_03& entries_03,
                          const ENTRIES_04& entries_04,
                          const ENTRIES_05& entries_05,
                          const ENTRIES_06& entries_06,
                          const ENTRIES_07& entries_07,
                          const ENTRIES_08& entries_08,
                          const ENTRIES_09& entries_09,
                          const ENTRIES_10& entries_10,
                          const ENTRIES_11& entries_11,
                          const ENTRIES_12& entries_12,
                          const ENTRIES_13& entries_13,
                          const ENTRIES_14& entries_14
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
bdld::Datum DatumMaker::m(const ENTRIES_01& entries_01,
                          const ENTRIES_02& entries_02,
                          const ENTRIES_03& entries_03,
                          const ENTRIES_04& entries_04,
                          const ENTRIES_05& entries_05,
                          const ENTRIES_06& entries_06,
                          const ENTRIES_07& entries_07,
                          const ENTRIES_08& entries_08,
                          const ENTRIES_09& entries_09,
                          const ENTRIES_10& entries_10,
                          const ENTRIES_11& entries_11,
                          const ENTRIES_12& entries_12,
                          const ENTRIES_13& entries_13,
                          const ENTRIES_14& entries_14,
                          const ENTRIES_15& entries_15,
                          const ENTRIES_16& entries_16
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
bdld::Datum DatumMaker::m(const ENTRIES_01& entries_01,
                          const ENTRIES_02& entries_02,
                          const ENTRIES_03& entries_03,
                          const ENTRIES_04& entries_04,
                          const ENTRIES_05& entries_05,
                          const ENTRIES_06& entries_06,
                          const ENTRIES_07& entries_07,
                          const ENTRIES_08& entries_08,
                          const ENTRIES_09& entries_09,
                          const ENTRIES_10& entries_10,
                          const ENTRIES_11& entries_11,
                          const ENTRIES_12& entries_12,
                          const ENTRIES_13& entries_13,
                          const ENTRIES_14& entries_14,
                          const ENTRIES_15& entries_15,
                          const ENTRIES_16& entries_16,
                          const ENTRIES_17& entries_17,
                          const ENTRIES_18& entries_18
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
bdld::Datum DatumMaker::m(const ENTRIES_01& entries_01,
                          const ENTRIES_02& entries_02,
                          const ENTRIES_03& entries_03,
                          const ENTRIES_04& entries_04,
                          const ENTRIES_05& entries_05,
                          const ENTRIES_06& entries_06,
                          const ENTRIES_07& entries_07,
                          const ENTRIES_08& entries_08,
                          const ENTRIES_09& entries_09,
                          const ENTRIES_10& entries_10,
                          const ENTRIES_11& entries_11,
                          const ENTRIES_12& entries_12,
                          const ENTRIES_13& entries_13,
                          const ENTRIES_14& entries_14,
                          const ENTRIES_15& entries_15,
                          const ENTRIES_16& entries_16,
                          const ENTRIES_17& entries_17,
                          const ENTRIES_18& entries_18,
                          const ENTRIES_19& entries_19,
                          const ENTRIES_20& entries_20
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
bdld::Datum DatumMaker::m(const ENTRIES_01& entries_01,
                          const ENTRIES_02& entries_02,
                          const ENTRIES_03& entries_03,
                          const ENTRIES_04& entries_04,
                          const ENTRIES_05& entries_05,
                          const ENTRIES_06& entries_06,
                          const ENTRIES_07& entries_07,
                          const ENTRIES_08& entries_08,
                          const ENTRIES_09& entries_09,
                          const ENTRIES_10& entries_10,
                          const ENTRIES_11& entries_11,
                          const ENTRIES_12& entries_12,
                          const ENTRIES_13& entries_13,
                          const ENTRIES_14& entries_14,
                          const ENTRIES_15& entries_15,
                          const ENTRIES_16& entries_16,
                          const ENTRIES_17& entries_17,
                          const ENTRIES_18& entries_18,
                          const ENTRIES_19& entries_19,
                          const ENTRIES_20& entries_20,
                          const ENTRIES_21& entries_21,
                          const ENTRIES_22& entries_22
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
bdld::Datum DatumMaker::m(const ENTRIES_01& entries_01,
                          const ENTRIES_02& entries_02,
                          const ENTRIES_03& entries_03,
                          const ENTRIES_04& entries_04,
                          const ENTRIES_05& entries_05,
                          const ENTRIES_06& entries_06,
                          const ENTRIES_07& entries_07,
                          const ENTRIES_08& entries_08,
                          const ENTRIES_09& entries_09,
                          const ENTRIES_10& entries_10,
                          const ENTRIES_11& entries_11,
                          const ENTRIES_12& entries_12,
                          const ENTRIES_13& entries_13,
                          const ENTRIES_14& entries_14,
                          const ENTRIES_15& entries_15,
                          const ENTRIES_16& entries_16,
                          const ENTRIES_17& entries_17,
                          const ENTRIES_18& entries_18,
                          const ENTRIES_19& entries_19,
                          const ENTRIES_20& entries_20,
                          const ENTRIES_21& entries_21,
                          const ENTRIES_22& entries_22,
                          const ENTRIES_23& entries_23,
                          const ENTRIES_24& entries_24
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
bdld::Datum DatumMaker::m(const ENTRIES_01& entries_01,
                          const ENTRIES_02& entries_02,
                          const ENTRIES_03& entries_03,
                          const ENTRIES_04& entries_04,
                          const ENTRIES_05& entries_05,
                          const ENTRIES_06& entries_06,
                          const ENTRIES_07& entries_07,
                          const ENTRIES_08& entries_08,
                          const ENTRIES_09& entries_09,
                          const ENTRIES_10& entries_10,
                          const ENTRIES_11& entries_11,
                          const ENTRIES_12& entries_12,
                          const ENTRIES_13& entries_13,
                          const ENTRIES_14& entries_14,
                          const ENTRIES_15& entries_15,
                          const ENTRIES_16& entries_16,
                          const ENTRIES_17& entries_17,
                          const ENTRIES_18& entries_18,
                          const ENTRIES_19& entries_19,
                          const ENTRIES_20& entries_20,
                          const ENTRIES_21& entries_21,
                          const ENTRIES_22& entries_22,
                          const ENTRIES_23& entries_23,
                          const ENTRIES_24& entries_24,
                          const ENTRIES_25& entries_25,
                          const ENTRIES_26& entries_26
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
bdld::Datum DatumMaker::m(const ENTRIES_01& entries_01,
                          const ENTRIES_02& entries_02,
                          const ENTRIES_03& entries_03,
                          const ENTRIES_04& entries_04,
                          const ENTRIES_05& entries_05,
                          const ENTRIES_06& entries_06,
                          const ENTRIES_07& entries_07,
                          const ENTRIES_08& entries_08,
                          const ENTRIES_09& entries_09,
                          const ENTRIES_10& entries_10,
                          const ENTRIES_11& entries_11,
                          const ENTRIES_12& entries_12,
                          const ENTRIES_13& entries_13,
                          const ENTRIES_14& entries_14,
                          const ENTRIES_15& entries_15,
                          const ENTRIES_16& entries_16,
                          const ENTRIES_17& entries_17,
                          const ENTRIES_18& entries_18,
                          const ENTRIES_19& entries_19,
                          const ENTRIES_20& entries_20,
                          const ENTRIES_21& entries_21,
                          const ENTRIES_22& entries_22,
                          const ENTRIES_23& entries_23,
                          const ENTRIES_24& entries_24,
                          const ENTRIES_25& entries_25,
                          const ENTRIES_26& entries_26,
                          const ENTRIES_27& entries_27,
                          const ENTRIES_28& entries_28
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
bdld::Datum DatumMaker::m(const ENTRIES_01& entries_01,
                          const ENTRIES_02& entries_02,
                          const ENTRIES_03& entries_03,
                          const ENTRIES_04& entries_04,
                          const ENTRIES_05& entries_05,
                          const ENTRIES_06& entries_06,
                          const ENTRIES_07& entries_07,
                          const ENTRIES_08& entries_08,
                          const ENTRIES_09& entries_09,
                          const ENTRIES_10& entries_10,
                          const ENTRIES_11& entries_11,
                          const ENTRIES_12& entries_12,
                          const ENTRIES_13& entries_13,
                          const ENTRIES_14& entries_14,
                          const ENTRIES_15& entries_15,
                          const ENTRIES_16& entries_16,
                          const ENTRIES_17& entries_17,
                          const ENTRIES_18& entries_18,
                          const ENTRIES_19& entries_19,
                          const ENTRIES_20& entries_20,
                          const ENTRIES_21& entries_21,
                          const ENTRIES_22& entries_22,
                          const ENTRIES_23& entries_23,
                          const ENTRIES_24& entries_24,
                          const ENTRIES_25& entries_25,
                          const ENTRIES_26& entries_26,
                          const ENTRIES_27& entries_27,
                          const ENTRIES_28& entries_28,
                          const ENTRIES_29& entries_29,
                          const ENTRIES_30& entries_30
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
bdld::Datum DatumMaker::m(const ENTRIES_01& entries_01,
                          const ENTRIES_02& entries_02,
                          const ENTRIES_03& entries_03,
                          const ENTRIES_04& entries_04,
                          const ENTRIES_05& entries_05,
                          const ENTRIES_06& entries_06,
                          const ENTRIES_07& entries_07,
                          const ENTRIES_08& entries_08,
                          const ENTRIES_09& entries_09,
                          const ENTRIES_10& entries_10,
                          const ENTRIES_11& entries_11,
                          const ENTRIES_12& entries_12,
                          const ENTRIES_13& entries_13,
                          const ENTRIES_14& entries_14,
                          const ENTRIES_15& entries_15,
                          const ENTRIES_16& entries_16,
                          const ENTRIES_17& entries_17,
                          const ENTRIES_18& entries_18,
                          const ENTRIES_19& entries_19,
                          const ENTRIES_20& entries_20,
                          const ENTRIES_21& entries_21,
                          const ENTRIES_22& entries_22,
                          const ENTRIES_23& entries_23,
                          const ENTRIES_24& entries_24,
                          const ENTRIES_25& entries_25,
                          const ENTRIES_26& entries_26,
                          const ENTRIES_27& entries_27,
                          const ENTRIES_28& entries_28,
                          const ENTRIES_29& entries_29,
                          const ENTRIES_30& entries_30,
                          const ENTRIES_31& entries_31,
                          const ENTRIES_32& entries_32
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

#endif

#if !BSLS_COMPILERFEATURES_SIMULATE_VARIADIC_TEMPLATES
template <typename... ENTRIES>
inline
bdld::Datum DatumMaker::mok(const ENTRIES&... entries) const
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
#else
inline
bdld::Datum DatumMaker::mok() const
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
bdld::Datum DatumMaker::mok(const ENTRIES_01& entries_01,
                            const ENTRIES_02& entries_02
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
bdld::Datum DatumMaker::mok(const ENTRIES_01& entries_01,
                            const ENTRIES_02& entries_02,
                            const ENTRIES_03& entries_03,
                            const ENTRIES_04& entries_04
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
bdld::Datum DatumMaker::mok(const ENTRIES_01& entries_01,
                            const ENTRIES_02& entries_02,
                            const ENTRIES_03& entries_03,
                            const ENTRIES_04& entries_04,
                            const ENTRIES_05& entries_05,
                            const ENTRIES_06& entries_06
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
bdld::Datum DatumMaker::mok(const ENTRIES_01& entries_01,
                            const ENTRIES_02& entries_02,
                            const ENTRIES_03& entries_03,
                            const ENTRIES_04& entries_04,
                            const ENTRIES_05& entries_05,
                            const ENTRIES_06& entries_06,
                            const ENTRIES_07& entries_07,
                            const ENTRIES_08& entries_08
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
bdld::Datum DatumMaker::mok(const ENTRIES_01& entries_01,
                            const ENTRIES_02& entries_02,
                            const ENTRIES_03& entries_03,
                            const ENTRIES_04& entries_04,
                            const ENTRIES_05& entries_05,
                            const ENTRIES_06& entries_06,
                            const ENTRIES_07& entries_07,
                            const ENTRIES_08& entries_08,
                            const ENTRIES_09& entries_09,
                            const ENTRIES_10& entries_10
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
bdld::Datum DatumMaker::mok(const ENTRIES_01& entries_01,
                            const ENTRIES_02& entries_02,
                            const ENTRIES_03& entries_03,
                            const ENTRIES_04& entries_04,
                            const ENTRIES_05& entries_05,
                            const ENTRIES_06& entries_06,
                            const ENTRIES_07& entries_07,
                            const ENTRIES_08& entries_08,
                            const ENTRIES_09& entries_09,
                            const ENTRIES_10& entries_10,
                            const ENTRIES_11& entries_11,
                            const ENTRIES_12& entries_12
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
bdld::Datum DatumMaker::mok(const ENTRIES_01& entries_01,
                            const ENTRIES_02& entries_02,
                            const ENTRIES_03& entries_03,
                            const ENTRIES_04& entries_04,
                            const ENTRIES_05& entries_05,
                            const ENTRIES_06& entries_06,
                            const ENTRIES_07& entries_07,
                            const ENTRIES_08& entries_08,
                            const ENTRIES_09& entries_09,
                            const ENTRIES_10& entries_10,
                            const ENTRIES_11& entries_11,
                            const ENTRIES_12& entries_12,
                            const ENTRIES_13& entries_13,
                            const ENTRIES_14& entries_14
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
bdld::Datum DatumMaker::mok(const ENTRIES_01& entries_01,
                            const ENTRIES_02& entries_02,
                            const ENTRIES_03& entries_03,
                            const ENTRIES_04& entries_04,
                            const ENTRIES_05& entries_05,
                            const ENTRIES_06& entries_06,
                            const ENTRIES_07& entries_07,
                            const ENTRIES_08& entries_08,
                            const ENTRIES_09& entries_09,
                            const ENTRIES_10& entries_10,
                            const ENTRIES_11& entries_11,
                            const ENTRIES_12& entries_12,
                            const ENTRIES_13& entries_13,
                            const ENTRIES_14& entries_14,
                            const ENTRIES_15& entries_15,
                            const ENTRIES_16& entries_16
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
bdld::Datum DatumMaker::mok(const ENTRIES_01& entries_01,
                            const ENTRIES_02& entries_02,
                            const ENTRIES_03& entries_03,
                            const ENTRIES_04& entries_04,
                            const ENTRIES_05& entries_05,
                            const ENTRIES_06& entries_06,
                            const ENTRIES_07& entries_07,
                            const ENTRIES_08& entries_08,
                            const ENTRIES_09& entries_09,
                            const ENTRIES_10& entries_10,
                            const ENTRIES_11& entries_11,
                            const ENTRIES_12& entries_12,
                            const ENTRIES_13& entries_13,
                            const ENTRIES_14& entries_14,
                            const ENTRIES_15& entries_15,
                            const ENTRIES_16& entries_16,
                            const ENTRIES_17& entries_17,
                            const ENTRIES_18& entries_18
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
bdld::Datum DatumMaker::mok(const ENTRIES_01& entries_01,
                            const ENTRIES_02& entries_02,
                            const ENTRIES_03& entries_03,
                            const ENTRIES_04& entries_04,
                            const ENTRIES_05& entries_05,
                            const ENTRIES_06& entries_06,
                            const ENTRIES_07& entries_07,
                            const ENTRIES_08& entries_08,
                            const ENTRIES_09& entries_09,
                            const ENTRIES_10& entries_10,
                            const ENTRIES_11& entries_11,
                            const ENTRIES_12& entries_12,
                            const ENTRIES_13& entries_13,
                            const ENTRIES_14& entries_14,
                            const ENTRIES_15& entries_15,
                            const ENTRIES_16& entries_16,
                            const ENTRIES_17& entries_17,
                            const ENTRIES_18& entries_18,
                            const ENTRIES_19& entries_19,
                            const ENTRIES_20& entries_20
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
bdld::Datum DatumMaker::mok(const ENTRIES_01& entries_01,
                            const ENTRIES_02& entries_02,
                            const ENTRIES_03& entries_03,
                            const ENTRIES_04& entries_04,
                            const ENTRIES_05& entries_05,
                            const ENTRIES_06& entries_06,
                            const ENTRIES_07& entries_07,
                            const ENTRIES_08& entries_08,
                            const ENTRIES_09& entries_09,
                            const ENTRIES_10& entries_10,
                            const ENTRIES_11& entries_11,
                            const ENTRIES_12& entries_12,
                            const ENTRIES_13& entries_13,
                            const ENTRIES_14& entries_14,
                            const ENTRIES_15& entries_15,
                            const ENTRIES_16& entries_16,
                            const ENTRIES_17& entries_17,
                            const ENTRIES_18& entries_18,
                            const ENTRIES_19& entries_19,
                            const ENTRIES_20& entries_20,
                            const ENTRIES_21& entries_21,
                            const ENTRIES_22& entries_22
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
bdld::Datum DatumMaker::mok(const ENTRIES_01& entries_01,
                            const ENTRIES_02& entries_02,
                            const ENTRIES_03& entries_03,
                            const ENTRIES_04& entries_04,
                            const ENTRIES_05& entries_05,
                            const ENTRIES_06& entries_06,
                            const ENTRIES_07& entries_07,
                            const ENTRIES_08& entries_08,
                            const ENTRIES_09& entries_09,
                            const ENTRIES_10& entries_10,
                            const ENTRIES_11& entries_11,
                            const ENTRIES_12& entries_12,
                            const ENTRIES_13& entries_13,
                            const ENTRIES_14& entries_14,
                            const ENTRIES_15& entries_15,
                            const ENTRIES_16& entries_16,
                            const ENTRIES_17& entries_17,
                            const ENTRIES_18& entries_18,
                            const ENTRIES_19& entries_19,
                            const ENTRIES_20& entries_20,
                            const ENTRIES_21& entries_21,
                            const ENTRIES_22& entries_22,
                            const ENTRIES_23& entries_23,
                            const ENTRIES_24& entries_24
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
bdld::Datum DatumMaker::mok(const ENTRIES_01& entries_01,
                            const ENTRIES_02& entries_02,
                            const ENTRIES_03& entries_03,
                            const ENTRIES_04& entries_04,
                            const ENTRIES_05& entries_05,
                            const ENTRIES_06& entries_06,
                            const ENTRIES_07& entries_07,
                            const ENTRIES_08& entries_08,
                            const ENTRIES_09& entries_09,
                            const ENTRIES_10& entries_10,
                            const ENTRIES_11& entries_11,
                            const ENTRIES_12& entries_12,
                            const ENTRIES_13& entries_13,
                            const ENTRIES_14& entries_14,
                            const ENTRIES_15& entries_15,
                            const ENTRIES_16& entries_16,
                            const ENTRIES_17& entries_17,
                            const ENTRIES_18& entries_18,
                            const ENTRIES_19& entries_19,
                            const ENTRIES_20& entries_20,
                            const ENTRIES_21& entries_21,
                            const ENTRIES_22& entries_22,
                            const ENTRIES_23& entries_23,
                            const ENTRIES_24& entries_24,
                            const ENTRIES_25& entries_25,
                            const ENTRIES_26& entries_26
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
bdld::Datum DatumMaker::mok(const ENTRIES_01& entries_01,
                            const ENTRIES_02& entries_02,
                            const ENTRIES_03& entries_03,
                            const ENTRIES_04& entries_04,
                            const ENTRIES_05& entries_05,
                            const ENTRIES_06& entries_06,
                            const ENTRIES_07& entries_07,
                            const ENTRIES_08& entries_08,
                            const ENTRIES_09& entries_09,
                            const ENTRIES_10& entries_10,
                            const ENTRIES_11& entries_11,
                            const ENTRIES_12& entries_12,
                            const ENTRIES_13& entries_13,
                            const ENTRIES_14& entries_14,
                            const ENTRIES_15& entries_15,
                            const ENTRIES_16& entries_16,
                            const ENTRIES_17& entries_17,
                            const ENTRIES_18& entries_18,
                            const ENTRIES_19& entries_19,
                            const ENTRIES_20& entries_20,
                            const ENTRIES_21& entries_21,
                            const ENTRIES_22& entries_22,
                            const ENTRIES_23& entries_23,
                            const ENTRIES_24& entries_24,
                            const ENTRIES_25& entries_25,
                            const ENTRIES_26& entries_26,
                            const ENTRIES_27& entries_27,
                            const ENTRIES_28& entries_28
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
bdld::Datum DatumMaker::mok(const ENTRIES_01& entries_01,
                            const ENTRIES_02& entries_02,
                            const ENTRIES_03& entries_03,
                            const ENTRIES_04& entries_04,
                            const ENTRIES_05& entries_05,
                            const ENTRIES_06& entries_06,
                            const ENTRIES_07& entries_07,
                            const ENTRIES_08& entries_08,
                            const ENTRIES_09& entries_09,
                            const ENTRIES_10& entries_10,
                            const ENTRIES_11& entries_11,
                            const ENTRIES_12& entries_12,
                            const ENTRIES_13& entries_13,
                            const ENTRIES_14& entries_14,
                            const ENTRIES_15& entries_15,
                            const ENTRIES_16& entries_16,
                            const ENTRIES_17& entries_17,
                            const ENTRIES_18& entries_18,
                            const ENTRIES_19& entries_19,
                            const ENTRIES_20& entries_20,
                            const ENTRIES_21& entries_21,
                            const ENTRIES_22& entries_22,
                            const ENTRIES_23& entries_23,
                            const ENTRIES_24& entries_24,
                            const ENTRIES_25& entries_25,
                            const ENTRIES_26& entries_26,
                            const ENTRIES_27& entries_27,
                            const ENTRIES_28& entries_28,
                            const ENTRIES_29& entries_29,
                            const ENTRIES_30& entries_30
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
bdld::Datum DatumMaker::mok(const ENTRIES_01& entries_01,
                            const ENTRIES_02& entries_02,
                            const ENTRIES_03& entries_03,
                            const ENTRIES_04& entries_04,
                            const ENTRIES_05& entries_05,
                            const ENTRIES_06& entries_06,
                            const ENTRIES_07& entries_07,
                            const ENTRIES_08& entries_08,
                            const ENTRIES_09& entries_09,
                            const ENTRIES_10& entries_10,
                            const ENTRIES_11& entries_11,
                            const ENTRIES_12& entries_12,
                            const ENTRIES_13& entries_13,
                            const ENTRIES_14& entries_14,
                            const ENTRIES_15& entries_15,
                            const ENTRIES_16& entries_16,
                            const ENTRIES_17& entries_17,
                            const ENTRIES_18& entries_18,
                            const ENTRIES_19& entries_19,
                            const ENTRIES_20& entries_20,
                            const ENTRIES_21& entries_21,
                            const ENTRIES_22& entries_22,
                            const ENTRIES_23& entries_23,
                            const ENTRIES_24& entries_24,
                            const ENTRIES_25& entries_25,
                            const ENTRIES_26& entries_26,
                            const ENTRIES_27& entries_27,
                            const ENTRIES_28& entries_28,
                            const ENTRIES_29& entries_29,
                            const ENTRIES_30& entries_30,
                            const ENTRIES_31& entries_31,
                            const ENTRIES_32& entries_32
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

#endif


#if !BSLS_COMPILERFEATURES_SIMULATE_VARIADIC_TEMPLATES
template <typename... ENTRIES>
inline
bdld::Datum DatumMaker::im(const ENTRIES&... entries) const
{
    const int numArguments = sizeof...(ENTRIES);

    // Due to MSVC not recognizing bitwise and of a constant expression and a
    // string literal as a constant expression, we don't use a meaningful error
    // as part of this assert.
    //
    // See: https://connect.microsoft.com/VisualStudio/feedback/details/1523001
    BSLMF_ASSERT(0 == numArguments % 2);

    const int mapElements = numArguments / 2;
    bdld::DatumIntMapBuilder builder(mapElements, d_allocator_p);
    pushBackHelper(&builder, entries...);
    return builder.commit();
}
#else
inline
bdld::Datum DatumMaker::im() const
{
    const int mapElements = 0;
    bdld::DatumIntMapBuilder builder(mapElements, d_allocator_p);
    pushBackHelper(&builder);
    return builder.commit();
}

template <typename ENTRY_01>
inline
bdld::Datum DatumMaker::im(int             key_01,
                           const ENTRY_01& entry_01
                           ) const
{
    const int mapElements = 1;
    bdld::DatumIntMapBuilder builder(mapElements, d_allocator_p);
    pushBackHelper(&builder, key_01, entry_01);
    return builder.commit();
}

template <typename ENTRY_01,
          typename ENTRY_02>
inline
bdld::Datum DatumMaker::im(int             key_01,
                           const ENTRY_01& entry_01,
                           int             key_02,
                           const ENTRY_02& entry_02
                           ) const
{
    const int mapElements = 2;
    bdld::DatumIntMapBuilder builder(mapElements, d_allocator_p);
    pushBackHelper(&builder, key_01, entry_01,
                             key_02, entry_02);
    return builder.commit();
}

template <typename ENTRY_01,
          typename ENTRY_02,
          typename ENTRY_03>
inline
bdld::Datum DatumMaker::im(int             key_01,
                           const ENTRY_01& entry_01,
                           int             key_02,
                           const ENTRY_02& entry_02,
                           int             key_03,
                           const ENTRY_03& entry_03
                           ) const
{
    const int mapElements = 3;
    bdld::DatumIntMapBuilder builder(mapElements, d_allocator_p);
    pushBackHelper(&builder, key_01, entry_01,
                             key_02, entry_02,
                             key_03, entry_03);
    return builder.commit();
}

template <typename ENTRY_01,
          typename ENTRY_02,
          typename ENTRY_03,
          typename ENTRY_04>
inline
bdld::Datum DatumMaker::im(int             key_01,
                           const ENTRY_01& entry_01,
                           int             key_02,
                           const ENTRY_02& entry_02,
                           int             key_03,
                           const ENTRY_03& entry_03,
                           int             key_04,
                           const ENTRY_04& entry_04
                           ) const
{
    const int mapElements = 4;
    bdld::DatumIntMapBuilder builder(mapElements, d_allocator_p);
    pushBackHelper(&builder, key_01, entry_01,
                             key_02, entry_02,
                             key_03, entry_03,
                             key_04, entry_04);
    return builder.commit();
}

template <typename ENTRY_01,
          typename ENTRY_02,
          typename ENTRY_03,
          typename ENTRY_04,
          typename ENTRY_05>
inline
bdld::Datum DatumMaker::im(int             key_01,
                           const ENTRY_01& entry_01,
                           int             key_02,
                           const ENTRY_02& entry_02,
                           int             key_03,
                           const ENTRY_03& entry_03,
                           int             key_04,
                           const ENTRY_04& entry_04,
                           int             key_05,
                           const ENTRY_05& entry_05
                           ) const
{
    const int mapElements = 5;
    bdld::DatumIntMapBuilder builder(mapElements, d_allocator_p);
    pushBackHelper(&builder, key_01, entry_01,
                             key_02, entry_02,
                             key_03, entry_03,
                             key_04, entry_04,
                             key_05, entry_05);
    return builder.commit();
}

template <typename ENTRY_01,
          typename ENTRY_02,
          typename ENTRY_03,
          typename ENTRY_04,
          typename ENTRY_05,
          typename ENTRY_06>
inline
bdld::Datum DatumMaker::im(int             key_01,
                           const ENTRY_01& entry_01,
                           int             key_02,
                           const ENTRY_02& entry_02,
                           int             key_03,
                           const ENTRY_03& entry_03,
                           int             key_04,
                           const ENTRY_04& entry_04,
                           int             key_05,
                           const ENTRY_05& entry_05,
                           int             key_06,
                           const ENTRY_06& entry_06
                           ) const
{
    const int mapElements = 6;
    bdld::DatumIntMapBuilder builder(mapElements, d_allocator_p);
    pushBackHelper(&builder, key_01, entry_01,
                             key_02, entry_02,
                             key_03, entry_03,
                             key_04, entry_04,
                             key_05, entry_05,
                             key_06, entry_06);
    return builder.commit();
}

template <typename ENTRY_01,
          typename ENTRY_02,
          typename ENTRY_03,
          typename ENTRY_04,
          typename ENTRY_05,
          typename ENTRY_06,
          typename ENTRY_07>
inline
bdld::Datum DatumMaker::im(int             key_01,
                           const ENTRY_01& entry_01,
                           int             key_02,
                           const ENTRY_02& entry_02,
                           int             key_03,
                           const ENTRY_03& entry_03,
                           int             key_04,
                           const ENTRY_04& entry_04,
                           int             key_05,
                           const ENTRY_05& entry_05,
                           int             key_06,
                           const ENTRY_06& entry_06,
                           int             key_07,
                           const ENTRY_07& entry_07
                           ) const
{
    const int mapElements = 7;
    bdld::DatumIntMapBuilder builder(mapElements, d_allocator_p);
    pushBackHelper(&builder, key_01, entry_01,
                             key_02, entry_02,
                             key_03, entry_03,
                             key_04, entry_04,
                             key_05, entry_05,
                             key_06, entry_06,
                             key_07, entry_07);
    return builder.commit();
}

template <typename ENTRY_01,
          typename ENTRY_02,
          typename ENTRY_03,
          typename ENTRY_04,
          typename ENTRY_05,
          typename ENTRY_06,
          typename ENTRY_07,
          typename ENTRY_08>
inline
bdld::Datum DatumMaker::im(int             key_01,
                           const ENTRY_01& entry_01,
                           int             key_02,
                           const ENTRY_02& entry_02,
                           int             key_03,
                           const ENTRY_03& entry_03,
                           int             key_04,
                           const ENTRY_04& entry_04,
                           int             key_05,
                           const ENTRY_05& entry_05,
                           int             key_06,
                           const ENTRY_06& entry_06,
                           int             key_07,
                           const ENTRY_07& entry_07,
                           int             key_08,
                           const ENTRY_08& entry_08
                           ) const
{
    const int mapElements = 8;
    bdld::DatumIntMapBuilder builder(mapElements, d_allocator_p);
    pushBackHelper(&builder, key_01, entry_01,
                             key_02, entry_02,
                             key_03, entry_03,
                             key_04, entry_04,
                             key_05, entry_05,
                             key_06, entry_06,
                             key_07, entry_07,
                             key_08, entry_08);
    return builder.commit();
}

template <typename ENTRY_01,
          typename ENTRY_02,
          typename ENTRY_03,
          typename ENTRY_04,
          typename ENTRY_05,
          typename ENTRY_06,
          typename ENTRY_07,
          typename ENTRY_08,
          typename ENTRY_09>
inline
bdld::Datum DatumMaker::im(int             key_01,
                           const ENTRY_01& entry_01,
                           int             key_02,
                           const ENTRY_02& entry_02,
                           int             key_03,
                           const ENTRY_03& entry_03,
                           int             key_04,
                           const ENTRY_04& entry_04,
                           int             key_05,
                           const ENTRY_05& entry_05,
                           int             key_06,
                           const ENTRY_06& entry_06,
                           int             key_07,
                           const ENTRY_07& entry_07,
                           int             key_08,
                           const ENTRY_08& entry_08,
                           int             key_09,
                           const ENTRY_09& entry_09
                           ) const
{
    const int mapElements = 9;
    bdld::DatumIntMapBuilder builder(mapElements, d_allocator_p);
    pushBackHelper(&builder, key_01, entry_01,
                             key_02, entry_02,
                             key_03, entry_03,
                             key_04, entry_04,
                             key_05, entry_05,
                             key_06, entry_06,
                             key_07, entry_07,
                             key_08, entry_08,
                             key_09, entry_09);
    return builder.commit();
}

template <typename ENTRY_01,
          typename ENTRY_02,
          typename ENTRY_03,
          typename ENTRY_04,
          typename ENTRY_05,
          typename ENTRY_06,
          typename ENTRY_07,
          typename ENTRY_08,
          typename ENTRY_09,
          typename ENTRY_10>
inline
bdld::Datum DatumMaker::im(int             key_01,
                           const ENTRY_01& entry_01,
                           int             key_02,
                           const ENTRY_02& entry_02,
                           int             key_03,
                           const ENTRY_03& entry_03,
                           int             key_04,
                           const ENTRY_04& entry_04,
                           int             key_05,
                           const ENTRY_05& entry_05,
                           int             key_06,
                           const ENTRY_06& entry_06,
                           int             key_07,
                           const ENTRY_07& entry_07,
                           int             key_08,
                           const ENTRY_08& entry_08,
                           int             key_09,
                           const ENTRY_09& entry_09,
                           int             key_10,
                           const ENTRY_10& entry_10
                           ) const
{
    const int mapElements = 10;
    bdld::DatumIntMapBuilder builder(mapElements, d_allocator_p);
    pushBackHelper(&builder, key_01, entry_01,
                             key_02, entry_02,
                             key_03, entry_03,
                             key_04, entry_04,
                             key_05, entry_05,
                             key_06, entry_06,
                             key_07, entry_07,
                             key_08, entry_08,
                             key_09, entry_09,
                             key_10, entry_10);
    return builder.commit();
}

template <typename ENTRY_01,
          typename ENTRY_02,
          typename ENTRY_03,
          typename ENTRY_04,
          typename ENTRY_05,
          typename ENTRY_06,
          typename ENTRY_07,
          typename ENTRY_08,
          typename ENTRY_09,
          typename ENTRY_10,
          typename ENTRY_11>
inline
bdld::Datum DatumMaker::im(int             key_01,
                           const ENTRY_01& entry_01,
                           int             key_02,
                           const ENTRY_02& entry_02,
                           int             key_03,
                           const ENTRY_03& entry_03,
                           int             key_04,
                           const ENTRY_04& entry_04,
                           int             key_05,
                           const ENTRY_05& entry_05,
                           int             key_06,
                           const ENTRY_06& entry_06,
                           int             key_07,
                           const ENTRY_07& entry_07,
                           int             key_08,
                           const ENTRY_08& entry_08,
                           int             key_09,
                           const ENTRY_09& entry_09,
                           int             key_10,
                           const ENTRY_10& entry_10,
                           int             key_11,
                           const ENTRY_11& entry_11
                           ) const
{
    const int mapElements = 11;
    bdld::DatumIntMapBuilder builder(mapElements, d_allocator_p);
    pushBackHelper(&builder, key_01, entry_01,
                             key_02, entry_02,
                             key_03, entry_03,
                             key_04, entry_04,
                             key_05, entry_05,
                             key_06, entry_06,
                             key_07, entry_07,
                             key_08, entry_08,
                             key_09, entry_09,
                             key_10, entry_10,
                             key_11, entry_11);
    return builder.commit();
}

template <typename ENTRY_01,
          typename ENTRY_02,
          typename ENTRY_03,
          typename ENTRY_04,
          typename ENTRY_05,
          typename ENTRY_06,
          typename ENTRY_07,
          typename ENTRY_08,
          typename ENTRY_09,
          typename ENTRY_10,
          typename ENTRY_11,
          typename ENTRY_12>
inline
bdld::Datum DatumMaker::im(int             key_01,
                           const ENTRY_01& entry_01,
                           int             key_02,
                           const ENTRY_02& entry_02,
                           int             key_03,
                           const ENTRY_03& entry_03,
                           int             key_04,
                           const ENTRY_04& entry_04,
                           int             key_05,
                           const ENTRY_05& entry_05,
                           int             key_06,
                           const ENTRY_06& entry_06,
                           int             key_07,
                           const ENTRY_07& entry_07,
                           int             key_08,
                           const ENTRY_08& entry_08,
                           int             key_09,
                           const ENTRY_09& entry_09,
                           int             key_10,
                           const ENTRY_10& entry_10,
                           int             key_11,
                           const ENTRY_11& entry_11,
                           int             key_12,
                           const ENTRY_12& entry_12
                           ) const
{
    const int mapElements = 12;
    bdld::DatumIntMapBuilder builder(mapElements, d_allocator_p);
    pushBackHelper(&builder, key_01, entry_01,
                             key_02, entry_02,
                             key_03, entry_03,
                             key_04, entry_04,
                             key_05, entry_05,
                             key_06, entry_06,
                             key_07, entry_07,
                             key_08, entry_08,
                             key_09, entry_09,
                             key_10, entry_10,
                             key_11, entry_11,
                             key_12, entry_12);
    return builder.commit();
}

template <typename ENTRY_01,
          typename ENTRY_02,
          typename ENTRY_03,
          typename ENTRY_04,
          typename ENTRY_05,
          typename ENTRY_06,
          typename ENTRY_07,
          typename ENTRY_08,
          typename ENTRY_09,
          typename ENTRY_10,
          typename ENTRY_11,
          typename ENTRY_12,
          typename ENTRY_13>
inline
bdld::Datum DatumMaker::im(int             key_01,
                           const ENTRY_01& entry_01,
                           int             key_02,
                           const ENTRY_02& entry_02,
                           int             key_03,
                           const ENTRY_03& entry_03,
                           int             key_04,
                           const ENTRY_04& entry_04,
                           int             key_05,
                           const ENTRY_05& entry_05,
                           int             key_06,
                           const ENTRY_06& entry_06,
                           int             key_07,
                           const ENTRY_07& entry_07,
                           int             key_08,
                           const ENTRY_08& entry_08,
                           int             key_09,
                           const ENTRY_09& entry_09,
                           int             key_10,
                           const ENTRY_10& entry_10,
                           int             key_11,
                           const ENTRY_11& entry_11,
                           int             key_12,
                           const ENTRY_12& entry_12,
                           int             key_13,
                           const ENTRY_13& entry_13
                           ) const
{
    const int mapElements = 13;
    bdld::DatumIntMapBuilder builder(mapElements, d_allocator_p);
    pushBackHelper(&builder, key_01, entry_01,
                             key_02, entry_02,
                             key_03, entry_03,
                             key_04, entry_04,
                             key_05, entry_05,
                             key_06, entry_06,
                             key_07, entry_07,
                             key_08, entry_08,
                             key_09, entry_09,
                             key_10, entry_10,
                             key_11, entry_11,
                             key_12, entry_12,
                             key_13, entry_13);
    return builder.commit();
}

template <typename ENTRY_01,
          typename ENTRY_02,
          typename ENTRY_03,
          typename ENTRY_04,
          typename ENTRY_05,
          typename ENTRY_06,
          typename ENTRY_07,
          typename ENTRY_08,
          typename ENTRY_09,
          typename ENTRY_10,
          typename ENTRY_11,
          typename ENTRY_12,
          typename ENTRY_13,
          typename ENTRY_14>
inline
bdld::Datum DatumMaker::im(int             key_01,
                           const ENTRY_01& entry_01,
                           int             key_02,
                           const ENTRY_02& entry_02,
                           int             key_03,
                           const ENTRY_03& entry_03,
                           int             key_04,
                           const ENTRY_04& entry_04,
                           int             key_05,
                           const ENTRY_05& entry_05,
                           int             key_06,
                           const ENTRY_06& entry_06,
                           int             key_07,
                           const ENTRY_07& entry_07,
                           int             key_08,
                           const ENTRY_08& entry_08,
                           int             key_09,
                           const ENTRY_09& entry_09,
                           int             key_10,
                           const ENTRY_10& entry_10,
                           int             key_11,
                           const ENTRY_11& entry_11,
                           int             key_12,
                           const ENTRY_12& entry_12,
                           int             key_13,
                           const ENTRY_13& entry_13,
                           int             key_14,
                           const ENTRY_14& entry_14
                           ) const
{
    const int mapElements = 14;
    bdld::DatumIntMapBuilder builder(mapElements, d_allocator_p);
    pushBackHelper(&builder, key_01, entry_01,
                             key_02, entry_02,
                             key_03, entry_03,
                             key_04, entry_04,
                             key_05, entry_05,
                             key_06, entry_06,
                             key_07, entry_07,
                             key_08, entry_08,
                             key_09, entry_09,
                             key_10, entry_10,
                             key_11, entry_11,
                             key_12, entry_12,
                             key_13, entry_13,
                             key_14, entry_14);
    return builder.commit();
}

template <typename ENTRY_01,
          typename ENTRY_02,
          typename ENTRY_03,
          typename ENTRY_04,
          typename ENTRY_05,
          typename ENTRY_06,
          typename ENTRY_07,
          typename ENTRY_08,
          typename ENTRY_09,
          typename ENTRY_10,
          typename ENTRY_11,
          typename ENTRY_12,
          typename ENTRY_13,
          typename ENTRY_14,
          typename ENTRY_15>
inline
bdld::Datum DatumMaker::im(int             key_01,
                           const ENTRY_01& entry_01,
                           int             key_02,
                           const ENTRY_02& entry_02,
                           int             key_03,
                           const ENTRY_03& entry_03,
                           int             key_04,
                           const ENTRY_04& entry_04,
                           int             key_05,
                           const ENTRY_05& entry_05,
                           int             key_06,
                           const ENTRY_06& entry_06,
                           int             key_07,
                           const ENTRY_07& entry_07,
                           int             key_08,
                           const ENTRY_08& entry_08,
                           int             key_09,
                           const ENTRY_09& entry_09,
                           int             key_10,
                           const ENTRY_10& entry_10,
                           int             key_11,
                           const ENTRY_11& entry_11,
                           int             key_12,
                           const ENTRY_12& entry_12,
                           int             key_13,
                           const ENTRY_13& entry_13,
                           int             key_14,
                           const ENTRY_14& entry_14,
                           int             key_15,
                           const ENTRY_15& entry_15
                           ) const
{
    const int mapElements = 15;
    bdld::DatumIntMapBuilder builder(mapElements, d_allocator_p);
    pushBackHelper(&builder, key_01, entry_01,
                             key_02, entry_02,
                             key_03, entry_03,
                             key_04, entry_04,
                             key_05, entry_05,
                             key_06, entry_06,
                             key_07, entry_07,
                             key_08, entry_08,
                             key_09, entry_09,
                             key_10, entry_10,
                             key_11, entry_11,
                             key_12, entry_12,
                             key_13, entry_13,
                             key_14, entry_14,
                             key_15, entry_15);
    return builder.commit();
}

template <typename ENTRY_01,
          typename ENTRY_02,
          typename ENTRY_03,
          typename ENTRY_04,
          typename ENTRY_05,
          typename ENTRY_06,
          typename ENTRY_07,
          typename ENTRY_08,
          typename ENTRY_09,
          typename ENTRY_10,
          typename ENTRY_11,
          typename ENTRY_12,
          typename ENTRY_13,
          typename ENTRY_14,
          typename ENTRY_15,
          typename ENTRY_16>
inline
bdld::Datum DatumMaker::im(int             key_01,
                           const ENTRY_01& entry_01,
                           int             key_02,
                           const ENTRY_02& entry_02,
                           int             key_03,
                           const ENTRY_03& entry_03,
                           int             key_04,
                           const ENTRY_04& entry_04,
                           int             key_05,
                           const ENTRY_05& entry_05,
                           int             key_06,
                           const ENTRY_06& entry_06,
                           int             key_07,
                           const ENTRY_07& entry_07,
                           int             key_08,
                           const ENTRY_08& entry_08,
                           int             key_09,
                           const ENTRY_09& entry_09,
                           int             key_10,
                           const ENTRY_10& entry_10,
                           int             key_11,
                           const ENTRY_11& entry_11,
                           int             key_12,
                           const ENTRY_12& entry_12,
                           int             key_13,
                           const ENTRY_13& entry_13,
                           int             key_14,
                           const ENTRY_14& entry_14,
                           int             key_15,
                           const ENTRY_15& entry_15,
                           int             key_16,
                           const ENTRY_16& entry_16
                           ) const
{
    const int mapElements = 16;
    bdld::DatumIntMapBuilder builder(mapElements, d_allocator_p);
    pushBackHelper(&builder, key_01, entry_01,
                             key_02, entry_02,
                             key_03, entry_03,
                             key_04, entry_04,
                             key_05, entry_05,
                             key_06, entry_06,
                             key_07, entry_07,
                             key_08, entry_08,
                             key_09, entry_09,
                             key_10, entry_10,
                             key_11, entry_11,
                             key_12, entry_12,
                             key_13, entry_13,
                             key_14, entry_14,
                             key_15, entry_15,
                             key_16, entry_16);
    return builder.commit();
}

#endif

inline
bdld::Datum DatumMaker::ref(const bslstl::StringRef& string) const
{
    return bdld::Datum::createStringRef(string, d_allocator_p);
}

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
