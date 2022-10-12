// s_baltst_mysequencewithanonymouschoice.cpp *DO NOT EDIT* @generated -*-C++-*-

#include <bsls_ident.h>
BSLS_IDENT_RCSID(s_baltst_mysequencewithanonymouschoice_cpp, "$Id$ $CSID$")

#include <s_baltst_mysequencewithanonymouschoice.h>

#include <bdlat_formattingmode.h>
#include <bdlat_valuetypefunctions.h>
#include <bdlb_print.h>
#include <bdlb_printmethods.h>
#include <bdlb_string.h>

#include <bdlb_nullablevalue.h>
#include <bsl_string.h>
#include <bslim_printer.h>
#include <bsls_assert.h>

#include <bsl_cstring.h>
#include <bsl_iomanip.h>
#include <bsl_limits.h>
#include <bsl_ostream.h>
#include <bsl_utility.h>

namespace BloombergLP {
namespace s_baltst {

                 // -----------------------------------------
                 // class MySequenceWithAnonymousChoiceChoice
                 // -----------------------------------------

// CONSTANTS

const char MySequenceWithAnonymousChoiceChoice::CLASS_NAME[] = "MySequenceWithAnonymousChoiceChoice";

const bdlat_SelectionInfo MySequenceWithAnonymousChoiceChoice::SELECTION_INFO_ARRAY[] = {
    {
        SELECTION_ID_MY_CHOICE1,
        "myChoice1",
        sizeof("myChoice1") - 1,
        "",
        bdlat_FormattingMode::e_DEC
    },
    {
        SELECTION_ID_MY_CHOICE2,
        "myChoice2",
        sizeof("myChoice2") - 1,
        "",
        bdlat_FormattingMode::e_TEXT
    }
};

// CLASS METHODS

const bdlat_SelectionInfo *MySequenceWithAnonymousChoiceChoice::lookupSelectionInfo(
        const char *name,
        int         nameLength)
{
    for (int i = 0; i < 2; ++i) {
        const bdlat_SelectionInfo& selectionInfo =
                    MySequenceWithAnonymousChoiceChoice::SELECTION_INFO_ARRAY[i];

        if (nameLength == selectionInfo.d_nameLength
        &&  0 == bsl::memcmp(selectionInfo.d_name_p, name, nameLength))
        {
            return &selectionInfo;
        }
    }

    return 0;
}

const bdlat_SelectionInfo *MySequenceWithAnonymousChoiceChoice::lookupSelectionInfo(int id)
{
    switch (id) {
      case SELECTION_ID_MY_CHOICE1:
        return &SELECTION_INFO_ARRAY[SELECTION_INDEX_MY_CHOICE1];
      case SELECTION_ID_MY_CHOICE2:
        return &SELECTION_INFO_ARRAY[SELECTION_INDEX_MY_CHOICE2];
      default:
        return 0;
    }
}

// CREATORS

MySequenceWithAnonymousChoiceChoice::MySequenceWithAnonymousChoiceChoice(
    const MySequenceWithAnonymousChoiceChoice& original,
    bslma::Allocator *basicAllocator)
: d_selectionId(original.d_selectionId)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    switch (d_selectionId) {
      case SELECTION_ID_MY_CHOICE1: {
        new (d_myChoice1.buffer())
            int(original.d_myChoice1.object());
      } break;
      case SELECTION_ID_MY_CHOICE2: {
        new (d_myChoice2.buffer())
            bsl::string(
                original.d_myChoice2.object(), d_allocator_p);
      } break;
      default:
        BSLS_ASSERT(SELECTION_ID_UNDEFINED == d_selectionId);
    }
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
MySequenceWithAnonymousChoiceChoice::MySequenceWithAnonymousChoiceChoice(MySequenceWithAnonymousChoiceChoice&& original) noexcept
: d_selectionId(original.d_selectionId)
, d_allocator_p(original.d_allocator_p)
{
    switch (d_selectionId) {
      case SELECTION_ID_MY_CHOICE1: {
        new (d_myChoice1.buffer())
            int(bsl::move(original.d_myChoice1.object()));
      } break;
      case SELECTION_ID_MY_CHOICE2: {
        new (d_myChoice2.buffer())
            bsl::string(
                bsl::move(original.d_myChoice2.object()), d_allocator_p);
      } break;
      default:
        BSLS_ASSERT(SELECTION_ID_UNDEFINED == d_selectionId);
    }
}

MySequenceWithAnonymousChoiceChoice::MySequenceWithAnonymousChoiceChoice(
    MySequenceWithAnonymousChoiceChoice&& original,
    bslma::Allocator *basicAllocator)
: d_selectionId(original.d_selectionId)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    switch (d_selectionId) {
      case SELECTION_ID_MY_CHOICE1: {
        new (d_myChoice1.buffer())
            int(bsl::move(original.d_myChoice1.object()));
      } break;
      case SELECTION_ID_MY_CHOICE2: {
        new (d_myChoice2.buffer())
            bsl::string(
                bsl::move(original.d_myChoice2.object()), d_allocator_p);
      } break;
      default:
        BSLS_ASSERT(SELECTION_ID_UNDEFINED == d_selectionId);
    }
}
#endif

// MANIPULATORS

MySequenceWithAnonymousChoiceChoice&
MySequenceWithAnonymousChoiceChoice::operator=(const MySequenceWithAnonymousChoiceChoice& rhs)
{
    if (this != &rhs) {
        switch (rhs.d_selectionId) {
          case SELECTION_ID_MY_CHOICE1: {
            makeMyChoice1(rhs.d_myChoice1.object());
          } break;
          case SELECTION_ID_MY_CHOICE2: {
            makeMyChoice2(rhs.d_myChoice2.object());
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
MySequenceWithAnonymousChoiceChoice&
MySequenceWithAnonymousChoiceChoice::operator=(MySequenceWithAnonymousChoiceChoice&& rhs)
{
    if (this != &rhs) {
        switch (rhs.d_selectionId) {
          case SELECTION_ID_MY_CHOICE1: {
            makeMyChoice1(bsl::move(rhs.d_myChoice1.object()));
          } break;
          case SELECTION_ID_MY_CHOICE2: {
            makeMyChoice2(bsl::move(rhs.d_myChoice2.object()));
          } break;
          default:
            BSLS_ASSERT(SELECTION_ID_UNDEFINED == rhs.d_selectionId);
            reset();
        }
    }

    return *this;
}
#endif

void MySequenceWithAnonymousChoiceChoice::reset()
{
    switch (d_selectionId) {
      case SELECTION_ID_MY_CHOICE1: {
        // no destruction required
      } break;
      case SELECTION_ID_MY_CHOICE2: {
        typedef bsl::string Type;
        d_myChoice2.object().~Type();
      } break;
      default:
        BSLS_ASSERT(SELECTION_ID_UNDEFINED == d_selectionId);
    }

    d_selectionId = SELECTION_ID_UNDEFINED;
}

int MySequenceWithAnonymousChoiceChoice::makeSelection(int selectionId)
{
    switch (selectionId) {
      case SELECTION_ID_MY_CHOICE1: {
        makeMyChoice1();
      } break;
      case SELECTION_ID_MY_CHOICE2: {
        makeMyChoice2();
      } break;
      case SELECTION_ID_UNDEFINED: {
        reset();
      } break;
      default:
        return -1;
    }
    return 0;
}

int MySequenceWithAnonymousChoiceChoice::makeSelection(const char *name, int nameLength)
{
    const bdlat_SelectionInfo *selectionInfo =
                                         lookupSelectionInfo(name, nameLength);
    if (0 == selectionInfo) {
       return -1;
    }

    return makeSelection(selectionInfo->d_id);
}

int& MySequenceWithAnonymousChoiceChoice::makeMyChoice1()
{
    if (SELECTION_ID_MY_CHOICE1 == d_selectionId) {
        bdlat_ValueTypeFunctions::reset(&d_myChoice1.object());
    }
    else {
        reset();
        new (d_myChoice1.buffer())
            int();
        d_selectionId = SELECTION_ID_MY_CHOICE1;
    }

    return d_myChoice1.object();
}

int& MySequenceWithAnonymousChoiceChoice::makeMyChoice1(int value)
{
    if (SELECTION_ID_MY_CHOICE1 == d_selectionId) {
        d_myChoice1.object() = value;
    }
    else {
        reset();
        new (d_myChoice1.buffer())
                int(value);
        d_selectionId = SELECTION_ID_MY_CHOICE1;
    }

    return d_myChoice1.object();
}

bsl::string& MySequenceWithAnonymousChoiceChoice::makeMyChoice2()
{
    if (SELECTION_ID_MY_CHOICE2 == d_selectionId) {
        bdlat_ValueTypeFunctions::reset(&d_myChoice2.object());
    }
    else {
        reset();
        new (d_myChoice2.buffer())
                bsl::string(d_allocator_p);
        d_selectionId = SELECTION_ID_MY_CHOICE2;
    }

    return d_myChoice2.object();
}

bsl::string& MySequenceWithAnonymousChoiceChoice::makeMyChoice2(const bsl::string& value)
{
    if (SELECTION_ID_MY_CHOICE2 == d_selectionId) {
        d_myChoice2.object() = value;
    }
    else {
        reset();
        new (d_myChoice2.buffer())
                bsl::string(value, d_allocator_p);
        d_selectionId = SELECTION_ID_MY_CHOICE2;
    }

    return d_myChoice2.object();
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
bsl::string& MySequenceWithAnonymousChoiceChoice::makeMyChoice2(bsl::string&& value)
{
    if (SELECTION_ID_MY_CHOICE2 == d_selectionId) {
        d_myChoice2.object() = bsl::move(value);
    }
    else {
        reset();
        new (d_myChoice2.buffer())
                bsl::string(bsl::move(value), d_allocator_p);
        d_selectionId = SELECTION_ID_MY_CHOICE2;
    }

    return d_myChoice2.object();
}
#endif

// ACCESSORS

bsl::ostream& MySequenceWithAnonymousChoiceChoice::print(
    bsl::ostream& stream,
    int           level,
    int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    switch (d_selectionId) {
      case SELECTION_ID_MY_CHOICE1: {
        printer.printAttribute("myChoice1", d_myChoice1.object());
      }  break;
      case SELECTION_ID_MY_CHOICE2: {
        printer.printAttribute("myChoice2", d_myChoice2.object());
      }  break;
      default:
        stream << "SELECTION UNDEFINED\n";
    }
    printer.end();
    return stream;
}


const char *MySequenceWithAnonymousChoiceChoice::selectionName() const
{
    switch (d_selectionId) {
      case SELECTION_ID_MY_CHOICE1:
        return SELECTION_INFO_ARRAY[SELECTION_INDEX_MY_CHOICE1].name();
      case SELECTION_ID_MY_CHOICE2:
        return SELECTION_INFO_ARRAY[SELECTION_INDEX_MY_CHOICE2].name();
      default:
        BSLS_ASSERT(SELECTION_ID_UNDEFINED == d_selectionId);
        return "(* UNDEFINED *)";
    }
}

                    // -----------------------------------
                    // class MySequenceWithAnonymousChoice
                    // -----------------------------------

// CONSTANTS

const char MySequenceWithAnonymousChoice::CLASS_NAME[] = "MySequenceWithAnonymousChoice";

const bdlat_AttributeInfo MySequenceWithAnonymousChoice::ATTRIBUTE_INFO_ARRAY[] = {
    {
        ATTRIBUTE_ID_ATTRIBUTE1,
        "attribute1",
        sizeof("attribute1") - 1,
        "",
        bdlat_FormattingMode::e_DEC
    },
    {
        ATTRIBUTE_ID_CHOICE,
        "Choice",
        sizeof("Choice") - 1,
        "",
        bdlat_FormattingMode::e_DEFAULT
      | bdlat_FormattingMode::e_UNTAGGED
    },
    {
        ATTRIBUTE_ID_ATTRIBUTE2,
        "attribute2",
        sizeof("attribute2") - 1,
        "",
        bdlat_FormattingMode::e_TEXT
    }
};

// CLASS METHODS

const bdlat_AttributeInfo *MySequenceWithAnonymousChoice::lookupAttributeInfo(
        const char *name,
        int         nameLength)
{
    if (bdlb::String::areEqualCaseless("myChoice1", name, nameLength)) {
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CHOICE];
    }

    if (bdlb::String::areEqualCaseless("myChoice2", name, nameLength)) {
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CHOICE];
    }

    for (int i = 0; i < 3; ++i) {
        const bdlat_AttributeInfo& attributeInfo =
                    MySequenceWithAnonymousChoice::ATTRIBUTE_INFO_ARRAY[i];

        if (nameLength == attributeInfo.d_nameLength
        &&  0 == bsl::memcmp(attributeInfo.d_name_p, name, nameLength))
        {
            return &attributeInfo;
        }
    }

    return 0;
}

const bdlat_AttributeInfo *MySequenceWithAnonymousChoice::lookupAttributeInfo(int id)
{
    switch (id) {
      case ATTRIBUTE_ID_ATTRIBUTE1:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE1];
      case ATTRIBUTE_ID_CHOICE:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CHOICE];
      case ATTRIBUTE_ID_ATTRIBUTE2:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE2];
      default:
        return 0;
    }
}

// CREATORS

MySequenceWithAnonymousChoice::MySequenceWithAnonymousChoice(bslma::Allocator *basicAllocator)
: d_attribute2(basicAllocator)
, d_choice(basicAllocator)
, d_attribute1()
{
}

MySequenceWithAnonymousChoice::MySequenceWithAnonymousChoice(const MySequenceWithAnonymousChoice& original,
                                                             bslma::Allocator *basicAllocator)
: d_attribute2(original.d_attribute2, basicAllocator)
, d_choice(original.d_choice, basicAllocator)
, d_attribute1(original.d_attribute1)
{
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
MySequenceWithAnonymousChoice::MySequenceWithAnonymousChoice(MySequenceWithAnonymousChoice&& original) noexcept
: d_attribute2(bsl::move(original.d_attribute2))
, d_choice(bsl::move(original.d_choice))
, d_attribute1(bsl::move(original.d_attribute1))
{
}

MySequenceWithAnonymousChoice::MySequenceWithAnonymousChoice(MySequenceWithAnonymousChoice&& original,
                                                             bslma::Allocator *basicAllocator)
: d_attribute2(bsl::move(original.d_attribute2), basicAllocator)
, d_choice(bsl::move(original.d_choice), basicAllocator)
, d_attribute1(bsl::move(original.d_attribute1))
{
}
#endif

MySequenceWithAnonymousChoice::~MySequenceWithAnonymousChoice()
{
}

// MANIPULATORS

MySequenceWithAnonymousChoice&
MySequenceWithAnonymousChoice::operator=(const MySequenceWithAnonymousChoice& rhs)
{
    if (this != &rhs) {
        d_attribute1 = rhs.d_attribute1;
        d_choice = rhs.d_choice;
        d_attribute2 = rhs.d_attribute2;
    }

    return *this;
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
MySequenceWithAnonymousChoice&
MySequenceWithAnonymousChoice::operator=(MySequenceWithAnonymousChoice&& rhs)
{
    if (this != &rhs) {
        d_attribute1 = bsl::move(rhs.d_attribute1);
        d_choice = bsl::move(rhs.d_choice);
        d_attribute2 = bsl::move(rhs.d_attribute2);
    }

    return *this;
}
#endif

void MySequenceWithAnonymousChoice::reset()
{
    bdlat_ValueTypeFunctions::reset(&d_attribute1);
    bdlat_ValueTypeFunctions::reset(&d_choice);
    bdlat_ValueTypeFunctions::reset(&d_attribute2);
}

// ACCESSORS

bsl::ostream& MySequenceWithAnonymousChoice::print(
        bsl::ostream& stream,
        int           level,
        int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute("attribute1", this->attribute1());
    printer.printAttribute("choice", this->choice());
    printer.printAttribute("attribute2", this->attribute2());
    printer.end();
    return stream;
}


}  // close package namespace
}  // close enterprise namespace

// GENERATED BY @BLP_BAS_CODEGEN_VERSION@
// USING bas_codegen.pl s_baltst_mysequencewithanonymouschoice.xsd --mode msg --includedir . --msgComponent mysequencewithanonymouschoice --noRecurse --noExternalization --noHashSupport --noAggregateConversion
// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright 2022 Bloomberg Finance L.P. All rights reserved.
//      Property of Bloomberg Finance L.P. (BFLP)
//      This software is made available solely pursuant to the
//      terms of a BFLP license agreement which governs its use.
// ------------------------------- END-OF-FILE --------------------------------
