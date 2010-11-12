// bael_loggermanager.h                                               -*-C++-*-
#ifndef INCLUDED_BAEL_LOGGERMANAGER
#define INCLUDED_BAEL_LOGGERMANAGER

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a manager of core logging functionality.
//
//@CLASSES:
//                   bael_Logger: log record store and publication manager
//            bael_LoggerManager: logger factory and category administrator
// bael_LoggerManagerScopedGuard: scoped guard for managing singleton creation
//       bael_LoggerCategoryIter: sequential accessor of read-only categories
//      bael_LoggerCategoryManip: sequential accessor of modifiable categories
//
//@AUTHOR: Hong Shi (hshi2)
//
//@SEE_ALSO: bael_record, bael_recordattributes, bael_observer, bael_context,
//           bael_loggermanagerdefaults, bael_loggermanagerconfiguration,
//           bael_severity, bael_transmission, bael_log
//
//@DESCRIPTION: This component provides the core of the 'bael' logging toolkit:
// the logger class itself, 'bael_Logger', which manages log record storage and
// publication control, and the logger manager class, 'bael_LoggerManager',
// which is a singleton that is both a factory for loggers and a category
// manager.
//
///General Features and Behavior
///-----------------------------
// The 'bael' logging toolkit is very flexible.  A user can log messages with
// very little effort, and with only a superficial understanding of logger
// operation, in which case the logger will exhibit its "default behavior".
// The user can also elect to customize many aspects of logging, such as
// storage and publication behavior, both at start-up and dynamically during
// program execution.  Naturally, to exercise such control, the user must
// become more familiar with 'bael' logger operation; the user can choose
// more convenience or more versatility, with a reasonably fine granularity.
//
// Log records incorporate both fixed (logger-defined) and optional
// (user-defined) fields, affording yet more flexibility (see "Log Record
// Contents" below).  The logger directly populates certain of the required
// fields, and indirectly manages population of any optional fields by invoking
// a client-supplied callback function that sets the optional values.
//
// Clients obtain one or more loggers from the logger manager, although at most
// one logger may be "active" in any one thread; a request to log a message is
// directed to the active logger in that thread.  Each logger both stores and
// publishes appropriate log records.
//
// All loggers share a single registered observer to which log records are
// transmitted when they are published (see the component-level documentation
// of 'bael_observer' for more information on observers).  The shared observer
// must be supplied when the logger manager singleton is initialized.
//
// A logger can achieve high performance through the use of an in-memory record
// buffer for storing the records logged by a program.  Each logger is
// constructed with a record manager, which is an instance of a concrete class
// derived from 'bael_RecordBuffer'.  The singleton logger manager supplies a
// "default" record manager to the default logger; loggers allocated by the
// logger manager's 'allocateLogger' method use a record manager supplied by
// the client.  The default log record buffer is of user-configurable static
// size and is circular (see the 'bael_circularrecordbuffer' component for
// details), whereby continuous logging (without publication of logged records)
// can result in older records being overwritten by newer ones.  A circular
// buffer provides an efficient "trace-back" strategy, wherein only log records
// proximate to a user-specified logging event (see below) are published.  Such
// a circular buffer may not be appropriate for all situations; the user can
// change the behavior of the default logger by adjusting the logging threshold
// levels (see below) or can install a logger that uses a different kind of
// record buffer.
//
///Logger Manager Singleton Initialization
///---------------------------------------
// The recommended way to initialize the logger manager singleton is to create
// a 'bael_LoggerManagerScopedGuard' object in 'main' (*before* creating any
// threads).  The logger manager scoped guard constructor takes an observer, a
// configuration object (an instance of 'bael_LoggerManagerConfiguration'), and
// an optional allocator.  The logger manager singleton is created as a
// side-effect of creating the scoped guard object.  When the guard object goes
// out of scope (i.e., on program exit), the logger manager singleton is
// automatically destroyed.
//
// The 'bael_LoggerManagerConfiguration' object is used to supply a set of
// user-defined "default" values and other options.  However, to obtain the
// "default" logging behavior, it is sufficient to instantiate a default
// 'bael_LoggerManagerConfiguration' object and pass that to the constructor of
// the scoped guard along with an observer.  (See "Usage Examples" below.)
//
// As an alternative to using the scoped guard, the 'initSingleton' method that
// takes the same arguments as the scoped guard may be used to initialize the
// singleton.  However, in this case the 'shutDownSingleton' method must be
// explicitly called to destroy the logger manager singleton on program exit.
// Unless 'shutDownSingleton' is called, the singleton will not be destroyed
// and resources used by the singleton will leak.
//
///Deprecation Notice
///------------------
// Direct use of the 'public' logger manager constructor to initialize the
// logger manager singleton is *deprecated*.  The constructor will be declared
// 'private' in a future release.
//
// Direct use of any of the 8 'initSingleton' methods that do *not* take an
// instance of 'bael_LoggerManagerConfiguration' to initialize the logger
// manager singleton is *deprecated*.  These methods will be eliminated in a
// future release.
//
///Categories, Severities, and Threshold Levels
///--------------------------------------------
// The logger supports the notions of "severity level" and "category"; every
// record is logged at some severity level and to some category.  Categories
// are user-defined (except for the "default category"), and have unique names.
// Severity levels are integers in the range '[0 .. 255]', and are most
// typically chosen from among the enumeration in the 'bael_severity'
// component, although use of the 'enum' is optional.  The severity level and
// the category name are each among the fixed fields of the record being logged
// (see "Log Record Contents" below).
//
// From the logger's perspective, all categories are peers; there is no special
// significance to any sequence of characters in a category name.  The user may
// impose a hierarchical *meaning* to category names, and the logger manager
// facilitates a certain degree of hierarchical *behavior* via several callback
// functors provided within this component (see below, and also the
// 'bael_loggerfunctorpayloads' component).  However, such hierarchy is not
// fundamental to categories, nor to the behavior described in this section.
// Similarly, there is no a priori significance to severity levels except that
// they are ordered and may be compared for inequality, although the enumerator
// names in the 'bael_Severity::Level' enumeration (e.g., 'DEBUG', 'WARN',
// 'ERROR', etc.) suggest the intended "standard" meanings.
//
// Every category has associated with it four "severity threshold levels",
// which may be set explicitly by the user on category creation/registration
// (via the 'addCategory' method) or else will default to specific values via
// one of several mechanisms described below (invoked by the one-argument
// 'setCategory' method).  Category threshold levels may also be changed during
// program execution via the five-argument 'setCategory' method.
//
// When the user logs a record to a given category and at a given severity (via
// the 'bael_Logger' 'logMessage' method or via the logging macros -- see the
// 'bael_log' component), the logger manager uses the specified severity and
// the category's registered severity threshold levels to govern the logger's
// behavior; depending on the thresholds, the message may be recorded to an
// in-memory buffer, published to an external observer, or ignored.  In
// addition, if thresholds are set appropriately, the entire contents of the
// in-memory buffer of one or more loggers may be published to external
// observers.  Clients of the logger can use, and dynamically administer, the
// category threshold levels to enhance run-time performance and/or to reduce
// message volume while still capturing all critical log messages.
//
// The names and exact meanings of the four severity threshold levels are as
// follows:
//..
//   Record:       If the severity level of the record is at least as severe
//                 as the Record threshold level of the associated category,
//                 then the record will be stored by the logger in its
//                 log record buffer (i.e., it will be recorded).
//
//   Pass:         If the severity of the record is at least as severe as the
//                 Pass threshold level of the associated category, then the
//                 record will be immediately published by the logger (i.e., it
//                 will be transmitted to the logger's downstream recipient --
//                 the observer).
//
//   Trigger:      If the severity of the record is at least as severe as the
//                 Trigger threshold level of the associated category, then the
//                 record will cause immediate publication of that record and
//                 any records in the logger's log record buffer (i.e., this
//                 record will trigger a general log record dump).
//
//   Trigger-All:  If the severity of the record is at least as severe as the
//                 Trigger-All threshold level of the associated category, then
//                 the record will cause immediate publication of that record
//                 and all other log records stored by *all* active loggers.
//..
// Note that more than one of the above actions can apply to a given log
// record, since the four threshold levels are independent of one another.
// Note also that *all* of these actions are governed by the threshold levels
// of the record being logged, and not by the threshold levels of any stored
// records that are published as a result of a Trigger or Trigger-All event.
//
///Terminology: "Factory Default" Thresholds
///- - - - - - - - - - - - - - - - - - - - -
// The logger manager supplies "default values" for category threshold levels
// whenever a category is created without client-supplied values.  These
// default values can come from any one of several possible sources, depending
// upon options that the user has elected; the system is flexible, but leads to
// a bit of confusion in terminology.  This section explains the meaning of
// "factory default" values and introduces the various "default" threshold
// mechanisms.
//
// The logger manager is a "factory" for loggers; we therefore define "factory
// defaults" to be the default values that the 'bael_LoggerManager' singleton
// is aware of at construction.  Depending on the values and options in the
// 'bael_LoggerManagerConfiguration' object provided to the logger manager on
// construction, the factory defaults may be either implementation-defined or
// user-defined.
//
// In either case, the user can *change* the default values during logger
// operation via the 'setDefaultThresholdLevels' method.  These threshold
// levels become the "default" values for new categories, but they are not
// "factory defaults", which can subsequently be restored via the
// 'resetDefaultThresholdLevels' method.
//
// A third mechanism, the 'bael_LoggerManager::DefaultThresholdLevelsCallback'
// functor, adds even more flexibility.  If this callback is installed by the
// user at construction, or subsequently via the
// 'setDefaultThresholdLevelsCallback' method, the callback is the source of
// all default thresholds, and the above mechanisms are not used.  The next
// section covers category thresholds in more detail.
//
///Category Creation, Management, and Threshold Levels
///- - - - - - - - - - - - - - - - - - - - - - - - - -
// When the logger manager singleton is created, a unique category known as the
// *default* *category* is created, and is given "factory-supplied" default
// threshold levels.  The default values for the default category are each in
// the range '[0 .. 255]', but are otherwise unspecified.  The user can also
// specify default values explicitly when the logger manager singleton is
// constructed.  This is accomplished by constructing a
// 'bael_LoggerManagerDefaults' object, setting the desired values, and then
// setting that object as an attribute of the 'bael_LoggerManagerConfiguration'
// argument to the 'bael_LoggerManagerScopedGuard' constructor.
//
// The default category is issued to the user via the return value of the
// 'setCategory(const char *categoryName)' method whenever a new category
// cannot be created due to a capacity limitation on the category registry
// maintained by the logger manager.  The method's normal behavior is to return
// the category having 'categoryName'.
//
// Categories that are added to the registry during logging through calls
// to the 'setCategory(const char *)' method are given threshold levels by
// one of two means.  The "default" mechanism (a slightly overloaded term in
// 'bael') is to use the same default thresholds as described above for the
// default category.  The alternative is to specify a
// 'bael_LoggerManager::DefaultThresholdLevelsCallback' functor, either when
// the logger manager singleton is initialized or else afterwards via the
// 'setDefaultThresholdLevelsCallback' method.  This functor, if provided, is
// used by the logger manager to supply the four 'int' threshold values; the
// functor may generate these values by any means that the user sees fit.  See
// the 'bael_loggerfunctorpayloads' component for an example payload function
// for the functor.
//
// The default threshold levels can be adjusted ('setDefaultThresholdLevels')
// and reset to their original values ('resetDefaultThresholdLevels').  Note
// that if factory values are overridden at initialization, a reset will
// restore thresholds to the user-specified default values.  In addition, there
// is a method to set the threshold levels of a given category to the current
// default threshold levels ('setCategoryThresholdsToCurrentDefaults') or
// to the factory-supplied (or client-overridden) default values
// ('setCategoryThresholdsToFactoryDefaults').
//
// As a final note regarding categories, a client can optionally supply to the
// logger manager on construction a
// 'bael_LoggerManager::CategoryNameFilterCallback' functor (via the
// 'bael_LoggerManagerConfiguration' object) to translate category names from
// an external to an internal representation.  For example, a project may allow
// programmers to refer to categories using mixed-case within an application,
// but provide a 'toLower' 'CategoryNameFilterCallback' to map all external
// upper-case letters to lower-case internally.  In this scenario, the
// (hypothetical) external category names "EQUITY.MARKET.NYSE" and
// "equity.market.nyse" would be mapped to the same category internally by the
// presumed 'toLower' functor.
//
///Log Record Contents
///-------------------
// Each log record contains a set of fixed fields and a set of optional,
// user-definable fields.  The following table lists the fixed fields in each
// log record (see the component-level documentation of 'bael_recordattributes'
// for more information on the fixed fields of a log record):
//..
//     Field Name         Type                  Description
//     -----------    -------------    --------------------------------
//     timestamp      bdet_Datetime    creation date and time
//     process ID     int              process ID of creator
//     thread ID      int              thread ID of creator
//     filename       string           file where created  (i.e., '__FILE__')
//     line number    int              line number in file (i.e., '__LINE__')
//     category       string           category name
//     severity       int              severity of logged record
//     message        string           log message text
//..
// The user-defined fields, if any, are described by a 'bdem_Schema' optionally
// supplied by the client when the logger manager singleton is created.  If a
// schema is supplied by the client, a corresponding
// 'bael_Logger::UserPopulatorCallback' functor must also be supplied.
// Thereafter, every logged record has its user-defined fields (indirectly)
// populated by an invocation of the 'UserPopulatorCallback' functor.
//
///Multi-Threaded Usage
///--------------------
// The 'bael' logging toolkit may be used in single-threaded and multi-threaded
// library code and applications with equal ease, and with virtually no
// difference in coding.  In particular, the same use of the
// 'bael_LoggerManagerScopedGuard' class to initialize the logger manager
// singleton is required in 'main' in both cases, and individual
// calls to the 'bael_Logger' instance method 'logMessage' (and logging calls
// via the logging macros -- see 'bael_log') are identical, from the user's
// perspective.  Category threshold administration is also identical in both
// cases.
//
// Differences in logger usage, or, more precisely, additional options for the
// multi-threaded user, arise when the user wishes to allocate one or more
// loggers beyond the default logger, which is owned by the singleton logger
// manager.  If a user does *not* explicitly allocate a logger (via the logger
// manager instance method 'allocateLogger') and install that logger for a
// given thread (via the manager instance method 'setLogger'), then all
// records from all threads in a program will be logged to the one default
// logger.  However, since each thread of execution may have its own logger
// instance, multi-threaded users may choose to allocate and install multiple
// loggers.  Note that each thread may have at most one logger, but a single
// logger may be used by any number of threads.
//
// Multi-threaded users of logging may prefer to allocate and install one
// logger per thread in order to take advantage of the "trace-back" feature
// described above on a per-thread basis.  In the event of an error condition
// as defined by the programmer, such a logging configuration provides a
// trace-back through the record buffer of the thread that caused the error,
// without any dilution from records from other threads.  Conversely, if
// several threads are known to interact closely, it may be advantageous to
// have them share a common logger so that the trace-back log *does* include
// all relevant records.
//
///Usage Examples
///--------------
// This section illustrates instantiation of the logger manager singleton,
// which is required (once!) in 'main', and also shows *direct* use of the
// logger and logger manager interfaces, much of which is actually *not*
// recommended.  The most basic logger functionality has been wrapped in macros
// defined in the 'bael_log' component.  See the 'bael' package-level
// documentation and the 'bael_log' component documentation for recommended
// real-world usage examples.
//
///Usage 1 -- Initialization #1
///- - - - - - - - - - - - - -
// Clients that perform logging must first instantiate the singleton logger
// manager using the 'bael_LoggerManagerScopedGuard' class.  This example shows
// how to create a logger manager with the most basic "default behavior".
// Subsequent examples will show more customized behavior.  Note that a
// 'bael_Observer' (which will receive the records that are published) is a
// required argument; we will use a 'bael_DefaultObserver' here.
//
// The following snippets of code illustrate the initialization sequence
// (typically performed near the top of 'main').
//
// First, we create a 'bael_DefaultObserver' object 'observer' that will
// publish records to 'stdout':
//..
//    // myApp.cpp
//
//    int main() {
//
//        // ...
//
//        static bael_DefaultObserver observer(bsl::cout);
//..
// Next, we create a 'bael_LoggerManagerConfiguration' object, 'configuration'.
// In this example, the default configuration will be good enough:
//..
//        bael_LoggerManagerConfiguration configuration;
//..
// We next create a 'bael_LoggerManagerScopedGuard' object whose constructor
// takes the observer and configuration object just created.  The guard will
// initialize the logger manager singleton on creation and destroy the
// singleton upon destruction.  This guarantees that any resources used by the
// logger manager will be properly released when they are not needed:
//..
//        bael_LoggerManagerScopedGuard guard(&observer, configuration);
//..
// The application is now prepared to log messages using the 'bael' logging
// subsystem:
//..
//        // ...
//    }
//..
///Usage 2 -- Client Usage
///- - - - - - - - - - - -
// Once the logger manager has been initialized, the client may use it to
// obtain a logger and begin logging.  This example builds on the previous, by
// assuming that the initialization steps above have been taken, to illustrate
// an application logging messages.
//
// The following simple 'factorial' function takes and returns values of type
// 'int'.  Note that this function has a very limited range of input, namely
// integers in the range '[0 .. 13]'.  This limited range serves to illustrate
// a usage pattern of the logger, namely to log "warnings" whenever a key
// function is given bad input.
//
// For this example, it is sufficient to use the severity levels defined in the
// 'bael_Severity::Level' enumeration:
//..
//    enum Level {
//        OFF   =   0,  // disable generation of corresponding message
//        FATAL =  32,  // a condition that will (likely) cause a *crash*
//        ERROR =  64,  // a condition that *will* cause incorrect behavior
//        WARN  =  96,  // a *potentially* problematic condition
//        INFO  = 128,  // data about the running process
//        DEBUG = 160,  // information useful while debugging
//        TRACE = 192   // execution trace data
//    };
//..
// Note that the intervals left between enumerator values allow applications
// to define additional values in case there is a desire to log with more
// finely-graduated levels of severity.  We will not need that granularity
// here; 'bael_Severity::BAEL_WARN' is appropriate to log a warning message if
// the input argument to our factorial function is not in this range of values.
//
// We will register a unique category for this function, so that logged
// messages from our function will be identified in the published output.
// Also, with a unique category name, the logging behavior of this function can
// be administered by resetting the various threshold levels for the category.
// In this example, we will accept the default thresholds.
//
// The 'setCategory' method accepts a name and returns the address of a
// 'bael_Category' with that name or, in some circumstances, the address of the
// *Default* *Category* (see the function-level documentation of 'setCategory'
// for details).  The address returned by 'setCategory' is stored in a
// function-static pointer variable (i.e., it is fetched only once upon first
// use).  In this example, we assume that we are writing a function for
// Equities Graphics that will live in that group's Math library.  The dot
// "delimiters" ('.') have no particular significance to the logger, but may be
// used by the administration methods to "induce" a hierarchical behavior on
// our category, should that be useful.  See, e.g., the callback functor
// 'bael_LoggerManager::DefaultThresholdLevelsCallback' and its documentation,
// and Usage Example 3 below for information on how to use category names to
// customize logger behavior:
//..
//   int factorial(int n)
//       // Return the factorial of the specified value 'n' if the factorial
//       // can be represented as an 'int', and a negative value otherwise.
//   {
//       static const bael_Category *factorialCategory =
//                      logger.setCategory("equities.graphics.math.factorial");
//..
// We must also obtain a reference to a logger by calling the logger manager
// 'getLogger' method.  Note that this logger may not safely be cached as a
// function 'static' variable since our function may be called in different
// threads having different loggers.  Even in a single-threaded program, the
// owner of 'main' is free to install new loggers at any point, so a
// statically-cached logger would be a problem:
//..
//       bael_Logger& logger = bael_LoggerManager::singleton().getLogger();
//..
// Now we validate the input value 'n'.  If 'n' is either negative or too
// large, we will log a warning message (at severity level
// 'bael_Severity::BAEL_WARN') and return a negative value.  Note that calls to
// 'logMessage' have no run-time overhead (beyond the execution of a simple
// 'if' test) unless 'bael_Severity::BAEL_WARN' is at least as severe as one
// of the threshold levels of 'factorialCategory':
//..
//       if (0 > n) {
//           logger.logMessage(*factorialCategory,
//                             bael_Severity::BAEL_WARN,
//                             __FILE__,
//                             __LINE__,
//                             "Attempt to take factorial of negative value.");
//           return n;
//       }
//
//       enum { MAX_ARGUMENT = 13; };  // maximum value accepted by 'factorial'
//
//       if (MAX_ARGUMENT < n) {
//           logger.logMessage(*factorialCategory,
//                             bael_Severity::BAEL_WARN,
//                             __FILE__,
//                             __LINE__,
//                             "Result too large for 'int'.");
//           return -n;
//       }
//..
// The remaining code proceeds mostly as expected, but adds one last message
// that tracks control flow when 'bael_Severity::BAEL_TRACE' is at least as
// severe as one of the threshold levels of 'factorialCategory' (e.g., as
// might be the case during debugging):
//..
//       int product = 1;
//       while (1 < n) {
//           product *= n;
//           --n;
//       }
//
//       logger.logMessage(*factorialCategory,
//                         bael_Severity::BAEL_TRACE,
//                         __FILE__,
//                         __LINE__,
//                         "Exiting 'factorial' successfully.");
//
//       return product;
//   }
//..
///Usage 3 -- Initialization #2
///- - - - - - - - - - - - - -
// In this example, we demonstrate a more elaborate initial configuration for
// the logger manager.  In particular, we create the singleton logger manager
// with a configuration that has a category name filter functor, a
// 'DefaultThresholdLevelsCallback' functor, and user-chosen values for the
// "factory default" threshold levels.
//
// First we define three 'static' functions that are employed by the two
// functors.  The 'toLower' function implements our category name filter.  It
/// is wrapped within a functor object and maps category names to lower-case:
//..
//    static
//    void toLower(bsl::string *buffer, const char *s)
//    {
//        assert(buffer);
//        assert(s);
//
//        buffer->clear();
//        while (*s) {
//            buffer->push_back(bsl::tolower(static_cast<unsigned char>(*s)));
//            ++s;
//        }
//        buffer->push_back(0);
//    }
//..
// The following two functions provide the implementation for our
// 'DefaultThresholdLevelsCallback' functor.  The 'inheritThresholdLevels'
// function is wrapped within a functor object; the 'getDefaultThresholdLevels'
// function is a helper that does the hard work.  We assume a hierarchical
// category naming scheme that uses '.' to delimit the constituents of names.
// For example, the three categories named "x", "x.y", and "x.y.z" are related
// in the sense that "x" is an ancestor of both "x.y" and "x.y.z", and "x.y" is
// an ancestor "x.y.z".  Suppose that "x" is added to the registry first.  If
// "x.y" is then added to the registry by calling 'setCategory(const char *)',
// it would "inherit" threshold level values from "x".  Similarly, when "x.y.z"
// is added to the registry by calling the 1-argument 'setCategory' method, it
// inherits threshold level values from "x.y" (i.e., a category inherits from
// its nearest ancestor that exists in the registry when it is added).  Note
// that a category named "xx.y" (for example) is not related to either of "x",
// "x.y", or "x.y.z":
//..
//    static
//    int getDefaultThresholdLevels(int                       *recordLevel,
//                                  int                       *passLevel,
//                                  int                       *triggerLevel,
//                                  int                       *triggerAllLevel,
//                                  char                       delimiter,
//                                  const bael_LoggerManager&  loggerManager,
//                                  const char                *categoryName)
//        // Obtain appropriate threshold levels for the category having the
//        // specified 'categoryName' by searching the registry of the
//        // specified 'loggerManager', and store the resulting values at the
//        // specified 'recordLevel', 'passLevel', 'triggerLevel', and
//        // 'triggerAllLevel' addresses.  A hierarchical category naming
//        // scheme is assumed that employs the specified 'delimiter' to
//        // separate the components of category names.  Return 0 on success,
//        // and a non-zero value otherwise.  The behavior is undefined unless
//        // 'recordLevel', 'passLevel', 'triggerLevel', and 'triggerAllLevel'
//        // are non-null, and 'categoryName' is null-terminated.
//    {
//        assert(recordLevel);
//        assert(passLevel);
//        assert(triggerLevel);
//        assert(triggerAllLevel);
//        assert(categoryName);
//
//        enum { SUCCESS = 0, FAILURE = -1 };
//
//        bsl::string buffer(categoryName);
//        while (1) {
//            for (bael_LoggerCategoryIter it(loggerManager); it; ++it) {
//                const bael_Category& category = it();
//                const char *name = category.categoryName();
//                if (0 == bsl::strcmp(name, buffer.c_str())) {
//                    *recordLevel     = category.recordLevel();
//                    *passLevel       = category.passLevel();
//                    *triggerLevel    = category.triggerLevel();
//                    *triggerAllLevel = category.triggerAllLevel();
//                    return SUCCESS;                                 // RETURN
//                }
//            }
//            char *p = bsl::strrchr(buffer.c_str(), delimiter);
//            if (!p) {
//                return FAILURE;                                     // RETURN
//            }
//            *p = 0;
//        }
//    }
//
//    static
//    void inheritThresholdLevels(int        *recordLevel,
//                                int        *passLevel,
//                                int        *triggerLevel,
//                                int        *triggerAllLevel,
//                                const char *categoryName)
//        // Obtain appropriate threshold levels for the category having the
//        // specified 'categoryName', and store the resulting values at the
//        // specified 'recordLevel', 'passLevel', 'triggerLevel', and
//        // 'triggerAllLevel' addresses.  The behavior is undefined unless
//        // 'recordLevel', 'passLevel', 'triggerLevel', and 'triggerAllLevel'
//        // are non-null, and 'categoryName' is null-terminated.
//    {
//        assert(recordLevel);
//        assert(passLevel);
//        assert(triggerLevel);
//        assert(triggerAllLevel);
//        assert(categoryName);
//
//        const bael_LoggerManager& manager = bael_LoggerManager::singleton();
//        if (0 != getDefaultThresholdLevels(recordLevel,
//                                           passLevel,
//                                           triggerLevel,
//                                           triggerAllLevel,
//                                           '.',
//                                           manager,
//                                           categoryName)) {
//            *recordLevel     = manager.defaultRecordThresholdLevel();
//            *passLevel       = manager.defaultPassThresholdLevel();
//            *triggerLevel    = manager.defaultTriggerThresholdLevel();
//            *triggerAllLevel = manager.defaultTriggerAllThresholdLevel();
//        }
//    }
//..
// As in "Usage 1" above, we assume that the initialization sequence occurs
// somewhere near the top of 'main', and again we use a 'bael_DefaultObserver'
// to publish to 'stdout':
//..
//    // myApp2.cpp
//
//    int main() {
//
//        // ...
//
//        static bael_DefaultObserver observer(bsl::cout);
//..
// The following wraps the 'toLower' category name filter within a
// 'bdef_Function' functor:
//..
//        bael_LoggerManager::CategoryNameFilterCallback nameFilter(&toLower);
//..
// and the following wraps the 'inheritThresholdLevels' function within a
// 'bdef_Function' functor:
//..
//        bael_LoggerManager::DefaultThresholdLevelsCallback
//                                 thresholdsCallback(&inheritThresholdLevels);
//..
// Next we define four values for our custom "factory default" thresholds.
// These values will be stored within the logger manager and will be available
// to all users whenever the "factory defaults" are needed, for the life of the
// logger manager.  In this example, however, we will also be installing the
// 'thresholdsCallback' defined above, so unless that functor is un-installed
// (by a call to 'setDefaultThresholdLevelsCallback'), these four "factory
// defaults" will have no practical effect, since the callback mechanism "steps
// in front of" the default values:
//..
//        int recordLevel     = 125;
//        int passLevel       = 100;
//        int triggerLevel    =  75;
//        int triggerAllLevel =  50;
//..
// Now we can configure a 'bael_LoggerManagerDefaults' object, 'defaults', with
// these four threshold values.  'defaults' can then be used to configure the
// 'bael_LoggerManagerConfiguration' object that will be passed to the
// 'bael_LoggerManagerScopedGuard' constructor (below):
//..
//        bael_LoggerManagerDefaults defaults;
//        defaults.setDefaultThresholdLevelsIfValid(recordLevel,
//                                                  passLevel,
//                                                  triggerLevel,
//                                                  triggerAllLevel);
//..
// With 'defaults' and the callback functors defined above, we can now create
// and set the 'bael_LoggerManagerConfiguration' object, 'configuration', that
// will describe our desired configuration:
//..
//        bael_LoggerManagerConfiguration configuration;
//        configuration.setDefaultValues(defaults);
//        configuration.setCategoryNameFilterCallback(nameFilter);
//        configuration.setDefaultThresholdLevelsCallback(thresholdsCallback);
//..
// Finally, we can instantiate the singleton logger manager, passing in the
// 'observer' and 'configuration' that we have just created:
//..
//        bael_LoggerManagerScopedGuard guard(&observer, configuration);
//        bael_LoggerManager& manager = bael_LoggerManager::singleton();
//..
// The application is now prepared to log messages using the 'bael' logging
// subsystem, but first we will demonstrate the functors and client-supplied
// default threshold overrides.
//
// First, assume that we are not in the same lexical scope, and so we cannot
// see 'manager' above.  We must therefore obtain a reference to the singleton
// logger manager:
//..
//        bael_LoggerManager& loggerManager = bael_LoggerManager::singleton();
//..
// Next obtain a reference to the *Default* *Category* and 'assert' that
// its threshold levels match the client-supplied values that overrode the
// "factory-supplied" default values:
//..
//        const bael_Category& defaultCategory =
//                                             loggerManager.defaultCategory();
//        assert(125 == defaultCategory.recordLevel());
//        assert(100 == defaultCategory.passLevel());
//        assert( 75 == defaultCategory.triggerLevel());
//        assert( 50 == defaultCategory.triggerAllLevel());
//..
// Next add a category named "BloombergLP" (by calling 'addCategory').  Note
// that the logger manager invokes the supplied category name filter to map
// the category name to lower-case before the new category is added to the
// category registry.  The name filter is also invoked by 'lookupCategory'
// whenever a category is searched for (i.e., by name) in the registry:
//..
//        const bael_Category *blpCategory =
//                   loggerManager.addCategory("BloombergLP", 128, 96, 64, 32);
//        assert(blpCategory == loggerManager.lookupCategory("BLOOMBERGLP"));
//        assert(  0 == bsl::strcmp("bloomberglp",
//                                  blpCategory->categoryName()));
//        assert(128 == blpCategory->recordLevel());
//        assert( 96 == blpCategory->passLevel());
//        assert( 64 == blpCategory->triggerLevel());
//        assert( 32 == blpCategory->triggerAllLevel());
//..
// Next add a second category named "BloombergLP.bae.bael" (by calling
// 'setCategory') and 'assert' that the threshold levels are "inherited" from
// category "BloombergLP":
//..
//        const bael_Category *baelCategory =
//                           loggerManager.setCategory("BloombergLP.bae.bael");
//        assert(baelCategory ==
//                       loggerManager.lookupCategory("bloomberglp.bae.bael"));
//        assert(  0 == bsl::strcmp("bloomberglp.bae.bael",
//                                  baelCategory->categoryName()));
//        assert(128 == baelCategory->recordLevel());
//        assert( 96 == baelCategory->passLevel());
//        assert( 64 == baelCategory->triggerLevel());
//        assert( 32 == baelCategory->triggerAllLevel());
//..
// Finally add a third category named "Bloomberg.equities", again by calling
// 'setCategory'.  This category has no ancestor currently in the registry, so
// its threshold levels match those of the *Default* *Category*:
//..
//        const bael_Category *equitiesCategory =
//                           loggerManager.setCategory("BloombergLP.equities");
//        assert(equitiesCategory ==
//                       loggerManager.lookupCategory("BLOOMBERGLP.EQUITIES"));
//        assert(  0 == bsl::strcmp("bloomberglp.equities",
//                                  equitiesCategory->categoryName()));
//        assert(125 == equitiesCategory->recordLevel());
//        assert(100 == equitiesCategory->passLevel());
//        assert( 75 == equitiesCategory->triggerLevel());
//        assert( 50 == equitiesCategory->triggerAllLevel());
//
//        // ...
//    }
//..
///Usage 4 -- Efficient Logging of 'ostream'-able Objects
/// - - - - - - - - - - - - - - - - - - - - - - - - - - -
// The following example demonstrates how instances of a class supporting
// streaming to 'bsl::ostream' (via overloaded 'operator<<') can be logged.  It
// also demonstrates how to use the 'logMessage' method to log messages to a
// logger.  Suppose we want to *efficiently* log instances of the following
// class:
//..
//    class Information {
//        // This (incomplete) class is a simple aggregate of a "heading" and
//        // "contents" pertaining to that heading.  It serves to illustrate
//        // how to log the string representation of an object.
//
//        bsl::string d_heading;
//        bsl::string d_contents;
//
//      public:
//        Information(const char *heading, const char *contents);
//        ~Information();
//        const bsl::string& heading() const;
//        const bsl::string& contents() const;
//    };
//..
// In addition, we define the following free operator for streaming instances
// of 'Information' to an 'bsl::ostream':
//..
//    bsl::ostream& operator<<(bsl::ostream&      stream,
//                             const Information& information)
//    {
//        stream << information.heading()  << bsl::endl;
//        stream << ": ";
//        stream << information.contents() << bsl::endl;
//        return stream;
//    }
//..
// The following function logs an 'Information' object to the specified
// 'logger':
//..
//    void logInformation(bael_Logger          *logger,
//                        const Information&    information,
//                        bael_Severity::Level  severity,
//                        const bael_Category&  category,
//                        const char           *fileName,
//                        int                   lineNumber)
//    {
//..
// First, obtain a record that has its 'fileName' and 'lineNumber' attributes
// set:
//..
//        bael_Record *record = logger->getRecord(fileName, lineNumber);
//..
// Next, get a modifiable reference to the fixed fields of 'record':
//..
//        bael_RecordAttributes& attributes = record->fixedFields();
//..
// Create an 'bsl::ostream' to which the string representation 'information'
// can by output.  Note that 'stream' is supplied with the stream buffer of
// 'record':
//..
//        bsl::ostream stream(&attributes.messageStreamBuf());
//..
// Now stream 'information' into our output 'stream'.  This will set the
// message attribute of 'record' to the streamed data:
//..
//        stream << information;
//..
// Finally, log 'record' using 'logger':
//..
//        logger->logMessage(category, severity, record);
//    }
//..
// Notice that we did not need to allocate a scratch buffer to stream the
// object contents into.  That would have required an extra copy and the cost
// of allocation and deallocation, and thus would have been more inefficient.

