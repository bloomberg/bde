// bdem_fielddef.cpp                                                  -*-C++-*-
#include <bdem_fielddef.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bdem_fielddef_cpp,"$Id$ $CSID$")

namespace BloombergLP {

                          // -------------------
                          // class bdem_FieldDef
                          // -------------------

// ACCESSORS
bdem_FieldSpec bdem_FieldDef::fieldSpec() const
{
    bdem_FieldSpec tmp(elemType(), 0, formattingMode(), isNullable());
    if (enumerationConstraint()) {
        tmp.setConstraint(enumerationConstraint());
    }
    else {
        tmp.setConstraint(recordConstraint());
    }
    if (hasDefaultValue()) {
        tmp.defaultValue().replaceValue(defaultValue());
    }
    return tmp;
}

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2010
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
