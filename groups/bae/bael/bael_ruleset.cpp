// bael_ruleset.cpp                 -*-C++-*-
#include <bael_ruleset.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bael_ruleset_cpp,"$Id$ $CSID$")

#include <bdeu_bitstringutil.h>

#include <bsls_assert.h>
#include <bsl_functional.h>

namespace BloombergLP {

// CLASS MEMBER
int bael_RuleSet::RuleHash::s_hashtableSize = INT_MAX;

// CLASS METHODS
void bael_RuleSet::printMask(bsl::ostream& stream,
                             MaskType      mask,
                             int           level,
                             int           spacesPerLevel)
{
    bdeu_BitstringUtil::print(stream,
                              (int *)&mask,
                              sizeof(MaskType) * 8,
                              level,
                              spacesPerLevel);
}

// CREATORS
bael_RuleSet::bael_RuleSet(bslma_Allocator *basicAllocator)
: d_ruleHashtable(maxNumRules(),              // initial size
                  RuleHash(),                 // hash functor
                  bsl::equal_to<bael_Rule>(), // equal functor
                  basicAllocator)
, d_ruleAddresses(basicAllocator)
, d_freeRuleIds(basicAllocator)
, d_numPredicates(0)
{
    for (int i = 0; i < maxNumRules(); ++i)
    {
        d_ruleAddresses.push_back(0);
        d_freeRuleIds.push_back(i);
    }
}

bael_RuleSet::bael_RuleSet(const bael_RuleSet&  original,
                           bslma_Allocator     *basicAllocator)
: d_ruleHashtable(maxNumRules(),              // initial size
                  RuleHash(),                 // hash functor
                  bsl::equal_to<bael_Rule>(), // equal functor
                  basicAllocator)
, d_ruleAddresses(basicAllocator)
, d_freeRuleIds(basicAllocator)
{
    for (int i = 0; i < maxNumRules(); ++i)
    {
        d_ruleAddresses.push_back(0);
        d_freeRuleIds.push_back(i);
    }
    addRules(original);
}

bael_RuleSet::~bael_RuleSet()
{
}


// MANIPULATORS
int bael_RuleSet::addRule(const bael_Rule& value)
{
    if (d_ruleHashtable.find(value) != d_ruleHashtable.end()) {
        return -1;                                                    // RETURN
    }
    if ((int)d_ruleHashtable.size() >= maxNumRules()) {
        return -2;                                                    // RETURN
    }

    HashtableType::const_iterator iter = d_ruleHashtable.insert(value).first;
    int ruleId = d_freeRuleIds.back();
    d_freeRuleIds.pop_back();
    d_ruleAddresses[ruleId] = &*iter;
    d_numPredicates += value.numPredicates();
    return ruleId;
}

int bael_RuleSet::addRules(const bael_RuleSet& rules)
{
    int count = 0;
    for (int i = 0; i < rules.maxNumRules(); ++i) {
        const bael_Rule *rule = rules.getRuleById(i);
        if (rule) {
            count += addRule(*rule) >= 0;
        }
    }

    return count;
}

int bael_RuleSet::removeRule(const bael_Rule& value)
{
    int id = ruleId(value);
    if (id < 0) {
        return 0;                                                     // RETURN
    }
    return removeRuleById(id);
}

int bael_RuleSet::removeRuleById(int id)
{
    BSLS_ASSERT(0 <= id);
    BSLS_ASSERT(id < maxNumRules());

    const bael_Rule *rule = d_ruleAddresses[id];
    if (!rule) {
        return 0;                                                     // RETURN
    }

    d_numPredicates -= rule->numPredicates();

    // Note that removing 'iter' from 'd_ruleHashTable' invalidates 'rule'.
    HashtableType::iterator iter = d_ruleHashtable.find(*rule);
    BSLS_ASSERT(iter != d_ruleHashtable.end());
    d_ruleHashtable.erase(iter);

    d_ruleAddresses[id] = 0;
    d_freeRuleIds.push_back(id);

    return 1;
}

int bael_RuleSet::removeRules(const bael_RuleSet& rules)
{
    int count = 0;
    for (int i = 0; i < rules.maxNumRules(); ++i) {
        const bael_Rule *rule = rules.getRuleById(i);
        if (rule) {
            int id = ruleId(*rule);
            if (id >= 0) {
                count += removeRuleById(id);
            }
        }
    }

    return count;
}

void bael_RuleSet::removeAllRules()
{
    d_ruleAddresses.clear();
    d_ruleHashtable.clear();
    d_freeRuleIds.clear();

    for(int i = 0; i < maxNumRules(); ++i)
    {
        d_ruleAddresses.push_back(0);
        d_freeRuleIds.push_back(i);
    }
}

bael_RuleSet& bael_RuleSet::operator=(const bael_RuleSet& rhs)
{
    if (this != &rhs) {
        removeAllRules();
        addRules(rhs);
    }

    return *this;
}

// ACCESSOR
int bael_RuleSet::ruleId(const bael_Rule& value) const
{
    HashtableType::const_iterator iter = d_ruleHashtable.find(value);

    if (iter == d_ruleHashtable.end()) {
        return -1;                                                    // RETURN
    }

    const bael_Rule *rule = &*iter;

    for (unsigned int i = 0; i < d_ruleAddresses.size(); ++i) {
        if (d_ruleAddresses[i] == rule) {
            return i;
        }
    }

    return -1;
}

bsl::ostream& bael_RuleSet::print(bsl::ostream& stream,
                                  int           level,
                                  int           spacesPerLevel) const
{
    const char NL = spacesPerLevel >= 0 ? '\n' : ' ';

    if (level < 0) {
        level = -level;
    }
    else {
        bdeu_Print::indent(stream, level, spacesPerLevel);
    }

    stream << '{' << NL;

    for (int i = 0; i < maxNumRules(); ++i) {
        const bael_Rule *rule = getRuleById(i);
        if (rule) {
            rule->print(stream, level + 1, spacesPerLevel);
        }
    }

    bdeu_Print::indent(stream, level, spacesPerLevel);
    stream << '}' << NL;

    return stream;
}

// FREE OPERATORS
bool operator==(const bael_RuleSet& lhs, const bael_RuleSet& rhs)
{
    if (lhs.numRules() != rhs.numRules()) {
        return false;                                                 // RETURN
    }

    for (int i = 0; i < lhs.maxNumRules(); ++i) {
        const bael_Rule *rule = lhs.getRuleById(i);
        if (rule && 0 > rhs.ruleId(*rule) ) {
            return false;                                             // RETURN
        }
    }
    return true;
}

bool operator!=(const bael_RuleSet& lhs, const bael_RuleSet& rhs)
{
    return !(lhs == rhs);
}

} // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2007
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
