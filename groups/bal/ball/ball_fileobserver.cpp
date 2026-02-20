// ball_fileobserver.cpp                                              -*-C++-*-
#include <ball_fileobserver.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ball_fileobserver_cpp,"$Id$ $CSID$")

#include <ball_context.h>
#include <ball_log.h>                         // for testing only
#include <ball_loggermanager.h>               // for testing only
#include <ball_loggermanagerconfiguration.h>  // for testing only
#include <ball_record.h>
#include <ball_streamobserver.h>              // for testing only

#include <bslmt_lockguard.h>

#include <bsl_cstdio.h>
#include <bsl_cstring.h>                      // for 'bsl::strcmp'
#include <bsl_iostream.h>

namespace BloombergLP {
namespace ball {

namespace {

static const char *const k_DEFAULT_LONG_FORMAT =
                                              "\n%d %p:%t %s %f:%l %c %m %u\n";

static const char *const k_DEFAULT_LONG_FORMAT_WITHOUT_USERFIELDS =
                                              "\n%d %p:%t %s %f:%l %c %m\n";

static const char *const k_DEFAULT_SHORT_FORMAT =
                                              "\n%s %f:%l %c %m %u\n";

static const char *const k_DEFAULT_SHORT_FORMAT_WITHOUT_USERFIELDS =
                                              "\n%s %f:%l %c %m\n";
}  // close unnamed namespace

