// ball_asyncfileobserver.h                                           -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BALL_ASYNCFILEOBSERVER
#define INCLUDED_BALL_ASYNCFILEOBSERVER

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide an asynchronous observer that logs to a file and 'stdout'.
//
//@CLASSES:
//  ball::AsyncFileObserver: observer that outputs logs to a file and 'stdout'
//
//@SEE_ALSO: ball_record, ball_context, ball_observer, ball_fileobserver
//
//@DESCRIPTION: This component provides a concrete implementation of the
// 'ball::Observer' protocol for asynchronously publishing log records to
// 'stdout' and user-specified files.  A 'ball::AsyncFileObserver' object
// processes the log records received through its 'publish' method by pushing
// the provided 'ball::Record' object into a queue and returns immediately.
// The supplied record is finally published when an independent publication
// thread reads the log record from the queue and writes it to the configured
// log file and 'stdout'.  The async file observer is by default configured to
// drop records when the queue reaches its (configurable) maximum length.  The
// following inheritance hierarchy diagram shows the classes involved and their
// methods:
//..
//             ,----------------------.
//            ( ball::AsyncFileObserver )
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
//                         |              setOnFileRotationCallback
//                         |              setStdoutThreshold
//                         |              setLogFormat
//                         |              shutdownPublicationThread
//                         |              startPublicationThread
//                         |              stopPublicationThread
//                         |              isFileLoggingEnabled
//                         |              isStdoutLoggingPrefixEnabled
//                         |              isUserFieldsLoggingEnabled
//                         |              isPublishInLocalTimeEnabled
//                         |              isPublicationThreadRunning
//                         |              recordQueueLength
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
//  18MAY2005_18:58:12.000 7959:1 WARN ball_asyncfileobserver.t.cpp:404 TEST
//  hello!
//..
// The default format for records published to 'stdout' can be shortened by
// calling 'disableStdoutLoggingPrefix'.  This method has the effect of
// reducing the above example message to:
//..
//  WARN ball_asyncfileobserver.t.cpp:404 TEST hello!
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
// A 'ball::AsyncFileObserver' may be configured to perform automatic rotation
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
///Thread Safety
///-------------
// All public methods of 'ball::AsyncFileObserver' are thread-safe, and can be
// called concurrently by multiple threads.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Publication Through Logger Manager
///- - - - - - - - - - - - - - - - - - - - - - -
// The following code fragments illustrate the essentials of using a file
// observer within a 'ball' logging system.
//
// First we create a 'ball::AsyncFileObserver' named 'asyncFileObserver' that
// by default has a queue of records whose maximum length is 8,192, and which
// will drop any incoming record when that queue is full.  Note that the
// 'ball::AsyncFileObserver' constructor accepts optional arguments specifying
// both the maximum length of the record queue and the behavior when the queue
// is full.

//..
//  ball::AsyncFileObserver asyncFileObserver;
//..
// Then, start the publication thread by invoking 'startPublicationThread':
//..
//  asyncFileObserver.startPublicationThread();
//..
// Next, the async file observer must then be installed within a 'ball' logging
// system.  All messages that are published to the logging system will be
// transmitted to the 'publish' method of 'asyncFileObserver'.  Set the log
// category once this is done by passing the async file observer object to the
// 'ball::LoggerManager::initSingleton' method:
//..
//  ball::LoggerManagerConfiguration configuration;
//  ball::LoggerManager::initSingleton(&asyncFileObserver, configuration);
//  BALL_LOG_SET_CATEGORY("ball::AsyncFileObserverTest");
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
//  BALL_LOG_INFO << "Will not be published on 'stdout'."
//                << BALL_LOG_END;
//  BALL_LOG_WARN << "This warning *will* be published on 'stdout'."
//                << BALL_LOG_END;
//..
// Then, change the default severity for logging to 'stdout' by calling the
// 'setStdoutThreshold' method:
//..
//  asyncFileObserver.setStdoutThreshold(ball::Severity::e_INFO);
//  BALL_LOG_DEBUG << "This debug message is not published on 'stdout'."
//                 << BALL_LOG_END;
//  BALL_LOG_INFO  << "This info will be published on 'stdout'."
//                 << BALL_LOG_END;
//  BALL_LOG_WARN  << "This warning will be published on 'stdout'."
//                 << BALL_LOG_END;
//..
// Next, enable logging to a file and then specify log file rotation rules
// based on the size of the log file or its lifetime:
//..
//  asyncFileObserver.enableFileLogging("/var/log/task/task.log");
//      // Create and log records to a file named "/var/log/task/task.log".
//
//  asyncFileObserver.setStdoutThreshold(ball::Severity::e_OFF);
//      // Disable 'stdout' logging.
//
//  asyncFileObserver.rotateOnSize(1024 * 256);
//      // Rotate the file when its size becomes greater than or equal to 256
//      // mega-bytes.
//
//  asyncFileObserver.rotateOnTimeInterval(bdlt::DatetimeInterval(1));
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
// 'stopPublicationThread' method.  The 'stopPublicationThread' is also invoked
// when the async file observer is destroyed.
//..
//  asyncFileObserver.stopPublicationThread();
//..
// Notice that, the logger manager in which the async file observer is
// registered must be destroyed before that async file observer does.  On
// destruction the logger manager calls 'releaseRecords' method of the async
// file observer to release any queued log record whose memory may be managed
// by the the logger manager.  The 'releaseRecords' method can be used in
// similar situations where the underlying resources referred to by queued
// shared pointers need to be released immediately.

