// s_baltst_generatetestdynamictype.h                                 -*-C++-*-
#ifndef INCLUDED_S_BALTST_GENERATETESTDYNAMICTYPE
#define INCLUDED_S_BALTST_GENERATETESTDYNAMICTYPE

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a component for generating test dynamic types.
//
//@CLASSES:
//  s_baltst::GenerateTestDynamicType: generate test dynamic type
//  s_baltst::GenerateTestDynamicTypePlaceHolder: gen dynamic type placeholder

#include <s_baltst_testdynamictype.h>
#include <s_baltst_testdynamictypeutil.h>
#include <s_baltst_testplaceholder.h>

namespace BloombergLP {
namespace s_baltst {

                       // =============================
                       // class GenerateTestDynamicType
                       // =============================

/// This in-core value-semantic class provides a function object whose
/// function call operator can be used to generate objects of
/// specializations of `TestDynamicType`.
class GenerateTestDynamicType {

  public:
    // TYPES
    typedef TestDynamicTypeUtil Util;

    // CREATORS
    GenerateTestDynamicType();

    // ACCESSORS
    template <class VALUE_TYPE>
    TestDynamicType<VALUE_TYPE> operator()(const VALUE_TYPE& value) const;
};

                    // ====================================
                    // class GenerateTestDynamicPlaceHolder
                    // ====================================

/// This in-core value-semantic class provides a function object whose
/// function call operator can be used to generate `PlaceHolder`
/// specializations for `TestDynamicType` types.
class GenerateTestDynamicPlaceHolder {

  public:
    // TYPES
    typedef TestDynamicTypeUtil Util;

    // CREATORS
    GenerateTestDynamicPlaceHolder();

    // ACCESSORS
    template <class VALUE_TYPE>
    TestPlaceHolder<TestDynamicType<VALUE_TYPE> > operator()(
                                     const TestPlaceHolder<VALUE_TYPE>&) const;
};

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

                       // -----------------------------
                       // class GenerateTestDynamicType
                       // -----------------------------

// CREATORS
inline
GenerateTestDynamicType::GenerateTestDynamicType()
{
}

// ACCESSORS
template <class VALUE_TYPE>
TestDynamicType<VALUE_TYPE> GenerateTestDynamicType::operator()(
                                                 const VALUE_TYPE& value) const
{
    return Util::generateDynamicType(value);
}

                  // ----------------------------------------
                  // class GenerateTestDynamicTypePlaceHolder
                  // ----------------------------------------

// CREATORS
inline
GenerateTestDynamicPlaceHolder::GenerateTestDynamicPlaceHolder()
{
}

// ACCESSORS
template <class VALUE_TYPE>
TestPlaceHolder<TestDynamicType<VALUE_TYPE> >
GenerateTestDynamicPlaceHolder::operator()(
                                      const TestPlaceHolder<VALUE_TYPE>&) const
{
    return TestPlaceHolder<TestDynamicType<VALUE_TYPE> >();
}

}  // close package namespace
}  // close enterprise namespace

#endif // INCLUDED_S_BALTST_GENERATETESTDYNAMICTYPE

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
