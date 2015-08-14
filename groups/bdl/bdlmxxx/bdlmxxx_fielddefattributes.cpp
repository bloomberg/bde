// bdlmxxx_fielddefattributes.cpp                                     -*-C++-*-
#include <bdlmxxx_fielddefattributes.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlmxxx_fielddefattributes_cpp,"$Id$ $CSID$")

#include <bdlmxxx_aggregateoption.h>
#include <bdlmxxx_elemattrlookup.h>

#include <bdlat_formattingmode.h>
#include <bdlimpxxx_bitwisecopy.h>
#include <bdlb_print.h>

#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bsls_assert.h>

#include <bsl_ios.h>
#include <bsl_ostream.h>

#include <bsl_climits.h>
#include <bsl_cstring.h>

namespace BloombergLP {

// STATIC HELPER FUNCTIONS
static
const bdlmxxx::Descriptor *defaultValueDescriptor(bdlmxxx::ElemType::Type fieldType)
    // Return the descriptor for the default value corresponding to the
    // specified 'fieldType'.  If 'fieldType' is a scalar, return the
    // descriptor for 'fieldType'.  If 'fieldType' is an array of scalars,
    // return the descriptor for the corresponding scalar type.  Otherwise,
    // return the descriptor for the 'VOID' type.
{
    if (bdlmxxx::ElemType::isArrayType(fieldType)) {
        fieldType = bdlmxxx::ElemType::fromArrayType(fieldType);
    }

    if (! bdlmxxx::ElemType::isScalarType(fieldType)) {
        fieldType = bdlmxxx::ElemType::BDEM_VOID;
    }

    return bdlmxxx::ElemAttrLookup::lookupTable()[fieldType];
}

namespace bdlmxxx {
                     // -----------------------------
                     // class FieldDefAttributes
                     // -----------------------------

// CREATORS
FieldDefAttributes::FieldDefAttributes(
                                              bslma::Allocator *basicAllocator)
: d_elemType(ElemType::BDEM_VOID)
, d_flags(DEFAULT_VALUE_IS_NULL_FLAG)
, d_formattingMode(bdeat_FormattingMode::BDEAT_DEFAULT)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    // Note that 'defaultValueDescriptor' converts from array type to scalar,
    // and from aggregate to 'VOID'.

    defaultValueDescriptor(ElemType::BDEM_VOID)
                      ->unsetConstruct(&d_defaultValue,
                                       AggregateOption::BDEM_PASS_THROUGH,
                                       d_allocator_p);
}

FieldDefAttributes::FieldDefAttributes(
                                           ElemType::Type  type,
                                           bslma::Allocator    *basicAllocator)
: d_elemType(type)
, d_flags(DEFAULT_VALUE_IS_NULL_FLAG)
, d_formattingMode(bdeat_FormattingMode::BDEAT_DEFAULT)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    // Note that 'defaultValueDescriptor' converts from array type to scalar,
    // and from aggregate to 'VOID'.

    defaultValueDescriptor(type)->unsetConstruct(
                                       &d_defaultValue,
                                       AggregateOption::BDEM_PASS_THROUGH,
                                       d_allocator_p);
}

FieldDefAttributes::FieldDefAttributes(
                                         ElemType::Type  type,
                                         bool                 nullabilityFlag,
                                         bslma::Allocator    *basicAllocator)
: d_elemType(type)
, d_flags((nullabilityFlag ? IS_NULLABLE_FLAG : 0) |
                                                    DEFAULT_VALUE_IS_NULL_FLAG)
, d_formattingMode(bdeat_FormattingMode::BDEAT_DEFAULT)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    // Note that 'defaultValueDescriptor' converts from array type to scalar,
    // and from aggregate to 'VOID'.

    defaultValueDescriptor(type)->unsetConstruct(
                                       &d_defaultValue,
                                       AggregateOption::BDEM_PASS_THROUGH,
                                       d_allocator_p);
}

FieldDefAttributes::FieldDefAttributes(
                                         ElemType::Type  type,
                                         bool                 nullabilityFlag,
                                         int                  formattingMode,
                                         bslma::Allocator    *basicAllocator)
: d_elemType(type)
, d_flags((nullabilityFlag ? IS_NULLABLE_FLAG : 0) |
                                                    DEFAULT_VALUE_IS_NULL_FLAG)
, d_formattingMode(formattingMode)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    // Note that 'defaultValueDescriptor' converts from array type to scalar,
    // and from aggregate to 'VOID'.

    defaultValueDescriptor(type)->unsetConstruct(
                                       &d_defaultValue,
                                       AggregateOption::BDEM_PASS_THROUGH,
                                       d_allocator_p);
}

FieldDefAttributes::FieldDefAttributes(
                                const FieldDefAttributes&  original,
                                bslma::Allocator               *basicAllocator)
