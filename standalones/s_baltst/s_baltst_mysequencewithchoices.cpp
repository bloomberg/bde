// s_baltst_mysequencewithchoices.cpp    *DO NOT EDIT*     @generated -*-C++-*-

#include <bsls_ident.h>
BSLS_IDENT_RCSID(s_baltst_mysequencewithchoices_cpp, "$Id$ $CSID$")

#include <s_baltst_mysequencewithchoices.h>

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

                     // ----------------------------------
                     // class MySequenceWithChoicesChoice1
                     // ----------------------------------

// CONSTANTS

const char MySequenceWithChoicesChoice1::CLASS_NAME[] = "MySequenceWithChoicesChoice1";

const bdlat_SelectionInfo MySequenceWithChoicesChoice1::SELECTION_INFO_ARRAY[] = {
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

const bdlat_SelectionInfo *MySequenceWithChoicesChoice1::lookupSelectionInfo(
        const char *name,
        int         nameLength)
{
    for (int i = 0; i < 2; ++i) {
        const bdlat_SelectionInfo& selectionInfo =
                    MySequenceWithChoicesChoice1::SELECTION_INFO_ARRAY[i];

        if (nameLength == selectionInfo.d_nameLength
        &&  0 == bsl::memcmp(selectionInfo.d_name_p, name, nameLength))
        {
            return &selectionInfo;
        }
    }

    return 0;
}

const bdlat_SelectionInfo *MySequenceWithChoicesChoice1::lookupSelectionInfo(int id)
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

MySequenceWithChoicesChoice1::MySequenceWithChoicesChoice1(const MySequenceWithChoicesChoice1& original)
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
MySequenceWithChoicesChoice1::MySequenceWithChoicesChoice1(MySequenceWithChoicesChoice1&& original) noexcept
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

MySequenceWithChoicesChoice1&
MySequenceWithChoicesChoice1::operator=(const MySequenceWithChoicesChoice1& rhs)
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
MySequenceWithChoicesChoice1&
MySequenceWithChoicesChoice1::operator=(MySequenceWithChoicesChoice1&& rhs)
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

void MySequenceWithChoicesChoice1::reset()
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

int MySequenceWithChoicesChoice1::makeSelection(int selectionId)
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

int MySequenceWithChoicesChoice1::makeSelection(const char *name, int nameLength)
{
    const bdlat_SelectionInfo *selectionInfo =
                                         lookupSelectionInfo(name, nameLength);
    if (0 == selectionInfo) {
       return -1;
    }

    return makeSelection(selectionInfo->d_id);
}

int& MySequenceWithChoicesChoice1::makeChoiceA()
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

int& MySequenceWithChoicesChoice1::makeChoiceA(int value)
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

int& MySequenceWithChoicesChoice1::makeChoiceB()
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

int& MySequenceWithChoicesChoice1::makeChoiceB(int value)
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

bsl::ostream& MySequenceWithChoicesChoice1::print(
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


const char *MySequenceWithChoicesChoice1::selectionName() const
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

                     // ----------------------------------
                     // class MySequenceWithChoicesChoice2
                     // ----------------------------------

// CONSTANTS

const char MySequenceWithChoicesChoice2::CLASS_NAME[] = "MySequenceWithChoicesChoice2";

const bdlat_SelectionInfo MySequenceWithChoicesChoice2::SELECTION_INFO_ARRAY[] = {
    {
        SELECTION_ID_CHOICE_C,
        "choiceC",
        sizeof("choiceC") - 1,
        "",
        bdlat_FormattingMode::e_DEC
    },
    {
        SELECTION_ID_CHOICE_D,
        "choiceD",
        sizeof("choiceD") - 1,
        "",
        bdlat_FormattingMode::e_DEC
    }
};

// CLASS METHODS

const bdlat_SelectionInfo *MySequenceWithChoicesChoice2::lookupSelectionInfo(
        const char *name,
        int         nameLength)
{
    for (int i = 0; i < 2; ++i) {
        const bdlat_SelectionInfo& selectionInfo =
                    MySequenceWithChoicesChoice2::SELECTION_INFO_ARRAY[i];

        if (nameLength == selectionInfo.d_nameLength
        &&  0 == bsl::memcmp(selectionInfo.d_name_p, name, nameLength))
        {
            return &selectionInfo;
        }
    }

    return 0;
}

const bdlat_SelectionInfo *MySequenceWithChoicesChoice2::lookupSelectionInfo(int id)
{
    switch (id) {
      case SELECTION_ID_CHOICE_C:
        return &SELECTION_INFO_ARRAY[SELECTION_INDEX_CHOICE_C];
      case SELECTION_ID_CHOICE_D:
        return &SELECTION_INFO_ARRAY[SELECTION_INDEX_CHOICE_D];
      default:
        return 0;
    }
}

// CREATORS

MySequenceWithChoicesChoice2::MySequenceWithChoicesChoice2(const MySequenceWithChoicesChoice2& original)
: d_selectionId(original.d_selectionId)
{
    switch (d_selectionId) {
      case SELECTION_ID_CHOICE_C: {
        new (d_choiceC.buffer())
            int(original.d_choiceC.object());
      } break;
      case SELECTION_ID_CHOICE_D: {
        new (d_choiceD.buffer())
            int(original.d_choiceD.object());
      } break;
      default:
        BSLS_ASSERT(SELECTION_ID_UNDEFINED == d_selectionId);
    }
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
MySequenceWithChoicesChoice2::MySequenceWithChoicesChoice2(MySequenceWithChoicesChoice2&& original) noexcept
: d_selectionId(original.d_selectionId)
{
    switch (d_selectionId) {
      case SELECTION_ID_CHOICE_C: {
        new (d_choiceC.buffer())
            int(bsl::move(original.d_choiceC.object()));
      } break;
      case SELECTION_ID_CHOICE_D: {
        new (d_choiceD.buffer())
            int(bsl::move(original.d_choiceD.object()));
      } break;
      default:
        BSLS_ASSERT(SELECTION_ID_UNDEFINED == d_selectionId);
    }
}
#endif

// MANIPULATORS

MySequenceWithChoicesChoice2&
MySequenceWithChoicesChoice2::operator=(const MySequenceWithChoicesChoice2& rhs)
{
    if (this != &rhs) {
        switch (rhs.d_selectionId) {
          case SELECTION_ID_CHOICE_C: {
            makeChoiceC(rhs.d_choiceC.object());
          } break;
          case SELECTION_ID_CHOICE_D: {
            makeChoiceD(rhs.d_choiceD.object());
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
MySequenceWithChoicesChoice2&
MySequenceWithChoicesChoice2::operator=(MySequenceWithChoicesChoice2&& rhs)
{
    if (this != &rhs) {
        switch (rhs.d_selectionId) {
          case SELECTION_ID_CHOICE_C: {
            makeChoiceC(bsl::move(rhs.d_choiceC.object()));
          } break;
          case SELECTION_ID_CHOICE_D: {
            makeChoiceD(bsl::move(rhs.d_choiceD.object()));
          } break;
          default:
            BSLS_ASSERT(SELECTION_ID_UNDEFINED == rhs.d_selectionId);
            reset();
        }
    }

    return *this;
}
#endif

void MySequenceWithChoicesChoice2::reset()
{
    switch (d_selectionId) {
      case SELECTION_ID_CHOICE_C: {
        // no destruction required
      } break;
      case SELECTION_ID_CHOICE_D: {
        // no destruction required
      } break;
      default:
        BSLS_ASSERT(SELECTION_ID_UNDEFINED == d_selectionId);
    }

    d_selectionId = SELECTION_ID_UNDEFINED;
}

int MySequenceWithChoicesChoice2::makeSelection(int selectionId)
{
    switch (selectionId) {
      case SELECTION_ID_CHOICE_C: {
        makeChoiceC();
      } break;
      case SELECTION_ID_CHOICE_D: {
        makeChoiceD();
      } break;
      case SELECTION_ID_UNDEFINED: {
        reset();
      } break;
      default:
        return -1;
    }
    return 0;
}

int MySequenceWithChoicesChoice2::makeSelection(const char *name, int nameLength)
{
    const bdlat_SelectionInfo *selectionInfo =
                                         lookupSelectionInfo(name, nameLength);
    if (0 == selectionInfo) {
       return -1;
    }

    return makeSelection(selectionInfo->d_id);
}

int& MySequenceWithChoicesChoice2::makeChoiceC()
{
    if (SELECTION_ID_CHOICE_C == d_selectionId) {
        bdlat_ValueTypeFunctions::reset(&d_choiceC.object());
    }
    else {
        reset();
        new (d_choiceC.buffer())
            int();
        d_selectionId = SELECTION_ID_CHOICE_C;
    }

    return d_choiceC.object();
}

int& MySequenceWithChoicesChoice2::makeChoiceC(int value)
{
    if (SELECTION_ID_CHOICE_C == d_selectionId) {
        d_choiceC.object() = value;
    }
    else {
        reset();
        new (d_choiceC.buffer())
                int(value);
        d_selectionId = SELECTION_ID_CHOICE_C;
    }

    return d_choiceC.object();
}

int& MySequenceWithChoicesChoice2::makeChoiceD()
{
    if (SELECTION_ID_CHOICE_D == d_selectionId) {
        bdlat_ValueTypeFunctions::reset(&d_choiceD.object());
    }
    else {
        reset();
        new (d_choiceD.buffer())
            int();
        d_selectionId = SELECTION_ID_CHOICE_D;
    }

    return d_choiceD.object();
}

int& MySequenceWithChoicesChoice2::makeChoiceD(int value)
{
    if (SELECTION_ID_CHOICE_D == d_selectionId) {
        d_choiceD.object() = value;
    }
    else {
        reset();
        new (d_choiceD.buffer())
                int(value);
        d_selectionId = SELECTION_ID_CHOICE_D;
    }

    return d_choiceD.object();
}

// ACCESSORS

bsl::ostream& MySequenceWithChoicesChoice2::print(
    bsl::ostream& stream,
    int           level,
    int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    switch (d_selectionId) {
      case SELECTION_ID_CHOICE_C: {
        printer.printAttribute("choiceC", d_choiceC.object());
      }  break;
      case SELECTION_ID_CHOICE_D: {
        printer.printAttribute("choiceD", d_choiceD.object());
      }  break;
      default:
        stream << "SELECTION UNDEFINED\n";
    }
    printer.end();
    return stream;
}


const char *MySequenceWithChoicesChoice2::selectionName() const
{
    switch (d_selectionId) {
      case SELECTION_ID_CHOICE_C:
        return SELECTION_INFO_ARRAY[SELECTION_INDEX_CHOICE_C].name();
      case SELECTION_ID_CHOICE_D:
        return SELECTION_INFO_ARRAY[SELECTION_INDEX_CHOICE_D].name();
      default:
        BSLS_ASSERT(SELECTION_ID_UNDEFINED == d_selectionId);
        return "(* UNDEFINED *)";
    }
}

                        // ---------------------------
                        // class MySequenceWithChoices
                        // ---------------------------

// CONSTANTS

const char MySequenceWithChoices::CLASS_NAME[] = "MySequenceWithChoices";

const bdlat_AttributeInfo MySequenceWithChoices::ATTRIBUTE_INFO_ARRAY[] = {
    {
        ATTRIBUTE_ID_FIRST,
        "first",
        sizeof("first") - 1,
        "",
        bdlat_FormattingMode::e_DEFAULT
    },
    {
        ATTRIBUTE_ID_SECOND,
        "second",
        sizeof("second") - 1,
        "",
        bdlat_FormattingMode::e_DEFAULT
    }
};

// CLASS METHODS

const bdlat_AttributeInfo *MySequenceWithChoices::lookupAttributeInfo(
        const char *name,
        int         nameLength)
{
    for (int i = 0; i < 2; ++i) {
        const bdlat_AttributeInfo& attributeInfo =
                    MySequenceWithChoices::ATTRIBUTE_INFO_ARRAY[i];

        if (nameLength == attributeInfo.d_nameLength
        &&  0 == bsl::memcmp(attributeInfo.d_name_p, name, nameLength))
        {
            return &attributeInfo;
        }
    }

    return 0;
}

const bdlat_AttributeInfo *MySequenceWithChoices::lookupAttributeInfo(int id)
{
    switch (id) {
      case ATTRIBUTE_ID_FIRST:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_FIRST];
      case ATTRIBUTE_ID_SECOND:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SECOND];
      default:
        return 0;
    }
}

// CREATORS

MySequenceWithChoices::MySequenceWithChoices()
: d_second()
, d_first()
{
}

MySequenceWithChoices::MySequenceWithChoices(const MySequenceWithChoices& original)
: d_second(original.d_second)
, d_first(original.d_first)
{
}

MySequenceWithChoices::~MySequenceWithChoices()
{
}

// MANIPULATORS

MySequenceWithChoices&
MySequenceWithChoices::operator=(const MySequenceWithChoices& rhs)
{
    if (this != &rhs) {
        d_first = rhs.d_first;
        d_second = rhs.d_second;
    }

    return *this;
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
MySequenceWithChoices&
MySequenceWithChoices::operator=(MySequenceWithChoices&& rhs)
{
    if (this != &rhs) {
        d_first = bsl::move(rhs.d_first);
        d_second = bsl::move(rhs.d_second);
    }

    return *this;
}
#endif

void MySequenceWithChoices::reset()
{
    bdlat_ValueTypeFunctions::reset(&d_first);
    bdlat_ValueTypeFunctions::reset(&d_second);
}

// ACCESSORS

bsl::ostream& MySequenceWithChoices::print(
        bsl::ostream& stream,
        int           level,
        int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute("first", this->first());
    printer.printAttribute("second", this->second());
    printer.end();
    return stream;
}


}  // close package namespace
}  // close enterprise namespace

// GENERATED BY @BLP_BAS_CODEGEN_VERSION@
// USING bas_codegen.pl s_baltst_mysequencewithchoices.xsd --mode msg --includedir . --msgComponent mysequencewithchoices --noRecurse --noExternalization --noHashSupport --noAggregateConversion
// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright 2022 Bloomberg Finance L.P. All rights reserved.
//      Property of Bloomberg Finance L.P. (BFLP)
//      This software is made available solely pursuant to the
//      terms of a BFLP license agreement which governs its use.
// ------------------------------- END-OF-FILE --------------------------------
