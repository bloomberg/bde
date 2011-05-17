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
//@AUTHOR: Guillaume Morin (gmorin1), Gang Chen (gchen20), Dino Oliva (doliva1)
//
//@DESCRIPTION: This component provides a concrete implementation of the
// 'bael_Observer' protocol for publishing log records to a user-specified
// file:
//..
//               ( bael_FileObserver2 )
//                         |              ctor
//                         |              disableFileLogging
//                         |              disableLifetimeRotation
//                         |              disableSizeRotation
//                         |              disablePublishInLocalTime
//                         |              enableFileLogging
//                         |              enablePublishInLocalTime
//                         |              forceRotation
//                         |              rotateOnSize
//                         |              rotateOnLifetime
//                         |              setLogFileFunctor
//                         |              isFileLoggingEnabled
//                         |              isPublishInLocaltimeEnabled
//                         |              rotationLifetime
//                         |              rotationSize
//                         |              localTimeOffset
//                         V
//                 ( bael_Observer )
//                                        dtor
//                                        publish
//..
// An instance of 'bael_FileObserver2' processes the log records received
// through its 'publish' method by writing them to a user-specified file.
// The format of published log records is user-configurable (see "Log Record
// Formatting" below).  In addition, a file observer may be configured to
// perform automatic log file rotation (see "Log File Rotation" below).
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
// 'bael_RecordStringFormatter conveniently provides such a functor:
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
// changed by calling 'enablePublishInLocalTime()' which will cause timestamp
// attributes to be written in local time instead.  This method will not have
// the intended effect if the 'setLogFileFunctor' method has been called to
// install an alternate logging functor from the default.  However, if the
// functor passed to the 'setLogFileFunctor' method is provided by a
// 'bael_RecordStringFormatter' object, the timestamp attribute of records can
// be configured to be written in local time or UTC time as desired.  (See the
// component-level documentation of 'bael_recordstringformatter' for more
// information on configuring the format of log records.)
//
///Log File Rotation
///-----------------
// A 'bael_FileObserver2' may be configured to perform automatic rotation of
// log files based on simple file rotation conditions (or rules).  Rotation
// rules may be established based on the size of the log file (i.e., a
// "rotation-on-size" rule), and the length of time that the log file has been
// opened (i.e., a "rotation-on-lifetime" rule).  These rules are independently
// enabled by the 'rotateOnSize' and 'rotateOnLifetime' methods, respectively.
// If both rules are in effect, log file rotation is performed when either rule
// applies.  When file rotation occurs, the current log file is closed and
// optionally renamed as indicated by the most recent successful call to
// 'enableFileLogging' (e.g., a typical configuration will cause the file
// to be renamed by appending a timestamp of the form '.YYYYMMDD_HHMMSS').
// A new log file will be opened after the file is rotated.  If the new log
// file has the same name as the rotated log file, a ".1" suffix will be
// appended to the rotated file.  If a file already exists with a ".N" suffix ,
// rename the existing file with the suffix ".N+1" (recursively).
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
// size of the log file or its lifetime:
//..
//  fileObserver.enableFileLogging("/var/log/task/task.log");
//      // Create and log records to a file named "/var/log/task/task.log".
//
//  fileObserver.rotateOnSize(1024 * 256);
//      // Rotate the file when its size becomes greater than or equal to 256
//      // mega-bytes.
//
//  fileObserver.rotateOnLifetime(bdet_DatetimeInterval(1));
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

#ifndef INCLUDED_BSL_LIST
#include <bsl_list.h>
#endif

#ifndef INCLUDED_BSL_STRING
#include <bsl_string.h>
#endif

#ifndef INCLUDED_BDESU_FDSTREAMBUF
#include <bdesu_fdstreambuf.h>
#endif

namespace BloombergLP {

class bael_Context;
class bael_Record;

class bslma_Allocator;

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

  private:
    // DATA
    bdesu_FdStreamBuf      d_logStreamBuf;             // stream buffer for
                                                       // file logging

    bsl::ostream           d_logFileStream;            // output stream for
                                                       // file logging

    bsl::string            d_logFilePattern;           // log filename pattern

    bsl::string            d_logFileName;              // current filename
                                                       // after replacing
                                                       // pattern tokens

    bdet_Datetime          d_logFileTimestamp;         // timestamp when log
                                                       // file was opened

    bool                   d_datetimeInfoInFileName;   // 'true' if there are
                                                       // any date/time fields
                                                       // in log filename

    bool                   d_isOpenWithTimestampFlag;  // 'true' if log file
                                                       // was opened with
                                                       // timestamp suffix

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

    bdet_DatetimeInterval  d_rotationLifetime;         // maximum log file
                                                       // lifetime before
                                                       // rotation

    // NOT IMPLEMENTED
    bael_FileObserver2(const bael_FileObserver2&);
    bael_FileObserver2& operator=(const bael_FileObserver2&);

  private:
    // PRIVATE MANIPULATORS
    void logRecordDefault(bsl::ostream& stream, const bael_Record& record);
        // Write the specified log 'record' to the specified output 'stream'
        // using the default record format of this file observer.

