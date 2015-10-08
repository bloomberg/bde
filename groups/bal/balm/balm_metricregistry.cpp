// balm_metricregistry.cpp                                            -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <balm_metricregistry.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(balm_metricregistry_cpp,"$Id$ $CSID$")

#include <balm_metricformat.h>

#include <bslmt_readlockguard.h>
#include <bslmt_rwmutex.h>
#include <bslmt_writelockguard.h>
#include <bdlb_print.h>
#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bsls_assert.h>

#include <bsl_cstring.h>
#include <bsl_map.h>
#include <bsl_ostream.h>
#include <bsl_set.h>
#include <bsl_utility.h>

namespace BloombergLP {

namespace {

void combineUserData(bsl::vector<const void *>        *result,
                     const bsl::vector<const void *>&  userData)
    // For each index position in the specified 'userData' containing a
    // non-null value, load that value into the same index position in the
    // specified 'result'.
{
    if (result->size() < userData.size()) {
        result->resize(userData.size());
    }

    bsl::vector<const void *>::iterator       dstIt = result->begin();
    bsl::vector<const void *>::const_iterator srcIt = userData.begin();

    for (; srcIt != userData.end(); ++srcIt, ++dstIt) {
        if (0 != *srcIt) {
            *dstIt = *srcIt;
        }
    }
}

bool isPrefix(const char *candidatePrefix, const char *string)
    // Return 'true' if the specified 'candidatePrefix' is a prefix of the
    // specified 'string', and 'false' otherwise.
{
    while (*candidatePrefix == *string && *candidatePrefix) {
        ++candidatePrefix;
        ++string;
    }
    return 0 == *candidatePrefix;
}

}  // close unnamed namespace