#ifndef INCLUDED_BAESCM_VERSION
#include <baescm_version.h>
#endif

#ifndef INCLUDED_BAEL_CATEGORYMANAGER
#include <bael_categorymanager.h>
#endif

#ifndef INCLUDED_BAEL_LOGGERMANAGERCONFIGURATION
#include <bael_loggermanagerconfiguration.h>
#endif

#ifndef INCLUDED_BAEL_RECORD
#include <bael_record.h>
#endif

#ifndef INCLUDED_BAEL_THRESHOLDAGGREGATE
#include <bael_thresholdaggregate.h>
#endif

#ifndef INCLUDED_BAEL_TRANSMISSION
#include <bael_transmission.h>
#endif

#ifndef INCLUDED_BCEC_OBJECTPOOL
#include <bcec_objectpool.h>
#endif

#ifndef INCLUDED_BCEMT_MUTEX
#include <bcemt_mutex.h>
#endif

#ifndef INCLUDED_BCEMT_RWMUTEX
#include <bcemt_rwmutex.h>
#endif

#ifndef INCLUDED_BDEF_FUNCTION
#include <bdef_function.h>
#endif

#ifndef INCLUDED_BDEM_SCHEMA
#include <bdem_schema.h>
#endif

#ifndef INCLUDED_BSLMA_ALLOCATOR
#include <bslma_allocator.h>
#endif

