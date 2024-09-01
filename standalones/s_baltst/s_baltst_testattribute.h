// s_baltst_testattribute.h                                           -*-C++-*-
#ifndef INCLUDED_S_BALTST_TESTATTRIBUTE
#define INCLUDED_S_BALTST_TESTATTRIBUTE

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide attribute metadata types for test sequences.
//
//@CLASSES:
//  s_baltst::TestAttribute: attribute metadata for test sequences
//  s_baltst::TestAttribute1: an example attribute
//  s_baltst::TestAttribute2: an example attribute
//  s_baltst::TestAttributeDefaults: defaults
//  s_baltst::TypedTestAttribute: attribute metadata with value type

#include <bdlat_attributeinfo.h>

#include <bsl_cstring.h>
#include <bsl_string.h>
#include <bsl_string_view.h>

namespace BloombergLP {
namespace s_baltst {

                        // ============================
                        // struct TestAttributeDefaults
                        // ============================

/// This utility `struct` provides a namespace for non-modifiable,
/// constant-initialized default values for the non-type template parameters
/// of a `TestAttribute`.
struct TestAttributeDefaults {

    // CLASS DATA
    static const char k_DEFAULT_ANNOTATION[1];
    static const char k_DEFAULT_NAME[1];
    enum { k_DEFAULT_FORMATTING_MODE = 0 };
};

                            // ===================
                            // class TestAttribute
                            // ===================

/// This class provides a namespace for a suite of non-modifiable,
/// constant-initialized data that can be used to specify all attributes of
/// a `bdlat_AttributeInfo` object.
template <int         ID,
          const char *NAME       = TestAttributeDefaults::k_DEFAULT_NAME,
          const char *ANNOTATION = TestAttributeDefaults::k_DEFAULT_ANNOTATION,
          int         FORMATTING_MODE =
              TestAttributeDefaults::k_DEFAULT_FORMATTING_MODE>
class TestAttribute {

  public:
    // CLASS DATA
    static const char *k_NAME;
    static const char *k_ANNOTATION;
    enum { k_ID = ID, k_FORMATTING_MODE = FORMATTING_MODE };

    // CLASS METHODS
    static int id();

    static bsl::string_view name();

    static bsl::string_view annotation();

    static int formattingMode();

    static bdlat_AttributeInfo attributeInfo();

    // CREATORS
    TestAttribute();
};

// CLASS DATA
template <int         ID,
          const char *NAME,
          const char *ANNOTATION,
          int         FORMATTING_MODE>
const char *TestAttribute<ID, NAME, ANNOTATION, FORMATTING_MODE>::k_NAME =
    NAME;

template <int         ID,
          const char *NAME,
          const char *ANNOTATION,
          int         FORMATTING_MODE>
const char
    *TestAttribute<ID, NAME, ANNOTATION, FORMATTING_MODE>::k_ANNOTATION =
        ANNOTATION;

                          // ========================
                          // class TypedTestAttribute
                          // ========================

/// This class provides two member type definitions, `Type`, and
/// `Attribute`, which are aliases to the arguments supplied to the `TYPE`
/// and `TEST_SELECTION` template parameters, respectively.
template <class TYPE, class TEST_ATTRIBUTE>
class TypedTestAttribute {

  public:
    // TYPES
    typedef TYPE           Type;
    typedef TEST_ATTRIBUTE Attribute;
};

                          // =========================
                          // struct TestAttribute_Data
                          // =========================

struct TestAttribute_Data {
    // PUBLIC CLASS DATA
    static const int  k_ATTRIBUTE_1_ID = 0;
    static const char k_ATTRIBUTE_1_NAME[sizeof("attr1")];
    static const char k_ATTRIBUTE_1_ANNOTATION[sizeof("")];
    static const int  k_ATTRIBUTE_1_FORMATTING_MODE = 0;
    static const int  k_ATTRIBUTE_2_ID              = 1;
    static const char k_ATTRIBUTE_2_NAME[sizeof("attr2")];
    static const char k_ATTRIBUTE_2_ANNOTATION[sizeof("")];
    static const int  k_ATTRIBUTE_2_FORMATTING_MODE = 0;
};

                            // ====================
                            // class TestAttribute1
                            // ====================

typedef TestAttribute<TestAttribute_Data::k_ATTRIBUTE_1_ID,
                      TestAttribute_Data::k_ATTRIBUTE_1_NAME,
                      TestAttribute_Data::k_ATTRIBUTE_1_ANNOTATION,
                      TestAttribute_Data::k_ATTRIBUTE_1_FORMATTING_MODE>
    TestAttribute1;

                            // ====================
                            // class TestAttribute2
                            // ====================

typedef TestAttribute<TestAttribute_Data::k_ATTRIBUTE_2_ID,
                      TestAttribute_Data::k_ATTRIBUTE_2_NAME,
                      TestAttribute_Data::k_ATTRIBUTE_2_ANNOTATION,
                      TestAttribute_Data::k_ATTRIBUTE_2_FORMATTING_MODE>
    TestAttribute2;

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

                            // -------------------
                            // class TestAttribute
                            // -------------------

// CLASS METHODS
template <int         ID,
          const char *NAME,
          const char *ANNOTATION,
          int         FORMATTING_MODE>
int TestAttribute<ID, NAME, ANNOTATION, FORMATTING_MODE>::id()
{
    return k_ID;
}

template <int         ID,
          const char *NAME,
          const char *ANNOTATION,
          int         FORMATTING_MODE>
bsl::string_view TestAttribute<ID, NAME, ANNOTATION, FORMATTING_MODE>::name()
{
    return k_NAME;
}

template <int         ID,
          const char *NAME,
          const char *ANNOTATION,
          int         FORMATTING_MODE>
bsl::string_view
TestAttribute<ID, NAME, ANNOTATION, FORMATTING_MODE>::annotation()
{
    return k_ANNOTATION;
}

template <int         ID,
          const char *NAME,
          const char *ANNOTATION,
          int         FORMATTING_MODE>
int TestAttribute<ID, NAME, ANNOTATION, FORMATTING_MODE>::formattingMode()
{
    return k_FORMATTING_MODE;
}

template <int         ID,
          const char *NAME,
          const char *ANNOTATION,
          int         FORMATTING_MODE>
bdlat_AttributeInfo
TestAttribute<ID, NAME, ANNOTATION, FORMATTING_MODE>::attributeInfo()
{
    bdlat_AttributeInfo result = {k_ID,
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
TestAttribute<ID, NAME, ANNOTATION, FORMATTING_MODE>::TestAttribute()
{
}

}  // close package namespace
}  // close enterprise namespace

#endif // INCLUDED_S_BALTST_TESTATTRIBUTE

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
