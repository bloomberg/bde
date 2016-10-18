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

#ifndef INCLUDED_DLCSCM_VERSION
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

#ifndef INCLUDED_BDLB_NULLABLEVALUE
#include <bdlb_nullablevalue.h>
#endif

#ifndef INCLUDED_BSLMF_ASSERT
#include <bslmf_assert.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSLS_COMPILERFEATURES
#include <bsls_compilerfeatures.h>
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
        // 'bool'. This (unimplemented) function template should not be
        // instantiated unless 'operator()' is called with an unsupported type.

#ifdef BSLS_COMPILERFEATURES_SUPPORT_VARIADIC_TEMPLATES
    // PRIVATE ACCESSORS
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
#endif

  public:
    // CREATORS
    explicit DatumMaker(bslma::Allocator *basicAllocator);
        // Create a new 'DatumMaker' object using the specified
        // 'basicAllocator' to allocate memory for created 'bdld::Datum'
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
    bdld::Datum operator()(bsls::Types::Int64               value) const;
    bdld::Datum operator()(const bdld::DatumUdt&            value) const;
    bdld::Datum operator()(const bdld::Datum&               value) const;
    bdld::Datum operator()(const bdld::DatumArrayRef&       value) const;
    bdld::Datum operator()(const bdld::DatumMutableMapRef&  value) const;
        // Return a 'bdld::Datum' having the specified 'value'.  Note that
        // where possible, no memory is allocated - array are returned as
        // references.  Note that 'ConstDatumMapRef' is not supported at the
        // moment.

    bdld::Datum operator()(const bdld::Datum               *elements,
                           int                              size)  const;
    bdld::Datum operator()(const bdld::DatumMapEntry *elements,
                           int                        size,
                           bool                       sorted = false)  const;
        // Return a 'bdld::Datum' having the specified 'size' number of
        // 'elements'.  Note that where possible, no memory is allocated -
        // arrays are returned as references.  Note that 'DatumMapRef' is
        // not supported at the moment.

    bdld::Datum operator()(const bslstl::StringRef&  value) const;
    bdld::Datum operator()(const char               *value) const;
        // Return a 'bdld::Datum' having the specified 'value'.  The returned
        // 'bdld::Datum' object will contain a deep-copy of 'value'.

    template <class TYPE>
    bdld::Datum operator()(const bdlb::NullableValue<TYPE>& value) const;
        // Return a 'bdld::Datum' having the specified 'value', or null if
        // 'value' is unset.

    bdld::Datum a() const;
        // Return a 'bdld::Datum' having an empty array value.

#ifdef BSLS_COMPILERFEATURES_SUPPORT_VARIADIC_TEMPLATES
    template <typename... ELEMENTS>
    bdld::Datum a(const ELEMENTS&... elements) const;
        // Return a 'bdld::Datum' having an array value of the specified
        // 'elements'.
#else
    template <class T0> bdld::Datum a(const T0& t0) const;
    template <class T0, class T1>
                        bdld::Datum a(const T0& t0,
                                      const T1& t1) const;
    template <class T0, class T1, class T2>
                        bdld::Datum a(const T0& t0,
                                      const T1& t1,
                                      const T2& t2) const;
    template <class T0, class T1, class T2, class T3>
                        bdld::Datum a(const T0& t0,
                                      const T1& t1,
                                      const T2& t2,
                                      const T3& t3) const;
    template <class T0, class T1, class T2, class T3, class T4>
                        bdld::Datum a(const T0& t0,
                                      const T1& t1,
                                      const T2& t2,
                                      const T3& t3,
                                      const T4& t4) const;
    template <class T0, class T1, class T2, class T3, class T4, class T5>
                        bdld::Datum a(const T0& t0,
                                      const T1& t1,
                                      const T2& t2,
                                      const T3& t3,
                                      const T4& t4,
                                      const T5& t5) const;
    template <class T0, class T1, class T2, class T3, class T4, class T5,
              class T6>
                        bdld::Datum a(const T0& t0,
                                      const T1& t1,
                                      const T2& t2,
                                      const T3& t3,
                                      const T4& t4,
                                      const T5& t5,
                                      const T6& t6) const;
    template <class T0, class T1, class T2, class T3, class T4, class T5,
              class T6, class T7>
                        bdld::Datum a(const T0& t0,
                                      const T1& t1,
                                      const T2& t2,
                                      const T3& t3,
                                      const T4& t4,
                                      const T5& t5,
                                      const T6& t6,
                                      const T7& t7) const;
    template <class T0, class T1, class T2, class T3, class T4, class T5,
              class T6, class T7, class T8>
                        bdld::Datum a(const T0& t0,
                                      const T1& t1,
                                      const T2& t2,
                                      const T3& t3,
                                      const T4& t4,
                                      const T5& t5,
                                      const T6& t6,
                                      const T7& t7,
                                      const T8& t8) const;
    template <class T0, class T1, class T2, class T3, class T4, class T5,
              class T6, class T7, class T8, class T9>
                        bdld::Datum a(const T0& t0,
                                      const T1& t1,
                                      const T2& t2,
                                      const T3& t3,
                                      const T4& t4,
                                      const T5& t5,
                                      const T6& t6,
                                      const T7& t7,
                                      const T8& t8,
                                      const T9& t9) const;
    template <class T0, class T1, class T2, class T3, class T4, class T5,
              class T6, class T7, class T8, class T9, class T10>
                        bdld::Datum a(const T0&  t0,
                                      const T1&  t1,
                                      const T2&  t2,
                                      const T3&  t3,
                                      const T4&  t4,
                                      const T5&  t5,
                                      const T6&  t6,
                                      const T7&  t7,
                                      const T8&  t8,
                                      const T9&  t9,
                                      const T10& t10) const;
    template <class T0, class T1, class T2, class T3, class T4, class T5,
              class T6, class T7, class T8, class T9, class T10, class T11>
                        bdld::Datum a(const T0&  t0,
                                      const T1&  t1,
                                      const T2&  t2,
                                      const T3&  t3,
                                      const T4&  t4,
                                      const T5&  t5,
                                      const T6&  t6,
                                      const T7&  t7,
                                      const T8&  t8,
                                      const T9&  t9,
                                      const T10& t10,
                                      const T11& t11) const;
    template <class T0, class T1, class T2, class T3, class T4, class T5,
              class T6, class T7, class T8, class T9, class T10, class T11,
              class T12>
                        bdld::Datum a(const T0&  t0,
                                      const T1&  t1,
                                      const T2&  t2,
                                      const T3&  t3,
                                      const T4&  t4,
                                      const T5&  t5,
                                      const T6&  t6,
                                      const T7&  t7,
                                      const T8&  t8,
                                      const T9&  t9,
                                      const T10& t10,
                                      const T11& t11,
                                      const T12& t12) const;
    template <class T0, class T1, class T2, class T3, class T4, class T5,
              class T6, class T7, class T8, class T9, class T10, class T11,
              class T12, class T13>
                        bdld::Datum a(const T0&  t0,
                                      const T1&  t1,
                                      const T2&  t2,
                                      const T3&  t3,
                                      const T4&  t4,
                                      const T5&  t5,
                                      const T6&  t6,
                                      const T7&  t7,
                                      const T8&  t8,
                                      const T9&  t9,
                                      const T10& t10,
                                      const T11& t11,
                                      const T12& t12,
                                      const T13& t13) const;
    template <class T0, class T1, class T2, class T3, class T4, class T5,
              class T6, class T7, class T8, class T9, class T10, class T11,
              class T12, class T13, class T14>
                        bdld::Datum a(const T0&  t0,
                                      const T1&  t1,
                                      const T2&  t2,
                                      const T3&  t3,
                                      const T4&  t4,
                                      const T5&  t5,
                                      const T6&  t6,
                                      const T7&  t7,
                                      const T8&  t8,
                                      const T9&  t9,
                                      const T10& t10,
                                      const T11& t11,
                                      const T12& t12,
                                      const T13& t13,
                                      const T14& t14) const;
    template <class T0, class T1, class T2, class T3, class T4, class T5,
              class T6, class T7, class T8, class T9, class T10, class T11,
              class T12, class T13, class T14, class T15>
                        bdld::Datum a(const T0&  t0,
                                      const T1&  t1,
                                      const T2&  t2,
                                      const T3&  t3,
                                      const T4&  t4,
                                      const T5&  t5,
                                      const T6&  t6,
                                      const T7&  t7,
                                      const T8&  t8,
                                      const T9&  t9,
                                      const T10& t10,
                                      const T11& t11,
                                      const T12& t12,
                                      const T13& t13,
                                      const T14& t14,
                                      const T15& t15) const;
#endif
        // Return a 'bdld::Datum' object containing an array of the specified
        // values.  Note that with C++11 this could be a single method.

    bdld::Datum m() const;
        // Return a 'bdld::Datum' object containing an empty map.

#ifdef BSLS_COMPILERFEATURES_SUPPORT_VARIADIC_TEMPLATES
    template <typename... ENTRIES>
    bdld::Datum m(const ENTRIES&... entries) const;
        // Return a 'bdld::Datum' object containing a map of the specified
        // 'entries'.