#ifndef INCLUDED_BSL_MAP
#include <bsl_map.h>
#endif

#ifndef INCLUDED_BSL_SET
#include <bsl_set.h>
#endif

namespace BloombergLP {

class bael_LoggerManager;
class bael_LoggerCategoryIter;
class bael_LoggerCategoryManip;
class bael_Observer;
class bael_RecordBuffer;

template <typename TYPE>
class bdema_ManagedPtr;

class bdem_List;

                           // =================
                           // class bael_Logger
                           // =================

class bael_Logger {
    // This class provides log record management services.  Each instance of
    // 'bael_Logger' receives log records from one or more clients, manages the
    // storage of those records, and transmits them to a registered recipient
    // (i.e., an observer) when appropriate.

  public:
    // TYPES
    typedef bael_LoggerManagerConfiguration::UserPopulatorCallback
                                                         UserPopulatorCallback;
        // 'UserPopulatorCallback' is the type of a user-supplied callback
        // functor used to populate the user-defined fields in each log record.
        // Note that the user-defined fields of each record must be
        // type-consistent with the schema of the user populator callback.

    typedef bdef_Function<void (*)(bael_Transmission::Cause)>
                                                     PublishAllTriggerCallback;
        // 'PublishAllTriggerCallback' is the type of the functor that is
        // invoked with the publication cause to publish all record buffers of
        // all loggers that are allocated by the logger manager.

