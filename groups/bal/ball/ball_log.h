// ball_log.h                                                         -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BALL_LOG
#define INCLUDED_BALL_LOG

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide macros and utility functions to facilitate logging.
//
//@CLASSES:
//  ball::Log: namespace for a suite of logging utilities
//
//@SEE_ALSO: ball_loggermanager, ball_categorymanager, ball_severity,
//           ball_record
//
//@DESCRIPTION: This component provides pre-processor macros and utility
// functions to facilitate use of the 'ball_loggermanager' component.  In
// particular, the macros defined herein greatly simplify the mechanics of
// generating log records.  The utility functions are only intended for use by
// the macros and should *not* be called directly.
//
///Thread Safety
///-------------
// All macros defined in this component are thread-safe, and can be invoked
// concurrently by multiple threads.
//
///Macro Reference
///---------------
// This section documents the pre-processor macros defined in this component.
//
// The following two macros establish the logging context required by the other
// macros.  A use of one of these two macros must be visible from within the
// lexical scope where the C++ stream-based and 'printf'-style macros are used:
//
//: 'BALL_LOG_SET_CATEGORY(CATEGORY)':
//:     Set the category for logging to the specified 'CATEGORY' (assumed to be
//:     of type convertible to 'const char *').  On the *first* invocation of
//:     this macro in a code block, the 'ball::Log::setCategory' method is
//:     invoked to retrieve the address of an appropriate category structure
//:     for its scope; subsequent invocations will use a cached address of the
//:     category.  (See the function-level documentation of
//:     'ball::Log::setCategory' for more information.)  Note that this macro
//:     must be used at block scope, and can be used at most once in any given
//:     block (or else a compiler diagnostic will result).
//:
//: 'BALL_LOG_SET_DYNAMIC_CATEGORY(CATEGORY)':
//:      Set, *on EACH invocation*, the category for logging to the specified
//:     'CATEGORY' (assumed to be of type convertible to 'const char *').  On
//:     *EVERY* invocation of this macro in a code block, the
//:     'ball::Log::setCategory' method is invoked to retrieve the address of
//:     an appropriate category structure for its scope; the address returned
//:     from 'ball::Log::setCategory' is *NOT* cached for subsequent calls.
//:     (See the function-level documentation of 'ball::Log::setCategory' for
//:     more information.)  Note that this macro should be used to create
//:     categories that depend on *RUN-TIME* values only (e.g., LUW or UUID).
//:     Also note that this macro must be used at block scope and can be used
//:     at most once in any given block (or else a compiler diagnostic will
//:     result).
//
// Note that there can be at most one use of either 'BALL_LOG_SET_CATEGORY' or
// 'BALL_LOG_SET_DYNAMIC_CATEGORY' in any given block (or else a compiler
// diagnostic will result).  Also note that categories that are set, including
// dynamic categories, are not destroyed until process termination.
//
// The seven macros based on C++ streams that are most commonly used,
// 'BALL_LOG_TRACE', 'BALL_LOG_DEBUG', 'BALL_LOG_INFO', 'BALL_LOG_WARN',
// 'BALL_LOG_ERROR', 'BALL_LOG_FATAL', and 'BALL_LOG_END', have the following
// usage pattern:
//..
//  BALL_LOG_TRACE << X << Y ... << BALL_LOG_END
//  BALL_LOG_DEBUG << X << Y ... << BALL_LOG_END
//  BALL_LOG_INFO  << X << Y ... << BALL_LOG_END
//  BALL_LOG_WARN  << X << Y ... << BALL_LOG_END
//  BALL_LOG_ERROR << X << Y ... << BALL_LOG_END
//  BALL_LOG_FATAL << X << Y ... << BALL_LOG_END
//      where X, Y, ... represents any sequence of values for which
//      'operator<<' is defined.  The resulting formatted message string is
//      logged with the severity indicated by the name of the initial macro
//      (e.g., 'BALL_LOG_TRACE' ... 'BALL_LOG_END' logs with severity
//      'ball::Severity::e_TRACE').  Note that the formatted string includes
//      the category and filename as established by the
//      'BALL_LOG_SET_CATEGORY' (or 'BALL_LOG_SET_DYNAMIC_CATEGORY') and
//      '__FILE__' macros, respectively.
//..
// Two closely-related macros that are also based on C++ streams,
// 'BALL_LOG_STREAM' and 'BALL_LOG_STREAM_UNLIKELY', require that the severity
// be explicitly indicated.  They have the following usage pattern:
//..
//  BALL_LOG_STREAM(severity)          << X << Y ... << BALL_LOG_END
//  BALL_LOG_STREAM_UNLIKELY(severity) << X << Y ... << BALL_LOG_END
//      where X, Y, ... represents any sequence of values for which
//      'operator<<' is defined.  The resulting formatted message string is
//      logged with the specified 'severity'.  'BALL_LOG_STREAM_UNLIKELY'
//      differs from 'BALL_LOG_STREAM' in that the former provides a hint to
//      the compiler that it is "unlikely" to be executed; it is intended for
//      special-purpose use only.  Note that 'BALL_LOG_TRACE', for example, is
//      equivalent to 'BALL_LOG_STREAM(ball::Severity::e_TRACE)'.  Also note
//      that the formatted string includes the category and filename as
//      established by the 'BALL_LOG_SET_CATEGORY' (or
//      'BALL_LOG_SET_DYNAMIC_CATEGORY') and '__FILE__' macros, respectively.
//..
// Seven other macros based on C++ streams, similar to 'BALL_LOG_TRACE', etc.,
// allow the caller to specify a "callback" function which is passed the
// 'ball::UserFields *' used to represent the user fields of a log record.  The
// callback is expected to populate these user fields according to the schema
// specified in the logger manager configuration.  'BALL_LOGCB_TRACE',
// 'BALL_LOGCB_DEBUG', 'BALL_LOGCB_INFO', 'BALL_LOGCB_WARN',
// 'BALL_LOGCB_ERROR', 'BALL_LOGCB_FATAL', and 'BALL_LOGCB_END', have the
// following usage pattern:
//..
//  BALL_LOGCB_TRACE(CB) << X << Y ... << BALL_LOGCB_END
//  BALL_LOGCB_DEBUG(CB) << X << Y ... << BALL_LOGCB_END
//  BALL_LOGCB_INFO(CB)  << X << Y ... << BALL_LOGCB_END
//  BALL_LOGCB_WARN(CB)  << X << Y ... << BALL_LOGCB_END
//  BALL_LOGCB_ERROR(CB) << X << Y ... << BALL_LOGCB_END
//  BALL_LOGCB_FATAL(CB) << X << Y ... << BALL_LOGCB_END
//      where X, Y, ... represents any sequence of values for which
//      'operator<<' is defined and 'CB' is a callback taking a
//      'ball::UserFields *' as an argument.  The resulting formatted message
//      string is logged with the severity indicated by the name of the
//      initial macro (e.g., 'BALL_LOGCB_ERROR' ... 'BALL_LOGCB_END' logs with
//      severity 'ball::Severity::e_ERROR').  The log record will contain the
//      'ball::UserFields' representing user fields as populated by 'CB'.
//      Note that the formatted string includes the category and filename as
//      established by the 'BALL_LOG_SET_CATEGORY' (or
//      'BALL_LOG_SET_DYNAMIC_CATEGORY') and '__FILE__' macros, respectively.
//      Note the callback supplied to the logging macro must match the
//      prototype 'void (*)(ball::UserFields *)'.
//..
// The remaining macros are based on 'printf'-style format specifications:
//..
//  BALL_LOG0_TRACE(MSG);
//  BALL_LOG0_DEBUG(MSG);
//  BALL_LOG0_INFO (MSG);
//  BALL_LOG0_WARN (MSG);
//  BALL_LOG0_ERROR(MSG);
//  BALL_LOG0_FATAL(MSG);
//      Log the specified 'MSG' (assumed to be of type convertible to
//      'const char *') with the severity indicated by the name of the macro
//      (e.g., 'BALL_LOG0_DEBUG' logs with severity
//      'ball::Severity::e_DEBUG').  Note that the formatted message string
//      includes the category and filename as established by the
//      'BALL_LOG_SET_CATEGORY' (or 'BALL_LOG_SET_DYNAMIC_CATEGORY') and
//      '__FILE__' macros, respectively.  Also note that each use of these
//      macros must be terminated by a ';'.
//
//  BALL_LOG1_TRACE(MSG, ARG1);
//  BALL_LOG1_DEBUG(MSG, ARG1);
//  BALL_LOG1_INFO (MSG, ARG1);
//  BALL_LOG1_WARN (MSG, ARG1);
//  BALL_LOG1_ERROR(MSG, ARG1);
//  BALL_LOG1_FATAL(MSG, ARG1);
//      Format the specified 'ARG1' according to the 'printf'-style format
//      specification in the specified 'MSG' (assumed to be of type convertible
//      to 'const char *') and log the resulting formatted message string with
//      the severity indicated by the name of the macro (e.g., 'BALL_LOG1_WARN'
//      logs with severity 'ball::Severity::e_WARN').  The behavior is
//      undefined unless 'ARG1' is compatible with its corresponding format
//      specification in 'MSG'.  Note that the formatted string includes the
//      category and filename as established by the 'BALL_LOG_SET_CATEGORY' (or
//      'BALL_LOG_SET_DYNAMIC_CATEGORY') and '__FILE__' macros, respectively.
//      Also note that each use of these macros must be terminated by a ';'.
//
//  BALL_LOGn_TRACE(MSG, ARG1, ARG2, ..., ARGn);  // 2 <= n <= 9
//  BALL_LOGn_DEBUG(MSG, ARG1, ARG2, ..., ARGn);
//  BALL_LOGn_INFO (MSG, ARG1, ARG2, ..., ARGn);
//  BALL_LOGn_WARN (MSG, ARG1, ARG2, ..., ARGn);
//  BALL_LOGn_ERROR(MSG, ARG1, ARG2, ..., ARGn);
//  BALL_LOGn_FATAL(MSG, ARG1, ARG2, ..., ARGn);
//      Format the specified 'ARG1', 'ARG2', ..., 'ARGn' (2 <= n <= 9)
//      according to the 'printf'-style format specification in the specified
//      'MSG' (assumed to be of type convertible to 'const char *') and log the
//      resulting formatted message string with the severity indicated by the
//      name of the macro (e.g., 'BALL_LOG8_FATAL' logs with severity
//      'ball::Severity::e_FATAL').  The behavior is undefined unless each
//      'ARGn' is compatible with its corresponding format specification in
//      'MSG'.  Note that the formatted string includes the category and
//      filename as established by the 'BALL_LOG_SET_CATEGORY' (or
//      'BALL_LOG_SET_DYNAMIC_CATEGORY') and '__FILE__' macros, respectively.
//      Also note that each use of these macros must be terminated by a ';'.
//
//  BALL_LOG_IS_ENABLED(BALL_SEVERITY)
//      Return 'true' if the specified 'BALL_SEVERITY' is more severe than any
//      of the threshold levels of the current context's logging category
//      (which must be established using 'BALL_LOG_SET_CATEGORY'), and 'false'
//      otherwise.
//..
//
///Usage
///-----
// The following code fragments illustrate the standard pattern of macro usage.
//
///Example 1: A Basic Logging Example
/// - - - - - - - - - - - - - - - - -
// The following trivial example shows how to use the logging macros to log
// messages at various levels of severity.
//..
//  void exampleFunction()
//  {
//..
// We start by initializing the log category within the context of this
// function.  The logging macros such as 'BALL_LOG_ERROR' will not compile
// unless a category has been specified in the current lexical scope.
//..
//      BALL_LOG_SET_CATEGORY("EXAMPLE.CATEGORY");
//..
// Now we record messages at various levels of severity.  These messages will
// (or will not) be written to the log depending on the current logging
// threshold of the category (configured using the 'ball::LoggerManager'
// singleton).
//..
//      BALL_LOG_FATAL << "Write this message to the log if the log threshold "
//                     << "is above 'ball::Severity::FATAL' (i.e., 32)."
//                     << BALL_LOG_END;
//
//      BALL_LOG_TRACE << "Write this message to the log if the log threshold "
//                     << "is above 'ball::Severity::FATAL' (i.e., 192)."
//                     << BALL_LOG_END;
//  }
//..
// Note that failing to match the start of a logged message with an
// appropriate 'BALL_LOG_END' will result in a compilation error.
//
///Example 2: Setting the Current Log Category
///- - - - - - - - - - - - - - - - - - - - - -
// This example provides more detail on setting the log category in the
// current lexical scope.  The following macro instantiation sets the category
// for logging to be "EQUITY.NASD" in the enclosing lexical scope:
//..
//      BALL_LOG_SET_CATEGORY("EQUITY.NASD")
//..
// Note that this macro must not be used at file scope and it can be used
// at most once in any given block (or else a compiler diagnostic will result).
// A different category may be established to override one that is in effect,
// but it must occur in a nested scope.  In any case, a use of this macro (or
// of 'BALL_LOG_SET_DYNAMIC_CATEGORY') must be visible from within the lexical
// scope of every use of the log-generating macros.  The following fragment of
// code shows how to set a different category in a nested inner block that
// hides a category set in an enclosing block:
//..
//      void logIt()
//      {
//          BALL_LOG_SET_CATEGORY("EQUITY.NASD")
//
//          // Logging to category "EQUITY.NASD" unless overridden in a
//          // nested block.
//          // [*] ...
//
//          {
//              // [*] ...
//              // Still logging to category "EQUITY.NASD".
//
//              BALL_LOG_SET_CATEGORY("EQUITY.NASD.SUNW")
//
//              // Now logging to category "EQUITY.NASD.SUNW".
//              // [*] ...
//          }
//          // Again logging to category "EQUITY.NASD".
//          // [*] ...
//      }
//..
// Within 'logIt', the required logging context is in place at each of the
// locations marked by [*].
//
///Example 3: 'printf'-Style Versus C++ I/O Streams-Style Logging Macros
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// In the following example, we expand the 'logIt' function (defined above)
// to log two messages using the logging macros provided by this component.
// The first logging macro we use, 'BALL_LOG3_INFO' takes a format string and a
// series of variables, similar to 'printf'.  The second logging macro we use,
// 'BALL_LOG_TRACE', is provided input using the C++ stream operator '<<'.
//..
//      void logIt()
//      {
//          int     lotSize = 400;
//          char   *ticker  = "SUNW";
//          double  price   = 5.65;
//
//          BALL_LOG_SET_CATEGORY("EQUITY.NASD")
//
//          // Logging to category "EQUITY.NASD" unless overridden in a
//          // nested block.
//
//          BALL_LOG3_INFO("%d shares of %s sold at %f\n",
//                         lotSize, ticker, price);
//          {
//              BALL_LOG_SET_CATEGORY("EQUITY.NASD.SUNW")
//
//              // Now logging to category "EQUITY.NASD.SUNW".
//
//              BloombergLP::bdlt::Datetime now;
//              BloombergLP::bdlt::EpochUtil::convertFromTimeT(&now, time(0));
//
//              BALL_LOG_INFO << now << ": " << lotSize << " shares of "
//                            << ticker << " sold at " << price
//                            << BALL_LOG_END
//          }
//          // Again logging to category "EQUITY.NASD".
//      }
//..
// The 'BALL_LOG3_INFO' macro logs the following message:
//..
//      400 shares of SUNW sold at 5.650000
//..
// to category "EQUITY.NASD.SUNW" at severity level 'ball::Severity::e_INFO'.
// Note that the first argument supplied to the 'BALL_LOG3_INFO' macro is a
// 'printf'-style format specification; the "3" in the macro name indicates the
// number of additional arguments that must be supplied to 'BALL_LOG3_INFO'
// and which must conform to the format specification.  The log record that is
// generated for this message will include the file name as indicated by the
// '__FILE__' macro and the line number of the line on which the use of
// 'BALL_LOG3_INFO' occurs.
//
// The 'BALL_LOG_INFO' macro formats a message using standard C++ streaming
// facilities rather than a 'printf'-style format specification.  The use of
// this macro in 'logIt' logs the following message:
//..
//      10FEB2004_13:29:49.000: 400 shares of SUNW sold at 5.65
//..
// The current timestamp ('now') was prepended to the message as a simple
// illustration of the added flexibility provided by the C++ stream-based
// macros.  (Note that the current timestamp is automatically included as a
// distinct field in all log records generated by the 'ball' logging system.)
// This latter message is logged to category "EQUITY.NASD.SUNW" at severity
// level 'ball::Severity::e_TRACE'.  The log record that is generated for
// this message will include the same file name as for 'BALL_LOG3_INFO', but
// will naturally have a different line number associated with it.
//
// The C++ stream-based macros, as opposed to the 'printf'-style macros, ensure
// at compile-time that no run-time format mismatches will occur.  Use of the
// stream-based logging style exclusively will likely lead to clearer, more
// maintainable code with fewer initial defects.
//
// Note that all uses of the log-generating macros *must* occur within function
// scope (i.e., not at file scope).
//
///Example 4: Dynamic Categories
///- - - - - - - - - - - - - - -
// Logging must sometimes be controlled by parameters that are not available
// until run-time.  The 'BALL_LOG_SET_DYNAMIC_CATEGORY' macro sets a category
// each time it is invoked (unlike 'BALL_LOG_SET_CATEGORY', which sets a
// category only on the first invocation and uses the cached address of the
// category on subsequent invocations).  The category name in the following
// 'processSecurity' function is a combination of a static prefix and the
// (dynamic) 'exchange' argument:
//..
//  void processSecurity(const char *security, const char *exchange)
//  {
//      bsl::string categoryName("EXCHANGE:");
//      categoryName.append(exchange);
//
//      BALL_LOG_SET_DYNAMIC_CATEGORY(categoryName.c_str());
//
//      BALL_LOG_TRACE << "processing: " << security << BALL_LOG_END;
//
//      // ...
//  }
//..
// Now logging can be controlled independently for each 'exchange'.
//
// *WARNING*: Along with the added flexibility provided by dynamic categories
// comes the additional overhead of computing and setting a category on each
// invocation.  Consequently, dynamic categories should be used *SPARINGLY* in
// most applications.
//
///Example 5: Rule-Based Logging
///- - - - - - - - - - - - - - -
// The following example demonstrates the use of attributes and rules to
// conditionally enable logging.
//
// We start by defining a function, 'processData', that is passed data in a
// 'vector<char>' and information about the user who sent the data.  This
// example function performs no actual processing, but does log a single
// message at the 'ball::Severity::DEBUG' threshold level.  The 'processData'
// function also adds the user information passed to this function to the
// thread's attribute context.  We will use these attributes later, to create a
// logging rule that enables verbose logging only for a particular user.
//..
//  void processData(int                      uuid,
//                   int                      luw,
//                   int                      terminalNumber,
//                   const bsl::vector<char>& data)
//      // Process the specified 'data' associated with the specified bloomberg
//      // 'uuid', 'luw', and 'terminalNumber'.
//  {
//..
// We create a generic "default" attribute container to hold our attributes.
// Note that, in practice, we might choose to define a more efficient
// implementation of the 'ball::AttributeContainer' protocol specifically for
// these three attributes, uuid, luw, and firmNumber.  See the
// 'ball::attributeContainer' component documentation for an example.
//..
//      ball::DefaultAttributeContainer attributes;
//      attributes.addAttribute(ball::Attribute("uuid", uuid));
//      attributes.addAttribute(ball::Attribute("luw", luw));
//      attributes.addAttribute(ball::Attribute("terminalNumber",
//                                              terminalNumber);
//
//      ball::AttributeContext *context = ball::AttributeContext::getContext();
//      ball::AttributeContext::iterator it =
//                                         context->addAttributes(&attributes);
//..
// In this simplified example we perform no actual processing, and simply log
// a message at the 'ball::Severity::DEBUG' level.
//..
//      BALL_LOG_SET_CATEGORY("EXAMPLE.CATEGORY");
//
//      BALL_LOG_DEBUG << "An example message" << BALL_LOG_END;
//..
// Because 'attributes' is defined on this thread's stack, it must be removed
// from this thread's attribute context before exiting the function (the
// 'ball_scopedattributes' component provides a proctor for this purpose).
//..
//      context->removeAttributes(it);
//  }
//..
// Next we demonstrate how to create a logging rule that sets the pass-through
// logging threshold to 'ball::Severity::TRACE' (i.e., enables verbose logging)
// for a particular user when calling the 'processData' function defined
// above.
//
// We start by creating the singleton logger manager, which we configure with
// the default observer and a default configuration.  We then call the
// 'processData' function: This first call to 'processData' will not result in
// any logged messages because 'processData' logs its message at the
// 'ball::Severity::DEBUG' level, which is below the default configured logging
// threshold.
//..
//  ball::DefaultObserver observer(&bsl::cout);
//  ball::LoggerManagerConfiguration configuration;
//  ball::LoggerManagerScopedGuard lmg(&observer, configuration);
//
//  BALL_LOG_SET_CATEGORY("EXAMPLE.CATEGORY");
//
//  bsl::vector<char> message;
//
//  BALL_LOG_ERROR << "Processing the first message." << BALL_LOG_END;
//  processData(3938908, 2, 9001, message);
//..
// Now we add a logging rule, setting the pass-through threshold to be
// 'ball::Severity::TRACE' (i.e., enabling verbose logging) if the thread's
// context contains a "uuid" of '3938908'.  Note that we use the wild-card
// value '*' for the category so that the 'ball::Rule' rule will apply to all
// categories.
//..
//  ball::Rule rule("*", 0, ball::Severity::TRACE, 0, 0);
//  rule.addPredicate(ball::Predicate("uuid", 3938908));
//  ball::LoggerManager::singleton().addRule(rule);
//
//  BALL_LOG_ERROR << "Processing the second message." << BALL_LOG_END;
//  processData(3938908, 2, 9001, message);
//..
// The final call to the 'processData' function below, passes a "uuid" of
// '2171395' (not '3938908') so the logging rule we defined will *not* apply
// and no message will be logged.
//..
//  BALL_LOG_ERROR << "Processing the third message." << BALL_LOG_END;
//  processData(2171395, 2, 9001, message);
//..
// The resulting logged output for this example looks like the following:
//..
// ERROR example.cpp:105 EXAMPLE.CATEGORY Processing the first message.
// ERROR example.cpp:117 EXAMPLE.CATEGORY Processing the second message.
// DEBUG example.cpp:35 EXAMPLE.CATEGORY An example message
// ERROR example.cpp:129 EXAMPLE.CATEGORY Processing the third message.
//..
//
///Example 6: Logging Using a Callback
///- - - - - - - - - - - - - - - - - -
// The following example demonstrates how to register a logging callback.  The
// C++ stream-based macros that take a callback are particularly useful to
// seamlessly populate the user fields of a record, thus simplying the
// logging line.
//
// We define a callback function 'populateUsingPoint' that appends to the
// specified 'fields' the attributes of the 'point' to log:
//..
//  void populateUsingPoint(ball::UserFields *fields, const Point& point)
//      // Append to the specified 'list' the name, x value, and y value of
//      // the specified 'point'.
//  {
//      fields->appendString(point.name());
//      fields->appendInt64(point.x());
//      fields->appendInt64(point.y());
//  }
//
//  int validatePoint(const Point& point)
//  {
//      BALL_LOG_SET_CATEGORY("EXAMPLE.CATEGORY");
//..
// We now bind our callback function 'populateUsingPoint' and the supplied
// 'point' to a functor object we will pass to the logging callback.  Note
// that the callback supplied to the logging macro must match the prototype
// 'void (*)(ball::UserFields *)'.
//..
//      bsl::function <void(ball::UserFields *)> callback;
//      callback = bdlf::BindUtil::bind(&populateUsingPoint,
//                                     bdlf::PlaceHolders::_1,
//                                     point);
//
//      int numErrors = 0;
//      if (point.x() > 255) {
//          BALL_LOGCB_ERROR(callback) << "X > 255"  << BALL_LOGCB_END
//          ++numErrors;
//      }
//      if (point.x() < -255) {
//          BALL_LOGCB_ERROR(callback) << "X < -255" << BALL_LOGCB_END
//          ++numErrors;
//      }
//      if (point.y() > 255) {
//          BALL_LOGCB_ERROR(callback) << "Y > 255"  << BALL_LOGCB_END
//          ++numErrors;
//      }
//      if (point.y() < -255) {
//          BALL_LOGCB_ERROR(callback) << "Y < -255" << BALL_LOGCB_END
//          ++numErrors;
//      }
//      return numErrors;
//  }
//..

