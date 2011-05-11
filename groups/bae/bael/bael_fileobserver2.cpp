// bael_fileobserver2.cpp                                             -*-C++-*-
#include <bael_fileobserver2.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bael_fileobserver2_cpp,"$Id$ $CSID$")

#include <bael_context.h>
#include <bael_record.h>
#include <bael_recordattributes.h>

#ifdef BDE_FOR_TESTING_ONLY
#include <bael_defaultobserver.h>             // for testing only
#include <bael_log.h>                         // for testing only
#include <bael_loggermanager.h>               // for testing only
#include <bael_loggermanagerconfiguration.h>  // for testing only
#include <bael_multiplexobserver.h>           // for testing only
#include <bael_recordstringformatter.h>       // for testing only
#endif

#include <bcemt_lockguard.h>

#include <bdem_list.h>

#include <bdetu_datetime.h>
#include <bdetu_systemtime.h>

#include <bdef_memfn.h>

#include <bsls_assert.h>
#include <bsls_platform.h>

#include <bsl_cmath.h>
#include <bsl_cstdio.h>
#include <bsl_cstring.h>
#include <bsl_iomanip.h>
#include <bsl_iosfwd.h>    // for 'bsl::streamoff' type
#include <bsl_iostream.h>
#include <bsl_sstream.h>

#include <bsl_c_errno.h>
#include <bsl_c_time.h>

#include <bsl_c_stdio.h>   // for 'snprintf'

#ifdef BSLS_PLATFORM__OS_UNIX
#include <sys/types.h>
#include <sys/stat.h>
#endif

#ifdef BSLS_PLATFORM__OS_WINDOWS
#include <windows.h>
#endif

static const char *const months[] = {
    0,
    "JAN", "FEB", "MAR", "APR",
    "MAY", "JUN", "JUL", "AUG",
    "SEP", "OCT", "NOV", "DEC"
};

namespace {

using namespace BloombergLP;

int getErrorCode(void)
    // Return the system-specific error code.
{
#ifdef BSLS_PLATFORM__OS_WINDOWS
    int rc = GetLastError();
    return rc ? rc : errno;
#else
    return errno;
#endif
}

bsl::string getTimestampSuffix(const bdet_Datetime& timestamp)
    // Return the specified 'timestamp' in the '.YYYYMMDD_hhmmss' format.
{
    char buffer[20];

#if defined(BSLS_PLATFORM__CMP_MSVC)
#define snprintf _snprintf
#endif

    snprintf(buffer,
             sizeof buffer,
             ".%4d%02d%02d_%02d%02d%02d",
             timestamp.year(),
             timestamp.month(),
             timestamp.day(),
             timestamp.hour(),
             timestamp.minute(),
             timestamp.second());

#if defined(BSLS_PLATFORM__CMP_MSVC)
#undef snprintf
#endif

    return bsl::string(buffer);
}

bool compareLogFileTimes(const bsl::pair<bsl::string, time_t>& lhs,
                         const bsl::pair<bsl::string, time_t>& rhs)
    // Return 'true' if the datetime field of the specified 'lhs' is later
    // than that of the specified 'rhs', and 'false' otherwise.
{
    return lhs.second > rhs.second;
}

void getLogFileName(bsl::string          *logFileName,
                    bool                 *datetimeInfoInFileName,
                    const char           *logFilePattern,
                    const bdet_Datetime&  logFileTimestamp)
    // Load into the specified '*logFileName' that is obtained by replacing
    // every timestamp-specific token in the specified 'logFilePattern' with
    // the corresponding field in the specified 'logFileTimestamp'.  Load
    // 'true' into the specified '*datetimeInfoInFileName' if 'logFilePattern'
    // contains at least one timestamp-specific token, and 'false' otherwise.
{
    bsl::ostringstream os;
    *datetimeInfoInFileName = false;

    for (; *logFilePattern; ++logFilePattern) {
        if ('%' == *logFilePattern) {
            if (*++logFilePattern) {
                switch (*logFilePattern) {
                  case 'Y': {
                    os << bsl::setw(4) << bsl::setfill('0')
                       << logFileTimestamp.year();
                    *datetimeInfoInFileName = true;
                  } break;
                  case 'M': {
                    os << bsl::setw(2) << bsl::setfill('0')
                       << logFileTimestamp.month();
                    *datetimeInfoInFileName = true;
                  } break;
                  case 'D': {
                    os << bsl::setw(2) << bsl::setfill('0')
                       << logFileTimestamp.day();
                    *datetimeInfoInFileName = true;
                  } break;
                  case 'h': {
                    os << bsl::setw(2) << bsl::setfill('0')
                       << logFileTimestamp.hour();
                    *datetimeInfoInFileName = true;
                  } break;
                  case 'm': {
                    os << bsl::setw(2) << bsl::setfill('0')
                       << logFileTimestamp.minute();
                    *datetimeInfoInFileName = true;
                  } break;
                  case 's': {
                    os << bsl::setw(2) << bsl::setfill('0')
                       << logFileTimestamp.second();
                    *datetimeInfoInFileName = true;
                  } break;
                  case '%': {
                    *datetimeInfoInFileName = true;
                  } break;
                  default: {
                    os << '%' << *logFilePattern;
                  } break;
                }
            }
            else {
                os << '%';  // trailing '%' in pattern
                break;
            }
        } else {
            os << *logFilePattern;
        }
    }
    *logFileName = os.str();
}

}  // close unnamed namespace

