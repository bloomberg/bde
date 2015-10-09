// ball_categorymanager.t.cpp                                         -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------


#include <ball_categorymanager.h>

#include <bslmt_barrier.h>
#include <bslmt_lockguard.h>

#include <bdlb_bitutil.h>

#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>
#include <bslma_testallocatorexception.h>

#include <bsls_platform.h>
#include <bsls_types.h>

#include <bsl_algorithm.h>
#include <bsl_bitset.h>
#include <bsl_climits.h>
#include <bsl_cstdio.h>
#include <bsl_cstdlib.h>
#include <bsl_cstring.h>
#include <bsl_iomanip.h>
#include <bsl_iostream.h>
#include <bsl_new.h>
#include <bsl_ostream.h>    // i2bs
#include <bsl_string.h>
#include <bsl_streambuf.h>  // i2bs
#include <bsl_strstream.h>
#include <bsl_sstream.h>
#include <bsl_vector.h>

#include <bsl_c_stdlib.h>             // for rand_r

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

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
// 'ball::Category' private methods, tested indirectly:
// [ 2] ball::Category(const char *name, int, int, int, int, *ba = 0);
// [ 2] ~ball::Category();
//
// 'ball::Category' public interface:
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
// 'ball::CategoryManager' public interface:
// [ 2] ball::CategoryManager(bslma::Allocator *ba = 0);
// [ 2] ~ball::CategoryManager();
// [ 6] ball::Category& operator[](int index)
// [ 5] ball::Category *addCategory(const char *name, int, int, int, int);
// [11] ball::Category *addCategory(Holder *, const char *, int, int, int, int);
// [ 3] ball::Category *lookupCategory(const char *name);
// [11] ball::Category *lookupCategory(Holder *, const char *name);
// [11] void resetCategoryHolders();
// [ 5] ball::Category *setThresholdLevels(*name, int, int, int, int);
// [ 8] int addRule(const ball::Rule& rule);
// [ 9] int addRules(const ball::RuleSet& ruleSet);
// [ 8] int removeRule(const ball::Rule& rule);
// [ 9] int removeRules(const ball::RuleSet& ruleSet);
// [ 8] void removeAllRules();
// [13] bslmt::Mutex& rulesetMutex();
// [ 8] const ball::Category& operator[](int index) const
// [ 3] const ball::Category *lookupCategory(const char *name) const;
// [ 3] int length() const;
// [ 8] int ruleSequenceNumber() const;
// [13] ball::RuleSet& ruleSet() const;
//
// 'ball::CategoryHolder' public interface:
// [10] void reset();
// [10] void setCategory(const ball::Category *category);
// [10] void setThreshold(int threshold);
// [10] void setNext(ball::CategoryHolder *holder);
// [10] const ball::Category *category() const;
// [10] int threshold() const;
// [10] ball::CategoryHolder *next() const;
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 2] BASIC CONSTRUCTORS AND PRIMARY MANIPULATORS (BOOTSTRAP)
// [ 2] BOOTSTRAP: ball::Category *addCategory(*name, int, int, int, int);
// [ 7] MT-SAFETY
// [12] TESTING IMPACT OF RULES ON CATEGORY HOLDERS
// [13] CONCURRENCY TEST: RULES
// [14] USAGE EXAMPLE

//-----------------------------------------------------------------------------

//=============================================================================
//                        STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
static int testStatus = 0;
static int verbose;
static int veryVerbose;
static int veryVeryVerbose;

void aSsErT(int c, const char *s, int i)
{
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (0 <= testStatus && testStatus <= 100) ++testStatus;
    }
}

#define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

