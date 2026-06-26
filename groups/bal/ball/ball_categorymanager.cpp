// ball_categorymanager.cpp                                           -*-C++-*-
#include <ball_categorymanager.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ball_categorymanager_cpp,"$Id$ $CSID$")

#include <ball_severity.h>
#include <ball_thresholdaggregate.h>

#include <bslmt_mutexassert.h>

#include <bdlb_bitutil.h>

#include <bslma_default.h>

#include <bslmt_lockguard.h>
#include <bslmt_readlockguard.h>
#include <bslmt_writelockguard.h>

#include <bsls_assert.h>
#include <bsls_atomicoperations.h>
#include <bsls_platform.h>

#include <bsl_algorithm.h>
#include <bsl_cstring.h>  // bsl::strcmp
#include <bsl_map.h>
#include <bsl_utility.h>
#include <bsl_vector.h>

// Note: on Windows -> WinDef.h:#define max(a,b) ...
#if defined(BSLS_PLATFORM_CMP_MSVC) && defined(max)
#undef max
#endif

// ============================================================================
//                           IMPLEMENTATION NOTES
// ----------------------------------------------------------------------------
//
///Category Registry Capacity
///--------------------------
// The category registry capacity is managed by the 'setMaxNumCategories' and
// 'maxNumCategories' methods, and the following data member:
//..
//  unsigned int ball::CategoryManager::d_maxNumCategoriesMinusOne;
//..
// From the client's perspective, valid capacity values are in the range
// '[0 .. INT_MAX]'.  A value of 0 implies that no limit is imposed.  Capacity
// values stored internally in 'd_maxNumCategoriesMinusOne' are in the range
// '[-1 .. INT_MAX - 1]' (i.e., stored in an 'unsigned int' variable).  Note
// that 'setMaxNumCategories' and 'maxNumCategories' compensate by subtracting
// 1 from 'd_maxNumCategoriesMinusOne' and adding 1 to
// 'd_maxNumCategoriesMinusOne', respectively.  This trick allows for more
// efficient capacity testing, e.g.:
//..
//  if (d_maxNumCategoriesMinusOne >= d_categoryManager.length()) {
//      // there is sufficient capacity to add a new category
//      // ...
//  }
//..
// as compared to something like the following if the registry capacity were
// stored in a hypothetical 'int' data member
// 'ball::LoggerManager::d_maxNumCategories' not biased by 1:
//..
//  if (0 == d_maxNumCategories
//   || d_maxNumCategories > d_categoryManager.length()) {
//      // there is sufficient capacity to add a new category
//      // ...
//  }
//..
// ----------------------------------------------------------------------------

namespace BloombergLP {
namespace ball {

namespace {

// LOCAL CONSTANTS
static const char *const k_DEFAULT_CATEGORY_NAME = "";

typedef bsls::AtomicOperations AtomicOps;

/// This object is used for assigning a unique initial rule set sequence
/// number to each category manager that is created.
static
AtomicOps::AtomicTypes::Int64 categoryManagerSequenceNumber = { -1 };

                         // =========================
                         // class UpdateLevelsByIndex
                         // =========================

/// Functor to update the threshold levels of categories that are identified by
/// their index into `CategoryManager::d_categories`.
class UpdateLevelsByIndex {

    // PRIVATE TYPES
    typedef bsl::vector<Category *> CategoryPtrVector;

    // DATA
    const CategoryPtrVector *d_categories_p;
    int                      d_recordLevel;
    int                      d_passLevel;
    int                      d_triggerLevel;
    int                      d_triggerAllLevel;

  public:
    // CREATORS
    UpdateLevelsByIndex(const CategoryPtrVector *categories,
                        int recordLevel,
                        int passLevel,
                        int triggerLevel,
                        int triggerAllLevel)
    : d_categories_p(categories)
    , d_recordLevel(recordLevel)
    , d_passLevel(passLevel)
    , d_triggerLevel(triggerLevel)
    , d_triggerAllLevel(triggerAllLevel)
    {}

    // ACCESSORS
    void operator()(const bsl::string_view& /*key*/, int index) const
    {
        Category *const catPtr = (*d_categories_p)[index];

        catPtr->setLevels(d_recordLevel,
                          d_passLevel,
                          d_triggerLevel,
                          d_triggerAllLevel);
    }
};

                          // =====================
                          // class CategoryProctor
                          // =====================

/// This class facilitates exception neutrality by proctoring memory
/// management for `Category` objects.
class CategoryProctor {

    // PRIVATE TYPES
    typedef bsl::vector<ball::Category *>  CategoryVector;
    typedef CategoryVector::iterator       CategoryVectorIter;

    typedef CategoryManager_RadixTree<int> Prefixer;

    // DATA
    Category         *d_category_p;    // category object to delete on failure
    bslma::Allocator *d_allocator_p;   // allocator for the category object

    CategoryVector   *d_categories_p;  // category collection to rollback on
                                       // failure

    Prefixer         *d_prefixer_p;    // Prefix tree to roll back on failure

  private:
    // NOT IMPLEMENTED
    CategoryProctor(const CategoryProctor&);
    CategoryProctor& operator=(const CategoryProctor&);

  public:
    // CREATORS

    /// Create a proctor to manage the specified `category` object,
    /// allocated with the specified `allocator`.  On this proctor's
    /// destruction, unless release has been called, the `category` will be
    /// destroyed and its footprint deallocated.
    CategoryProctor(Category *category, bslma::Allocator *allocator);

    /// Rollback the owned objects to their initial state on failure.
    ~CategoryProctor();

