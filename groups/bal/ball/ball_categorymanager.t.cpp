// ball_categorymanager.t.cpp                                         -*-C++-*-
#include <ball_categorymanager.h>

#include <ball_loggermanagerdefaults.h>

#include <bdlb_bitutil.h>

#include <bdlsb_fixedmemoutstreambuf.h>

#include <bslim_testutil.h>

#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>
#include <bslma_testallocatorexception.h>

#include <bslmt_barrier.h>
#include <bslmt_lockguard.h>
#include <bslmt_threadutil.h>

#include <bsls_platform.h>

#include <bsl_algorithm.h>
#include <bsl_bitset.h>
#include <bsl_climits.h>
#include <bsl_cstddef.h>
#include <bsl_cstdio.h>
#include <bsl_cstdlib.h>
#include <bsl_cstring.h>
#include <bsl_iomanip.h>
#include <bsl_iostream.h>
#include <bsl_new.h>
#include <bsl_ostream.h>    // i2bs
#include <bsl_sstream.h>
#include <bsl_streambuf.h>  // i2bs
#include <bsl_string.h>
#include <bsl_vector.h>

#include <bsl_c_stdlib.h>

using namespace BloombergLP;
using namespace bsl;

//=============================================================================
//                                   TEST PLAN
//-----------------------------------------------------------------------------
//                                   Overview
//                                   --------
// The component under test manages categories, which are composed of a string
// and four integral values.  The category manager provides accessors and
// manipulators to the categories, and an iterator and a manipulator to step
// through all categories.  We must ensure that (1) the categories are added
// correctly, (2) the accessors return the correct values, (3) the
// manipulators provide modifiable access to the categories, and (4) the
// iterator and the manipulator are correctly implemented.
//
// Note that places where test drivers in this family are likely to require
// adjustment are indicated by the tag: "ADJ".
//-----------------------------------------------------------------------------
// `ball::Category` private methods, tested indirectly:
// [ 2] ball::Category(const char *name, int, int, int, int, *ba = 0);
// [ 2] ~ball::Category();
//
// `ball::Category` public interface:
// [ 5] static bool areValidThresholdLevels(int, int, int, int);
// [ 5] int setLevels(int, int, int, int);
// [ 3] const char *categoryName() const;
// [ 4] bool isEnabled(int) const;
// [ 3] int recordLevel() const;
// [ 3] int passLevel() const;
// [ 3] const ball::ThresholdAggregate& thresholdLevels() const;
// [ 3] int triggerLevel() const;
// [ 3] int triggerAllLevel() const;
//
// `ball::CategoryManager` public interface:
// [ 2] ball::CategoryManager(bslma::Allocator *ba = 0);
// [ 2] ~ball::CategoryManager();
// [ 6] ball::Category& operator[](int index);
// [ 5] ball::Category *addCategory(const char *name, int, int, int, int);
// [11] ball::Category *addCategory(Holder *, name, int, int, int, int);
// [18] Category *addCategoryHierarchically(const char *name);
// [ 3] ball::Category *lookupCategory(const char *name);
// [11] ball::Category *lookupCategory(Holder *, const char *name);
// [11] void resetCategoryHolders();
// [22] const CategoryHolder *setCategory(const char *name);
// [22] void setCategory(CategoryHolder *holder, const char *name);
// [22] const CategoryHolder *setCategory(const char*, int, int, int, int);
// [20] static void setDefaultThresholdLevels(int, int, int, int);
// [20] static int defaultRecordThresholdLevel();
// [20] static int defaultPassThresholdLevel();
// [20] static int defaultTriggerThresholdLevel();
// [20] static int defaultTriggerAllThresholdLevel();
// [20] static void setDefaultThresholdLevelsCallback(Callback);
// [21] static int maxNumCategories();
// [21] static void setMaxNumCategories(int length);
// [20] static void resetDefaultThresholdLevels();
// [20] static void setCategoryThresholdsToCurrentDefaults(Category *cat);
// [20] static void setCategoryThresholdsToFactoryDefaults(Category *cat);
// [ 5] ball::Category *setThresholdLevels(*name, int, int, int, int);
// [19] void setThresholdLevelsHierarchically(const char*, int, int, int, int);
// [ 8] int addRule(const ball::Rule& rule);
// [ 9] int addRules(const ball::RuleSet& ruleSet);
// [ 8] int removeRule(const ball::Rule& rule);
// [ 9] int removeRules(const ball::RuleSet& ruleSet);
// [ 8] void removeAllRules();
// [13] bslmt::Mutex& rulesetMutex();
// [ 6] const ball::Category& operator[](int index) const;
// [ 3] const ball::Category *lookupCategory(const char *name) const;
// [ 3] int length() const;
// [13] ball::RuleSet& ruleSet() const;
// [ 8] bsls::Types::Int64 ruleSetSequenceNumber() const;
// [23] template <class VISITOR> void visitCategories(VISITOR& visitor);
// [23] template <class VISITOR> void visitCategories(VISITOR&) const;
//
// `ball::CategoryHolder` public interface:
// [10] void reset();
// [10] void setCategory(const ball::Category *category);
// [10] void setThreshold(int threshold);
// [10] void setNext(ball::CategoryHolder *holder);
// [10] const ball::Category *category() const;
// [10] int threshold() const;
// [10] ball::CategoryHolder *next() const;
//
// [18] template CategoryManager_Trie
//
#ifndef BDE_OMIT_INTERNAL_DEPRECATED
// `ball::CategoryManagerIter` public interface:
// [16] CategoryManagerIter(const CategoryManager& cm);
// [16] ~CategoryManagerIter();
// [16] void operator++();
// [16] operator const void *() const;
// [16] const Category& operator()() const;
//
// `ball::CategoryManagerManip` public interface:
// [17] CategoryManagerManip(CategoryManager *cm);
// [17] ~CategoryManagerManip();
// [17] void advance();
// [17] Category& operator()();
// [17] operator const void *() const;
#endif // BDE_OMIT_INTERNAL_DEPRECATED
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 2] BASIC CONSTRUCTORS AND PRIMARY MANIPULATORS (BOOTSTRAP)
// [ 2] BOOTSTRAP: ball::Category *addCategory(*name, int, int, int, int);
// [ 7] MT-SAFETY
// [12] TESTING IMPACT OF RULES ON CATEGORY HOLDERS
// [13] CONCURRENCY TEST: RULES
// [14] UNIQUENESS OF INITIAL RULE SET SEQUENCE NUMBER
// [15] DRQS 171004031 - TSAN test
// [16] TESTING `ball::CategoryManagerIter` (DEPRECATED)
// [17] TESTING `ball::CategoryManagerManip` (DEPRECATED)
// [18] TESTING `addCategoryHierarchically`
// [19] TESTING `setThresholdLevelsHierarchically`
// [20] TESTING DEFAULT THRESHOLD LEVELS
// [21] TESTING CATEGORY LIMITS
// [22] TESTING `setCategory` (3 overloads)
// [23] TESTING `visitCategories` (const and non-const)
// [24] MT-SAFETY: `addCategoryHierarchically`
// [25] USAGE EXAMPLE

// ============================================================================
//                     STANDARD BDE ASSERT TEST FUNCTION
// ----------------------------------------------------------------------------

namespace {

int testStatus = 0;

void aSsErT(bool condition, const char *message, int line)
{
    if (condition) {
        cout << "Error " __FILE__ "(" << line << "): " << message
             << "    (failed)" << endl;

        if (0 <= testStatus && testStatus <= 100) {
            ++testStatus;
        }
    }
}

}  // close unnamed namespace

// ============================================================================
//               STANDARD BDE TEST DRIVER MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT       BSLIM_TESTUTIL_ASSERT
#define ASSERTV      BSLIM_TESTUTIL_ASSERTV

#define LOOP_ASSERT  BSLIM_TESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLIM_TESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLIM_TESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLIM_TESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLIM_TESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLIM_TESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLIM_TESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLIM_TESTUTIL_LOOP6_ASSERT

#define Q            BSLIM_TESTUTIL_Q   // Quote identifier literally.
#define P            BSLIM_TESTUTIL_P   // Print identifier and value.
#define P_           BSLIM_TESTUTIL_P_  // P(X) without '\n'.
#define T_           BSLIM_TESTUTIL_T_  // Print a tab (w/o newline).
#define L_           BSLIM_TESTUTIL_L_  // current Line number

//=============================================================================
//                           GLOBAL MACROS FOR TESTING
//-----------------------------------------------------------------------------
#define PA(X)  cout << #X " = " << ((void *) X) << endl;
#define PA_(X) cout << #X " = " << ((void *) X) << ", " << flush;

// The following variable and macros provide a thread-safe framework for using
// `bsl::cout`.

static bslmt::Mutex coutMutex;

#define MTCOUT coutMutex.lock(); { \
                   bsl::cout << bsl::setw(3) \
                             << bslmt::ThreadUtil::selfId() \
                             << ": "
#define MTENDL  bsl::endl;  } coutMutex.unlock()
#define MTFLUSH bsl::flush; } coutMutex.unlock()

void mTaSsErT(int c, const char *s, int i)
{
    if (c) {
        MTCOUT << "Error " << __FILE__ << "(" << i << "): " << s
               << "    (failed)" << MTENDL;
    }
}

#define MTASSERT(X) { mTaSsErT(!(X), #X, __LINE__); }

