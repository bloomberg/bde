  // tutorial_example4.cpp                                             -*-C++-*-

  // f_log.h                                                          -*-C++-*-
  #ifndef INCLUDED_F_LOG
  #define INCLUDED_F_LOG

  namespace BloombergLP {

  void logThisInfoMsg(const char *message);
      // Log the specified 'message' at the 'ball::Severity::e_INFO' severity
      // level.  The category to which 'message' is logged is defined in
      // the implementation.  Note that this function may affect the
      // operation of other logging operations that do not explicitly set
      // their own categories.

  }  // close enterprise namespace

  #endif
//..
// Then the 'f_log.cpp' implementation file:
//..
  // f_log.cpp                                                        -*-C++-*-

  #include <ball_log.h>

  namespace BloombergLP {

  void logThisInfoMsg(const char *message)
  {
      // Note that the uppercase symbols are macros defined in 'ball_log'.

      BALL_LOG_SET_CATEGORY("function category");
          // Set a category -- arbitrary and may be changed.

      BALL_LOG_INFO << message;
          // Log 'message' at 'ball::Severity::e_INFO' severity level.
  }

  }  // close enterprise namespace
//..
// Finally, the 'logging.m.cpp' file that defines 'main':
//..
  // logging.m.cpp                                                    -*-C++-*-

  #include <ball_log.h>
  #include <ball_loggermanager.h>
  #include <ball_loggermanagerconfiguration.h>
  #include <ball_severity.h>
  #include <ball_streamobserver.h>

  #include <bslma_allocator.h>
  #include <bslma_default.h>

  #include <bsl_iostream.h>
  #include <bsl_memory.h>

//  #include <f_log.h>

  using namespace BloombergLP;

  int main(int argc, char *argv[])
  {
      int verbose = argc > 1;
          // Enable command-line control of program behavior.

      bslma::Allocator *alloc_p = bslma::Default::globalAllocator();
          // Get global allocator.

      ball::LoggerManagerConfiguration configuration;
          // Instantiate the default configuration.

      ball::LoggerManagerScopedGuard scopedGuard(configuration);
          // Instantiate the logger manager singleton.

      ball::LoggerManager& manager = ball::LoggerManager::singleton();

      manager.setDefaultThresholdLevels(
                    ball::Severity::e_TRACE,   // sets "Record" threshold
                    ball::Severity::e_INFO,    // sets "Pass-Through" threshold
                    ball::Severity::e_ERROR,   // sets "Trigger" threshold
                    ball::Severity::e_FATAL);  // sets "Trigger-All" threshold

      bsl::shared_ptr<ball::StreamObserver> observer(
                                new(*alloc_p) ball::StreamObserver(&bsl::cout),
                                alloc_p);
          // Create simple observer; writes to 'stdout'.

      manager.registerObserver(observer, "default");
          // Register the observer under (arbitrary) name "default".

      BALL_LOG_SET_CATEGORY("main category");
          // Set a category -- note that this will *not* affect the category
          // set in 'logThisInfoMsg'.

      BALL_LOG_INFO << "Called directly from 'main'";
          // Logged to "main category".

      if (verbose) {  // 'if' to allow command-line activation
          for (int i = 0; i < 3; ++i) {
              logThisInfoMsg("Hello world!");
              bsl::cout << "Watch the loop execute:  i = " << i << bsl::endl;
                  // proves Msg is published as "pass-through"
          }
      }

      BALL_LOG_INFO << "Called again directly from 'main'";
          // Logged to "main category".

      return 0;
  }
