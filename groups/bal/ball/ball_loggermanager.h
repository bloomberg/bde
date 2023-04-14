// ball_loggermanager.h                                               -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BALL_LOGGERMANAGER
#define INCLUDED_BALL_LOGGERMANAGER

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a manager of core logging functionality.
//
//@CLASSES:
//  ball::Logger: log record store and publication manager
//  ball::LoggerManager: logger factory and category administrator
//  ball::LoggerManagerScopedGuard: scoped guard for 'LoggerManager' singleton
//
//@SEE_ALSO: ball_record, ball_recordattributes, ball_observer, ball_context,
//           ball_loggermanagerdefaults, ball_loggermanagerconfiguration,
//           ball_severity, ball_transmission, ball_log
//
//@DESCRIPTION: This component provides the core of the 'ball' logging toolkit:
// the logger class itself, 'ball::Logger', that manages log record storage
// and publication control; the logger manager class, 'ball::LoggerManager',
// typically instantiated as a singleton, that is both a factory for loggers
// and a category manager; and the logger manager scoped guard,
// 'ball::LoggerManagerScopedGuard', that provides a convenient way to
// initialize and manage lifetime of the logger manager singleton object.
//
///General Features and Behavior
///-----------------------------
// The 'ball' logging toolkit is very flexible.  A user can log messages with
// very little effort, and with only a superficial understanding of logger
// operation, in which case the logger will exhibit its "default behavior".
// The user can also elect to customize many aspects of logging, such as
// storage and publication behavior, both at start-up and dynamically during
// program execution.  Naturally, to exercise such control, the user must
// become more familiar with 'ball' logger operation; the user can choose more
// convenience or more versatility, with a reasonably fine granularity.
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
// All loggers share a single internal broadcast observer to which log records
// are transmitted when they are published (see the component-level
// documentation of 'ball_observer' for more information on observers).
//
// A logger can achieve high performance through the use of an in-memory record
// buffer for storing the records logged by a program.  Each logger is
// constructed with a record manager that is an instance of a concrete class
// derived from 'ball::RecordBuffer'.  The singleton logger manager supplies a
// "default" record manager to the default logger; loggers allocated by the
// logger manager's 'allocateLogger' method use a record manager supplied by
// the client.  The default log record buffer is of user-configurable static
// size and is circular (see the 'ball_circularrecordbuffer' component for
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
// a 'ball::LoggerManagerScopedGuard' object in 'main' *before* creating any
// threads.  The logger manager scoped guard constructor takes a configuration
// object (an instance of 'ball::LoggerManagerConfiguration'), and an optional
// allocator.  The logger manager singleton is created as a side-effect of
// creating the scoped guard object.  When the guard object goes out of scope
// (i.e., on program exit), the logger manager singleton is automatically
// destroyed.
//
// The 'ball::LoggerManagerConfiguration' object is used to supply a set of
// user-defined "default" values and other options.  However, to obtain the
// "default" logging behavior, it is sufficient to instantiate a default
// 'ball::LoggerManagerConfiguration' object and pass that to the constructor
// of the scoped guard along with an observer.  (See {Usage} below.)
//
// As an alternative to using the scoped guard, the 'initSingleton' method that
// takes the same arguments as the scoped guard may be used to initialize the
// singleton.  However, in this case the 'shutDownSingleton' method must be
// explicitly called to destroy the logger manager singleton on program exit.
// Unless 'shutDownSingleton' is called, the singleton will not be destroyed
// and resources used by the singleton will leak.
//
// Note that the logger manager singleton *can* be reinitialized after it has
// been destroyed.  However, such practice should generally be restricted to
// test drivers and very specialized use cases.  Clients should generally avoid
// initializing and destroying the singleton more than once in a program unless
// they know what they are doing.
//
///Deprecation Notice
///------------------
// Direct use of any of the 'ball::LoggerManager' or
// 'ball::LoggerManagerScopedGuard' methods that take raw pointers to observers
// is *deprecated*.  These methods will be eliminated in a future release.
//
// The 'ball::LoggerManagerCategoryIter' and 'ball::LoggerManagerCategoryManip'
// classes are *deprecated*.  Clients of 'ball::LoggerManager' should use the
// 'visitCategories' accessor (the replacement for 'LoggerManagerCategoryIter')
// or 'visitCategories' manipulator (replacing 'LoggerManagerCategoryManip')
// instead.
//
///Categories, Severities, and Threshold Levels
///--------------------------------------------
// The logger supports the notions of "severity level" and "category"; every
// record is logged at some severity level and to some category.  Categories
// are user-defined (except for the "default category"), and have unique names.
// Severity levels are integers in the range '[0 .. 255]', and are most
// typically chosen from among the enumeration in the 'ball_severity'
// component, although use of the 'enum' is optional.  The severity level and
// the category name are each among the fixed fields of the record being logged
// (see "Log Record Contents" below).
//
// From the logger's perspective, all categories are peers; there is no special
// significance to any sequence of characters in a category name.  The user may
// impose a hierarchical *meaning* to category names, and the logger manager
// facilitates a certain degree of hierarchical *behavior* via several callback
// functors provided within this component (see below, and also the
// 'ball_loggerfunctorpayloads' component).  However, such hierarchy is not
// fundamental to categories, nor to the behavior described in this section.
// Similarly, there is no a priori significance to severity levels except that
// they are ordered and may be compared for inequality, although the enumerator
// names in the 'ball::Severity::Level' enumeration (e.g., 'DEBUG', 'WARN',
// 'ERROR', etc.) suggest the intended "standard" meanings.
//
// Every category has associated with it four "severity threshold levels" that
// may be set explicitly by the user on category creation/registration (via the
// 'addCategory' method) or else will default to specific values via one of
// several mechanisms described below (invoked by the one-argument
// 'setCategory' method).  Category threshold levels may also be changed during
// program execution via the five-argument 'setCategory' method.
//
// When the user logs a record to a given category and at a given severity (via
// the 'ball::Logger' 'logMessage' method or via the logging macros -- see the
// 'ball_log' component), the logger manager uses the specified severity and
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
//
//: Record:
//:     If the severity level of the record is at least as severe as the Record
//:     threshold level of the associated category, then the record will be
//:     stored by the logger in its log record buffer (i.e., it will be
//:     recorded).
//:
//: Pass:
//:     If the severity of the record is at least as severe as the Pass
//:     threshold level of the associated category, then the record will be
//:     immediately published by the logger (i.e., it will be transmitted to
//:     the logger's downstream recipient -- the observer).
//:
//: Trigger:
//:     If the severity of the record is at least as severe as the Trigger
//:     threshold level of the associated category, then the record will cause
//:     immediate publication of that record and any records in the logger's
//:     log record buffer (i.e., this record will trigger a general log record
//:     dump).
//:
//: Trigger-All:
//:     If the severity of the record is at least as severe as the Trigger-All
//:     threshold level of the associated category, then the record will cause
//:     immediate publication of that record and all other log records stored
//:     by *all* active loggers.
//
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
// defaults" to be the default values that the 'ball::LoggerManager' singleton
// is aware of at construction.  Depending on the values and options in the
// 'ball::LoggerManagerConfiguration' object provided to the logger manager on
// construction, the factory defaults may be either implementation-defined or
// user-defined.
//
// In either case, the user can *change* the default values during logger
// operation via the 'setDefaultThresholdLevels' method.  These threshold
// levels become the "default" values for new categories, but they do not
// affect the "factory defaults" that subsequently can be restored via the
// 'resetDefaultThresholdLevels' method.
//
// A third mechanism, the 'ball::LoggerManager::DefaultThresholdLevelsCallback'
// functor, adds even more flexibility.  If this callback is installed by the
// user at construction, or subsequently via the
// 'setDefaultThresholdLevelsCallback' method, the callback is the source of
// all default thresholds, and the above mechanisms are not used.  The next
// section covers category thresholds in more detail.
//
///Category Creation, Management, and Threshold Levels
///- - - - - - - - - - - - - - - - - - - - - - - - - -
// When the logger manager singleton is created, a unique category known as the
// *Default* *Category* is created, and is given "factory-supplied" default
// threshold levels.  The default values for the default category are each in
// the range '[0 .. 255]', but are otherwise unspecified.  The user can also
// specify default values explicitly when the logger manager singleton is
// constructed.  This is accomplished by constructing a
// 'ball::LoggerManagerDefaults' object, setting the desired values, and then
// setting that object as an attribute of the
// 'ball::LoggerManagerConfiguration' argument to the
// 'ball::LoggerManagerScopedGuard' constructor.
//
// The default category is issued to the user via the return value of the
// 'setCategory(const char *categoryName)' method whenever a new category
// cannot be created due to a capacity limitation on the category registry
// maintained by the logger manager.  The method's normal behavior is to return
// the category having 'categoryName'.
//
// Categories that are added to the registry during logging through calls to
// the 'setCategory(const char *)' method are given threshold levels by one of
// two means.  The "default" mechanism (a slightly overloaded term in 'ball')
// is to use the same default thresholds as described above for the default
// category.  The alternative is to specify a
// 'ball::LoggerManager::DefaultThresholdLevelsCallback' functor, either when
// the logger manager singleton is initialized or else afterwards via the
// 'setDefaultThresholdLevelsCallback' method.  This functor, if provided, is
// used by the logger manager to supply the four 'int' threshold values; the
// functor may generate these values by any means that the user sees fit.  See
// the 'ball_loggerfunctorpayloads' component for an example payload function
// for the functor.
//
// The default threshold levels can be adjusted ('setDefaultThresholdLevels')
// and reset to their original values ('resetDefaultThresholdLevels').  Note
// that if factory values are overridden at initialization, a reset will
// restore thresholds to the user-specified default values.  In addition, there
// is a method to set the threshold levels of a given category to the current
// default threshold levels ('setCategoryThresholdsToCurrentDefaults') or to
// the factory-supplied (or client-overridden) default values
// ('setCategoryThresholdsToFactoryDefaults').
//
// As a final note regarding categories, a client can optionally supply to the
// logger manager on construction a
// 'ball::LoggerManager::CategoryNameFilterCallback' functor (via the
// 'ball::LoggerManagerConfiguration' object) to translate category names from
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
// user-definable fields and attributes.  The following table lists the fixed
// fields in each log record (see the component-level documentation of
// 'ball_recordattributes' for more information on the fixed fields of a log
// record):
//..
//  Field Name    Type             Description
//  -----------   --------------   --------------------------------------
//  timestamp     bdlt::Datetime   creation date and time
//  process ID    int              process ID of creator
//  thread ID     int              thread ID of creator
//  filename      string           file where created  (i.e., '__FILE__')
//  line number   int              line number in file (i.e., '__LINE__')
//  category      string           category name
//  severity      int              severity of logged record
//  message       string           log message text
//..
//
// The following table lists optional fields and attributes in each log record
// (see the component-level documentation of 'ball_userfields' and
// 'ball_managedattribute' for more information):
//..
//  Field Name    Type                                Description
//  -----------   --------------                      -------------------------
//  userFields    ball::UserFields                    [!DEPRECATED!]
//  attributes    bsl::vector<ball::ManagedAttribute> user-managed log
//                                                    attributes
//..
//
// [!DEPRECATED!] If a 'ball::LoggerManager::UserFieldsPopulatorCallback'
// functor is supplied by the client (see 'ball_loggermanagerconfiguration'),
// thereafter, every logged record has its user-defined fields (indirectly)
// populated by an invocation of the 'UserFieldsPopulatorCallback' functor.
//
// The log record's attributes are populated by attribute collector functor(s)
// registered by the user.
//
///Multi-Threaded Usage
///--------------------
// The 'ball' logging toolkit may be used in single-threaded and multi-threaded
// library code and applications with equal ease, and with virtually no
// difference in coding.  In particular, the same use of the
// 'ball::LoggerManagerScopedGuard' class to initialize the logger manager
// singleton is required in 'main' in both cases, and individual calls to the
// 'ball::Logger' instance method 'logMessage' (and logging calls via the
// logging macros -- see 'ball_log') are identical, from the user's
// perspective.  Category threshold administration is also identical in both
// cases.
//
// Differences in logger usage, or, more precisely, additional options for the
// multi-threaded user, arise when the user wishes to allocate one or more
// loggers beyond the default logger that is owned by the singleton logger
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
///'bsls::Log' Logging Redirection
///-------------------------------
// The 'ball::LoggerManager' singleton, on construction, redirects 'bsls::Log'
// messages to 'ball'.  Such messages use the logging category "BSLS.LOG".
// Upon its destruction, the logger manager singleton redirects 'bsls::Log'
// messages back to the 'bsls::Log' message handler that was in effect prior to
// the creation of the singleton (see 'bsls_log').
//
///Usage
///-----
// This section illustrates instantiation of the logger manager singleton that
// is required in 'main', and also shows *direct* use of the logger and logger
// manager interfaces, much of which is actually *not* recommended.  The most
// basic logger functionality has been wrapped in macros defined in the
// 'ball_log' component.  See the 'ball' package-level documentation and the
// 'ball_log' component documentation for recommended real-world usage
// examples.
//
///Example 1: Initialization #1
/// - - - - - - - - - - - - - -
// Clients that perform logging must first instantiate the singleton logger
// manager using the 'ball::LoggerManagerScopedGuard' class.  This example
// shows how to create a logger manager with the most basic "default behavior".
// Subsequent examples will show more customized behavior.
//
// The following snippets of code illustrate the initialization sequence
// (typically performed near the top of 'main').
//
// First, we create a 'ball::LoggerManagerConfiguration' object,
// 'configuration', and set the logging "pass-through" level -- the level at
// which log records are published to registered observers -- to 'WARN' (see
// {'Categories, Severities, and Threshold Levels'}):
//..
//  // myApp.cpp
//
//  int main()
//  {
//      ball::LoggerManagerConfiguration configuration;
//      configuration.setDefaultThresholdLevelsIfValid(ball::Severity::e_WARN);
//..
// Next, create a 'ball::LoggerManagerScopedGuard' object whose constructor
// takes the configuration object just created.  The guard will initialize the
// logger manager singleton on creation and destroy the singleton upon
// destruction.  This guarantees that any resources used by the logger manager
// will be properly released when they are not needed:
//..
//      ball::LoggerManagerScopedGuard guard(configuration);
//..
// Note that the application is now prepared to log messages using the 'ball'
// logging subsystem, but until the application registers an observer, all log
// messages will be discarded.
//
// Finally, we create a 'ball::StreamObserver' object 'observer' that will
// publish records to 'stdout' and register it with the logger manager
// singleton.  Note that observers must be registered by name; this example
// simply uses "default" for a name:
//..
//      bslma::Allocator *alloc = bslma::Default::globalAllocator(0);
//
//      bsl::shared_ptr<ball::StreamObserver> observer(
//                                new(*alloc) ball::StreamObserver(&bsl::cout),
//                                alloc);
//      ball::LoggerManager::singleton().registerObserver(observer, "default");
//..
// The application is now prepared to log messages using the 'ball' logging
// subsystem:
//..
//        // ...
//
//        return 0;
//    }
//..
// Note that concrete observers that can be configured after their creation
// (e.g., as to whether log records are published in UTC or local time)
// generally can have their configuration adjusted at any time, either before
// or after being registered with a logger manager.  For an example of such an
// observer, see 'ball_asyncfileobserver'.
//
///Example 2: Initialization #2
/// - - - - - - - - - - - - - -
// In this example, we demonstrate a more elaborate initial configuration for
// the logger manager.  In particular, we create the singleton logger manager
// with a configuration that has a category name filter functor, a
// 'DefaultThresholdLevelsCallback' functor, and user-chosen values for the
// "factory default" threshold levels.
//
// First, we define three 'static' functions that are employed by the two
// functors.  The 'toLower' function implements our category name filter.  It
// is wrapped within a functor object and maps category names to lower-case:
//..
//  static
//  void toLower(bsl::string *buffer, const char *s)
//  {
//      assert(buffer);
//      assert(s);
//
//      buffer->clear();
//      while (*s) {
//          buffer->push_back(static_cast<char>(bsl::tolower(*s)));
//          ++s;
//      }
//      buffer->push_back(0);
//  }
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
//  static
//  int getDefaultThresholdLevels(int                        *recordLevel,
//                                int                        *passLevel,
//                                int                        *triggerLevel,
//                                int                        *triggerAllLevel,
//                                char                        delimiter,
//                                const ball::LoggerManager&  loggerManager,
//                                const char                 *categoryName)
//      // Obtain appropriate threshold levels for the category having the
//      // specified 'categoryName' by searching the registry of the specified
//      // 'loggerManager', and store the resulting values at the specified
//      // 'recordLevel', 'passLevel', 'triggerLevel', and 'triggerAllLevel'
//      // addresses.  A hierarchical category naming scheme is assumed that
//      // employs the specified 'delimiter' to separate the components of
//      // category names.  Return 0 on success, and a non-zero value
//      // otherwise.  The behavior is undefined unless 'recordLevel',
//      // 'passLevel', 'triggerLevel', and 'triggerAllLevel' are non-null, and
//      // 'categoryName' is null-terminated.
//  {
//      assert(recordLevel);
//      assert(passLevel);
//      assert(triggerLevel);
//      assert(triggerAllLevel);
//      assert(categoryName);
//
//      enum { SUCCESS = 0, FAILURE = -1 };
//
//      bsl::string buffer(categoryName);
//      while (1) {
//          const ball::Category *category =
//              loggerManager.lookupCategory(buffer.c_str());
//          if (0 != category) {
//              *recordLevel     = category->recordLevel();
//              *passLevel       = category->passLevel();
//              *triggerLevel    = category->triggerLevel();
//              *triggerAllLevel = category->triggerAllLevel();
//              return SUCCESS;                                       // RETURN
//          }
//
//          const char *newEnd = bsl::strrchr(buffer.c_str(), delimiter);
//          if (0 == newEnd) {
//              return FAILURE;                                       // RETURN
//          }
//          buffer.resize(newEnd - buffer.data());
//      }
//  }
//
//  static
//  void inheritThresholdLevels(int        *recordLevel,
//                              int        *passLevel,
//                              int        *triggerLevel,
//                              int        *triggerAllLevel,
//                              const char *categoryName)
//      // Obtain appropriate threshold levels for the category having the
//      // specified 'categoryName', and store the resulting values at the
//      // specified 'recordLevel', 'passLevel', 'triggerLevel', and
//      // 'triggerAllLevel' addresses.  The behavior is undefined unless
//      // 'recordLevel', 'passLevel', 'triggerLevel', and 'triggerAllLevel'
//      // are non-null, and 'categoryName' is null-terminated.
//  {
//      assert(recordLevel);
//      assert(passLevel);
//      assert(triggerLevel);
//      assert(triggerAllLevel);
//      assert(categoryName);
//
//      const ball::LoggerManager& manager = ball::LoggerManager::singleton();
//      if (0 != getDefaultThresholdLevels(recordLevel,
//                                         passLevel,
//                                         triggerLevel,
//                                         triggerAllLevel,
//                                         '.',
//                                         manager,
//                                         categoryName)) {
//          *recordLevel     = manager.defaultRecordThresholdLevel();
//          *passLevel       = manager.defaultPassThresholdLevel();
//          *triggerLevel    = manager.defaultTriggerThresholdLevel();
//          *triggerAllLevel = manager.defaultTriggerAllThresholdLevel();
//      }
//  }
//..
// Then, we create the callback functors that will be supplied to the logger
// manager singleton initialization (as in "Example 1" above, we assume that
// the initialization sequence occurs somewhere near the top of 'main'):
//..
//  // myApp2.cpp
//
//  int main() {
//      // ...
//
//      ball::LoggerManager::CategoryNameFilterCallback nameFilter(&toLower);
//
//      ball::LoggerManager::DefaultThresholdLevelsCallback
//                                 thresholdsCallback(&inheritThresholdLevels);
//..
// Next, we define four values for our custom "factory default" thresholds.
// These values will be stored within the logger manager and will be available
// to all users whenever the "factory defaults" are needed, for the life of the
// logger manager.  In this example, however, we will also be installing the
// 'thresholdsCallback' defined above, so unless that functor is un-installed
// (by a call to 'setDefaultThresholdLevelsCallback'), these four "factory
// defaults" will have no practical effect, since the callback mechanism "steps
// in front of" the default values:
//..
//      int recordLevel     = 125;
//      int passLevel       = 100;
//      int triggerLevel    =  75;
//      int triggerAllLevel =  50;
//..
// Then, we can configure a 'ball::LoggerManagerDefaults' object, 'defaults',
// with these four threshold values.  'defaults' can then be used to configure
// the 'ball::LoggerManagerConfiguration' object that will be passed to the
// 'ball::LoggerManagerScopedGuard' constructor (below):
//..
//      ball::LoggerManagerDefaults defaults;
//      defaults.setDefaultThresholdLevelsIfValid(recordLevel,
//                                                passLevel,
//                                                triggerLevel,
//                                                triggerAllLevel);
//..
// Next, we create and set the 'ball::LoggerManagerConfiguration' object,
// 'configuration', that will describe our desired configuration:
//..
//      ball::LoggerManagerConfiguration configuration;
//      configuration.setDefaultValues(defaults);
//      configuration.setCategoryNameFilterCallback(nameFilter);
//      configuration.setDefaultThresholdLevelsCallback(thresholdsCallback);
//..
// Then, we instantiate the singleton logger manager, passing in the
// 'configuration' that we have just created:
//..
//      ball::LoggerManagerScopedGuard guard(configuration);
//..
// Note that the application is now prepared to log messages using the 'ball'
// logging subsystem, but until the application registers an observer, all log
// messages will be discarded.
//
// Now, we will demonstrate the functors and client-supplied default threshold
// overrides.
//
// First, we obtain a reference to the singleton logger manager:
//..
//      ball::LoggerManager& manager = ball::LoggerManager::singleton();
//..
// Then, we obtain a reference to the *Default* *Category* and 'assert' that
// its threshold levels match the client-supplied values that override the
// "factory-supplied" default values:
//..
//      const ball::Category& defaultCategory = manager.defaultCategory();
//      assert(125 == defaultCategory.recordLevel());
//      assert(100 == defaultCategory.passLevel());
//      assert( 75 == defaultCategory.triggerLevel());
//      assert( 50 == defaultCategory.triggerAllLevel());
//..
// Next, we add a category named "BloombergLP" (by calling 'addCategory').
// Note that threshold levels supplied with the category override all defaults
// (including thresholds set by the supplied callback).  Also note that the
// logger manager invokes the supplied category name filter to map the category
// name to lower-case before the new category is added to the category
// registry.  The name filter is also invoked by 'lookupCategory' whenever a
// category is searched for (i.e., by name) in the registry:
//..
//      const ball::Category *blpCategory =
//                         manager.addCategory("BloombergLP", 128, 96, 64, 32);
//      assert(blpCategory == manager.lookupCategory("BLOOMBERGLP"));
//      assert(  0 == bsl::strcmp("bloomberglp", blpCategory->categoryName()));
//      assert(128 == blpCategory->recordLevel());
//      assert( 96 == blpCategory->passLevel());
//      assert( 64 == blpCategory->triggerLevel());
//      assert( 32 == blpCategory->triggerAllLevel());
//..
// Then, we add a second category named "BloombergLP.bal.ball" (by calling
// 'setCategory') and 'assert' that the threshold levels are "inherited" from
// category "BloombergLP":
//..
//      const ball::Category *ballCategory =
//                                 manager.setCategory("BLOOMbergLP.bal.ball");
//
//      assert(ballCategory == manager.lookupCategory("bloomberglp.bal.ball"));
//      assert(  0 == bsl::strcmp("bloomberglp.bal.ball",
//                                ballCategory->categoryName()));
//      assert(128 == ballCategory->recordLevel());
//      assert( 96 == ballCategory->passLevel());
//      assert( 64 == ballCategory->triggerLevel());
//      assert( 32 == ballCategory->triggerAllLevel());
//..
// Now, we add a third category named "Other.equities", again by calling
// 'setCategory'.  This category has no ancestor currently in the registry, so
// its threshold levels match those of the *Default* *Category*:
//..
//      const ball::Category *equitiesCategory =
//                                       manager.setCategory("Other.equities");
//      assert(equitiesCategory == manager.lookupCategory("OTHER.EQUITIES"));
//      assert(  0 == bsl::strcmp("other.equities",
//                                equitiesCategory->categoryName()));
//      assert(125 == equitiesCategory->recordLevel());
//      assert(100 == equitiesCategory->passLevel());
//      assert( 75 == equitiesCategory->triggerLevel());
//      assert( 50 == equitiesCategory->triggerAllLevel());
//..
// Finally, we create a 'ball::StreamObserver' object 'observer' that will
// publish records to 'stdout' and register it with the logger manager
// singleton.  Note that observers must be registered by name; this example
// simply uses "default" for a name:
//..
//      bslma::Allocator *alloc =  bslma::Default::globalAllocator(0);
//
//      bsl::shared_ptr<ball::StreamObserver> observer(
//                                new(*alloc) ball::StreamObserver(&bsl::cout),
//                                alloc);
//
//      manager.registerObserver(observer, "default");
//      // ...
//
//      return 0;
//  }
//..
//
///Example 3: Efficient Logging of 'ostream'-able Objects
/// - - - - - - - - - - - - - - - - - - - - - - - - - - -
// The following example demonstrates how instances of a class supporting
// streaming to 'bsl::ostream' (via overloaded 'operator<<') can be logged.  It
// also demonstrates how to use the 'logMessage' method to log messages to a
// logger.  Suppose we want to *efficiently* log instances of the following
// class:
//..
//  class Information {
//      // This (incomplete) class is a simple aggregate of a "heading" and
//      // "contents" pertaining to that heading.  It serves to illustrate how
//      // to log the string representation of an object.
//
//      bsl::string d_heading;
//      bsl::string d_contents;
//
//    public:
//      Information(const char *heading, const char *contents);
//      ~Information();
//      const bsl::string& heading() const;
//      const bsl::string& contents() const;
//  };
//..
// In addition, we define the following free operator for streaming instances
// of 'Information' to an 'bsl::ostream':
//..
//  bsl::ostream& operator<<(bsl::ostream&      stream,
//                           const Information& information)
//  {
//      stream << information.heading();
//      stream << ": ";
//      stream << information.contents() << bsl::endl;
//      return stream;
//  }
//..
// The following function logs an 'Information' object to the specified
// 'logger':
//..
//  void logInformation(ball::Logger          *logger,
//                      const Information&     information,
//                      ball::Severity::Level  severity,
//                      const ball::Category&  category,
//                      const char            *fileName,
//                      int                    lineNumber)
//  {
//..
// First, obtain a record that has its 'fileName' and 'lineNumber' attributes
// set:
//..
//      ball::Record *record = logger->getRecord(fileName, lineNumber);
//..
// Then, we get a non-'const' reference to the fixed fields of 'record':
//..
//      ball::RecordAttributes& attributes = record->fixedFields();
//..
// Next, we create a 'bsl::ostream' to which the string representation
// 'information' can be output.  Note that 'stream' is supplied with the stream
// buffer of 'attributes':
//..
//      bsl::ostream stream(&attributes.messageStreamBuf());
//..
// Then, we stream 'information' into our output 'stream'.  This will set the
// message attribute of 'record' to the streamed data:
//..
//      stream << information;
//..
// Finally, we log 'record' using 'logger':
//..
//      logger->logMessage(category, severity, record);
//  }
//..
// Notice that we did not need to allocate a scratch buffer to stream the
// object contents into.  That would have required an extra copy and the cost
// of allocation and deallocation, and thus would have been more inefficient.
//
///Example 4: Logging using a 'ball::Logger'
///- - - - - - - - - - - - - - - - - - - - -
// This example demonstrates using a 'ball::Logger' directly to log messages.
// In practice, clients are encouraged to use the logging macros (see
// {'ball_log'}.  The following example assumes logging has been correctly
// initialized (see prior examples).
//
// The following simple 'factorial' function takes and returns values of type
// 'int'.  Note that this function has a very limited range of input, namely
// integers in the range '[0 .. 13]'.  This limited range serves to illustrate
// a usage pattern of the logger, namely to log "warnings" whenever a key
// function is given bad input.
//
// For this example, it is sufficient to use the severity levels defined in the
// 'ball::Severity::Level' enumeration:
//..
//  enum Level {
//      OFF   =   0,  // disable generation of corresponding message
//      FATAL =  32,  // a condition that will (likely) cause a *crash*
//      ERROR =  64,  // a condition that *will* cause incorrect behavior
//      WARN  =  96,  // a *potentially* problematic condition
//      INFO  = 128,  // data about the running process
//      DEBUG = 160,  // information useful while debugging
//      TRACE = 192   // execution trace data
//  };
//..
// Note that the intervals left between enumerator values allow applications
// to define additional values in case there is a desire to log with more
// finely-graduated levels of severity.  We will not need that granularity
// here; 'ball::Severity::e_WARN' is appropriate to log a warning message if
// the input argument to our factorial function is not in this range of values.
//
// We will register a unique category for this function, so that logged
// messages from our function will be identified in the published output.
// Also, with a unique category name, the logging behavior of this function can
// be administered by resetting the various threshold levels for the category.
// In this example, we will accept the default thresholds.
//
// The 'setCategory' method accepts a name and returns the address of a
// 'ball::Category' with that name or, in some circumstances, the address of
// the *Default* *Category* (see the function-level documentation of
// 'setCategory' for details).  The address returned by 'setCategory' is stored
// in a function-static pointer variable (i.e., it is fetched only once upon
// first use).  In this example, we assume that we are writing a function for
// Equities Graphics that will live in that group's Math library.  The dot
// "delimiters" ('.') have no particular significance to the logger, but may be
// used by the administration methods to "induce" a hierarchical behavior on
// our category, should that be useful.  See, e.g., the callback functor
// 'ball::LoggerManager::DefaultThresholdLevelsCallback' and its documentation,
// and Usage Example 2 above for information on how to use category names to
// customize logger behavior:
//..
//  int factorial(int n)
//      // Return the factorial of the specified value 'n' if the factorial
//      // can be represented as an 'int', and a negative value otherwise.
//  {
//      static const ball::Category *factorialCategory =
//          ball::LoggerManager::singleton().setCategory(
//                                          "equities.graphics.math.factorial",
//                                          ball::Severity::e_INFO,
//                                          ball::Severity::e_TRACE,
//                                          ball::Severity::e_ERROR,
//                                          ball::Severity::e_FATAL);
//..
// We must also obtain a reference to a logger by calling the logger manager
// 'getLogger' method.  Note that this logger may not safely be cached as a
// function 'static' variable since our function may be called in different
// threads having different loggers.  Even in a single-threaded program, the
// owner of 'main' is free to install new loggers at any point, so a
// statically-cached logger would be a problem:
//..
//      ball::Logger& logger = ball::LoggerManager::singleton().getLogger();
//..
// Now we validate the input value 'n'.  If 'n' is either negative or too
// large, we will log a warning message (at severity level
// 'ball::Severity::e_WARN') and return a negative value.  Note that calls to
// 'logMessage' have no run-time overhead (beyond the execution of a simple
// 'if' test) unless 'ball::Severity::e_WARN' is at least as severe as one of
// the threshold levels of 'factorialCategory':
//..
//      if (0 > n) {
//          logger.logMessage(*factorialCategory,
//                            ball::Severity::e_WARN,
//                            __FILE__,
//                            __LINE__,
//                            "Attempt to take factorial of negative value.");
//          return n;                                                 // RETURN
//      }
//
//      enum { MAX_ARGUMENT = 13 };  // maximum value accepted by 'factorial'
//
//      if (MAX_ARGUMENT < n) {
//          logger.logMessage(*factorialCategory,
//                            ball::Severity::e_WARN,
//                            __FILE__,
//                            __LINE__,
//                            "Result too large for 'int'.");
//          return -n;                                                // RETURN
//      }
//..
// The remaining code proceeds mostly as expected, but adds one last message
// that tracks control flow when 'ball::Severity::e_TRACE' is at least as
// severe as one of the threshold levels of 'factorialCategory' (e.g., as
// might be the case during debugging):
//..
//      int product = 1;
//      while (1 < n) {
//          product *= n;
//          --n;
//      }
//
//      logger.logMessage(*factorialCategory,
//                        ball::Severity::e_TRACE,
//                        __FILE__,
//                        __LINE__,
//                        "Exiting 'factorial' successfully.");
//
//      return product;
//  }
//..

