// keyfeatures_example2.cpp                                           -*-C++-*-

#include <ball_log.h>
#include <ball_loggermanager.h>
#include <ball_loggermanagerconfiguration.h>
#include <ball_fileobserver.h>
#include <ball_scopedattribute.h>
#include <bslma_allocator.h>
#include <bslma_default.h>

#include <bsl_iostream.h>
#include <bsl_memory.h>

using namespace BloombergLP;

///Key Example 2: Initialization
///- - - - - - - - - - - - - - -
// Clients that perform logging must first instantiate the singleton logger
// manager using the 'ball::LoggerManagerScopedGuard' class.  This example
// shows how to create a logger manager with basic "default behavior".
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
    // myApp.cpp
//
    int main()
    {
        ball::LoggerManagerConfiguration configuration;
        configuration.setDefaultThresholdLevelsIfValid(ball::Severity::e_WARN);
//..
// Next, create a 'ball::LoggerManagerScopedGuard' object whose constructor
// takes the configuration object just created.  The guard will initialize the
// logger manager singleton on creation and destroy the singleton upon
// destruction.  This guarantees that any resources used by the logger manager
// will be properly released when they are not needed:
//..
        ball::LoggerManagerScopedGuard guard(configuration);
//..
// Note that the application is now prepared to log messages using the 'ball'
// logging subsystem, but until the application registers an observer, all log
// messages will be discarded.
//
// Finally, we create a 'ball::FileObserver' object 'observer' that will
// publish records to a file, and exceptional records to 'stdout'.  We
// configure the log format to publish log attributes (see
// {Key Example 1: Write to a Log}, enable the logger to write to a log file,
// and then register 'observer' with the logger manager.  Note that observers
// must be registered by name; this example simply uses "default" for a name:
//..
        bslma::Allocator *alloc = bslma::Default::globalAllocator(0);
//
        bsl::shared_ptr<ball::FileObserver> observer =
            bsl::allocate_shared<ball::FileObserver>(alloc);

        observer->setLogFormat(
                        ball::RecordStringFormatter::k_BASIC_ATTRIBUTE_FORMAT,
                        ball::RecordStringFormatter::k_BASIC_ATTRIBUTE_FORMAT);

        if (0 != observer->enableFileLogging("myapplication.log.%T")) {
            bsl::cout << "Failed to enable logging" << bsl::endl;
            return -1;
        }
        ball::LoggerManager::singleton().registerObserver(observer, "default");
//..
// The application is now prepared to log messages using the 'ball' logging
// subsystem:
//..
        // ...
//
        BALL_LOG_SET_CATEGORY("MYLIBRARY.MYSUBSYSTEM");
        BALL_LOG_ERROR << "Exiting the application (0)";

        return 0;
    }
//..
// Note that concrete observers that can be configured after their creation
// (e.g., as to whether log records are published in UTC or local time)
// generally can have their configuration adjusted at any time, either before
// or after being registered with a logger manager.  For an example of such an
// observer, see 'ball_asyncfileobserver'.
