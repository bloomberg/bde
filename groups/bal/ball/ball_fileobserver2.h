// ball_fileobserver2.h                                               -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BALL_FILEOBSERVER2
#define INCLUDED_BALL_FILEOBSERVER2

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a thread-safe observer that emits log records to a file.
//
//@CLASSES:
//  ball::FileObserver2: observer that outputs log records to a file
//
//@SEE_ALSO: ball_record, ball_context, ball_observer,
//           ball_recordstringformatter
//
//@DESCRIPTION: This component provides a concrete implementation of the
// 'ball::Observer' protocol, 'ball::FileObserver2', for publishing log records
// to a user-specified file.  The following inheritance hierarchy diagram shows
// the classes involved and their methods:
//..
//                ,-------------------.
//               ( ball::FileObserver2 )
//                `-------------------'
//                         |              ctor
//                         |              disableFileLogging
//                         |              disableTimeIntervalRotation
//                         |              disableSizeRotation
//                         |              disablePublishInLocalTime
//                         |              enableFileLogging
//                         |              enablePublishInLocalTime
//                         |              forceRotation
//                         |              rotateOnSize
//                         |              rotateOnTimeInterval
//                         |              setLogFileFunctor
//                         |              setOnFileRotationCallback
//                         |              isFileLoggingEnabled
//                         |              isPublishInLocalTimeEnabled
//                         |              rotationLifetime
//                         |              rotationSize
//                         V
//                  ,--------------.
//                 ( ball::Observer )
//                  `--------------'
//                                        dtor
//                                        publish
//                                        releaseRecords
//..
// A 'ball::FileObserver2' object processes the log records received through
// its 'publish' method by writing them to a user-specified file.  Note that
// the 'enableFileLogging' method must be called to enable logging since
// logging to a file is initially disabled following construction.  The format
// of published log records is user-configurable (see {Log Record Formatting}
// below).  In addition, a file observer may be configured to perform automatic
// log file rotation (see {Log File Rotation} below).
//
///File Observer Configuration Synopsis
///------------------------------------
// 'ball::FileObserver2' offers several manipulators that may be used to
// configure various aspects of a file observer object.  These are summarized
// in the following table along with the accessors that can be used to query
// the current state of the configuration.  Further details are provided in the
// following sections and the function-level documentation.
//..
// +-------------+-----------------------------+------------------------------+
// | Aspect      | Manipulators                | Accessors                    |
// +=============+=============================+==============================+
// | Log Record  | setLogFileFunctor           |                              |
// | Formatting  |                             |                              |
// +-------------+-----------------------------+------------------------------+
// | Log Record  | enablePublishInLocalTime    | isPublishInLocalTimeEnabled  |
// | Timestamps  | disablePublishInLocalTime   |                              |
// +-------------+-----------------------------+------------------------------+
// | File        | enableFileLogging           | isFileLoggingEnabled         |
// | Logging     | disableFileLogging          |                              |
// +-------------+-----------------------------+------------------------------+
// | Log File    | rotateOnSize                | rotationSize                 |
// | Rotation    | rotateOnTimeInterval        | rotationLifetime             |
// |             | disableSizeRotation         |                              |
// |             | disableTimeIntervalRotation |                              |
// |             | setOnFileRotationCallback   |                              |
// +-------------+-----------------------------+------------------------------+
//..
// In general, a 'ball::FileObserver2' object can be dynamically configured
// throughout its lifetime (in particular, before or after being registered
// with a logger manager).  However, note that for 'ball::FileObserver2',
// configuration changes that affect how records are logged (e.g.,
// 'enablePublishInLocalTime' and 'disablePublishInLocalTime') impact only
// those records that are published subsequent to making the configuration
// change.
//
///Log Record Formatting
///---------------------
// By default, the output format of published log records is:
//..
//  DATE_TIME PID:THREAD-ID SEVERITY FILE:LINE CATEGORY MESSAGE USER-FIELDS
//..
// where 'DATE' and 'TIME' are of the form 'DDMonYYYY' and 'HH:MM:SS.mmm',
// respectively ('Mon' being the 3-letter abbreviation for the month).  For
// example, assuming that no user-defined fields are present, a log record
// will have the following appearance when the default format is in effect:
//..
//  18MAY2005_18:58:12.076 7959:1 WARN ball_fileobserver2.t.cpp:404 TEST hello!
//..
// The default format can be overridden by supplying a suitable formatting
// functor to 'setLogFileFunctor'.  For example, an instance of
// 'ball::RecordStringFormatter' conveniently provides such a functor:
//..
//  fileObserver.setLogFileFunctor(
//              ball::RecordStringFormatter("\n%I %p:%t %s %f:%l %c %m %u\n"));
//..
// The above statement will cause subsequent records to be logged in a format
// that is almost identical to the default format except that the timestamp
// attribute will be written in ISO 8601 format.  See
// {'ball_recordstringformatter'} for information on how format specifications
// are defined and interpreted.
//
// Note that the observer emits newline characters at the beginning and at the
// end of a log record by default, so the user needs to add them explicitly to
// the format string to preserve this behavior.
//
// Also note that in the sample message above the timestamp has millisecond
// precision ('18MAY2005_18:58:12.076').  If microsecond precision is desired
// instead, consider using either the '%D' or '%O' format specification
// supported by 'ball_recordstringformatter'.
//
///Log Record Timestamps
///---------------------
// By default, the timestamp attributes of published records are written in UTC
// time.  To write timestamps in local time instead, call the
// 'enablePublishInLocalTime' method.  Note that the local time offset is
// calculated using the UTC timestamp of each record.  To revert to UTC time,
// call the 'disablePublishInLocalTime' method.  Whether UTC time or local time
// is in effect can be queried via 'isPublishInLocalTimeEnabled'.  However,
// note that if the user installs a log record formatting functor via
// 'setLogFileFunctor' that is not a 'ball::RecordStringFormatter', then the
// supplied functor determines how record timestamps are rendered to the log.
//
///Local Time Offset Calculations
/// - - - - - - - - - - - - - - -
// The calculation of the local time offset adds some overhead to the
// publication of each log record.  If this overhead is an issue, it can be
// mitigated by installing a high-performance local-time offset callback for
// 'bdlt::CurrentTime' in 'main'.  See {'bsls_systemtime'} for the details of
// installing such a callback and see {'baltzo_localtimeoffsetutil'} for an
// example facility.  Note that such callbacks can improve performance for all
// users of 'bdlt::CurrentTime', not just the 'ball' logger.
//
///Log Filename Patterns
///---------------------
// The 'enableFileLogging' method supports the use of '%'-escape sequences to
// specify log filenames.  The recognized sequences are as follows:
//..
//  %Y - current year   (4 digits with leading zeros)
//  %M - current month  (2 digits with leading zeros)
//  %D - current day    (2 digits with leading zeros)
//  %h - current hour   (2 digits with leading zeros)
//  %m - current minute (2 digits with leading zeros)
//  %s - current second (2 digits with leading zeros)
//  %T - current datetime, equivalent to "%Y%M%D_%h%m%s"
//  %p - process ID
//..
// The date and time elements of the derived filename are based on the time
// when the log file is created.  Furthermore, these elements are based on
// either UTC time or local time depending on the value returned by
// 'isPublishInLocalTimeEnabled'.  (See {Log Record Timestamps} for the
// similarity with the treatment of record timestamps.)
//
// For example, a log filename pattern of "task.log.%Y%M%D_%h%m%s" will yield
// the filename 'task.log.20110501_123000' if the file is created on
// 01-May-2011 at 12:30:00 local time (assuming 'enablePublishInLocalTime' was
// called).
//
///Log File Rotation
///-----------------
// A 'ball::FileObserver2' may be configured to perform automatic rotation of
// log files based on simple file rotation conditions (or rules).
//
///File Rotation Conditions
/// - - - - - - - - - - - -
// Rotation rules may be established based on the size of the log file (i.e., a
// "rotation-on-size" rule), and a periodic time interval (i.e., a
// "rotation-on-time-interval" rule).  These rules are independently enabled by
// the 'rotateOnSize' and 'rotateOnTimeInterval' methods, respectively.  If
// both rules are in effect, log file rotation is performed whenever either
// rule applies.
//
///Rotated File Naming
///- - - - - - - - - -
// When a log file is rotated, a new filename is generated using the pattern
// supplied to 'enableFileLogging'.  If the new filename is the same as the old
// filename, the old file is renamed by appending a timestamp in the form
// ".%Y%M%D_%h%m%s" where the timestamp indicates when the file being rotated
// was last opened (the time of either the last file rotation or the last call
// to 'enableFileLogging', whichever is most recent).  As with timestamps of
// logged records, the timestamps appended to log filenames upon rotation will
// be in UTC time or local time depending on the value returned by
// 'isPublishInLocalTimeEnabled'.
//
// The two tables below illustrate the names of old and new log files when a
// file rotation occurs.  We assume that the log file is rotated on 2011-May-20
// at 16:45:00 local time and that the last rotation occurred at 12:30:00 on
// the same day.  We further assume that 'enablePublishInLocalTime' was called,
// so that all date and time elements are rendered in local time.
//
// The first table shows the name change (if any) of the (old) log file being
// rotated:
//..
//  +----------------+-------------------------+-------------------------------
//  | Pattern        | Log Before Rotation     | Rotated Log Filename
//  +----------------+-------------------------+-------------------------------
//  | "a.log"        | a.log                   | a.log.20110520_123000
//  | "a.log.%T"     | a.log.20110520_123000   | a.log.20110520_123000
//  | "a.log.%Y"     | a.log.2011              | a.log.2011.20110520_123000
//  | "a.log.%Y%M%D" | a.log.20110520          | a.log.20110520.20110520_123000
//  +----------------+-------------------------+-------------------------------
//..
// Note that upon rotation a timestamp was appended to the name of the rotated
// file in all cases except where the filename pattern includes "%T".  In that
// case, the name of the file before rotation indicates that the file was
// created at 12:30:00 on the same day of the rotation being illustrated.
//
// The next table shows the (possibly new) name of the (new) log file following
// rotation:
//..
//  +----------------+-------------------------+
//  | Pattern        | Log After Rotation      |
//  +----------------+-------------------------+
//  | "a.log"        | a.log                   |
//  | "a.log.%T"     | a.log.20110520_164500   |
//  | "a.log.%Y"     | a.log.2011              |
//  | "a.log.%Y%M%D" | a.log.20110520          |
//  +----------------+-------------------------+
//..
// Note that the original filename is reused in all cases except where "%T" is
// used in the filename pattern.  In that case, a unique name on each rotation
// is produced with the (local) time at which file rotation occurred embedded
// in the filename.  In any case, logging resumes to a new, initially empty,
// file.
//
///Thread Safety
///-------------
// All methods of 'ball::FileObserver2' are thread-safe, and can be called
// concurrently by multiple threads.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Publication Through the Logger Manager
///- - - - - - - - - - - - - - - - - - - - - - - - -
// This example demonstrates using a 'ball::FileObserver2' within the 'ball'
// logging system.
//
// First, we initialize the 'ball' logging subsystem with the default
// configuration:
//..
//  ball::LoggerManagerConfiguration configuration;
//  ball::LoggerManagerScopedGuard   guard(configuration);
//
//  ball::LoggerManager& manager = ball::LoggerManager::singleton();
//..
// Note that the application is now prepared to log messages using the 'ball'
// logging subsystem, but until the application registers an observer, all log
// records will be discarded.
//
// Then, we create a shared pointer to a 'ball::FileObserver2' object,
// 'observerPtr', having default attributes:
//..
//  bslma::Allocator *alloc =  bslma::Default::globalAllocator(0);
//  bsl::shared_ptr<ball::FileObserver2> observerPtr(
//                                           new(*alloc) ball::FileObserver2(),
//                                           alloc);
//..
// Next, we configure the log file rotation rules:
//..
//  observerPtr->rotateOnSize(1024 * 128);
//      // Rotate the file when its size becomes greater than or equal to 128
//      // megabytes.
//
//  observerPtr->rotateOnTimeInterval(bdlt::DatetimeInterval(1));
//      // Rotate the file every 24 hours.
//..
// Note that in this configuration the user may end up with multiple log files
// for a specific day (because of the rotation-on-size rule).
//
// Then, we enable logging to a file:
//..
//  observerPtr->enableFileLogging("/var/log/task/task.log");
//      // Create and log records to a file named "/var/log/task/task.log".
//..
// Finally, we register the file observer with the logger manager.  Upon
// successful registration, the observer will start to receive log records via
// the 'publish' method:
//..
//  int rc = manager.registerObserver(observerPtr, "fileObserver");
//  assert(0 == rc);
//..

