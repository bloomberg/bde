// ball_attributecontext.t.cpp                                        -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <ball_attributecontext.h>

#include <ball_attributecontainer.h>
#include <ball_categorymanager.h>
#include <ball_predicate.h>
#include <ball_rule.h>
#include <ball_testobserver.h>
#include <ball_thresholdaggregate.h>

#include <bslim_printer.h>
#include <bslim_testutil.h>

#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_newdeleteallocator.h>
#include <bslma_testallocator.h>
#include <bslma_testallocatormonitor.h>

#include <bslmt_barrier.h>
#include <bslmt_lockguard.h>
#include <bslmt_mutex.h>

#include <bsls_asserttest.h>
#include <bsls_objectbuffer.h>
#include <bsls_platform.h>
#include <bsls_review.h>

#include <bsl_cstddef.h>
#include <bsl_cstdlib.h>
#include <bsl_cstring.h>
#include <bsl_iostream.h>
#include <bsl_new.h>         // placement 'new' syntax
#include <bsl_set.h>
#include <bsl_string.h>
#include <bsl_vector.h>

#include <bsl_c_stdlib.h>    // rand_r

using namespace BloombergLP;
using namespace bsl;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// The component under test manages a thread-specific attribute map and
// provides an interface ('hasRelevantActiveRules' and
// 'determineThresholdLevels') to determine whether a given category is active
// and what the aggregated threshold levels should be.  We must ensure that 1)
// basic attribute manipulation works as expected, (2) different attribute
// contexts don't interfere with each other, and (3) 'hasRelevantActiveRules'
// and 'determineThresholdLevels' must return the expected values.
//-----------------------------------------------------------------------------
// ball::AttributeContext:
// [ 2] static void initialize(CategoryManager *cm, Allocator *gA = 0);
// [ 2] static void reset();
// [ 2] static AttributeContext *getContext();
// [ 2] static AttributeContext *lookupContext();
// [ 3] iterator addAttributes(const AttributeContainer *attributes);
// [ 4] void clearCache();
// [ 3] void removeAttributes(iterator element);
// [ 4] bool hasRelevantActiveRules(const Cat *cat) const;
// [ 4] void determineThresholdLevels(TL *lvls, const Cat *cat) const;
// [ 3] bool hasAttribute(const Attribute& value) const;
// [ 3] const AttributeContainerList& containers() const;
// [  ] bsl::ostream& print(bsl::ostream& stream, int level, int spl) const;
// [  ] bsl::ostream& operator<<(bsl::ostream&, const AttributeContext&);
//
// ball::AttributeContextProctor:
// [ 5] AttributeContextProctor();
// [ 5] ~AttributeContextProctor();
//-----------------------------------------------------------------------------
// [ 1] AttributeSet
// [ 6] CONCERN: No false positives from 'hasRelevantActiveRules'.
// [ 7] (OLD) USAGE EXAMPLE
// [ 8] USAGE EXAMPLE 1
// [ 9] USAGE EXAMPLE 2

//=============================================================================
//                      STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------

static int testStatus = 0;

bslmt::Mutex coutMutex;                    // mutex for output

void aSsErT(int c, const char *s, int i)
{
    if (c) {
        bslmt::LockGuard<bslmt::Mutex> guard(&coutMutex);
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (0 <= testStatus && testStatus <= 100) ++testStatus;
    }
}

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

// ============================================================================
//                  NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT_SAFE_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(EXPR)
#define ASSERT_SAFE_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPR)
#define ASSERT_PASS(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS(EXPR)
#define ASSERT_FAIL(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL(EXPR)
#define ASSERT_OPT_PASS(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS(EXPR)
#define ASSERT_OPT_FAIL(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL(EXPR)

// The following macros facilitate thread-safe streaming to standard output.

#define MTCOUT   { coutMutex.lock(); cout << bslmt::ThreadUtil::selfIdAsInt() \
                                          << ": "
#define MTENDL   endl << bsl::flush; coutMutex.unlock(); }

#define MTFLUSH  bsl::flush; } coutMutex.unlock()

#define PT(X)      { MTCOUT << #X " = " << (X) << MTENDL; }
#define PT2(X,Y)   { MTCOUT << #X " = " << (X) << ", "                        \
                            << #Y " = " << (Y) << MTENDL; }
#define PT3(X,Y,Z) { MTCOUT << #X " = " << (X) << ", "                        \
                            << #Y " = " << (Y) << ", "                        \
                            << #Z " = " << (Z) << MTENDL; }
                                                 // Print identifier and value.

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS/VARIABLES FOR TESTING
//-----------------------------------------------------------------------------

typedef ball::AttributeContext       Obj;

typedef ball::AttributeContainerList List;
typedef ball::CategoryManager        CatMngr;

typedef bsls::Types::Int64           Int64;

bool verbose;
bool veryVerbose;
bool veryVeryVerbose;
bool veryVeryVeryVerbose;

//=============================================================================
//                          CLASSES FOR TESTING
//-----------------------------------------------------------------------------

struct AttributeComparator {

    // ACCESSORS
    bool operator()(const ball::Attribute& lhs,
                    const ball::Attribute& rhs) const
        // Return 'true' if the specified 'lhs' is ordered before the
        // specified 'rhs', and 'false' otherwise.  Note that in the interest
        // of readable results, this comparator first orders attributes by
        // name, then by value type, then by value.
    {
        const int cmp = bsl::strcmp(lhs.name(), rhs.name());
        if (0 != cmp) {
            return cmp < 0;                                           // RETURN
        }

        if (lhs.value().typeIndex() != rhs.value().typeIndex()) {
            return lhs.value().typeIndex() < rhs.value().typeIndex(); // RETURN
        }

        switch (lhs.value().typeIndex()) {
          case 0: // unset?
            BSLS_ASSERT_OPT(false);
            return true;                                              // RETURN
          case 1: // 'int'
            return lhs.value().the<int>() < rhs.value().the<int>();   // RETURN
          case 2: // 'Int64'
            return lhs.value().the<Int64>() < rhs.value().the<Int64>();
                                                                      // RETURN
          case 3: // string
            return lhs.value().the<bsl::string>()
                                   < rhs.value().the<bsl::string>();  // RETURN
        }
        BSLS_ASSERT_OPT(false);
        return false;
    }
};

class AttributeSet : public ball::AttributeContainer {
    // This class provides a simple set-based implementation of the
    // 'ball::AttributeContainer' protocol used for testing.

    // DATA
    bsl::set<ball::Attribute, AttributeComparator> d_set;

  private:
    // NOT IMPLEMENTED
    AttributeSet(const AttributeSet&);
    AttributeSet& operator=(const AttributeSet&);

  public:
    // CREATORS
    AttributeSet(bslma::Allocator *basicAllocator = 0);
        // Create an attribute set.

    virtual ~AttributeSet();
        // Destroy this attribute set.

    // MANIPULATORS
    void insert(const ball::Attribute& value);
        // Add the specified 'value' to this attribute set.

    bool remove(const ball::Attribute& value);
        // Remove the specified 'value' from this attribute set.  Return 'true'
        // if the attribute was found in this set, and 'false' otherwise.

