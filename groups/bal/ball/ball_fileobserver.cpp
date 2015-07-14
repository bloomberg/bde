// ball_fileobserver.cpp                                              -*-C++-*-
#include <ball_fileobserver.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ball_fileobserver_cpp,"$Id$ $CSID$")

#include <ball_context.h>
#include <ball_record.h>

#ifdef FOR_TESTING_ONLY
#include <ball_defaultobserver.h>             // for testing only
#include <ball_log.h>                         // for testing only
#include <ball_loggermanager.h>               // for testing only
#include <ball_loggermanagerconfiguration.h>  // for testing only
#include <ball_multiplexobserver.h>           // for testing only
#endif

#include <bdlmtt_lockguard.h>

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

namespace ball {
                          // -----------------------
                          // class FileObserver
                          // -----------------------

// CREATORS
FileObserver::FileObserver(Severity::Level  stdoutThreshold,
                                     bslma::Allocator     *basicAllocator)
: d_logFileFormatter(DEFAULT_LONG_FORMAT,
                     bdlt::DatetimeInterval(0),
                     basicAllocator)
, d_stdoutFormatter(DEFAULT_LONG_FORMAT,
                    bdlt::DatetimeInterval(0),
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

FileObserver::FileObserver(Severity::Level  stdoutThreshold,
                                     bool                  publishInLocalTime,
                                     bslma::Allocator     *basicAllocator)
: d_logFileFormatter(DEFAULT_LONG_FORMAT,
                     publishInLocalTime,
                     basicAllocator)
, d_stdoutFormatter(DEFAULT_LONG_FORMAT,
                    publishInLocalTime,
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
        d_fileObserver2.enablePublishInLocalTime();
    } 
}

FileObserver::~FileObserver()
{
}

// MANIPULATORS
void FileObserver::disableStdoutLoggingPrefix()
{
    bdlmtt::LockGuard<bdlmtt::Mutex> guard(&d_mutex);
    if (false == d_useRegularFormatOnStdoutFlag) {
        return;                                                       // RETURN
    }
    d_useRegularFormatOnStdoutFlag = false;
    d_stdoutFormatter.setFormat(d_stdoutShortFormat.c_str());
}

void FileObserver::disableUserFieldsLogging()
{
    bdlmtt::LockGuard<bdlmtt::Mutex> guard(&d_mutex);
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

void FileObserver::disablePublishInLocalTime()
{
    bdlmtt::LockGuard<bdlmtt::Mutex> guard(&d_mutex);
    d_publishInLocalTime = false;
    d_stdoutFormatter.disablePublishInLocalTime();
    d_logFileFormatter.disablePublishInLocalTime();

    // Unfortunately, this is necessary because 'd_fileObserver2' has a *copy*
    // of the log file formatter.

    d_fileObserver2.setLogFileFunctor(d_logFileFormatter);
}

void FileObserver::enableStdoutLoggingPrefix()
{
    bdlmtt::LockGuard<bdlmtt::Mutex> guard(&d_mutex);
    if (true == d_useRegularFormatOnStdoutFlag) {
        return;                                                       // RETURN
    }
    d_useRegularFormatOnStdoutFlag = true;
    d_stdoutFormatter.setFormat(d_stdoutLongFormat.c_str());
}

void FileObserver::enableUserFieldsLogging()
{
    bdlmtt::LockGuard<bdlmtt::Mutex> guard(&d_mutex);
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

void FileObserver::enablePublishInLocalTime()
{
    bdlmtt::LockGuard<bdlmtt::Mutex> guard(&d_mutex);
    d_publishInLocalTime = true;
    d_stdoutFormatter.enablePublishInLocalTime();
    d_logFileFormatter.enablePublishInLocalTime();

    // Unfortunately, this is necessary because 'd_fileObserver2' has a *copy*
    // of the log file formatter.

    d_fileObserver2.setLogFileFunctor(d_logFileFormatter);
}

void FileObserver::publish(const Record&  record,
                                const Context& context)
{
    bdlmtt::LockGuard<bdlmtt::Mutex> guard(&d_mutex);

    if (record.fixedFields().severity() <= d_stdoutThreshold) {
        bsl::ostringstream oss;
        d_stdoutFormatter(oss, record);

        // Use 'fwrite' to specify the length to write.

        bsl::fwrite(oss.str().c_str(), 1, oss.str().length(), stdout);
        bsl::fflush(stdout);
    }

    d_fileObserver2.publish(record, context);
}

void
FileObserver::setStdoutThreshold(Severity::Level stdoutThreshold)
{
    bdlmtt::LockGuard<bdlmtt::Mutex> guard(&d_mutex);
    d_stdoutThreshold = stdoutThreshold;
}

void
FileObserver::setLogFormat(const char *logFileFormat,
                                const char *stdoutFormat)
{
    bdlmtt::LockGuard<bdlmtt::Mutex> guard(&d_mutex);
    d_stdoutLongFormat = stdoutFormat;
    d_logFileFormatter.setFormat(logFileFormat);
    d_fileObserver2.setLogFileFunctor(d_logFileFormatter);
    d_stdoutFormatter.setFormat(stdoutFormat);
}

// ACCESSORS
Severity::Level FileObserver::stdoutThreshold() const
{
    bdlmtt::LockGuard<bdlmtt::Mutex> guard(&d_mutex);
    return d_stdoutThreshold;
}

bool FileObserver::isStdoutLoggingPrefixEnabled() const
{
    bdlmtt::LockGuard<bdlmtt::Mutex> guard(&d_mutex);
    return d_useRegularFormatOnStdoutFlag;
}

bool FileObserver::isUserFieldsLoggingEnabled() const
{
    bdlmtt::LockGuard<bdlmtt::Mutex> guard(&d_mutex);
    return d_userFieldsLoggingFlag;
}

bool FileObserver::isPublishInLocalTimeEnabled() const
{
    bdlmtt::LockGuard<bdlmtt::Mutex> guard(&d_mutex);
    return d_publishInLocalTime;
}

void
FileObserver::getLogFormat(const char **logFileFormat,
                                const char **stdoutFormat) const
{
    bdlmtt::LockGuard<bdlmtt::Mutex> guard(&d_mutex);
    *logFileFormat = d_logFileFormatter.format();
    *stdoutFormat  = d_stdoutFormatter.format();
}
}  // close package namespace

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2007
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