#else
    template <class T0>
    bdld::Datum m(const bslstl::StringRef& k0, const T0& v0) const;

    template <class T0, class T1>
    bdld::Datum m(const bslstl::StringRef& k0, const T0& v0,
                  const bslstl::StringRef& k1, const T1& v1) const;

    template <class T0, class T1, class T2>
    bdld::Datum m(const bslstl::StringRef& k0, const T0& v0,
                  const bslstl::StringRef& k1, const T1& v1,
                  const bslstl::StringRef& k2, const T2& v2) const;

    template <class T0, class T1, class T2, class T3>
    bdld::Datum m(const bslstl::StringRef& k0, const T0& v0,
                  const bslstl::StringRef& k1, const T1& v1,
                  const bslstl::StringRef& k2, const T2& v2,
                  const bslstl::StringRef& k3, const T3& v3) const;

    template <class T0, class T1, class T2, class T3, class T4>
    bdld::Datum m(const bslstl::StringRef& k0, const T0& v0,
                  const bslstl::StringRef& k1, const T1& v1,
                  const bslstl::StringRef& k2, const T2& v2,
                  const bslstl::StringRef& k3, const T3& v3,
                  const bslstl::StringRef& k4, const T4& v4) const;

    template <class T0, class T1, class T2, class T3, class T4, class T5>
    bdld::Datum m(const bslstl::StringRef& k0, const T0& v0,
                  const bslstl::StringRef& k1, const T1& v1,
                  const bslstl::StringRef& k2, const T2& v2,
                  const bslstl::StringRef& k3, const T3& v3,
                  const bslstl::StringRef& k4, const T4& v4,
                  const bslstl::StringRef& k5, const T5& v5) const;

    template <class T0, class T1, class T2, class T3, class T4, class T5,
              class T6>
    bdld::Datum m(const bslstl::StringRef& k0, const T0& v0,
                  const bslstl::StringRef& k1, const T1& v1,
                  const bslstl::StringRef& k2, const T2& v2,
                  const bslstl::StringRef& k3, const T3& v3,
                  const bslstl::StringRef& k4, const T4& v4,
                  const bslstl::StringRef& k5, const T5& v5,
                  const bslstl::StringRef& k6, const T6& v6) const;

    template <class T0, class T1, class T2, class T3, class T4, class T5,
              class T6, class T7>
    bdld::Datum m(const bslstl::StringRef& k0, const T0& v0,
                  const bslstl::StringRef& k1, const T1& v1,
                  const bslstl::StringRef& k2, const T2& v2,
                  const bslstl::StringRef& k3, const T3& v3,
                  const bslstl::StringRef& k4, const T4& v4,
                  const bslstl::StringRef& k5, const T5& v5,
                  const bslstl::StringRef& k6, const T6& v6,
                  const bslstl::StringRef& k7, const T7& v7) const;

    template <class T0, class T1, class T2, class T3, class T4, class T5,
              class T6, class T7, class T8>
    bdld::Datum m(const bslstl::StringRef& k0, const T0& v0,
                  const bslstl::StringRef& k1, const T1& v1,
                  const bslstl::StringRef& k2, const T2& v2,
                  const bslstl::StringRef& k3, const T3& v3,
                  const bslstl::StringRef& k4, const T4& v4,
                  const bslstl::StringRef& k5, const T5& v5,
                  const bslstl::StringRef& k6, const T6& v6,
                  const bslstl::StringRef& k7, const T7& v7,
                  const bslstl::StringRef& k8, const T8& v8) const;

    template <class T0, class T1, class T2, class T3, class T4, class T5,
              class T6, class T7, class T8, class T9>
    bdld::Datum m(const bslstl::StringRef& k0, const T0& v0,
                  const bslstl::StringRef& k1, const T1& v1,
                  const bslstl::StringRef& k2, const T2& v2,
                  const bslstl::StringRef& k3, const T3& v3,
                  const bslstl::StringRef& k4, const T4& v4,
                  const bslstl::StringRef& k5, const T5& v5,
                  const bslstl::StringRef& k6, const T6& v6,
                  const bslstl::StringRef& k7, const T7& v7,
                  const bslstl::StringRef& k8, const T8& v8,
                  const bslstl::StringRef& k9, const T9& v9) const;

    template <class T0, class T1, class T2, class T3, class T4, class T5,
              class T6, class T7, class T8, class T9, class T10>
    bdld::Datum m(const bslstl::StringRef& k0,  const T0&  v0,
                  const bslstl::StringRef& k1,  const T1&  v1,
                  const bslstl::StringRef& k2,  const T2&  v2,
                  const bslstl::StringRef& k3,  const T3&  v3,
                  const bslstl::StringRef& k4,  const T4&  v4,
                  const bslstl::StringRef& k5,  const T5&  v5,
                  const bslstl::StringRef& k6,  const T6&  v6,
                  const bslstl::StringRef& k7,  const T7&  v7,
                  const bslstl::StringRef& k8,  const T8&  v8,
                  const bslstl::StringRef& k9,  const T9&  v9,
                  const bslstl::StringRef& k10, const T10& v10) const;

    template <class T0, class T1, class T2, class T3, class T4,  class T5,
              class T6, class T7, class T8, class T9, class T10, class T11>
    bdld::Datum m(const bslstl::StringRef& k0,  const T0&  v0,
                  const bslstl::StringRef& k1,  const T1&  v1,
                  const bslstl::StringRef& k2,  const T2&  v2,
                  const bslstl::StringRef& k3,  const T3&  v3,
                  const bslstl::StringRef& k4,  const T4&  v4,
                  const bslstl::StringRef& k5,  const T5&  v5,
                  const bslstl::StringRef& k6,  const T6&  v6,
                  const bslstl::StringRef& k7,  const T7&  v7,
                  const bslstl::StringRef& k8,  const T8&  v8,
                  const bslstl::StringRef& k9,  const T9&  v9,
                  const bslstl::StringRef& k10, const T10& v10,
                  const bslstl::StringRef& k11, const T11& v11) const;

    template <class T0, class T1, class T2, class T3, class T4,  class T5,
              class T6, class T7, class T8, class T9, class T10, class T11,
              class T12>
    bdld::Datum m(const bslstl::StringRef& k0,  const T0&  v0,
                  const bslstl::StringRef& k1,  const T1&  v1,
                  const bslstl::StringRef& k2,  const T2&  v2,
                  const bslstl::StringRef& k3,  const T3&  v3,
                  const bslstl::StringRef& k4,  const T4&  v4,
                  const bslstl::StringRef& k5,  const T5&  v5,
                  const bslstl::StringRef& k6,  const T6&  v6,
                  const bslstl::StringRef& k7,  const T7&  v7,
                  const bslstl::StringRef& k8,  const T8&  v8,
                  const bslstl::StringRef& k9,  const T9&  v9,
                  const bslstl::StringRef& k10, const T10& v10,
                  const bslstl::StringRef& k11, const T11& v11,
                  const bslstl::StringRef& k12, const T12& v12) const;

    template <class T0,  class T1, class T2, class T3, class T4,  class T5,
              class T6,  class T7, class T8, class T9, class T10, class T11,
              class T12, class T13>
    bdld::Datum m(const bslstl::StringRef& k0,  const T0&  v0,
                  const bslstl::StringRef& k1,  const T1&  v1,
                  const bslstl::StringRef& k2,  const T2&  v2,
                  const bslstl::StringRef& k3,  const T3&  v3,
                  const bslstl::StringRef& k4,  const T4&  v4,
                  const bslstl::StringRef& k5,  const T5&  v5,
                  const bslstl::StringRef& k6,  const T6&  v6,
                  const bslstl::StringRef& k7,  const T7&  v7,
                  const bslstl::StringRef& k8,  const T8&  v8,
                  const bslstl::StringRef& k9,  const T9&  v9,
                  const bslstl::StringRef& k10, const T10& v10,
                  const bslstl::StringRef& k11, const T11& v11,
                  const bslstl::StringRef& k12, const T12& v12,
                  const bslstl::StringRef& k13, const T13& v13) const;

    template <class T0,  class T1,  class T2, class T3, class T4,  class T5,
              class T6,  class T7,  class T8, class T9, class T10, class T11,
              class T12, class T13, class T14>
    bdld::Datum m(const bslstl::StringRef& k0,  const T0&  v0,
                  const bslstl::StringRef& k1,  const T1&  v1,
                  const bslstl::StringRef& k2,  const T2&  v2,
                  const bslstl::StringRef& k3,  const T3&  v3,
                  const bslstl::StringRef& k4,  const T4&  v4,
                  const bslstl::StringRef& k5,  const T5&  v5,
                  const bslstl::StringRef& k6,  const T6&  v6,
                  const bslstl::StringRef& k7,  const T7&  v7,
                  const bslstl::StringRef& k8,  const T8&  v8,
                  const bslstl::StringRef& k9,  const T9&  v9,
                  const bslstl::StringRef& k10, const T10& v10,
                  const bslstl::StringRef& k11, const T11& v11,
                  const bslstl::StringRef& k12, const T12& v12,
                  const bslstl::StringRef& k13, const T13& v13,
                  const bslstl::StringRef& k14, const T14& v14) const;

    template <class T0,  class T1,  class T2, class T3, class T4,  class T5,
              class T6,  class T7,  class T8, class T9, class T10, class T11,
              class T12, class T13, class T14, class T15>
    bdld::Datum m(const bslstl::StringRef& k0,  const T0&  v0,
                  const bslstl::StringRef& k1,  const T1&  v1,
                  const bslstl::StringRef& k2,  const T2&  v2,
                  const bslstl::StringRef& k3,  const T3&  v3,
                  const bslstl::StringRef& k4,  const T4&  v4,
                  const bslstl::StringRef& k5,  const T5&  v5,
                  const bslstl::StringRef& k6,  const T6&  v6,
                  const bslstl::StringRef& k7,  const T7&  v7,
                  const bslstl::StringRef& k8,  const T8&  v8,
                  const bslstl::StringRef& k9,  const T9&  v9,
                  const bslstl::StringRef& k10, const T10& v10,
                  const bslstl::StringRef& k11, const T11& v11,
                  const bslstl::StringRef& k12, const T12& v12,
                  const bslstl::StringRef& k13, const T13& v13,
                  const bslstl::StringRef& k14, const T14& v14,
                  const bslstl::StringRef& k15, const T15& v15) const;
#endif
        // Return a 'bdld::Datum' object containing a map with the up to
        // 16 specified key/value pairs, where the keys are not owned.  The
        // behavior is undefined unless all specified key values are unique.

    bdld::Datum mok() const;
        // Return a 'bdld::Datum' object containing an empty map with owned
        // keys.

#ifdef BSLS_COMPILERFEATURES_SUPPORT_VARIADIC_TEMPLATES
    template <typename... ENTRIES>
    bdld::Datum mok(const ENTRIES&... entries) const;
        // Return a 'bdld::Datum' object containing a map with owned keys
        // consisting of the specified 'entries'.
