// bael_fileobserver2.h                                               -*-C++-*-
#ifndef INCLUDED_BAEL_FILEOBSERVER2
#define INCLUDED_BAEL_FILEOBSERVER2

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a thread-safe observer that emits log records to a file.
//
//@CLASSES:
//  bael_FileObserver2: observer that outputs log records to a file
//
//@SEE_ALSO: bael_record, bael_context, bael_observer,
//           bael_recordstringformatter
//
//@AUTHOR: Guillaume Morin (gmorin1), Gang Chen (gchen20),
//         Dino Oliva (doliva1), Raymond Chiu (schiu49)
//
//@DESCRIPTION: This component provides a concrete implementation of the
// 'bael_Observer' protocol for publishing log records to a user-specified
// file.  The following inheritance hierarchy diagram shows the classes
// involved and their methods:
//..
//                ,------------------.
//               ( bael_FileObserver2 )
//                `------------------'
//                         |              ctor
//                         |              disableFileLogging
//                         |              disableLifetimeRotation
//                         |              disableTimeIntervalRotation
//                         |              disableSizeRotation
//                         |              disablePublishInLocalTime
//                         |              enableFileLogging
//                         |              enablePublishInLocalTime
//                         |              forceRotation
//                         |              rotateOnSize
//                         |              rotateOnLifetime
//                         |              rotateOnTimeInterval
//                         |              setLogFileFunctor
//                         |              setOnFileRotationCallback
//                         |              isFileLoggingEnabled
//                         |              isPublishInLocaltimeEnabled
//                         |              rotationLifetime
//                         |              rotationSize
//                         |              localTimeOffset
//                         V
//                  ,-------------.
//                 ( bael_Observer )
//                  `-------------'
//                                        dtor
//                                        publish
//..
// A 'bael_FileObserver2' object processes the log records received through its
// 'publish' method by writing them to a user-specified file.  The format of
// published log records is user-configurable (see "Log Record Formatting"
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
//  18MAY2005_18:58:12.000 7959:1 WARN bael_fileobserver.t.cpp:404 TEST hello!
//..
// The default format can be overridden by calling 'setLogFileFunctor' which
// takes a formatting functor as an argument.  For example, an instance of
// 'bael_RecordStringFormatter' conveniently provides such a functor:
//..
//  fileObserver.setLogFileFunctor(
//                   bael_RecordStringFormatter("%i %p:%t %s %f:%l %c %m %u"));
//..
// The above statement will cause subsequent records to be logged in a format
// that is almost identical to the default format except that the timestamp
// attribute will be written in ISO 8601 format.  (See the component-level
// documentation of 'bael_recordstringformatter' for information on how format
// specifications are defined and interpreted.)
//
///Log Record Timestamps
///---------------------
// By default, the timestamp attributes of published records are written in UTC
// time.  If the default logging functor is in effect, this behavior can be
// changed by calling 'enablePublishInLocalTime' which will cause timestamp
// attributes to be written in local time instead.  This method will not have
// the intended effect if the 'setLogFileFunctor' method has been called to
// install an alternate logging functor from the default.  However, if the
// functor passed to the 'setLogFileFunctor' method is provided by a
// 'bael_RecordStringFormatter' object, the timestamp attribute of records can
// be configured to be written in local time or UTC time as desired.  (See the
// component-level documentation of 'bael_recordstringformatter' for more
// information on configuring the format of log records.)
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
//..
// For example, a log filename pattern of "task.log.%Y%M%D_%h%m%s" will yield
// the a filename with the appearance of "task.log.20110501_123000" if the file
// is opened on '01-May-2011 12:30:00'.
//
///Log File Rotation
///-----------------
// A 'bael_FileObserver2' may be configured to perform automatic rotation of
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
///Thread-Safety
///-------------
// All methods of 'bael_FileObserver2' are thread-safe, and can be called
// concurrently by multiple threads.
//
///Usage
///-----
// The following code fragments illustrate the essentials of using a file
// observer within a 'bael' logging system.
//
// First create a 'bael_FileObserver2' named 'fileObserver':
//..
//  bael_FileObserver2 fileObserver;
//..
// The file observer must then be installed within a 'bael' logging system.
// This is done by passing 'fileObserver' to the 'bael_LoggerManager'
// 'initSingleton' method:
//..
//  bael_LoggerManagerConfiguration configuration;
//  bael_LoggerManager::initSingleton(&fileObserver, configuration);
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
//  fileObserver.rotateOnTimeInterval(bdet_DatetimeInterval(1));
//      // Rotate the file every 24 hours.
//..
// Note that in this configuration the user may end up with multiple log files
// for a specific day (because of the rotation-on-size rule).  This feature can
// be disabled dynamically later:
//..
//  fileObserver.disableSizeRotation();
//..

