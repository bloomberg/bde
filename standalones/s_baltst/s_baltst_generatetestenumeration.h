// s_baltst_generatetestenumeration.h                                 -*-C++-*-
#ifndef INCLUDED_S_BALTST_GENERATETESTENUMERATION
#define INCLUDED_S_BALTST_GENERATETESTENUMERATION

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a component for generating test enumeration types.
//
//@CLASSES:
//  s_baltst::GenerateTestEnumeration: generate test enumeration
//  s_baltst::GenerateTestEnumerationPlaceHolder: gen test enum placeholder

#include <s_baltst_testenumerator.h>
#include <s_baltst_testenumeration.h>
#include <s_baltst_testenumerationutil.h>
#include <s_baltst_testplaceholder.h>

namespace BloombergLP {
namespace s_baltst {

                       // =============================
                       // class GenerateTestEnumeration
                       // =============================

/// This in-core value-semantic class provides a function object whose
/// function call operator can be used to generate `TestEnumeration`
/// objects.
class GenerateTestEnumeration {

  public:
    // TYPES
    typedef TestEnumerationUtil Util;

    // CREATORS
    GenerateTestEnumeration();

    // ACCESSORS
    template <class ENUMERATOR_0>
    TestEnumeration<ENUMERATOR_0> operator()(const ENUMERATOR_0& enumerator,
                                             int                 value) const;

    template <class ENUMERATOR_0, class ENUMERATOR_1>
    TestEnumeration<ENUMERATOR_0, ENUMERATOR_1> operator()(
                                              const ENUMERATOR_0& enumerator0,
                                              const ENUMERATOR_1& enumerator1,
                                              int                 value) const;
};

                  // ========================================
                  // class GenerateTestEnumerationPlaceHolder
                  // ========================================

/// This in-core value-semantic class provides a function object whose
/// function call operator can be used to generate `PlaceHolder`
/// specializations for `TestEnumeration` types.
class GenerateTestEnumerationPlaceHolder {

  public:
    // TYPES
    typedef TestEnumerationUtil Util;

    // CREATORS
    GenerateTestEnumerationPlaceHolder();

    // ACCESSORS
    template <class ENUMERATOR_0>
    TestPlaceHolder<TestEnumeration<ENUMERATOR_0> > operator()(
                                         const ENUMERATOR_0& enumerator) const;

    template <class ENUMERATOR_0, class ENUMERATOR_1>
    TestPlaceHolder<TestEnumeration<ENUMERATOR_0, ENUMERATOR_1> > operator()(
                                        const ENUMERATOR_0& enumerator0,
                                        const ENUMERATOR_1& enumerator1) const;
};

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

                       // -----------------------------
                       // class GenerateTestEnumeration
                       // -----------------------------

// CREATORS
GenerateTestEnumeration::GenerateTestEnumeration()
{
}

// ACCESSORS
template <class ENUMERATOR_0>
TestEnumeration<ENUMERATOR_0> GenerateTestEnumeration::operator()(
                                               const ENUMERATOR_0& enumerator,
                                               int                 value) const
{
    return Util::generate(enumerator, value);
}

template <class ENUMERATOR_0, class ENUMERATOR_1>
TestEnumeration<ENUMERATOR_0, ENUMERATOR_1>
GenerateTestEnumeration::operator()(const ENUMERATOR_0& enumerator0,
                                    const ENUMERATOR_1& enumerator1,
                                    int                 value) const
{
    return Util::generate(enumerator0, enumerator1, value);
}

                  // ----------------------------------------
                  // class GenerateTestEnumerationPlaceHolder
                  // ----------------------------------------

// CREATORS
GenerateTestEnumerationPlaceHolder::GenerateTestEnumerationPlaceHolder()
{
}

// ACCESSORS
template <class ENUMERATOR_0>
TestPlaceHolder<TestEnumeration<ENUMERATOR_0> >
GenerateTestEnumerationPlaceHolder::operator()(
                                          const ENUMERATOR_0& enumerator) const
{
    return Util::generatePlaceHolder(enumerator);
}

template <class ENUMERATOR_0, class ENUMERATOR_1>
TestPlaceHolder<TestEnumeration<ENUMERATOR_0, ENUMERATOR_1> >
GenerateTestEnumerationPlaceHolder::operator()(
                                         const ENUMERATOR_0& enumerator0,
                                         const ENUMERATOR_1& enumerator1) const
{
    return Util::generatePlaceHolder(enumerator0, enumerator1);
}

}  // close package namespace
}  // close enterprise namespace

#endif // INCLUDED_S_BALTST_GENERATETESTENUMERATION

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
