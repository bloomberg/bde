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
//@AUTHOR: Shijin Kong (skong25)
//
//@DESCRIPTION: This component provides a concrete implementation of the
// 'ball::Observer' protocol, 'ball::AsyncFileObserver', for *asynchronously*
// publishing log records to 'stdout' and, optionally, to a user-specified
// file.  A 'ball::AsyncFileObserver' (informally, "async file observer")
// object processes the log records received through its 'publish' method by
// pushing the supplied 'ball::Record' object onto a queue and (typically)
// returning immediately (see {Log Record Queue}).  Each enqueued record is
// ultimately published when an independent publication thread removes the log
// record from the queue and writes it to the configured log file and 'stdout'.
// The following inheritance hierarchy diagram shows the classes involved and
// their methods:
//..
//             ,-----------------------.
//            ( ball::AsyncFileObserver )
//             `-----------------------'
//                         |              ctor
//                         |              disableFileLogging
//                         |              disablePublishInLocalTime
//                         |              disableSizeRotation
//                         |              disableStdoutLoggingPrefix
//                         |              disableTimeIntervalRotation
//                         |              enableFileLogging
//                         |              enableStdoutLoggingPrefix
//                         |              enablePublishInLocalTime
//                         |              forceRotation
//                         |              rotateOnSize
//                         |              rotateOnTimeInterval
//                         |              setLogFormat
//                         |              setOnFileRotationCallback
//                         |              setStdoutThreshold
//                         |              shutdownPublicationThread
//                         |              startPublicationThread
//                         |              stopPublicationThread
//                         |              getLogFormat
//                         |              isFileLoggingEnabled
//                         |              isPublicationThreadRunning
//                         |              isPublishInLocalTimeEnabled
//                         |              isStdoutLoggingPrefixEnabled
//                         |              recordQueueLength
//                         |              rotationLifetime
//                         |              rotationSize
//                         |              stdoutThreshold
//                         V
//                  ,--------------.
//                 ( ball::Observer )
//                  `--------------'
//                                        dtor
//                                        publish
//                                        releaseRecords
//..
// The format of published log records is user-configurable for both logging to
// 'stdout' and logging to a file (see {Log Record Formatting} below).
// Although logging to a file is initially disabled following construction, the
// most common use-case for 'ball::AsyncFileObserver' is to also log to a file,
// enabled by calling the 'enableFileLogging' method.  In addition, an async
// file observer may be configured to perform automatic log file rotation (see
// {Log File Rotation} below).
//
///Async File Observer Configuration Synopsis
///------------------------------------------
// 'ball::AsyncFileObserver' offers several constructor arguments and
// manipulators that may be used to configure various aspects of an async file
// observer object.  These are summarized in the following tables along with
// the accessors that can be used to query the current state of the
// configuration.  Further details are provided in the following sections and
// the function-level documentation.
//..
// +-----------------------+---------------------------------+
// | Aspect                | Constructor Arguments           |
// +=======================+=================================+
// | Log Record Timestamps | publishInLocalTime              |
// +-----------------------+---------------------------------+
// | 'stdout' Logging      | stdoutThreshold                 |
// +-----------------------+---------------------------------+
// | Log Record Queue      | maxRecordQueueSize              |
// |                       | dropRecordsOnFullQueueThreshold |
// +-----------------------+---------------------------------+
//
// +-------------+-----------------------------+------------------------------+
// | Aspect      | Manipulators                | Accessors                    |
// +=============+=============================+==============================+
// | Log Record  | setLogFormat                | getLogFormat                 |
// | Formatting  | enableStdoutLoggingPrefix   | isStdoutLoggingPrefixEnabled |
// |             | disableStdoutLoggingPrefix  |                              |
// +-------------+-----------------------------+------------------------------+
// | Log Record  | enablePublishInLocalTime    | isPublishInLocalTimeEnabled  |
// | Timestamps  | disablePublishInLocalTime   |                              |
// +-------------+-----------------------------+------------------------------+
// | File        | enableFileLogging           | isFileLoggingEnabled         |
// | Logging     | disableFileLogging          |                              |
// +-------------+-----------------------------+------------------------------+
// | 'stdout'    | setStdoutThreshold          | stdoutThreshold              |
// | Logging     | enableStdoutLoggingPrefix   | isStdoutLoggingPrefixEnabled |
// |             | disableStdoutLoggingPrefix  |                              |
// +-------------+-----------------------------+------------------------------+
// | Log File    | rotateOnSize                | rotationSize                 |
// | Rotation    | rotateOnTimeInterval        | rotationLifetime             |
// |             | disableSizeRotation         |                              |
// |             | disableTimeIntervalRotation |                              |
// |             | setOnFileRotationCallback   |                              |
// +-------------+-----------------------------+------------------------------+
// | Publication | startPublicationThread      | isPublicationThreadRunning   |
// | Thread      | stopPublicationThread       |                              |
// | Management  | shutdownPublicationThread   |                              |
// +-------------+-----------------------------+------------------------------+
//..
// In general, a 'ball::AsyncFileObserver' object can be dynamically configured
// throughout its lifetime (in particular, before or after being registered
// with a logger manager).  Note that for 'ball::AsyncFileObserver',
// configuration changes that affect how records are logged (e.g.,
// 'enablePublishInLocalTime' and 'disablePublishInLocalTime') impact both
// those records received by the 'publish' method subsequent to making the
// configuration change as well as those records that are already on the queue.
//
///Log Record Queue
///----------------
// The log record queue of an async file observer has a configurable, but
// fixed, maximum size, where the default is 8192.  The 'maxRecordQueueSize'
// constructor argument may be used to specify an alternative maximum size for
// the record queue that can be either larger or smaller than the default.
//
// By default, an async file observer is configured for the 'publish' method to
// drop *all* records that it receives while the queue is full.  This behavior
// can be modified by supplying a 'Severity::Level' for the constructor
// argument 'dropRecordsOnFullQueueThreshold' whereby only those records whose
// severity is less severe than that threshold are dropped.  Each record whose
// severity is at least as severe as 'dropRecordsOnFullQueueThreshold' will
// cause the 'publish' method to *block* until the queue has spare capacity to
// accommodate the record (presumably freed up by the publication thread having
// consumed a record from the queue).  For example, if this threshold is set to
// 'e_WARN' on construction, 'e_INFO', 'e_DEBUG', and 'e_TRACE' records are
// dropped by 'publish' when the queue is full whereas 'e_WARN', 'e_ERROR', and
// 'e_FATAL' records will cause 'publish' to block while waiting for room in
// the queue.  Note that while 'publish' will never block with the default
// threshold setting, clients should consider the trade-offs between dropping
// potentially critical log messages versus possibly blocking in 'publish' on
// such messages.
//
// An async file observer maintains a count of the records that are dropped,
// periodically publishing a warning (i.e., an internally generated log record
// with severity 'e_WARN') that reports the number of dropped records.  The
// record count is reset to 0 after each such warning is published, so each
// dropped record is counted only once.
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
//  18MAY2005_18:58:12.076 7959:1 WARN ball_asyncfileobserver.t.cpp:404 TEST hi
//..
// The default format for records published to 'stdout' (only) can be shortened
// by calling 'disableStdoutLoggingPrefix'.  This method has the effect of
// reducing the above example message to the following when output to 'stdout':
//..
//  WARN ball_asyncfileobserver.t.cpp:404 TEST hi
//..
// For additional flexibility, the 'setLogFormat' method can be called to
// configure the format of published records.  This method takes two arguments:
// one specifies the format of records logged to 'stdout' and the other applies
// to records that are logged to a file.  The respective formats are specified
// using 'printf'-style ('%'-prefixed) conversion specifications.  (See
// {'ball_recordstringformatter'} for information on how format specifications
// are defined and interpreted.)  For example, the following statement will
// force subsequent records to be logged in a format that is almost identical
// to the default long format except that the timestamp attribute will be
// written in ISO 8601 format:
//..
//  asyncFileObserver.setLogFormat("%I %p:%t %s %f:%l %c %m %u",
//                                 "%I %p:%t %s %f:%l %c %m %u");
//..
// Once a customized format is specified for 'stdout', calling
// 'disableStdoutLoggingPrefix' will switch to the default short format, i.e.,
// "%s %f:%l %c %m %u".  If 'enableStdoutLoggingPrefix' is subsequently called,
// the customized format specified in the most recent call to 'setLogFormat'
// will be reinstated.
//
// Note that in the sample long-form message above the timestamp has
// millisecond precision ('18MAY2005_18:58:12.076').  If microsecond precision
// is desired instead, consider using either the '%D' or '%O' format
// specification supported by 'ball_recordstringformatter'.
//
///Log Record Timestamps
///---------------------
// By default, the timestamp attributes of published records are written in UTC
// time (unless 'true' is supplied for the optional 'publishInLocalTime'
// constructor flag).  To write timestamps in local time instead, call the
// 'enablePublishInLocalTime' method.  Note that the local time offset is
// calculated using the UTC timestamp of each record.  To revert to UTC time,
// call the 'disablePublishInLocalTime' method.  Whether UTC time or local time
// is in effect can be queried via 'isPublishInLocalTimeEnabled'.
//
// Note that log record timestamps that are output (to either 'stdout' or to a
// file) reflect the times at which the records were received by the 'publish'
// method of the async file observer (and pushed onto the queue).  That is, the
// length of time that a record resides on the queue does not impact the
// record's timestamp when it is eventually output by the publication thread.
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
///Log Filename Pattern
///--------------------
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
// A 'ball::AsyncFileObserver' may be configured to perform automatic rotation
// of log files based on simple file rotation conditions (or rules).
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
// All public methods of 'ball::AsyncFileObserver' are thread-safe, and can be
// called concurrently by multiple threads.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Publication Through the Logger Manager
///- - - - - - - - - - - - - - - - - - - - - - - - -
// This example demonstrates using a 'ball::AsyncFileObserver' within the
// 'ball' logging system.
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
// Then, we create a shared pointer to a 'ball::AsyncFileObserver' object,
// 'observerPtr', having default attributes.  Note that a default-constructed
// async file observer has a maximum (fixed) size of 8192 for its log record
// queue and will drop incoming log records when that queue is full.  (See
// {Log Record Queue} for further information.)
//..
//  bslma::Allocator *alloc =  bslma::Default::globalAllocator(0);
//  bsl::shared_ptr<ball::AsyncFileObserver> observerPtr(
//                                       new(*alloc) ball::AsyncFileObserver(),
//                                       alloc);
//..
// Next, we set the required logging format by calling the 'setLogFormat'
// method.  The statement below outputs timestamps in ISO 8601 format to a log
// file and in 'bdlt'-style (default) format to 'stdout', where timestamps are
// output with millisecond precision in both cases:
//..
//  observerPtr->setLogFormat("%I %p:%t %s %f:%l %c %m",
//                            "%d %p:%t %s %f:%l %c %m");
//..
// Note that both of the above format specifications omit user fields ('%u') in
// the output.
//
// Next, we start the publication thread by invoking 'startPublicationThread':
//..
//  observerPtr->startPublicationThread();
//..
// Then, we register the async file observer with the logger manager.  Upon
// successful registration, the observer will start to receive log records via
// the 'publish' method:
//..
//  int rc = manager.registerObserver(observerPtr, "asyncObserver");
//  assert(0 == rc);
//..
// Next, we set the log category and log a few records with different logging
// severity.  By default, only the records with 'e_WARN', 'e_ERROR', or
// 'e_FATAL' severity will be logged to 'stdout'.  Note that logging to a file
// is not enabled by default:
//..
//  BALL_LOG_SET_CATEGORY("ball::AsyncFileObserverTest");
//
//  BALL_LOG_INFO << "Will not be published on 'stdout'.";
//  BALL_LOG_WARN << "This warning *will* be published on 'stdout'.";
//..
// Then, we change the default severity for logging to 'stdout' by calling the
// 'setStdoutThreshold' method:
//..
//  observerPtr->setStdoutThreshold(ball::Severity::e_INFO);
//
//  BALL_LOG_DEBUG << "This debug message is not published on 'stdout'.";
//  BALL_LOG_INFO  << "This info will be published on 'stdout'.";
//  BALL_LOG_WARN  << "This warning will be published on 'stdout'.";
//..
// Next, we disable logging to 'stdout' and enable logging to a file:
//..
//  observerPtr->setStdoutThreshold(ball::Severity::e_OFF);
//
//  observerPtr->enableFileLogging("/var/log/task/task.log");
//      // Create and log records to a file named "/var/log/task/task.log".
//..
// Note that logs are now asynchronously written to the file.
//
// Then, we specify rules for log file rotation based on the size and time
// interval:
//..
//  observerPtr->rotateOnSize(1024 * 32);
//      // Rotate the file when its size becomes greater than or equal to 32
//      // megabytes.
//
//  observerPtr->rotateOnTimeInterval(bdlt::DatetimeInterval(1));
//      // Rotate the file every 24 hours.
//..
// Note that in this configuration the user may end up with multiple log files
// for a specific day (because of the rotation-on-size rule).
//
// Next, we demonstrate how to correctly shut down the async file observer.  We
// first stop the publication thread by explicitly calling the
// 'stopPublicationThread' method.  This method blocks until all the log
// records that were on the record queue on entry to 'stopPublicationThread'
// have been published:
//..
//  observerPtr->stopPublicationThread();
//..
// Then, we disable the log rotation rules established earlier and also
// completely disable logging to a file:
//..
//  observerPtr->disableSizeRotation();
//
//  observerPtr->disableTimeIntervalRotation();
//
//  observerPtr->disableFileLogging();
//..
// Note that stopping the publication thread and disabling various features of
// the async file observer is not strictly necessary before object destruction.
// In particular, if a publication thread is still running when the destructor
// is invoked, all records on the record queue upon entry are published and
// then the publication thread is automatically stopped before destroying the
// async file observer.  In any case, all resources managed by the async file
// observer will be released when the object is destroyed.
//
// Finally, we can deregister our async file observer from the 'ball' logging
// subsystem entirely (and destroy the observer later):
//..
//  rc = manager.deregisterObserver("asyncObserver");
//  assert(0 == rc);
//..

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

