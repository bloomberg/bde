// ball_fileobserver2.h                                               -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BALL_FILEOBSERVER2
#define INCLUDED_BALL_FILEOBSERVER2

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
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
// 'ball::Observer' protocol for publishing log records to a user-specified
// file.  The following inheritance hierarchy diagram shows the classes
// involved and their methods:
//..
//                ,------------------.
//               ( ball::FileObserver2 )
//                `------------------'
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
//                         |              localTimeOffset
//                         V
//                  ,-------------.
//                 ( ball::Observer )
//                  `-------------'
//                                        dtor
//                                        publish
//                                        releaseRecords
//..
// A 'ball::FileObserver2' object processes the log records received through
// its 'publish' method by writing them to a user-specified file.  The format
// of published log records is user-configurable (see "Log Record Formatting"
// below).  In addition, a file observer may be configured to perform automatic
// log file rotation (see "Log File Rotation" below).
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
//  18MAY2005_18:58:12.000 7959:1 WARN ball_fileobserver.t.cpp:404 TEST hello!
//..
// The default format can be overridden by calling 'setLogFileFunctor' which
// takes a formatting functor as an argument.  For example, an instance of
// 'ball::RecordStringFormatter' conveniently provides such a functor:
//..
//  fileObserver.setLogFileFunctor(
//                  ball::RecordStringFormatter("%i %p:%t %s %f:%l %c %m %u"));
//..
// The above statement will cause subsequent records to be logged in a format
// that is almost identical to the default format except that the timestamp
// attribute will be written in ISO 8601 format.  (See the component-level
// documentation of 'ball_recordstringformatter' for information on how format
// specifications are defined and interpreted.)
//
///Log Record Timestamps
///---------------------
// By default, the timestamp attributes of published records are written in UTC
// time.  If the default logging functor is in effect, this behavior can be
// changed by calling 'enablePublishInLocalTime' which will cause timestamp
// attributes to be written in local time instead.  The local time offset is
// calculated using the UTC timestamp of each record.  Note that local time
// offsets for the calculation of log file names (see {Log Filename Pattern})
// use the local time offset in effect at construction.  Note that if the user
// installs a log record formatting functor using 'setLogFileFunctor' (that is
// not a 'ball::RecordStringFormatter' with 'PublishInLocalTime' enabled) the
// supplied functor determines how local time values are rendered to the log.
//
///Local Time Offset Calculations
/// - - - - - - - - - - - - - - -
// The calculation of the local time offset adds some overhead to the
// publication of each log record.  If that is problematic, the overhead can be
// mitigated if the owner of 'main installs a high-performance local-time
// offset callback for 'bdlt::CurrentTime'.  See {'bdetu_systemtime'} for
// details of installing such callback and see {'baltzo_localtimeoffsetutil'}
// for a an example facility.  Note that such callbacks can improve performance
// for all users of 'bdlt::CurrentTime', not just logging.
//
///Log Filename Pattern
///--------------------
// The 'enableFileLogging' method allow the use of '%'-escape sequences to
// specify the log filename.  The recognized sequences are as follows:
//..
//   %Y - current year (four digits with leading zeros)
//   %M - current month (two digits with leading zeros)
//   %D - current day (two digits with leading zeros)
//   %h - current hour (two digits with leading zeros)
//   %m - current minute (two digits with leading zeros)
//   %s - current second (two digits with leading zeros)
//   %T - current datetime, equivalent to "%Y%M%D_%h%m%s"
//   %p - process Id
//..
// For example, a log filename pattern of "task.log.%Y%M%D_%h%m%s" will yield
// the a filename with the appearance of "task.log.20110501_123000" if the file
// is opened on '01-May-2011 12:30:00'.
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
// both rules are in effect, log file rotation is performed when either rule
// applies.
//
///Rotated File Naming
///- - - - - - - - - -
// When file rotation occurs, a new log filename is generated using the pattern
// supplied to 'enableFileLogging'.  If the new log filename is the same as the
// old log filename, the old log file is renamed by appending a timestamp in
// the form ".%Y%M%D_%h%m%s".
//
// The table below demonstrates the names of the log files opened at
// '2011-May-11 12:30:00' and rotated in the same day based on the filename
// patterns:
//..
// ----------------+-------------------------+---------------------------------
//  Pattern        | Name Before Rotation    | Name After Rotation
// ----------------+-------------------------+---------------------------------
//  "a.log"        | "a.log"                 | "a.log.20110520_123000"
//  "a.log.%T"     | "a.log.20110520_123000" | "a.log.20110520_123000"
//  "a.log.%Y"     | "a.log.2011"            | "a.log.2011_20110520_123000"
//  "a.log.%Y%M%D" | "a.log.20110520"        | "a.log.20110520_20110520_123000"
// ----------------+-------------------------+---------------------------------
//..
// Note that timestamp pattern elements in a log file name are typically
// selected so they produce unique names for each rotation.
//
///Thread Safety
///-------------
// All methods of 'ball::FileObserver2' are thread-safe, and can be called
// concurrently by multiple threads.
//
///Usage
///-----
// The following code fragments illustrate the essentials of using a file
// observer within a 'ball' logging system.
//
// First create a 'ball::FileObserver2' named 'fileObserver':
//..
//  ball::FileObserver2 fileObserver;
//..
// The file observer must then be installed within a 'ball' logging system.
// This is done by passing 'fileObserver' to the 'ball::LoggerManager'
// 'initSingleton' method:
//..
//  ball::LoggerManagerConfiguration configuration;
//  ball::LoggerManager::initSingleton(&fileObserver, configuration);
//..
// Henceforth, all messages that are published by the logging system will be
// transmitted to the 'publish' method of 'fileObserver'.  The user can log
// all messages to a specified file and specify rotation rules based on the
// size of the log file or a periodic time interval:
//..
//  fileObserver.enableFileLogging("/var/log/task/task.log");
//      // Create and log records to a file named "/var/log/task/task.log".
//
//  fileObserver.rotateOnSize(1024 * 256);
//      // Rotate the file when its size becomes greater than or equal to 256
//      // mega-bytes.
//
//  fileObserver.rotateOnTimeInterval(bdlt::DatetimeInterval(1));
//      // Rotate the file every 24 hours.
//..
// Note that in this configuration the user may end up with multiple log files
// for a specific day (because of the rotation-on-size rule).  This feature can
// be disabled dynamically later:
//..
//  fileObserver.disableSizeRotation();
//..

