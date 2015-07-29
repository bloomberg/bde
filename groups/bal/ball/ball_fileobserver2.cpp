// ball_fileobserver2.cpp                                             -*-C++-*-
#include <ball_fileobserver2.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ball_fileobserver2_cpp,"$Id$ $CSID$")

#include <ball_context.h>
#include <ball_record.h>
#include <ball_recordattributes.h>

#ifdef BDE_FOR_TESTING_ONLY
#include <ball_defaultobserver.h>             // for testing only
#include <ball_log.h>                         // for testing only
#include <ball_loggermanager.h>               // for testing only
#include <ball_loggermanagerconfiguration.h>  // for testing only
#include <ball_multiplexobserver.h>           // for testing only
#include <ball_recordstringformatter.h>       // for testing only
#endif

#include <bdlqq_lockguard.h>

#include <bdlmxxx_list.h>

#include <bdlt_currenttime.h>

#include <bdlf_memfn.h>

#include <bdlsu_filesystemutil.h>
#include <bdlsu_processutil.h>

#include <bdlt_currenttime.h>
#include <bdlt_localtimeoffset.h>

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

#ifdef BSLS_PLATFORM_OS_UNIX
#include <sys/types.h>
#include <sys/stat.h>
#endif

#ifdef BSLS_PLATFORM_OS_WINDOWS
#include <windows.h>
#endif

namespace {

using namespace BloombergLP;

// Messages written to 'stderr' are prefixed with a unique string to allow
// them to be easily identified.

const char errorMsgPrefix[] = { "ERROR: ball::FileObserver2:" };
const char warnMsgPrefix[]  = { "WARN: ball::FileObserver2:" };

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
#ifdef BSLS_PLATFORM_OS_WINDOWS
    int rc = GetLastError();
    return rc ? rc : errno;
#else
    return errno;
#endif
}

bsl::string getTimestampSuffix(const bdlt::Datetime& timestamp)
    // Return the specified 'timestamp' in the 'YYYYMMDD_hhmmss' format.
{
    char buffer[20];

#if defined(BSLS_PLATFORM_CMP_MSVC)
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

#if defined(BSLS_PLATFORM_CMP_MSVC)
#undef snprintf
#endif

    return bsl::string(buffer);
}

bdlt::DatetimeInterval localTimeOffsetInterval(bdlt::Datetime timeUtc)
    // Return the offset of local time from UTC time at the specified
    // 'timeUtc'.
{
    return bdlt::IntervalConversionUtil::convertToDatetimeInterval(
                              bdlt::LocalTimeOffset::localTimeOffset(timeUtc));
}

