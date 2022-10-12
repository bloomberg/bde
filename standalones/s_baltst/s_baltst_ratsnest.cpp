// s_baltst_ratsnest.cpp           *DO NOT EDIT*           @generated -*-C++-*-

#include <bsls_ident.h>
BSLS_IDENT_RCSID(s_baltst_ratsnest_cpp, "$Id$ $CSID$")

#include <s_baltst_ratsnest.h>

#include <bdlat_formattingmode.h>
#include <bdlat_valuetypefunctions.h>
#include <bdlb_print.h>
#include <bdlb_printmethods.h>
#include <bdlb_string.h>

#include <bdlb_nullableallocatedvalue.h>
#include <bdlb_nullablevalue.h>
#include <bdlt_datetimetz.h>
#include <bsl_string.h>
#include <bsl_vector.h>
#include <s_baltst_customint.h>
#include <s_baltst_customstring.h>
#include <s_baltst_enumerated.h>
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
                              // class Sequence3
                              // ---------------

// CONSTANTS

const char Sequence3::CLASS_NAME[] = "Sequence3";

const bdlat_AttributeInfo Sequence3::ATTRIBUTE_INFO_ARRAY[] = {
    {
        ATTRIBUTE_ID_ELEMENT1,
        "element1",
        sizeof("element1") - 1,
        "",
        bdlat_FormattingMode::e_DEFAULT
    },
    {
        ATTRIBUTE_ID_ELEMENT2,
        "element2",
        sizeof("element2") - 1,
        "",
        bdlat_FormattingMode::e_TEXT
    },
    {
        ATTRIBUTE_ID_ELEMENT3,
        "element3",
        sizeof("element3") - 1,
        "",
        bdlat_FormattingMode::e_TEXT
    },
    {
        ATTRIBUTE_ID_ELEMENT4,
        "element4",
        sizeof("element4") - 1,
        "",
        bdlat_FormattingMode::e_TEXT
    },
    {
        ATTRIBUTE_ID_ELEMENT5,
        "element5",
        sizeof("element5") - 1,
        "",
        bdlat_FormattingMode::e_DEFAULT
    },
    {
        ATTRIBUTE_ID_ELEMENT6,
        "element6",
        sizeof("element6") - 1,
        "",
        bdlat_FormattingMode::e_DEFAULT
      | bdlat_FormattingMode::e_NILLABLE
    }
};

// CLASS METHODS

const bdlat_AttributeInfo *Sequence3::lookupAttributeInfo(
        const char *name,
        int         nameLength)
{
    for (int i = 0; i < 6; ++i) {
        const bdlat_AttributeInfo& attributeInfo =
                    Sequence3::ATTRIBUTE_INFO_ARRAY[i];

        if (nameLength == attributeInfo.d_nameLength
        &&  0 == bsl::memcmp(attributeInfo.d_name_p, name, nameLength))
        {
            return &attributeInfo;
        }
    }

    return 0;
}

const bdlat_AttributeInfo *Sequence3::lookupAttributeInfo(int id)
{
    switch (id) {
      case ATTRIBUTE_ID_ELEMENT1:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT1];
      case ATTRIBUTE_ID_ELEMENT2:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT2];
      case ATTRIBUTE_ID_ELEMENT3:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT3];
      case ATTRIBUTE_ID_ELEMENT4:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT4];
      case ATTRIBUTE_ID_ELEMENT5:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT5];
      case ATTRIBUTE_ID_ELEMENT6:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT6];
      default:
        return 0;
    }
}

// CREATORS

Sequence3::Sequence3(bslma::Allocator *basicAllocator)
: d_element2(basicAllocator)
, d_element4(basicAllocator)
, d_element5(basicAllocator)
, d_element1(basicAllocator)
, d_element6(basicAllocator)
, d_element3()
{
}

Sequence3::Sequence3(const Sequence3& original,
                     bslma::Allocator *basicAllocator)
: d_element2(original.d_element2, basicAllocator)
, d_element4(original.d_element4, basicAllocator)
, d_element5(original.d_element5, basicAllocator)
, d_element1(original.d_element1, basicAllocator)
, d_element6(original.d_element6, basicAllocator)
, d_element3(original.d_element3)
{
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
Sequence3::Sequence3(Sequence3&& original) noexcept
: d_element2(bsl::move(original.d_element2))
, d_element4(bsl::move(original.d_element4))
, d_element5(bsl::move(original.d_element5))
, d_element1(bsl::move(original.d_element1))
, d_element6(bsl::move(original.d_element6))
, d_element3(bsl::move(original.d_element3))
{
}

Sequence3::Sequence3(Sequence3&& original,
                     bslma::Allocator *basicAllocator)
: d_element2(bsl::move(original.d_element2), basicAllocator)
, d_element4(bsl::move(original.d_element4), basicAllocator)
, d_element5(bsl::move(original.d_element5), basicAllocator)
, d_element1(bsl::move(original.d_element1), basicAllocator)
, d_element6(bsl::move(original.d_element6), basicAllocator)
, d_element3(bsl::move(original.d_element3))
{
}
#endif

Sequence3::~Sequence3()
{
}

// MANIPULATORS

Sequence3&
Sequence3::operator=(const Sequence3& rhs)
{
    if (this != &rhs) {
        d_element1 = rhs.d_element1;
        d_element2 = rhs.d_element2;
        d_element3 = rhs.d_element3;
        d_element4 = rhs.d_element4;
        d_element5 = rhs.d_element5;
        d_element6 = rhs.d_element6;
    }

    return *this;
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
Sequence3&
Sequence3::operator=(Sequence3&& rhs)
{
    if (this != &rhs) {
        d_element1 = bsl::move(rhs.d_element1);
        d_element2 = bsl::move(rhs.d_element2);
        d_element3 = bsl::move(rhs.d_element3);
        d_element4 = bsl::move(rhs.d_element4);
        d_element5 = bsl::move(rhs.d_element5);
        d_element6 = bsl::move(rhs.d_element6);
    }

    return *this;
}
#endif

void Sequence3::reset()
{
    bdlat_ValueTypeFunctions::reset(&d_element1);
    bdlat_ValueTypeFunctions::reset(&d_element2);
    bdlat_ValueTypeFunctions::reset(&d_element3);
    bdlat_ValueTypeFunctions::reset(&d_element4);
    bdlat_ValueTypeFunctions::reset(&d_element5);
    bdlat_ValueTypeFunctions::reset(&d_element6);
}

// ACCESSORS

bsl::ostream& Sequence3::print(
        bsl::ostream& stream,
        int           level,
        int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute("element1", this->element1());
    printer.printAttribute("element2", this->element2());
    printer.printAttribute("element3", this->element3());
    printer.printAttribute("element4", this->element4());
    printer.printAttribute("element5", this->element5());
    printer.printAttribute("element6", this->element6());
    printer.end();
    return stream;
}



                              // ---------------
                              // class Sequence5
                              // ---------------

// CONSTANTS

const char Sequence5::CLASS_NAME[] = "Sequence5";

const bdlat_AttributeInfo Sequence5::ATTRIBUTE_INFO_ARRAY[] = {
    {
        ATTRIBUTE_ID_ELEMENT1,
        "element1",
        sizeof("element1") - 1,
        "",
        bdlat_FormattingMode::e_DEFAULT
    },
    {
        ATTRIBUTE_ID_ELEMENT2,
        "element2",
        sizeof("element2") - 1,
        "",
        bdlat_FormattingMode::e_TEXT
      | bdlat_FormattingMode::e_NILLABLE
    },
    {
        ATTRIBUTE_ID_ELEMENT3,
        "element3",
        sizeof("element3") - 1,
        "",
        bdlat_FormattingMode::e_DEFAULT
      | bdlat_FormattingMode::e_NILLABLE
    },
    {
        ATTRIBUTE_ID_ELEMENT4,
        "element4",
        sizeof("element4") - 1,
        "",
        bdlat_FormattingMode::e_HEX
      | bdlat_FormattingMode::e_NILLABLE
    },
    {
        ATTRIBUTE_ID_ELEMENT5,
        "element5",
        sizeof("element5") - 1,
        "",
        bdlat_FormattingMode::e_DEC
      | bdlat_FormattingMode::e_NILLABLE
    },
    {
        ATTRIBUTE_ID_ELEMENT6,
        "element6",
        sizeof("element6") - 1,
        "",
        bdlat_FormattingMode::e_DEFAULT
      | bdlat_FormattingMode::e_NILLABLE
    },
    {
        ATTRIBUTE_ID_ELEMENT7,
        "element7",
        sizeof("element7") - 1,
        "",
        bdlat_FormattingMode::e_DEFAULT
      | bdlat_FormattingMode::e_NILLABLE
    }
};

// CLASS METHODS

const bdlat_AttributeInfo *Sequence5::lookupAttributeInfo(
        const char *name,
        int         nameLength)
{
    for (int i = 0; i < 7; ++i) {
        const bdlat_AttributeInfo& attributeInfo =
                    Sequence5::ATTRIBUTE_INFO_ARRAY[i];

        if (nameLength == attributeInfo.d_nameLength
        &&  0 == bsl::memcmp(attributeInfo.d_name_p, name, nameLength))
        {
            return &attributeInfo;
        }
    }

    return 0;
}

const bdlat_AttributeInfo *Sequence5::lookupAttributeInfo(int id)
{
    switch (id) {
      case ATTRIBUTE_ID_ELEMENT1:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT1];
      case ATTRIBUTE_ID_ELEMENT2:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT2];
      case ATTRIBUTE_ID_ELEMENT3:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT3];
      case ATTRIBUTE_ID_ELEMENT4:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT4];
      case ATTRIBUTE_ID_ELEMENT5:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT5];
      case ATTRIBUTE_ID_ELEMENT6:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT6];
      case ATTRIBUTE_ID_ELEMENT7:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT7];
      default:
        return 0;
    }
}