#include <balscm_version.h>

#include <ball_attribute.h>
#include <ball_attributecollectorregistry.h>
#include <ball_broadcastobserver.h>
#include <ball_categorymanager.h>
#include <ball_loggermanagerconfiguration.h>
#include <ball_record.h>
#include <ball_recordbuffer.h>
#include <ball_thresholdaggregate.h>
#include <ball_transmission.h>

#include <bdlcc_objectpool.h>
#include <bdlcc_sharedobjectpool.h>

#include <bdlma_concurrentpool.h>

#include <bslma_allocator.h>
#include <bslma_managedptr.h>

#include <bslmf_util.h>    // 'forward(V)'

#include <bslmt_mutex.h>
#include <bslmt_readerwritermutex.h>

#include <bsls_compilerfeatures.h>
#include <bsls_util.h>     // 'forward<T>(V)'

#include <bsl_functional.h>
#include <bsl_map.h>
#include <bsl_memory.h>
#include <bsl_set.h>
#include <bsl_string_view.h>

namespace BloombergLP {
namespace ball {

class LoggerManager;
class Observer;
class RecordBuffer;

                           // ============
                           // class Logger
                           // ============

class Logger {
    // This class provides log record management services.  Each instance of
    // 'Logger' receives log records from one or more clients, manages the
    // storage of those records, and transmits them to a registered recipient
    // (i.e., an observer) when appropriate.

