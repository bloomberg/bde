// s_baltst_testsequenceutil.h                                        -*-C++-*-
#ifndef INCLUDED_S_BALTST_TESTSEQUENCEUTIL
#define INCLUDED_S_BALTST_TESTSEQUENCEUTIL

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide non-primitive operations on 's_baltst::TestSequence'.
//
//@CLASSES:
//  s_baltst::TestSequenceUtil: utilities for test sequences

#include <s_baltst_testattribute.h>
#include <s_baltst_testplaceholder.h>
#include <s_baltst_testsequence.h>

namespace BloombergLP {
namespace s_baltst {

                          // =======================
                          // struct TestSequenceUtil
                          // =======================

struct TestSequenceUtil {
    // CLASS METHODS
    template <class ATTRIBUTE_0, class TYPE_0>
    static TestSequence<TypedTestAttribute<TYPE_0, ATTRIBUTE_0> > generate(
                                                     const ATTRIBUTE_0&,
                                                     const TYPE_0&      value);

    template <class ATTRIBUTE_0, class ATTRIBUTE_1, class TYPE_0, class TYPE_1>
    static TestSequence<TypedTestAttribute<TYPE_0, ATTRIBUTE_0>,
                        TypedTestAttribute<TYPE_1, ATTRIBUTE_1> >
    generate(const ATTRIBUTE_0&,
             const ATTRIBUTE_1&,
             const TYPE_0& value0,
             const TYPE_1& value1);

    template <class ATTRIBUTE_0,
              class ATTRIBUTE_1,
              class ATTRIBUTE_2,
              class TYPE_0,
              class TYPE_1,
              class TYPE_2>
    static TestSequence<TypedTestAttribute<TYPE_0, ATTRIBUTE_0>,
                        TypedTestAttribute<TYPE_1, ATTRIBUTE_1>,
                        TypedTestAttribute<TYPE_2, ATTRIBUTE_2> >
    generate(const ATTRIBUTE_0&,
             const ATTRIBUTE_1&,
             const ATTRIBUTE_2&,
             const TYPE_0& value0,
             const TYPE_1& value1,
             const TYPE_2& value2);

    template <class ATTRIBUTE_0, class TYPE_0>
    static
    TestPlaceHolder<TestSequence<TypedTestAttribute<TYPE_0, ATTRIBUTE_0> > >
    generatePlaceHolder(const ATTRIBUTE_0&, const TestPlaceHolder<TYPE_0>&);

    template <class ATTRIBUTE_0, class ATTRIBUTE_1, class TYPE_0, class TYPE_1>
    static TestPlaceHolder<
        TestSequence<TypedTestAttribute<TYPE_0, ATTRIBUTE_0>,
                     TypedTestAttribute<TYPE_1, ATTRIBUTE_1> > >
    generatePlaceHolder(const ATTRIBUTE_0&,
                        const ATTRIBUTE_1&,
                        const TestPlaceHolder<TYPE_0>&,
                        const TestPlaceHolder<TYPE_1>&);

    template <class ATTRIBUTE_0,
              class ATTRIBUTE_1,
              class ATTRIBUTE_2,
              class TYPE_0,
              class TYPE_1,
              class TYPE_2>
    static TestPlaceHolder<
        TestSequence<TypedTestAttribute<TYPE_0, ATTRIBUTE_0>,
                     TypedTestAttribute<TYPE_1, ATTRIBUTE_1>,
                     TypedTestAttribute<TYPE_2, ATTRIBUTE_2> > >
    generatePlaceHolder(const ATTRIBUTE_0&,
                        const ATTRIBUTE_1&,
                        const ATTRIBUTE_2&,
                        const TestPlaceHolder<TYPE_0>&,
                        const TestPlaceHolder<TYPE_1>&,
                        const TestPlaceHolder<TYPE_2>&);
};

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

