// s_baltst_testchoiceutil.h                                          -*-C++-*-
#ifndef INCLUDED_S_BALTST_TESTCHOICEUTIL
#define INCLUDED_S_BALTST_TESTCHOICEUTIL

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide non-primitive operations on 's_baltst::TestChoice'.
//
//@CLASSES:
//  s_baltst::TestChoiceUtil: utilities for test choices

#include <s_baltst_testchoice.h>
#include <s_baltst_testplaceholder.h>
#include <s_baltst_testselection.h>

namespace BloombergLP {
namespace s_baltst {

                           // =====================
                           // struct TestChoiceUtil
                           // =====================

struct TestChoiceUtil {
    // CLASS METHODS
    template <class SELECTION_0, class TYPE_0>
    static TestChoice<TypedTestSelection<TYPE_0, SELECTION_0> > generate(
                                                     const SELECTION_0&,
                                                     const TYPE_0&      value);

    template <class SELECTION_0, class SELECTION_1, class TYPE_0, class TYPE_1>
    static TestChoice<TypedTestSelection<TYPE_0, SELECTION_0>,
                      TypedTestSelection<TYPE_1, SELECTION_1> >
    generate(const SELECTION_0&,
             const SELECTION_1&,
             const TYPE_0& value,
             const TestPlaceHolder<TYPE_1>&);

    template <class SELECTION_0, class SELECTION_1, class TYPE_0, class TYPE_1>
    static TestChoice<TypedTestSelection<TYPE_0, SELECTION_0>,
                      TypedTestSelection<TYPE_1, SELECTION_1> >
    generate(const SELECTION_0&,
             const SELECTION_1&,
             const TestPlaceHolder<TYPE_0>&,
             const TYPE_1& value);
};

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

                            // --------------------
                            // class TestChoiceUtil
                            // --------------------

// CLASS METHODS
template <class SELECTION_0, class TYPE_0>
TestChoice<TypedTestSelection<TYPE_0, SELECTION_0> > TestChoiceUtil::generate(
                                                      const SELECTION_0&,
                                                      const TYPE_0&      value)
{
    typedef TypedTestSelection<TYPE_0, SELECTION_0> Selection0;

    return TestChoice<Selection0>(value);
}

template <class SELECTION_0, class SELECTION_1, class TYPE_0, class TYPE_1>
TestChoice<TypedTestSelection<TYPE_0, SELECTION_0>,
           TypedTestSelection<TYPE_1, SELECTION_1> >
TestChoiceUtil::generate(const SELECTION_0&,
                         const SELECTION_1&,
                         const TYPE_0& value,
                         const TestPlaceHolder<TYPE_1>&)
{
    typedef TypedTestSelection<TYPE_0, SELECTION_0> Selection0;
    typedef TypedTestSelection<TYPE_1, SELECTION_1> Selection1;

    return TestChoice<Selection0, Selection1>(value);
}

template <class SELECTION_0, class SELECTION_1, class TYPE_0, class TYPE_1>
TestChoice<TypedTestSelection<TYPE_0, SELECTION_0>,
           TypedTestSelection<TYPE_1, SELECTION_1> >
TestChoiceUtil::generate(const SELECTION_0&,
                         const SELECTION_1&,
                         const TestPlaceHolder<TYPE_0>&,
                         const TYPE_1& value)
{
    typedef TypedTestSelection<TYPE_0, SELECTION_0> Selection0;
    typedef TypedTestSelection<TYPE_1, SELECTION_1> Selection1;

    return TestChoice<Selection0, Selection1>(value);
}

}  // close package namespace
}  // close enterprise namespace

#endif // INCLUDED_S_BALTST_TESTCHOICEUTIL

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
