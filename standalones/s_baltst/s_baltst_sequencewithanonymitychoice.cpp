// s_baltst_sequencewithanonymitychoice.cpp        *DO NOT EDIT*         @generated -*-C++-*-

#include <bsls_ident.h>
BSLS_IDENT_RCSID(s_baltst_sequencewithanonymitychoice_cpp,"$Id$ $CSID$")

#include <s_baltst_sequencewithanonymitychoice.h>

#include <bdlat_formattingmode.h>
#include <bdlat_valuetypefunctions.h>
#include <bdlb_print.h>
#include <bdlb_printmethods.h>
#include <bdlb_string.h>

#include <s_baltst_address.h>
#include <s_baltst_basicrecord.h>
#include <s_baltst_customint.h>
#include <s_baltst_customstring.h>
#include <s_baltst_customizedstring.h>
#include <s_baltst_enumerated.h>
#include <s_baltst_mychoice.h>
#include <s_baltst_myenumeration.h>
#include <s_baltst_mysequence.h>
#include <s_baltst_mysequencewithanonymouschoicechoice.h>
#include <s_baltst_mysequencewitharray.h>
#include <s_baltst_mysequencewithattributes.h>
#include <s_baltst_mysequencewithnillable.h>
#include <s_baltst_mysequencewithnullable.h>
#include <s_baltst_mysimplecontent.h>
#include <s_baltst_mysimpleintcontent.h>
#include <s_baltst_rawdata.h>
#include <s_baltst_rawdataswitched.h>
#include <s_baltst_rawdataunformatted.h>
#include <s_baltst_sequencewithanonymitychoice1.h>
#include <s_baltst_simplerequest.h>
#include <s_baltst_sqrt.h>
#include <s_baltst_unsignedsequence.h>
#include <s_baltst_voidsequence.h>
#include <s_baltst_bigrecord.h>
#include <s_baltst_employee.h>
#include <s_baltst_mysequencewithanonymouschoice.h>
#include <s_baltst_mysequencewithnillables.h>
#include <s_baltst_mysequencewithnullables.h>
#include <s_baltst_ratsnest.h>
#include <s_baltst_ratsnest.h>
#include <s_baltst_ratsnest.h>
#include <s_baltst_ratsnest.h>
#include <bslim_printer.h>
#include <bsls_assert.h>

#include <bsl_iomanip.h>
#include <bsl_limits.h>
#include <bsl_ostream.h>

namespace BloombergLP {
namespace s_baltst {

                     // ---------------------------------
                     // class SequenceWithAnonymityChoice
                     // ---------------------------------

// CONSTANTS

const char SequenceWithAnonymityChoice::CLASS_NAME[] = "SequenceWithAnonymityChoice";

const bdlat_SelectionInfo SequenceWithAnonymityChoice::SELECTION_INFO_ARRAY[] = {
    {
        SELECTION_ID_SELECTION1,
        "selection1",
        sizeof("selection1") - 1,
        "",
        bdlat_FormattingMode::e_DEFAULT
    },
    {
        SELECTION_ID_SELECTION2,
        "selection2",
        sizeof("selection2") - 1,
        "",
        bdlat_FormattingMode::e_DEC
    },
    {
        SELECTION_ID_SELECTION3,
        "selection3",
        sizeof("selection3") - 1,
        "",
        bdlat_FormattingMode::e_TEXT
    },
    {
        SELECTION_ID_SELECTION4,
        "selection4",
        sizeof("selection4") - 1,
        "",
        bdlat_FormattingMode::e_DEC
    }
};

// CLASS METHODS

const bdlat_SelectionInfo *SequenceWithAnonymityChoice::lookupSelectionInfo(
        const char *name,
        int         nameLength)
{
    for (int i = 0; i < 4; ++i) {
        const bdlat_SelectionInfo& selectionInfo =
                    SequenceWithAnonymityChoice::SELECTION_INFO_ARRAY[i];

        if (nameLength == selectionInfo.d_nameLength
        &&  0 == bsl::memcmp(selectionInfo.d_name_p, name, nameLength))
        {
            return &selectionInfo;
        }
    }

    return 0;
}

const bdlat_SelectionInfo *SequenceWithAnonymityChoice::lookupSelectionInfo(int id)
{
    switch (id) {
      case SELECTION_ID_SELECTION1:
        return &SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION1];
      case SELECTION_ID_SELECTION2:
        return &SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION2];
      case SELECTION_ID_SELECTION3:
        return &SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION3];
      case SELECTION_ID_SELECTION4:
        return &SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION4];
      default:
        return 0;
    }
}

