// s_baltst_mysequencewithnullableanonymouschoice.cpp*DO NOT EDIT*@generated -*-C++-*-

#include <bsls_ident.h>
BSLS_IDENT_RCSID(s_baltst_mysequencewithnullableanonymouschoice_cpp,"$Id$ $CSID$")

#include <s_baltst_mysequencewithnullableanonymouschoice.h>

#include <bdlat_formattingmode.h>
#include <bdlat_valuetypefunctions.h>
#include <bdlb_print.h>
#include <bdlb_printmethods.h>
#include <bdlb_string.h>

#include <bdlb_nullablevalue.h>
#include <bslim_printer.h>
#include <bsls_assert.h>

#include <bsl_iomanip.h>
#include <bsl_limits.h>
#include <bsl_ostream.h>

namespace BloombergLP {
namespace s_baltst {

             // -------------------------------------------------
             // class MySequenceWithNullableAnonymousChoiceChoice
             // -------------------------------------------------

// CONSTANTS

const char MySequenceWithNullableAnonymousChoiceChoice::CLASS_NAME[] = "MySequenceWithNullableAnonymousChoiceChoice";

const bdlat_SelectionInfo MySequenceWithNullableAnonymousChoiceChoice::SELECTION_INFO_ARRAY[] = {
    {
        SELECTION_ID_CHOICE_A,
        "choiceA",
        sizeof("choiceA") - 1,
        "",
        bdlat_FormattingMode::e_DEC
    },
    {
        SELECTION_ID_CHOICE_B,
        "choiceB",
        sizeof("choiceB") - 1,
        "",
        bdlat_FormattingMode::e_DEC
    }
};

// CLASS METHODS

const bdlat_SelectionInfo *MySequenceWithNullableAnonymousChoiceChoice::lookupSelectionInfo(
        const char *name,
        int         nameLength)
{
    for (int i = 0; i < 2; ++i) {
        const bdlat_SelectionInfo& selectionInfo =
                    MySequenceWithNullableAnonymousChoiceChoice::SELECTION_INFO_ARRAY[i];

        if (nameLength == selectionInfo.d_nameLength
        &&  0 == bsl::memcmp(selectionInfo.d_name_p, name, nameLength))
        {
            return &selectionInfo;
        }
    }

    return 0;
}

const bdlat_SelectionInfo *MySequenceWithNullableAnonymousChoiceChoice::lookupSelectionInfo(int id)
{
    switch (id) {
      case SELECTION_ID_CHOICE_A:
        return &SELECTION_INFO_ARRAY[SELECTION_INDEX_CHOICE_A];
      case SELECTION_ID_CHOICE_B:
        return &SELECTION_INFO_ARRAY[SELECTION_INDEX_CHOICE_B];
      default:
        return 0;
    }
}

// CREATORS

MySequenceWithNullableAnonymousChoiceChoice::MySequenceWithNullableAnonymousChoiceChoice(const MySequenceWithNullableAnonymousChoiceChoice& original)
: d_selectionId(original.d_selectionId)
{
    switch (d_selectionId) {
      case SELECTION_ID_CHOICE_A: {
        new (d_choiceA.buffer())
            int(original.d_choiceA.object());
      } break;
      case SELECTION_ID_CHOICE_B: {
        new (d_choiceB.buffer())
            int(original.d_choiceB.object());
      } break;
      default:
        BSLS_ASSERT(SELECTION_ID_UNDEFINED == d_selectionId);
    }
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
MySequenceWithNullableAnonymousChoiceChoice::MySequenceWithNullableAnonymousChoiceChoice(MySequenceWithNullableAnonymousChoiceChoice&& original) noexcept
: d_selectionId(original.d_selectionId)
{
    switch (d_selectionId) {
      case SELECTION_ID_CHOICE_A: {
        new (d_choiceA.buffer())
            int(bsl::move(original.d_choiceA.object()));
      } break;
      case SELECTION_ID_CHOICE_B: {
        new (d_choiceB.buffer())
            int(bsl::move(original.d_choiceB.object()));
      } break;
      default:
        BSLS_ASSERT(SELECTION_ID_UNDEFINED == d_selectionId);
    }
}
#endif

// MANIPULATORS

MySequenceWithNullableAnonymousChoiceChoice&
MySequenceWithNullableAnonymousChoiceChoice::operator=(const MySequenceWithNullableAnonymousChoiceChoice& rhs)
{
    if (this != &rhs) {
        switch (rhs.d_selectionId) {
          case SELECTION_ID_CHOICE_A: {
            makeChoiceA(rhs.d_choiceA.object());
          } break;
          case SELECTION_ID_CHOICE_B: {
            makeChoiceB(rhs.d_choiceB.object());
          } break;
          default:
            BSLS_ASSERT(SELECTION_ID_UNDEFINED == rhs.d_selectionId);
            reset();
        }
    }

    return *this;
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
MySequenceWithNullableAnonymousChoiceChoice&
MySequenceWithNullableAnonymousChoiceChoice::operator=(MySequenceWithNullableAnonymousChoiceChoice&& rhs)
{
    if (this != &rhs) {
        switch (rhs.d_selectionId) {
          case SELECTION_ID_CHOICE_A: {
            makeChoiceA(bsl::move(rhs.d_choiceA.object()));
          } break;
          case SELECTION_ID_CHOICE_B: {
            makeChoiceB(bsl::move(rhs.d_choiceB.object()));
          } break;
          default:
            BSLS_ASSERT(SELECTION_ID_UNDEFINED == rhs.d_selectionId);
            reset();
        }
    }

    return *this;
}
#endif

void MySequenceWithNullableAnonymousChoiceChoice::reset()
{
    switch (d_selectionId) {
      case SELECTION_ID_CHOICE_A: {
        // no destruction required
      } break;
      case SELECTION_ID_CHOICE_B: {
        // no destruction required
      } break;
      default:
        BSLS_ASSERT(SELECTION_ID_UNDEFINED == d_selectionId);
    }

    d_selectionId = SELECTION_ID_UNDEFINED;
}

int MySequenceWithNullableAnonymousChoiceChoice::makeSelection(int selectionId)
{
    switch (selectionId) {
      case SELECTION_ID_CHOICE_A: {
        makeChoiceA();
      } break;
      case SELECTION_ID_CHOICE_B: {
        makeChoiceB();
      } break;
      case SELECTION_ID_UNDEFINED: {
        reset();
      } break;
      default:
        return -1;
    }
    return 0;
}

int MySequenceWithNullableAnonymousChoiceChoice::makeSelection(const char *name, int nameLength)
{
    const bdlat_SelectionInfo *selectionInfo =
                                         lookupSelectionInfo(name, nameLength);
    if (0 == selectionInfo) {
       return -1;
    }

    return makeSelection(selectionInfo->d_id);
}

int& MySequenceWithNullableAnonymousChoiceChoice::makeChoiceA()
{
    if (SELECTION_ID_CHOICE_A == d_selectionId) {
        bdlat_ValueTypeFunctions::reset(&d_choiceA.object());
    }
    else {
        reset();
        new (d_choiceA.buffer())
            int();
        d_selectionId = SELECTION_ID_CHOICE_A;
    }

    return d_choiceA.object();
}

int& MySequenceWithNullableAnonymousChoiceChoice::makeChoiceA(int value)
{
    if (SELECTION_ID_CHOICE_A == d_selectionId) {
        d_choiceA.object() = value;
    }
    else {
        reset();
        new (d_choiceA.buffer())
                int(value);
        d_selectionId = SELECTION_ID_CHOICE_A;
    }

    return d_choiceA.object();
}

int& MySequenceWithNullableAnonymousChoiceChoice::makeChoiceB()
{
    if (SELECTION_ID_CHOICE_B == d_selectionId) {
        bdlat_ValueTypeFunctions::reset(&d_choiceB.object());
    }
    else {
        reset();
        new (d_choiceB.buffer())
            int();
        d_selectionId = SELECTION_ID_CHOICE_B;
    }

    return d_choiceB.object();
}

int& MySequenceWithNullableAnonymousChoiceChoice::makeChoiceB(int value)
{
    if (SELECTION_ID_CHOICE_B == d_selectionId) {
        d_choiceB.object() = value;
    }
    else {
        reset();
        new (d_choiceB.buffer())
                int(value);
        d_selectionId = SELECTION_ID_CHOICE_B;
    }

    return d_choiceB.object();
}

// ACCESSORS

bsl::ostream& MySequenceWithNullableAnonymousChoiceChoice::print(
    bsl::ostream& stream,
    int           level,
    int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    switch (d_selectionId) {
      case SELECTION_ID_CHOICE_A: {
        printer.printAttribute("choiceA", d_choiceA.object());
      }  break;
      case SELECTION_ID_CHOICE_B: {
        printer.printAttribute("choiceB", d_choiceB.object());
      }  break;
      default:
        stream << "SELECTION UNDEFINED\n";
    }
    printer.end();
    return stream;
}


const char *MySequenceWithNullableAnonymousChoiceChoice::selectionName() const
{
    switch (d_selectionId) {
      case SELECTION_ID_CHOICE_A:
        return SELECTION_INFO_ARRAY[SELECTION_INDEX_CHOICE_A].name();
      case SELECTION_ID_CHOICE_B:
        return SELECTION_INFO_ARRAY[SELECTION_INDEX_CHOICE_B].name();
      default:
        BSLS_ASSERT(SELECTION_ID_UNDEFINED == d_selectionId);
        return "(* UNDEFINED *)";
    }
}

                // -------------------------------------------
                // class MySequenceWithNullableAnonymousChoice
                // -------------------------------------------

// CONSTANTS

const char MySequenceWithNullableAnonymousChoice::CLASS_NAME[] = "MySequenceWithNullableAnonymousChoice";

const bdlat_AttributeInfo MySequenceWithNullableAnonymousChoice::ATTRIBUTE_INFO_ARRAY[] = {
    {
        ATTRIBUTE_ID_CHOICE,
        "Choice",
        sizeof("Choice") - 1,
        "",
        bdlat_FormattingMode::e_DEFAULT
      | bdlat_FormattingMode::e_UNTAGGED
    }
};

// CLASS METHODS

const bdlat_AttributeInfo *MySequenceWithNullableAnonymousChoice::lookupAttributeInfo(
        const char *name,
        int         nameLength)
{
    if (bdlb::String::areEqualCaseless("choiceA", name, nameLength)) {
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CHOICE];
    }

    if (bdlb::String::areEqualCaseless("choiceB", name, nameLength)) {
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CHOICE];
    }

    for (int i = 0; i < 1; ++i) {
        const bdlat_AttributeInfo& attributeInfo =
                    MySequenceWithNullableAnonymousChoice::ATTRIBUTE_INFO_ARRAY[i];

        if (nameLength == attributeInfo.d_nameLength
        &&  0 == bsl::memcmp(attributeInfo.d_name_p, name, nameLength))
        {
            return &attributeInfo;
        }
    }

    return 0;
}

const bdlat_AttributeInfo *MySequenceWithNullableAnonymousChoice::lookupAttributeInfo(int id)
{
    switch (id) {
      case ATTRIBUTE_ID_CHOICE:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CHOICE];
      default:
        return 0;
    }
}

// CREATORS

MySequenceWithNullableAnonymousChoice::MySequenceWithNullableAnonymousChoice()
: d_choice()
{
}

MySequenceWithNullableAnonymousChoice::MySequenceWithNullableAnonymousChoice(const MySequenceWithNullableAnonymousChoice& original)
: d_choice(original.d_choice)
{
}

MySequenceWithNullableAnonymousChoice::~MySequenceWithNullableAnonymousChoice()
{
}

// MANIPULATORS

MySequenceWithNullableAnonymousChoice&
MySequenceWithNullableAnonymousChoice::operator=(const MySequenceWithNullableAnonymousChoice& rhs)
{
    if (this != &rhs) {
        d_choice = rhs.d_choice;
    }

    return *this;
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
MySequenceWithNullableAnonymousChoice&
MySequenceWithNullableAnonymousChoice::operator=(MySequenceWithNullableAnonymousChoice&& rhs)
{
    if (this != &rhs) {
        d_choice = bsl::move(rhs.d_choice);
    }

    return *this;
}
#endif

void MySequenceWithNullableAnonymousChoice::reset()
{
    bdlat_ValueTypeFunctions::reset(&d_choice);
}

// ACCESSORS

bsl::ostream& MySequenceWithNullableAnonymousChoice::print(
        bsl::ostream& stream,
        int           level,
        int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute("choice", this->choice());
    printer.end();
    return stream;
}


}  // close package namespace
}  // close enterprise namespace

// GENERATED BY BLP_BAS_CODEGEN_2021.08.08
// USING bas_codegen.pl s_baltst_mysequencewithnullableanonymouschoice.xsd -m msg -C mysequencewithnullableanonymouschoice -p s_baltst --noExternalization --noHashSupport --noAggregateConversion
// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright 2021 Bloomberg Finance L.P. All rights reserved.
//      Property of Bloomberg Finance L.P. (BFLP)
//      This software is made available solely pursuant to the
//      terms of a BFLP license agreement which governs its use.
// ------------------------------- END-OF-FILE --------------------------------