  private:
    bcec_ObjectPool<bael_Record>
                          d_recordPool;         // pool of records

    bael_Observer        *d_observer_p;         // holds observer (not owned)

    bael_RecordBuffer    *d_recordBuffer_p;     // holds log record buffer
                                                // (not owned)

    const bdem_Schema    *d_userSchema_p;       // holds schema for user-
                                                // defined fields (not owned)

    UserPopulatorCallback d_populator;          // user populator functor

    PublishAllTriggerCallback
                          d_publishAll;         // publishAll callback functor

    char                 *d_scratchBuffer_p;    // buffer for formatting log
                                                // messages (owned)

    int                   d_scratchBufferSize;  // message buffer size (bytes)

    bcemt_Mutex           d_scratchBufferMutex; // ensure thread-safety of
                                                // message buffer

    bael_LoggerManagerConfiguration::LogOrder
                          d_logOrder;           // logging order

    bael_LoggerManagerConfiguration::TriggerMarkers
                          d_triggerMarkers;     // trigger markers

    bslma_Allocator      *d_allocator_p;        // memory allocator (held, not
                                                // owned)

    // FRIENDS
    friend class bael_LoggerManager;

    // NOT IMPLEMENTED
    bael_Logger(const bael_Logger& original);
    bael_Logger& operator=(const bael_Logger& rhs);

