// ball_attributecontext.h                                            -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BALL_ATTRIBUTECONTEXT
#define INCLUDED_BALL_ATTRIBUTECONTEXT

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a container for storing attributes and caching results.
//
//@CLASSES:
//         ball::AttributeContext: thread-local list of attribute containers
//  ball::AttributeContextProctor: proctor for deleting an attribute context
//
//@SEE_ALSO: ball_attributecontainer
//
//@AUTHOR: Gang Chen (gchen20), Mike Verschell (hversche)
//
//@DESCRIPTION: This component provides a mechanism, 'ball::AttributeContext',
// used for storing attributes in thread-local storage and evaluating rules
// associated with a given category using those stored attributes, and a scoped
// proctor, 'ball::AttributeContextProctor', used for destroying the attribute
// context of the current thread.
//
// This component participates in the implementation of "Rule-Based Logging".
// For more information on how to use that feature, please see the package
// level documentation and usage examples for "Rule-Based Logging".
//
// Clients obtain the attribute context for the current thread by calling the
// 'getContext' class method.  Attributes are added and removed from an
// attribute context using the 'addAttributes' and 'removeAttributes' methods,
// respectively.  Additionally, 'ball::AttributeContext' provides methods, used
// primarily by other components in the 'ball' package' (see {Active Rules}
// below), to determine the effect of the current logging rules on the logging
// thresholds of a category.
//
// 'ball::AttributeContext' contains class data members that must be
// initialized, using the 'initialize' class method, with a 'CategoryManager'
// object containing a 'RuleSet' representing the currently installed (global)
// logging rules for the process.  However, clients generally should not call
// 'initialize' directly.  Instead, 'initialize' is called *internally* when
// the logger manager singleton is initialized.
//
///Active Rules
///------------
// 'ball::AttributeContext' provides two methods, 'hasRelevantActiveRules' and
// 'determineThresholdLevels', that are used to determine the effect of the
// current logging rules maintained by the category manager on the logging
// thresholds of a given category.  Note that these methods are generally
// intended for use by other components in the 'ball' package'.
//
// 'hasRelevantActiveRules' returns 'true' if there is at least one relevant
// and active rule (in the global set of rules) that might modify the logging
// thresholds of the supplied 'category'.  A rule is "relevant" if the rule's
// pattern matches the category's name, and a rule is "active" if all the
// predicates defined for that rule are satisfied by the current thread's
// attributes (i.e., 'ball::Rule::evaluate' returns 'true' for the collection
// of attributes maintained for the current thread by the thread's
// 'ball::AttributeContext' object).
//
// 'determineThresholdLevels' returns the logging threshold levels for a
// category, factoring in any active rules that apply to the category that
// might override the category's thresholds.
//
///Usage
///-----
// This section illustrates the intended use of 'ball::AttributeContext'.
//
///Example 1: Managing Attributes
///- - - - - - - - - - - - - - -
// First we will define a thread function that will create and install two
// attributes.  Note that we will use the 'AttributeSet' implementation of the
// 'ball::AttributeContainer' protocol defined in the component documentation
// for 'ball_attributecontainer'; the 'ball' package provides a similar class
// in the 'ball_defaultattributecontainer' component.
//..
//  extern "C" void *thread1(void *)
//  {
//..
// Inside this thread function, we create an attribute set to hold our
// attribute values, then we create two 'ball::Attribute' objects and add them
// to that set:
//..
//      AttributeSet attributes;
//      ball::Attribute a1("uuid", 4044457);
//      ball::Attribute a2("name", "Gang Chen");
//      attributes.insert(a1);
//      attributes.insert(a2);
//..
// Next, we obtain a reference to the current thread's attribute context using
// the 'getContext' class method:
//..
//      ball::AttributeContext *context = ball::AttributeContext::getContext();
//      assert(context);
//      assert(context == ball::AttributeContext::lookupContext());
//..
// We can add our attribute container, 'attributes', to the current context
// using the 'addAttributes' method.  We store the returned iterator so that
// we can remove 'attributes' before it goes out of scope and is destroyed:
//..
//      ball::AttributeContext::iterator it =
//                                         context->addAttributes(&attributes);
//      assert(context->hasAttribute(a1));
//      assert(context->hasAttribute(a2));
//..
// We then call the 'removeAttributes' method to remove the attributes from
// the attribute context:
//..
//      context->removeAttributes(it);
//      assert(false == context->hasAttribute(a1));
//      assert(false == context->hasAttribute(a2));
//..
// This completes the first thread function:
//..
//      return 0;
//  }
//..
// The second thread function will simply verify that there is no currently
// available attribute context.  Note that attribute contexts are created and
// managed by individual threads using thread-specific storage, and that
// attribute contexts created by one thread are not visible in any other
// threads:
//..
//  extern "C" void *thread2(void *)
//  {
//      assert(0 == ball::AttributeContext::lookupContext());
//      return 0;
//  }
//..
//
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
// this initialization should *not* be performed by clients of the 'ball'
// package: 'ball::AttributeContext::initialize' is called *internally* as part
// of the initialization of the 'ball::LoggerManager' singleton.
//..
//  ball::CategoryManager categoryManager;
//  ball::AttributeContext::initialize(&categoryManager);
//..
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
//  const ball::Category *cat1 =
//                  categoryManager.addCategory("MyCategory", 128, 96, 64, 32);
//..
// Next, we obtain the context for the current thread:
//..
//  ball::AttributeContext *context = ball::AttributeContext::getContext();
//..
// We call 'hasRelevantActiveRules' on 'cat1'.  This will be 'false' because
// we haven't supplied any rules:
//..
//  assert(!context->hasRelevantActiveRules(cat1));
//..
// We call 'determineThresholdLevels' on 'cat1'.  This will simply return the
// logging threshold levels we defined for 'cat1' when it was created because
// no rules have been defined that might modify those thresholds:
//..
//  ball::ThresholdAggregate cat1ThresholdLevels(0, 0, 0, 0);
//  context->determineThresholdLevels(&cat1ThresholdLevels, cat1);
//  assert(128 == cat1ThresholdLevels.recordLevel());
//  assert( 96 == cat1ThresholdLevels.passLevel());
//  assert( 64 == cat1ThresholdLevels.triggerLevel());
//  assert( 32 == cat1ThresholdLevels.triggerAllLevel());
//..
// Next, we create a rule that will apply to those categories whose names match
// the pattern "My*", where '*' is a wild-card value.  The rule defines a set
// of thresholds levels that may override the threshold levels of those
// categories whose name matches the rule's pattern:
//..
//  ball::Rule myRule("My*", 120, 110, 70, 40);
//  categoryManager.addRule(myRule);
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
//  assert(context->hasRelevantActiveRules(cat1));
//..
// Next, we call 'determineThresholdLevels' for 'cat1'.  This method compares
// the threshold levels defined for a category with those of any active rules
// that apply to that category, and determines the minimum severity (i.e., the
// maximum numerical value) for each respective threshold amongst those values.
//..
//  ball::ThresholdAggregate thresholdLevels(0, 0, 0, 0);
//  context->determineThresholdLevels(&thresholdLevels, cat1);
//  assert(128 == thresholdLevels.recordLevel());
//  assert(110 == thresholdLevels.passLevel());
//  assert( 70 == thresholdLevels.triggerLevel());
//  assert( 40 == thresholdLevels.triggerAllLevel());
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
//  categoryManager.removeRule(myRule);
//  ball::Predicate predicate("uuid", 3938908);
//  myRule.addPredicate(predicate);
//  categorymanager.addRule(myRule);
//..
// When we again call 'hasRelevantActiveRules' for 'cat1', it now returns
// 'false'.  The rule, 'myRule', still applies to 'cat1' (i.e., it is still
// "relevant" to 'cat1'), but the predicates defined by 'myRule' are no longer
// satisfied by the current thread, i.e., the current thread's attribute
// context does not contain an attribute matching '("uuid", 3938908)'.
//..
//  assert(!context->hasRelevantActiveRules(cat1));
//..
// Next, we call 'determineThresholdLevels' on 'cat1' and find that it
// returns the threshold levels we defined for 'cat1' when we created it:
//..
//  context->determineThresholdLevels(&thresholdLevels, cat1);
//  assert(thresholdLevels == cat1ThresholdLevels);
//..
// Finally, we add an attribute to the current thread's attribute context (as
// we did in the first example, "Managing Attributes").  Note that we keep an
// iterator referring to the added attributes so that we can remove them before
// 'attributes' goes out of scope and is destroyed.  Also note that the class
// 'AttributeSet' is defined in the component documentation for
// 'ball_attributecontainer'.
//..
//  AttributeSet attributes;
//  attributes.insert(ball::Attribute("uuid", 3938908));
//  ball::AttributeContext::iterator it = context->addAttributes(&attributes);
//..
// The following call to 'hasRelevantActiveRules' will return 'true' for 'cat1'
// because there is at least one rule, 'myRule', that is both "relevant"
// (i.e., its pattern matches the category name of 'cat1') and "active" (i.e.,
// all of the predicates defined for 'myRule' are satisfied by the attributes
// held by this thread's attribute context):
//..
//  assert(context->hasRelevantActiveRules(cat1));
//..
// Now, when we call 'determineThresholdLevels', it will again return the
// maximum threshold level from 'cat1' and 'myRule':
//..
//  context->determineThresholdLevels(&thresholdLevels, cat1);
//  assert(128 == thresholdLevels.recordLevel());
//  assert(110 == thresholdLevels.passLevel());
//  assert( 70 == thresholdLevels.triggerLevel());
//  assert( 40 == thresholdLevels.triggerAllLevel());
//..
// We must be careful to remove 'attributes' from the attribute context before
// it goes out of scope and is destroyed.  Note that the 'ball' package
// provides a component, 'ball_scopedattributes', for adding, and automatically
// removing, attributes from the current thread's attribute context.
//..
//  context->removeAttributes(it);
//..

