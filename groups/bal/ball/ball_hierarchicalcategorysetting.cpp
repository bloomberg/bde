// ball_hierarchicalcategorysetting.cpp                               -*-C++-*-
#include <ball_hierarchicalcategorysetting.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ball_hierarchicalcategorysetting_cpp, "$Id$ $CSID$")

#include <ball_category.h>
#include <ball_thresholdaggregate.h>

#include <bslim_printer.h>

#include <bsls_assert.h>

#include <bsl_ostream.h>

namespace BloombergLP {
namespace ball {

                     // ---------------------------------
                     // class HierarchicalCategorySetting
                     // ---------------------------------

// CREATORS
HierarchicalCategorySetting::HierarchicalCategorySetting(
                                       const bsl::string_view& categoryPrefix,
                                       int                     recordLevel,
                                       int                     passLevel,
                                       int                     triggerLevel,
                                       int                     triggerAllLevel,
                                       allocator_type          allocator)
: d_categoryPrefix(categoryPrefix, allocator)
, d_thresholdLevels(ThresholdAggregateUtil::pack(ThresholdAggregate(
                                                             recordLevel,
                                                             passLevel,
                                                             triggerLevel,
                                                             triggerAllLevel)))
{
    BSLS_ASSERT(ThresholdAggregate::areValidThresholdLevels(recordLevel,
                                                            passLevel,
                                                            triggerLevel,
                                                            triggerAllLevel));
}

HierarchicalCategorySetting::HierarchicalCategorySetting(
                                      const bsl::string_view&   categoryPrefix,
                                      const ThresholdAggregate& levels,
                                      allocator_type            allocator)
: d_categoryPrefix(categoryPrefix, allocator)
, d_thresholdLevels(ThresholdAggregateUtil::pack(levels))
{
}

HierarchicalCategorySetting::HierarchicalCategorySetting(
                                  const HierarchicalCategorySetting& original,
                                  const allocator_type&              allocator)
: d_categoryPrefix(original.d_categoryPrefix, allocator)
, d_thresholdLevels(original.d_thresholdLevels)
{
}

HierarchicalCategorySetting::HierarchicalCategorySetting(
                  bslmf::MovableRef<HierarchicalCategorySetting> original)
                                                         BSLS_KEYWORD_NOEXCEPT
: d_categoryPrefix(
                  MoveUtil::move(MoveUtil::access(original).d_categoryPrefix)),
  d_thresholdLevels(
                  MoveUtil::move(MoveUtil::access(original).d_thresholdLevels))
{
}

HierarchicalCategorySetting::HierarchicalCategorySetting(
                      bslmf::MovableRef<HierarchicalCategorySetting> original,
                      const allocator_type&                          allocator)
: d_categoryPrefix(MoveUtil::move(MoveUtil::access(original).d_categoryPrefix),
                   allocator)
, d_thresholdLevels(
                  MoveUtil::move(MoveUtil::access(original).d_thresholdLevels))
{
}

// MANIPULATORS
int HierarchicalCategorySetting::setLevels(int recordLevel,
                                           int passLevel,
                                           int triggerLevel,
                                           int triggerAllLevel)
{
    if (!Category::areValidThresholdLevels(recordLevel,
                                           passLevel,
                                           triggerLevel,
                                           triggerAllLevel)) {
        return -1;                                                    // RETURN
    }

    d_thresholdLevels = ThresholdAggregateUtil::pack(ThresholdAggregate(
                                                             recordLevel,
                                                             passLevel,
                                                             triggerLevel,
                                                             triggerAllLevel));

    return 0;
}

// ACCESSORS

                                  // Aspects

bsl::ostream&
HierarchicalCategorySetting::print(bsl::ostream& stream,
                                   int           level,
                                   int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute("categoryPrefix",  d_categoryPrefix.c_str());
    printer.printAttribute("recordLevel",     recordLevel());
    printer.printAttribute("passLevel",       passLevel());
    printer.printAttribute("triggerLevel",    triggerLevel());
    printer.printAttribute("triggerAllLevel", triggerAllLevel());
    printer.end();

    return stream;
}

}  // close package namespace

// FREE OPERATORS
bsl::ostream& ball::operator<<(bsl::ostream&                      stream,
                               const HierarchicalCategorySetting& object)
{
    return object.print(stream, 0, -1);
}

}  // close enterprise namespace

// ----------------------------------------------------------------------------
// Copyright 2025 Bloomberg Finance L.P.
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