#else
    template <class T0>
    bdld::Datum mok(const bslstl::StringRef& k0, const T0& v0) const;

    template <class T0, class T1>
    bdld::Datum mok(const bslstl::StringRef& k0, const T0& v0,
                    const bslstl::StringRef& k1, const T1& v1) const;

    template <class T0, class T1, class T2>
    bdld::Datum mok(const bslstl::StringRef& k0, const T0& v0,
                    const bslstl::StringRef& k1, const T1& v1,
                    const bslstl::StringRef& k2, const T2& v2) const;

    template <class T0, class T1, class T2, class T3>
    bdld::Datum mok(const bslstl::StringRef& k0, const T0& v0,
                    const bslstl::StringRef& k1, const T1& v1,
                    const bslstl::StringRef& k2, const T2& v2,
                    const bslstl::StringRef& k3, const T3& v3) const;

    template <class T0, class T1, class T2, class T3, class T4>
    bdld::Datum mok(const bslstl::StringRef& k0, const T0& v0,
                    const bslstl::StringRef& k1, const T1& v1,
                    const bslstl::StringRef& k2, const T2& v2,
                    const bslstl::StringRef& k3, const T3& v3,
                    const bslstl::StringRef& k4, const T4& v4) const;

    template <class T0, class T1, class T2, class T3, class T4, class T5>
    bdld::Datum mok(const bslstl::StringRef& k0, const T0& v0,
                    const bslstl::StringRef& k1, const T1& v1,
                    const bslstl::StringRef& k2, const T2& v2,
                    const bslstl::StringRef& k3, const T3& v3,
                    const bslstl::StringRef& k4, const T4& v4,
                    const bslstl::StringRef& k5, const T5& v5) const;

    template <class T0, class T1, class T2, class T3, class T4, class T5,
              class T6>
    bdld::Datum mok(const bslstl::StringRef& k0, const T0& v0,
                    const bslstl::StringRef& k1, const T1& v1,
                    const bslstl::StringRef& k2, const T2& v2,
                    const bslstl::StringRef& k3, const T3& v3,
                    const bslstl::StringRef& k4, const T4& v4,
                    const bslstl::StringRef& k5, const T5& v5,
                    const bslstl::StringRef& k6, const T6& v6) const;

    template <class T0, class T1, class T2, class T3, class T4, class T5,
              class T6, class T7>
    bdld::Datum mok(const bslstl::StringRef& k0, const T0& v0,
                    const bslstl::StringRef& k1, const T1& v1,
                    const bslstl::StringRef& k2, const T2& v2,
                    const bslstl::StringRef& k3, const T3& v3,
                    const bslstl::StringRef& k4, const T4& v4,
                    const bslstl::StringRef& k5, const T5& v5,
                    const bslstl::StringRef& k6, const T6& v6,
                    const bslstl::StringRef& k7, const T7& v7) const;

    template <class T0, class T1, class T2, class T3, class T4, class T5,
              class T6, class T7, class T8>
    bdld::Datum mok(const bslstl::StringRef& k0, const T0& v0,
                    const bslstl::StringRef& k1, const T1& v1,
                    const bslstl::StringRef& k2, const T2& v2,
                    const bslstl::StringRef& k3, const T3& v3,
                    const bslstl::StringRef& k4, const T4& v4,
                    const bslstl::StringRef& k5, const T5& v5,
                    const bslstl::StringRef& k6, const T6& v6,
                    const bslstl::StringRef& k7, const T7& v7,
                    const bslstl::StringRef& k8, const T8& v8) const;

    template <class T0, class T1, class T2, class T3, class T4, class T5,
              class T6, class T7, class T8, class T9>
    bdld::Datum mok(const bslstl::StringRef& k0, const T0& v0,
                    const bslstl::StringRef& k1, const T1& v1,
                    const bslstl::StringRef& k2, const T2& v2,
                    const bslstl::StringRef& k3, const T3& v3,
                    const bslstl::StringRef& k4, const T4& v4,
                    const bslstl::StringRef& k5, const T5& v5,
                    const bslstl::StringRef& k6, const T6& v6,
                    const bslstl::StringRef& k7, const T7& v7,
                    const bslstl::StringRef& k8, const T8& v8,
                    const bslstl::StringRef& k9, const T9& v9) const;

    template <class T0, class T1, class T2, class T3, class T4, class T5,
              class T6, class T7, class T8, class T9, class T10>
    bdld::Datum mok(const bslstl::StringRef& k0,  const T0&  v0,
                    const bslstl::StringRef& k1,  const T1&  v1,
                    const bslstl::StringRef& k2,  const T2&  v2,
                    const bslstl::StringRef& k3,  const T3&  v3,
                    const bslstl::StringRef& k4,  const T4&  v4,
                    const bslstl::StringRef& k5,  const T5&  v5,
                    const bslstl::StringRef& k6,  const T6&  v6,
                    const bslstl::StringRef& k7,  const T7&  v7,
                    const bslstl::StringRef& k8,  const T8&  v8,
                    const bslstl::StringRef& k9,  const T9&  v9,
                    const bslstl::StringRef& k10, const T10& v10) const;

    template <class T0, class T1, class T2, class T3, class T4,  class T5,
              class T6, class T7, class T8, class T9, class T10, class T11>
    bdld::Datum mok(const bslstl::StringRef& k0,  const T0&  v0,
                    const bslstl::StringRef& k1,  const T1&  v1,
                    const bslstl::StringRef& k2,  const T2&  v2,
                    const bslstl::StringRef& k3,  const T3&  v3,
                    const bslstl::StringRef& k4,  const T4&  v4,
                    const bslstl::StringRef& k5,  const T5&  v5,
                    const bslstl::StringRef& k6,  const T6&  v6,
                    const bslstl::StringRef& k7,  const T7&  v7,
                    const bslstl::StringRef& k8,  const T8&  v8,
                    const bslstl::StringRef& k9,  const T9&  v9,
                    const bslstl::StringRef& k10, const T10& v10,
                    const bslstl::StringRef& k11, const T11& v11) const;

    template <class T0, class T1, class T2, class T3, class T4,  class T5,
              class T6, class T7, class T8, class T9, class T10, class T11,
              class T12>
    bdld::Datum mok(const bslstl::StringRef& k0,  const T0&  v0,
                    const bslstl::StringRef& k1,  const T1&  v1,
                    const bslstl::StringRef& k2,  const T2&  v2,
                    const bslstl::StringRef& k3,  const T3&  v3,
                    const bslstl::StringRef& k4,  const T4&  v4,
                    const bslstl::StringRef& k5,  const T5&  v5,
                    const bslstl::StringRef& k6,  const T6&  v6,
                    const bslstl::StringRef& k7,  const T7&  v7,
                    const bslstl::StringRef& k8,  const T8&  v8,
                    const bslstl::StringRef& k9,  const T9&  v9,
                    const bslstl::StringRef& k10, const T10& v10,
                    const bslstl::StringRef& k11, const T11& v11,
                    const bslstl::StringRef& k12, const T12& v12) const;

    template <class T0,  class T1, class T2, class T3, class T4,  class T5,
              class T6,  class T7, class T8, class T9, class T10, class T11,
              class T12, class T13>
    bdld::Datum mok(const bslstl::StringRef& k0,  const T0&  v0,
                    const bslstl::StringRef& k1,  const T1&  v1,
                    const bslstl::StringRef& k2,  const T2&  v2,
                    const bslstl::StringRef& k3,  const T3&  v3,
                    const bslstl::StringRef& k4,  const T4&  v4,
                    const bslstl::StringRef& k5,  const T5&  v5,
                    const bslstl::StringRef& k6,  const T6&  v6,
                    const bslstl::StringRef& k7,  const T7&  v7,
                    const bslstl::StringRef& k8,  const T8&  v8,
                    const bslstl::StringRef& k9,  const T9&  v9,
                    const bslstl::StringRef& k10, const T10& v10,
                    const bslstl::StringRef& k11, const T11& v11,
                    const bslstl::StringRef& k12, const T12& v12,
                    const bslstl::StringRef& k13, const T13& v13) const;

    template <class T0,  class T1,  class T2, class T3, class T4,  class T5,
              class T6,  class T7,  class T8, class T9, class T10, class T11,
              class T12, class T13, class T14>
    bdld::Datum mok(const bslstl::StringRef& k0,  const T0&  v0,
                    const bslstl::StringRef& k1,  const T1&  v1,
                    const bslstl::StringRef& k2,  const T2&  v2,
                    const bslstl::StringRef& k3,  const T3&  v3,
                    const bslstl::StringRef& k4,  const T4&  v4,
                    const bslstl::StringRef& k5,  const T5&  v5,
                    const bslstl::StringRef& k6,  const T6&  v6,
                    const bslstl::StringRef& k7,  const T7&  v7,
                    const bslstl::StringRef& k8,  const T8&  v8,
                    const bslstl::StringRef& k9,  const T9&  v9,
                    const bslstl::StringRef& k10, const T10& v10,
                    const bslstl::StringRef& k11, const T11& v11,
                    const bslstl::StringRef& k12, const T12& v12,
                    const bslstl::StringRef& k13, const T13& v13,
                    const bslstl::StringRef& k14, const T14& v14) const;

    template <class T0,  class T1,  class T2, class T3, class T4,  class T5,
              class T6,  class T7,  class T8, class T9, class T10, class T11,
              class T12, class T13, class T14, class T15>
    bdld::Datum mok(const bslstl::StringRef& k0,  const T0&  v0,
                    const bslstl::StringRef& k1,  const T1&  v1,
                    const bslstl::StringRef& k2,  const T2&  v2,
                    const bslstl::StringRef& k3,  const T3&  v3,
                    const bslstl::StringRef& k4,  const T4&  v4,
                    const bslstl::StringRef& k5,  const T5&  v5,
                    const bslstl::StringRef& k6,  const T6&  v6,
                    const bslstl::StringRef& k7,  const T7&  v7,
                    const bslstl::StringRef& k8,  const T8&  v8,
                    const bslstl::StringRef& k9,  const T9&  v9,
                    const bslstl::StringRef& k10, const T10& v10,
                    const bslstl::StringRef& k11, const T11& v11,
                    const bslstl::StringRef& k12, const T12& v12,
                    const bslstl::StringRef& k13, const T13& v13,
                    const bslstl::StringRef& k14, const T14& v14,
                    const bslstl::StringRef& k15, const T15& v15) const;
#endif
        // Return a 'bdld::Datum' object containing a map with the up to
        // 16 specified key/value pairs, where the keys are copied and owned by
        // the returned datum.  The behavior is undefined unless all specified
        // key values are unique.
};

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

                              // ----------------
                              // class DatumMaker
                              // ----------------

#ifdef BSLS_COMPILERFEATURES_SUPPORT_VARIADIC_TEMPLATES
// PRIVATE ACCESSORS
template <typename TYPE>
void DatumMaker::pushBackHelper(bdld::DatumArrayBuilder *builder,
                                const TYPE&              element) const
{
    builder->pushBack((*this)(element));
}

template <typename TYPE, typename... ELEMENTS>
void DatumMaker::pushBackHelper(bdld::DatumArrayBuilder *builder,
                                const TYPE&              element,
                                const ELEMENTS&...       elements) const
{
    builder->pushBack((*this)(element));
    pushBackHelper(builder, elements...);
}

template <typename TYPE>
void DatumMaker::pushBackHelper(bdld::DatumMapBuilder    *builder,
                                const bslstl::StringRef&  key,
                                const TYPE&               value) const
{
    builder->pushBack(key, (*this)(value));
}

template <typename TYPE, typename... ENTRIES>
void DatumMaker::pushBackHelper(bdld::DatumMapBuilder    *builder,
                                const bslstl::StringRef&  key,
                                const TYPE&               value,
                                const ENTRIES&...         entries) const
{
    builder->pushBack(key, (*this)(value));
    pushBackHelper(builder, entries...);
}

template <typename TYPE>
void DatumMaker::pushBackHelper(bdld::DatumMapOwningKeysBuilder *builder,
                                const bslstl::StringRef&         key,
                                const TYPE&                      value) const
{
    builder->pushBack(key, (*this)(value));
}

template <typename TYPE, typename... ENTRIES>
void DatumMaker::pushBackHelper(
                               bdld::DatumMapOwningKeysBuilder *builder,
                               const bslstl::StringRef&         key,
                               const TYPE&                      value,
                               const ENTRIES&...                entries) const
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
bdld::Datum DatumMaker::operator()(const bdld::Datum *value,
                                   int                size) const
{
    return (*this)(bdld::DatumArrayRef(value, size));
}

