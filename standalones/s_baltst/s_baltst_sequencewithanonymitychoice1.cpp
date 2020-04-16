// s_baltst_sequencewithanonymitychoice1.cpp        *DO NOT EDIT*         @generated -*-C++-*-

#include <bsls_ident.h>
BSLS_IDENT_RCSID(s_baltst_sequencewithanonymitychoice1_cpp,"$Id$ $CSID$")

#include <s_baltst_sequencewithanonymitychoice1.h>

#include <bdlat_formattingmode.h>
#include <bdlat_valuetypefunctions.h>
#include <bdlb_print.h>
#include <bdlb_printmethods.h>
#include <bdlb_string.h>

#include <bsl_string.h>
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
#include <bslim_printer.h>
#include <bsls_assert.h>

#include <bsl_iomanip.h>
#include <bsl_limits.h>
#include <bsl_ostream.h>

namespace BloombergLP {
namespace s_baltst {

                     // ----------------------------------
                     // class SequenceWithAnonymityChoice1
                     // ----------------------------------

// CONSTANTS

const char SequenceWithAnonymityChoice1::CLASS_NAME[] = "SequenceWithAnonymityChoice1";

const bdlat_SelectionInfo SequenceWithAnonymityChoice1::SELECTION_INFO_ARRAY[] = {
    {
        SELECTION_ID_SELECTION5,
        "selection5",
        sizeof("selection5") - 1,
        "",
        bdlat_FormattingMode::e_TEXT
    },
    {
        SELECTION_ID_SELECTION6,
        "selection6",
        sizeof("selection6") - 1,
        "",
        bdlat_FormattingMode::e_TEXT
    }
};

// CLASS METHODS

const bdlat_SelectionInfo *SequenceWithAnonymityChoice1::lookupSelectionInfo(
        const char *name,
        int         nameLength)
{
    for (int i = 0; i < 2; ++i) {
        const bdlat_SelectionInfo& selectionInfo =
                    SequenceWithAnonymityChoice1::SELECTION_INFO_ARRAY[i];

        if (nameLength == selectionInfo.d_nameLength
        &&  0 == bsl::memcmp(selectionInfo.d_name_p, name, nameLength))
        {
            return &selectionInfo;
        }
    }

    return 0;
}

const bdlat_SelectionInfo *SequenceWithAnonymityChoice1::lookupSelectionInfo(int id)
{
    switch (id) {
      case SELECTION_ID_SELECTION5:
        return &SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION5];
      case SELECTION_ID_SELECTION6:
        return &SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION6];
      default:
        return 0;
    }
}

// CREATORS

SequenceWithAnonymityChoice1::SequenceWithAnonymityChoice1(
    const SequenceWithAnonymityChoice1& original,
    bslma::Allocator *basicAllocator)
: d_selectionId(original.d_selectionId)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    switch (d_selectionId) {
      case SELECTION_ID_SELECTION5: {
        new (d_selection5.buffer())
            bool(original.d_selection5.object());
      } break;
      case SELECTION_ID_SELECTION6: {
        new (d_selection6.buffer())
            bsl::string(
                original.d_selection6.object(), d_allocator_p);
      } break;
      default:
        BSLS_ASSERT(SELECTION_ID_UNDEFINED == d_selectionId);
    }
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
SequenceWithAnonymityChoice1::SequenceWithAnonymityChoice1(SequenceWithAnonymityChoice1&& original) noexcept
: d_selectionId(original.d_selectionId)
, d_allocator_p(original.d_allocator_p)
{
    switch (d_selectionId) {
      case SELECTION_ID_SELECTION5: {
        new (d_selection5.buffer())
            bool(bsl::move(original.d_selection5.object()));
      } break;
      case SELECTION_ID_SELECTION6: {
        new (d_selection6.buffer())
            bsl::string(
                bsl::move(original.d_selection6.object()), d_allocator_p);
      } break;
      default:
        BSLS_ASSERT(SELECTION_ID_UNDEFINED == d_selectionId);
    }
}

SequenceWithAnonymityChoice1::SequenceWithAnonymityChoice1(
    SequenceWithAnonymityChoice1&& original,
    bslma::Allocator *basicAllocator)
: d_selectionId(original.d_selectionId)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    switch (d_selectionId) {
      case SELECTION_ID_SELECTION5: {
        new (d_selection5.buffer())
            bool(bsl::move(original.d_selection5.object()));
      } break;
      case SELECTION_ID_SELECTION6: {
        new (d_selection6.buffer())
            bsl::string(
                bsl::move(original.d_selection6.object()), d_allocator_p);
      } break;
      default:
        BSLS_ASSERT(SELECTION_ID_UNDEFINED == d_selectionId);
    }
}
#endif

// MANIPULATORS

