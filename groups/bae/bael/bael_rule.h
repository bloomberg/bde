// bael_rule.h                                                        -*-C++-*-
#ifndef INCLUDED_BAEL_RULE
#define INCLUDED_BAEL_RULE

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide an object having a pattern, thresholds, and predicates.
//
//@CLASSES:
//    bael_Rule: a pattern, thresholds, and predicate sets
//
//@SEE_ALSO: bael_ruleset
//
//@AUTHOR: Gang Chen (gchen20)
//
//@DESCRIPTION: This component implements a rule object that consists of a
// pattern, four threshold levels, and a set of predicates.  The pattern
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
// all 'bael_Severity::OFF' except the 'pass-through' level.  A 'pass-through'
// level of 'bael_Severity::INFO' indicates that whenever the rule is active
// and the severity is equal to or exceeds 'bael_Severity::INFO', log records
// will be passed to the observer:
//..
//  bael_Rule rule("WEEKEND*",             // pattern
//                 bael_Severity::OFF,     // record level
//                 bael_Severity::INFO,    // pass-through level
//                 bael_Severity::OFF,     // trigger level
//                 bael_Severity::OFF);    // triggerAll level
//..
// Create some predicates and then add one to the rule:
//..
//  bael_Predicate p1("uuid", 4044457);
//  bael_Predicate p2("name", "Gang Chen");
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
//  assert(bael_Severity::OFF  == rule.recordLevel());
//  assert(bael_Severity::INFO == rule.passLevel());
//  assert(bael_Severity::OFF  == rule.triggerLevel());
//  assert(bael_Severity::OFF  == rule.triggerAllLevel());
//
//  rule.setLevels(bael_Severity::INFO,
//                 bael_Severity::OFF,
//                 bael_Severity::INFO,
//                 bael_Severity::INFO);
//
//  assert(bael_Severity::INFO == rule.recordLevel());
//  assert(bael_Severity::OFF  == rule.passLevel());
//  assert(bael_Severity::INFO == rule.triggerLevel());
//  assert(bael_Severity::INFO == rule.triggerAllLevel());
//..

#ifndef INCLUDED_BAESCM_VERSION
#include <baescm_version.h>
#endif

#ifndef INCLUDED_BAEL_PATTERNUTIL
#include <bael_patternutil.h>
#endif

#ifndef INCLUDED_BAEL_PREDICATESET
#include <bael_predicateset.h>
#endif

#ifndef INCLUDED_BAEL_THRESHOLDAGGREGATE
#include <bael_thresholdaggregate.h>
#endif

#ifndef INCLUDED_BDEUT_STRINGREF
#include <bdeut_stringref.h>
#endif

#ifndef INCLUDED_BDEX_INSTREAMFUNCTIONS
#include <bdex_instreamfunctions.h>
#endif

#ifndef INCLUDED_BDEX_OUTSTREAMFUNCTIONS
#include <bdex_outstreamfunctions.h>
#endif

#ifndef INCLUDED_BSL_STRING
#include <bsl_string.h>
#endif

namespace BloombergLP {

class bslma_Allocator;

class bael_AttributeContainerList;

                       // ===============
                       // class bael_Rule
                       // ===============

class bael_Rule {
    // This class defines a value-semantic object that holds a pattern, four
    // threshold levels, and a predicate set.  For each of these fields there
    // is an accessor for obtaining the field value and a manipulator for
    // changing that value.  There are a few methods as well for directly
    // adding/removing/searching predicates.
    //
    // Additionally, this class supports a complete set of *value* *semantic*
    // operations, including copy construction, assignment and equality
    // comparison, 'ostream' printing, and 'bdex' serialization.  A precise
    // operational definition of when two instances have the same value can be
    // found in the description of 'operator==' for the class.  This class is
    // *exception* *neutral* with no guarantee of rollback: If an exception is
    // thrown during the invocation of a method on a pre-existing instance, the
    // object is left in a valid state, but its value is undefined.  In no
    // event is memory leaked.  Finally, *aliasing* (e.g., using all or part of
    // an object as both source and destination) is supported in all cases.

