// bael_fileobserver.cpp                                              -*-C++-*-
#include <bael_fileobserver.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bael_fileobserver_cpp,"$Id$ $CSID$")

#include <bael_context.h>
#include <bael_record.h>

#ifdef FOR_TESTING_ONLY
#include <bael_defaultobserver.h>             // for testing only
#include <bael_log.h>                         // for testing only
#include <bael_loggermanager.h>               // for testing only
#include <bael_loggermanagerconfiguration.h>  // for testing only
#include <bael_multiplexobserver.h>           // for testing only
#endif

#include <bcemt_lockguard.h>

#include <bsl_cstdio.h>
#include <bsl_cstring.h>   // for 'bsl::strcmp'
#include <bsl_sstream.h>

namespace {

const char *const DEFAULT_LONG_FORMAT =  "\n%d %p:%t %s %f:%l %c %m %u\n";
const char *const DEFAULT_LONG_FORMAT_WITHOUT_USERFIELDS =
                                         "\n%d %p:%t %s %f:%l %c %m\n";

const char *const DEFAULT_SHORT_FORMAT = "\n%s %f:%l %c %m %u\n";
const char *const DEFAULT_SHORT_FORMAT_WITHOUT_USERFIELDS =
                                         "\n%s %f:%l %c %m\n";

}  // close unnamed namespace

namespace BloombergLP {

