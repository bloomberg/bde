// bdem_fielddefattributes.cpp                                        -*-C++-*-
#include <bdem_fielddefattributes.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bdem_fielddefattributes_cpp,"$Id$ $CSID$")

#include <bdem_aggregateoption.h>
#include <bdem_elemattrlookup.h>

#include <bdeat_formattingmode.h>
#include <bdeu_print.h>

#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bsls_assert.h>

#include <bsl_ostream.h>

#include <bsl_climits.h>
#include <bsl_cstring.h>

namespace BloombergLP {

// STATIC HELPER FUNCTIONS
static
const bdem_Descriptor *defaultValueDescriptor(bdem_ElemType::Type fieldType)
    // Return the descriptor for the default value corresponding to the
    // specified 'fieldType'.  If 'fieldType' is a scalar, return the
    // descriptor for 'fieldType'.  If 'fieldType' is an array of scalars,
    // return the descriptor for the corresponding scalar type.  Otherwise,
    // return the descriptor for the 'VOID' type.
{
    if (bdem_ElemType::isArrayType(fieldType)) {
        fieldType = bdem_ElemType::fromArrayType(fieldType);
    }

    if (! bdem_ElemType::isScalarType(fieldType)) {
        fieldType = bdem_ElemType::BDEM_VOID;
    }

    return bdem_ElemAttrLookup::lookupTable()[fieldType];
}