#ifndef INCLUDED_BAESCM_VERSION
#include <baescm_version.h>
#endif

#ifndef INCLUDED_BAEL_OBSERVER
#include <bael_observer.h>
#endif

#ifndef INCLUDED_BAEL_SEVERITY
#include <bael_severity.h>
#endif

#ifndef INCLUDED_BCEMT_THREAD
#include <bcemt_thread.h>
#endif

#ifndef INCLUDED_BDET_DATETIME
#include <bdet_datetime.h>
#endif

#ifndef INCLUDED_BDET_DATETIMEINTERVAL
#include <bdet_datetimeinterval.h>
#endif

#ifndef INCLUDED_BDEF_FUNCTION
#include <bdef_function.h>
#endif

#ifndef INCLUDED_BSL_FSTREAM
#include <bsl_fstream.h>
#endif

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#endif

#ifndef INCLUDED_BSL_STRING
#include <bsl_string.h>
#endif

#ifndef INCLUDED_BDESU_FDSTREAMBUF
#include <bdesu_fdstreambuf.h>
#endif

#ifndef INCLUDED_BSLFWD_BSLMA_ALLOCATOR
#include <bslfwd_bslma_allocator.h>
#endif

namespace BloombergLP {

class bael_Context;
class bael_Record;

                          // ========================
                          // class bael_FileObserver2
                          // ========================

class bael_FileObserver2 : public bael_Observer {
    // This class implements the 'bael_Observer' protocol.  The 'publish'
    // method of this class outputs the log records that it receives to a
    // user-specified file.  This class is thread-safe; different threads can
    // operate on this object concurrently.  This class is exception-neutral
    // with no guarantee of rollback.  In no event is memory leaked.

  public:
    // PUBLIC TYPES
    typedef bdef_Function<void (*)(bsl::ostream&, const bael_Record&)>
                                                              LogRecordFunctor;
        // 'LogRecordFunctor' defines the type of the functor used for
        // formatting log records to a stream.

    // PUBLIC TYPES
    typedef bdef_Function<void (*)(int, const bsl::string&)>
                                                        OnFileRotationCallback;
        // An 'OnFileRotationCallback' is an alias for a user-supplied
        // callback function that is invoked after the file observer attempts
        // to rotate its log file.  The provided callback function takes two
        // parameters: (1) an integer status value, 0 indicates a new log file
        // was successfully created and a non-zero value indicates an error
        // occurred while rotating the file (2) a string that, if a file
        // rotation was successfully performed, indicates the name of the
        // rotated log file.
        // E.g.:
        //..
        //  void onLogFileRotation(int                rotationStatus,
        //                         const std::string& rotatedLogFileName);
        //..

  private:
    // DATA
    bdesu_FdStreamBuf      d_logStreamBuf;             // stream buffer for
                                                       // file logging

    bsl::ostream           d_logOutStream;             // output stream for
                                                       // file logging (refers
                                                       // to the buffer
                                                       // 'd_logStreamBuf')

    bsl::string            d_logFilePattern;           // log filename pattern

    bsl::string            d_logFileName;              // current filename
                                                       // after replacing
                                                       // pattern tokens

    bdet_Datetime          d_logFileTimestamp;         // the last modification
                                                       // time of the log file
                                                       // when it was opened
                                                       // (or the creation time
                                                       // if the log file did
                                                       // not already exist)

    LogRecordFunctor       d_logFileFunctor;           // formatting functor
                                                       // used when writing to
                                                       // log file