#include <balscm_version.h>

#include <ball_attributecontainerlist.h>
#include <ball_ruleset.h>

#include <bslma_allocator.h>

#include <bslmt_threadutil.h>

#include <bsls_assert.h>
#include <bsls_types.h>

#include <bsl_iosfwd.h>

namespace BloombergLP {
namespace ball {

class Attribute;
class AttributeContainer;
class Category;
class CategoryManager;
class ThresholdAggregate;

             // ==========================================
             // class AttributeContext_RuleEvaluationCache
             // ==========================================

class AttributeContext_RuleEvaluationCache {
    // This is an implementation type of 'AttributeContext' and should not be
    // used by clients of this package.  A rule evaluation cache is a mechanism
    // for evaluating and caching whether a rule is active.  A rule is
    // considered active if all of its predicates are satisfied by the
    // collection of attributes held in a 'AttributeContainerList' object
    // (i.e., 'Rule::evaluate' returns 'true' for the 'AttributeContainerList'
    // object).  The rules this cache evaluates are contained in a 'RuleSet'
    // object.  'RuleSet::MaskType' is a bit mask for a rule set, where each
    // bit is a boolean value associated with the rule at the corresponding
    // index in a rule set.  An 'AttributeContext' determines, using the
    // 'isDataAvailable' method, if a particular set of rules (described using
    // a bit mask) have already been evaluated.  A context accesses the current
    // cache of rule evaluations using the 'knownActiveRules' method.  Finally,
    // a context updates the cache of rule evaluations using the 'update'
    // method.  Note that the 'isDataAvailable' method should be used prior to
    // using 'knownActiveRules' in order to ensure the relevant rules have been
    // evaluated and that those evaluations are up-to-date.

