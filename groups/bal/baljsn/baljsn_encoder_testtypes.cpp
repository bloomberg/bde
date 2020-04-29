// baljsn_encoder_testtypes.cpp       *DO NOT EDIT*        @generated -*-C++-*-

#include <bsls_ident.h>
BSLS_IDENT_RCSID(baljsn_encoder_testtypes_cpp,"$Id$ $CSID$")

#include <baljsn_encoder_testtypes.h>

#include <bdlat_formattingmode.h>
#include <bdlat_valuetypefunctions.h>
#include <bdlde_utf8util.h>
#include <bdlb_print.h>
#include <bdlb_printmethods.h>
#include <bdlb_string.h>

#include <bdlb_nullablevalue.h>
#include <bsl_string.h>
#include <bsl_vector.h>
#include <bslim_printer.h>
#include <bsls_assert.h>

#include <bsl_iomanip.h>
#include <bsl_limits.h>
#include <bsl_ostream.h>

namespace BloombergLP {
namespace baljsn {

                          // ------------------------
                          // class EncoderTestAddress
                          // ------------------------

// CONSTANTS

const char EncoderTestAddress::CLASS_NAME[] = "EncoderTestAddress";

const bdlat_AttributeInfo EncoderTestAddress::ATTRIBUTE_INFO_ARRAY[] = {
    {
        ATTRIBUTE_ID_STREET,
        "street",
        sizeof("street") - 1,
        "",
        bdlat_FormattingMode::e_TEXT
    },
    {
        ATTRIBUTE_ID_CITY,
        "city",
        sizeof("city") - 1,
        "",
        bdlat_FormattingMode::e_TEXT
    },
    {
        ATTRIBUTE_ID_STATE,
        "state",
        sizeof("state") - 1,
        "",
        bdlat_FormattingMode::e_TEXT
    }
};

// CLASS METHODS

const bdlat_AttributeInfo *EncoderTestAddress::lookupAttributeInfo(
        const char *name,
        int         nameLength)
{
    for (int i = 0; i < 3; ++i) {
        const bdlat_AttributeInfo& attributeInfo =
                    EncoderTestAddress::ATTRIBUTE_INFO_ARRAY[i];

        if (nameLength == attributeInfo.d_nameLength
        &&  0 == bsl::memcmp(attributeInfo.d_name_p, name, nameLength))
        {
            return &attributeInfo;
        }
    }

    return 0;
}

const bdlat_AttributeInfo *EncoderTestAddress::lookupAttributeInfo(int id)
{
    switch (id) {
      case ATTRIBUTE_ID_STREET:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_STREET];
      case ATTRIBUTE_ID_CITY:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CITY];
      case ATTRIBUTE_ID_STATE:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_STATE];
      default:
        return 0;
    }
}

// CREATORS

EncoderTestAddress::EncoderTestAddress(bslma::Allocator *basicAllocator)
: d_street(basicAllocator)
, d_city(basicAllocator)
, d_state(basicAllocator)
{
}

EncoderTestAddress::EncoderTestAddress(const EncoderTestAddress& original,
                                       bslma::Allocator *basicAllocator)