#ifndef INCLUDED_BDLT_DATETIMEINTERVAL
#include <bdlt_datetimeinterval.h>
#endif

#ifndef INCLUDED_BSLMA_ALLOCATOR
#include <bslma_allocator.h>
#endif

#ifndef INCLUDED_BSLMF_NESTEDTRAITDECLARATION
#include <bslmf_nestedtraitdeclaration.h>
#endif

#ifndef INCLUDED_BSLMT_THREADUTIL
#include <bslmt_threadutil.h>
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

                          // ===============================
                          // struct AsyncFileObserver_Record
                          // ===============================

struct AsyncFileObserver_Record {
    // PRIVATE STRUCT.  For use by the 'ball::AsyncFileObserver' implementation
    // only.  This 'struct' holds a log record and its associated context.

    // PUBLIC DATA
    bsl::shared_ptr<const Record> d_record;   // log record
    Context                       d_context;  // context of log record
};

                          // =======================
                          // class AsyncFileObserver
                          // =======================

class AsyncFileObserver : public Observer {
    // This class implements the 'Observer' protocol.  The 'publish' method of
    // this class outputs log records asynchronously to 'stdout' and optionally
    // to a user-specified file.  This class is thread-safe; different threads
    // can operate on an object concurrently.  This class is exception-neutral
    // with no guarantee of rollback.  In no event is memory leaked.