    // DATA
    RuleSet::MaskType  d_evalMask;        // set of bits, each of which
                                          // indicates whether the
                                          // corresponding rule has been
                                          // evaluated and cached in
                                          // 'd_resultMask' (1 if evaluated and
                                          // 0 otherwise)

    RuleSet::MaskType  d_resultMask;      // set of bits, each of which caches
                                          // the result of the most recent
                                          // evaluation of the corresponding
                                          // rule (1 if the rule is active and
                                          // 0 otherwise)

    bsls::Types::Int64 d_sequenceNumber;  // sequence number used to determine
                                          // if this cache is in sync with the
                                          // rule set maintained by the
                                          // category manager (see 'update');
                                          // if the sequence number changes it
                                          // indicates the cache is out of date

    // NOT IMPLEMENTED
    AttributeContext_RuleEvaluationCache(
                                  const AttributeContext_RuleEvaluationCache&);
    AttributeContext_RuleEvaluationCache& operator=(
                                  const AttributeContext_RuleEvaluationCache&);

  public:
    // CREATORS
    AttributeContext_RuleEvaluationCache();
        // Create an empty rule evaluation cache having a sequence number of
        // -1.

    // ~AttributeContext_RuleEvaluationCache();
        // Destroy this rule evaluation cache.  Note that this trivial
        // destructor is generated by the compiler.