//=============================================================================
//                     STANDARD BDE LOOP-ASSERT TEST MACROS
//-----------------------------------------------------------------------------
#define LOOP_ASSERT(I,X) { \
   if (!(X)) { cout << #I << ": " << I << "\n"; aSsErT(1, #X, __LINE__); }}

#define LOOP2_ASSERT(I,J,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " \
              << J << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP3_ASSERT(I,J,K,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" \
              << #K << ": " << K << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP4_ASSERT(I,J,K,L,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" << \
       #K << ": " << K << "\t" << #L << ": " << L << "\n"; \
       aSsErT(1, #X, __LINE__); } }

#define LOOP5_ASSERT(I,J,K,L,M,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" << \
       #K << ": " << K << "\t" << #L << ": " << L << "\t" << \
       #M << ": " << M << "\n"; \
       aSsErT(1, #X, __LINE__); } }

#define LOOP6_ASSERT(I,J,K,L,M,N,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" << \
       #K << ": " << K << "\t" << #L << ": " << L << "\t" << \
       #M << ": " << M << "\t" << #N << ": " << N << "\n"; \
       aSsErT(1, #X, __LINE__); } }

//=============================================================================
//                       SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", "<< flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number
#define T_()  cout << "\t" << flush;          // Print tab w/o newline

//=============================================================================
//                           GLOBAL MACROS FOR TESTING
//-----------------------------------------------------------------------------
#define PA(X) cout << #X " = " << ((void *) X) << endl;
#define PA_(X) cout << #X " = " << ((void *) X) << ", " << flush;

// The following variable and macros provide a thread-safe framework
// for using bsl::cout.

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
typedef ball::ThresholdAggregate      Thresholds;
typedef const char*                  Name;
typedef bslma::TestAllocator         TestAllocator;
typedef bslma::DefaultAllocatorGuard DefaultAllocGuard;


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

const int NUM_LEVELS = sizeof LEVELS / sizeof *LEVELS;

const int *LA = LEVELS[0],
          *LB = LEVELS[1],
          *LC = LEVELS[2],
          *LD = LEVELS[3],
          *LE = LEVELS[4],
          *LF = LEVELS[5],
          *LG = LEVELS[6];

//-----------------------------------------------------------------------------
                             // =================
                             // class my_ListType
                             // =================

class my_ListType : public bsl::vector<void*> {
    // Objects of this type are passed in thread arguments to
    // functions declared 'extern "C"'.
    //
    // Note that this is a workaround for certain compilers (such as
    // Sun WorkShop 6 update 1 C++ 5.2 Patch 109508-09 2002/07/08)
    // which do not allow template functions to be called in 'extern "C"'
    // functions.

  public:
    // MANIPULATORS
    void sort();
        // Sort this list in ascending order.
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

struct my_ThreadParameters {
    // This class provides a set of parameters given to a worker thread.  In
    // particular, this class provides a category manager on which to perform
    // thread-specific actions, a list of category names to act on, a
    // barrier object to coordinate thread activity, and a container to store
    // the results of any computations.

    bslmt::Barrier *d_barrier_p;  // coordinates activity between threads (held)
    my_ListType   *d_results_p;  // container to store results (held)
    Obj           *d_cm_p;       // a category manager (held)
    bsl::string   *d_names_p;    // a list of category names (held)
    int            d_size;       // number of category names in list
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
using namespace bsl;  // automatically added by script

    return bs.template to_string<char, char_traits<char>, allocator<char> >();
}

extern "C"
void *case9ThreadW(void *arg)
{
    // Retrieve the parameters 'cm', 'names', 'NUM_NAMES', and 'results'
    // from the opaque argument 'arg' of type pointer to 'my_ThreadParameters'.
    // In a tight loop, write each category names in the 'names' array of
    // size 'NUM_NAMES' to the category manager 'cm'.  Store the address
    // of each category added to 'cm' in the 'results' container.  Return
    // after attempting to add each category.  Note that when more than one
    // "write" thread is active, we expect each to add only a subset of
    // categories to the manager due to the acquisition of write locks and
    // the fact that the 'addCategory' method fails if the category is already
    // present.

    my_ThreadParameters& params = *static_cast<my_ThreadParameters*>(arg);
    Obj&                 cm = *params.d_cm_p;
    bsl::string         *names = params.d_names_p;
    const int            NUM_NAMES = params.d_size;
    my_ListType&         results = *params.d_results_p;

    params.d_barrier_p->wait();

    for (int i = 0; i < NUM_NAMES; ++i) {
        if (veryVeryVerbose) {
            MTCOUT << "Add category '" << names[i] << "'"
                   << MTENDL;
        }

        Holder mH; const Holder& H = mH;
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
               << results.size() << " categories were added"
               << MTENDL;
    }

    return 0;
}

extern "C"
void *case9ThreadQ(void *arg)
{
    // Retrieve the parameters 'cm', 'names', 'NUM_NAMES', and 'results'
    // from the opaque argument 'arg' of type pointer to 'my_ThreadParameters'.
    // In a tight loop, query the category manager 'cm' for each category
    // named in the 'names' array of size 'NUM_NAMES'.  Store each address
    // in the 'results' container.  After all categories have been
    // retrieved, sort 'results', and return.

    my_ThreadParameters& params = *static_cast<my_ThreadParameters*>(arg);
    Obj&                 cm = *params.d_cm_p;
    bsl::string         *names = params.d_names_p;
    const int            NUM_NAMES = params.d_size;
    my_ListType&         results = *params.d_results_p;

    params.d_barrier_p->wait();

    for (int i = 0; i < NUM_NAMES; ++i) {
        if (veryVeryVerbose) {
            MTCOUT << "Lookup category '" << names[i] << "'"
                   << MTENDL;
        }

        Holder mH; const Holder& H = mH;
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
               << "results.size() = " << results.size()
               << MTENDL;
    }

    ASSERT(NUM_NAMES == (signed) results.size());
    results.sort();
    return 0;
}

struct RuleThreadTestArgs {
    Obj           *d_mx;
    bslmt::Barrier *d_barrier;
};

extern "C" void *ruleThreadTest(void *args)
{
    Obj&            mX      = *((RuleThreadTestArgs *)args)->d_mx;
    bslmt::Barrier&  barrier = *((RuleThreadTestArgs *)args)->d_barrier;
    const Obj&      MX      = mX;


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
                        (bsls::Types::Int64) bslmt::ThreadUtil::selfIdAsInt()));
        ball::Rule rule4(US, 255, 255, 255, 255);

        mX.addRule(rule1);
        mX.addRule(rule2);
        ASSERT(1 == mX.addRule(rule3));
        ASSERT(0 == mX.addRule(rule3));
        ASSERT(1 == mX.addRule(rule4));
        ASSERT(0 == mX.addRule(rule4));


        mX.rulesetMutex().lock();
        int ruleId1 = MX.ruleSet().ruleId(rule1);
        int ruleId3 = MX.ruleSet().ruleId(rule3);
        int ruleId4 = MX.ruleSet().ruleId(rule4);
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
            ASSERT(!bdlb::BitUtil::isBitSet(entry->relevantRuleMask(),ruleId4));
        }

        mX.removeRule(rule2);
        ASSERT(1 == mX.removeRule(rule3));
        ASSERT(1 == mX.removeRule(rule4));

        ball::RuleSet ruleSet;
        ruleSet.addRule(rule4);
        ruleSet.addRule(rule2);

        ASSERT(1 <= mX.addRules(ruleSet));

        mX.rulesetMutex().lock();
        ruleId4 = MX.ruleSet().ruleId(rule4);
        mX.rulesetMutex().unlock();

        const Entry *entry = mX.lookupCategory(US);
        ASSERT(bdlb::BitUtil::isBitSet(entry->relevantRuleMask(), ruleId4));
        {
            bslmt::LockGuard<bslmt::Mutex> guard(&mX.rulesetMutex());
            int seqNumber = MX.ruleSequenceNumber();
            const ball::RuleSet& rules = MX.ruleSet();
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

            ASSERT(seqNumber     == MX.ruleSequenceNumber());
            ASSERT(count         == rules.numRules());
            ASSERT(numRules      == rules.numRules());
            ASSERT(numPredicates == rules.numPredicates());

        }
        ASSERT(1 <= mX.removeRules(ruleSet));

    }
    barrier.wait();


    return 0;
}

//=============================================================================
//                                 MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;

    verbose = argc > 2;
    veryVerbose = argc > 3;
    veryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;;

    bslma::TestAllocator testAllocator(veryVeryVerbose);

    switch (test) { case 0:  // Zero is always the leading case.
      case 13: {
        // --------------------------------------------------------------------
        // TESTING USAGE EXAMPLE
        //
        // Concerns:
        //   The usage example provided in the component header file must
        //   compile, link, and run on all platforms as shown.
        //
        // Plan:
        //   Incorporate usage example from header into driver, remove leading
        //   comment characters, and replace 'assert' with 'ASSERT'.  This now
        //   becomes the source, which is then "copied" back to the header file
        //   by reversing the above process.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "Testing Usage Example" << endl
                                  << "=====================" << endl;

        char buf[2048];
        ostrstream out(buf, sizeof buf);

        const char *myCategories[] = {
            "EQUITY.MARKET.NYSE",
            "EQUITY.MARKET.NASDAQ",
            "EQUITY.GRAPHICS.MATH.FACTORIAL",
            "EQUITY.GRAPHICS.MATH.ACKERMANN"
        };
        const int NUM_CATEGORIES = sizeof myCategories
                                 / sizeof myCategories[0];

        ball::CategoryManager cm;

        for (int i = 0; i < NUM_CATEGORIES; ++i) {
            cm.addCategory(myCategories[i], 192 + i, 96 + i, 64 + i, 32 + i);
        }

        for (int i = 0; i < NUM_CATEGORIES; ++i) {
            const ball::Category *category = cm.lookupCategory(myCategories[i]);
            out << "[ " << myCategories[i]
                << ", " << category->recordLevel()
                << ", " << category->passLevel()
                << ", " << category->triggerLevel()
                << ", " << category->triggerAllLevel()
                << " ]" << endl;
        }

        if (veryVerbose) { out << ends; cout << buf << endl; }
        out.seekp(0);  // reset ostrstream

        for (int i = 0; i < NUM_CATEGORIES; ++i) {
            ball::Category *category = cm.lookupCategory(myCategories[i]);
            category->setLevels(category->recordLevel() + 1,
                                category->passLevel() + 1,
                                category->triggerLevel() + 1,
                                category->triggerAllLevel() + 1);
        }

        for (int i = 0; i < NUM_CATEGORIES; ++i) {
            const ball::Category *category = cm.lookupCategory(myCategories[i]);
            out << "[ " << myCategories[i]
                << ", " << category->recordLevel()
                << ", " << category->passLevel()
                << ", " << category->triggerLevel()
                << ", " << category->triggerAllLevel()
                << " ]" << endl;
        }

        if (veryVerbose) { out << ends; cout << buf << endl; }
        out.seekp(0);  // reset ostrstream

        for (int i = 0; i < cm.length(); ++i) {
            ball::Category&   category = cm[i];
            category.setLevels(category.recordLevel() + 1,
                               category.passLevel() + 1,
                               category.triggerLevel() + 1,
                               category.triggerAllLevel() + 1);
        }

        for (int i = 0; i < cm.length(); ++i) {
            const ball::Category&   category = cm[i];
            out << "[ " << category.categoryName()
                << ", " << category.recordLevel()
                << ", " << category.passLevel()
                << ", " << category.triggerLevel()
                << ", " << category.triggerAllLevel()
                << " ]" << endl;
        }
        if (veryVerbose) { out << ends; cout << buf << endl; }
      }  break;
      case 12: {
        // --------------------------------------------------------------------
        // CONCURRENCY TEST: RULES
        //
        // Concerns:  That the operations for accessing and modifying the
        //    rules of a category manager are thread-safe.
        //
        // Plan:
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "Testing Rule Concurrency" << endl
                                  << "========================" << endl;

        enum {
            NUM_THREADS = 5
        };

        bsl::vector<bslmt::ThreadUtil::Handle> handles;
        handles.resize(NUM_THREADS);

        bslma::TestAllocator ta(veryVeryVerbose);
        Obj                 mX(&ta);
        bslmt::Barrier       barrier(NUM_THREADS);
        RuleThreadTestArgs  args = { &mX, &barrier };

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
      case 14: {
        // --------------------------------------------------------------------
        // TESTING IMPACT OF RULES ON CATEGORY HOLDERS
        //
        // Concerns:  The category holder's of a category have the correct
        //    threshold as rules are added and removed from the category
        //    manager.
        //
        // Plan:
        //    Create a set of 'ball::ThresholdAggregate' objects containing a
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

        if (verbose) cout << endl
                          << "Testing rules and category holders" << endl
                          << "==================================" << endl;

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
        Obj mX(&ta); const Obj& MX = mX;

        const Entry *C = mX.addCategory("C", 255, 255, 255, 255);
        for (int i = 0; i < holders.size(); ++i) {
            ASSERT(C == mX.lookupCategory(&holders[i], "C"));
            ASSERT(255 == holders[i].threshold());
        }

        // Set the threshold levels for the category.
        for (int i = 0; i < thresholds.size(); ++i) {
            ASSERT(0 !=
                   mX.setThresholdLevels("C",
                                         thresholds[i].recordLevel(),
                                         thresholds[i].passLevel(),
                                         thresholds[i].triggerLevel(),
                                         thresholds[i].triggerAllLevel()));
            int catThreshold = Thresholds::maxLevel(thresholds[i]);

            for (int k = 0; k < holders.size(); ++k) {
                LOOP2_ASSERT(i,k, catThreshold == holders[k].threshold());
            }

            // Set a series of rule thresholds for the category.
            for (int j = 0; j < holders.size(); ++j) {
                for (int k = 0; k < holders.size(); ++k) {
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
                for (int k = 0; k < holders.size(); ++k) {
                    ASSERT(newThreshold == holders[k].threshold());
                }
                ASSERT(1 == mX.removeRule(rule));

                for (int k = 0; k < holders.size(); ++k) {
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
                for (int k = 0; k < holders.size(); ++k) {
                    ASSERT(currentThreshold == holders[k].threshold());
                }
            }
            mX.removeAllRules();
        }
      } break;
      case 11: {
        // --------------------------------------------------------------------
        // TESTING FUNCTIONS TAKING A HOLDER
        //
        // Concerns:
        //   The 'addCategory' function taking a ball::CategoryHolder object
        //   works exactly as the 'addCategory' without the
        //   ball_CategoryHolder.
        //
        // Plan
        //   Construct a category manager.  Add some categories with varied
        //   names and threshold levels, passing a category holder object.
        //   Verify that the returned category object is correct and also
        //   check that the passed in holder object is correctly populated.
        //   Use the 'lookupCategory' function, again passing it the holder
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

        if (verbose) cout << endl << "Test Functions taking a holder" << endl
                                  << "==============================" << endl;

        TestAllocator da(veryVeryVerbose); const TestAllocator& DA = da;
        TestAllocator ta(veryVeryVerbose); const TestAllocator& TA = ta;
        DefaultAllocGuard guard(&da);
        int numBytes = TA.numBytesInUse();

      BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
        Obj mX(&ta); const Obj& X = mX;
        const int UC = Holder::e_UNINITIALIZED_CATEGORY;
        {
            Holder mH; const Holder& H = mH;
            mH.reset();
            Entry *entry = mX.lookupCategory(&mH, "dummy");
            ASSERT(0        == DA.numBytesInUse());
            ASSERT(numBytes < TA.numBytesInUse());
            ASSERT(0        == entry);
            ASSERT(UC       == H.threshold());
            ASSERT(0        == H.category());
            ASSERT(0        == H.next());
        }

        ASSERT(0        == DA.numBytesInUse());
        ASSERT(numBytes < TA.numBytesInUse());
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

        // Test passing NULL Holder to 'addCategory'
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


        // Test passing NULL Holder to 'lookupCategory'
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
        //   'reset' function to put X into its default state.  Finally, use
        //   the manipulator functions to re-assign the same data value to X.
        //   Confirm the state of X using the accessors.
        //
        // Testing:
        // void reset();
        // void setCategory(const ball::Category *category);
        // void setThreshold(int threshold);
        // void setNext(ball::CategoryHolder *holder);
        // const ball::Category *category() const;
        // int threshold() const;
        // ball::CategoryHolder *next() const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "Test ball::CategoryHolder" << endl
                                  << "========================" << endl;

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
            const int NUM_BYTES = testAllocator.numBytesInUse();

            Holder mX = {
                Holder::e_DYNAMIC_CATEGORY,
                const_cast<Entry *>(CATEGORY),
                NEXT
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
            LOOP3_ASSERT(LINE, NEXT, X.next(),
                         NEXT == X.next());

            LOOP3_ASSERT(LINE, NUM_BYTES, testAllocator.numBytesInUse(),
                         NUM_BYTES == testAllocator.numBytesInUse());
        }
      } break;
      case 9: {
        // --------------------------------------------------------------------
        // TESTING: 'addRules', 'removeRules'
        //
        // Concerns:  That 'addRules' and 'removeRules' correctly update the
        //   category manager's rule set and the relevant rule masks for the
        //   managed categories, and return the correct value.
        //
        // Plan:
        //   For each possible subset of a set of categories, create a rule
        //   for each category in the subset and add it to a rule-set.  Add the
        //   set of rules to the category manager and verify: (1) the return
        //   value for 'addRules' is the number of rules (2) the sequence
        //   number is updated (3) the category manager's rule set contains
        //   the newly added rules, and (4) that the relevant rule bit mask
        //   for each category is correct.  Then, create a rule set for all
        //   categories and invoke 'addRules', verify the return value for
        //   the 'addRules' method is the number of rules that were *not*
        //   originally added.  Then remove the original subset of rules, and
        //   verify the same 4 points as when we originally added the rules.
        //   Finally, remove all the rules, and verify the return value.
        //
        // Testing:
        //  int addRules(const ball::RuleSet& );
        //  int removeRules(const ball::RuleSet& );
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Test 'addRules', 'removeRules'" << endl
                          << "==============================" << endl;

        bslma::TestAllocator ta;
        Obj mX(&ta); const Obj& MX = mX;

        for (int i = 0; i < NUM_NAMES; ++i) {
            mX.addCategory(NAMES[i], LEVELS[i][0], LEVELS[i][1],
                                     LEVELS[i][2], LEVELS[i][3]);
        }

        // Iterate over each possible set of rules.
        ball::RuleSet::MaskType endMask = ~((~0) << NUM_NAMES);

        for (ball::RuleSet::MaskType mask = 0; mask <= endMask; ++mask) {
            int seqNumber = MX.ruleSequenceNumber();
            ball::RuleSet rules(&ta);
            for (int i= 0; i < NUM_NAMES; ++i) {
                if (bdlb::BitUtil::isBitSet(mask, i)) {
                    ball::Rule rule(NAMES[i], LEVELS[i][0], LEVELS[i][1],
                                             LEVELS[i][2], LEVELS[i][3]);
                    rules.addRule(rule);
                }
            }

            // Add the subset of rules indicated by the bitmask 'mask'.
            ASSERT(rules.numRules()            == mX.addRules(rules));
            ASSERT(bdlb::BitUtil::numBitsSet(mask) ==
                   MX.ruleSet().numRules());
            if (mask != 0) {
                ASSERT(seqNumber < MX.ruleSequenceNumber());
                seqNumber = MX.ruleSequenceNumber();
            }

            ASSERT(0         == mX.addRules(rules));
            ASSERT(seqNumber == MX.ruleSequenceNumber());

            // Verify the relevant rule masks for each category.
            for (int i = 0; i < NUM_NAMES; ++i) {
                if (bdlb::BitUtil::isBitSet(mask, i)) {
                    ball::Rule rule(NAMES[i], LEVELS[i][0], LEVELS[i][1],
                                             LEVELS[i][2], LEVELS[i][3]);
                    int ruleId = MX.ruleSet().ruleId(rule);
                    ASSERT(0 <= ruleId);
                    for (int j = 0; j < NUM_NAMES; ++j) {
                        bool  isSet = i == j;
                        const Entry *cat = MX.lookupCategory(NAMES[j]);
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
            // rules indicated by the bitmask 'mask'.
            ASSERT(bdlb::BitUtil::numBitsSet(endMask & ~mask) ==
                   mX.addRules(allRules));
            ASSERT(bdlb::BitUtil::numBitsSet(endMask) ==mX.ruleSet().numRules());
            ASSERT(0 == mX.addRules(allRules));

            seqNumber = MX.ruleSequenceNumber();

            // Remove those rules indicated by the bitmask 'mask'.  Note that
            // the category manager should have a rule for every category.
            ASSERT(bdlb::BitUtil::numBitsSet(mask) == mX.removeRules(rules));
            ASSERT(bdlb::BitUtil::numBitsSet(~(~endMask | mask)) ==
                                            MX.ruleSet().numRules());

            if (mask != 0) {
                ASSERT(seqNumber < MX.ruleSequenceNumber());
            }
            ASSERT(0 == mX.removeRules(rules));

            // Verify the relevant rule bit mask for each category after we
            // have removed the rules.
            for (int i = 0; i < NUM_NAMES; ++i) {
                if (!bdlb::BitUtil::isBitSet(mask, i)) {
                    ball::Rule rule(NAMES[i], LEVELS[i][0], LEVELS[i][1],
                                             LEVELS[i][2], LEVELS[i][3]);
                    int ruleId = MX.ruleSet().ruleId(rule);
                    ASSERT(0 <= ruleId);
                    for (int j = 0; j < NUM_NAMES; ++j) {
                        bool  isSet = i == j;
                        const Entry *cat = MX.lookupCategory(NAMES[j]);
                        ASSERT(isSet == bdlb::BitUtil::isBitSet(
                                                   cat->relevantRuleMask(),
                                                   ruleId));
                    }
                }
            }

            // Remove all the rules.  Note that this removes the remaining
            // rules, i.e., those rules *not* indicated by the bitmask 'mask'.
            ASSERT(bdlb::BitUtil::numBitsSet(endMask & ~mask) ==
                   mX.removeRules(allRules));
            ASSERT(0 == MX.ruleSet().numRules());
        }
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // TESTING: 'addRule', 'removeRule', 'removeAllRules'
        //
        // Concerns:
        //   'addRule', 'removeRule', 'removeAllRules' update the set of rules
        //   and the relevant rule masks for the affected ball::Category
        //   objects.
        //
        // Plan:
        //    For each category, in a set of test categories, create a rule
        //    and verify: (1) the return value for the 'addRule' method, (2)
        //    that the rule has been added to the category manager's set of
        //    rules (3) that the sequence number has been updated, and (4) that
        //    relevant rule masks for the categories is correct.  Create a
        //    second rule, for the same category (with an additional
        //    predicate), and add it to the category manager.  Verify the same
        //    4 points.  Finally remove the second rule, and verify the same 4
        //    points.
        //
        // Testing:
        //   int addRules(const ball::RuleSet& );
        //   int removeRule(const ball::Rule& );
        //   void removeAllRules();
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Test 'addRule', 'removeRule'  " << endl
                          << "==============================" << endl;

        bslma::TestAllocator ta;
        Obj mX(&ta); const Obj& MX = mX;

        for (int i = 0; i < NUM_NAMES; ++i) {
            mX.addCategory(NAMES[i], LEVELS[i][0], LEVELS[i][1],
                                     LEVELS[i][2], LEVELS[i][3]);
        }

        int seqNumber = MX.ruleSequenceNumber();
        for (int i = 0; i < NUM_NAMES; ++i) {
            ball::Rule rule1(NAMES[i], LEVELS[i][0], LEVELS[i][1],
                                      LEVELS[i][2], LEVELS[i][3]);

            // Add a rule for this category.
            ASSERT(1 == mX.addRule(rule1));
            ASSERT(0 == mX.addRule(rule1));
            ASSERT(++seqNumber == MX.ruleSequenceNumber());

            int ruleId1 = MX.ruleSet().ruleId(rule1);
            ASSERT(0 <= ruleId1);

            for (int j = 0; j < NUM_NAMES; ++j) {
                bool  isSet = i == j;
                const Entry *cat = MX.lookupCategory(NAMES[j]);
                ASSERT(isSet == bdlb::BitUtil::isBitSet(cat->relevantRuleMask(),
                                                       ruleId1));
            }

            ball::Rule rule2(NAMES[i], LEVELS[i][0], LEVELS[i][1],
                                      LEVELS[i][2], LEVELS[i][3]);
            ball::Predicate p2("A", 1);
            rule2.addPredicate(p2);

            // Add a second rule for this category.
            ASSERT(1 == mX.addRule(rule2));
            ASSERT(0 == mX.addRule(rule2));
            ASSERT(++seqNumber == MX.ruleSequenceNumber());

            int ruleId2 = MX.ruleSet().ruleId(rule2);
            ASSERT(0 <= ruleId2)
            for (int j = 0; j < NUM_NAMES; ++j) {
                bool isSet = i == j;
                const Entry *cat = MX.lookupCategory(NAMES[j]);
                ASSERT(isSet == bdlb::BitUtil::isBitSet(cat->relevantRuleMask(),
                                                       ruleId1));
                ASSERT(isSet == bdlb::BitUtil::isBitSet(cat->relevantRuleMask(),
                                                       ruleId2));

            }

            // Remove the second rule for this category.
            ASSERT(1 == mX.removeRule(rule2));
            ASSERT(0 == mX.removeRule(rule2));
            ASSERT(++seqNumber == MX.ruleSequenceNumber());
            for (int j = 0; j < NUM_NAMES; ++j) {
                bool isSet = i == j;
                const Entry *cat = MX.lookupCategory(NAMES[j]);
                ASSERT(isSet == bdlb::BitUtil::isBitSet(cat->relevantRuleMask(),
                                                       ruleId1));
                ASSERT(false == bdlb::BitUtil::isBitSet(cat->relevantRuleMask(),
                                                       ruleId2));
            }
        }

        // Remove all the rules.
        mX.removeAllRules();
        ASSERT(++seqNumber == MX.ruleSequenceNumber());
        for (int i = 0; i < NUM_NAMES; ++i) {
            const Entry *cat = MX.lookupCategory(NAMES[i]);
            ASSERT(0 == cat->relevantRuleMask());
        }
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING MT-SAFETY: ADD AND LOOKUP
        //
        // Concerns:
        //   'addCategory' and 'lookupCategory' should operate as expected in
        //   a multi-threaded environment, particularly on the same object:
        //
        //   - calls to 'lookupCategory' can occur concurrently
        //   - calls to 'addCategory' have transactional integrity
        //   - successful calls to 'lookupCategory' refer to the same object
        //
        // Plan:
        //   Create a category manager 'mX'.  In several "writer" threads, add
        //   categories to 'mX' having various names and threshold levels.  In
        //   several other threads, query 'mX' for each category until all
        //   queries are satisfied.  The result of each call to 'addCategory'
        //   and 'lookupCategory' is stored in a container associated with the
        //   calling thread.  When all threads have completed, the values in
        //   these containers are validated to ensure that
        //
        //   - All categories added via 'addCategory' are unique across all
        //     "write" threads.
        //   - The lists of of categories queried via 'lookupCategory' are
        //     identical across all "query" threads.
        //
        // Testing:
        //   MT-SAFETY
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Test MT-Safety: Add and Lookup" << endl
                          << "==============================" << endl;

        enum {
            NUM_CATEGORIES = 100,    // number of categories to add
            NUM_W_THREADS  = 4,      // number of "write" threads
            NUM_Q_THREADS  = 16,     // number of "query" threads
            NUM_THREADS    = NUM_W_THREADS + NUM_Q_THREADS
        };

        // Generate 'ball::Category' names.
        bsl::string categoryNames[NUM_CATEGORIES];
        for (int i = 0; i < NUM_CATEGORIES; ++i) {
            categoryNames[i] = bitset2string(bsl::bitset<32>(i));
        }

        bslma::TestAllocator ta(veryVeryVerbose);
        Obj                 mX(&ta);
        bslmt::Barrier       barrier(NUM_THREADS);

        struct {
            bslmt::ThreadUtil::Handle d_handle;    // thread handle
            my_ListType              d_results;   // container for results
            my_ThreadParameters      d_params;    // bundled thread parameters
        } threads[NUM_THREADS];

        // Create threads.
        for (int i = 0; i < NUM_THREADS; ++i) {
            threads[i].d_results.reserve(NUM_CATEGORIES);
            threads[i].d_params.d_results_p = &threads[i].d_results;
            threads[i].d_params.d_cm_p = &mX;
            threads[i].d_params.d_names_p = categoryNames;
            threads[i].d_params.d_size = NUM_CATEGORIES;
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
            totalAddedCategories += threads[i].d_results.size();
            if (veryVeryVerbose) {
                T_();
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

        // Merge "write" threads' results into 'results'.
        my_ListType results;
        for (int i = 0; i < NUM_W_THREADS; ++i) {
            my_ListType&   list = threads[i].d_results;
            results.insert(results.end(), list.begin(), list.end());
        }
        bsl::sort(results.begin(), results.end());
        if (veryVerbose) {
            T_(); P(results.size());
        }
        ASSERT(NUM_CATEGORIES == results.size());

        // Validate "query" threads' results against 'results'.
        for (int i = NUM_W_THREADS; i < NUM_THREADS; ++i) {
            if (veryVerbose) {
                T_();
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
      }  break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING INDEXED ACCESS
        //
        // Concerns:
        //   The basic concern is that the indexed access operators work
        //   as expected.
        //
        // Plan:
        //   Create a modifiable 'ball::CategoryManager' object, 'mX', and a
        //   non-modifiable reference to 'mX' named 'X'.  Add categories to
        //   'mX' having various names and threshold level values.  Iterate
        //   over 'mX', using the non-const index operator to change the
        //   threshold level values on 'ball::Category' objects obtained from
        //   'mX'.  Verify that the values are changed using const
        //   'ball::Category' objects obtained from 'X'.  Change the threshold
        //   levels back to their original values using the const index
        //   operator on 'mX', and verify that they were reset, again using
        //   'X'.
        //
        // Testing:
        //   ball::Category& operator[](int index);
        //   const ball::Category& operator[](int index) const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Test Indexed Access" << endl
                          << "===================" << endl;

        bslma::TestAllocator ta;
        Obj                 mX(&ta);
        const Obj&          X = mX;

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

        ASSERT(0 < ta.numAllocations());
        ASSERT(0 < ta.numBytesInUse());
      }  break;
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
        //   side-effects.  In particular, test that 'setLevels',
        //   'addCategory', and 'setThresholdLevels' exhibit the correct
        //   behavior with respect to the modification of existing categories
        //   or the addition of new categories.  Note that it is assumed that
        //   'addCategory' was thoroughly tested in case 2 (Bootstrap) with
        //   valid threshold levels.
        //
        // Testing:
        //   static bool areValidThresholdLevels(int, int, int, int);
        //   int setLevels(int, int, int, int);
        //   ball::Category *addCategory(const char *name, int, int, int, int);
        //   ball::Category *setThresholdLevels(*name, int, int, int, int);
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "Test Manipulators" << endl
                                  << "=================" << endl;

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
                    sprintf(setName + 3, "%03d", i);  // unique name for set
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
        //   The concern is that 'isEnabled' returns the correct value with
        //   various threshold level settings.
        //
        // Plan
        //   Construct a category manager and add a category.  Set the
        //   threshold level settings to different values and verify that in
        //   each case 'isEnabled' returns the correct value.
        //
        // Testing:
        //   bool isEnabled(int) const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "Test 'isEnabled'" << endl
                                  << "================" << endl;

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
            const int LINE             = DATA[i].d_line;
            const int LEVEL            = DATA[i].d_level;
            const int RECORD_LEVEL     = DATA[i].d_recordLevel;
            const int PASS_LEVEL       = DATA[i].d_passLevel;
            const int TRIGGER_LEVEL    = DATA[i].d_triggerLevel;
            const int TRIGGERALL_LEVEL = DATA[i].d_triggerAllLevel;
            const int ENABLED          = DATA[i].d_expIsEnabled;

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

        if (verbose) cout << endl << "Test Direct Accessors" << endl
                                  << "=====================" << endl;

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
        //    3. 'addCategory' adds the expected category.
        //    4. 'setThresholdLevels' sets the appropriate category with the
        //       specified values.
        // Plan:
        //   To address concerns 1a - 1b, create an object using the default
        //   constructor:
        //    - With and without passing in an allocator.
        //    - Verify that in the presence of 'bslma::TestAllocator', the
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

        if (verbose) cout << endl << "Bootstrap Test" << endl
                                  << "==============" << endl;

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
        //   First, create a 'ball::CategoryManager' instance.  Add a category
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

        if (verbose) cout << endl << "BREATHING TEST" << endl
                                  << "==============" << endl;

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        Obj mX;  const Obj& X = mX;

        ASSERT(0 == X.length());

        if (veryVerbose) {
            cout << "Add category '" << VA << "' with:";
            T_(); P_(LA[0]); P_(LA[1]); P_(LA[2]); P(LA[3]);
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
            T_(); P_(LB[0]); P_(LB[1]); P_(LB[2]); P(LB[3]);
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
