// bael_asyncfileobserver.h                                           -*-C++-*-
#ifndef INCLUDED_BAEL_ASYNCFILEOBSERVER
#define INCLUDED_BAEL_ASYNCFILEOBSERVER

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide an asynchronous observer that logs to a file and 'stdout'.
//
//@CLASSES:
//  bael_AsyncFileObserver: observer that outputs logs to a file and 'stdout'
//
//@SEE_ALSO: bael_record, bael_context, bael_observer, bael_fileobserver
//
//@AUTHOR: Shijin Kong (skong25)
//
//@DESCRIPTION: This component provides a concrete implementation of the
//'bael_Observer' protocol for asynchronously publishing log records to
//'stdout' and user-specified files.  A 'bael_AsyncFileObserver' object
//processes the log records received through its 'publish' method by pushing
//the provided 'bael_Record' object into a queue and returns immediately.  The
//supplied record is finally published when an independent publication thread
//reads the log record from the queue and writes it to the configured log file
//and 'stdout'.  The async file observer is by default configured to drop
//records when the queue gets full.  The following inheritance hierarchy
//diagram shows the classes involved and their methods:
//..
//             ,----------------------.
//            ( bael_AsyncFileObserver )
//             `----------------------'
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
//                         |              setStdoutThreshold
//                         |              setLogFormat
//                         |              shutdownPublicationThread
//                         |              startPublicationThread
//                         |              stopPublicationThread
//                         |              isFileLoggingEnabled
//                         |              isStdoutLoggingPrefixEnabled
//                         |              isUserFieldsLoggingEnabled
//                         |              rotationLifetime
//                         |              rotationSize
//                         |              localTimeOffset
//                         |              getLogFormat
//                         V
//                  ,-------------.
//                 ( bael_Observer )
//                  `-------------'
//                                        dtor
//                                        publish
//                                        releaseRecords
//..
// The format of published log records is user-configurable (see "Log Record
// Formatting" below).  In addition, an async file-observer may be configured
// to perform automatic log file rotation (see "Log File Rotation" below).
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
//  18MAY2005_18:58:12.000 7959:1 WARN bael_asyncfileobserver.t.cpp:404 TEST
//  hello!
//..
// The default format for records published to 'stdout' can be shortened by
// calling 'disableStdoutLoggingPrefix'.  This method has the effect of
// reducing the above example message to:
//..
//  WARN bael_asyncfileobserver.t.cpp:404 TEST hello!
//..
// For additional flexibility, the 'setLogFormat' method can be called to
// configure the format of published records.  This method takes two arguments:
// one specifies the format of records logged to 'stdout' and the other applies
// to records that are logged to a file.  The respective formats are specified
// using 'printf'-style ('%'-prefixed) conversion specifications.  (See the
// component-level documentation of 'bael_recordstringformatter' for
// information on how format specifications are defined and interpreted.)  For
// example, the following statement will force subsequent records to be logged
// in a format that is almost identical to the default (long) format except
// that the timestamp attribute will be written in ISO 8601 format:
//..
//   asyncFileObserver.setLogFormat("%i %p:%t %s %f:%l %c %m %u",
//                                  "%i %p:%t %s %f:%l %c %m %u");
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
// A 'bael_AsyncFileObserver' may be configured to perform automatic rotation
// of log files based on simple file rotation conditions (or rules).
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
// All public methods of 'bael_AsyncFileObserver' are thread-safe, and can be
// called concurrently by multiple threads.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Publication Through Logger Manager
///- - - - - - - - - - - - - - - - - - - - - - -
// The following code fragments illustrate the essentials of using a file
// observer within a 'bael' logging system.
//
// First we create a 'bael_AsyncFileObserver' named 'asyncFileObserver' that by
// default has a queue of maximum length 8192 records and drops any incoming
// record when queue is full.  Alternatively the async file observer can be
// created via other constructors by specifying customized maximum queue length
// and dropping severity threshold.
//..
//  bael_AsyncFileObserver asyncFileObserver;
//..
// Then, start the publication thread by invoking 'startPublicationThread':
//..
//  asyncFileObserver.startPublicationThread();
//..
// Next, the async file observer must then be installed within a 'bael' logging
// system.  All messages that are published to the logging system will be
// transmitted to the 'publish' method of 'asyncFileObserver'.  Set the log
// category once this is done by passing the async file observer object to the
// 'bael_LoggerManager::initSingleton' method:
//..
//  bael_LoggerManagerConfiguration configuration;
//  bael_LoggerManager::initSingleton(&asyncFileObserver, configuration);
//  BAEL_LOG_SET_CATEGORY("bael_AsyncFileObserverTest");
//..
// Then, the logging format can be optionally changed by calling the
// 'setLogFormat' method.  The statement below outputs timestamps in ISO 8601
// format to a log file and in 'bdet'-style (default) format to 'stdout':
//..
//  asyncFileObserver.setLogFormat("%i %p:%t %s %f:%l %c %m",
//                                 "%d %p:%t %s %f:%l %c %m");
//..
// Next, log one message to log file and log one message to 'stdout' by
// specifying different logging severity.  By default, only the messages with
// 'WARN', 'ERROR' or 'FATAL' severity will be logged to 'stdout'.
//..
//  BAEL_LOG_INFO << "Will not be published on 'stdout'."
//                << BAEL_LOG_END;
//  BAEL_LOG_WARN << "This warning *will* be published on 'stdout'."
//                << BAEL_LOG_END;
//..
// Then, change the default severities for logging to 'stdout' by calling the
// 'setStdoutThreshold' method:
//..
//  asyncFileObserver.setStdoutThreshold(bael_Severity::BAEL_INFO);
//  BAEL_LOG_DEBUG << "This debug message is not published on 'stdout'."
//                 << BAEL_LOG_END;
//  BAEL_LOG_INFO  << "This info will be published on 'stdout'."
//                 << BAEL_LOG_END;
//  BAEL_LOG_WARN  << "This warning will be published on 'stdout'."
//                 << BAEL_LOG_END;
//..
// Next, the user can log all messages to a specified file and specify rotation
// rules based on the size of the log file or its lifetime:
//..
//  asyncFileObserver.enableFileLogging("/var/log/task/task.log");
//      // Create and log records to a file named "/var/log/task/task.log".
//
//  asyncFileObserver.setStdoutThreshold(bael_Severity::BAEL_OFF);
//      // Disable 'stdout' logging.
//
//  asyncFileObserver.rotateOnSize(1024 * 256);
//      // Rotate the file when its size becomes greater than or equal to 256
//      // mega-bytes.
//
//  asyncFileObserver.rotateOnTimeInterval(bdet_DatetimeInterval(1));
//      // Rotate the file every 24 hours.
//..
// Notice that, in this configuration the user may end up with multiple log
// files for a specific day (because of the rotation-on-size rule).
//
// Now, dynamically disable this feature and file logging:
//..
//  asyncFileObserver.disableSizeRotation();
//  asyncFileObserver.disableFileLogging();
//..
// Finally, stop the publication thread by explicitly calling the
// 'stopPublicationThread' method.  The 'stopPublicationThread' is also
// invoked when the async file observer is
// destroyed.
//..
//  asyncFileObserver.stopPublicationThread();
//..
// Notice that, the logger manager in which the async file observer is
// registered must get destroyed before the async file observer does.  On
// destruction the logger manager calls 'releaseRecords' method of the async
// file observer to release any queued log record whose memory is managed by
// the the logger manager.  The 'releaseRecords' method can be used in similar
// situations where the underlying resources referred by queued shared pointers
// need to be released immediately.

