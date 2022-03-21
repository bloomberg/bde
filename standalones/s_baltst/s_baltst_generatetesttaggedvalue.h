// s_baltst_generatetesttaggedvalue.h                                 -*-C++-*-
#ifndef INCLUDED_S_BALTST_GENERATETESTTAGGEDVALUE
#define INCLUDED_S_BALTST_GENERATETESTTAGGEDVALUE

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a component for generating test tagged value types.
//
//@CLASSES:
//  s_baltst::GenerateTestTaggedValue: generate test tagged value
//  s_baltst::GenerateTestTaggedValuePlaceHolder: gen tagged value placeholder

#include <s_baltst_testplaceholder.h>
#include <s_baltst_testtaggedvalue.h>
#include <s_baltst_testtaggedvalueutil.h>

namespace BloombergLP {
namespace s_baltst {

                       // =============================
                       // class GenerateTestTaggedValue
                       // =============================

class GenerateTestTaggedValue {
  public:
    // TYPES
    typedef TestTaggedValueUtil Util;

    // CREATORS
    GenerateTestTaggedValue();

    // ACCESSORS
    template <class TAG_TYPE, class VALUE_TYPE>
    TestTaggedValue<TAG_TYPE, VALUE_TYPE> operator()(
                                 const TestPlaceHolder<TAG_TYPE>& tag,
                                 const VALUE_TYPE&                value) const;
};

                  // ========================================
                  // class GenerateTestTaggedValuePlaceHolder
                  // ========================================

class GenerateTestTaggedValuePlaceHolder {
  public:
    // TYPES
    typedef TestTaggedValueUtil Util;

    // CREATORS
    GenerateTestTaggedValuePlaceHolder();

    // ACCESSORS
    template <class TAG_TYPE, class VALUE_TYPE>
    TestPlaceHolder<TestTaggedValue<TAG_TYPE, VALUE_TYPE> > operator()(
                               const TestPlaceHolder<TAG_TYPE>&   tag,
                               const TestPlaceHolder<VALUE_TYPE>& value) const;
};

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

                       // -----------------------------
                       // class GenerateTestTaggedValue
                       // -----------------------------

// CREATORS
inline
GenerateTestTaggedValue::GenerateTestTaggedValue()
{
}

// ACCESSORS
template <class TAG_TYPE, class VALUE_TYPE>
TestTaggedValue<TAG_TYPE, VALUE_TYPE> GenerateTestTaggedValue::operator()(
                                  const TestPlaceHolder<TAG_TYPE>& tag,
                                  const VALUE_TYPE&                value) const
{
    return Util::generate(tag, value);
}

                  // ----------------------------------------
                  // class GenerateTestTaggedValuePlaceHolder
                  // ----------------------------------------

// CREATORS
inline
GenerateTestTaggedValuePlaceHolder::GenerateTestTaggedValuePlaceHolder()
{
}

// ACCESSORS
template <class TAG_TYPE, class VALUE_TYPE>
TestPlaceHolder<TestTaggedValue<TAG_TYPE, VALUE_TYPE> >
GenerateTestTaggedValuePlaceHolder::operator()(
                                const TestPlaceHolder<TAG_TYPE>&   tag,
                                const TestPlaceHolder<VALUE_TYPE>& value) const
{
    return Util::generatePlaceHolder(tag, value);
}

}  // close package namespace
}  // close enterprise namespace

#endif // INCLUDED_S_BALTST_GENERATETESTTAGGEDVALUE

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
