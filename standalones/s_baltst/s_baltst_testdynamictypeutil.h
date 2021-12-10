// s_baltst_testdynamictypeutil.h                                     -*-C++-*-
#ifndef INCLUDED_S_BALTST_TESTDYNAMICTYPEUTIL
#define INCLUDED_S_BALTST_TESTDYNAMICTYPEUTIL

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide non-primitive operations on 's_baltst::TestDynamicType'.
//
//@CLASSES:
//  s_baltst::TestDynamicTypeUtil: utilities for test dynamic types

#include <s_baltst_testdynamictype.h>

namespace BloombergLP {
namespace s_baltst {

                         // ==========================
                         // struct TestDynamicTypeUtil
                         // ==========================

struct TestDynamicTypeUtil {
    // CLASS METHODS
    template <class VALUE_TYPE>
    static TestDynamicType<VALUE_TYPE> generateDynamicType(
                                                      const VALUE_TYPE& value);
};

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

                         // --------------------------
                         // struct TestDynamicTypeUtil
                         // --------------------------

// CLASS METHODS
template <class VALUE_TYPE>
TestDynamicType<VALUE_TYPE> TestDynamicTypeUtil::generateDynamicType(
                                                       const VALUE_TYPE& value)
{
    return TestDynamicType<VALUE_TYPE>(value);
}

}  // close package namespace
}  // close enterprise namespace

#endif // INCLUDED_S_BALTST_TESTDYNAMICTYPEUTIL

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
