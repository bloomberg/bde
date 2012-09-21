// bael_attributecontext.t.cpp            -*-C++-*-

#include <bael_attributecontext.h>

#include <bael_attributecontainer.h>
#include <bael_categorymanager.h>
#include <bael_attributecontainer.h>
#include <bael_defaultobserver.h>
#include <bael_testobserver.h>            // for testing only

#include <bcema_blob.h>
#include <bcemt_barrier.h>
#include <bcemt_thread.h>

#include <bslma_allocator.h>
#include <bslma_testallocator.h>
#include <bslma_defaultallocatorguard.h>
#include <bsls_assert.h>
#include <bsls_platform.h>         // placement 'new' syntax

#include <bsl_iostream.h>
#include <bsl_new.h>         // placement 'new' syntax
#include <bsl_set.h>
#include <bsl_string.h>
#include <bsl_strstream.h>
#include <bsl_vector.h>

#include <bsl_c_stdlib.h>    // rand_r

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// The component under test manages a thread-specific attribute map and
// provides an interface ('hasRelevantActiveRules' and
// 'determineThresholdLevels') to determine
// whether a given category is active and what the aggregated threshold levels
// should be.  We must ensure that 1) basic attribute manipulation works as
// expected, (2) different attribute contexts don't interfere with each other,
// and (3) 'hasRelevantActiveRules' and 'determineThresholdLevels' must return
// the expected value.
//-----------------------------------------------------------------------------
// [1] static bael_AttributeContext *getContext(Allocator *ba = 0);
// [1] static bael_AttributeContext *lookupContext();
// [2] int addAttribute(const bael_Attribute& value);
// [2] int removeAttribute(const bael_Attribute& value);
// [2] bool hasAttribute(const bael_Attribute& value) const;
// [2] int numAttributes() const;
// [3] const bael_RuleSet::MaskType& resultMask() const;
// [3] const bael_RuleSet::MaskType& evalMask() const;
// [3] bool hasRelevantActiveRules(const Cat *cat);
// [3] void determineThresholdLevels(TL *lvls, const Cat *cat);
//-----------------------------------------------------------------------------
// [4] USAGE EXAMPLE
//=============================================================================
//                      STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
static int testStatus = 0;

bcemt_Mutex coutMutex;                    // mutex for output

void aSsErT(int c, const char *s, int i)
{
    if (c) {
        bcemt_LockGuard<bcemt_Mutex> guard(&coutMutex);
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (0 <= testStatus && testStatus <= 100) ++testStatus;
    }
}

#define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