namespace BloombergLP {

                          // ------------------------
                          // class bael_FileObserver2
                          // ------------------------

// PRIVATE MANIPULATORS
void bael_FileObserver2::logRecordDefault(bsl::ostream&      stream,
                                          const bael_Record& record)

{
    const bael_RecordAttributes& fixedFields = record.fixedFields();

    bdet_Datetime timestamp = fixedFields.timestamp();
    if (d_publishInLocalTime) {
        timestamp += d_localTimeOffset;
    }

    bsl::string output;
    output.reserve(1024);
    char buffer[256];

    int y, m, d;
    timestamp.date().getYearMonthDay(&y, &m, &d);

    const char *const month = months[m];
    int hour, min, sec, mSec;
    timestamp.time().getTime(&hour, &min, &sec, &mSec);

#if defined(BSLS_PLATFORM__CMP_MSVC)
#define snprintf _snprintf
#endif

    snprintf(buffer,
             sizeof(buffer),
             "\n%02d%s%04d_%02d:%02d:%02d.%03d "
             "%d:%llu %s %s:%d ",
             d,
             month,
             y,
             hour,
             min,
             sec,
             mSec,
             fixedFields.processID(),
             fixedFields.threadID(),
             bael_Severity::toAscii(
                                 (bael_Severity::Level)fixedFields.severity()),
             fixedFields.fileName(),
             fixedFields.lineNumber());

#if defined(BSLS_PLATFORM__CMP_MSVC)
#undef snprintf
#endif

    output += buffer;
    output += fixedFields.category();
    output += ' ';
    output += fixedFields.message();
    output += ' ';

    const bdem_List& userFields = record.userFields();
    const int numUserFields = userFields.length();

    stream << output;

    for (int i = 0; i < numUserFields; ++i) {
        stream << userFields[i] << ' ';
    }

    stream << '\n';

    stream.flush();
}

int bael_FileObserver2::openLogFile(bool rollFileIfExistFlag)
{
    d_logFileTimestamp = bdetu_SystemTime::nowAsDatetimeGMT();

    const bdet_Datetime localTimestamp =
                                        d_logFileTimestamp + d_localTimeOffset;

    getLogFileName(&d_logFileName,
                   &d_datetimeInfoInFileName,
                   d_logFilePattern.c_str(),
                   localTimestamp);

    if (d_isOpenWithTimestampFlag && !d_datetimeInfoInFileName) {
        d_logFileName += getTimestampSuffix(localTimestamp);
    }

    bool fileExistFlag = bdesu_FileUtil::exists(d_logFileName.c_str());
    if (rollFileIfExistFlag && fileExistFlag) {
        const int MAX_ROTATED_FILE_SUFFIX = 256;

        bdesu_FileUtil::rollFileChain(d_logFileName.c_str(),
                                      MAX_ROTATED_FILE_SUFFIX);
        fileExistFlag = false;
    }
    bdesu_FileUtil::FileDescriptor fd =  bdesu_FileUtil::open(
                                                         d_logFileName.c_str(),
                                                         true,
                                                         fileExistFlag,
                                                         true);
    d_logStreamBuf.reset(fd, true, true, true);

    if (d_logStreamBuf.isOpened()) {
        d_logFileStream.clear();
        return 0;                                                     // RETURN
    }

    fprintf(stderr,
            "Cannot open log file %s: %s.  File logging will be disabled!\n",
            d_logFileName.c_str(), bsl::strerror(getErrorCode()));

    return -1;
}

void bael_FileObserver2::rotateFile()
{
    if (!d_logStreamBuf.isOpened()) {
        return;                                                       // RETURN
    }

    BSLS_ASSERT(d_logFilePattern.size() > 0);

    d_logStreamBuf.clear();

    if (!d_isOpenWithTimestampFlag && !d_datetimeInfoInFileName) {

        bsl::string newFileName(d_logFileName);
        newFileName +=
                    getTimestampSuffix(d_logFileTimestamp + d_localTimeOffset);

        if (0 != bsl::rename(d_logFileName.c_str(), newFileName.c_str())) {
            fprintf(stderr, "Cannot rename %s to %s: %s\n",
                    d_logFileName.c_str(), newFileName.c_str(),
                    bsl::strerror(getErrorCode()));
        }
    }

    openLogFile(true);
}

void bael_FileObserver2::rotateIfNecessary(const bdet_Datetime& timestamp)
{
    BSLS_ASSERT(d_rotationSize >= 0);
    BSLS_ASSERT(d_rotationLifetime.totalSeconds() >= 0);

    if (!d_logStreamBuf.isOpened()) {
        return;                                                       // RETURN
    }

    if (d_rotationSize) {

        // 'tellp' returns -1 on failure.  Rotate the log file if either
        // 'tellp' fails, or the rotation size is exceeded.

        if (static_cast<bsls_Types::Uint64>(d_logFileStream.tellp()) >
            static_cast<bsls_Types::Uint64>(d_rotationSize) * 1024) {

            rotateFile();
            return;                                                   // RETURN
        }
    }

    if (d_rotationLifetime.totalSeconds()
                   && (timestamp - d_logFileTimestamp) >= d_rotationLifetime) {
        rotateFile();
        return;                                                       // RETURN
    }
}

// CREATORS
bael_FileObserver2::bael_FileObserver2(bslma_Allocator *basicAllocator)
: d_logStreamBuf(bdesu_FileUtil::INVALID_FD, false)
, d_logFileStream(&d_logStreamBuf)
, d_logFilePattern(basicAllocator)
, d_logFileName(basicAllocator)
, d_datetimeInfoInFileName(false)
, d_isOpenWithTimestampFlag(false)
, d_logFileFunctor(
            bdef_MemFnUtil::memFn(&bael_FileObserver2::logRecordDefault, this),
            basicAllocator)
, d_localTimeOffset(bdetu_SystemTime::localTimeOffset())
, d_publishInLocalTime(false)
, d_rotationSize(0)
, d_rotationLifetime(0)
{
}

bael_FileObserver2::~bael_FileObserver2()
{
    if (d_logStreamBuf.isOpened()) {
        d_logStreamBuf.clear();
    }
}

// MANIPULATORS
void bael_FileObserver2::disableFileLogging()
{
    bcemt_LockGuard<bcemt_Mutex> guard(&d_mutex);
    if (d_logStreamBuf.isOpened()) {
        d_logStreamBuf.clear();
    }
}

void bael_FileObserver2::disableLifetimeRotation()
{
    bcemt_LockGuard<bcemt_Mutex> guard(&d_mutex);
    d_rotationLifetime.setTotalSeconds(0);
}

void bael_FileObserver2::disableSizeRotation()
{
    bcemt_LockGuard<bcemt_Mutex> guard(&d_mutex);
    d_rotationSize = 0;
}

void bael_FileObserver2::disablePublishInLocalTime()
{
    bcemt_LockGuard<bcemt_Mutex> guard(&d_mutex);
    d_publishInLocalTime = false;
}

int bael_FileObserver2::enableFileLogging(const char *fileNamePattern,
                                          bool        timestampFlag)
{
    BSLS_ASSERT(fileNamePattern);

    bcemt_LockGuard<bcemt_Mutex> guard(&d_mutex);
    if (d_logStreamBuf.isOpened()) {
        return 1;                                                     // RETURN
    }
    d_logFilePattern = fileNamePattern;
    d_isOpenWithTimestampFlag = timestampFlag;
    return openLogFile();
}

void bael_FileObserver2::forceRotation()
{
    bcemt_LockGuard<bcemt_Mutex> guard(&d_mutex);
    rotateFile();
}

void bael_FileObserver2::enablePublishInLocalTime()
{
    bcemt_LockGuard<bcemt_Mutex> guard(&d_mutex);
    d_publishInLocalTime = true;
}

void bael_FileObserver2::publish(const bael_Record&  record,
                                 const bael_Context& )
{
    bcemt_LockGuard<bcemt_Mutex> guard(&d_mutex);
    rotateIfNecessary(record.fixedFields().timestamp());

    if (d_logStreamBuf.isOpened()) {
        d_logFileFunctor(d_logFileStream, record);

        if (!d_logFileStream) {
            fprintf(stderr, "Error on file stream for %s: %s\n",
                    d_logFileName.c_str(), bsl::strerror(getErrorCode()));

            d_logStreamBuf.clear();
        }
    }
}

void bael_FileObserver2::rotateOnLifetime(
                                     const bdet_DatetimeInterval& timeInterval)
{
    const int totalSeconds = timeInterval.totalSeconds();
    BSLS_ASSERT(totalSeconds > 0);

    bcemt_LockGuard<bcemt_Mutex> guard(&d_mutex);
    d_rotationLifetime.setTotalSeconds(totalSeconds);
}

void bael_FileObserver2::rotateOnSize(int size)
{
    BSLS_ASSERT(size > 0);

    bcemt_LockGuard<bcemt_Mutex> guard(&d_mutex);
    d_rotationSize = size;
}

void
bael_FileObserver2::setLogFileFunctor(const LogRecordFunctor& logFileFunctor)
{
    bcemt_LockGuard<bcemt_Mutex> guard(&d_mutex);
    d_logFileFunctor = logFileFunctor;
}

// ACCESSORS
bool bael_FileObserver2::isFileLoggingEnabled() const
{
    bcemt_LockGuard<bcemt_Mutex> guard(&d_mutex);

    // The standard says that 'fstream::is_open()' is a non-'const' method.
    // However, 'streambuf::is_open()' is 'const'.

    return d_logStreamBuf.isOpened();
}

bool bael_FileObserver2::isFileLoggingEnabled(bsl::string *result) const
{
    BSLS_ASSERT(result);

    bcemt_LockGuard<bcemt_Mutex> guard(&d_mutex);

    // The standard says that 'fstream::is_open()' is a non-'const' method.
    // However, 'streambuf::is_open()' is 'const'.

    bool rc = d_logStreamBuf.isOpened();
    if (rc) {
        result->assign(d_logFileName);
    }

    return rc;
}

bool bael_FileObserver2::isPublishInLocalTimeEnabled() const
{
    bcemt_LockGuard<bcemt_Mutex> guard(&d_mutex);
    return d_publishInLocalTime;
}

bdet_DatetimeInterval bael_FileObserver2::rotationLifetime() const
{
    bcemt_LockGuard<bcemt_Mutex> guard(&d_mutex);
    return d_rotationLifetime;
}

int bael_FileObserver2::rotationSize() const
{
    bcemt_LockGuard<bcemt_Mutex> guard(&d_mutex);
    return d_rotationSize;
}

bdet_DatetimeInterval bael_FileObserver2::localTimeOffset() const
{
    bcemt_LockGuard<bcemt_Mutex> guard(&d_mutex);
    return d_localTimeOffset;
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