#include <balscm_version.h>

#include <ball_observer.h>
#include <ball_severity.h>

#include <bdls_fdstreambuf.h>

#include <bdlt_datetime.h>
#include <bdlt_datetimeinterval.h>

#include <bslma_allocator.h>
#include <bslma_usesbslmaallocator.h>

#include <bslmf_nestedtraitdeclaration.h>

#include <bslmt_mutex.h>

#include <bsl_fstream.h>
#include <bsl_functional.h>
#include <bsl_iosfwd.h>
#include <bsl_memory.h>
#include <bsl_string.h>

namespace BloombergLP {
namespace ball {

class Context;
class Record;

                          // ===================
                          // class FileObserver2
                          // ===================

class FileObserver2 : public Observer {
    // This class implements the 'Observer' protocol.  The 'publish' method of
    // this class outputs the log records that it receives to a user-specified
    // file.  This class is thread-safe; different threads can operate on an
    // object concurrently.  This class is exception-neutral with no guarantee
    // of rollback.  In no event is memory leaked.

  public:
    // PUBLIC TYPES
    typedef bsl::function<void(bsl::ostream&, const Record&)> LogRecordFunctor;
        // 'LogRecordFunctor' is an alias for the type of the functor used for
        // formatting log records to a stream.

    typedef bsl::function<void(int, const bsl::string&)>
                                                        OnFileRotationCallback;
        // 'OnFileRotationCallback' is an alias for a user-supplied callback
        // function that is invoked after the file observer attempts to rotate
        // its log file.  The callback takes two arguments: (1) an integer
        // status value where 0 indicates a new log file was successfully
        // created and a non-zero value indicates an error occurred during
        // rotation, and (2) a string that provides the name of the rotated log
        // file if the rotation was successful.  E.g.:
        //..
        //  void onLogFileRotation(int                rotationStatus,
        //                         const bsl::string& rotatedLogFileName);
        //..