    // DATA
    bsl::string             d_pattern;       // the pattern for the name of
                                             // categories to which this rule
                                             // will become relevant

    bael_ThresholdAggregate d_thresholds;    // an aggregate of four threshold
                                             // levels

    bael_PredicateSet       d_predicateSet;  // set of predicates

    mutable int             d_hashValue;     // the cached hash value; <0 means
                                             // it's invalid

    mutable int             d_hashSize;      // the number of slots from which
                                             // 'd_hashValue' was calculated

    // FRIENDS
    friend bool operator==(const bael_Rule&, const bael_Rule&);
    friend bool operator!=(const bael_Rule&, const bael_Rule&);
    friend bsl::ostream& operator<<(bsl::ostream&, const bael_Rule&);

  public:
    // CLASS METHODS
    static int hash(const bael_Rule& rule, int size);
        // Return a hash value calculated from the specified 'rule' using the
        // specified 'size' as the number of slots.  The value returned is
        // guaranteed to be in the range [0 .. size - 1].

    static int maxSupportedBdexVersion();
        // Return the most current 'bdex' streaming version number supported
        // by this class.  (See the 'bde' package-group-level documentation
        // for more information on 'bdex' streaming of value-semantic types
        // and containers.)

    // CREATORS
    explicit bael_Rule(bslma_Allocator  *basicAllocator = 0);
        // Create a 'bael_Rule' object whose pattern is an empty string and
        // whose thresholds levels are all 0.  Optionally specify a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator will be used.  Note that
        // a newly created 'bael_Rule' object does not have any predicates.

    // CREATORS
    bael_Rule(const bdeut_StringRef& pattern,
              int                    recordLevel,
              int                    passLevel,
              int                    triggerLevel,
              int                    triggerAllLevel,
              bslma_Allocator       *basicAllocator = 0);
        // Create a 'bael_Rule' object whose pattern is the specified
        // 'pattern' and whose thresholds levels are the specified
        // 'recordLevel', 'passLevel', 'triggerLevel', and 'triggerAllLevel'
        // respectively.  Optionally specify a 'basicAllocator' used to supply
        // memory.  If 'basicAllocator' is 0, the currently installed default
        // allocator will be used.  The behavior is undefined unless each of
        // the four threshold level values is not in the range [0 .. 255].
        // Note that a newly created 'bael_Rule' object does not have any
        // predicates.

    bael_Rule(const bael_Rule&  original,
              bslma_Allocator  *basicAllocator = 0);
        // Create a 'bael_Rule' object that has the same value as that of the
        // specified 'original' object.  Optionally specify a 'basicAllocator'
        // used to supply memory.  If 'basicAllocator' is 0, the currently
        // installed default allocator will be used.

    ~bael_Rule();
        // Destroy this object.

    // MANIPULATORS
    bael_Rule& operator=(const bael_Rule& rhs);
        // Assign to this object the value of the specified 'rhs' object.

    int addPredicate(const bael_Predicate& value);
        // Add a predicate having the specified 'value' to this object.
        // Return 1 on success and 0 if a predicate having the same value
        // already exists in this object.

    int removePredicate(const bael_Predicate& value);
        // Remove the predicate having the specified 'value' from this object.
        // Return the number of predicates being removed (i.e., 1 on success
        // and 0 if no predicate having the specified 'value' exists in this
        // object).

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

    void setPattern(const bdeut_StringRef& value);
        // Set the pattern of this object to the specified 'value'.