  public:
    // TYPES
    typedef LoggerManagerConfiguration::UserFieldsPopulatorCallback
                                                   UserFieldsPopulatorCallback;
        // 'UserFieldsPopulatorCallback' is the type of a user-supplied
        // callback functor used to populate the user-defined fields in each
        // log record.

    typedef bsl::function<void(Transmission::Cause)> PublishAllTriggerCallback;
        // 'PublishAllTriggerCallback' is the type of the functor that is
        // invoked with the publication cause to publish all record buffers of
        // all loggers that are allocated by the logger manager.

  private:
    // DATA
    bdlcc::SharedObjectPool<Record,
                            bdlcc::ObjectPoolFunctors::DefaultCreator,
                            bdlcc::ObjectPoolFunctors::Clear<Record> >
                  d_recordPool;                 // pool of records with a
                                                // custom 'RESETTER'

    const bsl::shared_ptr<Observer>
                  d_observer;                   // holds observer

    RecordBuffer *d_recordBuffer_p;             // holds log record buffer
                                                // (not owned)

    UserFieldsPopulatorCallback
                  d_userFieldsPopulator;        // user fields populator
                                                // functor

    const AttributeCollectorRegistry
                 *d_attributeCollectors_p;      // pointer to the registry of
                                                // attribute collector
                                                // callbacks (not owned)