#ifndef INCLUDED_BALSCM_VERSION
#include <balscm_version.h>
#endif

#ifndef INCLUDED_BALL_CATEGORYMANAGER
#include <ball_categorymanager.h>
#endif

#ifndef INCLUDED_BALL_LOGGERMANAGER
#include <ball_loggermanager.h>
#endif

#ifndef INCLUDED_BALL_SEVERITY
#include <ball_severity.h>
#endif

#ifndef INCLUDED_BSLS_PERFORMANCEHINT
#include <bsls_performancehint.h>
#endif

#ifndef INCLUDED_BSLS_PLATFORM
#include <bsls_platform.h>
#endif

#ifndef INCLUDED_BSL_OSTREAM
#include <bsl_ostream.h>
#endif

                       // =========================
                       // Logging Macro Definitions
                       // =========================

#define BALL_LOG_CATEGORY (BALL_LOG_CATEGORYHOLDER.category())

#define BALL_LOG_THRESHOLD (BALL_LOG_CATEGORYHOLDER.threshold())

#define BALL_RECORD (ball_lOcAl_StReAm.record())

#define BALL_STREAM (ball_lOcAl_StReAm.stream())

#define BALL_LOG_SET_CATEGORY(CATEGORY)                                       \
    static BloombergLP::ball::CategoryHolder BALL_LOG_CATEGORYHOLDER = {      \
        BloombergLP::ball::CategoryHolder::e_UNINITIALIZED_CATEGORY, 0, 0     \
    };                                                                        \
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!BALL_LOG_CATEGORY)) {          \
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;                                   \
        BloombergLP::ball::Log::setCategory(&BALL_LOG_CATEGORYHOLDER,         \
                                           CATEGORY);                         \
    }