// CREATORS

SequenceWithAnonymityChoice::SequenceWithAnonymityChoice(
    const SequenceWithAnonymityChoice& original,
    bslma::Allocator *basicAllocator)
: d_selectionId(original.d_selectionId)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    switch (d_selectionId) {
      case SELECTION_ID_SELECTION1: {
        new (d_selection1.buffer())
            Sequence6(
                original.d_selection1.object(), d_allocator_p);
      } break;
      case SELECTION_ID_SELECTION2: {
        new (d_selection2.buffer())
            unsigned char(original.d_selection2.object());
      } break;
      case SELECTION_ID_SELECTION3: {
        new (d_selection3.buffer())
            CustomString(
                original.d_selection3.object(), d_allocator_p);
      } break;
      case SELECTION_ID_SELECTION4: {
        new (d_selection4.buffer())
            CustomInt(original.d_selection4.object());
      } break;
      default:
        BSLS_ASSERT(SELECTION_ID_UNDEFINED == d_selectionId);
    }
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
SequenceWithAnonymityChoice::SequenceWithAnonymityChoice(SequenceWithAnonymityChoice&& original) noexcept
: d_selectionId(original.d_selectionId)
, d_allocator_p(original.d_allocator_p)
{
    switch (d_selectionId) {
      case SELECTION_ID_SELECTION1: {
        new (d_selection1.buffer())
            Sequence6(
                bsl::move(original.d_selection1.object()), d_allocator_p);
      } break;
      case SELECTION_ID_SELECTION2: {
        new (d_selection2.buffer())
            unsigned char(bsl::move(original.d_selection2.object()));
      } break;
      case SELECTION_ID_SELECTION3: {
        new (d_selection3.buffer())
            CustomString(
                bsl::move(original.d_selection3.object()), d_allocator_p);
      } break;
      case SELECTION_ID_SELECTION4: {
        new (d_selection4.buffer())
            CustomInt(bsl::move(original.d_selection4.object()));
      } break;
      default:
        BSLS_ASSERT(SELECTION_ID_UNDEFINED == d_selectionId);
    }
}

SequenceWithAnonymityChoice::SequenceWithAnonymityChoice(
    SequenceWithAnonymityChoice&& original,
    bslma::Allocator *basicAllocator)
: d_selectionId(original.d_selectionId)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    switch (d_selectionId) {
      case SELECTION_ID_SELECTION1: {
        new (d_selection1.buffer())
            Sequence6(
                bsl::move(original.d_selection1.object()), d_allocator_p);
      } break;
      case SELECTION_ID_SELECTION2: {
        new (d_selection2.buffer())
            unsigned char(bsl::move(original.d_selection2.object()));
      } break;
      case SELECTION_ID_SELECTION3: {
        new (d_selection3.buffer())
            CustomString(
                bsl::move(original.d_selection3.object()), d_allocator_p);
      } break;
      case SELECTION_ID_SELECTION4: {
        new (d_selection4.buffer())
            CustomInt(bsl::move(original.d_selection4.object()));
      } break;
      default:
        BSLS_ASSERT(SELECTION_ID_UNDEFINED == d_selectionId);
    }
}
#endif

// MANIPULATORS