    // DATA
    FileObserver                   d_fileObserver;   // forward most public
                                                     // method calls to this
                                                     // (non-async) file
                                                     // observer member

    bslmt::ThreadUtil::Handle      d_threadHandle;   // handle of asynchronous
                                                     // publication thread

    bdlcc::FixedQueue<AsyncFileObserver_Record>
                                   d_recordQueue;    // fixed-size queue of
                                                     // records processed by
                                                     // the publication thread

    bsls::AtomicInt                d_shuttingDownFlag;
                                                     // flag that indicates the
                                                     // publication thread is
                                                     // being shutdown

    Severity::Level                d_dropRecordsOnFullQueueThreshold;
                                                     // records with severity
                                                     // below this threshold
                                                     // are dropped when the
                                                     // queue is full; default
                                                     // is 'Severity::e_OFF'

    bsls::AtomicInt                d_dropCount;      // number of dropped
                                                     // records; reset to 0
                                                     // each time drop count is
                                                     // published

    bsl::function<void()>          d_publishThreadEntryPoint;
                                                     // publication thread
                                                     // entry point functor

    Record                         d_droppedRecordWarning;
                                                     // cached record used for
                                                     // publishing the count of
                                                     // dropped log records

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
        // C++11 constructor chaining is available on all supported platforms.