#define BALL_LOG_SET_DYNAMIC_CATEGORY(CATEGORY)                               \
    const BloombergLP::ball::Category *BALL_LOG_DYNAMIC_CATEGORY =            \
                             BloombergLP::ball::Log::setCategory(CATEGORY);   \
    BloombergLP::ball::CategoryHolder BALL_LOG_CATEGORYHOLDER = {             \
        BloombergLP::ball::CategoryHolder::e_DYNAMIC_CATEGORY,                \
        const_cast<BloombergLP::ball::Category *>(BALL_LOG_DYNAMIC_CATEGORY), \
        0 \
    };

                       // =======================
                       // C++ stream-based macros
                       // =======================

#define BALL_LOG_STREAM(BALL_SEVERITY) {                                      \
    using namespace BloombergLP;                                              \
    if (BALL_LOG_THRESHOLD >= (BALL_SEVERITY)) {                              \
        if (ball::Log::isCategoryEnabled(&BALL_LOG_CATEGORYHOLDER,            \
                                        BALL_SEVERITY)) {                     \
            ball::Log_Stream ball_lOcAl_StReAm(BALL_LOG_CATEGORY, __FILE__,   \
                                              __LINE__, BALL_SEVERITY);       \
            BALL_STREAM

#define BALL_LOG_STREAM_UNLIKELY(BALL_SEVERITY) {                             \
    using namespace BloombergLP;                                              \
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(                                \
                                 BALL_LOG_THRESHOLD >= (BALL_SEVERITY))) {    \
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;                                   \
        if (ball::Log::isCategoryEnabled(&BALL_LOG_CATEGORYHOLDER,            \
                                        BALL_SEVERITY)) {                     \
            ball::Log_Stream ball_lOcAl_StReAm(BALL_LOG_CATEGORY, __FILE__,   \
                                              __LINE__, BALL_SEVERITY);       \
            BALL_STREAM

// We expect TRACE and DEBUG messages not to be logged.

#define BALL_LOG_TRACE BALL_LOG_STREAM_UNLIKELY(ball::Severity::e_TRACE)

#define BALL_LOG_DEBUG BALL_LOG_STREAM_UNLIKELY(ball::Severity::e_DEBUG)

#define BALL_LOG_INFO  BALL_LOG_STREAM(ball::Severity::e_INFO)

#define BALL_LOG_WARN  BALL_LOG_STREAM(ball::Severity::e_WARN)

#define BALL_LOG_ERROR BALL_LOG_STREAM(ball::Severity::e_ERROR)

#define BALL_LOG_FATAL BALL_LOG_STREAM(ball::Severity::e_FATAL)

// We indirectly define the macro for ending a 'BALL_LOG_*' block because the
// 'BALL_LOG_[LEVEL]' macros that most often begin such a block (e.g.,
// 'BALL_LOG_ERROR') indirectly forward to 'BALL_LOG_STREAM'.  This symmetry
// enables Microsoft intellisense to reasonably parse code following a use of
// this macro.

#define BALL_LOG_REAL_END bsl::ends;                                          \
        }                                                                     \
    }                                                                         \
}
#define BALL_LOG_END BALL_LOG_REAL_END

               // ========================================
               // C++ stream-based macros using a callback
               // ========================================