                     // -----------------------------
                     // class bdem_FieldDefAttributes
                     // -----------------------------

// CREATORS
bdem_FieldDefAttributes::bdem_FieldDefAttributes(
                                               bslma_Allocator *basicAllocator)
: d_elemType(bdem_ElemType::BDEM_VOID)
, d_flags(DEFAULT_VALUE_IS_NULL_FLAG)
, d_formattingMode(bdeat_FormattingMode::BDEAT_DEFAULT)
, d_allocator_p(bslma_Default::allocator(basicAllocator))
{
    // Note that 'defaultValueDescriptor' converts from array type to scalar,
    // and from aggregate to 'VOID'.

    defaultValueDescriptor(bdem_ElemType::BDEM_VOID)
                      ->unsetConstruct(&d_defaultValue,
                                       bdem_AggregateOption::BDEM_PASS_THROUGH,
                                       d_allocator_p);
}

bdem_FieldDefAttributes::bdem_FieldDefAttributes(
                                           bdem_ElemType::Type  type,
                                           bslma_Allocator     *basicAllocator)
: d_elemType(type)
, d_flags(DEFAULT_VALUE_IS_NULL_FLAG)
, d_formattingMode(bdeat_FormattingMode::BDEAT_DEFAULT)
, d_allocator_p(bslma_Default::allocator(basicAllocator))
{
    // Note that 'defaultValueDescriptor' converts from array type to scalar,
    // and from aggregate to 'VOID'.

    defaultValueDescriptor(type)->unsetConstruct(
                                       &d_defaultValue,
                                       bdem_AggregateOption::BDEM_PASS_THROUGH,
                                       d_allocator_p);
}

bdem_FieldDefAttributes::bdem_FieldDefAttributes(
                                         bdem_ElemType::Type  type,
                                         bool                 nullabilityFlag,
                                         bslma_Allocator     *basicAllocator)
: d_elemType(type)
, d_flags((nullabilityFlag ? IS_NULLABLE_FLAG : 0) |
                                                    DEFAULT_VALUE_IS_NULL_FLAG)
, d_formattingMode(bdeat_FormattingMode::BDEAT_DEFAULT)
, d_allocator_p(bslma_Default::allocator(basicAllocator))
{
    // Note that 'defaultValueDescriptor' converts from array type to scalar,
    // and from aggregate to 'VOID'.

    defaultValueDescriptor(type)->unsetConstruct(
                                       &d_defaultValue,
                                       bdem_AggregateOption::BDEM_PASS_THROUGH,
                                       d_allocator_p);
}

bdem_FieldDefAttributes::bdem_FieldDefAttributes(
                                         bdem_ElemType::Type  type,
                                         bool                 nullabilityFlag,
                                         int                  formattingMode,
                                         bslma_Allocator     *basicAllocator)
: d_elemType(type)
, d_flags((nullabilityFlag ? IS_NULLABLE_FLAG : 0) |
                                                    DEFAULT_VALUE_IS_NULL_FLAG)
, d_formattingMode(formattingMode)
, d_allocator_p(bslma_Default::allocator(basicAllocator))
{
    // Note that 'defaultValueDescriptor' converts from array type to scalar,
    // and from aggregate to 'VOID'.

    defaultValueDescriptor(type)->unsetConstruct(
                                       &d_defaultValue,
                                       bdem_AggregateOption::BDEM_PASS_THROUGH,
                                       d_allocator_p);
}

bdem_FieldDefAttributes::bdem_FieldDefAttributes(
                                const bdem_FieldDefAttributes&  original,
                                bslma_Allocator                *basicAllocator)
: d_elemType(original.d_elemType)
, d_flags(original.d_flags)
, d_formattingMode(original.d_formattingMode)
, d_allocator_p(bslma_Default::allocator(basicAllocator))
{
    // Note that 'defaultValueDescriptor' converts from array type to scalar,
    // and from aggregate to 'VOID'.

    defaultValueDescriptor(d_elemType)->copyConstruct(
                                       &d_defaultValue,
                                       &original.d_defaultValue,
                                       bdem_AggregateOption::BDEM_PASS_THROUGH,
                                       d_allocator_p);
}

bdem_FieldDefAttributes::~bdem_FieldDefAttributes()
{
    // Note that 'defaultValueDescriptor' converts from array type to scalar,
    // and from aggregate to 'VOID'.

    defaultValueDescriptor(d_elemType)->destroy(&d_defaultValue);
}

// MANIPULATORS
bdem_FieldDefAttributes& bdem_FieldDefAttributes::operator=(
                                            const bdem_FieldDefAttributes& rhs)
{
    if (this == &rhs) {
        return *this;                                                 // RETURN
    }

    // Note that 'defaultValueDescriptor' converts from array type to scalar,
    // and from aggregate to 'VOID'.
    const bdem_Descriptor *lhsDefDesc = defaultValueDescriptor(d_elemType);
    const bdem_Descriptor *rhsDefDesc = defaultValueDescriptor(rhs.d_elemType);

    // Assignment and copy construction of 'STRING' default values are the only
    // operations that can throw.  Note that 'd_elemType' is assigned *after*
    // the default value is assigned to avoid creating a field def attributes
    // object of type 'STRING' that has an unconstructed default value.

    if (lhsDefDesc == rhsDefDesc) {
        // Exception safety:
        // If 'assign' throws, 'lhs' will be left unchanged.

        lhsDefDesc->assign(&d_defaultValue, &rhs.d_defaultValue);
    }
    else if (bdem_ElemType::BDEM_STRING == rhs.d_elemType) {
        // Exception safety (non-'STRING' = 'STRING'):
        // 'copyConstruct' can throw, while also possibly corrupting
        // 'd_defaultValue' in the process (i.e., if it were passed as the
        // first argument to 'copyConstruct' as in the 'else' clause below).
        // For example, 'd_datetime' may be left in a state that is not valid
        // for a 'bdet_Datetime' object.  So the 'STRING' default value is
        // constructed in a temporary, and then copied into 'd_defaultValue' if
        // no exception is thrown.

        bsls_ObjectBuffer<bsl::string> tmp;

        rhsDefDesc->copyConstruct(&tmp,
                                  &rhs.d_defaultValue,
                                  bdem_AggregateOption::BDEM_PASS_THROUGH,
                                  d_allocator_p);

        lhsDefDesc->destroy(&d_defaultValue);

        bsl::memcpy(&d_defaultValue, tmp.buffer(), sizeof(bsl::string));
    }
    else {
        // Exception safety ('STRING' = non-'STRING'):
        // 'copyConstruct' cannot throw.

        lhsDefDesc->destroy(&d_defaultValue);

        rhsDefDesc->copyConstruct(&d_defaultValue,
                                  &rhs.d_defaultValue,
                                  bdem_AggregateOption::BDEM_PASS_THROUGH,
                                  d_allocator_p);
    }

    d_elemType       = rhs.d_elemType;
    d_flags          = rhs.d_flags;
    d_formattingMode = rhs.d_formattingMode;

    return *this;
}

bdem_ElemRef bdem_FieldDefAttributes::defaultValue()
{
    BSLS_ASSERT(!bdem_ElemType::isAggregateType(d_elemType));

    // If field is an array of scalars, then its default value will have the
    // corresponding scalar type.

    bdem_ElemType::Type defaultType = d_elemType;
    if (bdem_ElemType::isArrayType(defaultType)) {
        defaultType = bdem_ElemType::fromArrayType(defaultType);
    }

    return bdem_ElemRef(&d_defaultValue,
                        bdem_ElemAttrLookup::lookupTable()[defaultType],
                        &d_flags,
                        DEFAULT_VALUE_IS_NULL_BIT_POS);
}

void bdem_FieldDefAttributes::reset(bdem_ElemType::Type type,
                                    bool                nullabilityFlag,
                                    int                 formattingMode)
{
    BSLS_ASSERT(type >= -bdem_ElemType::BDEM_NUM_PSEUDO_TYPES);
    BSLS_ASSERT(type < (int)bdem_ElemType::BDEM_NUM_TYPES);

    *this = bdem_FieldDefAttributes(type, nullabilityFlag, formattingMode);
}

// ACCESSORS
bsl::ostream&
bdem_FieldDefAttributes::print(bsl::ostream& stream,
                               int           level,
                               int           spacesPerLevel) const
{
    if (0 <= spacesPerLevel) {
        bdeu_Print::indent(stream, level, spacesPerLevel);
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
        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
    }
    else {
        stream << ' ';
    }
    stream << bdem_ElemType::toAscii(d_elemType);

    if (0 <= spacesPerLevel) {
        stream << '\n';
        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
    }
    else {
        stream << ' ';
    }
    stream << ((d_flags & IS_NULLABLE_FLAG) ? "" : "!") << "nullable";

    if (hasDefaultValue()) {
        if (0 <= spacesPerLevel) {
            stream << '\n';
            bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        }
        else {
            stream << ' ';
        }
        BSLS_ASSERT(! bdem_ElemType::isAggregateType(d_elemType));
        const bdem_Descriptor *defaultDesc =
                                            defaultValueDescriptor(d_elemType);
        defaultDesc->print(&d_defaultValue, stream, 0, -1);
    }

    if (0 <= spacesPerLevel) {
        stream << '\n';
        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
    }
    else {
        stream << ' ';
    }
    stream << "0x" << bsl::hex << formattingMode() << bsl::dec;

    if (0 <= spacesPerLevel) {
        stream << '\n';
        bdeu_Print::indent(stream, level, spacesPerLevel);
        stream << "}\n";
    }
    else {
        stream << " }";
    }

    return stream << bsl::flush;
}

// FREE OPERATORS
bool operator==(const bdem_FieldDefAttributes& lhs,
                const bdem_FieldDefAttributes& rhs)
{
    return lhs.elemType()        == rhs.elemType()
        && lhs.isNullable()      == rhs.isNullable()
        && lhs.formattingMode()  == rhs.formattingMode()
        && lhs.hasDefaultValue() == rhs.hasDefaultValue()
        && (!lhs.hasDefaultValue() ||
                                    lhs.defaultValue() == rhs.defaultValue());
}

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2002
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
