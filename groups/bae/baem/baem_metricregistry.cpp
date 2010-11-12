// baem_metricregistry.cpp                                            -*-C++-*-
#include <baem_metricregistry.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(baem_metricregistry_cpp,"$Id$ $CSID$")

#include <baem_metricformat.h>

#include <bcemt_readlockguard.h>
#include <bcemt_rwmutex.h>
#include <bcemt_writelockguard.h>
#include <bdeu_print.h>
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

                          // -------------------------
                          // class baem_MetricRegistry
                          // -------------------------

// PRIVATE MANIPULATORS
bsl::pair<baem_MetricId, bool>
baem_MetricRegistry::insertId(const char *category, const char *name)
{
    // Insert the strings for 'category' and 'name' into the unique strings
    // table 'd_uniqueStrings' (if they are already in the table, this simply
    // looks them up).

    const char *categoryStr = d_uniqueStrings.insert(category).first->c_str();
    const char *nameStr     = d_uniqueStrings.insert(name).first->c_str();
    CategoryAndName id(categoryStr, nameStr);

    MetricRegistry::const_iterator mIt = d_metrics.find(id);
    if (mIt != d_metrics.end()) {
        return bsl::make_pair(baem_MetricId(mIt->second.ptr()), false);
                                                                      // RETURN
    }

    bsl::vector<const void *> userData;
    defaultUserData(&userData, category);

    CategoryRegistry::const_iterator catIt = d_categories.find(categoryStr);
    if (catIt == d_categories.end()) {
        bcema_SharedPtr<baem_Category> categoryPtr(
                       new (*d_allocator_p) baem_Category(categoryStr,
                                                          d_defaultEnabled),
                       d_allocator_p);

        catIt = d_categories.insert(
                               bsl::make_pair(categoryStr, categoryPtr)).first;
    }

    bcema_SharedPtr<baem_MetricDescription> metricPtr(
                     new (*d_allocator_p) baem_MetricDescription(
                                                 catIt->second.ptr(), nameStr),
                     d_allocator_p);

    for (int i = 0; i < userData.size(); ++i) {
        metricPtr->setUserData(i, userData[i]);
    }

    d_metrics.insert(bsl::make_pair(id, metricPtr));
    return bsl::make_pair(baem_MetricId(metricPtr.ptr()), true);
}

void baem_MetricRegistry::setCurrentUserData(
                                 const char                          *category,
                                 baem_MetricDescription::UserDataKey  key,
                                 const void                          *value)
{
    CategoryAndName catAndName(category, "");
    MetricRegistry::iterator it = d_metrics.lower_bound(catAndName);
    while (it != d_metrics.end()
       && 0 == bsl::strcmp(it->first.first, category)) {
        it->second->setUserData(key, value);
        ++it;
    }
}

