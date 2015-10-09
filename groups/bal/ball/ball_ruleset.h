// ball_ruleset.h                                                     -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BALL_RULESET
#define INCLUDED_BALL_RULESET

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a set of unique rules.
//
//@CLASSES:
//    ball::RuleSet: container for unique rules
//
//@SEE_ALSO: ball_rule
//
//@DESCRIPTION: This component provides a value-semantic container,
// 'ball::RuleSet', for storage and efficient retrieval of 'ball::Rule'
// objects.
//
///Thread Safety
///-------------
// 'ball::RuleSet' is *not* thread-safe in that multiple threads attempting to
// concurrently modify the same 'ball::RuleSet' object will leave the object in
// an undefined state.  To ensure thread-safety, concurrent accesses to a
// 'ball::RuleSet' must be serialized by a mutex.
//
///Usage
///-----
// The following code fragments illustrate how to use a rule set.
//
// We first create a rule whose pattern is 'WEEKEND*' and whose threshold
// levels are all 'ball::Severity::e_OFF' except the 'pass-through' level.  A
// 'pass-through' level of 'ball::Severity::e_INFO' indicates that whenever the
// rule is active and the severity equals or exceeds 'ball::Severity::e_INFO',
// log records will be passed to the observer:
//..
//  ball::Rule rule1("WEEKEND*",               // pattern
//                  ball::Severity::e_OFF,     // record level
//                  ball::Severity::e_INFO,    // pass-through level
//                  ball::Severity::e_OFF,     // trigger level
//                  ball::Severity::e_OFF);    // triggerAll level
//..
// Next, we create another rule having a different pattern, but the same
// threshold levels:
//..
//  ball::Rule rule2("WEEKDAY*",               // pattern
//                  ball::Severity::e_OFF,     // record level
//                  ball::Severity::e_INFO,    // pass-through level
//                  ball::Severity::e_OFF,     // trigger level
//                  ball::Severity::e_OFF);    // triggerAll level
//..
// We then create a 'ball::RuleSet' object, add the two rules, and verify that
// rules were added correctly:
//..
//  ball::RuleSet ruleSet;
//  assert(0 <= ruleSet.addRule(rule1));
//  assert(0 <= ruleSet.addRule(rule2));
//  assert(2 == ruleSet.numRules());
//..
// Duplicate rules cannot be added:
//..
//  assert(-1 == ruleSet.addRule(rule1));
//  assert(-1 == ruleSet.addRule(rule2));
//  assert( 2 == ruleSet.numRules());
//..
// Rules in a rule set can be looked up by the 'ruleId' method:
//..
//  int i1 = ruleSet.ruleId(rule1);
//  int i2 = ruleSet.ruleId(rule2);
//  assert(0 <= i1); assert(i1 < ruleSet.maxNumRules());
//  assert(0 <= i2); assert(i2 < ruleSet.maxNumRules());
//  assert(i1 != i2);
//..
// The 'removeRule' method can be used to remove rules from a rule set.
//..
//  assert(ruleSet.removeRule(rule1));
//  assert(1 == ruleSet.numRules());
//  assert(ruleSet.ruleId(rule1) < 0);
//  assert(ruleSet.ruleId(rule2) == i2);
//..

#ifndef INCLUDED_BALSCM_VERSION
#include <balscm_version.h>
#endif

#ifndef INCLUDED_BALL_RULE
#include <ball_rule.h>
#endif

#ifndef INCLUDED_BSLMA_ALLOCATOR
#include <bslma_allocator.h>
#endif

#ifndef INCLUDED_BSLMA_USESBSLMAALLOCATOR
#include <bslma_usesbslmaallocator.h>
#endif

#ifndef INCLUDED_BSLMF_NESTEDTRAITDECLARATION
#include <bslmf_nestedtraitdeclaration.h>
#endif

#ifndef INCLUDED_BSL_UNORDERED_SET
#include <bsl_unordered_set.h>
#endif

#ifndef INCLUDED_BSL_VECTOR
#include <bsl_vector.h>
#endif

namespace BloombergLP {

namespace ball {
                          // =============
                          // class RuleSet
                          // =============

class RuleSet {
    // This class manages a set of unique rule values.  Rules may be added to
    // or removed from the set; however, rules having duplicate values will
    // not be added.  For the definition of two rules having the same value,
    // please refer to the function-level documentation associated with the
    // 'Rule::operator==' function.

  public:
    // PUBLIC TYPES
    typedef unsigned int MaskType;
        // 'MaskType' is an alias for the fundamental integral type used to
        // indicate rule subsets compactly.

    enum {
        e_MAX_NUM_RULES = 8 * sizeof(MaskType)
           // The maximum number of rules managed by this object.

    };

  private:
    // PRIVATE TYPES
    struct RuleHash
        // hash functor for 'Rule'
    {
      private:
        static int s_hashtableSize;              // the default hashtable size
      public:
        int operator()(const Rule& rule) const
            // Return the hash value of the specified 'rule'.
        {
            return Rule::hash(rule, s_hashtableSize);
        }
    };

    typedef bsl::unordered_set<Rule, RuleHash> HashtableType;

    // DATA
    HashtableType              d_ruleHashtable;  // the hash table that
                                                 // manages all the rules
                                                 // maintained by this rule
                                                 // set

    bsl::vector<const Rule *>  d_ruleAddresses;  // secondary map between ids
                                                 // and the addresses of rules

    bsl::vector<int>           d_freeRuleIds;    // rule Ids that are not being
                                                 // used

    int                        d_numPredicates;  // total number of predicates

    // FRIENDS
    friend bool operator==(const RuleSet&,    const RuleSet&);
    friend bool operator!=(const RuleSet&,    const RuleSet&);
    friend bsl::ostream& operator<<(bsl::ostream&, const RuleSet&);