                          // ------------------
                          // class FileObserver
                          // ------------------

// PRIVATE MANIPULATORS
int FileObserver::setFileLogFormatUnlocked(const char *logFileFormat)
{
    return d_fileObserver2.setFormat(logFileFormat);
}

int FileObserver::setStdoutLogFormatUnlocked(const char *stdoutFormat)
{
    const int rc = d_stdoutObserver.setFormat(stdoutFormat);

    // If the short format is in use, make this format the long format and
    // indicate that the long format is in use.  See contract.
    if (0 == rc && !d_stdoutUsesLongFormat) {
        // this is the long format now
        d_stdoutLongFormat = stdoutFormat;

        // enable prefix
        d_stdoutUsesLongFormat = true;
    }

    return rc;
}

// CREATORS
FileObserver::FileObserver()
: d_userFieldsLoggingFlag(true)
, d_stdoutThreshold(Severity::e_WARN)
, d_stdoutUsesLongFormat(true)
, d_stdoutLongFormat(k_DEFAULT_LONG_FORMAT)
, d_stdoutShortFormat(k_DEFAULT_SHORT_FORMAT)
, d_stdoutObserver(stdout)
, d_fileObserver2()
{
    d_fileObserver2.setFormat(k_DEFAULT_LONG_FORMAT);
    d_stdoutObserver.setFormat(k_DEFAULT_LONG_FORMAT);
}

FileObserver::FileObserver(bslma::Allocator *basicAllocator)
: d_userFieldsLoggingFlag(true)
, d_stdoutThreshold(Severity::e_WARN)
, d_stdoutUsesLongFormat(true)
, d_stdoutLongFormat(k_DEFAULT_LONG_FORMAT, basicAllocator)
, d_stdoutShortFormat(k_DEFAULT_SHORT_FORMAT, basicAllocator)
, d_stdoutObserver(stdout, basicAllocator)
, d_fileObserver2(basicAllocator)
{
    d_stdoutObserver.setFormat(k_DEFAULT_LONG_FORMAT);
    d_fileObserver2.setFormat(k_DEFAULT_LONG_FORMAT);
}

FileObserver::FileObserver(Severity::Level   stdoutThreshold,
                           bslma::Allocator *basicAllocator)
: d_userFieldsLoggingFlag(true)
, d_stdoutThreshold(stdoutThreshold)
, d_stdoutUsesLongFormat(true)
, d_stdoutLongFormat(k_DEFAULT_LONG_FORMAT, basicAllocator)
, d_stdoutShortFormat(k_DEFAULT_SHORT_FORMAT, basicAllocator)
, d_stdoutObserver(stdout, basicAllocator)
, d_fileObserver2(basicAllocator)
{
    d_stdoutObserver.setFormat(k_DEFAULT_LONG_FORMAT);
    d_fileObserver2.setFormat(k_DEFAULT_LONG_FORMAT);
}

FileObserver::FileObserver(Severity::Level   stdoutThreshold,
                           bool              publishInLocalTime,
                           bslma::Allocator *basicAllocator)
: d_userFieldsLoggingFlag(true)
, d_stdoutThreshold(stdoutThreshold)
, d_stdoutUsesLongFormat(true)
, d_stdoutLongFormat(k_DEFAULT_LONG_FORMAT, basicAllocator)
, d_stdoutShortFormat(k_DEFAULT_SHORT_FORMAT, basicAllocator)
, d_stdoutObserver(stdout, basicAllocator)
, d_fileObserver2(basicAllocator)
{
    if (publishInLocalTime) {
        d_stdoutObserver.enablePublishInLocalTime();
        d_fileObserver2.enablePublishInLocalTime();
    }
    d_stdoutObserver.setFormat(k_DEFAULT_LONG_FORMAT);
    d_fileObserver2.setFormat(k_DEFAULT_LONG_FORMAT);
}

FileObserver::~FileObserver()
{
}

// MANIPULATORS
void FileObserver::disableStdoutLoggingPrefix()
{
    bslmt::LockGuard<bslmt::Mutex> guard(&d_mutex);

    if (false == d_stdoutUsesLongFormat) {
        return;                                                       // RETURN
    }

    if (0 == d_stdoutObserver.setFormat(d_stdoutShortFormat.c_str())) {
        d_stdoutUsesLongFormat = false;
    }
}

void FileObserver::disableUserFieldsLogging()
{
    bslmt::LockGuard<bslmt::Mutex> guard(&d_mutex);

    if (false == d_userFieldsLoggingFlag) {
        return;                                                       // RETURN
    }

    d_userFieldsLoggingFlag = false;

    if (d_stdoutObserver.getFormat() ==
                   (d_stdoutUsesLongFormat ? k_DEFAULT_LONG_FORMAT
                                           : k_DEFAULT_SHORT_FORMAT)) {
        (void)d_stdoutObserver.setFormat(
                                  d_stdoutUsesLongFormat
                                  ? k_DEFAULT_LONG_FORMAT_WITHOUT_USERFIELDS
                                  : k_DEFAULT_SHORT_FORMAT_WITHOUT_USERFIELDS);
    }

    if (d_fileObserver2.getFormat() == k_DEFAULT_LONG_FORMAT) {
        (void)d_fileObserver2.setFormat(
                                     k_DEFAULT_LONG_FORMAT_WITHOUT_USERFIELDS);
    }
}

void FileObserver::disablePublishInLocalTime()
{
    bslmt::LockGuard<bslmt::Mutex> guard(&d_mutex);

    d_stdoutObserver.disablePublishInLocalTime();
    d_fileObserver2.disablePublishInLocalTime();
}

void FileObserver::enableStdoutLoggingPrefix()
{
    bslmt::LockGuard<bslmt::Mutex> guard(&d_mutex);

    if (true == d_stdoutUsesLongFormat) {
        return;                                                       // RETURN
    }

    d_stdoutUsesLongFormat = true;
    // We need to recreate the formatter to enable the new format
    d_stdoutObserver.setFormat(d_stdoutLongFormat.c_str());
}

void FileObserver::enableUserFieldsLogging()
{
    bslmt::LockGuard<bslmt::Mutex> guard(&d_mutex);

    if (true == d_userFieldsLoggingFlag) {
        return;                                                       // RETURN
    }

    d_userFieldsLoggingFlag = true;

    if (d_stdoutObserver.getFormat() ==
                         (d_stdoutUsesLongFormat
                             ? k_DEFAULT_LONG_FORMAT_WITHOUT_USERFIELDS
                             : k_DEFAULT_SHORT_FORMAT_WITHOUT_USERFIELDS)) {
        (void)d_stdoutObserver.setFormat(d_stdoutUsesLongFormat
                                       ? k_DEFAULT_LONG_FORMAT
                                      : k_DEFAULT_SHORT_FORMAT);
    }

    if (d_fileObserver2.getFormat() ==
                                    k_DEFAULT_LONG_FORMAT_WITHOUT_USERFIELDS) {
        (void)d_fileObserver2.setFormat(k_DEFAULT_LONG_FORMAT);
    }
}

void FileObserver::enablePublishInLocalTime()
{
    bslmt::LockGuard<bslmt::Mutex> guard(&d_mutex);

    d_stdoutObserver.enablePublishInLocalTime();
    d_fileObserver2.enablePublishInLocalTime();
}

void FileObserver::publish(const bsl::shared_ptr<const Record>& record,
                           const Context&                       context)
{
    bslmt::LockGuard<bslmt::Mutex> guard(&d_mutex);

    if (record->fixedFields().severity() <= d_stdoutThreshold) {
        d_stdoutObserver.publish(record, context);
    }

    d_fileObserver2.publish(record, context);
}

int FileObserver::setLogFormats(const char *logFileFormat,
                                const char *stdoutFormat)
{
    bslmt::LockGuard<bslmt::Mutex> guard(&d_mutex);

    const int rv1 = setFileLogFormatUnlocked(logFileFormat);
    const int rv2 = setStdoutLogFormatUnlocked(stdoutFormat);

    return rv1 != 0 && rv2 == 0
         ? -1  // file log format error
         : rv1 == 0 && rv2 != 0
         ? -2  // stdout log format error
         : rv1 != 0 && rv2 != 0
         ? -3  // both log formats error
         : 0;  // both success
}

int FileObserver::setFileLogFormat(const char *logFileFormat)
{
    bslmt::LockGuard<bslmt::Mutex> guard(&d_mutex);

    return setFileLogFormatUnlocked(logFileFormat);
}

int FileObserver::setStdoutLogFormat(const char *stdoutFormat)
{
    bslmt::LockGuard<bslmt::Mutex> guard(&d_mutex);

    return setStdoutLogFormatUnlocked(stdoutFormat);
}

void FileObserver::setStdoutThreshold(Severity::Level stdoutThreshold)
{
    bslmt::LockGuard<bslmt::Mutex> guard(&d_mutex);

    d_stdoutThreshold = stdoutThreshold;
}

// ACCESSORS
bslma::Allocator *FileObserver::allocator() const
{
    return d_stdoutLongFormat.get_allocator().mechanism();
}

void FileObserver::getLogFormat(const char **logFileFormat,
                                const char **stdoutFormat) const
{
    bslmt::LockGuard<bslmt::Mutex> guard(&d_mutex);

    *logFileFormat = d_fileObserver2.getFormat().c_str();
    *stdoutFormat  = d_stdoutObserver.getFormat().c_str();
}

const bsl::string& FileObserver::getFileLogFormat() const
{
    bslmt::LockGuard<bslmt::Mutex> guard(&d_mutex);

    return d_fileObserver2.getFormat();
}

const bsl::string& FileObserver::getStdoutLogFormat() const
{
    bslmt::LockGuard<bslmt::Mutex> guard(&d_mutex);

    return d_stdoutObserver.getFormat();
}

bool FileObserver::isPublishInLocalTimeEnabled() const
{
    bslmt::LockGuard<bslmt::Mutex> guard(&d_mutex);

    return d_stdoutObserver.isPublishInLocalTimeEnabled();
}

bool FileObserver::isStdoutLoggingPrefixEnabled() const
{
    bslmt::LockGuard<bslmt::Mutex> guard(&d_mutex);

    return d_stdoutUsesLongFormat;
}

bool FileObserver::isUserFieldsLoggingEnabled() const
{
    bslmt::LockGuard<bslmt::Mutex> guard(&d_mutex);

    return d_userFieldsLoggingFlag;
}

Severity::Level FileObserver::stdoutThreshold() const
{
    bslmt::LockGuard<bslmt::Mutex> guard(&d_mutex);

    return d_stdoutThreshold;
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