#ifndef INCLUDED_BAESCM_VERSION
#include <baescm_version.h>
#endif

#ifndef INCLUDED_BAEL_FILEOBSERVER
#include <bael_fileobserver.h>
#endif

#ifndef INCLUDED_BAEL_FILEOBSERVER2
#include <bael_fileobserver2.h>
#endif

#ifndef INCLUDED_BAEL_OBSERVER
#include <bael_observer.h>
#endif

#ifndef INCLUDED_BAEL_RECORDSTRINGFORMATTER
#include <bael_recordstringformatter.h>
#endif

#ifndef INCLUDED_BAEL_SEVERITY
#include <bael_severity.h>
#endif

#ifndef INCLUDED_BAEL_RECORD
#include <bael_record.h>
#endif

#ifndef INCLUDED_BAEL_CONTEXT
#include <bael_context.h>
#endif

#ifndef INCLUDED_BCEC_FIXEDQUEUE
#include <bcec_fixedqueue.h>
#endif

#ifndef INCLUDED_BDEF_FUNCTION
#include <bdef_function.h>
#endif

#ifndef INCLUDED_BCEMT_THREAD
#include <bcemt_thread.h>
#endif

#ifndef INCLUDED_BCES_ATOMICTYPES
#include <bces_atomictypes.h>
#endif