    PublishAllTriggerCallback
                  d_publishAll;                 // publishAll callback functor

    bdlma::ConcurrentPool
                  d_bufferPool;                 // pool of buffers for
                                                // formatting log messages
                                                // allowing recursive access

    char         *d_scratchBuffer_p;            // buffer for formatting log
                                                // messages (owned)

    bslmt::Mutex  d_scratchBufferMutex;         // ensure thread-safety of
                                                // message buffer

    int           d_scratchBufferSize;          // message buffer size (bytes)

    LoggerManagerConfiguration::LogOrder
                  d_logOrder;                   // logging order

    LoggerManagerConfiguration::TriggerMarkers
                  d_triggerMarkers;             // trigger markers

    bslma::Allocator
                 *d_allocator_p;                // memory allocator (held, not
                                                // owned)

    // FRIENDS
    friend class LoggerManager;

  private:
    // NOT IMPLEMENTED
    Logger(const Logger&);
    Logger& operator=(const Logger&);

    // PRIVATE CREATORS
    Logger(const bsl::shared_ptr<Observer>&            observer,
           RecordBuffer                               *recordBuffer,
           const UserFieldsPopulatorCallback&          userFieldsPopulator,
           const AttributeCollectorRegistry           *attributeCollectors,
           const PublishAllTriggerCallback&            publishAllCallback,
           int                                         scratchBufferSize,
           LoggerManagerConfiguration::LogOrder        logOrder,
           LoggerManagerConfiguration::TriggerMarkers  triggerMarkers,
           bslma::Allocator                           *globalAllocator);
        // Create a logger having the specified 'observer' that receives
        // published log records, the specified 'recordBuffer' that stores log
        // records, the specified 'userFieldsPopulator' that populates the
        // user-defined fields of log records, the specified
        // 'attributeCollectors' registry of user-installed attribute
        // collectors, the specified 'publishAllCallback' that is invoked when
        // a Trigger-All event occurs, the specified 'scratchBufferSize' for
        // the internal message buffer accessible via 'obtainMessageBuffer',
        // and the specified 'globalAllocator' used to supply memory.  On a
        // Trigger or Trigger-All event, the messages are published in the
        // specified 'logOrder'.  Note that this constructor is 'private' since
        // the creation of instances of 'Logger' is managed by its 'friend'
        // 'LoggerManager'.

    ~Logger();
        // Destroy this logger.

    // PRIVATE MANIPULATORS
    bsl::shared_ptr<Record> getRecordPtr(const char *fileName, int lineNumber);
        // Return a shared pointer to a modifiable record having the specified
        // 'fileName' and 'lineNumber' attributes, and retrieved from the
        // shared object pool managed by this logger.

    void logMessage(const Category&                category,
                    int                            severity,
                    const bsl::shared_ptr<Record>& record,
                    const ThresholdAggregate&      levels);
        // Log the specified 'record' after setting its category field to the
        // specified 'category', severity field to the specified 'severity',
        // and the rest of the fixed fields (except 'fileName', 'lineNumber',
        // and 'message', all of which are assumed to be already set in
        // 'record') based on the threshold levels of the specified 'levels'.
        // (See the component-level documentation of 'ball_record' for more
        // information on the fields that are logged.)  Store the record in the
        // buffer held by this logger if 'severity' is at least as severe as
        // the "Record" threshold level of 'levels'.  Pass the record directly
        // to the observers registered with this logger if 'severity' is at
        // least as severe as the "Pass" threshold level of 'levels'.  Publish
        // the entire contents of the buffer of this logger if 'severity' is at
        // least as severe as the "Trigger" threshold level of 'levels'.
        // Publish the entire contents of all buffers of all active loggers if
        // 'severity' is at least as severe as the "Trigger-All" threshold
        // level of 'levels' (i.e., via the callback supplied at construction).
        // Finally, dispose of 'record'.  This method has no effect (other than
        // disposing of 'record') if 'severity' is less severe than each of the
        // threshold levels of 'levels'.  The behavior is undefined unless
        // 'severity' is in the range '[1 .. 255]' and 'record' was previously
        // obtained via a call to 'getRecord'.  Note that 'record' will be
        // invalid after this method returns.

    void publish(Transmission::Cause cause);
        // Publish to the observer held by this logger all records stored in
        // the record buffer of this logger and indicate to the observer the
        // specified publication 'cause'.

  public:
    // MANIPULATORS
    Record *getRecord(const char *fileName, int lineNumber);
        // Return the address of a modifiable record having the specified
        // 'fileName' and 'lineNumber' attributes, and retrieved from the
        // object pool managed by this logger.  Note that the returned 'Record'
        // must subsequently be supplied to a call to the 3-argument
        // 'logMessage' method on this logger.

    void logMessage(const Category&  category,
                    int              severity,
                    const char      *fileName,
                    int              lineNumber,
                    const char      *message);
        // Log a record containing the specified 'message' text, 'fileName',
        // 'lineNumber', 'severity', and the name of the specified 'category'.
        // (See the component-level documentation of 'ball_record' for more
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

    void logMessage(const Category&  category,
                    int              severity,
                    Record          *record);
        // Log the specified '*record' after setting its category attribute to
        // the name of the specified 'category' and severity attribute to the
        // specified 'severity'.  (See the component-level documentation of
        // 'ball_record' for more information on the fields that are logged.)
        // Store the record in the buffer held by this logger if 'severity' is
        // at least as severe as the current "Record" threshold level of
        // 'category'.  Pass the record directly to the observer held by this
        // logger if 'severity' is at least as severe as the current "Pass"
        // threshold level of 'category'.  Publish the entire contents of the
        // buffer of this logger if 'severity' is at least as severe as the
        // current "Trigger" threshold level of 'category'.  Publish the entire
        // contents of all buffers of all active loggers if 'severity' is at
        // least as severe as the current "Trigger-All" threshold level of
        // 'category' (i.e., via the callback supplied at construction).
        // Finally, dispose of 'record'.  This method has no effect (other than
        // disposing of 'record') if 'severity' is less severe than each of the
        // threshold levels of 'category'.  The behavior is undefined unless
        // 'severity' is in the range '[1 .. 255]', both 'fileName' and
        // 'message' are null-terminated, and 'record' was previously obtained
        // by a call to 'getRecord' on this logger.  Note that 'record' will be
        // invalid after this method returns.

#ifndef BDE_OMIT_INTERNAL_DEPRECATED
    char *messageBuffer();
        // Return the address of the modifiable message buffer managed by this
        // logger.  Note that the returned buffer is intended to be used *only*
        // for formatting log messages immediately before calling 'logMessage'.
        //
        // !DEPRECATED!: Use 'obtainMessageBuffer' instead.  Do *not* use this
        // method in multi-threaded code.
#endif // BDE_OMIT_INTERNAL_DEPRECATED

    char *obtainMessageBuffer(bslmt::Mutex **mutex, int *bufferSize);
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

    bslma::ManagedPtr<char> obtainMessageBuffer(int *bufferSize);
        // Return a managed pointer that refers to the memory block to which
        // this thread of execution has exclusive access and load the size (in
        // bytes) of this buffer into the specified 'bufferSize' address.  Note
        // that this method is intended for *internal* *use* only.

    void publish();
        // Publish to the observer held by this logger all records stored in
        // the record buffer of this logger and indicate to the observer that
        // the cause is 'MANUAL_PUBLISH'.

    void removeAll();
        // Remove all log records from the record buffer of this logger.

    // ACCESSORS
    int messageBufferSize() const;
        // Return the size, in bytes, of the message buffer managed by this
        // logger.

    int numRecordsInUse() const;
        // Return a *snapshot* of number of records that have been dispensed by
        // 'getRecord' but have not yet been supplied (returned) using
        // 'logRecord'.
};

#ifndef BDE_OMIT_INTERNAL_DEPRECATED
class LoggerManagerCategoryIter;
class LoggerManagerCategoryManip;
#endif  // BDE_OMIT_INTERNAL_DEPRECATED

                           // ===================
                           // class LoggerManager
                           // ===================

class LoggerManager {
    // This class is *usually* a singleton.  It provides a factory for 'Logger'
    // objects and is also a wrapper for category administration services.
    // Note that some services provided by this class are available only after
    // the singleton has been initialized.

  public:
    // TYPES
    typedef LoggerManagerConfiguration::CategoryNameFilterCallback
                                                    CategoryNameFilterCallback;
        // 'CategoryNameFilterCallback' is the type of the user-supplied
        // functor that translates external category names to internal names.

    typedef LoggerManagerConfiguration::DefaultThresholdLevelsCallback
                                                DefaultThresholdLevelsCallback;
        // 'DefaultThresholdLevelsCallback' is the type of the functor that
        // determines default threshold levels for categories added to the
        // registry by the 'setCategory(const char *)' method.

    typedef Logger::PublishAllTriggerCallback PublishAllTriggerCallback;
        // 'PublishAllTriggerCallback' is the type of the functor that is
        // invoked to publish all record buffers of all active loggers (i.e.,
        // loggers allocated by the logger manager that have not yet been
        // deallocated).

    typedef LoggerManagerConfiguration::UserFieldsPopulatorCallback
                                                   UserFieldsPopulatorCallback;
        // 'UserFieldsPopulatorCallback' is the type of a user-supplied
        // callback functor used to populate the user-defined fields in each
        // log record.

    typedef AttributeCollectorRegistry::Collector AttributeCollector;
        // 'AttributeCollector' is the type of a user-supplied functor used to
        // visit a collection of attributes.

    typedef AttributeCollectorRegistry::Visitor AttributeVisitor;
        // 'AttributeVisitor' is the type of a user-supplied functor invoked
        // by an attribute collector for every attribute.