    int openLogFile();
        // Open a log file for logging by this file observer; return 0 on
        // success, and a non-zero value otherwise.  The name of the file is
        // indicated by the most recent successful call to 'enableFileLogging'.
        // The caller is responsible for acquiring any necessary lock.

    void rotateFile();
        // Perform log file rotation by closing the current log file of this
        // file observer, rename it as indicated by the most recent successful
        // call to 'enableFileLogging', and open a new log file.  If the new
        // file has the same name as the file that was just rotated, rename the
        // old file by appending a suffix ".1", and, if a file already exists
        // with the suffix ".N" rename the existing file with the suffix ".N+1"
        // (recursively).  The caller is responsible for acquiring any
        // necessary lock.


    void rotateIfNecessary(const bdet_Datetime& timestamp);
        // Perform log file rotation if any rotation rule currently in effect
        // for this file observer indicates that the log file should be
        // rotated, using the specified 'timestamp' to determine if the log
        // file has pass its maximum lifetime.  The caller is responsible for
        // acquiring any necessary lock.

    void setLogFileName();
        // Prepare this object for opening a new log file by updating the log
        // file name.  The name of the file is indicated by the most recent
        // successful call to 'enableFileLogging'.

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
        // Disable log file rotation based on log file lifetime for this file
        // observer.  This method has no effect if rotation-on-lifetime is not
        // enabled.

    void disableSizeRotation();
        // Disable log file rotation based on log file size for this file
        // observer.  This method has no effect if rotation-on-size is not
        // enabled.

    void disablePublishInLocalTime();
        // Disable publishing of the timestamp attribute of records in local
        // time by this file observer.  This method has no effect if publishing
        // in local time is not enabled.

    int enableFileLogging(const char *logFilenamePattern,
                          bool        appendTimestampFlag = false);
        // Enable logging of all messages published to this file observer to
        // a file indicated by the specified 'logFilenamePattern' and the
        // optionally-specified 'apppendTimestampFlag'.  The basename of
        // 'logFilenamePattern' may contain '%'-escape sequences that are
        // interpreted as follows:
        //..
        //   %Y - current year (four digits with leading zeros)
        //   %M - current month (two digits with leading zeros)
        //   %D - current day (two digits with leading zeros)
        //   %h - current hour (two digits with leading zeros)
        //   %m - current minute (two digits with leading zeros)
        //   %s - current second (two digits with leading zeros)
        //   %% - the empty string
        //..
        // Each time a log file is opened by this file observer (upon a
        // successful call to this method and following each log file rotation)
        // the name of the log file is derived from 'logFilenamePattern' by
        // interpolating the above recognized '%'-escape sequences.
        // Optionally, if the basename of 'logFilenamePattern' does *not*
        // contain any of the '%'-escape sequences recognized by this method,
        // set 'appendTimestampFlag' to 'true' to append '.%Y%M%D_%H%M%S' (the
        // current timestamp) to the filename.  If 'appendTimestampFlag' is
        // 'false' and 'logFilenamePattern' does not contain a recognized
        // '%'-escape sequence, a timestamp will be appended to the file *only*
        // when it is rotated (see Log File Rotation section under @DESCRIPTION
        // in component-level documentation for details).
        // 'appendTimestampFlag' has no effect if 'logFilenamePattern' contains
        // a recognized '%'-escape sequence.
        //
        // Return 0 on success, a positive value if file logging is already
        // enabled, and a negative value for any I/O error.

    void enablePublishInLocalTime();
        // Enable publishing of the timestamp attribute of records in local
        // time by this file observer.  This method has no effect if publishing
        // in local time is already enabled.

    void publish(const bael_Record& record, const bael_Context& context);
        // Process the specified log 'record' having the specified publishing
        // 'context' by writing 'record' and 'context' to a file if file
        // logging is enabled for this file observer.  The method has no effect
        // if file logging is not enabled.

    void forceRotation();
        // Forcefully perform a log file rotation by this file observer.  Close
        // the current log file, optionally rename it as indicated by the most
        // recent successful call to 'enableFileLogging', and open a new log
        // file.  This method has no effect if file logging is not enabled.

    void rotateOnSize(int size);
        // Set this file observer to perform log file rotation when the size of
        // the file exceeds the specified 'size' (in kilo-bytes).  This rule
        // replaces any rotation-on-size rule currently in effect, if any.  The
        // behavior is undefined unless 'size > 0'.

    void rotateOnLifetime(const bdet_DatetimeInterval& timeInterval);
        // Set this file observer to perform log file rotation when the length
        // of time that the file has been opened exceeds the specified
        // 'timeInterval'.  This rule replaces any rotation-on-lifetime rule
        // currently in effect, if any.

    void setLogFileFunctor(const LogRecordFunctor& logFileFunctor);
        // Set the formatting functor used when writing records to the log file
        // of this file observer to the specified 'logFileFunctor'.  Note that
        // a default format is in effect until this method is called.

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
