// bdlmxxx_fieldspec.cpp                                              -*-C++-*-
#include <bdlmxxx_fieldspec.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlmxxx_fieldspec_cpp,"$Id$ $CSID$")

#include <bdlmxxx_enumerationdef.h>

#include <bslma_allocator.h>
#include <bslma_default.h>

#include <bdlb_print.h>

#include <bsls_assert.h>

#include <bsl_ios.h>
#include <bsl_ostream.h>

namespace BloombergLP {

namespace bdlmxxx {
                      // --------------------
                      // class FieldSpec
                      // --------------------

// MANIPULATORS
int FieldSpec::reset(ElemType::Type   type,
                          const RecordDef *constraint,
                          int                   formattingMode,
                          bool                  nullabilityFlag)
{
    if (constraint) {
        if (type != ElemType::BDEM_LIST    &&
            type != ElemType::BDEM_TABLE   &&
            type != ElemType::BDEM_CHOICE  &&
            type != ElemType::BDEM_CHOICE_ARRAY) {
            return -1;                                                // RETURN
        }
    }

    d_attributes.reset(type, nullabilityFlag, formattingMode);
    d_recordConstraint_p = constraint;
    return 0;
}

int FieldSpec::reset(ElemType::Type  type,
                          LowPrecedenceEnumPtr constraint,
                          int                  formattingMode,
                          bool                 nullabilityFlag)
{
    if (constraint.d_ptr
     && ! EnumerationDef::canHaveEnumConstraint(type)) {
        return -1;                                                    // RETURN
    }

    d_attributes.reset(type, nullabilityFlag, formattingMode);
    d_enumConstraint_p = constraint.d_ptr;

    return 0;
}

int FieldSpec::setConstraint(const RecordDef *constraint)
{
    if (constraint) {
        if (elemType() != ElemType::BDEM_LIST    &&
            elemType() != ElemType::BDEM_TABLE   &&
            elemType() != ElemType::BDEM_CHOICE  &&
            elemType() != ElemType::BDEM_CHOICE_ARRAY) {
            return -1;                                                // RETURN
        }
    }
    d_recordConstraint_p = constraint;
    return 0;
}

int FieldSpec::setConstraint(LowPrecedenceEnumPtr constraint)
{
    if (constraint.d_ptr
     && ! EnumerationDef::canHaveEnumConstraint(elemType())) {
        return -1;                                                    // RETURN
    }

    d_enumConstraint_p = constraint.d_ptr;

    return 0;
}

// ACCESSORS
bsl::ostream&
FieldSpec::print(bsl::ostream& stream,
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
    stream << ElemType::toAscii(elemType());

    if (recordConstraint()) {
        stream << "[RECORD CONSTRAINT]";
    }
    else if (enumerationConstraint()) {
        stream << "[ENUM CONSTRAINT]";
    }

    if (0 <= spacesPerLevel) {
        stream << '\n';
        bdlb::Print::indent(stream, levelPlus1, spacesPerLevel);
    }
    else {
        stream << ' ';
    }
    stream << (d_attributes.isNullable() ? "" : "!") << "nullable";

    if (0 <= spacesPerLevel) {
        stream << '\n';
        bdlb::Print::indent(stream, levelPlus1, spacesPerLevel);
    }
    else {
        stream << ' ';
    }
    stream << "0x" << bsl::hex << d_attributes.formattingMode() << bsl::dec;

    if (hasDefaultValue()) {
        if (0 <= spacesPerLevel) {
            stream << '\n';
            bdlb::Print::indent(stream, levelPlus1, spacesPerLevel);
        }
        else {
            stream << ' ';
        }
        BSLS_ASSERT(! ElemType::isAggregateType(elemType()));
        d_attributes.defaultValue().print(stream, 0, -1);
    }

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

}  // close enterprise namespace

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2002
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ----------------------------------
