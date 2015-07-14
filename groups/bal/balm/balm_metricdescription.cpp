// balm_metricdescription.cpp      -*-C++-*-
#include <balm_metricdescription.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(balm_metricdescription_cpp,"$Id$ $CSID$")

#include <balm_category.h>
#include <balm_metricformat.h>

#include <bsl_ostream.h>

namespace BloombergLP {

namespace balm {
                   // ----------------------------
                   // class MetricDescription
                   // ----------------------------

// ACCESSORS
bsl::ostream& MetricDescription::print(bsl::ostream& stream) const
{
    stream << d_category_p->name() << "." << d_name_p;
    return stream;
}

bsl::ostream& MetricDescription::printDescription(
                                                    bsl::ostream& stream) const
{
    stream << "[ " << d_category_p->name() << "." << d_name_p << " ";
    bdlmtt::LockGuard<bdlmtt::Mutex> guard(&d_mutex);
    stream << d_preferredPublicationType;
    if (d_format) {
        stream << " format: " << *d_format;
    }

    stream << " user data: (";
    bsl::vector<const void *>::const_iterator it = d_userData.begin();
    for ( ; it != d_userData.end(); ++it) {
        if (it != d_userData.begin()) {
            stream << ' ';
        }
        stream << *it;
    }
    stream << " ) ]";

    return stream;
}
}  // close package namespace

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2009
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
