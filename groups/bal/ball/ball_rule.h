// ball_rule.h                                                        -*-C++-*-
#ifndef INCLUDED_BALL_RULE
#define INCLUDED_BALL_RULE

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide an object having a pattern, thresholds, and attributes.
//
//@CLASSES:
//  ball::Rule: a pattern, thresholds, and attribute sets
//
//@SEE_ALSO: ball_ruleset
//
//@DESCRIPTION: This component implements a type, `ball::Rule`, that consists
// of a pattern, four threshold levels, and a set of attributes.  The pattern
// indicates the names of the categories for which the rule will become
// relevant.  The four threshold levels determine what actions will be
// performed on log records when their severity level equals or exceeds any of
// these threshold levels.  The attribute set is a collection of unique
// attribute name/value pairs.
//
// Note that multiple attributes with the same name are permitted so long as
// they correspond to different values.
//
// This component participates in the implementation of "Rule-Based Logging".
// For more information on how to use that feature, please see the package
// level documentation and usage examples for "Rule-Based Logging".
//
///Usage
///-----
// The following code fragments illustrate how to create a rule and add
// attributes.
//
// We create a rule whose pattern is `WEEKEND*` and whose threshold levels are
// all `ball::Severity::e_OFF` except the `pass-through` level.  A
// `pass-through` level of `ball::Severity::e_INFO` indicates that whenever the
// rule is active and the severity is equal to or exceeds
// `ball::Severity::e_INFO`, log records will be passed to the observer:
// ```
// ball::Rule rule("WEEKEND*",              // pattern
//                 ball::Severity::e_OFF,   // record level
//                 ball::Severity::e_INFO,  // pass-through level
//                 ball::Severity::e_OFF,   // trigger level
//                 ball::Severity::e_OFF);  // triggerAll level
// ```
// Create some attributes and then add one to the rule:
// ```
// ball::ManagedAttribute p1("myLib.uuid", 4044457);
// ball::ManagedAttribute p2("myLib.name", "John Smith");
// rule.addAttribute(p1);
// ```
// Attributes can be looked up by the `hasAttribute` method:
// ```
// assert(true  == rule.hasAttribute(p1));
// assert(false == rule.hasAttribute(p2));
// ```
// We then add the other attribute:
// ```
// rule.addAttribute(p2);
// assert(true  == rule.hasAttribute(p2));
// ```
// Attributes can also be removed from the rule by the `removeAttribute`
// method:
// ```
// rule.removeAttribute(p1);
// assert(false == rule.hasAttribute(p1));
// assert(true  == rule.hasAttribute(p2));
// ```
// The pattern of a rule can be changed by the `setPattern` method:
// ```
// assert(0 == strcmp(rule.pattern(), "WEEKEND*"));
//
// rule.setPattern("WEEKDAY*");
// assert(0 == strcmp(rule.pattern(), "WEEKDAY*"));
// ```
// The threshold levels of a rule can also be modified by the `setLevels`
// method:
// ```
// assert(ball::Severity::e_OFF  == rule.recordLevel());
// assert(ball::Severity::e_INFO == rule.passLevel());
// assert(ball::Severity::e_OFF  == rule.triggerLevel());
// assert(ball::Severity::e_OFF  == rule.triggerAllLevel());
//
// rule.setLevels(ball::Severity::e_INFO,
//                ball::Severity::e_OFF,
//                ball::Severity::e_INFO,
//                ball::Severity::e_INFO);
//
// assert(ball::Severity::e_INFO == rule.recordLevel());
// assert(ball::Severity::e_OFF  == rule.passLevel());
// assert(ball::Severity::e_INFO == rule.triggerLevel());
// assert(ball::Severity::e_INFO == rule.triggerAllLevel());
// ```

#include <balscm_version.h>

#include <ball_managedattribute.h>
#include <ball_managedattributeset.h>
#include <ball_patternutil.h>
#include <ball_predicate.h>
#include <ball_predicateset.h>
#include <ball_thresholdaggregate.h>

#include <bslma_allocator.h>
#include <bslma_usesbslmaallocator.h>

#include <bslmf_nestedtraitdeclaration.h>

#include <bsl_string.h>

namespace BloombergLP {
namespace ball {

class AttributeContainerList;

                        // ==========
                        // class Rule
                        // ==========

/// This class defines a value-semantic object that holds a pattern, four
/// threshold levels, and an attribute set.  For each of these fields there
/// is an accessor for obtaining the field value and a manipulator for
/// changing that value.  There are a few methods as well for directly
/// adding/removing/searching attributes.
///
/// Additionally, this class supports a complete set of *value* *semantic*
/// operations, including copy construction, assignment and equality
/// comparison, and `ostream` printing.  A precise operational definition of
/// when two instances have the same value can be found in the description
/// of `operator==` for the class.  This class is *exception* *neutral* with
/// no guarantee of rollback: If an exception is thrown during the
/// invocation of a method on a pre-existing instance, the object is left in
/// a valid state, but its value is undefined.  In no event is memory
/// leaked.  Finally, *aliasing* (e.g., using all or part of an object as
/// both source and destination) is supported in all cases.
class Rule {