#define MTLOOP_ASSERT(I,X) { \
   if (!(X)) { MTCOUT << #I << ": " << I << MTENDL; \
               mTaSsErT(1, #X, __LINE__); }}

#define MTLOOP2_ASSERT(I,J,X) { \
   if (!(X)) { MTCOUT << #I << ": " << I << "\t" << #J << ": " \
              << J << MTENDL; mTaSsErT(1, #X, __LINE__); } }

//=============================================================================
//              GLOBAL TYPES, CONSTANTS, AND VARIABLES FOR TESTING
//-----------------------------------------------------------------------------

typedef ball::CategoryManager         Obj;

typedef ball::Category                Entry;
typedef ball::CategoryHolder          Holder;
typedef ball::RuleSet::MaskType       MaskType;
typedef ball::ThresholdAggregate      Thresholds;

typedef const char                   *Name;

typedef bslma::TestAllocator          TestAllocator;
typedef bslma::DefaultAllocatorGuard  DefaultAllocGuard;

typedef bsls::Types::Int64            Int64;

static int verbose;
static int veryVerbose;
static int veryVeryVerbose;

Name NAMES[] = {
    "",
    "A",
    "ABC",
    "abc",
    "0123 abc ABC defghi",
    "_______",
    "ABC.DEF.GHIJKLMNOPQ.ABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890.XYZ"
};

const Name &VA = NAMES[0],
           &VB = NAMES[1],
           &VC = NAMES[2],
           &VD = NAMES[3],
           &VE = NAMES[4],
           &VF = NAMES[5],
           &VG = NAMES[6];

const int NUM_NAMES = sizeof NAMES / sizeof *NAMES;

const int LEVELS[][4]  = {
    {   0,   0,   0,   0 },
    {   4,   3,   2,   1 },
    {   5,   6,   7,   8 },
    {  19,  19,  20,  20 },
    { 224, 192, 160, 128 },
    { 254, 254, 224, 224 },
    { 200, 250, 100, 150 }
};

const int *LA = LEVELS[0],
          *LB = LEVELS[1],
          *LC = LEVELS[2],
          *LD = LEVELS[3],
          *LE = LEVELS[4],
          *LF = LEVELS[5],
          *LG = LEVELS[6];

//-----------------------------------------------------------------------------

// Helper data for hierarchical tests
static ball::ThresholdAggregate s_callbackLevels;

/// Callback to be used by category manager to obtain new logging threshold
/// levels for a new category that is about to be created.
static void dtlCallbackRaw(int        *recordLevel,
                           int        *passLevel,
                           int        *triggerLevel,
                           int        *triggerAllLevel,
                           const char *)
{
    *recordLevel     = s_callbackLevels.recordLevel();
    *passLevel       = s_callbackLevels.passLevel();
    *triggerLevel    = s_callbackLevels.triggerLevel();
    *triggerAllLevel = s_callbackLevels.triggerAllLevel();
}

                             // =================
                             // class my_ListType
                             // =================

/// Objects of this type are passed in thread arguments to
/// functions declared `extern "C"`.
///
/// Note that this is a workaround for certain compilers (such as
/// Sun WorkShop 6 update 1 C++ 5.2 Patch 109508-09 2002/07/08)
/// which do not allow template functions to be called in `extern "C"`
/// functions.
class my_ListType : public bsl::vector<void *> {

  public:
    // MANIPULATORS

    /// Sort this list in ascending order.
    void sort();
};

// MANIPULATORS
inline
void my_ListType::sort()
{
    bsl::sort(begin(), end());
}

                         // =========================
                         // class my_ThreadParameters
                         // =========================

/// This class provides a set of parameters given to a worker thread.  In
/// particular, this class provides a category manager on which to perform
/// thread-specific actions, a list of category names to act on, a
/// barrier object to coordinate thread activity, and a container to store
/// the results of any computations.
struct my_ThreadParameters {

    bslmt::Barrier *d_barrier_p; // coordinates activity between threads (held)
    my_ListType    *d_results_p; // container to store results (held)
    Obj            *d_cm_p;      // a category manager (held)
    bsl::string    *d_names_p;   // a list of category names (held)
    int             d_size;      // number of category names in list
};

//=============================================================================
//                      GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

int calculateThreshold(int record, int pass, int trigger, int triggerAll)
{
    int value = record;
    if (value < pass) {
        value = pass;
    }
    if (value < trigger) {
        value = trigger;
    }
    if (value < triggerAll) {
        value = triggerAll;
    }
    return value;
}

template <bsl::size_t BITS>
bsl::string bitset2string(const bsl::bitset<BITS>& bs)
{
    return bs.template to_string<char, char_traits<char>, allocator<char> >();
}

extern "C"
void *case9ThreadW(void *arg)
{
    // Retrieve the parameters `cm`, `names`, `NUM_NAMES`, and `results`
    // from the opaque argument `arg` of type pointer to `my_ThreadParameters`.
    // In a tight loop, write each category names in the `names` array of
    // size `NUM_NAMES` to the category manager `cm`.  Store the address
    // of each category added to `cm` in the `results` container.  Return
    // after attempting to add each category.  Note that when more than one
    // "write" thread is active, we expect each to add only a subset of
    // categories to the manager due to the acquisition of write locks and
    // the fact that the `addCategory` method fails if the category is already
    // present.

    my_ThreadParameters&  params    = *static_cast<my_ThreadParameters*>(arg);
    Obj&                  cm        = *params.d_cm_p;
    bsl::string          *names     = params.d_names_p;
    const int             NUM_NAMES = params.d_size;
    my_ListType&          results   = *params.d_results_p;

    params.d_barrier_p->wait();

    for (int i = 0; i < NUM_NAMES; ++i) {
        if (veryVeryVerbose) {
            MTCOUT << "Add category '" << names[i] << "'" << MTENDL;
        }

        Holder mH;
        mH.reset();
        void *obj = cm.addCategory(&mH, names[i].c_str(), 12, 34, 56, 78);

        if (obj) {
            MTLOOP_ASSERT(mH.threshold(), 78  == mH.threshold());
            MTLOOP2_ASSERT(obj, mH.category(),
                           obj == (const void *) mH.category());
            results.push_back(obj);
        }
    }

    if (veryVerbose) {
        MTCOUT << "\t"
               << bsl::setw(4)
               << results.size() << " categories were added" << MTENDL;
    }

    return 0;
}

extern "C"
void *case9ThreadQ(void *arg)
{
    // Retrieve the parameters `cm`, `names`, `NUM_NAMES`, and `results`
    // from the opaque argument `arg` of type pointer to `my_ThreadParameters`.
    // In a tight loop, query the category manager `cm` for each category
    // named in the `names` array of size `NUM_NAMES`.  Store each address
    // in the `results` container.  After all categories have been
    // retrieved, sort `results`, and return.

    my_ThreadParameters&  params    = *static_cast<my_ThreadParameters*>(arg);
    Obj&                  cm        = *params.d_cm_p;
    bsl::string          *names     = params.d_names_p;
    const int             NUM_NAMES = params.d_size;
    my_ListType&          results   = *params.d_results_p;

    params.d_barrier_p->wait();

    for (int i = 0; i < NUM_NAMES; ++i) {
        if (veryVeryVerbose) {
            MTCOUT << "Lookup category '" << names[i] << "'" << MTENDL;
        }

        Holder mH;
        mH.reset();
        void *obj = 0;
        do {
            obj = cm.lookupCategory(&mH, names[i].c_str());
        } while(0 == obj);

        MTLOOP_ASSERT(mH.threshold(), 78  == mH.threshold());
        MTLOOP2_ASSERT(obj, mH.category(),
                       obj == (const void *) mH.category());

        results.push_back(obj);
    }

    if (veryVeryVerbose) {
        MTCOUT << "\t"
               << "NUM_NAMES = " << NUM_NAMES << ", "
               << "results.size() = " << results.size() << MTENDL;
    }

    ASSERT(NUM_NAMES == (signed) results.size());
    results.sort();
    return 0;
}

extern "C"
void *case24ThreadW(void *arg)
{
    // Retrieve the parameters `cm`, `names`, `NUM_NAMES`, and `results`
    // from the opaque argument `arg` of type pointer to `my_ThreadParameters`.
    // In a tight loop, add each category name in the `names` array of size
    // `NUM_NAMES` to the category manager `cm` using addCategoryHierarchically.
    // Store the address of each category added to `cm` in the `results`
    // container.  Return after attempting to add each category.

    my_ThreadParameters&  params    = *static_cast<my_ThreadParameters*>(arg);
    Obj&                  cm        = *params.d_cm_p;
    bsl::string          *names     = params.d_names_p;
    const int             NUM_NAMES = params.d_size;
    my_ListType&          results   = *params.d_results_p;

    params.d_barrier_p->wait();

    for (int i = 0; i < NUM_NAMES; ++i) {
        if (veryVeryVerbose) {
            MTCOUT << "Add category hierarchically '" << names[i] << "'"
                   << MTENDL;
        }

        void *obj = cm.addCategoryHierarchically(names[i].c_str());

        if (obj) {
            results.push_back(obj);
        }
    }

    if (veryVerbose) {
        MTCOUT << "\t"
               << bsl::setw(4)
               << results.size() << " categories were added" << MTENDL;
    }

    return 0;
}

extern "C"
void *case24ThreadQ(void *arg)
{
    // Query thread for case 24: lookup categories that were added
    // hierarchically.  Unlike case9ThreadQ, this does not check for a
    // specific threshold value since addCategoryHierarchically uses defaults.

    my_ThreadParameters&  params    = *static_cast<my_ThreadParameters*>(arg);
    Obj&                  cm        = *params.d_cm_p;
    bsl::string          *names     = params.d_names_p;
    const int             NUM_NAMES = params.d_size;
    my_ListType&          results   = *params.d_results_p;

    params.d_barrier_p->wait();

    for (int i = 0; i < NUM_NAMES; ++i) {
        if (veryVeryVerbose) {
            MTCOUT << "Lookup category '" << names[i] << "'" << MTENDL;
        }

        const void *obj = 0;
        do {
            obj = cm.lookupCategory(names[i].c_str());
            if (0 == obj) {
                bslmt::ThreadUtil::yield();
            }
        } while (0 == obj);

        results.push_back(const_cast<void *>(obj));
    }

    if (veryVeryVerbose) {
        MTCOUT << "\t"
               << "NUM_NAMES = " << NUM_NAMES << ", "
               << "results.size() = " << results.size() << MTENDL;
    }

    ASSERT(NUM_NAMES == (signed) results.size());
    results.sort();

    return 0;
}

struct RuleThreadTestArgs {
    Obj            *d_mx_p;
    bslmt::Barrier *d_barrier_p;
};

extern "C" void *ruleThreadTest(void *args)
{
    Obj&            mX      = *((RuleThreadTestArgs *)args)->d_mx_p;
    bslmt::Barrier& barrier = *((RuleThreadTestArgs *)args)->d_barrier_p;
    const Obj&      X       = mX;

    Holder initialValue; initialValue.reset();
    bsl::vector<Holder> holders(NUM_NAMES, initialValue);
    for (int i = 0; i < NUM_NAMES; ++i) {
        ASSERT(0 != mX.setThresholdLevels(NAMES[i], 1, 1, 1, 1));
        ASSERT(0 != mX.lookupCategory(&holders[i], NAMES[i]));
    }

    barrier.wait();
    for (int i = 0; i < NUM_NAMES; ++i) {

        // Create a unique string for this thread and this iteration.
        bsl::ostringstream uniqueStream;
        uniqueStream << "ID" << i <<  bslmt::ThreadUtil::selfIdAsInt();
        bsl::string uniqueString(uniqueStream.str());
        const char *US = uniqueString.c_str();

        ASSERT(0 != mX.setThresholdLevels(US, 1, 1, 1, 1));

        // Rule 1 is shared by all threads and permanent, rule 2 is shared by
        // all threads but will be removed, rule 3 and 4 are only by this
        // thread (and removed).

        ball::Rule rule1(NAMES[i], 255, 255, 255, 255);
        ball::Rule rule2(NAMES[i], 255, 255, 255, 255);
        rule2.addPredicate(ball::Predicate("shared", 1));
        ball::Rule rule3(NAMES[i], 255, 255, 255, 255);
        rule3.addPredicate(ball::Predicate(
                                     "thread",
                                     (Int64)bslmt::ThreadUtil::selfIdAsInt()));
        ball::Rule rule4(US, 255, 255, 255, 255);

        mX.addRule(rule1);
        mX.addRule(rule2);
        ASSERT(1 == mX.addRule(rule3));
        ASSERT(0 == mX.addRule(rule3));
        ASSERT(1 == mX.addRule(rule4));
        ASSERT(0 == mX.addRule(rule4));
        barrier.wait();

        mX.rulesetMutex().lock();
        int ruleId1 = X.ruleSet().ruleId(rule1);
        int ruleId3 = X.ruleSet().ruleId(rule3);
        int ruleId4 = X.ruleSet().ruleId(rule4);
        mX.rulesetMutex().unlock();

        ASSERT(0 <= ruleId1);
        ASSERT(0 <= ruleId3);
        ASSERT(0 <= ruleId4);
        for (int j = 0; j < NUM_NAMES; ++j) {
            bool exp = i == j;
            const Entry *entry = mX.lookupCategory(NAMES[j]);
            ASSERT(exp == bdlb::BitUtil::isBitSet(entry->relevantRuleMask(),
                                                  ruleId1));
            ASSERT(exp == bdlb::BitUtil::isBitSet(entry->relevantRuleMask(),
                                                  ruleId3));
            ASSERT(!bdlb::BitUtil::isBitSet(entry->relevantRuleMask(),
                                            ruleId4));
        }

        mX.removeRule(rule2);
        ASSERT(1 == mX.removeRule(rule3));
        ASSERT(1 == mX.removeRule(rule4));

        ball::RuleSet ruleSet;
        ruleSet.addRule(rule4);
        ruleSet.addRule(rule2);

        ASSERT(1 <= mX.addRules(ruleSet));

        mX.rulesetMutex().lock();
        ruleId4 = X.ruleSet().ruleId(rule4);
        mX.rulesetMutex().unlock();
        barrier.wait();

        const Entry *entry = mX.lookupCategory(US);
        ASSERT(bdlb::BitUtil::isBitSet(entry->relevantRuleMask(), ruleId4));
        {
            bslmt::LockGuard<bslmt::Mutex> guard(&mX.rulesetMutex());
            Int64 seqNo = X.ruleSetSequenceNumber();
            const ball::RuleSet& rules = X.ruleSet();
            int numRules      = rules.numRules();
            int numPredicates = rules.numPredicates();

            bslmt::ThreadUtil::yield();

            int count = 0;
            for (int i = 0; i < ball::RuleSet::maxNumRules(); ++i) {
                if (0 != rules.getRuleById(i)) {
                    ++count;
                }
            }
            ASSERT(ruleId4 == rules.ruleId(rule4));
            ASSERT(ruleId1 == rules.ruleId(rule1));

            ASSERT(seqNo         == X.ruleSetSequenceNumber());
            ASSERT(count         == rules.numRules());
            ASSERT(numRules      == rules.numRules());
            ASSERT(numPredicates == rules.numPredicates());

        }
        ASSERT(1 <= mX.removeRules(ruleSet));
    }
    barrier.wait();

    return 0;
}

// ============================================================================
//                         CASE 14 RELATED ENTITIES
// ----------------------------------------------------------------------------

namespace BALL_CATEGORYMANAGER_UNIQUENESS_OF_SEQUENCE_NUMBERS {

enum {
    k_NUM_OBJECTS = 512,
    k_NUM_THREADS = 3
};

bslmt::Barrier barrier(k_NUM_THREADS);  // synchronize threads

struct ThreadArgs {
    bool *d_flags_p;  // `k_NUM_OBJECTS * k_NUM_THREADS` elements
};

extern "C" void *seqNumUniquenessThread(void *args)
{
    ThreadArgs *threadArgs = reinterpret_cast<ThreadArgs *>(args);

    bool *flags = threadArgs->d_flags_p;

    barrier.wait();

    for (int i = 0; i < k_NUM_OBJECTS; ++i) {
        const Obj X;

        const Int64 shiftedSeqNum = X.ruleSetSequenceNumber() >> 48;
        ASSERT(0 <= shiftedSeqNum);
        ASSERT(     shiftedSeqNum < k_NUM_OBJECTS * k_NUM_THREADS);

        flags[shiftedSeqNum] = true;
    }

    return 0;
}

}  // close namespace BALL_CATEGORYMANAGER_UNIQUENESS_OF_SEQUENCE_NUMBERS

struct TestDrqs171004031Data {
    bslmt::Barrier     d_barrier;       // encourage collisions by sync start
    Obj                d_mx;            // Object under test
    const char * const d_categoryName;  // category name

    /// Create a TestDrqs171004031Data object with a barrier to synchronize
    /// the start of the specified `threads`, and the specified
    /// `categoryName`.
    TestDrqs171004031Data(unsigned int  threads,
                          const char   *categoryName);
};

TestDrqs171004031Data::TestDrqs171004031Data(unsigned int  threads,
                                             const char   *categoryName)
: d_barrier(threads)
, d_mx()
, d_categoryName(categoryName)
{
}

/// After waiting for a barrier to encourage collisions, invoke
/// `lookupCategory` on the `CategoryManager` referenced by the
/// `TestDrqs171004031Data` object pointed to by the specified `void_p`.
void *lookupCategoryFunction(void* void_p)
{
    TestDrqs171004031Data &testData =
                                *(static_cast<TestDrqs171004031Data*>(void_p));
    Obj& mX = testData.d_mx;
    static ball::CategoryHolder holder;
    testData.d_barrier.wait();
    mX.lookupCategory(&holder, testData.d_categoryName);
    return 0;
}

/// After waiting for a barrier to encourage collisions, invoke
/// `addCategory` on the `CategoryManager` referenced by the
/// `TestDrqs171004031Data` object pointed to by the specified `void_p`.
void *addCategoryFunction(void* void_p)
{
    TestDrqs171004031Data &testData =
                                *(static_cast<TestDrqs171004031Data*>(void_p));
    Obj& mX = testData.d_mx;
    static ball::CategoryHolder holder;
    testData.d_barrier.wait();
    mX.addCategory(&holder, testData.d_categoryName, 0, 0, 0, 0);
    return 0;
}

/// Create test data and run the specified `addCategoryThreads` number of
/// threads adding categories and the specified `lookupCategoryThreads`
/// number of threads looking up categories.
void testDrqs171004031(unsigned int addCategoryThreads,
                       unsigned int lookupCategoryThreads)
{
    TestDrqs171004031Data testData(addCategoryThreads + lookupCategoryThreads,
                                   "TSAN category");

    bsl::vector<bslmt::ThreadUtil::Handle> handles;
    handles.reserve(addCategoryThreads + lookupCategoryThreads);

    // There must be rules in the system to trigger the collision.
    for (int rule = 0; rule < ball::RuleSet::maxNumRules(); ++rule) {
        testData.d_mx.addRule(ball::Rule("TSAN*",
                                         rule+2,
                                         rule+2,
                                         rule+2,
                                         rule+2));
    }

    for (unsigned int add = 0; add < addCategoryThreads; ++add) {
        handles.push_back(bslmt::ThreadUtil::Handle());
        bslmt::ThreadUtil::create(&(handles.back()),
                addCategoryFunction,
                static_cast<void *>(&testData));
    }
    for (unsigned int lookup = 0; lookup < lookupCategoryThreads; ++lookup) {
        handles.push_back(bslmt::ThreadUtil::Handle());
        bslmt::ThreadUtil::create(&(handles.back()),
                lookupCategoryFunction,
                static_cast<void *>(&testData));
    }
    for (bsl::vector<bslmt::ThreadUtil::Handle>::iterator it = handles.begin();
            it != handles.end();
            ++it)
    {
        bslmt::ThreadUtil::join(*it, 0);
    }
}

/// Functor for testing non-const `visitCategories`.  Must be defined at
/// namespace level for C++03 compatibility (templates cannot be instantiated
/// with function-local types in C++03).
struct CategoryModifierFunctor {
    bsl::vector<bsl::string> *d_visitedNames_p;
    int                      *d_visitCount_p;

    CategoryModifierFunctor(bsl::vector<bsl::string> *names, int *count)
    : d_visitedNames_p(names)
    , d_visitCount_p(count)
    {
    }

    void operator()(ball::Category *cat) const {
        ++(*d_visitCount_p);
        d_visitedNames_p->push_back(cat->categoryName());

        // Verify we can modify through pointer
        int oldRecord = cat->recordLevel();
        cat->setLevels(oldRecord + 1,
                       cat->passLevel(),
                       cat->triggerLevel(),
                       cat->triggerAllLevel());
    }
};

/// Functor for testing const `visitCategories`.  Must be defined at
/// namespace level for C++03 compatibility (templates cannot be instantiated
/// with function-local types in C++03).
struct CategoryReaderFunctor {
    bsl::vector<bsl::string> *d_visitedNames_p;
    int                      *d_visitCount_p;
    int                      *d_totalRecordLevel_p;

    CategoryReaderFunctor(bsl::vector<bsl::string> *names,
                          int                      *count,
                          int                      *total)
    : d_visitedNames_p(names)
    , d_visitCount_p(count)
    , d_totalRecordLevel_p(total)
    {
    }

    void operator()(const ball::Category *cat) const {
        ++(*d_visitCount_p);
        d_visitedNames_p->push_back(cat->categoryName());
        *d_totalRecordLevel_p += cat->recordLevel();
    }
};

/// Test helper for `setDefaultThresholdLevelsCallback` testing.  Must be
/// defined at namespace level for C++03 compatibility.
struct DefaultThresholdCallbackTester {
    static int s_recordLevel;
    static int s_passLevel;
    static int s_triggerLevel;
    static int s_triggerAllLevel;

    static void callback(int *record,
                        int *pass,
                        int *trigger,
                        int *triggerAll,
                        const char *categoryName)
    {
        (void)categoryName;  // Unused in this test
        *record     = s_recordLevel;
        *pass       = s_passLevel;
        *trigger    = s_triggerLevel;
        *triggerAll = s_triggerAllLevel;
    }
};

// Define static members
int DefaultThresholdCallbackTester::s_recordLevel     = 0;
int DefaultThresholdCallbackTester::s_passLevel       = 0;
int DefaultThresholdCallbackTester::s_triggerLevel    = 0;
int DefaultThresholdCallbackTester::s_triggerAllLevel = 0;

/// Return `true` if the specified `name` is contained in the specified
/// `names` vector, and `false` otherwise.
bool containsName(const bsl::vector<bsl::string>& names,
                  const bsl::string_view&         name)
{
    typedef bsl::vector<bsl::string>::const_iterator Iter;
    for (Iter it = names.begin(); it != names.end(); ++it) {
        if (*it == name) {
            return true;                                              // RETURN
        }
    }

    return false;
}

//=============================================================================
//                                 MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;

    verbose         = argc > 2;
    veryVerbose     = argc > 3;
    veryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;;

    bslma::TestAllocator testAllocator(veryVeryVerbose);

    switch (test) { case 0:  // Zero is always the leading case.
      case 25: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //   Extracted from component header file.
        //
        // Concerns:
        // 1. The usage example provided in the component header file compiles,
        //    links, and runs as shown.
        //
        // Plan:
        // 1. Incorporate usage example from header into test driver, remove
        //    leading comment characters, and replace `assert` with `ASSERT`.
        //    (C-1)
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << "USAGE EXAMPLE\n"
                             "=============\n";

        // Redirect 'bsl::cout' to a buffer unless veryVerbose.
        char                        buf[4096];
        bdlsb::FixedMemOutStreamBuf obuf(buf, sizeof buf);
        bsl::streambuf             *coutBuf = bsl::cout.rdbuf();
        if (!veryVerbose) {
            bsl::cout.rdbuf(&obuf);
        }

///Example 1: Basic Usage
/// - - - - - - - - - - -
// The code fragments in the following example illustrate some basic operations
// of category management including (1) adding categories to the registry,
// (2) accessing and modifying the threshold levels of existing categories,
// and (3) iterating over the categories in the registry.
//
// First we define some hypothetical category names:
// ```
        const char *myCategories[] = {
            "EQUITY.MARKET.NYSE",
            "EQUITY.MARKET.NASDAQ",
            "EQUITY.GRAPHICS.MATH.FACTORIAL",
            "EQUITY.GRAPHICS.MATH.ACKERMANN"
        };
// ```
// Next we create a `ball::CategoryManager` named `manager` and use the
// `addCategory` method to define a category for each of the names in
// `myCategories`.  The threshold levels of each of the categories are set to
// slightly different values to help distinguish them when they are displayed
// later:
// ```
        ball::CategoryManager manager;

        const int NUM_CATEGORIES = sizeof myCategories / sizeof *myCategories;
        for (int i = 0; i < NUM_CATEGORIES; ++i) {
            manager.addCategory(myCategories[i],
                                192 + i, 96 + i, 64 + i, 32 + i);
        }
// ```
// In the following, each of the new categories is accessed from the registry
// and their names and threshold levels printed:
// ```
        for (int i = 0; i < NUM_CATEGORIES; ++i) {
            const ball::Category *category =
                                     manager.lookupCategory(myCategories[i]);
            bsl::cout << "[ " << myCategories[i]
                      << ", " << category->recordLevel()
                      << ", " << category->passLevel()
                      << ", " << category->triggerLevel()
                      << ", " << category->triggerAllLevel()
                      << " ]" << bsl::endl;
        }
// ```
// The following is printed to `stdout`:
// ```
//   [ EQUITY.MARKET.NYSE, 192, 96, 64, 32 ]
//   [ EQUITY.MARKET.NASDAQ, 193, 97, 65, 33 ]
//   [ EQUITY.GRAPHICS.MATH.FACTORIAL, 194, 98, 66, 34 ]
//   [ EQUITY.GRAPHICS.MATH.ACKERMANN, 195, 99, 67, 35 ]
        // Verify output from first loop.
        if (!veryVerbose) {
            const char EXPECTED1[] =
                "[ EQUITY.MARKET.NYSE, 192, 96, 64, 32 ]\n"
                "[ EQUITY.MARKET.NASDAQ, 193, 97, 65, 33 ]\n"
                "[ EQUITY.GRAPHICS.MATH.FACTORIAL, 194, 98, 66, 34 ]\n"
                "[ EQUITY.GRAPHICS.MATH.ACKERMANN, 195, 99, 67, 35 ]\n";
            ASSERTV(bsl::string_view(buf, obuf.length()),
                    bsl::string_view(buf, obuf.length()) == EXPECTED1);
            obuf.pubseekpos(0);
        }
// ```
// We next use the `setLevels` method of `ball::Category` to adjust the
// threshold levels of our categories.  The following also demonstrates use of
// the `recordLevel`, etc., accessors of `ball::Category`:
// ```
        for (int i = 0; i < NUM_CATEGORIES; ++i) {
            ball::Category *category = manager.lookupCategory(myCategories[i]);
            category->setLevels(category->recordLevel() + 1,
                                category->passLevel() + 1,
                                category->triggerLevel() + 1,
                                category->triggerAllLevel() + 1);
        }
// ```
// Repeating the second `for` loop from above generates the following output
// on `stdout`:
// ```
//   [ EQUITY.MARKET.NYSE, 193, 97, 65, 33 ]
//   [ EQUITY.MARKET.NASDAQ, 194, 98, 66, 34 ]
//   [ EQUITY.GRAPHICS.MATH.FACTORIAL, 195, 99, 67, 35 ]
//   [ EQUITY.GRAPHICS.MATH.ACKERMANN, 196, 100, 68, 36 ]
        for (int i = 0; i < NUM_CATEGORIES; ++i) {
            const ball::Category *category =
                                     manager.lookupCategory(myCategories[i]);
            bsl::cout << "[ " << myCategories[i]
                      << ", " << category->recordLevel()
                      << ", " << category->passLevel()
                      << ", " << category->triggerLevel()
                      << ", " << category->triggerAllLevel()
                      << " ]" << bsl::endl;
        }

        // Verify output from repeated loop.
        if (!veryVerbose) {
            const char EXPECTED2[] =
                "[ EQUITY.MARKET.NYSE, 193, 97, 65, 33 ]\n"
                "[ EQUITY.MARKET.NASDAQ, 194, 98, 66, 34 ]\n"
                "[ EQUITY.GRAPHICS.MATH.FACTORIAL, 195, 99, 67, 35 ]\n"
                "[ EQUITY.GRAPHICS.MATH.ACKERMANN, 196, 100, 68, 36 ]\n";
            ASSERTV(bsl::string_view(buf, obuf.length()),
                    bsl::string_view(buf, obuf.length()) == EXPECTED2);
            obuf.pubseekpos(0);
        }
// ```
// Next we illustrate use of the index operator as a means of iterating over
// the registry of categories.  In particular, we illustrate an alternate
// approach to modifying the threshold levels of our categories by iterating
// over the categories in the registry of `manager` to increment their
// threshold levels a second time:
// ```
        for (int i = 0; i < manager.length(); ++i) {
            ball::Category& category = manager[i];
            category.setLevels(category.recordLevel() + 1,
                               category.passLevel() + 1,
                               category.triggerLevel() + 1,
                               category.triggerAllLevel() + 1);
        }
// ```
// Note that the `ball::CategoryManager` is intended to be used only on the
// "write" side of the logging facility.  The index operator is provided to
// allow iteration over the registry of categories, e.g., to print their
// names and threshold levels.
//
// Finally, we iterate over the registry to print out the current threshold
// levels of all of our categories:
// ```
        for (int i = 0; i < manager.length(); ++i) {
            const ball::Category& category = manager[i];
            bsl::cout << "[ " << category.categoryName()
                      << ", " << category.recordLevel()
                      << ", " << category.passLevel()
                      << ", " << category.triggerLevel()
                      << ", " << category.triggerAllLevel()
                      << " ]" << bsl::endl;
        }
// ```
// This iteration produces the following output on `stdout`:
// ```
//   [ EQUITY.MARKET.NYSE, 194, 98, 66, 34 ]
//   [ EQUITY.MARKET.NASDAQ, 195, 99, 67, 35 ]
//   [ EQUITY.GRAPHICS.MATH.FACTORIAL, 196, 100, 68, 36 ]
//   [ EQUITY.GRAPHICS.MATH.ACKERMANN, 197, 101, 69, 37 ]
// ```

        // Verify output from final iteration.
        if (!veryVerbose) {
            const char EXPECTED3[] =
                "[ EQUITY.MARKET.NYSE, 194, 98, 66, 34 ]\n"
                "[ EQUITY.MARKET.NASDAQ, 195, 99, 67, 35 ]\n"
                "[ EQUITY.GRAPHICS.MATH.FACTORIAL, 196, 100, 68, 36 ]\n"
                "[ EQUITY.GRAPHICS.MATH.ACKERMANN, 197, 101, 69, 37 ]\n";
            ASSERTV(bsl::string_view(buf, obuf.length()),
                    bsl::string_view(buf, obuf.length()) == EXPECTED3);
            obuf.pubseekpos(0);
        }

///Example 2: Hierarchical Category Management
/// - - - - - - - - - - - - - - - - - - - - - -
// This example demonstrates the hierarchical category management features of
// `ball::CategoryManager`, which allows threshold levels to be inherited from
// parent categories (categories whose names are prefixes of other category
// names).
//
// First, we create a category manager and enable hierarchical threshold
// management by setting a default threshold levels callback.  For this
// example, we'll use static default thresholds:
// ```
        ball::CategoryManager manager2;
// ```
// Then, we create two new categories, `"EQ"` and `"EQ.MARKET"`, with
// explicitly set threshold levels (different from the defaults):
// ```
        manager2.addCategory("EQ", 192, 96, 64, 32);
        manager2.addCategory("EQ.MARKET", 193, 97, 65, 33);
// ```
// Next, we add a new category using `addCategoryHierarchically`.  This method
// finds the longest prefix match among existing categories and inherits
// threshold levels from that category:
// ```
        ball::Category *nyseCategory =
                          manager2.addCategoryHierarchically("EQ.MARKET.NYSE");
// ```
// The new category `"EQ.MARKET.NYSE"` inherits its threshold levels from
// `"EQ.MARKET"` (rather than from `"EQ"` or the defaults) because
// `"EQ.MARKET"` is the longest prefix match:
// ```
        ASSERT(193 == nyseCategory->recordLevel());
        ASSERT( 97 == nyseCategory->passLevel());
        ASSERT( 65 == nyseCategory->triggerLevel());
        ASSERT( 33 == nyseCategory->triggerAllLevel());
// ```
// Then, we use `setThresholdLevelsHierarchically` to adjust the threshold
// levels for all categories whose name starts with `"EQ.MARKET"`:
// ```
        int numUpdated = manager2.setThresholdLevelsHierarchically("EQ.MARKET",
                                                                   194,
                                                                   98,
                                                                   66,
                                                                   34);
        ASSERT(2 == numUpdated);  // Updated "EQ.MARKET" and "EQ.MARKET.NYSE"
// ```
// We can verify that both `"EQ.MARKET"` and `"EQ.MARKET.NYSE"` have been
// updated, while `"EQ"` remains unchanged:
// ```
        const ball::Category *eqCategory = manager2.lookupCategory("EQ");
        const ball::Category *marketCategory =
                                          manager2.lookupCategory("EQ.MARKET");
        const ball::Category *nyseCategory2 =
                                     manager2.lookupCategory("EQ.MARKET.NYSE");

        ASSERT(192 == eqCategory->recordLevel());       // unchanged
        ASSERT(194 == marketCategory->recordLevel());   // updated
        ASSERT(194 == nyseCategory2->recordLevel());    // updated
// ```
// Finally, if we add another category under `"EQ.MARKET"` using
// `addCategoryHierarchically`, it will inherit the updated thresholds:
// ```
        ball::Category *nasdaqCategory =
                        manager2.addCategoryHierarchically("EQ.MARKET.NASDAQ");
        ASSERT(194 == nasdaqCategory->recordLevel());
        ASSERT( 98 == nasdaqCategory->passLevel());
        ASSERT( 66 == nasdaqCategory->triggerLevel());
        ASSERT( 34 == nasdaqCategory->triggerAllLevel());
// ```
// Note that hierarchical category management facilitates organizing logging
// categories into logical groupings where related categories can share common
// threshold configurations while still allowing fine-grained control over
// individual categories.

        // Restore 'bsl::cout'.
        if (!veryVerbose) {
            bsl::cout.rdbuf(coutBuf);
        }
      } break;
      case 24: {
        // --------------------------------------------------------------------
        // MT-SAFETY: `addCategoryHierarchically`
        //
        // Concerns:
        //   `addCategoryHierarchically` and `lookupCategory` should operate
        //   as expected in a multi-threaded environment:
        //
        //   - calls to `lookupCategory` can occur concurrently
        //   - calls to `addCategoryHierarchically` have transactional
        //     integrity
        //   - successful calls to `lookupCategory` refer to the same object
        //
        // Plan:
        //   Create a category manager `mX`.  In several "writer" threads, add
        //   categories to `mX` using `addCategoryHierarchically` with various
        //   hierarchical names.  In several other threads, query `mX` for
        //   each category until all queries are satisfied.  The result of
        //   each call is stored in a container associated with the calling
        //   thread.  When all threads have completed, the values are
        //   validated to ensure that:
        //
        //   - All categories added are unique across all "write" threads.
        //   - The lists of categories queried via `lookupCategory` are
        //     identical across all "query" threads.
        //
        // Testing:
        //   MT-SAFETY: `addCategoryHierarchically`
        // --------------------------------------------------------------------

        if (verbose) cout << "MT-Safety: addCategoryHierarchically\n"
                             "=====================================\n";

        enum {
            NUM_CATEGORIES_PER_THREAD = 25,  // categories per write thread
            NUM_SHARED_CATEGORIES     = 5,   // ~20% shared among all threads
            NUM_UNIQUE_PER_THREAD     = NUM_CATEGORIES_PER_THREAD
                                        - NUM_SHARED_CATEGORIES,
            NUM_W_THREADS  = 4,              // number of "write" threads
            NUM_Q_THREADS  = 16,             // number of "query" threads
            NUM_THREADS    = NUM_W_THREADS + NUM_Q_THREADS,
            // Total unique categories = shared + (unique per thread * threads)
            NUM_UNIQUE_CATEGORIES = NUM_SHARED_CATEGORIES
                                    + NUM_UNIQUE_PER_THREAD * NUM_W_THREADS
        };

        // Generate shared category names (all threads will race for these)
        bsl::string sharedNames[NUM_SHARED_CATEGORIES];
        for (int i = 0; i < NUM_SHARED_CATEGORIES; ++i) {
            bsl::ostringstream oss;
            oss << "mt.test.hier.shared." << i;
            sharedNames[i] = oss.str();
        }

        // Generate per-thread category names (unique + shared)
        bsl::string threadNames[NUM_W_THREADS][NUM_CATEGORIES_PER_THREAD];
        for (int t = 0; t < NUM_W_THREADS; ++t) {
            // First NUM_SHARED_CATEGORIES are the shared ones
            for (int i = 0; i < NUM_SHARED_CATEGORIES; ++i) {
                threadNames[t][i] = sharedNames[i];
            }
            // Rest are unique to this thread
            for (int i = 0; i < NUM_UNIQUE_PER_THREAD; ++i) {
                bsl::ostringstream oss;
                oss << "mt.test.hier." << t << "." << i;
                threadNames[t][NUM_SHARED_CATEGORIES + i] = oss.str();
            }
        }

        // All category names for query threads
        bsl::string allCategoryNames[NUM_UNIQUE_CATEGORIES];
        int idx = 0;
        for (int i = 0; i < NUM_SHARED_CATEGORIES; ++i) {
            allCategoryNames[idx++] = sharedNames[i];
        }
        for (int t = 0; t < NUM_W_THREADS; ++t) {
            for (int i = 0; i < NUM_UNIQUE_PER_THREAD; ++i) {
                allCategoryNames[idx++] = threadNames[t][NUM_SHARED_CATEGORIES + i];
            }
        }

        bslma::TestAllocator ta(veryVeryVerbose);
        Obj                  mX(&ta);
        bslmt::Barrier       barrier(NUM_THREADS);

        struct {
            bslmt::ThreadUtil::Handle d_handle;   // thread handle
            my_ListType               d_results;  // container for results
            my_ThreadParameters       d_params;   // bundled thread parameters
        } threads[NUM_THREADS];

        // Create threads.
        for (int i = 0; i < NUM_THREADS; ++i) {
            threads[i].d_results.reserve(NUM_CATEGORIES_PER_THREAD);
            threads[i].d_params.d_results_p = &threads[i].d_results;
            threads[i].d_params.d_cm_p      = &mX;
            if (i < NUM_W_THREADS) {
                // Each write thread gets shared + unique categories
                threads[i].d_params.d_names_p = threadNames[i];
                threads[i].d_params.d_size    = NUM_CATEGORIES_PER_THREAD;
            }
            else {
                // Query threads look up all unique categories
                threads[i].d_params.d_names_p = allCategoryNames;
                threads[i].d_params.d_size    = NUM_UNIQUE_CATEGORIES;
            }
            threads[i].d_params.d_barrier_p = &barrier;
            bslmt::ThreadUtil::ThreadFunction action = (i < NUM_W_THREADS)
                                                       ? case24ThreadW
                                                       : case24ThreadQ;
            ASSERT(0 == bslmt::ThreadUtil::create(&threads[i].d_handle,
                                                  action,
                                                  &threads[i].d_params));
        }

        for (int i = 0; i < NUM_THREADS; ++i) {
            ASSERT(0 == bslmt::ThreadUtil::join(threads[i].d_handle));
        }

        // Verify the category manager contains exactly the expected number
        // of unique categories.
        ASSERTV(mX.length(), NUM_UNIQUE_CATEGORIES == mX.length());

        // Count total categories added across all write threads.
        // Due to races on shared categories, each thread adds between
        // NUM_UNIQUE_PER_THREAD (if it lost all races) and
        // NUM_CATEGORIES_PER_THREAD (if it won all races).
        int totalAdded = 0;
        for (int i = 0; i < NUM_W_THREADS; ++i) {
            int added = static_cast<int>(threads[i].d_results.size());
            ASSERTV(i, added, added >= NUM_UNIQUE_PER_THREAD);
            ASSERTV(i, added, added <= NUM_CATEGORIES_PER_THREAD);
            totalAdded += added;
        }
        // Total added = unique categories + (NUM_W_THREADS - 1) * 0 for
        // shared ones that only one thread wins
        ASSERTV(totalAdded, totalAdded == NUM_UNIQUE_CATEGORIES);

        if (veryVerbose) {
            for (int i = 0; i < NUM_W_THREADS; ++i) {
                T_;
                P_(i);
                #if !defined(BSLS_PLATFORM_CMP_MSVC)
                P_(threads[i].d_handle);
                #else
                P_(threads[i].d_handle.d_handle);
                #endif
                P(threads[i].d_results.size());
            }
        }

        // Verify all query threads found the same categories.
        for (int i = NUM_W_THREADS + 1; i < NUM_THREADS; ++i) {
            ASSERT(threads[NUM_W_THREADS].d_results ==
                   threads[i].d_results);
        }
      } break;
      case 23: {
        // --------------------------------------------------------------------
        // TESTING `visitCategories`
        //
        // Concerns:
        // 1. Both `const` and non-`const` `visitCategories` correctly visit
        //    all categories in the manager.
        //
        // 2. The visitor function is called exactly once for each category.
        //
        // Plan:
        // 1. Create a category manager with multiple categories.
        //
        // 2. Use a visitor that counts invocations and stores category names.
        //
        // 3. Call both `visitCategories` overloads and verify all categories
        //    were visited exactly once.
        //
        // Testing:
        //   void visitCategories(const bsl::function<void(Category&)>&);
        //   void visitCategories(const bsl::function<void(const Category&)>&)
        //       const;
        // --------------------------------------------------------------------

        if (verbose) cout << "TESTING `visitCategories`\n"
                             "=========================\n";

        if (veryVerbose) cout << "\tTesting non-`const` `visitCategories`\n";
        {
            ball::CategoryManager cm;

            cm.addCategory("cat1",  64,  48, 32, 16);
            cm.addCategory("cat2", 128,  96, 64, 32);
            cm.addCategory("cat3", 192, 144, 96, 48);

            bsl::vector<bsl::string> visitedNames;
            int visitCount = 0;

            const CategoryModifierFunctor modifier(&visitedNames, &visitCount);

            cm.visitCategories(modifier);

            ASSERTV(visitCount,          3 == visitCount);
            ASSERTV(visitedNames.size(), 3 == visitedNames.size());
            ASSERT( containsName(visitedNames, "cat1"));
            ASSERT( containsName(visitedNames, "cat2"));
            ASSERT( containsName(visitedNames, "cat3"));

            // Verify modifications were applied
            ASSERTV(cm.lookupCategory("cat1")->recordLevel(),
                    65 == cm.lookupCategory("cat1")->recordLevel());
            ASSERTV(cm.lookupCategory("cat2")->recordLevel(),
                    129 == cm.lookupCategory("cat2")->recordLevel());
            ASSERTV(cm.lookupCategory("cat3")->recordLevel(),
                    193 == cm.lookupCategory("cat3")->recordLevel());
        }

        // Test const visitCategories
        {
            if (veryVerbose) cout << "\tTesting const visitCategories\n";

            ball::CategoryManager cm;

            cm.addCategory("alpha", 32, 24, 16,  8);
            cm.addCategory("beta",  64, 48, 32, 16);
            cm.addCategory("gamma", 96, 72, 48, 24);

            const ball::CategoryManager& constCm = cm;

            bsl::vector<bsl::string> visitedNames;
            int visitCount = 0;
            int totalRecordLevel = 0;

            CategoryReaderFunctor reader(&visitedNames, &visitCount,
                                         &totalRecordLevel);

            constCm.visitCategories(reader);

            ASSERTV(visitCount,          3 == visitCount);
            ASSERTV(visitedNames.size(), 3 == visitedNames.size());
            ASSERT( containsName(visitedNames, "alpha"));
            ASSERT( containsName(visitedNames, "beta"));
            ASSERT( containsName(visitedNames, "gamma"));
            ASSERTV(totalRecordLevel, 192 == totalRecordLevel);
        }
      } break;
      case 22: {
        // --------------------------------------------------------------------
        // TESTING `setCategory`
        //
        // Concerns:
        // 1. `setCategory(const char *)` finds or creates a category and
        //    returns its pointer.
        //
        // 2. `setCategory(CategoryHolder *, const char *)` also populates the
        //    provided holder with the category.
        //
        // 3. `setCategory(const char *, int, int, int, int)` creates a
        //    category with specified threshold levels.
        //
        // 4. All overloads correctly handle both existing and non-existing
        //    categories.
        //
        // Plan:
        // 1. Test each overload with non-existing categories (creation).
        //
        // 2. Test each overload with existing categories (lookup).
        //
        // 3. Verify holder population and threshold levels are correct.
        //
        // Testing:
        // - Test setCategory(const char *)
        // - Test setCategory(CategoryHolder *, const char *)
        // - Test setCategory(const char *, int, int, int, int)
        //
        // --------------------------------------------------------------------
        if (verbose) cout << "Testing `setCategory`\n"
                             "=====================\n";

        Obj mX(&testAllocator);

        // setCategory(const char *) - creates new category
        {
            const Entry *cat1 = mX.setCategory("alpha");
            ASSERT(cat1);
            ASSERT(0 == strcmp("alpha", cat1->categoryName()));

            // Test again with the same name - should return the same category
            const Entry *cat2 = mX.setCategory("alpha");
            ASSERT(cat1 == cat2);
        }

        // setCategory(CategoryHolder *, const char *) - also populates holder
        {
            static Holder holder;
            const Entry *cat1 = mX.setCategory(&holder, "beta");
            ASSERT(cat1);
            ASSERT(holder.category());
            ASSERT(holder.category() == cat1);
            ASSERT(0 == strcmp("beta", cat1->categoryName()));

            // Test with an existing category that is already in a holder
            static Holder holder2;
            const Entry *cat2 = mX.setCategory(&holder2, "beta");
            ASSERT(cat1 == cat2);
            ASSERT(holder.category() == holder2.category());

            // Test with an existing category that is not in a holder yet
            const Entry *catAlpha = mX.setCategory("alpha");

            static Holder holder3;
            const Entry *cat3 = mX.setCategory(&holder3, "alpha");
            ASSERT(cat3 == catAlpha);
            ASSERT(holder3.category() == cat3);
        }

        // setCategory(const char *, int, int, int, int) -
        //                                        creates with threshold levels
        {
            Entry *cat1 = mX.setCategory("gamma",
                                         64,  // record
                                         48,  // pass
                                         32,  // trigger
                                         16); // triggerAll
            ASSERT(cat1);
            ASSERT(0 == strcmp("gamma", cat1->categoryName()));
            ASSERT(64 == cat1->recordLevel());
            ASSERT(48 == cat1->passLevel());
            ASSERT(32 == cat1->triggerLevel());
            ASSERT(16 == cat1->triggerAllLevel());

            // Test with existing category - should update threshold levels
            Entry *cat2 = mX.setCategory("gamma",
                                         96,  // record
                                         80,  // pass
                                         64,  // trigger
                                         48); // triggerAll
            ASSERT(cat1 == cat2);
            ASSERT(96 == cat2->recordLevel());
            ASSERT(80 == cat2->passLevel());
            ASSERT(64 == cat2->triggerLevel());
            ASSERT(48 == cat2->triggerAllLevel());
        }
      } break;
      case 21: {
        // --------------------------------------------------------------------
        // TESTING CATEGORY LIMITS
        //
        // Concerns:
        // 1. `maxNumCategories` returns the current maximum number of
        //    categories.
        //
        // 2. `setMaxNumCategories` correctly sets the maximum number of
        //    categories.
        //
        // 3. The default value for `maxNumCategories` is 0 (unlimited).
        //
        // Plan:
        // 1. Verify the default value of `maxNumCategories`.
        //
        // 2. Set a new limit and verify `maxNumCategories` returns it.
        //
        // 3. Restore original limit and verify.
        //
        // Testing:
        //   int maxNumCategories();
        //   void setMaxNumCategories(int length);
        // --------------------------------------------------------------------
        if (verbose)
            cout << "Testing category limits\n"
                    "=======================\n";

        Obj mX(&testAllocator);

        // Verify default value
        const int origMax = mX.maxNumCategories();
        ASSERT(0 == origMax);  // Default is 0 (unlimited)

        // Set new limit
        mX.setMaxNumCategories(100);
        ASSERT(100 == mX.maxNumCategories());

        // Set different limit
        mX.setMaxNumCategories(50);
        ASSERT(50 == mX.maxNumCategories());

        // Restore original
        mX.setMaxNumCategories(origMax);
        ASSERT(origMax == mX.maxNumCategories());
      } break;
      case 20: {
        // --------------------------------------------------------------------
        // TESTING DEFAULT THRESHOLD LEVELS
        //
        // Concerns:
        // 1. `setDefaultThresholdLevels` correctly updates default threshold
        //    levels for new categories.
        //
        // 2. The four default threshold level accessors return correct values:
        //    - `defaultRecordThresholdLevel`
        //    - `defaultPassThresholdLevel`
        //    - `defaultTriggerThresholdLevel`
        //    - `defaultTriggerAllThresholdLevel`
        //
        // 3. `resetDefaultThresholdLevels` restores factory defaults.
        //
        // 4. `setCategoryThresholdsToCurrentDefaults` updates category to
        //    current defaults.
        //
        // 5. `setCategoryThresholdsToFactoryDefaults` updates category to
        //    factory defaults.
        //
        // 6. `setDefaultThresholdLevelsCallback` sets a callback to determine
        //    default threshold levels for new categories.
        //
        // Plan:
        // 1. Test default threshold level setters and accessors.
        //
        // 2. Test reset functions.
        //
        // 3. Test callback mechanism.
        //
        // Testing:
        //   void setDefaultThresholdLevels(int, int, int, int);
        //   int defaultRecordThresholdLevel();
        //   int defaultPassThresholdLevel();
        //   int defaultTriggerThresholdLevel();
        //   int defaultTriggerAllThresholdLevel();
        //   void resetDefaultThresholdLevels();
        //   void setCategoryThresholdsToCurrentDefaults(Category *cat);
        //   void setCategoryThresholdsToFactoryDefaults(Category *cat);
        //   void setDefaultThresholdLevelsCallback(Callback);
        // --------------------------------------------------------------------
        if (verbose) cout << "Testing default threshold levels\n"
                             "================================\n";

        Obj mX(&testAllocator);
        const Obj& X = mX;

        if (veryVerbose)
               cout << "\tTesting `setDefaultThresholdLevels` and accessors\n";
        {
            // Check factory defaults
            ASSERT(0   == mX.defaultRecordThresholdLevel());
            ASSERT(64  == mX.defaultPassThresholdLevel());
            ASSERT(0   == mX.defaultTriggerThresholdLevel());
            ASSERT(0   == mX.defaultTriggerAllThresholdLevel());

            // Set new defaults
            mX.setDefaultThresholdLevels(64, 48, 32, 16);
            ASSERT(64  == mX.defaultRecordThresholdLevel());
            ASSERT(48  == mX.defaultPassThresholdLevel());
            ASSERT(32  == mX.defaultTriggerThresholdLevel());
            ASSERT(16  == mX.defaultTriggerAllThresholdLevel());

            // Create category - should use current defaults
            const Entry *cat1 = mX.setCategory("alpha");
            ASSERT(64 == cat1->recordLevel());
            ASSERT(48 == cat1->passLevel());
            ASSERT(32 == cat1->triggerLevel());
            ASSERT(16 == cat1->triggerAllLevel());

            // Change defaults - existing categories unchanged
            mX.setDefaultThresholdLevels(96, 80, 64, 48);
            ASSERT(64 == cat1->recordLevel());
            ASSERT(48 == cat1->passLevel());
            ASSERT(32 == cat1->triggerLevel());
            ASSERT(16 == cat1->triggerAllLevel());

            // New category uses new defaults
            const Entry *cat2 = mX.setCategory("beta");
            ASSERT(96 == cat2->recordLevel());
            ASSERT(80 == cat2->passLevel());
            ASSERT(64 == cat2->triggerLevel());
            ASSERT(48 == cat2->triggerAllLevel());
        }

        if (veryVerbose) cout << "\tTesting `resetDefaultThresholdLevels`\n";
        {
            // Set custom defaults
            mX.setDefaultThresholdLevels(128, 112, 96, 80);
            ASSERT(128 == mX.defaultRecordThresholdLevel());

            // Reset to factory defaults
            mX.resetDefaultThresholdLevels();
            ASSERT(0   == mX.defaultRecordThresholdLevel());
            ASSERT(64  == mX.defaultPassThresholdLevel());
            ASSERT(0   == mX.defaultTriggerThresholdLevel());
            ASSERT(0   == mX.defaultTriggerAllThresholdLevel());
        }

        if (veryVerbose)
                cout << "\tTesting `setCategoryThresholdsToCurrentDefaults`\n";
        {
            // Set custom defaults
            mX.setDefaultThresholdLevels(100, 90, 80, 70);

            // Create category with custom defaults
            Entry *cat = const_cast<Entry *>(mX.setCategory("delta"));
            ASSERT(100 == cat->recordLevel());

            // Manually change the thresholds
            cat->setLevels(10, 20, 30, 40);
            ASSERT(10 == cat->recordLevel());

            // Update to current defaults
            mX.setCategoryThresholdsToCurrentDefaults(cat);
            ASSERT(100 == cat->recordLevel());
            ASSERT(90  == cat->passLevel());
            ASSERT(80  == cat->triggerLevel());
            ASSERT(70  == cat->triggerAllLevel());

            // Cleanup
            mX.resetDefaultThresholdLevels();
        }

        if (veryVerbose)
                cout << "\tTesting `setCategoryThresholdsToFactoryDefaults`\n";
        {
            // Set custom defaults
            mX.setDefaultThresholdLevels(50, 40, 30, 20);

            // Create category using custom defaults
            Entry *cat = const_cast<Entry *>(mX.setCategory("epsilon"));
            ASSERT(50 == cat->recordLevel());

            // Update to factory defaults
            mX.setCategoryThresholdsToFactoryDefaults(cat);
            ASSERT(0  == cat->recordLevel());
            ASSERT(64 == cat->passLevel());
            ASSERT(0  == cat->triggerLevel());
            ASSERT(0  == cat->triggerAllLevel());

            // Cleanup
            mX.resetDefaultThresholdLevels();
        }

        if (veryVerbose)
                     cout << "\tTesting `setDefaultThresholdLevelsCallback`\n";
        {
            // Set callback-provided values
            DefaultThresholdCallbackTester::s_recordLevel     = 111;
            DefaultThresholdCallbackTester::s_passLevel       = 222;
            DefaultThresholdCallbackTester::s_triggerLevel    =  33;
            DefaultThresholdCallbackTester::s_triggerAllLevel =  44;

            // Set the callback
            Obj::DefaultThresholdLevelsCallback dtlCb(
                                    &DefaultThresholdCallbackTester::callback);
            mX.setDefaultThresholdLevelsCallback(&dtlCb);

            // Create a category - should use the callback values above
            const Entry *cat = mX.setCategory("gamma");
            ASSERT(111 == cat->recordLevel()    );
            ASSERT(222 == cat->passLevel()      );
            ASSERT( 33 == cat->triggerLevel()   );
            ASSERT( 44 == cat->triggerAllLevel());

            // Change callback values
            DefaultThresholdCallbackTester::s_recordLevel     = 128;
            DefaultThresholdCallbackTester::s_passLevel       =  96;
            DefaultThresholdCallbackTester::s_triggerLevel    =  64;
            DefaultThresholdCallbackTester::s_triggerAllLevel =  32;

            // Create another category - should use the new callback values
            const Entry *cat2 = mX.setCategory("theta");
            ASSERT(128 == cat2->recordLevel()    );
            ASSERT( 96 == cat2->passLevel()      );
            ASSERT( 64 == cat2->triggerLevel()   );
            ASSERT( 32 == cat2->triggerAllLevel());

            // Clear callback by passing 0
            mX.setDefaultThresholdLevelsCallback(0);

            // Verify `thresholdLevelsForNewCategory` returns defaults (not
            // the old callback values) after callback is cleared.
            {
                ball::ThresholdAggregate levels;
                X.thresholdLevelsForNewCategory(&levels, "noCbCategory");
                ASSERT(X.defaultRecordThresholdLevel()
                                                     == levels.recordLevel());
                ASSERT(X.defaultPassThresholdLevel()
                                                     == levels.passLevel());
                ASSERT(X.defaultTriggerThresholdLevel()
                                                     == levels.triggerLevel());
                ASSERT(X.defaultTriggerAllThresholdLevel()
                                                  == levels.triggerAllLevel());
            }

            // Verify that with callback cleared, new categories use static
            // defaults (which are still factory defaults at this point)
            const Entry *cat3 = mX.setCategory("iota");
            ASSERT(X.defaultRecordThresholdLevel()  == cat3->recordLevel());
            ASSERT(X.defaultPassThresholdLevel()    == cat3->passLevel());
            ASSERT(X.defaultTriggerThresholdLevel() == cat3->triggerLevel());
            ASSERT(X.defaultTriggerAllThresholdLevel()
                                                   == cat3->triggerAllLevel());

            // Reset to factory defaults
            mX.resetDefaultThresholdLevels();
            typedef ball::LoggerManagerDefaults Defaults;
            ASSERT(Defaults::defaultDefaultRecordLevel()
                                           == X.defaultRecordThresholdLevel());
            ASSERT(Defaults::defaultDefaultPassLevel()
                                           == X.defaultPassThresholdLevel());
            ASSERT(Defaults::defaultDefaultTriggerLevel()
                                          == X.defaultTriggerThresholdLevel());
            ASSERT(Defaults::defaultDefaultTriggerAllLevel()
                                       == X.defaultTriggerAllThresholdLevel());
        }
      } break;
      case 19: {
        // --------------------------------------------------------------------
        // TESTING `setThresholdLevelsHierarchically`
        //
        // Concerns:
        // 1. `setThresholdLevelsHierarchically` correctly sets threshold
        //    levels for the specified category.
        //
        // 2. `setThresholdLevelsHierarchically` correctly traverses category
        //    hierarchy using prefix matching to update descendants.
        //
        // 3. `setThresholdLevelsHierarchically` correctly sets threshold
        //    levels for all categories whose names start with the specified
        //    prefix (i.e., the category itself and its descendants).
        //
        // 4. `setThresholdLevelsHierarchically` does not modify ancestor
        //    categories or siblings.
        //
        // 5. `setThresholdLevelsHierarchically` does not create new
        //    categories, or delete existing categories.
        //
        // 6. `setThresholdLevelsHierarchically` creates an "orphaned" setting
        //    for prefixes that do not have a corresponding category (where
        //    the category name is the prefix).
        //
        // Plan
        // 1. Create a set of test categories with known hierarchical
        //    relationships.
        //
        // 2. Call `setThresholdLevelsHierarchically` with varying category
        //    name prefixes.
        //
        // 3. Verify that threshold levels are set correctly for the specified
        //    category and all its descendants (categories whose names start
        //    with the prefix), using the prefix matching behavior.
        //
        // 4. Verify that parent categories and siblings remain unchanged.
        //
        // 5. Verify that no new categories are created by
        //    `setThresholdLevelsHierarchically`.
        //
        // Testing:
        //   void setThresholdLevelsHierarchically(const char *, int, int,
        //                                          int, int);
        // --------------------------------------------------------------------

        if (verbose)
            cout << "TESTING `setThresholdLevelsHierarchically`\n"
                    "==========================================\n";

        ball::CategoryManager cm;

        // Create hierarchical categories
        cm.addCategory("a",       0, 0, 0, 0);
        cm.addCategory("a.b",     0, 0, 0, 0);
        cm.addCategory("a.b.c",   0, 0, 0, 0);
        cm.addCategory("a.b.c.d", 0, 0, 0, 0);
        cm.addCategory("a.b.x",   0, 0, 0, 0);
        cm.addCategory("x",       0, 0, 0, 0);
        cm.addCategory("x.y",     0, 0, 0, 0);
        cm.addCategory("x.y.z",   0, 0, 0, 0);

        const int INIT_LENGTH = cm.length();

        if (veryVerbose) { T_ P(INIT_LENGTH) }

        // Test 1: Set levels for "a.b" - should update "a.b", "a.b.c",
        // "a.b.c.d", "a.b.x" but NOT "a"
        {
            if (veryVerbose)
                cout << "\tTest setThresholdLevelsHierarchically('a.b', "
                     << "192, 96, 64, 32)" << endl;

            int numUpdated = cm.setThresholdLevelsHierarchically(
                                                       "a.b", 192, 96, 64, 32);
            if (veryVerbose) { T_ T_ P(numUpdated) }
            // Should update a.b, a.b.c, a.b.c.d, a.b.x
            ASSERTV(numUpdated, 4 == numUpdated);

            // Verify "a.b" has new levels (prefix itself)
            const ball::Category *cat = cm.lookupCategory("a.b");
            ASSERT(cat);
            ASSERTV(cat->recordLevel(),     192 == cat->recordLevel());
            ASSERTV(cat->passLevel(),       96  == cat->passLevel());
            ASSERTV(cat->triggerLevel(),    64  == cat->triggerLevel());
            ASSERTV(cat->triggerAllLevel(), 32  == cat->triggerAllLevel());

            // Verify "a.b.c" has new levels (descendant)
            cat = cm.lookupCategory("a.b.c");
            ASSERT(cat);
            ASSERTV(cat->recordLevel(),     192 == cat->recordLevel());
            ASSERTV(cat->passLevel(),       96  == cat->passLevel());
            ASSERTV(cat->triggerLevel(),    64  == cat->triggerLevel());
            ASSERTV(cat->triggerAllLevel(), 32  == cat->triggerAllLevel());

            // Verify "a.b.c.d" has new levels (descendant)
            cat = cm.lookupCategory("a.b.c.d");
            ASSERT(cat);
            ASSERTV(cat->recordLevel(),     192 == cat->recordLevel());
            ASSERTV(cat->passLevel(),       96  == cat->passLevel());
            ASSERTV(cat->triggerLevel(),    64  == cat->triggerLevel());
            ASSERTV(cat->triggerAllLevel(), 32  == cat->triggerAllLevel());

            // Verify "a.b.x" has new levels (descendant)
            cat = cm.lookupCategory("a.b.x");
            ASSERT(cat);
            ASSERTV(cat->recordLevel(),     192 == cat->recordLevel());
            ASSERTV(cat->passLevel(),       96  == cat->passLevel());
            ASSERTV(cat->triggerLevel(),    64  == cat->triggerLevel());
            ASSERTV(cat->triggerAllLevel(), 32  == cat->triggerAllLevel());

            // Verify "a" is unchanged (parent/ancestor)
            cat = cm.lookupCategory("a");
            ASSERT(cat);
            ASSERTV(cat->recordLevel(),     0 == cat->recordLevel());
            ASSERTV(cat->passLevel(),       0 == cat->passLevel());
            ASSERTV(cat->triggerLevel(),    0 == cat->triggerLevel());
            ASSERTV(cat->triggerAllLevel(), 0 == cat->triggerAllLevel());

            // Verify "x" hierarchy is unchanged
            cat = cm.lookupCategory("x");
            ASSERT(cat);
            ASSERTV(cat->recordLevel(),     0 == cat->recordLevel());
            ASSERTV(cat->passLevel(),       0 == cat->passLevel());
            ASSERTV(cat->triggerLevel(),    0 == cat->triggerLevel());
            ASSERTV(cat->triggerAllLevel(), 0 == cat->triggerAllLevel());

            // Verify no new categories were created
            ASSERTV(cm.length(), INIT_LENGTH == cm.length());
        }

        // Test 2: Set levels for "x.y" - should update "x.y" and "x.y.z"
        // but NOT "x"
        {
            if (veryVerbose)
                cout << "\tTest setThresholdLevelsHierarchically('x.y', "
                     << "128, 64, 32, 16)" << endl;

            cm.setThresholdLevelsHierarchically("x.y", 128, 64, 32, 16);

            // Verify "x.y" has new levels (prefix itself)
            const ball::Category *cat = cm.lookupCategory("x.y");
            ASSERT(cat);
            ASSERTV(cat->recordLevel(),     128 == cat->recordLevel());
            ASSERTV(cat->passLevel(),       64  == cat->passLevel());
            ASSERTV(cat->triggerLevel(),    32  == cat->triggerLevel());
            ASSERTV(cat->triggerAllLevel(), 16  == cat->triggerAllLevel());

            // Verify "x.y.z" has new levels (descendant)
            cat = cm.lookupCategory("x.y.z");
            ASSERT(cat);
            ASSERTV(cat->recordLevel(),     128 == cat->recordLevel());
            ASSERTV(cat->passLevel(),       64  == cat->passLevel());
            ASSERTV(cat->triggerLevel(),    32  == cat->triggerLevel());
            ASSERTV(cat->triggerAllLevel(), 16  == cat->triggerAllLevel());

            // Verify "x" is unchanged (parent/ancestor)
            cat = cm.lookupCategory("x");
            ASSERT(cat);
            ASSERTV(cat->recordLevel(),     0 == cat->recordLevel());
            ASSERTV(cat->passLevel(),       0 == cat->passLevel());
            ASSERTV(cat->triggerLevel(),    0 == cat->triggerLevel());
            ASSERTV(cat->triggerAllLevel(), 0 == cat->triggerAllLevel());

            // Verify "a.b" hierarchy retains previous settings
            cat = cm.lookupCategory("a.b");
            ASSERT(cat);
            ASSERTV(cat->recordLevel(),     192 == cat->recordLevel());
            ASSERTV(cat->passLevel(),       96  == cat->passLevel());
            ASSERTV(cat->triggerLevel(),    64  == cat->triggerLevel());
            ASSERTV(cat->triggerAllLevel(), 32  == cat->triggerAllLevel());

            // Verify no new categories were created
            ASSERTV(cm.length(), INIT_LENGTH == cm.length());
        }

        // Test 3: Set levels for non-existent category
        {
            if (veryVerbose)
                cout << "\tTest setThresholdLevelsHierarchically("
                     << "'nonexistent', 255, 255, 255, 255)" << endl;

            cm.setThresholdLevelsHierarchically("nonexistent",
                                                255, 255, 255, 255);

            // Verify category was not created
            const ball::Category *cat = cm.lookupCategory("nonexistent");
            ASSERT(0 == cat);

            // Verify no new categories were created
            ASSERTV(cm.length(), INIT_LENGTH == cm.length());
        }

        // Test 4: Set levels for "a.b.c" - should update "a.b.c" and
        // "a.b.c.d" but NOT "a.b" or "a.b.x"
        {
            if (veryVerbose)
                cout << "\tTest setThresholdLevelsHierarchically('a.b.c', "
                     << "64, 32, 16, 8)" << endl;

            cm.setThresholdLevelsHierarchically("a.b.c", 64, 32, 16, 8);

            // Verify "a.b.c" has new levels (prefix itself)
            const ball::Category *cat = cm.lookupCategory("a.b.c");
            ASSERT(cat);
            ASSERTV(cat->recordLevel(),     64 == cat->recordLevel());
            ASSERTV(cat->passLevel(),       32 == cat->passLevel());
            ASSERTV(cat->triggerLevel(),    16 == cat->triggerLevel());
            ASSERTV(cat->triggerAllLevel(), 8  == cat->triggerAllLevel());

            // Verify "a.b.c.d" has new levels (descendant)
            cat = cm.lookupCategory("a.b.c.d");
            ASSERT(cat);
            ASSERTV(cat->recordLevel(),     64 == cat->recordLevel());
            ASSERTV(cat->passLevel(),       32 == cat->passLevel());
            ASSERTV(cat->triggerLevel(),    16 == cat->triggerLevel());
            ASSERTV(cat->triggerAllLevel(), 8  == cat->triggerAllLevel());

            // Verify "a.b" retains previous levels (parent/ancestor)
            cat = cm.lookupCategory("a.b");
            ASSERT(cat);
            ASSERTV(cat->recordLevel(),     192 == cat->recordLevel());
            ASSERTV(cat->passLevel(),       96  == cat->passLevel());
            ASSERTV(cat->triggerLevel(),    64  == cat->triggerLevel());
            ASSERTV(cat->triggerAllLevel(), 32  == cat->triggerAllLevel());

            // Verify "a.b.x" retains previous levels (sibling)
            cat = cm.lookupCategory("a.b.x");
            ASSERT(cat);
            ASSERTV(cat->recordLevel(),     192 == cat->recordLevel());
            ASSERTV(cat->passLevel(),       96  == cat->passLevel());
            ASSERTV(cat->triggerLevel(),    64  == cat->triggerLevel());
            ASSERTV(cat->triggerAllLevel(), 32  == cat->triggerAllLevel());

            // Verify "a" is still unchanged (ancestor)
            cat = cm.lookupCategory("a");
            ASSERT(cat);
            ASSERTV(cat->recordLevel(),     0 == cat->recordLevel());
            ASSERTV(cat->passLevel(),       0 == cat->passLevel());
            ASSERTV(cat->triggerLevel(),    0 == cat->triggerLevel());
            ASSERTV(cat->triggerAllLevel(), 0 == cat->triggerAllLevel());

            // Verify no new categories were created
            ASSERTV(cm.length(), INIT_LENGTH == cm.length());
        }

        // Test 5: Verify no categories are deleted by
        // setThresholdLevelsHierarchically
        {
            if (veryVerbose)
                cout << "\tTest that no categories are deleted" << endl;

            const int lengthBefore = cm.length();

            // Apply hierarchical threshold changes
            cm.setThresholdLevelsHierarchically("a", 10, 20, 30, 40);

            // Verify all original categories still exist
            ASSERT(cm.lookupCategory("a"));
            ASSERT(cm.lookupCategory("a.b"));
            ASSERT(cm.lookupCategory("a.b.c"));
            ASSERT(cm.lookupCategory("a.b.c.d"));
            ASSERT(cm.lookupCategory("a.b.x"));
            ASSERT(cm.lookupCategory("x"));
            ASSERT(cm.lookupCategory("x.y"));
            ASSERT(cm.lookupCategory("x.y.z"));

            // Verify length unchanged (no deletions or additions)
            ASSERTV(cm.length(), lengthBefore == cm.length());
        }

        // Test 6: Orphaned prefix setting - prefix without matching category
        {
            if (veryVerbose)
                cout << "\tTest orphaned prefix setting" << endl;

            // "a.b.orphan" does not exist as a category, but we set
            // thresholds for it.  This creates an "orphaned" setting that
            // will apply to future categories with this prefix.
            const int lengthBefore = cm.length();

            int numUpdated = cm.setThresholdLevelsHierarchically(
                                                 "a.b.orphan", 50, 60, 70, 80);
            // No existing categories match this prefix
            ASSERTV(numUpdated, 0 == numUpdated);

            // No category was created
            ASSERT(0 == cm.lookupCategory("a.b.orphan"));
            ASSERTV(cm.length(), lengthBefore == cm.length());

            // Now add a category that matches the orphaned prefix - only that
            // category is added (not the parent), and it uses the orphaned
            // threshold settings.
            cm.addCategoryHierarchically("a.b.orphan.child");

            // Only "a.b.orphan.child" is created - "a.b.orphan" is NOT created
            // as a category; it remains as an orphaned setting.
            ASSERTV(cm.length(), lengthBefore + 1 == cm.length());

            const ball::Category *orphanChild =
                                         cm.lookupCategory("a.b.orphan.child");
            ASSERT(orphanChild);
            ASSERTV(orphanChild->recordLevel(),
                                             50 == orphanChild->recordLevel());
            ASSERTV(orphanChild->passLevel(), 60 == orphanChild->passLevel());
            ASSERTV(orphanChild->triggerLevel(),
                                            70 == orphanChild->triggerLevel());
            ASSERTV(orphanChild->triggerAllLevel(),
                                         80 == orphanChild->triggerAllLevel());

            // "a.b.orphan" is NOT a category - it's still an orphaned setting
            ASSERT(0 == cm.lookupCategory("a.b.orphan"));

            // Now add "a.b.orphan" as a category - it will use the orphaned
            // settings, and the orphaned setting is then removed.
            cm.addCategoryHierarchically("a.b.orphan");

            ASSERTV(cm.length(), lengthBefore + 2 == cm.length());

            const ball::Category *exOrphan = cm.lookupCategory("a.b.orphan");
            ASSERT(exOrphan);
            ASSERTV(exOrphan->recordLevel(),  50 == exOrphan->recordLevel());
            ASSERTV(exOrphan->passLevel(),    60 == exOrphan->passLevel());
            ASSERTV(exOrphan->triggerLevel(), 70 == exOrphan->triggerLevel());
            ASSERTV(exOrphan->triggerAllLevel(),
                                            80 == exOrphan->triggerAllLevel());
        }
      } break;
      case 18: {
        // --------------------------------------------------------------------
        // TESTING `addCategoryHierarchically`
        //
        // Concerns:
        // 1. `addCategoryHierarchically` adds the specified category and all
        //    hierarchically related parent categories if they do not exist.
        //
        // 2. Category added by `addCategoryHierarchically` have correct
        //    threshold levels derived from an existing parent category or an
        //    orphaned setting (whichever has the longest matching prefix), or
        //    if neither exists the specified default threshold level callback
        //    if set, or otherwise from static default thresholds.
        //
        //    2.1. If a parent category exists and its name is longer than the
        //         matching "orphaned" setting prefix or no matching orphaned
        //         setting exists, the matching parent category's threshold
        //         levels are used.
        //
        //    2.2. If a parent category doesn't exist or its name is shorter
        //         than the matching "orphaned" setting prefix, the matching
        //         orphaned setting's threshold levels are used.
        //
        //    2.3. If neither a parent category nor an orphaned setting exists,
        //         the default threshold levels callback is used to determine
        //         the threshold levels, if it is set.
        //
        //   2.4. If neither a parent category nor an orphaned setting exists,
        //         and no default threshold levels callback is set, the static
        //         default threshold levels are used.
        //
        // Plan
        // 1. Test with static default thresholds (no callback): Add categories
        //    without any parent categories or orphaned settings.  Verify they
        //    use static default threshold levels.  (C-2.4)
        //
        // 2. Test with default threshold callback: Add categories without any
        //    parent categories or orphaned settings.  Verify they use
        //    threshold levels from the callback.  (C-2.3)
        //
        // 3. Test parent category threshold inheritance: Create a parent
        //    category with specific thresholds, then add a child category.
        //    Verify the child inherits the parent's thresholds.  (C-2.1)
        //
        // 4. Test orphaned setting threshold inheritance: Set thresholds for
        //    a non-existent prefix, then add a child, then the exactly
        //    matching category matching that prefix.  Verify the category uses
        //    the orphaned setting's thresholds.  (C-2.2)
        //
        // 5. Test parent vs orphaned setting priority: When both exist, the
        //    one with the longest matching prefix wins.  (C-2.1, C-2.2)
        //
        // Testing:
        //   void addCategoryHierarchically(const char *name);
        // --------------------------------------------------------------------

        if (verbose) cout << "TESTING `addCategoryHierarchically`\n"
                             "===================================\n";

        ball::CategoryManager cm;

        static const struct {
            int         d_line;
            const char *d_name;
        } DATA[] = {
            // line         category name
            // ----  --------------------------
            {  L_,   ""                        },
            {  L_,   "a"                       },
            {  L_,   "a."                      },
            {  L_,   "a.b"                     },
            {  L_,   "a.b."                    },
            {  L_,   "A.B.C.D"                 },
            {  L_,   "x.y.z"                   },
            {  L_,   "x.y.z."                  },
            {  L_,   "x.y.z.a.b.c.d.e.f.g"     },
            {  L_,   "x.y.z.a.b.c.d.e.f.g."    },
            {  L_,   "p.q."                    },
            {  L_,   "p.q.r."                  },
            {  L_,   "p.q.r.s."                },
            {  L_,   "p.q.r.s.t."              },
            {  L_,   "1.2.3.4.5.6.7.8.9.a"     },
            {  L_,   "1.2.3.4.5.6.7.8.9.a."    },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        // Test without default threshold callback
        {
            if (veryVerbose) cout << "\tWithout default threshold callback.\n";

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE = DATA[i].d_line;
                const char *NAME = DATA[i].d_name;

                if (veryVerbose) { T_ T_ P_(LINE) P(NAME) }

                cm.addCategoryHierarchically(NAME);

                const ball::Category *category = cm.lookupCategory(NAME);
                ASSERTV(LINE, NAME, category);
                ASSERTV(LINE, NAME,
                        0 == bsl::strcmp(NAME, category->categoryName()));

                // Verify threshold levels are the factory defaults
                ASSERTV(LINE, NAME, 0   == category->recordLevel());
                ASSERTV(LINE, NAME, 64  == category->passLevel());
                ASSERTV(LINE, NAME, 0   == category->triggerLevel());
                ASSERTV(LINE, NAME, 0   == category->triggerAllLevel());
            }
        }

        // Test with default threshold callback
        {
            if (veryVerbose) cout << "\tWith default threshold callback.\n";

            ball::CategoryManager cm2;

            // Set up callback to return specific threshold levels
            s_callbackLevels = ball::ThresholdAggregate(160, 80, 40, 20);
            Obj::DefaultThresholdLevelsCallback dtlCb(&dtlCallbackRaw);
            cm2.setDefaultThresholdLevelsCallback(&dtlCb);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE = DATA[i].d_line;
                const char *NAME = DATA[i].d_name;

                if (veryVerbose) { T_ T_ P_(LINE) P(NAME) }

                cm2.addCategoryHierarchically(NAME);

                const ball::Category *category = cm2.lookupCategory(NAME);
                ASSERTV(LINE, NAME, category);
                ASSERTV(LINE, NAME,
                        0 == bsl::strcmp(NAME, category->categoryName()));

                // Verify threshold levels from callback
                ASSERTV(LINE, NAME, 160 == category->recordLevel());
                ASSERTV(LINE, NAME, 80  == category->passLevel());
                ASSERTV(LINE, NAME, 40  == category->triggerLevel());
                ASSERTV(LINE, NAME, 20  == category->triggerAllLevel());
            }
        }

        // Test parent category threshold inheritance
        {
            if (veryVerbose)
                          cout << "\tParent category threshold inheritance.\n";

            ball::CategoryManager cm3;

            // Create parent category with specific thresholds
            ball::Category *parent = cm3.addCategory("parent",
                                                     100, 90, 80, 70);
            ASSERT(parent);

            // Add child category hierarchically
            cm3.addCategoryHierarchically("parent.child");

            const ball::Category *child = cm3.lookupCategory("parent.child");
            ASSERT(child);

            // Child should inherit parent's thresholds
            ASSERT(100 == child->recordLevel());
            ASSERT(90  == child->passLevel());
            ASSERT(80  == child->triggerLevel());
            ASSERT(70  == child->triggerAllLevel());
        }

        // Test orphaned setting threshold inheritance
        {
            if (veryVerbose)
                cout << "\tOrphaned setting threshold inheritance." << endl;

            ball::CategoryManager cm4;

            // Set thresholds for non-existent prefix (creates orphaned
            // setting).  Use setThresholdLevelsHierarchically, not
            // setThresholdLevels, to create an orphaned setting without
            // creating the category itself.
            cm4.setThresholdLevelsHierarchically("orphan", 110, 100, 90, 80);

            // Verify orphan category does not exist yet
            ASSERT(0 == cm4.lookupCategory("orphan"));

            // Add child category matching the orphaned prefix
            cm4.addCategoryHierarchically("orphan.child");

            const ball::Category *child = cm4.lookupCategory("orphan.child");
            ASSERT(child);

            // Child should inherit orphaned setting's thresholds
            ASSERT(110 == child->recordLevel());
            ASSERT(100 == child->passLevel());
            ASSERT(90  == child->triggerLevel());
            ASSERT(80  == child->triggerAllLevel());

            // The orphan category still does not exist
            ASSERT(0 == cm4.lookupCategory("orphan"));

            // Now add the exactly matching category
            cm4.addCategoryHierarchically("orphan");

            const ball::Category *orphan = cm4.lookupCategory("orphan");
            ASSERT(orphan);

            // Should use the orphaned setting's thresholds
            ASSERT(110 == orphan->recordLevel());
            ASSERT(100 == orphan->passLevel());
            ASSERT(90  == orphan->triggerLevel());
            ASSERT(80  == orphan->triggerAllLevel());
        }

        // Test parent vs orphaned setting priority (longest prefix wins)
        {
            if (veryVerbose)
                           cout << "\tParent vs. orphaned setting priority.\n";

            ball::CategoryManager cm5;

            // Create parent category "a" with specific thresholds
            ball::Category *parentA = cm5.addCategory("a", 10, 20, 30, 40);
            ASSERT(parentA);

            // Set orphaned setting for "a.b" (longer prefix).  Use
            // setThresholdLevelsHierarchically to create an orphaned setting.
            cm5.setThresholdLevelsHierarchically("a.b", 50, 60, 70, 80);

            // Add child "a.b.c" - should use orphaned "a.b" (longer match)
            cm5.addCategoryHierarchically("a.b.c");

            const ball::Category *abc = cm5.lookupCategory("a.b.c");
            ASSERT(abc);
            ASSERT(50 == abc->recordLevel());
            ASSERT(60 == abc->passLevel());
            ASSERT(70 == abc->triggerLevel());
            ASSERT(80 == abc->triggerAllLevel());

            // Now create parent "a.b" with different thresholds
            ball::Category *parentAB = cm5.addCategory("a.b",
                                                       100, 110, 120, 130);
            ASSERT(parentAB);

            // Add child "a.b.d" - should use parent "a.b" (exact parent)
            cm5.addCategoryHierarchically("a.b.d");

            const ball::Category *abd = cm5.lookupCategory("a.b.d");
            ASSERT(abd);
            ASSERT(100 == abd->recordLevel());
            ASSERT(110 == abd->passLevel());
            ASSERT(120 == abd->triggerLevel());
            ASSERT(130 == abd->triggerAllLevel());

            // Add child "a.c" - should use parent "a" (only parent match)
            cm5.addCategoryHierarchically("a.c");

            const ball::Category *ac = cm5.lookupCategory("a.c");
            ASSERT(ac);
            ASSERT(10 == ac->recordLevel());
            ASSERT(20 == ac->passLevel());
            ASSERT(30 == ac->triggerLevel());
            ASSERT(40 == ac->triggerAllLevel());
        }
      } break;
      case 17: {
#ifndef BDE_OMIT_INTERNAL_DEPRECATED
        // --------------------------------------------------------------------
        // TESTING `ball::CategoryManagerManip`
        //
        // Concerns:
        // 1. The basic concern is that the constructor, the destructor, the
        //    manipulators:
        //      - void advance();
        //      - Category& operator()();
        //    and the accessor:
        //      - operator const void *() const;
        //    operate as expected.
        //
        // Plan:
        // 1. Create a category manager X.  Add categories to X having various
        //    names and threshold level values.  Create an iterator for X.
        //    Change the threshold level values using the modifiable access
        //    provided by the iterator.  Verify that the values are changed.
        //    Change the threshold levels back to their original values, and
        //    verify that they were reset.  (C-1)
        //
        // Testing:
        //   CategoryManagerManip(CategoryManager *cm);
        //   ~CategoryManagerManip();
        //   void advance();
        //   Category& operator()();
        //   operator const void *() const;
        // --------------------------------------------------------------------

        if (verbose) cout << "TESTING `ball::CategoryManagerManip`\n"
                             "====================================\n";

        Obj mX;  const Obj& X = mX;

        for (int i = 0; i < NUM_NAMES; ++i) {
            mX.addCategory(NAMES[i], LEVELS[i][0], LEVELS[i][1],
                                     LEVELS[i][2], LEVELS[i][3]);
        }
        ASSERT(NUM_NAMES == X.length());

        for (ball::CategoryManagerManip it(&mX); it; it.advance()) {
            const Entry *p          = X.lookupCategory(it().categoryName());
            const int    record     = p->recordLevel();
            const int    pass       = p->passLevel();
            const int    trigger    = p->triggerLevel();
            const int    triggerAll = p->triggerAllLevel();

            ASSERT(record     == it().recordLevel());
            ASSERT(pass       == it().passLevel());
            ASSERT(trigger    == it().triggerLevel());
            ASSERT(triggerAll == it().triggerAllLevel());

            it().setLevels(it().recordLevel() + 1,
                           it().passLevel() + 1,
                           it().triggerLevel() + 1,
                           it().triggerAllLevel() + 1);

            ASSERT(record + 1     == p->recordLevel());
            ASSERT(pass + 1       == p->passLevel());
            ASSERT(trigger + 1    == p->triggerLevel());
            ASSERT(triggerAll + 1 == p->triggerAllLevel());

            it().setLevels(it().recordLevel() - 1,
                           it().passLevel() - 1,
                           it().triggerLevel() - 1,
                           it().triggerAllLevel() - 1);

            ASSERT(record     == p->recordLevel());
            ASSERT(pass       == p->passLevel());
            ASSERT(trigger    == p->triggerLevel());
            ASSERT(triggerAll == p->triggerAllLevel());
        }
#endif  // BDE_OMIT_INTERNAL_DEPRECATED
      } break;
      case 16: {
#ifndef BDE_OMIT_INTERNAL_DEPRECATED
        // --------------------------------------------------------------------
        // TESTING `ball::CategoryManagerIter`
        //
        // Concerns:
        // 1. The basic concerns for the iterator are that the constructor,
        //    the destructor, the manipulator `operator++`, and the accessors:
        //      - operator const void *() const;
        //      - const Category& operator()() const;
        //    operate as expected.
        //
        // Plan
        // 1. Construct a category manager X.  Add categories to X having
        //    various names and threshold levels.  Construct another empty
        //    category manager Y.  Walk the categories of X with an iterator;
        //    add categories with names and threshold levels obtained from the
        //    iterator into Y.  Verify that the two category managers have the
        //    same length, and that entries in each with that same name have
        //    the same threshold levels.  (C-1)
        //
        // Testing:
        //   CategoryManagerIter(const CategoryManager& cm);
        //   ~CategoryManagerIter();
        //   void operator ++();
        //   operator const void *() const;
        //   const Category& operator()() const;
        // --------------------------------------------------------------------

        if (verbose) cout << "TESTING `ball::CategoryManagerIter`\n"
                             "===================================\n";

        Obj mX;  const Obj& X = mX;

        for (int i = 0; i < NUM_NAMES; ++i) {
            mX.addCategory(NAMES[i], LEVELS[i][0], LEVELS[i][1],
                                     LEVELS[i][2], LEVELS[i][3]);
        }
        ASSERT(NUM_NAMES == X.length());

        Obj mY;  const Obj& Y = mY;
        ASSERT(0 == Y.length());
        for (ball::CategoryManagerIter it(X); it; ++it) {
            mY.addCategory(it().categoryName(),
                           it().recordLevel(),
                           it().passLevel(),
                           it().triggerLevel(),
                           it().triggerAllLevel());
        }
        ASSERT(X.length() == Y.length());

        for (int i = 0; i < NUM_NAMES; ++i) {
            const Entry *px = X.lookupCategory(NAMES[i]);
            const Entry *py = Y.lookupCategory(NAMES[i]);

            ASSERT(px != py);
            ASSERT(px->recordLevel()     == py->recordLevel());
            ASSERT(px->passLevel()       == py->passLevel());
            ASSERT(px->triggerLevel()    == py->triggerLevel());
            ASSERT(px->triggerAllLevel() == py->triggerAllLevel());
        }
#endif  // BDE_OMIT_INTERNAL_DEPRECATED
      } break;
      case 15: {
        // --------------------------------------------------------------------
        // DRQS 171004031 - TSAN test
        //
        // Concerns:
        // 1. DRQS 171004031 showed the presence of data races, those were
        //    confirmed by this test prior to fixing them, and can now be shown
        //    to be fixed by running this test under TSAN.
        //
        // Plan:
        // 1. Populate the rule set, then simultaneously add categories and
        //    look up categories.
        //
        // Testing:
        //   DRQS 171004031 - TSAN test
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "DRQS 171004031 - TSAN TEST" << endl
                                  << "==========================" << endl;

        testDrqs171004031(4, 4);
      } break;
      case 14: {
        // --------------------------------------------------------------------
        // TESTING UNIQUENESS OF INITIAL RULE SET SEQUENCE NUMBER
        //
        // Concerns:
        // 1. For each object, the initial value for the rule set sequence
        //    number is unique.
        //
        // Plan
        // 1. Run three threads concurrently that each create a succession of
        //    category manager objects in a tight loop.  The threads share an
        //    array having the same number of elements as the total number of
        //    objects created across the three threads.  Each time an object is
        //    created, the element in the array corresponding to that object's
        //    initial rule set sequence number is marked.  After joining the
        //    three threads, verify that all elements of the array have been
        //    marked.  (C-1)
        //
        // Testing:
        //   UNIQUENESS OF INITIAL RULE SET SEQUENCE NUMBER
        // --------------------------------------------------------------------

        if (verbose)
            cout << "TESTING UNIQUENESS OF INITIAL RULE SET SEQUENCE NUMBER\n"
                 << "======================================================\n";

        using namespace BALL_CATEGORYMANAGER_UNIQUENESS_OF_SEQUENCE_NUMBERS;

        const int k_NUM_FLAGS = k_NUM_OBJECTS * k_NUM_THREADS;

        bool       flags[k_NUM_FLAGS];
        ThreadArgs threadArgs = { flags };

        {
            bsl::fill_n(flags, k_NUM_FLAGS, false);

            bslmt::ThreadUtil::Handle threads[k_NUM_THREADS];

            for (int i = 0; i < k_NUM_THREADS; ++i) {
                bslmt::ThreadUtil::create(
                                        &threads[i],
                                        seqNumUniquenessThread,
                                        reinterpret_cast<void *>(&threadArgs));
            }

            for (int i = 0; i < k_NUM_THREADS; ++i) {
                bslmt::ThreadUtil::join(threads[i], 0);
            }

            for (int j = 0; j < k_NUM_FLAGS; ++j) {
                ASSERTV(j, flags[j]);
            }
        }
      } break;
      case 13: {
        // --------------------------------------------------------------------
        // CONCURRENCY TEST: RULES
        //
        // Concerns:  That the operations for accessing and modifying the
        //    rules of a category manager are thread-safe.
        //
        // Plan:
        // --------------------------------------------------------------------

        if (verbose) cout << "Testing Rule Concurrency\n"
                             "========================\n";

        enum {
            NUM_THREADS = 5
        };

        bsl::vector<bslmt::ThreadUtil::Handle> handles;
        handles.resize(NUM_THREADS);

        bslma::TestAllocator ta(veryVeryVerbose);
        Obj                  mX(&ta);
        bslmt::Barrier       barrier(NUM_THREADS);
        RuleThreadTestArgs   args = { &mX, &barrier };

        // Create threads.
        for (int i = 0; i < NUM_THREADS; ++i) {
            ASSERT(0 == bslmt::ThreadUtil::create(&handles[i],
                                                  &ruleThreadTest,
                                                  &args));
        }
        for (int i = 0; i < NUM_THREADS; ++i) {
            ASSERT(0 == bslmt::ThreadUtil::join(handles[i]));
        }

      } break;
      case 12: {
        // --------------------------------------------------------------------
        // TESTING IMPACT OF RULES ON CATEGORY HOLDERS
        //
        // Concerns:  The category holder's of a category have the correct
        //    threshold as rules are added and removed from the category
        //    manager.
        //
        // Plan:
        //    Create a set of `ball::ThresholdAggregate` objects containing a
        //    variety of test threshold levels, and create a series of
        //    category holders for a test category.  Then, for each threshold
        //    aggregate in the test values, set the threshold levels for the
        //    test category.  Verify the threshold levels for the category
        //    holders of the test category.  Then, for each threshold
        //    aggregate in the test values, add a rule that applies to the
        //    category with those threshold values.  Verify the category
        //    holders have the correct threshold, then remove the rule, and
        //    verify the holders return to their original thresholds.
        //    Finally, add a series of rules with different threshold levels,
        //    and verify that the threshold level of the category holders is
        //    the aggregate of multiple rules.
        //
        // --------------------------------------------------------------------

        if (verbose) cout << "Testing rules and category holders\n"
                             "==================================\n";

        bslma::TestAllocator ta;
        int VALUES[] = { 1,
                         31,  // e_FATAL - 1
                         32,  // e_FATAL
                         33,  // e_FATAL + 1
                         63,  // e_ERROR - 1
                         64,  // e_ERROR
                         65,  // e_ERROR + 1
                         95,  // e_WARN - 1
                         96,  // e_WARN
                         97,  // e_WARN + 1
                         127, // e_INFO - 1
                         128, // e_INFO
                         129, // e_INFO + 1
                         159, // e_DEBUG - 1
                         160, // e_DEBUG
                         161, // e_DEBUG + 1
                         191, // e_TRACE - 1
                         192, // e_TRACE
                         193  // e_TRACE + 1
        };
        const int NUM_VALUES = sizeof (VALUES) / sizeof(*VALUES);
        bsl::vector<Thresholds> thresholds(&ta);
        for (int i = 0; i < NUM_VALUES; ++i) {
            thresholds.push_back(Thresholds(VALUES[i], VALUES[i],
                                            VALUES[i], VALUES[i]));
            thresholds.push_back(Thresholds(VALUES[i], 1, 1, 1));
            thresholds.push_back(Thresholds(1, VALUES[i], 1, 1));
            thresholds.push_back(Thresholds(1, 1, VALUES[i], 1));
            thresholds.push_back(Thresholds(1, 1, 1, VALUES[i]));
        }

        Holder initialValue; initialValue.reset();
        const int NUM_HOLDERS = 10;
        bsl::vector<Holder> holders(NUM_HOLDERS, initialValue, &ta);
        Obj mX(&ta);

        const Entry *C = mX.addCategory("C", 255, 255, 255, 255);
        for (bsl::size_t i = 0; i < holders.size(); ++i) {
            ASSERT(C == mX.lookupCategory(&holders[i], "C"));
            ASSERT(255 == holders[i].threshold());
        }

        // Set the threshold levels for the category.
        for (bsl::size_t i = 0; i < thresholds.size(); ++i) {
            ASSERT(0 !=
                   mX.setThresholdLevels("C",
                                         thresholds[i].recordLevel(),
                                         thresholds[i].passLevel(),
                                         thresholds[i].triggerLevel(),
                                         thresholds[i].triggerAllLevel()));
            int catThreshold = Thresholds::maxLevel(thresholds[i]);

            for (bsl::size_t k = 0; k < holders.size(); ++k) {
                LOOP2_ASSERT(i,k, catThreshold == holders[k].threshold());
            }

            // Set a series of rule thresholds for the category.
            for (bsl::size_t j = 0; j < holders.size(); ++j) {
                for (bsl::size_t k = 0; k < holders.size(); ++k) {
                    ASSERT(catThreshold == holders[k].threshold());
                }

                ball::Rule rule("C",
                                thresholds[j].recordLevel(),
                                thresholds[j].passLevel(),
                                thresholds[j].triggerLevel(),
                                thresholds[j].triggerAllLevel());
                ASSERT(1 == mX.addRule(rule));

                int newThreshold = bsl::max(
                            Thresholds::maxLevel(thresholds[j]), catThreshold);
                for (bsl::size_t k = 0; k < holders.size(); ++k) {
                    ASSERT(newThreshold == holders[k].threshold());
                }
                ASSERT(1 == mX.removeRule(rule));

                for (bsl::size_t k = 0; k < holders.size(); ++k) {
                    ASSERT(catThreshold == holders[k].threshold());
                }
            }

            // Testing multiple rules
            int currentThreshold;
            for (int j = 0; j < NUM_VALUES; ++j) {
                ball::Rule rule("C",
                                VALUES[j],
                                VALUES[j],
                                VALUES[j],
                                VALUES[j]);
                ASSERT(1 == mX.addRule(rule));
                currentThreshold = bsl::max(VALUES[j], catThreshold);
                for (bsl::size_t k = 0; k < holders.size(); ++k) {
                    ASSERT(currentThreshold == holders[k].threshold());
                }
            }
            mX.removeAllRules();
        }

        if (verbose) {
            cout << "\tTest updating a large number of categories with a rule."
                 << endl;
        }
        {

            Obj mX(&ta);

            // Adding a rule with  4K+ categories; DRQS 175529764
            for (int j = 0; j < 4096*2; ++j) {
                bsl::stringstream categoryPattern;
                categoryPattern << "Category" << j;

                bsl::string categoryName = categoryPattern.str();

                const Entry *C = mX.addCategory(categoryName.c_str(),
                                                255, 255, 255, 255);
                ASSERT(C == mX.lookupCategory(categoryName.c_str()));
            }

            ball::Rule rule("C", 1, 2, 3, 4);
            ASSERT(1 == mX.addRule(rule));

            mX.removeAllRules();
        }
      } break;
      case 11: {
        // --------------------------------------------------------------------
        // TESTING FUNCTIONS TAKING A HOLDER
        //
        // Concerns:
        //   The `addCategory` function taking a ball::CategoryHolder object
        //   works exactly as the `addCategory` without the
        //   ball_CategoryHolder.
        //
        // Plan
        //   Construct a category manager.  Add some categories with varied
        //   names and threshold levels, passing a category holder object.
        //   Verify that the returned category object is correct and also
        //   check that the passed in holder object is correctly populated.
        //   Use the `lookupCategory` function, again passing it the holder
        //   object, to get back the same category information.  Verify that
        //   the returned category object and the holder object are correct.
        //   For some of the data values confirm that passing a null holder
        //   also works.
        //
        // Testing:
        //   ball::Category *addCategory(Holder *, name, int, int, int, int);
        //   ball::Category *lookupCategory(Holder *, const char *name);
        //   void resetCategoryHolders();
        // --------------------------------------------------------------------

        if (verbose) cout << "Test Functions taking a holder\n"
                             "==============================\n";

        TestAllocator da(veryVeryVerbose); const TestAllocator& DA = da;
        TestAllocator ta(veryVeryVerbose); const TestAllocator& TA = ta;
        DefaultAllocGuard guard(&da);
        bsls::Types::Int64 numBytes = TA.numBytesInUse();

      BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
        Obj mX(&ta);  const Obj& X = mX;
        const int UC = Holder::e_UNINITIALIZED_CATEGORY;
        {
            Holder mH; const Holder& H = mH;
            mH.reset();
            Entry *entry = mX.lookupCategory(&mH, "dummy");
            ASSERT(0        == DA.numBytesInUse());
            ASSERT(numBytes <  TA.numBytesInUse());
            ASSERT(0        == entry);
            ASSERT(UC       == H.threshold());
            ASSERT(0        == H.category());
            ASSERT(0        == H.next());
        }

        ASSERT(0        == DA.numBytesInUse());
        ASSERT(numBytes <  TA.numBytesInUse());
        Holder mH; const Holder& H = mH;
        Holder mG; const Holder& G = mG;
        mH.reset();
        mG.reset();
        int THRESHOLD = calculateThreshold(LA[0], LA[1], LA[2],LA[3]);
        numBytes = TA.numBytesInUse();
        const Entry *pa = mX.addCategory(&mH, VA, LA[0], LA[1], LA[2], LA[3]);
        ASSERT(0         == DA.numBytesInUse());
        ASSERT(numBytes  <  TA.numBytesInUse());
        ASSERT(1         == X.length());
        ASSERT(0         == strcmp(VA, pa->categoryName()));
        ASSERT(LA[0]     == pa->recordLevel());
        ASSERT(LA[1]     == pa->passLevel());
        ASSERT(LA[2]     == pa->triggerLevel());
        ASSERT(LA[3]     == pa->triggerAllLevel());
        ASSERT(pa        == X.lookupCategory(VA));
        ASSERT(pa        == mX.lookupCategory(&mG, VA));
        ASSERT(THRESHOLD == H.threshold());
        ASSERT(pa        == H.category());
        ASSERT(0         == H.next());
        ASSERT(THRESHOLD == G.threshold());
        ASSERT(pa        == G.category());
        ASSERT(&H        == G.next());

        THRESHOLD = calculateThreshold(LB[0], LB[1], LB[2], LB[3]);
        numBytes  = TA.numBytesInUse();
        mH.reset();
        mG.reset();
        const Entry *pb = mX.addCategory(&mH, VB, LB[0], LB[1], LB[2], LB[3]);
        ASSERT(0         == DA.numBytesInUse());
        ASSERT(numBytes  <  TA.numBytesInUse());
        ASSERT(2         == X.length());
        ASSERT(0         == strcmp(VB, pb->categoryName()));
        ASSERT(LB[0]     == pb->recordLevel());
        ASSERT(LB[1]     == pb->passLevel());
        ASSERT(LB[2]     == pb->triggerLevel());
        ASSERT(LB[3]     == pb->triggerAllLevel());
        ASSERT(pb        == X.lookupCategory(VB));
        ASSERT(pb        == mX.lookupCategory(&mG, VB));
        ASSERT(THRESHOLD == H.threshold());
        ASSERT(pb        == H.category());
        ASSERT(0         == H.next());
        ASSERT(THRESHOLD == G.threshold());
        ASSERT(pb        == G.category());
        ASSERT(&H        == G.next());

        THRESHOLD = calculateThreshold(LC[0], LC[1], LC[2], LC[3]);
        numBytes  = TA.numBytesInUse();
        mH.reset();
        mG.reset();
        const Entry *pc = mX.addCategory(&mH, VC, LC[0], LC[1], LC[2], LC[3]);
        ASSERT(0         == DA.numBytesInUse());
        ASSERT(numBytes  <  TA.numBytesInUse());
        ASSERT(3         == X.length());
        ASSERT(0         == strcmp(VC, pc->categoryName()));
        ASSERT(LC[0]     == pc->recordLevel());
        ASSERT(LC[1]     == pc->passLevel());
        ASSERT(LC[2]     == pc->triggerLevel());
        ASSERT(LC[3]     == pc->triggerAllLevel());
        ASSERT(pc        == X.lookupCategory(VC));
        ASSERT(pc        == mX.lookupCategory(&mG, VC));
        ASSERT(THRESHOLD == H.threshold());
        ASSERT(pc        == H.category());
        ASSERT(0         == H.next());
        ASSERT(THRESHOLD == G.threshold());
        ASSERT(pc        == G.category());
        ASSERT(&H        == G.next());

        THRESHOLD = calculateThreshold(LD[0], LD[1], LD[2], LD[3]);
        numBytes  = TA.numBytesInUse();
        mH.reset();
        mG.reset();
        const Entry *pd = mX.addCategory(&mH, VD, LD[0], LD[1], LD[2], LD[3]);
        ASSERT(0         == DA.numBytesInUse());
        ASSERT(numBytes  <  TA.numBytesInUse());
        ASSERT(4         == X.length());
        ASSERT(0         == strcmp(VD, pd->categoryName()));
        ASSERT(LD[0]     == pd->recordLevel());
        ASSERT(LD[1]     == pd->passLevel());
        ASSERT(LD[2]     == pd->triggerLevel());
        ASSERT(LD[3]     == pd->triggerAllLevel());
        ASSERT(pd        == X.lookupCategory(VD));
        ASSERT(pd        == mX.lookupCategory(&mG, VD));
        ASSERT(THRESHOLD == H.threshold());
        ASSERT(pd        == H.category());
        ASSERT(0         == H.next());
        ASSERT(THRESHOLD == G.threshold());
        ASSERT(pd        == G.category());
        ASSERT(&H        == G.next());

        THRESHOLD = calculateThreshold(LE[0], LE[1], LE[2], LE[3]);
        numBytes  = TA.numBytesInUse();
        mH.reset();
        mG.reset();
        const Entry *pe = mX.addCategory(&mH, VE, LE[0], LE[1], LE[2], LE[3]);
        ASSERT(0         == DA.numBytesInUse());
        ASSERT(numBytes  <  TA.numBytesInUse());
        ASSERT(5         == X.length());
        ASSERT(0         == strcmp(VE, pe->categoryName()));
        ASSERT(LE[0]     == pe->recordLevel());
        ASSERT(LE[1]     == pe->passLevel());
        ASSERT(LE[2]     == pe->triggerLevel());
        ASSERT(LE[3]     == pe->triggerAllLevel());
        ASSERT(pe        == X.lookupCategory(VE));
        ASSERT(pe        == mX.lookupCategory(&mG, VE));
        ASSERT(THRESHOLD == H.threshold());
        ASSERT(pe        == H.category());
        ASSERT(0         == H.next());
        ASSERT(THRESHOLD == G.threshold());
        ASSERT(pe        == G.category());
        ASSERT(&H        == G.next());

        // Test passing NULL Holder to `addCategory`
        THRESHOLD = calculateThreshold(LF[0], LF[1], LF[2], LF[3]);
        numBytes  = TA.numBytesInUse();
        mH.reset();
        mG.reset();
        const Entry *pf = mX.addCategory(0, VF, LF[0], LF[1], LF[2], LF[3]);
        ASSERT(0         == DA.numBytesInUse());
        ASSERT(numBytes  <  TA.numBytesInUse());
        ASSERT(6         == X.length());
        ASSERT(0         == strcmp(VF, pf->categoryName()));
        ASSERT(LF[0]     == pf->recordLevel());
        ASSERT(LF[1]     == pf->passLevel());
        ASSERT(LF[2]     == pf->triggerLevel());
        ASSERT(LF[3]     == pf->triggerAllLevel());
        ASSERT(pf        == X.lookupCategory(VF));
        ASSERT(pf        == mX.lookupCategory(&mG, VF));
        ASSERT(THRESHOLD == G.threshold());
        ASSERT(pf        == G.category());
        ASSERT(0         == G.next());


        // Test passing NULL Holder to `lookupCategory`
        THRESHOLD = calculateThreshold(LG[0], LG[1], LG[2], LG[3]);
        numBytes  = TA.numBytesInUse();
        mH.reset();
        mG.reset();
        const Entry *pg = mX.addCategory(&mH, VG, LG[0], LG[1], LG[2], LG[3]);
        ASSERT(0         == DA.numBytesInUse());
        ASSERT(numBytes  <  TA.numBytesInUse());
        ASSERT(7         == X.length());
        ASSERT(0         == strcmp(VG, pg->categoryName()));
        ASSERT(LG[0]     == pg->recordLevel());
        ASSERT(LG[1]     == pg->passLevel());
        ASSERT(LG[2]     == pg->triggerLevel());
        ASSERT(LG[3]     == pg->triggerAllLevel());
        ASSERT(pg        == X.lookupCategory(VG));
        ASSERT(pg        == mX.lookupCategory(0, VG));
        ASSERT(THRESHOLD == H.threshold());
        ASSERT(pg        == H.category());
        ASSERT(0         == H.next());

        mX.resetCategoryHolders();
        mG.reset();
        THRESHOLD = calculateThreshold(LA[0], LA[1], LA[2], LA[3]);
        ASSERT(pa        == mX.lookupCategory(&mG, VA));
        ASSERT(THRESHOLD == G.threshold());
        ASSERT(pa        == G.category());
        ASSERT(0         == G.next());

        mG.reset();
        THRESHOLD = calculateThreshold(LB[0], LB[1], LB[2], LB[3]);
        ASSERT(pb        == mX.lookupCategory(&mG, VB));
        ASSERT(THRESHOLD == G.threshold());
        ASSERT(pb        == G.category());
        ASSERT(0         == G.next());

        mG.reset();
        THRESHOLD = calculateThreshold(LC[0], LC[1], LC[2], LC[3]);
        ASSERT(pc        == mX.lookupCategory(&mG, VC));
        ASSERT(THRESHOLD == G.threshold());
        ASSERT(pc        == G.category());
        ASSERT(0         == G.next());

        mG.reset();
        THRESHOLD = calculateThreshold(LD[0], LD[1], LD[2], LD[3]);
        ASSERT(pd        == mX.lookupCategory(&mG, VD));
        ASSERT(THRESHOLD == G.threshold());
        ASSERT(pd        == G.category());
        ASSERT(0         == G.next());

        mG.reset();
        THRESHOLD = calculateThreshold(LE[0], LE[1], LE[2], LE[3]);
        ASSERT(pe        == mX.lookupCategory(&mG, VE));
        ASSERT(THRESHOLD == G.threshold());
        ASSERT(pe        == G.category());
        ASSERT(0         == G.next());

        mG.reset();
        THRESHOLD = calculateThreshold(LF[0], LF[1], LF[2], LF[3]);
        ASSERT(pf        == mX.lookupCategory(&mG, VF));
        ASSERT(THRESHOLD == G.threshold());
        ASSERT(pf        == G.category());
        ASSERT(0         == G.next());

        mG.reset();
        THRESHOLD = calculateThreshold(LG[0], LG[1], LG[2], LG[3]);
        ASSERT(pg        == mX.lookupCategory(&mG, VG));
        ASSERT(THRESHOLD == G.threshold());
        ASSERT(pg        == G.category());
        ASSERT(0         == G.next());
      } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
      } break;
      case 10: {
        // --------------------------------------------------------------------
        // TESTING ball::CategoryHolder
        //
        // Concerns:
        //   The basic concern is that the manipulators:
        //     - void reset();
        //     - void setCategory(const ball::Category *category);
        //     - void setThreshold(int threshold);
        //     - void setNext(ball::CategoryHolder *holder);
        //   and the accessors:
        //     - const ball::Category *category() const;
        //     - int threshold() const;
        //     - ball::CategoryHolder *next() const;
        //   operate as expected.
        //
        // Plan:
        //   Specify a large set of data values that can be used to construct
        //   a ball::CategoryHolder object.  Statically initialize an object X
        //   using one of the specified data values.  Using the accessor
        //   functions verify that X contains the expected data.  Then use the
        //   `reset` function to put X into its default state.  Finally, use
        //   the manipulator functions to re-assign the same data value to X.
        //   Confirm the state of X using the accessors.
        //
        // Testing:
        //   void reset();
        //   void setCategory(const ball::Category *category);
        //   void setThreshold(int threshold);
        //   void setNext(ball::CategoryHolder *holder);
        //   const ball::Category *category() const;
        //   int threshold() const;
        //   ball::CategoryHolder *next() const;
        // --------------------------------------------------------------------

        if (verbose) cout << "Test ball::CategoryHolder\n"
                             "=========================\n";

        static const struct {
            int         d_line;            // line number
            int         d_recordLevel;     // record level
            int         d_passLevel;       // pass level
            int         d_triggerLevel;    // trigger level
            int         d_triggerAllLevel; // trigger all level
            const char *d_name_p;          // category name
            Holder     *d_next_p;          // pointer to the next holder
        } DATA[] = {
     // line    record    pass     trigger  triggerAll name     next
     // no.     level     level     level     level
     // ----    ------    ------    ------    -----    ------   ----
     {  L_,  0,        0,        0,        0,       "",        (Holder*)  0 },
     {  L_,  1,        0,        0,        0,       "A",       (Holder*)  0 },
     {  L_,  0,        1,        0,        0,       "A",       (Holder*)  0 },
     {  L_,  0,        0,        1,        0,       "AA",      (Holder*)  0 },
     {  L_,  0,        0,        0,        1,       "AA",      (Holder*)  0 },
     {  L_,  16,       32,       48,       64,      "AB",      (Holder*)  0 },
     {  L_,  64,       48,       32,       16,      "AC",      (Holder*)  0 },
     {  L_,  16,       32,       64,       48,      "AD",      (Holder*)  0 },
     {  L_,  16,       48,       32,       64,      "AAA",     (Holder*) 32 },
     {  L_,  32,       16,       48,       64,      "ABC",     (Holder*) 32 },
     {  L_,  255,      0,        0,        0,       "ABCD",    (Holder*) 32 },
     {  L_,  0,        255,      0,        0,       "ABCD",    (Holder*) 32 },
     {  L_,  0,        0,        255,      0,       "DEFG",    (Holder*) 32 },
     {  L_,  0,        0,        0,        255,     "HIJK",    (Holder*) 32 },
     {  L_,  255,      255,      255,      255,     "ALL",     (Holder*) 32 },
     {  L_,  256,      0,        0,        0,       "ALL.FS",  (Holder*) 32 },
     {  L_,  0,        256,      0,        0,       "ALL.TCP", (Holder*) 32 },
     {  L_,  0,        0,        256,      0,       "ALL.1.2", (Holder*) 32 },
     {  L_,  0,        0,        0,        256,     "BALL",    (Holder*) 64 },
     {  L_,  256,      256,      256,      256,     "BALL",    (Holder*) 64 },
     {  L_,  INT_MAX,  0,        0,        0,       "BALL.LOG",(Holder*) 64 },
     {  L_,  0,        INT_MAX,  0,        0,       "BALL_LOG",(Holder*) 64 },
     {  L_,  0,        0,        INT_MAX,  0,       "B123",    (Holder*) 64 },
     {  L_,  0,        0,        0,        INT_MAX, "12345",   (Holder*) 64 },
     {  L_,  INT_MAX,  INT_MAX,  INT_MAX,  INT_MAX, "BDE",     (Holder*) 64 },
     {  L_,  -1,       0,        0,        0,       "BDECS",   (Holder*) 256},
     {  L_,  0,        -1,       0,        0,       "BDE.CS",  (Holder*) 256},
     {  L_,  0,        0,        -1,       0,       "BALL",    (Holder*) 256},
     {  L_,  0,        0,        0,        -1,      "XYZ",     (Holder*) 256},
     {  L_,  -1,       -1,       -1,       -1,      "WXZY",    (Holder*) 256},
     {  L_,  INT_MIN,  0,        0,        0,       "7902",    (Holder*) 256},
     {  L_,  0,        INT_MIN,  0,        0,       "ABCD",    (Holder*) 256},
     {  L_,  0,        0,        INT_MIN,  0,       "DEFG",    (Holder*) 1024},
     {  L_,  0,        0,        0,        INT_MIN, "HIJK",    (Holder*) 1024},
     {  L_,  INT_MIN,  INT_MIN,  INT_MIN,  INT_MIN, "1234",    (Holder*) 1024}
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        Obj manager;

        for (int i = 0; i < NUM_DATA; ++i) {
            const int     LINE        = DATA[i].d_line;
            const int     RECORD      = DATA[i].d_recordLevel;
            const int     PASS        = DATA[i].d_passLevel;
            const int     TRIGGER     = DATA[i].d_triggerLevel;
            const int     TRIGGER_ALL = DATA[i].d_triggerAllLevel;
            const char   *NAME        = DATA[i].d_name_p;
            Holder       *NEXT        = DATA[i].d_next_p;

            const Entry *CATEGORY = manager.addCategory(NAME,
                                                        RECORD,
                                                        PASS,
                                                        TRIGGER,
                                                        TRIGGER_ALL);

            bslma::DefaultAllocatorGuard guard(&testAllocator);
            const bsls::Types::Int64 NUM_BYTES = testAllocator.numBytesInUse();

            Holder mX = {
                { Holder::e_DYNAMIC_CATEGORY },
                { const_cast<Entry *>(CATEGORY) },
                { NEXT }
            };
            const Holder& X = mX;
            LOOP3_ASSERT(LINE, Holder::e_DYNAMIC_CATEGORY, X.threshold(),
                         Holder::e_DYNAMIC_CATEGORY == X.threshold());
            LOOP3_ASSERT(LINE, CATEGORY, X.category(),
                         CATEGORY == X.category());
            LOOP3_ASSERT(LINE, NEXT, X.next(), NEXT == X.next());

            mX.reset();
            LOOP2_ASSERT(LINE, X.threshold(),
                         Holder::e_UNINITIALIZED_CATEGORY == X.threshold());
            LOOP2_ASSERT(LINE, X.category(), 0 == X.category());
            LOOP2_ASSERT(LINE, X.next(),     0 == X.next());

            mX.setThreshold(TRIGGER_ALL);
            LOOP3_ASSERT(LINE, TRIGGER_ALL, X.threshold(),
                         TRIGGER_ALL == X.threshold());

            mX.setCategory(CATEGORY);
            LOOP3_ASSERT(LINE, CATEGORY, X.category(),
                         CATEGORY == X.category());

            mX.setNext(NEXT);
            LOOP3_ASSERT(LINE, NEXT, X.next(), NEXT == X.next());

            LOOP3_ASSERT(LINE, NUM_BYTES, testAllocator.numBytesInUse(),
                         NUM_BYTES == testAllocator.numBytesInUse());
        }
      } break;
      case 9: {
        // --------------------------------------------------------------------
        // TESTING: `addRules`, `removeRules`
        //
        // Concerns:  That `addRules` and `removeRules` correctly update the
        //   category manager's rule set and the relevant rule masks for the
        //   managed categories, and return the correct value.
        //
        // Plan:
        //   For each possible subset of a set of categories, create a rule
        //   for each category in the subset and add it to a rule-set.  Add the
        //   set of rules to the category manager and verify: (1) the return
        //   value for `addRules` is the number of rules (2) the sequence
        //   number is updated (3) the category manager's rule set contains
        //   the newly added rules, and (4) that the relevant rule bit mask
        //   for each category is correct.  Then, create a rule set for all
        //   categories and invoke `addRules`, verify the return value for
        //   the `addRules` method is the number of rules that were *not*
        //   originally added.  Then remove the original subset of rules, and
        //   verify the same 4 points as when we originally added the rules.
        //   Finally, remove all the rules, and verify the return value.
        //
        // Testing:
        //   int addRules(const ball::RuleSet& ruleSet);
        //   int removeRules(const ball::RuleSet& ruleSet);
        // --------------------------------------------------------------------

        if (verbose) cout << "Test `addRules`, `removeRules`\n"
                             "==============================\n";

        bslma::TestAllocator ta;
        Obj mX(&ta);  const Obj& X = mX;

        for (int i = 0; i < NUM_NAMES; ++i) {
            mX.addCategory(NAMES[i], LEVELS[i][0], LEVELS[i][1],
                                     LEVELS[i][2], LEVELS[i][3]);
        }

        // Iterate over each possible set of rules.
        MaskType endMask = ~(static_cast<MaskType>(~0) << NUM_NAMES);

        for (MaskType mask = 0; mask <= endMask; ++mask) {

            if (veryVerbose) { T_ P(mask) }

            Int64 seqNo = X.ruleSetSequenceNumber();
            ball::RuleSet rules(&ta);
            for (int i= 0; i < NUM_NAMES; ++i) {
                if (bdlb::BitUtil::isBitSet(mask, i)) {
                    ball::Rule rule(NAMES[i], LEVELS[i][0], LEVELS[i][1],
                                              LEVELS[i][2], LEVELS[i][3]);
                    rules.addRule(rule);
                }
            }

            // Add the subset of rules indicated by the bitmask `mask`.
            ASSERT(rules.numRules()                == mX.addRules(rules));
            ASSERT(bdlb::BitUtil::numBitsSet(mask) == X.ruleSet().numRules());
            if (mask != 0) {
                ASSERT(seqNo < X.ruleSetSequenceNumber());
                seqNo = X.ruleSetSequenceNumber();
            }

            ASSERT(0     == mX.addRules(rules));
            ASSERT(seqNo == X.ruleSetSequenceNumber());

            // Verify the relevant rule masks for each category.
            for (int i = 0; i < NUM_NAMES; ++i) {
                if (bdlb::BitUtil::isBitSet(mask, i)) {
                    ball::Rule rule(NAMES[i], LEVELS[i][0], LEVELS[i][1],
                                              LEVELS[i][2], LEVELS[i][3]);
                    int ruleId = X.ruleSet().ruleId(rule);
                    ASSERT(0 <= ruleId);
                    for (int j = 0; j < NUM_NAMES; ++j) {
                        bool  isSet = i == j;
                        const Entry *cat = X.lookupCategory(NAMES[j]);
                        ASSERT(isSet == bdlb::BitUtil::isBitSet(
                                                   cat->relevantRuleMask(),
                                                   ruleId));
                    }
                }
            }

            ball::RuleSet allRules(&ta);
            for (int i = 0; i < NUM_NAMES; ++i) {
                ball::Rule rule(NAMES[i], LEVELS[i][0], LEVELS[i][1],
                                          LEVELS[i][2], LEVELS[i][3]);
                allRules.addRule(rule);
            }

            // Add all the rules.  Note that the number of rules actually
            // added should be only those rules not belonging to the subset of
            // rules indicated by the bitmask `mask`.
            ASSERT(bdlb::BitUtil::numBitsSet(endMask & ~mask) ==
                                                        mX.addRules(allRules));
            ASSERT(bdlb::BitUtil::numBitsSet(endMask) ==
                                                      mX.ruleSet().numRules());
            ASSERT(0 == mX.addRules(allRules));

            seqNo = X.ruleSetSequenceNumber();

            // Remove those rules indicated by the bitmask `mask`.  Note that
            // the category manager should have a rule for every category.
            ASSERT(bdlb::BitUtil::numBitsSet(mask) == mX.removeRules(rules));
            ASSERT(bdlb::BitUtil::numBitsSet(~(~endMask | mask)) ==
                                                      X.ruleSet().numRules());

            if (mask != 0) {
                ASSERT(seqNo < X.ruleSetSequenceNumber());
            }
            ASSERT(0 == mX.removeRules(rules));

            // Verify the relevant rule bit mask for each category after we
            // have removed the rules.
            for (int i = 0; i < NUM_NAMES; ++i) {
                if (!bdlb::BitUtil::isBitSet(mask, i)) {
                    ball::Rule rule(NAMES[i], LEVELS[i][0], LEVELS[i][1],
                                              LEVELS[i][2], LEVELS[i][3]);
                    int ruleId = X.ruleSet().ruleId(rule);
                    ASSERT(0 <= ruleId);
                    for (int j = 0; j < NUM_NAMES; ++j) {
                        bool  isSet = i == j;
                        const Entry *cat = X.lookupCategory(NAMES[j]);
                        ASSERT(isSet == bdlb::BitUtil::isBitSet(
                                                   cat->relevantRuleMask(),
                                                   ruleId));
                    }
                }
            }

            // Remove all the rules.  Note that this removes the remaining
            // rules, i.e., those rules *not* indicated by the bitmask `mask`.
            ASSERT(bdlb::BitUtil::numBitsSet(endMask & ~mask) ==
                                                     mX.removeRules(allRules));
            ASSERT(0 == X.ruleSet().numRules());
        }
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // TESTING `addRule`, `removeRule`, `removeAllRules`
        //
        // Concerns:
        //   `addRule`, `removeRule`, `removeAllRules` update the set of rules
        //   and the relevant rule masks for the affected ball::Category
        //   objects.
        //
        // Plan:
        //    For each category, in a set of test categories, create a rule
        //    and verify: (1) the return value for the `addRule` method, (2)
        //    that the rule has been added to the category manager's set of
        //    rules (3) that the sequence number has been updated, and (4) that
        //    relevant rule masks for the categories is correct.  Create a
        //    second rule, for the same category (with an additional
        //    predicate), and add it to the category manager.  Verify the same
        //    4 points.  Finally remove the second rule, and verify the same 4
        //    points.
        //
        // Testing:
        //   int addRule(const ball::Rule& rule);
        //   int removeRule(const ball::Rule& rule);
        //   void removeAllRules();
        //   bsls::Types::Int64 ruleSetSequenceNumber() const;
        // --------------------------------------------------------------------

        if (verbose)
            cout << "TESTING `addRule`, `removeRule`, `removeAllRules`\n"
                    "=================================================\n";

        bslma::TestAllocator ta;

        Obj mX(&ta);  const Obj& X = mX;

        Int64 seqNo = X.ruleSetSequenceNumber();

        for (int i = 0; i < NUM_NAMES; ++i) {
            mX.addCategory(NAMES[i], LEVELS[i][0], LEVELS[i][1],
                                     LEVELS[i][2], LEVELS[i][3]);
        }
        ASSERT(seqNo == X.ruleSetSequenceNumber());

        for (int i = 0; i < NUM_NAMES; ++i) {
            ball::Rule rule1(NAMES[i], LEVELS[i][0], LEVELS[i][1],
                                       LEVELS[i][2], LEVELS[i][3]);

            // Add a rule for this category.
            ASSERT(1 == mX.addRule(rule1));
            ASSERT(0 == mX.addRule(rule1));
            ASSERT(seqNo < X.ruleSetSequenceNumber());
            seqNo = X.ruleSetSequenceNumber();

            int ruleId1 = X.ruleSet().ruleId(rule1);
            ASSERT(0 <= ruleId1);

            for (int j = 0; j < NUM_NAMES; ++j) {
                bool  isSet = i == j;
                const Entry *cat = X.lookupCategory(NAMES[j]);
                ASSERT(isSet == bdlb::BitUtil::isBitSet(
                                                       cat->relevantRuleMask(),
                                                       ruleId1));
            }

            ball::Rule rule2(NAMES[i], LEVELS[i][0], LEVELS[i][1],
                                       LEVELS[i][2], LEVELS[i][3]);
            ball::Predicate p2("A", 1);
            rule2.addPredicate(p2);

            // Add a second rule for this category.
            ASSERT(1 == mX.addRule(rule2));
            ASSERT(0 == mX.addRule(rule2));
            ASSERT(seqNo < X.ruleSetSequenceNumber());
            seqNo = X.ruleSetSequenceNumber();

            int ruleId2 = X.ruleSet().ruleId(rule2);
            ASSERT(0 <= ruleId2)
            for (int j = 0; j < NUM_NAMES; ++j) {
                bool isSet = i == j;
                const Entry *cat = X.lookupCategory(NAMES[j]);
                ASSERT(isSet == bdlb::BitUtil::isBitSet(
                                                       cat->relevantRuleMask(),
                                                       ruleId1));
                ASSERT(isSet == bdlb::BitUtil::isBitSet(
                                                       cat->relevantRuleMask(),
                                                       ruleId2));
            }

            // Remove the second rule for this category.
            ASSERT(1 == mX.removeRule(rule2));
            ASSERT(0 == mX.removeRule(rule2));
            ASSERT(seqNo < X.ruleSetSequenceNumber());
            seqNo = X.ruleSetSequenceNumber();

            for (int j = 0; j < NUM_NAMES; ++j) {
                bool isSet = i == j;
                const Entry *cat = X.lookupCategory(NAMES[j]);
                ASSERT(isSet == bdlb::BitUtil::isBitSet(
                                                       cat->relevantRuleMask(),
                                                       ruleId1));
                ASSERT(false == bdlb::BitUtil::isBitSet(
                                                       cat->relevantRuleMask(),
                                                       ruleId2));
            }
        }

        // Remove all the rules.
        mX.removeAllRules();
        ASSERT(seqNo < X.ruleSetSequenceNumber());
        for (int i = 0; i < NUM_NAMES; ++i) {
            const Entry *cat = X.lookupCategory(NAMES[i]);
            ASSERT(0 == cat->relevantRuleMask());
        }
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING MT-SAFETY: ADD AND LOOKUP
        //
        // Concerns:
        //   `addCategory` and `lookupCategory` should operate as expected in
        //   a multi-threaded environment, particularly on the same object:
        //
        //   - calls to `lookupCategory` can occur concurrently
        //   - calls to `addCategory` have transactional integrity
        //   - successful calls to `lookupCategory` refer to the same object
        //
        // Plan:
        //   Create a category manager `mX`.  In several "writer" threads, add
        //   categories to `mX` having various names and threshold levels.  In
        //   several other threads, query `mX` for each category until all
        //   queries are satisfied.  The result of each call to `addCategory`
        //   and `lookupCategory` is stored in a container associated with the
        //   calling thread.  When all threads have completed, the values in
        //   these containers are validated to ensure that
        //
        //   - All categories added via `addCategory` are unique across all
        //     "write" threads.
        //   - The lists of categories queried via `lookupCategory` are
        //     identical across all "query" threads.
        //
        // Testing:
        //   MT-SAFETY
        // --------------------------------------------------------------------

        if (verbose) cout << "Test MT-Safety: Add and Lookup\n"
                             "==============================\n";

        enum {
            NUM_CATEGORIES = 100,    // number of categories to add
            NUM_W_THREADS  = 4,      // number of "write" threads
            NUM_Q_THREADS  = 16,     // number of "query" threads
            NUM_THREADS    = NUM_W_THREADS + NUM_Q_THREADS
        };

        // Generate `ball::Category` names.
        bsl::string categoryNames[NUM_CATEGORIES];
        for (int i = 0; i < NUM_CATEGORIES; ++i) {
            categoryNames[i] = bitset2string(bsl::bitset<32>(i));
        }

        bslma::TestAllocator ta(veryVeryVerbose);
        Obj                  mX(&ta);
        bslmt::Barrier       barrier(NUM_THREADS);

        struct {
            bslmt::ThreadUtil::Handle d_handle;   // thread handle
            my_ListType               d_results;  // container for results
            my_ThreadParameters       d_params;   // bundled thread parameters
        } threads[NUM_THREADS];

        // Create threads.
        for (int i = 0; i < NUM_THREADS; ++i) {
            threads[i].d_results.reserve(NUM_CATEGORIES);
            threads[i].d_params.d_results_p = &threads[i].d_results;
            threads[i].d_params.d_cm_p      = &mX;
            threads[i].d_params.d_names_p   = categoryNames;
            threads[i].d_params.d_size      = NUM_CATEGORIES;
            threads[i].d_params.d_barrier_p = &barrier;
            bslmt::ThreadUtil::ThreadFunction action = (i < NUM_W_THREADS)
                                                       ? case9ThreadW
                                                       : case9ThreadQ;
            ASSERT(0 == bslmt::ThreadUtil::create(&threads[i].d_handle,
                                                  action,
                                                  &threads[i].d_params));
        }

        for (int i = 0; i < NUM_THREADS; ++i) {
            ASSERT(0 == bslmt::ThreadUtil::join(threads[i].d_handle));
        }

        // Verify the number of categories added to the category manager.
        int totalAddedCategories = 0;
        for (int i = 0; i < NUM_W_THREADS; ++i) {
            totalAddedCategories +=
                                 static_cast<int>(threads[i].d_results.size());
            if (veryVeryVerbose) {
                T_;
                P_(i);
                #if !defined(BSLS_PLATFORM_CMP_MSVC)
                P_(threads[i].d_handle);
                #else
                P_(threads[i].d_handle.d_handle);
                #endif
                P_(threads[i].d_results.size());
                P(totalAddedCategories);
            }
        }
        ASSERT(NUM_CATEGORIES == mX.length());
        ASSERT(NUM_CATEGORIES == totalAddedCategories);

        // Merge "write" threads' results into `results`.
        my_ListType results;
        for (int i = 0; i < NUM_W_THREADS; ++i) {
            my_ListType&   list = threads[i].d_results;
            results.insert(results.end(), list.begin(), list.end());
        }
        bsl::sort(results.begin(), results.end());
        if (veryVerbose) {
            T_; P(results.size());
        }
        ASSERT(NUM_CATEGORIES == results.size());

        // Validate "query" threads' results against `results`.
        for (int i = NUM_W_THREADS; i < NUM_THREADS; ++i) {
            if (veryVerbose) {
                T_;
                P_(i);
                #if !defined(BSLS_PLATFORM_CMP_MSVC)
                P_(threads[i].d_handle);
                #else
                P_(threads[i].d_handle.d_handle);
                #endif
                P(threads[i].d_results.size());
            }
            LOOP2_ASSERT(i, threads[i].d_results.size(),
                         results == threads[i].d_results);
        }

        ASSERT(0 < ta.numAllocations());
        ASSERT(0 < ta.numBytesInUse());
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING INDEXED ACCESS
        //
        // Concerns:
        //   The basic concern is that the indexed access operators work
        //   as expected.
        //
        // Plan:
        //   Create a modifiable `ball::CategoryManager` object, `mX`, and a
        //   non-modifiable reference to `mX` named `X`.  Add categories to
        //   `mX` having various names and threshold level values.  Iterate
        //   over `mX`, using the non-`const` index `operator` to change the
        //   threshold level values on `ball::Category` objects obtained from
        //   `mX`.  Verify that the values are changed using const
        //   `ball::Category` objects obtained from `X`.  Change the threshold
        //   levels back to their original values using the const index
        //   operator on `mX`, and verify that they were reset, again using
        //   `X`.
        //
        // Testing:
        //   ball::Category& operator[](int index);
        //   const ball::Category& operator[](int index) const;
        // --------------------------------------------------------------------

        if (verbose) cout << "Test Indexed Access\n"
                             "===================\n";

        bslma::TestAllocator ta;

        Obj mX(&ta);  const Obj& X = mX;

        for (int i = 0; i < NUM_NAMES; ++i) {
            mX.addCategory(NAMES[i], LEVELS[i][0], LEVELS[i][1],
                                     LEVELS[i][2], LEVELS[i][3]);
        }
        ASSERT(NUM_NAMES == X.length());

        for (int i = 0; i < X.length(); ++i) {
            const ball::Category& category = X[i];
            const Entry *p          = X.lookupCategory(X[i].categoryName());
            const int    record     = p->recordLevel();
            const int    pass       = p->passLevel();
            const int    trigger    = p->triggerLevel();
            const int    triggerAll = p->triggerAllLevel();
            ASSERT(record     == category.recordLevel());
            ASSERT(pass       == category.passLevel());
            ASSERT(trigger    == category.triggerLevel());
            ASSERT(triggerAll == category.triggerAllLevel());

            mX[i].setLevels(category.recordLevel() + 1,
                            category.passLevel() + 1,
                            category.triggerLevel() + 1,
                            category.triggerAllLevel() + 1);

            ASSERT(record + 1     == p->recordLevel());
            ASSERT(pass + 1       == p->passLevel());
            ASSERT(trigger + 1    == p->triggerLevel());
            ASSERT(triggerAll + 1 == p->triggerAllLevel());

            mX[i].setLevels(category.recordLevel() - 1,
                            category.passLevel() - 1,
                            category.triggerLevel() - 1,
                            category.triggerAllLevel() - 1);

            ASSERT(record     == p->recordLevel());
            ASSERT(pass       == p->passLevel());
            ASSERT(trigger    == p->triggerLevel());
            ASSERT(triggerAll == p->triggerAllLevel());
        }

        if (verbose) cout << "\tTest edge cases for operator[]" << endl;

        // Test 1: Verify first and last index access
        {
            const ball::Category& first = X[0];
            const ball::Category& last = X[X.length() - 1];
            ASSERT(0 == strcmp(NAMES[0], first.categoryName()));
            ASSERT(0 == strcmp(NAMES[NUM_NAMES - 1], last.categoryName()));

            // Verify non-const access
            ball::Category& firstNonConst = mX[0];
            ball::Category& lastNonConst = mX[X.length() - 1];
            ASSERT(&first == &firstNonConst);
            ASSERT(&last == &lastNonConst);
        }

        // Test 2: Verify all indices return unique categories
        {
            for (int i = 0; i < X.length(); ++i) {
                for (int j = i + 1; j < X.length(); ++j) {
                    ASSERT(&X[i] != &X[j]);
                    ASSERT(0 != strcmp(X[i].categoryName(),
                                       X[j].categoryName()));
                }
            }
        }

        // Test 3: Verify operator[] and lookupCategory return same object
        {
            for (int i = 0; i < X.length(); ++i) {
                const char *name = X[i].categoryName();
                const Entry *pLookup = X.lookupCategory(name);
                const Entry *pIndexed = &X[i];
                ASSERT(pLookup == pIndexed);
            }
        }

        // Test 4: Verify modifications via operator[] are visible
        {
            const int testIndex = X.length() / 2;
            const int origRecord = X[testIndex].recordLevel();
            const int origPass = X[testIndex].passLevel();
            const int origTrigger = X[testIndex].triggerLevel();
            const int origTriggerAll = X[testIndex].triggerAllLevel();

            // Modify via non-const operator[]
            mX[testIndex].setLevels(100, 101, 102, 103);

            // Verify via const operator[]
            ASSERT(100 == X[testIndex].recordLevel());
            ASSERT(101 == X[testIndex].passLevel());
            ASSERT(102 == X[testIndex].triggerLevel());
            ASSERT(103 == X[testIndex].triggerAllLevel());

            // Verify via lookupCategory
            const Entry *p = X.lookupCategory(X[testIndex].categoryName());
            ASSERT(100 == p->recordLevel());
            ASSERT(101 == p->passLevel());
            ASSERT(102 == p->triggerLevel());
            ASSERT(103 == p->triggerAllLevel());

            // Restore original values
            mX[testIndex].setLevels(origRecord,
                                    origPass,
                                    origTrigger,
                                    origTriggerAll);
        }

        // Test 5: Verify sequential access order
        {
            bsl::vector<bsl::string> namesViaOperator;
            for (int i = 0; i < X.length(); ++i) {
                namesViaOperator.push_back(X[i].categoryName());
            }

            // Verify categories can be accessed in reverse order
            for (int i = X.length() - 1; i >= 0; --i) {
                ASSERT(namesViaOperator[i] == X[i].categoryName());
            }
        }

        // Test 6: Test with empty manager (boundary case with new manager)
        {
            Obj mY(&ta);  const Obj& Y = mY;
            ASSERT(0 == Y.length());
            // Note: Cannot test Y[0] as it would be undefined behavior
            // We verify length() correctly reports empty state
        }

        // Test 7: Test with single category (boundary case)
        {
            Obj mY(&ta);  const Obj& Y = mY;
            mY.addCategory("SingleCategory", 1, 2, 3, 4);
            ASSERT(1 == Y.length());
            ASSERT(0 == strcmp("SingleCategory",
                               Y[0].categoryName()));
            ASSERT(1 == Y[0].recordLevel());
            ASSERT(2 == Y[0].passLevel());
            ASSERT(3 == Y[0].triggerLevel());
            ASSERT(4 == Y[0].triggerAllLevel());

            // Verify modification works with single category
            mY[0].setLevels(10, 20, 30, 40);
            ASSERT(10 == Y[0].recordLevel());
            ASSERT(20 == Y[0].passLevel());
            ASSERT(30 == Y[0].triggerLevel());
            ASSERT(40 == Y[0].triggerAllLevel());
        }

        // Test 8: Verify operator[] returns reference that persists
        {
            const int testIndex = 0;
            const ball::Category& ref1 = X[testIndex];
            const ball::Category& ref2 = X[testIndex];
            ASSERT(&ref1 == &ref2);
            ASSERT(0 == strcmp(ref1.categoryName(), ref2.categoryName()));
        }

        ASSERT(0 < ta.numAllocations());
        ASSERT(0 < ta.numBytesInUse());
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING AREVALIDTHRESHOLDLEVELS AND MANIPULATORS
        //
        // Concerns:
        //   The primary concern is that the following methods behave correctly
        //   when passed various threshold level values:
        //    - areValidThresholdLevels
        //    - setLevels
        //    - addCategory
        //    - setThresholdLevels
        //
        // Plan
        //   Tabulate various threshold level values, call each of the four
        //   methods under test using the tabulated values as arguments, and
        //   assert the expected function return values and the expected
        //   side-effects.  In particular, test that `setLevels`,
        //   `addCategory`, and `setThresholdLevels` exhibit the correct
        //   behavior with respect to the modification of existing categories
        //   or the addition of new categories.  Note that it is assumed that
        //   `addCategory` was thoroughly tested in case 2 (Bootstrap) with
        //   valid threshold levels.
        //
        // Testing:
        //   static bool areValidThresholdLevels(int, int, int, int);
        //   int setLevels(int, int, int, int);
        //   ball::Category *addCategory(const char *name, int, int, int, int);
        //   ball::Category *setThresholdLevels(*name, int, int, int, int);
        // --------------------------------------------------------------------

        if (verbose) cout << "Test Manipulators\n"
                             "=================\n";

        bslma::DefaultAllocatorGuard guard(&testAllocator);

        static const struct {
            int d_line;            // line number
            int d_recordLevel;     // record level
            int d_passLevel;       // pass level
            int d_triggerLevel;    // trigger level
            int d_triggerAllLevel; // trigger all level
            int d_valid;           // expected result
        } DATA[] = {
            // line    record    pass     trigger  triggerAll expected
            // no.     level     level     level     level    value
            // ----    ------    ------    ------    -----    --------
            {  L_,     0,        0,        0,        0,       1        },
            {  L_,     1,        0,        0,        0,       1        },
            {  L_,     0,        1,        0,        0,       1        },
            {  L_,     0,        0,        1,        0,       1        },
            {  L_,     0,        0,        0,        1,       1        },
            {  L_,     16,       32,       48,       64,      1        },
            {  L_,     64,       48,       32,       16,      1        },
            {  L_,     16,       32,       64,       48,      1        },
            {  L_,     16,       48,       32,       64,      1        },
            {  L_,     32,       16,       48,       64,      1        },
            {  L_,     255,      0,        0,        0,       1        },
            {  L_,     0,        255,      0,        0,       1        },
            {  L_,     0,        0,        255,      0,       1        },
            {  L_,     0,        0,        0,        255,     1        },
            {  L_,     255,      255,      255,      255,     1        },
            {  L_,     256,      0,        0,        0,       0        },
            {  L_,     0,        256,      0,        0,       0        },
            {  L_,     0,        0,        256,      0,       0        },
            {  L_,     0,        0,        0,        256,     0        },
            {  L_,     256,      256,      256,      256,     0        },
            {  L_,     INT_MAX,  0,        0,        0,       0        },
            {  L_,     0,        INT_MAX,  0,        0,       0        },
            {  L_,     0,        0,        INT_MAX,  0,       0        },
            {  L_,     0,        0,        0,        INT_MAX, 0        },
            {  L_,     INT_MAX,  INT_MAX,  INT_MAX,  INT_MAX, 0        },
            {  L_,     -1,       0,        0,        0,       0        },
            {  L_,     0,        -1,       0,        0,       0        },
            {  L_,     0,        0,        -1,       0,       0        },
            {  L_,     0,        0,        0,        -1,      0        },
            {  L_,     -1,       -1,       -1,       -1,      0        },
            {  L_,     INT_MIN,  0,        0,        0,       0        },
            {  L_,     0,        INT_MIN,  0,        0,       0        },
            {  L_,     0,        0,        INT_MIN,  0,       0        },
            {  L_,     0,        0,        0,        INT_MIN, 0        },
            {  L_,     INT_MIN,  INT_MIN,  INT_MIN,  INT_MIN, 0        }
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        Obj mX;  const Obj& X = mX;
        Entry *pCat = 0;
        BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator)
            pCat = mX.addCategory("TEST CATEGORY", 111, 112, 113, 114);
        BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

        if (veryVerbose) P(X.length());
        ASSERT(1 == X.length());

        for (int i = 0; i < NUM_DATA; ++i) {
            const int LINE             = DATA[i].d_line;
            const int RECORD_LEVEL     = DATA[i].d_recordLevel;
            const int PASS_LEVEL       = DATA[i].d_passLevel;
            const int TRIGGER_LEVEL    = DATA[i].d_triggerLevel;
            const int TRIGGERALL_LEVEL = DATA[i].d_triggerAllLevel;
            const int VALID            = DATA[i].d_valid;

            if (veryVerbose) {
                P_(LINE); P_(RECORD_LEVEL); P_(PASS_LEVEL);
                P_(TRIGGER_LEVEL); P_(TRIGGERALL_LEVEL); P(VALID);
            }

            {
                const int r = ball::Category::areValidThresholdLevels(
                                                             RECORD_LEVEL,
                                                             PASS_LEVEL,
                                                             TRIGGER_LEVEL,
                                                             TRIGGERALL_LEVEL);
                LOOP_ASSERT(LINE, VALID == r);
            }

            {
                const int r = pCat->setLevels(RECORD_LEVEL,
                                              PASS_LEVEL,
                                              TRIGGER_LEVEL,
                                              TRIGGERALL_LEVEL);
                LOOP_ASSERT(LINE, VALID == !r);

                if (VALID) {
                    ASSERT(RECORD_LEVEL     == pCat->recordLevel());
                    ASSERT(PASS_LEVEL       == pCat->passLevel());
                    ASSERT(TRIGGER_LEVEL    == pCat->triggerLevel());
                    ASSERT(TRIGGERALL_LEVEL == pCat->triggerAllLevel());

                    ASSERT(0 == pCat->setLevels(111, 112, 113, 114));  // reset
                }
                else {
                    ASSERT(111 == pCat->recordLevel());
                    ASSERT(112 == pCat->passLevel());
                    ASSERT(113 == pCat->triggerLevel());
                    ASSERT(114 == pCat->triggerAllLevel());
                }
            }

            {
                const char *name = VALID ? "TEST CATEGORY" : "SET CATEGORY";

                const Entry *pCat2 = mX.setThresholdLevels(name,
                                                           RECORD_LEVEL,
                                                           PASS_LEVEL,
                                                           TRIGGER_LEVEL,
                                                           TRIGGERALL_LEVEL);
                if (VALID) {
                    LOOP_ASSERT(LINE, pCat  == pCat2);

                    ASSERT(RECORD_LEVEL     == pCat->recordLevel());
                    ASSERT(PASS_LEVEL       == pCat->passLevel());
                    ASSERT(TRIGGER_LEVEL    == pCat->triggerLevel());
                    ASSERT(TRIGGERALL_LEVEL == pCat->triggerAllLevel());

                    ASSERT(0 == pCat->setLevels(111, 112, 113, 114));  // reset

                    char setName[] = "Setiii";
                    snprintf(setName + 3, sizeof setName - 3, "%03d", i);
                                                         // unique name for set

                    ASSERT(0 == X.lookupCategory(setName));

                    const Entry *pCat3 = 0;
                    BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator)
                        pCat3 = mX.setThresholdLevels(setName,
                                                      RECORD_LEVEL,
                                                      PASS_LEVEL,
                                                      TRIGGER_LEVEL,
                                                      TRIGGERALL_LEVEL);
                    BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
                    ASSERT(0 != pCat3);

                    ASSERT(RECORD_LEVEL     == pCat3->recordLevel());
                    ASSERT(PASS_LEVEL       == pCat3->passLevel());
                    ASSERT(TRIGGER_LEVEL    == pCat3->triggerLevel());
                    ASSERT(TRIGGERALL_LEVEL == pCat3->triggerAllLevel());
                    ASSERT(pCat3            == X.lookupCategory(setName));
                }
                else {
                    LOOP_ASSERT(LINE, 0 == pCat2);

                    ASSERT(111 == pCat->recordLevel());
                    ASSERT(112 == pCat->passLevel());
                    ASSERT(113 == pCat->triggerLevel());
                    ASSERT(114 == pCat->triggerAllLevel());
                    ASSERT(0   == X.lookupCategory(name));
                }
            }

            {
                const char *name = VALID ? "TEST CATEGORY" : "ADD CATEGORY";

                const Entry *pCat2 = mX.addCategory(name,
                                                    RECORD_LEVEL,
                                                    PASS_LEVEL,
                                                    TRIGGER_LEVEL,
                                                    TRIGGERALL_LEVEL);
                LOOP_ASSERT(LINE, 0 == pCat2);

                if (VALID) {
                    ASSERT(111 == pCat->recordLevel());
                    ASSERT(112 == pCat->passLevel());
                    ASSERT(113 == pCat->triggerLevel());
                    ASSERT(114 == pCat->triggerAllLevel());
                }
                else {
                    ASSERT(0   == X.lookupCategory(name));
                }
            }
        }
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING ISENABLED
        //
        // Concerns:
        //   The concern is that `isEnabled` returns the correct value with
        //   various threshold level settings.
        //
        // Plan
        //   Construct a category manager and add a category.  Set the
        //   threshold level settings to different values and verify that in
        //   each case `isEnabled` returns the correct value.
        //
        // Testing:
        //   bool isEnabled(int) const;
        // --------------------------------------------------------------------

        if (verbose) cout << "Test `isEnabled`\n"
                             "================\n";

        static const struct {
            int d_line;            // line number
            int d_level;           // input level
            int d_recordLevel;     // record level
            int d_passLevel;       // pass level
            int d_triggerLevel;    // trigger level
            int d_triggerAllLevel; // trigger all level
            int d_expIsEnabled;    // expected isEnabled result
        } DATA[] = {
        // line    input     record    pass     trigger  triggerAll expected
        // no.     level     level     level     level     level    value
        // ----    ------    ------    ------    ------    -----    --------
        {  L_,     0,        0,        0,        0,        0,       1        },
        {  L_,     0,        1,        1,        0,        0,       1        },
        {  L_,     0,        1,        1,        0,        0,       1        },
        {  L_,     0,        1,        1,        1,        0,       1        },
        {  L_,     0,        1,        1,        1,        1,       1        },
        {  L_,     1,        0,        0,        0,        0,       0        },
        {  L_,     1,        0,        1,        0,        0,       1        },
        {  L_,     1,        0,        0,        1,        0,       1        },
        {  L_,     1,        0,        0,        0,        1,       1        },
        {  L_,     1,        1,        0,        0,        0,       1        },
        {  L_,     1,        2,        0,        0,        0,       1        },
        {  L_,     1,        255,      0,        0,        0,       1        },
        {  L_,     1,        255,      255,      255,      255,     1        },
        {  L_,     1,        1,        1,        0,        0,       1        },
        {  L_,     1,        2,        2,        0,        0,       1        },
        {  L_,     1,        255,      255,      0,        0,       1        },
        {  L_,     1,        1,        1,        1,        0,       1        },
        {  L_,     1,        2,        2,        2,        0,       1        },
        {  L_,     1,        255,      255,      255,      0,       1        },
        {  L_,     1,        1,        1,        1,        1,       1        },
        {  L_,     1,        2,        2,        2,        2,       1        },
        {  L_,     1,        255,      255,      255,      255,     1        },
        // line    input     record    pass     trigger  triggerAll expected
        // no.     level     level     level     level     level    value
        // ----    ------    ------    ------    ------    -----    --------
        {  L_,     99,       0,        0,        0,        0,       0        },
        {  L_,     99,       1,        0,        0,        0,       0        },
        {  L_,     99,       98,       0,        0,        0,       0        },
        {  L_,     99,       0,        98,       0,        0,       0        },
        {  L_,     99,       0,        0,        98,       0,       0        },
        {  L_,     99,       0,        0,        0,        98,      0        },
        {  L_,     99,       99,       0,        0,        0,       1        },
        {  L_,     99,       0,        99,       0,        0,       1        },
        {  L_,     99,       0,        0,        99,       0,       1        },
        {  L_,     99,       0,        0,        0,        99,      1        },
        {  L_,     99,       100,      0,        0,        0,       1        },
        {  L_,     99,       0,        100,      0,        0,       1        },
        {  L_,     99,       0,        0,        100,      0,       1        },
        {  L_,     99,       0,        0,        0,        100,     1        },
        {  L_,     99,       255,      0,        0,        0,       1        },
        {  L_,     99,       1,        1,        0,        0,       0        },
        {  L_,     99,       98,       98,       0,        0,       0        },
        // line    input     record    pass     trigger  triggerAll expected
        // no.     level     level     level     level     level    value
        // ----    ------    ------    ------    ------    -----    --------
        {  L_,     99,       98,       99,       0,        0,       1        },
        {  L_,     99,       98,       100,      0,        0,       1        },
        {  L_,     99,       0,        255,      0,        0,       1        },
        {  L_,     99,       255,      255,      0,        0,       1        },
        {  L_,     99,       1,        1,        1,        0,       0        },
        {  L_,     99,       98,       98,       98,       0,       0        },
        {  L_,     99,       98,       98,       99,       0,       1        },
        {  L_,     99,       98,       99,       99,       0,       1        },
        {  L_,     99,       100,      100,      100,      0,       1        },
        {  L_,     99,       255,      255,      255,      0,       1        },
        {  L_,     99,       1,        1,        1,        1,       0        },
        {  L_,     99,       1,        1,        1,        99,      1        },
        {  L_,     99,       98,       98,       98,       98,      0        },
        {  L_,     99,       98,       98,       98,       99,      1        },
        {  L_,     99,       99,       99,       99,       99,      1        },
        {  L_,     99,       0,        100,      0,        0,       1        },
        {  L_,     99,       0,        0,        0,        100,     1        },
        {  L_,     99,       100,      100,      100,      100,     1        },
        {  L_,     99,       0,        0,        0,        255,     1        },
        {  L_,     99,       0,        0,        255,      0,       1        },
        {  L_,     99,       0,        255,      0,        0,       1        },
        {  L_,     99,       255,      255,      255,      255,     1        },
        // line    input     record    pass     trigger  triggerAll expected
        // no.     level     level     level     level     level    value
        // ----    ------    ------    ------    ------    -----    --------
        {  L_,     255,      0,        0,        0,        0,       0        },
        {  L_,     255,      1,        0,        0,        0,       0        },
        {  L_,     255,      254,      0,        0,        0,       0        },
        {  L_,     255,      0,        254,      0,        0,       0        },
        {  L_,     255,      0,        0,        254,      0,       0        },
        {  L_,     255,      0,        0,        0,        254,     0        },
        {  L_,     255,      255,      0,        0,        0,       1        },
        {  L_,     255,      0,        255,      0,        0,       1        },
        {  L_,     255,      0,        0,        255,      0,       1        },
        {  L_,     255,      0,        0,        0,        255,     1        },
        {  L_,     255,      1,        1,        0,        0,       0        },
        {  L_,     255,      254,      254,      0,        0,       0        },
        {  L_,     255,      254,      255,      0,        0,       1        },
        {  L_,     255,      1,        1,        1,        0,       0        },
        {  L_,     255,      254,      254,      254,      0,       0        },
        {  L_,     255,      254,      254,      254,      255,     1        },
        {  L_,     255,      255,      255,      255,      0,       1        },
        {  L_,     255,      1,        1,        1,        1,       0        },
        {  L_,     255,      254,      254,      254,      254,     0        },
        {  L_,     255,      255,      255,      255,      255,     1        }
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        Obj mX;
        Entry       *pmCat = mX.addCategory("TEST CATEGORY", 0, 0, 0, 0);
        const Entry *pCat  = pmCat;

        for (int i = 0; i < NUM_DATA; ++i) {
            const int  LINE             = DATA[i].d_line;
            const int  LEVEL            = DATA[i].d_level;
            const int  RECORD_LEVEL     = DATA[i].d_recordLevel;
            const int  PASS_LEVEL       = DATA[i].d_passLevel;
            const int  TRIGGER_LEVEL    = DATA[i].d_triggerLevel;
            const int  TRIGGERALL_LEVEL = DATA[i].d_triggerAllLevel;
            const bool ENABLED          = DATA[i].d_expIsEnabled;

            if (veryVerbose) {
                P_(LINE); P_(LEVEL); P_(RECORD_LEVEL); P_(PASS_LEVEL);
                P_(TRIGGER_LEVEL); P_(TRIGGERALL_LEVEL); P(ENABLED);
            }

            const int r = pmCat->setLevels(RECORD_LEVEL,
                                           PASS_LEVEL,
                                           TRIGGER_LEVEL,
                                           TRIGGERALL_LEVEL);
            LOOP_ASSERT(LINE, 0       == r);
            LOOP_ASSERT(LINE, ENABLED == pCat->isEnabled(LEVEL));
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING DIRECT ACCESSORS
        //
        // Concerns:
        //   The basic concern is that the direct accessors return the four
        //   threshold levels correctly, and that the accessors to the
        //   category name and category return correct values.
        //
        // Plan
        //   Construct a category manager.  Add some categories with varied
        //   names and threshold levels.  Verify that each accessor returns
        //   the expected value.
        //
        // Testing:
        //   const char *categoryName() const;
        //   int recordLevel() const;
        //   int passLevel() const;
        //   const ball::ThresholdAggregate& thresholdLevels() const;
        //   int triggerLevel() const;
        //   int triggerAllLevel() const;
        //
        //   ball::Category *lookupCategory(const char *name);
        //   const ball::Category *lookupCategory(const char *name) const;
        //   int length() const;
        // --------------------------------------------------------------------

        if (verbose) cout << "Test Direct Accessors\n"
                             "=====================\n";

        Obj mX;  const Obj& X = mX;
        ASSERT(0 == X.length());

        {
            const Entry *pConst    = X.lookupCategory("dummy");
            Entry       *pNonConst = mX.lookupCategory("dummy");
            ASSERT(0 == pConst);
            ASSERT(0 == pNonConst);
        }

        const Entry *pa = mX.addCategory(VA, LA[0], LA[1], LA[2], LA[3]);
        ASSERT(1     == X.length());
        ASSERT(0     == strcmp(VA, pa->categoryName()));
        ASSERT(LA[0] == pa->recordLevel());
        ASSERT(LA[1] == pa->passLevel());
        ASSERT(LA[2] == pa->triggerLevel());
        ASSERT(LA[3] == pa->triggerAllLevel());
        ASSERT(LA[0] == pa->thresholdLevels().recordLevel());
        ASSERT(LA[1] == pa->thresholdLevels().passLevel());
        ASSERT(LA[2] == pa->thresholdLevels().triggerLevel());
        ASSERT(LA[3] == pa->thresholdLevels().triggerAllLevel());
        ASSERT(pa    == X.lookupCategory(VA));
        ASSERT(pa    == mX.lookupCategory(VA));

        const Entry *pb = mX.addCategory(VB, LB[0], LB[1], LB[2], LB[3]);
        ASSERT(2     == X.length());
        ASSERT(0     == strcmp(VB, pb->categoryName()));
        ASSERT(LB[0] == pb->recordLevel());
        ASSERT(LB[1] == pb->passLevel());
        ASSERT(LB[2] == pb->triggerLevel());
        ASSERT(LB[3] == pb->triggerAllLevel());
        ASSERT(LB[0] == pb->thresholdLevels().recordLevel());
        ASSERT(LB[1] == pb->thresholdLevels().passLevel());
        ASSERT(LB[2] == pb->thresholdLevels().triggerLevel());
        ASSERT(LB[3] == pb->thresholdLevels().triggerAllLevel());
        ASSERT(pb    == X.lookupCategory(VB));
        ASSERT(pb    == mX.lookupCategory(VB));

        const Entry *pc = mX.addCategory(VC, LC[0], LC[1], LC[2], LC[3]);
        ASSERT(3     == X.length());
        ASSERT(0     == strcmp(VC, pc->categoryName()));
        ASSERT(LC[0] == pc->recordLevel());
        ASSERT(LC[1] == pc->passLevel());
        ASSERT(LC[2] == pc->triggerLevel());
        ASSERT(LC[3] == pc->triggerAllLevel());
        ASSERT(LC[0] == pc->thresholdLevels().recordLevel());
        ASSERT(LC[1] == pc->thresholdLevels().passLevel());
        ASSERT(LC[2] == pc->thresholdLevels().triggerLevel());
        ASSERT(LC[3] == pc->thresholdLevels().triggerAllLevel());
        ASSERT(pc    == X.lookupCategory(VC));
        ASSERT(pc    == mX.lookupCategory(VC));

        const Entry *pd = mX.addCategory(VD, LD[0], LD[1], LD[2], LD[3]);
        ASSERT(4     == X.length());
        ASSERT(0     == strcmp(VD, pd->categoryName()));
        ASSERT(LD[0] == pd->recordLevel());
        ASSERT(LD[1] == pd->passLevel());
        ASSERT(LD[2] == pd->triggerLevel());
        ASSERT(LD[3] == pd->triggerAllLevel());
        ASSERT(LD[0] == pd->thresholdLevels().recordLevel());
        ASSERT(LD[1] == pd->thresholdLevels().passLevel());
        ASSERT(LD[2] == pd->thresholdLevels().triggerLevel());
        ASSERT(LD[3] == pd->thresholdLevels().triggerAllLevel());
        ASSERT(pd    == X.lookupCategory(VD));
        ASSERT(pd    == mX.lookupCategory(VD));

        const Entry *pe = mX.addCategory(VE, LE[0], LE[1], LE[2], LE[3]);
        ASSERT(5     == X.length());
        ASSERT(0     == strcmp(VE, pe->categoryName()));
        ASSERT(LE[0] == pe->recordLevel());
        ASSERT(LE[1] == pe->passLevel());
        ASSERT(LE[2] == pe->triggerLevel());
        ASSERT(LE[3] == pe->triggerAllLevel());
        ASSERT(LE[0] == pe->thresholdLevels().recordLevel());
        ASSERT(LE[1] == pe->thresholdLevels().passLevel());
        ASSERT(LE[2] == pe->thresholdLevels().triggerLevel());
        ASSERT(LE[3] == pe->thresholdLevels().triggerAllLevel());
        ASSERT(pe    == X.lookupCategory(VE));
        ASSERT(pe    == mX.lookupCategory(VE));

        const Entry *pf = mX.addCategory(VF, LF[0], LF[1], LF[2], LF[3]);
        ASSERT(6     == X.length());
        ASSERT(0     == strcmp(VF, pf->categoryName()));
        ASSERT(LF[0] == pf->recordLevel());
        ASSERT(LF[1] == pf->passLevel());
        ASSERT(LF[2] == pf->triggerLevel());
        ASSERT(LF[3] == pf->triggerAllLevel());
        ASSERT(LF[0] == pf->thresholdLevels().recordLevel());
        ASSERT(LF[1] == pf->thresholdLevels().passLevel());
        ASSERT(LF[2] == pf->thresholdLevels().triggerLevel());
        ASSERT(LF[3] == pf->thresholdLevels().triggerAllLevel());
        ASSERT(pf    == X.lookupCategory(VF));
        ASSERT(pf    == mX.lookupCategory(VF));

        const Entry *pg = mX.addCategory(VG, LG[0], LG[1], LG[2], LG[3]);
        ASSERT(7     == X.length());
        ASSERT(0     == strcmp(VG, pg->categoryName()));
        ASSERT(LG[0] == pg->recordLevel());
        ASSERT(LG[1] == pg->passLevel());
        ASSERT(LG[2] == pg->triggerLevel());
        ASSERT(LG[3] == pg->triggerAllLevel());
        ASSERT(LG[0] == pg->thresholdLevels().recordLevel());
        ASSERT(LG[1] == pg->thresholdLevels().passLevel());
        ASSERT(LG[2] == pg->thresholdLevels().triggerLevel());
        ASSERT(LG[3] == pg->thresholdLevels().triggerAllLevel());
        ASSERT(pg    == X.lookupCategory(VG));
        ASSERT(pg    == mX.lookupCategory(VG));

        if (verbose) cout << "\tTest edge cases" << endl;

        // Test 1: Lookup non-existent categories with various names
        {
            ASSERT(0 == X.lookupCategory("NonExistent"));
            ASSERT(0 == mX.lookupCategory("NonExistent"));
            ASSERT(0 == X.lookupCategory("a"));
            ASSERT(0 == mX.lookupCategory("a"));
        }

        // Test 2: Lookup with special characters
        {
            const char *specialName = "Special.Category:With*Chars";
            const Entry *pSpecial = mX.addCategory(specialName, 1, 2, 3, 4);
            ASSERT(8 == X.length());
            ASSERT(pSpecial == X.lookupCategory(specialName));
            ASSERT(pSpecial == mX.lookupCategory(specialName));
            ASSERT(0 == strcmp(specialName, pSpecial->categoryName()));
        }

        // Test 3: Lookup with very long name
        {
            bsl::string longName(1000, 'X');
            const Entry *pLong = mX.addCategory(longName.c_str(),
                                                10, 20, 30, 40);
            ASSERT(9 == X.length());
            ASSERT(pLong == X.lookupCategory(longName.c_str()));
            ASSERT(pLong == mX.lookupCategory(longName.c_str()));
            ASSERT(0 == strcmp(longName.c_str(), pLong->categoryName()));
        }

        // Test 4: Case sensitivity verification
        {
            const Entry *pLower = mX.addCategory("lowercase",
                                                 5, 6, 7, 8);
            const Entry *pUpper = mX.addCategory("LOWERCASE",
                                                 15, 16, 17, 18);
            ASSERT(11 == X.length());
            ASSERT(pLower != pUpper);
            ASSERT(pLower == X.lookupCategory("lowercase"));
            ASSERT(pUpper == X.lookupCategory("LOWERCASE"));
            ASSERT(0 == X.lookupCategory("Lowercase"));
            ASSERT(0 == X.lookupCategory("LowerCase"));
        }

        // Test 5: Names with whitespace
        {
            const Entry *pSpace = mX.addCategory("name with spaces",
                                                 11, 12, 13, 14);
            ASSERT(12 == X.length());
            ASSERT(pSpace == X.lookupCategory("name with spaces"));
            ASSERT(pSpace == mX.lookupCategory("name with spaces"));
            ASSERT(0 == X.lookupCategory("name with  spaces"));
            ASSERT(0 == X.lookupCategory("namewithspaces"));
        }

        // Test 6: Similar names
        {
            const Entry *p1 = mX.addCategory("SimilarName", 1, 1, 1, 1);
            const Entry *p2 = mX.addCategory("SimilarName1", 2, 2, 2, 2);
            const Entry *p3 = mX.addCategory("SimilarName12", 3, 3, 3, 3);
            const Entry *p4 = mX.addCategory("SimilarNam", 4, 4, 4, 4);
            ASSERT(16 == X.length());
            ASSERT(p1 != p2);
            ASSERT(p2 != p3);
            ASSERT(p3 != p4);
            ASSERT(p1 == X.lookupCategory("SimilarName"));
            ASSERT(p2 == X.lookupCategory("SimilarName1"));
            ASSERT(p3 == X.lookupCategory("SimilarName12"));
            ASSERT(p4 == X.lookupCategory("SimilarNam"));
        }

        // Test 7: Verify length consistency throughout all operations
        {
            const int finalLength = X.length();
            ASSERT(16 == finalLength);
            int countedLength = 0;
            for (int i = 0; i < finalLength; ++i) {
                const Entry *p = X.lookupCategory(X[i].categoryName());
                ASSERT(0 != p);
                ++countedLength;
            }
            ASSERT(finalLength == countedLength);
        }

        // Test 8: Verify const vs non-const lookupCategory return
        //         same pointers
        {
            for (int i = 0; i < X.length(); ++i) {
                const char *name = X[i].categoryName();
                const Entry *pConst = X.lookupCategory(name);
                Entry *pNonConst = mX.lookupCategory(name);
                ASSERT(pConst == pNonConst);
            }
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING PRIMARY MANIPULATORS (BOOTSTRAP)
        //
        // Concerns:
        //   The basic concern is that the default constructor, the destructor,
        //   and the primary manipulators:
        //      - addCategory         (black-box)
        //      - setThresholdLevels  (white-box)
        //   operate as expected.  We have the following specific concerns:
        //    1. The default constructor
        //        1a. Creates the correct initial value.
        //        1b. Has the internal memory management system hooked up
        //            properly so that *all* internally allocated memory
        //            draws from the same user-supplied allocator whenever
        //            one is specified.
        //    2. The destructor properly deallocates all allocated memory to
        //       its corresponding allocator from any attainable state.
        //    3. `addCategory` adds the expected category.
        //    4. `setThresholdLevels` sets the appropriate category with the
        //       specified values.
        // Plan:
        //   To address concerns 1a - 1b, create an object using the default
        //   constructor:
        //    - With and without passing in an allocator.
        //    - Verify that in the presence of `bslma::TestAllocator`, the
        //      memory allocation occurs using the test allocator.
        //
        //   To address concern 3, construct an object, add a series of
        //   categories with various (including identical) names.  In each
        //   test, allow the object to leave scope without further
        //   modification, so that the destructor asserts internal object
        //   invariants appropriately.  After the final add operation in
        //   each test, use the (untested) basic accessors to cross-check the
        //   value of the object.
        //
        // Testing:
        //  ^ball::Category(const char *name, int, int, int, int, *ba = 0);
        //  ^~ball::Category();
        //   ball::CategoryManager(bslma::Allocator *ba = 0);
        //   ~ball::CategoryManager();
        //   BOOTSTRAP: ball::Category *addCategory(*name, int, int, int, int);
        //
        //   Note: '^' indicates private methods which are tested indirectly.
        // --------------------------------------------------------------------

        if (verbose) cout << "Bootstrap Test\n"
                             "==============\n";

        if (verbose) cout << "Testing Category Manager Constructor"
                          << " allocator hookup." << endl;

        {
            bslma::TestAllocator ta;
            bslma::DefaultAllocatorGuard g(&ta);

            Obj mX;  const Obj& X = mX;
            ASSERT(0 == X.length());

            const Entry *pX = mX.addCategory(VA, LA[0], LA[1], LA[2], LA[3]);
            ASSERT(pX);

            ASSERT(1     == X.length());
            ASSERT(0     == strcmp(VA, pX->categoryName()));
            ASSERT(LA[0] == pX->recordLevel());
            ASSERT(LA[1] == pX->passLevel());
            ASSERT(LA[2] == pX->triggerLevel());
            ASSERT(LA[3] == pX->triggerAllLevel());

            ASSERT(0 < ta.numAllocations());
            ASSERT(0 < ta.numBytesInUse());

        }

        bslma::TestAllocator ta(veryVeryVerbose);
        {
            ASSERT(0 == ta.numAllocations());
            ASSERT(0 == ta.numBytesInUse());

            Obj mY(&ta);  const Obj& Y = mY;
            ASSERT(0 == Y.length());

            const Entry *pY = mY.addCategory(VB, LB[0], LB[1], LB[2], LB[3]);
            ASSERT(pY);

            ASSERT(1     == Y.length());
            ASSERT(0     == strcmp(VB, pY->categoryName()));
            ASSERT(LB[0] == pY->recordLevel());
            ASSERT(LB[1] == pY->passLevel());
            ASSERT(LB[2] == pY->triggerLevel());
            ASSERT(LB[3] == pY->triggerAllLevel());

            ASSERT(0 < ta.numAllocations());
            ASSERT(0 < ta.numBytesInUse());
        }
        ASSERT(0  == ta.numBytesInUse());

        if (verbose) cout << "Testing Category Manager Constructor"
                          << " and primary manipulator." << endl;
        {
            Obj mZ(&ta);  const Obj& Z = mZ;
            ASSERT(0 == Z.length());

            Entry       *pmaZ = mZ.addCategory(VA, LA[0], LA[1], LA[2], LA[3]);
            const Entry *paZ  = pmaZ;
            ASSERT(pmaZ);  ASSERT(0 == mZ.addCategory(VA, 2, 3, 4, 5));

            ASSERT(1     == Z.length());
            ASSERT(0     == strcmp(VA, paZ->categoryName()));
            ASSERT(LA[0] == paZ->recordLevel());
            ASSERT(LA[1] == paZ->passLevel());
            ASSERT(LA[2] == paZ->triggerLevel());
            ASSERT(LA[3] == paZ->triggerAllLevel());

            pmaZ->setLevels(LA[0] + 1, LA[1] + 1, LA[2] + 1, LA[3] + 1);
            ASSERT(LA[0] + 1 == paZ->recordLevel());
            ASSERT(LA[1] + 1 == paZ->passLevel());
            ASSERT(LA[2] + 1 == paZ->triggerLevel());
            ASSERT(LA[3] + 1 == paZ->triggerAllLevel());

            mZ.setThresholdLevels(VA, LA[0], LA[1], LA[2], LA[3]);
            ASSERT(LA[0] == paZ->recordLevel());
            ASSERT(LA[1] == paZ->passLevel());
            ASSERT(LA[2] == paZ->triggerLevel());
            ASSERT(LA[3] == paZ->triggerAllLevel());

            Entry       *pmbZ = mZ.addCategory(VB, LB[0], LB[1], LB[2], LB[3]);
            const Entry *pbZ  = pmbZ;
            ASSERT(pmbZ);  ASSERT(0 == mZ.addCategory(VB, 2, 3, 4, 5));

            ASSERT(2     == Z.length());
            ASSERT(0     == strcmp(VB, pbZ->categoryName()));
            ASSERT(LB[0] == pbZ->recordLevel());
            ASSERT(LB[1] == pbZ->passLevel());
            ASSERT(LB[2] == pbZ->triggerLevel());
            ASSERT(LB[3] == pbZ->triggerAllLevel());

            pmbZ->setLevels(LB[0] + 1, LB[1] + 1, LB[2] + 1, LB[3] + 1);
            ASSERT(LB[0] + 1 == pbZ->recordLevel());
            ASSERT(LB[1] + 1 == pbZ->passLevel());
            ASSERT(LB[2] + 1 == pbZ->triggerLevel());
            ASSERT(LB[3] + 1 == pbZ->triggerAllLevel());

            mZ.setThresholdLevels(VB, LB[0], LB[1], LB[2], LB[3]);
            ASSERT(LB[0] == pbZ->recordLevel());
            ASSERT(LB[1] == pbZ->passLevel());
            ASSERT(LB[2] == pbZ->triggerLevel());
            ASSERT(LB[3] == pbZ->triggerAllLevel());

            Entry       *pmcZ = mZ.addCategory(VC, LC[0], LC[1], LC[2], LC[3]);
            const Entry *pcZ  = pmcZ;
            ASSERT(pmcZ);  ASSERT(0 == mZ.addCategory(VC, 2, 3, 4, 5));

            ASSERT(3     == Z.length());
            ASSERT(0     == strcmp(VC, pcZ->categoryName()));
            ASSERT(LC[0] == pcZ->recordLevel());
            ASSERT(LC[1] == pcZ->passLevel());
            ASSERT(LC[2] == pcZ->triggerLevel());
            ASSERT(LC[3] == pcZ->triggerAllLevel());

            pmcZ->setLevels(LC[0] + 1, LC[1] + 1, LC[2] + 1, LC[3] + 1);
            ASSERT(LC[0] + 1 == pcZ->recordLevel());
            ASSERT(LC[1] + 1 == pcZ->passLevel());
            ASSERT(LC[2] + 1 == pcZ->triggerLevel());
            ASSERT(LC[3] + 1 == pcZ->triggerAllLevel());

            mZ.setThresholdLevels(VC, LC[0], LC[1], LC[2], LC[3]);
            ASSERT(LC[0] == pcZ->recordLevel());
            ASSERT(LC[1] == pcZ->passLevel());
            ASSERT(LC[2] == pcZ->triggerLevel());
            ASSERT(LC[3] == pcZ->triggerAllLevel());

            Entry       *pmdZ = mZ.addCategory(VD, LD[0], LD[1], LD[2], LD[3]);
            const Entry *pdZ  = pmdZ;
            ASSERT(pmdZ);  ASSERT(0 == mZ.addCategory(VD, 2, 3, 4, 5));

            ASSERT(4     == Z.length());
            ASSERT(0     == strcmp(VD, pdZ->categoryName()));
            ASSERT(LD[0] == pdZ->recordLevel());
            ASSERT(LD[1] == pdZ->passLevel());
            ASSERT(LD[2] == pdZ->triggerLevel());
            ASSERT(LD[3] == pdZ->triggerAllLevel());

            pmdZ->setLevels(LD[0] + 1, LD[1] + 1, LD[2] + 1, LD[3] + 1);
            ASSERT(LD[0] + 1 == pdZ->recordLevel());
            ASSERT(LD[1] + 1 == pdZ->passLevel());
            ASSERT(LD[2] + 1 == pdZ->triggerLevel());
            ASSERT(LD[3] + 1 == pdZ->triggerAllLevel());

            mZ.setThresholdLevels(VD, LD[0], LD[1], LD[2], LD[3]);
            ASSERT(LD[0] == pdZ->recordLevel());
            ASSERT(LD[1] == pdZ->passLevel());
            ASSERT(LD[2] == pdZ->triggerLevel());
            ASSERT(LD[3] == pdZ->triggerAllLevel());

            Entry       *pmeZ = mZ.addCategory(VE, LE[0], LE[1], LE[2], LE[3]);
            const Entry *peZ  = pmeZ;
            ASSERT(pmeZ);  ASSERT(0 == mZ.addCategory(VE, 2, 3, 4, 5));

            ASSERT(5     == Z.length());
            ASSERT(0     == strcmp(VE, peZ->categoryName()));
            ASSERT(LE[0] == peZ->recordLevel());
            ASSERT(LE[1] == peZ->passLevel());
            ASSERT(LE[2] == peZ->triggerLevel());
            ASSERT(LE[3] == peZ->triggerAllLevel());

            pmeZ->setLevels(LE[0] + 1, LE[1] + 1, LE[2] + 1, LE[3] + 1);
            ASSERT(LE[0] + 1 == peZ->recordLevel());
            ASSERT(LE[1] + 1 == peZ->passLevel());
            ASSERT(LE[2] + 1 == peZ->triggerLevel());
            ASSERT(LE[3] + 1 == peZ->triggerAllLevel());

            mZ.setThresholdLevels(VE, LE[0], LE[1], LE[2], LE[3]);
            ASSERT(LE[0] == peZ->recordLevel());
            ASSERT(LE[1] == peZ->passLevel());
            ASSERT(LE[2] == peZ->triggerLevel());
            ASSERT(LE[3] == peZ->triggerAllLevel());

            Entry       *pmfZ = mZ.addCategory(VF, LF[0], LF[1], LF[2], LF[3]);
            const Entry *pfZ  = pmfZ;
            ASSERT(pmfZ);  ASSERT(0 == mZ.addCategory(VF, 2, 3, 4, 5));

            ASSERT(6     == Z.length());
            ASSERT(0     == strcmp(VF, pfZ->categoryName()));
            ASSERT(LF[0] == pfZ->recordLevel());
            ASSERT(LF[1] == pfZ->passLevel());
            ASSERT(LF[2] == pfZ->triggerLevel());
            ASSERT(LF[3] == pfZ->triggerAllLevel());

            pmfZ->setLevels(LF[0] + 1, LF[1] + 1, LF[2] + 1, LF[3] + 1);
            ASSERT(LF[0] + 1 == pfZ->recordLevel());
            ASSERT(LF[1] + 1 == pfZ->passLevel());
            ASSERT(LF[2] + 1 == pfZ->triggerLevel());
            ASSERT(LF[3] + 1 == pfZ->triggerAllLevel());

            mZ.setThresholdLevels(VF, LF[0], LF[1], LF[2], LF[3]);
            ASSERT(LF[0] == pfZ->recordLevel());
            ASSERT(LF[1] == pfZ->passLevel());
            ASSERT(LF[2] == pfZ->triggerLevel());
            ASSERT(LF[3] == pfZ->triggerAllLevel());

            Entry       *pmgZ = mZ.addCategory(VG, LG[0], LG[1], LG[2], LG[3]);
            const Entry *pgZ  = pmgZ;
            ASSERT(pmgZ);  ASSERT(0 == mZ.addCategory(VG, 2, 3, 4, 5));

            ASSERT(7     == Z.length());
            ASSERT(0     == strcmp(VG, pgZ->categoryName()));
            ASSERT(LG[0] == pgZ->recordLevel());
            ASSERT(LG[1] == pgZ->passLevel());
            ASSERT(LG[2] == pgZ->triggerLevel());
            ASSERT(LG[3] == pgZ->triggerAllLevel());

            pmgZ->setLevels(LG[0] + 1, LG[1] + 1, LG[2] + 1, LG[3] + 1);
            ASSERT(LG[0] + 1 == pgZ->recordLevel());
            ASSERT(LG[1] + 1 == pgZ->passLevel());
            ASSERT(LG[2] + 1 == pgZ->triggerLevel());
            ASSERT(LG[3] + 1 == pgZ->triggerAllLevel());

            mZ.setThresholdLevels(VG, LG[0], LG[1], LG[2], LG[3]);
            ASSERT(LG[0] == pgZ->recordLevel());
            ASSERT(LG[1] == pgZ->passLevel());
            ASSERT(LG[2] == pgZ->triggerLevel());
            ASSERT(LG[3] == pgZ->triggerAllLevel());
        }
        ASSERT(0 == ta.numBytesInUse());
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //
        // Concerns:
        //   We want to exercise basic functionality of this class.  In
        //   particular, we want to demonstrate a base-line level of correct
        //   operation of the following methods and operators:
        //     - constructor
        //     - addCategory
        //     - lookupCategory
        //     - setThresholdLevels
        //     - categoryName
        //     - recordLevel
        //     - passLevel
        //     - triggerLevel
        //     - triggerAllLevel
        //     - isEnabled
        //     - length
        //
        // Plan:
        //   First, create a `ball::CategoryManager` instance.  Add a category
        //   to the category manager, verify the category manager's length, and
        //   verify that all levels and the name are set correctly.
        //
        //   Next, set the severity levels to different values.  Verify they
        //   are set correctly.  Set severity levels back to their original
        //   values.
        //
        //   Next, add another category, verify the name and levels of the new
        //   category, and verify the new length.  Lookup the category by name,
        //   and verify the obtained category name is the same as that of the
        //   added category.
        //
        // Testing:
        //   This "test" *exercises* basic functionality, but *tests* nothing.
        // --------------------------------------------------------------------

        if (verbose) cout << "BREATHING TEST\n"
                             "==============\n";

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        Obj mX;  const Obj& X = mX;

        ASSERT(0 == X.length());

        if (veryVerbose) {
            cout << "Add category '" << VA << "' with:";
            T_; P_(LA[0]); P_(LA[1]); P_(LA[2]); P(LA[3]);
        }

        ASSERT(mX.addCategory(VA, LA[0], LA[1], LA[2], LA[3]));
        ASSERT(1 == X.length());

        Entry       *me1 = mX.lookupCategory(VA);
        const Entry *e1  = me1;
        ASSERT(e1);

        ASSERT(0     == strcmp(VA, e1->categoryName()));
        ASSERT(LA[0] == e1->recordLevel());
        ASSERT(LA[1] == e1->passLevel());
        ASSERT(LA[2] == e1->triggerLevel());
        ASSERT(LA[3] == e1->triggerAllLevel());

        ASSERT(0     == me1->setLevels(LB[0], LB[1], LB[2], LB[3]));
        ASSERT(LB[0] == e1->recordLevel());
        ASSERT(LB[1] == e1->passLevel());
        ASSERT(LB[2] == e1->triggerLevel());
        ASSERT(LB[3] == e1->triggerAllLevel());

        ASSERT(mX.setThresholdLevels(VA, LA[0], LA[1], LA[2], LA[3]));
        ASSERT(LA[0] == e1->recordLevel());
        ASSERT(LA[1] == e1->passLevel());
        ASSERT(LA[2] == e1->triggerLevel());
        ASSERT(LA[3] == e1->triggerAllLevel());

        if (veryVerbose) {
            cout << "Add category '" << VB << "' with:";
            T_; P_(LB[0]); P_(LB[1]); P_(LB[2]); P(LB[3]);
        }

        ASSERT(mX.addCategory(VB, LB[0], LB[1], LB[2], LB[3]));
        ASSERT(2 == X.length());

        const Entry *e2 = X.lookupCategory(VB);
        ASSERT(e2);

        ASSERT(0     == strcmp(VB, e2->categoryName()));
        ASSERT(LB[0] == e2->recordLevel());
        ASSERT(LB[1] == e2->passLevel());
        ASSERT(LB[2] == e2->triggerLevel());
        ASSERT(LB[3] == e2->triggerAllLevel());
      } break;
      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        cerr << "Error, non-zero test status = " << testStatus << "." << endl;
    }
    return testStatus;
}

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