    void logDroppedMessageWarning(int numDropped);
        // Synchronously log a record to the underlying file observer
        // indicating that the specified 'numDropped' number of records have
        // been dropped since the last such warning was issued.  The behavior
        // is undefined if this method is invoked concurrently from multiple
        // threads, i.e., it is *not* thread-safe.

    void publishThreadEntryPoint();
        // Publish records from the record queue, to the log file and 'stdout',
        // until signaled to stop.  The behavior is undefined if this method is
        // invoked concurrently from multiple threads, i.e., it is *not*
        // thread-safe.  Note that this function is the entry point for the
        // publication thread.

    int shutdownThread();
        // Stop the publication thread and discard all currently queued log
        // records.  Return 0 on success, and a non-zero value if there is an
        // error joining the publication thread.  The behavior is undefined
        // unless the calling thread holds a lock on 'd_mutex'.

    int startThread();
        // Create a publication thread to publish records from the record
        // queue.  If a publication thread is already active, this operation
        // has no effect.  Return 0 on success, and a non-zero value if there
        // is an error creating the publication thread.  The behavior is
        // undefined unless the calling thread holds a lock on 'd_mutex'.

    int stopThread();
        // Stop the publication thread after all records on the record queue at
        // the time this method was entered have been published.  If there is
        // no publication thread this operation has no effect.  Return 0 on
        // success, and a non-zero value if there is an error joining the
        // publication thread.  The behavior is undefined unless the calling
        // thread holds a lock on 'd_mutex'.

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(AsyncFileObserver,
                                   bslma::UsesBslmaAllocator);