                            // --------------------
                            // class MetricRegistry
                            // --------------------

namespace balm {

// PRIVATE MANIPULATORS
bsl::pair<MetricId, bool>
MetricRegistry::insertId(const char *category, const char *name)
{
    // Insert the strings for 'category' and 'name' into the unique strings
    // table 'd_uniqueStrings' (if they are already in the table, this simply
    // looks them up).

    const char *categoryStr = d_uniqueStrings.insert(category).first->c_str();
    const char *nameStr     = d_uniqueStrings.insert(name).first->c_str();
    CategoryAndName id(categoryStr, nameStr);

    MetricMap::const_iterator mIt = d_metrics.find(id);
    if (mIt != d_metrics.end()) {
        return bsl::make_pair(MetricId(mIt->second.get()), false);    // RETURN
                                                                      // RETURN
    }

    bsl::vector<const void *> userData;
    defaultUserData(&userData, category);

    CategoryRegistry::const_iterator catIt = d_categories.find(categoryStr);
    if (catIt == d_categories.end()) {
        bsl::shared_ptr<Category> categoryPtr(
                       new (*d_allocator_p) Category(categoryStr,
                                                          d_defaultEnabled),
                       d_allocator_p);

        catIt = d_categories.insert(
                               bsl::make_pair(categoryStr, categoryPtr)).first;
    }

    bsl::shared_ptr<MetricDescription> metricPtr(
                     new (*d_allocator_p) MetricDescription(
                             catIt->second.get(),
                             nameStr,
                             d_allocator_p),
                     d_allocator_p);

    for (unsigned int u = 0; u < userData.size(); ++u) {
        metricPtr->setUserData(u, userData[u]);
    }

    d_metrics.insert(bsl::make_pair(id, metricPtr));
    return bsl::make_pair(MetricId(metricPtr.get()), true);
}

void MetricRegistry::setCurrentUserData(
                                      const char                     *category,
                                      MetricDescription::UserDataKey  key,
                                      const void                     *value)
{
    CategoryAndName catAndName(category, "");
    MetricMap::iterator it = d_metrics.lower_bound(catAndName);
    while (it != d_metrics.end()
       && 0 == bsl::strcmp(it->first.first, category)) {
        it->second->setUserData(key, value);
        ++it;
    }
}

// PRIVATE ACCESSORS
void MetricRegistry::defaultUserData(
                                 bsl::vector<const void *> *result,
                                 const char                *categoryName) const
{
    if (d_categoryUserData.empty() && d_categoryPrefixUserData.empty()) {
        return;                                                       // RETURN
    }

    UserDataRegistry::const_iterator it =
                                         d_categoryUserData.find(categoryName);
    if (it != d_categoryUserData.end()) {
        combineUserData(result, it->second);
    }

    // Perform a linear search over 'd_categoryPrefixUserData'.  This is more
    // efficient if 'd_categoryPrefixUserData.size()' is small (expected
    // usage).  For large values of 'd_categoryPrefixUserData.size()', one
    // could perform a series of find operations over all the possible
    // prefixes of 'categoryName'.

    it = d_categoryPrefixUserData.begin();
    for (; it != d_categoryPrefixUserData.end(); ++it) {
        if (isPrefix(it->first, categoryName)) {
            combineUserData(result, it->second);
        }
    }
}

// CREATORS
MetricRegistry::MetricRegistry(bslma::Allocator *basicAllocator)
: d_uniqueStrings(basicAllocator)
, d_categories(basicAllocator)
, d_metrics(basicAllocator)
, d_defaultEnabled(true)
, d_categoryUserData(basicAllocator)
, d_categoryPrefixUserData(basicAllocator)
, d_nextKey(0)
, d_lock()
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

MetricRegistry::~MetricRegistry()
{
}

// MANIPULATORS
MetricId MetricRegistry::addId(const char *category,
                               const char *name)
{
    bslmt::WriteLockGuard<bslmt::RWMutex> guard(&d_lock);

    bsl::pair<MetricId, bool> ret = insertId(category, name);
    return ret.second ? ret.first : MetricId();                       // RETURN
}

MetricId MetricRegistry::getId(const char *category,
                               const char *name)
{
    // Lookup the 'category' and 'name' using 'findId', as that uses only a
    // read-lock on 'd_lock'.

    MetricId result = findId(category, name);
    if (result.isValid()) {
        return result;                                                // RETURN
    }

    // Perform the add operation.  However, it is possible that the metric id
    // was added between the return of 'findId' and the acquisition of the
    // write lock on 'd_lock'.

    bslmt::WriteLockGuard<bslmt::RWMutex> guard(&d_lock);

    return insertId(category, name).first;
}

const Category *MetricRegistry::addCategory(const char *category)
{
    bslmt::WriteLockGuard<bslmt::RWMutex> guard(&d_lock);

    // Insert the string for 'category' into the unique strings table
    // 'd_uniqueStrings' (if it is already in the table, this simply looks it
    // up).

    const char *categoryStr = d_uniqueStrings.insert(category).first->c_str();

    CategoryRegistry::const_iterator catIt = d_categories.find(categoryStr);
    if (catIt == d_categories.end()) {
        bsl::shared_ptr<Category> categoryPtr(
                       new (*d_allocator_p) Category(categoryStr,
                                                          d_defaultEnabled),
                       d_allocator_p);

        catIt = d_categories.insert(
                               bsl::make_pair(categoryStr, categoryPtr)).first;
        return catIt->second.get();                                   // RETURN
    }
    return 0;
}

const Category *MetricRegistry::getCategory(const char *category)
{
    // Lookup the 'category' using 'findCategory', as that uses only a
    // read-lock on 'd_lock'

    const Category *result = findCategory(category);
    if (0 != result) {
        return result;                                                // RETURN
    }

    // Perform the add operation.  However, it is possible that the category
    // was added between the return of 'findCategory' and the acquisition of
    // the write lock on 'd_lock'.

    bslmt::WriteLockGuard<bslmt::RWMutex> guard(&d_lock);

    // Insert the string for 'category' into the unique strings table
    // 'd_uniqueStrings' (if it is already in the table, this simply looks it
    // up).

    const char *categoryStr = d_uniqueStrings.insert(category).first->c_str();

    CategoryRegistry::const_iterator catIt = d_categories.find(categoryStr);
    if (catIt == d_categories.end()) {
        bsl::shared_ptr<Category> categoryPtr(
                       new (*d_allocator_p) Category(categoryStr,
                                                          d_defaultEnabled),
                       d_allocator_p);

        catIt = d_categories.insert(
                               bsl::make_pair(categoryStr, categoryPtr)).first;
    }
    return catIt->second.get();
}

// MANIPULATORS
void MetricRegistry::setCategoryEnabled(const Category* category,
                                        bool            value)
{
    bslmt::WriteLockGuard<bslmt::RWMutex> guard(&d_lock);

    const_cast<Category *>(category)->setEnabled(value);
}

void MetricRegistry::setAllCategoriesEnabled(bool value)
{
    bslmt::WriteLockGuard<bslmt::RWMutex> guard(&d_lock);

    if (d_defaultEnabled == value) {
        return;                                                       // RETURN
    }

    d_defaultEnabled = value;

    CategoryRegistry::const_iterator it = d_categories.begin();
    for (; it != d_categories.end(); ++it) {
        it->second->setEnabled(value);
    }
}

void MetricRegistry::setPreferredPublicationType(const MetricId&        metric,
                                                 PublicationType::Value type)
{
    MetricDescription *desc =
                 const_cast<MetricDescription *>(metric.description());
    desc->setPreferredPublicationType(type);
}

void MetricRegistry::registerCategoryHolder(const Category *category,
                                            CategoryHolder *holder)
{
    bslmt::WriteLockGuard<bslmt::RWMutex> guard(&d_lock);

    // We *must* obtain a write lock on 'd_lock' to prevent
    // 'setCategoryEnabled' walking the linked list of category holders while
    // we modify it.  We also *must* test if 'holder->category()' is 0 to
    // prevent two simultaneous calls to this method from registering 'holder'
    // twice and invalidating the linked list.

    if (0 == holder->category()) {
        const_cast<Category *>(category)->registerCategoryHolder(holder);
    }
}

void MetricRegistry::setFormat(const MetricId&     metricId,
                               const MetricFormat& format)
{
    MetricDescription *desc =
                       const_cast<MetricDescription *>(metricId.description());

    bslmt::WriteLockGuard<bslmt::RWMutex> guard(&d_lock);

    // Note that we need to use a non-'const' pointer to the format in order to
    // initialize it.

    MetricFormat *formatPtr =
                 new (*d_allocator_p) MetricFormat(format, d_allocator_p);

    bsl::shared_ptr<const MetricFormat> formatSPtr(formatPtr,
                                                        d_allocator_p);

    // We must allocate the memory for the format strings from the pool of
    // unique string values.
    for (int i = 0; i < PublicationType::k_LENGTH; ++i) {
        PublicationType::Value  type = (PublicationType::Value)i;
        const MetricFormatSpec *spec = format.formatSpec(type);
        if (0 != spec) {
            const char *fmt =
                         d_uniqueStrings.insert(spec->format()).first->c_str();
            formatPtr->setFormatSpec(
                              type, MetricFormatSpec(spec->scale(), fmt));
        }
    }
    desc->setFormat(formatSPtr);
}

void MetricRegistry::setUserData(const MetricId&                 metricId,
                                 MetricDescription::UserDataKey  key,
                                 const void                     *value)
{
    BSLS_ASSERT(0 <= key && key < d_nextKey);

    MetricDescription *desc =
                 const_cast<MetricDescription *>(metricId.description());
    desc->setUserData(key, value);
}

void MetricRegistry::setUserData(const char                     *categoryName,
                                 MetricDescription::UserDataKey  key,
                                 const void                     *value,
                                 bool                            prefixFlag)
{
    BSLS_ASSERT(0 <= key && key < d_nextKey);

    bslmt::WriteLockGuard<bslmt::RWMutex> guard(&d_lock);

    const char *category = d_uniqueStrings.insert(categoryName).first->c_str();
    if (!prefixFlag) {
        bsl::vector<const void *>& userData = d_categoryUserData[category];
        if (userData.size() <= static_cast<unsigned>(key)) {
            userData.resize(key + 1, 0);
        }
        userData[key] = value;

        setCurrentUserData(category, key, value);
    }
    else {
        bsl::vector<const void *>& userData =
                                            d_categoryPrefixUserData[category];
        if (userData.size() <= static_cast<unsigned>(key)) {
            userData.resize(key + 1, 0);
        }
        userData[key] = value;
        CategoryRegistry::const_iterator it =
                                            d_categories.lower_bound(category);
        while (it != d_categories.end() && isPrefix(category, it->first)) {
            setCurrentUserData(it->first, key, value);
            ++it;
        }
    }
}

MetricDescription::UserDataKey MetricRegistry::createUserDataKey()
{
    return d_nextKey++;
}

// ACCESSORS
bsl::size_t MetricRegistry::numMetrics() const
{
    bslmt::ReadLockGuard<bslmt::RWMutex> guard(&d_lock);
    return d_metrics.size();
}

bsl::size_t MetricRegistry::numCategories() const
{
    bslmt::ReadLockGuard<bslmt::RWMutex> guard(&d_lock);
    return d_categories.size();
}

const Category *MetricRegistry::findCategory(const char *category) const
{
    bslmt::ReadLockGuard<bslmt::RWMutex> guard(&d_lock);
    CategoryRegistry::const_iterator it = d_categories.find(category);
    return it == d_categories.end() ? 0 : it->second.get();
}

MetricId MetricRegistry::findId(const char *category,
                                const char *name) const
{
    bslmt::ReadLockGuard<bslmt::RWMutex> guard(&d_lock);
    MetricMap::const_iterator it =
                                d_metrics.find(bsl::make_pair(category, name));
    return it == d_metrics.end() ? MetricId() : MetricId(it->second.get());
}

void MetricRegistry::getAllCategories(
                               bsl::vector<const Category *> *categories) const
{
    bslmt::ReadLockGuard<bslmt::RWMutex> guard(&d_lock);
    categories->reserve(categories->size() + d_categories.size());
    CategoryRegistry::const_iterator it = d_categories.begin();
    for (; it != d_categories.end(); ++it) {
        categories->push_back(it->second.get());
    }
}

bsl::ostream& MetricRegistry::print(bsl::ostream& stream,
                                    int           level,
                                    int           spacesPerLevel) const
{
    bslmt::ReadLockGuard<bslmt::RWMutex> guard(&d_lock);

    char SEP = level <= 0 ? ' ' : '\n';
    bdlb::Print::indent(stream, level, spacesPerLevel);
    stream << "[" << SEP;

    // Note that this implementation assumes that the registry map is sorted by
    // category (i.e., it is not an 'unordered_map', and the comparator
    // compares by category before name).

    const Category *lastCategory = 0;
    MetricMap::const_iterator it = d_metrics.begin();
    for (; it != d_metrics.end(); ++it) {
        const Category *category = it->second->category();
        BSLS_ASSERT(0 != category);
        if (lastCategory != category) {
            if (0 != lastCategory) {
                // for all but the first category
                bdlb::Print::indent(stream, level + 1, spacesPerLevel);
                stream << "]" << SEP;
            }
            lastCategory = category;
            bdlb::Print::indent(stream, level + 1, spacesPerLevel);
            stream << "[  " << *category << SEP;
        }
        bdlb::Print::indent(stream, level + 2, spacesPerLevel);
        stream << *it->second << SEP;
    }
    if (0 != lastCategory) {
        bdlb::Print::indent(stream, level + 1, spacesPerLevel);
        stream << "]" << SEP;
    }
    bdlb::Print::indent(stream, level, spacesPerLevel);
    stream << "]" << SEP;
    return stream;
}

}  // close package namespace
}  // close enterprise namespace

// ----------------------------------------------------------------------------
// Copyright 2015 Bloomberg Finance L.P.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// ----------------------------- END-OF-FILE ----------------------------------
