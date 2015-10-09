// ball_rule.h                                                        -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BALL_RULE
#define INCLUDED_BALL_RULE

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide an object having a pattern, thresholds, and predicates.
//
//@CLASSES:
//    ball::Rule: a pattern, thresholds, and predicate sets
//
//@SEE_ALSO: ball_ruleset
//
//@DESCRIPTION: This component implements a type, 'ball::Rule', that consists
// of a pattern, four threshold levels, and a set of predicates.  The pattern
// indicates the names of the categories for which the rule will become
// relevant.  The four threshold levels determine what actions will be
// performed on log records when their severity level equals or exceeds any of
// these threshold levels.  The predicate set is a collection of unique
// attribute name/value pairs.
//
// Note that multiple predicates with the same name are permitted so long as
// they correspond to different values.
//
///Usage
///-----
// The following code fragments illustrate how to create a rule and add
// predicates.
//
// We create a rule whose pattern is 'WEEKEND*' and whose threshold levels are
// all 'ball::Severity::OFF' except the 'pass-through' level.  A 'pass-through'
// level of 'ball::Severity::INFO' indicates that whenever the rule is active
// and the severity is equal to or exceeds 'ball::Severity::INFO', log records
// will be passed to the observer:
//..
//  ball::Rule rule("WEEKEND*",             // pattern
//                 ball::Severity::OFF,     // record level
//                 ball::Severity::INFO,    // pass-through level
//                 ball::Severity::OFF,     // trigger level
//                 ball::Severity::OFF);    // triggerAll level
//..
// Create some predicates and then add one to the rule:
//..
//  ball::Predicate p1("uuid", 4044457);
//  ball::Predicate p2("name", "Gang Chen");
//  rule.addPredicate(p1);
//..
// Predicates can be looked up by the 'hasPredicate' method:
//..
//  assert(true  == rule.hasPredicate(p1));
//  assert(false == rule.hasPredicate(p2));
//..
// We then add the other predicate:
//..
//  rule.addPredicate(p2);
//  assert(true  == rule.hasPredicate(p2));
//..
// Predicates can also be removed from the rule by the 'removePredicate'
// method:
//..
//  rule.removePredicate(p1);
//  assert(false == rule.hasPredicate(p1));
//  assert(true  == rule.hasPredicate(p2));
//..
// The pattern of a rule can be changed by the 'setPattern' method:
//..
//  assert(0 == strcmp(rule.pattern(), "WEEKEND*"));
//
//  rule.setPattern("WEEKDAY*");
//  assert(0 == strcmp(rule.pattern(), "WEEKDAY*"));
//..
// The threshold levels of a rule can also be modified by the 'setLevels'
// method:
//..
//  assert(ball::Severity::OFF  == rule.recordLevel());
//  assert(ball::Severity::INFO == rule.passLevel());
//  assert(ball::Severity::OFF  == rule.triggerLevel());
//  assert(ball::Severity::OFF  == rule.triggerAllLevel());
//
//  rule.setLevels(ball::Severity::INFO,
//                 ball::Severity::OFF,
//                 ball::Severity::INFO,
//                 ball::Severity::INFO);
//
//  assert(ball::Severity::INFO == rule.recordLevel());
//  assert(ball::Severity::OFF  == rule.passLevel());
//  assert(ball::Severity::INFO == rule.triggerLevel());
//  assert(ball::Severity::INFO == rule.triggerAllLevel());
//..

#ifndef INCLUDED_BALSCM_VERSION
#include <balscm_version.h>
#endif

#ifndef INCLUDED_BALL_PATTERNUTIL
#include <ball_patternutil.h>
#endif

#ifndef INCLUDED_BALL_PREDICATESET
#include <ball_predicateset.h>
#endif

#ifndef INCLUDED_BALL_THRESHOLDAGGREGATE
#include <ball_thresholdaggregate.h>
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

#ifndef INCLUDED_BSL_STRING
#include <bsl_string.h>
#endif

namespace BloombergLP {

namespace ball {

class AttributeContainerList;

                       // ==========
                       // class Rule
                       // ==========

class Rule {
    // This class defines a value-semantic object that holds a pattern, four
    // threshold levels, and a predicate set.  For each of these fields there
    // is an accessor for obtaining the field value and a manipulator for
    // changing that value.  There are a few methods as well for directly
    // adding/removing/searching predicates.
    //
    // Additionally, this class supports a complete set of *value* *semantic*
    // operations, including copy construction, assignment and equality
    // comparison, and 'ostream' printing.  A precise operational definition of
    // when two instances have the same value can be found in the description
    // of 'operator==' for the class.  This class is *exception* *neutral* with
    // no guarantee of rollback: If an exception is thrown during the
    // invocation of a method on a pre-existing instance, the object is left in
    // a valid state, but its value is undefined.  In no event is memory
    // leaked.  Finally, *aliasing* (e.g., using all or part of an object as
    // both source and destination) is supported in all cases.

