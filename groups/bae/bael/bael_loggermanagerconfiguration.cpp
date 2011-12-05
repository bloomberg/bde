// bael_loggermanagerconfiguration.cpp                                -*-C++-*-
#include <bael_loggermanagerconfiguration.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bael_loggermanagerconfiguration_cpp,"$Id$ $CSID$")

#include <bdeu_print.h>

#include <bdef_function.h>

#include <bdex_byteinstream.h>                  // for testing only
#include <bdex_byteoutstream.h>                 // for testing only
#include <bdex_testinstream.h>                  // for testing only
#include <bdex_testinstreamexception.h>         // for testing only
#include <bdex_testoutstream.h>                 // for testing only

#include <bslma_default.h>

#include <bsl_ostream.h>
#include <bsl_string.h>

namespace BloombergLP {

                    // -------------------------------------
                    // class bael_LoggerManagerConfiguration
                    // -------------------------------------

// CLASS METHODS
bool
bael_LoggerManagerConfiguration::isValidDefaultRecordBufferSize(int numBytes)
{
    return bael_LoggerManagerDefaults::isValidDefaultRecordBufferSize(
                                                                     numBytes);
}

bool
bael_LoggerManagerConfiguration::isValidDefaultLoggerBufferSize(int numBytes)
{
    return bael_LoggerManagerDefaults::isValidDefaultLoggerBufferSize(
                                                                     numBytes);
}

bool
bael_LoggerManagerConfiguration::areValidDefaultThresholdLevels(
                                                           int recordLevel,
                                                           int passLevel,
                                                           int triggerLevel,
                                                           int triggerAllLevel)
{
    return bael_LoggerManagerDefaults::areValidDefaultThresholdLevels(
                                                              recordLevel,
                                                              passLevel,
                                                              triggerLevel,
                                                              triggerAllLevel);
}

// CREATORS
bael_LoggerManagerConfiguration::bael_LoggerManagerConfiguration(
                                               bslma_Allocator *basicAllocator)
: d_defaults()
, d_userSchema(basicAllocator)
, d_userPopulator()
, d_categoryNameFilter()
, d_defaultThresholdsCb()
, d_logOrder(BAEL_LIFO)
, d_triggerMarkers(BAEL_BEGIN_END_MARKERS)
, d_allocator_p(bslma_Default::allocator(basicAllocator))
{
}

bael_LoggerManagerConfiguration::bael_LoggerManagerConfiguration(
                        const bael_LoggerManagerConfiguration&  original,
                        bslma_Allocator                        *basicAllocator)
: d_defaults(original.d_defaults)
, d_userSchema(original.d_userSchema, basicAllocator)
, d_userPopulator(original.d_userPopulator)
, d_categoryNameFilter(original.d_categoryNameFilter)
, d_defaultThresholdsCb(original.d_defaultThresholdsCb)
, d_logOrder(original.d_logOrder)
, d_triggerMarkers(original.d_triggerMarkers)
, d_allocator_p(bslma_Default::allocator(basicAllocator))
{
}

bael_LoggerManagerConfiguration::~bael_LoggerManagerConfiguration()
{
}

// MANIPULATORS
bael_LoggerManagerConfiguration&
bael_LoggerManagerConfiguration::operator=(
                                    const bael_LoggerManagerConfiguration& rhs)
{
    d_defaults            = rhs.d_defaults;
    d_userSchema          = rhs.d_userSchema;
    d_userPopulator       = rhs.d_userPopulator;
    d_categoryNameFilter  = rhs.d_categoryNameFilter;
    d_defaultThresholdsCb = rhs.d_defaultThresholdsCb;
    d_logOrder            = rhs.d_logOrder;
    d_triggerMarkers      = rhs.d_triggerMarkers;

    return *this;
}

void bael_LoggerManagerConfiguration::setDefaultValues(
                                    const bael_LoggerManagerDefaults& defaults)
{
    d_defaults = defaults;
}

int bael_LoggerManagerConfiguration::setDefaultRecordBufferSizeIfValid(
                                                                  int numBytes)
{
    return d_defaults.setDefaultRecordBufferSizeIfValid(numBytes);
}

int bael_LoggerManagerConfiguration::setDefaultLoggerBufferSizeIfValid(
                                                                  int numBytes)
{
    return d_defaults.setDefaultLoggerBufferSizeIfValid(numBytes);
}

int bael_LoggerManagerConfiguration::setDefaultThresholdLevelsIfValid(
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

void bael_LoggerManagerConfiguration::setUserFields(
                        const bdem_Schema&            schema,
                        const UserPopulatorCallback&  populator)
{
    d_userSchema    = schema;
    d_userPopulator = populator;
}

void bael_LoggerManagerConfiguration::setCategoryNameFilterCallback(
                                  const CategoryNameFilterCallback& nameFilter)
{
    d_categoryNameFilter = nameFilter;
}

void bael_LoggerManagerConfiguration::setDefaultThresholdLevelsCallback(
                        const DefaultThresholdLevelsCallback& thresholdsCb)
{
    d_defaultThresholdsCb = thresholdsCb;
}

void bael_LoggerManagerConfiguration::setLogOrder(LogOrder value)
{
    d_logOrder = value;
}

void bael_LoggerManagerConfiguration::setTriggerMarkers(TriggerMarkers value)
{
    d_triggerMarkers = value;
}

// ACCESSORS
const bael_LoggerManagerDefaults&
bael_LoggerManagerConfiguration::defaults() const
{
    return d_defaults;
}

int bael_LoggerManagerConfiguration::defaultRecordBufferSize() const
{
    return d_defaults.defaultRecordBufferSize();
}

int bael_LoggerManagerConfiguration::defaultLoggerBufferSize() const
{
    return d_defaults.defaultLoggerBufferSize();
}

int bael_LoggerManagerConfiguration::defaultRecordLevel() const
{
    return d_defaults.defaultRecordLevel();
}

int bael_LoggerManagerConfiguration::defaultPassLevel() const
{
    return d_defaults.defaultPassLevel();
}

int bael_LoggerManagerConfiguration::defaultTriggerLevel() const
{
    return d_defaults.defaultTriggerLevel();
}

int bael_LoggerManagerConfiguration::defaultTriggerAllLevel() const
{
    return d_defaults.defaultTriggerAllLevel();
}

const bdem_Schema& bael_LoggerManagerConfiguration::userSchema() const
{
    return d_userSchema;
}

const bael_LoggerManagerConfiguration::UserPopulatorCallback&
bael_LoggerManagerConfiguration::userPopulatorCallback() const
{
    return d_userPopulator;
}

const bael_LoggerManagerConfiguration::CategoryNameFilterCallback&
bael_LoggerManagerConfiguration::categoryNameFilterCallback() const
{
    return d_categoryNameFilter;
}

const bael_LoggerManagerConfiguration::DefaultThresholdLevelsCallback&
bael_LoggerManagerConfiguration::defaultThresholdLevelsCallback() const
{
    return d_defaultThresholdsCb;
}

bael_LoggerManagerConfiguration::LogOrder
bael_LoggerManagerConfiguration::logOrder() const
{
    return d_logOrder;
}

bael_LoggerManagerConfiguration::TriggerMarkers
bael_LoggerManagerConfiguration::triggerMarkers() const
{
    return d_triggerMarkers;
}

bsl::ostream&
bael_LoggerManagerConfiguration::print(bsl::ostream& stream,
                                       int           level,
                                       int           spacesPerLevel) const
{
    if (stream.bad()) {
        return stream;                                                // RETURN
    }

    const char NL = spacesPerLevel >= 0 ? '\n'
                                        : ' '; // one-line format if negative

    if (level >= 0) {
        bdeu_Print::indent(stream, level, spacesPerLevel);
    }
    else {
        level = -level;
    }

    stream << '[' << NL;

    bdeu_Print::indent(stream, level + 1, spacesPerLevel);
    stream << "Defaults:" << NL;
    d_defaults.print(stream, level + 1, spacesPerLevel);

    bdeu_Print::indent(stream, level + 1, spacesPerLevel);
    stream << "User Fields Schema:" << NL;
    d_userSchema.print(stream, level + 1, spacesPerLevel);

    bdeu_Print::indent(stream, level + 1, spacesPerLevel);
    const char *nullPop = d_userPopulator ? "not null" : "null";
    stream << "User Fields Populator functor is " << nullPop << NL;

    bdeu_Print::indent(stream, level + 1, spacesPerLevel);
    const char *nullCnf = d_categoryNameFilter ? "not null" : "null";
    stream << "Category Name Filter functor is " << nullCnf << NL;

    bdeu_Print::indent(stream, level + 1, spacesPerLevel);
    const char *nullDtcb = d_defaultThresholdsCb ? "not null" : "null";
    stream << "Default Threshold Callback functor is " << nullDtcb << NL;

    bdeu_Print::indent(stream, level + 1, spacesPerLevel);
    const char *logOrder = d_logOrder == BAEL_FIFO ? "FIFO" : "LIFO";
    stream << "Logging order is " << logOrder << NL;

    bdeu_Print::indent(stream, level + 1, spacesPerLevel);
    const char *triggerMarker = d_triggerMarkers == BAEL_NO_MARKERS
                                                 ? "NO_MARKERS"
                                                 : "BEGIN_END_MARKERS";
    stream << "Trigger markers are " << triggerMarker << NL;

    bdeu_Print::indent(stream, level, spacesPerLevel);
    stream << ']' << NL;

    return stream;
}

// FREE OPERATORS
bool operator==(const bael_LoggerManagerConfiguration& lhs,
                const bael_LoggerManagerConfiguration& rhs)
{
    return lhs.d_defaults            == rhs.d_defaults
        && lhs.d_userSchema          == rhs.d_userSchema
        && lhs.d_userPopulator       == rhs.d_userPopulator
        && lhs.d_categoryNameFilter  == rhs.d_categoryNameFilter
        && lhs.d_defaultThresholdsCb == rhs.d_defaultThresholdsCb
        && lhs.d_logOrder            == rhs.d_logOrder
        && lhs.d_triggerMarkers      == rhs.d_triggerMarkers;
}

bool operator!=(const bael_LoggerManagerConfiguration& lhs,
                const bael_LoggerManagerConfiguration& rhs)
{
    return !(lhs == rhs);
}

bsl::ostream& operator<<(bsl::ostream&                          stream,
                         const bael_LoggerManagerConfiguration& configuration)
{
    return configuration.print(stream);
}

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2007
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