    // CREATORS
    explicit AsyncFileObserver(bslma::Allocator *basicAllocator  = 0);
    explicit AsyncFileObserver(Severity::Level   stdoutThreshold,
                               bslma::Allocator *basicAllocator  = 0);
        // Create an async file observer that asynchronously publishes log
        // records to 'stdout' if their severity is at least as severe as the
        // optionally specified 'stdoutThreshold' level, and has file logging
        // initially disabled.  If 'stdoutThreshold' is not specified, log
        // records are published to 'stdout' if their severity is at least as
        // severe as 'Severity::e_WARN'.  Optionally specify a 'basicAllocator'
        // used to supply memory.  If 'basicAllocator' is 0, the currently
        // installed default allocator is used.  Records received by the
        // 'publish' method are appended to a queue having a maximum (fixed)
        // length of 8192, and published later by an independent publication
        // thread.  All records received while the queue is full are discarded.
        // (See {Log Record Queue} for further information.)  Note that
        // 'isPublishInLocalTimeEnabled' returns 'false' following construction
        // indicating that the timestamp attribute of published records will be
        // written in UTC time (see 'enablePublishInLocalTime').  Also note
        // that independent default record formats are in effect for 'stdout'
        // and file logging (see 'setLogFormat').

    AsyncFileObserver(Severity::Level   stdoutThreshold,
                      bool              publishInLocalTime,
                      bslma::Allocator *basicAllocator = 0);
        // Create an async file observer that asynchronously publishes log
        // records to 'stdout' if their severity is at least as severe as the
        // specified 'stdoutThreshold' level, and has file logging initially
        // disabled.  The timestamp attribute of published records is written
        // in local time if the specified 'publishInLocalTime' flag is 'true',
        // and in UTC time otherwise.  Optionally specify a 'basicAllocator'
        // used to supply memory.  If 'basicAllocator' is 0, the currently
        // installed default allocator is used.  Records received by the
        // 'publish' method are appended to a queue having a maximum (fixed)
        // length of 8192, and published later by an independent publication
        // thread.  All records received while the queue is full are discarded.
        // (See {Log Record Queue} for further information.)  Note that
        // independent default record formats are in effect for 'stdout' and
        // file logging (see 'setLogFormat').

    AsyncFileObserver(Severity::Level   stdoutThreshold,
                      bool              publishInLocalTime,
                      int               maxRecordQueueSize,
                      bslma::Allocator *basicAllocator = 0);
    AsyncFileObserver(Severity::Level   stdoutThreshold,
                      bool              publishInLocalTime,
                      int               maxRecordQueueSize,
                      Severity::Level   dropRecordsOnFullQueueThreshold,
                      bslma::Allocator *basicAllocator = 0);
        // Create an async file observer that asynchronously publishes log
        // records to 'stdout' if their severity is at least as severe as the
        // specified 'stdoutThreshold' level, and has file logging initially
        // disabled.  The timestamp attribute of published records is written
        // in local time if the specified 'publishInLocalTime' flag is 'true',
        // and in UTC time otherwise.  Records received by the 'publish' method
        // are appended to a queue having the specified (fixed)
        // 'maxRecordQueueSize', and published later by an independent
        // publication thread.  Optionally specify a
        // 'dropRecordsOnFullQueueThreshold' indicating the severity threshold
        // below which records received when the queue is full will be
        // discarded; records received whose severity is at least as severe as
        // this threshold will block the calling thread if the queue is full,
        // until space is available.  If 'dropRecordsOnFullQueueThreshold' is
        // not specified, all records received while the queue is full are
        // discarded.  (See {Log Record Queue} for further information.)
        // Optionally specify a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.  Note that independent default record formats are in effect
        // for 'stdout' and file logging (see 'setLogFormat').

    ~AsyncFileObserver();
        // Publish all records that were on the record queue upon entry if a
        // publication thread is running, stop the publication thread (if any),
        // close the log file if file logging is enabled, and destroy this
        // async file observer.

    // MANIPULATORS
    void disableFileLogging();
        // Disable file logging for this async file observer.  This method has
        // no effect if file logging is not enabled.  Calling this method will
        // prevent the logging to a file of any unpublished records held by
        // this observer.  Note that records subsequently received through the
        // 'publish' method as well as those that are currently on the queue
        // may still be logged to 'stdout' after calling this method.

    void disablePublishInLocalTime();
        // Disable publishing of the timestamp attribute of records in local
        // time by this async file observer; henceforth, timestamps will be in
        // UTC time.  This method has no effect if publishing in local time is
        // not enabled.  Note that this method also affects log filenames (see
        // {Log Filename Patterns}).  Also note that this method affects
        // records subsequently received through the 'publish' method as well
        // as those that are currently on the queue.

    void disableSizeRotation();
        // Disable log file rotation based on log file size for this async file
        // observer.  This method has no effect if rotation-on-size is not
        // enabled.

