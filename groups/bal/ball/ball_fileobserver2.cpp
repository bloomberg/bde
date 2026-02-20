// ball_fileobserver2.cpp                                             -*-C++-*-
#include <ball_fileobserver2.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ball_fileobserver2_cpp,"$Id$ $CSID$")

#include <ball_context.h>
#include <ball_record.h>
#include <ball_recordattributes.h>
#include <ball_userfields.h>
#include <ball_userfieldvalue.h>

#include <ball_log.h>                         // for testing only
#include <ball_loggermanager.h>               // for testing only
#include <ball_loggermanagerconfiguration.h>  // for testing only
#include <ball_recordstringformatter.h>       // for testing only
#include <ball_streamobserver.h>              // for testing only

#include <bdlf_memfn.h>

#include <bdls_filesystemutil.h>
#include <bdls_processutil.h>

#include <bdlt_currenttime.h>
#include <bdlt_date.h>
#include <bdlt_intervalconversionutil.h>
#include <bdlt_localtimeoffset.h>
#include <bdlt_time.h>

#include <bslmt_lockguard.h>

#include <bsls_assert.h>
#include <bsls_log.h>
#include <bsls_platform.h>
#include <bsls_types.h>

#include <bslstl_stringref.h>

#include <bsl_cstdio.h>
#include <bsl_cstring.h>
#include <bsl_format.h>
#include <bsl_iomanip.h>
#include <bsl_ios.h>
#include <bsl_memory.h>
#include <bsl_ostream.h>
#include <bsl_sstream.h>

#include <bsl_c_errno.h>
#include <bsl_c_time.h>
#include <bsl_c_stdio.h>   // for 'snprintf'

#ifdef BSLS_PLATFORM_OS_UNIX
#include <sys/types.h>
#include <sys/stat.h>
#endif

#ifdef BSLS_PLATFORM_OS_WINDOWS
#include <windows.h>
#endif

#if defined(BSLS_PLATFORM_CMP_MSVC)
#define snprintf _snprintf
#endif

