// bael_ruleset.h                                                     -*-C++-*-
#ifndef INCLUDED_BAEL_RULESET
#define INCLUDED_BAEL_RULESET

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a set of unique rules.
//
//@CLASSES:
//    bael_RuleSet: container for unique rules
//
//@SEE_ALSO: bael_rule
//
//@AUTHOR: Gang Chen (gchen20)
//
//@DESCRIPTION: This component provides a value-semantic container for
// storage and efficient retrieval of 'bael_Rule' objects.
//
///Thread Safety
///-------------
// 'bael_RuleSet' is *not* thread-safe in that multiple threads attempting to
// concurrently modify the same 'bael_RuleSet' object will leave the object in
// an undefined state.  To ensure thread-safety, concurrent accesses to a
// 'bael_RuleSet' must be serialized by a mutex.
//
///Usage
///-----
// The following code fragments illustrate how to use a rule set.
//
// We first create a rule whose pattern is 'WEEKEND*' and whose threshold
// levels are all 'bael_Severity::BAEL_OFF' except the 'pass-through' level.  A
// 'pass-through' level of 'bael_Severity::BAEL_INFO' indicates that whenever
// the rule is active and the severity equals or exceeds
// 'bael_Severity::BAEL_INFO', log records will be passed to the observer:
//..
//  bael_Rule rule1("WEEKEND*",                  // pattern
//                  bael_Severity::BAEL_OFF,     // record level
//                  bael_Severity::BAEL_INFO,    // pass-through level
//                  bael_Severity::BAEL_OFF,     // trigger level
//                  bael_Severity::BAEL_OFF);    // triggerAll level
//..
// Next, we create another rule having a different pattern, but the same
// threshold levels:
//..
//  bael_Rule rule2("WEEKDAY*",                  // pattern
//                  bael_Severity::BAEL_OFF,     // record level
//                  bael_Severity::BAEL_INFO,    // pass-through level
//                  bael_Severity::BAEL_OFF,     // trigger level
//                  bael_Severity::BAEL_OFF);    // triggerAll level
//..
// We then create a 'bael_RuleSet' object, add the two rules, and verify that
// rules were added correctly:
//..
//  bael_RuleSet ruleSet;
//  assert(1 == ruleSet.addRule(rule1));
//  assert(1 == ruleSet.addRule(rule2));
//  assert(2 == ruleSet.numRules());
//..
// Duplicate rules cannot be added:
//..
//  assert(0 == ruleSet.addRule(rule1));
//  assert(0 == ruleSet.addRule(rule2));
//  assert(2 == ruleSet.numRules());
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

#ifndef INCLUDED_BAESCM_VERSION
#include <baescm_version.h>
#endif

#ifndef INCLUDED_BAEL_RULE
#include <bael_rule.h>
#endif

#ifndef INCLUDED_BDEX_INSTREAMFUNCTIONS
#include <bdex_instreamfunctions.h>
#endif

#ifndef INCLUDED_BDEX_OUTSTREAMFUNCTIONS
#include <bdex_outstreamfunctions.h>
#endif

#ifndef INCLUDED_BSLS_PLATFORMUTIL
#include <bsls_platformutil.h>
#endif

#ifndef INCLUDED_BSL_VECTOR
#include <bsl_vector.h>
#endif

namespace BloombergLP {

class bslma_Allocator;

                      // ==================
                      // class bael_RuleSet
                      // ==================

class bael_RuleSet {
    // This class manages a set of unique rule values.  Rules may be added to
    // or removed from the set; however, rules having duplicate values will
    // not be added.  For the definition of two rules having the same value,
    // please refer to the function-level documentation associated with the
    // 'bael_Rule::operator==' function.

  public:
    // PUBLIC TYPES
    typedef unsigned int MaskType;
        // 'MaskType' is an alias for the fundamental integral type used to
        // indicate rule subsets compactly.

    enum {
        BAEL_MAX_NUM_RULES = 8 * sizeof(MaskType)
           // The maximum number of rules managed by this object.
#if !defined(BSL_LEGACY) || 1 == BSL_LEGACY
      , MAX_NUM_RULES = BAEL_MAX_NUM_RULES
#endif
    };