#ifndef INCLUDED_BALSCM_VERSION
#include <balscm_version.h>
#endif

#ifndef INCLUDED_BALL_OBSERVER
#include <ball_observer.h>
#endif

#ifndef INCLUDED_BALL_SEVERITY
#include <ball_severity.h>
#endif

#ifndef INCLUDED_BSLMT_MUTEX
#include <bslmt_mutex.h>
#endif

#ifndef INCLUDED_BDLS_FDSTREAMBUF
#include <bdls_fdstreambuf.h>
#endif

#ifndef INCLUDED_BDLT_DATETIME
#include <bdlt_datetime.h>
#endif

#ifndef INCLUDED_BDLT_DATETIMEINTERVAL
#include <bdlt_datetimeinterval.h>
#endif

#ifndef INCLUDED_BSLMA_ALLOCATOR
#include <bslma_allocator.h>
#endif

#ifndef INCLUDED_BSLMA_USESBSLMAALLOCATOR
#include <bslma_usesbslmaallocator.h>
#endif

#ifndef INCLUDED_BSLMF_NESTEDTRAITDECLARATION
#include <bslmf_nestedtraitdeclaration.h>
#endif

#ifndef INCLUDED_BSL_FSTREAM
#include <bsl_fstream.h>
#endif

#ifndef INCLUDED_BSL_FUNCTIONAL
#include <bsl_functional.h>
#endif

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#endif

#ifndef INCLUDED_BSL_MEMORY
#include <bsl_memory.h>
#endif