: d_elemType(original.d_elemType)
, d_flags(original.d_flags)
, d_formattingMode(original.d_formattingMode)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    // Note that 'defaultValueDescriptor' converts from array type to scalar,
    // and from aggregate to 'VOID'.

    defaultValueDescriptor(d_elemType)->copyConstruct(
                                       &d_defaultValue,
                                       &original.d_defaultValue,
                                       AggregateOption::BDEM_PASS_THROUGH,
                                       d_allocator_p);
}

FieldDefAttributes::~FieldDefAttributes()
{
    // Note that 'defaultValueDescriptor' converts from array type to scalar,
    // and from aggregate to 'VOID'.

    defaultValueDescriptor(d_elemType)->destroy(&d_defaultValue);
}

// MANIPULATORS
FieldDefAttributes& FieldDefAttributes::operator=(
                                            const FieldDefAttributes& rhs)
{
    FieldDefAttributes(rhs, d_allocator_p).swap(*this);
    return *this;
}

ElemRef FieldDefAttributes::defaultValue()
{
    BSLS_ASSERT(!ElemType::isAggregateType(d_elemType));

    // If field is an array of scalars, then its default value will have the
    // corresponding scalar type.

    ElemType::Type defaultType = d_elemType;
    if (ElemType::isArrayType(defaultType)) {
        defaultType = ElemType::fromArrayType(defaultType);
    }

    return ElemRef(&d_defaultValue,
                        ElemAttrLookup::lookupTable()[defaultType],
                        &d_flags,
                        DEFAULT_VALUE_IS_NULL_BIT_POS);
}

void FieldDefAttributes::reset(ElemType::Type type,
                                    bool                nullabilityFlag,
                                    int                 formattingMode)
{
    BSLS_ASSERT(type >= -ElemType::BDEM_NUM_PSEUDO_TYPES);
    BSLS_ASSERT(static_cast<int>(type) < ElemType::BDEM_NUM_TYPES);

    *this = FieldDefAttributes(type, nullabilityFlag, formattingMode);
}

void FieldDefAttributes::swap(FieldDefAttributes& other)
{
    // 'swap' is undefined for non-equal allocators.

    BSLS_ASSERT(d_allocator_p == other.d_allocator_p);

    bdlimpxxx::BitwiseCopy<FieldDefAttributes>::swap(this, &other);
}

// ACCESSORS
bsl::ostream&
FieldDefAttributes::print(bsl::ostream& stream,
                               int           level,
                               int           spacesPerLevel) const
{
    if (0 <= spacesPerLevel) {
        bdlb::Print::indent(stream, level, spacesPerLevel);
        stream << "{\n";
    }
    else {
        // No newlines if '0 > spacesPerLevel'.
        stream << '{';
    }

    if (level < 0) {
        level = -level;
    }

    int levelPlus1 = level + 1;

    if (0 <= spacesPerLevel) {
        bdlb::Print::indent(stream, levelPlus1, spacesPerLevel);
    }
    else {
        stream << ' ';
    }
    stream << ElemType::toAscii(d_elemType);

    if (0 <= spacesPerLevel) {
        stream << '\n';
        bdlb::Print::indent(stream, levelPlus1, spacesPerLevel);
    }
    else {
        stream << ' ';
    }
    stream << ((d_flags & IS_NULLABLE_FLAG) ? "" : "!") << "nullable";

    if (hasDefaultValue()) {
        if (0 <= spacesPerLevel) {
            stream << '\n';
            bdlb::Print::indent(stream, levelPlus1, spacesPerLevel);
        }
        else {
            stream << ' ';
        }
        BSLS_ASSERT(! ElemType::isAggregateType(d_elemType));
        const Descriptor *defaultDesc =
                                            defaultValueDescriptor(d_elemType);
        defaultDesc->print(&d_defaultValue, stream, 0, -1);
    }

    if (0 <= spacesPerLevel) {
        stream << '\n';
        bdlb::Print::indent(stream, levelPlus1, spacesPerLevel);
    }
    else {
        stream << ' ';
    }
    stream << "0x" << bsl::hex << formattingMode() << bsl::dec;

    if (0 <= spacesPerLevel) {
        stream << '\n';
        bdlb::Print::indent(stream, level, spacesPerLevel);
        stream << "}\n";
    }
    else {
        stream << " }";
    }

    return stream << bsl::flush;
}
}  // close package namespace

// FREE OPERATORS
bool bdlmxxx::operator==(const FieldDefAttributes& lhs,
                const FieldDefAttributes& rhs)
{
    return lhs.elemType()        == rhs.elemType()
        && lhs.isNullable()      == rhs.isNullable()
        && lhs.formattingMode()  == rhs.formattingMode()
        && lhs.hasDefaultValue() == rhs.hasDefaultValue()
        && (!lhs.hasDefaultValue() ||
                                    lhs.defaultValue() == rhs.defaultValue());
}

}  // close enterprise namespace

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2002
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ----------------------------------