  private:
    // DATA
    bdls::FdStreamBuf      d_logStreamBuf;             // stream buffer for
                                                       // file logging

    bsl::ostream           d_logOutStream;             // output stream for
                                                       // file logging (refers
                                                       // to 'd_logStreamBuf')

    bsl::string            d_logFilePattern;           // log filename pattern

    bsl::string            d_logFileName;              // current log filename

    bdlt::Datetime         d_logFileTimestampUtc;      // modification time of
                                                       // the log file when it
                                                       // was opened (or the
                                                       // creation time if the
                                                       // log file did not
                                                       // already exist)

    LogRecordFunctor       d_logFileFunctor;           // formatting functor
                                                       // used when writing to
                                                       // log file

    bool                   d_publishInLocalTime;       // 'true' if timestamps
                                                       // of records are output
                                                       // in local time,
                                                       // otherwise UTC time

    mutable bslmt::Mutex   d_mutex;                    // serialize operations

    int                    d_rotationSize;             // maximum log file size
                                                       // before rotation (in
                                                       // kilobytes)

    bdlt::Datetime         d_rotationReferenceLocalTime;
                                                       // reference *local*
                                                       // start time for
                                                       // time-based rotation

    bdlt::DatetimeInterval d_rotationInterval;         // time interval between
                                                       // two time-based
                                                       // rotations