//=============================================================================
//                  STANDARD BDE LOOP-ASSERT TEST MACROS
//-----------------------------------------------------------------------------
#define LOOP_ASSERT(I,X) { \
   if (!(X)) { cout << #I << ": " << I << "\n"; aSsErT(1, #X, __LINE__); }}

#define LOOP2_ASSERT(I,J,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " \
              << J << "\n"; aSsErT(1, #X, __LINE__); } }

//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", "<< flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number
#define T_()  cout << "\t" << flush;          // Print tab w/o newline


// The following macros facilitate thread-safe streaming to standard output.

#define MTCOUT   { coutMutex.lock(); cout \
                                           << bcemt_ThreadUtil::selfIdAsInt() \
                                           << ": "
#define MTENDL   endl << bsl::flush ;  coutMutex.unlock(); }
#define MTFLUSH  bsl::flush; } coutMutex.unlock()

#define PT(X) { MTCOUT << #X " = " << (X) << MTENDL; }
#define PT2(X,Y) { MTCOUT << #X " = " << (X) << ", " \
                          << #Y " = " << (Y) << MTENDL; }
#define PT3(X,Y,Z) { MTCOUT << #X " = " << (X) << ", " \
                            << #Y " = " << (Y) << ", " \
                            << #Z " = " << (Z) << MTENDL; }
                                                 // Print identifier and value.


//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS/VARIABLES FOR TESTING
//-----------------------------------------------------------------------------

typedef bael_AttributeContext Obj;

int verbose;
int veryVerbose;
int veryVeryVerbose;

bslma_TestAllocator testAllocator;

//=============================================================================
//                          CLASSES FOR TESTING
//-----------------------------------------------------------------------------


struct AttributeComparator {
    bool operator()(const bael_Attribute& lhs, const bael_Attribute& rhs) const
        // Return 'true' if the specified 'lhs' is ordered before the
        // specified 'rhs'.  In the interests of readable results, this
        // comparator orders attributes by name, then value type, then value.
    {
        int cmp = bsl::strcmp(lhs.name(), rhs.name());
        if (0 != cmp) {
            return cmp < 0;
        }
        if (lhs.value().typeIndex() != rhs.value().typeIndex()) {
            return lhs.value().typeIndex() < rhs.value().typeIndex();
        }
        switch (lhs.value().typeIndex()) {
          case 0: // unset?
            ASSERT(false);
            return true;
          case 1: // int
            return lhs.value().the<int>() < rhs.value().the<int>();
          case 2: // int64
            return lhs.value().the<bsls_PlatformUtil::Int64>()
                <  rhs.value().the<bsls_PlatformUtil::Int64>();
          case 3: // string
            return lhs.value().the<bsl::string>()
                <  rhs.value().the<bsl::string>();
        }
        BSLS_ASSERT(false);
        return false;
    }
};

class AttributeSet : public bael_AttributeContainer {
    // A simple set based implementation of the 'bael_AttributeContainer'
    // protocol used for testing.

    bsl::set<bael_Attribute, AttributeComparator> d_set;

    // NOT IMPLEMENTED
    AttributeSet(const AttributeSet&);
    AttributeSet& operator=(const AttributeSet&);

  public:
    // CREATORS
    AttributeSet(bslma_Allocator *basicAllocator = 0);
        // Create this attribute set.

    virtual ~AttributeSet();
        // Destroy this attribute set.

    // MANIPULATORS
    void insert(const bael_Attribute& value);
        // Add the specified value to this attribute set.

    bool remove(const bael_Attribute& value);
        // Remove the specified value from this attribute set, return
        // 'true' if the attribute was found, and 'false' if 'value' was not
        // a member of this set.


    // ACCESSORS
    virtual bool hasValue(const bael_Attribute& value) const;
        // Return 'true' if the attribute having specified 'value' exists in
        // this object, and 'false' otherwise.

    virtual bsl::ostream& print(bsl::ostream& stream,
                                int           level = 0,
                                int           spacesPerLevel = 4) const;
        // Format this object to the specified output 'stream' at the
        // (absolute value of) the optionally specified indentation 'level'
        // and return a reference to 'stream'.  If 'level' is specified,
        // optionally specify 'spacesPerLevel', the number of spaces per
        // indentation level for this and all of its nested objects.  If
        // 'level' is negative, suppress indentation of the first line.  If
        // 'spacesPerLevel' is negative, format the entire output on one line,
        // suppressing all but the initial indentation (as governed by
        // 'level').  If 'stream' is not valid on entry, this operation has no
        // effect.
};

// CREATORS
inline
AttributeSet::AttributeSet(bslma_Allocator *basicAllocator)
: d_set(AttributeComparator(), basicAllocator)
{
}

AttributeSet::~AttributeSet()
{
}

// MANIPULATORS
void AttributeSet::insert(const bael_Attribute& value)
{
    d_set.insert(value);
}

bool AttributeSet::remove(const bael_Attribute& value)
{
    bsl::set<bael_Attribute, AttributeComparator>::iterator it =
                                                            d_set.find(value);
    if (it != d_set.end()) {
        d_set.erase(it);
        return true;
    }
    return false;
}


// ACCESSORS
bool AttributeSet::hasValue(const bael_Attribute& value) const
{
    return d_set.find(value) != d_set.end();
}

bsl::ostream& AttributeSet::print(bsl::ostream& stream,
                                  int           level,
                                  int           spacesPerLevel) const
{
    char EL = (spacesPerLevel < 0) ? ' ' : '\n';
    bdeu_Print::indent(stream, level, spacesPerLevel);
    stream << "[" << EL;

    bsl::set<bael_Attribute>::const_iterator it = d_set.begin();
    for (; it != d_set.end(); ++it) {
        it->print(stream, level+1, spacesPerLevel);
    }
    bdeu_Print::indent(stream, level, spacesPerLevel);
    stream << "]" << EL;
    return stream;
}

//=============================================================================
//                             USAGE EXAMPLE
//-----------------------------------------------------------------------------

namespace BAEL_ATTRIBUTECONTEXT_USAGE_EXAMPLE
{

///Managing Attributes
///- - - - - - - - - -
// First we define a thread function that will create and install two
// attributes.
//
// Note that we use the 'AttributeSet' implementation of the
// 'bael_AttributeContainer' protocol defined in the component documentation
// for 'bael_attributecontainer'.
//..
    extern "C" void *thread1(void*)
    {
//..
// Inside the thread function, we create an attribute set to hold our
// attribute values, and we create two 'bael_Attribute' objects.
//..
        AttributeSet attributes;
        bael_Attribute a1("uuid", 4044457);
        bael_Attribute a2("name", "Gang Chen");
        attributes.insert(a1);
        attributes.insert(a2);
//..
// Next we obtain a reference to the current threads attribute context using
// the 'getContext()' class method:
//..
        bael_AttributeContext *context = bael_AttributeContext::getContext();
        ASSERT(context);
        ASSERT(context == bael_AttributeContext::lookupContext());
//..
// We can add our attribute container, 'attributes', to the current context
// using the 'addAttributes' method.  We store the returned iterator so that
// we can remove the attributes before they go out of scope and are destroyed:
//..
        bael_AttributeContext::iterator it =
                                           context->addAttributes(&attributes);
//..
        ASSERT(context->hasAttribute(a1));
        ASSERT(context->hasAttribute(a2));
//..
// We then call the 'removeAttributes' method to remove the attributes from
// the attribute context:
//..
        context->removeAttributes(it);
        ASSERT(false == context->hasAttribute(a1));
        ASSERT(false == context->hasAttribute(a2));
//..
// This completes the first thread function:
//..
        return 0;
    }
//..
// The second thread function will simply verify that there is no currently
// available attribute context.  Note that attribute contexts are created and
// managed by individual threads using thread-specific storage, and that
// attribute contexts created by one thread is not visible in any other
// threads:
//..
    extern "C" void *thread2(void*)
    {
        ASSERT(0 == bael_AttributeContext::lookupContext());
        return 0;
    }

} // close namespace BAEL_ATTRIBUTECONTEXT_USAGE_EXAMPLE

//=============================================================================
//                         CASE 6 RELATED ENTITIES
//-----------------------------------------------------------------------------
namespace BAEL_ATTRIBUTECONTEXT_TEST_CASE_6
{

bcemt_Barrier barrier(2);

struct WorkerThreadArgs {
    bael_CategoryManager *d_categoryManager;
};

extern "C" void *workerThread1(void *)
{
    bael_AttributeContext * context = bael_AttributeContext::getContext();
    ASSERT(context);
    ASSERT(context == bael_AttributeContext::lookupContext());

    bael_Attribute a1("uuid", 4044457);
    bael_Attribute a2("name", "Gang Chen");

    barrier.wait();

    // Note that we must use the thread-safe 'global' allocator (default
    // allocator is *not* thread safe).
    AttributeSet attributes;
    attributes.insert(a1);
    attributes.insert(a2);

    bael_AttributeContext::iterator it = context->addAttributes(&attributes);

    ASSERT(context->hasAttribute(a1));
    ASSERT(context->hasAttribute(a2));

    barrier.wait();

    context->removeAttributes(it);

    ASSERT(false == context->hasAttribute(a1));
    ASSERT(false == context->hasAttribute(a2));

    barrier.wait();

    return NULL;
}

extern "C" void *workerThread2(void *)
{
    barrier.wait();
    ASSERT(0 == bael_AttributeContext::lookupContext());
    barrier.wait();
    ASSERT(0 == bael_AttributeContext::lookupContext());
    barrier.wait();
    ASSERT(0 == bael_AttributeContext::lookupContext());
    return NULL;
}

extern "C" void *workerThread3(void *args)
{
    bael_CategoryManager *manager =
                               ((WorkerThreadArgs *)args)->d_categoryManager;

    const bael_Category *cat1 =
                            manager->addCategory("weekday", 128, 96, 64, 32);

    const bael_Category *cat2 =
                            manager->addCategory("weekend", 125, 100, 75, 50);

    bael_Rule rule("week*", 120, 110, 70, 40);

    manager->addRule(rule);

    // Note that we must use the thread-safe 'global' allocator (default
    // allocator is *not* thread safe).
    AttributeSet attributes;

    bael_AttributeContext *attrContext = bael_AttributeContext::getContext();
    bael_Attribute attr("uuid", 1);
    attributes.insert(attr);
    Obj::iterator iter = attrContext->addAttributes(&attributes);

    ASSERT(true == attrContext->hasRelevantActiveRules(cat1));
    ASSERT(true == attrContext->hasRelevantActiveRules(cat2));

    bael_ThresholdAggregate levels(0, 0, 0, 0);
    attrContext->determineThresholdLevels(&levels, cat1);
    ASSERT(128 == levels.recordLevel());
    ASSERT(110 == levels.passLevel());
    ASSERT(70  == levels.triggerLevel());
    ASSERT(40  == levels.triggerAllLevel());
    attrContext->determineThresholdLevels(&levels, cat2);
    ASSERT(125 == levels.recordLevel());
    ASSERT(110 == levels.passLevel());
    ASSERT(75  == levels.triggerLevel());
    ASSERT(50  == levels.triggerAllLevel());

    manager->removeRule(rule);


    ASSERT(false == attrContext->hasRelevantActiveRules(cat1));
    ASSERT(false == attrContext->hasRelevantActiveRules(cat2));
    attrContext->determineThresholdLevels(&levels, cat1);
    ASSERT(128 == levels.recordLevel());
    ASSERT(96  == levels.passLevel());
    ASSERT(64  == levels.triggerLevel());
    ASSERT(32  == levels.triggerAllLevel());
    attrContext->determineThresholdLevels(&levels, cat2);
    ASSERT(125 == levels.recordLevel());
    ASSERT(100 == levels.passLevel());
    ASSERT(75  == levels.triggerLevel());
    ASSERT(50  == levels.triggerAllLevel());


    bael_Predicate pred1("uuid", 1);
    rule.addPredicate(pred1);
    manager->addRule(rule);

    ASSERT(true == attrContext->hasRelevantActiveRules(cat1));
    ASSERT(true == attrContext->hasRelevantActiveRules(cat2));

    attrContext->determineThresholdLevels(&levels, cat1);
    ASSERT(128 == levels.recordLevel());
    ASSERT(110 == levels.passLevel());
    ASSERT(70  == levels.triggerLevel());
    ASSERT(40  == levels.triggerAllLevel());
    attrContext->determineThresholdLevels(&levels, cat2);
    ASSERT(125 == levels.recordLevel());
    ASSERT(110 == levels.passLevel());
    ASSERT(75  == levels.triggerLevel());
    ASSERT(50  == levels.triggerAllLevel());

    manager->removeRule(rule);
    rule.setPattern("weekend");
    manager->addRule(rule);

    ASSERT(false == attrContext->hasRelevantActiveRules(cat1));
    ASSERT(true  == attrContext->hasRelevantActiveRules(cat2));
    attrContext->determineThresholdLevels(&levels, cat1);
    ASSERT(128 == levels.recordLevel());
    ASSERT(96  == levels.passLevel());
    ASSERT(64  == levels.triggerLevel());
    ASSERT(32  == levels.triggerAllLevel());
    attrContext->determineThresholdLevels(&levels, cat2);
    ASSERT(125 == levels.recordLevel());
    ASSERT(110 == levels.passLevel());
    ASSERT(75  == levels.triggerLevel());
    ASSERT(50  == levels.triggerAllLevel());

    manager->removeRule(rule);
    rule.removePredicate(pred1);
    bael_Predicate pred2("uuid", 2);
    rule.addPredicate(pred2);
    manager->addRule(rule);

    ASSERT(false == attrContext->hasRelevantActiveRules(cat1));
    ASSERT(false == attrContext->hasRelevantActiveRules(cat2));
    attrContext->determineThresholdLevels(&levels, cat1);
    ASSERT(128 == levels.recordLevel());
    ASSERT(96  == levels.passLevel());
    ASSERT(64  == levels.triggerLevel());
    ASSERT(32  == levels.triggerAllLevel());
    attrContext->determineThresholdLevels(&levels, cat2);
    ASSERT(125 == levels.recordLevel());
    ASSERT(100 == levels.passLevel());
    ASSERT(75  == levels.triggerLevel());
    ASSERT(50  == levels.triggerAllLevel());
    return NULL;
}

} // namespace BAEL_ATTRIBUTECONTEXT_TEST_CASE_6

//=============================================================================
//                         CASE 4 RELATED ENTITIES
//-----------------------------------------------------------------------------
namespace BAEL_ATTRIBUTECONTEXT_TEST_CASE_4
{

enum {
    NUM_RULETHREADS    =  2, // number of threads adding/deleting rules
    NUM_CONTEXTTHREADS =  4, // number of threads verifying attribute contexts
    NUM_TESTS          = 10, // number of repetition
    NUM_THREADS = NUM_RULETHREADS + NUM_CONTEXTTHREADS
};


bcemt_Barrier barrier(NUM_THREADS);   // synchronizing threads

struct WorkerThreadArgs {
    bael_CategoryManager *d_categoryManager;
    unsigned int          d_seed;
};

int randomValue(unsigned int *seed)
    // Return a random value based on the specified 'seed'.
{
#ifdef BSLS_PLATFORM_OS_WINDOWS
    // 'rand' is thread-safe on windows when linked with the multi-threaded
    // CRT (common run-time) library (which we must be doing anyway).
    return rand();
#else
    return rand_r(seed);
#endif
}

extern "C" void *case4RuleThread(void *args)
{
    bael_CategoryManager *manager =
                               ((WorkerThreadArgs *)args)->d_categoryManager;
    unsigned int          seed = ((WorkerThreadArgs *)args)->d_seed;


    // Note that we must use the thread-safe 'global' allocator (default
    // allocator is *not* thread safe).
    bael_RuleSet ruleSet;

    barrier.wait();
    if (verbose) {
        MTCOUT << "\n\tPhase 1: Randomly add rules "
               << "until the rule set is full" << MTENDL;
    }

    // Create all rules to be added in a rule set.

    for (int i = 0; i < bael_RuleSet::maxNumRules(); ++i) {
        string pattern(i + 1, 'a');
        pattern += '*';
        bael_Rule rule(pattern.c_str(), 0, i + 1, i + 1, i + 1);

        for(int j = 0; j < bael_RuleSet::maxNumRules() - i; ++j) {
            bael_Predicate predicate("uuid", j);
            rule.addPredicate(predicate);
        }
        ruleSet.addRule(rule);
    }

    // Adding rules.

    while (manager->ruleSet().numRules() != bael_RuleSet::maxNumRules()) {
        int r = randomValue(&seed) % ruleSet.numRules();
        // 1/3 chance to remove a rule, 2/3 chance to add a rule

        if (randomValue(&seed) % 3) {
            manager->addRule(*ruleSet.getRuleById(r));
            while(0 == manager->addRule(*ruleSet.getRuleById(r))
             && manager->ruleSet().numRules() != bael_RuleSet::maxNumRules()) {
                r = randomValue(&seed) % ruleSet.numRules();
            }
        }
        else {
            manager->removeRule(*ruleSet.getRuleById(r));
        }
    }

    // The rule set owned by the logger manager should be full when all
    // 'rule' threads arrive here.

    barrier.wait();
    if (verbose) {
        MTCOUT << "\n\tPhase 2: Wait for 'AttributeContext' threads."
               << MTENDL;
    }

    barrier.wait();
    if (verbose) {
        MTCOUT <<"\n\tPhase 3: Randomly remove rules until the rule set"
               << " is empty." << MTENDL;
    }

    while (manager->ruleSet().numRules()) {
        int r = randomValue(&seed) % ruleSet.numRules();
        // 1/3 chance to add a rule, 2/3 chance to remove a rule
        if (randomValue(&seed) % 3) {
            while(0 == manager->removeRule(*ruleSet.getRuleById(r))
               && manager->ruleSet().numRules()) {
                r = randomValue(&seed) % ruleSet.numRules();
            }
        }
        else {
            manager->addRule(*ruleSet.getRuleById(r));
        }
    }

    barrier.wait();
    if (verbose) {
        MTCOUT << "\n\tPhase 4: Wait for 'AttributeContext' threads."
               << MTENDL;
    }

    return NULL;
}

struct ContextThreadData
{
    unsigned int d_seed;
    bslma_TestAllocator * d_allocator_p;
};

extern "C" void *case4ContextThread(void *args)
{
    bael_CategoryManager *manager =
                               ((WorkerThreadArgs *)args)->d_categoryManager;

    barrier.wait();
    if (verbose) {
        MTCOUT << "\n\tPhase 1: Initializing attribute contexts "
               << "and creating categories." << MTENDL;
    }

    // Creating an empty attribute context.

    ASSERT(0 == Obj::lookupContext());
    Obj* context = Obj::getContext();
    ASSERT(context);
    ASSERT(context == Obj::lookupContext());


    // Verify that the same context is observed in difference scopes.

    ASSERT(context == Obj::getContext());
    {
        ASSERT(context == Obj::lookupContext());
        ASSERT(context == Obj::getContext());
    }
    ASSERT(context == Obj::lookupContext());

    // Add categories C0, C1, C2, ... C32 whose names are "", "a", "aa", etc.

    const int NUM_CATEGORIES = bael_RuleSet::maxNumRules() + 1;
    vector<const bael_Category*> CATEGORIES(NUM_CATEGORIES);
    for (int i = 0; i < NUM_CATEGORIES; ++i) {
        string name(i, 'a');
        CATEGORIES[i] =
                  manager->setThresholdLevels(name.c_str(), i + 1, 0, 0, 0);
        LOOP_ASSERT(i, CATEGORIES[i]);
    }

    // Note that we must use the thread-safe 'global' allocator (default
    // allocator is *not* thread safe).
    AttributeSet attributes;

    // Add attributes uuid = 0, uuid = 1, ..., uuid = 31.
    for (int i = 0; i < NUM_CATEGORIES - 1; ++i) {
        bael_Attribute attribute("uuid", i);
        attributes.insert(attribute);
    }

    Obj::iterator iterator = context->addAttributes(&attributes);

    // Verify that all attributes exist and they cannot be added again.
    for (int i = 0; i < NUM_CATEGORIES - 1; ++i) {
        bael_Attribute attribute("uuid", i);
        LOOP_ASSERT(i, true == context->hasAttribute(attribute));
    }

    // Loop until there are 32 active categories.
    int numActiveCategories = 0;
    while(numActiveCategories != NUM_CATEGORIES - 1) {
        numActiveCategories = 0;
        for (int i = 1; i < NUM_CATEGORIES; ++i) {
            numActiveCategories +=
                context->hasRelevantActiveRules(CATEGORIES[i]) ? 1 : 0;
        }
    }

    barrier.wait();
    if (verbose) {
        MTCOUT << "\n\tPhase 2: Verify 'hasRelevantActiveRules' and "
               << "'determineThresholdLevels'."
               << MTENDL;
    }

    // All categories except the first one must be active now.
    ASSERT(false == context->hasRelevantActiveRules(CATEGORIES[0]));
    for (int i = 1; i < NUM_CATEGORIES; ++i) {
        LOOP_ASSERT(i, context->hasRelevantActiveRules(CATEGORIES[i]));
    }

    // Categories C0 has no relevant rules, so 'determineThresholdLevels'
    // should return its own threshold levels.
    bael_ThresholdAggregate levels(0, 0, 0, 0);
    context->determineThresholdLevels(&levels, CATEGORIES[0]);
    ASSERT(CATEGORIES[0]->recordLevel()     == levels.recordLevel());
    ASSERT(CATEGORIES[0]->passLevel()       == levels.passLevel());
    ASSERT(CATEGORIES[0]->triggerLevel()    == levels.triggerLevel());
    ASSERT(CATEGORIES[0]->triggerAllLevel() == levels.triggerAllLevel());

    // For other categories, the recordLevel is obtained from the category
    // itself, while all other levels should come from the corresponding
    // 'largest' relevant rule.

    for (int i = 1; i < NUM_CATEGORIES; ++i) {
        context->determineThresholdLevels(&levels, CATEGORIES[i]);
        LOOP_ASSERT(i, CATEGORIES[i]->recordLevel() == levels.recordLevel());
        LOOP_ASSERT(i, i == levels.passLevel());
        LOOP_ASSERT(i, i == levels.triggerLevel());
        LOOP_ASSERT(i, i == levels.triggerAllLevel());
    }

    // Now remove attributes one by one.  Verify that every time an attribute
    // is removed the corresponding category will be affected.

    for (int i = 0; i < NUM_CATEGORIES - 1; ++i) {
        bael_Attribute attribute("uuid", NUM_CATEGORIES - 2 - i);
        attributes.remove(attribute);
        context->removeAttributes(iterator);
        iterator = context->addAttributes(&attributes);

        if (veryVeryVerbose) {
            MTCOUT << "\n\t\tRemoving attribute uuid = "
                   << NUM_CATEGORIES - 2 - i << MTENDL;
        }

        for (int j = 0; j < NUM_CATEGORIES; ++j) {
            LOOP2_ASSERT(i, j,  j - 1 > i ==
                              context->hasRelevantActiveRules(CATEGORIES[j]));

            context->determineThresholdLevels(&levels, CATEGORIES[j]);

            if (veryVeryVerbose) {
                MTCOUT << "\n\t\tCategory " << j
                       << " -- hasRelevantActiveRules: "
                       << context->hasRelevantActiveRules(CATEGORIES[j])
                       << " getThreasholdLevels: " << levels
                       << MTENDL;
            }

            LOOP2_ASSERT(i, j,
                         CATEGORIES[j]->recordLevel() == levels.recordLevel());
            int level = j - 1 > i ? j : 0;
            LOOP2_ASSERT(i, j, level == levels.passLevel());
            LOOP2_ASSERT(i, j, level == levels.triggerLevel());
            LOOP2_ASSERT(i, j, level == levels.triggerAllLevel());

        }
    }

    // Verify that there are no attributes.

    for (int i = 0; i < NUM_CATEGORIES - 1; ++i) {
        bael_Attribute attribute("uuid", i);
        LOOP_ASSERT(i, false == context->hasAttribute(attribute));
    }

    // Add all the attributes back.
    for (int i = 0; i < NUM_CATEGORIES - 1; ++i) {
        attributes.insert(bael_Attribute("uuid", i));
    }
    context->removeAttributes(iterator);
    iterator = context->addAttributes(&attributes);

    // All categories except the first one must be active now.
    ASSERT(false == context->hasRelevantActiveRules(CATEGORIES[0]));
    for (int i = 1; i < NUM_CATEGORIES; ++i) {
        LOOP_ASSERT(i, context->hasRelevantActiveRules(CATEGORIES[i]));
    }

    // Categories C0 has no relevant rules, so 'determineThresholdLevels'
    // should return its own threshold levels.

    context->determineThresholdLevels(&levels, CATEGORIES[0]);
    ASSERT(CATEGORIES[0]->recordLevel()     == levels.recordLevel());
    ASSERT(CATEGORIES[0]->passLevel()       == levels.passLevel());
    ASSERT(CATEGORIES[0]->triggerLevel()    == levels.triggerLevel());
    ASSERT(CATEGORIES[0]->triggerAllLevel() == levels.triggerAllLevel());

    // For other categories, the recordLevel is obtained from the category
    // itself, while all other levels should come from the corresponding
    // 'largest' relevant rule.

    for (int i = 1; i < NUM_CATEGORIES; ++i) {
        context->determineThresholdLevels(&levels, CATEGORIES[i]);
        LOOP_ASSERT(i, CATEGORIES[i]->recordLevel() == levels.recordLevel());
        LOOP_ASSERT(i, i == levels.passLevel());
        LOOP_ASSERT(i, i == levels.triggerLevel());
        LOOP_ASSERT(i, i == levels.triggerAllLevel());
    }

    barrier.wait();
    if (verbose) {
        MTCOUT <<"\n\tPhase 3: Wait until all rules are removed."
               << MTENDL;
    }

    numActiveCategories = NUM_CATEGORIES - 1;
    while(numActiveCategories != 0) {
        numActiveCategories = 0;
        for (int i = 1; i < NUM_CATEGORIES; ++i) {
            numActiveCategories +=
                  context->hasRelevantActiveRules(CATEGORIES[i]) ? 1 : 0;
        }
    }

    barrier.wait();
    if (verbose) {
         MTCOUT << "\n\tPhase 4: Checking categories again."
                << MTENDL;
    }

    // There should not be any active categories.

    for (int i = 0; i < NUM_CATEGORIES; ++i) {
        LOOP_ASSERT(i,
                    false == context->hasRelevantActiveRules(CATEGORIES[i]));
        context->determineThresholdLevels(&levels, CATEGORIES[i]);
        LOOP_ASSERT(i, CATEGORIES[i]->recordLevel()
                                                == levels.recordLevel());
        LOOP_ASSERT(i, CATEGORIES[i]->passLevel()
                                                == levels.passLevel());
        LOOP_ASSERT(i, CATEGORIES[i]->triggerLevel()
                                                == levels.triggerLevel());
        LOOP_ASSERT(i, CATEGORIES[i]->triggerAllLevel()
                                                == levels.triggerAllLevel());
    }

    return NULL;
}

} // namespace BAEL_ATTRIBUTECONTEXT_TEST_CASE_4

//=============================================================================
//                         CASE 3 RELATED ENTITIES
//-----------------------------------------------------------------------------
namespace BAEL_ATTRIBUTECONTEXT_TEST_CASE_3
{

const int NUM_THREADS =  6;           // number of threads
const int NUM_TESTS   = 10;           // number of tests

bael_Attribute A0("", "12345678");
bael_Attribute A1("", 12345678);
bael_Attribute A2("", (bsls_PlatformUtil::Int64)12345678);
bael_Attribute A3("uuid", "12345678");
bael_Attribute A4("uuid", 12345678);
bael_Attribute A5("uuid", (bsls_PlatformUtil::Int64)12345678);
bael_Attribute A6("UUID", "12345678");
bael_Attribute A7("UUID", 12345678);
bael_Attribute A8("UUID", (bsls_PlatformUtil::Int64)12345678);

const bael_Attribute ATTRS[] = { A0, A1, A2, A3, A4, A5, A6, A7, A8 };

const int NUM_ATTRS = sizeof ATTRS / sizeof *ATTRS;

extern "C" void *case3ContextThread(void *arg)
{
    // Creating an empty attribute context.

    ASSERT(0 == Obj::lookupContext());
    Obj* context = Obj::getContext();
    ASSERT(context);
    ASSERT(context == Obj::lookupContext());

    // Note that we must use the thread-safe 'global' allocator (default
    // allocator is *not* thread safe).
    AttributeSet attributes;
    Obj::iterator it = context->addAttributes(&attributes);
    for (int i = 0; i < NUM_ATTRS; ++i) {
        attributes.insert(ATTRS[i]);
        context->removeAttributes(it);
        context->addAttributes(&attributes);

        LOOP_ASSERT(i, context->hasAttribute(ATTRS[i]));
        for (int j = 0; j < NUM_ATTRS; ++j) {
            if (j <= i) {
                LOOP2_ASSERT(i, j, true == context->hasAttribute(ATTRS[j]));
            }
            else {
                LOOP2_ASSERT(i, j, false == context->hasAttribute(ATTRS[j]));
            }
        }
    }

    for (int i = 0; i < NUM_ATTRS; ++i) {
        attributes.remove(ATTRS[i]);
        context->removeAttributes(it);
        context->addAttributes(&attributes);

        LOOP_ASSERT(i, false == context->hasAttribute(ATTRS[i]));
        for (int j = 0; j < NUM_ATTRS; ++j) {
            if (j <= i) {
                LOOP2_ASSERT(i, j, false == context->hasAttribute(ATTRS[j]));
            }
            else {
                LOOP2_ASSERT(i, j, true == context->hasAttribute(ATTRS[j]));
            }
        }
    }

    return NULL;
}

} // namespace BAEL_ATTRIBUTECONTEXT_TEST_CASE_3

//=============================================================================
//                         CASE 2 RELATED ENTITIES
//-----------------------------------------------------------------------------
namespace BAEL_ATTRIBUTECONTEXT_TEST_CASE_2
{

const int NUM_THREADS =  6;           // number of threads
const int NUM_TESTS   = 10;           // number of tests

bcemt_Barrier barrier(NUM_THREADS);   // synchronizing threads

extern "C" void *case2ContextThread(void *arg)
{

    // Creating an empty attribute context.

    barrier.wait();

    ASSERT(0 == Obj::lookupContext());
    Obj* context = Obj::getContext();
    ASSERT(context);
    ASSERT(context == Obj::lookupContext());


    // Verify that the same context is observed in difference scopes.

    ASSERT(context == Obj::getContext());
    {
        ASSERT(context == Obj::lookupContext());
        ASSERT(context == Obj::getContext());
    }
    ASSERT(context == Obj::lookupContext());

    barrier.wait();

    return (void*)context;
}

} // namespace BAEL_ATTRIBUTECONTEXT_TEST_CASE_2

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;

    verbose = argc > 2;
    veryVerbose = argc > 3;
    veryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;;


    bslma_Allocator *Z = &testAllocator;
    bslma_TestAllocator defaultAllocator;
    bslma_DefaultAllocatorGuard guard(&defaultAllocator);
    bslma_Allocator *globalAllocator = bslma_Default::globalAllocator(0);

    switch (test) { case 0:  // Zero is always the leading case.
      case 7: {
        // --------------------------------------------------------------------
        // TESTING USAGE EXAMPLE
        //
        // Concerns:
        //   The usage example provided in the component header file must
        //   compile, link, and run on all platforms as shown.
        //
        // Plan:
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "Testing Usage Example" << endl
                                  << "=====================" << endl;


        bslma_DefaultAllocatorGuard guard(globalAllocator);
        using namespace BAEL_ATTRIBUTECONTEXT_USAGE_EXAMPLE;

//..
///Calling 'hasRelevantActiveRules' and 'determineThresholdLevels'
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// In this example we demonstrate how to call the 'hasRelevantActiveRules'
// and 'determineThresholdLevels' methods.  These methods are used
// (primarily by other components in the 'bael' package) to determine the
// affect of the current logging rules on the logging thresholds of a
// category.  Note that a rule is "relevant" if the rule's pattern matches
// the category's name, and a rule is "active" if 'bael_Rule::evaluate()'
// returns 'true' for the collection of attributes maintained for the current
// thread by the thread's 'bael_AttributeContext' object.
//
// We start by creating a 'bael_CategoryManager' and use it to initialize the
// static data members of 'bael_AttributeContext'.  Note that, in practice,
// this initialization *should* *not* be performed by clients of the 'bael'
// package':  The 'bael_LoggerManager' singleton will initialize the
// 'bael_AttributeContext' with the when the singleton is created.
//..
    bael_CategoryManager categoryManager;
    bael_AttributeContext::initialize(&categoryManager);  // this is normally
                                                          // performed by the
                                                          // bael_LoggerManager
//..
// Next we add a category to the category manager.  Each created category
// has a name and the logging threshold levels for that category.  The logging
// threshold levels indicate the minimum severity for logged message that
// will trigger the relevant action.  The four thresholds are the "record
// level" (messages logged with a higher severity than this threshold should
// be added to the current logger's record buffer), the "passthrough level"
// (messages logged with a severity higher than this threshold should be
// published immediately), the "trigger level" (messages logged with a higher
// severity than this threshold should trigger the publication of the entire
// contents of the current logger's record buffer), and the "trigger-all
// level" (messages logged with a higher severity than this threshold should
// trigger the publication of every logger's record buffer), respectively.
// Note that, clients are generally most interested in the "passthrough"
// threshold level.  Also note that a higher number indicates a lower
// severity.
//..
    const bael_Category *cat1 =
               categoryManager.addCategory("MyCategory", 128, 96, 64, 32);
//..
// Next we obtain the context for the current thread.
//..
    bael_AttributeContext *context = bael_AttributeContext::getContext();
//..
// We call the 'hasRelevantActiveRules' method on 'cat1'.  This will be 'false'
// because we haven't supplied any rules.
//..
// ASSERT(false == context->hasRelevantActiveRules(cat1));
//..
// We call the 'determineThresholdLevels' method on 'cat1'.  This will simply
// return the logging threshold levels we defined for 'cat1' when it was
// created because no rules have been defined that might modify those
// thresholds:
//..
    bael_ThresholdAggregate cat1ThresholdLevels(0, 0, 0, 0);
    context->determineThresholdLevels(&cat1ThresholdLevels, cat1);
    ASSERT(128 == cat1ThresholdLevels.recordLevel());
    ASSERT(96  == cat1ThresholdLevels.passLevel());
    ASSERT(64  == cat1ThresholdLevels.triggerLevel());
    ASSERT(32  == cat1ThresholdLevels.triggerAllLevel());
//..
// Next we create a rule that will apply to those categories whose names match
// the pattern "My*", where '*' is a wild-card value.  The rule defines a
// set of thresholds levels that may override the threshold levels of those
// categories whose name matches the rule's pattern:
//..
    bael_Rule myRule("My*", 120, 110, 70, 40);
    categoryManager.addRule(myRule);
//..
// Now we call the 'hasRelevantActiveRules' method again for 'cat1', but this
// time the method returns 'true' because the rule we just added is both
// "relevant" to 'cat1', and "active".  'myRule' is "relevant" to 'cat1',
// because the name of 'cat1' ("MyCategory") matches the pattern for 'myRule'
// ("My*") (i.e., 'myRule' applies to 'cat1').  'myRule' is also "active"
// because all the
// predicates defined for the rule are satisfied by the current thread (in
// this case the rule has no predicates, so the rule is always "active").
// Note that, we will discuss the meaning of "active" and the use of
// predicates later in this example.
//..
    ASSERT(true == context->hasRelevantActiveRules(cat1));
//..
// Next we call the 'determineThresholdLevels' method for 'cat1'.  The
// 'determineThresholdLevels' method compares the threshold levels defined
// for category with those of any active rules that apply to that category,
// and determines the maximum value (i.e., the minimum severity), for each
// respective threshold amongst those values.
//..
    bael_ThresholdAggregate thresholdLevels(0, 0, 0, 0);
    context->determineThresholdLevels(&thresholdLevels, cat1);
    ASSERT(128 == thresholdLevels.recordLevel());
    ASSERT(110 == thresholdLevels.passLevel());
    ASSERT(70  == thresholdLevels.triggerLevel());
    ASSERT(40  == thresholdLevels.triggerAllLevel());
//..
// In this case the "passthrough", "trigger", and "trigger-all" threshold
// levels defined by 'myRule' (110, 70, and 40) are greater (i.e., define a
// lower severity) than those respective values defined for 'cat1' (96, 64,
// and 32), so those values override the values defined for 'cat1'.  On the
// other hand the "record" threshold level for 'cat1' (128) is greater than
// the value defined by 'myRule' (120), so the threshold level for defined
// for 'cat1' is  returned.  In effect, 'myRule' has lowered the severity at
// which messages logged in the "MyCategory" category will be published
// immediately, trigger the publication of the current logger's record buffer,
// and trigger the publication of every logger's record buffer.
//
// Next we modify 'myRule', adding a predicate indicating that the rule should
// only apply if the attribute context for the current thread contains the
// attribute '("uuid", 3938908)':
//..
    categoryManager.removeRule(myRule);
    bael_Predicate predicate("uuid", 3938908);
    myRule.addPredicate(predicate);
    categoryManager.addRule(myRule);
//..
// When we again call the 'hasRelevantActiveRules' method for 'cat1', it now
// returns 'false'.  The rule, 'myRule', still applies to 'cat1' (i.e., it is
// still "relevant" to 'cat1') but the predicates defined by 'myRule' are no
// longer satisfied by the current thread, i.e., the current threads attribute
// context does not contain an attribute matching '("uuid", 3938908)'.
//..
    ASSERT(false == context->hasRelevantActiveRules(cat1));
//..
// Next we call the 'determineThresholdLevels' method on 'cat1' and find that
// it returns the threshold levels we defined for 'cat1' when we created it:
//..
    context->determineThresholdLevels(&thresholdLevels, cat1);
    ASSERT(thresholdLevels  == cat1ThresholdLevels);
//..
// Finally, we add an attribute to the current thread's attribute
// context (as we did in the first example, "Managing Attributes").  Note that
// we keep an iterator referring to the added attributes so that we can remove
// them before 'attributest' goes out of scope and is destroyed.  Also note
// that the class 'AttributeSet' is defined in the component documentation for
// 'bael_attributecontainer'.
//..
    AttributeSet attributes;
    attributes.insert(bael_Attribute("uuid", 3938908));
    bael_AttributeContext::iterator it = context->addAttributes(&attributes);
//..
// The following call to the 'hasRelevantActiveRules' method will return 'true'
// for 'cat1' because there is at least one rule, 'myRule', that is both
// "relevant" (i.e., its pattern matches the category name of 'cat1') and
// "active" (i.e., all of the predicates defined for 'myRule' are satisfied by
// the attributes held by this thread's attribute context).
//..
    ASSERT(true == context->hasRelevantActiveRules(cat1));
//..
// Now when we call the 'determineThresholdLevels' method, it will again return
// the maximum threshold level from 'cat1' an 'myRule'.
//..
    context->determineThresholdLevels(&thresholdLevels, cat1);
    ASSERT(128 == thresholdLevels.recordLevel());
    ASSERT(110 == thresholdLevels.passLevel());
    ASSERT(70  == thresholdLevels.triggerLevel());
    ASSERT(40  == thresholdLevels.triggerAllLevel());
//..
// We must be careful to remove 'attributes' from the attribute context
// before it goes out of scope and is destroyed.  Note that the 'bael'
// package supplies a component, 'bael_scopedattributes' for adding, and
// automatically removing, attributes from the current thread's attribute
// context.
//..
// context->removeAttributes(it);
//..


      } break;

      case 6: {
        // --------------------------------------------------------------------
        // TESTING ORIGINAL USAGE EXAMPLE
        //
        // Concerns:
        //   This test runs the original usage example for this
        //   component.  It remains in the test driver for completeness.
        // Plan:
        //
        // Testing:
        //
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "Testing (Old) Usage Example" << endl
                                  << "===========================" << endl;

        bslma_DefaultAllocatorGuard guard(globalAllocator);
        using namespace BAEL_ATTRIBUTECONTEXT_TEST_CASE_6;

        bael_CategoryManager manager;
        bael_AttributeContext::initialize(&manager);

        BAEL_ATTRIBUTECONTEXT_TEST_CASE_6::WorkerThreadArgs args =
                                                                 { &manager };

        bcemt_ThreadUtil::Handle threads[3];
        bcemt_ThreadUtil::create(&threads[0], workerThread1, NULL);
        bcemt_ThreadUtil::create(&threads[1], workerThread2, NULL);
        bcemt_ThreadUtil::create(&threads[2], workerThread3, &args);
        bcemt_ThreadUtil::join(threads[0]);
        bcemt_ThreadUtil::join(threads[1]);
        bcemt_ThreadUtil::join(threads[2]);


        const bael_Category *cat1 =
                       manager.setThresholdLevels("weekday", 128, 96, 64, 32);

        const bael_Category *cat2 =
                       manager.setThresholdLevels("weekend", 125, 100, 75, 50);

        bael_Rule rule("week*", 120, 110, 70, 40);
        manager.addRule(rule);

        bael_AttributeContext *attrContext =
                                   bael_AttributeContext::getContext();
        bael_Attribute attr("uuid", 1);

        AttributeSet attributes;
        attributes.insert(attr);
        bael_AttributeContext::iterator it =
                                       attrContext->addAttributes(&attributes);

        ASSERT(attrContext->hasRelevantActiveRules(cat1));
        ASSERT(attrContext->hasRelevantActiveRules(cat2));

        bael_ThresholdAggregate levels(0, 0, 0, 0);

        attrContext->determineThresholdLevels(&levels, cat1);
        ASSERT(128 == levels.recordLevel());
        ASSERT(110 == levels.passLevel());
        ASSERT(70  == levels.triggerLevel());
        ASSERT(40  == levels.triggerAllLevel());

        attrContext->determineThresholdLevels(&levels,  cat2);
        ASSERT(125 == levels.recordLevel());
        ASSERT(110 == levels.passLevel());
        ASSERT(75  == levels.triggerLevel());
        ASSERT(50  == levels.triggerAllLevel());

        manager.removeRule(rule);
        ASSERT(false == attrContext->hasRelevantActiveRules(cat1));
        ASSERT(false == attrContext->hasRelevantActiveRules(cat2));

        attrContext->determineThresholdLevels(&levels, cat1);
        ASSERT(128 == levels.recordLevel());
        ASSERT(96  == levels.passLevel());
        ASSERT(64  == levels.triggerLevel());
        ASSERT(32  == levels.triggerAllLevel());

        attrContext->determineThresholdLevels(&levels, cat2);
        ASSERT(125 == levels.recordLevel());
        ASSERT(100 == levels.passLevel());
        ASSERT(75  == levels.triggerLevel());
        ASSERT(50  == levels.triggerAllLevel());

        bael_Predicate pred1("uuid", 1);
        rule.addPredicate(pred1);
        manager.addRule(rule);

        ASSERT(attrContext->hasRelevantActiveRules(cat1));
        ASSERT(attrContext->hasRelevantActiveRules(cat2));

        attrContext->determineThresholdLevels(&levels, cat1);
        ASSERT(128 == levels.recordLevel());
        ASSERT(110 == levels.passLevel());
        ASSERT(70  == levels.triggerLevel());
        ASSERT(40  == levels.triggerAllLevel());

        attrContext->determineThresholdLevels(&levels, cat2);
        ASSERT(125 == levels.recordLevel());
        ASSERT(110 == levels.passLevel());
        ASSERT(75  == levels.triggerLevel());
        ASSERT(50  == levels.triggerAllLevel());

        manager.removeRule(rule);
        rule.setPattern("weekend");
        manager.addRule(rule);
        ASSERT(false == attrContext->hasRelevantActiveRules(cat1));
        ASSERT(true  == attrContext->hasRelevantActiveRules(cat2));

        attrContext->determineThresholdLevels(&levels, cat1);
        ASSERT(128 == levels.recordLevel());
        ASSERT(96  == levels.passLevel());
        ASSERT(64  == levels.triggerLevel());
        ASSERT(32  == levels.triggerAllLevel());

        attrContext->determineThresholdLevels(&levels, cat2);
        ASSERT(125 == levels.recordLevel());
        ASSERT(110 == levels.passLevel());
        ASSERT(75  == levels.triggerLevel());
        ASSERT(50  == levels.triggerAllLevel());

        manager.removeRule(rule);
        rule.removePredicate(pred1);
        bael_Predicate pred2("uuid", 2);
        rule.addPredicate(pred2);
        manager.addRule(rule);

        ASSERT(false == attrContext->hasRelevantActiveRules(cat1));
        ASSERT(false == attrContext->hasRelevantActiveRules(cat2));

        attrContext->determineThresholdLevels(&levels, cat1);
        ASSERT(128 == levels.recordLevel());
        ASSERT(96  == levels.passLevel());
        ASSERT(64  == levels.triggerLevel());
        ASSERT(32  == levels.triggerAllLevel());

        attrContext->determineThresholdLevels(&levels, cat2);
        ASSERT(125 == levels.recordLevel());
        ASSERT(100 == levels.passLevel());
        ASSERT(75  == levels.triggerLevel());
        ASSERT(50  == levels.triggerAllLevel());

      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING ALLOCATION
        //
        // Concerns:
        //   Verify the allocator supplied to 'initialize' is used to allocate
        //   attribute context objects.
        //
        // Plan:
        //   First verify that if the 'initialize' method is *not* invoked, the
        //   installed global allocator is used by 'getContext()'.  Then
        //   invoke the 'initialize' method and verify the supplied allocator
        //   is used to construct the context returned by 'getContext()'.  Use
        //   'bael_AttributeContextProctor' to delete the context, and verify
        //   that its memory has been released.
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "Test memory allocation\n"
                          << "======================\n";

        bslma_TestAllocator globalAllocator;
        bslma_Default::setGlobalAllocator(&globalAllocator);
        {
            {
                ASSERT(0 == testAllocator.numBytesInUse());
                ASSERT(0 == globalAllocator.numBytesInUse());
                ASSERT(0 == defaultAllocator.numBytesInUse());

                Obj::getContext();

                ASSERT(0 < globalAllocator.numBytesInUse());
                ASSERT(0 == defaultAllocator.numBytesInUse());
                ASSERT(0 == testAllocator.numBytesInUse());
            }
            ASSERT(0 < globalAllocator.numBytesInUse());
            ASSERT(0 == defaultAllocator.numBytesInUse());
            ASSERT(0 == testAllocator.numBytesInUse());

            bael_AttributeContextProctor proctor;
        }
        ASSERT(0 == globalAllocator.numBytesInUse());
        ASSERT(0 == defaultAllocator.numBytesInUse());
        ASSERT(0 == testAllocator.numBytesInUse());

        {

            bael_CategoryManager manager(Z);
            bael_AttributeContext::initialize(&manager, Z);
            Obj::getContext();


            ASSERT(0 == globalAllocator.numBytesInUse());
            ASSERT(0 == defaultAllocator.numBytesInUse());
            ASSERT(0 < testAllocator.numBytesInUse());

            bael_AttributeContextProctor proctor;

          }
        ASSERT(0 == globalAllocator.numBytesInUse());
        ASSERT(0 == defaultAllocator.numBytesInUse());
        ASSERT(0 == testAllocator.numBytesInUse());


      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING 'hasRelevantActiveRules' and 'determineThresholdLevels'
        //
        // Concerns:
        //   'hasRelevantActiveRules' and 'determineThresholdLevels' must
        //   return the correct status even in a multi-threaded environment.
        //
        // Plan:
        //   Assume that the maximum number of rules allowed in a rule set is
        //   32.  Create 32 rules R0, R1, ..., R31 and 33 categories C0, C1,
        //   ..., C31, C32. Set up the category names and rule pattern such
        //   that Ri is a relevant rule to Cj if i < j.  Also add to Ri all
        //   predicates {"uuid" = j | 0 <= j < 32 - i}.  The relation between
        //   these categories and rules can be illustrated as follows (an 'X'
        //   denotes the rule is relevant to the category):
        //
        //       R0      R1      R2    ...   R29      R30      R31
        //--------------------------------------------------------------
        // C0                          ...
        // C1    X                     ...
        // C2    X       X             ...
        // C3    X       X       X     ...
        // ...                         ...
        // C30   X       X       X     ...   X
        // C31   X       X       X     ...   X        X
        // C32   X       X       X     ...   X        X        X
        //--------------------------------------------------------------
        //    uuid=0  uuid=0  uuid=0   ... uuid=0   uuid=0   uuid=0
        //    uuid=1  uuid=1  uuid=1   ... uuid=1   uuid=1
        //    uuid=2  uuid=2  uuid=2   ... uuid=2
        //                             ...
        //    uuid=29 uuid=29 uuid=29
        //    uuid=30 uuid=30
        //    uuid=31
        //
        //   There are two types of threads running concurrently.  One type of
        //   threads, called 'rule' threads, will create all those rules, add
        //   them to the ruleset one by one, and then remove all the rules.
        //   In the meantime, another type of threads, called 'context'
        //   threads, will add all the categories to the logger manager
        //   singleton, verify that 'hasRelevantActiveRules' and
        //   'determineThresholdLevels' return expected values.  All threads
        //   are synchronized by a barrier which divides the running stage
        //   into 4 phases.
        //
        // Testing:
        //    const bael_RuleSet::MaskType& resultMask() const;
        //    const bael_RuleSet::MaskType& evalMask() const;
        //    static bool hasRelevantActiveRules(AC **context, const Cat *cat);
        //    static void determineThresholdLevels(AC **, const Cat *, TL *);
        // --------------------------------------------------------------------

        if (verbose) cout
            << endl
            << "Test 'hasRelevantActiveRules' and 'determineThresholdLevels'\n"
            << "============================================================"
            << endl;

        bslma_DefaultAllocatorGuard guard(globalAllocator);
        using namespace BAEL_ATTRIBUTECONTEXT_TEST_CASE_4;

        unsigned int ruleThreadSeeds[NUM_RULETHREADS];
        ContextThreadData contextThreadData[NUM_CONTEXTTHREADS];

        bcemt_ThreadUtil::Handle ruleThreads[NUM_RULETHREADS];
        bcemt_ThreadUtil::Handle contextThreads[NUM_CONTEXTTHREADS];

        bael_CategoryManager manager;
        bael_AttributeContext::initialize(&manager);

        BAEL_ATTRIBUTECONTEXT_TEST_CASE_4::WorkerThreadArgs
                                               ruleThreadArgs[NUM_RULETHREADS];
        BAEL_ATTRIBUTECONTEXT_TEST_CASE_4::WorkerThreadArgs
                                         contextThreadArgs[NUM_CONTEXTTHREADS];

        unsigned int seed = 0;
        for (int i = 0; i < NUM_TESTS; ++i)
        {
            for (int j = 0; j < NUM_RULETHREADS; ++j) {
                ruleThreadArgs[j].d_categoryManager = &manager;
                ruleThreadArgs[j].d_seed = seed;
                bcemt_ThreadUtil::create(&ruleThreads[j],
                                         case4RuleThread,
                                         (void*)&ruleThreadArgs[j]);
            }

            bslma_TestAllocator testAllocators[NUM_CONTEXTTHREADS];
            for (int j = 0; j < NUM_CONTEXTTHREADS; ++j) {
                contextThreadArgs[j].d_categoryManager = &manager;
                contextThreadArgs[j].d_seed = seed;
                bcemt_ThreadUtil::create(&contextThreads[j],
                                         case4ContextThread,
                                         (void*)&contextThreadArgs[j]);
            }

            for (int j = 0; j < NUM_RULETHREADS; ++j) {
                bcemt_ThreadUtil::join(ruleThreads[j]);
            }

            for (int j = 0; j < NUM_CONTEXTTHREADS; ++j) {
                bcemt_ThreadUtil::join(contextThreads[j]);
            }
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING ATTRIBUTE MANIPULATION
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        //    static int addAttribute(const bael_Attribute& value);
        //    static int removeAttribute(const bael_Attribute& value);
        //    static bool hasAttribute(const bael_Attribute& value);
        //    int numAttributes() const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Test Attribute Manipulation\n"
                          << "===========================\n";

        bslma_DefaultAllocatorGuard guard(globalAllocator);
        using namespace BAEL_ATTRIBUTECONTEXT_TEST_CASE_3;

        bcemt_ThreadUtil::Handle Threads[NUM_THREADS];

        for (int i = 0; i < NUM_TESTS; ++i)
        {
            for (int j = 0; j < NUM_THREADS; ++j) {
                bcemt_ThreadUtil::create(&Threads[j],
                                         case3ContextThread,
                                         (void*)0);
            }

            void* contexts[NUM_THREADS];

            for (int j = 0; j < NUM_THREADS; ++j) {
                bcemt_ThreadUtil::join(Threads[j], NULL);
            }

        }

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING ATTRIBUTE CONTEXT CREATION
        //
        // Concerns:
        //   Attribute contexts must be created properly on a per-thread basis.
        //
        // Plan:
        //   Create a number of independent but synchronized threads, each of
        //   which will create its own 'bael_AttributeContext' object.  Verify
        //   that within each thread, only on 'bael_AttributeContext' can be
        //   created and that 'lookupContext()' should correctly return
        //   the 'bael_AttributeContext' that has been created even in
        //   different scopes.  Also verify that the 'bael_AttributeContext'
        //   objects created by different threads are distinct.
        //
        // Testing:
        //    static bael_AttributeContext *getContext(Allocator *ba = 0);
        //    static bael_AttributeContext *lookupContext();
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Test Attribute Context Creation\n"
                          << "===============================\n";

        bslma_DefaultAllocatorGuard guard(globalAllocator);
        using namespace BAEL_ATTRIBUTECONTEXT_TEST_CASE_2;

        bcemt_ThreadUtil::Handle Threads[NUM_THREADS];
        Obj* contexts[NUM_THREADS];

        if (verbose) cout << "\n\tWithout passing an allocator" << endl;

        for (int i = 0; i < NUM_TESTS; ++i)
        {
            for (int j = 0; j < NUM_THREADS; ++j) {
                bcemt_ThreadUtil::create(&Threads[j],
                                         case2ContextThread,
                                         (void*)0);
            }

            void* contexts[NUM_THREADS];

            for (int j = 0; j < NUM_THREADS; ++j) {
                bcemt_ThreadUtil::join(Threads[j], &contexts[j]);
            }

            for (int j = 0; j < NUM_THREADS; ++j) {
            for (int k = 0; k < NUM_THREADS; ++k) {
                LOOP2_ASSERT(j, k, (j == k) == (contexts[j] == contexts[k]));
            }
            }

        }

        if (verbose) cout << "\n\tWith an allocator" << endl;

        for (int i = 0; i < NUM_TESTS; ++i)
        {
            bslma_TestAllocator testAllocators[NUM_THREADS];

            for (int j = 0; j < NUM_THREADS; ++j) {
                bcemt_ThreadUtil::create(&Threads[j],
                                         case2ContextThread,
                                         (void*)&testAllocators[j]);
            }

            void* contexts[NUM_THREADS];

            for (int j = 0; j < NUM_THREADS; ++j) {
                bcemt_ThreadUtil::join(Threads[j], &contexts[j]);
            }

            for (int j = 0; j < NUM_THREADS; ++j) {
            for (int k = 0; k < NUM_THREADS; ++k) {
                LOOP2_ASSERT(j, k, (j == k) == (contexts[j] == contexts[k]));
            }
            }
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // TESTING HELPER CLASSES
        //
        // Concerns:
        //   That the helper classes defined in this test driver behave
        //   correctly.
        //
        // Plan:
        //
        // Testing:
        //    AttributeSet
        // --------------------------------------------------------------------
        typedef bsls_PlatformUtil::Int64 Int64;
        const char *VALUES[]   = { "A", "B", "C", "D", "E" };
        const int   NUM_VALUES = sizeof VALUES/sizeof *VALUES;

        if (verbose) cout << endl
                          << "Test Helper Classes\n"
                          << "===================\n";


        {
            if (veryVerbose)
                cout  << "\tPrimary method test (w/ test allocator)\n";

            AttributeSet mX(Z); const AttributeSet& MX = mX;

            for (int i = 0; i < NUM_VALUES; ++i) {
                mX.insert(bael_Attribute("uuid", i));
                mX.insert(bael_Attribute("uuid", (Int64)i));
                mX.insert(bael_Attribute("name", VALUES[i]));
                if (veryVerbose) {  P(MX); }
                for (int j = 0; j < NUM_VALUES; ++j) {
                    bool inc = j <= i;
                    ASSERT(inc == MX.hasValue(bael_Attribute("uuid", j)));
                    ASSERT(inc == MX.hasValue(bael_Attribute("uuid",
                                                             (Int64)j)));
                    ASSERT(inc == MX.hasValue(bael_Attribute("name",
                                                             VALUES[j])));
                }
            }
            ASSERT(0 == defaultAllocator.numBytesInUse());

            for (int i = 0; i < NUM_VALUES; ++i) {
                mX.remove(bael_Attribute("uuid", i));
                mX.remove(bael_Attribute("uuid", (Int64)i));
                mX.remove(bael_Attribute("name", VALUES[i]));
                if (veryVerbose) { P(MX); }
                for (int j = 0; j < NUM_VALUES; ++j) {
                    bool inc = j > i;
                    ASSERT(inc == MX.hasValue(bael_Attribute("uuid", j)));
                    ASSERT(inc == MX.hasValue(bael_Attribute("uuid",
                                                             (Int64)j)));
                    ASSERT(inc == MX.hasValue(bael_Attribute("name",
                                                             VALUES[j])));
                }
            }
            ASSERT(0 == defaultAllocator.numBytesInUse());
        }
        if (veryVerbose)
            cout  << "\tTest use of default allocator\n";

        ASSERT(0 == testAllocator.numBytesInUse());
        AttributeSet mY; const AttributeSet& MY = mY;
        mY.insert(bael_Attribute("uuid", 1));
        mY.insert(bael_Attribute("uuid",  "A"));

        ASSERT(MY.hasValue(bael_Attribute("uuid", 1)));
        ASSERT(MY.hasValue(bael_Attribute("uuid",  "A")));

        ASSERT(0  < defaultAllocator.numBytesInUse());
        ASSERT(0 == testAllocator.numBytesInUse());

        mY.remove(bael_Attribute("uuid", 1));
        mY.remove(bael_Attribute("uuid",  "A"));

        ASSERT(!MY.hasValue(bael_Attribute("uuid", 1)));
        ASSERT(!MY.hasValue(bael_Attribute("uuid",  "A")));


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

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2004
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