#define BALL_LOGCB_STREAM(BALL_SEVERITY, CB) {                                \
    using namespace BloombergLP;                                              \
    if (BALL_LOG_THRESHOLD >= (BALL_SEVERITY)) {                              \
        if (ball::Log::isCategoryEnabled(&BALL_LOG_CATEGORYHOLDER,            \
                                        BALL_SEVERITY)) {                     \
            ball::Log_Stream ball_lOcAl_StReAm(BALL_LOG_CATEGORY, __FILE__,   \
                                              __LINE__, BALL_SEVERITY);       \
            CB(&ball_lOcAl_StReAm.record()->userFields());                    \
            BALL_STREAM

#define BALL_LOGCB_TRACE(CB) BALL_LOGCB_STREAM(ball::Severity::e_TRACE, CB)

#define BALL_LOGCB_DEBUG(CB) BALL_LOGCB_STREAM(ball::Severity::e_DEBUG, CB)

#define BALL_LOGCB_INFO(CB) BALL_LOGCB_STREAM(ball::Severity::e_INFO, CB)

#define BALL_LOGCB_WARN(CB) BALL_LOGCB_STREAM(ball::Severity::e_WARN, CB)

#define BALL_LOGCB_ERROR(CB) BALL_LOGCB_STREAM(ball::Severity::e_ERROR, CB)

#define BALL_LOGCB_FATAL(CB) BALL_LOGCB_STREAM(ball::Severity::e_FATAL, CB)

// We indirectly define the macro for ending a 'BALL_LOGCB_*' block because the
// 'BALL_LOGCB_[LEVEL]' macros that most often begin such a block (e.g.,
// 'BALL_LOGCB_ERROR') indirectly forward to 'BALL_LOGCB_STREAM'.  This
// symmetry enables Microsoft intellisense to reasonably parse code following a
// use of this macro.

#define BALL_LOGCB_REAL_END bsl::ends;                                     \
        }                                                                  \
    }                                                                      \
}
#define BALL_LOGCB_END BALL_LOGCB_REAL_END

                       // =====================
                       // 'printf'-style macros
                       // =====================

#define BALL_LOG0(BALL_SEVERITY, MSG)                                      \
do {                                                                       \
    using namespace BloombergLP;                                           \
    if (BALL_LOG_THRESHOLD >= (BALL_SEVERITY)) {                           \
        if (ball::Log::isCategoryEnabled(&BALL_LOG_CATEGORYHOLDER,         \
                                        BALL_SEVERITY)) {                  \
            ball::Log_Formatter ball_lOcAl_FoRmAtTeR(BALL_LOG_CATEGORY,    \
                                                    __FILE__, __LINE__,    \
                                                    BALL_SEVERITY);        \
            ball::Log::format(ball_lOcAl_FoRmAtTeR.messageBuffer(),        \
                             ball_lOcAl_FoRmAtTeR.messageBufferLen(),      \
                             MSG);                                         \
        }                                                                  \
    }                                                                      \
} while(0)

#define BALL_LOG1(BALL_SEVERITY, MSG, ARG1)                                \
do {                                                                       \
    using namespace BloombergLP;                                           \
    if (BALL_LOG_THRESHOLD >= (BALL_SEVERITY)) {                           \
        if (ball::Log::isCategoryEnabled(&BALL_LOG_CATEGORYHOLDER,         \
                                        BALL_SEVERITY)) {                  \
            ball::Log_Formatter ball_lOcAl_FoRmAtTeR(BALL_LOG_CATEGORY,    \
                                                    __FILE__, __LINE__,    \
                                                    BALL_SEVERITY);        \
            ball::Log::format(ball_lOcAl_FoRmAtTeR.messageBuffer(),        \
                             ball_lOcAl_FoRmAtTeR.messageBufferLen(),      \
                             MSG, ARG1);                                   \
        }                                                                  \
    }                                                                      \
} while(0)

#define BALL_LOG2(BALL_SEVERITY, MSG, ARG1, ARG2)                          \
do {                                                                       \
    using namespace BloombergLP;                                           \
    if (BALL_LOG_THRESHOLD >= (BALL_SEVERITY)) {                           \
        if (ball::Log::isCategoryEnabled(&BALL_LOG_CATEGORYHOLDER,         \
                                        BALL_SEVERITY)) {                  \
            ball::Log_Formatter ball_lOcAl_FoRmAtTeR(BALL_LOG_CATEGORY,    \
                                                    __FILE__, __LINE__,    \
                                                    BALL_SEVERITY);        \
            ball::Log::format(ball_lOcAl_FoRmAtTeR.messageBuffer(),        \
                             ball_lOcAl_FoRmAtTeR.messageBufferLen(),      \
                             MSG, ARG1, ARG2);                             \
        }                                                                  \
    }                                                                      \
} while(0)