#ifndef INCLUDED_BALSCM_VERSION
#include <balscm_version.h>
#endif

#ifndef INCLUDED_BALL_CONTEXT
#include <ball_context.h>
#endif

#ifndef INCLUDED_BALL_FILEOBSERVER
#include <ball_fileobserver.h>
#endif

#ifndef INCLUDED_BALL_FILEOBSERVER2
#include <ball_fileobserver2.h>
#endif

#ifndef INCLUDED_BALL_OBSERVER
#include <ball_observer.h>
#endif

#ifndef INCLUDED_BALL_RECORD
#include <ball_record.h>
#endif

#ifndef INCLUDED_BALL_RECORDSTRINGFORMATTER
#include <ball_recordstringformatter.h>
#endif

#ifndef INCLUDED_BALL_SEVERITY
#include <ball_severity.h>
#endif

#ifndef INCLUDED_BDLCC_FIXEDQUEUE
#include <bdlcc_fixedqueue.h>
#endif

#ifndef INCLUDED_BSLMT_THREADUTIL
#include <bslmt_threadutil.h>
#endif

#ifndef INCLUDED_BDLT_DATETIMEINTERVAL
#include <bdlt_datetimeinterval.h>
#endif

#ifndef INCLUDED_BSLMF_NESTEDTRAITDECLARATION
#include <bslmf_nestedtraitdeclaration.h>
#endif

#ifndef INCLUDED_BSLMA_ALLOCATOR
#include <bslma_allocator.h>
#endif

#ifndef INCLUDED_BSLS_ATOMIC
#include <bsls_atomic.h>
#endif

#ifndef INCLUDED_BSL_FUNCTIONAL
#include <bsl_functional.h>
#endif

#ifndef INCLUDED_BSL_MEMORY
#include <bsl_memory.h>
#endif

#ifndef INCLUDED_BSL_STRING
#include <bsl_string.h>
#endif

namespace BloombergLP {

namespace ball {
                          // =======================
                          // class AsyncFileObserver
                          // =======================

class AsyncFileObserver : public Observer {
    // This class implements the 'Observer' protocol.  The 'publish' method of
    // this class outputs the log records asynchronously to 'stdout' and
    // optionally to a user-specified file.  This class is thread-safe;
    // different threads can operate on this object concurrently.  This class
    // is exception-neutral with no guarantee of rollback.  In no event is
    // memory leaked.

    // DATA
    struct AsyncRecord
    {
        bsl::shared_ptr<const Record> d_record;
        Context                       d_context;
    };

    FileObserver                   d_fileObserver;   // forward most public
                                                     // method calls to this
                                                     // file observer member

    bslmt::ThreadUtil::Handle      d_threadHandle;   // handle of asynchronous
                                                     // publication thread

    bdlcc::FixedQueue<AsyncRecord> d_recordQueue;    // queue transmitting
                                                     // records to the
                                                     // publication thread

    bsls::AtomicInt                d_shuttingDownFlag;
                                                     // flag that indicates
                                                     // the publication thread
                                                     // is being shutdown

    Severity::Level                d_dropRecordsOnFullQueueThreshold;
                                                     // severity threshold that
                                                     // indicates if the queue
                                                     // drops records when full

    bsls::AtomicInt                d_dropCount;      // number of dropped
                                                     // records, reset when
                                                     // published

    bsl::function<void()>          d_publishThreadEntryPoint;
                                                     // functor that contains
                                                     // publication thread
                                                     // function

    Record                         d_droppedRecordWarning;
                                                     // cached record object
                                                     // used to publish the
                                                     // count of dropped log
                                                     // records

    mutable bslmt::Mutex           d_mutex;          // serialize operations