  private:
    // PRIVATE TYPES
    struct RuleHash
        // hash functor for 'bael_Rule'
    {
      private:
        static int s_hashtableSize;              // the default hashtable size
      public:
        int operator()(const bael_Rule& rule) const
            // Return the hash value of the specified 'rule'.
        {
            return bael_Rule::hash(rule, s_hashtableSize);
        }
    };

    typedef bsl::hash_set<bael_Rule, RuleHash> HashtableType;

    // DATA
    HashtableType              d_ruleHashtable;  // the hash table that
                                                 // manages all the rules
                                                 // maintained by this rule
                                                 // set
    bsl::vector<const bael_Rule *>
                               d_ruleAddresses;  // secondary map between
                                                 // ids and the addresses
                                                 // of rules

    bsl::vector<int>           d_freeRuleIds;    // rule Ids that are not being
                                                 // used

    int                        d_numPredicates;  // total number of predicates

    // FRIENDS
    friend bool operator==(const bael_RuleSet&,    const bael_RuleSet&);
    friend bool operator!=(const bael_RuleSet&,    const bael_RuleSet&);
    friend bsl::ostream& operator<<(bsl::ostream&, const bael_RuleSet&);

  public:
    // CLASS METHODS
    static int maxNumRules();
        // Return the maximum number of rules that can be simultaneously
        // maintained by this object.

    static int maxSupportedBdexVersion();
        // Return the most current 'bdex' streaming version number supported by
        // this class.  (See the package-group-level documentation for more
        // information on 'bdex' streaming of value-semantic types and
        // containers.)

    static void printMask(bsl::ostream& stream,
                          MaskType      mask,
                          int           level = 0,
                          int           spacesPerLevel = 0);
        // Format the specified 'mask' to the specified output 'stream' at the
        // optionally specified indentation 'level' and return a reference to
        // the modifiable 'stream'.  If 'level' is specified, optionally
        // specify 'spacesPerLevel', the number of spaces per indentation
        // level for this and all of its nested objects.  Each line is
        // indented by the absolute value of 'level * spacesPerLevel'.  If
        // 'level' is negative, suppress indentation of the first line.  If
        // 'spacesPerLevel' is negative, suppress line breaks and format the
        // entire output on one line.  If 'stream' is initially invalid, this
        // operation has no effect.

    // CREATORS
    explicit bael_RuleSet(bslma_Allocator *basicAllocator = 0);
        // Create an empty rule set.  Optionally specify a 'basicAllocator'
        // used to supply memory.  If 'basicAllocator' is 0, the currently
        // installed default allocator will be used.

    bael_RuleSet(const bael_RuleSet&  original,
                 bslma_Allocator     *basicAllocator = 0);
        // Create a 'bael_RuleSet' object having the same value as that of the
        // specified 'original' object.  Optionally specify a 'basicAllocator'
        // used to supply memory.  If 'basicAllocator' is 0, the currently
        // installed default allocator will be used.

    ~bael_RuleSet();
        // Destroy this rule set.

    // MANIPULATOR
    int addRule(const bael_Rule& value);
        // Create a new 'bael_Rule' object having the specified 'value'.
        // Return the non-negative id of this non-modifiable object on
        // success, and a negative value otherwise.  A return value of -1
        // indicates that another rule having this value already exists.  A
        // return value of -2 indicates that the maximum number of rules for
        // this rule set has been reached.

    int addRules(const bael_RuleSet& rules);
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
        // unless '0 <= id < BAEL_MAX_NUM_RULES'.

    int removeRule(const bael_Rule& value);
        // Remove the rule having the specified 'value' from this rule set.
        // Return the number of rules removed (i.e., 1 on success and 0 if
        // there is no such a rule).

    int removeRules(const bael_RuleSet& rules);
        // Remove each rule in the specified 'rules' from this rule set.
        // Return the number of rules removed.

    void removeAllRules();
        // Remove every rule in the rule set maintained by this object.