#define BALL_LOG3(BALL_SEVERITY, MSG, ARG1, ARG2, ARG3)                    \
do {                                                                       \
    using namespace BloombergLP;                                           \
    if (BALL_LOG_THRESHOLD >= (BALL_SEVERITY)) {                           \
        if (ball::Log::isCategoryEnabled(&BALL_LOG_CATEGORYHOLDER,         \
                                        BALL_SEVERITY)) {                  \
            ball::Log_Formatter ball_lOcAl_FoRmAtTeR(BALL_LOG_CATEGORY,    \
                                                    __FILE__, __LINE__,    \
                                                    BALL_SEVERITY);        \
            ball::Log::format(ball_lOcAl_FoRmAtTeR.messageBuffer(),        \
                             ball_lOcAl_FoRmAtTeR.messageBufferLen(),      \
                             MSG, ARG1, ARG2, ARG3);                       \
        }                                                                  \
    }                                                                      \
} while(0)

#define BALL_LOG4(BALL_SEVERITY, MSG, ARG1, ARG2, ARG3, ARG4)              \
do {                                                                       \
    using namespace BloombergLP;                                           \
    if (BALL_LOG_THRESHOLD >= (BALL_SEVERITY)) {                           \
        if (ball::Log::isCategoryEnabled(&BALL_LOG_CATEGORYHOLDER,         \
                                        BALL_SEVERITY)) {                  \
            ball::Log_Formatter ball_lOcAl_FoRmAtTeR(BALL_LOG_CATEGORY,    \
                                                    __FILE__, __LINE__,    \
                                                    BALL_SEVERITY);        \
            ball::Log::format(ball_lOcAl_FoRmAtTeR.messageBuffer(),        \
                             ball_lOcAl_FoRmAtTeR.messageBufferLen(),      \
                             MSG, ARG1, ARG2, ARG3, ARG4);                 \
        }                                                                  \
    }                                                                      \
} while(0)

#define BALL_LOG5(BALL_SEVERITY, MSG, ARG1, ARG2, ARG3, ARG4, ARG5)        \
do {                                                                       \
    using namespace BloombergLP;                                           \
    if (BALL_LOG_THRESHOLD >= (BALL_SEVERITY)) {                           \
        if (ball::Log::isCategoryEnabled(&BALL_LOG_CATEGORYHOLDER,         \
                                        BALL_SEVERITY)) {                  \
            ball::Log_Formatter ball_lOcAl_FoRmAtTeR(BALL_LOG_CATEGORY,    \
                                                    __FILE__, __LINE__,    \
                                                    BALL_SEVERITY);        \
            ball::Log::format(ball_lOcAl_FoRmAtTeR.messageBuffer(),        \
                             ball_lOcAl_FoRmAtTeR.messageBufferLen(),      \
                             MSG, ARG1, ARG2, ARG3, ARG4, ARG5);           \
        }                                                                  \
    }                                                                      \
} while(0)

#define BALL_LOG6(BALL_SEVERITY, MSG, ARG1, ARG2, ARG3, ARG4, ARG5, ARG6)  \
do {                                                                       \
    using namespace BloombergLP;                                           \
    if (BALL_LOG_THRESHOLD >= (BALL_SEVERITY)) {                           \
        if (ball::Log::isCategoryEnabled(&BALL_LOG_CATEGORYHOLDER,         \
                                        BALL_SEVERITY)) {                  \
            ball::Log_Formatter ball_lOcAl_FoRmAtTeR(BALL_LOG_CATEGORY,    \
                                                    __FILE__, __LINE__,    \
                                                    BALL_SEVERITY);        \
            ball::Log::format(ball_lOcAl_FoRmAtTeR.messageBuffer(),        \
                             ball_lOcAl_FoRmAtTeR.messageBufferLen(),      \
                             MSG, ARG1, ARG2, ARG3, ARG4, ARG5, ARG6);     \
        }                                                                  \
    }                                                                      \
} while(0)

#define BALL_LOG7(BALL_SEVERITY, MSG, ARG1, ARG2, ARG3, ARG4, ARG5, ARG6,  \
                                      ARG7)                                \
do {                                                                       \
    using namespace BloombergLP;                                           \
    if (BALL_LOG_THRESHOLD >= (BALL_SEVERITY)) {                           \
        if (ball::Log::isCategoryEnabled(&BALL_LOG_CATEGORYHOLDER,         \
                                        BALL_SEVERITY)) {                  \
            ball::Log_Formatter ball_lOcAl_FoRmAtTeR(BALL_LOG_CATEGORY,    \
                                                    __FILE__, __LINE__,    \
                                                    BALL_SEVERITY);        \
            ball::Log::format(ball_lOcAl_FoRmAtTeR.messageBuffer(),        \
                             ball_lOcAl_FoRmAtTeR.messageBufferLen(),      \
                             MSG, ARG1, ARG2, ARG3, ARG4, ARG5, ARG6,      \
                             ARG7);                                        \
        }                                                                  \
    }                                                                      \
} while(0)

#define BALL_LOG8(BALL_SEVERITY, MSG, ARG1, ARG2, ARG3, ARG4, ARG5, ARG6,  \
                                      ARG7, ARG8)                          \
do {                                                                       \
    using namespace BloombergLP;                                           \
    if (BALL_LOG_THRESHOLD >= (BALL_SEVERITY)) {                           \
        if (ball::Log::isCategoryEnabled(&BALL_LOG_CATEGORYHOLDER,         \
                                        BALL_SEVERITY)) {                  \
            ball::Log_Formatter ball_lOcAl_FoRmAtTeR(BALL_LOG_CATEGORY,    \
                                                    __FILE__, __LINE__,    \
                                                    BALL_SEVERITY);        \
            ball::Log::format(ball_lOcAl_FoRmAtTeR.messageBuffer(),        \
                             ball_lOcAl_FoRmAtTeR.messageBufferLen(),      \
                             MSG, ARG1, ARG2, ARG3, ARG4, ARG5, ARG6,      \
                             ARG7, ARG8);                                  \
        }                                                                  \
    }                                                                      \
} while(0)

#define BALL_LOG9(BALL_SEVERITY, MSG, ARG1, ARG2, ARG3, ARG4, ARG5, ARG6,  \
                                      ARG7, ARG8, ARG9)                    \
do {                                                                       \
    using namespace BloombergLP;                                           \
    if (BALL_LOG_THRESHOLD >= (BALL_SEVERITY)) {                           \
        if (ball::Log::isCategoryEnabled(&BALL_LOG_CATEGORYHOLDER,         \
                                        BALL_SEVERITY)) {                  \
            ball::Log_Formatter ball_lOcAl_FoRmAtTeR(BALL_LOG_CATEGORY,    \
                                                    __FILE__, __LINE__,    \
                                                    BALL_SEVERITY);        \
            ball::Log::format(ball_lOcAl_FoRmAtTeR.messageBuffer(),        \
                             ball_lOcAl_FoRmAtTeR.messageBufferLen(),      \
                             MSG, ARG1, ARG2, ARG3, ARG4, ARG5, ARG6,      \
                             ARG7, ARG8, ARG9);                            \
        }                                                                  \
    }                                                                      \
} while(0)

#define BALL_LOG0_TRACE(MSG)                                               \
    BALL_LOG0(ball::Severity::e_TRACE, MSG)

#define BALL_LOG1_TRACE(MSG, ARG1)                                         \
    BALL_LOG1(ball::Severity::e_TRACE, MSG, ARG1)

#define BALL_LOG2_TRACE(MSG, ARG1, ARG2)                                   \
    BALL_LOG2(ball::Severity::e_TRACE, MSG, ARG1, ARG2)

#define BALL_LOG3_TRACE(MSG, ARG1, ARG2, ARG3)                             \
    BALL_LOG3(ball::Severity::e_TRACE, MSG, ARG1, ARG2, ARG3)

#define BALL_LOG4_TRACE(MSG, ARG1, ARG2, ARG3, ARG4)                       \
    BALL_LOG4(ball::Severity::e_TRACE, MSG, ARG1, ARG2, ARG3, ARG4)

#define BALL_LOG5_TRACE(MSG, ARG1, ARG2, ARG3, ARG4, ARG5)                 \
    BALL_LOG5(ball::Severity::e_TRACE, MSG, ARG1, ARG2, ARG3, ARG4, ARG5)

#define BALL_LOG6_TRACE(MSG, ARG1, ARG2, ARG3, ARG4, ARG5, ARG6)           \
    BALL_LOG6(ball::Severity::e_TRACE, MSG, ARG1, ARG2, ARG3, ARG4,        \
                                              ARG5, ARG6)

#define BALL_LOG7_TRACE(MSG, ARG1, ARG2, ARG3, ARG4, ARG5, ARG6, ARG7)     \
    BALL_LOG7(ball::Severity::e_TRACE, MSG, ARG1, ARG2, ARG3, ARG4,        \
                                              ARG5, ARG6, ARG7)

