// ball_fileobserver.h                                                -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BALL_FILEOBSERVER
#define INCLUDED_BALL_FILEOBSERVER

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a thread-safe observer that logs to a file and to 'stdout'.
//
//@CLASSES:
//  ball::FileObserver: observer that writes log records to a file and 'stdout'
//
//@SEE_ALSO: ball_record, ball_context, ball_observer, ball_fileobserver2
//
//@DESCRIPTION: This component provides a concrete implementation of the
// 'ball::Observer' protocol for publishing log records to a user-specified
// file.  The following inheritance hierarchy diagram shows the classes
// involved and their methods:
//..
//                ,-----------------.
//               ( ball::FileObserver )
//                `-----------------'
//                         |              ctor
//                         |              disableFileLogging
//                         |              disableTimeIntervalRotation
//                         |              disableSizeRotation
//                         |              disableStdoutLoggingPrefix
//                         |              disablePublishInLocalTime
//                         |              enableFileLogging
//                         |              enableStdoutLoggingPrefix
//                         |              enablePublishInLocalTime
//                         |              forceRotation
//                         |              rotateOnSize
//                         |              rotateOnTimeInterval
//                         |              setOnFileRotationCallback
//                         |              setStdoutThreshold
//                         |              setLogFormat
//                         |              isFileLoggingEnabled
//                         |              isStdoutLoggingPrefixEnabled
//                         |              isUserFieldsLoggingEnabled
//                         |              isPublishInLocalTimeEnabled
//                         |              rotationLifetime
//                         |              rotationSize
//                         |              localTimeOffset
//                         |              stdoutThreshold
//                         |              getLogFormat
//                         V
//                  ,-------------.
//                 ( ball::Observer )
//                  `-------------'
//                                        dtor
//                                        publish
//                                        releaseRecords
//..
// A 'ball::FileObserver' object processes the log records received through its
// 'publish' method by writing them to a user-specified file.  The format of
// published log records is user-configurable (see "Log Record Formatting"
// below).  In addition, a file observer may be configured to perform automatic
// log file rotation (see "Log File Rotation" below).
//
///Log Record Formatting
///---------------------
// By default, the output format of published log records (whether to 'stdout'
// or a user-specified file) is:
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
// The default format for records published to 'stdout' can be shortened by
// calling 'disableStdoutLoggingPrefix'.  This method has the effect of
// reducing the above example message to:
//..
//  WARN ball_fileobserver.t.cpp:404 TEST hello!
//..
// For additional flexibility, the 'setLogFormat' method can be called to
// configure the format of published records.  This method takes two arguments:
// one specifies the format of records logged to 'stdout' and the other applies
// to records that are logged to a file.  The respective formats are specified
// using 'printf'-style ('%'-prefixed) conversion specifications.  (See the
// component-level documentation of 'ball_recordstringformatter' for
// information on how format specifications are defined and interpreted.)  For
// example, the following statement will force subsequent records to be logged
// in a format that is almost identical to the default (long) format except
// that the timestamp attribute will be written in ISO 8601 format:
//..
//   fileObserver.setLogFormat("%i %p:%t %s %f:%l %c %m %u",
//                             "%i %p:%t %s %f:%l %c %m %u");
//..
// Note that once a customized format is specified for 'stdout', calling
// 'disableStdoutLoggingPrefix' will switch to the default short format (i.e.,
// "%s %f:%l %c %m %u").  If 'enableStdoutLoggingPrefix' is subsequently
// called, the customized format specified in an earlier call to 'setLogFormat'
// will be reinstated.
//
///Log Record Timestamps
///---------------------
// By default, the timestamp attributes of published records are written in UTC
// time.  This behavior can be changed by calling 'enablePublishInLocalTime'
// which will cause timestamp attributes to be written in local time instead.
// The local time offset is calculated using the UTC timestamp of each record.
// Note that local time offsets for the calculation of log file names (see {Log
// Filename Pattern}) use the local time offset in effect at construction.
//
///Local Time Offset Calculations
/// - - - - - - - - - - - - - - -
// The calculation of the local time offset adds some overhead to the
// publication of each log record.  If that is problematic, the overhead can be
// mitigated if the owner of 'main' installs a high-performance local-time
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
// A 'ball::FileObserver' may be configured to perform automatic rotation of
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
// All methods of 'ball::FileObserver' are thread-safe, and can be called
// concurrently by multiple threads.
//
///Usage
///-----
// The following code fragments illustrate the essentials of using a file
// observer within a 'ball' logging system.
//
// First create a 'ball::FileObserver' named 'fileObserver':
//..
//  ball::FileObserver fileObserver;
//..
// The file observer must then be installed within a 'ball' logging system.
// This is done by passing 'fileObserver' to the 'ball::LoggerManager'
// 'initSingleton' method:
//..
//  ball::LoggerManagerConfiguration configuration;
//  ball::LoggerManager::initSingleton(&fileObserver, configuration);
//..
// Optionally, the format can be changed by calling the 'setLogFormat' method.
// The statement below outputs timestamps in ISO 8601 format to a log file and
// in 'bdet'-style (default) format to 'stdout':
//..
//  observer.setLogFormat("%i %p:%t %s %f:%l %c %m",
//                        "%d %p:%t %s %f:%l %c %m");
//..
// Henceforth, all messages that are published by the logging system will be
// transmitted to the 'publish' method of 'fileObserver'.  By default, only the
// messages with a 'WARN', 'ERROR' or 'FATAL' severity will be logged to
// 'stdout':
//..
//  BALL_LOG_INFO << "Will not be published on 'stdout'."
//                << BALL_LOG_END;
//  BALL_LOG_WARN << "This warning *will* be published on 'stdout'."
//                << BALL_LOG_END;
//..
// This default can be changed by specifying an optional argument to the
// 'ball::FileObserver' constructor or by calling the 'setStdoutThreshold'
// method:
//..
//  fileObserver.setStdoutThreshold(ball::Severity::e_INFO);
//  BALL_LOG_DEBUG << "This debug message is not published on 'stdout'."
//                 << BALL_LOG_END;
//  BALL_LOG_INFO  << "This info will be published on 'stdout'."
//                 << BALL_LOG_END;
//  BALL_LOG_WARN  << "This warning will be published on 'stdout'."
//                 << BALL_LOG_END;
//..
// The user can log all messages to a specified file and specify rotation rules
// based on the size of the log file or its lifetime:
//..
//  fileObserver.enableFileLogging("/var/log/task/task.log");
//      // Create and log records to a file named "/var/log/task/task.log".
//
//  fileObserver.setStdoutThreshold(ball::Severity::e_OFF);
//      // Disable 'stdout' logging.
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