  public:
    // CLASS METHODS
    static int maxNumRules();
        // Return the maximum number of rules that can be simultaneously
        // maintained by this object.

    static void printMask(bsl::ostream& stream,
                          MaskType      mask,
                          int           level = 0,
                          int           spacesPerLevel = 0);
        // Format the specified 'mask' to the specified output 'stream' at the
        // optionally specified indentation 'level' and return a reference to
        // the modifiable 'stream'.  If 'level' is specified, optionally
        // specify 'spacesPerLevel', the number of spaces per indentation level
        // for this and all of its nested objects.  Each line is indented by
        // the absolute value of 'level * spacesPerLevel'.  If 'level' is
        // negative, suppress indentation of the first line.  If
        // 'spacesPerLevel' is negative, suppress line breaks and format the
        // entire output on one line.  If 'stream' is initially invalid, this
        // operation has no effect.

    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(RuleSet, bslma::UsesBslmaAllocator);


    // CREATORS
    explicit RuleSet(bslma::Allocator *basicAllocator = 0);
        // Create an empty rule set.  Optionally specify a 'basicAllocator'
        // used to supply memory.  If 'basicAllocator' is 0, the currently
        // installed default allocator will be used.

    RuleSet(const RuleSet&    original,
            bslma::Allocator *basicAllocator = 0);
        // Create a 'RuleSet' object having the same value as that of the
        // specified 'original' object.  Optionally specify a 'basicAllocator'
        // used to supply memory.  If 'basicAllocator' is 0, the currently
        // installed default allocator will be used.

    ~RuleSet();
        // Destroy this rule set.

    // MANIPULATOR
    int addRule(const Rule& value);
        // Create a new 'Rule' object having the specified 'value'.  Return the
        // non-negative id of this non-modifiable object on success, and a
        // negative value otherwise.  A return value of -1 indicates that
        // another rule having this value already exists.  A return value of -2
        // indicates that the maximum number of rules for this rule set has
        // been reached.

    int addRules(const RuleSet& rules);
        // Add each rule in the specified 'rules' to this rule set.  Return
        // the number of rules added.  Note that a rule in 'rules' will be
        // ignored if there is an existing rule having the same value or if
        // the number of rules in the set has reached the upper limit.  Also
        // note that if not all valid rules will fit, the (possibly empty)
        // subset of unique values that will be added is implementation
        // dependent.

    int removeRuleById(int id);
        // Remove from this rule set the rule having the specified 'id'.
        // Return the number of rules removed (i.e., 1 on success and 0 if
        // there is no rule whose id is 'id').  The behavior is undefined
        // unless '0 <= id < e_MAX_NUM_RULES'.

    int removeRule(const Rule& value);
        // Remove the rule having the specified 'value' from this rule set.
        // Return the number of rules removed (i.e., 1 on success and 0 if
        // there is no such a rule).

    int removeRules(const RuleSet& rules);
        // Remove each rule in the specified 'rules' from this rule set.
        // Return the number of rules removed.

    void removeAllRules();
        // Remove every rule in the rule set maintained by this object.

    RuleSet& operator=(const RuleSet& rhs);
        // Assign to this object the value of the specified 'rhs' object.

    // ACCESSORS
    int ruleId(const Rule& value) const;
        // Return the id of the rule having the specified 'value' if such a
        // rule exists, and a negative value otherwise.  Note that if there are
        // multiple rules having 'value', the id of the first one found will be
        // returned and the order in which rules are searched is implementation
        // dependent.

    const Rule *getRuleById(int id) const;
        // Return the address of the rule having the specified 'id' if such a
        // rule exists, and 0 otherwise.  The behavior is undefined unless
        // '0 <= id < maxNumRules()'.  Note that rules may be assigned
        // non-sequential identifiers, and that there may be a valid rule whose
        // identifier is greater than 'numRules()' (i.e., valid rules may
        // appear anywhere in the range '0 <= id < maxNumRules()').

    int numRules() const;
        // Return the number of unique rules maintained in this 'RuleSet'
        // object.  Note that this value is *not* the maximum identifier for
        // the rules currently in this container.

    int numPredicates() const;
        // Return the total number of predicates in all rules maintained by
        // this object.

    bsl::ostream& print(bsl::ostream& stream,
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

// FREE OPERATORS
bool operator==(const RuleSet& lhs, const RuleSet& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' rule sets have the same
    // value, and 'false' otherwise.  Two rule sets have the same value if
    // every rule that exists in one rule set also exists in the other.

bool operator!=(const RuleSet& lhs, const RuleSet& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' rule sets do not have the
    // same value, and 'false' otherwise.  Two rule sets do not have the same
    // value if there is at least one rule that exists in one rule but does not
    // exist in the other.

bsl::ostream& operator<<(bsl::ostream& output, const RuleSet& rules);
    // Write the value of the specified 'rules' to the specified 'output'
    // stream.  Return the specified 'output' stream

// ============================================================================
//                              INLINE DEFINITIONS
// ============================================================================

                          // -------------
                          // class RuleSet
                          // -------------

// CLASS METHODS
inline
int RuleSet::maxNumRules()
{
    return e_MAX_NUM_RULES;
}

// ACCESSORS
inline
const Rule *RuleSet::getRuleById(int id) const
{
    return d_ruleAddresses[id];
}

inline
int RuleSet::numRules() const
{
    return static_cast<int>(d_ruleHashtable.size());
}

inline
int RuleSet::numPredicates() const
{
    return d_numPredicates;
}

}  // close package namespace

inline
bsl::ostream& ball::operator<<(bsl::ostream& output, const RuleSet& rules)
{
    return rules.print(output, 0, -1);
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