    // PRIVATE CREATORS
    bael_Logger(
           bael_Observer                                   *observer,
           bael_RecordBuffer                               *recordBuffer,
           const bdem_Schema                               *schema,
           const UserPopulatorCallback&                     populator,
           const PublishAllTriggerCallback&                 publishAllCallback,
           int                                              scratchBufferSize,
           bael_LoggerManagerConfiguration::LogOrder        logOrder,
           bael_LoggerManagerConfiguration::TriggerMarkers  triggerMarkers,
           bslma_Allocator                                 *globalAllocator);
        // Create a logger having the specified 'observer' that receives
        // published log records, the specified 'recordBuffer' that stores log
        // records, the specified 'schema' that describes the structure of the
        // user-defined fields of log records, the specified 'populator' that
        // populates the user-defined fields of log records, the specified
        // 'publishAllCallback' that is invoked when a Trigger-All event
        // occurs, the specified 'scratchBufferSize' for the internal message
        // buffer accessible via 'obtainMessageBuffer', and the specified
        // 'globalAllocator' used to supply memory.  On a Trigger or
        // Trigger-All event, the messages are published in the specified
        // 'logOrder'.  The behavior is undefined unless 'observer',
        // 'recordBuffer', 'schema', and 'globalAllocator' are non-null.  Note
        // that this constructor is 'private' since the creation of instances
        // of 'bael_Logger' is managed by its 'friend' 'bael_LoggerManager'.

    ~bael_Logger();
        // Destroy this logger.

    // PRIVATE MANIPULATORS
    void publish(bael_Transmission::Cause cause);
        // Publish to the observer held by this logger all records stored in
        // the record buffer of this logger and indicate to the observer the
        // specified publication 'cause'.

    void logMessage(const bael_Category&            category,
                    int                             severity,
                    bael_Record                    *record,
                    const bael_ThresholdAggregate&  levels);
        // Log the specified '*record' after setting its category log field to
        // the specified 'category', severity log field to the specified
        // 'severity' and the rest of the log fields (except 'fileName',
        // 'lineNumber' and 'message', which are assumed to be already set in
        // the specified 'record'), based on the threshold levels of the
        // specified 'levels'.  (See the component-level documentation of
        // 'bael_record' for more information on the fields that are logged).
        // Store the record in the buffer held by this logger if 'severity' is
        // at least as severe as the current "Record" threshold level of
        // 'levels'.  Pass the record directly to the observer registered
        // with this logger if 'severity' is at least as severe as the current
        // "Pass" threshold level of 'levels'.  Publish the entire contents of
        // the buffer of this logger if 'severity' is at least as severe as
        // the current "Trigger" threshold level of 'levels'.  Publish the
        // entire contents of all buffers of all active loggers if 'severity'
        // is at least as severe as the current "Trigger-All" threshold level
        // of 'levels' (i.e., via the callback supplied at construction).
        // Note that this method will have no effect if 'severity' is less
        // severe than all of the threshold levels of 'levels'.  The behavior
        // is undefined unless 'severity' is in the range [1 .. 255], 'record'
        // is previously obtained by a call to 'getRecord', and 'record' is
        // not reused after invoking this method.

  public:
    // MANIPULATORS
    bael_Record *getRecord(const char *file, int line);
        // Return the address of a modifiable record having the specified
        // 'file' and 'line' attributes, and retrieved from the object pool
        // managed by this logger.

    void logMessage(const bael_Category&  category,
                    int                   severity,
                    const char           *fileName,
                    int                   lineNumber,
                    const char           *message);
        // Log a record containing the specified 'message' text, 'fileName',
        // 'lineNumber', 'severity', and the name of the specified 'category'.
        // (See the component-level documentation of 'bael_record' for more
        // information on the additional fields that are logged.)  Store the
        // record in the buffer held by this logger if 'severity' is at least
        // as severe as the current "Record" threshold level of 'category'.
        // Pass the record directly to the observer held by this logger if
        // 'severity' is at least as severe as the current "Pass" threshold
        // level of 'category'.  Publish the entire contents of the buffer of
        // this logger if 'severity' is at least as severe as the current
        // "Trigger" threshold level of 'category'.  Publish the entire
        // contents of all buffers of all active loggers of this logger factory
        // if 'severity' is at least as severe as the current "Trigger-All"
        // threshold level of 'category' (i.e., via the callback supplied at
        // construction).  Note that this method will have no effect if
        // 'severity' is less severe than all of the threshold levels of
        // 'category'.  The behavior is undefined unless 'severity' is in the
        // range '[1 .. 255]'.
        //
        // DEPRECATED: Use the three-argument 'logMessage' method instead.

    void logMessage(const bael_Category&  category,
                    int                   severity,
                    bael_Record          *record);
        // Log the specified '*record' after setting its category attribute to
        // the name of the specified 'category' and severity attribute to the
        // specified 'severity'.  (See the component-level documentation of
        // 'bael_record' for more information on the fields that are logged.)
        // Store the record in the buffer held by this logger if 'severity' is
        // at least as severe as the current "Record" threshold level of
        // 'category'.  Pass the record directly to the observer held by this
        // logger if 'severity' is at least as severe as the current "Pass"
        // threshold level of 'category'.  Publish the entire contents of
        // the buffer of this logger if 'severity' is at least as severe as the
        // current "Trigger" threshold level of 'category'.  Publish the entire
        // contents of all buffers of all active loggers if 'severity' is at
        // least as severe as the current "Trigger-All" threshold level of
        // 'category' (i.e., via the callback supplied at construction).  The
        // behavior is undefined unless 'severity' is in the range
        // '[1 .. 255]', both 'fileName' and 'message' are null-terminated, and
        // 'record' was previously obtained by a call to 'getRecord' on this
        // logger.  Note that this method will have no effect if 'severity' is
        // less severe than all of the threshold levels of 'category'.  Also
        // note that 'record' must not be reused after invoking this method.

    void publish();
        // Publish to the observer held by this logger all records stored in
        // the record buffer of this logger and indicate to the observer that
        // the cause is 'MANUAL_PUBLISH'.

    void removeAll();
        // Remove all log records from the record buffer of this logger.

    char *obtainMessageBuffer(bcemt_Mutex **mutex, int *bufferSize);
        // Block until access to the buffer of this logger used for formatting
        // messages is available.  Return the address of the modifiable buffer
        // to which this thread of execution has exclusive access, load the
        // address of the mutex that protects the buffer into the specified
        // '*mutex' address, and load the size (in bytes) of the buffer into
        // the specified 'bufferSize' address.  The address remains valid, and
        // the buffer remains locked by this thread of execution, until this
        // thread calls 'mutex->unlock()'.  The behavior is undefined if this
        // thread of execution currently holds a lock on the buffer.  Note that
        // the buffer is intended to be used *only* for formatting log messages
        // immediately before calling 'logMessage'; other use may adversely
        // affect performance for the entire program.

    char *messageBuffer();
        // Return the address of the modifiable message buffer managed by this
        // logger.  Note that the returned buffer is intended to be used *only*
        // for formatting log messages immediately before calling 'logMessage'.
        //
        // DEPRECATED: Use 'obtainMessageBuffer' instead.  Do *not* use this
        // method in multi-threaded code.

    // ACCESSORS
    int messageBufferSize() const;
        // Return the size, in bytes, of the message buffer managed by this
        // logger.
};

                           // ========================
                           // class bael_LoggerManager
                           // ========================

class bael_LoggerManager {
    // This class is a singleton.  It provides a factory for 'bael_Logger'
    // objects and is also a wrapper for category administration services.
    // Note that the services provided by this class are available only after
    // the singleton has been initialized.

  public:
    // TYPES
    typedef bael_LoggerManagerConfiguration::CategoryNameFilterCallback
                                                    CategoryNameFilterCallback;
        // 'CategoryNameFilterCallback' is the type of the user-supplied
        // functor that translates external category names to internal names.

    typedef bael_LoggerManagerConfiguration::DefaultThresholdLevelsCallback
                                                DefaultThresholdLevelsCallback;
        // 'DefaultThresholdLevelsCallback' is the type of the functor that
        // determines default threshold levels for categories added to the
        // registry by the 'setCategory(const char *)' method.

    typedef bael_Logger::PublishAllTriggerCallback PublishAllTriggerCallback;
        // 'PublishAllTriggerCallback' is the type of the functor that is
        // invoked to publish all record buffers of all active loggers (i.e.,
        // loggers allocated by the logger manager that have not yet been
        // deallocated).

    struct FactoryDefaultThresholds {
        // This 'struct' enables uniform use of an optional 'initSingleton'
        // argument to override the implementation-supplied default threshold
        // levels.  For example:
        //..
        //    bael_LoggerManager::FactoryDefaultThresholds x(128, 96, 64, 32);
        //..
        // defines an instance 'x' with record, pass, trigger, and
        // trigger-all threshold levels of 128, 96, 64, and 32, respectively.
        // The "factory-supplied" initial default threshold levels are
        // overridden by passing 'x' as an argument to one of the variants
        // of 'initSingleton'.
        //
        // DEPRECATED: Use 'bael_LoggerManagerDefaults' and
        // 'bael_LoggerManagerConfiguration' instead.

        int d_recordLevel;
        int d_passLevel;
        int d_triggerLevel;
        int d_triggerAllLevel;

        // CREATORS
        FactoryDefaultThresholds(int record,
                                 int pass,
                                 int trigger,
                                 int triggerAll)
        : d_recordLevel(record)
        , d_passLevel(pass)
        , d_triggerLevel(trigger)
        , d_triggerAllLevel(triggerAll)
        {
        }

        ~FactoryDefaultThresholds()
        {
        }
    };

  private:
    // FRIENDS
    friend class bael_LoggerCategoryIter;
    friend class bael_LoggerCategoryManip;

    // NOT IMPLEMENTED
    bael_LoggerManager(const bael_LoggerManager& original);
    bael_LoggerManager& operator=(const bael_LoggerManager& rhs);

    // CLASS DATA
    static bael_LoggerManager *s_singleton_p;    // singleton-enforcement