    bdlt::Datetime         d_nextRotationTimeUtc;      // next scheduled time
                                                       // for time-based
                                                       // rotation

    OnFileRotationCallback d_onRotationCb;             // user callback
                                                       // invoked following
                                                       // file rotation

    mutable bslmt::Mutex   d_rotationCbMutex;          // serialize access to
                                                       // 'd_onRotationCb';
                                                       // required because
                                                       // callback must be
                                                       // called with 'd_mutex'
                                                       // unlocked

  private:
    // NOT IMPLEMENTED
    FileObserver2(const FileObserver2&);
    FileObserver2& operator=(const FileObserver2&);

  private:
    // PRIVATE MANIPULATORS
    void logRecordDefault(bsl::ostream& stream, const Record& record);
        // Write the specified log 'record' to the specified output 'stream'
        // using the default record format of this file observer.

    int rotateFile(bsl::string *rotatedLogFileName);
        // Perform a log file rotation by closing the current log file of this
        // file observer, renaming the closed log file if necessary, and
        // opening a new log file.  Load, into the specified
        // 'rotatedLogFileName', the name of the rotated log file.  Return 0 on
        // success, a positive value if logging is not enabled, and a negative
        // value otherwise.  The existing log file is renamed if the new log
        // filename, as determined by the 'logFilenamePattern' of the latest
        // call to 'enableFileLogging', is the same as the old log filename.

