// s_baltst_sequencewithanonymitychoice2.cpp        *DO NOT EDIT*         @generated -*-C++-*-

#include <bsls_ident.h>
BSLS_IDENT_RCSID(s_baltst_sequencewithanonymitychoice2_cpp,"$Id$ $CSID$")

#include <s_baltst_sequencewithanonymitychoice2.h>

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
#include <s_baltst_sequencewithanonymitychoice.h>
#include <s_baltst_timingrequest.h>
#include <s_baltst_ratsnest.h>
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

                     // ----------------------------------
                     // class SequenceWithAnonymityChoice2
                     // ----------------------------------

// CONSTANTS

const char SequenceWithAnonymityChoice2::CLASS_NAME[] = "SequenceWithAnonymityChoice2";

const bdlat_SelectionInfo SequenceWithAnonymityChoice2::SELECTION_INFO_ARRAY[] = {
    {
        SELECTION_ID_SELECTION7,
        "selection7",
        sizeof("selection7") - 1,
        "",
        bdlat_FormattingMode::e_DEFAULT
    },
    {
        SELECTION_ID_SELECTION8,
        "selection8",
        sizeof("selection8") - 1,
        "",
        bdlat_FormattingMode::e_DEFAULT
    }
};

// CLASS METHODS

const bdlat_SelectionInfo *SequenceWithAnonymityChoice2::lookupSelectionInfo(
        const char *name,
        int         nameLength)
{
    for (int i = 0; i < 2; ++i) {
        const bdlat_SelectionInfo& selectionInfo =
                    SequenceWithAnonymityChoice2::SELECTION_INFO_ARRAY[i];

        if (nameLength == selectionInfo.d_nameLength
        &&  0 == bsl::memcmp(selectionInfo.d_name_p, name, nameLength))
        {
            return &selectionInfo;
        }
    }

    return 0;
}

const bdlat_SelectionInfo *SequenceWithAnonymityChoice2::lookupSelectionInfo(int id)
{
    switch (id) {
      case SELECTION_ID_SELECTION7:
        return &SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION7];
      case SELECTION_ID_SELECTION8:
        return &SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION8];
      default:
        return 0;
    }
}

// CREATORS

SequenceWithAnonymityChoice2::SequenceWithAnonymityChoice2(
    const SequenceWithAnonymityChoice2& original,
    bslma::Allocator *basicAllocator)
: d_selectionId(original.d_selectionId)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    switch (d_selectionId) {
      case SELECTION_ID_SELECTION7: {
        d_selection7 = new (*d_allocator_p)
                Sequence4(*original.d_selection7, d_allocator_p);
      } break;
      case SELECTION_ID_SELECTION8: {
        d_selection8 = new (*d_allocator_p)
                Choice2(*original.d_selection8, d_allocator_p);
      } break;
      default:
        BSLS_ASSERT(SELECTION_ID_UNDEFINED == d_selectionId);
    }
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
SequenceWithAnonymityChoice2::SequenceWithAnonymityChoice2(SequenceWithAnonymityChoice2&& original) noexcept
: d_selectionId(original.d_selectionId)
, d_allocator_p(original.d_allocator_p)
{
    switch (d_selectionId) {
      case SELECTION_ID_SELECTION7: {
        d_selection7 = original.d_selection7;
        original.d_selectionId = SELECTION_ID_UNDEFINED;
      } break;
      case SELECTION_ID_SELECTION8: {
        d_selection8 = original.d_selection8;
        original.d_selectionId = SELECTION_ID_UNDEFINED;
      } break;
      default:
        BSLS_ASSERT(SELECTION_ID_UNDEFINED == d_selectionId);
    }
}

SequenceWithAnonymityChoice2::SequenceWithAnonymityChoice2(
    SequenceWithAnonymityChoice2&& original,
    bslma::Allocator *basicAllocator)
: d_selectionId(original.d_selectionId)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    switch (d_selectionId) {
      case SELECTION_ID_SELECTION7: {
        if (d_allocator_p == original.d_allocator_p) {
            d_selection7 = original.d_selection7;
            original.d_selectionId = SELECTION_ID_UNDEFINED;
        }
        else {
            d_selection7 = new (*d_allocator_p)
                    Sequence4(bsl::move(*original.d_selection7), d_allocator_p);
        }
      } break;
      case SELECTION_ID_SELECTION8: {
        if (d_allocator_p == original.d_allocator_p) {
            d_selection8 = original.d_selection8;
            original.d_selectionId = SELECTION_ID_UNDEFINED;
        }
        else {
            d_selection8 = new (*d_allocator_p)
                    Choice2(bsl::move(*original.d_selection8), d_allocator_p);
        }
      } break;
      default:
        BSLS_ASSERT(SELECTION_ID_UNDEFINED == d_selectionId);
    }
}
#endif

// MANIPULATORS