#ifndef INCLUDED_BSL_STRING
#include <bsl_string.h>
#endif

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
    // file.  This class is thread-safe; different threads can operate on this
    // object concurrently.  This class is exception-neutral with no guarantee
    // of rollback.  In no event is memory leaked.

  public:
    // PUBLIC TYPES
    typedef bsl::function<void(bsl::ostream&, const Record&)> LogRecordFunctor;
        // 'LogRecordFunctor' defines the type of the functor used for
        // formatting log records to a stream.

    // PUBLIC TYPES
    typedef bsl::function<void(int, const bsl::string&)>
                                                        OnFileRotationCallback;
        // An 'OnFileRotationCallback' is an alias for a user-supplied callback
        // function that is invoked after the file observer attempts to rotate
        // its log file.  The provided callback function takes two parameters:
        // (1) an integer status value, 0 indicates a new log file was
        // successfully created and a non-zero value indicates an error
        // occurred while rotating the file (2) a string that, if a file
        // rotation was successfully performed, indicates the name of the
        // rotated log file.  E.g.:
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
                                                       // to the buffer
                                                       // 'd_logStreamBuf')

    bsl::string            d_logFilePattern;           // log filename pattern

    bsl::string            d_logFileName;              // current filename
                                                       // after replacing
                                                       // pattern tokens

    bdlt::Datetime         d_logFileTimestampUtc;      // the last modification
                                                       // time of the log file
                                                       // when it was opened
                                                       // (or the creation time
                                                       // if the log file did
                                                       // not already exist)

    LogRecordFunctor       d_logFileFunctor;           // formatting functor
                                                       // used when writing to
                                                       // log file

    bool                   d_publishInLocalTime;       // 'true' if timestamp
                                                       // attribute published
                                                       // in local time (UTC
                                                       // otherwise)

    mutable bslmt::Mutex   d_mutex;                    // serialize operations

    int                    d_rotationSize;             // maximum log file size
                                                       // before rotation

    bdlt::Datetime         d_rotationReferenceLocalTime;
                                                       // the reference start
                                                       // *local* time for a
                                                       // time-based rotation

    bdlt::DatetimeInterval d_rotationInterval;         // time interval between
                                                       // two time-based
                                                       // rotations

    bdlt::Datetime         d_nextRotationTimeUtc;      // the next scheduled
                                                       // time for time-based
                                                       // rotation

    OnFileRotationCallback d_onRotationCb;             // user-callback for
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
        // filename, as determined by the 'logFilenamePattern' of latest call
        // to 'enableFileLogging', is the same as the old log filename.

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
        // and the 'rotateOnSize' methods respectively.  The behavior is
        // undefined unless the caller acquired the lock for this object.

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(FileObserver2, bslma::UsesBslmaAllocator);

    // CREATORS
    explicit FileObserver2(bslma::Allocator *basicAllocator = 0);
        // Create a file observer with file logging initially disabled.
        // Optionally specify a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.  Note that, when enabled for file logging, the timestamp
        // attribute of published records is written in UTC time by default.

    ~FileObserver2();
        // Close the log file of this file observer if file logging is enabled,
        // and destroy this file observer.

    // MANIPULATORS
    void disableFileLogging();
        // Disable file logging for this file observer.  This method has no
        // effect if file logging is not enabled.

    void disableLifetimeRotation();
        // Disable log file rotation based on periodic time interval for this
        // file observer.  This method has no effect if
        // rotation-on-time-interval is not enabled.
        //
        // DEPRECATED: Use 'disableTimeIntervalRotation' instead.

    void disableTimeIntervalRotation();
        // Disable log file rotation based on periodic time interval for this
        // file observer.  This method has no effect if
        // rotation-on-time-interval is not enabled.

    void disableSizeRotation();
        // Disable log file rotation based on log file size for this file
        // observer.  This method has no effect if rotation-on-size is not
        // enabled.

    void disablePublishInLocalTime();
        // Disable publishing of the timestamp attribute of records in local
        // time by this file observer (time stamps will be in UTC time).  This
        // method has no effect if publishing in local time is not enabled.
        // This method also disables using local time values when replacing
        // '%'-escape sequences when generating a log file name (see
        // 'enableFileLogging').

    int enableFileLogging(const char *logFilenamePattern);
        // Enable logging of all messages published to this file observer to a
        // file indicated by the specified 'logFilenamePattern'.  Return 0 on
        // success, a positive value if file logging is already enabled, and a
        // negative value otherwise.  If file logging is already enabled, fail
        // with no effect.  The basename of 'logFilenamePattern' may contain
        // '%'-escape sequences that are interpreted as follows:
        //..
        //   %Y - current year (four digits with leading zeros)
        //   %M - current month (two digits with leading zeros)
        //   %D - current day (two digits with leading zeros)
        //   %h - current hour (two digits with leading zeros)
        //   %m - current minute (two digits with leading zeros)
        //   %s - current second (two digits with leading zeros)
        //   %T - current datetime, equivalent to "%Y%M%D_%h%m%s"
        //   %p - process Id
        //..
        // Each time a log file is opened by this file observer (upon a
        // successful call to this method and following each log file rotation)
        // the name of the log file is derived from 'logFilenamePattern' by
        // interpreting the above recognized '%'-escape sequences.  On
        // rotation, the current log file is closed, and a new log-file name is
        // derived.  If the newly derived log-file name is the same as that of
        // the current (rotated) log file, the current log file is renamed by
        // appending a timestamp of the form ".%Y%M%D_%h%m%s".  Then, the new
        // log file is opened for logging.  If the
        // 'isPublishInLocalTimeEnabled' attribute of this observer is 'true',
        // the '%'-escape sequences related to time will be substituted with
        // local time values, and UTC values otherwise.

    int enableFileLogging(const char *logFilenamePattern,
                          bool        appendTimestampFlag);
        // !DEPRECATED!: Use 'enableFileLogging(logFilenamePattern)' instead
        // (use the ".%T" pattern to replicate 'appendTimestampFlag' being
        // enabled).
        //
        // Enable logging of all messages published to this file observer to a
        // file indicated by the specified 'logFilenamePattern' and append a
        // timestamp to the log filename if specified 'appendTimestampFlag' is
        // set to 'true'.  Return 0 on success, a positive value if file
        // logging is already enabled, and a negative value otherwise.  If file
        // logging is already enabled, fail with no effect.  If the
        // 'appendTimestampFlag' is 'true' and 'logFilenamePattern' does not
        // contain any '%'-escape sequence, this method behaves as if ".%T" is
        // appended to 'logFilenamePattern'.

    void enablePublishInLocalTime();
        // Enable publishing of the timestamp attribute of records in local
        // time by this file observer.  This method has no effect if publishing
        // in local time is already enabled.    This method also enables using
        // local time values when replacing '%'-escape sequences when
        // generating a log file name (see 'enableFileLogging').

    void publish(const Record& record, const Context& context);
        // Process the specified log 'record' having the specified publishing
        // 'context' by writing 'record' and 'context' to a file if file
        // logging is enabled for this file observer.  The method has no effect
        // if file logging is not enabled.

    void publish(const bsl::shared_ptr<const Record>& record,
                 const Context&                       context);
        // Process the record referred by the specified shared pointer 'record'
        // by writing the record and the specified 'context' of the record to
        // a file if file logging is enabled for this file observer.  The
        // method has no effect if file logging is not enabled.

    void releaseRecords();
        // Discard any shared reference to a 'Record' object that was supplied
        // to the 'publish' method, and is held by this observer.  Note that
        // this operation should be called if resources underlying the
        // previously provided shared-pointers must be released.

    void forceRotation();
        // Forcefully perform a log file rotation by this file observer.  Close
        // the current log file, rename the log file if necessary, and open a
        // new log file.  See the "Rotated File Naming" section under
        // @DESCRIPTION in the component-level documentation for details on
        // filenames of the rotated log files.  This method has no effect if
        // file logging is not enabled.

    void rotateOnSize(int size);
        // Set this file observer to perform log file rotation when the size of
        // the file exceeds the specified 'size' (in kilo-bytes).  This rule
        // replaces any rotation-on-size rule currently in effect.  The
        // behavior is undefined unless 'size > 0'.

    void rotateOnLifetime(const bdlt::DatetimeInterval& timeInterval);
        // Set this file observer to perform a periodic log-file rotation at
        // multiples of the specified 'interval'.  The behavior is undefined
        // unless '0 < interval.totalMilliseconds()'.  This rule replaces any
        // rotation-on-time-interval rule currently in effect.
        //
        // DEPRECATED: Use 'rotateOnTimeInterval' instead.

    void rotateOnTimeInterval(const bdlt::DatetimeInterval& interval);
    void rotateOnTimeInterval(
                             const bdlt::DatetimeInterval& interval,
                             const bdlt::Datetime&         referenceStartTime);
        // Set this file observer to perform a periodic log-file rotation at
        // multiples of the specified 'interval'.  Optionally, specify
        // 'referenceStartTime' indicating the *local* datetime to use as the
        // starting point for computing the periodic rotation schedule.
        // 'referenceStartTime' is interpreted using the local-time offset at
        // the time this object was constructed.  If 'referenceStartTime' is
        // unspecified, the current time is used.  The behavior is undefined
        // unless '0 < interval.totalMilliseconds()'.  This rule replaces any
        // rotation-on-time-interval rule currently in effect.  Note that
        // 'referenceStartTime' may be a fixed time in the past.  E.g., a
        // reference time of 'bdlt::Datetime(1, 1, 1)' and an interval of 24
        // hours would configure a periodic rotation at midnight each day.

    void setLogFileFunctor(const LogRecordFunctor& logFileFunctor);
        // Set the formatting functor used when writing records to the log file
        // of this file observer to the specified 'logFileFunctor'.  Note that
        // a default format is in effect until this method is called.

    void setOnFileRotationCallback(
                             const OnFileRotationCallback& onRotationCallback);
        // Set the specified 'onRotationCallback' to be invoked after each time
        // this file observer attempts perform a log file rotation.  The
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
        // 'result' unaffected otherwise.

    bool isPublishInLocalTimeEnabled() const;
        // Return 'true' if this file observer writes the timestamp attribute
        // of records that it publishes in local time, and 'false' otherwise.

    bdlt::DatetimeInterval rotationLifetime() const;
        // Return the lifetime of the log file that will trigger a file
        // rotation by this file observer if rotation-on-lifetime is in effect,
        // and a 0 time interval otherwise.

    int rotationSize() const;
        // Return the size (in kilo-bytes) of the log file that will trigger a
        // file rotation by this file observer if rotation-on-size is in
        // effect, and 0 otherwise.

    bdlt::DatetimeInterval localTimeOffset() const;
        // !DEPRECATED!: Use 'bdlt::LocalTimeOffset' instead.
        //
        // Return the difference between the local time and UTC time in effect
        // when this file observer was constructed.  Note that this value
        // remains unchanged during the lifetime of this object and therefore
        // may become incorrect when the difference between the local time and
        // UTC time changes (e.g., when transitioning into or out of daylight
        // savings time).
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