    int rotateIfNecessary(bsl::string           *rotatedLogFileName,
                          const bdlt::Datetime&  currentLogTimeUtc);
        // Perform log file rotation if the specified 'currentLogTimeUtc' is
        // later than the scheduled rotation time of the current log file, or
        // if the log file is larger than the allowable size, and if a rotation
        // is performed, load into the specified 'rotatedLogFileName' the name
        // of the rotated file.  Return 0 if the log file is rotated
        // successfully, a positive value if a rotation was determined to be
        // unnecessary, and a negative value otherwise.  The rotation schedule
        // and the allowable file size are set by the 'rotateOnTimeInterval'
        // and the 'rotateOnSize' methods, respectively.  The behavior is
        // undefined unless the caller acquired the lock for this object.

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(FileObserver2, bslma::UsesBslmaAllocator);

    // CREATORS
    explicit FileObserver2(bslma::Allocator *basicAllocator = 0);
        // Create a file observer with file logging initially disabled.
        // Optionally specify a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.  Note that 'isPublishInLocalTimeEnabled' returns 'false'
        // following construction indicating that the timestamp attribute of
        // published records will be written in UTC time (see
        // 'enablePublishInLocalTime').  Also note that a default record format
        // is in effect for file logging (see 'setLogFileFunctor').

    ~FileObserver2();
        // Close the log file of this file observer if file logging is enabled,
        // and destroy this file observer.

    // MANIPULATORS
    void disableFileLogging();
        // Disable file logging for this file observer.  This method has no
        // effect if file logging is not enabled.  Note that records
        // subsequently received through the 'publish' method will be dropped
        // until file logging is reenabled.

    void disableLifetimeRotation();
        // Disable log file rotation based on a periodic time interval for this
        // file observer.  This method has no effect if
        // rotation-on-time-interval is not enabled.
        //
        // !DEPRECATED!: Use 'disableTimeIntervalRotation' instead.

    void disableTimeIntervalRotation();
        // Disable log file rotation based on a periodic time interval for this
        // file observer.  This method has no effect if
        // rotation-on-time-interval is not enabled.

    void disableSizeRotation();
        // Disable log file rotation based on log file size for this file
        // observer.  This method has no effect if rotation-on-size is not
        // enabled.

    void disablePublishInLocalTime();
        // Disable publishing of the timestamp attribute of records in local
        // time by this file observer; henceforth, timestamps will be in UTC
        // time.  This method has no effect if publishing in local time is not
        // enabled.  Note that this method also affects log filenames (see {Log
        // Filename Patterns}).