    typedef BroadcastObserver::ObserverRegistry ObserverRegistry;
        // This 'typedef' is an alias for the type of the internal broadcast
        // observer registry.

  private:
    // NOT IMPLEMENTED
    LoggerManager(const LoggerManager&);
    LoggerManager& operator=(const LoggerManager&);

#ifndef BDE_OMIT_INTERNAL_DEPRECATED
    // FRIENDS
    friend class LoggerManagerCategoryIter;
    friend class LoggerManagerCategoryManip;
#endif  // BDE_OMIT_INTERNAL_DEPRECATED

    // CLASS DATA
    static LoggerManager  *s_singleton_p;        // address of singleton if
                                                 // initialized; 0 otherwise

    static bool            s_isSingletonOwned;   // 'true' by default whereby
                                                 // 'ball' owns the singleton
                                                 // and destroys it in
                                                 // 'shutDownSingleton'; can be
                                                 // set to 'false' by the
                                                 // Windows-specific
                                                 // 'initSingleton' taking an
                                                 // 'adoptSingleton' flag

    // DATA
    const bsl::shared_ptr<BroadcastObserver>
                           d_observer;           // internal broadcast
                                                 // observer

    CategoryNameFilterCallback
                           d_nameFilter;         // category name filter
                                                 // functor

    DefaultThresholdLevelsCallback
                           d_defaultThresholds;  // functor for obtaining
                                                 // default threshold levels of
                                                 // "set" categories

    mutable bslmt::ReaderWriterMutex
                           d_defaultThresholdsLock;
                                                 // 'd_defaultThresholdsLock'
                                                 // protector

    ThresholdAggregate     d_defaultThresholdLevels;
                                                 // default threshold levels

    const ThresholdAggregate
                           d_factoryThresholdLevels;
                                                 // factory default threshold
                                                 // levels

    UserFieldsPopulatorCallback
                           d_userFieldsPopulator;
                                                 // user fields populator
                                                 // functor

    ball::AttributeCollectorRegistry
                           d_attributeCollectors;// Registered attribute
                                                 // collector callbacks

    Logger                *d_logger_p;           // holds default logger
                                                 // (owned)

    CategoryManager        d_categoryManager;    // category manager

    unsigned int           d_maxNumCategoriesMinusOne;
                                                 // one less than the current
                                                 // capacity of the registry

    bsl::set<Logger *>     d_loggers;            // set of *allocated* loggers

    bslmt::ReaderWriterMutex
                           d_loggersLock;        // 'd_loggers' protector

    RecordBuffer          *d_recordBuffer_p;     // holds record buffer (owned)

    PublishAllTriggerCallback
                           d_publishAllCallback; // self-installed callback
                                                 // functor to publish all
                                                 // records within process
                                                 // (always valid)

    Category              *d_defaultCategory_p;  // holds *Default* *Category*
                                                 // (owned)

    int                    d_scratchBufferSize;  // logger default message
                                                 // buffer size (bytes)

    bsl::map<void *, Logger *>
                           d_defaultLoggers;     // *registered* loggers

    bslmt::ReaderWriterMutex
                           d_defaultLoggersLock; // registry lock

    LoggerManagerConfiguration::LogOrder
                           d_logOrder;           // logging order

    LoggerManagerConfiguration::TriggerMarkers
                           d_triggerMarkers;     // trigger markers

    bslma::Allocator      *d_allocator_p;        // memory allocator (held,
                                                 // not owned)

    // PRIVATE CLASS METHODS
    static void initSingletonImpl(
                           const LoggerManagerConfiguration&  configuration,
                           bslma::Allocator                  *globalAllocator);
        // Initialize the logger manager singleton having the specified
        // 'configuration' of defaults and attributes, and the specified
        // 'globalAllocator' used to supply memory.  If 'globalAllocator' is 0,
        // the currently installed global allocator is used.  This method has
        // no effect (aside from logging a warning) if the logger manager
        // singleton already exists.  The behavior is undefined unless the lock
        // guarding the singleton is acquired before calling this method.  Note
        // that 'singletonQLock', an instance of 'bslmt::QLock' local to the
        // implementation file, enforces exclusive access to 's_singleton_p'
        // during initialization and shutdown of the logger manager singleton.

    // PRIVATE CREATORS
#ifndef BDE_OMIT_INTERNAL_DEPRECATED
    LoggerManager(const LoggerManagerConfiguration&  configuration,
                  Observer                          *observer,
                  bslma::Allocator                  *globalAllocator = 0);
        // Create a logger manager having the specified 'observer' that
        // receives published log records and the specified 'configuration' of
        // defaults and attributes.  Optionally specify a 'globalAllocator'
        // used to supply memory.  If 'globalAllocator' is 0, the currently
        // installed global allocator is used.  The behavior is undefined if
        // 'observer' is 0, goes out of scope, or is otherwise destroyed.  Note
        // that the new logger manager is *not* the singleton logger manager
        // used by macros of the BALL logging framework.
#endif  // BDE_OMIT_INTERNAL_DEPRECATED

    // PRIVATE MANIPULATORS
    void constructObject(const LoggerManagerConfiguration& configuration);
        // Construct the default category, default logger members, and record
        // buffer members of this logger manager based on the specified
        // 'configuration'.  The behavior is undefined if this method is
        // invoked more than once on this logger manager.

    void publishAllImp(Transmission::Cause cause);
        // Transmit to the observers registered with this logger manager all
        // log records accumulated in the record buffers of all loggers managed
        // by this logger manager and indicate to the observers the specified
        // publication 'cause'.

  public:
    // CLASS METHODS
#ifndef BDE_OMIT_INTERNAL_DEPRECATED
    static void createLoggerManager(
                        bslma::ManagedPtr<LoggerManager>  *manager,
                        Observer                          *observer,
                        const LoggerManagerConfiguration&  configuration,
                        bslma::Allocator                  *basicAllocator = 0);
        // Create a logger manager that is *not* the singleton logger manager
        // having the specified 'observer' that receives published log records
        // and the specified 'configuration' of defaults and attributes; load
        // the newly-created logger manager into the specified 'manager'
        // managed pointer.  Optionally specify a 'basicAllocator' used to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.  The behavior is undefined if 'observer'
        // is 0, goes out of scope, or is otherwise destroyed.  Note that this
        // method does *not* create the singleton logger manager used by the
        // macros of the BALL logging framework.
        //
        // !DEPRECATED!: Use the 'createLoggerManager' method that does not
        // take a *raw* pointer to an 'observer', together with the
        // 'registerObserver' method (which takes a *shared* pointer to an
        // 'observer'), instead.
#endif  // BDE_OMIT_INTERNAL_DEPRECATED

    static void createLoggerManager(
                        bslma::ManagedPtr<LoggerManager>  *manager,
                        const LoggerManagerConfiguration&  configuration,
                        bslma::Allocator                  *basicAllocator = 0);
        // Create a logger manager that is *not* the singleton logger manager
        // having the specified 'configuration' of defaults and attributes;
        // load the newly-created logger manager into the specified 'manager'
        // managed pointer.  Optionally specify a 'basicAllocator' used to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.  Note that this method does *not* create
        // the singleton logger manager used by the macros of the BALL logging
        // framework.

    static Record *getRecord(const char *fileName, int lineNumber);
        // Return the address of a modifiable record with the specified
        // 'fileName' and 'lineNumber' attributes, and whose memory is supplied
        // by the currently installed default allocator.  Note that the
        // returned 'Record' must subsequently be supplied to a call to the
        // 'LoggerManager::logMessage' method.

#ifndef BDE_OMIT_INTERNAL_DEPRECATED
    static LoggerManager& initSingleton(
                       Observer                          *observer,
                       bslma::Allocator                  *globalAllocator = 0);
    static LoggerManager& initSingleton(
                       Observer                          *observer,
                       const LoggerManagerConfiguration&  configuration,
                       bslma::Allocator                  *globalAllocator = 0);
        // Initialize the logger manager singleton having the specified
        // 'observer' that receives published log records.  Optionally specify
        // a 'configuration' describing how the singleton should be configured.
        // If 'configuration' is not specified, a default constructed
        // 'LoggerManagerConfiguration' object is used.  Optionally specify a
        // 'globalAllocator' used to supply memory.  If 'globalAllocator' is 0,
        // the currently installed global allocator is used.  Return a
        // non-'const' reference to the logger manager singleton.  This method
        // has no effect (aside from logging a warning) if the logger manager
        // singleton already exists.  The behavior is undefined if 'observer'
        // is 0, goes out of scope, or is otherwise destroyed.
        //
        // !DEPRECATED!: Use the 'initSingleton' method that does not take a
        // *raw* pointer to an 'observer', together with the 'registerObserver'
        // method (which takes a *shared* pointer to an 'observer'), instead.
#endif  // BDE_OMIT_INTERNAL_DEPRECATED

    static LoggerManager& initSingleton(bslma::Allocator *globalAllocator = 0);
    static LoggerManager& initSingleton(
                       const LoggerManagerConfiguration&  configuration,
                       bslma::Allocator                  *globalAllocator = 0);
        // Initialize the logger manager singleton.  Optionally specify a
        // 'configuration' describing how the singleton should be configured.
        // If 'configuration' is not specified, a default constructed
        // 'LoggerManagerConfiguration' object is used.  Optionally specify a
        // 'globalAllocator' used to supply memory.  If 'globalAllocator' is 0,
        // the currently installed global allocator is used.  Return a
        // non-'const' reference to the logger manager singleton.  This method
        // has no effect (aside from logging a warning) if the logger manager
        // singleton already exists.

    static int initSingleton(LoggerManager *singleton,
                             bool           adoptSingleton = false);
        // Initialize the logger manager singleton with the specified
        // 'singleton'.  Optionally specify an 'adoptSingleton' flag indicating
        // whether this method takes ownership of 'singleton', in which case
        // 'shutDownSingleton' will destroy 'singleton'.  If 'adoptSingleton'
        // is not specified, this method does *not* take ownership of
        // 'singleton' (and 'shutDownSingleton' will not destroy it).  Return 0
        // if the logger manager singleton was successfully initialized, and a
        // non-zero value otherwise.  This method has no effect (aside from
        // logging a warning) if the logger manager singleton already exists,
        // in which case this method does *not* take ownership of 'singleton'
        // regardless of the value of 'adoptSingleton'.  Note that this version
        // of 'initSingleton' is meant for use *only* on Windows to initialize
        // another dynamically loaded copy of the 'LoggerManager' system.  Also
        // note that a suitable singleton may be obtained by calling
        // 'createLoggerManager', or from the 'singleton' class method of an
        // already-initialized 'LoggerManager' system.

    static bool isInitialized();
        // Return 'true' if the logger manager singleton exists, and 'false'
        // otherwise.

    static void logMessage(int severity, Record *record);
        // Publish the specified 'record' using
        // 'bsls::Log::platformDefaultMessageHandler' after setting its
        // severity attribute to the specified 'severity', and dispose of
        // 'record'.  The behavior is undefined unless 'record' was obtained by
        // a call to the 'LoggerManager::getRecord' method.  Note that 'record'
        // will be invalid after this method returns.

    static char *obtainMessageBuffer(bslmt::Mutex **mutex, int *bufferSize);
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

    static bslma::ManagedPtr<char> obtainMessageBuffer(int *bufferSize);
        // Return a managed pointer that refers to the memory block to which
        // this thread of execution has exclusive access and load the size (in
        // bytes) of this buffer into the specified 'bufferSize' address.  Note
        // that this method is intended for *internal* *use* only.