// CREATORS

Sequence5::Sequence5(bslma::Allocator *basicAllocator)
: d_allocator_p(bslma::Default::allocator(basicAllocator))
, d_element5(basicAllocator)
, d_element3(basicAllocator)
, d_element2(basicAllocator)
, d_element6(basicAllocator)
, d_element7(basicAllocator)
, d_element4(basicAllocator)
{
    d_element1 = new (*d_allocator_p)
            Sequence3(d_allocator_p);
}

Sequence5::Sequence5(const Sequence5& original,
                     bslma::Allocator *basicAllocator)
: d_allocator_p(bslma::Default::allocator(basicAllocator))
, d_element5(original.d_element5, basicAllocator)
, d_element3(original.d_element3, basicAllocator)
, d_element2(original.d_element2, basicAllocator)
, d_element6(original.d_element6, basicAllocator)
, d_element7(original.d_element7, basicAllocator)
, d_element4(original.d_element4, basicAllocator)
{
    d_element1 = new (*d_allocator_p)
            Sequence3(*original.d_element1, d_allocator_p);
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
Sequence5::Sequence5(Sequence5&& original) noexcept
: d_allocator_p(original.d_allocator_p)
, d_element5(bsl::move(original.d_element5))
, d_element3(bsl::move(original.d_element3))
, d_element2(bsl::move(original.d_element2))
, d_element6(bsl::move(original.d_element6))
, d_element7(bsl::move(original.d_element7))
, d_element4(bsl::move(original.d_element4))
{
    d_element1 = original.d_element1;
    original.d_element1 = 0;
}

Sequence5::Sequence5(Sequence5&& original,
                     bslma::Allocator *basicAllocator)
: d_allocator_p(bslma::Default::allocator(basicAllocator))
, d_element5(bsl::move(original.d_element5), basicAllocator)
, d_element3(bsl::move(original.d_element3), basicAllocator)
, d_element2(bsl::move(original.d_element2), basicAllocator)
, d_element6(bsl::move(original.d_element6), basicAllocator)
, d_element7(bsl::move(original.d_element7), basicAllocator)
, d_element4(bsl::move(original.d_element4), basicAllocator)
{
    if (d_allocator_p == original.d_allocator_p) {
        d_element1 = original.d_element1;
        original.d_element1 = 0;
    }
    else {
        d_element1 = new (*d_allocator_p)
                Sequence3(bsl::move(*original.d_element1), d_allocator_p);
    }
}
#endif

Sequence5::~Sequence5()
{
    d_allocator_p->deleteObject(d_element1);
}

// MANIPULATORS

Sequence5&
Sequence5::operator=(const Sequence5& rhs)
{
    if (this != &rhs) {
        if (d_element1) {
            *d_element1 = *rhs.d_element1;
        }
        else {
            d_element1 = new (*d_allocator_p)
                Sequence3(*rhs.d_element1, d_allocator_p);
        }
        d_element2 = rhs.d_element2;
        d_element3 = rhs.d_element3;
        d_element4 = rhs.d_element4;
        d_element5 = rhs.d_element5;
        d_element6 = rhs.d_element6;
        d_element7 = rhs.d_element7;
    }

    return *this;
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
Sequence5&
Sequence5::operator=(Sequence5&& rhs)
{
    if (this != &rhs) {
        if (d_allocator_p == rhs.d_allocator_p) {
            d_allocator_p->deleteObject(d_element1);
            d_element1 = rhs.d_element1;
            rhs.d_element1 = 0;
        }
        else if (d_element1) {
            *d_element1 = bsl::move(*rhs.d_element1);
        }
        else {
            d_element1 = new (*d_allocator_p)
                Sequence3(bsl::move(*rhs.d_element1), d_allocator_p);
        }
        d_element2 = bsl::move(rhs.d_element2);
        d_element3 = bsl::move(rhs.d_element3);
        d_element4 = bsl::move(rhs.d_element4);
        d_element5 = bsl::move(rhs.d_element5);
        d_element6 = bsl::move(rhs.d_element6);
        d_element7 = bsl::move(rhs.d_element7);
    }

    return *this;
}
#endif

void Sequence5::reset()
{
    BSLS_ASSERT(d_element1);
    bdlat_ValueTypeFunctions::reset(d_element1);
    bdlat_ValueTypeFunctions::reset(&d_element2);
    bdlat_ValueTypeFunctions::reset(&d_element3);
    bdlat_ValueTypeFunctions::reset(&d_element4);
    bdlat_ValueTypeFunctions::reset(&d_element5);
    bdlat_ValueTypeFunctions::reset(&d_element6);
    bdlat_ValueTypeFunctions::reset(&d_element7);
}

// ACCESSORS

bsl::ostream& Sequence5::print(
        bsl::ostream& stream,
        int           level,
        int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute("element1", this->element1());
    printer.printAttribute("element2", this->element2());
    printer.printAttribute("element3", this->element3());
    printer.printAttribute("element4", this->element4());
    printer.printAttribute("element5", this->element5());
    printer.printAttribute("element6", this->element6());
    printer.printAttribute("element7", this->element7());
    printer.end();
    return stream;
}



                              // ---------------
                              // class Sequence6
                              // ---------------

// CONSTANTS

const char Sequence6::CLASS_NAME[] = "Sequence6";

const bdlat_AttributeInfo Sequence6::ATTRIBUTE_INFO_ARRAY[] = {
    {
        ATTRIBUTE_ID_ELEMENT1,
        "element1",
        sizeof("element1") - 1,
        "",
        bdlat_FormattingMode::e_DEC
    },
    {
        ATTRIBUTE_ID_ELEMENT2,
        "element2",
        sizeof("element2") - 1,
        "",
        bdlat_FormattingMode::e_TEXT
    },
    {
        ATTRIBUTE_ID_ELEMENT3,
        "element3",
        sizeof("element3") - 1,
        "",
        bdlat_FormattingMode::e_DEC
    },
    {
        ATTRIBUTE_ID_ELEMENT4,
        "element4",
        sizeof("element4") - 1,
        "",
        bdlat_FormattingMode::e_DEC
    },
    {
        ATTRIBUTE_ID_ELEMENT5,
        "element5",
        sizeof("element5") - 1,
        "",
        bdlat_FormattingMode::e_DEC
    },
    {
        ATTRIBUTE_ID_ELEMENT6,
        "element6",
        sizeof("element6") - 1,
        "",
        bdlat_FormattingMode::e_DEC
      | bdlat_FormattingMode::e_NILLABLE
    },
    {
        ATTRIBUTE_ID_ELEMENT7,
        "element7",
        sizeof("element7") - 1,
        "",
        bdlat_FormattingMode::e_TEXT
    },
    {
        ATTRIBUTE_ID_ELEMENT8,
        "element8",
        sizeof("element8") - 1,
        "",
        bdlat_FormattingMode::e_DEC
    },
    {
        ATTRIBUTE_ID_ELEMENT9,
        "element9",
        sizeof("element9") - 1,
        "",
        bdlat_FormattingMode::e_DEC
    },
    {
        ATTRIBUTE_ID_ELEMENT10,
        "element10",
        sizeof("element10") - 1,
        "",
        bdlat_FormattingMode::e_DEC
    },
    {
        ATTRIBUTE_ID_ELEMENT11,
        "element11",
        sizeof("element11") - 1,
        "",
        bdlat_FormattingMode::e_TEXT
    },
    {
        ATTRIBUTE_ID_ELEMENT12,
        "element12",
        sizeof("element12") - 1,
        "",
        bdlat_FormattingMode::e_DEC
    },
    {
        ATTRIBUTE_ID_ELEMENT13,
        "element13",
        sizeof("element13") - 1,
        "",
        bdlat_FormattingMode::e_DEC
      | bdlat_FormattingMode::e_NILLABLE
    },
    {
        ATTRIBUTE_ID_ELEMENT14,
        "element14",
        sizeof("element14") - 1,
        "",
        bdlat_FormattingMode::e_DEC
    },
    {
        ATTRIBUTE_ID_ELEMENT15,
        "element15",
        sizeof("element15") - 1,
        "",
        bdlat_FormattingMode::e_DEC
      | bdlat_FormattingMode::e_NILLABLE
    }
};

// CLASS METHODS

const bdlat_AttributeInfo *Sequence6::lookupAttributeInfo(
        const char *name,
        int         nameLength)
{
    for (int i = 0; i < 15; ++i) {
        const bdlat_AttributeInfo& attributeInfo =
                    Sequence6::ATTRIBUTE_INFO_ARRAY[i];

        if (nameLength == attributeInfo.d_nameLength
        &&  0 == bsl::memcmp(attributeInfo.d_name_p, name, nameLength))
        {
            return &attributeInfo;
        }
    }

    return 0;
}

const bdlat_AttributeInfo *Sequence6::lookupAttributeInfo(int id)
{
    switch (id) {
      case ATTRIBUTE_ID_ELEMENT1:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT1];
      case ATTRIBUTE_ID_ELEMENT2:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT2];
      case ATTRIBUTE_ID_ELEMENT3:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT3];
      case ATTRIBUTE_ID_ELEMENT4:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT4];
      case ATTRIBUTE_ID_ELEMENT5:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT5];
      case ATTRIBUTE_ID_ELEMENT6:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT6];
      case ATTRIBUTE_ID_ELEMENT7:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT7];
      case ATTRIBUTE_ID_ELEMENT8:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT8];
      case ATTRIBUTE_ID_ELEMENT9:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT9];
      case ATTRIBUTE_ID_ELEMENT10:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT10];
      case ATTRIBUTE_ID_ELEMENT11:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT11];
      case ATTRIBUTE_ID_ELEMENT12:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT12];
      case ATTRIBUTE_ID_ELEMENT13:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT13];
      case ATTRIBUTE_ID_ELEMENT14:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT14];
      case ATTRIBUTE_ID_ELEMENT15:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT15];
      default:
        return 0;
    }
}