    bslma::Allocator              *d_allocator_p;    // memory allocator (held,
                                                     // not owned)

  private:
    // NOT IMPLEMENTED
    AsyncFileObserver(const AsyncFileObserver&);
    AsyncFileObserver& operator=(const AsyncFileObserver&);

    // PRIVATE MANIPULATORS
    void construct();
        // Initialize members of this object that do not vary between
        // constructor overloads.  Note that this method should be removed when
        // C++11 constructor chaining is available.

    void logDroppedMessageWarning(int numDropped);
        // Synchronously write an entry into the underlying file observer
        // indicating that the specified 'numDropped' number of records have
        // been dropped.  The behavior is undefined if this method is invoked
        // concurrently from multiple threads (i.e., it is *not* *threadsafe*).

    void publishThreadEntryPoint();
        // Thread function of the publication thread.  The publication thread
        // pops record shared pointers and contexts from queue and writes the
        // records referred by these shared pointers to files or 'stdout'.  The
        // behavior is undefined if this method is invoked concurrently from
        // multiple threads (i.e., it is *not* *threadsafe*).  Publish records
        // from the record queue until signaled to stop.  This is the entry
        // point function for the publication thread.

    int shutdownThread();
        // Stop the publication thread without waiting for remaining log
        // records in the record queue to be published.  Discard currently
        // queued log records.  Return 0 on success, and a non-zero value if
        // there is an error joining the publication thread.  The behavior is
        // undefined unless the calling thread holds a lock on 'd_mutex'.

    int startThread();
        // Create a publication thread to publish records from the record
        // queue.  If a publication thread is already active, this operation
        // has no effect.  Return 0 on success, and a non-zero value if there
        // is an error creating the publication thread.  The behavior is
        // undefined unless the calling thread holds a lock on 'd_mutex'.

    int stopThread();
        // Stop the publication thread after all the remaining records in the
        // record queue have been published.  If there is no publication thread
        // this operation has no effect.  Return 0 on success, and a non-zero
        // value if there is an error joining the publication thread.  The
        // behavior is undefined unless the calling thread holds a lock on
        // 'd_mutex'.

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(AsyncFileObserver,
                                   bslma::UsesBslmaAllocator);

    // CREATORS
    explicit AsyncFileObserver(
              Severity::Level   stdoutThreshold = Severity::e_WARN,
              bslma::Allocator *basicAllocator  = 0);
        // Create a file observer that asynchronously publishes log records
        // both to a log file, and possibly also to 'stdout' if a record's
        // severity us at least as severe as the optionally specified
        // 'stdoutThreshold'.  If 'stdoutThreshold' is not specified, log
        // records are published to 'stdout' if their severity is at least as
        // severe as 'Severity::e_WARN'.  Optionally specify a 'basicAllocator'
        // used to supply memory.  If 'basicAllocator' is 0, the currently
        // installed default allocator is used.  The timestamp attribute of
        // published records is written in UTC time.  Published records are
        // added onto the end of a queue having a maximum length of 8,192
        // records, and then later published by an independent publication
        // thread.  If 'publish' is called when the record queue is full, the
        // published record is discarded (and a warning may be written to
        // 'stderr').  Note that user-defined fields are published to 'stdout'
        // by default.  Also note that file logging is initially disabled.

    AsyncFileObserver(Severity::Level   stdoutThreshold,
                      bool              publishInLocalTime,
                      bslma::Allocator *basicAllocator = 0);
        // Create a file observer that asynchronously publishes log records
        // both to a log file, and possibly also to 'stdout', if a record's
        // severity is at least as severe as the specified 'stdoutThreshold',
        // where the timestamp attribute of each published record is written in
        // local-time if the specified 'publishInLocalTimeFlag' is 'true', and
        // UTC time otherwise.  Optionally specify a 'basicAllocator' used to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.  Published records are added onto the end
        // of a queue having a maximum length of 8,192 records, and then later
        // published by an independent publication thread.  If 'publish' is
        // called when the record queue is full, the published record is
        // discarded (and a warning may be written to 'stderr').  Note that
        // user-defined fields are published to 'stdout' by default.  Also note
        // that file logging is initially disabled.

