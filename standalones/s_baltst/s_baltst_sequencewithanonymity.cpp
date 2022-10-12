// s_baltst_sequencewithanonymity.cpp    *DO NOT EDIT*     @generated -*-C++-*-

#include <bsls_ident.h>
BSLS_IDENT_RCSID(s_baltst_sequencewithanonymity_cpp, "$Id$ $CSID$")

#include <s_baltst_sequencewithanonymity.h>

#include <bdlat_formattingmode.h>
#include <bdlat_valuetypefunctions.h>
#include <bdlb_print.h>
#include <bdlb_printmethods.h>
#include <bdlb_string.h>

#include <bdlb_nullablevalue.h>
#include <bsl_string.h>
#include <s_baltst_customint.h>
#include <s_baltst_customstring.h>
#include <s_baltst_ratsnest.h>
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
            s_baltst::Sequence6(
                original.d_selection1.object(), d_allocator_p);
      } break;
      case SELECTION_ID_SELECTION2: {
        new (d_selection2.buffer())
            unsigned char(original.d_selection2.object());
      } break;
      case SELECTION_ID_SELECTION3: {
        new (d_selection3.buffer())
            s_baltst::CustomString(
                original.d_selection3.object(), d_allocator_p);
      } break;
      case SELECTION_ID_SELECTION4: {
        new (d_selection4.buffer())
            s_baltst::CustomInt(original.d_selection4.object());
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
            s_baltst::Sequence6(
                bsl::move(original.d_selection1.object()), d_allocator_p);
      } break;
      case SELECTION_ID_SELECTION2: {
        new (d_selection2.buffer())
            unsigned char(bsl::move(original.d_selection2.object()));
      } break;
      case SELECTION_ID_SELECTION3: {
        new (d_selection3.buffer())
            s_baltst::CustomString(
                bsl::move(original.d_selection3.object()), d_allocator_p);
      } break;
      case SELECTION_ID_SELECTION4: {
        new (d_selection4.buffer())
            s_baltst::CustomInt(bsl::move(original.d_selection4.object()));
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
            s_baltst::Sequence6(
                bsl::move(original.d_selection1.object()), d_allocator_p);
      } break;
      case SELECTION_ID_SELECTION2: {
        new (d_selection2.buffer())
            unsigned char(bsl::move(original.d_selection2.object()));
      } break;
      case SELECTION_ID_SELECTION3: {
        new (d_selection3.buffer())
            s_baltst::CustomString(
                bsl::move(original.d_selection3.object()), d_allocator_p);
      } break;
      case SELECTION_ID_SELECTION4: {
        new (d_selection4.buffer())
            s_baltst::CustomInt(bsl::move(original.d_selection4.object()));
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
        typedef s_baltst::Sequence6 Type;
        d_selection1.object().~Type();
      } break;
      case SELECTION_ID_SELECTION2: {
        // no destruction required
      } break;
      case SELECTION_ID_SELECTION3: {
        typedef s_baltst::CustomString Type;
        d_selection3.object().~Type();
      } break;
      case SELECTION_ID_SELECTION4: {
        typedef s_baltst::CustomInt Type;
        d_selection4.object().~Type();
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

s_baltst::Sequence6& SequenceWithAnonymityChoice::makeSelection1()
{
    if (SELECTION_ID_SELECTION1 == d_selectionId) {
        bdlat_ValueTypeFunctions::reset(&d_selection1.object());
    }
    else {
        reset();
        new (d_selection1.buffer())
                s_baltst::Sequence6(d_allocator_p);
        d_selectionId = SELECTION_ID_SELECTION1;
    }

    return d_selection1.object();
}

s_baltst::Sequence6& SequenceWithAnonymityChoice::makeSelection1(const s_baltst::Sequence6& value)
{
    if (SELECTION_ID_SELECTION1 == d_selectionId) {
        d_selection1.object() = value;
    }
    else {
        reset();
        new (d_selection1.buffer())
                s_baltst::Sequence6(value, d_allocator_p);
        d_selectionId = SELECTION_ID_SELECTION1;
    }

    return d_selection1.object();
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
s_baltst::Sequence6& SequenceWithAnonymityChoice::makeSelection1(s_baltst::Sequence6&& value)
{
    if (SELECTION_ID_SELECTION1 == d_selectionId) {
        d_selection1.object() = bsl::move(value);
    }
    else {
        reset();
        new (d_selection1.buffer())
                s_baltst::Sequence6(bsl::move(value), d_allocator_p);
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

s_baltst::CustomString& SequenceWithAnonymityChoice::makeSelection3()
{
    if (SELECTION_ID_SELECTION3 == d_selectionId) {
        bdlat_ValueTypeFunctions::reset(&d_selection3.object());
    }
    else {
        reset();
        new (d_selection3.buffer())
                s_baltst::CustomString(d_allocator_p);
        d_selectionId = SELECTION_ID_SELECTION3;
    }

    return d_selection3.object();
}

s_baltst::CustomString& SequenceWithAnonymityChoice::makeSelection3(const s_baltst::CustomString& value)
{
    if (SELECTION_ID_SELECTION3 == d_selectionId) {
        d_selection3.object() = value;
    }
    else {
        reset();
        new (d_selection3.buffer())
                s_baltst::CustomString(value, d_allocator_p);
        d_selectionId = SELECTION_ID_SELECTION3;
    }

    return d_selection3.object();
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
s_baltst::CustomString& SequenceWithAnonymityChoice::makeSelection3(s_baltst::CustomString&& value)
{
    if (SELECTION_ID_SELECTION3 == d_selectionId) {
        d_selection3.object() = bsl::move(value);
    }
    else {
        reset();
        new (d_selection3.buffer())
                s_baltst::CustomString(bsl::move(value), d_allocator_p);
        d_selectionId = SELECTION_ID_SELECTION3;
    }

    return d_selection3.object();
}
#endif

s_baltst::CustomInt& SequenceWithAnonymityChoice::makeSelection4()
{
    if (SELECTION_ID_SELECTION4 == d_selectionId) {
        bdlat_ValueTypeFunctions::reset(&d_selection4.object());
    }
    else {
        reset();
        new (d_selection4.buffer())
            s_baltst::CustomInt();
        d_selectionId = SELECTION_ID_SELECTION4;
    }

    return d_selection4.object();
}

s_baltst::CustomInt& SequenceWithAnonymityChoice::makeSelection4(const s_baltst::CustomInt& value)
{
    if (SELECTION_ID_SELECTION4 == d_selectionId) {
        d_selection4.object() = value;
    }
    else {
        reset();
        new (d_selection4.buffer())
                s_baltst::CustomInt(value);
        d_selectionId = SELECTION_ID_SELECTION4;
    }

    return d_selection4.object();
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
s_baltst::CustomInt& SequenceWithAnonymityChoice::makeSelection4(s_baltst::CustomInt&& value)
{
    if (SELECTION_ID_SELECTION4 == d_selectionId) {
        d_selection4.object() = bsl::move(value);
    }
    else {
        reset();
        new (d_selection4.buffer())
                s_baltst::CustomInt(bsl::move(value));
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
                s_baltst::Sequence4(*original.d_selection7, d_allocator_p);
      } break;
      case SELECTION_ID_SELECTION8: {
        d_selection8 = new (*d_allocator_p)
                s_baltst::Choice2(*original.d_selection8, d_allocator_p);
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
                    s_baltst::Sequence4(bsl::move(*original.d_selection7), d_allocator_p);
        }
      } break;
      case SELECTION_ID_SELECTION8: {
        if (d_allocator_p == original.d_allocator_p) {
            d_selection8 = original.d_selection8;
            original.d_selectionId = SELECTION_ID_UNDEFINED;
        }
        else {
            d_selection8 = new (*d_allocator_p)
                    s_baltst::Choice2(bsl::move(*original.d_selection8), d_allocator_p);
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

s_baltst::Sequence4& SequenceWithAnonymityChoice2::makeSelection7()
{
    if (SELECTION_ID_SELECTION7 == d_selectionId) {
        bdlat_ValueTypeFunctions::reset(d_selection7);
    }
    else {
        reset();
        d_selection7 = new (*d_allocator_p)
                s_baltst::Sequence4(d_allocator_p);
        d_selectionId = SELECTION_ID_SELECTION7;
    }

    return *d_selection7;
}

s_baltst::Sequence4& SequenceWithAnonymityChoice2::makeSelection7(const s_baltst::Sequence4& value)
{
    if (SELECTION_ID_SELECTION7 == d_selectionId) {
        *d_selection7 = value;
    }
    else {
        reset();
        d_selection7 = new (*d_allocator_p)
                s_baltst::Sequence4(value, d_allocator_p);
        d_selectionId = SELECTION_ID_SELECTION7;
    }

    return *d_selection7;
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
s_baltst::Sequence4& SequenceWithAnonymityChoice2::makeSelection7(s_baltst::Sequence4&& value)
{
    if (SELECTION_ID_SELECTION7 == d_selectionId) {
        *d_selection7 = bsl::move(value);
    }
    else {
        reset();
        d_selection7 = new (*d_allocator_p)
                s_baltst::Sequence4(bsl::move(value), d_allocator_p);
        d_selectionId = SELECTION_ID_SELECTION7;
    }

    return *d_selection7;
}
#endif

s_baltst::Choice2& SequenceWithAnonymityChoice2::makeSelection8()
{
    if (SELECTION_ID_SELECTION8 == d_selectionId) {
        bdlat_ValueTypeFunctions::reset(d_selection8);
    }
    else {
        reset();
        d_selection8 = new (*d_allocator_p)
                s_baltst::Choice2(d_allocator_p);
        d_selectionId = SELECTION_ID_SELECTION8;
    }

    return *d_selection8;
}

s_baltst::Choice2& SequenceWithAnonymityChoice2::makeSelection8(const s_baltst::Choice2& value)
{
    if (SELECTION_ID_SELECTION8 == d_selectionId) {
        *d_selection8 = value;
    }
    else {
        reset();
        d_selection8 = new (*d_allocator_p)
                s_baltst::Choice2(value, d_allocator_p);
        d_selectionId = SELECTION_ID_SELECTION8;
    }

    return *d_selection8;
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
s_baltst::Choice2& SequenceWithAnonymityChoice2::makeSelection8(s_baltst::Choice2&& value)
{
    if (SELECTION_ID_SELECTION8 == d_selectionId) {
        *d_selection8 = bsl::move(value);
    }
    else {
        reset();
        d_selection8 = new (*d_allocator_p)
                s_baltst::Choice2(bsl::move(value), d_allocator_p);
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

                        // ---------------------------
                        // class SequenceWithAnonymity
                        // ---------------------------

// CONSTANTS

const char SequenceWithAnonymity::CLASS_NAME[] = "SequenceWithAnonymity";

const bdlat_AttributeInfo SequenceWithAnonymity::ATTRIBUTE_INFO_ARRAY[] = {
    {
        ATTRIBUTE_ID_CHOICE,
        "Choice",
        sizeof("Choice") - 1,
        "",
        bdlat_FormattingMode::e_DEFAULT
      | bdlat_FormattingMode::e_UNTAGGED
    },
    {
        ATTRIBUTE_ID_CHOICE1,
        "Choice-1",
        sizeof("Choice-1") - 1,
        "",
        bdlat_FormattingMode::e_DEFAULT
      | bdlat_FormattingMode::e_UNTAGGED
    },
    {
        ATTRIBUTE_ID_CHOICE2,
        "Choice-2",
        sizeof("Choice-2") - 1,
        "",
        bdlat_FormattingMode::e_DEFAULT
      | bdlat_FormattingMode::e_UNTAGGED
    },
    {
        ATTRIBUTE_ID_ELEMENT4,
        "element4",
        sizeof("element4") - 1,
        "",
        bdlat_FormattingMode::e_DEFAULT
    }
};

// CLASS METHODS

const bdlat_AttributeInfo *SequenceWithAnonymity::lookupAttributeInfo(
        const char *name,
        int         nameLength)
{
    if (bdlb::String::areEqualCaseless("selection1", name, nameLength)) {
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CHOICE];
    }

    if (bdlb::String::areEqualCaseless("selection2", name, nameLength)) {
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CHOICE];
    }

    if (bdlb::String::areEqualCaseless("selection3", name, nameLength)) {
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CHOICE];
    }

    if (bdlb::String::areEqualCaseless("selection4", name, nameLength)) {
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CHOICE];
    }

    if (bdlb::String::areEqualCaseless("selection5", name, nameLength)) {
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CHOICE1];
    }

    if (bdlb::String::areEqualCaseless("selection6", name, nameLength)) {
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CHOICE1];
    }

    if (bdlb::String::areEqualCaseless("selection7", name, nameLength)) {
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CHOICE2];
    }

    if (bdlb::String::areEqualCaseless("selection8", name, nameLength)) {
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CHOICE2];
    }

    for (int i = 0; i < 4; ++i) {
        const bdlat_AttributeInfo& attributeInfo =
                    SequenceWithAnonymity::ATTRIBUTE_INFO_ARRAY[i];

        if (nameLength == attributeInfo.d_nameLength
        &&  0 == bsl::memcmp(attributeInfo.d_name_p, name, nameLength))
        {
            return &attributeInfo;
        }
    }

    return 0;
}

const bdlat_AttributeInfo *SequenceWithAnonymity::lookupAttributeInfo(int id)
{
    switch (id) {
      case ATTRIBUTE_ID_CHOICE:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CHOICE];
      case ATTRIBUTE_ID_CHOICE1:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CHOICE1];
      case ATTRIBUTE_ID_CHOICE2:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CHOICE2];
      case ATTRIBUTE_ID_ELEMENT4:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT4];
      default:
        return 0;
    }
}

