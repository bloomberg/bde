// ball_loggermanagerdefaults.cpp                                     -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <ball_loggermanagerdefaults.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ball_loggermanagerdefaults_cpp,"$Id$ $CSID$")

#include <bdlb_print.h>
#include <bsls_assert.h>

#include <bsl_ostream.h>

namespace BloombergLP {

                    // ================================
                    // class ball::LoggerManagerDefaults
                    // ================================

enum {
    // This enumeration provides the default values for the attributes of
    // 'ball::LoggerManagerDefaults'.

    DEFAULT_RECORD_BUFFER_SIZE = 32768,
    DEFAULT_LOGGER_BUFFER_SIZE =  8192,
    DEFAULT_RECORD_LEVEL       =     0, // "Record" disabled
    DEFAULT_PASS_LEVEL         =    64, // 'ERROR'
    DEFAULT_TRIGGER_LEVEL      =     0, // "Trigger" disabled
    DEFAULT_TRIGGER_ALL_LEVEL  =     0  // "TriggerAll" disabled
};

namespace ball {
// CLASS METHODS
bool LoggerManagerDefaults::isValidDefaultRecordBufferSize(int numBytes)
{
    return 0 < numBytes;
}

bool LoggerManagerDefaults::isValidDefaultLoggerBufferSize(int numBytes)
{
    return 0 < numBytes;
}

bool
LoggerManagerDefaults::areValidDefaultThresholdLevels(int recordLevel,
                                                           int passLevel,
                                                           int triggerLevel,
                                                           int triggerAllLevel)
{
    const int bitwiseOr =
                      recordLevel | passLevel | triggerLevel | triggerAllLevel;

    return !(bitwiseOr & ~255);
}

int LoggerManagerDefaults::defaultDefaultRecordBufferSize()
{
    return DEFAULT_RECORD_BUFFER_SIZE;
}

int LoggerManagerDefaults::defaultDefaultLoggerBufferSize()
{
    return DEFAULT_LOGGER_BUFFER_SIZE;
}

int LoggerManagerDefaults::defaultDefaultRecordLevel()
{
    return DEFAULT_RECORD_LEVEL;
}

int LoggerManagerDefaults::defaultDefaultPassLevel()
{
    return DEFAULT_PASS_LEVEL;
}

int LoggerManagerDefaults::defaultDefaultTriggerLevel()
{
    return DEFAULT_TRIGGER_LEVEL;
}

int LoggerManagerDefaults::defaultDefaultTriggerAllLevel()
{
    return DEFAULT_TRIGGER_ALL_LEVEL;
}


// CREATORS
LoggerManagerDefaults::LoggerManagerDefaults()
: d_recordBufferSize(DEFAULT_RECORD_BUFFER_SIZE)
, d_loggerBufferSize(DEFAULT_LOGGER_BUFFER_SIZE)
, d_defaultRecordLevel(DEFAULT_RECORD_LEVEL)
, d_defaultPassLevel(DEFAULT_PASS_LEVEL)
, d_defaultTriggerLevel(DEFAULT_TRIGGER_LEVEL)
, d_defaultTriggerAllLevel(DEFAULT_TRIGGER_ALL_LEVEL)
{
}

LoggerManagerDefaults::LoggerManagerDefaults(
                                    const LoggerManagerDefaults& original)
: d_recordBufferSize(original.d_recordBufferSize)
, d_loggerBufferSize(original.d_loggerBufferSize)
, d_defaultRecordLevel(original.d_defaultRecordLevel)
, d_defaultPassLevel(original.d_defaultPassLevel)
, d_defaultTriggerLevel(original.d_defaultTriggerLevel)
, d_defaultTriggerAllLevel(original.d_defaultTriggerAllLevel)
{
}

LoggerManagerDefaults::~LoggerManagerDefaults()
{
    BSLS_ASSERT(0   <  d_recordBufferSize);
    BSLS_ASSERT(0   <  d_loggerBufferSize);
    BSLS_ASSERT(0   <= d_defaultRecordLevel);
    BSLS_ASSERT(255 >= d_defaultRecordLevel);
    BSLS_ASSERT(0   <= d_defaultPassLevel);
    BSLS_ASSERT(255 >= d_defaultPassLevel);
    BSLS_ASSERT(0   <= d_defaultTriggerLevel);
    BSLS_ASSERT(255 >= d_defaultTriggerLevel);
    BSLS_ASSERT(0   <= d_defaultTriggerAllLevel);
    BSLS_ASSERT(255 >= d_defaultTriggerAllLevel);
}

// MANIPULATORS
LoggerManagerDefaults&
LoggerManagerDefaults::operator=(const LoggerManagerDefaults& rhs)
{
    d_recordBufferSize       = rhs.d_recordBufferSize;
    d_loggerBufferSize       = rhs.d_loggerBufferSize;
    d_defaultRecordLevel     = rhs.d_defaultRecordLevel;
    d_defaultPassLevel       = rhs.d_defaultPassLevel;
    d_defaultTriggerLevel    = rhs.d_defaultTriggerLevel;
    d_defaultTriggerAllLevel = rhs.d_defaultTriggerAllLevel;
    return *this;
}

int LoggerManagerDefaults::setDefaultRecordBufferSizeIfValid(int numBytes)
{
    if (isValidDefaultRecordBufferSize(numBytes)) {
        d_recordBufferSize = numBytes;
        return 0;                                                     // RETURN
    }
    return -1;
}

int LoggerManagerDefaults::setDefaultLoggerBufferSizeIfValid(int numBytes)
{
    if (isValidDefaultLoggerBufferSize(numBytes)) {
        d_loggerBufferSize = numBytes;
        return 0;                                                     // RETURN
    }
    return -1;
}

int LoggerManagerDefaults::setDefaultThresholdLevelsIfValid(int passLevel)
{
    if (areValidDefaultThresholdLevels(0,
                                       passLevel,
                                       0,
                                       0)) {

        d_defaultRecordLevel     = 0;
        d_defaultPassLevel       = passLevel;
        d_defaultTriggerLevel    = 0;
        d_defaultTriggerAllLevel = 0;
        return 0;                                                     // RETURN
    }
    return -1;
}

int LoggerManagerDefaults::setDefaultThresholdLevelsIfValid(
                                                           int recordLevel,
                                                           int passLevel,
                                                           int triggerLevel,
                                                           int triggerAllLevel)
{
    if (areValidDefaultThresholdLevels(recordLevel,
                                       passLevel,
                                       triggerLevel,
                                       triggerAllLevel)) {

        d_defaultRecordLevel     = recordLevel;
        d_defaultPassLevel       = passLevel;
        d_defaultTriggerLevel    = triggerLevel;
        d_defaultTriggerAllLevel = triggerAllLevel;
        return 0;                                                     // RETURN
    }
    return -1;
}

// ACCESSORS
int LoggerManagerDefaults::defaultRecordBufferSize() const
{
    return d_recordBufferSize;
}

int LoggerManagerDefaults::defaultLoggerBufferSize() const
{
    return d_loggerBufferSize;
}

int LoggerManagerDefaults::defaultRecordLevel() const
{
    return d_defaultRecordLevel;
}

int LoggerManagerDefaults::defaultPassLevel() const
{
    return d_defaultPassLevel;
}

int LoggerManagerDefaults::defaultTriggerLevel() const
{
    return d_defaultTriggerLevel;
}

int LoggerManagerDefaults::defaultTriggerAllLevel() const
{
    return d_defaultTriggerAllLevel;
}

bsl::ostream&
LoggerManagerDefaults::print(bsl::ostream& stream,
                                  int           level,
                                  int           spacesPerLevel) const
{
    if (stream.bad()) {
        return stream;                                                // RETURN
    }

    const char NL = spacesPerLevel >=0 ? '\n' : ' ';
        // one-line format if negative

    if (level >= 0) {
        bdlb::Print::indent(stream, level, spacesPerLevel);
    }
    else {
        level = -level;
    }

    stream << '[' << NL;

    bdlb::Print::indent(stream, level + 1, spacesPerLevel);
    stream << "recordBufferSize : " << d_recordBufferSize       << NL;
    bdlb::Print::indent(stream, level + 1, spacesPerLevel);
    stream << "loggerBufferSize : " << d_loggerBufferSize       << NL;
    bdlb::Print::indent(stream, level + 1, spacesPerLevel);
    stream << "recordLevel      : " << d_defaultRecordLevel     << NL;
    bdlb::Print::indent(stream, level + 1, spacesPerLevel);
    stream << "passLevel        : " << d_defaultPassLevel       << NL;
    bdlb::Print::indent(stream, level + 1, spacesPerLevel);
    stream << "triggerLevel     : " << d_defaultTriggerLevel    << NL;
    bdlb::Print::indent(stream, level + 1, spacesPerLevel);
    stream << "triggerAllLevel  : " << d_defaultTriggerAllLevel << NL;

    bdlb::Print::indent(stream, level, spacesPerLevel);
    stream << ']' << NL;

    return stream;
}
}  // close package namespace

// FREE OPERATORS
bool ball::operator==(const LoggerManagerDefaults& lhs,
                const LoggerManagerDefaults& rhs)
{
    return
        lhs.d_recordBufferSize       == rhs.d_recordBufferSize       &&
        lhs.d_loggerBufferSize       == rhs.d_loggerBufferSize       &&
        lhs.d_defaultRecordLevel     == rhs.d_defaultRecordLevel     &&
        lhs.d_defaultPassLevel       == rhs.d_defaultPassLevel       &&
        lhs.d_defaultTriggerLevel    == rhs.d_defaultTriggerLevel    &&
        lhs.d_defaultTriggerAllLevel == rhs.d_defaultTriggerAllLevel;
}

bool ball::operator!=(const LoggerManagerDefaults& lhs,
                const LoggerManagerDefaults& rhs)
{
    return !(lhs == rhs);
}

bsl::ostream& ball::operator<<(bsl::ostream&                     stream,
                         const LoggerManagerDefaults& config)
{
    return config.print(stream);
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
