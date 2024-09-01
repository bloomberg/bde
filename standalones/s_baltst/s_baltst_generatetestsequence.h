// s_baltst_generatetestsequence.h                                    -*-C++-*-
#ifndef INCLUDED_S_BALTST_GENERATETESTSEQUENCE
#define INCLUDED_S_BALTST_GENERATETESTSEQUENCE

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a component for generating test sequence types.
//
//@CLASSES:
//  s_baltst::GenerateTestSequence: generate test sequence
//  s_baltst::GenerateTestSequencePlaceHolder: generate test sequence p-holder

#include <s_baltst_testattribute.h>
#include <s_baltst_testplaceholder.h>
#include <s_baltst_testsequence.h>
#include <s_baltst_testsequenceutil.h>

namespace BloombergLP {
namespace s_baltst {

                         // ==========================
                         // class GenerateTestSequence
                         // ==========================

/// This class provides a function object whose function call operator can
/// be used to generate objects of `TestSequence` specializations.
class GenerateTestSequence {

  public:
    // TYPES
    typedef TestSequenceUtil Util;

    // CREATORS
    GenerateTestSequence();

    // ACCESSORS
    template <class ATTRIBUTE_0, class TYPE_0>
    TestSequence<TypedTestAttribute<TYPE_0, ATTRIBUTE_0> > operator()(
                                               const ATTRIBUTE_0& attribute,
                                               const TYPE_0&      value) const;

    template <class ATTRIBUTE_0, class ATTRIBUTE_1, class TYPE_0, class TYPE_1>
    TestSequence<TypedTestAttribute<TYPE_0, ATTRIBUTE_0>,
                 TypedTestAttribute<TYPE_1, ATTRIBUTE_1> >
    operator()(const ATTRIBUTE_0& attribute0,
               const ATTRIBUTE_1& attribute1,
               const TYPE_0&      value0,
               const TYPE_1&      value1) const;

    template <class ATTRIBUTE_0,
              class ATTRIBUTE_1,
              class ATTRIBUTE_2,
              class TYPE_0,
              class TYPE_1,
              class TYPE_2>
    TestSequence<TypedTestAttribute<TYPE_0, ATTRIBUTE_0>,
                 TypedTestAttribute<TYPE_1, ATTRIBUTE_1>,
                 TypedTestAttribute<TYPE_2, ATTRIBUTE_2> >
    operator()(const ATTRIBUTE_0& attribute0,
               const ATTRIBUTE_1& attribute1,
               const ATTRIBUTE_2& attribute2,
               const TYPE_0&      value0,
               const TYPE_1&      value1,
               const TYPE_2&      value2) const;
};

                   // =====================================
                   // class GenerateTestSequencePlaceHolder
                   // =====================================

/// This class provides a function object whose function call operator can
/// be used to generate `PlaceHolder` specializations for `TestSequence`
/// types.
class GenerateTestSequencePlaceHolder {

  public:
    // TYPES
    typedef TestSequenceUtil Util;

    // CREATORS
    GenerateTestSequencePlaceHolder();

    // ACCESSORS
    template <class ATTRIBUTE_0, class TYPE_0>
    TestPlaceHolder<TestSequence<TypedTestAttribute<TYPE_0, ATTRIBUTE_0> > >
    operator()(const ATTRIBUTE_0&             attribute,
               const TestPlaceHolder<TYPE_0>& value) const;

    template <class ATTRIBUTE_0, class ATTRIBUTE_1, class TYPE_0, class TYPE_1>
    TestPlaceHolder<TestSequence<TypedTestAttribute<TYPE_0, ATTRIBUTE_0>,
                                 TypedTestAttribute<TYPE_1, ATTRIBUTE_1> > >
    operator()(const ATTRIBUTE_0&             attribute0,
               const ATTRIBUTE_1&             attribute1,
               const TestPlaceHolder<TYPE_0>& value0,
               const TestPlaceHolder<TYPE_1>& value1) const;

