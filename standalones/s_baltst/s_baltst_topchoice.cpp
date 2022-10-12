// s_baltst_topchoice.cpp          *DO NOT EDIT*           @generated -*-C++-*-

#include <bsls_ident.h>
BSLS_IDENT_RCSID(s_baltst_topchoice_cpp, "$Id$ $CSID$")

#include <s_baltst_topchoice.h>

#include <bdlat_formattingmode.h>
#include <bdlat_valuetypefunctions.h>
#include <bdlb_print.h>
#include <bdlb_printmethods.h>
#include <bdlb_string.h>

#include <bdlt_datetimetz.h>
#include <bsl_vector.h>
#include <s_baltst_customstring.h>
#include <s_baltst_enumerated.h>
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

                              // ---------------
                              // class Topchoice
                              // ---------------

// CONSTANTS

const char Topchoice::CLASS_NAME[] = "Topchoice";

const bdlat_SelectionInfo Topchoice::SELECTION_INFO_ARRAY[] = {
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
        bdlat_FormattingMode::e_HEX
    },
    {
        SELECTION_ID_SELECTION3,
        "selection3",
        sizeof("selection3") - 1,
        "",
        bdlat_FormattingMode::e_DEFAULT
    },
    {
        SELECTION_ID_SELECTION4,
        "selection4",
        sizeof("selection4") - 1,
        "",
        bdlat_FormattingMode::e_DEFAULT
    },
    {
        SELECTION_ID_SELECTION5,
        "selection5",
        sizeof("selection5") - 1,
        "",
        bdlat_FormattingMode::e_DEFAULT
    },
    {
        SELECTION_ID_SELECTION6,
        "selection6",
        sizeof("selection6") - 1,
        "",
        bdlat_FormattingMode::e_TEXT
    },
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

const bdlat_SelectionInfo *Topchoice::lookupSelectionInfo(
        const char *name,
        int         nameLength)
{
    for (int i = 0; i < 8; ++i) {
        const bdlat_SelectionInfo& selectionInfo =
                    Topchoice::SELECTION_INFO_ARRAY[i];

        if (nameLength == selectionInfo.d_nameLength
        &&  0 == bsl::memcmp(selectionInfo.d_name_p, name, nameLength))
        {
            return &selectionInfo;
        }
    }

    return 0;
}

const bdlat_SelectionInfo *Topchoice::lookupSelectionInfo(int id)
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
      case SELECTION_ID_SELECTION5:
        return &SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION5];
      case SELECTION_ID_SELECTION6:
        return &SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION6];
      case SELECTION_ID_SELECTION7:
        return &SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION7];
      case SELECTION_ID_SELECTION8:
        return &SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION8];
      default:
        return 0;
    }
}

// CREATORS

Topchoice::Topchoice(
    const Topchoice& original,
    bslma::Allocator *basicAllocator)
