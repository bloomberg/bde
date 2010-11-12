// bdeat_attributeinfo.cpp                  -*-C++-*-
#include <bdeat_attributeinfo.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bdeat_attributeinfo_cpp,"$Id$ $CSID$")

#include <bsl_ostream.h>

namespace BloombergLP {

                        // --------------------------
                        // struct bdeat_AttributeInfo
                        // --------------------------

// FREE OPERATORS
bsl::ostream& operator<<(bsl::ostream&              stream,
                         const bdeat_AttributeInfo& attributeInfo)
{
    stream << "( "  << attributeInfo.formattingMode()
           << ", "  << attributeInfo.id()
           << ", ";

    const int nameLen = attributeInfo.nameLength();
    if (nameLen) {
        stream.write(attributeInfo.name(), nameLen);
    }
    else {
        stream << "<null-name>";
    }

    stream << ", " << nameLen
           << ", ";

    const char *annotation = attributeInfo.annotation();
    if (annotation) {
        stream << annotation;
    }
    else {
        stream << "<null-annotation>";
    }

    stream << " )";

    return stream;
}

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2005
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
