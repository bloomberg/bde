// s_baltst_generatetestarray.h                                       -*-C++-*-
#ifndef INCLUDED_S_BALTST_GENERATETESTARRAY
#define INCLUDED_S_BALTST_GENERATETESTARRAY

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a component for generating test array types.
//
//@CLASSES:
//  s_baltst::GenerateTestArray: generate test array
//  s_baltst::GenerateTestArrayPlaceHolder: generate test array placeholder

#include <s_baltst_testarrayutil.h>
#include <s_baltst_testplaceholder.h>

#include <bsl_vector.h>

namespace BloombergLP {
namespace s_baltst {

                          // =======================
                          // class GenerateTestArray
                          // =======================

class GenerateTestArray {
    // This class provides a function object whose function call operator can
    // be used to generate objects of specializations of 'bsl::vector'.

  public:
    // TYPES
    typedef TestArrayUtil Util;

    // CREATORS
    GenerateTestArray();

    // ACCESSORS
    template <class VALUE_TYPE>
    bsl::vector<VALUE_TYPE> operator()(
                                     const TestPlaceHolder<VALUE_TYPE>&) const;

    template <class VALUE_TYPE>
    bsl::vector<VALUE_TYPE> operator()(const VALUE_TYPE& value) const;

    template <class VALUE_TYPE>
    bsl::vector<VALUE_TYPE> operator()(const VALUE_TYPE& value0,
                                       const VALUE_TYPE& value1) const;

    template <class VALUE_TYPE>
    bsl::vector<VALUE_TYPE> operator()(const VALUE_TYPE& value0,
                                       const VALUE_TYPE& value1,
                                       const VALUE_TYPE& value2) const;
};

                     // ==================================
                     // class GenerateTestArrayPlaceHolder
                     // ==================================

class GenerateTestArrayPlaceHolder {
    // This class provides a function object whose function call operator can
    // be used to generate 'TestPlaceHolder' specializations for 'bsl::vector'
    // types.

  public:
    // TYPES
    typedef TestArrayUtil Util;

    // CREATORS
    GenerateTestArrayPlaceHolder();

    // ACCESSORS
    template <class VALUE_TYPE>
    TestPlaceHolder<bsl::vector<VALUE_TYPE> > operator()(
                                     const TestPlaceHolder<VALUE_TYPE>&) const;
};


// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

                          // -----------------------
                          // class GenerateTestArray
                          // -----------------------

// CREATORS
GenerateTestArray::GenerateTestArray()
{
}

// ACCESSORS
template <class VALUE_TYPE>
bsl::vector<VALUE_TYPE> GenerateTestArray::operator()(
                                      const TestPlaceHolder<VALUE_TYPE>&) const
{
    return Util::generate(TestPlaceHolder<VALUE_TYPE>());
}

template <class VALUE_TYPE>
bsl::vector<VALUE_TYPE> GenerateTestArray::operator()(
                                                 const VALUE_TYPE& value) const
{
    return Util::generate(value);
}

template <class VALUE_TYPE>
bsl::vector<VALUE_TYPE> GenerateTestArray::operator()(
                                                const VALUE_TYPE& value0,
                                                const VALUE_TYPE& value1) const
{
    return Util::generate(value0, value1);
}

template <class VALUE_TYPE>
bsl::vector<VALUE_TYPE> GenerateTestArray::operator()(
                                                const VALUE_TYPE& value0,
                                                const VALUE_TYPE& value1,
                                                const VALUE_TYPE& value2) const
{
    return Util::generate(value0, value1, value2);
}

                     // ----------------------------------
                     // class GenerateTestArrayPlaceHolder
                     // ----------------------------------

// CREATORS
GenerateTestArrayPlaceHolder::GenerateTestArrayPlaceHolder()
{
}

// ACCESSORS
template <class VALUE_TYPE>
TestPlaceHolder<bsl::vector<VALUE_TYPE> >
GenerateTestArrayPlaceHolder::operator()(
                                      const TestPlaceHolder<VALUE_TYPE>&) const
{
    return TestPlaceHolder<bsl::vector<VALUE_TYPE> >();
}

}  // close package namespace
}  // close enterprise namespace

#endif // INCLUDED_S_BALTST_GENERATETESTARRAY

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