#ifndef INCLUDED_BDET_DATETIMEINTERVAL
#include <bdet_datetimeinterval.h>
#endif

#ifndef INCLUDED_BSL_STRING
#include <bsl_string.h>
#endif

namespace BloombergLP {

class bslma_Allocator;


                          // ============================
                          // class bael_AsyncFileObserver
                          // ============================

class bael_AsyncFileObserver : public bael_Observer {
    // This class implements the 'bael_Observer' protocol.  The 'publish'
    // method of this class outputs the log records asynchronously to
    // 'stdout' and optionally to a user-specified file.  This class is
    // thread-safe; different threads can operate on this object concurrently.
    // This class is exception-neutral with no guarantee of rollback.  In no
    // event is memory leaked.

    // DATA
    struct AsyncRecord
    {
        bcema_SharedPtr<const bael_Record>  d_record;
        bael_Context d_context;
    };

    bael_FileObserver             d_fileObserver;    // forward most public
                                                     // method calls to this
                                                     // file observer member

    bcemt_ThreadUtil::Handle      d_threadHandle;    // handle of asynchronous
                                                     // publication thread

    bcec_FixedQueue<AsyncRecord>  d_recordQueue;     // queue transmitting
                                                     // records to the
                                                     // publication thread

    bool                          d_clearing;        // flag that indicates
                                                     // queue in clearance

    bael_Severity::Level          d_dropRecordsOnFullQueueThreshold;
                                                     // severity threshold that
                                                     // indicates if the queue
                                                     // drops records when full

    bces_AtomicInt                d_dropCount;       // counter that keeps
                                                     // tracking dropped
                                                     // records

    bdef_Function<void (*)()>     d_publishThreadEntryPoint;
                                                     // functor that contains
                                                     // publication thread
                                                     // function

    mutable bcemt_Mutex           d_mutex;           // serialize operations

    bslma_Allocator              *d_allocator_p;     // memory allocator (held,
                                                     // not owned)

  private:
    // NOT IMPLEMENTED
    bael_AsyncFileObserver(const bael_AsyncFileObserver&);
    bael_AsyncFileObserver& operator=(const bael_AsyncFileObserver&);

    // PRIVATE MANIPULATORS
    void publishThreadEntryPoint();
        // Thread function of the publication thread.  The publication thread
        // pops record shared pointers and contexts from queue and writes the
        // records referred by these shared pointers to files or 'stdout'.

    int startThread();
        // Create publication thread using the thread function
        // 'publishThreadEntryPoint'.  Return 0 on success or if the
        // publication thread has already started, and a non-zero value
        // otherwise.  This method is not thread-safe.  The behavior is
        // undefined if more than two calls of this method occur concurrently.

    int stopThread();
        // Stop publication thread by pushing a record shared pointer referring
        // a special 'bael_Record' object with the cause of its context set to
        // 'BAEL_END'.  This method is not thread-safe.  The behavior is
        // undefined if more than two calls of this method occur concurrently.

  public:
    // CREATORS
    explicit bael_AsyncFileObserver(
              bael_Severity::Level  stdoutThreshold = bael_Severity::BAEL_WARN,
              bslma_Allocator      *basicAllocator  = 0);
        // Create a file observer that asynchronously publishes log records to
        // 'stdout' or a log file if the records severities are at least as
        // severe as the optionally specified 'stdoutThreshold'.  If
        // 'stdoutThreshold' is not specified, log records are published if
        // their severity is at least as severe as 'bael_Severity::BAEL_WARN'.
        // The timestamp attribute of published records is written in GMT time
        // by default.  Records are pushed into a queue of maximum length 8,192
        // and published by an independent publication thread.  Any incoming record
        // is dropped by default when the queue is full.  An alert will be
        // printed to 'stderr' the first time a record is dropped, and then
        // every 5,000 dropped records.  Optionally specify a 'basicAllocator'
        // used to supply memory.  If 'basicAllocator' is 0, the currently
        // installed default allocator is used.  Note that user-defined fields
        // are published to 'stdout' by default.  Also note that file logging
        // is initially disabled.


