  // tutorial_example2.cpp                                             -*-C++-*-

  #include <ball_log.h>
  #include <ball_loggermanager.h>
  #include <ball_loggermanagerconfiguration.h>
  #include <ball_severity.h>
  #include <ball_streamobserver.h>

  #include <bslma_allocator.h>
  #include <bslma_default.h>

  #include <bsl_iostream.h>
  #include <bsl_memory.h>

  using namespace BloombergLP;

  int main(int argc, char *argv[])
  {
      int verbose = argc > 1;
          // Enable command-line control of program behavior.

      bslma::Allocator *alloc_p = bslma::Default::globalAllocator();
          // Get global allocator.

      ball::LoggerManagerConfiguration configuration;
          // Instantiate the default configuration.

      configuration.setDefaultThresholdLevelsIfValid(
                               ball::Severity::e_TRACE,   // record level
                               ball::Severity::e_WARN,    // pass-through level
                               ball::Severity::e_ERROR,   // trigger level
                               ball::Severity::e_FATAL);  // trigger-all level
          // Set the four severity threshold levels; note that this method can
          // fail, and therefore returns a status.

      ball::LoggerManagerScopedGuard scopedGuard(configuration);
          // Instantiate the logger manager singleton.

      ball::LoggerManager& manager = ball::LoggerManager::singleton();

      bsl::shared_ptr<ball::StreamObserver> observer(
                                new(*alloc_p) ball::StreamObserver(&bsl::cout),
                                alloc_p);
          // Create simple observer; writes to 'stdout'.

      manager.registerObserver(observer, "default");
          // Register the observer under (arbitrary) name "default".

      BALL_LOG_SET_CATEGORY("main category");
          // Set a category -- an arbitrary name.

      BALL_LOG_INFO << "Hello world!";
          // With default settings, this line has no effect.

      if (verbose) {  // 'if' to allow command-line activation
          BALL_LOG_ERROR << "Good-bye world!";
              // Log 'message' at 'ball::Severity::e_ERROR' severity level
              // *and* trigger the publication of the record buffer
              // "trace-back".
      }
      else {
          bsl::cout << "This program should produce no other output.\n";
              // 'e_INFO' messages are recorded but *not* automatically
              // published; an 'e_ERROR' or 'e_FATAL' message is needed to
              // trigger the publication of the record buffer.
      }

      return 0;
  }