#ifndef INCLUDED_BALL_FILEOBSERVER2
#include <ball_fileobserver2.h>
#endif

#ifndef INCLUDED_BALL_OBSERVER
#include <ball_observer.h>
#endif

#ifndef INCLUDED_BALL_RECORDSTRINGFORMATTER
#include <ball_recordstringformatter.h>
#endif

#ifndef INCLUDED_BALL_SEVERITY
#include <ball_severity.h>
#endif

#ifndef INCLUDED_BSLMT_MUTEX
#include <bslmt_mutex.h>
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

                          // ==================
                          // class FileObserver
                          // ==================

class FileObserver : public Observer {
    // This class implements the 'Observer' protocol.  The 'publish' method of
    // this class outputs the log records that it receives to 'stdout' and
    // optionally to a user-specified file.  This class is thread-safe;
    // different threads can operate on this object concurrently.  This class
    // is exception-neutral with no guarantee of rollback.  In no event is
    // memory leaked.

    // DATA
    RecordStringFormatter d_logFileFormatter;   // record formatter used when
                                                // logging to a file

    RecordStringFormatter d_stdoutFormatter;    // record formatter used when
                                                // logging to 'stdout'

    Severity::Level       d_stdoutThreshold;    // minimum severity for
                                                // published messages

    bool                  d_useRegularFormatOnStdoutFlag;
                                                // 'true' if messages published
                                                // to 'stdout' in regular
                                                // format

    bool                  d_publishInLocalTime; // 'true' if timestamps of
                                                // records are published in
                                                // local time

    bool                  d_userFieldsLoggingFlag;
                                                // 'true' if user-defined
                                                // fields published

    bsl::string           d_stdoutLongFormat;   // long format of records
                                                // printed to 'stdout'

    bsl::string           d_stdoutShortFormat;  // short format records
                                                // printed to 'stdout'

    mutable bslmt::Mutex  d_mutex;              // serialize operations

    FileObserver2         d_fileObserver2;      // forward most operations
                                                // this object

  private:
    // NOT IMPLEMENTED
    FileObserver(const FileObserver&);
    FileObserver& operator=(const FileObserver&);

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(FileObserver, bslma::UsesBslmaAllocator);