    int enableFileLogging(const char *logFilenamePattern);
        // Enable logging of all records published to this file observer to a
        // file whose name is derived from the specified 'logFilenamePattern'.
        // Return 0 on success, a positive value if file logging is already
        // enabled (with no effect), and a negative value otherwise.  The
        // basename of 'logFilenamePattern' may contain '%'-escape sequences
        // that are interpreted as follows:
        //..
        //   %Y - current year   (4 digits with leading zeros)
        //   %M - current month  (2 digits with leading zeros)
        //   %D - current day    (2 digits with leading zeros)
        //   %h - current hour   (2 digits with leading zeros)
        //   %m - current minute (2 digits with leading zeros)
        //   %s - current second (2 digits with leading zeros)
        //   %T - current datetime, equivalent to "%Y%M%D_%h%m%s"
        //   %p - process ID
        //..
        // Each time a log file is opened by this file observer (upon a
        // successful call to this method and following each log file
        // rotation), the name of the new log file is derived from
        // 'logFilenamePattern' by interpreting the above recognized '%'-escape
        // sequences.  If 'isPublishInLocalTimeEnabled' returns 'true', the
        // '%'-escape sequences related to time will be substituted with local
        // time values, and UTC time values otherwise.  See {Log Filename
        // Patterns}.

    int enableFileLogging(const char *logFilenamePattern,
                          bool        appendTimestampFlag);
        // Enable logging of all records published to this file observer to a
        // file whose name is derived from the specified 'logFilenamePattern'
        // and append a timestamp to the log filename if the specified
        // 'appendTimestampFlag' is 'true'.  Return 0 on success, a positive
        // value if file logging is already enabled (with no effect), and a
        // negative value otherwise.  If the 'appendTimestampFlag' is 'true'
        // and 'logFilenamePattern' does not contain any '%'-escape sequences,
        // this method behaves as if ".%T" is appended to 'logFilenamePattern'.
        //
        // !DEPRECATED!: Use 'enableFileLogging(logFilenamePattern)' instead
        // (use the ".%T" pattern to replicate 'true == appendTimestampFlag'
        // behavior).

    void enablePublishInLocalTime();
        // Enable publishing of the timestamp attribute of records in local
        // time by this file observer.  This method has no effect if publishing
        // in local time is already enabled.  Note that this method also
        // affects log filenames (see {Log Filename Patterns}).

    void publish(const Record& record, const Context& context);
        // Process the specified log 'record' having the specified publishing
        // 'context' by writing 'record' and 'context' to the current log file
        // if file logging is enabled for this file observer.  The method has
        // no effect if file logging is not enabled, in which case 'record' is
        // dropped.

    void publish(const bsl::shared_ptr<const Record>& record,
                 const Context&                       context);
        // Process the record referenced by the specified 'record' shared
        // pointer having the specified publishing 'context' by writing the
        // record and 'context' to the current log file if file logging is
        // enabled for this file observer.  The method has no effect if file
        // logging is not enabled, in which case 'record' is dropped.

    void releaseRecords();
        // Discard any shared references to 'Record' objects that were supplied
        // to the 'publish' method, and are held by this observer.  Note that
        // this operation should be called if resources underlying the
        // previously provided shared pointers must be released.

    void forceRotation();
        // Forcefully perform a log file rotation by this file observer.  Close
        // the current log file, rename the log file if necessary, and open a
        // new log file.  This method has no effect if file logging is not
        // enabled.  See {Rotated File Naming} for details on filenames of
        // rotated log files.

    void rotateOnSize(int size);
        // Set this file observer to perform log file rotation when the size of
        // the file exceeds the specified 'size' (in kilobytes).  This rule
        // replaces any rotation-on-size rule currently in effect.  The
        // behavior is undefined unless 'size > 0'.

    void rotateOnLifetime(const bdlt::DatetimeInterval& timeInterval);
        // Set this file observer to perform a periodic log file rotation at
        // multiples of the specified 'timeInterval'.  This rule replaces any
        // rotation-on-time-interval rule currently in effect.  The behavior is
        // undefined unless '0 < timeInterval.totalMilliseconds()'.
        //
        // !DEPRECATED!: Use 'rotateOnTimeInterval' instead.