    // DATA
    bsl::string         d_pattern;       // the pattern for the name of
                                         // categories to which this rule will
                                         // become relevant

    ThresholdAggregate  d_thresholds;    // an aggregate of four threshold
                                         // levels

    ManagedAttributeSet d_attributeSet;  // set of attributes

    mutable int         d_hashValue;     // cached hash value; < 0 indicates it
                                         // is invalid

    mutable int         d_hashSize;      // number of slots from which
                                         // 'd_hashValue' was calculated; 0
                                         // indicates it is invalid

    // FRIENDS
    friend bool operator==(const Rule&, const Rule&);
    friend bool operator!=(const Rule&, const Rule&);
    friend bsl::ostream& operator<<(bsl::ostream&, const Rule&);

  public:
    // CLASS METHODS

    /// Return a hash value calculated from the specified `rule` using the
    /// specified `size` as the number of slots.  The value returned is
    /// guaranteed to be in the range `[0 .. size - 1]`.  The behavior is
    /// undefined unless `0 < size`.
    static int hash(const Rule& rule, int size);

    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(Rule, bslma::UsesBslmaAllocator);

    // CREATORS

    /// Create a `Rule` object whose pattern is an empty string and whose
    /// thresholds levels are all 0.  Optionally specify a `basicAllocator`
    /// used to supply memory.  If `basicAllocator` is 0, the currently
    /// installed default allocator will be used.  Note that a newly created
    /// `Rule` object does not have any attributes.
    explicit Rule(bslma::Allocator *basicAllocator = 0);

    /// Create a `Rule` object whose pattern is the specified `pattern` and
    /// whose thresholds levels are the specified `recordLevel`,
    /// `passLevel`, `triggerLevel`, and `triggerAllLevel` respectively.
    /// Optionally specify a `basicAllocator` used to supply memory.  If
    /// `basicAllocator` is 0, the currently installed default allocator
    /// will be used.  The behavior is undefined unless each of the four
    /// threshold level values is not in the range [0 .. 255].  Note that a
    /// newly created `Rule` object does not have any attributes.
    Rule(const bsl::string_view&  pattern,
         int                      recordLevel,
         int                      passLevel,
         int                      triggerLevel,
         int                      triggerAllLevel,
         bslma::Allocator        *basicAllocator = 0);

    /// Create a `Rule` object that has the same value as that of the
    /// specified `original` object.  Optionally specify a `basicAllocator`
    /// used to supply memory.  If `basicAllocator` is 0, the currently
    /// installed default allocator will be used.
    Rule(const Rule& original, bslma::Allocator *basicAllocator = 0);

    /// Destroy this object.
    //! ~Rule() = default;

    // MANIPULATORS

    /// Assign to this object the value of the specified `rhs` object.
    Rule& operator=(const Rule& rhs);

    /// Add an attribute having the specified `value` to this object.
    /// Return 1 on success and 0 if an attribute having the same value
    /// already exists in this object.
    int addAttribute(const ManagedAttribute& value);

    /// @DEPRECATED: Use `addAttribute` instead.
    int addPredicate(const ManagedAttribute& value);

    /// Remove the attribute having the specified `value` from this object.
    /// Return the number of attributes being removed (i.e., 1 on success
    /// and 0 if no attribute having `value` exists in this object).
    int removeAttribute(const ManagedAttribute& value);

    /// @DEPRECATED: Use `removeAttribute` instead.
    int removePredicate(const ManagedAttribute& value);

    /// Remove all attributes from this rule.
    void removeAll();

    /// @DEPRECATED: Use `removeAll` instead.
    void removeAllPredicates();

    /// Set the threshold levels of this object to the specified
    /// `recordLevel`, `passLevel`, `triggerLevel`, and `triggerAllLevel`
    /// values, respectively, if each of the specified values is in the
    /// range [0 .. 255].  Return 0 on success, and a non-zero value
    /// otherwise (with no effect on the threshold levels of this object).
    int setLevels(int recordLevel,
                  int passLevel,
                  int triggerLevel,
                  int triggerAllLevel);

    /// Set the pattern of this object to the specified `value`.
    void setPattern(const bsl::string_view& value);

    // ACCESSORS

    /// Return `true` if for every attribute maintained by this object, an
    /// attribute with the same name and value exists in the specified
    /// `containerList`; and `false` otherwise.
    bool evaluate(const AttributeContainerList& containerList) const;

    /// Return the number of attributes managed by this object.
    int numAttributes() const;

    /// @DEPRECATED: Use `numAttributes` instead.
    int numPredicates() const;

    /// Return `true` if an attribute having specified `value` exists in
    /// this object, and `false` otherwise.
    bool hasAttribute(const ManagedAttribute& value) const;

    /// @DEPRECATED: Use `hasAttribute` instead.
    bool hasPredicate(const Predicate& value) const;

    /// Return an iterator referring to the first member of the attribute set
    /// maintained by this object.
    ManagedAttributeSet::const_iterator begin() const;