    // DATA
    bsl::string        d_pattern;       // the pattern for the name of
                                        // categories to which this rule will
                                        // become relevant

    ThresholdAggregate d_thresholds;    // an aggregate of four threshold
                                        // levels

    PredicateSet       d_predicateSet;  // set of predicates

    mutable int        d_hashValue;     // the cached hash value; < 0 means
                                         // it's invalid

    mutable int        d_hashSize;      // the number of slots from which
                                        // 'd_hashValue' was calculated

    // FRIENDS
    friend bool operator==(const Rule&, const Rule&);
    friend bool operator!=(const Rule&, const Rule&);
    friend bsl::ostream& operator<<(bsl::ostream&, const Rule&);

  public:
    // CLASS METHODS
    static int hash(const Rule& rule, int size);
        // Return a hash value calculated from the specified 'rule' using the
        // specified 'size' as the number of slots.  The value returned is
        // guaranteed to be in the range [0 .. size - 1].

    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(Rule, bslma::UsesBslmaAllocator);

    // CREATORS
    explicit Rule(bslma::Allocator *basicAllocator = 0);
        // Create a 'Rule' object whose pattern is an empty string and whose
        // thresholds levels are all 0.  Optionally specify a 'basicAllocator'
        // used to supply memory.  If 'basicAllocator' is 0, the currently
        // installed default allocator will be used.  Note that a newly created
        // 'Rule' object does not have any predicates.

    // CREATORS
    Rule(const bslstl::StringRef& pattern,
         int                      recordLevel,
         int                      passLevel,
         int                      triggerLevel,
         int                      triggerAllLevel,
         bslma::Allocator        *basicAllocator = 0);
        // Create a 'Rule' object whose pattern is the specified 'pattern' and
        // whose thresholds levels are the specified 'recordLevel',
        // 'passLevel', 'triggerLevel', and 'triggerAllLevel' respectively.
        // Optionally specify a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator
        // will be used.  The behavior is undefined unless each of the four
        // threshold level values is not in the range [0 .. 255].  Note that a
        // newly created 'Rule' object does not have any predicates.

    Rule(const Rule&       original,
         bslma::Allocator *basicAllocator = 0);
        // Create a 'Rule' object that has the same value as that of the
        // specified 'original' object.  Optionally specify a 'basicAllocator'
        // used to supply memory.  If 'basicAllocator' is 0, the currently
        // installed default allocator will be used.

    ~Rule();
        // Destroy this object.

    // MANIPULATORS
    Rule& operator=(const Rule& rhs);
        // Assign to this object the value of the specified 'rhs' object.

    int addPredicate(const Predicate& value);
        // Add a predicate having the specified 'value' to this object.  Return
        // 1 on success and 0 if a predicate having the same value already
        // exists in this object.

    int removePredicate(const Predicate& value);
        // Remove the predicate having the specified 'value' from this object.
        // Return the number of predicates being removed (i.e., 1 on success
        // and 0 if no predicate having 'value' exists in this object).

    void removeAllPredicates();
        // Remove every predicate maintained by this object.

    int setLevels(int recordLevel,
                  int passLevel,
                  int triggerLevel,
                  int triggerAllLevel);
        // Set the threshold levels of this object to the specified
        // 'recordLevel', 'passLevel', 'triggerLevel', and 'triggerAllLevel'
        // values, respectively, if each of the specified values is in the
        // range [0 .. 255].  Return 0 on success, and a non-zero value
        // otherwise (with no effect on the threshold levels of this object).

    void setPattern(const bslstl::StringRef& value);
        // Set the pattern of this object to the specified 'value'.

    // ACCESSORS
    bool evaluate(const AttributeContainerList& containerList) const;
        // Return 'true' if for every predicate maintained by this object, an
        // attribute with the same name and value exists in the specified
        // 'containerList'; and 'false' otherwise.

    int numPredicates() const;
        // Return the number of predicates in by this object.

    bool hasPredicate(const Predicate& value) const;
        // Return 'true' if the predicate having specified 'value' exists in
        // this object, and 'false' otherwise.

    PredicateSet::const_iterator begin() const;
        // Return an iterator pointing at the first member of the predicate
        // set maintained by this object.

    PredicateSet::const_iterator end() const;
        // Return an iterator pointing at one past the last member of the
        // predicate set maintain

    int recordLevel() const;
        // Return the record level of this object.

    int passLevel() const;
        // Return the pass level of this object.

    int triggerLevel() const;
        // Return the trigger level of this object.

    int triggerAllLevel() const;
        // Return the trigger-all level of this object.

    const char *pattern() const;
        // Return the pattern of this object.