inline
bdld::Datum DatumMaker::operator()(const bdld::DatumMutableMapRef& value) const
{
    return bdld::Datum::adoptMap(value);
}

inline
bdld::Datum DatumMaker::operator()(const bdld::DatumMapEntry *value,
                                   int                        size,
                                   bool                       sorted) const
{
    bdld::DatumMutableMapRef map;
    bdld::Datum::createUninitializedMap(&map, size, d_allocator_p);
    for (int i = 0; i < size; ++i) {
        map.data()[i] = value[i];
    }
    *map.size()   = size;
    *map.sorted() = sorted;
    return bdld::Datum::adoptMap(map);
}

template <class TYPE>
bdld::Datum DatumMaker::operator()(
                                  const bdlb::NullableValue<TYPE>& value) const
{
    return value.isNull() ? (*this)() : (*this)(value.value());
}

inline
bdld::Datum DatumMaker::a() const
{
    return bdld::Datum::createArrayReference(bdld::DatumArrayRef(),
                                             d_allocator_p);
}

#ifdef BSLS_COMPILERFEATURES_SUPPORT_VARIADIC_TEMPLATES
template <typename... ELEMENTS>
bdld::Datum DatumMaker::a(const ELEMENTS&... elements) const
{
    const int numElements = sizeof...(elements);
    bdld::DatumArrayBuilder builder(numElements, d_allocator_p);
    pushBackHelper(&builder, elements...);
    return builder.commit();
}

#else
template <class T0> inline
bdld::Datum DatumMaker::a(const T0& t0) const
{
    bdld::DatumArrayBuilder builder(1, d_allocator_p);
    builder.pushBack((*this)(t0));
    return builder.commit();
}

template <class T0, class T1> inline
bdld::Datum DatumMaker::a(const T0& t0,
                          const T1& t1) const
{
    bdld::DatumArrayBuilder builder(2, d_allocator_p);
    builder.pushBack((*this)(t0));
    builder.pushBack((*this)(t1));
    return builder.commit();
}

template <class T0, class T1, class T2> inline
bdld::Datum DatumMaker::a(const T0& t0,
                          const T1& t1,
                          const T2& t2) const
{
    bdld::DatumArrayBuilder builder(3, d_allocator_p);
    builder.pushBack((*this)(t0));
    builder.pushBack((*this)(t1));
    builder.pushBack((*this)(t2));
    return builder.commit();
}

template <class T0, class T1, class T2, class T3> inline
bdld::Datum DatumMaker::a(const T0& t0,
                          const T1& t1,
                          const T2& t2,
                          const T3& t3) const
{
    bdld::DatumArrayBuilder builder(4, d_allocator_p);
    builder.pushBack((*this)(t0));
    builder.pushBack((*this)(t1));
    builder.pushBack((*this)(t2));
    builder.pushBack((*this)(t3));
    return builder.commit();
}

template <class T0, class T1, class T2, class T3, class T4> inline
bdld::Datum DatumMaker::a(const T0& t0,
                          const T1& t1,
                          const T2& t2,
                          const T3& t3,
                          const T4& t4) const
{
    bdld::DatumArrayBuilder builder(5, d_allocator_p);
    builder.pushBack((*this)(t0));
    builder.pushBack((*this)(t1));
    builder.pushBack((*this)(t2));
    builder.pushBack((*this)(t3));
    builder.pushBack((*this)(t4));
    return builder.commit();
}

template <class T0, class T1, class T2, class T3, class T4, class T5> inline
bdld::Datum DatumMaker::a(const T0& t0,
                          const T1& t1,
                          const T2& t2,
                          const T3& t3,
                          const T4& t4,
                          const T5& t5) const
{
    bdld::DatumArrayBuilder builder(6, d_allocator_p);
    builder.pushBack((*this)(t0));
    builder.pushBack((*this)(t1));
    builder.pushBack((*this)(t2));
    builder.pushBack((*this)(t3));
    builder.pushBack((*this)(t4));
    builder.pushBack((*this)(t5));
    return builder.commit();
}

template <class T0, class T1, class T2, class T3, class T4, class T5, class T6>
inline
bdld::Datum DatumMaker::a(const T0& t0,
                          const T1& t1,
                          const T2& t2,
                          const T3& t3,
                          const T4& t4,
                          const T5& t5,
                          const T6& t6) const
{
    bdld::DatumArrayBuilder builder(7, d_allocator_p);
    builder.pushBack((*this)(t0));
    builder.pushBack((*this)(t1));
    builder.pushBack((*this)(t2));
    builder.pushBack((*this)(t3));
    builder.pushBack((*this)(t4));
    builder.pushBack((*this)(t5));
    builder.pushBack((*this)(t6));
    return builder.commit();
}

template <class T0, class T1, class T2, class T3, class T4, class T5, class T6,
          class T7>
inline
bdld::Datum DatumMaker::a(const T0& t0,
                          const T1& t1,
                          const T2& t2,
                          const T3& t3,
                          const T4& t4,
                          const T5& t5,
                          const T6& t6,
                          const T7& t7) const
{
    bdld::DatumArrayBuilder builder(8, d_allocator_p);
    builder.pushBack((*this)(t0));
    builder.pushBack((*this)(t1));
    builder.pushBack((*this)(t2));
    builder.pushBack((*this)(t3));
    builder.pushBack((*this)(t4));
    builder.pushBack((*this)(t5));
    builder.pushBack((*this)(t6));
    builder.pushBack((*this)(t7));
    return builder.commit();
}

template <class T0, class T1, class T2, class T3, class T4, class T5, class T6,
          class T7, class T8>
inline
bdld::Datum DatumMaker::a(const T0& t0,
                          const T1& t1,
                          const T2& t2,
                          const T3& t3,
                          const T4& t4,
                          const T5& t5,
                          const T6& t6,
                          const T7& t7,
                          const T8& t8) const
{
    bdld::DatumArrayBuilder builder(9, d_allocator_p);
    builder.pushBack((*this)(t0));
    builder.pushBack((*this)(t1));
    builder.pushBack((*this)(t2));
    builder.pushBack((*this)(t3));
    builder.pushBack((*this)(t4));
    builder.pushBack((*this)(t5));
    builder.pushBack((*this)(t6));
    builder.pushBack((*this)(t7));
    builder.pushBack((*this)(t8));
    return builder.commit();
}

template <class T0, class T1, class T2, class T3, class T4, class T5, class T6,
          class T7, class T8, class T9>
inline
bdld::Datum DatumMaker::a(const T0& t0,
                          const T1& t1,
                          const T2& t2,
                          const T3& t3,
                          const T4& t4,
                          const T5& t5,
                          const T6& t6,
                          const T7& t7,
                          const T8& t8,
                          const T9& t9) const
{
    bdld::DatumArrayBuilder builder(10, d_allocator_p);
    builder.pushBack((*this)(t0));
    builder.pushBack((*this)(t1));
    builder.pushBack((*this)(t2));
    builder.pushBack((*this)(t3));
    builder.pushBack((*this)(t4));
    builder.pushBack((*this)(t5));
    builder.pushBack((*this)(t6));
    builder.pushBack((*this)(t7));
    builder.pushBack((*this)(t8));
    builder.pushBack((*this)(t9));
    return builder.commit();
}

template <class T0, class T1, class T2, class T3, class T4, class T5, class T6,
          class T7, class T8, class T9, class T10>
inline
bdld::Datum DatumMaker::a(const T0&  t0,
                          const T1&  t1,
                          const T2&  t2,
                          const T3&  t3,
                          const T4&  t4,
                          const T5&  t5,
                          const T6&  t6,
                          const T7&  t7,
                          const T8&  t8,
                          const T9&  t9,
                          const T10& t10) const
{
    bdld::DatumArrayBuilder builder(11, d_allocator_p);
    builder.pushBack((*this)(t0));
    builder.pushBack((*this)(t1));
    builder.pushBack((*this)(t2));
    builder.pushBack((*this)(t3));
    builder.pushBack((*this)(t4));
    builder.pushBack((*this)(t5));
    builder.pushBack((*this)(t6));
    builder.pushBack((*this)(t7));
    builder.pushBack((*this)(t8));
    builder.pushBack((*this)(t9));
    builder.pushBack((*this)(t10));
    return builder.commit();
}

template <class T0, class T1, class T2, class T3, class T4, class T5, class T6,
          class T7, class T8, class T9, class T10, class T11>
inline
bdld::Datum DatumMaker::a(const T0&  t0,
                          const T1&  t1,
                          const T2&  t2,
                          const T3&  t3,
                          const T4&  t4,
                          const T5&  t5,
                          const T6&  t6,
                          const T7&  t7,
                          const T8&  t8,
                          const T9&  t9,
                          const T10& t10,
                          const T11& t11) const
{
    bdld::DatumArrayBuilder builder(12, d_allocator_p);
    builder.pushBack((*this)(t0));
    builder.pushBack((*this)(t1));
    builder.pushBack((*this)(t2));
    builder.pushBack((*this)(t3));
    builder.pushBack((*this)(t4));
    builder.pushBack((*this)(t5));
    builder.pushBack((*this)(t6));
    builder.pushBack((*this)(t7));
    builder.pushBack((*this)(t8));
    builder.pushBack((*this)(t9));
    builder.pushBack((*this)(t10));
    builder.pushBack((*this)(t11));
    return builder.commit();
}

template <class T0, class T1, class T2, class T3, class T4, class T5, class T6,
          class T7, class T8, class T9, class T10, class T11, class T12>
inline
bdld::Datum DatumMaker::a(const T0&  t0,
                          const T1&  t1,
                          const T2&  t2,
                          const T3&  t3,
                          const T4&  t4,
                          const T5&  t5,
                          const T6&  t6,
                          const T7&  t7,
                          const T8&  t8,
                          const T9&  t9,
                          const T10& t10,
                          const T11& t11,
                          const T12& t12) const
{
    bdld::DatumArrayBuilder builder(13, d_allocator_p);
    builder.pushBack((*this)(t0));
    builder.pushBack((*this)(t1));
    builder.pushBack((*this)(t2));
    builder.pushBack((*this)(t3));
    builder.pushBack((*this)(t4));
    builder.pushBack((*this)(t5));
    builder.pushBack((*this)(t6));
    builder.pushBack((*this)(t7));
    builder.pushBack((*this)(t8));
    builder.pushBack((*this)(t9));
    builder.pushBack((*this)(t10));
    builder.pushBack((*this)(t11));
    builder.pushBack((*this)(t12));
    return builder.commit();
}

template <class T0, class T1, class T2, class T3, class T4, class T5, class T6,
          class T7, class T8, class T9, class T10, class T11, class T12,
          class T13>