    // MANIPULATORS
    void clear();
        // Clear any currently cached rule evaluation data, restoring this
        // object to its default constructed state (empty).

    RuleSet::MaskType update(bsls::Types::Int64            sequenceNumber,
                             RuleSet::MaskType             relevantRulesMask,
                             const RuleSet&                rules,
                             const AttributeContainerList& attributes);
        // Update, for the specified 'sequenceNumber', the cache for those
        // rules indicated by the specified 'relevantRulesMask' bit mask in the
        // specified set of 'rules', by evaluating those rules for the
        // specified 'attributes'; return the bit mask indicating those rules
        // that are known to be active.  If a bit in the returned bit mask
        // value is set to 1, the rule at the corresponding index in 'rules' is
        // "active"; however, if a bit is set to 0, the corresponding rule is
        // either not active *or* has not been evaluated.  This operation does,
        // however, guarantee that all the rules indicated by the
        // 'relevantRulesMask' *will* be evaluated.  A particular rule is
        // considered "active" if all of its predicates are satisfied by
        // 'attributes' (i.e., if 'Rule::evaluate' returns 'true' for
        // 'attributes').  The behavior is undefined unless 'rules' is not
        // modified during this operation (i.e., any lock associated with
        // 'rules' must be locked during this operation).

    // ACCESSORS
    bool isDataAvailable(bsls::Types::Int64 sequenceNumber,
                         RuleSet::MaskType  relevantRulesMask) const;
        // Return 'true' if this cache contains up-to-date cached rule
        // evaluations having the specified 'sequenceNumber' for the set of
        // rules indicated by the specified 'relevantRulesMask' bit mask, and
        // 'false' otherwise.

    RuleSet::MaskType knownActiveRules() const;
        // Return a bit mask indicating those rules, from the set of rules
        // provided in the last call to 'update', that are known to be active
        // (as of that last call to 'update').  If a bit in the returned value
        // is set to 1, the rule at the corresponding index is active; however,
        // if a bit is set to 0, the corresponding rule is either not active
        // *or* has not been evaluated.  Note that 'isDataAvailable' should be
        // called to test if this cache contains up-to-date evaluated rule
        // information for the rules in which they are interested before using
        // the result of this method.

    bsl::ostream& print(bsl::ostream& stream,
                        int           level = 0,
                        int           spacesPerLevel = 4) const;
        // Format this object to the specified output 'stream' at the (absolute
        // value of) the optionally specified indentation 'level' and return a
        // reference to 'stream'.  If 'level' is specified, optionally specify
        // 'spacesPerLevel', the number of spaces per indentation level for
        // this and all of its nested objects.  If 'level' is negative,
        // suppress indentation of the first line.  If 'spacesPerLevel' is
        // negative, format the entire output on one line, suppressing all but
        // the initial indentation (as governed by 'level').  If 'stream' is
        // not valid on entry, this operation has no effect.
};

// FREE OPERATORS
bsl::ostream& operator<<(
                     bsl::ostream&                               stream,
                     const AttributeContext_RuleEvaluationCache& cache);
    // Write a description of the data members of the specified 'cache' to the
    // specified 'stream' in some single-line human readable format, and return
    // the modifiable 'stream'.

