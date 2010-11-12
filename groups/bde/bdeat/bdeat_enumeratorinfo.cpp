// bdeat_enumeratorinfo.cpp                  -*-C++-*-
#include <bdeat_enumeratorinfo.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bdeat_enumeratorinfo_cpp,"$Id$ $CSID$")

#include <bsl_ostream.h>

namespace BloombergLP {

                        // ---------------------------
                        // struct bdeat_EnumeratorInfo
                        // ---------------------------

// FREE OPERATORS
bsl::ostream& operator<<(bsl::ostream&               stream,
                         const bdeat_EnumeratorInfo& enumeratorInfo)
{
    stream << "( " << enumeratorInfo.value()
           << ", ";

    const int nameLen = enumeratorInfo.nameLength();
    if (nameLen) {
        stream.write(enumeratorInfo.name(), nameLen);
    }
    else {
        stream << "<null-name>";
    }

    stream << ", ";

    const char *annotation = enumeratorInfo.annotation();
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