SequenceWithAnonymityChoice2&
SequenceWithAnonymityChoice2::operator=(const SequenceWithAnonymityChoice2& rhs)
{
    if (this != &rhs) {
        switch (rhs.d_selectionId) {
          case SELECTION_ID_SELECTION7: {
            makeSelection7(*rhs.d_selection7);
          } break;
          case SELECTION_ID_SELECTION8: {
            makeSelection8(*rhs.d_selection8);
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
SequenceWithAnonymityChoice2&
SequenceWithAnonymityChoice2::operator=(SequenceWithAnonymityChoice2&& rhs)
{
    if (this != &rhs) {
        switch (rhs.d_selectionId) {
          case SELECTION_ID_SELECTION7: {
            if (d_allocator_p == rhs.d_allocator_p) {
                reset();
                d_selection7 = rhs.d_selection7;
                d_selectionId = SELECTION_ID_SELECTION7;
                rhs.d_selectionId = SELECTION_ID_UNDEFINED;
            }
            else {
                makeSelection7(bsl::move(*rhs.d_selection7));
            }
          } break;
          case SELECTION_ID_SELECTION8: {
            if (d_allocator_p == rhs.d_allocator_p) {
                reset();
                d_selection8 = rhs.d_selection8;
                d_selectionId = SELECTION_ID_SELECTION8;
                rhs.d_selectionId = SELECTION_ID_UNDEFINED;
            }
            else {
                makeSelection8(bsl::move(*rhs.d_selection8));
            }
          } break;
          default:
            BSLS_ASSERT(SELECTION_ID_UNDEFINED == rhs.d_selectionId);
            reset();
        }
    }

    return *this;
}
#endif

void SequenceWithAnonymityChoice2::reset()
{
    switch (d_selectionId) {
      case SELECTION_ID_SELECTION7: {
        d_allocator_p->deleteObject(d_selection7);
      } break;
      case SELECTION_ID_SELECTION8: {
        d_allocator_p->deleteObject(d_selection8);
      } break;
      default:
        BSLS_ASSERT(SELECTION_ID_UNDEFINED == d_selectionId);
    }

    d_selectionId = SELECTION_ID_UNDEFINED;
}

int SequenceWithAnonymityChoice2::makeSelection(int selectionId)
{
    switch (selectionId) {
      case SELECTION_ID_SELECTION7: {
        makeSelection7();
      } break;
      case SELECTION_ID_SELECTION8: {
        makeSelection8();
      } break;
      case SELECTION_ID_UNDEFINED: {
        reset();
      } break;
      default:
        return -1;
    }
    return 0;
}

int SequenceWithAnonymityChoice2::makeSelection(const char *name, int nameLength)
{
    const bdlat_SelectionInfo *selectionInfo =
                                         lookupSelectionInfo(name, nameLength);
    if (0 == selectionInfo) {
       return -1;
    }

    return makeSelection(selectionInfo->d_id);
}

Sequence4& SequenceWithAnonymityChoice2::makeSelection7()
{
    if (SELECTION_ID_SELECTION7 == d_selectionId) {
        bdlat_ValueTypeFunctions::reset(d_selection7);
    }
    else {
        reset();
        d_selection7 = new (*d_allocator_p)
                Sequence4(d_allocator_p);
        d_selectionId = SELECTION_ID_SELECTION7;
    }

    return *d_selection7;
}

Sequence4& SequenceWithAnonymityChoice2::makeSelection7(const Sequence4& value)
{
    if (SELECTION_ID_SELECTION7 == d_selectionId) {
        *d_selection7 = value;
    }
    else {
        reset();
        d_selection7 = new (*d_allocator_p)
                Sequence4(value, d_allocator_p);
        d_selectionId = SELECTION_ID_SELECTION7;
    }

    return *d_selection7;
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
Sequence4& SequenceWithAnonymityChoice2::makeSelection7(Sequence4&& value)
{
    if (SELECTION_ID_SELECTION7 == d_selectionId) {
        *d_selection7 = bsl::move(value);
    }
    else {
        reset();
        d_selection7 = new (*d_allocator_p)
                Sequence4(bsl::move(value), d_allocator_p);
        d_selectionId = SELECTION_ID_SELECTION7;
    }

    return *d_selection7;
}
#endif

Choice2& SequenceWithAnonymityChoice2::makeSelection8()
{
    if (SELECTION_ID_SELECTION8 == d_selectionId) {
        bdlat_ValueTypeFunctions::reset(d_selection8);
    }
    else {
        reset();
        d_selection8 = new (*d_allocator_p)
                Choice2(d_allocator_p);
        d_selectionId = SELECTION_ID_SELECTION8;
    }

    return *d_selection8;
}

Choice2& SequenceWithAnonymityChoice2::makeSelection8(const Choice2& value)
{
    if (SELECTION_ID_SELECTION8 == d_selectionId) {
        *d_selection8 = value;
    }
    else {
        reset();
        d_selection8 = new (*d_allocator_p)
                Choice2(value, d_allocator_p);
        d_selectionId = SELECTION_ID_SELECTION8;
    }

    return *d_selection8;
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
Choice2& SequenceWithAnonymityChoice2::makeSelection8(Choice2&& value)
{
    if (SELECTION_ID_SELECTION8 == d_selectionId) {
        *d_selection8 = bsl::move(value);
    }
    else {
        reset();
        d_selection8 = new (*d_allocator_p)
                Choice2(bsl::move(value), d_allocator_p);
        d_selectionId = SELECTION_ID_SELECTION8;
    }

    return *d_selection8;
}
#endif

// ACCESSORS

bsl::ostream& SequenceWithAnonymityChoice2::print(
    bsl::ostream& stream,
    int           level,
    int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    switch (d_selectionId) {
      case SELECTION_ID_SELECTION7: {
    printer.printAttribute("selection7", *d_selection7);
      }  break;
      case SELECTION_ID_SELECTION8: {
    printer.printAttribute("selection8", *d_selection8);
      }  break;
      default:
        stream << "SELECTION UNDEFINED\n";
    }
    printer.end();
    return stream;
}


const char *SequenceWithAnonymityChoice2::selectionName() const
{
    switch (d_selectionId) {
      case SELECTION_ID_SELECTION7:
        return SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION7].name();
      case SELECTION_ID_SELECTION8:
        return SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION8].name();
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
