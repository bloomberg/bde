// s_baltst_anonymouschoicesequence.cpp   *DO NOT EDIT*    @generated -*-C++-*-

#include <bsls_ident.h>
BSLS_IDENT_RCSID(s_baltst_anonymouschoicesequence_cpp, "$Id$ $CSID$")

#include <s_baltst_anonymouschoicesequence.h>

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

                          // ------------------------
                          // class AnonChoiceSequence
                          // ------------------------

// CONSTANTS

const char AnonChoiceSequence::CLASS_NAME[] = "AnonChoiceSequence";

const bdlat_AttributeInfo AnonChoiceSequence::ATTRIBUTE_INFO_ARRAY[] = {
    {
        ATTRIBUTE_ID_A,
        "a",
        sizeof("a") - 1,
        "",
        bdlat_FormattingMode::e_DEC
    }
};

// CLASS METHODS

const bdlat_AttributeInfo *AnonChoiceSequence::lookupAttributeInfo(
        const char *name,
        int         nameLength)
{
    for (int i = 0; i < 1; ++i) {
        const bdlat_AttributeInfo& attributeInfo =
                    AnonChoiceSequence::ATTRIBUTE_INFO_ARRAY[i];

        if (nameLength == attributeInfo.d_nameLength
        &&  0 == bsl::memcmp(attributeInfo.d_name_p, name, nameLength))
        {
            return &attributeInfo;
        }
    }

    return 0;
}

const bdlat_AttributeInfo *AnonChoiceSequence::lookupAttributeInfo(int id)
{
    switch (id) {
      case ATTRIBUTE_ID_A:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_A];
      default:
        return 0;
    }
}

// CREATORS

AnonChoiceSequence::AnonChoiceSequence()
: d_a()
{
}

// MANIPULATORS

void AnonChoiceSequence::reset()
{
    bdlat_ValueTypeFunctions::reset(&d_a);
}

// ACCESSORS

bsl::ostream& AnonChoiceSequence::print(bsl::ostream& stream,
                                        int           level,
                                        int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute("a", this->a());
    printer.end();
    return stream;
}



                              // ----------------
                              // class AnonChoice
                              // ----------------

// CONSTANTS

const char AnonChoice::CLASS_NAME[] = "AnonChoice";

const bdlat_SelectionInfo AnonChoice::SELECTION_INFO_ARRAY[] = {
    {
        SELECTION_ID_SEQUENCE,
        "Sequence",
        sizeof("Sequence") - 1,
        "",
        bdlat_FormattingMode::e_DEFAULT
      | bdlat_FormattingMode::e_UNTAGGED
    }
};

// CLASS METHODS

const bdlat_SelectionInfo *AnonChoice::lookupSelectionInfo(
        const char *name,
        int         nameLength)
{
    for (int i = 0; i < 1; ++i) {
        const bdlat_SelectionInfo& selectionInfo =
                    AnonChoice::SELECTION_INFO_ARRAY[i];

        if (nameLength == selectionInfo.d_nameLength
        &&  0 == bsl::memcmp(selectionInfo.d_name_p, name, nameLength))
        {
            return &selectionInfo;
        }
    }

    return 0;
}

const bdlat_SelectionInfo *AnonChoice::lookupSelectionInfo(int id)
{
    switch (id) {
      case SELECTION_ID_SEQUENCE:
        return &SELECTION_INFO_ARRAY[SELECTION_INDEX_SEQUENCE];
      default:
        return 0;
    }
}

// CREATORS

