// s_baltst_testenumerator.h                                          -*-C++-*-
#ifndef INCLUDED_S_BALTST_TESTENUMERATOR
#define INCLUDED_S_BALTST_TESTENUMERATOR

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide enumerator metadata types for test enumerations.
//
//@CLASSES:
//  s_baltst::TestEnumerator: enumerator metadata for test enumerators
//  s_baltst::TestNilEnumerator: type representing no enumerator

#include <bsl_string_view.h>

namespace BloombergLP {
namespace s_baltst {

                            // ====================
                            // class TestEnumerator
                            // ====================

template <int INT_VALUE, const char *STRING_VALUE>
class TestEnumerator {
    // This class provides a namespace for a suite of constant-initialized data
    // that can be used to specify the integer and string values of an
    // enumerator for a 'bdlat' 'Enumeration' type.

  public:
    // CLASS DATA
    static const char *k_STRING_VALUE;
    enum { k_INT_VALUE = INT_VALUE };

    // CLASS METHODS
    static int              intValue();
    static bsl::string_view stringValue();

    // CREATORS
    TestEnumerator();
};

                          // =======================
                          // class TestNilEnumerator
                          // =======================

class TestNilEnumerator {
    // This class provides a namespace for a type having a set of public
    // members with the same names and types as 'TestEnumerator', and that can
    // be used as a sentinel type to indicate that no such enumerator exists.

  public:
    // CLASS DATA
    static const char *k_STRING_VALUE;
    enum { k_INT_VALUE = 0 };

    // CLASS METHODS
    static int              intValue();
    static bsl::string_view stringValue();
};

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

                            // --------------------
                            // class TestEnumerator
                            // --------------------

// CLASS DATA
template <int INT_VALUE, const char *STRING_VALUE>
const char *TestEnumerator<INT_VALUE, STRING_VALUE>::k_STRING_VALUE =
    STRING_VALUE;

// CLASS METHODS
template <int INT_VALUE, const char *STRING_VALUE>
int TestEnumerator<INT_VALUE, STRING_VALUE>::intValue()
{
    return k_INT_VALUE;
}

template <int INT_VALUE, const char *STRING_VALUE>
bsl::string_view TestEnumerator<INT_VALUE, STRING_VALUE>::stringValue()
{
    return k_STRING_VALUE;
}

// CREATORS
template <int INT_VALUE, const char *STRING_VALUE>
TestEnumerator<INT_VALUE, STRING_VALUE>::TestEnumerator()
{
}

                          // -----------------------
                          // class TestNilEnumerator
                          // -----------------------

// CLASS METHODS
inline
int TestNilEnumerator::intValue()
{
    return k_INT_VALUE;
}

inline
bsl::string_view TestNilEnumerator::stringValue()
{
    return k_STRING_VALUE;
}

}  // close package namespace
}  // close enterprise namespace

#endif  // INCLUDED_S_BALTST_TESTENUMERATOR

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
