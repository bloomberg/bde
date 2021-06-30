// s_baltst_basicschema.cpp         *DO NOT EDIT*          @generated -*-C++-*-

#include <bsls_ident.h>
BSLS_IDENT_RCSID(s_baltst_basicschema_cpp,"$Id$ $CSID$")

#include <s_baltst_basicschema.h>

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

#include <bsl_iomanip.h>
#include <bsl_limits.h>
#include <bsl_ostream.h>

namespace BloombergLP {
namespace s_baltst {

                     // ----------------------------------
                     // class BasicSchemaAnnotationElement
                     // ----------------------------------

// CONSTANTS

const char BasicSchemaAnnotationElement::CLASS_NAME[] = "BasicSchemaAnnotationElement";

const bdlat_AttributeInfo BasicSchemaAnnotationElement::ATTRIBUTE_INFO_ARRAY[] = {
    {
        ATTRIBUTE_ID_DOCUMENTATION,
        "documentation",
        sizeof("documentation") - 1,
        "",
        bdlat_FormattingMode::e_TEXT
    }
};

// CLASS METHODS

const bdlat_AttributeInfo *BasicSchemaAnnotationElement::lookupAttributeInfo(
        const char *name,
        int         nameLength)
{
    for (int i = 0; i < 1; ++i) {
        const bdlat_AttributeInfo& attributeInfo =
                    BasicSchemaAnnotationElement::ATTRIBUTE_INFO_ARRAY[i];

        if (nameLength == attributeInfo.d_nameLength
        &&  0 == bsl::memcmp(attributeInfo.d_name_p, name, nameLength))
        {
            return &attributeInfo;
        }
    }

    return 0;
}

const bdlat_AttributeInfo *BasicSchemaAnnotationElement::lookupAttributeInfo(int id)
{
    switch (id) {
      case ATTRIBUTE_ID_DOCUMENTATION:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_DOCUMENTATION];
      default:
        return 0;
    }
}

// CREATORS

BasicSchemaAnnotationElement::BasicSchemaAnnotationElement(bslma::Allocator *basicAllocator)
: d_documentation(basicAllocator)
{
}

BasicSchemaAnnotationElement::BasicSchemaAnnotationElement(const BasicSchemaAnnotationElement& original,
                                                           bslma::Allocator *basicAllocator)
: d_documentation(original.d_documentation, basicAllocator)
{
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
BasicSchemaAnnotationElement::BasicSchemaAnnotationElement(BasicSchemaAnnotationElement&& original) noexcept
: d_documentation(bsl::move(original.d_documentation))
{
}

BasicSchemaAnnotationElement::BasicSchemaAnnotationElement(BasicSchemaAnnotationElement&& original,
                                                           bslma::Allocator *basicAllocator)
: d_documentation(bsl::move(original.d_documentation), basicAllocator)
{
}
#endif

BasicSchemaAnnotationElement::~BasicSchemaAnnotationElement()
{
}

// MANIPULATORS

BasicSchemaAnnotationElement&
BasicSchemaAnnotationElement::operator=(const BasicSchemaAnnotationElement& rhs)
{
    if (this != &rhs) {
        d_documentation = rhs.d_documentation;
    }

    return *this;
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
BasicSchemaAnnotationElement&
BasicSchemaAnnotationElement::operator=(BasicSchemaAnnotationElement&& rhs)
{
    if (this != &rhs) {
        d_documentation = bsl::move(rhs.d_documentation);
    }

    return *this;
}
#endif

void BasicSchemaAnnotationElement::reset()
{
    bdlat_ValueTypeFunctions::reset(&d_documentation);
}

// ACCESSORS

bsl::ostream& BasicSchemaAnnotationElement::print(
        bsl::ostream& stream,
        int           level,
        int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute("documentation", d_documentation);
    printer.end();
    return stream;
}



                 // -----------------------------------------
                 // class BasicSchemaEnumerationElementChoice
                 // -----------------------------------------

// CONSTANTS

const char BasicSchemaEnumerationElementChoice::CLASS_NAME[] = "BasicSchemaEnumerationElementChoice";

const bdlat_SelectionInfo BasicSchemaEnumerationElementChoice::SELECTION_INFO_ARRAY[] = {
    {
        SELECTION_ID_ANNOTATION,
        "annotation",
        sizeof("annotation") - 1,
        "",
        bdlat_FormattingMode::e_DEFAULT
    }
};

// CLASS METHODS

const bdlat_SelectionInfo *BasicSchemaEnumerationElementChoice::lookupSelectionInfo(
        const char *name,
        int         nameLength)
{
    for (int i = 0; i < 1; ++i) {
        const bdlat_SelectionInfo& selectionInfo =
                    BasicSchemaEnumerationElementChoice::SELECTION_INFO_ARRAY[i];

        if (nameLength == selectionInfo.d_nameLength
        &&  0 == bsl::memcmp(selectionInfo.d_name_p, name, nameLength))
        {
            return &selectionInfo;
        }
    }

    return 0;
}

const bdlat_SelectionInfo *BasicSchemaEnumerationElementChoice::lookupSelectionInfo(int id)
{
    switch (id) {
      case SELECTION_ID_ANNOTATION:
        return &SELECTION_INFO_ARRAY[SELECTION_INDEX_ANNOTATION];
      default:
        return 0;
    }
}

// CREATORS

BasicSchemaEnumerationElementChoice::BasicSchemaEnumerationElementChoice(
    const BasicSchemaEnumerationElementChoice& original,
    bslma::Allocator *basicAllocator)
: d_selectionId(original.d_selectionId)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    switch (d_selectionId) {
      case SELECTION_ID_ANNOTATION: {
        new (d_annotation.buffer())
            BasicSchemaAnnotationElement(
                original.d_annotation.object(), d_allocator_p);
      } break;
      default:
        BSLS_ASSERT(SELECTION_ID_UNDEFINED == d_selectionId);
    }
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
BasicSchemaEnumerationElementChoice::BasicSchemaEnumerationElementChoice(BasicSchemaEnumerationElementChoice&& original) noexcept
: d_selectionId(original.d_selectionId)
, d_allocator_p(original.d_allocator_p)
{
    switch (d_selectionId) {
      case SELECTION_ID_ANNOTATION: {
        new (d_annotation.buffer())
            BasicSchemaAnnotationElement(
                bsl::move(original.d_annotation.object()), d_allocator_p);
      } break;
      default:
        BSLS_ASSERT(SELECTION_ID_UNDEFINED == d_selectionId);
    }
}

BasicSchemaEnumerationElementChoice::BasicSchemaEnumerationElementChoice(
    BasicSchemaEnumerationElementChoice&& original,
    bslma::Allocator *basicAllocator)
: d_selectionId(original.d_selectionId)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    switch (d_selectionId) {
      case SELECTION_ID_ANNOTATION: {
        new (d_annotation.buffer())
            BasicSchemaAnnotationElement(
                bsl::move(original.d_annotation.object()), d_allocator_p);
      } break;
      default:
        BSLS_ASSERT(SELECTION_ID_UNDEFINED == d_selectionId);
    }
}
#endif

// MANIPULATORS

BasicSchemaEnumerationElementChoice&
BasicSchemaEnumerationElementChoice::operator=(const BasicSchemaEnumerationElementChoice& rhs)
{
    if (this != &rhs) {
        switch (rhs.d_selectionId) {
          case SELECTION_ID_ANNOTATION: {
            makeAnnotation(rhs.d_annotation.object());
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
BasicSchemaEnumerationElementChoice&
BasicSchemaEnumerationElementChoice::operator=(BasicSchemaEnumerationElementChoice&& rhs)
{
    if (this != &rhs) {
        switch (rhs.d_selectionId) {
          case SELECTION_ID_ANNOTATION: {
            makeAnnotation(bsl::move(rhs.d_annotation.object()));
          } break;
          default:
            BSLS_ASSERT(SELECTION_ID_UNDEFINED == rhs.d_selectionId);
            reset();
        }
    }

    return *this;
}
#endif

void BasicSchemaEnumerationElementChoice::reset()
{
    switch (d_selectionId) {
      case SELECTION_ID_ANNOTATION: {
        d_annotation.object().~BasicSchemaAnnotationElement();
      } break;
      default:
        BSLS_ASSERT(SELECTION_ID_UNDEFINED == d_selectionId);
    }

    d_selectionId = SELECTION_ID_UNDEFINED;
}

int BasicSchemaEnumerationElementChoice::makeSelection(int selectionId)
{
    switch (selectionId) {
      case SELECTION_ID_ANNOTATION: {
        makeAnnotation();
      } break;
      case SELECTION_ID_UNDEFINED: {
        reset();
      } break;
      default:
        return -1;
    }
    return 0;
}

int BasicSchemaEnumerationElementChoice::makeSelection(const char *name, int nameLength)
{
    const bdlat_SelectionInfo *selectionInfo =
                                         lookupSelectionInfo(name, nameLength);
    if (0 == selectionInfo) {
       return -1;
    }

    return makeSelection(selectionInfo->d_id);
}

BasicSchemaAnnotationElement& BasicSchemaEnumerationElementChoice::makeAnnotation()
{
    if (SELECTION_ID_ANNOTATION == d_selectionId) {
        bdlat_ValueTypeFunctions::reset(&d_annotation.object());
    }
    else {
        reset();
        new (d_annotation.buffer())
                BasicSchemaAnnotationElement(d_allocator_p);
        d_selectionId = SELECTION_ID_ANNOTATION;
    }

    return d_annotation.object();
}

BasicSchemaAnnotationElement& BasicSchemaEnumerationElementChoice::makeAnnotation(const BasicSchemaAnnotationElement& value)
{
    if (SELECTION_ID_ANNOTATION == d_selectionId) {
        d_annotation.object() = value;
    }
    else {
        reset();
        new (d_annotation.buffer())
                BasicSchemaAnnotationElement(value, d_allocator_p);
        d_selectionId = SELECTION_ID_ANNOTATION;
    }

    return d_annotation.object();
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
BasicSchemaAnnotationElement& BasicSchemaEnumerationElementChoice::makeAnnotation(BasicSchemaAnnotationElement&& value)
{
    if (SELECTION_ID_ANNOTATION == d_selectionId) {
        d_annotation.object() = bsl::move(value);
    }
    else {
        reset();
        new (d_annotation.buffer())
                BasicSchemaAnnotationElement(bsl::move(value), d_allocator_p);
        d_selectionId = SELECTION_ID_ANNOTATION;
    }

    return d_annotation.object();
}
#endif

// ACCESSORS

bsl::ostream& BasicSchemaEnumerationElementChoice::print(
    bsl::ostream& stream,
    int           level,
    int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    switch (d_selectionId) {
      case SELECTION_ID_ANNOTATION: {
        printer.printAttribute("annotation", d_annotation.object());
      }  break;
      default:
        stream << "SELECTION UNDEFINED\n";
    }
    printer.end();
    return stream;
}


const char *BasicSchemaEnumerationElementChoice::selectionName() const
{
    switch (d_selectionId) {
      case SELECTION_ID_ANNOTATION:
        return SELECTION_INFO_ARRAY[SELECTION_INDEX_ANNOTATION].name();
      default:
        BSLS_ASSERT(SELECTION_ID_UNDEFINED == d_selectionId);
        return "(* UNDEFINED *)";
    }
}

                    // -----------------------------------
                    // class BasicSchemaEnumerationElement
                    // -----------------------------------

// CONSTANTS

const char BasicSchemaEnumerationElement::CLASS_NAME[] = "BasicSchemaEnumerationElement";

const bdlat_AttributeInfo BasicSchemaEnumerationElement::ATTRIBUTE_INFO_ARRAY[] = {
    {
        ATTRIBUTE_ID_VALUE,
        "value",
        sizeof("value") - 1,
        "",
        bdlat_FormattingMode::e_TEXT
      | bdlat_FormattingMode::e_ATTRIBUTE
    },
    {
        ATTRIBUTE_ID_CHOICE,
        "Choice",
        sizeof("Choice") - 1,
        "",
        bdlat_FormattingMode::e_DEFAULT
      | bdlat_FormattingMode::e_UNTAGGED
    }
};

// CLASS METHODS

const bdlat_AttributeInfo *BasicSchemaEnumerationElement::lookupAttributeInfo(
        const char *name,
        int         nameLength)
{
    if (bdlb::String::areEqualCaseless("annotation", name, nameLength)) {
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CHOICE];
    }

    for (int i = 0; i < 2; ++i) {
        const bdlat_AttributeInfo& attributeInfo =
                    BasicSchemaEnumerationElement::ATTRIBUTE_INFO_ARRAY[i];

        if (nameLength == attributeInfo.d_nameLength
        &&  0 == bsl::memcmp(attributeInfo.d_name_p, name, nameLength))
        {
            return &attributeInfo;
        }
    }

    return 0;
}

const bdlat_AttributeInfo *BasicSchemaEnumerationElement::lookupAttributeInfo(int id)
{
    switch (id) {
      case ATTRIBUTE_ID_VALUE:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_VALUE];
      case ATTRIBUTE_ID_CHOICE:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CHOICE];
      default:
        return 0;
    }
}

// CREATORS

BasicSchemaEnumerationElement::BasicSchemaEnumerationElement(bslma::Allocator *basicAllocator)
: d_choice(basicAllocator)
, d_value(basicAllocator)
{
}

BasicSchemaEnumerationElement::BasicSchemaEnumerationElement(const BasicSchemaEnumerationElement& original,
                                                             bslma::Allocator *basicAllocator)
: d_choice(original.d_choice, basicAllocator)
, d_value(original.d_value, basicAllocator)
{
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
BasicSchemaEnumerationElement::BasicSchemaEnumerationElement(BasicSchemaEnumerationElement&& original) noexcept
: d_choice(bsl::move(original.d_choice))
, d_value(bsl::move(original.d_value))
{
}

BasicSchemaEnumerationElement::BasicSchemaEnumerationElement(BasicSchemaEnumerationElement&& original,
                                                             bslma::Allocator *basicAllocator)
: d_choice(bsl::move(original.d_choice), basicAllocator)
, d_value(bsl::move(original.d_value), basicAllocator)
{
}
#endif

BasicSchemaEnumerationElement::~BasicSchemaEnumerationElement()
{
}

// MANIPULATORS

BasicSchemaEnumerationElement&
BasicSchemaEnumerationElement::operator=(const BasicSchemaEnumerationElement& rhs)
{
    if (this != &rhs) {
        d_value = rhs.d_value;
        d_choice = rhs.d_choice;
    }

    return *this;
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
BasicSchemaEnumerationElement&
BasicSchemaEnumerationElement::operator=(BasicSchemaEnumerationElement&& rhs)
{
    if (this != &rhs) {
        d_value = bsl::move(rhs.d_value);
        d_choice = bsl::move(rhs.d_choice);
    }

    return *this;
}
#endif

void BasicSchemaEnumerationElement::reset()
{
    bdlat_ValueTypeFunctions::reset(&d_value);
    bdlat_ValueTypeFunctions::reset(&d_choice);
}

// ACCESSORS

bsl::ostream& BasicSchemaEnumerationElement::print(
        bsl::ostream& stream,
        int           level,
        int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute("value", d_value);
    printer.printAttribute("choice", d_choice);
    printer.end();
    return stream;
}



                 // -----------------------------------------
                 // class BasicSchemaRestrictionElementChoice
                 // -----------------------------------------

// CONSTANTS

const char BasicSchemaRestrictionElementChoice::CLASS_NAME[] = "BasicSchemaRestrictionElementChoice";

const bdlat_SelectionInfo BasicSchemaRestrictionElementChoice::SELECTION_INFO_ARRAY[] = {
    {
        SELECTION_ID_ANNOTATION,
        "annotation",
        sizeof("annotation") - 1,
        "",
        bdlat_FormattingMode::e_DEFAULT
    },
    {
        SELECTION_ID_ENUMERATION,
        "enumeration",
        sizeof("enumeration") - 1,
        "",
        bdlat_FormattingMode::e_DEFAULT
    }
};

// CLASS METHODS

const bdlat_SelectionInfo *BasicSchemaRestrictionElementChoice::lookupSelectionInfo(
        const char *name,
        int         nameLength)
{
    for (int i = 0; i < 2; ++i) {
        const bdlat_SelectionInfo& selectionInfo =
                    BasicSchemaRestrictionElementChoice::SELECTION_INFO_ARRAY[i];

        if (nameLength == selectionInfo.d_nameLength
        &&  0 == bsl::memcmp(selectionInfo.d_name_p, name, nameLength))
        {
            return &selectionInfo;
        }
    }

    return 0;
}

const bdlat_SelectionInfo *BasicSchemaRestrictionElementChoice::lookupSelectionInfo(int id)
{
    switch (id) {
      case SELECTION_ID_ANNOTATION:
        return &SELECTION_INFO_ARRAY[SELECTION_INDEX_ANNOTATION];
      case SELECTION_ID_ENUMERATION:
        return &SELECTION_INFO_ARRAY[SELECTION_INDEX_ENUMERATION];
      default:
        return 0;
    }
}

// CREATORS

BasicSchemaRestrictionElementChoice::BasicSchemaRestrictionElementChoice(
    const BasicSchemaRestrictionElementChoice& original,
    bslma::Allocator *basicAllocator)
: d_selectionId(original.d_selectionId)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    switch (d_selectionId) {
      case SELECTION_ID_ANNOTATION: {
        new (d_annotation.buffer())
            BasicSchemaAnnotationElement(
                original.d_annotation.object(), d_allocator_p);
      } break;
      case SELECTION_ID_ENUMERATION: {
        new (d_enumeration.buffer())
            BasicSchemaEnumerationElement(
                original.d_enumeration.object(), d_allocator_p);
      } break;
      default:
        BSLS_ASSERT(SELECTION_ID_UNDEFINED == d_selectionId);
    }
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
BasicSchemaRestrictionElementChoice::BasicSchemaRestrictionElementChoice(BasicSchemaRestrictionElementChoice&& original) noexcept
: d_selectionId(original.d_selectionId)
, d_allocator_p(original.d_allocator_p)
{
    switch (d_selectionId) {
      case SELECTION_ID_ANNOTATION: {
        new (d_annotation.buffer())
            BasicSchemaAnnotationElement(
                bsl::move(original.d_annotation.object()), d_allocator_p);
      } break;
      case SELECTION_ID_ENUMERATION: {
        new (d_enumeration.buffer())
            BasicSchemaEnumerationElement(
                bsl::move(original.d_enumeration.object()), d_allocator_p);
      } break;
      default:
        BSLS_ASSERT(SELECTION_ID_UNDEFINED == d_selectionId);
    }
}

BasicSchemaRestrictionElementChoice::BasicSchemaRestrictionElementChoice(
    BasicSchemaRestrictionElementChoice&& original,
    bslma::Allocator *basicAllocator)
: d_selectionId(original.d_selectionId)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    switch (d_selectionId) {
      case SELECTION_ID_ANNOTATION: {
        new (d_annotation.buffer())
            BasicSchemaAnnotationElement(
                bsl::move(original.d_annotation.object()), d_allocator_p);
      } break;
      case SELECTION_ID_ENUMERATION: {
        new (d_enumeration.buffer())
            BasicSchemaEnumerationElement(
                bsl::move(original.d_enumeration.object()), d_allocator_p);
      } break;
      default:
        BSLS_ASSERT(SELECTION_ID_UNDEFINED == d_selectionId);
    }
}
#endif

// MANIPULATORS

BasicSchemaRestrictionElementChoice&
BasicSchemaRestrictionElementChoice::operator=(const BasicSchemaRestrictionElementChoice& rhs)
{
    if (this != &rhs) {
        switch (rhs.d_selectionId) {
          case SELECTION_ID_ANNOTATION: {
            makeAnnotation(rhs.d_annotation.object());
          } break;
          case SELECTION_ID_ENUMERATION: {
            makeEnumeration(rhs.d_enumeration.object());
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
BasicSchemaRestrictionElementChoice&
BasicSchemaRestrictionElementChoice::operator=(BasicSchemaRestrictionElementChoice&& rhs)
{
    if (this != &rhs) {
        switch (rhs.d_selectionId) {
          case SELECTION_ID_ANNOTATION: {
            makeAnnotation(bsl::move(rhs.d_annotation.object()));
          } break;
          case SELECTION_ID_ENUMERATION: {
            makeEnumeration(bsl::move(rhs.d_enumeration.object()));
          } break;
          default:
            BSLS_ASSERT(SELECTION_ID_UNDEFINED == rhs.d_selectionId);
            reset();
        }
    }

    return *this;
}
#endif

void BasicSchemaRestrictionElementChoice::reset()
{
    switch (d_selectionId) {
      case SELECTION_ID_ANNOTATION: {
        d_annotation.object().~BasicSchemaAnnotationElement();
      } break;
      case SELECTION_ID_ENUMERATION: {
        d_enumeration.object().~BasicSchemaEnumerationElement();
      } break;
      default:
        BSLS_ASSERT(SELECTION_ID_UNDEFINED == d_selectionId);
    }

    d_selectionId = SELECTION_ID_UNDEFINED;
}

int BasicSchemaRestrictionElementChoice::makeSelection(int selectionId)
{
    switch (selectionId) {
      case SELECTION_ID_ANNOTATION: {
        makeAnnotation();
      } break;
      case SELECTION_ID_ENUMERATION: {
        makeEnumeration();
      } break;
      case SELECTION_ID_UNDEFINED: {
        reset();
      } break;
      default:
        return -1;
    }
    return 0;
}

int BasicSchemaRestrictionElementChoice::makeSelection(const char *name, int nameLength)
{
    const bdlat_SelectionInfo *selectionInfo =
                                         lookupSelectionInfo(name, nameLength);
    if (0 == selectionInfo) {
       return -1;
    }

    return makeSelection(selectionInfo->d_id);
}

BasicSchemaAnnotationElement& BasicSchemaRestrictionElementChoice::makeAnnotation()
{
    if (SELECTION_ID_ANNOTATION == d_selectionId) {
        bdlat_ValueTypeFunctions::reset(&d_annotation.object());
    }
    else {
        reset();
        new (d_annotation.buffer())
                BasicSchemaAnnotationElement(d_allocator_p);
        d_selectionId = SELECTION_ID_ANNOTATION;
    }

    return d_annotation.object();
}

BasicSchemaAnnotationElement& BasicSchemaRestrictionElementChoice::makeAnnotation(const BasicSchemaAnnotationElement& value)
{
    if (SELECTION_ID_ANNOTATION == d_selectionId) {
        d_annotation.object() = value;
    }
    else {
        reset();
        new (d_annotation.buffer())
                BasicSchemaAnnotationElement(value, d_allocator_p);
        d_selectionId = SELECTION_ID_ANNOTATION;
    }

    return d_annotation.object();
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
BasicSchemaAnnotationElement& BasicSchemaRestrictionElementChoice::makeAnnotation(BasicSchemaAnnotationElement&& value)
{
    if (SELECTION_ID_ANNOTATION == d_selectionId) {
        d_annotation.object() = bsl::move(value);
    }
    else {
        reset();
        new (d_annotation.buffer())
                BasicSchemaAnnotationElement(bsl::move(value), d_allocator_p);
        d_selectionId = SELECTION_ID_ANNOTATION;
    }

    return d_annotation.object();
}
#endif

BasicSchemaEnumerationElement& BasicSchemaRestrictionElementChoice::makeEnumeration()
{
    if (SELECTION_ID_ENUMERATION == d_selectionId) {
        bdlat_ValueTypeFunctions::reset(&d_enumeration.object());
    }
    else {
        reset();
        new (d_enumeration.buffer())
                BasicSchemaEnumerationElement(d_allocator_p);
        d_selectionId = SELECTION_ID_ENUMERATION;
    }

    return d_enumeration.object();
}

BasicSchemaEnumerationElement& BasicSchemaRestrictionElementChoice::makeEnumeration(const BasicSchemaEnumerationElement& value)
{
    if (SELECTION_ID_ENUMERATION == d_selectionId) {
        d_enumeration.object() = value;
    }
    else {
        reset();
        new (d_enumeration.buffer())
                BasicSchemaEnumerationElement(value, d_allocator_p);
        d_selectionId = SELECTION_ID_ENUMERATION;
    }

    return d_enumeration.object();
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
BasicSchemaEnumerationElement& BasicSchemaRestrictionElementChoice::makeEnumeration(BasicSchemaEnumerationElement&& value)
{
    if (SELECTION_ID_ENUMERATION == d_selectionId) {
        d_enumeration.object() = bsl::move(value);
    }
    else {
        reset();
        new (d_enumeration.buffer())
                BasicSchemaEnumerationElement(bsl::move(value), d_allocator_p);
        d_selectionId = SELECTION_ID_ENUMERATION;
    }

    return d_enumeration.object();
}
#endif

// ACCESSORS

bsl::ostream& BasicSchemaRestrictionElementChoice::print(
    bsl::ostream& stream,
    int           level,
    int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    switch (d_selectionId) {
      case SELECTION_ID_ANNOTATION: {
        printer.printAttribute("annotation", d_annotation.object());
      }  break;
      case SELECTION_ID_ENUMERATION: {
        printer.printAttribute("enumeration", d_enumeration.object());
      }  break;
      default:
        stream << "SELECTION UNDEFINED\n";
    }
    printer.end();
    return stream;
}


const char *BasicSchemaRestrictionElementChoice::selectionName() const
{
    switch (d_selectionId) {
      case SELECTION_ID_ANNOTATION:
        return SELECTION_INFO_ARRAY[SELECTION_INDEX_ANNOTATION].name();
      case SELECTION_ID_ENUMERATION:
        return SELECTION_INFO_ARRAY[SELECTION_INDEX_ENUMERATION].name();
      default:
        BSLS_ASSERT(SELECTION_ID_UNDEFINED == d_selectionId);
        return "(* UNDEFINED *)";
    }
}

                    // -----------------------------------
                    // class BasicSchemaRestrictionElement
                    // -----------------------------------

// CONSTANTS

const char BasicSchemaRestrictionElement::CLASS_NAME[] = "BasicSchemaRestrictionElement";

const bdlat_AttributeInfo BasicSchemaRestrictionElement::ATTRIBUTE_INFO_ARRAY[] = {
    {
        ATTRIBUTE_ID_BASE,
        "base",
        sizeof("base") - 1,
        "",
        bdlat_FormattingMode::e_TEXT
      | bdlat_FormattingMode::e_ATTRIBUTE
    },
    {
        ATTRIBUTE_ID_CHOICE,
        "Choice",
        sizeof("Choice") - 1,
        "",
        bdlat_FormattingMode::e_DEFAULT
      | bdlat_FormattingMode::e_UNTAGGED
    }
};

// CLASS METHODS

const bdlat_AttributeInfo *BasicSchemaRestrictionElement::lookupAttributeInfo(
        const char *name,
        int         nameLength)
{
    if (bdlb::String::areEqualCaseless("annotation", name, nameLength)) {
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CHOICE];
    }

    if (bdlb::String::areEqualCaseless("enumeration", name, nameLength)) {
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CHOICE];
    }

    for (int i = 0; i < 2; ++i) {
        const bdlat_AttributeInfo& attributeInfo =
                    BasicSchemaRestrictionElement::ATTRIBUTE_INFO_ARRAY[i];

        if (nameLength == attributeInfo.d_nameLength
        &&  0 == bsl::memcmp(attributeInfo.d_name_p, name, nameLength))
        {
            return &attributeInfo;
        }
    }

    return 0;
}

const bdlat_AttributeInfo *BasicSchemaRestrictionElement::lookupAttributeInfo(int id)
{
    switch (id) {
      case ATTRIBUTE_ID_BASE:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_BASE];
      case ATTRIBUTE_ID_CHOICE:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CHOICE];
      default:
        return 0;
    }
}

// CREATORS

BasicSchemaRestrictionElement::BasicSchemaRestrictionElement(bslma::Allocator *basicAllocator)
: d_choice(basicAllocator)
, d_base(basicAllocator)
{
}

BasicSchemaRestrictionElement::BasicSchemaRestrictionElement(const BasicSchemaRestrictionElement& original,
                                                             bslma::Allocator *basicAllocator)
: d_choice(original.d_choice, basicAllocator)
, d_base(original.d_base, basicAllocator)
{
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
BasicSchemaRestrictionElement::BasicSchemaRestrictionElement(BasicSchemaRestrictionElement&& original) noexcept
: d_choice(bsl::move(original.d_choice))
, d_base(bsl::move(original.d_base))
{
}

BasicSchemaRestrictionElement::BasicSchemaRestrictionElement(BasicSchemaRestrictionElement&& original,
                                                             bslma::Allocator *basicAllocator)
: d_choice(bsl::move(original.d_choice), basicAllocator)
, d_base(bsl::move(original.d_base), basicAllocator)
{
}
#endif

BasicSchemaRestrictionElement::~BasicSchemaRestrictionElement()
{
}

// MANIPULATORS

BasicSchemaRestrictionElement&
BasicSchemaRestrictionElement::operator=(const BasicSchemaRestrictionElement& rhs)
{
    if (this != &rhs) {
        d_base = rhs.d_base;
        d_choice = rhs.d_choice;
    }

    return *this;
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
BasicSchemaRestrictionElement&
BasicSchemaRestrictionElement::operator=(BasicSchemaRestrictionElement&& rhs)
{
    if (this != &rhs) {
        d_base = bsl::move(rhs.d_base);
        d_choice = bsl::move(rhs.d_choice);
    }

    return *this;
}
#endif

void BasicSchemaRestrictionElement::reset()
{
    bdlat_ValueTypeFunctions::reset(&d_base);
    bdlat_ValueTypeFunctions::reset(&d_choice);
}

// ACCESSORS

bsl::ostream& BasicSchemaRestrictionElement::print(
        bsl::ostream& stream,
        int           level,
        int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute("base", d_base);
    printer.printAttribute("choice", d_choice);
    printer.end();
    return stream;
}



                  // ----------------------------------------
                  // class BasicSchemaSimpleTypeElementChoice
                  // ----------------------------------------

// CONSTANTS

const char BasicSchemaSimpleTypeElementChoice::CLASS_NAME[] = "BasicSchemaSimpleTypeElementChoice";

const bdlat_SelectionInfo BasicSchemaSimpleTypeElementChoice::SELECTION_INFO_ARRAY[] = {
    {
        SELECTION_ID_ANNOTATION,
        "annotation",
        sizeof("annotation") - 1,
        "",
        bdlat_FormattingMode::e_DEFAULT
    },
    {
        SELECTION_ID_RESTRICTION,
        "restriction",
        sizeof("restriction") - 1,
        "",
        bdlat_FormattingMode::e_DEFAULT
    }
};

// CLASS METHODS

const bdlat_SelectionInfo *BasicSchemaSimpleTypeElementChoice::lookupSelectionInfo(
        const char *name,
        int         nameLength)
{
    for (int i = 0; i < 2; ++i) {
        const bdlat_SelectionInfo& selectionInfo =
                    BasicSchemaSimpleTypeElementChoice::SELECTION_INFO_ARRAY[i];

        if (nameLength == selectionInfo.d_nameLength
        &&  0 == bsl::memcmp(selectionInfo.d_name_p, name, nameLength))
        {
            return &selectionInfo;
        }
    }

    return 0;
}

const bdlat_SelectionInfo *BasicSchemaSimpleTypeElementChoice::lookupSelectionInfo(int id)
{
    switch (id) {
      case SELECTION_ID_ANNOTATION:
        return &SELECTION_INFO_ARRAY[SELECTION_INDEX_ANNOTATION];
      case SELECTION_ID_RESTRICTION:
        return &SELECTION_INFO_ARRAY[SELECTION_INDEX_RESTRICTION];
      default:
        return 0;
    }
}

// CREATORS

BasicSchemaSimpleTypeElementChoice::BasicSchemaSimpleTypeElementChoice(
    const BasicSchemaSimpleTypeElementChoice& original,
    bslma::Allocator *basicAllocator)
: d_selectionId(original.d_selectionId)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    switch (d_selectionId) {
      case SELECTION_ID_ANNOTATION: {
        new (d_annotation.buffer())
            BasicSchemaAnnotationElement(
                original.d_annotation.object(), d_allocator_p);
      } break;
      case SELECTION_ID_RESTRICTION: {
        new (d_restriction.buffer())
            BasicSchemaRestrictionElement(
                original.d_restriction.object(), d_allocator_p);
      } break;
      default:
        BSLS_ASSERT(SELECTION_ID_UNDEFINED == d_selectionId);
    }
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
BasicSchemaSimpleTypeElementChoice::BasicSchemaSimpleTypeElementChoice(BasicSchemaSimpleTypeElementChoice&& original) noexcept
: d_selectionId(original.d_selectionId)
, d_allocator_p(original.d_allocator_p)
{
    switch (d_selectionId) {
      case SELECTION_ID_ANNOTATION: {
        new (d_annotation.buffer())
            BasicSchemaAnnotationElement(
                bsl::move(original.d_annotation.object()), d_allocator_p);
      } break;
      case SELECTION_ID_RESTRICTION: {
        new (d_restriction.buffer())
            BasicSchemaRestrictionElement(
                bsl::move(original.d_restriction.object()), d_allocator_p);
      } break;
      default:
        BSLS_ASSERT(SELECTION_ID_UNDEFINED == d_selectionId);
    }
}

BasicSchemaSimpleTypeElementChoice::BasicSchemaSimpleTypeElementChoice(
    BasicSchemaSimpleTypeElementChoice&& original,
    bslma::Allocator *basicAllocator)
: d_selectionId(original.d_selectionId)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    switch (d_selectionId) {
      case SELECTION_ID_ANNOTATION: {
        new (d_annotation.buffer())
            BasicSchemaAnnotationElement(
                bsl::move(original.d_annotation.object()), d_allocator_p);
      } break;
      case SELECTION_ID_RESTRICTION: {
        new (d_restriction.buffer())
            BasicSchemaRestrictionElement(
                bsl::move(original.d_restriction.object()), d_allocator_p);
      } break;
      default:
        BSLS_ASSERT(SELECTION_ID_UNDEFINED == d_selectionId);
    }
}
#endif

// MANIPULATORS

BasicSchemaSimpleTypeElementChoice&
BasicSchemaSimpleTypeElementChoice::operator=(const BasicSchemaSimpleTypeElementChoice& rhs)
{
    if (this != &rhs) {
        switch (rhs.d_selectionId) {
          case SELECTION_ID_ANNOTATION: {
            makeAnnotation(rhs.d_annotation.object());
          } break;
          case SELECTION_ID_RESTRICTION: {
            makeRestriction(rhs.d_restriction.object());
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
BasicSchemaSimpleTypeElementChoice&
BasicSchemaSimpleTypeElementChoice::operator=(BasicSchemaSimpleTypeElementChoice&& rhs)
{
    if (this != &rhs) {
        switch (rhs.d_selectionId) {
          case SELECTION_ID_ANNOTATION: {
            makeAnnotation(bsl::move(rhs.d_annotation.object()));
          } break;
          case SELECTION_ID_RESTRICTION: {
            makeRestriction(bsl::move(rhs.d_restriction.object()));
          } break;
          default:
            BSLS_ASSERT(SELECTION_ID_UNDEFINED == rhs.d_selectionId);
            reset();
        }
    }

    return *this;
}
#endif

void BasicSchemaSimpleTypeElementChoice::reset()
{
    switch (d_selectionId) {
      case SELECTION_ID_ANNOTATION: {
        d_annotation.object().~BasicSchemaAnnotationElement();
      } break;
      case SELECTION_ID_RESTRICTION: {
        d_restriction.object().~BasicSchemaRestrictionElement();
      } break;
      default:
        BSLS_ASSERT(SELECTION_ID_UNDEFINED == d_selectionId);
    }

    d_selectionId = SELECTION_ID_UNDEFINED;
}

int BasicSchemaSimpleTypeElementChoice::makeSelection(int selectionId)
{
    switch (selectionId) {
      case SELECTION_ID_ANNOTATION: {
        makeAnnotation();
      } break;
      case SELECTION_ID_RESTRICTION: {
        makeRestriction();
      } break;
      case SELECTION_ID_UNDEFINED: {
        reset();
      } break;
      default:
        return -1;
    }
    return 0;
}

int BasicSchemaSimpleTypeElementChoice::makeSelection(const char *name, int nameLength)
{
    const bdlat_SelectionInfo *selectionInfo =
                                         lookupSelectionInfo(name, nameLength);
    if (0 == selectionInfo) {
       return -1;
    }

    return makeSelection(selectionInfo->d_id);
}

BasicSchemaAnnotationElement& BasicSchemaSimpleTypeElementChoice::makeAnnotation()
{
    if (SELECTION_ID_ANNOTATION == d_selectionId) {
        bdlat_ValueTypeFunctions::reset(&d_annotation.object());
    }
    else {
        reset();
        new (d_annotation.buffer())
                BasicSchemaAnnotationElement(d_allocator_p);
        d_selectionId = SELECTION_ID_ANNOTATION;
    }

    return d_annotation.object();
}

BasicSchemaAnnotationElement& BasicSchemaSimpleTypeElementChoice::makeAnnotation(const BasicSchemaAnnotationElement& value)
{
    if (SELECTION_ID_ANNOTATION == d_selectionId) {
        d_annotation.object() = value;
    }
    else {
        reset();
        new (d_annotation.buffer())
                BasicSchemaAnnotationElement(value, d_allocator_p);
        d_selectionId = SELECTION_ID_ANNOTATION;
    }

    return d_annotation.object();
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
BasicSchemaAnnotationElement& BasicSchemaSimpleTypeElementChoice::makeAnnotation(BasicSchemaAnnotationElement&& value)
{
    if (SELECTION_ID_ANNOTATION == d_selectionId) {
        d_annotation.object() = bsl::move(value);
    }
    else {
        reset();
        new (d_annotation.buffer())
                BasicSchemaAnnotationElement(bsl::move(value), d_allocator_p);
        d_selectionId = SELECTION_ID_ANNOTATION;
    }

    return d_annotation.object();
}
#endif

BasicSchemaRestrictionElement& BasicSchemaSimpleTypeElementChoice::makeRestriction()
{
    if (SELECTION_ID_RESTRICTION == d_selectionId) {
        bdlat_ValueTypeFunctions::reset(&d_restriction.object());
    }
    else {
        reset();
        new (d_restriction.buffer())
                BasicSchemaRestrictionElement(d_allocator_p);
        d_selectionId = SELECTION_ID_RESTRICTION;
    }

    return d_restriction.object();
}

BasicSchemaRestrictionElement& BasicSchemaSimpleTypeElementChoice::makeRestriction(const BasicSchemaRestrictionElement& value)
{
    if (SELECTION_ID_RESTRICTION == d_selectionId) {
        d_restriction.object() = value;
    }
    else {
        reset();
        new (d_restriction.buffer())
                BasicSchemaRestrictionElement(value, d_allocator_p);
        d_selectionId = SELECTION_ID_RESTRICTION;
    }

    return d_restriction.object();
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
BasicSchemaRestrictionElement& BasicSchemaSimpleTypeElementChoice::makeRestriction(BasicSchemaRestrictionElement&& value)
{
    if (SELECTION_ID_RESTRICTION == d_selectionId) {
        d_restriction.object() = bsl::move(value);
    }
    else {
        reset();
        new (d_restriction.buffer())
                BasicSchemaRestrictionElement(bsl::move(value), d_allocator_p);
        d_selectionId = SELECTION_ID_RESTRICTION;
    }

    return d_restriction.object();
}
#endif

// ACCESSORS

bsl::ostream& BasicSchemaSimpleTypeElementChoice::print(
    bsl::ostream& stream,
    int           level,
    int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    switch (d_selectionId) {
      case SELECTION_ID_ANNOTATION: {
        printer.printAttribute("annotation", d_annotation.object());
      }  break;
      case SELECTION_ID_RESTRICTION: {
        printer.printAttribute("restriction", d_restriction.object());
      }  break;
      default:
        stream << "SELECTION UNDEFINED\n";
    }
    printer.end();
    return stream;
}


const char *BasicSchemaSimpleTypeElementChoice::selectionName() const
{
    switch (d_selectionId) {
      case SELECTION_ID_ANNOTATION:
        return SELECTION_INFO_ARRAY[SELECTION_INDEX_ANNOTATION].name();
      case SELECTION_ID_RESTRICTION:
        return SELECTION_INFO_ARRAY[SELECTION_INDEX_RESTRICTION].name();
      default:
        BSLS_ASSERT(SELECTION_ID_UNDEFINED == d_selectionId);
        return "(* UNDEFINED *)";
    }
}

                     // ----------------------------------
                     // class BasicSchemaSimpleTypeElement
                     // ----------------------------------

// CONSTANTS

const char BasicSchemaSimpleTypeElement::CLASS_NAME[] = "BasicSchemaSimpleTypeElement";

const bdlat_AttributeInfo BasicSchemaSimpleTypeElement::ATTRIBUTE_INFO_ARRAY[] = {
    {
        ATTRIBUTE_ID_NAME,
        "name",
        sizeof("name") - 1,
        "",
        bdlat_FormattingMode::e_TEXT
      | bdlat_FormattingMode::e_ATTRIBUTE
    },
    {
        ATTRIBUTE_ID_PRESERVE_ENUM_ORDER,
        "preserveEnumOrder",
        sizeof("preserveEnumOrder") - 1,
        "",
        bdlat_FormattingMode::e_TEXT
      | bdlat_FormattingMode::e_ATTRIBUTE
    },
    {
        ATTRIBUTE_ID_CHOICE,
        "Choice",
        sizeof("Choice") - 1,
        "",
        bdlat_FormattingMode::e_DEFAULT
      | bdlat_FormattingMode::e_UNTAGGED
    }
};

// CLASS METHODS

const bdlat_AttributeInfo *BasicSchemaSimpleTypeElement::lookupAttributeInfo(
        const char *name,
        int         nameLength)
{
    if (bdlb::String::areEqualCaseless("annotation", name, nameLength)) {
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CHOICE];
    }

    if (bdlb::String::areEqualCaseless("restriction", name, nameLength)) {
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CHOICE];
    }

    for (int i = 0; i < 3; ++i) {
        const bdlat_AttributeInfo& attributeInfo =
                    BasicSchemaSimpleTypeElement::ATTRIBUTE_INFO_ARRAY[i];

        if (nameLength == attributeInfo.d_nameLength
        &&  0 == bsl::memcmp(attributeInfo.d_name_p, name, nameLength))
        {
            return &attributeInfo;
        }
    }

    return 0;
}

const bdlat_AttributeInfo *BasicSchemaSimpleTypeElement::lookupAttributeInfo(int id)
{
    switch (id) {
      case ATTRIBUTE_ID_NAME:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_NAME];
      case ATTRIBUTE_ID_PRESERVE_ENUM_ORDER:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_PRESERVE_ENUM_ORDER];
      case ATTRIBUTE_ID_CHOICE:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CHOICE];
      default:
        return 0;
    }
}

// CREATORS

BasicSchemaSimpleTypeElement::BasicSchemaSimpleTypeElement(bslma::Allocator *basicAllocator)
: d_choice(basicAllocator)
, d_name(basicAllocator)
, d_preserveEnumOrder(basicAllocator)
{
}

BasicSchemaSimpleTypeElement::BasicSchemaSimpleTypeElement(const BasicSchemaSimpleTypeElement& original,
                                                           bslma::Allocator *basicAllocator)
: d_choice(original.d_choice, basicAllocator)
, d_name(original.d_name, basicAllocator)
, d_preserveEnumOrder(original.d_preserveEnumOrder, basicAllocator)
{
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
BasicSchemaSimpleTypeElement::BasicSchemaSimpleTypeElement(BasicSchemaSimpleTypeElement&& original) noexcept
: d_choice(bsl::move(original.d_choice))
, d_name(bsl::move(original.d_name))
, d_preserveEnumOrder(bsl::move(original.d_preserveEnumOrder))
{
}

BasicSchemaSimpleTypeElement::BasicSchemaSimpleTypeElement(BasicSchemaSimpleTypeElement&& original,
                                                           bslma::Allocator *basicAllocator)
: d_choice(bsl::move(original.d_choice), basicAllocator)
, d_name(bsl::move(original.d_name), basicAllocator)
, d_preserveEnumOrder(bsl::move(original.d_preserveEnumOrder), basicAllocator)
{
}
#endif

BasicSchemaSimpleTypeElement::~BasicSchemaSimpleTypeElement()
{
}

// MANIPULATORS

BasicSchemaSimpleTypeElement&
BasicSchemaSimpleTypeElement::operator=(const BasicSchemaSimpleTypeElement& rhs)
{
    if (this != &rhs) {
        d_name = rhs.d_name;
        d_preserveEnumOrder = rhs.d_preserveEnumOrder;
        d_choice = rhs.d_choice;
    }

    return *this;
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
BasicSchemaSimpleTypeElement&
BasicSchemaSimpleTypeElement::operator=(BasicSchemaSimpleTypeElement&& rhs)
{
    if (this != &rhs) {
        d_name = bsl::move(rhs.d_name);
        d_preserveEnumOrder = bsl::move(rhs.d_preserveEnumOrder);
        d_choice = bsl::move(rhs.d_choice);
    }

    return *this;
}
#endif

void BasicSchemaSimpleTypeElement::reset()
{
    bdlat_ValueTypeFunctions::reset(&d_name);
    bdlat_ValueTypeFunctions::reset(&d_preserveEnumOrder);
    bdlat_ValueTypeFunctions::reset(&d_choice);
}

// ACCESSORS

bsl::ostream& BasicSchemaSimpleTypeElement::print(
        bsl::ostream& stream,
        int           level,
        int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute("name", d_name);
    printer.printAttribute("preserveEnumOrder", d_preserveEnumOrder);
    printer.printAttribute("choice", d_choice);
    printer.end();
    return stream;
}



                       // ------------------------------
                       // class BasicSchemaChoiceElement
                       // ------------------------------

// CONSTANTS

const char BasicSchemaChoiceElement::CLASS_NAME[] = "BasicSchemaChoiceElement";

const bdlat_AttributeInfo BasicSchemaChoiceElement::ATTRIBUTE_INFO_ARRAY[] = {
    {
        ATTRIBUTE_ID_CHOICE,
        "Choice",
        sizeof("Choice") - 1,
        "",
        bdlat_FormattingMode::e_DEFAULT
      | bdlat_FormattingMode::e_UNTAGGED
    }
};

// CLASS METHODS

const bdlat_AttributeInfo *BasicSchemaChoiceElement::lookupAttributeInfo(
        const char *name,
        int         nameLength)
{
    if (bdlb::String::areEqualCaseless("annotation", name, nameLength)) {
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CHOICE];
    }

    if (bdlb::String::areEqualCaseless("element", name, nameLength)) {
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CHOICE];
    }

    for (int i = 0; i < 1; ++i) {
        const bdlat_AttributeInfo& attributeInfo =
                    BasicSchemaChoiceElement::ATTRIBUTE_INFO_ARRAY[i];

        if (nameLength == attributeInfo.d_nameLength
        &&  0 == bsl::memcmp(attributeInfo.d_name_p, name, nameLength))
        {
            return &attributeInfo;
        }
    }

    return 0;
}

const bdlat_AttributeInfo *BasicSchemaChoiceElement::lookupAttributeInfo(int id)
{
    switch (id) {
      case ATTRIBUTE_ID_CHOICE:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CHOICE];
      default:
        return 0;
    }
}

// CREATORS

BasicSchemaChoiceElement::BasicSchemaChoiceElement(bslma::Allocator *basicAllocator)
: d_choice(basicAllocator)
{
}

BasicSchemaChoiceElement::BasicSchemaChoiceElement(const BasicSchemaChoiceElement& original,
                                                   bslma::Allocator *basicAllocator)
: d_choice(original.d_choice, basicAllocator)
{
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
BasicSchemaChoiceElement::BasicSchemaChoiceElement(BasicSchemaChoiceElement&& original) noexcept
: d_choice(bsl::move(original.d_choice))
{
}

BasicSchemaChoiceElement::BasicSchemaChoiceElement(BasicSchemaChoiceElement&& original,
                                                   bslma::Allocator *basicAllocator)
: d_choice(bsl::move(original.d_choice), basicAllocator)
{
}
#endif

BasicSchemaChoiceElement::~BasicSchemaChoiceElement()
{
}

// MANIPULATORS

BasicSchemaChoiceElement&
BasicSchemaChoiceElement::operator=(const BasicSchemaChoiceElement& rhs)
{
    if (this != &rhs) {
        d_choice = rhs.d_choice;
    }

    return *this;
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
BasicSchemaChoiceElement&
BasicSchemaChoiceElement::operator=(BasicSchemaChoiceElement&& rhs)
{
    if (this != &rhs) {
        d_choice = bsl::move(rhs.d_choice);
    }

    return *this;
}
#endif

void BasicSchemaChoiceElement::reset()
{
    bdlat_ValueTypeFunctions::reset(&d_choice);
}

// ACCESSORS

bsl::ostream& BasicSchemaChoiceElement::print(
        bsl::ostream& stream,
        int           level,
        int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute("choice", d_choice);
    printer.end();
    return stream;
}



                    // ------------------------------------
                    // class BasicSchemaChoiceElementChoice
                    // ------------------------------------

// CONSTANTS

const char BasicSchemaChoiceElementChoice::CLASS_NAME[] = "BasicSchemaChoiceElementChoice";

const bdlat_SelectionInfo BasicSchemaChoiceElementChoice::SELECTION_INFO_ARRAY[] = {
    {
        SELECTION_ID_ANNOTATION,
        "annotation",
        sizeof("annotation") - 1,
        "",
        bdlat_FormattingMode::e_DEFAULT
    },
    {
        SELECTION_ID_ELEMENT,
        "element",
        sizeof("element") - 1,
        "",
        bdlat_FormattingMode::e_DEFAULT
    }
};

// CLASS METHODS

const bdlat_SelectionInfo *BasicSchemaChoiceElementChoice::lookupSelectionInfo(
        const char *name,
        int         nameLength)
{
    for (int i = 0; i < 2; ++i) {
        const bdlat_SelectionInfo& selectionInfo =
                    BasicSchemaChoiceElementChoice::SELECTION_INFO_ARRAY[i];

        if (nameLength == selectionInfo.d_nameLength
        &&  0 == bsl::memcmp(selectionInfo.d_name_p, name, nameLength))
        {
            return &selectionInfo;
        }
    }

    return 0;
}

const bdlat_SelectionInfo *BasicSchemaChoiceElementChoice::lookupSelectionInfo(int id)
{
    switch (id) {
      case SELECTION_ID_ANNOTATION:
        return &SELECTION_INFO_ARRAY[SELECTION_INDEX_ANNOTATION];
      case SELECTION_ID_ELEMENT:
        return &SELECTION_INFO_ARRAY[SELECTION_INDEX_ELEMENT];
      default:
        return 0;
    }
}

// CREATORS

BasicSchemaChoiceElementChoice::BasicSchemaChoiceElementChoice(
    const BasicSchemaChoiceElementChoice& original,
    bslma::Allocator *basicAllocator)
: d_selectionId(original.d_selectionId)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    switch (d_selectionId) {
      case SELECTION_ID_ANNOTATION: {
        new (d_annotation.buffer())
            BasicSchemaAnnotationElement(
                original.d_annotation.object(), d_allocator_p);
      } break;
      case SELECTION_ID_ELEMENT: {
        d_element = new (*d_allocator_p)
                BasicSchemaElement(*original.d_element, d_allocator_p);
      } break;
      default:
        BSLS_ASSERT(SELECTION_ID_UNDEFINED == d_selectionId);
    }
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
BasicSchemaChoiceElementChoice::BasicSchemaChoiceElementChoice(BasicSchemaChoiceElementChoice&& original) noexcept
: d_selectionId(original.d_selectionId)
, d_allocator_p(original.d_allocator_p)
{
    switch (d_selectionId) {
      case SELECTION_ID_ANNOTATION: {
        new (d_annotation.buffer())
            BasicSchemaAnnotationElement(
                bsl::move(original.d_annotation.object()), d_allocator_p);
      } break;
      case SELECTION_ID_ELEMENT: {
        d_element = original.d_element;
        original.d_selectionId = SELECTION_ID_UNDEFINED;
      } break;
      default:
        BSLS_ASSERT(SELECTION_ID_UNDEFINED == d_selectionId);
    }
}

BasicSchemaChoiceElementChoice::BasicSchemaChoiceElementChoice(
    BasicSchemaChoiceElementChoice&& original,
    bslma::Allocator *basicAllocator)
: d_selectionId(original.d_selectionId)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    switch (d_selectionId) {
      case SELECTION_ID_ANNOTATION: {
        new (d_annotation.buffer())
            BasicSchemaAnnotationElement(
                bsl::move(original.d_annotation.object()), d_allocator_p);
      } break;
      case SELECTION_ID_ELEMENT: {
        if (d_allocator_p == original.d_allocator_p) {
            d_element = original.d_element;
            original.d_selectionId = SELECTION_ID_UNDEFINED;
        }
        else {
            d_element = new (*d_allocator_p)
                    BasicSchemaElement(bsl::move(*original.d_element), d_allocator_p);
        }
      } break;
      default:
        BSLS_ASSERT(SELECTION_ID_UNDEFINED == d_selectionId);
    }
}
#endif

// MANIPULATORS

BasicSchemaChoiceElementChoice&
BasicSchemaChoiceElementChoice::operator=(const BasicSchemaChoiceElementChoice& rhs)
{
    if (this != &rhs) {
        switch (rhs.d_selectionId) {
          case SELECTION_ID_ANNOTATION: {
            makeAnnotation(rhs.d_annotation.object());
          } break;
          case SELECTION_ID_ELEMENT: {
            makeElement(*rhs.d_element);
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
BasicSchemaChoiceElementChoice&
BasicSchemaChoiceElementChoice::operator=(BasicSchemaChoiceElementChoice&& rhs)
{
    if (this != &rhs) {
        switch (rhs.d_selectionId) {
          case SELECTION_ID_ANNOTATION: {
            makeAnnotation(bsl::move(rhs.d_annotation.object()));
          } break;
          case SELECTION_ID_ELEMENT: {
            if (d_allocator_p == rhs.d_allocator_p) {
                reset();
                d_element = rhs.d_element;
                d_selectionId = SELECTION_ID_ELEMENT;
                rhs.d_selectionId = SELECTION_ID_UNDEFINED;
            }
            else {
                makeElement(bsl::move(*rhs.d_element));
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

void BasicSchemaChoiceElementChoice::reset()
{
    switch (d_selectionId) {
      case SELECTION_ID_ANNOTATION: {
        d_annotation.object().~BasicSchemaAnnotationElement();
      } break;
      case SELECTION_ID_ELEMENT: {
        d_allocator_p->deleteObject(d_element);
      } break;
      default:
        BSLS_ASSERT(SELECTION_ID_UNDEFINED == d_selectionId);
    }

    d_selectionId = SELECTION_ID_UNDEFINED;
}

int BasicSchemaChoiceElementChoice::makeSelection(int selectionId)
{
    switch (selectionId) {
      case SELECTION_ID_ANNOTATION: {
        makeAnnotation();
      } break;
      case SELECTION_ID_ELEMENT: {
        makeElement();
      } break;
      case SELECTION_ID_UNDEFINED: {
        reset();
      } break;
      default:
        return -1;
    }
    return 0;
}

int BasicSchemaChoiceElementChoice::makeSelection(const char *name, int nameLength)
{
    const bdlat_SelectionInfo *selectionInfo =
                                         lookupSelectionInfo(name, nameLength);
    if (0 == selectionInfo) {
       return -1;
    }

    return makeSelection(selectionInfo->d_id);
}

BasicSchemaAnnotationElement& BasicSchemaChoiceElementChoice::makeAnnotation()
{
    if (SELECTION_ID_ANNOTATION == d_selectionId) {
        bdlat_ValueTypeFunctions::reset(&d_annotation.object());
    }
    else {
        reset();
        new (d_annotation.buffer())
                BasicSchemaAnnotationElement(d_allocator_p);
        d_selectionId = SELECTION_ID_ANNOTATION;
    }

    return d_annotation.object();
}

BasicSchemaAnnotationElement& BasicSchemaChoiceElementChoice::makeAnnotation(const BasicSchemaAnnotationElement& value)
{
    if (SELECTION_ID_ANNOTATION == d_selectionId) {
        d_annotation.object() = value;
    }
    else {
        reset();
        new (d_annotation.buffer())
                BasicSchemaAnnotationElement(value, d_allocator_p);
        d_selectionId = SELECTION_ID_ANNOTATION;
    }

    return d_annotation.object();
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
BasicSchemaAnnotationElement& BasicSchemaChoiceElementChoice::makeAnnotation(BasicSchemaAnnotationElement&& value)
{
    if (SELECTION_ID_ANNOTATION == d_selectionId) {
        d_annotation.object() = bsl::move(value);
    }
    else {
        reset();
        new (d_annotation.buffer())
                BasicSchemaAnnotationElement(bsl::move(value), d_allocator_p);
        d_selectionId = SELECTION_ID_ANNOTATION;
    }

    return d_annotation.object();
}
#endif

BasicSchemaElement& BasicSchemaChoiceElementChoice::makeElement()
{
    if (SELECTION_ID_ELEMENT == d_selectionId) {
        bdlat_ValueTypeFunctions::reset(d_element);
    }
    else {
        reset();
        d_element = new (*d_allocator_p)
                BasicSchemaElement(d_allocator_p);
        d_selectionId = SELECTION_ID_ELEMENT;
    }

    return *d_element;
}

BasicSchemaElement& BasicSchemaChoiceElementChoice::makeElement(const BasicSchemaElement& value)
{
    if (SELECTION_ID_ELEMENT == d_selectionId) {
        *d_element = value;
    }
    else {
        reset();
        d_element = new (*d_allocator_p)
                BasicSchemaElement(value, d_allocator_p);
        d_selectionId = SELECTION_ID_ELEMENT;
    }

    return *d_element;
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
BasicSchemaElement& BasicSchemaChoiceElementChoice::makeElement(BasicSchemaElement&& value)
{
    if (SELECTION_ID_ELEMENT == d_selectionId) {
        *d_element = bsl::move(value);
    }
    else {
        reset();
        d_element = new (*d_allocator_p)
                BasicSchemaElement(bsl::move(value), d_allocator_p);
        d_selectionId = SELECTION_ID_ELEMENT;
    }

    return *d_element;
}
#endif

// ACCESSORS

bsl::ostream& BasicSchemaChoiceElementChoice::print(
    bsl::ostream& stream,
    int           level,
    int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    switch (d_selectionId) {
      case SELECTION_ID_ANNOTATION: {
        printer.printAttribute("annotation", d_annotation.object());
      }  break;
      case SELECTION_ID_ELEMENT: {
    printer.printAttribute("element", *d_element);
      }  break;
      default:
        stream << "SELECTION UNDEFINED\n";
    }
    printer.end();
    return stream;
}


const char *BasicSchemaChoiceElementChoice::selectionName() const
{
    switch (d_selectionId) {
      case SELECTION_ID_ANNOTATION:
        return SELECTION_INFO_ARRAY[SELECTION_INDEX_ANNOTATION].name();
      case SELECTION_ID_ELEMENT:
        return SELECTION_INFO_ARRAY[SELECTION_INDEX_ELEMENT].name();
      default:
        BSLS_ASSERT(SELECTION_ID_UNDEFINED == d_selectionId);
        return "(* UNDEFINED *)";
    }
}

                    // -----------------------------------
                    // class BasicSchemaComplexTypeElement
                    // -----------------------------------

// CONSTANTS

const char BasicSchemaComplexTypeElement::CLASS_NAME[] = "BasicSchemaComplexTypeElement";

const bdlat_AttributeInfo BasicSchemaComplexTypeElement::ATTRIBUTE_INFO_ARRAY[] = {
    {
        ATTRIBUTE_ID_NAME,
        "name",
        sizeof("name") - 1,
        "",
        bdlat_FormattingMode::e_TEXT
      | bdlat_FormattingMode::e_ATTRIBUTE
    },
    {
        ATTRIBUTE_ID_CHOICE,
        "Choice",
        sizeof("Choice") - 1,
        "",
        bdlat_FormattingMode::e_DEFAULT
      | bdlat_FormattingMode::e_UNTAGGED
    }
};

// CLASS METHODS

const bdlat_AttributeInfo *BasicSchemaComplexTypeElement::lookupAttributeInfo(
        const char *name,
        int         nameLength)
{
    if (bdlb::String::areEqualCaseless("annotation", name, nameLength)) {
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CHOICE];
    }

    if (bdlb::String::areEqualCaseless("sequence", name, nameLength)) {
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CHOICE];
    }

    if (bdlb::String::areEqualCaseless("choice", name, nameLength)) {
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CHOICE];
    }

    for (int i = 0; i < 2; ++i) {
        const bdlat_AttributeInfo& attributeInfo =
                    BasicSchemaComplexTypeElement::ATTRIBUTE_INFO_ARRAY[i];

        if (nameLength == attributeInfo.d_nameLength
        &&  0 == bsl::memcmp(attributeInfo.d_name_p, name, nameLength))
        {
            return &attributeInfo;
        }
    }

    return 0;
}

const bdlat_AttributeInfo *BasicSchemaComplexTypeElement::lookupAttributeInfo(int id)
{
    switch (id) {
      case ATTRIBUTE_ID_NAME:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_NAME];
      case ATTRIBUTE_ID_CHOICE:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CHOICE];
      default:
        return 0;
    }
}

// CREATORS

BasicSchemaComplexTypeElement::BasicSchemaComplexTypeElement(bslma::Allocator *basicAllocator)
: d_choice(basicAllocator)
, d_name(basicAllocator)
{
}

BasicSchemaComplexTypeElement::BasicSchemaComplexTypeElement(const BasicSchemaComplexTypeElement& original,
                                                             bslma::Allocator *basicAllocator)
: d_choice(original.d_choice, basicAllocator)
, d_name(original.d_name, basicAllocator)
{
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
BasicSchemaComplexTypeElement::BasicSchemaComplexTypeElement(BasicSchemaComplexTypeElement&& original) noexcept
: d_choice(bsl::move(original.d_choice))
, d_name(bsl::move(original.d_name))
{
}

BasicSchemaComplexTypeElement::BasicSchemaComplexTypeElement(BasicSchemaComplexTypeElement&& original,
                                                             bslma::Allocator *basicAllocator)
: d_choice(bsl::move(original.d_choice), basicAllocator)
, d_name(bsl::move(original.d_name), basicAllocator)
{
}
#endif

BasicSchemaComplexTypeElement::~BasicSchemaComplexTypeElement()
{
}

// MANIPULATORS

BasicSchemaComplexTypeElement&
BasicSchemaComplexTypeElement::operator=(const BasicSchemaComplexTypeElement& rhs)
{
    if (this != &rhs) {
        d_name = rhs.d_name;
        d_choice = rhs.d_choice;
    }

    return *this;
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
BasicSchemaComplexTypeElement&
BasicSchemaComplexTypeElement::operator=(BasicSchemaComplexTypeElement&& rhs)
{
    if (this != &rhs) {
        d_name = bsl::move(rhs.d_name);
        d_choice = bsl::move(rhs.d_choice);
    }

    return *this;
}
#endif

void BasicSchemaComplexTypeElement::reset()
{
    bdlat_ValueTypeFunctions::reset(&d_name);
    bdlat_ValueTypeFunctions::reset(&d_choice);
}

// ACCESSORS

bsl::ostream& BasicSchemaComplexTypeElement::print(
        bsl::ostream& stream,
        int           level,
        int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute("name", d_name);
    printer.printAttribute("choice", d_choice);
    printer.end();
    return stream;
}



                 // -----------------------------------------
                 // class BasicSchemaComplexTypeElementChoice
                 // -----------------------------------------

// CONSTANTS

const char BasicSchemaComplexTypeElementChoice::CLASS_NAME[] = "BasicSchemaComplexTypeElementChoice";

const bdlat_SelectionInfo BasicSchemaComplexTypeElementChoice::SELECTION_INFO_ARRAY[] = {
    {
        SELECTION_ID_ANNOTATION,
        "annotation",
        sizeof("annotation") - 1,
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
        SELECTION_ID_CHOICE,
        "choice",
        sizeof("choice") - 1,
        "",
        bdlat_FormattingMode::e_DEFAULT
    }
};

// CLASS METHODS

const bdlat_SelectionInfo *BasicSchemaComplexTypeElementChoice::lookupSelectionInfo(
        const char *name,
        int         nameLength)
{
    for (int i = 0; i < 3; ++i) {
        const bdlat_SelectionInfo& selectionInfo =
                    BasicSchemaComplexTypeElementChoice::SELECTION_INFO_ARRAY[i];

        if (nameLength == selectionInfo.d_nameLength
        &&  0 == bsl::memcmp(selectionInfo.d_name_p, name, nameLength))
        {
            return &selectionInfo;
        }
    }

    return 0;
}

const bdlat_SelectionInfo *BasicSchemaComplexTypeElementChoice::lookupSelectionInfo(int id)
{
    switch (id) {
      case SELECTION_ID_ANNOTATION:
        return &SELECTION_INFO_ARRAY[SELECTION_INDEX_ANNOTATION];
      case SELECTION_ID_SEQUENCE:
        return &SELECTION_INFO_ARRAY[SELECTION_INDEX_SEQUENCE];
      case SELECTION_ID_CHOICE:
        return &SELECTION_INFO_ARRAY[SELECTION_INDEX_CHOICE];
      default:
        return 0;
    }
}

// CREATORS

BasicSchemaComplexTypeElementChoice::BasicSchemaComplexTypeElementChoice(
    const BasicSchemaComplexTypeElementChoice& original,
    bslma::Allocator *basicAllocator)
: d_selectionId(original.d_selectionId)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    switch (d_selectionId) {
      case SELECTION_ID_ANNOTATION: {
        new (d_annotation.buffer())
            BasicSchemaAnnotationElement(
                original.d_annotation.object(), d_allocator_p);
      } break;
      case SELECTION_ID_SEQUENCE: {
        d_sequence = new (*d_allocator_p)
                BasicSchemaSequenceElement(*original.d_sequence, d_allocator_p);
      } break;
      case SELECTION_ID_CHOICE: {
        d_choice = new (*d_allocator_p)
                BasicSchemaChoiceElement(*original.d_choice, d_allocator_p);
      } break;
      default:
        BSLS_ASSERT(SELECTION_ID_UNDEFINED == d_selectionId);
    }
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
BasicSchemaComplexTypeElementChoice::BasicSchemaComplexTypeElementChoice(BasicSchemaComplexTypeElementChoice&& original) noexcept
: d_selectionId(original.d_selectionId)
, d_allocator_p(original.d_allocator_p)
{
    switch (d_selectionId) {
      case SELECTION_ID_ANNOTATION: {
        new (d_annotation.buffer())
            BasicSchemaAnnotationElement(
                bsl::move(original.d_annotation.object()), d_allocator_p);
      } break;
      case SELECTION_ID_SEQUENCE: {
        d_sequence = original.d_sequence;
        original.d_selectionId = SELECTION_ID_UNDEFINED;
      } break;
      case SELECTION_ID_CHOICE: {
        d_choice = original.d_choice;
        original.d_selectionId = SELECTION_ID_UNDEFINED;
      } break;
      default:
        BSLS_ASSERT(SELECTION_ID_UNDEFINED == d_selectionId);
    }
}

BasicSchemaComplexTypeElementChoice::BasicSchemaComplexTypeElementChoice(
    BasicSchemaComplexTypeElementChoice&& original,
    bslma::Allocator *basicAllocator)
: d_selectionId(original.d_selectionId)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    switch (d_selectionId) {
      case SELECTION_ID_ANNOTATION: {
        new (d_annotation.buffer())
            BasicSchemaAnnotationElement(
                bsl::move(original.d_annotation.object()), d_allocator_p);
      } break;
      case SELECTION_ID_SEQUENCE: {
        if (d_allocator_p == original.d_allocator_p) {
            d_sequence = original.d_sequence;
            original.d_selectionId = SELECTION_ID_UNDEFINED;
        }
        else {
            d_sequence = new (*d_allocator_p)
                    BasicSchemaSequenceElement(bsl::move(*original.d_sequence), d_allocator_p);
        }
      } break;
      case SELECTION_ID_CHOICE: {
        if (d_allocator_p == original.d_allocator_p) {
            d_choice = original.d_choice;
            original.d_selectionId = SELECTION_ID_UNDEFINED;
        }
        else {
            d_choice = new (*d_allocator_p)
                    BasicSchemaChoiceElement(bsl::move(*original.d_choice), d_allocator_p);
        }
      } break;
      default:
        BSLS_ASSERT(SELECTION_ID_UNDEFINED == d_selectionId);
    }
}
#endif

// MANIPULATORS

BasicSchemaComplexTypeElementChoice&
BasicSchemaComplexTypeElementChoice::operator=(const BasicSchemaComplexTypeElementChoice& rhs)
{
    if (this != &rhs) {
        switch (rhs.d_selectionId) {
          case SELECTION_ID_ANNOTATION: {
            makeAnnotation(rhs.d_annotation.object());
          } break;
          case SELECTION_ID_SEQUENCE: {
            makeSequence(*rhs.d_sequence);
          } break;
          case SELECTION_ID_CHOICE: {
            makeChoice(*rhs.d_choice);
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
BasicSchemaComplexTypeElementChoice&
BasicSchemaComplexTypeElementChoice::operator=(BasicSchemaComplexTypeElementChoice&& rhs)
{
    if (this != &rhs) {
        switch (rhs.d_selectionId) {
          case SELECTION_ID_ANNOTATION: {
            makeAnnotation(bsl::move(rhs.d_annotation.object()));
          } break;
          case SELECTION_ID_SEQUENCE: {
            if (d_allocator_p == rhs.d_allocator_p) {
                reset();
                d_sequence = rhs.d_sequence;
                d_selectionId = SELECTION_ID_SEQUENCE;
                rhs.d_selectionId = SELECTION_ID_UNDEFINED;
            }
            else {
                makeSequence(bsl::move(*rhs.d_sequence));
            }
          } break;
          case SELECTION_ID_CHOICE: {
            if (d_allocator_p == rhs.d_allocator_p) {
                reset();
                d_choice = rhs.d_choice;
                d_selectionId = SELECTION_ID_CHOICE;
                rhs.d_selectionId = SELECTION_ID_UNDEFINED;
            }
            else {
                makeChoice(bsl::move(*rhs.d_choice));
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

void BasicSchemaComplexTypeElementChoice::reset()
{
    switch (d_selectionId) {
      case SELECTION_ID_ANNOTATION: {
        d_annotation.object().~BasicSchemaAnnotationElement();
      } break;
      case SELECTION_ID_SEQUENCE: {
        d_allocator_p->deleteObject(d_sequence);
      } break;
      case SELECTION_ID_CHOICE: {
        d_allocator_p->deleteObject(d_choice);
      } break;
      default:
        BSLS_ASSERT(SELECTION_ID_UNDEFINED == d_selectionId);
    }

    d_selectionId = SELECTION_ID_UNDEFINED;
}

int BasicSchemaComplexTypeElementChoice::makeSelection(int selectionId)
{
    switch (selectionId) {
      case SELECTION_ID_ANNOTATION: {
        makeAnnotation();
      } break;
      case SELECTION_ID_SEQUENCE: {
        makeSequence();
      } break;
      case SELECTION_ID_CHOICE: {
        makeChoice();
      } break;
      case SELECTION_ID_UNDEFINED: {
        reset();
      } break;
      default:
        return -1;
    }
    return 0;
}

int BasicSchemaComplexTypeElementChoice::makeSelection(const char *name, int nameLength)
{
    const bdlat_SelectionInfo *selectionInfo =
                                         lookupSelectionInfo(name, nameLength);
    if (0 == selectionInfo) {
       return -1;
    }

    return makeSelection(selectionInfo->d_id);
}

BasicSchemaAnnotationElement& BasicSchemaComplexTypeElementChoice::makeAnnotation()
{
    if (SELECTION_ID_ANNOTATION == d_selectionId) {
        bdlat_ValueTypeFunctions::reset(&d_annotation.object());
    }
    else {
        reset();
        new (d_annotation.buffer())
                BasicSchemaAnnotationElement(d_allocator_p);
        d_selectionId = SELECTION_ID_ANNOTATION;
    }

    return d_annotation.object();
}

BasicSchemaAnnotationElement& BasicSchemaComplexTypeElementChoice::makeAnnotation(const BasicSchemaAnnotationElement& value)
{
    if (SELECTION_ID_ANNOTATION == d_selectionId) {
        d_annotation.object() = value;
    }
    else {
        reset();
        new (d_annotation.buffer())
                BasicSchemaAnnotationElement(value, d_allocator_p);
        d_selectionId = SELECTION_ID_ANNOTATION;
    }

    return d_annotation.object();
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
BasicSchemaAnnotationElement& BasicSchemaComplexTypeElementChoice::makeAnnotation(BasicSchemaAnnotationElement&& value)
{
    if (SELECTION_ID_ANNOTATION == d_selectionId) {
        d_annotation.object() = bsl::move(value);
    }
    else {
        reset();
        new (d_annotation.buffer())
                BasicSchemaAnnotationElement(bsl::move(value), d_allocator_p);
        d_selectionId = SELECTION_ID_ANNOTATION;
    }

    return d_annotation.object();
}
#endif

BasicSchemaSequenceElement& BasicSchemaComplexTypeElementChoice::makeSequence()
{
    if (SELECTION_ID_SEQUENCE == d_selectionId) {
        bdlat_ValueTypeFunctions::reset(d_sequence);
    }
    else {
        reset();
        d_sequence = new (*d_allocator_p)
                BasicSchemaSequenceElement(d_allocator_p);
        d_selectionId = SELECTION_ID_SEQUENCE;
    }

    return *d_sequence;
}

BasicSchemaSequenceElement& BasicSchemaComplexTypeElementChoice::makeSequence(const BasicSchemaSequenceElement& value)
{
    if (SELECTION_ID_SEQUENCE == d_selectionId) {
        *d_sequence = value;
    }
    else {
        reset();
        d_sequence = new (*d_allocator_p)
                BasicSchemaSequenceElement(value, d_allocator_p);
        d_selectionId = SELECTION_ID_SEQUENCE;
    }

    return *d_sequence;
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
BasicSchemaSequenceElement& BasicSchemaComplexTypeElementChoice::makeSequence(BasicSchemaSequenceElement&& value)
{
    if (SELECTION_ID_SEQUENCE == d_selectionId) {
        *d_sequence = bsl::move(value);
    }
    else {
        reset();
        d_sequence = new (*d_allocator_p)
                BasicSchemaSequenceElement(bsl::move(value), d_allocator_p);
        d_selectionId = SELECTION_ID_SEQUENCE;
    }

    return *d_sequence;
}
#endif

BasicSchemaChoiceElement& BasicSchemaComplexTypeElementChoice::makeChoice()
{
    if (SELECTION_ID_CHOICE == d_selectionId) {
        bdlat_ValueTypeFunctions::reset(d_choice);
    }
    else {
        reset();
        d_choice = new (*d_allocator_p)
                BasicSchemaChoiceElement(d_allocator_p);
        d_selectionId = SELECTION_ID_CHOICE;
    }

    return *d_choice;
}

BasicSchemaChoiceElement& BasicSchemaComplexTypeElementChoice::makeChoice(const BasicSchemaChoiceElement& value)
{
    if (SELECTION_ID_CHOICE == d_selectionId) {
        *d_choice = value;
    }
    else {
        reset();
        d_choice = new (*d_allocator_p)
                BasicSchemaChoiceElement(value, d_allocator_p);
        d_selectionId = SELECTION_ID_CHOICE;
    }

    return *d_choice;
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
BasicSchemaChoiceElement& BasicSchemaComplexTypeElementChoice::makeChoice(BasicSchemaChoiceElement&& value)
{
    if (SELECTION_ID_CHOICE == d_selectionId) {
        *d_choice = bsl::move(value);
    }
    else {
        reset();
        d_choice = new (*d_allocator_p)
                BasicSchemaChoiceElement(bsl::move(value), d_allocator_p);
        d_selectionId = SELECTION_ID_CHOICE;
    }

    return *d_choice;
}
#endif

// ACCESSORS

bsl::ostream& BasicSchemaComplexTypeElementChoice::print(
    bsl::ostream& stream,
    int           level,
    int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    switch (d_selectionId) {
      case SELECTION_ID_ANNOTATION: {
        printer.printAttribute("annotation", d_annotation.object());
      }  break;
      case SELECTION_ID_SEQUENCE: {
    printer.printAttribute("sequence", *d_sequence);
      }  break;
      case SELECTION_ID_CHOICE: {
    printer.printAttribute("choice", *d_choice);
      }  break;
      default:
        stream << "SELECTION UNDEFINED\n";
    }
    printer.end();
    return stream;
}


const char *BasicSchemaComplexTypeElementChoice::selectionName() const
{
    switch (d_selectionId) {
      case SELECTION_ID_ANNOTATION:
        return SELECTION_INFO_ARRAY[SELECTION_INDEX_ANNOTATION].name();
      case SELECTION_ID_SEQUENCE:
        return SELECTION_INFO_ARRAY[SELECTION_INDEX_SEQUENCE].name();
      case SELECTION_ID_CHOICE:
        return SELECTION_INFO_ARRAY[SELECTION_INDEX_CHOICE].name();
      default:
        BSLS_ASSERT(SELECTION_ID_UNDEFINED == d_selectionId);
        return "(* UNDEFINED *)";
    }
}

                          // ------------------------
                          // class BasicSchemaElement
                          // ------------------------

// CONSTANTS

const char BasicSchemaElement::CLASS_NAME[] = "BasicSchemaElement";

const bdlat_AttributeInfo BasicSchemaElement::ATTRIBUTE_INFO_ARRAY[] = {
    {
        ATTRIBUTE_ID_NAME,
        "name",
        sizeof("name") - 1,
        "",
        bdlat_FormattingMode::e_TEXT
      | bdlat_FormattingMode::e_ATTRIBUTE
    },
    {
        ATTRIBUTE_ID_TYPE,
        "type",
        sizeof("type") - 1,
        "",
        bdlat_FormattingMode::e_TEXT
      | bdlat_FormattingMode::e_ATTRIBUTE
    },
    {
        ATTRIBUTE_ID_MIN_OCCURS,
        "minOccurs",
        sizeof("minOccurs") - 1,
        "",
        bdlat_FormattingMode::e_TEXT
      | bdlat_FormattingMode::e_ATTRIBUTE
    },
    {
        ATTRIBUTE_ID_MAX_OCCURS,
        "maxOccurs",
        sizeof("maxOccurs") - 1,
        "",
        bdlat_FormattingMode::e_TEXT
      | bdlat_FormattingMode::e_ATTRIBUTE
    },
    {
        ATTRIBUTE_ID_DEFAULT_VALUE,
        "default",
        sizeof("default") - 1,
        "",
        bdlat_FormattingMode::e_TEXT
      | bdlat_FormattingMode::e_ATTRIBUTE
    },
    {
        ATTRIBUTE_ID_CPPDEFAULT,
        "cppdefault",
        sizeof("cppdefault") - 1,
        "",
        bdlat_FormattingMode::e_TEXT
      | bdlat_FormattingMode::e_ATTRIBUTE
    },
    {
        ATTRIBUTE_ID_CPPTYPE,
        "cpptype",
        sizeof("cpptype") - 1,
        "",
        bdlat_FormattingMode::e_TEXT
      | bdlat_FormattingMode::e_ATTRIBUTE
    },
    {
        ATTRIBUTE_ID_ALLOCATES_MEMORY,
        "allocatesMemory",
        sizeof("allocatesMemory") - 1,
        "",
        bdlat_FormattingMode::e_TEXT
      | bdlat_FormattingMode::e_ATTRIBUTE
    },
    {
        ATTRIBUTE_ID_ALLOWS_DIRECT_MANIPULATION,
        "allowsDirectManipulation",
        sizeof("allowsDirectManipulation") - 1,
        "",
        bdlat_FormattingMode::e_TEXT
      | bdlat_FormattingMode::e_ATTRIBUTE
    },
    {
        ATTRIBUTE_ID_CHOICE,
        "Choice",
        sizeof("Choice") - 1,
        "",
        bdlat_FormattingMode::e_DEFAULT
      | bdlat_FormattingMode::e_UNTAGGED
    }
};

// CLASS METHODS

const bdlat_AttributeInfo *BasicSchemaElement::lookupAttributeInfo(
        const char *name,
        int         nameLength)
{
    if (bdlb::String::areEqualCaseless("annotation", name, nameLength)) {
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CHOICE];
    }

    if (bdlb::String::areEqualCaseless("simpleType", name, nameLength)) {
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CHOICE];
    }

    if (bdlb::String::areEqualCaseless("complexType", name, nameLength)) {
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CHOICE];
    }

    for (int i = 0; i < 10; ++i) {
        const bdlat_AttributeInfo& attributeInfo =
                    BasicSchemaElement::ATTRIBUTE_INFO_ARRAY[i];

        if (nameLength == attributeInfo.d_nameLength
        &&  0 == bsl::memcmp(attributeInfo.d_name_p, name, nameLength))
        {
            return &attributeInfo;
        }
    }

    return 0;
}

const bdlat_AttributeInfo *BasicSchemaElement::lookupAttributeInfo(int id)
{
    switch (id) {
      case ATTRIBUTE_ID_NAME:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_NAME];
      case ATTRIBUTE_ID_TYPE:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_TYPE];
      case ATTRIBUTE_ID_MIN_OCCURS:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_MIN_OCCURS];
      case ATTRIBUTE_ID_MAX_OCCURS:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_MAX_OCCURS];
      case ATTRIBUTE_ID_DEFAULT_VALUE:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_DEFAULT_VALUE];
      case ATTRIBUTE_ID_CPPDEFAULT:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CPPDEFAULT];
      case ATTRIBUTE_ID_CPPTYPE:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CPPTYPE];
      case ATTRIBUTE_ID_ALLOCATES_MEMORY:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ALLOCATES_MEMORY];
      case ATTRIBUTE_ID_ALLOWS_DIRECT_MANIPULATION:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ALLOWS_DIRECT_MANIPULATION];
      case ATTRIBUTE_ID_CHOICE:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CHOICE];
      default:
        return 0;
    }
}

// CREATORS

BasicSchemaElement::BasicSchemaElement(bslma::Allocator *basicAllocator)
: d_choice(basicAllocator)
, d_name(basicAllocator)
, d_type(basicAllocator)
, d_minOccurs(basicAllocator)
, d_maxOccurs(basicAllocator)
, d_default(basicAllocator)
, d_cppdefault(basicAllocator)
, d_cpptype(basicAllocator)
, d_allocatesMemory(basicAllocator)
, d_allowsDirectManipulation(basicAllocator)
{
}

BasicSchemaElement::BasicSchemaElement(const BasicSchemaElement& original,
                                       bslma::Allocator *basicAllocator)
: d_choice(original.d_choice, basicAllocator)
, d_name(original.d_name, basicAllocator)
, d_type(original.d_type, basicAllocator)
, d_minOccurs(original.d_minOccurs, basicAllocator)
, d_maxOccurs(original.d_maxOccurs, basicAllocator)
, d_default(original.d_default, basicAllocator)
, d_cppdefault(original.d_cppdefault, basicAllocator)
, d_cpptype(original.d_cpptype, basicAllocator)
, d_allocatesMemory(original.d_allocatesMemory, basicAllocator)
, d_allowsDirectManipulation(original.d_allowsDirectManipulation, basicAllocator)
{
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
BasicSchemaElement::BasicSchemaElement(BasicSchemaElement&& original) noexcept
: d_choice(bsl::move(original.d_choice))
, d_name(bsl::move(original.d_name))
, d_type(bsl::move(original.d_type))
, d_minOccurs(bsl::move(original.d_minOccurs))
, d_maxOccurs(bsl::move(original.d_maxOccurs))
, d_default(bsl::move(original.d_default))
, d_cppdefault(bsl::move(original.d_cppdefault))
, d_cpptype(bsl::move(original.d_cpptype))
, d_allocatesMemory(bsl::move(original.d_allocatesMemory))
, d_allowsDirectManipulation(bsl::move(original.d_allowsDirectManipulation))
{
}

BasicSchemaElement::BasicSchemaElement(BasicSchemaElement&& original,
                                       bslma::Allocator *basicAllocator)
: d_choice(bsl::move(original.d_choice), basicAllocator)
, d_name(bsl::move(original.d_name), basicAllocator)
, d_type(bsl::move(original.d_type), basicAllocator)
, d_minOccurs(bsl::move(original.d_minOccurs), basicAllocator)
, d_maxOccurs(bsl::move(original.d_maxOccurs), basicAllocator)
, d_default(bsl::move(original.d_default), basicAllocator)
, d_cppdefault(bsl::move(original.d_cppdefault), basicAllocator)
, d_cpptype(bsl::move(original.d_cpptype), basicAllocator)
, d_allocatesMemory(bsl::move(original.d_allocatesMemory), basicAllocator)
, d_allowsDirectManipulation(bsl::move(original.d_allowsDirectManipulation), basicAllocator)
{
}
#endif

BasicSchemaElement::~BasicSchemaElement()
{
}

// MANIPULATORS

BasicSchemaElement&
BasicSchemaElement::operator=(const BasicSchemaElement& rhs)
{
    if (this != &rhs) {
        d_name = rhs.d_name;
        d_type = rhs.d_type;
        d_minOccurs = rhs.d_minOccurs;
        d_maxOccurs = rhs.d_maxOccurs;
        d_default = rhs.d_default;
        d_cppdefault = rhs.d_cppdefault;
        d_cpptype = rhs.d_cpptype;
        d_allocatesMemory = rhs.d_allocatesMemory;
        d_allowsDirectManipulation = rhs.d_allowsDirectManipulation;
        d_choice = rhs.d_choice;
    }

    return *this;
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
BasicSchemaElement&
BasicSchemaElement::operator=(BasicSchemaElement&& rhs)
{
    if (this != &rhs) {
        d_name = bsl::move(rhs.d_name);
        d_type = bsl::move(rhs.d_type);
        d_minOccurs = bsl::move(rhs.d_minOccurs);
        d_maxOccurs = bsl::move(rhs.d_maxOccurs);
        d_default = bsl::move(rhs.d_default);
        d_cppdefault = bsl::move(rhs.d_cppdefault);
        d_cpptype = bsl::move(rhs.d_cpptype);
        d_allocatesMemory = bsl::move(rhs.d_allocatesMemory);
        d_allowsDirectManipulation = bsl::move(rhs.d_allowsDirectManipulation);
        d_choice = bsl::move(rhs.d_choice);
    }

    return *this;
}
#endif

void BasicSchemaElement::reset()
{
    bdlat_ValueTypeFunctions::reset(&d_name);
    bdlat_ValueTypeFunctions::reset(&d_type);
    bdlat_ValueTypeFunctions::reset(&d_minOccurs);
    bdlat_ValueTypeFunctions::reset(&d_maxOccurs);
    bdlat_ValueTypeFunctions::reset(&d_default);
    bdlat_ValueTypeFunctions::reset(&d_cppdefault);
    bdlat_ValueTypeFunctions::reset(&d_cpptype);
    bdlat_ValueTypeFunctions::reset(&d_allocatesMemory);
    bdlat_ValueTypeFunctions::reset(&d_allowsDirectManipulation);
    bdlat_ValueTypeFunctions::reset(&d_choice);
}

// ACCESSORS

bsl::ostream& BasicSchemaElement::print(
        bsl::ostream& stream,
        int           level,
        int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute("name", d_name);
    printer.printAttribute("type", d_type);
    printer.printAttribute("minOccurs", d_minOccurs);
    printer.printAttribute("maxOccurs", d_maxOccurs);
    printer.printAttribute("defaultValue", d_default);
    printer.printAttribute("cppdefault", d_cppdefault);
    printer.printAttribute("cpptype", d_cpptype);
    printer.printAttribute("allocatesMemory", d_allocatesMemory);
    printer.printAttribute("allowsDirectManipulation", d_allowsDirectManipulation);
    printer.printAttribute("choice", d_choice);
    printer.end();
    return stream;
}



                       // ------------------------------
                       // class BasicSchemaElementChoice
                       // ------------------------------

// CONSTANTS

const char BasicSchemaElementChoice::CLASS_NAME[] = "BasicSchemaElementChoice";

const bdlat_SelectionInfo BasicSchemaElementChoice::SELECTION_INFO_ARRAY[] = {
    {
        SELECTION_ID_ANNOTATION,
        "annotation",
        sizeof("annotation") - 1,
        "",
        bdlat_FormattingMode::e_DEFAULT
    },
    {
        SELECTION_ID_SIMPLE_TYPE,
        "simpleType",
        sizeof("simpleType") - 1,
        "",
        bdlat_FormattingMode::e_DEFAULT
    },
    {
        SELECTION_ID_COMPLEX_TYPE,
        "complexType",
        sizeof("complexType") - 1,
        "",
        bdlat_FormattingMode::e_DEFAULT
    }
};

// CLASS METHODS

const bdlat_SelectionInfo *BasicSchemaElementChoice::lookupSelectionInfo(
        const char *name,
        int         nameLength)
{
    for (int i = 0; i < 3; ++i) {
        const bdlat_SelectionInfo& selectionInfo =
                    BasicSchemaElementChoice::SELECTION_INFO_ARRAY[i];

        if (nameLength == selectionInfo.d_nameLength
        &&  0 == bsl::memcmp(selectionInfo.d_name_p, name, nameLength))
        {
            return &selectionInfo;
        }
    }

    return 0;
}

const bdlat_SelectionInfo *BasicSchemaElementChoice::lookupSelectionInfo(int id)
{
    switch (id) {
      case SELECTION_ID_ANNOTATION:
        return &SELECTION_INFO_ARRAY[SELECTION_INDEX_ANNOTATION];
      case SELECTION_ID_SIMPLE_TYPE:
        return &SELECTION_INFO_ARRAY[SELECTION_INDEX_SIMPLE_TYPE];
      case SELECTION_ID_COMPLEX_TYPE:
        return &SELECTION_INFO_ARRAY[SELECTION_INDEX_COMPLEX_TYPE];
      default:
        return 0;
    }
}

// CREATORS

BasicSchemaElementChoice::BasicSchemaElementChoice(
    const BasicSchemaElementChoice& original,
    bslma::Allocator *basicAllocator)
: d_selectionId(original.d_selectionId)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    switch (d_selectionId) {
      case SELECTION_ID_ANNOTATION: {
        new (d_annotation.buffer())
            BasicSchemaAnnotationElement(
                original.d_annotation.object(), d_allocator_p);
      } break;
      case SELECTION_ID_SIMPLE_TYPE: {
        new (d_simpleType.buffer())
            BasicSchemaSimpleTypeElement(
                original.d_simpleType.object(), d_allocator_p);
      } break;
      case SELECTION_ID_COMPLEX_TYPE: {
        d_complexType = new (*d_allocator_p)
                BasicSchemaComplexTypeElement(*original.d_complexType, d_allocator_p);
      } break;
      default:
        BSLS_ASSERT(SELECTION_ID_UNDEFINED == d_selectionId);
    }
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
BasicSchemaElementChoice::BasicSchemaElementChoice(BasicSchemaElementChoice&& original) noexcept
: d_selectionId(original.d_selectionId)
, d_allocator_p(original.d_allocator_p)
{
    switch (d_selectionId) {
      case SELECTION_ID_ANNOTATION: {
        new (d_annotation.buffer())
            BasicSchemaAnnotationElement(
                bsl::move(original.d_annotation.object()), d_allocator_p);
      } break;
      case SELECTION_ID_SIMPLE_TYPE: {
        new (d_simpleType.buffer())
            BasicSchemaSimpleTypeElement(
                bsl::move(original.d_simpleType.object()), d_allocator_p);
      } break;
      case SELECTION_ID_COMPLEX_TYPE: {
        d_complexType = original.d_complexType;
        original.d_selectionId = SELECTION_ID_UNDEFINED;
      } break;
      default:
        BSLS_ASSERT(SELECTION_ID_UNDEFINED == d_selectionId);
    }
}

BasicSchemaElementChoice::BasicSchemaElementChoice(
    BasicSchemaElementChoice&& original,
    bslma::Allocator *basicAllocator)
: d_selectionId(original.d_selectionId)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    switch (d_selectionId) {
      case SELECTION_ID_ANNOTATION: {
        new (d_annotation.buffer())
            BasicSchemaAnnotationElement(
                bsl::move(original.d_annotation.object()), d_allocator_p);
      } break;
      case SELECTION_ID_SIMPLE_TYPE: {
        new (d_simpleType.buffer())
            BasicSchemaSimpleTypeElement(
                bsl::move(original.d_simpleType.object()), d_allocator_p);
      } break;
      case SELECTION_ID_COMPLEX_TYPE: {
        if (d_allocator_p == original.d_allocator_p) {
            d_complexType = original.d_complexType;
            original.d_selectionId = SELECTION_ID_UNDEFINED;
        }
        else {
            d_complexType = new (*d_allocator_p)
                    BasicSchemaComplexTypeElement(bsl::move(*original.d_complexType), d_allocator_p);
        }
      } break;
      default:
        BSLS_ASSERT(SELECTION_ID_UNDEFINED == d_selectionId);
    }
}
#endif

// MANIPULATORS

BasicSchemaElementChoice&
BasicSchemaElementChoice::operator=(const BasicSchemaElementChoice& rhs)
{
    if (this != &rhs) {
        switch (rhs.d_selectionId) {
          case SELECTION_ID_ANNOTATION: {
            makeAnnotation(rhs.d_annotation.object());
          } break;
          case SELECTION_ID_SIMPLE_TYPE: {
            makeSimpleType(rhs.d_simpleType.object());
          } break;
          case SELECTION_ID_COMPLEX_TYPE: {
            makeComplexType(*rhs.d_complexType);
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
BasicSchemaElementChoice&
BasicSchemaElementChoice::operator=(BasicSchemaElementChoice&& rhs)
{
    if (this != &rhs) {
        switch (rhs.d_selectionId) {
          case SELECTION_ID_ANNOTATION: {
            makeAnnotation(bsl::move(rhs.d_annotation.object()));
          } break;
          case SELECTION_ID_SIMPLE_TYPE: {
            makeSimpleType(bsl::move(rhs.d_simpleType.object()));
          } break;
          case SELECTION_ID_COMPLEX_TYPE: {
            if (d_allocator_p == rhs.d_allocator_p) {
                reset();
                d_complexType = rhs.d_complexType;
                d_selectionId = SELECTION_ID_COMPLEX_TYPE;
                rhs.d_selectionId = SELECTION_ID_UNDEFINED;
            }
            else {
                makeComplexType(bsl::move(*rhs.d_complexType));
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

void BasicSchemaElementChoice::reset()
{
    switch (d_selectionId) {
      case SELECTION_ID_ANNOTATION: {
        d_annotation.object().~BasicSchemaAnnotationElement();
      } break;
      case SELECTION_ID_SIMPLE_TYPE: {
        d_simpleType.object().~BasicSchemaSimpleTypeElement();
      } break;
      case SELECTION_ID_COMPLEX_TYPE: {
        d_allocator_p->deleteObject(d_complexType);
      } break;
      default:
        BSLS_ASSERT(SELECTION_ID_UNDEFINED == d_selectionId);
    }

    d_selectionId = SELECTION_ID_UNDEFINED;
}

int BasicSchemaElementChoice::makeSelection(int selectionId)
{
    switch (selectionId) {
      case SELECTION_ID_ANNOTATION: {
        makeAnnotation();
      } break;
      case SELECTION_ID_SIMPLE_TYPE: {
        makeSimpleType();
      } break;
      case SELECTION_ID_COMPLEX_TYPE: {
        makeComplexType();
      } break;
      case SELECTION_ID_UNDEFINED: {
        reset();
      } break;
      default:
        return -1;
    }
    return 0;
}

int BasicSchemaElementChoice::makeSelection(const char *name, int nameLength)
{
    const bdlat_SelectionInfo *selectionInfo =
                                         lookupSelectionInfo(name, nameLength);
    if (0 == selectionInfo) {
       return -1;
    }

    return makeSelection(selectionInfo->d_id);
}

BasicSchemaAnnotationElement& BasicSchemaElementChoice::makeAnnotation()
{
    if (SELECTION_ID_ANNOTATION == d_selectionId) {
        bdlat_ValueTypeFunctions::reset(&d_annotation.object());
    }
    else {
        reset();
        new (d_annotation.buffer())
                BasicSchemaAnnotationElement(d_allocator_p);
        d_selectionId = SELECTION_ID_ANNOTATION;
    }

    return d_annotation.object();
}

BasicSchemaAnnotationElement& BasicSchemaElementChoice::makeAnnotation(const BasicSchemaAnnotationElement& value)
{
    if (SELECTION_ID_ANNOTATION == d_selectionId) {
        d_annotation.object() = value;
    }
    else {
        reset();
        new (d_annotation.buffer())
                BasicSchemaAnnotationElement(value, d_allocator_p);
        d_selectionId = SELECTION_ID_ANNOTATION;
    }

    return d_annotation.object();
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
BasicSchemaAnnotationElement& BasicSchemaElementChoice::makeAnnotation(BasicSchemaAnnotationElement&& value)
{
    if (SELECTION_ID_ANNOTATION == d_selectionId) {
        d_annotation.object() = bsl::move(value);
    }
    else {
        reset();
        new (d_annotation.buffer())
                BasicSchemaAnnotationElement(bsl::move(value), d_allocator_p);
        d_selectionId = SELECTION_ID_ANNOTATION;
    }

    return d_annotation.object();
}
#endif

BasicSchemaSimpleTypeElement& BasicSchemaElementChoice::makeSimpleType()
{
    if (SELECTION_ID_SIMPLE_TYPE == d_selectionId) {
        bdlat_ValueTypeFunctions::reset(&d_simpleType.object());
    }
    else {
        reset();
        new (d_simpleType.buffer())
                BasicSchemaSimpleTypeElement(d_allocator_p);
        d_selectionId = SELECTION_ID_SIMPLE_TYPE;
    }

    return d_simpleType.object();
}

BasicSchemaSimpleTypeElement& BasicSchemaElementChoice::makeSimpleType(const BasicSchemaSimpleTypeElement& value)
{
    if (SELECTION_ID_SIMPLE_TYPE == d_selectionId) {
        d_simpleType.object() = value;
    }
    else {
        reset();
        new (d_simpleType.buffer())
                BasicSchemaSimpleTypeElement(value, d_allocator_p);
        d_selectionId = SELECTION_ID_SIMPLE_TYPE;
    }

    return d_simpleType.object();
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
BasicSchemaSimpleTypeElement& BasicSchemaElementChoice::makeSimpleType(BasicSchemaSimpleTypeElement&& value)
{
    if (SELECTION_ID_SIMPLE_TYPE == d_selectionId) {
        d_simpleType.object() = bsl::move(value);
    }
    else {
        reset();
        new (d_simpleType.buffer())
                BasicSchemaSimpleTypeElement(bsl::move(value), d_allocator_p);
        d_selectionId = SELECTION_ID_SIMPLE_TYPE;
    }

    return d_simpleType.object();
}
#endif

BasicSchemaComplexTypeElement& BasicSchemaElementChoice::makeComplexType()
{
    if (SELECTION_ID_COMPLEX_TYPE == d_selectionId) {
        bdlat_ValueTypeFunctions::reset(d_complexType);
    }
    else {
        reset();
        d_complexType = new (*d_allocator_p)
                BasicSchemaComplexTypeElement(d_allocator_p);
        d_selectionId = SELECTION_ID_COMPLEX_TYPE;
    }

    return *d_complexType;
}

BasicSchemaComplexTypeElement& BasicSchemaElementChoice::makeComplexType(const BasicSchemaComplexTypeElement& value)
{
    if (SELECTION_ID_COMPLEX_TYPE == d_selectionId) {
        *d_complexType = value;
    }
    else {
        reset();
        d_complexType = new (*d_allocator_p)
                BasicSchemaComplexTypeElement(value, d_allocator_p);
        d_selectionId = SELECTION_ID_COMPLEX_TYPE;
    }

    return *d_complexType;
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
BasicSchemaComplexTypeElement& BasicSchemaElementChoice::makeComplexType(BasicSchemaComplexTypeElement&& value)
{
    if (SELECTION_ID_COMPLEX_TYPE == d_selectionId) {
        *d_complexType = bsl::move(value);
    }
    else {
        reset();
        d_complexType = new (*d_allocator_p)
                BasicSchemaComplexTypeElement(bsl::move(value), d_allocator_p);
        d_selectionId = SELECTION_ID_COMPLEX_TYPE;
    }

    return *d_complexType;
}
#endif

// ACCESSORS

bsl::ostream& BasicSchemaElementChoice::print(
    bsl::ostream& stream,
    int           level,
    int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    switch (d_selectionId) {
      case SELECTION_ID_ANNOTATION: {
        printer.printAttribute("annotation", d_annotation.object());
      }  break;
      case SELECTION_ID_SIMPLE_TYPE: {
        printer.printAttribute("simpleType", d_simpleType.object());
      }  break;
      case SELECTION_ID_COMPLEX_TYPE: {
    printer.printAttribute("complexType", *d_complexType);
      }  break;
      default:
        stream << "SELECTION UNDEFINED\n";
    }
    printer.end();
    return stream;
}


const char *BasicSchemaElementChoice::selectionName() const
{
    switch (d_selectionId) {
      case SELECTION_ID_ANNOTATION:
        return SELECTION_INFO_ARRAY[SELECTION_INDEX_ANNOTATION].name();
      case SELECTION_ID_SIMPLE_TYPE:
        return SELECTION_INFO_ARRAY[SELECTION_INDEX_SIMPLE_TYPE].name();
      case SELECTION_ID_COMPLEX_TYPE:
        return SELECTION_INFO_ARRAY[SELECTION_INDEX_COMPLEX_TYPE].name();
      default:
        BSLS_ASSERT(SELECTION_ID_UNDEFINED == d_selectionId);
        return "(* UNDEFINED *)";
    }
}

                      // --------------------------------
                      // class BasicSchemaSequenceElement
                      // --------------------------------

// CONSTANTS

const char BasicSchemaSequenceElement::CLASS_NAME[] = "BasicSchemaSequenceElement";

const bdlat_AttributeInfo BasicSchemaSequenceElement::ATTRIBUTE_INFO_ARRAY[] = {
    {
        ATTRIBUTE_ID_CHOICE,
        "Choice",
        sizeof("Choice") - 1,
        "",
        bdlat_FormattingMode::e_DEFAULT
      | bdlat_FormattingMode::e_UNTAGGED
    }
};

// CLASS METHODS

const bdlat_AttributeInfo *BasicSchemaSequenceElement::lookupAttributeInfo(
        const char *name,
        int         nameLength)
{
    if (bdlb::String::areEqualCaseless("annotation", name, nameLength)) {
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CHOICE];
    }

    if (bdlb::String::areEqualCaseless("element", name, nameLength)) {
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CHOICE];
    }

    for (int i = 0; i < 1; ++i) {
        const bdlat_AttributeInfo& attributeInfo =
                    BasicSchemaSequenceElement::ATTRIBUTE_INFO_ARRAY[i];

        if (nameLength == attributeInfo.d_nameLength
        &&  0 == bsl::memcmp(attributeInfo.d_name_p, name, nameLength))
        {
            return &attributeInfo;
        }
    }

    return 0;
}

const bdlat_AttributeInfo *BasicSchemaSequenceElement::lookupAttributeInfo(int id)
{
    switch (id) {
      case ATTRIBUTE_ID_CHOICE:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CHOICE];
      default:
        return 0;
    }
}

// CREATORS

BasicSchemaSequenceElement::BasicSchemaSequenceElement(bslma::Allocator *basicAllocator)
: d_choice(basicAllocator)
{
}

BasicSchemaSequenceElement::BasicSchemaSequenceElement(const BasicSchemaSequenceElement& original,
                                                       bslma::Allocator *basicAllocator)
: d_choice(original.d_choice, basicAllocator)
{
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
BasicSchemaSequenceElement::BasicSchemaSequenceElement(BasicSchemaSequenceElement&& original) noexcept
: d_choice(bsl::move(original.d_choice))
{
}

BasicSchemaSequenceElement::BasicSchemaSequenceElement(BasicSchemaSequenceElement&& original,
                                                       bslma::Allocator *basicAllocator)
: d_choice(bsl::move(original.d_choice), basicAllocator)
{
}
#endif

BasicSchemaSequenceElement::~BasicSchemaSequenceElement()
{
}

// MANIPULATORS

BasicSchemaSequenceElement&
BasicSchemaSequenceElement::operator=(const BasicSchemaSequenceElement& rhs)
{
    if (this != &rhs) {
        d_choice = rhs.d_choice;
    }

    return *this;
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
BasicSchemaSequenceElement&
BasicSchemaSequenceElement::operator=(BasicSchemaSequenceElement&& rhs)
{
    if (this != &rhs) {
        d_choice = bsl::move(rhs.d_choice);
    }

    return *this;
}
#endif

void BasicSchemaSequenceElement::reset()
{
    bdlat_ValueTypeFunctions::reset(&d_choice);
}

// ACCESSORS

bsl::ostream& BasicSchemaSequenceElement::print(
        bsl::ostream& stream,
        int           level,
        int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute("choice", d_choice);
    printer.end();
    return stream;
}



                   // --------------------------------------
                   // class BasicSchemaSequenceElementChoice
                   // --------------------------------------

// CONSTANTS

const char BasicSchemaSequenceElementChoice::CLASS_NAME[] = "BasicSchemaSequenceElementChoice";

const bdlat_SelectionInfo BasicSchemaSequenceElementChoice::SELECTION_INFO_ARRAY[] = {
    {
        SELECTION_ID_ANNOTATION,
        "annotation",
        sizeof("annotation") - 1,
        "",
        bdlat_FormattingMode::e_DEFAULT
    },
    {
        SELECTION_ID_ELEMENT,
        "element",
        sizeof("element") - 1,
        "",
        bdlat_FormattingMode::e_DEFAULT
    }
};

// CLASS METHODS

const bdlat_SelectionInfo *BasicSchemaSequenceElementChoice::lookupSelectionInfo(
        const char *name,
        int         nameLength)
{
    for (int i = 0; i < 2; ++i) {
        const bdlat_SelectionInfo& selectionInfo =
                    BasicSchemaSequenceElementChoice::SELECTION_INFO_ARRAY[i];

        if (nameLength == selectionInfo.d_nameLength
        &&  0 == bsl::memcmp(selectionInfo.d_name_p, name, nameLength))
        {
            return &selectionInfo;
        }
    }

    return 0;
}

const bdlat_SelectionInfo *BasicSchemaSequenceElementChoice::lookupSelectionInfo(int id)
{
    switch (id) {
      case SELECTION_ID_ANNOTATION:
        return &SELECTION_INFO_ARRAY[SELECTION_INDEX_ANNOTATION];
      case SELECTION_ID_ELEMENT:
        return &SELECTION_INFO_ARRAY[SELECTION_INDEX_ELEMENT];
      default:
        return 0;
    }
}

// CREATORS

BasicSchemaSequenceElementChoice::BasicSchemaSequenceElementChoice(
    const BasicSchemaSequenceElementChoice& original,
    bslma::Allocator *basicAllocator)
: d_selectionId(original.d_selectionId)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    switch (d_selectionId) {
      case SELECTION_ID_ANNOTATION: {
        new (d_annotation.buffer())
            BasicSchemaAnnotationElement(
                original.d_annotation.object(), d_allocator_p);
      } break;
      case SELECTION_ID_ELEMENT: {
        d_element = new (*d_allocator_p)
                BasicSchemaElement(*original.d_element, d_allocator_p);
      } break;
      default:
        BSLS_ASSERT(SELECTION_ID_UNDEFINED == d_selectionId);
    }
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
BasicSchemaSequenceElementChoice::BasicSchemaSequenceElementChoice(BasicSchemaSequenceElementChoice&& original) noexcept
: d_selectionId(original.d_selectionId)
, d_allocator_p(original.d_allocator_p)
{
    switch (d_selectionId) {
      case SELECTION_ID_ANNOTATION: {
        new (d_annotation.buffer())
            BasicSchemaAnnotationElement(
                bsl::move(original.d_annotation.object()), d_allocator_p);
      } break;
      case SELECTION_ID_ELEMENT: {
        d_element = original.d_element;
        original.d_selectionId = SELECTION_ID_UNDEFINED;
      } break;
      default:
        BSLS_ASSERT(SELECTION_ID_UNDEFINED == d_selectionId);
    }
}

BasicSchemaSequenceElementChoice::BasicSchemaSequenceElementChoice(
    BasicSchemaSequenceElementChoice&& original,
    bslma::Allocator *basicAllocator)
: d_selectionId(original.d_selectionId)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    switch (d_selectionId) {
      case SELECTION_ID_ANNOTATION: {
        new (d_annotation.buffer())
            BasicSchemaAnnotationElement(
                bsl::move(original.d_annotation.object()), d_allocator_p);
      } break;
      case SELECTION_ID_ELEMENT: {
        if (d_allocator_p == original.d_allocator_p) {
            d_element = original.d_element;
            original.d_selectionId = SELECTION_ID_UNDEFINED;
        }
        else {
            d_element = new (*d_allocator_p)
                    BasicSchemaElement(bsl::move(*original.d_element), d_allocator_p);
        }
      } break;
      default:
        BSLS_ASSERT(SELECTION_ID_UNDEFINED == d_selectionId);
    }
}
#endif

// MANIPULATORS

BasicSchemaSequenceElementChoice&
BasicSchemaSequenceElementChoice::operator=(const BasicSchemaSequenceElementChoice& rhs)
{
    if (this != &rhs) {
        switch (rhs.d_selectionId) {
          case SELECTION_ID_ANNOTATION: {
            makeAnnotation(rhs.d_annotation.object());
          } break;
          case SELECTION_ID_ELEMENT: {
            makeElement(*rhs.d_element);
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
BasicSchemaSequenceElementChoice&
BasicSchemaSequenceElementChoice::operator=(BasicSchemaSequenceElementChoice&& rhs)
{
    if (this != &rhs) {
        switch (rhs.d_selectionId) {
          case SELECTION_ID_ANNOTATION: {
            makeAnnotation(bsl::move(rhs.d_annotation.object()));
          } break;
          case SELECTION_ID_ELEMENT: {
            if (d_allocator_p == rhs.d_allocator_p) {
                reset();
                d_element = rhs.d_element;
                d_selectionId = SELECTION_ID_ELEMENT;
                rhs.d_selectionId = SELECTION_ID_UNDEFINED;
            }
            else {
                makeElement(bsl::move(*rhs.d_element));
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

void BasicSchemaSequenceElementChoice::reset()
{
    switch (d_selectionId) {
      case SELECTION_ID_ANNOTATION: {
        d_annotation.object().~BasicSchemaAnnotationElement();
      } break;
      case SELECTION_ID_ELEMENT: {
        d_allocator_p->deleteObject(d_element);
      } break;
      default:
        BSLS_ASSERT(SELECTION_ID_UNDEFINED == d_selectionId);
    }

    d_selectionId = SELECTION_ID_UNDEFINED;
}

int BasicSchemaSequenceElementChoice::makeSelection(int selectionId)
{
    switch (selectionId) {
      case SELECTION_ID_ANNOTATION: {
        makeAnnotation();
      } break;
      case SELECTION_ID_ELEMENT: {
        makeElement();
      } break;
      case SELECTION_ID_UNDEFINED: {
        reset();
      } break;
      default:
        return -1;
    }
    return 0;
}

int BasicSchemaSequenceElementChoice::makeSelection(const char *name, int nameLength)
{
    const bdlat_SelectionInfo *selectionInfo =
                                         lookupSelectionInfo(name, nameLength);
    if (0 == selectionInfo) {
       return -1;
    }

    return makeSelection(selectionInfo->d_id);
}

BasicSchemaAnnotationElement& BasicSchemaSequenceElementChoice::makeAnnotation()
{
    if (SELECTION_ID_ANNOTATION == d_selectionId) {
        bdlat_ValueTypeFunctions::reset(&d_annotation.object());
    }
    else {
        reset();
        new (d_annotation.buffer())
                BasicSchemaAnnotationElement(d_allocator_p);
        d_selectionId = SELECTION_ID_ANNOTATION;
    }

    return d_annotation.object();
}

BasicSchemaAnnotationElement& BasicSchemaSequenceElementChoice::makeAnnotation(const BasicSchemaAnnotationElement& value)
{
    if (SELECTION_ID_ANNOTATION == d_selectionId) {
        d_annotation.object() = value;
    }
    else {
        reset();
        new (d_annotation.buffer())
                BasicSchemaAnnotationElement(value, d_allocator_p);
        d_selectionId = SELECTION_ID_ANNOTATION;
    }

    return d_annotation.object();
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
BasicSchemaAnnotationElement& BasicSchemaSequenceElementChoice::makeAnnotation(BasicSchemaAnnotationElement&& value)
{
    if (SELECTION_ID_ANNOTATION == d_selectionId) {
        d_annotation.object() = bsl::move(value);
    }
    else {
        reset();
        new (d_annotation.buffer())
                BasicSchemaAnnotationElement(bsl::move(value), d_allocator_p);
        d_selectionId = SELECTION_ID_ANNOTATION;
    }

    return d_annotation.object();
}
#endif

BasicSchemaElement& BasicSchemaSequenceElementChoice::makeElement()
{
    if (SELECTION_ID_ELEMENT == d_selectionId) {
        bdlat_ValueTypeFunctions::reset(d_element);
    }
    else {
        reset();
        d_element = new (*d_allocator_p)
                BasicSchemaElement(d_allocator_p);
        d_selectionId = SELECTION_ID_ELEMENT;
    }

    return *d_element;
}

BasicSchemaElement& BasicSchemaSequenceElementChoice::makeElement(const BasicSchemaElement& value)
{
    if (SELECTION_ID_ELEMENT == d_selectionId) {
        *d_element = value;
    }
    else {
        reset();
        d_element = new (*d_allocator_p)
                BasicSchemaElement(value, d_allocator_p);
        d_selectionId = SELECTION_ID_ELEMENT;
    }

    return *d_element;
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
BasicSchemaElement& BasicSchemaSequenceElementChoice::makeElement(BasicSchemaElement&& value)
{
    if (SELECTION_ID_ELEMENT == d_selectionId) {
        *d_element = bsl::move(value);
    }
    else {
        reset();
        d_element = new (*d_allocator_p)
                BasicSchemaElement(bsl::move(value), d_allocator_p);
        d_selectionId = SELECTION_ID_ELEMENT;
    }

    return *d_element;
}
#endif

// ACCESSORS

bsl::ostream& BasicSchemaSequenceElementChoice::print(
    bsl::ostream& stream,
    int           level,
    int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    switch (d_selectionId) {
      case SELECTION_ID_ANNOTATION: {
        printer.printAttribute("annotation", d_annotation.object());
      }  break;
      case SELECTION_ID_ELEMENT: {
    printer.printAttribute("element", *d_element);
      }  break;
      default:
        stream << "SELECTION UNDEFINED\n";
    }
    printer.end();
    return stream;
}


const char *BasicSchemaSequenceElementChoice::selectionName() const
{
    switch (d_selectionId) {
      case SELECTION_ID_ANNOTATION:
        return SELECTION_INFO_ARRAY[SELECTION_INDEX_ANNOTATION].name();
      case SELECTION_ID_ELEMENT:
        return SELECTION_INFO_ARRAY[SELECTION_INDEX_ELEMENT].name();
      default:
        BSLS_ASSERT(SELECTION_ID_UNDEFINED == d_selectionId);
        return "(* UNDEFINED *)";
    }
}

                          // -----------------------
                          // class BasicSchemaChoice
                          // -----------------------

// CONSTANTS

const char BasicSchemaChoice::CLASS_NAME[] = "BasicSchemaChoice";

const bdlat_SelectionInfo BasicSchemaChoice::SELECTION_INFO_ARRAY[] = {
    {
        SELECTION_ID_ANNOTATION,
        "annotation",
        sizeof("annotation") - 1,
        "",
        bdlat_FormattingMode::e_DEFAULT
    },
    {
        SELECTION_ID_SIMPLE_TYPE,
        "simpleType",
        sizeof("simpleType") - 1,
        "",
        bdlat_FormattingMode::e_DEFAULT
    },
    {
        SELECTION_ID_COMPLEX_TYPE,
        "complexType",
        sizeof("complexType") - 1,
        "",
        bdlat_FormattingMode::e_DEFAULT
    }
};

// CLASS METHODS

const bdlat_SelectionInfo *BasicSchemaChoice::lookupSelectionInfo(
        const char *name,
        int         nameLength)
{
    for (int i = 0; i < 3; ++i) {
        const bdlat_SelectionInfo& selectionInfo =
                    BasicSchemaChoice::SELECTION_INFO_ARRAY[i];

        if (nameLength == selectionInfo.d_nameLength
        &&  0 == bsl::memcmp(selectionInfo.d_name_p, name, nameLength))
        {
            return &selectionInfo;
        }
    }

    return 0;
}

const bdlat_SelectionInfo *BasicSchemaChoice::lookupSelectionInfo(int id)
{
    switch (id) {
      case SELECTION_ID_ANNOTATION:
        return &SELECTION_INFO_ARRAY[SELECTION_INDEX_ANNOTATION];
      case SELECTION_ID_SIMPLE_TYPE:
        return &SELECTION_INFO_ARRAY[SELECTION_INDEX_SIMPLE_TYPE];
      case SELECTION_ID_COMPLEX_TYPE:
        return &SELECTION_INFO_ARRAY[SELECTION_INDEX_COMPLEX_TYPE];
      default:
        return 0;
    }
}

// CREATORS

BasicSchemaChoice::BasicSchemaChoice(
    const BasicSchemaChoice& original,
    bslma::Allocator *basicAllocator)
: d_selectionId(original.d_selectionId)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    switch (d_selectionId) {
      case SELECTION_ID_ANNOTATION: {
        new (d_annotation.buffer())
            BasicSchemaAnnotationElement(
                original.d_annotation.object(), d_allocator_p);
      } break;
      case SELECTION_ID_SIMPLE_TYPE: {
        new (d_simpleType.buffer())
            BasicSchemaSimpleTypeElement(
                original.d_simpleType.object(), d_allocator_p);
      } break;
      case SELECTION_ID_COMPLEX_TYPE: {
        d_complexType = new (*d_allocator_p)
                BasicSchemaComplexTypeElement(*original.d_complexType, d_allocator_p);
      } break;
      default:
        BSLS_ASSERT(SELECTION_ID_UNDEFINED == d_selectionId);
    }
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
BasicSchemaChoice::BasicSchemaChoice(BasicSchemaChoice&& original) noexcept
: d_selectionId(original.d_selectionId)
, d_allocator_p(original.d_allocator_p)
{
    switch (d_selectionId) {
      case SELECTION_ID_ANNOTATION: {
        new (d_annotation.buffer())
            BasicSchemaAnnotationElement(
                bsl::move(original.d_annotation.object()), d_allocator_p);
      } break;
      case SELECTION_ID_SIMPLE_TYPE: {
        new (d_simpleType.buffer())
            BasicSchemaSimpleTypeElement(
                bsl::move(original.d_simpleType.object()), d_allocator_p);
      } break;
      case SELECTION_ID_COMPLEX_TYPE: {
        d_complexType = original.d_complexType;
        original.d_selectionId = SELECTION_ID_UNDEFINED;
      } break;
      default:
        BSLS_ASSERT(SELECTION_ID_UNDEFINED == d_selectionId);
    }
}

BasicSchemaChoice::BasicSchemaChoice(
    BasicSchemaChoice&& original,
    bslma::Allocator *basicAllocator)
: d_selectionId(original.d_selectionId)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    switch (d_selectionId) {
      case SELECTION_ID_ANNOTATION: {
        new (d_annotation.buffer())
            BasicSchemaAnnotationElement(
                bsl::move(original.d_annotation.object()), d_allocator_p);
      } break;
      case SELECTION_ID_SIMPLE_TYPE: {
        new (d_simpleType.buffer())
            BasicSchemaSimpleTypeElement(
                bsl::move(original.d_simpleType.object()), d_allocator_p);
      } break;
      case SELECTION_ID_COMPLEX_TYPE: {
        if (d_allocator_p == original.d_allocator_p) {
            d_complexType = original.d_complexType;
            original.d_selectionId = SELECTION_ID_UNDEFINED;
        }
        else {
            d_complexType = new (*d_allocator_p)
                    BasicSchemaComplexTypeElement(bsl::move(*original.d_complexType), d_allocator_p);
        }
      } break;
      default:
        BSLS_ASSERT(SELECTION_ID_UNDEFINED == d_selectionId);
    }
}
#endif

// MANIPULATORS

BasicSchemaChoice&
BasicSchemaChoice::operator=(const BasicSchemaChoice& rhs)
{
    if (this != &rhs) {
        switch (rhs.d_selectionId) {
          case SELECTION_ID_ANNOTATION: {
            makeAnnotation(rhs.d_annotation.object());
          } break;
          case SELECTION_ID_SIMPLE_TYPE: {
            makeSimpleType(rhs.d_simpleType.object());
          } break;
          case SELECTION_ID_COMPLEX_TYPE: {
            makeComplexType(*rhs.d_complexType);
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
BasicSchemaChoice&
BasicSchemaChoice::operator=(BasicSchemaChoice&& rhs)
{
    if (this != &rhs) {
        switch (rhs.d_selectionId) {
          case SELECTION_ID_ANNOTATION: {
            makeAnnotation(bsl::move(rhs.d_annotation.object()));
          } break;
          case SELECTION_ID_SIMPLE_TYPE: {
            makeSimpleType(bsl::move(rhs.d_simpleType.object()));
          } break;
          case SELECTION_ID_COMPLEX_TYPE: {
            if (d_allocator_p == rhs.d_allocator_p) {
                reset();
                d_complexType = rhs.d_complexType;
                d_selectionId = SELECTION_ID_COMPLEX_TYPE;
                rhs.d_selectionId = SELECTION_ID_UNDEFINED;
            }
            else {
                makeComplexType(bsl::move(*rhs.d_complexType));
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

void BasicSchemaChoice::reset()
{
    switch (d_selectionId) {
      case SELECTION_ID_ANNOTATION: {
        d_annotation.object().~BasicSchemaAnnotationElement();
      } break;
      case SELECTION_ID_SIMPLE_TYPE: {
        d_simpleType.object().~BasicSchemaSimpleTypeElement();
      } break;
      case SELECTION_ID_COMPLEX_TYPE: {
        d_allocator_p->deleteObject(d_complexType);
      } break;
      default:
        BSLS_ASSERT(SELECTION_ID_UNDEFINED == d_selectionId);
    }

    d_selectionId = SELECTION_ID_UNDEFINED;
}

int BasicSchemaChoice::makeSelection(int selectionId)
{
    switch (selectionId) {
      case SELECTION_ID_ANNOTATION: {
        makeAnnotation();
      } break;
      case SELECTION_ID_SIMPLE_TYPE: {
        makeSimpleType();
      } break;
      case SELECTION_ID_COMPLEX_TYPE: {
        makeComplexType();
      } break;
      case SELECTION_ID_UNDEFINED: {
        reset();
      } break;
      default:
        return -1;
    }
    return 0;
}

int BasicSchemaChoice::makeSelection(const char *name, int nameLength)
{
    const bdlat_SelectionInfo *selectionInfo =
                                         lookupSelectionInfo(name, nameLength);
    if (0 == selectionInfo) {
       return -1;
    }

    return makeSelection(selectionInfo->d_id);
}

BasicSchemaAnnotationElement& BasicSchemaChoice::makeAnnotation()
{
    if (SELECTION_ID_ANNOTATION == d_selectionId) {
        bdlat_ValueTypeFunctions::reset(&d_annotation.object());
    }
    else {
        reset();
        new (d_annotation.buffer())
                BasicSchemaAnnotationElement(d_allocator_p);
        d_selectionId = SELECTION_ID_ANNOTATION;
    }

    return d_annotation.object();
}

BasicSchemaAnnotationElement& BasicSchemaChoice::makeAnnotation(const BasicSchemaAnnotationElement& value)
{
    if (SELECTION_ID_ANNOTATION == d_selectionId) {
        d_annotation.object() = value;
    }
    else {
        reset();
        new (d_annotation.buffer())
                BasicSchemaAnnotationElement(value, d_allocator_p);
        d_selectionId = SELECTION_ID_ANNOTATION;
    }

    return d_annotation.object();
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
BasicSchemaAnnotationElement& BasicSchemaChoice::makeAnnotation(BasicSchemaAnnotationElement&& value)
{
    if (SELECTION_ID_ANNOTATION == d_selectionId) {
        d_annotation.object() = bsl::move(value);
    }
    else {
        reset();
        new (d_annotation.buffer())
                BasicSchemaAnnotationElement(bsl::move(value), d_allocator_p);
        d_selectionId = SELECTION_ID_ANNOTATION;
    }

    return d_annotation.object();
}
#endif

BasicSchemaSimpleTypeElement& BasicSchemaChoice::makeSimpleType()
{
    if (SELECTION_ID_SIMPLE_TYPE == d_selectionId) {
        bdlat_ValueTypeFunctions::reset(&d_simpleType.object());
    }
    else {
        reset();
        new (d_simpleType.buffer())
                BasicSchemaSimpleTypeElement(d_allocator_p);
        d_selectionId = SELECTION_ID_SIMPLE_TYPE;
    }

    return d_simpleType.object();
}

BasicSchemaSimpleTypeElement& BasicSchemaChoice::makeSimpleType(const BasicSchemaSimpleTypeElement& value)
{
    if (SELECTION_ID_SIMPLE_TYPE == d_selectionId) {
        d_simpleType.object() = value;
    }
    else {
        reset();
        new (d_simpleType.buffer())
                BasicSchemaSimpleTypeElement(value, d_allocator_p);
        d_selectionId = SELECTION_ID_SIMPLE_TYPE;
    }

    return d_simpleType.object();
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
BasicSchemaSimpleTypeElement& BasicSchemaChoice::makeSimpleType(BasicSchemaSimpleTypeElement&& value)
{
    if (SELECTION_ID_SIMPLE_TYPE == d_selectionId) {
        d_simpleType.object() = bsl::move(value);
    }
    else {
        reset();
        new (d_simpleType.buffer())
                BasicSchemaSimpleTypeElement(bsl::move(value), d_allocator_p);
        d_selectionId = SELECTION_ID_SIMPLE_TYPE;
    }

    return d_simpleType.object();
}
#endif

BasicSchemaComplexTypeElement& BasicSchemaChoice::makeComplexType()
{
    if (SELECTION_ID_COMPLEX_TYPE == d_selectionId) {
        bdlat_ValueTypeFunctions::reset(d_complexType);
    }
    else {
        reset();
        d_complexType = new (*d_allocator_p)
                BasicSchemaComplexTypeElement(d_allocator_p);
        d_selectionId = SELECTION_ID_COMPLEX_TYPE;
    }

    return *d_complexType;
}

BasicSchemaComplexTypeElement& BasicSchemaChoice::makeComplexType(const BasicSchemaComplexTypeElement& value)
{
    if (SELECTION_ID_COMPLEX_TYPE == d_selectionId) {
        *d_complexType = value;
    }
    else {
        reset();
        d_complexType = new (*d_allocator_p)
                BasicSchemaComplexTypeElement(value, d_allocator_p);
        d_selectionId = SELECTION_ID_COMPLEX_TYPE;
    }

    return *d_complexType;
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
BasicSchemaComplexTypeElement& BasicSchemaChoice::makeComplexType(BasicSchemaComplexTypeElement&& value)
{
    if (SELECTION_ID_COMPLEX_TYPE == d_selectionId) {
        *d_complexType = bsl::move(value);
    }
    else {
        reset();
        d_complexType = new (*d_allocator_p)
                BasicSchemaComplexTypeElement(bsl::move(value), d_allocator_p);
        d_selectionId = SELECTION_ID_COMPLEX_TYPE;
    }

    return *d_complexType;
}
#endif

// ACCESSORS

bsl::ostream& BasicSchemaChoice::print(
    bsl::ostream& stream,
    int           level,
    int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    switch (d_selectionId) {
      case SELECTION_ID_ANNOTATION: {
        printer.printAttribute("annotation", d_annotation.object());
      }  break;
      case SELECTION_ID_SIMPLE_TYPE: {
        printer.printAttribute("simpleType", d_simpleType.object());
      }  break;
      case SELECTION_ID_COMPLEX_TYPE: {
    printer.printAttribute("complexType", *d_complexType);
      }  break;
      default:
        stream << "SELECTION UNDEFINED\n";
    }
    printer.end();
    return stream;
}


const char *BasicSchemaChoice::selectionName() const
{
    switch (d_selectionId) {
      case SELECTION_ID_ANNOTATION:
        return SELECTION_INFO_ARRAY[SELECTION_INDEX_ANNOTATION].name();
      case SELECTION_ID_SIMPLE_TYPE:
        return SELECTION_INFO_ARRAY[SELECTION_INDEX_SIMPLE_TYPE].name();
      case SELECTION_ID_COMPLEX_TYPE:
        return SELECTION_INFO_ARRAY[SELECTION_INDEX_COMPLEX_TYPE].name();
      default:
        BSLS_ASSERT(SELECTION_ID_UNDEFINED == d_selectionId);
        return "(* UNDEFINED *)";
    }
}

                             // -----------------
                             // class BasicSchema
                             // -----------------

// CONSTANTS

const char BasicSchema::CLASS_NAME[] = "BasicSchema";

const bdlat_AttributeInfo BasicSchema::ATTRIBUTE_INFO_ARRAY[] = {
    {
        ATTRIBUTE_ID_XS,
        "xs",
        sizeof("xs") - 1,
        "",
        bdlat_FormattingMode::e_TEXT
      | bdlat_FormattingMode::e_ATTRIBUTE
    },
    {
        ATTRIBUTE_ID_BDEM,
        "bdem",
        sizeof("bdem") - 1,
        "",
        bdlat_FormattingMode::e_TEXT
      | bdlat_FormattingMode::e_ATTRIBUTE
    },
    {
        ATTRIBUTE_ID_PACKAGE,
        "package",
        sizeof("package") - 1,
        "",
        bdlat_FormattingMode::e_TEXT
      | bdlat_FormattingMode::e_ATTRIBUTE
    },
    {
        ATTRIBUTE_ID_ELEMENT_FORM_DEFAULT,
        "elementFormDefault",
        sizeof("elementFormDefault") - 1,
        "",
        bdlat_FormattingMode::e_TEXT
      | bdlat_FormattingMode::e_ATTRIBUTE
    },
    {
        ATTRIBUTE_ID_CHOICE,
        "Choice",
        sizeof("Choice") - 1,
        "",
        bdlat_FormattingMode::e_DEFAULT
      | bdlat_FormattingMode::e_UNTAGGED
    }
};

// CLASS METHODS

const bdlat_AttributeInfo *BasicSchema::lookupAttributeInfo(
        const char *name,
        int         nameLength)
{
    if (bdlb::String::areEqualCaseless("annotation", name, nameLength)) {
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CHOICE];
    }

    if (bdlb::String::areEqualCaseless("simpleType", name, nameLength)) {
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CHOICE];
    }

    if (bdlb::String::areEqualCaseless("complexType", name, nameLength)) {
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CHOICE];
    }

    for (int i = 0; i < 5; ++i) {
        const bdlat_AttributeInfo& attributeInfo =
                    BasicSchema::ATTRIBUTE_INFO_ARRAY[i];

        if (nameLength == attributeInfo.d_nameLength
        &&  0 == bsl::memcmp(attributeInfo.d_name_p, name, nameLength))
        {
            return &attributeInfo;
        }
    }

    return 0;
}

const bdlat_AttributeInfo *BasicSchema::lookupAttributeInfo(int id)
{
    switch (id) {
      case ATTRIBUTE_ID_XS:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_XS];
      case ATTRIBUTE_ID_BDEM:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_BDEM];
      case ATTRIBUTE_ID_PACKAGE:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_PACKAGE];
      case ATTRIBUTE_ID_ELEMENT_FORM_DEFAULT:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT_FORM_DEFAULT];
      case ATTRIBUTE_ID_CHOICE:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CHOICE];
      default:
        return 0;
    }
}

// CREATORS

BasicSchema::BasicSchema(bslma::Allocator *basicAllocator)
: d_choice(basicAllocator)
, d_xs(basicAllocator)
, d_bdem(basicAllocator)
, d_package(basicAllocator)
, d_elementFormDefault(basicAllocator)
{
}

BasicSchema::BasicSchema(const BasicSchema& original,
                         bslma::Allocator *basicAllocator)
: d_choice(original.d_choice, basicAllocator)
, d_xs(original.d_xs, basicAllocator)
, d_bdem(original.d_bdem, basicAllocator)
, d_package(original.d_package, basicAllocator)
, d_elementFormDefault(original.d_elementFormDefault, basicAllocator)
{
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
BasicSchema::BasicSchema(BasicSchema&& original) noexcept
: d_choice(bsl::move(original.d_choice))
, d_xs(bsl::move(original.d_xs))
, d_bdem(bsl::move(original.d_bdem))
, d_package(bsl::move(original.d_package))
, d_elementFormDefault(bsl::move(original.d_elementFormDefault))
{
}

BasicSchema::BasicSchema(BasicSchema&& original,
                         bslma::Allocator *basicAllocator)
: d_choice(bsl::move(original.d_choice), basicAllocator)
, d_xs(bsl::move(original.d_xs), basicAllocator)
, d_bdem(bsl::move(original.d_bdem), basicAllocator)
, d_package(bsl::move(original.d_package), basicAllocator)
, d_elementFormDefault(bsl::move(original.d_elementFormDefault), basicAllocator)
{
}
#endif

BasicSchema::~BasicSchema()
{
}

// MANIPULATORS

BasicSchema&
BasicSchema::operator=(const BasicSchema& rhs)
{
    if (this != &rhs) {
        d_xs = rhs.d_xs;
        d_bdem = rhs.d_bdem;
        d_package = rhs.d_package;
        d_elementFormDefault = rhs.d_elementFormDefault;
        d_choice = rhs.d_choice;
    }

    return *this;
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
BasicSchema&
BasicSchema::operator=(BasicSchema&& rhs)
{
    if (this != &rhs) {
        d_xs = bsl::move(rhs.d_xs);
        d_bdem = bsl::move(rhs.d_bdem);
        d_package = bsl::move(rhs.d_package);
        d_elementFormDefault = bsl::move(rhs.d_elementFormDefault);
        d_choice = bsl::move(rhs.d_choice);
    }

    return *this;
}
#endif

void BasicSchema::reset()
{
    bdlat_ValueTypeFunctions::reset(&d_xs);
    bdlat_ValueTypeFunctions::reset(&d_bdem);
    bdlat_ValueTypeFunctions::reset(&d_package);
    bdlat_ValueTypeFunctions::reset(&d_elementFormDefault);
    bdlat_ValueTypeFunctions::reset(&d_choice);
}

// ACCESSORS

bsl::ostream& BasicSchema::print(
        bsl::ostream& stream,
        int           level,
        int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute("xs", d_xs);
    printer.printAttribute("bdem", d_bdem);
    printer.printAttribute("package", d_package);
    printer.printAttribute("elementFormDefault", d_elementFormDefault);
    printer.printAttribute("choice", d_choice);
    printer.end();
    return stream;
}


}  // close package namespace
}  // close enterprise namespace

// GENERATED BY BLP_BAS_CODEGEN_VERSION
// USING bas_codegen.pl --mode msg --noExternalization --noAggregateConversion --package s_baltst --msgComponent basicschema s_baltst_basicschema.xsd
// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright 2020 Bloomberg Finance L.P. All rights reserved.
//      Property of Bloomberg Finance L.P. (BFLP)
//      This software is made available solely pursuant to the
//      terms of a BFLP license agreement which governs its use.
// ------------------------------- END-OF-FILE --------------------------------