    // INSTANCE DATA
    bael_Observer         *d_observer_p;         // holds (but does not own)
                                                 // observer

    CategoryNameFilterCallback
                           d_nameFilter;         // category name filter
                                                 // functor

    DefaultThresholdLevelsCallback
                           d_defaultThresholds;  // functor for obtaining
                                                 // default threshold levels of
                                                 // "set" categories

    bael_ThresholdAggregate
                           d_defaultThresholdLevels;
                                                 // default threshold levels

    const bael_ThresholdAggregate
                           d_factoryThresholdLevels;
                                                 // factory default threshold
                                                 // levels

    bdem_Schema            d_userSchema;         // schema for user-defined
                                                 // fields

    bael_Logger::UserPopulatorCallback
                           d_populator;          // populator functor

    bael_Logger           *d_logger_p;           // holds default logger
                                                 // (owned)

    bael_CategoryManager   d_categoryManager;    // category manager

    unsigned int           d_maxNumCategoriesMinusOne;
                                                 // one less than the current
                                                 // capacity of the registry

    bsl::set<bael_Logger *>
                           d_loggers;            // set of *allocated* loggers

    bcemt_RWMutex          d_loggersLock;        // 'd_loggers' protector

    bael_RecordBuffer     *d_recordBuffer_p;     // holds record buffer (owned)

    PublishAllTriggerCallback
                           d_publishAllCallback; // self-installed callback
                                                 // functor to publish all
                                                 // records within process
                                                 // (always valid)

    bael_Category         *d_defaultCategory_p;  // holds *default* *category*
                                                 // (owned)

    int                    d_scratchBufferSize;  // logger default message
                                                 // buffer size (bytes)

    bsl::map<void *, bael_Logger *>
                           d_defaultLoggers;     // *registered* loggers

    bcemt_RWMutex          d_defaultLoggersLock; // registry lock

    bael_LoggerManagerConfiguration::LogOrder
                           d_logOrder;           // logging order

    bael_LoggerManagerConfiguration::TriggerMarkers
                           d_triggerMarkers;     // trigger markers

    bslma_Allocator       *d_allocator_p;        // memory allocator (held,
                                                 // not owned)

    // PRIVATE CLASS METHODS
    static void initSingletonImpl(
                      bael_Observer                          *observer,
                      const bael_LoggerManagerConfiguration&  configuration,
                      bslma_Allocator                        *globalAllocator);
        // Initialize (once!) the logger manager singleton having the specified
        // 'observer' that receives published log records, the specified
        // 'configuration' of defaults and attributes, and the specified
        // 'globalAllocator' used to supply memory.  If 'globalAllocator' is
        // 0, the currently installed global allocator is used.  The behavior
        // is undefined if 'observer' is 0, goes out of scope, or is otherwise
        // destroyed.  Note that this method has no effect if the logger
        // manager singleton has already been initialized.

    // PRIVATE CREATORS
    bael_LoggerManager(
                  const bael_LoggerManagerConfiguration&  configuration,
                  bael_Observer                          *observer,
                  bslma_Allocator                        *globalAllocator = 0);
        // Create a logger manager having the specified 'observer' that
        // receives published log records and the specified 'configuration' of
        // defaults and attributes.  Optionally specify a 'globalAllocator'
        // used to supply memory.  If 'globalAllocator' is 0, the currently
        // installed global allocator is used.  The behavior is undefined if
        // if 'observer' is 0, goes out of scope, or is otherwise destroyed.
        // Note that the new logger manager is *not* the singleton logger
        // manager used by macros of the BAEL logging framework.

  public:  // TBD: *TEMPORARILY* make the constructor 'public'; remove
           // this line when making the constructor 'private' again.

    // CREATORS
    bael_LoggerManager(
                  bael_Observer                          *observer,
                  const bael_LoggerManagerConfiguration&  configuration,
                  bslma_Allocator                        *globalAllocator = 0);
        // Create (once!) the logger manager singleton having the specified
        // 'observer' that receives published log records and the specified
        // 'configuration' of defaults and attributes.  Optionally specify a
        // 'globalAllocator' used to supply memory.  If 'globalAllocator' is 0,
        // the currently installed global allocator is used.  The behavior is
        // undefined if this (singleton) constructor is called more than once,
        // or if 'observer' is 0, goes out of scope, or is otherwise destroyed.

    ~bael_LoggerManager();
        // Destroy this logger manager.  Note that since the logger manager is
        // a singleton, this destructor should be called only with great care.
        // Unless you *know* that it is valid to do so, don't!

  private:  // TBD: *TEMPORARILY* make the constructor 'public'; remove
            // this line when making the constructor 'private' again.

    // PRIVATE MANIPULATORS
    void publishAllImp(bael_Transmission::Cause cause);
        // Transmit to the observer registered with this logger manager all log
        // records accumulated in the record buffers of all loggers managed by
        // this logger manager and indicate to the observer the specified
        // publication 'cause'.

    void constructObject(const bael_LoggerManagerConfiguration& configuration);
        // Construct the default category, default logger members, and
        // record buffer members of this logger manager based on the specified
        // 'configuration'.  The behavior is undefined if this method is
        // invoked again on this logger manager.

  public:
    // CLASS METHODS
    static bael_LoggerManager& initSingleton(
                  bael_Observer                          *observer,
                  const bael_LoggerManagerConfiguration&  configuration,
                  bslma_Allocator                        *basicAllocator = 0);
        // Initialize (once!) the logger manager singleton having the specified
        // 'observer' that receives published log records and the specified
        // 'configuration' of defaults and attributes.  Return a reference to
        // the modifiable logger manager singleton.  Optionally specify a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.  The behavior is
        // undefined if 'observer' is 0, goes out of scope, or is otherwise
        // destroyed.  Note that this method has no effect if the logger
        // manager singleton has already been initialized.

    static void shutDownSingleton();
        // Destroy the logger manager singleton and release all resources used
        // by it.  This method has no effect if the logger manager singleton
        // has not been initialized or has already been destroyed.  The
        // behavior is undefined if this method is called from one thread while
        // another thread is accessing the logger manager singleton (i.e., this
        // method is *not* thread-safe).

    static void initSingleton(
                        bael_Observer                    *observer,
                        bslma_Allocator                  *globalAllocator = 0);

    static void initSingleton(
                    bael_Observer                        *observer,
                    const DefaultThresholdLevelsCallback& defaultThresholds,
                    bslma_Allocator                      *globalAllocator = 0);
    static void initSingleton(
                        bael_Observer                    *observer,
                        const FactoryDefaultThresholds&   factoryThresholds,
                        bslma_Allocator                  *globalAllocator = 0);

    static void initSingleton(
               bael_Observer                             *observer,
               const bdem_Schema&                         userSchema,
               const bael_Logger::UserPopulatorCallback&  populator,
               bslma_Allocator                           *globalAllocator = 0);

    static void initSingleton(
                   bael_Observer                         *observer,
                   const DefaultThresholdLevelsCallback&  defaultThresholds,
                   const FactoryDefaultThresholds&        factoryThresholds,
                   bslma_Allocator                       *globalAllocator = 0);

    static void initSingleton(
               bael_Observer                             *observer,
               const DefaultThresholdLevelsCallback&      defaultThresholds,
               const bdem_Schema&                         userSchema,
               const bael_Logger::UserPopulatorCallback&  populator,
               bslma_Allocator                           *globalAllocator = 0);

    static void initSingleton(
               bael_Observer                             *observer,
               const FactoryDefaultThresholds&            factoryThresholds,
               const bdem_Schema&                         userSchema,
               const bael_Logger::UserPopulatorCallback&  populator,
               bslma_Allocator                           *globalAllocator = 0);

    static void initSingleton(
               bael_Observer                             *observer,
               const DefaultThresholdLevelsCallback&      defaultThresholds,
               const FactoryDefaultThresholds&            factoryThresholds,
               const bdem_Schema&                         userSchema,
               const bael_Logger::UserPopulatorCallback&  populator,
               bslma_Allocator                           *globalAllocator = 0);
        // Initialize (once!) the logger manager singleton having the specified
        // 'observer' that receives published log records.  Optionally specify
        // a category 'nameFilter' functor that translates external category
        // names to internal category names.  Optionally specify a
        // 'defaultThresholds' functor that determines default threshold levels
        // for categories added to the registry by 'setCategory(const char *)'.
        // Optionally specify 'factoryThresholds' to override the
        // "factory-supplied" initial default threshold levels.  Optionally
        // specify a 'userSchema' that describes the structure of the
        // user-defined fields of log records and a corresponding 'populator'
        // functor that populates those user-defined fields.  Optionally
        // specify a 'globalAllocator' used to supply memory.  If
        // 'globalAllocator' is 0, the currently installed global allocator is
        // used.  The behavior is undefined if 'observer' is 0, or goes out of
        // scope, or is otherwise destroyed.  Note that after this method has
        // been called once, subsequent calls have no effect.
        //
        // DEPRECATED: Use 'bael_LoggerManagerScopedGuard' instead.

    static void createLoggerManager(
                  bdema_ManagedPtr<bael_LoggerManager>   *manager,
                  bael_Observer                          *observer,
                  const bael_LoggerManagerConfiguration&  configuration,
                  bslma_Allocator                        *basicAllocator = 0);
        // Create a logger manager that is *not* the singleton logger manager
        // having the specified 'observer' that receives published log records
        // and the specified 'configuration' of defaults and attributes; load
        // the newly-created logger manager into the specified 'manager'
        // managed pointer.  Optionally specify a 'basicAllocator' used to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.  Note that this method does *not* create
        // the singleton logger manager used by the macros of the BAEL logging
        // framework.

    static bool isInitialized();
        // Return 'true' if the logger manager singleton has been initialized
        // and has not yet been destroyed, and 'false' otherwise.

    static bael_LoggerManager& singleton();
        // Return a reference to the modifiable logger manager singleton.  The
        // behavior is undefined unless the logger manager singleton has been
        // initialized and has not yet been destroyed.