// CREATORS

Sequence6::Sequence6(bslma::Allocator *basicAllocator)
: d_element7(basicAllocator)
, d_element12(basicAllocator)
, d_element10(basicAllocator)
, d_element11(basicAllocator)
, d_element15(basicAllocator)
, d_element13(basicAllocator)
, d_element2(basicAllocator)
, d_element4()
, d_element8()
, d_element14(basicAllocator)
, d_element6(basicAllocator)
, d_element9()
, d_element3()
, d_element5()
, d_element1()
{
}

Sequence6::Sequence6(const Sequence6& original,
                     bslma::Allocator *basicAllocator)
: d_element7(original.d_element7, basicAllocator)
, d_element12(original.d_element12, basicAllocator)
, d_element10(original.d_element10, basicAllocator)
, d_element11(original.d_element11, basicAllocator)
, d_element15(original.d_element15, basicAllocator)
, d_element13(original.d_element13, basicAllocator)
, d_element2(original.d_element2, basicAllocator)
, d_element4(original.d_element4)
, d_element8(original.d_element8)
, d_element14(original.d_element14, basicAllocator)
, d_element6(original.d_element6, basicAllocator)
, d_element9(original.d_element9)
, d_element3(original.d_element3)
, d_element5(original.d_element5)
, d_element1(original.d_element1)
{
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
Sequence6::Sequence6(Sequence6&& original) noexcept
: d_element7(bsl::move(original.d_element7))
, d_element12(bsl::move(original.d_element12))
, d_element10(bsl::move(original.d_element10))
, d_element11(bsl::move(original.d_element11))
, d_element15(bsl::move(original.d_element15))
, d_element13(bsl::move(original.d_element13))
, d_element2(bsl::move(original.d_element2))
, d_element4(bsl::move(original.d_element4))
, d_element8(bsl::move(original.d_element8))
, d_element14(bsl::move(original.d_element14))
, d_element6(bsl::move(original.d_element6))
, d_element9(bsl::move(original.d_element9))
, d_element3(bsl::move(original.d_element3))
, d_element5(bsl::move(original.d_element5))
, d_element1(bsl::move(original.d_element1))
{
}

Sequence6::Sequence6(Sequence6&& original,
                     bslma::Allocator *basicAllocator)
: d_element7(bsl::move(original.d_element7), basicAllocator)
, d_element12(bsl::move(original.d_element12), basicAllocator)
, d_element10(bsl::move(original.d_element10), basicAllocator)
, d_element11(bsl::move(original.d_element11), basicAllocator)
, d_element15(bsl::move(original.d_element15), basicAllocator)
, d_element13(bsl::move(original.d_element13), basicAllocator)
, d_element2(bsl::move(original.d_element2), basicAllocator)
, d_element4(bsl::move(original.d_element4))
, d_element8(bsl::move(original.d_element8))
, d_element14(bsl::move(original.d_element14), basicAllocator)
, d_element6(bsl::move(original.d_element6), basicAllocator)
, d_element9(bsl::move(original.d_element9))
, d_element3(bsl::move(original.d_element3))
, d_element5(bsl::move(original.d_element5))
, d_element1(bsl::move(original.d_element1))
{
}
#endif

Sequence6::~Sequence6()
{
}

// MANIPULATORS

Sequence6&
Sequence6::operator=(const Sequence6& rhs)
{
    if (this != &rhs) {
        d_element1 = rhs.d_element1;
        d_element2 = rhs.d_element2;
        d_element3 = rhs.d_element3;
        d_element4 = rhs.d_element4;
        d_element5 = rhs.d_element5;
        d_element6 = rhs.d_element6;
        d_element7 = rhs.d_element7;
        d_element8 = rhs.d_element8;
        d_element9 = rhs.d_element9;
        d_element10 = rhs.d_element10;
        d_element11 = rhs.d_element11;
        d_element12 = rhs.d_element12;
        d_element13 = rhs.d_element13;
        d_element14 = rhs.d_element14;
        d_element15 = rhs.d_element15;
    }

    return *this;
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
Sequence6&
Sequence6::operator=(Sequence6&& rhs)
{
    if (this != &rhs) {
        d_element1 = bsl::move(rhs.d_element1);
        d_element2 = bsl::move(rhs.d_element2);
        d_element3 = bsl::move(rhs.d_element3);
        d_element4 = bsl::move(rhs.d_element4);
        d_element5 = bsl::move(rhs.d_element5);
        d_element6 = bsl::move(rhs.d_element6);
        d_element7 = bsl::move(rhs.d_element7);
        d_element8 = bsl::move(rhs.d_element8);
        d_element9 = bsl::move(rhs.d_element9);
        d_element10 = bsl::move(rhs.d_element10);
        d_element11 = bsl::move(rhs.d_element11);
        d_element12 = bsl::move(rhs.d_element12);
        d_element13 = bsl::move(rhs.d_element13);
        d_element14 = bsl::move(rhs.d_element14);
        d_element15 = bsl::move(rhs.d_element15);
    }

    return *this;
}
#endif

void Sequence6::reset()
{
    bdlat_ValueTypeFunctions::reset(&d_element1);
    bdlat_ValueTypeFunctions::reset(&d_element2);
    bdlat_ValueTypeFunctions::reset(&d_element3);
    bdlat_ValueTypeFunctions::reset(&d_element4);
    bdlat_ValueTypeFunctions::reset(&d_element5);
    bdlat_ValueTypeFunctions::reset(&d_element6);
    bdlat_ValueTypeFunctions::reset(&d_element7);
    bdlat_ValueTypeFunctions::reset(&d_element8);
    bdlat_ValueTypeFunctions::reset(&d_element9);
    bdlat_ValueTypeFunctions::reset(&d_element10);
    bdlat_ValueTypeFunctions::reset(&d_element11);
    bdlat_ValueTypeFunctions::reset(&d_element12);
    bdlat_ValueTypeFunctions::reset(&d_element13);
    bdlat_ValueTypeFunctions::reset(&d_element14);
    bdlat_ValueTypeFunctions::reset(&d_element15);
}

// ACCESSORS

bsl::ostream& Sequence6::print(
        bsl::ostream& stream,
        int           level,
        int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute("element1", this->element1());
    printer.printAttribute("element2", this->element2());
    printer.printAttribute("element3", this->element3());
    printer.printAttribute("element4", this->element4());
    printer.printAttribute("element5", (int)this->element5());
    printer.printAttribute("element6", this->element6());
    printer.printAttribute("element7", this->element7());
    printer.printAttribute("element8", this->element8());
    printer.printAttribute("element9", this->element9());
    printer.printAttribute("element10", this->element10());
    printer.printAttribute("element11", this->element11());
    printer.printAttribute("element12", this->element12());
    printer.printAttribute("element13", this->element13());
    printer.printAttribute("element14", this->element14());
    printer.printAttribute("element15", this->element15());
    printer.end();
    return stream;
}



                               // -------------
                               // class Choice3
                               // -------------

// CONSTANTS

const char Choice3::CLASS_NAME[] = "Choice3";

const bdlat_SelectionInfo Choice3::SELECTION_INFO_ARRAY[] = {
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

const bdlat_SelectionInfo *Choice3::lookupSelectionInfo(
        const char *name,
        int         nameLength)
{
    for (int i = 0; i < 4; ++i) {
        const bdlat_SelectionInfo& selectionInfo =
                    Choice3::SELECTION_INFO_ARRAY[i];

        if (nameLength == selectionInfo.d_nameLength
        &&  0 == bsl::memcmp(selectionInfo.d_name_p, name, nameLength))
        {
            return &selectionInfo;
        }
    }

    return 0;
}

const bdlat_SelectionInfo *Choice3::lookupSelectionInfo(int id)
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

Choice3::Choice3(
    const Choice3& original,
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
Choice3::Choice3(Choice3&& original) noexcept
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

Choice3::Choice3(
    Choice3&& original,
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

Choice3&
Choice3::operator=(const Choice3& rhs)
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
Choice3&
Choice3::operator=(Choice3&& rhs)
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

void Choice3::reset()
{
    switch (d_selectionId) {
      case SELECTION_ID_SELECTION1: {
        d_selection1.object().~Sequence6();
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

int Choice3::makeSelection(int selectionId)
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

int Choice3::makeSelection(const char *name, int nameLength)
{
    const bdlat_SelectionInfo *selectionInfo =
                                         lookupSelectionInfo(name, nameLength);
    if (0 == selectionInfo) {
       return -1;
    }

    return makeSelection(selectionInfo->d_id);
}

Sequence6& Choice3::makeSelection1()
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

Sequence6& Choice3::makeSelection1(const Sequence6& value)
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
Sequence6& Choice3::makeSelection1(Sequence6&& value)
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

unsigned char& Choice3::makeSelection2()
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

unsigned char& Choice3::makeSelection2(unsigned char value)
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

s_baltst::CustomString& Choice3::makeSelection3()
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

s_baltst::CustomString& Choice3::makeSelection3(const s_baltst::CustomString& value)
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
s_baltst::CustomString& Choice3::makeSelection3(s_baltst::CustomString&& value)
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

s_baltst::CustomInt& Choice3::makeSelection4()
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

s_baltst::CustomInt& Choice3::makeSelection4(const s_baltst::CustomInt& value)
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
s_baltst::CustomInt& Choice3::makeSelection4(s_baltst::CustomInt&& value)
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

bsl::ostream& Choice3::print(
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


const char *Choice3::selectionName() const
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

                               // -------------
                               // class Choice1
                               // -------------

// CONSTANTS

const char Choice1::CLASS_NAME[] = "Choice1";

const bdlat_SelectionInfo Choice1::SELECTION_INFO_ARRAY[] = {
    {
        SELECTION_ID_SELECTION1,
        "selection1",
        sizeof("selection1") - 1,
        "",
        bdlat_FormattingMode::e_DEC
    },
    {
        SELECTION_ID_SELECTION2,
        "selection2",
        sizeof("selection2") - 1,
        "",
        bdlat_FormattingMode::e_DEFAULT
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
    }
};

// CLASS METHODS

const bdlat_SelectionInfo *Choice1::lookupSelectionInfo(
        const char *name,
        int         nameLength)
{
    for (int i = 0; i < 4; ++i) {
        const bdlat_SelectionInfo& selectionInfo =
                    Choice1::SELECTION_INFO_ARRAY[i];

        if (nameLength == selectionInfo.d_nameLength
        &&  0 == bsl::memcmp(selectionInfo.d_name_p, name, nameLength))
        {
            return &selectionInfo;
        }
    }

    return 0;
}

const bdlat_SelectionInfo *Choice1::lookupSelectionInfo(int id)
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

Choice1::Choice1(
    const Choice1& original,
    bslma::Allocator *basicAllocator)
: d_selectionId(original.d_selectionId)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    switch (d_selectionId) {
      case SELECTION_ID_SELECTION1: {
        new (d_selection1.buffer())
            int(original.d_selection1.object());
      } break;
      case SELECTION_ID_SELECTION2: {
        new (d_selection2.buffer())
            double(original.d_selection2.object());
      } break;
      case SELECTION_ID_SELECTION3: {
        d_selection3 = new (*d_allocator_p)
                Sequence4(*original.d_selection3, d_allocator_p);
      } break;
      case SELECTION_ID_SELECTION4: {
        d_selection4 = new (*d_allocator_p)
                Choice2(*original.d_selection4, d_allocator_p);
      } break;
      default:
        BSLS_ASSERT(SELECTION_ID_UNDEFINED == d_selectionId);
    }
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
Choice1::Choice1(Choice1&& original) noexcept
: d_selectionId(original.d_selectionId)
, d_allocator_p(original.d_allocator_p)
{
    switch (d_selectionId) {
      case SELECTION_ID_SELECTION1: {
        new (d_selection1.buffer())
            int(bsl::move(original.d_selection1.object()));
      } break;
      case SELECTION_ID_SELECTION2: {
        new (d_selection2.buffer())
            double(bsl::move(original.d_selection2.object()));
      } break;
      case SELECTION_ID_SELECTION3: {
        d_selection3 = original.d_selection3;
        original.d_selectionId = SELECTION_ID_UNDEFINED;
      } break;
      case SELECTION_ID_SELECTION4: {
        d_selection4 = original.d_selection4;
        original.d_selectionId = SELECTION_ID_UNDEFINED;
      } break;
      default:
        BSLS_ASSERT(SELECTION_ID_UNDEFINED == d_selectionId);
    }
}

Choice1::Choice1(
    Choice1&& original,
    bslma::Allocator *basicAllocator)
: d_selectionId(original.d_selectionId)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    switch (d_selectionId) {
      case SELECTION_ID_SELECTION1: {
        new (d_selection1.buffer())
            int(bsl::move(original.d_selection1.object()));
      } break;
      case SELECTION_ID_SELECTION2: {
        new (d_selection2.buffer())
            double(bsl::move(original.d_selection2.object()));
      } break;
      case SELECTION_ID_SELECTION3: {
        if (d_allocator_p == original.d_allocator_p) {
            d_selection3 = original.d_selection3;
            original.d_selectionId = SELECTION_ID_UNDEFINED;
        }
        else {
            d_selection3 = new (*d_allocator_p)
                    Sequence4(bsl::move(*original.d_selection3), d_allocator_p);
        }
      } break;
      case SELECTION_ID_SELECTION4: {
        if (d_allocator_p == original.d_allocator_p) {
            d_selection4 = original.d_selection4;
            original.d_selectionId = SELECTION_ID_UNDEFINED;
        }
        else {
            d_selection4 = new (*d_allocator_p)
                    Choice2(bsl::move(*original.d_selection4), d_allocator_p);
        }
      } break;
      default:
        BSLS_ASSERT(SELECTION_ID_UNDEFINED == d_selectionId);
    }
}
#endif

// MANIPULATORS

Choice1&
Choice1::operator=(const Choice1& rhs)
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
            makeSelection3(*rhs.d_selection3);
          } break;
          case SELECTION_ID_SELECTION4: {
            makeSelection4(*rhs.d_selection4);
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
Choice1&
Choice1::operator=(Choice1&& rhs)
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
            if (d_allocator_p == rhs.d_allocator_p) {
                reset();
                d_selection3 = rhs.d_selection3;
                d_selectionId = SELECTION_ID_SELECTION3;
                rhs.d_selectionId = SELECTION_ID_UNDEFINED;
            }
            else {
                makeSelection3(bsl::move(*rhs.d_selection3));
            }
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
          default:
            BSLS_ASSERT(SELECTION_ID_UNDEFINED == rhs.d_selectionId);
            reset();
        }
    }

    return *this;
}
#endif

void Choice1::reset()
{
    switch (d_selectionId) {
      case SELECTION_ID_SELECTION1: {
        // no destruction required
      } break;
      case SELECTION_ID_SELECTION2: {
        // no destruction required
      } break;
      case SELECTION_ID_SELECTION3: {
        d_allocator_p->deleteObject(d_selection3);
      } break;
      case SELECTION_ID_SELECTION4: {
        d_allocator_p->deleteObject(d_selection4);
      } break;
      default:
        BSLS_ASSERT(SELECTION_ID_UNDEFINED == d_selectionId);
    }

    d_selectionId = SELECTION_ID_UNDEFINED;
}

int Choice1::makeSelection(int selectionId)
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

int Choice1::makeSelection(const char *name, int nameLength)
{
    const bdlat_SelectionInfo *selectionInfo =
                                         lookupSelectionInfo(name, nameLength);
    if (0 == selectionInfo) {
       return -1;
    }

    return makeSelection(selectionInfo->d_id);
}

int& Choice1::makeSelection1()
{
    if (SELECTION_ID_SELECTION1 == d_selectionId) {
        bdlat_ValueTypeFunctions::reset(&d_selection1.object());
    }
    else {
        reset();
        new (d_selection1.buffer())
            int();
        d_selectionId = SELECTION_ID_SELECTION1;
    }

    return d_selection1.object();
}

int& Choice1::makeSelection1(int value)
{
    if (SELECTION_ID_SELECTION1 == d_selectionId) {
        d_selection1.object() = value;
    }
    else {
        reset();
        new (d_selection1.buffer())
                int(value);
        d_selectionId = SELECTION_ID_SELECTION1;
    }

    return d_selection1.object();
}

double& Choice1::makeSelection2()
{
    if (SELECTION_ID_SELECTION2 == d_selectionId) {
        bdlat_ValueTypeFunctions::reset(&d_selection2.object());
    }
    else {
        reset();
        new (d_selection2.buffer())
            double();
        d_selectionId = SELECTION_ID_SELECTION2;
    }

    return d_selection2.object();
}

double& Choice1::makeSelection2(double value)
{
    if (SELECTION_ID_SELECTION2 == d_selectionId) {
        d_selection2.object() = value;
    }
    else {
        reset();
        new (d_selection2.buffer())
                double(value);
        d_selectionId = SELECTION_ID_SELECTION2;
    }

    return d_selection2.object();
}

Sequence4& Choice1::makeSelection3()
{
    if (SELECTION_ID_SELECTION3 == d_selectionId) {
        bdlat_ValueTypeFunctions::reset(d_selection3);
    }
    else {
        reset();
        d_selection3 = new (*d_allocator_p)
                Sequence4(d_allocator_p);
        d_selectionId = SELECTION_ID_SELECTION3;
    }

    return *d_selection3;
}

Sequence4& Choice1::makeSelection3(const Sequence4& value)
{
    if (SELECTION_ID_SELECTION3 == d_selectionId) {
        *d_selection3 = value;
    }
    else {
        reset();
        d_selection3 = new (*d_allocator_p)
                Sequence4(value, d_allocator_p);
        d_selectionId = SELECTION_ID_SELECTION3;
    }

    return *d_selection3;
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
Sequence4& Choice1::makeSelection3(Sequence4&& value)
{
    if (SELECTION_ID_SELECTION3 == d_selectionId) {
        *d_selection3 = bsl::move(value);
    }
    else {
        reset();
        d_selection3 = new (*d_allocator_p)
                Sequence4(bsl::move(value), d_allocator_p);
        d_selectionId = SELECTION_ID_SELECTION3;
    }

    return *d_selection3;
}
#endif

Choice2& Choice1::makeSelection4()
{
    if (SELECTION_ID_SELECTION4 == d_selectionId) {
        bdlat_ValueTypeFunctions::reset(d_selection4);
    }
    else {
        reset();
        d_selection4 = new (*d_allocator_p)
                Choice2(d_allocator_p);
        d_selectionId = SELECTION_ID_SELECTION4;
    }

    return *d_selection4;
}

Choice2& Choice1::makeSelection4(const Choice2& value)
{
    if (SELECTION_ID_SELECTION4 == d_selectionId) {
        *d_selection4 = value;
    }
    else {
        reset();
        d_selection4 = new (*d_allocator_p)
                Choice2(value, d_allocator_p);
        d_selectionId = SELECTION_ID_SELECTION4;
    }

    return *d_selection4;
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
Choice2& Choice1::makeSelection4(Choice2&& value)
{
    if (SELECTION_ID_SELECTION4 == d_selectionId) {
        *d_selection4 = bsl::move(value);
    }
    else {
        reset();
        d_selection4 = new (*d_allocator_p)
                Choice2(bsl::move(value), d_allocator_p);
        d_selectionId = SELECTION_ID_SELECTION4;
    }

    return *d_selection4;
}
#endif

// ACCESSORS

bsl::ostream& Choice1::print(
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
        printer.printAttribute("selection2", d_selection2.object());
      }  break;
      case SELECTION_ID_SELECTION3: {
        printer.printAttribute("selection3", *d_selection3);
      }  break;
      case SELECTION_ID_SELECTION4: {
        printer.printAttribute("selection4", *d_selection4);
      }  break;
      default:
        stream << "SELECTION UNDEFINED\n";
    }
    printer.end();
    return stream;
}


const char *Choice1::selectionName() const
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

                               // -------------
                               // class Choice2
                               // -------------

// CONSTANTS

const char Choice2::CLASS_NAME[] = "Choice2";

const bdlat_SelectionInfo Choice2::SELECTION_INFO_ARRAY[] = {
    {
        SELECTION_ID_SELECTION1,
        "selection1",
        sizeof("selection1") - 1,
        "",
        bdlat_FormattingMode::e_TEXT
    },
    {
        SELECTION_ID_SELECTION2,
        "selection2",
        sizeof("selection2") - 1,
        "",
        bdlat_FormattingMode::e_TEXT
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
        bdlat_FormattingMode::e_DEC
    }
};

// CLASS METHODS

const bdlat_SelectionInfo *Choice2::lookupSelectionInfo(
        const char *name,
        int         nameLength)
{
    for (int i = 0; i < 4; ++i) {
        const bdlat_SelectionInfo& selectionInfo =
                    Choice2::SELECTION_INFO_ARRAY[i];

        if (nameLength == selectionInfo.d_nameLength
        &&  0 == bsl::memcmp(selectionInfo.d_name_p, name, nameLength))
        {
            return &selectionInfo;
        }
    }

    return 0;
}

const bdlat_SelectionInfo *Choice2::lookupSelectionInfo(int id)
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

Choice2::Choice2(
    const Choice2& original,
    bslma::Allocator *basicAllocator)
: d_selectionId(original.d_selectionId)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    switch (d_selectionId) {
      case SELECTION_ID_SELECTION1: {
        new (d_selection1.buffer())
            bool(original.d_selection1.object());
      } break;
      case SELECTION_ID_SELECTION2: {
        new (d_selection2.buffer())
            bsl::string(
                original.d_selection2.object(), d_allocator_p);
      } break;
      case SELECTION_ID_SELECTION3: {
        d_selection3 = new (*d_allocator_p)
                Choice1(*original.d_selection3, d_allocator_p);
      } break;
      case SELECTION_ID_SELECTION4: {
        new (d_selection4.buffer())
            unsigned int(original.d_selection4.object());
      } break;
      default:
        BSLS_ASSERT(SELECTION_ID_UNDEFINED == d_selectionId);
    }
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
Choice2::Choice2(Choice2&& original) noexcept
: d_selectionId(original.d_selectionId)
, d_allocator_p(original.d_allocator_p)
{
    switch (d_selectionId) {
      case SELECTION_ID_SELECTION1: {
        new (d_selection1.buffer())
            bool(bsl::move(original.d_selection1.object()));
      } break;
      case SELECTION_ID_SELECTION2: {
        new (d_selection2.buffer())
            bsl::string(
                bsl::move(original.d_selection2.object()), d_allocator_p);
      } break;
      case SELECTION_ID_SELECTION3: {
        d_selection3 = original.d_selection3;
        original.d_selectionId = SELECTION_ID_UNDEFINED;
      } break;
      case SELECTION_ID_SELECTION4: {
        new (d_selection4.buffer())
            unsigned int(bsl::move(original.d_selection4.object()));
      } break;
      default:
        BSLS_ASSERT(SELECTION_ID_UNDEFINED == d_selectionId);
    }
}

Choice2::Choice2(
    Choice2&& original,
    bslma::Allocator *basicAllocator)
: d_selectionId(original.d_selectionId)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    switch (d_selectionId) {
      case SELECTION_ID_SELECTION1: {
        new (d_selection1.buffer())
            bool(bsl::move(original.d_selection1.object()));
      } break;
      case SELECTION_ID_SELECTION2: {
        new (d_selection2.buffer())
            bsl::string(
                bsl::move(original.d_selection2.object()), d_allocator_p);
      } break;
      case SELECTION_ID_SELECTION3: {
        if (d_allocator_p == original.d_allocator_p) {
            d_selection3 = original.d_selection3;
            original.d_selectionId = SELECTION_ID_UNDEFINED;
        }
        else {
            d_selection3 = new (*d_allocator_p)
                    Choice1(bsl::move(*original.d_selection3), d_allocator_p);
        }
      } break;
      case SELECTION_ID_SELECTION4: {
        new (d_selection4.buffer())
            unsigned int(bsl::move(original.d_selection4.object()));
      } break;
      default:
        BSLS_ASSERT(SELECTION_ID_UNDEFINED == d_selectionId);
    }
}
#endif

// MANIPULATORS

Choice2&
Choice2::operator=(const Choice2& rhs)
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
            makeSelection3(*rhs.d_selection3);
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
Choice2&
Choice2::operator=(Choice2&& rhs)
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
            if (d_allocator_p == rhs.d_allocator_p) {
                reset();
                d_selection3 = rhs.d_selection3;
                d_selectionId = SELECTION_ID_SELECTION3;
                rhs.d_selectionId = SELECTION_ID_UNDEFINED;
            }
            else {
                makeSelection3(bsl::move(*rhs.d_selection3));
            }
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

void Choice2::reset()
{
    switch (d_selectionId) {
      case SELECTION_ID_SELECTION1: {
        // no destruction required
      } break;
      case SELECTION_ID_SELECTION2: {
        typedef bsl::string Type;
        d_selection2.object().~Type();
      } break;
      case SELECTION_ID_SELECTION3: {
        d_allocator_p->deleteObject(d_selection3);
      } break;
      case SELECTION_ID_SELECTION4: {
        // no destruction required
      } break;
      default:
        BSLS_ASSERT(SELECTION_ID_UNDEFINED == d_selectionId);
    }

    d_selectionId = SELECTION_ID_UNDEFINED;
}

int Choice2::makeSelection(int selectionId)
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

int Choice2::makeSelection(const char *name, int nameLength)
{
    const bdlat_SelectionInfo *selectionInfo =
                                         lookupSelectionInfo(name, nameLength);
    if (0 == selectionInfo) {
       return -1;
    }

    return makeSelection(selectionInfo->d_id);
}

bool& Choice2::makeSelection1()
{
    if (SELECTION_ID_SELECTION1 == d_selectionId) {
        bdlat_ValueTypeFunctions::reset(&d_selection1.object());
    }
    else {
        reset();
        new (d_selection1.buffer())
            bool();
        d_selectionId = SELECTION_ID_SELECTION1;
    }

    return d_selection1.object();
}

bool& Choice2::makeSelection1(bool value)
{
    if (SELECTION_ID_SELECTION1 == d_selectionId) {
        d_selection1.object() = value;
    }
    else {
        reset();
        new (d_selection1.buffer())
                bool(value);
        d_selectionId = SELECTION_ID_SELECTION1;
    }

    return d_selection1.object();
}

bsl::string& Choice2::makeSelection2()
{
    if (SELECTION_ID_SELECTION2 == d_selectionId) {
        bdlat_ValueTypeFunctions::reset(&d_selection2.object());
    }
    else {
        reset();
        new (d_selection2.buffer())
                bsl::string(d_allocator_p);
        d_selectionId = SELECTION_ID_SELECTION2;
    }

    return d_selection2.object();
}

bsl::string& Choice2::makeSelection2(const bsl::string& value)
{
    if (SELECTION_ID_SELECTION2 == d_selectionId) {
        d_selection2.object() = value;
    }
    else {
        reset();
        new (d_selection2.buffer())
                bsl::string(value, d_allocator_p);
        d_selectionId = SELECTION_ID_SELECTION2;
    }

    return d_selection2.object();
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
bsl::string& Choice2::makeSelection2(bsl::string&& value)
{
    if (SELECTION_ID_SELECTION2 == d_selectionId) {
        d_selection2.object() = bsl::move(value);
    }
    else {
        reset();
        new (d_selection2.buffer())
                bsl::string(bsl::move(value), d_allocator_p);
        d_selectionId = SELECTION_ID_SELECTION2;
    }

    return d_selection2.object();
}
#endif

Choice1& Choice2::makeSelection3()
{
    if (SELECTION_ID_SELECTION3 == d_selectionId) {
        bdlat_ValueTypeFunctions::reset(d_selection3);
    }
    else {
        reset();
        d_selection3 = new (*d_allocator_p)
                Choice1(d_allocator_p);
        d_selectionId = SELECTION_ID_SELECTION3;
    }

    return *d_selection3;
}

Choice1& Choice2::makeSelection3(const Choice1& value)
{
    if (SELECTION_ID_SELECTION3 == d_selectionId) {
        *d_selection3 = value;
    }
    else {
        reset();
        d_selection3 = new (*d_allocator_p)
                Choice1(value, d_allocator_p);
        d_selectionId = SELECTION_ID_SELECTION3;
    }

    return *d_selection3;
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
Choice1& Choice2::makeSelection3(Choice1&& value)
{
    if (SELECTION_ID_SELECTION3 == d_selectionId) {
        *d_selection3 = bsl::move(value);
    }
    else {
        reset();
        d_selection3 = new (*d_allocator_p)
                Choice1(bsl::move(value), d_allocator_p);
        d_selectionId = SELECTION_ID_SELECTION3;
    }

    return *d_selection3;
}
#endif

unsigned int& Choice2::makeSelection4()
{
    if (SELECTION_ID_SELECTION4 == d_selectionId) {
        bdlat_ValueTypeFunctions::reset(&d_selection4.object());
    }
    else {
        reset();
        new (d_selection4.buffer())
            unsigned int();
        d_selectionId = SELECTION_ID_SELECTION4;
    }

    return d_selection4.object();
}

unsigned int& Choice2::makeSelection4(unsigned int value)
{
    if (SELECTION_ID_SELECTION4 == d_selectionId) {
        d_selection4.object() = value;
    }
    else {
        reset();
        new (d_selection4.buffer())
                unsigned int(value);
        d_selectionId = SELECTION_ID_SELECTION4;
    }

    return d_selection4.object();
}

// ACCESSORS

bsl::ostream& Choice2::print(
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
        printer.printAttribute("selection2", d_selection2.object());
      }  break;
      case SELECTION_ID_SELECTION3: {
        printer.printAttribute("selection3", *d_selection3);
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


const char *Choice2::selectionName() const
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

                              // ---------------
                              // class Sequence4
                              // ---------------

// CONSTANTS

const char Sequence4::CLASS_NAME[] = "Sequence4";

const bdlat_AttributeInfo Sequence4::ATTRIBUTE_INFO_ARRAY[] = {
    {
        ATTRIBUTE_ID_ELEMENT1,
        "element1",
        sizeof("element1") - 1,
        "",
        bdlat_FormattingMode::e_DEFAULT
    },
    {
        ATTRIBUTE_ID_ELEMENT2,
        "element2",
        sizeof("element2") - 1,
        "",
        bdlat_FormattingMode::e_DEFAULT
    },
    {
        ATTRIBUTE_ID_ELEMENT3,
        "element3",
        sizeof("element3") - 1,
        "",
        bdlat_FormattingMode::e_HEX
    },
    {
        ATTRIBUTE_ID_ELEMENT4,
        "element4",
        sizeof("element4") - 1,
        "",
        bdlat_FormattingMode::e_DEC
    },
    {
        ATTRIBUTE_ID_ELEMENT5,
        "element5",
        sizeof("element5") - 1,
        "",
        bdlat_FormattingMode::e_DEFAULT
    },
    {
        ATTRIBUTE_ID_ELEMENT6,
        "element6",
        sizeof("element6") - 1,
        "",
        bdlat_FormattingMode::e_TEXT
    },
    {
        ATTRIBUTE_ID_ELEMENT7,
        "element7",
        sizeof("element7") - 1,
        "",
        bdlat_FormattingMode::e_DEFAULT
    },
    {
        ATTRIBUTE_ID_ELEMENT8,
        "element8",
        sizeof("element8") - 1,
        "",
        bdlat_FormattingMode::e_TEXT
    },
    {
        ATTRIBUTE_ID_ELEMENT9,
        "element9",
        sizeof("element9") - 1,
        "",
        bdlat_FormattingMode::e_TEXT
    },
    {
        ATTRIBUTE_ID_ELEMENT10,
        "element10",
        sizeof("element10") - 1,
        "",
        bdlat_FormattingMode::e_DEFAULT
    },
    {
        ATTRIBUTE_ID_ELEMENT11,
        "element11",
        sizeof("element11") - 1,
        "",
        bdlat_FormattingMode::e_HEX
    },
    {
        ATTRIBUTE_ID_ELEMENT12,
        "element12",
        sizeof("element12") - 1,
        "",
        bdlat_FormattingMode::e_DEC
    },
    {
        ATTRIBUTE_ID_ELEMENT13,
        "element13",
        sizeof("element13") - 1,
        "",
        bdlat_FormattingMode::e_DEFAULT
    },
    {
        ATTRIBUTE_ID_ELEMENT14,
        "element14",
        sizeof("element14") - 1,
        "",
        bdlat_FormattingMode::e_TEXT
    },
    {
        ATTRIBUTE_ID_ELEMENT15,
        "element15",
        sizeof("element15") - 1,
        "",
        bdlat_FormattingMode::e_DEFAULT
    },
    {
        ATTRIBUTE_ID_ELEMENT16,
        "element16",
        sizeof("element16") - 1,
        "",
        bdlat_FormattingMode::e_HEX
    },
    {
        ATTRIBUTE_ID_ELEMENT17,
        "element17",
        sizeof("element17") - 1,
        "",
        bdlat_FormattingMode::e_DEC
    },
    {
        ATTRIBUTE_ID_ELEMENT18,
        "element18",
        sizeof("element18") - 1,
        "",
        bdlat_FormattingMode::e_DEFAULT
    },
    {
        ATTRIBUTE_ID_ELEMENT19,
        "element19",
        sizeof("element19") - 1,
        "",
        bdlat_FormattingMode::e_TEXT
    }
};

// CLASS METHODS

const bdlat_AttributeInfo *Sequence4::lookupAttributeInfo(
        const char *name,
        int         nameLength)
{
    for (int i = 0; i < 19; ++i) {
        const bdlat_AttributeInfo& attributeInfo =
                    Sequence4::ATTRIBUTE_INFO_ARRAY[i];

        if (nameLength == attributeInfo.d_nameLength
        &&  0 == bsl::memcmp(attributeInfo.d_name_p, name, nameLength))
        {
            return &attributeInfo;
        }
    }

    return 0;
}

const bdlat_AttributeInfo *Sequence4::lookupAttributeInfo(int id)
{
    switch (id) {
      case ATTRIBUTE_ID_ELEMENT1:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT1];
      case ATTRIBUTE_ID_ELEMENT2:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT2];
      case ATTRIBUTE_ID_ELEMENT3:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT3];
      case ATTRIBUTE_ID_ELEMENT4:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT4];
      case ATTRIBUTE_ID_ELEMENT5:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT5];
      case ATTRIBUTE_ID_ELEMENT6:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT6];
      case ATTRIBUTE_ID_ELEMENT7:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT7];
      case ATTRIBUTE_ID_ELEMENT8:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT8];
      case ATTRIBUTE_ID_ELEMENT9:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT9];
      case ATTRIBUTE_ID_ELEMENT10:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT10];
      case ATTRIBUTE_ID_ELEMENT11:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT11];
      case ATTRIBUTE_ID_ELEMENT12:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT12];
      case ATTRIBUTE_ID_ELEMENT13:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT13];
      case ATTRIBUTE_ID_ELEMENT14:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT14];
      case ATTRIBUTE_ID_ELEMENT15:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT15];
      case ATTRIBUTE_ID_ELEMENT16:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT16];
      case ATTRIBUTE_ID_ELEMENT17:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT17];
      case ATTRIBUTE_ID_ELEMENT18:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT18];
      case ATTRIBUTE_ID_ELEMENT19:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT19];
      default:
        return 0;
    }
}

