  // tutorial_example6.cpp                                             -*-C++-*-

  #include <ball_fixedsizerecordbuffer.h>
  #include <ball_log.h>
  #include <ball_loggermanager.h>
  #include <ball_loggermanagerconfiguration.h>
  #include <ball_severity.h>
  #include <ball_streamobserver.h>

  #include <bslma_allocator.h>
  #include <bslma_default.h>

  #include <bslmt_threadutil.h>

  #include <bsls_timeinterval.h>

  #include <bsl_fstream.h>
  #include <bsl_memory.h>

  using namespace BloombergLP;

  void f1(const char *message)
      // Log the specified 'message' to the "Function 1" category at 'e_INFO'
      // severity.
  {
      BALL_LOG_SET_CATEGORY("Function 1");
      BALL_LOG_INFO << message;
  }

  void f2(const char *message)
      // Log the specified 'message' to the "Function 2" category at 'e_WARN'
      // severity.
  {
      BALL_LOG_SET_CATEGORY("Function 2");
      BALL_LOG_WARN << message;
  }

  extern "C" void *threadFunction1(void *logger)
      // Log to the specified 'logger' a sequence of messages to the "Function
      // 1" category at 'e_INFO' severity.
  {
      ball::LoggerManager::singleton().setLogger((ball::Logger *)logger);
          // Install a local logger for this thread.

      char               buf[10] = "Message n";
      bsls::TimeInterval waitTime(4.0);

      for (int i = 0; i < 3; ++i) {
          buf[8] = '0' + i;
          f1(buf);
          bslmt::ThreadUtil::sleep(waitTime);
      }
      return 0;
  }

  extern "C" void *threadFunction2(void *)
      // Log to the default logger a sequence of messages to the "Function 2"
      // category at 'e_WARN' severity.
  {
      char               buf[10] = "Message n";
      bsls::TimeInterval waitTime(2.0);

      for (int i = 0; i < 3; ++i) {
          buf[8] = '0' + i;
          bslmt::ThreadUtil::sleep(waitTime);
          f2(buf);
      }
      return 0;
  }

  int main(int argc, char *argv[])
  {
      int verbose = argc > 1; // allows user to control output from command
                              // line

      bslma::Allocator *alloc_p = bslma::Default::globalAllocator();
          // Get global allocator.

      ball::LoggerManagerConfiguration configuration;
      configuration.setDefaultThresholdLevelsIfValid(
                                   ball::Severity::e_TRACE,   // "Record"
                                   ball::Severity::e_WARN,    // "Pass-Through"
                                   ball::Severity::e_ERROR,   // "Trigger"
                                   ball::Severity::e_FATAL);  // "Trigger-All"

      ball::LoggerManagerScopedGuard scopedGuard(configuration);
          // Instantiate the logger manager singleton.

      ball::LoggerManager& manager = ball::LoggerManager::singleton();

      bsl::ofstream outFile("outFile");
      bsl::shared_ptr<ball::StreamObserver> observer(
                                  new(*alloc_p) ball::StreamObserver(&outFile),
                                  alloc_p);
          // Create simple observer; writes to 'outFile".

      manager.registerObserver(observer, "default");
          // Register the observer under (arbitrary) name "default".

      BALL_LOG_SET_CATEGORY("main");

      // The following lines prepare resources to allocate a 'logger' from the
      // logger manager.  'main' is responsible for managing resource
      // lifetimes.

      enum { LOGBUF_SIZE = 32 * 1024 };
      ball::FixedSizeRecordBuffer  rb(LOGBUF_SIZE, alloc_p);
      ball::Logger                *logger  = manager.allocateLogger(&rb);

      bslmt::ThreadAttributes   attributes;
      bslmt::ThreadUtil::Handle handle1;
      bslmt::ThreadUtil::Handle handle2;

      // first thread gets 'logger'; second thread uses default logger
      bslmt::ThreadUtil::create(&handle1, attributes, threadFunction1, logger);
      bslmt::ThreadUtil::create(&handle2, attributes, threadFunction2, 0);

      bslmt::ThreadUtil::join(handle1);
      bslmt::ThreadUtil::join(handle2);

      if (verbose) {  // 'if' to allow command-line activation
          BALL_LOG_FATAL << "Force publication.";
      }

      manager.deallocateLogger(logger);  // free resources
      return 0;
  }