    // MANIPULATORS

    // BDE_VERIFY pragma: push
    // BDE_VERIFY pragma: -FABC01: Functions not in alphanumeric order

    /// Take ownership of the specified `categories` object to roll it back
    /// on failure.
    void setCategories(CategoryVector *categories);

    /// Take ownership of the specified `prefixer` object to roll it back on
    /// failure by removing the elements with the category name of the
    /// protected category as prefix.
    void setPrefixer(CategoryManager_RadixTree<int> *prefixer);

    /// Release the ownership of all objects currently managed by this
    /// proctor.
    void release();

    // BDE_VERIFY pragma: pop
};

                          // ---------------------
                          // class CategoryProctor
                          // ---------------------

// CREATORS
inline
CategoryProctor::CategoryProctor(Category         *category,
                                 bslma::Allocator *allocator)
: d_category_p(category)
, d_allocator_p(allocator)
, d_categories_p(0)
, d_prefixer_p(0)
{
    BSLS_ASSERT(category);
    BSLS_ASSERT(allocator);
}

inline
CategoryProctor::~CategoryProctor()
{
    if (d_categories_p) {
        d_categories_p->pop_back();
    }

    if (d_category_p) {
        if (d_prefixer_p) {
            d_prefixer_p->erase(d_category_p->categoryName());
        }

        d_category_p->~Category();
        d_allocator_p->deallocate(d_category_p);
    }
}

// MANIPULATORS

// BDE_VERIFY pragma: push
// BDE_VERIFY pragma: -FABC01: Functions not in alphanumeric order

inline
void CategoryProctor::setCategories(CategoryVector *categories)
{
    BSLS_ASSERT(categories);

    d_categories_p = categories;
}

inline
void CategoryProctor::setPrefixer(CategoryManager_RadixTree<int> *prefixer)
{
    BSLS_ASSERT(prefixer);

    d_prefixer_p = prefixer;
}

inline
void CategoryProctor::release()
{
    d_category_p   = 0;
    d_categories_p = 0;
    d_prefixer_p   = 0;
}

// BDE_VERIFY pragma: pop

}  // close unnamed namespace