    static void shutDownSingleton();
        // Destroy the logger manager singleton and release all resources used
        // by it.  This method has no effect if the logger manager singleton
        // does not exist (i.e., it has not been initialized or has already
        // been destroyed).  The behavior is undefined if this method is called
        // from one thread while another thread is accessing the logger manager
        // singleton (i.e., this method is *not* thread-safe).

    static LoggerManager& singleton();
        // Return a non-'const' reference to the logger manager singleton.  The
        // behavior is undefined unless the logger manager singleton exists.

    // CREATORS
    explicit LoggerManager(
                       const LoggerManagerConfiguration&  configuration,
                       bslma::Allocator                  *globalAllocator = 0);
        // Create a logger manager having the specified 'configuration' of
        // defaults and attributes.  Optionally specify a 'globalAllocator'
        // used to supply memory.  If 'globalAllocator' is 0, the currently
        // installed global allocator is used.  Note that the new logger
        // manager is *not* the singleton logger manager used by macros of the
        // BALL logging framework.

    ~LoggerManager();
        // Destroy this logger manager.

    // MANIPULATORS
                             // Logger Management

    Logger *allocateLogger(RecordBuffer *buffer);
    Logger *allocateLogger(RecordBuffer *buffer, int scratchBufferSize);
        // Return the address of a modifiable logger managed by this logger
        // manager configured with the specified record 'buffer'.  Optionally
        // specify a 'scratchBufferSize' for the logger's user-accessible
        // message buffer.  If 'scratchBufferSize' is not specified, the value
        // configured at construction is used.  Note that this method is
        // primarily intended for use in multi-threaded applications, but can
        // be used to partition logging streams even within a single thread.
        // Also note that ownership of 'buffer' is *not* transferred, and
        // hence, will *not* be destroyed (or otherwise affected) after the
        // logger is deallocated.

#ifndef BDE_OMIT_INTERNAL_DEPRECATED
    Logger *allocateLogger(RecordBuffer *buffer,
                           Observer     *observer);
    Logger *allocateLogger(RecordBuffer *buffer,
                           int           scratchBufferSize,
                           Observer     *observer);
        // Return the address of a modifiable logger managed by this logger
        // manager having the specified 'observer' that receives published log
        // records and configured with the specified record 'buffer'.
        // Optionally specify a 'scratchBufferSize' for the logger's
        // user-accessible message buffer.  If 'scratchBufferSize' is not
        // specified, the value configured at construction is used.  Note that
        // this method is primarily intended for use in multi-threaded
        // applications, but can be used to partition logging streams even
        // within a single thread.  Also note that ownership of 'buffer' and
        // 'observer' is *not* transferred, and hence, will *not* be destroyed
        // (or otherwise affected) after the logger is deallocated.
        //
        // !DEPRECATED!: Use the 'allocateLogger' method that does not take a
        // *raw* pointer to an 'observer', together with the 'registerObserver'
        // method (which takes a *shared* pointer to an 'observer'), instead.
#endif // BDE_OMIT_INTERNAL_DEPRECATED

    Logger *allocateLogger(RecordBuffer                     *buffer,
                           const bsl::shared_ptr<Observer>&  observer);
    Logger *allocateLogger(RecordBuffer                     *buffer,
                           int                               scratchBufferSize,
                           const bsl::shared_ptr<Observer>&  observer);
        // Return the address of a modifiable logger managed by this logger
        // manager having the specified 'observer' that receives published log
        // records and configured with the specified record 'buffer'.
        // Optionally specify a 'scratchBufferSize' for the logger's
        // user-accessible message buffer.  If 'scratchBufferSize' is not
        // specified, the value configured at construction is used.  Note that
        // this method is primarily intended for use in multi-threaded
        // applications, but can be used to partition logging streams even
        // within a single thread.  Also note that ownership of 'buffer' and
        // 'observer' is *not* transferred, and hence, will *not* be destroyed
        // (or otherwise affected) after the logger is deallocated.

    void deallocateLogger(Logger *logger);
        // Deallocate the specified 'logger' and release 'logger' from
        // management by this logger manager.  The behavior is undefined unless
        // 'logger' was obtained by invoking the 'allocateLogger' method of
        // this logger manager and 'logger' has not yet been deallocated.  Note
        // that this method is intended primarily for multi-threaded
        // applications.

    Logger& getLogger();
        // Return a non-'const' reference to a logger managed by this logger
        // manager suitable for performing logging operations for this thread
        // of execution.

    void setLogger(Logger *logger);
        // Set the default logger used by this thread of execution to the
        // specified 'logger', or to the global default logger if 'logger' is
        // 0.  The behavior is undefined unless 'logger' was obtained from this
        // logger manager, and this thread of execution does not hold exclusive
        // access to the record buffer of its current default logger.

                             // Category Management