    AsyncFileObserver(Severity::Level   stdoutThreshold,
                      bool              publishInLocalTime,
                      int               maxRecordQueueSize,
                      bslma::Allocator *basicAllocator = 0);
    AsyncFileObserver(Severity::Level   stdoutThreshold,
                      bool              publishInLocalTime,
                      int               maxRecordQueueSize,
                      Severity::Level   dropRecordsOnFullQueueThreshold,
                      bslma::Allocator *basicAllocator = 0);
        // Create a file observer that asynchronously publishes log records by
        // enqueuing them onto a record queue having the specified
        // 'maxRecordQueue' length, where an independent publication thread
        // will later write them both to a log file and possibly also to
        // 'stdout', if the records's severities are at least as severe as the
        // specified 'stdoutThreshold', and where the timestamp attribute of
        // each published record is written in local-time if
        // 'publishInLocalTimeFlag' is 'true', and UTC otherwise.  Optionally
        // specify 'dropRecordsOnFullQueueThreshold' indicating the severity
        // threshold below which records published when the queue is full will
        // be discarded (records published above this severity will block the
        // calling thread if the queue is full, until space is available in the
        // queue).  If 'dropRecordsOnFullQueueThreshold' is not specified, all
        // records published when the queue is full will be discarded.
        // Optionally specify a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.  Note that user-defined fields are published to 'stdout' by
        // default.  Also note that file logging is initially disabled.

    ~AsyncFileObserver();
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
        //   %p - process Id
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

    using Observer::publish;  // Avoid hiding base class.

    void publish(const bsl::shared_ptr<const Record>& record,
                 const Context&                       context);
        // Process the record referred by the specified shared pointer 'record'
        // by writing the record and the specified 'context' of the record to
        // a file if file logging is enabled for this async file observer, and
        // to 'stdout' if the severity of 'record' is at least as severe as
        // the severity level specified at construction.

    void releaseRecords();
        // Discard any shared references to records that have been published
        // but have not yet been written to the log file or 'stdout'.
        // Discarded records will not be published.  This method should be
        // called to immediately release the resources managed by the shared
        // pointers supplied to 'publish'.

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

    void rotateOnTimeInterval(const bdlt::DatetimeInterval& interval);
    void rotateOnTimeInterval(
                             const bdlt::DatetimeInterval& interval,
                             const bdlt::Datetime&         referenceStartTime);
        // Set this async file observer to perform a periodic log-file rotation
        // at multiples of the specified 'interval'.  Optionally, specify
        // 'referenceStartTime' indicating the *local* datetime to use as the
        // starting point for computing the periodic rotation schedule.
        // 'referenceStartTime' is interpreted using the local-time offset at
        // the time this object was constructred.  If 'referenceStartTime' is
        // unspecified, the current time is used.  The behavior is undefined
        // unless '0 < interval.totalMilliseconds()'.  This rule replaces any
        // rotation-on-time-interval rule currently in effect.  Note that
        // 'referenceStartTime' may be a fixed time in the past.  E.g., a
        // reference time of 'bdlt::Datetime(1, 1, 1)' and an interval of 24
        // hours would configure a periodic rotation at midnight each day.

    void setOnFileRotationCallback(
              const FileObserver2::OnFileRotationCallback& onRotationCallback);
        // Set the specified 'onRotationCallback' to be invoked after each time
        // this async file observer attempts perform a log file rotation.  The
        // behavior is undefined if the supplied function calls either
        // 'setOnFileRotationCallback', 'forceRotation', or 'publish' on this
        // async file observer (i.e., the supplied callback should *not*
        // attempt to write to the 'ball' log).

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

    int shutdownPublicationThread();
        // Stop the publication thread without waiting for remaining log
        // records in the record queue to be published.  Discard currently
        // queued log records.  Return 0 on success, and a non-zero value if
        // there is an error joining the publication thread.

    int startPublicationThread();
        // Start a publication thread to asynchronously publish logged records
        // from the record queue.  If a publication thread is already active,
        // this operation has no effect.  Return 0 on success, and a non-zero
        // value if there is an error creating the publication thread.

    int stopPublicationThread();
        // Stop the publication thread after all the remaining records in the
        // record queue have been published.  If there is no publication thread
        // this operation has no effect.  Return 0 on success, and a non-zero
        // value if there is an error joining the publication thread.


    // ACCESSORS
    bool isFileLoggingEnabled() const;
    bool isFileLoggingEnabled(bsl::string *result) const;
        // Return 'true' if file logging is enabled for this async file
        // observer, and 'false' otherwise.  Load the optionally specified
        // 'result' with the name of the current log file if file logging is
        // enabled, and leave 'result' unmodified otherwise.

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

    int recordQueueLength() const;
        // Return the number of log records currently in this observer's log
        // record queue.

    bdlt::DatetimeInterval rotationLifetime() const;
        // Return the lifetime of the log file that will trigger a file
        // rotation by this async file observer if rotation-on-lifetime is in
        // effect, and a 0 time interval otherwise.