    bool isMatch(const char *inputString) const;
        // Return 'true' if the specified 'inputString' matches the pattern of
        // this rule, and 'false' otherwise.  (For the definition of a string
        // matching the pattern of a rule, please refer to the function-level
        // documentation associated with the 'PatternUtil::isMatch' function).

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
bool operator==(const Rule& lhs, const Rule& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects have the same
    // value, and 'false' otherwise.  Two 'Rule' objects have the same value if
    // they have the same predicate, the same four respective threshold levels,
    // and the same pattern.

bool operator!=(const Rule& lhs, const Rule& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects do not have the
    // same value, and 'false' otherwise.  Two 'Rule' objects do not have the
    // same value if they have different predicates, different values for any
    // of the four respective threshold levels, or different patterns.

bsl::ostream& operator<<(bsl::ostream& output, const Rule& rule);
    // Write the value of the specified 'rule' to the specified 'output'
    // stream.  Return the specified 'output' stream.

// ============================================================================
//                              INLINE DEFINITIONS
// ============================================================================

                       // ----------
                       // class Rule
                       // ----------

// CREATORS
inline
Rule::Rule(bslma::Allocator *basicAllocator)
: d_pattern("", basicAllocator)
, d_thresholds(0, 0, 0, 0)
, d_predicateSet(basicAllocator)
, d_hashValue(-1)
, d_hashSize(0)
{
}

inline
Rule::Rule(const bslstl::StringRef&  pattern,
           int                       recordLevel,
           int                       passLevel,
           int                       triggerLevel,
           int                       triggerAllLevel,
           bslma::Allocator         *basicAllocator)
: d_pattern(pattern.data(), pattern.length(), basicAllocator)
, d_thresholds(recordLevel, passLevel, triggerLevel, triggerAllLevel)
, d_predicateSet(basicAllocator)
, d_hashValue(-1)
, d_hashSize(0)
{
}

inline
Rule::Rule(const Rule&       original,
           bslma::Allocator *basicAllocator)
: d_pattern(original.d_pattern, basicAllocator)
, d_thresholds(original.d_thresholds)
, d_predicateSet(original.d_predicateSet)
, d_hashValue(original.d_hashValue)
, d_hashSize(0)
{
}

inline
Rule::~Rule()
{
}

// MANIPULATORS
inline
int Rule::addPredicate(const Predicate& value)
{
    d_hashValue = -1;
    return d_predicateSet.addPredicate(value);
}

inline
int Rule::removePredicate(const Predicate& value)
{
    d_hashValue = -1;
    return d_predicateSet.removePredicate(value);
}

inline
void Rule::removeAllPredicates()
{
    d_hashValue = -1;
    d_predicateSet.removeAllPredicates();
}

inline
int Rule::setLevels(int recordLevel,
                    int passLevel,
                    int triggerLevel,
                    int triggerAllLevel)
{
    d_hashValue = -1;
    return d_thresholds.setLevels(recordLevel,
                                  passLevel,
                                  triggerLevel,
                                  triggerAllLevel);
}

inline
void Rule::setPattern(const bslstl::StringRef& value)
{
    d_hashValue = -1;
    d_pattern.assign(value.data(), value.length());
}

// ACCESSORS
inline
bool Rule::evaluate(const AttributeContainerList& containerList) const
{
    return d_predicateSet.evaluate(containerList);
}

inline
int Rule::numPredicates() const
{
    return d_predicateSet.numPredicates();
}

inline
bool Rule::hasPredicate(const Predicate& value) const
{
    return d_predicateSet.isMember(value);
}
inline
PredicateSet::const_iterator Rule::begin() const
{
    return d_predicateSet.begin();
}

inline
PredicateSet::const_iterator Rule::end() const
{
    return d_predicateSet.end();
}

inline
int Rule::recordLevel() const
{
    return d_thresholds.recordLevel();
}

inline
int Rule::passLevel() const
{
    return d_thresholds.passLevel();
}

inline
int Rule::triggerLevel() const
{
    return d_thresholds.triggerLevel();
}

inline
int Rule::triggerAllLevel() const
{
    return d_thresholds.triggerAllLevel();
}

inline
const char *Rule::pattern() const
{
    return d_pattern.c_str();
}

inline
bool Rule::isMatch(const char *inputString) const
{
    return PatternUtil::isMatch(inputString, d_pattern.c_str());
}

}  // close package namespace

// FREE OPERATORS
inline
bool ball::operator==(const Rule& lhs, const Rule& rhs)
{
    if (lhs.d_hashValue > 0
     && rhs.d_hashValue > 0
     && lhs.d_hashSize  == rhs.d_hashSize
     && lhs.d_hashValue != rhs.d_hashValue) {
        return false;                                                 // RETURN
    }

    return lhs.d_pattern      == rhs.d_pattern
        && lhs.d_thresholds   == rhs.d_thresholds
        && lhs.d_predicateSet == rhs.d_predicateSet;
}

inline
bool ball::operator!=(const Rule& lhs, const Rule& rhs)
{
    return !(lhs == rhs);
}

inline
bsl::ostream& ball::operator<<(bsl::ostream& output, const Rule& rule)
{
    return rule.print(output, 0, -1);
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