    bael_AsyncFileObserver(bael_Severity::Level  stdoutThreshold,
                           bool                  publishInLocalTime,
                           bslma_Allocator      *basicAllocator = 0);
        // Create a file observer that asynchronously publishes log records at
        // least as severe as the specified 'stdoutThreshold' to 'stdout' or a
        // log file with the records timestamps published in local time if the
        // specified 'publishInLocalTime' flag is 'true', or otherwise in UTC
        // time.  The offset between the local time and UTC time is computed at
        // construction and remains unchanged during the lifetime of this
        // object.  Records are pushed into a queue of maximum length 8,192 and
        // published by an independent publication thread.  Any incoming record is
        // dropped by default when the queue is full.  An alert will be printed
        // to 'stderr' the first time a record is dropped, and then every 5,000
        // dropped records.  Optionally specify a 'basicAllocator' used to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.  Note that user-defined fields are
        // published to 'stdout' by default.  Also note that file logging is
        // initially disabled.

    bael_AsyncFileObserver(bael_Severity::Level  stdoutThreshold,
                           bool                  publishInLocalTime,
                           int                   maxRecordQueueSize,
                           bslma_Allocator      *basicAllocator = 0);
        // Create a file observer that asynchronously publishes log records at
        // least as severe as the specified 'stdoutThreshold' to 'stdout' with
        // the records timestamps published in local time if the specified
        // 'publishInLocalTime' flag is 'true', or otherwise in UTC time, by
        // passing the records to an independent publication thread through a queue
        // having the specified 'maxRecordQueueSize' as maximum length.  The
        // offset between the local time and UTC time is computed at
        // construction and remains unchanged during the lifetime of this
        // object.  Any incoming record is dropped when the queue is full.  An
        // alert will be printed to 'stderr' the first time a record is
        // dropped, and then every 100 dropped records.  Optionally specify a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.  Note that
        // user-defined fields are published to 'stdout' by default.  Also note
        // that file logging is initially disabled.

    bael_AsyncFileObserver(
                         bael_Severity::Level  stdoutThreshold,
                         bool                  publishInLocalTime,
                         int                   maxRecordQueueSize,
                         bael_Severity::Level  dropRecordsOnFullQueueThreshold,
                         bslma_Allocator      *basicAllocator = 0);
        // Create a file observer that asynchronously publishes log records at
        // least as severe as the specified 'stdoutThreshold' to 'stdout' with
        // the records timestamps published in local time if the specified
        // 'publishInLocalTime' flag is 'true', or otherwise in UTC time, by
        // passing the records to an independent publication thread through a queue
        // having the specified 'maxRecordQueueSize' as maximum length and
        // dropping any incoming record when the queue is full unless the
        // record is at least as severe as the specified
        // 'dropRecordsOnFullQueueThreshold'.  The offset between the local
        // time and UTC time is computed at construction and remains unchanged
        // during the lifetime of this object.  An alert will be printed to
        // 'stderr' the first time a record is dropped, and then every 5,000
        // dropped records.  Optionally specify a 'basicAllocator' used to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.  Note that user-defined fields are
        // published to 'stdout' by default.  Also note that file logging is
        // initially disabled.

    ~bael_AsyncFileObserver();
        // Close the log file of this async file observer if file logging is
        // enabled, and destroy this async file observer.

    // MANIPULATORS
    void disableFileLogging();
        // Disable file logging for this async file observer.  This method has
        // no effect if file logging is not enabled.

    void disableTimeIntervalRotation();
        // Disable log file rotation based on periodic time interval for this
        // file observer.  This method has no effect if
        // rotation-on-time-interval is not enabled.

    void disableSizeRotation();
        // Disable log file rotation based on log file size for this file
        // observer.  This method has no effect if rotation-on-size is not
        // enabled.

    void disableStdoutLoggingPrefix();
        // Disable this async file observer from using the default long output
        // format when logging to 'stdout'.  This method has no effect if the
        // default long output format for 'stdout' logging is not enabled.