    void disableStdoutLoggingPrefix();
        // Disable this async file observer from using the long output format
        // when logging to 'stdout'.  Henceforth, this async file observer will
        // use the default short output format ("%s %f:%l %c %m %u") when
        // logging to 'stdout'.  This method has no effect if the long output
        // format for 'stdout' logging is not enabled.  Note that this method
        // omits the "%d %p:%t " prefix from the default long output format.
        // Also note that this method affects records subsequently received
        // through the 'publish' method as well as those that are currently on
        // the queue.

    void disableTimeIntervalRotation();
        // Disable log file rotation based on a periodic time interval for this
        // async file observer.  This method has no effect if
        // rotation-on-time-interval is not enabled.

    int enableFileLogging(const char *logFilenamePattern);
        // Enable logging of all records published to this async file observer
        // to a file whose name is derived from the specified
        // 'logFilenamePattern'.  Return 0 on success, a positive value if file
        // logging is already enabled (with no effect), and a negative value
        // otherwise.  The basename of 'logFilenamePattern' may contain
        // '%'-escape sequences that are interpreted as follows:
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
        // Each time a log file is opened by this async file observer (upon a
        // successful call to this method and following each log file
        // rotation), the name of the new log file is derived from
        // 'logFilenamePattern' by interpreting the above recognized '%'-escape
        // sequences.  If 'isPublishInLocalTimeEnabled' returns 'true', the
        // '%'-escape sequences related to time will be substituted with local
        // time values, and UTC time values otherwise.  Note that, if a
        // publication thread is running, it will now publish queued records to
        // the log file where it previously was only publishing queued records
        // to 'stdout'.

    void enableStdoutLoggingPrefix();
        // Enable this async file observer to use the long output format when
        // logging to 'stdout'.  Henceforth, this async file observer will use
        // the output format for 'stdout' logging that was set by the most
        // recent call to 'setLogFormat', or the default long output format
        // ("%d %p:%t %s %f:%l %c %m %u") if 'setLogFormat' has not yet been
        // called.  This method has no effect if the long output format for
        // 'stdout' logging is already enabled.  Note that this method affects
        // records subsequently received through the 'publish' method as well
        // as those that are currently on the queue.

    void enablePublishInLocalTime();
        // Enable publishing of the timestamp attribute of records in local
        // time by this async file observer.  This method has no effect if
        // publishing in local time is already enabled.  Note that this method
        // also affects log filenames (see {Log Filename Patterns}).  Also note
        // that this method affects records subsequently received through the
        // 'publish' method as well as those that are currently on the queue.

    void forceRotation();
        // Forcefully perform a log file rotation by this async file observer.
        // Close the current log file, rename the log file if necessary, and
        // open a new log file.  This method has no effect if file logging is
        // not enabled.  See {Rotated File Naming} for details on filenames of
        // rotated log files.

    using Observer::publish;  // Avoid hiding base class.

    void publish(const bsl::shared_ptr<const Record>& record,
                 const Context&                       context);
        // Process the record referenced by the specified 'record' shared
        // pointer having the specified publishing 'context' by writing the
        // record and 'context' to the current log file if file logging is
        // enabled for this async file observer, and to 'stdout' if the
        // severity of 'record' is at least as severe as the value returned by
        // 'stdoutThreshold'.  'record' and 'context' are appended to the
        // record queue and published (asynchronously) later by the publication
        // thread.  If the queue is full, 'record' and 'context' are discarded
        // unless the constructor taking 'dropRecordsOnFullQueueThreshold' was
        // used; if 'dropRecordsOnFullQueueThreshold' was supplied with a
        // 'Severity::Level' at construction, 'record' and 'context' are
        // discarded only if the severity of 'record' is below that threshold,
        // otherwise, this method will block waiting until space is available
        // on the queue.  See {Log Record Queue} for further information.

    void releaseRecords();
        // Discard any shared references to 'Record' objects that were supplied
        // to the 'publish' method, and are held by this observer.  Note that
        // this operation should be called if resources underlying the
        // previously provided shared pointers must be released.  Also note
        // that all currently queued records are discarded.

    void rotateOnSize(int size);
        // Set this async file observer to perform log file rotation when the
        // size of the file exceeds the specified 'size' (in kilobytes).  This
        // rule replaces any rotation-on-size rule currently in effect.  The
        // behavior is undefined unless 'size > 0'.

