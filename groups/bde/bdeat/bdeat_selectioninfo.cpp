// bdeat_selectioninfo.cpp                  -*-C++-*-
#include <bdeat_selectioninfo.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bdeat_selectioninfo_cpp,"$Id$ $CSID$")

#include <bsl_ostream.h>

namespace BloombergLP {

                        // --------------------------
                        // struct bdeat_SelectionInfo
                        // --------------------------

// FREE OPERATORS
bsl::ostream& operator<<(bsl::ostream&              stream,
                         const bdeat_SelectionInfo& selectionInfo)
{
    stream << "( " << selectionInfo.formattingMode()
           << ", " << selectionInfo.id()
           << ", ";

    const int nameLen = selectionInfo.nameLength();
    if (nameLen) {
        stream.write(selectionInfo.name(), nameLen);
    }
    else {
        stream << "<null-name>";
    }

    stream << ", " << nameLen
           << ", ";

    const char *annotation = selectionInfo.annotation();
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