#define BALL_LOG8_TRACE(MSG, ARG1, ARG2, ARG3, ARG4, ARG5, ARG6, ARG7,     \
                             ARG8)                                         \
    BALL_LOG8(ball::Severity::e_TRACE, MSG, ARG1, ARG2, ARG3, ARG4,        \
                                              ARG5, ARG6, ARG7, ARG8)

#define BALL_LOG9_TRACE(MSG, ARG1, ARG2, ARG3, ARG4, ARG5, ARG6, ARG7,     \
                             ARG8, ARG9)                                   \
    BALL_LOG9(ball::Severity::e_TRACE, MSG, ARG1, ARG2, ARG3, ARG4,        \
                                              ARG5, ARG6, ARG7, ARG8, ARG9)

#define BALL_LOG0_DEBUG(MSG)                                               \
    BALL_LOG0(ball::Severity::e_DEBUG, MSG)

#define BALL_LOG1_DEBUG(MSG, ARG1)                                         \
    BALL_LOG1(ball::Severity::e_DEBUG, MSG, ARG1)

#define BALL_LOG2_DEBUG(MSG, ARG1, ARG2)                                   \
    BALL_LOG2(ball::Severity::e_DEBUG, MSG, ARG1, ARG2)

#define BALL_LOG3_DEBUG(MSG, ARG1, ARG2, ARG3)                             \
    BALL_LOG3(ball::Severity::e_DEBUG, MSG, ARG1, ARG2, ARG3)

#define BALL_LOG4_DEBUG(MSG, ARG1, ARG2, ARG3, ARG4)                       \
    BALL_LOG4(ball::Severity::e_DEBUG, MSG, ARG1, ARG2, ARG3, ARG4)

#define BALL_LOG5_DEBUG(MSG, ARG1, ARG2, ARG3, ARG4, ARG5)                 \
    BALL_LOG5(ball::Severity::e_DEBUG, MSG, ARG1, ARG2, ARG3, ARG4, ARG5)

#define BALL_LOG6_DEBUG(MSG, ARG1, ARG2, ARG3, ARG4, ARG5, ARG6)           \
    BALL_LOG6(ball::Severity::e_DEBUG, MSG, ARG1, ARG2, ARG3, ARG4,        \
                                              ARG5, ARG6)

#define BALL_LOG7_DEBUG(MSG, ARG1, ARG2, ARG3, ARG4, ARG5, ARG6, ARG7)     \
    BALL_LOG7(ball::Severity::e_DEBUG, MSG, ARG1, ARG2, ARG3, ARG4,        \
                                              ARG5, ARG6, ARG7)

#define BALL_LOG8_DEBUG(MSG, ARG1, ARG2, ARG3, ARG4, ARG5, ARG6, ARG7,     \
                             ARG8)                                         \
    BALL_LOG8(ball::Severity::e_DEBUG, MSG, ARG1, ARG2, ARG3, ARG4,        \
                                              ARG5, ARG6, ARG7, ARG8)

#define BALL_LOG9_DEBUG(MSG, ARG1, ARG2, ARG3, ARG4, ARG5, ARG6, ARG7,     \
                             ARG8, ARG9)                                   \
    BALL_LOG9(ball::Severity::e_DEBUG, MSG, ARG1, ARG2, ARG3, ARG4,        \
                                              ARG5, ARG6, ARG7, ARG8, ARG9)

#define BALL_LOG0_INFO(MSG)                                                \
    BALL_LOG0(ball::Severity::e_INFO, MSG)

#define BALL_LOG1_INFO(MSG, ARG1)                                          \
    BALL_LOG1(ball::Severity::e_INFO, MSG, ARG1)

#define BALL_LOG2_INFO(MSG, ARG1, ARG2)                                    \
    BALL_LOG2(ball::Severity::e_INFO, MSG, ARG1, ARG2)

#define BALL_LOG3_INFO(MSG, ARG1, ARG2, ARG3)                              \
    BALL_LOG3(ball::Severity::e_INFO, MSG, ARG1, ARG2, ARG3)

#define BALL_LOG4_INFO(MSG, ARG1, ARG2, ARG3, ARG4)                        \
    BALL_LOG4(ball::Severity::e_INFO, MSG, ARG1, ARG2, ARG3, ARG4)

#define BALL_LOG5_INFO(MSG, ARG1, ARG2, ARG3, ARG4, ARG5)                  \
    BALL_LOG5(ball::Severity::e_INFO, MSG, ARG1, ARG2, ARG3, ARG4, ARG5)

#define BALL_LOG6_INFO(MSG, ARG1, ARG2, ARG3, ARG4, ARG5, ARG6)            \
    BALL_LOG6(ball::Severity::e_INFO, MSG, ARG1, ARG2, ARG3, ARG4,         \
                                             ARG5, ARG6)

#define BALL_LOG7_INFO(MSG, ARG1, ARG2, ARG3, ARG4, ARG5, ARG6, ARG7)      \
    BALL_LOG7(ball::Severity::e_INFO, MSG, ARG1, ARG2, ARG3, ARG4,         \
                                             ARG5, ARG6, ARG7)

#define BALL_LOG8_INFO(MSG, ARG1, ARG2, ARG3, ARG4, ARG5, ARG6, ARG7,      \
                            ARG8)                                          \
    BALL_LOG8(ball::Severity::e_INFO, MSG, ARG1, ARG2, ARG3, ARG4,         \
                                             ARG5, ARG6, ARG7, ARG8)

#define BALL_LOG9_INFO(MSG, ARG1, ARG2, ARG3, ARG4, ARG5, ARG6, ARG7,      \
                             ARG8, ARG9)                                   \
    BALL_LOG9(ball::Severity::e_INFO, MSG, ARG1, ARG2, ARG3, ARG4,         \
                                             ARG5, ARG6, ARG7, ARG8, ARG9)

#define BALL_LOG0_WARN(MSG)                                                \
    BALL_LOG0(ball::Severity::e_WARN, MSG)

#define BALL_LOG1_WARN(MSG, ARG1)                                          \
    BALL_LOG1(ball::Severity::e_WARN, MSG, ARG1)

#define BALL_LOG2_WARN(MSG, ARG1, ARG2)                                    \
    BALL_LOG2(ball::Severity::e_WARN, MSG, ARG1, ARG2)

#define BALL_LOG3_WARN(MSG, ARG1, ARG2, ARG3)                              \
    BALL_LOG3(ball::Severity::e_WARN, MSG, ARG1, ARG2, ARG3)

#define BALL_LOG4_WARN(MSG, ARG1, ARG2, ARG3, ARG4)                        \
    BALL_LOG4(ball::Severity::e_WARN, MSG, ARG1, ARG2, ARG3, ARG4)

#define BALL_LOG5_WARN(MSG, ARG1, ARG2, ARG3, ARG4, ARG5)                  \
    BALL_LOG5(ball::Severity::e_WARN, MSG, ARG1, ARG2, ARG3, ARG4, ARG5)

#define BALL_LOG6_WARN(MSG, ARG1, ARG2, ARG3, ARG4, ARG5, ARG6)            \
    BALL_LOG6(ball::Severity::e_WARN, MSG, ARG1, ARG2, ARG3, ARG4,         \
                                             ARG5, ARG6)

#define BALL_LOG7_WARN(MSG, ARG1, ARG2, ARG3, ARG4, ARG5, ARG6, ARG7)      \
    BALL_LOG7(ball::Severity::e_WARN, MSG, ARG1, ARG2, ARG3, ARG4,         \
                                             ARG5, ARG6, ARG7)

#define BALL_LOG8_WARN(MSG, ARG1, ARG2, ARG3, ARG4, ARG5, ARG6, ARG7,      \
                             ARG8)                                         \
    BALL_LOG8(ball::Severity::e_WARN, MSG, ARG1, ARG2, ARG3, ARG4,         \
                                             ARG5, ARG6, ARG7, ARG8)

#define BALL_LOG9_WARN(MSG, ARG1, ARG2, ARG3, ARG4, ARG5, ARG6, ARG7,      \
                             ARG8, ARG9)                                   \
    BALL_LOG9(ball::Severity::e_WARN, MSG, ARG1, ARG2, ARG3, ARG4,         \
                                             ARG5, ARG6, ARG7, ARG8, ARG9)

#define BALL_LOG0_ERROR(MSG)                                               \
    BALL_LOG0(ball::Severity::e_ERROR, MSG)

#define BALL_LOG1_ERROR(MSG, ARG1)                                         \
    BALL_LOG1(ball::Severity::e_ERROR, MSG, ARG1)