    void rotateOnTimeInterval(const bdlt::DatetimeInterval& interval);
    void rotateOnTimeInterval(const bdlt::DatetimeInterval& interval,
                              const bdlt::Datetime&         startTime);
        // Set this async file observer to perform a periodic log file rotation
        // at multiples of the specified 'interval'.  Optionally specify a
        // 'startTime' indicating the *local* datetime to use as the starting
        // point for computing the periodic rotation schedule.  If 'startTime'
        // is unspecified, the current time is used.  This rule replaces any
        // rotation-on-time-interval rule currently in effect.  The behavior is
        // undefined unless '0 < interval.totalMilliseconds()'.  Note that
        // 'startTime' may be a fixed time in the past; e.g., a reference time
        // of 'bdlt::Datetime(1, 1, 1)' and an interval of 24 hours would
        // configure a periodic rotation at midnight each day.

    void setLogFormat(const char *logFileFormat, const char *stdoutFormat);
        // Set the format specifications for log records written to the log
        // file and to 'stdout' to the specified 'logFileFormat' and
        // 'stdoutFormat', respectively.  If the default short output format is
        // currently in effect for logging to 'stdout', this method has the
        // effect of calling 'enableStdoutLoggingPrefix' (see that method and
        // 'disableStdoutLoggingPrefix').  See {Log Record Formatting} for
        // details on the syntax of format specifications.  Note that default
        // formats are in effect following construction until this method is
        // called ("%d %p:%t %s %f:%l %c %m %u" for both file and 'stdout'
        // logging).  Also note that this method affects records subsequently
        // received through the 'publish' method as well as those that are
        // currently on the queue.

    void setOnFileRotationCallback(
              const FileObserver2::OnFileRotationCallback& onRotationCallback);
        // Set the specified 'onRotationCallback' to be invoked after each time
        // this async file observer attempts to perform a log file rotation.
        // The behavior is undefined if the supplied function calls either
        // 'setOnFileRotationCallback', 'forceRotation', or 'publish' on this
        // async file observer (i.e., the supplied callback should *not*
        // attempt to write to the 'ball' log).

    void setStdoutThreshold(Severity::Level stdoutThreshold);
        // Set the minimum severity of records logged to 'stdout' by this async
        // file observer to the specified 'stdoutThreshold' level.  Note that
        // if the value of 'stdoutThreshold' is 'Severity::e_OFF', logging to
        // 'stdout' is disabled.  Also note that this method affects records
        // subsequently received through the 'publish' method as well as those
        // that are currently on the queue.

    int shutdownPublicationThread();
        // Stop the publication thread without waiting for log records
        // currently on the record queue to be published, and discard all
        // currently queued records.  Return 0 on success, and a non-zero value
        // if there is an error joining the publication thread.  Note that log
        // records received by the 'publish' method will continue to be added
        // to the queue after the publication thread is shut down.

    int startPublicationThread();
        // Start a publication thread to asynchronously publish log records
        // from the record queue.  If a publication thread is already active,
        // this operation has no effect.  Return 0 on success, and a non-zero
        // value if there is an error creating the publication thread.  Note
        // that log records received by the 'publish' method may have
        // accumulated on the queue before the publication thread is started.

    int stopPublicationThread();
        // Block until all records that were on the record queue upon entry
        // have been published, then stop the publication thread.  If there is
        // no publication thread this operation has no effect.  Return 0 on
        // success, and a non-zero value if there is an error joining the
        // publication thread.  Note that log records received by the 'publish'
        // method will continue to be added to the queue after the publication
        // thread is stopped.

    // ACCESSORS
    void getLogFormat(const char **logFileFormat,
                      const char **stdoutFormat) const;
        // Load the format specification for log records written by this async
        // file observer to the log file into the specified '*logFileFormat'
        // address and the format specification for log records written to
        // 'stdout' into the specified '*stdoutFormat' address.  See {Log
        // Record Formatting} for details on the syntax of format
        // specifications.

    bool isFileLoggingEnabled() const;
    bool isFileLoggingEnabled(bsl::string *result) const;
        // Return 'true' if file logging is enabled for this async file
        // observer, and 'false' otherwise.  Load the optionally specified
        // 'result' with the name of the current log file if file logging is
        // enabled, and leave 'result' unmodified otherwise.  Note that records
        // received through the 'publish' method of this async file observer
        // may still be logged to 'stdout' when this method returns 'false'.

    bool isPublicationThreadRunning() const;
        // Return 'true' if a publication thread is running, and 'false'
        // otherwise.  Note that records received by the 'publish' method will
        // still be added to the record queue even if there is no publication
        // thread running.

    bool isPublishInLocalTimeEnabled() const;
        // Return 'true' if this async file observer writes the timestamp
        // attribute of records that it publishes in local time, and 'false'
        // otherwise (in which case timestamps are written in UTC time).  Note
        // that the value returned by this method also affects log filenames
        // (see {Log Filename Patterns}).