SequenceWithAnonymityChoice&
SequenceWithAnonymityChoice::operator=(const SequenceWithAnonymityChoice& rhs)
{
    if (this != &rhs) {
        switch (rhs.d_selectionId) {
          case SELECTION_ID_SELECTION1: {
            makeSelection1(rhs.d_selection1.object());
          } break;
          case SELECTION_ID_SELECTION2: {
            makeSelection2(rhs.d_selection2.object());
          } break;
          case SELECTION_ID_SELECTION3: {
            makeSelection3(rhs.d_selection3.object());
          } break;
          case SELECTION_ID_SELECTION4: {
            makeSelection4(rhs.d_selection4.object());
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
SequenceWithAnonymityChoice&
SequenceWithAnonymityChoice::operator=(SequenceWithAnonymityChoice&& rhs)
{
    if (this != &rhs) {
        switch (rhs.d_selectionId) {
          case SELECTION_ID_SELECTION1: {
            makeSelection1(bsl::move(rhs.d_selection1.object()));
          } break;
          case SELECTION_ID_SELECTION2: {
            makeSelection2(bsl::move(rhs.d_selection2.object()));
          } break;
          case SELECTION_ID_SELECTION3: {
            makeSelection3(bsl::move(rhs.d_selection3.object()));
          } break;
          case SELECTION_ID_SELECTION4: {
            makeSelection4(bsl::move(rhs.d_selection4.object()));
          } break;
          default:
            BSLS_ASSERT(SELECTION_ID_UNDEFINED == rhs.d_selectionId);
            reset();
        }
    }

    return *this;
}
#endif

void SequenceWithAnonymityChoice::reset()
{
    switch (d_selectionId) {
      case SELECTION_ID_SELECTION1: {
        d_selection1.object().~Sequence6();
      } break;
      case SELECTION_ID_SELECTION2: {
        // no destruction required
      } break;
      case SELECTION_ID_SELECTION3: {
        d_selection3.object().~CustomString();
      } break;
      case SELECTION_ID_SELECTION4: {
        d_selection4.object().~CustomInt();
      } break;
      default:
        BSLS_ASSERT(SELECTION_ID_UNDEFINED == d_selectionId);
    }

    d_selectionId = SELECTION_ID_UNDEFINED;
}

int SequenceWithAnonymityChoice::makeSelection(int selectionId)
{
    switch (selectionId) {
      case SELECTION_ID_SELECTION1: {
        makeSelection1();
      } break;
      case SELECTION_ID_SELECTION2: {
        makeSelection2();
      } break;
      case SELECTION_ID_SELECTION3: {
        makeSelection3();
      } break;
      case SELECTION_ID_SELECTION4: {
        makeSelection4();
      } break;
      case SELECTION_ID_UNDEFINED: {
        reset();
      } break;
      default:
        return -1;
    }
    return 0;
}

int SequenceWithAnonymityChoice::makeSelection(const char *name, int nameLength)
{
    const bdlat_SelectionInfo *selectionInfo =
                                         lookupSelectionInfo(name, nameLength);
    if (0 == selectionInfo) {
       return -1;
    }

    return makeSelection(selectionInfo->d_id);
}

Sequence6& SequenceWithAnonymityChoice::makeSelection1()
{
    if (SELECTION_ID_SELECTION1 == d_selectionId) {
        bdlat_ValueTypeFunctions::reset(&d_selection1.object());
    }
    else {
        reset();
        new (d_selection1.buffer())
                Sequence6(d_allocator_p);
        d_selectionId = SELECTION_ID_SELECTION1;
    }

    return d_selection1.object();
}

Sequence6& SequenceWithAnonymityChoice::makeSelection1(const Sequence6& value)
{
    if (SELECTION_ID_SELECTION1 == d_selectionId) {
        d_selection1.object() = value;
    }
    else {
        reset();
        new (d_selection1.buffer())
                Sequence6(value, d_allocator_p);
        d_selectionId = SELECTION_ID_SELECTION1;
    }

    return d_selection1.object();
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
Sequence6& SequenceWithAnonymityChoice::makeSelection1(Sequence6&& value)
{
    if (SELECTION_ID_SELECTION1 == d_selectionId) {
        d_selection1.object() = bsl::move(value);
    }
    else {
        reset();
        new (d_selection1.buffer())
                Sequence6(bsl::move(value), d_allocator_p);
        d_selectionId = SELECTION_ID_SELECTION1;
    }

    return d_selection1.object();
}
#endif

unsigned char& SequenceWithAnonymityChoice::makeSelection2()
{
    if (SELECTION_ID_SELECTION2 == d_selectionId) {
        bdlat_ValueTypeFunctions::reset(&d_selection2.object());
    }
    else {
        reset();
        new (d_selection2.buffer())
            unsigned char();
        d_selectionId = SELECTION_ID_SELECTION2;
    }

    return d_selection2.object();
}

unsigned char& SequenceWithAnonymityChoice::makeSelection2(unsigned char value)
{
    if (SELECTION_ID_SELECTION2 == d_selectionId) {
        d_selection2.object() = value;
    }
    else {
        reset();
        new (d_selection2.buffer())
                unsigned char(value);
        d_selectionId = SELECTION_ID_SELECTION2;
    }

    return d_selection2.object();
}

CustomString& SequenceWithAnonymityChoice::makeSelection3()
{
    if (SELECTION_ID_SELECTION3 == d_selectionId) {
        bdlat_ValueTypeFunctions::reset(&d_selection3.object());
    }
    else {
        reset();
        new (d_selection3.buffer())
                CustomString(d_allocator_p);
        d_selectionId = SELECTION_ID_SELECTION3;
    }

    return d_selection3.object();
}

CustomString& SequenceWithAnonymityChoice::makeSelection3(const CustomString& value)
{
    if (SELECTION_ID_SELECTION3 == d_selectionId) {
        d_selection3.object() = value;
    }
    else {
        reset();
        new (d_selection3.buffer())
                CustomString(value, d_allocator_p);
        d_selectionId = SELECTION_ID_SELECTION3;
    }

    return d_selection3.object();
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
CustomString& SequenceWithAnonymityChoice::makeSelection3(CustomString&& value)
{
    if (SELECTION_ID_SELECTION3 == d_selectionId) {
        d_selection3.object() = bsl::move(value);
    }
    else {
        reset();
        new (d_selection3.buffer())
                CustomString(bsl::move(value), d_allocator_p);
        d_selectionId = SELECTION_ID_SELECTION3;
    }

    return d_selection3.object();
}
#endif

CustomInt& SequenceWithAnonymityChoice::makeSelection4()
{
    if (SELECTION_ID_SELECTION4 == d_selectionId) {
        bdlat_ValueTypeFunctions::reset(&d_selection4.object());
    }
    else {
        reset();
        new (d_selection4.buffer())
            CustomInt();
        d_selectionId = SELECTION_ID_SELECTION4;
    }

    return d_selection4.object();
}

CustomInt& SequenceWithAnonymityChoice::makeSelection4(const CustomInt& value)
{
    if (SELECTION_ID_SELECTION4 == d_selectionId) {
        d_selection4.object() = value;
    }
    else {
        reset();
        new (d_selection4.buffer())
                CustomInt(value);
        d_selectionId = SELECTION_ID_SELECTION4;
    }

    return d_selection4.object();
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
CustomInt& SequenceWithAnonymityChoice::makeSelection4(CustomInt&& value)
{
    if (SELECTION_ID_SELECTION4 == d_selectionId) {
        d_selection4.object() = bsl::move(value);
    }
    else {
        reset();
        new (d_selection4.buffer())
                CustomInt(bsl::move(value));
        d_selectionId = SELECTION_ID_SELECTION4;
    }

    return d_selection4.object();
}
#endif

// ACCESSORS

bsl::ostream& SequenceWithAnonymityChoice::print(
    bsl::ostream& stream,
    int           level,
    int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    switch (d_selectionId) {
      case SELECTION_ID_SELECTION1: {
        printer.printAttribute("selection1", d_selection1.object());
      }  break;
      case SELECTION_ID_SELECTION2: {
        printer.printAttribute("selection2", (int)d_selection2.object());
      }  break;
      case SELECTION_ID_SELECTION3: {
        printer.printAttribute("selection3", d_selection3.object());
      }  break;
      case SELECTION_ID_SELECTION4: {
        printer.printAttribute("selection4", d_selection4.object());
      }  break;
      default:
        stream << "SELECTION UNDEFINED\n";
    }
    printer.end();
    return stream;
}


const char *SequenceWithAnonymityChoice::selectionName() const
{
    switch (d_selectionId) {
      case SELECTION_ID_SELECTION1:
        return SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION1].name();
      case SELECTION_ID_SELECTION2:
        return SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION2].name();
      case SELECTION_ID_SELECTION3:
        return SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION3].name();
      case SELECTION_ID_SELECTION4:
        return SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION4].name();
      default:
        BSLS_ASSERT(SELECTION_ID_UNDEFINED == d_selectionId);
        return "(* UNDEFINED *)";
    }
}
}  // close package namespace
}  // close enterprise namespace

// GENERATED BY BLP_BAS_CODEGEN_2020.03.30
// USING bas_codegen.pl s_baltst.xsd -m msg -p s_baltst -C tmp_singles --msgSplit 1 --noExternalization --noHashSupport --noAggregateConversion
// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright 2020 Bloomberg Finance L.P. All rights reserved.
//      Property of Bloomberg Finance L.P. (BFLP)
//      This software is made available solely pursuant to the
//      terms of a BFLP license agreement which governs its use.
// ------------------------------- END-OF-FILE --------------------------------