// CREATORS

SequenceWithAnonymity::SequenceWithAnonymity(bslma::Allocator *basicAllocator)
: d_element4(basicAllocator)
, d_choice2(basicAllocator)
, d_choice1(basicAllocator)
, d_choice(basicAllocator)
{
}

SequenceWithAnonymity::SequenceWithAnonymity(const SequenceWithAnonymity& original,
                                             bslma::Allocator *basicAllocator)
: d_element4(original.d_element4, basicAllocator)
, d_choice2(original.d_choice2, basicAllocator)
, d_choice1(original.d_choice1, basicAllocator)
, d_choice(original.d_choice, basicAllocator)
{
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
SequenceWithAnonymity::SequenceWithAnonymity(SequenceWithAnonymity&& original) noexcept
: d_element4(bsl::move(original.d_element4))
, d_choice2(bsl::move(original.d_choice2))
, d_choice1(bsl::move(original.d_choice1))
, d_choice(bsl::move(original.d_choice))
{
}

SequenceWithAnonymity::SequenceWithAnonymity(SequenceWithAnonymity&& original,
                                             bslma::Allocator *basicAllocator)
: d_element4(bsl::move(original.d_element4), basicAllocator)
, d_choice2(bsl::move(original.d_choice2), basicAllocator)
, d_choice1(bsl::move(original.d_choice1), basicAllocator)
, d_choice(bsl::move(original.d_choice), basicAllocator)
{
}
#endif

SequenceWithAnonymity::~SequenceWithAnonymity()
{
}

// MANIPULATORS

SequenceWithAnonymity&
SequenceWithAnonymity::operator=(const SequenceWithAnonymity& rhs)
{
    if (this != &rhs) {
        d_choice = rhs.d_choice;
        d_choice1 = rhs.d_choice1;
        d_choice2 = rhs.d_choice2;
        d_element4 = rhs.d_element4;
    }

    return *this;
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
SequenceWithAnonymity&
SequenceWithAnonymity::operator=(SequenceWithAnonymity&& rhs)
{
    if (this != &rhs) {
        d_choice = bsl::move(rhs.d_choice);
        d_choice1 = bsl::move(rhs.d_choice1);
        d_choice2 = bsl::move(rhs.d_choice2);
        d_element4 = bsl::move(rhs.d_element4);
    }

    return *this;
}
#endif

void SequenceWithAnonymity::reset()
{
    bdlat_ValueTypeFunctions::reset(&d_choice);
    bdlat_ValueTypeFunctions::reset(&d_choice1);
    bdlat_ValueTypeFunctions::reset(&d_choice2);
    bdlat_ValueTypeFunctions::reset(&d_element4);
}

// ACCESSORS

bsl::ostream& SequenceWithAnonymity::print(
        bsl::ostream& stream,
        int           level,
        int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute("choice", this->choice());
    printer.printAttribute("choice1", this->choice1());
    printer.printAttribute("choice2", this->choice2());
    printer.printAttribute("element4", this->element4());
    printer.end();
    return stream;
}


}  // close package namespace
}  // close enterprise namespace

// GENERATED BY @BLP_BAS_CODEGEN_VERSION@
// USING bas_codegen.pl s_baltst_sequencewithanonymity.xsd --mode msg --includedir . --msgComponent sequencewithanonymity --noRecurse --noExternalization --noHashSupport --noAggregateConversion
// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright 2022 Bloomberg Finance L.P. All rights reserved.
//      Property of Bloomberg Finance L.P. (BFLP)
//      This software is made available solely pursuant to the
//      terms of a BFLP license agreement which governs its use.
// ------------------------------- END-OF-FILE --------------------------------