void getLogFileName(bsl::string                  *logFileName,
                    bdlt::Datetime                *timestampUtc,
                    const char                   *logFilePattern,
                    bool                          publishInLocalTime)
    // Load, into the specified 'logFileName', the filename that is obtained by
    // replacing every '%'-escape sequence in the specified 'logFilePattern'.
    // If the specified 'publishInLocalTime' is 'true', replace the time
    // patterns with local time values, and replace them with UTC time values
    // otherwise.  Load the current time in UTC into the specified
    // 'timestampUtc'.
{
    BSLS_ASSERT(logFileName);
    BSLS_ASSERT(timestampUtc);
    BSLS_ASSERT(logFilePattern);

    *timestampUtc = bdlt::CurrentTime::utc();

    bdlt::Datetime logFileTimestamp = *timestampUtc;

    if (publishInLocalTime) {
        logFileTimestamp += localTimeOffsetInterval(*timestampUtc);
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
                    os << bdlsu::ProcessUtil::getProcessId();
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

    const bool fileExistFlag = bdlsu::FilesystemUtil::exists(filename);
    bdlsu::FilesystemUtil::FileDescriptor fd = bdlsu::FilesystemUtil::open(
                       filename,
                       fileExistFlag ? bdlsu::FilesystemUtil::e_OPEN
                                     : bdlsu::FilesystemUtil::e_OPEN_OR_CREATE,
                       bdlsu::FilesystemUtil::e_READ_APPEND);

    if (fd == bdlsu::FilesystemUtil::k_INVALID_FD) {
        fprintf(
           stderr,
           "%s Cannot open log file %s: %s.  File logging will be disabled!\n",
           errorMsgPrefix, filename, bsl::strerror(getErrorCode()));
        return -1;                                                    // RETURN
    }

#ifdef BSLS_PLATFORM_OS_UNIX
    // Add read/write access to other, because 'bdlsu::FilesystemUtil::open' set file
    // permission to 'rw-rw----'.

    chmod(filename, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
#endif

    bdlsu::FdStreamBuf *streamBuf = dynamic_cast<bdlsu::FdStreamBuf *>(
                                                              stream->rdbuf());
    BSLS_ASSERT(streamBuf);

    if (0 != streamBuf->reset(fd, true, true, true)) {
        fprintf(
              stderr,
              "%s Cannot close previous log file %s: %s.  "
              "File logging will be disabled!\n",
              warnMsgPrefix, filename, bsl::strerror(getErrorCode()));
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


int toSerialDate(bdlt::Date date)
    // Return the elapsed number of days between 01JAN0001 and the specified
    // 'date'.
{
    return bdlt::DelegatingDateImpUtil::ymdToSerial(date.year(),
                                                    date.month(),
                                                    date.day());
}


bdlt::Datetime computeNextRotationTime(
                     const bdlt::Datetime&          referenceStartTimeLocal,
                     const bdlt::DatetimeInterval&  interval,
                     const bdlt::Datetime&          fileCreationTimeUtc)
    // Return the UTC time for the next scheduled file rotation after the
    // specified 'fileCreationTimeUtc', for a schedule that has a start
    // reference time indicated by the specified 'referenceStartTimeLocal' and
    // rotated every specified 'interval'.  'referenceStartTimeLocal' must be a
    // local time value, and 'fileCreationTimeUtc' must be a UTC time value.
    // The behavior is undefined unless '0 <= interval.totalMilliseconds()'.
    // Note that 'referenceStartTimeLocal' is a local time value because
    // 'rotateOnTimeInterval' accepts a local time value and converting that
    // value to UTC might cause an underflow.
{
    BSLS_ASSERT(0 < interval.totalMilliseconds());


    // Notice that the logic for computing the next time interval must
    // currently be expressed using 'bdlt::DelegatingDateImpUtil' to avoid
    // possible use of 'bsls::Log' to report warnings about date math.  Such
    // warnings, when issued from within a function in bael cause an attempt
    // to recursively re-enter the file-observer (and a dead-lock).  Once
    // 'bdlt' no longer uses bsls log to report date arithmetic this logic can
    // be returned to:
    //..
    //  bsls::Types::Int64 timeLeft =
    //      (fileCreationTimeUtc + localTimeOffset - referenceStartTimeLocal).
    //                    totalMilliseconds() % interval.totalMilliseconds();
    //..

    bdlt::DatetimeInterval localTimeOffset = 
                                  localTimeOffsetInterval(fileCreationTimeUtc);

    bdlt::Datetime fileCreationTimeLocal = fileCreationTimeUtc +
                                           localTimeOffset;

    int creation  = toSerialDate(fileCreationTimeLocal.date());
    int reference = toSerialDate(referenceStartTimeLocal.date());

    bdlt::DatetimeInterval fileCreationInterval(creation - reference, 0, 0);
    fileCreationInterval += bdlt::DatetimeInterval(
               fileCreationTimeLocal.time() - referenceStartTimeLocal.time());


    bsls::Types::Int64 timeLeft = fileCreationInterval.totalMilliseconds() %
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

    // Prevent rotation at 'referenceStartTimeLocal' as this may cause an
    // empty log to be generated if 'rotateOnTimeInterval' is called after
    // 'enableFileLogging'.

    if (referenceStartTimeLocal == resultUtc + localTimeOffset) {
        resultUtc += interval;
    }
    return resultUtc;
}

}  // close unnamed namespace

namespace BloombergLP {

namespace ball {
                          // ------------------------
                          // class FileObserver2
                          // ------------------------

// PRIVATE MANIPULATORS
void FileObserver2::logRecordDefault(bsl::ostream&      stream,
                                          const Record& record)

{
    const RecordAttributes& fixedFields = record.fixedFields();

    bdlt::Datetime timestamp = fixedFields.timestamp();
    if (d_publishInLocalTime) {
        int localTimeOffsetInSeconds  = 
            bdlt::LocalTimeOffset::localTimeOffset(timestamp).totalSeconds();
        timestamp.addSeconds(localTimeOffsetInSeconds);
    }

    char buffer[256];
    char *ptr = buffer;

    *ptr = '\n';
    ++ptr;

    int length = timestamp.printToBuffer(ptr, sizeof(buffer) - 1);
    ptr += length;

#if defined(BSLS_PLATFORM_CMP_MSVC)
#define snprintf _snprintf
#endif

    snprintf(ptr,
             sizeof(buffer) - length - 1,
             " %d:%llu %s %s:%d ",
             fixedFields.processID(),
             fixedFields.threadID(),
             Severity::toAscii(
                                 (Severity::Level)fixedFields.severity()),
             fixedFields.fileName(),
             fixedFields.lineNumber());

#if defined(BSLS_PLATFORM_CMP_MSVC)
#undef snprintf
#endif

    stream << buffer;
    stream << fixedFields.category();
    stream << ' ';
    bslstl::StringRef message = fixedFields.messageRef();
    stream.write(message.data(), message.length());
    stream << ' ';

    const bdlmxxx::List& userFields = record.userFields();
    const int numUserFields = userFields.length();
    for (int i = 0; i < numUserFields; ++i) {
        stream << userFields[i] << ' ';
    }

    stream << '\n';

    stream.flush();
}

int FileObserver2::rotateFile(bsl::string *rotatedLogFileName)
{
    BSLS_ASSERT(rotatedLogFileName);

    if (!d_logStreamBuf.isOpened()) {
        return 1;                                                     // RETURN
    }

    BSLS_ASSERT(d_logFilePattern.size() > 0);

    int returnStatus = ROTATE_SUCCESS;

    if (0 != d_logStreamBuf.clear()) {
        fprintf(stderr, "%s Unable to close old log file: %s\n",
                warnMsgPrefix, d_logFileName.c_str());
        returnStatus = ROTATE_RENAME_ERROR;                           // RETURN
    }

    *rotatedLogFileName = d_logFileName;

    const bdlt::Datetime oldLogFileTimestamp = d_logFileTimestampUtc;

    getLogFileName(&d_logFileName,
                   &d_logFileTimestampUtc,
                   d_logFilePattern.c_str(),
                   d_publishInLocalTime);

    if (bdlsu::FilesystemUtil::exists(d_logFileName.c_str())) {
        bdlt::Datetime timeStampSuffix(oldLogFileTimestamp);
        if (d_publishInLocalTime) {
            timeStampSuffix += localTimeOffsetInterval(oldLogFileTimestamp);
        }

        bsl::string newFileName(d_logFileName);
        newFileName += '.';
        newFileName += getTimestampSuffix(timeStampSuffix);

        if (0 == bsl::rename(d_logFileName.c_str(), newFileName.c_str())) {
            *rotatedLogFileName = newFileName;
        }
        else {
            fprintf(stderr, "%s Cannot rename %s to %s: %s\n",
                    warnMsgPrefix, d_logFileName.c_str(), newFileName.c_str(),
                    bsl::strerror(getErrorCode()));
            returnStatus = ROTATE_RENAME_ERROR;
        }
    }

    if (0 < d_rotationInterval.totalSeconds()) {
        d_nextRotationTimeUtc = computeNextRotationTime(
                                                  d_rotationReferenceLocalTime,
                                                  d_rotationInterval,
                                                  d_logFileTimestampUtc);
    }

    if (0 != openLogFile(&d_logOutStream, d_logFileName.c_str())) {
        fprintf(stderr, "%s Cannot open new log file: %s\n",
                errorMsgPrefix, d_logFileName.c_str());
        return ROTATE_SUCCESS != returnStatus
               ? ROTATE_RENAME_AND_NEW_LOG_ERROR
               : ROTATE_NEW_LOG_ERROR;                                // RETURN
    }

    return returnStatus;
}

int  FileObserver2::rotateIfNecessary(
                                      bsl::string          *rotatedLogFileName,
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

// CREATORS
FileObserver2::FileObserver2(bslma::Allocator *basicAllocator)
: d_logStreamBuf(bdlsu::FilesystemUtil::k_INVALID_FD, false)
, d_logOutStream(&d_logStreamBuf)
, d_logFilePattern(basicAllocator)
, d_logFileName(basicAllocator)
, d_logFileFunctor(
            bdlf::MemFnUtil::memFn(&FileObserver2::logRecordDefault, this),
            basicAllocator)
, d_publishInLocalTime(false)
, d_rotationSize(0)
, d_rotationInterval(0)
, d_onRotationCb()
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
    bdlqq::LockGuard<bdlqq::Mutex> guard(&d_mutex);
    if (d_logStreamBuf.isOpened()) {
        d_logStreamBuf.clear();
    }
}

void FileObserver2::disableLifetimeRotation()
{
    disableTimeIntervalRotation();
}

void FileObserver2::disableTimeIntervalRotation()
{
    bdlqq::LockGuard<bdlqq::Mutex> guard(&d_mutex);
    d_rotationInterval.setTotalSeconds(0);
}

void FileObserver2::disableSizeRotation()
{
    bdlqq::LockGuard<bdlqq::Mutex> guard(&d_mutex);
    d_rotationSize = 0;
}

void FileObserver2::disablePublishInLocalTime()
{
    bdlqq::LockGuard<bdlqq::Mutex> guard(&d_mutex);
    d_publishInLocalTime = false;
}

int FileObserver2::enableFileLogging(const char *logFilenamePattern)
{
    BSLS_ASSERT(logFilenamePattern);

    bdlqq::LockGuard<bdlqq::Mutex> guard(&d_mutex);
    if (d_logStreamBuf.isOpened()) {
        return 1;                                                     // RETURN
    }
    d_logFilePattern = logFilenamePattern;

    getLogFileName(&d_logFileName,
                   &d_logFileTimestampUtc,
                   d_logFilePattern.c_str(),
                   d_publishInLocalTime);

    // Use the last modification time of the log file to calculate the next
    // rotation time if the log file already exists.  The
    // 'getLastModificationTime' method will simply fail without modifying
    // 'd_logFileTimestampUtc' if the log file does not already exist.

    bdlsu::FilesystemUtil::getLastModificationTime(&d_logFileTimestampUtc,
                                            d_logFileName);

    if (0 < d_rotationInterval.totalSeconds()) {
        d_nextRotationTimeUtc = computeNextRotationTime(
                                                  d_rotationReferenceLocalTime,
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
        bdlqq::LockGuard<bdlqq::Mutex> guard(&d_mutex);
        rotationStatus = rotateFile(&rotatedLogFileName);
    }

    // The file-rotation callback must be invoked without a lock on 'd_mutex'
    // to allow the callback to invoke other manipulators on this object.

    if (0 >= rotationStatus) {
        bdlqq::LockGuard<bdlqq::Mutex> guard(&d_rotationCbMutex);
        if (d_onRotationCb) {
            d_onRotationCb(rotationStatus, rotatedLogFileName);
        }
    }
}

void FileObserver2::enablePublishInLocalTime()
{
    bdlqq::LockGuard<bdlqq::Mutex> guard(&d_mutex);
    d_publishInLocalTime = true;
}

void FileObserver2::publish(const Record&  record,
                                 const Context& )
{
    bsl::string rotatedFileName;
    int         rotationStatus;

    {
        bdlqq::LockGuard<bdlqq::Mutex> guard(&d_mutex);
        rotationStatus = rotateIfNecessary(&rotatedFileName,
                                           record.fixedFields().timestamp());

        if (d_logStreamBuf.isOpened()) {
            d_logFileFunctor(d_logOutStream, record);

            if (!d_logOutStream) {
                fprintf(stderr, "%s Error on file stream for %s: %s\n",
                        errorMsgPrefix,
                        d_logFileName.c_str(), bsl::strerror(getErrorCode()));

                d_logStreamBuf.clear();
            }
        }
    }

    if (0 >= rotationStatus) {
        bdlqq::LockGuard<bdlqq::Mutex> guard(&d_rotationCbMutex);
        if (d_onRotationCb) {
            d_onRotationCb(rotationStatus, rotatedFileName);
        }
    }
}

void FileObserver2::rotateOnLifetime(
                                     const bdlt::DatetimeInterval& timeInterval)
{
    rotateOnTimeInterval(timeInterval);
}

void FileObserver2::rotateOnTimeInterval(
                                         const bdlt::DatetimeInterval& interval)
{
    rotateOnTimeInterval(interval, bdlt::CurrentTime::local());
}

void FileObserver2::rotateOnTimeInterval(
                               const bdlt::DatetimeInterval& interval,
                               const bdlt::Datetime&         referenceStartTime)
{
    BSLS_ASSERT(0 < interval.totalMilliseconds());

    bdlqq::LockGuard<bdlqq::Mutex> guard(&d_mutex);
    d_rotationInterval = interval;

    // Reference time is stored as local time as conversion to UTC time may
    // cause an underflow (or overflow).

    d_rotationReferenceLocalTime = referenceStartTime;

    // Need to determine the next rotation time if the file is already opened.

    if (d_logStreamBuf.isOpened()) {
        d_nextRotationTimeUtc = computeNextRotationTime(
                                                  d_rotationReferenceLocalTime,
                                                  d_rotationInterval,
                                                  d_logFileTimestampUtc);
    }
}

void FileObserver2::rotateOnSize(int size)
{
    BSLS_ASSERT(size > 0);

    bdlqq::LockGuard<bdlqq::Mutex> guard(&d_mutex);
    d_rotationSize = size;
}

void FileObserver2::setLogFileFunctor(
                                        const LogRecordFunctor& logFileFunctor)
{
    bdlqq::LockGuard<bdlqq::Mutex> guard(&d_mutex);
    d_logFileFunctor = logFileFunctor;
}

void FileObserver2::setOnFileRotationCallback(
                              const OnFileRotationCallback& onRotationCallback)
{
    bdlqq::LockGuard<bdlqq::Mutex> guard(&d_rotationCbMutex);
    d_onRotationCb = onRotationCallback;
}

// ACCESSORS
bool FileObserver2::isFileLoggingEnabled() const
{
    bdlqq::LockGuard<bdlqq::Mutex> guard(&d_mutex);

    return d_logStreamBuf.isOpened();
}

bool FileObserver2::isFileLoggingEnabled(bsl::string *result) const
{
    BSLS_ASSERT(result);

    bdlqq::LockGuard<bdlqq::Mutex> guard(&d_mutex);

    bool rc = d_logStreamBuf.isOpened();
    if (rc) {
        result->assign(d_logFileName);
    }

    return rc;
}

bool FileObserver2::isPublishInLocalTimeEnabled() const
{
    bdlqq::LockGuard<bdlqq::Mutex> guard(&d_mutex);
    return d_publishInLocalTime;
}

bdlt::DatetimeInterval FileObserver2::rotationLifetime() const
{
    bdlqq::LockGuard<bdlqq::Mutex> guard(&d_mutex);
    return d_rotationInterval;
}

int FileObserver2::rotationSize() const
{
    bdlqq::LockGuard<bdlqq::Mutex> guard(&d_mutex);
    return d_rotationSize;
}

bdlt::DatetimeInterval FileObserver2::localTimeOffset() const
{
    bdlqq::LockGuard<bdlqq::Mutex> guard(&d_mutex);
    bdlt::Datetime timestamp = d_logStreamBuf.isOpened()
                               ? d_logFileTimestampUtc
                              : bdlt::CurrentTime::utc();

    return localTimeOffsetInterval(timestamp);
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