                           // ----------------------
                           // class TestSequenceUtil
                           // ----------------------

// CLASS METHODS
template <class ATTRIBUTE_0, class TYPE_0>
TestSequence<TypedTestAttribute<TYPE_0, ATTRIBUTE_0> >
TestSequenceUtil::generate(const ATTRIBUTE_0&, const TYPE_0& value)
{
    typedef TypedTestAttribute<TYPE_0, ATTRIBUTE_0> Attribute0;

    return TestSequence<Attribute0>(value);
}

template <class ATTRIBUTE_0, class ATTRIBUTE_1, class TYPE_0, class TYPE_1>
TestSequence<TypedTestAttribute<TYPE_0, ATTRIBUTE_0>,
             TypedTestAttribute<TYPE_1, ATTRIBUTE_1> >
TestSequenceUtil::generate(const ATTRIBUTE_0&,
                           const ATTRIBUTE_1&,
                           const TYPE_0& value0,
                           const TYPE_1& value1)
{
    typedef TypedTestAttribute<TYPE_0, ATTRIBUTE_0> Attribute0;
    typedef TypedTestAttribute<TYPE_1, ATTRIBUTE_1> Attribute1;

    return TestSequence<Attribute0, Attribute1>(value0, value1);
}

template <class ATTRIBUTE_0,
          class ATTRIBUTE_1,
          class ATTRIBUTE_2,
          class TYPE_0,
          class TYPE_1,
          class TYPE_2>
TestSequence<TypedTestAttribute<TYPE_0, ATTRIBUTE_0>,
             TypedTestAttribute<TYPE_1, ATTRIBUTE_1>,
             TypedTestAttribute<TYPE_2, ATTRIBUTE_2> >
TestSequenceUtil::generate(const ATTRIBUTE_0&,
                           const ATTRIBUTE_1&,
                           const ATTRIBUTE_2&,
                           const TYPE_0& value0,
                           const TYPE_1& value1,
                           const TYPE_2& value2)
{
    typedef TypedTestAttribute<TYPE_0, ATTRIBUTE_0> Attribute0;
    typedef TypedTestAttribute<TYPE_1, ATTRIBUTE_1> Attribute1;
    typedef TypedTestAttribute<TYPE_2, ATTRIBUTE_2> Attribute2;

    return TestSequence<Attribute0, Attribute1, Attribute2>(value0,
                                                            value1,
                                                            value2);
}

template <class ATTRIBUTE_0, class TYPE_0>
TestPlaceHolder<TestSequence<TypedTestAttribute<TYPE_0, ATTRIBUTE_0> > >
TestSequenceUtil::generatePlaceHolder(const ATTRIBUTE_0&,
                                      const TestPlaceHolder<TYPE_0>&)
{
    typedef TypedTestAttribute<TYPE_0, ATTRIBUTE_0> Attribute0;
    typedef TestSequence<Attribute0>                Sequence;

    return TestPlaceHolder<Sequence>();
}

template <class ATTRIBUTE_0, class ATTRIBUTE_1, class TYPE_0, class TYPE_1>
TestPlaceHolder<TestSequence<TypedTestAttribute<TYPE_0, ATTRIBUTE_0>,
                             TypedTestAttribute<TYPE_1, ATTRIBUTE_1> > >
TestSequenceUtil::generatePlaceHolder(const ATTRIBUTE_0&,
                                      const ATTRIBUTE_1&,
                                      const TestPlaceHolder<TYPE_0>&,
                                      const TestPlaceHolder<TYPE_1>&)
{
    typedef TypedTestAttribute<TYPE_0, ATTRIBUTE_0> Attribute0;
    typedef TypedTestAttribute<TYPE_1, ATTRIBUTE_1> Attribute1;
    typedef TestSequence<Attribute0, Attribute1>    Sequence;

    return TestPlaceHolder<Sequence>();
}

template <class ATTRIBUTE_0,
          class ATTRIBUTE_1,
          class ATTRIBUTE_2,
          class TYPE_0,
          class TYPE_1,
          class TYPE_2>
TestPlaceHolder<TestSequence<TypedTestAttribute<TYPE_0, ATTRIBUTE_0>,
                             TypedTestAttribute<TYPE_1, ATTRIBUTE_1>,
                             TypedTestAttribute<TYPE_2, ATTRIBUTE_2> > >
TestSequenceUtil::generatePlaceHolder(const ATTRIBUTE_0&,
                                      const ATTRIBUTE_1&,
                                      const ATTRIBUTE_2&,
                                      const TestPlaceHolder<TYPE_0>&,
                                      const TestPlaceHolder<TYPE_1>&,
                                      const TestPlaceHolder<TYPE_2>&)
{
    typedef TypedTestAttribute<TYPE_0, ATTRIBUTE_0>          Attribute0;
    typedef TypedTestAttribute<TYPE_1, ATTRIBUTE_1>          Attribute1;
    typedef TypedTestAttribute<TYPE_2, ATTRIBUTE_2>          Attribute2;
    typedef TestSequence<Attribute0, Attribute1, Attribute2> Sequence;

    return TestPlaceHolder<Sequence>();
}

}  // close package namespace
}  // close enterprise namespace

#endif  // INCLUDED_S_BALTST_TESTSEQUENCEUTIL

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
