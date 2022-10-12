// s_baltst_mysequencewithnillable.cpp    *DO NOT EDIT*    @generated -*-C++-*-

#include <bsls_ident.h>
BSLS_IDENT_RCSID(s_baltst_mysequencewithnillable_cpp, "$Id$ $CSID$")

#include <s_baltst_mysequencewithnillable.h>

#include <bdlat_formattingmode.h>
#include <bdlat_valuetypefunctions.h>
#include <bdlb_print.h>
#include <bdlb_printmethods.h>
#include <bdlb_string.h>

#include <bdlb_nullablevalue.h>
#include <bsl_string.h>
#include <bsl_vector.h>
#include <bslim_printer.h>
#include <bsls_assert.h>

#include <bsl_cstring.h>
#include <bsl_iomanip.h>
#include <bsl_limits.h>
#include <bsl_ostream.h>
#include <bsl_utility.h>

namespace BloombergLP {
namespace s_baltst {

                        // ----------------------------
                        // class MySequenceWithNillable
                        // ----------------------------

// CONSTANTS

const char MySequenceWithNillable::CLASS_NAME[] = "MySequenceWithNillable";

const bdlat_AttributeInfo MySequenceWithNillable::ATTRIBUTE_INFO_ARRAY[] = {
    {
        ATTRIBUTE_ID_ATTRIBUTE1,
        "attribute1",
        sizeof("attribute1") - 1,
        "",
        bdlat_FormattingMode::e_DEC
    },
    {
        ATTRIBUTE_ID_MY_NILLABLE,
        "myNillable",
        sizeof("myNillable") - 1,
        "",
        bdlat_FormattingMode::e_TEXT
      | bdlat_FormattingMode::e_NILLABLE
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

const bdlat_AttributeInfo *MySequenceWithNillable::lookupAttributeInfo(
        const char *name,
        int         nameLength)
{
    for (int i = 0; i < 3; ++i) {
        const bdlat_AttributeInfo& attributeInfo =
                    MySequenceWithNillable::ATTRIBUTE_INFO_ARRAY[i];

        if (nameLength == attributeInfo.d_nameLength
        &&  0 == bsl::memcmp(attributeInfo.d_name_p, name, nameLength))
        {
            return &attributeInfo;
        }
    }

    return 0;
}

const bdlat_AttributeInfo *MySequenceWithNillable::lookupAttributeInfo(int id)
{
    switch (id) {
      case ATTRIBUTE_ID_ATTRIBUTE1:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE1];
      case ATTRIBUTE_ID_MY_NILLABLE:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_MY_NILLABLE];
      case ATTRIBUTE_ID_ATTRIBUTE2:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE2];
      default:
        return 0;
    }
}

// CREATORS

MySequenceWithNillable::MySequenceWithNillable(bslma::Allocator *basicAllocator)
: d_attribute2(basicAllocator)
, d_myNillable(basicAllocator)
, d_attribute1()
{
}

MySequenceWithNillable::MySequenceWithNillable(const MySequenceWithNillable& original,
                                               bslma::Allocator *basicAllocator)
: d_attribute2(original.d_attribute2, basicAllocator)
, d_myNillable(original.d_myNillable, basicAllocator)
, d_attribute1(original.d_attribute1)
{
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
MySequenceWithNillable::MySequenceWithNillable(MySequenceWithNillable&& original) noexcept
: d_attribute2(bsl::move(original.d_attribute2))
, d_myNillable(bsl::move(original.d_myNillable))
, d_attribute1(bsl::move(original.d_attribute1))
{
}

MySequenceWithNillable::MySequenceWithNillable(MySequenceWithNillable&& original,
                                               bslma::Allocator *basicAllocator)
: d_attribute2(bsl::move(original.d_attribute2), basicAllocator)
, d_myNillable(bsl::move(original.d_myNillable), basicAllocator)
, d_attribute1(bsl::move(original.d_attribute1))
{
}
#endif

MySequenceWithNillable::~MySequenceWithNillable()
{
}

// MANIPULATORS

MySequenceWithNillable&
MySequenceWithNillable::operator=(const MySequenceWithNillable& rhs)
{
    if (this != &rhs) {
        d_attribute1 = rhs.d_attribute1;
        d_myNillable = rhs.d_myNillable;
        d_attribute2 = rhs.d_attribute2;
    }

    return *this;
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
MySequenceWithNillable&
MySequenceWithNillable::operator=(MySequenceWithNillable&& rhs)
{
    if (this != &rhs) {
        d_attribute1 = bsl::move(rhs.d_attribute1);
        d_myNillable = bsl::move(rhs.d_myNillable);
        d_attribute2 = bsl::move(rhs.d_attribute2);
    }

    return *this;
}
#endif

void MySequenceWithNillable::reset()
{
    bdlat_ValueTypeFunctions::reset(&d_attribute1);
    bdlat_ValueTypeFunctions::reset(&d_myNillable);
    bdlat_ValueTypeFunctions::reset(&d_attribute2);
}

// ACCESSORS

bsl::ostream& MySequenceWithNillable::print(
        bsl::ostream& stream,
        int           level,
        int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute("attribute1", this->attribute1());
    printer.printAttribute("myNillable", this->myNillable());
    printer.printAttribute("attribute2", this->attribute2());
    printer.end();
    return stream;
}



                  // ----------------------------------------
                  // class MySequenceWithNillableBase64Binary
                  // ----------------------------------------

// CONSTANTS

const char MySequenceWithNillableBase64Binary::CLASS_NAME[] = "MySequenceWithNillableBase64Binary";

const bdlat_AttributeInfo MySequenceWithNillableBase64Binary::ATTRIBUTE_INFO_ARRAY[] = {
    {
        ATTRIBUTE_ID_ATTRIBUTE1,
        "attribute1",
        sizeof("attribute1") - 1,
        "",
        bdlat_FormattingMode::e_BASE64
      | bdlat_FormattingMode::e_NILLABLE
    }
};

// CLASS METHODS

const bdlat_AttributeInfo *MySequenceWithNillableBase64Binary::lookupAttributeInfo(
        const char *name,
        int         nameLength)
{
    for (int i = 0; i < 1; ++i) {
        const bdlat_AttributeInfo& attributeInfo =
                    MySequenceWithNillableBase64Binary::ATTRIBUTE_INFO_ARRAY[i];

        if (nameLength == attributeInfo.d_nameLength
        &&  0 == bsl::memcmp(attributeInfo.d_name_p, name, nameLength))
        {
            return &attributeInfo;
        }
    }

    return 0;
}

const bdlat_AttributeInfo *MySequenceWithNillableBase64Binary::lookupAttributeInfo(int id)
{
    switch (id) {
      case ATTRIBUTE_ID_ATTRIBUTE1:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE1];
      default:
        return 0;
    }
}

// CREATORS

MySequenceWithNillableBase64Binary::MySequenceWithNillableBase64Binary(bslma::Allocator *basicAllocator)
: d_attribute1(basicAllocator)
{
}

MySequenceWithNillableBase64Binary::MySequenceWithNillableBase64Binary(const MySequenceWithNillableBase64Binary& original,
                                                                       bslma::Allocator *basicAllocator)
: d_attribute1(original.d_attribute1, basicAllocator)
{
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
MySequenceWithNillableBase64Binary::MySequenceWithNillableBase64Binary(MySequenceWithNillableBase64Binary&& original) noexcept
: d_attribute1(bsl::move(original.d_attribute1))
{
}

MySequenceWithNillableBase64Binary::MySequenceWithNillableBase64Binary(MySequenceWithNillableBase64Binary&& original,
                                                                       bslma::Allocator *basicAllocator)
: d_attribute1(bsl::move(original.d_attribute1), basicAllocator)
{
}
#endif

MySequenceWithNillableBase64Binary::~MySequenceWithNillableBase64Binary()
{
}

// MANIPULATORS

MySequenceWithNillableBase64Binary&
MySequenceWithNillableBase64Binary::operator=(const MySequenceWithNillableBase64Binary& rhs)
{
    if (this != &rhs) {
        d_attribute1 = rhs.d_attribute1;
    }

    return *this;
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
MySequenceWithNillableBase64Binary&
MySequenceWithNillableBase64Binary::operator=(MySequenceWithNillableBase64Binary&& rhs)
{
    if (this != &rhs) {
        d_attribute1 = bsl::move(rhs.d_attribute1);
    }

    return *this;
}
#endif

void MySequenceWithNillableBase64Binary::reset()
{
    bdlat_ValueTypeFunctions::reset(&d_attribute1);
}

// ACCESSORS

bsl::ostream& MySequenceWithNillableBase64Binary::print(
        bsl::ostream& stream,
        int           level,
        int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute("attribute1", this->attribute1());
    printer.end();
    return stream;
}



                   // -------------------------------------
                   // class MySequenceWithNillableHexBinary
                   // -------------------------------------

// CONSTANTS

const char MySequenceWithNillableHexBinary::CLASS_NAME[] = "MySequenceWithNillableHexBinary";

const bdlat_AttributeInfo MySequenceWithNillableHexBinary::ATTRIBUTE_INFO_ARRAY[] = {
    {
        ATTRIBUTE_ID_ATTRIBUTE1,
        "attribute1",
        sizeof("attribute1") - 1,
        "",
        bdlat_FormattingMode::e_HEX
      | bdlat_FormattingMode::e_NILLABLE
    }
};

// CLASS METHODS

const bdlat_AttributeInfo *MySequenceWithNillableHexBinary::lookupAttributeInfo(
        const char *name,
        int         nameLength)
{
    for (int i = 0; i < 1; ++i) {
        const bdlat_AttributeInfo& attributeInfo =
                    MySequenceWithNillableHexBinary::ATTRIBUTE_INFO_ARRAY[i];

        if (nameLength == attributeInfo.d_nameLength
        &&  0 == bsl::memcmp(attributeInfo.d_name_p, name, nameLength))
        {
            return &attributeInfo;
        }
    }

    return 0;
}

const bdlat_AttributeInfo *MySequenceWithNillableHexBinary::lookupAttributeInfo(int id)
{
    switch (id) {
      case ATTRIBUTE_ID_ATTRIBUTE1:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE1];
      default:
        return 0;
    }
}

// CREATORS

MySequenceWithNillableHexBinary::MySequenceWithNillableHexBinary(bslma::Allocator *basicAllocator)
: d_attribute1(basicAllocator)
{
}

MySequenceWithNillableHexBinary::MySequenceWithNillableHexBinary(const MySequenceWithNillableHexBinary& original,
                                                                 bslma::Allocator *basicAllocator)
: d_attribute1(original.d_attribute1, basicAllocator)
{
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
MySequenceWithNillableHexBinary::MySequenceWithNillableHexBinary(MySequenceWithNillableHexBinary&& original) noexcept
: d_attribute1(bsl::move(original.d_attribute1))
{
}

MySequenceWithNillableHexBinary::MySequenceWithNillableHexBinary(MySequenceWithNillableHexBinary&& original,
                                                                 bslma::Allocator *basicAllocator)
: d_attribute1(bsl::move(original.d_attribute1), basicAllocator)
{
}
#endif

MySequenceWithNillableHexBinary::~MySequenceWithNillableHexBinary()
{
}

// MANIPULATORS

MySequenceWithNillableHexBinary&
MySequenceWithNillableHexBinary::operator=(const MySequenceWithNillableHexBinary& rhs)
{
    if (this != &rhs) {
        d_attribute1 = rhs.d_attribute1;
    }

    return *this;
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
MySequenceWithNillableHexBinary&
MySequenceWithNillableHexBinary::operator=(MySequenceWithNillableHexBinary&& rhs)
{
    if (this != &rhs) {
        d_attribute1 = bsl::move(rhs.d_attribute1);
    }

    return *this;
}
#endif

void MySequenceWithNillableHexBinary::reset()
{
    bdlat_ValueTypeFunctions::reset(&d_attribute1);
}

// ACCESSORS

bsl::ostream& MySequenceWithNillableHexBinary::print(
        bsl::ostream& stream,
        int           level,
        int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute("attribute1", this->attribute1());
    printer.end();
    return stream;
}



                    // ------------------------------------
                    // class MySequenceWithNillableIntArray
                    // ------------------------------------

// CONSTANTS

const char MySequenceWithNillableIntArray::CLASS_NAME[] = "MySequenceWithNillableIntArray";

const bdlat_AttributeInfo MySequenceWithNillableIntArray::ATTRIBUTE_INFO_ARRAY[] = {
    {
        ATTRIBUTE_ID_ATTRIBUTE1,
        "attribute1",
        sizeof("attribute1") - 1,
        "",
        bdlat_FormattingMode::e_DEC
      | bdlat_FormattingMode::e_NILLABLE
    }
};

// CLASS METHODS

const bdlat_AttributeInfo *MySequenceWithNillableIntArray::lookupAttributeInfo(
        const char *name,
        int         nameLength)
{
    for (int i = 0; i < 1; ++i) {
        const bdlat_AttributeInfo& attributeInfo =
                    MySequenceWithNillableIntArray::ATTRIBUTE_INFO_ARRAY[i];

        if (nameLength == attributeInfo.d_nameLength
        &&  0 == bsl::memcmp(attributeInfo.d_name_p, name, nameLength))
        {
            return &attributeInfo;
        }
    }

    return 0;
}

const bdlat_AttributeInfo *MySequenceWithNillableIntArray::lookupAttributeInfo(int id)
{
    switch (id) {
      case ATTRIBUTE_ID_ATTRIBUTE1:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE1];
      default:
        return 0;
    }
}

// CREATORS

MySequenceWithNillableIntArray::MySequenceWithNillableIntArray(bslma::Allocator *basicAllocator)
: d_attribute1(basicAllocator)
{
}

MySequenceWithNillableIntArray::MySequenceWithNillableIntArray(const MySequenceWithNillableIntArray& original,
                                                               bslma::Allocator *basicAllocator)
: d_attribute1(original.d_attribute1, basicAllocator)
{
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
MySequenceWithNillableIntArray::MySequenceWithNillableIntArray(MySequenceWithNillableIntArray&& original) noexcept
: d_attribute1(bsl::move(original.d_attribute1))
{
}

MySequenceWithNillableIntArray::MySequenceWithNillableIntArray(MySequenceWithNillableIntArray&& original,
                                                               bslma::Allocator *basicAllocator)
: d_attribute1(bsl::move(original.d_attribute1), basicAllocator)
{
}
#endif

MySequenceWithNillableIntArray::~MySequenceWithNillableIntArray()
{
}

// MANIPULATORS

MySequenceWithNillableIntArray&
MySequenceWithNillableIntArray::operator=(const MySequenceWithNillableIntArray& rhs)
{
    if (this != &rhs) {
        d_attribute1 = rhs.d_attribute1;
    }

    return *this;
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
MySequenceWithNillableIntArray&
MySequenceWithNillableIntArray::operator=(MySequenceWithNillableIntArray&& rhs)
{
    if (this != &rhs) {
        d_attribute1 = bsl::move(rhs.d_attribute1);
    }

    return *this;
}
#endif

void MySequenceWithNillableIntArray::reset()
{
    bdlat_ValueTypeFunctions::reset(&d_attribute1);
}

// ACCESSORS

bsl::ostream& MySequenceWithNillableIntArray::print(
        bsl::ostream& stream,
        int           level,
        int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute("attribute1", this->attribute1());
    printer.end();
    return stream;
}



            // ----------------------------------------------------
            // class MySequenceWithNillableIntSequenceArraySequence
            // ----------------------------------------------------

// CONSTANTS

const char MySequenceWithNillableIntSequenceArraySequence::CLASS_NAME[] = "MySequenceWithNillableIntSequenceArraySequence";

const bdlat_AttributeInfo MySequenceWithNillableIntSequenceArraySequence::ATTRIBUTE_INFO_ARRAY[] = {
    {
        ATTRIBUTE_ID_ATTRIBUTE1,
        "attribute1",
        sizeof("attribute1") - 1,
        "",
        bdlat_FormattingMode::e_DEC
    }
};

// CLASS METHODS

const bdlat_AttributeInfo *MySequenceWithNillableIntSequenceArraySequence::lookupAttributeInfo(
        const char *name,
        int         nameLength)
{
    for (int i = 0; i < 1; ++i) {
        const bdlat_AttributeInfo& attributeInfo =
                    MySequenceWithNillableIntSequenceArraySequence::ATTRIBUTE_INFO_ARRAY[i];

        if (nameLength == attributeInfo.d_nameLength
        &&  0 == bsl::memcmp(attributeInfo.d_name_p, name, nameLength))
        {
            return &attributeInfo;
        }
    }

    return 0;
}

const bdlat_AttributeInfo *MySequenceWithNillableIntSequenceArraySequence::lookupAttributeInfo(int id)
{
    switch (id) {
      case ATTRIBUTE_ID_ATTRIBUTE1:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE1];
      default:
        return 0;
    }
}

// CREATORS

MySequenceWithNillableIntSequenceArraySequence::MySequenceWithNillableIntSequenceArraySequence()
: d_attribute1()
{
}

MySequenceWithNillableIntSequenceArraySequence::MySequenceWithNillableIntSequenceArraySequence(const MySequenceWithNillableIntSequenceArraySequence& original)
: d_attribute1(original.d_attribute1)
{
}

MySequenceWithNillableIntSequenceArraySequence::~MySequenceWithNillableIntSequenceArraySequence()
{
}

// MANIPULATORS

MySequenceWithNillableIntSequenceArraySequence&
MySequenceWithNillableIntSequenceArraySequence::operator=(const MySequenceWithNillableIntSequenceArraySequence& rhs)
{
    if (this != &rhs) {
        d_attribute1 = rhs.d_attribute1;
    }

    return *this;
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
MySequenceWithNillableIntSequenceArraySequence&
MySequenceWithNillableIntSequenceArraySequence::operator=(MySequenceWithNillableIntSequenceArraySequence&& rhs)
{
    if (this != &rhs) {
        d_attribute1 = bsl::move(rhs.d_attribute1);
    }

    return *this;
}
#endif

void MySequenceWithNillableIntSequenceArraySequence::reset()
{
    bdlat_ValueTypeFunctions::reset(&d_attribute1);
}

// ACCESSORS

bsl::ostream& MySequenceWithNillableIntSequenceArraySequence::print(
        bsl::ostream& stream,
        int           level,
        int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute("attribute1", this->attribute1());
    printer.end();
    return stream;
}



                  // ---------------------------------------
                  // class MySequenceWithNillableStringArray
                  // ---------------------------------------

// CONSTANTS

const char MySequenceWithNillableStringArray::CLASS_NAME[] = "MySequenceWithNillableStringArray";

const bdlat_AttributeInfo MySequenceWithNillableStringArray::ATTRIBUTE_INFO_ARRAY[] = {
    {
        ATTRIBUTE_ID_ATTRIBUTE1,
        "attribute1",
        sizeof("attribute1") - 1,
        "",
        bdlat_FormattingMode::e_TEXT
      | bdlat_FormattingMode::e_NILLABLE
    }
};

// CLASS METHODS

const bdlat_AttributeInfo *MySequenceWithNillableStringArray::lookupAttributeInfo(
        const char *name,
        int         nameLength)
{
    for (int i = 0; i < 1; ++i) {
        const bdlat_AttributeInfo& attributeInfo =
                    MySequenceWithNillableStringArray::ATTRIBUTE_INFO_ARRAY[i];

        if (nameLength == attributeInfo.d_nameLength
        &&  0 == bsl::memcmp(attributeInfo.d_name_p, name, nameLength))
        {
            return &attributeInfo;
        }
    }

    return 0;
}

const bdlat_AttributeInfo *MySequenceWithNillableStringArray::lookupAttributeInfo(int id)
{
    switch (id) {
      case ATTRIBUTE_ID_ATTRIBUTE1:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE1];
      default:
        return 0;
    }
}

// CREATORS

MySequenceWithNillableStringArray::MySequenceWithNillableStringArray(bslma::Allocator *basicAllocator)
: d_attribute1(basicAllocator)
{
}

MySequenceWithNillableStringArray::MySequenceWithNillableStringArray(const MySequenceWithNillableStringArray& original,
                                                                     bslma::Allocator *basicAllocator)
: d_attribute1(original.d_attribute1, basicAllocator)
{
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
MySequenceWithNillableStringArray::MySequenceWithNillableStringArray(MySequenceWithNillableStringArray&& original) noexcept
: d_attribute1(bsl::move(original.d_attribute1))
{
}

MySequenceWithNillableStringArray::MySequenceWithNillableStringArray(MySequenceWithNillableStringArray&& original,
                                                                     bslma::Allocator *basicAllocator)
: d_attribute1(bsl::move(original.d_attribute1), basicAllocator)
{
}
#endif

MySequenceWithNillableStringArray::~MySequenceWithNillableStringArray()
{
}

// MANIPULATORS

MySequenceWithNillableStringArray&
MySequenceWithNillableStringArray::operator=(const MySequenceWithNillableStringArray& rhs)
{
    if (this != &rhs) {
        d_attribute1 = rhs.d_attribute1;
    }

    return *this;
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
MySequenceWithNillableStringArray&
MySequenceWithNillableStringArray::operator=(MySequenceWithNillableStringArray&& rhs)
{
    if (this != &rhs) {
        d_attribute1 = bsl::move(rhs.d_attribute1);
    }

    return *this;
}
#endif

void MySequenceWithNillableStringArray::reset()
{
    bdlat_ValueTypeFunctions::reset(&d_attribute1);
}

// ACCESSORS

bsl::ostream& MySequenceWithNillableStringArray::print(
        bsl::ostream& stream,
        int           level,
        int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute("attribute1", this->attribute1());
    printer.end();
    return stream;
}



          // -------------------------------------------------------
          // class MySequenceWithNillableStringSequenceArraySequence
          // -------------------------------------------------------

// CONSTANTS

const char MySequenceWithNillableStringSequenceArraySequence::CLASS_NAME[] = "MySequenceWithNillableStringSequenceArraySequence";

const bdlat_AttributeInfo MySequenceWithNillableStringSequenceArraySequence::ATTRIBUTE_INFO_ARRAY[] = {
    {
        ATTRIBUTE_ID_ATTRIBUTE1,
        "attribute1",
        sizeof("attribute1") - 1,
        "",
        bdlat_FormattingMode::e_TEXT
    }
};

// CLASS METHODS

const bdlat_AttributeInfo *MySequenceWithNillableStringSequenceArraySequence::lookupAttributeInfo(
        const char *name,
        int         nameLength)
{
    for (int i = 0; i < 1; ++i) {
        const bdlat_AttributeInfo& attributeInfo =
                    MySequenceWithNillableStringSequenceArraySequence::ATTRIBUTE_INFO_ARRAY[i];

        if (nameLength == attributeInfo.d_nameLength
        &&  0 == bsl::memcmp(attributeInfo.d_name_p, name, nameLength))
        {
            return &attributeInfo;
        }
    }

    return 0;
}

const bdlat_AttributeInfo *MySequenceWithNillableStringSequenceArraySequence::lookupAttributeInfo(int id)
{
    switch (id) {
      case ATTRIBUTE_ID_ATTRIBUTE1:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE1];
      default:
        return 0;
    }
}

// CREATORS

MySequenceWithNillableStringSequenceArraySequence::MySequenceWithNillableStringSequenceArraySequence(bslma::Allocator *basicAllocator)
: d_attribute1(basicAllocator)
{
}

MySequenceWithNillableStringSequenceArraySequence::MySequenceWithNillableStringSequenceArraySequence(const MySequenceWithNillableStringSequenceArraySequence& original,
                                                                                                     bslma::Allocator *basicAllocator)
: d_attribute1(original.d_attribute1, basicAllocator)
{
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
MySequenceWithNillableStringSequenceArraySequence::MySequenceWithNillableStringSequenceArraySequence(MySequenceWithNillableStringSequenceArraySequence&& original) noexcept
: d_attribute1(bsl::move(original.d_attribute1))
{
}

MySequenceWithNillableStringSequenceArraySequence::MySequenceWithNillableStringSequenceArraySequence(MySequenceWithNillableStringSequenceArraySequence&& original,
                                                                                                     bslma::Allocator *basicAllocator)
: d_attribute1(bsl::move(original.d_attribute1), basicAllocator)
{
}
#endif

MySequenceWithNillableStringSequenceArraySequence::~MySequenceWithNillableStringSequenceArraySequence()
{
}

// MANIPULATORS

MySequenceWithNillableStringSequenceArraySequence&
MySequenceWithNillableStringSequenceArraySequence::operator=(const MySequenceWithNillableStringSequenceArraySequence& rhs)
{
    if (this != &rhs) {
        d_attribute1 = rhs.d_attribute1;
    }

    return *this;
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
MySequenceWithNillableStringSequenceArraySequence&
MySequenceWithNillableStringSequenceArraySequence::operator=(MySequenceWithNillableStringSequenceArraySequence&& rhs)
{
    if (this != &rhs) {
        d_attribute1 = bsl::move(rhs.d_attribute1);
    }

    return *this;
}
#endif

void MySequenceWithNillableStringSequenceArraySequence::reset()
{
    bdlat_ValueTypeFunctions::reset(&d_attribute1);
}

// ACCESSORS

bsl::ostream& MySequenceWithNillableStringSequenceArraySequence::print(
        bsl::ostream& stream,
        int           level,
        int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute("attribute1", this->attribute1());
    printer.end();
    return stream;
}



                    // -----------------------------------
                    // class MySequenceWithNillableIntList
                    // -----------------------------------

// CONSTANTS

const char MySequenceWithNillableIntList::CLASS_NAME[] = "MySequenceWithNillableIntList";

const bdlat_AttributeInfo MySequenceWithNillableIntList::ATTRIBUTE_INFO_ARRAY[] = {
    {
        ATTRIBUTE_ID_ATTRIBUTE1,
        "attribute1",
        sizeof("attribute1") - 1,
        "",
        bdlat_FormattingMode::e_LIST
      | bdlat_FormattingMode::e_NILLABLE
    }
};

// CLASS METHODS

const bdlat_AttributeInfo *MySequenceWithNillableIntList::lookupAttributeInfo(
        const char *name,
        int         nameLength)
{
    for (int i = 0; i < 1; ++i) {
        const bdlat_AttributeInfo& attributeInfo =
                    MySequenceWithNillableIntList::ATTRIBUTE_INFO_ARRAY[i];

        if (nameLength == attributeInfo.d_nameLength
        &&  0 == bsl::memcmp(attributeInfo.d_name_p, name, nameLength))
        {
            return &attributeInfo;
        }
    }

    return 0;
}

const bdlat_AttributeInfo *MySequenceWithNillableIntList::lookupAttributeInfo(int id)
{
    switch (id) {
      case ATTRIBUTE_ID_ATTRIBUTE1:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE1];
      default:
        return 0;
    }
}

// CREATORS

MySequenceWithNillableIntList::MySequenceWithNillableIntList(bslma::Allocator *basicAllocator)
: d_attribute1(basicAllocator)
{
}

MySequenceWithNillableIntList::MySequenceWithNillableIntList(const MySequenceWithNillableIntList& original,
                                                             bslma::Allocator *basicAllocator)
: d_attribute1(original.d_attribute1, basicAllocator)
{
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
MySequenceWithNillableIntList::MySequenceWithNillableIntList(MySequenceWithNillableIntList&& original) noexcept
: d_attribute1(bsl::move(original.d_attribute1))
{
}

MySequenceWithNillableIntList::MySequenceWithNillableIntList(MySequenceWithNillableIntList&& original,
                                                             bslma::Allocator *basicAllocator)
: d_attribute1(bsl::move(original.d_attribute1), basicAllocator)
{
}
#endif

MySequenceWithNillableIntList::~MySequenceWithNillableIntList()
{
}

// MANIPULATORS

MySequenceWithNillableIntList&
MySequenceWithNillableIntList::operator=(const MySequenceWithNillableIntList& rhs)
{
    if (this != &rhs) {
        d_attribute1 = rhs.d_attribute1;
    }

    return *this;
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
MySequenceWithNillableIntList&
MySequenceWithNillableIntList::operator=(MySequenceWithNillableIntList&& rhs)
{
    if (this != &rhs) {
        d_attribute1 = bsl::move(rhs.d_attribute1);
    }

    return *this;
}
#endif

void MySequenceWithNillableIntList::reset()
{
    bdlat_ValueTypeFunctions::reset(&d_attribute1);
}

// ACCESSORS

bsl::ostream& MySequenceWithNillableIntList::print(
        bsl::ostream& stream,
        int           level,
        int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute("attribute1", this->attribute1());
    printer.end();
    return stream;
}



                // --------------------------------------------
                // class MySequenceWithNillableIntSequenceArray
                // --------------------------------------------

// CONSTANTS

const char MySequenceWithNillableIntSequenceArray::CLASS_NAME[] = "MySequenceWithNillableIntSequenceArray";

const bdlat_AttributeInfo MySequenceWithNillableIntSequenceArray::ATTRIBUTE_INFO_ARRAY[] = {
    {
        ATTRIBUTE_ID_ATTRIBUTE1,
        "attribute1",
        sizeof("attribute1") - 1,
        "",
        bdlat_FormattingMode::e_DEFAULT
      | bdlat_FormattingMode::e_NILLABLE
    }
};

// CLASS METHODS

const bdlat_AttributeInfo *MySequenceWithNillableIntSequenceArray::lookupAttributeInfo(
        const char *name,
        int         nameLength)
{
    for (int i = 0; i < 1; ++i) {
        const bdlat_AttributeInfo& attributeInfo =
                    MySequenceWithNillableIntSequenceArray::ATTRIBUTE_INFO_ARRAY[i];

        if (nameLength == attributeInfo.d_nameLength
        &&  0 == bsl::memcmp(attributeInfo.d_name_p, name, nameLength))
        {
            return &attributeInfo;
        }
    }

    return 0;
}

const bdlat_AttributeInfo *MySequenceWithNillableIntSequenceArray::lookupAttributeInfo(int id)
{
    switch (id) {
      case ATTRIBUTE_ID_ATTRIBUTE1:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE1];
      default:
        return 0;
    }
}

// CREATORS

MySequenceWithNillableIntSequenceArray::MySequenceWithNillableIntSequenceArray(bslma::Allocator *basicAllocator)
: d_attribute1(basicAllocator)
{
}

MySequenceWithNillableIntSequenceArray::MySequenceWithNillableIntSequenceArray(const MySequenceWithNillableIntSequenceArray& original,
                                                                               bslma::Allocator *basicAllocator)
: d_attribute1(original.d_attribute1, basicAllocator)
{
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
MySequenceWithNillableIntSequenceArray::MySequenceWithNillableIntSequenceArray(MySequenceWithNillableIntSequenceArray&& original) noexcept
: d_attribute1(bsl::move(original.d_attribute1))
{
}

MySequenceWithNillableIntSequenceArray::MySequenceWithNillableIntSequenceArray(MySequenceWithNillableIntSequenceArray&& original,
                                                                               bslma::Allocator *basicAllocator)
: d_attribute1(bsl::move(original.d_attribute1), basicAllocator)
{
}
#endif

MySequenceWithNillableIntSequenceArray::~MySequenceWithNillableIntSequenceArray()
{
}

// MANIPULATORS

MySequenceWithNillableIntSequenceArray&
MySequenceWithNillableIntSequenceArray::operator=(const MySequenceWithNillableIntSequenceArray& rhs)
{
    if (this != &rhs) {
        d_attribute1 = rhs.d_attribute1;
    }

    return *this;
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
MySequenceWithNillableIntSequenceArray&
MySequenceWithNillableIntSequenceArray::operator=(MySequenceWithNillableIntSequenceArray&& rhs)
{
    if (this != &rhs) {
        d_attribute1 = bsl::move(rhs.d_attribute1);
    }

    return *this;
}
#endif

void MySequenceWithNillableIntSequenceArray::reset()
{
    bdlat_ValueTypeFunctions::reset(&d_attribute1);
}

// ACCESSORS

bsl::ostream& MySequenceWithNillableIntSequenceArray::print(
        bsl::ostream& stream,
        int           level,
        int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute("attribute1", this->attribute1());
    printer.end();
    return stream;
}



                   // --------------------------------------
                   // class MySequenceWithNillableStringList
                   // --------------------------------------

// CONSTANTS

const char MySequenceWithNillableStringList::CLASS_NAME[] = "MySequenceWithNillableStringList";

const bdlat_AttributeInfo MySequenceWithNillableStringList::ATTRIBUTE_INFO_ARRAY[] = {
    {
        ATTRIBUTE_ID_ATTRIBUTE1,
        "attribute1",
        sizeof("attribute1") - 1,
        "",
        bdlat_FormattingMode::e_LIST
      | bdlat_FormattingMode::e_NILLABLE
    }
};

// CLASS METHODS

const bdlat_AttributeInfo *MySequenceWithNillableStringList::lookupAttributeInfo(
        const char *name,
        int         nameLength)
{
    for (int i = 0; i < 1; ++i) {
        const bdlat_AttributeInfo& attributeInfo =
                    MySequenceWithNillableStringList::ATTRIBUTE_INFO_ARRAY[i];

        if (nameLength == attributeInfo.d_nameLength
        &&  0 == bsl::memcmp(attributeInfo.d_name_p, name, nameLength))
        {
            return &attributeInfo;
        }
    }

    return 0;
}

const bdlat_AttributeInfo *MySequenceWithNillableStringList::lookupAttributeInfo(int id)
{
    switch (id) {
      case ATTRIBUTE_ID_ATTRIBUTE1:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE1];
      default:
        return 0;
    }
}

// CREATORS

MySequenceWithNillableStringList::MySequenceWithNillableStringList(bslma::Allocator *basicAllocator)
: d_attribute1(basicAllocator)
{
}

MySequenceWithNillableStringList::MySequenceWithNillableStringList(const MySequenceWithNillableStringList& original,
                                                                   bslma::Allocator *basicAllocator)
: d_attribute1(original.d_attribute1, basicAllocator)
{
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
MySequenceWithNillableStringList::MySequenceWithNillableStringList(MySequenceWithNillableStringList&& original) noexcept
: d_attribute1(bsl::move(original.d_attribute1))
{
}

MySequenceWithNillableStringList::MySequenceWithNillableStringList(MySequenceWithNillableStringList&& original,
                                                                   bslma::Allocator *basicAllocator)
: d_attribute1(bsl::move(original.d_attribute1), basicAllocator)
{
}
#endif

MySequenceWithNillableStringList::~MySequenceWithNillableStringList()
{
}

// MANIPULATORS

MySequenceWithNillableStringList&
MySequenceWithNillableStringList::operator=(const MySequenceWithNillableStringList& rhs)
{
    if (this != &rhs) {
        d_attribute1 = rhs.d_attribute1;
    }

    return *this;
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
MySequenceWithNillableStringList&
MySequenceWithNillableStringList::operator=(MySequenceWithNillableStringList&& rhs)
{
    if (this != &rhs) {
        d_attribute1 = bsl::move(rhs.d_attribute1);
    }

    return *this;
}
#endif

void MySequenceWithNillableStringList::reset()
{
    bdlat_ValueTypeFunctions::reset(&d_attribute1);
}

// ACCESSORS

bsl::ostream& MySequenceWithNillableStringList::print(
        bsl::ostream& stream,
        int           level,
        int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute("attribute1", this->attribute1());
    printer.end();
    return stream;
}



              // -----------------------------------------------
              // class MySequenceWithNillableStringSequenceArray
              // -----------------------------------------------

// CONSTANTS

const char MySequenceWithNillableStringSequenceArray::CLASS_NAME[] = "MySequenceWithNillableStringSequenceArray";

const bdlat_AttributeInfo MySequenceWithNillableStringSequenceArray::ATTRIBUTE_INFO_ARRAY[] = {
    {
        ATTRIBUTE_ID_ATTRIBUTE1,
        "attribute1",
        sizeof("attribute1") - 1,
        "",
        bdlat_FormattingMode::e_DEFAULT
      | bdlat_FormattingMode::e_NILLABLE
    }
};

// CLASS METHODS

const bdlat_AttributeInfo *MySequenceWithNillableStringSequenceArray::lookupAttributeInfo(
        const char *name,
        int         nameLength)
{
    for (int i = 0; i < 1; ++i) {
        const bdlat_AttributeInfo& attributeInfo =
                    MySequenceWithNillableStringSequenceArray::ATTRIBUTE_INFO_ARRAY[i];

        if (nameLength == attributeInfo.d_nameLength
        &&  0 == bsl::memcmp(attributeInfo.d_name_p, name, nameLength))
        {
            return &attributeInfo;
        }
    }

    return 0;
}

const bdlat_AttributeInfo *MySequenceWithNillableStringSequenceArray::lookupAttributeInfo(int id)
{
    switch (id) {
      case ATTRIBUTE_ID_ATTRIBUTE1:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE1];
      default:
        return 0;
    }
}

// CREATORS

MySequenceWithNillableStringSequenceArray::MySequenceWithNillableStringSequenceArray(bslma::Allocator *basicAllocator)
: d_attribute1(basicAllocator)
{
}

MySequenceWithNillableStringSequenceArray::MySequenceWithNillableStringSequenceArray(const MySequenceWithNillableStringSequenceArray& original,
                                                                                     bslma::Allocator *basicAllocator)
: d_attribute1(original.d_attribute1, basicAllocator)
{
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
MySequenceWithNillableStringSequenceArray::MySequenceWithNillableStringSequenceArray(MySequenceWithNillableStringSequenceArray&& original) noexcept
: d_attribute1(bsl::move(original.d_attribute1))
{
}

MySequenceWithNillableStringSequenceArray::MySequenceWithNillableStringSequenceArray(MySequenceWithNillableStringSequenceArray&& original,
                                                                                     bslma::Allocator *basicAllocator)
: d_attribute1(bsl::move(original.d_attribute1), basicAllocator)
{
}
#endif

MySequenceWithNillableStringSequenceArray::~MySequenceWithNillableStringSequenceArray()
{
}

// MANIPULATORS

MySequenceWithNillableStringSequenceArray&
MySequenceWithNillableStringSequenceArray::operator=(const MySequenceWithNillableStringSequenceArray& rhs)
{
    if (this != &rhs) {
        d_attribute1 = rhs.d_attribute1;
    }

    return *this;
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
MySequenceWithNillableStringSequenceArray&
MySequenceWithNillableStringSequenceArray::operator=(MySequenceWithNillableStringSequenceArray&& rhs)
{
    if (this != &rhs) {
        d_attribute1 = bsl::move(rhs.d_attribute1);
    }

    return *this;
}
#endif

void MySequenceWithNillableStringSequenceArray::reset()
{
    bdlat_ValueTypeFunctions::reset(&d_attribute1);
}

// ACCESSORS

bsl::ostream& MySequenceWithNillableStringSequenceArray::print(
        bsl::ostream& stream,
        int           level,
        int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute("attribute1", this->attribute1());
    printer.end();
    return stream;
}


}  // close package namespace
}  // close enterprise namespace

// GENERATED BY @BLP_BAS_CODEGEN_VERSION@
// USING bas_codegen.pl s_baltst_mysequencewithnillable.xsd --mode msg --includedir . --msgComponent mysequencewithnillable --noRecurse --noExternalization --noHashSupport --noAggregateConversion
// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright 2022 Bloomberg Finance L.P. All rights reserved.
//      Property of Bloomberg Finance L.P. (BFLP)
//      This software is made available solely pursuant to the
//      terms of a BFLP license agreement which governs its use.
// ------------------------------- END-OF-FILE --------------------------------
