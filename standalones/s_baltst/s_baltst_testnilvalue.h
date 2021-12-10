// s_baltst_testnilvalue.h                                            -*-C++-*-
#ifndef INCLUDED_S_BALTST_TESTNILVALUE
#define INCLUDED_S_BALTST_TESTNILVALUE

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a singular, valueless, value-semantic type for testing.
//
//@CLASSES:
//  s_baltst::TestNilValue: singular value type for testing

#include <bsl_ostream.h>

namespace BloombergLP {
namespace s_baltst {

                             // ==================
                             // class TestNilValue
                             // ==================

class TestNilValue {
    // This in-memory value-semantic class provides a representation of the
    // type having only one value.
};

// FREE FUNCTIONS
bsl::ostream& operator<<(bsl::ostream& stream, const TestNilValue&);

bool operator==(const TestNilValue&, const TestNilValue&);

bool operator!=(const TestNilValue&, const TestNilValue&);

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

                             // ------------------
                             // class TestNilValue
                             // ------------------

// FREE FUNCTIONS
inline
bsl::ostream& operator<<(bsl::ostream& stream, const TestNilValue&)
{
    return stream << "nil";
}

inline
bool operator==(const TestNilValue&, const TestNilValue&)
{
    return true;
}

inline
bool operator!=(const TestNilValue&, const TestNilValue&)
{
    return false;
}

}  // close package namespace
}  // close enterprise namespace

#endif // INCLUDED_S_BALTST_TESTNILVALUE

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
