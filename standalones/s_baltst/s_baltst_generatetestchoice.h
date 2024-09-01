// s_baltst_generatetestchoice.h                                      -*-C++-*-
#ifndef INCLUDED_S_BALTST_GENERATETESTCHOICE
#define INCLUDED_S_BALTST_GENERATETESTCHOICE

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a component for generating test choice types.
//
//@CLASSES:
//  s_baltst::GenerateTestChoice: generate test choice
//  s_baltst::GenerateTestChoicePlaceHolder: generate test choice placeholder

#include <s_baltst_testchoice.h>
#include <s_baltst_testchoiceutil.h>
#include <s_baltst_testselection.h>

#include <bslmf_assert.h>

namespace BloombergLP {
namespace s_baltst {

                          // ========================
                          // class GenerateTestChoice
                          // ========================

/// This in-core value-semantic class provides a function object whose
/// function call operator can be used to generate objects of
/// specializations of `TestChoice`.
class GenerateTestChoice {

  public:
    // TYPES
    typedef TestChoiceUtil Util;

    // CREATORS
    GenerateTestChoice();

    // ACCESSORS
    template <class SELECTION_0, class TYPE_0>
    TestChoice<TypedTestSelection<TYPE_0, SELECTION_0> > operator()(
                                               const SELECTION_0& selection,
                                               const TYPE_0&      value) const;

    template <class SELECTION_0, class SELECTION_1, class TYPE_0, class TYPE_1>
    TestChoice<TypedTestSelection<TYPE_0, SELECTION_0>,
               TypedTestSelection<TYPE_1, SELECTION_1> >
    operator()(const SELECTION_0&             selection0,
               const SELECTION_1&             selection1,
               const TYPE_0&                  value,
               const TestPlaceHolder<TYPE_1>& placeHolder) const;

    template <class SELECTION_0, class SELECTION_1, class TYPE_0, class TYPE_1>
    TestChoice<TypedTestSelection<TYPE_0, SELECTION_0>,
               TypedTestSelection<TYPE_1, SELECTION_1> >
    operator()(const SELECTION_0&             selection0,
               const SELECTION_1&             selection1,
               const TestPlaceHolder<TYPE_0>& placeHolder,
               const TYPE_1&                  value) const;
};

                    // ===================================
                    // class GenerateTestChoicePlaceHolder
                    // ===================================

/// This in-core value-semantic class provides a function object whose
/// function call operator can be used to generate objects of
/// specializations of `TestChoice`.
class GenerateTestChoicePlaceHolder {

  public:
    // TYPES
    typedef TestChoiceUtil Util;

    // CREATORS
    GenerateTestChoicePlaceHolder();

    // ACCESSORS
    template <class SELECTION_0, class TYPE_0>
    TestPlaceHolder<TestChoice<TypedTestSelection<TYPE_0, SELECTION_0> > >
    operator()(const SELECTION_0&, const TestPlaceHolder<TYPE_0>&) const;

    template <class SELECTION_0, class SELECTION_1, class TYPE_0, class TYPE_1>
    TestPlaceHolder<TestChoice<TypedTestSelection<TYPE_0, SELECTION_0>,
                               TypedTestSelection<TYPE_1, SELECTION_1> > >
    operator()(const SELECTION_0&,
               const SELECTION_1&,
               const TestPlaceHolder<TYPE_0>&,
               const TestPlaceHolder<TYPE_1>&) const;
};


// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

                          // ------------------------
                          // class GenerateTestChoice
                          // ------------------------

// CREATORS
inline
GenerateTestChoice::GenerateTestChoice()
{
}

// ACCESSORS
template <class SELECTION_0, class TYPE_0>
TestChoice<TypedTestSelection<TYPE_0, SELECTION_0> >
GenerateTestChoice::operator()(const SELECTION_0& selection,
                               const TYPE_0&      value) const
{
    return Util::generate(selection, value);
}

template <class SELECTION_0, class SELECTION_1, class TYPE_0, class TYPE_1>
TestChoice<TypedTestSelection<TYPE_0, SELECTION_0>,
           TypedTestSelection<TYPE_1, SELECTION_1> >
GenerateTestChoice::
operator()(const SELECTION_0&             selection0,
           const SELECTION_1&             selection1,
           const TYPE_0&                  value,
           const TestPlaceHolder<TYPE_1>& placeHolder) const
{
    return Util::generate(selection0, selection1, value, placeHolder);
}

template <class SELECTION_0, class SELECTION_1, class TYPE_0, class TYPE_1>
TestChoice<TypedTestSelection<TYPE_0, SELECTION_0>,
           TypedTestSelection<TYPE_1, SELECTION_1> >
GenerateTestChoice::operator()(const SELECTION_0&             selection0,
                               const SELECTION_1&             selection1,
                               const TestPlaceHolder<TYPE_0>& placeHolder,
                               const TYPE_1&                  value) const
{
    return Util::generate(selection0, selection1, placeHolder, value);
}

                    // -----------------------------------
                    // class GenerateTestChoicePlaceHolder
                    // -----------------------------------

// CREATORS
inline
GenerateTestChoicePlaceHolder::GenerateTestChoicePlaceHolder()
{
}

// ACCESSORS
template <class SELECTION_0, class TYPE_0>
TestPlaceHolder<TestChoice<TypedTestSelection<TYPE_0, SELECTION_0> > >
GenerateTestChoicePlaceHolder::operator()(const SELECTION_0&,
                                          const TestPlaceHolder<TYPE_0>&) const
{
    typedef TypedTestSelection<TYPE_0, SELECTION_0> Selection0;
    typedef TestChoice<Selection0>                  Choice;

    return TestPlaceHolder<Choice>();
}

template <class SELECTION_0, class SELECTION_1, class TYPE_0, class TYPE_1>
TestPlaceHolder<TestChoice<TypedTestSelection<TYPE_0, SELECTION_0>,
                           TypedTestSelection<TYPE_1, SELECTION_1> > >
GenerateTestChoicePlaceHolder::operator()(const SELECTION_0&,
                                          const SELECTION_1&,
                                          const TestPlaceHolder<TYPE_0>&,
                                          const TestPlaceHolder<TYPE_1>&) const
{
    typedef TypedTestSelection<TYPE_0, SELECTION_0> Selection0;
    typedef TypedTestSelection<TYPE_1, SELECTION_1> Selection1;
    typedef TestChoice<Selection0, Selection1>      Choice;

    return TestPlaceHolder<Choice>();
}

}  // close package namespace
}  // close enterprise namespace

#endif // INCLUDED_S_BALTST_GENERATETESTCHOICE

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
