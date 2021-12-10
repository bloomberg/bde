// s_baltst_generatetestnullablevalue.h                               -*-C++-*-
#ifndef INCLUDED_S_BALTST_GENERATETESTNULLABLEVALUE
#define INCLUDED_S_BALTST_GENERATETESTNULLABLEVALUE

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a component for generating test nullable value types.
//
//@CLASSES:
//  s_baltst::GenerateTestNullableValue: generate test nullable value
//  s_baltst::GenerateTestNullableValuePlaceHolder: gen nullable placeholder

#include <s_baltst_testnullablevalueutil.h>
#include <s_baltst_testplaceholder.h>

namespace BloombergLP {
namespace s_baltst {

                      // ===============================
                      // class GenerateTestNullableValue
                      // ===============================

class GenerateTestNullableValue {
    // This in-core value-semantic class provides a function object whose
    // function call operator can be used to generate objects of
    // 'bdlb::NullableValue' specializations.

  public:
    // TYPES
    typedef TestNullableValueUtil Util;

    // CREATORS
    GenerateTestNullableValue();

    // ACCESSORS
    template <class VALUE_TYPE>
    bdlb::NullableValue<VALUE_TYPE> operator()(
                                     const TestPlaceHolder<VALUE_TYPE>&) const;

    template <class VALUE_TYPE>
    bdlb::NullableValue<VALUE_TYPE> operator()(const VALUE_TYPE& value) const;

};

                   // =====================================
                   // class GenerateTestNullablePlaceHolder
                   // =====================================

class GenerateTestNullablePlaceHolder {
    // This in-core value-semantic class provides a function object whose
    // function call operator can be used to generate 'PlaceHolder'
    // specializations for 'bdlb::NullableValue' types.

  public:
    // TYPES
    typedef TestNullableValueUtil Util;

    // CREATORS
    GenerateTestNullablePlaceHolder();

    // ACCESSOR
    template <class VALUE_TYPE>
    TestPlaceHolder<bdlb::NullableValue<VALUE_TYPE> > operator()(
                                      const TestPlaceHolder<VALUE_TYPE>&) const;

};

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

                      // -------------------------------
                      // class GenerateTestNullableValue
                      // -------------------------------

// CREATORS
inline
GenerateTestNullableValue::GenerateTestNullableValue()
{
}

// ACCESSORS
template <class VALUE_TYPE>
bdlb::NullableValue<VALUE_TYPE> GenerateTestNullableValue::operator()(
                                      const TestPlaceHolder<VALUE_TYPE>&) const
{
    return Util::generate(TestPlaceHolder<VALUE_TYPE>());
}

template <class VALUE_TYPE>
bdlb::NullableValue<VALUE_TYPE> GenerateTestNullableValue::operator()(
                                                 const VALUE_TYPE& value) const
{
    return Util::generate(value);

}

                 // ------------------------------------------
                 // class GenerateTestNullableValuePlaceHolder
                 // ------------------------------------------

// CREATORS
inline
GenerateTestNullablePlaceHolder::GenerateTestNullablePlaceHolder()
{
}

// ACCESSOR
template <class VALUE_TYPE>
TestPlaceHolder<bdlb::NullableValue<VALUE_TYPE> >
GenerateTestNullablePlaceHolder::operator()(
                                      const TestPlaceHolder<VALUE_TYPE>&) const
{
    return TestPlaceHolder<bdlb::NullableValue<VALUE_TYPE> >();
}

}  // close package namespace
}  // close enterprise namespace

#endif  // INCLUDED_S_BALTST_GENERATETESTNULLABLEVALUE

// ----------------------------------------------------------------------------
// Copyright 2021 Bloomberg Finance L.P.
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