: d_selectionId(original.d_selectionId)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    switch (d_selectionId) {
      case SELECTION_ID_SELECTION1: {
        new (d_selection1.buffer())
            s_baltst::Sequence1(
                original.d_selection1.object(), d_allocator_p);
      } break;
      case SELECTION_ID_SELECTION2: {
        new (d_selection2.buffer())
            bsl::vector<char>(
                original.d_selection2.object(), d_allocator_p);
      } break;
      case SELECTION_ID_SELECTION3: {
        new (d_selection3.buffer())
            s_baltst::Sequence2(
                original.d_selection3.object(), d_allocator_p);
      } break;
      case SELECTION_ID_SELECTION4: {
        d_selection4 = new (*d_allocator_p)
                s_baltst::Sequence3(*original.d_selection4, d_allocator_p);
      } break;
      case SELECTION_ID_SELECTION5: {
        new (d_selection5.buffer())
            bdlt::DatetimeTz(original.d_selection5.object());
      } break;
      case SELECTION_ID_SELECTION6: {
        new (d_selection6.buffer())
            s_baltst::CustomString(
                original.d_selection6.object(), d_allocator_p);
      } break;
      case SELECTION_ID_SELECTION7: {
        new (d_selection7.buffer())
            s_baltst::Enumerated::Value(original.d_selection7.object());
      } break;
      case SELECTION_ID_SELECTION8: {
        new (d_selection8.buffer())
            s_baltst::Choice3(
                original.d_selection8.object(), d_allocator_p);
      } break;
      default:
        BSLS_ASSERT(SELECTION_ID_UNDEFINED == d_selectionId);
    }
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
Topchoice::Topchoice(Topchoice&& original) noexcept
: d_selectionId(original.d_selectionId)
, d_allocator_p(original.d_allocator_p)
{
    switch (d_selectionId) {
      case SELECTION_ID_SELECTION1: {
        new (d_selection1.buffer())
            s_baltst::Sequence1(
                bsl::move(original.d_selection1.object()), d_allocator_p);
      } break;
      case SELECTION_ID_SELECTION2: {
        new (d_selection2.buffer())
            bsl::vector<char>(
                bsl::move(original.d_selection2.object()), d_allocator_p);
      } break;
      case SELECTION_ID_SELECTION3: {
        new (d_selection3.buffer())
            s_baltst::Sequence2(
                bsl::move(original.d_selection3.object()), d_allocator_p);
      } break;
      case SELECTION_ID_SELECTION4: {
        d_selection4 = original.d_selection4;
        original.d_selectionId = SELECTION_ID_UNDEFINED;
      } break;
      case SELECTION_ID_SELECTION5: {
        new (d_selection5.buffer())
            bdlt::DatetimeTz(bsl::move(original.d_selection5.object()));
      } break;
      case SELECTION_ID_SELECTION6: {
        new (d_selection6.buffer())
            s_baltst::CustomString(
                bsl::move(original.d_selection6.object()), d_allocator_p);
      } break;
      case SELECTION_ID_SELECTION7: {
        new (d_selection7.buffer())
            s_baltst::Enumerated::Value(bsl::move(original.d_selection7.object()));
      } break;
      case SELECTION_ID_SELECTION8: {
        new (d_selection8.buffer())
            s_baltst::Choice3(
                bsl::move(original.d_selection8.object()), d_allocator_p);
      } break;
      default:
        BSLS_ASSERT(SELECTION_ID_UNDEFINED == d_selectionId);
    }
}

Topchoice::Topchoice(
    Topchoice&& original,
    bslma::Allocator *basicAllocator)
: d_selectionId(original.d_selectionId)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    switch (d_selectionId) {
      case SELECTION_ID_SELECTION1: {
        new (d_selection1.buffer())
            s_baltst::Sequence1(
                bsl::move(original.d_selection1.object()), d_allocator_p);
      } break;
      case SELECTION_ID_SELECTION2: {
        new (d_selection2.buffer())
            bsl::vector<char>(
                bsl::move(original.d_selection2.object()), d_allocator_p);
      } break;
      case SELECTION_ID_SELECTION3: {
        new (d_selection3.buffer())
            s_baltst::Sequence2(
                bsl::move(original.d_selection3.object()), d_allocator_p);
      } break;
      case SELECTION_ID_SELECTION4: {
        if (d_allocator_p == original.d_allocator_p) {
            d_selection4 = original.d_selection4;
            original.d_selectionId = SELECTION_ID_UNDEFINED;
        }
        else {
            d_selection4 = new (*d_allocator_p)
                    s_baltst::Sequence3(bsl::move(*original.d_selection4), d_allocator_p);
        }
      } break;
      case SELECTION_ID_SELECTION5: {
        new (d_selection5.buffer())
            bdlt::DatetimeTz(bsl::move(original.d_selection5.object()));
      } break;
      case SELECTION_ID_SELECTION6: {
        new (d_selection6.buffer())
            s_baltst::CustomString(
                bsl::move(original.d_selection6.object()), d_allocator_p);
      } break;
      case SELECTION_ID_SELECTION7: {
        new (d_selection7.buffer())
            s_baltst::Enumerated::Value(bsl::move(original.d_selection7.object()));
      } break;
      case SELECTION_ID_SELECTION8: {
        new (d_selection8.buffer())
            s_baltst::Choice3(
                bsl::move(original.d_selection8.object()), d_allocator_p);
      } break;
      default:
        BSLS_ASSERT(SELECTION_ID_UNDEFINED == d_selectionId);
    }
}
#endif