// CREATORS

Sequence4::Sequence4(bslma::Allocator *basicAllocator)
: d_element10()
, d_element19(basicAllocator)
, d_element17(basicAllocator)
, d_element15(basicAllocator)
, d_element11(basicAllocator)
, d_element14(basicAllocator)
, d_element18(basicAllocator)
, d_element1(basicAllocator)
, d_element2(basicAllocator)
, d_element9(basicAllocator)
, d_element6(basicAllocator)
, d_element3(basicAllocator)
, d_element16(basicAllocator)
, d_element5()
, d_element13(static_cast<s_baltst::Enumerated::Value>(0))
, d_element12()
, d_element7()
, d_element4()
, d_element8()
{
}

Sequence4::Sequence4(const Sequence4& original,
                     bslma::Allocator *basicAllocator)
: d_element10(original.d_element10)
, d_element19(original.d_element19, basicAllocator)
, d_element17(original.d_element17, basicAllocator)
, d_element15(original.d_element15, basicAllocator)
, d_element11(original.d_element11, basicAllocator)
, d_element14(original.d_element14, basicAllocator)
, d_element18(original.d_element18, basicAllocator)
, d_element1(original.d_element1, basicAllocator)
, d_element2(original.d_element2, basicAllocator)
, d_element9(original.d_element9, basicAllocator)
, d_element6(original.d_element6, basicAllocator)
, d_element3(original.d_element3, basicAllocator)
, d_element16(original.d_element16, basicAllocator)
, d_element5(original.d_element5)
, d_element13(original.d_element13)
, d_element12(original.d_element12)
, d_element7(original.d_element7)
, d_element4(original.d_element4)
, d_element8(original.d_element8)
{
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
Sequence4::Sequence4(Sequence4&& original) noexcept
: d_element10(bsl::move(original.d_element10))
, d_element19(bsl::move(original.d_element19))
, d_element17(bsl::move(original.d_element17))
, d_element15(bsl::move(original.d_element15))
, d_element11(bsl::move(original.d_element11))
, d_element14(bsl::move(original.d_element14))
, d_element18(bsl::move(original.d_element18))
, d_element1(bsl::move(original.d_element1))
, d_element2(bsl::move(original.d_element2))
, d_element9(bsl::move(original.d_element9))
, d_element6(bsl::move(original.d_element6))
, d_element3(bsl::move(original.d_element3))
, d_element16(bsl::move(original.d_element16))
, d_element5(bsl::move(original.d_element5))
, d_element13(bsl::move(original.d_element13))
, d_element12(bsl::move(original.d_element12))
, d_element7(bsl::move(original.d_element7))
, d_element4(bsl::move(original.d_element4))
, d_element8(bsl::move(original.d_element8))
{
}

Sequence4::Sequence4(Sequence4&& original,
                     bslma::Allocator *basicAllocator)
: d_element10(bsl::move(original.d_element10))
, d_element19(bsl::move(original.d_element19), basicAllocator)
, d_element17(bsl::move(original.d_element17), basicAllocator)
, d_element15(bsl::move(original.d_element15), basicAllocator)
, d_element11(bsl::move(original.d_element11), basicAllocator)
, d_element14(bsl::move(original.d_element14), basicAllocator)
, d_element18(bsl::move(original.d_element18), basicAllocator)
, d_element1(bsl::move(original.d_element1), basicAllocator)
, d_element2(bsl::move(original.d_element2), basicAllocator)
, d_element9(bsl::move(original.d_element9), basicAllocator)
, d_element6(bsl::move(original.d_element6), basicAllocator)
, d_element3(bsl::move(original.d_element3), basicAllocator)
, d_element16(bsl::move(original.d_element16), basicAllocator)
, d_element5(bsl::move(original.d_element5))
, d_element13(bsl::move(original.d_element13))
, d_element12(bsl::move(original.d_element12))
, d_element7(bsl::move(original.d_element7))
, d_element4(bsl::move(original.d_element4))
, d_element8(bsl::move(original.d_element8))
{
}
#endif

Sequence4::~Sequence4()
{
}

// MANIPULATORS

Sequence4&
Sequence4::operator=(const Sequence4& rhs)
{
    if (this != &rhs) {
        d_element1 = rhs.d_element1;
        d_element2 = rhs.d_element2;
        d_element3 = rhs.d_element3;
        d_element4 = rhs.d_element4;
        d_element5 = rhs.d_element5;
        d_element6 = rhs.d_element6;
        d_element7 = rhs.d_element7;
        d_element8 = rhs.d_element8;
        d_element9 = rhs.d_element9;
        d_element10 = rhs.d_element10;
        d_element11 = rhs.d_element11;
        d_element12 = rhs.d_element12;
        d_element13 = rhs.d_element13;
        d_element14 = rhs.d_element14;
        d_element15 = rhs.d_element15;
        d_element16 = rhs.d_element16;
        d_element17 = rhs.d_element17;
        d_element18 = rhs.d_element18;
        d_element19 = rhs.d_element19;
    }

    return *this;
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
Sequence4&
Sequence4::operator=(Sequence4&& rhs)
{
    if (this != &rhs) {
        d_element1 = bsl::move(rhs.d_element1);
        d_element2 = bsl::move(rhs.d_element2);
        d_element3 = bsl::move(rhs.d_element3);
        d_element4 = bsl::move(rhs.d_element4);
        d_element5 = bsl::move(rhs.d_element5);
        d_element6 = bsl::move(rhs.d_element6);
        d_element7 = bsl::move(rhs.d_element7);
        d_element8 = bsl::move(rhs.d_element8);
        d_element9 = bsl::move(rhs.d_element9);
        d_element10 = bsl::move(rhs.d_element10);
        d_element11 = bsl::move(rhs.d_element11);
        d_element12 = bsl::move(rhs.d_element12);
        d_element13 = bsl::move(rhs.d_element13);
        d_element14 = bsl::move(rhs.d_element14);
        d_element15 = bsl::move(rhs.d_element15);
        d_element16 = bsl::move(rhs.d_element16);
        d_element17 = bsl::move(rhs.d_element17);
        d_element18 = bsl::move(rhs.d_element18);
        d_element19 = bsl::move(rhs.d_element19);
    }

    return *this;
}
#endif

void Sequence4::reset()
{
    bdlat_ValueTypeFunctions::reset(&d_element1);
    bdlat_ValueTypeFunctions::reset(&d_element2);
    bdlat_ValueTypeFunctions::reset(&d_element3);
    bdlat_ValueTypeFunctions::reset(&d_element4);
    bdlat_ValueTypeFunctions::reset(&d_element5);
    bdlat_ValueTypeFunctions::reset(&d_element6);
    bdlat_ValueTypeFunctions::reset(&d_element7);
    bdlat_ValueTypeFunctions::reset(&d_element8);
    bdlat_ValueTypeFunctions::reset(&d_element9);
    bdlat_ValueTypeFunctions::reset(&d_element10);
    bdlat_ValueTypeFunctions::reset(&d_element11);
    bdlat_ValueTypeFunctions::reset(&d_element12);
    bdlat_ValueTypeFunctions::reset(&d_element13);
    bdlat_ValueTypeFunctions::reset(&d_element14);
    bdlat_ValueTypeFunctions::reset(&d_element15);
    bdlat_ValueTypeFunctions::reset(&d_element16);
    bdlat_ValueTypeFunctions::reset(&d_element17);
    bdlat_ValueTypeFunctions::reset(&d_element18);
    bdlat_ValueTypeFunctions::reset(&d_element19);
}

// ACCESSORS

bsl::ostream& Sequence4::print(
        bsl::ostream& stream,
        int           level,
        int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute("element1", this->element1());
    printer.printAttribute("element2", this->element2());
    printer.printAttribute("element3", this->element3());
    printer.printAttribute("element4", this->element4());
    printer.printAttribute("element5", this->element5());
    printer.printAttribute("element6", this->element6());
    printer.printAttribute("element7", this->element7());
    printer.printAttribute("element8", this->element8());
    printer.printAttribute("element9", this->element9());
    printer.printAttribute("element10", this->element10());
    {
        bool multilineFlag = (0 <= level);
        bdlb::Print::indent(stream, level + 1, spacesPerLevel);
        stream << (multilineFlag ? "" : " ");
        stream << "element11 = [ ";
        bdlb::Print::singleLineHexDump(
            stream, this->element11().begin(), this->element11().end());
        stream << " ]" << (multilineFlag ? "\n" : "");
    }
    printer.printAttribute("element12", this->element12());
    printer.printAttribute("element13", this->element13());
    printer.printAttribute("element14", this->element14());
    printer.printAttribute("element15", this->element15());
    printer.printAttribute("element16", this->element16());
    printer.printAttribute("element17", this->element17());
    printer.printAttribute("element18", this->element18());
    printer.printAttribute("element19", this->element19());
    printer.end();
    return stream;
}



                              // ---------------
                              // class Sequence1
                              // ---------------

// CONSTANTS

const char Sequence1::CLASS_NAME[] = "Sequence1";

const bdlat_AttributeInfo Sequence1::ATTRIBUTE_INFO_ARRAY[] = {
    {
        ATTRIBUTE_ID_ELEMENT1,
        "element1",
        sizeof("element1") - 1,
        "",
        bdlat_FormattingMode::e_DEFAULT
    },
    {
        ATTRIBUTE_ID_ELEMENT2,
        "element2",
        sizeof("element2") - 1,
        "",
        bdlat_FormattingMode::e_DEFAULT
    },
    {
        ATTRIBUTE_ID_ELEMENT3,
        "element3",
        sizeof("element3") - 1,
        "",
        bdlat_FormattingMode::e_DEFAULT
    },
    {
        ATTRIBUTE_ID_ELEMENT4,
        "element4",
        sizeof("element4") - 1,
        "",
        bdlat_FormattingMode::e_DEFAULT
      | bdlat_FormattingMode::e_NILLABLE
    },
    {
        ATTRIBUTE_ID_ELEMENT5,
        "element5",
        sizeof("element5") - 1,
        "",
        bdlat_FormattingMode::e_DEFAULT
    }
};

// CLASS METHODS

const bdlat_AttributeInfo *Sequence1::lookupAttributeInfo(
        const char *name,
        int         nameLength)
{
    for (int i = 0; i < 5; ++i) {
        const bdlat_AttributeInfo& attributeInfo =
                    Sequence1::ATTRIBUTE_INFO_ARRAY[i];

        if (nameLength == attributeInfo.d_nameLength
        &&  0 == bsl::memcmp(attributeInfo.d_name_p, name, nameLength))
        {
            return &attributeInfo;
        }
    }

    return 0;
}

const bdlat_AttributeInfo *Sequence1::lookupAttributeInfo(int id)
{
    switch (id) {
      case ATTRIBUTE_ID_ELEMENT1:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT1];
      case ATTRIBUTE_ID_ELEMENT2:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT2];
      case ATTRIBUTE_ID_ELEMENT3:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT3];
      case ATTRIBUTE_ID_ELEMENT4:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT4];
      case ATTRIBUTE_ID_ELEMENT5:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT5];
      default:
        return 0;
    }
}