                        // ======================
                        // class AttributeContext
                        // ======================

class AttributeContext {
    // This class provides a mechanism for associating attributes with the
    // current thread, and evaluating the logging rules associated with a
    // category using those stored attributes.  'AttributeContext' contains
    // class data members that must be initialized (using the 'initialize'
    // class method) with a 'CategoryManager' object containing a 'RuleSet'
    // that represents the currently installed logging rules for the process.
    // Clients can obtain the context for the current thread by calling the
    // 'getContext' class method.  The 'addAttributes' and 'removeAttributes'
    // methods are used to add and remove collections of attributes from the
    // (thread-local) context object.  Finally, 'AttributeContext' provides
    // methods (used primarily by other components in the 'ball' package') to
    // determine the effect of the current logging rules on the logging
    // thresholds of a category.  The 'hasRelevantActiveRules' method returns
    // 'true' if there are any relevant and active rules that might modify the
    // logging thresholds of the supplied category.  A rule is "relevant" if
    // the rule's pattern matches the category's name, and a rule is "active"
    // if all the predicates defined for the rule are satisfied by the current
    // thread's attributes (i.e., 'Rule::evaluate' returns 'true' for the
    // collection of attributes maintained for the current thread by the
    // thread's 'AttributeContext' object).  The 'determineThresholdLevels'
    // method returns the logging threshold levels for a category, factoring in
    // any active rules that apply to the category that might override the
    // category's thresholds.  The behavior for the 'hasRelevantActiveRules'
    // and 'determineThresholdLevels' methods is undefined unless 'initialize'
    // has been called.  Note that, in practice, 'initialize' is called
    // *internally* when the logger manager singleton is initialized; clients
    // ordinarily should not call 'initialize' directly.

    // PRIVATE TYPES
    typedef AttributeContext_RuleEvaluationCache RuleEvaluationCache;

    // CLASS DATA
    static CategoryManager  *s_categoryManager_p;  // holds the rule set, rule
                                                   // set sequence number, and
                                                   // rule set mutex

    static bslma::Allocator *s_globalAllocator_p;  // allocator for thread-
                                                   // local context objects

    // DATA
    AttributeContainerList   d_containerList;      // list of attribute
                                                   // containers

    mutable RuleEvaluationCache
                             d_ruleCache_p;        // cache of rule evaluations

    bslma::Allocator        *d_allocator_p;        // allocator used to create
                                                   // this object (held, not
                                                   // owned)

    // FRIENDS
    friend class AttributeContextProctor;

    // NOT IMPLEMENTED
    AttributeContext(const AttributeContext&);
    AttributeContext& operator=(const AttributeContext&);

    // PRIVATE CLASS METHODS
    static const bslmt::ThreadUtil::Key& contextKey();
        // Return a 'const' reference to the singleton key for the thread-local
        // storage in which the 'AttributeContext' object is stored.  This
        // method creates the key on the first invocation; all subsequent
        // invocations return the key created on the initial call.  Note that
        // it is more efficient to cache the return value of this method than
        // to invoke it repeatedly.

    static void removeContext(void *arg);
        // Destroy the 'AttributeContext' object pointed to by the specified
        // 'arg'.  Note that this function is intended to be called by the
        // thread-specific storage facility when a thread exits.

    // PRIVATE CREATORS
    AttributeContext(bslma::Allocator *globalAllocator = 0);
        // Create an 'AttributeContext' object initially having no attributes.
        // Optionally specify a 'globalAllocator' used to supply memory.  If
        // 'globalAllocator' is 0, the currently installed global allocator is
        // used.  Note that the 'getContext' class method must be used to
        // obtain the address of the attribute context for the current thread.

    ~AttributeContext();
        // Destroy this object.

  public:
    // PUBLIC TYPES
    typedef AttributeContainerList::iterator iterator;

    // CLASS METHODS
    static AttributeContext *getContext();
        // Return the address of the current thread's attribute context, if
        // such context exists; otherwise, create an attribute context, install
        // it in thread-local storage, and return the address of the newly
        // created context.  Note that this method can be invoked safely even
        // if the 'initialize' class method has not yet been called.

    static void initialize(CategoryManager  *categoryManager,
                           bslma::Allocator *globalAllocator = 0);
        // Initialize the static data members of 'AttributeContext' using the
        // specified 'categoryManager'.  Optionally specify a 'globalAllocator'
        // used to supply memory.  If 'globalAllocator' is 0, the currently
        // installed global allocator is used.  Unless 'reset' is subsequently
        // called, invoking this method more than once will log an error
        // message using 'bsls::Log::platformDefaultMessageHandler', but will
        // have no other effect.  Note that in practice this method will be
        // called *automatically* when the 'LoggerManager' singleton is
        // initialized -- i.e., it is not intended to be called directly by
        // clients of the 'ball' package.