    void rotateOnTimeInterval(const bdlt::DatetimeInterval& interval);
    void rotateOnTimeInterval(const bdlt::DatetimeInterval& interval,
                              const bdlt::Datetime&         startTime);
        // Set this file observer to perform a periodic log file rotation at
        // multiples of the specified 'interval'.  Optionally specify a
        // 'startTime' indicating the *local* datetime to use as the starting
        // point for computing the periodic rotation schedule.  If 'startTime'
        // is not specified, the current time is used.  This rule replaces any
        // rotation-on-time-interval rule currently in effect.  The behavior is
        // undefined unless '0 < interval.totalMilliseconds()'.  Note that
        // 'startTime' may be a fixed time in the past; e.g., a reference time
        // of 'bdlt::Datetime(1, 1, 1)' and an interval of 24 hours would
        // configure a periodic rotation at midnight each day.

    void setLogFileFunctor(const LogRecordFunctor& logFileFunctor);
        // Set the formatting functor used when writing records to the log file
        // of this file observer to the specified 'logFileFunctor'.  Note that
        // a default format ("\n%d %p:%t %s %f:%l %c %m %u\n") is in effect
        // until this method is called (see 'ball_recordstringformatter').
        // Also note that the observer emits newline characters at the
        // beginning and at the end of a log record by default, so the user
        // needs to add them explicitly to the format string to preserve this
        // behavior.


    void setOnFileRotationCallback(
                             const OnFileRotationCallback& onRotationCallback);
        // Set the specified 'onRotationCallback' to be invoked after each time
        // this file observer attempts to perform a log file rotation.  The
        // behavior is undefined if the supplied function calls either
        // 'setOnFileRotationCallback', 'forceRotation', or 'publish' on this
        // file observer (i.e., the supplied callback should *not* attempt to
        // write to the 'ball' log).

    // ACCESSORS
    bool isFileLoggingEnabled() const;
    bool isFileLoggingEnabled(bsl::string *result) const;
        // Return 'true' if file logging is enabled for this file observer, and
        // 'false' otherwise.  Load the optionally specified 'result' with the
        // name of the current log file if file logging is enabled, and leave
        // 'result' unmodified otherwise.  Note that records received through
        // the 'publish' method of this file observer will be dropped when this
        // method returns 'false'.

    bool isPublishInLocalTimeEnabled() const;
        // Return 'true' if this file observer writes the timestamp attribute
        // of records that it publishes in local time, and 'false' otherwise
        // (in which case timestamps are written in UTC time).  Note that the
        // value returned by this method also affects log filenames (see {Log
        // Filename Patterns}).

    bdlt::DatetimeInterval rotationLifetime() const;
        // Return the lifetime of the log file that will trigger a file
        // rotation by this file observer if rotation-on-lifetime is in effect,
        // and a 0 time interval otherwise.

    int rotationSize() const;
        // Return the size (in kilobytes) of the log file that will trigger a
        // file rotation by this file observer if rotation-on-size is in
        // effect, and 0 otherwise.

    bdlt::DatetimeInterval localTimeOffset() const;
        // Return the difference between the local time and UTC time in effect
        // when this file observer was constructed.  Note that this value
        // remains unchanged during the lifetime of this object and therefore
        // may become incorrect when the difference between the local time and
        // UTC time changes (e.g., when transitioning into or out of daylight
        // savings time).
        //
        // !DEPRECATED!: Use 'bdlt::LocalTimeOffset' instead.
};

// ============================================================================
//                              INLINE DEFINITIONS
// ============================================================================

                          // -------------------
                          // class FileObserver2
                          // -------------------

// MANIPULATORS
inline
void FileObserver2::publish(const bsl::shared_ptr<const Record>& record,
                            const Context&                       context)
{
    publish(*record, context);
}

inline
void FileObserver2::releaseRecords()
{
}

}  // close package namespace
}  // close enterprise namespace

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