    Category *addCategory(const char *categoryName,
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

    Category& defaultCategory();
        // Return a non-'const' reference to the *Default* *Category* in the
        // category registry of this logger manager.

    Category *lookupCategory(const char *categoryName);
        // Return the address of the modifiable category in the category
        // registry of this logger manager having the specified 'categoryName',
        // or 0 if no such category exists.  The behavior is undefined unless
        // 'categoryName' is null-terminated.

    const Category *setCategory(const char *categoryName);
        // Add to the category registry of this logger manager a new category
        // having the specified 'categoryName' and default threshold levels if
        // 'categoryName' is not present in the registry and the number of
        // categories in the registry is less than the registry capacity.
        // Return the address of the (possibly newly-created) non-modifiable
        // category having 'categoryName', if such a category exists, and the
        // address of the non-modifiable *Default* *Category* otherwise.  The
        // behavior is undefined unless 'categoryName' is null-terminated.
        // Note that a valid category address is *always* returned.

    const Category *setCategory(CategoryHolder *categoryHolder,
                                const char     *categoryName);
        // Add to the category registry of this logger manager a new category
        // having the specified 'categoryName' and default threshold levels if
        // 'categoryName' is not present in the registry and the number of
        // categories in the registry is less than the registry capacity.
        // Return the address of the (possibly newly-created) non-modifiable
        // category having 'categoryName', if such a category exists, and the
        // address of the non-modifiable *Default* *Category* otherwise.  If
        // the specified 'categoryHolder' is non-null, then also load into
        // 'categoryHolder' the returned category and its maximum level and
        // link 'categoryHolder' to the category if it has not yet been linked.
        // The behavior is undefined unless 'categoryName' is null-terminated.
        // Note that a valid category address is *always* returned.

    Category *setCategory(const char *categoryName,
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
        // *Default* *Category*, is returned on failure.

    void setMaxNumCategories(int length);
        // Set the capacity of the category registry of this logger manager to
        // the specified 'length'.  If 'length' is 0, no limit will be imposed.
        // No categories are removed from the registry if the current number of
        // categories exceeds 'length'.  However, subsequent attempts to add
        // categories to the registry will fail.  The behavior is undefined
        // unless '0 <= length'.

                             // Observer Management

    void deregisterAllObservers();
        // Remove all observers from the registry of observers maintained by
        // this logger manager.

    int deregisterObserver(const bsl::string_view& observerName);
        // Remove the observer having the specified 'observerName' from the
        // registry of observers maintained by this logger manager.  Return 0
        // if the observer having 'observerName' was successfully deregistered
        // from this logger manager, and a non-zero value (with no effect)
        // otherwise.  Henceforth, the observer that had 'observerName' will no
        // longer receive log records published by this logger manager.

    int deregisterAttributeCollector(const bsl::string_view& collectorName);
        // Remove the attribute collector having the specified 'collectorName'
        // from the registry of collectors maintained by this logger manager.
        // Return 0 if the collector having 'collectorName' was successfully
        // deregistered from this logger manager, and a non-zero value (with no
        // effect) otherwise.

    bsl::shared_ptr<Observer> findObserver(
                                         const bsl::string_view& observerName);
        // Return a shared pointer to the observer having the specified
        // 'observerName' in the registry of this logger manager, and an empty
        // shared pointer if there is no such observer otherwise.

    template <class t_OBSERVER>
    int findObserver(bsl::shared_ptr<t_OBSERVER> *result,
                     const bsl::string_view&      observerName);
        // Load into the specified 'result' a shared pointer to the observer of
        // (template parameter) 't_OBSERVER' type having the specified
        // 'observerName' in the registry of this logger manager, and an empty
        // shared pointer if there is no such observer otherwise.  Return 0 if
        // a non-empty shared pointer was loaded, and a non-zero value
        // otherwise.  Note that an empty shared pointer will be loaded if
        // either no observer having 'observerName' is in the registry or the
        // observer registered with that name is not of 't_OBSERVER' type.

#ifndef BDE_OMIT_INTERNAL_DEPRECATED
    Observer *observer();
        // Return the address of the modifiable legacy observer registered with
        // this logger manager.
        //
        // !DEPRECATED!: Use 'findObserver' instead.
#endif  // BDE_OMIT_INTERNAL_DEPRECATED

    int registerObserver(const bsl::shared_ptr<Observer>& observer,
                         const bsl::string_view&          observerName);
        // Add the specified 'observer' with the specified 'observerName' to
        // the registry of observers maintained by this logger manager.  Return
        // 0 if 'observer' was successfully registered with this logger
        // manager, and a non-zero value (with no effect) otherwise.
        // Henceforth, all log records published by this logger manager are
        // published to this observer, until 'observer' is deregistered.  The
        // behavior is undefined if a cyclic reference is created among
        // registered observers.  Note that this method will fail if an
        // observer having 'observerName' is already registered.

    int registerAttributeCollector(const AttributeCollector& collector,
                                   const bsl::string_view&   collectorName);
        // Add the specified 'collector' with the specified 'collectorName' to
        // the registry of attribute collectors maintained by this logger
        // manager.  Return 0 if 'collector' was successfully registered with
        // this logger manager, and a non-zero value (with no effect)
        // otherwise.  Note that this method will fail if a collector having
        // 'collectorName' is already registered.

                     // Threshold Level Management Manipulators

    void resetDefaultThresholdLevels();
        // Reset the default threshold levels of this logger manager to the
        // original "factory-supplied" default values or the factory overrides
        // supplied at construction.

    void setCategoryThresholdsToCurrentDefaults(Category *category);
        // Set the threshold levels of the specified 'category' in the category
        // registry of this logger manager to the current default threshold
        // values.  The behavior is undefined unless 'category' is non-null.

    void setCategoryThresholdsToFactoryDefaults(Category *category);
        // Set the threshold levels of the specified 'category' in the category
        // registry of this logger manager to the original "factory-supplied"
        // default values or the factory overrides supplied at construction.
        // The behavior is undefined unless 'category' is non-null.

    int setDefaultThresholdLevels(int recordLevel,
                                  int passLevel,
                                  int triggerLevel,
                                  int triggerAllLevel);
        // Set the default threshold levels of this logger manager to the
        // specified 'recordLevel', 'passLevel', 'triggerLevel', and
        // 'triggerAllLevel' values, respectively, if each threshold level is
        // in the range '[0 .. 255]'.  Return 0 on success, and a non-zero
        // value otherwise (with no effect on any default threshold level).

    void setDefaultThresholdLevelsCallback(
                                     DefaultThresholdLevelsCallback *callback);
        // Set the default-thresholds callback of this logger manager to the
        // specified 'callback'.  The default-thresholds callback is used to
        // determine default threshold levels for categories added to the
        // registry by the 'setCategory(const char *)' method.

                             // Rule Management

    int addRule(const Rule& value);
        // Add a rule having the specified 'value' to the set of (unique)
        // rules maintained by this object.  Return the number of rules added
        // (i.e., 1 on success and 0 if a rule with the same value is already
        // present).

    int addRules(const RuleSet& ruleSet);
        // Add each rule in the specified 'ruleSet' to the set of (unique)
        // rules maintained by this object.  Return the number of rules added.
        // Note that each rule having the same value as an existing rule will
        // be ignored.

    void removeAllRules();
        // Remove every rule from the set of rules maintained by this object.

    int removeRule(const Rule& value);
        // Remove the rule having the specified 'value' from this set of rules
        // maintained by this object.  Return the number of rules removed
        // (i.e., 1 on success and 0 if no rule having the same value is
        // found.)

    int removeRules(const RuleSet& ruleSet);
        // Remove each rule in the specified 'ruleSet' from this set of rules
        // maintained by this object.  Return the number of rules removed.

                             // Miscellaneous

    void publishAll();
        // Transmit to the observers registered with this logger manager all
        // log records accumulated in the record buffers of all loggers managed
        // by this logger manager, and indicate the publication cause to be
        // 'MANUAL_PUBLISH_ALL'.

    template <class t_CATEGORY_VISITOR>
    void visitCategories(const t_CATEGORY_VISITOR& visitor);
        // Invoke the specified 'visitor' functor on each category managed by
        // this object, providing that functor modifiable access to each
        // category.  'visitor' must be a functor that can be called as if it
        // had the following signature:
        //..
        //  void operator()(Category *);
        //..

    template <class t_OBSERVER_VISITOR>
    void visitObservers(
                BSLS_COMPILERFEATURES_FORWARD_REF(t_OBSERVER_VISITOR) visitor);
        // Invoke the specified 'visitor' functor of (template parameter)
        // 't_OBSERVER_VISITOR' type on each element in the registry of this
        // logger manager, supplying that functor modifiable access to each
        // observer.  'visitor' must be a functor that can be called as if it
        // had the following signature:
        //..
        //  void operator()(const bsl::shared_ptr<Observer>& observer,
        //                  const bsl::string_view&          observerName);
        //..

    // ACCESSORS
    bslma::Allocator *allocator() const;
        // Return the address of the modifiable allocator held by this logger
        // manager.

    const Category& defaultCategory() const;
        // Return a 'const' reference to the *Default* *Category* in the
        // category registry of this logger manager.

    int defaultPassThresholdLevel() const;
        // Return the default pass threshold level of this logger manager.

    int defaultRecordThresholdLevel() const;
        // Return the default record threshold level of this logger manager.

    int defaultTriggerAllThresholdLevel() const;
        // Return the default trigger-all threshold level of this logger
        // manager.

    int defaultTriggerThresholdLevel() const;
        // Return the default trigger threshold level of this logger manager.

    bsl::shared_ptr<const Observer> findObserver(
                                   const bsl::string_view& observerName) const;
        // Return a shared pointer to the observer having the specified
        // 'observerName' in the registry of this logger manager, and an empty
        // shared pointer if there is no such observer otherwise.

    template <class t_OBSERVER>
    int findObserver(bsl::shared_ptr<const t_OBSERVER> *result,
                     const bsl::string_view&            observerName) const;
        // Load into the specified 'result' a shared pointer to the observer of
        // (template parameter) 't_OBSERVER' type having the specified
        // 'observerName' in the registry of this logger manager, and an empty
        // shared pointer if there is no such observer otherwise.  Return 0 if
        // a non-empty shared pointer was loaded, and a non-zero value
        // otherwise.  Note that an empty shared pointer will be loaded if
        // either no observer having 'observerName' is in the registry or the
        // observer registered with that name is not of 't_OBSERVER' type.

    bool isCategoryEnabled(const Category *category, int severity) const;
        // Return 'true' if the specified 'severity' is more severe (i.e., is
        // numerically less than) at least one of the threshold levels of the
        // specified 'category', and 'false' otherwise.  If the returned
        // 'value' is 'false', then a subsequent call to
        // 'getLogger().logMessage()' (with 'category' and 'severity') will
        // have no effect.  This method compares 'severity' with the threshold
        // levels determined by combining 'category->thresholdLevels()' with
        // the thresholds provided by any relevant and active logging rules (in
        // 'ruleSet()') that apply to 'category'.  Note that a rule applies to
        // 'category' if the rule's pattern matches 'category->categoryName()',
        // and a rule is active if all the predicates defined for that rule are
        // satisfied by the current thread's attributes (i.e.,
        // 'Rule::evaluate()' returns 'true' for the collection of attributes
        // maintained by the current thread's 'AttributeContext' object).

    const Category *lookupCategory(const char *categoryName) const;
        // Return the address of the non-modifiable category in the category
        // registry of this logger manager having the specified 'categoryName',
        // or 0 if no such category exists.  The behavior is undefined unless
        // 'categoryName' is null-terminated.

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

#ifndef BDE_OMIT_INTERNAL_DEPRECATED
    const Observer *observer() const;
        // Return the address of the non-modifiable observer registered with
        // this logger manager.
        //
        // !DEPRECATED!: Use 'findObserver' instead.
#endif  // BDE_OMIT_INTERNAL_DEPRECATED

    const RuleSet& ruleSet() const;
        // Return a 'const' reference to the rule set maintained by this
        // object.

    const UserFieldsPopulatorCallback *userFieldsPopulatorCallback() const;
        // Return the address of the non-modifiable user populator functor
        // registered with this logger manager, or 0 if there is no registered
        // user populator functor.

    template <class t_CATEGORY_VISITOR>
    void visitCategories(const t_CATEGORY_VISITOR& visitor) const;
        // Invoke the specified 'visitor' functor on each category managed by
        // this object, providing that functor non-modifiable access to each
        // category.  'visitor' must be a functor that can be called as if it
        // had the following signature:
        //..
        //  void operator()(const Category *);
        //..

    template <class t_OBSERVER_VISITOR>
    void visitObservers(
          BSLS_COMPILERFEATURES_FORWARD_REF(t_OBSERVER_VISITOR) visitor) const;
        // Invoke the specified 'visitor' functor of (template parameter)
        // 't_OBSERVER_VISITOR' type on each element in the registry of this
        // logger manager, supplying that functor modifiable access to each
        // observer.  'visitor' must be a functor that can be called as if it
        // had the following signature:
        //..
        //  void operator()(const bsl::shared_ptr<Observer>& observer,
        //                  const bsl::string_view&          observerName);
        //..

    // Threshold Level Management Accessors

    const ThresholdAggregate& defaultThresholdLevels() const;
        // Return the default threshold levels associated with this logger
        // manager object.

    int thresholdLevelsForNewCategory(ThresholdAggregate *levels,
                                      const char         *categoryName) const;
        // Load into the specified '*levels' the threshold levels that would be
        // set for a newly created category, irrespective of whether a category
        // with the specified 'categoryName' is already in the registry.
        // Return 0 on success and a non-zero value otherwise.  If the client
        // has configured a default threshold levels callback (see
        // 'ball::LoggerManager::DefaultThresholdLevelsCallback' in the
        // component doc), the 'categoryName' will be supplied to that callback
        // which will set '*levels'.  Otherwise, if no default threshold levels
        // callback has been provided, the default threshold levels are used.
        // Note that this function will report an error if the callback returns
        // invalid levels.  Also note that if a category named 'categoryName'
        // is already in the registry, the levels returned by this method may
        // differ from the levels of that category.
};

                        // ==============================
                        // class LoggerManagerScopedGuard
                        // ==============================

class LoggerManagerScopedGuard {
    // This class implements a scoped guard that, on construction, creates the
    // logger manager singleton, and, on destruction, destroys the singleton.

  private:
    // NOT IMPLEMENTED
    LoggerManagerScopedGuard(const LoggerManagerScopedGuard&);
    LoggerManagerScopedGuard& operator=(const LoggerManagerScopedGuard&);

  public:
    // CREATORS
#ifndef BDE_OMIT_INTERNAL_DEPRECATED
    LoggerManagerScopedGuard(
                       Observer                          *observer,
                       const LoggerManagerConfiguration&  configuration,
                       bslma::Allocator                  *globalAllocator = 0);
        // Create a scoped guard that will create the logger manager singleton
        // having the specified 'observer' that receives published log records
        // and the specified 'configuration' of defaults and attributes.
        // Optionally specify a 'globalAllocator' used to supply memory.  If
        // 'globalAllocator' is 0, the currently installed global allocator is
        // used.  This method has no effect (aside from logging a warning) if
        // the logger manager singleton already exists.  The behavior is
        // undefined if 'observer' is 0, goes out of scope, or is otherwise
        // destroyed.  Note that on destruction, this scoped guard will destroy
        // the logger manager singleton, if the singleton exists at that time.
        //
        // !DEPRECATED!: Use the 'LoggerManagerScopedGuard' constructor that
        // does not take a *raw* pointer to an 'observer', together with the
        // 'registerObserver' method (which takes a *shared* pointer to an
        // 'observer'), instead.
#endif  // BDE_OMIT_INTERNAL_DEPRECATED

    explicit LoggerManagerScopedGuard(
                       const LoggerManagerConfiguration&  configuration,
                       bslma::Allocator                  *globalAllocator = 0);
        // Create a scoped guard that will create the logger manager singleton
        // having the specified 'configuration' of defaults and attributes.
        // Optionally specify a 'globalAllocator' used to supply memory.  If
        // 'globalAllocator' is 0, the currently installed global allocator is
        // used.  This method has no effect (aside from logging a warning) if
        // the logger manager singleton already exists.  Note that on
        // destruction, this scoped guard will destroy the logger manager
        // singleton, if the singleton exists at that time.

    ~LoggerManagerScopedGuard();
        // Destroy the logger manager singleton, if the singleton exists, and
        // destroy this scoped guard.
};

#ifndef BDE_OMIT_INTERNAL_DEPRECATED

                        // ===============================
                        // class LoggerManagerCategoryIter
                        // ===============================

class LoggerManagerCategoryIter {
    // This class defines an iterator providing sequential, read-only access to
    // the categories in the registry of a logger manager's category manager.
    // The order of the iteration is undefined.
    //
    // !DEPRECATED!: Use the 'LoggerManager::visitCategories' accessor instead.

    // DATA
    CategoryManagerIter d_iter;  // contained category manager iterator

  private:
    // NOT IMPLEMENTED
    LoggerManagerCategoryIter(const LoggerManagerCategoryIter&);
    LoggerManagerCategoryIter& operator=(const LoggerManagerCategoryIter&);

  public:
    // CREATORS
    explicit LoggerManagerCategoryIter(const LoggerManager& loggerManager);
        // Create an iterator providing non-modifiable access to the categories
        // maintained by the specified 'loggerManager' that is initialized to
        // refer to the first category in the sequence of categories in the
        // registry of 'loggerManager', if one exists, and is initialized to be
        // invalid otherwise.  The order of iteration is undefined.  The
        // behavior is undefined unless the lifetime of 'loggerManager' is at
        // least as long as the lifetime of this iterator.

    //! ~LoggerManagerCategoryIter() = default;
        // Destroy this iterator.

    // MANIPULATORS
    void operator++();
        // Advance this iterator to refer to the next unvisited category.  If
        // no such category exists, this iterator becomes invalid.  The
        // behavior is undefined unless this iterator is initially valid.  Note
        // that the order of iteration is undefined.

    // ACCESSORS
    operator const void *() const;
        // Return a non-zero value if this iterator is valid, and 0 otherwise.

    const Category& operator()() const;
        // Return a 'const' reference to the category currently referred to by
        // this iterator.  The behavior is undefined unless this iterator is
        // valid.
};

                        // ================================
                        // class LoggerManagerCategoryManip
                        // ================================

class LoggerManagerCategoryManip {
    // This class defines an iterator providing sequential, modifiable access
    // to the categories in the registry of a logger manager's category
    // manager.  The order of the iteration is undefined.
    //
    // !DEPRECATED!: Use the 'LoggerManager::visitCategories' manipulator
    // instead.

    // DATA
    CategoryManagerManip d_manip;  // contained category manager iterator

  private:
    // NOT IMPLEMENTED
    LoggerManagerCategoryManip(const LoggerManagerCategoryManip&);
    LoggerManagerCategoryManip& operator=(const LoggerManagerCategoryManip&);

  public:
    // CREATORS
    explicit LoggerManagerCategoryManip(LoggerManager *loggerManager);
        // Create an iterator providing modifiable access to the categories
        // maintained by the specified 'loggerManager' that is initialized to
        // refer to the first category in the sequence of categories in the
        // registry of 'loggerManager', if one exists, and is initialized to be
        // invalid otherwise.  The order of iteration is undefined.  The
        // behavior is undefined unless the lifetime of 'loggerManager' is at
        // least as long as the lifetime of this iterator.

    //! ~LoggerManagerCategoryManip() = default;
        // Destroy this iterator.

    // MANIPULATORS
    void advance();
        // Advance this iterator to refer to the next unvisited category.  If
        // no such category exists, this iterator becomes invalid.  The
        // behavior is undefined unless this iterator is initially valid.  Note
        // that the order of iteration is undefined.

    Category& operator()();
        // Return a non-'const' reference to the category currently referred to
        // by this iterator.  The behavior is undefined unless this iterator is
        // valid.

    // ACCESSORS
    operator const void *() const;
        // Return a non-zero value if this iterator is valid, and 0 otherwise.
};

#endif  // BDE_OMIT_INTERNAL_DEPRECATED

// ============================================================================
//                              INLINE DEFINITIONS
// ============================================================================

                        // ------------
                        // class Logger
                        // ------------

// MANIPULATORS
#ifndef BDE_OMIT_INTERNAL_DEPRECATED
inline
char *Logger::messageBuffer()
{
    return d_scratchBuffer_p;
}
#endif // BDE_OMIT_INTERNAL_DEPRECATED

inline
void Logger::publish()
{
    publish(Transmission::e_MANUAL_PUBLISH);
}

inline
void Logger::removeAll()
{
    d_recordBuffer_p->removeAll();
}

// ACCESSORS
inline
int Logger::messageBufferSize() const
{
    return d_scratchBufferSize;
}

inline
int Logger::numRecordsInUse() const
{
    return d_recordPool.numObjects() - d_recordPool.numAvailableObjects();
}

                        // -------------------
                        // class LoggerManager
                        // -------------------

// CLASS METHODS
inline
bool LoggerManager::isInitialized()
{
    return (LoggerManager *)0 != s_singleton_p;
}

inline
LoggerManager& LoggerManager::singleton()
{
    return *s_singleton_p;
}

// MANIPULATORS
                             // Category Management

inline
Category& LoggerManager::defaultCategory()
{
    return *d_defaultCategory_p;
}

inline
const Category *LoggerManager::setCategory(const char *categoryName)
{
    return setCategory(0, categoryName);
}

                             // Observer Management

inline
void LoggerManager::deregisterAllObservers()
{
    d_observer->deregisterAllObservers();
}

inline
int LoggerManager::deregisterObserver(const bsl::string_view& observerName)
{
    return d_observer->deregisterObserver(observerName);
}

inline
int LoggerManager::deregisterAttributeCollector(
                                         const bsl::string_view& collectorName)
{
    return d_attributeCollectors.removeCollector(collectorName);
}

inline
bsl::shared_ptr<Observer>
LoggerManager::findObserver(const bsl::string_view& observerName)
{
    return d_observer->findObserver(observerName);
}

template <class t_OBSERVER>
inline
int LoggerManager::findObserver(bsl::shared_ptr<t_OBSERVER> *result,
                                const bsl::string_view&      observerName)
{
    return d_observer->findObserver(result, observerName);
}

inline
int
LoggerManager::registerObserver(const bsl::shared_ptr<Observer>& observer,
                                const bsl::string_view&          observerName)
{
    return d_observer->registerObserver(observer, observerName);
}

inline
int
LoggerManager::registerAttributeCollector(
                                       const AttributeCollector& collector,
                                       const bsl::string_view&   collectorName)
{
    return d_attributeCollectors.addCollector(collector, collectorName);
}
                             // Threshold Level Management

inline
void LoggerManager::resetDefaultThresholdLevels()
{
    d_defaultThresholdLevels = d_factoryThresholdLevels;
}

                             // Rule Management

inline
int LoggerManager::addRule(const Rule& value)
{
    return d_categoryManager.addRule(value);
}

inline
int LoggerManager::addRules(const RuleSet& ruleSet)
{
    return d_categoryManager.addRules(ruleSet);
}

inline
void LoggerManager::removeAllRules()
{
    d_categoryManager.removeAllRules();
}

inline
int LoggerManager::removeRule(const Rule& value)
{
    return d_categoryManager.removeRule(value);
}

inline
int LoggerManager::removeRules(const RuleSet& ruleSet)
{
    return d_categoryManager.removeRules(ruleSet);
}

                             // Miscellaneous

inline
void LoggerManager::publishAll()
{
    publishAllImp(Transmission::e_MANUAL_PUBLISH_ALL);
}

template <class t_CATEGORY_VISITOR>
inline
void LoggerManager::visitCategories(const t_CATEGORY_VISITOR& visitor)
{
    d_categoryManager.visitCategories(visitor);
}

template <class t_OBSERVER_VISITOR>
inline
void LoggerManager::visitObservers(
                 BSLS_COMPILERFEATURES_FORWARD_REF(t_OBSERVER_VISITOR) visitor)
{
    d_observer->visitObservers(
                   BSLS_COMPILERFEATURES_FORWARD(t_OBSERVER_VISITOR, visitor));
}

// ACCESSORS
inline
bslma::Allocator *LoggerManager::allocator() const
{
    return d_allocator_p;
}

inline
const Category& LoggerManager::defaultCategory() const
{
    return *d_defaultCategory_p;
}

inline
int LoggerManager::defaultPassThresholdLevel() const
{
    return d_defaultThresholdLevels.passLevel();
}

inline
int LoggerManager::defaultRecordThresholdLevel() const
{
    return d_defaultThresholdLevels.recordLevel();
}

inline
int LoggerManager::defaultTriggerAllThresholdLevel() const
{
    return d_defaultThresholdLevels.triggerAllLevel();
}

inline
int LoggerManager::defaultTriggerThresholdLevel() const
{
    return d_defaultThresholdLevels.triggerLevel();
}

inline
bsl::shared_ptr<const Observer>
LoggerManager::findObserver(const bsl::string_view& observerName) const
{
    const BroadcastObserver *observerPtr = d_observer.get();

    return observerPtr->findObserver(observerName);
}

template <class t_OBSERVER>
inline
int LoggerManager::findObserver(
                         bsl::shared_ptr<const t_OBSERVER> *result,
                         const bsl::string_view&            observerName) const
{
    const BroadcastObserver *observerPtr = d_observer.get();

    return observerPtr->findObserver(result, observerName);
}

inline
int LoggerManager::maxNumCategories() const
{
    return static_cast<int>(d_maxNumCategoriesMinusOne) + 1;
}

inline
int LoggerManager::numCategories() const
{
    return d_categoryManager.length();
}

inline
const RuleSet& LoggerManager::ruleSet() const
{
    return d_categoryManager.ruleSet();
}

template <class t_CATEGORY_VISITOR>
inline
void LoggerManager::visitCategories(const t_CATEGORY_VISITOR& visitor) const
{
    d_categoryManager.visitCategories(visitor);
}

template <class t_OBSERVER_VISITOR>
inline
void LoggerManager::visitObservers(
           BSLS_COMPILERFEATURES_FORWARD_REF(t_OBSERVER_VISITOR) visitor) const
{
    d_observer->visitObservers(
                   BSLS_COMPILERFEATURES_FORWARD(t_OBSERVER_VISITOR, visitor));
}

                        // ------------------------------
                        // class LoggerManagerScopedGuard
                        // ------------------------------

// CREATORS
#ifndef BDE_OMIT_INTERNAL_DEPRECATED
inline
LoggerManagerScopedGuard::LoggerManagerScopedGuard(
                            Observer                          *observer,
                            const LoggerManagerConfiguration&  configuration,
                            bslma::Allocator                  *globalAllocator)
{
    LoggerManager::initSingleton(observer, configuration, globalAllocator);
}
#endif  // BDE_OMIT_INTERNAL_DEPRECATED

inline
LoggerManagerScopedGuard::LoggerManagerScopedGuard(
                            const LoggerManagerConfiguration&  configuration,
                            bslma::Allocator                  *globalAllocator)
{
    LoggerManager::initSingleton(configuration, globalAllocator);
}

inline
LoggerManagerScopedGuard::~LoggerManagerScopedGuard()
{
    LoggerManager::shutDownSingleton();
}

#ifndef BDE_OMIT_INTERNAL_DEPRECATED

                        // -------------------------------
                        // class LoggerManagerCategoryIter
                        // -------------------------------

// CREATORS
inline
LoggerManagerCategoryIter::LoggerManagerCategoryIter(
                                            const LoggerManager& loggerManager)
: d_iter(loggerManager.d_categoryManager)
{
}

// MANIPULATORS
inline
void LoggerManagerCategoryIter::operator++()
{
    ++d_iter;
}

// ACCESSORS
inline
LoggerManagerCategoryIter::operator const void *() const
{
    return d_iter;
}

inline
const Category& LoggerManagerCategoryIter::operator()() const
{
    return d_iter();
}

                        // --------------------------------
                        // class LoggerManagerCategoryManip
                        // --------------------------------

// CREATORS
inline
LoggerManagerCategoryManip::LoggerManagerCategoryManip(
                                                  LoggerManager *loggerManager)
: d_manip(&loggerManager->d_categoryManager)
{
}

// MANIPULATORS
inline
void LoggerManagerCategoryManip::advance()
{
    d_manip.advance();
}

inline
Category& LoggerManagerCategoryManip::operator()()
{
    return d_manip();
}

// ACCESSORS
inline
LoggerManagerCategoryManip::operator const void *() const
{
    return d_manip;
}

#endif  // BDE_OMIT_INTERNAL_DEPRECATED

}  // close package namespace
}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2017 Bloomberg Finance L.P.
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