    bdet_DatetimeInterval  d_localTimeOffset;          // differential between
                                                       // local and UTC times

    bool                   d_publishInLocalTime;       // 'true' if timestamp
                                                       // attribute published
                                                       // in local time (UTC
                                                       // otherwise)

    mutable bcemt_Mutex    d_mutex;                    // serialize operations

    int                    d_rotationSize;             // maximum log file size
                                                       // before rotation

    bdet_Datetime          d_rotationReferenceLocalTime;
                                                       // the reference start
                                                       // *local* time for a
                                                       // time-based rotation

    bdet_DatetimeInterval  d_rotationInterval;         // time interval between
                                                       // two time-based
                                                       // rotations

    bdet_Datetime          d_nextRotationTime;         // the next scheduled
                                                       // time for time-based
                                                       // rotation

    OnFileRotationCallback d_onRotationCb;             // user-callback
                                                       // for file rotation

    mutable bcemt_Mutex    d_rotationCbMutex;          // serialize access to
                                                       // 'd_onRotationCb';
                                                       // required because
                                                       // callback must be
                                                       // called with 'd_mutex'
                                                       // unlocked

  private:
    // NOT IMPLEMENTED
    bael_FileObserver2(const bael_FileObserver2&);
    bael_FileObserver2& operator=(const bael_FileObserver2&);

  private:
    // PRIVATE MANIPULATORS
    void logRecordDefault(bsl::ostream& stream, const bael_Record& record);
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

    int rotateIfNecessary(bsl::string         *rotatedLogFileName,
                          const bdet_Datetime& currentLogTime);
        // Perform log file rotation if the specified 'currentLogTime' is
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
    // CREATORS
    explicit bael_FileObserver2(bslma_Allocator *basicAllocator = 0);
        // Create a file observer with file logging initially disabled.
        // Optionally specify a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.  Note that, when enabled for file logging, the timestamp
        // attribute of published records is written in UTC time by default.

    ~bael_FileObserver2();
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
        // time by this file observer.  This method has no effect if publishing
        // in local time is not enabled.

    int enableFileLogging(const char *logFilenamePattern);
        // Enable logging of all messages published to this file observer to a
        // file indicated by the specified 'logFilenamePattern'.  Return 0 on
        // success, a positive value if file logging is already enabled, and a
        // negative value otherwise.  The basename of 'logFilenamePattern' may
        // contain '%'-escape sequences that are interpreted as follows:
        //..
        //   %Y - current year (four digits with leading zeros)
        //   %M - current month (two digits with leading zeros)
        //   %D - current day (two digits with leading zeros)
        //   %h - current hour (two digits with leading zeros)
        //   %m - current minute (two digits with leading zeros)
        //   %s - current second (two digits with leading zeros)
        //   %T - current datetime, equivalent to "%Y%M%D_%h%m%s"
        //..
        // Each time a log file is opened by this file observer (upon a
        // successful call to this method and following each log file rotation)
        // the name of the log file is derived from 'logFilenamePattern' by
        // interpreting the above recognized '%'-escape sequences.  On
        // rotation, the current log file is closed, and a new log-file name is
        // derived.  If the newly derived log-file name is the same as that of
        // the current (rotated) log file, the current log file is renamed by
        // appending a timestamp of the form ".%Y%M%D_%h%m%s".  Then, the new
        // log file is opened for logging.

    int enableFileLogging(const char *logFilenamePattern,
                          bool        appendTimestampFlag);
        // Enable logging of all messages published to this file observer to a
        // file indicated by the specified 'logFilenamePattern' and append a
        // timestamp to the log filename if specified 'appendTimestampFlag' is
        // set to 'true'.  Return 0 on success, a positive value if file
        // logging is already enabled, and a negative value otherwise.  If the
        // 'appendTimestampFlag' is 'true' and 'logFilenamePattern' does not
        // contain any '%'-escape sequence, this method behaves as if ".%T" is
        // appended to 'logFilenamePattern'.
        //
        // DEPRECATED: Use 'enableFileLogging(logFilenamePattern)' instead (use
        // the ".%T" pattern to replicate 'appendTimestampFlag' being enabled).