// CREATORS

Sequence1::Sequence1(bslma::Allocator *basicAllocator)
: d_allocator_p(bslma::Default::allocator(basicAllocator))
, d_element4(basicAllocator)
, d_element5(basicAllocator)
, d_element2(basicAllocator)
, d_element1(basicAllocator)
{
    d_element3 = new (*d_allocator_p)
            Choice2(d_allocator_p);
}

Sequence1::Sequence1(const Sequence1& original,
                     bslma::Allocator *basicAllocator)
: d_allocator_p(bslma::Default::allocator(basicAllocator))
, d_element4(original.d_element4, basicAllocator)
, d_element5(original.d_element5, basicAllocator)
, d_element2(original.d_element2, basicAllocator)
, d_element1(original.d_element1, basicAllocator)
{
    d_element3 = new (*d_allocator_p)
            Choice2(*original.d_element3, d_allocator_p);
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
Sequence1::Sequence1(Sequence1&& original) noexcept
: d_allocator_p(original.d_allocator_p)
, d_element4(bsl::move(original.d_element4))
, d_element5(bsl::move(original.d_element5))
, d_element2(bsl::move(original.d_element2))
, d_element1(bsl::move(original.d_element1))
{
    d_element3 = original.d_element3;
    original.d_element3 = 0;
}

Sequence1::Sequence1(Sequence1&& original,
                     bslma::Allocator *basicAllocator)
: d_allocator_p(bslma::Default::allocator(basicAllocator))
, d_element4(bsl::move(original.d_element4), basicAllocator)
, d_element5(bsl::move(original.d_element5), basicAllocator)
, d_element2(bsl::move(original.d_element2), basicAllocator)
, d_element1(bsl::move(original.d_element1), basicAllocator)
{
    if (d_allocator_p == original.d_allocator_p) {
        d_element3 = original.d_element3;
        original.d_element3 = 0;
    }
    else {
        d_element3 = new (*d_allocator_p)
                Choice2(bsl::move(*original.d_element3), d_allocator_p);
    }
}
#endif

Sequence1::~Sequence1()
{
    d_allocator_p->deleteObject(d_element3);
}

// MANIPULATORS

Sequence1&
Sequence1::operator=(const Sequence1& rhs)
{
    if (this != &rhs) {
        d_element1 = rhs.d_element1;
        d_element2 = rhs.d_element2;
        if (d_element3) {
            *d_element3 = *rhs.d_element3;
        }
        else {
            d_element3 = new (*d_allocator_p)
                Choice2(*rhs.d_element3, d_allocator_p);
        }
        d_element4 = rhs.d_element4;
        d_element5 = rhs.d_element5;
    }

    return *this;
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
Sequence1&
Sequence1::operator=(Sequence1&& rhs)
{
    if (this != &rhs) {
        d_element1 = bsl::move(rhs.d_element1);
        d_element2 = bsl::move(rhs.d_element2);
        if (d_allocator_p == rhs.d_allocator_p) {
            d_allocator_p->deleteObject(d_element3);
            d_element3 = rhs.d_element3;
            rhs.d_element3 = 0;
        }
        else if (d_element3) {
            *d_element3 = bsl::move(*rhs.d_element3);
        }
        else {
            d_element3 = new (*d_allocator_p)
                Choice2(bsl::move(*rhs.d_element3), d_allocator_p);
        }
        d_element4 = bsl::move(rhs.d_element4);
        d_element5 = bsl::move(rhs.d_element5);
    }

    return *this;
}
#endif

void Sequence1::reset()
{
    bdlat_ValueTypeFunctions::reset(&d_element1);
    bdlat_ValueTypeFunctions::reset(&d_element2);
    BSLS_ASSERT(d_element3);
    bdlat_ValueTypeFunctions::reset(d_element3);
    bdlat_ValueTypeFunctions::reset(&d_element4);
    bdlat_ValueTypeFunctions::reset(&d_element5);
}

// ACCESSORS

bsl::ostream& Sequence1::print(
        bsl::ostream& stream,
        int           level,
        int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute("element1", this->element1());
    printer.printAttribute("element2", this->element2());
    printer.printAttribute("element3", this->element3());
    printer.printAttribute("element4", this->element4());
    printer.printAttribute("element5", this->element5());
    printer.end();
    return stream;
}



                              // ---------------
                              // class Sequence2
                              // ---------------

// CONSTANTS

const char Sequence2::CLASS_NAME[] = "Sequence2";

const bdlat_AttributeInfo Sequence2::ATTRIBUTE_INFO_ARRAY[] = {
    {
        ATTRIBUTE_ID_ELEMENT1,
        "element1",
        sizeof("element1") - 1,
        "",
        bdlat_FormattingMode::e_TEXT
    },
    {
        ATTRIBUTE_ID_ELEMENT2,
        "element2",
        sizeof("element2") - 1,
        "",
        bdlat_FormattingMode::e_DEC
    },
    {
        ATTRIBUTE_ID_ELEMENT3,
        "element3",
        sizeof("element3") - 1,
        "",
        bdlat_FormattingMode::e_DEFAULT
    },
    {
        ATTRIBUTE_ID_ELEMENT4,
        "element4",
        sizeof("element4") - 1,
        "",
        bdlat_FormattingMode::e_DEFAULT
    },
    {
        ATTRIBUTE_ID_ELEMENT5,
        "element5",
        sizeof("element5") - 1,
        "",
        bdlat_FormattingMode::e_DEFAULT
    },
    {
        ATTRIBUTE_ID_ELEMENT6,
        "element6",
        sizeof("element6") - 1,
        "",
        bdlat_FormattingMode::e_TEXT
      | bdlat_FormattingMode::e_NILLABLE
    },
    {
        ATTRIBUTE_ID_ELEMENT7,
        "element7",
        sizeof("element7") - 1,
        "",
        bdlat_FormattingMode::e_TEXT
      | bdlat_FormattingMode::e_NILLABLE
    }
};

// CLASS METHODS

const bdlat_AttributeInfo *Sequence2::lookupAttributeInfo(
        const char *name,
        int         nameLength)
{
    for (int i = 0; i < 7; ++i) {
        const bdlat_AttributeInfo& attributeInfo =
                    Sequence2::ATTRIBUTE_INFO_ARRAY[i];

        if (nameLength == attributeInfo.d_nameLength
        &&  0 == bsl::memcmp(attributeInfo.d_name_p, name, nameLength))
        {
            return &attributeInfo;
        }
    }

    return 0;
}

const bdlat_AttributeInfo *Sequence2::lookupAttributeInfo(int id)
{
    switch (id) {
      case ATTRIBUTE_ID_ELEMENT1:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT1];
      case ATTRIBUTE_ID_ELEMENT2:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT2];
      case ATTRIBUTE_ID_ELEMENT3:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT3];
      case ATTRIBUTE_ID_ELEMENT4:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT4];
      case ATTRIBUTE_ID_ELEMENT5:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT5];
      case ATTRIBUTE_ID_ELEMENT6:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT6];
      case ATTRIBUTE_ID_ELEMENT7:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT7];
      default:
        return 0;
    }
}

