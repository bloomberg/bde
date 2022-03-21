// s_baltst_testnullablevalueutil.h                                   -*-C++-*-
#ifndef INCLUDED_S_BALTST_TESTNULLABLEVALUEUTIL
#define INCLUDED_S_BALTST_TESTNULLABLEVALUEUTIL

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide non-primitive operations on nullable values for testing.
//
//@CLASSES:
//  s_baltst::TestNullableValueUtil: test utilities for nullable values

#include <bdlb_nullablevalue.h>

#include <s_baltst_testplaceholder.h>

namespace BloombergLP {
namespace s_baltst {

                        // ============================
                        // struct TestNullableValueUtil
                        // ============================

struct TestNullableValueUtil {
    // CLASS METHODS
    template <class VALUE_TYPE>
    static bdlb::NullableValue<VALUE_TYPE> generate(
                                           const TestPlaceHolder<VALUE_TYPE>&);

    template <class VALUE_TYPE>
    static bdlb::NullableValue<VALUE_TYPE> generate(const VALUE_TYPE& value);
};

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

                        // ----------------------------
                        // struct TestNullableValueUtil
                        // ----------------------------

// CLASS METHODS
template <class VALUE_TYPE>
bdlb::NullableValue<VALUE_TYPE> TestNullableValueUtil::generate(
                                            const TestPlaceHolder<VALUE_TYPE>&)
{
    return bdlb::NullableValue<VALUE_TYPE>();
}

template <class VALUE_TYPE>
bdlb::NullableValue<VALUE_TYPE> TestNullableValueUtil::generate(
                                                       const VALUE_TYPE& value)
{
    return bdlb::NullableValue<VALUE_TYPE>(value);
}

}  // close package namespace
}  // close enterprise namespace

#endif // INCLUDED_S_BALTST_TESTNULLABLEVALUEUTIL

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