    template <class ATTRIBUTE_0,
              class ATTRIBUTE_1,
              class ATTRIBUTE_2,
              class TYPE_0,
              class TYPE_1,
              class TYPE_2>
    TestPlaceHolder<TestSequence<TypedTestAttribute<TYPE_0, ATTRIBUTE_0>,
                                 TypedTestAttribute<TYPE_1, ATTRIBUTE_1>,
                                 TypedTestAttribute<TYPE_2, ATTRIBUTE_2> > >
    operator()(const ATTRIBUTE_0&             attribute0,
               const ATTRIBUTE_1&             attribute1,
               const ATTRIBUTE_2&             attribute2,
               const TestPlaceHolder<TYPE_0>& value0,
               const TestPlaceHolder<TYPE_1>& value1,
               const TestPlaceHolder<TYPE_2>& value2) const;
};

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

                         // --------------------------
                         // class GenerateTestSequence
                         // --------------------------

// CREATORS
GenerateTestSequence::GenerateTestSequence()
{
}

// ACCESSORS
template <class ATTRIBUTE_0, class TYPE_0>
TestSequence<TypedTestAttribute<TYPE_0, ATTRIBUTE_0> >
GenerateTestSequence::operator()(const ATTRIBUTE_0& attribute,
                                 const TYPE_0&      value) const
{
    return Util::generate(attribute, value);
}

template <class ATTRIBUTE_0, class ATTRIBUTE_1, class TYPE_0, class TYPE_1>
TestSequence<TypedTestAttribute<TYPE_0, ATTRIBUTE_0>,
             TypedTestAttribute<TYPE_1, ATTRIBUTE_1> >
GenerateTestSequence::operator()(const ATTRIBUTE_0& attribute0,
                                 const ATTRIBUTE_1& attribute1,
                                 const TYPE_0&      value0,
                                 const TYPE_1&      value1) const
{
    return Util::generate(attribute0, attribute1, value0, value1);
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
GenerateTestSequence::operator()(const ATTRIBUTE_0& attribute0,
                                 const ATTRIBUTE_1& attribute1,
                                 const ATTRIBUTE_2& attribute2,
                                 const TYPE_0&      value0,
                                 const TYPE_1&      value1,
                                 const TYPE_2&      value2) const
{
    return Util::generate(attribute0,
                          attribute1,
                          attribute2,
                          value0,
                          value1,
                          value2);
}

                   // -------------------------------------
                   // class GenerateTestSequencePlaceHolder
                   // -------------------------------------

// CREATORS
GenerateTestSequencePlaceHolder::GenerateTestSequencePlaceHolder()
{
}

// ACCESSORS
template <class ATTRIBUTE_0, class TYPE_0>
TestPlaceHolder<TestSequence<TypedTestAttribute<TYPE_0, ATTRIBUTE_0> > >
GenerateTestSequencePlaceHolder::operator()(
                                    const ATTRIBUTE_0&             attribute,
                                    const TestPlaceHolder<TYPE_0>& value) const
{
    return Util::generatePlaceHolder(attribute, value);
}

template <class ATTRIBUTE_0, class ATTRIBUTE_1, class TYPE_0, class TYPE_1>
TestPlaceHolder<TestSequence<TypedTestAttribute<TYPE_0, ATTRIBUTE_0>,
                             TypedTestAttribute<TYPE_1, ATTRIBUTE_1> > >
GenerateTestSequencePlaceHolder::operator()(
                                   const ATTRIBUTE_0&             attribute0,
                                   const ATTRIBUTE_1&             attribute1,
                                   const TestPlaceHolder<TYPE_0>& value0,
                                   const TestPlaceHolder<TYPE_1>& value1) const
{
    return Util::generatePlaceHolder(attribute0, attribute1, value0, value1);
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
GenerateTestSequencePlaceHolder::operator()(
                                   const ATTRIBUTE_0&             attribute0,
                                   const ATTRIBUTE_1&             attribute1,
                                   const ATTRIBUTE_2&             attribute2,
                                   const TestPlaceHolder<TYPE_0>& value0,
                                   const TestPlaceHolder<TYPE_1>& value1,
                                   const TestPlaceHolder<TYPE_2>& value2) const
{
    return Util::generatePlaceHolder(attribute0,
                                     attribute1,
                                     attribute2,
                                     value0,
                                     value1,
                                     value2);
}

}  // close package namespace
}  // close enterprise namespace

#endif  // INCLUDED_S_BALTST_GENERATETESTSEQUENCE

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