#define BALL_LOG2_ERROR(MSG, ARG1, ARG2)                                   \
    BALL_LOG2(ball::Severity::e_ERROR, MSG, ARG1, ARG2)

#define BALL_LOG3_ERROR(MSG, ARG1, ARG2, ARG3)                             \
    BALL_LOG3(ball::Severity::e_ERROR, MSG, ARG1, ARG2, ARG3)

#define BALL_LOG4_ERROR(MSG, ARG1, ARG2, ARG3, ARG4)                       \
    BALL_LOG4(ball::Severity::e_ERROR, MSG, ARG1, ARG2, ARG3, ARG4)

#define BALL_LOG5_ERROR(MSG, ARG1, ARG2, ARG3, ARG4, ARG5)                 \
    BALL_LOG5(ball::Severity::e_ERROR, MSG, ARG1, ARG2, ARG3, ARG4, ARG5)

#define BALL_LOG6_ERROR(MSG, ARG1, ARG2, ARG3, ARG4, ARG5, ARG6)           \
    BALL_LOG6(ball::Severity::e_ERROR, MSG, ARG1, ARG2, ARG3, ARG4,        \
                                              ARG5, ARG6)

#define BALL_LOG7_ERROR(MSG, ARG1, ARG2, ARG3, ARG4, ARG5, ARG6, ARG7)     \
    BALL_LOG7(ball::Severity::e_ERROR, MSG, ARG1, ARG2, ARG3, ARG4,        \
                                              ARG5, ARG6, ARG7)

#define BALL_LOG8_ERROR(MSG, ARG1, ARG2, ARG3, ARG4, ARG5, ARG6, ARG7,     \
                             ARG8)                                         \
    BALL_LOG8(ball::Severity::e_ERROR, MSG, ARG1, ARG2, ARG3, ARG4,        \
                                              ARG5, ARG6, ARG7, ARG8)

#define BALL_LOG9_ERROR(MSG, ARG1, ARG2, ARG3, ARG4, ARG5, ARG6, ARG7,     \
                             ARG8, ARG9)                                   \
    BALL_LOG9(ball::Severity::e_ERROR, MSG, ARG1, ARG2, ARG3, ARG4,        \
                                              ARG5, ARG6, ARG7, ARG8, ARG9)

#define BALL_LOG0_FATAL(MSG)                                               \
    BALL_LOG0(ball::Severity::e_FATAL, MSG)

#define BALL_LOG1_FATAL(MSG, ARG1)                                         \
    BALL_LOG1(ball::Severity::e_FATAL, MSG, ARG1)

#define BALL_LOG2_FATAL(MSG, ARG1, ARG2)                                   \
    BALL_LOG2(ball::Severity::e_FATAL, MSG, ARG1, ARG2)

#define BALL_LOG3_FATAL(MSG, ARG1, ARG2, ARG3)                             \
    BALL_LOG3(ball::Severity::e_FATAL, MSG, ARG1, ARG2, ARG3)

#define BALL_LOG4_FATAL(MSG, ARG1, ARG2, ARG3, ARG4)                       \
    BALL_LOG4(ball::Severity::e_FATAL, MSG, ARG1, ARG2, ARG3, ARG4)

#define BALL_LOG5_FATAL(MSG, ARG1, ARG2, ARG3, ARG4, ARG5)                 \
    BALL_LOG5(ball::Severity::e_FATAL, MSG, ARG1, ARG2, ARG3, ARG4, ARG5)

#define BALL_LOG6_FATAL(MSG, ARG1, ARG2, ARG3, ARG4, ARG5, ARG6)           \
    BALL_LOG6(ball::Severity::e_FATAL, MSG, ARG1, ARG2, ARG3, ARG4,        \
                                              ARG5, ARG6)

#define BALL_LOG7_FATAL(MSG, ARG1, ARG2, ARG3, ARG4, ARG5, ARG6, ARG7)     \
    BALL_LOG7(ball::Severity::e_FATAL, MSG, ARG1, ARG2, ARG3, ARG4,        \
                                              ARG5, ARG6, ARG7)

#define BALL_LOG8_FATAL(MSG, ARG1, ARG2, ARG3, ARG4, ARG5, ARG6, ARG7,     \
                             ARG8)                                         \
    BALL_LOG8(ball::Severity::e_FATAL, MSG, ARG1, ARG2, ARG3, ARG4,        \
                                              ARG5, ARG6, ARG7, ARG8)

#define BALL_LOG9_FATAL(MSG, ARG1, ARG2, ARG3, ARG4, ARG5, ARG6, ARG7,     \
                             ARG8, ARG9)                                   \
    BALL_LOG9(ball::Severity::e_FATAL, MSG, ARG1, ARG2, ARG3, ARG4,        \
                                              ARG5, ARG6, ARG7, ARG8, ARG9)

                       // ==============
                       // Utility Macros
                       // ==============

#define BALL_LOG_IS_ENABLED(BALL_SEVERITY)                                    \
    ((BALL_LOG_THRESHOLD >= (BALL_SEVERITY)) &&                               \
    ball::Log::isCategoryEnabled(&BALL_LOG_CATEGORYHOLDER, BALL_SEVERITY))

namespace BloombergLP {

namespace bslmt { class Mutex; }

namespace ball {

class Record;

                         // ==========
                         // struct Log
                         // ==========

struct Log {
    // This 'struct' provides a namespace for a suite of utility functions that
    // simplify usage of the 'ball_loggermanager' component.  The direct use
    // of these utility functions is *strongly* discouraged.

    // CLASS METHODS
    static int format(char        *buffer,
                      bsl::size_t  numBytes,
                      const char  *format, ...);
        // Fill the specified 'buffer' with at most the specified 'numBytes'
        // characters produced by formatting the variable argument list
        // according to the specified 'printf'-style 'format' argument; return
        // the number of characters in the resulting formatted string.  The
        // last character placed into 'buffer' is always a null terminator
        // (leaving at most 'numBytes - 1' bytes of formatted data).  If
        // 'numBytes' is insufficient for the entire formatted string, this
        // method fills 'buffer' with the initial 'numBytes - 1' bytes of
        // formatted data followed by a null terminator and returns -1.  Note
        // that with the exception of the return value, the behavior of this
        // function exactly matches that of the C99 function 'snprintf'.  Also
        // note that 'snprintf' is not part of standard C++-98, so its
        // functionality is provided here.

    static Record *getRecord(const Category *category,
                             const char     *file,
                             int             line);
        // Return the address of a modifiable record having the specified
        // 'file' and 'line' attributes.  The memory for the record will be
        // supplied by the allocator held by the logger manager singleton if
        // the specified 'category' is non-null, or by the currently installed
        // default allocator otherwise.

    static void logMessage(const Category *category,
                           int             severity,
                           const char     *fileName,
                           int             lineNumber,
                           const char     *message);
        // Log a record containing the specified 'message' text, 'fileName',
        // 'lineNumber', 'severity', and the name of the specified 'category'.
        // (See the component-level documentation of 'ball_record' for more
        // information on the additional fields that are logged.)  Store the
        // record in the buffer held by the logger if 'severity' is at least
        // as severe as the current "Record" threshold level of 'category'.
        // Pass the record directly to the registered observer if 'severity'
        // is at least as severe as the current "Pass" threshold level of
        // 'category'.  Publish the entire contents of the buffer of the
        // logger if 'severity' is at least as severe as the current "Trigger"
        // threshold level of 'category'.  Publish the entire contents of all
        // buffers of all loggers if 'severity' is at least as severe as the
        // current "Trigger-All" threshold level of 'category' (i.e., via the
        // callback supplied at construction of the logger manager).  Note that
        // this method has no effect if 'severity' is less severe than each of
        // the threshold levels of 'category'.  The behavior is undefined
        // unless 'severity' is in the range '[1 .. 255]' and the logger
        // manager singleton has been initialized.

    static void logMessage(const Category *category,
                           int             severity,
                           Record         *record);
        // Log the specified 'record' after setting its category attribute to
        // the specified 'category' and its severity attribute to the specified
        // 'severity'.  (See the component-level documentation of 'ball_record'
        // for more information on the fields that are logged.)  Store the
        // record in the buffer held by the logger if 'severity' is at least
        // as severe as the current "Record" threshold level of 'category'.
        // Pass the record directly to the registered observer if 'severity'
        // is at least as severe as the current "Pass" threshold level of
        // 'category'.  Publish the entire contents of the buffer of the
        // logger if 'severity' is at least as severe as the current "Trigger"
        // threshold level of 'category'.  Publish the entire contents of all
        // buffers of all loggers if 'severity' is at least as severe as the
        // current "Trigger-All" threshold level of 'category' (i.e., via the
        // callback supplied at construction of the logger manager).  Note that
        // this method has no effect if 'severity' is less severe than each of
        // the threshold levels of 'category'.  The behavior is undefined
        // unless 'severity' is in the range '[1 .. 255]', 'record' was
        // obtained by a call to 'Log::getRecord', and the logger manager
        // singleton has been initialized.