namespace BloombergLP {
namespace ball {

namespace {

enum {
    // status code for the call back function.
    // Can be ORed together if multiple errors occur.
    k_ROTATE_SUCCESS                  =  0,
    k_ROTATE_RENAME_ERROR             =  1,
    k_ROTATE_NEW_LOG_ERROR            =  2
};

enum {
    k_ERROR_BUFFER_SIZE   = 1280  // 1024 (max path) + 256
};

#define LOG_PLATFORM_MESSAGE(severity, formatStr, ...) \
    {\
        char message[k_ERROR_BUFFER_SIZE]; \
        snprintf(message, sizeof(message), formatStr, __VA_ARGS__); \
        bsls::Log::platformDefaultMessageHandler( \
            severity, __FILE__, __LINE__, message); \
    }

/// Return the system-specific error code.
static int getErrorCode(void)
{
#ifdef BSLS_PLATFORM_OS_WINDOWS
    int rc = GetLastError();
    return rc ? rc : errno;
#else
    return errno;
#endif
}

/// Return the specified `timestamp` in the `YYYYMMDD_hhmmss` format.
static bsl::string getTimestampSuffix(const bdlt::Datetime& timestamp)
{
    char buffer[16];

    snprintf(buffer,
             sizeof buffer,
             "%04d%02d%02d_%02d%02d%02d",
             timestamp.year(),
             timestamp.month(),
             timestamp.day(),
             timestamp.hour(),
             timestamp.minute(),
             timestamp.second());

    return bsl::string(buffer);
}

/// Load, into the specified `logFileName`, the filename that is obtained by
/// replacing every `%`-escape sequence in the specified `logFilePattern`.
/// If the specified `publishInLocalTime` is `true`, replace the time
/// patterns with local time values, and replace them with UTC time values
/// otherwise.  Load the current time in UTC into the specified
/// `timestampUtc`.
static void getLogFileName(bsl::string    *logFileName,
                           bdlt::Datetime *timestampUtc,
                           const char     *logFilePattern,
                           bool            publishInLocalTime)
{
    BSLS_ASSERT(logFileName);
    BSLS_ASSERT(timestampUtc);
    BSLS_ASSERT(logFilePattern);

    *timestampUtc = bdlt::CurrentTime::utc();

    bdlt::Datetime logFileTimestamp = *timestampUtc;

    if (publishInLocalTime) {
        logFileTimestamp +=
            bdlt::LocalTimeOffset::localTimeOffset(*timestampUtc);
    }

    bsl::ostringstream os;

    for (; *logFilePattern; ++logFilePattern) {
        if ('%' == *logFilePattern) {
            if (*++logFilePattern) {
                switch (*logFilePattern) {
                  case 'T': {
                    os << getTimestampSuffix(logFileTimestamp);
                  } break;
                  case 'Y': {
                    os << bsl::setw(4) << bsl::setfill('0')
                       << logFileTimestamp.year();
                  } break;
                  case 'M': {
                    os << bsl::setw(2) << bsl::setfill('0')
                       << logFileTimestamp.month();
                  } break;
                  case 'D': {
                    os << bsl::setw(2) << bsl::setfill('0')
                       << logFileTimestamp.day();
                  } break;
                  case 'h': {
                    os << bsl::setw(2) << bsl::setfill('0')
                       << logFileTimestamp.hour();
                  } break;
                  case 'm': {
                    os << bsl::setw(2) << bsl::setfill('0')
                       << logFileTimestamp.minute();
                  } break;
                  case 's': {
                    os << bsl::setw(2) << bsl::setfill('0')
                       << logFileTimestamp.second();
                  } break;
                  case 'p': {
                    os << bdls::ProcessUtil::getProcessId();
                  } break;
                  case '%': {
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

/// Return `true` if the specified `logFilePattern` contains a recognized
/// `%`-escape sequence, and false otherwise.  The recognized escape
/// sequence are "%Y", "%M", "%D", "%h", "%m", "%s", and "%%".
static bool hasEscapePattern(const char *logFilePattern)

{
    for (; *logFilePattern; ++logFilePattern) {
        if ('%' == *logFilePattern) {
            ++logFilePattern;
            if ('\0' == *logFilePattern) {
                return false;                                         // RETURN
            }

            switch (*logFilePattern) {
              case 'Y':
              case 'M':
              case 'D':
              case 'h':
              case 'm':
              case 's':
              case '%': {
                return true;                                          // RETURN
              } break;
            }
        }
    }
    return false;
}

/// Open a file stream referred to by the specified `stream` for the file
/// with the specified `filename` in append mode.  Return 0 on success, and
/// a non-zero value otherwise.
static int openLogFile(bsl::ostream *stream, const char *filename)
{
    BSLS_ASSERT(stream);
    BSLS_ASSERT(filename);

    typedef bdls::FilesystemUtil FileUtil;

    const bool fileExistFlag = FileUtil::exists(filename);

    FileUtil::FileDescriptor fd =  FileUtil::open(filename,
                                                  FileUtil::e_OPEN_OR_CREATE,
                                                  FileUtil::e_READ_APPEND,
                                                  FileUtil::e_KEEP);

    if (fd == FileUtil::k_INVALID_FD) {
        LOG_PLATFORM_MESSAGE(bsls::LogSeverity::e_ERROR,
                             "Cannot open log file %s: %s. "
                             "File logging will be disabled!",
                             filename,
                             bsl::strerror(getErrorCode()));
        return -1;                                                    // RETURN
    }

    bdls::FdStreamBuf *streamBuf = dynamic_cast<bdls::FdStreamBuf *>(
                                                              stream->rdbuf());
    BSLS_ASSERT(streamBuf);

    if (0 != streamBuf->reset(fd, true, true, true)) {
        LOG_PLATFORM_MESSAGE(bsls::LogSeverity::e_WARN,
                             "Cannot close previous log file %s: %s. "
                             "File logging will be disabled!",
                             filename,
                             bsl::strerror(getErrorCode()));

        return -1;                                                    // RETURN
    }

    if (fileExistFlag) {
        // Set the put pointer to the end of the file because the put pointer
        // may not be set until the first output on some platforms.

        stream->seekp(0, bsl::ios::end);
    }

    stream->clear();
    return 0;
}

/// Return `true` if the specified `a` and `b` times are within 10% of the
/// specified `interval` from each other, and `false` otherwise.  The
/// behavior is undefined unless `0 <= interval.totalMilliseconds()`.
bool fuzzyEqual(const bdlt::Datetime&         a,
                const bdlt::Datetime&         b,
                const bdlt::DatetimeInterval& interval)
{
    BSLS_ASSERT(0 <= interval.totalMilliseconds());

    // Note that 'abs(long long)' not available across platforms (C++11).

    bsls::Types::Int64 distance = (a - b).totalMilliseconds();

    if (distance < 0) {
        distance = -distance;
    }

    return distance < (interval.totalMilliseconds() / 10);
}

/// Return the UTC time for the next scheduled file rotation after the
/// specified `fileCreationTimeUtc`, for a schedule that has a start
/// reference time indicated by the specified `referenceStartTime` and
/// rotated every specified `interval`.  If the specified
/// `referenceStartTimeInLocalTime` is `true`, the `referenceStartTime`
/// interpreted as local time value, and as UTC time value otherwise.
/// `fileCreationTimeUtc` must be a UTC time value.  The behavior is
/// undefined unless `0 <= interval.totalMilliseconds()`.
static bdlt::Datetime computeNextRotationTime(
                   const bdlt::Datetime&         referenceStartTime,
                   bool                          referenceStartTimeInLocalTime,
                   const bdlt::DatetimeInterval& interval,
                   const bdlt::Datetime&         fileCreationTimeUtc)
{
    BSLS_ASSERT(0 < interval.totalMilliseconds());

    bdlt::Datetime fileCreationTime(fileCreationTimeUtc);

    if (referenceStartTimeInLocalTime) {
        fileCreationTime +=
            bdlt::LocalTimeOffset::localTimeOffset(fileCreationTimeUtc);
    }

    // If the reference start time is (effectively) equal to the file creation
    // time, don't rotate until at least one interval has occurred.  A fuzzy
    // comparison is required because the time stamps come from different
    // sources, which may occur in close proximity during the configuration of
    // logging at task startup (the 'fileCreationTime' is determined when
    // logging is enabled, while the 'referenceStartTime' may be
    // determined on a call to 'rotateOnTimeInterval').

    if (fuzzyEqual(referenceStartTime, fileCreationTime, interval)) {
        return fileCreationTimeUtc + interval;                        // RETURN
    }

    bsls::Types::Int64 timeLeft =
       (fileCreationTime - referenceStartTime).totalMilliseconds() %
       interval.totalMilliseconds();

    // The modulo operator may return a negative number depending on
    // implementation.

    if (timeLeft >= 0) {
        timeLeft = interval.totalMilliseconds() - timeLeft;
    }
    else {
        timeLeft = -timeLeft;
    }

    bdlt::Datetime resultUtc = fileCreationTimeUtc;
    resultUtc.addMilliseconds(timeLeft);

    return resultUtc;
}

}  // close unnamed namespace

                          // -------------------
                          // class FileObserver2
                          // -------------------

// PRIVATE MANIPULATORS
int FileObserver2::rotateFile(bsl::string *rotatedLogFileName)
{
    BSLS_ASSERT(rotatedLogFileName);

    if (!d_logStreamBuf.isOpened()) {
        return 1;                                                     // RETURN
    }

    BSLS_ASSERT(d_logFilePattern.size() > 0);

    int returnStatus = k_ROTATE_SUCCESS;

    // Close current log file.
    if (0 != d_logStreamBuf.clear()) {
        LOG_PLATFORM_MESSAGE(bsls::LogSeverity::e_WARN,
                             "Unable to close old log file: %s.",
                             d_logFileName.c_str());
        returnStatus |= k_ROTATE_RENAME_ERROR;
    }

    *rotatedLogFileName = d_logFileName;

    const bdlt::Datetime oldLogFileTimestamp = d_logFileTimestampUtc;

    // Get the new log file name based on current time.
    getLogFileName(&d_logFileName,
                   &d_logFileTimestampUtc,
                   d_logFilePattern.c_str(),
                   d_observerFormatterImp.getTimezoneDefault()
                                          == RecordFormatterTimezone::e_LOCAL);

    // If the `d_suppressUniqueFileName` is `false`, the new log file must have
    // a unique name. Let's check the file existence and rename it if needed.
    if (!d_suppressUniqueFileName) {
        // Note that the new name can be the same as the old name if the log
        // file pattern does not contain any time-related specifiers, or if the
        // current time is the same as the time when the log file was opened.
        // In this case, we try to rename the log file by appending an
        // additional timestamp suffix to the old log file name.
        if (bdls::FilesystemUtil::exists(d_logFileName)) {
            bdlt::Datetime timeStampSuffix(oldLogFileTimestamp);

            if (d_observerFormatterImp.getTimezoneDefault()
                                         == RecordFormatterTimezone::e_LOCAL) {
                timeStampSuffix += bdlt::LocalTimeOffset::localTimeOffset(
                                                          oldLogFileTimestamp);
            }

            bsl::string filenameWithTimestamp = bsl::format(
                                          "{}.{}",
                                          d_logFileName,
                                          getTimestampSuffix(timeStampSuffix));

            // If file with an appended timestamp still does exist, we skip the
            // rotation.
            if (bdls::FilesystemUtil::exists(filenameWithTimestamp)) {
                // We should not log the warning to the console as this
                // scenario assumes that someone tries to rotate the log file
                // with a sub-second delays.
                returnStatus |= k_ROTATE_RENAME_ERROR;
            }
            else {
                if (0 == bsl::rename(d_logFileName.c_str(),
                                     filenameWithTimestamp.c_str())) {
                    *rotatedLogFileName = filenameWithTimestamp;
                }
                else {
                    LOG_PLATFORM_MESSAGE(bsls::LogSeverity::e_WARN,
                                         "Cannot rename %s to %s: %s.",
                                         d_logFileName.c_str(),
                                         filenameWithTimestamp.c_str(),
                                         bsl::strerror(getErrorCode()));

                    returnStatus |= k_ROTATE_RENAME_ERROR;
                }
            }
        }
    }

    if (0 < d_rotationInterval.totalSeconds()) {
        d_nextRotationTimeUtc = computeNextRotationTime(
                         d_rotationReferenceTime,
                         d_observerFormatterImp.getTimezoneDefault()
                                           == RecordFormatterTimezone::e_LOCAL,
                         d_rotationInterval,
                         d_logFileTimestampUtc);
    }

    // Open the new log file (under some conditions the new log file may be the
    // same as the old one).
    if (0 != openLogFile(&d_logOutStream, d_logFileName.c_str())) {
        LOG_PLATFORM_MESSAGE(bsls::LogSeverity::e_WARN,
                             "Cannot open new log file: %s. "
                             "File logging will be disabled!",
                             d_logFileName.c_str());

        returnStatus |= k_ROTATE_NEW_LOG_ERROR;
    }

    return -returnStatus;
}

int FileObserver2::rotateIfNecessary(bsl::string           *rotatedLogFileName,
                                     const bdlt::Datetime&  currentLogTimeUtc)
{
    BSLS_ASSERT(d_rotationSize >= 0);
    BSLS_ASSERT(d_rotationInterval.totalSeconds() >= 0);
    BSLS_ASSERT(rotatedLogFileName);

    if (!d_logStreamBuf.isOpened()) {
        return 1;                                                     // RETURN
    }

    if (d_rotationSize) {
        // 'tellp' returns -1 on failure.  Rotate the log file if either
        // 'tellp' fails, or the rotation size is exceeded.

        if (static_cast<bsls::Types::Uint64>(d_logOutStream.tellp()) >
            static_cast<bsls::Types::Uint64>(d_rotationSize) * 1024) {

            return rotateFile(rotatedLogFileName);                    // RETURN
        }
    }

    if (d_rotationInterval.totalSeconds()
        && d_nextRotationTimeUtc <= currentLogTimeUtc) {
        return rotateFile(rotatedLogFileName);                        // RETURN
    }

    return 1;
}

// PRIVATE ACCESSORS
template <class STRING>
bool FileObserver2::isFileLoggingEnabledImpl(STRING *result) const
{
    BSLS_ASSERT(result);

    bslmt::LockGuard<bslmt::Mutex> guard(&d_mutex);

    bool rc = d_logStreamBuf.isOpened();
    if (rc) {
        result->assign(d_logFileName.cbegin(), d_logFileName.cend());
    }

    return rc;
}

// CREATORS
FileObserver2::FileObserver2(bslma::Allocator *basicAllocator)
: d_logStreamBuf(bdls::FilesystemUtil::k_INVALID_FD,
                 false,
                 true,
                 false,
                 basicAllocator)
, d_logOutStream(&d_logStreamBuf)
, d_logFilePattern(basicAllocator)
, d_logFileName(basicAllocator)
, d_observerFormatterImp("\n%d %p:%t %s %f:%l %c %m %u\n",
                         RecordFormatterTimezone::e_UTC,
                         basicAllocator)
, d_suppressUniqueFileName(false)
, d_rotationSize(0)
, d_rotationInterval(0)
, d_onRotationCb(bsl::allocator_arg_t(),
                 bsl::allocator<FileObserver2::OnFileRotationCallback>(
                                                               basicAllocator))
, d_rotationCbMutex()
{
}

FileObserver2::~FileObserver2()
{
    if (d_logStreamBuf.isOpened()) {
        d_logStreamBuf.clear();
    }
}

// MANIPULATORS
void FileObserver2::disableFileLogging()
{
    bslmt::LockGuard<bslmt::Mutex> guard(&d_mutex);

    if (d_logStreamBuf.isOpened()) {
        d_logStreamBuf.clear();
    }
}

void FileObserver2::disableLifetimeRotation()
{
    disableTimeIntervalRotation();
}

void FileObserver2::disablePublishInLocalTime()
{
    bslmt::LockGuard<bslmt::Mutex> guard(&d_mutex);
    d_observerFormatterImp.setTimezoneDefault(RecordFormatterTimezone::e_UTC);
}

void FileObserver2::disableSizeRotation()
{
    bslmt::LockGuard<bslmt::Mutex> guard(&d_mutex);

    d_rotationSize = 0;
}

void FileObserver2::disableTimeIntervalRotation()
{
    bslmt::LockGuard<bslmt::Mutex> guard(&d_mutex);

    d_rotationInterval.setTotalSeconds(0);
}

int FileObserver2::enableFileLogging(const char *logFilenamePattern)
{
    BSLS_ASSERT(logFilenamePattern);

    bslmt::LockGuard<bslmt::Mutex> guard(&d_mutex);

    if (d_logStreamBuf.isOpened()) {
        return 1;                                                     // RETURN
    }

    d_logFilePattern = logFilenamePattern;

    getLogFileName(&d_logFileName,
                   &d_logFileTimestampUtc,
                   d_logFilePattern.c_str(),
                   d_observerFormatterImp.getTimezoneDefault()
                                          == RecordFormatterTimezone::e_LOCAL);

    // Use the last modification time of the log file to calculate the next
    // rotation time if the log file already exists.  The
    // 'getLastModificationTime' method will simply fail without modifying
    // 'd_logFileTimestampUtc' if the log file does not already exist.

    bdls::FilesystemUtil::getLastModificationTime(&d_logFileTimestampUtc,
                                                  d_logFileName);

    if (0 < d_rotationInterval.totalSeconds()) {
        d_nextRotationTimeUtc = computeNextRotationTime(
                         d_rotationReferenceTime,
                         d_observerFormatterImp.getTimezoneDefault()
                                           == RecordFormatterTimezone::e_LOCAL,
                         d_rotationInterval,
                         d_logFileTimestampUtc);
    }

    return openLogFile(&d_logOutStream, d_logFileName.c_str());
}

int FileObserver2::enableFileLogging(const char *logFilenamePattern,
                                     bool        appendTimestampFlag)
{
    BSLS_ASSERT(logFilenamePattern);

    if (appendTimestampFlag && !hasEscapePattern(logFilenamePattern)) {
        bsl::string pattern(logFilenamePattern);
        pattern += ".%T";
        return enableFileLogging(pattern.c_str());                    // RETURN
    }

    return enableFileLogging(logFilenamePattern);
}

void FileObserver2::forceRotation()
{
    bsl::string rotatedLogFileName;
    int         rotationStatus;
    {
        bslmt::LockGuard<bslmt::Mutex> guard(&d_mutex);
        rotationStatus = rotateFile(&rotatedLogFileName);
    }

    // The file-rotation callback must be invoked without a lock on 'd_mutex'
    // to allow the callback to invoke other manipulators on this object.

    if (0 >= rotationStatus) {
        bslmt::LockGuard<bslmt::Mutex> guard(&d_rotationCbMutex);
        if (d_onRotationCb) {
            d_onRotationCb(rotationStatus, rotatedLogFileName);
        }
    }
}

void FileObserver2::enablePublishInLocalTime()
{
    bslmt::LockGuard<bslmt::Mutex> guard(&d_mutex);
    d_observerFormatterImp.setTimezoneDefault(
                                           RecordFormatterTimezone::e_LOCAL);
}

void FileObserver2::publish(const bsl::shared_ptr<const Record>& record,
                            const Context&)
{
    bsl::string rotatedFileName;
    int         rotationStatus;

    {
        bslmt::LockGuard<bslmt::Mutex> guard(&d_mutex);
        rotationStatus = rotateIfNecessary(&rotatedFileName,
                                           record->fixedFields().timestamp());

        if (d_logStreamBuf.isOpened()) {
            d_observerFormatterImp.formatLogRecord(d_logOutStream, record);

            if (!d_logOutStream) {
                LOG_PLATFORM_MESSAGE(bsls::LogSeverity::e_ERROR,
                                     "Error on file stream for %s: %s.",
                                     d_logFileName.c_str(),
                                     bsl::strerror(getErrorCode()));

                d_logStreamBuf.clear();
            }
        }
    }

    if (0 >= rotationStatus) {
        bslmt::LockGuard<bslmt::Mutex> guard(&d_rotationCbMutex);

        if (d_onRotationCb) {
            d_onRotationCb(rotationStatus, rotatedFileName);
        }
    }
}

void FileObserver2::suppressUniqueFileNameOnRotation(bool suppress)
{
    bslmt::LockGuard<bslmt::Mutex> guard(&d_mutex);

    d_suppressUniqueFileName = suppress;
}

void FileObserver2::rotateOnLifetime(
                                    const bdlt::DatetimeInterval& timeInterval)
{
    rotateOnTimeInterval(timeInterval);
}

void FileObserver2::rotateOnTimeInterval(
                                        const bdlt::DatetimeInterval& interval)
{
    rotateOnTimeInterval(interval,
        d_observerFormatterImp.getTimezoneDefault()
                                            == RecordFormatterTimezone::e_LOCAL
      ? bdlt::CurrentTime::local()
      : bdlt::CurrentTime::utc());
}

void FileObserver2::rotateOnTimeInterval(
                                       const bdlt::DatetimeInterval& interval,
                                       const bdlt::Datetime&         startTime)
{
    BSLS_ASSERT(0 < interval.totalMilliseconds());

    bslmt::LockGuard<bslmt::Mutex> guard(&d_mutex);

    d_rotationInterval = interval;

    // Reference time is interpreted as local or UTC time depending on the
    // value of d_publishInLocalTime.

    d_rotationReferenceTime = startTime;

    // Need to determine the next rotation time if the file is already opened.

    if (d_logStreamBuf.isOpened()) {
        d_nextRotationTimeUtc = computeNextRotationTime(
                         d_rotationReferenceTime,
                         d_observerFormatterImp.getTimezoneDefault()
                                           == RecordFormatterTimezone::e_LOCAL,
                         d_rotationInterval,
                         d_logFileTimestampUtc);
    }
}

void FileObserver2::rotateOnSize(int size)
{
    BSLS_ASSERT(size > 0);

    bslmt::LockGuard<bslmt::Mutex> guard(&d_mutex);
    d_rotationSize = size;
}

void FileObserver2::setLogFileFunctor(const RecordFormatter& logFileFunctor)
{
    bslmt::LockGuard<bslmt::Mutex> guard(&d_mutex);

    d_observerFormatterImp.setFormatFunctor(logFileFunctor);
}

int FileObserver2::setFormat(const bsl::string_view& format)
{
    bslmt::LockGuard<bslmt::Mutex> guard(&d_mutex);
    return d_observerFormatterImp.setFormat(format);
}

void FileObserver2::setOnFileRotationCallback(
                              const OnFileRotationCallback& onRotationCallback)
{
    bslmt::LockGuard<bslmt::Mutex> guard(&d_rotationCbMutex);
    d_onRotationCb = onRotationCallback;
}

// ACCESSORS
bool FileObserver2::isFileLoggingEnabled() const
{
    bslmt::LockGuard<bslmt::Mutex> guard(&d_mutex);

    return d_logStreamBuf.isOpened();
}

bool FileObserver2::isFileLoggingEnabled(bsl::string *result) const
{
    return isFileLoggingEnabledImpl(result);
}

bool FileObserver2::isFileLoggingEnabled(std::string *result) const
{
    return isFileLoggingEnabledImpl(result);
}

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR_STRING
bool FileObserver2::isFileLoggingEnabled(std::pmr::string *result) const
{
    return isFileLoggingEnabledImpl(result);
}
#endif  //BSLS_LIBRARYFEATURES_HAS_CPP17_PMR_STRING

bool FileObserver2::isPublishInLocalTimeEnabled() const
{
    bslmt::LockGuard<bslmt::Mutex> guard(&d_mutex);

    return d_observerFormatterImp.getTimezoneDefault()
                                           == RecordFormatterTimezone::e_LOCAL;
}

bool FileObserver2::isSuppressUniqueFileNameOnRotation() const
{
    bslmt::LockGuard<bslmt::Mutex> guard(&d_mutex);

    return d_suppressUniqueFileName;
}

const bsl::string& FileObserver2::getFormat() const
{
    return d_observerFormatterImp.getFormat();
}

bdlt::DatetimeInterval FileObserver2::localTimeOffset() const
{
    bslmt::LockGuard<bslmt::Mutex> guard(&d_mutex);

    bdlt::Datetime timestamp = d_logStreamBuf.isOpened()
                               ? d_logFileTimestampUtc
                               : bdlt::CurrentTime::utc();

    return bdlt::IntervalConversionUtil::convertToDatetimeInterval(
                            bdlt::LocalTimeOffset::localTimeOffset(timestamp));
}

bdlt::DatetimeInterval FileObserver2::rotationLifetime() const
{
    bslmt::LockGuard<bslmt::Mutex> guard(&d_mutex);

    return d_rotationInterval;
}

int FileObserver2::rotationSize() const
{
    bslmt::LockGuard<bslmt::Mutex> guard(&d_mutex);

    return d_rotationSize;
}

}  // close package namespace
}  // close enterprise namespace

#if defined(BSLS_PLATFORM_CMP_MSVC)
#undef snprintf
#endif

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