    bool isStdoutLoggingPrefixEnabled() const;
        // Return 'true' if this async file observer uses the long output
        // format when writing to 'stdout', and 'false' otherwise (in which
        // case the default short output format is used).  See
        // 'enableStdoutLoggingPrefix' and 'disableStdoutLoggingPrefix'.

    bool isUserFieldsLoggingEnabled() const;
        // Return 'true' if the logging of user-defined fields is enabled for
        // this async file observer, and 'false' otherwise.
        //
        // !DEPRECATED!: Do not use.

#ifndef BDE_OMIT_INTERNAL_DEPRECATED
    bdlt::DatetimeInterval localTimeOffset() const;
        // Return the difference between the local time and UTC time in effect
        // when this async file observer was constructed.  Note that this value
        // remains unchanged during the lifetime of this object and therefore
        // may become incorrect when the difference between the local time and
        // UTC time changes (e.g., when transitioning into or out of daylight
        // savings time).
        //
        // !DEPRECATED!: Use 'bdlt::LocalTimeOffset' instead.
#endif // BDE_OMIT_INTERNAL_DEPRECATED

    int recordQueueLength() const;
        // Return the number of log records currently on the record queue of
        // this async file observer.

    bdlt::DatetimeInterval rotationLifetime() const;
        // Return the log file lifetime that will trigger a file rotation by
        // this async file observer if rotation-on-lifetime is in effect, and a
        // 0 time interval otherwise.

    int rotationSize() const;
        // Return the size (in kilobytes) of the log file that will trigger a
        // file rotation by this async file observer if rotation-on-size is in
        // effect, and 0 otherwise.

    Severity::Level stdoutThreshold() const;
        // Return the minimum severity of records that will be logged to
        // 'stdout' by this async file observer.  Note that records with a
        // threshold less severe than 'stdoutThreshold()' may still be output
        // to the log file if file logging is enabled.
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
void AsyncFileObserver::disablePublishInLocalTime()
{
    d_fileObserver.disablePublishInLocalTime();
}

inline
void AsyncFileObserver::disableSizeRotation()
{
    d_fileObserver.disableSizeRotation();
}

inline
void AsyncFileObserver::disableStdoutLoggingPrefix()
{
    d_fileObserver.disableStdoutLoggingPrefix();
}

inline
void AsyncFileObserver::disableTimeIntervalRotation()
{
    d_fileObserver.disableTimeIntervalRotation();
}

inline
int AsyncFileObserver::enableFileLogging(const char *logFilenamePattern)
{
    return d_fileObserver.enableFileLogging(logFilenamePattern);
}

inline
void AsyncFileObserver::enablePublishInLocalTime()
{
    d_fileObserver.enablePublishInLocalTime();
}

inline
void AsyncFileObserver::enableStdoutLoggingPrefix()
{
    d_fileObserver.enableStdoutLoggingPrefix();
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
                                       const bdlt::Datetime&         startTime)
{
    d_fileObserver.rotateOnTimeInterval(interval, startTime);
}

inline
void AsyncFileObserver::setLogFormat(const char *logFileFormat,
                                     const char *stdoutFormat)
{
    d_fileObserver.setLogFormat(logFileFormat, stdoutFormat);
}

inline
void AsyncFileObserver::setOnFileRotationCallback(
               const FileObserver2::OnFileRotationCallback& onRotationCallback)
{
    d_fileObserver.setOnFileRotationCallback(onRotationCallback);
}

inline
void AsyncFileObserver::setStdoutThreshold(Severity::Level stdoutThreshold)
{
    d_fileObserver.FileObserver::setStdoutThreshold(stdoutThreshold);
}

// ACCESSORS
inline
void AsyncFileObserver::getLogFormat(const char **logFileFormat,
                                     const char **stdoutFormat) const
{
    d_fileObserver.getLogFormat(logFileFormat, stdoutFormat);
}

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
bool AsyncFileObserver::isPublicationThreadRunning() const
{
    return bslmt::ThreadUtil::invalidHandle() != d_threadHandle;
}

inline
bool AsyncFileObserver::isPublishInLocalTimeEnabled() const
{
    return d_fileObserver.isPublishInLocalTimeEnabled();
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

#ifndef BDE_OMIT_INTERNAL_DEPRECATED
inline
bdlt::DatetimeInterval AsyncFileObserver::localTimeOffset() const
{
    return d_fileObserver.localTimeOffset();
}
#endif // BDE_OMIT_INTERNAL_DEPRECATED

inline
int AsyncFileObserver::recordQueueLength() const
{
    return d_recordQueue.length();
}

inline
bdlt::DatetimeInterval AsyncFileObserver::rotationLifetime() const
{
    return d_fileObserver.rotationLifetime();
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
