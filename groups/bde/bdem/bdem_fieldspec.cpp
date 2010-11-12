// bdem_fieldspec.cpp              -*-C++-*-
#include <bdem_fieldspec.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bdem_fieldspec_cpp,"$Id$ $CSID$")

#include <bdem_enumerationdef.h>

#include <bslma_allocator.h>
#include <bslma_default.h>

#include <bdeu_print.h>

#include <bsls_assert.h>

#include <bsl_ostream.h>

namespace BloombergLP {

                      // --------------------
                      // class bdem_FieldSpec
                      // --------------------

// MANIPULATORS
int bdem_FieldSpec::reset(bdem_ElemType::Type   type,
                          const bdem_RecordDef *constraint,
                          int                   formattingMode,
                          bool                  nullabilityFlag)
{
    if (constraint) {
        if (type != bdem_ElemType::BDEM_LIST    &&
            type != bdem_ElemType::BDEM_TABLE   &&
            type != bdem_ElemType::BDEM_CHOICE  &&
            type != bdem_ElemType::BDEM_CHOICE_ARRAY) {
            return -1;
        }
    }

    d_attributes.reset(type, nullabilityFlag, formattingMode);
    d_recordConstraint_p = constraint;
    return 0;
}

int bdem_FieldSpec::reset(bdem_ElemType::Type  type,
                          LowPrecedenceEnumPtr constraint,
                          int                  formattingMode,
                          bool                 nullabilityFlag)
{
    if (constraint.d_ptr
     && ! bdem_EnumerationDef::canHaveEnumConstraint(type)) {
        return -1;                                                    // RETURN
    }

    d_attributes.reset(type, nullabilityFlag, formattingMode);
    d_enumConstraint_p = constraint.d_ptr;

    return 0;
}

int bdem_FieldSpec::setConstraint(const bdem_RecordDef *constraint)
{
    if (constraint) {
        if (elemType() != bdem_ElemType::BDEM_LIST    &&
            elemType() != bdem_ElemType::BDEM_TABLE   &&
            elemType() != bdem_ElemType::BDEM_CHOICE  &&
            elemType() != bdem_ElemType::BDEM_CHOICE_ARRAY) {
            return -1;
        }
    }
    d_recordConstraint_p = constraint;
    return 0;
}

int bdem_FieldSpec::setConstraint(LowPrecedenceEnumPtr constraint)
{
    if (constraint.d_ptr
     && ! bdem_EnumerationDef::canHaveEnumConstraint(elemType())) {
        return -1;                                                    // RETURN
    }

    d_enumConstraint_p = constraint.d_ptr;

    return 0;
}

// ACCESSORS
bsl::ostream&
bdem_FieldSpec::print(bsl::ostream& stream,
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
    stream << bdem_ElemType::toAscii(elemType());

    if (recordConstraint()) {
        stream << "[RECORD CONSTRAINT]";
    }
    else if (enumerationConstraint()) {
        stream << "[ENUM CONSTRAINT]";
    }

    if (0 <= spacesPerLevel) {
        stream << '\n';
        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
    }
    else {
        stream << ' ';
    }
    stream << (d_attributes.isNullable() ? "" : "!") << "nullable";

    if (0 <= spacesPerLevel) {
        stream << '\n';
        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
    }
    else {
        stream << ' ';
    }
    stream << "0x" << bsl::hex << d_attributes.formattingMode() << bsl::dec;

    if (hasDefaultValue()) {
        if (0 <= spacesPerLevel) {
            stream << '\n';
            bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        }
        else {
            stream << ' ';
        }
        BSLS_ASSERT(! bdem_ElemType::isAggregateType(elemType()));
        d_attributes.defaultValue().print(stream, 0, -1);
    }

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

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2002
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