SequenceWithAnonymityChoice1&
SequenceWithAnonymityChoice1::operator=(const SequenceWithAnonymityChoice1& rhs)
{
    if (this != &rhs) {
        switch (rhs.d_selectionId) {
          case SELECTION_ID_SELECTION5: {
            makeSelection5(rhs.d_selection5.object());
          } break;
          case SELECTION_ID_SELECTION6: {
            makeSelection6(rhs.d_selection6.object());
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
SequenceWithAnonymityChoice1&
SequenceWithAnonymityChoice1::operator=(SequenceWithAnonymityChoice1&& rhs)
{
    if (this != &rhs) {
        switch (rhs.d_selectionId) {
          case SELECTION_ID_SELECTION5: {
            makeSelection5(bsl::move(rhs.d_selection5.object()));
          } break;
          case SELECTION_ID_SELECTION6: {
            makeSelection6(bsl::move(rhs.d_selection6.object()));
          } break;
          default:
            BSLS_ASSERT(SELECTION_ID_UNDEFINED == rhs.d_selectionId);
            reset();
        }
    }

    return *this;
}
#endif

void SequenceWithAnonymityChoice1::reset()
{
    switch (d_selectionId) {
      case SELECTION_ID_SELECTION5: {
        // no destruction required
      } break;
      case SELECTION_ID_SELECTION6: {
        typedef bsl::string Type;
        d_selection6.object().~Type();
      } break;
      default:
        BSLS_ASSERT(SELECTION_ID_UNDEFINED == d_selectionId);
    }

    d_selectionId = SELECTION_ID_UNDEFINED;
}

int SequenceWithAnonymityChoice1::makeSelection(int selectionId)
{
    switch (selectionId) {
      case SELECTION_ID_SELECTION5: {
        makeSelection5();
      } break;
      case SELECTION_ID_SELECTION6: {
        makeSelection6();
      } break;
      case SELECTION_ID_UNDEFINED: {
        reset();
      } break;
      default:
        return -1;
    }
    return 0;
}

int SequenceWithAnonymityChoice1::makeSelection(const char *name, int nameLength)
{
    const bdlat_SelectionInfo *selectionInfo =
                                         lookupSelectionInfo(name, nameLength);
    if (0 == selectionInfo) {
       return -1;
    }

    return makeSelection(selectionInfo->d_id);
}

bool& SequenceWithAnonymityChoice1::makeSelection5()
{
    if (SELECTION_ID_SELECTION5 == d_selectionId) {
        bdlat_ValueTypeFunctions::reset(&d_selection5.object());
    }
    else {
        reset();
        new (d_selection5.buffer())
            bool();
        d_selectionId = SELECTION_ID_SELECTION5;
    }

    return d_selection5.object();
}

bool& SequenceWithAnonymityChoice1::makeSelection5(bool value)
{
    if (SELECTION_ID_SELECTION5 == d_selectionId) {
        d_selection5.object() = value;
    }
    else {
        reset();
        new (d_selection5.buffer())
                bool(value);
        d_selectionId = SELECTION_ID_SELECTION5;
    }

    return d_selection5.object();
}

bsl::string& SequenceWithAnonymityChoice1::makeSelection6()
{
    if (SELECTION_ID_SELECTION6 == d_selectionId) {
        bdlat_ValueTypeFunctions::reset(&d_selection6.object());
    }
    else {
        reset();
        new (d_selection6.buffer())
                bsl::string(d_allocator_p);
        d_selectionId = SELECTION_ID_SELECTION6;
    }

    return d_selection6.object();
}

bsl::string& SequenceWithAnonymityChoice1::makeSelection6(const bsl::string& value)
{
    if (SELECTION_ID_SELECTION6 == d_selectionId) {
        d_selection6.object() = value;
    }
    else {
        reset();
        new (d_selection6.buffer())
                bsl::string(value, d_allocator_p);
        d_selectionId = SELECTION_ID_SELECTION6;
    }

    return d_selection6.object();
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
bsl::string& SequenceWithAnonymityChoice1::makeSelection6(bsl::string&& value)
{
    if (SELECTION_ID_SELECTION6 == d_selectionId) {
        d_selection6.object() = bsl::move(value);
    }
    else {
        reset();
        new (d_selection6.buffer())
                bsl::string(bsl::move(value), d_allocator_p);
        d_selectionId = SELECTION_ID_SELECTION6;
    }

    return d_selection6.object();
}
#endif

// ACCESSORS

bsl::ostream& SequenceWithAnonymityChoice1::print(
    bsl::ostream& stream,
    int           level,
    int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    switch (d_selectionId) {
      case SELECTION_ID_SELECTION5: {
        printer.printAttribute("selection5", d_selection5.object());
      }  break;
      case SELECTION_ID_SELECTION6: {
        printer.printAttribute("selection6", d_selection6.object());
      }  break;
      default:
        stream << "SELECTION UNDEFINED\n";
    }
    printer.end();
    return stream;
}


const char *SequenceWithAnonymityChoice1::selectionName() const
{
    switch (d_selectionId) {
      case SELECTION_ID_SELECTION5:
        return SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION5].name();
      case SELECTION_ID_SELECTION6:
        return SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION6].name();
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