: d_street(original.d_street, basicAllocator)
, d_city(original.d_city, basicAllocator)
, d_state(original.d_state, basicAllocator)
{
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
EncoderTestAddress::EncoderTestAddress(EncoderTestAddress&& original) noexcept
: d_street(bsl::move(original.d_street))
, d_city(bsl::move(original.d_city))
, d_state(bsl::move(original.d_state))
{
}

EncoderTestAddress::EncoderTestAddress(EncoderTestAddress&& original,
                                       bslma::Allocator *basicAllocator)
: d_street(bsl::move(original.d_street), basicAllocator)
, d_city(bsl::move(original.d_city), basicAllocator)
, d_state(bsl::move(original.d_state), basicAllocator)
{
}
#endif

EncoderTestAddress::~EncoderTestAddress()
{
}

// MANIPULATORS

EncoderTestAddress&
EncoderTestAddress::operator=(const EncoderTestAddress& rhs)
{
    if (this != &rhs) {
        d_street = rhs.d_street;
        d_city = rhs.d_city;
        d_state = rhs.d_state;
    }

    return *this;
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
EncoderTestAddress&
EncoderTestAddress::operator=(EncoderTestAddress&& rhs)
{
    if (this != &rhs) {
        d_street = bsl::move(rhs.d_street);
        d_city = bsl::move(rhs.d_city);
        d_state = bsl::move(rhs.d_state);
    }

    return *this;
}
#endif

void EncoderTestAddress::reset()
{
    bdlat_ValueTypeFunctions::reset(&d_street);
    bdlat_ValueTypeFunctions::reset(&d_city);
    bdlat_ValueTypeFunctions::reset(&d_state);
}

// ACCESSORS

bsl::ostream& EncoderTestAddress::print(
        bsl::ostream& stream,
        int           level,
        int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute("street", d_street);
    printer.printAttribute("city", d_city);
    printer.printAttribute("state", d_state);
    printer.end();
    return stream;
}



               // ----------------------------------------------
               // class EncoderTestChoiceWithAllCategoriesChoice
               // ----------------------------------------------

// CONSTANTS

const char EncoderTestChoiceWithAllCategoriesChoice::CLASS_NAME[] = "EncoderTestChoiceWithAllCategoriesChoice";

const bdlat_SelectionInfo EncoderTestChoiceWithAllCategoriesChoice::SELECTION_INFO_ARRAY[] = {
    {
        SELECTION_ID_SELECTION0,
        "selection0",
        sizeof("selection0") - 1,
        "",
        bdlat_FormattingMode::e_DEC
    }
};

// CLASS METHODS

const bdlat_SelectionInfo *EncoderTestChoiceWithAllCategoriesChoice::lookupSelectionInfo(
        const char *name,
        int         nameLength)
{
    for (int i = 0; i < 1; ++i) {
        const bdlat_SelectionInfo& selectionInfo =
                    EncoderTestChoiceWithAllCategoriesChoice::SELECTION_INFO_ARRAY[i];

        if (nameLength == selectionInfo.d_nameLength
        &&  0 == bsl::memcmp(selectionInfo.d_name_p, name, nameLength))
        {
            return &selectionInfo;
        }
    }

    return 0;
}

const bdlat_SelectionInfo *EncoderTestChoiceWithAllCategoriesChoice::lookupSelectionInfo(int id)
{
    switch (id) {
      case SELECTION_ID_SELECTION0:
        return &SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION0];
      default:
        return 0;
    }
}

// CREATORS

EncoderTestChoiceWithAllCategoriesChoice::EncoderTestChoiceWithAllCategoriesChoice(const EncoderTestChoiceWithAllCategoriesChoice& original)
: d_selectionId(original.d_selectionId)
{
    switch (d_selectionId) {
      case SELECTION_ID_SELECTION0: {
        new (d_selection0.buffer())
            int(original.d_selection0.object());
      } break;
      default:
        BSLS_ASSERT(SELECTION_ID_UNDEFINED == d_selectionId);
    }
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
EncoderTestChoiceWithAllCategoriesChoice::EncoderTestChoiceWithAllCategoriesChoice(EncoderTestChoiceWithAllCategoriesChoice&& original) noexcept
: d_selectionId(original.d_selectionId)
{
    switch (d_selectionId) {
      case SELECTION_ID_SELECTION0: {
        new (d_selection0.buffer())
            int(bsl::move(original.d_selection0.object()));
      } break;
      default:
        BSLS_ASSERT(SELECTION_ID_UNDEFINED == d_selectionId);
    }
}
#endif

// MANIPULATORS

EncoderTestChoiceWithAllCategoriesChoice&
EncoderTestChoiceWithAllCategoriesChoice::operator=(const EncoderTestChoiceWithAllCategoriesChoice& rhs)
{
    if (this != &rhs) {
        switch (rhs.d_selectionId) {
          case SELECTION_ID_SELECTION0: {
            makeSelection0(rhs.d_selection0.object());
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
EncoderTestChoiceWithAllCategoriesChoice&
EncoderTestChoiceWithAllCategoriesChoice::operator=(EncoderTestChoiceWithAllCategoriesChoice&& rhs)
{
    if (this != &rhs) {
        switch (rhs.d_selectionId) {
          case SELECTION_ID_SELECTION0: {
            makeSelection0(bsl::move(rhs.d_selection0.object()));
          } break;
          default:
            BSLS_ASSERT(SELECTION_ID_UNDEFINED == rhs.d_selectionId);
            reset();
        }
    }

    return *this;
}
#endif

void EncoderTestChoiceWithAllCategoriesChoice::reset()
{
    switch (d_selectionId) {
      case SELECTION_ID_SELECTION0: {
        // no destruction required
      } break;
      default:
        BSLS_ASSERT(SELECTION_ID_UNDEFINED == d_selectionId);
    }

    d_selectionId = SELECTION_ID_UNDEFINED;
}

int EncoderTestChoiceWithAllCategoriesChoice::makeSelection(int selectionId)
{
    switch (selectionId) {
      case SELECTION_ID_SELECTION0: {
        makeSelection0();
      } break;
      case SELECTION_ID_UNDEFINED: {
        reset();
      } break;
      default:
        return -1;
    }
    return 0;
}

int EncoderTestChoiceWithAllCategoriesChoice::makeSelection(const char *name, int nameLength)
{
    const bdlat_SelectionInfo *selectionInfo =
                                         lookupSelectionInfo(name, nameLength);
    if (0 == selectionInfo) {
       return -1;
    }

    return makeSelection(selectionInfo->d_id);
}

int& EncoderTestChoiceWithAllCategoriesChoice::makeSelection0()
{
    if (SELECTION_ID_SELECTION0 == d_selectionId) {
        bdlat_ValueTypeFunctions::reset(&d_selection0.object());
    }
    else {
        reset();
        new (d_selection0.buffer())
            int();
        d_selectionId = SELECTION_ID_SELECTION0;
    }

    return d_selection0.object();
}

int& EncoderTestChoiceWithAllCategoriesChoice::makeSelection0(int value)
{
    if (SELECTION_ID_SELECTION0 == d_selectionId) {
        d_selection0.object() = value;
    }
    else {
        reset();
        new (d_selection0.buffer())
                int(value);
        d_selectionId = SELECTION_ID_SELECTION0;
    }

    return d_selection0.object();
}

// ACCESSORS

bsl::ostream& EncoderTestChoiceWithAllCategoriesChoice::print(
    bsl::ostream& stream,
    int           level,
    int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    switch (d_selectionId) {
      case SELECTION_ID_SELECTION0: {
        printer.printAttribute("selection0", d_selection0.object());
      }  break;
      default:
        stream << "SELECTION UNDEFINED\n";
    }
    printer.end();
    return stream;
}


const char *EncoderTestChoiceWithAllCategoriesChoice::selectionName() const
{
    switch (d_selectionId) {
      case SELECTION_ID_SELECTION0:
        return SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION0].name();
      default:
        BSLS_ASSERT(SELECTION_ID_UNDEFINED == d_selectionId);
        return "(* UNDEFINED *)";
    }
}

           // ------------------------------------------------------
           // class EncoderTestChoiceWithAllCategoriesCustomizedType
           // ------------------------------------------------------

// PUBLIC CLASS METHODS

int EncoderTestChoiceWithAllCategoriesCustomizedType::checkRestrictions(const bsl::string& value)
{
    if (10 < bdlde::Utf8Util::numCharacters(value.c_str(), value.length())) {
        return -1;
    }

    return 0;
}

// CONSTANTS

const char EncoderTestChoiceWithAllCategoriesCustomizedType::CLASS_NAME[] = "EncoderTestChoiceWithAllCategoriesCustomizedType";



            // ---------------------------------------------------
            // class EncoderTestChoiceWithAllCategoriesEnumeration
            // ---------------------------------------------------

// CONSTANTS

const char EncoderTestChoiceWithAllCategoriesEnumeration::CLASS_NAME[] = "EncoderTestChoiceWithAllCategoriesEnumeration";

const bdlat_EnumeratorInfo EncoderTestChoiceWithAllCategoriesEnumeration::ENUMERATOR_INFO_ARRAY[] = {
    {
        EncoderTestChoiceWithAllCategoriesEnumeration::A,
        "A",
        sizeof("A") - 1,
        ""
    },
    {
        EncoderTestChoiceWithAllCategoriesEnumeration::B,
        "B",
        sizeof("B") - 1,
        ""
    }
};

// CLASS METHODS

int EncoderTestChoiceWithAllCategoriesEnumeration::fromInt(EncoderTestChoiceWithAllCategoriesEnumeration::Value *result, int number)
{
    switch (number) {
      case EncoderTestChoiceWithAllCategoriesEnumeration::A:
      case EncoderTestChoiceWithAllCategoriesEnumeration::B:
        *result = static_cast<EncoderTestChoiceWithAllCategoriesEnumeration::Value>(number);
        return 0;
      default:
        return -1;
    }
}

int EncoderTestChoiceWithAllCategoriesEnumeration::fromString(
        EncoderTestChoiceWithAllCategoriesEnumeration::Value *result,
        const char         *string,
        int                 stringLength)
{
    for (int i = 0; i < 2; ++i) {
        const bdlat_EnumeratorInfo& enumeratorInfo =
                    EncoderTestChoiceWithAllCategoriesEnumeration::ENUMERATOR_INFO_ARRAY[i];

        if (stringLength == enumeratorInfo.d_nameLength
        &&  0 == bsl::memcmp(enumeratorInfo.d_name_p, string, stringLength))
        {
            *result = static_cast<EncoderTestChoiceWithAllCategoriesEnumeration::Value>(enumeratorInfo.d_value);
            return 0;
        }
    }

    return -1;
}

const char *EncoderTestChoiceWithAllCategoriesEnumeration::toString(EncoderTestChoiceWithAllCategoriesEnumeration::Value value)
{
    switch (value) {
      case A: {
        return "A";
      }
      case B: {
        return "B";
      }
    }

    BSLS_ASSERT(!"invalid enumerator");
    return 0;
}


              // ------------------------------------------------
              // class EncoderTestChoiceWithAllCategoriesSequence
              // ------------------------------------------------

// CONSTANTS

const char EncoderTestChoiceWithAllCategoriesSequence::CLASS_NAME[] = "EncoderTestChoiceWithAllCategoriesSequence";

const bdlat_AttributeInfo EncoderTestChoiceWithAllCategoriesSequence::ATTRIBUTE_INFO_ARRAY[] = {
    {
        ATTRIBUTE_ID_ATTRIBUTE,
        "attribute",
        sizeof("attribute") - 1,
        "",
        bdlat_FormattingMode::e_DEC
    }
};

// CLASS METHODS

const bdlat_AttributeInfo *EncoderTestChoiceWithAllCategoriesSequence::lookupAttributeInfo(
        const char *name,
        int         nameLength)
{
    for (int i = 0; i < 1; ++i) {
        const bdlat_AttributeInfo& attributeInfo =
                    EncoderTestChoiceWithAllCategoriesSequence::ATTRIBUTE_INFO_ARRAY[i];

        if (nameLength == attributeInfo.d_nameLength
        &&  0 == bsl::memcmp(attributeInfo.d_name_p, name, nameLength))
        {
            return &attributeInfo;
        }
    }

    return 0;
}

const bdlat_AttributeInfo *EncoderTestChoiceWithAllCategoriesSequence::lookupAttributeInfo(int id)
{
    switch (id) {
      case ATTRIBUTE_ID_ATTRIBUTE:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE];
      default:
        return 0;
    }
}

// CREATORS

EncoderTestChoiceWithAllCategoriesSequence::EncoderTestChoiceWithAllCategoriesSequence()
: d_attribute()
{
}

EncoderTestChoiceWithAllCategoriesSequence::EncoderTestChoiceWithAllCategoriesSequence(const EncoderTestChoiceWithAllCategoriesSequence& original)
: d_attribute(original.d_attribute)
{
}

EncoderTestChoiceWithAllCategoriesSequence::~EncoderTestChoiceWithAllCategoriesSequence()
{
}

// MANIPULATORS

EncoderTestChoiceWithAllCategoriesSequence&
EncoderTestChoiceWithAllCategoriesSequence::operator=(const EncoderTestChoiceWithAllCategoriesSequence& rhs)
{
    if (this != &rhs) {
        d_attribute = rhs.d_attribute;
    }

    return *this;
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
EncoderTestChoiceWithAllCategoriesSequence&
EncoderTestChoiceWithAllCategoriesSequence::operator=(EncoderTestChoiceWithAllCategoriesSequence&& rhs)
{
    if (this != &rhs) {
        d_attribute = bsl::move(rhs.d_attribute);
    }

    return *this;
}
#endif

void EncoderTestChoiceWithAllCategoriesSequence::reset()
{
    bdlat_ValueTypeFunctions::reset(&d_attribute);
}

// ACCESSORS

bsl::ostream& EncoderTestChoiceWithAllCategoriesSequence::print(
        bsl::ostream& stream,
        int           level,
        int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute("attribute", d_attribute);
    printer.end();
    return stream;
}



                 // ------------------------------------------
                 // class EncoderTestDegenerateChoice1Sequence
                 // ------------------------------------------

// CONSTANTS

const char EncoderTestDegenerateChoice1Sequence::CLASS_NAME[] = "EncoderTestDegenerateChoice1Sequence";


// CLASS METHODS

const bdlat_AttributeInfo *EncoderTestDegenerateChoice1Sequence::lookupAttributeInfo(
        const char *name,
        int         nameLength)
{
    (void)name;
    (void)nameLength;
    return 0;
}

const bdlat_AttributeInfo *EncoderTestDegenerateChoice1Sequence::lookupAttributeInfo(int id)
{
    switch (id) {
      default:
        return 0;
    }
}

// CREATORS

EncoderTestDegenerateChoice1Sequence::EncoderTestDegenerateChoice1Sequence()
{
}

EncoderTestDegenerateChoice1Sequence::EncoderTestDegenerateChoice1Sequence(const EncoderTestDegenerateChoice1Sequence& original)
{
    (void)original;
}

EncoderTestDegenerateChoice1Sequence::~EncoderTestDegenerateChoice1Sequence()
{
}

// MANIPULATORS

EncoderTestDegenerateChoice1Sequence&
EncoderTestDegenerateChoice1Sequence::operator=(const EncoderTestDegenerateChoice1Sequence& rhs)
{
    (void)rhs;
    return *this;
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
EncoderTestDegenerateChoice1Sequence&
EncoderTestDegenerateChoice1Sequence::operator=(EncoderTestDegenerateChoice1Sequence&& rhs)
{
    (void)rhs;
    return *this;
}
#endif

void EncoderTestDegenerateChoice1Sequence::reset()
{
}

// ACCESSORS

bsl::ostream& EncoderTestDegenerateChoice1Sequence::print(
        bsl::ostream& stream,
        int           level,
        int           spacesPerLevel) const
{
    (void)level;
    (void)spacesPerLevel;
    return stream;
}



              // ------------------------------------------------
              // class EncoderTestSequenceWithAllCategoriesChoice
              // ------------------------------------------------

// CONSTANTS

const char EncoderTestSequenceWithAllCategoriesChoice::CLASS_NAME[] = "EncoderTestSequenceWithAllCategoriesChoice";

const bdlat_SelectionInfo EncoderTestSequenceWithAllCategoriesChoice::SELECTION_INFO_ARRAY[] = {
    {
        SELECTION_ID_SELECTION0,
        "selection0",
        sizeof("selection0") - 1,
        "",
        bdlat_FormattingMode::e_DEC
    }
};

// CLASS METHODS

const bdlat_SelectionInfo *EncoderTestSequenceWithAllCategoriesChoice::lookupSelectionInfo(
        const char *name,
        int         nameLength)
{
    for (int i = 0; i < 1; ++i) {
        const bdlat_SelectionInfo& selectionInfo =
                    EncoderTestSequenceWithAllCategoriesChoice::SELECTION_INFO_ARRAY[i];

        if (nameLength == selectionInfo.d_nameLength
        &&  0 == bsl::memcmp(selectionInfo.d_name_p, name, nameLength))
        {
            return &selectionInfo;
        }
    }

    return 0;
}

const bdlat_SelectionInfo *EncoderTestSequenceWithAllCategoriesChoice::lookupSelectionInfo(int id)
{
    switch (id) {
      case SELECTION_ID_SELECTION0:
        return &SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION0];
      default:
        return 0;
    }
}

// CREATORS

EncoderTestSequenceWithAllCategoriesChoice::EncoderTestSequenceWithAllCategoriesChoice(const EncoderTestSequenceWithAllCategoriesChoice& original)
: d_selectionId(original.d_selectionId)
{
    switch (d_selectionId) {
      case SELECTION_ID_SELECTION0: {
        new (d_selection0.buffer())
            int(original.d_selection0.object());
      } break;
      default:
        BSLS_ASSERT(SELECTION_ID_UNDEFINED == d_selectionId);
    }
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
EncoderTestSequenceWithAllCategoriesChoice::EncoderTestSequenceWithAllCategoriesChoice(EncoderTestSequenceWithAllCategoriesChoice&& original) noexcept
: d_selectionId(original.d_selectionId)
{
    switch (d_selectionId) {
      case SELECTION_ID_SELECTION0: {
        new (d_selection0.buffer())
            int(bsl::move(original.d_selection0.object()));
      } break;
      default:
        BSLS_ASSERT(SELECTION_ID_UNDEFINED == d_selectionId);
    }
}
#endif

// MANIPULATORS

EncoderTestSequenceWithAllCategoriesChoice&
EncoderTestSequenceWithAllCategoriesChoice::operator=(const EncoderTestSequenceWithAllCategoriesChoice& rhs)
{
    if (this != &rhs) {
        switch (rhs.d_selectionId) {
          case SELECTION_ID_SELECTION0: {
            makeSelection0(rhs.d_selection0.object());
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
EncoderTestSequenceWithAllCategoriesChoice&
EncoderTestSequenceWithAllCategoriesChoice::operator=(EncoderTestSequenceWithAllCategoriesChoice&& rhs)
{
    if (this != &rhs) {
        switch (rhs.d_selectionId) {
          case SELECTION_ID_SELECTION0: {
            makeSelection0(bsl::move(rhs.d_selection0.object()));
          } break;
          default:
            BSLS_ASSERT(SELECTION_ID_UNDEFINED == rhs.d_selectionId);
            reset();
        }
    }

    return *this;
}
#endif

void EncoderTestSequenceWithAllCategoriesChoice::reset()
{
    switch (d_selectionId) {
      case SELECTION_ID_SELECTION0: {
        // no destruction required
      } break;
      default:
        BSLS_ASSERT(SELECTION_ID_UNDEFINED == d_selectionId);
    }

    d_selectionId = SELECTION_ID_UNDEFINED;
}

int EncoderTestSequenceWithAllCategoriesChoice::makeSelection(int selectionId)
{
    switch (selectionId) {
      case SELECTION_ID_SELECTION0: {
        makeSelection0();
      } break;
      case SELECTION_ID_UNDEFINED: {
        reset();
      } break;
      default:
        return -1;
    }
    return 0;
}

int EncoderTestSequenceWithAllCategoriesChoice::makeSelection(const char *name, int nameLength)
{
    const bdlat_SelectionInfo *selectionInfo =
                                         lookupSelectionInfo(name, nameLength);
    if (0 == selectionInfo) {
       return -1;
    }

    return makeSelection(selectionInfo->d_id);
}

int& EncoderTestSequenceWithAllCategoriesChoice::makeSelection0()
{
    if (SELECTION_ID_SELECTION0 == d_selectionId) {
        bdlat_ValueTypeFunctions::reset(&d_selection0.object());
    }
    else {
        reset();
        new (d_selection0.buffer())
            int();
        d_selectionId = SELECTION_ID_SELECTION0;
    }

    return d_selection0.object();
}

int& EncoderTestSequenceWithAllCategoriesChoice::makeSelection0(int value)
{
    if (SELECTION_ID_SELECTION0 == d_selectionId) {
        d_selection0.object() = value;
    }
    else {
        reset();
        new (d_selection0.buffer())
                int(value);
        d_selectionId = SELECTION_ID_SELECTION0;
    }

    return d_selection0.object();
}

// ACCESSORS

bsl::ostream& EncoderTestSequenceWithAllCategoriesChoice::print(
    bsl::ostream& stream,
    int           level,
    int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    switch (d_selectionId) {
      case SELECTION_ID_SELECTION0: {
        printer.printAttribute("selection0", d_selection0.object());
      }  break;
      default:
        stream << "SELECTION UNDEFINED\n";
    }
    printer.end();
    return stream;
}


const char *EncoderTestSequenceWithAllCategoriesChoice::selectionName() const
{
    switch (d_selectionId) {
      case SELECTION_ID_SELECTION0:
        return SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION0].name();
      default:
        BSLS_ASSERT(SELECTION_ID_UNDEFINED == d_selectionId);
        return "(* UNDEFINED *)";
    }
}

          // --------------------------------------------------------
          // class EncoderTestSequenceWithAllCategoriesCustomizedType
          // --------------------------------------------------------

// PUBLIC CLASS METHODS

int EncoderTestSequenceWithAllCategoriesCustomizedType::checkRestrictions(const bsl::string& value)
{
    if (10 < bdlde::Utf8Util::numCharacters(value.c_str(), value.length())) {
        return -1;
    }

    return 0;
}

// CONSTANTS

const char EncoderTestSequenceWithAllCategoriesCustomizedType::CLASS_NAME[] = "EncoderTestSequenceWithAllCategoriesCustomizedType";



           // -----------------------------------------------------
           // class EncoderTestSequenceWithAllCategoriesEnumeration
           // -----------------------------------------------------

// CONSTANTS

const char EncoderTestSequenceWithAllCategoriesEnumeration::CLASS_NAME[] = "EncoderTestSequenceWithAllCategoriesEnumeration";

const bdlat_EnumeratorInfo EncoderTestSequenceWithAllCategoriesEnumeration::ENUMERATOR_INFO_ARRAY[] = {
    {
        EncoderTestSequenceWithAllCategoriesEnumeration::A,
        "A",
        sizeof("A") - 1,
        ""
    },
    {
        EncoderTestSequenceWithAllCategoriesEnumeration::B,
        "B",
        sizeof("B") - 1,
        ""
    }
};

// CLASS METHODS

int EncoderTestSequenceWithAllCategoriesEnumeration::fromInt(EncoderTestSequenceWithAllCategoriesEnumeration::Value *result, int number)
{
    switch (number) {
      case EncoderTestSequenceWithAllCategoriesEnumeration::A:
      case EncoderTestSequenceWithAllCategoriesEnumeration::B:
        *result = static_cast<EncoderTestSequenceWithAllCategoriesEnumeration::Value>(number);
        return 0;
      default:
        return -1;
    }
}

int EncoderTestSequenceWithAllCategoriesEnumeration::fromString(
        EncoderTestSequenceWithAllCategoriesEnumeration::Value *result,
        const char         *string,
        int                 stringLength)
{
    for (int i = 0; i < 2; ++i) {
        const bdlat_EnumeratorInfo& enumeratorInfo =
                    EncoderTestSequenceWithAllCategoriesEnumeration::ENUMERATOR_INFO_ARRAY[i];

        if (stringLength == enumeratorInfo.d_nameLength
        &&  0 == bsl::memcmp(enumeratorInfo.d_name_p, string, stringLength))
        {
            *result = static_cast<EncoderTestSequenceWithAllCategoriesEnumeration::Value>(enumeratorInfo.d_value);
            return 0;
        }
    }

    return -1;
}

const char *EncoderTestSequenceWithAllCategoriesEnumeration::toString(EncoderTestSequenceWithAllCategoriesEnumeration::Value value)
{
    switch (value) {
      case A: {
        return "A";
      }
      case B: {
        return "B";
      }
    }

    BSLS_ASSERT(!"invalid enumerator");
    return 0;
}


             // --------------------------------------------------
             // class EncoderTestSequenceWithAllCategoriesSequence
             // --------------------------------------------------

// CONSTANTS

const char EncoderTestSequenceWithAllCategoriesSequence::CLASS_NAME[] = "EncoderTestSequenceWithAllCategoriesSequence";

const bdlat_AttributeInfo EncoderTestSequenceWithAllCategoriesSequence::ATTRIBUTE_INFO_ARRAY[] = {
    {
        ATTRIBUTE_ID_ATTRIBUTE,
        "attribute",
        sizeof("attribute") - 1,
        "",
        bdlat_FormattingMode::e_DEC
    }
};

// CLASS METHODS

const bdlat_AttributeInfo *EncoderTestSequenceWithAllCategoriesSequence::lookupAttributeInfo(
        const char *name,
        int         nameLength)
{
    for (int i = 0; i < 1; ++i) {
        const bdlat_AttributeInfo& attributeInfo =
                    EncoderTestSequenceWithAllCategoriesSequence::ATTRIBUTE_INFO_ARRAY[i];

        if (nameLength == attributeInfo.d_nameLength
        &&  0 == bsl::memcmp(attributeInfo.d_name_p, name, nameLength))
        {
            return &attributeInfo;
        }
    }

    return 0;
}

const bdlat_AttributeInfo *EncoderTestSequenceWithAllCategoriesSequence::lookupAttributeInfo(int id)
{
    switch (id) {
      case ATTRIBUTE_ID_ATTRIBUTE:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE];
      default:
        return 0;
    }
}

// CREATORS

EncoderTestSequenceWithAllCategoriesSequence::EncoderTestSequenceWithAllCategoriesSequence()
: d_attribute()
{
}

EncoderTestSequenceWithAllCategoriesSequence::EncoderTestSequenceWithAllCategoriesSequence(const EncoderTestSequenceWithAllCategoriesSequence& original)
: d_attribute(original.d_attribute)
{
}

EncoderTestSequenceWithAllCategoriesSequence::~EncoderTestSequenceWithAllCategoriesSequence()
{
}

// MANIPULATORS

EncoderTestSequenceWithAllCategoriesSequence&
EncoderTestSequenceWithAllCategoriesSequence::operator=(const EncoderTestSequenceWithAllCategoriesSequence& rhs)
{
    if (this != &rhs) {
        d_attribute = rhs.d_attribute;
    }

    return *this;
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
EncoderTestSequenceWithAllCategoriesSequence&
EncoderTestSequenceWithAllCategoriesSequence::operator=(EncoderTestSequenceWithAllCategoriesSequence&& rhs)
{
    if (this != &rhs) {
        d_attribute = bsl::move(rhs.d_attribute);
    }

    return *this;
}
#endif

void EncoderTestSequenceWithAllCategoriesSequence::reset()
{
    bdlat_ValueTypeFunctions::reset(&d_attribute);
}

// ACCESSORS

bsl::ostream& EncoderTestSequenceWithAllCategoriesSequence::print(
        bsl::ostream& stream,
        int           level,
        int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute("attribute", d_attribute);
    printer.end();
    return stream;
}



                   // --------------------------------------
                   // class EncoderTestSequenceWithUntagged0
                   // --------------------------------------

// CONSTANTS

const char EncoderTestSequenceWithUntagged0::CLASS_NAME[] = "EncoderTestSequenceWithUntagged0";


// CLASS METHODS

const bdlat_AttributeInfo *EncoderTestSequenceWithUntagged0::lookupAttributeInfo(
        const char *name,
        int         nameLength)
{
    (void)name;
    (void)nameLength;
    return 0;
}

const bdlat_AttributeInfo *EncoderTestSequenceWithUntagged0::lookupAttributeInfo(int id)
{
    switch (id) {
      default:
        return 0;
    }
}

// CREATORS

EncoderTestSequenceWithUntagged0::EncoderTestSequenceWithUntagged0()
{
}

EncoderTestSequenceWithUntagged0::EncoderTestSequenceWithUntagged0(const EncoderTestSequenceWithUntagged0& original)
{
    (void)original;
}

EncoderTestSequenceWithUntagged0::~EncoderTestSequenceWithUntagged0()
{
}

// MANIPULATORS

EncoderTestSequenceWithUntagged0&
EncoderTestSequenceWithUntagged0::operator=(const EncoderTestSequenceWithUntagged0& rhs)
{
    (void)rhs;
    return *this;
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
EncoderTestSequenceWithUntagged0&
EncoderTestSequenceWithUntagged0::operator=(EncoderTestSequenceWithUntagged0&& rhs)
{
    (void)rhs;
    return *this;
}
#endif

void EncoderTestSequenceWithUntagged0::reset()
{
}

// ACCESSORS

bsl::ostream& EncoderTestSequenceWithUntagged0::print(
        bsl::ostream& stream,
        int           level,
        int           spacesPerLevel) const
{
    (void)level;
    (void)spacesPerLevel;
    return stream;
}



              // -----------------------------------------------
              // class EncoderTestSequenceWithUntagged10Sequence
              // -----------------------------------------------

// CONSTANTS

const char EncoderTestSequenceWithUntagged10Sequence::CLASS_NAME[] = "EncoderTestSequenceWithUntagged10Sequence";


// CLASS METHODS

const bdlat_AttributeInfo *EncoderTestSequenceWithUntagged10Sequence::lookupAttributeInfo(
        const char *name,
        int         nameLength)
{
    (void)name;
    (void)nameLength;
    return 0;
}

const bdlat_AttributeInfo *EncoderTestSequenceWithUntagged10Sequence::lookupAttributeInfo(int id)
{
    switch (id) {
      default:
        return 0;
    }
}

// CREATORS

EncoderTestSequenceWithUntagged10Sequence::EncoderTestSequenceWithUntagged10Sequence()
{
}

EncoderTestSequenceWithUntagged10Sequence::EncoderTestSequenceWithUntagged10Sequence(const EncoderTestSequenceWithUntagged10Sequence& original)
{
    (void)original;
}

EncoderTestSequenceWithUntagged10Sequence::~EncoderTestSequenceWithUntagged10Sequence()
{
}

// MANIPULATORS

EncoderTestSequenceWithUntagged10Sequence&
EncoderTestSequenceWithUntagged10Sequence::operator=(const EncoderTestSequenceWithUntagged10Sequence& rhs)
{
    (void)rhs;
    return *this;
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
EncoderTestSequenceWithUntagged10Sequence&
EncoderTestSequenceWithUntagged10Sequence::operator=(EncoderTestSequenceWithUntagged10Sequence&& rhs)
{
    (void)rhs;
    return *this;
}
#endif

void EncoderTestSequenceWithUntagged10Sequence::reset()
{
}

// ACCESSORS

bsl::ostream& EncoderTestSequenceWithUntagged10Sequence::print(
        bsl::ostream& stream,
        int           level,
        int           spacesPerLevel) const
{
    (void)level;
    (void)spacesPerLevel;
    return stream;
}



              // -----------------------------------------------
              // class EncoderTestSequenceWithUntagged11Sequence
              // -----------------------------------------------

// CONSTANTS

const char EncoderTestSequenceWithUntagged11Sequence::CLASS_NAME[] = "EncoderTestSequenceWithUntagged11Sequence";


// CLASS METHODS

const bdlat_AttributeInfo *EncoderTestSequenceWithUntagged11Sequence::lookupAttributeInfo(
        const char *name,
        int         nameLength)
{
    (void)name;
    (void)nameLength;
    return 0;
}

const bdlat_AttributeInfo *EncoderTestSequenceWithUntagged11Sequence::lookupAttributeInfo(int id)
{
    switch (id) {
      default:
        return 0;
    }
}

// CREATORS

EncoderTestSequenceWithUntagged11Sequence::EncoderTestSequenceWithUntagged11Sequence()
{
}

EncoderTestSequenceWithUntagged11Sequence::EncoderTestSequenceWithUntagged11Sequence(const EncoderTestSequenceWithUntagged11Sequence& original)
{
    (void)original;
}

EncoderTestSequenceWithUntagged11Sequence::~EncoderTestSequenceWithUntagged11Sequence()
{
}

// MANIPULATORS

EncoderTestSequenceWithUntagged11Sequence&
EncoderTestSequenceWithUntagged11Sequence::operator=(const EncoderTestSequenceWithUntagged11Sequence& rhs)
{
    (void)rhs;
    return *this;
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
EncoderTestSequenceWithUntagged11Sequence&
EncoderTestSequenceWithUntagged11Sequence::operator=(EncoderTestSequenceWithUntagged11Sequence&& rhs)
{
    (void)rhs;
    return *this;
}
#endif

void EncoderTestSequenceWithUntagged11Sequence::reset()
{
}

// ACCESSORS

bsl::ostream& EncoderTestSequenceWithUntagged11Sequence::print(
        bsl::ostream& stream,
        int           level,
        int           spacesPerLevel) const
{
    (void)level;
    (void)spacesPerLevel;
    return stream;
}



              // ------------------------------------------------
              // class EncoderTestSequenceWithUntagged11Sequence1
              // ------------------------------------------------

// CONSTANTS

const char EncoderTestSequenceWithUntagged11Sequence1::CLASS_NAME[] = "EncoderTestSequenceWithUntagged11Sequence1";


// CLASS METHODS

const bdlat_AttributeInfo *EncoderTestSequenceWithUntagged11Sequence1::lookupAttributeInfo(
        const char *name,
        int         nameLength)
{
    (void)name;
    (void)nameLength;
    return 0;
}

const bdlat_AttributeInfo *EncoderTestSequenceWithUntagged11Sequence1::lookupAttributeInfo(int id)
{
    switch (id) {
      default:
        return 0;
    }
}

// CREATORS

EncoderTestSequenceWithUntagged11Sequence1::EncoderTestSequenceWithUntagged11Sequence1()
{
}

EncoderTestSequenceWithUntagged11Sequence1::EncoderTestSequenceWithUntagged11Sequence1(const EncoderTestSequenceWithUntagged11Sequence1& original)
{
    (void)original;
}

EncoderTestSequenceWithUntagged11Sequence1::~EncoderTestSequenceWithUntagged11Sequence1()
{
}

// MANIPULATORS

EncoderTestSequenceWithUntagged11Sequence1&
EncoderTestSequenceWithUntagged11Sequence1::operator=(const EncoderTestSequenceWithUntagged11Sequence1& rhs)
{
    (void)rhs;
    return *this;
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
EncoderTestSequenceWithUntagged11Sequence1&
EncoderTestSequenceWithUntagged11Sequence1::operator=(EncoderTestSequenceWithUntagged11Sequence1&& rhs)
{
    (void)rhs;
    return *this;
}
#endif

void EncoderTestSequenceWithUntagged11Sequence1::reset()
{
}

// ACCESSORS

bsl::ostream& EncoderTestSequenceWithUntagged11Sequence1::print(
        bsl::ostream& stream,
        int           level,
        int           spacesPerLevel) const
{
    (void)level;
    (void)spacesPerLevel;
    return stream;
}



              // -----------------------------------------------
              // class EncoderTestSequenceWithUntagged12Sequence
              // -----------------------------------------------

// CONSTANTS

const char EncoderTestSequenceWithUntagged12Sequence::CLASS_NAME[] = "EncoderTestSequenceWithUntagged12Sequence";


// CLASS METHODS

const bdlat_AttributeInfo *EncoderTestSequenceWithUntagged12Sequence::lookupAttributeInfo(
        const char *name,
        int         nameLength)
{
    (void)name;
    (void)nameLength;
    return 0;
}

const bdlat_AttributeInfo *EncoderTestSequenceWithUntagged12Sequence::lookupAttributeInfo(int id)
{
    switch (id) {
      default:
        return 0;
    }
}

// CREATORS

EncoderTestSequenceWithUntagged12Sequence::EncoderTestSequenceWithUntagged12Sequence()
{
}

EncoderTestSequenceWithUntagged12Sequence::EncoderTestSequenceWithUntagged12Sequence(const EncoderTestSequenceWithUntagged12Sequence& original)
{
    (void)original;
}

EncoderTestSequenceWithUntagged12Sequence::~EncoderTestSequenceWithUntagged12Sequence()
{
}

// MANIPULATORS

EncoderTestSequenceWithUntagged12Sequence&
EncoderTestSequenceWithUntagged12Sequence::operator=(const EncoderTestSequenceWithUntagged12Sequence& rhs)
{
    (void)rhs;
    return *this;
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
EncoderTestSequenceWithUntagged12Sequence&
EncoderTestSequenceWithUntagged12Sequence::operator=(EncoderTestSequenceWithUntagged12Sequence&& rhs)
{
    (void)rhs;
    return *this;
}
#endif

void EncoderTestSequenceWithUntagged12Sequence::reset()
{
}

// ACCESSORS

bsl::ostream& EncoderTestSequenceWithUntagged12Sequence::print(
        bsl::ostream& stream,
        int           level,
        int           spacesPerLevel) const
{
    (void)level;
    (void)spacesPerLevel;
    return stream;
}



              // -----------------------------------------------
              // class EncoderTestSequenceWithUntagged13Sequence
              // -----------------------------------------------

// CONSTANTS

const char EncoderTestSequenceWithUntagged13Sequence::CLASS_NAME[] = "EncoderTestSequenceWithUntagged13Sequence";


// CLASS METHODS

const bdlat_AttributeInfo *EncoderTestSequenceWithUntagged13Sequence::lookupAttributeInfo(
        const char *name,
        int         nameLength)
{
    (void)name;
    (void)nameLength;
    return 0;
}

const bdlat_AttributeInfo *EncoderTestSequenceWithUntagged13Sequence::lookupAttributeInfo(int id)
{
    switch (id) {
      default:
        return 0;
    }
}

// CREATORS

EncoderTestSequenceWithUntagged13Sequence::EncoderTestSequenceWithUntagged13Sequence()
{
}

EncoderTestSequenceWithUntagged13Sequence::EncoderTestSequenceWithUntagged13Sequence(const EncoderTestSequenceWithUntagged13Sequence& original)
{
    (void)original;
}

EncoderTestSequenceWithUntagged13Sequence::~EncoderTestSequenceWithUntagged13Sequence()
{
}

// MANIPULATORS

EncoderTestSequenceWithUntagged13Sequence&
EncoderTestSequenceWithUntagged13Sequence::operator=(const EncoderTestSequenceWithUntagged13Sequence& rhs)
{
    (void)rhs;
    return *this;
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
EncoderTestSequenceWithUntagged13Sequence&
EncoderTestSequenceWithUntagged13Sequence::operator=(EncoderTestSequenceWithUntagged13Sequence&& rhs)
{
    (void)rhs;
    return *this;
}
#endif

void EncoderTestSequenceWithUntagged13Sequence::reset()
{
}

// ACCESSORS

bsl::ostream& EncoderTestSequenceWithUntagged13Sequence::print(
        bsl::ostream& stream,
        int           level,
        int           spacesPerLevel) const
{
    (void)level;
    (void)spacesPerLevel;
    return stream;
}



                  // ---------------------------------------
                  // class EncoderTestSequenceWithUntagged14
                  // ---------------------------------------

// CONSTANTS

const char EncoderTestSequenceWithUntagged14::CLASS_NAME[] = "EncoderTestSequenceWithUntagged14";

const bdlat_AttributeInfo EncoderTestSequenceWithUntagged14::ATTRIBUTE_INFO_ARRAY[] = {
    {
        ATTRIBUTE_ID_ATTRIBUTE0,
        "attribute0",
        sizeof("attribute0") - 1,
        "",
        bdlat_FormattingMode::e_DEC
    },
    {
        ATTRIBUTE_ID_ATTRIBUTE1,
        "attribute1",
        sizeof("attribute1") - 1,
        "",
        bdlat_FormattingMode::e_DEC
    },
    {
        ATTRIBUTE_ID_ATTRIBUTE2,
        "attribute2",
        sizeof("attribute2") - 1,
        "",
        bdlat_FormattingMode::e_DEC
    }
};

// CLASS METHODS

const bdlat_AttributeInfo *EncoderTestSequenceWithUntagged14::lookupAttributeInfo(
        const char *name,
        int         nameLength)
{
    for (int i = 0; i < 3; ++i) {
        const bdlat_AttributeInfo& attributeInfo =
                    EncoderTestSequenceWithUntagged14::ATTRIBUTE_INFO_ARRAY[i];

        if (nameLength == attributeInfo.d_nameLength
        &&  0 == bsl::memcmp(attributeInfo.d_name_p, name, nameLength))
        {
            return &attributeInfo;
        }
    }

    return 0;
}

const bdlat_AttributeInfo *EncoderTestSequenceWithUntagged14::lookupAttributeInfo(int id)
{
    switch (id) {
      case ATTRIBUTE_ID_ATTRIBUTE0:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE0];
      case ATTRIBUTE_ID_ATTRIBUTE1:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE1];
      case ATTRIBUTE_ID_ATTRIBUTE2:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE2];
      default:
        return 0;
    }
}

// CREATORS

EncoderTestSequenceWithUntagged14::EncoderTestSequenceWithUntagged14()
: d_attribute0()
, d_attribute1()
, d_attribute2()
{
}

EncoderTestSequenceWithUntagged14::EncoderTestSequenceWithUntagged14(const EncoderTestSequenceWithUntagged14& original)
: d_attribute0(original.d_attribute0)
, d_attribute1(original.d_attribute1)
, d_attribute2(original.d_attribute2)
{
}

EncoderTestSequenceWithUntagged14::~EncoderTestSequenceWithUntagged14()
{
}

// MANIPULATORS

EncoderTestSequenceWithUntagged14&
EncoderTestSequenceWithUntagged14::operator=(const EncoderTestSequenceWithUntagged14& rhs)
{
    if (this != &rhs) {
        d_attribute0 = rhs.d_attribute0;
        d_attribute1 = rhs.d_attribute1;
        d_attribute2 = rhs.d_attribute2;
    }

    return *this;
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
EncoderTestSequenceWithUntagged14&
EncoderTestSequenceWithUntagged14::operator=(EncoderTestSequenceWithUntagged14&& rhs)
{
    if (this != &rhs) {
        d_attribute0 = bsl::move(rhs.d_attribute0);
        d_attribute1 = bsl::move(rhs.d_attribute1);
        d_attribute2 = bsl::move(rhs.d_attribute2);
    }

    return *this;
}
#endif

void EncoderTestSequenceWithUntagged14::reset()
{
    bdlat_ValueTypeFunctions::reset(&d_attribute0);
    bdlat_ValueTypeFunctions::reset(&d_attribute1);
    bdlat_ValueTypeFunctions::reset(&d_attribute2);
}

// ACCESSORS

bsl::ostream& EncoderTestSequenceWithUntagged14::print(
        bsl::ostream& stream,
        int           level,
        int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute("attribute0", d_attribute0);
    printer.printAttribute("attribute1", d_attribute1);
    printer.printAttribute("attribute2", d_attribute2);
    printer.end();
    return stream;
}



               // ----------------------------------------------
               // class EncoderTestSequenceWithUntagged1Sequence
               // ----------------------------------------------

// CONSTANTS

const char EncoderTestSequenceWithUntagged1Sequence::CLASS_NAME[] = "EncoderTestSequenceWithUntagged1Sequence";


// CLASS METHODS

const bdlat_AttributeInfo *EncoderTestSequenceWithUntagged1Sequence::lookupAttributeInfo(
        const char *name,
        int         nameLength)
{
    (void)name;
    (void)nameLength;
    return 0;
}

const bdlat_AttributeInfo *EncoderTestSequenceWithUntagged1Sequence::lookupAttributeInfo(int id)
{
    switch (id) {
      default:
        return 0;
    }
}

// CREATORS

EncoderTestSequenceWithUntagged1Sequence::EncoderTestSequenceWithUntagged1Sequence()
{
}

EncoderTestSequenceWithUntagged1Sequence::EncoderTestSequenceWithUntagged1Sequence(const EncoderTestSequenceWithUntagged1Sequence& original)
{
    (void)original;
}

EncoderTestSequenceWithUntagged1Sequence::~EncoderTestSequenceWithUntagged1Sequence()
{
}

// MANIPULATORS

EncoderTestSequenceWithUntagged1Sequence&
EncoderTestSequenceWithUntagged1Sequence::operator=(const EncoderTestSequenceWithUntagged1Sequence& rhs)
{
    (void)rhs;
    return *this;
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
EncoderTestSequenceWithUntagged1Sequence&
EncoderTestSequenceWithUntagged1Sequence::operator=(EncoderTestSequenceWithUntagged1Sequence&& rhs)
{
    (void)rhs;
    return *this;
}
#endif

void EncoderTestSequenceWithUntagged1Sequence::reset()
{
}

// ACCESSORS

bsl::ostream& EncoderTestSequenceWithUntagged1Sequence::print(
        bsl::ostream& stream,
        int           level,
        int           spacesPerLevel) const
{
    (void)level;
    (void)spacesPerLevel;
    return stream;
}



                   // --------------------------------------
                   // class EncoderTestSequenceWithUntagged2
                   // --------------------------------------

// CONSTANTS

const char EncoderTestSequenceWithUntagged2::CLASS_NAME[] = "EncoderTestSequenceWithUntagged2";

const bdlat_AttributeInfo EncoderTestSequenceWithUntagged2::ATTRIBUTE_INFO_ARRAY[] = {
    {
        ATTRIBUTE_ID_ATTRIBUTE0,
        "attribute0",
        sizeof("attribute0") - 1,
        "",
        bdlat_FormattingMode::e_DEC
    }
};

// CLASS METHODS

const bdlat_AttributeInfo *EncoderTestSequenceWithUntagged2::lookupAttributeInfo(
        const char *name,
        int         nameLength)
{
    for (int i = 0; i < 1; ++i) {
        const bdlat_AttributeInfo& attributeInfo =
                    EncoderTestSequenceWithUntagged2::ATTRIBUTE_INFO_ARRAY[i];

        if (nameLength == attributeInfo.d_nameLength
        &&  0 == bsl::memcmp(attributeInfo.d_name_p, name, nameLength))
        {
            return &attributeInfo;
        }
    }

    return 0;
}

const bdlat_AttributeInfo *EncoderTestSequenceWithUntagged2::lookupAttributeInfo(int id)
{
    switch (id) {
      case ATTRIBUTE_ID_ATTRIBUTE0:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE0];
      default:
        return 0;
    }
}

// CREATORS

EncoderTestSequenceWithUntagged2::EncoderTestSequenceWithUntagged2()
: d_attribute0()
{
}

EncoderTestSequenceWithUntagged2::EncoderTestSequenceWithUntagged2(const EncoderTestSequenceWithUntagged2& original)
: d_attribute0(original.d_attribute0)
{
}

EncoderTestSequenceWithUntagged2::~EncoderTestSequenceWithUntagged2()
{
}

// MANIPULATORS

EncoderTestSequenceWithUntagged2&
EncoderTestSequenceWithUntagged2::operator=(const EncoderTestSequenceWithUntagged2& rhs)
{
    if (this != &rhs) {
        d_attribute0 = rhs.d_attribute0;
    }

    return *this;
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
EncoderTestSequenceWithUntagged2&
EncoderTestSequenceWithUntagged2::operator=(EncoderTestSequenceWithUntagged2&& rhs)
{
    if (this != &rhs) {
        d_attribute0 = bsl::move(rhs.d_attribute0);
    }

    return *this;
}
#endif

void EncoderTestSequenceWithUntagged2::reset()
{
    bdlat_ValueTypeFunctions::reset(&d_attribute0);
}

// ACCESSORS

bsl::ostream& EncoderTestSequenceWithUntagged2::print(
        bsl::ostream& stream,
        int           level,
        int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute("attribute0", d_attribute0);
    printer.end();
    return stream;
}



               // ----------------------------------------------
               // class EncoderTestSequenceWithUntagged3Sequence
               // ----------------------------------------------

// CONSTANTS

const char EncoderTestSequenceWithUntagged3Sequence::CLASS_NAME[] = "EncoderTestSequenceWithUntagged3Sequence";


// CLASS METHODS

const bdlat_AttributeInfo *EncoderTestSequenceWithUntagged3Sequence::lookupAttributeInfo(
        const char *name,
        int         nameLength)
{
    (void)name;
    (void)nameLength;
    return 0;
}

const bdlat_AttributeInfo *EncoderTestSequenceWithUntagged3Sequence::lookupAttributeInfo(int id)
{
    switch (id) {
      default:
        return 0;
    }
}

// CREATORS

EncoderTestSequenceWithUntagged3Sequence::EncoderTestSequenceWithUntagged3Sequence()
{
}

EncoderTestSequenceWithUntagged3Sequence::EncoderTestSequenceWithUntagged3Sequence(const EncoderTestSequenceWithUntagged3Sequence& original)
{
    (void)original;
}

EncoderTestSequenceWithUntagged3Sequence::~EncoderTestSequenceWithUntagged3Sequence()
{
}

// MANIPULATORS

EncoderTestSequenceWithUntagged3Sequence&
EncoderTestSequenceWithUntagged3Sequence::operator=(const EncoderTestSequenceWithUntagged3Sequence& rhs)
{
    (void)rhs;
    return *this;
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
EncoderTestSequenceWithUntagged3Sequence&
EncoderTestSequenceWithUntagged3Sequence::operator=(EncoderTestSequenceWithUntagged3Sequence&& rhs)
{
    (void)rhs;
    return *this;
}
#endif

void EncoderTestSequenceWithUntagged3Sequence::reset()
{
}

// ACCESSORS

bsl::ostream& EncoderTestSequenceWithUntagged3Sequence::print(
        bsl::ostream& stream,
        int           level,
        int           spacesPerLevel) const
{
    (void)level;
    (void)spacesPerLevel;
    return stream;
}



              // -----------------------------------------------
              // class EncoderTestSequenceWithUntagged3Sequence1
              // -----------------------------------------------

// CONSTANTS

const char EncoderTestSequenceWithUntagged3Sequence1::CLASS_NAME[] = "EncoderTestSequenceWithUntagged3Sequence1";


// CLASS METHODS

const bdlat_AttributeInfo *EncoderTestSequenceWithUntagged3Sequence1::lookupAttributeInfo(
        const char *name,
        int         nameLength)
{
    (void)name;
    (void)nameLength;
    return 0;
}

const bdlat_AttributeInfo *EncoderTestSequenceWithUntagged3Sequence1::lookupAttributeInfo(int id)
{
    switch (id) {
      default:
        return 0;
    }
}

// CREATORS

EncoderTestSequenceWithUntagged3Sequence1::EncoderTestSequenceWithUntagged3Sequence1()
{
}

EncoderTestSequenceWithUntagged3Sequence1::EncoderTestSequenceWithUntagged3Sequence1(const EncoderTestSequenceWithUntagged3Sequence1& original)
{
    (void)original;
}

EncoderTestSequenceWithUntagged3Sequence1::~EncoderTestSequenceWithUntagged3Sequence1()
{
}

// MANIPULATORS

EncoderTestSequenceWithUntagged3Sequence1&
EncoderTestSequenceWithUntagged3Sequence1::operator=(const EncoderTestSequenceWithUntagged3Sequence1& rhs)
{
    (void)rhs;
    return *this;
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
EncoderTestSequenceWithUntagged3Sequence1&
EncoderTestSequenceWithUntagged3Sequence1::operator=(EncoderTestSequenceWithUntagged3Sequence1&& rhs)
{
    (void)rhs;
    return *this;
}
#endif

void EncoderTestSequenceWithUntagged3Sequence1::reset()
{
}

// ACCESSORS

bsl::ostream& EncoderTestSequenceWithUntagged3Sequence1::print(
        bsl::ostream& stream,
        int           level,
        int           spacesPerLevel) const
{
    (void)level;
    (void)spacesPerLevel;
    return stream;
}



               // ----------------------------------------------
               // class EncoderTestSequenceWithUntagged4Sequence
               // ----------------------------------------------

// CONSTANTS

const char EncoderTestSequenceWithUntagged4Sequence::CLASS_NAME[] = "EncoderTestSequenceWithUntagged4Sequence";


// CLASS METHODS

const bdlat_AttributeInfo *EncoderTestSequenceWithUntagged4Sequence::lookupAttributeInfo(
        const char *name,
        int         nameLength)
{
    (void)name;
    (void)nameLength;
    return 0;
}

const bdlat_AttributeInfo *EncoderTestSequenceWithUntagged4Sequence::lookupAttributeInfo(int id)
{
    switch (id) {
      default:
        return 0;
    }
}

// CREATORS

EncoderTestSequenceWithUntagged4Sequence::EncoderTestSequenceWithUntagged4Sequence()
{
}

EncoderTestSequenceWithUntagged4Sequence::EncoderTestSequenceWithUntagged4Sequence(const EncoderTestSequenceWithUntagged4Sequence& original)
{
    (void)original;
}

EncoderTestSequenceWithUntagged4Sequence::~EncoderTestSequenceWithUntagged4Sequence()
{
}

// MANIPULATORS

EncoderTestSequenceWithUntagged4Sequence&
EncoderTestSequenceWithUntagged4Sequence::operator=(const EncoderTestSequenceWithUntagged4Sequence& rhs)
{
    (void)rhs;
    return *this;
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
EncoderTestSequenceWithUntagged4Sequence&
EncoderTestSequenceWithUntagged4Sequence::operator=(EncoderTestSequenceWithUntagged4Sequence&& rhs)
{
    (void)rhs;
    return *this;
}
#endif

void EncoderTestSequenceWithUntagged4Sequence::reset()
{
}

// ACCESSORS

bsl::ostream& EncoderTestSequenceWithUntagged4Sequence::print(
        bsl::ostream& stream,
        int           level,
        int           spacesPerLevel) const
{
    (void)level;
    (void)spacesPerLevel;
    return stream;
}



               // ----------------------------------------------
               // class EncoderTestSequenceWithUntagged5Sequence
               // ----------------------------------------------

// CONSTANTS

const char EncoderTestSequenceWithUntagged5Sequence::CLASS_NAME[] = "EncoderTestSequenceWithUntagged5Sequence";


// CLASS METHODS

const bdlat_AttributeInfo *EncoderTestSequenceWithUntagged5Sequence::lookupAttributeInfo(
        const char *name,
        int         nameLength)
{
    (void)name;
    (void)nameLength;
    return 0;
}

const bdlat_AttributeInfo *EncoderTestSequenceWithUntagged5Sequence::lookupAttributeInfo(int id)
{
    switch (id) {
      default:
        return 0;
    }
}

// CREATORS

EncoderTestSequenceWithUntagged5Sequence::EncoderTestSequenceWithUntagged5Sequence()
{
}

EncoderTestSequenceWithUntagged5Sequence::EncoderTestSequenceWithUntagged5Sequence(const EncoderTestSequenceWithUntagged5Sequence& original)
{
    (void)original;
}

EncoderTestSequenceWithUntagged5Sequence::~EncoderTestSequenceWithUntagged5Sequence()
{
}

// MANIPULATORS

EncoderTestSequenceWithUntagged5Sequence&
EncoderTestSequenceWithUntagged5Sequence::operator=(const EncoderTestSequenceWithUntagged5Sequence& rhs)
{
    (void)rhs;
    return *this;
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
EncoderTestSequenceWithUntagged5Sequence&
EncoderTestSequenceWithUntagged5Sequence::operator=(EncoderTestSequenceWithUntagged5Sequence&& rhs)
{
    (void)rhs;
    return *this;
}
#endif

void EncoderTestSequenceWithUntagged5Sequence::reset()
{
}

// ACCESSORS

bsl::ostream& EncoderTestSequenceWithUntagged5Sequence::print(
        bsl::ostream& stream,
        int           level,
        int           spacesPerLevel) const
{
    (void)level;
    (void)spacesPerLevel;
    return stream;
}



                   // --------------------------------------
                   // class EncoderTestSequenceWithUntagged6
                   // --------------------------------------

// CONSTANTS

const char EncoderTestSequenceWithUntagged6::CLASS_NAME[] = "EncoderTestSequenceWithUntagged6";

const bdlat_AttributeInfo EncoderTestSequenceWithUntagged6::ATTRIBUTE_INFO_ARRAY[] = {
    {
        ATTRIBUTE_ID_ATTRIBUTE0,
        "attribute0",
        sizeof("attribute0") - 1,
        "",
        bdlat_FormattingMode::e_DEC
    },
    {
        ATTRIBUTE_ID_ATTRIBUTE1,
        "attribute1",
        sizeof("attribute1") - 1,
        "",
        bdlat_FormattingMode::e_DEC
    }
};

// CLASS METHODS

const bdlat_AttributeInfo *EncoderTestSequenceWithUntagged6::lookupAttributeInfo(
        const char *name,
        int         nameLength)
{
    for (int i = 0; i < 2; ++i) {
        const bdlat_AttributeInfo& attributeInfo =
                    EncoderTestSequenceWithUntagged6::ATTRIBUTE_INFO_ARRAY[i];

        if (nameLength == attributeInfo.d_nameLength
        &&  0 == bsl::memcmp(attributeInfo.d_name_p, name, nameLength))
        {
            return &attributeInfo;
        }
    }

    return 0;
}

const bdlat_AttributeInfo *EncoderTestSequenceWithUntagged6::lookupAttributeInfo(int id)
{
    switch (id) {
      case ATTRIBUTE_ID_ATTRIBUTE0:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE0];
      case ATTRIBUTE_ID_ATTRIBUTE1:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE1];
      default:
        return 0;
    }
}

// CREATORS

EncoderTestSequenceWithUntagged6::EncoderTestSequenceWithUntagged6()
: d_attribute0()
, d_attribute1()
{
}

EncoderTestSequenceWithUntagged6::EncoderTestSequenceWithUntagged6(const EncoderTestSequenceWithUntagged6& original)
: d_attribute0(original.d_attribute0)
, d_attribute1(original.d_attribute1)
{
}

EncoderTestSequenceWithUntagged6::~EncoderTestSequenceWithUntagged6()
{
}

// MANIPULATORS

EncoderTestSequenceWithUntagged6&
EncoderTestSequenceWithUntagged6::operator=(const EncoderTestSequenceWithUntagged6& rhs)
{
    if (this != &rhs) {
        d_attribute0 = rhs.d_attribute0;
        d_attribute1 = rhs.d_attribute1;
    }

    return *this;
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
EncoderTestSequenceWithUntagged6&
EncoderTestSequenceWithUntagged6::operator=(EncoderTestSequenceWithUntagged6&& rhs)
{
    if (this != &rhs) {
        d_attribute0 = bsl::move(rhs.d_attribute0);
        d_attribute1 = bsl::move(rhs.d_attribute1);
    }

    return *this;
}
#endif

void EncoderTestSequenceWithUntagged6::reset()
{
    bdlat_ValueTypeFunctions::reset(&d_attribute0);
    bdlat_ValueTypeFunctions::reset(&d_attribute1);
}

// ACCESSORS

bsl::ostream& EncoderTestSequenceWithUntagged6::print(
        bsl::ostream& stream,
        int           level,
        int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute("attribute0", d_attribute0);
    printer.printAttribute("attribute1", d_attribute1);
    printer.end();
    return stream;
}



               // ----------------------------------------------
               // class EncoderTestSequenceWithUntagged7Sequence
               // ----------------------------------------------

// CONSTANTS

const char EncoderTestSequenceWithUntagged7Sequence::CLASS_NAME[] = "EncoderTestSequenceWithUntagged7Sequence";


// CLASS METHODS

const bdlat_AttributeInfo *EncoderTestSequenceWithUntagged7Sequence::lookupAttributeInfo(
        const char *name,
        int         nameLength)
{
    (void)name;
    (void)nameLength;
    return 0;
}

const bdlat_AttributeInfo *EncoderTestSequenceWithUntagged7Sequence::lookupAttributeInfo(int id)
{
    switch (id) {
      default:
        return 0;
    }
}

// CREATORS

EncoderTestSequenceWithUntagged7Sequence::EncoderTestSequenceWithUntagged7Sequence()
{
}

EncoderTestSequenceWithUntagged7Sequence::EncoderTestSequenceWithUntagged7Sequence(const EncoderTestSequenceWithUntagged7Sequence& original)
{
    (void)original;
}

EncoderTestSequenceWithUntagged7Sequence::~EncoderTestSequenceWithUntagged7Sequence()
{
}

// MANIPULATORS

EncoderTestSequenceWithUntagged7Sequence&
EncoderTestSequenceWithUntagged7Sequence::operator=(const EncoderTestSequenceWithUntagged7Sequence& rhs)
{
    (void)rhs;
    return *this;
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
EncoderTestSequenceWithUntagged7Sequence&
EncoderTestSequenceWithUntagged7Sequence::operator=(EncoderTestSequenceWithUntagged7Sequence&& rhs)
{
    (void)rhs;
    return *this;
}
#endif

void EncoderTestSequenceWithUntagged7Sequence::reset()
{
}

// ACCESSORS

bsl::ostream& EncoderTestSequenceWithUntagged7Sequence::print(
        bsl::ostream& stream,
        int           level,
        int           spacesPerLevel) const
{
    (void)level;
    (void)spacesPerLevel;
    return stream;
}



              // -----------------------------------------------
              // class EncoderTestSequenceWithUntagged7Sequence1
              // -----------------------------------------------

// CONSTANTS

const char EncoderTestSequenceWithUntagged7Sequence1::CLASS_NAME[] = "EncoderTestSequenceWithUntagged7Sequence1";


// CLASS METHODS

const bdlat_AttributeInfo *EncoderTestSequenceWithUntagged7Sequence1::lookupAttributeInfo(
        const char *name,
        int         nameLength)
{
    (void)name;
    (void)nameLength;
    return 0;
}

const bdlat_AttributeInfo *EncoderTestSequenceWithUntagged7Sequence1::lookupAttributeInfo(int id)
{
    switch (id) {
      default:
        return 0;
    }
}

// CREATORS

EncoderTestSequenceWithUntagged7Sequence1::EncoderTestSequenceWithUntagged7Sequence1()
{
}

EncoderTestSequenceWithUntagged7Sequence1::EncoderTestSequenceWithUntagged7Sequence1(const EncoderTestSequenceWithUntagged7Sequence1& original)
{
    (void)original;
}

EncoderTestSequenceWithUntagged7Sequence1::~EncoderTestSequenceWithUntagged7Sequence1()
{
}

// MANIPULATORS

EncoderTestSequenceWithUntagged7Sequence1&
EncoderTestSequenceWithUntagged7Sequence1::operator=(const EncoderTestSequenceWithUntagged7Sequence1& rhs)
{
    (void)rhs;
    return *this;
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
EncoderTestSequenceWithUntagged7Sequence1&
EncoderTestSequenceWithUntagged7Sequence1::operator=(EncoderTestSequenceWithUntagged7Sequence1&& rhs)
{
    (void)rhs;
    return *this;
}
#endif

void EncoderTestSequenceWithUntagged7Sequence1::reset()
{
}

// ACCESSORS

bsl::ostream& EncoderTestSequenceWithUntagged7Sequence1::print(
        bsl::ostream& stream,
        int           level,
        int           spacesPerLevel) const
{
    (void)level;
    (void)spacesPerLevel;
    return stream;
}



              // -----------------------------------------------
              // class EncoderTestSequenceWithUntagged7Sequence2
              // -----------------------------------------------

// CONSTANTS

const char EncoderTestSequenceWithUntagged7Sequence2::CLASS_NAME[] = "EncoderTestSequenceWithUntagged7Sequence2";


// CLASS METHODS

const bdlat_AttributeInfo *EncoderTestSequenceWithUntagged7Sequence2::lookupAttributeInfo(
        const char *name,
        int         nameLength)
{
    (void)name;
    (void)nameLength;
    return 0;
}

const bdlat_AttributeInfo *EncoderTestSequenceWithUntagged7Sequence2::lookupAttributeInfo(int id)
{
    switch (id) {
      default:
        return 0;
    }
}

// CREATORS

EncoderTestSequenceWithUntagged7Sequence2::EncoderTestSequenceWithUntagged7Sequence2()
{
}

EncoderTestSequenceWithUntagged7Sequence2::EncoderTestSequenceWithUntagged7Sequence2(const EncoderTestSequenceWithUntagged7Sequence2& original)
{
    (void)original;
}

EncoderTestSequenceWithUntagged7Sequence2::~EncoderTestSequenceWithUntagged7Sequence2()
{
}

// MANIPULATORS

EncoderTestSequenceWithUntagged7Sequence2&
EncoderTestSequenceWithUntagged7Sequence2::operator=(const EncoderTestSequenceWithUntagged7Sequence2& rhs)
{
    (void)rhs;
    return *this;
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
EncoderTestSequenceWithUntagged7Sequence2&
EncoderTestSequenceWithUntagged7Sequence2::operator=(EncoderTestSequenceWithUntagged7Sequence2&& rhs)
{
    (void)rhs;
    return *this;
}
#endif

void EncoderTestSequenceWithUntagged7Sequence2::reset()
{
}

// ACCESSORS

bsl::ostream& EncoderTestSequenceWithUntagged7Sequence2::print(
        bsl::ostream& stream,
        int           level,
        int           spacesPerLevel) const
{
    (void)level;
    (void)spacesPerLevel;
    return stream;
}



               // ----------------------------------------------
               // class EncoderTestSequenceWithUntagged8Sequence
               // ----------------------------------------------

// CONSTANTS

const char EncoderTestSequenceWithUntagged8Sequence::CLASS_NAME[] = "EncoderTestSequenceWithUntagged8Sequence";


// CLASS METHODS

const bdlat_AttributeInfo *EncoderTestSequenceWithUntagged8Sequence::lookupAttributeInfo(
        const char *name,
        int         nameLength)
{
    (void)name;
    (void)nameLength;
    return 0;
}

const bdlat_AttributeInfo *EncoderTestSequenceWithUntagged8Sequence::lookupAttributeInfo(int id)
{
    switch (id) {
      default:
        return 0;
    }
}

// CREATORS

EncoderTestSequenceWithUntagged8Sequence::EncoderTestSequenceWithUntagged8Sequence()
{
}

EncoderTestSequenceWithUntagged8Sequence::EncoderTestSequenceWithUntagged8Sequence(const EncoderTestSequenceWithUntagged8Sequence& original)
{
    (void)original;
}

EncoderTestSequenceWithUntagged8Sequence::~EncoderTestSequenceWithUntagged8Sequence()
{
}

// MANIPULATORS

EncoderTestSequenceWithUntagged8Sequence&
EncoderTestSequenceWithUntagged8Sequence::operator=(const EncoderTestSequenceWithUntagged8Sequence& rhs)
{
    (void)rhs;
    return *this;
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
EncoderTestSequenceWithUntagged8Sequence&
EncoderTestSequenceWithUntagged8Sequence::operator=(EncoderTestSequenceWithUntagged8Sequence&& rhs)
{
    (void)rhs;
    return *this;
}
#endif

void EncoderTestSequenceWithUntagged8Sequence::reset()
{
}

// ACCESSORS

bsl::ostream& EncoderTestSequenceWithUntagged8Sequence::print(
        bsl::ostream& stream,
        int           level,
        int           spacesPerLevel) const
{
    (void)level;
    (void)spacesPerLevel;
    return stream;
}



              // -----------------------------------------------
              // class EncoderTestSequenceWithUntagged8Sequence1
              // -----------------------------------------------

// CONSTANTS

const char EncoderTestSequenceWithUntagged8Sequence1::CLASS_NAME[] = "EncoderTestSequenceWithUntagged8Sequence1";


// CLASS METHODS

const bdlat_AttributeInfo *EncoderTestSequenceWithUntagged8Sequence1::lookupAttributeInfo(
        const char *name,
        int         nameLength)
{
    (void)name;
    (void)nameLength;
    return 0;
}

const bdlat_AttributeInfo *EncoderTestSequenceWithUntagged8Sequence1::lookupAttributeInfo(int id)
{
    switch (id) {
      default:
        return 0;
    }
}

// CREATORS

EncoderTestSequenceWithUntagged8Sequence1::EncoderTestSequenceWithUntagged8Sequence1()
{
}

EncoderTestSequenceWithUntagged8Sequence1::EncoderTestSequenceWithUntagged8Sequence1(const EncoderTestSequenceWithUntagged8Sequence1& original)
{
    (void)original;
}

EncoderTestSequenceWithUntagged8Sequence1::~EncoderTestSequenceWithUntagged8Sequence1()
{
}

// MANIPULATORS

EncoderTestSequenceWithUntagged8Sequence1&
EncoderTestSequenceWithUntagged8Sequence1::operator=(const EncoderTestSequenceWithUntagged8Sequence1& rhs)
{
    (void)rhs;
    return *this;
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
EncoderTestSequenceWithUntagged8Sequence1&
EncoderTestSequenceWithUntagged8Sequence1::operator=(EncoderTestSequenceWithUntagged8Sequence1&& rhs)
{
    (void)rhs;
    return *this;
}
#endif

void EncoderTestSequenceWithUntagged8Sequence1::reset()
{
}

// ACCESSORS

bsl::ostream& EncoderTestSequenceWithUntagged8Sequence1::print(
        bsl::ostream& stream,
        int           level,
        int           spacesPerLevel) const
{
    (void)level;
    (void)spacesPerLevel;
    return stream;
}



               // ----------------------------------------------
               // class EncoderTestSequenceWithUntagged9Sequence
               // ----------------------------------------------

// CONSTANTS

const char EncoderTestSequenceWithUntagged9Sequence::CLASS_NAME[] = "EncoderTestSequenceWithUntagged9Sequence";


// CLASS METHODS

const bdlat_AttributeInfo *EncoderTestSequenceWithUntagged9Sequence::lookupAttributeInfo(
        const char *name,
        int         nameLength)
{
    (void)name;
    (void)nameLength;
    return 0;
}

const bdlat_AttributeInfo *EncoderTestSequenceWithUntagged9Sequence::lookupAttributeInfo(int id)
{
    switch (id) {
      default:
        return 0;
    }
}

// CREATORS

EncoderTestSequenceWithUntagged9Sequence::EncoderTestSequenceWithUntagged9Sequence()
{
}

EncoderTestSequenceWithUntagged9Sequence::EncoderTestSequenceWithUntagged9Sequence(const EncoderTestSequenceWithUntagged9Sequence& original)
{
    (void)original;
}

EncoderTestSequenceWithUntagged9Sequence::~EncoderTestSequenceWithUntagged9Sequence()
{
}

// MANIPULATORS

EncoderTestSequenceWithUntagged9Sequence&
EncoderTestSequenceWithUntagged9Sequence::operator=(const EncoderTestSequenceWithUntagged9Sequence& rhs)
{
    (void)rhs;
    return *this;
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
EncoderTestSequenceWithUntagged9Sequence&
EncoderTestSequenceWithUntagged9Sequence::operator=(EncoderTestSequenceWithUntagged9Sequence&& rhs)
{
    (void)rhs;
    return *this;
}
#endif

void EncoderTestSequenceWithUntagged9Sequence::reset()
{
}

// ACCESSORS

bsl::ostream& EncoderTestSequenceWithUntagged9Sequence::print(
        bsl::ostream& stream,
        int           level,
        int           spacesPerLevel) const
{
    (void)level;
    (void)spacesPerLevel;
    return stream;
}



              // -----------------------------------------------
              // class EncoderTestSequenceWithUntagged9Sequence1
              // -----------------------------------------------

// CONSTANTS

const char EncoderTestSequenceWithUntagged9Sequence1::CLASS_NAME[] = "EncoderTestSequenceWithUntagged9Sequence1";


// CLASS METHODS

const bdlat_AttributeInfo *EncoderTestSequenceWithUntagged9Sequence1::lookupAttributeInfo(
        const char *name,
        int         nameLength)
{
    (void)name;
    (void)nameLength;
    return 0;
}

const bdlat_AttributeInfo *EncoderTestSequenceWithUntagged9Sequence1::lookupAttributeInfo(int id)
{
    switch (id) {
      default:
        return 0;
    }
}

// CREATORS

EncoderTestSequenceWithUntagged9Sequence1::EncoderTestSequenceWithUntagged9Sequence1()
{
}

EncoderTestSequenceWithUntagged9Sequence1::EncoderTestSequenceWithUntagged9Sequence1(const EncoderTestSequenceWithUntagged9Sequence1& original)
{
    (void)original;
}

EncoderTestSequenceWithUntagged9Sequence1::~EncoderTestSequenceWithUntagged9Sequence1()
{
}

// MANIPULATORS

EncoderTestSequenceWithUntagged9Sequence1&
EncoderTestSequenceWithUntagged9Sequence1::operator=(const EncoderTestSequenceWithUntagged9Sequence1& rhs)
{
    (void)rhs;
    return *this;
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
EncoderTestSequenceWithUntagged9Sequence1&
EncoderTestSequenceWithUntagged9Sequence1::operator=(EncoderTestSequenceWithUntagged9Sequence1&& rhs)
{
    (void)rhs;
    return *this;
}
#endif

void EncoderTestSequenceWithUntagged9Sequence1::reset()
{
}

// ACCESSORS

bsl::ostream& EncoderTestSequenceWithUntagged9Sequence1::print(
        bsl::ostream& stream,
        int           level,
        int           spacesPerLevel) const
{
    (void)level;
    (void)spacesPerLevel;
    return stream;
}



                  // ----------------------------------------
                  // class EncoderTestChoiceWithAllCategories
                  // ----------------------------------------

// CONSTANTS

const char EncoderTestChoiceWithAllCategories::CLASS_NAME[] = "EncoderTestChoiceWithAllCategories";

const bdlat_SelectionInfo EncoderTestChoiceWithAllCategories::SELECTION_INFO_ARRAY[] = {
    {
        SELECTION_ID_CHAR_ARRAY,
        "charArray",
        sizeof("charArray") - 1,
        "",
        bdlat_FormattingMode::e_HEX
    },
    {
        SELECTION_ID_CHOICE,
        "choice",
        sizeof("choice") - 1,
        "",
        bdlat_FormattingMode::e_DEFAULT
    },
    {
        SELECTION_ID_CUSTOMIZED_TYPE,
        "customizedType",
        sizeof("customizedType") - 1,
        "",
        bdlat_FormattingMode::e_TEXT
    },
    {
        SELECTION_ID_ENUMERATION,
        "enumeration",
        sizeof("enumeration") - 1,
        "",
        bdlat_FormattingMode::e_DEFAULT
    },
    {
        SELECTION_ID_SEQUENCE,
        "sequence",
        sizeof("sequence") - 1,
        "",
        bdlat_FormattingMode::e_DEFAULT
    },
    {
        SELECTION_ID_SIMPLE,
        "simple",
        sizeof("simple") - 1,
        "",
        bdlat_FormattingMode::e_DEC
    }
};

// CLASS METHODS

const bdlat_SelectionInfo *EncoderTestChoiceWithAllCategories::lookupSelectionInfo(
        const char *name,
        int         nameLength)
{
    for (int i = 0; i < 6; ++i) {
        const bdlat_SelectionInfo& selectionInfo =
                    EncoderTestChoiceWithAllCategories::SELECTION_INFO_ARRAY[i];

        if (nameLength == selectionInfo.d_nameLength
        &&  0 == bsl::memcmp(selectionInfo.d_name_p, name, nameLength))
        {
            return &selectionInfo;
        }
    }

    return 0;
}

const bdlat_SelectionInfo *EncoderTestChoiceWithAllCategories::lookupSelectionInfo(int id)
{
    switch (id) {
      case SELECTION_ID_CHAR_ARRAY:
        return &SELECTION_INFO_ARRAY[SELECTION_INDEX_CHAR_ARRAY];
      case SELECTION_ID_CHOICE:
        return &SELECTION_INFO_ARRAY[SELECTION_INDEX_CHOICE];
      case SELECTION_ID_CUSTOMIZED_TYPE:
        return &SELECTION_INFO_ARRAY[SELECTION_INDEX_CUSTOMIZED_TYPE];
      case SELECTION_ID_ENUMERATION:
        return &SELECTION_INFO_ARRAY[SELECTION_INDEX_ENUMERATION];
      case SELECTION_ID_SEQUENCE:
        return &SELECTION_INFO_ARRAY[SELECTION_INDEX_SEQUENCE];
      case SELECTION_ID_SIMPLE:
        return &SELECTION_INFO_ARRAY[SELECTION_INDEX_SIMPLE];
      default:
        return 0;
    }
}

// CREATORS

EncoderTestChoiceWithAllCategories::EncoderTestChoiceWithAllCategories(
    const EncoderTestChoiceWithAllCategories& original,
    bslma::Allocator *basicAllocator)
: d_selectionId(original.d_selectionId)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    switch (d_selectionId) {
      case SELECTION_ID_CHAR_ARRAY: {
        new (d_charArray.buffer())
            bsl::vector<char>(
                original.d_charArray.object(), d_allocator_p);
      } break;
      case SELECTION_ID_CHOICE: {
        new (d_choice.buffer())
            EncoderTestChoiceWithAllCategoriesChoice(original.d_choice.object());
      } break;
      case SELECTION_ID_CUSTOMIZED_TYPE: {
        new (d_customizedType.buffer())
            EncoderTestChoiceWithAllCategoriesCustomizedType(
                original.d_customizedType.object(), d_allocator_p);
      } break;
      case SELECTION_ID_ENUMERATION: {
        new (d_enumeration.buffer())
            EncoderTestChoiceWithAllCategoriesEnumeration::Value(original.d_enumeration.object());
      } break;
      case SELECTION_ID_SEQUENCE: {
        new (d_sequence.buffer())
            EncoderTestChoiceWithAllCategoriesSequence(original.d_sequence.object());
      } break;
      case SELECTION_ID_SIMPLE: {
        new (d_simple.buffer())
            int(original.d_simple.object());
      } break;
      default:
        BSLS_ASSERT(SELECTION_ID_UNDEFINED == d_selectionId);
    }
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
EncoderTestChoiceWithAllCategories::EncoderTestChoiceWithAllCategories(EncoderTestChoiceWithAllCategories&& original) noexcept
: d_selectionId(original.d_selectionId)
, d_allocator_p(original.d_allocator_p)
{
    switch (d_selectionId) {
      case SELECTION_ID_CHAR_ARRAY: {
        new (d_charArray.buffer())
            bsl::vector<char>(
                bsl::move(original.d_charArray.object()), d_allocator_p);
      } break;
      case SELECTION_ID_CHOICE: {
        new (d_choice.buffer())
            EncoderTestChoiceWithAllCategoriesChoice(bsl::move(original.d_choice.object()));
      } break;
      case SELECTION_ID_CUSTOMIZED_TYPE: {
        new (d_customizedType.buffer())
            EncoderTestChoiceWithAllCategoriesCustomizedType(
                bsl::move(original.d_customizedType.object()), d_allocator_p);
      } break;
      case SELECTION_ID_ENUMERATION: {
        new (d_enumeration.buffer())
            EncoderTestChoiceWithAllCategoriesEnumeration::Value(bsl::move(original.d_enumeration.object()));
      } break;
      case SELECTION_ID_SEQUENCE: {
        new (d_sequence.buffer())
            EncoderTestChoiceWithAllCategoriesSequence(bsl::move(original.d_sequence.object()));
      } break;
      case SELECTION_ID_SIMPLE: {
        new (d_simple.buffer())
            int(bsl::move(original.d_simple.object()));
      } break;
      default:
        BSLS_ASSERT(SELECTION_ID_UNDEFINED == d_selectionId);
    }
}

EncoderTestChoiceWithAllCategories::EncoderTestChoiceWithAllCategories(
    EncoderTestChoiceWithAllCategories&& original,
    bslma::Allocator *basicAllocator)
: d_selectionId(original.d_selectionId)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    switch (d_selectionId) {
      case SELECTION_ID_CHAR_ARRAY: {
        new (d_charArray.buffer())
            bsl::vector<char>(
                bsl::move(original.d_charArray.object()), d_allocator_p);
      } break;
      case SELECTION_ID_CHOICE: {
        new (d_choice.buffer())
            EncoderTestChoiceWithAllCategoriesChoice(bsl::move(original.d_choice.object()));
      } break;
      case SELECTION_ID_CUSTOMIZED_TYPE: {
        new (d_customizedType.buffer())
            EncoderTestChoiceWithAllCategoriesCustomizedType(
                bsl::move(original.d_customizedType.object()), d_allocator_p);
      } break;
      case SELECTION_ID_ENUMERATION: {
        new (d_enumeration.buffer())
            EncoderTestChoiceWithAllCategoriesEnumeration::Value(bsl::move(original.d_enumeration.object()));
      } break;
      case SELECTION_ID_SEQUENCE: {
        new (d_sequence.buffer())
            EncoderTestChoiceWithAllCategoriesSequence(bsl::move(original.d_sequence.object()));
      } break;
      case SELECTION_ID_SIMPLE: {
        new (d_simple.buffer())
            int(bsl::move(original.d_simple.object()));
      } break;
      default:
        BSLS_ASSERT(SELECTION_ID_UNDEFINED == d_selectionId);
    }
}
#endif

// MANIPULATORS

EncoderTestChoiceWithAllCategories&
EncoderTestChoiceWithAllCategories::operator=(const EncoderTestChoiceWithAllCategories& rhs)
{
    if (this != &rhs) {
        switch (rhs.d_selectionId) {
          case SELECTION_ID_CHAR_ARRAY: {
            makeCharArray(rhs.d_charArray.object());
          } break;
          case SELECTION_ID_CHOICE: {
            makeChoice(rhs.d_choice.object());
          } break;
          case SELECTION_ID_CUSTOMIZED_TYPE: {
            makeCustomizedType(rhs.d_customizedType.object());
          } break;
          case SELECTION_ID_ENUMERATION: {
            makeEnumeration(rhs.d_enumeration.object());
          } break;
          case SELECTION_ID_SEQUENCE: {
            makeSequence(rhs.d_sequence.object());
          } break;
          case SELECTION_ID_SIMPLE: {
            makeSimple(rhs.d_simple.object());
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
EncoderTestChoiceWithAllCategories&
EncoderTestChoiceWithAllCategories::operator=(EncoderTestChoiceWithAllCategories&& rhs)
{
    if (this != &rhs) {
        switch (rhs.d_selectionId) {
          case SELECTION_ID_CHAR_ARRAY: {
            makeCharArray(bsl::move(rhs.d_charArray.object()));
          } break;
          case SELECTION_ID_CHOICE: {
            makeChoice(bsl::move(rhs.d_choice.object()));
          } break;
          case SELECTION_ID_CUSTOMIZED_TYPE: {
            makeCustomizedType(bsl::move(rhs.d_customizedType.object()));
          } break;
          case SELECTION_ID_ENUMERATION: {
            makeEnumeration(bsl::move(rhs.d_enumeration.object()));
          } break;
          case SELECTION_ID_SEQUENCE: {
            makeSequence(bsl::move(rhs.d_sequence.object()));
          } break;
          case SELECTION_ID_SIMPLE: {
            makeSimple(bsl::move(rhs.d_simple.object()));
          } break;
          default:
            BSLS_ASSERT(SELECTION_ID_UNDEFINED == rhs.d_selectionId);
            reset();
        }
    }

    return *this;
}
#endif

void EncoderTestChoiceWithAllCategories::reset()
{
    switch (d_selectionId) {
      case SELECTION_ID_CHAR_ARRAY: {
        typedef bsl::vector<char> Type;
        d_charArray.object().~Type();
      } break;
      case SELECTION_ID_CHOICE: {
        d_choice.object().~EncoderTestChoiceWithAllCategoriesChoice();
      } break;
      case SELECTION_ID_CUSTOMIZED_TYPE: {
        d_customizedType.object().~EncoderTestChoiceWithAllCategoriesCustomizedType();
      } break;
      case SELECTION_ID_ENUMERATION: {
        typedef EncoderTestChoiceWithAllCategoriesEnumeration::Value Type;
        d_enumeration.object().~Type();
      } break;
      case SELECTION_ID_SEQUENCE: {
        d_sequence.object().~EncoderTestChoiceWithAllCategoriesSequence();
      } break;
      case SELECTION_ID_SIMPLE: {
        // no destruction required
      } break;
      default:
        BSLS_ASSERT(SELECTION_ID_UNDEFINED == d_selectionId);
    }

    d_selectionId = SELECTION_ID_UNDEFINED;
}

int EncoderTestChoiceWithAllCategories::makeSelection(int selectionId)
{
    switch (selectionId) {
      case SELECTION_ID_CHAR_ARRAY: {
        makeCharArray();
      } break;
      case SELECTION_ID_CHOICE: {
        makeChoice();
      } break;
      case SELECTION_ID_CUSTOMIZED_TYPE: {
        makeCustomizedType();
      } break;
      case SELECTION_ID_ENUMERATION: {
        makeEnumeration();
      } break;
      case SELECTION_ID_SEQUENCE: {
        makeSequence();
      } break;
      case SELECTION_ID_SIMPLE: {
        makeSimple();
      } break;
      case SELECTION_ID_UNDEFINED: {
        reset();
      } break;
      default:
        return -1;
    }
    return 0;
}

int EncoderTestChoiceWithAllCategories::makeSelection(const char *name, int nameLength)
{
    const bdlat_SelectionInfo *selectionInfo =
                                         lookupSelectionInfo(name, nameLength);
    if (0 == selectionInfo) {
       return -1;
    }

    return makeSelection(selectionInfo->d_id);
}

bsl::vector<char>& EncoderTestChoiceWithAllCategories::makeCharArray()
{
    if (SELECTION_ID_CHAR_ARRAY == d_selectionId) {
        bdlat_ValueTypeFunctions::reset(&d_charArray.object());
    }
    else {
        reset();
        new (d_charArray.buffer())
                bsl::vector<char>(d_allocator_p);
        d_selectionId = SELECTION_ID_CHAR_ARRAY;
    }

    return d_charArray.object();
}

bsl::vector<char>& EncoderTestChoiceWithAllCategories::makeCharArray(const bsl::vector<char>& value)
{
    if (SELECTION_ID_CHAR_ARRAY == d_selectionId) {
        d_charArray.object() = value;
    }
    else {
        reset();
        new (d_charArray.buffer())
                bsl::vector<char>(value, d_allocator_p);
        d_selectionId = SELECTION_ID_CHAR_ARRAY;
    }

    return d_charArray.object();
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
bsl::vector<char>& EncoderTestChoiceWithAllCategories::makeCharArray(bsl::vector<char>&& value)
{
    if (SELECTION_ID_CHAR_ARRAY == d_selectionId) {
        d_charArray.object() = bsl::move(value);
    }
    else {
        reset();
        new (d_charArray.buffer())
                bsl::vector<char>(bsl::move(value), d_allocator_p);
        d_selectionId = SELECTION_ID_CHAR_ARRAY;
    }

    return d_charArray.object();
}
#endif

EncoderTestChoiceWithAllCategoriesChoice& EncoderTestChoiceWithAllCategories::makeChoice()
{
    if (SELECTION_ID_CHOICE == d_selectionId) {
        bdlat_ValueTypeFunctions::reset(&d_choice.object());
    }
    else {
        reset();
        new (d_choice.buffer())
            EncoderTestChoiceWithAllCategoriesChoice();
        d_selectionId = SELECTION_ID_CHOICE;
    }

    return d_choice.object();
}

EncoderTestChoiceWithAllCategoriesChoice& EncoderTestChoiceWithAllCategories::makeChoice(const EncoderTestChoiceWithAllCategoriesChoice& value)
{
    if (SELECTION_ID_CHOICE == d_selectionId) {
        d_choice.object() = value;
    }
    else {
        reset();
        new (d_choice.buffer())
                EncoderTestChoiceWithAllCategoriesChoice(value);
        d_selectionId = SELECTION_ID_CHOICE;
    }

    return d_choice.object();
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
EncoderTestChoiceWithAllCategoriesChoice& EncoderTestChoiceWithAllCategories::makeChoice(EncoderTestChoiceWithAllCategoriesChoice&& value)
{
    if (SELECTION_ID_CHOICE == d_selectionId) {
        d_choice.object() = bsl::move(value);
    }
    else {
        reset();
        new (d_choice.buffer())
                EncoderTestChoiceWithAllCategoriesChoice(bsl::move(value));
        d_selectionId = SELECTION_ID_CHOICE;
    }

    return d_choice.object();
}
#endif

EncoderTestChoiceWithAllCategoriesCustomizedType& EncoderTestChoiceWithAllCategories::makeCustomizedType()
{
    if (SELECTION_ID_CUSTOMIZED_TYPE == d_selectionId) {
        bdlat_ValueTypeFunctions::reset(&d_customizedType.object());
    }
    else {
        reset();
        new (d_customizedType.buffer())
                EncoderTestChoiceWithAllCategoriesCustomizedType(d_allocator_p);
        d_selectionId = SELECTION_ID_CUSTOMIZED_TYPE;
    }

    return d_customizedType.object();
}

EncoderTestChoiceWithAllCategoriesCustomizedType& EncoderTestChoiceWithAllCategories::makeCustomizedType(const EncoderTestChoiceWithAllCategoriesCustomizedType& value)
{
    if (SELECTION_ID_CUSTOMIZED_TYPE == d_selectionId) {
        d_customizedType.object() = value;
    }
    else {
        reset();
        new (d_customizedType.buffer())
                EncoderTestChoiceWithAllCategoriesCustomizedType(value, d_allocator_p);
        d_selectionId = SELECTION_ID_CUSTOMIZED_TYPE;
    }

    return d_customizedType.object();
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
EncoderTestChoiceWithAllCategoriesCustomizedType& EncoderTestChoiceWithAllCategories::makeCustomizedType(EncoderTestChoiceWithAllCategoriesCustomizedType&& value)
{
    if (SELECTION_ID_CUSTOMIZED_TYPE == d_selectionId) {
        d_customizedType.object() = bsl::move(value);
    }
    else {
        reset();
        new (d_customizedType.buffer())
                EncoderTestChoiceWithAllCategoriesCustomizedType(bsl::move(value), d_allocator_p);
        d_selectionId = SELECTION_ID_CUSTOMIZED_TYPE;
    }

    return d_customizedType.object();
}
#endif

EncoderTestChoiceWithAllCategoriesEnumeration::Value& EncoderTestChoiceWithAllCategories::makeEnumeration()
{
    if (SELECTION_ID_ENUMERATION == d_selectionId) {
        bdlat_ValueTypeFunctions::reset(&d_enumeration.object());
    }
    else {
        reset();
        new (d_enumeration.buffer())
                    EncoderTestChoiceWithAllCategoriesEnumeration::Value(static_cast<EncoderTestChoiceWithAllCategoriesEnumeration::Value>(0));
        d_selectionId = SELECTION_ID_ENUMERATION;
    }

    return d_enumeration.object();
}

EncoderTestChoiceWithAllCategoriesEnumeration::Value& EncoderTestChoiceWithAllCategories::makeEnumeration(EncoderTestChoiceWithAllCategoriesEnumeration::Value value)
{
    if (SELECTION_ID_ENUMERATION == d_selectionId) {
        d_enumeration.object() = value;
    }
    else {
        reset();
        new (d_enumeration.buffer())
                EncoderTestChoiceWithAllCategoriesEnumeration::Value(value);
        d_selectionId = SELECTION_ID_ENUMERATION;
    }

    return d_enumeration.object();
}

EncoderTestChoiceWithAllCategoriesSequence& EncoderTestChoiceWithAllCategories::makeSequence()
{
    if (SELECTION_ID_SEQUENCE == d_selectionId) {
        bdlat_ValueTypeFunctions::reset(&d_sequence.object());
    }
    else {
        reset();
        new (d_sequence.buffer())
            EncoderTestChoiceWithAllCategoriesSequence();
        d_selectionId = SELECTION_ID_SEQUENCE;
    }

    return d_sequence.object();
}

EncoderTestChoiceWithAllCategoriesSequence& EncoderTestChoiceWithAllCategories::makeSequence(const EncoderTestChoiceWithAllCategoriesSequence& value)
{
    if (SELECTION_ID_SEQUENCE == d_selectionId) {
        d_sequence.object() = value;
    }
    else {
        reset();
        new (d_sequence.buffer())
                EncoderTestChoiceWithAllCategoriesSequence(value);
        d_selectionId = SELECTION_ID_SEQUENCE;
    }

    return d_sequence.object();
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
EncoderTestChoiceWithAllCategoriesSequence& EncoderTestChoiceWithAllCategories::makeSequence(EncoderTestChoiceWithAllCategoriesSequence&& value)
{
    if (SELECTION_ID_SEQUENCE == d_selectionId) {
        d_sequence.object() = bsl::move(value);
    }
    else {
        reset();
        new (d_sequence.buffer())
                EncoderTestChoiceWithAllCategoriesSequence(bsl::move(value));
        d_selectionId = SELECTION_ID_SEQUENCE;
    }

    return d_sequence.object();
}
#endif

int& EncoderTestChoiceWithAllCategories::makeSimple()
{
    if (SELECTION_ID_SIMPLE == d_selectionId) {
        bdlat_ValueTypeFunctions::reset(&d_simple.object());
    }
    else {
        reset();
        new (d_simple.buffer())
            int();
        d_selectionId = SELECTION_ID_SIMPLE;
    }

    return d_simple.object();
}

int& EncoderTestChoiceWithAllCategories::makeSimple(int value)
{
    if (SELECTION_ID_SIMPLE == d_selectionId) {
        d_simple.object() = value;
    }
    else {
        reset();
        new (d_simple.buffer())
                int(value);
        d_selectionId = SELECTION_ID_SIMPLE;
    }

    return d_simple.object();
}

// ACCESSORS

bsl::ostream& EncoderTestChoiceWithAllCategories::print(
    bsl::ostream& stream,
    int           level,
    int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    switch (d_selectionId) {
      case SELECTION_ID_CHAR_ARRAY: {
        bool multilineFlag = (0 <= level);
        bdlb::Print::indent(stream, level + 1, spacesPerLevel);
        stream << (multilineFlag ? "" : " ");
        stream << "charArray = [ ";
        bdlb::Print::singleLineHexDump(
            stream, d_charArray.object().begin(), d_charArray.object().end());
        stream << " ]" << (multilineFlag ? "\n" : "");
      }  break;
      case SELECTION_ID_CHOICE: {
        printer.printAttribute("choice", d_choice.object());
      }  break;
      case SELECTION_ID_CUSTOMIZED_TYPE: {
        printer.printAttribute("customizedType", d_customizedType.object());
      }  break;
      case SELECTION_ID_ENUMERATION: {
        printer.printAttribute("enumeration", d_enumeration.object());
      }  break;
      case SELECTION_ID_SEQUENCE: {
        printer.printAttribute("sequence", d_sequence.object());
      }  break;
      case SELECTION_ID_SIMPLE: {
        printer.printAttribute("simple", d_simple.object());
      }  break;
      default:
        stream << "SELECTION UNDEFINED\n";
    }
    printer.end();
    return stream;
}


const char *EncoderTestChoiceWithAllCategories::selectionName() const
{
    switch (d_selectionId) {
      case SELECTION_ID_CHAR_ARRAY:
        return SELECTION_INFO_ARRAY[SELECTION_INDEX_CHAR_ARRAY].name();
      case SELECTION_ID_CHOICE:
        return SELECTION_INFO_ARRAY[SELECTION_INDEX_CHOICE].name();
      case SELECTION_ID_CUSTOMIZED_TYPE:
        return SELECTION_INFO_ARRAY[SELECTION_INDEX_CUSTOMIZED_TYPE].name();
      case SELECTION_ID_ENUMERATION:
        return SELECTION_INFO_ARRAY[SELECTION_INDEX_ENUMERATION].name();
      case SELECTION_ID_SEQUENCE:
        return SELECTION_INFO_ARRAY[SELECTION_INDEX_SEQUENCE].name();
      case SELECTION_ID_SIMPLE:
        return SELECTION_INFO_ARRAY[SELECTION_INDEX_SIMPLE].name();
      default:
        BSLS_ASSERT(SELECTION_ID_UNDEFINED == d_selectionId);
        return "(* UNDEFINED *)";
    }
}

                     // ----------------------------------
                     // class EncoderTestDegenerateChoice1
                     // ----------------------------------

// CONSTANTS

const char EncoderTestDegenerateChoice1::CLASS_NAME[] = "EncoderTestDegenerateChoice1";

const bdlat_SelectionInfo EncoderTestDegenerateChoice1::SELECTION_INFO_ARRAY[] = {
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

const bdlat_SelectionInfo *EncoderTestDegenerateChoice1::lookupSelectionInfo(
        const char *name,
        int         nameLength)
{
    for (int i = 0; i < 1; ++i) {
        const bdlat_SelectionInfo& selectionInfo =
                    EncoderTestDegenerateChoice1::SELECTION_INFO_ARRAY[i];

        if (nameLength == selectionInfo.d_nameLength
        &&  0 == bsl::memcmp(selectionInfo.d_name_p, name, nameLength))
        {
            return &selectionInfo;
        }
    }

    return 0;
}

const bdlat_SelectionInfo *EncoderTestDegenerateChoice1::lookupSelectionInfo(int id)
{
    switch (id) {
      case SELECTION_ID_SEQUENCE:
        return &SELECTION_INFO_ARRAY[SELECTION_INDEX_SEQUENCE];
      default:
        return 0;
    }
}

// CREATORS

EncoderTestDegenerateChoice1::EncoderTestDegenerateChoice1(const EncoderTestDegenerateChoice1& original)
: d_selectionId(original.d_selectionId)
{
    switch (d_selectionId) {
      case SELECTION_ID_SEQUENCE: {
        new (d_sequence.buffer())
            EncoderTestDegenerateChoice1Sequence(original.d_sequence.object());
      } break;
      default:
        BSLS_ASSERT(SELECTION_ID_UNDEFINED == d_selectionId);
    }
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
EncoderTestDegenerateChoice1::EncoderTestDegenerateChoice1(EncoderTestDegenerateChoice1&& original) noexcept
: d_selectionId(original.d_selectionId)
{
    switch (d_selectionId) {
      case SELECTION_ID_SEQUENCE: {
        new (d_sequence.buffer())
            EncoderTestDegenerateChoice1Sequence(bsl::move(original.d_sequence.object()));
      } break;
      default:
        BSLS_ASSERT(SELECTION_ID_UNDEFINED == d_selectionId);
    }
}
#endif

// MANIPULATORS

EncoderTestDegenerateChoice1&
EncoderTestDegenerateChoice1::operator=(const EncoderTestDegenerateChoice1& rhs)
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
EncoderTestDegenerateChoice1&
EncoderTestDegenerateChoice1::operator=(EncoderTestDegenerateChoice1&& rhs)
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

void EncoderTestDegenerateChoice1::reset()
{
    switch (d_selectionId) {
      case SELECTION_ID_SEQUENCE: {
        d_sequence.object().~EncoderTestDegenerateChoice1Sequence();
      } break;
      default:
        BSLS_ASSERT(SELECTION_ID_UNDEFINED == d_selectionId);
    }

    d_selectionId = SELECTION_ID_UNDEFINED;
}

int EncoderTestDegenerateChoice1::makeSelection(int selectionId)
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

int EncoderTestDegenerateChoice1::makeSelection(const char *name, int nameLength)
{
    const bdlat_SelectionInfo *selectionInfo =
                                         lookupSelectionInfo(name, nameLength);
    if (0 == selectionInfo) {
       return -1;
    }

    return makeSelection(selectionInfo->d_id);
}

EncoderTestDegenerateChoice1Sequence& EncoderTestDegenerateChoice1::makeSequence()
{
    if (SELECTION_ID_SEQUENCE == d_selectionId) {
        bdlat_ValueTypeFunctions::reset(&d_sequence.object());
    }
    else {
        reset();
        new (d_sequence.buffer())
            EncoderTestDegenerateChoice1Sequence();
        d_selectionId = SELECTION_ID_SEQUENCE;
    }

    return d_sequence.object();
}

EncoderTestDegenerateChoice1Sequence& EncoderTestDegenerateChoice1::makeSequence(const EncoderTestDegenerateChoice1Sequence& value)
{
    if (SELECTION_ID_SEQUENCE == d_selectionId) {
        d_sequence.object() = value;
    }
    else {
        reset();
        new (d_sequence.buffer())
                EncoderTestDegenerateChoice1Sequence(value);
        d_selectionId = SELECTION_ID_SEQUENCE;
    }

    return d_sequence.object();
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
EncoderTestDegenerateChoice1Sequence& EncoderTestDegenerateChoice1::makeSequence(EncoderTestDegenerateChoice1Sequence&& value)
{
    if (SELECTION_ID_SEQUENCE == d_selectionId) {
        d_sequence.object() = bsl::move(value);
    }
    else {
        reset();
        new (d_sequence.buffer())
                EncoderTestDegenerateChoice1Sequence(bsl::move(value));
        d_selectionId = SELECTION_ID_SEQUENCE;
    }

    return d_sequence.object();
}
#endif

// ACCESSORS

bsl::ostream& EncoderTestDegenerateChoice1::print(
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


const char *EncoderTestDegenerateChoice1::selectionName() const
{
    switch (d_selectionId) {
      case SELECTION_ID_SEQUENCE:
        return SELECTION_INFO_ARRAY[SELECTION_INDEX_SEQUENCE].name();
      default:
        BSLS_ASSERT(SELECTION_ID_UNDEFINED == d_selectionId);
        return "(* UNDEFINED *)";
    }
}

                         // -------------------------
                         // class EncoderTestEmployee
                         // -------------------------

// CONSTANTS

const char EncoderTestEmployee::CLASS_NAME[] = "EncoderTestEmployee";

const bdlat_AttributeInfo EncoderTestEmployee::ATTRIBUTE_INFO_ARRAY[] = {
    {
        ATTRIBUTE_ID_NAME,
        "name",
        sizeof("name") - 1,
        "",
        bdlat_FormattingMode::e_TEXT
    },
    {
        ATTRIBUTE_ID_HOME_ADDRESS,
        "homeAddress",
        sizeof("homeAddress") - 1,
        "",
        bdlat_FormattingMode::e_DEFAULT
    },
    {
        ATTRIBUTE_ID_AGE,
        "age",
        sizeof("age") - 1,
        "",
        bdlat_FormattingMode::e_DEC
    }
};

// CLASS METHODS

const bdlat_AttributeInfo *EncoderTestEmployee::lookupAttributeInfo(
        const char *name,
        int         nameLength)
{
    for (int i = 0; i < 3; ++i) {
        const bdlat_AttributeInfo& attributeInfo =
                    EncoderTestEmployee::ATTRIBUTE_INFO_ARRAY[i];

        if (nameLength == attributeInfo.d_nameLength
        &&  0 == bsl::memcmp(attributeInfo.d_name_p, name, nameLength))
        {
            return &attributeInfo;
        }
    }

    return 0;
}

const bdlat_AttributeInfo *EncoderTestEmployee::lookupAttributeInfo(int id)
{
    switch (id) {
      case ATTRIBUTE_ID_NAME:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_NAME];
      case ATTRIBUTE_ID_HOME_ADDRESS:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_HOME_ADDRESS];
      case ATTRIBUTE_ID_AGE:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_AGE];
      default:
        return 0;
    }
}

// CREATORS

EncoderTestEmployee::EncoderTestEmployee(bslma::Allocator *basicAllocator)
: d_name(basicAllocator)
, d_homeAddress(basicAllocator)
, d_age()
{
}

EncoderTestEmployee::EncoderTestEmployee(const EncoderTestEmployee& original,
                                         bslma::Allocator *basicAllocator)
: d_name(original.d_name, basicAllocator)
, d_homeAddress(original.d_homeAddress, basicAllocator)
, d_age(original.d_age)
{
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
EncoderTestEmployee::EncoderTestEmployee(EncoderTestEmployee&& original) noexcept
: d_name(bsl::move(original.d_name))
, d_homeAddress(bsl::move(original.d_homeAddress))
, d_age(bsl::move(original.d_age))
{
}

EncoderTestEmployee::EncoderTestEmployee(EncoderTestEmployee&& original,
                                         bslma::Allocator *basicAllocator)
: d_name(bsl::move(original.d_name), basicAllocator)
, d_homeAddress(bsl::move(original.d_homeAddress), basicAllocator)
, d_age(bsl::move(original.d_age))
{
}
#endif

EncoderTestEmployee::~EncoderTestEmployee()
{
}

// MANIPULATORS

EncoderTestEmployee&
EncoderTestEmployee::operator=(const EncoderTestEmployee& rhs)
{
    if (this != &rhs) {
        d_name = rhs.d_name;
        d_homeAddress = rhs.d_homeAddress;
        d_age = rhs.d_age;
    }

    return *this;
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
EncoderTestEmployee&
EncoderTestEmployee::operator=(EncoderTestEmployee&& rhs)
{
    if (this != &rhs) {
        d_name = bsl::move(rhs.d_name);
        d_homeAddress = bsl::move(rhs.d_homeAddress);
        d_age = bsl::move(rhs.d_age);
    }

    return *this;
}
#endif

void EncoderTestEmployee::reset()
{
    bdlat_ValueTypeFunctions::reset(&d_name);
    bdlat_ValueTypeFunctions::reset(&d_homeAddress);
    bdlat_ValueTypeFunctions::reset(&d_age);
}

// ACCESSORS

bsl::ostream& EncoderTestEmployee::print(
        bsl::ostream& stream,
        int           level,
        int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute("name", d_name);
    printer.printAttribute("homeAddress", d_homeAddress);
    printer.printAttribute("age", d_age);
    printer.end();
    return stream;
}



                 // ------------------------------------------
                 // class EncoderTestSequenceWithAllCategories
                 // ------------------------------------------

// CONSTANTS

const char EncoderTestSequenceWithAllCategories::CLASS_NAME[] = "EncoderTestSequenceWithAllCategories";

const bdlat_AttributeInfo EncoderTestSequenceWithAllCategories::ATTRIBUTE_INFO_ARRAY[] = {
    {
        ATTRIBUTE_ID_CHAR_ARRAY,
        "charArray",
        sizeof("charArray") - 1,
        "",
        bdlat_FormattingMode::e_HEX
    },
    {
        ATTRIBUTE_ID_ARRAY,
        "array",
        sizeof("array") - 1,
        "",
        bdlat_FormattingMode::e_DEC
    },
    {
        ATTRIBUTE_ID_CHOICE,
        "choice",
        sizeof("choice") - 1,
        "",
        bdlat_FormattingMode::e_DEFAULT
    },
    {
        ATTRIBUTE_ID_CUSTOMIZED_TYPE,
        "customizedType",
        sizeof("customizedType") - 1,
        "",
        bdlat_FormattingMode::e_TEXT
    },
    {
        ATTRIBUTE_ID_ENUMERATION,
        "enumeration",
        sizeof("enumeration") - 1,
        "",
        bdlat_FormattingMode::e_DEFAULT
    },
    {
        ATTRIBUTE_ID_NULLABLE_VALUE,
        "nullableValue",
        sizeof("nullableValue") - 1,
        "",
        bdlat_FormattingMode::e_DEC
    },
    {
        ATTRIBUTE_ID_SEQUENCE,
        "sequence",
        sizeof("sequence") - 1,
        "",
        bdlat_FormattingMode::e_DEFAULT
    },
    {
        ATTRIBUTE_ID_SIMPLE,
        "simple",
        sizeof("simple") - 1,
        "",
        bdlat_FormattingMode::e_DEC
    }
};

// CLASS METHODS

const bdlat_AttributeInfo *EncoderTestSequenceWithAllCategories::lookupAttributeInfo(
        const char *name,
        int         nameLength)
{
    for (int i = 0; i < 8; ++i) {
        const bdlat_AttributeInfo& attributeInfo =
                    EncoderTestSequenceWithAllCategories::ATTRIBUTE_INFO_ARRAY[i];

        if (nameLength == attributeInfo.d_nameLength
        &&  0 == bsl::memcmp(attributeInfo.d_name_p, name, nameLength))
        {
            return &attributeInfo;
        }
    }

    return 0;
}

const bdlat_AttributeInfo *EncoderTestSequenceWithAllCategories::lookupAttributeInfo(int id)
{
    switch (id) {
      case ATTRIBUTE_ID_CHAR_ARRAY:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CHAR_ARRAY];
      case ATTRIBUTE_ID_ARRAY:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ARRAY];
      case ATTRIBUTE_ID_CHOICE:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CHOICE];
      case ATTRIBUTE_ID_CUSTOMIZED_TYPE:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CUSTOMIZED_TYPE];
      case ATTRIBUTE_ID_ENUMERATION:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ENUMERATION];
      case ATTRIBUTE_ID_NULLABLE_VALUE:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_NULLABLE_VALUE];
      case ATTRIBUTE_ID_SEQUENCE:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SEQUENCE];
      case ATTRIBUTE_ID_SIMPLE:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SIMPLE];
      default:
        return 0;
    }
}

// CREATORS

EncoderTestSequenceWithAllCategories::EncoderTestSequenceWithAllCategories(bslma::Allocator *basicAllocator)
: d_array(basicAllocator)
, d_charArray(basicAllocator)
, d_sequence()
, d_customizedType(basicAllocator)
, d_choice()
, d_simple()
, d_nullableValue()
, d_enumeration(static_cast<EncoderTestSequenceWithAllCategoriesEnumeration::Value>(0))
{
}

EncoderTestSequenceWithAllCategories::EncoderTestSequenceWithAllCategories(const EncoderTestSequenceWithAllCategories& original,
                                                                           bslma::Allocator *basicAllocator)
: d_array(original.d_array, basicAllocator)
, d_charArray(original.d_charArray, basicAllocator)
, d_sequence(original.d_sequence)
, d_customizedType(original.d_customizedType, basicAllocator)
, d_choice(original.d_choice)
, d_simple(original.d_simple)
, d_nullableValue(original.d_nullableValue)
, d_enumeration(original.d_enumeration)
{
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
EncoderTestSequenceWithAllCategories::EncoderTestSequenceWithAllCategories(EncoderTestSequenceWithAllCategories&& original) noexcept
: d_array(bsl::move(original.d_array))
, d_charArray(bsl::move(original.d_charArray))
, d_sequence(bsl::move(original.d_sequence))
, d_customizedType(bsl::move(original.d_customizedType))
, d_choice(bsl::move(original.d_choice))
, d_simple(bsl::move(original.d_simple))
, d_nullableValue(bsl::move(original.d_nullableValue))
, d_enumeration(bsl::move(original.d_enumeration))
{
}

EncoderTestSequenceWithAllCategories::EncoderTestSequenceWithAllCategories(EncoderTestSequenceWithAllCategories&& original,
                                                                           bslma::Allocator *basicAllocator)
: d_array(bsl::move(original.d_array), basicAllocator)
, d_charArray(bsl::move(original.d_charArray), basicAllocator)
, d_sequence(bsl::move(original.d_sequence))
, d_customizedType(bsl::move(original.d_customizedType), basicAllocator)
, d_choice(bsl::move(original.d_choice))
, d_simple(bsl::move(original.d_simple))
, d_nullableValue(bsl::move(original.d_nullableValue))
, d_enumeration(bsl::move(original.d_enumeration))
{
}
#endif

EncoderTestSequenceWithAllCategories::~EncoderTestSequenceWithAllCategories()
{
}

// MANIPULATORS

EncoderTestSequenceWithAllCategories&
EncoderTestSequenceWithAllCategories::operator=(const EncoderTestSequenceWithAllCategories& rhs)
{
    if (this != &rhs) {
        d_charArray = rhs.d_charArray;
        d_array = rhs.d_array;
        d_choice = rhs.d_choice;
        d_customizedType = rhs.d_customizedType;
        d_enumeration = rhs.d_enumeration;
        d_nullableValue = rhs.d_nullableValue;
        d_sequence = rhs.d_sequence;
        d_simple = rhs.d_simple;
    }

    return *this;
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
EncoderTestSequenceWithAllCategories&
EncoderTestSequenceWithAllCategories::operator=(EncoderTestSequenceWithAllCategories&& rhs)
{
    if (this != &rhs) {
        d_charArray = bsl::move(rhs.d_charArray);
        d_array = bsl::move(rhs.d_array);
        d_choice = bsl::move(rhs.d_choice);
        d_customizedType = bsl::move(rhs.d_customizedType);
        d_enumeration = bsl::move(rhs.d_enumeration);
        d_nullableValue = bsl::move(rhs.d_nullableValue);
        d_sequence = bsl::move(rhs.d_sequence);
        d_simple = bsl::move(rhs.d_simple);
    }

    return *this;
}
#endif

void EncoderTestSequenceWithAllCategories::reset()
{
    bdlat_ValueTypeFunctions::reset(&d_charArray);
    bdlat_ValueTypeFunctions::reset(&d_array);
    bdlat_ValueTypeFunctions::reset(&d_choice);
    bdlat_ValueTypeFunctions::reset(&d_customizedType);
    bdlat_ValueTypeFunctions::reset(&d_enumeration);
    bdlat_ValueTypeFunctions::reset(&d_nullableValue);
    bdlat_ValueTypeFunctions::reset(&d_sequence);
    bdlat_ValueTypeFunctions::reset(&d_simple);
}

// ACCESSORS

bsl::ostream& EncoderTestSequenceWithAllCategories::print(
        bsl::ostream& stream,
        int           level,
        int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    {
        bool multilineFlag = (0 <= level);
        bdlb::Print::indent(stream, level + 1, spacesPerLevel);
        stream << (multilineFlag ? "" : " ");
        stream << "charArray = [ ";
        bdlb::Print::singleLineHexDump(
            stream, d_charArray.begin(), d_charArray.end());
        stream << " ]" << (multilineFlag ? "\n" : "");
    }
    printer.printAttribute("array", d_array);
    printer.printAttribute("choice", d_choice);
    printer.printAttribute("customizedType", d_customizedType);
    printer.printAttribute("enumeration", d_enumeration);
    printer.printAttribute("nullableValue", d_nullableValue);
    printer.printAttribute("sequence", d_sequence);
    printer.printAttribute("simple", d_simple);
    printer.end();
    return stream;
}



                   // --------------------------------------
                   // class EncoderTestSequenceWithUntagged1
                   // --------------------------------------

// CONSTANTS

const char EncoderTestSequenceWithUntagged1::CLASS_NAME[] = "EncoderTestSequenceWithUntagged1";

const bdlat_AttributeInfo EncoderTestSequenceWithUntagged1::ATTRIBUTE_INFO_ARRAY[] = {
    {
        ATTRIBUTE_ID_SEQUENCE,
        "Sequence",
        sizeof("Sequence") - 1,
        "",
        bdlat_FormattingMode::e_DEFAULT
      | bdlat_FormattingMode::e_UNTAGGED
    }
};

// CLASS METHODS

const bdlat_AttributeInfo *EncoderTestSequenceWithUntagged1::lookupAttributeInfo(
        const char *name,
        int         nameLength)
{
    for (int i = 0; i < 1; ++i) {
        const bdlat_AttributeInfo& attributeInfo =
                    EncoderTestSequenceWithUntagged1::ATTRIBUTE_INFO_ARRAY[i];

        if (nameLength == attributeInfo.d_nameLength
        &&  0 == bsl::memcmp(attributeInfo.d_name_p, name, nameLength))
        {
            return &attributeInfo;
        }
    }

    return 0;
}

const bdlat_AttributeInfo *EncoderTestSequenceWithUntagged1::lookupAttributeInfo(int id)
{
    switch (id) {
      case ATTRIBUTE_ID_SEQUENCE:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SEQUENCE];
      default:
        return 0;
    }
}

// CREATORS

EncoderTestSequenceWithUntagged1::EncoderTestSequenceWithUntagged1()
: d_sequence()
{
}

EncoderTestSequenceWithUntagged1::EncoderTestSequenceWithUntagged1(const EncoderTestSequenceWithUntagged1& original)
: d_sequence(original.d_sequence)
{
}

EncoderTestSequenceWithUntagged1::~EncoderTestSequenceWithUntagged1()
{
}

// MANIPULATORS

EncoderTestSequenceWithUntagged1&
EncoderTestSequenceWithUntagged1::operator=(const EncoderTestSequenceWithUntagged1& rhs)
{
    if (this != &rhs) {
        d_sequence = rhs.d_sequence;
    }

    return *this;
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
EncoderTestSequenceWithUntagged1&
EncoderTestSequenceWithUntagged1::operator=(EncoderTestSequenceWithUntagged1&& rhs)
{
    if (this != &rhs) {
        d_sequence = bsl::move(rhs.d_sequence);
    }

    return *this;
}
#endif

void EncoderTestSequenceWithUntagged1::reset()
{
    bdlat_ValueTypeFunctions::reset(&d_sequence);
}

// ACCESSORS

bsl::ostream& EncoderTestSequenceWithUntagged1::print(
        bsl::ostream& stream,
        int           level,
        int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute("sequence", d_sequence);
    printer.end();
    return stream;
}



                  // ---------------------------------------
                  // class EncoderTestSequenceWithUntagged10
                  // ---------------------------------------

// CONSTANTS

const char EncoderTestSequenceWithUntagged10::CLASS_NAME[] = "EncoderTestSequenceWithUntagged10";

const bdlat_AttributeInfo EncoderTestSequenceWithUntagged10::ATTRIBUTE_INFO_ARRAY[] = {
    {
        ATTRIBUTE_ID_SEQUENCE,
        "Sequence",
        sizeof("Sequence") - 1,
        "",
        bdlat_FormattingMode::e_DEFAULT
      | bdlat_FormattingMode::e_UNTAGGED
    },
    {
        ATTRIBUTE_ID_ATTRIBUTE0,
        "attribute0",
        sizeof("attribute0") - 1,
        "",
        bdlat_FormattingMode::e_DEC
    },
    {
        ATTRIBUTE_ID_ATTRIBUTE1,
        "attribute1",
        sizeof("attribute1") - 1,
        "",
        bdlat_FormattingMode::e_DEC
    }
};

// CLASS METHODS

const bdlat_AttributeInfo *EncoderTestSequenceWithUntagged10::lookupAttributeInfo(
        const char *name,
        int         nameLength)
{
    for (int i = 0; i < 3; ++i) {
        const bdlat_AttributeInfo& attributeInfo =
                    EncoderTestSequenceWithUntagged10::ATTRIBUTE_INFO_ARRAY[i];

        if (nameLength == attributeInfo.d_nameLength
        &&  0 == bsl::memcmp(attributeInfo.d_name_p, name, nameLength))
        {
            return &attributeInfo;
        }
    }

    return 0;
}

const bdlat_AttributeInfo *EncoderTestSequenceWithUntagged10::lookupAttributeInfo(int id)
{
    switch (id) {
      case ATTRIBUTE_ID_SEQUENCE:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SEQUENCE];
      case ATTRIBUTE_ID_ATTRIBUTE0:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE0];
      case ATTRIBUTE_ID_ATTRIBUTE1:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE1];
      default:
        return 0;
    }
}

// CREATORS

EncoderTestSequenceWithUntagged10::EncoderTestSequenceWithUntagged10()
: d_sequence()
, d_attribute0()
, d_attribute1()
{
}

EncoderTestSequenceWithUntagged10::EncoderTestSequenceWithUntagged10(const EncoderTestSequenceWithUntagged10& original)
: d_sequence(original.d_sequence)
, d_attribute0(original.d_attribute0)
, d_attribute1(original.d_attribute1)
{
}

EncoderTestSequenceWithUntagged10::~EncoderTestSequenceWithUntagged10()
{
}

// MANIPULATORS

EncoderTestSequenceWithUntagged10&
EncoderTestSequenceWithUntagged10::operator=(const EncoderTestSequenceWithUntagged10& rhs)
{
    if (this != &rhs) {
        d_sequence = rhs.d_sequence;
        d_attribute0 = rhs.d_attribute0;
        d_attribute1 = rhs.d_attribute1;
    }

    return *this;
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
EncoderTestSequenceWithUntagged10&
EncoderTestSequenceWithUntagged10::operator=(EncoderTestSequenceWithUntagged10&& rhs)
{
    if (this != &rhs) {
        d_sequence = bsl::move(rhs.d_sequence);
        d_attribute0 = bsl::move(rhs.d_attribute0);
        d_attribute1 = bsl::move(rhs.d_attribute1);
    }

    return *this;
}
#endif

void EncoderTestSequenceWithUntagged10::reset()
{
    bdlat_ValueTypeFunctions::reset(&d_sequence);
    bdlat_ValueTypeFunctions::reset(&d_attribute0);
    bdlat_ValueTypeFunctions::reset(&d_attribute1);
}

// ACCESSORS

bsl::ostream& EncoderTestSequenceWithUntagged10::print(
        bsl::ostream& stream,
        int           level,
        int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute("sequence", d_sequence);
    printer.printAttribute("attribute0", d_attribute0);
    printer.printAttribute("attribute1", d_attribute1);
    printer.end();
    return stream;
}



                  // ---------------------------------------
                  // class EncoderTestSequenceWithUntagged11
                  // ---------------------------------------

// CONSTANTS

const char EncoderTestSequenceWithUntagged11::CLASS_NAME[] = "EncoderTestSequenceWithUntagged11";

const bdlat_AttributeInfo EncoderTestSequenceWithUntagged11::ATTRIBUTE_INFO_ARRAY[] = {
    {
        ATTRIBUTE_ID_ATTRIBUTE0,
        "attribute0",
        sizeof("attribute0") - 1,
        "",
        bdlat_FormattingMode::e_DEC
    },
    {
        ATTRIBUTE_ID_SEQUENCE,
        "Sequence",
        sizeof("Sequence") - 1,
        "",
        bdlat_FormattingMode::e_DEFAULT
      | bdlat_FormattingMode::e_UNTAGGED
    },
    {
        ATTRIBUTE_ID_SEQUENCE1,
        "Sequence-1",
        sizeof("Sequence-1") - 1,
        "",
        bdlat_FormattingMode::e_DEFAULT
      | bdlat_FormattingMode::e_UNTAGGED
    }
};

// CLASS METHODS

const bdlat_AttributeInfo *EncoderTestSequenceWithUntagged11::lookupAttributeInfo(
        const char *name,
        int         nameLength)
{
    for (int i = 0; i < 3; ++i) {
        const bdlat_AttributeInfo& attributeInfo =
                    EncoderTestSequenceWithUntagged11::ATTRIBUTE_INFO_ARRAY[i];

        if (nameLength == attributeInfo.d_nameLength
        &&  0 == bsl::memcmp(attributeInfo.d_name_p, name, nameLength))
        {
            return &attributeInfo;
        }
    }

    return 0;
}

const bdlat_AttributeInfo *EncoderTestSequenceWithUntagged11::lookupAttributeInfo(int id)
{
    switch (id) {
      case ATTRIBUTE_ID_ATTRIBUTE0:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE0];
      case ATTRIBUTE_ID_SEQUENCE:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SEQUENCE];
      case ATTRIBUTE_ID_SEQUENCE1:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SEQUENCE1];
      default:
        return 0;
    }
}

// CREATORS

EncoderTestSequenceWithUntagged11::EncoderTestSequenceWithUntagged11()
: d_sequence1()
, d_sequence()
, d_attribute0()
{
}

EncoderTestSequenceWithUntagged11::EncoderTestSequenceWithUntagged11(const EncoderTestSequenceWithUntagged11& original)
: d_sequence1(original.d_sequence1)
, d_sequence(original.d_sequence)
, d_attribute0(original.d_attribute0)
{
}

EncoderTestSequenceWithUntagged11::~EncoderTestSequenceWithUntagged11()
{
}

// MANIPULATORS

EncoderTestSequenceWithUntagged11&
EncoderTestSequenceWithUntagged11::operator=(const EncoderTestSequenceWithUntagged11& rhs)
{
    if (this != &rhs) {
        d_attribute0 = rhs.d_attribute0;
        d_sequence = rhs.d_sequence;
        d_sequence1 = rhs.d_sequence1;
    }

    return *this;
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
EncoderTestSequenceWithUntagged11&
EncoderTestSequenceWithUntagged11::operator=(EncoderTestSequenceWithUntagged11&& rhs)
{
    if (this != &rhs) {
        d_attribute0 = bsl::move(rhs.d_attribute0);
        d_sequence = bsl::move(rhs.d_sequence);
        d_sequence1 = bsl::move(rhs.d_sequence1);
    }

    return *this;
}
#endif

void EncoderTestSequenceWithUntagged11::reset()
{
    bdlat_ValueTypeFunctions::reset(&d_attribute0);
    bdlat_ValueTypeFunctions::reset(&d_sequence);
    bdlat_ValueTypeFunctions::reset(&d_sequence1);
}

// ACCESSORS

bsl::ostream& EncoderTestSequenceWithUntagged11::print(
        bsl::ostream& stream,
        int           level,
        int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute("attribute0", d_attribute0);
    printer.printAttribute("sequence", d_sequence);
    printer.printAttribute("sequence1", d_sequence1);
    printer.end();
    return stream;
}



                  // ---------------------------------------
                  // class EncoderTestSequenceWithUntagged12
                  // ---------------------------------------

// CONSTANTS

const char EncoderTestSequenceWithUntagged12::CLASS_NAME[] = "EncoderTestSequenceWithUntagged12";

const bdlat_AttributeInfo EncoderTestSequenceWithUntagged12::ATTRIBUTE_INFO_ARRAY[] = {
    {
        ATTRIBUTE_ID_ATTRIBUTE0,
        "attribute0",
        sizeof("attribute0") - 1,
        "",
        bdlat_FormattingMode::e_DEC
    },
    {
        ATTRIBUTE_ID_SEQUENCE,
        "Sequence",
        sizeof("Sequence") - 1,
        "",
        bdlat_FormattingMode::e_DEFAULT
      | bdlat_FormattingMode::e_UNTAGGED
    },
    {
        ATTRIBUTE_ID_ATTRIBUTE1,
        "attribute1",
        sizeof("attribute1") - 1,
        "",
        bdlat_FormattingMode::e_DEC
    }
};

// CLASS METHODS

const bdlat_AttributeInfo *EncoderTestSequenceWithUntagged12::lookupAttributeInfo(
        const char *name,
        int         nameLength)
{
    for (int i = 0; i < 3; ++i) {
        const bdlat_AttributeInfo& attributeInfo =
                    EncoderTestSequenceWithUntagged12::ATTRIBUTE_INFO_ARRAY[i];

        if (nameLength == attributeInfo.d_nameLength
        &&  0 == bsl::memcmp(attributeInfo.d_name_p, name, nameLength))
        {
            return &attributeInfo;
        }
    }

    return 0;
}

const bdlat_AttributeInfo *EncoderTestSequenceWithUntagged12::lookupAttributeInfo(int id)
{
    switch (id) {
      case ATTRIBUTE_ID_ATTRIBUTE0:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE0];
      case ATTRIBUTE_ID_SEQUENCE:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SEQUENCE];
      case ATTRIBUTE_ID_ATTRIBUTE1:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE1];
      default:
        return 0;
    }
}

// CREATORS

EncoderTestSequenceWithUntagged12::EncoderTestSequenceWithUntagged12()
: d_sequence()
, d_attribute0()
, d_attribute1()
{
}

EncoderTestSequenceWithUntagged12::EncoderTestSequenceWithUntagged12(const EncoderTestSequenceWithUntagged12& original)
: d_sequence(original.d_sequence)
, d_attribute0(original.d_attribute0)
, d_attribute1(original.d_attribute1)
{
}

EncoderTestSequenceWithUntagged12::~EncoderTestSequenceWithUntagged12()
{
}

// MANIPULATORS

EncoderTestSequenceWithUntagged12&
EncoderTestSequenceWithUntagged12::operator=(const EncoderTestSequenceWithUntagged12& rhs)
{
    if (this != &rhs) {
        d_attribute0 = rhs.d_attribute0;
        d_sequence = rhs.d_sequence;
        d_attribute1 = rhs.d_attribute1;
    }

    return *this;
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
EncoderTestSequenceWithUntagged12&
EncoderTestSequenceWithUntagged12::operator=(EncoderTestSequenceWithUntagged12&& rhs)
{
    if (this != &rhs) {
        d_attribute0 = bsl::move(rhs.d_attribute0);
        d_sequence = bsl::move(rhs.d_sequence);
        d_attribute1 = bsl::move(rhs.d_attribute1);
    }

    return *this;
}
#endif

void EncoderTestSequenceWithUntagged12::reset()
{
    bdlat_ValueTypeFunctions::reset(&d_attribute0);
    bdlat_ValueTypeFunctions::reset(&d_sequence);
    bdlat_ValueTypeFunctions::reset(&d_attribute1);
}

// ACCESSORS

bsl::ostream& EncoderTestSequenceWithUntagged12::print(
        bsl::ostream& stream,
        int           level,
        int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute("attribute0", d_attribute0);
    printer.printAttribute("sequence", d_sequence);
    printer.printAttribute("attribute1", d_attribute1);
    printer.end();
    return stream;
}



                  // ---------------------------------------
                  // class EncoderTestSequenceWithUntagged13
                  // ---------------------------------------

// CONSTANTS

const char EncoderTestSequenceWithUntagged13::CLASS_NAME[] = "EncoderTestSequenceWithUntagged13";

const bdlat_AttributeInfo EncoderTestSequenceWithUntagged13::ATTRIBUTE_INFO_ARRAY[] = {
    {
        ATTRIBUTE_ID_ATTRIBUTE0,
        "attribute0",
        sizeof("attribute0") - 1,
        "",
        bdlat_FormattingMode::e_DEC
    },
    {
        ATTRIBUTE_ID_ATTRIBUTE1,
        "attribute1",
        sizeof("attribute1") - 1,
        "",
        bdlat_FormattingMode::e_DEC
    },
    {
        ATTRIBUTE_ID_SEQUENCE,
        "Sequence",
        sizeof("Sequence") - 1,
        "",
        bdlat_FormattingMode::e_DEFAULT
      | bdlat_FormattingMode::e_UNTAGGED
    }
};

// CLASS METHODS

const bdlat_AttributeInfo *EncoderTestSequenceWithUntagged13::lookupAttributeInfo(
        const char *name,
        int         nameLength)
{
    for (int i = 0; i < 3; ++i) {
        const bdlat_AttributeInfo& attributeInfo =
                    EncoderTestSequenceWithUntagged13::ATTRIBUTE_INFO_ARRAY[i];

        if (nameLength == attributeInfo.d_nameLength
        &&  0 == bsl::memcmp(attributeInfo.d_name_p, name, nameLength))
        {
            return &attributeInfo;
        }
    }

    return 0;
}

const bdlat_AttributeInfo *EncoderTestSequenceWithUntagged13::lookupAttributeInfo(int id)
{
    switch (id) {
      case ATTRIBUTE_ID_ATTRIBUTE0:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE0];
      case ATTRIBUTE_ID_ATTRIBUTE1:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE1];
      case ATTRIBUTE_ID_SEQUENCE:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SEQUENCE];
      default:
        return 0;
    }
}

// CREATORS

EncoderTestSequenceWithUntagged13::EncoderTestSequenceWithUntagged13()
: d_sequence()
, d_attribute0()
, d_attribute1()
{
}

EncoderTestSequenceWithUntagged13::EncoderTestSequenceWithUntagged13(const EncoderTestSequenceWithUntagged13& original)
: d_sequence(original.d_sequence)
, d_attribute0(original.d_attribute0)
, d_attribute1(original.d_attribute1)
{
}

EncoderTestSequenceWithUntagged13::~EncoderTestSequenceWithUntagged13()
{
}

// MANIPULATORS

EncoderTestSequenceWithUntagged13&
EncoderTestSequenceWithUntagged13::operator=(const EncoderTestSequenceWithUntagged13& rhs)
{
    if (this != &rhs) {
        d_attribute0 = rhs.d_attribute0;
        d_attribute1 = rhs.d_attribute1;
        d_sequence = rhs.d_sequence;
    }

    return *this;
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
EncoderTestSequenceWithUntagged13&
EncoderTestSequenceWithUntagged13::operator=(EncoderTestSequenceWithUntagged13&& rhs)
{
    if (this != &rhs) {
        d_attribute0 = bsl::move(rhs.d_attribute0);
        d_attribute1 = bsl::move(rhs.d_attribute1);
        d_sequence = bsl::move(rhs.d_sequence);
    }

    return *this;
}
#endif

void EncoderTestSequenceWithUntagged13::reset()
{
    bdlat_ValueTypeFunctions::reset(&d_attribute0);
    bdlat_ValueTypeFunctions::reset(&d_attribute1);
    bdlat_ValueTypeFunctions::reset(&d_sequence);
}

// ACCESSORS

bsl::ostream& EncoderTestSequenceWithUntagged13::print(
        bsl::ostream& stream,
        int           level,
        int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute("attribute0", d_attribute0);
    printer.printAttribute("attribute1", d_attribute1);
    printer.printAttribute("sequence", d_sequence);
    printer.end();
    return stream;
}



                   // --------------------------------------
                   // class EncoderTestSequenceWithUntagged3
                   // --------------------------------------

// CONSTANTS

const char EncoderTestSequenceWithUntagged3::CLASS_NAME[] = "EncoderTestSequenceWithUntagged3";

const bdlat_AttributeInfo EncoderTestSequenceWithUntagged3::ATTRIBUTE_INFO_ARRAY[] = {
    {
        ATTRIBUTE_ID_SEQUENCE,
        "Sequence",
        sizeof("Sequence") - 1,
        "",
        bdlat_FormattingMode::e_DEFAULT
      | bdlat_FormattingMode::e_UNTAGGED
    },
    {
        ATTRIBUTE_ID_SEQUENCE1,
        "Sequence-1",
        sizeof("Sequence-1") - 1,
        "",
        bdlat_FormattingMode::e_DEFAULT
      | bdlat_FormattingMode::e_UNTAGGED
    }
};

// CLASS METHODS

const bdlat_AttributeInfo *EncoderTestSequenceWithUntagged3::lookupAttributeInfo(
        const char *name,
        int         nameLength)
{
    for (int i = 0; i < 2; ++i) {
        const bdlat_AttributeInfo& attributeInfo =
                    EncoderTestSequenceWithUntagged3::ATTRIBUTE_INFO_ARRAY[i];

        if (nameLength == attributeInfo.d_nameLength
        &&  0 == bsl::memcmp(attributeInfo.d_name_p, name, nameLength))
        {
            return &attributeInfo;
        }
    }

    return 0;
}

const bdlat_AttributeInfo *EncoderTestSequenceWithUntagged3::lookupAttributeInfo(int id)
{
    switch (id) {
      case ATTRIBUTE_ID_SEQUENCE:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SEQUENCE];
      case ATTRIBUTE_ID_SEQUENCE1:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SEQUENCE1];
      default:
        return 0;
    }
}

// CREATORS

EncoderTestSequenceWithUntagged3::EncoderTestSequenceWithUntagged3()
: d_sequence1()
, d_sequence()
{
}

EncoderTestSequenceWithUntagged3::EncoderTestSequenceWithUntagged3(const EncoderTestSequenceWithUntagged3& original)
: d_sequence1(original.d_sequence1)
, d_sequence(original.d_sequence)
{
}

EncoderTestSequenceWithUntagged3::~EncoderTestSequenceWithUntagged3()
{
}

// MANIPULATORS

EncoderTestSequenceWithUntagged3&
EncoderTestSequenceWithUntagged3::operator=(const EncoderTestSequenceWithUntagged3& rhs)
{
    if (this != &rhs) {
        d_sequence = rhs.d_sequence;
        d_sequence1 = rhs.d_sequence1;
    }

    return *this;
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
EncoderTestSequenceWithUntagged3&
EncoderTestSequenceWithUntagged3::operator=(EncoderTestSequenceWithUntagged3&& rhs)
{
    if (this != &rhs) {
        d_sequence = bsl::move(rhs.d_sequence);
        d_sequence1 = bsl::move(rhs.d_sequence1);
    }

    return *this;
}
#endif

void EncoderTestSequenceWithUntagged3::reset()
{
    bdlat_ValueTypeFunctions::reset(&d_sequence);
    bdlat_ValueTypeFunctions::reset(&d_sequence1);
}

// ACCESSORS

bsl::ostream& EncoderTestSequenceWithUntagged3::print(
        bsl::ostream& stream,
        int           level,
        int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute("sequence", d_sequence);
    printer.printAttribute("sequence1", d_sequence1);
    printer.end();
    return stream;
}



                   // --------------------------------------
                   // class EncoderTestSequenceWithUntagged4
                   // --------------------------------------

// CONSTANTS

const char EncoderTestSequenceWithUntagged4::CLASS_NAME[] = "EncoderTestSequenceWithUntagged4";

const bdlat_AttributeInfo EncoderTestSequenceWithUntagged4::ATTRIBUTE_INFO_ARRAY[] = {
    {
        ATTRIBUTE_ID_SEQUENCE,
        "Sequence",
        sizeof("Sequence") - 1,
        "",
        bdlat_FormattingMode::e_DEFAULT
      | bdlat_FormattingMode::e_UNTAGGED
    },
    {
        ATTRIBUTE_ID_ATTRIBUTE0,
        "attribute0",
        sizeof("attribute0") - 1,
        "",
        bdlat_FormattingMode::e_DEC
    }
};

// CLASS METHODS

const bdlat_AttributeInfo *EncoderTestSequenceWithUntagged4::lookupAttributeInfo(
        const char *name,
        int         nameLength)
{
    for (int i = 0; i < 2; ++i) {
        const bdlat_AttributeInfo& attributeInfo =
                    EncoderTestSequenceWithUntagged4::ATTRIBUTE_INFO_ARRAY[i];

        if (nameLength == attributeInfo.d_nameLength
        &&  0 == bsl::memcmp(attributeInfo.d_name_p, name, nameLength))
        {
            return &attributeInfo;
        }
    }

    return 0;
}

const bdlat_AttributeInfo *EncoderTestSequenceWithUntagged4::lookupAttributeInfo(int id)
{
    switch (id) {
      case ATTRIBUTE_ID_SEQUENCE:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SEQUENCE];
      case ATTRIBUTE_ID_ATTRIBUTE0:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE0];
      default:
        return 0;
    }
}

// CREATORS

EncoderTestSequenceWithUntagged4::EncoderTestSequenceWithUntagged4()
: d_sequence()
, d_attribute0()
{
}

EncoderTestSequenceWithUntagged4::EncoderTestSequenceWithUntagged4(const EncoderTestSequenceWithUntagged4& original)
: d_sequence(original.d_sequence)
, d_attribute0(original.d_attribute0)
{
}

EncoderTestSequenceWithUntagged4::~EncoderTestSequenceWithUntagged4()
{
}

// MANIPULATORS

EncoderTestSequenceWithUntagged4&
EncoderTestSequenceWithUntagged4::operator=(const EncoderTestSequenceWithUntagged4& rhs)
{
    if (this != &rhs) {
        d_sequence = rhs.d_sequence;
        d_attribute0 = rhs.d_attribute0;
    }

    return *this;
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
EncoderTestSequenceWithUntagged4&
EncoderTestSequenceWithUntagged4::operator=(EncoderTestSequenceWithUntagged4&& rhs)
{
    if (this != &rhs) {
        d_sequence = bsl::move(rhs.d_sequence);
        d_attribute0 = bsl::move(rhs.d_attribute0);
    }

    return *this;
}
#endif

void EncoderTestSequenceWithUntagged4::reset()
{
    bdlat_ValueTypeFunctions::reset(&d_sequence);
    bdlat_ValueTypeFunctions::reset(&d_attribute0);
}

// ACCESSORS

bsl::ostream& EncoderTestSequenceWithUntagged4::print(
        bsl::ostream& stream,
        int           level,
        int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute("sequence", d_sequence);
    printer.printAttribute("attribute0", d_attribute0);
    printer.end();
    return stream;
}



                   // --------------------------------------
                   // class EncoderTestSequenceWithUntagged5
                   // --------------------------------------

// CONSTANTS

const char EncoderTestSequenceWithUntagged5::CLASS_NAME[] = "EncoderTestSequenceWithUntagged5";

const bdlat_AttributeInfo EncoderTestSequenceWithUntagged5::ATTRIBUTE_INFO_ARRAY[] = {
    {
        ATTRIBUTE_ID_ATTRIBUTE0,
        "attribute0",
        sizeof("attribute0") - 1,
        "",
        bdlat_FormattingMode::e_DEC
    },
    {
        ATTRIBUTE_ID_SEQUENCE,
        "Sequence",
        sizeof("Sequence") - 1,
        "",
        bdlat_FormattingMode::e_DEFAULT
      | bdlat_FormattingMode::e_UNTAGGED
    }
};

// CLASS METHODS

const bdlat_AttributeInfo *EncoderTestSequenceWithUntagged5::lookupAttributeInfo(
        const char *name,
        int         nameLength)
{
    for (int i = 0; i < 2; ++i) {
        const bdlat_AttributeInfo& attributeInfo =
                    EncoderTestSequenceWithUntagged5::ATTRIBUTE_INFO_ARRAY[i];

        if (nameLength == attributeInfo.d_nameLength
        &&  0 == bsl::memcmp(attributeInfo.d_name_p, name, nameLength))
        {
            return &attributeInfo;
        }
    }

    return 0;
}

const bdlat_AttributeInfo *EncoderTestSequenceWithUntagged5::lookupAttributeInfo(int id)
{
    switch (id) {
      case ATTRIBUTE_ID_ATTRIBUTE0:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE0];
      case ATTRIBUTE_ID_SEQUENCE:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SEQUENCE];
      default:
        return 0;
    }
}

// CREATORS

EncoderTestSequenceWithUntagged5::EncoderTestSequenceWithUntagged5()
: d_sequence()
, d_attribute0()
{
}

EncoderTestSequenceWithUntagged5::EncoderTestSequenceWithUntagged5(const EncoderTestSequenceWithUntagged5& original)
: d_sequence(original.d_sequence)
, d_attribute0(original.d_attribute0)
{
}

EncoderTestSequenceWithUntagged5::~EncoderTestSequenceWithUntagged5()
{
}

// MANIPULATORS

EncoderTestSequenceWithUntagged5&
EncoderTestSequenceWithUntagged5::operator=(const EncoderTestSequenceWithUntagged5& rhs)
{
    if (this != &rhs) {
        d_attribute0 = rhs.d_attribute0;
        d_sequence = rhs.d_sequence;
    }

    return *this;
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
EncoderTestSequenceWithUntagged5&
EncoderTestSequenceWithUntagged5::operator=(EncoderTestSequenceWithUntagged5&& rhs)
{
    if (this != &rhs) {
        d_attribute0 = bsl::move(rhs.d_attribute0);
        d_sequence = bsl::move(rhs.d_sequence);
    }

    return *this;
}
#endif

void EncoderTestSequenceWithUntagged5::reset()
{
    bdlat_ValueTypeFunctions::reset(&d_attribute0);
    bdlat_ValueTypeFunctions::reset(&d_sequence);
}

// ACCESSORS

bsl::ostream& EncoderTestSequenceWithUntagged5::print(
        bsl::ostream& stream,
        int           level,
        int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute("attribute0", d_attribute0);
    printer.printAttribute("sequence", d_sequence);
    printer.end();
    return stream;
}



                   // --------------------------------------
                   // class EncoderTestSequenceWithUntagged7
                   // --------------------------------------

// CONSTANTS

const char EncoderTestSequenceWithUntagged7::CLASS_NAME[] = "EncoderTestSequenceWithUntagged7";

const bdlat_AttributeInfo EncoderTestSequenceWithUntagged7::ATTRIBUTE_INFO_ARRAY[] = {
    {
        ATTRIBUTE_ID_SEQUENCE,
        "Sequence",
        sizeof("Sequence") - 1,
        "",
        bdlat_FormattingMode::e_DEFAULT
      | bdlat_FormattingMode::e_UNTAGGED
    },
    {
        ATTRIBUTE_ID_SEQUENCE1,
        "Sequence-1",
        sizeof("Sequence-1") - 1,
        "",
        bdlat_FormattingMode::e_DEFAULT
      | bdlat_FormattingMode::e_UNTAGGED
    },
    {
        ATTRIBUTE_ID_SEQUENCE2,
        "Sequence-2",
        sizeof("Sequence-2") - 1,
        "",
        bdlat_FormattingMode::e_DEFAULT
      | bdlat_FormattingMode::e_UNTAGGED
    }
};

// CLASS METHODS

const bdlat_AttributeInfo *EncoderTestSequenceWithUntagged7::lookupAttributeInfo(
        const char *name,
        int         nameLength)
{
    for (int i = 0; i < 3; ++i) {
        const bdlat_AttributeInfo& attributeInfo =
                    EncoderTestSequenceWithUntagged7::ATTRIBUTE_INFO_ARRAY[i];

        if (nameLength == attributeInfo.d_nameLength
        &&  0 == bsl::memcmp(attributeInfo.d_name_p, name, nameLength))
        {
            return &attributeInfo;
        }
    }

    return 0;
}

const bdlat_AttributeInfo *EncoderTestSequenceWithUntagged7::lookupAttributeInfo(int id)
{
    switch (id) {
      case ATTRIBUTE_ID_SEQUENCE:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SEQUENCE];
      case ATTRIBUTE_ID_SEQUENCE1:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SEQUENCE1];
      case ATTRIBUTE_ID_SEQUENCE2:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SEQUENCE2];
      default:
        return 0;
    }
}

// CREATORS

EncoderTestSequenceWithUntagged7::EncoderTestSequenceWithUntagged7()
: d_sequence2()
, d_sequence1()
, d_sequence()
{
}

EncoderTestSequenceWithUntagged7::EncoderTestSequenceWithUntagged7(const EncoderTestSequenceWithUntagged7& original)
: d_sequence2(original.d_sequence2)
, d_sequence1(original.d_sequence1)
, d_sequence(original.d_sequence)
{
}

EncoderTestSequenceWithUntagged7::~EncoderTestSequenceWithUntagged7()
{
}

// MANIPULATORS

EncoderTestSequenceWithUntagged7&
EncoderTestSequenceWithUntagged7::operator=(const EncoderTestSequenceWithUntagged7& rhs)
{
    if (this != &rhs) {
        d_sequence = rhs.d_sequence;
        d_sequence1 = rhs.d_sequence1;
        d_sequence2 = rhs.d_sequence2;
    }

    return *this;
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
EncoderTestSequenceWithUntagged7&
EncoderTestSequenceWithUntagged7::operator=(EncoderTestSequenceWithUntagged7&& rhs)
{
    if (this != &rhs) {
        d_sequence = bsl::move(rhs.d_sequence);
        d_sequence1 = bsl::move(rhs.d_sequence1);
        d_sequence2 = bsl::move(rhs.d_sequence2);
    }

    return *this;
}
#endif

void EncoderTestSequenceWithUntagged7::reset()
{
    bdlat_ValueTypeFunctions::reset(&d_sequence);
    bdlat_ValueTypeFunctions::reset(&d_sequence1);
    bdlat_ValueTypeFunctions::reset(&d_sequence2);
}

// ACCESSORS

bsl::ostream& EncoderTestSequenceWithUntagged7::print(
        bsl::ostream& stream,
        int           level,
        int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute("sequence", d_sequence);
    printer.printAttribute("sequence1", d_sequence1);
    printer.printAttribute("sequence2", d_sequence2);
    printer.end();
    return stream;
}



                   // --------------------------------------
                   // class EncoderTestSequenceWithUntagged8
                   // --------------------------------------

// CONSTANTS

const char EncoderTestSequenceWithUntagged8::CLASS_NAME[] = "EncoderTestSequenceWithUntagged8";

const bdlat_AttributeInfo EncoderTestSequenceWithUntagged8::ATTRIBUTE_INFO_ARRAY[] = {
    {
        ATTRIBUTE_ID_SEQUENCE,
        "Sequence",
        sizeof("Sequence") - 1,
        "",
        bdlat_FormattingMode::e_DEFAULT
      | bdlat_FormattingMode::e_UNTAGGED
    },
    {
        ATTRIBUTE_ID_SEQUENCE1,
        "Sequence-1",
        sizeof("Sequence-1") - 1,
        "",
        bdlat_FormattingMode::e_DEFAULT
      | bdlat_FormattingMode::e_UNTAGGED
    },
    {
        ATTRIBUTE_ID_ATTRIBUTE0,
        "attribute0",
        sizeof("attribute0") - 1,
        "",
        bdlat_FormattingMode::e_DEC
    }
};

// CLASS METHODS

const bdlat_AttributeInfo *EncoderTestSequenceWithUntagged8::lookupAttributeInfo(
        const char *name,
        int         nameLength)
{
    for (int i = 0; i < 3; ++i) {
        const bdlat_AttributeInfo& attributeInfo =
                    EncoderTestSequenceWithUntagged8::ATTRIBUTE_INFO_ARRAY[i];

        if (nameLength == attributeInfo.d_nameLength
        &&  0 == bsl::memcmp(attributeInfo.d_name_p, name, nameLength))
        {
            return &attributeInfo;
        }
    }

    return 0;
}

const bdlat_AttributeInfo *EncoderTestSequenceWithUntagged8::lookupAttributeInfo(int id)
{
    switch (id) {
      case ATTRIBUTE_ID_SEQUENCE:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SEQUENCE];
      case ATTRIBUTE_ID_SEQUENCE1:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SEQUENCE1];
      case ATTRIBUTE_ID_ATTRIBUTE0:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE0];
      default:
        return 0;
    }
}

// CREATORS

EncoderTestSequenceWithUntagged8::EncoderTestSequenceWithUntagged8()
: d_sequence1()
, d_sequence()
, d_attribute0()
{
}

EncoderTestSequenceWithUntagged8::EncoderTestSequenceWithUntagged8(const EncoderTestSequenceWithUntagged8& original)
: d_sequence1(original.d_sequence1)
, d_sequence(original.d_sequence)
, d_attribute0(original.d_attribute0)
{
}

EncoderTestSequenceWithUntagged8::~EncoderTestSequenceWithUntagged8()
{
}

// MANIPULATORS

EncoderTestSequenceWithUntagged8&
EncoderTestSequenceWithUntagged8::operator=(const EncoderTestSequenceWithUntagged8& rhs)
{
    if (this != &rhs) {
        d_sequence = rhs.d_sequence;
        d_sequence1 = rhs.d_sequence1;
        d_attribute0 = rhs.d_attribute0;
    }

    return *this;
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
EncoderTestSequenceWithUntagged8&
EncoderTestSequenceWithUntagged8::operator=(EncoderTestSequenceWithUntagged8&& rhs)
{
    if (this != &rhs) {
        d_sequence = bsl::move(rhs.d_sequence);
        d_sequence1 = bsl::move(rhs.d_sequence1);
        d_attribute0 = bsl::move(rhs.d_attribute0);
    }

    return *this;
}
#endif

void EncoderTestSequenceWithUntagged8::reset()
{
    bdlat_ValueTypeFunctions::reset(&d_sequence);
    bdlat_ValueTypeFunctions::reset(&d_sequence1);
    bdlat_ValueTypeFunctions::reset(&d_attribute0);
}

// ACCESSORS

bsl::ostream& EncoderTestSequenceWithUntagged8::print(
        bsl::ostream& stream,
        int           level,
        int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute("sequence", d_sequence);
    printer.printAttribute("sequence1", d_sequence1);
    printer.printAttribute("attribute0", d_attribute0);
    printer.end();
    return stream;
}



                   // --------------------------------------
                   // class EncoderTestSequenceWithUntagged9
                   // --------------------------------------

// CONSTANTS

const char EncoderTestSequenceWithUntagged9::CLASS_NAME[] = "EncoderTestSequenceWithUntagged9";

const bdlat_AttributeInfo EncoderTestSequenceWithUntagged9::ATTRIBUTE_INFO_ARRAY[] = {
    {
        ATTRIBUTE_ID_SEQUENCE,
        "Sequence",
        sizeof("Sequence") - 1,
        "",
        bdlat_FormattingMode::e_DEFAULT
      | bdlat_FormattingMode::e_UNTAGGED
    },
    {
        ATTRIBUTE_ID_ATTRIBUTE0,
        "attribute0",
        sizeof("attribute0") - 1,
        "",
        bdlat_FormattingMode::e_DEC
    },
    {
        ATTRIBUTE_ID_SEQUENCE1,
        "Sequence-1",
        sizeof("Sequence-1") - 1,
        "",
        bdlat_FormattingMode::e_DEFAULT
      | bdlat_FormattingMode::e_UNTAGGED
    }
};

// CLASS METHODS

const bdlat_AttributeInfo *EncoderTestSequenceWithUntagged9::lookupAttributeInfo(
        const char *name,
        int         nameLength)
{
    for (int i = 0; i < 3; ++i) {
        const bdlat_AttributeInfo& attributeInfo =
                    EncoderTestSequenceWithUntagged9::ATTRIBUTE_INFO_ARRAY[i];

        if (nameLength == attributeInfo.d_nameLength
        &&  0 == bsl::memcmp(attributeInfo.d_name_p, name, nameLength))
        {
            return &attributeInfo;
        }
    }

    return 0;
}

const bdlat_AttributeInfo *EncoderTestSequenceWithUntagged9::lookupAttributeInfo(int id)
{
    switch (id) {
      case ATTRIBUTE_ID_SEQUENCE:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SEQUENCE];
      case ATTRIBUTE_ID_ATTRIBUTE0:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE0];
      case ATTRIBUTE_ID_SEQUENCE1:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SEQUENCE1];
      default:
        return 0;
    }
}

// CREATORS

EncoderTestSequenceWithUntagged9::EncoderTestSequenceWithUntagged9()
: d_sequence1()
, d_sequence()
, d_attribute0()
{
}

EncoderTestSequenceWithUntagged9::EncoderTestSequenceWithUntagged9(const EncoderTestSequenceWithUntagged9& original)
: d_sequence1(original.d_sequence1)
, d_sequence(original.d_sequence)
, d_attribute0(original.d_attribute0)
{
}

EncoderTestSequenceWithUntagged9::~EncoderTestSequenceWithUntagged9()
{
}

// MANIPULATORS

EncoderTestSequenceWithUntagged9&
EncoderTestSequenceWithUntagged9::operator=(const EncoderTestSequenceWithUntagged9& rhs)
{
    if (this != &rhs) {
        d_sequence = rhs.d_sequence;
        d_attribute0 = rhs.d_attribute0;
        d_sequence1 = rhs.d_sequence1;
    }

    return *this;
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
EncoderTestSequenceWithUntagged9&
EncoderTestSequenceWithUntagged9::operator=(EncoderTestSequenceWithUntagged9&& rhs)
{
    if (this != &rhs) {
        d_sequence = bsl::move(rhs.d_sequence);
        d_attribute0 = bsl::move(rhs.d_attribute0);
        d_sequence1 = bsl::move(rhs.d_sequence1);
    }

    return *this;
}
#endif

void EncoderTestSequenceWithUntagged9::reset()
{
    bdlat_ValueTypeFunctions::reset(&d_sequence);
    bdlat_ValueTypeFunctions::reset(&d_attribute0);
    bdlat_ValueTypeFunctions::reset(&d_sequence1);
}

// ACCESSORS

bsl::ostream& EncoderTestSequenceWithUntagged9::print(
        bsl::ostream& stream,
        int           level,
        int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute("sequence", d_sequence);
    printer.printAttribute("attribute0", d_attribute0);
    printer.printAttribute("sequence1", d_sequence1);
    printer.end();
    return stream;
}


}  // close package namespace
}  // close enterprise namespace

// GENERATED BY BLP_BAS_CODEGEN_VERSION
// USING bas_codegen.pl --mode msg --noAggregateConversion --noExternalization --msgComponent=encoder_testtypes --package=baljsn baljsn_encoder_testtypes.xsd
// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright 2020 Bloomberg Finance L.P. All rights reserved.
//      Property of Bloomberg Finance L.P. (BFLP)
//      This software is made available solely pursuant to the
//      terms of a BFLP license agreement which governs its use.
// ------------------------------- END-OF-FILE --------------------------------