                          // ---------------------
                          // class CategoryManager
                          // ---------------------

// PRIVATE MANIPULATORS
Category *CategoryManager::addFilteredCategory(
                                          CategoryHolder *categoryHolder,
                                          const char     *filteredCategoryName,
                                          int             recordLevel,
                                          int             passLevel,
                                          int             triggerLevel,
                                          int             triggerAllLevel)
{
    BSLS_ASSERT(filteredCategoryName);

    if (!Category::areValidThresholdLevels(recordLevel,
                                           passLevel,
                                           triggerLevel,
                                           triggerAllLevel)) {
        // Invalid threshold levels
        return 0;                                                     // RETURN
    }

    bslmt::WriteLockGuard<bslmt::ReaderWriterLock> registryGuard(
                                                              &d_registryLock);

    if (d_maxNumCategoriesMinusOne < d_categories.size()) {
        // Too many categories
        return 0;                                                     // RETURN
    }

    const CategoryMap::const_iterator iter =
                                         d_registry.find(filteredCategoryName);
    if (iter != d_registry.end()) {
        // Category already exists
        return 0;                                                     // RETURN
    }

    Category * const category = addNewCategory(filteredCategoryName,
                                               recordLevel,
                                               passLevel,
                                               triggerLevel,
                                               triggerAllLevel);

    if (categoryHolder) {
        CategoryManagerImpUtil::linkCategoryHolder(category, categoryHolder);
    }

    registryGuard.release()->unlock();

    const bslmt::LockGuard<bslmt::Mutex> ruleSetGuard(&d_ruleSetMutex);

    privateApplyRulesToCategory(category);

    if (categoryHolder) {
        categoryHolder->setThreshold(bsl::max(category->threshold(),
                                              category->ruleThreshold()));
    }

    return category;
}

Category *CategoryManager::addNewCategory(const char *filteredCategoryName,
                                          int         recordLevel,
                                          int         passLevel,
                                          int         triggerLevel,
                                          int         triggerAllLevel)
{
    // Create a new category and add it to the category registry.

    Category *category = new (*d_allocator_p) Category(filteredCategoryName,
                                                       recordLevel,
                                                       passLevel,
                                                       triggerLevel,
                                                       triggerAllLevel,
                                                       d_allocator_p);

    CategoryProctor proctor(category, d_allocator_p);  // rollback on exception

    d_categories.push_back(category);
    proctor.setCategories(&d_categories);

    d_categoryPrefixer.emplace(category->categoryName(),
                               static_cast<int>(d_categories.size() - 1));
    proctor.setPrefixer(&d_categoryPrefixer);

    d_registry[category->categoryName()] =
                                     static_cast<int>(d_categories.size() - 1);
    proctor.release();

    // We need to remove any "shadow" setting (this cannot fail in a
    // recoverable manner so no need for the proxy-protection).
    d_orphanHierarchicalSettings.erasePrefix(category->categoryName());

    return category;
}

Category *CategoryManager::lookupFilteredCategory(
                                              const char *filteredCategoryName)
{
    const CategoryManager *thisConst = this;
    return const_cast<Category *>(
                      thisConst->lookupFilteredCategory(filteredCategoryName));
}

Category *CategoryManager::lookupFilteredCategory(
                                          CategoryHolder *categoryHolder,
                                          const char     *filteredCategoryName)
{
    // If we have a category holder, we may need to modify (link it),
    // so use write lock from the start to avoid upgrade contention.
    if (categoryHolder) {
        const bslmt::WriteLockGuard<bslmt::ReaderWriterLock> registryGuard(
                                                              &d_registryLock);

        const CategoryMap::const_iterator iter =
                                         d_registry.find(filteredCategoryName);
        if (iter != d_registry.end()) {
            Category *category = d_categories[iter->second];
            // Link holder if needed
            if (!categoryHolder->category()) {
                CategoryManagerImpUtil::linkCategoryHolder(category,
                                                           categoryHolder);
            }
            return category;                                          // RETURN
        }
    }
    else {
        // No holder - read lock is sufficient
        const bslmt::ReadLockGuard<bslmt::ReaderWriterLock> registryGuard(
                                                              &d_registryLock);

        const CategoryMap::const_iterator iter =
                                         d_registry.find(filteredCategoryName);
        if (iter != d_registry.end()) {
            return d_categories[iter->second];                        // RETURN
        }
    }

    return 0;
}

void CategoryManager::privateApplyRulesToCategory(Category* category)
{
    RuleSet::MaskType mask      = 0;
    int               threshold = 0;
    for (int i = 0; i < RuleSet::maxNumRules(); ++i) {
        const Rule *rule = d_ruleSet.getRuleById(i);
        if (rule && rule->isMatch(category->categoryName())) {
            mask = bdlb::BitUtil::withBitSet(mask, i);
            threshold = bsl::max(threshold,
                                 ThresholdAggregate::maxLevel(
                                         rule->recordLevel(),
                                         rule->passLevel(),
                                         rule->triggerLevel(),
                                         rule->triggerAllLevel()));
        }
    }
    CategoryManagerImpUtil::setRelevantRuleMask(category, mask);
    if (threshold != category->ruleThreshold()) {
        CategoryManagerImpUtil::setRuleThreshold(category, threshold);
        CategoryManagerImpUtil::updateThresholdForHolders(category);
    }
}

void CategoryManager::privateApplyRulesToAllCategories(
                                     bslmt::LockGuard<bslmt::Mutex>& ruleGuard)
{
    typedef bsls::Types::Int64 Int64;
    BSLMT_MUTEXASSERT_IS_LOCKED(&d_ruleSetMutex);
    BSLS_ASSERT(ruleGuard.ptr() == &d_ruleSetMutex);
    const bsls::Types::Int64 incrementedRuleSetSeqNum =
                                                     ++d_ruleSetSequenceNumber;
    ruleGuard.release()->unlock();

    const Int64 k_BATCH_SIZE = 4096;
    Int64       offset       = 0;
    Int64       batch;
    bool        done         = false;

    CategoryPtrVector cachedCategories(d_allocator_p);
    while (!done) {
        cachedCategories.clear();
        {
            const bslmt::ReadLockGuard<bslmt::ReaderWriterLock> guard(
                                                              &d_registryLock);
            const Int64 remaining = static_cast<Int64>(d_categories.size() - offset);
            batch = std::min(k_BATCH_SIZE, remaining);
            done = (remaining == batch);
            cachedCategories.assign(d_categories.begin() + offset,
                                    d_categories.begin() + offset + batch);
            offset += batch;
        }
        {
            const bslmt::LockGuard<bslmt::Mutex> guard(&d_ruleSetMutex);
            if (incrementedRuleSetSeqNum != d_ruleSetSequenceNumber) {
                // Another thread is applying newer rules.
                return;                                               // RETURN
            }
            for (CategoryPtrVector::iterator iter =
                                               cachedCategories.begin();
                                               iter != cachedCategories.end();
                                               ++iter) {
                privateApplyRulesToCategory(*iter);
            }
        }
    }
}

// PRIVATE ACCESSORS
const char *CategoryManager::filterCategoryName(
                                               bsl::string *filteredNameBuffer,
                                               const char  *originalName) const
{
    BSLS_ASSERT(filteredNameBuffer);
    BSLS_ASSERT(originalName);

    // afeher: If we add the (missing) setter for the name filter callback we
    // may need to add a mutex and a lock!  For now the callback is set at
    // construction only so no lock is needed.  Adding a setter would be a
    // monumental change though because the registry would need to be
    // "translated" between the old and new filter and we do not have a source
    // of truth for the original names.

    if (!d_categoryNameFilterCallback) {
        return originalName;                                          // RETURN
    }
    else {
        filteredNameBuffer->clear();
        d_categoryNameFilterCallback(filteredNameBuffer, originalName);
        filteredNameBuffer->push_back(0);  // append the null 'char'
        return filteredNameBuffer->c_str();                           // RETURN
    }
}

const Category *CategoryManager::lookupFilteredCategory(
                                        const char *filteredCategoryName) const
{
    const bslmt::ReadLockGuard<bslmt::ReaderWriterLock> registryGuard(
                                                              &d_registryLock);

    const CategoryMap::const_iterator iter =
                                         d_registry.find(filteredCategoryName);
    return iter != d_registry.end() ? d_categories[iter->second] : 0;
}


// CREATORS
CategoryManager::CategoryManager(bslma::Allocator *basicAllocator)
: d_defaultCategory_p(0)
, d_defaultThresholdsCallback(bsl::allocator_arg, basicAllocator)
, d_defaultThresholdLevels(ThresholdDefaults::k_RECORD_LEVEL,
                           ThresholdDefaults::k_PASS_LEVEL,
                           ThresholdDefaults::k_TRIGGER_LEVEL,
                           ThresholdDefaults::k_TRIGGER_ALL_LEVEL)
, d_factoryThresholdLevels(ThresholdDefaults::k_RECORD_LEVEL,
                           ThresholdDefaults::k_PASS_LEVEL,
                           ThresholdDefaults::k_TRIGGER_LEVEL,
                           ThresholdDefaults::k_TRIGGER_ALL_LEVEL)
, d_categoryNameFilterCallback(bsl::allocator_arg, basicAllocator)
, d_maxNumCategoriesMinusOne((unsigned int)-1)
, d_registry(1, bdlb::CStringHash(), bdlb::CStringEqualTo(), basicAllocator)
, d_ruleSetSequenceNumber(
      AtomicOps::incrementInt64Nv(&categoryManagerSequenceNumber) << 48)
, d_ruleSet(bslma::Default::allocator(basicAllocator))
, d_categories(basicAllocator)
, d_categoryPrefixer(basicAllocator)
, d_orphanHierarchicalSettings(basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

CategoryManager::CategoryManager(
         int                                    defaultRecordLevel,
         int                                    defaultPassLevel,
         int                                    defaultTriggerLevel,
         int                                    defaultTriggerAllLevel,
         const DefaultThresholdLevelsCallback&  defaultThresholdLevelsCallback,
         const CategoryNameFilterCallback&      categoryNameFilterCallback,
         bslma::Allocator                      *basicAllocator)
: d_defaultCategory_p(0)
, d_defaultThresholdsCallback(bsl::allocator_arg,
                              basicAllocator,
                              defaultThresholdLevelsCallback)
, d_defaultThresholdLevels(defaultRecordLevel,
                           defaultPassLevel,
                           defaultTriggerLevel,
                           defaultTriggerAllLevel)
, d_factoryThresholdLevels(defaultRecordLevel,
                           defaultPassLevel,
                           defaultTriggerLevel,
                           defaultTriggerAllLevel)
, d_categoryNameFilterCallback(bsl::allocator_arg,
                               basicAllocator,
                               categoryNameFilterCallback)
, d_maxNumCategoriesMinusOne((unsigned int)-1)
, d_registry(1, bdlb::CStringHash(), bdlb::CStringEqualTo(), basicAllocator)
, d_ruleSetSequenceNumber(
             AtomicOps::incrementInt64Nv(&categoryManagerSequenceNumber) << 48)
, d_ruleSet(bslma::Default::allocator(basicAllocator))
, d_categories(basicAllocator)
, d_categoryPrefixer(basicAllocator)
, d_orphanHierarchicalSettings(basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

CategoryManager::~CategoryManager()
{
    BSLS_ASSERT(d_allocator_p);
    BSLS_ASSERT(-1 <= (int)d_maxNumCategoriesMinusOne);

    for (int i = 0; i < length(); ++i) {
        d_categories[i]->~Category();
        d_allocator_p->deallocate(d_categories[i]);
    }
}

// MANIPULATORS

// BDE_VERIFY pragma: push
// BDE_VERIFY pragma: -FABC01: Functions not in alphanumeric order

Category *CategoryManager::addCategory(CategoryHolder *categoryHolder,
                                       const char     *categoryName,
                                       int             recordLevel,
                                       int             passLevel,
                                       int             triggerLevel,
                                       int             triggerAllLevel)
{
    BSLS_ASSERT(categoryName);

    if (!Category::areValidThresholdLevels(recordLevel,
                                           passLevel,
                                           triggerLevel,
                                           triggerAllLevel)) {
        return 0;                                                     // RETURN
    }

    bsl::string  filteredName;
    const char  *localCategoryName = filterCategoryName(&filteredName,
                                                        categoryName);
    return addFilteredCategory(categoryHolder,
                               localCategoryName,
                               recordLevel,
                               passLevel,
                               triggerLevel,
                               triggerAllLevel);
}

Category *CategoryManager::addCategoryHierarchically(const char *categoryName)
{
    BSLS_ASSERT(categoryName);

    bsl::string  filteredName;
    const char  *localCategoryName = filterCategoryName(&filteredName,
                                                        categoryName);

    d_registryLock.lockReadReserveWrite();
    bslmt::WriteLockGuard<bslmt::ReaderWriterLock> registryGuard(
                                                               &d_registryLock,
                                                               1);
    {
        const CategoryMap::const_iterator iter =
                                            d_registry.find(localCategoryName);
        if (iter != d_registry.end()) {
            // Category already exists
            return 0;                                                 // RETURN
        }
    }

    if (d_maxNumCategoriesMinusOne < d_categories.size()) {
        // Too many categories
        return 0;                                                     // RETURN
    }

    const bsl::string_view catNameSv(localCategoryName);

    // Find the longest category name that is a prefix of the new category
    // name.

        CategoryManager_RadixTree<int>::OptValueRef longestCatMatchIndexRef;
        const bsl::string_view longestMatchingCatName =
            d_categoryPrefixer.findLongestCommonPrefix(
                                                      &longestCatMatchIndexRef,
                                                      catNameSv);
        // Empty category matches only the empty category
        const bool hasLongestCatMatch = (longestCatMatchIndexRef.has_value() &&
                         (!longestMatchingCatName.empty() ||
                          catNameSv.empty()));

    // Find the longest shadow setting prefix that is a prefix of the new
    // category name.

    CategoryManager_RadixTree<HierarchicalCategorySetting>::OptValueRef
                                                   longestSettingMatchIndexRef;
    const bsl::string_view longestMatchingSettingPrefix =
                d_orphanHierarchicalSettings.findLongestCommonPrefix(
                                                  &longestSettingMatchIndexRef,
                                                  catNameSv);
    // We never store the empty category in settings, so we don't need to
    // worry about matching it.  (If an empty category setting is performed we
    // just update all existing categories to those threshold levels and clear
    // `d_orphanHierarchicalSettings`.  In other words, we never have an empty
    // category setting "orphan" in `d_orphanHierarchicalSettings` essentially
    // overwrite the default levels for all hierarchical categories.)
    const bool hasLongestSettingMatch =
                                       longestSettingMatchIndexRef.has_value();

    // The bunch of conditional branches below determines the threshold levels
    // by filling out the following variables.  All branches set all four
    // values so there is no need for initialization.
    int recordLevel;
    int passLevel;
    int triggerLevel;
    int triggerAllLevel;

    if (!hasLongestCatMatch && !hasLongestSettingMatch) {
        // We need the default threshold values because we found no category
        // name that is a prefix and no such setting either.

        // The default may be overridden by a callback
        bslmt::ReadLockGuard<bslmt::ReaderWriterMutex> gcb(
                                            &d_defaultThresholdsCallbackMutex);
        if (d_defaultThresholdsCallback) {
            d_defaultThresholdsCallback(&recordLevel,
                                        &passLevel,
                                        &triggerLevel,
                                        &triggerAllLevel,
                                        localCategoryName);
        }
        else {
            // No need to lock the callback anymore (we won't call it)
            gcb.release()->unlock();

            recordLevel     = d_defaultThresholdLevels.recordLevel();
            passLevel       = d_defaultThresholdLevels.passLevel();
            triggerLevel    = d_defaultThresholdLevels.triggerLevel();
            triggerAllLevel = d_defaultThresholdLevels.triggerAllLevel();
        }
    }
    else if (!hasLongestCatMatch ||
             (hasLongestSettingMatch &&
              longestMatchingCatName.length() <
                                      longestMatchingSettingPrefix.length())) {
        // If there is no category match or there is also a settings match
        // whose name is longer than the category match we use the settings'
        // thresholds.  Since we have covered the "neither matched" case
        // earlier, and we are in an `else` of that, we do not have to check
        // `true == hasLongestSettingMatch` when `false == hasLongestCatMatch`
        // because it is guaranteed to be so.
        recordLevel     = longestSettingMatchIndexRef->get().recordLevel();
        passLevel       = longestSettingMatchIndexRef->get().passLevel();
        triggerLevel    = longestSettingMatchIndexRef->get().triggerLevel();
        triggerAllLevel = longestSettingMatchIndexRef->get().triggerAllLevel();
    }
    else {
        // If there was only a category match, or the settings match was weaker
        // (shorter) we set the values from the found category match.
        recordLevel  = d_categories[*longestCatMatchIndexRef]->recordLevel();
        passLevel    = d_categories[*longestCatMatchIndexRef]->passLevel();
        triggerLevel = d_categories[*longestCatMatchIndexRef]->triggerLevel();
        triggerAllLevel =
                     d_categories[*longestCatMatchIndexRef]->triggerAllLevel();
    }

    // We have determined the threshold levels to use.  Now we can add the new
    // category.
    d_registryLock.upgradeToWriteLock();

    // Recheck if the category was added by another thread while we were
    // waiting for the write lock.
    {
        const CategoryMap::const_iterator iter =
                                            d_registry.find(localCategoryName);
        if (iter != d_registry.end()) {
            // Category was added by another thread
            return 0;                                                 // RETURN
        }
    }

    Category *category = addNewCategory(localCategoryName,
                                        recordLevel,
                                        passLevel,
                                        triggerLevel,
                                        triggerAllLevel);
    // Not touching the registry anymore, so we can release the lock.
    registryGuard.release()->unlock();

    const bslmt::LockGuard<bslmt::Mutex> ruleSetGuard(&d_ruleSetMutex);
    privateApplyRulesToCategory(category);

    return category;
}

void CategoryManager::addDefaultCategory()
{
    // The default category is added on demand only to keep the test driver
    // simpler (it does not have to account for the default category unless it
    // is explicitly added).
    //
    // The default category is also special as we store a direct pointer to it.
    // That also means that adding the default category (empty name) by the
    // normal path results in (library) undefined behavior.

    bslmt::WriteLockGuard<bslmt::ReaderWriterMutex> defGuard(
                                              &d_defaultThresholdsLevelsMutex);
    if (d_defaultCategory_p) {
        return;                                                       // RETURN
    }

    const ThresholdAggregate defaultLevels = d_defaultThresholdLevels;
    defGuard.release()->unlock();

    d_defaultCategory_p = addCategory(k_DEFAULT_CATEGORY_NAME,
                                      defaultLevels.recordLevel(),
                                      defaultLevels.passLevel(),
                                      defaultLevels.triggerLevel(),
                                      defaultLevels.triggerAllLevel());
}

Category *CategoryManager::lookupCategory(const char *categoryName)
{
    // Notice that we do not lock here.  That is OK because the called methods
    // lock mutexes as needed.  And don't worry that `filterCategoryName` does
    // not lock, the callback is set only during construction time and never
    // changed afterwards.
    bsl::string filteredName;
    return lookupFilteredCategory(filterCategoryName(&filteredName,
                                                     categoryName));
}

Category *CategoryManager::lookupCategory(CategoryHolder *categoryHolder,
                                          const char     *categoryName)
{
    // Notice that we do not lock here.  That is OK because the called methods
    // lock mutexes as needed.  And don't worry that `filterCategoryName` does
    // not lock, the callback is set only during construction time and never
    // changed afterwards.
    bsl::string filteredName;
    return lookupFilteredCategory(categoryHolder,
                                  filterCategoryName(&filteredName,
                                                     categoryName));
}

void CategoryManager::resetCategoryHolders()
{
    // Intentionally not locking.  This method should only be called just prior
    // to destroying the category manager.

    const int numCategories = length();
    for (int i = 0; i < numCategories; ++i) {
        CategoryManagerImpUtil::resetCategoryHolders(d_categories[i]);
    }
}

void CategoryManager::resetDefaultThresholdLevels()
{
    const bslmt::WriteLockGuard<bslmt::ReaderWriterMutex> guard(
                                              &d_defaultThresholdsLevelsMutex);

    d_defaultThresholdLevels = d_factoryThresholdLevels;
}

const Category *CategoryManager::setCategory(CategoryHolder *categoryHolder,
                                             const char     *categoryName)
{
    BSLS_ASSERT(categoryName);

    bsl::string  filteredName;
    const char  *localCategoryName = filterCategoryName(&filteredName,
                                                        categoryName);

    const Category *category = lookupFilteredCategory(categoryHolder,
                                                localCategoryName);
    if (!category && d_maxNumCategoriesMinusOne >= (unsigned int)length()) {
        // If there is no such category, we add it with default levels.

        int recordLevel, passLevel, triggerLevel, triggerAllLevel;
        {
            bslmt::ReadLockGuard<bslmt::ReaderWriterMutex> cbGuard(
                                            &d_defaultThresholdsCallbackMutex);
            if (d_defaultThresholdsCallback) {
                d_defaultThresholdsCallback(&recordLevel,
                                            &passLevel,
                                            &triggerLevel,
                                            &triggerAllLevel,
                                            localCategoryName);
            }
            else {
                cbGuard.release()->unlock();  // Won't touch the callback again

                bslmt::ReadLockGuard<bslmt::ReaderWriterMutex> g(
                                              &d_defaultThresholdsLevelsMutex);
                const ThresholdAggregate defaults = d_defaultThresholdLevels;
                g.release()->unlock();  // Copied the settings no need to lock

                recordLevel     = defaults.recordLevel();
                passLevel       = defaults.passLevel();
                triggerLevel    = defaults.triggerLevel();
                triggerAllLevel = defaults.triggerAllLevel();
            }
        }

        // Here all locks must be unlocked!

        category = addFilteredCategory(categoryHolder,
                                       localCategoryName,
                                       recordLevel,
                                       passLevel,
                                       triggerLevel,
                                       triggerAllLevel);

        if (!category) {
            // `addFilteredCategory` failed, possibly because another thread
            // added the category in the meantime.  Try to look it up again.
            category = lookupFilteredCategory(categoryHolder,
                                              localCategoryName);
        }
    }

    // We have either added a category, found an existing one, or return the
    // default category if the registry is full.
    return category ? category
                    : lookupFilteredCategory(categoryHolder,
                                             k_DEFAULT_CATEGORY_NAME);
}

Category *CategoryManager::setCategory(const char *categoryName,
                                       int         recordLevel,
                                       int         passLevel,
                                       int         triggerLevel,
                                       int         triggerAllLevel)
{
    BSLS_ASSERT(categoryName);

    if (!Category::areValidThresholdLevels(recordLevel,
                                           passLevel,
                                           triggerLevel,
                                           triggerAllLevel)) {
        return 0;                                                     // RETURN
    }

    bsl::string filteredName;
    const char  *localCategoryName = filterCategoryName(&filteredName,
                                                        categoryName);

    Category *category = lookupFilteredCategory(localCategoryName);

    if (category) {
        // Found it, set the levels
        category->setLevels(recordLevel,
                            passLevel,
                            triggerLevel,
                            triggerAllLevel);
        return category;                                              // RETURN
    }

    if (d_maxNumCategoriesMinusOne < (unsigned int)length()) {
        // Too many categories
        return 0;                                                     // RETURN
    }

    category = addFilteredCategory(0,
                                   localCategoryName,
                                   recordLevel,
                                   passLevel,
                                   triggerLevel,
                                   triggerAllLevel);

    // `addFilteredCategory` might have failed because another thread added
    // this very category while we were unlocked, so we try to find it again:
    return category ? category : lookupFilteredCategory(localCategoryName);
}

int CategoryManager::setDefaultThresholdLevels(int recordLevel,
                                               int passLevel,
                                               int triggerLevel,
                                               int triggerAllLevel)
{
    const int k_SUCCESS =  0;
    const int k_FAILURE = -1;

    if (!Category::areValidThresholdLevels(recordLevel,
                                           passLevel,
                                           triggerLevel,
                                           triggerAllLevel)) {
        return k_FAILURE;                                             // RETURN
    }

    {
        const bslmt::WriteLockGuard<bslmt::ReaderWriterMutex> g(
                                              &d_defaultThresholdsLevelsMutex);
        d_defaultThresholdLevels.setLevels(recordLevel,
                                           passLevel,
                                           triggerLevel,
                                           triggerAllLevel);
    }

    return k_SUCCESS;
}

Category *CategoryManager::setThresholdLevels(const char *categoryName,
                                              int         recordLevel,
                                              int         passLevel,
                                              int         triggerLevel,
                                              int         triggerAllLevel)
{
    BSLS_ASSERT(categoryName);

    if (!Category::areValidThresholdLevels(recordLevel,
                                           passLevel,
                                           triggerLevel,
                                           triggerAllLevel)) {
        return 0;                                                     // RETURN
    }

    bsl::string  filteredName;
    const char  *localCategoryName = filterCategoryName(&filteredName,
                                                        categoryName);

    d_registryLock.lockReadReserveWrite();
    bslmt::WriteLockGuard<bslmt::ReaderWriterLock> registryGuard(
                                                               &d_registryLock,
                                                               1);

    const CategoryMap::iterator iter = d_registry.find(localCategoryName);
    if (iter != d_registry.end()) {
        Category *category = d_categories[iter->second];
        category->setLevels(recordLevel,
                            passLevel,
                            triggerLevel,
                            triggerAllLevel);

        return category;                                              // RETURN
    }
    else {
        d_registryLock.upgradeToWriteLock();

        Category *category = addNewCategory(localCategoryName,
                                            recordLevel,
                                            passLevel,
                                            triggerLevel,
                                            triggerAllLevel);
        registryGuard.release();
        d_registryLock.unlock();

        const bslmt::LockGuard<bslmt::Mutex> ruleSetGuard(&d_ruleSetMutex);
        privateApplyRulesToCategory(category);

        return category;                                              // RETURN
    }
}

void CategoryManager::setDefaultThresholdLevelsCallback(
          const DefaultThresholdLevelsCallback *defaultThresholdLevelsCallback)
{
    const bslmt::WriteLockGuard<bslmt::ReaderWriterMutex> g(
                                            &d_defaultThresholdsCallbackMutex);
    if (defaultThresholdLevelsCallback) {
        d_defaultThresholdsCallback = *defaultThresholdLevelsCallback;
    } else {
        DefaultThresholdLevelsCallback nullCallback;
        d_defaultThresholdsCallback = nullCallback;
    }
}

int CategoryManager::setThresholdLevelsHierarchically(
                                                const char *categoryNamePrefix,
                                                int         recordLevel,
                                                int         passLevel,
                                                int         triggerLevel,
                                                int         triggerAllLevel)
{
    BSLS_ASSERT(categoryNamePrefix);

    if (!Category::areValidThresholdLevels(recordLevel,
                                           passLevel,
                                           triggerLevel,
                                           triggerAllLevel)) {
        return -1;                                                    // RETURN
    }

    bsl::string filterBuffer;
    const char * const filteredPrefix = filterCategoryName(&filterBuffer,
                                                           categoryNamePrefix);

    // Empty prefix is special
    if (filteredPrefix[0] == 0) {
        // The empty prefix sets everything existing category and it is not
        // stored as a hierarchical setting as it would overwrite the defaults.
        // It also obsoletes every "shadow" setting as it overrides them.

        const bslmt::WriteLockGuard<bslmt::ReaderWriterLock> registryGuard(
                                                              &d_registryLock);

        typedef CategoryPtrVector::const_iterator Iter;
        for (Iter it = d_categories.begin(); it != d_categories.end(); ++it) {
            (*it)->setLevels(recordLevel,
                             passLevel,
                             triggerLevel,
                             triggerAllLevel);
        }

        d_orphanHierarchicalSettings.clear();

        return static_cast<int>(d_categories.size());                 // RETURN
    }

    // We always need to write.  If we find matching categories or prefixes we
    // have to update the levels.  If we don't find any we have to add a prefix
    // to remember these settings.
    const bslmt::WriteLockGuard<bslmt::ReaderWriterLock> registryGuard(
                                                              &d_registryLock);

    // If we do not have a category that matches the setting exactly we need to
    // add a "shadow" setting into `d_orphanHierarchicalSettings` so that we
    // can apply the threshold values to later created hierarchical categories
    // whose name starts with this prefix, or *is* this prefix (in which case
    // we will remove the "shadow" setting).  Note that we can use
    // `filteredPrefix.data()` in the search because it is always null
    // terminated: `bsl::string` always null terminates its buffer, and the
    // incoming argument is also null terminated.
    const CategoryMap::const_iterator regFindIter =
                                               d_registry.find(filteredPrefix);
    const bool masterCategoryExists = (regFindIter != d_registry.end());

#ifdef BSLS_ASSERT_IS_ACTIVE
    // Find if we have a "shadow" setting for this category name prefix.  Note
    // that we can use `filteredPrefix.data()` in the search because it is
    // always null terminated: `bsl::string` always null terminates its buffer,
    // and the incoming argument is also null terminated.
    const bsl::optional<HierarchicalCategorySetting> settingFind =
                             d_orphanHierarchicalSettings.find(filteredPrefix);
    const bool masterSettingExists = (!!settingFind);

    // This must not happen, we delete the orphaned setting when the category
    // with that name is added.
    BSLS_ASSERT(!(masterSettingExists && masterCategoryExists));
#endif

    // If the master category does not exist we have to add an orphan/shadow
    if (!masterCategoryExists) {
        d_orphanHierarchicalSettings.emplace(filteredPrefix,
                                             filteredPrefix,
                                             recordLevel,
                                             passLevel,
                                             triggerLevel,
                                             triggerAllLevel);
    }

    // This setting overrides any setting that was for a longer name starting
    // with this prefix, so we need to remove all those "shadow" settings:
    d_orphanHierarchicalSettings.eraseChildrenOfPrefix(filteredPrefix);

    // Finally we update all categories that match the prefix:
    return static_cast<int>(d_categoryPrefixer.forEachPrefix(
                                        filteredPrefix,
                                        UpdateLevelsByIndex(&d_categories,
                                                            recordLevel,
                                                            passLevel,
                                                            triggerLevel,
                                                            triggerAllLevel)));
}

int CategoryManager::addRule(const Rule& ruleToAdd)
{
    bslmt::LockGuard<bslmt::Mutex> guard(&d_ruleSetMutex);

    const int ruleId = d_ruleSet.addRule(ruleToAdd);
    if (ruleId < 0) {
        return 0;                                                     // RETURN
    }

    privateApplyRulesToAllCategories(guard);

    return 1;
}

int CategoryManager::addRules(const RuleSet& ruleSet)
{
    bslmt::LockGuard<bslmt::Mutex> guard(&d_ruleSetMutex);

    const int count = d_ruleSet.addRules(ruleSet);

    if (count) {
        privateApplyRulesToAllCategories(guard);
    }

    return count;
}

int CategoryManager::removeRule(const Rule& ruleToRemove)
{
    bslmt::LockGuard<bslmt::Mutex> guard(&d_ruleSetMutex);

    const int count = d_ruleSet.removeRule(ruleToRemove);

    if (count) {
        privateApplyRulesToAllCategories(guard);
    }

    return count;
}

int CategoryManager::removeRules(const RuleSet& ruleSet)
{
    bslmt::LockGuard<bslmt::Mutex> guard(&d_ruleSetMutex);

    const int count = d_ruleSet.removeRules(ruleSet);

    if (count) {
        privateApplyRulesToAllCategories(guard);
    }

    return count;
}

void CategoryManager::removeAllRules()
{
    bslmt::LockGuard<bslmt::Mutex> guard(&d_ruleSetMutex);

    const int count = d_ruleSet.numRules();

    if (count) {
        d_ruleSet.removeAllRules();
        privateApplyRulesToAllCategories(guard);
    }
}

// BDE_VERIFY pragma: pop

// ACCESSORS
const Category *CategoryManager::lookupCategory(const char *categoryName) const
{
    bsl::string filteredName;
    return lookupFilteredCategory(filterCategoryName(&filteredName,
                                                     categoryName));
}

                        // Individual Default Levels

int CategoryManager::defaultPassThresholdLevel() const
{
    const bslmt::ReadLockGuard<bslmt::ReaderWriterMutex> guard(
                                              &d_defaultThresholdsLevelsMutex);

    return d_defaultThresholdLevels.passLevel();
}

int CategoryManager::defaultRecordThresholdLevel() const
{
    const bslmt::ReadLockGuard<bslmt::ReaderWriterMutex> guard(
                                              &d_defaultThresholdsLevelsMutex);

    return d_defaultThresholdLevels.recordLevel();
}

int CategoryManager::defaultTriggerAllThresholdLevel() const
{
    const bslmt::ReadLockGuard<bslmt::ReaderWriterMutex> guard(
                                              &d_defaultThresholdsLevelsMutex);

    return d_defaultThresholdLevels.triggerAllLevel();
}

int CategoryManager::defaultTriggerThresholdLevel() const
{
    const bslmt::ReadLockGuard<bslmt::ReaderWriterMutex> guard(
                                              &d_defaultThresholdsLevelsMutex);

    return d_defaultThresholdLevels.triggerLevel();
}

                          // Default Levels Aggregate

ThresholdAggregate CategoryManager::defaultThresholdLevels() const
{
    const bslmt::ReadLockGuard<bslmt::ReaderWriterMutex> guard(
                                              &d_defaultThresholdsLevelsMutex);

    return d_defaultThresholdLevels;
}

                   // Determine Default Threshold Levels

int CategoryManager::thresholdLevelsForNewCategory(
                                        ThresholdAggregate *levels,
                                        const char         *categoryName) const
{
    BSLS_ASSERT(levels);
    BSLS_ASSERT(categoryName);

    bslmt::ReadLockGuard<bslmt::ReaderWriterMutex> cbGuard(
                                            &d_defaultThresholdsCallbackMutex);
    if (d_defaultThresholdsCallback) {
        bsl::string  filteredName;
        const char  *localCategoryName = filterCategoryName(&filteredName,
                                                            categoryName);

        int recordLevel, passLevel, triggerLevel, triggerAllLevel;
        d_defaultThresholdsCallback(&recordLevel,
                                    &passLevel,
                                    &triggerLevel,
                                    &triggerAllLevel,
                                    localCategoryName);

        return levels->setLevels(recordLevel,
                                 passLevel,
                                 triggerLevel,
                                 triggerAllLevel);                    // RETURN
    }
    cbGuard.release()->unlock();

    *levels = d_defaultThresholdLevels;

    return 0;
}

void CategoryManager::setCategoryThresholdsToCurrentDefaults(
                                                      Category *category) const
{
    const bslmt::ReadLockGuard<bslmt::ReaderWriterMutex> g(
                                              &d_defaultThresholdsLevelsMutex);
    category->setLevels(d_defaultThresholdLevels.recordLevel(),
                        d_defaultThresholdLevels.passLevel(),
                        d_defaultThresholdLevels.triggerLevel(),
                        d_defaultThresholdLevels.triggerAllLevel());
}

void CategoryManager::setCategoryThresholdsToFactoryDefaults(
                                                      Category *category) const
{
    category->setLevels(d_factoryThresholdLevels.recordLevel(),
                        d_factoryThresholdLevels.passLevel(),
                        d_factoryThresholdLevels.triggerLevel(),
                        d_factoryThresholdLevels.triggerAllLevel());
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