    static bael_Record *getRecord(const char *file, int line);
        // Return the address of a modifiable record with the specified 'file'
        // and 'line' attributes, and whose memory is supplied by the currently
        // installed default allocator.

    static void logMessage(int severity, bael_Record *record);
        // Publish the specified 'record' to 'stderr' after setting its
        // severity attribute to the specified 'severity'.  The behavior is
        // undefined unless 'record' was obtained by a call to the
        // 'bael_LoggerManager::getRecord' method.

    static char *obtainMessageBuffer(bcemt_Mutex **mutex, int *bufferSize);
        // Block until access to the static buffer used for formatting messages
        // is available.  Return the address of the modifiable buffer to which
        // this thread of execution has exclusive access, load the address of
        // the mutex that protects the buffer into the specified '*mutex'
        // address, and load the size (in bytes) of the buffer into the
        // specified 'bufferSize' address.  The address remains valid, and the
        // buffer remains locked by this thread of execution, this thread calls
        // 'mutex->unlock()'.  The behavior is undefined if this thread of
        // execution currently holds a lock on the buffer.  Note that the
        // buffer is intended to be used *only* for formatting log messages
        // immediately before calling 'logMessage'; other use may adversely
        // affect performance for the entire program.

    // MANIPULATORS
    bael_Logger *allocateLogger(bael_RecordBuffer *buffer);
    bael_Logger *allocateLogger(bael_RecordBuffer *buffer,
                                int                scratchBufferSize);
    bael_Logger *allocateLogger(bael_RecordBuffer *buffer,
                                bael_Observer     *observer);
    bael_Logger *allocateLogger(bael_RecordBuffer *buffer,
                                int                scratchBufferSize,
                                bael_Observer     *observer);
        // Return the address of a modifiable logger managed by this logger
        // manager configured with the specified record 'buffer'.  Optionally
        // specify a 'scratchBufferSize' for the logger's user-accessible
        // message buffer.  Optionally specify an 'observer' that receives
        // published log records.  Note that this method is primarily intended
        // for use in multi-threaded applications, but can be used to partition
        // logging streams even within a single thread.  Also note that
        // ownership of 'buffer' and 'observer' is *not* transferred, and
        // hence, will *not* be destroyed (or otherwise affected) after the
        // logger is deallocated.

    void deallocateLogger(bael_Logger *logger);
        // Deallocate the specified 'logger' and release 'logger' from
        // management by this logger manager.  The behavior is undefined unless
        // 'logger' was obtained by invoking the 'allocateLogger' method of
        // this logger manager and 'logger' has not yet been deallocated.  Note
        // that this method is intended primarily for multi-threaded
        // applications.

    void setLogger(bael_Logger *logger);
        // Set the default logger used by this thread of execution to the
        // specified 'logger', or to the global default logger if 'logger' is
        // 0.  The behavior is undefined unless 'logger' was obtained from this
        // logger manager, and this thread of execution does not hold exclusive
        // access to the record buffer of its current default logger.

    bael_Logger& getLogger();
        // Return a reference to a modifiable logger managed by this logger
        // manager suitable for performing logging operations for this thread
        // of execution.

    bael_Category *lookupCategory(const char *categoryName);
        // Return the address of the modifiable category in the category
        // registry of this logger manager having the specified 'categoryName',
        // or 0 if no such category exists.  The behavior is undefined unless
        // 'categoryName' is null-terminated.

    bael_Category *addCategory(const char *categoryName,
                               int         recordLevel,
                               int         passLevel,
                               int         triggerLevel,
                               int         triggerAllLevel);
        // Add to the category registry of this logger manager a new category
        // having the specified 'categoryName' and the specified 'recordLevel',
        // 'passLevel', 'triggerLevel', and 'triggerAllLevel' threshold levels,
        // respectively, if (1) 'categoryName' is not present in the registry,
        // (2) the number of categories in the registry is less than the
        // registry capacity, and (3) each threshold level is in the range
        // '[0 .. 255]'.  Return the address of the new modifiable category on
        // success, and 0 otherwise.  The behavior is undefined unless
        // 'categoryName' is null-terminated.

    const bael_Category *setCategory(const char *categoryName);
        // Add to the category registry of this logger manager a new category
        // having the specified 'categoryName' and default threshold levels if
        // 'categoryName' is not present in the registry and the number of
        // categories in the registry is less than the registry capacity.
        // Return the address of the (possibly newly-created) modifiable
        // category having 'categoryName', if such a category exists, and the
        // address of the non-modifiable *default* *category* otherwise.  The
        // behavior is undefined unless 'categoryName' is null-terminated.
        // Note that a valid category address is *always* returned.

    const bael_Category *setCategory(bael_CategoryHolder *categoryHolder,
                                     const char          *categoryName);
        // Add to the category registry of this logger manager a new category
        // having the specified 'categoryName' and default threshold levels if
        // 'categoryName' is not present in the registry and the number of
        // categories in the registry is less than the registry capacity.
        // Return the address of the (possibly newly-created) modifiable
        // category having 'categoryName', if such a category exists, and the
        // address of the non-modifiable *default* *category* otherwise.  If
        // the specified 'categoryHolder' is non-null, then also load into
        // 'categoryHolder' the returned category and its maximum level and
        // link 'categoryHolder' to the category if it has not yet been linked.
        // The behavior is undefined unless 'categoryName' is null-terminated.
        // Note that a valid category address is *always* returned.

    bael_Category *setCategory(const char *categoryName,
                               int         recordLevel,
                               int         passLevel,
                               int         triggerLevel,
                               int         triggerAllLevel);
        // Add to the category registry of this logger manager a new category
        // having the specified 'categoryName' and 'recordLevel', 'passLevel',
        // 'triggerLevel', and 'triggerAllLevel' threshold levels,
        // respectively, if (1) 'categoryName' is not present in the registry,
        // (2) the number of categories in the registry is less than the
        // registry capacity, and (3) 'recordLevel', 'passLevel',
        // 'triggerLevel', and 'triggerAllLevel' are all within the range
        // '[0 .. 255]'.  If 'categoryName' is already present and each
        // threshold level is within the valid range then reset the threshold
        // levels of 'categoryName' to the specified values.  Return the
        // address of the (possibly newly-created) modifiable category having
        // 'categoryName' if 'categoryName' was either created or its
        // thresholds reset, and 0 otherwise.  The behavior is undefined unless
        // 'categoryName' is null-terminated.  Note that 0, and *not* the
        // *default* *category*, is returned on failure.

    bael_Category& defaultCategory();
        // Return a reference to the modifiable *Default* *Category* in the
        // category registry of this logger manager.

    int setDefaultThresholdLevels(int recordLevel,
                                  int passLevel,
                                  int triggerLevel,
                                  int triggerAllLevel);
        // Set the default threshold levels of this logger manager to the
        // specified 'recordLevel', 'passLevel', 'triggerLevel', and
        // 'triggerAllLevel' values, respectively, if each threshold level is
        // in the range '[0 .. 255]'.  Return 0 on success, and a non-zero
        // value otherwise (with no effect on any default threshold level).

    void resetDefaultThresholdLevels();
        // Reset the default threshold levels of this logger manager to the
        // original "factory-supplied" default values or the factory overrides
        // supplied at construction.

    void setCategoryThresholdsToCurrentDefaults(bael_Category *category);
        // Set the threshold levels of the specified 'category' in the category
        // registry of this logger manager to the current default threshold
        // values.  The behavior is undefined unless 'category' is non-null.

    void setCategoryThresholdsToFactoryDefaults(bael_Category *category);
        // Set the threshold levels of the specified 'category' in the category
        // registry of this logger manager to the original "factory-supplied"
        // default values or the factory overrides supplied at construction.
        // The behavior is undefined unless 'category' is non-null.

    void setDefaultThresholdLevelsCallback(
                                     DefaultThresholdLevelsCallback *callback);
        // Set the default-thresholds callback of this logger manager to the
        // specified 'callback'.  The default-thresholds callback is used to
        // determine default threshold levels for categories added to the
        // registry by the 'setCategory(const char *)' method.

    void setMaxNumCategories(int length);
        // Set the capacity of the category registry of this logger manager to
        // the specified 'length'.  If 'length' is 0, no limit will be imposed.
        // No categories are removed from the registry if the current number of
        // categories exceeds 'length'.  However, subsequent attempts to add
        // categories to the registry will fail.  The behavior is undefined
        // unless '0 <= length'.

    bael_Observer *observer();
        // Return the address of the modifiable observer registered with this
        // logger manager.

    void publishAll();
        // Transmit to the observer registered with this logger manager all log
        // records accumulated in the record buffers of all loggers managed by
        // this logger manager, and indicate the publication cause to be
        // 'MANUAL_PUBLISH_ALL'.

    int addRule(const bael_Rule& value);
        // Add a rule having the specified 'value' to the set of (unique)
        // rules maintained by this object.  Return the number of rules added
        // (i.e., 1 on success and 0 if a rule with the same value is already
        // present).

    int addRules(const bael_RuleSet& ruleSet);
        // Add each rule in the specified 'ruleSet' to the set of
        // (unique) rules maintained by this object.  Return the number of
        // rules added.  Note that each rule having the same value as an
        // existing rule will be ignored.

    int removeRule(const bael_Rule& value);
        // Remove the rule having the specified 'value' from this set of rules
        // maintained by this object.  Return the number of rules removed
        // (i.e., 1 on success and 0 if no rule having the same value is
        // found.)

    int removeRules(const bael_RuleSet& ruleSet);
        // Remove each rule in the specified 'ruleSet' from this set of
        // rules maintained by this object.  Return the number of rules
        // removed.

    void removeAllRules();
        // Remove every rule from the set of rules maintained by this object.

    // ACCESSORS
    bslma_Allocator *allocator() const;
        // Return the address of the modifiable allocator held by this logger
        // manager.

