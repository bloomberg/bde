// btls5_networkdescription.cpp                                       -*-C++-*-
#include <btls5_networkdescription.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(btls5_networkdescription_cpp, "$Id$ $CSID$")

#include <bslim_printer.h>
#include <bsls_assert.h>

#include <bsl_ostream.h>
#include <bsl_sstream.h>

namespace BloombergLP {

namespace btls5 {

                       // ------------------------
                       // class NetworkDescription
                       // ------------------------

// MANIPULATORS
bsl::size_t NetworkDescription::addProxy(bsl::size_t            level,
                                         const btlso::Endpoint& address,
                                         const Credentials&     credentials)
{
    // make sure 'd_proxies' has at least 'level+1' (possibly empty) levels

    if (d_proxies.size() < level + 1) {
        d_proxies.resize(level + 1);
    }
    d_proxies[level].push_back(ProxyDescription(address, credentials));

    return d_proxies[level].size() - 1;
}

void NetworkDescription::setCredentials(bsl::size_t        level,
                                        bsl::size_t        order,
                                        const Credentials& credentials)
{
    BSLS_ASSERT(level < d_proxies.size());
    BSLS_ASSERT(order < d_proxies[level].size());

    d_proxies[level][order].setCredentials(credentials);
}

// ACCESSORS
                                  // Aspects

bsl::ostream& NetworkDescription::print(bsl::ostream& stream,
                                        int           level,
                                        int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    for (bsl::size_t level = 0, endLevel = numLevels();
            level < endLevel;
            level++) {
        bsl::ostringstream label;
        label << "level " << level;
        printer.printAttribute(label.str().c_str(), d_proxies[level]);
    }
    printer.end();

    return stream;
}

}  // close package namespace

// FREE OPERATORS
bsl::ostream& btls5::operator<<(bsl::ostream&             output,
                                const NetworkDescription& object)
{
    output << "[ ";
    for (bsl::size_t level = 0, endLevel = object.numLevels();
            level < endLevel;
            level++) {
        output << "Proxy level " << level << ":";
        for (NetworkDescription::ProxyIterator
                 proxy = object.beginLevel(level),
                 endProxy = object.endLevel(level);
             proxy != endProxy;
             proxy++) {
            output << " " << *proxy;
        }
        if (level + 1  < endLevel) {
            output << " ";
        }
    }
    output << " ]";
    return output;
}

}  // close enterprise namespace

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2015
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
