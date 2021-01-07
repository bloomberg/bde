// advancedfeatures_example2.cpp                                      -*-C++-*-

#include <ball_attributecontainer.h>
#include <ball_attributecontext.h>
#include <ball_log.h>
#include <ball_loggermanager.h>
#include <ball_recordstringformatter.h>
#include <ball_streamobserver.h>
#include <ball_scopedattribute.h>
#include <bslim_printer.h>

#include <bsl_iostream.h>

#if defined (BSLS_PLATFORM_OS_LINUX)
#include <unistd.h>
#else
int gethostname(char *name, size_t len)
{
    bsl::strcpy(name, "myhostname");
}
#endif


using namespace BloombergLP;

///Advanced Features Example 2: Customizing Attribute Collection
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// This example demonstrates how to customize the collection of log attributes.
// Attributes are typically associated with a log record using a
// 'ball::ScopedAttribute' object (see {Key Example 1: Write to a Log} and
// {Key Example 2: Initialization}).  However, advanced users can customize
// the collection of attributes, either by registering an attribute-collector
// callback with the 'ball::LoggerManager', or by creating their own
// 'ball::AttributeCollector' implementation, or both.  These customizations
// can be used to implement alternative behavior, or provide faster
// implementation (e.g., by taking advantage of compile-time knowledge of the
// attributes being recorded).
//
// Suppose we are writing a performance critical infrastructure system
// that processes requests.  We want to:
//
//: o Obtain a (very small) performance benefit by implementing our own
//:   'ball::AttributeContainer (rather than using 'ball::ScopedAttribute').
//:
//: o Install a separate attribute collector callback function, in this
//:   instance one that will report information about global process state.
//
// First we create a 'ball::AttributeContainer' implementation.  Although the
// performance gains from doing so are typically insignificant, the use of a
// context-specific attribute container object allows us to take advantage of
// compile-time knowledge of the attributes being collected and make small
// improvements in the overhead required, which may be important for
// performance critical systems.
//..
//  // serviceattributes.h
//
    class ServiceAttributes : public ball::AttributeContainer {
        // Provide a concrete implementation of the 'ball::AttributeContainer'
        // protocol that holds the 'uuid', 'luw', and 'firmId' associated with a
        // request to the example service.

        int d_uuid;
        int d_luw;
        int d_firmId;
//
        // ...
//
      public:
        // CREATORS
        ServiceAttributes(int uuid, int luw, int firmId);
            // Create a service-attributes object with the specified 'uuid',
            // 'luw', and 'firmId'.
//
        virtual ~ServiceAttributes();

        // ACCESSORS
        virtual bool hasValue(const ball::Attribute& value) const;

        virtual void visitAttributes(
             const bsl::function<void(const ball::Attribute&)>& visitor) const;

        virtual bsl::ostream& print(bsl::ostream& stream,
                                    int           level = 0,
                                    int           spacesPerLevel = 4) const;
            // Format this object to the specified output 'stream'.
    };
//
    // CREATORS
    inline
    ServiceAttributes::ServiceAttributes(int uuid, int luw, int firmId)
    : d_uuid(uuid)
    , d_luw(luw)
    , d_firmId(firmId)
    {
    }
//
      // serviceattributes.cpp
//
    // CREATORS
    ServiceAttributes::~ServiceAttributes()
    {
    }
//
    // ACCESSORS
    bool ServiceAttributes::hasValue(const ball::Attribute& value) const
    {
        return ball::Attribute("mylibrary.uuid",    d_uuid)   == value
            || ball::Attribute("mylibrary.luw",     d_luw)    == value
            || ball::Attribute("mylibrary.firmId",  d_firmId) == value;
    }
//
    void ServiceAttributes::visitAttributes(
        const bsl::function<void(const ball::Attribute&)>& visitor) const
    {
        visitor(ball::Attribute("mylibrary.uuid",   d_uuid));
        visitor(ball::Attribute("mylibrary.luw",    d_luw));
        visitor(ball::Attribute("mylibrary.firmId", d_firmId));
    }
//
    bsl::ostream& ServiceAttributes::print(bsl::ostream& stream,
                                           int           level,
                                           int           spacesPerLevel) const
    {
        bslim::Printer printer(&stream, level, spacesPerLevel);
        printer.start();
        printer.printAttribute("uuid", d_uuid);
        printer.printAttribute("luw", d_luw);
        printer.printAttribute("firmId", d_firmId);
        printer.end();
        return stream;
    }
//..
// Then we create a guard to add and remove a 'ServiceAttributes' container
// from the current logging attribute context:
//..
    class ServiceAttributesGuard {
        // DATA
        ServiceAttributes                      d_attributes;
            // attributes

        const ball::AttributeContext::iterator d_it;
            // reference to attribute container

      public:
        ServiceAttributesGuard(int uuid, int luw, int firmId)
        : d_attributes(uuid, luw, firmId)
        , d_it(
            ball::AttributeContext::getContext()->addAttributes(&d_attributes))
        {
        }

        ~ServiceAttributesGuard()
        {
            ball::AttributeContext::getContext()->removeAttributes(d_it);
        }
    };
//..
// Now we use a 'ServiceAttributesGuard' in a critical infrastructure
// function:
//..
    int processData(int  uuid, int luw, int firmId, const char *data)
    {
        BALL_LOG_SET_CATEGORY("MYLIBRARY.MYSUBSYSTEM");
        ServiceAttributesGuard attributes(uuid, luw, firmId);

        BALL_LOG_TRACE << "Processing data: " << data;

        int rc = 0;

        // ...

        if (0 != rc) {
            BALL_LOG_WARN << "Error processing data: " << data;
        }
        return rc;
    }
//..
// Notice that when 'processData' is called, attributes for 'uuid', 'luw', and
// 'firmId' will be associated with each log message emitted during that
// function call.
//
// Next, we create a callback function that will be used to associate
// a hostname attribute to each log record for the lifetime of the process:
//..
    void loadHostnameAttribute(
        const ball::LoggerManager::AttributeVisitor& visitor)
    {
        char hostname[256];
        if (0!= gethostname(hostname, 256)) {
            bsl::strcpy(hostname, "failed.to.get.hostname");
        }
        visitor(ball::Attribute("mylibrary.hostname", hostname));
    }
//..
// Finally we demonstrate a function that registers the
// 'loadHostnameAttribute' with the logger manager:
//..
    int configureLibrary()
    {
        ball::LoggerManager::singleton().registerAttributeCollector(
              &loadHostnameAttribute, "mylibrary.hostnamecollector");

        // ...
    }
//..
// Notice that the attribute "mylibrary.hostname" will now be associated with
// every log message created (until "mylibrary.hostnamecollector" is unregistered
// or the logger manager is destroyed).
//..

    int main()
    {
        ball::LoggerManagerConfiguration configuration;
        configuration.setDefaultThresholdLevelsIfValid(ball::Severity::e_TRACE);
        ball::LoggerManagerScopedGuard guard(configuration);
        bslma::Allocator *alloc = bslma::Default::globalAllocator(0);

        bsl::shared_ptr<ball::StreamObserver> observer =
            bsl::allocate_shared<ball::StreamObserver>(alloc, &bsl::cout);
        ball::LoggerManager::singleton().registerObserver(observer, "default");
        observer->setRecordFormatFunctor(ball::RecordStringFormatter(
                                          "\n%d %p:%t %s %a %m\n"));
        configureLibrary();
        processData(3114, 834314, 4, "data message");
        return 0;
    }