    template <class STREAM>
    STREAM& bdexStreamIn(STREAM& stream, int version);
        // Assign to this object the value read from the specified input
        // 'stream' using the specified 'version' format and return a
        // reference to the modifiable 'stream'.  If 'stream' is initially
        // invalid, this operation has no effect.  If 'stream' becomes invalid
        // during this operation, this object is valid, but its value is
        // undefined.  If 'version' is not supported, 'stream' is marked
        // invalid and this object is unaltered.  Note that no version is read
        // from 'stream'.

    // ACCESSORS
    bool evaluate(const bael_AttributeContainerList& containerList) const;
        // Return 'true' if for every predicate maintained by this object, an
        // attribute with the same name and value exists in the specified
        // 'containerList'; and 'false' otherwise.

    int numPredicates() const;
        // Return the number of predicates in by this object.

    bool hasPredicate(const bael_Predicate& value) const;
        // Return 'true' if the predicate having specified 'value' exists in
        // this object, and 'false' otherwise.

    bael_PredicateSet::const_iterator begin() const;
        // Return an iterator pointing at the first member of the predicate
        // set maintained by this object.

    bael_PredicateSet::const_iterator end() const;
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
        // documentation associated with the 'bael_PatternUtil::isMatch'
        // function.)

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

    template <class STREAM>
    STREAM& bdexStreamOut(STREAM& stream, int version) const;
        // Write this value to the specified output 'stream' using the
        // specified 'version' format and return a reference to the modifiable
        // 'stream'.  If 'version' is not supported, 'stream' is unmodified.
        // Note that 'version' is not written to 'stream'.
};

// FREE OPERATORS
bool operator==(const bael_Rule& lhs, const bael_Rule& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects have the same
    // value, and 'false' otherwise.  Two 'bael_Rule' objects have the same
    // value if they have the same predicate, the same four respective
    // threshold levels, and the same pattern.

bool operator!=(const bael_Rule& lhs, const bael_Rule& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects do not have the
    // same value, and 'false' otherwise.  Two 'bael_Rule' objects do not have
    // the same value if they have different predicates, different values for
    // any of the four respective threshold levels, or different patterns.

bsl::ostream& operator<<(bsl::ostream& output, const bael_Rule& rule);
    // Write the value of the specified 'rule' to the specified 'output'
    // stream.  Return the specified 'output' stream.

// ===========================================================================
//                        INLINE FUNCTION DEFINITIONS
// ===========================================================================

                       // ---------------
                       // class bael_Rule
                       // ---------------

// CLASS METHODS
inline
int bael_Rule::maxSupportedBdexVersion()
{
    return 1;  // Required by BDE policy; versions start at 1.
}

// CREATORS
inline
bael_Rule::bael_Rule(bslma_Allocator *basicAllocator)
: d_pattern("", basicAllocator)
, d_thresholds(0, 0, 0, 0)
, d_predicateSet(basicAllocator)
, d_hashValue(-1)
, d_hashSize(0)
{
}

inline
bael_Rule::bael_Rule(const bdeut_StringRef& pattern,
                     int                    recordLevel,
                     int                    passLevel,
                     int                    triggerLevel,
                     int                    triggerAllLevel,
                     bslma_Allocator       *basicAllocator)
: d_pattern(pattern.data(), pattern.length(), basicAllocator)
, d_thresholds(recordLevel, passLevel, triggerLevel, triggerAllLevel)
, d_predicateSet(basicAllocator)
, d_hashValue(-1)
, d_hashSize(0)
{
}

inline
bael_Rule::bael_Rule(const bael_Rule&  original,
                     bslma_Allocator  *basicAllocator)
: d_pattern(original.d_pattern, basicAllocator)
, d_thresholds(original.d_thresholds)
, d_predicateSet(original.d_predicateSet)
, d_hashValue(original.d_hashValue)
, d_hashSize(0)
{
}

inline
bael_Rule::~bael_Rule()
{
}

// MANIPULATORS
inline
int bael_Rule::addPredicate(const bael_Predicate& value)
{
    d_hashValue = -1;
    return d_predicateSet.addPredicate(value);
}

inline
int bael_Rule::removePredicate(const bael_Predicate& value)
{
    d_hashValue = -1;
    return d_predicateSet.removePredicate(value);
}

inline
void bael_Rule::removeAllPredicates()
{
    d_hashValue = -1;
    d_predicateSet.removeAllPredicates();
}

inline
int bael_Rule::setLevels(int recordLevel,
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
void bael_Rule::setPattern(const bdeut_StringRef& pattern)
{
    d_hashValue = -1;
    d_pattern.assign(pattern.data(), pattern.length());
}

template <class STREAM>
STREAM&  bael_Rule::bdexStreamIn(STREAM& stream, int version)
{
    if (stream) {
        switch (version) {
          case 1: {
            d_hashValue = -1;
            bdex_InStreamFunctions::streamIn(stream, d_pattern, 0);
            if (!stream) {
                return stream;                                        // RETURN
            }

            bdex_InStreamFunctions::streamIn(stream, d_thresholds, 1);
            if (!stream) {
                return stream;                                        // RETURN
            }

            bdex_InStreamFunctions::streamIn(stream, d_predicateSet, 1);
            if (!stream) {
                return stream;                                        // RETURN
            }
          } break;
          default: {
            stream.invalidate();
          }
        }
    }
    return stream;

}

// ACCESSORS
inline
bool bael_Rule::evaluate(
                       const bael_AttributeContainerList& containerList) const
{
    return d_predicateSet.evaluate(containerList);
}

inline
int bael_Rule::numPredicates() const
{
    return d_predicateSet.numPredicates();
}

inline
bool bael_Rule::hasPredicate(const bael_Predicate& value) const
{
    return d_predicateSet.isMember(value);
}
inline
bael_PredicateSet::const_iterator bael_Rule::begin() const
{
    return d_predicateSet.begin();
}

inline
bael_PredicateSet::const_iterator bael_Rule::end() const
{
    return d_predicateSet.end();
}

inline
int bael_Rule::recordLevel() const
{
    return d_thresholds.recordLevel();
}

inline
int bael_Rule::passLevel() const
{
    return d_thresholds.passLevel();
}

inline
int bael_Rule::triggerLevel() const
{
    return d_thresholds.triggerLevel();
}

inline
int bael_Rule::triggerAllLevel() const
{
    return d_thresholds.triggerAllLevel();
}

inline
const char *bael_Rule::pattern() const
{
    return d_pattern.c_str();
}

inline
bool bael_Rule::isMatch(const char *inputString) const
{
    return bael_PatternUtil::isMatch(inputString, d_pattern.c_str());
}

template <class STREAM>
STREAM& bael_Rule::bdexStreamOut(STREAM& stream, int version) const
{
    switch (version) {
      case 1: {
        bdex_OutStreamFunctions::streamOut(stream, d_pattern, 0);
        bdex_OutStreamFunctions::streamOut(stream, d_thresholds, 1);
        bdex_OutStreamFunctions::streamOut(stream, d_predicateSet, 1);
      } break;
    }
    return stream;
}

// FREE OPERATORS
inline
bool operator==(const bael_Rule& lhs, const bael_Rule& rhs)
{
    if (lhs.d_hashValue > 0
     && rhs.d_hashValue > 0
     && lhs.d_hashSize  == rhs.d_hashSize
     && lhs.d_hashValue != rhs.d_hashValue) {
        return false;
    }

    return lhs.d_pattern      == rhs.d_pattern
        && lhs.d_thresholds   == rhs.d_thresholds
        && lhs.d_predicateSet == rhs.d_predicateSet;
}

inline
bool operator!=(const bael_Rule& lhs, const bael_Rule& rhs)
{
    return !(lhs == rhs);
}

inline
bsl::ostream& operator<<(bsl::ostream& output, const bael_Rule& rule)
{
    return rule.print(output, 0, -1);
}

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2007
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
