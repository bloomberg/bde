// bdem_schemaenumerationutil.cpp         -*-C++-*-
#include <bdem_schemaenumerationutil.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bdem_schemaenumerationutil_cpp,"$Id$ $CSID$")


namespace BloombergLP {

                       // ---------------------------------
                       // struct bdem_SchemaEnumerationUtil
                       // ---------------------------------

// CLASS METHODS

bool bdem_SchemaEnumerationUtil::isConformant(
                                       const bsl::vector<int>&    value,
                                       const bdem_EnumerationDef& enumeration)
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

bool bdem_SchemaEnumerationUtil::isConformant(
                                   const bsl::vector<bsl::string>& value,
                                   const bdem_EnumerationDef&      enumeration)
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

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2008
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
