// s_baltst_generatetestcustomizedtype.h                              -*-C++-*-
#ifndef INCLUDED_S_BALTST_GENERATETESTCUSTOMIZEDTYPE
#define INCLUDED_S_BALTST_GENERATETESTCUSTOMIZEDTYPE

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a component for generating test customized types.
//
//@CLASSES:
//  s_baltst::GenerateTestCustomizedType: generate test customized type

#include <s_baltst_testcustomizedtype.h>
#include <s_baltst_testcustomizedtypeutil.h>

namespace BloombergLP {
namespace s_baltst {

                      // ================================
                      // class GenerateTestCustomizedType
                      // ================================

class GenerateTestCustomizedType {
    // This in-core value-semantic class provides a function object whose
    // function call operator can be used to generate objects of
    // specializations of 'TestCustomizedType'.

  public:
    // TYPES
    typedef TestCustomizedTypeUtil Util;

    // CREATORS
    GenerateTestCustomizedType();

    // ACCESSORS
    template <class VALUE_TYPE, class BASE_TYPE>
    TestCustomizedType<VALUE_TYPE, BASE_TYPE> operator()(
                                     const VALUE_TYPE&                 value,
                                     const TestPlaceHolder<BASE_TYPE>&) const;
};

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

                      // --------------------------------
                      // class GenerateTestCustomizedType
                      // --------------------------------

// CREATORS
GenerateTestCustomizedType::GenerateTestCustomizedType()
{
}

// ACCESSORS
template <class VALUE_TYPE, class BASE_TYPE>
TestCustomizedType<VALUE_TYPE, BASE_TYPE>
GenerateTestCustomizedType::operator()(
                                      const VALUE_TYPE&                 value,
                                      const TestPlaceHolder<BASE_TYPE>&) const
{
    return Util::generate(value, TestPlaceHolder<BASE_TYPE>());
}

}  // close package namespace
}  // close enterprise namespace

#endif // INCLUDED_S_BALTST_GENERATETESTCUSTOMIZEDTYPE

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
