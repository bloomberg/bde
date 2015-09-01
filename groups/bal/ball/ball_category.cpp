// ball_category.cpp                                                  -*-C++-*-
#include <ball_category.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ball_category_cpp,"$Id$ $CSID$")

#include <bslim_printer.h>

#include <bsls_assert.h>

#include <bsl_algorithm.h>
#include <bsl_ostream.h>

#include <ball_severity.h>                                  // for testing only

namespace BloombergLP {
namespace ball {



                            // --------------
                            // class Category
                            // --------------

// PRIVATE CREATORS
Category::Category(const char       *categoryName,
                   int               recordLevel,
                   int               passLevel,
                   int               triggerLevel,
                   int               triggerAllLevel,
                   bslma::Allocator *basicAllocator)
    // Note that this constructor is private, so the validation of the
    // threshold level values does not need to be repeated here.  They are
    // validated in 'CategoryManager::addCategory', prior to creating an
    // instance of this class.
: d_thresholdLevels(recordLevel, passLevel, triggerLevel, triggerAllLevel)
, d_threshold(ThresholdAggregate::maxLevel(recordLevel,
                                                passLevel,
                                                triggerLevel,
                                                triggerAllLevel))
, d_categoryName(categoryName, basicAllocator)
, d_categoryHolder(0)
, d_relevantRuleMask(0)
, d_ruleThreshold(0)
{
    BSLS_ASSERT(categoryName);
}

Category::~Category()
{
}

// PRIVATE MANIPULATORS
void
Category::linkCategoryHolder(CategoryHolder *categoryHolder)
{
    BSLS_ASSERT(categoryHolder);

    if (!categoryHolder->category()) {
        categoryHolder->setThreshold(bsl::max(d_threshold, d_ruleThreshold));
        categoryHolder->setCategory(this);
        categoryHolder->setNext(d_categoryHolder);
        d_categoryHolder = categoryHolder;
    }
}

void Category::resetCategoryHolders()
{
    CategoryHolder *holder = d_categoryHolder;
    while (holder) {
        CategoryHolder *nextHolder = holder->next();
        holder->reset();
        holder = nextHolder;
    }
    d_categoryHolder = 0;
}

// CLASS METHODS
bool Category::areValidThresholdLevels(int recordLevel,
                                       int passLevel,
                                       int triggerLevel,
                                       int triggerAllLevel)
{
    enum { BITS_PER_CHAR = 8 };

    return !((recordLevel | passLevel | triggerLevel | triggerAllLevel)
             >> BITS_PER_CHAR);
}

void Category::updateThresholdForHolders()
{
    if (d_categoryHolder) {
        CategoryHolder *holder = d_categoryHolder;
        const int threshold = bsl::max(d_threshold, d_ruleThreshold);
        if (threshold != holder->threshold()) {
            do {
                holder->setThreshold(threshold);
                holder = holder->next();
            } while (holder);
        }
    }
}

// MANIPULATORS
int Category::setLevels(int recordLevel,
                        int passLevel,
                        int triggerLevel,
                        int triggerAllLevel)
{
    if (Category::areValidThresholdLevels(recordLevel,
                                          passLevel,
                                          triggerLevel,
                                          triggerAllLevel)) {

        d_thresholdLevels.setLevels(recordLevel,
                                    passLevel,
                                    triggerLevel,
                                    triggerAllLevel);

        d_threshold = ThresholdAggregate::maxLevel(recordLevel,
                                                   passLevel,
                                                   triggerLevel,
                                                   triggerAllLevel);

        updateThresholdForHolders();
        return 0;                                                     // RETURN
    }

    return -1;
}

// ACCESSORS
bool Category::isEnabled(int level) const
{
    return d_threshold >= level;
}

                        // --------------------
                        // class CategoryHolder
                        // --------------------

// MANIPULATORS
void CategoryHolder::reset()
{
    AtomicOps::setIntRelaxed(&d_threshold, e_UNINITIALIZED_CATEGORY);
    AtomicOps::setPtrRelease(&d_category_p, 0);
    AtomicOps::setPtrRelease(&d_next_p, 0);
}

}  // close package namespace
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
