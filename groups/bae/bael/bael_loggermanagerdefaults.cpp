// bael_loggermanagerdefaults.cpp  -*-C++-*-
#include <bael_loggermanagerdefaults.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bael_loggermanagerdefaults_cpp,"$Id$ $CSID$")

#include <bdeu_print.h>
#include <bsls_assert.h>
#include <bdex_testoutstream.h>                 // for testing only
#include <bdex_testinstream.h>                  // for testing only
#include <bdex_testinstreamexception.h>         // for testing only
#include <bdex_byteoutstream.h>                 // for testing only
#include <bdex_byteinstream.h>                  // for testing only

#include <bsl_ostream.h>

namespace BloombergLP {

                    // ================================
                    // class bael_LoggerManagerDefaults
                    // ================================

enum {
    // This enumeration provides the default values for the attributes of
    // 'bael_LoggerManagerDefaults'.

    DEFAULT_RECORD_BUFFER_SIZE = 32768,
    DEFAULT_LOGGER_BUFFER_SIZE =  8192,
    DEFAULT_RECORD_LEVEL       =     0, // "Record" disabled
    DEFAULT_PASS_LEVEL         =    64, // 'ERROR'
    DEFAULT_TRIGGER_LEVEL      =     0, // "Trigger" disabled
    DEFAULT_TRIGGER_ALL_LEVEL  =     0  // "TriggerAll" disabled
};

// CLASS METHODS
bool bael_LoggerManagerDefaults::isValidDefaultRecordBufferSize(int numBytes)
{
    return 0 < numBytes;
}

bool bael_LoggerManagerDefaults::isValidDefaultLoggerBufferSize(int numBytes)
{
    return 0 < numBytes;
}

bool
bael_LoggerManagerDefaults::areValidDefaultThresholdLevels(int recordLevel,
                                                           int passLevel,
                                                           int triggerLevel,
                                                           int triggerAllLevel)
{
    const int bitwiseOr =
                      recordLevel | passLevel | triggerLevel | triggerAllLevel;

    return !(bitwiseOr & ~255);
}

int bael_LoggerManagerDefaults::defaultDefaultRecordBufferSize()
{
    return DEFAULT_RECORD_BUFFER_SIZE;
}

int bael_LoggerManagerDefaults::defaultDefaultLoggerBufferSize()
{
    return DEFAULT_LOGGER_BUFFER_SIZE;
}

int bael_LoggerManagerDefaults::defaultDefaultRecordLevel()
{
    return DEFAULT_RECORD_LEVEL;
}

int bael_LoggerManagerDefaults::defaultDefaultPassLevel()
{
    return DEFAULT_PASS_LEVEL;
}

int bael_LoggerManagerDefaults::defaultDefaultTriggerLevel()
{
    return DEFAULT_TRIGGER_LEVEL;
}

int bael_LoggerManagerDefaults::defaultDefaultTriggerAllLevel()
{
    return DEFAULT_TRIGGER_ALL_LEVEL;
}

int bael_LoggerManagerDefaults::maxSupportedBdexVersion()
{
    return 1;
}

// CREATORS
bael_LoggerManagerDefaults::bael_LoggerManagerDefaults()
: d_recordBufferSize(DEFAULT_RECORD_BUFFER_SIZE)
, d_loggerBufferSize(DEFAULT_LOGGER_BUFFER_SIZE)
, d_defaultRecordLevel(DEFAULT_RECORD_LEVEL)
, d_defaultPassLevel(DEFAULT_PASS_LEVEL)
, d_defaultTriggerLevel(DEFAULT_TRIGGER_LEVEL)
, d_defaultTriggerAllLevel(DEFAULT_TRIGGER_ALL_LEVEL)
{
}

bael_LoggerManagerDefaults::bael_LoggerManagerDefaults(
                                    const bael_LoggerManagerDefaults& original)
: d_recordBufferSize(original.d_recordBufferSize)
, d_loggerBufferSize(original.d_loggerBufferSize)
, d_defaultRecordLevel(original.d_defaultRecordLevel)
, d_defaultPassLevel(original.d_defaultPassLevel)
, d_defaultTriggerLevel(original.d_defaultTriggerLevel)
, d_defaultTriggerAllLevel(original.d_defaultTriggerAllLevel)
{
}

bael_LoggerManagerDefaults::~bael_LoggerManagerDefaults()
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
bael_LoggerManagerDefaults&
bael_LoggerManagerDefaults::operator=(const bael_LoggerManagerDefaults& rhs)
{
    d_recordBufferSize       = rhs.d_recordBufferSize;
    d_loggerBufferSize       = rhs.d_loggerBufferSize;
    d_defaultRecordLevel     = rhs.d_defaultRecordLevel;
    d_defaultPassLevel       = rhs.d_defaultPassLevel;
    d_defaultTriggerLevel    = rhs.d_defaultTriggerLevel;
    d_defaultTriggerAllLevel = rhs.d_defaultTriggerAllLevel;
    return *this;
}

int bael_LoggerManagerDefaults::setDefaultRecordBufferSizeIfValid(int numBytes)
{
    if (isValidDefaultRecordBufferSize(numBytes)) {
        d_recordBufferSize = numBytes;
        return 0;                                                     // RETURN
    }
    return -1;
}

int bael_LoggerManagerDefaults::setDefaultLoggerBufferSizeIfValid(int numBytes)
{
    if (isValidDefaultLoggerBufferSize(numBytes)) {
        d_loggerBufferSize = numBytes;
        return 0;                                                     // RETURN
    }
    return -1;
}

int bael_LoggerManagerDefaults::setDefaultThresholdLevelsIfValid(
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
int bael_LoggerManagerDefaults::defaultRecordBufferSize() const
{
    return d_recordBufferSize;
}

int bael_LoggerManagerDefaults::defaultLoggerBufferSize() const
{
    return d_loggerBufferSize;
}

int bael_LoggerManagerDefaults::defaultRecordLevel() const
{
    return d_defaultRecordLevel;
}

int bael_LoggerManagerDefaults::defaultPassLevel() const
{
    return d_defaultPassLevel;
}

int bael_LoggerManagerDefaults::defaultTriggerLevel() const
{
    return d_defaultTriggerLevel;
}

int bael_LoggerManagerDefaults::defaultTriggerAllLevel() const
{
    return d_defaultTriggerAllLevel;
}

bsl::ostream&
bael_LoggerManagerDefaults::print(bsl::ostream& stream,
                                  int           level,
                                  int           spacesPerLevel) const
{
    if (stream.bad()) {
        return stream;
    }

    const char NL = spacesPerLevel >=0 ? '\n' : ' ';
        // one-line format if negative

    if (level >= 0) {
        bdeu_Print::indent(stream, level, spacesPerLevel);
    }
    else {
        level = -level;
    }

    stream << '[' << NL;

    bdeu_Print::indent(stream, level + 1, spacesPerLevel);
    stream << "recordBufferSize : " << d_recordBufferSize       << NL;
    bdeu_Print::indent(stream, level + 1, spacesPerLevel);
    stream << "loggerBufferSize : " << d_loggerBufferSize       << NL;
    bdeu_Print::indent(stream, level + 1, spacesPerLevel);
    stream << "recordLevel      : " << d_defaultRecordLevel     << NL;
    bdeu_Print::indent(stream, level + 1, spacesPerLevel);
    stream << "passLevel        : " << d_defaultPassLevel       << NL;
    bdeu_Print::indent(stream, level + 1, spacesPerLevel);
    stream << "triggerLevel     : " << d_defaultTriggerLevel    << NL;
    bdeu_Print::indent(stream, level + 1, spacesPerLevel);
    stream << "triggerAllLevel  : " << d_defaultTriggerAllLevel << NL;

    bdeu_Print::indent(stream, level, spacesPerLevel);
    stream << ']' << NL;

    return stream;
}

// FREE OPERATORS
bool operator==(const bael_LoggerManagerDefaults& lhs,
                const bael_LoggerManagerDefaults& rhs)
{
    return
        lhs.d_recordBufferSize       == rhs.d_recordBufferSize       &&
        lhs.d_loggerBufferSize       == rhs.d_loggerBufferSize       &&
        lhs.d_defaultRecordLevel     == rhs.d_defaultRecordLevel     &&
        lhs.d_defaultPassLevel       == rhs.d_defaultPassLevel       &&
        lhs.d_defaultTriggerLevel    == rhs.d_defaultTriggerLevel    &&
        lhs.d_defaultTriggerAllLevel == rhs.d_defaultTriggerAllLevel;
}

bool operator!=(const bael_LoggerManagerDefaults& lhs,
                const bael_LoggerManagerDefaults& rhs)
{
    return !(lhs == rhs);
}

bsl::ostream& operator<<(bsl::ostream&                     stream,
                         const bael_LoggerManagerDefaults& config)
{
    return config.print(stream);
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