    const bael_Category *lookupCategory(const char *categoryName) const;
        // Return the address of the non-modifiable category in the category
        // registry of this logger manager having the specified 'categoryName',
        // or 0 if no such category exists.  The behavior is undefined unless
        // 'categoryName' is null-terminated.

    const bael_Category& defaultCategory() const;
        // Return a reference to the non-modifiable *default* *category* in the
        // category registry of this logger manager.

    const bael_Observer *observer() const;
        // Return the address of the non-modifiable observer registered with
        // this logger manager.

    const bael_Logger::UserPopulatorCallback *userPopulatorCallback() const;
        // Return the address of the non-modifiable user populator functor
        // registered with this logger manager, or 0 if there is no registered
        // user populator functor.

    int defaultRecordThresholdLevel() const;
        // Return the default record threshold level of this logger manager.

    int defaultPassThresholdLevel() const;
        // Return the default pass threshold level of this logger manager.

    int defaultTriggerThresholdLevel() const;
        // Return the default trigger threshold level of this logger manager.

    int defaultTriggerAllThresholdLevel() const;
        // Return the default trigger-all threshold level of this logger
        // manager.

    int maxNumCategories() const;
        // Return the current capacity of the category registry of this logger
        // manager.  A capacity of 0 implies that no limit will be imposed;
        // otherwise, new categories may be added only if
        // 'numCategories() < maxNumCategories()'.  Note that
        // '0 < maxNumCategories() < numCategories()' *is* a valid state,
        // implying no new categories may be added.

    int numCategories() const;
        // Return the number of categories in the category registry of this
        // logger manager.

    const bael_RuleSet& ruleSet() const;
        // Return a reference to the non-modifiable rule set maintained by
        // this object.

    bool isCategoryEnabled(const bael_Category *category,
                           int                  severity) const;

        // Return 'true' if the specified 'severity' is more severe (i.e., is
        // numerically less than) at least one of the threshold levels of the
        // specified 'category', and 'false' otherwise.  If the returned
        // 'value' is 'false', then a subsequent call to
        // 'getLogger().logMessage()' (with 'category' and 'severity') will
        // have no effect.  This method compares 'severity' with the threshold
        // levels determined by combining 'category->thresholdLevels()' with
        // the thresholds provided by any relevant and active logging rules
        // (in 'ruleSet()') that apply to 'category'.  Note that a rule
        // applies to 'category' if the rule's pattern matches
        // 'category->categoryName()', and a rule is active if all the
        // predicates defined for that rule are satisfied by the current
        // thread's attributes (i.e., 'bael_Rule::evaluate()' returns 'true'
        // for the collection of attributes maintained by the current thread's
        // 'bael_AttributeContext' object).
};

                        // ===================================
                        // class bael_LoggerManagerScopedGuard
                        // ===================================

class bael_LoggerManagerScopedGuard {
    // This class implements a scoped guard that, on construction, creates the
    // logger manager singleton, and, on destruction, destroys the singleton.

    // NOT IMPLEMENTED
    bael_LoggerManagerScopedGuard(
                                const bael_LoggerManagerScopedGuard& original);
    bael_LoggerManagerScopedGuard& operator=(
                                     const bael_LoggerManagerScopedGuard& rhs);

  public:
    // CREATORS
    bael_LoggerManagerScopedGuard(
                  bael_Observer                          *observer,
                  const bael_LoggerManagerConfiguration&  configuration,
                  bslma_Allocator                        *globalAllocator = 0);
        // Create a scoped guard that will create (once!) the logger manager
        // singleton having the specified 'observer' that receives published
        // log records and the specified 'configuration' of defaults and
        // attributes.  Optionally specify a 'globalAllocator' used to supply
        // memory.  If 'globalAllocator' is 0, the currently installed global
        // allocator is used.  The behavior is undefined if 'observer' is 0,
        // goes out of scope, or is otherwise destroyed.  Note that on
        // destruction, this scoped guard will destroy the logger manager
        // singleton, if the singleton exists at that time.

    ~bael_LoggerManagerScopedGuard();
        // Destroy the logger manager singleton, if the singleton exists, and
        // destroy this scoped guard.
};

                        // =============================
                        // class bael_LoggerCategoryIter
                        // =============================

class bael_LoggerCategoryIter {
    // This class provides sequential, read-only access to the categories in
    // the registry of a logger manager.  The order of iteration is undefined.
    //
    // This class is *DEPRECATED*, and should not be used.

    // DATA
    bael_CategoryManagerIter d_iter;  // contained category manager iterator

    // NOT IMPLEMENTED
    bael_LoggerCategoryIter(const bael_LoggerCategoryIter& original);
    bael_LoggerCategoryIter& operator=(const bael_LoggerCategoryIter& rhs);

  public:
    // CREATORS
    explicit bael_LoggerCategoryIter(const bael_LoggerManager& loggerManager);
        // Create an iterator for the specified 'loggerManager' initialized to
        // refer to the first category in the sequence of categories maintained
        // by 'loggerManager'.  The order of iteration is undefined.  The
        // behavior is undefined unless the lifetime of 'loggerManager' is at
        // least as long as the lifetime of this iterator.

    ~bael_LoggerCategoryIter();
        // Destroy this iterator.

    // MANIPULATORS
    void operator++();
        // Advance this iterator to refer to the next unvisited category.  If
        // no such category exists, this iterator becomes invalid.  The
        // behavior is undefined unless this iterator is initially valid.
        // Note that the order of iteration is undefined.

    // ACCESSORS
    operator const void *() const;
        // Return a non-zero value if this iterator is valid, and 0 otherwise.

    const bael_Category& operator()() const;
        // Return a reference to the non-modifiable category currently
        // referred to by this iterator.  The behavior is undefined unless
        // this iterator is valid.
};

                        // ==============================
                        // class bael_LoggerCategoryManip
                        // ==============================

class bael_LoggerCategoryManip {
    // This class provides sequential, modifiable access to the categories in
    // the registry of a logger manager.  The order of iteration is undefined.
    //
    // This class is *DEPRECATED*, and should not be used.

    // DATA
    bael_CategoryManagerManip d_manip;  // contained category manager
                                        // manipulator

    // NOT IMPLEMENTED
    bael_LoggerCategoryManip(const bael_LoggerCategoryManip& original);
    bael_LoggerCategoryManip& operator=(const bael_LoggerCategoryManip& rhs);

  public:
    // CREATORS
    explicit bael_LoggerCategoryManip(bael_LoggerManager *loggerManager);
        // Create a manipulator for the specified 'loggerManager' initialized
        // to refer to the first category in the sequence of categories
        // maintained by 'loggerManager'.  The order of iteration is undefined.
        // The behavior is undefined unless 'loggerManager' is non-null and the
        // lifetime of 'loggerManager' is at least as long as the lifetime of
        // this manipulator.

    ~bael_LoggerCategoryManip();
        // Destroy this manipulator.

    // MANIPULATORS
    void advance();
        // Advance this manipulator to refer to the next unvisited category.
        // If no such category exists, this manipulator becomes invalid.  The
        // behavior is undefined unless this manipulator is initially valid.
        // Note that the order of iteration is undefined.

    bael_Category& operator()();
        // Return a reference to the modifiable category currently referred to
        // by this manipulator.  The behavior is undefined unless this
        // manipulator is valid.

    // ACCESSORS
    operator const void *() const;
        // Return a non-zero value if this manipulator is valid, and 0
        // otherwise.
};

// ============================================================================
//                        INLINE FUNCTION DEFINITIONS
// ============================================================================

                        // ------------------------
                        // class bael_LoggerManager
                        // ------------------------

// CLASS METHODS
inline
bool bael_LoggerManager::isInitialized()
{
    return (bael_LoggerManager *)0 != s_singleton_p;
}

                        // -----------------------------------
                        // class bael_LoggerManagerScopedGuard
                        // -----------------------------------

// CREATORS
inline
bael_LoggerManagerScopedGuard::bael_LoggerManagerScopedGuard(
                       bael_Observer                          *observer,
                       const bael_LoggerManagerConfiguration&  configuration,
                       bslma_Allocator                        *globalAllocator)
{
    bael_LoggerManager::initSingleton(observer,
                                      configuration,
                                      globalAllocator);
}

inline
bael_LoggerManagerScopedGuard::~bael_LoggerManagerScopedGuard()
{
    bael_LoggerManager::shutDownSingleton();
}

                        // -----------------------------
                        // class bael_LoggerCategoryIter
                        // -----------------------------

// CREATORS
inline
bael_LoggerCategoryIter::bael_LoggerCategoryIter(
                                       const bael_LoggerManager& loggerManager)
: d_iter(loggerManager.d_categoryManager)
{
}

inline
bael_LoggerCategoryIter::~bael_LoggerCategoryIter()
{
}

// MANIPULATORS
inline
void bael_LoggerCategoryIter::operator++()
{
    ++d_iter;
}

// ACCESSORS
inline
bael_LoggerCategoryIter::operator const void *() const
{
    return d_iter;
}

inline
const bael_Category& bael_LoggerCategoryIter::operator()() const
{
    return d_iter();
}

                        // ------------------------------
                        // class bael_LoggerCategoryManip
                        // ------------------------------

// CREATORS
inline
bael_LoggerCategoryManip::bael_LoggerCategoryManip(
                                             bael_LoggerManager *loggerManager)
: d_manip(&loggerManager->d_categoryManager)
{
}

inline
bael_LoggerCategoryManip::~bael_LoggerCategoryManip()
{
}

// MANIPULATORS
inline
void bael_LoggerCategoryManip::advance()
{
    d_manip.advance();
}

inline
bael_Category& bael_LoggerCategoryManip::operator()()
{
    return d_manip();
}

// ACCESSORS
inline
bael_LoggerCategoryManip::operator const void *() const
{
    return d_manip;
}

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2004
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
