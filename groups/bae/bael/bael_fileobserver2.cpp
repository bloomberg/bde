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

#include <bdetu_systemtime.h>

#include <bdef_memfn.h>

#include <bdesu_fileutil.h>

#include <bsls_assert.h>
#include <bsls_platform.h>
#include <bsls_types.h>

#include <bslstl_stringref.h>

#include <bsl_cstdio.h>
#include <bsl_iomanip.h>
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

namespace {

using namespace BloombergLP;

enum {
    // status code for the call back function.

    ROTATE_SUCCESS                  =  0,
    ROTATE_RENAME_ERROR             = -1,
    ROTATE_NEW_LOG_ERROR            = -2,
    ROTATE_RENAME_AND_NEW_LOG_ERROR = -3
};

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
    // Return the specified 'timestamp' in the 'YYYYMMDD_hhmmss' format.
{
    char buffer[20];

#if defined(BSLS_PLATFORM__CMP_MSVC)
#define snprintf _snprintf
#endif

    snprintf(buffer,
             sizeof buffer,
             "%04d%02d%02d_%02d%02d%02d",
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

void getLogFileName(bsl::string                  *logFileName,
                    bdet_Datetime                *timestamp,
                    const char                   *logFilePattern,
                    const bdet_DatetimeInterval&  localTimeOffset)
    // Load, into the specified 'logFileName', the filename that is obtained by
    // replacing every '%'-escape sequence in the specified 'logFilePattern'
    // with the corresponding field of the local time calculated from the
    // current time in UTC and the specified 'localTimeOffset'.  Load the
    // current time in UTC into the specified 'timestamp'.
{
    BSLS_ASSERT(logFileName);
    BSLS_ASSERT(timestamp);
    BSLS_ASSERT(logFilePattern);

    *timestamp = bdetu_SystemTime::nowAsDatetimeUtc();

    const bdet_Datetime logFileTimestamp = *timestamp + localTimeOffset;

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

bool hasEscapePattern(const char *logFilePattern)
    // Return 'true' if the specified 'logFilePattern' contains a recognized
    // '%'-escape sequence, and false otherwise.  The recognized escape
    // sequence are "%Y", "%M", "%D", "%h", "%m", "%s", and "%%".

{
    for (; *logFilePattern; ++logFilePattern) {
        if ('%' == *logFilePattern) {
            ++logFilePattern;
            if ('\0' == *logFilePattern) {
                return false;                                         // RETURN
            }

            switch(*logFilePattern) {
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

int openLogFile(bsl::ostream *stream, const char *filename)
    // Open a file stream referred to by the specified 'stream' for the file
    // with the specified 'filename' in append mode.  Return 0 on success, and
    // a non-zero value otherwise.
{
    BSLS_ASSERT(stream);
    BSLS_ASSERT(filename);

    const bool fileExistFlag = bdesu_FileUtil::exists(filename);
    bdesu_FileUtil::FileDescriptor fd = bdesu_FileUtil::open(filename,
                                                             true,
                                                             fileExistFlag,
                                                             true);

#ifdef BSLS_PLATFORM__OS_UNIX
    // Add read/write access to other, because 'bdesu_FileUtil::open' set file
    // permission to 'rw-rw----'.

    chmod(filename, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
#endif

    if (fd == bdesu_FileUtil::INVALID_FD) {
        fprintf(
              stderr,
              "Cannot open log file %s: %s.  File logging will be disabled!\n",
              filename, bsl::strerror(getErrorCode()));
        return -1;                                                    // RETURN
    }

    bdesu_FdStreamBuf *streamBuf = dynamic_cast<bdesu_FdStreamBuf *>(
                                                              stream->rdbuf());
    BSLS_ASSERT(streamBuf);

    if (0 != streamBuf->reset(fd, true, true, true)) {
        fprintf(
              stderr,
              "Cannot close previous log file %s: %s.  "
              "File logging will be disabled!\n",
              filename, bsl::strerror(getErrorCode()));
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

void computeNextRotationTime(bdet_Datetime                *result,
                             const bdet_Datetime&          referenceStartTime,
                             const bdet_DatetimeInterval&  localTimeOffset,
                             const bdet_DatetimeInterval&  interval,
                             const bdet_Datetime&          fileCreationTime)
    // Load, into the specified 'result', the next scheduled rotation time
    // after the specified 'fileCreationTime' for a schedule that has a start
    // reference time indicated by the specified 'referenceStartTime' and
    // rotated every specified 'interval'.  The behavior is undefined unless
    // '0 <= interval.totalMilliseconds()'.
{
    BSLS_ASSERT(result);
    BSLS_ASSERT(0 < interval.totalMilliseconds());

    bsls_Types::Int64 timeLeft = (fileCreationTime
                                  - referenceStartTime
                                  + localTimeOffset).totalMilliseconds()
                               % interval.totalMilliseconds();

    // The modulo operator may return a negative number depending on
    // implementation.

    if (timeLeft >= 0) {
        timeLeft = interval.totalMilliseconds() - timeLeft;
    }
    else {
        timeLeft = -timeLeft;
    }

    *result = fileCreationTime;
    result->addMilliseconds(timeLeft);

    // Prevent rotation at 'referenceStartTime' as this may cause an empty log
    // to be generated if 'rotateOnTimeInterval' is called after
    // 'enableFileLogging'.

    if (referenceStartTime == *result + localTimeOffset) {
        *result += interval;
    }
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

    char buffer[256];
    char *ptr = buffer;

    *ptr = '\n';
    ++ptr;

    int length = timestamp.printToBuffer(ptr, sizeof(buffer) - 1);
    ptr += length;

#if defined(BSLS_PLATFORM__CMP_MSVC)
#define snprintf _snprintf
#endif

    snprintf(ptr,
             sizeof(buffer) - length - 1,
             " %d:%llu %s %s:%d ",
             fixedFields.processID(),
             fixedFields.threadID(),
             bael_Severity::toAscii(
                                 (bael_Severity::Level)fixedFields.severity()),
             fixedFields.fileName(),
             fixedFields.lineNumber());

#if defined(BSLS_PLATFORM__CMP_MSVC)
#undef snprintf
#endif

    stream << buffer;
    stream << fixedFields.category();
    stream << ' ';
    bslstl_StringRef message = fixedFields.messageRef();
    stream.write(message.data(), message.length());
    stream << ' ';

    const bdem_List& userFields = record.userFields();
    const int numUserFields = userFields.length();
    for (int i = 0; i < numUserFields; ++i) {
        stream << userFields[i] << ' ';
    }

    stream << '\n';

    stream.flush();
}

int bael_FileObserver2::rotateFile(bsl::string *rotatedLogFileName)
{
    BSLS_ASSERT(rotatedLogFileName);

    if (!d_logStreamBuf.isOpened()) {
        return 1;                                                     // RETURN
    }

    BSLS_ASSERT(d_logFilePattern.size() > 0);

    int returnStatus = ROTATE_SUCCESS;

    if (0 != d_logStreamBuf.clear()) {
        fprintf(stderr, "Unable to close old log file: %s\n",
                d_logFileName.c_str());
        returnStatus = ROTATE_RENAME_ERROR;                           // RETURN
    }

    *rotatedLogFileName = d_logFileName;

    const bdet_Datetime oldLogFileTimestamp = d_logFileTimestamp;

    getLogFileName(&d_logFileName,
                   &d_logFileTimestamp,
                   d_logFilePattern.c_str(),
                   d_localTimeOffset);

    if (bdesu_FileUtil::exists(d_logFileName.c_str())) {
        bsl::string newFileName(d_logFileName);
        newFileName += '.';
        newFileName +=
                   getTimestampSuffix(oldLogFileTimestamp + d_localTimeOffset);

        if (0 == bsl::rename(d_logFileName.c_str(), newFileName.c_str())) {
            *rotatedLogFileName = newFileName;
        }
        else {
            fprintf(stderr, "Cannot rename %s to %s: %s\n",
                    d_logFileName.c_str(), newFileName.c_str(),
                    bsl::strerror(getErrorCode()));
            returnStatus = ROTATE_RENAME_ERROR;
        }
    }

    if (0 < d_rotationInterval.totalSeconds()) {
        computeNextRotationTime(&d_nextRotationTime,
                                d_rotationReferenceLocalTime,
                                d_localTimeOffset,
                                d_rotationInterval,
                                d_logFileTimestamp);
    }

    if (0 != openLogFile(&d_logOutStream, d_logFileName.c_str())) {
        fprintf(stderr, "Cannot open new log file: %s\n",
                d_logFileName.c_str());
        return ROTATE_SUCCESS != returnStatus
               ? ROTATE_RENAME_AND_NEW_LOG_ERROR
               : ROTATE_NEW_LOG_ERROR;                                // RETURN
    }

    return returnStatus;
}

int  bael_FileObserver2::rotateIfNecessary(
                                      bsl::string          *rotatedLogFileName,
                                      const bdet_Datetime&  currentLogTime)
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

        if (static_cast<bsls_Types::Uint64>(d_logOutStream.tellp()) >
            static_cast<bsls_Types::Uint64>(d_rotationSize) * 1024) {

            return rotateFile(rotatedLogFileName);                    // RETURN
        }
    }

    if (d_rotationInterval.totalSeconds()
     && d_nextRotationTime <= currentLogTime) {
        return rotateFile(rotatedLogFileName);                        // RETURN
    }
    return 1;
}

// CREATORS
bael_FileObserver2::bael_FileObserver2(bslma_Allocator *basicAllocator)
: d_logStreamBuf(bdesu_FileUtil::INVALID_FD, false)
, d_logOutStream(&d_logStreamBuf)
, d_logFilePattern(basicAllocator)
, d_logFileName(basicAllocator)
, d_logFileFunctor(
            bdef_MemFnUtil::memFn(&bael_FileObserver2::logRecordDefault, this),
            basicAllocator)
, d_localTimeOffset(bdetu_SystemTime::localTimeOffset())
, d_publishInLocalTime(false)
, d_rotationSize(0)
, d_rotationInterval(0)
, d_onRotationCb()
, d_rotationCbMutex()
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
    disableTimeIntervalRotation();
}

void bael_FileObserver2::disableTimeIntervalRotation()
{
    bcemt_LockGuard<bcemt_Mutex> guard(&d_mutex);
    d_rotationInterval.setTotalSeconds(0);
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

int bael_FileObserver2::enableFileLogging(const char *logFilenamePattern)
{
    BSLS_ASSERT(logFilenamePattern);

    bcemt_LockGuard<bcemt_Mutex> guard(&d_mutex);
    if (d_logStreamBuf.isOpened()) {
        return 1;                                                     // RETURN
    }
    d_logFilePattern = logFilenamePattern;

    getLogFileName(&d_logFileName,
                   &d_logFileTimestamp,
                   d_logFilePattern.c_str(),
                   d_localTimeOffset);

    // Use the last modification time of the log file to calculate the next
    // rotation time if the log file already exists.  The
    // 'getLastModificationTime' method will simply fail without modifying
    // 'd_logFileTimestamp' if the log file does not already exist.

    bdesu_FileUtil::getLastModificationTime(&d_logFileTimestamp,
                                            d_logFileName);

    if (0 < d_rotationInterval.totalSeconds()) {
        computeNextRotationTime(&d_nextRotationTime,
                                d_rotationReferenceLocalTime,
                                d_localTimeOffset,
                                d_rotationInterval,
                                d_logFileTimestamp);
    }

    return openLogFile(&d_logOutStream, d_logFileName.c_str());
}

int bael_FileObserver2::enableFileLogging(const char *logFilenamePattern,
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

void bael_FileObserver2::forceRotation()
{
    bsl::string rotatedLogFileName;
    int         rotationStatus;
    {
        bcemt_LockGuard<bcemt_Mutex> guard(&d_mutex);
        rotationStatus = rotateFile(&rotatedLogFileName);
    }

    // The file-rotation callback must be invoked without a lock on 'd_mutex'
    // to allow the callback to invoke other manipulators on this object.

    if (0 >= rotationStatus) {
        bcemt_LockGuard<bcemt_Mutex> guard(&d_rotationCbMutex);
        if (d_onRotationCb) {
            d_onRotationCb(rotationStatus, rotatedLogFileName);
        }
    }
}

void bael_FileObserver2::enablePublishInLocalTime()
{
    bcemt_LockGuard<bcemt_Mutex> guard(&d_mutex);
    d_publishInLocalTime = true;
}

void bael_FileObserver2::publish(const bael_Record&  record,
                                 const bael_Context& context)
{
    bsl::string rotatedFileName;
    int         rotationStatus;

    {
        bcemt_LockGuard<bcemt_Mutex> guard(&d_mutex);
        rotationStatus = rotateIfNecessary(&rotatedFileName,
                                           record.fixedFields().timestamp());

        if (d_logStreamBuf.isOpened()) {
            d_logFileFunctor(d_logOutStream, record);

            if (!d_logOutStream) {
                fprintf(stderr, "Error on file stream for %s: %s\n",
                        d_logFileName.c_str(), bsl::strerror(getErrorCode()));

                d_logStreamBuf.clear();
            }
        }
    }

    if (0 >= rotationStatus) {
        bcemt_LockGuard<bcemt_Mutex> guard(&d_rotationCbMutex);
        if (d_onRotationCb) {
            d_onRotationCb(rotationStatus, rotatedFileName);
        }
    }
}

void bael_FileObserver2::rotateOnLifetime(
                                     const bdet_DatetimeInterval& timeInterval)
{
    rotateOnTimeInterval(timeInterval);
}

void bael_FileObserver2::rotateOnTimeInterval(
                                         const bdet_DatetimeInterval& interval)
{
    rotateOnTimeInterval(interval,
                         bdetu_SystemTime::nowAsDatetimeUtc()
                                                          + d_localTimeOffset);
}

void bael_FileObserver2::rotateOnTimeInterval(
                               const bdet_DatetimeInterval& interval,
                               const bdet_Datetime&         referenceStartTime)
{
    BSLS_ASSERT(0 < interval.totalMilliseconds());

    bcemt_LockGuard<bcemt_Mutex> guard(&d_mutex);
    d_rotationInterval = interval;

    // Reference time is stored as local time as conversion to UTC time may
    // cause an underflow (or overflow).

    d_rotationReferenceLocalTime = referenceStartTime;

    // Need to determine the next rotation time if the file is already opened.

    if (d_logStreamBuf.isOpened()) {
        computeNextRotationTime(&d_nextRotationTime,
                                d_rotationReferenceLocalTime,
                                d_localTimeOffset,
                                d_rotationInterval,
                                d_logFileTimestamp);
    }
}

void bael_FileObserver2::rotateOnSize(int size)
{
    BSLS_ASSERT(size > 0);

    bcemt_LockGuard<bcemt_Mutex> guard(&d_mutex);
    d_rotationSize = size;
}

void bael_FileObserver2::setLogFileFunctor(
                                        const LogRecordFunctor& logFileFunctor)
{
    bcemt_LockGuard<bcemt_Mutex> guard(&d_mutex);
    d_logFileFunctor = logFileFunctor;
}

void bael_FileObserver2::setOnFileRotationCallback(
                              const OnFileRotationCallback& onRotationCallback)
{
    bcemt_LockGuard<bcemt_Mutex> guard(&d_rotationCbMutex);
    d_onRotationCb = onRotationCallback;
}

// ACCESSORS
bool bael_FileObserver2::isFileLoggingEnabled() const
{
    bcemt_LockGuard<bcemt_Mutex> guard(&d_mutex);

    return d_logStreamBuf.isOpened();
}

bool bael_FileObserver2::isFileLoggingEnabled(bsl::string *result) const
{
    BSLS_ASSERT(result);

    bcemt_LockGuard<bcemt_Mutex> guard(&d_mutex);

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
    return d_rotationInterval;
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