    void disablePublishInLocalTime();
        // Disable publishing of the timestamp attribute of records in local
        // time by this async file observer.  This method has no effect if
        // publishing in local time is not enabled.

    int enableFileLogging(const char *logFilenamePattern);
        // Enable logging of all messages published to this async file observer
        // to a file indicated by the specified 'logFilenamePattern'.  Return 0
        // on success, a positive value if file logging is already enabled, and
        // a negative value otherwise.  The basename of 'logFilenamePattern'
        // may contain '%'-escape sequences that are interpreted as follows:
        //..
        //   %Y - current year (four digits with leading zeros)
        //   %M - current month (two digits with leading zeros)
        //   %D - current day (two digits with leading zeros)
        //   %h - current hour (two digits with leading zeros)
        //   %m - current minute (two digits with leading zeros)
        //   %s - current second (two digits with leading zeros)
        //   %T - current datetime, equivalent to "%Y%M%D_%h%m%s"
        //..
        // Each time a log file is opened by this async file observer (upon a
        // successful call to this method and following each log file rotation)
        // the name of the log file is derived from 'logFilenamePattern' by
        // interpreting the above recognized '%'-escape sequences.  On
        // rotation, the current log file is closed, and a new log-file name is
        // derived.  If the newly derived log-file name is the same as that of
        // the current (rotated) log file, the current log file is renamed by
        // appending a timestamp of the form ".%Y%M%D_%h%m%s".  Then, the new
        // log file is opened for logging.

    void enableStdoutLoggingPrefix();
        // Enable this async file observer to use the default long output
        // format when logging to 'stdout'.  This method has no effect if the
        // default long output format for 'stdout' logging is already enabled.

    void enablePublishInLocalTime();
        // Enable publishing of the timestamp attribute of records in local
        // time by this async file observer.  This method has no effect if
        // publishing in local time is already enabled.

    void publish(const bcema_SharedPtr<const bael_Record>& record,
                 const bael_Context&                       context);
        // Process the record referred by the specified shared pointer 'record'
        // by writing the record and the specified 'context' of the record to
        // a file if file logging is enabled for this async file observer, and
        // to 'stdout' if the severity of 'record' is at least as severe as
        // the severity level specified at construction.

    void releaseRecords();
        // Discard the shared references in the fixed queue of this async file
        // observer without publishing the records referred by these shared
        // pointers.  If the publication thread is running, this method stops
        // the publication thread before clearing the queue and restarts the
        // publication thread afterwards.  This method should be called by the
        // owner of the records pointed by the share pointers in the fixed
        // queue when these records are no longer usable.

    void forceRotation();
        // Forcefully perform a log file rotation by this async file observer.
        // Close the current log file, rename the log file if necessary, and
        // open a new log file.  See the "Rotated File Naming" section under
        // @DESCRIPTION in the component-level documentation for details on
        // filenames of the rotated log files.  This method has no effect if
        // file logging is not enabled.

    void rotateOnSize(int size);
        // Set this async file observer to perform log file rotation when the
        // size of the file exceeds the specified 'size' (in kilo-bytes).
        // This rule replaces any rotation-on-size rule currently in effect.
        // The behavior is undefined unless 'size > 0'.

    void rotateOnTimeInterval(const bdet_DatetimeInterval& interval);
    void rotateOnTimeInterval(const bdet_DatetimeInterval& interval,
                              const bdet_Datetime&         referenceStartTime);
        // Set this async file observer to perform a periodic log-file rotation
        // at multiples of the specified 'interval'.  Optionally, specify
        // 'referenceStartTime' indicating the *local* datetime to use as the
        // starting point for computing the periodic rotation schedule.  If
        // 'referenceStartTime' is unspecified, the current time is used.  The
        // behavior is undefined unless '0 < interval.totalMilliseconds()'.
        // This rule replaces any rotation-on-time-interval rule currently in
        // effect.  Note that 'referenceStartTime' may be a fixed time in the
        // past.  E.g., a reference time of 'bdet_Datetime(1, 1, 1)' and an
        // interval of 24 hours would configure a periodic rotation at midnight
        // each day.