// CREATORS

Sequence2::Sequence2(bslma::Allocator *basicAllocator)
: d_element1(basicAllocator)
, d_element7(basicAllocator)
, d_element6(basicAllocator)
, d_element3()
, d_element5()
, d_element4(basicAllocator)
, d_element2()
{
}

Sequence2::Sequence2(const Sequence2& original,
                     bslma::Allocator *basicAllocator)
: d_element1(original.d_element1, basicAllocator)
, d_element7(original.d_element7, basicAllocator)
, d_element6(original.d_element6, basicAllocator)
, d_element3(original.d_element3)
, d_element5(original.d_element5)
, d_element4(original.d_element4, basicAllocator)
, d_element2(original.d_element2)
{
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
Sequence2::Sequence2(Sequence2&& original) noexcept
: d_element1(bsl::move(original.d_element1))
, d_element7(bsl::move(original.d_element7))
, d_element6(bsl::move(original.d_element6))
, d_element3(bsl::move(original.d_element3))
, d_element5(bsl::move(original.d_element5))
, d_element4(bsl::move(original.d_element4))
, d_element2(bsl::move(original.d_element2))
{
}

Sequence2::Sequence2(Sequence2&& original,
                     bslma::Allocator *basicAllocator)
: d_element1(bsl::move(original.d_element1), basicAllocator)
, d_element7(bsl::move(original.d_element7), basicAllocator)
, d_element6(bsl::move(original.d_element6), basicAllocator)
, d_element3(bsl::move(original.d_element3))
, d_element5(bsl::move(original.d_element5))
, d_element4(bsl::move(original.d_element4), basicAllocator)
, d_element2(bsl::move(original.d_element2))
{
}
#endif

Sequence2::~Sequence2()
{
}

// MANIPULATORS

Sequence2&
Sequence2::operator=(const Sequence2& rhs)
{
    if (this != &rhs) {
        d_element1 = rhs.d_element1;
        d_element2 = rhs.d_element2;
        d_element3 = rhs.d_element3;
        d_element4 = rhs.d_element4;
        d_element5 = rhs.d_element5;
        d_element6 = rhs.d_element6;
        d_element7 = rhs.d_element7;
    }

    return *this;
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
Sequence2&
Sequence2::operator=(Sequence2&& rhs)
{
    if (this != &rhs) {
        d_element1 = bsl::move(rhs.d_element1);
        d_element2 = bsl::move(rhs.d_element2);
        d_element3 = bsl::move(rhs.d_element3);
        d_element4 = bsl::move(rhs.d_element4);
        d_element5 = bsl::move(rhs.d_element5);
        d_element6 = bsl::move(rhs.d_element6);
        d_element7 = bsl::move(rhs.d_element7);
    }

    return *this;
}
#endif

void Sequence2::reset()
{
    bdlat_ValueTypeFunctions::reset(&d_element1);
    bdlat_ValueTypeFunctions::reset(&d_element2);
    bdlat_ValueTypeFunctions::reset(&d_element3);
    bdlat_ValueTypeFunctions::reset(&d_element4);
    bdlat_ValueTypeFunctions::reset(&d_element5);
    bdlat_ValueTypeFunctions::reset(&d_element6);
    bdlat_ValueTypeFunctions::reset(&d_element7);
}

// ACCESSORS

bsl::ostream& Sequence2::print(
        bsl::ostream& stream,
        int           level,
        int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute("element1", this->element1());
    printer.printAttribute("element2", (int)this->element2());
    printer.printAttribute("element3", this->element3());
    printer.printAttribute("element4", this->element4());
    printer.printAttribute("element5", this->element5());
    printer.printAttribute("element6", this->element6());
    printer.printAttribute("element7", this->element7());
    printer.end();
    return stream;
}


}  // close package namespace
}  // close enterprise namespace

// GENERATED BY @BLP_BAS_CODEGEN_VERSION@
// USING bas_codegen.pl s_baltst_ratsnest.xsd --mode msg --includedir . --msgComponent ratsnest --noRecurse --noExternalization --noHashSupport --noAggregateConversion
// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright 2022 Bloomberg Finance L.P. All rights reserved.
//      Property of Bloomberg Finance L.P. (BFLP)
//      This software is made available solely pursuant to the
//      terms of a BFLP license agreement which governs its use.
// ------------------------------- END-OF-FILE --------------------------------
