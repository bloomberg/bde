// s_baltst_testarrayutil.h                                           -*-C++-*-
#ifndef INCLUDED_S_BALTST_TESTARRAYUTIL
#define INCLUDED_S_BALTST_TESTARRAYUTIL

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a utilty for generating test arrays.
//
//@CLASSES:
//  s_baltst::TestArrayUtil: utility for generating test arrays

#include <s_baltst_testplaceholder.h>

#include <bsl_vector.h>

namespace BloombergLP {
namespace s_baltst {

                            // ====================
                            // struct TestArrayUtil
                            // ====================

struct TestArrayUtil {
    // CLASS METHODS
    template <class VALUE_TYPE>
    static bsl::vector<VALUE_TYPE> generate(
                                           const TestPlaceHolder<VALUE_TYPE>&);

    template <class VALUE_TYPE>
    static bsl::vector<VALUE_TYPE> generate(const VALUE_TYPE& value);

    template <class VALUE_TYPE>
    static bsl::vector<VALUE_TYPE> generate(const VALUE_TYPE& value0,
                                            const VALUE_TYPE& value1);

    template <class VALUE_TYPE>
    static bsl::vector<VALUE_TYPE> generate(const VALUE_TYPE& value0,
                                            const VALUE_TYPE& value1,
                                            const VALUE_TYPE& value2);

    template <class VALUE_TYPE>
    static TestPlaceHolder<bsl::vector<VALUE_TYPE> > generatePlaceHolder(
                                           const TestPlaceHolder<VALUE_TYPE>&);
};

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

                            // -------------------
                            // class TestArrayUtil
                            // -------------------

// CLASS METHODS
template <class VALUE_TYPE>
bsl::vector<VALUE_TYPE> TestArrayUtil::generate(
                                            const TestPlaceHolder<VALUE_TYPE>&)
{
    return bsl::vector<VALUE_TYPE>();
}

template <class VALUE_TYPE>
bsl::vector<VALUE_TYPE> TestArrayUtil::generate(const VALUE_TYPE& value)
{
    bsl::vector<VALUE_TYPE> result;
    result.push_back(value);
    return result;
}

template <class VALUE_TYPE>
bsl::vector<VALUE_TYPE> TestArrayUtil::generate(const VALUE_TYPE& value0,
                                                const VALUE_TYPE& value1)
{
    bsl::vector<VALUE_TYPE> result;
    result.push_back(value0);
    result.push_back(value1);
    return result;
}

template <class VALUE_TYPE>
bsl::vector<VALUE_TYPE> TestArrayUtil::generate(const VALUE_TYPE& value0,
                                                const VALUE_TYPE& value1,
                                                const VALUE_TYPE& value2)
{
    bsl::vector<VALUE_TYPE> result;
    result.push_back(value0);
    result.push_back(value1);
    result.push_back(value2);
    return result;
}

template <class VALUE_TYPE>
TestPlaceHolder<bsl::vector<VALUE_TYPE> > TestArrayUtil::generatePlaceHolder(
                                            const TestPlaceHolder<VALUE_TYPE>&)
{
    return TestPlaceHolder<bsl::vector<VALUE_TYPE> >();
}

}  // close package namespace
}  // close enterprise namespace

#endif // INCLUDED_S_BALTST_TESTARRAYUTIL

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