    static char *obtainMessageBuffer(bslmt::Mutex **mutex, int *bufferSize);
        // Block until access to the buffer used for formatting messages in
        // this thread of execution is available.  Return the address of the
        // modifiable buffer to which this thread of execution has exclusive
        // access, load the address of the mutex that protects the buffer into
        // the specified '*mutex' address, and load the size (in bytes) of the
        // buffer into the specified 'bufferSize' address.  The address remains
        // valid, and the buffer remains locked by this thread of execution,
        // until the 'Log::releaseMessageBuffer' method is called.  The
        // behavior is undefined if this thread of execution currently holds a
        // lock on the buffer.  Note that the buffer is intended to be used
        // *only* for formatting log messages immediately before a call to
        // 'Log::logMessage'; other use may adversely affect performance for
        // the entire program.

    static void releaseMessageBuffer(bslmt::Mutex *mutex);
        // Unlock the specified '*mutex' that guards the buffer used for
        // formatting messages in this thread of execution.  The behavior is
        // undefined unless '*mutex' was obtained by a call to
        // 'Log::obtainMessageBuffer' and has not yet been unlocked.

    static const Category *setCategory(const char *categoryName);
        // Return from the logger manager's category registry the address of
        // the non-modifiable category having the specified 'categoryName' if
        // such a category exists, or if a new category having 'categoryName'
        // can be added to the registry (i.e., if the registry has sufficient
        // capacity to accommodate new entries); otherwise, return the address
        // of the non-modifiable *Default* *Category*.  Return 0 if the logger
        // manager singleton has not been initialized or has been destroyed.

    static void setCategory(CategoryHolder *categoryHolder,
                            const char     *categoryName);
        // Load into the specified 'categoryHolder' the address of the
        // non-modifiable category having the specified 'categoryName' if such
        // a category exists, or if a new category having 'categoryName' can
        // be added to the registry (i.e., if the registry has sufficient
        // capacity to accommodate new entries); otherwise, load the address of
        // the non-modifiable *Default* *Category*.  Also load into
        // 'categoryHolder' the maximum threshold level of the category
        // ultimately loaded into 'categoryHolder'.

    static bool isCategoryEnabled(const CategoryHolder *categoryHolder,
                                  int                   severity);
        // Return 'true' if logging to the category associated with the
        // specified 'categoryHolder' at the specified 'severity' is enabled,
        // or if 'Severity::e_WARN >= severity' and the logger manager
        // singleton is not initialized; return 'false' otherwise.
};

                        // ================
                        // class Log_Stream
                        // ================

class Log_Stream {
    // This class provides an aggregate of several objects relevant to the
    // logging of a message via the C++ stream-based macros:
    //..
    //  - record to be logged
    //  - category to which to log the record
    //  - severity at which to log the record
    //  - stream to which the user log message is put
    //..
    // As a side-effect of creating an object of this class, the record and
    // stream are also constructed.  As a side-effect of destroying the
    // object, the record is logged.
    //
    // This class should *not* be used directly by client code.  It is an
    // implementation detail of the macros provided by this component.

    // DATA
    const Category *d_category_p;  // category to which record is logged
                                   // (held, not owned)

    Record         *d_record_p;    // logged record (held, not owned)

    const int       d_severity;    // severity at which record is logged

    bsl::ostream    d_stream;      // stream to which log message is put

  private:
    // NOT IMPLEMENTED
    Log_Stream(const Log_Stream&);
    Log_Stream& operator=(const Log_Stream&);

  public:
    // CREATORS
    Log_Stream(const Category *category,
               const char     *fileName,
               int             lineNumber,
               int             severity);
        // Create a logging stream that holds (1) the specified 'category' and
        // 'severity', (2) a record that is created from the specified
        // 'fileName' and 'lineNumber', and (3) an 'bsl::ostream' to which the
        // log message is put.

    ~Log_Stream();
        // Log the record held by this logging stream to the held category
        // (as returned by 'category') at the held severity (as returned by
        // 'severity') and destroy this logging stream.

    // MANIPULATORS
    Record *record();
        // Return the address of the modifiable log record held by this logging
        // stream.  The address is valid until this logging stream is
        // destroyed.

    bsl::ostream& stream();
        // Return a reference to the modifiable stream held by this logging
        // stream.  The reference is valid until this logging stream is
        // destroyed.

    // ACCESSORS
    const Category *category() const;
        // Return the address of the non-modifiable category held by this
        // logging stream.

    const Record *record() const;
        // Return the address of the non-modifiable log record held by this
        // logging stream.  The address is valid until this logging stream is
        // destroyed.

    int severity() const;
        // Return the severity held by this logging stream.
};

                     // ===================
                     // class Log_Formatter
                     // ===================

class Log_Formatter {
    // This class provides an aggregate of several objects relevant to the
    // logging of a message via the 'printf'-style macros:
    //..
    //  - record to be logged
    //  - category to which to log the record
    //  - severity at which to log the record
    //  - buffer in which the user log message is formatted
    //  - mutex mediating exclusive access to the buffer
    //..
    // As a side-effect of creating an object of this class, the record is
    // constructed and the mutex is locked.  As a side-effect of destroying the
    // object, the record is logged and the mutex unlocked.
    //
    // This class should *not* be used directly by client code.  It is an
    // implementation detail of the macros provided by this component.

    // DATA
    const Category *d_category_p;  // category to which record is logged
                                   // (held, not owned)

    Record         *d_record_p;    // logged record (held, not owned)

    const int       d_severity;    // severity at which record is logged

    char           *d_buffer_p;    // buffer for formatted user log message
                                   // (held, not owned)

    int             d_bufferLen;   // length of buffer

    bslmt::Mutex   *d_mutex_p;     // mutex to lock buffer (held, not owned)

  private:
    // NOT IMPLEMENTED
    Log_Formatter(const Log_Formatter&);
    Log_Formatter& operator=(const Log_Formatter&);

  public:
    // CREATORS
    Log_Formatter(const Category *category,
                  const char     *fileName,
                  int             lineNumber,
                  int             severity);
        // Create a logging formatter that holds (1) the specified 'category'
        // and 'severity', (2) a record that is created from the specified
        // 'fileName' and 'lineNumber', and (3) a buffer into which the log
        // message is formatted, and for which a lock is acquired for exclusive
        // access to the buffer.

    ~Log_Formatter();
        // Log the record held by this logging formatter to the held category
        // (as returned by 'category') at the held severity (as returned by
        // 'severity'), release the lock on the held buffer, and destroy this
        // logging formatter.

    // MANIPULATORS
    Record *record();
        // Return the address of the modifiable log record held by this logging
        // formatter.  The address is valid until this logging formatter is
        // destroyed.

    char *messageBuffer();
        // Return the address of the modifiable buffer held by this logging
        // formatter.  The address is valid until this logging formatter is
        // destroyed.

    // ACCESSORS
    const Category *category() const;
        // Return the address of the non-modifiable category held by this
        // logging formatter.

    int messageBufferLen() const;
        // Return the length (in bytes) of the buffer held by this logging
        // formatter.

    const Record *record() const;
        // Return the address of the non-modifiable log record held by this
        // logging formatter.  The address is valid until this logging
        // formatter is destroyed.

    int severity() const;
        // Return the severity held by this logging formatter.
};

// ============================================================================
//                              INLINE DEFINITIONS
// ============================================================================

                     // ----------------
                     // class Log_Stream
                     // ----------------

// MANIPULATORS
inline
Record *Log_Stream::record()
{
    return d_record_p;
}

inline
bsl::ostream& Log_Stream::stream()
{
    return d_stream;
}

// ACCESSORS
inline
const Category *Log_Stream::category() const
{
    return d_category_p;
}

inline
const Record *Log_Stream::record() const
{
    return d_record_p;
}

inline
int Log_Stream::severity() const
{
    return d_severity;
}

                     // -------------------
                     // class Log_Formatter
                     // -------------------

// MANIPULATORS
inline
Record *Log_Formatter::record()
{
    return d_record_p;
}

inline
char *Log_Formatter::messageBuffer()
{
    return d_buffer_p;
}

// ACCESSORS
inline
const Category *Log_Formatter::category() const
{
    return d_category_p;
}

inline
int Log_Formatter::messageBufferLen() const
{
    return d_bufferLen;
}

inline
const Record *Log_Formatter::record() const
{
    return d_record_p;
}

inline
int Log_Formatter::severity() const
{
    return d_severity;
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