    // CREATORS
    explicit FileObserver(
              Severity::Level   stdoutThreshold = Severity::e_WARN,
              bslma::Allocator *basicAllocator  = 0);
        // Create a file observer that publishes log records to 'stdout' if
        // their severity is at least as severe as the optionally specified
        // 'stdoutThreshold' level.  If 'stdoutThreshold' is not specified, log
        // records are published to 'stdout' if their severity is at least as
        // severe as 'Severity::e_WARN'.  The timestamp attribute of
        // published records is written in UTC time by default.  Optionally
        // specify a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.  Note that user-defined fields are published to 'stdout' by
        // default.  Also note that file logging is initially disabled.

    FileObserver(Severity::Level   stdoutThreshold,
                 bool              publishInLocalTime,
                 bslma::Allocator *basicAllocator = 0);
        // Create a file observer that publishes log records to 'stdout' if
        // their severity is at least as severe as the specified
        // 'stdoutThreshold' level.  If the specified 'publishInLocalTime' flag
        // is 'true', the timestamp attribute of published records is written
        // in local time; otherwise the timestamp attribute of published
        // records is written in UTC time.  The offset between the local time
        // and UTC time is computed at construction and remains unchanged
        // during the lifetime of this object.  Note that user-defined fields
        // are published to 'stdout' by default.  Also note that file logging
        // is initially disabled.

    ~FileObserver();
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

    void disableStdoutLoggingPrefix();
        // Disable this file observer from using the default long output format
        // when logging to 'stdout'.  This method has no effect if the default
        // long output format for 'stdout' logging is not enabled.

    void disableUserFieldsLogging();
        // Disable the logging of user-defined fields by this file observer.
        // This method has no effect if logging of user-defined fields is not
        // enabled, or if a format string other than the default one is in
        // effect.
        //
        // DEPRECATED: Use 'setLogFormat' instead.

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

    void enableStdoutLoggingPrefix();
        // Enable this file observer to use the default long output format
        // when logging to 'stdout'.  This method has no effect if the default
        // long output format for 'stdout' logging is already enabled.

    void enableUserFieldsLogging();
        // Enable the logging of user-defined fields by this file observer.
        // This method has no effect if logging of user-defined fields is
        // already enabled, or if a format string other than the default one is
        // in effect.
        //
        // DEPRECATED: Use 'setLogFormat' instead.

    void enablePublishInLocalTime();
        // Enable publishing of the timestamp attribute of records in local
        // time by this file observer.  This method has no effect if publishing
        // in local time is already enabled.

    void publish(const Record& record, const Context& context);
        // Process the specified log 'record' having the specified publishing
        // 'context' by writing 'record' and 'context' to a file if file
        // logging is enabled for this file observer, and to 'stdout' if the
        // severity of 'record' is at least as severe as the severity level
        // specified at construction.

    void publish(const bsl::shared_ptr<const Record>& record,
                 const Context&                       context);
        // Process the record referred by the specified shared pointer 'record'
        // by writing the record and the specified 'context' of the record to
        // a file if file logging is enabled for this file observer, and to
        // 'stdout' if the severity of 'record' is at least as severe as the
        // severity level specified at construction.

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

    void setOnFileRotationCallback(
         const FileObserver2::OnFileRotationCallback& onRotationCallback);
        // Set the specified 'onRotationCallback' to be invoked after each time
        // this file observer attempts perform a log file rotation.  The
        // behavior is undefined if the supplied function calls either
        // 'setOnFileRotationCallback', 'forceRotation', or 'publish' on this
        // file observer (i.e., the supplied callback should *not* attempt to
        // write to the 'ball' log).

    void setStdoutThreshold(Severity::Level stdoutThreshold);
        // Set the minimum severity of messages logged to 'stdout' by this file
        // observer to the specified 'stdoutThreshold' level.  Note that if the
        // value of 'stdoutThreshold' is 'Severity::e_OFF', logging to 'stdout'
        // is disabled.

    void setLogFormat(const char *logFileFormat, const char *stdoutFormat);
        // Set the format of log records written to the log file and to
        // 'stdout' to the specified 'logFileFormat' and 'stdoutFormat',
        // respectively.  If this method is not called, default formats are
        // used when publishing log records.  See "Log Record Formatting" under
        // @DESCRIPTION for details of formatting syntax.

