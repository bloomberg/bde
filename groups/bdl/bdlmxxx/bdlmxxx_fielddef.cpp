// bdlmxxx_fielddef.cpp                                               -*-C++-*-
#include <bdlmxxx_fielddef.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlmxxx_fielddef_cpp,"$Id$ $CSID$")

namespace BloombergLP {

namespace bdlmxxx {
                          // -------------------
                          // class FieldDef
                          // -------------------

// ACCESSORS
FieldSpec FieldDef::fieldSpec() const
{
    FieldSpec tmp(elemType(), 0, formattingMode(), isNullable());
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
}  // close package namespace

}  // close enterprise namespace

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2010
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ----------------------------------