    void enablePublishInLocalTime();
        // Enable publishing of the timestamp attribute of records in local
        // time by this file observer.  This method has no effect if publishing
        // in local time is already enabled.

    void publish(const bael_Record& record, const bael_Context& context);
        // Process the specified log 'record' having the specified publishing
        // 'context' by writing 'record' and 'context' to a file if file
        // logging is enabled for this file observer.  The method has no effect
        // if file logging is not enabled.

    void publish(const bcema_SharedPtr<const bael_Record>& record,
                 const bael_Context&                       context);
        // Process the record referred by the specified shared pointer 'record'
        // by writing the record and the specified 'context' of the record to
        // a file if file logging is enabled for this file observer.  The
        // method has no effect if file logging is not enabled.

    void releaseRecords();
        // Discard any shared reference to a 'bael_Record' object that was
        // supplied to the 'publish' method, and is held by this observer.
        // Note that this operation should be called if resources underlying
        // the previously provided shared-pointers must be released.

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

    void rotateOnLifetime(const bdet_DatetimeInterval& timeInterval);
        // Set this file observer to perform a periodic log-file rotation at
        // multiples of the specified 'interval'.  The behavior is undefined
        // unless '0 < interval.totalMilliseconds()'.  This rule replaces any
        // rotation-on-time-interval rule currently in effect.
        //
        // DEPRECATED: Use 'rotateOnTimeInterval' instead.

    void rotateOnTimeInterval(const bdet_DatetimeInterval& interval);
    void rotateOnTimeInterval(const bdet_DatetimeInterval& interval,
                              const bdet_Datetime&         referenceStartTime);
        // Set this file observer to perform a periodic log-file rotation at
        // multiples of the specified 'interval'.  Optionally, specify
        // 'referenceStartTime' indicating the *local* datetime to use as the
        // starting point for computing the periodic rotation schedule.  If
        // 'referenceStartTime' is unspecified, the current time is used.  The
        // behavior is undefined unless '0 < interval.totalMilliseconds()'.
        // This rule replaces any rotation-on-time-interval rule currently in
        // effect.  Note that 'referenceStartTime' may be a fixed time in the
        // past.  E.g., a reference time of 'bdet_Datetime(1, 1, 1)' and an
        // interval of 24 hours would configure a periodic rotation at midnight
        // each day.

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
        // file observer.

    // ACCESSORS
    bool isFileLoggingEnabled() const;
    bool isFileLoggingEnabled(bsl::string *result) const;
        // Return 'true' if file logging is enabled for this file observer, and
        // 'false' otherwise.  Load the optionally-specified 'result' with the
        // name of the current log file if file logging is enabled, and leave
        // 'result' uneffected otherwise.

    bool isPublishInLocalTimeEnabled() const;
        // Return 'true' if this file observer writes the timestamp attribute
        // of records that it publishes in local time, and 'false' otherwise.

    bdet_DatetimeInterval rotationLifetime() const;
        // Return the lifetime of the log file that will trigger a file
        // rotation by this file observer if rotation-on-lifetime is in effect,
        // and a 0 time interval otherwise.

    int rotationSize() const;
        // Return the size (in kilo-bytes) of the log file that will trigger a
        // file rotation by this file observer if rotation-on-size is in
        // effect, and 0 otherwise.

    bdet_DatetimeInterval localTimeOffset() const;
        // Return the difference between the local time and UTC time in effect
        // when this file observer was constructed.  Note that this value
        // remains unchanged during the lifetime of this object and therefore
        // may become incorrect when the difference between the local time and
        // UTC time changes (e.g., when transitioning into or out of daylight
        // savings time).
};

// ============================================================================
//                          INLINE FUNCTION DEFINITIONS
// ============================================================================

                          // ------------------------
                          // class bael_FileObserver2
                          // ------------------------

// MANIPULATORS
inline
void bael_FileObserver2::publish(
                             const bcema_SharedPtr<const bael_Record>& record,
                             const bael_Context&                       context)
{
    publish(*record, context);
}

inline
void bael_FileObserver2::releaseRecords()
{
}

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2005
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
