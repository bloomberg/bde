// bdlat_attributeinfo.cpp                                            -*-C++-*-
#include <bdlat_attributeinfo.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlat_attributeinfo_cpp,"$Id$ $CSID$")

#include <bsl_ostream.h>

namespace BloombergLP {

                        // --------------------------
                        // struct bdlat_AttributeInfo
                        // --------------------------

// FREE OPERATORS
bsl::ostream& operator<<(bsl::ostream&              stream,
                         const bdlat_AttributeInfo& attributeInfo)
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

}  // close enterprise namespace

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2005
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ----------------------------------
