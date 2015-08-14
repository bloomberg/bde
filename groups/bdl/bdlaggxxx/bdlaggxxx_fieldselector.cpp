// bdlaggxxx_fieldselector.cpp                                        -*-C++-*-
#include <bdlaggxxx_fieldselector.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlaggxxx_fieldselector_cpp,"$Id$ $CSID$")

#include <bslim_printer.h>

#include <bsl_cstring.h>
#include <bsl_ostream.h>

namespace BloombergLP {

namespace bdlaggxxx {
                     // ------------------------
                     // class FieldSelector
                     // ------------------------

// ACCESSORS
bsl::ostream& FieldSelector::print(bsl::ostream& stream,
                                        int           level,
                                        int           spacesPerLevel) const
{
    if (stream.bad()) {
        return stream;                                                // RETURN
    }

    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();

    printer.printAttribute("state", (int) d_state);
    if (isName()) {
        printer.printAttribute("name", d_name_p);
    }
    else if (isIndex()) {
        printer.printAttribute("index", d_index);
    }

    printer.end();

    return stream;
}
}  // close package namespace

// FREE OPERATORS
bool bdlaggxxx::operator==(const FieldSelector& lhs, const FieldSelector& rhs)
{
    if (lhs.isEmpty() && rhs.isEmpty()) {
        return true;                                                  // RETURN
    }

    if (lhs.isName() && rhs.isName()) {
        return !bsl::strcmp(lhs.name(), rhs.name());                  // RETURN
    }

    if (lhs.isIndex() && rhs.isIndex()) {
        return lhs.index() == rhs.index();                            // RETURN
    }

    return false;
}

}  // close enterprise namespace

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2012
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ----------------------------------
