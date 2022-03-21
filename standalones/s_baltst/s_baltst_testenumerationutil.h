// s_baltst_testenumerationutil.h                                     -*-C++-*-
#ifndef INCLUDED_S_BALTST_TESTENUMERATIONUTIL
#define INCLUDED_S_BALTST_TESTENUMERATIONUTIL

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide non-primitive operations on 's_baltst::TestEnumeration'.
//
//@CLASSES:
//  s_baltst::TestEnumerationUtil: utilities for test enumerations

#include <s_baltst_testenumerator.h>
#include <s_baltst_testenumeration.h>
#include <s_baltst_testplaceholder.h>

namespace BloombergLP {
namespace s_baltst {

                         // ==========================
                         // struct TestEnumerationUtil
                         // ==========================

struct TestEnumerationUtil {
    // CLASS METHODS
    template <class ENUMERATOR_0>
    static TestEnumeration<ENUMERATOR_0> generate(const ENUMERATOR_0&,
                                                  int                 value);

    template <class ENUMERATOR_0, class ENUMERATOR_1>
    static TestEnumeration<ENUMERATOR_0, ENUMERATOR_1> generate(
                                                    const ENUMERATOR_0&,
                                                    const ENUMERATOR_1&,
                                                    int                 value);

    template <class ENUMERATOR_0>
    static TestPlaceHolder<TestEnumeration<ENUMERATOR_0> > generatePlaceHolder(
                                                          const ENUMERATOR_0&);

    template <class ENUMERATOR_0, class ENUMERATOR_1>
    static TestPlaceHolder<TestEnumeration<ENUMERATOR_0, ENUMERATOR_1> >
    generatePlaceHolder(const ENUMERATOR_0&, const ENUMERATOR_1&);
};

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

                         // --------------------------
                         // struct TestEnumerationUtil
                         // --------------------------

// CLASS METHODS
template <class ENUMERATOR_0>
TestEnumeration<ENUMERATOR_0> TestEnumerationUtil::generate(
                                                     const ENUMERATOR_0&,
                                                     int                 value)
{
    return TestEnumeration<ENUMERATOR_0>(value);
}

template <class ENUMERATOR_0, class ENUMERATOR_1>
TestEnumeration<ENUMERATOR_0, ENUMERATOR_1> TestEnumerationUtil::generate(
                                                     const ENUMERATOR_0&,
                                                     const ENUMERATOR_1&,
                                                     int                 value)
{
    return TestEnumeration<ENUMERATOR_0, ENUMERATOR_1>(value);
}

template <class ENUMERATOR_0>
TestPlaceHolder<TestEnumeration<ENUMERATOR_0> >
TestEnumerationUtil::generatePlaceHolder(const ENUMERATOR_0&)
{
    typedef TestEnumeration<ENUMERATOR_0> Enumeration;

    return TestPlaceHolder<Enumeration>();
}

template <class ENUMERATOR_0, class ENUMERATOR_1>
TestPlaceHolder<TestEnumeration<ENUMERATOR_0, ENUMERATOR_1> >
TestEnumerationUtil::generatePlaceHolder(const ENUMERATOR_0&,
                                         const ENUMERATOR_1&)
{
    typedef TestEnumeration<ENUMERATOR_0, ENUMERATOR_1> Enumeration;

    return TestPlaceHolder<Enumeration>();
}

}  // close package namespace
}  // close enterprise namespace

#endif // INCLUDED_S_BALTST_TESTENUMERATIONUTIL

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
