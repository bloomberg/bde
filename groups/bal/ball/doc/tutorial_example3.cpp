  // tutorial_example3.cpp                                             -*-C++-*-

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

  int main()
  {
      bslma::Allocator *alloc_p = bslma::Default::globalAllocator();
          // Get global allocator.

      ball::LoggerManagerConfiguration configuration;
          // Create default configuration.

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

      BALL_LOG_INFO << "Hello again, world!";

      bsl::cout << "We're almost ready to exit."     " "
                   "Let's publish the buffer first:" "\n";

      manager.getLogger().publish();
          // This chain of calls insures that all messages that are still held
          // in the logger's in-memory buffer are published before the program
          // terminates.

      return 0;
  }

