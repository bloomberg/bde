// bdlat_bdeatoverrides.t.cpp                                         -*-C++-*-

#include <bdlat_bdeatoverrides.h>
#include <bdlat_choicefunctions.h>  // for testing only
#include <bdlat_formattingmode.h>   // for testing only
#include <bdlat_typecategory.h>     // for testing only

#include <bdls_testutil.h>

#include <bsl_cstdlib.h>
#include <bsl_cstring.h>
#include <bsl_iostream.h>

using namespace BloombergLP;
using namespace bsl;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
//                               TBD doc
//-----------------------------------------------------------------------------
// [ 1] DEFINITIONS
// [ 2] ENUMERATORS
//-----------------------------------------------------------------------------

// ============================================================================
//                     STANDARD BSL ASSERT TEST FUNCTION
// ----------------------------------------------------------------------------

namespace {

int testStatus = 0;

void aSsErT(bool condition, const char *message, int line)
{
    if (condition) {
        cout << "Error " __FILE__ "(" << line << "): " << message
             << "    (failed)" << endl;

        if (0 <= testStatus && testStatus <= 100) {
            ++testStatus;
        }
    }
}

}  // close unnamed namespace

// ============================================================================
//               STANDARD BDE TEST DRIVER MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT       BDLS_TESTUTIL_ASSERT
#define ASSERTV      BDLS_TESTUTIL_ASSERTV

#define LOOP_ASSERT  BDLS_TESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BDLS_TESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BDLS_TESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BDLS_TESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BDLS_TESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BDLS_TESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BDLS_TESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BDLS_TESTUTIL_LOOP6_ASSERT