    void setOnFileRotationCallback(
         const bael_FileObserver2::OnFileRotationCallback& onRotationCallback);
        // Set the specified 'onRotationCallback' to be invoked after each time
        // this async file observer attempts perform a log file rotation.  The
        // behavior is undefined if the supplied function calls either
        // 'setOnFileRotationCallback', 'forceRotation', or 'publish' on this
        // async file observer.

    void setStdoutThreshold(bael_Severity::Level stdoutThreshold);
        // Set the minimum severity of messages logged to 'stdout' by this file
        // observer to the specified 'stdoutThreshold' level.  Note that if the
        // value of 'stdoutThreshold' is 'bael_Severity::BAEL_OFF', logging to
        // 'stdout' is disabled.

    void setLogFormat(const char *logFileFormat, const char *stdoutFormat);
        // Set the format of log records written to the log file and to
        // 'stdout' to the specified 'logFileFormat' and 'stdoutFormat',
        // respectively.  If this method is not called, default formats are
        // used when publishing log records.  See "Log Record Formatting" under
        // @DESCRIPTION for details of formatting syntax.

    int shutdownPublicationThread();
        // Immediately shutdown the publication thread and return to the
        // caller.  Currently queue'd records will remain in the queue and
        // un-published until either 'startPublicationThread' or
        // 'releaseRecords' is called.

    int startPublicationThread();
        // Start the publication thread of this async file observer.  Return 0
        // on success or if the publication thread has already started, and a
        // non-zero value otherwise.  This method has no effect if the
        // publication thread has already started.

    int stopPublicationThread();
        // Disable queuing of published records and wait until all currently
        // enqueued records are published, then shut down the publication
        // thread and return to the caller.

    // ACCESSORS
    bool isFileLoggingEnabled() const;
    bool isFileLoggingEnabled(bsl::string *result) const;
        // Return 'true' if file logging is enabled for this async file
        // observer, and 'false' otherwise.  Load the optionally-specified
        // 'result' with the name of the current log file if file logging is
        // enabled, and leave 'result' uneffected otherwise.

    bool isStdoutLoggingPrefixEnabled() const;
        // Return 'true' if this async file observer uses the default output
        // format when writing to 'stdout', and 'false' otherwise.

    bool isUserFieldsLoggingEnabled() const;
        // Return 'true' if the logging of user-defined fields is enabled for
        // this async file observer, and 'false' otherwise.

    bool isPublishInLocalTimeEnabled() const;
        // Return 'true' if this async file observer writes the timestamp
        // attribute of records that it publishes in local time, and 'false'
        // otherwise.

    bool isPublicationThreadRunning() const;
        // Return 'true' if the publication thread is running, and 'false'
        // otherwise.

    bdet_DatetimeInterval rotationLifetime() const;
        // Return the lifetime of the log file that will trigger a file
        // rotation by this async file observer if rotation-on-lifetime is in
        // effect, and a 0 time interval otherwise.

    int rotationSize() const;
        // Return the size (in kilo-bytes) of the log file that will trigger a
        // file rotation by this async file observer if rotation-on-size is in
        // effect, and 0 otherwise.

    bdet_DatetimeInterval localTimeOffset() const;
        // Return the difference between the local time and UTC time in effect
        // when this async file observer was constructed.  Note that this value
        // remains unchanged during the lifetime of this object and therefore
        // may become incorrect when the difference between the local time and
        // UTC time changes (e.g., when transitioning into or out of daylight
        // savings time).

    bael_Severity::Level stdoutThreshold() const;
        // Return the minimum severity of messages that will be logged to
        // 'stdout' by this async file observer.

    void getLogFormat(const char **logFileFormat,
                      const char **stdoutFormat) const;
        // Load the format of log records written by this async file observer
        // to the log file into the specified '*logFileFormat' address and the
        // format of log records written to 'stdout' into the specified
        // '*stdoutFormat' address.  See "Log Record Formatting" under
        // @DESCRIPTION for details of formatting syntax.
};

// ============================================================================
//                          INLINE FUNCTION DEFINITIONS
// ============================================================================

