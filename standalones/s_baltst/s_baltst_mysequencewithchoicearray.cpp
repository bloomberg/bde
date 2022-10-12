// s_baltst_mysequencewithchoicearray.cpp  *DO NOT EDIT*   @generated -*-C++-*-

#include <bsls_ident.h>
BSLS_IDENT_RCSID(s_baltst_mysequencewithchoicearray_cpp, "$Id$ $CSID$")

#include <s_baltst_mysequencewithchoicearray.h>

#include <bdlat_formattingmode.h>
#include <bdlat_valuetypefunctions.h>
#include <bdlb_print.h>
#include <bdlb_printmethods.h>
#include <bdlb_string.h>

#include <bsl_vector.h>
#include <bslim_printer.h>
#include <bsls_assert.h>

#include <bsl_cstring.h>
#include <bsl_iomanip.h>
#include <bsl_limits.h>
#include <bsl_ostream.h>
#include <bsl_utility.h>

namespace BloombergLP {
namespace s_baltst {

                   // -------------------------------------
                   // class MySequenceWithChoiceArrayChoice
                   // -------------------------------------

// CONSTANTS

const char MySequenceWithChoiceArrayChoice::CLASS_NAME[] = "MySequenceWithChoiceArrayChoice";

const bdlat_SelectionInfo MySequenceWithChoiceArrayChoice::SELECTION_INFO_ARRAY[] = {
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

const bdlat_SelectionInfo *MySequenceWithChoiceArrayChoice::lookupSelectionInfo(
        const char *name,
        int         nameLength)
{
    for (int i = 0; i < 2; ++i) {
        const bdlat_SelectionInfo& selectionInfo =
                    MySequenceWithChoiceArrayChoice::SELECTION_INFO_ARRAY[i];

        if (nameLength == selectionInfo.d_nameLength
        &&  0 == bsl::memcmp(selectionInfo.d_name_p, name, nameLength))
        {
            return &selectionInfo;
        }
    }

    return 0;
}

const bdlat_SelectionInfo *MySequenceWithChoiceArrayChoice::lookupSelectionInfo(int id)
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

MySequenceWithChoiceArrayChoice::MySequenceWithChoiceArrayChoice(const MySequenceWithChoiceArrayChoice& original)
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
MySequenceWithChoiceArrayChoice::MySequenceWithChoiceArrayChoice(MySequenceWithChoiceArrayChoice&& original) noexcept
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

MySequenceWithChoiceArrayChoice&
MySequenceWithChoiceArrayChoice::operator=(const MySequenceWithChoiceArrayChoice& rhs)
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
MySequenceWithChoiceArrayChoice&
MySequenceWithChoiceArrayChoice::operator=(MySequenceWithChoiceArrayChoice&& rhs)
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

void MySequenceWithChoiceArrayChoice::reset()
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

int MySequenceWithChoiceArrayChoice::makeSelection(int selectionId)
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

int MySequenceWithChoiceArrayChoice::makeSelection(const char *name, int nameLength)
{
    const bdlat_SelectionInfo *selectionInfo =
                                         lookupSelectionInfo(name, nameLength);
    if (0 == selectionInfo) {
       return -1;
    }

    return makeSelection(selectionInfo->d_id);
}

int& MySequenceWithChoiceArrayChoice::makeChoiceA()
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

int& MySequenceWithChoiceArrayChoice::makeChoiceA(int value)
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

int& MySequenceWithChoiceArrayChoice::makeChoiceB()
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

int& MySequenceWithChoiceArrayChoice::makeChoiceB(int value)
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

bsl::ostream& MySequenceWithChoiceArrayChoice::print(
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


const char *MySequenceWithChoiceArrayChoice::selectionName() const
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

                      // -------------------------------
                      // class MySequenceWithChoiceArray
                      // -------------------------------

// CONSTANTS

const char MySequenceWithChoiceArray::CLASS_NAME[] = "MySequenceWithChoiceArray";

const bdlat_AttributeInfo MySequenceWithChoiceArray::ATTRIBUTE_INFO_ARRAY[] = {
    {
        ATTRIBUTE_ID_CHOICES,
        "choices",
        sizeof("choices") - 1,
        "",
        bdlat_FormattingMode::e_DEFAULT
    }
};

// CLASS METHODS

const bdlat_AttributeInfo *MySequenceWithChoiceArray::lookupAttributeInfo(
        const char *name,
        int         nameLength)
{
    for (int i = 0; i < 1; ++i) {
        const bdlat_AttributeInfo& attributeInfo =
                    MySequenceWithChoiceArray::ATTRIBUTE_INFO_ARRAY[i];

        if (nameLength == attributeInfo.d_nameLength
        &&  0 == bsl::memcmp(attributeInfo.d_name_p, name, nameLength))
        {
            return &attributeInfo;
        }
    }

    return 0;
}

const bdlat_AttributeInfo *MySequenceWithChoiceArray::lookupAttributeInfo(int id)
{
    switch (id) {
      case ATTRIBUTE_ID_CHOICES:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CHOICES];
      default:
        return 0;
    }
}

// CREATORS

MySequenceWithChoiceArray::MySequenceWithChoiceArray(bslma::Allocator *basicAllocator)
: d_choices(basicAllocator)
{
}

MySequenceWithChoiceArray::MySequenceWithChoiceArray(const MySequenceWithChoiceArray& original,
                                                     bslma::Allocator *basicAllocator)
: d_choices(original.d_choices, basicAllocator)
{
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
MySequenceWithChoiceArray::MySequenceWithChoiceArray(MySequenceWithChoiceArray&& original) noexcept
: d_choices(bsl::move(original.d_choices))
{
}

MySequenceWithChoiceArray::MySequenceWithChoiceArray(MySequenceWithChoiceArray&& original,
                                                     bslma::Allocator *basicAllocator)
: d_choices(bsl::move(original.d_choices), basicAllocator)
{
}
#endif

MySequenceWithChoiceArray::~MySequenceWithChoiceArray()
{
}

// MANIPULATORS

MySequenceWithChoiceArray&
MySequenceWithChoiceArray::operator=(const MySequenceWithChoiceArray& rhs)
{
    if (this != &rhs) {
        d_choices = rhs.d_choices;
    }

    return *this;
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
MySequenceWithChoiceArray&
MySequenceWithChoiceArray::operator=(MySequenceWithChoiceArray&& rhs)
{
    if (this != &rhs) {
        d_choices = bsl::move(rhs.d_choices);
    }

    return *this;
}
#endif

void MySequenceWithChoiceArray::reset()
{
    bdlat_ValueTypeFunctions::reset(&d_choices);
}

// ACCESSORS

bsl::ostream& MySequenceWithChoiceArray::print(
        bsl::ostream& stream,
        int           level,
        int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute("choices", this->choices());
    printer.end();
    return stream;
}


}  // close package namespace
}  // close enterprise namespace

// GENERATED BY @BLP_BAS_CODEGEN_VERSION@
// USING bas_codegen.pl s_baltst_mysequencewithchoicearray.xsd --mode msg --includedir . --msgComponent mysequencewithchoicearray --noRecurse --noExternalization --noHashSupport --noAggregateConversion
// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright 2022 Bloomberg Finance L.P. All rights reserved.
//      Property of Bloomberg Finance L.P. (BFLP)
//      This software is made available solely pursuant to the
//      terms of a BFLP license agreement which governs its use.
// ------------------------------- END-OF-FILE --------------------------------