    int rotationSize() const;
        // Return the size (in kilo-bytes) of the log file that will trigger a
        // file rotation by this async file observer if rotation-on-size is in
        // effect, and 0 otherwise.



    Severity::Level stdoutThreshold() const;
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
//                              INLINE DEFINITIONS
// ============================================================================

                          // -----------------------
                          // class AsyncFileObserver
                          // -----------------------

// MANIPULATORS
inline
void AsyncFileObserver::disableFileLogging()
{
    d_fileObserver.disableFileLogging();
}

inline
void AsyncFileObserver::disableTimeIntervalRotation()
{
    d_fileObserver.disableTimeIntervalRotation();
}

inline
void AsyncFileObserver::disableSizeRotation()
{
    d_fileObserver.disableSizeRotation();
}

inline
int AsyncFileObserver::enableFileLogging(const char *logFilenamePattern)
{
    return d_fileObserver.enableFileLogging(logFilenamePattern);
}

inline
void AsyncFileObserver::forceRotation()
{
    d_fileObserver.forceRotation();
}

inline
void AsyncFileObserver::rotateOnSize(int size)
{
    d_fileObserver.rotateOnSize(size);
}

inline
void AsyncFileObserver::rotateOnTimeInterval(
                                        const bdlt::DatetimeInterval& interval)
{
    d_fileObserver.rotateOnTimeInterval(interval);
}

inline
void AsyncFileObserver::rotateOnTimeInterval(
                              const bdlt::DatetimeInterval& interval,
                              const bdlt::Datetime&         referenceStartTime)
{
    d_fileObserver.rotateOnTimeInterval(interval, referenceStartTime);
}

inline
void AsyncFileObserver::setOnFileRotationCallback(
          const FileObserver2::OnFileRotationCallback& onRotationCallback)
{
    d_fileObserver.setOnFileRotationCallback(onRotationCallback);
}

inline
void AsyncFileObserver::disableStdoutLoggingPrefix()
{
    d_fileObserver.disableStdoutLoggingPrefix();
}

inline
void AsyncFileObserver::disablePublishInLocalTime()
{
    d_fileObserver.disablePublishInLocalTime();
}

inline
void AsyncFileObserver::enableStdoutLoggingPrefix()
{
    d_fileObserver.enableStdoutLoggingPrefix();
}

inline
void AsyncFileObserver::enablePublishInLocalTime()
{
    d_fileObserver.enablePublishInLocalTime();
}

inline
void AsyncFileObserver::setStdoutThreshold(
                                          Severity::Level stdoutThreshold)
{
    d_fileObserver.FileObserver::setStdoutThreshold(stdoutThreshold);
}

inline
void AsyncFileObserver::setLogFormat(const char *logFileFormat,
                                     const char *stdoutFormat)
{
    d_fileObserver.setLogFormat(logFileFormat, stdoutFormat);
}

// ACCESSORS
inline
bool AsyncFileObserver::isFileLoggingEnabled() const
{
    return d_fileObserver.isFileLoggingEnabled();
}

inline
bool AsyncFileObserver::isFileLoggingEnabled(bsl::string *result) const
{
    return d_fileObserver.isFileLoggingEnabled(result);
}

inline
bdlt::DatetimeInterval AsyncFileObserver::rotationLifetime() const
{
    return d_fileObserver.rotationLifetime();
}

inline
int AsyncFileObserver::recordQueueLength() const
{
    return d_recordQueue.length();
}


inline
int AsyncFileObserver::rotationSize() const
{
    return d_fileObserver.rotationSize();
}


inline
Severity::Level AsyncFileObserver::stdoutThreshold() const
{
    return d_fileObserver.stdoutThreshold();
}

inline
bool AsyncFileObserver::isStdoutLoggingPrefixEnabled() const
{
    return d_fileObserver.isStdoutLoggingPrefixEnabled();
}

inline
bool AsyncFileObserver::isUserFieldsLoggingEnabled() const
{
    return d_fileObserver.isUserFieldsLoggingEnabled();
}

inline
bool AsyncFileObserver::isPublishInLocalTimeEnabled() const
{
    return d_fileObserver.isPublishInLocalTimeEnabled();
}

inline
bool AsyncFileObserver::isPublicationThreadRunning() const
{
    return bslmt::ThreadUtil::invalidHandle() != d_threadHandle;
}

inline
void AsyncFileObserver::getLogFormat(const char **logFileFormat,
                                     const char **stdoutFormat) const
{
    d_fileObserver.getLogFormat(logFileFormat, stdoutFormat);
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