    bael_RuleSet& operator=(const bael_RuleSet& rhs);
        // Assign to this object the value of the specified 'rhs' object.

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
    int ruleId(const bael_Rule& value) const;
       // Return the id of the rule having the specified 'value' if such a
       // rule exists, and a negative value otherwise.  Note that if there are
       // multiple rules having the specified 'value', the id of the first
       // one found will be returned and the order in which rules are searched
       // is implementation dependent.

    const bael_Rule *getRuleById(int id) const;
        // Return the address of the rule having the specified 'id' if such a
        // rule exists, and 0 otherwise.  The behavior is undefined unless
        // '0 <= id < maxNumRules()'.  Note that rules may be assigned
        // non-sequential identifiers, and that there may be a valid rule whose
        // identifier is greater than 'numRules()' (i.e., valid rules may
        // appear anywhere in the range '0 <= id < maxNumRules()').

    int numRules() const;
        // Return the number of unique rules maintained in this 'bael_RuleSet'
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

    template <class STREAM>
    STREAM& bdexStreamOut(STREAM& stream, int version) const;
        // Write this value to the specified output 'stream' using the
        // specified 'version' format and return a reference to the modifiable
        // 'stream'.  If 'version' is not supported, 'stream' is unmodified.
        // Note that 'version' is not written to 'stream'.
};

// FREE OPERATORS
bool operator==(const bael_RuleSet& lhs, const bael_RuleSet& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' rule sets have the same
    // value, and 'false' otherwise.  Two rule sets have the same value if
    // every rule that exists in one rule set also exists in the other.

bool operator!=(const bael_RuleSet& lhs, const bael_RuleSet& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' rule sets do not have the
    // same value, and 'false' otherwise.  Two rule sets do not have the same
    // value if there is at least one rule that exists in one rule but does not
    // exist in the other.

bsl::ostream& operator<<(bsl::ostream& output, const bael_RuleSet& rules);
    // Write the value of the specified 'rules' to the specified 'output'
    // stream.  Return the specified 'output' stream

// ===========================================================================
//                        INLINE FUNCTION DEFINITIONS
// ===========================================================================

                      // ------------------
                      // class bael_RuleSet
                      // ------------------

// CLASS METHODS
inline
int bael_RuleSet::maxNumRules()
{
    return BAEL_MAX_NUM_RULES;
}

inline
int bael_RuleSet::maxSupportedBdexVersion()
{
    return 1;  // Required by BDE policy; versions start at 1.
}

// MANIPULATORS
template <class STREAM>
STREAM&  bael_RuleSet::bdexStreamIn(STREAM& stream, int version)
{
    if (stream) {
        switch (version) {
          case 1: {
            int numRules = 0;
            bdex_InStreamFunctions::streamIn(stream, numRules, 0);
            if (!stream ) {
                return stream;                                        // RETURN
            }

            if (0 > numRules) {
                stream.invalidate();
                return stream;                                        // RETURN
            }

            removeAllRules();

            for (int i = 0; i < numRules; ++i) {
                bael_Rule rule("", 0, 0, 0, 0);
                bdex_InStreamFunctions::streamIn(stream, rule, 1);
                if (!stream) {
                    return stream;                                    // RETURN
                }
                addRule(rule);
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
const bael_Rule *bael_RuleSet::getRuleById(int id) const
{
    return d_ruleAddresses[id];
}

inline
int bael_RuleSet::numRules() const
{
    return static_cast<int>(d_ruleHashtable.size());
}

inline
int bael_RuleSet::numPredicates() const
{
    return d_numPredicates;
}

template <class STREAM>
STREAM& bael_RuleSet::bdexStreamOut(STREAM& stream, int version) const
{
    switch (version) {
      case 1: {
        bdex_OutStreamFunctions::streamOut(stream,
                                           (int)d_ruleHashtable.size(),
                                           0);
        for (int i = 0; i < maxNumRules(); ++i) {
            const bael_Rule *rule = getRuleById(i);
            if (rule) {
                bdex_OutStreamFunctions::streamOut(stream, *rule, 1);
            }
        }
      } break;
    }
    return stream;
}

inline
bsl::ostream& operator<<(bsl::ostream& output, const bael_RuleSet& rules)
{
    return rules.print(output, 0, -1);
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