    static AttributeContext *lookupContext();
        // Return the address of the modifiable 'AttributeContext' object
        // installed in local storage for the current thread, or 0 if no
        // attribute context has been created for this thread.  Note that this
        // method can be invoked safely even if the 'initialize' class method
        // has not yet been called.

    static void reset();
        // Reset the static data members of 'AttributeContext' to their initial
        // state (0).  Unless 'initialize' is subsequently called, invoking
        // this method more than once has no effect.  Note that in practice
        // this method will be called *automatically* when the 'LoggerManager'
        // singleton is destroyed -- i.e., it is not intended to be called
        // directly by clients of the 'ball' package.

    // MANIPULATORS
    iterator addAttributes(const AttributeContainer *attributes);
        // Add the specified 'attributes' to the list of attribute containers
        // maintained by this object.  The behavior is undefined unless
        // 'attributes' remains valid *and* *unmodified* until either
        // 'attributes' is removed from this context, 'clearCache' is called,
        // or this object is destroyed.  Note that this method can be invoked
        // safely even if the 'initialize' class method has not yet been
        // called.

    void clearCache();
        // Clear this object's cache of evaluated rules.  Note that this method
        // must be called if an 'AttributeContainer' object supplied to
        // 'addAttributes' is modified outside of this context.

    void removeAttributes(iterator element);
        // Remove the specified 'element' from the list of attribute containers
        // maintained by this object.  Note that this method can be invoked
        // safely even if the 'initialize' class method has not yet been
        // called.

    // ACCESSORS
    bool hasRelevantActiveRules(const Category *category) const;
        // Return 'true' if there is at least one rule defined for this process
        // that is both "relevant" to the specified 'category' and "active",
        // and 'false' otherwise.  A rule is "relevant" to 'category' if the
        // rule's pattern matches 'category->categoryName()', and a rule is
        // "active" if all the predicates defined for that rule are satisfied
        // by the current thread's attributes (i.e., 'Rule::evaluate' returns
        // 'true' for the collection of attributes maintained by this object).
        // This method operates on the set of rules maintained by the category
        // manager supplied to the 'initialize' class method (which, in
        // practice, should be the global set of rules for the process).  The
        // behavior is undefined unless 'initialize' has previously been
        // invoked without a subsequent call to 'reset', and 'category' is
        // contained in the registry maintained by the category manager
        // supplied to 'initialize'.

    void determineThresholdLevels(ThresholdAggregate *levels,
                                  const Category     *category) const;
        // Populate the specified 'levels' with the threshold levels for the
        // specified 'category'.  This method compares the threshold levels
        // defined by 'category' with those of any active rules that apply to
        // that category, and determines the minimum severity (i.e., the
        // maximum numerical value) for each respective threshold amongst those
        // values.  A rule applies to 'category' if the rule's pattern matches
        // 'category->categoryName()', and a rule is active if all the
        // predicates defined for that rule are satisfied by the current
        // thread's attributes (i.e., 'Rule::evaluate' returns 'true' for the
        // collection of attributes maintained by this object).  This method
        // operates on the set of rules maintained by the category manager
        // supplied to the 'initialize' class method (which, in practice,
        // should be the global set of rules for the process).  The behavior is
        // undefined unless 'initialize' has previously been invoked without a
        // subsequent call to 'reset', and 'category' is contained in the
        // registry maintained by the category manager supplied to
        // 'initialize'.

    bool hasAttribute(const Attribute& value) const;
        // Return 'true' if an attribute having the specified 'value' exists in
        // any of the attribute containers maintained by this object, and
        // 'false' otherwise.  Note that this method can be invoked safely even
        // if the 'initialize' class method has not yet been called.

    const AttributeContainerList& containers() const;
        // Return a 'const' reference to the list of attribute containers
        // maintained by this object.  Note that this method can be invoked
        // safely even if the 'initialize' class method has not yet been
        // called.

