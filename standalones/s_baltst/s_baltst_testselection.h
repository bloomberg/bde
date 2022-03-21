// s_baltst_testselection.h                                           -*-C++-*-
#ifndef INCLUDED_S_BALTST_TESTSELECTION
#define INCLUDED_S_BALTST_TESTSELECTION

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide selection metadata types for test choices.
//
//@CLASSES:
//  s_baltst::TestSelection: selection metadata for test choices
//  s_baltst::TestSelection1: an example selection
//  s_baltst::TestSelection2: an example selection
//  s_baltst::TestSelectionDefaults: defaults
//  s_baltst::TypedTestSelection: selection metadata with value type

#include <bdlat_selectioninfo.h>

#include <bsl_cstring.h>
#include <bsl_string.h>
#include <bsl_string_view.h>

namespace BloombergLP {
namespace s_baltst {

                        // ============================
                        // struct TestSelectionDefaults
                        // ============================

struct TestSelectionDefaults {
    // This utility 'struct' provides a namespace for non-modifiable,
    // constant-initialized default values for the non-type template parameters
    // of a 'TestSelection'.

    // CLASS DATA
    static const char k_DEFAULT_ANNOTATION[1];
    static const char k_DEFAULT_NAME[1];
    enum { k_DEFAULT_FORMATTING_MODE = 0 };
};

                            // ===================
                            // class TestSelection
                            // ===================

template <int         ID,
          const char *NAME = TestSelectionDefaults::k_DEFAULT_NAME,

          const char *ANNOTATION = TestSelectionDefaults::k_DEFAULT_ANNOTATION,
          int         FORMATTING_MODE =
              TestSelectionDefaults::k_DEFAULT_FORMATTING_MODE>
class TestSelection {
    // This class provides a namespace for a suite of non-modifiable,
    // constant-initialized data that can be used to specify all attribute of a
    // 'bdlat_SelectionInfo' object.

  public:
    // PUBLIC CLASS DATA
    static const char *k_NAME;
    static const char *k_ANNOTATION;
    enum { k_ID = ID, k_FORMATTING_MODE = FORMATTING_MODE };

    // CLASS METHODS
    static int id();

    static bsl::string_view name();

    static bsl::string_view annotation();

    static int formattingMode();

    static bdlat_SelectionInfo selectionInfo();

    // CREATORS
    TestSelection();
};

                          // ========================
                          // class TypedTestSelection
                          // ========================

template <class TYPE, class TEST_SELECTION>
class TypedTestSelection {
    // This class provides two member type definitions, 'Type' and 'Selection',
    // which are aliases to the arguments supplied to the 'TYPE' and
    // 'TEST_SELECTION' template parameters, respectively.

  public:
    // TYPES
    typedef TYPE           Type;
    typedef TEST_SELECTION Selection;
};

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

                            // -------------------
                            // class TestSelection
                            // -------------------

// PUBLIC CLASS DATA
template <int         ID,
          const char *NAME,
          const char *ANNOTATION,
          int         FORMATTING_MODE>
const char *TestSelection<ID, NAME, ANNOTATION, FORMATTING_MODE>::k_NAME =
    NAME;

template <int         ID,
          const char *NAME,
          const char *ANNOTATION,
          int         FORMATTING_MODE>
const char
    *TestSelection<ID, NAME, ANNOTATION, FORMATTING_MODE>::k_ANNOTATION =
        ANNOTATION;

// CLASS METHODS
template <int         ID,
          const char *NAME,
          const char *ANNOTATION,
          int         FORMATTING_MODE>
int TestSelection<ID, NAME, ANNOTATION, FORMATTING_MODE>::id()
{
    return k_ID;
}

template <int         ID,
          const char *NAME,
          const char *ANNOTATION,
          int         FORMATTING_MODE>
bsl::string_view TestSelection<ID, NAME, ANNOTATION, FORMATTING_MODE>::name()
{
    return k_NAME;
}

template <int         ID,
          const char *NAME,
          const char *ANNOTATION,
          int         FORMATTING_MODE>
bsl::string_view
TestSelection<ID, NAME, ANNOTATION, FORMATTING_MODE>::annotation()
{
    return k_ANNOTATION;
}

template <int         ID,
          const char *NAME,
          const char *ANNOTATION,
          int         FORMATTING_MODE>
int TestSelection<ID, NAME, ANNOTATION, FORMATTING_MODE>::formattingMode()
{
    return k_FORMATTING_MODE;
}

template <int         ID,
          const char *NAME,
          const char *ANNOTATION,
          int         FORMATTING_MODE>
bdlat_SelectionInfo
TestSelection<ID, NAME, ANNOTATION, FORMATTING_MODE>::selectionInfo()
{
    bdlat_SelectionInfo result = {k_ID,
                                  k_NAME,
                                  static_cast<int>(bsl::strlen(k_NAME)),
                                  k_ANNOTATION,
                                  k_FORMATTING_MODE};

    return result;
}

// CREATORS
template <int         ID,
          const char *NAME,
          const char *ANNOTATION,
          int         FORMATTING_MODE>
TestSelection<ID, NAME, ANNOTATION, FORMATTING_MODE>::TestSelection()
{
}

}  // close package namespace
}  // close enterprise namespace

#endif // INCLUDED_S_BALTST_TESTSELECTION

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