    // ACCESSORS
    bool isFileLoggingEnabled() const;
    bool isFileLoggingEnabled(bsl::string *result) const;
        // Return 'true' if file logging is enabled for this file observer, and
        // 'false' otherwise.  Load the optionally specified 'result' with the
        // name of the current log file if file logging is enabled, and leave
        // 'result' unaffected otherwise.

    bool isStdoutLoggingPrefixEnabled() const;
        // Return 'true' if this file observer uses the default output format
        // when writing to 'stdout', and 'false' otherwise.

    bool isUserFieldsLoggingEnabled() const;
        // Return 'true' if the logging of user-defined fields is enabled for
        // this file observer, and 'false' otherwise.

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
        // [!DEPRECATED!] Use 'bdlt::LocalTimeOffset' instead.
        //
        // Return the difference between the local time and UTC time in effect
        // when this file observer was constructed.  Note that this value
        // remains unchanged during the lifetime of this object and therefore
        // may become incorrect when the difference between the local time and
        // UTC time changes (e.g., when transitioning into or out of daylight
        // savings time).

    Severity::Level stdoutThreshold() const;
        // Return the minimum severity of messages that will be logged to
        // 'stdout' by this file observer.

    void getLogFormat(const char **logFileFormat,
                      const char **stdoutFormat) const;
        // Load the format of log records written by this file observer to the
        // log file into the specified '*logFileFormat' address and the format
        // of log records written to 'stdout' into the specified
        // '*stdoutFormat' address.  See "Log Record Formatting" under
        // @DESCRIPTION for details of formatting syntax.
};

// ============================================================================
//                              INLINE DEFINITIONS
// ============================================================================

                          // ------------------
                          // class FileObserver
                          // ------------------

// MANIPULATORS
inline
void FileObserver::disableFileLogging()
{
    d_fileObserver2.disableFileLogging();
}

inline
void FileObserver::disableLifetimeRotation()
{
    d_fileObserver2.disableLifetimeRotation();
}

inline
void FileObserver::disableTimeIntervalRotation()
{
    d_fileObserver2.disableTimeIntervalRotation();
}

inline
void FileObserver::disableSizeRotation()
{
    d_fileObserver2.disableSizeRotation();
}

inline
int FileObserver::enableFileLogging(const char *logFilenamePattern)
{
    return d_fileObserver2.enableFileLogging(logFilenamePattern);
}

inline
int FileObserver::enableFileLogging(const char *logFilenamePattern,
                                    bool        appendTimestampFlag)
{
    return d_fileObserver2.enableFileLogging(logFilenamePattern,
                                             appendTimestampFlag);
}

inline
void FileObserver::forceRotation()
{
    d_fileObserver2.forceRotation();
}

inline
void FileObserver::publish(const bsl::shared_ptr<const Record>& record,
                           const Context&                       context)
{
    publish(*record, context);
}

inline
void FileObserver::releaseRecords()
{
}

inline
void FileObserver::rotateOnSize(int size)
{
    d_fileObserver2.rotateOnSize(size);
}

inline
void FileObserver::rotateOnLifetime(const bdlt::DatetimeInterval& timeInterval)
{
    d_fileObserver2.rotateOnLifetime(timeInterval);
}

inline
void FileObserver::rotateOnTimeInterval(const bdlt::DatetimeInterval& interval)
{
    d_fileObserver2.rotateOnTimeInterval(interval);
}

inline
void FileObserver::rotateOnTimeInterval(
                              const bdlt::DatetimeInterval& interval,
                              const bdlt::Datetime&         referenceStartTime)
{
    d_fileObserver2.rotateOnTimeInterval(interval, referenceStartTime);
}

inline
void FileObserver::setOnFileRotationCallback(
          const FileObserver2::OnFileRotationCallback& onRotationCallback)
{
    d_fileObserver2.setOnFileRotationCallback(onRotationCallback);
}

// ACCESSORS
inline
bool FileObserver::isFileLoggingEnabled() const
{
    return d_fileObserver2.isFileLoggingEnabled();
}

inline
bool FileObserver::isFileLoggingEnabled(bsl::string *result) const
{
    return d_fileObserver2.isFileLoggingEnabled(result);
}

inline
bdlt::DatetimeInterval FileObserver::rotationLifetime() const
{
    return d_fileObserver2.rotationLifetime();
}

inline
int FileObserver::rotationSize() const
{
    return d_fileObserver2.rotationSize();
}

inline
bdlt::DatetimeInterval FileObserver::localTimeOffset() const
{
    return d_fileObserver2.localTimeOffset();
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