    bsl::ostream& print(bsl::ostream& stream,
                        int           level = 0,
                        int           spacesPerLevel = 4) const;
        // Format this object to the specified output 'stream' at the (absolute
        // value of) the optionally specified indentation 'level' and return a
        // reference to 'stream'.  If 'level' is specified, optionally specify
        // 'spacesPerLevel', the number of spaces per indentation level for
        // this and all of its nested objects.  If 'level' is negative,
        // suppress indentation of the first line.  If 'spacesPerLevel' is
        // negative, format the entire output on one line, suppressing all but
        // the initial indentation (as governed by 'level').  If 'stream' is
        // not valid on entry, this operation has no effect.
};

// FREE OPERATORS
bsl::ostream& operator<<(bsl::ostream&           stream,
                         const AttributeContext& context);
    // Write a description of the data members of the specified 'context' to
    // the specified 'stream' in a single-line human readable format, and
    // return a reference to the modifiable 'stream'.

                     // =============================
                     // class AttributeContextProctor
                     // =============================

class AttributeContextProctor {
    // This class implements a proctor that, on its own destruction, will
    // destroy the attribute context of the current thread.  Attribute contexts
    // are stored in thread-local memory.  On destruction, objects of this type
    // will deallocate the current thread's attribute context (if one has been
    // created), and set the thread-local storage pointer to 0.

    // NOT IMPLEMENTED
    AttributeContextProctor(const AttributeContextProctor&);
    AttributeContextProctor& operator=(const AttributeContextProctor&);

  public:
    // CREATORS
    explicit AttributeContextProctor();
        // Create an 'AttributeContextProctor' object that will destroy the
        // current attribute context on destruction.

    ~AttributeContextProctor();
        // Destroy this object (as well as the current attribute context).
};

// ============================================================================
//                              INLINE DEFINITIONS
// ============================================================================

               // ------------------------------------------
               // class AttributeContext_RuleEvaluationCache
               // ------------------------------------------

// CREATORS
inline
AttributeContext_RuleEvaluationCache::AttributeContext_RuleEvaluationCache()
: d_evalMask(0)
, d_resultMask(0)
, d_sequenceNumber(-1)
{
}

// MANIPULATORS
inline
void AttributeContext_RuleEvaluationCache::clear()
{
    d_evalMask       =  0;
    d_resultMask     =  0;
    d_sequenceNumber = -1;
}

// ACCESSORS
inline
bool AttributeContext_RuleEvaluationCache::isDataAvailable(
                                    bsls::Types::Int64 sequenceNumber,
                                    RuleSet::MaskType  relevantRulesMask) const
{
    return sequenceNumber    == d_sequenceNumber
        && relevantRulesMask == (relevantRulesMask & d_evalMask);
}

inline
RuleSet::MaskType
AttributeContext_RuleEvaluationCache::knownActiveRules() const
{
    return d_resultMask;
}

                        // ----------------------
                        // class AttributeContext
                        // ----------------------

// MANIPULATORS
inline
AttributeContext::iterator
AttributeContext::addAttributes(const AttributeContainer *attributes)
{
    BSLS_ASSERT_SAFE(attributes);

    d_ruleCache_p.clear();
    return d_containerList.pushFront(attributes);
}

inline
void AttributeContext::clearCache()
{
    d_ruleCache_p.clear();
}

inline
void AttributeContext::removeAttributes(iterator element)
{
    d_ruleCache_p.clear();
    d_containerList.remove(element);
}

// ACCESSORS
inline
const AttributeContainerList& AttributeContext::containers() const
{
    return d_containerList;
}

inline
bool AttributeContext::hasAttribute(const Attribute& value) const
{
    return d_containerList.hasValue(value);
}

                        // -----------------------------
                        // class AttributeContextProctor
                        // -----------------------------

// CREATORS
inline
AttributeContextProctor::AttributeContextProctor()
{
}

}  // close package namespace

// FREE OPERATORS
inline
bsl::ostream& ball::operator<<(
                            bsl::ostream&                               stream,
                            const AttributeContext_RuleEvaluationCache& cache)
{
    return cache.print(stream, 0, -1);
}

inline
bsl::ostream& ball::operator<<(bsl::ostream&           stream,
                               const AttributeContext& context)
{
    return context.print(stream, 0, -1);
}

}  // close enterprise namespace

#endif

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