inline
bdld::Datum DatumMaker::a(const T0&  t0,
                          const T1&  t1,
                          const T2&  t2,
                          const T3&  t3,
                          const T4&  t4,
                          const T5&  t5,
                          const T6&  t6,
                          const T7&  t7,
                          const T8&  t8,
                          const T9&  t9,
                          const T10& t10,
                          const T11& t11,
                          const T12& t12,
                          const T13& t13) const
{
    bdld::DatumArrayBuilder builder(14, d_allocator_p);
    builder.pushBack((*this)(t0));
    builder.pushBack((*this)(t1));
    builder.pushBack((*this)(t2));
    builder.pushBack((*this)(t3));
    builder.pushBack((*this)(t4));
    builder.pushBack((*this)(t5));
    builder.pushBack((*this)(t6));
    builder.pushBack((*this)(t7));
    builder.pushBack((*this)(t8));
    builder.pushBack((*this)(t9));
    builder.pushBack((*this)(t10));
    builder.pushBack((*this)(t11));
    builder.pushBack((*this)(t12));
    builder.pushBack((*this)(t13));
    return builder.commit();
}

template <class T0, class T1, class T2, class T3, class T4, class T5, class T6,
          class T7, class T8, class T9, class T10, class T11, class T12,
          class T13, class T14>
inline
bdld::Datum DatumMaker::a(const T0&  t0,
                          const T1&  t1,
                          const T2&  t2,
                          const T3&  t3,
                          const T4&  t4,
                          const T5&  t5,
                          const T6&  t6,
                          const T7&  t7,
                          const T8&  t8,
                          const T9&  t9,
                          const T10& t10,
                          const T11& t11,
                          const T12& t12,
                          const T13& t13,
                          const T14& t14) const
{
    bdld::DatumArrayBuilder builder(15, d_allocator_p);
    builder.pushBack((*this)(t0));
    builder.pushBack((*this)(t1));
    builder.pushBack((*this)(t2));
    builder.pushBack((*this)(t3));
    builder.pushBack((*this)(t4));
    builder.pushBack((*this)(t5));
    builder.pushBack((*this)(t6));
    builder.pushBack((*this)(t7));
    builder.pushBack((*this)(t8));
    builder.pushBack((*this)(t9));
    builder.pushBack((*this)(t10));
    builder.pushBack((*this)(t11));
    builder.pushBack((*this)(t12));
    builder.pushBack((*this)(t13));
    builder.pushBack((*this)(t14));
    return builder.commit();
}

template <class T0, class T1, class T2, class T3, class T4, class T5, class T6,
          class T7, class T8, class T9, class T10, class T11, class T12,
          class T13, class T14, class T15>
inline
bdld::Datum DatumMaker::a(const T0&  t0,
                          const T1&  t1,
                          const T2&  t2,
                          const T3&  t3,
                          const T4&  t4,
                          const T5&  t5,
                          const T6&  t6,
                          const T7&  t7,
                          const T8&  t8,
                          const T9&  t9,
                          const T10& t10,
                          const T11& t11,
                          const T12& t12,
                          const T13& t13,
                          const T14& t14,
                          const T15& t15) const
{
    bdld::DatumArrayBuilder builder(16, d_allocator_p);
    builder.pushBack((*this)(t0));
    builder.pushBack((*this)(t1));
    builder.pushBack((*this)(t2));
    builder.pushBack((*this)(t3));
    builder.pushBack((*this)(t4));
    builder.pushBack((*this)(t5));
    builder.pushBack((*this)(t6));
    builder.pushBack((*this)(t7));
    builder.pushBack((*this)(t8));
    builder.pushBack((*this)(t9));
    builder.pushBack((*this)(t10));
    builder.pushBack((*this)(t11));
    builder.pushBack((*this)(t12));
    builder.pushBack((*this)(t13));
    builder.pushBack((*this)(t14));
    builder.pushBack((*this)(t15));
    return builder.commit();
}
#endif

inline
bdld::Datum DatumMaker::m() const
{
    return (*this)(bdld::DatumMutableMapRef());
}