AnonChoice::AnonChoice(const AnonChoice& original)
: d_selectionId(original.d_selectionId)
{
    switch (d_selectionId) {
      case SELECTION_ID_SEQUENCE: {
        new (d_sequence.buffer())
            AnonChoiceSequence(original.d_sequence.object());
      } break;
      default:
        BSLS_ASSERT(SELECTION_ID_UNDEFINED == d_selectionId);
    }
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
AnonChoice::AnonChoice(AnonChoice&& original) noexcept
: d_selectionId(original.d_selectionId)
{
    switch (d_selectionId) {
      case SELECTION_ID_SEQUENCE: {
        new (d_sequence.buffer())
            AnonChoiceSequence(bsl::move(original.d_sequence.object()));
      } break;
      default:
        BSLS_ASSERT(SELECTION_ID_UNDEFINED == d_selectionId);
    }
}
#endif

// MANIPULATORS

AnonChoice&
AnonChoice::operator=(const AnonChoice& rhs)
{
    if (this != &rhs) {
        switch (rhs.d_selectionId) {
          case SELECTION_ID_SEQUENCE: {
            makeSequence(rhs.d_sequence.object());
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
AnonChoice&
AnonChoice::operator=(AnonChoice&& rhs)
{
    if (this != &rhs) {
        switch (rhs.d_selectionId) {
          case SELECTION_ID_SEQUENCE: {
            makeSequence(bsl::move(rhs.d_sequence.object()));
          } break;
          default:
            BSLS_ASSERT(SELECTION_ID_UNDEFINED == rhs.d_selectionId);
            reset();
        }
    }

    return *this;
}
#endif

void AnonChoice::reset()
{
    switch (d_selectionId) {
      case SELECTION_ID_SEQUENCE: {
        d_sequence.object().~AnonChoiceSequence();
      } break;
      default:
        BSLS_ASSERT(SELECTION_ID_UNDEFINED == d_selectionId);
    }

    d_selectionId = SELECTION_ID_UNDEFINED;
}

int AnonChoice::makeSelection(int selectionId)
{
    switch (selectionId) {
      case SELECTION_ID_SEQUENCE: {
        makeSequence();
      } break;
      case SELECTION_ID_UNDEFINED: {
        reset();
      } break;
      default:
        return -1;
    }
    return 0;
}

int AnonChoice::makeSelection(const char *name, int nameLength)
{
    const bdlat_SelectionInfo *selectionInfo =
                                         lookupSelectionInfo(name, nameLength);
    if (0 == selectionInfo) {
       return -1;
    }

    return makeSelection(selectionInfo->d_id);
}

AnonChoiceSequence& AnonChoice::makeSequence()
{
    if (SELECTION_ID_SEQUENCE == d_selectionId) {
        bdlat_ValueTypeFunctions::reset(&d_sequence.object());
    }
    else {
        reset();
        new (d_sequence.buffer())
            AnonChoiceSequence();
        d_selectionId = SELECTION_ID_SEQUENCE;
    }

    return d_sequence.object();
}

AnonChoiceSequence& AnonChoice::makeSequence(const AnonChoiceSequence& value)
{
    if (SELECTION_ID_SEQUENCE == d_selectionId) {
        d_sequence.object() = value;
    }
    else {
        reset();
        new (d_sequence.buffer())
                AnonChoiceSequence(value);
        d_selectionId = SELECTION_ID_SEQUENCE;
    }

    return d_sequence.object();
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
AnonChoiceSequence& AnonChoice::makeSequence(AnonChoiceSequence&& value)
{
    if (SELECTION_ID_SEQUENCE == d_selectionId) {
        d_sequence.object() = bsl::move(value);
    }
    else {
        reset();
        new (d_sequence.buffer())
                AnonChoiceSequence(bsl::move(value));
        d_selectionId = SELECTION_ID_SEQUENCE;
    }

    return d_sequence.object();
}
#endif

// ACCESSORS

bsl::ostream& AnonChoice::print(
    bsl::ostream& stream,
    int           level,
    int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    switch (d_selectionId) {
      case SELECTION_ID_SEQUENCE: {
        printer.printAttribute("sequence", d_sequence.object());
      }  break;
      default:
        stream << "SELECTION UNDEFINED\n";
    }
    printer.end();
    return stream;
}


const char *AnonChoice::selectionName() const
{
    switch (d_selectionId) {
      case SELECTION_ID_SEQUENCE:
        return SELECTION_INFO_ARRAY[SELECTION_INDEX_SEQUENCE].name();
      default:
        BSLS_ASSERT(SELECTION_ID_UNDEFINED == d_selectionId);
        return "(* UNDEFINED *)";
    }
}
}  // close package namespace
}  // close enterprise namespace

// GENERATED BY BLP_BAS_CODEGEN_2025.03.13
// USING bas_codegen.pl s_baltst_anonymouschoicesequence.xsd --mode msg --includedir . --msgComponent anonymouschoicesequence --noRecurse --noExternalization --noHashSupport --noAggregateConversion
// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright 2025 Bloomberg Finance L.P. All rights reserved.
//      Property of Bloomberg Finance L.P. (BFLP)
//      This software is made available solely pursuant to the
//      terms of a BFLP license agreement which governs its use.
// ------------------------------- END-OF-FILE --------------------------------
