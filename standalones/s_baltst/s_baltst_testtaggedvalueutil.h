// s_baltst_testtaggedvalueutil.h                                     -*-C++-*-
#ifndef INCLUDED_S_BALTST_TESTTAGGEDVALUEUTIL
#define INCLUDED_S_BALTST_TESTTAGGEDVALUEUTIL

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide non-primitive operations on 's_baltst::TestTaggedValue'.
//
//@CLASSES:
//  s_baltst::TestTaggedValueUtil: utilities for test tagged values

#include <s_baltst_testplaceholder.h>
#include <s_baltst_testtaggedvalue.h>

namespace BloombergLP {
namespace s_baltst {

                         // ==========================
                         // struct TestTaggedValueUtil
                         // ==========================

struct TestTaggedValueUtil {
    // CLASS METHODS
    template <class TAG_TYPE, class VALUE_TYPE>
    static TestTaggedValue<TAG_TYPE, VALUE_TYPE> generate(
                                       const TestPlaceHolder<TAG_TYPE>&,
                                       const VALUE_TYPE&                value);

    template <class TAG_TYPE, class VALUE_TYPE>
    static TestPlaceHolder<TestTaggedValue<TAG_TYPE, VALUE_TYPE> >
    generatePlaceHolder(const TestPlaceHolder<TAG_TYPE>&,
                        const TestPlaceHolder<VALUE_TYPE>&);
};

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

                         // --------------------------
                         // struct TestTaggedValueUtil
                         // --------------------------

// CLASS METHODS
template <class TAG_TYPE, class VALUE_TYPE>
TestTaggedValue<TAG_TYPE, VALUE_TYPE> TestTaggedValueUtil::generate(
                                        const TestPlaceHolder<TAG_TYPE>&,
                                        const VALUE_TYPE&                value)
{
    return TestTaggedValue<TAG_TYPE, VALUE_TYPE>(value);
}

template <class TAG_TYPE, class VALUE_TYPE>
TestPlaceHolder<TestTaggedValue<TAG_TYPE, VALUE_TYPE> >
TestTaggedValueUtil::generatePlaceHolder(const TestPlaceHolder<TAG_TYPE>&,
                                         const TestPlaceHolder<VALUE_TYPE>&)
{
    typedef TestTaggedValue<TAG_TYPE, VALUE_TYPE> TaggedValue;

    return TestPlaceHolder<TaggedValue>();
}

}  // close package namespace
}  // close enterprise namespace

#endif // INCLUDED_S_BALTST_TESTTAGGEDVALUEUTIL

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