#ifdef BSLS_COMPILERFEATURES_SUPPORT_VARIADIC_TEMPLATES
template <typename... ENTRIES>
bdld::Datum DatumMaker::m(const ENTRIES&... entries) const
{
    const int numArguments = sizeof...(entries);

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
template <class T0>
bdld::Datum DatumMaker::m(const bslstl::StringRef& k0, const T0& v0) const
{
    bdld::DatumMapBuilder builder(1, d_allocator_p);
    builder.pushBack(k0, (*this)(v0));
    return builder.commit();
}

template <class T0,  class T1>
bdld::Datum DatumMaker::m(const bslstl::StringRef& k0, const T0& v0,
                          const bslstl::StringRef& k1, const T1& v1) const
{
    bdld::DatumMapBuilder builder(2, d_allocator_p);
    builder.pushBack(k0, (*this)(v0));
    builder.pushBack(k1, (*this)(v1));
    return builder.commit();
}

template <class T0,  class T1, class T2>
bdld::Datum DatumMaker::m(const bslstl::StringRef& k0, const T0& v0,
                          const bslstl::StringRef& k1, const T1& v1,
                          const bslstl::StringRef& k2, const T2& v2) const
{
    bdld::DatumMapBuilder builder(3, d_allocator_p);
    builder.pushBack(k0, (*this)(v0));
    builder.pushBack(k1, (*this)(v1));
    builder.pushBack(k2, (*this)(v2));
    return builder.commit();
}

template <class T0,  class T1, class T2, class T3>
bdld::Datum DatumMaker::m(const bslstl::StringRef& k0, const T0& v0,
                          const bslstl::StringRef& k1, const T1& v1,
                          const bslstl::StringRef& k2, const T2& v2,
                          const bslstl::StringRef& k3, const T3& v3) const
{
    bdld::DatumMapBuilder builder(4, d_allocator_p);
    builder.pushBack(k0, (*this)(v0));
    builder.pushBack(k1, (*this)(v1));
    builder.pushBack(k2, (*this)(v2));
    builder.pushBack(k3, (*this)(v3));
    return builder.commit();
}

template <class T0,  class T1, class T2, class T3, class T4>
bdld::Datum DatumMaker::m(const bslstl::StringRef& k0, const T0& v0,
                          const bslstl::StringRef& k1, const T1& v1,
                          const bslstl::StringRef& k2, const T2& v2,
                          const bslstl::StringRef& k3, const T3& v3,
                          const bslstl::StringRef& k4, const T4& v4) const
{
    bdld::DatumMapBuilder builder(5, d_allocator_p);
    builder.pushBack(k0, (*this)(v0));
    builder.pushBack(k1, (*this)(v1));
    builder.pushBack(k2, (*this)(v2));
    builder.pushBack(k3, (*this)(v3));
    builder.pushBack(k4, (*this)(v4));
    return builder.commit();
}

template <class T0,  class T1, class T2, class T3, class T4,  class T5>
bdld::Datum DatumMaker::m(const bslstl::StringRef& k0, const T0& v0,
                          const bslstl::StringRef& k1, const T1& v1,
                          const bslstl::StringRef& k2, const T2& v2,
                          const bslstl::StringRef& k3, const T3& v3,
                          const bslstl::StringRef& k4, const T4& v4,
                          const bslstl::StringRef& k5, const T5& v5) const
{
    bdld::DatumMapBuilder builder(6, d_allocator_p);
    builder.pushBack(k0, (*this)(v0));
    builder.pushBack(k1, (*this)(v1));
    builder.pushBack(k2, (*this)(v2));
    builder.pushBack(k3, (*this)(v3));
    builder.pushBack(k4, (*this)(v4));
    builder.pushBack(k5, (*this)(v5));
    return builder.commit();
}

template <class T0,  class T1, class T2, class T3, class T4,  class T5,
          class T6>
bdld::Datum DatumMaker::m(const bslstl::StringRef& k0, const T0& v0,
                          const bslstl::StringRef& k1, const T1& v1,
                          const bslstl::StringRef& k2, const T2& v2,
                          const bslstl::StringRef& k3, const T3& v3,
                          const bslstl::StringRef& k4, const T4& v4,
                          const bslstl::StringRef& k5, const T5& v5,
                          const bslstl::StringRef& k6, const T6& v6) const
{
    bdld::DatumMapBuilder builder(7, d_allocator_p);
    builder.pushBack(k0, (*this)(v0));
    builder.pushBack(k1, (*this)(v1));
    builder.pushBack(k2, (*this)(v2));
    builder.pushBack(k3, (*this)(v3));
    builder.pushBack(k4, (*this)(v4));
    builder.pushBack(k5, (*this)(v5));
    builder.pushBack(k6, (*this)(v6));
    return builder.commit();
}

template <class T0,  class T1, class T2, class T3, class T4,  class T5,
          class T6,  class T7>
bdld::Datum DatumMaker::m(const bslstl::StringRef& k0, const T0& v0,
                          const bslstl::StringRef& k1, const T1& v1,
                          const bslstl::StringRef& k2, const T2& v2,
                          const bslstl::StringRef& k3, const T3& v3,
                          const bslstl::StringRef& k4, const T4& v4,
                          const bslstl::StringRef& k5, const T5& v5,
                          const bslstl::StringRef& k6, const T6& v6,
                          const bslstl::StringRef& k7, const T7& v7) const
{
    bdld::DatumMapBuilder builder(8, d_allocator_p);
    builder.pushBack(k0, (*this)(v0));
    builder.pushBack(k1, (*this)(v1));
    builder.pushBack(k2, (*this)(v2));
    builder.pushBack(k3, (*this)(v3));
    builder.pushBack(k4, (*this)(v4));
    builder.pushBack(k5, (*this)(v5));
    builder.pushBack(k6, (*this)(v6));
    builder.pushBack(k7, (*this)(v7));
    return builder.commit();
}

template <class T0,  class T1, class T2, class T3, class T4,  class T5,
          class T6,  class T7, class T8>
bdld::Datum DatumMaker::m(const bslstl::StringRef& k0, const T0& v0,
                          const bslstl::StringRef& k1, const T1& v1,
                          const bslstl::StringRef& k2, const T2& v2,
                          const bslstl::StringRef& k3, const T3& v3,
                          const bslstl::StringRef& k4, const T4& v4,
                          const bslstl::StringRef& k5, const T5& v5,
                          const bslstl::StringRef& k6, const T6& v6,
                          const bslstl::StringRef& k7, const T7& v7,
                          const bslstl::StringRef& k8, const T8& v8) const
{
    bdld::DatumMapBuilder builder(9, d_allocator_p);
    builder.pushBack(k0, (*this)(v0));
    builder.pushBack(k1, (*this)(v1));
    builder.pushBack(k2, (*this)(v2));
    builder.pushBack(k3, (*this)(v3));
    builder.pushBack(k4, (*this)(v4));
    builder.pushBack(k5, (*this)(v5));
    builder.pushBack(k6, (*this)(v6));
    builder.pushBack(k7, (*this)(v7));
    builder.pushBack(k8, (*this)(v8));
    return builder.commit();
}

template <class T0,  class T1, class T2, class T3, class T4,  class T5,
          class T6,  class T7, class T8, class T9>
bdld::Datum DatumMaker::m(const bslstl::StringRef& k0, const T0& v0,
                          const bslstl::StringRef& k1, const T1& v1,
                          const bslstl::StringRef& k2, const T2& v2,
                          const bslstl::StringRef& k3, const T3& v3,
                          const bslstl::StringRef& k4, const T4& v4,
                          const bslstl::StringRef& k5, const T5& v5,
                          const bslstl::StringRef& k6, const T6& v6,
                          const bslstl::StringRef& k7, const T7& v7,
                          const bslstl::StringRef& k8, const T8& v8,
                          const bslstl::StringRef& k9, const T9& v9) const
{
    bdld::DatumMapBuilder builder(10, d_allocator_p);
    builder.pushBack(k0, (*this)(v0));
    builder.pushBack(k1, (*this)(v1));
    builder.pushBack(k2, (*this)(v2));
    builder.pushBack(k3, (*this)(v3));
    builder.pushBack(k4, (*this)(v4));
    builder.pushBack(k5, (*this)(v5));
    builder.pushBack(k6, (*this)(v6));
    builder.pushBack(k7, (*this)(v7));
    builder.pushBack(k8, (*this)(v8));
    builder.pushBack(k9, (*this)(v9));
    return builder.commit();
}

template <class T0,  class T1, class T2, class T3, class T4,  class T5,
          class T6,  class T7, class T8, class T9, class T10>
bdld::Datum DatumMaker::m(const bslstl::StringRef& k0,  const T0&  v0,
                          const bslstl::StringRef& k1,  const T1&  v1,
                          const bslstl::StringRef& k2,  const T2&  v2,
                          const bslstl::StringRef& k3,  const T3&  v3,
                          const bslstl::StringRef& k4,  const T4&  v4,
                          const bslstl::StringRef& k5,  const T5&  v5,
                          const bslstl::StringRef& k6,  const T6&  v6,
                          const bslstl::StringRef& k7,  const T7&  v7,
                          const bslstl::StringRef& k8,  const T8&  v8,
                          const bslstl::StringRef& k9,  const T9&  v9,
                          const bslstl::StringRef& k10, const T10& v10) const
{
    bdld::DatumMapBuilder builder(11, d_allocator_p);
    builder.pushBack(k0,  (*this)(v0));
    builder.pushBack(k1,  (*this)(v1));
    builder.pushBack(k2,  (*this)(v2));
    builder.pushBack(k3,  (*this)(v3));
    builder.pushBack(k4,  (*this)(v4));
    builder.pushBack(k5,  (*this)(v5));
    builder.pushBack(k6,  (*this)(v6));
    builder.pushBack(k7,  (*this)(v7));
    builder.pushBack(k8,  (*this)(v8));
    builder.pushBack(k9,  (*this)(v9));
    builder.pushBack(k10, (*this)(v10));
    return builder.commit();
}

template <class T0,  class T1, class T2, class T3, class T4,  class T5,
          class T6,  class T7, class T8, class T9, class T10, class T11>
bdld::Datum DatumMaker::m(const bslstl::StringRef&  k0, const T0&  v0,
                          const bslstl::StringRef&  k1, const T1&  v1,
                          const bslstl::StringRef&  k2, const T2&  v2,
                          const bslstl::StringRef&  k3, const T3&  v3,
                          const bslstl::StringRef&  k4, const T4&  v4,
                          const bslstl::StringRef&  k5, const T5&  v5,
                          const bslstl::StringRef&  k6, const T6&  v6,
                          const bslstl::StringRef&  k7, const T7&  v7,
                          const bslstl::StringRef&  k8, const T8&  v8,
                          const bslstl::StringRef&  k9, const T9&  v9,
                          const bslstl::StringRef& k10, const T10& v10,
                          const bslstl::StringRef& k11, const T11& v11) const
{
    bdld::DatumMapBuilder builder(12, d_allocator_p);
    builder.pushBack(k0,  (*this)(v0));
    builder.pushBack(k1,  (*this)(v1));
    builder.pushBack(k2,  (*this)(v2));
    builder.pushBack(k3,  (*this)(v3));
    builder.pushBack(k4,  (*this)(v4));
    builder.pushBack(k5,  (*this)(v5));
    builder.pushBack(k6,  (*this)(v6));
    builder.pushBack(k7,  (*this)(v7));
    builder.pushBack(k8,  (*this)(v8));
    builder.pushBack(k9,  (*this)(v9));
    builder.pushBack(k10, (*this)(v10));
    builder.pushBack(k11, (*this)(v11));
    return builder.commit();
}

template <class T0,  class T1, class T2, class T3, class T4,  class T5,
          class T6,  class T7, class T8, class T9, class T10, class T11,
          class T12>
bdld::Datum DatumMaker::m(const bslstl::StringRef& k0,  const T0&  v0,
                          const bslstl::StringRef& k1,  const T1&  v1,
                          const bslstl::StringRef& k2,  const T2&  v2,
                          const bslstl::StringRef& k3,  const T3&  v3,
                          const bslstl::StringRef& k4,  const T4&  v4,
                          const bslstl::StringRef& k5,  const T5&  v5,
                          const bslstl::StringRef& k6,  const T6&  v6,
                          const bslstl::StringRef& k7,  const T7&  v7,
                          const bslstl::StringRef& k8,  const T8&  v8,
                          const bslstl::StringRef& k9,  const T9&  v9,
                          const bslstl::StringRef& k10, const T10& v10,
                          const bslstl::StringRef& k11, const T11& v11,
                          const bslstl::StringRef& k12, const T12& v12) const
{
    bdld::DatumMapBuilder builder(13, d_allocator_p);
    builder.pushBack(k0,  (*this)(v0));
    builder.pushBack(k1,  (*this)(v1));
    builder.pushBack(k2,  (*this)(v2));
    builder.pushBack(k3,  (*this)(v3));
    builder.pushBack(k4,  (*this)(v4));
    builder.pushBack(k5,  (*this)(v5));
    builder.pushBack(k6,  (*this)(v6));
    builder.pushBack(k7,  (*this)(v7));
    builder.pushBack(k8,  (*this)(v8));
    builder.pushBack(k9,  (*this)(v9));
    builder.pushBack(k10, (*this)(v10));
    builder.pushBack(k11, (*this)(v11));
    builder.pushBack(k12, (*this)(v12));
    return builder.commit();
}

template <class T0,  class T1, class T2, class T3, class T4,  class T5,
          class T6,  class T7, class T8, class T9, class T10, class T11,
          class T12, class T13>
bdld::Datum DatumMaker::m(const bslstl::StringRef& k0,  const T0&  v0,
                          const bslstl::StringRef& k1,  const T1&  v1,
                          const bslstl::StringRef& k2,  const T2&  v2,
                          const bslstl::StringRef& k3,  const T3&  v3,
                          const bslstl::StringRef& k4,  const T4&  v4,
                          const bslstl::StringRef& k5,  const T5&  v5,
                          const bslstl::StringRef& k6,  const T6&  v6,
                          const bslstl::StringRef& k7,  const T7&  v7,
                          const bslstl::StringRef& k8,  const T8&  v8,
                          const bslstl::StringRef& k9,  const T9&  v9,
                          const bslstl::StringRef& k10, const T10& v10,
                          const bslstl::StringRef& k11, const T11& v11,
                          const bslstl::StringRef& k12, const T12& v12,
                          const bslstl::StringRef& k13, const T13& v13) const
{
    bdld::DatumMapBuilder builder(14, d_allocator_p);
    builder.pushBack(k0,  (*this)(v0));
    builder.pushBack(k1,  (*this)(v1));
    builder.pushBack(k2,  (*this)(v2));
    builder.pushBack(k3,  (*this)(v3));
    builder.pushBack(k4,  (*this)(v4));
    builder.pushBack(k5,  (*this)(v5));
    builder.pushBack(k6,  (*this)(v6));
    builder.pushBack(k7,  (*this)(v7));
    builder.pushBack(k8,  (*this)(v8));
    builder.pushBack(k9,  (*this)(v9));
    builder.pushBack(k10, (*this)(v10));
    builder.pushBack(k11, (*this)(v11));
    builder.pushBack(k12, (*this)(v12));
    builder.pushBack(k13, (*this)(v13));
    return builder.commit();
}

template <class T0,  class T1, class T2, class T3, class T4,  class T5,
          class T6,  class T7, class T8, class T9, class T10, class T11,
          class T12, class T13, class T14>
bdld::Datum DatumMaker::m(const bslstl::StringRef& k0,  const T0&  v0,
                          const bslstl::StringRef& k1,  const T1&  v1,
                          const bslstl::StringRef& k2,  const T2&  v2,
                          const bslstl::StringRef& k3,  const T3&  v3,
                          const bslstl::StringRef& k4,  const T4&  v4,
                          const bslstl::StringRef& k5,  const T5&  v5,
                          const bslstl::StringRef& k6,  const T6&  v6,
                          const bslstl::StringRef& k7,  const T7&  v7,
                          const bslstl::StringRef& k8,  const T8&  v8,
                          const bslstl::StringRef& k9,  const T9&  v9,
                          const bslstl::StringRef& k10, const T10& v10,
                          const bslstl::StringRef& k11, const T11& v11,
                          const bslstl::StringRef& k12, const T12& v12,
                          const bslstl::StringRef& k13, const T13& v13,
                          const bslstl::StringRef& k14, const T14& v14) const
{
    bdld::DatumMapBuilder builder(15, d_allocator_p);
    builder.pushBack(k0,  (*this)(v0));
    builder.pushBack(k1,  (*this)(v1));
    builder.pushBack(k2,  (*this)(v2));
    builder.pushBack(k3,  (*this)(v3));
    builder.pushBack(k4,  (*this)(v4));
    builder.pushBack(k5,  (*this)(v5));
    builder.pushBack(k6,  (*this)(v6));
    builder.pushBack(k7,  (*this)(v7));
    builder.pushBack(k8,  (*this)(v8));
    builder.pushBack(k9,  (*this)(v9));
    builder.pushBack(k10, (*this)(v10));
    builder.pushBack(k11, (*this)(v11));
    builder.pushBack(k12, (*this)(v12));
    builder.pushBack(k13, (*this)(v13));
    builder.pushBack(k14, (*this)(v14));
    return builder.commit();
}

template <class T0,  class T1, class T2, class T3, class T4,  class T5,
          class T6,  class T7, class T8, class T9, class T10, class T11,
          class T12, class T13, class T14, class T15>
bdld::Datum DatumMaker::m(const bslstl::StringRef& k0,  const T0&  v0,
                          const bslstl::StringRef& k1,  const T1&  v1,
                          const bslstl::StringRef& k2,  const T2&  v2,
                          const bslstl::StringRef& k3,  const T3&  v3,
                          const bslstl::StringRef& k4,  const T4&  v4,
                          const bslstl::StringRef& k5,  const T5&  v5,
                          const bslstl::StringRef& k6,  const T6&  v6,
                          const bslstl::StringRef& k7,  const T7&  v7,
                          const bslstl::StringRef& k8,  const T8&  v8,
                          const bslstl::StringRef& k9,  const T9&  v9,
                          const bslstl::StringRef& k10, const T10& v10,
                          const bslstl::StringRef& k11, const T11& v11,
                          const bslstl::StringRef& k12, const T12& v12,
                          const bslstl::StringRef& k13, const T13& v13,
                          const bslstl::StringRef& k14, const T14& v14,
                          const bslstl::StringRef& k15, const T15& v15) const
{
    bdld::DatumMapBuilder builder(16, d_allocator_p);
    builder.pushBack(k0,  (*this)(v0));
    builder.pushBack(k1,  (*this)(v1));
    builder.pushBack(k2,  (*this)(v2));
    builder.pushBack(k3,  (*this)(v3));
    builder.pushBack(k4,  (*this)(v4));
    builder.pushBack(k5,  (*this)(v5));
    builder.pushBack(k6,  (*this)(v6));
    builder.pushBack(k7,  (*this)(v7));
    builder.pushBack(k8,  (*this)(v8));
    builder.pushBack(k9,  (*this)(v9));
    builder.pushBack(k10, (*this)(v10));
    builder.pushBack(k11, (*this)(v11));
    builder.pushBack(k12, (*this)(v12));
    builder.pushBack(k13, (*this)(v13));
    builder.pushBack(k14, (*this)(v14));
    builder.pushBack(k15, (*this)(v15));
    return builder.commit();
}
#endif

inline
bdld::Datum DatumMaker::mok() const
{
    return (*this)(bdld::DatumMutableMapRef());
}

#ifdef BSLS_COMPILERFEATURES_SUPPORT_VARIADIC_TEMPLATES
template <typename... ENTRIES>
bdld::Datum DatumMaker::mok(const ENTRIES&... entries) const
{
    const int numArguments = sizeof...(entries);

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
template <class T0>
bdld::Datum DatumMaker::mok(const bslstl::StringRef& k0, const T0& v0) const
{
    bdld::DatumMapOwningKeysBuilder builder(d_allocator_p);
    builder.pushBack(k0, (*this)(v0));
    return builder.commit();
}

template <class T0,  class T1>
bdld::Datum DatumMaker::mok(const bslstl::StringRef& k0, const T0& v0,
                            const bslstl::StringRef& k1, const T1& v1) const
{
    bdld::DatumMapOwningKeysBuilder builder(d_allocator_p);
    builder.pushBack(k0, (*this)(v0));
    builder.pushBack(k1, (*this)(v1));
    return builder.commit();
}

template <class T0,  class T1, class T2>
bdld::Datum DatumMaker::mok(const bslstl::StringRef& k0, const T0& v0,
                            const bslstl::StringRef& k1, const T1& v1,
                            const bslstl::StringRef& k2, const T2& v2) const
{
    bdld::DatumMapOwningKeysBuilder builder(d_allocator_p);
    builder.pushBack(k0, (*this)(v0));
    builder.pushBack(k1, (*this)(v1));
    builder.pushBack(k2, (*this)(v2));
    return builder.commit();
}

template <class T0,  class T1, class T2, class T3>
bdld::Datum DatumMaker::mok(const bslstl::StringRef& k0, const T0& v0,
                            const bslstl::StringRef& k1, const T1& v1,
                            const bslstl::StringRef& k2, const T2& v2,
                            const bslstl::StringRef& k3, const T3& v3) const
{
    bdld::DatumMapOwningKeysBuilder builder(d_allocator_p);
    builder.pushBack(k0, (*this)(v0));
    builder.pushBack(k1, (*this)(v1));
    builder.pushBack(k2, (*this)(v2));
    builder.pushBack(k3, (*this)(v3));
    return builder.commit();
}

template <class T0,  class T1, class T2, class T3, class T4>
bdld::Datum DatumMaker::mok(const bslstl::StringRef& k0, const T0& v0,
                            const bslstl::StringRef& k1, const T1& v1,
                            const bslstl::StringRef& k2, const T2& v2,
                            const bslstl::StringRef& k3, const T3& v3,
                            const bslstl::StringRef& k4, const T4& v4) const
{
    bdld::DatumMapOwningKeysBuilder builder(d_allocator_p);
    builder.pushBack(k0, (*this)(v0));
    builder.pushBack(k1, (*this)(v1));
    builder.pushBack(k2, (*this)(v2));
    builder.pushBack(k3, (*this)(v3));
    builder.pushBack(k4, (*this)(v4));
    return builder.commit();
}

template <class T0,  class T1, class T2, class T3, class T4,  class T5>
bdld::Datum DatumMaker::mok(const bslstl::StringRef& k0, const T0& v0,
                            const bslstl::StringRef& k1, const T1& v1,
                            const bslstl::StringRef& k2, const T2& v2,
                            const bslstl::StringRef& k3, const T3& v3,
                            const bslstl::StringRef& k4, const T4& v4,
                            const bslstl::StringRef& k5, const T5& v5) const
{
    bdld::DatumMapOwningKeysBuilder builder(d_allocator_p);
    builder.pushBack(k0, (*this)(v0));
    builder.pushBack(k1, (*this)(v1));
    builder.pushBack(k2, (*this)(v2));
    builder.pushBack(k3, (*this)(v3));
    builder.pushBack(k4, (*this)(v4));
    builder.pushBack(k5, (*this)(v5));
    return builder.commit();
}

template <class T0,  class T1, class T2, class T3, class T4,  class T5,
          class T6>
bdld::Datum DatumMaker::mok(const bslstl::StringRef& k0, const T0& v0,
                            const bslstl::StringRef& k1, const T1& v1,
                            const bslstl::StringRef& k2, const T2& v2,
                            const bslstl::StringRef& k3, const T3& v3,
                            const bslstl::StringRef& k4, const T4& v4,
                            const bslstl::StringRef& k5, const T5& v5,
                            const bslstl::StringRef& k6, const T6& v6) const
{
    bdld::DatumMapOwningKeysBuilder builder(d_allocator_p);
    builder.pushBack(k0, (*this)(v0));
    builder.pushBack(k1, (*this)(v1));
    builder.pushBack(k2, (*this)(v2));
    builder.pushBack(k3, (*this)(v3));
    builder.pushBack(k4, (*this)(v4));
    builder.pushBack(k5, (*this)(v5));
    builder.pushBack(k6, (*this)(v6));
    return builder.commit();
}

template <class T0,  class T1, class T2, class T3, class T4,  class T5,
          class T6,  class T7>
bdld::Datum DatumMaker::mok(const bslstl::StringRef& k0, const T0& v0,
                            const bslstl::StringRef& k1, const T1& v1,
                            const bslstl::StringRef& k2, const T2& v2,
                            const bslstl::StringRef& k3, const T3& v3,
                            const bslstl::StringRef& k4, const T4& v4,
                            const bslstl::StringRef& k5, const T5& v5,
                            const bslstl::StringRef& k6, const T6& v6,
                            const bslstl::StringRef& k7, const T7& v7) const
{
    bdld::DatumMapOwningKeysBuilder builder(d_allocator_p);
    builder.pushBack(k0, (*this)(v0));
    builder.pushBack(k1, (*this)(v1));
    builder.pushBack(k2, (*this)(v2));
    builder.pushBack(k3, (*this)(v3));
    builder.pushBack(k4, (*this)(v4));
    builder.pushBack(k5, (*this)(v5));
    builder.pushBack(k6, (*this)(v6));
    builder.pushBack(k7, (*this)(v7));
    return builder.commit();
}

template <class T0,  class T1, class T2, class T3, class T4,  class T5,
          class T6,  class T7, class T8>
bdld::Datum DatumMaker::mok(const bslstl::StringRef& k0, const T0& v0,
                            const bslstl::StringRef& k1, const T1& v1,
                            const bslstl::StringRef& k2, const T2& v2,
                            const bslstl::StringRef& k3, const T3& v3,
                            const bslstl::StringRef& k4, const T4& v4,
                            const bslstl::StringRef& k5, const T5& v5,
                            const bslstl::StringRef& k6, const T6& v6,
                            const bslstl::StringRef& k7, const T7& v7,
                            const bslstl::StringRef& k8, const T8& v8) const
{
    bdld::DatumMapOwningKeysBuilder builder(d_allocator_p);
    builder.pushBack(k0, (*this)(v0));
    builder.pushBack(k1, (*this)(v1));
    builder.pushBack(k2, (*this)(v2));
    builder.pushBack(k3, (*this)(v3));
    builder.pushBack(k4, (*this)(v4));
    builder.pushBack(k5, (*this)(v5));
    builder.pushBack(k6, (*this)(v6));
    builder.pushBack(k7, (*this)(v7));
    builder.pushBack(k8, (*this)(v8));
    return builder.commit();
}

template <class T0,  class T1, class T2, class T3, class T4,  class T5,
          class T6,  class T7, class T8, class T9>
bdld::Datum DatumMaker::mok(const bslstl::StringRef& k0, const T0& v0,
                            const bslstl::StringRef& k1, const T1& v1,
                            const bslstl::StringRef& k2, const T2& v2,
                            const bslstl::StringRef& k3, const T3& v3,
                            const bslstl::StringRef& k4, const T4& v4,
                            const bslstl::StringRef& k5, const T5& v5,
                            const bslstl::StringRef& k6, const T6& v6,
                            const bslstl::StringRef& k7, const T7& v7,
                            const bslstl::StringRef& k8, const T8& v8,
                            const bslstl::StringRef& k9, const T9& v9) const
{
    bdld::DatumMapOwningKeysBuilder builder(d_allocator_p);
    builder.pushBack(k0, (*this)(v0));
    builder.pushBack(k1, (*this)(v1));
    builder.pushBack(k2, (*this)(v2));
    builder.pushBack(k3, (*this)(v3));
    builder.pushBack(k4, (*this)(v4));
    builder.pushBack(k5, (*this)(v5));
    builder.pushBack(k6, (*this)(v6));
    builder.pushBack(k7, (*this)(v7));
    builder.pushBack(k8, (*this)(v8));
    builder.pushBack(k9, (*this)(v9));
    return builder.commit();
}

template <class T0,  class T1, class T2, class T3, class T4,  class T5,
          class T6,  class T7, class T8, class T9, class T10>
bdld::Datum DatumMaker::mok(const bslstl::StringRef& k0,  const T0&  v0,
                            const bslstl::StringRef& k1,  const T1&  v1,
                            const bslstl::StringRef& k2,  const T2&  v2,
                            const bslstl::StringRef& k3,  const T3&  v3,
                            const bslstl::StringRef& k4,  const T4&  v4,
                            const bslstl::StringRef& k5,  const T5&  v5,
                            const bslstl::StringRef& k6,  const T6&  v6,
                            const bslstl::StringRef& k7,  const T7&  v7,
                            const bslstl::StringRef& k8,  const T8&  v8,
                            const bslstl::StringRef& k9,  const T9&  v9,
                            const bslstl::StringRef& k10, const T10& v10) const
{
    bdld::DatumMapOwningKeysBuilder builder(d_allocator_p);
    builder.pushBack(k0,  (*this)(v0));
    builder.pushBack(k1,  (*this)(v1));
    builder.pushBack(k2,  (*this)(v2));
    builder.pushBack(k3,  (*this)(v3));
    builder.pushBack(k4,  (*this)(v4));
    builder.pushBack(k5,  (*this)(v5));
    builder.pushBack(k6,  (*this)(v6));
    builder.pushBack(k7,  (*this)(v7));
    builder.pushBack(k8,  (*this)(v8));
    builder.pushBack(k9,  (*this)(v9));
    builder.pushBack(k10, (*this)(v10));
    return builder.commit();
}

template <class T0,  class T1, class T2, class T3, class T4,  class T5,
          class T6,  class T7, class T8, class T9, class T10, class T11>
bdld::Datum DatumMaker::mok(const bslstl::StringRef&  k0, const T0&  v0,
                            const bslstl::StringRef&  k1, const T1&  v1,
                            const bslstl::StringRef&  k2, const T2&  v2,
                            const bslstl::StringRef&  k3, const T3&  v3,
                            const bslstl::StringRef&  k4, const T4&  v4,
                            const bslstl::StringRef&  k5, const T5&  v5,
                            const bslstl::StringRef&  k6, const T6&  v6,
                            const bslstl::StringRef&  k7, const T7&  v7,
                            const bslstl::StringRef&  k8, const T8&  v8,
                            const bslstl::StringRef&  k9, const T9&  v9,
                            const bslstl::StringRef& k10, const T10& v10,
                            const bslstl::StringRef& k11, const T11& v11) const
{
    bdld::DatumMapOwningKeysBuilder builder(d_allocator_p);
    builder.pushBack(k0,  (*this)(v0));
    builder.pushBack(k1,  (*this)(v1));
    builder.pushBack(k2,  (*this)(v2));
    builder.pushBack(k3,  (*this)(v3));
    builder.pushBack(k4,  (*this)(v4));
    builder.pushBack(k5,  (*this)(v5));
    builder.pushBack(k6,  (*this)(v6));
    builder.pushBack(k7,  (*this)(v7));
    builder.pushBack(k8,  (*this)(v8));
    builder.pushBack(k9,  (*this)(v9));
    builder.pushBack(k10, (*this)(v10));
    builder.pushBack(k11, (*this)(v11));
    return builder.commit();
}

template <class T0,  class T1, class T2, class T3, class T4,  class T5,
          class T6,  class T7, class T8, class T9, class T10, class T11,
          class T12>
bdld::Datum DatumMaker::mok(const bslstl::StringRef& k0,  const T0&  v0,
                            const bslstl::StringRef& k1,  const T1&  v1,
                            const bslstl::StringRef& k2,  const T2&  v2,
                            const bslstl::StringRef& k3,  const T3&  v3,
                            const bslstl::StringRef& k4,  const T4&  v4,
                            const bslstl::StringRef& k5,  const T5&  v5,
                            const bslstl::StringRef& k6,  const T6&  v6,
                            const bslstl::StringRef& k7,  const T7&  v7,
                            const bslstl::StringRef& k8,  const T8&  v8,
                            const bslstl::StringRef& k9,  const T9&  v9,
                            const bslstl::StringRef& k10, const T10& v10,
                            const bslstl::StringRef& k11, const T11& v11,
                            const bslstl::StringRef& k12, const T12& v12) const
{
    bdld::DatumMapOwningKeysBuilder builder(d_allocator_p);
    builder.pushBack(k0,  (*this)(v0));
    builder.pushBack(k1,  (*this)(v1));
    builder.pushBack(k2,  (*this)(v2));
    builder.pushBack(k3,  (*this)(v3));
    builder.pushBack(k4,  (*this)(v4));
    builder.pushBack(k5,  (*this)(v5));
    builder.pushBack(k6,  (*this)(v6));
    builder.pushBack(k7,  (*this)(v7));
    builder.pushBack(k8,  (*this)(v8));
    builder.pushBack(k9,  (*this)(v9));
    builder.pushBack(k10, (*this)(v10));
    builder.pushBack(k11, (*this)(v11));
    builder.pushBack(k12, (*this)(v12));
    return builder.commit();
}

template <class T0,  class T1, class T2, class T3, class T4,  class T5,
          class T6,  class T7, class T8, class T9, class T10, class T11,
          class T12, class T13>
bdld::Datum DatumMaker::mok(const bslstl::StringRef& k0,  const T0&  v0,
                            const bslstl::StringRef& k1,  const T1&  v1,
                            const bslstl::StringRef& k2,  const T2&  v2,
                            const bslstl::StringRef& k3,  const T3&  v3,
                            const bslstl::StringRef& k4,  const T4&  v4,
                            const bslstl::StringRef& k5,  const T5&  v5,
                            const bslstl::StringRef& k6,  const T6&  v6,
                            const bslstl::StringRef& k7,  const T7&  v7,
                            const bslstl::StringRef& k8,  const T8&  v8,
                            const bslstl::StringRef& k9,  const T9&  v9,
                            const bslstl::StringRef& k10, const T10& v10,
                            const bslstl::StringRef& k11, const T11& v11,
                            const bslstl::StringRef& k12, const T12& v12,
                            const bslstl::StringRef& k13, const T13& v13) const
{
    bdld::DatumMapOwningKeysBuilder builder(d_allocator_p);
    builder.pushBack(k0,  (*this)(v0));
    builder.pushBack(k1,  (*this)(v1));
    builder.pushBack(k2,  (*this)(v2));
    builder.pushBack(k3,  (*this)(v3));
    builder.pushBack(k4,  (*this)(v4));
    builder.pushBack(k5,  (*this)(v5));
    builder.pushBack(k6,  (*this)(v6));
    builder.pushBack(k7,  (*this)(v7));
    builder.pushBack(k8,  (*this)(v8));
    builder.pushBack(k9,  (*this)(v9));
    builder.pushBack(k10, (*this)(v10));
    builder.pushBack(k11, (*this)(v11));
    builder.pushBack(k12, (*this)(v12));
    builder.pushBack(k13, (*this)(v13));
    return builder.commit();
}

template <class T0,  class T1, class T2, class T3, class T4,  class T5,
          class T6,  class T7, class T8, class T9, class T10, class T11,
          class T12, class T13, class T14>
bdld::Datum DatumMaker::mok(const bslstl::StringRef& k0,  const T0&  v0,
                            const bslstl::StringRef& k1,  const T1&  v1,
                            const bslstl::StringRef& k2,  const T2&  v2,
                            const bslstl::StringRef& k3,  const T3&  v3,
                            const bslstl::StringRef& k4,  const T4&  v4,
                            const bslstl::StringRef& k5,  const T5&  v5,
                            const bslstl::StringRef& k6,  const T6&  v6,
                            const bslstl::StringRef& k7,  const T7&  v7,
                            const bslstl::StringRef& k8,  const T8&  v8,
                            const bslstl::StringRef& k9,  const T9&  v9,
                            const bslstl::StringRef& k10, const T10& v10,
                            const bslstl::StringRef& k11, const T11& v11,
                            const bslstl::StringRef& k12, const T12& v12,
                            const bslstl::StringRef& k13, const T13& v13,
                            const bslstl::StringRef& k14, const T14& v14) const
{
    bdld::DatumMapOwningKeysBuilder builder(d_allocator_p);
    builder.pushBack(k0,  (*this)(v0));
    builder.pushBack(k1,  (*this)(v1));
    builder.pushBack(k2,  (*this)(v2));
    builder.pushBack(k3,  (*this)(v3));
    builder.pushBack(k4,  (*this)(v4));
    builder.pushBack(k5,  (*this)(v5));
    builder.pushBack(k6,  (*this)(v6));
    builder.pushBack(k7,  (*this)(v7));
    builder.pushBack(k8,  (*this)(v8));
    builder.pushBack(k9,  (*this)(v9));
    builder.pushBack(k10, (*this)(v10));
    builder.pushBack(k11, (*this)(v11));
    builder.pushBack(k12, (*this)(v12));
    builder.pushBack(k13, (*this)(v13));
    builder.pushBack(k14, (*this)(v14));
    return builder.commit();
}

template <class T0,  class T1, class T2, class T3, class T4,  class T5,
          class T6,  class T7, class T8, class T9, class T10, class T11,
          class T12, class T13, class T14, class T15>
bdld::Datum DatumMaker::mok(const bslstl::StringRef& k0,  const T0&  v0,
                            const bslstl::StringRef& k1,  const T1&  v1,
                            const bslstl::StringRef& k2,  const T2&  v2,
                            const bslstl::StringRef& k3,  const T3&  v3,
                            const bslstl::StringRef& k4,  const T4&  v4,
                            const bslstl::StringRef& k5,  const T5&  v5,
                            const bslstl::StringRef& k6,  const T6&  v6,
                            const bslstl::StringRef& k7,  const T7&  v7,
                            const bslstl::StringRef& k8,  const T8&  v8,
                            const bslstl::StringRef& k9,  const T9&  v9,
                            const bslstl::StringRef& k10, const T10& v10,
                            const bslstl::StringRef& k11, const T11& v11,
                            const bslstl::StringRef& k12, const T12& v12,
                            const bslstl::StringRef& k13, const T13& v13,
                            const bslstl::StringRef& k14, const T14& v14,
                            const bslstl::StringRef& k15, const T15& v15) const
{
    bdld::DatumMapOwningKeysBuilder builder(d_allocator_p);
    builder.pushBack(k0,  (*this)(v0));
    builder.pushBack(k1,  (*this)(v1));
    builder.pushBack(k2,  (*this)(v2));
    builder.pushBack(k3,  (*this)(v3));
    builder.pushBack(k4,  (*this)(v4));
    builder.pushBack(k5,  (*this)(v5));
    builder.pushBack(k6,  (*this)(v6));
    builder.pushBack(k7,  (*this)(v7));
    builder.pushBack(k8,  (*this)(v8));
    builder.pushBack(k9,  (*this)(v9));
    builder.pushBack(k10, (*this)(v10));
    builder.pushBack(k11, (*this)(v11));
    builder.pushBack(k12, (*this)(v12));
    builder.pushBack(k13, (*this)(v13));
    builder.pushBack(k14, (*this)(v14));
    builder.pushBack(k15, (*this)(v15));
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
