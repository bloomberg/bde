// s_baltst_mysequencewithchoice.cpp     *DO NOT EDIT*     @generated -*-C++-*-

#include <bsls_ident.h>
BSLS_IDENT_RCSID(s_baltst_mysequencewithchoice_cpp, "$Id$ $CSID$")

#include <s_baltst_mysequencewithchoice.h>

#include <bdlat_formattingmode.h>
#include <bdlat_valuetypefunctions.h>
#include <bdlb_print.h>
#include <bdlb_printmethods.h>
#include <bdlb_string.h>

#include <bslim_printer.h>
#include <bsls_assert.h>

#include <bsl_cstring.h>
#include <bsl_iomanip.h>
#include <bsl_limits.h>
#include <bsl_ostream.h>
#include <bsl_utility.h>

namespace BloombergLP {
namespace s_baltst {

                      // --------------------------------
                      // class MySequenceWithChoiceChoice
                      // --------------------------------

// CONSTANTS

const char MySequenceWithChoiceChoice::CLASS_NAME[] = "MySequenceWithChoiceChoice";

const bdlat_SelectionInfo MySequenceWithChoiceChoice::SELECTION_INFO_ARRAY[] = {
    {
        SELECTION_ID_CHOICE_A,
        "choiceA",
        sizeof("choiceA") - 1,
        "",
        bdlat_FormattingMode::e_DEC
    }
};

// CLASS METHODS

const bdlat_SelectionInfo *MySequenceWithChoiceChoice::lookupSelectionInfo(
        const char *name,
        int         nameLength)
{
    for (int i = 0; i < 1; ++i) {
        const bdlat_SelectionInfo& selectionInfo =
                    MySequenceWithChoiceChoice::SELECTION_INFO_ARRAY[i];

        if (nameLength == selectionInfo.d_nameLength
        &&  0 == bsl::memcmp(selectionInfo.d_name_p, name, nameLength))
        {
            return &selectionInfo;
        }
    }

    return 0;
}

const bdlat_SelectionInfo *MySequenceWithChoiceChoice::lookupSelectionInfo(int id)
{
    switch (id) {
      case SELECTION_ID_CHOICE_A:
        return &SELECTION_INFO_ARRAY[SELECTION_INDEX_CHOICE_A];
      default:
        return 0;
    }
}

// CREATORS

MySequenceWithChoiceChoice::MySequenceWithChoiceChoice(const MySequenceWithChoiceChoice& original)
: d_selectionId(original.d_selectionId)
{
    switch (d_selectionId) {
      case SELECTION_ID_CHOICE_A: {
        new (d_choiceA.buffer())
            int(original.d_choiceA.object());
      } break;
      default:
        BSLS_ASSERT(SELECTION_ID_UNDEFINED == d_selectionId);
    }
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
MySequenceWithChoiceChoice::MySequenceWithChoiceChoice(MySequenceWithChoiceChoice&& original) noexcept
: d_selectionId(original.d_selectionId)
{
    switch (d_selectionId) {
      case SELECTION_ID_CHOICE_A: {
        new (d_choiceA.buffer())
            int(bsl::move(original.d_choiceA.object()));
      } break;
      default:
        BSLS_ASSERT(SELECTION_ID_UNDEFINED == d_selectionId);
    }
}
#endif

// MANIPULATORS

MySequenceWithChoiceChoice&
MySequenceWithChoiceChoice::operator=(const MySequenceWithChoiceChoice& rhs)
{
    if (this != &rhs) {
        switch (rhs.d_selectionId) {
          case SELECTION_ID_CHOICE_A: {
            makeChoiceA(rhs.d_choiceA.object());
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
MySequenceWithChoiceChoice&
MySequenceWithChoiceChoice::operator=(MySequenceWithChoiceChoice&& rhs)
{
    if (this != &rhs) {
        switch (rhs.d_selectionId) {
          case SELECTION_ID_CHOICE_A: {
            makeChoiceA(bsl::move(rhs.d_choiceA.object()));
          } break;
          default:
            BSLS_ASSERT(SELECTION_ID_UNDEFINED == rhs.d_selectionId);
            reset();
        }
    }

    return *this;
}
#endif

void MySequenceWithChoiceChoice::reset()
{
    switch (d_selectionId) {
      case SELECTION_ID_CHOICE_A: {
        // no destruction required
      } break;
      default:
        BSLS_ASSERT(SELECTION_ID_UNDEFINED == d_selectionId);
    }

    d_selectionId = SELECTION_ID_UNDEFINED;
}

int MySequenceWithChoiceChoice::makeSelection(int selectionId)
{
    switch (selectionId) {
      case SELECTION_ID_CHOICE_A: {
        makeChoiceA();
      } break;
      case SELECTION_ID_UNDEFINED: {
        reset();
      } break;
      default:
        return -1;
    }
    return 0;
}

int MySequenceWithChoiceChoice::makeSelection(const char *name, int nameLength)
{
    const bdlat_SelectionInfo *selectionInfo =
                                         lookupSelectionInfo(name, nameLength);
    if (0 == selectionInfo) {
       return -1;
    }

    return makeSelection(selectionInfo->d_id);
}

int& MySequenceWithChoiceChoice::makeChoiceA()
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

int& MySequenceWithChoiceChoice::makeChoiceA(int value)
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

// ACCESSORS

bsl::ostream& MySequenceWithChoiceChoice::print(
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
      default:
        stream << "SELECTION UNDEFINED\n";
    }
    printer.end();
    return stream;
}


const char *MySequenceWithChoiceChoice::selectionName() const
{
    switch (d_selectionId) {
      case SELECTION_ID_CHOICE_A:
        return SELECTION_INFO_ARRAY[SELECTION_INDEX_CHOICE_A].name();
      default:
        BSLS_ASSERT(SELECTION_ID_UNDEFINED == d_selectionId);
        return "(* UNDEFINED *)";
    }
}

                         // --------------------------
                         // class MySequenceWithChoice
                         // --------------------------

// CONSTANTS

const char MySequenceWithChoice::CLASS_NAME[] = "MySequenceWithChoice";

const bdlat_AttributeInfo MySequenceWithChoice::ATTRIBUTE_INFO_ARRAY[] = {
    {
        ATTRIBUTE_ID_MODE,
        "mode",
        sizeof("mode") - 1,
        "",
        bdlat_FormattingMode::e_DEFAULT
    }
};

// CLASS METHODS

const bdlat_AttributeInfo *MySequenceWithChoice::lookupAttributeInfo(
        const char *name,
        int         nameLength)
{
    for (int i = 0; i < 1; ++i) {
        const bdlat_AttributeInfo& attributeInfo =
                    MySequenceWithChoice::ATTRIBUTE_INFO_ARRAY[i];

        if (nameLength == attributeInfo.d_nameLength
        &&  0 == bsl::memcmp(attributeInfo.d_name_p, name, nameLength))
        {
            return &attributeInfo;
        }
    }

    return 0;
}

const bdlat_AttributeInfo *MySequenceWithChoice::lookupAttributeInfo(int id)
{
    switch (id) {
      case ATTRIBUTE_ID_MODE:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_MODE];
      default:
        return 0;
    }
}

// CREATORS

MySequenceWithChoice::MySequenceWithChoice()
: d_mode()
{
}

// MANIPULATORS

void MySequenceWithChoice::reset()
{
    bdlat_ValueTypeFunctions::reset(&d_mode);
}

// ACCESSORS

bsl::ostream& MySequenceWithChoice::print(bsl::ostream& stream,
                                          int           level,
                                          int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute("mode", this->mode());
    printer.end();
    return stream;
}


}  // close package namespace
}  // close enterprise namespace

// GENERATED BY BLP_BAS_CODEGEN_2025.08.21
// USING bas_codegen.pl s_baltst_mysequencewithchoice.xsd --mode msg --includedir . --msgComponent mysequencewithchoice --noRecurse --noExternalization --noHashSupport --noAggregateConversion
// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright 2025 Bloomberg Finance L.P. All rights reserved.
//      Property of Bloomberg Finance L.P. (BFLP)
//      This software is made available solely pursuant to the
//      terms of a BFLP license agreement which governs its use.
// ------------------------------- END-OF-FILE --------------------------------