// MANIPULATORS

Topchoice&
Topchoice::operator=(const Topchoice& rhs)
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
            makeSelection4(*rhs.d_selection4);
          } break;
          case SELECTION_ID_SELECTION5: {
            makeSelection5(rhs.d_selection5.object());
          } break;
          case SELECTION_ID_SELECTION6: {
            makeSelection6(rhs.d_selection6.object());
          } break;
          case SELECTION_ID_SELECTION7: {
            makeSelection7(rhs.d_selection7.object());
          } break;
          case SELECTION_ID_SELECTION8: {
            makeSelection8(rhs.d_selection8.object());
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
Topchoice&
Topchoice::operator=(Topchoice&& rhs)
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
            if (d_allocator_p == rhs.d_allocator_p) {
                reset();
                d_selection4 = rhs.d_selection4;
                d_selectionId = SELECTION_ID_SELECTION4;
                rhs.d_selectionId = SELECTION_ID_UNDEFINED;
            }
            else {
                makeSelection4(bsl::move(*rhs.d_selection4));
            }
          } break;
          case SELECTION_ID_SELECTION5: {
            makeSelection5(bsl::move(rhs.d_selection5.object()));
          } break;
          case SELECTION_ID_SELECTION6: {
            makeSelection6(bsl::move(rhs.d_selection6.object()));
          } break;
          case SELECTION_ID_SELECTION7: {
            makeSelection7(bsl::move(rhs.d_selection7.object()));
          } break;
          case SELECTION_ID_SELECTION8: {
            makeSelection8(bsl::move(rhs.d_selection8.object()));
          } break;
          default:
            BSLS_ASSERT(SELECTION_ID_UNDEFINED == rhs.d_selectionId);
            reset();
        }
    }

    return *this;
}
#endif

void Topchoice::reset()
{
    switch (d_selectionId) {
      case SELECTION_ID_SELECTION1: {
        typedef s_baltst::Sequence1 Type;
        d_selection1.object().~Type();
      } break;
      case SELECTION_ID_SELECTION2: {
        typedef bsl::vector<char> Type;
        d_selection2.object().~Type();
      } break;
      case SELECTION_ID_SELECTION3: {
        typedef s_baltst::Sequence2 Type;
        d_selection3.object().~Type();
      } break;
      case SELECTION_ID_SELECTION4: {
        d_allocator_p->deleteObject(d_selection4);
      } break;
      case SELECTION_ID_SELECTION5: {
        // no destruction required
      } break;
      case SELECTION_ID_SELECTION6: {
        typedef s_baltst::CustomString Type;
        d_selection6.object().~Type();
      } break;
      case SELECTION_ID_SELECTION7: {
        typedef s_baltst::Enumerated::Value Type;
        d_selection7.object().~Type();
      } break;
      case SELECTION_ID_SELECTION8: {
        typedef s_baltst::Choice3 Type;
        d_selection8.object().~Type();
      } break;
      default:
        BSLS_ASSERT(SELECTION_ID_UNDEFINED == d_selectionId);
    }

    d_selectionId = SELECTION_ID_UNDEFINED;
}

int Topchoice::makeSelection(int selectionId)
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
      case SELECTION_ID_SELECTION5: {
        makeSelection5();
      } break;
      case SELECTION_ID_SELECTION6: {
        makeSelection6();
      } break;
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

int Topchoice::makeSelection(const char *name, int nameLength)
{
    const bdlat_SelectionInfo *selectionInfo =
                                         lookupSelectionInfo(name, nameLength);
    if (0 == selectionInfo) {
       return -1;
    }

    return makeSelection(selectionInfo->d_id);
}

s_baltst::Sequence1& Topchoice::makeSelection1()
{
    if (SELECTION_ID_SELECTION1 == d_selectionId) {
        bdlat_ValueTypeFunctions::reset(&d_selection1.object());
    }
    else {
        reset();
        new (d_selection1.buffer())
                s_baltst::Sequence1(d_allocator_p);
        d_selectionId = SELECTION_ID_SELECTION1;
    }

    return d_selection1.object();
}

