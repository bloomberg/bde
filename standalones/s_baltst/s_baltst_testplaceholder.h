// s_baltst_testplaceholder.h                                         -*-C++-*-
#ifndef INCLUDED_S_BALTST_TESTPLACEHOLDER
#define INCLUDED_S_BALTST_TESTPLACEHOLDER

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a value-level representation of a type for testing.
//
//@CLASSES:
//  s_baltst::TestPlaceHolder: value-level representation of a type

namespace BloombergLP {
namespace s_baltst {

                           // =====================
                           // class TestPlaceHolder
                           // =====================

template <class TYPE>
class TestPlaceHolder {
    // This class provides subset of the semantics of an in-core value-semantic
    // type.  It is intended to be used to guide template (type) argument
    // deduction in function invocation expressions, where it may not make
    // sense for a value of the type to be supplied to guide the deduction, and
    // where specifying a template argument may require more characters than a
    // function argument.

  public:
    // CREATORS
    TestPlaceHolder();
};

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

                           // ---------------------
                           // class TestPlaceHolder
                           // ---------------------

template <class TYPE>
TestPlaceHolder<TYPE>::TestPlaceHolder()
{
}

}  // close package namespace
}  // close enterprise namespace

#endif // INCLUDED_S_BALTST_TESTPLACEHOLDER

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
