// baem_metricdescription.cpp      -*-C++-*-
#include <baem_metricdescription.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(baem_metricdescription_cpp,"$Id$ $CSID$")

#include <baem_category.h>
#include <baem_metricformat.h>

#include <bsl_ostream.h>

namespace BloombergLP {

                   // ----------------------------
                   // class baem_MetricDescription
                   // ----------------------------

// ACCESSORS
bsl::ostream& baem_MetricDescription::print(bsl::ostream& stream) const
{
    stream << d_category_p->name() << "." << d_name_p;
    return stream;
}

bsl::ostream& baem_MetricDescription::printDescription(
                                                    bsl::ostream& stream) const
{
    stream << "[ " << d_category_p->name() << "." << d_name_p << " ";
    bcemt_LockGuard<bcemt_Mutex> guard(&d_mutex);
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

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2009
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