s_baltst::Sequence1& Topchoice::makeSelection1(const s_baltst::Sequence1& value)
{
    if (SELECTION_ID_SELECTION1 == d_selectionId) {
        d_selection1.object() = value;
    }
    else {
        reset();
        new (d_selection1.buffer())
                s_baltst::Sequence1(value, d_allocator_p);
        d_selectionId = SELECTION_ID_SELECTION1;
    }

    return d_selection1.object();
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
s_baltst::Sequence1& Topchoice::makeSelection1(s_baltst::Sequence1&& value)
{
    if (SELECTION_ID_SELECTION1 == d_selectionId) {
        d_selection1.object() = bsl::move(value);
    }
    else {
        reset();
        new (d_selection1.buffer())
                s_baltst::Sequence1(bsl::move(value), d_allocator_p);
        d_selectionId = SELECTION_ID_SELECTION1;
    }

    return d_selection1.object();
}
#endif

bsl::vector<char>& Topchoice::makeSelection2()
{
    if (SELECTION_ID_SELECTION2 == d_selectionId) {
        bdlat_ValueTypeFunctions::reset(&d_selection2.object());
    }
    else {
        reset();
        new (d_selection2.buffer())
                bsl::vector<char>(d_allocator_p);
        d_selectionId = SELECTION_ID_SELECTION2;
    }

    return d_selection2.object();
}

bsl::vector<char>& Topchoice::makeSelection2(const bsl::vector<char>& value)
{
    if (SELECTION_ID_SELECTION2 == d_selectionId) {
        d_selection2.object() = value;
    }
    else {
        reset();
        new (d_selection2.buffer())
                bsl::vector<char>(value, d_allocator_p);
        d_selectionId = SELECTION_ID_SELECTION2;
    }

    return d_selection2.object();
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
bsl::vector<char>& Topchoice::makeSelection2(bsl::vector<char>&& value)
{
    if (SELECTION_ID_SELECTION2 == d_selectionId) {
        d_selection2.object() = bsl::move(value);
    }
    else {
        reset();
        new (d_selection2.buffer())
                bsl::vector<char>(bsl::move(value), d_allocator_p);
        d_selectionId = SELECTION_ID_SELECTION2;
    }

    return d_selection2.object();
}
#endif

s_baltst::Sequence2& Topchoice::makeSelection3()
{
    if (SELECTION_ID_SELECTION3 == d_selectionId) {
        bdlat_ValueTypeFunctions::reset(&d_selection3.object());
    }
    else {
        reset();
        new (d_selection3.buffer())
                s_baltst::Sequence2(d_allocator_p);
        d_selectionId = SELECTION_ID_SELECTION3;
    }

    return d_selection3.object();
}

s_baltst::Sequence2& Topchoice::makeSelection3(const s_baltst::Sequence2& value)
{
    if (SELECTION_ID_SELECTION3 == d_selectionId) {
        d_selection3.object() = value;
    }
    else {
        reset();
        new (d_selection3.buffer())
                s_baltst::Sequence2(value, d_allocator_p);
        d_selectionId = SELECTION_ID_SELECTION3;
    }

    return d_selection3.object();
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
s_baltst::Sequence2& Topchoice::makeSelection3(s_baltst::Sequence2&& value)
{
    if (SELECTION_ID_SELECTION3 == d_selectionId) {
        d_selection3.object() = bsl::move(value);
    }
    else {
        reset();
        new (d_selection3.buffer())
                s_baltst::Sequence2(bsl::move(value), d_allocator_p);
        d_selectionId = SELECTION_ID_SELECTION3;
    }

    return d_selection3.object();
}
#endif

s_baltst::Sequence3& Topchoice::makeSelection4()
{
    if (SELECTION_ID_SELECTION4 == d_selectionId) {
        bdlat_ValueTypeFunctions::reset(d_selection4);
    }
    else {
        reset();
        d_selection4 = new (*d_allocator_p)
                s_baltst::Sequence3(d_allocator_p);
        d_selectionId = SELECTION_ID_SELECTION4;
    }

    return *d_selection4;
}

s_baltst::Sequence3& Topchoice::makeSelection4(const s_baltst::Sequence3& value)
{
    if (SELECTION_ID_SELECTION4 == d_selectionId) {
        *d_selection4 = value;
    }
    else {
        reset();
        d_selection4 = new (*d_allocator_p)
                s_baltst::Sequence3(value, d_allocator_p);
        d_selectionId = SELECTION_ID_SELECTION4;
    }

    return *d_selection4;
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
s_baltst::Sequence3& Topchoice::makeSelection4(s_baltst::Sequence3&& value)
{
    if (SELECTION_ID_SELECTION4 == d_selectionId) {
        *d_selection4 = bsl::move(value);
    }
    else {
        reset();
        d_selection4 = new (*d_allocator_p)
                s_baltst::Sequence3(bsl::move(value), d_allocator_p);
        d_selectionId = SELECTION_ID_SELECTION4;
    }

    return *d_selection4;
}
#endif

bdlt::DatetimeTz& Topchoice::makeSelection5()
{
    if (SELECTION_ID_SELECTION5 == d_selectionId) {
        bdlat_ValueTypeFunctions::reset(&d_selection5.object());
    }
    else {
        reset();
        new (d_selection5.buffer())
            bdlt::DatetimeTz();
        d_selectionId = SELECTION_ID_SELECTION5;
    }

    return d_selection5.object();
}

bdlt::DatetimeTz& Topchoice::makeSelection5(const bdlt::DatetimeTz& value)
{
    if (SELECTION_ID_SELECTION5 == d_selectionId) {
        d_selection5.object() = value;
    }
    else {
        reset();
        new (d_selection5.buffer())
                bdlt::DatetimeTz(value);
        d_selectionId = SELECTION_ID_SELECTION5;
    }

    return d_selection5.object();
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
bdlt::DatetimeTz& Topchoice::makeSelection5(bdlt::DatetimeTz&& value)
{
    if (SELECTION_ID_SELECTION5 == d_selectionId) {
        d_selection5.object() = bsl::move(value);
    }
    else {
        reset();
        new (d_selection5.buffer())
                bdlt::DatetimeTz(bsl::move(value));
        d_selectionId = SELECTION_ID_SELECTION5;
    }

    return d_selection5.object();
}
#endif

s_baltst::CustomString& Topchoice::makeSelection6()
{
    if (SELECTION_ID_SELECTION6 == d_selectionId) {
        bdlat_ValueTypeFunctions::reset(&d_selection6.object());
    }
    else {
        reset();
        new (d_selection6.buffer())
                s_baltst::CustomString(d_allocator_p);
        d_selectionId = SELECTION_ID_SELECTION6;
    }

    return d_selection6.object();
}

s_baltst::CustomString& Topchoice::makeSelection6(const s_baltst::CustomString& value)
{
    if (SELECTION_ID_SELECTION6 == d_selectionId) {
        d_selection6.object() = value;
    }
    else {
        reset();
        new (d_selection6.buffer())
                s_baltst::CustomString(value, d_allocator_p);
        d_selectionId = SELECTION_ID_SELECTION6;
    }

    return d_selection6.object();
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
s_baltst::CustomString& Topchoice::makeSelection6(s_baltst::CustomString&& value)
{
    if (SELECTION_ID_SELECTION6 == d_selectionId) {
        d_selection6.object() = bsl::move(value);
    }
    else {
        reset();
        new (d_selection6.buffer())
                s_baltst::CustomString(bsl::move(value), d_allocator_p);
        d_selectionId = SELECTION_ID_SELECTION6;
    }

    return d_selection6.object();
}
#endif

s_baltst::Enumerated::Value& Topchoice::makeSelection7()
{
    if (SELECTION_ID_SELECTION7 == d_selectionId) {
        bdlat_ValueTypeFunctions::reset(&d_selection7.object());
    }
    else {
        reset();
        new (d_selection7.buffer())
                    s_baltst::Enumerated::Value(static_cast<s_baltst::Enumerated::Value>(0));
        d_selectionId = SELECTION_ID_SELECTION7;
    }

    return d_selection7.object();
}

s_baltst::Enumerated::Value& Topchoice::makeSelection7(s_baltst::Enumerated::Value value)
{
    if (SELECTION_ID_SELECTION7 == d_selectionId) {
        d_selection7.object() = value;
    }
    else {
        reset();
        new (d_selection7.buffer())
                s_baltst::Enumerated::Value(value);
        d_selectionId = SELECTION_ID_SELECTION7;
    }

    return d_selection7.object();
}

s_baltst::Choice3& Topchoice::makeSelection8()
{
    if (SELECTION_ID_SELECTION8 == d_selectionId) {
        bdlat_ValueTypeFunctions::reset(&d_selection8.object());
    }
    else {
        reset();
        new (d_selection8.buffer())
                s_baltst::Choice3(d_allocator_p);
        d_selectionId = SELECTION_ID_SELECTION8;
    }

    return d_selection8.object();
}

s_baltst::Choice3& Topchoice::makeSelection8(const s_baltst::Choice3& value)
{
    if (SELECTION_ID_SELECTION8 == d_selectionId) {
        d_selection8.object() = value;
    }
    else {
        reset();
        new (d_selection8.buffer())
                s_baltst::Choice3(value, d_allocator_p);
        d_selectionId = SELECTION_ID_SELECTION8;
    }

    return d_selection8.object();
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
s_baltst::Choice3& Topchoice::makeSelection8(s_baltst::Choice3&& value)
{
    if (SELECTION_ID_SELECTION8 == d_selectionId) {
        d_selection8.object() = bsl::move(value);
    }
    else {
        reset();
        new (d_selection8.buffer())
                s_baltst::Choice3(bsl::move(value), d_allocator_p);
        d_selectionId = SELECTION_ID_SELECTION8;
    }

    return d_selection8.object();
}
#endif

// ACCESSORS

bsl::ostream& Topchoice::print(
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
        bool multilineFlag = (0 <= level);
        bdlb::Print::indent(stream, level + 1, spacesPerLevel);
        stream << (multilineFlag ? "" : " ");
        stream << "selection2 = [ ";
        bdlb::Print::singleLineHexDump(
            stream, d_selection2.object().begin(), d_selection2.object().end());
        stream << " ]" << (multilineFlag ? "\n" : "");
      }  break;
      case SELECTION_ID_SELECTION3: {
        printer.printAttribute("selection3", d_selection3.object());
      }  break;
      case SELECTION_ID_SELECTION4: {
        printer.printAttribute("selection4", *d_selection4);
      }  break;
      case SELECTION_ID_SELECTION5: {
        printer.printAttribute("selection5", d_selection5.object());
      }  break;
      case SELECTION_ID_SELECTION6: {
        printer.printAttribute("selection6", d_selection6.object());
      }  break;
      case SELECTION_ID_SELECTION7: {
        printer.printAttribute("selection7", d_selection7.object());
      }  break;
      case SELECTION_ID_SELECTION8: {
        printer.printAttribute("selection8", d_selection8.object());
      }  break;
      default:
        stream << "SELECTION UNDEFINED\n";
    }
    printer.end();
    return stream;
}


const char *Topchoice::selectionName() const
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
      case SELECTION_ID_SELECTION5:
        return SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION5].name();
      case SELECTION_ID_SELECTION6:
        return SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION6].name();
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

// GENERATED BY @BLP_BAS_CODEGEN_VERSION@
// USING bas_codegen.pl s_baltst_topchoice.xsd --mode msg --includedir . --msgComponent topchoice --noRecurse --noExternalization --noHashSupport --noAggregateConversion
// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright 2022 Bloomberg Finance L.P. All rights reserved.
//      Property of Bloomberg Finance L.P. (BFLP)
//      This software is made available solely pursuant to the
//      terms of a BFLP license agreement which governs its use.
// ------------------------------- END-OF-FILE --------------------------------