#define Q            BDLS_TESTUTIL_Q   // Quote identifier literally.
#define P            BDLS_TESTUTIL_P   // Print identifier and value.
#define P_           BDLS_TESTUTIL_P_  // P(X) without '\n'.
#define T_           BDLS_TESTUTIL_T_  // Print a tab (w/o newline).
#define L_           BDLS_TESTUTIL_L_  // current Line number

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;
    // int veryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:  // Zero is always the leading case.
      case 1: {
        // --------------------------------------------------------------------
        // TEST DEFINITIONS
        //
        // Concerns:
        //: 1 When BDE_OPENSOURCE_PUBLICATION is not defined, the suite of
        //:   BDEAT_... and bdeat_... macros should be defined to be their
        //:   BDLAT_... and bdlat_... versions.
        //:
        //: 2 When BDE_OPENSOURCE_PUBLICATION is defined, the BDEAT_... and
        //:   bdeat_... macros should not be defined.
        //
        // Plan:
        //: 1 Obtain a stringized version of the macro definition through the
        //:   usual indirection through a secondary macro before quoting.
        //:
        //: 2 When BDE_OPENSOURCE_PUBLICATION is not defined, comapre the
        //:   stringized version to the expected value.  (C-1)
        //:
        //: 3 When BDE_OPENSOURCE_PUBLICATION is defined, compare the
        //:   stringized version to the macro name (without a macro definition
        //:   the quoting process will just return the quoted name).
        //
        // Testing:
        //   DEFINITIONS
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTEST DEFINITIONS"
                          << "\n================" << endl;

#undef q
#define q(x) #x    // quote argument

#undef v
#define v(x) q(x)  // quote macro-expanded argument

#undef l
#define l(x) \
{ L_, "bdeat_" #x, "bdlat_" #x, v(bdeat_ ## x) }  // lower-case 'bdeat' macro

#undef u
#define u(x) \
{ L_, "BDEAT_" #x, "BDLAT_" #x, v(BDEAT_ ## x) }  // upper-case 'BDEAT' macro

        static struct {
            int         d_line;        // line number
            const char *d_name;        // macro name
            const char *d_expected;    // expected macro definition
            const char *d_actual;      // actual macro definition
        } DATA[] = {
            u(DECL_CHOICE_TRAITS),
            u(DECL_CHOICE_WITH_ALLOCATOR_BITWISEMOVEABLE_TRAITS),
            u(DECL_CHOICE_WITH_ALLOCATOR_TRAITS),
            u(DECL_CHOICE_WITH_BITWISEMOVEABLE_TRAITS),
            u(DECL_CUSTOMIZEDTYPE_TRAITS),
            u(DECL_CUSTOMIZEDTYPE_WITH_ALLOCATOR_BITWISEMOVEABLE_TRAITS),
            u(DECL_CUSTOMIZEDTYPE_WITH_ALLOCATOR_TRAITS),
            u(DECL_CUSTOMIZEDTYPE_WITH_BITWISEMOVEABLE_TRAITS),
            u(DECL_ENUMERATION_TRAITS),
            u(DECL_SEQUENCE_TRAITS),
            u(DECL_SEQUENCE_WITH_ALLOCATOR_BITWISEMOVEABLE_TRAITS),
            u(DECL_SEQUENCE_WITH_ALLOCATOR_TRAITS),
            u(DECL_SEQUENCE_WITH_BITWISEMOVEABLE_TRAITS),
            l(ArrayFunctions),
            l(ArrayIterators),
            l(AttributeInfo),
            l(BasicEnumerationWrapper),
            l(ChoiceFunctions),
            l(CustomizedTypeFunctions),
            l(EnumFunctions),
            l(EnumeratorInfo),
            l(FormattingMode),
            l(IsBasicChoice),
            l(IsBasicCustomizedType),
            l(IsBasicEnumeration),
            l(IsBasicSequence),
            l(NullableValueFunctions),
            l(SelectionInfo),
            l(SequenceFunctions),
            l(SymbolicConverter),
            l(TypeCategory),
            l(TypeCategoryDeclareDynamic),
            l(TypeCategoryFunctions),
            l(TypeCategoryUtil),
            l(TypeName),
            l(TypeName_className),
            l(TypeName_name),
            l(TypeName_xsdName),
            l(TypeTraitBasicChoice),
            l(TypeTraitBasicCustomizedType),
            l(TypeTraitBasicEnumeration),
            l(TypeTraitBasicSequence),
            l(ValueTypeFunctions),
            l(arrayAccessElement),
            l(arrayManipulateElement),
            l(arrayResize),
            l(arraySize),
            l(choiceAccessSelection),
            l(choiceHasSelection),
            l(choiceMakeSelection),
            l(choiceManipulateSelection),
            l(choiceSelectionId),
            l(enumFromInt),
            l(enumFromString),
            l(enumToInt),
            l(enumToString),
            l(nullableValueAccessValue),
            l(nullableValueIsNull),
            l(nullableValueMakeValue),
            l(nullableValueManipulateValue),
            l(sequenceAccessAttribute),
            l(sequenceAccessAttributes),
            l(sequenceHasAttribute),
            l(sequenceManipulateAttribute),
            l(sequenceManipulateAttributes),
            l(typeCategoryAccessArray),
            l(typeCategoryAccessChoice),
            l(typeCategoryAccessCustomizedType),
            l(typeCategoryAccessEnumeration),
            l(typeCategoryAccessNullableValue),
            l(typeCategoryAccessSequence),
            l(typeCategoryAccessSimple),
            l(typeCategoryManipulateArray),
            l(typeCategoryManipulateChoice),
            l(typeCategoryManipulateCustomizedType),
            l(typeCategoryManipulateEnumeration),
            l(typeCategoryManipulateNullableValue),
            l(typeCategoryManipulateSequence),
            l(typeCategoryManipulateSimple),
            l(typeCategorySelect),
            l(valueTypeAssign),
            l(valueTypeReset),
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int i = 0; i != NUM_DATA; ++i) {
            const int   LINE     = DATA[i].d_line;
            const char *NAME     = DATA[i].d_name;
#ifndef BDE_OPENSOURCE_PUBLICATION
            const char *EXPECTED = DATA[i].d_expected;
#else
            const char *EXPECTED = DATA[i].d_name;
#endif
            const char *ACTUAL   = DATA[i].d_actual;

            if (veryVerbose) { P_(LINE) P_(NAME) P_(EXPECTED) P(ACTUAL) }

            ASSERTV(LINE, NAME, EXPECTED, ACTUAL, !strcmp(EXPECTED, ACTUAL));
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TEST ENUMERATORS
        //
        // Concerns
        //: 1 When BDE_OPENSOURCE_PUBLICATION is not defined, the BDEAT_...
        //:   enumeration literals should exist and evaluate to their BDLAT_...
        //:   equivalents.
        //:
        //: 2 When BDE_OPENSOURCE_PUBLICATION is defined, the BDEAT_...
        //:   enumeration literals should not exist.
        //
        // Plan
        //: 1 When BDE_OPENSOURCE_PUBLICATION is not defined, check that the
        //:   BDEAT_... enumeration literals evaluate to their BDLAT_...
        //:   equivalents.  (C-1)
        //:
        //: 2 We cannot check for (C-2), so hope for the best.
        //
        // Testing:
        //   ENUMERATORS
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTEST ENUMERATORS"
                          << "\n================" << endl;

#undef e
#define e(Class, Enumerator)                                                  \
    { L_, #Class "::BDxAT_" #Enumerator,                                      \
      Class::BDLAT_##Enumerator, Class::BDEAT_##Enumerator }

        static struct {
            int         d_line;         // line number
            const char *d_name;         // printable enumerator name
            int         d_bdeat_value;  // value of BDEAT_... version
            int         d_bdlat_value;  // value of BDLAT_... version
        } DATA [] = {
#ifndef BDE_OPENSOURCE_PUBLICATION
          e(bdlat_ChoiceFunctions, UNDEFINED_SELECTION_ID),
          e(bdlat_FormattingMode, DEFAULT),
          e(bdlat_FormattingMode, DEC),
          e(bdlat_FormattingMode, HEX),
          e(bdlat_FormattingMode, BASE64),
          e(bdlat_FormattingMode, TEXT),
          e(bdlat_FormattingMode, TYPE_MASK),
          e(bdlat_FormattingMode, UNTAGGED),
          e(bdlat_FormattingMode, ATTRIBUTE),
          e(bdlat_FormattingMode, SIMPLE_CONTENT),
          e(bdlat_FormattingMode, NILLABLE),
          e(bdlat_FormattingMode, LIST),
          e(bdlat_FormattingMode, FLAGS_MASK),
          e(bdlat_TypeCategory, DYNAMIC_CATEGORY),
          e(bdlat_TypeCategory, ARRAY_CATEGORY),
          e(bdlat_TypeCategory, CHOICE_CATEGORY),
          e(bdlat_TypeCategory, CUSTOMIZED_TYPE_CATEGORY),
          e(bdlat_TypeCategory, ENUMERATION_CATEGORY),
          e(bdlat_TypeCategory, NULLABLE_VALUE_CATEGORY),
          e(bdlat_TypeCategory, SEQUENCE_CATEGORY),
          e(bdlat_TypeCategory, SIMPLE_CATEGORY),
          e(bdlat_TypeCategory::Select<void*>, SELECTION),
#endif
          { L_, "None", 0, 0 },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int i = 0; i < NUM_DATA; ++i) {
            const int   LINE   = DATA[i].d_line;
            const char *NAME   = DATA[i].d_name;
            const int   EVALUE = DATA[i].d_bdeat_value;
            const int   LVALUE = DATA[i].d_bdlat_value;

            if (veryVerbose) { P_(LINE) P_(NAME) P_(EVALUE) P(LVALUE) }

            ASSERTV(LINE, NAME, EVALUE, LVALUE, EVALUE == LVALUE);
        }
      } break;
      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        cerr << "Error, non-zero test status = " << testStatus << "." << endl;
    }

    return testStatus;
}

// ----------------------------------------------------------------------------
// Copyright 2015 Bloomberg Finance L.P.
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
