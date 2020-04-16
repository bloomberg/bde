// s_baltst_timingrequest.cpp        *DO NOT EDIT*         @generated -*-C++-*-

#include <bsls_ident.h>
BSLS_IDENT_RCSID(s_baltst_timingrequest_cpp,"$Id$ $CSID$")

#include <s_baltst_timingrequest.h>

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
#include <bslim_printer.h>
#include <bsls_assert.h>

#include <bsl_iomanip.h>
#include <bsl_limits.h>
#include <bsl_ostream.h>

namespace BloombergLP {
namespace s_baltst {

                            // -------------------
                            // class TimingRequest
                            // -------------------

// CONSTANTS

const char TimingRequest::CLASS_NAME[] = "TimingRequest";

const bdlat_SelectionInfo TimingRequest::SELECTION_INFO_ARRAY[] = {
    {
        SELECTION_ID_SQRT,
        "sqrt",
        sizeof("sqrt") - 1,
        "",
        bdlat_FormattingMode::e_DEFAULT
    },
    {
        SELECTION_ID_BASIC,
        "basic",
        sizeof("basic") - 1,
        "",
        bdlat_FormattingMode::e_DEFAULT
    },
    {
        SELECTION_ID_BIG,
        "big",
        sizeof("big") - 1,
        "",
        bdlat_FormattingMode::e_DEFAULT
    }
};

// CLASS METHODS

const bdlat_SelectionInfo *TimingRequest::lookupSelectionInfo(
        const char *name,
        int         nameLength)
{
    for (int i = 0; i < 3; ++i) {
        const bdlat_SelectionInfo& selectionInfo =
                    TimingRequest::SELECTION_INFO_ARRAY[i];

        if (nameLength == selectionInfo.d_nameLength
        &&  0 == bsl::memcmp(selectionInfo.d_name_p, name, nameLength))
        {
            return &selectionInfo;
        }
    }

    return 0;
}

const bdlat_SelectionInfo *TimingRequest::lookupSelectionInfo(int id)
{
    switch (id) {
      case SELECTION_ID_SQRT:
        return &SELECTION_INFO_ARRAY[SELECTION_INDEX_SQRT];
      case SELECTION_ID_BASIC:
        return &SELECTION_INFO_ARRAY[SELECTION_INDEX_BASIC];
      case SELECTION_ID_BIG:
        return &SELECTION_INFO_ARRAY[SELECTION_INDEX_BIG];
      default:
        return 0;
    }
}

// CREATORS

TimingRequest::TimingRequest(
    const TimingRequest& original,
    bslma::Allocator *basicAllocator)
: d_selectionId(original.d_selectionId)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    switch (d_selectionId) {
      case SELECTION_ID_SQRT: {
        new (d_sqrt.buffer())
            Sqrt(original.d_sqrt.object());
      } break;
      case SELECTION_ID_BASIC: {
        new (d_basic.buffer())
            BasicRecord(
                original.d_basic.object(), d_allocator_p);
      } break;
      case SELECTION_ID_BIG: {
        new (d_big.buffer())
            BigRecord(
                original.d_big.object(), d_allocator_p);
      } break;
      default:
        BSLS_ASSERT(SELECTION_ID_UNDEFINED == d_selectionId);
    }
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
TimingRequest::TimingRequest(TimingRequest&& original) noexcept
: d_selectionId(original.d_selectionId)
, d_allocator_p(original.d_allocator_p)
{
    switch (d_selectionId) {
      case SELECTION_ID_SQRT: {
        new (d_sqrt.buffer())
            Sqrt(bsl::move(original.d_sqrt.object()));
      } break;
      case SELECTION_ID_BASIC: {
        new (d_basic.buffer())
            BasicRecord(
                bsl::move(original.d_basic.object()), d_allocator_p);
      } break;
      case SELECTION_ID_BIG: {
        new (d_big.buffer())
            BigRecord(
                bsl::move(original.d_big.object()), d_allocator_p);
      } break;
      default:
        BSLS_ASSERT(SELECTION_ID_UNDEFINED == d_selectionId);
    }
}

TimingRequest::TimingRequest(
    TimingRequest&& original,
    bslma::Allocator *basicAllocator)
: d_selectionId(original.d_selectionId)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    switch (d_selectionId) {
      case SELECTION_ID_SQRT: {
        new (d_sqrt.buffer())
            Sqrt(bsl::move(original.d_sqrt.object()));
      } break;
      case SELECTION_ID_BASIC: {
        new (d_basic.buffer())
            BasicRecord(
                bsl::move(original.d_basic.object()), d_allocator_p);
      } break;
      case SELECTION_ID_BIG: {
        new (d_big.buffer())
            BigRecord(
                bsl::move(original.d_big.object()), d_allocator_p);
      } break;
      default:
        BSLS_ASSERT(SELECTION_ID_UNDEFINED == d_selectionId);
    }
}
#endif

// MANIPULATORS

TimingRequest&
TimingRequest::operator=(const TimingRequest& rhs)
{
    if (this != &rhs) {
        switch (rhs.d_selectionId) {
          case SELECTION_ID_SQRT: {
            makeSqrt(rhs.d_sqrt.object());
          } break;
          case SELECTION_ID_BASIC: {
            makeBasic(rhs.d_basic.object());
          } break;
          case SELECTION_ID_BIG: {
            makeBig(rhs.d_big.object());
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
TimingRequest&
TimingRequest::operator=(TimingRequest&& rhs)
{
    if (this != &rhs) {
        switch (rhs.d_selectionId) {
          case SELECTION_ID_SQRT: {
            makeSqrt(bsl::move(rhs.d_sqrt.object()));
          } break;
          case SELECTION_ID_BASIC: {
            makeBasic(bsl::move(rhs.d_basic.object()));
          } break;
          case SELECTION_ID_BIG: {
            makeBig(bsl::move(rhs.d_big.object()));
          } break;
          default:
            BSLS_ASSERT(SELECTION_ID_UNDEFINED == rhs.d_selectionId);
            reset();
        }
    }

    return *this;
}
#endif

void TimingRequest::reset()
{
    switch (d_selectionId) {
      case SELECTION_ID_SQRT: {
        d_sqrt.object().~Sqrt();
      } break;
      case SELECTION_ID_BASIC: {
        d_basic.object().~BasicRecord();
      } break;
      case SELECTION_ID_BIG: {
        d_big.object().~BigRecord();
      } break;
      default:
        BSLS_ASSERT(SELECTION_ID_UNDEFINED == d_selectionId);
    }

    d_selectionId = SELECTION_ID_UNDEFINED;
}

int TimingRequest::makeSelection(int selectionId)
{
    switch (selectionId) {
      case SELECTION_ID_SQRT: {
        makeSqrt();
      } break;
      case SELECTION_ID_BASIC: {
        makeBasic();
      } break;
      case SELECTION_ID_BIG: {
        makeBig();
      } break;
      case SELECTION_ID_UNDEFINED: {
        reset();
      } break;
      default:
        return -1;
    }
    return 0;
}

int TimingRequest::makeSelection(const char *name, int nameLength)
{
    const bdlat_SelectionInfo *selectionInfo =
                                         lookupSelectionInfo(name, nameLength);
    if (0 == selectionInfo) {
       return -1;
    }

    return makeSelection(selectionInfo->d_id);
}

Sqrt& TimingRequest::makeSqrt()
{
    if (SELECTION_ID_SQRT == d_selectionId) {
        bdlat_ValueTypeFunctions::reset(&d_sqrt.object());
    }
    else {
        reset();
        new (d_sqrt.buffer())
            Sqrt();
        d_selectionId = SELECTION_ID_SQRT;
    }

    return d_sqrt.object();
}

Sqrt& TimingRequest::makeSqrt(const Sqrt& value)
{
    if (SELECTION_ID_SQRT == d_selectionId) {
        d_sqrt.object() = value;
    }
    else {
        reset();
        new (d_sqrt.buffer())
                Sqrt(value);
        d_selectionId = SELECTION_ID_SQRT;
    }

    return d_sqrt.object();
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
Sqrt& TimingRequest::makeSqrt(Sqrt&& value)
{
    if (SELECTION_ID_SQRT == d_selectionId) {
        d_sqrt.object() = bsl::move(value);
    }
    else {
        reset();
        new (d_sqrt.buffer())
                Sqrt(bsl::move(value));
        d_selectionId = SELECTION_ID_SQRT;
    }

    return d_sqrt.object();
}
#endif

BasicRecord& TimingRequest::makeBasic()
{
    if (SELECTION_ID_BASIC == d_selectionId) {
        bdlat_ValueTypeFunctions::reset(&d_basic.object());
    }
    else {
        reset();
        new (d_basic.buffer())
                BasicRecord(d_allocator_p);
        d_selectionId = SELECTION_ID_BASIC;
    }

    return d_basic.object();
}

BasicRecord& TimingRequest::makeBasic(const BasicRecord& value)
{
    if (SELECTION_ID_BASIC == d_selectionId) {
        d_basic.object() = value;
    }
    else {
        reset();
        new (d_basic.buffer())
                BasicRecord(value, d_allocator_p);
        d_selectionId = SELECTION_ID_BASIC;
    }

    return d_basic.object();
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
BasicRecord& TimingRequest::makeBasic(BasicRecord&& value)
{
    if (SELECTION_ID_BASIC == d_selectionId) {
        d_basic.object() = bsl::move(value);
    }
    else {
        reset();
        new (d_basic.buffer())
                BasicRecord(bsl::move(value), d_allocator_p);
        d_selectionId = SELECTION_ID_BASIC;
    }

    return d_basic.object();
}
#endif

BigRecord& TimingRequest::makeBig()
{
    if (SELECTION_ID_BIG == d_selectionId) {
        bdlat_ValueTypeFunctions::reset(&d_big.object());
    }
    else {
        reset();
        new (d_big.buffer())
                BigRecord(d_allocator_p);
        d_selectionId = SELECTION_ID_BIG;
    }

    return d_big.object();
}

BigRecord& TimingRequest::makeBig(const BigRecord& value)
{
    if (SELECTION_ID_BIG == d_selectionId) {
        d_big.object() = value;
    }
    else {
        reset();
        new (d_big.buffer())
                BigRecord(value, d_allocator_p);
        d_selectionId = SELECTION_ID_BIG;
    }

    return d_big.object();
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
BigRecord& TimingRequest::makeBig(BigRecord&& value)
{
    if (SELECTION_ID_BIG == d_selectionId) {
        d_big.object() = bsl::move(value);
    }
    else {
        reset();
        new (d_big.buffer())
                BigRecord(bsl::move(value), d_allocator_p);
        d_selectionId = SELECTION_ID_BIG;
    }

    return d_big.object();
}
#endif

// ACCESSORS

bsl::ostream& TimingRequest::print(
    bsl::ostream& stream,
    int           level,
    int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    switch (d_selectionId) {
      case SELECTION_ID_SQRT: {
        printer.printAttribute("sqrt", d_sqrt.object());
      }  break;
      case SELECTION_ID_BASIC: {
        printer.printAttribute("basic", d_basic.object());
      }  break;
      case SELECTION_ID_BIG: {
        printer.printAttribute("big", d_big.object());
      }  break;
      default:
        stream << "SELECTION UNDEFINED\n";
    }
    printer.end();
    return stream;
}


const char *TimingRequest::selectionName() const
{
    switch (d_selectionId) {
      case SELECTION_ID_SQRT:
        return SELECTION_INFO_ARRAY[SELECTION_INDEX_SQRT].name();
      case SELECTION_ID_BASIC:
        return SELECTION_INFO_ARRAY[SELECTION_INDEX_BASIC].name();
      case SELECTION_ID_BIG:
        return SELECTION_INFO_ARRAY[SELECTION_INDEX_BIG].name();
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
