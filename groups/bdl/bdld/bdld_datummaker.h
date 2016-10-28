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

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES // $var-args=0
    // PRIVATE ACCESSORS
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
    void pushBackHelper(bdld::DatumMapBuilder *builder,
                        bslstl::StringRef&&    key,
                        TYPE&&                 value) const;
        // 'push_back' the specified 'key' and 'value' pair (forming a
        // property) into the specified 'builder'.

    template <typename TYPE, typename... ENTRIES>
    void pushBackHelper(bdld::DatumMapBuilder *builder,
                        bslstl::StringRef&&    key,
                        TYPE&&                 value,
                        ENTRIES&&...           entries) const;
        // 'push_back' the specified 'key' and 'value' pair (forming a
        // property) into the specified 'builder', then call 'pushBackHelper'
        // with the specified (variadic) entries.

    template <typename TYPE>
    void pushBackHelper(bdld::DatumMapOwningKeysBuilder *builder,
                        bslstl::StringRef&&              key,
                        TYPE&&                           value) const;
        // 'push_back' the specified 'key' and 'value' pair (forming a
        // property) into the specified 'builder'.

    template <typename TYPE, typename... ENTRIES>
    void pushBackHelper(bdld::DatumMapOwningKeysBuilder *builder,
                        bslstl::StringRef&&              key,
                        TYPE&&                           value,
                        ENTRIES&&...                     entries) const;
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

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES // $var-args=0
	template <typename... ELEMENTS>
    bdld::Datum a(ELEMENTS&&... elements) const;
        // Return a 'bdld::Datum' having an array value of the specified
        // 'elements'.
#endif

    bdld::Datum m() const;
        // Return a 'bdld::Datum' object containing an empty map.

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES // $var-args=0
	template <typename... ENTRIES>
    bdld::Datum m(ENTRIES&&... entries) const;
        // Return a 'bdld::Datum' object containing a map of the specified
        // 'entries'.
#endif

    bdld::Datum mok() const;
        // Return a 'bdld::Datum' object containing an empty map with owned
        // keys.

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES // $var-args=0
	template <typename... ENTRIES>
    bdld::Datum mok(ENTRIES&&... entries) const;
        // Return a 'bdld::Datum' object containing a map with owned keys
        // consisting of the specified 'entries'.
#endif
};

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

                              // ----------------
                              // class DatumMaker
                              // ----------------

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES // $var-args=0
// PRIVATE ACCESSORS
template <typename TYPE>
void DatumMaker::pushBackHelper(bdld::DatumArrayBuilder *builder,
                                TYPE&&                   element) const
{
    builder->pushBack((*this)(element));
}

template <typename TYPE, typename... ELEMENTS>
void DatumMaker::pushBackHelper(bdld::DatumArrayBuilder *builder,
                                TYPE&&                   element,
                                ELEMENTS&&...            elements) const
{
    builder->pushBack((*this)(element));
    pushBackHelper(builder, elements...);
}

template <typename TYPE>
void DatumMaker::pushBackHelper(bdld::DatumMapBuilder *builder,
                                bslstl::StringRef&&    key,
                                TYPE&&                 value) const
{
    builder->pushBack(key, (*this)(value));
}

template <typename TYPE, typename... ENTRIES>
void DatumMaker::pushBackHelper(bdld::DatumMapBuilder *builder,
                                bslstl::StringRef&&    key,
                                TYPE&&                 value,
                                ENTRIES&&...           entries) const
{
    builder->pushBack(key, (*this)(value));
    pushBackHelper(builder, entries...);
}

template <typename TYPE>
void DatumMaker::pushBackHelper(bdld::DatumMapOwningKeysBuilder *builder,
                                bslstl::StringRef&&              key,
                                TYPE&&                           value) const
{
    builder->pushBack(key, (*this)(value));
}

template <typename TYPE, typename... ENTRIES>
void DatumMaker::pushBackHelper(
                               bdld::DatumMapOwningKeysBuilder *builder,
                               bslstl::StringRef&&              key,
                               TYPE&&                           value,
                               ENTRIES&&...                     entries) const
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

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES // $var-args=0
template <typename... ELEMENTS>
bdld::Datum DatumMaker::a(ELEMENTS&&... elements) const
{
    const int numElements = sizeof...(ELEMENTS);
    bdld::DatumArrayBuilder builder(numElements, d_allocator_p);
    pushBackHelper(&builder, elements...);
    return builder.commit();
}
#endif

inline
bdld::Datum DatumMaker::m() const
{
    return (*this)(bdld::DatumMutableMapRef());
}

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES // $var-args=0
template <typename... ENTRIES>
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
#endif

inline
bdld::Datum DatumMaker::mok() const
{
    return (*this)(bdld::DatumMutableMapRef());
}

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES // $var-args=0
template <typename... ENTRIES>
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
