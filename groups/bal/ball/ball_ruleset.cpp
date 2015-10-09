// ball_ruleset.cpp                                                   -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <ball_ruleset.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ball_ruleset_cpp,"$Id$ $CSID$")

#include <bdlb_bitstringutil.h>
#include <bdlb_bitutil.h>

#include <bslim_printer.h>
#include <bslmf_assert.h>
#include <bsls_assert.h>

#include <bsl_climits.h>
#include <bsl_cstdint.h>
#include <bsl_functional.h>

namespace BloombergLP {

// CLASS MEMBER
int ball::RuleSet::RuleHash::s_hashtableSize = INT_MAX;

namespace ball {

                          // -------------
                          // class RuleSet
                          // -------------

// CLASS METHODS
void RuleSet::printMask(bsl::ostream& stream,
                        MaskType      mask,
                        int           level,
                        int           spacesPerLevel)
{
    bsl::uint64_t dummy = mask;

    BSLMF_ASSERT(sizeof(mask) <= sizeof(dummy));

    bdlb::BitStringUtil::print(stream,
                               &dummy,
                               bdlb::BitUtil::sizeInBits(mask),
                               level,
                               spacesPerLevel);
}

// CREATORS
RuleSet::RuleSet(bslma::Allocator *basicAllocator)
: d_ruleHashtable(maxNumRules(),          // initial size
                  RuleHash(),             // hash functor
                  bsl::equal_to<Rule>(),  // equal functor
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

RuleSet::RuleSet(const RuleSet&    original,
                 bslma::Allocator *basicAllocator)
: d_ruleHashtable(maxNumRules(),          // initial size
                  RuleHash(),             // hash functor
                  bsl::equal_to<Rule>(),  // equal functor
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

RuleSet::~RuleSet()
{
}

// MANIPULATORS
int RuleSet::addRule(const Rule& value)
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

int RuleSet::addRules(const RuleSet& rules)
{
    int count = 0;
    for (int i = 0; i < rules.maxNumRules(); ++i) {
        const Rule *rule = rules.getRuleById(i);
        if (rule) {
            count += addRule(*rule) >= 0;
        }
    }

    return count;
}

int RuleSet::removeRule(const Rule& value)
{
    int id = ruleId(value);
    if (id < 0) {
        return 0;                                                     // RETURN
    }
    return removeRuleById(id);
}

int RuleSet::removeRuleById(int id)
{
    BSLS_ASSERT(0 <= id);
    BSLS_ASSERT(id < maxNumRules());

    const Rule *rule = d_ruleAddresses[id];
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

int RuleSet::removeRules(const RuleSet& rules)
{
    int count = 0;
    for (int i = 0; i < rules.maxNumRules(); ++i) {
        const Rule *rule = rules.getRuleById(i);
        if (rule) {
            int id = ruleId(*rule);
            if (id >= 0) {
                count += removeRuleById(id);
            }
        }
    }

    return count;
}

void RuleSet::removeAllRules()
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

RuleSet& RuleSet::operator=(const RuleSet& rhs)
{
    if (this != &rhs) {
        removeAllRules();
        addRules(rhs);
    }

    return *this;
}

// ACCESSOR
int RuleSet::ruleId(const Rule& value) const
{
    HashtableType::const_iterator iter = d_ruleHashtable.find(value);

    if (iter == d_ruleHashtable.end()) {
        return -1;                                                    // RETURN
    }

    const Rule *rule = &*iter;

    for (unsigned int i = 0; i < d_ruleAddresses.size(); ++i) {
        if (d_ruleAddresses[i] == rule) {
            return i;                                                 // RETURN
        }
    }

    return -1;
}

bsl::ostream& RuleSet::print(bsl::ostream& stream,
                             int           level,
                             int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);

    printer.start();
    for (int i = 0; i < maxNumRules(); ++i) {
        const Rule *rule = getRuleById(i);
        if (rule) {
            printer.printValue(*rule);
        }
    }
    printer.end();

    return stream;
}

}  // close package namespace

// FREE OPERATORS
bool ball::operator==(const RuleSet& lhs, const RuleSet& rhs)
{
    if (lhs.numRules() != rhs.numRules()) {
        return false;                                                 // RETURN
    }

    for (int i = 0; i < lhs.maxNumRules(); ++i) {
        const Rule *rule = lhs.getRuleById(i);
        if (rule && 0 > rhs.ruleId(*rule) ) {
            return false;                                             // RETURN
        }
    }
    return true;
}

bool ball::operator!=(const RuleSet& lhs, const RuleSet& rhs)
{
    return !(lhs == rhs);
}

}  // close enterprise namespace

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
