  // advancedfeatures_example1.cpp                                     -*-C++-*-

  #include <ball_log.h>
  #include <ball_loggermanager.h>
  #include <ball_loggermanagerconfiguration.h>
  #include <ball_managedattribute.h>
  #include <ball_recordstringformatter.h>
  #include <ball_rule.h>
  #include <ball_scopedattribute.h>
  #include <ball_streamobserver.h>


  #include <bslma_allocator.h>
  #include <bslma_default.h>

  #include <bsl_iostream.h>
  #include <bsl_memory.h>

  using namespace BloombergLP;

// The following example demonstrates the use of attributes and rules to
// conditionally enable logging.
//
// We start by defining a function, 'processData', that is passed data in a
// 'vector<char>' and information about the user who sent the data.  This
// example function performs no actual processing, but does log a single
// message at the 'ball::Severity::e_DEBUG' threshold level.  The 'processData'
// function also adds the user information passed to this function to the
// thread's attribute context.  We will use these attributes later, to create a
// logging rule that enables verbose logging only for a particular user.
//..
    void processData(int                      uuid,
                     int                      luw,
                     int                      terminalNumber,
                     const bsl::vector<char>& data)
        // Process the specified 'data' associated with the specified Bloomberg
        // 'uuid', 'luw', and 'terminalNumber'.
    {
        (void)data;  // suppress "unused" warning
//..
// We add our attributes using 'ball::ScopedAttribute', which adds an attribute
// container with one attribute to a list of containers.  This is easy and
// efficient if the number of attributes is small, but should not be used if
// there are a large number of attributes.  If motivated, we could use
// 'ball::DefaultAttributeContainer', which provides an efficient container for
// a large number of attributes, or even create a more efficient attribute
// container implementation specifically for these three attributes (uuid, luw,
// and terminalNumber).  See {'ball_scopedattributes'} (plural) for an example
// of using a different attribute container, and {'ball_attributecontainer'}
// for an example of creating a custom attribute container.
//..
        // We use 'ball::ScopedAttribute' here because the number of
        // attributes is relatively small.
//
        ball::ScopedAttribute uuidAttribute("mylibrary.uuid", uuid);
        ball::ScopedAttribute luwAttribute("mylibrary.luw", luw);
        ball::ScopedAttribute termNumAttribute("mylibrary.terminalNumber",
                                               terminalNumber);
//..
// In this simplified example we perform no actual processing, and simply log
// a message at the 'ball::Severity::e_DEBUG' level.
//..
        BALL_LOG_SET_CATEGORY("EXAMPLE.CATEGORY");
//
        BALL_LOG_DEBUG << "An example message";
//..
// Notice that if we were not using a "scoped" attribute container like that
// provided automatically by 'ball::ScopedAttribute' (e.g., if we were using a
// local 'ball::DefaultAttributeContainer' instead), then the container
// **must** be removed from the 'ball::AttributeContext' before it is
// destroyed!  See 'ball_scopedattributes' (plural) for an example.
//..
    }
//..
// Next we demonstrate how to create a logging rule that sets the pass-through
// logging threshold to 'ball::Severity::e_TRACE' (i.e., enables verbose logging)
// for a particular user when calling the 'processData' function defined
// above.
//
// We start by creating the singleton logger manager that we configure with
// the stream observer and a default configuration.  We then call the
// 'processData' function: This first call to 'processData' will not result in
// any logged messages because 'processData' logs its message at the
// 'ball::Severity::e_DEBUG' level, which is below the default configured logging
// threshold.
//..
    int main(int argc, const char *argv[])
    {
        ball::LoggerManagerConfiguration configuration;
        ball::LoggerManagerScopedGuard lmg(configuration);
        ball::LoggerManager& manager = ball::LoggerManager::singleton();

        bsl::shared_ptr<ball::StreamObserver> observer =
            bsl::make_shared<ball::StreamObserver>(&bsl::cout);
        manager.registerObserver(observer, "default");
//
        BALL_LOG_SET_CATEGORY("EXAMPLE.CATEGORY");
//
        bsl::vector<char> message;
//
        BALL_LOG_ERROR << "Processing the first message.";
        processData(3938908, 2, 9001, message);

//..
// Now we add a logging rule, setting the pass-through threshold to be
// 'ball::Severity::e_TRACE' (i.e., enabling verbose logging) if the thread's
// context contains a "uuid" of 3938908.  Note that we use the wild-card
// value '*' for the category so that the 'ball::Rule' rule will apply to all
// categories.
//..
        ball::Rule rule("*", 0, ball::Severity::e_TRACE, 0, 0);
        rule.addAttribute(ball::ManagedAttribute("mylibrary.uuid", 3938908));
        ball::LoggerManager::singleton().addRule(rule);
//
        BALL_LOG_ERROR << "Processing the second message.";
        processData(3938908, 2, 9001, message);
//..
// The final call to the 'processData' function below, passes a "uuid" of
// 2171395 (not 3938908) so the logging rule we defined will *not* apply and
// no message will be logged.
//..
        BALL_LOG_ERROR << "Processing the third message.";
        processData(2171395, 2, 9001, message);
    }
//..
