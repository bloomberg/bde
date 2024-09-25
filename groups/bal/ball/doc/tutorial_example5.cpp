  // tutorial_example5.cpp                                             -*-C++-*-

  #include <ball_log.h>
  #include <ball_loggermanager.h>
  #include <ball_loggermanagerconfiguration.h>
  #include <ball_severity.h>
  #include <ball_streamobserver.h>

  #include <bslma_allocator.h>
  #include <bslma_default.h>

  #include <bslmt_threadutil.h>

  #include <bsls_timeinterval.h>

  #include <bsl_iostream.h>
  #include <bsl_memory.h>

  using namespace BloombergLP;

  /// Log the specified `message` to the "Function 1" category at `e_INFO`
  /// severity.
  void f1(const char *message)
  {
      BALL_LOG_SET_CATEGORY("Function 1");
      BALL_LOG_INFO << message;
  }

  /// Log the specified `message` to the "Function 2" category at `e_WARN`
  /// severity.
  void f2(const char *message)
  {
      BALL_LOG_SET_CATEGORY("Function 2");
      BALL_LOG_WARN << message;
  }

  /// Log to the default logger a sequence of messages to the "Function 1"
  /// category at `e_INFO` severity.
  extern "C" void *threadFunction1(void *)
  {
      char               buf[10] = "Message n";
      bsls::TimeInterval waitTime(4.0);

      for (int i = 0; i < 3; ++i) {
          buf[8] = '0' + i;
          f1(buf);
          bslmt::ThreadUtil::sleep(waitTime);
      }
      return 0;
  }

  /// Log to the default logger a sequence of messages to the "Function 2"
  /// category at `e_WARN` severity.
  extern "C" void *threadFunction2(void *)
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
      int verbose = argc > 1;  // allows user to control output from command
                               // line

      // Get global allocator.
      bslma::Allocator *alloc_p = bslma::Default::globalAllocator();

      ball::LoggerManagerConfiguration configuration;
      configuration.setDefaultThresholdLevelsIfValid(
                                    ball::Severity::e_TRACE,  // "Record"
                                    ball::Severity::e_WARN,   // "Pass-Through"
                                    ball::Severity::e_ERROR,  // "Trigger"
                                    ball::Severity::e_FATAL); // "Trigger-All"

      // Instantiate the logger manager singleton.
      ball::LoggerManagerScopedGuard scopedGuard(configuration);

      ball::LoggerManager& manager = ball::LoggerManager::singleton();

      // Create simple observer; writes to `stdout`.
      bsl::shared_ptr<ball::StreamObserver> observer(
                                new(*alloc_p) ball::StreamObserver(&bsl::cout),
                                alloc_p);

      manager.registerObserver(observer, "default");
          // Register the observer under (arbitrary) name "default".

      BALL_LOG_SET_CATEGORY("main");

      bslmt::ThreadAttributes   attributes;
      bslmt::ThreadUtil::Handle handle1;
      bslmt::ThreadUtil::Handle handle2;

      bslmt::ThreadUtil::create(&handle1, attributes, threadFunction1, 0);
      bslmt::ThreadUtil::create(&handle2, attributes, threadFunction2, 0);

      bslmt::ThreadUtil::join(handle1);
      bslmt::ThreadUtil::join(handle2);

      if (verbose) {  // 'if' to allow command-line activation
          BALL_LOG_ERROR << "Force publication.";
      }
      return 0;
  }