    // ACCESSORS
    virtual bool hasValue(const ball::Attribute& value) const;
        // Return 'true' if the attribute having the specified 'value' exists
        // in this set, and 'false' otherwise.

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
AttributeSet::AttributeSet(bslma::Allocator *basicAllocator)
: d_set(AttributeComparator(), basicAllocator)
{
}

AttributeSet::~AttributeSet()
{
}

// MANIPULATORS
void AttributeSet::insert(const ball::Attribute& value)
{
    d_set.insert(value);
}

bool AttributeSet::remove(const ball::Attribute& value)
{
    bsl::set<ball::Attribute, AttributeComparator>::iterator it =
                                                            d_set.find(value);
    if (it != d_set.end()) {
        d_set.erase(it);
        return true;                                                  // RETURN
    }
    return false;
}

// ACCESSORS
bool AttributeSet::hasValue(const ball::Attribute& value) const
{
    return d_set.find(value) != d_set.end();
}

bsl::ostream& AttributeSet::print(bsl::ostream& stream,
                                  int           level,
                                  int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();

    bsl::set<ball::Attribute>::const_iterator it = d_set.begin();
    for (; it != d_set.end(); ++it) {
        printer.printValue(*it);
    }
    printer.end();
    return stream;
}

//=============================================================================
//                             USAGE EXAMPLE
//-----------------------------------------------------------------------------

namespace BALL_ATTRIBUTECONTEXT_USAGE_EXAMPLE_2 {

struct ThreadArgs {
    CatMngr *d_categoryManager_p;
};

extern "C" void *usageExample2(void *args)
{
    CatMngr  *manager =
                     reinterpret_cast<ThreadArgs *>(args)->d_categoryManager_p;
    CatMngr&  categoryManager = *manager;

// Next, we add a category to the category manager.  Each created category has
// a name and the logging threshold levels for that category.  The logging
// threshold levels indicate the minimum severity for logged messages that will
// trigger the relevant action.  The four thresholds are the "record level"
// (messages logged with a higher severity than this threshold should be added
// to the current logger's record buffer), the "pass-through level" (messages
// logged with a severity higher than this threshold should be published
// immediately), the "trigger level" (messages logged with a higher severity
// than this threshold should trigger the publication of the entire contents of
// the current logger's record buffer), and the "trigger-all level" (messages
// logged with a higher severity than this threshold should trigger the
// publication of every logger's record buffer), respectively.  Note that
// clients are generally most interested in the "pass-through" threshold level.
// Also note that a higher number indicates a lower severity.
//..
    const ball::Category *cat1 =
                    categoryManager.addCategory("MyCategory", 128, 96, 64, 32);
//..
// Next we obtain the context for the current thread.
//..
    ball::AttributeContext *context = ball::AttributeContext::getContext();
//..
// We call 'hasRelevantActiveRules' on 'cat1'.  This will be 'false' because
// we haven't supplied any rules:
//..
    ASSERT(!context->hasRelevantActiveRules(cat1));
//..
// We call 'determineThresholdLevels' on 'cat1'.  This will simply return the
// logging threshold levels we defined for 'cat1' when it was created because
// no rules have been defined that might modify those thresholds:
//..
    ball::ThresholdAggregate cat1ThresholdLevels(0, 0, 0, 0);
    context->determineThresholdLevels(&cat1ThresholdLevels, cat1);
    ASSERT(128 == cat1ThresholdLevels.recordLevel());
    ASSERT( 96 == cat1ThresholdLevels.passLevel());
    ASSERT( 64 == cat1ThresholdLevels.triggerLevel());
    ASSERT( 32 == cat1ThresholdLevels.triggerAllLevel());
//..
// Next, we create a rule that will apply to those categories whose names match
// the pattern "My*", where '*' is a wild-card value.  The rule defines a set
// of thresholds levels that may override the threshold levels of those
// categories whose name matches the rule's pattern:
//..
    ball::Rule myRule("My*", 120, 110, 70, 40);
    categoryManager.addRule(myRule);
//..
// Now, we call 'hasRelevantActiveRules' again for 'cat1', but this time the
// method returns 'true' because the rule we just added is both "relevant" to
// 'cat1' and "active".  'myRule' is "relevant" to 'cat1' because the name of
// 'cat1' ("MyCategory") matches the pattern for 'myRule' ("My*") (i.e.,
// 'myRule' applies to 'cat1').  'myRule' is also "active" because all the
// predicates defined for the rule are satisfied by the current thread (in this
// case the rule has no predicates, so the rule is always "active").  Note
// that we will discuss the meaning of "active" and the use of predicates later
// in this example.
//..
    ASSERT(context->hasRelevantActiveRules(cat1));
//..
// Next, we call 'determineThresholdLevels' for 'cat1'.  This method compares
// the threshold levels defined for a category with those of any active rules
// that apply to that category, and determines the minimum severity (i.e., the
// maximum numerical value) for each respective threshold amongst those values.
//..
    ball::ThresholdAggregate thresholdLevels(0, 0, 0, 0);
    context->determineThresholdLevels(&thresholdLevels, cat1);
    ASSERT(128 == thresholdLevels.recordLevel());
    ASSERT(110 == thresholdLevels.passLevel());
    ASSERT( 70 == thresholdLevels.triggerLevel());
    ASSERT( 40 == thresholdLevels.triggerAllLevel());
//..
// In this case the "pass-through", "trigger", and "trigger-all" threshold
// levels defined by 'myRule' (110, 70, and 40) are greater (i.e., define a
// lower severity) than those respective values defined for 'cat1' (96, 64,
// and 32), so those values override the values defined for 'cat1'.  On the
// other hand, the "record" threshold level for 'cat1' (128) is greater than
// the value defined by 'myRule' (120), so the threshold level defined for
// 'cat1' is  returned.  In effect, 'myRule' has lowered the severity at which
// messages logged in the "MyCategory" category will be published immediately,
// trigger the publication of the current logger's record buffer, and trigger
// the publication of every logger's record buffer.
//
// Next we modify 'myRule', adding a predicate indicating that the rule should
// only apply if the attribute context for the current thread contains the
// attribute '("uuid", 3938908)':
//..
    categoryManager.removeRule(myRule);
    ball::Predicate predicate("uuid", 3938908);
    myRule.addPredicate(predicate);
    categoryManager.addRule(myRule);
//..
// When we again call 'hasRelevantActiveRules' for 'cat1', it now returns
// 'false'.  The rule, 'myRule', still applies to 'cat1' (i.e., it is still
// "relevant" to 'cat1'), but the predicates defined by 'myRule' are no longer
// satisfied by the current thread, i.e., the current thread's attribute
// context does not contain an attribute matching '("uuid", 3938908)'.
//..
    ASSERT(!context->hasRelevantActiveRules(cat1));
//..
// Next, we call 'determineThresholdLevels' on 'cat1' and find that it
// returns the threshold levels we defined for 'cat1' when we created it:
//..
    context->determineThresholdLevels(&thresholdLevels, cat1);
    ASSERT(thresholdLevels == cat1ThresholdLevels);
//..
// Finally, we add an attribute to the current thread's attribute context (as
// we did in the first example, "Managing Attributes").  Note that we keep an
// iterator referring to the added attributes so that we can remove them before
// 'attributes' goes out of scope and is destroyed.  Also note that the class
// 'AttributeSet' is defined in the component documentation for
// 'ball_attributecontainer'.
//..
    AttributeSet attributes;
    attributes.insert(ball::Attribute("uuid", 3938908));
    ball::AttributeContext::iterator it = context->addAttributes(&attributes);
//..
// The following call to 'hasRelevantActiveRules' will return 'true' for 'cat1'
// because there is at least one rule, 'myRule', that is both "relevant"
// (i.e., its pattern matches the category name of 'cat1') and "active" (i.e.,
// all of the predicates defined for 'myRule' are satisfied by the attributes
// held by this thread's attribute context):
//..
    ASSERT(context->hasRelevantActiveRules(cat1));
//..
// Now, when we call 'determineThresholdLevels', it will again return the
// maximum threshold level from 'cat1' and 'myRule':
//..
    context->determineThresholdLevels(&thresholdLevels, cat1);
    ASSERT(128 == thresholdLevels.recordLevel());
    ASSERT(110 == thresholdLevels.passLevel());
    ASSERT( 70 == thresholdLevels.triggerLevel());
    ASSERT( 40 == thresholdLevels.triggerAllLevel());
//..
// We must be careful to remove 'attributes' from the attribute context before
// it goes out of scope and is destroyed.  Note that the 'ball' package
// provides a component, 'ball_scopedattributes', for adding, and automatically
// removing, attributes from the current thread's attribute context.
//..
    context->removeAttributes(it);
//..

    return 0;
}

}  // close namespace BALL_ATTRIBUTECONTEXT_USAGE_EXAMPLE_2

namespace BALL_ATTRIBUTECONTEXT_USAGE_EXAMPLE_1 {

bslmt::Barrier barrier(2);  // synchronize threads

struct ThreadArgs {
    CatMngr *d_categoryManager_p;
};

///Example 1: Managing Attributes
///- - - - - - - - - - - - - - -
// First we will define a thread function that will create and install two
// attributes.  Note that we will use the 'AttributeSet' implementation of the
// 'ball::AttributeContainer' protocol defined in the component documentation
// for 'ball_attributecontainer'; the 'ball' package provides a similar class
// in the 'ball_defaultattributecontainer' component.
//..
    extern "C" void *workerThread1(void *)
    {
//..
// Inside this thread function, we create an attribute set to hold our
// attribute values, then we create two 'ball::Attribute' objects and add them
// to that set:
//..
        AttributeSet attributes;
        ball::Attribute a1("uuid", 4044457);
        ball::Attribute a2("name", "Gang Chen");
        attributes.insert(a1);
        attributes.insert(a2);

        barrier.wait();  // *** added to Example 1 from header file
//..
// Next, we obtain a reference to the current thread's attribute context using
// the 'getContext' class method (note that in practice we would use a scoped
// guard for this purpose; see {'ball_scopedattributes'}):
//..
        ball::AttributeContext *context = ball::AttributeContext::getContext();
        ASSERT(context);
        ASSERT(context == ball::AttributeContext::lookupContext());
//..
// We can add our attribute container, 'attributes', to the current context
// using the 'addAttributes' method.  We store the returned iterator so that
// we can remove 'attributes' before it goes out of scope and is destroyed:
//..
        ball::AttributeContext::iterator it =
                                           context->addAttributes(&attributes);
        ASSERT(context->hasAttribute(a1));
        ASSERT(context->hasAttribute(a2));
//..
// We then call the 'removeAttributes' method to remove the attributes from
// the attribute context:
//..
        barrier.wait();  // *** added to Example 1 from header file

        context->removeAttributes(it);
        ASSERT(false == context->hasAttribute(a1));
        ASSERT(false == context->hasAttribute(a2));
//..
// This completes the first thread function:
//..
        barrier.wait();  // *** added to Example 1 from header file

        return 0;
    }

//..
// The second thread function will simply verify that there is no currently
// available attribute context.  Note that attribute contexts are created and
// managed by individual threads using thread-specific storage, and that
// attribute contexts created by one thread are not visible in any other
// threads:
//..

extern "C" void *workerThread2(void *)
{
    barrier.wait();
    ASSERT(0 == ball::AttributeContext::lookupContext());

    barrier.wait();
    ASSERT(0 == ball::AttributeContext::lookupContext());

    barrier.wait();
    ASSERT(0 == ball::AttributeContext::lookupContext());

    return 0;
}

}  // close namespace BALL_ATTRIBUTECONTEXT_USAGE_EXAMPLE_1

namespace BALL_ATTRIBUTECONTEXT_USAGE_EXAMPLE_OLD {

struct ThreadArgs {
    CatMngr *d_categoryManager_p;
};

extern "C" void *oldUsageExample(void *args)
{
    CatMngr *manager =
                     reinterpret_cast<ThreadArgs *>(args)->d_categoryManager_p;

    ASSERT(manager);
    ASSERT(0 == ball::AttributeContext::lookupContext());

    const ball::Category *cat1 =
                      manager->setThresholdLevels("weekday", 128,  96, 64, 32);
    ASSERT(cat1);

    const ball::Category *cat2 =
                      manager->setThresholdLevels("weekend", 125, 100, 75, 50);
    ASSERT(cat2);

    ball::Rule rule("week*", 120, 110, 70, 40);

    manager->addRule(rule);

    AttributeSet attributes;

    ball::AttributeContext *attrContext = ball::AttributeContext::getContext();
    ball::Attribute attr("uuid", 1);
    attributes.insert(attr);
    Obj::iterator iter = attrContext->addAttributes(&attributes);
    (void)iter;

    ASSERT(attrContext->hasRelevantActiveRules(cat1));
    ASSERT(attrContext->hasRelevantActiveRules(cat2));

    ball::ThresholdAggregate levels(0, 0, 0, 0);

    attrContext->determineThresholdLevels(&levels, cat1);
    ASSERT(128 == levels.recordLevel());
    ASSERT(110 == levels.passLevel());
    ASSERT( 70 == levels.triggerLevel());
    ASSERT( 40 == levels.triggerAllLevel());

    attrContext->determineThresholdLevels(&levels, cat2);
    ASSERT(125 == levels.recordLevel());
    ASSERT(110 == levels.passLevel());
    ASSERT( 75 == levels.triggerLevel());
    ASSERT( 50 == levels.triggerAllLevel());

    manager->removeRule(rule);

    ASSERT(!attrContext->hasRelevantActiveRules(cat1));
    ASSERT(!attrContext->hasRelevantActiveRules(cat2));

    attrContext->determineThresholdLevels(&levels, cat1);
    ASSERT(128 == levels.recordLevel());
    ASSERT( 96 == levels.passLevel());
    ASSERT( 64 == levels.triggerLevel());
    ASSERT( 32 == levels.triggerAllLevel());

    attrContext->determineThresholdLevels(&levels, cat2);
    ASSERT(125 == levels.recordLevel());
    ASSERT(100 == levels.passLevel());
    ASSERT( 75 == levels.triggerLevel());
    ASSERT( 50 == levels.triggerAllLevel());

    ball::Predicate pred1("uuid", 1);
    rule.addPredicate(pred1);
    manager->addRule(rule);

    ASSERT(attrContext->hasRelevantActiveRules(cat1));
    ASSERT(attrContext->hasRelevantActiveRules(cat2));

    attrContext->determineThresholdLevels(&levels, cat1);
    ASSERT(128 == levels.recordLevel());
    ASSERT(110 == levels.passLevel());
    ASSERT( 70 == levels.triggerLevel());
    ASSERT( 40 == levels.triggerAllLevel());

    attrContext->determineThresholdLevels(&levels, cat2);
    ASSERT(125 == levels.recordLevel());
    ASSERT(110 == levels.passLevel());
    ASSERT( 75 == levels.triggerLevel());
    ASSERT( 50 == levels.triggerAllLevel());

    manager->removeRule(rule);
    rule.setPattern("weekend");
    manager->addRule(rule);

    ASSERT(!attrContext->hasRelevantActiveRules(cat1));
    ASSERT( attrContext->hasRelevantActiveRules(cat2));

    attrContext->determineThresholdLevels(&levels, cat1);
    ASSERT(128 == levels.recordLevel());
    ASSERT( 96 == levels.passLevel());
    ASSERT( 64 == levels.triggerLevel());
    ASSERT( 32 == levels.triggerAllLevel());

    attrContext->determineThresholdLevels(&levels, cat2);
    ASSERT(125 == levels.recordLevel());
    ASSERT(110 == levels.passLevel());
    ASSERT( 75 == levels.triggerLevel());
    ASSERT( 50 == levels.triggerAllLevel());

    manager->removeRule(rule);
    rule.removePredicate(pred1);
    ball::Predicate pred2("uuid", 2);
    rule.addPredicate(pred2);
    manager->addRule(rule);

    ASSERT(!attrContext->hasRelevantActiveRules(cat1));
    ASSERT(!attrContext->hasRelevantActiveRules(cat2));

    attrContext->determineThresholdLevels(&levels, cat1);
    ASSERT(128 == levels.recordLevel());
    ASSERT( 96 == levels.passLevel());
    ASSERT( 64 == levels.triggerLevel());
    ASSERT( 32 == levels.triggerAllLevel());

    attrContext->determineThresholdLevels(&levels, cat2);
    ASSERT(125 == levels.recordLevel());
    ASSERT(100 == levels.passLevel());
    ASSERT( 75 == levels.triggerLevel());
    ASSERT( 50 == levels.triggerAllLevel());

    return 0;
}

}  // close namespace BALL_ATTRIBUTECONTEXT_USAGE_EXAMPLE_OLD

//=============================================================================
//                         CASE 4 RELATED ENTITIES
//-----------------------------------------------------------------------------

namespace BALL_ATTRIBUTECONTEXT_TEST_CASE_4 {

enum {
    NUM_RULETHREADS    =  2, // number of threads adding/deleting rules
    NUM_CONTEXTTHREADS =  4, // number of threads verifying attribute contexts
    NUM_TESTS          = 10, // number of repetitions
    NUM_THREADS        = NUM_RULETHREADS + NUM_CONTEXTTHREADS
};

bslmt::Barrier barrier(NUM_THREADS);  // synchronize threads

struct ThreadArgs {
    CatMngr      *d_categoryManager_p;
    unsigned int  d_seed;
};

int randomValue(unsigned int *seed)
    // Return a random value based on the specified 'seed'.
{
#ifdef BSLS_PLATFORM_OS_WINDOWS
    // 'rand' is thread-safe on Windows when linked with the multi-threaded CRT
    // (common run-time) library (which we must be doing anyway).

    return rand();
#else
    return rand_r(seed);
#endif
}

extern "C" void *case4RuleThread(void *args)
{
    ThreadArgs *threadArgs = reinterpret_cast<ThreadArgs *>(args);

    CatMngr      *manager = threadArgs->d_categoryManager_p;
    unsigned int  seed    = threadArgs->d_seed;

    barrier.wait();

    if (veryVerbose) {
        MTCOUT << "\n\tPhase 1: Randomly add rules until the rule set is full."
               << MTENDL;
    }

    // Create all rules to be added in a rule set.

    ball::RuleSet ruleSet;

    for (int i = 0; i < ball::RuleSet::e_MAX_NUM_RULES; ++i) {
        bsl::string pattern(i + 1, 'a');
        pattern += '*';
        ball::Rule rule(pattern.c_str(), 0, i + 1, i + 1, i + 1);

        for (int j = 0; j < ball::RuleSet::e_MAX_NUM_RULES - i; ++j) {
            ball::Predicate predicate("uuid", j);
            rule.addPredicate(predicate);
        }
        ruleSet.addRule(rule);
    }
    ASSERT(ball::RuleSet::e_MAX_NUM_RULES == ruleSet.numRules());

    // Add rules to the category manager.

    while (manager->ruleSet().numRules() < ball::RuleSet::e_MAX_NUM_RULES) {
        int r = randomValue(&seed) % ball::RuleSet::e_MAX_NUM_RULES;

        // 2/3 chance to add a rule, 1/3 chance to remove a rule

        if (0 != (randomValue(&seed) % 3)) {
            while (0 == manager->addRule(*ruleSet.getRuleById(r))
                   && manager->ruleSet().numRules() <
                                              ball::RuleSet::e_MAX_NUM_RULES) {
                r = randomValue(&seed) % ball::RuleSet::e_MAX_NUM_RULES;
            }
        }
        else {
            manager->removeRule(*ruleSet.getRuleById(r));
        }
    }
    ASSERT(ball::RuleSet::e_MAX_NUM_RULES == manager->ruleSet().numRules());

    barrier.wait();

    if (veryVerbose) {
        MTCOUT << "\n\tPhase 2: Wait for \"context\" threads." << MTENDL;
    }

    barrier.wait();

    if (veryVerbose) {
        MTCOUT <<
              "\n\tPhase 3: Randomly remove rules until the rule set is empty."
               << MTENDL;
    }

    while (manager->ruleSet().numRules() > 0) {
        int r = randomValue(&seed) % ball::RuleSet::e_MAX_NUM_RULES;

        // 2/3 chance to remove a rule, 1/3 chance to add a rule

        if (0 != (randomValue(&seed) % 3)) {
            while (0 == manager->removeRule(*ruleSet.getRuleById(r))
                   && manager->ruleSet().numRules() > 0) {
                r = randomValue(&seed) % ball::RuleSet::e_MAX_NUM_RULES;
            }
        }
        else {
            manager->addRule(*ruleSet.getRuleById(r));
        }
    }
    ASSERT(0 == manager->ruleSet().numRules());

    if (veryVerbose) {
        MTCOUT << "\n\tPhase 4: Wait for \"context\" threads." << MTENDL;
    }

    barrier.wait();

    return 0;
}

extern "C" void *case4ContextThread(void *args)
{
    CatMngr *manager =
                     reinterpret_cast<ThreadArgs *>(args)->d_categoryManager_p;

    barrier.wait();

    if (veryVerbose) {
        MTCOUT << "\n\tPhase 1: Initializing attribute contexts "
               << "and creating categories."
               << MTENDL;
    }

    // Create an empty attribute context.

    ASSERT(0 == Obj::lookupContext());

    Obj *mX = Obj::getContext();  const Obj& X = *mX;
    ASSERT(mX);
    ASSERT(mX == Obj::lookupContext());

    // Add categories C0, C1, C2, ... C32 whose names are "", "a", "aa", etc.

    const int NUM_CATEGORIES = ball::RuleSet::e_MAX_NUM_RULES + 1;
    bsl::vector<const ball::Category *> CATEGORIES(NUM_CATEGORIES);
    for (int i = 0; i < NUM_CATEGORIES; ++i) {
        bsl::string name(i, 'a');
        CATEGORIES[i] =
                     manager->setThresholdLevels(name.c_str(), i + 1, 0, 0, 0);
        LOOP_ASSERT(i, CATEGORIES[i]);
    }

    // Add attributes uuid = 0, uuid = 1, ..., uuid = 31.

    AttributeSet attributes;

    for (int i = 0; i < NUM_CATEGORIES - 1; ++i) {
        ball::Attribute attribute("uuid", i);
        attributes.insert(attribute);
    }

    Obj::iterator iterator = mX->addAttributes(&attributes);

    // Verify that all attributes exist in the context.
    for (int i = 0; i < NUM_CATEGORIES - 1; ++i) {
        ball::Attribute attribute("uuid", i);
        LOOP_ASSERT(i, true == X.hasAttribute(attribute));
    }

    // Loop until there are 32 active categories (the "rule" threads are
    // concurrently adding rules to the rule set of the category manager).

    int numActiveCategories = 0;
    while (numActiveCategories != NUM_CATEGORIES - 1) {
        numActiveCategories = 0;
        for (int i = 1; i < NUM_CATEGORIES; ++i) {
            numActiveCategories +=
                               X.hasRelevantActiveRules(CATEGORIES[i]) ? 1 : 0;
        }
    }

    barrier.wait();

    if (veryVerbose) {
        MTCOUT << "\n\tPhase 2: Verify 'hasRelevantActiveRules' and "
               << "'determineThresholdLevels'."
               << MTENDL;
    }

    // All categories except the first one must be active now.

    ASSERT(!X.hasRelevantActiveRules(CATEGORIES[0]));
    for (int i = 1; i < NUM_CATEGORIES; ++i) {
        LOOP_ASSERT(i, X.hasRelevantActiveRules(CATEGORIES[i]));

        // Forcibly clear the rule evaluation cache every few iterations.

        if (0 == i % 5) {
            mX->clearCache();
        }
    }

    // Category C0 has no relevant rules, so 'determineThresholdLevels' should
    // return its own threshold levels.

    ball::ThresholdAggregate levels(0, 0, 0, 0);
    X.determineThresholdLevels(&levels, CATEGORIES[0]);
    ASSERT(CATEGORIES[0]->recordLevel()     == levels.recordLevel());
    ASSERT(CATEGORIES[0]->passLevel()       == levels.passLevel());
    ASSERT(CATEGORIES[0]->triggerLevel()    == levels.triggerLevel());
    ASSERT(CATEGORIES[0]->triggerAllLevel() == levels.triggerAllLevel());

    // For other categories, the recordLevel is obtained from the category
    // itself, while all other levels should come from the corresponding
    // "largest" relevant rule.

    for (int i = 1; i < NUM_CATEGORIES; ++i) {
        X.determineThresholdLevels(&levels, CATEGORIES[i]);
        LOOP_ASSERT(i, CATEGORIES[i]->recordLevel() == levels.recordLevel());
        LOOP_ASSERT(i, i == levels.passLevel());
        LOOP_ASSERT(i, i == levels.triggerLevel());
        LOOP_ASSERT(i, i == levels.triggerAllLevel());

        // Forcibly clear the rule evaluation cache every few iterations.

        if (0 == i % 3) {
            mX->clearCache();
        }
    }

    // Now remove attributes one by one.  Verify that every time an attribute
    // is removed the corresponding category is affected.

    for (int i = 0; i < NUM_CATEGORIES - 1; ++i) {
        mX->removeAttributes(iterator);
        ball::Attribute attribute("uuid", NUM_CATEGORIES - 2 - i);
        attributes.remove(attribute);
        iterator = mX->addAttributes(&attributes);

        if (veryVeryVerbose) {
            MTCOUT << "\n\t\tRemoving attribute uuid = "
                   << NUM_CATEGORIES - 2 - i
                   << MTENDL;
        }

        for (int j = 0; j < NUM_CATEGORIES; ++j) {
            LOOP2_ASSERT(i, j, (j - 1 > i) ==
                                      X.hasRelevantActiveRules(CATEGORIES[j]));

            X.determineThresholdLevels(&levels, CATEGORIES[j]);

            if (veryVeryVerbose) {
                MTCOUT << "\n\t\tCategory " << j
                       << " -- hasRelevantActiveRules: "
                       << X.hasRelevantActiveRules(CATEGORIES[j])
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
        ball::Attribute attribute("uuid", i);
        LOOP_ASSERT(i, false == X.hasAttribute(attribute));
    }

    // Reinsert all of the attributes.

    mX->removeAttributes(iterator);
    for (int i = 0; i < NUM_CATEGORIES - 1; ++i) {
        attributes.insert(ball::Attribute("uuid", i));
    }
    iterator = mX->addAttributes(&attributes);

    // All categories except the first one must be active now.

    ASSERT(!X.hasRelevantActiveRules(CATEGORIES[0]));
    for (int i = 1; i < NUM_CATEGORIES; ++i) {
        LOOP_ASSERT(i, X.hasRelevantActiveRules(CATEGORIES[i]));
    }

    // Category C0 has no relevant rules, so 'determineThresholdLevels' should
    // return its own threshold levels.

    X.determineThresholdLevels(&levels, CATEGORIES[0]);
    ASSERT(CATEGORIES[0]->recordLevel()     == levels.recordLevel());
    ASSERT(CATEGORIES[0]->passLevel()       == levels.passLevel());
    ASSERT(CATEGORIES[0]->triggerLevel()    == levels.triggerLevel());
    ASSERT(CATEGORIES[0]->triggerAllLevel() == levels.triggerAllLevel());

    // For other categories, the 'recordLevel' is obtained from the category
    // itself, while all other levels should come from the corresponding
    // "largest" relevant rule.

    for (int i = 1; i < NUM_CATEGORIES; ++i) {
        X.determineThresholdLevels(&levels, CATEGORIES[i]);
        LOOP_ASSERT(i, CATEGORIES[i]->recordLevel() == levels.recordLevel());
        LOOP_ASSERT(i, i == levels.passLevel());
        LOOP_ASSERT(i, i == levels.triggerLevel());
        LOOP_ASSERT(i, i == levels.triggerAllLevel());
    }

    barrier.wait();

    if (veryVerbose) {
        MTCOUT << "\n\tPhase 3: Loop until all rules are removed." << MTENDL;
    }

    numActiveCategories = NUM_CATEGORIES - 1;
    while (numActiveCategories > 0) {
        numActiveCategories = 0;
        for (int i = 1; i < NUM_CATEGORIES; ++i) {
            numActiveCategories +=
                               X.hasRelevantActiveRules(CATEGORIES[i]) ? 1 : 0;
        }
    }

    barrier.wait();

    if (veryVerbose) {
         MTCOUT << "\n\tPhase 4: Checking categories again." << MTENDL;
    }

    // There are no longer any active categories.

    for (int i = 0; i < NUM_CATEGORIES; ++i) {
        LOOP_ASSERT(i, !X.hasRelevantActiveRules(CATEGORIES[i]));
        X.determineThresholdLevels(&levels, CATEGORIES[i]);
        LOOP_ASSERT(i, CATEGORIES[i]->recordLevel() == levels.recordLevel());
        LOOP_ASSERT(i, CATEGORIES[i]->passLevel()   == levels.passLevel());
        LOOP_ASSERT(i, CATEGORIES[i]->triggerLevel()== levels.triggerLevel());
        LOOP_ASSERT(i, CATEGORIES[i]->triggerAllLevel()
                                                  == levels.triggerAllLevel());
    }

    return 0;
}

}  // close namespace BALL_ATTRIBUTECONTEXT_TEST_CASE_4

//=============================================================================
//                         CASE 3 RELATED ENTITIES
//-----------------------------------------------------------------------------

namespace BALL_ATTRIBUTECONTEXT_TEST_CASE_3 {

const int NUM_THREADS =  6;  // number of threads
const int NUM_TESTS   = 10;  // number of tests

bslma::Allocator *allocator = &bslma::NewDeleteAllocator::singleton();

ball::Attribute A0("",     "12345678",      allocator);
ball::Attribute A1("",     12345678,        allocator);
ball::Attribute A2("",     (Int64)12345678, allocator);

ball::Attribute A3("uuid", "12345678",      allocator);
ball::Attribute A4("uuid", 12345678,        allocator);
ball::Attribute A5("uuid", (Int64)12345678, allocator);

ball::Attribute A6("UUID", "12345678",      allocator);
ball::Attribute A7("UUID", 12345678,        allocator);
ball::Attribute A8("UUID", (Int64)12345678, allocator);

const ball::Attribute ATTRS[] = { A0, A1, A2, A3, A4, A5, A6, A7, A8 };

const int NUM_ATTRS = sizeof ATTRS / sizeof *ATTRS;

extern "C" void *case3ContextThread(void *)
{
    ASSERT(0 == Obj::lookupContext());

    Obj *mX = Obj::getContext();  const Obj& X = *mX;
    ASSERT(mX);
    ASSERT(mX == Obj::lookupContext());

    // Test with no attributes.
    {
        ASSERT(0 == X.containers().numContainers());

        for (int i = 0; i < NUM_ATTRS; ++i) {
            ASSERTV(i, false == X.hasAttribute(ATTRS[i]));
        }

        AttributeSet attributes;
        Obj::iterator it = mX->addAttributes(&attributes);
        ASSERT(1 == X.containers().numContainers());

        for (int i = 0; i < NUM_ATTRS; ++i) {
            ASSERTV(i, false == X.hasAttribute(ATTRS[i]));
        }

        {
            AttributeSet attributes;
            Obj::iterator it = mX->addAttributes(&attributes);
            ASSERT(2 == X.containers().numContainers());

            mX->removeAttributes(it);
            ASSERT(1 == X.containers().numContainers());
        }

        mX->removeAttributes(it);
        ASSERT(0 == X.containers().numContainers());
    }

    // Test with all attributes in a single container.
    {
        AttributeSet attributes;
        Obj::iterator it = mX->addAttributes(&attributes);
        ASSERT(1 == X.containers().numContainers());

        for (int i = 0; i < NUM_ATTRS; ++i) {
            mX->removeAttributes(it);
            ASSERT(0 == X.containers().numContainers());

            attributes.insert(ATTRS[i]);

            it = mX->addAttributes(&attributes);
            ASSERT(1 == X.containers().numContainers());

            for (int j = 0; j < NUM_ATTRS; ++j) {
                bool hasAttr = j <= i;

                ASSERTV(i, j, hasAttr == X.hasAttribute(ATTRS[j]));
                ASSERTV(i, j, hasAttr == X.containers().hasValue(ATTRS[j]));
            }
        }

        for (int i = 0; i < NUM_ATTRS; ++i) {
            mX->removeAttributes(it);
            ASSERT(0 == X.containers().numContainers());

            attributes.remove(ATTRS[i]);

            it = mX->addAttributes(&attributes);
            ASSERT(1 == X.containers().numContainers());

            for (int j = 0; j < NUM_ATTRS; ++j) {
                bool hasAttr = !(j <= i);

                ASSERTV(i, j, hasAttr == X.hasAttribute(ATTRS[j]));
                ASSERTV(i, j, hasAttr == X.containers().hasValue(ATTRS[j]));
            }
        }

        mX->removeAttributes(it);
        ASSERT(0 == X.containers().numContainers());
    }

    // Test with attributes distributed over three containers.
    {
        AttributeSet attributes0;
        Obj::iterator it0 = mX->addAttributes(&attributes0);
        ASSERT(1 == X.containers().numContainers());

        AttributeSet attributes1;
        Obj::iterator it1 = mX->addAttributes(&attributes1);
        ASSERT(2 == X.containers().numContainers());

        AttributeSet attributes2;
        Obj::iterator it2 = mX->addAttributes(&attributes2);
        ASSERT(3 == X.containers().numContainers());

        for (int i = 0; i < NUM_ATTRS; ++i) {
            AttributeSet  *as;
            Obj::iterator  it;

            if (0 == (i % 3)) {
                as = &attributes0;
                it = it0;
            }
            else if (1 == (i % 3)) {
                as = &attributes1;
                it = it1;
            }
            else {  // 2 == (i % 3)
                as = &attributes2;
                it = it2;
            }

            mX->removeAttributes(it);
            ASSERT(2 == X.containers().numContainers());

            as->insert(ATTRS[i]);

            mX->addAttributes(as);
            ASSERT(3 == X.containers().numContainers());

            for (int j = 0; j < NUM_ATTRS; ++j) {
                bool hasAttr = j <= i;

                ASSERTV(i, j, hasAttr == X.hasAttribute(ATTRS[j]));
                ASSERTV(i, j, hasAttr == X.containers().hasValue(ATTRS[j]));
            }
        }

        for (int i = 0; i < NUM_ATTRS; ++i) {
            AttributeSet  *as;
            Obj::iterator  it;

            if (0 == (i % 3)) {
                as = &attributes0;
                it = it0;
            }
            else if (1 == (i % 3)) {
                as = &attributes1;
                it = it1;
            }
            else {  // 2 == (i % 3)
                as = &attributes2;
                it = it2;
            }

            mX->removeAttributes(it);
            ASSERT(2 == X.containers().numContainers());

            as->remove(ATTRS[i]);

            mX->addAttributes(as);
            ASSERT(3 == X.containers().numContainers());

            for (int j = 0; j < NUM_ATTRS; ++j) {
                bool hasAttr = !(j <= i);

                ASSERTV(i, j, hasAttr == X.hasAttribute(ATTRS[j]));
                ASSERTV(i, j, hasAttr == X.containers().hasValue(ATTRS[j]));
            }
        }

        mX->removeAttributes(it1);
        ASSERT(2 == X.containers().numContainers());

        mX->removeAttributes(it0);
        ASSERT(1 == X.containers().numContainers());

        mX->removeAttributes(it2);
        ASSERT(0 == X.containers().numContainers());
    }

    return 0;
}

}  // close namespace BALL_ATTRIBUTECONTEXT_TEST_CASE_3

//=============================================================================
//                         CASE 2 RELATED ENTITIES
//-----------------------------------------------------------------------------

namespace BALL_ATTRIBUTECONTEXT_TEST_CASE_2 {

const int NUM_THREADS =  6;  // number of threads
const int NUM_TESTS   = 10;  // number of tests

bslmt::Barrier barrier(NUM_THREADS);  // synchronize threads

extern "C" void *case2ContextThread(void *)
{
    barrier.wait();

    ASSERT(0 == Obj::lookupContext());

    // Create an empty attribute context.

    Obj *context = Obj::getContext();
    ASSERT(context);
    ASSERT(context == Obj::lookupContext());

    // Verify that the same context is observed in different scopes.

    ASSERT(context == Obj::getContext());
    {
        ASSERT(context == Obj::lookupContext());
        ASSERT(context == Obj::getContext());
    }
    ASSERT(context == Obj::lookupContext());

    // Force an allocation from the memory allocator held by the context.
    {
        AttributeSet dummy;  // no allocation (from any allocator)

        Obj::iterator it = context->addAttributes(&dummy);  // allocates from
                                                            // held allocator
        context->removeAttributes(it);
    }

    barrier.wait();

    return (void *)context;
}

}  // close namespace BALL_ATTRIBUTECONTEXT_TEST_CASE_2

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;

    verbose = argc > 2;
    veryVerbose = argc > 3;
    veryVeryVerbose = argc > 4;
    veryVeryVeryVerbose = argc > 5;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;;

    // CONCERN: 'BSLS_REVIEW' failures should lead to test failures.
    bsls::ReviewFailureHandlerGuard reviewGuard(&bsls::Review::failByAbort);

    bslma::TestAllocator  testAllocator("test", veryVeryVeryVerbose);
    bslma::TestAllocator *Z = &testAllocator;

    bslma::TestAllocator defaultAllocator("default", veryVeryVeryVerbose);
    bslma::DefaultAllocatorGuard guard(&defaultAllocator);

    bslma::TestAllocator globalAllocator("global", veryVeryVeryVerbose);
    bslma::Default::setGlobalAllocator(&globalAllocator);

    switch (test) { case 0:  // Zero is always the leading case.
      case 9: {
        // --------------------------------------------------------------------
        // TESTING USAGE EXAMPLE 2
        //   Extracted from component header file.
        //
        // Concerns:
        //: 1 The *second* usage example provided in the component header file
        //:   compiles, links, and runs as shown.
        //
        // Plan:
        //: 1 Incorporate usage example from header into test driver, remove
        //:   leading comment characters, and replace 'assert' with 'ASSERT'.
        //:   (C-1)
        //
        // Testing:
        //   USAGE EXAMPLE 2
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "TESTING USAGE EXAMPLE 2" << endl
                                  << "=======================" << endl;

        using namespace BALL_ATTRIBUTECONTEXT_USAGE_EXAMPLE_2;

///Example 2: Calling 'hasRelevantActiveRules' and 'determineThresholdLevels'
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// In this example we demonstrate how to call the 'hasRelevantActiveRules' and
// 'determineThresholdLevels' methods.  These methods are used (primarily by
// other components in the 'ball' package) to determine the effect of the
// current logging rules on the logging thresholds of a category.  Note that a
// rule is "relevant" if the rule's pattern matches the category's name, and a
// rule is "active" if 'ball::Rule::evaluate' returns 'true' for the
// collection of attributes maintained for the current thread by the thread's
// 'ball::AttributeContext' object.
//
// We start by creating a 'ball::CategoryManager' and use it to initialize the
// static data members of 'ball::AttributeContext'.  Note that, in practice,
// this initialization *should* *not* be performed by clients of the 'ball'
// package: 'ball::AttributeContext::initialize' is called *internally* as part
// of the initialization of the 'ball::LoggerManager' singleton.
//..
    ball::CategoryManager categoryManager;
    ball::AttributeContext::initialize(&categoryManager);
//..

        ThreadArgs args = { &categoryManager };

        bslmt::ThreadUtil::Handle mainThread;
        bslmt::ThreadUtil::create(&mainThread, usageExample2, &args);
        bslmt::ThreadUtil::join(mainThread);

      } break;
      case 8: {
        // --------------------------------------------------------------------
        // TESTING USAGE EXAMPLE 1
        //   Extracted from component header file.
        //
        // Concerns:
        //: 1 The *first* usage example provided in the component header file
        //:   compiles, links, and runs as shown.
        //
        // Plan:
        //: 1 Incorporate usage example from header into test driver, remove
        //:   leading comment characters, and replace 'assert' with 'ASSERT'.
        //:   (C-1)
        //
        // Testing:
        //   USAGE EXAMPLE 1
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "TESTING USAGE EXAMPLE 1" << endl
                                  << "=======================" << endl;

        using namespace BALL_ATTRIBUTECONTEXT_USAGE_EXAMPLE_1;

        ball::CategoryManager manager;
        ball::AttributeContext::initialize(&manager);

//      ThreadArgs args = { &manager };

        bslmt::ThreadUtil::Handle threads[2];
        bslmt::ThreadUtil::create(&threads[0], workerThread1, (void *)0);
        bslmt::ThreadUtil::create(&threads[1], workerThread2, (void *)0);
        bslmt::ThreadUtil::join(threads[0]);
        bslmt::ThreadUtil::join(threads[1]);

      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING ORIGINAL USAGE EXAMPLE
        //   This test runs the original usage example for this component.  It
        //   remains in the test driver for completeness.
        //
        // Concerns:
        //: 1 That the original usage example continues to compile and behave
        //:   as expected.
        //
        // Plan:
        //: 1 Retain the original usage example in perpetuity.  (C-1)
        //
        // Testing:
        //   (OLD) USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "TESTING ORIGINAL USAGE EXAMPLE" << endl
                                  << "==============================" << endl;

        using namespace BALL_ATTRIBUTECONTEXT_USAGE_EXAMPLE_OLD;

        ball::CategoryManager manager;
        ball::AttributeContext::initialize(&manager);

        ThreadArgs args = { &manager };

        bslmt::ThreadUtil::Handle mainThread;
        bslmt::ThreadUtil::create(&mainThread, oldUsageExample, &args);
        bslmt::ThreadUtil::join(mainThread);

      } break;
      case 6: {
        // --------------------------------------------------------------------
        // NO FALSE POSITIVES FROM 'hasRelevantActiveRules'
        //
        // Concerns:
        //: 1 That 'hasRelevantActiveRules' does not produce any false
        //:   positives.  The basis for this concern is that the initial scheme
        //:   for initializing the rule set sequence number in the category
        //:   manager does open up 'hasRelevantActiveRules' for producing false
        //:   positives now that the logger manager singleton, and necessarily
        //:   the global state of 'AttributeContext', can be reinitialized.
        //:   This is because the rule evaluation cache uses that sequence
        //:   number to determine if the cache needs to be refreshed.
        //:
        //: 2 That 'determineThresholdLevels' returns the correct result after
        //:   the global state of 'AttributeContext' has been reinitialized.
        //:   The basis for this concern is that the logic used by
        //:   'determineThresholdLevels' to determine if the cache needs to be
        //:   refreshed is the same as that used by 'hasRelevantActiveRules'.
        //
        // Plan:
        //: 1 Using ad hoc testing: (C-1..2)
        //:
        //:   1 Create a category manager having one category and one rule that
        //:     is relevant to that category.  The rule has one predicate.
        //:
        //:   2 Initialize the 'AttributeContext' global state with the
        //:     category manager created in P-1.1.
        //:
        //:   3 Add an attribute to the main thread's attribute context that
        //:     matches the predicate from P-1.1.  Leave the context unchanged
        //:     for the remainder of the test.
        //:
        //:   4 Verify that both 'hasRelevantActiveRules' and
        //:     'determineThresholdLevels' produce the expected result, i.e.,
        //:     the rule *is* relevant and it *should* influence the category's
        //:     threshold levels.
        //:
        //:   5 Reset the 'AttributeContext' global state (simulating what
        //:     would happen when the logger manager singleton is shut down).
        //:
        //:   6 Repeat P-1.1, but this time the predicate added to the rule
        //:     should not match the attribute that is (still) in the main
        //:     thread's attribute context.
        //:
        //:   7 Again verify that both 'hasRelevantActiveRules' and
        //:     'determineThresholdLevels' produce the expected result, i.e.,
        //:     the rule is *not* relevant and it should *not* influence the
        //:     category's threshold levels.  (C-1..2)
        //
        // Note that assertions that would fail if the old scheme for
        // initializing the rule set sequence number in the category manager
        // was still in place are marked by "!*!*!*" comments.
        //
        // Testing:
        //   CONCERN: No false positives from 'hasRelevantActiveRules'.
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "NO FALSE POSITIVES FROM 'hasRelevantActiveRules'"
                          << endl
                          << "================================================"
                          << endl;

        {
            ball::ThresholdAggregate catLevels( 128,  96, 64, 32);
            ball::ThresholdAggregate ruleLevels(130, 110, 70, 40);
            ball::ThresholdAggregate levels(      0,   0,  0,  0);

            // [1] Initialize 'AttributeContext' with first category manager.
            // Load category manager with:
            //   o category "ABC-Category", and
            //   o matching rule with predicate ("uuid", 2468).
            // Load context with attribute ("uuid", 2468).
            // 'hasRelevantActiveRules' should return 'true'.

            bsls::ObjectBuffer<CatMngr> buffer1;
            new (buffer1.address()) CatMngr(&globalAllocator);

            CatMngr *cmPtr1 = buffer1.address();

            Obj::initialize(cmPtr1, &globalAllocator);

            Obj *mX = Obj::getContext();  const Obj& X = *mX;

            const ball::Category *cat =
                          cmPtr1->addCategory("ABC-Category", 128, 96, 64, 32);
            ASSERT(cat);
            ASSERT(!X.hasRelevantActiveRules(cat));

            {
                ball::Rule rule("ABC-*", 130, 110, 70, 40);
                ball::Predicate pred("uuid", 2468);
                rule.addPredicate(pred);
                cmPtr1->addRule(rule);
            }

            ASSERT(!X.hasRelevantActiveRules(cat));

            AttributeSet attrSet;
            attrSet.insert(ball::Attribute("uuid", 2468));
            ball::AttributeContext::iterator it = mX->addAttributes(&attrSet);

            ASSERT( X.hasRelevantActiveRules(cat));

            X.determineThresholdLevels(&levels, cat);
            ASSERT(levels == ruleLevels);

            // The context must not be modified from here on until it is
            // destroyed at the end of the test case.

            buffer1.object().~CatMngr();

            // [2] Reinitialize 'AttributeContext' with new category manager.
            // Load new category manager with:
            //   o category "ABC-Category" (same name), and
            //   o matching rule with predicate ("uuid", 1357).
            // Context attribute ("uuid", 2468) remains in place.
            // 'hasRelevantActiveRules' should return 'false'.

            bsls::ObjectBuffer<CatMngr> buffer2;
            new (buffer2.address()) CatMngr(&globalAllocator);

            CatMngr *cmPtr2 = buffer2.address();

            Obj::reset();
            Obj::initialize(cmPtr2, &globalAllocator);

            cat = cmPtr2->addCategory("ABC-Category", 128, 96, 64, 32);
            ASSERT(cat);

            {
                ball::Rule rule("ABC-*", 130, 110, 70, 40);
                ball::Predicate pred("uuid", 1357);
                rule.addPredicate(pred);
                cmPtr2->addRule(rule);
            }

            ASSERT(!X.hasRelevantActiveRules(cat));  // !*!*!*

            X.determineThresholdLevels(&levels, cat);
            ASSERT(levels == catLevels);             // !*!*!*

            buffer2.object().~CatMngr();

            // [3] Reinitialize 'AttributeContext' again.
            // Load new category manager with:
            //   o category "XYZ-Category" (different name), and
            //   o matching rule with predicate ("uuid", 1357).
            // Context attribute ("uuid", 2468) still remains in place.
            // 'hasRelevantActiveRules' should return 'false'.

            bsls::ObjectBuffer<CatMngr> buffer3;
            new (buffer3.address()) CatMngr(&globalAllocator);

            CatMngr *cmPtr3 = buffer3.address();

            Obj::reset();
            Obj::initialize(cmPtr3, &globalAllocator);

            cat = cmPtr3->addCategory("XYZ-Category", 128, 96, 64, 32);
            ASSERT(cat);

            {
                ball::Rule rule("XYZ-*", 130, 110, 70, 40);
                ball::Predicate pred("uuid", 1357);
                rule.addPredicate(pred);
                cmPtr3->addRule(rule);
            }

            ASSERT(!X.hasRelevantActiveRules(cat));  // !*!*!*

            X.determineThresholdLevels(&levels, cat);
            ASSERT(levels == catLevels);             // !*!*!*

            buffer3.object().~CatMngr();

            // [4] Clean up context.

            mX->removeAttributes(it);

            ball::AttributeContextProctor proctor;  // destroys context
        }

      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING 'AttributeContextProctor'
        //
        // Concerns:
        //: 1 That the 'AttributeContextProctor' destructor destroys the
        //:   current thread's context (if any) and releases all its memory.
        //
        // Plan:
        //: 1 First test that destroying an 'AttributeContextProctor' when
        //:   there is no context has no effect.  Then test destroying proctors
        //:   where (1) the context defaults to using the global allocator, and
        //:   (2) the context uses the allocator established by a call to the
        //:   'initialize' class method.  Verify in each case that the
        //:   context's memory has been released following the destruction of
        //:   the proctor.  (C-1)
        //
        // Testing:
        //   AttributeContextProctor();
        //   ~AttributeContextProctor();
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING 'AttributeContextProctor'\n"
                          << "=================================\n";

        bslma::TestAllocatorMonitor dam(&defaultAllocator);
        bslma::TestAllocatorMonitor gam(&globalAllocator);
        bslma::TestAllocatorMonitor tam(&testAllocator);

        // The following should have no effect.
        {
            ball::AttributeContextProctor proctor;
        }
        ASSERT(dam.isTotalSame());
        ASSERT(gam.isTotalSame());
        ASSERT(tam.isTotalSame());

        if (veryVerbose) cout << "\tTest defaulting to global allocator.\n";

        ASSERT(0 == Obj::lookupContext());
        ASSERT(dam.isTotalSame());
        ASSERT(gam.isTotalSame());

        {
            Obj *context;
            {
                context = Obj::getContext();
                ASSERT(context);
                ASSERT(dam.isTotalSame());
                ASSERT(gam.isInUseUp());

                gam.reset();

                ASSERT(context == Obj::lookupContext());
                ASSERT(dam.isTotalSame());
                ASSERT(gam.isTotalSame());
            }
            ASSERT(context == Obj::lookupContext());
            ASSERT(dam.isTotalSame());
            ASSERT(gam.isTotalSame());

            ball::AttributeContextProctor proctor;

            ASSERT(dam.isTotalSame());
            ASSERT(gam.isTotalSame());
        }
        ASSERT(0 == Obj::lookupContext());
        ASSERT(dam.isTotalSame());
        ASSERT(gam.isInUseDown());
        ASSERT(0 == globalAllocator.numBytesInUse());

        gam.reset();

        if (veryVerbose) cout << "\tTest with specified allocator.\n";

        bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

        CatMngr manager(&scratch);

        {
            Obj::initialize(&manager, Z);
            ASSERT(dam.isTotalSame());
            ASSERT(gam.isTotalSame());
            ASSERT(tam.isTotalSame());

            Obj *context = Obj::getContext();
            ASSERT(context);
            ASSERT(dam.isTotalSame());
            ASSERT(gam.isTotalSame());
            ASSERT(tam.isInUseUp());

            tam.reset();

            ASSERT(context == Obj::lookupContext());
            ASSERT(dam.isTotalSame());
            ASSERT(gam.isTotalSame());
            ASSERT(tam.isTotalSame());

            ball::AttributeContextProctor proctor;

            ASSERT(dam.isTotalSame());
            ASSERT(gam.isTotalSame());
            ASSERT(tam.isTotalSame());
        }
        ASSERT(0 == Obj::lookupContext());
        ASSERT(dam.isTotalSame());
        ASSERT(gam.isTotalSame());
        ASSERT(tam.isInUseDown());
        ASSERT(0 == testAllocator.numBytesInUse());

      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING 'hasRelevantActiveRules' and 'determineThresholdLevels'
        //
        // Concerns:
        //   'hasRelevantActiveRules' and 'determineThresholdLevels' must
        //   return the correct values even in a multi-threaded environment.
        //
        // Plan:
        //   Assume that the maximum number of rules allowed in a rule set is
        //   32.  Create 32 rules R0, R1, ..., R31 and 33 categories C0, C1,
        //   ..., C31, C32.  Set up the category names and rule patterns such
        //   that Ri is a rule relevant to Cj if i < j.  Also add to Ri all
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
        //   thread, called "rule" threads, creates all the rules, adds them to
        //   the rule set one by one, and then removes all the rules.  In the
        //   meantime, another type of thread, called "context" threads, adds
        //   all the categories to the category manager and verifies that
        //   'hasRelevantActiveRules' and 'determineThresholdLevels' return the
        //   expected values.  All threads are synchronized by a barrier that
        //   divides the running stage into 4 phases.
        //
        // Testing:
        //   void clearCache();
        //   bool hasRelevantActiveRules(const Cat *cat) const;
        //   void determineThresholdLevels(TL *lvls, const Cat *cat) const;
        // --------------------------------------------------------------------

        if (verbose) cout
            << endl
            << "Test 'hasRelevantActiveRules' and 'determineThresholdLevels'\n"
            << "============================================================"
            << endl;

        using namespace BALL_ATTRIBUTECONTEXT_TEST_CASE_4;

        CatMngr manager;
        Obj::initialize(&manager);

        unsigned int seed = 0;

        for (int i = 0; i < NUM_TESTS; ++i) {
            bslmt::ThreadUtil::Handle ruleThreads[NUM_RULETHREADS];
            bslmt::ThreadUtil::Handle contextThreads[NUM_CONTEXTTHREADS];

            for (int j = 0; j < NUM_RULETHREADS; ++j) {
                ThreadArgs ruleThreadArgs[NUM_RULETHREADS];

                ruleThreadArgs[j].d_categoryManager_p = &manager;
                ruleThreadArgs[j].d_seed              = seed;

                bslmt::ThreadUtil::create(
                                 &ruleThreads[j],
                                 case4RuleThread,
                                 reinterpret_cast<void *>(&ruleThreadArgs[j]));
            }

            for (int j = 0; j < NUM_CONTEXTTHREADS; ++j) {
                ThreadArgs contextThreadArgs[NUM_CONTEXTTHREADS];

                contextThreadArgs[j].d_categoryManager_p = &manager;
                contextThreadArgs[j].d_seed              = seed;

                bslmt::ThreadUtil::create(
                              &contextThreads[j],
                              case4ContextThread,
                              reinterpret_cast<void *>(&contextThreadArgs[j]));
            }

            for (int j = 0; j < NUM_RULETHREADS; ++j) {
                bslmt::ThreadUtil::join(ruleThreads[j]);
            }

            for (int j = 0; j < NUM_CONTEXTTHREADS; ++j) {
                bslmt::ThreadUtil::join(contextThreads[j]);
            }
        }

        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bsls::AssertTestHandlerGuard hG;

            const ball::Category *cat =
                     manager.addCategory("negative-testing", 128,  96, 64, 32);
            ASSERT(cat);

            Obj *context = Obj::getContext();
            ASSERT(context);

            ASSERT_PASS(context->hasRelevantActiveRules(cat));
            ASSERT_FAIL(context->hasRelevantActiveRules(0));

            ball::ThresholdAggregate levels(0, 0, 0, 0);

            ASSERT_PASS(context->determineThresholdLevels(&levels, cat));
            ASSERT_FAIL(context->determineThresholdLevels(&levels,   0));
            ASSERT_FAIL(context->determineThresholdLevels(      0, cat));

            ball::AttributeContextProctor proctor;  // destroys context
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
        //   iterator addAttributes(const AttributeContainer *attributes);
        //   void removeAttributes(iterator element);
        //   bool hasAttribute(const Attribute& value) const;
        //   const AttributeContainerList& containers() const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Test Attribute Manipulation\n"
                          << "===========================\n";

        using namespace BALL_ATTRIBUTECONTEXT_TEST_CASE_3;

        for (int i = 0; i < NUM_TESTS; ++i) {
            bslmt::ThreadUtil::Handle threads[NUM_THREADS];

            for (int j = 0; j < NUM_THREADS; ++j) {
                bslmt::ThreadUtil::create(&threads[j],
                                          case3ContextThread,
                                          (void *)0);
            }

            for (int j = 0; j < NUM_THREADS; ++j) {
                bslmt::ThreadUtil::join(threads[j], 0);
            }
        }

        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bsls::AssertTestHandlerGuard hG;

            AttributeSet as;

            Obj *context = Obj::getContext();
            ASSERT(context);

            ASSERT_SAFE_PASS(context->addAttributes(&as));
            ASSERT_SAFE_FAIL(context->addAttributes(0));

            ball::AttributeContextProctor proctor;  // destroys context
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
        //   which will create its own 'ball::AttributeContext' object.  Verify
        //   that within each thread, only one 'ball::AttributeContext' is
        //   created (by the first call to 'getContext') and that
        //   'lookupContext' (and subsequent calls to 'getContext') correctly
        //   returns the context even in different scopes.  Also verify that
        //   the 'ball::AttributeContext' objects created by different threads
        //   are distinct and that they draw dynamic memory from the expected
        //   allocator.
        //
        // Testing:
        //   static void initialize(CategoryManager *cm, Allocator *gA = 0);
        //   static void reset();
        //   static AttributeContext *getContext();
        //   static AttributeContext *lookupContext();
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Test Attribute Context Creation\n"
                          << "===============================\n";

        using namespace BALL_ATTRIBUTECONTEXT_TEST_CASE_2;

        CatMngr dummy;

        if (veryVerbose)
            cout << "\tTest with various allocator configurations.\n";

        for (char cfg = 'a'; cfg <= 'd'; ++cfg) {

            const char CONFIG = cfg;  // how we specify the allocator

            if (veryVerbose) { T_ T_ P(CONFIG) }

            bslma::TestAllocator *objAllocatorPtr;

            switch (CONFIG) {
              case 'a': {
                objAllocatorPtr = &globalAllocator;
              } break;
              case 'b': {
                Obj::initialize(&dummy);
                objAllocatorPtr = &globalAllocator;
              } break;
              case 'c': {
                Obj::initialize(&dummy, 0);
                objAllocatorPtr = &globalAllocator;
              } break;
              case 'd': {
                Obj::initialize(&dummy, Z);
                objAllocatorPtr = Z;
              } break;
              default: {
                ASSERTV(CONFIG, !"Bad allocator config.");
              } break;
            }

            bslma::TestAllocatorMonitor dam(&defaultAllocator);
            bslma::TestAllocatorMonitor gam(&globalAllocator);

            for (int i = 0; i < NUM_TESTS; ++i) {
                bslmt::ThreadUtil::Handle threads[NUM_THREADS];

                bslma::TestAllocatorMonitor monitor(objAllocatorPtr);

                for (int j = 0; j < NUM_THREADS; ++j) {
                    bslmt::ThreadUtil::create(&threads[j],
                                              case2ContextThread,
                                              (void *)0);
                }

                void *contexts[NUM_THREADS];

                for (int j = 0; j < NUM_THREADS; ++j) {
                    bslmt::ThreadUtil::join(threads[j], &contexts[j]);
                }

                ASSERT(monitor.isTotalUp());
                ASSERT(monitor.isInUseSame());

                for (int j = 0; j < NUM_THREADS; ++j) {
                    for (int k = 0; k < NUM_THREADS; ++k) {
                        bool same = j == k;
                        ASSERTV(j, k, same == (contexts[j] == contexts[k]));
                    }
                }
            }
            ASSERT(dam.isTotalSame());

            if (objAllocatorPtr == Z) {
                ASSERT(gam.isTotalSame());
            }

            Obj::reset();
        }

        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bsls::AssertTestHandlerGuard hG;

            CatMngr dummy;

            ASSERT_OPT_PASS(Obj::initialize(&dummy));
            Obj::reset();
            ASSERT_OPT_FAIL(Obj::initialize(0));
            Obj::reset();
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
        //   AttributeSet
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Test Helper Classes\n"
                          << "===================\n";

        const char *VALUES[]   = { "A", "B", "C", "D", "E" };
        const int   NUM_VALUES = sizeof VALUES / sizeof *VALUES;

        if (veryVerbose)
            cout << "\tPrimary methods test (w/test allocator).\n";
        {
            AttributeSet mX(Z);  const AttributeSet& X = mX;

            for (int i = 0; i < NUM_VALUES; ++i) {
                mX.insert(ball::Attribute("uuid", i));
                mX.insert(ball::Attribute("uuid", (Int64)i));
                mX.insert(ball::Attribute("name", VALUES[i]));

                if (veryVerbose) { P(X); }

                for (int j = 0; j < NUM_VALUES; ++j) {
                    bool hasAttr = j <= i;

                    ASSERT(hasAttr == X.hasValue(ball::Attribute("uuid", j)));
                    ASSERT(hasAttr == X.hasValue(ball::Attribute("uuid",
                                                                 (Int64)j)));
                    ASSERT(hasAttr == X.hasValue(ball::Attribute("name",
                                                                 VALUES[j])));
                }
            }
            ASSERT(0 == defaultAllocator.numBytesInUse());
            ASSERT(0 <  testAllocator.numBytesInUse());

            for (int i = 0; i < NUM_VALUES; ++i) {
                mX.remove(ball::Attribute("uuid", i));
                mX.remove(ball::Attribute("uuid", (Int64)i));
                mX.remove(ball::Attribute("name", VALUES[i]));

                if (veryVerbose) { P(X); }

                for (int j = 0; j < NUM_VALUES; ++j) {
                    bool hasAttr = j > i;

                    ASSERT(hasAttr == X.hasValue(ball::Attribute("uuid", j)));
                    ASSERT(hasAttr == X.hasValue(ball::Attribute("uuid",
                                                                 (Int64)j)));
                    ASSERT(hasAttr == X.hasValue(ball::Attribute("name",
                                                                 VALUES[j])));
                }
            }
            ASSERT(0 == defaultAllocator.numBytesInUse());
        }
        ASSERT(0 == testAllocator.numBytesInUse());

        if (veryVerbose) cout << "\tTest use of default allocator.\n";
        {
            AttributeSet mY;  const AttributeSet& Y = mY;
            mY.insert(ball::Attribute("uuid", 1));
            mY.insert(ball::Attribute("uuid", "A"));

            ASSERT(Y.hasValue(ball::Attribute("uuid", 1)));
            ASSERT(Y.hasValue(ball::Attribute("uuid", "A")));

            ASSERT(0 <  defaultAllocator.numBytesInUse());
            ASSERT(0 == testAllocator.numBytesInUse());

            mY.remove(ball::Attribute("uuid", 1));
            mY.remove(ball::Attribute("uuid", "A"));

            ASSERT(!Y.hasValue(ball::Attribute("uuid", 1)));
            ASSERT(!Y.hasValue(ball::Attribute("uuid", "A")));
        }
        ASSERT(0 == defaultAllocator.numBytesInUse());

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
