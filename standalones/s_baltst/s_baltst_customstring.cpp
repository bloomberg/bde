// s_baltst_customstring.cpp         *DO NOT EDIT*         @generated -*-C++-*-

#include <bsls_ident.h>
BSLS_IDENT_RCSID(s_baltst_customstring_cpp, "$Id$ $CSID$")

#include <s_baltst_customstring.h>

#include <bdlat_formattingmode.h>
#include <bdlat_valuetypefunctions.h>
#include <bdlde_utf8util.h>
#include <bdlb_print.h>
#include <bdlb_printmethods.h>
#include <bdlb_string.h>

#include <bsl_string.h>
#include <bslim_printer.h>
#include <bsls_assert.h>

#include <bsl_cstring.h>
#include <bsl_iomanip.h>
#include <bsl_limits.h>
#include <bsl_ostream.h>
#include <bsl_utility.h>

namespace BloombergLP {
namespace s_baltst {

                             // ------------------
                             // class CustomString
                             // ------------------

// PUBLIC CLASS METHODS

int CustomString::checkRestrictions(const bsl::string& value)
{
    const char                *invalid = 0;
    const bsls::Types::IntPtr  valueLength =
        bdlde::Utf8Util::numCodePointsIfValid(
            &invalid, value.data(), value.length());

    // Check whether 'value' is a valid UTF-8 string.
    // bdlde::Utf8Util::numCodePointsIfValid will return
    // a negative value on invalid UTF-8.
    if (0 > valueLength) {
        return -1;                                                    // RETURN
    }

    if (8 < valueLength) {
        return -1;                                                    // RETURN
    }

    return 0;
}

// CONSTANTS

const char CustomString::CLASS_NAME[] = "CustomString";


}  // close package namespace
}  // close enterprise namespace

// GENERATED BY @BLP_BAS_CODEGEN_VERSION@
// USING bas_codegen.pl s_baltst_customstring.xsd --mode msg --includedir . --msgComponent customstring --noRecurse --noExternalization --noHashSupport --noAggregateConversion
// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright 2022 Bloomberg Finance L.P. All rights reserved.
//      Property of Bloomberg Finance L.P. (BFLP)
//      This software is made available solely pursuant to the
//      terms of a BFLP license agreement which governs its use.
// ------------------------------- END-OF-FILE --------------------------------