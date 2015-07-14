// bdlmxxx_schemaenumerationutil.cpp         -*-C++-*-
#include <bdlmxxx_schemaenumerationutil.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlmxxx_schemaenumerationutil_cpp,"$Id$ $CSID$")


namespace BloombergLP {

namespace bdlmxxx {
                       // ---------------------------------
                       // struct SchemaEnumerationUtil
                       // ---------------------------------

// CLASS METHODS

bool SchemaEnumerationUtil::isConformant(
                                       const bsl::vector<int>&    value,
                                       const EnumerationDef& enumeration)
{
    typedef bsl::vector<int>::const_iterator Iterator;

    Iterator end = value.end();
    for (Iterator it = value.begin(); it != end; ++it) {
        if (!isConformant(*it, enumeration)) {
            return false;
        }
    }

    return true;
}

bool SchemaEnumerationUtil::isConformant(
                                   const bsl::vector<bsl::string>& value,
                                   const EnumerationDef&      enumeration)
{
    typedef bsl::vector<bsl::string>::const_iterator Iterator;

    Iterator end = value.end();
    for (Iterator it = value.begin(); it != end; ++it) {
        if (!isConformant(*it, enumeration)) {
            return false;
        }
    }

    return true;
}
}  // close package namespace

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2008
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
