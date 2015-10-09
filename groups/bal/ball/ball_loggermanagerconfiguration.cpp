// ball_loggermanagerconfiguration.cpp                                -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <ball_loggermanagerconfiguration.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ball_loggermanagerconfiguration_cpp,"$Id$ $CSID$")

#include <bdlb_print.h>

#include <bslma_default.h>

#include <bsl_ostream.h>
#include <bsl_string.h>

namespace BloombergLP {

namespace ball {
                    // --------------------------------
                    // class LoggerManagerConfiguration
                    // --------------------------------

// CLASS METHODS
bool
LoggerManagerConfiguration::isValidDefaultRecordBufferSize(int numBytes)
{
    return LoggerManagerDefaults::isValidDefaultRecordBufferSize(numBytes);
}

bool
LoggerManagerConfiguration::isValidDefaultLoggerBufferSize(int numBytes)
{
    return LoggerManagerDefaults::isValidDefaultLoggerBufferSize(numBytes);
}

bool
LoggerManagerConfiguration::areValidDefaultThresholdLevels(int recordLevel,
                                                           int passLevel,
                                                           int triggerLevel,
                                                           int triggerAllLevel)
{
    return LoggerManagerDefaults::areValidDefaultThresholdLevels(
                                                              recordLevel,
                                                              passLevel,
                                                              triggerLevel,
                                                              triggerAllLevel);
}

// CREATORS
LoggerManagerConfiguration::LoggerManagerConfiguration(
                                              bslma::Allocator *basicAllocator)
: d_defaults()
, d_userFieldsSchema(basicAllocator)
, d_userPopulator(bsl::allocator_arg_t(),
                  bsl::allocator<UserFieldsPopulatorCallback>(basicAllocator))
, d_categoryNameFilter(
                    bsl::allocator_arg_t(),
                    bsl::allocator<CategoryNameFilterCallback>(basicAllocator))
, d_defaultThresholdsCb(
                bsl::allocator_arg_t(),
                bsl::allocator<DefaultThresholdLevelsCallback>(basicAllocator))
, d_logOrder(e_LIFO)
, d_triggerMarkers(e_BEGIN_END_MARKERS)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

LoggerManagerConfiguration::LoggerManagerConfiguration(
                             const LoggerManagerConfiguration&  original,
                             bslma::Allocator                  *basicAllocator)
: d_defaults(original.d_defaults)
, d_userFieldsSchema(original.d_userFieldsSchema, basicAllocator)
, d_userPopulator(bsl::allocator_arg_t(),
                  bsl::allocator<UserFieldsPopulatorCallback>(basicAllocator),
                  original.d_userPopulator)
, d_categoryNameFilter(
                    bsl::allocator_arg_t(),
                    bsl::allocator<CategoryNameFilterCallback>(basicAllocator),
                    original.d_categoryNameFilter)
, d_defaultThresholdsCb(
                bsl::allocator_arg_t(),
                bsl::allocator<DefaultThresholdLevelsCallback>(basicAllocator),
                original.d_defaultThresholdsCb)
, d_logOrder(original.d_logOrder)
, d_triggerMarkers(original.d_triggerMarkers)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

LoggerManagerConfiguration::~LoggerManagerConfiguration()
{
}

// MANIPULATORS
LoggerManagerConfiguration&
LoggerManagerConfiguration::operator=(const LoggerManagerConfiguration& rhs)
{
    d_defaults            = rhs.d_defaults;
    d_userFieldsSchema    = rhs.d_userFieldsSchema;
    d_userPopulator       = rhs.d_userPopulator;
    d_categoryNameFilter  = rhs.d_categoryNameFilter;
    d_defaultThresholdsCb = rhs.d_defaultThresholdsCb;
    d_logOrder            = rhs.d_logOrder;
    d_triggerMarkers      = rhs.d_triggerMarkers;

    return *this;
}

void LoggerManagerConfiguration::setDefaultValues(
                                    const LoggerManagerDefaults& defaults)
{
    d_defaults = defaults;
}

int LoggerManagerConfiguration::setDefaultRecordBufferSizeIfValid(int numBytes)
{
    return d_defaults.setDefaultRecordBufferSizeIfValid(numBytes);
}

int LoggerManagerConfiguration::setDefaultLoggerBufferSizeIfValid(int numBytes)
{
    return d_defaults.setDefaultLoggerBufferSizeIfValid(numBytes);
}


int LoggerManagerConfiguration::setDefaultThresholdLevelsIfValid(int passLevel)
{
    return d_defaults.setDefaultThresholdLevelsIfValid(passLevel);
}

int LoggerManagerConfiguration::setDefaultThresholdLevelsIfValid(
                                                           int recordLevel,
                                                           int passLevel,
                                                           int triggerLevel,
                                                           int triggerAllLevel)
{
    return d_defaults.setDefaultThresholdLevelsIfValid(recordLevel,
                                                       passLevel,
                                                       triggerLevel,
                                                       triggerAllLevel);
}

void LoggerManagerConfiguration::setUserFieldsSchema(
                          const ball::UserFieldsSchema       fieldDescriptions,
                          const UserFieldsPopulatorCallback& populatorCallback)

{
    d_userFieldsSchema = fieldDescriptions;
    d_userPopulator        = populatorCallback;
}

void LoggerManagerConfiguration::setCategoryNameFilterCallback(
                                  const CategoryNameFilterCallback& nameFilter)
{
    d_categoryNameFilter = nameFilter;
}

void LoggerManagerConfiguration::setDefaultThresholdLevelsCallback(
                        const DefaultThresholdLevelsCallback& thresholdsCb)
{
    d_defaultThresholdsCb = thresholdsCb;
}

void LoggerManagerConfiguration::setLogOrder(LogOrder value)
{
    d_logOrder = value;
}

void LoggerManagerConfiguration::setTriggerMarkers(TriggerMarkers value)
{
    d_triggerMarkers = value;
}

// ACCESSORS
const LoggerManagerDefaults&
LoggerManagerConfiguration::defaults() const
{
    return d_defaults;
}

int LoggerManagerConfiguration::defaultRecordBufferSize() const
{
    return d_defaults.defaultRecordBufferSize();
}

int LoggerManagerConfiguration::defaultLoggerBufferSize() const
{
    return d_defaults.defaultLoggerBufferSize();
}

int LoggerManagerConfiguration::defaultRecordLevel() const
{
    return d_defaults.defaultRecordLevel();
}

int LoggerManagerConfiguration::defaultPassLevel() const
{
    return d_defaults.defaultPassLevel();
}

int LoggerManagerConfiguration::defaultTriggerLevel() const
{
    return d_defaults.defaultTriggerLevel();
}

int LoggerManagerConfiguration::defaultTriggerAllLevel() const
{
    return d_defaults.defaultTriggerAllLevel();
}

const ball::UserFieldsSchema&
LoggerManagerConfiguration::userFieldsSchema() const
{
    return d_userFieldsSchema;
}

const LoggerManagerConfiguration::UserFieldsPopulatorCallback&
LoggerManagerConfiguration::userFieldsPopulatorCallback() const
{
    return d_userPopulator;
}

const LoggerManagerConfiguration::CategoryNameFilterCallback&
LoggerManagerConfiguration::categoryNameFilterCallback() const
{
    return d_categoryNameFilter;
}

const LoggerManagerConfiguration::DefaultThresholdLevelsCallback&
LoggerManagerConfiguration::defaultThresholdLevelsCallback() const
{
    return d_defaultThresholdsCb;
}

LoggerManagerConfiguration::LogOrder
LoggerManagerConfiguration::logOrder() const
{
    return d_logOrder;
}

LoggerManagerConfiguration::TriggerMarkers
LoggerManagerConfiguration::triggerMarkers() const
{
    return d_triggerMarkers;
}

bsl::ostream&
LoggerManagerConfiguration::print(bsl::ostream& stream,
                                  int           level,
                                  int           spacesPerLevel) const
{
    if (stream.bad()) {
        return stream;                                                // RETURN
    }

    const char NL = spacesPerLevel >= 0 ? '\n'
                                        : ' '; // one-line format if negative

    if (level >= 0) {
        bdlb::Print::indent(stream, level, spacesPerLevel);
    }
    else {
        level = -level;
    }

    stream << '[' << NL;

    bdlb::Print::indent(stream, level + 1, spacesPerLevel);
    stream << "Defaults:" << NL;
    d_defaults.print(stream, level + 1, spacesPerLevel);

    bdlb::Print::indent(stream, level + 1, spacesPerLevel);
    stream << "User Fields Schema:" << NL;
    d_userFieldsSchema.print(stream, level + 1, spacesPerLevel);

    bdlb::Print::indent(stream, level + 1, spacesPerLevel);
    const char *nullPop = d_userPopulator ? "not null" : "null";
    stream << "User Fields Populator functor is " << nullPop << NL;

    bdlb::Print::indent(stream, level + 1, spacesPerLevel);
    const char *nullCnf = d_categoryNameFilter ? "not null" : "null";
    stream << "Category Name Filter functor is " << nullCnf << NL;

    bdlb::Print::indent(stream, level + 1, spacesPerLevel);
    const char *nullDtcb = d_defaultThresholdsCb ? "not null" : "null";
    stream << "Default Threshold Callback functor is " << nullDtcb << NL;

    bdlb::Print::indent(stream, level + 1, spacesPerLevel);
    const char *logOrder = d_logOrder == e_FIFO ? "FIFO" : "LIFO";
    stream << "Logging order is " << logOrder << NL;

    bdlb::Print::indent(stream, level + 1, spacesPerLevel);
    const char *triggerMarker = d_triggerMarkers == e_NO_MARKERS
                                                 ? "NO_MARKERS"
                                                 : "BEGIN_END_MARKERS";
    stream << "Trigger markers are " << triggerMarker << NL;

    bdlb::Print::indent(stream, level, spacesPerLevel);
    stream << ']' << NL;

    return stream;
}

}  // close package namespace

// FREE OPERATORS
bool ball::operator==(const ball::LoggerManagerConfiguration& lhs,
                      const ball::LoggerManagerConfiguration& rhs)
{   // TBD: Note that we are casting the three 'bsl::function' data members to
    // 'bool' and comparing the boolean values as this was the accidental
    // behavior present in 'bsl::function'.  Now that 'bsl::function' has
    // been updated to suppress this operator, the operation has been coded
    // inline here to retain the old semantic - but we need to consider whether
    // this is truly the desired behavior, and if so remove this note, or
    // correct it otherwise.
    return lhs.d_defaults                  == rhs.d_defaults
        && lhs.d_userFieldsSchema          == rhs.d_userFieldsSchema
        && (bool)lhs.d_userPopulator       == (bool)rhs.d_userPopulator
        && (bool)lhs.d_categoryNameFilter  == (bool)rhs.d_categoryNameFilter
        && (bool)lhs.d_defaultThresholdsCb == (bool)rhs.d_defaultThresholdsCb
        && lhs.d_logOrder                  == rhs.d_logOrder
        && lhs.d_triggerMarkers            == rhs.d_triggerMarkers;
}

bool ball::operator!=(const ball::LoggerManagerConfiguration& lhs,
                      const ball::LoggerManagerConfiguration& rhs)
{
    return !(lhs == rhs);
}

bsl::ostream& ball::operator<<(
                         bsl::ostream&                           stream,
                         const ball::LoggerManagerConfiguration& configuration)
{
    return configuration.print(stream);
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