    /// Return an iterator referring to one past the last member of the
    /// attribute set maintained by this object.
    ManagedAttributeSet::const_iterator end() const;

    /// Return the record level of this object.
    int recordLevel() const;

    /// Return the pass level of this object.
    int passLevel() const;

    /// Return the trigger level of this object.
    int triggerLevel() const;

    /// Return the trigger-all level of this object.
    int triggerAllLevel() const;

    /// Return the pattern of this object.
    const char *pattern() const;

    /// Return `true` if the specified `inputString` matches the pattern of
    /// this rule, and `false` otherwise.  (For the definition of a string
    /// matching the pattern of a rule, please refer to the function-level
    /// documentation associated with the `PatternUtil::isMatch` function).
    bool isMatch(const char *inputString) const;

    /// Format this object to the specified output `stream` at the
    /// (absolute value of) the optionally specified indentation `level`
    /// and return a reference to `stream`.  If `level` is specified,
    /// optionally specify `spacesPerLevel`, the number of spaces per
    /// indentation level for this and all of its nested objects.  If
    /// `level` is negative, suppress indentation of the first line.  If
    /// `spacesPerLevel` is negative, format the entire output on one line,
    /// suppressing all but the initial indentation (as governed by
    /// `level`).  If `stream` is not valid on entry, this operation has no
    /// effect.
    bsl::ostream& print(bsl::ostream& stream,
                        int           level = 0,
                        int           spacesPerLevel = 4) const;
};

// FREE OPERATORS

/// Return `true` if the specified `lhs` and `rhs` objects have the same
/// value, and `false` otherwise.  Two `Rule` objects have the same value if
/// they have the same attributes, the same four respective threshold
/// levels, and the same pattern.
bool operator==(const Rule& lhs, const Rule& rhs);

/// Return `true` if the specified `lhs` and `rhs` objects do not have the
/// same value, and `false` otherwise.  Two `Rule` objects do not have the
/// same value if they have different attributes, different values for any
/// of the four respective threshold levels, or different patterns.
bool operator!=(const Rule& lhs, const Rule& rhs);

/// Write the value of the specified `rule` to the specified `output` stream.
/// Return the specified `output` stream.
bsl::ostream& operator<<(bsl::ostream& output, const Rule& rule);

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
, d_attributeSet(basicAllocator)
, d_hashValue(-1)
, d_hashSize(0)
{
}

inline
Rule::Rule(const bsl::string_view&   pattern,
           int                       recordLevel,
           int                       passLevel,
           int                       triggerLevel,
           int                       triggerAllLevel,
           bslma::Allocator         *basicAllocator)
: d_pattern(pattern.data(), pattern.length(), basicAllocator)
, d_thresholds(recordLevel, passLevel, triggerLevel, triggerAllLevel)
, d_attributeSet(basicAllocator)
, d_hashValue(-1)
, d_hashSize(0)
{
}

inline
Rule::Rule(const Rule& original, bslma::Allocator *basicAllocator)
: d_pattern(original.d_pattern, basicAllocator)
, d_thresholds(original.d_thresholds)
, d_attributeSet(original.d_attributeSet, basicAllocator)
, d_hashValue(original.d_hashValue)
, d_hashSize(original.d_hashSize)
{
}

// MANIPULATORS
inline
int Rule::addAttribute(const ManagedAttribute& value)
{
    d_hashValue = -1;
    return d_attributeSet.addAttribute(value);
}

inline
int Rule::addPredicate(const ManagedAttribute& value)
{
    return addAttribute(value);
}

inline
int Rule::removeAttribute(const ManagedAttribute& value)
{
    d_hashValue = -1;
    return d_attributeSet.removeAttribute(value);
}

inline
int Rule::removePredicate(const ManagedAttribute& value)
{
    return removeAttribute(value);
}

inline
void Rule::removeAll()
{
    d_hashValue = -1;
    d_attributeSet.removeAll();
}

inline
void Rule::removeAllPredicates()
{
    removeAll();
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
void Rule::setPattern(const bsl::string_view& value)
{
    d_hashValue = -1;
    d_pattern.assign(value);
}

// ACCESSORS
inline
bool Rule::evaluate(const AttributeContainerList& containerList) const
{
    return d_attributeSet.evaluate(containerList);
}

inline
int Rule::numAttributes() const
{
    return d_attributeSet.numAttributes();
}

inline
int Rule::numPredicates() const
{
    return numAttributes();
}

inline
bool Rule::hasAttribute(const ManagedAttribute& value) const
{
    return d_attributeSet.isMember(value);
}

inline
bool Rule::hasPredicate(const ManagedAttribute& value) const
{
    return hasAttribute(value);
}

inline
ManagedAttributeSet::const_iterator Rule::begin() const
{
    return d_attributeSet.begin();
}

inline
ManagedAttributeSet::const_iterator Rule::end() const
{
    return d_attributeSet.end();
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
        && lhs.d_attributeSet == rhs.d_attributeSet;
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
