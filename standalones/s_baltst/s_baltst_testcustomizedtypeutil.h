// s_baltst_testcustomizedtypeutil.h                                  -*-C++-*-
#ifndef INCLUDED_S_BALTST_TESTCUSTOMIZEDTYPEUTIL
#define INCLUDED_S_BALTST_TESTCUSTOMIZEDTYPEUTIL

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide non-primitive operations on `s_baltst::TestCustomizedType`.
//
//@CLASSES:
//  s_baltst::TestCustomizedTypeUtil: utilities for test customized types

#include <s_baltst_testcustomizedtype.h>
#include <s_baltst_testplaceholder.h>

namespace BloombergLP {
namespace s_baltst {

                       // =============================
                       // struct TestCustomizedTypeUtil
                       // =============================

struct TestCustomizedTypeUtil {
    // CLASS METHODS
    template <class VALUE_TYPE, class BASE_TYPE>
    static TestCustomizedType<VALUE_TYPE, BASE_TYPE> generate(
                                       const VALUE_TYPE&                 value,
                                       const TestPlaceHolder<BASE_TYPE>&);
};

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

                        // ----------------------------
                        // class TestCustomizedTypeUtil
                        // ----------------------------

// CLASS METHODS
template <class VALUE_TYPE, class BASE_TYPE>
TestCustomizedType<VALUE_TYPE, BASE_TYPE> TestCustomizedTypeUtil::generate(
                                       const VALUE_TYPE&                 value,
                                       const TestPlaceHolder<BASE_TYPE>&)
{
    return TestCustomizedType<VALUE_TYPE, BASE_TYPE>(value);
}

}  // close package namespace
}  // close enterprise namespace

#endif // INCLUDED_S_BALTST_TESTCUSTOMIZEDTYPEUTIL

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