// PRIVATE ACCESSORS
void baem_MetricRegistry::defaultUserData(
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
baem_MetricRegistry::baem_MetricRegistry(bslma_Allocator *basicAllocator)
: d_uniqueStrings(basicAllocator)
, d_categories(basicAllocator)
, d_metrics(basicAllocator)
, d_defaultEnabled(true)
, d_categoryUserData(basicAllocator)
, d_categoryPrefixUserData(basicAllocator)
, d_nextKey(0)
, d_lock()
, d_allocator_p(bslma_Default::allocator(basicAllocator))
{
}

baem_MetricRegistry::~baem_MetricRegistry()
{
}

// MANIPULATORS
baem_MetricId baem_MetricRegistry::addId(const char *category,
                                         const char *name)
{
    bcemt_WriteLockGuard<bcemt_RWMutex> guard(&d_lock);

    bsl::pair<baem_MetricId, bool> ret = insertId(category, name);
    return ret.second ? ret.first : baem_MetricId();
}

baem_MetricId baem_MetricRegistry::getId(const char *category,
                                         const char *name)
{
    // Lookup the 'category' and 'name' using 'findId', as that uses only a
    // read-lock on 'd_lock'.

    baem_MetricId result = findId(category, name);
    if (result.isValid()) {
        return result;                                                // RETURN
    }

    // Perform the add operation.  However, it is possible that the metric id
    // was added between the return of 'findId' and the acquisition of the
    // write lock on 'd_lock'.

    bcemt_WriteLockGuard<bcemt_RWMutex> guard(&d_lock);

    return insertId(category, name).first;
}

const baem_Category *baem_MetricRegistry::addCategory(const char *category)
{
    bcemt_WriteLockGuard<bcemt_RWMutex> guard(&d_lock);

    // Insert the string for 'category' into the unique strings table
    // 'd_uniqueStrings' (if it is already in the table, this simply looks it
    // up).

    const char *categoryStr = d_uniqueStrings.insert(category).first->c_str();

    CategoryRegistry::const_iterator catIt = d_categories.find(categoryStr);
    if (catIt == d_categories.end()) {
        bcema_SharedPtr<baem_Category> categoryPtr(
                       new (*d_allocator_p) baem_Category(categoryStr,
                                                          d_defaultEnabled),
                       d_allocator_p);

        catIt = d_categories.insert(
                               bsl::make_pair(categoryStr, categoryPtr)).first;
        return catIt->second.ptr();                                   // RETURN
    }
    return 0;
}

const baem_Category *baem_MetricRegistry::getCategory(const char *category)
{
    // Lookup the 'category' using 'findCategory', as that uses only a
    // read-lock on 'd_lock'

    const baem_Category *result = findCategory(category);
    if (0 != result) {
        return result;                                                // RETURN
    }

    // Perform the add operation.  However, it is possible that the category
    // was added between the return of 'findCategory' and the acquisition of
    // the write lock on 'd_lock'.

    bcemt_WriteLockGuard<bcemt_RWMutex> guard(&d_lock);

    // Insert the string for 'category' into the unique strings table
    // 'd_uniqueStrings' (if it is already in the table, this simply looks it
    // up).

    const char *categoryStr = d_uniqueStrings.insert(category).first->c_str();

    CategoryRegistry::const_iterator catIt = d_categories.find(categoryStr);
    if (catIt == d_categories.end()) {
        bcema_SharedPtr<baem_Category> categoryPtr(
                       new (*d_allocator_p) baem_Category(categoryStr,
                                                          d_defaultEnabled),
                       d_allocator_p);

        catIt = d_categories.insert(
                               bsl::make_pair(categoryStr, categoryPtr)).first;
    }
    return catIt->second.ptr();
}

// MANIPULATORS
void baem_MetricRegistry::setCategoryEnabled(const baem_Category* category,
                                             bool                 value)
{
    bcemt_WriteLockGuard<bcemt_RWMutex> guard(&d_lock);

    const_cast<baem_Category *>(category)->setEnabled(value);
}

void baem_MetricRegistry::setAllCategoriesEnabled(bool value)
{
    bcemt_WriteLockGuard<bcemt_RWMutex> guard(&d_lock);

    if (d_defaultEnabled == value) {
        return;                                                       // RETURN
    }

    d_defaultEnabled = value;

    CategoryRegistry::const_iterator it = d_categories.begin();
    for (; it != d_categories.end(); ++it) {
        it->second->setEnabled(value);
    }
}

void baem_MetricRegistry::setPreferredPublicationType(
                                           const baem_MetricId&        metric,
                                           baem_PublicationType::Value type)
{
    baem_MetricDescription *desc =
                 const_cast<baem_MetricDescription *>(metric.description());
    desc->setPreferredPublicationType(type);
}

void baem_MetricRegistry::registerCategoryHolder(const baem_Category *category,
                                                 baem_CategoryHolder *holder)
{
    bcemt_WriteLockGuard<bcemt_RWMutex> guard(&d_lock);

    // We *must* obtain a write lock on 'd_lock' to prevent
    // 'setCategoryEnabled' walking the linked list of category holders while
    // we modify it.  We also *must* test if 'holder->category()' is 0 to
    // prevent two simultaneous calls to this method from registering 'holder'
    // twice and invalidating the linked list.

    if (0 == holder->category()) {
        const_cast<baem_Category *>(category)->registerCategoryHolder(holder);
    }
}

void baem_MetricRegistry::setFormat(const baem_MetricId&     metricId,
                                    const baem_MetricFormat& format)
{
    baem_MetricDescription *desc =
                 const_cast<baem_MetricDescription *>(metricId.description());

    bcemt_WriteLockGuard<bcemt_RWMutex> guard(&d_lock);

    // Note that we need to use a non-'const' pointer to the format in order
    // to initialize it.

    baem_MetricFormat *formatPtr =
                 new (*d_allocator_p) baem_MetricFormat(format, d_allocator_p);

    bcema_SharedPtr<const baem_MetricFormat> formatSPtr(formatPtr,
                                                        d_allocator_p);

    // We must allocate the memory for the format strings from the pool of
    // unique string values.
    for (int i = 0; i < baem_PublicationType::BAEM_LENGTH; ++i) {
        baem_PublicationType::Value  type = (baem_PublicationType::Value)i;
        const baem_MetricFormatSpec *spec = format.formatSpec(type);
        if (0 != spec) {
            const char *fmt =
                         d_uniqueStrings.insert(spec->format()).first->c_str();
            formatPtr->setFormatSpec(
                              type, baem_MetricFormatSpec(spec->scale(), fmt));
        }
    }
    desc->setFormat(formatSPtr);
}

void baem_MetricRegistry::setUserData(
                               const baem_MetricId&                 metricId,
                               baem_MetricDescription::UserDataKey  key,
                               const void                          *value)
{
    BSLS_ASSERT(0 <= key && key < d_nextKey);

    baem_MetricDescription *desc =
                 const_cast<baem_MetricDescription *>(metricId.description());
    desc->setUserData(key, value);
}

void baem_MetricRegistry::setUserData(
                           const char                          *categoryName,
                           baem_MetricDescription::UserDataKey  key,
                           const void                          *value,
                           bool                                 prefixFlag)
{
    BSLS_ASSERT(0 <= key && key < d_nextKey);

    bcemt_WriteLockGuard<bcemt_RWMutex> guard(&d_lock);

    const char *category = d_uniqueStrings.insert(categoryName).first->c_str();
    if (!prefixFlag) {
        bsl::vector<const void *>& userData = d_categoryUserData[category];
        if (userData.size() <= key) {
            userData.resize(key + 1, 0);
        }
        userData[key] = value;

        setCurrentUserData(category, key, value);
    }
    else {
        bsl::vector<const void *>& userData =
                                            d_categoryPrefixUserData[category];
        if (userData.size() <= key) {
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

baem_MetricDescription::UserDataKey baem_MetricRegistry::createUserDataKey()
{
    return d_nextKey++;
}

// ACCESSORS
bsl::size_t baem_MetricRegistry::numMetrics() const
{
    bcemt_ReadLockGuard<bcemt_RWMutex> guard(&d_lock);
    return d_metrics.size();
}

bsl::size_t baem_MetricRegistry::numCategories() const
{
    bcemt_ReadLockGuard<bcemt_RWMutex> guard(&d_lock);
    return d_categories.size();
}

const baem_Category *baem_MetricRegistry::findCategory(
                                                    const char *category) const
{
    bcemt_ReadLockGuard<bcemt_RWMutex> guard(&d_lock);
    CategoryRegistry::const_iterator it = d_categories.find(category);
    return it == d_categories.end() ? 0 : it->second.ptr();
}

baem_MetricId baem_MetricRegistry::findId(const char *category,
                                          const char *name) const
{
    bcemt_ReadLockGuard<bcemt_RWMutex> guard(&d_lock);
    MetricRegistry::const_iterator it =
                                d_metrics.find(bsl::make_pair(category, name));
    return it == d_metrics.end()
           ? baem_MetricId()
           : baem_MetricId(it->second.ptr());
}

void baem_MetricRegistry::getAllCategories(
                         bsl::vector<const baem_Category *> *categories) const
{
    bcemt_ReadLockGuard<bcemt_RWMutex> guard(&d_lock);
    categories->reserve(categories->size() + d_categories.size());
    CategoryRegistry::const_iterator it = d_categories.begin();
    for (; it != d_categories.end(); ++it) {
        categories->push_back(it->second.ptr());
    }
}

bsl::ostream& baem_MetricRegistry::print(bsl::ostream& stream,
                                         int           level,
                                         int           spacesPerLevel) const
{
    bcemt_ReadLockGuard<bcemt_RWMutex> guard(&d_lock);

    char SEP = level <= 0 ? ' ' : '\n';
    bdeu_Print::indent(stream, level, spacesPerLevel);
    stream << "[" << SEP;

    // Note that this implementation assumes that the registry map is sorted
    // by category (i.e., it is not a 'hash_map', and the comparator compares
    // by category before name).

    const baem_Category *lastCategory = 0;
    MetricRegistry::const_iterator it = d_metrics.begin();
    for (; it != d_metrics.end(); ++it) {
        const baem_Category *category = it->second->category();
        BSLS_ASSERT(0 != category);
        if (lastCategory != category) {
            if (0 != lastCategory) {
                // for all but the first category
                bdeu_Print::indent(stream, level + 1, spacesPerLevel);
                stream << "]" << SEP;
            }
            lastCategory = category;
            bdeu_Print::indent(stream, level + 1, spacesPerLevel);
            stream << "[  " << *category << SEP;
        }
        bdeu_Print::indent(stream, level + 2, spacesPerLevel);
        stream << *it->second << SEP;
    }
    if (0 != lastCategory) {
        bdeu_Print::indent(stream, level + 1, spacesPerLevel);
        stream << "]" << SEP;
    }
    bdeu_Print::indent(stream, level, spacesPerLevel);
    stream << "]" << SEP;
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
