// btes5_networkdescription.cpp                                       -*-C++-*-
#include <btes5_networkdescription.h>

#include <bslma_default.h>
#include <bsl_ostream.h>
#include <bsls_assert.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(btes5_networkdescription, "$Id$ $CSID$")

namespace BloombergLP {

                        // -----------------------------
                       // class btes5_NetworkDescription
                       // ------------------------------

// CREATORS
btes5_NetworkDescription::btes5_NetworkDescription(bslma::Allocator *allocator)
: d_proxies(bslma::Default::allocator(allocator))
, d_allocator_p(bslma::Default::allocator(allocator))
{
}

btes5_NetworkDescription::btes5_NetworkDescription(
    const btes5_NetworkDescription&  original,
    bslma::Allocator                *allocator)
: d_proxies(original.d_proxies, bslma::Default::allocator(allocator))
, d_allocator_p(bslma::Default::allocator(allocator))
{
}

btes5_NetworkDescription::~btes5_NetworkDescription()
{
}

// MANIPULATORS
btes5_NetworkDescription& btes5_NetworkDescription::operator=(const btes5_NetworkDescription& rhs)
{
    d_proxies = rhs.d_proxies;
    return *this;
}

int btes5_NetworkDescription::addProxy(int level,
             const bteso_Endpoint&         address,
             const btes5_Credentials&  credentials)
    // Add a proxy host with the specified 'address' and 'credentials' t
    // the specified 'level', and return its ordinal number in the 'level'.
{
    BSLS_ASSERT(level >= 0);
    // make sure we have a contiugous vector of proxy levels
    if (d_proxies.size() < bsl::size_t(level + 1)) {
        d_proxies.resize(level + 1);
    }
    d_proxies[level].push_back(btes5_ProxyDescription(address, credentials));
    return d_proxies[level].size();
}

int btes5_NetworkDescription::addProxy(int level, const bteso_Endpoint& address)
{
    return addProxy(level, address, btes5_Credentials());
}

void btes5_NetworkDescription::setCredentials(
    int                          level,
    int                          order,
    const btes5_Credentials& credentials)
{
    BSLS_ASSERT(0 <= level && level < d_proxies.size());
    BSLS_ASSERT(0 <= order && order < d_proxies[level].size());
    d_proxies[level][order].setCredentials(credentials);
}

// ACCESSORS
int btes5_NetworkDescription::levelCount() const {
    return d_proxies.size();
}

btes5_NetworkDescription::ProxyIterator btes5_NetworkDescription::beginLevel(
    int level) const
{
    BSLS_ASSERT(0 <= level && level < d_proxies.size());
    return d_proxies[level].begin();
}

btes5_NetworkDescription::ProxyIterator btes5_NetworkDescription::endLevel(
    int level) const
{
    BSLS_ASSERT(0 <= level && level < d_proxies.size());
    return d_proxies[level].end();
}

// FREE OPERATORS
bool operator==(const btes5_NetworkDescription& lhs,
                const btes5_NetworkDescription& rhs)
{
    return !(lhs != rhs);
}

bool operator!=(const btes5_NetworkDescription& lhs,
                const btes5_NetworkDescription& rhs)
{
    if (lhs.levelCount() != rhs.levelCount()) {
        return true;
    }
    for (int l = 0, endLevel = lhs.levelCount(); l < endLevel; l++) {
        for (btes5_NetworkDescription::ProxyIterator
                lhsProxy = lhs.beginLevel(l),
                lhsEnd = lhs.endLevel(l),
                rhsProxy = rhs.beginLevel(l),
                rhsEnd = rhs.endLevel(l);
             lhsProxy != lhsEnd || rhsProxy != rhsEnd;
             lhsProxy++, rhsProxy++) {
            if (lhsProxy == lhsEnd || rhsProxy == rhsEnd
                    || *lhsProxy != *rhsProxy) {
                return false;
            }
        }
    }
    return false;
}

bsl::ostream& operator<<(bsl::ostream&                 output,
                         const btes5_NetworkDescription& object)
{
    for (int l = 0, endLevel = object.levelCount(); l < endLevel; l++) {
        output << "Proxy level " << l << ":";
        for (btes5_NetworkDescription::ProxyIterator
                proxy = object.beginLevel(l),
                endProxy = object.endLevel(l);
            proxy != endProxy;
            proxy++) {
            output << " " << *proxy;
        }
        output << "\n";
    }
    return output;
}

                     // -----------------------------------
                     // struct btes5_NetworkDescriptionUtil
                     // -----------------------------------
void btes5_NetworkDescriptionUtil::setLevelCredentials(
    btes5_NetworkDescription     *proxyNetwork,
    int                           level,
    const btes5_Credentials&  credentials)
{
    BSLS_ASSERT(0 <= level && level < proxyNetwork->levelCount());
    int order = 0;
    for (btes5_NetworkDescription::ProxyIterator
            proxy = proxyNetwork->beginLevel(level),
            end = proxyNetwork->endLevel(level);
         proxy != end;
         proxy++) {
            proxyNetwork->setCredentials(level, order, credentials);
            order++;
         }
}

void btes5_NetworkDescriptionUtil::setAllCredentials(
    btes5_NetworkDescription      *proxyNetwork,
    const btes5_Credentials&   credentials)
{
    for (int l = 0, end = proxyNetwork->levelCount(); l != end; l++) {
        setLevelCredentials(proxyNetwork, l, credentials);
    }
}

}  // close enterprise namespace

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2013
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