                          // ----------------------------
                          // class bael_AsyncFileObserver
                          // ----------------------------

// MANIPULATORS
inline
void bael_AsyncFileObserver::disableFileLogging()
{
    d_fileObserver.disableFileLogging();
}

inline
void bael_AsyncFileObserver::disableTimeIntervalRotation()
{
    d_fileObserver.disableTimeIntervalRotation();
}

inline
void bael_AsyncFileObserver::disableSizeRotation()
{
    d_fileObserver.disableSizeRotation();
}

inline
int bael_AsyncFileObserver::enableFileLogging(const char *logFilenamePattern)
{
    return d_fileObserver.enableFileLogging(logFilenamePattern);
}

inline
void bael_AsyncFileObserver::forceRotation()
{
    d_fileObserver.forceRotation();
}

inline
void bael_AsyncFileObserver::rotateOnSize(int size)
{
    d_fileObserver.rotateOnSize(size);
}

inline
void bael_AsyncFileObserver::rotateOnTimeInterval(
                                         const bdet_DatetimeInterval& interval)
{
    d_fileObserver.rotateOnTimeInterval(interval);
}

inline
void bael_AsyncFileObserver::rotateOnTimeInterval(
                               const bdet_DatetimeInterval& interval,
                               const bdet_Datetime&         referenceStartTime)
{
    d_fileObserver.rotateOnTimeInterval(interval, referenceStartTime);
}

inline
void bael_AsyncFileObserver::setOnFileRotationCallback(
          const bael_FileObserver2::OnFileRotationCallback& onRotationCallback)
{
    d_fileObserver.setOnFileRotationCallback(onRotationCallback);
}

inline
void bael_AsyncFileObserver::disableStdoutLoggingPrefix()
{
    d_fileObserver.disableStdoutLoggingPrefix();
}

inline
void bael_AsyncFileObserver::disablePublishInLocalTime()
{
    d_fileObserver.disablePublishInLocalTime();
}

inline
void bael_AsyncFileObserver::enableStdoutLoggingPrefix()
{
    d_fileObserver.enableStdoutLoggingPrefix();
}

inline
void bael_AsyncFileObserver::enablePublishInLocalTime()
{
    d_fileObserver.enablePublishInLocalTime();
}

inline
void bael_AsyncFileObserver::setStdoutThreshold(
                                          bael_Severity::Level stdoutThreshold)
{
    d_fileObserver.bael_FileObserver::setStdoutThreshold(stdoutThreshold);
}

inline
void bael_AsyncFileObserver::setLogFormat(const char *logFileFormat,
                                          const char *stdoutFormat)
{
    d_fileObserver.setLogFormat(logFileFormat, stdoutFormat);
}

// ACCESSORS
inline
bool bael_AsyncFileObserver::isFileLoggingEnabled() const
{
    return d_fileObserver.isFileLoggingEnabled();
}

inline
bool bael_AsyncFileObserver::isFileLoggingEnabled(bsl::string *result) const
{
    return d_fileObserver.isFileLoggingEnabled(result);
}

inline
bdet_DatetimeInterval bael_AsyncFileObserver::rotationLifetime() const
{
    return d_fileObserver.rotationLifetime();
}

inline
int bael_AsyncFileObserver::rotationSize() const
{
    return d_fileObserver.rotationSize();
}

inline
bdet_DatetimeInterval bael_AsyncFileObserver::localTimeOffset() const
{
    return d_fileObserver.localTimeOffset();
}

inline
bael_Severity::Level bael_AsyncFileObserver::stdoutThreshold() const
{
    return d_fileObserver.stdoutThreshold();
}

inline
bool bael_AsyncFileObserver::isStdoutLoggingPrefixEnabled() const
{
    return d_fileObserver.isStdoutLoggingPrefixEnabled();
}

inline
bool bael_AsyncFileObserver::isUserFieldsLoggingEnabled() const
{
    return d_fileObserver.isUserFieldsLoggingEnabled();
}

inline
bool bael_AsyncFileObserver::isPublishInLocalTimeEnabled() const
{
    return d_fileObserver.isPublishInLocalTimeEnabled();
}

inline
bool bael_AsyncFileObserver::isPublicationThreadRunning() const
{
    return bcemt_ThreadUtil::invalidHandle() != d_threadHandle;
}

inline
void bael_AsyncFileObserver::getLogFormat(const char **logFileFormat,
                                          const char **stdoutFormat) const
{
    d_fileObserver.getLogFormat(logFileFormat, stdoutFormat);
}

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2012
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