                          // -----------------------
                          // class bael_FileObserver
                          // -----------------------

// CREATORS
bael_FileObserver::bael_FileObserver(bael_Severity::Level  stdoutThreshold,
                                     bslma_Allocator      *basicAllocator)
: d_logFileFormatter(DEFAULT_LONG_FORMAT,
                     bdet_DatetimeInterval(0),
                     basicAllocator)
, d_stdoutFormatter(DEFAULT_LONG_FORMAT,
                    bdet_DatetimeInterval(0),
                    basicAllocator)
, d_stdoutThreshold(stdoutThreshold)
, d_useRegularFormatOnStdoutFlag(true)
, d_publishInLocalTime(false)
, d_userFieldsLoggingFlag(true)
, d_stdoutLongFormat(DEFAULT_LONG_FORMAT, basicAllocator)
, d_stdoutShortFormat(DEFAULT_SHORT_FORMAT, basicAllocator)
, d_fileObserver2(basicAllocator)
{
}

bael_FileObserver::bael_FileObserver(bael_Severity::Level  stdoutThreshold,
                                     bool                  publishInLocalTime,
                                     bslma_Allocator      *basicAllocator)
: d_logFileFormatter(DEFAULT_LONG_FORMAT,
                     bdet_DatetimeInterval(0),
                     basicAllocator)
, d_stdoutFormatter(DEFAULT_LONG_FORMAT,
                    bdet_DatetimeInterval(0),
                    basicAllocator)
, d_stdoutThreshold(stdoutThreshold)
, d_useRegularFormatOnStdoutFlag(true)
, d_publishInLocalTime(publishInLocalTime)
, d_userFieldsLoggingFlag(true)
, d_stdoutLongFormat(DEFAULT_LONG_FORMAT, basicAllocator)
, d_stdoutShortFormat(DEFAULT_SHORT_FORMAT, basicAllocator)
, d_fileObserver2(basicAllocator)
{
    if (d_publishInLocalTime) {
        d_logFileFormatter.setTimestampOffset(
                                            d_fileObserver2.localTimeOffset());
        d_stdoutFormatter.setTimestampOffset(
                                            d_fileObserver2.localTimeOffset());
        d_fileObserver2.enablePublishInLocalTime();
    }
}

bael_FileObserver::~bael_FileObserver()
{
}

// MANIPULATORS
void bael_FileObserver::disableStdoutLoggingPrefix()
{
    bcemt_LockGuard<bcemt_Mutex> guard(&d_mutex);
    if (false == d_useRegularFormatOnStdoutFlag) {
        return;                                                       // RETURN
    }
    d_useRegularFormatOnStdoutFlag = false;
    d_stdoutFormatter.setFormat(d_stdoutShortFormat.c_str());
}

void bael_FileObserver::disableUserFieldsLogging()
{
    bcemt_LockGuard<bcemt_Mutex> guard(&d_mutex);
    if (false == d_userFieldsLoggingFlag) {
        return;                                                       // RETURN
    }
    d_userFieldsLoggingFlag = false;
    if (0 == bsl::strcmp(
                      d_stdoutFormatter.format(),
                      d_useRegularFormatOnStdoutFlag ? DEFAULT_LONG_FORMAT
                                                     : DEFAULT_SHORT_FORMAT)) {
        d_stdoutFormatter.setFormat(
                                d_useRegularFormatOnStdoutFlag
                                    ? DEFAULT_LONG_FORMAT_WITHOUT_USERFIELDS
                                    : DEFAULT_SHORT_FORMAT_WITHOUT_USERFIELDS);
    }
    if (0 == bsl::strcmp(d_logFileFormatter.format(), DEFAULT_LONG_FORMAT)) {
        d_logFileFormatter.setFormat(DEFAULT_LONG_FORMAT_WITHOUT_USERFIELDS);
        d_fileObserver2.setLogFileFunctor(d_logFileFormatter);
    }
}

void bael_FileObserver::disablePublishInLocalTime()
{
    bcemt_LockGuard<bcemt_Mutex> guard(&d_mutex);
    d_publishInLocalTime = false;
    d_stdoutFormatter.setTimestampOffset(bdet_DatetimeInterval(0));
    d_logFileFormatter.setTimestampOffset(bdet_DatetimeInterval(0));
    d_fileObserver2.setLogFileFunctor(d_logFileFormatter);
}

void bael_FileObserver::enableStdoutLoggingPrefix()
{
    bcemt_LockGuard<bcemt_Mutex> guard(&d_mutex);
    if (true == d_useRegularFormatOnStdoutFlag) {
        return;                                                       // RETURN
    }
    d_useRegularFormatOnStdoutFlag = true;
    d_stdoutFormatter.setFormat(d_stdoutLongFormat.c_str());
}

void bael_FileObserver::enableUserFieldsLogging()
{
    bcemt_LockGuard<bcemt_Mutex> guard(&d_mutex);
    if (true == d_userFieldsLoggingFlag) {
        return;                                                       // RETURN
    }
    d_userFieldsLoggingFlag = true;
    if (0 == bsl::strcmp(d_stdoutFormatter.format(),
                         d_useRegularFormatOnStdoutFlag
                             ? DEFAULT_LONG_FORMAT_WITHOUT_USERFIELDS
                             : DEFAULT_SHORT_FORMAT_WITHOUT_USERFIELDS)) {
        d_stdoutFormatter.setFormat(
                        d_useRegularFormatOnStdoutFlag ? DEFAULT_LONG_FORMAT
                                                       : DEFAULT_SHORT_FORMAT);
    }
    if (0 == bsl::strcmp(d_logFileFormatter.format(),
                         DEFAULT_LONG_FORMAT_WITHOUT_USERFIELDS)) {
        d_logFileFormatter.setFormat(DEFAULT_LONG_FORMAT);
        d_fileObserver2.setLogFileFunctor(d_logFileFormatter);
    }
}

void bael_FileObserver::enablePublishInLocalTime()
{
    bcemt_LockGuard<bcemt_Mutex> guard(&d_mutex);
    d_publishInLocalTime = true;
    d_stdoutFormatter.setTimestampOffset(d_fileObserver2.localTimeOffset());
    d_logFileFormatter.setTimestampOffset(d_fileObserver2.localTimeOffset());
    d_fileObserver2.setLogFileFunctor(d_logFileFormatter);
}

void bael_FileObserver::publish(const bael_Record&  record,
                                const bael_Context& context)
{
    bcemt_LockGuard<bcemt_Mutex> guard(&d_mutex);

    if (record.fixedFields().severity() <= d_stdoutThreshold) {
        bsl::ostringstream oss;
        d_stdoutFormatter(oss, record);

        // Use 'fwrite' to specify the length to write.

        bsl::fwrite(oss.str().c_str(), oss.str().length(), 1, stdout);
        bsl::fflush(stdout);
    }

    d_fileObserver2.publish(record, context);
}

void
bael_FileObserver::setStdoutThreshold(bael_Severity::Level stdoutThreshold)
{
    bcemt_LockGuard<bcemt_Mutex> guard(&d_mutex);
    d_stdoutThreshold = stdoutThreshold;
}

void
bael_FileObserver::setLogFormat(const char *logFileFormat,
                                const char *stdoutFormat)
{
    bcemt_LockGuard<bcemt_Mutex> guard(&d_mutex);
    d_stdoutLongFormat = stdoutFormat;
    d_logFileFormatter.setFormat(logFileFormat);
    d_fileObserver2.setLogFileFunctor(d_logFileFormatter);
    d_stdoutFormatter.setFormat(stdoutFormat);
}

// ACCESSORS
bael_Severity::Level bael_FileObserver::stdoutThreshold() const
{
    bcemt_LockGuard<bcemt_Mutex> guard(&d_mutex);
    return d_stdoutThreshold;
}

bool bael_FileObserver::isStdoutLoggingPrefixEnabled() const
{
    bcemt_LockGuard<bcemt_Mutex> guard(&d_mutex);
    return d_useRegularFormatOnStdoutFlag;
}

bool bael_FileObserver::isUserFieldsLoggingEnabled() const
{
    bcemt_LockGuard<bcemt_Mutex> guard(&d_mutex);
    return d_userFieldsLoggingFlag;
}

bool bael_FileObserver::isPublishInLocalTimeEnabled() const
{
    bcemt_LockGuard<bcemt_Mutex> guard(&d_mutex);
    return d_publishInLocalTime;
}

void
bael_FileObserver::getLogFormat(const char **logFileFormat,
                                const char **stdoutFormat) const
{
    bcemt_LockGuard<bcemt_Mutex> guard(&d_mutex);
    *logFileFormat = d_logFileFormatter.format();
    *stdoutFormat  = d_stdoutFormatter.format();
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
